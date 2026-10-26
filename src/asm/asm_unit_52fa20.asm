; asm_unit_52fa20.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_64726b:NEAR
EXTERN @Car_555500@8:PROC
EXTERN @DpMessage_Construct@8:PROC
EXTERN @Int_Copy@12:PROC
EXTERN @Media_RecordBlock@12:PROC
EXTERN @MsgBuf_Destroy@8:PROC
EXTERN @Msg_Write120@20:PROC
EXTERN @Ramp_Value@8:PROC
EXTERN _Ai_HitPlayer:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _Vec3_Length:PROC

PUBLIC $L_530065, $L_546278, $L_554c13

_TEXT SEGMENT
$L_530065 LABEL NEAR
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
FUN_546240 PROC

            push -1
            push OFFSET $L_64726b
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,428
            push ESI
            mov ESI,ECX
            mov DWORD PTR [ESP+36],11
            mov EAX,DWORD PTR [ESI+1204]
            lea ECX,DWORD PTR [ESI+21052]
            mov DWORD PTR [ESP+32],EAX
            xor EAX,EAX
$L_546278::

            mov DL,BYTE PTR [ECX]
            add ECX,4
            mov BYTE PTR [ESP+EAX*1+40],DL
            inc EAX
            cmp EAX,17
            jl $L_546278

            mov ECX,ESI
            call @Car_555500@8

            fst DWORD PTR [ESI+10416]
            fstp DWORD PTR [ESP+57]
            lea ECX,DWORD PTR [ESP+4]
            call @DpMessage_Construct@8

            movsx ECX,WORD PTR [(_KingData+69898)]
            lea EAX,DWORD PTR [ESP+32]
            push 29
            push EAX
            push ECX
            lea ECX,DWORD PTR [ESP+16]
            mov DWORD PTR [ESP+452],0
            call @Msg_Write120@20

            mov ECX,DWORD PTR [(_KingBss+412124)]
            lea EDX,DWORD PTR [ESP+4]
            push EDX
            call @Media_RecordBlock@12

            lea ECX,DWORD PTR [ESP+4]
            mov DWORD PTR [ESP+440],4294967295
            call @MsgBuf_Destroy@8

            mov ECX,DWORD PTR [ESP+432]
            pop ESI
            mov DWORD PTR FS:[0],ECX
            add ESP,440
            ret 
FUN_546240 ENDP
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
$L_554bb0 PROC

            sub ESP,8
            mov EAX,DWORD PTR [ESP+16]
            push ESI
            push EDI
            mov ESI,ECX
            push EAX
            call _Vec3_Length

            fdiv DWORD PTR [ESI+412]
            mov ECX,DWORD PTR [ESI+20636]
            mov EAX,DWORD PTR [(_KingBss+4464)]
            lea EDI,DWORD PTR [ESI+20636]
            add ESP,4
            cmp ECX,EAX
            fstp DWORD PTR [ESP+24]
            je $L_554c13

            lea ECX,DWORD PTR [ESI+21204]
            call @Ramp_Value@8

            fmul DWORD PTR [ESP+24]
            push 0
            fstp QWORD PTR [ESP+12]
            mov EDX,DWORD PTR [ESP+16]
            mov EAX,DWORD PTR [ESP+12]
            push EDX
            push EAX
            push ECX
            mov ECX,ESP
            push EDI
            call @Int_Copy@12

            call _Ai_HitPlayer

            add ESP,16
$L_554c13::

            pop EDI
            pop ESI
            add ESP,8
            ret 8
$L_554bb0 ENDP
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
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
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
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
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
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
