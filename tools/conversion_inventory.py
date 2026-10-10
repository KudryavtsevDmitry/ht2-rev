"""Inventory game procedures using original addresses and units.csv.

Library units (MFC, CRT, compression, imports and iostreams) are excluded.
Exception funclets beyond the game range need a separate ownership audit.
"""
import csv
import json
import re
from pathlib import Path

repo = Path(__file__).resolve().parent.parent
units = list(csv.DictReader((repo / 'units.csv').open(newline='')))
source = (repo / 'king.masm').read_text()
ends = {m[1].lower(): m.start() for m in re.finditer(r'(?mi)^(\S+)\s+ENDP\b', source)}
converted = set()
for match in re.finditer(r'(?ms)^IFDEF ASM_\w+\s*\n(.*?)^ELSE\s*\n(.*?)^ENDIF', source):
    if 'EXTERN ' in match[2] and 'TEXTEQU' in match[2]:
        converted.update(re.findall(r'(?m)^((?:FUN_|\$L_)[0-9a-f]{6}) PROC', match[1]))

procedures = []
for match in re.finditer(r'(?m)^((?:FUN_|\$L_)([0-9a-f]{6})) PROC[^\n]*', source):
    address = int(match[2], 16)
    unit = next((u for u in units if int(u['start'], 16) <= address < int(u['end'], 16)), None)
    if not unit or unit['basis'] == 'library':
        continue
    end = ends.get(match[1].lower(), -1)
    if end < 0:
        raise ValueError('Missing ENDP for ' + match[1])
    procedures.append(dict(label=match[1], address=match[2],
                           unit=unit['name'] or ('unit_' + unit['start']),
                           lines=source.count('\n', match.start(), end) + 1,
                           converted=match[1] in converted))

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--json', action='store_true')
    args = parser.parse_args()
    if args.json:
        print(json.dumps(procedures, indent=2))
    else:
        print('Unit                         C++    MASM remaining')
        for name in dict.fromkeys(p['unit'] for p in procedures):
            group = [p for p in procedures if p['unit'] == name]
            done = sum(p['converted'] for p in group)
            print(f'{name:28} {done:5} {len(group) - done:9}')
        done = sum(p['converted'] for p in procedures)
        print(f'Total: {done} converted, {len(procedures) - done} remaining game procedures')
