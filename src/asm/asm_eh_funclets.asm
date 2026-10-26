; asm_eh_funclets.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_462e30:NEAR
EXTERN $L_61c4c0:NEAR
EXTERN $L_61c730:NEAR
EXTERN ??3@YAXPAX@Z:NEAR
EXTERN @AiVec_FreeStorage@8:PROC
EXTERN @Buf459df0_Destruct@8:PROC
EXTERN @Cdc_Destruct@4:PROC
EXTERN @CmdTarget_Destruct@4:PROC
EXTERN @Gate_602bd0@8:PROC
EXTERN @Gate_FreeBuffers@8:PROC
EXTERN @List16_Destroy@4:PROC
EXTERN @MsgBuf_Destroy@8:PROC
EXTERN @MsgBuf_FreeStorage@8:PROC
EXTERN @Object_Destroy@4:PROC
EXTERN @Play_DestructBase@8:PROC
EXTERN @RopeFn_ResetVtbl@8:PROC
EXTERN @SchHandle_Release@8:PROC
EXTERN @Shell_Nop@8:PROC
EXTERN @String_Destruct@4:PROC
EXTERN @Thunk530190@8:PROC
EXTERN @VecBytes_FreeStorage@4:PROC
EXTERN @Wea5a47f0_Destroy@4:PROC
EXTERN @WinThread_Destruct@4:PROC
EXTERN @Wnd_Destruct@4:PROC
EXTERN _Fw_Delete@4:PROC
EXTERN _KingRdata:BYTE
EXTERN _NoTrack_Delete@4:PROC
EXTERN _Stl_Unlock:PROC
EXTERN ___CxxFrameHandler:PROC

PUBLIC $L_63b648, $L_63b64f, $L_63f8a0, $L_63f8b0, $L_63f8c0, $L_63f8d0, $L_63f8d8, $L_63fde0
PUBLIC $L_63fdf0, $L_63fe00, $L_63fe40, $L_63fe78, $L_63fe7f, $L_63fe87, $L_63fec0, $L_63fed0
PUBLIC $L_63fee1, $L_63fef0, $L_63ff01, $L_63ff10, $L_63ff21, $L_63ff30, $L_63ff60, $L_63ff71
PUBLIC $L_63ff80, $L_640020, $L_640031, $L_640040, $L_640051, $L_640060, $L_640950, $L_640960
PUBLIC $L_640971, $L_640980, $L_643ed0, $L_643ed8, $L_643ef0, $L_643f00, $L_643f10, $L_643f20
PUBLIC $L_643f30, $L_643f40, $L_643f50, $L_643f60, $L_644420, $L_644ae0, $L_644ae8, $L_645e30
PUBLIC $L_645e38, $L_649af0, $L_649afb, $L_649d20, $L_649df0, $L_649df8, $L_649e10, $L_649e20
PUBLIC $L_649e30, $L_649e38, $L_649e50, $L_649e60, $L_649e6b, $L_64a298, $L_64a2a0, $L_64a2ac
PUBLIC $L_64a2b4, $L_64a2c0, $L_64a2c8, $L_64a2d4, $L_64a2dd, $L_64a2e8, $L_64a2f0, $L_64a2fc
PUBLIC $L_64a304, $L_64a310, $L_64a319, $L_64a324, $L_64a330, $L_64a338, $L_64a344, $L_64a34c
PUBLIC $L_64a358, $L_64a360, $L_64a36c, $L_64a375, $L_64a380, $L_64a388, $L_64a394, $L_64a3a0
PUBLIC $L_64a3aa, $L_64a3b4, $L_64a3c0, $L_64a3c8, $L_64a3d4, $L_64a3e0, $L_64a3e8, $L_64a3f4

_TEXT SEGMENT
$L_63b630 PROC

            cmp DWORD PTR [EBP-16],0
            je $L_63b648

            mov EAX,DWORD PTR [EBP-16]
            add EAX,4
            mov DWORD PTR [EBP-120],EAX
            jmp $L_63b64f
$L_63b648::

            mov DWORD PTR [EBP-120],0
$L_63b64f::

            mov ECX,DWORD PTR [EBP-120]
            jmp @List16_Destroy@4
$L_63b630 ENDP
$L_63b657 PROC

            mov EAX,OFFSET (_KingRdata+45712)
            jmp ___CxxFrameHandler
$L_63b657 ENDP
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
          BYTE 0ccH
$L_63b670 PROC

            lea ECX,DWORD PTR [EBP-12]
            jmp _Stl_Unlock
$L_63b670 ENDP
$L_63b678 PROC

            mov EAX,OFFSET (_KingRdata+45752)
            jmp ___CxxFrameHandler
$L_63b678 ENDP
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
$L_63f899 PROC

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63f8a0::

            mov EAX,OFFSET (_KingRdata+67992)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63f8b0::

            mov EAX,OFFSET (_KingRdata+68128)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63f8c0::

            mov EAX,OFFSET (_KingRdata+68216)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63f8d0::

            lea ECX,DWORD PTR [EBP+12]
            jmp @SchHandle_Release@8
$L_63f8d8::

            mov EAX,OFFSET (_KingRdata+68304)
            jmp ___CxxFrameHandler
$L_63f899 ENDP
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
$L_63fdd0 PROC

            mov EAX,OFFSET (_KingRdata+70600)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63fde0::

            mov EAX,OFFSET (_KingRdata+70688)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63fdf0::

            mov EAX,OFFSET (_KingRdata+70776)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63fe00::

            lea ECX,DWORD PTR [EBP-32]
            jmp $L_462e30
$L_63fdd0 ENDP
$L_63fe08 PROC

            lea ECX,DWORD PTR [EBP-28]
            jmp @AiVec_FreeStorage@8
$L_63fe08 ENDP
$L_63fe10 PROC

            lea ECX,DWORD PTR [EBP-28]
            jmp @VecBytes_FreeStorage@4
$L_63fe10 ENDP
$L_63fe18 PROC

            mov EAX,OFFSET (_KingRdata+71000)
            jmp ___CxxFrameHandler
$L_63fe18 ENDP
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
$L_63fe30 PROC

            mov EAX,OFFSET (_KingRdata+71112)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63fe40::

            lea ECX,DWORD PTR [EBP-292]
            jmp @AiVec_FreeStorage@8
$L_63fe30 ENDP
$L_63fe4b PROC

            lea ECX,DWORD PTR [EBP-292]
            jmp @VecBytes_FreeStorage@4
$L_63fe4b ENDP
$L_63fe56 PROC

            mov EAX,OFFSET (_KingRdata+71200)
            jmp ___CxxFrameHandler
$L_63fe56 ENDP
$L_63fe60 PROC

            cmp DWORD PTR [EBP+12],0
            je $L_63fe78

            mov EAX,DWORD PTR [EBP+12]
            add EAX,4
            mov DWORD PTR [EBP-20],EAX
            jmp $L_63fe7f
$L_63fe78::

            mov DWORD PTR [EBP-20],0
$L_63fe7f::

            mov ECX,DWORD PTR [EBP-20]
            jmp @Buf459df0_Destruct@8
$L_63fe60 ENDP
$L_63fe87::
            mov ECX,DWORD PTR [EBP+12]
            jmp @RopeFn_ResetVtbl@8
$L_63fe8f PROC

            mov EAX,OFFSET (_KingRdata+71248)
            jmp ___CxxFrameHandler
$L_63fe8f ENDP
$L_63feba PROC

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63fec0::

            mov EAX,OFFSET (_KingRdata+71448)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63fed0::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_63fee1::

            mov EAX,OFFSET (_KingRdata+71536)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63fef0::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_63ff01::

            mov EAX,OFFSET (_KingRdata+71632)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63ff10::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-28]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_63ff21::

            mov EAX,OFFSET (_KingRdata+71728)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63ff30::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_63feba ENDP
$L_63ff5b PROC

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63ff60::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_63ff71::

            mov EAX,OFFSET (_KingRdata+71936)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_63ff80::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_63ff5b ENDP
$L_64001e PROC

            int 3

            int 3
$L_640020::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_640031::

            mov EAX,OFFSET (_KingRdata+72368)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_640040::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_640051::

            mov EAX,OFFSET (_KingRdata+72464)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_640060::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_64001e ENDP
$L_640940 PROC

            mov EAX,OFFSET (_KingRdata+75984)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_640950::

            mov EAX,OFFSET (_KingRdata+76072)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_640960::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_640971::

            mov EAX,OFFSET (_KingRdata+76160)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_640980::

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP-24]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_640940 ENDP
$L_641620 PROC

            lea ECX,DWORD PTR [EBP+12]
            jmp @Shell_Nop@8
$L_641620 ENDP
$L_641628 PROC

            mov EAX,OFFSET (_KingRdata+80456)
            jmp ___CxxFrameHandler
$L_641628 ENDP
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
$L_641690 PROC

            lea ECX,DWORD PTR [EBP-12]
            jmp _Stl_Unlock
$L_641690 ENDP
$L_641698 PROC

            mov EAX,OFFSET (_KingRdata+80592)
            jmp ___CxxFrameHandler
$L_641698 ENDP
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
$L_643ece PROC

            int 3

            int 3
$L_643ed0::

            lea ECX,DWORD PTR [EBP-32]
            jmp _Stl_Unlock
$L_643ed8::

            mov EAX,OFFSET (_KingRdata+92008)
            jmp ___CxxFrameHandler
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
$L_643f40::

            mov EAX,OFFSET (_KingRdata+92664)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_643f50::

            mov EAX,OFFSET (_KingRdata+92752)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_643f60::

            lea ECX,DWORD PTR [EBP+8]
            jmp @SchHandle_Release@8
$L_643ece ENDP
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
$L_645e2e PROC

            int 3

            int 3
$L_645e30::

            mov ECX,DWORD PTR [EBP-16]
            jmp @Thunk530190@8
$L_645e38::

            mov EAX,OFFSET (_KingRdata+101272)
            jmp ___CxxFrameHandler
$L_645e2e ENDP
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
$L_647260 PROC

            lea ECX,DWORD PTR [EBP-440]
            jmp @MsgBuf_Destroy@8
$L_647260 ENDP
$L_64726b PROC

            mov EAX,OFFSET (_KingRdata+106792)
            jmp ___CxxFrameHandler
$L_64726b ENDP
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
$L_648600 PROC

            mov ECX,DWORD PTR [EBP-80]
            jmp @Object_Destroy@4
$L_648600 ENDP
$L_648608 PROC

            mov EAX,OFFSET (_KingRdata+112568)
            jmp ___CxxFrameHandler
$L_648608 ENDP
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
$L_648ba0 PROC

            mov ECX,DWORD PTR [EBP-16]
            jmp @Wea5a47f0_Destroy@4
$L_648ba0 ENDP
$L_648ba8 PROC

            mov EAX,OFFSET (_KingRdata+114256)
            jmp ___CxxFrameHandler
$L_648ba8 ENDP
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
$L_649ae8 PROC

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_649af0::

            mov EAX,DWORD PTR [EBP+4]
            push EAX
            call ??3@YAXPAX@Z

            pop ECX
            ret 
$L_649afb::

            mov EAX,OFFSET (_KingRdata+118752)
            jmp ___CxxFrameHandler
$L_649ae8 ENDP
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
$L_649d1b PROC

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_649d20::

            lea ECX,DWORD PTR [EBP-44]
            jmp @Gate_FreeBuffers@8
$L_649d1b ENDP
$L_649d28 PROC

            mov EAX,DWORD PTR [EBP+4]
            push EAX
            mov ECX,DWORD PTR [EBP+20]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_649d28 ENDP
$L_649d39 PROC

            mov EAX,OFFSET (_KingRdata+119496)
            jmp ___CxxFrameHandler
$L_649d39 ENDP
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
$L_649d50 PROC

            lea ECX,DWORD PTR [EBP-12]
            jmp _Stl_Unlock
$L_649d50 ENDP
$L_649d58 PROC

            mov EAX,OFFSET (_KingRdata+119544)
            jmp ___CxxFrameHandler
$L_649d58 ENDP
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
$L_649dd0 PROC

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            mov ECX,DWORD PTR [EBP+12]
            push ECX
            call @Shell_Nop@8

            add ESP,8
            ret 
$L_649dd0 ENDP
$L_649de1 PROC

            mov EAX,OFFSET (_KingRdata+119752)
            jmp ___CxxFrameHandler
$L_649de1 ENDP
$L_649deb PROC

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_649df0::

            mov ECX,DWORD PTR [EBP-16]
            jmp @MsgBuf_FreeStorage@8
$L_649df8::

            mov EAX,OFFSET (_KingRdata+119848)
            jmp ___CxxFrameHandler
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
$L_649e10::

            mov EAX,OFFSET (_KingRdata+119944)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_649e20::

            mov EAX,OFFSET (_KingRdata+120032)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_649e30::

            mov ECX,DWORD PTR [EBP-16]
            jmp @Play_DestructBase@8
$L_649e38::

            mov EAX,OFFSET (_KingRdata+120120)
            jmp ___CxxFrameHandler
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
$L_649e50::

            mov EAX,OFFSET (_KingRdata+120160)
            jmp ___CxxFrameHandler

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_649e60::

            mov EAX,DWORD PTR [EBP-60]
            push EAX
            call ??3@YAXPAX@Z

            pop ECX
            ret 
$L_649e6b::

            mov EAX,OFFSET (_KingRdata+120248)
            jmp ___CxxFrameHandler
$L_649deb ENDP
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
$L_64a296 PROC

            int 3

            int 3
$L_64a298::

            mov ECX,DWORD PTR [EBP-16]
            jmp $L_61c4c0
$L_64a2a0::

            mov EAX,OFFSET (_KingRdata+121456)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a2ac::

            mov ECX,DWORD PTR [EBP-16]
            jmp @WinThread_Destruct@4
$L_64a2b4::

            mov EAX,OFFSET (_KingRdata+121496)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a2c0::

            mov ECX,DWORD PTR [EBP-16]
            jmp @WinThread_Destruct@4
$L_64a2c8::

            mov EAX,OFFSET (_KingRdata+121536)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a2d4::

            push DWORD PTR [EBP-16]
            call _NoTrack_Delete@4

            ret 
$L_64a2dd::

            mov EAX,OFFSET (_KingRdata+121576)
            jmp ___CxxFrameHandler

            int 3
$L_64a2e8::

            lea ECX,DWORD PTR [EBP-16]
            jmp @String_Destruct@4
$L_64a2f0::

            mov EAX,OFFSET (_KingRdata+121616)
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
$L_64a310::

            push DWORD PTR [EBP-16]
            call _Fw_Delete@4

            ret 
$L_64a319::

            mov EAX,OFFSET (_KingRdata+121752)
            jmp ___CxxFrameHandler

            int 3
$L_64a324::

            mov EAX,OFFSET (_KingRdata+121792)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a330::

            mov ECX,DWORD PTR [EBP-16]
            jmp @CmdTarget_Destruct@4
$L_64a338::

            mov EAX,OFFSET (_KingRdata+121880)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a344::

            mov ECX,DWORD PTR [EBP-16]
            jmp @CmdTarget_Destruct@4
$L_64a34c::

            mov EAX,OFFSET (_KingRdata+121920)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a358::

            mov ECX,DWORD PTR [EBP-16]
            jmp @CmdTarget_Destruct@4
$L_64a360::

            mov EAX,OFFSET (_KingRdata+121960)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a36c::

            push DWORD PTR [EBP-16]
            call _NoTrack_Delete@4

            ret 
$L_64a375::

            mov EAX,OFFSET (_KingRdata+122000)
            jmp ___CxxFrameHandler

            int 3
$L_64a380::

            mov ECX,DWORD PTR [EBP-16]
            jmp @Shell_Nop@8
$L_64a388::

            mov EAX,OFFSET (_KingRdata+122040)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a394::

            mov EAX,OFFSET (_KingRdata+122080)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a3a0::

            push DWORD PTR [EBP+8]
            call ??3@YAXPAX@Z

            pop ECX
            ret 
$L_64a3aa::

            mov EAX,OFFSET (_KingRdata+122168)
            jmp ___CxxFrameHandler
$L_64a3b4::

            mov EAX,OFFSET (_KingRdata+122208)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a3c0::

            mov ECX,DWORD PTR [EBP-16]
            jmp @CmdTarget_Destruct@4
$L_64a3c8::

            mov EAX,OFFSET (_KingRdata+122296)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a3d4::

            mov EAX,OFFSET (_KingRdata+122336)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a3e0::

            lea ECX,DWORD PTR [EBP-8]
            jmp $L_61c730
$L_64a3e8::

            mov EAX,OFFSET (_KingRdata+122424)
            jmp ___CxxFrameHandler

            int 3

            int 3
$L_64a3f4::

            lea ECX,DWORD PTR [EBP-36]
            jmp @Cdc_Destruct@4
$L_64a296 ENDP
_TEXT ENDS
END
