; asm_mmt.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Thunk592ed0@8:PROC
EXTERN _Joy_EnumObject@8:PROC
EXTERN _Joy_Log:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _OldCrt_fclose:PROC
EXTERN _OldCrt_fprintf:PROC
EXTERN __imp__GetLocalTime@4:PROC
EXTERN _atexit:PROC
EXTERN _sprintf:PROC
EXTERN _strerror:PROC

PUBLIC $L_576D9D, $L_576DA8, $L_576DB3, $L_576DBE, $L_576DC9, $L_576DD4, $L_576DDF, $L_576DEA
PUBLIC $L_576DF5, $L_576E00, $L_576E0B, $L_576E16, $L_576e1f, $L_576e3c, $L_576e90, $L_576ee0
PUBLIC $L_576ef0, $L_576f00, $L_576f10, $L_576f30, $L_576f82, $L_576f90, $L_576fcb, $L_577014
PUBLIC $L_577074, $L_5770b6, $L_5770e0, $L_577117, $L_577139, $L_577185, $L_5771c1, $L_5771e9
PUBLIC $L_577230, $L_577282, $L_5772de, $L_5772fc, $L_577315

_TEXT SEGMENT
ALIGN 16
FUN_576d70 PROC

            sub ESP,28
            lea EAX,DWORD PTR [ESP+12]
            push ESI
            push EDI
            push EAX
            call DWORD PTR __imp__GetLocalTime@4

            mov ECX,DWORD PTR [ESP+22]
            and ECX,65535
            lea EAX,DWORD PTR [ECX-1]
            cmp EAX,11
            ja $L_576e3c

            jmp DWORD PTR [EAX*4+$L_576e90]
$L_576D9D::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66824)
            jmp $L_576e1f
$L_576DA8::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66820)
            jmp $L_576e1f
$L_576DB3::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66816)
            jmp $L_576e1f
$L_576DBE::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66812)
            jmp $L_576e1f
$L_576DC9::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66808)
            jmp $L_576e1f
$L_576DD4::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66804)
            jmp $L_576e1f
$L_576DDF::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66800)
            jmp $L_576e1f
$L_576DEA::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66796)
            jmp $L_576e1f
$L_576DF5::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66792)
            jmp $L_576e1f
$L_576E00::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66788)
            jmp $L_576e1f
$L_576E0B::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66784)
            jmp $L_576e1f
$L_576E16::
            lea EDX,DWORD PTR [ESP+8]
            mov EDI,OFFSET (_KingData+66780)
$L_576e1f::

            or ECX,4294967295
            xor EAX,EAX

            repne scasb 

            not ECX
            sub EDI,ECX
            mov EAX,ECX
            mov ESI,EDI
            mov EDI,EDX
            shr ECX,2

            rep movsd 

            mov ECX,EAX
            and ECX,3

            rep movsb 
$L_576e3c::

            mov ECX,DWORD PTR [ESP+32]
            mov EDX,DWORD PTR [ESP+30]
            mov EAX,DWORD PTR [ESP+28]
            and ECX,65535
            and EDX,65535
            push ECX
            mov ECX,DWORD PTR [ESP+24]
            and EAX,65535
            push EDX
            push EAX
            mov EAX,DWORD PTR [ESP+38]
            and ECX,65535
            lea EDX,DWORD PTR [ESP+20]
            push ECX
            mov ECX,DWORD PTR [ESP+56]
            and EAX,65535
            push EDX
            push EAX
            push OFFSET (_KingData+66752)
            push ECX
            call _sprintf

            add ESP,32
            pop EDI
            pop ESI
            add ESP,28
            ret 

            align 16
$L_576e90:: 
            DWORD $L_576D9D
            DWORD $L_576DA8                   
            DWORD $L_576DB3
            DWORD $L_576DBE
            DWORD $L_576DC9
            DWORD $L_576DD4
            DWORD $L_576DDF
            DWORD $L_576DEA
            DWORD $L_576DF5
            DWORD $L_576E00
            DWORD $L_576E0B
            DWORD $L_576E16
      
      FUN_576EC0 PROC
          call FUN_576ED0
          jmp $L_576EE0
 
      FUN_576EC0 ENDP
      align 16
      FUN_576ED0 PROC
            mov ECX,OFFSET (_KingBss+429296)
            jmp $L_576f10
          align 16
$L_576ee0::

            push OFFSET $L_576ef0
            call _atexit

            pop ECX
            ret 
          align 16
$L_576ef0::

          mov ECX,OFFSET (_KingBss+429296)
          jmp $L_576f00
          align 16
$L_576f00::

          jmp @Thunk592ed0@8
          align 16
$L_576f10::

            mov EAX,ECX
            xor ECX,ECX
            mov DWORD PTR [EAX+8],ECX
            mov DWORD PTR [EAX+4],ECX
            mov DWORD PTR [EAX],OFFSET (_KingRdata+21288)
            ret 
            FUN_576ED0 ENDP
          align 16
$L_576f30::

            mov EAX,DWORD PTR [ESP+4]
            sub ESP,200
            mov ECX,DWORD PTR [EAX+24]
            mov EDX,DWORD PTR [EAX+28]
            push ECX
            mov ECX,DWORD PTR [EAX]
            push EDX
            mov EDX,DWORD PTR [EAX+20]
            push ECX
            add EAX,32
            push EDX
            push EAX
            lea EAX,DWORD PTR [ESP+20]
            push OFFSET (_KingData+66828)
            push EAX
            call _sprintf

            lea ECX,DWORD PTR [ESP+28]
            push ECX
            call _Joy_Log

            mov EAX,DWORD PTR [ESP+240]
            add ESP,32
            test EAX,EAX
            je $L_576f82

            lea EDX,DWORD PTR [ESP]
            push EDX
            push EAX
            call _OldCrt_fprintf

            add ESP,8
$L_576f82::

            mov EAX,1
            add ESP,200
            ret 8
$L_576f90::

            sub ESP,632
            mov EAX,DWORD PTR [(_KingBss+435044)]
            lea EDX,DWORD PTR [ESP+4]
            push EBX
            push EBP
            mov ECX,DWORD PTR [EAX]
            push ESI
            push 0
            push EDX
            mov EDX,DWORD PTR [ESP+656]
            add EDX,4
            push EDX
            push EAX
            call DWORD PTR [ECX+12]

            test EAX,EAX
            jge $L_576fcb

            pop ESI
            pop EBP
            mov EAX,1
            pop EBX
            add ESP,632
            ret 8
$L_576fcb::

            mov EDX,DWORD PTR [(_KingBss+435056)]
            mov EAX,DWORD PTR [ESP+16]
            mov ECX,DWORD PTR [EAX]
            lea EDX,DWORD PTR [EDX*4+(_KingBss+435000)]
            push EDX
            push OFFSET (_KingRdata+42096)
            push EAX
            call DWORD PTR [ECX]

            mov EAX,DWORD PTR [(_KingBss+435056)]
            mov DWORD PTR [ESP+64],580
            mov EBX,DWORD PTR [ESP+652]
            mov EBP,DWORD PTR [EAX*4+(_KingBss+435000)]
            test EBX,EBX
            je $L_577014

            push OFFSET (_KingData+67260)
            push EBX
            call _OldCrt_fprintf

            add ESP,8
$L_577014::

            mov ECX,DWORD PTR [EBP]
            lea EDX,DWORD PTR [ESP+64]
            push EDX
            push EBP
            call DWORD PTR [ECX+60]

            mov ESI,EAX
            cmp ESI,2147942487
            je $L_5772fc

            cmp ESI,2147942421
            je $L_5772fc

            test ESI,ESI
            jl $L_5772fc

            test EBX,EBX
            je $L_577074

            push OFFSET (_KingData+45672)
            push EBX
            call _OldCrt_fprintf

            mov EDX,DWORD PTR [ESP+108]
            add ESP,8
            lea EAX,DWORD PTR [ESP+364]
            lea ECX,DWORD PTR [ESP+104]
            push EAX
            push ECX
            push EDX
            push OFFSET (_KingData+67212)
            push EBX
            call _OldCrt_fprintf

            add ESP,20
$L_577074::

            lea EAX,DWORD PTR [ESP+364]
            push EDI
            test EAX,EAX
            mov EAX,DWORD PTR [(_KingBss+435056)]
            lea EDX,DWORD PTR [EAX+EAX*8]
            je $L_5770b6

            shl EDX,3
            sub EDX,EAX
            lea EDI,DWORD PTR [ESP+368]
            or ECX,4294967295
            xor EAX,EAX
            shl EDX,3

            repne scasb 

            not ECX
            lea ESI,DWORD PTR [EDX+(_KingBss+429368)]
            sub EDI,ECX
            mov DWORD PTR [ESP+16],ESI
            mov ESI,EDI
            mov EDI,DWORD PTR [ESP+16]
            mov EAX,ECX
            jmp $L_5770e0
$L_5770b6::

            shl EDX,3
            sub EDX,EAX
            mov EDI,OFFSET (_KingData+67200)
            or ECX,4294967295
            xor EAX,EAX
            shl EDX,3

            repne scasb 

            not ECX
            lea ESI,DWORD PTR [EDX+(_KingBss+429368)]
            sub EDI,ECX
            mov DWORD PTR [ESP+16],ESI
            mov ESI,EDI
            mov EDI,DWORD PTR [ESP+16]
            mov EAX,ECX
$L_5770e0::

            shr ECX,2

            rep movsd 

            mov ECX,EAX
            and ECX,3

            rep movsb 

            lea ECX,DWORD PTR [ESP+108]
            test ECX,ECX
            je $L_577117

            lea EDI,DWORD PTR [ESP+108]
            or ECX,4294967295
            xor EAX,EAX
            lea ESI,DWORD PTR [EDX+(_KingBss+429628)]

            repne scasb 

            not ECX
            sub EDI,ECX
            mov DWORD PTR [ESP+16],ESI
            mov ESI,EDI
            mov EDI,DWORD PTR [ESP+16]
            mov EAX,ECX
            jmp $L_577139
$L_577117::

            mov EDI,OFFSET (_KingData+67200)
            or ECX,4294967295
            xor EAX,EAX
            lea ESI,DWORD PTR [EDX+(_KingBss+429628)]

            repne scasb 

            not ECX
            sub EDI,ECX
            mov DWORD PTR [ESP+16],ESI
            mov ESI,EDI
            mov EDI,DWORD PTR [ESP+16]
            mov EAX,ECX
$L_577139::

            shr ECX,2

            rep movsd 

            mov ECX,EAX
            mov EAX,DWORD PTR [ESP+104]
            and ECX,3

            rep movsb 

            mov DWORD PTR [EDX+(_KingBss+429320)],EAX
            mov BYTE PTR [EDX+(_KingBss+429320)+4],AL
            mov ECX,EAX
            and EAX,65536
            shr ECX,8
            cmp EAX,65536
            mov BYTE PTR [EDX+(_KingBss+429320)+5],CL
            sete AL
            test EBX,EBX
            mov BYTE PTR [EDX+(_KingBss+429320)+6],AL
            je $L_577185

            push OFFSET (_KingData+67184)
            push EBX
            call _OldCrt_fprintf

            add ESP,8
$L_577185::

            mov ECX,DWORD PTR [EBP]
            push 0
            push EBX
            push OFFSET _Joy_EnumObject@8
            push EBP
            call DWORD PTR [ECX+16]

            push OFFSET (_KingData+67148)
            call _Joy_Log

            mov EDX,DWORD PTR [EBP]
            add ESP,4
            push 0
            push EBX
            push OFFSET $L_576f30
            push EBP
            call DWORD PTR [EDX+76]

            test EAX,EAX
            jge $L_5771c1

            push OFFSET (_KingData+67084)
            call _Joy_Log

            add ESP,4
$L_5771c1::

            mov ECX,11
            xor EAX,EAX
            lea EDI,DWORD PTR [ESP+24]

            rep stosd DWORD PTR ES:[EDI]

            test EBX,EBX
            mov DWORD PTR [ESP+24],44
            pop EDI
            je $L_5771e9

            push OFFSET (_KingData+67064)
            push EBX
            call _OldCrt_fprintf

            add ESP,8
$L_5771e9::

            mov EAX,DWORD PTR [EBP]
            lea ECX,DWORD PTR [ESP+20]
            push ECX
            push EBP
            call DWORD PTR [EAX+12]

            mov ESI,EAX
            test ESI,ESI
            jge $L_577230

            test EBX,EBX
            je $L_577315

            push ESI
            call _strerror

            add ESP,4
            push EAX
            push ESI
            push OFFSET (_KingData+67048)
            push EBX
            call _OldCrt_fprintf

            push EBX
            call _OldCrt_fclose

            add ESP,20
            mov EAX,ESI
            pop ESI
            pop EBP
            pop EBX
            add ESP,632
            ret 8
$L_577230::

            test EBX,EBX
            je $L_577282

            push OFFSET (_KingData+45672)
            push EBX
            call _OldCrt_fprintf

            mov EDX,DWORD PTR [ESP+52]
            mov EAX,DWORD PTR [ESP+56]
            mov ECX,DWORD PTR [ESP+64]
            add ESP,8
            push EDX
            mov EDX,DWORD PTR [ESP+56]
            push EAX
            mov EAX,DWORD PTR [ESP+68]
            push ECX
            mov ECX,DWORD PTR [ESP+36]
            push EDX
            mov EDX,DWORD PTR [ESP+56]
            push EAX
            mov EAX,DWORD PTR [ESP+56]
            push ECX
            mov ECX,DWORD PTR [ESP+56]
            push EDX
            mov EDX,DWORD PTR [ESP+56]
            push EAX
            push ECX
            push EDX
            push OFFSET (_KingData+66920)
            push EBX
            call _OldCrt_fprintf

            add ESP,48
$L_577282::

            mov ECX,DWORD PTR [(_KingBss+435056)]
            mov DX,WORD PTR [ESP+32]
            lea EAX,DWORD PTR [ECX+ECX*8]
            shl EAX,3
            sub EAX,ECX
            shl EAX,3
            mov WORD PTR [EAX+(_KingBss+429320)+40],DX
            mov DX,WORD PTR [ESP+36]
            mov WORD PTR [EAX+(_KingBss+429320)+42],DX
            mov DX,WORD PTR [ESP+40]
            mov WORD PTR [EAX+(_KingBss+429320)+44],DX
            mov EDX,DWORD PTR [ESP+24]
            mov EAX,1
            test AL,DH
            je $L_5772de

            mov DWORD PTR [ECX*4+(_KingBss+429256)],EAX
            inc ECX
            pop ESI
            pop EBP
            mov DWORD PTR [(_KingBss+435056)],ECX
            pop EBX
            add ESP,632
            ret 8
$L_5772de::

            mov DWORD PTR [ECX*4+(_KingBss+429256)],0
            inc ECX
            pop ESI
            pop EBP
            mov DWORD PTR [(_KingBss+435056)],ECX
            pop EBX
            add ESP,632
            ret 8
$L_5772fc::

            test EBX,EBX
            je $L_577315

            push ESI
            push OFFSET (_KingData+66908)
            push EBX
            call _OldCrt_fprintf

            push EBX
            call _OldCrt_fclose

            add ESP,16
$L_577315::

            mov EAX,ESI
            pop ESI
            pop EBP
            pop EBX
            add ESP,632
            ret 8
FUN_576d70 ENDP
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
ALIGN 16
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original


; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
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
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; mmt: src\truck\force_feedback.cpp, nmake ASM_FORCE_FEEDBACK=1 assembles the original
          BYTE 090H
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
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
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
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
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
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
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
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
; src/object_ids.cpp, nmake ASM_OBJECT_IDS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
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
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
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
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
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
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
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
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
ALIGN 16
; engine: src\truck\moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/moving_items.cpp, nmake ASM_MOVING_ITEMS=1 assembles the original
_TEXT ENDS
END
