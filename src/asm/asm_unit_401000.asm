; asm_unit_401000.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN _KingBss:BYTE


_TEXT SEGMENT

ALIGN 16
FUN_401000 PROC

            fld QWORD PTR [(_KingBss+0)]
            ret 
FUN_401000 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
_TEXT ENDS
END
