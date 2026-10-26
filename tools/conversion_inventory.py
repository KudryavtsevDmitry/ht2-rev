"""Inventory the game procedures still in assembly (src/asm/*.asm).

Library units (MFC, CRT, compression, imports and iostreams) are excluded,
using the original addresses and units.csv.
"""
import csv
import json
import re
from pathlib import Path

repo = Path(__file__).resolve().parent.parent
units = list(csv.DictReader((repo / 'units.csv').open(newline='')))

procedures = []
for path in sorted((repo / 'src' / 'asm').glob('*.asm')):
    source = path.read_text()
    ends = {m[1].lower(): m.start() for m in re.finditer(r'(?mi)^(\S+)\s+ENDP\b', source)}
    for match in re.finditer(r'(?m)^((?:FUN_|\$L_)([0-9a-f]{6})) PROC[^\n]*', source):
        address = int(match[2], 16)
        unit = next((u for u in units if int(u['start'], 16) <= address < int(u['end'], 16)), None)
        if not unit or unit['basis'] == 'library':
            continue
        end = ends.get(match[1].lower(), -1)
        if end < 0:
            raise ValueError('Missing ENDP for ' + match[1])
        procedures.append(dict(label=match[1], address=match[2], file=path.name,
                               unit=unit['name'] or ('unit_' + unit['start']),
                               lines=source.count('\n', match.start(), end) + 1))

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--json', action='store_true')
    args = parser.parse_args()
    if args.json:
        print(json.dumps(procedures, indent=2))
    else:
        print('Unit                         procs     lines')
        for name in dict.fromkeys(p['unit'] for p in procedures):
            group = [p for p in procedures if p['unit'] == name]
            print(f'{name:28} {len(group):5} {sum(p["lines"] for p in group):9}')
        print(f'Total: {len(procedures)} game procedures in assembly, '
              f'{sum(p["lines"] for p in procedures)} lines')
