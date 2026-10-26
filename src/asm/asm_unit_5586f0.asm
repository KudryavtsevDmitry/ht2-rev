; asm_unit_5586f0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Body_FrameIntegrate@12:PROC
EXTERN @Body_Get9508@8:PROC
EXTERN @Body_GetVec9012@12:PROC
EXTERN @Body_GetVec9024@12:PROC
EXTERN @Body_GetVec9048@12:PROC
EXTERN @Body_GetVec9084@12:PROC
EXTERN @Body_GetVec9144@12:PROC
EXTERN @Body_Integrate@16:PROC
EXTERN @Body_Matrix2274@12:PROC
EXTERN @Body_Matrix22a4@12:PROC
EXTERN @Body_SetSpin@20:PROC
EXTERN @Body_SetVec9012@20:PROC
EXTERN @Body_StateStep@4:PROC
EXTERN @Car_PlaceWithTrailer@16:PROC
EXTERN @Engine_AddContact@20:PROC
EXTERN @Engine_AddPush@16:PROC
EXTERN @Hitch_Align@16:PROC
EXTERN @Trailer_SaddleLoad@12:PROC
EXTERN @Vec3_AddAssign@16:PROC
EXTERN @Vec3_MulAssign@16:PROC
EXTERN @Vec3_SetThis@20:PROC
EXTERN @Vec3_ZeroThis@8:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _Vec3_Add:PROC
EXTERN _Vec3_Cross:PROC
EXTERN _Vec3_DivComponents:PROC
EXTERN _Vec3_DivScalar:PROC
EXTERN _Vec3_Length:PROC
EXTERN _Vec3_Negated:PROC
EXTERN _Vec3_Normalized:PROC
EXTERN _Vec3_RotateBy:PROC
EXTERN _Vec3_Scaled:PROC

PUBLIC $L_559198, $L_5591c4, $L_5591f0, $L_55921c, $L_5593d8, $L_5593e0, $L_559478, $L_55948c
PUBLIC $L_559a76, $L_559ad4, $L_559b71, $L_559cb9, $L_559cee, $L_559d40, $L_559d42, $L_559e6e
PUBLIC $L_55a029, $L_55a07d, $L_55a081, $L_55a129, $L_55a2c0, $L_55a30b, $L_55a30f, $L_55a50c
PUBLIC $L_55a5ee, $L_55a5f0, $L_55a8af, $L_55a8b2, $L_55a8ef, $L_55a937, $L_55a93d, $L_55a966
PUBLIC $L_55ac40, $L_55ae6b, $L_55aef4, $L_55af0d, $L_55af31, $L_55af4c, $L_55af76, $L_55afa1
PUBLIC $L_55afaf, $L_55b094, $L_55b17c, $L_55b180, $L_55b1db, $L_55b1e9, $L_55b4f2, $L_55b4fe
PUBLIC $L_55b561, $L_55b56e

_TEXT SEGMENT
FUN_558f20 PROC

            sub ESP,2308
            push EBX
            push EBP
            mov EBX,ECX
            push ESI
            push EDI
            mov ECX,12
            mov EAX,DWORD PTR [EBX+10420]
            lea ESI,DWORD PTR [EBX+708]
            lea EDI,DWORD PTR [ESP+148]
            mov EBP,DWORD PTR [EBX+21880]

            rep movsd 

            fld DWORD PTR [ESP+164]
            lea ECX,DWORD PTR [EAX+EAX*2]
            lea ESI,DWORD PTR [EBP+708]
            lea EDI,DWORD PTR [ESP+148]
            lea ECX,DWORD PTR [ECX+ECX*4]
            lea ECX,DWORD PTR [ECX+ECX*4]
            lea EAX,DWORD PTR [EAX+ECX*2]
            mov ECX,12
            shl EAX,4
            fmul DWORD PTR [EAX+(_KingBss+55164)+160]
            fld DWORD PTR [ESP+152]
            fmul DWORD PTR [EAX+(_KingBss+55164)+156]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+176]
            fmul DWORD PTR [EAX+(_KingBss+55164)+164]
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESP+188]
            fld DWORD PTR [ESP+156]
            fmul DWORD PTR [EAX+(_KingBss+55164)+156]
            fld DWORD PTR [ESP+180]
            fmul DWORD PTR [EAX+(_KingBss+55164)+164]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+168]
            fmul DWORD PTR [EAX+(_KingBss+55164)+160]
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESP+192]
            fstp DWORD PTR [ESP+220]
            fld DWORD PTR [ESP+172]
            fmul DWORD PTR [EAX+(_KingBss+55164)+164]
            fld DWORD PTR [ESP+160]
            fmul DWORD PTR [EAX+(_KingBss+55164)+160]
            mov EDX,DWORD PTR [ESP+220]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+148]
            fmul DWORD PTR [EAX+(_KingBss+55164)+156]
            mov DWORD PTR [ESP+204],EDX
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESP+184]
            fstp DWORD PTR [ESP+196]
            fstp DWORD PTR [ESP+200]
            fld DWORD PTR [EAX+(_KingBss+55164)+204]
            mov EAX,DWORD PTR [EBP+10420]
            fld ST(0)
            fmul DWORD PTR [(_KingBss+491008)]

            rep movsd 

            fstp DWORD PTR [ESP+260]
            lea ECX,DWORD PTR [EAX+EAX*2]
            fld ST(0)
            fmul DWORD PTR [(_KingBss+491012)]
            lea ECX,DWORD PTR [ECX+ECX*4]
            lea ECX,DWORD PTR [ECX+ECX*4]
            fstp DWORD PTR [ESP+264]
            lea EAX,DWORD PTR [EAX+ECX*2]
            fmul DWORD PTR [(_KingBss+491016)]
            fld DWORD PTR [ESP+260]
            shl EAX,4
            add EAX,OFFSET (_KingBss+55164)+168
            fadd DWORD PTR [EAX]
            fstp DWORD PTR [ESP+100]
            fld DWORD PTR [ESP+264]
            fadd DWORD PTR [EAX+4]
            fstp DWORD PTR [ESP+104]
            fadd DWORD PTR [EAX+8]
            fld DWORD PTR [ESP+148]
            fmul DWORD PTR [ESP+100]
            fld ST(1)
            fmul DWORD PTR [ESP+172]
            lea EDX,DWORD PTR [ESP+196]
            lea EAX,DWORD PTR [ESP+1652]
            push EDX
            push EAX
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+112]
            fmul DWORD PTR [ESP+168]
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESP+192]
            fstp DWORD PTR [ESP+216]
            fld DWORD PTR [ESP+108]
            fmul DWORD PTR [ESP+160]
            fld ST(1)
            fmul DWORD PTR [ESP+184]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+112]
            fmul DWORD PTR [ESP+172]
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESP+196]
            fstp DWORD PTR [ESP+220]
            fld DWORD PTR [ESP+108]
            fmul DWORD PTR [ESP+164]
            fxch ST(1)
            fmul DWORD PTR [ESP+188]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+112]
            fmul DWORD PTR [ESP+176]
            faddp ST(1),ST(0)
            fadd DWORD PTR [ESP+200]
            fstp DWORD PTR [ESP+224]
            call _Vec3_Normalized

            lea ECX,DWORD PTR [ESP+216]
            lea EDX,DWORD PTR [ESP+1648]
            push ECX
            push EDX
            call _Vec3_Normalized

            mov EAX,DWORD PTR [EBX+21600]
            add ESP,16
            xor ECX,ECX
            lea ESI,DWORD PTR [ESP+284]
            mov EDX,DWORD PTR [EAX+13124]
            mov EDI,4
            test EDX,EDX
            mov EDX,DWORD PTR [EBP+21600]
            sete CL
            mov DWORD PTR [ESP+32],ECX
            mov ECX,DWORD PTR [EDX+13124]
            mov EDX,DWORD PTR [EBP+3284]
            xor EAX,EAX
            test ECX,ECX
            mov ECX,DWORD PTR [EBX+3284]
            mov DWORD PTR [ESP+40],EDX
            sete AL
            mov DWORD PTR [ESP+72],EAX
            mov DWORD PTR [ESP+36],ECX
$L_559198::

            mov ECX,ESI
            call @Vec3_ZeroThis@8

            add ESI,12
            dec EDI
            jne $L_559198

            mov ECX,12
            mov ESI,OFFSET (_KingBss+490960)
            lea EDI,DWORD PTR [ESP+284]

            rep movsd 

            lea ESI,DWORD PTR [ESP+148]
            mov EDI,4
$L_5591c4::

            mov ECX,ESI
            call @Vec3_ZeroThis@8

            add ESI,12
            dec EDI
            jne $L_5591c4

            mov ECX,12
            mov ESI,OFFSET (_KingBss+490960)
            lea EDI,DWORD PTR [ESP+148]

            rep movsd 

            lea ESI,DWORD PTR [ESP+1940]
            mov EDI,4
$L_5591f0::

            mov ECX,ESI
            call @Vec3_ZeroThis@8

            add ESI,12
            dec EDI
            jne $L_5591f0

            mov ECX,12
            mov ESI,OFFSET (_KingBss+490960)
            lea EDI,DWORD PTR [ESP+1940]

            rep movsd 

            lea ESI,DWORD PTR [ESP+1988]
            mov EDI,4
$L_55921c::

            mov ECX,ESI
            call @Vec3_ZeroThis@8

            add ESI,12
            dec EDI
            jne $L_55921c

            fld DWORD PTR [(_KingBss+491024)]
            mov ECX,12
            mov ESI,OFFSET (_KingBss+490960)
            lea EDI,DWORD PTR [ESP+1988]
            lea EAX,DWORD PTR [EBX+3304]

            rep movsd 

            fchs 
            mov ECX,DWORD PTR [EAX]
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [ESP+404],ECX
            fstp DWORD PTR [ESP+224]
            fld DWORD PTR [(_KingBss+491028)]
            mov EAX,DWORD PTR [EAX+8]
            lea ECX,DWORD PTR [EBP+3304]
            fchs 
            mov DWORD PTR [ESP+408],EDX
            mov DWORD PTR [ESP+412],EAX
            mov EDX,DWORD PTR [ECX]
            fstp DWORD PTR [ESP+228]
            fld DWORD PTR [(_KingBss+491032)]
            mov DWORD PTR [ESP+392],EDX
            mov EAX,DWORD PTR [ECX+4]
            mov EDX,DWORD PTR [ESP+224]
            mov DWORD PTR [ESP+396],EAX
            mov ECX,DWORD PTR [ECX+8]
            mov EAX,DWORD PTR [ESP+228]
            fchs 
            fstp DWORD PTR [ESP+232]
            mov DWORD PTR [ESP+400],ECX
            mov ECX,DWORD PTR [ESP+232]
            mov DWORD PTR [ESP+260],EDX
            lea EDX,DWORD PTR [ESP+260]
            mov DWORD PTR [ESP+564],0
            mov DWORD PTR [ESP+568],0
            mov DWORD PTR [ESP+572],0
            mov DWORD PTR [ESP+528],0
            mov DWORD PTR [ESP+532],0
            mov DWORD PTR [ESP+536],0
            mov DWORD PTR [ESP+504],0
            mov DWORD PTR [ESP+508],0
            mov DWORD PTR [ESP+512],0
            mov DWORD PTR [ESP+540],0
            mov DWORD PTR [ESP+544],0
            mov DWORD PTR [ESP+548],0
            mov DWORD PTR [ESP+552],0
            mov DWORD PTR [ESP+556],0
            mov DWORD PTR [ESP+560],0
            mov DWORD PTR [ESP+516],0
            mov DWORD PTR [ESP+520],0
            mov DWORD PTR [ESP+524],0
            mov DWORD PTR [ESP+268],ECX
            mov ECX,DWORD PTR [EBP+21600]
            push EDX
            mov DWORD PTR [ESP+268],EAX
            call @Trailer_SaddleLoad@12

            mov EAX,DWORD PTR [ESP+32]
            fstp DWORD PTR [ESP+56]
            test EAX,EAX
            je $L_5593d8

            mov EAX,DWORD PTR [ESP+72]
            test EAX,EAX
            je $L_5593d8

            mov EAX,DWORD PTR [EBX+21892]
            mov DWORD PTR [ESP+28],0
            test EAX,EAX
            jle $L_5593e0
$L_5593d8::

            mov DWORD PTR [ESP+28],1
$L_5593e0::

            mov ECX,DWORD PTR [EBX+21600]
            push OFFSET (_KingBss+491056)
            lea EAX,DWORD PTR [ESP+200]
            push OFFSET (_KingBss+491056)
            push EAX
            add ECX,9984
            call @Engine_AddContact@20

            push OFFSET (_KingBss+491056)
            lea ECX,DWORD PTR [ESP+212]
            push OFFSET (_KingBss+491056)
            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            mov DWORD PTR [ESP+452],EAX
            add ECX,9984
            call @Engine_AddContact@20

            mov ECX,DWORD PTR [EBX+21600]
            push OFFSET (_KingBss+491056)
            push OFFSET (_KingBss+491056)
            add ECX,9984
            mov DWORD PTR [ESP+228],EAX
            call @Engine_AddPush@16

            mov ECX,DWORD PTR [EBP+21600]
            push OFFSET (_KingBss+491056)
            push OFFSET (_KingBss+491056)
            add ECX,9984
            mov DWORD PTR [ESP+632],EAX
            call @Engine_AddPush@16

            mov DWORD PTR [ESP+640],EAX
$L_559478::

            mov EAX,DWORD PTR [ESP+32]
            test EAX,EAX
            jne $L_55948c

            mov EAX,DWORD PTR [ESP+72]
            test EAX,EAX
            je $L_55b56e
$L_55948c::

            mov ECX,DWORD PTR [EBX+21600]
            lea EDX,DWORD PTR [ESP+2132]
            push EDX
            call @Body_Matrix22a4@12

            mov ECX,12
            mov ESI,EAX
            lea EDI,DWORD PTR [ESP+284]
            lea EAX,DWORD PTR [ESP+2036]

            rep movsd 

            mov ECX,DWORD PTR [EBP+21600]
            push EAX
            call @Body_Matrix22a4@12

            mov ECX,12
            mov ESI,EAX
            lea EDI,DWORD PTR [ESP+148]

            rep movsd 

            lea ECX,DWORD PTR [ESP+1220]
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            call @Body_Matrix2274@12

            mov ECX,DWORD PTR [EBP+21600]
            lea EDX,DWORD PTR [ESP+1268]
            push EDX
            call @Body_Matrix2274@12

            fld DWORD PTR [ESP+284]
            fmul DWORD PTR [ESP+196]
            fld DWORD PTR [ESP+308]
            fmul DWORD PTR [ESP+204]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+296]
            fmul DWORD PTR [ESP+200]
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+248]
            fld DWORD PTR [ESP+196]
            fmul DWORD PTR [ESP+288]
            fld DWORD PTR [ESP+312]
            fmul DWORD PTR [ESP+204]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+300]
            fmul DWORD PTR [ESP+200]
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+24]
            fstp DWORD PTR [ESP+252]
            fld DWORD PTR [ESP+196]
            fmul DWORD PTR [ESP+292]
            fld DWORD PTR [ESP+316]
            fmul DWORD PTR [ESP+204]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+304]
            fmul DWORD PTR [ESP+200]
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+16]
            fstp DWORD PTR [ESP+256]
            fld DWORD PTR [ESP+172]
            fmul DWORD PTR [ESP+216]
            fld DWORD PTR [ESP+160]
            fmul DWORD PTR [ESP+212]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+148]
            fmul DWORD PTR [ESP+208]
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+20]
            fstp DWORD PTR [ESP+272]
            fld DWORD PTR [ESP+176]
            fmul DWORD PTR [ESP+216]
            fld DWORD PTR [ESP+164]
            fmul DWORD PTR [ESP+212]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+208]
            fmul DWORD PTR [ESP+152]
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+612]
            fstp DWORD PTR [ESP+276]
            fld DWORD PTR [ESP+180]
            fmul DWORD PTR [ESP+216]
            fld DWORD PTR [ESP+168]
            fmul DWORD PTR [ESP+212]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+208]
            fmul DWORD PTR [ESP+156]
            faddp ST(1),ST(0)
            fst DWORD PTR [ESP+620]
            fstp DWORD PTR [ESP+280]
            fadd DWORD PTR [ESP+320]
            fstp DWORD PTR [ESP+444]
            fld DWORD PTR [ESP+24]
            fadd DWORD PTR [ESP+324]
            fstp DWORD PTR [ESP+448]
            fld DWORD PTR [ESP+16]
            fadd DWORD PTR [ESP+328]
            fstp DWORD PTR [ESP+452]
            fld DWORD PTR [ESP+20]
            fadd DWORD PTR [ESP+184]
            fstp DWORD PTR [ESP+468]
            fld DWORD PTR [ESP+612]
            fadd DWORD PTR [ESP+188]
            fstp DWORD PTR [ESP+472]
            fld DWORD PTR [ESP+620]
            fadd DWORD PTR [ESP+192]
            fstp DWORD PTR [ESP+476]
            fld DWORD PTR [ESP+444]
            fsub DWORD PTR [ESP+468]
            fstp DWORD PTR [ESP+576]
            fld DWORD PTR [ESP+448]
            fsub DWORD PTR [ESP+472]
            mov EAX,DWORD PTR [ESP+576]
            mov DWORD PTR [ESP+88],EAX
            lea EAX,DWORD PTR [ESP+480]
            fstp DWORD PTR [ESP+580]
            fld DWORD PTR [ESP+452]
            fsub DWORD PTR [ESP+476]
            mov ECX,DWORD PTR [ESP+580]
            push EAX
            mov DWORD PTR [ESP+96],ECX
            mov ECX,DWORD PTR [EBX+21600]
            fstp DWORD PTR [ESP+588]
            mov EDX,DWORD PTR [ESP+588]
            mov DWORD PTR [ESP+100],EDX
            call @Body_GetVec9012@12

            lea ECX,DWORD PTR [ESP+224]
            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            call @Body_GetVec9012@12

            mov ECX,DWORD PTR [EBX+21600]
            lea EDX,DWORD PTR [ESP+1664]
            push EDX
            call @Body_GetVec9048@12

            fld DWORD PTR [ESP+308]
            fmul DWORD PTR [EAX+8]
            fld DWORD PTR [ESP+296]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+284]
            fmul DWORD PTR [EAX]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+456]
            fld DWORD PTR [ESP+312]
            fmul DWORD PTR [EAX+8]
            fld DWORD PTR [ESP+300]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+288]
            fmul DWORD PTR [EAX]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+460]
            fld DWORD PTR [ESP+316]
            fmul DWORD PTR [EAX+8]
            fld DWORD PTR [ESP+304]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+292]
            fmul DWORD PTR [EAX]
            mov ECX,DWORD PTR [EBP+21600]
            lea EAX,DWORD PTR [ESP+1688]
            push EAX
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP+468]
            call @Body_GetVec9048@12

            fld DWORD PTR [ESP+148]
            fmul DWORD PTR [EAX]
            fld DWORD PTR [ESP+172]
            fmul DWORD PTR [EAX+8]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+160]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+152]
            fmul DWORD PTR [EAX]
            fld DWORD PTR [ESP+176]
            fmul DWORD PTR [EAX+8]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+164]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+156]
            fmul DWORD PTR [EAX]
            fld DWORD PTR [ESP+180]
            fmul DWORD PTR [EAX+8]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+168]
            fmul DWORD PTR [EAX+4]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+460]
            fmul DWORD PTR [ESP+256]
            fld DWORD PTR [ESP+464]
            fmul DWORD PTR [ESP+252]
            fsubp ST(1),ST(0)
            fld DWORD PTR [ESP+248]
            fmul DWORD PTR [ESP+464]
            fld DWORD PTR [ESP+456]
            fmul DWORD PTR [ESP+256]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+936]
            fld DWORD PTR [ESP+456]
            fmul DWORD PTR [ESP+252]
            fld DWORD PTR [ESP+248]
            fmul DWORD PTR [ESP+460]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+940]
            fld DWORD PTR [ESP+480]
            fadd ST(0),ST(1)
            fstp DWORD PTR [ESP+628]
            fstp ST(0)
            fld DWORD PTR [ESP+936]
            fadd DWORD PTR [ESP+484]
            fstp DWORD PTR [ESP+632]
            fld DWORD PTR [ESP+940]
            fadd DWORD PTR [ESP+488]
            fstp DWORD PTR [ESP+636]
            fld ST(1)
            fmul DWORD PTR [ESP+280]
            fld ST(1)
            fmul DWORD PTR [ESP+276]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+956]
            fld DWORD PTR [ESP+272]
            fmul ST(0),ST(1)
            fld ST(3)
            fmul DWORD PTR [ESP+280]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP+960]
            push ECX
            fstp ST(0)
            fxch ST(1)
            fmul DWORD PTR [ESP+280]
            fld DWORD PTR [ESP+276]
            fmul ST(0),ST(2)
            fsubp ST(1),ST(0)
            fxch ST(1)
            fstp ST(0)
            fadd DWORD PTR [ESP+236]
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+964]
            fadd DWORD PTR [ESP+232]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+232]
            fadd DWORD PTR [ESP+964]
            push ECX
            lea ECX,DWORD PTR [ESP+1052]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+636]
            fsub DWORD PTR [ESP+1048]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+636]
            fsub DWORD PTR [ESP+1048]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+636]
            fsub DWORD PTR [ESP+1048]
            push ECX
            lea ECX,DWORD PTR [ESP+1100]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov ECX,DWORD PTR [ESP+1088]
            mov EDX,DWORD PTR [ESP+1092]
            mov EAX,DWORD PTR [ESP+1096]
            mov DWORD PTR [ESP+124],ECX
            mov ECX,DWORD PTR [ESP+88]
            mov DWORD PTR [ESP+128],EDX
            mov EDX,DWORD PTR [ESP+92]
            mov DWORD PTR [ESP+356],ECX
            mov DWORD PTR [ESP+132],EAX
            mov EAX,DWORD PTR [ESP+96]
            lea ECX,DWORD PTR [ESP+88]
            mov DWORD PTR [ESP+360],EDX
            push ECX
            mov DWORD PTR [ESP+368],EAX
            call _Vec3_Length

            fst DWORD PTR [ESP+24]
            fcomp DWORD PTR [(_KingRdata+4708)]
            add ESP,4
            fnstsw AX
            test AH,65
            jne $L_559a76

            fld DWORD PTR [ESP+96]
            fdiv DWORD PTR [ESP+20]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+96]
            fdiv DWORD PTR [ESP+24]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+96]
            fdiv DWORD PTR [ESP+28]
            push ECX
            lea ECX,DWORD PTR [ESP+692]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov EDX,DWORD PTR [ESP+680]
            mov EAX,DWORD PTR [ESP+684]
            mov ECX,DWORD PTR [ESP+688]
            mov DWORD PTR [ESP+356],EDX
            mov DWORD PTR [ESP+360],EAX
            mov DWORD PTR [ESP+364],ECX
$L_559a76::

            mov EAX,DWORD PTR [ESP+28]
            xor EDI,EDI
            cmp EAX,EDI
            jne $L_55a8af

            mov EAX,DWORD PTR [EBX+21892]
            or ESI,4294967295
            cmp EAX,ESI
            je $L_55a8b2

            fld DWORD PTR [ESP+40]
            fmul DWORD PTR [ESP+36]
            fld DWORD PTR [ESP+40]
            fadd DWORD PTR [ESP+36]
            fdivp ST(1),ST(0)
            fsqrt 
            fst DWORD PTR [ESP+16]
            fmul DWORD PTR [(_KingRdata+17376)]
            fstp DWORD PTR [ESP+24]
            fld DWORD PTR [ESP+20]
            fcomp DWORD PTR [(_KingRdata+4704)]
            fld DWORD PTR [ESP+20]
            fnstsw AX
            test AH,65
            jne $L_559ad4

            fmul DWORD PTR [(_KingRdata+4704)]
            fsqrt 
$L_559ad4::

            fmul DWORD PTR [ESP+24]
            push ECX
            fld ST(0)
            fmul DWORD PTR [ESP+368]
            fstp DWORD PTR [ESP]
            push ECX
            fld ST(0)
            fmul DWORD PTR [ESP+368]
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+364]
            fmul ST(0),ST(1)
            push ECX
            lea ECX,DWORD PTR [ESP+704]
            fstp DWORD PTR [ESP]
            fstp ST(0)
            call @Vec3_SetThis@20

            mov EAX,DWORD PTR [ESP+696]
            mov EDX,DWORD PTR [ESP+692]
            mov ECX,DWORD PTR [ESP+700]
            mov DWORD PTR [ESP+240],EAX
            mov EAX,DWORD PTR [EBX+21892]
            mov DWORD PTR [ESP+236],EDX
            cmp EAX,ESI
            mov DWORD PTR [ESP+244],ECX
            jne $L_559b71

            lea EDX,DWORD PTR [ESP+1712]
            push 1045220557
            push EDX
            lea ECX,DWORD PTR [ESP+244]
            call @Vec3_MulAssign@16

            fld DWORD PTR [ESP+20]
            fcomp QWORD PTR [(_KingRdata+1760)]
            fnstsw AX
            test AH,1
            je $L_559b71

            mov DWORD PTR [EBX+21892],EDI
$L_559b71::

            fld DWORD PTR [(_KingBss+491032)]
            fmul DWORD PTR [(_KingRdata+4696)]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [(_KingBss+491028)]
            fmul DWORD PTR [(_KingRdata+4696)]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [(_KingBss+491024)]
            fmul DWORD PTR [(_KingRdata+4696)]
            push ECX
            lea ECX,DWORD PTR [ESP+1076]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+1072]
            fadd DWORD PTR [ESP+96]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+1072]
            fadd DWORD PTR [ESP+96]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+1072]
            fadd DWORD PTR [ESP+96]
            push ECX
            lea ECX,DWORD PTR [ESP+1352]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+16]
            fmul DWORD PTR [(_KingRdata+10168)]
            mov ECX,DWORD PTR [(_KingBss+491028)]
            mov EAX,DWORD PTR [(_KingBss+491024)]
            push ECX
            mov EDX,DWORD PTR [(_KingBss+491032)]
            fld ST(0)
            fmul DWORD PTR [ESP+136]
            mov DWORD PTR [ESP+376],ECX
            mov DWORD PTR [ESP+372],EAX
            mov DWORD PTR [ESP+380],EDX
            fstp DWORD PTR [ESP]
            push ECX
            fld ST(0)
            fmul DWORD PTR [ESP+136]
            fstp DWORD PTR [ESP]
            push ECX
            lea ECX,DWORD PTR [ESP+728]
            fmul DWORD PTR [ESP+136]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+124]
            fmul DWORD PTR [ESP+88]
            fld DWORD PTR [ESP+128]
            fmul DWORD PTR [ESP+92]
            mov EAX,DWORD PTR [ESP+716]
            mov ECX,DWORD PTR [ESP+720]
            mov EDX,DWORD PTR [ESP+724]
            mov DWORD PTR [ESP+428],EAX
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+132]
            fmul DWORD PTR [ESP+96]
            mov DWORD PTR [ESP+432],ECX
            mov DWORD PTR [ESP+436],EDX
            faddp ST(1),ST(0)
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,65
            jne $L_559cb9

            lea EAX,DWORD PTR [ESP+1316]
            push 1073741824
            push EAX
            lea ECX,DWORD PTR [ESP+436]
            call @Vec3_MulAssign@16
$L_559cb9::

            fld DWORD PTR [ESP+20]
            fcomp DWORD PTR [(_KingRdata+4704)]
            fnstsw AX
            test AH,1
            je $L_559cee

            fld DWORD PTR [ESP+20]
            fmul DWORD PTR [(_KingRdata+19008)]
            push ECX
            lea ECX,DWORD PTR [ESP+1368]
            fsqrt 
            fstp DWORD PTR [ESP]
            push ECX
            lea ECX,DWORD PTR [ESP+436]
            call @Vec3_MulAssign@16
$L_559cee::

            lea EDX,DWORD PTR [ESP+428]
            push EDX
            call _Vec3_Length

            fstp DWORD PTR [ESP+20]
            lea EAX,DWORD PTR [ESP+240]
            push EAX
            call _Vec3_Length

            fstp ST(0)
            fld DWORD PTR [ESP+32]
            fadd ST(0),ST(0)
            add ESP,8
            fcom DWORD PTR [ESP+16]
            fnstsw AX
            test AH,1
            je $L_559d40

            fdiv DWORD PTR [ESP+16]
            push ECX
            lea ECX,DWORD PTR [ESP+1764]
            fstp DWORD PTR [ESP]
            push ECX
            lea ECX,DWORD PTR [ESP+436]
            call @Vec3_MulAssign@16

            jmp $L_559d42
$L_559d40::

            fstp ST(0)
$L_559d42::

            mov ECX,DWORD PTR [EBX+21600]
            call @Body_Get9508@8

            mov EAX,DWORD PTR [ESP+428]
            mov ECX,DWORD PTR [ESP+432]
            fstp ST(0)
            fld DWORD PTR [ESP+20]
            fcomp DWORD PTR [(_KingRdata+4868)]
            mov EDX,DWORD PTR [ESP+436]
            mov DWORD PTR [ESP+60],EAX
            mov DWORD PTR [ESP+44],EAX
            mov DWORD PTR [ESP+64],ECX
            mov DWORD PTR [ESP+68],EDX
            mov DWORD PTR [ESP+48],ECX
            fnstsw AX
            mov DWORD PTR [ESP+52],EDX
            test AH,1
            je $L_55a30f

            fld DWORD PTR [ESP+124]
            fld DWORD PTR [ESP+132]
            fld DWORD PTR [ESP+128]
            fld ST(0)
            fmul ST(0),ST(1)
            fld ST(3)
            fmul ST(0),ST(4)
            faddp ST(1),ST(0)
            fld ST(2)
            fmul ST(0),ST(3)
            faddp ST(1),ST(0)
            fcomp QWORD PTR [(_KingRdata+19000)]
            fstp ST(0)
            fnstsw AX
            fstp ST(0)
            test AH,65
            fstp ST(0)
            jne $L_55a30f

            lea EDX,DWORD PTR [ESP+60]
            push EDX
            call _Vec3_Length

            fstp DWORD PTR [ESP+20]
            fld DWORD PTR [ESP+20]
            fcomp DWORD PTR [(_KingRdata+4708)]
            mov EAX,DWORD PTR [(_KingBss+491056)]
            mov ECX,DWORD PTR [(_KingBss+491060)]
            mov EDX,DWORD PTR [(_KingBss+491064)]
            mov DWORD PTR [ESP+336],EAX
            add ESP,4
            mov DWORD PTR [ESP+336],ECX
            fnstsw AX
            mov DWORD PTR [ESP+340],EDX
            test AH,65
            jne $L_559e6e

            fld DWORD PTR [(_KingRdata+4688)]
            fdiv DWORD PTR [ESP+16]
            push ECX
            fld ST(0)
            fmul DWORD PTR [ESP+72]
            fstp DWORD PTR [ESP]
            push ECX
            fld ST(0)
            fmul DWORD PTR [ESP+72]
            fstp DWORD PTR [ESP]
            push ECX
            lea ECX,DWORD PTR [ESP+1040]
            fmul DWORD PTR [ESP+72]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov EAX,DWORD PTR [ESP+1028]
            mov ECX,DWORD PTR [ESP+1032]
            mov EDX,DWORD PTR [ESP+1036]
            mov DWORD PTR [ESP+332],EAX
            mov DWORD PTR [ESP+336],ECX
            mov DWORD PTR [ESP+340],EDX
$L_559e6e::

            fld DWORD PTR [ESP+328]
            fsub DWORD PTR [ESP+452]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+328]
            fsub DWORD PTR [ESP+452]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+328]
            fsub DWORD PTR [ESP+452]
            push ECX
            lea ECX,DWORD PTR [ESP+752]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov EAX,DWORD PTR [ESP+740]
            mov ECX,DWORD PTR [ESP+744]
            mov EDX,DWORD PTR [ESP+748]
            mov DWORD PTR [ESP+980],EAX
            mov DWORD PTR [ESP+984],ECX
            lea EAX,DWORD PTR [ESP+980]
            lea ECX,DWORD PTR [ESP+136]
            push EAX
            push ECX
            mov DWORD PTR [ESP+996],EDX
            call _Vec3_Normalized

            fld DWORD PTR [ESP+148]
            fmul DWORD PTR [ESP+340]
            fld DWORD PTR [ESP+344]
            fmul DWORD PTR [ESP+144]
            add ESP,4
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+344]
            fmul DWORD PTR [ESP+140]
            fld DWORD PTR [ESP+148]
            fmul DWORD PTR [ESP+336]
            push ECX
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+152]
            fmul DWORD PTR [ESP+344]
            fld DWORD PTR [ESP+148]
            fmul DWORD PTR [ESP+348]
            push ECX
            lea ECX,DWORD PTR [ESP+776]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+772]
            fld DWORD PTR [ESP+768]
            fld DWORD PTR [ESP+764]
            fld ST(0)
            fmul ST(0),ST(1)
            fld ST(2)
            fmul ST(0),ST(3)
            faddp ST(1),ST(0)
            fld ST(3)
            fmul ST(0),ST(4)
            faddp ST(1),ST(0)
            fstp ST(3)
            fstp ST(0)
            fstp ST(0)
            fcomp QWORD PTR [(_KingRdata+1848)]
            fnstsw AX
            test AH,1
            je $L_55a081

            fld DWORD PTR [ESP+144]
            fmul DWORD PTR [ESP+68]
            fld DWORD PTR [ESP+140]
            fmul DWORD PTR [ESP+64]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+136]
            fmul DWORD PTR [ESP+60]
            faddp ST(1),ST(0)
            fcomp QWORD PTR [(_KingRdata+1656)]
            fld DWORD PTR [ESP+16]
            fnstsw AX
            test AH,65
            jne $L_55a029

            fmul DWORD PTR [ESP+144]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+20]
            fmul DWORD PTR [ESP+144]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+24]
            fmul DWORD PTR [ESP+144]
            push ECX
            lea ECX,DWORD PTR [ESP+1088]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov EDX,DWORD PTR [ESP+1076]
            mov EAX,DWORD PTR [ESP+1080]
            mov ECX,DWORD PTR [ESP+1084]
            mov DWORD PTR [ESP+60],EDX
            mov DWORD PTR [ESP+64],EAX
            jmp $L_55a07d
$L_55a029::

            fchs 
            fld DWORD PTR [ESP+144]
            fmul ST(0),ST(1)
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+144]
            fmul ST(0),ST(1)
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+144]
            fmul ST(0),ST(1)
            push ECX
            lea ECX,DWORD PTR [ESP+800]
            fstp DWORD PTR [ESP]
            fstp ST(0)
            call @Vec3_SetThis@20

            mov EDX,DWORD PTR [ESP+788]
            mov EAX,DWORD PTR [ESP+792]
            mov ECX,DWORD PTR [ESP+796]
            mov DWORD PTR [ESP+60],EDX
            mov DWORD PTR [ESP+64],EAX
$L_55a07d::

            mov DWORD PTR [ESP+68],ECX
$L_55a081::

            lea EDX,DWORD PTR [ESP+44]
            push EDX
            call _Vec3_Length

            fstp DWORD PTR [ESP+20]
            fld DWORD PTR [ESP+20]
            fcomp DWORD PTR [(_KingRdata+4708)]
            mov EAX,DWORD PTR [(_KingBss+491056)]
            mov ECX,DWORD PTR [(_KingBss+491060)]
            mov EDX,DWORD PTR [(_KingBss+491064)]
            mov DWORD PTR [ESP+348],EAX
            add ESP,4
            mov DWORD PTR [ESP+348],ECX
            fnstsw AX
            mov DWORD PTR [ESP+352],EDX
            test AH,65
            jne $L_55a129

            fld DWORD PTR [(_KingRdata+4688)]
            fdiv DWORD PTR [ESP+16]
            push ECX
            fld DWORD PTR [ESP+56]
            fmul ST(0),ST(1)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+52]
            fmul ST(0),ST(1)
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+52]
            fmul ST(0),ST(1)
            push ECX
            lea ECX,DWORD PTR [ESP+1016]
            fstp DWORD PTR [ESP]
            fstp ST(0)
            call @Vec3_SetThis@20

            mov EAX,DWORD PTR [ESP+1004]
            mov ECX,DWORD PTR [ESP+1008]
            mov EDX,DWORD PTR [ESP+1012]
            mov DWORD PTR [ESP+344],EAX
            mov DWORD PTR [ESP+348],ECX
            mov DWORD PTR [ESP+352],EDX
$L_55a129::

            fld DWORD PTR [ESP+192]
            fsub DWORD PTR [ESP+476]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+192]
            fsub DWORD PTR [ESP+476]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+192]
            fsub DWORD PTR [ESP+476]
            push ECX
            lea ECX,DWORD PTR [ESP+824]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov EAX,DWORD PTR [ESP+812]
            mov ECX,DWORD PTR [ESP+816]
            mov EDX,DWORD PTR [ESP+820]
            mov DWORD PTR [ESP+1052],EAX
            mov DWORD PTR [ESP+1056],ECX
            lea EAX,DWORD PTR [ESP+1052]
            lea ECX,DWORD PTR [ESP+112]
            push EAX
            push ECX
            mov DWORD PTR [ESP+1068],EDX
            call _Vec3_Normalized

            fld DWORD PTR [ESP+124]
            fmul DWORD PTR [ESP+352]
            fld DWORD PTR [ESP+356]
            fmul DWORD PTR [ESP+120]
            add ESP,4
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+356]
            fmul DWORD PTR [ESP+116]
            fld DWORD PTR [ESP+124]
            fmul DWORD PTR [ESP+348]
            push ECX
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+128]
            fmul DWORD PTR [ESP+356]
            fld DWORD PTR [ESP+124]
            fmul DWORD PTR [ESP+360]
            push ECX
            lea ECX,DWORD PTR [ESP+848]
            fsubp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+844]
            fld DWORD PTR [ESP+840]
            fld DWORD PTR [ESP+836]
            fld ST(0)
            fmul ST(0),ST(1)
            fld ST(2)
            fmul ST(0),ST(3)
            faddp ST(1),ST(0)
            fld ST(3)
            fmul ST(0),ST(4)
            faddp ST(1),ST(0)
            fstp ST(3)
            fstp ST(0)
            fstp ST(0)
            fcomp QWORD PTR [(_KingRdata+1848)]
            fnstsw AX
            test AH,1
            je $L_55a30f

            fld DWORD PTR [ESP+120]
            fmul DWORD PTR [ESP+52]
            fld DWORD PTR [ESP+116]
            fmul DWORD PTR [ESP+48]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+112]
            fmul DWORD PTR [ESP+44]
            faddp ST(1),ST(0)
            fcomp QWORD PTR [(_KingRdata+1656)]
            fld DWORD PTR [ESP+16]
            fnstsw AX
            test AH,65
            jne $L_55a2c0

            fmul DWORD PTR [ESP+120]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+20]
            fmul DWORD PTR [ESP+120]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+24]
            fmul DWORD PTR [ESP+120]
            push ECX
            lea ECX,DWORD PTR [ESP+956]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov EDX,DWORD PTR [ESP+944]
            mov EAX,DWORD PTR [ESP+948]
            mov ECX,DWORD PTR [ESP+952]
            mov DWORD PTR [ESP+44],EDX
            mov DWORD PTR [ESP+48],EAX
            jmp $L_55a30b
$L_55a2c0::

            fchs 
            fld DWORD PTR [ESP+120]
            fmul ST(0),ST(1)
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+120]
            fmul ST(0),ST(1)
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+120]
            fmul ST(0),ST(1)
            push ECX
            lea ECX,DWORD PTR [ESP+872]
            fstp DWORD PTR [ESP]
            fstp ST(0)
            call @Vec3_SetThis@20

            mov EDX,DWORD PTR [ESP+860]
            mov EAX,DWORD PTR [ESP+864]
            mov ECX,DWORD PTR [ESP+868]
            mov DWORD PTR [ESP+44],EDX
            mov DWORD PTR [ESP+48],EAX
$L_55a30b::

            mov DWORD PTR [ESP+52],ECX
$L_55a30f::

            fld DWORD PTR [ESP+376]
            fadd DWORD PTR [ESP+244]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+376]
            fadd DWORD PTR [ESP+244]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+376]
            fadd DWORD PTR [ESP+244]
            push ECX
            lea ECX,DWORD PTR [ESP+980]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+376]
            fadd DWORD PTR [ESP+244]
            push ECX
            mov EDX,DWORD PTR [ESP+976]
            mov EAX,DWORD PTR [ESP+980]
            mov EDI,DWORD PTR [ESP+972]
            mov DWORD PTR [ESP+596],EDX
            mov DWORD PTR [ESP+600],EAX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+376]
            fadd DWORD PTR [ESP+244]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+376]
            fadd DWORD PTR [ESP+244]
            push ECX
            lea ECX,DWORD PTR [ESP+896]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+892]
            push ECX
            fchs 
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+892]
            push ECX
            fchs 
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+892]
            push ECX
            lea ECX,DWORD PTR [ESP+1004]
            fchs 
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov EAX,DWORD PTR [ESP+1000]
            mov ECX,DWORD PTR [ESP+992]
            mov EDX,DWORD PTR [ESP+996]
            mov DWORD PTR [ESP+108],EAX
            mov EAX,DWORD PTR [EBX+21892]
            mov DWORD PTR [ESP+100],ECX
            cmp EAX,ESI
            mov DWORD PTR [ESP+104],EDX
            jne $L_55a50c

            fld DWORD PTR [ESP+168]
            fsubr QWORD PTR [(_KingRdata+1976)]
            fld DWORD PTR [ESP+376]
            push ECX
            fmul ST(0),ST(1)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+376]
            fmul ST(0),ST(1)
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+376]
            fmul ST(0),ST(1)
            push ECX
            lea ECX,DWORD PTR [ESP+920]
            fstp DWORD PTR [ESP]
            fstp ST(0)
            call @Vec3_SetThis@20

            mov ECX,DWORD PTR [ESP+908]
            mov EDX,DWORD PTR [ESP+912]
            mov EAX,DWORD PTR [ESP+916]
            mov DWORD PTR [ESP+1016],ECX
            mov DWORD PTR [ESP+1020],EDX
            lea ECX,DWORD PTR [ESP+1016]
            mov DWORD PTR [ESP+1024],EAX
            lea EDX,DWORD PTR [ESP+44]
            push ECX
            lea EAX,DWORD PTR [ESP+240]
            push EDX
            lea ECX,DWORD PTR [ESP+1396]
            push EAX
            push ECX
            call _Vec3_Add

            add ESP,12
            lea EDX,DWORD PTR [ESP+1680]
            push EAX
            push EDX
            call _Vec3_Add

            mov EDI,DWORD PTR [EAX]
            mov ECX,DWORD PTR [EAX+4]
            mov DWORD PTR [ESP+604],ECX
            lea ECX,DWORD PTR [ESP+248]
            mov EDX,DWORD PTR [EAX+8]
            lea EAX,DWORD PTR [ESP+72]
            mov DWORD PTR [ESP+608],EDX
            push EAX
            lea EDX,DWORD PTR [ESP+1428]
            push ECX
            push EDX
            call _Vec3_Add

            push EAX
            lea EAX,DWORD PTR [ESP+1812]
            push EAX
            call _Vec3_Negated

            mov ECX,DWORD PTR [EAX]
            add ESP,32
            mov DWORD PTR [ESP+100],ECX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [ESP+104],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [ESP+108],EAX
$L_55a50c::

            mov ECX,DWORD PTR [(_KingBss+491060)]
            mov EDX,DWORD PTR [(_KingBss+491064)]
            mov EAX,DWORD PTR [(_KingBss+491056)]
            mov DWORD PTR [ESP+496],ECX
            mov DWORD PTR [ESP+264],ECX
            mov DWORD PTR [ESP+500],EDX
            mov DWORD PTR [ESP+268],EDX
            lea ECX,DWORD PTR [ESP+160]
            mov DWORD PTR [ESP+492],EAX
            mov DWORD PTR [ESP+260],EAX
            lea EDX,DWORD PTR [ESP+296]
            push ECX
            lea EAX,DWORD PTR [ESP+1104]
            push EDX
            push EAX
            call _Vec3_Cross

            lea ECX,DWORD PTR [ESP+1112]
            push ECX
            call _Vec3_Length

            fcom QWORD PTR [(_KingRdata+18992)]
            add ESP,16
            fnstsw AX
            test AH,65
            jne $L_55a5ee

            fld DWORD PTR [ESP+400]
            fmul DWORD PTR [ESP+412]
            fld ST(1)
            fsub QWORD PTR [(_KingRdata+18992)]
            push ECX
            lea EDX,DWORD PTR [ESP+1104]
            fmulp ST(1),ST(0)
            fxch ST(1)
            lea EAX,DWORD PTR [ESP+1440]
            fmulp ST(1),ST(0)
            fmul QWORD PTR [(_KingRdata+1920)]
            fld DWORD PTR [ESP+404]
            fadd DWORD PTR [ESP+416]
            fdivp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            push EDX
            push EAX
            call _Vec3_Scaled

            mov ECX,DWORD PTR [EAX]
            add ESP,12
            mov DWORD PTR [ESP+492],ECX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [ESP+496],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [ESP+500],EAX
            jmp $L_55a5f0
$L_55a5ee::

            fstp ST(0)
$L_55a5f0::

            lea ECX,DWORD PTR [ESP+1700]
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            call @Body_GetVec9048@12

            mov EDX,DWORD PTR [EAX+4]
            mov ECX,DWORD PTR [EBP+21600]
            lea EAX,DWORD PTR [ESP+1460]
            mov DWORD PTR [ESP+616],EDX
            push EAX
            call @Body_GetVec9048@12

            fld DWORD PTR [EAX+4]
            fsub DWORD PTR [ESP+616]
            fld DWORD PTR [ESP+396]
            fmul DWORD PTR [ESP+408]
            mov ESI,DWORD PTR [ESP+440]
            mov ECX,DWORD PTR [EBX+21600]
            mov EAX,DWORD PTR [ESP+100]
            fmul ST(0),ST(1)
            lea ESI,DWORD PTR [ESI+ESI*2]
            shl ESI,2
            fmul DWORD PTR [(_KingRdata+5200)]
            fld DWORD PTR [ESP+396]
            fadd DWORD PTR [ESP+408]
            lea EDX,DWORD PTR [ESI+ECX*1+10244]
            mov ECX,DWORD PTR [ESP+104]
            fdivp ST(1),ST(0)
            mov DWORD PTR [EDX],EAX
            mov EAX,DWORD PTR [ESP+108]
            mov DWORD PTR [EDX+4],ECX
            lea ECX,DWORD PTR [ESP+60]
            push ECX
            mov DWORD PTR [EDX+8],EAX
            lea EDX,DWORD PTR [ESP+1812]
            push EDX
            fstp DWORD PTR [ESP+32]
            fstp ST(0)
            call _Vec3_Negated

            mov ECX,DWORD PTR [EBX+21600]
            lea EDX,DWORD PTR [ESI+ECX*1+10304]
            mov ECX,DWORD PTR [EAX]
            mov ESI,DWORD PTR [ESP+632]
            mov DWORD PTR [EDX],ECX
            mov ECX,DWORD PTR [EAX+4]
            lea ESI,DWORD PTR [ESI+ESI*2]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EAX+8]
            shl ESI,2
            mov DWORD PTR [EDX+8],EAX
            mov EAX,DWORD PTR [ESP+228]
            mov ECX,DWORD PTR [EBP+21600]
            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,2
            lea EDX,DWORD PTR [ECX+EAX*1+10244]
            mov ECX,DWORD PTR [ESP+600]
            mov DWORD PTR [EDX],EDI
            mov DWORD PTR [EDX+4],ECX
            mov ECX,DWORD PTR [ESP+604]
            mov DWORD PTR [EDX+8],ECX
            mov EDX,DWORD PTR [EBP+21600]
            mov ECX,DWORD PTR [ESP+52]
            lea EAX,DWORD PTR [EDX+EAX*1+10304]
            mov DWORD PTR [EAX],ECX
            mov EDX,DWORD PTR [ESP+56]
            mov DWORD PTR [EAX+4],EDX
            mov ECX,DWORD PTR [ESP+60]
            mov EDX,DWORD PTR [ESP+32]
            mov DWORD PTR [EAX+8],ECX
            push EDX
            lea EAX,DWORD PTR [ESP+1496]
            push OFFSET (_KingBss+491008)
            push EAX
            call _Vec3_Scaled

            add ESP,20
            lea ECX,DWORD PTR [ESP+1220]
            lea EDX,DWORD PTR [ESP+492]
            push EAX
            push ECX
            push EDX
            lea EAX,DWORD PTR [ESP+1736]
            push EAX
            call _Vec3_RotateBy

            add ESP,12
            lea ECX,DWORD PTR [ESP+1512]
            push EAX
            push ECX
            call _Vec3_Add

            mov EDX,DWORD PTR [EBX+21600]
            mov EDI,DWORD PTR [ESP+652]
            fld DWORD PTR [ESP+36]
            lea ECX,DWORD PTR [ESI+EDX*1+10364]
            mov EDX,DWORD PTR [EAX]
            add ESP,8
            lea EDI,DWORD PTR [EDI+EDI*2]
            mov DWORD PTR [ECX],EDX
            mov EDX,DWORD PTR [EAX+4]
            fchs 
            fstp DWORD PTR [ESP]
            mov DWORD PTR [ECX+4],EDX
            mov EAX,DWORD PTR [EAX+8]
            push OFFSET (_KingBss+491008)
            mov DWORD PTR [ECX+8],EAX
            lea ECX,DWORD PTR [ESP+1840]
            push ECX
            shl EDI,2
            call _Vec3_Scaled

            add ESP,12
            lea EDX,DWORD PTR [ESP+1268]
            lea ECX,DWORD PTR [ESP+1532]
            push EAX
            lea EAX,DWORD PTR [ESP+496]
            push EDX
            push EAX
            push ECX
            call _Vec3_Negated

            add ESP,8
            lea EDX,DWORD PTR [ESP+1756]
            push EAX
            push EDX
            call _Vec3_RotateBy

            add ESP,12
            push EAX
            lea EAX,DWORD PTR [ESP+1564]
            push EAX
            call _Vec3_Add

            mov ECX,DWORD PTR [EBP+21600]
            lea EDX,DWORD PTR [ECX+EDI*1+10364]
            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [EDX],ECX
            mov ECX,DWORD PTR [EAX+4]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EAX+8]
            lea ECX,DWORD PTR [ESP+1232]
            mov DWORD PTR [EDX+8],EAX
            lea EDX,DWORD PTR [ESP+272]
            push ECX
            lea EAX,DWORD PTR [ESP+1812]
            push EDX
            push EAX
            call _Vec3_RotateBy

            mov ECX,DWORD PTR [EBX+21600]
            add ESP,24
            lea EDX,DWORD PTR [ESI+ECX*1+10424]
            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [EDX],ECX
            mov ECX,DWORD PTR [EAX+4]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [EAX+8]
            lea ECX,DWORD PTR [ESP+1268]
            mov DWORD PTR [EDX+8],EAX
            lea EDX,DWORD PTR [ESP+260]
            push ECX
            lea EAX,DWORD PTR [ESP+1584]
            push EDX
            push EAX
            call _Vec3_Negated

            add ESP,8
            lea ECX,DWORD PTR [ESP+1776]
            push EAX
            push ECX
            call _Vec3_RotateBy

            mov EDX,DWORD PTR [EBP+21600]
            add ESP,12
            fld DWORD PTR [ESP+20]
            fcomp DWORD PTR [(_KingRdata+4896)]
            lea ECX,DWORD PTR [EDX+EDI*1+10424]
            mov EDX,DWORD PTR [EAX]
            mov DWORD PTR [ECX],EDX
            mov EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [ECX+4],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [ECX+8],EAX
            fnstsw AX
            test AH,65
            jne $L_55aef4

            mov DWORD PTR [ESP+28],1
            jmp $L_55aef4
$L_55a8af::

            or ESI,4294967295
$L_55a8b2::

            lea ECX,DWORD PTR [ESP+124]
            push ECX
            call _Vec3_Length

            fld ST(0)
            fadd DWORD PTR [(_KingRdata+4688)]
            add ESP,4
            fdivr ST(0),ST(1)
            fstp DWORD PTR [ESP+24]
            fstp ST(0)
            fld DWORD PTR [ESP+168]
            fsubr QWORD PTR [(_KingRdata+1648)]
            fcom DWORD PTR [(_KingRdata+4688)]
            fnstsw AX
            test AH,1
            je $L_55a8ef

            fld ST(0)
            fmul ST(0),ST(1)
            fmulp ST(1),ST(0)
$L_55a8ef::

            mov EAX,DWORD PTR [(_KingBss+491060)]
            mov EDX,DWORD PTR [(_KingBss+491056)]
            mov ECX,DWORD PTR [(_KingBss+491064)]
            mov DWORD PTR [ESP+80],EAX
            mov EAX,DWORD PTR [EBX+21892]
            mov DWORD PTR [ESP+76],EDX
            cmp EAX,ESI
            mov DWORD PTR [ESP+84],ECX
            jne $L_55ac40

            fld DWORD PTR [ESP+180]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,65
            jne $L_55a937

            fld DWORD PTR [ESP+180]
            jmp $L_55a93d
$L_55a937::

            fld DWORD PTR [(_KingRdata+4708)]
$L_55a93d::

            fld DWORD PTR [ESP+20]
            fcomp DWORD PTR [(_KingRdata+4708)]
            fnstsw AX
            test AH,65
            jne $L_55a966

            fld DWORD PTR [ESP+96]
            fmul QWORD PTR [(_KingRdata+8432)]
            fdiv DWORD PTR [ESP+20]
            fadd QWORD PTR [(_KingRdata+1648)]
            fxch ST(1)
            fmulp ST(1),ST(0)
$L_55a966::

            fld DWORD PTR [ESP+20]
            fadd QWORD PTR [(_KingRdata+1760)]
            fxch ST(1)
            push ECX
            fmulp ST(1),ST(0)
            fxch ST(1)
            fmulp ST(1),ST(0)
            fmul DWORD PTR [ESP+60]
            fmul QWORD PTR [(_KingRdata+9080)]
            fld DWORD PTR [ESP+24]
            fadd DWORD PTR [(_KingRdata+4688)]
            fdivp ST(1),ST(0)
            fld ST(0)
            fmul DWORD PTR [(_KingBss+491032)]
            fstp DWORD PTR [ESP]
            push ECX
            fld ST(0)
            fmul DWORD PTR [(_KingBss+491028)]
            fstp DWORD PTR [ESP]
            push ECX
            lea ECX,DWORD PTR [ESP+392]
            fmul DWORD PTR [(_KingBss+491024)]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+388]
            fmul DWORD PTR [ESP+316]
            fld DWORD PTR [ESP+384]
            fmul DWORD PTR [ESP+304]
            push ECX
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+384]
            fmul DWORD PTR [ESP+296]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+392]
            fmul DWORD PTR [ESP+316]
            fld DWORD PTR [ESP+388]
            fmul DWORD PTR [ESP+304]
            push ECX
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+388]
            fmul DWORD PTR [ESP+296]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+388]
            fmul DWORD PTR [ESP+292]
            fld DWORD PTR [ESP+396]
            fmul DWORD PTR [ESP+316]
            push ECX
            lea ECX,DWORD PTR [ESP+656]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+396]
            fmul DWORD PTR [ESP+308]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+652]
            push ECX
            fchs 
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+652]
            push ECX
            fchs 
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+652]
            push ECX
            lea ECX,DWORD PTR [ESP+680]
            fchs 
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov EDX,DWORD PTR [ESP+668]
            mov EAX,DWORD PTR [ESP+672]
            mov ECX,DWORD PTR [ESP+676]
            mov DWORD PTR [ESP+656],EDX
            mov DWORD PTR [ESP+660],EAX
            lea EDX,DWORD PTR [ESP+656]
            lea EAX,DWORD PTR [ESP+1604]
            mov DWORD PTR [ESP+664],ECX
            push EDX
            push EAX
            lea ECX,DWORD PTR [ESP+84]
            call @Vec3_AddAssign@16

            fld DWORD PTR [ESP+40]
            fmul DWORD PTR [ESP+36]
            fld DWORD PTR [ESP+40]
            fadd DWORD PTR [ESP+36]
            push ECX
            fdivp ST(1),ST(0)
            fsqrt 
            fmul DWORD PTR [(_KingRdata+18988)]
            fld ST(0)
            fmul DWORD PTR [ESP+136]
            fstp DWORD PTR [ESP]
            push ECX
            fld ST(0)
            fmul DWORD PTR [ESP+136]
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+132]
            fmul ST(0),ST(1)
            push ECX
            lea ECX,DWORD PTR [ESP+740]
            fstp DWORD PTR [ESP]
            fstp ST(0)
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+364]
            fmul DWORD PTR [ESP+56]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+364]
            fmul DWORD PTR [ESP+60]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+364]
            fmul DWORD PTR [ESP+64]
            push ECX
            lea ECX,DWORD PTR [ESP+716]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+712]
            fmul DWORD PTR [(_KingRdata+8952)]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+712]
            fmul DWORD PTR [(_KingRdata+8952)]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+712]
            fmul DWORD PTR [(_KingRdata+8952)]
            push ECX
            lea ECX,DWORD PTR [ESP+764]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+736]
            fsub DWORD PTR [ESP+760]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+736]
            fsub DWORD PTR [ESP+760]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+736]
            fsub DWORD PTR [ESP+760]
            push ECX
            lea ECX,DWORD PTR [ESP+788]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov ECX,DWORD PTR [ESP+776]
            mov EDX,DWORD PTR [ESP+780]
            mov EAX,DWORD PTR [ESP+784]
            mov DWORD PTR [ESP+800],ECX
            mov DWORD PTR [ESP+804],EDX
            mov DWORD PTR [ESP+808],EAX
            lea ECX,DWORD PTR [ESP+800]
            lea EDX,DWORD PTR [ESP+1820]
            push ECX
            push EDX
            lea ECX,DWORD PTR [ESP+84]
            call @Vec3_AddAssign@16

            fld DWORD PTR [ESP+20]
            fcomp QWORD PTR [(_KingRdata+1760)]
            fnstsw AX
            test AH,1
            je $L_55ae6b

            mov DWORD PTR [EBX+21892],EDI
            jmp $L_55ae6b
$L_55ac40::

            fld ST(0)
            fmul DWORD PTR [(_KingBss+491032)]
            push ECX
            fstp DWORD PTR [ESP]
            push ECX
            fld ST(0)
            fmul DWORD PTR [(_KingBss+491028)]
            fstp DWORD PTR [ESP]
            push ECX
            lea ECX,DWORD PTR [ESP+836]
            fmul DWORD PTR [(_KingBss+491024)]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [(_KingBss+491016)]
            mov ECX,DWORD PTR [ESP+828]
            mov EAX,DWORD PTR [ESP+824]
            fmul DWORD PTR [(_KingRdata+4944)]
            push ECX
            mov EDX,DWORD PTR [ESP+836]
            mov DWORD PTR [ESP+608],ECX
            mov DWORD PTR [ESP+604],EAX
            mov DWORD PTR [ESP+612],EDX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [(_KingBss+491012)]
            fmul DWORD PTR [(_KingRdata+4944)]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [(_KingBss+491008)]
            fmul DWORD PTR [(_KingRdata+4944)]
            push ECX
            lea ECX,DWORD PTR [ESP+860]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+856]
            fsub DWORD PTR [ESP+608]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+856]
            fsub DWORD PTR [ESP+608]
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+856]
            fsub DWORD PTR [ESP+608]
            push ECX
            lea ECX,DWORD PTR [ESP+884]
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+56]
            fmul QWORD PTR [(_KingRdata+1976)]
            fld DWORD PTR [ESP+880]
            push ECX
            fmul ST(0),ST(1)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+880]
            fmul ST(0),ST(1)
            push ECX
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+880]
            fmul ST(0),ST(1)
            push ECX
            lea ECX,DWORD PTR [ESP+428]
            fstp DWORD PTR [ESP]
            fstp ST(0)
            call @Vec3_SetThis@20

            fld DWORD PTR [ESP+424]
            fmul DWORD PTR [ESP+316]
            fld DWORD PTR [ESP+420]
            fmul DWORD PTR [ESP+304]
            push ECX
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+420]
            fmul DWORD PTR [ESP+296]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+428]
            fmul DWORD PTR [ESP+316]
            fld DWORD PTR [ESP+424]
            fmul DWORD PTR [ESP+304]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+420]
            fmul DWORD PTR [ESP+292]
            push ECX
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+424]
            fmul DWORD PTR [ESP+292]
            fld DWORD PTR [ESP+432]
            fmul DWORD PTR [ESP+316]
            push ECX
            lea ECX,DWORD PTR [ESP+1196]
            faddp ST(1),ST(0)
            fld DWORD PTR [ESP+432]
            fmul DWORD PTR [ESP+308]
            faddp ST(1),ST(0)
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            lea EAX,DWORD PTR [ESP+1184]
            lea ECX,DWORD PTR [ESP+1628]
            push EAX
            push ECX
            lea ECX,DWORD PTR [ESP+84]
            call @Vec3_AddAssign@16

            fld DWORD PTR [ESP+24]
            fmul DWORD PTR [ESP+24]
            push ECX
            fsubr QWORD PTR [(_KingRdata+1648)]
            fld ST(0)
            fmul DWORD PTR [ESP+88]
            fstp DWORD PTR [ESP]
            push ECX
            fld ST(0)
            fmul DWORD PTR [ESP+88]
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+84]
            fmul ST(0),ST(1)
            push ECX
            lea ECX,DWORD PTR [ESP+908]
            fstp DWORD PTR [ESP]
            fstp ST(0)
            call @Vec3_SetThis@20

            mov EDX,DWORD PTR [ESP+896]
            mov EAX,DWORD PTR [ESP+900]
            mov ECX,DWORD PTR [ESP+904]
            mov DWORD PTR [ESP+76],EDX
            mov DWORD PTR [ESP+80],EAX
            mov DWORD PTR [ESP+84],ECX
$L_55ae6b::

            mov EAX,DWORD PTR [ESP+440]
            lea EDX,DWORD PTR [EAX+EAX*2]
            mov EAX,DWORD PTR [EBX+21600]
            lea ECX,DWORD PTR [EAX+EDX*4+10244]
            mov EDX,DWORD PTR [ESP+76]
            mov DWORD PTR [ECX],EDX
            mov EAX,DWORD PTR [ESP+80]
            mov DWORD PTR [ECX+4],EAX
            mov EDX,DWORD PTR [ESP+84]
            push ECX
            mov DWORD PTR [ECX+8],EDX
            fld DWORD PTR [ESP+88]
            fchs 
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+84]
            push ECX
            fchs 
            fstp DWORD PTR [ESP]
            fld DWORD PTR [ESP+84]
            push ECX
            lea ECX,DWORD PTR [ESP+932]
            fchs 
            fstp DWORD PTR [ESP]
            call @Vec3_SetThis@20

            mov EAX,DWORD PTR [ESP+220]
            mov ECX,DWORD PTR [EBP+21600]
            lea EAX,DWORD PTR [EAX+EAX*2]
            lea EDX,DWORD PTR [ECX+EAX*4+10244]
            mov EAX,DWORD PTR [ESP+920]
            mov DWORD PTR [EDX],EAX
            mov ECX,DWORD PTR [ESP+924]
            mov DWORD PTR [EDX+4],ECX
            mov EAX,DWORD PTR [ESP+928]
            mov DWORD PTR [EDX+8],EAX
$L_55aef4::

            mov EAX,DWORD PTR [ESP+32]
            mov ECX,DWORD PTR [EBX+21600]
            test EAX,EAX
            je $L_55af0d

            call @Body_StateStep@4

            mov DWORD PTR [ESP+32],EAX
            jmp $L_55af31
$L_55af0d::

            mov EAX,DWORD PTR [ECX+13124]
            test EAX,EAX
            je $L_55af31

            mov ECX,DWORD PTR [EBP+21600]
            call @Body_Get9508@8

            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            fstp DWORD PTR [ESP]
            call @Body_FrameIntegrate@12
$L_55af31::

            mov EDI,DWORD PTR [ESP+72]
            test EDI,EDI
            je $L_55af4c

            mov ECX,DWORD PTR [EBP+21600]
            call @Body_StateStep@4

            mov EDI,EAX
            mov DWORD PTR [ESP+72],EDI
            jmp $L_55af76
$L_55af4c::

            mov EDX,DWORD PTR [EBP+21600]
            mov EAX,DWORD PTR [EDX+13124]
            test EAX,EAX
            je $L_55af76

            mov ECX,DWORD PTR [EBX+21600]
            call @Body_Get9508@8

            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            fstp DWORD PTR [ESP]
            call @Body_FrameIntegrate@12
$L_55af76::

            mov ESI,DWORD PTR [ESP+28]
            test ESI,ESI
            jne $L_55b094

            cmp DWORD PTR [EBX+21892],-1
            je $L_55b094

            mov EAX,DWORD PTR [ESP+32]
            cmp EAX,1
            jg $L_55afa1

            cmp EAX,-1
            jne $L_55b180
$L_55afa1::

            cmp EDI,1
            jg $L_55afaf

            cmp EDI,-1
            jne $L_55b180
$L_55afaf::

            mov EAX,DWORD PTR [ESP+36]
            lea ECX,DWORD PTR [ESP+1736]
            push EAX
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            call @Body_GetVec9084@12

            lea EDX,DWORD PTR [ESP+1128]
            push EAX
            push EDX
            call _Vec3_DivScalar

            mov EAX,DWORD PTR [ESP+52]
            add ESP,12
            lea ECX,DWORD PTR [ESP+1328]
            push EAX
            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            call @Body_GetVec9084@12

            lea EDX,DWORD PTR [ESP+1152]
            push EAX
            push EDX
            call _Vec3_DivScalar

            add ESP,12
            lea EAX,DWORD PTR [ESP+404]
            lea ECX,DWORD PTR [ESP+1352]
            push EAX
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            call @Body_GetVec9144@12

            lea EDX,DWORD PTR [ESP+1212]
            push EAX
            push EDX
            call _Vec3_DivComponents

            add ESP,12
            lea EAX,DWORD PTR [ESP+392]
            lea ECX,DWORD PTR [ESP+1376]
            push EAX
            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            call @Body_GetVec9144@12

            lea EDX,DWORD PTR [ESP+1176]
            push EAX
            push EDX
            call _Vec3_DivComponents

            add ESP,12
            lea EAX,DWORD PTR [ESP+1208]
            lea ECX,DWORD PTR [ESP+1124]
            push EAX
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            call @Body_Integrate@16

            mov ECX,DWORD PTR [EBP+21600]
            lea EDX,DWORD PTR [ESP+1172]
            lea EAX,DWORD PTR [ESP+1148]
            push EDX
            push EAX
            call @Body_Integrate@16

            jmp $L_55b17c
$L_55b094::

            mov ECX,DWORD PTR [ESP+36]
            lea EDX,DWORD PTR [ESP+1400]
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            push EDX
            call @Body_GetVec9084@12

            push EAX
            lea EAX,DWORD PTR [ESP+1120]
            push EAX
            call _Vec3_DivScalar

            mov ECX,DWORD PTR [ESP+52]
            add ESP,12
            lea EDX,DWORD PTR [ESP+1424]
            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            push EDX
            call @Body_GetVec9084@12

            push EAX
            lea EAX,DWORD PTR [ESP+1168]
            push EAX
            call _Vec3_DivScalar

            add ESP,12
            lea ECX,DWORD PTR [ESP+404]
            lea EDX,DWORD PTR [ESP+1448]
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            push EDX
            call @Body_GetVec9144@12

            push EAX
            lea EAX,DWORD PTR [ESP+1204]
            push EAX
            call _Vec3_DivComponents

            add ESP,12
            lea ECX,DWORD PTR [ESP+392]
            lea EDX,DWORD PTR [ESP+1472]
            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            push EDX
            call @Body_GetVec9144@12

            push EAX
            lea EAX,DWORD PTR [ESP+1144]
            push EAX
            call _Vec3_DivComponents

            add ESP,12
            lea ECX,DWORD PTR [ESP+1196]
            lea EDX,DWORD PTR [ESP+1112]
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            push EDX
            call @Body_Integrate@16

            lea EAX,DWORD PTR [ESP+1136]
            lea ECX,DWORD PTR [ESP+1160]
            push EAX
            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            call @Body_Integrate@16

            test ESI,ESI
            jne $L_55b561
$L_55b17c::

            mov EAX,DWORD PTR [ESP+32]
$L_55b180::

            mov ECX,DWORD PTR [EBX+21600]
            mov EDX,DWORD PTR [ECX+13124]
            test EDX,EDX
            jne $L_55b561

            mov EDX,DWORD PTR [EBP+21600]
            mov ESI,DWORD PTR [EDX+13124]
            test ESI,ESI
            jne $L_55b561

            mov EDX,DWORD PTR [EBX+21892]
            mov DWORD PTR [ESP+28],0
            cmp EDX,-1
            je $L_55b4f2

            mov EDX,DWORD PTR [(_KingBss+412328)]
            test EDX,EDX
            je $L_55b4f2

            cmp EAX,1
            jg $L_55b1db

            cmp EAX,-1
            jne $L_55b4f2
$L_55b1db::

            cmp EDI,1
            jg $L_55b1e9

            cmp EDI,-1
            jne $L_55b4f2
$L_55b1e9::

            lea EAX,DWORD PTR [ESP+2228]
            push EAX
            call @Body_Matrix22a4@12

            mov ECX,12
            mov ESI,EAX
            lea EDI,DWORD PTR [ESP+284]

            rep movsd 

            lea ECX,DWORD PTR [ESP+2084]
            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            call @Body_Matrix22a4@12

            mov ECX,12
            mov ESI,EAX
            lea EDI,DWORD PTR [ESP+148]
            lea EDX,DWORD PTR [ESP+2180]

            rep movsd 

            mov ECX,DWORD PTR [EBX+21600]
            push EDX
            call @Body_Matrix2274@12

            mov ECX,12
            mov ESI,EAX
            lea EDI,DWORD PTR [ESP+1220]
            lea EAX,DWORD PTR [ESP+2276]

            rep movsd 

            mov ECX,DWORD PTR [EBP+21600]
            push EAX
            call @Body_Matrix2274@12

            mov ECX,12
            mov ESI,EAX
            lea EDI,DWORD PTR [ESP+1268]
            lea EDX,DWORD PTR [ESP+196]

            rep movsd 

            lea ECX,DWORD PTR [ESP+284]
            lea EAX,DWORD PTR [ESP+1496]
            push ECX
            push EDX
            push EAX
            call _Vec3_RotateBy

            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [ESP+260],ECX
            lea ECX,DWORD PTR [ESP+160]
            mov EDX,DWORD PTR [EAX+4]
            push ECX
            mov DWORD PTR [ESP+268],EDX
            lea EDX,DWORD PTR [ESP+224]
            mov EAX,DWORD PTR [EAX+8]
            push EDX
            mov DWORD PTR [ESP+276],EAX
            lea EAX,DWORD PTR [ESP+1540]
            push EAX
            call _Vec3_RotateBy

            mov ECX,DWORD PTR [EAX]
            add ESP,24
            mov DWORD PTR [ESP+272],ECX
            lea ECX,DWORD PTR [ESP+1544]
            mov EDX,DWORD PTR [EAX+4]
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            mov DWORD PTR [ESP+280],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [ESP+284],EAX
            call @Body_GetVec9012@12

            mov EDX,DWORD PTR [EAX]
            mov DWORD PTR [ESP+564],EDX
            mov ECX,DWORD PTR [EAX+4]
            mov DWORD PTR [ESP+568],ECX
            mov EDX,DWORD PTR [EAX+8]
            lea EAX,DWORD PTR [ESP+1568]
            mov DWORD PTR [ESP+572],EDX
            mov ECX,DWORD PTR [EBP+21600]
            push EAX
            call @Body_GetVec9012@12

            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [ESP+540],ECX
            lea ECX,DWORD PTR [ESP+1592]
            mov EDX,DWORD PTR [EAX+4]
            push ECX
            mov ECX,DWORD PTR [EBX+21600]
            mov DWORD PTR [ESP+548],EDX
            mov EAX,DWORD PTR [EAX+8]
            mov DWORD PTR [ESP+552],EAX
            call @Body_GetVec9024@12

            mov EDX,DWORD PTR [EAX]
            mov DWORD PTR [ESP+528],EDX
            mov ECX,DWORD PTR [EAX+4]
            mov DWORD PTR [ESP+532],ECX
            mov EDX,DWORD PTR [EAX+8]
            lea EAX,DWORD PTR [ESP+1616]
            mov DWORD PTR [ESP+536],EDX
            mov ECX,DWORD PTR [EBP+21600]
            push EAX
            call @Body_GetVec9024@12

            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [ESP+552],ECX
            lea ECX,DWORD PTR [ESP+516]
            mov EDX,DWORD PTR [EAX+4]
            push ECX
            mov DWORD PTR [ESP+560],EDX
            lea EDX,DWORD PTR [ESP+508]
            mov EAX,DWORD PTR [EAX+8]
            push EDX
            mov DWORD PTR [ESP+568],EAX
            lea EAX,DWORD PTR [ESP+232]
            lea ECX,DWORD PTR [ESP+488]
            push EAX
            lea EDX,DWORD PTR [ESP+404]
            push ECX
            lea EAX,DWORD PTR [ESP+420]
            push EDX
            lea ECX,DWORD PTR [ESP+1288]
            push EAX
            lea EDX,DWORD PTR [ESP+1244]
            push ECX
            lea EAX,DWORD PTR [ESP+176]
            push EDX
            lea ECX,DWORD PTR [ESP+316]
            push EAX
            lea EDX,DWORD PTR [ESP+588]
            push ECX
            lea EAX,DWORD PTR [ESP+568]
            push EDX
            lea ECX,DWORD PTR [ESP+584]
            push EAX
            lea EDX,DWORD PTR [ESP+612]
            push ECX
            lea EAX,DWORD PTR [ESP+324]
            push EDX
            mov EDX,DWORD PTR [ESP+96]
            lea ECX,DWORD PTR [ESP+304]
            push EAX
            mov EAX,DWORD PTR [ESP+96]
            push ECX
            push EDX
            push EAX
            call DWORD PTR [(_KingData+58540)]

            mov EDX,DWORD PTR [ESP+552]
            mov EAX,DWORD PTR [ESP+556]
            add ESP,60
            mov ECX,ESP
            mov DWORD PTR [ECX],EDX
            mov EDX,DWORD PTR [ESP+500]
            mov DWORD PTR [ECX+4],EAX
            mov DWORD PTR [ECX+8],EDX
            mov ECX,DWORD PTR [EBX+21600]
            call @Body_SetVec9012@20

            mov ECX,DWORD PTR [ESP+504]
            mov EDX,DWORD PTR [ESP+508]
            sub ESP,12
            mov EAX,ESP
            mov DWORD PTR [EAX],ECX
            mov ECX,DWORD PTR [ESP+524]
            mov DWORD PTR [EAX+4],EDX
            mov DWORD PTR [EAX+8],ECX
            mov ECX,DWORD PTR [EBX+21600]
            call @Body_SetSpin@20

            mov EAX,DWORD PTR [ESP+224]
            mov ECX,DWORD PTR [ESP+228]
            sub ESP,12
            mov EDX,ESP
            mov DWORD PTR [EDX],EAX
            mov EAX,DWORD PTR [ESP+244]
            mov DWORD PTR [EDX+4],ECX
            mov ECX,DWORD PTR [EBP+21600]
            mov DWORD PTR [EDX+8],EAX
            call @Body_SetVec9012@20

            mov EDX,DWORD PTR [ESP+516]
            mov EAX,DWORD PTR [ESP+520]
            sub ESP,12
            mov ECX,ESP
            mov DWORD PTR [ECX],EDX
            mov EDX,DWORD PTR [ESP+536]
            mov DWORD PTR [ECX+4],EAX
            mov DWORD PTR [ECX+8],EDX
            mov ECX,DWORD PTR [EBP+21600]
            call @Body_SetSpin@20

            mov EDI,DWORD PTR [ESP+72]
            mov EAX,DWORD PTR [ESP+32]
$L_55b4f2::

            test EAX,EAX
            jne $L_55b4fe

            test EDI,EDI
            je $L_559478
$L_55b4fe::

            mov ECX,DWORD PTR [EBX+21600]
            lea EAX,DWORD PTR [ESP+1892]
            push EAX
            call @Body_Matrix22a4@12

            lea ECX,DWORD PTR [ESP+1844]
            push ECX
            mov ECX,DWORD PTR [EBP+21600]
            call @Body_Matrix22a4@12

            lea EDX,DWORD PTR [ESP+1844]
            lea EAX,DWORD PTR [ESP+1892]
            push EDX
            push EAX
            mov ECX,EBX
            call @Hitch_Align@16

            neg EAX
            sbb EAX,EAX
            lea ECX,DWORD PTR [ESP+1844]
            lea EDX,DWORD PTR [ESP+1892]
            push ECX
            neg EAX
            push EDX
            mov ECX,EBX
            mov DWORD PTR [ESP+36],EAX
            call @Car_PlaceWithTrailer@16

            jmp $L_559478
$L_55b561::

            mov DWORD PTR [ESP+28],1
            jmp $L_559478
$L_55b56e::

            mov ECX,DWORD PTR [ESP+28]
            xor EAX,EAX
            pop EDI
            pop ESI
            test ECX,ECX
            pop EBP
            pop EBX
            setne AL
            add ESP,2308
            ret 
FUN_558f20 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; engine: src\truck\vehicle_engine.cpp, nmake ASM_VEHICLE_ENGINE=1 assembles the original
ALIGN 16
; engine: src\truck\vehicle_engine.cpp, nmake ASM_VEHICLE_ENGINE=1 assembles the original
ALIGN 16
ALIGN 16
; engine: src\truck\vehicle_engine.cpp, nmake ASM_VEHICLE_ENGINE=1 assembles the original
ALIGN 16
_TEXT ENDS
END
