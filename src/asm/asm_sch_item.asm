; asm_sch_item.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_644430:NEAR
EXTERN $L_644ae8:NEAR

PUBLIC $L_4b130b, $L_4b130f, $L_4b1316, $L_4b1360, $L_4b1364, $L_4b136b, $L_4b6982, $L_4b6984
PUBLIC $L_4b69c1, $L_4b69c5, $L_4b69cc, $L_4b69ce, $L_4b6a03

_TEXT SEGMENT
$L_4b12b0 PROC

            push -1
            push OFFSET $L_644430
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,12
            push ESI
            push EDI
            xor EDI,EDI
            mov ESI,ECX
            mov DWORD PTR [ESP+8],EDI
            mov DWORD PTR [ESP+12],EDI
            mov DWORD PTR [ESP+16],EDI
            mov EAX,DWORD PTR [ESI+20]
            mov DWORD PTR [ESP+28],EDI
            cmp EAX,EDI
            je $L_4b1316

            test DWORD PTR [ESI+28],65536
            jne $L_4b1316

            mov ECX,DWORD PTR [EAX+4]
            dec ECX
            mov DWORD PTR [EAX+4],ECX
            mov ECX,DWORD PTR [ESI+20]
            cmp DWORD PTR [ECX+4],EDI
            jne $L_4b1316

            cmp ECX,EDI
            jne $L_4b130f

            cmp DWORD PTR [ESI+28],EDI
            jne $L_4b130b

            cmp DWORD PTR [ESI+24],EDI
            je $L_4b1316
$L_4b130b::

            cmp ECX,EDI
            je $L_4b1316
$L_4b130f::

            mov EAX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EAX+12]
$L_4b1316::

            mov DWORD PTR [ESI+24],EDI
            mov DWORD PTR [ESI+20],EDI
            mov DWORD PTR [ESI+28],EDI
            mov DWORD PTR [ESP+8],EDI
            mov DWORD PTR [ESP+12],EDI
            mov DWORD PTR [ESP+16],EDI
            mov EAX,DWORD PTR [ESI+32]
            mov DWORD PTR [ESP+28],1
            cmp EAX,EDI
            je $L_4b136b

            test DWORD PTR [ESI+40],65536
            jne $L_4b136b

            mov ECX,DWORD PTR [EAX+4]
            dec ECX
            mov DWORD PTR [EAX+4],ECX
            mov ECX,DWORD PTR [ESI+32]
            cmp DWORD PTR [ECX+4],EDI
            jne $L_4b136b

            cmp ECX,EDI
            jne $L_4b1364

            cmp DWORD PTR [ESI+40],EDI
            jne $L_4b1360

            cmp DWORD PTR [ESI+36],EDI
            je $L_4b136b
$L_4b1360::

            cmp ECX,EDI
            je $L_4b136b
$L_4b1364::

            mov EDX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EDX+12]
$L_4b136b::

            mov ECX,DWORD PTR [ESP+20]
            mov DWORD PTR [ESI+36],EDI
            mov DWORD PTR [ESI+32],EDI
            mov DWORD PTR [ESI+40],EDI
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            add ESP,24
            ret 
$L_4b12b0 ENDP
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
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
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
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
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
$L_4b6930 PROC

            mov EAX,DWORD PTR FS:[0]
            push -1
            push OFFSET $L_644ae8
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,12
            push EBX
            mov EBX,ECX
            push EBP
            xor ECX,ECX
            mov EAX,DWORD PTR [EBX+24]
            mov EDX,DWORD PTR [EBX+20]
            sub EAX,EDX
            xor EBP,EBP
            mov EDX,EAX
            mov EAX,715827883
            imul EDX
            sar EDX,1
            mov EAX,EDX
            shr EAX,31
            add EDX,EAX
            je $L_4b6a03

            push ESI
            push EDI
            mov DWORD PTR [ESP+16],ECX
            mov DWORD PTR [ESP+20],ECX
            mov DWORD PTR [ESP+24],ECX
            xor ESI,ESI
            jmp $L_4b6984
$L_4b6982::

            xor ECX,ECX
$L_4b6984::

            mov EDI,DWORD PTR [EBX+20]
            mov DWORD PTR [ESP+36],ECX
            mov EAX,DWORD PTR [ESI+EDI*1]
            cmp EAX,ECX
            je $L_4b69ce

            test DWORD PTR [ESI+EDI*1+8],65536
            jne $L_4b69ce

            mov ECX,DWORD PTR [EAX+4]
            dec ECX
            mov DWORD PTR [EAX+4],ECX
            mov ECX,DWORD PTR [ESI+EDI*1]
            mov EAX,DWORD PTR [ECX+4]
            test EAX,EAX
            jne $L_4b69cc

            test ECX,ECX
            jne $L_4b69c5

            mov EAX,DWORD PTR [ESI+EDI*1+8]
            test EAX,EAX
            jne $L_4b69c1

            mov EAX,DWORD PTR [ESI+EDI*1+4]
            test EAX,EAX
            je $L_4b69cc
$L_4b69c1::

            test ECX,ECX
            je $L_4b69cc
$L_4b69c5::

            mov EDX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EDX+12]
$L_4b69cc::

            xor ECX,ECX
$L_4b69ce::

            mov DWORD PTR [ESI+EDI*1+4],ECX
            mov DWORD PTR [ESI+EDI*1],ECX
            mov DWORD PTR [ESI+EDI*1+8],ECX
            mov ECX,DWORD PTR [EBX+24]
            mov EDX,DWORD PTR [EBX+20]
            mov EAX,715827883
            sub ECX,EDX
            inc EBP
            imul ECX
            sar EDX,1
            mov EAX,EDX
            add ESI,12
            shr EAX,31
            add EDX,EAX
            mov DWORD PTR [ESP+36],4294967295
            cmp EBP,EDX
            jb $L_4b6982

            pop EDI
            pop ESI
$L_4b6a03::

            mov ECX,DWORD PTR [ESP+20]
            pop EBP
            pop EBX
            mov DWORD PTR FS:[0],ECX
            add ESP,24
            ret 
$L_4b6930 ENDP
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
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
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
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
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
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
_TEXT ENDS
END
