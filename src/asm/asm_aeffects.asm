; asm_aeffects.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Dtor4d0ba0_DeletingDestructor@12:PROC
EXTERN @Dtor4d0ed0_DeletingDestructor@12:PROC
EXTERN @Dtor4d12d0_DeletingDestructor@12:PROC
EXTERN @Dtor4d25d0_DeletingDestructor@12:PROC
EXTERN FUN_4ff660:NEAR
EXTERN _KingBss:BYTE

PUBLIC $L_4d8330, $L_4d8340, $L_4d9380, $L_4d9390, $L_4d93a0, $L_4d93b0, $L_4d93c0, $L_4d93d0
PUBLIC $L_4d93e0, $L_4d93f0, $L_4d9400, $L_4d9410, $L_4d9420, $L_4d9430, $L_4d9440, $L_4d944a

_TEXT SEGMENT
ALIGN 16
FUN_4d2970 PROC

            jmp FUN_4ff660

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
FUN_4d2970 ENDP
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
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
$L_4d8320 PROC

            ret 
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
$L_4d8330::

            ret 
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
            nop
$L_4d8340::

            ret 
          BYTE 090H
          BYTE 090H
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
            nop
            nop
            nop
$L_4d8320 ENDP
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
$L_4d937f PROC

            int 3
$L_4d9380::

            sub ECX,272
            jmp @Dtor4d25d0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d9390::

            sub ECX,392
            jmp @Dtor4d25d0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d93a0::

            sub ECX,16
            jmp @Dtor4d0ba0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d93b0::

            sub ECX,272
            jmp @Dtor4d0ba0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d93c0::

            sub ECX,392
            jmp @Dtor4d0ba0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d93d0::

            sub ECX,16
            jmp @Dtor4d12d0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d93e0::

            sub ECX,272
            jmp @Dtor4d12d0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d93f0::

            sub ECX,392
            jmp @Dtor4d12d0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d9400::

            sub ECX,16
            jmp @Dtor4d0ed0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d9410::

            sub ECX,272
            jmp @Dtor4d0ed0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d9420::

            sub ECX,392
            jmp @Dtor4d0ed0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_4d9430::

            jmp $L_4d9440
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
$L_4d9440::

            mov EAX,OFFSET (_KingBss+47980)
            mov ECX,37
$L_4d944a::

            mov DWORD PTR [EAX],4294967295
            add EAX,24
            dec ECX
            jne $L_4d944a

            ret 
$L_4d937f ENDP
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
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
ALIGN 16
; effects: src\truck\effects.cpp, nmake ASM_EFFECTS=1 assembles the original
ALIGN 16
; engine: src\truck\startup.cpp, nmake ASM_STARTUP=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; engine: src\truck\startup.cpp, nmake ASM_STARTUP=1 assembles the original
ALIGN 16
ALIGN 16
; engine: src\truck\startup.cpp, nmake ASM_STARTUP=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; engine: src\truck\startup.cpp, nmake ASM_STARTUP=1 assembles the original
ALIGN 16
; engine: src\truck\startup.cpp, nmake ASM_STARTUP=1 assembles the original
ALIGN 16
; engine: src\truck\startup.cpp, nmake ASM_STARTUP=1 assembles the original
ALIGN 16
ALIGN 16
; engine: src\truck\startup.cpp, nmake ASM_STARTUP=1 assembles the original
ALIGN 16
_TEXT ENDS
END
