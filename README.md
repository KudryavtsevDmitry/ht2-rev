# ht2-rev

Reverse engineering of `king.exe` from **Hard Truck 2: King of the Road** (*Дальнобойщики 2*), the build with the version string `v 8.0 30.01.03` (PE timestamp 2004-01-14, linked with Visual C++ 6).

`king.masm` is the whole program as MASM source. It assembles back into a `king.exe` that runs the game, so the program can be rewritten in C++ piece by piece: a rewritten function is linked into the same executable in place of its assembly, and the game keeps working at every step. The first piece done this way is the main loop (`src/mainloop.cpp`).

## Contents

| Path | What it is |
|---|---|
| `king.masm` | the program as MASM source, made from a disassembly of the game's `king.exe` (~1.1 million lines) |
| `extractResources.py` | copies the resources of the game's `king.exe` (icons, cursor, bitmaps, strings) into `king.res` |
| `symbols.csv` | names, original addresses and prototypes of the library functions the game calls (C runtime, iostreams, MFC) |
| `applySymbols.py` | renames the labels of `king.masm` after `symbols.csv` |
| `units.csv` | the source files (object files) the program was linked from: code ranges, names, `.data` starts |
| `src/king.h` | glue between the C++ code and `king.masm`, partial layouts of the engine's classes |
| `src/mainloop.cpp` | the main loop in C++ |
| `src/compile_flags.txt` | makes clangd check `src/` as 32-bit MSVC code |
| `tools/objdiff.py` | compares two `king.obj`: whether a change to `king.masm` changed code or data |
| `Makefile` | NMAKE build |

The game itself is not included. Building needs your own copy of its original `king.exe` for the resources, and running needs an installed game.

## Building

Requirements (their paths are set in the `Makefile`; change them for another machine):

- Visual Studio 2022 Build Tools with the MSVC 14.29.30133 (v142) toolset: x86 `ml.exe`, `cl.exe`, `link.exe`, and `nmake`
- Windows 10 SDK 10.0.26100.0: headers and the `um\x86` import libraries
- DirectX 7 SDK, `lib` directory (DirectInput, DirectDraw and DirectSound import libraries)
- Python with `pefile`, started as `py` (`nmake PYTHON=python` for another command)
- the game's original `king.exe`, for the first build

The `Makefile` passes all tool, include and library paths itself, so only `nmake` has to be on `PATH`; no Developer Command Prompt is needed.

```
nmake "GAME_EXE=C:\path\to\king.exe"   # first build: resources from the game's king.exe
nmake                                  # king.exe with the C++ main loop
nmake ASM_MAINLOOP=1                   # king.exe with the original assembly main loop
nmake clean                            # delete king.obj, mainloop.obj, king.exe
```

`GAME_EXE` is needed only while `king.res` doesn't exist; `nmake clean` keeps it. `GAME_EXE` can also be set as an environment variable.

Run `nmake clean` before switching between the two variants; nmake doesn't notice the changed flag. Assembling `king.masm` takes about 25 seconds.

The `ASM_MAINLOOP=1` build is byte-identical to the build before C++ was added, except for timestamps and the checksum. Use it as the reference when something behaves differently.

The executable is linked like the original: fixed base `0x400000` without relocations (`/FIXED`), the original's DLL characteristics (`/NXCOMPAT:NO /TSAWARE:NO`), and the CRT startup in `king.masm` as entry point (`__EntryPoint`).

## Running

`king.exe` needs the game's data: copy it into an installed game folder, or start it with that folder as working directory. The game reads `TRUCK.INI` from the working directory and finds everything else through it.

Here the game runs through [D2GI](https://github.com/REDPOWAR/D2GI), a DirectDraw → Direct3D 9 wrapper made for this game (`ddraw.dll` and `d2gi.ini` in the game folder). D2GI patches the same places in builds whose code sits at different addresses, so rebuilt executables work with it.

To test a build without touching the installed game, put `king.exe`, D2GI's `ddraw.dll` and a `d2gi.ini` with `WindowMode = windowed` into a separate folder, and start `king.exe` from there with the game folder as working directory. D2GI reads its ini and writes `d2gi.log` next to the exe; the game takes its data from the working directory:

```powershell
Start-Process C:\test\king.exe -WorkingDirectory 'C:\Program Files (x86)\King'
```

**Known issue.** When a script started the game and then just waited, loading the world ("Загрузка игрового мира…") stalled every time: the window stayed "Not Responding" and closing it ended in an access violation. When the script polled the window once a second (`Process.Responding`), the world loaded in about 7 seconds every time. Both builds, C++ and assembly main loop, behaved the same.

## How the program is organised

`king.exe` is an MFC application. MFC and the C runtime are linked statically, so they are part of `king.masm` as well. The game's own code fills `0x401000`–`0x61c4c0` of the original; MFC, the C runtime, gzip and the old iostream library follow, and the exception-handling funclets of all of it close the code section (`0x63b610`–`0x64a78a`).

- **Startup:** `__EntryPoint` (CRT) → `_WinMain@16` → MFC's `AfxWinMain`, which calls `InitInstance` and then `Run` of the application object.
- **Application:** the object is `$L_696750`, a pointer to it is in `$L_721750`. Its class chain is `CWinApp` → engine application (vtable `$L_652664`) → an intermediate class (`$L_65195c`) → the game's `CKingApp` (`$L_64d830`).
- **Windows:** the engine's "windows" are rectangles of the game screen (3D view, HUD panels, menus), not Win32 windows. The application keeps them in lists; each one draws into a surface.
- **Input:** the message handlers of the Win32 window put mouse and keyboard messages into a 100-entry queue (`$L_702660`, `FUN_5acb60`), and the frame takes them out (`FUN_5aca90`).
- **Time:** `FUN_6037e0` returns milliseconds (QueryPerformanceCounter, scaled against `timeGetTime` at startup).
- **Crash trace:** `taskdump=1` in the `[ENV]` section of `TRUCK.INI` makes every frame write its stages (`BEGIN FRAME`, `BEGIN EXECUTE`, `END EXECUTE`, `END FRAME`) into a memory-mapped `state.dump`, so after a hang it shows how far the last frame got.

### Source files

`units.csv` divides the code into the object files it was linked from, in link order. Their names come from the developers' `$Id` strings (`static char rcsid[] = "$Id: ai_mult.cpp 2000/12/21 20:12:14"`) and from the `__FILE__` paths in error messages:

- `C:\Nek\Vrappl\Gi\Htai\`: the AI, `Ai_main.cpp` (a third of a megabyte of code) to `ai_trce.cpp`, at the start of the code
- `sch_embd.cpp`, `Sch_eval.cpp`, `sch_istr.cpp`, `sch_item.cpp`: an embedded Scheme interpreter
- `car2e.cpp`, `caran.cpp`, `carat.cpp`, `carvstr.cpp`, `carvvk.cpp`, `copter.cpp`, `movingb.cpp`: vehicles
- `C:\Nek\VRAPPLVS\TRUCK\`: `aeffects.cpp`, `mmt.cpp`, `wasm.cpp`
- `C:\Nek\Vrappl\Gi\Srmidp\D1gate.cpp`, `rdtsc.cpp`
- `C:\Nek\VRAPPLVS\marine\` and `terrain\`: at the end of the game code

Within a group the files are in alphabetical order, so an unnamed unit's name sorts between its neighbours' (`aeffects.cpp` … `car2e.cpp` … `wasm.cpp` look like one folder). 31 of the 55 units have a name; the others come from the layout of the data alone, and such a unit can also be two files whose boundary the data doesn't show. Where the functions around a boundary use no `.data`, `start_range` gives the range the boundary can lie in and `start` is the best guess within it.

`units.csv` was made from `king.masm` alone, by `findUnits.py` (removed since; it is in commit `a1868b5`). Every object file put its code, its `.data` and its `.bss` into one contiguous piece each, in the same order, so code and the data it uses rise together, file after file. A boundary is a place where the code before it uses only data below some address and the code after it only data above, and where the `.data` there is more than one string literal after another (the next file's `$Id`, or other data). All functions that use a file's own `$Id` or `__FILE__` string stay in one unit.

### Main loop

Everything in this chain is C++ now (`src/mainloop.cpp`):

```
CWinApp::Run               [$L_63267d]  MFC message loop; calls OnIdle whenever no message is waiting
 └ OnIdle        vtbl+96   [$L_5dd440]  runs one frame and returns TRUE, so it is called again
    └ Frame      vtbl+168  [$L_569650]  CKingApp::Frame (the engine's own: [$L_5e1af0])
       └ RunFrame vtbl+164 [$L_5e2040]  the frame itself
```

A frame, `CGameApp::RunFrame`:

1. Unless the game is paused, the screen is prepared and cleared, `vtbl+172` runs (`CKingApp`: `$L_4e0d50`), and every visible window gets `vtbl+60` (selects its surface) and `vtbl+48` (its work for the frame), with timings. Then, paused or not, `vtbl+176` runs.
2. Input from the queue: mouse events go to the window under the mouse (to the input window while the mouse is captured), everything else to the input window through `vtbl+192`. The Pause key pauses the game and the joystick's force feedback; Alt+F4 quits through `vtbl+180`.
3. The mouse cursor is drawn: the cursor image of the window under the mouse, or a crosshair.
4. The frame is presented, windows closed during the frame are deleted (10-slot queue), and a requested video mode change is applied.

`CKingApp::Frame` then plays `trirr.bne` when the game asked for it, and reacts to a few flags of the main game view (such as leaving the game screen).

## C++ and assembly in one executable

The C++ code is compiled for the same 32-bit target and linked together with `king.obj`. The tools for this are in `src/king.h`.

### Calling assembly from C++

```cpp
ASM_PROC(FUN_5d8630)                    // a procedure of king.masm
ASM_VAR(HWND, g_hWnd, $L_720920)        // a variable of king.masm

struct CWindow {
    // ...fields...
    void Clear(int color) { CallThis(FUN_5d8630, this, color); }  // thiscall method
    void v_Execute() { CallVirt(this, 48); }                       // virtual, through the vtable
};

inline double Timer_Now() { return CallC<double>(FUN_6037e0); }   // cdecl function
```

- `CallC`, `CallStd` and `CallThis` call cdecl, stdcall (MFC's `AFXAPI`) and thiscall procedures. `CallVirt` calls through the object's vtable, so the game's overrides still run.
- A variable's label must also be in the `PUBLIC` lines at the top of `king.masm`. Procedure labels are public already.
- The names are bound with the linker's `/alternatename` (x86 C names get a leading underscore, the masm labels don't). A wrong name shows up as an unresolved external symbol.
- The library functions in `symbols.csv` carry their real, decorated names in `king.masm`, so a declaration with the library's own signature binds to them without `/alternatename`: `king.h` declares `void AFXAPI AfxPostQuitMessage(int)`, which references `?AfxPostQuitMessage@@YGXH@Z`, and `extern "C" int __cdecl sprintf(char*, const char*, ...)` would reach the game's `_sprintf`.

### Replacing an assembly procedure

1. Write the function in C++, usually as a member of its class in `king.h`. Give the class the fields the function uses and pin their offsets with `CHECK_OFFSET`.
2. Export it with a signature the assembly can call as thiscall:
   ```cpp
   extern "C" BOOL __fastcall CGameApp_OnIdle(CGameApp* self, void* /*edx*/, LONG count)
   {
       return self->OnIdle(count);
   }
   ```
   `__fastcall` passes the first two arguments in ECX and EDX and the rest on the stack, which the callee removes. With an unused EDX argument that is exactly `__thiscall`. The symbol is named `@name@N`, where N is the size of all parameters in bytes (here `@CGameApp_OnIdle@12`).
3. In `king.masm`, keep the original procedure for the assembly build and otherwise point its label at the export, so every call and vtable entry that uses the label reaches the C++ code:
   ```asm
   IFDEF ASM_MAINLOOP
   $L_5dd440 PROC
       ...
   $L_5dd440 ENDP
   ELSE
   EXTERN @CGameApp_OnIdle@12:PROC
   $L_5dd440 TEXTEQU <@CGameApp_OnIdle@12>
   ENDIF
   ```
   `ASM_MAINLOOP` is the main loop's switch; other groups of functions can get their own in the `Makefile`. First make sure that no label *inside* the procedure is used from outside it: jump tables, shared code, or a procedure that the disassembler split into several `PROC` blocks (`$L_569650` continues in `$L_569692`).
4. Add new `.cpp` files to the `Makefile` the way `mainloop.obj` is added.

### Rules for the C++ code

- It has no C runtime of its own: it is built with `/Zl /GS- /GR- /EHs-c-` and links only against Windows import libraries and `king.obj`. The functions of the game's VC6 C runtime that are named in `symbols.csv` link to the game's copy: `memset` and `memcpy` (also where the compiler turns loops or large copies into them), `malloc`, `strlen` and the like. Functions that the Windows SDK headers define inline, such as `sprintf` and the other stdio functions, have to be declared by hand as above. A runtime function without a name in `symbols.csv` is an unresolved external until it gets one. `new` reaches MFC's `operator new`; `delete` needs `/Zc:sizedDealloc-`, otherwise the compiler calls a sized `operator delete(void*, unsigned)` that VC6 doesn't have. `_fltused` is defined in `mainloop.cpp`.
- Floating point is compiled to SSE2, while the assembly uses the x87 FPU, so results can differ in the last bits.
- Pointers are 4 bytes. Check layouts with `CHECK_OFFSET`; `src/compile_flags.txt` makes clangd use the 32-bit target too.

## Working with king.masm

- Labels are named after their address in the original `king.exe`: `FUN_xxxxxx` for functions, `$L_xxxxxx` for everything else. Renamed labels keep the old name in a comment (`_WinMain@16 PROC ;FUN_6317d0`).
- The library functions that the game calls have their real names, decorated as the linker saw them: `_sprintf`, `??2@YAPAXI@Z` (`operator new`), `?Default@CWnd@@IAEJXZ`. `symbols.csv` lists them with their original addresses and prototypes; after adding rows, `py applySymbols.py` renames the labels and every use of them. Renaming changes no byte of the built `king.exe`.
- `py tools\objdiff.py a.obj b.obj` compares two assembled `king.obj`. It ignores label names and `PROC`/`ENDP` lines, so it shows whether an edit of `king.masm` changed code or data.
- The rebuilt `king.exe` doesn't keep the original addresses; they start to drift at `0x4014f0`. Link once with `/MAP:king.map` to see where a label ended up. The map lists procedures and `PUBLIC` variables.
- `cdb` from the Windows SDK debugging tools (x86) can look into the running game and detach again, leaving it running: `cdb -pv -p <pid> -c "~*kv; qd"` prints the stacks of all threads. Resolve the addresses with the map file.
