; asm_gzip.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN ??2@YAPAXI@Z:NEAR
EXTERN ??3@YAXPAX@Z:NEAR
EXTERN _strrchr:PROC
EXTERN _perror:PROC
EXTERN _remove:PROC
EXTERN _Gz_BiInit:PROC
EXTERN _Gz_CtInit:PROC
EXTERN _Gz_CtTally:PROC
EXTERN _Gz_FillWindow:PROC
EXTERN _Gz_FlushBlock:PROC
EXTERN _Gz_Inflate:PROC
EXTERN _Gz_LongestMatch:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _OldCrt_fprintf:PROC
EXTERN __errno:PROC
EXTERN _exit:PROC
EXTERN _free:PROC

PUBLIC $L_62a927, $L_62a948, $L_62a9d1, $L_62a9f5, $L_62aa37, $L_62aa61, $L_62aa75, $L_62aa83
PUBLIC $L_62aac0, $L_62ab53, $L_62abb3, $L_62ac1d, $L_62ac42, $L_62ac8e, $L_62aca1, $L_62aca9
PUBLIC $L_62acbc, $L_62acbf, $L_62ace1, $L_62acf4, $L_62acfd, $L_62ad0f, $L_62ada8, $L_62adb3
PUBLIC $L_62addd, $L_62ae0a, $L_62ae3a, $L_62ae80, $L_62ae9e, $L_62aeb2, $L_62aee4, $L_62af0a
PUBLIC $L_62af1d, $L_62af36, $L_62af8b, $L_62af91, $L_62afd7, $L_62afdd, $L_62aff8, $L_62b009
PUBLIC $L_62b022, $L_62b031, $L_62b050, $L_62b05f, $L_62b07e, $L_62b08d, $L_62b0a2, $L_62b0b3
PUBLIC $L_62b0c2, $L_62b0d3, $L_62b0e2, $L_62b105, $L_62b116, $L_62b12f, $L_62b13e, $L_62b170
PUBLIC $L_62b196, $L_62b1a7, $L_62b1c0, $L_62b1cf, $L_62b201, $L_62b209, $L_62b21a, $L_62b229
PUBLIC $L_62b22c, $L_62b259, $L_62b277, $L_62b297, $L_62b2a0, $L_62b2be, $L_62b2d0, $L_62b2d6
PUBLIC $L_62b2df, $L_62b30f, $L_62b31e, $L_62b324, $L_62b333, $L_62b337, $L_62b349, $L_62b357
PUBLIC $L_62b374, $L_62b39a, $L_62b3ac, $L_62b3b1, $L_62b3d3, $L_62b3f9, $L_62b415, $L_62b440
PUBLIC $L_62b4af, $L_62b541, $L_62b55f, $L_62b57d, $L_62b59c, $L_62b5a3, $L_62b5d4, $L_62b5f0
PUBLIC $L_62b617, $L_62b635, $L_62b65d, $L_62b681, $L_62b6a2, $L_62b6be, $L_62b6e5, $L_62b708
PUBLIC $L_62b730, $L_62b74f, $L_62b7b1, $L_62b7de, $L_62b81e, $L_62b823, $L_62b843, $L_62b854
PUBLIC $L_62b885, $L_62b893, $L_62b8b2, $L_62b91b, $L_62b958, $L_62b95e, $L_62b96e, $L_62b979
PUBLIC $L_62b9d9, $L_62ba1a, $L_62ba36, $L_62ba61, $L_62ba6b, $L_62ba80, $L_62baa7, $L_62bab9
PUBLIC $L_62bacb, $L_62bb32, $L_62bb44, $L_62bba2, $L_62bbd1, $L_62bbda, $L_62bc54, $L_62bc80
PUBLIC $L_62bc95, $L_62bca1, $L_62bcb0, $L_62bcef, $L_62bd20, $L_62bd2c, $L_62bd3d, $L_62bd6c
PUBLIC $L_62bd99, $L_62bda5, $L_62bdaf, $L_62bde2, $L_62be00, $L_62be32, $L_62be42, $L_62be4c
PUBLIC $L_62be58, $L_62bede, $L_62befb, $L_62bf02, $L_62bf20, $L_62bf2a, $L_62bf31, $L_62bf34
PUBLIC $L_62bf76, $L_62bf7b, $L_62bf9d, $L_62bfdc, $L_62bff3, $L_62c001, $L_62c031, $L_62c04b
PUBLIC $L_62c070, $L_62c074, $L_62c08f, $L_62c0c1, $L_62c0e3, $L_62c13f, $L_62c159, $L_62c174
PUBLIC $L_62c188, $L_62c194, $L_62c199, $L_62c1a5, $L_62c1b4, $L_62c1c1, $L_62c21a, $L_62c226
PUBLIC $L_62c24b, $L_62c25b, $L_62c265, $L_62c26e, $L_62c28f, $L_62c2a3, $L_62c2b0, $L_62c2d2
PUBLIC $L_62c2dc, $L_62c2ed, $L_62c30a, $L_62c356, $L_62c366, $L_62c370, $L_62c379, $L_62c3a8
PUBLIC $L_62c3da, $L_62c3f1, $L_62c440, $L_62c453, $L_62c4a2, $L_62c4fc, $L_62c507, $L_62c511
PUBLIC $L_62c520, $L_62c532, $L_62c56e, $L_62c588, $L_62c5a1, $L_62c5af, $L_62c5be, $L_62c626
PUBLIC $L_62c642, $L_62c657, $L_62c696, $L_62c6a9, $L_62c6b2, $L_62c6ca, $L_62c6d2, $L_62c6ef
PUBLIC $L_62c718, $L_62c725, $L_62c74d, $L_62c782, $L_62c78e, $L_62c7a5, $L_62c7c7, $L_62c7f5
PUBLIC $L_62c80a, $L_62c852, $L_62c871, $L_62c8d2, $L_62c90a, $L_62c97d, $L_62c981, $L_62c99a
PUBLIC $L_62c9bc, $L_62c9d4, $L_62c9f7, $L_62ca0c, $L_62ca20, $L_62ca39, $L_62ca55, $L_62ca6d
PUBLIC $L_62ca85, $L_62ca93, $L_62cad6, $L_62cae7, $L_62cb0c, $L_62cb28, $L_62cb41, $L_62cb51
PUBLIC $L_62cb84, $L_62cb9c, $L_62cbba, $L_62cbca, $L_62cbe3, $L_62cbf9, $L_62cc26, $L_62cc43
PUBLIC $L_62cc55, $L_62cc6e, $L_62cc84, $L_62cc8e, $L_62ccaa, $L_62ccd1, $L_62ccf9, $L_62cd03
PUBLIC $L_62cd26, $L_62cd38, $L_62cd44, $L_62cd4d, $L_62cd56, $L_62cd61, $L_62ce07, $L_62ce41
PUBLIC $L_62ce58, $L_62ce69, $L_62cf2c, $L_62cf4a, $L_62cf5c, $L_62cff4, $L_62cffc, $L_62d01a
PUBLIC $L_62d024, $L_62d044, $L_62d047, $L_62d050, $L_62d055, $L_62d05d, $L_62d071, $L_62d08a
PUBLIC $L_62d099, $L_62d10d, $L_62d121, $L_62d13a, $L_62d149, $L_62d1b8, $L_62d1bf, $L_62d1de
PUBLIC $L_62d1f3, $L_62d26f, $L_62d281, $L_62d297, $L_62d2f0, $L_62d2fd, $L_62d35b, $L_62d367
PUBLIC $L_62d3a5, $L_62d3ba, $L_62d3be, $L_62d3e6, $L_62d671, $L_62d68d, $L_62d70e, $L_62d734
PUBLIC $L_62d73a, $L_62d740, $L_62d78d, $L_62d800, $L_62d811, $L_62d813, $L_62d838, $L_62d869
PUBLIC $L_62d86b, $L_62d88d, $L_62d893, $L_62d8ad, $L_62d8c8, $L_62d8d4, $L_62d8fb, $L_62d91b
PUBLIC $L_62d921, $L_62d942, $L_62d988, $L_62d9ec, $L_62d9f2, $L_62da36, $L_62da80, $L_62dab8
PUBLIC $L_62daed, $L_62db03, $L_62db05, $L_62db25, $L_62db31, $L_62db58, $L_62db64, $L_62db6a
PUBLIC $L_62db8a

_TEXT SEGMENT
FUN_62a910 PROC

            mov ECX,DWORD PTR [ESP+4]
            test ECX,ECX
            je $L_62a927

            mov EDX,DWORD PTR [ESP+12]
            mov EAX,DWORD PTR [ECX]
            push EDX
            mov EDX,DWORD PTR [ESP+12]
            push EDX
            call DWORD PTR [EAX]

            ret 
$L_62a927::

            or EAX,4294967295
            ret 
FUN_62a910 ENDP
ALIGN 16
FUN_62a930 PROC

            mov ECX,DWORD PTR [ESP+4]
            test ECX,ECX
            je $L_62a948

            mov EDX,DWORD PTR [ESP+12]
            mov EAX,DWORD PTR [ECX]
            push EDX
            mov EDX,DWORD PTR [ESP+12]
            push EDX
            call DWORD PTR [EAX+4]

            ret 
$L_62a948::

            mov EAX,DWORD PTR [ESP+12]
            ret 
FUN_62a930 ENDP
ALIGN 16
FUN_62a950 PROC

            push ESI
            mov ESI,DWORD PTR [ESP+8]
            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [ECX]
            call DWORD PTR [EAX+12]

            mov ECX,DWORD PTR [ESI]
            push ECX
            call ??3@YAXPAX@Z

            add ESP,4
            mov DWORD PTR [ESI],0
            xor EAX,EAX
            pop ESI
            ret 
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
            nop
            nop
            nop
            nop
FUN_62a950 ENDP
ALIGN 16
FUN_62a980 PROC

            mov EDX,DWORD PTR [ESP+8]
            mov EAX,ECX
            mov ECX,DWORD PTR [ESP+4]
            mov BYTE PTR [EAX+16],1
            mov DWORD PTR [EAX],OFFSET (_KingRdata+41568)
            mov DWORD PTR [EAX+12],ECX
            mov DWORD PTR [EAX+4],0
            mov DWORD PTR [EAX+8],EDX
            ret 8
FUN_62a980 ENDP
$L_62a9b0 PROC

            mov EDX,ECX
            push EDI
            mov EDI,DWORD PTR [EDX+12]
            test EDI,EDI
            je $L_62a9f5

            mov EAX,DWORD PTR [ESP+12]
            mov ECX,DWORD PTR [EDX+8]
            push EBX
            push ESI
            mov ESI,DWORD PTR [EDX+4]
            lea EBX,DWORD PTR [ESI+EAX*1]
            cmp EBX,ECX
            jb $L_62a9d1

            sub ECX,ESI
            mov EAX,ECX
$L_62a9d1::

            mov ECX,EAX
            add ESI,EDI
            mov EDI,DWORD PTR [ESP+16]
            mov EBX,ECX
            shr ECX,2

            rep movsd 

            mov ECX,EBX
            and ECX,3

            rep movsb 

            mov ECX,DWORD PTR [EDX+4]
            pop ESI
            add ECX,EAX
            pop EBX
            mov DWORD PTR [EDX+4],ECX
            pop EDI
            ret 8
$L_62a9f5::

            or EAX,4294967295
            pop EDI
            ret 8

            nop
            nop
            nop
            nop
$L_62a9b0 ENDP
$L_62aa00 PROC

            mov EDX,ECX
            push EDI
            mov EDI,DWORD PTR [EDX+12]
            test EDI,EDI
            je $L_62aa37

            mov EAX,DWORD PTR [ESP+12]
            push EBX
            mov EBX,DWORD PTR [EDX+4]
            mov ECX,EAX
            add EDI,EBX
            mov EBX,ECX
            push ESI
            mov ESI,DWORD PTR [ESP+16]
            shr ECX,2

            rep movsd 

            mov ECX,EBX
            and ECX,3

            rep movsb 

            mov ECX,DWORD PTR [EDX+4]
            pop ESI
            add ECX,EAX
            pop EBX
            mov DWORD PTR [EDX+4],ECX
            pop EDI
            ret 8
$L_62aa37::

            or EAX,4294967295
            pop EDI
            ret 8
$L_62aa00 ENDP
$L_62aa40 PROC

            mov EAX,DWORD PTR [ECX+12]
            test EAX,EAX
            je $L_62aa83

            mov EAX,DWORD PTR [ESP+8]
            cmp EAX,1
            jne $L_62aa61

            mov EAX,DWORD PTR [ESP+4]
            mov EDX,DWORD PTR [ECX+4]
            add EDX,EAX
            mov DWORD PTR [ECX+4],EDX
            mov EAX,EDX
            ret 8
$L_62aa61::

            cmp EAX,2
            jne $L_62aa75

            mov EAX,DWORD PTR [ECX+8]
            mov EDX,DWORD PTR [ESP+4]
            sub EAX,EDX
            mov DWORD PTR [ECX+4],EAX
            ret 8
$L_62aa75::

            test EAX,EAX
            jne $L_62aa83

            mov EAX,DWORD PTR [ESP+4]
            mov DWORD PTR [ECX+4],EAX
            ret 8
$L_62aa83::

            or EAX,4294967295
            ret 8
$L_62aa40 ENDP
$L_62aa90 PROC

            mov EDX,DWORD PTR [ECX+12]
            xor EAX,EAX
            test EDX,EDX
            sete AL
            ret 
$L_62aa90 ENDP
ALIGN 16
FUN_62aaa0 PROC

            push 20
            call ??2@YAPAXI@Z

            add ESP,4
            test EAX,EAX
            je $L_62aac0

            mov ECX,DWORD PTR [ESP+8]
            mov EDX,DWORD PTR [ESP+4]
            push ECX
            push EDX
            mov ECX,EAX
            call FUN_62a980

            ret 
$L_62aac0::

            xor EAX,EAX
            ret 
FUN_62aaa0 ENDP
ALIGN 16
FUN_62aad0 PROC

            xor EAX,EAX
            or ECX,4294967295
            mov DWORD PTR [(_KingBss+985008)],EAX
            mov DWORD PTR [(_KingBss+985012)],EAX
            mov DWORD PTR [(_KingBss+985016)],EAX
            mov DWORD PTR [(_KingBss+985020)],EAX
            mov DWORD PTR [(_KingData+120776)],ECX
            mov DWORD PTR [(_KingData+120780)],ECX
            mov DWORD PTR [(_KingBss+985024)],EAX
            mov DWORD PTR [(_KingBss+985028)],EAX
            mov DWORD PTR [(_KingBss+985032)],EAX
            mov DWORD PTR [(_KingBss+985036)],EAX
            mov DWORD PTR [(_KingBss+985040)],EAX
            mov DWORD PTR [(_KingBss+985044)],EAX
            mov DWORD PTR [(_KingData+120784)],16
            mov DWORD PTR [(_KingData+120788)],8
            mov DWORD PTR [(_KingData+120792)],6
            mov DWORD PTR [(_KingBss+985048)],EAX
            mov DWORD PTR [(_KingBss+985064)],EAX
            ret 
FUN_62aad0 ENDP
ALIGN 16
FUN_62ab40 PROC

            mov EAX,DWORD PTR [(_KingBss+985068)]
            test EAX,EAX
            jne $L_62ab53

            mov DWORD PTR [(_KingBss+985068)],1
$L_62ab53::

            ret 
FUN_62ab40 ENDP
ALIGN 16
FUN_62ab60 PROC

            push ESI
            push EDI
            call FUN_62ab40

            call FUN_62aad0

            mov ESI,DWORD PTR [ESP+12]
            mov EDI,DWORD PTR [ESP+16]
            mov DWORD PTR [(_KingBss+985016)],0
            mov DWORD PTR [(_KingBss+671248)],ESI
            mov DWORD PTR [(_KingBss+704084)],EDI
            call FUN_62b8c0

            push EDI
            push ESI
            mov DWORD PTR [(_KingBss+704092)],0
            call FUN_62b450

            add ESP,8
            test EAX,EAX
            pop EDI
            pop ESI
            je $L_62abb3

            mov DWORD PTR [(_KingData+120788)],4294967295
$L_62abb3::

            xor EAX,EAX
            ret 
FUN_62ab60 ENDP
ALIGN 16
FUN_62abc0 PROC

            call FUN_62ab40

            call FUN_62aad0

            mov EAX,DWORD PTR [ESP+4]
            mov ECX,DWORD PTR [ESP+8]
            mov DWORD PTR [(_KingBss+985016)],1
            mov DWORD PTR [(_KingBss+671248)],EAX
            mov DWORD PTR [(_KingBss+704084)],ECX
            call FUN_62b8c0

            mov EDX,DWORD PTR [ESP+4]
            mov DWORD PTR [(_KingBss+704092)],0
            push EDX
            call FUN_62ac50

            add ESP,4
            mov DWORD PTR [(_KingData+120788)],EAX
            test EAX,EAX
            jge $L_62ac1d

            lea EAX,DWORD PTR [ESP+4]
            push EAX
            call FUN_62a950

            add ESP,4
            or EAX,4294967295
            ret 
$L_62ac1d::

            mov ECX,DWORD PTR [(_KingBss+704084)]
            mov EDX,DWORD PTR [(_KingBss+671248)]
            push ECX
            push EDX
            call DWORD PTR [(_KingData+121184)]

            add ESP,8
            test EAX,EAX
            je $L_62ac42

            mov DWORD PTR [(_KingData+120788)],4294967295
$L_62ac42::

            xor EAX,EAX
            ret 
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
            nop
FUN_62abc0 ENDP
ALIGN 16
FUN_62ac50 PROC

            push ECX
            mov EAX,DWORD PTR [(_KingBss+985020)]
            push EBX
            push EBP
            xor EBP,EBP
            push ESI
            cmp EAX,EBP
            mov ESI,1
            je $L_62acbf

            cmp DWORD PTR [(_KingBss+985012)],EBP
            je $L_62acbf

            mov ECX,DWORD PTR [(_KingBss+704100)]
            mov EAX,DWORD PTR [(_KingBss+704096)]
            cmp ECX,EAX
            jae $L_62ac8e

            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov BYTE PTR [ESP+14],AL
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62aca1
$L_62ac8e::

            push ESI
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
            mov BYTE PTR [ESP+14],AL
$L_62aca1::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62acbc
$L_62aca9::

            mov DL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov BYTE PTR [ESP+15],DL
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62ad0f
$L_62acbc::

            push ESI
            jmp $L_62acfd
$L_62acbf::

            mov ECX,DWORD PTR [(_KingBss+704100)]
            mov EAX,DWORD PTR [(_KingBss+704096)]
            cmp ECX,EAX
            jae $L_62ace1

            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov BYTE PTR [ESP+14],AL
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62acf4
$L_62ace1::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
            mov BYTE PTR [ESP+14],AL
$L_62acf4::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jb $L_62aca9

            push EBP
$L_62acfd::

            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
            mov BYTE PTR [ESP+15],AL
$L_62ad0f::

            mov EBX,DWORD PTR [(_KingBss+704092)]
            lea EDX,DWORD PTR [ESP+14]
            mov EAX,OFFSET (_KingData+121996)
            inc EBX
            mov DX,WORD PTR [EDX]
            mov DWORD PTR [(_KingData+120788)],4294967295
            cmp DX,WORD PTR [EAX]
            mov DWORD PTR [(_KingBss+704092)],EBX
            mov DWORD PTR [(_KingBss+985080)],EBP
            mov DWORD PTR [(_KingBss+919152)],EBP
            je $L_62ae80

            lea EDX,DWORD PTR [ESP+14]
            mov EAX,OFFSET (_KingData+121992)
            mov DX,WORD PTR [EDX]
            cmp DX,WORD PTR [EAX]
            je $L_62ae80

            lea EDX,DWORD PTR [ESP+14]
            mov EAX,OFFSET (_KingData+121984)
            mov DX,WORD PTR [EDX]
            cmp DX,WORD PTR [EAX]
            jne $L_62adb3

            cmp ECX,2
            jne $L_62adb3

            mov ECX,OFFSET (_KingBss+671252)
            mov EDX,DWORD PTR [ECX]
            mov ECX,DWORD PTR [EAX]
            cmp EDX,ECX
            jne $L_62adb3

            mov EAX,DWORD PTR [ESP+20]
            mov DWORD PTR [(_KingBss+704100)],EBP
            push EAX
            mov DWORD PTR [(_KingData+121184)],OFFSET $L_62ce90
            call FUN_62cd70

            add ESP,4
            test EAX,EAX
            je $L_62ada8

            pop ESI
            pop EBP
            or EAX,4294967295
            pop EBX
            pop ECX
            ret 
$L_62ada8::

            mov DWORD PTR [(_KingBss+919152)],ESI
            jmp $L_62b349
$L_62adb3::

            lea EDX,DWORD PTR [ESP+14]
            mov ECX,OFFSET (_KingData+121980)
            mov AX,WORD PTR [EDX]
            cmp AX,WORD PTR [ECX]
            jne $L_62addd

            mov EAX,2
            pop ESI
            pop EBP
            mov DWORD PTR [(_KingData+121184)],OFFSET $L_62c920
            mov DWORD PTR [(_KingData+120788)],EAX
            pop EBX
            pop ECX
            ret 
$L_62addd::

            lea EDX,DWORD PTR [ESP+14]
            mov ECX,OFFSET (_KingData+121976)
            mov AX,WORD PTR [EDX]
            cmp AX,WORD PTR [ECX]
            jne $L_62ae0a

            mov EAX,ESI
            mov DWORD PTR [(_KingBss+919152)],ESI
            pop ESI
            pop EBP
            mov DWORD PTR [(_KingData+121184)],OFFSET $L_62c3b0
            mov DWORD PTR [(_KingData+120788)],EAX
            pop EBX
            pop ECX
            ret 
$L_62ae0a::

            lea EDX,DWORD PTR [ESP+14]
            mov ECX,OFFSET (_KingData+121972)
            mov AX,WORD PTR [EDX]
            cmp AX,WORD PTR [ECX]
            jne $L_62ae3a

            mov DWORD PTR [(_KingBss+919152)],ESI
            mov EAX,3
            pop ESI
            pop EBP
            mov DWORD PTR [(_KingData+121184)],OFFSET $L_62bb80
            mov DWORD PTR [(_KingData+120788)],EAX
            pop EBX
            pop ECX
            ret 
$L_62ae3a::

            cmp DWORD PTR [(_KingBss+985020)],EBP
            je $L_62b357

            cmp DWORD PTR [(_KingBss+985012)],EBP
            je $L_62b357

            cmp DWORD PTR [(_KingBss+985028)],EBP
            jne $L_62b357

            mov DWORD PTR [(_KingBss+919152)],ESI
            xor EAX,EAX
            mov DWORD PTR [(_KingBss+704100)],EBP
            pop ESI
            pop EBP
            mov DWORD PTR [(_KingData+120788)],EAX
            mov DWORD PTR [(_KingData+121184)],OFFSET $L_62b7c0
            pop EBX
            pop ECX
            ret 
$L_62ae80::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62ae9e

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingData+120788)],EAX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62aeb2
$L_62ae9e::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
            mov DWORD PTR [(_KingData+120788)],EAX
$L_62aeb2::

            cmp EAX,8
            je $L_62aee4

            mov ECX,DWORD PTR [(_KingBss+919420)]
            push EAX
            push OFFSET (_KingBss+919156)
            push ECX
            push OFFSET (_KingData+121916)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,20
            mov DWORD PTR [(_KingBss+985048)],ESI
            or EAX,4294967295
            pop ESI
            pop EBP
            pop EBX
            pop ECX
            ret 
$L_62aee4::

            mov EAX,DWORD PTR [(_KingBss+704096)]
            mov DWORD PTR [(_KingData+121184)],OFFSET $L_62ce90
            cmp ECX,EAX
            jae $L_62af0a

            mov DL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov BYTE PTR [ESP+20],DL
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62af1d
$L_62af0a::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
            mov BYTE PTR [ESP+20],AL
$L_62af1d::

            mov BL,BYTE PTR [ESP+20]
            test BL,32
            je $L_62af51

            mov EAX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push EAX
            push OFFSET (_KingData+121864)
$L_62af36::

            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,16
            mov DWORD PTR [(_KingBss+985048)],ESI
            or EAX,4294967295
            pop ESI
            pop EBP
            pop EBX
            pop ECX
            ret 
FUN_62ac50 ENDP
$L_62af51 PROC

            and BL,2
            je $L_62af91

            mov ECX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push ECX
            push OFFSET (_KingData+121796)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            mov EAX,DWORD PTR [(_KingBss+985020)]
            add ESP,16
            cmp EAX,ESI
            mov DWORD PTR [(_KingBss+985048)],ESI
            jg $L_62af8b

            pop ESI
            pop EBP
            or EAX,4294967295
            pop EBX
            pop ECX
            ret 
$L_62af8b::

            mov ECX,DWORD PTR [(_KingBss+704100)]
$L_62af91::

            test BYTE PTR [ESP+20],192
            je $L_62afdd

            mov EDX,DWORD PTR [ESP+20]
            mov EAX,DWORD PTR [(_KingBss+919420)]
            and EDX,255
            push EDX
            push OFFSET (_KingBss+919156)
            push EAX
            push OFFSET (_KingData+121744)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            mov EAX,DWORD PTR [(_KingBss+985020)]
            add ESP,20
            cmp EAX,ESI
            mov DWORD PTR [(_KingBss+985048)],ESI
            jg $L_62afd7

            pop ESI
            pop EBP
            or EAX,4294967295
            pop EBX
            pop ECX
            ret 
$L_62afd7::

            mov ECX,DWORD PTR [(_KingBss+704100)]
$L_62afdd::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62aff8

            xor EDX,EDX
            mov DL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov ESI,EDX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b009
$L_62aff8::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
            mov ESI,EAX
$L_62b009::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b022

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b031
$L_62b022::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b031::

            shl EAX,8
            or ESI,EAX
            mov EAX,DWORD PTR [(_KingBss+704096)]
            cmp ECX,EAX
            jae $L_62b050

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b05f
$L_62b050::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b05f::

            shl EAX,16
            or ESI,EAX
            mov EAX,DWORD PTR [(_KingBss+704096)]
            cmp ECX,EAX
            jae $L_62b07e

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b08d
$L_62b07e::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b08d::

            shl EAX,24
            or ESI,EAX
            je $L_62b0a2

            cmp DWORD PTR [(_KingData+120780)],EBP
            jne $L_62b0a2

            mov DWORD PTR [(_KingBss+670976)],ESI
$L_62b0a2::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b0b3

            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b0c2
$L_62b0b3::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b0c2::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b0d3

            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b0e2
$L_62b0d3::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b0e2::

            test BL,BL
            je $L_62b170

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b105

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov ESI,EAX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b116
$L_62b105::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
            mov ESI,EAX
$L_62b116::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b12f

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b13e
$L_62b12f::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b13e::

            cmp DWORD PTR [(_KingBss+985032)],EBP
            je $L_62b170

            mov ECX,DWORD PTR [(_KingBss+919420)]
            shl EAX,8
            or EAX,ESI
            push EAX
            push OFFSET (_KingBss+919156)
            push ECX
            push OFFSET (_KingData+121720)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,20
$L_62b170::

            test BYTE PTR [ESP+20],4
            je $L_62b22c

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b196

            xor EDX,EDX
            mov DL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov ESI,EDX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b1a7
$L_62b196::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
            mov ESI,EAX
$L_62b1a7::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b1c0

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b1cf
$L_62b1c0::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b1cf::

            shl EAX,8
            or ESI,EAX
            mov EAX,DWORD PTR [(_KingBss+985032)]
            cmp EAX,EBP
            je $L_62b201

            mov EAX,DWORD PTR [(_KingBss+919420)]
            push ESI
            push OFFSET (_KingBss+919156)
            push EAX
            push OFFSET (_KingData+121676)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,20
$L_62b201::

            mov EDX,ESI
            dec ESI
            test EDX,EDX
            je $L_62b22c

            inc ESI
$L_62b209::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b21a

            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b229
$L_62b21a::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b229::

            dec ESI
            jne $L_62b209
$L_62b22c::

            mov AL,BYTE PTR [ESP+20]
            push EDI
            test AL,8
            je $L_62b2d6

            cmp DWORD PTR [(_KingData+120776)],EBP
            jne $L_62b2f8

            cmp DWORD PTR [(_KingBss+985012)],EBP
            je $L_62b259

            cmp DWORD PTR [(_KingBss+985028)],EBP
            je $L_62b2f8
$L_62b259::

            cmp DWORD PTR [(_KingBss+704092)],1
            jg $L_62b2f8

            push OFFSET (_KingBss+670984)
            call FUN_62ba90

            mov ESI,EAX
            add ESP,4
            mov EDI,ESI
$L_62b277::

            mov ECX,DWORD PTR [(_KingBss+704100)]
            mov EAX,DWORD PTR [(_KingBss+704096)]
            cmp ECX,EAX
            jae $L_62b297

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b2a0
$L_62b297::

            push EBP
            call FUN_62b8e0

            add ESP,4
$L_62b2a0::

            mov BYTE PTR [ESI],AL
            inc ESI
            test AL,AL
            je $L_62b2be

            cmp ESI,OFFSET (_KingBss+671244)
            jb $L_62b277

            push OFFSET (_KingData+121636)
            call FUN_62bad0
$L_62af51 ENDP
$L_62b2b9 PROC

            add ESP,4
            jmp $L_62b277
$L_62b2be::

            cmp DWORD PTR [(_KingBss+985028)],EBP
            jne $L_62b2d0

            cmp EDI,EBP
            je $L_62b2d0

            mov DWORD PTR [(_KingBss+985028)],EBP
$L_62b2d0::

            mov ECX,DWORD PTR [(_KingBss+704100)]
$L_62b2d6::

            mov AL,BYTE PTR [ESP+24]
            pop EDI
            test AL,16
            je $L_62b337
$L_62b2df::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b324

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b333
$L_62b2b9 ENDP
$L_62b2f8 PROC

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62b30f

            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62b31e
$L_62b30f::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b31e::

            test AL,AL
            jne $L_62b2f8

            jmp $L_62b2d6
$L_62b324::

            push EBP
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62b333::

            cmp EAX,EBP
            jne $L_62b2df
$L_62b337::

            cmp DWORD PTR [(_KingBss+704092)],1
            jne $L_62b349

            add ECX,8
            mov DWORD PTR [(_KingBss+985080)],ECX
$L_62b349::

            mov EAX,DWORD PTR [(_KingData+120788)]
            cmp EAX,EBP
            jge $L_62b3b1

            mov ESI,1
$L_62b357::

            cmp DWORD PTR [(_KingBss+704092)],ESI
            jne $L_62b374

            mov EAX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push EAX
            push OFFSET (_KingData+121604)
            jmp $L_62af36
$L_62b374::

            cmp DWORD PTR [(_KingBss+985036)],EBP
            jne $L_62b39a

            mov ECX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push ECX
            push OFFSET (_KingData+121548)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,16
$L_62b39a::

            cmp DWORD PTR [(_KingBss+985048)],EBP
            jne $L_62b3ac

            mov DWORD PTR [(_KingBss+985048)],2
$L_62b3ac::

            mov EAX,4294967294
$L_62b3b1::

            pop ESI
            pop EBP
            pop EBX
            pop ECX
            ret 
$L_62b2f8 ENDP
ALIGN 16
FUN_62b3c0 PROC

            mov EAX,DWORD PTR [(_KingBss+985072)]
            test EAX,EAX
            je $L_62b3d3

            mov EAX,DWORD PTR [ESP+4]
            push EAX
            call _exit
$L_62b3d3::

            mov EAX,DWORD PTR [(_KingBss+671244)]
            mov DWORD PTR [(_KingBss+985072)],1
            test EAX,EAX
            je $L_62b3f9

            push EAX
            call _free

            add ESP,4
            mov DWORD PTR [(_KingBss+671244)],0
$L_62b3f9::

            mov EAX,DWORD PTR [(_KingBss+985052)]
            test EAX,EAX
            je $L_62b415

            push EAX
            call _free

            add ESP,4
            mov DWORD PTR [(_KingBss+985052)],0
$L_62b415::

            mov ECX,DWORD PTR [ESP+4]
            push ECX
            call _exit

            nop
FUN_62b3c0 ENDP
$L_62b420 PROC

            mov EAX,DWORD PTR [(_KingBss+985064)]
            test EAX,EAX
            je $L_62b440

            push OFFSET (_KingBss+704084)
            call FUN_62a950

            push OFFSET (_KingBss+670984)
            call _remove

            add ESP,8
$L_62b440::

            push 1
            call FUN_62b3c0
$L_62b420 ENDP
$L_62b447 PROC

            pop ECX
            ret 
$L_62b447 ENDP
ALIGN 16
FUN_62b450 PROC

            sub ESP,8
            mov ECX,DWORD PTR [ESP+12]
            mov DL,BYTE PTR [(_KingData+121996)]
            mov DWORD PTR [(_KingBss+671248)],ECX
            mov CL,BYTE PTR [(_KingData+121997)]
            mov BYTE PTR [(_KingBss+704109)],CL
            mov ECX,DWORD PTR [(_KingBss+670980)]
            push ESI
            mov ESI,DWORD PTR [ESP+20]
            xor AL,AL
            mov DWORD PTR [ESP+8],0
            test ECX,ECX
            mov DWORD PTR [ESP+4],0
            mov DWORD PTR [(_KingBss+704084)],ESI
            mov DWORD PTR [(_KingData+120788)],8
            mov BYTE PTR [(_KingBss+704108)],DL
            mov BYTE PTR [(_KingBss+704110)],8
            je $L_62b4af

            mov AL,8
$L_62b4af::

            mov BYTE PTR [(_KingBss+704111)],AL
            mov EAX,DWORD PTR [(_KingBss+670976)]
            xor EDX,EDX
            mov BYTE PTR [(_KingBss+704112)],AL
            mov DL,AH
            xor ECX,ECX
            shr EAX,16
            mov CL,AH
            push 0
            push 0
            mov BYTE PTR [(_KingBss+704113)],DL
            mov BYTE PTR [(_KingBss+704114)],AL
            mov BYTE PTR [(_KingBss+704115)],CL
            mov DWORD PTR [(_KingBss+704104)],8
            call FUN_62b870

            push ESI
            mov DWORD PTR [(_KingBss+985076)],EAX
            call _Gz_BiInit

            lea EDX,DWORD PTR [ESP+20]
            push OFFSET (_KingData+120788)
            push EDX
            call _Gz_CtInit

            mov ECX,DWORD PTR [(_KingData+120792)]
            lea EAX,DWORD PTR [ESP+24]
            push EAX
            push ECX
            call FUN_62d2e0

            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov DL,BYTE PTR [ESP+32]
            add ESP,28
            mov BYTE PTR [EAX+(_KingBss+704108)],DL
            inc EAX
            cmp EAX,16384
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b541

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
$L_62b541::

            mov BYTE PTR [EAX+(_KingBss+704108)],11
            inc EAX
            cmp EAX,16384
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b55f

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
$L_62b55f::

            mov ECX,DWORD PTR [(_KingBss+670980)]
            test ECX,ECX
            je $L_62b5a3

            push OFFSET (_KingBss+919156)
            call FUN_62ba90

            mov ESI,EAX
            mov EAX,DWORD PTR [(_KingBss+704104)]
            add ESP,4
$L_62b57d::

            mov CL,BYTE PTR [ESI]
            mov BYTE PTR [EAX+(_KingBss+704108)],CL
            inc EAX
            cmp EAX,16384
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b59c

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
$L_62b59c::

            mov DL,BYTE PTR [ESI]
            inc ESI
            test DL,DL
            jne $L_62b57d
$L_62b5a3::

            mov DWORD PTR [(_KingBss+985080)],EAX
            call FUN_62d650

            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov ECX,DWORD PTR [(_KingBss+985076)]
            cmp EAX,16382
            pop ESI
            mov BYTE PTR [EAX+(_KingBss+704108)],CL
            inc EAX
            jae $L_62b5d4

            xor EDX,EDX
            inc EAX
            mov DL,CH
            mov BYTE PTR [EAX+(_KingBss+704107)],DL
            jmp $L_62b617
$L_62b5d4::

            cmp EAX,16384
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b5f0

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov ECX,DWORD PTR [(_KingBss+985076)]
$L_62b5f0::

            xor EDX,EDX
            inc EAX
            mov DL,CH
            cmp EAX,16384
            mov BYTE PTR [EAX+(_KingBss+704107)],DL
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b617

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov ECX,DWORD PTR [(_KingBss+985076)]
$L_62b617::

            cmp EAX,16382
            jae $L_62b635

            shr ECX,16
            mov BYTE PTR [EAX+(_KingBss+704108)],CL
            xor EDX,EDX
            inc EAX
            mov DL,CH
            mov BYTE PTR [EAX+(_KingBss+704108)],DL
            inc EAX
            jmp $L_62b681
$L_62b635::

            mov EDX,ECX
            shr EDX,16
            mov BYTE PTR [EAX+(_KingBss+704108)],DL
            inc EAX
            cmp EAX,16384
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b65d

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov ECX,DWORD PTR [(_KingBss+985076)]
$L_62b65d::

            shr ECX,16
            xor EDX,EDX
            inc EAX
            mov DL,CH
            cmp EAX,16384
            mov BYTE PTR [EAX+(_KingBss+704107)],DL
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b681

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
$L_62b681::

            mov ECX,DWORD PTR [(_KingBss+919416)]
            cmp EAX,16382
            mov BYTE PTR [EAX+(_KingBss+704108)],CL
            inc EAX
            jae $L_62b6a2

            xor EDX,EDX
            inc EAX
            mov DL,CH
            mov BYTE PTR [EAX+(_KingBss+704107)],DL
            jmp $L_62b6e5
$L_62b6a2::

            cmp EAX,16384
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b6be

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov ECX,DWORD PTR [(_KingBss+919416)]
$L_62b6be::

            xor EDX,EDX
            inc EAX
            mov DL,CH
            cmp EAX,16384
            mov BYTE PTR [EAX+(_KingBss+704107)],DL
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b6e5

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov ECX,DWORD PTR [(_KingBss+919416)]
$L_62b6e5::

            cmp EAX,16382
            jae $L_62b708

            shr ECX,16
            mov BYTE PTR [EAX+(_KingBss+704108)],CL
            xor EDX,EDX
            inc EAX
            mov DL,CH
            mov BYTE PTR [EAX+(_KingBss+704108)],DL
            inc EAX
            mov DWORD PTR [(_KingBss+704104)],EAX
            jmp $L_62b74f
$L_62b708::

            mov EDX,ECX
            shr EDX,16
            mov BYTE PTR [EAX+(_KingBss+704108)],DL
            inc EAX
            cmp EAX,16384
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b730

            call FUN_62b9a0

            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov ECX,DWORD PTR [(_KingBss+919416)]
$L_62b730::

            shr ECX,16
            xor EDX,EDX
            inc EAX
            mov DL,CH
            cmp EAX,16384
            mov BYTE PTR [EAX+(_KingBss+704107)],DL
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62b74f

            call FUN_62b9a0
$L_62b74f::

            add DWORD PTR [(_KingBss+985080)],8
            call FUN_62b9a0

            xor EAX,EAX
            add ESP,8
            ret 
FUN_62b450 ENDP
$L_62b770 PROC

            mov EAX,DWORD PTR [ESP+8]
            mov ECX,DWORD PTR [(_KingBss+671248)]
            push ESI
            push EDI
            mov EDI,DWORD PTR [ESP+12]
            push EAX
            push EDI
            push ECX
            call FUN_62a910

            mov ESI,EAX
            add ESP,12
            cmp ESI,-1
            je $L_62b7b1

            test ESI,ESI
            je $L_62b7b1

            push ESI
            push EDI
            call FUN_62b870

            mov DWORD PTR [(_KingBss+985076)],EAX
            mov EAX,DWORD PTR [(_KingBss+919416)]
            add ESP,8
            add EAX,ESI
            mov DWORD PTR [(_KingBss+919416)],EAX
$L_62b7b1::

            mov EAX,ESI
            pop EDI
            pop ESI
            ret 
$L_62b770 ENDP
$L_62b7c0 PROC

            push ESI
            push EDI
            call __errno

            mov DWORD PTR [EAX],0
            mov EAX,DWORD PTR [(_KingBss+704096)]
            test EAX,EAX
            je $L_62b81e

            mov ESI,DWORD PTR [ESP+16]
            mov EDI,DWORD PTR [ESP+12]
$L_62b7de::

            cmp EAX,-1
            je $L_62b823

            push EAX
            push OFFSET (_KingBss+671252)
            push ESI
            call FUN_62ba40

            mov EAX,DWORD PTR [(_KingBss+704096)]
            mov ECX,DWORD PTR [(_KingBss+722540)]
            push 32768
            add ECX,EAX
            push OFFSET (_KingBss+671252)
            push EDI
            mov DWORD PTR [(_KingBss+722540)],ECX
            call FUN_62a910

            add ESP,24
            mov DWORD PTR [(_KingBss+704096)],EAX
            test EAX,EAX
            jne $L_62b7de
$L_62b81e::

            cmp EAX,-1
            jne $L_62b843
$L_62b823::

            call __errno

            cmp DWORD PTR [EAX],0
            je $L_62b854

            call FUN_62bb00

            mov ECX,DWORD PTR [(_KingBss+722540)]
            pop EDI
            mov DWORD PTR [(_KingBss+919416)],ECX
            xor EAX,EAX
            pop ESI
            ret 
$L_62b843::

            mov EDX,DWORD PTR [(_KingBss+722540)]
            pop EDI
            mov DWORD PTR [(_KingBss+919416)],EDX
            xor EAX,EAX
            pop ESI
            ret 
$L_62b854::

            mov EAX,DWORD PTR [(_KingBss+722540)]
            pop EDI
            mov DWORD PTR [(_KingBss+919416)],EAX
            xor EAX,EAX
            pop ESI
            ret 
$L_62b7c0 ENDP
ALIGN 16
FUN_62b870 PROC

            mov ECX,DWORD PTR [ESP+4]
            push ESI
            test ECX,ECX
            jne $L_62b885

            or EAX,4294967295
            pop ESI
            mov DWORD PTR [(_KingData+122000)],EAX
            not EAX
            ret 
$L_62b885::

            mov ESI,DWORD PTR [ESP+12]
            mov EAX,DWORD PTR [(_KingData+122000)]
            test ESI,ESI
            je $L_62b8b2

            push EBX
$L_62b893::

            mov EDX,EAX
            xor EBX,EBX
            mov BL,BYTE PTR [ECX]
            and EDX,255
            xor EDX,EBX
            shr EAX,8
            mov EDX,DWORD PTR [EDX*4+(_KingData+122004)]
            xor EAX,EDX
            inc ECX
            dec ESI
            jne $L_62b893

            pop EBX
$L_62b8b2::

            mov DWORD PTR [(_KingData+122000)],EAX
            pop ESI
            not EAX
            ret 
FUN_62b870 ENDP
ALIGN 16
FUN_62b8c0 PROC

            xor EAX,EAX
            mov DWORD PTR [(_KingBss+704104)],EAX
            mov DWORD PTR [(_KingBss+704100)],EAX
            mov DWORD PTR [(_KingBss+704096)],EAX
            mov DWORD PTR [(_KingBss+722540)],EAX
            mov DWORD PTR [(_KingBss+919416)],EAX
            ret 
FUN_62b8c0 ENDP
ALIGN 16
FUN_62b8e0 PROC

            mov DWORD PTR [(_KingBss+704096)],0
            call __errno

            mov DWORD PTR [EAX],0
            mov EAX,DWORD PTR [(_KingBss+704096)]
            mov ECX,32768
            sub ECX,EAX
            lea EDX,DWORD PTR [EAX+(_KingBss+671252)]
            mov EAX,DWORD PTR [(_KingBss+671248)]
            push ECX
            push EDX
            push EAX
            call FUN_62a910

            add ESP,12
            test EAX,EAX
            je $L_62b958
$L_62b91b::

            cmp EAX,-1
            je $L_62b958

            mov ECX,DWORD PTR [(_KingBss+704096)]
            add ECX,EAX
            cmp ECX,32768
            mov DWORD PTR [(_KingBss+704096)],ECX
            jae $L_62b95e

            mov EDX,32768
            lea EAX,DWORD PTR [ECX+(_KingBss+671252)]
            sub EDX,ECX
            mov ECX,DWORD PTR [(_KingBss+671248)]
            push EDX
            push EAX
            push ECX
            call FUN_62a910

            add ESP,12
            test EAX,EAX
            jne $L_62b91b
$L_62b958::

            mov ECX,DWORD PTR [(_KingBss+704096)]
$L_62b95e::

            test ECX,ECX
            jne $L_62b979

            mov EAX,DWORD PTR [ESP+4]
            test EAX,EAX
            je $L_62b96e

            or EAX,4294967295
            ret 
$L_62b96e::

            call FUN_62bb00
FUN_62b8e0 ENDP
$L_62b973 PROC

            mov ECX,DWORD PTR [(_KingBss+704096)]
$L_62b979::

            mov EAX,DWORD PTR [(_KingBss+919416)]
            mov DWORD PTR [(_KingBss+704100)],1
            add EAX,ECX
            mov DWORD PTR [(_KingBss+919416)],EAX
            xor EAX,EAX
            mov AL,BYTE PTR [(_KingBss+671252)]
            ret 
$L_62b973 ENDP
ALIGN 16
FUN_62b9a0 PROC

            mov EAX,DWORD PTR [(_KingBss+704104)]
            test EAX,EAX
            je $L_62b9d9

            push EAX
            mov EAX,DWORD PTR [(_KingBss+704084)]
            push OFFSET (_KingBss+704108)
            push EAX
            call FUN_62ba40

            mov ECX,DWORD PTR [(_KingBss+704104)]
            mov EAX,DWORD PTR [(_KingBss+722540)]
            add ESP,12
            add EAX,ECX
            mov DWORD PTR [(_KingBss+722540)],EAX
            mov DWORD PTR [(_KingBss+704104)],0
$L_62b9d9::

            ret 
FUN_62b9a0 ENDP
ALIGN 16
FUN_62b9e0 PROC

            mov EAX,DWORD PTR [(_KingBss+704104)]
            test EAX,EAX
            je $L_62ba36

            push EAX
            push OFFSET (_KingBss+853616)
            call FUN_62b870

            mov EAX,DWORD PTR [(_KingBss+985044)]
            add ESP,8
            test EAX,EAX
            jne $L_62ba1a

            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov ECX,DWORD PTR [(_KingBss+704084)]
            push EAX
            push OFFSET (_KingBss+853616)
            push ECX
            call FUN_62ba40

            add ESP,12
$L_62ba1a::

            mov EDX,DWORD PTR [(_KingBss+704104)]
            mov EAX,DWORD PTR [(_KingBss+722540)]
            add EAX,EDX
            mov DWORD PTR [(_KingBss+704104)],0
            mov DWORD PTR [(_KingBss+722540)],EAX
$L_62ba36::

            ret 
FUN_62b9e0 ENDP
ALIGN 16
FUN_62ba40 PROC

            push EBX
            mov EBX,DWORD PTR [ESP+12]
            push EBP
            mov EBP,DWORD PTR [ESP+12]
            push ESI
            push EDI
            mov EDI,DWORD PTR [ESP+28]
            push EDI
            push EBX
            push EBP
            call FUN_62a930

            mov ESI,EAX
            add ESP,12
            cmp ESI,EDI
            je $L_62ba80
$L_62ba61::

            cmp ESI,-1
            jne $L_62ba6b

            call FUN_62bb50
$L_62ba6b::

            sub EDI,ESI
            add EBX,ESI
            push EDI
            push EBX
            push EBP
            call FUN_62a930

            mov ESI,EAX
            add ESP,12
            cmp ESI,EDI
            jne $L_62ba61
$L_62ba80::

            pop EDI
            pop ESI
            pop EBP
            pop EBX
            ret 
FUN_62ba40 ENDP
ALIGN 16
FUN_62ba90 PROC

            push ESI
            mov ESI,DWORD PTR [ESP+8]
            push 47
            push ESI
            call _strrchr

            add ESP,8
            test EAX,EAX
            je $L_62baa7

            lea ESI,DWORD PTR [EAX+1]
$L_62baa7::

            push 92
            push ESI
            call _strrchr

            add ESP,8
            test EAX,EAX
            je $L_62bab9

            lea ESI,DWORD PTR [EAX+1]
$L_62bab9::

            push 58
            push ESI
            call _strrchr

            add ESP,8
            test EAX,EAX
            je $L_62bacb

            inc EAX
            pop ESI
            ret 
$L_62bacb::

            mov EAX,ESI
            pop ESI
            ret 
FUN_62ba90 ENDP
ALIGN 16
FUN_62bad0 PROC

            mov EAX,DWORD PTR [ESP+4]
            mov ECX,DWORD PTR [(_KingBss+919420)]
            push EAX
            push OFFSET (_KingBss+919156)
            push ECX
            push OFFSET (_KingData+123044)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,20
            jmp $L_62b420
FUN_62bad0 ENDP
ALIGN 16
FUN_62bb00 PROC

            mov EAX,DWORD PTR [(_KingBss+919420)]
            push EAX
            push OFFSET (_KingData+123088)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,12
            call __errno

            mov ECX,DWORD PTR [EAX]
            push OFFSET (_KingBss+919156)
            test ECX,ECX
            je $L_62bb32

            call _perror

            add ESP,4
            jmp $L_62bb44
$L_62bb32::

            push OFFSET (_KingData+123060)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,12
$L_62bb44::

            jmp $L_62b420
FUN_62bb00 ENDP
ALIGN 16
FUN_62bb50 PROC

            mov EAX,DWORD PTR [(_KingBss+919420)]
            push EAX
            push OFFSET (_KingData+123088)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            push OFFSET (_KingBss+670984)
            call _perror

            add ESP,16
            jmp $L_62b420
FUN_62bb50 ENDP
$L_62bb80 PROC

            mov EAX,DWORD PTR [ESP+4]
            push ESI
            mov ESI,DWORD PTR [ESP+12]
            mov DWORD PTR [(_KingBss+671248)],EAX
            mov DWORD PTR [(_KingBss+704084)],ESI
            call FUN_62bbe0

            mov EAX,DWORD PTR [(_KingBss+985672)]
            test EAX,EAX
            jne $L_62bbda
$L_62bba2::

            push OFFSET (_KingBss+853616)
            push 8192
            call FUN_62bcd0

            mov ECX,DWORD PTR [(_KingBss+985044)]
            add ESP,8
            test ECX,ECX
            jne $L_62bbd1

            test EAX,EAX
            jbe $L_62bbd1

            push EAX
            push OFFSET (_KingBss+853616)
            push ESI
            call FUN_62ba40

            add ESP,12
$L_62bbd1::

            mov EAX,DWORD PTR [(_KingBss+985672)]
            test EAX,EAX
            je $L_62bba2
$L_62bbda::

            xor EAX,EAX
            pop ESI
            ret 
$L_62bb80 ENDP
ALIGN 16
FUN_62bbe0 PROC

            call FUN_62bc00

            xor EAX,EAX
            mov DWORD PTR [(_KingBss+985116)],EAX
            mov DWORD PTR [(_KingBss+985672)],EAX
            ret 
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
            nop
            nop
            nop
            nop
FUN_62bbe0 ENDP
ALIGN 16
FUN_62bc00 PROC

            call FUN_62bc10

            mov DWORD PTR [(_KingBss+985120)],0
            ret 
FUN_62bc00 ENDP
ALIGN 16
FUN_62bc10 PROC

            xor EAX,EAX
            push 16
            mov WORD PTR [(_KingBss+985152)],AX
            mov DWORD PTR [(_KingBss+985148)],EAX
            mov DWORD PTR [(_KingBss+985668)],EAX
            call FUN_62bc30

            pop ECX
            ret 
FUN_62bc10 ENDP
ALIGN 16
FUN_62bc30 PROC

            push ESI
            mov ESI,DWORD PTR [ESP+8]
            mov EAX,DWORD PTR [(_KingBss+985148)]
            mov CX,SI
            shl WORD PTR [(_KingBss+985152)],CL
            mov ECX,DWORD PTR [(_KingBss+985668)]
            cmp ESI,ECX
            jle $L_62bcb0

            mov EDX,DWORD PTR [(_KingBss+704100)]
$L_62bc54::

            sub ESI,ECX
            mov ECX,ESI
            shl EAX,CL
            or WORD PTR [(_KingBss+985152)],AX
            mov EAX,DWORD PTR [(_KingBss+704096)]
            cmp EDX,EAX
            jae $L_62bc80

            xor EAX,EAX
            mov AL,BYTE PTR [EDX+(_KingBss+671252)]
            inc EDX
            mov DWORD PTR [(_KingBss+985148)],EAX
            mov DWORD PTR [(_KingBss+704100)],EDX
            jmp $L_62bc95
$L_62bc80::

            push 1
            call FUN_62b8e0

            mov EDX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
            mov DWORD PTR [(_KingBss+985148)],EAX
$L_62bc95::

            cmp EAX,-1
            jne $L_62bca1

            xor EAX,EAX
            mov DWORD PTR [(_KingBss+985148)],EAX
$L_62bca1::

            mov ECX,8
            cmp ESI,ECX
            mov DWORD PTR [(_KingBss+985668)],ECX
            jg $L_62bc54
$L_62bcb0::

            sub ECX,ESI
            pop ESI
            shr EAX,CL
            mov DWORD PTR [(_KingBss+985668)],ECX
            or WORD PTR [(_KingBss+985152)],AX
            ret 
FUN_62bc30 ENDP
ALIGN 16
FUN_62bcd0 PROC

            mov EAX,DWORD PTR [(_KingBss+985116)]
            push EBX
            mov EBX,DWORD PTR [ESP+8]
            push ESI
            xor ESI,ESI
            dec EAX
            push EDI
            mov EDI,DWORD PTR [ESP+20]
            mov DWORD PTR [(_KingBss+985116)],EAX
            js $L_62bd20

            mov EAX,DWORD PTR [(_KingBss+985144)]
$L_62bcef::

            mov AL,BYTE PTR [EAX+EDI*1]
            mov BYTE PTR [ESI+EDI*1],AL
            mov ECX,DWORD PTR [(_KingBss+985144)]
            lea EAX,DWORD PTR [ECX+1]
            and EAX,8191
            inc ESI
            cmp ESI,EBX
            mov DWORD PTR [(_KingBss+985144)],EAX
            je $L_62bdaf

            mov ECX,DWORD PTR [(_KingBss+985116)]
            dec ECX
            mov DWORD PTR [(_KingBss+985116)],ECX
            jns $L_62bcef
$L_62bd20::

            call FUN_62bdc0

            cmp EAX,510
            je $L_62bda5
$L_62bd2c::

            cmp EAX,255
            ja $L_62bd3d

            mov BYTE PTR [ESI+EDI*1],AL
            inc ESI
            cmp ESI,EBX
            je $L_62bdaf

            jmp $L_62bd99
$L_62bd3d::

            add EAX,4294967043
            mov DWORD PTR [(_KingBss+985116)],EAX
            call FUN_62c330

            mov ECX,DWORD PTR [(_KingBss+985116)]
            mov EDX,EAX
            mov EAX,ESI
            sub EAX,EDX
            dec EAX
            and EAX,8191
            dec ECX
            mov DWORD PTR [(_KingBss+985144)],EAX
            mov DWORD PTR [(_KingBss+985116)],ECX
            js $L_62bd99
$L_62bd6c::

            mov AL,BYTE PTR [EAX+EDI*1]
            mov BYTE PTR [ESI+EDI*1],AL
            mov ECX,DWORD PTR [(_KingBss+985144)]
            lea EAX,DWORD PTR [ECX+1]
            and EAX,8191
            inc ESI
            cmp ESI,EBX
            mov DWORD PTR [(_KingBss+985144)],EAX
            je $L_62bdaf

            mov ECX,DWORD PTR [(_KingBss+985116)]
            dec ECX
            mov DWORD PTR [(_KingBss+985116)],ECX
            jns $L_62bd6c
$L_62bd99::

            call FUN_62bdc0

            cmp EAX,510
            jne $L_62bd2c
$L_62bda5::

            mov DWORD PTR [(_KingBss+985672)],1
$L_62bdaf::

            mov EAX,ESI
            pop EDI
            pop ESI
            pop EBX
            ret 
FUN_62bcd0 ENDP
ALIGN 16
FUN_62bdc0 PROC

            mov EAX,DWORD PTR [(_KingBss+985120)]
            test EAX,EAX
            jne $L_62be00

            push 16
            call FUN_62be70

            add ESP,4
            mov DWORD PTR [(_KingBss+985120)],EAX
            test EAX,EAX
            jne $L_62bde2

            mov EAX,510
            ret 
$L_62bde2::

            push 3
            push 5
            push 19
            call FUN_62bea0

            call FUN_62c1d0

            push -1
            push 4
            push 14
            call FUN_62bea0

            add ESP,24
$L_62be00::

            mov EAX,DWORD PTR [(_KingBss+985120)]
            push ESI
            dec EAX
            xor ESI,ESI
            mov DWORD PTR [(_KingBss+985120)],EAX
            mov EAX,DWORD PTR [(_KingBss+985152)]
            and EAX,65535
            mov ECX,EAX
            shr ECX,4
            mov SI,WORD PTR [ECX*2+(_KingBss+919472)]
            cmp ESI,510
            jb $L_62be58

            mov ECX,8
$L_62be32::

            test ECX,EAX
            je $L_62be42

            xor EDX,EDX
            mov DX,WORD PTR [ESI*2+(_KingBss+788080)]
            jmp $L_62be4c
$L_62be42::

            xor EDX,EDX
            mov DX,WORD PTR [ESI*2+(_KingBss+722544)]
$L_62be4c::

            mov ESI,EDX
            shr ECX,1
            cmp ESI,510
            jae $L_62be32
$L_62be58::

            xor EAX,EAX
            mov AL,BYTE PTR [ESI+(_KingBss+704108)]
            push EAX
            call FUN_62bc30

            add ESP,4
            mov EAX,ESI
            pop ESI
            ret 
FUN_62bdc0 ENDP
ALIGN 16
FUN_62be70 PROC

            mov EAX,DWORD PTR [ESP+4]
            push ESI
            mov ESI,DWORD PTR [(_KingBss+985152)]
            mov CL,16
            sub CL,AL
            and ESI,65535
            push EAX
            shr ESI,CL
            call FUN_62bc30

            add ESP,4
            mov EAX,ESI
            pop ESI
            ret 
FUN_62be70 ENDP
ALIGN 16
FUN_62bea0 PROC

            push EBP
            push ESI
            mov ESI,DWORD PTR [ESP+16]
            push EDI
            push ESI
            call FUN_62be70

            mov EBP,EAX
            add ESP,4
            test EBP,EBP
            jne $L_62befb

            push ESI
            call FUN_62be70

            mov ECX,DWORD PTR [ESP+20]
            add ESP,4
            test ECX,ECX
            mov EDX,EAX
            jle $L_62bede

            mov ESI,ECX
            xor EAX,EAX
            mov EDI,OFFSET (_KingBss+985124)
            shr ECX,2

            rep stosd DWORD PTR ES:[EDI]

            mov ECX,ESI
            and ECX,3

            rep stosb BYTE PTR ES:[EDI]
$L_62bede::

            mov EAX,EDX
            mov ECX,128
            mov DX,AX
            mov EDI,OFFSET (_KingBss+985156)
            shl EDX,16
            mov DX,AX
            mov EAX,EDX

            rep stosd DWORD PTR ES:[EDI]

            pop EDI
            pop ESI
            pop EBP
            ret 
$L_62befb::

            xor ESI,ESI
            test EBP,EBP
            jle $L_62bf7b

            push EBX
$L_62bf02::

            mov EAX,DWORD PTR [(_KingBss+985152)]
            and EAX,65535
            mov EBX,EAX
            shr EBX,13
            cmp EBX,7
            jne $L_62bf2a

            test AH,16
            mov ECX,4096
            je $L_62bf31
$L_62bf20::

            shr ECX,1
            inc EBX
            test ECX,EAX
            jne $L_62bf20

            cmp EBX,7
$L_62bf2a::

            mov EAX,3
            jl $L_62bf34
$L_62bf31::

            lea EAX,DWORD PTR [EBX-3]
$L_62bf34::

            push EAX
            call FUN_62bc30

            mov EAX,DWORD PTR [ESP+32]
            add ESP,4
            mov BYTE PTR [ESI+(_KingBss+985124)],BL
            inc ESI
            cmp ESI,EAX
            jne $L_62bf76

            push 2
            call FUN_62be70

            add ESP,4
            dec EAX
            js $L_62bf76

            lea EDX,DWORD PTR [EAX+1]
            lea EDI,DWORD PTR [ESI+(_KingBss+985124)]
            mov ECX,EDX
            xor EAX,EAX
            mov EBX,ECX
            shr ECX,2

            rep stosd DWORD PTR ES:[EDI]

            mov ECX,EBX
            and ECX,3
            add ESI,EDX

            rep stosb BYTE PTR ES:[EDI]
$L_62bf76::

            cmp ESI,EBP
            jl $L_62bf02

            pop EBX
$L_62bf7b::

            mov EDX,DWORD PTR [ESP+16]
            cmp ESI,EDX
            jge $L_62bf9d

            mov ECX,EDX
            lea EDI,DWORD PTR [ESI+(_KingBss+985124)]
            sub ECX,ESI
            xor EAX,EAX
            mov ESI,ECX
            shr ECX,2

            rep stosd DWORD PTR ES:[EDI]

            mov ECX,ESI
            and ECX,3

            rep stosb BYTE PTR ES:[EDI]
$L_62bf9d::

            push OFFSET (_KingBss+985156)
            push 8
            push OFFSET (_KingBss+985124)
            push EDX
            call FUN_62bfc0

            add ESP,16
            pop EDI
            pop ESI
            pop EBP
            ret 
FUN_62bea0 ENDP
ALIGN 16
FUN_62bfc0 PROC

            sub ESP,88
            mov ECX,8
            xor EAX,EAX
            push EBX
            push EBP
            push ESI
            mov ESI,DWORD PTR [ESP+104]
            push EDI
            lea EDI,DWORD PTR [ESP+34]
            test ESI,ESI

            rep stosd DWORD PTR ES:[EDI]

            jbe $L_62bff3
$L_62bfdc::

            mov EDX,DWORD PTR [ESP+112]
            xor ECX,ECX
            mov CL,BYTE PTR [EAX+EDX*1]
            inc WORD PTR [ESP+ECX*2+32]
            lea ECX,DWORD PTR [ESP+ECX*2+32]
            inc EAX
            cmp EAX,ESI
            jb $L_62bfdc
$L_62bff3::

            mov WORD PTR [ESP+70],0
            mov ECX,15
            xor EAX,EAX
$L_62c001::

            mov DX,WORD PTR [ESP+EAX*1+34]
            add EAX,2
            shl DX,CL
            add DX,WORD PTR [ESP+EAX*1+68]
            dec ECX
            cmp EAX,30
            mov WORD PTR [ESP+EAX*1+70],DX
            jbe $L_62c001

            cmp WORD PTR [ESP+102],0
            je $L_62c031

            push OFFSET (_KingData+123100)
            call FUN_62bad0
FUN_62bfc0 ENDP
$L_62c02e PROC

            add ESP,4
$L_62c031::

            mov EBP,DWORD PTR [ESP+116]
            mov EBX,16
            mov EAX,1
            sub EBX,EBP
            cmp EBP,EAX
            mov DWORD PTR [ESP+16],EBX
            jb $L_62c070

            xor EDX,EDX
$L_62c04b::

            mov CX,BX
            mov EDI,1
            shr WORD PTR [ESP+EDX*1+70],CL
            mov ECX,EBP
            add EDX,2
            sub ECX,EAX
            shl EDI,CL
            inc EAX
            cmp EAX,EBP
            mov WORD PTR [ESP+EDX*1+32],DI
            jbe $L_62c04b

            cmp EAX,16
            ja $L_62c08f
$L_62c070::

            lea EDX,DWORD PTR [ESP+EAX*2+32]
$L_62c074::

            mov ECX,16
            mov EDI,1
            sub ECX,EAX
            add EDX,2
            shl EDI,CL
            inc EAX
            cmp EAX,16
            mov WORD PTR [EDX-2],DI
            jbe $L_62c074
$L_62c08f::

            xor EAX,EAX
            mov CL,BL
            mov AX,WORD PTR [ESP+EBP*2+70]
            shr EAX,CL
            test EAX,EAX
            je $L_62c0c1

            mov EDX,1
            mov ECX,EBP
            shl EDX,CL
            cmp EAX,EDX
            je $L_62c0c1

            mov ECX,DWORD PTR [ESP+120]
            lea EDI,DWORD PTR [ECX+EAX*2]
            mov ECX,EDX
            sub ECX,EAX
            xor EAX,EAX
            shr ECX,1

            rep stosd DWORD PTR ES:[EDI]

            adc ECX,ECX

            rep stosw WORD PTR ES:[EDI]
$L_62c0c1::

            mov ECX,15
            mov EAX,1
            sub ECX,EBP
            mov EBX,ESI
            shl EAX,CL
            xor ECX,ECX
            test ESI,ESI
            mov DWORD PTR [ESP+116],ECX
            mov DWORD PTR [ESP+20],EAX
            jbe $L_62c1c1
$L_62c0e3::

            mov EDX,DWORD PTR [ESP+112]
            xor EAX,EAX
            mov AL,BYTE PTR [ECX+EDX*1]
            test EAX,EAX
            je $L_62c1b4

            lea ECX,DWORD PTR [ESP+EAX*2+68]
            xor EDX,EDX
            xor ESI,ESI
            mov DWORD PTR [ESP+28],ECX
            mov DX,WORD PTR [ECX]
            mov SI,WORD PTR [ESP+EAX*2+32]
            add ESI,EDX
            cmp EAX,EBP
            mov DWORD PTR [ESP+24],ESI
            ja $L_62c13f

            cmp EDX,ESI
            jae $L_62c1a5

            mov EAX,DWORD PTR [ESP+120]
            mov ECX,ESI
            sub ECX,EDX
            lea EDI,DWORD PTR [EAX+EDX*2]
            mov EAX,DWORD PTR [ESP+116]
            mov DX,AX
            shl EDX,16
            mov DX,AX
            mov EAX,EDX
            shr ECX,1

            rep stosd DWORD PTR ES:[EDI]

            adc ECX,ECX

            rep stosw WORD PTR ES:[EDI]

            jmp $L_62c1a5
$L_62c13f::

            mov ECX,DWORD PTR [ESP+16]
            mov ESI,EDX
            shr ESI,CL
            mov ECX,DWORD PTR [ESP+120]
            sub EAX,EBP
            lea ECX,DWORD PTR [ECX+ESI*2]
            je $L_62c199

            lea ESI,DWORD PTR [EBX*2+(_KingBss+788080)]
$L_62c159::

            cmp WORD PTR [ECX],0
            jne $L_62c174

            mov WORD PTR [ESI-65536],0
            mov WORD PTR [ESI],0
            mov WORD PTR [ECX],BX
            inc EBX
            add ESI,2
$L_62c174::

            test DWORD PTR [ESP+20],EDX
            je $L_62c188

            xor EDI,EDI
            mov DI,WORD PTR [ECX]
            lea ECX,DWORD PTR [EDI*2+(_KingBss+788080)]
            jmp $L_62c194
$L_62c188::

            xor EDI,EDI
            mov DI,WORD PTR [ECX]
            lea ECX,DWORD PTR [EDI*2+(_KingBss+722544)]
$L_62c194::

            shl EDX,1
            dec EAX
            jne $L_62c159
$L_62c199::

            mov DX,WORD PTR [ESP+116]
            mov ESI,DWORD PTR [ESP+24]
            mov WORD PTR [ECX],DX
$L_62c1a5::

            mov EAX,DWORD PTR [ESP+28]
            mov ECX,DWORD PTR [ESP+116]
            mov WORD PTR [EAX],SI
            mov ESI,DWORD PTR [ESP+108]
$L_62c1b4::

            inc ECX
            cmp ECX,ESI
            mov DWORD PTR [ESP+116],ECX
            jb $L_62c0e3
$L_62c1c1::

            pop EDI
            pop ESI
            pop EBP
            pop EBX
            add ESP,88
            ret 
          BYTE 090H
          BYTE 090H

            nop
            nop
            nop
            nop
            nop
$L_62c02e ENDP
ALIGN 16
FUN_62c1d0 PROC

            push EBP
            push EDI
            push 9
            call FUN_62be70

            mov EBP,EAX
            add ESP,4
            test EBP,EBP
            jne $L_62c21a

            push 9
            call FUN_62be70

            mov EDX,EAX
            mov ECX,127
            xor EAX,EAX
            mov EDI,OFFSET (_KingBss+704108)

            rep stosd DWORD PTR ES:[EDI]

            stosw WORD PTR ES:[EDI]
            mov EAX,EDX
            add ESP,4
            mov DX,AX
            mov ECX,2048
            shl EDX,16
            mov DX,AX
            mov EDI,OFFSET (_KingBss+919472)
            mov EAX,EDX

            rep stosd DWORD PTR ES:[EDI]

            pop EDI
            pop EBP
            ret 
$L_62c21a::

            push ESI
            xor ESI,ESI
            test EBP,EBP
            jle $L_62c2ed

            push EBX
$L_62c226::

            mov ECX,DWORD PTR [(_KingBss+985152)]
            xor EBX,EBX
            and ECX,65535
            mov EAX,ECX
            shr EAX,8
            mov BX,WORD PTR [EAX*2+(_KingBss+985156)]
            cmp EBX,19
            jl $L_62c26e

            mov EAX,128
$L_62c24b::

            test EAX,ECX
            je $L_62c25b

            xor EDX,EDX
            mov DX,WORD PTR [EBX*2+(_KingBss+788080)]
            jmp $L_62c265
$L_62c25b::

            xor EDX,EDX
            mov DX,WORD PTR [EBX*2+(_KingBss+722544)]
$L_62c265::

            mov EBX,EDX
            shr EAX,1
            cmp EBX,19
            jge $L_62c24b
$L_62c26e::

            xor EAX,EAX
            mov AL,BYTE PTR [EBX+(_KingBss+985124)]
            push EAX
            call FUN_62bc30

            add ESP,4
            cmp EBX,2
            jg $L_62c2d2

            test EBX,EBX
            jne $L_62c28f

            mov EAX,1
            jmp $L_62c2b0
$L_62c28f::

            cmp EBX,1
            jne $L_62c2a3

            push 4
            call FUN_62be70

            add ESP,4
            add EAX,3
            jmp $L_62c2b0
$L_62c2a3::

            push 9
            call FUN_62be70

            add ESP,4
            add EAX,20
$L_62c2b0::

            dec EAX
            js $L_62c2dc

            lea EDX,DWORD PTR [EAX+1]
            lea EDI,DWORD PTR [ESI+(_KingBss+704108)]
            mov ECX,EDX
            xor EAX,EAX
            mov EBX,ECX
            shr ECX,2

            rep stosd DWORD PTR ES:[EDI]

            mov ECX,EBX
            and ECX,3
            add ESI,EDX

            rep stosb BYTE PTR ES:[EDI]

            jmp $L_62c2dc
$L_62c2d2::

            sub BL,2
            mov BYTE PTR [ESI+(_KingBss+704108)],BL
            inc ESI
$L_62c2dc::

            cmp ESI,EBP
            jl $L_62c226

            cmp ESI,510
            pop EBX
            jge $L_62c30a
$L_62c2ed::

            mov ECX,510
            lea EDI,DWORD PTR [ESI+(_KingBss+704108)]
            sub ECX,ESI
            xor EAX,EAX
            mov EDX,ECX
            shr ECX,2

            rep stosd DWORD PTR ES:[EDI]

            mov ECX,EDX
            and ECX,3

            rep stosb BYTE PTR ES:[EDI]
$L_62c30a::

            push OFFSET (_KingBss+919472)
            push 12
            push OFFSET (_KingBss+704108)
            push 510
            call FUN_62bfc0

            add ESP,16
            pop ESI
            pop EDI
            pop EBP
            ret 
FUN_62c1d0 ENDP
ALIGN 16
FUN_62c330 PROC

            mov ECX,DWORD PTR [(_KingBss+985152)]
            push ESI
            and ECX,65535
            xor ESI,ESI
            mov EAX,ECX
            shr EAX,8
            mov SI,WORD PTR [EAX*2+(_KingBss+985156)]
            cmp ESI,14
            jb $L_62c379

            mov EAX,128
$L_62c356::

            test EAX,ECX
            je $L_62c366

            xor EDX,EDX
            mov DX,WORD PTR [ESI*2+(_KingBss+788080)]
            jmp $L_62c370
$L_62c366::

            xor EDX,EDX
            mov DX,WORD PTR [ESI*2+(_KingBss+722544)]
$L_62c370::

            mov ESI,EDX
            shr EAX,1
            cmp ESI,14
            jae $L_62c356
$L_62c379::

            xor EAX,EAX
            mov AL,BYTE PTR [ESI+(_KingBss+985124)]
            push EAX
            call FUN_62bc30

            add ESP,4
            test ESI,ESI
            je $L_62c3a8

            lea ECX,DWORD PTR [ESI-1]
            push ECX
            call FUN_62be70

            lea ECX,DWORD PTR [ESI-1]
            mov EDX,1
            shl EDX,CL
            add ESP,4
            pop ESI
            add EAX,EDX
            ret 
$L_62c3a8::

            mov EAX,ESI
            pop ESI
            ret 
FUN_62c330 ENDP
$L_62c3b0 PROC

            mov EAX,DWORD PTR [(_KingBss+704100)]
            sub ESP,56
            push EBP
            mov EBP,DWORD PTR [(_KingBss+704096)]
            cmp EAX,EBP
            push EDI
            jae $L_62c3da

            xor ECX,ECX
            mov CL,BYTE PTR [EAX+(_KingBss+671252)]
            inc EAX
            mov DWORD PTR [(_KingData+120784)],ECX
            mov DWORD PTR [(_KingBss+704100)],EAX
            jmp $L_62c3f1
$L_62c3da::

            push 0
            call FUN_62b8e0

            mov EBP,DWORD PTR [(_KingBss+704096)]
            add ESP,4
            mov DWORD PTR [(_KingData+120784)],EAX
            mov ECX,EAX
$L_62c3f1::

            mov EDI,ECX
            mov EAX,ECX
            and EDI,128
            and EAX,96
            mov DWORD PTR [(_KingData+123112)],EDI
            je $L_62c453

            mov EDX,DWORD PTR [(_KingBss+985036)]
            test EDX,EDX
            jne $L_62c440

            push EAX
            mov EAX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push EAX
            push OFFSET (_KingData+123240)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            mov EBP,DWORD PTR [(_KingBss+704096)]
            mov EDI,DWORD PTR [(_KingData+123112)]
            mov ECX,DWORD PTR [(_KingData+120784)]
            add ESP,20
$L_62c440::

            mov EAX,DWORD PTR [(_KingBss+985048)]
            test EAX,EAX
            jne $L_62c453

            mov DWORD PTR [(_KingBss+985048)],2
$L_62c453::

            and ECX,31
            mov EAX,1
            shl EAX,CL
            cmp ECX,16
            mov DWORD PTR [(_KingData+120784)],ECX
            mov DWORD PTR [ESP+56],EAX
            jle $L_62c4a2

            push 16
            push ECX
            mov ECX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push ECX
            push OFFSET (_KingData+123180)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,24
            mov DWORD PTR [(_KingBss+985048)],1
            mov EAX,1
            pop EDI
            pop EBP
            add ESP,56
            ret 
$L_62c4a2::

            mov EDX,DWORD PTR [(_KingBss+704100)]
            push EBX
            mov EAX,511
            push ESI
            mov DWORD PTR [ESP+44],EAX
            mov DWORD PTR [ESP+48],EAX
            xor ESI,ESI
            xor EAX,EAX
            shl EDX,3
            cmp EDI,ESI
            mov ECX,64
            setne AL
            add EAX,256
            mov EDI,OFFSET (_KingBss+722544)
            mov DWORD PTR [ESP+28],EAX
            xor EAX,EAX
            mov EBX,9
            mov DWORD PTR [ESP+52],EBP

            rep stosd DWORD PTR ES:[EDI]

            mov DWORD PTR [ESP+24],EBX
            mov DWORD PTR [ESP+40],4294967295
            mov DWORD PTR [ESP+36],ESI
            mov DWORD PTR [ESP+16],ESI
            mov EAX,255
$L_62c4fc::

            mov BYTE PTR [EAX+(_KingBss+853616)],AL
            dec EAX
            jns $L_62c4fc

            jmp $L_62c511
$L_62c507::

            mov EDX,DWORD PTR [ESP+20]
            mov EBP,DWORD PTR [(_KingBss+704096)]
$L_62c511::

            sar EDX,3
            mov EDI,EDX
            xor ECX,ECX
            sub EBP,EDI
            mov EAX,EBP
            test EAX,EAX
            jle $L_62c532
$L_62c520::

            mov DL,BYTE PTR [ECX+EDI*1+(_KingBss+671252)]
            mov BYTE PTR [ECX+(_KingBss+671252)],DL
            inc ECX
            cmp ECX,EAX
            jl $L_62c520
$L_62c532::

            mov ECX,EAX
            xor EBP,EBP
            cmp EAX,64
            mov DWORD PTR [(_KingBss+704096)],ECX
            mov DWORD PTR [ESP+20],EBP
            jae $L_62c588

            mov ECX,DWORD PTR [ESP+76]
            lea EAX,DWORD PTR [EAX+(_KingBss+671252)]
            push 32768
            push EAX
            push ECX
            call FUN_62a910

            mov EDI,EAX
            add ESP,12
            cmp EDI,-1
            mov DWORD PTR [ESP+52],EDI
            jne $L_62c56e

            call FUN_62bb00
$L_62c56e::

            mov ECX,DWORD PTR [(_KingBss+704096)]
            mov EAX,DWORD PTR [(_KingBss+919416)]
            add ECX,EDI
            add EAX,EDI
            mov DWORD PTR [(_KingBss+704096)],ECX
            mov DWORD PTR [(_KingBss+919416)],EAX
$L_62c588::

            mov EAX,DWORD PTR [ESP+52]
            test EAX,EAX
            je $L_62c5a1

            mov EAX,ECX
            xor EDX,EDX
            div EBX
            sub ECX,EDX
            shl ECX,3
            mov DWORD PTR [ESP+56],ECX
            jmp $L_62c5af
$L_62c5a1::

            lea EAX,DWORD PTR [ECX*8]
            sub EAX,EBX
            inc EAX
            mov DWORD PTR [ESP+56],EAX
$L_62c5af::

            mov EDX,DWORD PTR [ESP+28]
            lea EAX,DWORD PTR [EDX*2+(_KingBss+722544)]
            mov DWORD PTR [ESP+60],EAX
$L_62c5be::

            cmp DWORD PTR [ESP+56],EBP
            jle $L_62c8d2

            mov ECX,DWORD PTR [ESP+28]
            mov EAX,DWORD PTR [ESP+44]
            cmp ECX,EAX
            jg $L_62c80a

            mov EAX,EBP
            xor EBX,EBX
            sar EAX,3
            add EAX,OFFSET (_KingBss+671252)
            xor EDX,EDX
            mov ECX,EBP
            mov EDI,DWORD PTR [ESP+40]
            mov BH,BYTE PTR [EAX+2]
            mov DL,BYTE PTR [EAX]
            mov BL,BYTE PTR [EAX+1]
            and ECX,7
            shl EBX,8
            or EBX,EDX
            mov EDX,DWORD PTR [ESP+48]
            sar EBX,CL
            add EBP,DWORD PTR [ESP+24]
            mov DWORD PTR [ESP+20],EBP
            and EBX,EDX
            cmp EDI,-1
            jne $L_62c642

            cmp EBX,256
            jl $L_62c626

            push OFFSET (_KingData+123164)
            call FUN_62bad0
$L_62c3b0 ENDP
$L_62c623 PROC

            add ESP,4
$L_62c626::

            mov BYTE PTR [ESI+(_KingBss+704108)],BL
            inc ESI
            mov DWORD PTR [ESP+40],EBX
            mov DWORD PTR [ESP+36],EBX
            mov EBX,DWORD PTR [ESP+24]
            mov DWORD PTR [ESP+16],ESI
            jmp $L_62c5be
$L_62c642::

            cmp EBX,256
            jne $L_62c657

            mov EAX,DWORD PTR [(_KingData+123112)]
            test EAX,EAX
            jne $L_62c871
$L_62c657::

            mov ECX,DWORD PTR [ESP+28]
            mov DWORD PTR [ESP+68],EBX
            cmp EBX,ECX
            mov EAX,OFFSET (_KingBss+985006)
            jl $L_62c6ca

            jle $L_62c6b2

            mov EAX,DWORD PTR [(_KingBss+985044)]
            test EAX,EAX
            jne $L_62c696

            test ESI,ESI
            jle $L_62c696

            mov EAX,DWORD PTR [ESP+80]
            push ESI
            push OFFSET (_KingBss+704108)
            push EAX
            call FUN_62ba40

            mov EAX,DWORD PTR [(_KingBss+722540)]
            add ESP,12
            add EAX,ESI
            mov DWORD PTR [(_KingBss+722540)],EAX
$L_62c696::

            mov EAX,DWORD PTR [(_KingBss+985012)]
            test EAX,EAX
            mov EAX,OFFSET (_KingData+123164)
            jne $L_62c6a9

            mov EAX,OFFSET (_KingData+123116)
$L_62c6a9::

            push EAX
            call FUN_62bad0
$L_62c623 ENDP
$L_62c6af PROC

            add ESP,4
$L_62c6b2::

            mov CL,BYTE PTR [ESP+36]
            mov DWORD PTR [ESP+32],OFFSET (_KingBss+985005)
            mov EAX,DWORD PTR [ESP+32]
            mov BYTE PTR [(_KingBss+985005)],CL
            mov EBX,EDI
$L_62c6ca::

            cmp EBX,256
            jb $L_62c6ef
$L_62c6d2::

            mov DL,BYTE PTR [EBX+(_KingBss+853616)]
            xor ECX,ECX
            mov CX,WORD PTR [EBX*2+(_KingBss+722544)]
            dec EAX
            mov EBX,ECX
            cmp EBX,256
            mov BYTE PTR [EAX],DL
            jae $L_62c6d2
$L_62c6ef::

            xor ECX,ECX
            dec EAX
            mov CL,BYTE PTR [EBX+(_KingBss+853616)]
            mov EBP,OFFSET (_KingBss+985006)
            sub EBP,EAX
            mov DWORD PTR [ESP+36],ECX
            mov DWORD PTR [ESP+32],EAX
            mov BYTE PTR [EAX],CL
            lea EDX,DWORD PTR [ESI+EBP*1]
            cmp EDX,16384
            jl $L_62c7a5
$L_62c718::

            mov ECX,16384
            sub ECX,ESI
            cmp EBP,ECX
            jle $L_62c725

            mov EBP,ECX
$L_62c725::

            test EBP,EBP
            jle $L_62c74d

            mov EBX,DWORD PTR [ESP+16]
            mov ECX,EBP
            mov EDX,ECX
            mov ESI,EAX
            lea EDI,DWORD PTR [EBX+(_KingBss+704108)]
            shr ECX,2

            rep movsd 

            mov ECX,EDX
            and ECX,3
            add EBX,EBP

            rep movsb 

            mov DWORD PTR [ESP+16],EBX
            mov ESI,EBX
$L_62c74d::

            cmp ESI,16384
            jl $L_62c78e

            mov ECX,DWORD PTR [(_KingBss+985044)]
            test ECX,ECX
            jne $L_62c782

            mov EAX,DWORD PTR [ESP+80]
            push ESI
            push OFFSET (_KingBss+704108)
            push EAX
            call FUN_62ba40

            mov EAX,DWORD PTR [(_KingBss+722540)]
            add ESP,12
            add EAX,ESI
            mov DWORD PTR [(_KingBss+722540)],EAX
            mov EAX,DWORD PTR [ESP+32]
$L_62c782::

            mov DWORD PTR [ESP+16],0
            mov ESI,DWORD PTR [ESP+16]
$L_62c78e::

            add EAX,EBP
            mov EBP,OFFSET (_KingBss+985006)
            sub EBP,EAX
            mov DWORD PTR [ESP+32],EAX
            test EBP,EBP
            jg $L_62c718

            jmp $L_62c7c7
$L_62c7a5::

            mov ESI,EAX
            mov EAX,DWORD PTR [ESP+16]
            mov ECX,EBP
            mov DWORD PTR [ESP+16],EDX
            lea EDI,DWORD PTR [EAX+(_KingBss+704108)]
            mov EAX,ECX
            shr ECX,2

            rep movsd 

            mov ECX,EAX
            and ECX,3

            rep movsb 

            mov ESI,EDX
$L_62c7c7::

            mov EAX,DWORD PTR [ESP+28]
            mov ECX,DWORD PTR [ESP+64]
            cmp EAX,ECX
            jge $L_62c7f5

            mov ECX,DWORD PTR [ESP+60]
            mov DX,WORD PTR [ESP+40]
            inc EAX
            mov WORD PTR [ECX],DX
            mov DL,BYTE PTR [ESP+36]
            mov BYTE PTR [EAX+(_KingBss+853615)],DL
            add ECX,2
            mov DWORD PTR [ESP+28],EAX
            mov DWORD PTR [ESP+60],ECX
$L_62c7f5::

            mov EAX,DWORD PTR [ESP+68]
            mov EBP,DWORD PTR [ESP+20]
            mov EBX,DWORD PTR [ESP+24]
            mov DWORD PTR [ESP+40],EAX
            jmp $L_62c5be
$L_62c80a::

            lea ECX,DWORD PTR [EBX*8]
            lea EDI,DWORD PTR [EBX*8]
            lea EAX,DWORD PTR [ECX+EBP*1-1]
            cdq 
            idiv EDI
            mov EAX,DWORD PTR [(_KingData+120784)]
            sub ECX,EDX
            inc EBX
            cmp EBX,EAX
            mov DWORD PTR [ESP+24],EBX
            lea ECX,DWORD PTR [ECX+EBP*1-1]
            mov DWORD PTR [ESP+20],ECX
            jne $L_62c852

            mov EDX,DWORD PTR [ESP+64]
            mov EAX,1
            mov ECX,EBX
            mov DWORD PTR [ESP+44],EDX
            shl EAX,CL
            dec EAX
            mov DWORD PTR [ESP+48],EAX
            jmp $L_62c507
$L_62c852::

            mov EAX,1
            mov ECX,EBX
            shl EAX,CL
            dec EAX
            mov DWORD PTR [ESP+44],EAX
            mov EAX,1
            shl EAX,CL
            dec EAX
            mov DWORD PTR [ESP+48],EAX
            jmp $L_62c507
$L_62c871::

            mov EDX,DWORD PTR [ESP+24]
            mov ECX,64
            xor EAX,EAX
            mov EDI,OFFSET (_KingBss+722544)

            rep stosd DWORD PTR ES:[EDI]

            lea ECX,DWORD PTR [EDX*8]
            lea EDI,DWORD PTR [EDX*8]
            mov DWORD PTR [ESP+24],9
            mov DWORD PTR [ESP+28],256
            lea EAX,DWORD PTR [ECX+EBP*1-1]
            mov EBX,DWORD PTR [ESP+24]
            cdq 
            idiv EDI
            sub ECX,EDX
            lea EAX,DWORD PTR [ECX+EBP*1-1]
            mov EBP,DWORD PTR [(_KingBss+704096)]
            mov DWORD PTR [ESP+20],EAX
            mov EAX,511
            mov EDX,DWORD PTR [ESP+20]
            mov DWORD PTR [ESP+44],EAX
            mov DWORD PTR [ESP+48],EAX
            jmp $L_62c511
$L_62c8d2::

            mov EAX,DWORD PTR [ESP+52]
            test EAX,EAX
            jne $L_62c507

            mov EAX,DWORD PTR [(_KingBss+985044)]
            test EAX,EAX
            jne $L_62c90a

            test ESI,ESI
            jle $L_62c90a

            mov ECX,DWORD PTR [ESP+80]
            push ESI
            push OFFSET (_KingBss+704108)
            push ECX
            call FUN_62ba40

            mov EAX,DWORD PTR [(_KingBss+722540)]
            add ESP,12
            add EAX,ESI
            mov DWORD PTR [(_KingBss+722540)],EAX
$L_62c90a::

            pop ESI
            pop EBX
            pop EDI
            xor EAX,EAX
            pop EBP
            add ESP,56
            ret 
$L_62c6af ENDP
$L_62c920 PROC

            push ECX
            mov EAX,DWORD PTR [ESP+8]
            mov ECX,DWORD PTR [ESP+12]
            push EBX
            push EBP
            push ESI
            push EDI
            mov DWORD PTR [(_KingBss+671248)],EAX
            mov DWORD PTR [(_KingBss+704084)],ECX
            call FUN_62cb10

            call FUN_62ccc0

            mov EBX,DWORD PTR [(_KingBss+986260)]
            mov EAX,DWORD PTR [(_KingBss+986144)]
            mov EDI,1
            mov ECX,EBX
            mov EAX,DWORD PTR [EAX*4+(_KingBss+985676)]
            mov EBP,DWORD PTR [(_KingBss+704100)]
            shl EDI,CL
            xor EDX,EDX
            xor ESI,ESI
            mov DWORD PTR [(_KingBss+986256)],EDX
            mov DWORD PTR [(_KingBss+986036)],ESI
            dec EDI
            dec EAX
            mov DWORD PTR [ESP+28],EDI
            mov DWORD PTR [ESP+16],EAX
$L_62c97d::

            cmp EDX,EBX
            jge $L_62c9d4
$L_62c981::

            cmp EBP,DWORD PTR [(_KingBss+704096)]
            jae $L_62c99a

            xor EAX,EAX
            mov AL,BYTE PTR [EBP+(_KingBss+671252)]
            inc EBP
            mov DWORD PTR [(_KingBss+704100)],EBP
            jmp $L_62c9bc
$L_62c99a::

            push 0
            call FUN_62b8e0

            mov EDX,DWORD PTR [(_KingBss+986256)]
            mov ESI,DWORD PTR [(_KingBss+986036)]
            mov EBP,DWORD PTR [(_KingBss+704100)]
            mov EBX,DWORD PTR [(_KingBss+986260)]
            add ESP,4
$L_62c9bc::

            shl ESI,8
            add EDX,8
            or ESI,EAX
            cmp EDX,EBX
            mov DWORD PTR [(_KingBss+986036)],ESI
            mov DWORD PTR [(_KingBss+986256)],EDX
            jl $L_62c981
$L_62c9d4::

            mov ECX,EDX
            mov EAX,ESI
            sub ECX,EBX
            shr EAX,CL
            xor ECX,ECX
            and EAX,EDI
            mov CL,BYTE PTR [EAX+(_KingBss+704108)]
            mov EDI,ECX
            test EDI,EDI
            jle $L_62c9f7

            mov ECX,EBX
            sub ECX,EDI
            shr EAX,CL
            jmp $L_62ca85
$L_62c9f7::

            mov EDI,DWORD PTR [(_KingBss+986260)]
            mov EBX,DWORD PTR [ESP+28]
            lea EAX,DWORD PTR [EDI*4+(_KingBss+986040)]
            mov DWORD PTR [ESP+24],EAX
$L_62ca0c::

            mov ECX,DWORD PTR [ESP+24]
            inc EDI
            add ECX,4
            cmp EDX,EDI
            mov DWORD PTR [ESP+24],ECX
            lea EBX,DWORD PTR [EBX+EBX*1+1]
            jge $L_62ca6d
$L_62ca20::

            cmp EBP,DWORD PTR [(_KingBss+704096)]
            jae $L_62ca39

            xor EAX,EAX
            mov AL,BYTE PTR [EBP+(_KingBss+671252)]
            inc EBP
            mov DWORD PTR [(_KingBss+704100)],EBP
            jmp $L_62ca55
$L_62ca39::

            push 0
            call FUN_62b8e0

            mov EDX,DWORD PTR [(_KingBss+986256)]
            mov ESI,DWORD PTR [(_KingBss+986036)]
            mov EBP,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62ca55::

            shl ESI,8
            add EDX,8
            or ESI,EAX
            cmp EDX,EDI
            mov DWORD PTR [(_KingBss+986036)],ESI
            mov DWORD PTR [(_KingBss+986256)],EDX
            jl $L_62ca20
$L_62ca6d::

            mov ECX,EDX
            mov EAX,ESI
            sub ECX,EDI
            shr EAX,CL
            mov ECX,DWORD PTR [ESP+24]
            and EAX,EBX
            cmp EAX,DWORD PTR [ECX]
            jb $L_62ca0c

            mov EBX,DWORD PTR [(_KingBss+986260)]
$L_62ca85::

            cmp EAX,DWORD PTR [ESP+16]
            jne $L_62ca93

            cmp EDI,DWORD PTR [(_KingBss+986144)]
            je $L_62cae7
$L_62ca93::

            mov ECX,DWORD PTR [EDI*4+(_KingBss+986152)]
            mov CL,BYTE PTR [ECX+EAX*1+(_KingBss+985780)]
            mov EAX,DWORD PTR [(_KingBss+704104)]
            mov BYTE PTR [EAX+(_KingBss+853616)],CL
            inc EAX
            cmp EAX,32768
            mov DWORD PTR [(_KingBss+704104)],EAX
            jne $L_62cad6

            call FUN_62b9e0

            mov EDX,DWORD PTR [(_KingBss+986256)]
            mov ESI,DWORD PTR [(_KingBss+986036)]
            mov EBP,DWORD PTR [(_KingBss+704100)]
            mov EBX,DWORD PTR [(_KingBss+986260)]
$L_62cad6::

            sub EDX,EDI
            mov EDI,DWORD PTR [ESP+28]
            mov DWORD PTR [(_KingBss+986256)],EDX
            jmp $L_62c97d
$L_62cae7::

            call FUN_62b9e0

            mov EDX,DWORD PTR [(_KingBss+986148)]
            mov EAX,DWORD PTR [(_KingBss+722540)]
            pop EDI
            pop ESI
            pop EBP
            cmp EDX,EAX
            pop EBX
            je $L_62cb0c

            push OFFSET (_KingData+123280)
            call FUN_62bad0

            add ESP,4
$L_62cb0c::

            xor EAX,EAX
            pop ECX
            ret 
$L_62c920 ENDP
ALIGN 16
FUN_62cb10 PROC

            mov EDX,DWORD PTR [(_KingBss+704100)]
            push EBP
            push ESI
            push EDI
            mov DWORD PTR [(_KingBss+986148)],0
            mov ESI,4
$L_62cb28::

            cmp EDX,DWORD PTR [(_KingBss+704096)]
            jae $L_62cb41

            xor EAX,EAX
            mov AL,BYTE PTR [EDX+(_KingBss+671252)]
            inc EDX
            mov DWORD PTR [(_KingBss+704100)],EDX
            jmp $L_62cb51
$L_62cb41::

            push 0
            call FUN_62b8e0

            mov EDX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62cb51::

            mov ECX,DWORD PTR [(_KingBss+986148)]
            shl ECX,8
            or ECX,EAX
            dec ESI
            mov DWORD PTR [(_KingBss+986148)],ECX
            jne $L_62cb28

            cmp EDX,DWORD PTR [(_KingBss+704096)]
            jae $L_62cb84

            xor ECX,ECX
            mov CL,BYTE PTR [EDX+(_KingBss+671252)]
            inc EDX
            mov DWORD PTR [(_KingBss+986144)],ECX
            mov DWORD PTR [(_KingBss+704100)],EDX
            jmp $L_62cb9c
$L_62cb84::

            push 0
            call FUN_62b8e0

            mov EDX,DWORD PTR [(_KingBss+704100)]
            mov ECX,EAX
            add ESP,4
            mov DWORD PTR [(_KingBss+986144)],ECX
$L_62cb9c::

            cmp ECX,25
            jle $L_62cbba

            push OFFSET (_KingData+123352)
            call FUN_62bad0
FUN_62cb10 ENDP
$L_62cbab PROC

            mov EDX,DWORD PTR [(_KingBss+704100)]
            mov ECX,DWORD PTR [(_KingBss+986144)]
            add ESP,4
$L_62cbba::

            mov ESI,1
            xor EBP,EBP
            cmp ECX,ESI
            jl $L_62cc26

            mov EDI,OFFSET (_KingBss+985680)
$L_62cbca::

            cmp EDX,DWORD PTR [(_KingBss+704096)]
            jae $L_62cbe3

            xor EAX,EAX
            mov AL,BYTE PTR [EDX+(_KingBss+671252)]
            inc EDX
            mov DWORD PTR [(_KingBss+704100)],EDX
            jmp $L_62cbf9
$L_62cbe3::

            push 0
            call FUN_62b8e0

            mov EDX,DWORD PTR [(_KingBss+704100)]
            mov ECX,DWORD PTR [(_KingBss+986144)]
            add ESP,4
$L_62cbf9::

            add EBP,EAX
            mov DWORD PTR [EDI],EAX
            inc ESI
            add EDI,4
            cmp ESI,ECX
            jle $L_62cbca

            cmp EBP,256
            jle $L_62cc26

            push OFFSET (_KingData+123320)
            call FUN_62bad0
$L_62cbab ENDP
$L_62cc17 PROC

            mov EDX,DWORD PTR [(_KingBss+704100)]
            mov ECX,DWORD PTR [(_KingBss+986144)]
            add ESP,4
$L_62cc26::

            mov ESI,DWORD PTR [ECX*4+(_KingBss+985676)]
            mov EBP,1
            inc ESI
            mov DWORD PTR [ECX*4+(_KingBss+985676)],ESI
            xor ESI,ESI
            cmp ECX,EBP
            jl $L_62ccaa

            push EBX
            xor EDI,EDI
$L_62cc43::

            mov EAX,DWORD PTR [EDI+(_KingBss+985680)]
            mov DWORD PTR [EDI+(_KingBss+986156)],ESI
            test EAX,EAX
            jle $L_62cc8e

            mov EBX,EAX
$L_62cc55::

            cmp EDX,DWORD PTR [(_KingBss+704096)]
            jae $L_62cc6e

            xor EAX,EAX
            mov AL,BYTE PTR [EDX+(_KingBss+671252)]
            inc EDX
            mov DWORD PTR [(_KingBss+704100)],EDX
            jmp $L_62cc84
$L_62cc6e::

            push 0
            call FUN_62b8e0

            mov EDX,DWORD PTR [(_KingBss+704100)]
            mov ECX,DWORD PTR [(_KingBss+986144)]
            add ESP,4
$L_62cc84::

            mov BYTE PTR [ESI+(_KingBss+985780)],AL
            inc ESI
            dec EBX
            jne $L_62cc55
$L_62cc8e::

            inc EBP
            add EDI,4
            cmp EBP,ECX
            jle $L_62cc43

            mov EAX,DWORD PTR [ECX*4+(_KingBss+985676)]
            pop EBX
            inc EAX
            pop EDI
            pop ESI
            mov DWORD PTR [ECX*4+(_KingBss+985676)],EAX
            pop EBP
            ret 
$L_62ccaa::

            mov EAX,DWORD PTR [ECX*4+(_KingBss+985676)]
            pop EDI
            inc EAX
            pop ESI
            mov DWORD PTR [ECX*4+(_KingBss+985676)],EAX
            pop EBP
            ret 
$L_62cc17 ENDP
ALIGN 16
FUN_62ccc0 PROC

            push EBX
            push ESI
            mov ESI,DWORD PTR [(_KingBss+986144)]
            xor EAX,EAX
            cmp ESI,1
            mov ECX,ESI
            jl $L_62ccf9
$L_62ccd1::

            mov EDX,DWORD PTR [ECX*4+(_KingBss+986152)]
            mov EBX,DWORD PTR [ECX*4+(_KingBss+985676)]
            sar EAX,1
            sub EDX,EAX
            mov DWORD PTR [ECX*4+(_KingBss+986040)],EAX
            mov DWORD PTR [ECX*4+(_KingBss+986152)],EDX
            add EAX,EBX
            dec ECX
            cmp ECX,1
            jge $L_62ccd1
$L_62ccf9::

            cmp ESI,12
            jle $L_62cd03

            mov ESI,12
$L_62cd03::

            mov EAX,1
            mov ECX,ESI
            shl EAX,CL
            mov EBX,1
            mov DWORD PTR [(_KingBss+986260)],ESI
            add EAX,OFFSET (_KingBss+704108)
            cmp ESI,EBX
            jl $L_62cd4d

            push EDI
            mov EDI,OFFSET (_KingBss+985680)
$L_62cd26::

            mov EDX,DWORD PTR [EDI]
            mov ECX,ESI
            sub ECX,EBX
            shl EDX,CL
            mov ECX,EDX
            dec EDX
            test ECX,ECX
            je $L_62cd44

            lea ECX,DWORD PTR [EDX+1]
$L_62cd38::

            dec EAX
            dec ECX
            mov BYTE PTR [EAX],BL
            jne $L_62cd38

            mov ESI,DWORD PTR [(_KingBss+986260)]
$L_62cd44::

            inc EBX
            add EDI,4
            cmp EBX,ESI
            jle $L_62cd26

            pop EDI
$L_62cd4d::

            pop ESI
            cmp EAX,OFFSET (_KingBss+704108)
            pop EBX
            jbe $L_62cd61
$L_62cd56::

            dec EAX
            cmp EAX,OFFSET (_KingBss+704108)
            mov BYTE PTR [EAX],0
            ja $L_62cd56
$L_62cd61::

            ret 
FUN_62ccc0 ENDP
ALIGN 16
FUN_62cd70 PROC

            mov ECX,DWORD PTR [ESP+4]
            push ESI
            mov ESI,DWORD PTR [(_KingBss+704100)]
            xor EDX,EDX
            mov DWORD PTR [(_KingBss+671248)],ECX
            xor ECX,ECX
            mov DH,BYTE PTR [ESI+(_KingBss+671281)]
            mov CH,BYTE PTR [ESI+(_KingBss+671279)]
            mov DL,BYTE PTR [ESI+(_KingBss+671280)]
            mov CL,BYTE PTR [ESI+(_KingBss+671278)]
            lea EAX,DWORD PTR [ESI+(_KingBss+671252)]
            add EDX,ESI
            lea ECX,DWORD PTR [EDX+ECX*1+30]
            mov EDX,DWORD PTR [(_KingBss+704096)]
            cmp ECX,EDX
            mov DWORD PTR [(_KingBss+704100)],ECX
            ja $L_62ce58

            xor EDX,EDX
            xor ECX,ECX
            mov DH,BYTE PTR [EAX+3]
            mov CL,BYTE PTR [EAX+1]
            mov DL,BYTE PTR [EAX+2]
            shl EDX,8
            or EDX,ECX
            xor ECX,ECX
            mov CL,BYTE PTR [EAX]
            shl EDX,8
            or EDX,ECX
            cmp EDX,67324752
            jne $L_62ce58

            xor ECX,ECX
            mov CL,BYTE PTR [EAX+8]
            test ECX,ECX
            mov DWORD PTR [(_KingData+120788)],ECX
            je $L_62ce07

            cmp ECX,8
            je $L_62ce07

            mov EDX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push EDX
            push OFFSET (_KingData+123476)
            jmp $L_62ce69
$L_62ce07::

            mov CL,BYTE PTR [EAX+6]
            mov ESI,1
            and ECX,ESI
            mov DWORD PTR [(_KingBss+986268)],ECX
            je $L_62ce41

            mov EDX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push EDX
            push OFFSET (_KingData+123436)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,16
            mov DWORD PTR [(_KingBss+985048)],ESI
            mov EAX,ESI
            pop ESI
            ret 
$L_62ce41::

            mov AL,BYTE PTR [EAX+6]
            mov DWORD PTR [(_KingBss+986272)],ESI
            shr EAX,3
            and EAX,ESI
            pop ESI
            mov DWORD PTR [(_KingBss+986276)],EAX
            xor EAX,EAX
            ret 
$L_62ce58::

            mov ECX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push ECX
            push OFFSET (_KingData+123404)
$L_62ce69::

            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,16
            mov ESI,1
            mov DWORD PTR [(_KingBss+985048)],ESI
            mov EAX,ESI
            pop ESI
            ret 
FUN_62cd70 ENDP
$L_62ce90 PROC

            mov EAX,DWORD PTR [ESP+4]
            mov ECX,DWORD PTR [ESP+8]
            sub ESP,20
            mov DWORD PTR [(_KingBss+671248)],EAX
            mov DWORD PTR [(_KingBss+704084)],ECX
            push EBX
            push EBP
            push ESI
            xor EBX,EBX
            push EDI
            push EBX
            push EBX
            xor ESI,ESI
            xor EDI,EDI
            call FUN_62b870

            mov EBP,DWORD PTR [(_KingBss+986272)]
            mov CL,BYTE PTR [(_KingBss+671277)]
            mov DL,BYTE PTR [(_KingBss+671276)]
            add ESP,8
            cmp EBP,EBX
            je $L_62cf2c

            cmp DWORD PTR [(_KingBss+986276)],EBX
            jne $L_62cf2c

            mov EAX,DWORD PTR [(_KingBss+671268)]
            mov BL,AH
            and EAX,255
            mov ESI,EBX
            shl ESI,8
            or ESI,EAX
            xor EAX,EAX
            mov AL,BYTE PTR [(_KingBss+671267)]
            shl ESI,8
            or ESI,EAX
            xor EAX,EAX
            mov AL,BYTE PTR [(_KingBss+671266)]
            shl ESI,8
            or ESI,EAX
            xor EAX,EAX
            mov AL,CL
            mov EDI,EAX
            xor EAX,EAX
            shl EDI,8
            mov AL,DL
            or EDI,EAX
            xor EAX,EAX
            mov AL,BYTE PTR [(_KingBss+671275)]
            shl EDI,8
            or EDI,EAX
            xor EAX,EAX
            mov AL,BYTE PTR [(_KingBss+671274)]
            shl EDI,8
            or EDI,EAX
            xor EBX,EBX
$L_62cf2c::

            mov EAX,DWORD PTR [(_KingData+120788)]
            cmp EAX,8
            jne $L_62cf5c

            call _Gz_Inflate

            cmp EAX,3
            jne $L_62cf4a

            push OFFSET (_KingData+123028)
            jmp $L_62d055
$L_62cf4a::

            cmp EAX,EBX
            je $L_62d05d

            push OFFSET (_KingData+123764)
            jmp $L_62d055
$L_62cf5c::

            cmp EBP,EBX
            je $L_62d050

            cmp EAX,EBX
            jne $L_62d050

            xor EBX,EBX
            xor EAX,EAX
            mov BL,CL
            xor ECX,ECX
            shl EBX,8
            mov CL,DL
            mov AL,BYTE PTR [(_KingBss+671274)]
            or EBX,ECX
            xor EDX,EDX
            mov DL,BYTE PTR [(_KingBss+671275)]
            mov ECX,DWORD PTR [(_KingBss+671272)]
            shl EBX,8
            or EBX,EDX
            xor EDX,EDX
            shl EBX,8
            or EBX,EAX
            xor EAX,EAX
            mov AL,CH
            and ECX,255
            shl EAX,8
            mov DL,BYTE PTR [(_KingBss+671270)]
            or EAX,ECX
            xor ECX,ECX
            mov CL,BYTE PTR [(_KingBss+671271)]
            shl EAX,8
            or EAX,ECX
            mov ECX,DWORD PTR [(_KingBss+986268)]
            shl EAX,8
            or EAX,EDX
            neg ECX
            sbb ECX,ECX
            mov EDX,EAX
            and ECX,12
            sub EDX,ECX
            cmp EBX,EDX
            je $L_62cff4

            push EAX
            push EBX
            push OFFSET (_KingData+123744)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            push OFFSET (_KingData+123700)
            call FUN_62bad0
$L_62ce90 ENDP
$L_62cff1 PROC

            add ESP,20
$L_62cff4::

            mov EAX,EBX
            dec EBX
            test EAX,EAX
            je $L_62d047

            inc EBX
$L_62cffc::

            mov ECX,DWORD PTR [(_KingBss+704100)]
            mov EAX,DWORD PTR [(_KingBss+704096)]
            cmp ECX,EAX
            jae $L_62d01a

            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62d024
$L_62d01a::

            push 0
            call FUN_62b8e0

            add ESP,4
$L_62d024::

            mov ECX,DWORD PTR [(_KingBss+704104)]
            mov BYTE PTR [ECX+(_KingBss+853616)],AL
            inc ECX
            cmp ECX,32768
            mov DWORD PTR [(_KingBss+704104)],ECX
            jne $L_62d044

            call FUN_62b9e0
$L_62d044::

            dec EBX
            jne $L_62cffc
$L_62d047::

            call FUN_62b9e0

            xor EBX,EBX
            jmp $L_62d05d
$L_62d050::

            push OFFSET (_KingData+123668)
$L_62d055::

            call FUN_62bad0
$L_62cff1 ENDP
$L_62d05a PROC

            add ESP,4
$L_62d05d::

            cmp DWORD PTR [(_KingBss+986272)],EBX
            jne $L_62d10d

            mov ECX,DWORD PTR [(_KingBss+704100)]
            xor ESI,ESI
$L_62d071::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62d08a

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62d099
$L_62d08a::

            push EBX
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62d099::

            mov BYTE PTR [ESP+ESI*1+16],AL
            inc ESI
            cmp ESI,8
            jl $L_62d071

            mov ESI,DWORD PTR [ESP+19]
            mov ECX,DWORD PTR [ESP+18]
            mov EDI,DWORD PTR [ESP+23]
            mov EDX,DWORD PTR [ESP+17]
            and ESI,255
            and ECX,255
            shl ESI,8
            mov EAX,DWORD PTR [ESP+16]
            or ESI,ECX
            mov ECX,DWORD PTR [ESP+22]
            and EDI,255
            shl ESI,8
            and EDX,255
            and ECX,255
            shl EDI,8
            or ESI,EDX
            mov EDX,DWORD PTR [ESP+21]
            or EDI,ECX
            and EDX,255
            shl EDI,8
            shl ESI,8
            and EAX,255
            or EDI,EDX
            or ESI,EAX
            mov EAX,DWORD PTR [ESP+20]
            shl EDI,8
            jmp $L_62d1b8
$L_62d10d::

            cmp DWORD PTR [(_KingBss+986276)],EBX
            je $L_62d1bf

            mov ECX,DWORD PTR [(_KingBss+704100)]
            xor ESI,ESI
$L_62d121::

            cmp ECX,DWORD PTR [(_KingBss+704096)]
            jae $L_62d13a

            xor EAX,EAX
            mov AL,BYTE PTR [ECX+(_KingBss+671252)]
            inc ECX
            mov DWORD PTR [(_KingBss+704100)],ECX
            jmp $L_62d149
$L_62d13a::

            push EBX
            call FUN_62b8e0

            mov ECX,DWORD PTR [(_KingBss+704100)]
            add ESP,4
$L_62d149::

            mov BYTE PTR [ESP+ESI*1+16],AL
            inc ESI
            cmp ESI,16
            jl $L_62d121

            mov ESI,DWORD PTR [ESP+23]
            mov ECX,DWORD PTR [ESP+22]
            mov EDI,DWORD PTR [ESP+31]
            mov EDX,DWORD PTR [ESP+21]
            and ESI,255
            and ECX,255
            shl ESI,8
            mov EAX,DWORD PTR [ESP+20]
            or ESI,ECX
            mov ECX,DWORD PTR [ESP+30]
            and EDI,255
            shl ESI,8
            and EDX,255
            and ECX,255
            shl EDI,8
            or ESI,EDX
            mov EDX,DWORD PTR [ESP+29]
            or EDI,ECX
            and EDX,255
            shl EDI,8
            shl ESI,8
            and EAX,255
            or EDI,EDX
            or ESI,EAX
            mov EAX,DWORD PTR [ESP+28]
            shl EDI,8
$L_62d1b8::

            and EAX,255
            or EDI,EAX
$L_62d1bf::

            push EBX
            push OFFSET (_KingBss+704108)
            call FUN_62b870

            add ESP,8
            cmp ESI,EAX
            je $L_62d1de

            push OFFSET (_KingData+123632)
            call FUN_62bad0
$L_62d05a ENDP
$L_62d1db PROC

            add ESP,4
$L_62d1de::

            cmp EDI,DWORD PTR [(_KingBss+722540)]
            je $L_62d1f3

            push OFFSET (_KingData+123280)
            call FUN_62bad0
$L_62d1db ENDP
$L_62d1f0 PROC

            add ESP,4
$L_62d1f3::

            cmp DWORD PTR [(_KingBss+986272)],EBX
            je $L_62d281

            mov EAX,DWORD PTR [(_KingBss+704100)]
            mov EDX,DWORD PTR [(_KingBss+704096)]
            lea ECX,DWORD PTR [EAX+4]
            cmp ECX,EDX
            jae $L_62d281

            xor EDX,EDX
            xor ECX,ECX
            mov DH,BYTE PTR [EAX+(_KingBss+671255)]
            mov CL,BYTE PTR [EAX+(_KingBss+671253)]
            mov DL,BYTE PTR [EAX+(_KingBss+671254)]
            shl EDX,8
            or EDX,ECX
            xor ECX,ECX
            mov CL,BYTE PTR [EAX+(_KingBss+671252)]
            shl EDX,8
            or EDX,ECX
            cmp EDX,67324752
            jne $L_62d281

            cmp DWORD PTR [(_KingBss+985012)],EBX
            je $L_62d297

            cmp DWORD PTR [(_KingBss+985036)],EBX
            jne $L_62d26f

            mov EDX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push EDX
            push OFFSET (_KingData+123584)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,16
$L_62d26f::

            cmp DWORD PTR [(_KingBss+985048)],EBX
            jne $L_62d281

            mov DWORD PTR [(_KingBss+985048)],2
$L_62d281::

            pop EDI
            pop ESI
            mov DWORD PTR [(_KingBss+986272)],EBX
            mov DWORD PTR [(_KingBss+986276)],EBX
            pop EBP
            xor EAX,EAX
            pop EBX
            add ESP,20
            ret 
$L_62d297::

            mov EAX,DWORD PTR [(_KingBss+919420)]
            push OFFSET (_KingBss+919156)
            push EAX
            push OFFSET (_KingData+123536)
            push OFFSET (_KingData+107392)
            call _OldCrt_fprintf

            add ESP,16
            mov EAX,1
            mov DWORD PTR [(_KingBss+986272)],EBX
            mov DWORD PTR [(_KingBss+986276)],EBX
            pop EDI
            pop ESI
            pop EBP
            mov DWORD PTR [(_KingBss+985048)],EAX
            pop EBX
            add ESP,20
            ret 
$L_62d1f0 ENDP
ALIGN 16
FUN_62d2e0 PROC

            push ESI
            mov ESI,DWORD PTR [ESP+8]
            cmp ESI,1
            push EDI
            jl $L_62d2f0

            cmp ESI,9
            jle $L_62d2fd
$L_62d2f0::

            push OFFSET (_KingData+123896)
            call FUN_62bad0
FUN_62d2e0 ENDP
$L_62d2fa PROC

            add ESP,4
$L_62d2fd::

            mov ECX,16384
            xor EAX,EAX
            mov EDI,OFFSET (_KingBss+788080)
            xor EDX,EDX
            mov DX,WORD PTR [ESI*8+(_KingData+123816)+4]
            mov DWORD PTR [(_KingBss+986308)],ESI

            rep stosd DWORD PTR ES:[EDI]

            mov AX,WORD PTR [ESI*8+(_KingData+123816)+2]
            xor ECX,ECX
            mov CX,WORD PTR [ESI*8+(_KingData+123816)]
            mov DWORD PTR [(_KingBss+986288)],EAX
            xor EAX,EAX
            cmp ESI,1
            mov AX,WORD PTR [ESI*8+(_KingData+123816)+6]
            mov DWORD PTR [(_KingBss+986312)],ECX
            mov DWORD PTR [(_KingBss+986300)],EDX
            mov DWORD PTR [(_KingBss+986304)],EAX
            jne $L_62d35b

            mov EAX,DWORD PTR [ESP+16]
            or BYTE PTR [EAX],4
            jmp $L_62d367
$L_62d35b::

            cmp ESI,9
            jne $L_62d367

            mov EAX,DWORD PTR [ESP+16]
            or BYTE PTR [EAX],2
$L_62d367::

            xor ESI,ESI
            push 65536
            push OFFSET (_KingBss+853616)
            mov DWORD PTR [(_KingBss+986284)],ESI
            mov DWORD PTR [(_KingBss+986320)],ESI
            call DWORD PTR [(_KingBss+998352)]

            add ESP,8
            cmp EAX,ESI
            mov DWORD PTR [(_KingBss+986324)],EAX
            je $L_62d3e6

            cmp EAX,-1
            je $L_62d3e6

            mov EDI,262
            mov DWORD PTR [(_KingBss+986296)],ESI
            cmp EAX,EDI
            jae $L_62d3ba
$L_62d3a5::

            cmp DWORD PTR [(_KingBss+986296)],ESI
            jne $L_62d3ba

            call _Gz_FillWindow

            cmp DWORD PTR [(_KingBss+986324)],EDI
            jb $L_62d3a5
$L_62d3ba::

            xor ECX,ECX
            xor EAX,EAX
$L_62d3be::

            xor EDX,EDX
            and ECX,1023
            mov DL,BYTE PTR [EAX+(_KingBss+853616)]
            and EDX,32767
            shl ECX,5
            xor ECX,EDX
            inc EAX
            cmp EAX,2
            jb $L_62d3be

            pop EDI
            mov DWORD PTR [(_KingBss+986292)],ECX
            pop ESI
            ret 
$L_62d3e6::

            mov DWORD PTR [(_KingBss+986324)],ESI
            pop EDI
            mov DWORD PTR [(_KingBss+986296)],1
            pop ESI
            ret 
          BYTE 090H
          BYTE 090H

            nop
            nop
            nop
            nop
            nop
$L_62d2fa ENDP
ALIGN 16
ALIGN 16
ALIGN 16
FUN_62d650 PROC

            push ECX
            mov EAX,DWORD PTR [(_KingBss+986308)]
            push EBP
            cmp EAX,3
            mov DWORD PTR [ESP+4],0
            mov EBP,2
            jg $L_62d671

            call FUN_62d960

            pop EBP
            pop ECX
            ret 
$L_62d671::

            mov EAX,DWORD PTR [(_KingBss+986324)]
            test EAX,EAX
            je $L_62d91b

            mov EDX,DWORD PTR [(_KingBss+986292)]
            mov ECX,DWORD PTR [(_KingBss+986284)]
            push EBX
            push ESI
            push EDI
$L_62d68d::

            xor EAX,EAX
            and EDX,1023
            mov AL,BYTE PTR [ECX+(_KingBss+853618)]
            mov ESI,ECX
            and EAX,32767
            mov EDI,DWORD PTR [(_KingBss+986316)]
            shl EDX,5
            xor EDX,EAX
            xor EAX,EAX
            and ESI,32767
            mov DWORD PTR [(_KingBss+986292)],EDX
            mov AX,WORD PTR [EDX*2+(_KingBss+788080)]
            mov WORD PTR [ESI*2+(_KingBss+722544)],AX
            mov ESI,EBP
            test EAX,EAX
            mov WORD PTR [EDX*2+(_KingBss+788080)],CX
            mov DWORD PTR [(_KingBss+986280)],ESI
            mov EBP,2
            je $L_62d740

            cmp ESI,DWORD PTR [(_KingBss+986288)]
            jae $L_62d740

            mov EBX,ECX
            sub EBX,EAX
            cmp EBX,32506
            ja $L_62d740

            push EAX
            call _Gz_LongestMatch

            mov EBP,EAX
            mov EAX,DWORD PTR [(_KingBss+986324)]
            add ESP,4
            cmp EBP,EAX
            jbe $L_62d70e

            mov EBP,EAX
$L_62d70e::

            cmp EBP,3
            jne $L_62d800

            mov ECX,DWORD PTR [(_KingBss+986284)]
            mov ESI,DWORD PTR [(_KingBss+986316)]
            mov EDX,ECX
            sub EDX,ESI
            cmp EDX,4096
            jbe $L_62d734

            mov EBP,2
$L_62d734::

            mov EDX,DWORD PTR [(_KingBss+986292)]
$L_62d73a::

            mov ESI,DWORD PTR [(_KingBss+986280)]
$L_62d740::

            cmp ESI,3
            jb $L_62d838

            cmp EBP,ESI
            ja $L_62d838

            sub ECX,EDI
            add ESI,-3
            dec ECX
            push ESI
            push ECX
            call _Gz_CtTally

            mov ESI,DWORD PTR [(_KingBss+986280)]
            mov ECX,DWORD PTR [(_KingBss+986324)]
            mov EDI,EAX
            mov EDX,DWORD PTR [(_KingBss+986292)]
            mov EAX,1
            add ESP,8
            sub EAX,ESI
            add ECX,EAX
            sub ESI,2
            mov DWORD PTR [(_KingBss+986324)],ECX
            mov ECX,DWORD PTR [(_KingBss+986284)]
$L_62d78d::

            inc ECX
            xor EAX,EAX
            and EDX,1023
            mov AL,BYTE PTR [ECX+(_KingBss+853618)]
            and EAX,32767
            shl EDX,5
            xor EDX,EAX
            mov EAX,ECX
            and EAX,32767
            dec ESI
            mov BX,WORD PTR [EDX*2+(_KingBss+788080)]
            mov WORD PTR [EAX*2+(_KingBss+722544)],BX
            mov WORD PTR [EDX*2+(_KingBss+788080)],CX
            jne $L_62d78d

            mov DWORD PTR [(_KingBss+986280)],ESI
            xor ESI,ESI
            inc ECX
            mov DWORD PTR [(_KingBss+986292)],EDX
            test EDI,EDI
            mov DWORD PTR [ESP+16],ESI
            mov EBP,2
            mov DWORD PTR [(_KingBss+986284)],ECX
            je $L_62d8c8

            mov EDX,DWORD PTR [(_KingBss+986320)]
            test EDX,EDX
            jl $L_62d811

            lea EAX,DWORD PTR [EDX+(_KingBss+853616)]
            jmp $L_62d813
$L_62d800::

            mov EDX,DWORD PTR [(_KingBss+986292)]
            mov ECX,DWORD PTR [(_KingBss+986284)]
            jmp $L_62d73a
$L_62d811::

            xor EAX,EAX
$L_62d813::

            sub ECX,EDX
            push 0
            push ECX
            push EAX
            call _Gz_FlushBlock

            mov ECX,DWORD PTR [(_KingBss+986284)]
            mov EDX,DWORD PTR [(_KingBss+986292)]
            add ESP,12
            mov DWORD PTR [(_KingBss+986320)],ECX
            jmp $L_62d8c8
$L_62d838::

            mov ESI,DWORD PTR [ESP+16]
            test ESI,ESI
            je $L_62d8ad

            xor EDX,EDX
            mov DL,BYTE PTR [ECX+(_KingBss+853615)]
            push EDX
            push 0
            call _Gz_CtTally

            add ESP,8
            test EAX,EAX
            je $L_62d88d

            mov ECX,DWORD PTR [(_KingBss+986320)]
            test ECX,ECX
            jl $L_62d869

            lea EAX,DWORD PTR [ECX+(_KingBss+853616)]
            jmp $L_62d86b
$L_62d869::

            xor EAX,EAX
$L_62d86b::

            mov EDX,DWORD PTR [(_KingBss+986284)]
            push 0
            sub EDX,ECX
            push EDX
            push EAX
            call _Gz_FlushBlock

            mov ECX,DWORD PTR [(_KingBss+986284)]
            add ESP,12
            mov DWORD PTR [(_KingBss+986320)],ECX
            jmp $L_62d893
$L_62d88d::

            mov ECX,DWORD PTR [(_KingBss+986284)]
$L_62d893::

            mov EAX,DWORD PTR [(_KingBss+986324)]
            mov EDX,DWORD PTR [(_KingBss+986292)]
            inc ECX
            dec EAX
            mov DWORD PTR [(_KingBss+986284)],ECX
            mov DWORD PTR [(_KingBss+986324)],EAX
            jmp $L_62d8c8
$L_62d8ad::

            mov EAX,DWORD PTR [(_KingBss+986324)]
            inc ECX
            mov ESI,1
            dec EAX
            mov DWORD PTR [ESP+16],ESI
            mov DWORD PTR [(_KingBss+986284)],ECX
            mov DWORD PTR [(_KingBss+986324)],EAX
$L_62d8c8::

            mov EAX,DWORD PTR [(_KingBss+986324)]
            cmp EAX,262
            jae $L_62d8fb
$L_62d8d4::

            mov EDI,DWORD PTR [(_KingBss+986296)]
            test EDI,EDI
            jne $L_62d8fb

            call _Gz_FillWindow

            mov EAX,DWORD PTR [(_KingBss+986324)]
            mov EDX,DWORD PTR [(_KingBss+986292)]
            mov ECX,DWORD PTR [(_KingBss+986284)]
            cmp EAX,262
            jb $L_62d8d4
$L_62d8fb::

            test EAX,EAX
            jne $L_62d68d

            pop EDI
            test ESI,ESI
            pop ESI
            pop EBX
            je $L_62d921

            mov AL,BYTE PTR [ECX+(_KingBss+853615)]
            push EAX
            push 0
            call _Gz_CtTally

            add ESP,8
$L_62d91b::

            mov ECX,DWORD PTR [(_KingBss+986284)]
$L_62d921::

            mov EDX,DWORD PTR [(_KingBss+986320)]
            test EDX,EDX
            jl $L_62d942

            sub ECX,EDX
            lea EAX,DWORD PTR [EDX+(_KingBss+853616)]
            push 1
            push ECX
            push EAX
            call _Gz_FlushBlock

            add ESP,12
            pop EBP
            pop ECX
            ret 
$L_62d942::

            sub ECX,EDX
            xor EAX,EAX
            push 1
            push ECX
            push EAX
            call _Gz_FlushBlock

            add ESP,12
            pop EBP
            pop ECX
            ret 
FUN_62d650 ENDP
ALIGN 16
FUN_62d960 PROC

            mov EAX,DWORD PTR [(_KingBss+986324)]
            push ESI
            xor ESI,ESI
            mov DWORD PTR [(_KingBss+986280)],2
            test EAX,EAX
            je $L_62db64

            mov EDX,DWORD PTR [(_KingBss+986292)]
            mov ECX,DWORD PTR [(_KingBss+986284)]
            push EBX
            push EDI
$L_62d988::

            xor EAX,EAX
            and EDX,1023
            mov AL,BYTE PTR [ECX+(_KingBss+853618)]
            mov EDI,ECX
            and EAX,32767
            and EDI,32767
            shl EDX,5
            xor EAX,EDX
            xor EDX,EDX
            mov DWORD PTR [(_KingBss+986292)],EAX
            mov DX,WORD PTR [EAX*2+(_KingBss+788080)]
            mov WORD PTR [EDI*2+(_KingBss+722544)],DX
            mov WORD PTR [EAX*2+(_KingBss+788080)],CX
            test EDX,EDX
            je $L_62d9f2

            mov EAX,ECX
            sub EAX,EDX
            cmp EAX,32506
            ja $L_62d9f2

            push EDX
            call _Gz_LongestMatch

            mov ESI,EAX
            mov EAX,DWORD PTR [(_KingBss+986324)]
            add ESP,4
            cmp ESI,EAX
            jbe $L_62d9ec

            mov ESI,EAX
$L_62d9ec::

            mov ECX,DWORD PTR [(_KingBss+986284)]
$L_62d9f2::

            cmp ESI,3
            jb $L_62dab8

            mov EBX,DWORD PTR [(_KingBss+986316)]
            lea EDX,DWORD PTR [ESI-3]
            sub ECX,EBX
            push EDX
            push ECX
            call _Gz_CtTally

            mov ECX,DWORD PTR [(_KingBss+986324)]
            mov EDI,EAX
            mov EAX,DWORD PTR [(_KingBss+986288)]
            add ESP,8
            sub ECX,ESI
            cmp ESI,EAX
            mov DWORD PTR [(_KingBss+986324)],ECX
            ja $L_62da80

            mov EDX,DWORD PTR [(_KingBss+986292)]
            mov ECX,DWORD PTR [(_KingBss+986284)]
            dec ESI
$L_62da36::

            inc ECX
            xor EAX,EAX
            and EDX,1023
            mov AL,BYTE PTR [ECX+(_KingBss+853618)]
            and EAX,32767
            shl EDX,5
            xor EDX,EAX
            mov EAX,ECX
            and EAX,32767
            dec ESI
            mov BX,WORD PTR [EDX*2+(_KingBss+788080)]
            mov WORD PTR [EAX*2+(_KingBss+722544)],BX
            mov WORD PTR [EDX*2+(_KingBss+788080)],CX
            jne $L_62da36

            inc ECX
            mov DWORD PTR [(_KingBss+986292)],EDX
            mov DWORD PTR [(_KingBss+986284)],ECX
            jmp $L_62daed
$L_62da80::

            mov ECX,DWORD PTR [(_KingBss+986284)]
            xor EDX,EDX
            add ECX,ESI
            xor EAX,EAX
            xor ESI,ESI
            mov DWORD PTR [(_KingBss+986284)],ECX
            mov DL,BYTE PTR [ECX+(_KingBss+853616)]
            mov AL,BYTE PTR [ECX+(_KingBss+853617)]
            and EDX,1023
            and EAX,32767
            shl EDX,5
            xor EDX,EAX
            mov DWORD PTR [(_KingBss+986292)],EDX
            jmp $L_62daed
$L_62dab8::

            xor EDX,EDX
            mov DL,BYTE PTR [ECX+(_KingBss+853616)]
            push EDX
            push 0
            call _Gz_CtTally

            mov ECX,DWORD PTR [(_KingBss+986324)]
            add ESP,8
            mov EDX,DWORD PTR [(_KingBss+986292)]
            dec ECX
            mov DWORD PTR [(_KingBss+986324)],ECX
            mov ECX,DWORD PTR [(_KingBss+986284)]
            inc ECX
            mov EDI,EAX
            mov DWORD PTR [(_KingBss+986284)],ECX
$L_62daed::

            test EDI,EDI
            je $L_62db25

            mov EDX,DWORD PTR [(_KingBss+986320)]
            test EDX,EDX
            jl $L_62db03

            lea EAX,DWORD PTR [EDX+(_KingBss+853616)]
            jmp $L_62db05
$L_62db03::

            xor EAX,EAX
$L_62db05::

            sub ECX,EDX
            push 0
            push ECX
            push EAX
            call _Gz_FlushBlock

            mov ECX,DWORD PTR [(_KingBss+986284)]
            mov EDX,DWORD PTR [(_KingBss+986292)]
            add ESP,12
            mov DWORD PTR [(_KingBss+986320)],ECX
$L_62db25::

            mov EAX,DWORD PTR [(_KingBss+986324)]
            cmp EAX,262
            jae $L_62db58
$L_62db31::

            mov EDI,DWORD PTR [(_KingBss+986296)]
            test EDI,EDI
            jne $L_62db58

            call _Gz_FillWindow

            mov EAX,DWORD PTR [(_KingBss+986324)]
            mov EDX,DWORD PTR [(_KingBss+986292)]
            mov ECX,DWORD PTR [(_KingBss+986284)]
            cmp EAX,262
            jb $L_62db31
$L_62db58::

            test EAX,EAX
            jne $L_62d988

            pop EDI
            pop EBX
            jmp $L_62db6a
$L_62db64::

            mov ECX,DWORD PTR [(_KingBss+986284)]
$L_62db6a::

            mov EDX,DWORD PTR [(_KingBss+986320)]
            pop ESI
            test EDX,EDX
            jl $L_62db8a

            sub ECX,EDX
            lea EAX,DWORD PTR [EDX+(_KingBss+853616)]
            push 1
            push ECX
            push EAX
            call _Gz_FlushBlock

            add ESP,12
            ret 
$L_62db8a::

            sub ECX,EDX
            xor EAX,EAX
            push 1
            push ECX
            push EAX
            call _Gz_FlushBlock

            add ESP,12
            ret 
FUN_62d960 ENDP
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
