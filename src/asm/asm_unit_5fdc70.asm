; asm_unit_5fdc70.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE

PUBLIC $L_5fdc76, $L_5fdc87, $L_5fdcaf, $L_5fdcde, $L_5fdce6, $L_5fdd0b, $L_5fdd19, $L_5fdd3a
PUBLIC $L_5fdd55, $L_5fdd68, $L_5fdd93, $L_5fdda1, $L_5fddd4, $L_5fddef, $L_5fde1d, $L_5fde44
PUBLIC $L_5fde50, $L_5fde56, $L_5fde7a, $L_5fde9b, $L_5fdea1, $L_5fded8, $L_5fdef9, $L_5fdf04
PUBLIC $L_5fdf39, $L_5fdf44, $L_5fdf73, $L_5fdf7f, $L_5fdf92, $L_5fdfc3, $L_5fdfeb, $L_5fdffe
PUBLIC $L_5fe042, $L_5fe06a, $L_5fe075, $L_5fe0c1, $L_5fe0cc, $L_5fe111, $L_5fe11d, $L_5fe12f
PUBLIC $L_5fe185, $L_5fe1ac, $L_5fe1be, $L_5fe218, $L_5fe219, $L_5fe244, $L_5fe24a, $L_5fe25a
PUBLIC $L_5fe26d, $L_5fe27b, $L_5fe290, $L_5fe2a3, $L_5fe2a9, $L_5fe2ca, $L_5fe2d6, $L_5fe2f5
PUBLIC $L_5fe30e, $L_5fe31f, $L_5fe343, $L_5fe34f, $L_5fe381, $L_5fe39a, $L_5fe3c2, $L_5fe3e4
PUBLIC $L_5fe3ee, $L_5fe3f4, $L_5fe414, $L_5fe433, $L_5fe439, $L_5fe46b, $L_5fe48a, $L_5fe490
PUBLIC $L_5fe4bd, $L_5fe4c3, $L_5fe4ea, $L_5fe4f4, $L_5fe4fa, $L_5fe52d, $L_5fe54c, $L_5fe552
PUBLIC $L_5fe597, $L_5fe5b6, $L_5fe5c1, $L_5fe603, $L_5fe60e, $L_5fe64a, $L_5fe654, $L_5fe65a
PUBLIC $L_5fe6a9, $L_5fe6af, $L_5fe6f8, $L_5fe702, $L_5fe714, $L_5fe764, $L_5fe789, $L_5fe79b
PUBLIC $L_5fe7ee, $L_5fe7ef
PUBLIC $L_5febe1, $L_5febeb
PUBLIC $L_5febf5, $L_5febff, $L_5fec09, $L_5fec13, $L_5fec1d, $L_5fec31, $L_5fec3b, $L_5fec7e
PUBLIC $L_5fec88, $L_5fec92, $L_5fec9c, $L_5feca6, $L_5fecb0, $L_5fecf7, $L_5fed01, $L_5fed0b
PUBLIC $L_5fed15, $L_5fed1f, $L_5fed29, $L_5fed5a, $L_5fed61, $L_5fed68, $L_5fed6f, $L_5fed76
PUBLIC $L_5fed7d, $L_5feda9, $L_5fedab, $L_5fedad, $L_5fedaf, $L_5fedb1, $L_5fede1, $L_5fedeb
PUBLIC $L_5fedf5, $L_5fee0e, $L_5fee15, $L_5fee1c, $L_5fee23, $L_5fee37, $L_5fee3e, $L_5fee45
PUBLIC $L_5fee5e, $L_5fee65, $L_5fee6c, $L_5fee73, $L_5fee92, $L_5fee9d, $L_5fef1a, $L_5fef80
PUBLIC $L_5fef8d, $L_5fefb6, $L_5fefcb, $L_5fefe6, $L_5feff2, $L_5ff003, $L_5ff016, $L_5ff02f
PUBLIC $L_5ff04c, $L_5ff05a, $L_5ff060, $L_5ff070, $L_5ff093, $L_5ff0c0, $L_5ff0c7

_TEXT SEGMENT
$L_5fdc70 PROC

            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fdc76::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fdc87

            mov WORD PTR [EDI],AX
            mov WORD PTR [EBP],SI
$L_5fdc87::

            add EDI,2
            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdc76

            ret 
$L_5fdc70 ENDP
$L_5fdc9c PROC

            mov EBP,DWORD PTR [(_KingData+97358)]
            shl EAX,16
            mov EBX,DWORD PTR [(_KingBss+606160)]
            add EBX,EAX
            xor EAX,EAX
$L_5fdcaf::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fdc87

            mov AX,WORD PTR [EDI]
            shl EAX,1
            mov AX,WORD PTR [EAX+EBX*1]
            mov WORD PTR [EDI],AX
            mov WORD PTR [EBP],SI
            add EDI,2
            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdcaf

            ret 
$L_5fdcde::

            mov EBX,DWORD PTR [(_KingBss+543680)]
            shl EAX,1
$L_5fdce6::

            xor ESI,ESI
            shld ESI,ECX,12
            shl ESI,2
            mov ESI,DWORD PTR [EBX+ESI*1]
            add ECX,DWORD PTR [(_KingData+97340)]
            mov DX,WORD PTR [ESI+EAX*1]
            mov WORD PTR [EDI],DX
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdce6

            ret 
$L_5fdd0b::

            mov EBX,DWORD PTR [(_KingBss+543680)]
            mov EBP,DWORD PTR [(_KingData+97358)]
            shl EAX,1
$L_5fdd19::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fdd3a

            mov WORD PTR [EBP],SI
            xor ESI,ESI
            shld ESI,ECX,12
            shl ESI,2
            mov ESI,DWORD PTR [EBX+ESI*1]
            mov DX,WORD PTR [ESI+EAX*1]
            mov WORD PTR [EDI],DX
$L_5fdd3a::

            add ECX,DWORD PTR [(_KingData+97340)]
            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdd19

            ret 
$L_5fdd55::

            mov EBX,DWORD PTR [(_KingBss+543680)]
            mov EBP,DWORD PTR [(_KingBss+606096)]
            mov EDX,0
            shl EAX,1
$L_5fdd68::

            xor ESI,ESI
            shld ESI,ECX,12
            shl ESI,2
            mov ESI,DWORD PTR [EBX+ESI*1]
            add ECX,DWORD PTR [(_KingData+97340)]
            mov DX,WORD PTR [ESI+EAX*1]
            shl EDX,1
            mov DX,WORD PTR [EDX+EBP*1]
            mov WORD PTR [EDI],DX
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdd68

            ret 
$L_5fdd93::

            mov EBX,DWORD PTR [(_KingBss+543680)]
            shl EAX,1
            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fdda1::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fddd4

            mov WORD PTR [EBP],SI
            xor ESI,ESI
            shld ESI,ECX,12
            shl ESI,2
            mov ESI,DWORD PTR [EBX+ESI*1]
            mov SI,WORD PTR [ESI+EAX*1]
            shl SI,1
            and ESI,255
            add ESI,DWORD PTR [(_KingBss+606096)]
            mov SI,WORD PTR [ESI]
            mov WORD PTR [EDI],SI
$L_5fddd4::

            add ECX,DWORD PTR [(_KingData+97340)]
            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdda1

            ret 
$L_5fddef::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DX,WORD PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            mov WORD PTR [EDI],DX
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fddef

            ret 
$L_5fde1d::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DX,WORD PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            test DX,DX
            je $L_5fde44

            mov WORD PTR [EDI],DX
$L_5fde44::

            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fde1d

            ret 
$L_5fde50::

            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fde56::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fde7a

            mov WORD PTR [EBP],SI
            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov SI,WORD PTR [ESI]
            mov WORD PTR [EDI],SI
$L_5fde7a::

            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fde56

            ret 
$L_5fde9b::

            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fdea1::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fded8

            mov WORD PTR [(_KingData+97308)],SI
            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov SI,WORD PTR [ESI]
            test SI,SI
            je $L_5fded8

            mov WORD PTR [EDI],SI
            mov SI,WORD PTR [(_KingData+97308)]
            mov WORD PTR [EBP],SI
$L_5fded8::

            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdea1

            ret 
$L_5fdef9::

            mov EBP,DWORD PTR [(_KingBss+606096)]
            mov EDX,0
$L_5fdf04::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DX,WORD PTR [ESI]
            shl EDX,1
            add EBX,DWORD PTR [(_KingData+97336)]
            mov DX,WORD PTR [EBP+EDX*1]
            mov WORD PTR [EDI],DX
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdf04

            ret 
$L_5fdf39::

            mov EBP,DWORD PTR [(_KingBss+606096)]
            mov EDX,0
$L_5fdf44::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DX,WORD PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            test DX,DX
            je $L_5fdf73

            shl DX,1
            mov DX,WORD PTR [EBP+EDX*1]
            mov WORD PTR [EDI],DX
$L_5fdf73::

            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdf44

            ret 
$L_5fdf7f::

            cli 
            mov DWORD PTR [(_KingData+97296)],ESP
            mov EBP,DWORD PTR [(_KingData+97358)]
            mov ESP,DWORD PTR [(_KingBss+606096)]
$L_5fdf92::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fdfc3

            mov WORD PTR [EBP],SI
            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov SI,WORD PTR [ESI]
            shl SI,1
            and ESI,65535
            mov SI,WORD PTR [ESP+ESI*1]
            mov WORD PTR [EDI],SI
$L_5fdfc3::

            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdf92

            mov ESP,DWORD PTR [(_KingData+97296)]
            sti 
            ret 
$L_5fdfeb::

            cli 
            mov EBP,DWORD PTR [(_KingData+97358)]
            mov DWORD PTR [(_KingData+97296)],ESP
            mov ESP,DWORD PTR [(_KingBss+606096)]
$L_5fdffe::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fe042

            mov WORD PTR [(_KingData+97308)],SI
            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov SI,WORD PTR [ESI]
            test SI,SI
            je $L_5fe042

            shl SI,1
            and ESI,65535
            mov SI,WORD PTR [ESP+ESI*1]
            mov WORD PTR [EDI],SI
            mov SI,WORD PTR [(_KingData+97308)]
            mov WORD PTR [EBP],SI
$L_5fe042::

            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fdffe

            mov ESP,DWORD PTR [(_KingData+97296)]
            sti 
            ret 
$L_5fe06a::

            mov EBP,DWORD PTR [(_KingData+97348)]
            mov EDX,0
$L_5fe075::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DX,WORD PTR [ESI]
            shl DX,1
            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            mov DX,WORD PTR [ESI+EDX*1]
            add EBX,DWORD PTR [(_KingData+97336)]
            mov WORD PTR [EDI],DX
            add EBP,DWORD PTR [(_KingData+97340)]
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe075

            ret 
$L_5fe0c1::

            mov EBP,DWORD PTR [(_KingData+97348)]
            mov EDX,0
$L_5fe0cc::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DX,WORD PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            test DX,DX
            je $L_5fe111

            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            shl DX,1
            mov DX,WORD PTR [ESI+EDX*1]
            add EBP,DWORD PTR [(_KingData+97340)]
            mov WORD PTR [EDI],DX
$L_5fe111::

            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe0cc

            ret 
$L_5fe11d::

            mov DWORD PTR [(_KingData+97304)],ECX
            mov EBP,DWORD PTR [(_KingData+97348)]
            mov ECX,DWORD PTR [(_KingData+97358)]
$L_5fe12f::

            shld ESI,EDX,16
            cmp SI,WORD PTR [ECX]
            jb $L_5fe185

            mov WORD PTR [ECX],SI
            push ECX
            xor ESI,ESI
            mov ECX,DWORD PTR [(_KingData+97304)]
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov CX,WORD PTR [ESI]
            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            mov DWORD PTR [(_KingData+97300)],ESI
            mov SI,CX
            shl SI,1
            and ESI,255
            add ESI,DWORD PTR [(_KingData+97300)]
            mov CX,WORD PTR [ESI]
            mov WORD PTR [EDI],CX
            pop ECX
$L_5fe185::

            add EDX,DWORD PTR [(_KingData+97344)]
            add ECX,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            add EBP,DWORD PTR [(_KingData+97340)]
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe12f

            ret 
$L_5fe1ac::

            mov DWORD PTR [(_KingData+97304)],ECX
            mov EBP,DWORD PTR [(_KingData+97348)]
            mov ECX,DWORD PTR [(_KingData+97358)]
$L_5fe1be::

            shld ESI,EDX,16
            cmp SI,WORD PTR [ECX]
            jb $L_5fe219

            mov WORD PTR [ECX],SI
            push ECX
            xor ESI,ESI
            mov ECX,DWORD PTR [(_KingData+97304)]
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            shl ESI,1
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov CX,WORD PTR [ESI]
            test CX,CX
            je $L_5fe218

            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            mov DWORD PTR [(_KingData+97300)],ESI
            mov SI,CX
            shl SI,1
            and ESI,255
            add ESI,DWORD PTR [(_KingData+97300)]
            mov CX,WORD PTR [ESI]
            mov WORD PTR [EDI],CX
$L_5fe218::

            pop ECX
$L_5fe219::

            add EDX,DWORD PTR [(_KingData+97344)]
            add ECX,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            add EBP,DWORD PTR [(_KingData+97340)]
            add EDI,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe1be

            ret 
$L_5fe244::

            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fe24a::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fe25a

            mov BYTE PTR [EDI],AL
            mov WORD PTR [EBP],SI
$L_5fe25a::

            inc EDI
            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe24a

            ret 
$L_5fe26d::

            mov EBP,DWORD PTR [(_KingData+97358)]
            mov AH,AL
            mov EBX,DWORD PTR [(_KingBss+606160)]
$L_5fe27b::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fe290

            mov AL,BYTE PTR [EDI]
            mov AL,BYTE PTR [EAX+EBX*1]
            mov BYTE PTR [EDI],AL
            mov WORD PTR [EBP],SI
$L_5fe290::

            inc EDI
            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe27b

            ret 
$L_5fe2a3::

            mov EBX,DWORD PTR [(_KingBss+543680)]
$L_5fe2a9::

            xor ESI,ESI
            shld ESI,ECX,12
            shl ESI,2
            mov ESI,DWORD PTR [EBX+ESI*1]
            add ECX,DWORD PTR [(_KingData+97340)]
            mov DL,BYTE PTR [ESI+EAX*1]
            mov BYTE PTR [EDI],DL
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe2a9

            ret 
$L_5fe2ca::

            mov EBX,DWORD PTR [(_KingBss+543680)]
            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fe2d6::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fe2f5

            mov WORD PTR [EBP],SI
            xor ESI,ESI
            shld ESI,ECX,12
            shl ESI,2
            mov ESI,DWORD PTR [EBX+ESI*1]
            mov DL,BYTE PTR [ESI+EAX*1]
            mov BYTE PTR [EDI],DL
$L_5fe2f5::

            add ECX,DWORD PTR [(_KingData+97340)]
            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe2d6

            ret 
$L_5fe30e::

            mov EBX,DWORD PTR [(_KingBss+543680)]
            mov EBP,DWORD PTR [(_KingBss+606096)]
            mov EDX,0
$L_5fe31f::

            xor ESI,ESI
            shld ESI,ECX,12
            shl ESI,2
            mov ESI,DWORD PTR [EBX+ESI*1]
            add ECX,DWORD PTR [(_KingData+97340)]
            mov DL,BYTE PTR [ESI+EAX*1]
            mov DL,BYTE PTR [EDX+EBP*1]
            mov BYTE PTR [EDI],DL
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe31f

            ret 
$L_5fe343::

            mov EBX,DWORD PTR [(_KingBss+543680)]
            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fe34f::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fe381

            mov WORD PTR [EBP],SI
            xor ESI,ESI
            shld ESI,ECX,12
            shl ESI,2
            mov ESI,DWORD PTR [EBX+ESI*1]
            mov DL,BYTE PTR [ESI+EAX*1]
            shld SI,DX,16
            and ESI,255
            add ESI,DWORD PTR [(_KingBss+606096)]
            mov DL,BYTE PTR [ESI]
            mov BYTE PTR [EDI],DL
$L_5fe381::

            add ECX,DWORD PTR [(_KingData+97340)]
            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe34f

            ret 
$L_5fe39a::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DL,BYTE PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            mov BYTE PTR [EDI],DL
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe39a

            ret 
$L_5fe3c2::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DL,BYTE PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            test DL,DL
            je $L_5fe3e4

            mov BYTE PTR [EDI],DL
$L_5fe3e4::

            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe3c2

            ret 
$L_5fe3ee::

            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fe3f4::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fe414

            mov WORD PTR [EBP],SI
            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov CH,BYTE PTR [ESI]
            mov BYTE PTR [EDI],CH
$L_5fe414::

            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe3f4

            ret 
$L_5fe433::

            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fe439::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fe46b

            mov WORD PTR [(_KingData+97322)],SI
            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov CH,BYTE PTR [ESI]
            test CH,CH
            je $L_5fe46b

            mov SI,WORD PTR [(_KingData+97322)]
            mov WORD PTR [EBP],SI
            mov BYTE PTR [EDI],CH
$L_5fe46b::

            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe439

            ret 
$L_5fe48a::

            mov EBP,DWORD PTR [(_KingBss+606096)]
$L_5fe490::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            movzx EDX,BYTE PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            mov DL,BYTE PTR [EBP+EDX*1]
            mov BYTE PTR [EDI],DL
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe490

            ret 
$L_5fe4bd::

            mov EBP,DWORD PTR [(_KingBss+606096)]
$L_5fe4c3::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            movzx EDX,BYTE PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            test DL,DL
            je $L_5fe4ea

            mov DL,BYTE PTR [EBP+EDX*1]
            mov BYTE PTR [EDI],DL
$L_5fe4ea::

            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe4c3

            ret 
$L_5fe4f4::

            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fe4fa::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fe52d

            mov WORD PTR [EBP],SI
            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov CH,BYTE PTR [ESI]
            shld SI,CX,8
            and ESI,255
            add ESI,DWORD PTR [(_KingBss+606096)]
            mov CH,BYTE PTR [ESI]
            mov BYTE PTR [EDI],CH
$L_5fe52d::

            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe4fa

            ret 
$L_5fe54c::

            mov EBP,DWORD PTR [(_KingData+97358)]
$L_5fe552::

            shld ESI,EDX,16
            cmp SI,WORD PTR [EBP]
            jb $L_5fe597

            mov WORD PTR [(_KingData+97322)],SI
            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov CH,BYTE PTR [ESI]
            test CH,CH
            je $L_5fe597

            shld SI,CX,8
            and ESI,255
            add ESI,DWORD PTR [(_KingBss+606096)]
            mov CH,BYTE PTR [ESI]
            mov SI,WORD PTR [(_KingData+97322)]
            mov WORD PTR [EBP],SI
            mov BYTE PTR [EDI],CH
$L_5fe597::

            add EDX,DWORD PTR [(_KingData+97344)]
            add EBP,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe552

            ret 
$L_5fe5b6::

            mov EBP,DWORD PTR [(_KingData+97348)]
            mov EDX,0
$L_5fe5c1::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DL,BYTE PTR [ESI]
            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            mov DL,BYTE PTR [ESI+EDX*1]
            add EBX,DWORD PTR [(_KingData+97336)]
            mov BYTE PTR [EDI],DL
            add EBP,DWORD PTR [(_KingData+97340)]
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe5c1

            ret 
$L_5fe603::

            mov EBP,DWORD PTR [(_KingData+97348)]
            mov EDX,0
$L_5fe60e::

            xor ESI,ESI
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DL,BYTE PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            test DL,DL
            je $L_5fe64a

            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            mov DL,BYTE PTR [ESI+EDX*1]
            add EBP,DWORD PTR [(_KingData+97340)]
            mov BYTE PTR [EDI],DL
$L_5fe64a::

            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe60e

            ret 
$L_5fe654::

            mov EBP,DWORD PTR [(_KingData+97348)]
$L_5fe65a::

            xor ESI,ESI
            mov EDX,0
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DL,BYTE PTR [ESI]
            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            mov DL,BYTE PTR [ESI+EDX*1]
            add EDX,DWORD PTR [(_KingBss+606096)]
            mov DL,BYTE PTR [EDX]
            add EBX,DWORD PTR [(_KingData+97336)]
            mov BYTE PTR [EDI],DL
            add EBP,DWORD PTR [(_KingData+97340)]
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe65a

            ret 
$L_5fe6a9::

            mov EBP,DWORD PTR [(_KingData+97348)]
$L_5fe6af::

            xor ESI,ESI
            mov EDX,0
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            add EAX,DWORD PTR [(_KingData+97332)]
            mov DL,BYTE PTR [ESI]
            add EBX,DWORD PTR [(_KingData+97336)]
            test DL,DL
            je $L_5fe6f8

            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            mov DL,BYTE PTR [ESI+EDX*1]
            add EDX,DWORD PTR [(_KingBss+606096)]
            mov DL,BYTE PTR [EDX]
            add EBP,DWORD PTR [(_KingData+97340)]
            mov BYTE PTR [EDI],DL
$L_5fe6f8::

            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe6af

            ret 
$L_5fe702::

            mov DWORD PTR [(_KingData+97318)],ECX
            mov EBP,DWORD PTR [(_KingData+97348)]
            mov ECX,DWORD PTR [(_KingData+97358)]
$L_5fe714::

            shld ESI,EDX,16
            cmp SI,WORD PTR [ECX]
            jb $L_5fe764

            mov WORD PTR [ECX],SI
            push ECX
            xor ESI,ESI
            mov ECX,DWORD PTR [(_KingData+97318)]
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov CH,BYTE PTR [ESI]
            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            mov DWORD PTR [(_KingData+97314)],ESI
            shld SI,CX,8
            and ESI,255
            add ESI,DWORD PTR [(_KingData+97314)]
            mov CH,BYTE PTR [ESI]
            mov BYTE PTR [EDI],CH
            pop ECX
$L_5fe764::

            add EDX,DWORD PTR [(_KingData+97344)]
            add ECX,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            add EBP,DWORD PTR [(_KingData+97340)]
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe714

            ret 
$L_5fe789::

            mov DWORD PTR [(_KingData+97318)],ECX
            mov EBP,DWORD PTR [(_KingData+97348)]
            mov ECX,DWORD PTR [(_KingData+97358)]
$L_5fe79b::

            shld ESI,EDX,16
            cmp SI,WORD PTR [ECX]
            jb $L_5fe7ef

            mov WORD PTR [ECX],SI
            push ECX
            xor ESI,ESI
            mov ECX,DWORD PTR [(_KingData+97318)]
            shld ESI,EBX,CL
            shld ESI,EAX,CL
            add ESI,DWORD PTR [(_KingBss+606120)]
            mov CH,BYTE PTR [ESI]
            test CH,CH
            je $L_5fe7ee

            xor ESI,ESI
            shld ESI,EBP,12
            shl ESI,2
            add ESI,DWORD PTR [(_KingBss+543680)]
            mov ESI,DWORD PTR [ESI]
            mov DWORD PTR [(_KingData+97314)],ESI
            shld SI,CX,8
            and ESI,255
            add ESI,DWORD PTR [(_KingData+97314)]
            mov CH,BYTE PTR [ESI]
            mov BYTE PTR [EDI],CH
$L_5fe7ee::

            pop ECX
$L_5fe7ef::

            add EDX,DWORD PTR [(_KingData+97344)]
            add ECX,2
            add EAX,DWORD PTR [(_KingData+97332)]
            add EBX,DWORD PTR [(_KingData+97336)]
            add EBP,DWORD PTR [(_KingData+97340)]
            inc EDI
            cmp EDI,DWORD PTR [(_KingData+97354)]
            jl $L_5fe79b

            ret 
$L_5fdc9c ENDP
ALIGN 4
ALIGN 16
ALIGN 4
ALIGN 2
ALIGN 4
ALIGN 2
ALIGN 2
ALIGN 2
FUN_5feb87 PROC

            enter 128,0
            pushad 
            cmp DWORD PTR [(_KingBss+543672)],1
            jg $L_5fecb0

            cmp DWORD PTR [EBP+12],0
            jg $L_5fec4a

            cmp DWORD PTR [EBP+8],6
            jg $L_5febff

            cmp DWORD PTR [EBP+8],5
            jg $L_5febeb

            cmp DWORD PTR [EBP+8],4
            jg $L_5fec09

            cmp DWORD PTR [EBP+8],3
            jg $L_5febf5

            cmp DWORD PTR [EBP+8],2
            jg $L_5fec3b

            cmp DWORD PTR [EBP+8],1
            jg $L_5fec1d

            cmp DWORD PTR [EBP+8],0
            jg $L_5fec13

            cmp WORD PTR [(_KingData+97352)],0
            je $L_5febe1

            mov ECX,OFFSET $L_5fe39a
            jmp $L_5fedb3
$L_5febe1::

            mov ECX,OFFSET $L_5fe39a
            jmp $L_5fedb3
$L_5febeb::

            mov ECX,OFFSET $L_5fe654
            jmp $L_5fedb3
$L_5febf5::

            mov ECX,OFFSET $L_5fe48a
            jmp $L_5fedb3
$L_5febff::

            mov ECX,OFFSET $L_5fe6a9
            jmp $L_5fedb3
$L_5fec09::

            mov ECX,OFFSET $L_5fe4bd
            jmp $L_5fedb3
$L_5fec13::

            mov ECX,OFFSET $L_5fe3c2
            jmp $L_5fedb3
$L_5fec1d::

            cmp WORD PTR [(_KingData+97352)],0
            je $L_5fec31

            mov ECX,OFFSET $L_5fe5b6
            jmp $L_5fedb3
$L_5fec31::

            mov ECX,OFFSET $L_5fe5b6
            jmp $L_5fedb3
$L_5fec3b::

            mov ECX,OFFSET $L_5fe603
            jmp $L_5fedb3
FUN_5feb87 ENDP
$L_5fec4a PROC

            cmp DWORD PTR [EBP+8],6
            jg $L_5fec88

            cmp DWORD PTR [EBP+8],5
            jg $L_5fec7e

            cmp DWORD PTR [EBP+8],4
            jg $L_5fec88

            cmp DWORD PTR [EBP+8],3
            jg $L_5fec7e

            cmp DWORD PTR [EBP+8],2
            jg $L_5feca6

            cmp DWORD PTR [EBP+8],1
            jg $L_5fec9c

            cmp DWORD PTR [EBP+8],0
            jg $L_5fec92

            mov ECX,OFFSET $L_5fe3ee
            jmp $L_5fedb3
$L_5fec7e::

            mov ECX,OFFSET $L_5fe4f4
            jmp $L_5fedb3
$L_5fec88::

            mov ECX,OFFSET $L_5fe54c
            jmp $L_5fedb3
$L_5fec92::

            mov ECX,OFFSET $L_5fe433
            jmp $L_5fedb3
$L_5fec9c::

            mov ECX,OFFSET $L_5fe702
            jmp $L_5fedb3
$L_5feca6::

            mov ECX,OFFSET $L_5fe789
            jmp $L_5fedb3
$L_5fecb0::

            cmp DWORD PTR [(_KingBss+543672)],2
            jg $L_5fed7d

            cmp DWORD PTR [EBP+12],0
            jg $L_5fed29

            cmp DWORD PTR [EBP+8],6
            jg $L_5fed01

            cmp DWORD PTR [EBP+8],5
            jg $L_5fecf7

            cmp DWORD PTR [EBP+8],4
            jg $L_5fed01

            cmp DWORD PTR [EBP+8],3
            jg $L_5fecf7

            cmp DWORD PTR [EBP+8],2
            jg $L_5fed1f

            cmp DWORD PTR [EBP+8],1
            jg $L_5fed15

            cmp DWORD PTR [EBP+8],0
            jg $L_5fed0b

            mov ECX,OFFSET $L_5fddef
            jmp $L_5fedb3
$L_5fecf7::

            mov ECX,OFFSET $L_5fdef9
            jmp $L_5fedb3
$L_5fed01::

            mov ECX,OFFSET $L_5fdf39
            jmp $L_5fedb3
$L_5fed0b::

            mov ECX,OFFSET $L_5fde1d
            jmp $L_5fedb3
$L_5fed15::

            mov ECX,OFFSET $L_5fe06a
            jmp $L_5fedb3
$L_5fed1f::

            mov ECX,OFFSET $L_5fe0c1
            jmp $L_5fedb3
$L_5fed29::

            cmp DWORD PTR [EBP+8],6
            jg $L_5fed61

            cmp DWORD PTR [EBP+8],5
            jg $L_5fed5a

            cmp DWORD PTR [EBP+8],4
            jg $L_5fed61

            cmp DWORD PTR [EBP+8],3
            jg $L_5fed5a

            cmp DWORD PTR [EBP+8],2
            jg $L_5fed76

            cmp DWORD PTR [EBP+8],1
            jg $L_5fed6f

            cmp DWORD PTR [EBP+8],0
            jg $L_5fed68

            mov ECX,OFFSET $L_5fde50
            jmp $L_5fedb3
$L_5fed5a::

            mov ECX,OFFSET $L_5fdf7f
            jmp $L_5fedb3
$L_5fed61::

            mov ECX,OFFSET $L_5fdfeb
            jmp $L_5fedb3
$L_5fed68::

            mov ECX,OFFSET $L_5fde9b
            jmp $L_5fedb3
$L_5fed6f::

            mov ECX,OFFSET $L_5fe11d
            jmp $L_5fedb3
$L_5fed76::

            mov ECX,OFFSET $L_5fe1ac
            jmp $L_5fedb3
$L_5fed7d::

            cmp DWORD PTR [EBP+8],6
            jg $L_5fedab

            cmp DWORD PTR [EBP+8],5
            jg $L_5feda9

            cmp DWORD PTR [EBP+8],4
            jg $L_5fedab

            cmp DWORD PTR [EBP+8],3
            jg $L_5feda9

            cmp DWORD PTR [EBP+8],2
            jg $L_5fedb1

            cmp DWORD PTR [EBP+8],1
            jg $L_5fedaf

            cmp DWORD PTR [EBP+8],0
            jg $L_5fedad

            jmp $L_5fedb3
$L_5feda9::

            jmp $L_5fedb3
$L_5fedab::

            jmp $L_5fedb3
$L_5fedad::

            jmp $L_5fedb3
$L_5fedaf::

            jmp $L_5fedb3
$L_5fedb1::

            jmp $L_5fedb3
$L_5fec4a ENDP
$L_5fedb3 PROC

            mov DWORD PTR [(_KingData+97362)],ECX
            popad 
            leave 
            ret 
$L_5fedb3 ENDP
ALIGN 4
FUN_5fedbc PROC

            enter 128,0
            pushad 
            cmp DWORD PTR [(_KingBss+543672)],1
            jg $L_5fee23

            cmp DWORD PTR [EBP+12],0
            jg $L_5fedf5

            cmp DWORD PTR [EBP+8],5
            jg $L_5fedeb

            cmp DWORD PTR [EBP+8],1
            jg $L_5fede1

            jmp $L_5fee73
$L_5fede1::

            mov ECX,OFFSET $L_5fe2a3
            jmp $L_5fee73
$L_5fedeb::

            mov ECX,OFFSET $L_5fe30e
            jmp $L_5fee73
$L_5fedf5::

            cmp DWORD PTR [EBP+8],5
            jg $L_5fee1c

            cmp DWORD PTR [EBP+8],1
            jg $L_5fee15

            cmp DWORD PTR [EBP+8],0
            jg $L_5fee0e

            mov ECX,OFFSET $L_5fe244
            jmp $L_5fee73
$L_5fee0e::

            mov ECX,OFFSET $L_5fe26d
            jmp $L_5fee73
$L_5fee15::

            mov ECX,OFFSET $L_5fe2ca
            jmp $L_5fee73
$L_5fee1c::

            mov ECX,OFFSET $L_5fe343
            jmp $L_5fee73
$L_5fee23::

            cmp DWORD PTR [EBP+12],0
            jg $L_5fee45

            cmp DWORD PTR [EBP+8],5
            jg $L_5fee3e

            cmp DWORD PTR [EBP+8],1
            jg $L_5fee37

            jmp $L_5fee73
$L_5fee37::

            mov ECX,OFFSET $L_5fdcde
            jmp $L_5fee73
$L_5fee3e::

            mov ECX,OFFSET $L_5fdd55
            jmp $L_5fee73
$L_5fee45::

            cmp DWORD PTR [EBP+8],5
            jg $L_5fee6c

            cmp DWORD PTR [EBP+8],1
            jg $L_5fee65

            cmp DWORD PTR [EBP+8],0
            jg $L_5fee5e

            mov ECX,OFFSET $L_5fdc70
            jmp $L_5fee73
$L_5fee5e::

            mov ECX,OFFSET $L_5fdc9c
            jmp $L_5fee73
$L_5fee65::

            mov ECX,OFFSET $L_5fdd0b
            jmp $L_5fee73
$L_5fee6c::

            mov ECX,OFFSET $L_5fdd93
            jmp $L_5fee73
$L_5fee73::

            mov DWORD PTR [(_KingData+97362)],ECX
            popad 
            leave 
            ret 
FUN_5fedbc ENDP
ALIGN 4
FUN_5fee7c PROC

            enter 128,0
            pushad 
            mov EDI,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EBP+12]
            cmp DWORD PTR [(_KingBss+543672)],4
            jl $L_5fee92

            shl EAX,1
$L_5fee92::

            cmp DWORD PTR [(_KingBss+543672)],2
            jl $L_5fee9d

            shl EAX,1
$L_5fee9d::

            add EAX,EDI
            mov DWORD PTR [(_KingData+97354)],EAX
            mov EAX,DWORD PTR [EBP+40]
            mov DWORD PTR [(_KingData+97358)],EAX
            mov EAX,DWORD PTR [EBP+44]
            mov DWORD PTR [(_KingData+97348)],EAX
            mov EAX,DWORD PTR [EBP+48]
            mov DWORD PTR [(_KingData+97340)],EAX
            mov CL,16
            sub CL,BYTE PTR [(_KingBss+543676)]
            mov EAX,DWORD PTR [EBP+24]
            shl EAX,CL
            mov DWORD PTR [(_KingData+97332)],EAX
            mov EAX,DWORD PTR [EBP+28]
            shl EAX,CL
            mov DWORD PTR [(_KingData+97336)],EAX
            mov EDX,DWORD PTR [EBP+32]
            shl EDX,7
            mov EAX,DWORD PTR [EBP+36]
            shl EAX,7
            mov DWORD PTR [(_KingData+97344)],EAX
            mov EAX,DWORD PTR [EBP+16]
            shl EAX,CL
            mov EBX,DWORD PTR [EBP+20]
            shl EBX,CL
            mov CL,BYTE PTR [(_KingBss+543676)]
            mov ESI,DWORD PTR [(_KingData+97362)]
            call ESI

            popad 
            leave 
            ret 
FUN_5fee7c ENDP
ALIGN 4
FUN_5fef04 PROC

            enter 128,0
            pushad 
            mov EDI,DWORD PTR [EBP+8]
            mov EAX,DWORD PTR [EBP+12]
            cmp DWORD PTR [(_KingBss+543672)],2
            jl $L_5fef1a

            shl EAX,1
$L_5fef1a::

            add EAX,EDI
            mov DWORD PTR [(_KingData+97354)],EAX
            mov EAX,DWORD PTR [EBP+36]
            mov DWORD PTR [(_KingData+97358)],EAX
            mov ECX,DWORD PTR [EBP+16]
            mov EAX,DWORD PTR [EBP+20]
            mov DWORD PTR [(_KingData+97340)],EAX
            mov EDX,DWORD PTR [EBP+28]
            shl EDX,7
            mov EAX,DWORD PTR [EBP+32]
            shl EAX,7
            mov DWORD PTR [(_KingData+97344)],EAX
            mov EAX,DWORD PTR [EBP+24]
            mov ESI,DWORD PTR [(_KingData+97362)]
            call ESI

            popad 
            leave 
            ret 
FUN_5fef04 ENDP
$L_5fef53 PROC

            enter 128,0
            pushad 
            pushfd 
            or DWORD PTR [ESP],2097152
            popfd 
            pushfd 
            test DWORD PTR [ESP],2097152
            je $L_5fef80

            and DWORD PTR [ESP],4292870143
            popfd 
            pushfd 
            test DWORD PTR [ESP],2097152
            jne $L_5fef80

            popfd 
            jmp $L_5fef8d
$L_5fef80::

            mov EAX,0
            mov DWORD PTR [(_KingBss+606148)],EAX
            popfd 
            jmp $L_5fefcb
$L_5fef8d::

            mov EAX,1
            cpuid 
            and EAX,3840
            shr EAX,8
            test EDX,8388608
            jne $L_5fefb6

            mov DWORD PTR [(_KingBss+606148)],EAX
            mov EAX,0
            mov WORD PTR [(_KingData+97352)],AX
            jmp $L_5fefcb
$L_5fefb6::

            or EAX,256
            mov DWORD PTR [(_KingBss+606148)],EAX
            mov EAX,1
            mov WORD PTR [(_KingData+97352)],AX
$L_5fefcb::

            popad 
            leave 
            ret 
$L_5fef53 ENDP
ALIGN 2
FUN_5fefce PROC

            mov EDI,DWORD PTR [ESP+4]
            mov ESI,DWORD PTR [ESP+8]
            mov EAX,DWORD PTR [ESP+12]
            mov ECX,EAX
            shr ECX,2
            cmp ECX,0
            je $L_5fefe6

            rep movsd 
$L_5fefe6::

            mov ECX,EAX
            and ECX,3
            cmp ECX,0
            je $L_5feff2

            rep movsb 
$L_5feff2::

            ret 
FUN_5fefce ENDP
FUN_5feff3 PROC

            enter 128,0
            pushad 
            mov EDX,DWORD PTR [EBP+8]
            mov EAX,0
            mov DWORD PTR [EBP-12],EAX
$L_5ff003::

            mov EAX,DWORD PTR [EBP-12]
            cmp EAX,DWORD PTR [EBP+24]
            jge $L_5ff05a

            mov EAX,DWORD PTR [EBP+12]
            mov DWORD PTR [EBP-4],EAX
            mov EBX,0
$L_5ff016::

            cmp EBX,DWORD PTR [EBP+20]
            jge $L_5ff04c

            mov EAX,DWORD PTR [EDX]
            test EAX,128
            je $L_5ff02f

            and EAX,127
            inc EDX
            add EBX,EAX
            add DWORD PTR [EBP-4],EAX
            jmp $L_5ff016
$L_5ff02f::

            and EAX,127
            inc EDX
            push EAX
            push EAX
            push EDX
            mov EAX,DWORD PTR [EBP-4]
            push EAX
            call FUN_5fefce

            pop EAX
            pop EAX
            pop EAX
            pop EAX
            add EBX,EAX
            add DWORD PTR [EBP-4],EAX
            add EDX,EAX
            jmp $L_5ff016
$L_5ff04c::

            inc DWORD PTR [EBP-12]
            mov EAX,DWORD PTR [EBP+12]
            add EAX,DWORD PTR [EBP+28]
            mov DWORD PTR [EBP+12],EAX
            jmp $L_5ff003
$L_5ff05a::

            popad 
            leave 
            ret 
FUN_5feff3 ENDP
$L_5ff05f PROC

            int 3
$L_5ff060::

            jmp $L_5ff070
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
$L_5ff070::

            push EBX
            push ESI
            push EDI
            mov ECX,55
            xor EAX,EAX
            mov EDI,OFFSET (_KingBss+627744)
            mov EBX,161803398
            xor ESI,ESI

            rep stosd DWORD PTR ES:[EDI]

            mov ECX,1
            mov DWORD PTR [(_KingBss+627960)],EBX
$L_5ff093::

            mov EAX,ESI
            xor EDX,EDX
            mov EDI,55
            sub EBX,ECX
            div EDI
            add ESI,21
            cmp ESI,1134
            mov DWORD PTR [EDX*4+(_KingBss+627744)],ECX
            mov ECX,EBX
            mov EBX,DWORD PTR [EDX*4+(_KingBss+627744)]
            jb $L_5ff093

            mov EDI,4
$L_5ff0c0::

            xor ECX,ECX
            mov ESI,OFFSET (_KingBss+627744)
$L_5ff0c7::

            lea EAX,DWORD PTR [ECX+31]
            xor EDX,EDX
            mov EBX,55
            add ESI,4
            div EBX
            mov EAX,DWORD PTR [EDX*4+(_KingBss+627744)]
            mov EDX,DWORD PTR [ESI-4]
            sub EDX,EAX
            inc ECX
            mov DWORD PTR [ESI-4],EDX
            cmp ECX,EBX
            jb $L_5ff0c7

            dec EDI
            jne $L_5ff0c0

            pop EDI
            pop ESI
            mov DWORD PTR [(_KingBss+627964)],0
            mov DWORD PTR [(_KingBss+627968)],31
            pop EBX
            ret 
$L_5ff05f ENDP
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; src\engine\sound_files.cpp, nmake ASM_SOUND_FILES=1 assembles the original
ALIGN 16
ALIGN 16
; src\engine\sound_files.cpp, nmake ASM_SOUND_FILES=1 assembles the original
ALIGN 16
_TEXT ENDS
END
