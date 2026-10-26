; asm_unit_603fd0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_649e10:NEAR
EXTERN @Wave_Free@8:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _Mixer_Unload:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN __imp__GetProcAddress@8:PROC
EXTERN __imp__LoadLibraryA@4:PROC

PUBLIC $L_60482c, $L_60485f, $L_604891, $L_604897, $L_6048c7, $L_6048eb, $L_604906, $L_604926
PUBLIC $L_60498e, $L_60499d, $L_6049bc, $L_604e00

_TEXT SEGMENT
FUN_6047d0 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_649e10
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,124
            mov ECX,DWORD PTR [(_KingBss+629116)]
            push EBX
            push ESI
            xor ESI,ESI
            push EDI
            cmp ECX,ESI
            mov DWORD PTR [EBP-16],ESP
            mov DWORD PTR [EBP-4],ESI
            jne $L_60482c

            push OFFSET (_KingData+100848)
            call DWORD PTR __imp__LoadLibraryA@4

            mov ECX,EAX
            cmp ECX,ESI
            mov DWORD PTR [(_KingBss+629116)],ECX
            jne $L_60482c

            lea EAX,DWORD PTR [EBP-20]
            push OFFSET (_KingRdata+119928)
            push EAX
            mov DWORD PTR [EBP-20],4
            call __CxxThrowException@8
$L_60482c::

            mov EAX,DWORD PTR [(_KingBss+629120)]
            cmp EAX,ESI
            jne $L_60485f

            push OFFSET (_KingData+100828)
            push ECX
            call DWORD PTR __imp__GetProcAddress@8

            cmp EAX,ESI
            mov DWORD PTR [(_KingBss+629120)],EAX
            jne $L_60485f

            lea ECX,DWORD PTR [EBP-24]
            push OFFSET (_KingRdata+119928)
            push ECX
            mov DWORD PTR [EBP-24],4
            call __CxxThrowException@8
$L_60485f::

            mov ECX,DWORD PTR [(_KingBss+629104)]
            cmp ECX,ESI
            jne $L_604897

            push ESI
            push OFFSET (_KingBss+629104)
            push ESI
            call EAX

            cmp EAX,ESI
            je $L_604891

            lea EDX,DWORD PTR [EBP-28]
            push OFFSET (_KingRdata+119928)
            push EDX
            mov DWORD PTR [(_KingBss+629104)],ESI
            mov DWORD PTR [EBP-28],4
            call __CxxThrowException@8
$L_604891::

            mov ECX,DWORD PTR [(_KingBss+629104)]
$L_604897::

            lea EDX,DWORD PTR [EBP-136]
            mov DWORD PTR [EBP-136],96
            mov EAX,DWORD PTR [ECX]
            push EDX
            push ECX
            call DWORD PTR [EAX+16]

            cmp EAX,ESI
            je $L_6048c7

            lea EAX,DWORD PTR [EBP-32]
            push OFFSET (_KingRdata+119928)
            push EAX
            mov DWORD PTR [EBP-32],4
            call __CxxThrowException@8
$L_6048c7::

            mov AL,BYTE PTR [EBP-132]
            test AL,8
            je $L_6048eb

            mov AX,16
            mov DWORD PTR [(_KingBss+629068)],1
            mov WORD PTR [(_KingBss+629022)],AX
            mov CX,2
            jmp $L_604926
$L_6048eb::

            mov ECX,2
            test CL,AL
            je $L_604906

            mov AX,8
            mov DWORD PTR [(_KingBss+629068)],ECX
            mov WORD PTR [(_KingBss+629022)],AX
            jmp $L_604926
$L_604906::

            test AL,1
            je $L_6049bc

            mov AX,8
            mov DWORD PTR [(_KingBss+629068)],3
            mov WORD PTR [(_KingBss+629022)],AX
            mov CX,1
$L_604926::

            shr AX,3
            imul AX,CX
            mov WORD PTR [(_KingBss+629020)],AX
            mov EAX,DWORD PTR [(_KingBss+629020)]
            and EAX,65535
            mov WORD PTR [(_KingBss+629010)],CX
            mov WORD PTR [(_KingBss+629008)],1
            mov DWORD PTR [(_KingBss+629012)],22050
            lea ECX,DWORD PTR [EAX+EAX*2]
            mov WORD PTR [(_KingBss+629024)],SI
            shl ECX,4
            add ECX,EAX
            mov DWORD PTR [EBP-4],4294967295
            lea EAX,DWORD PTR [ECX+ECX*4]
            lea EAX,DWORD PTR [EAX+EAX*4]
            lea ECX,DWORD PTR [EAX+EAX*8]
            mov EAX,DWORD PTR [(_KingBss+629108)]
            shl ECX,1
            cmp EAX,ESI
            mov DWORD PTR [(_KingBss+629016)],ECX
            je $L_60498e

            mov EDX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [EDX+8]
$L_60498e::

            mov EAX,DWORD PTR [(_KingBss+629104)]
            cmp EAX,ESI
            je $L_60499d

            mov ECX,DWORD PTR [EAX]
            push EAX
            call DWORD PTR [ECX+8]
$L_60499d::

            mov DWORD PTR [(_KingBss+629104)],ESI
            mov DWORD PTR [(_KingBss+629108)],ESI
            xor EAX,EAX
            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_6049bc::

            lea EDX,DWORD PTR [EBP-36]
            push OFFSET (_KingRdata+119928)
            push EDX
            mov DWORD PTR [EBP-36],4
            call __CxxThrowException@8
FUN_6047d0 ENDP
$L_6049d1 PROC

            call _Mixer_Unload

            mov EAX,OFFSET $L_6049dc
            ret 
$L_6049d1 ENDP
$L_6049dc PROC

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,DWORD PTR [EBP-40]
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_6049dc ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_604de0 PROC

            mov EAX,ECX
            xor ECX,ECX
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [EAX+4],ECX
            mov DWORD PTR [EAX+12],ECX
            mov DWORD PTR [EAX+8],ECX
            mov DWORD PTR [EAX+16],ECX
            mov DWORD PTR [EAX+20],ECX
            ret 

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
$L_604e00::

            jmp @Wave_Free@8
$L_604de0 ENDP
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
