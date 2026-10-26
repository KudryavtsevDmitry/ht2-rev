"""Take procedures out of src/asm once they are rewritten in C++.

    py tools\\asm_replace.py LABEL=CNAME [LABEL=CNAME ...]

CNAME is the C++ function's linker name (_Name for cdecl, _Name@8 for
stdcall, @Name@8 for fastcall). For each LABEL the script
  * deletes its code from src/asm: from the label to the next label at
    column 0 (a PROC's ENDP goes with its PROC line),
  * points the other assembly at CNAME (EXTERN CNAME:NEAR), drops the label
    from PUBLIC lines,
  * points C++ at CNAME: "LABEL" strings in KING_ALIAS lines (data_image.cpp's
    code pointers), the label argument of ASM_FUNC / ASM_VAR, and
    ASM_PROC(LABEL) becomes ASM_FUNC(LABEL, CNAME),
  * removes files left without code and updates src/asm/objects.mak.
Code that only the deleted part reached is not removed; it shows up as
unreferenced labels.
"""
import re
import sys
from pathlib import Path

repo = Path(__file__).resolve().parent.parent
asm_dir = repo / 'src' / 'asm'
tok = re.compile(r'[A-Za-z_$?@][\w$?@]*')
labdef = re.compile(r'^([$\w@?]+)(?:\s+PROC\b|::?(?!=)|\s+LABEL\b|\s+(?:BYTE|WORD|DWORD|QWORD|REAL4|REAL8|TBYTE|DB|DW|DD|DQ)\b)')

pairs = dict(a.split('=', 1) for a in sys.argv[1:])
if not pairs:
    raise SystemExit(__doc__)


def read(p):
    return p.read_text(encoding='utf-8').replace('\r\n', '\n').split('\n')


def write(p, lines):
    p.write_text('\n'.join(lines), encoding='utf-8', newline='\n')


def code(line):
    return line if "'" in line else line.split(';', 1)[0]


found = set()
for p in sorted(asm_dir.glob('asm_*.asm')):
    L = read(p)
    out = []
    skipping = None          # label being deleted
    procs_gone = set()
    for line in L:
        m = labdef.match(line)
        endp = re.match(r'^(\S+)\s+ENDP\b', line)
        if endp:
            skipping = None
            if endp[1] in procs_gone or endp[1] in pairs and endp[1] in found:
                continue
            out.append(line)
            continue
        if m and not line.startswith(('PUBLIC', 'EXTERN')):
            skipping = m[1] if m[1] in pairs else None
            if skipping:
                found.add(m[1])
                if re.match(r'^\S+\s+PROC\b', line):
                    procs_gone.add(m[1])
                continue
        if re.match(r'^\s*(_TEXT ENDS|ALIGN\b)', line):
            skipping = None
        if skipping:
            continue
        out.append(line)
    # A PROC whose first label went but whose ENDP stays needs its PROC line.
    for name in procs_gone:
        if any(re.match(r'^' + re.escape(name) + r'\s+ENDP\b', l) for l in out):
            raise SystemExit(f'{name}: only part of the PROC was deleted; convert its other labels too')
    # references
    s = out.index('_TEXT SEGMENT')
    body = []
    for line in out[s:]:
        if "'" not in line:
            c, sep, cm = line.partition(';')
            c = tok.sub(lambda t: pairs.get(t[0], t[0]), c)
            line = c + sep + cm
        body.append(line)
    used = set()
    for line in body:
        used.update(tok.findall(code(line)))
    head = []
    for line in out[:s]:
        mp = re.match(r'^PUBLIC (.*)', line)
        if mp:
            names = [x.strip() for x in mp[1].split(',') if x.strip() not in pairs]
            if names:
                head.append('PUBLIC ' + ', '.join(names))
            continue
        me = re.match(r'^EXTERN ([^:\s]+):', line)
        if me and (me[1] in pairs or me[1] not in used):
            continue
        head.append(line)
    have = {re.match(r'^EXTERN ([^:\s]+):', l)[1] for l in head if l.startswith('EXTERN ')}
    defined = {m[1] for l in body if (m := labdef.match(l))}
    new = sorted(c for c in set(pairs.values()) if c in used and c not in have and c not in defined)
    if new:
        k = next((i for i, l in enumerate(head) if l.startswith('EXTERN ')), 3)
        head[k:k] = [f'EXTERN {c}:NEAR' for c in new]
    out = head + body
    if out != L:
        has_code = any(code(l).strip() and not re.match(r'^\s*(ALIGN\b|_TEXT|END$|\S+\s+(PROC|ENDP)\b)', l)
                       for l in out[out.index('_TEXT SEGMENT') + 1:])
        if not has_code:
            p.unlink()
            mak = asm_dir / 'objects.mak'
            write(mak, [l.replace(' ' + p.stem + '.obj', '') for l in read(mak)])
            print('removed', p.name)
        else:
            write(p, out)
            print('updated', p.name)

missing = set(pairs) - found
if missing:
    print('not found in src/asm:', ' '.join(sorted(missing)))

for p in list((repo / 'src').rglob('*.cpp')) + list((repo / 'src').rglob('*.h')):
    t = p.read_text(encoding='utf-8')
    n = t
    for label, cname in pairs.items():
        e = re.escape(label)
        n = re.sub(r'ASM_PROC\(' + e + r'\)', f'ASM_FUNC({label}, {cname})', n)
        n = re.sub(r'(ASM_(?:FUNC|VAR)\([^()]*,\s*)' + e + r'\)', lambda m: m[1] + cname + ')', n)
        n = n.replace(f'"{label}"', f'"{cname}"')
    if n != t:
        p.write_text(n, encoding='utf-8', newline='')
        print('updated', p.relative_to(repo))
