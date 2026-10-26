; asm_ai_trce.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @Int_Copy@12:PROC
EXTERN @Player_SetPosition@12:PROC
EXTERN _Route_Call4870e0:PROC


_TEXT SEGMENT
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
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
ALIGN 16
ALIGN 16
; AI geometry, CRC and filters: src\ai\route_search.cpp, nmake ASM_ROUTE_SEARCH=1 assembles the original
ALIGN 16
ALIGN 16
_TEXT ENDS
END
