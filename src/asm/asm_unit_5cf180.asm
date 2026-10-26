; asm_unit_5cf180.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_5fef53:NEAR
EXTERN @MappedFile_Close@4:PROC
EXTERN FUN_6303ac:NEAR
EXTERN _Dd_EnumDevice@16:PROC
EXTERN _Dd_ErrorText:PROC
EXTERN _Dd_FreeLocalVideoMemory:PROC
EXTERN _Dd_MatchMode@8:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _Static5d2060_AtExit:PROC
EXTERN _Static71e808_Construct:PROC
EXTERN _Sys_Log:PROC

PUBLIC $L_5d2020, $L_5d7b90, $L_5d7bc2, $L_5d7bd1, $L_5d7bdd, $L_5d7bf2, $L_5d7bfe, $L_5d7c0c
PUBLIC $L_5d7c20, $L_5d7c38, $L_5d7c59, $L_5d7c68, $L_5d7c78, $L_5d7d0e, $L_5d7d4d, $L_5d7d51
PUBLIC $L_5d7d98, $L_5d7dbb, $L_5d7ddd, $L_5d7dfd, $L_5d7e1e, $L_5d7e36, $L_5d7e48, $L_5d7e5a
PUBLIC $L_5d7e6c, $L_5d7e7e, $L_5d7e90, $L_5d7ea2, $L_5d7f01, $L_5d7f0d, $L_5d7f61, $L_5d7f83
PUBLIC $L_5d7ff8, $L_5d8007, $L_5d802b, $L_5d803d, $L_5d8051, $L_5d8069, $L_5d807b, $L_5d808d
PUBLIC $L_5d809f, $L_5d80b1, $L_5d80c3, $L_5d80d1, $L_5d80dd, $L_5d80ef, $L_5d8107, $L_5d8119
PUBLIC $L_5d812b, $L_5d813d, $L_5d814f, $L_5d8161, $L_5d816f, $L_5d8179

_TEXT SEGMENT
$L_5d2010 PROC

            jmp $L_5d2020
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5d2020::

            jmp $L_5fef53
$L_5d2010 ENDP
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
$L_5d2040 PROC

            call _Static71e808_Construct

            jmp _Static5d2060_AtExit
$L_5d2040 ENDP
ALIGN 16
$L_5d2070 PROC

            mov ECX,OFFSET (_KingBss+606184)
            jmp @MappedFile_Close@4
$L_5d2070 ENDP
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
ALIGN 16
ALIGN 16
; src\engine\core.cpp, nmake ASM_CORE=1 assembles the original
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
$L_5d7b90::
            mov EAX,DWORD PTR [ESP+32]
            mov ECX,DWORD PTR [ESP+28]
            mov EDX,DWORD PTR [ESP+24]
            push ESI
            mov ESI,DWORD PTR [ESP+16]
            push EAX
            mov EAX,DWORD PTR [ESP+24]
            push ECX
            mov ECX,DWORD PTR [ESP+20]
            push EDX
            mov EDX,DWORD PTR [ESP+20]
            push EAX
            push ESI
            push ECX
            push EDX
            call $L_5d7c20
$L_5d7bb9 PROC

            add ESP,28
            test EAX,EAX
            jne $L_5d7bc2

            pop ESI
            ret 
$L_5d7bc2::

            lea ECX,DWORD PTR [ESI-1]
            cmp ECX,3
            ja $L_5d7bfe

            jmp DWORD PTR [ECX*4+$L_5d7c0c]
$L_5d7bd1::

            mov ECX,DWORD PTR [ESP+24]
            pop ESI
            mov DWORD PTR [ECX],1
            ret 
$L_5d7bdd::

            mov EDX,DWORD PTR [(_KingBss+614540)]
            mov ECX,DWORD PTR [ESP+24]
            neg EDX
            sbb EDX,EDX
            pop ESI
            add EDX,3
            mov DWORD PTR [ECX],EDX
            ret 

$L_5d7bf2::
            mov EDX,DWORD PTR [ESP+24]
            pop ESI
            mov DWORD PTR [EDX],6
            ret 
$L_5d7bfe::

            mov ECX,DWORD PTR [ESP+24]
            pop ESI
            mov DWORD PTR [ECX],0
            ret 
          BYTE 08bH
          BYTE 0ffH
$L_5d7c0c::
          DWORD $L_5d7bd1
          DWORD $L_5d7bdd
          DWORD $L_5d7bf2
          DWORD $L_5d7bf2
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5d7c20::
            sub ESP,968
            push EBX
            mov EBX,DWORD PTR [ESP+992]
            push ESI
            xor ESI,ESI
            cmp EBX,ESI
            push EDI
            je $L_5d7c38

            mov DWORD PTR [EBX],ESI
$L_5d7c38::

            mov EAX,DWORD PTR [ESP+996]
            mov ECX,DWORD PTR [ESP+992]
            cmp EAX,ESI
            jne $L_5d7c68

            cmp ECX,1
            je $L_5d7c59

            cmp ECX,2
            je $L_5d7c59

            cmp ECX,4
            jne $L_5d7c68
$L_5d7c59::

            pop EDI
            pop ESI
            mov EAX,1
            pop EBX
            add ESP,968
            ret 
$L_5d7c68::

            test AH,1
            jne $L_5d8179

            mov EDX,DWORD PTR [ESP+984]
$L_5d7c78::

            mov DWORD PTR [ESP+40],ESI
            mov DWORD PTR [ESP+12],EDX
            mov EDX,DWORD PTR [ESP+988]
            test AH,16
            mov DWORD PTR [ESP+36],ESI
            mov DWORD PTR [ESP+52],ESI
            mov DWORD PTR [ESP+56],ESI
            mov DWORD PTR [ESP+60],ESI
            mov DWORD PTR [ESP+64],ESI
            mov DWORD PTR [ESP+76],ESI
            mov DWORD PTR [ESP+72],ESI
            mov DWORD PTR [ESP+68],ESI
            mov DWORD PTR [ESP+132],ESI
            mov DWORD PTR [ESP+112],ESI
            mov DWORD PTR [ESP+44],ESI
            mov DWORD PTR [ESP+92],ESI
            mov DWORD PTR [ESP+48],ESI
            mov DWORD PTR [ESP+16],EDX
            mov DWORD PTR [ESP+20],ECX
            mov DWORD PTR [ESP+24],EAX
            mov DWORD PTR [ESP+216],2147942487
            je $L_5d7f0d

            mov EAX,DWORD PTR [(_KingBss+614596)]
            mov EDI,1
            mov DWORD PTR [(_KingBss+614668)],EAX
            mov EAX,DWORD PTR [(_KingBss+614640)]
            cmp EAX,ESI
            je $L_5d7d0e

            cmp DWORD PTR [(_KingBss+614604)],2
            jne $L_5d7d0e

            mov EDX,DWORD PTR [(_KingBss+614656)]
            mov ECX,DWORD PTR [EAX]
            push 8
            push EDX
            push EAX
            call DWORD PTR [ECX+80]
$L_5d7d0e::

            lea EAX,DWORD PTR [ESP+12]
            push EAX
            push OFFSET _Dd_EnumDevice@16
            call FUN_6303ac

            mov EAX,DWORD PTR [ESP+216]
            cmp EAX,ESI
            je $L_5d7d51

            lea ECX,DWORD PTR [ESP+136]
            push ESI
            push ECX
            push EAX
            call _Dd_ErrorText

            add ESP,8
            push EAX
            call _Sys_Log

            add ESP,8
            cmp EBX,ESI
            je $L_5d7d4d

            mov DWORD PTR [EBX],3
$L_5d7d4d::

            xor EDI,EDI
            jmp $L_5d7dbb
$L_5d7d51::

            mov EAX,380
            lea ECX,DWORD PTR [ESP+600]
            mov DWORD PTR [ESP+220],EAX
            mov DWORD PTR [ESP+600],EAX
            mov EAX,DWORD PTR [ESP+52]
            push ECX
            lea ECX,DWORD PTR [ESP+224]
            mov EDX,DWORD PTR [EAX]
            push ECX
            push EAX
            call DWORD PTR [EDX+44]

            mov ECX,DWORD PTR [ESP+44]
            cmp ECX,2097152
            jge $L_5d7d98

            xor EDI,EDI
            cmp EBX,ESI
            je $L_5d7dbb

            mov DWORD PTR [EBX],5
            jmp $L_5d7dbb
$L_5d7d98::

            mov EAX,DWORD PTR [ESP+1004]
            cmp EAX,ESI
            je $L_5d7dbb

            mov EDX,DWORD PTR [ESP+56]
            mov DWORD PTR [EAX],ECX
            push EDX
            call _Dd_FreeLocalVideoMemory

            mov ECX,DWORD PTR [ESP+1012]
            add ESP,4
            mov DWORD PTR [ECX],EAX
$L_5d7dbb::

            mov EAX,DWORD PTR [ESP+52]
            cmp EAX,ESI
            jne $L_5d7ddd

            push ESI
            push OFFSET (_KingData+92652)
            call _Sys_Log

            add ESP,8
            xor EAX,EAX
            pop EDI
            pop ESI
            pop EBX
            add ESP,968
            ret 
$L_5d7ddd::

            cmp DWORD PTR [ESP+56],ESI
            jne $L_5d7dfd

            push ESI
            push OFFSET (_KingData+92628)
            call _Sys_Log

            add ESP,8
            xor EAX,EAX
            pop EDI
            pop ESI
            pop EBX
            add ESP,968
            ret 
$L_5d7dfd::

            mov ECX,DWORD PTR [(_KingBss+614656)]
            mov EDX,DWORD PTR [EAX]
            push 8
            push ECX
            push EAX
            call DWORD PTR [EDX+80]

            mov EAX,DWORD PTR [ESP+64]
            cmp EAX,ESI
            je $L_5d7e1e

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+64],ESI
$L_5d7e1e::

            mov EAX,DWORD PTR [ESP+132]
            cmp EAX,ESI
            je $L_5d7e36

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+132],ESI
$L_5d7e36::

            mov EAX,DWORD PTR [ESP+76]
            cmp EAX,ESI
            je $L_5d7e48

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+76],ESI
$L_5d7e48::

            mov EAX,DWORD PTR [ESP+72]
            cmp EAX,ESI
            je $L_5d7e5a

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+72],ESI
$L_5d7e5a::

            mov EAX,DWORD PTR [ESP+68]
            cmp EAX,ESI
            je $L_5d7e6c

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+68],ESI
$L_5d7e6c::

            mov EAX,DWORD PTR [ESP+60]
            cmp EAX,ESI
            je $L_5d7e7e

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+60],ESI
$L_5d7e7e::

            mov EAX,DWORD PTR [ESP+56]
            cmp EAX,ESI
            je $L_5d7e90

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+56],ESI
$L_5d7e90::

            mov EAX,DWORD PTR [ESP+52]
            cmp EAX,ESI
            je $L_5d7ea2

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+52],ESI
$L_5d7ea2::

            mov EAX,DWORD PTR [(_KingBss+614604)]
            mov DWORD PTR [ESP+112],ESI
            cmp EAX,2
            mov DWORD PTR [ESP+40],ESI
            mov DWORD PTR [ESP+36],ESI
            mov DWORD PTR [ESP+44],ESI
            mov DWORD PTR [ESP+48],ESI
            mov DWORD PTR [ESP+92],ESI
            jne $L_5d7f01

            mov EAX,DWORD PTR [(_KingBss+614640)]
            cmp EAX,ESI
            je $L_5d7f01

            mov ECX,DWORD PTR [(_KingBss+614656)]
            mov EDX,DWORD PTR [EAX]
            push 91
            push ECX
            push EAX
            call DWORD PTR [EDX+80]

            mov ECX,DWORD PTR [(_KingBss+614512)]
            mov EAX,DWORD PTR [(_KingBss+614636)]
            push ESI
            push ESI
            mov EDX,DWORD PTR [EAX]
            shl ECX,3
            push ECX
            mov ECX,DWORD PTR [(_KingBss+614528)]
            push ECX
            mov ECX,DWORD PTR [(_KingBss+614524)]
            push ECX
            push EAX
            call DWORD PTR [EDX+84]
$L_5d7f01::

            mov EAX,EDI
            pop EDI
            pop ESI
            pop EBX
            add ESP,968
            ret 
$L_5d7f0d::

            test AH,6
            je $L_5d8179

            mov EDI,DWORD PTR [(_KingBss+614636)]
            cmp EDI,ESI
            jne $L_5d7ff8

            mov EDX,DWORD PTR [(_KingBss+614596)]
            lea EAX,DWORD PTR [ESP+12]
            push EAX
            push OFFSET _Dd_EnumDevice@16
            mov DWORD PTR [(_KingBss+614668)],EDX
            call FUN_6303ac

            mov EAX,DWORD PTR [ESP+52]
            cmp EAX,ESI
            jne $L_5d7f61

            push ESI
            push OFFSET (_KingData+92652)
            call _Sys_Log

            add ESP,8
            xor EAX,EAX
            pop EDI
            pop ESI
            pop EBX
            add ESP,968
            ret 
$L_5d7f61::

            mov EDX,DWORD PTR [(_KingBss+614656)]
            mov ECX,DWORD PTR [EAX]
            push 8
            push EDX
            push EAX
            call DWORD PTR [ECX+80]

            mov EAX,DWORD PTR [ESP+216]
            mov EDI,DWORD PTR [ESP+56]
            cmp EAX,ESI
            jne $L_5d80dd
$L_5d7f83::

            cmp EDI,ESI
            je $L_5d80dd

            mov ECX,DWORD PTR [EDI]
            mov EAX,380
            mov DWORD PTR [ESP+220],EAX
            mov DWORD PTR [ESP+600],EAX
            lea EDX,DWORD PTR [ESP+600]
            lea EAX,DWORD PTR [ESP+220]
            push EDX
            push EAX
            push EDI
            call DWORD PTR [ECX+44]

            mov CL,BYTE PTR [ESP+224]
            mov AL,64
            test AL,CL
            jne $L_5d8007

            test BYTE PTR [ESP+604],AL
            jne $L_5d8007

            mov ECX,DWORD PTR [EDI]
            push EDI
            call DWORD PTR [ECX+8]

            push ESI
            push OFFSET (_KingData+92620)
            call _Sys_Log

            add ESP,8
            cmp EBX,ESI
            je $L_5d8179

            pop EDI
            mov DWORD PTR [EBX],2
            pop ESI
            xor EAX,EAX
            pop EBX
            add ESP,968
            ret 
$L_5d7ff8::

            mov EAX,DWORD PTR [EDI]
            push EDI
            call DWORD PTR [EAX+4]

            mov DWORD PTR [ESP+56],EDI
            jmp $L_5d7f83
$L_5d8007::

            mov EDX,DWORD PTR [EDI]
            lea EAX,DWORD PTR [ESP+12]
            push OFFSET _Dd_MatchMode@8
            push EAX
            push ESI
            push ESI
            push EDI
            call DWORD PTR [EDX+32]

            test EAX,EAX
            je $L_5d802b

            push ESI
            push OFFSET (_KingData+92608)
            call _Sys_Log

            add ESP,8
$L_5d802b::

            mov EAX,DWORD PTR [ESP+36]
            cmp EBX,ESI
            je $L_5d803d

            cmp EAX,ESI
            jne $L_5d803d

            mov DWORD PTR [EBX],3
$L_5d803d::

            mov EDI,EAX
            mov EAX,DWORD PTR [ESP+64]
            cmp EAX,ESI
            je $L_5d8051

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+64],ESI
$L_5d8051::

            mov EAX,DWORD PTR [ESP+132]
            cmp EAX,ESI
            je $L_5d8069

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+132],ESI
$L_5d8069::

            mov EAX,DWORD PTR [ESP+76]
            cmp EAX,ESI
            je $L_5d807b

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+76],ESI
$L_5d807b::

            mov EAX,DWORD PTR [ESP+72]
            cmp EAX,ESI
            je $L_5d808d

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+72],ESI
$L_5d808d::

            mov EAX,DWORD PTR [ESP+68]
            cmp EAX,ESI
            je $L_5d809f

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+68],ESI
$L_5d809f::

            mov EAX,DWORD PTR [ESP+60]
            cmp EAX,ESI
            je $L_5d80b1

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+60],ESI
$L_5d80b1::

            mov EAX,DWORD PTR [ESP+56]
            cmp EAX,ESI
            je $L_5d80c3

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+56],ESI
$L_5d80c3::

            mov EAX,DWORD PTR [ESP+52]
            cmp EAX,ESI
            je $L_5d80d1

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]
$L_5d80d1::

            mov EAX,EDI
            pop EDI
            pop ESI
            pop EBX
            add ESP,968
            ret 
$L_5d80dd::

            mov EAX,DWORD PTR [ESP+64]
            cmp EAX,ESI
            je $L_5d80ef

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+64],ESI
$L_5d80ef::

            mov EAX,DWORD PTR [ESP+132]
            cmp EAX,ESI
            je $L_5d8107

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+132],ESI
$L_5d8107::

            mov EAX,DWORD PTR [ESP+76]
            cmp EAX,ESI
            je $L_5d8119

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+76],ESI
$L_5d8119::

            mov EAX,DWORD PTR [ESP+72]
            cmp EAX,ESI
            je $L_5d812b

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+72],ESI
$L_5d812b::

            mov EAX,DWORD PTR [ESP+68]
            cmp EAX,ESI
            je $L_5d813d

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+68],ESI
$L_5d813d::

            mov EAX,DWORD PTR [ESP+60]
            cmp EAX,ESI
            je $L_5d814f

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]

            mov DWORD PTR [ESP+60],ESI
$L_5d814f::

            mov EAX,DWORD PTR [ESP+56]
            cmp EAX,ESI
            je $L_5d8161

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]

            mov DWORD PTR [ESP+56],ESI
$L_5d8161::

            mov EAX,DWORD PTR [ESP+52]
            cmp EAX,ESI
            je $L_5d816f

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]
$L_5d816f::

            cmp EBX,ESI
            je $L_5d8179

            mov DWORD PTR [EBX],1
$L_5d8179::

            pop EDI
            pop ESI
            xor EAX,EAX
            pop EBX
            add ESP,968
            ret 
$L_5d7bb9 ENDP
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
