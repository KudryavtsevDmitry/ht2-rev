; asm_carvstr.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Dtor543de0_DeletingDestructor@12:PROC

PUBLIC $L_5574f0, $L_557500

_TEXT SEGMENT
$L_5574ef PROC

            int 3
$L_5574f0::

            sub ECX,272
            jmp @Dtor543de0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_557500::

            sub ECX,392
            jmp @Dtor543de0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_5574ef ENDP
ALIGN 16
ALIGN 16
_TEXT ENDS
END
