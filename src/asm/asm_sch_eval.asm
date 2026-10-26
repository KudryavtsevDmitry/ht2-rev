; asm_sch_eval.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Dq_ReserveBackPages@12:NEAR
EXTERN @Dq_ReserveFrontPages@12:NEAR
EXTERN $L_643f10:NEAR
EXTERN @DequeIt_Advance@12:PROC
EXTERN @DequeIt_Diff@12:PROC
EXTERN @Sch_Copy16@12:PROC
EXTERN _Ai_PoolFree:PROC
EXTERN _Deque4acc20:PROC
EXTERN _Deque4acd70:PROC
EXTERN _Deque_Copy4acb40:PROC
EXTERN _Deque_Copy4acb90:PROC
EXTERN _Deque_CopyBackward:PROC
EXTERN _Deque_CopyChars4acbe0:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN _free:PROC

PUBLIC $L_4ac450, $L_4ac5d6, $L_4ac66c, $L_4ac66d, $L_4ac695, $L_4ac6ac
PUBLIC $L_4ac6d1, $L_4ac862, $L_4ac909, $L_4ac91b

_TEXT SEGMENT
FUN_4ac3d0 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_643f10
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,108
            mov EAX,DWORD PTR [EBP+20]
            push EBX
            mov EBX,DWORD PTR [ECX+20]
            push ESI
            push EDI
            lea EDI,DWORD PTR [ECX+8]
            mov ESI,DWORD PTR [EBP+12]
            sub EAX,EBX
            mov EBX,DWORD PTR [EDI+8]
            mov EDX,DWORD PTR [EDI]
            sar EAX,2
            shl EAX,9
            add EAX,EBX
            mov DWORD PTR [EBP-16],ESP
            sub EAX,ESI
            lea ESI,DWORD PTR [ECX+24]
            sub EAX,EDX
            mov EDX,DWORD PTR [EBP+8]
            mov DWORD PTR [EBP-20],ECX
            push EDI
            mov ECX,ESI
            lea EBX,DWORD PTR [EAX+EDX*1-512]
            call @DequeIt_Diff@12

            mov DWORD PTR [EBP-24],EAX
            shr EAX,1
            cmp EBX,EAX
            jae $L_4ac6b5

            mov ECX,DWORD PTR [EBP-20]
            mov EAX,DWORD PTR [EDI]
            mov ESI,DWORD PTR [ECX+12]
            sub EAX,ESI
            mov ESI,DWORD PTR [EBP+32]
            cmp ESI,EAX
            jbe $L_4ac450

            mov EDX,ESI
            sub EDX,EAX
            push EDX
            call @Dq_ReserveFrontPages@12
$L_4ac450::

            push EDI
            lea ECX,DWORD PTR [EBP-88]
            call @Sch_Copy16@12

            mov EAX,ESI
            lea ECX,DWORD PTR [EBP-88]
            neg EAX
            push EAX
            mov DWORD PTR [EBP-24],EAX
            call @DequeIt_Advance@12

            push EAX
            lea ECX,DWORD PTR [EBP-40]
            call @Sch_Copy16@12

            mov EAX,DWORD PTR [EDI]
            mov ECX,DWORD PTR [EDI+4]
            mov EDX,DWORD PTR [EDI+8]
            mov DWORD PTR [EBP-56],EAX
            mov EAX,DWORD PTR [EDI+12]
            mov DWORD PTR [EBP-52],ECX
            push EDI
            lea ECX,DWORD PTR [EBP-88]
            mov DWORD PTR [EBP-48],EDX
            mov DWORD PTR [EBP-44],EAX
            call @Sch_Copy16@12

            push EBX
            lea ECX,DWORD PTR [EBP-88]
            call @DequeIt_Advance@12

            push EAX
            lea ECX,DWORD PTR [EBP-72]
            call @Sch_Copy16@12

            mov ECX,DWORD PTR [EBP-72]
            mov EDX,DWORD PTR [EBP-68]
            mov EAX,DWORD PTR [EBP-64]
            mov DWORD PTR [EBP+8],ECX
            mov ECX,DWORD PTR [EBP-60]
            cmp EBX,ESI
            mov DWORD PTR [EBP+12],EDX
            mov DWORD PTR [EBP+16],EAX
            mov DWORD PTR [EBP+20],ECX
            mov DWORD PTR [EBP-4],0
            jl $L_4ac5d6

            push EDI
            lea ECX,DWORD PTR [EBP-72]
            call @Sch_Copy16@12

            push ESI
            lea ECX,DWORD PTR [EBP-72]
            call @DequeIt_Advance@12

            push EAX
            lea ECX,DWORD PTR [EBP-88]
            call @Sch_Copy16@12

            mov EDX,DWORD PTR [EBP-40]
            sub ESP,16
            mov EAX,ESP
            mov DWORD PTR [EBP+32],ESP
            sub ESP,16
            mov DWORD PTR [EAX],EDX
            mov ECX,DWORD PTR [EBP-36]
            mov DWORD PTR [EAX+4],ECX
            mov EDX,DWORD PTR [EBP-32]
            mov DWORD PTR [EAX+8],EDX
            mov ECX,DWORD PTR [EBP-28]
            mov DWORD PTR [EAX+12],ECX
            lea EDX,DWORD PTR [EBP-88]
            mov ECX,ESP
            mov DWORD PTR [EBP+32],ESP
            push EDX
            call @Sch_Copy16@12

            sub ESP,16
            mov ECX,ESP
            mov DWORD PTR [EBP+32],ESP
            push EDI
            call @Sch_Copy16@12

            lea EAX,DWORD PTR [EBP-72]
            push EAX
            call _Deque_Copy4acb40

            mov ECX,DWORD PTR [EBP-40]
            mov EDX,DWORD PTR [EBP-36]
            mov EAX,DWORD PTR [EBP-32]
            add ESP,52
            mov DWORD PTR [EDI],ECX
            mov ECX,DWORD PTR [EBP-28]
            mov DWORD PTR [EDI+4],EDX
            mov EDX,DWORD PTR [EBP-56]
            sub ESP,16
            mov DWORD PTR [EDI+8],EAX
            mov EAX,ESP
            mov DWORD PTR [EDI+12],ECX
            mov ECX,DWORD PTR [EBP-52]
            mov DWORD PTR [EBP+32],ESP
            mov DWORD PTR [EAX],EDX
            mov EDX,DWORD PTR [EBP-48]
            mov DWORD PTR [EAX+4],ECX
            mov ECX,DWORD PTR [EBP-44]
            mov DWORD PTR [EAX+8],EDX
            sub ESP,16
            mov DWORD PTR [EAX+12],ECX
            lea EDX,DWORD PTR [EBP+8]
            mov ECX,ESP
            mov DWORD PTR [EBP+32],ESP
            push EDX
            call @Sch_Copy16@12

            sub ESP,16
            lea EAX,DWORD PTR [EBP-88]
            mov ECX,ESP
            mov DWORD PTR [EBP+32],ESP
            push EAX
            call @Sch_Copy16@12

            lea ECX,DWORD PTR [EBP-72]
            push ECX
            call _Deque_Copy4acb90

            mov EDX,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EBP+12]
            mov ECX,DWORD PTR [EBP+16]
            mov DWORD PTR [EBP-72],EDX
            mov EDX,DWORD PTR [EBP+20]
            add ESP,36
            mov DWORD PTR [EBP-68],EAX
            mov EAX,DWORD PTR [EBP-24]
            mov DWORD PTR [EBP+32],ESP
            mov ESI,ESP
            mov DWORD PTR [EBP-64],ECX
            mov DWORD PTR [EBP-60],EDX
            push EAX
            lea ECX,DWORD PTR [EBP-72]
            call @DequeIt_Advance@12

            push EAX
            mov ECX,ESI
            call @Sch_Copy16@12

            mov ECX,DWORD PTR [EBP+28]
            mov EDX,DWORD PTR [EBP+24]
            push ECX
            lea EAX,DWORD PTR [EBP-104]
            push EDX
            push EAX
            jmp $L_4ac66d
$L_4ac5d6::

            mov ECX,DWORD PTR [EBP+24]
            mov EAX,DWORD PTR [EBP+32]
            mov EDX,DWORD PTR [EBP-40]
            mov ESI,ECX
            sub ESI,EBX
            sub ESP,16
            add ESI,EAX
            mov EAX,ESP
            mov DWORD PTR [EBP+24],ESP
            push ESI
            mov DWORD PTR [EAX],EDX
            mov EDX,DWORD PTR [EBP-36]
            mov DWORD PTR [EAX+4],EDX
            mov EDX,DWORD PTR [EBP-32]
            mov DWORD PTR [EAX+8],EDX
            mov EDX,DWORD PTR [EBP-28]
            push ECX
            mov DWORD PTR [EAX+12],EDX
            sub ESP,16
            lea EAX,DWORD PTR [EBP+8]
            mov ECX,ESP
            mov DWORD PTR [EBP+24],ESP
            push EAX
            call @Sch_Copy16@12

            sub ESP,16
            mov ECX,ESP
            mov DWORD PTR [EBP+24],ESP
            push EDI
            call @Sch_Copy16@12

            lea ECX,DWORD PTR [EBP-104]
            push ECX
            call _Deque4acc20

            mov EDX,DWORD PTR [EBP-40]
            mov EAX,DWORD PTR [EBP-36]
            mov ECX,DWORD PTR [EBP-32]
            add ESP,60
            mov DWORD PTR [EDI],EDX
            mov EDX,DWORD PTR [EBP-28]
            mov DWORD PTR [EDI+4],EAX
            sub ESP,16
            mov DWORD PTR [EDI+8],ECX
            mov ECX,DWORD PTR [EBP-56]
            mov DWORD PTR [EDI+12],EDX
            mov EDX,DWORD PTR [EBP-52]
            mov EAX,ESP
            mov DWORD PTR [EBP+24],ESP
            mov DWORD PTR [EAX],ECX
            mov ECX,DWORD PTR [EBP-48]
            mov DWORD PTR [EAX+4],EDX
            mov EDX,DWORD PTR [EBP-44]
            mov DWORD PTR [EAX+8],ECX
            mov DWORD PTR [EAX+12],EDX
            mov EAX,DWORD PTR [EBP+28]
            lea ECX,DWORD PTR [EBP-104]
            push EAX
            push ESI
$L_4ac66c::

            push ECX
$L_4ac66d::

            call _Deque_CopyChars4acbe0

            mov ECX,DWORD PTR [EBP-12]
            add ESP,28
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 28
FUN_4ac3d0 ENDP
$L_4ac688 PROC

            mov EDX,DWORD PTR [EBP-20]
            mov ESI,DWORD PTR [EBP-28]
            mov EDI,DWORD PTR [EDX+20]
            cmp ESI,EDI
            jae $L_4ac6ac
$L_4ac695::

            mov EAX,DWORD PTR [ESI]
            push 512
            push EAX
            call _Ai_PoolFree

            add ESI,4
            add ESP,8
            cmp ESI,EDI
            jb $L_4ac695
$L_4ac6ac::

            push 0
            push 0
            call __CxxThrowException@8
$L_4ac688 ENDP
$L_4ac6b5 PROC

            mov ECX,DWORD PTR [EBP-20]
            mov EDX,DWORD PTR [ESI]
            mov EDI,DWORD PTR [EBP+32]
            mov EAX,DWORD PTR [ECX+32]
            sub EAX,EDX
            dec EAX
            cmp EDI,EAX
            jbe $L_4ac6d1

            mov EDX,EDI
            sub EDX,EAX
            push EDX
            call @Dq_ReserveBackPages@12
$L_4ac6d1::

            push ESI
            lea ECX,DWORD PTR [EBP-88]
            call @Sch_Copy16@12

            push EDI
            lea ECX,DWORD PTR [EBP-88]
            call @DequeIt_Advance@12

            push EAX
            lea ECX,DWORD PTR [EBP-56]
            call @Sch_Copy16@12

            mov EAX,DWORD PTR [ESI]
            mov EDX,DWORD PTR [ESI+8]
            mov ECX,DWORD PTR [ESI+4]
            mov DWORD PTR [EBP-40],EAX
            mov EAX,DWORD PTR [ESI+12]
            mov DWORD PTR [EBP-32],EDX
            mov EDX,DWORD PTR [ESI+4]
            mov DWORD PTR [EBP-28],EAX
            mov EAX,DWORD PTR [EBP-24]
            mov DWORD PTR [EBP-36],ECX
            mov ECX,DWORD PTR [ESI]
            sub EAX,EBX
            mov EBX,EAX
            mov EAX,DWORD PTR [ESI+8]
            mov DWORD PTR [EBP-68],EDX
            mov DWORD PTR [EBP-72],ECX
            mov ECX,DWORD PTR [ESI+12]
            mov EDX,EBX
            neg EDX
            mov DWORD PTR [EBP-60],ECX
            push EDX
            lea ECX,DWORD PTR [EBP-72]
            mov DWORD PTR [EBP-64],EAX
            call @DequeIt_Advance@12

            push EAX
            lea ECX,DWORD PTR [EBP-88]
            call @Sch_Copy16@12

            mov EAX,DWORD PTR [EBP-88]
            mov ECX,DWORD PTR [EBP-84]
            mov EDX,DWORD PTR [EBP-80]
            mov DWORD PTR [EBP+8],EAX
            mov EAX,DWORD PTR [EBP-76]
            cmp EBX,EDI
            mov DWORD PTR [EBP+12],ECX
            mov DWORD PTR [EBP+16],EDX
            mov DWORD PTR [EBP+20],EAX
            mov DWORD PTR [EBP-4],2
            jle $L_4ac862

            mov ECX,DWORD PTR [ESI]
            mov EDX,DWORD PTR [ESI+4]
            mov EAX,DWORD PTR [ESI+8]
            mov DWORD PTR [EBP-88],ECX
            mov ECX,DWORD PTR [ESI+12]
            mov DWORD PTR [EBP-84],EDX
            neg EDI
            mov DWORD PTR [EBP-76],ECX
            push EDI
            lea ECX,DWORD PTR [EBP-88]
            mov DWORD PTR [EBP-80],EAX
            call @DequeIt_Advance@12

            push EAX
            lea ECX,DWORD PTR [EBP-72]
            call @Sch_Copy16@12

            mov EDX,DWORD PTR [ESI]
            sub ESP,16
            mov EAX,ESP
            mov DWORD PTR [EBP+32],ESP
            sub ESP,16
            mov DWORD PTR [EAX],EDX
            mov ECX,DWORD PTR [ESI+4]
            mov DWORD PTR [EAX+4],ECX
            mov EDX,DWORD PTR [ESI+8]
            mov DWORD PTR [EAX+8],EDX
            mov ECX,DWORD PTR [ESI+12]
            mov DWORD PTR [EAX+12],ECX
            mov ECX,ESP
            mov DWORD PTR [EBP+32],ESP
            push ESI
            call @Sch_Copy16@12

            sub ESP,16
            lea EDX,DWORD PTR [EBP-72]
            mov ECX,ESP
            mov DWORD PTR [EBP+32],ESP
            push EDX
            call @Sch_Copy16@12

            lea EAX,DWORD PTR [EBP-104]
            push EAX
            call _Deque_Copy4acb40

            mov ECX,DWORD PTR [EBP-56]
            mov EDX,DWORD PTR [EBP-52]
            mov EAX,DWORD PTR [EBP-48]
            add ESP,52
            mov DWORD PTR [ESI],ECX
            mov ECX,DWORD PTR [EBP-44]
            mov DWORD PTR [ESI+4],EDX
            mov EDX,DWORD PTR [EBP-40]
            sub ESP,16
            mov DWORD PTR [ESI+8],EAX
            mov EAX,ESP
            mov DWORD PTR [ESI+12],ECX
            mov ECX,DWORD PTR [EBP-36]
            mov DWORD PTR [EBP+32],ESP
            mov DWORD PTR [EAX],EDX
            mov EDX,DWORD PTR [EBP-32]
            mov DWORD PTR [EAX+4],ECX
            mov ECX,DWORD PTR [EBP-28]
            mov DWORD PTR [EAX+8],EDX
            sub ESP,16
            mov DWORD PTR [EAX+12],ECX
            lea EDX,DWORD PTR [EBP-72]
            mov ECX,ESP
            mov DWORD PTR [EBP+32],ESP
            push EDX
            call @Sch_Copy16@12

            sub ESP,16
            lea EAX,DWORD PTR [EBP+8]
            mov ECX,ESP
            mov DWORD PTR [EBP+32],ESP
            push EAX
            call @Sch_Copy16@12

            lea ECX,DWORD PTR [EBP-104]
            push ECX
            call _Deque_CopyBackward

            mov EDX,DWORD PTR [EBP+8]
            add ESP,36
            mov EAX,ESP
            mov DWORD PTR [EBP+32],ESP
            mov DWORD PTR [EAX],EDX
            mov ECX,DWORD PTR [EBP+12]
            mov DWORD PTR [EAX+4],ECX
            mov EDX,DWORD PTR [EBP+16]
            mov DWORD PTR [EAX+8],EDX
            mov ECX,DWORD PTR [EBP+20]
            mov EDX,DWORD PTR [EBP+28]
            mov DWORD PTR [EAX+12],ECX
            mov EAX,DWORD PTR [EBP+24]
            push EDX
            push EAX
            lea ECX,DWORD PTR [EBP-104]
            jmp $L_4ac66c
$L_4ac862::

            mov EDX,DWORD PTR [ESI]
            sub ESP,16
            mov EAX,ESP
            mov EDI,DWORD PTR [EBP+24]
            mov DWORD PTR [EBP+24],ESP
            sub ESP,16
            mov DWORD PTR [EAX],EDX
            mov ECX,DWORD PTR [ESI+4]
            mov DWORD PTR [EAX+4],ECX
            mov EDX,DWORD PTR [ESI+8]
            mov DWORD PTR [EAX+8],EDX
            mov ECX,DWORD PTR [ESI+12]
            mov DWORD PTR [EAX+12],ECX
            mov ECX,ESP
            mov DWORD PTR [EBP+24],ESP
            push ESI
            add EBX,EDI
            call @Sch_Copy16@12

            sub ESP,16
            lea EDX,DWORD PTR [EBP+8]
            mov ECX,ESP
            mov DWORD PTR [EBP+24],ESP
            push EDX
            call @Sch_Copy16@12

            mov EAX,DWORD PTR [EBP+28]
            lea ECX,DWORD PTR [EBP-104]
            push EAX
            push EBX
            push ECX
            call _Deque4acd70

            mov EDX,DWORD PTR [EBP-56]
            mov EAX,DWORD PTR [EBP-52]
            mov ECX,DWORD PTR [EBP-48]
            add ESP,60
            mov DWORD PTR [ESI],EDX
            mov EDX,DWORD PTR [EBP-44]
            sub ESP,16
            mov DWORD PTR [ESI+4],EAX
            mov DWORD PTR [ESI+8],ECX
            mov ECX,DWORD PTR [EBP+8]
            mov EAX,ESP
            mov DWORD PTR [ESI+12],EDX
            mov DWORD PTR [EBP+24],ESP
            push EBX
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [EBP+12]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [EBP+16]
            mov DWORD PTR [EAX+8],ECX
            mov EDX,DWORD PTR [EBP+20]
            mov DWORD PTR [EAX+12],EDX
            lea EAX,DWORD PTR [EBP-120]
            push EDI
            push EAX
            jmp $L_4ac66d
$L_4ac6b5 ENDP
$L_4ac8f6 PROC

            mov EDX,DWORD PTR [EBP-20]
            mov ECX,DWORD PTR [EBP-44]
            mov ESI,DWORD PTR [EDX+36]
            lea EDI,DWORD PTR [ECX+4]
            add ESI,4
            cmp ESI,EDI
            jae $L_4ac91b
$L_4ac909::

            mov EAX,DWORD PTR [ESI]
            push EAX
            call _free

            add ESI,4
            add ESP,4
            cmp ESI,EDI
            jb $L_4ac909
$L_4ac91b::

            push 0
            push 0
            call __CxxThrowException@8

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
$L_4ac8f6 ENDP
ALIGN 16
_TEXT ENDS
END
