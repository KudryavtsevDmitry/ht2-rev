; asm_carat.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_64682c:NEAR
EXTERN @DMat_Copy@12:PROC
EXTERN @DMat_Determinant@8:PROC
EXTERN @DMat_Zero@8:PROC
EXTERN @DpMessage_Construct@8:PROC
EXTERN @DpMessage_Set@16:PROC
EXTERN @Font_TextWidth@12:PROC
EXTERN @Gate_602bd0@8:PROC
EXTERN @Gate_Set24@8:PROC
EXTERN @List_DrawAt@12:PROC
EXTERN @MsgBuf_Destroy@8:PROC
EXTERN @Msg_NextType@12:PROC
EXTERN @Msg_SkipRecord@8:PROC
EXTERN @Shell_ShowWithMusic@12:PROC
EXTERN @Stream_Construct@8:PROC
EXTERN @Stream_ReadBlob@20:PROC
EXTERN @Window_Show@12:PROC
EXTERN UnusedTrap:NEAR
EXTERN _Bitmap_Draw:PROC
EXTERN _CarList_Draw:PROC
EXTERN _DMat_Inverse:PROC
EXTERN _Ini_WriteInt:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _Net_Receive:PROC
EXTERN _Rec128_Combine:PROC
EXTERN _Scores_Draw:PROC
EXTERN _Shell_EnterWorld:PROC
EXTERN _Shell_LeaveGameScreens:PROC
EXTERN _Shell_LeaveRace:PROC
EXTERN _Shell_StartGame:PROC
EXTERN _Table71e858_Get:PROC
EXTERN _Text_Draw:PROC
EXTERN _Tsc_Now:PROC
EXTERN _rand:PROC

PUBLIC $L_507ce0, $L_50bb10, $L_50bb50, $L_50bb59, $L_50bb5e, $L_51f7c5, $L_51f7fc, $L_51f81c
PUBLIC $L_51f89a, $L_51f8ee, $L_51f917, $L_51f91f, $L_51f92b, $L_51f931, $L_51f9c8, $L_51f9d9
PUBLIC $L_51fa17, $L_51fa28, $L_51fa53, $L_51fa7e, $L_51fadf, $L_51fae6, $L_51faf3, $L_51fb13
PUBLIC $L_51fb38, $L_51fb41, $L_51fb50, $L_51fb60, $L_51fb6b, $L_51fb95, $L_51fba6, $L_51fbe8
PUBLIC $L_51fc13, $L_51fc3e, $L_51fca0, $L_51fca7, $L_51fcb4, $L_51fccb, $L_51fcee, $L_51fcf9
PUBLIC $L_51fcfe, $L_51fd12, $L_5231af, $L_523201

_TEXT SEGMENT
$L_507ccf PROC

            pop EDI
            pop ESI
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
$L_507ce0::

            push ESI
            lea ESI,DWORD PTR [ECX+12]
            mov ECX,ESI
            call UnusedTrap

            mov ECX,ESI
            call UnusedTrap

            pop ESI
            ret 
$L_507ccf ENDP
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
$L_50bb00 PROC

            jmp $L_50bb10
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_50bb10::

            push EBP
            mov EBP,ESP
            and ESP,4294967288
            sub ESP,648
            push EBX
            push EBP
            push ESI
            push EDI
            lea ECX,DWORD PTR [ESP+24]
            mov DWORD PTR [(_KingBss+277356)],0
            call @DMat_Zero@8

            lea ECX,DWORD PTR [ESP+152]
            call @DMat_Zero@8

            lea ECX,DWORD PTR [ESP+280]
            call @DMat_Zero@8

            mov EBP,1000
$L_50bb50::

            lea ESI,DWORD PTR [ESP+24]
            mov EBX,4
$L_50bb59::

            mov EDI,4
$L_50bb5e::

            call _rand

            mov DWORD PTR [ESP+20],EAX
            add ESI,8
            fild DWORD PTR [ESP+20]
            dec EDI
            fsub QWORD PTR [(_KingRdata+11120)]
            fmul QWORD PTR [(_KingRdata+11112)]
            fstp QWORD PTR [ESI-8]
            jne $L_50bb5e

            dec EBX
            jne $L_50bb59

            lea EAX,DWORD PTR [ESP+24]
            lea ECX,DWORD PTR [ESP+408]
            push EAX
            push ECX
            call _DMat_Inverse

            add ESP,8
            lea ECX,DWORD PTR [ESP+152]
            push EAX
            call @DMat_Copy@12

            lea EDX,DWORD PTR [ESP+24]
            lea EAX,DWORD PTR [ESP+152]
            push EDX
            lea ECX,DWORD PTR [ESP+540]
            push EAX
            push ECX
            call _Rec128_Combine

            add ESP,12
            lea ECX,DWORD PTR [ESP+280]
            push EAX
            call @DMat_Copy@12

            lea ECX,DWORD PTR [ESP+280]
            call @DMat_Determinant@8

            dec EBP
            fstp ST(0)
            jne $L_50bb50

            pop EDI
            pop ESI
            pop EBP
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_50bb00 ENDP
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
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
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
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
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
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
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
$L_51f760 PROC

            mov EAX,DWORD PTR FS:[0]
            push -1
            push OFFSET $L_64682c
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,620
            xor EAX,EAX
            xor EDX,EDX
            push EBX
            push EBP
            push ESI
            mov ESI,ECX
            push EDI
            mov ECX,DWORD PTR [ESI+304]
            mov EBX,DWORD PTR [ESI-52]
            mov AX,WORD PTR [ECX+20]
            mov DX,WORD PTR [EBX]
            sub EAX,EDX
            cdq 
            sub EAX,EDX
            mov EDI,EAX
            xor EAX,EAX
            mov AX,WORD PTR [EBX+2]
            mov EDX,EAX
            xor EAX,EAX
            mov AX,WORD PTR [ECX+22]
            sub EAX,EDX
            cdq 
            sub EAX,EDX
            mov EBP,EAX
            mov EAX,DWORD PTR [ESI+340]
            sar EDI,1
            sar EBP,1
            cmp EAX,2
            jle $L_51f7c5

            cmp EAX,5
            jne $L_51f81c
$L_51f7c5::

            mov EAX,DWORD PTR [(_KingBss+279628)]
            mov ECX,DWORD PTR [EAX+1316]
            test ECX,ECX
            jne $L_51f81c

            mov EBX,DWORD PTR [ESP+652]
            mov ECX,ESI
            push EBX
            call @List_DrawAt@12

            mov ESI,DWORD PTR [ESI+340]
            cmp ESI,1
            je $L_51f7fc

            cmp ESI,2
            je $L_51f7fc

            cmp ESI,5
            jne $L_51fd12
$L_51f7fc::

            mov ECX,440
            mov EDX,622
            sub ECX,EBP
            push 3
            sub EDX,EDI
            push ECX
            push EDX
            push EBX
            call _CarList_Draw

            add ESP,16
            jmp $L_51fd12
$L_51f81c::

            lea ECX,DWORD PTR [ESP+24]
            call @Stream_Construct@8

            xor EAX,EAX
            mov DWORD PTR [ESP+644],EAX
            mov DWORD PTR [ESP+36],EAX
            call _Tsc_Now

            fstp QWORD PTR [ESP+40]
            mov EAX,DWORD PTR [ESI+340]
            mov EBX,1
            test EAX,EAX
            mov DWORD PTR [ESP+644],EBX
            jne $L_51f8ee

            mov EAX,DWORD PTR [ESI+304]
            neg EBP
            push EAX
            mov EAX,DWORD PTR [ESI-52]
            neg EDI
            push EBP
            push EDI
            push EAX
            call _Bitmap_Draw

            fld QWORD PTR [ESI+344]
            fadd QWORD PTR [(_KingRdata+8400)]
            add ESP,16
            fstp QWORD PTR [ESP+16]
            call _Tsc_Now

            fcomp QWORD PTR [ESP+16]
            fnstsw AX
            test AH,65
            jne $L_51f89a

            mov EDX,DWORD PTR [ESI]
            push 163
            mov ECX,ESI
            call DWORD PTR [EDX+60]
$L_51f89a::

            push 74
            call _Table71e858_Get

            mov ECX,DWORD PTR [(_KingBss+279812)]
            add ESP,4
            mov ESI,EAX
            push ESI
            call @Font_TextWidth@12

            mov EDI,DWORD PTR [ESP+652]
            xor ECX,ECX
            push 0
            push 5
            mov CX,WORD PTR [EDI]
            push ESI
            mov EDX,ECX
            xor ECX,ECX
            mov CX,WORD PTR [EDI+2]
            sub EDX,EAX
            mov EAX,EDX
            sub ECX,30
            cdq 
            sub EAX,EDX
            mov EDX,DWORD PTR [(_KingBss+279812)]
            sar EAX,1
            push EDX
            push ECX
            push EAX
            push EDI
            call _Text_Draw

            add ESP,28
            jmp $L_51fcfe
$L_51f8ee::

            mov EAX,DWORD PTR [(_KingData+63324)]
            cmp EAX,2
            je $L_51fb6b

            mov ECX,DWORD PTR [(_KingBss+279672)]
            cmp DWORD PTR [ECX+384],2
            jge $L_51f91f

            cmp EAX,EBX
            jne $L_51f917

            mov EAX,DWORD PTR [ESI+316]
            jmp $L_51f931
$L_51f917::

            mov EAX,DWORD PTR [ESI+320]
            jmp $L_51f931
$L_51f91f::

            cmp EAX,EBX
            jne $L_51f92b

            mov EAX,DWORD PTR [ESI+324]
            jmp $L_51f931
$L_51f92b::

            mov EAX,DWORD PTR [ESI+328]
$L_51f931::

            mov ECX,DWORD PTR [ESI-52]
            push EAX
            mov EDX,EBP
            mov EAX,EDI
            neg EDX
            neg EAX
            push EDX
            push EAX
            push ECX
            call _Bitmap_Draw

            mov EDX,470
            mov EAX,622
            sub EDX,EBP
            sub EAX,EDI
            mov EDI,DWORD PTR [ESP+668]
            push EDX
            push EAX
            push EDI
            call _Scores_Draw

            push 74
            call _Table71e858_Get

            mov ECX,DWORD PTR [(_KingBss+279812)]
            add ESP,32
            mov EBP,EAX
            push EBP
            call @Font_TextWidth@12

            xor ECX,ECX
            push 0
            mov CX,WORD PTR [EDI]
            push 5
            mov EDX,ECX
            xor ECX,ECX
            mov CX,WORD PTR [EDI+2]
            sub EDX,EAX
            mov EAX,EDX
            sub ECX,30
            cdq 
            sub EAX,EDX
            mov EDX,DWORD PTR [(_KingBss+279812)]
            push EBP
            push EDX
            sar EAX,1
            push ECX
            push EAX
            push EDI
            call _Text_Draw

            mov EAX,DWORD PTR [(_KingBss+279628)]
            add ESP,28
            cmp DWORD PTR [EAX+1316],EBX
            jne $L_51fa28

            lea ECX,DWORD PTR [ESP+24]
            push ECX
            call _Net_Receive

            add ESP,4
            test EAX,EAX
            jne $L_51f9d9
$L_51f9c8::

            lea EDX,DWORD PTR [ESP+24]
            push EDX
            call _Net_Receive

            add ESP,4
            test EAX,EAX
            je $L_51f9c8
$L_51f9d9::

            fld QWORD PTR [ESI+344]
            fadd QWORD PTR [(_KingRdata+8400)]
            fstp QWORD PTR [ESP+16]
            call _Tsc_Now

            fcomp QWORD PTR [ESP+16]
            fnstsw AX
            test AH,65
            jne $L_51fcfe

            cmp DWORD PTR [ESI+340],3
            jne $L_51fa17

            mov EAX,DWORD PTR [ESI]
            push 168
            mov ECX,ESI
            call DWORD PTR [EAX+60]

            jmp $L_51fcfe
$L_51fa17::

            mov EDX,DWORD PTR [ESI]
            push 163
            mov ECX,ESI
            call DWORD PTR [EDX+60]

            jmp $L_51fcfe
$L_51fa28::

            lea ECX,DWORD PTR [ESP+64]
            call @DpMessage_Construct@8

            lea EAX,DWORD PTR [ESP+24]
            mov BYTE PTR [ESP+644],2
            push EAX
            call _Net_Receive

            add ESP,4
            test EAX,EAX
            jne $L_51fb50

            mov EBP,120
$L_51fa53::

            cmp DWORD PTR [ESP+24],4
            jne $L_51fb13

            mov ECX,DWORD PTR [ESP+32]
            mov EDX,DWORD PTR [ESP+28]
            push ECX
            push EDX
            lea ECX,DWORD PTR [ESP+72]
            call @DpMessage_Set@16

            lea ECX,DWORD PTR [ESP+64]
            call @Gate_Set24@8

            test EAX,EAX
            jne $L_51faf3
$L_51fa7e::

            lea EAX,DWORD PTR [ESP+48]
            lea ECX,DWORD PTR [ESP+64]
            push EAX
            call @Msg_NextType@12

            test EAX,EAX
            jne $L_51faf3

            cmp DWORD PTR [ESP+48],EBP
            jne $L_51fae6

            lea ECX,DWORD PTR [ESP+120]
            lea EDX,DWORD PTR [ESP+124]
            push ECX
            lea EAX,DWORD PTR [ESP+64]
            push EDX
            push EAX
            lea ECX,DWORD PTR [ESP+76]
            call @Stream_ReadBlob@20

            test EAX,EAX
            jne $L_51fae6

            mov EAX,DWORD PTR [ESP+60]
            cmp EAX,954
            jne $L_51fadf

            mov ECX,DWORD PTR [(_KingBss+50304)]
            mov EDX,DWORD PTR [ECX]
            call DWORD PTR [EDX+36]

            mov EAX,DWORD PTR [ESI-64]
            lea ECX,DWORD PTR [ESI-64]
            call DWORD PTR [EAX+32]

            mov ECX,DWORD PTR [(_KingBss+50304)]
            mov DWORD PTR [ECX+3024],EBX
            jmp $L_51fae6
$L_51fadf::

            cmp EAX,952
            je $L_51fb38
$L_51fae6::

            lea ECX,DWORD PTR [ESP+64]
            call @Msg_SkipRecord@8

            test EAX,EAX
            je $L_51fa7e
$L_51faf3::

            fld QWORD PTR [ESI+344]
            fadd QWORD PTR [(_KingRdata+8408)]
            fstp QWORD PTR [ESP+16]
            call _Tsc_Now

            fcomp QWORD PTR [ESP+16]
            fnstsw AX
            test AH,65
            je $L_51fb60
$L_51fb13::

            lea EDX,DWORD PTR [ESP+24]
            push EDX
            call _Net_Receive

            add ESP,4
            test EAX,EAX
            je $L_51fa53

            mov BYTE PTR [ESP+644],BL
            lea ECX,DWORD PTR [ESP+64]
            jmp $L_51fcf9
$L_51fb38::

            mov EAX,DWORD PTR [ESI-64]
            lea ECX,DWORD PTR [ESI-64]
            call DWORD PTR [EAX+32]
$L_51fb41::

            mov DWORD PTR [(_KingBss+429064)],0
            call _Shell_LeaveRace
$L_51fb50::

            mov BYTE PTR [ESP+644],BL
            lea ECX,DWORD PTR [ESP+64]
            jmp $L_51fcf9
$L_51fb60::

            mov EDX,DWORD PTR [ESI-64]
            lea ECX,DWORD PTR [ESI-64]
            call DWORD PTR [EDX+32]

            jmp $L_51fb41
$L_51fb6b::

            lea EDI,DWORD PTR [ESI-64]
            push 0
            mov ECX,EDI
            call @Window_Show@12

            mov EAX,DWORD PTR [(_KingBss+279628)]
            cmp DWORD PTR [EAX+1316],EBX
            jne $L_51fbe8

            lea ECX,DWORD PTR [ESP+24]
            push ECX
            call _Net_Receive

            add ESP,4
            test EAX,EAX
            jne $L_51fba6
$L_51fb95::

            lea EDX,DWORD PTR [ESP+24]
            push EDX
            call _Net_Receive

            add ESP,4
            test EAX,EAX
            je $L_51fb95
$L_51fba6::

            fld QWORD PTR [ESI+344]
            fadd QWORD PTR [(_KingRdata+2120)]
            fstp QWORD PTR [ESP+16]
            call _Tsc_Now

            fcomp QWORD PTR [ESP+16]
            fnstsw AX
            test AH,65
            jne $L_51fcfe

            cmp DWORD PTR [ESI+340],3
            jne $L_51fa17

            mov EAX,DWORD PTR [ESI]
            push 168
            mov ECX,ESI
            call DWORD PTR [EAX+60]

            jmp $L_51fcfe
$L_51fbe8::

            lea ECX,DWORD PTR [ESP+92]
            call @DpMessage_Construct@8

            lea EAX,DWORD PTR [ESP+24]
            mov BYTE PTR [ESP+644],3
            push EAX
            call _Net_Receive

            add ESP,4
            test EAX,EAX
            jne $L_51fcee

            mov EBP,120
$L_51fc13::

            cmp DWORD PTR [ESP+24],4
            jne $L_51fcb4

            mov ECX,DWORD PTR [ESP+32]
            mov EDX,DWORD PTR [ESP+28]
            push ECX
            push EDX
            lea ECX,DWORD PTR [ESP+100]
            call @DpMessage_Set@16

            lea ECX,DWORD PTR [ESP+92]
            call @Gate_Set24@8

            test EAX,EAX
            jne $L_51fcb4
$L_51fc3e::

            lea EAX,DWORD PTR [ESP+52]
            lea ECX,DWORD PTR [ESP+92]
            push EAX
            call @Msg_NextType@12

            test EAX,EAX
            jne $L_51fcb4

            cmp DWORD PTR [ESP+52],EBP
            jne $L_51fca7

            lea ECX,DWORD PTR [ESP+16]
            lea EDX,DWORD PTR [ESP+380]
            push ECX
            lea EAX,DWORD PTR [ESP+60]
            push EDX
            push EAX
            lea ECX,DWORD PTR [ESP+104]
            call @Stream_ReadBlob@20

            test EAX,EAX
            jne $L_51fca7

            mov EAX,DWORD PTR [ESP+56]
            cmp EAX,954
            jne $L_51fca0

            mov ECX,DWORD PTR [(_KingBss+50304)]
            mov EDX,DWORD PTR [ECX]
            call DWORD PTR [EDX+36]

            mov EAX,DWORD PTR [EDI]
            mov ECX,EDI
            call DWORD PTR [EAX+32]

            mov ECX,DWORD PTR [(_KingBss+50304)]
            mov DWORD PTR [ECX+3024],EBX
            jmp $L_51fca7
$L_51fca0::

            cmp EAX,952
            je $L_51fccb
$L_51fca7::

            lea ECX,DWORD PTR [ESP+92]
            call @Msg_SkipRecord@8

            test EAX,EAX
            je $L_51fc3e
$L_51fcb4::

            lea EDX,DWORD PTR [ESP+24]
            push EDX
            call _Net_Receive

            add ESP,4
            test EAX,EAX
            je $L_51fc13

            jmp $L_51fcee
$L_51fccb::

            mov EAX,DWORD PTR [EDI]
            mov ECX,EDI
            call DWORD PTR [EAX+32]

            mov DWORD PTR [(_KingBss+429064)],0
            call _Shell_LeaveRace
$L_51f760 ENDP
$L_51fce1 PROC

            mov ECX,DWORD PTR [(_KingBss+279712)]
            push 12
            call @Shell_ShowWithMusic@12
$L_51fcee::

            mov BYTE PTR [ESP+644],BL
            lea ECX,DWORD PTR [ESP+92]
$L_51fcf9::

            call @MsgBuf_Destroy@8
$L_51fcfe::

            lea ECX,DWORD PTR [ESP+24]
            mov DWORD PTR [ESP+644],4294967295
            call @Gate_602bd0@8
$L_51fd12::

            mov ECX,DWORD PTR [ESP+636]
            pop EDI
            pop ESI
            pop EBP
            pop EBX
            mov DWORD PTR FS:[0],ECX
            add ESP,632
            ret 4
$L_51fce1 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_523170 PROC

            mov EAX,DWORD PTR [ESP+4]
            push ESI
            cmp EAX,160
            mov ESI,ECX
            jne $L_5231af

            mov EAX,DWORD PTR [(_KingBss+49224)]
            push EAX
            push 0
            push OFFSET (_KingData+42904)
            push OFFSET (_KingData+27032)
            call _Ini_WriteInt

            mov EDX,DWORD PTR [ESI-64]
            lea ECX,DWORD PTR [ESI-64]
            add ESP,16
            call DWORD PTR [EDX+32]

            push 0
            call _Shell_EnterWorld

            add ESP,4
            pop ESI
            ret 4
$L_5231af::

            cmp EAX,161
            jne $L_523201

            mov EAX,DWORD PTR [ESI-64]
            lea ECX,DWORD PTR [ESI-64]
            call DWORD PTR [EAX+32]

            mov EAX,DWORD PTR [(_KingBss+279632)]
            mov ECX,DWORD PTR [EAX+408]
            and CL,63
            or ECX,48
            mov DWORD PTR [EAX+408],ECX
            mov EDX,DWORD PTR [(_KingBss+279628)]
            mov ESI,DWORD PTR [EDX+1300]
            call _Shell_LeaveGameScreens
$L_523170 ENDP
$L_5231e7 PROC

            mov EAX,DWORD PTR [(_KingBss+279628)]
            push 0
            push OFFSET (_KingBss+427256)
            mov DWORD PTR [EAX+1300],ESI
            call _Shell_StartGame

            add ESP,8
$L_523201::

            pop ESI
            ret 4
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H

            nop
            nop
            nop
            nop
$L_5231e7 ENDP
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
; game, menus and screens: src\truck\game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
ALIGN 16
_TEXT ENDS
END
