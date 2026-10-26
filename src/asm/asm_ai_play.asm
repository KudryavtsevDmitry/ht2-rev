; asm_ai_play.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Int_Set@12:PROC
EXTERN @SchList_Begin@12:PROC
EXTERN @SchList_End@12:PROC
EXTERN _KingBss:BYTE
EXTERN _Range_CallE:PROC

PUBLIC $L_477bac

_TEXT SEGMENT
FUN_477b60 PROC

            mov EAX,DWORD PTR [ESP+4]
            sub ESP,8
            push EAX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4464)
            call @SchList_End@12

            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4464)
            call @SchList_Begin@12

            lea ECX,DWORD PTR [ESP+12]
            push ECX
            call _Range_CallE

            mov ECX,DWORD PTR [ESP+16]
            mov EAX,DWORD PTR [(_KingBss+4464)]
            add ESP,16
            cmp ECX,EAX
            jne $L_477bac

            mov ECX,EAX
            mov EAX,DWORD PTR [ESP+12]
            mov DWORD PTR [(_KingBss+11476)],ECX
            mov DWORD PTR [EAX],ECX
            add ESP,8
            ret 
$L_477bac::

            push ECX
            mov EAX,ESP
            mov DWORD PTR [EAX],ECX
            lea ECX,DWORD PTR [ESP+8]
            call @Int_Set@12

            mov EAX,DWORD PTR [EAX]
            mov ECX,DWORD PTR [ESP+12]
            mov DWORD PTR [(_KingBss+11476)],EAX
            mov DWORD PTR [ECX],EAX
            mov EAX,ECX
            add ESP,8
            ret 
FUN_477b60 ENDP
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI road ids: src\ai\road_graph.cpp, nmake ASM_ROAD_GRAPH=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
_TEXT ENDS
END
