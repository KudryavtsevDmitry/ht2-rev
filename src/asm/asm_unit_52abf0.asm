; asm_unit_52abf0.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @CarPart_Select@12:PROC
EXTERN @Dtor50ff70_DeletingDestructor@12:PROC
EXTERN @Dtor5117e0_DeletingDestructor@12:PROC
EXTERN @Dtor514790_DeletingDestructor@12:PROC
EXTERN @Dtor517a20_DeletingDestructor@12:PROC
EXTERN @Dtor51dfe0_DeletingDestructor@12:PROC
EXTERN @Dtor51e710_DeletingDestructor@12:PROC
EXTERN @Dtor51ec80_DeletingDestructor@12:PROC
EXTERN @Dtor51f340_DeletingDestructor@12:PROC
EXTERN @Dtor520560_DeletingDestructor@12:PROC
EXTERN @Dtor5213f0_DeletingDestructor@12:PROC
EXTERN @Dtor5244f0_DeletingDestructor@12:PROC
EXTERN @Dtor525e00_DeletingDestructor@12:PROC
EXTERN @Dtor526b80_DeletingDestructor@12:PROC
EXTERN @Dtor527d00_DeletingDestructor@12:PROC
EXTERN @Dtor52b6d0_DeletingDestructor@12:PROC
EXTERN @Dtor52d3e0_DeletingDestructor@12:PROC
EXTERN @Dtor52da10_DeletingDestructor@12:PROC
EXTERN @Dtor52e2e0_DeletingDestructor@12:PROC
EXTERN @Pairs5_Clear@8:PROC
EXTERN @SceneObj_OnMessage@12:PROC
EXTERN @Scy52fce0_Construct@8:PROC
EXTERN @Vehicles_52e8c0@12:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _World_Fatal:PROC

PUBLIC $L_52ac00, $L_52ac10, $L_52ac20, $L_52ac30, $L_52ac40, $L_52ac50, $L_52ac60, $L_52ac70
PUBLIC $L_52ac80, $L_52ac90, $L_52aca0, $L_52acb0, $L_52acc0, $L_52acd0, $L_52ace0, $L_52acf0
PUBLIC $L_52acfc, $L_52b5bb, $L_52b5c8, $L_52b5dd, $L_52e94b, $L_52e958, $L_52e963, $L_52f4c0
PUBLIC $L_52f4d0, $L_52f4e0, $L_52f4f0, $L_52f500, $L_52f510, $L_52f520, $L_52f530, $L_52f540
PUBLIC $L_52f563, $L_52f590, $L_52f597, $L_52f690

_TEXT SEGMENT
$L_52abff PROC

            int 3
$L_52ac00::

            sub ECX,64
            jmp @Dtor514790_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ac10::

            sub ECX,64
            jmp @Dtor5117e0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ac20::

            sub ECX,64
            jmp @Dtor520560_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ac30::

            sub ECX,64
            jmp @Dtor5213f0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ac40::

            sub ECX,64
            jmp @Dtor517a20_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ac50::

            sub ECX,64
            jmp @Dtor5244f0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ac60::

            sub ECX,64
            jmp @Dtor51dfe0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ac70::

            sub ECX,64
            jmp @Dtor51e710_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ac80::

            sub ECX,64
            jmp @Dtor525e00_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ac90::

            sub ECX,64
            jmp @Dtor51ec80_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52aca0::

            sub ECX,64
            jmp @Dtor526b80_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52acb0::

            sub ECX,64
            jmp @Dtor50ff70_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52acc0::

            sub ECX,64
            jmp @Dtor527d00_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52acd0::

            sub ECX,64
            jmp @Dtor51f340_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52ace0::

            jmp $L_52acf0
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_52acf0::

            push ESI
            push EDI
            mov ESI,OFFSET (_KingBss+279928)
            mov EDI,35
$L_52acfc::

            mov ECX,ESI
            call @Pairs5_Clear@8

            add ESI,44
            dec EDI
            jne $L_52acfc

            pop EDI
            pop ESI
            ret 
$L_52abff ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
$L_52b590 PROC

            push ESI
            mov ESI,DWORD PTR [ESP+8]
            push EDI
            mov EDI,ECX
            push ESI
            call @SceneObj_OnMessage@12

            test EAX,EAX
            jne $L_52b5dd

            mov EAX,DWORD PTR [ESI]
            mov ECX,DWORD PTR [EDI+1188]
            cmp EAX,ECX
            je $L_52b5bb

            push OFFSET (_KingData+50336)
            call _World_Fatal
$L_52b590 ENDP
$L_52b5b8 PROC

            add ESP,4
$L_52b5bb::

            cmp DWORD PTR [ESI+4],13
            je $L_52b5c8

            pop EDI
            xor EAX,EAX
            pop ESI
            ret 4
$L_52b5c8::

            mov EAX,DWORD PTR [ESI+8]
            dec EAX
            jne $L_52b5dd

            mov ESI,DWORD PTR [ESI+12]
            lea ECX,DWORD PTR [EDI+9528]
            push ESI
            call @CarPart_Select@12
$L_52b5dd::

            pop EDI
            mov EAX,1
            pop ESI
            ret 4
$L_52b5b8 ENDP
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_52e920 PROC

            push ESI
            push EDI
            mov EDI,DWORD PTR [ESP+12]
            mov ESI,ECX
            push EDI
            call @SceneObj_OnMessage@12

            test EAX,EAX
            jne $L_52e963

            mov EAX,DWORD PTR [EDI]
            mov ECX,DWORD PTR [ESI+1188]
            cmp EAX,ECX
            je $L_52e94b

            push OFFSET (_KingData+50512)
            call _World_Fatal
$L_52e920 ENDP
$L_52e948 PROC

            add ESP,4
$L_52e94b::

            cmp DWORD PTR [EDI+4],10
            je $L_52e958

            pop EDI
            xor EAX,EAX
            pop ESI
            ret 4
$L_52e958::

            add EDI,8
            mov ECX,ESI
            push EDI
            call @Vehicles_52e8c0@12
$L_52e963::

            pop EDI
            mov EAX,1
            pop ESI
            ret 4
$L_52e948 ENDP
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
; src/startup.cpp, nmake ASM_STARTUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
; src/startup.cpp, nmake ASM_STARTUP=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
$L_52f4bf PROC

            int 3
$L_52f4c0::

            sub ECX,376
            jmp @Dtor52b6d0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52f4d0::

            sub ECX,256
            jmp @Dtor52da10_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52f4e0::

            sub ECX,376
            jmp @Dtor52da10_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52f4f0::

            sub ECX,256
            jmp @Dtor52d3e0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52f500::

            sub ECX,376
            jmp @Dtor52d3e0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52f510::

            sub ECX,256
            jmp @Dtor52e2e0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52f520::

            sub ECX,376
            jmp @Dtor52e2e0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_52f530::

            jmp $L_52f540
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_52f540::

            push EBX
            push ESI
            push EDI
            mov ECX,55
            xor EAX,EAX
            mov EDI,OFFSET (_KingBss+290880)
            mov EBX,161803398
            xor ESI,ESI

            rep stosd DWORD PTR ES:[EDI]

            mov ECX,1
            mov DWORD PTR [(_KingBss+291096)],EBX
$L_52f563::

            mov EAX,ESI
            xor EDX,EDX
            mov EDI,55
            sub EBX,ECX
            div EDI
            add ESI,21
            cmp ESI,1134
            mov DWORD PTR [EDX*4+(_KingBss+290880)],ECX
            mov ECX,EBX
            mov EBX,DWORD PTR [EDX*4+(_KingBss+290880)]
            jb $L_52f563

            mov EDI,4
$L_52f590::

            xor ECX,ECX
            mov ESI,OFFSET (_KingBss+290880)
$L_52f597::

            lea EAX,DWORD PTR [ECX+31]
            xor EDX,EDX
            mov EBX,55
            add ESI,4
            div EBX
            mov EAX,DWORD PTR [EDX*4+(_KingBss+290880)]
            mov EDX,DWORD PTR [ESI-4]
            sub EDX,EAX
            inc ECX
            mov DWORD PTR [ESI-4],EDX
            cmp ECX,EBX
            jb $L_52f597

            dec EDI
            jne $L_52f590

            pop EDI
            pop ESI
            mov DWORD PTR [(_KingBss+291100)],0
            mov DWORD PTR [(_KingBss+291104)],31
            pop EBX
            ret 
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
$L_52f4bf ENDP
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
$L_52f680 PROC

            jmp $L_52f690
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_52f690::

            mov ECX,OFFSET (_KingBss+291112)
            jmp @Scy52fce0_Construct@8
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H

            nop
            nop
$L_52f680 ENDP
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
; src/scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; engine: src\truck\scenery.cpp, nmake ASM_SCENERY=1 assembles the original
ALIGN 16
; vehicle: src\truck\vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
          BYTE 090H
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
; src/vehicle.cpp, nmake ASM_VEHICLE=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
_TEXT ENDS
END
