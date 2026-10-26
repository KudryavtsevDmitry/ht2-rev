"""Parse src/engine/data_image.cpp: which code labels the data names, and
which of those only the original exception tables (FuncInfo) name."""
import re, struct
from pathlib import Path

def load(path='src/engine/data_image.cpp'):
    t = Path(path).read_text(encoding='utf-8')
    kd = dict(re.findall(r'KING_ALIAS\("_(KD\d+)", "([^"]+)"\)', t))
    images = {}
    for name, sname in (('R', 'KingRdataImage'), ('D', 'KingDataImage')):
        sb = t.index('struct ' + sname + ' {')
        fields = re.findall(r'^\s+(?:unsigned char b\d+\[(\d+)\]|const void\* p\d+|void\* p\d+);', t[sb:t.index('};', sb)], re.M)
        var = 'KingRdata' if name == 'R' else 'KingData'
        ib = t.index(var + ' = {', t.index('struct KingDataImage'))
        ie = t.index('\n};', ib)
        entries = [l.strip().rstrip(',') for l in t[ib:ie].split('\n')[1:] if l.strip()]
        assert len(entries) == len(fields), (name, len(entries), len(fields))
        data = bytearray(); ptr = {}
        for f, e in zip(fields, entries):
            if f:
                b = [int(x) for x in e.strip('{}').split(',')]
                assert len(b) == int(f)
                data += bytes(b)
            else:
                m = re.search(r'&(KD\d+)\)', e)
                if m:
                    ptr[len(data)] = ('KD', kd[m[1]])
                else:
                    m = re.search(r'&(KingRdata|KingData|KingBss)(?:\) \+ (\d+)|\s*\+\s*(\d+))?', e)
                    off = int(m[2] or m[3] or 0) if m else 0
                    ptr[len(data)] = ({'KingRdata': 'R', 'KingData': 'D', 'KingBss': 'B'}[m[1]], off) if m else ('?', e)
                data += b'\0\0\0\0'
        images[name] = (data, ptr)
    return images

def eh_tables(images):
    """{FuncInfo rdata offset: set of code labels}, and the set of slots (img, off) in EH tables"""
    out, slots = {}, set()
    for img in ('R',):
        data, ptr = images[img]
        u32 = lambda o: struct.unpack_from('<I', data, o)[0]
        for f in range(0, len(data) - 20, 4):
            if u32(f) != 0x19930520 or f in ptr:
                continue
            labs = set()
            maxs, pu, ntry, pt = u32(f + 4), ptr.get(f + 8), u32(f + 12), ptr.get(f + 16)
            if pu and pu[0] == 'R' and maxs < 1000:
                for i in range(maxs):
                    s = pu[1] + 8 * i + 4
                    if s in ptr and ptr[s][0] == 'KD':
                        labs.add(ptr[s][1]); slots.add(('R', s))
            if pt and pt[0] == 'R' and ntry < 1000:
                for i in range(ntry):
                    tb = pt[1] + 20 * i
                    nc, ph = u32(tb + 12), ptr.get(tb + 16)
                    if ph and ph[0] == 'R':
                        for k in range(nc):
                            s = ph[1] + 16 * k + 12
                            if s in ptr and ptr[s][0] == 'KD':
                                labs.add(ptr[s][1]); slots.add(('R', s))
            out[f] = labs
    return out, slots

if __name__ == '__main__':
    im = load()
    eh, slots = eh_tables(im)
    print('funcinfos', len(eh), 'eh slots', len(slots))
    other = {v[1] for img, (d, p) in im.items() for o, v in p.items() if v[0] == 'KD' and (img, o) not in slots}
    only = {l for s in eh.values() for l in s} - other
    print('labels only in EH tables', len(only))
