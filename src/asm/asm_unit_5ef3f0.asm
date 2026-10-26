; asm_unit_5ef3f0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_649afb:NEAR
EXTERN ??2@YAPAXI@Z:NEAR
EXTERN ??3@YAXPAX@Z:NEAR
EXTERN ??_L@YGXPAXIHP6EX0@Z1@Z:NEAR
EXTERN @FlatFace_Read@12:PROC
EXTERN @FxRec_Free@4:PROC
EXTERN @FxRec_Init@4:PROC
EXTERN @Plane_Read@12:PROC
EXTERN @Vec3_Read@12:PROC
EXTERN _File_ReadInt:PROC
EXTERN _Static5ef4a0_AtExit:PROC
EXTERN _Static722940_AtExit:PROC
EXTERN _Static722940_Construct:PROC
EXTERN _Static723978_Construct:PROC

PUBLIC $L_5fa585, $L_5fa59b, $L_5fa5b1

_TEXT SEGMENT
$L_5ef3f0 PROC

            call _Static723978_Construct

            jmp _Static5ef4a0_AtExit
$L_5ef3f0 ENDP
ALIGN 16
ALIGN 16
$L_5ef4c0 PROC

            call _Static722940_Construct

            jmp _Static722940_AtExit
$L_5ef4c0 ENDP
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
$L_5f2dc0 PROC

            ret 
$L_5f2dc0 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; the element destructor that shares the original's code stays in assembly
$L_5f4b20 PROC
            mov EAX,DWORD PTR [ECX+4]
            push EAX
            call ??3@YAXPAX@Z
            pop ECX
            ret 
$L_5f4b20 ENDP
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
$L_5fa510 PROC

            mov EAX,DWORD PTR FS:[0]
            push -1
            push OFFSET $L_649afb
            push EAX
            mov DWORD PTR FS:[0],ESP
            push EBX
            push EBP
            mov EBP,DWORD PTR [ESP+24]
            push ESI
            mov ESI,ECX
            push EDI
            push EBP
            lea ECX,DWORD PTR [ESI+20]
            call @Plane_Read@12

            push EBP
            lea ECX,DWORD PTR [ESI+56]
            call @Vec3_Read@12

            push EBP
            call _File_ReadInt

            mov EDI,EAX
            mov DWORD PTR [ESI+48],EDI
            lea EAX,DWORD PTR [EDI+EDI*4]
            lea ECX,DWORD PTR [EAX*8+4]
            push ECX
            call ??2@YAPAXI@Z

            add ESP,8
            mov DWORD PTR [ESP+32],EAX
            xor EBX,EBX
            cmp EAX,EBX
            mov DWORD PTR [ESP+24],EBX
            je $L_5fa585

            push OFFSET @FxRec_Free@4
            push OFFSET @FxRec_Init@4
            lea EBX,DWORD PTR [EAX+4]
            push EDI
            push 40
            push EBX
            mov DWORD PTR [EAX],EDI
            call ??_L@YGXPAXIHP6EX0@Z1@Z
$L_5fa585::

            mov EAX,DWORD PTR [ESI+48]
            xor EDI,EDI
            test EAX,EAX
            mov DWORD PTR [ESP+24],4294967295
            mov DWORD PTR [ESI+52],EBX
            jle $L_5fa5b1

            xor EBX,EBX
$L_5fa59b::

            mov ECX,DWORD PTR [ESI+52]
            push EBP
            add ECX,EBX
            call @FlatFace_Read@12

            mov EAX,DWORD PTR [ESI+48]
            inc EDI
            add EBX,40
            cmp EDI,EAX
            jl $L_5fa59b
$L_5fa5b1::

            mov ECX,DWORD PTR [ESP+16]
            pop EDI
            pop ESI
            pop EBP
            mov EAX,1
            mov DWORD PTR FS:[0],ECX
            pop EBX
            add ESP,12
            ret 12
$L_5fa510 ENDP
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
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
_TEXT ENDS
END
