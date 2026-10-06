"""Find the translation units (object files) of king.exe and write units.csv:
    python findUnits.py [king.masm] [units.csv]

The linker put every object file's code, .data and .bss in one contiguous
piece each, in the same order, so code and the data it uses rise together,
file after file. A boundary between two files is a place where
  1. the code before it uses only data below some address and the code after
     it only data above (counted over a few functions on each side, ignoring
     globals used from far away), and
  2. the .data there is not just one string literal after another: it holds
     the next file's $Id string or other data.
Files are named after their $Id strings (static char rcsid[] = "$Id: ...")
and the __FILE__ paths of their error messages. All functions that use a
file's own $Id or __FILE__ string are kept in one unit. Where the functions
around a boundary use no .data, their .bss and calls decide where it goes,
and the range it could be anywhere in is written to start_range.

The statically linked libraries after the game code (MFC, C runtime, gzip,
iostreams) and the exception-handling funclets after them are listed with
their known ranges. All addresses are those of the original king.exe."""
import bisect
import collections
import csv
import re
import sys

GAME_LO, GAME_HI = 0x401000, 0x61c4c1   # game code; the libraries follow
EH_TAIL = 0x63b610                      # .text$x: exception-handling funclets
D_LO = 0x66c4b8                         # game .data, after the CRT's .CRT$X* tables
D_HI = 0x685e30                         # library .data (MFC's _afxNewHandler) from here
B_LO = 0x68b000                         # .bss (end of the initialized .data)
TEXT_END = 0x64a78a

LIBRARIES = [  # start, end, name
    (0x61c4c1, 0x61cb9a, "MFC 4.2 (CDC and CWnd helpers)"),
    (0x61cb9a, 0x62a910, "C runtime (VC6 LIBCMT)"),
    (0x62a910, 0x6300b0, "gzip (inflate, deflate, unlzw, unpack, unlzh)"),
    (0x6300b0, 0x6300e0, "import thunks (wsock32)"),
    (0x6300e0, 0x6303a0, "DirectInput data formats (dinput.lib)"),
    (0x6303a0, 0x6303d0, "import thunks (dinput, ddraw, msvfw32, dsound)"),
    (0x6303d0, 0x6317ac, "iostreams (VC6 classic iostream library)"),
    (0x6317ac, 0x6317d0, "import thunks (kernel32, user32, winspool)"),
    (0x6317d0, EH_TAIL, "MFC 4.2 (with the C runtime parts it pulls in)"),
    (EH_TAIL, TEXT_END, "exception-handling funclets of all of the above (.text$x)"),
]

ADDR = re.compile(r"(?:FUN_|\$L_)([0-9a-f]{6})")
PROC = re.compile(r"^(\S+)\s+PROC\b(.*)$")
LABEL = re.compile(r"^(\$L_[0-9a-f]{6}|FUN_[0-9a-f]{6})(::)?\s*(.*)$")
REF = re.compile(r"(?<![\w$?@])(\$L_[0-9a-f]{6}|FUN_[0-9a-f]{6}|[_?@][\w$?@]*)(?![\w$?@])")
CALL = re.compile(r"^\s*(?:call|jmp)\s+(\S+)\s*$")


def read_masm(path):
    with open(path, encoding="latin-1") as f:
        return f.read().split("\n")


def parse(lines):
    """functions of .text with their references, and the bytes of _DATA"""
    label_addr = {}
    for line in lines:
        m = PROC.match(line)
        if m:
            a = ADDR.match(m.group(1)) or ADDR.search(m.group(2))   # renamed: old label in the comment
            if a:
                label_addr[m.group(1)] = int(a.group(1), 16)
            continue
        m = LABEL.match(line)
        if m:
            label_addr[m.group(1)] = int(m.group(1)[-6:], 16)

    referenced = collections.Counter()
    for line in lines:
        for m in re.finditer(r"\b(?:call|OFFSET|DWORD|DD)\s+(\$L_[0-9a-f]{6})", line):
            referenced[m.group(1)] += 1

    funcs, cur, seg = [], None, None
    data = {}          # address -> byte value, or None for a pointer
    strings = []       # (address, text) of DB '...' strings in _DATA
    addr = None
    for line in lines:
        if re.match(r"^_\w+ SEGMENT", line):
            seg, addr = line.split()[0], None
            continue
        if re.match(r"^_\w+ ENDS", line):
            seg = None
            continue
        if seg == "_TEXT":
            m = PROC.match(line)
            if m:
                name = m.group(1)
                if not name.startswith("$L_") or referenced[name] or cur is None:
                    cur = {"addr": label_addr.get(name), "refs": [], "calls": []}
                    funcs.append(cur)
                continue
            if cur is None:
                continue
            code = line.split(";")[0]
            m = CALL.match(code)
            if m and m.group(1) in label_addr:
                cur["calls"].append(label_addr[m.group(1)])
            for r in REF.findall(code):
                a = label_addr.get(r)
                if a is not None and a >= 0x64b000:     # .rdata, .data, .bss
                    cur["refs"].append(a)
        elif seg == "_DATA":
            m = LABEL.match(line)
            stmt = line
            if m:
                addr, stmt = int(m.group(1)[-6:], 16), m.group(3)
            stmt = stmt.split(";")[0].strip() if not stmt.strip().startswith("DB '") else stmt.strip()
            if addr is None or not stmt:
                continue
            for item in data_items(stmt):
                if isinstance(item, str):
                    strings.append((addr, item))
                    for c in item.encode("latin-1"):
                        data[addr] = c
                        addr += 1
                elif item is None:
                    for _ in range(4):
                        data[addr] = None
                        addr += 1
                else:
                    data[addr] = item
                    addr += 1
    return funcs, data, strings


def data_items(stmt):
    """BYTE 0xxH, DB 'text'[,...], DB n DUP(0), DWORD label: values, str, None"""
    m = re.match(r"BYTE 0([0-9a-f]{2})H$", stmt)
    if m:
        return [int(m.group(1), 16)]
    m = re.match(r"DB (\d+) DUP\(0\)$", stmt)
    if m:
        return [0] * int(m.group(1))
    if stmt.startswith("DWORD "):
        return [None]
    if stmt.startswith("DB "):
        out = []
        for item in re.findall(r"'(?:[^']|'')*'|[^,]+", stmt[3:]):
            item = item.strip()
            if item.startswith("'"):
                out.append(item[1:-1].replace("''", "'"))
            elif item:
                out.append(int(item.rstrip("hH"), 16 if item[-1] in "hH" else 10))
        return out
    return []


def main():
    masm = sys.argv[1] if len(sys.argv) > 1 else "king.masm"
    out_path = sys.argv[2] if len(sys.argv) > 2 else "units.csv"
    allfuncs, data, strings = parse(read_masm(masm))

    fs = sorted((f for f in allfuncs if f["addr"] and GAME_LO <= f["addr"] < GAME_HI), key=lambda f: f["addr"])
    N = len(fs)
    addr = [f["addr"] for f in fs]
    index = {a: i for i, a in enumerate(addr)}
    lib_used = set()
    for f in allfuncs:
        if f["addr"] and GAME_HI <= f["addr"] < EH_TAIL:
            lib_used.update(f["refs"])
    B_HI = min(r for r in lib_used if r >= B_LO)

    # references that are local: near where the function's own data lies
    def staircase(lo, hi):
        users = collections.defaultdict(set)
        for i, f in enumerate(fs):
            for r in f["refs"]:
                if lo <= r < hi and r not in lib_used:
                    users[r].add(i)
        near = {r for r, u in users.items() if addr[max(u)] - addr[min(u)] <= 0x30000}
        meds = []
        for f in fs:
            rs = sorted(r for r in f["refs"] if r in near)
            meds.append(rs[len(rs) // 2] if rs else None)
        have = [i for i in range(N) if meds[i] is not None]
        vals = [meds[i] for i in have]
        st = [None] * N
        for k, i in enumerate(have):
            win = sorted(vals[max(0, k - 15):k + 16])
            st[i] = win[len(win) // 2]
        for order in (range(N), range(N - 1, -1, -1)):
            last = None
            for i in order:
                if st[i] is None:
                    st[i] = last
                else:
                    last = st[i]
        return st

    std, stb = staircase(D_LO, D_HI), staircase(B_LO, B_HI)
    PD = [sorted({r for r in f["refs"] if D_LO <= r < D_HI and r not in lib_used and abs(r - std[i]) <= 0x1800})
          for i, f in enumerate(fs)]
    PB = [sorted({r for r in f["refs"] if B_LO <= r < B_HI and r not in lib_used and abs(r - stb[i]) <= 0x2000})
          for i, f in enumerate(fs)]

    callees = [set() for _ in range(N)]
    for i, f in enumerate(fs):
        for c in f["calls"]:
            if c in index:
                callees[i].add(index[c])
    callers = [set() for _ in range(N)]
    for i in range(N):
        for j in callees[i]:
            callers[j].add(i)

    # names: $Id strings of .cpp files, __FILE__ paths of .cpp files
    cpp_id = {}
    for a, t in strings:
        m = re.match(r"\$Id: (\S+\.cpp)\b", t, re.I)
        if m and D_LO <= a < D_HI:
            cpp_id[a] = m.group(1)
    cpp_path = {a: t for a, t in strings if re.match(r"[A-Za-z]:\\.*\.cpp$", t, re.I)}
    id_addrs = sorted(cpp_id)
    anchor_users = collections.defaultdict(list)
    for i, f in enumerate(fs):
        for r in set(f["refs"]):
            if r in cpp_path or r in cpp_id:
                anchor_users[r].append(i)
    glue = [(min(u), max(u)) for u in anchor_users.values()]

    def glued(cut):
        return any(lo < cut <= hi for lo, hi in glue)

    TEXTY = set(range(0x20, 0x7f)) | {0xa8, 0xb8} | set(range(0xc0, 0x100)) | {9, 10, 13}

    def gap_evidence(lo, hi):
        """('id', addr) if .data [lo, hi) holds a .cpp $Id, ('data', addr) if
        anything but 4-aligned text, else (None, None)"""
        k = bisect.bisect_left(id_addrs, lo)
        if k < len(id_addrs) and id_addrs[k] < hi:
            return "id", id_addrs[k]
        a = lo
        while a < hi:
            b = data.get(a, 0)
            if b == 0:
                a += 1
                continue
            start = a
            while a < hi and data.get(a, 0) != 0:
                if data.get(a) is None or data[a] not in TEXTY:
                    return "data", a
                a += 1
            if start % 4:
                return "data", start
        return None, None

    def best_split(L, R):
        L, R = sorted(L), sorted(R)
        best = (len(L) + len(R) + 1, None)
        for d in sorted(set(L) | set(R)):
            v = (len(L) - bisect.bisect_right(L, d)) + bisect.bisect_right(R, d)
            if v < best[0]:
                best = (v, d)
        return best

    # 1. boundary candidates between neighbouring functions that use .data
    K = 6
    anch = [i for i in range(N) if PD[i]]
    groups = collections.defaultdict(list)
    for t in range(1, len(anch)):
        a, b = anch[t - 1], anch[t]
        L = [r for i in anch[max(0, t - K):t] for r in PD[i]]
        R = [r for i in anch[t:t + K] for r in PD[i]]
        v, dstar = best_split(L, R)
        LB = [r for i in range(max(0, a - 2 * K), a + 1) for r in PB[i]]
        RB = [r for i in range(b, min(N, b + 2 * K)) for r in PB[i]]
        vb = best_split(LB, RB)[0] if LB and RB else 0
        lmax = max([r for r in L if r <= dstar], default=None)
        rmin = min([r for r in R if r > dstar], default=None)
        if lmax is None or rmin is None or v > 2:
            continue
        lo = lmax
        while data.get(lo, 0) != 0:      # skip the rest of the left item
            lo += 1
        ev, at = gap_evidence(lo, rmin + 1)
        if ev and not all(glued(c) for c in range(a + 1, b + 1)):
            groups[at & ~3].append((v + vb, b - a, a + 1, b))    # objects start 4-aligned
    bounds = []
    for at, cs in sorted(groups.items()):
        v, _, lo, hi = min(cs)
        bounds.append({"data": at, "lo": lo, "hi": hi, "v": v})
    bounds.sort(key=lambda u: (u["lo"], u["data"]))
    kept = []
    for u in bounds:
        if kept and u["lo"] < kept[-1]["hi"]:
            if u["v"] < kept[-1]["v"]:
                kept[-1] = u
            continue
        if kept and u["data"] <= kept[-1]["data"]:
            continue
        kept.append(u)

    def place(lo, hi):
        """cut in [lo, hi] (first function of the new unit), by .bss and calls"""
        if hi <= lo:
            return lo
        left, right = set(range(max(0, lo - 30), lo)), set(range(hi, min(N, hi + 30)))
        lb = {r for i in left for r in PB[i]}
        rb = {r for i in right for r in PB[i]}
        score = []
        for f in range(lo, hi):
            nb = callees[f] | callers[f]
            score.append(sum(r in lb for r in PB[f]) - sum(r in rb for r in PB[f])
                         + sum(g in left for g in nb) - sum(g in right for g in nb))
        total, acc, best, cut = sum(score), 0, None, None
        for k in range(len(score) + 1):
            val = 2 * acc - total
            if not glued(lo + k) and (best is None or val > best):
                best, cut = val, lo + k
            if k < len(score):
                acc += score[k]
        return hi if cut is None else cut

    def cut_for(b, lo, hi):
        """cuts in [lo, hi] with the fewest references on the wrong side of b"""
        above = [sum(r >= b for r in PD[i]) for i in range(lo, hi)]
        below = [sum(r < b for r in PD[i]) for i in range(lo, hi)]
        best, cuts, lbad, rbad = None, [], 0, sum(below)
        for k in range(hi - lo + 1):
            if best is None or lbad + rbad < best:
                best, cuts = lbad + rbad, [lo + k]
            elif lbad + rbad == best:
                cuts.append(lo + k)
            if k < hi - lo:
                lbad += above[k]
                rbad -= below[k]
        return cuts[0], cuts[-1]

    # 2. units; a second .cpp $Id in a unit's .data starts one more unit
    units = [{"data": D_LO, "lo": 0, "hi": 0}] + kept
    for u in units:
        u["cut"] = place(u["lo"], u["hi"])
    extra = []
    for k, u in enumerate(units):
        dend = units[k + 1]["data"] if k + 1 < len(units) else D_HI
        end = units[k + 1]["cut"] if k + 1 < len(units) else N
        ids = [a for a in id_addrs if u["data"] <= a < dend]
        u["id"] = ids[0] if ids else None
        for a in ids[1:]:
            lo, hi = cut_for(a, u["cut"], end)
            extra.append({"data": a, "lo": lo, "hi": hi, "id": a, "cut": place(lo, hi)})
    units = sorted(units + extra, key=lambda u: (u["cut"], u["data"]))

    # 3. a unit using the __FILE__ paths of several .cpp files is several units:
    # split between the glued ranges of consecutive files
    path_glue = sorted((min(u), max(u), cpp_path[r]) for r, u in anchor_users.items() if r in cpp_path)
    extra = []
    for k, u in enumerate(units):
        end = units[k + 1]["cut"] if k + 1 < len(units) else N
        inside = [g for g in path_glue if u["cut"] <= g[0] < end]
        prev = u["data"]
        for (g0, g1, p), (h0, h1, q) in zip(inside, inside[1:]):
            if p.lower() != q.lower() and g1 < h0:
                # first .data of the file's own functions above the previous unit's
                # (header strings they share can lie lower)
                d = min((r for i in range(h0, h1 + 1) for r in PD[i] if r > prev), default=prev + 1)
                extra.append({"data": d, "lo": g1 + 1, "hi": h0, "id": None, "cut": place(g1 + 1, h0)})
                prev = d
    units = sorted(units + extra, key=lambda u: (u["cut"], u["data"]))

    rows = []
    for k, u in enumerate(units):
        end = max(units[k + 1]["cut"] if k + 1 < len(units) else N, u["cut"])
        path = next((cpp_path[r] for i in range(u["cut"], end) for r in fs[i]["refs"] if r in cpp_path), "")
        name = cpp_id[u["id"]] if u["id"] else path.split("\\")[-1]
        basis = "$Id" if u["id"] else "__FILE__" if path else "data"
        if rows and basis == "data" and end - u["cut"] < 3:     # fragment: join the unit before
            rows[-1]["end"] = addr[end] if end < N else GAME_HI
            continue
        at = lambda i: addr[i] if i < N else GAME_HI
        rows.append({"start": at(u["cut"]), "end": at(end), "name": name,
                     "dir": path.rsplit("\\", 1)[0] if path else "", "basis": basis,
                     "start_range": "" if u["lo"] == u["hi"] else f"{at(u['lo']):06x}-{at(u['hi']):06x}",
                     "data": f"{u['data']:06x}"})
    named = sum(1 for r in rows if r["name"])
    for lo, hi, name in LIBRARIES:
        rows.append({"start": lo, "end": hi, "name": name, "dir": "", "basis": "library",
                     "start_range": "", "data": ""})

    with open(out_path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["start", "end", "name", "dir", "basis", "start_range", "data"])
        for r in rows:
            w.writerow([f"{r['start']:06x}", f"{r['end']:06x}", r["name"], r["dir"], r["basis"],
                        r["start_range"], r["data"]])
    print(f"wrote {out_path}: {len(rows) - len(LIBRARIES)} units of game code ({named} named), "
          f"{len(LIBRARIES)} library ranges")


if __name__ == "__main__":
    main()
