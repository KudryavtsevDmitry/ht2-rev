; asm_unit_5bfe60.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_648ba8:NEAR
EXTERN @Item_Register@16:PROC
EXTERN @Panel_Clear@8:PROC
EXTERN @WeatherItem_Construct@84:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE

PUBLIC $L_5bfe9c, $L_5bfeb5, $L_5bfed9, $L_5bff24, $L_5bff2b, $L_5bff5b, $L_5bff70, $L_5bffea
PUBLIC $L_5bfff7, $L_5c0000, $L_5c63c0, $L_5c63d0

_TEXT SEGMENT
$L_5bfe60 PROC

            push EBX
            push ESI
            mov ESI,DWORD PTR [ESP+12]
            push EDI
            xor EDI,EDI
            movsx EAX,WORD PTR [ESI]
            cmp EAX,512
            jg $L_5bff5b

            je $L_5bfed9

            sub EAX,256
            je $L_5bfeb5

            sub EAX,2
            jne $L_5bfff7

            movsx EDI,WORD PTR [ESI+6]
            cmp EDI,27
            jne $L_5bfe9c

            xor EDI,EDI
            mov EAX,EDI
            pop EDI
            pop ESI
            pop EBX
            ret 4
$L_5bfe9c::

            cmp EDI,32
            jl $L_5bfff7

            mov EAX,DWORD PTR [ECX]
            push 1
            push EDI
            call DWORD PTR [EAX+40]

            mov EDI,EAX
            pop EDI
            pop ESI
            pop EBX
            ret 4
$L_5bfeb5::

            movsx EAX,WORD PTR [ESI+6]
            mov EDX,DWORD PTR [ECX]
            push 0
            push EAX
            call DWORD PTR [EDX+40]

            mov EDI,EAX
            cmp EDI,27
            jne $L_5bfff7

            mov EDI,32795
            mov EAX,EDI
            pop EDI
            pop ESI
            pop EBX
            ret 4
$L_5bfed9::

            movsx EAX,WORD PTR [ESI+2]
            sub EAX,DWORD PTR [ECX+44]
            mov DWORD PTR [(_KingData+78812)],EAX
            movsx EDX,WORD PTR [ESI+4]
            mov ESI,DWORD PTR [ECX+48]
            mov DWORD PTR [(_KingBss+493332)],EAX
            sub EDX,ESI
            mov ESI,DWORD PTR [(_KingBss+493316)]
            test ESI,ESI
            mov DWORD PTR [(_KingData+78816)],EDX
            mov DWORD PTR [(_KingBss+493336)],EDX
            je $L_5bff24

            mov ECX,DWORD PTR [(_KingBss+493328)]
            mov EBX,DWORD PTR [ESI]
            sub EDX,ECX
            mov ECX,ESI
            push EDX
            mov EDX,DWORD PTR [(_KingBss+493324)]
            sub EAX,EDX
            push EAX
            call DWORD PTR [EBX+36]

            jmp $L_5bff2b
$L_5bff24::

            mov ESI,DWORD PTR [ECX]
            push EDX
            push EAX
            call DWORD PTR [ESI+36]
$L_5bff2b::

            test EAX,EAX
            jne $L_5bfff7

            mov EAX,DWORD PTR [(_KingBss+493356)]
            test EAX,EAX
            je $L_5bfff7

            push 0
            push 0
            call EAX

            add ESP,8
            mov EAX,EDI
            mov DWORD PTR [(_KingBss+493340)],0
            pop EDI
            pop ESI
            pop EBX
            ret 4
$L_5bff5b::

            add EAX,4294966783
            cmp EAX,4
            ja $L_5bfff7

            jmp DWORD PTR [EAX*4+$L_5c0000]
$L_5bff70::

            movsx EAX,WORD PTR [ESI+2]
            sub EAX,DWORD PTR [ECX+44]
            mov DWORD PTR [(_KingData+78812)],EAX
            mov EDI,DWORD PTR [ECX+48]
            movsx EDX,WORD PTR [ESI+4]
            sub EDX,EDI
            mov EDI,DWORD PTR [(_KingBss+493316)]
            test EDI,EDI
            mov DWORD PTR [(_KingData+78816)],EDX
            mov DWORD PTR [(_KingBss+493332)],EAX
            mov DWORD PTR [(_KingBss+493336)],EDX
            je $L_5bffea

            mov ECX,DWORD PTR [(_KingBss+493328)]
            mov EBX,DWORD PTR [EDI]
            sub EDX,ECX
            movsx ECX,WORD PTR [ESI]
            push EDX
            mov EDX,DWORD PTR [(_KingBss+493324)]
            sub EAX,EDX
            push EAX
            push ECX
            mov ECX,EDI
            call DWORD PTR [EBX+32]

            mov EDI,EAX
            test EDI,32768
            je $L_5bfff7

            mov ECX,DWORD PTR [(_KingBss+493320)]
            test ECX,ECX
            je $L_5bfff7

            and EAX,4294934527
            jle $L_5bfff7

            cmp EAX,2
            jg $L_5bfff7

            call @Panel_Clear@8

            mov EAX,EDI
            pop EDI
            pop ESI
            pop EBX
            ret 4
$L_5bffea::

            mov EDI,DWORD PTR [ECX]
            push EDX
            movsx EDX,WORD PTR [ESI]
            push EAX
            push EDX
            call DWORD PTR [EDI+32]

            mov EDI,EAX
$L_5bfff7::

            mov EAX,EDI
            pop EDI
            pop ESI
            pop EBX
            ret 4
          BYTE 090H
$L_5c0000           DWORD $L_5bff70
          DWORD $L_5bff70
          DWORD $L_5bfff7
          DWORD $L_5bff70
          DWORD $L_5bff70

            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
$L_5bfe60 ENDP
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/controls.cpp, nmake ASM_CONTROLS=1 assembles the original
; src/controls.cpp, nmake ASM_CONTROLS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
; src/controls.cpp, nmake ASM_CONTROLS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/controls.cpp, nmake ASM_CONTROLS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
; the row callback that shares the original's code stays in assembly
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
FUN_5c6170 PROC

            push -1
            push OFFSET $L_648ba8
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ECX
            mov EAX,DWORD PTR [ESP+96]
            push EBX
            push ESI
            push EDI
            push EAX
            mov EDX,DWORD PTR [ESP+52]
            mov EBX,ECX
            sub ESP,48
            mov EAX,DWORD PTR [ESP+96]
            mov ECX,12
            lea ESI,DWORD PTR [ESP+108]
            mov EDI,ESP

            rep movsd 

            mov ECX,DWORD PTR [ESP+104]
            mov ESI,DWORD PTR [ESP+84]
            push ECX
            mov ECX,DWORD PTR [ESP+96]
            push EDX
            mov EDX,DWORD PTR [ESP+96]
            push EAX
            push ECX
            push EDX
            push ESI
            mov ECX,EBX
            mov DWORD PTR [ESP+88],EBX
            call @WeatherItem_Construct@84

            mov EAX,DWORD PTR [ESP+104]
            mov ECX,EBX
            push EAX
            push ESI
            mov DWORD PTR [ESP+32],0
            mov DWORD PTR [EBX],OFFSET (_KingRdata+27700)
            mov DWORD PTR [EBX+256],OFFSET (_KingRdata+27624)
            call @Item_Register@16

            mov ECX,DWORD PTR [ESP+16]
            pop EDI
            mov EAX,EBX
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            add ESP,16
            ret 80
FUN_5c6170 ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
$L_5c63bf PROC

            int 3
$L_5c63c0::

            jmp $L_5c63d0
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5c63d0::

            ret 
$L_5c63bf ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\media.cpp, nmake ASM_MEDIA=1 assembles the original
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/media.cpp, nmake ASM_MEDIA=1 assembles the original
_TEXT ENDS
END
