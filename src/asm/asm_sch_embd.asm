; asm_sch_embd.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Stl_Lock@8:PROC
EXTERN _KingBss:BYTE
EXTERN __CxxThrowException@8:PROC
EXTERN _free:PROC

PUBLIC $L_4a1d19, $L_4a1d43, $L_4a1d4a, $L_4a1d69, $L_4a1d90, $L_4a2059, $L_4a2080

_TEXT SEGMENT
$L_4a1d0d PROC

            mov EDI,DWORD PTR [EBP+12]
            mov EBX,DWORD PTR [EBP-20]
            cmp EDI,EBX
            mov ESI,EDI
            je $L_4a1d4a
$L_4a1d19::

            test DWORD PTR [ESI+8],65536
            jne $L_4a1d43

            mov EAX,DWORD PTR [ESI]
            test EAX,EAX
            je $L_4a1d43

            mov ECX,DWORD PTR [EAX+4]
            dec ECX
            mov DWORD PTR [EAX+4],ECX
            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [ECX+4]
            test EAX,EAX
            jne $L_4a1d43

            test ECX,ECX
            je $L_4a1d43

            mov EDX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EDX+12]
$L_4a1d43::

            add ESI,12
            cmp ESI,EBX
            jne $L_4a1d19
$L_4a1d4a::

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_4a1d90

            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,2
            cmp EAX,128
            jbe $L_4a1d69

            push EDI
            call _free

            add ESP,4
            jmp $L_4a1d90
$L_4a1d69::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov EAX,DWORD PTR [ESI]
            mov DWORD PTR [EDI],EAX
            mov DWORD PTR [ESI],EDI
            mov DWORD PTR [(_KingBss+10724)],0
$L_4a1d90::

            push 0
            push 0
            call __CxxThrowException@8

            nop
            nop
            nop
            nop
            nop
            nop
            nop
$L_4a1d0d ENDP
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
$L_4a2038 PROC

            mov EBX,DWORD PTR [EBP-28]
            mov EAX,DWORD PTR [EBX+4]
            mov EDI,DWORD PTR [EBX]
            test EAX,EAX
            je $L_4a2080

            shl EAX,2
            cmp EAX,128
            jbe $L_4a2059

            push EDI
            call _free

            add ESP,4
            jmp $L_4a2080
$L_4a2059::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov EAX,DWORD PTR [ESI]
            mov DWORD PTR [EDI],EAX
            mov DWORD PTR [ESI],EDI
            mov DWORD PTR [(_KingBss+10724)],0
$L_4a2080::

            push 0
            push 0
            mov DWORD PTR [EBX],0
            mov DWORD PTR [EBX+4],0
            call __CxxThrowException@8

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
$L_4a2038 ENDP
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
$L_4a2a62 PROC

            mov ECX,DWORD PTR [EBP-12]
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 8
$L_4a2a62 ENDP
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
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
ALIGN 16
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
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
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
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
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
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
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
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
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
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
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
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
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
_TEXT ENDS
END
