; asm_car2e.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Car_DriveForce@8:PROC
EXTERN @Car_EngineBrakeTorque@12:PROC
EXTERN @Car_TorqueNow@8:PROC
EXTERN @Car_WheelForceNow@12:PROC
EXTERN _KingRdata:BYTE

PUBLIC $L_4ff712, $L_4ff724, $L_4ff744, $L_4ff75e, $L_4ff764

_TEXT SEGMENT
ALIGN 16
FUN_4ff660 PROC

            sub ESP,12
            push ESI
            mov ESI,ECX
            cmp DWORD PTR [ESI+7196],1
            je $L_4ff75e

            fld DWORD PTR [ESI+7300]
            fcomp DWORD PTR [(_KingRdata+4688)]
            fnstsw AX
            test AH,1
            jne $L_4ff75e

            call @Car_TorqueNow@8
FUN_4ff660 ENDP
$L_4ff68f PROC

            mov EAX,DWORD PTR [ESI+7300]
            mov ECX,ESI
            fstp DWORD PTR [ESP+12]
            push EAX
            call @Car_EngineBrakeTorque@12

            mov ECX,DWORD PTR [ESP+12]
            fstp DWORD PTR [ESP+8]
            push ECX
            mov ECX,ESI
            call @Car_WheelForceNow@12

            mov EDX,DWORD PTR [ESP+8]
            mov ECX,ESI
            fstp DWORD PTR [ESP+12]
            push EDX
            call @Car_WheelForceNow@12

            fstp DWORD PTR [ESP+8]
            mov ECX,ESI
            call @Car_DriveForce@8

            fst DWORD PTR [ESP+4]
            fld DWORD PTR [ESP+12]
            fabs 
            fcompp 
            fnstsw AX
            test AH,1
            jne $L_4ff75e

            fld DWORD PTR [ESP+4]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,64
            jne $L_4ff75e

            mov EAX,DWORD PTR [ESI+7196]
            test EAX,EAX
            jne $L_4ff712

            fld DWORD PTR [ESP+8]
            fabs 
            fadd DWORD PTR [ESP+4]
            fld DWORD PTR [ESP+12]
            fsub DWORD PTR [ESP+8]
            fabs 
            fdivp ST(1),ST(0)
            jmp $L_4ff724
$L_4ff712::

            fld DWORD PTR [ESP+4]
            fsub DWORD PTR [ESP+8]
            fld DWORD PTR [ESP+12]
            fsub DWORD PTR [ESP+8]
            fdivp ST(1),ST(0)
$L_4ff724::

            fmul QWORD PTR [(_KingRdata+6488)]
            fcom QWORD PTR [(_KingRdata+6488)]
            fnstsw AX
            test AH,65
            jne $L_4ff744

            fstp ST(0)
            fld DWORD PTR [(_KingRdata+10808)]
            pop ESI
            add ESP,12
            ret 
$L_4ff744::

            fcom DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,1
            je $L_4ff764

            fstp ST(0)
            fld DWORD PTR [(_KingRdata+4708)]
            pop ESI
            add ESP,12
            ret 
$L_4ff75e::

            fld DWORD PTR [(_KingRdata+4688)]
$L_4ff764::

            pop ESI
            add ESP,12
            ret 
$L_4ff68f ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
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
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
ALIGN 16
; src/vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; vehicle methods: src\truck\vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
ALIGN 16
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
          BYTE 090H
ALIGN 16
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
ALIGN 16
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
ALIGN 16
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; engine and gearbox: src\truck\drivetrain.cpp, nmake ASM_DRIVETRAIN=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
          BYTE 090H
ALIGN 16
; src/game_shell.cpp, nmake ASM_GAME_SHELL=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/vehicle_setup.cpp, nmake ASM_VEHICLE_SETUP=1 assembles the original
_TEXT ENDS
END
