# NMAKE Makefile: build king.exe from the C++ code in src\ and the assembly
# still left in src\asm
#   nmake                 - build
#   nmake clean           - remove build outputs except king.res
# The first build takes the resources from the game's original king.exe:
#   nmake "GAME_EXE=C:\path\to\king.exe"   (or set GAME_EXE in the environment)
# The last build with king.masm and the ASM_* switches is the tag last-masm.

# 32-bit assembler, compiler and linker
VCDIR = C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.29.30133
ML = "$(VCDIR)\bin\HostX86\x86\ml.exe"
CC = "$(VCDIR)\bin\HostX86\x86\cl.exe"
LINK = "$(VCDIR)\bin\HostX86\x86\link.exe"
SDKINC = C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0
# Python with pefile (override: nmake PYTHON=python)
PYTHON = py

RES = king.res
OUT = king.exe

ALLCPPOBJS = data.obj data_image.obj x87_code.obj ai_containers.obj ai_drivers.obj ai_saveload.obj ai_task.obj ai_traffic.obj ai_world.obj cargo_orders.obj road_graph.obj route_search.obj clusters.obj controls.obj core.obj directdraw.obj framework.obj game_app.obj input.obj main_loop.obj mfc.obj media.obj renderer.obj scene.obj sound_files.obj vecmath.obj video_mode.obj marine_core.obj marine_grid.obj marine_startup.obj mesh_construct.obj refinement_math.obj wave_vertices.obj evaluator.obj native_values.obj natives.obj runtime.obj values.obj directsound.obj sound_gate.obj timer.obj grid_base.obj grid_support.obj grid_topology.obj terrain.obj terrain_grid.obj drivetrain.obj effects.obj force_feedback.obj game_shell.obj helicopter.obj king_app.obj moving_bodies.obj moving_items.obj object_ids.obj scenery.obj startup.obj vehicle.obj vehicle_engine.obj vehicle_physics.obj vehicle_setup.obj weather.obj world.obj world_objects.obj
CPPOBJS = $(ALLCPPOBJS) crt.obj

# The C++ code names its libraries itself (the modern static C runtime):
# /Zl no default libraries, /GS- no security cookies, /GR- /EHs-c- no RTTI, no EH.
# /arch:IA32: floating point on the x87 FPU like the assembly (see src\x87.h)
CFLAGS = /nologo /c /O2 /W3 /GS- /Zl /GR- /EHs-c- /arch:IA32 /I"src\common" /I"src\ai" /I"src\truck" /I"$(VCDIR)\include" /I"$(SDKINC)\um" /I"$(SDKINC)\shared" /I"$(SDKINC)\ucrt"

# the assembly not yet rewritten: one file per original unit (ASMOBJS)
!INCLUDE src\asm\objects.mak

LIBS = WSOCK32.lib winmm.lib DINPUT.lib DDRAW.lib MSVFW32.lib DSOUND.lib KERNEL32.lib USER32.lib GDI32.lib WINSPOOL.lib ADVAPI32.lib SHELL32.lib COMCTL32.lib ole32.lib OLEAUT32.lib
# the modern static C runtime; legacy_stdio_definitions has
# the printf family as functions for the assembly
CRTLIBS = libcmt.lib libvcruntime.lib libucrt.lib legacy_stdio_definitions.lib
LIBPATH_FLAGS = /LIBPATH:"$(VCDIR)\lib\x86" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\ucrt\x86" /LIBPATH:"C:\Users\Dmitry\Documents\dx7sdk\dx7sdk-700.1\lib" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86"

build: $(OUT)

{src\asm}.asm.obj:
	$(ML) /nologo /c /Isrc\asm /Fo$@ $<

$(ASMOBJS): src\asm\king.inc

{src\ai}.cpp.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

{src\engine}.cpp.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

{src\marine}.cpp.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

{src\script}.cpp.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

{src\sound}.cpp.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

{src\terrain}.cpp.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

{src\truck}.cpp.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

$(ALLCPPOBJS): src\ai\ai.h src\ai\road.h src\common\king.h src\common\vecmath.h src\common\x87.h src\truck\car.h

# Resources (icon, cursor, title bitmaps, strings) taken from the game's king.exe
$(RES): extractResources.py
!IF "$(GAME_EXE)" == ""
	@echo king.res needs the game's original king.exe: nmake "GAME_EXE=C:\path\to\king.exe"
	@exit 1
!ENDIF
	$(PYTHON) extractResources.py "$(GAME_EXE)" $(RES)

# /NXCOMPAT:NO /TSAWARE:NO keep the original's DLL characteristics (no DEP)
$(OUT): $(ASMOBJS) $(CPPOBJS) $(RES)
	$(LINK) /nologo $(ASMOBJS) $(CPPOBJS) $(RES) /SUBSYSTEM:WINDOWS,5.01 /MACHINE:X86 $(LIBPATH_FLAGS) $(LIBS) $(CRTLIBS) /FIXED /ENTRY:WinMainCRTStartup /RELEASE /INCREMENTAL:NO /DEBUG:NONE /NXCOMPAT:NO /TSAWARE:NO /OUT:$(OUT) /MAP:king.map

# king.res stays: rebuilding it needs the game's king.exe
clean:
	-del /q $(ASMOBJS) $(ALLCPPOBJS) crt.obj $(OUT) 2>nul
