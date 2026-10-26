r"""Remove code in src/asm that nothing can reach.

    py tools\asm_dead.py [--list] [--apply]

Blocks start at every label at column 0. A block is live when C++ names one of
its labels, when the data names it (data_image.cpp, except entries of the
original exception tables), when a live block references it or falls through
into it, or when a live block installs an exception table (FuncInfo) that
names it. --apply deletes the rest, points exception-table entries of removed
handlers at UnusedTrap and updates src/asm/objects.mak.
"""
import os, re, sys, glob
from pathlib import Path

tok = re.compile(r'[A-Za-z_$?@][\w$?@]*')
labdef = re.compile(r'^([$\w@?]+)(?:\s+PROC\b|::?(?!=)|\s+LABEL\b|\s+(?:BYTE|WORD|DWORD|QWORD|REAL4|REAL8|TBYTE|DB|DW|DD|DQ)\b)')

sys.path.insert(0, str(Path(__file__).parent))
os.chdir(Path(__file__).resolve().parent.parent)
import ehdata
images = ehdata.load()
eh, eh_slots = ehdata.eh_tables(images)
cpp_names = {v[1] for img, (d, ptrs) in images.items() for o, v in ptrs.items()
             if v[0] == 'KD' and (img, o) not in eh_slots}
for p in list(Path('src').rglob('*.cpp')) + list(Path('src').rglob('*.h')):
    if p.name == 'data_image.cpp':
        continue
    t = re.sub(r'//[^\n]*', '', p.read_text(encoding='utf-8', errors='replace'))
    cpp_names.update(tok.findall(t))
    cpp_names.update(re.findall(r'"_?([$\w@?]+)"', t))

files = {}
for p in sorted(glob.glob('src/asm/asm_*.asm')):
    files[p] = open(p, newline='').read().replace('\r\n', '\n').split('\n')


def code_part(line):
    if "'" in line:
        return line
    return line.split(';', 1)[0]


# instructions after which execution can't go on (the throw helper never returns)
ENDS = re.compile(r'^\s*(ret|retn|jmp|int\s+3|call\s+__CxxThrowException@8)\b', re.I)
blocks = []           # (file, start, end, labels, refs, falls)
label_block = {}
for p, L in files.items():
    s = L.index('_TEXT SEGMENT') + 1
    e = L.index('_TEXT ENDS')
    cur = None
    def close(i):
        if cur is not None:
            blocks.append(cur + [i])
    starts = []
    for i in range(s, e):
        m = labdef.match(code_part(L[i]))
        if m and not re.match(r'^\s*\S+\s+ENDP\b', L[i]):
            starts.append(i)
    bounds = [s] + starts + [e]
    for a, b in zip(bounds, bounds[1:]):
        if a == b:
            continue
        labels, refs = [], set()
        last = None
        for i in range(a, b):
            c = code_part(L[i])
            m = labdef.match(c)
            if m:
                labels.append(m[1])
                c = c[m.end():]
            refs.update(tok.findall(c))
            refs.update('R:' + x for x in re.findall(r'_KingRdata\+(\d+)', c))
            st = c.strip()
            if st and not re.match(r'^(ALIGN|EVEN|ENDP|nop|\S+\s+ENDP)\b', st) and not re.match(r'^\S+\s+ENDP\b', L[i].strip()):
                last = st
        falls = last is None or not (ENDS.match(last) or re.match(r'^(BYTE|WORD|DWORD|DD|DB|DW)\b', last, re.I))
        idx = len(blocks)
        blocks.append([p, a, b, labels, refs, falls])
        for l in labels:
            label_block[l] = idx

live = set()
work = []
for i, b in enumerate(blocks):
    if any(l in cpp_names for l in b[3]):
        work.append(i)
while work:
    i = work.pop()
    if i in live:
        continue
    live.add(i)
    p, a, e, labels, refs, falls = blocks[i]
    for r in list(refs):
        if r.startswith('R:') and int(r[2:]) in eh:
            for l in eh[int(r[2:])]:
                j = label_block.get(l)
                if j is not None and j not in live:
                    work.append(j)
        j = label_block.get(r)
        if j is not None and j not in live:
            work.append(j)
    if falls and i + 1 < len(blocks) and blocks[i + 1][0] == p:
        work.append(i + 1)

dead = [b for i, b in enumerate(blocks) if i not in live]
n = sum(b[2] - b[1] for b in dead)
print('blocks', len(blocks), 'dead', len(dead), 'lines', n)
if '--list' in sys.argv:
    for b in dead:
        print(b[0], b[3][:3], b[2] - b[1])

if '--apply' in sys.argv:
    kill = {}
    for b in dead:
        kill.setdefault(b[0], set()).update(range(b[1], b[2]))
    gone = {l for b in dead for l in b[3]}
    for p, L in files.items():
        k = kill.get(p, set())
        out = []
        for i, l in enumerate(L):
            if i in k:
                # keep PROC/ENDP framing whose partner survives
                if re.match(r'^\s*\S+\s+ENDP\b', l):
                    name = l.split()[0]
                    if name not in gone:
                        out.append(l)
                continue
            if re.match(r'^\s*\S+\s+ENDP\b', l) and l.split()[0] in gone:
                continue
            out.append(l)
        # PUBLIC / EXTERN tidy
        body_tokens = set()
        s = out.index('_TEXT SEGMENT')
        for l in out[s:]:
            body_tokens.update(tok.findall(code_part(l)))
        res = []
        for l in out[:s]:
            m = re.match(r'^PUBLIC (.*)', l)
            if m:
                names = [x.strip() for x in m[1].split(',') if x.strip() not in gone]
                if names:
                    res.append('PUBLIC ' + ', '.join(names))
                continue
            m = re.match(r'^EXTERN ([^:\s]+):', l)
            if m and m[1] not in body_tokens:
                continue
            res.append(l)
        out = res + out[s:]
        procs = sum(1 for l in out if re.match(r'^\s*\S+\s+PROC\b', l))
        if procs == 0 and not any(code_part(l).strip() for l in out[s + 1:out.index('_TEXT ENDS')]):
            Path(p).unlink()
            print('removed', p)
            continue
        open(p, 'w', newline='').write('\n'.join(out))

if '--apply' in sys.argv:
    gone = {l for b in dead for l in b[3]}
    p = Path('src/engine/data_image.cpp')
    t = p.read_text(encoding='utf-8')
    n = re.sub(r'(KING_ALIAS\("_KD\d+", ")([^"]+)("\))',
               lambda m: m[1] + ('_UnusedTrap' if m[2] in gone else m[2]) + m[3], t)
    if n != t:
        p.write_text(n, encoding='utf-8', newline='')
        print('data_image.cpp: exception-table entries of removed handlers point at UnusedTrap')
    mak = Path('src/asm/objects.mak')
    m = mak.read_text()
    for o in re.findall(r'(asm_\w+)\.obj', m):
        if not Path('src/asm/' + o + '.asm').exists():
            m = m.replace(' ' + o + '.obj', '')
    mak.write_text(m, newline='\n')
