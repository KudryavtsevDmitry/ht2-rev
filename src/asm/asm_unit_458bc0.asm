; asm_unit_458bc0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Rec60_Copy@12:PROC
EXTERN _LexCompare:PROC
EXTERN _Rec60_Call4667d0:PROC
EXTERN _Rec60_Call4668d0:PROC
EXTERN _Rec60_Call466a00:PROC

PUBLIC $L_45a7fb, $L_45a813, $L_45a848, $L_45a8a4
PUBLIC $L_45a900

_TEXT SEGMENT
FUN_45a7d0 PROC

            mov EAX,DWORD PTR [ESP+8]
            sub ESP,180
            xor EDX,EDX
            push EBX
            push ESI
            cmp EAX,EDX
            push EDI
            jne $L_45a7fb

            mov ECX,DWORD PTR [ESP+196]
            xor EAX,EAX
            cmp ECX,EDX
            setne AL
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a7fb::

            mov ESI,DWORD PTR [ESP+196]
            cmp ESI,EDX
            jne $L_45a813

            or EAX,4294967295
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a813::

            mov BL,BYTE PTR [ESI+8]
            mov EDI,DWORD PTR [ESI]
            mov ECX,DWORD PTR [EAX]
            cmp BL,DL
            jne $L_45a8a4

            cmp BYTE PTR [EAX+8],DL
            jne $L_45a848

            mov EAX,DWORD PTR [EAX+20]
            mov ESI,DWORD PTR [ESI+20]
            add ECX,EAX
            lea EDX,DWORD PTR [ESI+EDI*1]
            push ECX
            push EAX
            push EDX
            push ESI
            call _LexCompare

            add ESP,16
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a848::

            mov DWORD PTR [ESP+76],EAX
            mov DWORD PTR [ESP+16],EAX
            sub ESP,60
            mov DWORD PTR [ESP+72],ECX
            lea EAX,DWORD PTR [ESP+72]
            mov ECX,ESP
            push EAX
            mov DWORD PTR [ESP+136],EDX
            mov DWORD PTR [ESP+152],EDX
            mov DWORD PTR [ESP+92],EDX
            call @Rec60_Copy@12

            mov ESI,DWORD PTR [ESI+20]
            sub ESP,60
            lea EDX,DWORD PTR [ESP+192]
            mov ECX,ESP
            push EDX
            call @Rec60_Copy@12

            lea EAX,DWORD PTR [ESI+EDI*1]
            push EAX
            push ESI
            call _Rec60_Call4667d0

            add ESP,128
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a8a4::

            mov BL,BYTE PTR [EAX+8]
            mov DWORD PTR [ESP+12],EDX
            cmp BL,DL
            mov DWORD PTR [ESP+16],ESI
            mov DWORD PTR [ESP+28],EDX
            mov DWORD PTR [ESP+72],EDI
            mov DWORD PTR [ESP+76],ESI
            mov DWORD PTR [ESP+88],EDX
            jne $L_45a900

            mov EAX,DWORD PTR [EAX+20]
            lea EDX,DWORD PTR [ESP+72]
            add ECX,EAX
            push ECX
            push EAX
            sub ESP,60
            mov ECX,ESP
            push EDX
            call @Rec60_Copy@12

            sub ESP,60
            lea EAX,DWORD PTR [ESP+140]
            mov ECX,ESP
            push EAX
            call @Rec60_Copy@12

            call _Rec60_Call4668d0

            add ESP,128
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a900::

            sub ESP,60
            mov DWORD PTR [ESP+192],EDX
            mov ESI,ESP
            mov DWORD PTR [ESP+196],EAX
            mov DWORD PTR [ESP+208],EDX
            sub ESP,60
            mov DWORD PTR [ESI+16],EDX
            mov DWORD PTR [ESI],ECX
            lea EDX,DWORD PTR [ESP+252]
            mov ECX,ESP
            push EDX
            mov DWORD PTR [ESI+4],EAX
            call @Rec60_Copy@12

            sub ESP,60
            lea EAX,DWORD PTR [ESP+252]
            mov ECX,ESP
            push EAX
            call @Rec60_Copy@12

            sub ESP,60
            lea EDX,DWORD PTR [ESP+252]
            mov ECX,ESP
            push EDX
            call @Rec60_Copy@12

            call _Rec60_Call466a00

            add ESP,240
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
FUN_45a7d0 ENDP
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
_TEXT ENDS
END
