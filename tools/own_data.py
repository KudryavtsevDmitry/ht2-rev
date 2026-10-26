"""Give the _RDATA and _DATA segments of king.masm to C++ (one-time conversion).

Reads king.masm as the default build sees it (no ASM_* symbol defined) and
writes src/engine/data_image.cpp: KingRdata and KingData, byte for byte, with
every DWORD that names a symbol turned into a pointer (to the C++ image
itself, to KingBss, or to a code label through /alternatename). Then:
  * king.masm: the segments' contents go under IFDEF ASM_DATA; otherwise each
    of their labels is $L_x TEXTEQU <(_KingData+offset)> (or _KingRdata), and
    the code labels the images point at are made PUBLIC;
  * src/: ASM_VAR of a data label and KING_BSS become KING_VAR(type, name,
    address in hex without 0x).
"""
import re
from pathlib import Path

repo = Path(__file__).resolve().parent.parent
masm_path = repo / 'king.masm'
lines = masm_path.read_text().split('\n')

# --- the default configuration: which lines are assembled -----------------
active = []
stack = []          # (this branch active, any branch taken, parent active)
cur = True
for line in lines:
    s = line.strip()
    m = re.match(r'(IFDEF|IFNDEF|IF)\s+(\S+)', s)
    if m and not s.startswith(('IFDEF(', )):
        if m[1] == 'IFDEF':
            cond = False
        elif m[1] == 'IFNDEF':
            cond = True
        else:
            cond = m[2] not in ('0', 'FALSE')
        stack.append((cur, cond))
        cur = cur and cond
        active.append(None)
        continue
    if s == 'ELSE':
        parent, cond = stack[-1]
        cur = parent and not cond
        active.append(None)
        continue
    if s == 'ENDIF':
        cur = stack.pop()[0]
        active.append(None)
        continue
    active.append(cur)
assert not stack

textequ = {}
code_labels = set()
externs = set()
for line, a in zip(lines, active):
    if not a:
        continue
    m = re.match(r'^(\S+)\s+TEXTEQU\s+<(.*)>', line)
    if m:
        textequ[m[1]] = m[2]
        continue
    m = re.match(r'^\s*(\S+)\s+PROC\b', line) or re.match(r'^(\S+)::', line) \
        or re.match(r'^(\S+)\s+LABEL\s+NEAR', line) \
        or re.match(r'^(\$L_[0-9a-f]{6}|FUN_[0-9a-f]{6})\s+(DWORD|WORD|BYTE|DB|DW|DD) ', line)
    if m:
        code_labels.add(m[1])
        continue
    m = re.match(r'^\s*EXTERN\s+([^:\s]+):', line)
    if m:
        externs.add(m[1])

# --- the two segments ------------------------------------------------------
BSS_BASE = 0x68a820
segs = {}
for seg, sym in (('_RDATA', 'KingRdata'), ('_DATA', 'KingData')):
    start = next(i for i, l in enumerate(lines) if l.startswith(seg + ' SEGMENT'))
    end = lines.index(seg + ' ENDS')
    items = []      # ('b', bytes) / ('p', symbol, addend)
    labels = {}     # label -> offset
    base = None
    off = 0
    for i in range(start + 1, end):
        if not active[i]:
            continue
        line = lines[i]
        s = line.strip()
        if not s or s.startswith((';', 'PUBLIC', 'EXTERN')):
            continue
        m = re.match(r'^(\$L_([0-9a-f]{6}))\s+(.*)$', line)
        if m:
            addr = int(m[2], 16)
            if base is None:
                base = addr
            if addr - base < off:
                raise SystemExit(f'{m[1]} overlaps')
            if addr - base > off:
                print(f'gap of {addr - base - off} before {m[1]}')
                items.append(('b', bytes(addr - base - off)))
                off = addr - base
            labels[m[1]] = off
            s = m[3].strip()
        if s.startswith('ALIGN'):
            if base is None:
                continue
            n = int(s.split()[1])
            pad = (-(base + off)) % n
            items.append(('b', bytes(pad)))
            off += pad
            continue
        s = re.sub(r'\s*;.*$', '', s) if "'" not in s else s
        if m := re.fullmatch(r'BYTE ([0-9a-f]+)H', s):
            data = bytes([int(m[1], 16)])
        elif m := re.fullmatch(r'WORD ([0-9a-f]+)H', s):
            data = int(m[1], 16).to_bytes(2, 'little')
        elif m := re.fullmatch(r'DWORD ([0-9a-f]+)H', s):
            data = int(m[1], 16).to_bytes(4, 'little')
        elif m := re.fullmatch(r'DB (\d+) DUP\((\d+)\)', s):
            data = bytes([int(m[2])]) * int(m[1])
        elif m := re.fullmatch(r'DB ((?:[0-9a-f]+H,?)+)', s):
            data = bytes(int(x[:-1], 16) for x in m[1].split(','))
        elif s.startswith('DB '):
            data = b''
            for part in re.findall(r"'(?:[^']|'')*'|[^,\s]+", s[3:]):
                data += part[1:-1].replace("''", "'").encode('latin-1') if part.startswith("'") else bytes([int(part.rstrip('H'), 16 if part.endswith('H') else 10)])
        elif m := re.fullmatch(r'DWORD (\S+?)(?:\+(\d+))?', s):
            items.append(('p', m[1], int(m[2] or 0)))
            off += 4
            continue
        else:
            raise SystemExit(f'line {i + 1}: cannot parse {line!r}')
        items.append(('b', data))
        off += len(data)
    segs[seg] = dict(sym=sym, base=base, size=off, items=items, labels=labels)
    print(f'{seg}: base {base:#x}, size {off:#x}, {len(labels)} labels')

assert segs['_RDATA']['base'] + segs['_RDATA']['size'] <= segs['_DATA']['base']
assert segs['_DATA']['base'] + segs['_DATA']['size'] <= BSS_BASE
data_labels = {}
for seg in segs.values():
    for name, off in seg['labels'].items():
        data_labels[name] = (seg['sym'], off)

# --- pointer targets ---------------------------------------------------------
aliases = {}        # target symbol -> C name


def resolve(name, addend):
    seen = 0
    while name in textequ:
        name = textequ[name].strip()
        seen += 1
        assert seen < 20, name
    if m := re.fullmatch(r'\(_KingBss\+(\d+)\)', name):
        return f'KingBss + {int(m[1]) + addend}'
    if name in data_labels:
        sym, off = data_labels[name]
        return f'(const unsigned char*)&{sym} + {off + addend}'
    if name in code_labels or name in externs:
        if name not in aliases:
            aliases[name] = f'KD{len(aliases)}'
        c = aliases[name]
        return f'(const unsigned char*)&{c} + {addend}' if addend else f'&{c}'
    raise SystemExit('unresolved data pointer ' + name)


# --- C++ ---------------------------------------------------------------------
out = ['// data_image.cpp - generated by tools/own_data.py: the original _RDATA and',
       '// _DATA segments, byte for byte (king.masm points their labels into these',
       '// images unless ASM_DATA). DWORDs that held addresses are pointers.',
       '#ifndef ASM_DATA',
       '#define KING_ALIAS(cname, label) __pragma(comment(linker, "/alternatename:" cname "=" label))',
       '',
       'extern "C" unsigned char KingBss[];']
bodies = []
for seg in segs.values():
    fields, inits = [], []
    run = bytearray()
    def flush():
        if run:
            fields.append(f'    unsigned char b{len(fields)}[{len(run)}];')
            inits.append('{' + ','.join(str(b) for b in run) + '}')
            run.clear()
    for item in seg['items']:
        if item[0] == 'b':
            run += item[1]
        else:
            flush()
            fields.append(f'    const void* p{len(fields)};')
            inits.append('(const void*)(' + resolve(item[1], item[2]) + ')')
    flush()
    seg['fields'], seg['inits'] = fields, inits

for target, c in aliases.items():
    out.append(f'extern "C" void {c}(); KING_ALIAS("_{c}", "{target}")')
out.append('')
out.append('#pragma pack(push, 1)')
for seg in segs.values():
    out.append(f'struct {seg["sym"]}Image {{')
    out += seg['fields']
    out.append('};')
    out.append(f'static_assert(sizeof({seg["sym"]}Image) == {seg["size"]}, "size");')
out.append('#pragma pack(pop)')
out.append('extern "C" const KingRdataImage KingRdata;')
out.append('extern "C" KingDataImage KingData;')
out.append('')
for seg in segs.values():
    const = 'const ' if seg['sym'] == 'KingRdata' else ''
    out.append(f'extern "C" __declspec(align(16)) {const}{seg["sym"]}Image {seg["sym"]} = {{')
    out += ['    ' + x + ',' for x in seg['inits']]
    out.append('};')
    out.append('')
(repo / 'src/engine/data_image.cpp').write_text('\n'.join(out), newline='\n')
print(f'{len(aliases)} code symbols')

# --- king.masm -------------------------------------------------------------
res = []
public_moved = []
for i, line in enumerate(lines):
    for seg in ('_RDATA', '_DATA'):
        if line.startswith(seg + ' SEGMENT'):
            res += [line, 'IFDEF ASM_DATA']
            break
        if line == seg + ' ENDS':
            res += ['ENDIF', line]
            break
    else:
        if line.startswith('PUBLIC ') and active[i] is not False:
            names = [n.strip() for n in line[7:].split(',')]
            keep = [n for n in names if n not in data_labels]
            public_moved += [n for n in names if n in data_labels]
            if keep:
                res.append('PUBLIC ' + ', '.join(keep))
            continue
        res.append(line)

# into the IFNDEF ASM_DATA block that own_bss.py wrote
i = res.index('EXTERN _KingBss:BYTE')
block = ['EXTERN _KingRdata:BYTE', 'EXTERN _KingData:BYTE']
for name, (sym, off) in sorted(data_labels.items()):
    block.append(f'{name} TEXTEQU <(_{sym}+{off})>')
pub = sorted(t for t in aliases if t in code_labels)
for k in range(0, len(pub), 8):
    block.append('PUBLIC ' + ', '.join(pub[k:k + 8]))
res[i + 1:i + 1] = block
masm_text = '\n'.join(res)

# --- src/ ------------------------------------------------------------------
var_re = re.compile(r'ASM_VAR\(((?:[^(),]|\([^()]*\))+),\s*(\w+),\s*\$L_([0-9a-f]{6})\)')
bss_re = re.compile(r'KING_BSS\(((?:[^(),]|\([^()]*\))+),\s*(\w+),\s*0x([0-9a-f]{6})\)')
used = set()
count = 0
for path in (repo / 'src').rglob('*.[ch]*'):
    if path.name == 'data_image.cpp':
        continue
    src = path.read_bytes().decode('utf-8')
    def repl(m):
        global count
        if m.re is var_re and '$L_' + m[3] not in data_labels:
            return m[0]
        count += 1
        used.add('$L_' + m[3])
        return f'KING_VAR({m[1]}, {m[2]}, {m[3]})'
    new = bss_re.sub(repl, var_re.sub(repl, src))
    used.update('$L_' + a for a in re.findall(r'KING_VAR\((?:[^(),]|\([^()]*\))+,\s*\w+,\s*([0-9a-f]{6})\)', new))
    if new != src:
        path.write_bytes(new.encode('utf-8'))
print(f'{count} KING_VAR')

# ASM_DATA builds: C++ reaches the masm labels by name again
pubs = sorted(used | set(public_moved))
blk = ['IFDEF ASM_DATA'] + ['PUBLIC ' + ', '.join(pubs[k:k + 8]) for k in range(0, len(pubs), 8)] + ['ENDIF']
masm_text = masm_text.replace('; _BSS is owned by C++', '\n'.join(blk) + '\n; _BSS is owned by C++', 1)
masm_path.write_text(masm_text, newline='\n')
