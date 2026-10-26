; asm_ai_main.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN _Ai_Clock:NEAR
EXTERN _UnusedTrap:NEAR
EXTERN $L_63b657:NEAR
EXTERN $L_63b678:NEAR
EXTERN ??2@YAPAXI@Z:NEAR
EXTERN ??3@YAXPAX@Z:NEAR
EXTERN @BadAlloc_Copy@12:PROC
EXTERN @Int_Clear@8:PROC
EXTERN @Int_Copy@12:PROC
EXTERN @ListB_Insert@20:PROC
EXTERN @Note_Construct@24:PROC
EXTERN @SchList_End@12:PROC
EXTERN @SchValues_Destroy@4:PROC
EXTERN @Stl_Lock@8:PROC
EXTERN @Vec104_Destroy@4:PROC
EXTERN @Vec24Rec_Destroy@4:PROC
EXTERN @Vec4_FreeStorage@8:PROC
EXTERN @Vec4_FreeStorageB@8:PROC
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
PUBLIC $L_44bc7e, $L_44bc8a, $L_44bca7, $L_44bcb7, $L_44bccd, $L_44bcd6

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            call _Ai_Clock

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
            push OFFSET _UnusedTrap
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
_TEXT ENDS
END
