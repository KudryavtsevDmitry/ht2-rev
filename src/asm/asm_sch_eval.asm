; asm_sch_eval.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_643b18:NEAR
EXTERN $L_643f10:NEAR
EXTERN $L_643f20:NEAR
EXTERN $L_643f30:NEAR
EXTERN @DequeIt_Advance@12:PROC
EXTERN @DequeIt_Diff@12:PROC
EXTERN @Deque_ReallocMap@16:PROC
EXTERN @Sch_Copy16@12:PROC
EXTERN _Ai_OperatorNew:PROC
EXTERN _Ai_PoolFree:PROC
EXTERN _Deque4acc20:PROC
EXTERN _Deque4acd70:PROC
EXTERN _Deque_Copy4acb40:PROC
EXTERN _Deque_Copy4acb90:PROC
EXTERN _Deque_CopyBackward:PROC
EXTERN _Deque_CopyChars4acbe0:PROC
EXTERN _Deque_CopyChars4ad0f0:PROC
EXTERN _Sch4a5c40_Append:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN _free:PROC
EXTERN _malloc:PROC

PUBLIC $L_4a8b25, $L_4a8b44, $L_4a8b4c, $L_4a8baa
PUBLIC $L_4ac450, $L_4ac5d6, $L_4ac66c, $L_4ac66d, $L_4ac695, $L_4ac6ac
PUBLIC $L_4ac6d1, $L_4ac862, $L_4ac909, $L_4ac91b, $L_4ac97a, $L_4ac986, $L_4ac9ab, $L_4ac9cf
PUBLIC $L_4ac9e5, $L_4aca62, $L_4aca6e, $L_4aca93, $L_4acaac, $L_4acac0

_TEXT SEGMENT
$L_4a8ae0 PROC

            mov EAX,DWORD PTR FS:[0]
            push -1
            push OFFSET $L_643b18
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,12
            push ESI
            mov ESI,DWORD PTR [ESP+32]
            push EDI
            xor EDI,EDI
            mov EAX,DWORD PTR [ESI+112]
            mov EAX,DWORD PTR [EAX-40]
            mov ECX,DWORD PTR [EAX+28]
            test ECX,65536
            je $L_4a8b25

            mov DWORD PTR [ESP+16],ECX
            mov ECX,DWORD PTR [EAX+20]
            mov DWORD PTR [ESP+8],ECX
            mov EDX,DWORD PTR [EAX+24]
            mov DWORD PTR [ESP+12],EDX
            jmp $L_4a8b4c
$L_4a8b25::

            mov DWORD PTR [ESP+12],EDI
            mov ECX,DWORD PTR [EAX+20]
            cmp ECX,EDI
            je $L_4a8b44

            mov DWORD PTR [ESP+8],ECX
            mov EAX,DWORD PTR [EAX+28]
            mov DWORD PTR [ESP+16],EAX
            mov EAX,DWORD PTR [ECX+4]
            inc EAX
            mov DWORD PTR [ECX+4],EAX
            jmp $L_4a8b4c
$L_4a8b44::

            mov DWORD PTR [ESP+8],EDI
            mov DWORD PTR [ESP+16],EDI
$L_4a8b4c::

            mov ECX,DWORD PTR [ESI+112]
            push EDI
            add ECX,-44
            mov DWORD PTR [ESP+32],EDI
            mov DWORD PTR [ESI+112],ECX
            mov EDX,DWORD PTR [ECX]
            call DWORD PTR [EDX+4]

            mov EAX,DWORD PTR [ESI+112]
            lea ECX,DWORD PTR [ESP+8]
            add EAX,-44
            push ECX
            push EAX
            push ESI
            call _Sch4a5c40_Append

            mov EAX,DWORD PTR [ESP+28]
            add ESP,12
            test EAX,65536
            mov DWORD PTR [ESP+28],4294967295
            jne $L_4a8baa

            mov EAX,DWORD PTR [ESP+8]
            cmp EAX,EDI
            je $L_4a8baa

            mov ECX,DWORD PTR [EAX+4]
            dec ECX
            mov DWORD PTR [EAX+4],ECX
            mov ECX,DWORD PTR [ESP+8]
            cmp DWORD PTR [ECX+4],EDI
            jne $L_4a8baa

            cmp ECX,EDI
            je $L_4a8baa

            mov EDX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EDX+12]
$L_4a8baa::

            mov ECX,DWORD PTR [ESP+20]
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            add ESP,24
            ret 4
$L_4a8ae0 ENDP
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
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
            call FUN_4ac930
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
            call FUN_4aca10
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
FUN_4ac930 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_643f20
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,8
            mov EAX,DWORD PTR [EBP+8]
            push EBX
            push ESI
            push EDI
            mov EDI,ECX
            lea EBX,DWORD PTR [EAX+511]
            shr EBX,9
            mov ECX,DWORD PTR [EDI+20]
            mov ESI,DWORD PTR [EDI]
            sub ECX,ESI
            mov DWORD PTR [EBP-16],ESP
            sar ECX,2
            cmp EBX,ECX
            mov DWORD PTR [EBP-20],EDI
            jbe $L_4ac97a

            push 1
            push EBX
            mov ECX,EDI
            call @Deque_ReallocMap@16
$L_4ac97a::

            mov DWORD PTR [EBP-4],0
            mov ESI,1
$L_4ac986::

            cmp ESI,EBX
            mov DWORD PTR [EBP+8],ESI
            ja $L_4ac9ee

            push 512
            call _malloc

            add ESP,4
            test EAX,EAX
            jne $L_4ac9ab

            push 512
            call _Ai_OperatorNew

            add ESP,4
$L_4ac9ab::

            mov EDX,DWORD PTR [EDI+20]
            lea ECX,DWORD PTR [ESI*4]
            sub EDX,ECX
            inc ESI
            mov DWORD PTR [EDX],EAX
            jmp $L_4ac986
FUN_4ac930 ENDP
$L_4ac9bc PROC

            mov EAX,DWORD PTR [EBP+8]
            cmp EAX,1
            jbe $L_4ac9e5

            mov EBX,DWORD PTR [EBP-20]
            mov ESI,4
            lea EDI,DWORD PTR [EAX-1]
$L_4ac9cf::

            mov EDX,DWORD PTR [EBX+20]
            sub EDX,ESI
            mov EAX,DWORD PTR [EDX]
            push EAX
            call _free

            add ESP,4
            add ESI,4
            dec EDI
            jne $L_4ac9cf
$L_4ac9e5::

            push 0
            push 0
            call __CxxThrowException@8
$L_4ac9bc ENDP
$L_4ac9ee PROC

            mov ECX,DWORD PTR [EBP-12]
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 4
$L_4ac9ee ENDP
ALIGN 16
FUN_4aca10 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_643f30
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,8
            mov EAX,DWORD PTR [EBP+8]
            push EBX
            push ESI
            push EDI
            mov EDI,ECX
            lea EBX,DWORD PTR [EAX+511]
            shr EBX,9
            mov ECX,DWORD PTR [EDI+36]
            mov EDX,DWORD PTR [EDI]
            sub ECX,EDX
            mov EDX,DWORD PTR [EDI+4]
            sar ECX,2
            sub EDX,ECX
            lea EAX,DWORD PTR [EBX+1]
            cmp EAX,EDX
            mov DWORD PTR [EBP-16],ESP
            mov DWORD PTR [EBP-20],EDI
            jbe $L_4aca62

            push 0
            push EBX
            mov ECX,EDI
            call @Deque_ReallocMap@16
$L_4aca62::

            mov DWORD PTR [EBP-4],0
            mov ESI,1
$L_4aca6e::

            cmp ESI,EBX
            mov DWORD PTR [EBP+8],ESI
            ja $L_4acac9

            push 512
            call _malloc

            add ESP,4
            test EAX,EAX
            jne $L_4aca93

            push 512
            call _Ai_OperatorNew

            add ESP,4
$L_4aca93::

            mov EDX,DWORD PTR [EDI+36]
            inc ESI
            mov DWORD PTR [EDX+ESI*4-4],EAX
            jmp $L_4aca6e
FUN_4aca10 ENDP
$L_4aca9d PROC

            mov EDI,DWORD PTR [EBP+8]
            mov ESI,1
            cmp EDI,ESI
            jbe $L_4acac0

            mov EBX,DWORD PTR [EBP-20]
$L_4acaac::

            mov ECX,DWORD PTR [EBX+36]
            mov EAX,DWORD PTR [ECX+ESI*4]
            push EAX
            call _free

            add ESP,4
            inc ESI
            cmp ESI,EDI
            jb $L_4acaac
$L_4acac0::

            push 0
            push 0
            call __CxxThrowException@8
$L_4aca9d ENDP
$L_4acac9 PROC

            mov ECX,DWORD PTR [EBP-12]
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 4
$L_4acac9 ENDP
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
FUN_4ace70 PROC

            mov EAX,DWORD PTR [ESP+12]
            push ESI
            push EAX
            lea EDX,DWORD PTR [ESP+24]
            sub ESP,16
            mov ECX,ESP
            push EDX
            call @Sch_Copy16@12

            mov EAX,DWORD PTR [ESP+36]
            mov ECX,DWORD PTR [ESP+32]
            mov ESI,DWORD PTR [ESP+28]
            push EAX
            push ECX
            push ESI
            call _Deque_CopyChars4ad0f0

            add ESP,32
            mov EAX,ESI
            pop ESI
            ret 
FUN_4ace70 ENDP
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
ALIGN 16
ALIGN 16
; AI script evaluator: src\script\evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
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
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; AI script items: src\script\values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
_TEXT ENDS
END
