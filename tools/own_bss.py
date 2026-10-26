"""Give the _BSS segment of king.masm to C++ (one-time conversion).

C++ defines the whole block as KingBss (src/engine/data.cpp). Each label of
the segment becomes $L_x TEXTEQU <(_KingBss+offset)>, so the layout and the
assembly's offset arithmetic stay as they were. The original labels stay
under IFDEF ASM_DATA (nmake ASM=1). In src/, ASM_VAR of a BSS label becomes
KING_BSS(type, name, address).
"""
import re
from pathlib import Path

repo = Path(__file__).resolve().parent.parent
masm = repo / 'king.masm'
text = masm.read_text()
lines = text.split('\n')

start = lines.index('_BSS SEGMENT')
end = lines.index('_BSS ENDS')
label_re = re.compile(r'^(\$L_([0-9a-f]{6}))\s+DB (\d+) DUP\(\?\)$')
labels = []
for line in lines[start + 1:end]:
    m = label_re.match(line)
    if m:
        labels.append((m[1], int(m[2], 16), int(m[3])))
base = labels[0][1]
for (name, addr, size), nxt in zip(labels, labels[1:] + [(None, None, None)]):
    if nxt[1] is not None and addr + size != nxt[1]:
        raise SystemExit('gap after ' + name)
total = labels[-1][1] + labels[-1][2] - base
names = {name for name, _, _ in labels}
print(f'{len(labels)} labels, base {base:#x}, size {total:#x}')

out = []
for i, line in enumerate(lines):
    if i == start:
        out += [line, 'IFDEF ASM_DATA']
        continue
    if i == end:
        out += ['ENDIF', line]
        continue
    if line.startswith('PUBLIC '):
        keep = [n.strip() for n in line[7:].split(',') if n.strip() not in names]
        if not keep:
            continue
        line = 'PUBLIC ' + ', '.join(keep)
    if re.match(r'^(\S+) TEXTEQU <UnusedTrap>$', line) and line.split()[0] in names:
        raise SystemExit('BSS label in the unused list: ' + line)
    out.append(line)

# the BSS labels' TEXTEQUs go before anything uses them: ahead of the
# first PUBLIC line
first = next(i for i, l in enumerate(out) if l.startswith('PUBLIC '))
block = ['; _BSS is owned by C++ (KingBss, src/engine/data.cpp) unless ASM_DATA',
         'IFNDEF ASM_DATA', 'EXTERN _KingBss:BYTE']
block += [f'{name} TEXTEQU <(_KingBss+{addr - base})>' for name, addr, _ in labels]
block += ['ENDIF']
out[first:first] = block
masm.write_text('\n'.join(out), newline='\n')

# C++ side
var_re = re.compile(r'ASM_VAR\(((?:[^(),]|\([^()]*\))+),\s*(\w+),\s*(\$L_[0-9a-f]{6})\)')
count = 0
for path in (repo / 'src').rglob('*.[ch]*'):
    src = path.read_bytes().decode('utf-8')
    def repl(m):
        global count
        if m[3] not in names:
            return m[0]
        count += 1
        return f'KING_BSS({m[1]}, {m[2]}, 0x{m[3][3:]})'
    new = var_re.sub(repl, src)
    if new != src:
        path.write_bytes(new.encode('utf-8'))
print(f'{count} ASM_VAR -> KING_BSS')
print(f'KING_BSS_BASE {base:#x} KING_BSS_SIZE {total:#x}')
