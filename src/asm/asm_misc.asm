; asm_misc.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc


_TEXT SEGMENT
UnusedTrap PROC
            int 3
UnusedTrap ENDP
;===================================
_TEXT ENDS
END
