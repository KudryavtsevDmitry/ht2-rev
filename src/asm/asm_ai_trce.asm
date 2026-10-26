; asm_ai_trce.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Int_Copy@12:PROC
EXTERN @Player_SetPosition@12:PROC
EXTERN @Stl_Lock@8:PROC
EXTERN _KingBss:BYTE
EXTERN _Route_Call4870e0:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN _free:PROC

PUBLIC $L_487c7e, $L_487ca8

_TEXT SEGMENT
ALIGN 16
FUN_487290 PROC

            sub ESP,32
            mov EAX,DWORD PTR [ESP+40]
            mov EDX,DWORD PTR [ESP+36]
            or ECX,4294967295
            mov DWORD PTR [ESP+4],EAX
            mov DWORD PTR [ESP],ECX
            mov DWORD PTR [ESP+28],ECX
            lea ECX,DWORD PTR [ESP]
            xor EAX,EAX
            push ECX
            lea ECX,DWORD PTR [EDX+8]
            mov DWORD PTR [ESP+12],0
            mov DWORD PTR [ESP+16],0
            mov DWORD PTR [ESP+20],EAX
            mov DWORD PTR [ESP+24],EAX
            mov DWORD PTR [ESP+28],EAX
            call @Player_SetPosition@12

            mov EAX,DWORD PTR [ESP+44]
            lea EDX,DWORD PTR [ESP+36]
            push EAX
            push ECX
            mov ECX,ESP
            push EDX
            call @Int_Copy@12

            call _Route_Call4870e0

            add ESP,40
            ret 
FUN_487290 ENDP
ALIGN 16
; src/ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
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
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
ALIGN 16
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
$L_487c5c PROC

            mov EAX,DWORD PTR [EBP-24]
            test EAX,EAX
            je $L_487ca8

            lea EAX,DWORD PTR [EAX+EAX*2]
            shl EAX,2
            cmp EAX,128
            jbe $L_487c7e

            mov EDX,DWORD PTR [EBP-20]
            push EDX
            call _free

            add ESP,4
            jmp $L_487ca8
$L_487c7e::

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
$L_487ca8::

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
            nop
$L_487c5c ENDP
ALIGN 16
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; src/ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/ai_traffic.cpp, nmake ASM_AI_TRAFFIC=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
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
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
ALIGN 16
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
; src/route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
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
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
          BYTE 090H
          BYTE 090H
          BYTE 090H
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
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
          BYTE 090H
ALIGN 16
; AI helpers: src\script\runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
ALIGN 16
; src/runtime.cpp, nmake ASM_RUNTIME=1 assembles the original
_TEXT ENDS
END
