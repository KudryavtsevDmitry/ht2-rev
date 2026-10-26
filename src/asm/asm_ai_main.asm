; asm_ai_main.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_63b657:NEAR
EXTERN $L_63b678:NEAR
EXTERN ??2@YAPAXI@Z:NEAR
EXTERN ??3@YAXPAX@Z:NEAR
EXTERN @BadAlloc_Copy@12:PROC
EXTERN @Buf459df0_Destruct@8:PROC
EXTERN @Int_Clear@8:PROC
EXTERN @Int_Copy@12:PROC
EXTERN @ListB_Insert@20:PROC
EXTERN @Note_Construct@24:PROC
EXTERN @Rec40cb60_Destroy@8:PROC
EXTERN @Rec410e40_FreeVectors@8:PROC
EXTERN @Rec440dd0_FreeVectors@8:PROC
EXTERN @RopeConcat_Destroy@8:PROC
EXTERN @RopeSubstr_Destroy@8:PROC
EXTERN @Rope_FreeTree@8:PROC
EXTERN @Rope_Unref@8:PROC
EXTERN @SchHandle_Release@8:PROC
EXTERN @SchList_End@12:PROC
EXTERN @SchValues_Destroy@4:PROC
EXTERN @Stl_Lock@8:PROC
EXTERN @Str_FreeBoth@8:PROC
EXTERN @ValueBuf_Destroy@8:PROC
EXTERN @Vec104_Destroy@4:PROC
EXTERN @Vec16_Destroy@8:PROC
EXTERN @Vec24Rec_Destroy@4:PROC
EXTERN @Vec4_FreeStorage@8:PROC
EXTERN @Vec4_FreeStorageB@8:PROC
EXTERN @VecB_FreeStorage@8:PROC
EXTERN FUN_401000:NEAR
EXTERN UnusedTrap:NEAR
EXTERN _AI_Init68a830:PROC
EXTERN _AI_Init68cbe8:PROC
EXTERN _AI_Init68cc08:PROC
EXTERN _AI_Init68ceb0:PROC
EXTERN _AI_Init68cf00:PROC
EXTERN _AI_Init68cf48:PROC
EXTERN _AI_SwitchToRandom:PROC
EXTERN _AiStatic68ceb0_AtExit:PROC
EXTERN _Ai_AddCompetitor:PROC
EXTERN _Ai_FillStartGrid:PROC
EXTERN _Ai_HireTask:PROC
EXTERN _Ai_MarkAmbushes:PROC
EXTERN _Ai_PickPresent:PROC
EXTERN _Ai_PlayerEvent:PROC
EXTERN _Ai_PoolFree:PROC
EXTERN _Ai_UpdateAmbushes:PROC
EXTERN _Ai_UpdateChasedCars:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _MsgBuf_FreeThunk:PROC
EXTERN _Player_IsHuman:PROC
EXTERN _Static401e70_AtExit:PROC
EXTERN _Static401ec0_AtExit:PROC
EXTERN _Static402140_AtExit:PROC
EXTERN _Static402480_AtExit:PROC
EXTERN _Stl_ChunkAlloc:PROC
EXTERN _Task_ChangeLanes:PROC
EXTERN _Task_DropOldPresents:PROC
EXTERN _Task_DropOldRacePresents:PROC
EXTERN _Task_FarSounds:PROC
EXTERN _Task_OrdersExpired:PROC
EXTERN _Task_PresentOrdersExpired:PROC
EXTERN _Task_RandomHits:PROC
EXTERN _Task_TrafficJam:PROC
EXTERN _Task_Waiting:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN __imp__InterlockedDecrement@4:PROC
EXTERN __imp__InterlockedExchange@8:PROC
EXTERN __imp__Sleep@4:PROC
EXTERN _atexit:PROC
EXTERN _free:PROC

PUBLIC $L_401430, $L_4019a0, $L_4019d2, $L_4019e6, $L_4019ee, $L_401a1a, $L_401a3b, $L_401a61
PUBLIC $L_401c01, $L_401c12, $L_401c1b, $L_401c23, $L_401e30, $L_401e40, $L_401e80, $L_401e90
PUBLIC $L_402100, $L_402110, $L_402240, $L_402250, $L_402430, $L_402440, $L_402730, $L_41f968
PUBLIC $L_42704f, $L_427054, $L_427084, $L_430000, $L_434e66, $L_434ed5, $L_434ed7, $L_434fc0
PUBLIC $L_434fc2, $L_435036, $L_435038, $L_4350ac, $L_4350ae, $L_43511d, $L_43511f, $L_435194
PUBLIC $L_435196, $L_435204, $L_435206, $L_435271, $L_435273, $L_4352e2, $L_4352e4, $L_435353
PUBLIC $L_435355, $L_4353c4, $L_4353c6, $L_435435, $L_435437, $L_4354ae, $L_4354b0, $L_435523
PUBLIC $L_435525, $L_435598, $L_43559a, $L_435609, $L_43560b, $L_435633, $L_440000, $L_443a41
PUBLIC $L_443a51, $L_443a82, $L_443aa9, $L_44bbd3, $L_44bc14, $L_44bc34, $L_44bc64, $L_44bc69
PUBLIC $L_44bc7e, $L_44bc8a, $L_44bca7, $L_44bcb7, $L_44bccd, $L_44bcd6, $L_44c12d, $L_44c157
PUBLIC $L_44c430, $L_44c45a, $L_44c6dc, $L_44c706, $L_44c9c7, $L_44c9d5, $L_44c9ec, $L_44ce6f
PUBLIC $L_44ce99, $L_44d12f, $L_44d159, $L_44d4a3, $L_44d4cd, $L_44d75f, $L_44d789, $L_44da1f
PUBLIC $L_44da49, $L_44dd51, $L_44dd5f, $L_44dd79, $L_44e0ef, $L_44e119, $L_44e3af, $L_44e3d9
PUBLIC $L_44e66f, $L_44e699, $L_44e92f, $L_44e959, $L_44ebef, $L_44ec19, $L_44eeaf, $L_44eed9
PUBLIC $L_44f16f, $L_44f199, $L_44f5d7, $L_44f601, $L_44f60c, $L_44f617, $L_44f622, $L_44f62e
PUBLIC $L_44f637, $L_44f640, $L_44f9aa, $L_44f9d4, $L_44fc71, $L_44fc77, $L_44fc88, $L_44fcb9
PUBLIC $L_44fcc6, $L_45010f, $L_450139, $L_450541, $L_450550, $L_45056f, $L_450596, $L_4507ec
PUBLIC $L_4507fa, $L_450819, $L_450840, $L_450cac, $L_450cbe, $L_450cd5, $L_4511ef, $L_451219
PUBLIC $L_451482, $L_451490, $L_4514a7, $L_45170a, $L_451734, $L_451a3e, $L_451a68, $L_451cff
PUBLIC $L_451d29, $L_451fbf, $L_451fe9, $L_45225a, $L_452284, $L_4525d5, $L_4525ff, $L_4528c5
PUBLIC $L_4528d3, $L_4528ea, $L_452b7f, $L_452ba9, $L_452e44, $L_452e53, $L_452e70, $L_4530a4
PUBLIC $L_4530b5, $L_4530d4, $L_4530fb, $L_453b1f, $L_453b49, $L_4548e1, $L_45490b, $L_45641f
PUBLIC $L_456449, $L_45716a, $L_457194

_TEXT SEGMENT
$L_4013e0 PROC

            mov ECX,OFFSET (_KingBss+32)
            jmp @SchValues_Destroy@4
          BYTE 090H
          BYTE 090H
          BYTE 090H

            nop
            nop
            nop
$L_4013e0 ENDP
$L_4013f0 PROC

            call _AI_Init68cf48

            jmp $L_401420
$L_4013f0 ENDP
ALIGN 16
$L_401420 PROC

            push OFFSET $L_401430
            call _atexit

            pop ECX
            ret 

            nop
            nop
            nop
            nop
$L_401430::

            mov ECX,OFFSET (_KingBss+10024)
            jmp @Vec4_FreeStorageB@8
          BYTE 090H
          BYTE 090H
          BYTE 090H

            nop
            nop
            nop
$L_401420 ENDP
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
$L_401990 PROC

            mov ECX,OFFSET (_KingBss+4472)
            jmp $L_4019a0
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_4019a0::

            push -1
            push OFFSET $L_63b657
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ECX
            push EBX
            push EBP
            mov EBX,ECX
            push ESI
            push EDI
            mov DWORD PTR [ESP+16],EBX
            mov EAX,DWORD PTR [EBX+4]
            lea ESI,DWORD PTR [EBX+4]
            xor EBP,EBP
            mov EDI,DWORD PTR [EAX]
            mov DWORD PTR [ESP+28],EBP
            cmp EDI,EAX
            je $L_4019e6
$L_4019d2::

            mov EAX,DWORD PTR [EDI+8]
            push EAX
            call ??3@YAXPAX@Z

            mov EDI,DWORD PTR [EDI]
            mov EAX,DWORD PTR [ESI]
            add ESP,4
            cmp EDI,EAX
            jne $L_4019d2
$L_4019e6::

            mov EAX,DWORD PTR [ESI]
            mov EDI,DWORD PTR [EAX]
            cmp EDI,EAX
            je $L_401a1a
$L_4019ee::

            mov EBP,EDI
            mov EDI,DWORD PTR [EDI]
            mov ECX,OFFSET (_KingBss+10724)
            call @Stl_Lock@8

            mov EAX,DWORD PTR [(_KingBss+10664)]
            mov DWORD PTR [EBP],EAX
            mov DWORD PTR [(_KingBss+10664)],EBP
            mov DWORD PTR [(_KingBss+10724)],0
            cmp EDI,DWORD PTR [ESI]
            jne $L_4019ee

            xor EBP,EBP
$L_401a1a::

            mov EAX,DWORD PTR [ESI]
            mov DWORD PTR [ESP+28],4294967295
            neg EBX
            sbb EBX,EBX
            mov DWORD PTR [EAX],EAX
            mov EAX,DWORD PTR [ESI]
            and EBX,ESI
            mov EDI,EBX
            mov DWORD PTR [EAX+4],EAX
            mov EAX,DWORD PTR [EDI]
            mov ESI,DWORD PTR [EAX]
            cmp ESI,EAX
            je $L_401a61
$L_401a3b::

            mov EBX,ESI
            mov ESI,DWORD PTR [ESI]
            mov ECX,OFFSET (_KingBss+10724)
            call @Stl_Lock@8

            mov ECX,DWORD PTR [(_KingBss+10664)]
            mov DWORD PTR [EBX],ECX
            mov DWORD PTR [(_KingBss+10664)],EBX
            mov DWORD PTR [(_KingBss+10724)],EBP
            cmp ESI,DWORD PTR [EDI]
            jne $L_401a3b
$L_401a61::

            mov EAX,DWORD PTR [EDI]
            mov ECX,OFFSET (_KingBss+10724)
            mov DWORD PTR [EAX],EAX
            mov EAX,DWORD PTR [EDI]
            mov DWORD PTR [EAX+4],EAX
            mov EDI,DWORD PTR [EDI]
            call @Stl_Lock@8

            mov EDX,DWORD PTR [(_KingBss+10664)]
            mov DWORD PTR [EDI],EDX
            mov DWORD PTR [(_KingBss+10664)],EDI
            mov DWORD PTR [(_KingBss+10724)],EBP
            mov ECX,DWORD PTR [ESP+20]
            pop EDI
            pop ESI
            pop EBP
            pop EBX
            mov DWORD PTR FS:[0],ECX
            add ESP,16
            ret 
$L_401990 ENDP
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
$L_401b90 PROC

            push -1
            push OFFSET $L_63b678
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ECX
            push EBX
            mov EBX,ECX
            push EDI
            mov ECX,OFFSET (_KingBss+10724)
            mov BYTE PTR [EBX],1
            call @Stl_Lock@8

            mov DWORD PTR [ESP+20],0
            mov EAX,DWORD PTR [(_KingBss+10664)]
            test EAX,EAX
            jne $L_401c1b

            lea EAX,DWORD PTR [ESP+8]
            mov DWORD PTR [ESP+8],20
            push EAX
            push 16
            call _Stl_ChunkAlloc

            mov EDI,DWORD PTR [ESP+16]
            add ESP,8
            cmp EDI,1
            je $L_401c23

            lea ECX,DWORD PTR [EAX+16]
            push ESI
            mov DWORD PTR [(_KingBss+10664)],ECX
            mov EDX,ECX
            add ECX,16
            mov ESI,1
            dec EDI
            cmp EDI,ESI
            je $L_401c12
$L_401c01::

            mov DWORD PTR [EDX],ECX
            mov EDI,DWORD PTR [ESP+12]
            mov EDX,ECX
            inc ESI
            add ECX,16
            dec EDI
            cmp EDI,ESI
            jne $L_401c01
$L_401c12::

            mov DWORD PTR [EDX],0
            pop ESI
            jmp $L_401c23
$L_401c1b::

            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [(_KingBss+10664)],ECX
$L_401c23::

            mov DWORD PTR [(_KingBss+10724)],0
            mov ECX,DWORD PTR [ESP+12]
            mov DWORD PTR [EBX+4],EAX
            mov DWORD PTR [EAX],EAX
            mov EAX,DWORD PTR [EBX+4]
            pop EDI
            mov DWORD PTR [EAX+4],EAX
            mov EAX,EBX
            pop EBX
            mov DWORD PTR FS:[0],ECX
            add ESP,16
            ret 
$L_401b90 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
$L_401e30::
            mov ECX,OFFSET (_KingBss+9368)
            jmp @Vec104_Destroy@4
            nop
            nop
            nop
            nop
            nop
            nop
$L_401e40::

            call _AI_Init68cc08

            jmp _Static401e70_AtExit
ALIGN 16
$L_401e80::
            mov ECX,OFFSET (_KingBss+9192)
            jmp @Vec24Rec_Destroy@4
            nop
            nop
            nop
            nop
            nop
            nop
$L_401e90::

            call _AI_Init68cf00

            jmp _Static401ec0_AtExit
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_402100::
            mov ECX,OFFSET (_KingBss+9336)
            jmp @Vec4_FreeStorage@8
            nop
            nop
            nop
            nop
            nop
            nop
$L_402110::

            call _AI_Init68cbe8

            jmp _Static402140_AtExit
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_402240::
            mov ECX,OFFSET (_KingBss+9208)
            jmp @Vec4_FreeStorage@8
            nop
            nop
            nop
            nop
            nop
            nop
$L_402250::

            call _AI_Init68ceb0

            jmp _AiStatic68ceb0_AtExit
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_402430::
            mov ECX,OFFSET (_KingBss+9352)
            jmp @Vec4_FreeStorageB@8
            nop
            nop
            nop
            nop
            nop
            nop
$L_402440::

            call _AI_Init68a830

            jmp _Static402480_AtExit
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_402730::
            mov ECX,OFFSET (_KingBss+9320)
            jmp @Vec4_FreeStorageB@8
            nop
            nop
            nop
            nop
            nop
            nop
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
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
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
$L_41d32b PROC

            fld QWORD PTR [(_KingRdata+1656)]
            ret 4
$L_41d32b ENDP
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
$L_41f930 PROC

            jmp DWORD PTR [ECX+40]
$L_41f930 ENDP
$L_41f940 PROC

            mov EDX,DWORD PTR [ECX+48]
            lea EAX,DWORD PTR [ECX+48]
            cmp DWORD PTR [EDX+88],305419896
            jne $L_41f968

            push ECX
            mov ECX,DWORD PTR [ECX+52]
            mov EDX,ESP
            push ECX
            mov DWORD PTR [EDX],ECX
            mov ECX,ESP
            push EAX
            call @Int_Copy@12

            call _Ai_PickPresent

            add ESP,8
$L_41f968::

            ret 
$L_41f940 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
$L_427020 PROC

            push ESI
            lea ESI,DWORD PTR [ECX+48]
            mov ECX,DWORD PTR [ECX+48]
            cmp DWORD PTR [ECX+88],305419896
            jne $L_427084

            fld QWORD PTR [ECX+136]
            fcomp QWORD PTR [(_KingRdata+1656)]
            mov EDX,65536
            fnstsw AX
            test AH,1
            mov EAX,DWORD PTR [ECX+96]
            je $L_42704f

            or EAX,EDX
            jmp $L_427054
$L_42704f::

            and EAX,4294901759
$L_427054::

            mov DWORD PTR [ECX+96],EAX
            mov EAX,DWORD PTR [ESI]
            test DWORD PTR [EAX+96],EDX
            je $L_427084

            push ECX
            mov ECX,ESP
            mov DWORD PTR [ECX],EAX
            call _Player_IsHuman

            add ESP,4
            test AL,AL
            je $L_427084

            push 0
            push 12
            push ECX
            mov ECX,ESP
            push ESI
            call @Int_Copy@12

            call _Ai_PlayerEvent

            add ESP,12
$L_427084::

            pop ESI
            ret 
$L_427020 ENDP
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
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
$L_430000 LABEL NEAR
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16

ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
$L_434e64 PROC

            xor ESI,ESI
$L_434e66::

            lea EAX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EBP
            push EAX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea ECX,DWORD PTR [ESP+56]
            push ECX
            mov ECX,OFFSET (_KingBss+4476)
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],4
            je $L_434ed5

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],1077641216
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8960)
            mov DWORD PTR [ESI+40],OFFSET _Task_Waiting
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_434ed7
$L_434ed5::

            xor ESI,ESI
$L_434ed7::

            lea EDX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EBP
            push EDX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EAX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EAX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],5
            je $L_434fc0

            mov ECX,DWORD PTR [(_KingBss+4740)]
            xor EDX,EDX
            mov EDI,55
            push OFFSET (_KingData+8944)
            lea EAX,DWORD PTR [ECX+1]
            mov ECX,55
            div ECX
            mov DWORD PTR [ESP+24],EBX
            sub ESP,8
            mov ECX,EDX
            mov EDX,DWORD PTR [(_KingBss+4744)]
            mov DWORD PTR [(_KingBss+4740)],ECX
            lea EAX,DWORD PTR [EDX+1]
            xor EDX,EDX
            div EDI
            mov DWORD PTR [(_KingBss+4744)],EDX
            mov EAX,DWORD PTR [EDX*4+(_KingBss+4520)]
            mov EDX,DWORD PTR [ECX*4+(_KingBss+4520)]
            sub EDX,EAX
            mov DWORD PTR [ECX*4+(_KingBss+4520)],EDX
            mov ECX,DWORD PTR [(_KingBss+4740)]
            xor EDX,EDX
            mov EAX,DWORD PTR [ECX*4+(_KingBss+4520)]
            mov ECX,300
            div ECX
            mov ECX,ESI
            add EDX,600
            mov DWORD PTR [ESP+28],EDX
            fild QWORD PTR [ESP+28]
            fstp QWORD PTR [ESP]
            push OFFSET _AI_SwitchToRandom
            call @Note_Construct@24

            mov DWORD PTR [ESI],OFFSET (_KingRdata+2312)
            mov EDX,DWORD PTR [(_KingBss+10472)]
            mov EAX,DWORD PTR [(_KingBss+10476)]
            mov DWORD PTR [ESI+16],EDX
            mov DWORD PTR [ESI+20],EAX
            jmp $L_434fc2
$L_434fc0::

            xor ESI,ESI
$L_434fc2::

            lea ECX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],4294967295
            push ECX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EDX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EDX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],6
            mov EBP,1072693248
            je $L_435036

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],EBP
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8928)
            mov DWORD PTR [ESI+40],OFFSET _Ai_UpdateAmbushes
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_435038
$L_435036::

            xor ESI,ESI
$L_435038::

            lea EAX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],4294967295
            push EAX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea ECX,DWORD PTR [ESP+56]
            push ECX
            mov ECX,OFFSET (_KingBss+4476)
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],7
            mov EDI,1077805056
            je $L_4350ac

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],EDI
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8908)
            mov DWORD PTR [ESI+40],OFFSET _Ai_FillStartGrid
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_4350ae
$L_4350ac::

            xor ESI,ESI
$L_4350ae::

            lea EDX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],4294967295
            push EDX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EAX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EAX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],8
            je $L_43511d

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],EDI
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8880)
            mov DWORD PTR [ESI+40],OFFSET _Ai_AddCompetitor
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_43511f
$L_43511d::

            xor ESI,ESI
$L_43511f::

            lea ECX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],4294967295
            push ECX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [(_KingBss+10496)],ESI
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EDX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EDX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],9
            je $L_435194

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],EDI
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8864)
            mov DWORD PTR [ESI+40],OFFSET _Ai_HireTask
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_435196
$L_435194::

            xor ESI,ESI
$L_435196::

            lea EAX,DWORD PTR [ESP+40]
            or EDI,4294967295
            push EAX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+44],EDI
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea ECX,DWORD PTR [ESP+56]
            push ECX
            mov ECX,OFFSET (_KingBss+4476)
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],10
            je $L_435204

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],EBP
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8844)
            mov DWORD PTR [ESI+40],OFFSET _Task_OrdersExpired
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_435206
$L_435204::

            xor ESI,ESI
$L_435206::

            lea EDX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EDI
            push EDX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EAX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EAX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],11
            je $L_435271

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],EBP
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8824)
            mov DWORD PTR [ESI+40],OFFSET _Task_PresentOrdersExpired
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_435273
$L_435271::

            xor ESI,ESI
$L_435273::

            lea ECX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EDI
            push ECX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EDX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EDX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],12
            je $L_4352e2

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],1076101120
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8796)
            mov DWORD PTR [ESI+40],OFFSET _Task_DropOldPresents
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_4352e4
$L_4352e2::

            xor ESI,ESI
$L_4352e4::

            lea EAX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EDI
            push EAX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea ECX,DWORD PTR [ESP+56]
            push ECX
            mov ECX,OFFSET (_KingBss+4476)
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],13
            je $L_435353

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],1077084160
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8776)
            mov DWORD PTR [ESI+40],OFFSET _Task_DropOldRacePresents
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_435355
$L_435353::

            xor ESI,ESI
$L_435355::

            lea EDX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EDI
            push EDX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EAX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EAX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],14
            je $L_4353c4

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],1074266112
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8756)
            mov DWORD PTR [ESI+40],OFFSET _Ai_UpdateChasedCars
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_4353c6
$L_4353c4::

            xor ESI,ESI
$L_4353c6::

            lea ECX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EDI
            push ECX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EDX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EDX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],15
            je $L_435435

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EBX
            mov DWORD PTR [ESI+28],1081262080
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8740)
            mov DWORD PTR [ESI+40],OFFSET _Ai_MarkAmbushes
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_435437
$L_435435::

            xor ESI,ESI
$L_435437::

            lea EAX,DWORD PTR [ESP+40]
            or EBP,4294967295
            push EAX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+44],EBP
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea ECX,DWORD PTR [ESP+56]
            push ECX
            mov ECX,OFFSET (_KingBss+4476)
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],16
            mov EDI,1717986918
            je $L_4354ae

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EDI
            mov DWORD PTR [ESI+28],1075996262
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8728)
            mov DWORD PTR [ESI+40],OFFSET _Task_TrafficJam
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_4354b0
$L_4354ae::

            xor ESI,ESI
$L_4354b0::

            lea EDX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EBP
            push EDX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EAX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EAX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],17
            je $L_435523

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],2576980378
            mov DWORD PTR [ESI+28],1077615001
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8712)
            mov DWORD PTR [ESI+40],OFFSET _Task_FarSounds
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_435525
$L_435523::

            xor ESI,ESI
$L_435525::

            lea ECX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EBP
            push ECX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea EDX,DWORD PTR [ESP+56]
            mov ECX,OFFSET (_KingBss+4476)
            push EDX
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+44],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],18
            je $L_435598

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],858993459
            mov DWORD PTR [ESI+28],1075655475
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8700)
            mov DWORD PTR [ESI+40],OFFSET _Task_ChangeLanes
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_43559a
$L_435598::

            xor ESI,ESI
$L_43559a::

            lea EAX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+32],EBP
            push EAX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+52],ESI
            call @SchList_End@12

            lea ECX,DWORD PTR [ESP+56]
            push ECX
            mov ECX,OFFSET (_KingBss+4476)
            call @ListB_Insert@20

            push 48
            call ??2@YAPAXI@Z

            mov ESI,EAX
            add ESP,4
            mov DWORD PTR [ESP+40],ESI
            cmp ESI,EBX
            mov DWORD PTR [ESP+32],19
            je $L_435609

            mov DWORD PTR [ESI+8],EBX
            call FUN_401000

            fstp QWORD PTR [ESI+16]
            mov DWORD PTR [ESI+24],EDI
            mov DWORD PTR [ESI+28],1075078758
            mov DWORD PTR [ESI+32],OFFSET (_KingData+8688)
            mov DWORD PTR [ESI+40],OFFSET _Task_RandomHits
            mov DWORD PTR [ESI],OFFSET (_KingRdata+2336)
            jmp $L_43560b
$L_435609::

            xor ESI,ESI
$L_43560b::

            lea EDX,DWORD PTR [ESP+44]
            mov DWORD PTR [ESP+32],EBP
            push EDX
            push ECX
            push ESP
            mov ECX,OFFSET (_KingBss+4476)
            mov DWORD PTR [ESP+56],ESI
            call @SchList_End@12

            lea EAX,DWORD PTR [ESP+24]
            mov ECX,OFFSET (_KingBss+4476)
            push EAX
            call @ListB_Insert@20
$L_435633::

            mov ECX,DWORD PTR [ESP+24]
            pop EDI
            pop ESI
            pop EBP
            mov BYTE PTR [(_KingBss+10444)],1
            mov DWORD PTR FS:[0],ECX
            pop EBX
            add ESP,20
            ret 
$L_434e64 ENDP
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
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
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
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
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
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
$L_440000 LABEL NEAR    ; a data table holds this address
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
$L_443a10 PROC

            push -1
            push OFFSET UnusedTrap
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ECX
            push EBX
            push ESI
            push EDI
            lea EDI,DWORD PTR [ECX+120]
            mov DWORD PTR [ESP+12],EDI
            mov EBX,DWORD PTR [EDI+4]
            mov ESI,DWORD PTR [EDI]
            cmp ESI,EBX
            mov DWORD PTR [ESP+24],0
            je $L_443a51
$L_443a41::

            push ESI
            call _MsgBuf_FreeThunk

            add ESI,16
            add ESP,4
            cmp ESI,EBX
            jne $L_443a41
$L_443a51::

            mov ESI,DWORD PTR [EDI]
            mov EAX,DWORD PTR [EDI+8]
            sub EAX,ESI
            sar EAX,4
            je $L_443aa9

            shl EAX,4
            cmp EAX,128
            jbe $L_443a82

            push ESI
            call _free

            add ESP,4
            pop EDI
            pop ESI
            pop EBX
            mov ECX,DWORD PTR [ESP+4]
            mov DWORD PTR FS:[0],ECX
            add ESP,16
            ret 
$L_443a82::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea EDI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov EAX,DWORD PTR [EDI]
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [EDI],ESI
            mov ECX,OFFSET (_KingBss+10724)
            call @Int_Clear@8
$L_443aa9::

            mov ECX,DWORD PTR [ESP+16]
            pop EDI
            pop ESI
            pop EBX
            mov DWORD PTR FS:[0],ECX
            add ESP,16
            ret 
$L_443a10 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
$L_448570 PROC

            sub ESP,12
            push ECX
            lea ECX,DWORD PTR [ESP+4]
            call @BadAlloc_Copy@12

            lea EAX,DWORD PTR [ESP]
            push OFFSET (_KingRdata+60768)
            push EAX
            call __CxxThrowException@8

            nop
            nop
            nop
            nop
$L_448570 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_44b46a PROC

            mov ECX,OFFSET (_KingBss+10724)
            call @Stl_Lock@8

            mov EDX,DWORD PTR [(_KingBss+10688)]
            mov EAX,DWORD PTR [EBP-20]
            mov DWORD PTR [EAX],EDX
            mov DWORD PTR [(_KingBss+10688)],EAX
            mov DWORD PTR [(_KingBss+10724)],0
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
$L_44b46a ENDP
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
$L_44bbad PROC

            mov EAX,DWORD PTR [EBP+8]
            test EAX,EAX
            je $L_44bccd

            shl EAX,2
            cmp EAX,128
            jbe $L_44bbd3

            mov EDX,DWORD PTR [EBP-24]
            push EDX
            call _free

            add ESP,4
            jmp $L_44bccd
$L_44bbd3::

            add EAX,7
            push 1
            shr EAX,3
            push OFFSET (_KingBss+10724)
            lea EAX,DWORD PTR [EAX*4+(_KingBss+10656)]
            mov DWORD PTR [EBP+16],EAX
            call DWORD PTR __imp__InterlockedExchange@8

            test EAX,EAX
            je $L_44bcb7

            mov EDI,DWORD PTR [(_KingData+2108)]
            mov EAX,DWORD PTR [(_KingBss+10728)]
            mov DWORD PTR [EBP+8],17
            xor ESI,ESI
            test EDI,EDI
            jbe $L_44bc69

            mov EBX,EAX
            shr EBX,1
$L_44bc14::

            cmp ESI,EBX
            jb $L_44bc34

            mov EAX,DWORD PTR [(_KingBss+10724)]
            test EAX,EAX
            jne $L_44bc34

            push 1
            push OFFSET (_KingBss+10724)
            call DWORD PTR __imp__InterlockedExchange@8

            test EAX,EAX
            je $L_44bca7

            jmp $L_44bc64
$L_44bc34::

            mov ECX,DWORD PTR [EBP+8]
            mov EDX,DWORD PTR [EBP+8]
            imul ECX,EDX
            mov DWORD PTR [EBP+8],ECX
            mov EAX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EBP+8]
            imul EAX,ECX
            mov DWORD PTR [EBP+8],EAX
            mov EDX,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EBP+8]
            imul EDX,EAX
            mov DWORD PTR [EBP+8],EDX
            mov ECX,DWORD PTR [EBP+8]
            mov EDX,DWORD PTR [EBP+8]
            imul ECX,EDX
            mov DWORD PTR [EBP+8],ECX
$L_44bc64::

            inc ESI
            cmp ESI,EDI
            jb $L_44bc14
$L_44bc69::

            mov EBX,DWORD PTR __imp__Sleep@4
            mov DWORD PTR [(_KingData+2108)],30
            mov EDI,6
$L_44bc7e::

            cmp EDI,27
            mov ESI,EDI
            jle $L_44bc8a

            mov ESI,27
$L_44bc8a::

            push 1
            push OFFSET (_KingBss+10724)
            call DWORD PTR __imp__InterlockedExchange@8

            test EAX,EAX
            je $L_44bcb7

            cmp ESI,20
            jg $L_44bcd6

            push 0
            call EBX

            inc EDI
            jmp $L_44bc7e
$L_44bca7::

            mov DWORD PTR [(_KingBss+10728)],ESI
            mov DWORD PTR [(_KingData+2108)],1000
$L_44bcb7::

            mov ECX,DWORD PTR [EBP+16]
            mov EDX,DWORD PTR [ECX]
            mov EAX,DWORD PTR [EBP-24]
            mov DWORD PTR [EAX],EDX
            mov DWORD PTR [ECX],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_44bccd::

            push 0
            push 0
            call __CxxThrowException@8
$L_44bcd6::

            lea ECX,DWORD PTR [ESI-20]
            mov EAX,1
            shl EAX,CL
            push EAX
            call EBX

            inc EDI
            jmp $L_44bc7e
$L_44bbad ENDP
ALIGN 16
ALIGN 16
ALIGN 16
$L_44c111 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44c157

            cmp EAX,128
            jbe $L_44c12d

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44c157
$L_44c12d::

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
$L_44c157::

            push 0
            push 0
            call __CxxThrowException@8
$L_44c111 ENDP
ALIGN 16
ALIGN 16
$L_44c414 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44c45a

            cmp EAX,128
            jbe $L_44c430

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44c45a
$L_44c430::

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
$L_44c45a::

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
$L_44c414 ENDP
ALIGN 16
$L_44c6bd PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44c706

            shl EAX,3
            cmp EAX,128
            jbe $L_44c6dc

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            call _free

            add ESP,4
            jmp $L_44c706
$L_44c6dc::

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
$L_44c706::

            push 0
            push 0
            call __CxxThrowException@8

            nop
$L_44c6bd ENDP
ALIGN 16
$L_44c9bb PROC

            mov EBX,DWORD PTR [EBP-20]
            mov EDI,DWORD PTR [EBP-28]
            cmp EBX,EDI
            mov ESI,EBX
            je $L_44c9d5
$L_44c9c7::

            mov ECX,ESI
            call @SchHandle_Release@8

            add ESI,12
            cmp ESI,EDI
            jne $L_44c9c7
$L_44c9d5::

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44c9ec

            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,2
            push EAX
            push EBX
            call _Ai_PoolFree

            add ESP,8
$L_44c9ec::

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
$L_44c9bb ENDP
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
$L_44ce50 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44ce99

            shl EAX,2
            cmp EAX,128
            jbe $L_44ce6f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44ce99
$L_44ce6f::

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
$L_44ce99::

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
$L_44ce50 ENDP
ALIGN 16
$L_44d110 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44d159

            shl EAX,2
            cmp EAX,128
            jbe $L_44d12f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44d159
$L_44d12f::

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
$L_44d159::

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
$L_44d110 ENDP
ALIGN 16
$L_44d484 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44d4cd

            shl EAX,2
            cmp EAX,128
            jbe $L_44d4a3

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            call _free

            add ESP,4
            jmp $L_44d4cd
$L_44d4a3::

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
$L_44d4cd::

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
$L_44d484 ENDP
ALIGN 16
$L_44d740 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44d789

            shl EAX,2
            cmp EAX,128
            jbe $L_44d75f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44d789
$L_44d75f::

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
$L_44d789::

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
$L_44d740 ENDP
ALIGN 16
$L_44da00 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44da49

            shl EAX,2
            cmp EAX,128
            jbe $L_44da1f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44da49
$L_44da1f::

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
$L_44da49::

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
$L_44da00 ENDP
ALIGN 16
$L_44dd45 PROC

            mov EBX,DWORD PTR [EBP-20]
            mov EDI,DWORD PTR [EBP-28]
            cmp EBX,EDI
            mov ESI,EBX
            je $L_44dd5f
$L_44dd51::

            mov ECX,ESI
            call @ValueBuf_Destroy@8

            add ESI,104
            cmp ESI,EDI
            jne $L_44dd51
$L_44dd5f::

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44dd79

            lea EDX,DWORD PTR [EAX+EAX*2]
            lea EAX,DWORD PTR [EAX+EDX*4]
            shl EAX,3
            push EAX
            push EBX
            call _Ai_PoolFree

            add ESP,8
$L_44dd79::

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
$L_44dd45 ENDP
ALIGN 16
ALIGN 16
$L_44e0d0 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44e119

            shl EAX,2
            cmp EAX,128
            jbe $L_44e0ef

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44e119
$L_44e0ef::

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
$L_44e119::

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
$L_44e0d0 ENDP
ALIGN 16
$L_44e390 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44e3d9

            shl EAX,2
            cmp EAX,128
            jbe $L_44e3af

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44e3d9
$L_44e3af::

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
$L_44e3d9::

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
$L_44e390 ENDP
ALIGN 16
$L_44e650 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44e699

            shl EAX,2
            cmp EAX,128
            jbe $L_44e66f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44e699
$L_44e66f::

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
$L_44e699::

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
$L_44e650 ENDP
ALIGN 16
$L_44e910 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44e959

            shl EAX,2
            cmp EAX,128
            jbe $L_44e92f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44e959
$L_44e92f::

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
$L_44e959::

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
$L_44e910 ENDP
ALIGN 16
$L_44ebd0 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44ec19

            shl EAX,2
            cmp EAX,128
            jbe $L_44ebef

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44ec19
$L_44ebef::

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
$L_44ec19::

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
$L_44ebd0 ENDP
ALIGN 16
$L_44ee90 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44eed9

            shl EAX,2
            cmp EAX,128
            jbe $L_44eeaf

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44eed9
$L_44eeaf::

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
$L_44eed9::

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
$L_44ee90 ENDP
ALIGN 16
$L_44f150 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44f199

            shl EAX,2
            cmp EAX,128
            jbe $L_44f16f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44f199
$L_44f16f::

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
$L_44f199::

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
$L_44f150 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
$L_44f5bb PROC

            mov ESI,DWORD PTR [EBP+8]
            test ESI,ESI
            je $L_44f5d7

            lea EAX,DWORD PTR [ESI+4]
            push EAX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_44f5d7

            mov ECX,ESI
            call @Rope_FreeTree@8
$L_44f5d7::

            mov ESI,DWORD PTR [EBP+12]
            test ESI,ESI
            je $L_44f637

            lea ECX,DWORD PTR [ESI+4]
            push ECX
            call DWORD PTR __imp__InterlockedDecrement@4

            test EAX,EAX
            jne $L_44f637

            mov EAX,DWORD PTR [ESI+8]
            shl EAX,24
            sar EAX,24
            cmp EAX,3
            ja $L_44f637

            jmp DWORD PTR [EAX*4+$L_44f640]
$L_44f601::

            mov ECX,ESI
            call @Str_FreeBoth@8

            push 24
            jmp $L_44f62e
$L_44f60c::

            mov ECX,ESI
            call @RopeConcat_Destroy@8

            push 28
            jmp $L_44f62e
$L_44f617::

            mov ECX,ESI
            call @Buf459df0_Destruct@8

            push 28
            jmp $L_44f62e
$L_44f622::

            add ESI,-4
            mov ECX,ESI
            call @RopeSubstr_Destroy@8

            push 40
$L_44f62e::

            push ESI
            call _Ai_PoolFree

            add ESP,8
$L_44f637::

            push 0
            push 0
            call __CxxThrowException@8
$L_44f5bb ENDP
$L_44f640           DWORD $L_44f601
          DWORD $L_44f60c
          DWORD $L_44f622
          DWORD $L_44f617
ALIGN 16
ALIGN 16
$L_44f98b PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_44f9d4

            shl EAX,2
            cmp EAX,128
            jbe $L_44f9aa

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_44f9d4
$L_44f9aa::

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
$L_44f9d4::

            push 0
            push 0
            call __CxxThrowException@8

            nop
            nop
            nop
$L_44f98b ENDP
ALIGN 16
ALIGN 16
$L_44fc62 PROC

            mov ECX,DWORD PTR [EBP-40]
            mov EAX,DWORD PTR [EBP-44]
            sub ECX,EAX
            xor EDI,EDI
            sar ECX,2
            je $L_44fcc6
$L_44fc71::

            cmp DWORD PTR [EAX+EDI*4],0
            je $L_44fcb9
$L_44fc77::

            mov ESI,DWORD PTR [EAX+EDI*4]
            mov ECX,DWORD PTR [ESI+4]
            mov EBX,DWORD PTR [ESI]
            test ECX,ECX
            je $L_44fc88

            call @Rope_Unref@8
$L_44fc88::

            mov ECX,OFFSET (_KingBss+10724)
            call @Stl_Lock@8

            mov EDX,DWORD PTR [(_KingBss+10664)]
            mov DWORD PTR [ESI],EDX
            mov DWORD PTR [(_KingBss+10664)],ESI
            mov DWORD PTR [(_KingBss+10724)],0
            mov EAX,DWORD PTR [EBP-44]
            mov DWORD PTR [EAX+EDI*4],EBX
            mov EAX,DWORD PTR [EBP-44]
            cmp DWORD PTR [EAX+EDI*4],0
            jne $L_44fc77
$L_44fcb9::

            mov ECX,DWORD PTR [EBP-40]
            inc EDI
            sub ECX,EAX
            sar ECX,2
            cmp EDI,ECX
            jb $L_44fc71
$L_44fcc6::

            push 0
            push 0
            call __CxxThrowException@8

            nop
$L_44fc62 ENDP
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
$L_4500f0 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_450139

            shl EAX,2
            cmp EAX,128
            jbe $L_45010f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_450139
$L_45010f::

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
$L_450139::

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
$L_4500f0 ENDP
ALIGN 16
ALIGN 16
$L_450535 PROC

            mov EDI,DWORD PTR [EBP-20]
            mov EBX,DWORD PTR [EBP-28]
            cmp EDI,EBX
            mov ESI,EDI
            je $L_450550
$L_450541::

            lea ECX,DWORD PTR [ESI+8]
            call @VecB_FreeStorage@8

            add ESI,24
            cmp ESI,EBX
            jne $L_450541
$L_450550::

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_450596

            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,3
            cmp EAX,128
            jbe $L_45056f

            push EDI
            call _free

            add ESP,4
            jmp $L_450596
$L_45056f::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov EAX,DWORD PTR [ESI]
            mov DWORD PTR [EDI],EAX
            mov DWORD PTR [ESI],EDI
            mov DWORD PTR [(_KingBss+10724)],0
$L_450596::

            push 0
            push 0
            call __CxxThrowException@8

            nop
$L_450535 ENDP
ALIGN 16
$L_4507e0 PROC

            mov EDI,DWORD PTR [EBP-20]
            mov EBX,DWORD PTR [EBP-28]
            cmp EDI,EBX
            mov ESI,EDI
            je $L_4507fa
$L_4507ec::

            mov ECX,ESI
            call @Rec40cb60_Destroy@8

            add ESI,24
            cmp ESI,EBX
            jne $L_4507ec
$L_4507fa::

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_450840

            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,3
            cmp EAX,128
            jbe $L_450819

            push EDI
            call _free

            add ESP,4
            jmp $L_450840
$L_450819::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov EAX,DWORD PTR [ESI]
            mov DWORD PTR [EDI],EAX
            mov DWORD PTR [ESI],EDI
            mov DWORD PTR [(_KingBss+10724)],0
$L_450840::

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
$L_4507e0 ENDP
ALIGN 16
ALIGN 16
$L_450ca0 PROC

            mov EBX,DWORD PTR [EBP-28]
            mov EDI,DWORD PTR [EBP-24]
            cmp EBX,EDI
            mov ESI,EBX
            je $L_450cbe
$L_450cac::

            lea ECX,DWORD PTR [ESI+120]
            call @Vec16_Destroy@8

            add ESI,144
            cmp ESI,EDI
            jne $L_450cac
$L_450cbe::

            mov EAX,DWORD PTR [EBP-20]
            test EAX,EAX
            je $L_450cd5

            lea EDX,DWORD PTR [EAX+EAX*8]
            shl EDX,4
            push EDX
            push EBX
            call _Ai_PoolFree

            add ESP,8
$L_450cd5::

            push 0
            push 0
            call __CxxThrowException@8

            nop
            nop
$L_450ca0 ENDP
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
$L_4511d0 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_451219

            shl EAX,2
            cmp EAX,128
            jbe $L_4511ef

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_451219
$L_4511ef::

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
$L_451219::

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
$L_4511d0 ENDP
ALIGN 16
$L_451476 PROC

            mov EBX,DWORD PTR [EBP-20]
            mov EDI,DWORD PTR [EBP-28]
            cmp EBX,EDI
            mov ESI,EBX
            je $L_451490
$L_451482::

            mov ECX,ESI
            call @Rec410e40_FreeVectors@8

            add ESI,36
            cmp ESI,EDI
            jne $L_451482
$L_451490::

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_4514a7

            lea EAX,DWORD PTR [EAX+EAX*8]
            shl EAX,2
            push EAX
            push EBX
            call _Ai_PoolFree

            add ESP,8
$L_4514a7::

            push 0
            push 0
            call __CxxThrowException@8
$L_451476 ENDP
ALIGN 16
$L_4516eb PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_451734

            shl EAX,2
            cmp EAX,128
            jbe $L_45170a

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            call _free

            add ESP,4
            jmp $L_451734
$L_45170a::

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
$L_451734::

            push 0
            push 0
            call __CxxThrowException@8

            nop
            nop
            nop
$L_4516eb ENDP
ALIGN 16
$L_451a1f PROC

            mov EAX,DWORD PTR [EBP-20]
            test EAX,EAX
            je $L_451a68

            shl EAX,4
            cmp EAX,128
            jbe $L_451a3e

            mov EAX,DWORD PTR [EBP-24]
            push EAX
            call _free

            add ESP,4
            jmp $L_451a68
$L_451a3e::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP-24]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_451a68::

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
            nop
$L_451a1f ENDP
ALIGN 16
$L_451ce0 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_451d29

            shl EAX,2
            cmp EAX,128
            jbe $L_451cff

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_451d29
$L_451cff::

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
$L_451d29::

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
$L_451ce0 ENDP
ALIGN 16
$L_451fa0 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_451fe9

            shl EAX,2
            cmp EAX,128
            jbe $L_451fbf

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_451fe9
$L_451fbf::

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
$L_451fe9::

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
$L_451fa0 ENDP
ALIGN 16
$L_45223b PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_452284

            shl EAX,2
            cmp EAX,128
            jbe $L_45225a

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            call _free

            add ESP,4
            jmp $L_452284
$L_45225a::

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
$L_452284::

            push 0
            push 0
            call __CxxThrowException@8

            nop
            nop
            nop
$L_45223b ENDP
ALIGN 16
ALIGN 16
$L_4525b6 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_4525ff

            shl EAX,2
            cmp EAX,128
            jbe $L_4525d5

            mov ECX,DWORD PTR [EBP+16]
            push ECX
            call _free

            add ESP,4
            jmp $L_4525ff
$L_4525d5::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov EDX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP+16]
            mov DWORD PTR [EAX],EDX
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_4525ff::

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
$L_4525b6 ENDP
ALIGN 16
$L_4528b9 PROC

            mov EBX,DWORD PTR [EBP-24]
            mov EDI,DWORD PTR [EBP-32]
            cmp EBX,EDI
            mov ESI,EBX
            je $L_4528d3
$L_4528c5::

            mov ECX,ESI
            call @VecB_FreeStorage@8

            add ESI,12
            cmp ESI,EDI
            jne $L_4528c5
$L_4528d3::

            mov EAX,DWORD PTR [EBP-28]
            test EAX,EAX
            je $L_4528ea

            lea ECX,DWORD PTR [EAX+EAX*2]
            shl ECX,2
            push ECX
            push EBX
            call _Ai_PoolFree

            add ESP,8
$L_4528ea::

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
$L_4528b9 ENDP
ALIGN 16
$L_452b60 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_452ba9

            shl EAX,2
            cmp EAX,128
            jbe $L_452b7f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_452ba9
$L_452b7f::

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
$L_452ba9::

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
$L_452b60 ENDP
ALIGN 16
$L_452e38 PROC

            mov EBX,DWORD PTR [EBP-20]
            mov EDI,DWORD PTR [EBP-28]
            cmp EBX,EDI
            mov ESI,EBX
            je $L_452e53
$L_452e44::

            lea ECX,DWORD PTR [ESI+4]
            call @VecB_FreeStorage@8

            add ESI,56
            cmp ESI,EDI
            jne $L_452e44
$L_452e53::

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_452e70

            lea ECX,DWORD PTR [EAX*8]
            sub ECX,EAX
            shl ECX,3
            push ECX
            push EBX
            call _Ai_PoolFree

            add ESP,8
$L_452e70::

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
$L_452e38 ENDP
ALIGN 16
$L_453098 PROC

            mov EDI,DWORD PTR [EBP-28]
            mov EBX,DWORD PTR [EBP-24]
            cmp EDI,EBX
            mov ESI,EDI
            je $L_4530b5
$L_4530a4::

            mov ECX,ESI
            call @Rec440dd0_FreeVectors@8

            add ESI,192
            cmp ESI,EBX
            jne $L_4530a4
$L_4530b5::

            mov EAX,DWORD PTR [EBP-20]
            test EAX,EAX
            je $L_4530fb

            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,6
            cmp EAX,128
            jbe $L_4530d4

            push EDI
            call _free

            add ESP,4
            jmp $L_4530fb
$L_4530d4::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov EDX,DWORD PTR [ESI]
            mov DWORD PTR [EDI],EDX
            mov DWORD PTR [ESI],EDI
            mov DWORD PTR [(_KingBss+10724)],0
$L_4530fb::

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
$L_453098 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
$L_453b00 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_453b49

            shl EAX,2
            cmp EAX,128
            jbe $L_453b1f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_453b49
$L_453b1f::

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
$L_453b49::

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
$L_453b00 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
$L_4548bf PROC

            mov EAX,DWORD PTR [EBP-20]
            test EAX,EAX
            je $L_45490b

            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,2
            cmp EAX,128
            jbe $L_4548e1

            mov EAX,DWORD PTR [EBP-24]
            push EAX
            call _free

            add ESP,4
            jmp $L_45490b
$L_4548e1::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP-24]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_45490b::

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
$L_4548bf ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
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
$L_456400 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_456449

            shl EAX,2
            cmp EAX,128
            jbe $L_45641f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_456449
$L_45641f::

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
$L_456449::

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
$L_456400 ENDP
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
$L_45714b PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_457194

            shl EAX,5
            cmp EAX,128
            jbe $L_45716a

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            call _free

            add ESP,4
            jmp $L_457194
$L_45716a::

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
$L_457194::

            push 0
            push 0
            call __CxxThrowException@8

            nop
            nop
            nop
$L_45714b ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
; AI main: src\ai\ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; AI STL instances: src\ai\ai_containers.cpp, nmake ASM_AI_CONTAINERS=1 assembles the original
ALIGN 16
_TEXT ENDS
END
