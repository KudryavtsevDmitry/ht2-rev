; asm_unit_458bc0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN _Rope_TreeConcat:NEAR
EXTERN @Stl_LengthError@4:NEAR
EXTERN $L_63fe18:NEAR
EXTERN $L_63fe8f:NEAR
EXTERN ??0exception@@QAE@XZ:NEAR
EXTERN ??1exception@@UAE@XZ:NEAR
EXTERN @Buf459df0_Destruct@8:PROC
EXTERN @Rec60_Copy@12:PROC
EXTERN @RopeSubstr_Destroy@8:PROC
EXTERN @Rope_FreeTree@8:PROC
EXTERN @Rope_Unref@8:PROC
EXTERN @Str_FreeData@8:PROC
EXTERN @Tree_DestroyRoot@8:PROC
EXTERN _Ai_PoolFree:PROC
EXTERN _Copy1:PROC
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _LexCompare:PROC
EXTERN _Rec60_Call4667d0:PROC
EXTERN _Rec60_Call4668d0:PROC
EXTERN _Rec60_Call466a00:PROC
EXTERN _Rope_AddLeafToForest:PROC
EXTERN _SchString_Concat:PROC
EXTERN _Stl_Allocate:PROC
EXTERN _Str_FreeRounded:PROC
EXTERN _Tree_EraseRight:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN __imp__InterlockedDecrement@4:PROC
EXTERN __imp__InterlockedIncrement@4:PROC
EXTERN _free:PROC
EXTERN _strncpy:NEAR

PUBLIC $L_45a7fb, $L_45a813, $L_45a848, $L_45a8a4
PUBLIC $L_45a900
PUBLIC $L_4628a1
PUBLIC $L_4628c4, $L_4628c9, $L_46290d, $L_462938, $L_46293b, $L_46293e, $L_462995, $L_4629ab
PUBLIC $L_4629b6, $L_462a44, $L_462a5f, $L_462e30, $L_462e66
PUBLIC $L_462e83, $L_462e9c, $L_462eb2, $L_462ecf, $L_462ee5, $L_462ef9, $L_462f06, $L_462f1b
PUBLIC $L_462f20, $L_462f69, $L_462f7c, $L_462fa0, $L_462fe5, $L_463035, $L_46306c, $L_463088
PUBLIC $L_4630e2, $L_4630f3, $L_46310c, $L_463127, $L_463142, $L_463146, $L_463152, $L_46315b
PUBLIC $L_463191, $L_4631a2, $L_4631bb, $L_4631d6, $L_4631f1, $L_4631f5, $L_463220, $L_463222
PUBLIC $L_463229, $L_463232, $L_46323c, $L_46324c

_TEXT SEGMENT
FUN_45a7d0 PROC

            mov EAX,DWORD PTR [ESP+8]
            sub ESP,180
            xor EDX,EDX
            push EBX
            push ESI
            cmp EAX,EDX
            push EDI
            jne $L_45a7fb

            mov ECX,DWORD PTR [ESP+196]
            xor EAX,EAX
            cmp ECX,EDX
            setne AL
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a7fb::

            mov ESI,DWORD PTR [ESP+196]
            cmp ESI,EDX
            jne $L_45a813

            or EAX,4294967295
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a813::

            mov BL,BYTE PTR [ESI+8]
            mov EDI,DWORD PTR [ESI]
            mov ECX,DWORD PTR [EAX]
            cmp BL,DL
            jne $L_45a8a4

            cmp BYTE PTR [EAX+8],DL
            jne $L_45a848

            mov EAX,DWORD PTR [EAX+20]
            mov ESI,DWORD PTR [ESI+20]
            add ECX,EAX
            lea EDX,DWORD PTR [ESI+EDI*1]
            push ECX
            push EAX
            push EDX
            push ESI
            call _LexCompare

            add ESP,16
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a848::

            mov DWORD PTR [ESP+76],EAX
            mov DWORD PTR [ESP+16],EAX
            sub ESP,60
            mov DWORD PTR [ESP+72],ECX
            lea EAX,DWORD PTR [ESP+72]
            mov ECX,ESP
            push EAX
            mov DWORD PTR [ESP+136],EDX
            mov DWORD PTR [ESP+152],EDX
            mov DWORD PTR [ESP+92],EDX
            call @Rec60_Copy@12

            mov ESI,DWORD PTR [ESI+20]
            sub ESP,60
            lea EDX,DWORD PTR [ESP+192]
            mov ECX,ESP
            push EDX
            call @Rec60_Copy@12

            lea EAX,DWORD PTR [ESI+EDI*1]
            push EAX
            push ESI
            call _Rec60_Call4667d0

            add ESP,128
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a8a4::

            mov BL,BYTE PTR [EAX+8]
            mov DWORD PTR [ESP+12],EDX
            cmp BL,DL
            mov DWORD PTR [ESP+16],ESI
            mov DWORD PTR [ESP+28],EDX
            mov DWORD PTR [ESP+72],EDI
            mov DWORD PTR [ESP+76],ESI
            mov DWORD PTR [ESP+88],EDX
            jne $L_45a900

            mov EAX,DWORD PTR [EAX+20]
            lea EDX,DWORD PTR [ESP+72]
            add ECX,EAX
            push ECX
            push EAX
            sub ESP,60
            mov ECX,ESP
            push EDX
            call @Rec60_Copy@12

            sub ESP,60
            lea EAX,DWORD PTR [ESP+140]
            mov ECX,ESP
            push EAX
            call @Rec60_Copy@12

            call _Rec60_Call4668d0

            add ESP,128
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
$L_45a900::

            sub ESP,60
            mov DWORD PTR [ESP+192],EDX
            mov ESI,ESP
            mov DWORD PTR [ESP+196],EAX
            mov DWORD PTR [ESP+208],EDX
            sub ESP,60
            mov DWORD PTR [ESI+16],EDX
            mov DWORD PTR [ESI],ECX
            lea EDX,DWORD PTR [ESP+252]
            mov ECX,ESP
            push EDX
            mov DWORD PTR [ESI+4],EAX
            call @Rec60_Copy@12

            sub ESP,60
            lea EAX,DWORD PTR [ESP+252]
            mov ECX,ESP
            push EAX
            call @Rec60_Copy@12

            sub ESP,60
            lea EDX,DWORD PTR [ESP+252]
            mov ECX,ESP
            push EDX
            call @Rec60_Copy@12

            call _Rec60_Call466a00

            add ESP,240
            pop EDI
            pop ESI
            pop EBX
            add ESP,180
            ret 
FUN_45a7d0 ENDP
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
$L_46231e PROC

            mov ESI,DWORD PTR [EBP-20]
            mov EAX,DWORD PTR [ESI+8]
            push EAX
            call _free

            add ESP,4
            xor EDI,EDI
            mov DWORD PTR [ESI+8],EDI
            mov DWORD PTR [ESI+4],EDI
            push EDI
            push EDI
            call __CxxThrowException@8

            nop
            nop
            nop
            nop
$L_46231e ENDP
ALIGN 16
FUN_462850 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_63fe18
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,472
            push EBX
            push ESI
            mov ESI,DWORD PTR [EBP+8]
            push EDI
            mov ECX,46
            xor EAX,EAX
            lea EDI,DWORD PTR [EBP-216]
            xor EBX,EBX

            rep stosd DWORD PTR ES:[EDI]

            mov AL,BYTE PTR [ESI+12]
            mov DWORD PTR [EBP-16],ESP
            test AL,AL
            mov DWORD PTR [EBP-4],EBX
            je $L_4628a1

            lea EAX,DWORD PTR [EBP-216]
            push EAX
            push ESI
            call _Rope_AddLeafToForest

            jmp $L_4628c4
$L_4628a1::

            mov EDX,DWORD PTR [ESI+20]
            lea ECX,DWORD PTR [EBP-216]
            push ECX
            push EDX
            call _Tree_EraseRight

            mov ECX,DWORD PTR [ESI+24]
            add ESP,8
            lea EAX,DWORD PTR [EBP-216]
            push EAX
            push ECX
            call _Tree_EraseRight
$L_4628c4::

            add ESP,8
            xor ESI,ESI
$L_4628c9::

            cmp ESI,45
            mov DWORD PTR [EBP+8],ESI
            jg $L_46293e

            mov EAX,DWORD PTR [EBP+ESI*4-216]
            test EAX,EAX
            je $L_46293b

            mov EDI,EBX
            mov DWORD PTR [EBP-32],EDI
            push EBX
            push EAX
            mov BYTE PTR [EBP-4],1
            call FUN_462f30

            mov ESI,DWORD PTR [EBP+ESI*4-216]
            add ESP,8
            mov EBX,EAX
            lea EDX,DWORD PTR [ESI+4]
            push EDX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_46290d

            mov ECX,ESI
            call @Rope_FreeTree@8
$L_46290d::

            mov EAX,DWORD PTR [EBP+8]
            mov BYTE PTR [EBP-4],0
            test EDI,EDI
            mov DWORD PTR [EBP+EAX*4-216],0
            je $L_462938

            lea ECX,DWORD PTR [EDI+4]
            push ECX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_462938

            mov ECX,EDI
            call @Rope_FreeTree@8
$L_462938::

            mov ESI,DWORD PTR [EBP+8]
$L_46293b::

            inc ESI
            jmp $L_4628c9
$L_46293e::

            mov AL,BYTE PTR [EBX+13]
            or ECX,4294967295
            cmp AL,45
            mov DWORD PTR [EBP-4],ECX
            jbe $L_462a26

            xor EAX,EAX
            mov DWORD PTR [EBP-28],EAX
            mov DWORD PTR [EBP-24],EAX
            mov DWORD PTR [EBP-20],EAX
            mov EDI,OFFSET (_KingData+13620)
            mov DWORD PTR [EBP-4],3

            repne scasb 

            not ECX
            dec ECX
            mov ESI,ECX
            add ESI,OFFSET (_KingData+13620)
            mov EAX,ESI
            sub EAX,OFFSET (_KingData+13620)
            lea EDI,DWORD PTR [EAX+1]
            cmp EDI,-2
            ja $L_4629ab

            test EDI,EDI
            jne $L_462995

            xor EAX,EAX
            mov EDI,EAX
            mov DWORD PTR [EBP-28],EAX
            mov DWORD PTR [EBP-24],EAX
            mov DWORD PTR [EBP-20],EDI
            jmp $L_4629b6
$L_462995::

            push EDI
            call _Stl_Allocate

            add ESP,4
            add EDI,EAX
            mov DWORD PTR [EBP-28],EAX
            mov DWORD PTR [EBP-24],EAX
            mov DWORD PTR [EBP-20],EDI
            jmp $L_4629b6
$L_4629ab::

            lea ECX,DWORD PTR [EBP-28]
            call @Stl_LengthError@4
FUN_462850 ENDP
$L_4629b3 PROC

            mov EAX,DWORD PTR [EBP-28]
$L_4629b6::

            push EAX
            push ESI
            push OFFSET (_KingData+13620)
            call _Copy1

            add ESP,12
            lea ECX,DWORD PTR [EBP-28]
            mov DWORD PTR [EBP-24],EAX
            call @Tree_DestroyRoot@8

            lea ECX,DWORD PTR [EBP-484]
            mov DWORD PTR [EBP-4],4
            call ??0exception@@QAE@XZ

            mov EDX,DWORD PTR [EBP-28]
            push 256
            lea EAX,DWORD PTR [EBP-472]
            push EDX
            push EAX
            mov DWORD PTR [EBP-484],OFFSET (_KingRdata+2420)
            call _strncpy

            add ESP,12
            lea ECX,DWORD PTR [EBP-484]
            mov BYTE PTR [EBP-217],0
            mov DWORD PTR [EBP-484],OFFSET (_KingRdata+2412)
            push OFFSET (_KingRdata+70984)
            push ECX
            call __CxxThrowException@8
$L_4629b3 ENDP
$L_462a26 PROC

            mov ECX,DWORD PTR [EBP-12]
            pop EDI
            mov EAX,EBX
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_462a26 ENDP
$L_462a39 PROC

            lea EDI,DWORD PTR [EBP-216]
            mov EBX,46
$L_462a44::

            mov ESI,DWORD PTR [EDI]
            test ESI,ESI
            je $L_462a5f

            lea EDX,DWORD PTR [ESI+4]
            push EDX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_462a5f

            mov ECX,ESI
            call @Rope_FreeTree@8
$L_462a5f::

            add EDI,4
            dec EBX
            jne $L_462a44

            push 0
            push 0
            call __CxxThrowException@8

            nop
            nop
$L_462a39 ENDP
ALIGN 16
FUN_462b30 PROC

            jmp ??1exception@@UAE@XZ

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
FUN_462b30 ENDP
ALIGN 16
$L_462e2c PROC

            pop EDI
            pop ESI
            pop EBP
            pop EBX
$L_462e30::

            push ESI
            mov ESI,DWORD PTR [ECX]
            test ESI,ESI
            je $L_462f1b

            lea EAX,DWORD PTR [ESI+4]
            push EAX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_462f1b

            mov EAX,DWORD PTR [ESI+8]
            shl EAX,24
            sar EAX,24
            cmp EAX,3
            ja $L_462f1b

            jmp DWORD PTR [EAX*4+$L_462f20]
$L_462e66::

            mov ECX,DWORD PTR [ESI+16]
            mov EAX,DWORD PTR [ESI+20]
            cmp EAX,ECX
            je $L_462e83

            test ECX,ECX
            je $L_462e83

            mov EAX,DWORD PTR [ESI]
            inc EAX
            je $L_462e83

            push EAX
            push ECX
            call _Ai_PoolFree

            add ESP,8
$L_462e83::

            mov ECX,DWORD PTR [ESI]
            mov EDX,DWORD PTR [ESI+20]
            push ECX
            push EDX
            call _Str_FreeRounded

            push 24
            push ESI
            call _Ai_PoolFree

            add ESP,16
            pop ESI
            ret 
$L_462e9c::

            mov ECX,DWORD PTR [ESI+16]
            test ECX,ECX
            je $L_462eb2

            mov EAX,DWORD PTR [ESI]
            inc EAX
            je $L_462eb2

            push EAX
            push ECX
            call _Ai_PoolFree

            add ESP,8
$L_462eb2::

            mov ECX,DWORD PTR [ESI+20]
            call @Rope_Unref@8

            mov ECX,DWORD PTR [ESI+24]
            call @Rope_Unref@8

            push 28
            push ESI
            call _Ai_PoolFree

            add ESP,8
            pop ESI
            ret 
$L_462ecf::

            mov ECX,DWORD PTR [ESI+16]
            test ECX,ECX
            je $L_462ee5

            mov EAX,DWORD PTR [ESI]
            inc EAX
            je $L_462ee5

            push EAX
            push ECX
            call _Ai_PoolFree

            add ESP,8
$L_462ee5::

            mov AL,BYTE PTR [ESI+24]
            test AL,AL
            je $L_462ef9

            mov ECX,DWORD PTR [ESI+20]
            test ECX,ECX
            je $L_462ef9

            mov EAX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EAX]
$L_462ef9::

            push 28
            push ESI
            call _Ai_PoolFree

            add ESP,8
            pop ESI
            ret 
$L_462f06::

            add ESI,-4
            mov ECX,ESI
            call @RopeSubstr_Destroy@8

            push 40
            push ESI
            call _Ai_PoolFree

            add ESP,8
$L_462f1b::

            pop ESI
            ret 
$L_462e2c ENDP
          BYTE 08dH
          BYTE 049H
          BYTE 000H
$L_462f20           DWORD $L_462e66
          DWORD $L_462e9c
          DWORD $L_462f06
          DWORD $L_462ecf
ALIGN 16
FUN_462f30 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_63fe8f
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,8
            push EBX
            push ESI
            mov ESI,DWORD PTR [EBP+8]
            push EDI
            test ESI,ESI
            mov DWORD PTR [EBP-16],ESP
            jne $L_462f7c

            mov ESI,DWORD PTR [EBP+12]
            test ESI,ESI
            je $L_462f69

            lea EAX,DWORD PTR [ESI+4]
            push EAX
            call DWORD PTR __imp__InterlockedIncrement@4
$L_462f69::

            mov EAX,ESI
            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_462f7c::

            mov EDI,DWORD PTR [EBP+12]
            test EDI,EDI
            jne $L_462fa0

            lea ECX,DWORD PTR [ESI+4]
            push ECX
            call DWORD PTR __imp__InterlockedIncrement@4

            mov EAX,ESI
            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_462fa0::

            mov AL,BYTE PTR [EDI+8]
            test AL,AL
            jne $L_463091

            mov EAX,DWORD PTR [ESI+8]
            and EAX,255
            jne $L_462fe5

            mov EAX,DWORD PTR [EDI]
            mov EBX,DWORD PTR [ESI]
            mov EDX,EAX
            add EDX,EBX
            cmp EDX,23
            ja $L_463091

            push EAX
            mov EAX,DWORD PTR [EDI+20]
            push EAX
            push ESI
            call _SchString_Concat

            add ESP,12
            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_462fe5::

            cmp EAX,1
            jne $L_463091

            mov EAX,DWORD PTR [ESI+24]
            mov CL,BYTE PTR [EAX+8]
            test CL,CL
            jne $L_463091

            mov ECX,DWORD PTR [EDI]
            mov EDX,DWORD PTR [EAX]
            add EDX,ECX
            cmp EDX,23
            ja $L_463091

            mov ESI,DWORD PTR [ESI+20]
            push ECX
            mov ECX,DWORD PTR [EDI+20]
            mov DWORD PTR [EBP+12],ESI
            push ECX
            push EAX
            call _SchString_Concat

            add ESP,12
            lea EDX,DWORD PTR [ESI+4]
            mov EDI,EAX
            push EDX
            mov DWORD PTR [EBP+8],EDI
            call DWORD PTR __imp__InterlockedIncrement@4

            mov DWORD PTR [EBP-4],0
$L_463035::

            push EDI
            push ESI
            call _Rope_TreeConcat

            mov ECX,DWORD PTR [EBP-12]
            add ESP,8
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
FUN_462f30 ENDP
$L_463050 PROC

            mov ESI,DWORD PTR [EBP+12]
            test ESI,ESI
            je $L_46306c

            lea EAX,DWORD PTR [ESI+4]
            push EAX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_46306c

            mov ECX,ESI
            call @Rope_FreeTree@8
$L_46306c::

            mov ESI,DWORD PTR [EBP+8]
            test ESI,ESI
            je $L_463088

            lea ECX,DWORD PTR [ESI+4]
            push ECX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_463088

            mov ECX,ESI
            call @Rope_FreeTree@8
$L_463088::

            push 0
            push 0
            call __CxxThrowException@8
$L_463050 ENDP
$L_463091 PROC

            mov EBX,DWORD PTR __imp__InterlockedIncrement@4
            lea EDX,DWORD PTR [ESI+4]
            push EDX
            call EBX

            lea EAX,DWORD PTR [EDI+4]
            push EAX
            call EBX

            mov DWORD PTR [EBP-4],2
            jmp $L_463035
$L_463091 ENDP
$L_4630ac PROC

            mov ESI,DWORD PTR [EBP+8]
            test ESI,ESI
            je $L_46315b

            lea ECX,DWORD PTR [ESI+4]
            push ECX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_46315b

            mov EAX,DWORD PTR [ESI+8]
            shl EAX,24
            sar EAX,24
            cmp EAX,3
            ja $L_46315b

            jmp DWORD PTR [EAX*4+$L_46323c]
$L_4630e2::

            mov EDX,DWORD PTR [ESI+20]
            mov EAX,DWORD PTR [ESI+16]
            cmp EDX,EAX
            je $L_4630f3

            mov ECX,ESI
            call @Str_FreeData@8
$L_4630f3::

            mov EAX,DWORD PTR [ESI]
            mov ECX,DWORD PTR [ESI+20]
            push EAX
            push ECX
            call _Str_FreeRounded

            push 24
            push ESI
            call _Ai_PoolFree

            add ESP,16
            jmp $L_46315b
$L_46310c::

            mov ECX,ESI
            call @Str_FreeData@8

            mov ECX,DWORD PTR [ESI+20]
            call @Rope_Unref@8

            mov ECX,DWORD PTR [ESI+24]
            call @Rope_Unref@8

            push 28
            jmp $L_463152
$L_463127::

            mov ECX,ESI
            call @Str_FreeData@8

            mov AL,BYTE PTR [ESI+24]
            test AL,AL
            je $L_463142

            mov ECX,DWORD PTR [ESI+20]
            test ECX,ECX
            je $L_463142

            mov EDX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EDX]
$L_463142::

            push 28
            jmp $L_463152
$L_463146::

            add ESI,-4
            mov ECX,ESI
            call @RopeSubstr_Destroy@8

            push 40
$L_463152::

            push ESI
            call _Ai_PoolFree

            add ESP,8
$L_46315b::

            mov ESI,DWORD PTR [EBP+12]
            test ESI,ESI
            je $L_463232

            lea EAX,DWORD PTR [ESI+4]
            push EAX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_463232

            mov EAX,DWORD PTR [ESI+8]
            shl EAX,24
            sar EAX,24
            cmp EAX,3
            ja $L_463232

            jmp DWORD PTR [EAX*4+$L_46324c]
$L_463191::

            mov ECX,DWORD PTR [ESI+20]
            mov EAX,DWORD PTR [ESI+16]
            cmp ECX,EAX
            je $L_4631a2

            mov ECX,ESI
            call @Str_FreeData@8
$L_4631a2::

            mov EDX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [ESI+20]
            push EDX
            push EAX
            call _Str_FreeRounded

            push 24
            push ESI
            call _Ai_PoolFree

            add ESP,16
            jmp $L_463232
$L_4631bb::

            mov ECX,ESI
            call @Str_FreeData@8

            mov ECX,DWORD PTR [ESI+20]
            call @Rope_Unref@8

            mov ECX,DWORD PTR [ESI+24]
            call @Rope_Unref@8

            push 28
            jmp $L_463229
$L_4631d6::

            mov ECX,ESI
            call @Str_FreeData@8

            mov AL,BYTE PTR [ESI+24]
            test AL,AL
            je $L_4631f1

            mov ECX,DWORD PTR [ESI+20]
            test ECX,ECX
            je $L_4631f1

            mov EDX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EDX]
$L_4631f1::

            push 28
            jmp $L_463229
$L_4631f5::

            add ESI,-4
            mov DWORD PTR [EBP+12],ESI
            mov DWORD PTR [ESI],OFFSET (_KingRdata+1456)
            mov ECX,DWORD PTR [ESI+32]
            mov BYTE PTR [EBP-4],5
            call @Rope_Unref@8

            test ESI,ESI
            mov DWORD PTR [ESI],OFFSET (_KingRdata+1448)
            mov BYTE PTR [EBP-4],3
            je $L_463220

            lea ECX,DWORD PTR [ESI+4]
            jmp $L_463222
$L_463220::

            xor ECX,ECX
$L_463222::

            call @Buf459df0_Destruct@8

            push 40
$L_463229::

            push ESI
            call _Ai_PoolFree

            add ESP,8
$L_463232::

            push 0
            push 0
            call __CxxThrowException@8

            nop
$L_46323c           DWORD $L_4630e2
          DWORD $L_46310c
          DWORD $L_463146
          DWORD $L_463127
$L_46324c           DWORD $L_463191
          DWORD $L_4631bb
          DWORD $L_4631f5
          DWORD $L_4631d6

            nop
            nop
            nop
            nop
$L_4630ac ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
_TEXT ENDS
END
