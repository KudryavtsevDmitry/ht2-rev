"""Extract the resources (.rsrc) of the game's original king.exe into a .res
file so the reassembled king.exe can be linked with the same icons, cursor,
bitmaps and strings:  python extractResources.py <game's king.exe> king.res
then add king.res to the link command."""
import struct
import sys
import pefile


def res_id(entry):
    """name or ordinal of a resource directory entry, encoded for a .res header"""
    if entry.name is not None:
        return str(entry.name).encode("utf-16-le") + b"\0\0"
    return struct.pack("<HH", 0xFFFF, entry.id)


def pad4(data):
    return data + b"\0" * (-len(data) % 4)


def res_entry(type_id, name_id, lang, data, flags=0x1030):
    header = type_id + name_id
    header = pad4(header)
    # DataVersion, MemoryFlags (default MOVEABLE|PURE|DISCARDABLE), LanguageId, Version, Characteristics
    header += struct.pack("<IHHII", 0, flags, lang, 0, 0)
    header = struct.pack("<II", len(data), 8 + len(header)) + header
    return header + pad4(data)


def main():
    if len(sys.argv) != 3 or not sys.argv[1]:
        sys.exit("usage: python extractResources.py <game's king.exe> <output .res>")
    input_file, output_file = sys.argv[1:]
    pe = pefile.PE(input_file)
    out = [res_entry(struct.pack("<HH", 0xFFFF, 0), struct.pack("<HH", 0xFFFF, 0), 0, b"", flags=0)]
    count = 0
    for type_entry in pe.DIRECTORY_ENTRY_RESOURCE.entries:
        for name_entry in type_entry.directory.entries:
            for lang_entry in name_entry.directory.entries:
                d = lang_entry.data.struct
                data = pe.get_data(d.OffsetToData, d.Size)
                out.append(res_entry(res_id(type_entry), res_id(name_entry), lang_entry.id, data))
                count += 1
    with open(output_file, "wb") as f:
        f.write(b"".join(out))
    print("wrote", output_file, "with", count, "resources")


if __name__ == "__main__":
    main()
