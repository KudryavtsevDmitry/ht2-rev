; asm_unit_5a3e00.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_648608:NEAR
EXTERN @Dtor5a46e0_DeletingDestructor@12:PROC
EXTERN @Dtor5a4c80_DeletingDestructor@12:PROC
EXTERN @Object_ConstructKind@16:PROC
EXTERN @Owner_Attach@12:PROC
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _Scene_FindObject:PROC
EXTERN _Sys_Fatal:PROC
EXTERN _sprintf:PROC

PUBLIC $L_5a6942, $L_5a6986, $L_5a69c5, $L_5a69df, $L_5a6a13, $L_5a6a55, $L_5ab0b0, $L_5ab0d0
PUBLIC $L_5ab0e0

_TEXT SEGMENT
ALIGN 16
FUN_5a6890 PROC

            push -1
            push OFFSET $L_648608
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,68
            push EBX
            push EBP
            push ESI
            push EDI
            mov ESI,ECX
            push OFFSET (_KingData+76348)
            push 31
            mov DWORD PTR [ESP+24],ESI
            call @Object_ConstructKind@16

            mov ECX,DWORD PTR [ESP+104]
            mov EAX,DWORD PTR [ESP+100]
            mov EDX,DWORD PTR [ESP+108]
            mov EBP,DWORD PTR [ESP+120]
            mov DWORD PTR [ESI+40],ECX
            mov ECX,DWORD PTR [ESP+116]
            mov DWORD PTR [ESI+32],EAX
            mov EAX,DWORD PTR [ESP+112]
            mov DWORD PTR [ESI+44],EDX
            mov EDX,DWORD PTR [ESP+132]
            mov DWORD PTR [ESI+48],EAX
            mov EAX,DWORD PTR [ESP+128]
            mov DWORD PTR [ESI+120],EDX
            mov DWORD PTR [ESI+52],ECX
            mov ECX,DWORD PTR [ESP+124]
            push EBP
            lea EDX,DWORD PTR [ESP+24]
            push OFFSET (_KingData+76340)
            push EDX
            mov DWORD PTR [ESP+104],0
            mov DWORD PTR [ESI],OFFSET (_KingRdata+25960)
            mov DWORD PTR [ESI+116],EAX
            mov DWORD PTR [ESI+104],ECX
            call _sprintf

            lea EAX,DWORD PTR [ESP+32]
            push EAX
            push 0
            call _Scene_FindObject

            add ESP,20
            mov DWORD PTR [ESI+108],EAX
            test EAX,EAX
            jne $L_5a6942

            lea ECX,DWORD PTR [ESP+20]
            push EAX
            push ECX
            call _Sys_Fatal
FUN_5a6890 ENDP
$L_5a693f PROC

            add ESP,8
$L_5a6942::

            mov EAX,DWORD PTR [ESI+108]
            mov EBX,1
            push EBP
            push OFFSET (_KingData+76332)
            mov DL,BYTE PTR [EAX+40]
            or DL,BL
            mov BYTE PTR [EAX+40],DL
            lea EDX,DWORD PTR [ESP+28]
            push EDX
            call _sprintf

            lea EAX,DWORD PTR [ESP+32]
            push EAX
            push 0
            call _Scene_FindObject

            add ESP,20
            mov DWORD PTR [ESI+112],EAX
            test EAX,EAX
            jne $L_5a6986

            lea ECX,DWORD PTR [ESP+20]
            push EAX
            push ECX
            call _Sys_Fatal
$L_5a693f ENDP
$L_5a6983 PROC

            add ESP,8
$L_5a6986::

            mov EAX,DWORD PTR [ESI+112]
            push EBP
            push OFFSET (_KingData+76324)
            mov DL,BYTE PTR [EAX+40]
            or DL,BL
            mov BYTE PTR [EAX+40],DL
            lea EDX,DWORD PTR [ESP+28]
            push EDX
            call _sprintf

            lea EAX,DWORD PTR [ESP+32]
            push EAX
            push 0
            call _Scene_FindObject

            add ESP,20
            mov DWORD PTR [ESI+64],EAX
            test EAX,EAX
            jne $L_5a69c5

            lea ECX,DWORD PTR [ESP+20]
            push EAX
            push ECX
            call _Sys_Fatal
$L_5a6983 ENDP
$L_5a69c2 PROC

            add ESP,8
$L_5a69c5::

            test BYTE PTR [ESI+120],4
            jne $L_5a6a21

            mov EDX,DWORD PTR [ESP+124]
            mov EDI,EBX
            lea EAX,DWORD PTR [EDX+1]
            cmp EAX,EBX
            mov DWORD PTR [ESP+124],EAX
            jle $L_5a6a55

            lea EBX,DWORD PTR [ESI+68]
$L_5a69df::

            push EDI
            push EBP
            lea EAX,DWORD PTR [ESP+28]
            push OFFSET (_KingData+76316)
            push EAX
            call _sprintf

            lea ECX,DWORD PTR [ESP+36]
            push ECX
            push 0
            call _Scene_FindObject

            add ESP,24
            mov DWORD PTR [EBX],EAX
            test EAX,EAX
            jne $L_5a6a13

            lea EDX,DWORD PTR [ESP+20]
            push EAX
            push EDX
            call _Sys_Fatal
$L_5a69c2 ENDP
$L_5a6a10 PROC

            add ESP,8
$L_5a6a13::

            mov EAX,DWORD PTR [ESP+124]
            inc EDI
            add EBX,4
            cmp EDI,EAX
            jl $L_5a69df

            jmp $L_5a6a55
$L_5a6a10 ENDP
$L_5a6a21 PROC

            push EBP
            lea EAX,DWORD PTR [ESP+24]
            push OFFSET (_KingData+76308)
            push EAX
            call _sprintf

            lea ECX,DWORD PTR [ESP+32]
            push ECX
            push 0
            call _Scene_FindObject

            add ESP,20
            mov DWORD PTR [ESI+68],EAX
            test EAX,EAX
            jne $L_5a6a55

            lea EDX,DWORD PTR [ESP+20]
            push EAX
            push EDX
            call _Sys_Fatal
$L_5a6a21 ENDP
$L_5a6a52 PROC

            add ESP,8
$L_5a6a55::

            mov ECX,DWORD PTR [ESI+32]
            push ESI
            add ECX,2992
            call @Owner_Attach@12

            mov ECX,DWORD PTR [ESP+84]
            mov EAX,ESI
            pop EDI
            pop ESI
            pop EBP
            pop EBX
            mov DWORD PTR FS:[0],ECX
            add ESP,80
            ret 36
$L_5a6a52 ENDP
ALIGN 16
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
ALIGN 16
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
ALIGN 16
$L_5ab0a0 PROC

            ret 
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
          BYTE 090H
$L_5ab0b0::

            ret 
$L_5ab0a0 ENDP
$L_5ab0cf PROC

            int 3
$L_5ab0d0::

            sub ECX,256
            jmp @Dtor5a46e0_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_5ab0e0::

            sub ECX,32
            jmp @Dtor5a4c80_DeletingDestructor@12

            int 3
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH

            int 3
$L_5ab0cf ENDP
ALIGN 16
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
; engine: src\truck\weather.cpp, nmake ASM_WEATHER=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; vector math: src\engine\vecmath.cpp, nmake ASM_VECMATH=1 assembles the original
ALIGN 16
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
ALIGN 16
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
ALIGN 16
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
; input queue: src\engine\input.cpp, nmake ASM_INPUT=1 assembles the original
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
