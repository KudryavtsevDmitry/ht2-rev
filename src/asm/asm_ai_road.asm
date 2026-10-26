; asm_ai_road.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_640971:NEAR
EXTERN @Shell_Nop@8:PROC
EXTERN @Vec1_Free@8:PROC
EXTERN _Stl_Allocate:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN _memmove:NEAR

PUBLIC $L_47d38f, $L_47d3cb, $L_47d3fe, $L_47d41a, $L_47d43a

_TEXT SEGMENT
FUN_47d360 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_640971
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,12
            push EBX
            mov EBX,DWORD PTR [EBP+8]
            push ESI
            mov ESI,DWORD PTR [EBP+16]
            push EDI
            xor EDI,EDI
            mov DWORD PTR [EBP-16],ESP
            mov DWORD PTR [EBP-20],ESI
            mov DWORD PTR [EBP-4],EDI
$L_47d38f::

            cmp EBX,DWORD PTR [EBP+12]
            je $L_47d443

            mov DWORD PTR [EBP-24],ESI
            cmp ESI,EDI
            mov BYTE PTR [EBP-4],1
            je $L_47d3fe

            mov EAX,DWORD PTR [EBX]
            mov DWORD PTR [ESI],EAX
            mov EAX,DWORD PTR [EBX+8]
            mov ECX,DWORD PTR [EBX+4]
            mov DWORD PTR [ESI+4],EDI
            sub EAX,ECX
            mov DWORD PTR [ESI+8],EDI
            mov DWORD PTR [EBP+8],EAX
            mov DWORD PTR [ESI+12],EDI
            je $L_47d3cb

            push EAX
            call _Stl_Allocate

            mov EDI,EAX
            mov EAX,DWORD PTR [EBP+8]
            add ESP,4
$L_47d3cb::

            add EAX,EDI
            mov DWORD PTR [ESI+4],EDI
            mov DWORD PTR [ESI+8],EDI
            mov DWORD PTR [ESI+12],EAX
            mov ECX,DWORD PTR [EBX+8]
            mov EAX,DWORD PTR [EBX+4]
            mov DWORD PTR [EBP-20],ECX
            sub ECX,EAX
            push ECX
            push EAX
            push EDI
            mov DWORD PTR [EBP+8],EAX
            call _memmove

            mov ECX,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EBP-20]
            sub EDI,ECX
            add ESP,12
            add EDI,EAX
            mov DWORD PTR [ESI+8],EDI
            xor EDI,EDI
$L_47d3fe::

            add EBX,16
            add ESI,16
            mov BYTE PTR [EBP-4],0
            mov DWORD PTR [EBP-20],ESI
            jmp $L_47d38f
FUN_47d360 ENDP
$L_47d40d PROC

            mov EAX,DWORD PTR [EBP+16]
            mov EDI,DWORD PTR [EBP-20]
            cmp EAX,EDI
            je $L_47d43a

            lea ESI,DWORD PTR [EAX+4]
$L_47d41a::

            mov ECX,DWORD PTR [ESI+4]
            mov EDX,DWORD PTR [ESI]
            push ECX
            push EDX
            call @Shell_Nop@8

            add ESP,8
            mov ECX,ESI
            call @Vec1_Free@8

            add ESI,16
            lea EAX,DWORD PTR [ESI-4]
            cmp EAX,EDI
            jne $L_47d41a
$L_47d43a::

            push 0
            push 0
            call __CxxThrowException@8
$L_47d40d ENDP
$L_47d443 PROC

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,ESI
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_47d443 ENDP
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI road ids: src\ai\road_graph.cpp, nmake ASM_ROAD_GRAPH=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI road ids: src\ai\road_graph.cpp, nmake ASM_ROAD_GRAPH=1 assembles the original
ALIGN 16
ALIGN 16
; AI road ids: src\ai\road_graph.cpp, nmake ASM_ROAD_GRAPH=1 assembles the original
ALIGN 16
; AI road ids: src\ai\road_graph.cpp, nmake ASM_ROAD_GRAPH=1 assembles the original
ALIGN 16
_TEXT ENDS
END
