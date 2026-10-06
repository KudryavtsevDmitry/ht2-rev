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
!IFDEF ASM
ASM_MAINLOOP = 1
ASM_GAMEAPP = 1
ASM_KINGAPP = 1
ASM_INPUT = 1
ASM_CARAN = 1
ASM_AITASK = 1
ASM_VECMATH = 1
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
ALLCPPOBJS = mainloop.obj gameapp.obj kingapp.obj input.obj caran.obj aitask.obj vecmath.obj
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

$(ALLCPPOBJS): src\king.h src\x87.h src\car.h src\ai.h src\vecmath.h

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
