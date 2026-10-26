# ht2-rev

Reverse engineering of `king.exe` from **Hard Truck 2: King of the Road** (*Дальнобойщики 2*), version `v 8.0 30.01.03`.

`king.exe` is built from C++ (`src/`) and a little assembly (`src/asm/`). C++ owns all the program's data. The game code is C++ except the software renderer's hand-written span loops (`src/asm/asm_unit_5fdc70.asm`); a few procedures whose exact machine code matters are kept as inline `__asm` in C++ (`src/truck/x87_code.cpp`). The rest of `src/asm` is library code: the old C runtime's helpers, gzip and exception funclets. The statically linked MFC 4.2 is replaced by own code with the same layouts (`src/engine/mfc.cpp`; only two throw helpers and `_AfxThreadEntry` are still assembly), and the game runs on the modern static C runtime instead of its 1999 one. See `CONVERSION_LOG.md`.

The program started as a full disassembly, `king.masm`, which assembled back into a byte-identical `king.exe` and had a switch per source file to put the original assembly back. It is deleted; the tag `last-masm` is the last commit that has it (`nmake ASM=1` there builds the original program).

The game is not included: building needs your own original `king.exe` (for resources), running needs an installed game.

## Layout

| Path | What it is |
|---|---|
| `src/` | the C++ rewrite; folders follow the original libraries (`engine`, `ai`, `script`, `sound`, `truck`, `terrain`, `marine`), file names follow the original units in `units.csv` |
| `src/asm/` | the assembly not yet rewritten: `asm_<unit>.asm` per original unit, `king.inc` (common header), `objects.mak` (the list of objects) |
| `src/truck/x87_code.cpp` | procedures kept as the original machine code in naked functions (the helicopter step, the trailer hitch solver, a player lookup that reads leftover stack, the rope compare) |
| `src/engine/data_image.cpp`, `data.cpp` | the program's data: `_RDATA` and `_DATA` as byte images with real pointers, `_BSS` |
| `src/engine/crt.cpp` | `WinMain` and the shims between the old runtime's callers and the modern one |
| `src/common/king.h` | glue between C++ and the assembly |
| `src/common/x87.h` | x87 floating point as VC6 generated it |
| `units.csv` | the object files the program was linked from |
| `symbols.csv` | library function names |
| `extractResources.py` | copies the resources of the original `king.exe` into `king.res` |
| `tools/` | `smoke_world.py` (world-loading smoke test), `conversion_inventory.py` (game procedures left in `src/asm`), `asm_replace.py` and `asm_dead.py` (take converted and unreachable code out of `src/asm`) |

## Building

Needs VS 2022 Build Tools with the v142 (14.29) x86 toolset (`cl`, `ml`, `link`), Windows 10 SDK, the DirectX 7 SDK `lib` folder and Python with `pefile`. Paths are set in the `Makefile`.

```
nmake "GAME_EXE=C:\path\to\king.exe"   # first build (creates king.res)
nmake                                  # king.exe
nmake clean
```

## Running

Run `king.exe` with the game folder as working directory, e.g. through [D2GI](https://github.com/REDPOWAR/D2GI) in windowed mode:

```powershell
Start-Process C:\test\king.exe -WorkingDirectory 'C:\Program Files (x86)\King'
```

To reach the world click **ИГРА**, then **НОВАЯ ИГРА** (real mouse input only), or run `py tools\smoke_world.py` at the main menu.

## Next: no assembly

Run the drive test after every step.

1. **Static initializers.** `WinMain` (`crt.cpp`) still runs the old initializer table `$L_66c000`..`$L_66c488`. Replace it with a C++ list of the same init functions in the same order, then with C++ global objects where the order doesn't matter.
2. **Rest of MFC.** `AfxThrowMemoryException`, `AfxThrowResourceException` and `_AfxThreadEntry` become C++. Exceptions are always fatal, so the throw helpers can just report and exit.
3. **The remaining procedures.** *(Game code done except the span loops below.)* Left: gzip (zlib or a translation), the old runtime's helpers (`__CIpow`, `_ftol`, `_alldiv`, `_chkstk`, string functions: the modern CRT or `x87.h`) and the last exception funclets.
4. **The last assembly.** What is not worth rewriting becomes inline `__asm` inside C++ functions: `py tools\asm_inline.py FILE ENTRY CNAME OUT.cpp` turns a single-entry procedure into a naked function (done for the copter step, the hitch solver, `FUN_477b60`, `FUN_45a7d0`). The span loops (`asm_unit_5fdc70`) need more: their four entry points store each other's label addresses, which inline `__asm` can't share between functions. Then `src/asm`, `king.inc`, `objects.mak` and the `ml` rule go, and the build is `cl` and `link` only.
5. **Typed data.** Replace the raw images piece by piece with real C++ definitions: globals as typed variables (a struct when code reaches neighbouring labels by offset), strings, float/double constants with the same bit patterns, lookup tables, vtables as arrays of C++ functions. Then `KING_VAR` and the fixed addresses go.

## Replacing an assembly procedure

1. Write it in C++ and export it with the asm calling convention (`__fastcall` with an unused EDX argument for thiscall).
2. Run `py tools\asm_replace.py LABEL=CNAME ...` with the C++ linker name (`_Name`, `_Name@8` stdcall, `@Name@8` fastcall). It deletes the label's code from `src/asm`, points the other assembly (`EXTERN`), the data image's code pointers and the `ASM_PROC`/`ASM_FUNC`/`ASM_VAR` uses in C++ at the new name, and removes files left without code.
3. Run `py tools\asm_dead.py --apply`. It removes what only the deleted code reached, including catch and unwind handlers whose exception tables nothing installs any more.
4. Assembly still called from C++ is reached with `ASM_PROC` and `CallC`, `CallStd`, `CallThis`, `CallVirt` (`king.h`); data with `KING_VAR(type, name, address)`.

C++ is built with `/Zl /GS- /GR- /EHs-c-` and `/arch:IA32`; the `Makefile` names all libraries (`LIBS`, and the modern static runtime in `CRTLIBS`). The old VC6 runtime's entry points (heap, stdio, exit, errno, threads, exception handling) point at the modern runtime or at the shims in `src/engine/crt.cpp`. Converted code must not look inside a `FILE` (use `FILE_EOF`, `CRT_STREAM` from `king.h`).
