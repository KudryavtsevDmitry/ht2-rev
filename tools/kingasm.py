"""kingasm.py - finds the functions of king.masm and builds the assembly that is
actually assembled, with the functions rewritten in C++ taken out.

  py tools\\kingasm.py info LABEL...        what a function is made of, who uses
                                           it, whether it can be replaced
  py tools\\kingasm.py check                check the entries of src\\replace.txt
  py tools\\kingasm.py gen OUT [--asm SEL]  write king.masm without the functions
                                           of src\\replace.txt to OUT
  py tools\\kingasm.py procs [--out FILE]   rewrite the PROC/ENDP lines of
                                           king.masm: one PROC per function
  py tools\\kingasm.py stats                numbers about the functions found

LABEL is a label of king.masm, the old label of a renamed PROC (the one in
its comment), or an address in the original king.exe ($L_5e2040, L_5e2040,
FUN_5e2040, 5e2040, 0x5e2040). SEL is a comma separated
list of labels, addresses and groups of src\\replace.txt that stay in
assembly, or "all". check and gen take --replace FILE instead of
src\\replace.txt.

The PROC blocks of king.masm don't decide what a function is: the
disassembler's blocks split functions after calls it took for ones that don't
return and joined functions that follow each other, and the procs command
rewrites them from what is found here. The functions are found from the code
itself. A function starts at
  - every target of a call,
  - every code label data points at (vtables, function tables, exception
    tables), unless the instruction before it runs into it,
  - every code label the code takes the OFFSET of (callbacks),
  - the target of a jump across padding that nothing runs into (a tail
    call), and code that several functions jump into,
  - code that nothing reaches (functions nothing calls any more).
A function is everything reachable from its start through fall-through, jumps
and jump tables, up to the start of another function. Functions that can't
return (exit, throw) are found as well, so that a call to them ends the code
that follows it.
Only the standard library is used.
"""
import bisect
import os
import re
import sys
from collections import defaultdict

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
MASM = os.path.join(ROOT, "king.masm")
REPLACE = os.path.join(ROOT, "src", "replace.txt")
SRC = os.path.join(ROOT, "src")

# Functions that don't return although their code ends in ret, by their
# original labels. What only leads to them (AfxThrowMemoryException, ...) is
# found by the analysis.
NORETURN = {
    "fun_61d18e": "_CxxThrowException",     # ends in RaiseException
    "fun_61e8c3": "exit",                   # doexit, which returns for _cexit
    "fun_61e8d4": "_exit",
}
NORETURN_IMPORTS = ("__imp__ExitProcess@4", "__imp__ExitThread@4")


# ---------------------------------------------------------------------------
# Lines

BLANK, PROC, ENDP, LABEL, INSN, DATA, ALIGN, OTHER = range(8)

IDENT = re.compile(r"[A-Za-z_$@?][\w$@?]*")
SEGMENT = re.compile(r"^(_\w+)\s+(SEGMENT|ENDS)\b")
PROC_LINE = re.compile(r"^(\S+)\s+(PROC|ENDP)\b")
LABEL_LINE = re.compile(r"^(\S+)::\s*$")
DATA_DIRECTIVES = ("BYTE", "WORD", "DWORD", "QWORD", "DB", "DW", "DD", "DQ", "REAL4", "REAL8", "REAL10",
                   "TBYTE")
# a data label starts in the first column; one jump table has it indented
LABELED_DATA = re.compile(r"^(?:\s*(\$L_\w+|FUN_\w+)|([A-Za-z_$@?][\w$@?]*))\s+(%s)\b(.*)$"
                          % "|".join(DATA_DIRECTIVES))
PLAIN_DATA = re.compile(r"^\s+(%s)\b(.*)$" % "|".join(DATA_DIRECTIVES))
INSN_LINE = re.compile(r"^\s+([a-z][a-z0-9]*)\b\s*(.*)$")

RETURNS = ("ret", "retn", "iretd")
CONDITIONAL = frozenset(("ja", "jae", "jb", "jbe", "jc", "je", "jg", "jge", "jl", "jle", "jna", "jnae",
                         "jnb", "jnbe", "jnc", "jne", "jng", "jnge", "jnl", "jnle", "jno", "jnp", "jns",
                         "jnz", "jo", "jp", "jpe", "jpo", "js", "jz", "jcxz", "jecxz", "loop", "loope",
                         "loopne", "loopnz", "loopz"))
DATA_SIZE = {"BYTE": 1, "DB": 1, "WORD": 2, "DW": 2, "DWORD": 4, "DD": 4, "REAL4": 4, "QWORD": 8, "DQ": 8,
             "REAL8": 8, "REAL10": 10, "TBYTE": 10}


OLD_LABEL = re.compile(r";\s*((?:\$L_|FUN_)[0-9a-fA-F]{6})\b")


def label_address(name):
    """original address of a label named after it ($L_xxxxxx, FUN_xxxxxx)"""
    m = re.match(r"(?:\$l_|fun_)([0-9a-f]{6})$", name.lower())
    return int(m.group(1), 16) if m else None


def parse_address(text):
    """'5e2040', '0x5e2040' -> 0x5e2040"""
    m = re.fullmatch(r"(?:0x)?([0-9a-fA-F]{6})", text.strip())
    return int(m.group(1), 16) if m else None


def strip_comment(text):
    if ";" not in text:
        return text
    quote = None
    for n, ch in enumerate(text):
        if quote:
            if ch == quote:
                quote = None
        elif ch in "'\"":
            quote = ch
        elif ch == ";":
            return text[:n]
    return text


class Line:
    __slots__ = ("kind", "seg", "name", "mnem", "ops", "refs")

    def __init__(self, kind, seg, name=None, mnem=None, ops="", refs=()):
        self.kind = kind
        self.seg = seg          # "_TEXT", "_RDATA", "_DATA", "_BSS" or None
        self.name = name        # label defined on the line, lower case
        self.mnem = mnem        # instruction mnemonic or data directive
        self.ops = ops          # operands without the comment
        self.refs = refs        # labels used on the line, lower case


LEA_NOP = re.compile(r"(\w+),(?:DWORD PTR )?\[\1(?:\+0)?\]$")      # lea ECX,DWORD PTR [ECX]
MOV_NOP = re.compile(r"(\w+),\1$")                                    # mov EDI,EDI


def is_padding(line):
    """nop, int 3 and the instructions that do nothing (lea ECX,[ECX], mov
    EDI,EDI) between functions and before jump tables"""
    if line.kind != INSN:
        return False
    m = line.mnem
    return m == "nop" or (m == "int" and line.ops == "3") or \
        (m == "lea" and LEA_NOP.match(line.ops) is not None) or (m == "mov" and MOV_NOP.match(line.ops) is not None)


def is_neutral(line):
    """a line that is no code of a function: blank, padding, ALIGN"""
    if line.kind in (BLANK, ALIGN) or is_padding(line):
        return True
    return line.kind == DATA and line.seg == "_TEXT" and not line.name and not line.refs


# ---------------------------------------------------------------------------
# king.masm

class Masm:
    def __init__(self, path=MASM):
        self.path = path
        with open(path, "r", encoding="latin-1", newline="") as f:
            self.text = f.read().split("\n")
        self.lines = []
        self.defs = {}          # label -> line index
        self.spelling = {}      # label -> name as written
        # a PROC renamed by applySymbols.py (or by hand) keeps its old label
        # in a comment:  _fclose PROC ;FUN_61cb9a int fclose(FILE *stream)
        self.alias = {}         # old label -> label
        self.public = set()     # labels on PUBLIC lines
        self._parse()
        self._index()
        self._analyse()

    # -- parsing ------------------------------------------------------------

    def _define(self, name, i):
        key = name.lower()
        if key in self.defs:
            raise SystemExit("%s(%d): %s is defined twice" % (self.path, i + 1, name))
        self.defs[key] = i
        self.spelling[key] = name
        return key

    def _parse(self):
        seg = None
        pending = []            # lines whose names are resolved once all labels are known
        lines = self.lines
        for i, raw in enumerate(self.text):
            s = strip_comment(raw).rstrip()
            if not s.strip():
                lines.append(Line(BLANK, seg))
                continue
            if s[0] in " \t":
                m = INSN_LINE.match(s)
                if m and seg == "_TEXT" and m.group(1).upper() not in DATA_DIRECTIVES and m.group(1) != "align":
                    lines.append(Line(INSN, seg, None, m.group(1).lower(), m.group(2).strip()))
                    pending.append(i)
                    continue
            m = SEGMENT.match(s)
            if m:
                seg = m.group(1) if m.group(2) == "SEGMENT" else None
                lines.append(Line(OTHER, None))
                continue
            m = PROC_LINE.match(s)
            if m and seg == "_TEXT":
                if m.group(2) == "PROC":
                    key = self._define(m.group(1), i)
                    lines.append(Line(PROC, seg, key))
                    old = OLD_LABEL.match(raw[len(s):].strip())
                    if old and label_address(key) is None:
                        self.alias[old.group(1).lower()] = key
                else:
                    lines.append(Line(ENDP, seg, m.group(1).lower()))
                continue
            m = LABEL_LINE.match(s)
            if m:
                lines.append(Line(LABEL, seg, self._define(m.group(1), i)))
                continue
            if re.match(r"^\s*ALIGN\b", s, re.I):
                lines.append(Line(ALIGN, seg))
                continue
            m = LABELED_DATA.match(s)
            if m and (m.group(1) or m.group(2)).upper() not in DATA_DIRECTIVES:
                lines.append(Line(DATA, seg, self._define(m.group(1) or m.group(2), i), m.group(3).upper(),
                                  m.group(4).strip()))
                pending.append(i)
                continue
            m = PLAIN_DATA.match(s)
            if m:
                lines.append(Line(DATA, seg, None, m.group(1).upper(), m.group(2).strip()))
                pending.append(i)
                continue
            if s.startswith("PUBLIC"):
                self.public.update(n.strip().lower() for n in s[6:].split(","))
            lines.append(Line(OTHER, seg, None, None, s))
        defs = self.defs
        for i in pending:
            line = lines[i]
            ops = line.ops
            if "'" in ops:
                ops = re.sub(r"'[^']*'", "", ops)
            refs = [n.lower() for n in IDENT.findall(ops)]
            line.refs = tuple(n for n in refs if n in defs)

    def _index(self):
        lines = self.lines
        # tables in the code: a data label, or a code label the rows of a table follow
        self.tables = set()
        for k, i in self.defs.items():
            line = lines[i]
            if line.seg != "_TEXT":
                continue
            if line.kind == DATA:
                self.tables.add(k)
            elif line.kind == LABEL:
                j = i + 1
                while j < len(lines) and lines[j].kind == BLANK:
                    j += 1
                if j < len(lines) and lines[j].kind == DATA and lines[j].refs:
                    self.tables.add(k)
        self.code = set(k for k, i in self.defs.items()
                        if lines[i].seg == "_TEXT" and lines[i].kind in (PROC, LABEL) and k not in self.tables)
        # who uses a label: label -> line indices
        self.users = defaultdict(list)
        for i, line in enumerate(lines):
            for r in line.refs:
                self.users[r].append(i)
        # PROC blocks: PROC line -> ENDP line
        self.blocks = {}
        open_proc = None
        for i, line in enumerate(lines):
            if line.kind == PROC:
                open_proc = i
            elif line.kind == ENDP and open_proc is not None:
                self.blocks[open_proc] = i
                open_proc = None
        # a catch block returns the address where its function continues:
        #   mov EAX,OFFSET <continuation>
        #   ret
        self.continuations = {}     # line of the mov -> continuation label
        for i, line in enumerate(lines):
            if line.kind == INSN and line.mnem == "mov" and line.ops.upper().startswith("EAX,OFFSET") \
                    and line.refs and line.refs[0] in self.code:
                j = self.next_insn(i)
                if j is not None and lines[j].mnem in RETURNS:
                    self.continuations[i] = line.refs[0]
        # C++ exception handling. A function with objects to destroy or a try
        # block registers a handler thunk (push OFFSET <thunk>):
        #   <thunk>: mov EAX,OFFSET <FuncInfo>
        #            jmp __CxxFrameHandler
        # and FuncInfo leads to the code that runs for the function when an
        # exception passes: unwind actions (destructors) and catch blocks.
        self.thunks = {}            # thunk -> FuncInfo
        self.funcinfo = {}          # FuncInfo -> (unwind actions, catch blocks)
        for key in self.code:
            j = self.next_insn(self.defs[key])
            if j is None or lines[j].mnem != "mov" or not lines[j].ops.upper().startswith("EAX,OFFSET"):
                continue
            k = self.next_insn(j)
            info = [r for r in lines[j].refs if r not in self.code]
            if k is None or lines[k].mnem != "jmp" or not info:
                continue
            parsed = self.parse_funcinfo(info[0])
            if parsed:
                self.thunks[key] = info[0]
                self.funcinfo[info[0]] = parsed

    # -- data ---------------------------------------------------------------

    def data_values(self, line):
        """the bytes of a data line: ints, (label, n) for byte n of a pointer,
        None where not known"""
        size = DATA_SIZE.get(line.mnem, 1)
        out = []
        for item in re.findall(r"'[^']*'|[^,]+", line.ops):
            item = item.strip()
            if item.startswith("'"):
                out.extend(ord(c) for c in item[1:-1])
                continue
            count = 1
            m = re.match(r"(\d+)\s+DUP\s*\((.*)\)$", item, re.I)
            if m:
                count, item = int(m.group(1)), m.group(2).strip()
            value = None
            if re.fullmatch(r"[0-9][0-9a-fA-F]*[hH]", item):
                value = int(item[:-1], 16)
            elif re.fullmatch(r"-?\d+", item):
                value = int(item)
            key = item.lower()
            if key in self.defs:
                one = [(key, n) for n in range(size)]
            elif value is not None:
                value &= (1 << (8 * size)) - 1
                one = [(value >> (8 * n)) & 0xff for n in range(size)]
            else:
                one = [None] * size
            out.extend(one * count)
        return out

    def read_data(self, key, count):
        """count bytes from a data label on, across other labels"""
        out = []
        lines = self.lines
        i = self.defs[key]
        while len(out) < count and i < len(lines):
            line = lines[i]
            if line.kind == DATA:
                out.extend(self.data_values(line))
            elif line.kind != BLANK:
                break
            i += 1
        return out[:count]

    @staticmethod
    def u32(data, offset):
        """int, or the label of a pointer, or None"""
        part = data[offset:offset + 4]
        if len(part) < 4:
            return None
        if all(isinstance(b, int) for b in part):
            return part[0] | part[1] << 8 | part[2] << 16 | part[3] << 24
        if isinstance(part[0], tuple) and all(isinstance(b, tuple) and b[0] == part[0][0] for b in part):
            return part[0][0]
        return None

    def parse_funcinfo(self, key):
        """FuncInfo -> (unwind action labels, catch block labels), or None"""
        if self.lines[self.defs[key]].kind != DATA:
            return None
        d = self.read_data(key, 20)
        if self.u32(d, 0) not in (0x19930520, 0x19930521, 0x19930522):
            return None
        max_state, unwind, ntry, trymap = self.u32(d, 4), self.u32(d, 8), self.u32(d, 12), self.u32(d, 16)
        actions, catches = [], []
        if isinstance(max_state, int) and isinstance(unwind, str) and 0 < max_state < 10000:
            u = self.read_data(unwind, 8 * max_state)
            for n in range(max_state):
                a = self.u32(u, 8 * n + 4)
                if isinstance(a, str):
                    actions.append(a)
        if isinstance(ntry, int) and isinstance(trymap, str) and 0 < ntry < 10000:
            t = self.read_data(trymap, 20 * ntry)
            for n in range(ntry):
                ncatch, handlers = self.u32(t, 20 * n + 12), self.u32(t, 20 * n + 16)
                if isinstance(ncatch, int) and isinstance(handlers, str) and 0 < ncatch < 1000:
                    h = self.read_data(handlers, 16 * ncatch)
                    for c in range(ncatch):
                        a = self.u32(h, 16 * c + 12)
                        if isinstance(a, str):
                            catches.append(a)
        return actions, catches

    # -- helpers ------------------------------------------------------------

    def name(self, key):
        return self.spelling.get(key, key)

    def lookup(self, text):
        """label, old label of a renamed PROC, or address -> label"""
        key = text.strip().lower()
        if key.startswith("l_"):
            key = "$" + key         # nmake can't pass a $
        if key in self.defs:
            return key
        if key in self.alias:
            return self.alias[key]
        addr = parse_address(text)
        if addr is not None:
            for prefix in ("fun_", "$l_"):
                k = "%s%06x" % (prefix, addr)
                if k in self.defs:
                    return k
                if k in self.alias:
                    return self.alias[k]
        return None

    def address(self, key):
        """original address of a label, also of a renamed PROC"""
        a = label_address(key)
        if a is None:
            for old, new in self.alias.items():
                if new == key:
                    return label_address(old)
        return a

    def next_insn(self, i):
        """the next instruction after line i if only blank lines and labels are between"""
        lines = self.lines
        n = len(lines)
        i += 1
        while i < n and lines[i].kind in (BLANK, LABEL):
            i += 1
        return i if i < n and lines[i].kind == INSN else None

    def prev_insn(self, i):
        """the last real instruction before line i if only labels, blank lines,
        PROC/ENDP lines and nops are between"""
        lines = self.lines
        i -= 1
        while i >= 0:
            line = lines[i]
            if line.kind in (BLANK, LABEL, PROC, ENDP) or (line.kind == INSN and line.mnem == "nop"):
                i -= 1
                continue
            return i if line.kind == INSN else None
        return None

    def ends_flow(self, i):
        """True if the instruction on line i doesn't continue with the next one"""
        line = self.lines[i]
        m = line.mnem
        if m in RETURNS or m == "jmp" or (m == "int" and line.ops == "3"):
            return True
        if m == "call":
            return self.calls_noreturn(line)
        return False

    def calls_noreturn(self, line):
        if any(r in self.noreturn for r in line.refs):
            return True
        return any(imp in line.ops for imp in NORETURN_IMPORTS)

    def crosses_gap(self, a, b):
        """True if padding or ALIGN is between lines a and b: the compiler
        separates functions with it (and doesn't make tail calls without
        optimizing, which is when it doesn't align functions)"""
        lines = self.lines
        for i in range(min(a, b) + 1, max(a, b)):
            line = lines[i]
            if line.kind == ALIGN or is_padding(line) or \
                    (line.kind == DATA and line.mnem == "BYTE" and line.ops in ("090H", "0ccH")):
                return True
        return False

    def runs_into(self, key):
        """True if the code before label key continues into it"""
        p = self.prev_insn(self.defs[key])
        return p is not None and not self.ends_flow(p)

    def table_lines(self, key):
        """a table in the code (jump table, index table): its lines"""
        lines = self.lines
        i = self.defs[key]
        out = [i]
        if lines[i].kind == LABEL:
            i += 1
            while lines[i].kind == BLANK:
                out.append(i)
                i += 1
            out.append(i)
        directive = lines[i].mnem
        i += 1
        while i < len(lines):
            line = lines[i]
            if line.kind == BLANK:
                i += 1
                continue
            if line.kind != DATA or line.mnem != directive:
                break
            # an index table ends where the next table starts
            if line.name and directive == "BYTE" and not line.ops.startswith("("):
                break
            out.append(i)
            i += 1
        return out

    @staticmethod
    def data_size(line):
        size = DATA_SIZE.get(line.mnem, 0)
        total = 0
        for item in re.findall(r"'[^']*'|[^,]+", line.ops):
            item = item.strip()
            if item.startswith("'"):
                total += len(item) - 2
                continue
            m = re.match(r"(\d+)\s+DUP\s*\(", item, re.I)
            total += size * (int(m.group(1)) if m else 1)
        return total

    def data_position(self, i):
        """data line -> (label, byte offset) of the nearest label before it
        that something points at"""
        lines = self.lines
        offset = 0
        j = i
        while j >= 0:
            line = lines[j]
            if line.kind == DATA:
                if j != i:
                    offset += self.data_size(line)
                if line.name and self.users.get(line.name):
                    return line.name, offset
            elif line.kind not in (BLANK, ALIGN):
                return None, offset
            j -= 1
        return None, offset

    # -- functions ----------------------------------------------------------

    def _find_entries(self):
        lines = self.lines
        code = self.code
        entries = {}            # label -> why it starts a function
        interior = {}           # label something points at that is inside a function
        for key in code:
            i = self.defs[key]
            if lines[i].kind == PROC and label_address(key) is None and key not in self.renamed:
                entries[key] = "named"          # __EntryPoint
        for key, users in self.users.items():
            if key not in code:
                continue
            why = None
            for u in users:
                line = lines[u]
                if line.seg != "_TEXT":
                    why = why or "data"
                    continue
                if line.kind != INSN:
                    continue                    # jump table entry
                if line.mnem == "call":
                    why = "call"
                    break
                if u in self.continuations:
                    continue
                if "OFFSET" in line.ops.upper():
                    why = why or "offset"
                elif line.mnem == "jmp" and "[" not in line.ops and self.crosses_gap(u, self.defs[key]):
                    why = why or "jmp"
            if why is None:
                continue
            if why != "call" and self.runs_into(key):
                interior[key] = why
                continue
            entries[key] = why
        for key, why in self.extra.items():
            entries.setdefault(key, why)
        self.entries = entries
        self.interior = interior

    def _walk(self, work, owned, exits, notes):
        """follows the code from the lines in work; -> (whether it returns,
        exception handler thunk the code registers)"""
        lines = self.lines
        defs = self.defs
        entries = self.entries
        code = self.code
        tables = self.tables
        users = self.users
        thunks = self.thunks
        n = len(lines)
        returns = False
        thunk = None
        while work:
            i = work.pop()
            first = True
            dead = False            # after a call that doesn't return
            while i < n:
                if i in owned:
                    break
                line = lines[i]
                k = line.kind
                if k == LABEL or k == PROC:
                    # dead code after a call that doesn't return stays with the
                    # function up to a label something uses
                    if not first and (line.name in entries or dead) and \
                            not (dead and not users.get(line.name) and entries.get(line.name, "unused") == "unused"):
                        if not dead:
                            exits.add(line.name)
                            notes.append(("runs into", line.name, i))
                        break
                    owned.add(i)
                    first = False
                    i += 1
                    continue
                first = False
                if k == BLANK or k == ENDP:
                    owned.add(i)
                    i += 1
                    continue
                if k != INSN or (dead and is_padding(line)):
                    if not dead:
                        notes.append(("runs out", None, i))
                    break
                owned.add(i)
                m = line.mnem
                if m in RETURNS:
                    returns = returns or not dead
                    break
                if m == "int" and line.ops == "3":
                    break
                if line.refs:
                    for r in line.refs:
                        if r in tables:
                            for j in self.table_lines(r):
                                if j not in owned:
                                    owned.add(j)
                                    if lines[j].mnem == "DWORD":
                                        work.extend(defs[x] for x in lines[j].refs if x in code)
                    if m == "push" and line.refs[0] in thunks:
                        thunk = thunk or line.refs[0]
                if m == "jmp":
                    targets = [r for r in line.refs if r in code]
                    if targets and "[" not in line.ops:
                        t = targets[0]
                        if t in entries:
                            exits.add(t)
                        else:
                            work.append(defs[t])
                    elif not dead:
                        returns = True      # through a pointer: assume it returns
                    break
                if m in CONDITIONAL:
                    for t in line.refs:
                        if t in code:
                            if t in entries:
                                exits.add(t)
                            else:
                                work.append(defs[t])
                elif m == "call" and not dead and self.calls_noreturn(line):
                    dead = True
                i += 1
        return returns, thunk

    def _trace(self, entry):
        """the function starting at entry -> (its lines, labels it ends in by a
        tail call or by running into them, whether it can return, notes,
        exception handling)"""
        owned, exits, notes = set(), set(), []
        returns, thunk = self._walk([self.defs[entry]], owned, exits, notes)
        eh = None
        if thunk:
            # the code a catch block continues with is part of the function
            fi = self.thunks[thunk]
            actions, catches = self.funcinfo[fi]
            eh = (thunk, fi, actions, catches)
            starts = []
            for c in catches:
                if c in self.entries and c != entry:
                    c_owned = set()
                    self._walk([self.defs[c]], c_owned, set(), [])
                    starts.extend(self.defs[self.continuations[i]] for i in c_owned if i in self.continuations)
            if starts:
                more, _ = self._walk(starts, owned, exits, notes)
                returns = returns or more
        return owned, exits, returns, notes, eh

    def _retrace(self, keys):
        """traces the functions keys again (those that are no longer
        functions are dropped)"""
        owner = self.owner
        for e in keys:
            old = self.functions.pop(e, None)
            if old is not None:
                for i in old:
                    owners = owner[i]
                    owners.remove(e)
                    if not owners:
                        del owner[i]
                for x in self.exits.pop(e):
                    self.exited_by[x].discard(e)
                eh = self.eh.pop(e, None)
                if eh:
                    for x in [eh[0]] + eh[2] + eh[3]:
                        if self.eh_parent.get(x) == e:
                            del self.eh_parent[x]
                del self.direct_returns[e], self.notes[e]
            if e not in self.entries:
                continue
            owned, exits, returns, notes, eh = self._trace(e)
            self.functions[e] = owned
            self.exits[e] = exits
            for x in exits:
                self.exited_by[x].add(e)
            self.direct_returns[e] = returns
            self.notes[e] = notes
            if eh:
                self.eh[e] = eh
                for x in [eh[0]] + eh[2] + eh[3]:
                    self.eh_parent.setdefault(x, e)
            for i in owned:
                owner[i].append(e)

    def predecessors(self, i):
        """lines the code at label line i is reached from: jumps, jump table
        rows, and the instruction before it if that runs into it"""
        out = []
        lines = self.lines
        for u in self.users.get(lines[i].name, ()):
            line = lines[u]
            if (line.kind == INSN and (line.mnem == "jmp" or line.mnem in CONDITIONAL)) or \
                    (line.kind == DATA and line.seg == "_TEXT"):
                out.append(u)
        p = self.prev_insn(i)
        if p is not None and not self.ends_flow(p):
            out.append(p)
        return out

    def _propagate_returns(self):
        """a function returns if it reaches a ret or ends in a function that returns"""
        returns = set(e for e, r in self.direct_returns.items() if r)
        work = list(returns)
        while work:
            x = work.pop()
            for e in self.exited_by.get(x, ()):
                if e not in returns:
                    returns.add(e)
                    work.append(e)
        self.returns = returns

    def _update_entries(self):
        """finds the entries again -> functions to trace again"""
        old = self.entries
        self._find_entries()
        new = self.entries
        affected = set()
        for x in set(old) | set(new):
            if old.get(x) == new.get(x):
                continue
            affected.add(x)
            # functions that ran through it, or stopped at it
            affected.update(self.owner.get(self.defs[x], ()))
            affected.update(self.exited_by.get(x, ()))
        # a changed catch block changes the function it belongs to
        affected.update(self.eh_parent[x] for x in list(affected) if x in self.eh_parent)
        return affected

    def _analyse(self):
        self.renamed = set(self.alias.values())
        self.noreturn = set(self.lookup(k) for k in NORETURN if self.lookup(k))
        self.extra = {}
        self.functions, self.exits, self.direct_returns, self.notes = {}, {}, {}, {}
        self.owner = defaultdict(list)          # line -> functions it belongs to
        self.exited_by = defaultdict(set)       # label -> functions ending in it
        self.eh = {}                            # function -> (thunk, FuncInfo, unwind actions, catch blocks)
        self.eh_parent = {}                     # thunk, unwind action, catch block -> function
        lines = self.lines
        self.entries = {}
        self._find_entries()
        self._retrace(list(self.entries))
        self._propagate_returns()
        for self.rounds in range(1, 1000):
            # functions that can't return: the code after calls to them is dead
            new = set(e for e in self.entries if e not in self.returns) - self.noreturn
            if new:
                self.noreturn |= new
                affected = set()
                for x in new:
                    for u in self.users.get(x, ()):
                        if lines[u].kind == INSN and lines[u].mnem == "call":
                            affected.update(self.owner.get(u, ()))
                affected |= self._update_entries()
                self._retrace(affected)
                self._propagate_returns()
                continue
            changed = False
            # unused code that a function reaches after all, as its dead end or
            # by a jump; by a function that starts before it (two pieces that
            # jump into each other belong to the first)
            for key, why in list(self.extra.items()):
                if why != "unused":
                    continue
                at = self.defs[key]
                if any(o != key and self.defs[o] < at for o in self.owner.get(at, ())) or any(
                        lines[u].kind == INSN and (lines[u].mnem == "jmp" or lines[u].mnem in CONDITIONAL)
                        and any(o != key and self.defs[o] < at for o in self.owner.get(u, ()))
                        for u in self.users.get(key, ())):
                    del self.extra[key]
                    changed = True
            # a label data points at after code no function owns (the CRT's
            # "VC20XC00" before _except_handler3 disassembles as instructions)
            for key, why in self.interior.items():
                p = self.prev_insn(self.defs[key])
                if p is not None and p not in self.owner and key not in self.extra:
                    self.extra[key] = why
                    changed = True
            # code several functions reach is a function of its own, starting
            # where they enter it; and stops being one when only one function
            # reaches it any more
            for key, why in list(self.extra.items()):
                if why == "shared":
                    reach = set()
                    for p in self.predecessors(self.defs[key]):
                        reach.update(o for o in self.owner.get(p, ()) if o != key)
                    if len(reach) <= 1:
                        del self.extra[key]
                        changed = True
            for i, owners in list(self.owner.items()):
                if len(owners) > 1 and lines[i].kind in (LABEL, PROC) and lines[i].name not in self.entries:
                    here = set(owners)
                    if any(set(self.owner.get(p, ())) != here for p in self.predecessors(i)):
                        self.extra[lines[i].name] = "shared"
                        changed = True
            # code no function reaches: functions nobody uses any more; one
            # label for each piece, the rest may be reached from there
            piece = False
            for i, line in enumerate(lines):
                if line.kind == INSN and not is_padding(line):
                    if i in self.owner:
                        piece = False
                        continue
                    if piece:
                        continue
                    piece = True
                    j = i - 1
                    while lines[j].kind == BLANK:
                        j -= 1
                    if lines[j].kind in (LABEL, PROC) and lines[j].name not in self.entries:
                        self.extra[lines[j].name] = "unused"
                        changed = True
            if not changed:
                break
            self._retrace(self._update_entries())
            self._propagate_returns()

    # -- what a function is made of -----------------------------------------

    def function_of(self, key):
        """entry of the function a label is in"""
        if key in self.entries:
            return key
        owners = self.owner.get(self.defs[key], [])
        return owners[0] if len(owners) == 1 else None

    def callers(self, e):
        out = []
        for u in self.users.get(e, ()):
            line = self.lines[u]
            if line.kind == INSN and line.mnem == "call":
                out.append(u)
        return out

    def data_users(self, e):
        """data lines pointing at the function -> [(table label, offset)]"""
        out = []
        for u in self.users.get(e, ()):
            if self.lines[u].seg != "_TEXT":
                out.append(self.data_position(u))
        return out

    def stack_bytes(self, e):
        """set of N of the function's ret N"""
        out = set()
        for i in self.functions[e]:
            line = self.lines[i]
            if line.kind == INSN and line.mnem in RETURNS:
                out.add(int(line.ops, 0) if line.ops else 0)
        return out

    def eh_frame(self, e):
        """C++ exception handling of a function: (handler thunk, FuncInfo,
        unwind actions, catch blocks) or None"""
        return self.eh.get(e)

    def has_seh(self, e):
        for i in self.functions[e]:
            line = self.lines[i]
            if line.kind == INSN and "FS:[0]" in line.ops.upper().replace(" ", ""):
                return True
        return False

    def catch_blocks(self, e):
        """where the catch blocks of a function continue: [(line of the
        mov EAX,OFFSET in the catch block, continuation label)]"""
        eh = self.eh.get(e)
        if not eh:
            return []
        out = []
        for c in eh[3]:
            for i in self.functions.get(c, ()):
                if i in self.continuations:
                    out.append((i, self.continuations[i]))
        return sorted(set(out))

    def outside_users(self, e):
        """lines outside the function that use a label inside it (other than
        its entry): [(line, label)]"""
        owned = self.functions[e]
        out = []
        for i in owned:
            line = self.lines[i]
            if line.name and line.name != e:
                for u in self.users.get(line.name, ()):
                    if u not in owned:
                        out.append((u, line.name))
        return sorted(out)

    def shared_lines(self, e):
        return sorted(i for i in self.functions[e] if len(self.owner[i]) > 1)

    def span(self, e):
        owned = self.functions[e]
        return min(owned), max(owned)

    def code_lines(self, e):
        """lines of the function that hold code, labels or data"""
        return sorted(i for i in self.functions[e] if self.lines[i].kind in (INSN, LABEL, PROC, DATA))


# ---------------------------------------------------------------------------
# src\replace.txt

class Replacement:
    def __init__(self, label, symbol, group, lineno):
        self.label = label          # as written
        self.symbol = symbol        # C++ symbol as the linker sees it
        self.group = group
        self.lineno = lineno
        self.key = None             # label in king.masm

    def stack_bytes(self):
        """bytes of arguments the C++ function removes from the stack"""
        m = re.fullmatch(r"@\w+@(\d+)", self.symbol)       # __fastcall
        if m:
            return max(int(m.group(1)) - 8, 0)
        m = re.fullmatch(r"_\w+@(\d+)", self.symbol)       # __stdcall
        if m:
            return int(m.group(1))
        return 0                                            # __cdecl


def read_replace(path=REPLACE):
    out = []
    if not os.path.exists(path):
        return out
    with open(path, encoding="utf-8") as f:
        for n, raw in enumerate(f, 1):
            s = raw.split("#", 1)[0].strip()
            if not s:
                continue
            parts = s.split()
            if len(parts) not in (2, 3):
                raise SystemExit("%s(%d): expected: label symbol [group]" % (path, n))
            out.append(Replacement(parts[0], parts[1], parts[2] if len(parts) == 3 else "", n))
    return out


BINDING = re.compile(r"\bASM_PROC\(\s*([\w$]+)\s*\)"                 # ASM_PROC(label)
                     r"|\bASM_VAR\([^,]*,[^,]*,\s*([\w$]+)\s*\)"        # ASM_VAR(type, name, label)
                     r"|\bASM_SYM\(\s*\w+\s*,\s*\"([^\"]+)\"\s*\)")       # ASM_SYM(name, "symbol")


def asm_bindings():
    """labels the C++ code calls or reads through ASM_PROC/ASM_VAR/ASM_SYM"""
    out = {}
    for name in sorted(os.listdir(SRC)):
        if not name.endswith((".h", ".cpp")):
            continue
        path = os.path.join(SRC, name)
        with open(path, encoding="utf-8", errors="replace") as f:
            for n, raw in enumerate(f, 1):
                if raw.lstrip().startswith(("#define", "//")):
                    continue
                for m in BINDING.finditer(raw):
                    label = m.group(1) or m.group(2) or m.group(3)
                    out[label.lower()] = "%s(%d)" % (os.path.relpath(path, ROOT), n)
    return out


# ---------------------------------------------------------------------------
# checks

def check_replacement(masm, r, bindings):
    """-> (errors, warnings)"""
    errors, warnings = [], []
    key = masm.lookup(r.label)
    if key is None:
        return ["%s is not a label of king.masm" % r.label], warnings
    r.key = key
    if key not in masm.entries:
        f = masm.function_of(key)
        where = " (inside %s)" % masm.name(f) if f else ""
        return ["%s is not the start of a function%s" % (masm.name(key), where)], warnings
    name = masm.name(key)
    for u, label in masm.outside_users(key):
        if u in masm.continuations:
            continue            # catch block of the function: goes with it
        f = masm.owner.get(u)
        who = masm.name(f[0]) if f else "data" if masm.lines[u].seg != "_TEXT" else "code outside functions"
        errors.append("king.masm(%d) in %s uses %s inside %s" % (u + 1, who, masm.name(label), name))
    shared = masm.shared_lines(key)
    if shared:
        others = sorted(set(o for i in shared for o in masm.owner[i] if o != key))
        errors.append("%s shares code with %s (king.masm(%d))" % (name, ", ".join(masm.name(o) for o in others),
                                                                shared[0] + 1))
    for kind, label, i in masm.notes[key]:
        if kind == "runs into":
            errors.append("%s runs into %s at king.masm(%d); replace them together" % (name, masm.name(label), i + 1))
        elif kind == "runs out":
            errors.append("%s runs into padding or data at king.masm(%d)" % (name, i + 1))
    rets = masm.stack_bytes(key)
    want = r.stack_bytes()
    if len(rets) > 1:
        errors.append("%s returns with different stack sizes: %s" % (name, ", ".join("ret %d" % x for x in sorted(rets))))
    elif rets and want not in rets:
        got = rets.pop()
        errors.append("%s ends in ret %d (%d bytes of arguments), %s removes %d" % (name, got, got, r.symbol, want))
    if key in masm.noreturn:
        warnings.append("%s doesn't return" % name)
    eh = masm.eh_frame(key)
    if eh and eh[3]:
        warnings.append("%s has catch blocks (%s); the C++ code is built without exceptions"
                        % (name, ", ".join(masm.name(c) for c in eh[3])))
    elif masm.has_seh(key) and not eh:
        warnings.append("%s sets up an exception frame (FS:[0]) that isn't C++ exception handling" % name)
    if key in bindings:
        errors.append("%s is used by the C++ code through %s; call the C++ function instead" % (name, bindings[key]))
    return errors, warnings


# ---------------------------------------------------------------------------
# commands

def cmd_info(masm, args):
    replace = {}
    for r in read_replace():
        k = masm.lookup(r.label)
        if k:
            replace[k] = r
    bindings = asm_bindings()
    for text in args:
        key = masm.lookup(text)
        if key is None:
            print("%s: not a label of king.masm" % text)
            continue
        e = masm.function_of(key)
        if e is None:
            print("%s: not in a function" % masm.name(key))
            continue
        if e != key:
            print("%s is inside %s" % (masm.name(key), masm.name(e)))
        name = masm.name(e)
        lo, hi = masm.span(e)
        print("%s  king.masm(%d-%d), %d lines of code" % (name, lo + 1, hi + 1, len(masm.code_lines(e))))
        procs = [i for i in sorted(masm.functions[e]) if masm.lines[i].kind == PROC]
        print("  PROC blocks: %s" % ", ".join(masm.name(masm.lines[i].name) for i in procs))
        print("  starts a function because of: %s" % masm.entries[e])
        rets = sorted(masm.stack_bytes(e))
        if e in masm.noreturn:
            print("  doesn't return")
        elif rets:
            n = rets[0]
            print("  ret %d: thiscall -> extern \"C\" __fastcall with %d bytes of parameters (@Name@%d), "
                  "stdcall -> _Name@%d%s" % (n, n + 8, n + 8, n, ", or cdecl" if n == 0 else ""))
        callers = masm.callers(e)
        if callers:
            fs = sorted(set(masm.name(masm.owner[u][0]) if masm.owner.get(u) else "?" for u in callers))
            print("  called from %d places in: %s" % (len(callers), ", ".join(fs[:12]) + (" ..." if len(fs) > 12 else "")))
        for table, offset in masm.data_users(e):
            print("  in data: %s+%d" % (masm.name(table) if table else "?", offset))
        calls = defaultdict(int)
        for i in masm.functions[e]:
            line = masm.lines[i]
            if line.kind == INSN and line.mnem == "call":
                for r in line.refs:
                    calls[masm.name(r)] += 1
        if calls:
            print("  calls: %s" % ", ".join("%s%s" % (c, " x%d" % calls[c] if calls[c] > 1 else "") for c in sorted(calls)))
        tails = sorted(masm.exits[e])
        if tails:
            print("  ends in: %s" % ", ".join(masm.name(t) for t in tails))
        eh = masm.eh_frame(e)
        if eh:
            print("  C++ exception handling: handler %s, FuncInfo %s, %d unwind actions, catch blocks: %s"
                  % (masm.name(eh[0]), masm.name(eh[1]), len(eh[2]), ", ".join(masm.name(c) for c in eh[3]) or "none"))
        elif masm.has_seh(e):
            print("  sets up an exception frame (FS:[0])")
        r = replace.get(e)
        if r:
            print("  replaced by %s (src\\replace.txt line %d)" % (r.symbol, r.lineno))
        else:
            r = Replacement(name, "@x@%d" % ((rets[0] if rets else 0) + 8), "", 0)
        errors, warnings = check_replacement(masm, r, bindings)
        if r.lineno == 0:
            errors = [x for x in errors if " removes " not in x]
        for w in warnings:
            print("  note: " + w)
        if errors:
            print("  can't be replaced as it is:")
            for x in errors:
                print("    " + x)
        else:
            print("  can be replaced")
    return 0


def select(masm, replacements, spec):
    """--asm SEL -> labels that stay in assembly"""
    keep = set()
    if not spec:
        return keep
    groups = defaultdict(set)
    for r in replacements:
        if r.key:
            groups[r.group.lower()].add(r.key)
    for item in spec.split(","):
        item = item.strip()
        if not item:
            continue
        if item.lower() == "all":
            keep.update(r.key for r in replacements)
        elif item.lower() in groups:
            keep.update(groups[item.lower()])
        else:
            key = masm.lookup(item)
            if key is None:
                raise SystemExit("--asm: %s is neither a label nor a group of src\\replace.txt" % item)
            keep.add(key)
    return keep


def check_bindings(masm, bindings):
    """the labels the C++ code binds with ASM_PROC/ASM_VAR must be public
    labels of king.masm, or linking fails -> errors"""
    errors = []
    for key, where in sorted(bindings.items(), key=lambda kv: kv[1]):
        if key not in masm.defs:
            new = masm.lookup(key)
            spelled = re.sub(r"^fun_", "FUN_", re.sub(r"^\$l_", "$L_", key))
            errors.append("%s: %s isn't a label of king.masm%s" % (
                where, spelled, " any more, it is %s now" % masm.name(new) if new else ""))
        elif masm.lines[masm.defs[key]].kind != PROC and key not in masm.public:
            errors.append("%s: %s is neither a PROC nor in a PUBLIC line of king.masm" % (where, masm.name(key)))
    return errors


def run_checks(masm, replacements, path=REPLACE, cpp=True):
    bindings = asm_bindings()
    failed = False
    if cpp:
        for x in check_bindings(masm, bindings):
            print("error: " + x)
            failed = True
    seen = {}
    for r in replacements:
        errors, warnings = check_replacement(masm, r, bindings)
        if r.key in seen:
            errors.append("%s is already replaced in line %d" % (r.label, seen[r.key]))
        seen[r.key] = r.lineno
        where = "%s(%d)" % (os.path.relpath(path, ROOT), r.lineno)
        for w in warnings:
            print("%s: note: %s" % (where, w))
        for x in errors:
            print("%s: error: %s" % (where, x))
            failed = True
    return not failed


def cmd_check(masm, args):
    replacements = read_replace(args[1] if args[:1] == ["--replace"] else REPLACE)
    ok = run_checks(masm, replacements, args[1] if args[:1] == ["--replace"] else REPLACE)
    print("%d replacements, %s" % (len(replacements), "ok" if ok else "errors"))
    return 0 if ok else 1


def removal(masm, keys):
    """what taking the functions keys out of king.masm means for its lines ->
    (lines to drop, PROC lines that become public labels, labels that stay,
    at an int 3, because catch blocks of the functions return to them)"""
    lines = masm.lines
    drop = set()
    for e in keys:
        drop |= masm.functions[e]
    gone = set(lines[i].name for i in drop if lines[i].name)
    # a PROC block that loses code loses its PROC and ENDP lines; a PROC
    # label that stays becomes a public label
    relabel = set()
    for p, q in masm.blocks.items():
        if any(i in drop for i in range(p, q + 1)):
            if all(i in drop or is_neutral(lines[i]) for i in range(p + 1, q)):
                drop.update(range(p, q + 1))        # with its padding
                continue
            drop.add(q)
            if lines[p].name not in gone:
                relabel.add(p)
    stubs = set()
    for i in drop:
        name = lines[i].name
        if name and name not in keys and lines[i].kind != ENDP:
            if any(u not in drop for u in masm.users.get(name, ())):
                stubs.add(name)
    return drop, relabel, stubs


def cmd_gen(masm, args):
    out, spec, path = None, "", REPLACE
    it = iter(args)
    for a in it:
        if a == "--asm":
            spec = next(it, "")
        elif a.startswith("--asm="):
            spec = a[6:]
        elif a == "--replace":
            path = next(it, path)
        else:
            out = a
    if not out:
        raise SystemExit("usage: kingasm.py gen OUT [--asm SEL] [--replace FILE]")
    replacements = read_replace(path)
    if not run_checks(masm, replacements, path, cpp=spec.strip().lower() != "all"):
        return 1
    keep_asm = select(masm, replacements, spec)
    active = [r for r in replacements if r.key not in keep_asm]
    keys = set(r.key for r in active)
    drop, relabel, stubs = removal(masm, keys)
    lines = masm.lines
    text = list(masm.text)
    for i in drop:
        text[i] = ""
    for p in relabel:
        text[p] = "%s::" % masm.name(lines[p].name)
    for r in active:
        text[masm.defs[r.key]] = "; %s: C++ %s (src\\replace.txt)" % (masm.name(r.key), r.symbol)
    # labels the catch blocks of a removed function return to stay, at an
    # int 3: the catch blocks are dead code now
    for name in stubs:
        text[masm.defs[name]] = "%s::\n            int 3" % masm.name(name)
    # declarations before the first use
    decl = []
    for r in active:
        decl.append("EXTERN %s:PROC" % r.symbol)
        decl.append("%s TEXTEQU <%s>" % (masm.name(r.key), r.symbol))
    for p in sorted(relabel):
        decl.append("PUBLIC %s" % masm.name(lines[p].name))
    at = next(i for i, t in enumerate(masm.text) if t.startswith("_TEXT SEGMENT"))
    while at > 0 and masm.text[at - 1].startswith(";"):
        at -= 1
    header = ["; generated by tools\\kingasm.py from king.masm and src\\replace.txt: don't edit",
              "; functions replaced by C++: %d" % len(active)] + decl + [""]
    data = "\n".join(text[:at] + header + text[at:])
    old = None
    if os.path.exists(out):
        with open(out, "r", encoding="latin-1", newline="") as f:
            old = f.read()
    if old != data:
        with open(out, "w", encoding="latin-1", newline="") as f:
            f.write(data)
    print("%s: %d functions replaced by C++, %d kept in assembly" % (out, len(active), len(replacements) - len(active)))
    return 0


def cmd_stats(masm, args):
    lines = masm.lines
    insns = [i for i, l in enumerate(lines) if l.kind == INSN and not is_padding(l)]
    print("functions: %d (%d rounds)" % (len(masm.entries), masm.rounds))
    why = defaultdict(int)
    for w in masm.entries.values():
        why[w] += 1
    print("  started by: %s" % ", ".join("%s %d" % kv for kv in sorted(why.items())))
    print("  don't return: %d" % len(masm.noreturn))
    print("labels data points at inside a function: %d" % len(masm.interior))
    print("instructions: %d, in no function: %d, in several: %d" % (
        len(insns), sum(1 for i in insns if i not in masm.owner),
        sum(1 for i in insns if len(masm.owner.get(i, ())) > 1)))
    procs = [i for i, l in enumerate(lines) if l.kind == PROC]
    print("PROC blocks: %d, starting a function: %d" % (len(procs), sum(1 for i in procs if lines[i].name in masm.entries)))
    return 0


def plan_procs(masm):
    """-> ({entry: (first line, last line)} of the functions that get a PROC
    of their own, {entry: why not} of the others)"""
    lines = masm.lines
    defs = masm.defs
    # catch blocks between the code of their function go into its PROC, and
    # code that only they reach
    by_line = sorted((defs[k], k) for k in masm.entries)
    children = {}
    for e, eh in masm.eh.items():
        if e not in masm.functions:
            continue
        first, last = defs[e], max(masm.functions[e])
        keys = {e}
        catches = set(c for c in eh[3] if c in masm.functions)
        changed = bool(catches)
        while changed:
            changed = False
            lo = bisect.bisect_right(by_line, (first, ""))
            nxt = last + 1                  # a catch block may follow right after
            while nxt < len(lines) and lines[nxt].kind == BLANK:
                nxt += 1
            hi = bisect.bisect_right(by_line, (nxt, "\uffff"))
            for _, c in by_line[lo:hi]:
                if c not in keys and (c in catches or masm.exited_by.get(c) and masm.exited_by[c] <= keys):
                    keys.add(c)
                    last = max(last, max(masm.functions[c]))
                    changed = True
        if len(keys) > 1:
            children[e] = sorted(keys - {e}, key=lambda k: defs[k])
    inside = set(c for cs in children.values() for c in cs)
    plan, skipped = {}, {}
    for e in masm.entries:
        if e in inside:
            continue
        group = set(masm.functions[e])
        keys = {e}
        for c in children.get(e, ()):
            group |= masm.functions[c]
            keys.add(c)
        first, last = defs[e], max(group)
        if min(group) < first:
            skipped[e] = "has code before its start"
            continue
        why = None
        for i in range(first, last + 1):
            line = lines[i]
            if i in group:
                if any(o not in keys for o in masm.owner.get(i, ())):
                    why = "shares code with %s" % masm.name(next(o for o in masm.owner[i] if o not in keys))
                    break
                continue
            if is_neutral(line) or line.kind in (PROC, ENDP):
                continue
            if line.kind == LABEL and not masm.users.get(line.name) and line.name not in masm.entries:
                continue
            owners = masm.owner.get(i)
            why = "code of %s in between" % masm.name(owners[0]) if owners else "code of no function in between"
            break
        if why:
            skipped[e] = why
            continue
        # keep the PROC's own ENDP where it is if only padding is between
        if lines[first].kind == PROC and first in masm.blocks:
            q = masm.blocks[first]
            if q > last and all(is_neutral(lines[j]) for j in range(last + 1, q)):
                last = q
        plan[e] = (first, last)
    # A function start that has a PROC and gets no new one keeps the one it
    # has: the PROC makes its label public, and code outside king.masm may
    # use it (the C runtime's memcpy and memmove share code with their
    # neighbours). New PROCs that overlap it are dropped. Catch blocks inside
    # their function's PROC are only used through its exception tables.
    child_of = dict((c, e) for e, cs in children.items() for c in cs)
    while True:
        spans = sorted((f, l, e) for e, (f, l) in plan.items())
        firsts = [f for f, _, _ in spans]
        drop = set()
        for e in masm.entries:
            p = defs[e]
            if e in plan or child_of.get(e) in plan or lines[p].kind != PROC or p not in masm.blocks:
                continue
            q = masm.blocks[p]
            k = bisect.bisect_right(firsts, q) - 1
            while k >= 0 and spans[k][1] >= p:
                drop.add(spans[k][2])
                k -= 1
        if not drop:
            break
        for e in drop:
            skipped[e] = "overlaps a PROC that stays"
            del plan[e]
    return plan, skipped


def cmd_procs(masm, args):
    out = args[1] if args[:1] == ["--out"] else masm.path
    lines = masm.lines
    plan, skipped = plan_procs(masm)
    starts = {}
    ends = defaultdict(list)        # line -> functions whose new ENDP follows it
    own_end = set()                 # ENDP lines that stay the end of their PROC
    for e, (first, last) in plan.items():
        starts[first] = e
        if lines[last].kind == ENDP and masm.blocks.get(first) == last:
            own_end.add(last)
        else:
            ends[last].append(e)
    spans = sorted(plan.values())
    # PROC blocks of king.masm that no new PROC overlaps stay as they are
    firsts = [f for f, _ in spans]

    def overlaps(p, q):
        k = bisect.bisect_right(firsts, q) - 1
        return k >= 0 and spans[k][1] >= p
    kept = set()
    for p, q in masm.blocks.items():
        if p not in starts and not overlaps(p, q):
            kept.add(p)
            kept.add(q)
    text = masm.text
    result = []
    counts = defaultdict(int)
    for i, raw in enumerate(text):
        line = lines[i]
        comment = raw[len(strip_comment(raw)):]
        if i in starts:
            if line.kind == PROC:
                result.append(raw)
                counts["PROC kept"] += 1
            else:
                result.append("%s PROC%s" % (masm.name(starts[i]), comment and " " + comment.lstrip()))
                counts["PROC new"] += 1
        elif line.kind == PROC:
            if i in kept:
                result.append(raw)
            else:
                result.append("%s::%s" % (masm.name(line.name), comment and " " + comment.lstrip()))
                counts["PROC -> label"] += 1
        elif line.kind == ENDP:
            if i in kept or i in own_end:
                result.append(raw)
                if i in own_end:
                    counts["ENDP kept"] += 1
            else:
                counts["ENDP dropped"] += 1
        else:
            result.append(raw)
        for e in ends.get(i, ()):
            q = masm.blocks.get(masm.defs[e])
            old = masm.text[q] if q is not None else ""
            comment = old[len(strip_comment(old)):]
            result.append("%s ENDP%s" % (masm.name(e), comment and " " + comment.lstrip()))
            counts["ENDP new"] += 1
    bindings = asm_bindings()
    bound = [k for k in bindings if k in masm.code and k not in plan and not any(
        lines[p].name == k for p in kept if lines[p].kind == PROC)]
    for k in bound:
        print("error: %s is used by the C++ code (%s) but gets no PROC: %s"
              % (masm.name(k), bindings[k], skipped.get(k, "not a function")))
    if bound:
        return 1
    why = defaultdict(int)
    for w in skipped.values():
        why[w.split(" ")[0] + " " + " ".join(w.split(" ")[1:2])] += 1
    print("functions: %d, with a PROC of their own: %d, without: %d" % (len(masm.entries), len(plan), len(skipped)))
    print("  without: %s" % ", ".join("%s %d" % kv for kv in sorted(why.items())))
    print("  %s" % ", ".join("%s: %d" % kv for kv in sorted(counts.items())))
    with open(out, "w", encoding="latin-1", newline="") as f:
        f.write("\n".join(result))
    print("wrote %s" % out)
    return 0


COMMANDS = {"info": cmd_info, "check": cmd_check, "gen": cmd_gen, "procs": cmd_procs, "stats": cmd_stats}


def main(argv):
    if len(argv) < 2 or argv[1] not in COMMANDS:
        print(__doc__)
        return 2
    return COMMANDS[argv[1]](Masm(), argv[2:])


if __name__ == "__main__":
    sys.exit(main(sys.argv))
