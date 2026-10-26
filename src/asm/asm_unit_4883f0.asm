; asm_unit_4883f0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_641628:NEAR
EXTERN $L_641698:NEAR
EXTERN @List491_Insert@20:PROC
EXTERN @SchList_End@12:PROC
EXTERN @Shell_Nop@8:PROC
EXTERN @Stl_Lock@8:PROC
EXTERN _KingBss:BYTE
EXTERN _Stl_ChunkAlloc:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN _free:PROC

PUBLIC $L_489bf2, $L_489c1c, $L_489fcd, $L_489ff7, $L_48a27f, $L_48a2a9, $L_48a53f, $L_48a569
PUBLIC $L_48fa2a, $L_48fa3b, $L_48fa5f, $L_48fa86, $L_491478, $L_49148a, $L_4917bd, $L_4917ce
PUBLIC $L_4917d7, $L_4917df, $L_4917f9

_TEXT SEGMENT
$L_4898d3 PROC

            mov ECX,OFFSET (_KingBss+10724)
            call @Stl_Lock@8

            mov ECX,DWORD PTR [(_KingBss+10680)]
            mov EAX,DWORD PTR [EBP-20]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [(_KingBss+10680)],EAX
            xor EDI,EDI
            mov DWORD PTR [(_KingBss+10724)],EDI
            push EDI
            push EDI
            call __CxxThrowException@8

            nop
            nop
            nop
            nop
$L_4898d3 ENDP
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
$L_489bd0 PROC

            mov EAX,DWORD PTR [EBP-20]
            test EAX,EAX
            je $L_489c1c

            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,3
            cmp EAX,128
            jbe $L_489bf2

            mov EAX,DWORD PTR [EBP-24]
            push EAX
            call _free

            add ESP,4
            jmp $L_489c1c
$L_489bf2::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP-24]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_489c1c::

            push 0
            push 0
            call __CxxThrowException@8

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
$L_489bd0 ENDP
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
$L_489fab PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_489ff7

            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,3
            cmp EAX,128
            jbe $L_489fcd

            mov EAX,DWORD PTR [EBP-20]
            push EAX
            call _free

            add ESP,4
            jmp $L_489ff7
$L_489fcd::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP-20]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_489ff7::

            push 0
            push 0
            call __CxxThrowException@8
$L_489fab ENDP
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
$L_48a260 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_48a2a9

            shl EAX,2
            cmp EAX,128
            jbe $L_48a27f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_48a2a9
$L_48a27f::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP-20]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_48a2a9::

            push 0
            push 0
            call __CxxThrowException@8

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
            nop
$L_48a260 ENDP
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
$L_48a520 PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_48a569

            shl EAX,2
            cmp EAX,128
            jbe $L_48a53f

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_48a569
$L_48a53f::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP-20]
            mov DWORD PTR [EAX],ECX
            mov DWORD PTR [ESI],EAX
            mov DWORD PTR [(_KingBss+10724)],0
$L_48a569::

            push 0
            push 0
            call __CxxThrowException@8

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
            nop
$L_48a520 ENDP
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/values.cpp, nmake ASM_VALUES=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
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
; src/ai_world.cpp, nmake ASM_AI_WORLD=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
$L_48fa1e PROC

            mov EDI,DWORD PTR [EBP-28]
            mov EBX,DWORD PTR [EBP-20]
            cmp EDI,EBX
            mov ESI,EDI
            je $L_48fa3b
$L_48fa2a::

            mov ECX,ESI
            call @Shell_Nop@8

            add ESI,580
            cmp ESI,EBX
            jne $L_48fa2a
$L_48fa3b::

            mov ECX,DWORD PTR [EBP-24]
            test ECX,ECX
            je $L_48fa86

            lea EAX,DWORD PTR [ECX+ECX*8]
            shl EAX,4
            add EAX,ECX
            shl EAX,2
            cmp EAX,128
            jbe $L_48fa5f

            push EDI
            call _free

            add ESP,4
            jmp $L_48fa86
$L_48fa5f::

            add EAX,7
            mov ECX,OFFSET (_KingBss+10724)
            shr EAX,3
            lea ESI,DWORD PTR [EAX*4+(_KingBss+10656)]
            call @Stl_Lock@8

            mov ECX,DWORD PTR [ESI]
            mov DWORD PTR [EDI],ECX
            mov DWORD PTR [ESI],EDI
            mov DWORD PTR [(_KingBss+10724)],0
$L_48fa86::

            push 0
            push 0
            call __CxxThrowException@8

            nop
$L_48fa1e ENDP
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
FUN_491420 PROC

            mov EAX,DWORD PTR FS:[0]
            push -1
            push OFFSET $L_641628
            push EAX
            mov EAX,DWORD PTR [ESP+20]
            mov DWORD PTR FS:[0],ESP
            push ESI
            mov ECX,DWORD PTR [EAX+4220]
            test ECX,ECX
            jne $L_491478

            mov ECX,DWORD PTR [ESP+28]
            mov DWORD PTR [ESP+28],ECX
            lea EDX,DWORD PTR [ESP+28]
            lea ESI,DWORD PTR [EAX+44]
            push EDX
            push ECX
            push ESP
            mov ECX,ESI
            mov DWORD PTR [ESP+24],0
            call @SchList_End@12

            lea EAX,DWORD PTR [ESP+32]
            mov ECX,ESI
            push EAX
            call @List491_Insert@20

            mov ECX,DWORD PTR [ESI]
            mov ECX,DWORD PTR [ECX+4]
            jmp $L_49148a
$L_491478::

            mov EDX,DWORD PTR [ESP+28]
            dec ECX
            mov DWORD PTR [EAX+4220],ECX
            mov ECX,DWORD PTR [EAX+ECX*4+124]
            mov DWORD PTR [ECX+8],EDX
$L_49148a::

            mov EAX,DWORD PTR [ESP+20]
            pop ESI
            mov DWORD PTR [EAX],ECX
            mov ECX,DWORD PTR [ESP]
            mov DWORD PTR FS:[0],ECX
            add ESP,12
            ret 
FUN_491420 ENDP
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
FUN_491750 PROC

            push -1
            push OFFSET $L_641698
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,8
            push EDI
            mov ECX,OFFSET (_KingBss+10724)
            call @Stl_Lock@8

            mov DWORD PTR [ESP+20],0
            mov EAX,DWORD PTR [(_KingBss+10664)]
            test EAX,EAX
            jne $L_4917d7

            lea EAX,DWORD PTR [ESP+4]
            mov DWORD PTR [ESP+4],20
            push EAX
            push 16
            call _Stl_ChunkAlloc

            mov EDI,DWORD PTR [ESP+12]
            add ESP,8
            cmp EDI,1
            je $L_4917df

            lea ECX,DWORD PTR [EAX+16]
            push ESI
            mov DWORD PTR [(_KingBss+10664)],ECX
            mov EDX,ECX
            add ECX,16
            mov ESI,1
            dec EDI
            cmp EDI,ESI
            je $L_4917ce
$L_4917bd::

            mov DWORD PTR [EDX],ECX
            mov EDI,DWORD PTR [ESP+8]
            mov EDX,ECX
            inc ESI
            add ECX,16
            dec EDI
            cmp EDI,ESI
            jne $L_4917bd
$L_4917ce::

            mov DWORD PTR [EDX],0
            pop ESI
            jmp $L_4917df
$L_4917d7::

            mov ECX,DWORD PTR [EAX]
            mov DWORD PTR [(_KingBss+10664)],ECX
$L_4917df::

            mov DWORD PTR [(_KingBss+10724)],0
            lea ECX,DWORD PTR [EAX+8]
            test ECX,ECX
            pop EDI
            je $L_4917f9

            mov EDX,DWORD PTR [ESP+24]
            mov EDX,DWORD PTR [EDX]
            mov DWORD PTR [ECX],EDX
$L_4917f9::

            mov ECX,DWORD PTR [ESP+8]
            mov DWORD PTR FS:[0],ECX
            add ESP,20
            ret 4
FUN_491750 ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\script\native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
ALIGN 16
; engine: src\script\native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; engine: src\script\native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\script\native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
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
; src/evaluator.cpp, nmake ASM_EVALUATOR=1 assembles the original
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
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
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
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
; engine: src\script\native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
          BYTE 090H
ALIGN 16
; engine: src\script\native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\script\native_values.cpp, nmake ASM_NATIVE_VALUES=1 assembles the original
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
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; AI script evaluator: src\script\natives.cpp, nmake ASM_NATIVES=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/natives.cpp, nmake ASM_NATIVES=1 assembles the original
_TEXT ENDS
END
