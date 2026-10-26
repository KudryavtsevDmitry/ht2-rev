; asm_unit_458bc0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_63f8a0:NEAR
EXTERN $L_63f8b0:NEAR
EXTERN $L_63f8d8:NEAR
EXTERN $L_63fde0:NEAR
EXTERN $L_63fe18:NEAR
EXTERN $L_63fe56:NEAR
EXTERN $L_63fe8f:NEAR
EXTERN $L_63fee1:NEAR
EXTERN $L_63ff01:NEAR
EXTERN $L_63ff21:NEAR
EXTERN $L_63ff71:NEAR
EXTERN $L_640031:NEAR
EXTERN ??0exception@@QAE@XZ:NEAR
EXTERN ??1exception@@UAE@XZ:NEAR
EXTERN @Buf459df0_Destruct@8:PROC
EXTERN @Int_Copy@12:PROC
EXTERN @Rec60_Copy@12:PROC
EXTERN @RopeConcat_Destroy@8:PROC
EXTERN @RopeSubstr_Destroy@8:PROC
EXTERN @Rope_FreeTree@8:PROC
EXTERN @Rope_Unref@8:PROC
EXTERN @Shell_Nop@8:PROC
EXTERN @Stl_Lock@8:PROC
EXTERN @Str_FreeBoth@8:PROC
EXTERN @Str_FreeData@8:PROC
EXTERN @Tree_DestroyRoot@8:PROC
EXTERN @Vec1_Free@8:PROC
EXTERN _Ai_PoolFree:PROC
EXTERN _Copy1:PROC
EXTERN _Far_AdjustHeap:PROC
EXTERN _Heap504_Adjust:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _LexCompare:PROC
EXTERN _Rec60_Call4667d0:PROC
EXTERN _Rec60_Call4668d0:PROC
EXTERN _Rec60_Call466a00:PROC
EXTERN _Rope_AddLeafToForest:PROC
EXTERN _SchString_Concat:PROC
EXTERN _SchString_FromChars:PROC
EXTERN _SchValue_Same:PROC
EXTERN _Stl_Allocate:PROC
EXTERN _Stl_Free4s@8:PROC
EXTERN _Stl_FreeBytes@8:PROC
EXTERN _Str_FreeRounded:PROC
EXTERN _Tree_EraseRight:PROC
EXTERN _UninitFill4s:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN __imp__InterlockedDecrement@4:PROC
EXTERN __imp__InterlockedIncrement@4:PROC
EXTERN _free:PROC
EXTERN _malloc:PROC
EXTERN _memmove:NEAR
EXTERN _strncpy:NEAR

PUBLIC $L_45a2e7, $L_45a2fa, $L_45a30d, $L_45a340, $L_45a38e, $L_45a395, $L_45a3c5, $L_45a3ef
PUBLIC $L_45a3fa, $L_45a405, $L_45a410, $L_45a41c, $L_45a425, $L_45a42e, $L_45a495, $L_45a4e0
PUBLIC $L_45a4e9, $L_45a530, $L_45a57f, $L_45a58f, $L_45a598, $L_45a5a4, $L_45a5b4, $L_45a5c4
PUBLIC $L_45a625, $L_45a65c, $L_45a65e, $L_45a680, $L_45a6b9, $L_45a6ca, $L_45a6f7, $L_45a72c
PUBLIC $L_45a751, $L_45a758, $L_45a76a, $L_45a7c0, $L_45a7fb, $L_45a813, $L_45a848, $L_45a8a4
PUBLIC $L_45a900, $L_45b1bb, $L_45b1d4, $L_45b20c
PUBLIC $L_462395, $L_4623c0, $L_4623d9, $L_462810, $L_46283a, $L_4628a1
PUBLIC $L_4628c4, $L_4628c9, $L_46290d, $L_462938, $L_46293b, $L_46293e, $L_462995, $L_4629ab
PUBLIC $L_4629b6, $L_462a44, $L_462a5f, $L_462d8f, $L_462da8, $L_462db5, $L_462e30, $L_462e66
PUBLIC $L_462e83, $L_462e9c, $L_462eb2, $L_462ecf, $L_462ee5, $L_462ef9, $L_462f06, $L_462f1b
PUBLIC $L_462f20, $L_462f69, $L_462f7c, $L_462fa0, $L_462fe5, $L_463035, $L_46306c, $L_463088
PUBLIC $L_4630e2, $L_4630f3, $L_46310c, $L_463127, $L_463142, $L_463146, $L_463152, $L_46315b
PUBLIC $L_463191, $L_4631a2, $L_4631bb, $L_4631d6, $L_4631f1, $L_4631f5, $L_463220, $L_463222
PUBLIC $L_463229, $L_463232, $L_46323c, $L_46324c, $L_4644af, $L_4644d9, $L_46622f, $L_466271
PUBLIC $L_4662a5, $L_4662c4, $L_4662e4, $L_46632f, $L_46636b, $L_46639e, $L_4663ba, $L_4663da
PUBLIC $L_46646c, $L_4664ab, $L_4664d7, $L_4664f4, $L_46651d, $L_466d7f, $L_466dc1
PUBLIC $L_466df5, $L_466e14, $L_466e34, $L_46713e, $L_4672ae, $L_46734f, $L_467385
PUBLIC $L_4673b6, $L_4673cf, $L_4673f5, $L_467728, $L_469bd7, $L_469c0c

_TEXT SEGMENT
FUN_45a2b0 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_63f8a0
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ECX
            mov EAX,DWORD PTR [EBP+16]
            push EBX
            push ESI
            mov ESI,DWORD PTR [EBP+8]
            push EDI
            test EAX,EAX
            mov DWORD PTR [EBP-16],ESP
            jne $L_45a2fa

            test ESI,ESI
            je $L_45a2e7

            lea EAX,DWORD PTR [ESI+4]
            push EAX
            call DWORD PTR __imp__InterlockedIncrement@4
$L_45a2e7::

            mov EAX,ESI
            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_45a2fa::

            test ESI,ESI
            jne $L_45a30d

            mov ECX,DWORD PTR [EBP+12]
            push EAX
            push ECX
            call _SchString_FromChars

            jmp $L_45a395
$L_45a30d::

            mov ECX,DWORD PTR [ESI+8]
            and ECX,255
            jne $L_45a340

            mov EDX,DWORD PTR [ESI]
            add EDX,EAX
            cmp EDX,23
            ja $L_45a340

            push EAX
            mov EAX,DWORD PTR [EBP+12]
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
$L_45a340::

            cmp ECX,1
            jne $L_45a42e

            mov ECX,DWORD PTR [ESI+24]
            mov DL,BYTE PTR [ECX+8]
            test DL,DL
            jne $L_45a42e

            mov EDX,DWORD PTR [ECX]
            add EDX,EAX
            cmp EDX,23
            ja $L_45a42e

            mov ESI,DWORD PTR [ESI+20]
            push EAX
            mov EAX,DWORD PTR [EBP+12]
            mov DWORD PTR [EBP+8],ESI
            push EAX
            push ECX
            call _SchString_Concat

            add ESP,12
            lea ECX,DWORD PTR [ESI+4]
            mov EDI,EAX
            push ECX
            mov DWORD PTR [EBP+12],EDI
            call DWORD PTR __imp__InterlockedIncrement@4

            mov DWORD PTR [EBP-4],0
$L_45a38e::

            push EDI
            push ESI
            call FUN_45a5e0
$L_45a395::

            mov ECX,DWORD PTR [EBP-12]
            add ESP,8
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
FUN_45a2b0 ENDP
$L_45a3a9 PROC

            mov ESI,DWORD PTR [EBP+8]
            test ESI,ESI
            je $L_45a3c5

            lea EDX,DWORD PTR [ESI+4]
            push EDX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_45a3c5

            mov ECX,ESI
            call @Rope_FreeTree@8
$L_45a3c5::

            mov ESI,DWORD PTR [EBP+12]
            test ESI,ESI
            je $L_45a425

            lea EAX,DWORD PTR [ESI+4]
            push EAX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_45a425

            mov EAX,DWORD PTR [ESI+8]
            shl EAX,24
            sar EAX,24
            cmp EAX,3
            ja $L_45a425

            jmp DWORD PTR [EAX*4+$L_45a5a4]
$L_45a3ef::

            mov ECX,ESI
            call @Str_FreeBoth@8

            push 24
            jmp $L_45a41c
$L_45a3fa::

            mov ECX,ESI
            call @RopeConcat_Destroy@8

            push 28
            jmp $L_45a41c
$L_45a405::

            mov ECX,ESI
            call @Buf459df0_Destruct@8

            push 28
            jmp $L_45a41c
$L_45a410::

            add ESI,-4
            mov ECX,ESI
            call @RopeSubstr_Destroy@8

            push 40
$L_45a41c::

            push ESI
            call _Ai_PoolFree

            add ESP,8
$L_45a425::

            push 0
            push 0
            call __CxxThrowException@8
$L_45a42e::

            mov ECX,DWORD PTR [EBP+12]
            push EAX
            push ECX
            call _SchString_FromChars

            add ESP,8
            lea EDX,DWORD PTR [ESI+4]
            mov EDI,EAX
            mov DWORD PTR [EBP-4],2
            push EDX
            mov DWORD PTR [EBP+12],EDI
            call DWORD PTR __imp__InterlockedIncrement@4

            jmp $L_45a38e
$L_45a3a9 ENDP
$L_45a456 PROC

            mov ESI,DWORD PTR [EBP+8]
            test ESI,ESI
            je $L_45a4e9

            lea EAX,DWORD PTR [ESI+4]
            push EAX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_45a4e9

            mov EAX,DWORD PTR [ESI+8]
            shl EAX,24
            sar EAX,24
            cmp EAX,3
            ja $L_45a4e9

            jmp DWORD PTR [EAX*4+$L_45a5b4]
$L_45a456 ENDP
$L_45a484 PROC

            mov ECX,DWORD PTR [ESI+20]
            mov EAX,DWORD PTR [ESI+16]
            cmp ECX,EAX
            je $L_45a495

            mov ECX,ESI
            call @Str_FreeData@8
$L_45a495::

            mov EDX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [ESI+20]
            push EDX
            push EAX
            call _Str_FreeRounded

            push 24
            push ESI
            call _Ai_PoolFree

            add ESP,16
            jmp $L_45a4e9
$L_45a484 ENDP
$L_45a4ae PROC

            mov ECX,ESI
            call @Str_FreeData@8

            mov ECX,DWORD PTR [ESI+20]
            call @Rope_Unref@8

            mov ECX,DWORD PTR [ESI+24]
            call @Rope_Unref@8

            push 28
            jmp $L_45a4e0
$L_45a4ae ENDP
$L_45a4c9 PROC

            mov ECX,ESI
            call @Buf459df0_Destruct@8

            push 28
            jmp $L_45a4e0
$L_45a4c9 ENDP
$L_45a4d4 PROC

            add ESI,-4
            mov ECX,ESI
            call @RopeSubstr_Destroy@8

            push 40
$L_45a4e0::

            push ESI
            call _Ai_PoolFree

            add ESP,8
$L_45a4e9::

            mov ESI,DWORD PTR [EBP+12]
            test ESI,ESI
            je $L_45a598

            lea ECX,DWORD PTR [ESI+4]
            push ECX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_45a598

            mov EAX,DWORD PTR [ESI+8]
            shl EAX,24
            sar EAX,24
            cmp EAX,3
            ja $L_45a598

            jmp DWORD PTR [EAX*4+$L_45a5c4]
$L_45a4d4 ENDP
$L_45a51f PROC

            mov EDX,DWORD PTR [ESI+20]
            mov EAX,DWORD PTR [ESI+16]
            cmp EDX,EAX
            je $L_45a530

            mov ECX,ESI
            call @Str_FreeData@8
$L_45a530::

            mov EAX,DWORD PTR [ESI]
            mov ECX,DWORD PTR [ESI+20]
            push EAX
            push ECX
            call _Str_FreeRounded

            push 24
            push ESI
            call _Ai_PoolFree

            add ESP,16
            jmp $L_45a598
$L_45a51f ENDP
$L_45a549 PROC

            mov ECX,ESI
            call @Str_FreeData@8

            mov ECX,DWORD PTR [ESI+20]
            call @Rope_Unref@8

            mov ECX,DWORD PTR [ESI+24]
            call @Rope_Unref@8

            push 28
            jmp $L_45a58f
$L_45a549 ENDP
$L_45a564 PROC

            mov ECX,ESI
            call @Str_FreeData@8

            mov AL,BYTE PTR [ESI+24]
            test AL,AL
            je $L_45a57f

            mov ECX,DWORD PTR [ESI+20]
            test ECX,ECX
            je $L_45a57f

            mov EDX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EDX]
$L_45a57f::

            push 28
            jmp $L_45a58f
$L_45a564 ENDP
$L_45a583 PROC

            add ESI,-4
            mov ECX,ESI
            call @RopeSubstr_Destroy@8

            push 40
$L_45a58f::

            push ESI
            call _Ai_PoolFree

            add ESP,8
$L_45a598::

            push 0
            push 0
            call __CxxThrowException@8

            lea ECX,DWORD PTR [ECX]
$L_45a583 ENDP
$L_45a5a4           DWORD $L_45a3ef
          DWORD $L_45a3fa
          DWORD $L_45a410
          DWORD $L_45a405
$L_45a5b4           DWORD $L_45a484
          DWORD $L_45a4ae
          DWORD $L_45a4d4
          DWORD $L_45a4c9
$L_45a5c4           DWORD $L_45a51f
          DWORD $L_45a549
          DWORD $L_45a583
          DWORD $L_45a564
ALIGN 16
FUN_45a5e0 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_63f8b0
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ECX
            push EBX
            push ESI
            push EDI
            mov DWORD PTR [EBP-16],ESP
            push 28
            call _Stl_Allocate

            xor EBX,EBX
            add ESP,4
            cmp EAX,EBX
            je $L_45a65c

            mov ESI,DWORD PTR [EBP+8]
            mov EDI,DWORD PTR [EBP+12]
            mov BL,BYTE PTR [ESI+13]
            lea ECX,DWORD PTR [ESI+13]
            cmp BL,BYTE PTR [EDI+13]
            lea EDX,DWORD PTR [EDI+13]
            jb $L_45a625

            mov EDX,ECX
$L_45a625::

            mov ECX,DWORD PTR [ESI]
            mov EBX,DWORD PTR [EDI]
            add ECX,EBX
            xor EBX,EBX
            mov BL,BYTE PTR [EDX]
            mov DWORD PTR [EAX],ECX
            mov EDX,EBX
            inc EDX
            mov DWORD PTR [EAX+4],1
            mov ECX,DWORD PTR [EAX+8]
            mov DWORD PTR [EAX+20],ESI
            and CL,1
            mov BYTE PTR [EAX+13],DL
            or ECX,1
            xor EBX,EBX
            mov DWORD PTR [EAX+8],ECX
            mov BYTE PTR [EAX+12],BL
            mov DWORD PTR [EAX+16],EBX
            mov DWORD PTR [EAX+24],EDI
            mov ESI,EAX
            jmp $L_45a65e
$L_45a65c::

            xor ESI,ESI
$L_45a65e::

            xor EAX,EAX
            mov DWORD PTR [EBP+8],ESI
            mov AL,BYTE PTR [ESI+13]
            cmp EAX,20
            jbe $L_45a7aa

            cmp DWORD PTR [ESI],1000
            jb $L_45a680

            cmp EAX,45
            jbe $L_45a7aa
$L_45a680::

            push ESI
            mov DWORD PTR [EBP-4],EBX
            call FUN_462850

            add ESP,4
            lea EDX,DWORD PTR [ESI+4]
            mov EDI,EAX
            push EDX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_45a76a

            mov EAX,DWORD PTR [ESI+8]
            shl EAX,24
            sar EAX,24
            cmp EAX,3
            ja $L_45a76a

            jmp DWORD PTR [EAX*4+$L_45a7c0]
$L_45a6b9::

            mov EAX,DWORD PTR [ESI+20]
            mov ECX,DWORD PTR [ESI+16]
            cmp EAX,ECX
            je $L_45a6ca

            mov ECX,ESI
            call @Str_FreeData@8
$L_45a6ca::

            mov ECX,DWORD PTR [ESI]
            mov EDX,DWORD PTR [ESI+20]
            push ECX
            push EDX
            call _Str_FreeRounded

            add ESP,8
            push 24
            push ESI
            call _Ai_PoolFree

            add ESP,8
            mov EAX,EDI
            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_45a6f7::

            mov ECX,ESI
            call @Str_FreeData@8

            mov ECX,DWORD PTR [ESI+20]
            call @Rope_Unref@8

            mov ECX,DWORD PTR [ESI+24]
            call @Rope_Unref@8

            push 28
            push ESI
            call _Ai_PoolFree

            add ESP,8
            mov EAX,EDI
            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_45a72c::

            mov ECX,ESI
            call @Buf459df0_Destruct@8

            push 28
            push ESI
            call _Ai_PoolFree

            add ESP,8
            mov EAX,EDI
            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_45a751::

            cmp ESI,EBX
            je $L_45a758

            lea EBX,DWORD PTR [ESI-4]
$L_45a758::

            mov ECX,EBX
            call @RopeSubstr_Destroy@8

            push 40
            push EBX
            call _Ai_PoolFree

            add ESP,8
$L_45a76a::

            mov EAX,EDI
            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
FUN_45a5e0 ENDP
$L_45a77d PROC

            mov ECX,OFFSET (_KingBss+10724)
            call @Stl_Lock@8

            mov ECX,DWORD PTR [(_KingBss+10672)]
            mov EAX,DWORD PTR [EBP+8]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [(_KingBss+10672)],EAX
            mov DWORD PTR [(_KingBss+10724)],0
            push 0
            push 0
            call __CxxThrowException@8
$L_45a77d ENDP
$L_45a7aa PROC

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,ESI
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_45a7aa ENDP
          BYTE 08dH
          BYTE 049H
          BYTE 000H
$L_45a7c0           DWORD $L_45a6b9
          DWORD $L_45a6f7
          DWORD $L_45a751
          DWORD $L_45a72c
ALIGN 16
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
FUN_45b190 PROC

            mov EAX,DWORD PTR FS:[0]
            push -1
            push OFFSET $L_63f8d8
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ESI
            push EDI
            mov ESI,DWORD PTR [ESP+24]
            mov EDI,DWORD PTR [ESP+28]
            cmp ESI,EDI
            mov DWORD PTR [ESP+16],0
            je $L_45b1d4
$L_45b1bb::

            lea EAX,DWORD PTR [ESP+32]
            push EAX
            push ESI
            call _SchValue_Same

            add ESP,8
            test AL,AL
            jne $L_45b1d4

            add ESI,104
            cmp ESI,EDI
            jne $L_45b1bb
$L_45b1d4::

            mov EAX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+16],4294967295
            test EAX,65536
            jne $L_45b20c

            mov EAX,DWORD PTR [ESP+32]
            test EAX,EAX
            je $L_45b20c

            mov ECX,DWORD PTR [EAX+4]
            dec ECX
            mov DWORD PTR [EAX+4],ECX
            mov ECX,DWORD PTR [ESP+32]
            mov EAX,DWORD PTR [ECX+4]
            test EAX,EAX
            jne $L_45b20c

            test ECX,ECX
            je $L_45b20c

            mov EDX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EDX+12]
$L_45b20c::

            mov ECX,DWORD PTR [ESP+8]
            mov EAX,ESI
            pop EDI
            mov DWORD PTR FS:[0],ECX
            pop ESI
            add ESP,12
            ret 
FUN_45b190 ENDP
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
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
FUN_462340 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_63fde0
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,8
            mov EAX,DWORD PTR [EBP+12]
            push EBX
            mov EBX,DWORD PTR [EBP+8]
            push ESI
            push EDI
            xor EDI,EDI
            sub EAX,EBX
            mov EDX,EDI
            sar EAX,2
            mov ESI,ECX
            mov EDX,EAX
            mov EAX,EDX
            mov DWORD PTR [EBP-16],ESP
            mov DWORD PTR [ESI+4],EDI
            mov DWORD PTR [ESI+4],EDX
            cmp EAX,536870911
            mov DWORD PTR [EBP-20],ESI
            mov DWORD PTR [EBP-4],EDI
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [ESI+8],EDI
            jle $L_462395

            mov DWORD PTR [ESI+4],536870911
$L_462395::

            mov EAX,DWORD PTR [ESI+4]
            cmp EAX,EDI
            jle $L_4623c0

            lea ECX,DWORD PTR [EAX*4]
            push ECX
            call _malloc

            add ESP,4
            cmp EAX,EDI
            mov DWORD PTR [ESI+8],EAX
            jne $L_4623c0

            mov EAX,DWORD PTR [ESI+4]
            cdq 
            sub EAX,EDX
            sar EAX,1
            mov DWORD PTR [ESI+4],EAX
            jmp $L_462395
$L_4623c0::

            mov EAX,DWORD PTR [ESI+4]
            cmp EAX,EDI
            jle $L_4623d9

            mov EDX,DWORD PTR [EBP+12]
            mov ECX,DWORD PTR [ESI+8]
            push EDX
            push EBX
            push EAX
            push ECX
            call _UninitFill4s

            add ESP,16
$L_4623d9::

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,ESI
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 8
FUN_462340 ENDP
$L_4623ee PROC

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
$L_4623ee ENDP
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
$L_4627ec PROC

            mov EAX,DWORD PTR [EBP+16]
            mov ECX,DWORD PTR [EBP-28]
            lea EAX,DWORD PTR [ECX+EAX*1+8]
            and EAX,4294967288
            je $L_46283a

            cmp EAX,128
            jbe $L_462810

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_46283a
$L_462810::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP-20]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_46283a::

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
            nop
$L_4627ec ENDP
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
            call FUN_462d20
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
$L_462d00 PROC

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
$L_462d00 ENDP
ALIGN 16
ALIGN 16
FUN_462d20 PROC

            push -1
            push OFFSET $L_63fe56
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,280
            push EBX
            push EBP
            xor EBX,EBX
            push ESI
            push EDI
            mov DWORD PTR [ESP+16],EBX
            mov DWORD PTR [ESP+20],EBX
            mov DWORD PTR [ESP+24],EBX
            mov EDI,OFFSET (_KingData+13636)
            or ECX,4294967295
            xor EAX,EAX
            mov DWORD PTR [ESP+304],EBX

            repne scasb 

            not ECX
            dec ECX
            mov EDI,ECX
            add EDI,OFFSET (_KingData+13636)
            mov EBP,EDI
            sub EBP,OFFSET (_KingData+13636)
            lea ESI,DWORD PTR [EBP+1]
            cmp ESI,-2
            ja $L_462da8

            cmp ESI,EBX
            jne $L_462d8f

            add ESI,EAX
            mov DWORD PTR [ESP+16],EAX
            mov DWORD PTR [ESP+20],EAX
            mov DWORD PTR [ESP+24],ESI
            jmp $L_462db5
$L_462d8f::

            push ESI
            call _Stl_Allocate

            add ESP,4
            add ESI,EAX
            mov DWORD PTR [ESP+16],EAX
            mov DWORD PTR [ESP+20],EAX
            mov DWORD PTR [ESP+24],ESI
            jmp $L_462db5
$L_462da8::

            lea ECX,DWORD PTR [ESP+16]
            call FUN_462d20
FUN_462d20 ENDP
$L_462db1 PROC

            mov EAX,DWORD PTR [ESP+16]
$L_462db5::

            push EBP
            push OFFSET (_KingData+13636)
            push EAX
            mov ESI,EAX
            call _memmove

            sub ESI,OFFSET (_KingData+13636)
            add ESP,12
            add ESI,EDI
            lea ECX,DWORD PTR [ESP+16]
            mov DWORD PTR [ESP+20],ESI
            call @Tree_DestroyRoot@8

            lea ECX,DWORD PTR [ESP+28]
            mov DWORD PTR [ESP+304],1
            call ??0exception@@QAE@XZ

            mov EAX,DWORD PTR [ESP+16]
            push 256
            lea ECX,DWORD PTR [ESP+44]
            push EAX
            push ECX
            mov DWORD PTR [ESP+40],OFFSET (_KingRdata+2420)
            call _strncpy

            add ESP,12
            lea EDX,DWORD PTR [ESP+28]
            mov BYTE PTR [ESP+295],BL
            mov DWORD PTR [ESP+28],OFFSET (_KingRdata+2412)
            push OFFSET (_KingRdata+70984)
            push EDX
            call __CxxThrowException@8
$L_462db1 ENDP
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
            call FUN_45a5e0

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
$L_464490 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_4644d9

            shl EAX,2
            cmp EAX,128
            jbe $L_4644af

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_4644d9
$L_4644af::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP-20]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_4644d9::

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
            nop
            nop
$L_464490 ENDP
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
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
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
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
FUN_466200 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_63fee1
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,12
            push EBX
            mov EBX,DWORD PTR [EBP+8]
            push ESI
            mov ESI,DWORD PTR [EBP+16]
            push EDI
            xor EDI,EDI
            mov DWORD PTR [EBP-16],ESP
            mov DWORD PTR [EBP-20],ESI
            mov DWORD PTR [EBP-4],EDI
$L_46622f::

            cmp EBX,DWORD PTR [EBP+12]
            je $L_4662ed

            mov DWORD PTR [EBP-24],ESI
            cmp ESI,EDI
            mov BYTE PTR [EBP-4],1
            je $L_4662a5

            mov EAX,DWORD PTR [EBX]
            mov DWORD PTR [ESI],EAX
            mov ECX,DWORD PTR [EBX+4]
            mov DWORD PTR [ESI+4],ECX
            mov EAX,DWORD PTR [EBX+12]
            mov ECX,DWORD PTR [EBX+8]
            mov DWORD PTR [ESI+8],EDI
            sub EAX,ECX
            mov DWORD PTR [ESI+12],EDI
            mov DWORD PTR [EBP+8],EAX
            mov DWORD PTR [ESI+16],EDI
            je $L_466271

            push EAX
            call _Stl_Allocate

            mov EDI,EAX
            mov EAX,DWORD PTR [EBP+8]
            add ESP,4
$L_466271::

            lea EDX,DWORD PTR [EDI+EAX*1]
            mov DWORD PTR [ESI+8],EDI
            mov DWORD PTR [ESI+12],EDI
            mov DWORD PTR [ESI+16],EDX
            mov ECX,DWORD PTR [EBX+12]
            mov EAX,DWORD PTR [EBX+8]
            mov DWORD PTR [EBP-20],ECX
            sub ECX,EAX
            push ECX
            push EAX
            push EDI
            mov DWORD PTR [EBP+8],EAX
            call _memmove

            mov ECX,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EBP-20]
            sub EDI,ECX
            add ESP,12
            add EDI,EAX
            mov DWORD PTR [ESI+12],EDI
            xor EDI,EDI
$L_4662a5::

            add EBX,24
            add ESI,24
            mov BYTE PTR [EBP-4],0
            mov DWORD PTR [EBP-20],ESI
            jmp $L_46622f
FUN_466200 ENDP
$L_4662b7 PROC

            mov EAX,DWORD PTR [EBP+16]
            mov EDI,DWORD PTR [EBP-20]
            cmp EAX,EDI
            je $L_4662e4

            lea ESI,DWORD PTR [EAX+8]
$L_4662c4::

            mov EAX,DWORD PTR [ESI+4]
            mov ECX,DWORD PTR [ESI]
            push EAX
            push ECX
            call @Shell_Nop@8

            add ESP,8
            mov ECX,ESI
            call @Vec1_Free@8

            add ESI,24
            lea EDX,DWORD PTR [ESI-8]
            cmp EDX,EDI
            jne $L_4662c4
$L_4662e4::

            push 0
            push 0
            call __CxxThrowException@8
$L_4662b7 ENDP
$L_4662ed PROC

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,ESI
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_4662ed ENDP
ALIGN 16
FUN_466300 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_63ff01
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,12
            push EBX
            mov EBX,DWORD PTR [EBP+8]
            push ESI
            mov ESI,DWORD PTR [EBP+16]
            push EDI
            xor EDI,EDI
            mov DWORD PTR [EBP-16],ESP
            mov DWORD PTR [EBP-20],ESI
            mov DWORD PTR [EBP-4],EDI
$L_46632f::

            cmp EBX,DWORD PTR [EBP+12]
            je $L_4663e3

            mov DWORD PTR [EBP-24],ESI
            cmp ESI,EDI
            mov BYTE PTR [EBP-4],1
            je $L_46639e

            mov EAX,DWORD PTR [EBX]
            mov DWORD PTR [ESI],EAX
            mov EAX,DWORD PTR [EBX+8]
            mov ECX,DWORD PTR [EBX+4]
            mov DWORD PTR [ESI+4],EDI
            sub EAX,ECX
            mov DWORD PTR [ESI+8],EDI
            mov DWORD PTR [EBP+8],EAX
            mov DWORD PTR [ESI+12],EDI
            je $L_46636b

            push EAX
            call _Stl_Allocate

            mov EDI,EAX
            mov EAX,DWORD PTR [EBP+8]
            add ESP,4
$L_46636b::

            add EAX,EDI
            mov DWORD PTR [ESI+4],EDI
            mov DWORD PTR [ESI+8],EDI
            mov DWORD PTR [ESI+12],EAX
            mov ECX,DWORD PTR [EBX+8]
            mov EAX,DWORD PTR [EBX+4]
            mov DWORD PTR [EBP-20],ECX
            sub ECX,EAX
            push ECX
            push EAX
            push EDI
            mov DWORD PTR [EBP+8],EAX
            call _memmove

            mov ECX,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EBP-20]
            sub EDI,ECX
            add ESP,12
            add EDI,EAX
            mov DWORD PTR [ESI+8],EDI
            xor EDI,EDI
$L_46639e::

            add EBX,16
            add ESI,16
            mov BYTE PTR [EBP-4],0
            mov DWORD PTR [EBP-20],ESI
            jmp $L_46632f
FUN_466300 ENDP
$L_4663ad PROC

            mov EAX,DWORD PTR [EBP+16]
            mov EDI,DWORD PTR [EBP-20]
            cmp EAX,EDI
            je $L_4663da

            lea ESI,DWORD PTR [EAX+4]
$L_4663ba::

            mov ECX,DWORD PTR [ESI+4]
            mov EDX,DWORD PTR [ESI]
            push ECX
            push EDX
            call @Shell_Nop@8

            add ESP,8
            mov ECX,ESI
            call @Vec1_Free@8

            add ESI,16
            lea EAX,DWORD PTR [ESI-4]
            cmp EAX,EDI
            jne $L_4663ba
$L_4663da::

            push 0
            push 0
            call __CxxThrowException@8
$L_4663ad ENDP
$L_4663e3 PROC

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,ESI
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_4663e3 ENDP
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
FUN_466440 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_63ff21
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,16
            push EBX
            push ESI
            mov ESI,DWORD PTR [EBP+8]
            push EDI
            xor EDI,EDI
            mov DWORD PTR [EBP-16],ESP
            mov DWORD PTR [EBP-20],ESI
            mov DWORD PTR [EBP-4],EDI
$L_46646c::

            cmp DWORD PTR [EBP+12],EDI
            jbe $L_466526

            mov DWORD PTR [EBP-28],ESI
            cmp ESI,EDI
            mov BYTE PTR [EBP-4],1
            je $L_4664d7

            mov EBX,DWORD PTR [EBP+16]
            mov EAX,DWORD PTR [EBX+4]
            mov EDX,DWORD PTR [EBX]
            sub EAX,EDX
            mov DWORD PTR [ESI],EDI
            sar EAX,2
            mov DWORD PTR [EBP-24],EAX
            mov DWORD PTR [ESI+4],EDI
            mov DWORD PTR [ESI+8],EDI
            je $L_4664ab

            shl EAX,2
            push EAX
            call _Stl_Allocate

            mov EDI,EAX
            mov EAX,DWORD PTR [EBP-24]
            add ESP,4
$L_4664ab::

            lea ECX,DWORD PTR [EDI+EAX*4]
            mov DWORD PTR [ESI],EDI
            mov DWORD PTR [ESI+4],EDI
            mov DWORD PTR [ESI+8],ECX
            mov EAX,DWORD PTR [EBX+4]
            mov ECX,DWORD PTR [EBX]
            sub EAX,ECX
            sar EAX,2
            shl EAX,2
            mov EBX,EAX
            push EBX
            push ECX
            push EDI
            call _memmove

            add EBX,EDI
            add ESP,12
            mov DWORD PTR [ESI+4],EBX
            xor EDI,EDI
$L_4664d7::

            mov ECX,DWORD PTR [EBP+12]
            mov BYTE PTR [EBP-4],0
            dec ECX
            add ESI,12
            mov DWORD PTR [EBP+12],ECX
            mov DWORD PTR [EBP-20],ESI
            jmp $L_46646c
FUN_466440 ENDP
$L_4664ea PROC

            mov ESI,DWORD PTR [EBP+8]
            mov EDI,DWORD PTR [EBP-20]
            cmp ESI,EDI
            je $L_46651d
$L_4664f4::

            mov EDX,DWORD PTR [ESI+4]
            mov EAX,DWORD PTR [ESI]
            push EDX
            push EAX
            call @Shell_Nop@8

            mov EAX,DWORD PTR [ESI]
            mov ECX,DWORD PTR [ESI+8]
            sub ECX,EAX
            add ESP,8
            sar ECX,2
            push ECX
            push EAX
            mov ECX,ESI
            call _Stl_Free4s@8

            add ESI,12
            cmp ESI,EDI
            jne $L_4664f4
$L_46651d::

            push 0
            push 0
            call __CxxThrowException@8
$L_4664ea ENDP
$L_466526 PROC

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,ESI
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_466526 ENDP
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
FUN_466d50 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_63ff71
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,12
            push EBX
            mov EBX,DWORD PTR [EBP+8]
            push ESI
            mov ESI,DWORD PTR [EBP+16]
            push EDI
            xor EDI,EDI
            mov DWORD PTR [EBP-16],ESP
            mov DWORD PTR [EBP-20],ESI
            mov DWORD PTR [EBP-4],EDI
$L_466d7f::

            cmp EBX,DWORD PTR [EBP+12]
            je $L_466e3d

            mov DWORD PTR [EBP-24],ESI
            cmp ESI,EDI
            mov BYTE PTR [EBP-4],1
            je $L_466df5

            mov EAX,DWORD PTR [EBX]
            mov DWORD PTR [ESI],EAX
            mov ECX,DWORD PTR [EBX+4]
            mov DWORD PTR [ESI+4],ECX
            mov EAX,DWORD PTR [EBX+12]
            mov ECX,DWORD PTR [EBX+8]
            mov DWORD PTR [ESI+8],EDI
            sub EAX,ECX
            mov DWORD PTR [ESI+12],EDI
            mov DWORD PTR [EBP+8],EAX
            mov DWORD PTR [ESI+16],EDI
            je $L_466dc1

            push EAX
            call _Stl_Allocate

            mov EDI,EAX
            mov EAX,DWORD PTR [EBP+8]
            add ESP,4
$L_466dc1::

            lea EDX,DWORD PTR [EDI+EAX*1]
            mov DWORD PTR [ESI+8],EDI
            mov DWORD PTR [ESI+12],EDI
            mov DWORD PTR [ESI+16],EDX
            mov ECX,DWORD PTR [EBX+12]
            mov EAX,DWORD PTR [EBX+8]
            mov DWORD PTR [EBP-20],ECX
            sub ECX,EAX
            push ECX
            push EAX
            push EDI
            mov DWORD PTR [EBP+8],EAX
            call _memmove

            mov ECX,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EBP-20]
            sub EDI,ECX
            add ESP,12
            add EDI,EAX
            mov DWORD PTR [ESI+12],EDI
            xor EDI,EDI
$L_466df5::

            add EBX,24
            add ESI,24
            mov BYTE PTR [EBP-4],0
            mov DWORD PTR [EBP-20],ESI
            jmp $L_466d7f
FUN_466d50 ENDP
$L_466e07 PROC

            mov EAX,DWORD PTR [EBP+16]
            mov EDI,DWORD PTR [EBP-20]
            cmp EAX,EDI
            je $L_466e34

            lea ESI,DWORD PTR [EAX+8]
$L_466e14::

            mov EAX,DWORD PTR [ESI+4]
            mov ECX,DWORD PTR [ESI]
            push EAX
            push ECX
            call @Shell_Nop@8

            add ESP,8
            mov ECX,ESI
            call @Vec1_Free@8

            add ESI,24
            lea EDX,DWORD PTR [ESI-8]
            cmp EDX,EDI
            jne $L_466e14
$L_466e34::

            push 0
            push 0
            call __CxxThrowException@8
$L_466e07 ENDP
$L_466e3d PROC

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,ESI
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_466e3d ENDP
ALIGN 16
$L_46713e LABEL NEAR
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
$L_4672ae LABEL NEAR
ALIGN 16
FUN_467320 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_640031
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,12
            push EBX
            mov EBX,DWORD PTR [EBP+8]
            push ESI
            push EDI
            mov EDI,DWORD PTR [EBP+16]
            xor ESI,ESI
            mov DWORD PTR [EBP-16],ESP
            mov DWORD PTR [EBP-20],EDI
            mov DWORD PTR [EBP-4],ESI
$L_46734f::

            cmp EBX,DWORD PTR [EBP+12]
            je $L_4673fe

            mov DWORD PTR [EBP-24],EDI
            cmp EDI,ESI
            mov BYTE PTR [EBP-4],1
            je $L_4673b6

            mov EAX,DWORD PTR [EBX+4]
            mov ECX,DWORD PTR [EBX]
            sub EAX,ECX
            mov DWORD PTR [EDI],ESI
            mov DWORD PTR [EBP+8],EAX
            mov DWORD PTR [EDI+4],ESI
            mov DWORD PTR [EDI+8],ESI
            je $L_467385

            push EAX
            call _Stl_Allocate

            mov ESI,EAX
            mov EAX,DWORD PTR [EBP+8]
            add ESP,4
$L_467385::

            add EAX,ESI
            mov DWORD PTR [EDI],ESI
            mov DWORD PTR [EDI+4],ESI
            mov DWORD PTR [EDI+8],EAX
            mov ECX,DWORD PTR [EBX+4]
            mov EAX,DWORD PTR [EBX]
            mov DWORD PTR [EBP-20],ECX
            sub ECX,EAX
            push ECX
            push EAX
            push ESI
            mov DWORD PTR [EBP+8],EAX
            call _memmove

            mov ECX,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EBP-20]
            sub ESI,ECX
            add ESP,12
            add ESI,EAX
            mov DWORD PTR [EDI+4],ESI
            xor ESI,ESI
$L_4673b6::

            add EBX,12
            add EDI,12
            mov BYTE PTR [EBP-4],0
            mov DWORD PTR [EBP-20],EDI
            jmp $L_46734f
FUN_467320 ENDP
$L_4673c5 PROC

            mov ESI,DWORD PTR [EBP+16]
            mov EDI,DWORD PTR [EBP-20]
            cmp ESI,EDI
            je $L_4673f5
$L_4673cf::

            mov ECX,DWORD PTR [ESI+4]
            mov EDX,DWORD PTR [ESI]
            push ECX
            push EDX
            call @Shell_Nop@8

            mov EAX,DWORD PTR [ESI]
            mov ECX,DWORD PTR [ESI+8]
            add ESP,8
            sub ECX,EAX
            push ECX
            push EAX
            mov ECX,ESI
            call _Stl_FreeBytes@8

            add ESI,12
            cmp ESI,EDI
            jne $L_4673cf
$L_4673f5::

            push 0
            push 0
            call __CxxThrowException@8
$L_4673c5 ENDP
$L_4673fe PROC

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,EDI
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_4673fe ENDP
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
$L_467728 LABEL NEAR
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
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
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
FUN_469bc0 PROC

            push EBX
            mov EBX,DWORD PTR [ESP+12]
            push EBP
            mov EBP,DWORD PTR [ESP+12]
            mov EAX,EBX
            push ESI
            sub EAX,EBP
            push EDI
            and AL,252
            cmp EAX,4
            jle $L_469c0c
$L_469bd7::

            push 0
            mov ECX,6
            sub ESP,24
            lea ESI,DWORD PTR [ESP+56]
            mov EDI,ESP
            add EBX,-4

            rep movsd 

            push ECX
            mov ECX,ESP
            push EBX
            call @Int_Copy@12

            push EBX
            push EBX
            push EBP
            call FUN_469c20

            mov ECX,EBX
            add ESP,44
            sub ECX,EBP
            and ECX,4294967292
            cmp ECX,4
            jg $L_469bd7
$L_469c0c::

            pop EDI
            pop ESI
            pop EBP
            pop EBX
            ret 
FUN_469bc0 ENDP
ALIGN 16
FUN_469c20 PROC

            mov ECX,DWORD PTR [ESP+12]
            push EBX
            mov EBX,DWORD PTR [ESP+8]
            push ESI
            push EDI
            lea ESI,DWORD PTR [ESP+32]
            mov EAX,DWORD PTR [EBX]
            sub ESP,24
            mov DWORD PTR [ECX],EAX
            mov ECX,6
            mov EDI,ESP
            lea EDX,DWORD PTR [ESP+52]

            rep movsd 

            push ECX
            mov ECX,ESP
            push EDX
            call @Int_Copy@12

            mov EAX,DWORD PTR [ESP+48]
            sub EAX,EBX
            sar EAX,2
            push EAX
            push 0
            push EBX
            call _Far_AdjustHeap

            add ESP,40
            pop EDI
            pop ESI
            pop EBX
            ret 
FUN_469c20 ENDP
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
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
FUN_46ad50 PROC

            mov ECX,DWORD PTR [ESP+12]
            mov EDX,DWORD PTR [ESP+20]
            push ESI
            mov ESI,DWORD PTR [ESP+8]
            push EDX
            push ECX
            mov EAX,DWORD PTR [ESI]
            mov DWORD PTR [ECX],EAX
            lea EAX,DWORD PTR [ESP+28]
            mov ECX,ESP
            push EAX
            call @Int_Copy@12

            mov ECX,DWORD PTR [ESP+20]
            sub ECX,ESI
            sar ECX,2
            push ECX
            push 0
            push ESI
            call _Heap504_Adjust

            add ESP,20
            pop ESI
            ret 
FUN_46ad50 ENDP
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
; AI multiplayer: src\ai\ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
ALIGN 16
_TEXT ENDS
END
