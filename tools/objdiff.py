"""objdiff.py - compares what two COFF objects (king.obj from different
versions of king.masm) put into the executable.

  py tools\\objdiff.py a.obj b.obj

Section by section it compares the bytes and where each relocation points,
as section + offset. Symbol names and whether a label is a procedure, a
public label or a local one don't matter, so a change to PROC/ENDP lines or
label names alone shows no difference; a change to code or data does. Only
the standard library is used.
"""
import struct
import sys

IMAGE_SCN_LNK_NRELOC_OVFL = 0x01000000
RELOC_SIZE = {0x06: 4, 0x07: 4, 0x14: 4, 0x0B: 4, 0x0A: 2, 0x01: 2, 0x02: 2}   # x86 DIR32, DIR32NB, REL32, SECREL, ...


class Coff:
    def __init__(self, path):
        with open(path, "rb") as f:
            self.data = data = f.read()
        (machine, nsec, _, symptr, nsym, opt, _) = struct.unpack_from("<HHIIIHH", data, 0)
        if machine != 0x14C:
            raise SystemExit("%s: not an x86 COFF object" % path)
        strtab = symptr + 18 * nsym
        self.symbols = []
        i = 0
        while i < nsym:
            raw = data[symptr + 18 * i: symptr + 18 * i + 18]
            name, value, sec, typ, cls, aux = struct.unpack("<8sIhHBB", raw)
            if name[:4] == b"\0\0\0\0":
                off = struct.unpack_from("<I", name, 4)[0]
                end = data.index(b"\0", strtab + off)
                name = data[strtab + off:end]
            else:
                name = name.rstrip(b"\0")
            self.symbols.append((name.decode("latin-1"), value, sec, cls))
            self.symbols.extend([None] * aux)
            i += 1 + aux
        self.sections = []
        for s in range(nsec):
            h = struct.unpack_from("<8sIIIIIIHHI", data, 20 + opt + 40 * s)
            name = h[0].rstrip(b"\0").decode("latin-1")
            size, rawptr, relptr, nrel, flags = h[3], h[4], h[5], h[7], h[9]
            relocs = []
            if nrel == 0xFFFF and flags & IMAGE_SCN_LNK_NRELOC_OVFL:
                nrel = struct.unpack_from("<I", data, relptr)[0]
                first = 1
            else:
                first = 0
            for r in range(first, nrel):
                va, symidx, typ = struct.unpack_from("<IIH", data, relptr + 10 * r)
                relocs.append((va, symidx, typ))
            raw = data[rawptr:rawptr + size] if rawptr else b"\0" * size
            self.sections.append((name, raw, relocs, flags))

    def target(self, symidx, addend):
        """where a relocation points: (section name, offset) or (symbol,)"""
        name, value, sec, cls = self.symbols[symidx]
        if sec > 0:
            return (self.sections[sec - 1][0], value + addend)
        return (name, addend)


def compare(a, b, limit=20):
    out = []
    if [s[0] for s in a.sections] != [s[0] for s in b.sections]:
        out.append("sections differ: %s / %s" % ([s[0] for s in a.sections], [s[0] for s in b.sections]))
        return out
    for (name, raw_a, rel_a, _), (_, raw_b, rel_b, _) in zip(a.sections, b.sections):
        if name.startswith(".debug") or name == ".drectve":
            continue
        if len(raw_a) != len(raw_b):
            out.append("%s: %d bytes / %d bytes" % (name, len(raw_a), len(raw_b)))
            continue
        ma, mb = bytearray(raw_a), bytearray(raw_b)
        ta, tb = {}, {}
        for coff, raw, rels, mask, targets in ((a, raw_a, rel_a, ma, ta), (b, raw_b, rel_b, mb, tb)):
            for va, symidx, typ in rels:
                n = RELOC_SIZE.get(typ, 4)
                addend = int.from_bytes(raw[va:va + n], "little", signed=True)
                targets[va] = (typ, coff.target(symidx, addend))
                mask[va:va + n] = b"\0" * n
        diffs = [] if ma == mb else [i for i in range(len(ma)) if ma[i] != mb[i]]
        if diffs:
            out.append("%s: %d bytes differ, first at +0x%x" % (name, len(diffs), diffs[0]))
            for i in diffs[:limit]:
                out.append("  +0x%x: %02x / %02x" % (i, ma[i], mb[i]))
        for va in sorted(set(ta) | set(tb)):
            if ta.get(va) != tb.get(va):
                out.append("%s +0x%x: relocation %s / %s" % (name, va, ta.get(va), tb.get(va)))
                limit -= 1
                if limit <= 0:
                    out.append("  ...")
                    break
    return out


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    diffs = compare(Coff(argv[1]), Coff(argv[2]))
    for d in diffs:
        print(d)
    print("same code and data" if not diffs else "%d differences" % len(diffs))
    return 1 if diffs else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))