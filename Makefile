# NMAKE Makefile: build king.exe from king.masm and the C++ code in src\
#   nmake                 - build
#   nmake ASM_MAINLOOP=1  - build with the original assembly main loop instead
#                           of src\mainloop.cpp (nmake clean when switching)
#   nmake clean           - remove build outputs except king.res
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

!IFDEF ASM_MAINLOOP
MLFLAGS = /DASM_MAINLOOP
CPPOBJS =
!ELSE
CPPOBJS = mainloop.obj
!ENDIF

# The C++ code has no CRT of its own (king.masm contains the original one):
# /Zl no default libraries, /GS- no security cookies, /GR- /EHs-c- no RTTI, no EH
CFLAGS = /nologo /c /O2 /W3 /GS- /Zl /GR- /EHs-c- /I"$(VCDIR)\include" /I"$(SDKINC)\um" /I"$(SDKINC)\shared" /I"$(SDKINC)\ucrt"

LIBS = WSOCK32.lib winmm.lib DINPUT.lib DDRAW.lib MSVFW32.lib DSOUND.lib KERNEL32.lib USER32.lib GDI32.lib WINSPOOL.lib SHELL32.lib COMCTL32.lib ole32.lib OLEAUT32.lib
LIBPATH_FLAGS = /LIBPATH:"C:\Users\Dmitry\Documents\dx7sdk\dx7sdk-700.1\lib" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86"

build: $(OUT)

$(OBJ): $(SRC)
	$(ML) /nologo $(MLFLAGS) /c /Fo$(OBJ) $(SRC)

mainloop.obj: src\mainloop.cpp src\king.h
	$(CC) $(CFLAGS) /Fomainloop.obj src\mainloop.cpp

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
	-del /q $(OBJ) mainloop.obj $(OUT) 2>nul
