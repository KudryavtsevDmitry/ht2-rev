; asm_grd.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_649e50:NEAR
EXTERN _KingBss:BYTE
EXTERN _KingRdata:BYTE
EXTERN _Mixer_CreateBuffer:PROC
EXTERN __CxxThrowException@8:PROC

PUBLIC $L_60537f, $L_6053b2, $L_6053ec, $L_605434

_TEXT SEGMENT
ALIGN 16
FUN_605310 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET $L_649e50
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,36
            push EBX
            mov EBX,ECX
            push ESI
            mov EDX,DWORD PTR [(_KingBss+629060)]
            mov EAX,DWORD PTR [EBX+8]
            push EDI
            mov DWORD PTR [EBP-16],ESP
            push EAX
            mov EAX,DWORD PTR [EBX+52]
            lea ESI,DWORD PTR [EBX+56]
            push ESI
            xor EDI,EDI
            lea ECX,DWORD PTR [EAX+EAX*2]
            mov DWORD PTR [EBP-48],EBX
            mov DWORD PTR [EBP-36],1
            mov DWORD PTR [EBP-4],EDI
            mov EAX,DWORD PTR [EDX+ECX*8+20]
            mov ECX,DWORD PTR [(_KingBss+629104)]
            push EAX
            push ECX
            call _Mixer_CreateBuffer

            add ESP,16
            cmp EAX,EDI
            jne $L_60537f

            lea EDX,DWORD PTR [EBP-40]
            push OFFSET (_KingRdata+119928)
            push EDX
            mov DWORD PTR [EBP-40],EDI
            call __CxxThrowException@8
$L_60537f::

            lea EDX,DWORD PTR [EBP-24]
            push EDI
            push EDX
            lea EDX,DWORD PTR [EBP-28]
            mov EAX,DWORD PTR [ESI]
            push EDX
            lea EDX,DWORD PTR [EBP-20]
            mov ECX,DWORD PTR [EAX]
            push EDX
            lea EDX,DWORD PTR [EBP-32]
            push EDX
            mov EDX,DWORD PTR [EBX+8]
            push EDX
            push EDI
            push EAX
            call DWORD PTR [ECX+44]

            cmp EAX,EDI
            je $L_6053b2

            lea EAX,DWORD PTR [EBP-44]
            push OFFSET (_KingRdata+119928)
            push EAX
            mov DWORD PTR [EBP-44],EDI
            call __CxxThrowException@8
$L_6053b2::

            mov ECX,DWORD PTR [EBP-20]
            mov ESI,DWORD PTR [EBX+4]
            mov EDI,DWORD PTR [EBP-32]
            mov EDX,ECX
            shr ECX,2

            rep movsd 

            mov ECX,EDX
            and ECX,3

            rep movsb 

            mov ECX,DWORD PTR [EBP-24]
            test ECX,ECX
            je $L_6053ec

            mov ESI,DWORD PTR [EBX+4]
            mov EDI,DWORD PTR [EBP-20]
            mov EAX,ECX
            add ESI,EDI
            mov EDI,DWORD PTR [EBP-28]
            shr ECX,2

            rep movsd 

            mov ECX,EAX
            and ECX,3

            rep movsb 

            mov ECX,DWORD PTR [EBP-24]
$L_6053ec::

            mov EAX,DWORD PTR [EBX+56]
            push ECX
            mov ECX,DWORD PTR [EBP-28]
            mov EDX,DWORD PTR [EAX]
            push ECX
            mov ECX,DWORD PTR [EBP-20]
            push ECX
            mov ECX,DWORD PTR [EBP-32]
            push ECX
            push EAX
            call DWORD PTR [EDX+76]

            mov EAX,DWORD PTR [EBX+56]
            mov ECX,DWORD PTR [EBX+64]
            push ECX
            push EAX
            mov EDX,DWORD PTR [EAX]
            call DWORD PTR [EDX+64]

            mov EAX,DWORD PTR [EBX+56]
            mov ECX,DWORD PTR [EBX+68]
            push ECX
            push EAX
            mov EDX,DWORD PTR [EAX]
            call DWORD PTR [EDX+60]

            mov ESI,DWORD PTR [EBX+40]
            mov EAX,DWORD PTR [EBX+56]
            xor ECX,ECX
            mov EDX,DWORD PTR [EAX]
            test ESI,ESI
            setne CL
            push ECX
            push 0
            push 0
            push EAX
            call DWORD PTR [EDX+48]
$L_605434::

            mov ECX,DWORD PTR [EBP-12]
            mov EAX,DWORD PTR [EBP-36]
            pop EDI
            pop ESI
            mov DWORD PTR FS:[0],ECX
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
FUN_605310 ENDP
$L_605448 PROC

            mov ECX,DWORD PTR [EBP-48]
            mov EDX,DWORD PTR [ECX]
            call DWORD PTR [EDX+24]

            mov DWORD PTR [EBP-36],0
            mov EAX,OFFSET $L_605434
            ret 
$L_605448 ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
$L_60572c PROC

            push EBP
            mov EBP,ESP
            mov ECX,DWORD PTR [EBP+8]
            rdtsc 
            mov DWORD PTR [ECX],EAX
            mov DWORD PTR [ECX+4],EDX
            leave 
            ret 
$L_60572c ENDP
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
_TEXT ENDS
END
