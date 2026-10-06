"""Rename labels of king.masm to the names in symbols.csv:
    python applySymbols.py [symbols.csv] [king.masm]

Every use of a label is renamed. The renamed PROC keeps its old label and the
prototype from symbols.csv in a comment:
    _fclose PROC ;FUN_61cb9a int fclose(FILE *stream)
Labels that are already renamed are left alone, so running it again is safe."""
import csv
import re
import sys

IDENT = re.compile(r"[A-Za-z_$?@][A-Za-z0-9_$?@]*")


def split_comment(line):
    """code part and comment part (from the first ; outside quotes)"""
    quote = None
    for i, c in enumerate(line):
        if quote:
            if c == quote:
                quote = None
        elif c in "'\"":
            quote = c
        elif c == ";":
            return line[:i], line[i:]
    return line, ""


def rename_code(code, names):
    """rename identifiers in code, leaving quoted strings alone"""
    parts = re.split(r"('[^']*'|\"[^\"]*\")", code)
    for i in range(0, len(parts), 2):
        parts[i] = IDENT.sub(lambda m: names.get(m.group(0), m.group(0)), parts[i])
    return "".join(parts)


def main():
    csv_path = sys.argv[1] if len(sys.argv) > 1 else "symbols.csv"
    masm_path = sys.argv[2] if len(sys.argv) > 2 else "king.masm"
    with open(csv_path, newline="") as f:
        rows = list(csv.DictReader(f))
    names = {r["label"]: r["name"] for r in rows}
    prototypes = {r["label"]: r["prototype"] for r in rows}

    with open(masm_path, "rb") as f:
        lines = f.read().decode("latin-1").split("\n")

    proc = re.compile(r"^(\S+)(\s+PROC\b.*)$")
    renamed = set()
    procs = set()
    changed = 0
    for i, line in enumerate(lines):
        eol = "\r" if line.endswith("\r") else ""
        body = line[:-1] if eol else line
        code, comment = split_comment(body)
        m = proc.match(code)
        if m:
            procs.add(m.group(1))
        if m and m.group(1) in names:
            old = m.group(1)
            new_code = names[old] + m.group(2).rstrip()
            comment = (";" + old + " " + prototypes[old] + " " + comment).rstrip()
            new = new_code + " " + comment
            renamed.add(old)
        else:
            new = rename_code(code, names) + comment
        if new != body:
            lines[i] = new + eol
            changed += 1

    if changed:
        with open(masm_path, "wb") as f:
            f.write("\n".join(lines).encode("latin-1"))
    missing = sorted(old for old, new in names.items() if old not in renamed and new not in procs)
    print(f"{len(renamed)} procedures renamed, {changed} lines changed")
    if missing:
        print("no PROC with these labels or their names:", " ".join(missing))


if __name__ == "__main__":
    main()
