"""Write the assembly the default build still needs into src/asm/.

king.masm stays the reference program (nmake ASM=1 and the ASM_* switches).
The default build assembles only what is active without any switch: this
script takes those lines of _TEXT, replaces every TEXTEQU (labels of C++
functions and of the data owned by C++) by its text and splits the code into
one file per original unit, src/asm/asm_<unit>.asm, each with its own EXTERN
and PUBLIC lines. A cut is made only where execution can't run on into the
next procedure. Run it again after changing king.masm.
"""
import csv
import re
import shutil
from pathlib import Path

repo = Path(__file__).resolve().parent.parent
lines = (repo / 'king.masm').read_text().split('\n')

# --- the default configuration ----------------------------------------------
active = []
stack = []
cur = True
for line in lines:
    s = line.strip()
    m = re.match(r'(IFDEF|IFNDEF|IF)\s+(\S+)', s)
    if m:
        cond = {'IFDEF': False, 'IFNDEF': True}.get(m[1], m[2] not in ('0', 'FALSE'))
        stack.append((cur, cond))
        cur = cur and cond
        active.append(None)
    elif s == 'ELSE':
        parent, cond = stack[-1]
        cur = parent and not cond
        active.append(None)
    elif s == 'ENDIF':
        cur = stack.pop()[0]
        active.append(None)
    else:
        active.append(cur)
assert not stack

textequ = {}
externs = {}
for line, a in zip(lines, active):
    if not a:
        continue
    if m := re.match(r'^(\S+)\s+TEXTEQU\s+<(.*)>', line):
        textequ[m[1]] = m[2]
    elif m := re.match(r'^\s*EXTERN\s+([^:\s]+):(\w+)', line):
        externs[m[1]] = m[2]

tok_re = re.compile(r'[A-Za-z_$?@][\w$?@]*')


def expand(text):
    for _ in range(10):
        new = tok_re.sub(lambda m: textequ.get(m[0], m[0]), text)
        if new == text:
            return text
        text = new
    raise SystemExit('TEXTEQU loop: ' + text)


# --- the active code -----------------------------------------------------------
tstart = lines.index('_TEXT SEGMENT')
tend = lines.index('_TEXT ENDS')
code = []
for i in range(tstart + 1, tend):
    if not active[i]:
        continue
    line = lines[i]
    if re.match(r'^\s*(EXTERN|PUBLIC)\b', line) or re.match(r'^\S+\s+TEXTEQU\b', line):
        continue
    if ';' in line and "'" not in line:
        body, comment = line.split(';', 1)
        line = expand(body) + ';' + comment
    elif "'" not in line:
        line = expand(line)
    code.append(line)

sizes = {'DB': 'BYTE', 'BYTE': 'BYTE', 'DW': 'WORD', 'WORD': 'WORD', 'DD': 'DWORD',
         'DWORD': 'DWORD', 'DQ': 'QWORD', 'QWORD': 'QWORD', 'REAL4': 'REAL4',
         'REAL8': 'REAL8', 'TBYTE': 'TBYTE', 'DT': 'TBYTE'}


def defines(line):
    """(name, EXTERN type, is a PROC) of a global label defined on the line"""
    if m := re.match(r'^\s*(\S+)\s+PROC\b', line):
        return m[1], 'NEAR', True
    if m := re.match(r'^(\S+)::', line):
        return m[1], 'NEAR', False
    if m := re.match(r'^(\S+)\s+LABEL\s+(\w+)', line):
        return m[1], m[2].upper(), False
    if m := re.match(r'^([$\w@?]+)\s+(\w+)\b', line):
        if m[2].upper() in sizes:
            return m[1], sizes[m[2].upper()], False
    return None


globals_ = {}
for line in code:
    d = defines(line)
    if d:
        globals_[d[0]] = d[1]

# --- units ---------------------------------------------------------------------
units = list(csv.DictReader((repo / 'units.csv').open(newline='')))


def unit_of(name):
    m = re.search(r'([0-9a-fA-F]{6})', name)
    if not m:
        return None
    a = int(m[1], 16)
    for u in units:
        if int(u['start'], 16) <= a < int(u['end'], 16):
            n = u['name'] or 'unit_' + u['start']
            if u['basis'] == 'library':
                n = {'C runtime': 'crt', 'gzip': 'gzip', 'exception': 'eh_funclets',
                     'MFC': 'mfc'}.get(next((k for k in ('C runtime', 'gzip', 'exception', 'MFC')
                                              if k in u['name']), ''), 'library')
            return re.sub(r'\W+', '_', n.rsplit('.', 1)[0].lower())
    return None


def falls_through(chunk):
    """can execution run off the end of these lines?"""
    for line in reversed(chunk):
        s = re.sub(r';.*', '', line).strip()
        if not s or re.match(r'^(ALIGN|EVEN)\b', s) or re.match(r'^\S+\s+ENDP\b', s):
            continue
        if re.match(r'^(\S+::?|\S+\s+LABEL\b)', s) and not re.match(r'^\S+::?\s*\S', s):
            return True             # a label at the end: something may jump there
        if re.match(r'^(ret|retn|jmp|int\s+3)\b', s, re.I):
            return False
        d = defines(line)
        if re.match(r'^(DB|DW|DD|BYTE|WORD|DWORD|QWORD|REAL[48]|TBYTE)\b', s, re.I) or (d and not d[2] and d[1] != 'NEAR'):
            return False
        return True
    return False


files = {}
order = []
current = 'misc'
chunk = []
pending = []                        # ALIGN/blank lines before the next PROC


def flush():
    if chunk:
        if current not in files:
            files[current] = []
            order.append(current)
        files[current].append(list(chunk))
        chunk.clear()


for line in code:
    m = re.match(r'^\s*(\S+)\s+PROC\b', line)
    if m:
        u = unit_of(m[1])
        if u and u != current and not falls_through(chunk):
            flush()
            current = u
        chunk.extend(pending)
        pending.clear()
        chunk.append(line)
    elif re.match(r'^\s*(ALIGN\b.*)?$', line):
        pending.append(line)
    else:
        chunk.extend(pending)
        pending.clear()
        chunk.append(line)
chunk.extend(pending)
flush()

# --- write ---------------------------------------------------------------------
out_dir = repo / 'src' / 'asm'
if out_dir.exists():
    shutil.rmtree(out_dir)
out_dir.mkdir()
header = [
    '; king.inc - common to the assembly files of src\\asm',
    '.686p', '.XMM', '.MODEL FLAT', 'ASSUME FS:NOTHING', 'ASSUME GS:NOTHING',
    'OPTION PROC:PUBLIC',
] + [l for l, a in zip(lines, active) if a and l.startswith('INCLUDELIB ')]
(out_dir / 'king.inc').write_text('\n'.join(header) + '\n', newline='\n')
names = []
for unit in order:
    body = [l for c in files[unit] for l in c]
    defined = {}
    for line in body:
        d = defines(line)
        if d:
            defined[d[0]] = d
    used = set()
    for line in body:
        text = line.split(';', 1)[0] if "'" not in line else line
        used.update(tok_re.findall(text))
    ext = []
    for name in sorted(used - set(defined)):
        if name in globals_:
            ext.append(f'EXTERN {name}:{globals_[name]}')
        elif name in externs:
            ext.append(f'EXTERN {name}:{externs[name]}')
        elif name in ('_KingData', '_KingRdata', '_KingBss'):
            ext.append(f'EXTERN {name}:BYTE')
    pub = sorted(n for n, d in defined.items() if not d[2])
    text = [f'; asm_{unit}.asm - generated by tools/split_asm.py from king.masm (the',
            '; default build\'s assembly of this unit; edit king.masm, not this file)',
            'INCLUDE king.inc', '']
    text += ext + [''] if ext else []
    text += ['PUBLIC ' + ', '.join(pub[k:k + 8]) for k in range(0, len(pub), 8)]
    text += ['', '_TEXT SEGMENT'] + body + ['_TEXT ENDS', 'END', '']
    (out_dir / f'asm_{unit}.asm').write_text('\n'.join(text), newline='\n')
    names.append(f'asm_{unit}')
    print(f'asm_{unit}.asm: {sum(1 for l in body if re.match(r"^\s*\S+\s+PROC\b", l))} procs, {len(body)} lines')
(out_dir / 'objects.mak').write_text('ASMOBJS = ' + ' '.join(n + '.obj' for n in names) + '\n', newline='\n')
