; asm_sch_embd.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc



_TEXT SEGMENT
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
ALIGN 16
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
_TEXT ENDS
END
