; asm_mfc.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN @CmdTarget_EndWaitCursor@4:PROC
EXTERN UnusedTrap:NEAR
EXTERN _Afx_GetModuleState@0:PROC
EXTERN _KingData:BYTE
EXTERN __CxxThrowException@8:PROC
EXTERN __imp__LeaveCriticalSection@4:PROC


_TEXT SEGMENT
$L_61c730 PROC

            call _Afx_GetModuleState@0

            mov ECX,DWORD PTR [EAX+4]
            jmp @CmdTarget_EndWaitCursor@4
$L_61c730 ENDP
$L_61c73d PROC

            mov EAX,DWORD PTR [ECX+4]
            mov ECX,DWORD PTR [ECX]
            mov DWORD PTR [EAX+196],ECX
            ret 
$L_61c73d ENDP
ALIGN 2
ALIGN 4
ALIGN 8
ALIGN 2
ALIGN 2
ALIGN 2
$L_61cb08 PROC

            mov EAX,DWORD PTR [EBP-20]
            add EAX,16
            push EAX
            call DWORD PTR __imp__LeaveCriticalSection@4

            push 0
            push 0
            call __CxxThrowException@8
$L_61cb08 ENDP
ALIGN 2
ALIGN 2
FUN_61cb62 PROC

            mov EAX,OFFSET UnusedTrap
            mov DWORD PTR [(_KingData+108984)],OFFSET UnusedTrap
            mov DWORD PTR [(_KingData+108980)],EAX
            mov DWORD PTR [(_KingData+108988)],OFFSET UnusedTrap
            mov DWORD PTR [(_KingData+108992)],OFFSET UnusedTrap
            mov DWORD PTR [(_KingData+108996)],OFFSET UnusedTrap
            mov DWORD PTR [(_KingData+109000)],EAX
            ret 
FUN_61cb62 ENDP
ALIGN 2
_TEXT ENDS
END
