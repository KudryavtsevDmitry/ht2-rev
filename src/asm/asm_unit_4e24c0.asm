; asm_unit_4e24c0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Clock_Start@8:PROC
EXTERN @Clock_Stop@8:PROC
EXTERN @Matrix_IdentityThis@8:PROC
EXTERN @Node_Init@8:PROC
EXTERN @Obj4eb150_Kind@8:PROC
EXTERN @Obj_Flag20@8:PROC
EXTERN @SceneNode5f98c0_Draw@16:PROC
EXTERN @Sound_Play@20:PROC
EXTERN @Sound_Restart@16:PROC
EXTERN @Vec3_SetThis@20:PROC
EXTERN _Angle_F2Pi:PROC
EXTERN _Frame_Apply:PROC
EXTERN _Frame_GetWorld:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _Light_GetColor:PROC
EXTERN _Light_SetColor:PROC
EXTERN _Matrix_FromPose:PROC
EXTERN _Matrix_Translation:PROC
EXTERN _Obj_TransformVecs:PROC
EXTERN _Objects_Find:PROC
EXTERN _OldCrt_fprintf:PROC
EXTERN _Query_4e52fc:PROC
EXTERN _Query_4e5415:PROC
EXTERN _Query_4e54b0:PROC
EXTERN _Query_4e560a:PROC
EXTERN _Query_4e58ca:PROC
EXTERN _Query_4e6621:PROC
EXTERN _Query_4e6d41:PROC
EXTERN _Query_4e72ef:PROC
EXTERN _Query_Parts1:PROC
EXTERN _Query_Parts2:PROC
EXTERN _Scene_FindObject:PROC
EXTERN _Switch_Select:PROC
EXTERN _Vec3_Add:PROC
EXTERN _VehLog_Open:PROC
EXTERN _WarnLog_Close:PROC
EXTERN _WarnLog_Open:PROC
EXTERN _World_Fatal:PROC
EXTERN _World_ObjFatal:PROC
EXTERN __imp__InitCommonControls@0:PROC
EXTERN _atan2:NEAR
EXTERN _cos:NEAR
EXTERN _fabs:NEAR
EXTERN _sin:NEAR
EXTERN _sprintf:PROC
EXTERN _sqrt:NEAR
EXTERN _strcpy:NEAR

PUBLIC $L_4e2518, $L_4e281f, $L_4e3528, $L_4e352f, $L_4e4968, $L_4e4a25, $L_4e4a42, $L_4e4a53
PUBLIC $L_4e4a82, $L_4e4a9f, $L_4e4ab0, $L_4e4b4e, $L_4e4bb1, $L_4e4bc8, $L_4e4bdf, $L_4e4bf6
PUBLIC $L_4e4c0a, $L_4e4d96, $L_4e4dec, $L_4e4e2e, $L_4e4f03, $L_4e4f1a, $L_4e4f31, $L_4e4f48
PUBLIC $L_4e4f60, $L_4e4f74, $L_4e5294, $L_4e5296, $L_4e52b7, $L_4e52f5, $L_4e540f, $L_4e5411
PUBLIC $L_4e5600, $L_4e57e2, $L_4e57e4, $L_4e58c4, $L_4e58c6, $L_4e5a3b, $L_4e5b1c, $L_4e5b1e
PUBLIC $L_4e5f45, $L_4e673f, $L_4e69e7, $L_4e69e9, $L_4e6d39, $L_4e6d3b, $L_4e6e60, $L_4e746c
PUBLIC $L_4e76ac, $L_4e7741, $L_4e7744, $L_4e78cc, $L_4e7902, $L_4e794b, $L_4e9e79, $L_4e9e95
PUBLIC $L_4ea329, $L_4ea33b, $L_4ea33d, $L_4ea363

_TEXT SEGMENT
ALIGN 8
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
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 4
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e281c PROC

            add ESP,4
$L_4e281f::

            pop EBP
            ret 
$L_4e281c ENDP
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 16
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 4
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 4
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e3076 PROC

            push EBP
            mov EBP,ESP
            sub ESP,48
            mov EAX,DWORD PTR [EBP+12]
            fld DWORD PTR [EAX]
            fstp QWORD PTR [EBP-48]
            mov ECX,DWORD PTR [EBP+12]
            fld DWORD PTR [ECX+4]
            fstp QWORD PTR [EBP-40]
            mov EDX,DWORD PTR [EBP+12]
            fld DWORD PTR [EDX+8]
            fstp QWORD PTR [EBP-32]
            mov EAX,DWORD PTR [EBP+12]
            fld DWORD PTR [EAX+12]
            fstp QWORD PTR [EBP-24]
            mov ECX,DWORD PTR [EBP+12]
            fld DWORD PTR [ECX+16]
            fstp QWORD PTR [EBP-16]
            mov EDX,DWORD PTR [EBP+12]
            fld DWORD PTR [EDX+20]
            fstp QWORD PTR [EBP-8]
            mov EAX,DWORD PTR [EBP-4]
            push EAX
            mov ECX,DWORD PTR [EBP-8]
            push ECX
            mov EDX,DWORD PTR [EBP-12]
            push EDX
            mov EAX,DWORD PTR [EBP-16]
            push EAX
            mov ECX,DWORD PTR [EBP-20]
            push ECX
            mov EDX,DWORD PTR [EBP-24]
            push EDX
            mov EAX,DWORD PTR [EBP-28]
            push EAX
            mov ECX,DWORD PTR [EBP-32]
            push ECX
            mov EDX,DWORD PTR [EBP-36]
            push EDX
            mov EAX,DWORD PTR [EBP-40]
            push EAX
            mov ECX,DWORD PTR [EBP-44]
            push ECX
            mov EDX,DWORD PTR [EBP-48]
            push EDX
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _Matrix_FromPose

            add ESP,52
            mov EAX,DWORD PTR [EBP+8]
            mov ESP,EBP
            pop EBP
            ret 
$L_4e3076 ENDP
ALIGN 4
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 2
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
$L_4e322a PROC

            push EBP
            mov EBP,ESP
            sub ESP,84
            push ESI
            push EDI
            lea ECX,DWORD PTR [EBP-48]
            call @Matrix_IdentityThis@8

            mov EAX,DWORD PTR [EBP+16]
            push EAX
            mov ECX,DWORD PTR [EBP+12]
            push ECX
            lea EDX,DWORD PTR [EBP-60]
            push EDX
            call _Vec3_Add

            add ESP,12
            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [EBP-48],ECX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [EBP-44],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [EBP-40],EAX
            mov ECX,DWORD PTR [EBP+16]
            add ECX,12
            push ECX
            mov EDX,DWORD PTR [EBP+12]
            add EDX,12
            push EDX
            lea EAX,DWORD PTR [EBP-72]
            push EAX
            call _Vec3_Add

            add ESP,12
            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [EBP-36],ECX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [EBP-32],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [EBP-28],EAX
            mov ECX,DWORD PTR [EBP+16]
            add ECX,24
            push ECX
            mov EDX,DWORD PTR [EBP+12]
            add EDX,24
            push EDX
            lea EAX,DWORD PTR [EBP-84]
            push EAX
            call _Vec3_Add

            add ESP,12
            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [EBP-24],ECX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [EBP-20],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [EBP-16],EAX
            mov ECX,DWORD PTR [(_KingBss+491056)]
            mov DWORD PTR [EBP-12],ECX
            mov EDX,DWORD PTR [(_KingBss+491060)]
            mov DWORD PTR [EBP-8],EDX
            mov EAX,DWORD PTR [(_KingBss+491064)]
            mov DWORD PTR [EBP-4],EAX
            mov ECX,12
            lea ESI,DWORD PTR [EBP-48]
            mov EDI,DWORD PTR [EBP+8]

            rep movsd 

            mov EAX,DWORD PTR [EBP+8]
            pop EDI
            pop ESI
            mov ESP,EBP
            pop EBP
            ret 
$L_4e322a ENDP
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
$L_4e3497 PROC

            push EBP
            mov EBP,ESP
            sub ESP,12
            mov EAX,DWORD PTR [EBP+20]
            push EAX
            mov ECX,DWORD PTR [EBP+16]
            push ECX
            mov EDX,DWORD PTR [EBP+12]
            push EDX
            lea ECX,DWORD PTR [EBP-12]
            call @Vec3_SetThis@20

            sub ESP,12
            mov ECX,ESP
            mov EDX,DWORD PTR [EAX]
            mov DWORD PTR [ECX],EDX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [ECX+4],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [ECX+8],EAX
            mov ECX,DWORD PTR [EBP+8]
            push ECX
            call _Matrix_Translation

            add ESP,16
            mov EAX,DWORD PTR [EBP+8]
            mov ESP,EBP
            pop EBP
            ret 

            push EBP
            mov EBP,ESP
            push ECX
            fld DWORD PTR [EBP+8]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _fabs

            add ESP,8
            fmul QWORD PTR [(_KingRdata+9024)]
            fld DWORD PTR [EBP+12]
            fcompp 
            fnstsw AX
            test AH,65
            jne $L_4e3528

            fld DWORD PTR [EBP+16]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _fabs

            add ESP,8
            fadd ST(0),ST(0)
            fld DWORD PTR [EBP+12]
            fcompp 
            fnstsw AX
            test AH,65
            jne $L_4e3528

            mov DWORD PTR [EBP-4],1
            jmp $L_4e352f
$L_4e3528::

            mov DWORD PTR [EBP-4],0
$L_4e352f::

            mov AX,WORD PTR [EBP-4]
            mov ESP,EBP
            pop EBP
            ret 
$L_4e3497 ENDP
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e4364 PROC

            push EBP
            mov EBP,ESP
            push ECX
            mov DWORD PTR [EBP-4],ECX
            mov EAX,DWORD PTR [EBP-4]
            mov ECX,DWORD PTR [EAX+92]
            push ECX
            mov EDX,DWORD PTR [EBP-4]
            mov EAX,DWORD PTR [EDX+88]
            push EAX
            call _Switch_Select

            add ESP,8
            mov ECX,DWORD PTR [EBP+12]
            push ECX
            mov EDX,DWORD PTR [EBP+8]
            push EDX
            mov ECX,DWORD PTR [EBP-4]
            call @SceneNode5f98c0_Draw@16

            mov ESP,EBP
            pop EBP
            ret 8

            push EBP
            mov EBP,ESP
            push ECX
            mov DWORD PTR [EBP-4],ECX
            mov EAX,DWORD PTR [EBP-4]
            mov ECX,DWORD PTR [EAX+96]
            push ECX
            mov EDX,DWORD PTR [EBP-4]
            mov EAX,DWORD PTR [EDX+88]
            push EAX
            call _Switch_Select

            add ESP,8
            mov ECX,DWORD PTR [EBP-4]
            mov EDX,DWORD PTR [ECX+100]
            push EDX
            mov EAX,DWORD PTR [EBP-4]
            mov ECX,DWORD PTR [EAX+92]
            push ECX
            call _Switch_Select

            add ESP,8
            mov EDX,DWORD PTR [EBP+12]
            push EDX
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            mov ECX,DWORD PTR [EBP-4]
            call @SceneNode5f98c0_Draw@16

            mov ESP,EBP
            pop EBP
            ret 8

            push EBP
            mov EBP,ESP
            push ECX
            mov DWORD PTR [EBP-4],ECX
            mov EAX,DWORD PTR [EBP-4]
            mov ECX,DWORD PTR [EAX+156]
            push ECX
            mov EDX,DWORD PTR [EBP-4]
            mov EAX,DWORD PTR [EDX+88]
            push EAX
            call _Switch_Select

            add ESP,8
            mov ECX,DWORD PTR [EBP-4]
            mov EDX,DWORD PTR [ECX+160]
            push EDX
            mov EAX,DWORD PTR [EBP-4]
            mov ECX,DWORD PTR [EAX+92]
            push ECX
            call _Switch_Select

            add ESP,8
            mov EDX,DWORD PTR [EBP-4]
            mov EAX,DWORD PTR [EDX+164]
            push EAX
            mov ECX,DWORD PTR [EBP-4]
            mov EDX,DWORD PTR [ECX+96]
            push EDX
            call _Switch_Select

            add ESP,8
            mov EAX,DWORD PTR [EBP-4]
            add EAX,104
            push EAX
            mov ECX,DWORD PTR [EBP-4]
            mov EDX,DWORD PTR [ECX+152]
            push EDX
            mov EAX,DWORD PTR [EBP-4]
            mov ECX,DWORD PTR [EAX+100]
            push ECX
            call _Frame_Apply

            add ESP,12
            mov EDX,DWORD PTR [EBP+12]
            push EDX
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            mov ECX,DWORD PTR [EBP-4]
            call @SceneNode5f98c0_Draw@16

            mov ESP,EBP
            pop EBP
            ret 8
$L_4e4364 ENDP
ALIGN 8
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 16
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 4
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e4965 PROC

            add ESP,8
$L_4e4968::

            mov EDX,DWORD PTR [EBP+12]
            push EDX
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _Frame_GetWorld

            add ESP,8
            pop EBP
            ret 
$L_4e4965 ENDP
ALIGN 2
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e4a0a PROC

            push EBP
            mov EBP,ESP
            push ECX
            cmp DWORD PTR [EBP+8],0
            jne $L_4e4a25

            push OFFSET (_KingData+31740)
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _World_ObjFatal

            add ESP,8
$L_4e4a25::

            mov ECX,DWORD PTR [EBP+8]
            call @Obj4eb150_Kind@8

            mov DWORD PTR [EBP-4],EAX
            cmp DWORD PTR [EBP-4],12
            jne $L_4e4a42

            cmp DWORD PTR [EBP-4],20
            jne $L_4e4a42

            cmp DWORD PTR [EBP-4],11
            je $L_4e4a53
$L_4e4a42::

            push OFFSET (_KingData+31756)
            mov ECX,DWORD PTR [EBP+8]
            push ECX
            call _World_ObjFatal

            add ESP,8
$L_4e4a53::

            mov EDX,DWORD PTR [EBP+12]
            push EDX
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _Light_SetColor

            add ESP,8
            mov ESP,EBP
            pop EBP
            ret 

            push EBP
            mov EBP,ESP
            push ECX
            cmp DWORD PTR [EBP+8],0
            jne $L_4e4a82

            push OFFSET (_KingData+31792)
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _World_ObjFatal

            add ESP,8
$L_4e4a82::

            mov ECX,DWORD PTR [EBP+8]
            call @Obj4eb150_Kind@8

            mov DWORD PTR [EBP-4],EAX
            cmp DWORD PTR [EBP-4],12
            jne $L_4e4a9f

            cmp DWORD PTR [EBP-4],20
            jne $L_4e4a9f

            cmp DWORD PTR [EBP-4],11
            je $L_4e4ab0
$L_4e4a9f::

            push OFFSET (_KingData+31804)
            mov ECX,DWORD PTR [EBP+8]
            push ECX
            call _World_ObjFatal

            add ESP,8
$L_4e4ab0::

            mov EDX,DWORD PTR [EBP+12]
            push EDX
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _Light_GetColor

            add ESP,8
            mov ESP,EBP
            pop EBP
            ret 
$L_4e4a0a ENDP
ALIGN 4
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e4b3d PROC

            push EBP
            mov EBP,ESP
            sub ESP,12
            cmp DWORD PTR [EBP+8],0
            jne $L_4e4b4e

            jmp $L_4e4c0a
$L_4e4b4e::

            mov EAX,DWORD PTR [EBP+16]
            fld DWORD PTR [EAX+4]
            sub ESP,8
            fstp QWORD PTR [ESP]
            mov ECX,DWORD PTR [EBP+16]
            fld DWORD PTR [ECX]
            sub ESP,8
            fstp QWORD PTR [ESP]
            call _atan2

            add ESP,16
            fchs 
            fdiv QWORD PTR [(_KingRdata+10496)]
            fadd QWORD PTR [(_KingRdata+1648)]
            fdiv QWORD PTR [(_KingRdata+1632)]
            fstp DWORD PTR [EBP-8]
            mov DWORD PTR [EBP-12],0
            fld DWORD PTR [(_KingRdata+4688)]
            fsub DWORD PTR [EBP-8]
            fmul DWORD PTR [EBP+12]
            fstp DWORD PTR [EBP-4]
            fld DWORD PTR [EBP-12]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,1
            je $L_4e4bb1

            mov DWORD PTR [EBP-12],0
$L_4e4bb1::

            fld DWORD PTR [EBP-4]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,1
            je $L_4e4bc8

            mov DWORD PTR [EBP-4],0
$L_4e4bc8::

            fld DWORD PTR [EBP-12]
            fcomp DWORD PTR [(_KingRdata+4688)]
            fnstsw AX
            test AH,65
            jne $L_4e4bdf

            mov DWORD PTR [EBP-12],1065353216
$L_4e4bdf::

            fld DWORD PTR [EBP-4]
            fcomp DWORD PTR [(_KingRdata+4688)]
            fnstsw AX
            test AH,65
            jne $L_4e4bf6

            mov DWORD PTR [EBP-4],1065353216
$L_4e4bf6::

            mov EDX,DWORD PTR [EBP+20]
            push EDX
            mov EAX,DWORD PTR [EBP-4]
            push EAX
            mov ECX,DWORD PTR [EBP-12]
            push ECX
            mov ECX,DWORD PTR [EBP+8]
            call @Sound_Play@20
$L_4e4c0a::

            mov ESP,EBP
            pop EBP
            ret 
$L_4e4b3d ENDP
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
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
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 16
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 16
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e527f PROC

            push EBP
            mov EBP,ESP
            cmp DWORD PTR [EBP+8],0
            je $L_4e5294

            mov ECX,DWORD PTR [EBP+8]
            call @Obj_Flag20@8

            test EAX,EAX
            je $L_4e5296
$L_4e5294::

            jmp $L_4e52b7
$L_4e5296::

            mov ECX,DWORD PTR [(_KingBss+52388)]
            call @Clock_Start@8

            mov EAX,DWORD PTR [EBP+8]
            mov EDX,DWORD PTR [EAX]
            mov ECX,DWORD PTR [EBP+8]
            call DWORD PTR [EDX+40]

            mov ECX,DWORD PTR [(_KingBss+52388)]
            call @Clock_Stop@8
$L_4e52b7::

            pop EBP
            ret 

            push EBP
            mov EBP,ESP
            sub ESP,56
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _Objects_Find

            add ESP,4
            mov DWORD PTR [EBP-4],EAX
            cmp DWORD PTR [EBP-4],0
            jne $L_4e52f5

            mov ECX,DWORD PTR [EBP+8]
            push ECX
            push OFFSET (_KingData+31920)
            lea EDX,DWORD PTR [EBP-56]
            push EDX
            call _sprintf

            add ESP,12
            lea EAX,DWORD PTR [EBP-56]
            push EAX
            call _World_Fatal

            add ESP,4
$L_4e52f5::

            mov EAX,DWORD PTR [EBP-4]
            mov ESP,EBP
            pop EBP
            ret 
$L_4e527f ENDP
ALIGN 4
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e538b PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            lea EAX,DWORD PTR [EBP-208]
            push EAX
            mov ECX,DWORD PTR [EBP+16]
            push ECX
            mov EDX,DWORD PTR [EBP+12]
            push EDX
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _Query_4e52fc

            add ESP,16
            mov DWORD PTR [EBP-212],EAX
            mov ECX,DWORD PTR [EBP-212]
            and ECX,1
            test ECX,ECX
            je $L_4e540f

            mov EDX,DWORD PTR [EBP+20]
            mov EAX,DWORD PTR [EBP-156]
            mov DWORD PTR [EDX],EAX
            mov ECX,DWORD PTR [EBP-152]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EBP-148]
            mov DWORD PTR [EDX+8],EAX
            mov ECX,DWORD PTR [EBP+24]
            mov EDX,DWORD PTR [EBP-144]
            mov DWORD PTR [ECX],EDX
            mov EAX,DWORD PTR [EBP-140]
            mov DWORD PTR [ECX+4],EAX
            mov EDX,DWORD PTR [EBP-136]
            mov DWORD PTR [ECX+8],EDX
            mov EAX,DWORD PTR [EBP-212]
            jmp $L_4e5411
$L_4e540f::

            xor EAX,EAX
$L_4e5411::

            mov ESP,EBP
            pop EBP
            ret 
$L_4e538b ENDP
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 16
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e556d PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            lea EAX,DWORD PTR [EBP-208]
            push EAX
            mov ECX,DWORD PTR [EBP+24]
            push ECX
            mov EDX,DWORD PTR [EBP+20]
            push EDX
            mov EAX,DWORD PTR [EBP+16]
            push EAX
            mov ECX,DWORD PTR [EBP+12]
            push ECX
            mov EDX,DWORD PTR [EBP+8]
            push EDX
            call _Query_4e54b0

            add ESP,24
            mov DWORD PTR [EBP-212],EAX
            mov EAX,DWORD PTR [EBP-212]
            and EAX,1
            test EAX,EAX
            je $L_4e5600

            lea ECX,DWORD PTR [EBP-208]
            push ECX
            call _Obj_TransformVecs

            add ESP,4
            mov EDX,DWORD PTR [EBP+28]
            mov EAX,DWORD PTR [EBP-156]
            mov DWORD PTR [EDX],EAX
            mov ECX,DWORD PTR [EBP-152]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EBP-148]
            mov DWORD PTR [EDX+8],EAX
            mov ECX,DWORD PTR [EBP+32]
            mov EDX,DWORD PTR [EBP-144]
            mov DWORD PTR [ECX],EDX
            mov EAX,DWORD PTR [EBP-140]
            mov DWORD PTR [ECX+4],EAX
            mov EDX,DWORD PTR [EBP-136]
            mov DWORD PTR [ECX+8],EDX
$L_4e5600::

            mov EAX,DWORD PTR [EBP-212]
            mov ESP,EBP
            pop EBP
            ret 
$L_4e556d ENDP
ALIGN 2
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 8
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e574f PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            lea EAX,DWORD PTR [EBP-208]
            push EAX
            mov ECX,DWORD PTR [EBP+16]
            push ECX
            mov EDX,DWORD PTR [EBP+12]
            push EDX
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _Query_4e52fc

            add ESP,16
            mov DWORD PTR [EBP-212],EAX
            mov ECX,DWORD PTR [EBP-212]
            and ECX,1
            test ECX,ECX
            je $L_4e57e2

            lea EDX,DWORD PTR [EBP-208]
            push EDX
            call _Obj_TransformVecs

            add ESP,4
            mov EAX,DWORD PTR [EBP+20]
            mov ECX,DWORD PTR [EBP-156]
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [EBP-152]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [EBP-148]
            mov DWORD PTR [EAX+8],ECX
            mov EDX,DWORD PTR [EBP+24]
            mov EAX,DWORD PTR [EBP-144]
            mov DWORD PTR [EDX],EAX
            mov ECX,DWORD PTR [EBP-140]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EBP-136]
            mov DWORD PTR [EDX+8],EAX
            mov EAX,DWORD PTR [EBP-212]
            jmp $L_4e57e4
$L_4e57e2::

            xor EAX,EAX
$L_4e57e4::

            mov ESP,EBP
            pop EBP
            ret 
$L_4e574f ENDP
ALIGN 8
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e582e PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            lea EAX,DWORD PTR [EBP-208]
            push EAX
            mov ECX,DWORD PTR [EBP+20]
            push ECX
            mov EDX,DWORD PTR [EBP+16]
            push EDX
            mov EAX,DWORD PTR [EBP+12]
            push EAX
            mov ECX,DWORD PTR [EBP+8]
            push ECX
            call _Query_4e5415

            add ESP,20
            mov DWORD PTR [EBP-212],EAX
            mov EDX,DWORD PTR [EBP-212]
            and EDX,1
            test EDX,EDX
            je $L_4e58c4

            lea EAX,DWORD PTR [EBP-208]
            push EAX
            call _Obj_TransformVecs

            add ESP,4
            mov ECX,DWORD PTR [EBP+24]
            mov EDX,DWORD PTR [EBP-156]
            mov DWORD PTR [ECX],EDX
            mov EAX,DWORD PTR [EBP-152]
            mov DWORD PTR [ECX+4],EAX
            mov EDX,DWORD PTR [EBP-148]
            mov DWORD PTR [ECX+8],EDX
            mov EAX,DWORD PTR [EBP+28]
            mov ECX,DWORD PTR [EBP-144]
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [EBP-140]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [EBP-136]
            mov DWORD PTR [EAX+8],ECX
            mov EAX,1
            jmp $L_4e58c6
$L_4e58c4::

            xor EAX,EAX
$L_4e58c6::

            mov ESP,EBP
            pop EBP
            ret 
$L_4e582e ENDP
ALIGN 2
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 4
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e59a8 PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            mov EAX,DWORD PTR [EBP+32]
            push EAX
            lea ECX,DWORD PTR [EBP-208]
            push ECX
            mov EDX,DWORD PTR [EBP+20]
            push EDX
            mov EAX,DWORD PTR [EBP+16]
            push EAX
            mov ECX,DWORD PTR [EBP+12]
            push ECX
            mov EDX,DWORD PTR [EBP+8]
            push EDX
            call _Query_4e58ca

            add ESP,24
            mov DWORD PTR [EBP-212],EAX
            mov EAX,DWORD PTR [EBP-212]
            and EAX,1
            test EAX,EAX
            je $L_4e5a3b

            lea ECX,DWORD PTR [EBP-208]
            push ECX
            call _Obj_TransformVecs

            add ESP,4
            mov EDX,DWORD PTR [EBP+24]
            mov EAX,DWORD PTR [EBP-156]
            mov DWORD PTR [EDX],EAX
            mov ECX,DWORD PTR [EBP-152]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EBP-148]
            mov DWORD PTR [EDX+8],EAX
            mov ECX,DWORD PTR [EBP+28]
            mov EDX,DWORD PTR [EBP-144]
            mov DWORD PTR [ECX],EDX
            mov EAX,DWORD PTR [EBP-140]
            mov DWORD PTR [ECX+4],EAX
            mov EDX,DWORD PTR [EBP-136]
            mov DWORD PTR [ECX+8],EDX
$L_4e5a3b::

            mov EAX,DWORD PTR [EBP-212]
            mov ESP,EBP
            pop EBP
            ret 
$L_4e59a8 ENDP
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e5a85 PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            mov EAX,DWORD PTR [EBP+28]
            push EAX
            lea ECX,DWORD PTR [EBP-208]
            push ECX
            mov EDX,DWORD PTR [EBP+16]
            push EDX
            mov EAX,DWORD PTR [EBP+12]
            push EAX
            mov ECX,DWORD PTR [EBP+8]
            push ECX
            call _Query_4e560a

            add ESP,20
            mov DWORD PTR [EBP-212],EAX
            mov EDX,DWORD PTR [EBP-212]
            and EDX,1
            test EDX,EDX
            je $L_4e5b1c

            lea EAX,DWORD PTR [EBP-208]
            push EAX
            call _Obj_TransformVecs

            add ESP,4
            mov ECX,DWORD PTR [EBP+20]
            mov EDX,DWORD PTR [EBP-156]
            mov DWORD PTR [ECX],EDX
            mov EAX,DWORD PTR [EBP-152]
            mov DWORD PTR [ECX+4],EAX
            mov EDX,DWORD PTR [EBP-148]
            mov DWORD PTR [ECX+8],EDX
            mov EAX,DWORD PTR [EBP+24]
            mov ECX,DWORD PTR [EBP-144]
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [EBP-140]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [EBP-136]
            mov DWORD PTR [EAX+8],ECX
            mov EAX,DWORD PTR [EBP-212]
            jmp $L_4e5b1e
$L_4e5b1c::

            xor EAX,EAX
$L_4e5b1e::

            mov ESP,EBP
            pop EBP
            ret 
$L_4e5a85 ENDP
ALIGN 2
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 16
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 8
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e5f42 PROC

            add ESP,4
$L_4e5f45::

            mov ECX,DWORD PTR [EBP-92]
            add ECX,4
            push ECX
            lea EDX,DWORD PTR [EBP-88]
            push EDX
            call _strcpy

            add ESP,8
            lea EAX,DWORD PTR [EBP-88]
            push EAX
            push OFFSET (_KingData+32144)
            lea ECX,DWORD PTR [EBP-52]
            push ECX
            call _sprintf

            add ESP,12
            lea EDX,DWORD PTR [EBP-52]
            push EDX
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _Scene_FindObject

            add ESP,8
            mov DWORD PTR [EBP-56],EAX
            mov EAX,DWORD PTR [EBP-56]
            mov ESP,EBP
            pop EBP
            ret 
$L_4e5f42 ENDP
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 16
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 8
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e66bf PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            lea EAX,DWORD PTR [EBP-208]
            push EAX
            mov ECX,DWORD PTR [EBP+20]
            push ECX
            mov EDX,DWORD PTR [EBP+16]
            push EDX
            mov EAX,DWORD PTR [EBP+12]
            push EAX
            mov ECX,DWORD PTR [EBP+8]
            push ECX
            call _Query_4e6621

            add ESP,20
            mov DWORD PTR [EBP-212],EAX
            mov EDX,DWORD PTR [EBP-212]
            and EDX,1
            test EDX,EDX
            je $L_4e673f

            mov EAX,DWORD PTR [EBP+24]
            mov ECX,DWORD PTR [EBP-156]
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [EBP-152]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [EBP-148]
            mov DWORD PTR [EAX+8],ECX
            mov EDX,DWORD PTR [EBP+28]
            mov EAX,DWORD PTR [EBP-144]
            mov DWORD PTR [EDX],EAX
            mov ECX,DWORD PTR [EBP-140]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EBP-136]
            mov DWORD PTR [EDX+8],EAX
$L_4e673f::

            mov EAX,DWORD PTR [EBP-212]
            mov ESP,EBP
            pop EBP
            ret 
$L_4e66bf ENDP
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e6922 PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            push ESI
            push EDI
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            mov ECX,12
            mov ESI,OFFSET (_KingBss+490960)
            lea EDI,DWORD PTR [EBP-204]

            rep movsd 

            mov EAX,DWORD PTR [EBP+16]
            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [EBP-132],ECX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [EBP-128],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [EBP-124],EAX
            mov ECX,DWORD PTR [EBP+20]
            mov EDX,DWORD PTR [ECX]
            mov DWORD PTR [EBP-120],EDX
            mov EAX,DWORD PTR [ECX+4]
            mov DWORD PTR [EBP-116],EAX
            mov ECX,DWORD PTR [ECX+8]
            mov DWORD PTR [EBP-112],ECX
            mov EDX,DWORD PTR [EBP+12]
            mov DWORD PTR [EBP-208],EDX
            lea EAX,DWORD PTR [EBP-208]
            push EAX
            push 6
            mov ECX,DWORD PTR [EBP+8]
            push ECX
            call _Query_Parts2

            add ESP,12
            mov DWORD PTR [EBP-212],EAX
            mov EDX,DWORD PTR [EBP-212]
            and EDX,1
            test EDX,EDX
            je $L_4e69e7

            mov EAX,DWORD PTR [EBP+24]
            mov ECX,DWORD PTR [EBP-156]
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [EBP-152]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [EBP-148]
            mov DWORD PTR [EAX+8],ECX
            mov EDX,DWORD PTR [EBP+28]
            mov EAX,DWORD PTR [EBP-144]
            mov DWORD PTR [EDX],EAX
            mov ECX,DWORD PTR [EBP-140]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EBP-136]
            mov DWORD PTR [EDX+8],EAX
            mov EAX,1
            jmp $L_4e69e9
$L_4e69e7::

            xor EAX,EAX
$L_4e69e9::

            pop EDI
            pop ESI
            mov ESP,EBP
            pop EBP
            ret 
$L_4e6922 ENDP
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 4
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e6c5f PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            push ESI
            push EDI
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            mov ECX,12
            mov ESI,OFFSET (_KingBss+490960)
            lea EDI,DWORD PTR [EBP-204]

            rep movsd 

            mov EAX,DWORD PTR [EBP+16]
            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [EBP-132],ECX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [EBP-128],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [EBP-124],EAX
            mov ECX,DWORD PTR [EBP+20]
            mov EDX,DWORD PTR [ECX]
            mov DWORD PTR [EBP-120],EDX
            mov EAX,DWORD PTR [ECX+4]
            mov DWORD PTR [EBP-116],EAX
            mov ECX,DWORD PTR [ECX+8]
            mov DWORD PTR [EBP-112],ECX
            mov EDX,DWORD PTR [EBP+12]
            mov DWORD PTR [EBP-208],EDX
            mov DWORD PTR [EBP-92],0
            mov DWORD PTR [EBP-104],0
            mov DWORD PTR [EBP-88],0
            lea EAX,DWORD PTR [EBP-208]
            push EAX
            push 6
            mov ECX,DWORD PTR [EBP+8]
            push ECX
            call _Query_Parts1

            add ESP,12
            mov DWORD PTR [EBP-212],EAX
            mov EDX,DWORD PTR [EBP-212]
            and EDX,1
            test EDX,EDX
            je $L_4e6d39

            mov EAX,DWORD PTR [EBP+24]
            mov ECX,DWORD PTR [EBP-156]
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [EBP-152]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [EBP-148]
            mov DWORD PTR [EAX+8],ECX
            mov EDX,DWORD PTR [EBP+28]
            mov EAX,DWORD PTR [EBP-144]
            mov DWORD PTR [EDX],EAX
            mov ECX,DWORD PTR [EBP-140]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EBP-136]
            mov DWORD PTR [EDX+8],EAX
            mov EAX,1
            jmp $L_4e6d3b
$L_4e6d39::

            xor EAX,EAX
$L_4e6d3b::

            pop EDI
            pop ESI
            mov ESP,EBP
            pop EBP
            ret 
$L_4e6c5f ENDP
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e6ddc PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            lea EAX,DWORD PTR [EBP-208]
            push EAX
            mov ECX,DWORD PTR [EBP+24]
            push ECX
            mov EDX,DWORD PTR [EBP+20]
            push EDX
            mov EAX,DWORD PTR [EBP+16]
            push EAX
            mov ECX,DWORD PTR [EBP+12]
            push ECX
            mov EDX,DWORD PTR [EBP+8]
            push EDX
            call _Query_4e6d41

            add ESP,24
            mov DWORD PTR [EBP-212],EAX
            mov EAX,DWORD PTR [EBP-212]
            and EAX,1
            test EAX,EAX
            je $L_4e6e60

            mov ECX,DWORD PTR [EBP+28]
            mov EDX,DWORD PTR [EBP-156]
            mov DWORD PTR [ECX],EDX
            mov EAX,DWORD PTR [EBP-152]
            mov DWORD PTR [ECX+4],EAX
            mov EDX,DWORD PTR [EBP-148]
            mov DWORD PTR [ECX+8],EDX
            mov EAX,DWORD PTR [EBP+32]
            mov ECX,DWORD PTR [EBP-144]
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [EBP-140]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [EBP-136]
            mov DWORD PTR [EAX+8],ECX
$L_4e6e60::

            mov EAX,DWORD PTR [EBP-212]
            mov ESP,EBP
            pop EBP
            ret 
$L_4e6ddc ENDP
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 8
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e73e8 PROC

            push EBP
            mov EBP,ESP
            sub ESP,212
            lea ECX,DWORD PTR [EBP-208]
            call @Node_Init@8

            lea EAX,DWORD PTR [EBP-208]
            push EAX
            mov ECX,DWORD PTR [EBP+24]
            push ECX
            mov EDX,DWORD PTR [EBP+20]
            push EDX
            mov EAX,DWORD PTR [EBP+16]
            push EAX
            mov ECX,DWORD PTR [EBP+12]
            push ECX
            mov EDX,DWORD PTR [EBP+8]
            push EDX
            call _Query_4e72ef

            add ESP,24
            mov DWORD PTR [EBP-212],EAX
            mov EAX,DWORD PTR [EBP-212]
            and EAX,1
            test EAX,EAX
            je $L_4e746c

            mov ECX,DWORD PTR [EBP+28]
            mov EDX,DWORD PTR [EBP-156]
            mov DWORD PTR [ECX],EDX
            mov EAX,DWORD PTR [EBP-152]
            mov DWORD PTR [ECX+4],EAX
            mov EDX,DWORD PTR [EBP-148]
            mov DWORD PTR [ECX+8],EDX
            mov EAX,DWORD PTR [EBP+32]
            mov ECX,DWORD PTR [EBP-144]
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [EBP-140]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [EBP-136]
            mov DWORD PTR [EAX+8],ECX
$L_4e746c::

            mov EAX,DWORD PTR [EBP-212]
            mov ESP,EBP
            pop EBP
            ret 
$L_4e73e8 ENDP
ALIGN 2
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e7699 PROC

            push EBP
            mov EBP,ESP
            push ECX
            push ESI
            push EDI
            cmp DWORD PTR [EBP+8],0
            jne $L_4e76ac

            xor EAX,EAX
            jmp $L_4e7744
$L_4e76ac::

            mov EDI,DWORD PTR [EBP+20]
            add EDI,4
            mov ECX,12
            mov ESI,OFFSET (_KingBss+490960)

            rep movsd 

            mov EAX,DWORD PTR [EBP+12]
            mov ECX,DWORD PTR [EBP+20]
            add ECX,76
            mov EDX,DWORD PTR [EAX]
            mov DWORD PTR [ECX],EDX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [ECX+4],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [ECX+8],EAX
            mov ECX,DWORD PTR [EBP+16]
            mov EDX,DWORD PTR [EBP+20]
            add EDX,88
            mov EAX,DWORD PTR [ECX]
            mov DWORD PTR [EDX],EAX
            mov EAX,DWORD PTR [ECX+4]
            mov DWORD PTR [EDX+4],EAX
            mov ECX,DWORD PTR [ECX+8]
            mov DWORD PTR [EDX+8],ECX
            mov EDX,DWORD PTR [EBP+20]
            mov DWORD PTR [EDX+116],0
            mov EAX,DWORD PTR [EBP+20]
            mov DWORD PTR [EAX+104],0
            mov ECX,DWORD PTR [EBP+20]
            mov DWORD PTR [ECX],0
            mov EDX,DWORD PTR [EBP+20]
            mov DWORD PTR [EDX+100],0
            mov EAX,DWORD PTR [EBP+20]
            push EAX
            push 4
            mov ECX,DWORD PTR [EBP+8]
            mov EDX,DWORD PTR [ECX]
            mov ECX,DWORD PTR [EBP+8]
            call DWORD PTR [EDX+52]

            mov DWORD PTR [EBP-4],EAX
            mov EAX,DWORD PTR [EBP-4]
            and EAX,1
            test EAX,EAX
            je $L_4e7741

            mov ECX,DWORD PTR [EBP+20]
            push ECX
            call _Obj_TransformVecs

            add ESP,4
$L_4e7741::

            mov EAX,DWORD PTR [EBP-4]
$L_4e7744::

            pop EDI
            pop ESI
            mov ESP,EBP
            pop EBP
            ret 
$L_4e7699 ENDP
ALIGN 2
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
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
; src/vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
; src/vehicle_physics.cpp, nmake ASM_VEHICLE_PHYSICS=1 assembles the original
ALIGN 4
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e9d13 PROC

            push EBP
            mov EBP,ESP
            call _WarnLog_Open

            mov DWORD PTR [(_KingBss+52404)],1
            pop EBP
            ret 
$L_4e9d13 ENDP
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 16
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 16
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4e9e64 PROC

            push EBP
            mov EBP,ESP
            mov EAX,DWORD PTR [EBP+8]
            push EAX
            call _VehLog_Open

            add ESP,4
            test EAX,EAX
            jne $L_4e9e79

            jmp $L_4e9e95
$L_4e9e79::

            mov ECX,DWORD PTR [EBP+12]
            push ECX
            push OFFSET (_KingData+33076)
            mov EDX,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EDX*4+(_KingBss+52576)]
            push EAX
            call _OldCrt_fprintf

            add ESP,12
$L_4e9e95::

            pop EBP
            ret 
$L_4e9e64 ENDP
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
$L_4ea318 PROC

            push EBP
            mov EBP,ESP
            cmp DWORD PTR [(_KingBss+52404)],0
            je $L_4ea329

            call _WarnLog_Open
$L_4ea329::

            cmp DWORD PTR [(_KingBss+52400)],0
            je $L_4ea33b

            cmp DWORD PTR [(_KingBss+52616)],0
            jne $L_4ea33d
$L_4ea33b::

            jmp $L_4ea363
$L_4ea33d::

            mov EAX,DWORD PTR [EBP+8]
            push EAX
            push OFFSET (_KingData+33256)
            mov ECX,DWORD PTR [(_KingBss+52616)]
            push ECX
            call _OldCrt_fprintf

            add ESP,12
            cmp DWORD PTR [(_KingBss+52404)],0
            je $L_4ea363

            call _WarnLog_Close
$L_4ea363::

            pop EBP
            ret 
$L_4ea318 ENDP
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 8
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
; world: src\truck\world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 2
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
ALIGN 8
; src/world.cpp, nmake ASM_WORLD=1 assembles the original
_TEXT ENDS
END
