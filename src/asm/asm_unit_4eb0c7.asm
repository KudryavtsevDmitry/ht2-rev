; asm_unit_4eb0c7.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_645e38:NEAR
EXTERN @Holder_Construct@16:PROC
EXTERN _Game_Time:PROC
EXTERN _KingRdata:BYTE
EXTERN _cos:NEAR
EXTERN _log:NEAR
EXTERN _rand:PROC
EXTERN _sqrt:NEAR

PUBLIC $L_4eb338

_TEXT SEGMENT
FUN_4eb0c7 PROC

            push EBP
            mov EBP,ESP
            sub ESP,44
            call _rand

            mov DWORD PTR [EBP-16],EAX
            fild DWORD PTR [EBP-16]
            fdiv DWORD PTR [(_KingRdata+10600)]
            fstp DWORD PTR [EBP-8]
            call _rand

            mov DWORD PTR [EBP-20],EAX
            fild DWORD PTR [EBP-20]
            fdiv DWORD PTR [(_KingRdata+10600)]
            fstp DWORD PTR [EBP-12]
            fld DWORD PTR [EBP+8]
            fstp QWORD PTR [EBP-28]
            fld DWORD PTR [EBP+12]
            fstp QWORD PTR [EBP-36]
            fld DWORD PTR [EBP-8]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _log
FUN_4eb0c7 ENDP
$L_4eb10f PROC

            add ESP,8
            fmul QWORD PTR [(_KingRdata+10592)]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _sqrt
$L_4eb10f ENDP
$L_4eb123 PROC

            add ESP,8
            fmul QWORD PTR [EBP-36]
            fstp QWORD PTR [EBP-44]
            fld DWORD PTR [EBP-12]
            fmul QWORD PTR [(_KingRdata+10504)]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _cos

            add ESP,8
            fmul QWORD PTR [EBP-44]
            fadd QWORD PTR [EBP-28]
            fst DWORD PTR [EBP-4]
            mov ESP,EBP
            pop EBP
            ret 
$L_4eb123 ENDP
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
FUN_4eb260 PROC

            push -1
            push OFFSET $L_645e38
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ECX
            mov EAX,DWORD PTR [ESP+20]
            push ESI
            mov ESI,ECX
            push 1
            push EAX
            mov DWORD PTR [ESP+12],ESI
            call @Holder_Construct@16

            mov ECX,DWORD PTR [ESP+28]
            mov EDX,DWORD PTR [ESP+32]
            xor EAX,EAX
            mov DWORD PTR [ESI+24],ECX
            mov DWORD PTR [ESP+16],EAX
            mov DWORD PTR [ESI+32],EAX
            mov EAX,DWORD PTR [ESP+36]
            mov ECX,ESI
            mov DWORD PTR [ESI],OFFSET (_KingRdata+10604)
            mov DWORD PTR [ESI+36],EDX
            mov DWORD PTR [ESI+28],EAX
            call FUN_4eb2f0
FUN_4eb260 ENDP
$L_4eb2b4 PROC

            mov ECX,DWORD PTR [ESP+8]
            mov EAX,ESI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            add ESP,16
            ret 16
$L_4eb2b4 ENDP
ALIGN 16
FUN_4eb2f0 PROC

            push ECX
            push ESI
            mov ESI,ECX
            call _Game_Time

            mov EAX,DWORD PTR [ESI+24]
            push ECX
            fstp DWORD PTR [ESI+16]
            mov DWORD PTR [ESP+8],EAX
            mov ECX,EAX
            fld DWORD PTR [ESP+8]
            fmul QWORD PTR [(_KingRdata+8432)]
            fstp DWORD PTR [ESP]
            push ECX
            call FUN_4eb0c7
FUN_4eb2f0 ENDP
$L_4eb319 PROC

            fld DWORD PTR [ESI+24]
            fmul QWORD PTR [(_KingRdata+1760)]
            fld ST(1)
            add ESP,8
            fcomp ST(1)
            fnstsw AX
            test AH,1
            je $L_4eb338

            fxch ST(1)
            fsubr ST(0),ST(1)
            fadd ST(0),ST(1)
            fxch ST(1)
$L_4eb338::

            fstp ST(0)
            fadd DWORD PTR [ESI+16]
            fstp DWORD PTR [ESI+20]
            pop ESI
            pop ECX
            ret 
$L_4eb319 ENDP
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
; vehicle physics: src\truck\vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
_TEXT ENDS
END
