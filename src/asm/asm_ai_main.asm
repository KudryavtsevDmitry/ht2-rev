; asm_ai_main.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Vec104_Destroy@4:PROC
EXTERN @Vec24Rec_Destroy@4:PROC
EXTERN @Vec4_FreeStorage@8:PROC
EXTERN @Vec4_FreeStorageB@8:PROC
EXTERN _AI_Init68a830:PROC
EXTERN _AI_Init68cbe8:PROC
EXTERN _AI_Init68cc08:PROC
EXTERN _AI_Init68ceb0:PROC
EXTERN _AI_Init68cf00:PROC
EXTERN _AiStatic68ceb0_AtExit:PROC
EXTERN _KingBss:BYTE
EXTERN _Static401e70_AtExit:PROC
EXTERN _Static401ec0_AtExit:PROC
EXTERN _Static402140_AtExit:PROC
EXTERN _Static402480_AtExit:PROC

PUBLIC $L_401e30, $L_401e40, $L_401e80, $L_401e90
PUBLIC $L_402100, $L_402110, $L_402240, $L_402250, $L_402430, $L_402440, $L_402730

_TEXT SEGMENT
$L_401e30::
            mov ECX,OFFSET (_KingBss+9368)
            jmp @Vec104_Destroy@4
            nop
            nop
            nop
            nop
            nop
            nop
$L_401e40::

            call _AI_Init68cc08

            jmp _Static401e70_AtExit
ALIGN 16
$L_401e80::
            mov ECX,OFFSET (_KingBss+9192)
            jmp @Vec24Rec_Destroy@4
            nop
            nop
            nop
            nop
            nop
            nop
$L_401e90::

            call _AI_Init68cf00

            jmp _Static401ec0_AtExit
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_402100::
            mov ECX,OFFSET (_KingBss+9336)
            jmp @Vec4_FreeStorage@8
            nop
            nop
            nop
            nop
            nop
            nop
$L_402110::

            call _AI_Init68cbe8

            jmp _Static402140_AtExit
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_402240::
            mov ECX,OFFSET (_KingBss+9208)
            jmp @Vec4_FreeStorage@8
            nop
            nop
            nop
            nop
            nop
            nop
$L_402250::

            call _AI_Init68ceb0

            jmp _AiStatic68ceb0_AtExit
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_402430::
            mov ECX,OFFSET (_KingBss+9352)
            jmp @Vec4_FreeStorageB@8
            nop
            nop
            nop
            nop
            nop
            nop
$L_402440::

            call _AI_Init68a830

            jmp _Static402480_AtExit
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_402730::
            mov ECX,OFFSET (_KingBss+9320)
            jmp @Vec4_FreeStorageB@8
            nop
            nop
            nop
            nop
            nop
            nop
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
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
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
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
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16

ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
_TEXT ENDS
END
