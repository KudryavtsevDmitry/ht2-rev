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
#   ASM_AI_CONTAINERS          src\ai\ai_containers.cpp
#   ASM_AI_DRIVERS             src\ai\ai_drivers.cpp
#   ASM_AI_SAVELOAD            src\ai\ai_saveload.cpp
#   ASM_AI_TASK                src\ai\ai_task.cpp
#   ASM_AI_TRAFFIC             src\ai\ai_traffic.cpp
#   ASM_AI_WORLD               src\ai\ai_world.cpp
#   ASM_CARGO_ORDERS           src\ai\cargo_orders.cpp
#   ASM_ROAD_GRAPH             src\ai\road_graph.cpp
#   ASM_ROUTE_SEARCH           src\ai\route_search.cpp
#   ASM_CLUSTERS               src\engine\clusters.cpp
#   ASM_CONTROLS               src\engine\controls.cpp
#   ASM_CORE                   src\engine\core.cpp
#   ASM_DIRECTDRAW             src\engine\directdraw.cpp
#   ASM_FRAMEWORK              src\engine\framework.cpp
#   ASM_GAME_APP               src\engine\game_app.cpp
#   ASM_INPUT                  src\engine\input.cpp
#   ASM_MAIN_LOOP              src\engine\main_loop.cpp
#   ASM_MEDIA                  src\engine\media.cpp
#   ASM_RENDERER               src\engine\renderer.cpp
#   ASM_SCENE                  src\engine\scene.cpp
#   ASM_SOUND_FILES            src\engine\sound_files.cpp
#   ASM_VECMATH                src\engine\vecmath.cpp
#   ASM_VIDEO_MODE             src\engine\video_mode.cpp
#   ASM_MARINE_CORE            src\marine\marine_core.cpp
#   ASM_MARINE_GRID            src\marine\marine_grid.cpp
#   ASM_MARINE_STARTUP         src\marine\marine_startup.cpp
#   ASM_MESH_CONSTRUCT         src\marine\mesh_construct.cpp
#   ASM_REFINEMENT_MATH        src\marine\refinement_math.cpp
#   ASM_WAVE_VERTICES          src\marine\wave_vertices.cpp
#   ASM_EVALUATOR              src\script\evaluator.cpp
#   ASM_NATIVE_VALUES          src\script\native_values.cpp
#   ASM_NATIVES                src\script\natives.cpp
#   ASM_RUNTIME                src\script\runtime.cpp
#   ASM_VALUES                 src\script\values.cpp
#   ASM_DIRECTSOUND            src\sound\directsound.cpp
#   ASM_SOUND_GATE             src\sound\sound_gate.cpp
#   ASM_TIMER                  src\sound\timer.cpp
#   ASM_GRID_BASE              src\terrain\grid_base.cpp
#   ASM_GRID_SUPPORT           src\terrain\grid_support.cpp
#   ASM_GRID_TOPOLOGY          src\terrain\grid_topology.cpp
#   ASM_TERRAIN                src\terrain\terrain.cpp
#   ASM_TERRAIN_GRID           src\terrain\terrain_grid.cpp
#   ASM_DRIVETRAIN             src\truck\drivetrain.cpp
#   ASM_EFFECTS                src\truck\effects.cpp
#   ASM_FORCE_FEEDBACK         src\truck\force_feedback.cpp
#   ASM_GAME_SHELL             src\truck\game_shell.cpp
#   ASM_HELICOPTER             src\truck\helicopter.cpp
#   ASM_KING_APP               src\truck\king_app.cpp
#   ASM_MOVING_BODIES          src\truck\moving_bodies.cpp
#   ASM_MOVING_ITEMS           src\truck\moving_items.cpp
#   ASM_OBJECT_IDS             src\truck\object_ids.cpp
#   ASM_SCENERY                src\truck\scenery.cpp
#   ASM_STARTUP                src\truck\startup.cpp
#   ASM_VEHICLE                src\truck\vehicle.cpp
#   ASM_VEHICLE_ENGINE         src\truck\vehicle_engine.cpp
#   ASM_VEHICLE_PHYSICS        src\truck\vehicle_physics.cpp
#   ASM_VEHICLE_SETUP          src\truck\vehicle_setup.cpp
#   ASM_WEATHER                src\truck\weather.cpp
#   ASM_WORLD                  src\truck\world.cpp
#   ASM_WORLD_OBJECTS          src\truck\world_objects.cpp
!IFDEF ASM
ASM_AI_CONTAINERS = 1
ASM_AI_DRIVERS = 1
ASM_AI_SAVELOAD = 1
ASM_AI_TASK = 1
ASM_AI_TRAFFIC = 1
ASM_AI_WORLD = 1
ASM_CARGO_ORDERS = 1
ASM_ROAD_GRAPH = 1
ASM_ROUTE_SEARCH = 1
ASM_CLUSTERS = 1
ASM_CONTROLS = 1
ASM_CORE = 1
ASM_DIRECTDRAW = 1
ASM_FRAMEWORK = 1
ASM_GAME_APP = 1
ASM_INPUT = 1
ASM_MAIN_LOOP = 1
ASM_MEDIA = 1
ASM_RENDERER = 1
ASM_SCENE = 1
ASM_SOUND_FILES = 1
ASM_VECMATH = 1
ASM_VIDEO_MODE = 1
ASM_MARINE_CORE = 1
ASM_MARINE_GRID = 1
ASM_MARINE_STARTUP = 1
ASM_MESH_CONSTRUCT = 1
ASM_REFINEMENT_MATH = 1
ASM_WAVE_VERTICES = 1
ASM_EVALUATOR = 1
ASM_NATIVE_VALUES = 1
ASM_NATIVES = 1
ASM_RUNTIME = 1
ASM_VALUES = 1
ASM_DIRECTSOUND = 1
ASM_SOUND_GATE = 1
ASM_TIMER = 1
ASM_GRID_BASE = 1
ASM_GRID_SUPPORT = 1
ASM_GRID_TOPOLOGY = 1
ASM_TERRAIN = 1
ASM_TERRAIN_GRID = 1
ASM_DRIVETRAIN = 1
ASM_EFFECTS = 1
ASM_FORCE_FEEDBACK = 1
ASM_GAME_SHELL = 1
ASM_HELICOPTER = 1
ASM_KING_APP = 1
ASM_MOVING_BODIES = 1
ASM_MOVING_ITEMS = 1
ASM_OBJECT_IDS = 1
ASM_SCENERY = 1
ASM_STARTUP = 1
ASM_VEHICLE = 1
ASM_VEHICLE_ENGINE = 1
ASM_VEHICLE_PHYSICS = 1
ASM_VEHICLE_SETUP = 1
ASM_WEATHER = 1
ASM_WORLD = 1
ASM_WORLD_OBJECTS = 1
!ENDIF

MLFLAGS =
!IFDEF ASM_AI_CONTAINERS
MLFLAGS = $(MLFLAGS) /DASM_AI_CONTAINERS
!ENDIF
!IFDEF ASM_AI_DRIVERS
MLFLAGS = $(MLFLAGS) /DASM_AI_DRIVERS
!ENDIF
!IFDEF ASM_AI_SAVELOAD
MLFLAGS = $(MLFLAGS) /DASM_AI_SAVELOAD
!ENDIF
!IFDEF ASM_AI_TASK
MLFLAGS = $(MLFLAGS) /DASM_AI_TASK
!ENDIF
!IFDEF ASM_AI_TRAFFIC
MLFLAGS = $(MLFLAGS) /DASM_AI_TRAFFIC
!ENDIF
!IFDEF ASM_AI_WORLD
MLFLAGS = $(MLFLAGS) /DASM_AI_WORLD
!ENDIF
!IFDEF ASM_CARGO_ORDERS
MLFLAGS = $(MLFLAGS) /DASM_CARGO_ORDERS
!ENDIF
!IFDEF ASM_ROAD_GRAPH
MLFLAGS = $(MLFLAGS) /DASM_ROAD_GRAPH
!ENDIF
!IFDEF ASM_ROUTE_SEARCH
MLFLAGS = $(MLFLAGS) /DASM_ROUTE_SEARCH
!ENDIF
!IFDEF ASM_CLUSTERS
MLFLAGS = $(MLFLAGS) /DASM_CLUSTERS
!ENDIF
!IFDEF ASM_CONTROLS
MLFLAGS = $(MLFLAGS) /DASM_CONTROLS
!ENDIF
!IFDEF ASM_CORE
MLFLAGS = $(MLFLAGS) /DASM_CORE
!ENDIF
!IFDEF ASM_DIRECTDRAW
MLFLAGS = $(MLFLAGS) /DASM_DIRECTDRAW
!ENDIF
!IFDEF ASM_FRAMEWORK
MLFLAGS = $(MLFLAGS) /DASM_FRAMEWORK
!ENDIF
!IFDEF ASM_GAME_APP
MLFLAGS = $(MLFLAGS) /DASM_GAME_APP
!ENDIF
!IFDEF ASM_INPUT
MLFLAGS = $(MLFLAGS) /DASM_INPUT
!ENDIF
!IFDEF ASM_MAIN_LOOP
MLFLAGS = $(MLFLAGS) /DASM_MAIN_LOOP
!ENDIF
!IFDEF ASM_MEDIA
MLFLAGS = $(MLFLAGS) /DASM_MEDIA
!ENDIF
!IFDEF ASM_RENDERER
MLFLAGS = $(MLFLAGS) /DASM_RENDERER
!ENDIF
!IFDEF ASM_SCENE
MLFLAGS = $(MLFLAGS) /DASM_SCENE
!ENDIF
!IFDEF ASM_SOUND_FILES
MLFLAGS = $(MLFLAGS) /DASM_SOUND_FILES
!ENDIF
!IFDEF ASM_VECMATH
MLFLAGS = $(MLFLAGS) /DASM_VECMATH
!ENDIF
!IFDEF ASM_VIDEO_MODE
MLFLAGS = $(MLFLAGS) /DASM_VIDEO_MODE
!ENDIF
!IFDEF ASM_MARINE_CORE
MLFLAGS = $(MLFLAGS) /DASM_MARINE_CORE
!ENDIF
!IFDEF ASM_MARINE_GRID
MLFLAGS = $(MLFLAGS) /DASM_MARINE_GRID
!ENDIF
!IFDEF ASM_MARINE_STARTUP
MLFLAGS = $(MLFLAGS) /DASM_MARINE_STARTUP
!ENDIF
!IFDEF ASM_MESH_CONSTRUCT
MLFLAGS = $(MLFLAGS) /DASM_MESH_CONSTRUCT
!ENDIF
!IFDEF ASM_REFINEMENT_MATH
MLFLAGS = $(MLFLAGS) /DASM_REFINEMENT_MATH
!ENDIF
!IFDEF ASM_WAVE_VERTICES
MLFLAGS = $(MLFLAGS) /DASM_WAVE_VERTICES
!ENDIF
!IFDEF ASM_EVALUATOR
MLFLAGS = $(MLFLAGS) /DASM_EVALUATOR
!ENDIF
!IFDEF ASM_NATIVE_VALUES
MLFLAGS = $(MLFLAGS) /DASM_NATIVE_VALUES
!ENDIF
!IFDEF ASM_NATIVES
MLFLAGS = $(MLFLAGS) /DASM_NATIVES
!ENDIF
!IFDEF ASM_RUNTIME
MLFLAGS = $(MLFLAGS) /DASM_RUNTIME
!ENDIF
!IFDEF ASM_VALUES
MLFLAGS = $(MLFLAGS) /DASM_VALUES
!ENDIF
!IFDEF ASM_DIRECTSOUND
MLFLAGS = $(MLFLAGS) /DASM_DIRECTSOUND
!ENDIF
!IFDEF ASM_SOUND_GATE
MLFLAGS = $(MLFLAGS) /DASM_SOUND_GATE
!ENDIF
!IFDEF ASM_TIMER
MLFLAGS = $(MLFLAGS) /DASM_TIMER
!ENDIF
!IFDEF ASM_GRID_BASE
MLFLAGS = $(MLFLAGS) /DASM_GRID_BASE
!ENDIF
!IFDEF ASM_GRID_SUPPORT
MLFLAGS = $(MLFLAGS) /DASM_GRID_SUPPORT
!ENDIF
!IFDEF ASM_GRID_TOPOLOGY
MLFLAGS = $(MLFLAGS) /DASM_GRID_TOPOLOGY
!ENDIF
!IFDEF ASM_TERRAIN
MLFLAGS = $(MLFLAGS) /DASM_TERRAIN
!ENDIF
!IFDEF ASM_TERRAIN_GRID
MLFLAGS = $(MLFLAGS) /DASM_TERRAIN_GRID
!ENDIF
!IFDEF ASM_DRIVETRAIN
MLFLAGS = $(MLFLAGS) /DASM_DRIVETRAIN
!ENDIF
!IFDEF ASM_EFFECTS
MLFLAGS = $(MLFLAGS) /DASM_EFFECTS
!ENDIF
!IFDEF ASM_FORCE_FEEDBACK
MLFLAGS = $(MLFLAGS) /DASM_FORCE_FEEDBACK
!ENDIF
!IFDEF ASM_GAME_SHELL
MLFLAGS = $(MLFLAGS) /DASM_GAME_SHELL
!ENDIF
!IFDEF ASM_HELICOPTER
MLFLAGS = $(MLFLAGS) /DASM_HELICOPTER
!ENDIF
!IFDEF ASM_KING_APP
MLFLAGS = $(MLFLAGS) /DASM_KING_APP
!ENDIF
!IFDEF ASM_MOVING_BODIES
MLFLAGS = $(MLFLAGS) /DASM_MOVING_BODIES
!ENDIF
!IFDEF ASM_MOVING_ITEMS
MLFLAGS = $(MLFLAGS) /DASM_MOVING_ITEMS
!ENDIF
!IFDEF ASM_OBJECT_IDS
MLFLAGS = $(MLFLAGS) /DASM_OBJECT_IDS
!ENDIF
!IFDEF ASM_SCENERY
MLFLAGS = $(MLFLAGS) /DASM_SCENERY
!ENDIF
!IFDEF ASM_STARTUP
MLFLAGS = $(MLFLAGS) /DASM_STARTUP
!ENDIF
!IFDEF ASM_VEHICLE
MLFLAGS = $(MLFLAGS) /DASM_VEHICLE
!ENDIF
!IFDEF ASM_VEHICLE_ENGINE
MLFLAGS = $(MLFLAGS) /DASM_VEHICLE_ENGINE
!ENDIF
!IFDEF ASM_VEHICLE_PHYSICS
MLFLAGS = $(MLFLAGS) /DASM_VEHICLE_PHYSICS
!ENDIF
!IFDEF ASM_VEHICLE_SETUP
MLFLAGS = $(MLFLAGS) /DASM_VEHICLE_SETUP
!ENDIF
!IFDEF ASM_WEATHER
MLFLAGS = $(MLFLAGS) /DASM_WEATHER
!ENDIF
!IFDEF ASM_WORLD
MLFLAGS = $(MLFLAGS) /DASM_WORLD
!ENDIF
!IFDEF ASM_WORLD_OBJECTS
MLFLAGS = $(MLFLAGS) /DASM_WORLD_OBJECTS
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
ALLCPPOBJS = ai_containers.obj ai_drivers.obj ai_saveload.obj ai_task.obj ai_traffic.obj ai_world.obj cargo_orders.obj road_graph.obj route_search.obj clusters.obj controls.obj core.obj directdraw.obj framework.obj game_app.obj input.obj main_loop.obj media.obj renderer.obj scene.obj sound_files.obj vecmath.obj video_mode.obj marine_core.obj marine_grid.obj marine_startup.obj mesh_construct.obj refinement_math.obj wave_vertices.obj evaluator.obj native_values.obj natives.obj runtime.obj values.obj directsound.obj sound_gate.obj timer.obj grid_base.obj grid_support.obj grid_topology.obj terrain.obj terrain_grid.obj drivetrain.obj effects.obj force_feedback.obj game_shell.obj helicopter.obj king_app.obj moving_bodies.obj moving_items.obj object_ids.obj scenery.obj startup.obj vehicle.obj vehicle_engine.obj vehicle_physics.obj vehicle_setup.obj weather.obj world.obj world_objects.obj
!IFDEF ASM
CPPOBJS =
!ELSE
CPPOBJS = $(ALLCPPOBJS)
!ENDIF

# The C++ code has no CRT of its own (king.masm contains the original one):
# /Zl no default libraries, /GS- no security cookies, /GR- /EHs-c- no RTTI, no EH.
# /arch:IA32: floating point on the x87 FPU like the assembly (see src\x87.h)
CFLAGS = /nologo /c /O2 /W3 /GS- /Zl /GR- /EHs-c- /arch:IA32 /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"src\common" /I"src\ai" /I"src\truck" /I"$(VCDIR)\include" /I"$(SDKINC)\um" /I"$(SDKINC)\shared" /I"$(SDKINC)\ucrt"

LIBS = WSOCK32.lib winmm.lib DINPUT.lib DDRAW.lib MSVFW32.lib DSOUND.lib KERNEL32.lib USER32.lib GDI32.lib WINSPOOL.lib SHELL32.lib COMCTL32.lib ole32.lib OLEAUT32.lib
LIBPATH_FLAGS = /LIBPATH:"C:\Users\Dmitry\Documents\dx7sdk\dx7sdk-700.1\lib" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86"

build: $(OUT)

$(OBJ): $(SRC)
	$(ML) /nologo $(MLFLAGS) /c /Fo$(OBJ) $(SRC)

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
$(OUT): $(OBJ) $(CPPOBJS) $(RES)
	$(LINK) /nologo $(OBJ) $(CPPOBJS) $(RES) /SUBSYSTEM:WINDOWS,5.01 /MACHINE:X86 $(LIBPATH_FLAGS) $(LIBS) /FIXED /ENTRY:_EntryPoint /RELEASE /INCREMENTAL:NO /DEBUG:NONE /NXCOMPAT:NO /TSAWARE:NO /OUT:$(OUT) /MAP:king.map

# king.res stays: rebuilding it needs the game's king.exe
clean:
	-del /q $(OBJ) $(ALLCPPOBJS) $(OUT) 2>nul
