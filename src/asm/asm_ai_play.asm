; asm_ai_play.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Int_Set@12:PROC
EXTERN @Position_InitNode@12:PROC
EXTERN @SchList_Begin@12:PROC
EXTERN @SchList_End@12:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _Range_CallE:PROC
EXTERN _Road_PositionCostBest:PROC
EXTERN _Town_Id:PROC

PUBLIC $L_477bac

_TEXT SEGMENT
ALIGN 16
FUN_471780 PROC

            sub ESP,8
            mov ECX,DWORD PTR [(_KingBss+9888)]
            push EBX
            mov EBX,DWORD PTR [ESP+16]
            mov DWORD PTR [ESP+4],0
            mov DWORD PTR [ESP+8],0
            push ESI
            mov EAX,DWORD PTR [EBX+12]
            push EDI
            mov EDX,DWORD PTR [ECX+EAX*4]
            lea EAX,DWORD PTR [ESP+24]
            push EDX
            push EAX
            call _Town_Id

            mov EDX,DWORD PTR [ESP+32]
            add ESP,8
            lea ECX,DWORD PTR [ESP+12]
            push ECX
            sub ESP,32
            mov ECX,ESP
            push EDX
            call @Position_InitNode@12

            sub ESP,32
            mov ECX,8
            lea ESI,DWORD PTR [ESP+96]
            mov EDI,ESP

            rep movsd 

            push 0
            call _Road_PositionCostBest

            fld QWORD PTR [(_KingData+14496)]
            fmul QWORD PTR [ESP+84]
            add ESP,72
            pop EDI
            pop ESI
            fstp QWORD PTR [EBX+88]
            pop EBX
            add ESP,8
            ret 
          BYTE 090H

            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
FUN_471780 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
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
ALIGN 16
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
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
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
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
; AI players: src\ai\ai_drivers.cpp, nmake ASM_AI_DRIVERS=1 assembles the original
ALIGN 16
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
