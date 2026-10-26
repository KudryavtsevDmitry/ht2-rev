; asm_eh_funclets.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Eh_EmptyDtor@4:NEAR
EXTERN @Gate_602bd0@8:PROC
EXTERN @MsgBuf_Destroy@8:PROC
EXTERN @SchHandle_Release@8:PROC
EXTERN @Wnd_Destruct@4:PROC
EXTERN _KingRdata:BYTE
EXTERN ___CxxFrameHandler:PROC

PUBLIC $L_643ef0, $L_643f00, $L_643f10, $L_643f20
PUBLIC $L_643f30, $L_644420, $L_644ae0, $L_644ae8
PUBLIC $L_64a298, $L_64a2a0
PUBLIC $L_64a2fc
PUBLIC $L_64a304

_TEXT SEGMENT
$L_6436b0 PROC

            mov ECX,DWORD PTR [EBP+8]
            jmp @SchHandle_Release@8
$L_6436b0 ENDP
$L_6436b8 PROC

            mov EAX,OFFSET (_KingRdata+89256)
            jmp ___CxxFrameHandler
$L_6436b8 ENDP
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
$L_643b10 PROC

            lea ECX,DWORD PTR [EBP-24]
            jmp @SchHandle_Release@8
$L_643b10 ENDP
$L_643b18 PROC

            mov EAX,OFFSET (_KingRdata+90592)
            jmp ___CxxFrameHandler
$L_643b18 ENDP
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
$L_643ef0::

            mov EAX,OFFSET (_KingRdata+92104)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_643f00::

            mov EAX,OFFSET (_KingRdata+92240)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_643f10::

            mov EAX,OFFSET (_KingRdata+92336)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_643f20::

            mov EAX,OFFSET (_KingRdata+92472)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_643f30::

            mov EAX,OFFSET (_KingRdata+92568)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_64441d PROC

            int 3
          BYTE 0ccH

            int 3
$L_644420::

            lea ECX,DWORD PTR [EBP-24]
            jmp @SchHandle_Release@8
$L_64441d ENDP
$L_644428 PROC

            lea ECX,DWORD PTR [EBP-24]
            jmp @SchHandle_Release@8
$L_644428 ENDP
$L_644430 PROC

            mov EAX,OFFSET (_KingRdata+94280)
            jmp ___CxxFrameHandler
$L_644430 ENDP
$L_644ada PROC

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_644ae0::

            lea ECX,DWORD PTR [EBP-24]
            jmp @SchHandle_Release@8
$L_644ae8::

            mov EAX,OFFSET (_KingRdata+96224)
            jmp ___CxxFrameHandler
$L_644ada ENDP
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
$L_646800 PROC

            lea ECX,DWORD PTR [EBP-624]
            jmp @Gate_602bd0@8
$L_646800 ENDP
$L_64680b PROC

            lea ECX,DWORD PTR [EBP-624]
            jmp @Gate_602bd0@8
$L_64680b ENDP
$L_646816 PROC

            lea ECX,DWORD PTR [EBP-584]
            jmp @MsgBuf_Destroy@8
$L_646816 ENDP
$L_646821 PROC

            lea ECX,DWORD PTR [EBP-556]
            jmp @MsgBuf_Destroy@8
$L_646821 ENDP
$L_64682c PROC

            mov EAX,OFFSET (_KingRdata+104032)
            jmp ___CxxFrameHandler
$L_64682c ENDP
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
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
