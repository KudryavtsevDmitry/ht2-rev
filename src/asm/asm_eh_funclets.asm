; asm_eh_funclets.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Eh_EmptyDtor@4:NEAR
EXTERN @Wnd_Destruct@4:PROC
EXTERN _KingRdata:BYTE
EXTERN ___CxxFrameHandler:PROC

PUBLIC $L_64a298, $L_64a2a0
PUBLIC $L_64a2fc
PUBLIC $L_64a304

_TEXT SEGMENT
$L_64a298::

            mov ECX,DWORD PTR [EBP-16]
            jmp @Eh_EmptyDtor@4
$L_64a2a0::

            mov EAX,OFFSET (_KingRdata+121456)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a2fc::

            lea ECX,DWORD PTR [EBP-80]
            jmp @Wnd_Destruct@4
$L_64a304::

            mov EAX,OFFSET (_KingRdata+121656)
            jmp ___CxxFrameHandler

            int 3

            int 3
_TEXT ENDS
END
