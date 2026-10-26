; asm_unit_5d8540.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_5d7c20:NEAR
EXTERN @App_Path@12:PROC
EXTERN @CGameApp_SetVideoMode@24:PROC
EXTERN @EventRing_Push@12:PROC
EXTERN @FrameWnd_Destruct@4:PROC
EXTERN @FrameWnd_OnSysCommand@16:PROC
EXTERN @Module_Load@20:PROC
EXTERN @Module_LoadImage@16:PROC
EXTERN @NameHash_ResolvePending@8:PROC
EXTERN @Resources_AddTable@12:PROC
EXTERN @Resources_Unload@12:PROC
EXTERN @Room_Each@16:PROC
EXTERN @Wnd_Default@4:PROC
EXTERN @Wnd_SetWindowPos@32:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _Mode_TextureLevel:PROC
EXTERN _Room_QueueDelete:PROC
EXTERN _Set71e7d4:PROC
EXTERN _Sound_Close:PROC
EXTERN _Sound_SetVolume:PROC
EXTERN _Sound_Stop:PROC
EXTERN _Sys_Fatal:PROC
EXTERN _Video_Get67fcf0:PROC
EXTERN _Video_Get67fcf4:PROC
EXTERN _Wnd_FrameExtra:PROC
EXTERN __imp__GetSystemMetrics@4:PROC
EXTERN __imp__SetWindowLongA@12:PROC
EXTERN _sprintf:PROC

PUBLIC $L_5d9aec, $L_5d9b22, $L_5d9b54, $L_5d9b68, $L_5d9b91, $L_5d9be0, $L_5d9c11, $L_5d9e40
PUBLIC $L_5dc466, $L_5dd8c0, $L_5dd8f6, $L_5dd916, $L_5dd927, $L_5dd936, $L_5dd95b, $L_5dd9a0
PUBLIC $L_5dda8f, $L_5dda9e, $L_5ddad0, $L_5ddae4, $L_5ddaf3, $L_5ddb1e, $L_5e3120, $L_5e3145

_TEXT SEGMENT
$L_5d9ab0 PROC

            mov EAX,DWORD PTR [ESP+8]
            sub ESP,240
            push EBX
            push EBP
            xor EBP,EBP
            push ESI
            push EDI
            sub EAX,EBP
            mov EDI,ECX
            je $L_5d9bf0

            dec EAX
            je $L_5d9b3d

            dec EAX
            jne $L_5d9c11

            mov ECX,OFFSET (_KingBss+606208)
            call @NameHash_ResolvePending@8

            cmp EAX,EBP
            je $L_5d9aec

            push EBP
            push EAX
            call _Sys_Fatal
$L_5d9ab0 ENDP
$L_5d9ae9 PROC

            add ESP,8
$L_5d9aec::

            mov EAX,DWORD PTR [EDI]
            mov ECX,EDI
            call DWORD PTR [EAX+112]

            mov ESI,EAX
            cmp ESI,EBP
            mov EBP,DWORD PTR [ESP+268]
            je $L_5d9b22

            mov EBX,DWORD PTR [ESP+260]
            mov ECX,ESI
            push EBX
            call @Resources_AddTable@12

            mov EAX,DWORD PTR [EBP+12]
            mov EDX,DWORD PTR [EDI]
            push EAX
            mov ECX,EDI
            call DWORD PTR [EDX+92]

            push EBX
            mov ECX,ESI
            call @Resources_Unload@12
$L_5d9b22::

            mov EAX,DWORD PTR [EBP+12]
            mov EDX,DWORD PTR [EDI]
            push EAX
            mov ECX,EDI
            call DWORD PTR [EDX+80]

            mov EAX,DWORD PTR [EBP+12]
            pop EDI
            pop ESI
            pop EBP
            pop EBX
            add ESP,240
            ret 12
$L_5d9ae9 ENDP
$L_5d9b3d PROC

            mov ESI,DWORD PTR [ESP+268]
            mov ECX,DWORD PTR [ESI+424]
            mov EAX,DWORD PTR [ESI+428]
            cmp ECX,EAX
            je $L_5d9b91
$L_5d9b54::

            mov EDX,DWORD PTR [ESI+428]
            mov ECX,DWORD PTR [ESI+EDX*4+24]
            cmp ECX,EBP
            je $L_5d9b68

            mov EAX,DWORD PTR [ECX]
            push 1
            call DWORD PTR [EAX]
$L_5d9b68::

            mov ECX,DWORD PTR [ESI+428]
            mov DWORD PTR [ESI+ECX*4+24],EBP
            mov EAX,DWORD PTR [ESI+428]
            inc EAX
            mov ECX,100
            cdq 
            idiv ECX
            mov EAX,DWORD PTR [ESI+424]
            cmp EAX,EDX
            mov DWORD PTR [ESI+428],EDX
            jne $L_5d9b54
$L_5d9b91::

            mov EBX,DWORD PTR [ESP+260]
            mov EDX,DWORD PTR [EDI]
            lea EAX,DWORD PTR [ESP+96]
            lea ECX,DWORD PTR [ESP+16]
            push EAX
            lea EAX,DWORD PTR [ESP+180]
            push ECX
            push EAX
            push EBX
            mov ECX,EDI
            call DWORD PTR [EDX+88]

            lea ECX,DWORD PTR [ESP+96]
            lea EDX,DWORD PTR [ESP+16]
            push ECX
            lea EAX,DWORD PTR [ESP+180]
            push EDX
            push EAX
            mov ECX,EDI
            call @Module_Load@20

            cmp EAX,EBP
            mov DWORD PTR [ESI+12],EAX
            jne $L_5d9be0

            push EBX
            push OFFSET (_KingData+93184)
            call _Sys_Fatal
$L_5d9b3d ENDP
$L_5d9bdd PROC

            add ESP,8
$L_5d9be0::

            mov EAX,DWORD PTR [ESI+12]
            pop EDI
            pop ESI
            pop EBP
            pop EBX
            add ESP,240
            ret 12
$L_5d9bdd ENDP
$L_5d9bf0 PROC

            mov EAX,DWORD PTR [ESP+268]
            push EAX
            push OFFSET _Room_QueueDelete
            mov DWORD PTR [EAX+20],EDI
            mov ECX,DWORD PTR [EDI+488]
            mov EDX,DWORD PTR [ECX+76]
            mov ECX,DWORD PTR [EDX+36]
            call @Room_Each@16
$L_5d9c11::

            pop EDI
            pop ESI
            pop EBP
            xor EAX,EAX
            pop EBX
            add ESP,240
            ret 12
$L_5d9bf0 ENDP
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
$L_5d9e40::
            mov EAX,DWORD PTR [ESP+4]
            mov ECX,DWORD PTR [ECX+20976]
            push EAX
            call @EventRing_Push@12
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
$L_5d9e50 PROC

            ret 4
$L_5d9e50 ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; the ring push that shares the original's code stays in assembly
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5dc430 PROC

            mov EAX,DWORD PTR [ESP+8]
            push ESI
            mov ESI,ECX
            push EAX
            mov ECX,DWORD PTR [ESP+12]
            push ECX
            mov ECX,ESI
            call @App_Path@12

            push EAX
            mov ECX,ESI
            call @Module_LoadImage@16

            mov ECX,OFFSET (_KingBss+606208)
            call @NameHash_ResolvePending@8

            test EAX,EAX
            pop ESI
            je $L_5dc466

            push 0
            push EAX
            call _Sys_Fatal
$L_5dc430 ENDP
$L_5dc463 PROC

            add ESP,8
$L_5dc466::

            ret 8
$L_5dc463 ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
FUN_5dd2d0 PROC

            mov DWORD PTR [ECX],OFFSET (_KingRdata+29668)
            jmp @FrameWnd_Destruct@4
FUN_5dd2d0 ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5dd380 PROC

            mov EAX,DWORD PTR [ESP+8]
            mov EDX,DWORD PTR [ESP+4]
            push EAX
            push EDX
            call @FrameWnd_OnSysCommand@16

            ret 8
$L_5dd380 ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; main loop: src\engine\main_loop.cpp, nmake ASM_MAIN_LOOP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
$L_5dd700 PROC

            jmp @Wnd_Default@4
$L_5dd700 ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5dd8c0::
            sub ESP,8
            push EBX
            push EBP
            mov EBP,DWORD PTR [ESP+32]
            push ESI
            mov ESI,ECX
            push EDI
            mov DWORD PTR [ESP+16],ESI
            mov DWORD PTR [ESP+20],0
            mov EAX,DWORD PTR [ESI+268]
            and EAX,1024
            je $L_5dd8f6
            test EBP,1024
            jne $L_5dd8f6
            mov DWORD PTR [ESP+20],1
$L_5dd8f6::
            test EAX,EAX
            jne $L_5dd916
            test EBP,1024
            je $L_5dd916
$L_5dd902 PROC

            mov EAX,DWORD PTR [ESI+388]
            push 2415919104
            push -16
            push EAX
            call DWORD PTR __imp__SetWindowLongA@12
$L_5dd902 ENDP
$L_5dd916::
            cmp DWORD PTR [ESP+36],-1
            jne $L_5dd927
            mov ECX,DWORD PTR [ESI+264]
            mov DWORD PTR [ESP+36],ECX
$L_5dd927::
            cmp EBP,-1
            jne $L_5dd936
            mov EBP,DWORD PTR [ESI+268]
            mov DWORD PTR [ESP+40],EBP
$L_5dd936::
            mov EDX,DWORD PTR [ESP+36]
            mov EBX,DWORD PTR [ESP+32]
            mov EDI,DWORD PTR [ESP+28]
            push EBP
            push EDX
            push EBX
            push EDI
            mov ECX,ESI
            call $L_5ddad0
$L_5dd94d PROC

            test EAX,EAX
            jne $L_5dd95b

            pop EDI
            pop ESI
            pop EBP
            pop EBX
            add ESP,8
            ret 16
$L_5dd95b::

            mov EAX,DWORD PTR [(_KingBss+618288)]
            test BYTE PTR [EAX+292],2
            je $L_5dd9a0

            test EBP,1024
            jne $L_5dd9a0

            push 2
            call _Wnd_FrameExtra

            push 1
            mov EBP,EAX
            call _Wnd_FrameExtra

            add EBP,EAX
            push 4
            sub EDI,EBP
            call _Wnd_FrameExtra

            push 3
            mov EBP,EAX
            call _Wnd_FrameExtra

            add EBP,EAX
            add ESP,16
            sub EBX,EBP
            mov EBP,DWORD PTR [ESP+40]
$L_5dd9a0::

            mov ECX,DWORD PTR [ESP+36]
            push EBP
            push ECX
            push EBX
            push EDI
            mov ECX,ESI
            call @CGameApp_SetVideoMode@24

            mov EAX,DWORD PTR [ESI+268]
            test AH,4
            jne $L_5dda9e

            call _Video_Get67fcf0

            mov ESI,EAX
            imul ESI,EDI
            call _Video_Get67fcf4

            mov EDI,EAX
            push 2
            imul EDI,EBX
            call _Wnd_FrameExtra

            push 1
            mov EBX,EAX
            call _Wnd_FrameExtra

            add EAX,ESI
            push 4
            add EBX,EAX
            call _Wnd_FrameExtra

            push 3
            mov ESI,EAX
            call _Wnd_FrameExtra

            add ESP,16
            add EAX,EDI
            mov EDI,DWORD PTR __imp__GetSystemMetrics@4
            push 0
            lea EBP,DWORD PTR [EAX+ESI*1]
            call EDI

            sub EAX,EBX
            push 1
            cdq 
            sub EAX,EDX
            mov ESI,EAX
            sar ESI,1
            call EDI

            sub EAX,EBP
            push 64
            cdq 
            sub EAX,EDX
            mov EDX,DWORD PTR [(_KingBss+618288)]
            mov EDI,EAX
            push EBP
            mov EAX,DWORD PTR [EDX+292]
            push EBX
            mov ECX,EAX
            and AL,254
            sar EDI,1
            mov DWORD PTR [EDX+292],EAX
            mov EDX,DWORD PTR [ESP+28]
            and ECX,1
            push EDI
            mov DWORD PTR [ESP+52],ECX
            mov ECX,DWORD PTR [EDX+384]
            push ESI
            push OFFSET (_KingBss+668216)
            call @Wnd_SetWindowPos@32

            mov EAX,DWORD PTR [ESP+20]
            test EAX,EAX
            je $L_5dda8f

            mov EAX,DWORD PTR [ESP+16]
            push 13565952
            push -16
            mov ECX,DWORD PTR [EAX+388]
            push ECX
            call DWORD PTR __imp__SetWindowLongA@12

            mov EDX,DWORD PTR [ESP+16]
            push 96
            push EBP
            push EBX
            mov ECX,DWORD PTR [EDX+384]
            push EDI
            push ESI
            push OFFSET (_KingBss+668216)
            call @Wnd_SetWindowPos@32
$L_5dda8f::

            mov EAX,DWORD PTR [(_KingBss+618288)]
            mov ECX,DWORD PTR [ESP+36]
            or DWORD PTR [EAX+292],ECX
$L_5dda9e::

            call _Mode_TextureLevel

            xor EDX,EDX
            cmp EAX,2
            sete DL
            push EDX
            call _Set71e7d4

            add ESP,4
            mov EAX,1
            pop EDI
            pop ESI
            pop EBP
            pop EBX
            add ESP,8
            ret 16
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5ddad0::
            push EBX
            mov EBX,DWORD PTR [ESP+16]
            push ESI
            cmp EBX,-1
            push EDI
            mov ESI,ECX
            jne $L_5ddae4
            mov EBX,DWORD PTR [ESI+264]
$L_5ddae4::
            mov EDI,DWORD PTR [ESP+28]
            cmp EDI,-1
            jne $L_5ddaf3
            mov EDI,DWORD PTR [ESI+268]
$L_5ddaf3::
            mov ECX,DWORD PTR [ESP+20]
            mov EDX,DWORD PTR [ESP+16]
            mov EAX,EDI
            push 0
            push 0
            and EAX,20224
            push 0
            push EAX
            push EBX
            push ECX
            push EDX
            call $L_5d7c20

            add ESP,28
            test EAX,EAX
            jne $L_5ddb1e

            pop EDI
            pop ESI
            pop EBX
            ret 16
$L_5ddb1e::

            mov DWORD PTR [ESI+268],EDI
            mov DWORD PTR [ESI+264],EBX
            pop EDI
            pop ESI
            mov EAX,1
            pop EBX
            ret 16
$L_5dd94d ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; main loop: src\engine\main_loop.cpp, nmake ASM_MAIN_LOOP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; main loop: src\engine\main_loop.cpp, nmake ASM_MAIN_LOOP=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5e30f0 PROC

            mov EAX,DWORD PTR [ESP+4]
            sub ESP,64
            lea ECX,DWORD PTR [ESP]
            push EAX
            push OFFSET (_KingData+94568)
            push ECX
            call _sprintf

            lea EDX,DWORD PTR [ESP+12]
            push EDX
            push OFFSET (_KingData+94552)
            call _Sys_Fatal
$L_5e30f0 ENDP
$L_5e3116 PROC

            xor EAX,EAX
            add ESP,84
            ret 
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5e3120::

            mov EAX,DWORD PTR [(_KingBss+618288)]
            mov ECX,DWORD PTR [EAX+344]
            test ECX,ECX
            je $L_5e3145

            push 0
            call _Sound_SetVolume

            add ESP,4
            fstp ST(0)
            call _Sound_Stop

            call _Sound_Close
$L_5e3145::

            jmp DWORD PTR [(_KingBss+618368)]

            nop
            nop
            nop
            nop
            nop
$L_5e3116 ENDP
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; CGameApp: src\engine\game_app.cpp, nmake ASM_GAME_APP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
$L_5e557f PROC

            int 3
$L_5e557f ENDP
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/framework.cpp, nmake ASM_FRAMEWORK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/clusters.cpp, nmake ASM_CLUSTERS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
_TEXT ENDS
END
