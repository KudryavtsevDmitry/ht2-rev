# NMAKE Makefile: build king.exe from king.masm and the C++ code in src\
#   nmake                 - build
#   nmake ASM=1           - build the original program, without any C++
#   nmake ASM_GAMEAPP=1   - build with the original assembly for one group of
#                           rewritten functions (switches below)
#   nmake clean           - remove build outputs except king.res
# Run nmake clean when changing the switches; nmake doesn't notice them.
# The first build takes the resources from the game's original king.exe:
#   nmake "GAME_EXE=C:\path\to\king.exe"   (or set GAME_EXE in the environment)

# 32-bit assembler, compiler and linker
VCDIR = C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.29.30133
ML = "$(VCDIR)\bin\HostX86\x86\ml.exe"
CC = "$(VCDIR)\bin\HostX86\x86\cl.exe"
LINK = "$(VCDIR)\bin\HostX86\x86\link.exe"
SDKINC = C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0
# Python with pefile (override: nmake PYTHON=python)
PYTHON = py

SRC = king.masm
OBJ = king.obj
RES = king.res
OUT = king.exe

# Groups of functions rewritten in C++. Each switch makes king.masm assemble
# the group's original procedures, so the assembly calls those again; the C++
# code still calls the C++ functions.
#   ASM_MAINLOOP  src\mainloop.cpp  the main loop
#   ASM_GAMEAPP   src\gameapp.cpp   CGameApp, the engine's application class
#   ASM_KINGAPP   src\kingapp.cpp   CKingApp, the game's application class
#   ASM_INPUT     src\input.cpp     the input event queue
#   ASM_CARAN     src\caran.cpp     engine, gearbox, driving resistance
#   ASM_AITASK    src\aitask.cpp    the AI's game interface (AI_TASK.CPP)
#   ASM_VECMATH   src\vecmath.cpp   vector, matrix and plane math
#   ASM_AITRCE    src\aitrce.cpp    AI geometry, CRC and filters (src\aitrce.cpp)
#   ASM_AIROAD    src\airoad.cpp    AI road ids (src\airoad.cpp)
#   ASM_AIPLAY    src\aiplay.cpp    AI players (src\aiplay.cpp)
#   ASM_AEFFECTS  src\aeffects.cpp  effects (src\aeffects.cpp)
#   ASM_CAR2E     src\car2e.cpp     vehicle methods (src\car2e.cpp)
#   ASM_CARAT     src\carat.cpp     game, menus and screens (src\carat.cpp)
#   ASM_AIMAIN    src\aimain.cpp    AI main (src\aimain.cpp)
#   ASM_AISTL     src\aistl.cpp     AI STL instances (src\aistl.cpp)
#   ASM_AIMISC    src\aimisc.cpp    AI helpers (src\aimisc.cpp)
#   ASM_VEHICLE   src\vehicle.cpp   vehicle: controls, vehicle list, flashing light
#   ASM_SCHEVAL   src\scheval.cpp   scheval
#   ASM_WASM      src\wasm.cpp      wasm
#   ASM_COPTER    src\copter.cpp    copter
#   ASM_MOVINGB   src\movingb.cpp   movingb
#   ASM_WORLD     src\world.cpp     world
#   ASM_AIMULT    src\aimult.cpp    aimult
#   ASM_SCHITEM   src\schitem.cpp   schitem
#   ASM_MMT       src\mmt.cpp       mmt
#   ASM_CARPHYS   src\carphys.cpp   carphys
#   ASM_AIORDR    src\aiordr.cpp    aiordr
#   ASM_ENGINE    src\engine.cpp    engine
#   ASM_MARINE    src\marine.cpp, src\marineinit.cpp, src\marinegrid.cpp, src\marinemath.cpp, src\marinevertices.cpp, src\marineconstruct.cpp, src\gridbase.cpp, src\gridtopology.cpp    marine mesh processing
#   ASM_GUI       src\gui.cpp       window controls
#   ASM_VRAPPL    srcrappl.cpp    engine library: sound stream, tables, music, trees
!IFDEF ASM
ASM_MAINLOOP = 1
ASM_GAMEAPP = 1
ASM_KINGAPP = 1
ASM_INPUT = 1
ASM_CARAN = 1
ASM_AITASK = 1
ASM_VECMATH = 1
ASM_AITRCE = 1
ASM_AIROAD = 1
ASM_AIPLAY = 1
ASM_AEFFECTS = 1
ASM_CAR2E = 1
ASM_CARAT = 1
ASM_AIMAIN = 1
ASM_AISTL = 1
ASM_AIMISC = 1
ASM_VEHICLE = 1
ASM_SCHEVAL = 1
ASM_WASM = 1
ASM_COPTER = 1
ASM_MOVINGB = 1
ASM_WORLD = 1
ASM_AIMULT = 1
ASM_SCHITEM = 1
ASM_MMT = 1
ASM_CARPHYS = 1
ASM_AIORDR = 1
ASM_ENGINE = 1
ASM_MARINE = 1
ASM_GUI = 1
ASM_VRAPPL = 1
!ENDIF

MLFLAGS =
!IFDEF ASM_MAINLOOP
MLFLAGS = $(MLFLAGS) /DASM_MAINLOOP
!ENDIF
!IFDEF ASM_GAMEAPP
MLFLAGS = $(MLFLAGS) /DASM_GAMEAPP
!ENDIF
!IFDEF ASM_KINGAPP
MLFLAGS = $(MLFLAGS) /DASM_KINGAPP
!ENDIF
!IFDEF ASM_INPUT
MLFLAGS = $(MLFLAGS) /DASM_INPUT
!ENDIF
!IFDEF ASM_CARAN
MLFLAGS = $(MLFLAGS) /DASM_CARAN
!ENDIF

!IFDEF ASM_AITASK
MLFLAGS = $(MLFLAGS) /DASM_AITASK
!ENDIF
!IFDEF ASM_VECMATH
MLFLAGS = $(MLFLAGS) /DASM_VECMATH
!ENDIF
!IFDEF ASM_AITRCE
MLFLAGS = $(MLFLAGS) /DASM_AITRCE
!ENDIF
!IFDEF ASM_AIROAD
MLFLAGS = $(MLFLAGS) /DASM_AIROAD
!ENDIF
!IFDEF ASM_AIPLAY
MLFLAGS = $(MLFLAGS) /DASM_AIPLAY
!ENDIF
!IFDEF ASM_AEFFECTS
MLFLAGS = $(MLFLAGS) /DASM_AEFFECTS
!ENDIF
!IFDEF ASM_CAR2E
MLFLAGS = $(MLFLAGS) /DASM_CAR2E
!ENDIF
!IFDEF ASM_CARAT
MLFLAGS = $(MLFLAGS) /DASM_CARAT
!ENDIF
!IFDEF ASM_AIMAIN
MLFLAGS = $(MLFLAGS) /DASM_AIMAIN
!ENDIF
!IFDEF ASM_AISTL
MLFLAGS = $(MLFLAGS) /DASM_AISTL
!ENDIF
!IFDEF ASM_AIMISC
MLFLAGS = $(MLFLAGS) /DASM_AIMISC
!ENDIF
!IFDEF ASM_VEHICLE
MLFLAGS = $(MLFLAGS) /DASM_VEHICLE
!ENDIF
!IFDEF ASM_SCHEVAL
MLFLAGS = $(MLFLAGS) /DASM_SCHEVAL
!ENDIF
!IFDEF ASM_WASM
MLFLAGS = $(MLFLAGS) /DASM_WASM
!ENDIF
!IFDEF ASM_COPTER
MLFLAGS = $(MLFLAGS) /DASM_COPTER
!ENDIF
!IFDEF ASM_MOVINGB
MLFLAGS = $(MLFLAGS) /DASM_MOVINGB
!ENDIF
!IFDEF ASM_WORLD
MLFLAGS = $(MLFLAGS) /DASM_WORLD
!ENDIF
!IFDEF ASM_AIMULT
MLFLAGS = $(MLFLAGS) /DASM_AIMULT
!ENDIF
!IFDEF ASM_SCHITEM
MLFLAGS = $(MLFLAGS) /DASM_SCHITEM
!ENDIF
!IFDEF ASM_MMT
MLFLAGS = $(MLFLAGS) /DASM_MMT
!ENDIF
!IFDEF ASM_CARPHYS
MLFLAGS = $(MLFLAGS) /DASM_CARPHYS
!ENDIF
!IFDEF ASM_AIORDR
MLFLAGS = $(MLFLAGS) /DASM_AIORDR
!ENDIF
!IFDEF ASM_ENGINE
MLFLAGS = $(MLFLAGS) /DASM_ENGINE
!ENDIF
!IFDEF ASM_MARINE
MLFLAGS = $(MLFLAGS) /DASM_MARINE
!ENDIF
!IFDEF ASM_GUI
MLFLAGS = $(MLFLAGS) /DASM_GUI
!ENDIF
!IFDEF ASM_VRAPPL
MLFLAGS = $(MLFLAGS) /DASM_VRAPPL
!ENDIF
ALLCPPOBJS = mainloop.obj gameapp.obj kingapp.obj input.obj caran.obj aitask.obj vecmath.obj aitrce.obj airoad.obj aiplay.obj aeffects.obj car2e.obj carat.obj aimain.obj aistl.obj aimisc.obj vehicle.obj scheval.obj wasm.obj copter.obj movingb.obj world.obj aimult.obj schitem.obj mmt.obj carphys.obj aiordr.obj engine.obj marine.obj marineinit.obj marinegrid.obj marinemath.obj marinevertices.obj marineconstruct.obj gridbase.obj gridtopology.obj gui.obj vrappl.obj
!IFDEF ASM
CPPOBJS =
!ELSE
CPPOBJS = $(ALLCPPOBJS)
!ENDIF

# The C++ code has no CRT of its own (king.masm contains the original one):
# /Zl no default libraries, /GS- no security cookies, /GR- /EHs-c- no RTTI, no EH.
# /arch:IA32: floating point on the x87 FPU like the assembly (see src\x87.h)
CFLAGS = /nologo /c /O2 /W3 /GS- /Zl /GR- /EHs-c- /arch:IA32 /I"$(VCDIR)\include" /I"$(SDKINC)\um" /I"$(SDKINC)\shared" /I"$(SDKINC)\ucrt"

LIBS = WSOCK32.lib winmm.lib DINPUT.lib DDRAW.lib MSVFW32.lib DSOUND.lib KERNEL32.lib USER32.lib GDI32.lib WINSPOOL.lib SHELL32.lib COMCTL32.lib ole32.lib OLEAUT32.lib
LIBPATH_FLAGS = /LIBPATH:"C:\Users\Dmitry\Documents\dx7sdk\dx7sdk-700.1\lib" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86"

build: $(OUT)

$(OBJ): $(SRC)
	$(ML) /nologo $(MLFLAGS) /c /Fo$(OBJ) $(SRC)

{src}.cpp.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

$(ALLCPPOBJS): src\king.h src\x87.h src\car.h src\ai.h src\vecmath.h src\road.h

# Resources (icon, cursor, title bitmaps, strings) taken from the game's king.exe
$(RES): extractResources.py
!IF "$(GAME_EXE)" == ""
	@echo king.res needs the game's original king.exe: nmake "GAME_EXE=C:\path\to\king.exe"
	@exit 1
!ENDIF
	$(PYTHON) extractResources.py "$(GAME_EXE)" $(RES)

# /NXCOMPAT:NO /TSAWARE:NO keep the original's DLL characteristics (no DEP)
$(OUT): $(OBJ) $(CPPOBJS) $(RES)
	$(LINK) /nologo $(OBJ) $(CPPOBJS) $(RES) /SUBSYSTEM:WINDOWS,5.01 /MACHINE:X86 $(LIBPATH_FLAGS) $(LIBS) /FIXED /ENTRY:_EntryPoint /RELEASE /INCREMENTAL:NO /DEBUG:NONE /NXCOMPAT:NO /TSAWARE:NO /OUT:$(OUT)

# king.res stays: rebuilding it needs the game's king.exe
clean:
	-del /q $(OBJ) $(ALLCPPOBJS) $(OUT) 2>nul
