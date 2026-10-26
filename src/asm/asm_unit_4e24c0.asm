; asm_unit_4e24c0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Sound_Play@20:PROC
EXTERN @Sound_Restart@16:PROC
EXTERN _Angle_F2Pi:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _World_Fatal:PROC
EXTERN __imp__InitCommonControls@0:PROC
EXTERN _atan2:NEAR
EXTERN _cos:NEAR
EXTERN _fabs:NEAR
EXTERN _sin:NEAR
EXTERN _sprintf:PROC
EXTERN _sqrt:NEAR

PUBLIC $L_4e2518
PUBLIC $L_4e4d96, $L_4e4dec, $L_4e4e2e, $L_4e4f03, $L_4e4f1a, $L_4e4f31, $L_4e4f48
PUBLIC $L_4e4f60, $L_4e4f74
PUBLIC $L_4e78cc, $L_4e7902, $L_4e794b

_TEXT SEGMENT
FUN_4e24e8 PROC

            push EBP
            mov EBP,ESP
            push ECX
            mov EAX,DWORD PTR [EBP+8]
            mov DWORD PTR [(_KingBss+52388)],EAX
            mov ECX,DWORD PTR [EBP+12]
            mov DWORD PTR [(_KingBss+52392)],ECX
            mov EDX,DWORD PTR [EBP+16]
            mov DWORD PTR [(_KingBss+52372)],EDX
            mov DWORD PTR [EBP-4],0
            jmp $L_4e2518
FUN_4e24e8 ENDP
$L_4e250f PROC

            mov EAX,DWORD PTR [EBP-4]
            add EAX,1
            mov DWORD PTR [EBP-4],EAX
$L_4e2518::

            cmp DWORD PTR [EBP-4],360
            jge $L_4e254c

            fild DWORD PTR [EBP-4]
            fmul QWORD PTR [(_KingRdata+10496)]
            fadd ST(0),ST(0)
            fdiv QWORD PTR [(_KingRdata+10488)]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _sin
$L_4e250f ENDP
$L_4e253d PROC

            add ESP,8
            mov ECX,DWORD PTR [EBP-4]
            fstp DWORD PTR [ECX*4+(_KingBss+50848)]
            jmp $L_4e250f
$L_4e253d ENDP
$L_4e254c PROC

            call DWORD PTR __imp__InitCommonControls@0

            mov ESP,EBP
            pop EBP
            ret 
$L_4e254c ENDP
ALIGN 2
ALIGN 4
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
FUN_4e4d82 PROC

            push EBP
            mov EBP,ESP
            sub ESP,280
            cmp DWORD PTR [EBP+8],0
            jne $L_4e4d96

            jmp $L_4e4f74
$L_4e4d96::

            mov EAX,DWORD PTR [EBP+16]
            fld DWORD PTR [EAX]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _fabs

            fstp QWORD PTR [EBP-272]
            add ESP,8
            mov ECX,DWORD PTR [EBP+16]
            fld DWORD PTR [ECX+4]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _fabs

            add ESP,8
            fadd QWORD PTR [EBP-272]
            fstp QWORD PTR [EBP-56]
            fld QWORD PTR [EBP-56]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,1
            jne $L_4e4dec

            fld QWORD PTR [EBP-56]
            fcomp QWORD PTR [(_KingRdata+1712)]
            fnstsw AX
            test AH,65
            jne $L_4e4e2e
$L_4e4dec::

            mov EDX,DWORD PTR [EBP+16]
            fld DWORD PTR [EDX+4]
            sub ESP,8
            fstp QWORD PTR [ESP]
            mov EAX,DWORD PTR [EBP+16]
            fld DWORD PTR [EAX]
            sub ESP,8
            fstp QWORD PTR [ESP]
            mov ECX,DWORD PTR [EBP-52]
            push ECX
            mov EDX,DWORD PTR [EBP-56]
            push EDX
            push OFFSET (_KingData+31836)
            lea EAX,DWORD PTR [EBP-264]
            push EAX
            call _sprintf

            add ESP,32
            lea ECX,DWORD PTR [EBP-264]
            push ECX
            call _World_Fatal
FUN_4e4d82 ENDP
$L_4e4e2b PROC

            add ESP,4
$L_4e4e2e::

            mov DWORD PTR [EBP-44],942503068
            mov DWORD PTR [EBP-40],1073791314
            mov EDX,DWORD PTR [EBP+16]
            fld DWORD PTR [EDX+4]
            sub ESP,8
            fstp QWORD PTR [ESP]
            mov EAX,DWORD PTR [EBP+16]
            fld DWORD PTR [EAX]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _atan2

            add ESP,16
            fchs 
            fstp QWORD PTR [EBP-20]
            push OFFSET (_KingData+31876)
            fld QWORD PTR [EBP-20]
            fsub QWORD PTR [EBP-44]
            fst DWORD PTR [EBP-276]
            push ECX
            fstp DWORD PTR [ESP]
            call _Angle_F2Pi

            add ESP,8
            fstp QWORD PTR [EBP-36]
            push OFFSET (_KingData+31892)
            fld QWORD PTR [EBP-20]
            fadd QWORD PTR [EBP-44]
            fst DWORD PTR [EBP-280]
            push ECX
            fstp DWORD PTR [ESP]
            call _Angle_F2Pi

            add ESP,8
            fstp QWORD PTR [EBP-8]
            fld QWORD PTR [EBP-36]
            fdiv QWORD PTR [(_KingRdata+1632)]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _cos
$L_4e4e2b ENDP
$L_4e4eb4 PROC

            add ESP,8
            fstp QWORD PTR [EBP-28]
            fld QWORD PTR [EBP-8]
            fdiv QWORD PTR [(_KingRdata+1632)]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _cos
$L_4e4eb4 ENDP
$L_4e4ece PROC

            add ESP,8
            fstp QWORD PTR [EBP-64]
            fld DWORD PTR [EBP+12]
            fmul QWORD PTR [EBP-28]
            fmul QWORD PTR [EBP-28]
            fstp DWORD PTR [EBP-48]
            fld DWORD PTR [EBP+12]
            fmul QWORD PTR [EBP-64]
            fmul QWORD PTR [EBP-64]
            fstp DWORD PTR [EBP-12]
            fld DWORD PTR [EBP-48]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,1
            je $L_4e4f03

            mov DWORD PTR [EBP-48],0
$L_4e4f03::

            fld DWORD PTR [EBP-12]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,1
            je $L_4e4f1a

            mov DWORD PTR [EBP-12],0
$L_4e4f1a::

            fld DWORD PTR [EBP-48]
            fcomp DWORD PTR [(_KingRdata+4688)]
            fnstsw AX
            test AH,65
            jne $L_4e4f31

            mov DWORD PTR [EBP-48],1065353216
$L_4e4f31::

            fld DWORD PTR [EBP-12]
            fcomp DWORD PTR [(_KingRdata+4688)]
            fnstsw AX
            test AH,65
            jne $L_4e4f48

            mov DWORD PTR [EBP-12],1065353216
$L_4e4f48::

            cmp DWORD PTR [EBP+20],0
            jge $L_4e4f60

            mov ECX,DWORD PTR [EBP-12]
            push ECX
            mov EDX,DWORD PTR [EBP-48]
            push EDX
            mov ECX,DWORD PTR [EBP+8]
            call @Sound_Restart@16

            jmp $L_4e4f74
$L_4e4f60::

            mov EAX,DWORD PTR [EBP+20]
            push EAX
            mov ECX,DWORD PTR [EBP-12]
            push ECX
            mov EDX,DWORD PTR [EBP-48]
            push EDX
            mov ECX,DWORD PTR [EBP+8]
            call @Sound_Play@20
$L_4e4f74::

            mov ESP,EBP
            pop EBP
            ret 
$L_4e4ece ENDP
ALIGN 8
ALIGN 16
ALIGN 16
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
FUN_4e7889 PROC

            push EBP
            mov EBP,ESP
            sub ESP,8
            fld DWORD PTR [EBP+12]
            fmul DWORD PTR [EBP+12]
            fld DWORD PTR [(_KingRdata+5128)]
            fmul DWORD PTR [EBP+8]
            fmul DWORD PTR [EBP+16]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [EBP-4]
            fld DWORD PTR [EBP-4]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,1
            je $L_4e78cc

            mov EAX,DWORD PTR [EBP+24]
            mov DWORD PTR [EAX],0
            mov ECX,DWORD PTR [EBP+20]
            mov DWORD PTR [ECX],0
            xor EAX,EAX
            jmp $L_4e794b
$L_4e78cc::

            fld DWORD PTR [EBP-4]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,64
            je $L_4e7902

            fld DWORD PTR [EBP+12]
            fchs 
            fld DWORD PTR [(_KingRdata+5132)]
            fmul DWORD PTR [EBP+8]
            fdivp ST(1),ST(0)
            mov EDX,DWORD PTR [EBP+20]
            fstp DWORD PTR [EDX]
            mov EAX,DWORD PTR [EBP+24]
            mov ECX,DWORD PTR [EBP+20]
            mov EDX,DWORD PTR [ECX]
            mov DWORD PTR [EAX],EDX
            mov EAX,1
            jmp $L_4e794b
$L_4e7902::

            fld DWORD PTR [EBP-4]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _sqrt
FUN_4e7889 ENDP
$L_4e7910 PROC

            add ESP,8
            fstp DWORD PTR [EBP-8]
            fld DWORD PTR [EBP+12]
            fchs 
            fadd DWORD PTR [EBP-8]
            fld DWORD PTR [(_KingRdata+5132)]
            fmul DWORD PTR [EBP+8]
            fdivp ST(1),ST(0)
            mov EAX,DWORD PTR [EBP+20]
            fstp DWORD PTR [EAX]
            fld DWORD PTR [EBP+12]
            fchs 
            fsub DWORD PTR [EBP-8]
            fld DWORD PTR [(_KingRdata+5132)]
            fmul DWORD PTR [EBP+8]
            fdivp ST(1),ST(0)
            mov ECX,DWORD PTR [EBP+24]
            fstp DWORD PTR [ECX]
            mov EAX,2
$L_4e794b::

            mov ESP,EBP
            pop EBP
            ret 
$L_4e7910 ENDP
ALIGN 4
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
_TEXT ENDS
END
