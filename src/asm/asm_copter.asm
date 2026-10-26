; asm_copter.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN _Cv_InitNames:NEAR
EXTERN @Copter_SideDrift@12:PROC
EXTERN @Vec3_AddAssign@16:PROC
EXTERN _Angle_F2Pi:PROC
EXTERN _Copter_PlaneCircle:PROC
EXTERN _Game_FrameTime:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _Matrix_RotationZ:PROC
EXTERN _Query_4e7261:PROC
EXTERN _Query_4e7566:PROC
EXTERN _Vec3_Length:PROC
EXTERN _Vec3_Normalized:PROC
EXTERN _World_Fatal:PROC
EXTERN __CIacos:NEAR
EXTERN __CIpow:NEAR
EXTERN __ftol:NEAR
EXTERN _rand:PROC

PUBLIC $L_560f00, $L_5637ce, $L_5637d4, $L_5638d6, $L_563a70, $L_563a8e, $L_563b23, $L_563c0b
PUBLIC $L_563c13, $L_563c2f, $L_563c4f, $L_563c62, $L_563c64, $L_563ca2, $L_563cab, $L_563cc4
PUBLIC $L_563cc9, $L_563d1c, $L_563d35, $L_563d84, $L_563d89, $L_563dd3, $L_563dd9, $L_563ff8
PUBLIC $L_5640a8, $L_5640bd, $L_5640da, $L_5640f3, $L_5640f7, $L_564233, $L_564272, $L_564558
PUBLIC $L_56455e, $L_56457b, $L_564581, $L_5646f1, $L_56486a, $L_5648d8, $L_5649aa, $L_564a1f
PUBLIC $L_564b98, $L_564bac, $L_564c5f, $L_564ced, $L_564d58, $L_564d60, $L_564d9c, $L_564db1
PUBLIC $L_564de0, $L_564df5, $L_564dff, $L_564e01, $L_564e4b, $L_565074, $L_56509b, $L_56525b
PUBLIC $L_565272, $L_565346, $L_56535c, $L_56541f, $L_5655cf, $L_5655ea, $L_5655f4, $L_565600
PUBLIC $L_5656d8, $L_5656ef, $L_565737, $L_56574c, $L_565768, $L_5657ac, $L_5657c1, $L_5657df
PUBLIC $L_565845, $L_56585a, $L_565878, $L_5658ba, $L_5658d8, $L_565922, $L_565937, $L_56596c
PUBLIC $L_565976, $L_56598b, $L_56598d, $L_5659a4, $L_5659b9, $L_5659f1, $L_5659f3, $L_565ab2
PUBLIC $L_565ab4, $L_565ac9, $L_565af8, $L_565b8f, $L_565bab, $L_565c11, $L_565c17, $L_565c3b
PUBLIC $L_565c46, $L_565cd4, $L_565cfd, $L_565d10, $L_565d36, $L_565d5f, $L_565d70, $L_565d76
PUBLIC $L_565d8f, $L_565d9e, $L_565dc4, $L_565dd7, $L_565de1, $L_565de5, $L_565de7, $L_565e68
PUBLIC $L_565e81, $L_565e8b, $L_565ed9, $L_565ef9, $L_565efb, $L_565f0f, $L_565f62, $L_565f7b
PUBLIC $L_565fc8

_TEXT SEGMENT
$L_560ef0 PROC

            jmp $L_560f00
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_560f00::

            jmp _Cv_InitNames
$L_560ef0 ENDP
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; helicopters: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; helicopters: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; helicopters: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
FUN_563600 PROC

            sub ESP,608
            push ESI
            push EDI
            mov ESI,ECX
            call _Game_FrameTime

            fst DWORD PTR [ESP+52]
            fmul QWORD PTR [(_KingRdata+1920)]
            fstp QWORD PTR [ESP+144]
            fld QWORD PTR [(_KingRdata+6488)]
            fld QWORD PTR [ESP+144]
            call __CIpow
FUN_563600 ENDP
$L_563632 PROC

            fstp DWORD PTR [ESI+3212]
            fld QWORD PTR [(_KingRdata+10840)]
            fld QWORD PTR [ESP+144]
            call __CIpow
$L_563632 ENDP
$L_56364a PROC

            fstp DWORD PTR [ESI+3216]
            fld QWORD PTR [(_KingRdata+19744)]
            mov EAX,DWORD PTR [ESI+3212]
            fld QWORD PTR [ESP+144]
            mov DWORD PTR [ESI+3220],EAX
            call __CIpow

            fstp DWORD PTR [ESI+3224]
            mov ECX,DWORD PTR [ESI+3212]
            mov DWORD PTR [ESI+3228],ECX
            fld DWORD PTR [(_KingBss+491016)]
            fmul DWORD PTR [ESI+24]
            fld DWORD PTR [(_KingBss+491012)]
            fmul DWORD PTR [ESI+12]
            faddp ST(1),ST(0)
            fld DWORD PTR [(_KingBss+491008)]
            fmul DWORD PTR [ESI]
            faddp ST(1),ST(0)
            fld DWORD PTR [(_KingBss+491016)]
            fmul DWORD PTR [ESI+28]
            fld DWORD PTR [(_KingBss+491012)]
            fmul DWORD PTR [ESI+16]
            faddp ST(1),ST(0)
            fld DWORD PTR [(_KingBss+491008)]
            fmul DWORD PTR [ESI+4]
            faddp ST(1),ST(0)
            fld DWORD PTR [(_KingBss+491016)]
            fmul DWORD PTR [ESI+32]
            fld DWORD PTR [(_KingBss+491012)]
            fmul DWORD PTR [ESI+20]
            faddp ST(1),ST(0)
            fld DWORD PTR [(_KingBss+491008)]
            fmul DWORD PTR [ESI+8]
            faddp ST(1),ST(0)
            fld ST(0)
            fmul DWORD PTR [(_KingBss+491028)]
            fld ST(2)
            fmul DWORD PTR [(_KingBss+491032)]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+36]
            fld ST(2)
            fmul DWORD PTR [(_KingBss+491032)]
            fxch ST(1)
            fmul DWORD PTR [(_KingBss+491024)]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+40]
            fmul DWORD PTR [(_KingBss+491024)]
            fxch ST(1)
            fmul DWORD PTR [(_KingBss+491028)]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+40]
            fmul DWORD PTR [(_KingBss+491032)]
            fld ST(1)
            fmul DWORD PTR [(_KingBss+491028)]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+216]
            fmul DWORD PTR [(_KingBss+491024)]
            fld DWORD PTR [ESP+36]
            fmul DWORD PTR [(_KingBss+491032)]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+220]
            fld DWORD PTR [ESP+36]
            fmul DWORD PTR [(_KingBss+491028)]
            fld DWORD PTR [ESP+40]
            fmul DWORD PTR [(_KingBss+491024)]
            mov EDX,DWORD PTR [ESP+216]
            mov EAX,DWORD PTR [ESP+220]
            mov DWORD PTR [ESP+8],EDX
            mov DWORD PTR [ESP+12],EAX
            fsubp ST(1),ST(0)
            lea EDX,DWORD PTR [ESP+8]
            lea EAX,DWORD PTR [ESP+56]
            push EDX
            push EAX
            fstp DWORD PTR [ESP+232]
            mov ECX,DWORD PTR [ESP+232]
            mov DWORD PTR [ESP+24],ECX
            call _Vec3_Normalized

            fld DWORD PTR [ESI+76]
            fmul DWORD PTR [ESI+108]
            fld DWORD PTR [ESI+104]
            fmul DWORD PTR [ESI+64]
            add ESP,8
            faddp ST(1),ST(0)
            fld DWORD PTR [ESI+52]
            fmul DWORD PTR [ESI+100]
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+280]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,65
            jne $L_5637ce

            fld QWORD PTR [(_KingRdata+1648)]
            jmp $L_5637d4
$L_5637ce::

            fld QWORD PTR [(_KingRdata+6728)]
$L_5637d4::

            fstp DWORD PTR [ESP+80]
            fld DWORD PTR [ESP+52]
            fld DWORD PTR [ESI+3220]
            fsubr QWORD PTR [(_KingRdata+1648)]
            push EBX
            push EBP
            mov DWORD PTR [ESP+84],1065353216
            mov DWORD PTR [ESP+32],0
            fdivp ST(1),ST(0)
            mov DWORD PTR [ESP+36],0
            mov DWORD PTR [ESP+40],0
            mov DWORD PTR [ESP+308],0
            mov DWORD PTR [ESP+312],0
            mov DWORD PTR [ESP+316],0
            xor EBX,EBX
            lea EDI,DWORD PTR [ESI+36]
            fadd ST(0),ST(0)
            fmul DWORD PTR [ESP+288]
            fstp DWORD PTR [ESP+180]
            fld DWORD PTR [(_KingBss+491024)]
            fchs 
            fld DWORD PTR [(_KingBss+491028)]
            fchs 
            fstp DWORD PTR [ESP+20]
            fld DWORD PTR [(_KingBss+491032)]
            fchs 
            fstp DWORD PTR [ESP+24]
            fild DWORD PTR [ESI+3460]
            fmul QWORD PTR [(_KingRdata+1464)]
            fstp DWORD PTR [ESP+112]
            fmul DWORD PTR [ESP+112]
            fld DWORD PTR [ESP+20]
            fmul DWORD PTR [ESP+112]
            fstp DWORD PTR [ESP+48]
            fld DWORD PTR [ESP+24]
            fmul DWORD PTR [ESP+112]
            fstp DWORD PTR [ESP+52]
            fld ST(0)
            fchs 
            fstp DWORD PTR [ESP+224]
            fld DWORD PTR [ESP+48]
            fchs 
            fstp DWORD PTR [ESP+228]
            fld DWORD PTR [ESP+52]
            fchs 
            fstp DWORD PTR [ESP+232]
            fadd ST(0),ST(0)
            fstp DWORD PTR [ESP+184]
            fld DWORD PTR [ESP+48]
            fadd ST(0),ST(0)
            fstp DWORD PTR [ESP+188]
            fld DWORD PTR [ESP+52]
            fadd ST(0),ST(0)
            fstp DWORD PTR [ESP+192]
$L_5638d6::

            fld DWORD PTR [ESP+84]
            fsub DWORD PTR [(_KingRdata+4688)]
            fld DWORD PTR [ESP+224]
            fmul ST(0),ST(1)
            fstp DWORD PTR [ESP+16]
            fld ST(0)
            fmul DWORD PTR [ESP+228]
            fstp DWORD PTR [ESP+20]
            fmul DWORD PTR [ESP+232]
            fld DWORD PTR [ESP+16]
            fadd DWORD PTR [EDI]
            fstp DWORD PTR [ESP+44]
            fld DWORD PTR [ESP+20]
            fadd DWORD PTR [EDI+4]
            mov ECX,DWORD PTR [ESP+44]
            mov DWORD PTR [ESP+248],ECX
            fstp DWORD PTR [ESP+48]
            mov EDX,DWORD PTR [ESP+48]
            fadd DWORD PTR [EDI+8]
            mov DWORD PTR [ESP+252],EDX
            fstp DWORD PTR [ESP+52]
            fld DWORD PTR [ESP+184]
            fmul DWORD PTR [ESP+84]
            fld DWORD PTR [ESP+188]
            fmul DWORD PTR [ESP+84]
            mov EAX,DWORD PTR [ESP+52]
            mov DWORD PTR [ESP+256],EAX
            fstp DWORD PTR [ESP+368]
            fld DWORD PTR [ESP+192]
            fmul DWORD PTR [ESP+84]
            fstp DWORD PTR [ESP+372]
            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [ESP+180]
            fld DWORD PTR [ESP+68]
            fmul DWORD PTR [ESP+180]
            fstp DWORD PTR [ESP+344]
            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [ESP+180]
            fstp DWORD PTR [ESP+348]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+368]
            fadd DWORD PTR [ESP+344]
            fstp DWORD PTR [ESP+356]
            fld DWORD PTR [ESP+372]
            fadd DWORD PTR [ESP+348]
            fstp DWORD PTR [ESP+360]
            fadd DWORD PTR [EDI]
            fstp DWORD PTR [ESP+236]
            fld DWORD PTR [ESP+356]
            fadd DWORD PTR [EDI+4]
            mov ECX,DWORD PTR [ESP+236]
            mov DWORD PTR [ESP+296],ECX
            lea ECX,DWORD PTR [ESP+308]
            fstp DWORD PTR [ESP+240]
            fld DWORD PTR [ESP+360]
            fadd DWORD PTR [EDI+8]
            mov EDX,DWORD PTR [ESP+240]
            push ECX
            mov DWORD PTR [ESP+304],EDX
            fstp DWORD PTR [ESP+248]
            mov EAX,DWORD PTR [ESP+248]
            mov DWORD PTR [ESP+308],EAX
            lea EDX,DWORD PTR [ESP+36]
            lea EAX,DWORD PTR [ESP+300]
            push EDX
            mov EDX,DWORD PTR [ESI+3480]
            lea ECX,DWORD PTR [ESP+256]
            push EAX
            mov EAX,DWORD PTR [ESI+96]
            push ECX
            push 1065353216
            push EDX
            push EAX
            call _Query_4e7261

            add ESP,28
            test AL,1
            jne $L_563a70

            fld DWORD PTR [ESP+84]
            fmul DWORD PTR [(_KingRdata+5128)]
            inc EBX
            cmp EBX,4
            fstp DWORD PTR [ESP+84]
            jl $L_5638d6

            jmp $L_563a8e
$L_563a70::

            mov EDX,DWORD PTR [ESP+32]
            mov EAX,DWORD PTR [ESP+36]
            lea ECX,DWORD PTR [ESI+3172]
            mov DWORD PTR [ESI+3172],EDX
            mov EDX,DWORD PTR [ESP+40]
            mov DWORD PTR [ECX+4],EAX
            mov DWORD PTR [ECX+8],EDX
$L_563a8e::

            cmp EBX,4
            jne $L_563b23

            fild DWORD PTR [ESI+3460]
            lea EAX,DWORD PTR [ESI+3172]
            fld ST(0)
            fmul DWORD PTR [(_KingBss+491024)]
            fstp DWORD PTR [ESP+16]
            fld ST(0)
            fmul DWORD PTR [(_KingBss+491028)]
            fstp DWORD PTR [ESP+20]
            fmul DWORD PTR [(_KingBss+491032)]
            fld DWORD PTR [ESP+16]
            fmul DWORD PTR [(_KingRdata+4868)]
            fstp DWORD PTR [ESP+224]
            fld DWORD PTR [ESP+20]
            fmul DWORD PTR [(_KingRdata+4868)]
            fstp DWORD PTR [ESP+228]
            fmul DWORD PTR [(_KingRdata+4868)]
            fld DWORD PTR [EDI]
            fsub DWORD PTR [ESP+224]
            fstp DWORD PTR [ESP+44]
            fld DWORD PTR [EDI+4]
            fsub DWORD PTR [ESP+228]
            mov ECX,DWORD PTR [ESP+44]
            fstp DWORD PTR [ESP+48]
            fld DWORD PTR [EDI+8]
            mov EDX,DWORD PTR [ESP+48]
            mov DWORD PTR [EAX],ECX
            fsub ST(0),ST(1)
            mov DWORD PTR [EAX+4],EDX
            fstp DWORD PTR [ESP+52]
            mov ECX,DWORD PTR [ESP+52]
            fstp ST(0)
            mov DWORD PTR [EAX+8],ECX
$L_563b23::

            mov EAX,DWORD PTR [ESI+3432]
            xor EBX,EBX
            cmp EAX,EBX
            jne $L_563d84

            cmp DWORD PTR [ESI+3464],EBX
            je $L_563d84

            fld DWORD PTR [ESI+72]
            fmul DWORD PTR [ESI+3476]
            fld DWORD PTR [ESI+60]
            fmul DWORD PTR [ESI+3472]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESI+48]
            fmul DWORD PTR [ESI+3468]
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESI+84]
            fld DWORD PTR [ESI+76]
            fmul DWORD PTR [ESI+3476]
            fld DWORD PTR [ESI+52]
            fmul DWORD PTR [ESI+3468]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESI+64]
            fmul DWORD PTR [ESI+3472]
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESI+88]
            fpatan 
            mov DWORD PTR [ESI+3432],EBX
            fchs 
            fld DWORD PTR [ESP+60]
            fmul DWORD PTR [ESI+120]
            fabs 
            fld DWORD PTR [ESI+3224]
            fsubr QWORD PTR [(_KingRdata+1648)]
            fdivr QWORD PTR [(_KingRdata+1648)]
            fmulp ST(1),ST(0)
            fadd QWORD PTR [(_KingRdata+1440)]
            fstp DWORD PTR [ESP+112]
            fld ST(0)
            fmul DWORD PTR [ESI+120]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,65
            jne $L_563c64

            fld ST(0)
            fabs 
            fld DWORD PTR [ESP+112]
            fld ST(1)
            fcompp 
            fnstsw AX
            test AH,1
            je $L_563c62

            fld DWORD PTR [ESP+112]
            fmul DWORD PTR [(_KingRdata+4864)]
            fld ST(1)
            fcompp 
            fnstsw AX
            test AH,1
            je $L_563c13

            cmp DWORD PTR [ESI+3236],10
            fstp ST(0)
            je $L_563c0b

            fstp ST(0)
            fld DWORD PTR [(_KingRdata+4708)]
            jmp $L_563c64
$L_563c0b::

            fmul DWORD PTR [(_KingRdata+6504)]
            jmp $L_563c2f
$L_563c13::

            fld DWORD PTR [ESP+112]
            fdivr ST(0),ST(1)
            fsub QWORD PTR [(_KingRdata+19736)]
            fxch ST(2)
            fmulp ST(2),ST(0)
            fxch ST(1)
            fmul QWORD PTR [(_KingRdata+19728)]
            fxch ST(1)
            fstp ST(0)
$L_563c2f::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_563c4f

            fld QWORD PTR [(_KingRdata+1648)]
            call __ftol

            mov DWORD PTR [ESI+3432],EAX
            jmp $L_563c64
$L_563c4f::

            fld QWORD PTR [(_KingRdata+6728)]
            call __ftol

            mov DWORD PTR [ESI+3432],EAX
            jmp $L_563c64
$L_563c62::

            fstp ST(0)
$L_563c64::

            mov EAX,DWORD PTR [ESI+3236]
            fstp DWORD PTR [ESI+3444]
            cmp EAX,3
            je $L_563cc4

            cmp EAX,11
            je $L_563cc4

            cmp EAX,10
            jne $L_563d84

            fld DWORD PTR [ESP+288]
            fcomp DWORD PTR [(_KingRdata+5132)]
            fnstsw AX
            test AH,1
            je $L_563ca2

            mov EBP,1
            mov DWORD PTR [ESP+120],EBP
            jmp $L_563cab
$L_563ca2::

            mov DWORD PTR [ESP+120],EBX
            mov EBP,1
$L_563cab::

            fild DWORD PTR [ESP+120]
            fabs 
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,64
            jne $L_563d89

            jmp $L_563cc9
$L_563cc4::

            mov EBP,1
$L_563cc9::

            call _rand

            mov DWORD PTR [ESP+176],EAX
            fild DWORD PTR [ESP+176]
            fmul DWORD PTR [(_KingRdata+4692)]
            fsubr QWORD PTR [(_KingRdata+1760)]
            fstp QWORD PTR [ESP+152]
            call _Game_FrameTime

            fadd ST(0),ST(0)
            fmul QWORD PTR [ESP+152]
            fadd DWORD PTR [ESI+3332]
            fst DWORD PTR [ESI+3332]
            fcomp QWORD PTR [(_KingRdata+1648)]
            fnstsw AX
            test AH,65
            jne $L_563d1c

            mov DWORD PTR [ESI+3332],EBX
$L_563d1c::

            fld DWORD PTR [ESI+3332]
            fcomp QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_563d35

            mov DWORD PTR [ESI+3332],EBX
$L_563d35::

            call _Game_FrameTime

            fmul DWORD PTR [ESI+3332]
            push OFFSET (_KingData+62940)
            fadd DWORD PTR [ESI+3328]
            fst DWORD PTR [ESP+180]
            mov EDX,DWORD PTR [ESP+180]
            fstp DWORD PTR [ESI+3328]
            push EDX
            call _Angle_F2Pi

            fst DWORD PTR [ESI+3328]
            fsin 
            add ESP,8
            fmul DWORD PTR [(_KingData+62120)]
            fadd DWORD PTR [ESI+3444]
            fstp DWORD PTR [ESI+3444]
            jmp $L_563d89
$L_563d84::

            mov EBP,1
$L_563d89::

            lea EAX,DWORD PTR [ESP+64]
            mov ECX,ESI
            push EAX
            call @Copter_SideDrift@12

            fadd DWORD PTR [(_KingRdata+4896)]
            mov DWORD PTR [ESP+160],1203982336
            fst DWORD PTR [ESP+196]
            fmul QWORD PTR [(_KingRdata+1640)]
            fstp DWORD PTR [ESP+76]
            fld DWORD PTR [ESP+288]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,1
            jne $L_563dd3

            fld QWORD PTR [(_KingRdata+1648)]
            jmp $L_563dd9
$L_563dd3::

            fld QWORD PTR [(_KingRdata+6728)]
$L_563dd9::

            fmul DWORD PTR [ESP+76]
            fld DWORD PTR [ESP+64]
            fmul ST(0),ST(1)
            mov DWORD PTR [ESP+260],0
            mov DWORD PTR [ESP+264],0
            mov DWORD PTR [ESP+268],0
            mov DWORD PTR [ESP+124],0
            fstp DWORD PTR [ESP+100]
            fld DWORD PTR [ESP+68]
            fmul ST(0),ST(1)
            mov DWORD PTR [ESP+128],0
            mov DWORD PTR [ESP+132],0
            mov DWORD PTR [ESP+352],0
            mov DWORD PTR [ESP+356],0
            fstp DWORD PTR [ESP+104]
            fld DWORD PTR [ESP+72]
            fmul ST(0),ST(1)
            mov DWORD PTR [ESP+360],0
            mov DWORD PTR [ESP+224],0
            mov DWORD PTR [ESP+228],0
            mov DWORD PTR [ESP+232],0
            fstp DWORD PTR [ESP+108]
            fstp ST(0)
            fld DWORD PTR [ESP+100]
            fadd DWORD PTR [EDI]
            fstp DWORD PTR [ESP+44]
            fld DWORD PTR [ESP+104]
            fadd DWORD PTR [EDI+4]
            mov ECX,DWORD PTR [ESP+44]
            mov DWORD PTR [ESP+504],ECX
            lea ECX,DWORD PTR [ESP+224]
            fstp DWORD PTR [ESP+48]
            fld DWORD PTR [ESP+108]
            fadd DWORD PTR [EDI+8]
            mov EDX,DWORD PTR [ESP+48]
            push ECX
            mov ECX,DWORD PTR [ESI+3480]
            mov DWORD PTR [ESP+512],EDX
            fstp DWORD PTR [ESP+56]
            mov EAX,DWORD PTR [ESP+56]
            lea EDX,DWORD PTR [ESP+356]
            mov DWORD PTR [ESP+516],EAX
            lea EAX,DWORD PTR [ESP+508]
            push EDX
            mov EDX,DWORD PTR [ESI+96]
            push EAX
            push EDI
            push 1069547520
            push ECX
            push EDX
            call _Query_4e7566

            add ESP,28
            and EAX,EBP
            mov EBX,0
            setne BL
            test EAX,EAX
            mov DWORD PTR [ESP+376],EBX
            je $L_563ff8

            fld DWORD PTR [EDI]
            fsub DWORD PTR [ESP+352]
            fstp DWORD PTR [ESP+44]
            fld DWORD PTR [EDI+4]
            fsub DWORD PTR [ESP+356]
            mov EAX,DWORD PTR [ESP+44]
            mov DWORD PTR [ESP+16],EAX
            fstp DWORD PTR [ESP+48]
            fld DWORD PTR [EDI+8]
            fsub DWORD PTR [ESP+360]
            mov ECX,DWORD PTR [ESP+48]
            mov DWORD PTR [ESP+20],ECX
            fstp DWORD PTR [ESP+52]
            fld DWORD PTR [EDI]
            fstp DWORD PTR [ESP+44]
            fld DWORD PTR [EDI+4]
            mov EAX,DWORD PTR [ESP+44]
            mov EDX,DWORD PTR [ESP+52]
            fstp DWORD PTR [ESP+48]
            fld DWORD PTR [EDI+8]
            mov ECX,DWORD PTR [ESP+48]
            mov DWORD PTR [ESP+32],EAX
            fstp DWORD PTR [ESP+52]
            mov DWORD PTR [ESP+24],EDX
            mov EDX,DWORD PTR [ESP+52]
            lea EAX,DWORD PTR [ESP+16]
            mov DWORD PTR [ESP+36],ECX
            push EAX
            mov DWORD PTR [ESP+44],EDX
            call _Vec3_Length

            fstp DWORD PTR [ESP+180]
            lea ECX,DWORD PTR [ESP+36]
            push ECX
            call _Vec3_Length

            mov EDX,DWORD PTR [ESP+360]
            mov EAX,DWORD PTR [ESP+364]
            mov ECX,DWORD PTR [ESP+368]
            mov DWORD PTR [ESP+268],EDX
            mov EDX,DWORD PTR [ESP+232]
            mov DWORD PTR [ESP+272],EAX
            mov EAX,DWORD PTR [ESP+236]
            mov DWORD PTR [ESP+276],ECX
            mov ECX,DWORD PTR [ESP+240]
            mov DWORD PTR [ESP+132],EDX
            mov EDX,DWORD PTR [ESP+184]
            add ESP,8
            fstp ST(0)
            mov DWORD PTR [ESP+128],EAX
            mov DWORD PTR [ESP+132],ECX
            mov DWORD PTR [ESP+160],EDX
$L_563ff8::

            fld DWORD PTR [ESP+76]
            fsub DWORD PTR [ESP+196]
            xor EAX,EAX
            mov DWORD PTR [ESP+120],EAX
            mov DWORD PTR [ESP+176],EAX
            fld ST(0)
            fmul QWORD PTR [(_KingRdata+1760)]
            fadd DWORD PTR [ESP+196]
            fstp DWORD PTR [ESP+280]
            fmul QWORD PTR [(_KingRdata+1960)]
            fadd DWORD PTR [ESP+196]
            fstp DWORD PTR [ESP+116]
            fld DWORD PTR [ESP+280]
            fsub DWORD PTR [ESP+196]
            fstp DWORD PTR [ESP+336]
            fld DWORD PTR [ESP+160]
            fsub DWORD PTR [ESP+280]
            fdiv DWORD PTR [ESP+336]
            fst DWORD PTR [ESP+416]
            fld DWORD PTR [ESP+116]
            fsub DWORD PTR [ESP+196]
            fstp DWORD PTR [ESP+96]
            fld DWORD PTR [ESP+160]
            fsub DWORD PTR [ESP+116]
            fdiv DWORD PTR [ESP+96]
            fst DWORD PTR [ESP+152]
            fstp DWORD PTR [ESP+28]
            fcom QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_5640a8

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_5640bd
$L_5640a8::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_5640bd

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+1656)]
$L_5640bd::

            fstp DWORD PTR [ESP+84]
            fld DWORD PTR [ESP+28]
            fcomp QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_5640da

            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_5640f7
$L_5640da::

            fld DWORD PTR [ESP+28]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_5640f3

            fld QWORD PTR [(_KingRdata+1656)]
            jmp $L_5640f7
$L_5640f3::

            fld DWORD PTR [ESP+28]
$L_5640f7::

            fstp DWORD PTR [ESP+28]
            test BL,1
            je $L_564272

            fld DWORD PTR [ESP+128]
            fmul DWORD PTR [ESP+72]
            fld DWORD PTR [ESP+68]
            fmul DWORD PTR [ESP+132]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [ESP+132]
            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [ESP+124]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+68]
            fmul DWORD PTR [ESP+124]
            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [ESP+128]
            fsubp ST(1),ST(0)
            fld ST(1)
            fmul DWORD PTR [ESP+132]
            fld ST(1)
            fmul DWORD PTR [ESP+128]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+44]
            mov EAX,DWORD PTR [ESP+44]
            fmul DWORD PTR [ESP+124]
            fld DWORD PTR [ESP+132]
            fmul ST(0),ST(3)
            mov DWORD PTR [ESP+16],EAX
            lea EAX,DWORD PTR [ESP+16]
            push EAX
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+52]
            fld DWORD PTR [ESP+132]
            fmul ST(0),ST(2)
            fxch ST(1)
            fmul DWORD PTR [ESP+128]
            mov ECX,DWORD PTR [ESP+52]
            mov DWORD PTR [ESP+24],ECX
            lea ECX,DWORD PTR [ESP+48]
            fsubp ST(1),ST(0)
            push ECX
            fstp DWORD PTR [ESP+60]
            mov EDX,DWORD PTR [ESP+60]
            fstp ST(0)
            mov DWORD PTR [ESP+32],EDX
            call _Vec3_Normalized

            fld DWORD PTR [ESP+140]
            fcomp DWORD PTR [(_KingRdata+4708)]
            add ESP,8
            fnstsw AX
            test AH,65
            jne $L_564272

            fld DWORD PTR [ESP+288]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,64
            jne $L_564272

            fld DWORD PTR [ESP+52]
            fld DWORD PTR [ESP+48]
            fmul DWORD PTR [ESP+48]
            fld DWORD PTR [ESP+44]
            fmul DWORD PTR [ESP+44]
            faddp ST(1),ST(0)
            fsqrt 
            fdivp ST(1),ST(0)
            fld DWORD PTR [ESI+108]
            fld DWORD PTR [ESP+288]
            fabs 
            fdivp ST(1),ST(0)
            fld ST(1)
            fcomp QWORD PTR [(_KingRdata+1760)]
            fnstsw AX
            test AH,1
            je $L_564233

            mov DWORD PTR [ESI+3440],EBP
            mov DWORD PTR [ESI+3452],1065353216
            mov DWORD PTR [ESP+176],EBP
$L_564233::

            fxch ST(1)
            fcomp ST(1)
            fnstsw AX
            test AH,65
            fstp ST(0)
            jne $L_564272

            fld DWORD PTR [ESP+84]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,1
            je $L_564272

            fld DWORD PTR [ESP+88]
            call __ftol

            fld DWORD PTR [ESP+84]
            fmul DWORD PTR [ESP+88]
            mov DWORD PTR [ESI+3436],EAX
            mov DWORD PTR [ESP+120],EBP
            fstp DWORD PTR [ESI+3448]
$L_564272::

            cmp DWORD PTR [ESI+3236],EBP
            je $L_565de1

            lea EDX,DWORD PTR [ESP+564]
            push 1061752795
            push EDX
            call _Matrix_RotationZ

            fld DWORD PTR [ESP+116]
            fmul DWORD PTR [EAX+24]
            fld DWORD PTR [ESP+112]
            fmul DWORD PTR [EAX+12]
            push 3209236443
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+112]
            fmul DWORD PTR [EAX]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+120]
            fmul DWORD PTR [EAX+28]
            fld DWORD PTR [ESP+116]
            fmul DWORD PTR [EAX+16]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+112]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+32]
            fld DWORD PTR [ESP+120]
            fmul DWORD PTR [EAX+32]
            fld DWORD PTR [ESP+116]
            fmul DWORD PTR [EAX+20]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+112]
            fmul DWORD PTR [EAX+8]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+36]
            fadd DWORD PTR [EDI]
            fstp DWORD PTR [ESP+44]
            fld DWORD PTR [ESP+32]
            fadd DWORD PTR [EDI+4]
            mov EAX,DWORD PTR [ESP+44]
            mov DWORD PTR [ESP+468],EAX
            lea EAX,DWORD PTR [ESP+576]
            fstp DWORD PTR [ESP+48]
            fld DWORD PTR [ESP+36]
            fadd DWORD PTR [EDI+8]
            mov ECX,DWORD PTR [ESP+48]
            push EAX
            mov DWORD PTR [ESP+476],ECX
            fstp DWORD PTR [ESP+56]
            mov EDX,DWORD PTR [ESP+56]
            mov DWORD PTR [ESP+480],EDX
            call _Matrix_RotationZ

            fld DWORD PTR [ESP+124]
            fmul DWORD PTR [EAX+24]
            fld DWORD PTR [ESP+120]
            fmul DWORD PTR [EAX+12]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+116]
            fmul DWORD PTR [EAX]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+124]
            fmul DWORD PTR [EAX+28]
            fld DWORD PTR [ESP+120]
            fmul DWORD PTR [EAX+16]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+116]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+36]
            fld DWORD PTR [ESP+124]
            fmul DWORD PTR [EAX+32]
            fld DWORD PTR [ESP+120]
            fmul DWORD PTR [EAX+20]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+116]
            fmul DWORD PTR [EAX+8]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+340],0
            mov DWORD PTR [ESP+344],0
            fadd DWORD PTR [EDI]
            mov DWORD PTR [ESP+348],0
            mov DWORD PTR [ESP+264],0
            mov DWORD PTR [ESP+268],0
            mov DWORD PTR [ESP+272],0
            fstp DWORD PTR [ESP+48]
            fld DWORD PTR [ESP+36]
            fadd DWORD PTR [EDI+4]
            mov ECX,DWORD PTR [ESP+48]
            mov DWORD PTR [ESP+436],ECX
            mov ECX,DWORD PTR [(_KingBss+491060)]
            fstp DWORD PTR [ESP+52]
            fld DWORD PTR [ESP+40]
            fadd DWORD PTR [EDI+8]
            mov EDX,DWORD PTR [ESP+52]
            mov DWORD PTR [ESP+316],ECX
            mov DWORD PTR [ESP+440],EDX
            mov EDX,DWORD PTR [(_KingBss+491064)]
            fstp DWORD PTR [ESP+56]
            mov EAX,DWORD PTR [ESP+56]
            mov DWORD PTR [ESP+328],ECX
            mov DWORD PTR [ESP+444],EAX
            mov EAX,DWORD PTR [(_KingBss+491056)]
            mov DWORD PTR [ESP+320],EDX
            mov DWORD PTR [ESP+332],EDX
            lea ECX,DWORD PTR [ESP+340]
            mov DWORD PTR [ESP+312],EAX
            mov DWORD PTR [ESP+324],EAX
            lea EDX,DWORD PTR [ESP+312]
            push ECX
            mov ECX,DWORD PTR [ESI+3480]
            lea EAX,DWORD PTR [ESP+476]
            push EDX
            mov EDX,DWORD PTR [ESI+96]
            push EAX
            push EDI
            push 0
            push ECX
            push EDX
            call _Query_4e7566

            mov EBX,EAX
            lea EAX,DWORD PTR [ESP+292]
            lea ECX,DWORD PTR [ESP+352]
            push EAX
            mov EAX,DWORD PTR [ESI+3480]
            lea EDX,DWORD PTR [ESP+468]
            push ECX
            mov ECX,DWORD PTR [ESI+96]
            push EDX
            push EDI
            push 0
            and EBX,1
            push EAX
            push ECX
            mov DWORD PTR [ESP+184],EBX
            call _Query_4e7566

            fld DWORD PTR [ESP+368]
            fsub DWORD PTR [EDI]
            mov EBP,EAX
            add ESP,72
            and EBP,1
            fstp DWORD PTR [ESP+32]
            fld DWORD PTR [ESP+300]
            fsub DWORD PTR [EDI+4]
            mov EDX,DWORD PTR [ESP+32]
            mov DWORD PTR [ESP+180],EBP
            mov DWORD PTR [ESP+444],EDX
            fstp DWORD PTR [ESP+36]
            fld DWORD PTR [ESP+304]
            fsub DWORD PTR [EDI+8]
            fstp DWORD PTR [ESP+40]
            fld DWORD PTR [ESP+308]
            fsub DWORD PTR [EDI]
            mov EAX,DWORD PTR [ESP+36]
            mov ECX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+448],EAX
            mov DWORD PTR [ESP+452],ECX
            fstp DWORD PTR [ESP+32]
            fld DWORD PTR [ESP+312]
            fsub DWORD PTR [EDI+4]
            mov EDX,DWORD PTR [ESP+32]
            test EBX,EBX
            fstp DWORD PTR [ESP+36]
            fld DWORD PTR [ESP+316]
            fsub DWORD PTR [EDI+8]
            mov EAX,DWORD PTR [ESP+36]
            mov DWORD PTR [ESP+516],EDX
            mov DWORD PTR [ESP+520],EAX
            fstp DWORD PTR [ESP+40]
            mov ECX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+524],ECX
            je $L_564558

            lea EDX,DWORD PTR [ESP+444]
            push EDX
            call _Vec3_Length

            add ESP,4
            jmp $L_56455e
$L_564558::

            fld QWORD PTR [(_KingRdata+19720)]
$L_56455e::

            fstp DWORD PTR [ESP+380]
            test EBP,EBP
            je $L_56457b

            lea EAX,DWORD PTR [ESP+516]
            push EAX
            call _Vec3_Length

            add ESP,4
            jmp $L_564581
$L_56457b::

            fld QWORD PTR [(_KingRdata+19720)]
$L_564581::

            mov EAX,DWORD PTR [ESP+268]
            mov ECX,DWORD PTR [ESP+260]
            mov EDX,DWORD PTR [ESP+264]
            mov DWORD PTR [ESP+220],EAX
            mov EAX,DWORD PTR [ESP+132]
            xor EBX,EBX
            fstp DWORD PTR [ESP+388]
            mov DWORD PTR [ESP+172],EAX
            mov EAX,DWORD PTR [ESP+112]
            mov DWORD PTR [ESP+212],ECX
            mov ECX,DWORD PTR [ESP+124]
            mov DWORD PTR [ESP+216],EDX
            mov EDX,DWORD PTR [ESP+128]
            xor EBP,EBP
            cmp EAX,EBX
            mov DWORD PTR [ESP+80],EBX
            mov DWORD PTR [ESP+44],EBP
            mov DWORD PTR [ESP+48],EBP
            mov DWORD PTR [ESP+52],EBP
            mov DWORD PTR [ESP+540],EBP
            mov DWORD PTR [ESP+544],EBP
            mov DWORD PTR [ESP+548],EBP
            mov DWORD PTR [ESP+340],EBP
            mov DWORD PTR [ESP+344],EBP
            mov DWORD PTR [ESP+348],EBP
            mov DWORD PTR [ESP+468],EBP
            mov DWORD PTR [ESP+472],EBP
            mov DWORD PTR [ESP+476],EBP
            mov DWORD PTR [ESP+364],EBP
            mov DWORD PTR [ESP+368],EBP
            mov DWORD PTR [ESP+372],EBP
            mov DWORD PTR [ESP+492],EBP
            mov DWORD PTR [ESP+496],EBP
            mov DWORD PTR [ESP+500],EBP
            mov DWORD PTR [ESP+164],ECX
            mov DWORD PTR [ESP+168],EDX
            mov DWORD PTR [ESP+320],1203982336
            mov DWORD PTR [ESP+272],1203982336
            mov DWORD PTR [ESP+276],1203982336
            mov DWORD PTR [ESP+92],1203982336
            je $L_56486a

            fld DWORD PTR [EDI]
            fsub DWORD PTR [ESP+296]
            fld DWORD PTR [EDI+4]
            fsub DWORD PTR [ESP+300]
            fld DWORD PTR [EDI+8]
            fsub DWORD PTR [ESP+304]
            fld DWORD PTR [ESP+324]
            fmulp ST(3),ST(0)
            fld DWORD PTR [ESP+328]
            fmulp ST(2),ST(0)
            fxch ST(1)
            faddp ST(2),ST(0)
            fmul DWORD PTR [ESP+332]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+288]
            fabs 
            fmul QWORD PTR [(_KingRdata+2112)]
            fadd QWORD PTR [(_KingRdata+19712)]
            fstp DWORD PTR [ESP+56]
            fcom DWORD PTR [ESP+56]
            fnstsw AX
            test AH,65
            jne $L_5646f1

            fstp ST(0)
            fld DWORD PTR [ESP+56]
$L_5646f1::

            fld ST(0)
            fmul DWORD PTR [ESP+324]
            fstp DWORD PTR [ESP+16]
            fld DWORD PTR [ESP+328]
            fmul ST(0),ST(1)
            fstp DWORD PTR [ESP+20]
            fmul DWORD PTR [ESP+332]
            fld DWORD PTR [ESP+16]
            fmul DWORD PTR [(_KingRdata+10808)]
            fstp DWORD PTR [ESP+184]
            fld DWORD PTR [ESP+20]
            fmul DWORD PTR [(_KingRdata+10808)]
            fstp DWORD PTR [ESP+188]
            fmul DWORD PTR [(_KingRdata+10808)]
            fld DWORD PTR [ESP+184]
            fadd DWORD PTR [ESP+296]
            fstp DWORD PTR [ESP+32]
            fld DWORD PTR [ESP+188]
            fadd DWORD PTR [ESP+300]
            mov ECX,DWORD PTR [ESP+32]
            mov DWORD PTR [ESP+16],ECX
            lea ECX,DWORD PTR [ESP+468]
            fstp DWORD PTR [ESP+36]
            mov EDX,DWORD PTR [ESP+36]
            push ECX
            fadd DWORD PTR [ESP+308]
            mov ECX,DWORD PTR [ESI+120]
            mov DWORD PTR [ESP+24],EDX
            lea EDX,DWORD PTR [ESP+276]
            fstp DWORD PTR [ESP+44]
            mov EAX,DWORD PTR [ESP+44]
            push EDX
            mov EDX,DWORD PTR [ESP+296]
            mov DWORD PTR [ESP+32],EAX
            lea EAX,DWORD PTR [ESP+348]
            push EAX
            push ECX
            push OFFSET (_KingBss+491024)
            lea EAX,DWORD PTR [ESP+84]
            push EDX
            push EAX
            lea ECX,DWORD PTR [ESP+352]
            push EDI
            lea EDX,DWORD PTR [ESP+48]
            push ECX
            push EDX
            call _Copter_PlaneCircle

            mov EBP,EAX
            add ESP,40
            test EBP,EBP
            je $L_56486a

            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [(_KingBss+491028)]
            fld DWORD PTR [ESP+68]
            fmul DWORD PTR [(_KingBss+491032)]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [(_KingBss+491032)]
            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [(_KingBss+491024)]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+68]
            fmul DWORD PTR [(_KingBss+491024)]
            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [(_KingBss+491028)]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+340]
            fsub DWORD PTR [EDI]
            fld DWORD PTR [ESP+344]
            fsub DWORD PTR [EDI+4]
            fld DWORD PTR [ESP+348]
            fsub DWORD PTR [EDI+8]
            fstp DWORD PTR [ESP+412]
            fxch ST(4)
            fmulp ST(1),ST(0)
            fld DWORD PTR [ESP+412]
            fmulp ST(2),ST(0)
            fxch ST(1)
            faddp ST(1),ST(0)
            fxch ST(2)
            fmulp ST(1),ST(0)
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+100]
            fcomp DWORD PTR [(_KingRdata+4944)]
            fnstsw AX
            test AH,1
            je $L_56486a

            xor EBP,EBP
            mov DWORD PTR [ESP+272],1203982336
$L_56486a::

            mov EAX,DWORD PTR [ESP+376]
            test EAX,EAX
            je $L_5649aa

            fld DWORD PTR [EDI]
            fsub DWORD PTR [ESP+260]
            fld DWORD PTR [EDI+4]
            fsub DWORD PTR [ESP+264]
            fld DWORD PTR [EDI+8]
            fsub DWORD PTR [ESP+268]
            fld DWORD PTR [ESP+124]
            fmulp ST(3),ST(0)
            fmul DWORD PTR [ESP+132]
            faddp ST(2),ST(0)
            fmul DWORD PTR [ESP+128]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+288]
            fabs 
            fmul QWORD PTR [(_KingRdata+2112)]
            fadd QWORD PTR [(_KingRdata+19712)]
            fstp DWORD PTR [ESP+56]
            fcom DWORD PTR [ESP+56]
            fnstsw AX
            test AH,65
            jne $L_5648d8

            fstp ST(0)
            fld DWORD PTR [ESP+56]
$L_5648d8::

            fld ST(0)
            fmul DWORD PTR [ESP+124]
            fstp DWORD PTR [ESP+16]
            fld DWORD PTR [ESP+128]
            fmul ST(0),ST(1)
            fstp DWORD PTR [ESP+20]
            fmul DWORD PTR [ESP+132]
            fld DWORD PTR [ESP+16]
            fmul DWORD PTR [(_KingRdata+10808)]
            fstp DWORD PTR [ESP+184]
            fld DWORD PTR [ESP+20]
            fmul DWORD PTR [(_KingRdata+10808)]
            fstp DWORD PTR [ESP+188]
            fmul DWORD PTR [(_KingRdata+10808)]
            fld DWORD PTR [ESP+184]
            fadd DWORD PTR [ESP+260]
            fstp DWORD PTR [ESP+32]
            fld DWORD PTR [ESP+188]
            fadd DWORD PTR [ESP+264]
            mov EAX,DWORD PTR [ESP+32]
            mov DWORD PTR [ESP+16],EAX
            lea EAX,DWORD PTR [ESP+540]
            fstp DWORD PTR [ESP+36]
            mov ECX,DWORD PTR [ESP+36]
            push EAX
            fadd DWORD PTR [ESP+272]
            mov EAX,DWORD PTR [ESI+120]
            mov DWORD PTR [ESP+24],ECX
            lea ECX,DWORD PTR [ESP+324]
            fstp DWORD PTR [ESP+44]
            mov EDX,DWORD PTR [ESP+44]
            push ECX
            mov ECX,DWORD PTR [ESP+296]
            mov DWORD PTR [ESP+32],EDX
            lea EDX,DWORD PTR [ESP+52]
            push EDX
            push EAX
            push OFFSET (_KingBss+491024)
            lea EDX,DWORD PTR [ESP+84]
            push ECX
            push EDX
            lea EAX,DWORD PTR [ESP+152]
            push EDI
            lea ECX,DWORD PTR [ESP+48]
            push EAX
            push ECX
            call _Copter_PlaneCircle

            add ESP,40
            mov DWORD PTR [ESP+80],EAX
$L_5649aa::

            mov EAX,DWORD PTR [ESP+180]
            test EAX,EAX
            je $L_564b98

            fld DWORD PTR [EDI]
            fsub DWORD PTR [ESP+308]
            fld DWORD PTR [EDI+4]
            fsub DWORD PTR [ESP+312]
            fld DWORD PTR [EDI+8]
            fsub DWORD PTR [ESP+316]
            fld DWORD PTR [ESP+248]
            fmulp ST(3),ST(0)
            fld DWORD PTR [ESP+252]
            fmulp ST(2),ST(0)
            fxch ST(1)
            faddp ST(2),ST(0)
            fmul DWORD PTR [ESP+256]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+288]
            fabs 
            fmul QWORD PTR [(_KingRdata+2112)]
            fadd QWORD PTR [(_KingRdata+19712)]
            fstp DWORD PTR [ESP+56]
            fcom DWORD PTR [ESP+56]
            fnstsw AX
            test AH,65
            jne $L_564a1f

            fstp ST(0)
            fld DWORD PTR [ESP+56]
$L_564a1f::

            fld ST(0)
            fmul DWORD PTR [ESP+248]
            fstp DWORD PTR [ESP+16]
            fld DWORD PTR [ESP+252]
            fmul ST(0),ST(1)
            fstp DWORD PTR [ESP+20]
            fmul DWORD PTR [ESP+256]
            fld DWORD PTR [ESP+16]
            fmul DWORD PTR [(_KingRdata+10808)]
            fstp DWORD PTR [ESP+184]
            fld DWORD PTR [ESP+20]
            fmul DWORD PTR [(_KingRdata+10808)]
            fstp DWORD PTR [ESP+188]
            fmul DWORD PTR [(_KingRdata+10808)]
            fld DWORD PTR [ESP+184]
            fadd DWORD PTR [ESP+308]
            fstp DWORD PTR [ESP+32]
            fld DWORD PTR [ESP+188]
            fadd DWORD PTR [ESP+312]
            mov EDX,DWORD PTR [ESP+32]
            mov DWORD PTR [ESP+16],EDX
            lea EDX,DWORD PTR [ESP+492]
            fstp DWORD PTR [ESP+36]
            mov EAX,DWORD PTR [ESP+36]
            push EDX
            fadd DWORD PTR [ESP+320]
            mov EDX,DWORD PTR [ESI+120]
            mov DWORD PTR [ESP+24],EAX
            lea EAX,DWORD PTR [ESP+280]
            fstp DWORD PTR [ESP+44]
            mov ECX,DWORD PTR [ESP+44]
            push EAX
            mov EAX,DWORD PTR [ESP+296]
            mov DWORD PTR [ESP+32],ECX
            lea ECX,DWORD PTR [ESP+372]
            push ECX
            push EDX
            push OFFSET (_KingBss+491024)
            lea ECX,DWORD PTR [ESP+84]
            push EAX
            push ECX
            lea EDX,DWORD PTR [ESP+276]
            push EDI
            lea EAX,DWORD PTR [ESP+48]
            push EDX
            push EAX
            call _Copter_PlaneCircle

            mov EBX,EAX
            add ESP,40
            test EBX,EBX
            je $L_564b98

            fld DWORD PTR [ESP+68]
            fmul DWORD PTR [(_KingBss+491032)]
            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [(_KingBss+491028)]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [(_KingBss+491024)]
            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [(_KingBss+491032)]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [(_KingBss+491028)]
            fld DWORD PTR [ESP+68]
            fmul DWORD PTR [(_KingBss+491024)]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+364]
            fsub DWORD PTR [EDI]
            fld DWORD PTR [ESP+368]
            fsub DWORD PTR [EDI+4]
            fld DWORD PTR [ESP+372]
            fsub DWORD PTR [EDI+8]
            fstp DWORD PTR [ESP+412]
            fxch ST(4)
            fmulp ST(1),ST(0)
            fld DWORD PTR [ESP+412]
            fmulp ST(2),ST(0)
            fxch ST(1)
            faddp ST(1),ST(0)
            fxch ST(2)
            fmulp ST(1),ST(0)
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+100]
            fcomp DWORD PTR [(_KingRdata+4944)]
            fnstsw AX
            test AH,1
            je $L_564b98

            xor EBX,EBX
            mov DWORD PTR [ESP+276],1203982336
$L_564b98::

            test EBP,EBP
            jne $L_564bac

            test EBX,EBX
            jne $L_564bac

            mov EAX,DWORD PTR [ESP+80]
            test EAX,EAX
            je $L_564dff
$L_564bac::

            test EBP,EBP
            mov DWORD PTR [ESP+100],1
            je $L_564c5f

            fld DWORD PTR [ESP+272]
            fcomp DWORD PTR [ESP+276]
            fnstsw AX
            test AH,65
            je $L_564c5f

            fld DWORD PTR [ESP+272]
            fcomp DWORD PTR [ESP+320]
            fnstsw AX
            test AH,65
            je $L_564c5f

            mov EAX,DWORD PTR [ESP+344]
            mov ECX,DWORD PTR [ESP+272]
            mov EDX,DWORD PTR [ESP+340]
            mov DWORD PTR [ESP+216],EAX
            mov EAX,DWORD PTR [ESP+328]
            mov DWORD PTR [ESP+92],ECX
            mov ECX,DWORD PTR [ESP+348]
            mov DWORD PTR [ESP+168],EAX
            mov EAX,DWORD PTR [ESP+112]
            mov DWORD PTR [ESP+212],EDX
            mov EDX,DWORD PTR [ESP+324]
            mov DWORD PTR [ESP+220],ECX
            mov ECX,DWORD PTR [ESP+332]
            mov DWORD PTR [ESP+164],EDX
            test EAX,EAX
            mov DWORD PTR [ESP+172],ECX
            jne $L_564d60

            push OFFSET (_KingData+62868)
            jmp $L_564d58
$L_564c5f::

            test EBX,EBX
            je $L_564ced

            fld DWORD PTR [ESP+276]
            fcomp DWORD PTR [ESP+320]
            fnstsw AX
            test AH,65
            je $L_564ced

            mov EAX,DWORD PTR [ESP+364]
            mov EDX,DWORD PTR [ESP+276]
            mov ECX,DWORD PTR [ESP+368]
            mov DWORD PTR [ESP+212],EAX
            mov EAX,DWORD PTR [ESP+248]
            mov DWORD PTR [ESP+92],EDX
            mov EDX,DWORD PTR [ESP+372]
            mov DWORD PTR [ESP+164],EAX
            mov EAX,DWORD PTR [ESP+180]
            mov DWORD PTR [ESP+216],ECX
            mov ECX,DWORD PTR [ESP+252]
            mov DWORD PTR [ESP+220],EDX
            mov EDX,DWORD PTR [ESP+256]
            mov DWORD PTR [ESP+168],ECX
            test EAX,EAX
            mov DWORD PTR [ESP+172],EDX
            jne $L_564d60

            push OFFSET (_KingData+62800)
            jmp $L_564d58
$L_564ced::

            mov EAX,DWORD PTR [ESP+80]
            test EAX,EAX
            je $L_564d60

            mov EAX,DWORD PTR [ESP+320]
            mov ECX,DWORD PTR [ESP+44]
            mov EDX,DWORD PTR [ESP+48]
            mov DWORD PTR [ESP+92],EAX
            mov EAX,DWORD PTR [ESP+52]
            mov DWORD PTR [ESP+212],ECX
            mov ECX,DWORD PTR [ESP+124]
            mov DWORD PTR [ESP+220],EAX
            mov EAX,DWORD PTR [ESP+132]
            mov DWORD PTR [ESP+216],EDX
            mov EDX,DWORD PTR [ESP+128]
            mov DWORD PTR [ESP+172],EAX
            mov EAX,DWORD PTR [ESP+376]
            mov DWORD PTR [ESP+164],ECX
            test EAX,EAX
            mov DWORD PTR [ESP+168],EDX
            jne $L_564d60

            push OFFSET (_KingData+62756)
$L_564d58::

            call _World_Fatal

            add ESP,4
$L_564d60::

            fld DWORD PTR [ESP+92]
            fcomp DWORD PTR [ESP+76]
            fnstsw AX
            test AH,1
            je $L_564dff

            fld DWORD PTR [ESP+92]
            fsub DWORD PTR [ESP+280]
            fdiv DWORD PTR [ESP+336]
            fcom QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_564d9c

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_564db1
$L_564d9c::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_564db1

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+1656)]
$L_564db1::

            fstp DWORD PTR [ESP+84]
            fld DWORD PTR [ESP+92]
            fsub DWORD PTR [ESP+116]
            fdiv DWORD PTR [ESP+96]
            fcom QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_564de0

            mov ECX,DWORD PTR [ESP+100]
            fstp ST(0)
            fld QWORD PTR [(_KingRdata+6728)]
            fstp DWORD PTR [ESP+28]
            jmp $L_564e01
$L_564de0::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_564df5

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+1656)]
$L_564df5::

            fstp DWORD PTR [ESP+28]
            mov ECX,DWORD PTR [ESP+100]
            jmp $L_564e01
$L_564dff::

            xor ECX,ECX
$L_564e01::

            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [ESP+168]
            fld DWORD PTR [ESP+68]
            fmul DWORD PTR [ESP+164]
            mov DWORD PTR [ESP+200],0
            mov DWORD PTR [ESP+204],1072693248
            fsubp ST(1),ST(0)
            fst DWORD PTR [ESP+100]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,1
            jne $L_564e4b

            mov DWORD PTR [ESP+204],3220176896
$L_564e4b::

            test ECX,ECX
            je $L_565b8f

            fld DWORD PTR [ESP+172]
            fabs 
            fcomp QWORD PTR [(_KingRdata+19704)]
            fnstsw AX
            test AH,1
            je $L_565b8f

            fld DWORD PTR [ESP+28]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,1
            je $L_565b8f

            fld DWORD PTR [ESP+168]
            fmul DWORD PTR [ESP+68]
            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [ESP+164]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [ESP+172]
            faddp ST(1),ST(0)
            call __CIacos

            fmul QWORD PTR [ESP+200]
            fabs 
            fcomp QWORD PTR [(_KingRdata+9864)]
            fnstsw AX
            test AH,65
            jne $L_565b8f

            fld DWORD PTR [ESI+120]
            fdiv DWORD PTR [(_KingData+62100)]
            mov ECX,DWORD PTR [ESP+100]
            lea EDX,DWORD PTR [ESP+564]
            push ECX
            mov DWORD PTR [ESP+564],ECX
            fld ST(0)
            fchs 
            fadd DWORD PTR [(_KingBss+427032)]
            fstp DWORD PTR [ESP+60]
            fadd DWORD PTR [(_KingBss+427032)]
            fld DWORD PTR [(_KingBss+427032)]
            fmul DWORD PTR [(_KingBss+427032)]
            fstp DWORD PTR [ESP+152]
            fld DWORD PTR [(_KingBss+427032)]
            fadd ST(0),ST(0)
            fstp DWORD PTR [ESP+80]
            fld ST(0)
            fmul ST(0),ST(1)
            fsub DWORD PTR [ESP+152]
            fmul DWORD PTR [(_KingData+62100)]
            fdiv DWORD PTR [ESP+80]
            fchs 
            fstp DWORD PTR [ESP+104]
            fstp ST(0)
            fld DWORD PTR [ESP+60]
            fmul DWORD PTR [ESP+60]
            fsub DWORD PTR [ESP+152]
            fmul DWORD PTR [(_KingData+62100)]
            fdiv DWORD PTR [ESP+80]
            fstp DWORD PTR [ESP]
            push EDX
            call _Matrix_RotationZ

            mov EAX,DWORD PTR [ESP+108]
            lea ECX,DWORD PTR [ESP+572]
            push EAX
            push ECX
            call _Matrix_RotationZ

            fld DWORD PTR [(_KingBss+427032)]
            fadd ST(0),ST(0)
            lea EAX,DWORD PTR [ESI+100]
            add ESP,16
            mov DWORD PTR [ESP+432],0
            mov DWORD PTR [ESP+436],0
            fdiv DWORD PTR [ESP+60]
            mov EDX,DWORD PTR [EAX]
            mov DWORD PTR [ESP+440],0
            mov DWORD PTR [ESP+136],EDX
            mov DWORD PTR [ESP+528],0
            mov ECX,DWORD PTR [EAX+4]
            mov DWORD PTR [ESP+532],0
            mov DWORD PTR [ESP+140],ECX
            mov DWORD PTR [ESP+536],0
            mov EDX,DWORD PTR [EAX+8]
            mov EAX,EDI
            mov DWORD PTR [ESP+144],EDX
            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [ESP+184],ECX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [ESP+188],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [ESP+192],EAX
            fadd ST(0),ST(0)
            fadd DWORD PTR [(_KingRdata+5200)]
            call __ftol

            fld DWORD PTR [(_KingData+62096)]
            fmul DWORD PTR [ESI+3224]
            mov ECX,DWORD PTR [ESI+120]
            mov EBP,EAX
            mov DWORD PTR [ESP+80],0
            mov DWORD PTR [ESP+28],ECX
            fmul DWORD PTR [ESP+60]
            fld DWORD PTR [ESI+3224]
            fsubr QWORD PTR [(_KingRdata+1648)]
            mov DWORD PTR [ESP+76],0
            mov DWORD PTR [ESP+384],0
            mov DWORD PTR [ESP+92],0
            fdivp ST(1),ST(0)
            fst QWORD PTR [ESP+200]
            fld DWORD PTR [(_KingData+62100)]
            fstp QWORD PTR [ESP+100]
            fcomp QWORD PTR [ESP+100]
            fnstsw AX
            test AH,1
            lea EAX,DWORD PTR [ESP+200]
            jne $L_565074

            lea EAX,DWORD PTR [ESP+100]
$L_565074::

            fld QWORD PTR [EAX]
            fmul QWORD PTR [(_KingRdata+6488)]
            xor EBX,EBX
            fstp DWORD PTR [ESP+148]
            fld DWORD PTR [ESP+28]
            fcomp DWORD PTR [ESP+148]
            fnstsw AX
            test AH,1
            je $L_565272
$L_56509b::

            fld DWORD PTR [ESP+80]
            fadd DWORD PTR [ESP+60]
            lea EAX,DWORD PTR [ESP+564]
            fstp DWORD PTR [ESP+80]
            fld DWORD PTR [(_KingData+62096)]
            fmul DWORD PTR [ESP+60]
            fadd DWORD PTR [ESP+28]
            fst DWORD PTR [ESP+28]
            fmul DWORD PTR [ESP+60]
            fst DWORD PTR [ESP+56]
            fadd DWORD PTR [ESP+76]
            mov EDX,DWORD PTR [ESP+56]
            push EDX
            push EAX
            fstp DWORD PTR [ESP+84]
            call _Matrix_RotationZ

            fld DWORD PTR [ESP+152]
            fmul DWORD PTR [EAX+24]
            fld DWORD PTR [ESP+148]
            fmul DWORD PTR [EAX+12]
            add ESP,8
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+136]
            fmul DWORD PTR [EAX]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+200]
            fld DWORD PTR [ESP+144]
            fmul DWORD PTR [EAX+28]
            fld DWORD PTR [ESP+140]
            fmul DWORD PTR [EAX+16]
            mov ECX,DWORD PTR [ESP+200]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+136]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+204]
            fld DWORD PTR [ESP+144]
            fmul DWORD PTR [EAX+32]
            fld DWORD PTR [ESP+140]
            fmul DWORD PTR [EAX+20]
            mov EDX,DWORD PTR [ESP+204]
            mov DWORD PTR [ESP+140],EDX
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+136]
            fmul DWORD PTR [EAX+8]
            mov DWORD PTR [ESP+136],ECX
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+208]
            fld DWORD PTR [ESP+200]
            fmul DWORD PTR [ESP+60]
            mov EAX,DWORD PTR [ESP+208]
            mov DWORD PTR [ESP+144],EAX
            fstp DWORD PTR [ESP+236]
            fld DWORD PTR [ESP+204]
            fmul DWORD PTR [ESP+60]
            mov ECX,DWORD PTR [ESP+236]
            mov DWORD PTR [ESP+16],ECX
            lea ECX,DWORD PTR [ESP+16]
            fstp DWORD PTR [ESP+240]
            fld DWORD PTR [ESP+208]
            fmul DWORD PTR [ESP+60]
            mov EDX,DWORD PTR [ESP+240]
            push ECX
            mov DWORD PTR [ESP+24],EDX
            lea EDX,DWORD PTR [ESP+396]
            fstp DWORD PTR [ESP+248]
            mov EAX,DWORD PTR [ESP+248]
            push EDX
            lea ECX,DWORD PTR [ESP+192]
            mov DWORD PTR [ESP+32],EAX
            call @Vec3_AddAssign@16

            fld DWORD PTR [ESP+184]
            fsub DWORD PTR [ESP+212]
            fld DWORD PTR [ESP+188]
            fsub DWORD PTR [ESP+216]
            fld DWORD PTR [ESP+192]
            fsub DWORD PTR [ESP+220]
            fmul DWORD PTR [ESP+172]
            fld DWORD PTR [ESP+164]
            fmulp ST(3),ST(0)
            fxch ST(2)
            faddp ST(2),ST(0)
            fmul DWORD PTR [ESP+168]
            faddp ST(1),ST(0)
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,1
            jne $L_5655ea

            cmp EBX,EBP
            jle $L_56525b

            push OFFSET (_KingData+62712)
            call _World_Fatal

            add ESP,4
$L_56525b::

            fld DWORD PTR [ESP+28]
            fcomp DWORD PTR [ESP+148]
            inc EBX
            fnstsw AX
            test AH,1
            jne $L_56509b
$L_565272::

            mov EAX,DWORD PTR [ESP+76]
            lea ECX,DWORD PTR [ESP+564]
            push EAX
            push ECX
            call _Matrix_RotationZ

            fld DWORD PTR [ESP+76]
            fmul DWORD PTR [EAX+16]
            fld DWORD PTR [ESP+80]
            fmul DWORD PTR [EAX+28]
            lea ECX,DWORD PTR [ESP+392]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fadd DWORD PTR [EAX+40]
            fld DWORD PTR [ESP+76]
            fmul DWORD PTR [EAX+20]
            fld DWORD PTR [ESP+80]
            fmul DWORD PTR [EAX+32]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [EAX+8]
            faddp ST(1),ST(0)
            fadd DWORD PTR [EAX+44]
            fstp DWORD PTR [ESP+208]
            fld DWORD PTR [ESP+76]
            fmul DWORD PTR [EAX+12]
            fld DWORD PTR [ESP+80]
            fmul DWORD PTR [EAX+24]
            mov EDX,DWORD PTR [ESP+208]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+72]
            fmul DWORD PTR [EAX]
            faddp ST(1),ST(0)
            fadd DWORD PTR [EAX+36]
            lea EAX,DWORD PTR [ESP+536]
            mov DWORD PTR [ESP+32],EDX
            push EAX
            mov EAX,DWORD PTR [ESP+40]
            lea EDX,DWORD PTR [ESP+444]
            push ECX
            mov ECX,DWORD PTR [ESP+304]
            push EDX
            push EAX
            push OFFSET (_KingBss+491024)
            lea EDX,DWORD PTR [ESP+44]
            push ECX
            lea EAX,DWORD PTR [ESP+216]
            push EDX
            fstp DWORD PTR [ESP+52]
            lea ECX,DWORD PTR [ESP+200]
            push EAX
            fstp DWORD PTR [ESP+60]
            lea EDX,DWORD PTR [ESP+252]
            push ECX
            push EDX
            call _Copter_PlaneCircle

            mov EBX,EAX
            add ESP,48
            test EBX,EBX
            je $L_56535c
$L_565346::

            fld DWORD PTR [ESP+288]
            fmul DWORD PTR [ESP+80]
            fadd DWORD PTR [ESP+384]
            fstp DWORD PTR [ESP+92]
$L_56535c::

            mov EAX,DWORD PTR [ESI+100]
            mov ECX,DWORD PTR [ESI+104]
            fld DWORD PTR [ESP+148]
            mov DWORD PTR [ESP+136],EAX
            mov EAX,DWORD PTR [EDI]
            mov DWORD PTR [ESP+32],EAX
            mov EAX,DWORD PTR [ESI+120]
            fchs 
            fstp DWORD PTR [ESP+200]
            mov EDX,DWORD PTR [ESI+108]
            mov DWORD PTR [ESP+28],EAX
            fld DWORD PTR [ESP+28]
            fcomp DWORD PTR [ESP+200]
            mov DWORD PTR [ESP+140],ECX
            mov ECX,DWORD PTR [EDI+4]
            mov DWORD PTR [ESP+144],EDX
            mov EDX,DWORD PTR [EDI+8]
            xor EDI,EDI
            mov DWORD PTR [ESP+36],ECX
            fnstsw AX
            mov DWORD PTR [ESP+40],EDX
            mov DWORD PTR [ESP+404],0
            test AH,65
            mov DWORD PTR [ESP+408],0
            mov DWORD PTR [ESP+412],0
            mov DWORD PTR [ESP+480],0
            mov DWORD PTR [ESP+484],0
            mov DWORD PTR [ESP+488],0
            mov DWORD PTR [ESP+56],0
            mov DWORD PTR [ESP+76],0
            mov DWORD PTR [ESP+100],0
            mov DWORD PTR [ESP+80],0
            jne $L_565600
$L_56541f::

            fld DWORD PTR [ESP+56]
            fadd DWORD PTR [ESP+60]
            lea EDX,DWORD PTR [ESP+564]
            fstp DWORD PTR [ESP+56]
            fld DWORD PTR [(_KingData+62096)]
            fmul DWORD PTR [ESP+60]
            fsubr DWORD PTR [ESP+28]
            fst DWORD PTR [ESP+28]
            fmul DWORD PTR [ESP+60]
            fst DWORD PTR [ESP+148]
            fadd DWORD PTR [ESP+76]
            mov ECX,DWORD PTR [ESP+148]
            push ECX
            push EDX
            fstp DWORD PTR [ESP+84]
            call _Matrix_RotationZ

            fld DWORD PTR [ESP+152]
            fmul DWORD PTR [EAX+24]
            fld DWORD PTR [ESP+148]
            fmul DWORD PTR [EAX+12]
            add ESP,8
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+136]
            fmul DWORD PTR [EAX]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+236]
            fld DWORD PTR [ESP+144]
            fmul DWORD PTR [EAX+28]
            fld DWORD PTR [ESP+140]
            fmul DWORD PTR [EAX+16]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+136]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+240]
            fld DWORD PTR [ESP+144]
            fmul DWORD PTR [EAX+32]
            fld DWORD PTR [ESP+140]
            fmul DWORD PTR [EAX+20]
            mov ECX,DWORD PTR [ESP+240]
            mov DWORD PTR [ESP+140],ECX
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+136]
            fmul DWORD PTR [EAX+8]
            mov EAX,DWORD PTR [ESP+236]
            mov DWORD PTR [ESP+136],EAX
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+244]
            fld DWORD PTR [ESP+236]
            fmul DWORD PTR [ESP+60]
            mov EDX,DWORD PTR [ESP+244]
            mov DWORD PTR [ESP+144],EDX
            fstp DWORD PTR [ESP+16]
            fld DWORD PTR [ESP+240]
            fmul DWORD PTR [ESP+60]
            mov EAX,DWORD PTR [ESP+16]
            mov DWORD PTR [ESP+392],EAX
            lea EAX,DWORD PTR [ESP+392]
            fstp DWORD PTR [ESP+20]
            fld DWORD PTR [ESP+244]
            fmul DWORD PTR [ESP+60]
            mov ECX,DWORD PTR [ESP+20]
            push EAX
            mov DWORD PTR [ESP+400],ECX
            lea ECX,DWORD PTR [ESP+616]
            fstp DWORD PTR [ESP+28]
            mov EDX,DWORD PTR [ESP+28]
            push ECX
            lea ECX,DWORD PTR [ESP+40]
            mov DWORD PTR [ESP+408],EDX
            call @Vec3_AddAssign@16

            fld DWORD PTR [ESP+32]
            fsub DWORD PTR [ESP+212]
            fld DWORD PTR [ESP+36]
            fsub DWORD PTR [ESP+216]
            fld DWORD PTR [ESP+40]
            fsub DWORD PTR [ESP+220]
            fmul DWORD PTR [ESP+172]
            fld DWORD PTR [ESP+164]
            fmulp ST(3),ST(0)
            fxch ST(2)
            faddp ST(2),ST(0)
            fmul DWORD PTR [ESP+168]
            faddp ST(1),ST(0)
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,1
            jne $L_5655f4

            cmp EDI,EBP
            jle $L_5655cf

            push OFFSET (_KingData+62668)
            call _World_Fatal

            add ESP,4
$L_5655cf::

            fld DWORD PTR [ESP+28]
            fcomp DWORD PTR [ESP+200]
            inc EDI
            fnstsw AX
            test AH,65
            je $L_56541f

            xor EDI,EDI
            jmp $L_565600
$L_5655ea::

            mov EBX,1
            jmp $L_565346
$L_5655f4::

            mov EAX,1
            xor EDI,EDI
            jmp $L_5656d8
$L_565600::

            mov EDX,DWORD PTR [ESP+76]
            lea EAX,DWORD PTR [ESP+564]
            push EDX
            push EAX
            call _Matrix_RotationZ

            fld DWORD PTR [ESP+76]
            fmul DWORD PTR [EAX+16]
            fld DWORD PTR [ESP+80]
            fmul DWORD PTR [EAX+28]
            lea EDX,DWORD PTR [ESP+488]
            push EDX
            mov EDX,DWORD PTR [ESP+40]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+76]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fadd DWORD PTR [EAX+40]
            fld DWORD PTR [ESP+80]
            fmul DWORD PTR [EAX+20]
            fld DWORD PTR [ESP+84]
            fmul DWORD PTR [EAX+32]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+76]
            fmul DWORD PTR [EAX+8]
            faddp ST(1),ST(0)
            fadd DWORD PTR [EAX+44]
            fstp DWORD PTR [ESP+212]
            fld DWORD PTR [ESP+80]
            fmul DWORD PTR [EAX+12]
            fld DWORD PTR [ESP+84]
            fmul DWORD PTR [EAX+24]
            mov ECX,DWORD PTR [ESP+212]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+76]
            fmul DWORD PTR [EAX]
            faddp ST(1),ST(0)
            fadd DWORD PTR [EAX+36]
            mov DWORD PTR [ESP+412],ECX
            lea EAX,DWORD PTR [ESP+112]
            lea ECX,DWORD PTR [ESP+416]
            push EAX
            mov EAX,DWORD PTR [ESP+304]
            push ECX
            push EDX
            push OFFSET (_KingBss+491024)
            lea ECX,DWORD PTR [ESP+420]
            push EAX
            lea EDX,DWORD PTR [ESP+64]
            push ECX
            fstp DWORD PTR [ESP+428]
            lea EAX,DWORD PTR [ESP+200]
            push EDX
            fstp DWORD PTR [ESP+436]
            lea ECX,DWORD PTR [ESP+252]
            push EAX
            push ECX
            call _Copter_PlaneCircle

            add ESP,48
            cmp EAX,EDI
            je $L_5656ef
$L_5656d8::

            fld DWORD PTR [ESP+56]
            fmul DWORD PTR [ESP+288]
            cmp EAX,EDI
            fadd DWORD PTR [ESP+100]
            fstp DWORD PTR [ESP+80]
            jne $L_565768
$L_5656ef::

            cmp EBX,EDI
            je $L_5657df

            fld DWORD PTR [ESP+80]
            fcomp DWORD PTR [ESP+116]
            fnstsw AX
            test AH,1
            je $L_5659f3

            fld DWORD PTR [ESP+80]
            fsub DWORD PTR [ESP+116]
            mov DWORD PTR [ESI+3432],4294967295
            fdiv DWORD PTR [ESP+96]
            fcom QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_565737

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_56574c
$L_565737::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_56574c

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+1656)]
$L_56574c::

            fcom DWORD PTR [ESI+3444]
            fnstsw AX
            test AH,1
            je $L_5659f1

            fstp DWORD PTR [ESI+3444]
            jmp $L_5659f3
$L_565768::

            cmp EBX,EDI
            jne $L_5657df

            fld DWORD PTR [ESP+92]
            fcomp DWORD PTR [ESP+116]
            fnstsw AX
            test AH,1
            je $L_5659f3

            fld DWORD PTR [ESP+92]
            fsub DWORD PTR [ESP+116]
            mov DWORD PTR [ESI+3432],1
            fdiv DWORD PTR [ESP+96]
            fcom QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_5657ac

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_5657c1
$L_5657ac::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_5657c1

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+1656)]
$L_5657c1::

            fchs 
            fcom DWORD PTR [ESI+3444]
            fnstsw AX
            test AH,65
            jne $L_5659f1

            fstp DWORD PTR [ESI+3444]
            jmp $L_5659f3
$L_5657df::

            fld DWORD PTR [(_KingRdata+4708)]
            cmp EAX,EDI
            je $L_5658d8

            cmp EBX,EDI
            je $L_5658d8

            fstp ST(0)
            fld DWORD PTR [ESP+92]
            fcomp DWORD PTR [ESP+80]
            fnstsw AX
            test AH,65
            jne $L_565878

            fld DWORD PTR [ESP+92]
            fsub DWORD PTR [ESP+280]
            mov DWORD PTR [ESI+3432],1
            fdiv DWORD PTR [ESP+336]
            fld DWORD PTR [ESP+92]
            fsub DWORD PTR [ESP+116]
            fdiv DWORD PTR [ESP+96]
            fcom QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_565845

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_56585a
$L_565845::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_56585a

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+1656)]
$L_56585a::

            fchs 
            fcom DWORD PTR [ESI+3444]
            fnstsw AX
            test AH,65
            jne $L_56598b

            fstp DWORD PTR [ESI+3444]
            jmp $L_56598d
$L_565878::

            fld DWORD PTR [ESP+80]
            fsub DWORD PTR [ESP+280]
            mov DWORD PTR [ESI+3432],4294967295
            fdiv DWORD PTR [ESP+336]
            fld DWORD PTR [ESP+80]
            fsub DWORD PTR [ESP+116]
            fdiv DWORD PTR [ESP+96]
            fcom QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_5658ba

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_565976
$L_5658ba::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_565976

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+1656)]
            jmp $L_565976
$L_5658d8::

            cmp DWORD PTR [ESP+376],EDI
            je $L_56598d

            fld DWORD PTR [ESP+160]
            fcomp DWORD PTR [ESP+116]
            fnstsw AX
            test AH,1
            je $L_56598d

            fstp ST(0)
            fld DWORD PTR [ESP+416]
            fld DWORD PTR [ESP+152]
            fcom QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_565922

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_565937
$L_565922::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_565937

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+1656)]
$L_565937::

            fld DWORD PTR [ESP+560]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,65
            jne $L_56596c

            fchs 
            fcom DWORD PTR [ESI+3444]
            mov DWORD PTR [ESI+3432],1
            fnstsw AX
            test AH,65
            jne $L_56598b

            fstp DWORD PTR [ESI+3444]
            jmp $L_56598d
$L_56596c::

            mov DWORD PTR [ESI+3432],4294967295
$L_565976::

            fcom DWORD PTR [ESI+3444]
            fnstsw AX
            test AH,1
            je $L_56598b

            fstp DWORD PTR [ESI+3444]
            jmp $L_56598d
$L_56598b::

            fstp ST(0)
$L_56598d::

            fcom QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_5659a4

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_5659b9
$L_5659a4::

            fcom QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_5659b9

            fstp ST(0)
            fld QWORD PTR [(_KingRdata+1656)]
$L_5659b9::

            fst DWORD PTR [ESP+84]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,1
            je $L_5659f3

            fld DWORD PTR [ESP+88]
            call __ftol

            fld DWORD PTR [ESP+84]
            fmul DWORD PTR [ESP+88]
            mov DWORD PTR [ESI+3436],EAX
            mov DWORD PTR [ESP+120],1
            fstp DWORD PTR [ESI+3448]
            jmp $L_5659f3
$L_5659f1::

            fstp ST(0)
$L_5659f3::

            cmp DWORD PTR [ESP+112],EDI
            je $L_565af8

            cmp DWORD PTR [ESP+180],EDI
            je $L_565af8

            fld DWORD PTR [ESP+380]
            fcomp DWORD PTR [ESP+160]
            fnstsw AX
            test AH,1
            je $L_565af8

            fld DWORD PTR [ESP+388]
            fcomp DWORD PTR [ESP+160]
            fnstsw AX
            test AH,1
            je $L_565af8

            fld DWORD PTR [ESP+332]
            fabs 
            fcomp QWORD PTR [(_KingRdata+19704)]
            fnstsw AX
            test AH,1
            je $L_565af8

            fld DWORD PTR [ESP+256]
            fabs 
            fcomp QWORD PTR [(_KingRdata+19704)]
            fnstsw AX
            test AH,1
            je $L_565af8

            fld QWORD PTR [(_KingRdata+19696)]
            fsin 
            fld ST(0)
            fmul DWORD PTR [ESP+380]
            fmul DWORD PTR [(_KingRdata+19688)]
            fcomp DWORD PTR [ESP+160]
            fnstsw AX
            test AH,65
            jne $L_565ab2

            fmul DWORD PTR [ESP+388]
            fmul DWORD PTR [(_KingRdata+19688)]
            fcomp DWORD PTR [ESP+160]
            fnstsw AX
            test AH,65
            je $L_565ac9

            jmp $L_565ab4
$L_565ab2::

            fstp ST(0)
$L_565ab4::

            fld DWORD PTR [ESP+160]
            fcomp DWORD PTR [ESP+196]
            fnstsw AX
            test AH,1
            je $L_565af8
$L_565ac9::

            mov EDX,DWORD PTR [ESI+3432]
            mov ECX,DWORD PTR [ESP+260]
            lea EAX,DWORD PTR [ESI+3492]
            mov DWORD PTR [ESI+3484],EDX
            mov EDX,DWORD PTR [ESP+264]
            mov DWORD PTR [EAX],ECX
            mov ECX,DWORD PTR [ESP+268]
            mov DWORD PTR [EAX+4],EDX
            mov DWORD PTR [EAX+8],ECX
$L_565af8::

            mov ECX,DWORD PTR [ESP+212]
            lea EDX,DWORD PTR [ESI+3184]
            mov EAX,DWORD PTR [ESI+3432]
            mov DWORD PTR [EDX],ECX
            mov ECX,DWORD PTR [ESP+216]
            cmp EAX,EDI
            mov DWORD PTR [ESI+3208],EAX
            mov DWORD PTR [EDX+4],ECX
            mov ECX,DWORD PTR [ESP+220]
            mov DWORD PTR [EDX+8],ECX
            mov ECX,DWORD PTR [ESP+164]
            lea EDX,DWORD PTR [ESI+3196]
            mov DWORD PTR [ESI+3196],ECX
            mov ECX,DWORD PTR [ESP+168]
            mov DWORD PTR [EDX+4],ECX
            mov ECX,DWORD PTR [ESP+172]
            mov DWORD PTR [EDX+8],ECX
            jne $L_565de1

            cmp DWORD PTR [ESP+120],EDI
            jne $L_565de1

            cmp DWORD PTR [ESP+176],EDI
            jne $L_565de1

            fld DWORD PTR [ESP+84]
            fmul DWORD PTR [ESP+88]
            fstp DWORD PTR [ESI+3448]
            fld DWORD PTR [ESP+88]
            call __ftol

            mov DWORD PTR [ESI+3436],EAX
            jmp $L_565de7
$L_565b8f::

            mov EAX,DWORD PTR [ESI+3484]
            xor EDI,EDI
            cmp EAX,EDI
            jne $L_565bab

            cmp DWORD PTR [ESI+3208],EDI
            je $L_565de7

            cmp EAX,EDI
            je $L_565c17
$L_565bab::

            cmp DWORD PTR [ESP+180],EDI
            je $L_565c11

            cmp DWORD PTR [ESP+112],EDI
            je $L_565c11

            fld DWORD PTR [ESI+72]
            fmul DWORD PTR [ESI+3500]
            fld DWORD PTR [ESI+60]
            fmul DWORD PTR [ESI+3496]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESI+3492]
            fmul DWORD PTR [ESI+48]
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESI+84]
            fld DWORD PTR [ESI+52]
            fmul DWORD PTR [ESI+3492]
            fld DWORD PTR [ESI+76]
            fmul DWORD PTR [ESI+3500]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESI+64]
            fmul DWORD PTR [ESI+3496]
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESI+88]
            fxch ST(1)
            fabs 
            fmul QWORD PTR [(_KingRdata+1824)]
            fcompp 
            fnstsw AX
            test AH,65
            jne $L_565c17
$L_565c11::

            mov DWORD PTR [ESI+3484],EDI
$L_565c17::

            mov EAX,DWORD PTR [ESI+3484]
            cmp EAX,EDI
            mov DWORD PTR [ESP+152],EAX
            je $L_565c46

            fild DWORD PTR [ESP+152]
            mov DWORD PTR [ESI+3432],EAX
            fstp DWORD PTR [ESI+3444]
$L_565c3b::

            mov DWORD PTR [ESI+3208],EDI
            jmp $L_565de7
$L_565c46::

            mov EAX,DWORD PTR [ESI+3208]
            cmp EAX,EDI
            mov DWORD PTR [ESP+152],EAX
            je $L_565c3b

            fld DWORD PTR [(_KingRdata+4708)]
            fld DWORD PTR [ESI+3200]
            fmul DWORD PTR [ESP+68]
            fld DWORD PTR [ESI+3204]
            fmul DWORD PTR [ESP+72]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+64]
            fmul DWORD PTR [ESI+3196]
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+120]
            fcomp DWORD PTR [(_KingRdata+19684)]
            fnstsw AX
            test AH,1
            je $L_565dd7

            fild DWORD PTR [ESP+152]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fld DWORD PTR [ESI+120]
            fnstsw AX
            test AH,65
            jne $L_565d10

            fcomp DWORD PTR [(_KingRdata+4868)]
            fnstsw AX
            test AH,1
            je $L_565cd4

            fstp ST(0)
            fld DWORD PTR [(_KingRdata+4868)]
            fsub DWORD PTR [ESI+120]
            fdiv DWORD PTR [(_KingData+62100)]
            fadd QWORD PTR [(_KingRdata+1464)]
            jmp $L_565d76
$L_565cd4::

            fld DWORD PTR [ESI+120]
            fcomp DWORD PTR [(_KingRdata+4920)]
            fnstsw AX
            test AH,65
            jne $L_565cfd

            fstp ST(0)
            fld DWORD PTR [(_KingRdata+4920)]
            fsub DWORD PTR [ESI+120]
            fdiv DWORD PTR [(_KingData+62100)]
            fsub QWORD PTR [(_KingRdata+1464)]
            jmp $L_565d76
$L_565cfd::

            fld DWORD PTR [ESP+120]
            fcomp DWORD PTR [(_KingRdata+19680)]
            fnstsw AX
            test AH,65
            jne $L_565d76

            jmp $L_565d70
$L_565d10::

            fcomp DWORD PTR [(_KingRdata+9872)]
            fnstsw AX
            test AH,65
            jne $L_565d36

            fstp ST(0)
            fld DWORD PTR [(_KingRdata+9872)]
            fsub DWORD PTR [ESI+120]
            fdiv DWORD PTR [(_KingData+62100)]
            fsub QWORD PTR [(_KingRdata+1464)]
            jmp $L_565d76
$L_565d36::

            fld DWORD PTR [ESI+120]
            fcomp DWORD PTR [(_KingRdata+10872)]
            fnstsw AX
            test AH,1
            je $L_565d5f

            fstp ST(0)
            fld DWORD PTR [(_KingRdata+10872)]
            fsub DWORD PTR [ESI+120]
            fdiv DWORD PTR [(_KingData+62100)]
            fadd QWORD PTR [(_KingRdata+1464)]
            jmp $L_565d76
$L_565d5f::

            fld DWORD PTR [ESP+120]
            fcomp DWORD PTR [(_KingRdata+19680)]
            fnstsw AX
            test AH,65
            jne $L_565d76
$L_565d70::

            mov DWORD PTR [ESI+3208],EDI
$L_565d76::

            mov ECX,DWORD PTR [ESI+3208]
            cmp ECX,EDI
            jle $L_565d8f

            fcom DWORD PTR [ESI+3444]
            fnstsw AX
            test AH,65
            je $L_565d9e

            cmp ECX,EDI
$L_565d8f::

            jge $L_565de5

            fcom DWORD PTR [ESI+3444]
            fnstsw AX
            test AH,1
            je $L_565de5
$L_565d9e::

            fst DWORD PTR [ESI+3444]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            test AH,65
            jne $L_565dc4

            fld QWORD PTR [(_KingRdata+1648)]
            call __ftol

            mov DWORD PTR [ESI+3432],EAX
            jmp $L_565de7
$L_565dc4::

            fld QWORD PTR [(_KingRdata+6728)]
            call __ftol

            mov DWORD PTR [ESI+3432],EAX
            jmp $L_565de7
$L_565dd7::

            fstp ST(0)
            mov DWORD PTR [ESI+3208],EDI
            jmp $L_565de7
$L_565de1::

            xor EDI,EDI
            jmp $L_565de7
$L_565de5::

            fstp ST(0)
$L_565de7::

            mov EAX,DWORD PTR [ESI+3440]
            pop EBP
            cmp EAX,EDI
            pop EBX
            jne $L_565fc8

            mov EAX,DWORD PTR [ESI+3460]
            cmp EAX,EDI
            mov DWORD PTR [ESP+144],EAX
            jle $L_565fc8

            fld DWORD PTR [ESI+44]
            fld DWORD PTR [ESI+3180]
            fild DWORD PTR [ESP+144]
            fmul QWORD PTR [(_KingRdata+1464)]
            faddp ST(1),ST(0)
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+88]
            fld DWORD PTR [ESP+52]
            fmul DWORD PTR [ESI+108]
            fabs 
            fld DWORD PTR [ESI+3220]
            fsubr QWORD PTR [(_KingRdata+1648)]
            fdivr QWORD PTR [(_KingRdata+1648)]
            fmulp ST(1),ST(0)
            fadd QWORD PTR [(_KingRdata+1464)]
            fstp DWORD PTR [ESP+80]
            fld DWORD PTR [ESP+88]
            fcomp DWORD PTR [ESP+80]
            fnstsw AX
            test AH,65
            jne $L_565e68

            fld QWORD PTR [(_KingRdata+6728)]
            jmp $L_565e8b
$L_565e68::

            fld DWORD PTR [ESP+80]
            fchs 
            fcomp DWORD PTR [ESP+88]
            fnstsw AX
            test AH,65
            jne $L_565e81

            fld QWORD PTR [(_KingRdata+1648)]
            jmp $L_565e8b
$L_565e81::

            fld DWORD PTR [ESP+88]
            fdiv DWORD PTR [ESP+80]
            fchs 
$L_565e8b::

            fstp DWORD PTR [ESI+3452]
            fld DWORD PTR [ESP+88]
            fmul DWORD PTR [ESI+108]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,1
            je $L_565efb

            fld DWORD PTR [ESP+88]
            fabs 
            fld DWORD PTR [ESP+80]
            fld ST(1)
            fcompp 
            fnstsw AX
            test AH,1
            je $L_565ef9

            fld DWORD PTR [ESP+80]
            fmul DWORD PTR [(_KingRdata+4924)]
            fld ST(1)
            fcompp 
            fnstsw AX
            test AH,1
            je $L_565ed9

            fstp ST(0)
            mov DWORD PTR [ESI+3452],EDI
            jmp $L_565efb
$L_565ed9::

            fld DWORD PTR [ESP+80]
            fdivr ST(0),ST(1)
            fsub QWORD PTR [(_KingRdata+19672)]
            fld DWORD PTR [ESP+88]
            fchs 
            fmulp ST(1),ST(0)
            fmul QWORD PTR [(_KingRdata+19664)]
            fstp DWORD PTR [ESI+3452]
$L_565ef9::

            fstp ST(0)
$L_565efb::

            mov EAX,DWORD PTR [ESI+3236]
            cmp EAX,3
            je $L_565f0f

            cmp EAX,11
            jne $L_565fc8
$L_565f0f::

            call _rand

            mov DWORD PTR [ESP+144],EAX
            fild DWORD PTR [ESP+144]
            fmul DWORD PTR [(_KingRdata+4692)]
            fsubr QWORD PTR [(_KingRdata+1760)]
            fstp QWORD PTR [ESP+144]
            call _Game_FrameTime

            fadd ST(0),ST(0)
            fmul QWORD PTR [ESP+144]
            fadd DWORD PTR [ESI+3340]
            fst DWORD PTR [ESI+3340]
            fcomp QWORD PTR [(_KingRdata+1648)]
            fnstsw AX
            test AH,65
            jne $L_565f62

            mov DWORD PTR [ESI+3340],EDI
$L_565f62::

            fld DWORD PTR [ESI+3340]
            fcomp QWORD PTR [(_KingRdata+6728)]
            fnstsw AX
            test AH,1
            je $L_565f7b

            mov DWORD PTR [ESI+3340],EDI
$L_565f7b::

            call _Game_FrameTime

            fmul DWORD PTR [ESI+3340]
            push OFFSET (_KingData+62940)
            fadd DWORD PTR [ESI+3336]
            fst DWORD PTR [ESP+148]
            mov EDX,DWORD PTR [ESP+148]
            fstp DWORD PTR [ESI+3336]
            push EDX
            call _Angle_F2Pi

            fst DWORD PTR [ESI+3336]
            fsin 
            add ESP,8
            fmul DWORD PTR [(_KingData+62120)]
            fadd DWORD PTR [ESI+3452]
            fstp DWORD PTR [ESI+3452]
$L_565fc8::

            pop EDI
            pop ESI
            add ESP,608
            ret 
          BYTE 090H
          BYTE 090H

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
$L_56364a ENDP
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
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
; helicopters: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; helicopters: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
; main loop: src\engine\main_loop.cpp, nmake ASM_MAIN_LOOP=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; helicopters: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; helicopters: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
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
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; helicopters: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; helicopters: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; CKingApp: src\truck\king_app.cpp, nmake ASM_KING_APP=1 assembles the original
ALIGN 16
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
ALIGN 16
; copter: src\truck\helicopter.cpp, nmake ASM_HELICOPTER=1 assembles the original
_TEXT ENDS
END
