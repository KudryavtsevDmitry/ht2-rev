; asm_unit_5bc2f0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Ctl_Select@12:PROC
EXTERN @Node_Next@8:PROC

PUBLIC $L_5be2c9, $L_5be2da, $L_5be315, $L_5be32c, $L_5be32f, $L_5be33f, $L_5be352, $L_5be35b
PUBLIC $L_5be36f, $L_5be389, $L_5be391, $L_5be393, $L_5be399, $L_5be3ae, $L_5be3b0, $L_5be3c0
PUBLIC $L_5be3d1, $L_5be3da, $L_5be3f9, $L_5be40a, $L_5be411, $L_5be41f, $L_5be421, $L_5be431
PUBLIC $L_5be445, $L_5be453, $L_5be45c, $L_5be46c, $L_5be478, $L_5be47c, $L_5be480, $L_5be484
PUBLIC $L_5be488

_TEXT SEGMENT
$L_5bd55f PROC

            int 3
$L_5bd55f ENDP
ALIGN 16
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
$L_5be2b0 PROC

            push EBX
            mov EBX,ECX
            push EBP
            push ESI
            mov ECX,DWORD PTR [EBX+292]
            test ECX,ECX
            jne $L_5be2c9

            mov EAX,DWORD PTR [ESP+16]
            pop ESI
            pop EBP
            pop EBX
            ret 8
$L_5be2c9::

            mov EAX,DWORD PTR [EBX+40]
            test EAX,EAX
            je $L_5be2da

            mov EAX,DWORD PTR [ESP+16]
            pop ESI
            pop EBP
            pop EBX
            ret 8
$L_5be2da::

            mov ESI,DWORD PTR [ESP+20]
            mov EBP,DWORD PTR [ESP+16]
            mov EAX,DWORD PTR [ECX]
            push ESI
            push EBP
            call DWORD PTR [EAX+40]

            test AH,128
            jne $L_5be393

            test ESI,ESI
            jne $L_5be391

            lea EAX,DWORD PTR [EBP-9]
            cmp EAX,31
            ja $L_5be391

            xor ECX,ECX
            mov CL,BYTE PTR [EAX+$L_5be488]
            jmp DWORD PTR [ECX*4+$L_5be478]
$L_5be315::

            mov ECX,DWORD PTR [EBX+292]
            call @Node_Next@8

            mov ESI,EAX
            test ESI,ESI
            jne $L_5be32c

            mov ESI,DWORD PTR [EBX+288]
$L_5be32c::

            push EDI
            xor EDI,EDI
$L_5be32f::

            test DWORD PTR [ESI+32],65536
            jne $L_5be33f

            mov EAX,DWORD PTR [ESI+40]
            test EAX,EAX
            je $L_5be35b
$L_5be33f::

            mov ECX,ESI
            call @Node_Next@8

            mov ESI,EAX
            test ESI,ESI
            jne $L_5be352

            mov ESI,DWORD PTR [EBX+288]
$L_5be352::

            inc EDI
            cmp EDI,1000
            jl $L_5be32f
$L_5be35b::

            cmp EDI,1000
            pop EDI
            jne $L_5be36f

            pop ESI
            pop EBP
            mov EAX,32768
            pop EBX
            ret 8
$L_5be36f::

            mov ECX,DWORD PTR [EBX+292]
            mov EDX,DWORD PTR [ECX]
            call DWORD PTR [EDX+44]

            test EAX,EAX
            jne $L_5be389

            mov EAX,DWORD PTR [ESI]
            mov ECX,ESI
            call DWORD PTR [EAX+44]

            test EAX,EAX
            je $L_5be391
$L_5be389::

            push ESI
            mov ECX,EBX
            call @Ctl_Select@12
$L_5be391::

            mov EAX,EBP
$L_5be393::

            pop ESI
            pop EBP
            pop EBX
            ret 8
$L_5be399::

            mov ECX,DWORD PTR [EBX+292]
            call @Node_Next@8

            test EAX,EAX
            jne $L_5be3ae

            mov EAX,DWORD PTR [EBX+288]
$L_5be3ae::

            xor ESI,ESI
$L_5be3b0::

            test DWORD PTR [EAX+32],65536
            jne $L_5be3c0

            mov ECX,DWORD PTR [EAX+40]
            test ECX,ECX
            je $L_5be3da
$L_5be3c0::

            mov ECX,EAX
            call @Node_Next@8

            test EAX,EAX
            jne $L_5be3d1

            mov EAX,DWORD PTR [EBX+288]
$L_5be3d1::

            inc ESI
            cmp ESI,1000
            jl $L_5be3b0
$L_5be3da::

            cmp ESI,1000
            je $L_5be46c

            push EAX
            mov ECX,EBX
            call @Ctl_Select@12

            pop ESI
            pop EBP
            mov EAX,32768
            pop EBX
            ret 8
$L_5be3f9::

            mov EAX,DWORD PTR [EBX+292]
            mov ECX,DWORD PTR [EAX+4]
            test ECX,ECX
            je $L_5be40a

            mov EAX,ECX
            jmp $L_5be41f
$L_5be40a::

            mov ECX,DWORD PTR [EAX+8]
            test ECX,ECX
            je $L_5be41f
$L_5be411::

            mov ECX,EAX
            call @Node_Next@8

            mov ECX,DWORD PTR [EAX+8]
            test ECX,ECX
            jne $L_5be411
$L_5be41f::

            xor ESI,ESI
$L_5be421::

            test DWORD PTR [EAX+32],65536
            jne $L_5be431

            mov ECX,DWORD PTR [EAX+40]
            test ECX,ECX
            je $L_5be45c
$L_5be431::

            mov EAX,DWORD PTR [EAX+4]
            test EAX,EAX
            jne $L_5be453

            mov EAX,DWORD PTR [EBX+292]
            mov ECX,DWORD PTR [EAX+8]
            test ECX,ECX
            je $L_5be453
$L_5be445::

            mov ECX,EAX
            call @Node_Next@8

            mov ECX,DWORD PTR [EAX+8]
            test ECX,ECX
            jne $L_5be445
$L_5be453::

            inc ESI
            cmp ESI,1000
            jl $L_5be421
$L_5be45c::

            cmp ESI,1000
            je $L_5be46c

            push EAX
            mov ECX,EBX
            call @Ctl_Select@12
$L_5be46c::

            pop ESI
            pop EBP
            mov EAX,32768
            pop EBX
            ret 8
          BYTE 090H
$L_5be478           DWORD $L_5be315
$L_5be47c           DWORD $L_5be3f9
$L_5be480           DWORD $L_5be399
$L_5be484           DWORD $L_5be391
$L_5be488           BYTE ($L_5be478-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be484-$L_5be478)/4
          BYTE ($L_5be47c-$L_5be478)/4
          BYTE ($L_5be47c-$L_5be478)/4
          BYTE ($L_5be480-$L_5be478)/4
          BYTE ($L_5be480-$L_5be478)/4

            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
$L_5be2b0 ENDP
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
ALIGN 16
; src\engine\controls.cpp, nmake ASM_CONTROLS=1 assembles the original
_TEXT ENDS
END
