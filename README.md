# ht2-rev

Reverse engineering of `king.exe` from **Hard Truck 2: King of the Road** (*Дальнобойщики 2*), version `v 8.0 30.01.03`.

`king.masm` is the whole program as MASM source and assembles back into a working `king.exe`. Functions are rewritten in C++ (`src/`) one by one and linked in place of their assembly, so the game runs at every step. About 97% of the game code (by size) is C++ now. The statically linked MFC 4.2 is replaced by own code with the same layouts (`src/engine/mfc.cpp`; only two throw helpers and `_AfxThreadEntry` are still assembly), and the game runs on the modern static C runtime instead of its 1999 one (see below). See `CONVERSION_LOG.md`.

The game is not included: building needs your own original `king.exe` (for resources), running needs an installed game.

## Layout

| Path | What it is |
|---|---|
| `king.masm` | the program as MASM source |
| `src/` | the C++ rewrite; folders follow the original libraries (`engine`, `ai`, `script`, `sound`, `truck`, `terrain`, `marine`), file names follow the original units in `units.csv` |
| `src/common/king.h` | glue between C++ and `king.masm` |
| `src/common/x87.h` | x87 floating point as VC6 generated it |
| `units.csv` | the object files the program was linked from |
| `symbols.csv`, `applySymbols.py` | library function names, and the script that applies them to `king.masm` |
| `extractResources.py` | copies the resources of the original `king.exe` into `king.res` |
| `tools/` | `objdiff.py` (compare two `king.obj`), `smoke_world.py` (world-loading smoke test), `conversion_inventory.py` |

## Building

Needs VS 2022 Build Tools with the v142 (14.29) x86 toolset, Windows 10 SDK, the DirectX 7 SDK `lib` folder and Python with `pefile`. Paths are set in the `Makefile`.

```
nmake "GAME_EXE=C:\path\to\king.exe"   # first build (creates king.res)
nmake                                  # king.exe with the C++ code
nmake ASM=1                            # the original program, no C++
nmake ASM_MAIN_LOOP=1                  # original assembly for one source file
nmake clean
```

Every `src` file has a switch `ASM_<FILE NAME>`, and `ASM_CRT=1` switches back to the game's own C runtime; procedures the default build never reaches are under `IFDEF ASM_UNUSED` (any switch assembles them again, the default build points their labels at an `int 3` trap); run `nmake clean` before changing switches. `ASM=1` is byte-identical to the original apart from timestamps.

## Running

Run `king.exe` with the game folder as working directory, e.g. through [D2GI](https://github.com/REDPOWAR/D2GI) in windowed mode:

```powershell
Start-Process C:\test\king.exe -WorkingDirectory 'C:\Program Files (x86)\King'
```

To reach the world click **ИГРА**, then **НОВАЯ ИГРА** (real mouse input only), or run `py tools\smoke_world.py` at the main menu.

## Replacing an assembly procedure

1. Write it in C++ and export it with the asm calling convention (`__fastcall` with an unused EDX argument for thiscall).
2. In `king.masm` keep the original under the file's switch and point the label at the export:
   ```asm
   IFDEF ASM_MAIN_LOOP
   $L_5dd440 PROC
       ...
   $L_5dd440 ENDP
   ELSE
   EXTERN @CGameApp_OnIdle@12:PROC
   $L_5dd440 TEXTEQU <@CGameApp_OnIdle@12>
   ENDIF
   ```
   Labels inside the procedure that are used from outside need their own `TEXTEQU`.
3. Assembly still called from C++ is reached with `ASM_PROC` / `ASM_VAR` and `CallC`, `CallStd`, `CallThis`, `CallVirt` (`king.h`). Data labels need a `PUBLIC` line in `king.masm`.

C++ is built with `/Zl /GS- /GR- /EHs-c-`, `/arch:IA32` and names its libraries itself. king.exe runs on the modern static C runtime (libcmt/libucrt/libvcruntime): `king.masm` points the old VC6 runtime's entry points (heap, stdio, exit, errno, threads, exception handling) at it or at the shims in `src/engine/crt.cpp`, which also holds `WinMain` (the old runtime's C++ initializer table, then `AfxWinMain`). The old runtime's x87 math, `_ftol` and string helpers stay in assembly. `nmake ASM_CRT=1` builds on the game's own runtime again. Converted code must not look inside a `FILE` (use `FILE_EOF`, `CRT_STREAM` from `king.h`).
