; asm_crt.asm - the procedures of this original unit not yet rewritten in C++
INCLUDE king.inc

EXTERN $L_64a2a0:NEAR
EXTERN $L_64a304:NEAR
EXTERN @ThreadLocal_GetData@12:PROC
EXTERN @Wnd_Attach@12:PROC
EXTERN @Wnd_Construct@4:PROC
EXTERN @Wnd_Destruct@4:PROC
EXTERN @Wnd_Detach@4:PROC
EXTERN FUN_6317ac:NEAR
EXTERN _Afx_CriticalTerm:PROC
EXTERN _Afx_DeleteObject@4:PROC
EXTERN _Afx_EndThread@8:PROC
EXTERN _Afx_GetModuleState@0:PROC
EXTERN _Afx_GetThreadState:PROC
EXTERN _Afx_InitThread:PROC
EXTERN _Afx_TermLocalData@8:PROC
EXTERN _Afx_TlsRelease:PROC
EXTERN _KingBss:BYTE
EXTERN _KingData:BYTE
EXTERN _KingRdata:BYTE
EXTERN _ModuleThreadState_New:PROC
EXTERN _World_MathErr:PROC
EXTERN __CxxThrowException@8:PROC
EXTERN ___doserrno:PROC
EXTERN __errno:PROC
EXTERN __exit:PROC
EXTERN __imp__CloseHandle@4:PROC
EXTERN __imp__DeleteFileA@4:PROC
EXTERN __imp__EnterCriticalSection@4:PROC
EXTERN __imp__GetCurrentThreadId@0:PROC
EXTERN __imp__GetLastError@0:PROC
EXTERN __imp__GetModuleFileNameA@12:PROC
EXTERN __imp__GetProcAddress@8:PROC
EXTERN __imp__GetStdHandle@4:PROC
EXTERN __imp__HeapAlloc@12:PROC
EXTERN __imp__HeapFree@12:PROC
EXTERN __imp__HeapReAlloc@16:PROC
EXTERN __imp__InitializeCriticalSection@4:PROC
EXTERN __imp__IsBadCodePtr@4:PROC
EXTERN __imp__LeaveCriticalSection@4:PROC
EXTERN __imp__LoadLibraryA@4:PROC
EXTERN __imp__RaiseException@16:PROC
EXTERN __imp__SetEvent@4:PROC
EXTERN __imp__SetFilePointer@16:PROC
EXTERN __imp__SetLastError@4:PROC
EXTERN __imp__TlsGetValue@4:PROC
EXTERN __imp__TlsSetValue@8:PROC
EXTERN __imp__VirtualAlloc@16:PROC
EXTERN __imp__VirtualFree@12:PROC
EXTERN __imp__WaitForSingleObject@8:PROC
EXTERN __imp__WriteFile@20:PROC
EXTERN __setmbcp:PROC
EXTERN _abort:PROC
EXTERN _atexit:PROC
EXTERN _free:PROC
EXTERN _malloc:PROC

PUBLIC $L_61d0b8
PUBLIC $L_61d0e1, $L_61d0ff, $L_61d13a, $L_61d13c, $L_61d180
PUBLIC $L_61d62c, $L_61d63c, $L_61d674, $L_61d69e, $L_61d6be, $L_61d6e4, $L_61d71f
PUBLIC $L_61d721, $L_61d754, $L_61d770, $L_61d775, $L_61d782, $L_61d78a, $L_61d792, $L_61d7a4
PUBLIC $L_61d7ab, $L_61d7ae, $L_61d7b3, $L_61d7b9, $L_61d7c1, $L_61d7f7, $L_61d801, $L_61d80b
PUBLIC $L_61d80f, $L_61d817, $L_61d819, $L_61d821, $L_61d826
PUBLIC $L_61daf1
PUBLIC $L_61db0c, $L_61db44, $L_61db86, $L_61dbb2, $L_61dbbf, $L_61dbcd, $L_61dc1c, $L_61dc2c
PUBLIC $L_61dc6d, $L_61dcea, $L_61dcf4, $L_61dd05, $L_61dd19, $L_61dd20, $L_61dd69, $L_61dd6a
PUBLIC $L_61dda1, $L_61ddb8, $L_61ddd5, $L_61ddf6, $L_61ddfa, $L_61de01, $L_61de0f, $L_61de1c
PUBLIC $L_61e0c7, $L_61e0fc, $L_61e147, $L_61e14d, $L_61e169, $L_61e176
PUBLIC $L_61e196, $L_61e1b6
PUBLIC $L_61f0f4, $L_61f102, $L_61f11f, $L_61f134, $L_61f142, $L_61f15c
PUBLIC $L_61f164
PUBLIC $L_61f22c, $L_61f240, $L_61f273, $L_61f27d, $L_61f287, $L_61f291, $L_61f2bd, $L_61f2cc
PUBLIC $L_61f2ff, $L_61f304, $L_61f309, $L_61f30e, $L_61f311, $L_61f31d, $L_61f331, $L_61f336
PUBLIC $L_61f36a, $L_61f372, $L_61f37f, $L_61f388, $L_61f3f5, $L_61f406, $L_61f416, $L_61f424
PUBLIC $L_61f441, $L_61f5e3, $L_61f5e9, $L_61f607, $L_61f60d, $L_61f613, $L_61f705
PUBLIC $L_61f70e, $L_61f72b, $L_61f742, $L_61f749, $L_61f75e, $L_61f76b, $L_61f7c2, $L_61f7ca
PUBLIC $L_61f7e7, $L_61f7ef, $L_61f7fd, $L_61f804, $L_61f81f
PUBLIC $L_61fbf6, $L_61fc03, $L_61fc08
PUBLIC $L_61fd58, $L_61fdf2, $L_61fe00, $L_61fe1d, $L_61fe32, $L_61fe40, $L_61fe5a, $L_61fe62
PUBLIC $L_61ffe0, $L_6201cd, $L_6201ea, $L_6201ec, $L_6201ed, $L_620215, $L_62021a, $L_620222
PUBLIC $L_620249
PUBLIC $L_620d0d, $L_620d93, $L_620db9, $L_620dc9
PUBLIC $L_620de6, $L_622bf6, $L_622bfe, $L_622cfd, $L_622d84, $L_622da1, $L_622da3
PUBLIC $L_623373, $L_623383, $L_623391, $L_6233aa, $L_6233b1, $L_6233cb, $L_6233cd, $L_6233cf
PUBLIC $L_6233e2, $L_6233e5, $L_6233e9, $L_6233fb, $L_6233fd, $L_62340d, $L_623421, $L_623428
PUBLIC $L_62344f, $L_62346c, $L_623483, $L_623486, $L_6234af, $L_6234b8, $L_6234d5, $L_623515
PUBLIC $L_623543, $L_623546, $L_6235a6, $L_6235b8, $L_6235d1, $L_6235e7, $L_6235ea, $L_6235f6
PUBLIC $L_6235f9, $L_62362f, $L_623637, $L_62367f, $L_6236cf, $L_6236d3, $L_6236ea, $L_6236ff
PUBLIC $L_623708, $L_62371d, $L_623750, $L_62375d, $L_623796, $L_6237d3, $L_6237e3, $L_623af2
PUBLIC $L_623b0f, $L_623b5e, $L_623b6e, $L_623b83, $L_623ba5, $L_623bce, $L_623bf2, $L_623bf6
PUBLIC $L_623c04, $L_623c1b, $L_623c1d, $L_623de4, $L_623e0f, $L_623e32, $L_623e49, $L_623e4c
PUBLIC $L_623e5f, $L_623e7e, $L_623e8f, $L_623e92, $L_623ea1, $L_623eb1, $L_623eb8, $L_623ec4
PUBLIC $L_623ed0, $L_623eee, $L_623eff, $L_623f3c, $L_623f66, $L_623f72, $L_623f80, $L_623f82
PUBLIC $L_623fa5, $L_623fd7, $L_623fd9, $L_624012, $L_62401b, $L_624023, $L_62402c, $L_624033
PUBLIC $L_62403f, $L_624048, $L_624056, $L_624062, $L_624069, $L_62406e, $L_624076, $L_624079
PUBLIC $L_624091, $L_62409a, $L_6240ab, $L_6240b4, $L_6240bb, $L_6240cb, $L_6240d2, $L_6240e3
PUBLIC $L_6240ec, $L_6240f1, $L_6240fb, $L_6240fd, $L_624203, $L_62421b, $L_624221, $L_62422a
PUBLIC $L_624254, $L_624287, $L_6242db, $L_6242e6, $L_6242ed, $L_6242f4, $L_624309
PUBLIC $L_624646, $L_62467f, $L_62469d, $L_6246a4, $L_6246dc
PUBLIC $L_6246e3, $L_6246e8, $L_624700, $L_624709, $L_624730, $L_624761, $L_6247a9, $L_6247c7
PUBLIC $L_624809, $L_624819, $L_624840, $L_624843, $L_62486d, $L_624892, $L_62489e, $L_6248a5
PUBLIC $L_6248a8, $L_6248d3, $L_624919, $L_62494c, $L_62495f, $L_624987, $L_6249a3, $L_6249ab
PUBLIC $L_6249b9, $L_6249bd, $L_6249f3, $L_6249fb, $L_624a16, $L_624a18, $L_624a20, $L_624a22
PUBLIC $L_624a27, $L_624a88, $L_624a8a, $L_624a8d, $L_624c8b, $L_624c96, $L_624c98, $L_624d29
PUBLIC $L_624d35, $L_624d3a, $L_624d4c, $L_624d7b, $L_624fe1, $L_624ff7, $L_625002, $L_625016
PUBLIC $L_625025, $L_625056, $L_62508d, $L_6250d0, $L_62512d, $L_625153
PUBLIC $L_6254ee
PUBLIC $L_6254f5, $L_625525, $L_625545, $L_62554b, $L_62556a, $L_62557f, $L_6255bb, $L_6255bd
PUBLIC $L_6255e9, $L_6255f4, $L_625616, $L_625621, $L_62562a, $L_62562f, $L_625644, $L_625658
PUBLIC $L_62565b, $L_6256e4, $L_625704, $L_625706, $L_625711, $L_625716, $L_625735, $L_6257f4
PUBLIC $L_625851, $L_62588f, $L_6258dd, $L_6258f0, $L_625902, $L_625915, $L_625928, $L_6259b1
PUBLIC $L_6259bc, $L_6259c7, $L_6259d1, $L_6259db, $L_625a00, $L_625a0c, $L_625a16, $L_625a1a
PUBLIC $L_625a20, $L_625a3e, $L_625a4b, $L_625a56, $L_625a58, $L_625ad2, $L_625adb, $L_625ae4
PUBLIC $L_625aed, $L_625af5, $L_625b13, $L_625b1d, $L_625b25, $L_625b29, $L_625b2b, $L_625b42
PUBLIC $L_625b4b, $L_625b52, $L_625b54, $L_625b89, $L_625ba3, $L_625bfd, $L_625c05, $L_625c1d
PUBLIC $L_625c25, $L_625c2d, $L_625c45, $L_625c4d, $L_625c55, $L_625c6d, $L_625c73, $L_625c75
PUBLIC $L_625c7d, $L_625c98, $L_625ce3, $L_625cf5, $L_625cf7, $L_625d0e, $L_625d19, $L_625d25
PUBLIC $L_625d2b, $L_625d37, $L_625d41, $L_625d43, $L_625d50, $L_625d53, $L_625d6a, $L_625dda
PUBLIC $L_625de0, $L_625e19, $L_625e24, $L_625e2c, $L_625e42, $L_625e56, $L_625e5e, $L_625e66
PUBLIC $L_625e6c, $L_625e6e, $L_625ea5, $L_625ea8, $L_625eac, $L_625eb0, $L_625eb7, $L_625ed2
PUBLIC $L_625ee1, $L_625efc, $L_625f49, $L_625f5b, $L_625f6e, $L_625f83, $L_625f85, $L_625f88
PUBLIC $L_625fac, $L_625fc4, $L_625fdc, $L_625fde, $L_625ff1, $L_625ff7, $L_626005, $L_62601b
PUBLIC $L_626042, $L_6260a4, $L_6260b9, $L_6260c8, $L_6260d6, $L_6260e1, $L_6263fb, $L_626405
PUBLIC $L_62640b, $L_626455, $L_626456, $L_626468
PUBLIC $L_627000
PUBLIC $L_62705e, $L_62706e, $L_6270c8, $L_6270d5, $L_627110, $L_62711a, $L_62714d, $L_627178
PUBLIC $L_62719e, $L_6271f9, $L_627219, $L_627227, $L_627df1, $L_627df3, $L_628da8, $L_628e05
PUBLIC $L_628e0d, $L_629391, $L_629451, $L_629459, $L_62945d, $L_629461, $L_629465
PUBLIC $L_629467, $L_629492, $L_6294a4, $L_6294bb, $L_6294e3, $L_6294eb, $L_62957d, $L_62959c
PUBLIC $L_6295af, $L_6295b3, $L_631800, $L_63181e, $L_631871, $L_631893, $L_631898
PUBLIC $L_631d33, $L_631d35
PUBLIC $L_631d44, $L_632649, $L_632667, $L_63291b, $L_63294a, $L_63295e
PUBLIC $L_632961

_TEXT SEGMENT
FUN_61d0a0 PROC

            push EBP
            mov EBP,ESP
            push EBX
            push ESI
            push EDI
            push EBP
            push 0
            push 0
            push OFFSET $L_61d0b8
            push DWORD PTR [EBP+8]
            call FUN_6317ac
$L_61d0b8::

            pop EBP
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
FUN_61d0a0 ENDP
$L_61d0c0 PROC

            mov ECX,DWORD PTR [ESP+4]
            test DWORD PTR [ECX+4],6
            mov EAX,1
            je $L_61d0e1

            mov EAX,DWORD PTR [ESP+8]
            mov EDX,DWORD PTR [ESP+16]
            mov DWORD PTR [EDX],EAX
            mov EAX,3
$L_61d0e1::

            ret 
$L_61d0c0 ENDP
ALIGN 2
FUN_61d0e2 PROC

            push EBX
            push ESI
            push EDI
            mov EAX,DWORD PTR [ESP+16]
            push EAX
            push -2
            push OFFSET $L_61d0c0
            push DWORD PTR FS:[0]
            mov DWORD PTR FS:[0],ESP
$L_61d0ff::

            mov EAX,DWORD PTR [ESP+32]
            mov EBX,DWORD PTR [EAX+8]
            mov ESI,DWORD PTR [EAX+12]
            cmp ESI,-1
            je $L_61d13c

            cmp ESI,DWORD PTR [ESP+36]
            je $L_61d13c

            lea ESI,DWORD PTR [ESI+ESI*2]
            mov ECX,DWORD PTR [EBX+ESI*4]
            mov DWORD PTR [ESP+8],ECX
            mov DWORD PTR [EAX+12],ECX
            cmp DWORD PTR [EBX+ESI*4+4],0
            jne $L_61d13a

            push 257
            mov EAX,DWORD PTR [EBX+ESI*4+8]
            call FUN_61d176

            call DWORD PTR [EBX+ESI*4+8]
$L_61d13a::

            jmp $L_61d0ff
$L_61d13c::

            pop DWORD PTR FS:[0]
            add ESP,12
            pop EDI
            pop ESI
            pop EBX
            ret 
FUN_61d0e2 ENDP
ALIGN 2
FUN_61d176 PROC

            push EBX
            push ECX
            mov EBX,OFFSET (_KingData+107276)
            mov ECX,DWORD PTR [EBP+8]
$L_61d180::

            mov DWORD PTR [EBX+8],ECX
            mov DWORD PTR [EBX+4],EAX
            mov DWORD PTR [EBX+12],EBP
            pop ECX
            pop EBX
            ret 4
FUN_61d176 ENDP
ALIGN 2
ALIGN 8
ALIGN 2
$L_61d2b1 PROC

            push 25
            call FUN_61ffd2

            pop ECX
            ret 
$L_61d2b1 ENDP
??_M@YGXPAXIHP6EX0@Z@Z PROC ;FUN_61d5f5 void __stdcall `eh vector destructor iterator'(void *ptr, unsigned size, int count, void (__thiscall *dtor)(void *))

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET (_KingRdata+38688)
            push OFFSET $L_624254
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,12
            push EBX
            push ESI
            push EDI
            and DWORD PTR [EBP-28],0
            mov ESI,DWORD PTR [EBP+12]
            mov EAX,ESI
            imul EAX,DWORD PTR [EBP+16]
            add DWORD PTR [EBP+8],EAX
            and DWORD PTR [EBP-4],0
$L_61d62c::

            dec DWORD PTR [EBP+16]
            js $L_61d63c

            sub DWORD PTR [EBP+8],ESI
            mov ECX,DWORD PTR [EBP+8]
            call DWORD PTR [EBP+20]

            jmp $L_61d62c
$L_61d63c::

            mov DWORD PTR [EBP-28],1
            or DWORD PTR [EBP-4],4294967295
            call FUN_61d65d

            mov ECX,DWORD PTR [EBP-16]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 16
??_M@YGXPAXIHP6EX0@Z@Z ENDP
FUN_61d65d PROC

            cmp DWORD PTR [EBP-28],0
            jne $L_61d674

            push DWORD PTR [EBP+20]
            push DWORD PTR [EBP+16]
            push DWORD PTR [EBP+12]
            push DWORD PTR [EBP+8]
            call FUN_61d675
$L_61d674::

            ret 
FUN_61d65d ENDP
FUN_61d675 PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET (_KingRdata+38704)
            push OFFSET $L_624254
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ECX
            push ECX
            push EBX
            push ESI
            push EDI
            mov DWORD PTR [EBP-24],ESP
            and DWORD PTR [EBP-4],0
$L_61d69e::

            dec DWORD PTR [EBP+16]
            js $L_61d6be

            mov ECX,DWORD PTR [EBP+8]
            sub ECX,DWORD PTR [EBP+12]
            mov DWORD PTR [EBP+8],ECX
            call DWORD PTR [EBP+20]

            jmp $L_61d69e
FUN_61d675 ENDP
$L_61d6b1 PROC

            push DWORD PTR [EBP-20]
            call FUN_61d6d3

            pop ECX
            ret 
$L_61d6b1 ENDP
$L_61d6bb PROC

            mov ESP,DWORD PTR [EBP-24]
$L_61d6be::

            or DWORD PTR [EBP-4],4294967295
            mov ECX,DWORD PTR [EBP-16]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 16
$L_61d6bb ENDP
FUN_61d6d3 PROC

            mov EAX,DWORD PTR [ESP+4]
            mov EAX,DWORD PTR [EAX]
            cmp DWORD PTR [EAX],3765269347
            je $L_61d6e4

            xor EAX,EAX
            ret 
$L_61d6e4::

            jmp FUN_622caa
FUN_61d6d3 ENDP
_strncmp PROC ;FUN_61d6f0 int strncmp(const char *string1, const char *string2, size_t count)

            push EBP
            mov EBP,ESP
            push EDI
            push ESI
            push EBX
            mov ECX,DWORD PTR [EBP+16]
            jecxz $L_61d721

            mov EBX,ECX
            mov EDI,DWORD PTR [EBP+8]
            mov ESI,EDI
            xor EAX,EAX

            repne scasb 

            neg ECX
            add ECX,EBX
            mov EDI,ESI
            mov ESI,DWORD PTR [EBP+12]

            repe cmpsb BYTE PTR [ESI],BYTE PTR ES:[EDI]

            mov AL,BYTE PTR [ESI-1]
            xor ECX,ECX
            cmp AL,BYTE PTR [EDI-1]
            ja $L_61d71f

            je $L_61d721

            dec ECX
            dec ECX
$L_61d71f::

            not ECX
$L_61d721::

            mov EAX,ECX
            pop EBX
            pop ESI
            pop EDI
            leave 
            ret 
_strncmp ENDP
_strncpy PROC ;FUN_61d730 char *strncpy(char *dest, const char *src, size_t count)

            mov ECX,DWORD PTR [ESP+12]
            push EDI
            test ECX,ECX
            je $L_61d7b3

            push ESI
            push EBX
            mov EBX,ECX
            mov ESI,DWORD PTR [ESP+20]
            test ESI,3
            mov EDI,DWORD PTR [ESP+16]
            jne $L_61d754

            shr ECX,2
            jne $L_61d7c1

            jmp $L_61d775
$L_61d754::

            mov AL,BYTE PTR [ESI]
            inc ESI
            mov BYTE PTR [EDI],AL
            inc EDI
            dec ECX
            je $L_61d782

            test AL,AL
            je $L_61d78a

            test ESI,3
            jne $L_61d754

            mov EBX,ECX
            shr ECX,2
            jne $L_61d7c1
$L_61d770::

            and EBX,3
            je $L_61d782
$L_61d775::

            mov AL,BYTE PTR [ESI]
            inc ESI
            mov BYTE PTR [EDI],AL
            inc EDI
            test AL,AL
            je $L_61d7ae

            dec EBX
            jne $L_61d775
$L_61d782::

            mov EAX,DWORD PTR [ESP+16]
            pop EBX
            pop ESI
            pop EDI
            ret 
$L_61d78a::

            test EDI,3
            je $L_61d7a4
$L_61d792::

            mov BYTE PTR [EDI],AL
            inc EDI
            dec ECX
            je $L_61d826

            test EDI,3
            jne $L_61d792
$L_61d7a4::

            mov EBX,ECX
            shr ECX,2
            jne $L_61d817
$L_61d7ab::

            mov BYTE PTR [EDI],AL
            inc EDI
$L_61d7ae::

            dec EBX
            jne $L_61d7ab

            pop EBX
            pop ESI
$L_61d7b3::

            mov EAX,DWORD PTR [ESP+8]
            pop EDI
            ret 
$L_61d7b9::

            mov DWORD PTR [EDI],EDX
            add EDI,4
            dec ECX
            je $L_61d770
$L_61d7c1::

            mov EDX,2130640639
            mov EAX,DWORD PTR [ESI]
            add EDX,EAX
            xor EAX,4294967295
            xor EAX,EDX
            mov EDX,DWORD PTR [ESI]
            add ESI,4
            test EAX,2164326656
            je $L_61d7b9

            test DL,DL
            je $L_61d80b

            test DH,DH
            je $L_61d801

            test EDX,16711680
            je $L_61d7f7

            test EDX,4278190080
            jne $L_61d7b9

            mov DWORD PTR [EDI],EDX
            jmp $L_61d80f
$L_61d7f7::

            and EDX,65535
            mov DWORD PTR [EDI],EDX
            jmp $L_61d80f
$L_61d801::

            and EDX,255
            mov DWORD PTR [EDI],EDX
            jmp $L_61d80f
$L_61d80b::

            xor EDX,EDX
            mov DWORD PTR [EDI],EDX
$L_61d80f::

            add EDI,4
            xor EAX,EAX
            dec ECX
            je $L_61d821
$L_61d817::

            xor EAX,EAX
$L_61d819::

            mov DWORD PTR [EDI],EAX
            add EDI,4
            dec ECX
            jne $L_61d819
$L_61d821::

            and EBX,3
            jne $L_61d7ab
$L_61d826::

            mov EAX,DWORD PTR [ESP+16]
            pop EBX
            pop ESI
            pop EDI
            ret 
_strncpy ENDP
__ftol PROC ;FUN_61d830 __int64 _ftol(void): ST(0) truncated to EDX:EAX

            push EBP
            mov EBP,ESP
            add ESP,-12
            wait 
            fnstcw WORD PTR [EBP-2]
            wait 
            mov AX,WORD PTR [EBP-2]
            or AH,12
            mov WORD PTR [EBP-4],AX
            fldcw WORD PTR [EBP-4]
            fistp QWORD PTR [EBP-12]
            fldcw WORD PTR [EBP-2]
            mov EAX,DWORD PTR [EBP-12]
            mov EDX,DWORD PTR [EBP-8]
            leave 
            ret 
__ftol ENDP
??_L@YGXPAXIHP6EX0@Z1@Z PROC ;FUN_61dac3 void __stdcall `eh vector constructor iterator'(void *ptr, unsigned size, int count, void (__thiscall *ctor)(void *), void (__thiscall *dtor)(void *))

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET (_KingRdata+38744)
            push OFFSET $L_624254
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,16
            push EBX
            push ESI
            push EDI
            xor EAX,EAX
            mov DWORD PTR [EBP-32],EAX
            mov DWORD PTR [EBP-4],EAX
            mov DWORD PTR [EBP-28],EAX
$L_61daf1::

            mov EAX,DWORD PTR [EBP-28]
            cmp EAX,DWORD PTR [EBP+16]
            jge $L_61db0c

            mov ESI,DWORD PTR [EBP+8]
            mov ECX,ESI
            call DWORD PTR [EBP+20]

            add ESI,DWORD PTR [EBP+12]
            mov DWORD PTR [EBP+8],ESI
            inc DWORD PTR [EBP-28]
            jmp $L_61daf1
$L_61db0c::

            mov DWORD PTR [EBP-32],1
            or DWORD PTR [EBP-4],4294967295
            call FUN_61db2d

            mov ECX,DWORD PTR [EBP-16]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 20
??_L@YGXPAXIHP6EX0@Z1@Z ENDP
FUN_61db2d PROC

            cmp DWORD PTR [EBP-32],0
            jne $L_61db44

            push DWORD PTR [EBP+24]
            push DWORD PTR [EBP-28]
            push DWORD PTR [EBP+12]
            push DWORD PTR [EBP+8]
            call FUN_61d675
$L_61db44::

            ret 
FUN_61db2d ENDP
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
ALIGN 16
__CIpow PROC ;FUN_61db50 double _CIpow(void): pow(ST(1), ST(0))

            sub ESP,16
            fxch ST(1)
            fstp QWORD PTR [ESP]
            fst QWORD PTR [ESP+8]
            mov EAX,DWORD PTR [ESP+12]
            call FUN_61db72
__CIpow ENDP
$L_61db65 PROC

            add ESP,16
            ret 
$L_61db65 ENDP
FUN_61db72 PROC

            mov ECX,EAX
            push EAX
            wait 
            fnstcw WORD PTR [ESP]
            cmp WORD PTR [ESP],639
            je $L_61db86

            call FUN_624785
$L_61db86::

            and ECX,2146435072
            lea EDX,DWORD PTR [ESP+8]
            cmp ECX,2146435072
            je $L_61dc39

            call FUN_6247b5
FUN_61db72 ENDP
$L_61dba1 PROC

            je $L_61dc35

            test EAX,2146435072
            je $L_61dca8
$L_61dbb2::

            mov CL,BYTE PTR [ESP+15]
            and CL,128
            jne $L_61dd20
$L_61dbbf::

            fyl2x 
            call FUN_624770

            cmp CL,1
            jne $L_61dbcd

            fchs 
$L_61dbcd::

            cmp DWORD PTR [(_KingBss+669944)],0
            jne $L_62480e

            lea ECX,DWORD PTR [(_KingData+107304)]
            mov EDX,29
            jmp $L_624859
$L_61dba1 ENDP
$L_61dbea PROC

            cmp DWORD PTR [(_KingBss+669944)],0
            jne $L_62480e

            lea ECX,DWORD PTR [(_KingData+107304)]
            mov EDX,29
            call FUN_624710

            pop EDX
            ret 
$L_61dbea ENDP
$L_61dc09 PROC

            lea EDX,DWORD PTR [ESP+8]
            call FUN_6247b5
$L_61dc09 ENDP
$L_61dc12 PROC

            test BYTE PTR [ESP+22],8
            jne $L_61dc1c

            inc ECX
            jmp $L_61dc4f
$L_61dc1c::

            faddp ST(1),ST(0)
            mov EAX,1
            jmp $L_61dbea
$L_61dc12 ENDP
$L_61dc25 PROC

            test BYTE PTR [ESP+14],8
            jne $L_61dc1c
$L_61dc2c::

            faddp ST(1),ST(0)
            mov EAX,7
            jmp $L_61dbea
$L_61dc25 ENDP
$L_61dc35 PROC

            xor ECX,ECX
            jmp $L_61dc4f
$L_61dc35 ENDP
$L_61dc39 PROC

            xor ECX,ECX
            and EAX,1048575
            or EAX,DWORD PTR [ESP+16]
            jne $L_61dc09

            lea EDX,DWORD PTR [ESP+8]
            call FUN_6247b5
$L_61dc39 ENDP
$L_61dc4f PROC

            mov EAX,DWORD PTR [ESP+12]
            mov EDX,EAX
            and EAX,2146435072
            and EDX,1048575
            cmp EAX,2146435072
            jne $L_61dc6d

            or EDX,DWORD PTR [ESP+8]
            jne $L_61dc25
$L_61dc6d::

            test ECX,ECX
            jne $L_61dc2c

            sub ESP,116
            mov ECX,ESP
            push ECX
            sub ESP,16
            fstp QWORD PTR [ESP]
            fstp QWORD PTR [ESP+8]
            wait 
            fnsave DWORD PTR [ECX+8]
            call FUN_6248fc

            add ESP,16
            pop ECX
            frstor DWORD PTR [ECX+8]
            fld QWORD PTR [ECX]
            add ESP,116
            test EAX,EAX
            je $L_62480e

            mov EAX,1
            jmp $L_61dbea
$L_61dc4f ENDP
$L_61dca8 PROC

            mov EAX,DWORD PTR [ESP+12]
            and EAX,1048575
            or EAX,DWORD PTR [ESP+8]
            jne $L_61dbb2

            fstp ST(0)
            mov EAX,DWORD PTR [ESP+20]
            test EAX,2146435072
            je $L_61dd05

            call FUN_61dd45

            mov CH,BYTE PTR [ESP+15]
            shr CH,7
            test DWORD PTR [ESP+23],128
            je $L_61dcf4

            fld TBYTE PTR [(_KingData+117472)]
            test CH,CL
            je $L_61dcea

            fchs 
$L_61dcea::

            mov EAX,2
            jmp $L_61dbea
$L_61dcf4::

            fldz 
            test CH,CL
            je $L_62480e

            fchs 
            jmp $L_62480e
$L_61dd05::

            fstp ST(0)
            and EAX,1048575
            or EAX,DWORD PTR [ESP+16]
            jne $L_61dd19

            fld1 
            jmp $L_62480e
$L_61dd19::

            fldz 
            jmp $L_62480e
$L_61dd20::

            fld ST(1)
            call FUN_61dd45

            fchs 
            test CL,CL
            jne $L_61dbbf

            fstp ST(0)
            fstp ST(0)
            fld TBYTE PTR [(_KingData+117568)]
            mov EAX,1
            jmp $L_61dbea
$L_61dca8 ENDP
FUN_61dd45 PROC

            fld ST(0)
            frndint 
            fcomp ST(1)
            mov CL,0
            wait 
            fnstsw AX
            sahf 
            jne $L_61dd6a

            fmul QWORD PTR [(_KingData+107296)]
            inc CL
            fld ST(0)
            frndint 
            fcompp 
            wait 
            fnstsw AX
            sahf 
            jne $L_61dd69

            inc CL
$L_61dd69::

            ret 
$L_61dd6a::

            fstp ST(0)
            ret 
FUN_61dd45 ENDP
__CIacos PROC ;FUN_61dd70 double _CIacos(void): acos(ST(0))

            sub ESP,8
            fst QWORD PTR [ESP]
            call FUN_6247f8

            call FUN_61dd8d

            add ESP,8
            ret 
__CIacos ENDP
FUN_61dd8d PROC

            push EDX
            wait 
            fnstcw WORD PTR [ESP]
            je $L_61de01

            cmp WORD PTR [ESP],639
            je $L_61dda1

            call FUN_624785
$L_61dda1::

            cmp EAX,1072693248
            jae $L_61ddd5

            fld1 
            fadd ST(0),ST(1)
            fld1 
            fsub ST(0),ST(2)
            fmulp ST(1),ST(0)
            fsqrt 
            fxch ST(1)
            fpatan 
$L_61ddb8::

            cmp DWORD PTR [(_KingBss+669944)],0
            jne $L_62480e

            mov EDX,13
            lea ECX,DWORD PTR [(_KingData+107312)]
            jmp $L_62481b
$L_61ddd5::

            ja $L_61de0f

            mov EAX,DWORD PTR [ESP+12]
            mov ECX,EAX
            and EAX,1048575
            or EAX,DWORD PTR [ESP+8]
            jne $L_61de0f

            and ECX,2147483648
            fstp ST(0)
            je $L_61ddf6

            fldpi 
            jmp $L_61ddb8
$L_61ddf6::

            fldz 
            jmp $L_61ddb8
$L_61ddfa::

            call FUN_62479c

            jmp $L_61de1c
$L_61de01::

            test EAX,1048575
            jne $L_61ddfa

            cmp DWORD PTR [ESP+8],0
            jne $L_61ddfa
$L_61de0f::

            fstp ST(0)
            fld TBYTE PTR [(_KingData+117568)]
            mov EAX,1
$L_61de1c::

            cmp DWORD PTR [(_KingBss+669944)],0
            jne $L_62480e

            mov EDX,13
            lea ECX,DWORD PTR [(_KingData+107312)]
            call FUN_624727

            pop EDX
            ret 
FUN_61dd8d ENDP
??0exception@@QAE@XZ PROC ;FUN_61e0a0 exception::exception()

            mov EAX,ECX
            and DWORD PTR [EAX+4],0
            and DWORD PTR [EAX+8],0
            mov DWORD PTR [EAX],OFFSET (_KingRdata+38760)
            ret 
??0exception@@QAE@XZ ENDP
$L_61e0b1 PROC

            push ESI
            mov ESI,ECX
            call ??1exception@@UAE@XZ

            test BYTE PTR [ESP+8],1
            je $L_61e0c7

            push ESI
            call ??3@YAXPAX@Z

            pop ECX
$L_61e0c7::

            mov EAX,ESI
            pop ESI
            ret 4
$L_61e0b1 ENDP
??0exception@@QAE@ABQBD@Z PROC ;FUN_61e0cd exception::exception(const char *const &what)

            push ESI
            push EDI
            mov EDI,DWORD PTR [ESP+12]
            mov ESI,ECX
            mov DWORD PTR [ESI],OFFSET (_KingRdata+38760)
            push DWORD PTR [EDI]
            call _strlen

            inc EAX
            push EAX
            call ??2@YAPAXI@Z

            pop ECX
            mov DWORD PTR [ESI+4],EAX
            test EAX,EAX
            pop ECX
            je $L_61e0fc

            push DWORD PTR [EDI]
            push EAX
            call _strcpy

            pop ECX
            pop ECX
$L_61e0fc::

            mov DWORD PTR [ESI+8],1
            mov EAX,ESI
            pop EDI
            pop ESI
            ret 4
??0exception@@QAE@ABQBD@Z ENDP
ALIGN 2
??0exception@@QAE@ABV0@@Z PROC ;FUN_61e10a exception::exception(const exception &)

            push ESI
            push EDI
            mov EDI,DWORD PTR [ESP+12]
            mov ESI,ECX
            mov DWORD PTR [ESI],OFFSET (_KingRdata+38760)
            mov EAX,DWORD PTR [EDI+8]
            test EAX,EAX
            mov DWORD PTR [ESI+8],EAX
            je $L_61e147

            push DWORD PTR [EDI+4]
            call _strlen

            inc EAX
            push EAX
            call ??2@YAPAXI@Z

            pop ECX
            mov DWORD PTR [ESI+4],EAX
            test EAX,EAX
            pop ECX
            je $L_61e14d

            push DWORD PTR [EDI+4]
            push EAX
            call _strcpy

            pop ECX
            pop ECX
            jmp $L_61e14d
$L_61e147::

            mov EAX,DWORD PTR [EDI+4]
            mov DWORD PTR [ESI+4],EAX
$L_61e14d::

            mov EAX,ESI
            pop EDI
            pop ESI
            ret 4
??0exception@@QAE@ABV0@@Z ENDP
ALIGN 4
??1exception@@UAE@XZ PROC ;FUN_61e154 virtual exception::~exception()

            cmp DWORD PTR [ECX+8],0
            mov DWORD PTR [ECX],OFFSET (_KingRdata+38760)
            je $L_61e169

            push DWORD PTR [ECX+4]
            call ??3@YAXPAX@Z

            pop ECX
$L_61e169::

            ret 
??1exception@@UAE@XZ ENDP
$L_61e16a PROC

            mov EAX,DWORD PTR [ECX+4]
            test EAX,EAX
            jne $L_61e176

            mov EAX,OFFSET (_KingRdata+38768)
$L_61e176::

            ret 
$L_61e16a ENDP
FUN_61e177 PROC

            push ESI
            mov ESI,ECX
            push 27
            mov DWORD PTR [ESI],OFFSET (_KingRdata+38792)
            call FUN_6241d4

            mov ESI,DWORD PTR [ESI+4]
            pop ECX
            test ESI,ESI
            je $L_61e196

            push ESI
            call _free

            pop ECX
$L_61e196::

            push 27
            call FUN_624235

            pop ECX
            pop ESI
            ret 
FUN_61e177 ENDP
$L_61e1a0 PROC

            push ESI
            mov ESI,ECX
            call FUN_61e177

            test BYTE PTR [ESP+8],1
            je $L_61e1b6

            push ESI
            call ??3@YAXPAX@Z

            pop ECX
$L_61e1b6::

            mov EAX,ESI
            pop ESI
            ret 4
$L_61e1a0 ENDP
ALIGN 4
ALIGN 2
_floor PROC ;FUN_61f099 double floor(double x)

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            push EBX
            push ESI
            mov ESI,65535
            push ESI
            push DWORD PTR [(_KingData+108024)]
            call FUN_62606a

            fld QWORD PTR [EBP+8]
            pop ECX
            mov EBX,EAX
            mov EAX,DWORD PTR [EBP+14]
            pop ECX
            push ECX
            and AX,32752
            push ECX
            cmp AX,32752
            fstp QWORD PTR [ESP]
            jne $L_61f11f

            call FUN_625f32

            pop ECX
            test EAX,EAX
            pop ECX
            jle $L_61f102

            cmp EAX,2
            jle $L_61f0f4

            cmp EAX,3
            jne $L_61f102

            fld QWORD PTR [EBP+8]
            push EBX
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push 11
            call FUN_6257bf

            add ESP,16
            jmp $L_61f164
$L_61f0f4::

            push ESI
            push EBX
            call FUN_62606a

            fld QWORD PTR [EBP+8]
            pop ECX
            pop ECX
            jmp $L_61f164
$L_61f102::

            fld QWORD PTR [EBP+8]
            fadd QWORD PTR [(_KingRdata+1648)]
            push EBX
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push 11
            push 8
            jmp $L_61f15c
$L_61f11f::

            call FUN_6257ad

            fstp QWORD PTR [EBP-8]
            fld QWORD PTR [EBP-8]
            fcomp QWORD PTR [EBP+8]
            pop ECX
            pop ECX
            fnstsw AX
            sahf 
            jne $L_61f142
$L_61f134::

            push ESI
            push EBX
            call FUN_62606a

            fld QWORD PTR [EBP-8]
            pop ECX
            pop ECX
            jmp $L_61f164
$L_61f142::

            test BL,32
            jne $L_61f134

            fld QWORD PTR [EBP-8]
            push EBX
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push 11
            push 16
$L_61f15c::

            call FUN_625813

            add ESP,28
$L_61f164::

            pop ESI
            pop EBX
            leave 
            ret 
_floor ENDP
_strlen PROC ;FUN_61f220 size_t strlen(const char *string)

            mov ECX,DWORD PTR [ESP+4]
            test ECX,3
            je $L_61f240
$L_61f22c::

            mov AL,BYTE PTR [ECX]
            inc ECX
            test AL,AL
            je $L_61f273

            test ECX,3
            jne $L_61f22c

            add EAX,0
$L_61f240::

            mov EAX,DWORD PTR [ECX]
            mov EDX,2130640639
            add EDX,EAX
            xor EAX,4294967295
            xor EAX,EDX
            add ECX,4
            test EAX,2164326656
            je $L_61f240

            mov EAX,DWORD PTR [ECX-4]
            test AL,AL
            je $L_61f291

            test AH,AH
            je $L_61f287

            test EAX,16711680
            je $L_61f27d

            test EAX,4278190080
            je $L_61f273

            jmp $L_61f240
$L_61f273::

            lea EAX,DWORD PTR [ECX-1]
            mov ECX,DWORD PTR [ESP+4]
            sub EAX,ECX
            ret 
$L_61f27d::

            lea EAX,DWORD PTR [ECX-2]
            mov ECX,DWORD PTR [ESP+4]
            sub EAX,ECX
            ret 
$L_61f287::

            lea EAX,DWORD PTR [ECX-3]
            mov ECX,DWORD PTR [ESP+4]
            sub EAX,ECX
            ret 
$L_61f291::

            lea EAX,DWORD PTR [ECX-4]
            mov ECX,DWORD PTR [ESP+4]
            sub EAX,ECX
            ret 
_strlen ENDP
_strcpy PROC ;FUN_61f2a0 char *strcpy(char *dest, const char *src)

            push EDI
            mov EDI,DWORD PTR [ESP+8]
            jmp $L_61f311
_strcpy ENDP
          BYTE 08dH
          BYTE 0a4H
          BYTE 024H
          BYTE 000H
          BYTE 000H
          BYTE 000H
          BYTE 000H
          BYTE 08bH
          BYTE 0ffH
ALIGN 16
FUN_61f2b0 PROC

            mov ECX,DWORD PTR [ESP+4]
            push EDI
            test ECX,3
            je $L_61f2cc
$L_61f2bd::

            mov AL,BYTE PTR [ECX]
            inc ECX
            test AL,AL
            je $L_61f2ff

            test ECX,3
            jne $L_61f2bd
$L_61f2cc::

            mov EAX,DWORD PTR [ECX]
            mov EDX,2130640639
            add EDX,EAX
            xor EAX,4294967295
            xor EAX,EDX
            add ECX,4
            test EAX,2164326656
            je $L_61f2cc

            mov EAX,DWORD PTR [ECX-4]
            test AL,AL
            je $L_61f30e

            test AH,AH
            je $L_61f309

            test EAX,16711680
            je $L_61f304

            test EAX,4278190080
            je $L_61f2ff

            jmp $L_61f2cc
$L_61f2ff::

            lea EDI,DWORD PTR [ECX-1]
            jmp $L_61f311
$L_61f304::

            lea EDI,DWORD PTR [ECX-2]
            jmp $L_61f311
$L_61f309::

            lea EDI,DWORD PTR [ECX-3]
            jmp $L_61f311
$L_61f30e::

            lea EDI,DWORD PTR [ECX-4]
$L_61f311::

            mov ECX,DWORD PTR [ESP+12]
            test ECX,3
            je $L_61f336
$L_61f31d::

            mov DL,BYTE PTR [ECX]
            inc ECX
            test DL,DL
            je $L_61f388

            mov BYTE PTR [EDI],DL
            inc EDI
            test ECX,3
            jne $L_61f31d

            jmp $L_61f336
$L_61f331::

            mov DWORD PTR [EDI],EDX
            add EDI,4
$L_61f336::

            mov EDX,2130640639
            mov EAX,DWORD PTR [ECX]
            add EDX,EAX
            xor EAX,4294967295
            xor EAX,EDX
            mov EDX,DWORD PTR [ECX]
            add ECX,4
            test EAX,2164326656
            je $L_61f331

            test DL,DL
            je $L_61f388

            test DH,DH
            je $L_61f37f

            test EDX,16711680
            je $L_61f372

            test EDX,4278190080
            je $L_61f36a

            jmp $L_61f331
$L_61f36a::

            mov DWORD PTR [EDI],EDX
            mov EAX,DWORD PTR [ESP+8]
            pop EDI
            ret 
$L_61f372::

            mov WORD PTR [EDI],DX
            mov EAX,DWORD PTR [ESP+8]
            mov BYTE PTR [EDI+2],0
            pop EDI
            ret 
$L_61f37f::

            mov WORD PTR [EDI],DX
            mov EAX,DWORD PTR [ESP+8]
            pop EDI
            ret 
$L_61f388::

            mov BYTE PTR [EDI],DL
            mov EAX,DWORD PTR [ESP+8]
            pop EDI
            ret 
FUN_61f2b0 ENDP
ALIGN 16
_fabs PROC ;FUN_61f390 double fabs(double x)

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            push ESI
            mov ESI,65535
            push EDI
            push ESI
            push 4927
            call FUN_62606a

            mov EDI,EAX
            mov EAX,DWORD PTR [EBP+14]
            and AX,32752
            pop ECX
            cmp AX,32752
            pop ECX
            jne $L_61f424

            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            call FUN_625f32

            pop ECX
            dec EAX
            pop ECX
            je $L_61f416

            dec EAX
            je $L_61f406

            fld QWORD PTR [EBP+8]
            dec EAX
            push EDI
            je $L_61f3f5

            fadd QWORD PTR [(_KingRdata+1648)]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push 21
            push 8
            call FUN_625813

            add ESP,28
            jmp $L_61f441
$L_61f3f5::

            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push 21
            call FUN_6257bf

            add ESP,16
            jmp $L_61f441
$L_61f406::

            push ESI
            push EDI
            call FUN_62606a

            fld QWORD PTR [EBP+8]
            pop ECX
            fchs 
            pop ECX
            jmp $L_61f441
$L_61f416::

            push ESI
            push EDI
            call FUN_62606a

            fld QWORD PTR [EBP+8]
            pop ECX
            pop ECX
            jmp $L_61f441
$L_61f424::

            mov EAX,DWORD PTR [EBP+12]
            push ESI
            and EAX,2147483647
            push EDI
            mov DWORD PTR [EBP-4],EAX
            mov EAX,DWORD PTR [EBP+8]
            mov DWORD PTR [EBP-8],EAX
            call FUN_62606a

            fld QWORD PTR [EBP-8]
            pop ECX
            pop ECX
$L_61f441::

            pop EDI
            pop ESI
            leave 
            ret 
_fabs ENDP
_memset PROC ;FUN_61f5c0 void *memset(void *dest, int c, size_t count)

            mov EDX,DWORD PTR [ESP+12]
            mov ECX,DWORD PTR [ESP+4]
            test EDX,EDX
            je $L_61f613

            xor EAX,EAX
            mov AL,BYTE PTR [ESP+8]
            push EDI
            mov EDI,ECX
            cmp EDX,4
            jb $L_61f607

            neg ECX
            and ECX,3
            je $L_61f5e9

            sub EDX,ECX
$L_61f5e3::

            mov BYTE PTR [EDI],AL
            inc EDI
            dec ECX
            jne $L_61f5e3
$L_61f5e9::

            mov ECX,EAX
            shl EAX,8
            add EAX,ECX
            mov ECX,EAX
            shl EAX,16
            add EAX,ECX
            mov ECX,EDX
            and EDX,3
            shr ECX,2
            je $L_61f607

            rep stosd DWORD PTR ES:[EDI]

            test EDX,EDX
            je $L_61f60d
$L_61f607::

            mov BYTE PTR [EDI],AL
            inc EDI
            dec EDX
            jne $L_61f607
$L_61f60d::

            mov EAX,DWORD PTR [ESP+8]
            pop EDI
            ret 
$L_61f613::

            mov EAX,DWORD PTR [ESP+4]
            ret 
_memset ENDP
_sqrt PROC ;FUN_61f6e4 double sqrt(double x)

            lea EDX,DWORD PTR [ESP+4]
            call FUN_6247b5
_sqrt ENDP
FUN_61f6ed PROC

            push EDX
            wait 
            fnstcw WORD PTR [ESP]
            mov EAX,DWORD PTR [ESP+12]
            je $L_61f749

            cmp WORD PTR [ESP],639
            je $L_61f705

            call FUN_624785
$L_61f705::

            test EAX,2147483648
            jne $L_61f72b

            fsqrt 
$L_61f70e::

            cmp DWORD PTR [(_KingBss+669944)],0
            jne $L_62480e

            mov EDX,5
            lea ECX,DWORD PTR [(_KingData+108048)]
            jmp $L_62481b
$L_61f72b::

            test EAX,2146435072
            jne $L_61f75e

            test EAX,1048575
            jne $L_61f75e

            cmp DWORD PTR [ESP+8],0
            jne $L_61f75e

            jmp $L_61f70e
$L_61f742::

            call FUN_62479c

            jmp $L_61f76b
$L_61f749::

            test EAX,1048575
            jne $L_61f742

            cmp DWORD PTR [ESP+8],0
            jne $L_61f742

            and EAX,2147483648
            je $L_61f70e
$L_61f75e::

            fstp ST(0)
            fld TBYTE PTR [(_KingData+117568)]
            mov EAX,1
$L_61f76b::

            cmp DWORD PTR [(_KingBss+669944)],0
            jne $L_62480e

            mov EDX,5
            lea ECX,DWORD PTR [(_KingData+108048)]
            call FUN_624727

            pop EDX
            ret 
FUN_61f6ed ENDP
_cos PROC ;FUN_61f7a4 double cos(double x)

            lea EDX,DWORD PTR [ESP+4]
            call FUN_6247b5
_cos ENDP
FUN_61f7ad PROC

            push EDX
            wait 
            fnstcw WORD PTR [ESP]
            je $L_61f804

            cmp WORD PTR [ESP],639
            je $L_61f7c2

            fldcw WORD PTR [(_KingRdata+39512)]
$L_61f7c2::

            fcos 
            wait 
            fnstsw AX
            sahf 
            jp $L_61f7e7
$L_61f7ca::

            cmp DWORD PTR [(_KingBss+669944)],0
            jne $L_62480e

            mov EDX,18
            lea ECX,DWORD PTR [(_KingData+108064)]
            jmp $L_62481b
$L_61f7e7::

            fld TBYTE PTR [(_KingRdata+39514)]
            fxch ST(1)
$L_61f7ef::

            fprem1 
            wait 
            fnstsw AX
            sahf 
            jp $L_61f7ef

            fstp ST(1)
            fcos 
            jmp $L_61f7ca
$L_61f7fd::

            call FUN_62479c

            jmp $L_61f81f
$L_61f804::

            test EAX,1048575
            jne $L_61f7fd

            cmp DWORD PTR [ESP+8],0
            jne $L_61f7fd

            fstp ST(0)
            fld TBYTE PTR [(_KingData+117568)]
            mov EAX,1
$L_61f81f::

            cmp DWORD PTR [(_KingBss+669944)],0
            jne $L_62480e

            mov EDX,18
            lea ECX,DWORD PTR [(_KingData+108064)]
            call FUN_624727

            pop EDX
            ret 
FUN_61f7ad ENDP
$L_61fbf4 PROC

            fxch ST(1)
$L_61fbf6::

            cmp DWORD PTR [(_KingBss+669948)],1
            je $L_61fc03

            fprem 
            jmp $L_61fc08
$L_61fc03::

            call FUN_62717c
$L_61fc08::

            wait 
            fnstsw AX
            wait 
            sahf 
            jp $L_61fbf6

            fstp ST(1)
            ret 
$L_61fbf4 ENDP
ALIGN 2
ALIGN 8
ALIGN 4
ALIGN 16
FUN_61fd40 PROC

            mov EAX,DWORD PTR [(_KingBss+670064)]
            test EAX,EAX
            je $L_61fd58

            push DWORD PTR [ESP+4]
            call EAX

            test EAX,EAX
            pop ECX
            je $L_61fd58

            push 1
            pop EAX
            ret 
$L_61fd58::

            xor EAX,EAX
            ret 
FUN_61fd40 ENDP
ALIGN 4
_ceil PROC ;FUN_61fd97 double ceil(double x)

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            push EBX
            push ESI
            mov ESI,65535
            push ESI
            push DWORD PTR [(_KingData+108176)]
            call FUN_62606a

            fld QWORD PTR [EBP+8]
            pop ECX
            mov EBX,EAX
            mov EAX,DWORD PTR [EBP+14]
            pop ECX
            push ECX
            and AX,32752
            push ECX
            cmp AX,32752
            fstp QWORD PTR [ESP]
            jne $L_61fe1d

            call FUN_625f32

            pop ECX
            test EAX,EAX
            pop ECX
            jle $L_61fe00

            cmp EAX,2
            jle $L_61fdf2

            cmp EAX,3
            jne $L_61fe00

            fld QWORD PTR [EBP+8]
            push EBX
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push 12
            call FUN_6257bf

            add ESP,16
            jmp $L_61fe62
$L_61fdf2::

            push ESI
            push EBX
            call FUN_62606a

            fld QWORD PTR [EBP+8]
            pop ECX
            pop ECX
            jmp $L_61fe62
$L_61fe00::

            fld QWORD PTR [EBP+8]
            fadd QWORD PTR [(_KingRdata+1648)]
            push EBX
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push 12
            push 8
            jmp $L_61fe5a
$L_61fe1d::

            call FUN_6257ad

            fstp QWORD PTR [EBP-8]
            fld QWORD PTR [EBP-8]
            fcomp QWORD PTR [EBP+8]
            pop ECX
            pop ECX
            fnstsw AX
            sahf 
            jne $L_61fe40
$L_61fe32::

            push ESI
            push EBX
            call FUN_62606a

            fld QWORD PTR [EBP-8]
            pop ECX
            pop ECX
            jmp $L_61fe62
$L_61fe40::

            test BL,32
            jne $L_61fe32

            fld QWORD PTR [EBP-8]
            push EBX
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push 12
            push 16
$L_61fe5a::

            call FUN_625813

            add ESP,28
$L_61fe62::

            pop ESI
            pop EBX
            leave 
            ret 
_ceil ENDP
ALIGN 2
ALIGN 2
FUN_61ffd2 PROC

            cmp DWORD PTR [(_KingBss+670076)],1
            jne $L_61ffe0

            call FUN_624fca
$L_61ffe0::

            push DWORD PTR [ESP+4]
            call FUN_625003

            push 255
            call DWORD PTR [(_KingData+108180)]

            pop ECX
            pop ECX
            ret 
FUN_61ffd2 ENDP
FUN_62001c PROC

            push -1
            push EAX
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov EAX,DWORD PTR [ESP+12]
            mov DWORD PTR FS:[0],ESP
            mov DWORD PTR [ESP+12],EBP
            lea EBP,DWORD PTR [ESP+12]
            push EAX
            ret 
FUN_62001c ENDP
ALIGN 2
$L_620162 PROC

            mov ESP,DWORD PTR [EBP-24]
            push DWORD PTR [EBP-28]
            call __exit
$L_620162 ENDP
FUN_6201a0 PROC

            mov EAX,DWORD PTR [ESP+12]
            test EAX,EAX
            je $L_6201ec

            mov EDX,DWORD PTR [ESP+4]
            push ESI
            push EDI
            mov ESI,EDX
            mov EDI,DWORD PTR [ESP+16]
            or EDX,EDI
            and EDX,3
            je $L_6201ed

            test EAX,1
            je $L_6201cd

            mov CL,BYTE PTR [ESI]
            cmp CL,BYTE PTR [EDI]
            jne $L_62021a

            inc ESI
            inc EDI
            dec EAX
            je $L_6201ea
$L_6201cd::

            mov CL,BYTE PTR [ESI]
            mov DL,BYTE PTR [EDI]
            cmp CL,DL
            jne $L_62021a

            mov CL,BYTE PTR [ESI+1]
            mov DL,BYTE PTR [EDI+1]
            cmp CL,DL
            jne $L_62021a

            add EDI,2
            add ESI,2
            sub EAX,2
            jne $L_6201cd
$L_6201ea::

            pop EDI
            pop ESI
$L_6201ec::

            ret 
$L_6201ed::

            mov ECX,EAX
            and EAX,3
            shr ECX,2
            je $L_620222

            repe cmpsd DWORD PTR [ESI],DWORD PTR ES:[EDI]

            je $L_620222

            mov ECX,DWORD PTR [ESI-4]
            mov EDX,DWORD PTR [EDI-4]
            cmp CL,DL
            jne $L_620215

            cmp CH,DH
            jne $L_620215

            shr ECX,16
            shr EDX,16
            cmp CL,DL
            jne $L_620215

            cmp CH,DH
$L_620215::

            mov EAX,0
$L_62021a::

            sbb EAX,EAX
            pop EDI
            sbb EAX,-1
            pop ESI
            ret 
$L_620222::

            test EAX,EAX
            je $L_6201ea

            mov EDX,DWORD PTR [ESI]
            mov ECX,DWORD PTR [EDI]
            cmp DL,CL
            jne $L_620215

            dec EAX
            je $L_620249

            cmp DH,CH
            jne $L_620215

            dec EAX
            je $L_620249

            and ECX,16711680
            and EDX,16711680
            cmp EDX,ECX
            jne $L_620215

            dec EAX
$L_620249::

            pop EDI
            pop ESI
            ret 
FUN_6201a0 ENDP
ALIGN 16
FUN_620cf2 PROC

            cmp DWORD PTR [(_KingBss+1000144)],0
            jne $L_620d0d

            push -3
            call __setmbcp

            pop ECX
            mov DWORD PTR [(_KingBss+1000144)],1
$L_620d0d::

            ret 
FUN_620cf2 ENDP
ALIGN 2
ALIGN 16
FUN_620d80 PROC

            push ESI
            call ___doserrno

            mov ECX,DWORD PTR [ESP+8]
            xor ESI,ESI
            mov DWORD PTR [EAX],ECX
            mov EAX,OFFSET (_KingData+108440)
$L_620d93::

            cmp ECX,DWORD PTR [EAX]
            je $L_620db9

            add EAX,8
            inc ESI
            cmp EAX,OFFSET (_KingData+108800)
            jl $L_620d93

            cmp ECX,19
            jb $L_620dc9

            cmp ECX,36
            ja $L_620dc9

            call __errno

            mov DWORD PTR [EAX],13
            pop ESI
            ret 
$L_620db9::

            call __errno

            mov ECX,DWORD PTR [ESI*8+(_KingData+108440)+4]
            pop ESI
            mov DWORD PTR [EAX],ECX
            ret 
$L_620dc9::

            cmp ECX,188
            jb $L_620de6

            cmp ECX,202
            ja $L_620de6

            call __errno

            mov DWORD PTR [EAX],8
            pop ESI
            ret 
$L_620de6::

            call __errno

            mov DWORD PTR [EAX],22
            pop ESI
            ret 
FUN_620d80 ENDP
ALIGN 4
FUN_622b90 PROC

            mov EAX,DWORD PTR [ESP+4]
            mov DWORD PTR [EAX+80],OFFSET (_KingData+119520)
            mov DWORD PTR [EAX+20],1
            ret 
FUN_622b90 ENDP
FUN_622ba3 PROC

            push ESI
            push EDI
            call DWORD PTR __imp__GetLastError@0

            push DWORD PTR [(_KingData+109024)]
            mov EDI,EAX
            call DWORD PTR __imp__TlsGetValue@4

            mov ESI,EAX
            test ESI,ESI
            jne $L_622bfe

            push 116
            push 1
            call FUN_624c4d

            mov ESI,EAX
            pop ECX
            test ESI,ESI
            pop ECX
            je $L_622bf6

            push ESI
            push DWORD PTR [(_KingData+109024)]
            call DWORD PTR __imp__TlsSetValue@8

            test EAX,EAX
            je $L_622bf6

            push ESI
            call FUN_622b90

            pop ECX
            call DWORD PTR __imp__GetCurrentThreadId@0

            or DWORD PTR [ESI+4],4294967295
            mov DWORD PTR [ESI],EAX
            jmp $L_622bfe
$L_622bf6::

            push 16
            call FUN_61ffd2

            pop ECX
$L_622bfe::

            push EDI
            call DWORD PTR __imp__SetLastError@4

            mov EAX,ESI
            pop EDI
            pop ESI
            ret 
FUN_622ba3 ENDP
ALIGN 2
FUN_622caa PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET (_KingRdata+39408)
            push OFFSET $L_624254
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            push ECX
            push ECX
            push EBX
            push ESI
            push EDI
            mov DWORD PTR [EBP-24],ESP
            xor ESI,ESI
            mov DWORD PTR [EBP-4],ESI
            call FUN_622ba3

            cmp DWORD PTR [EAX+96],ESI
            je $L_622cfd

            mov DWORD PTR [EBP-4],1
            call FUN_622ba3

            call DWORD PTR [EAX+96]

            mov DWORD PTR [EBP-4],ESI
            jmp $L_622cfd
FUN_622caa ENDP
$L_622cf2 PROC

            push 1
            pop EAX
            ret 
$L_622cf2 ENDP
$L_622cf6 PROC

            mov ESP,DWORD PTR [EBP-24]
            and DWORD PTR [EBP-4],0
$L_622cfd::

            or DWORD PTR [EBP-4],4294967295
            call FUN_622d06
$L_622cf6 ENDP
ALIGN 2
FUN_622d06 PROC

            jmp _abort
FUN_622d06 ENDP
$L_622d61 PROC

            push ESI
            mov ESI,DWORD PTR [ESP+8]
            mov EAX,DWORD PTR [ESI]
            cmp DWORD PTR [EAX],3765269347
            jne $L_622d84

            cmp DWORD PTR [EAX+16],3
            jne $L_622d84

            cmp DWORD PTR [EAX+20],429065504
            jne $L_622d84

            jmp FUN_622caa
$L_622d84::

            mov EAX,DWORD PTR [(_KingBss+670156)]
            test EAX,EAX
            je $L_622da1

            push EAX
            call FUN_62937d

            test EAX,EAX
            pop ECX
            je $L_622da1

            push ESI
            call DWORD PTR [(_KingBss+670156)]

            jmp $L_622da3
$L_622da1::

            xor EAX,EAX
$L_622da3::

            pop ESI
            ret 4
$L_622d61 ENDP
FUN_623333 PROC

            push EBP
            mov EBP,ESP
            sub ESP,20
            mov EAX,DWORD PTR [(_KingBss+999548)]
            mov EDX,DWORD PTR [(_KingBss+999552)]
            push EBX
            push ESI
            lea EAX,DWORD PTR [EAX+EAX*4]
            push EDI
            lea EDI,DWORD PTR [EDX+EAX*4]
            mov EAX,DWORD PTR [EBP+8]
            mov DWORD PTR [EBP-4],EDI
            lea ECX,DWORD PTR [EAX+23]
            and ECX,4294967280
            mov DWORD PTR [EBP-16],ECX
            sar ECX,4
            dec ECX
            cmp ECX,32
            jge $L_623373

            or ESI,4294967295
            shr ESI,CL
            or DWORD PTR [EBP-8],4294967295
            mov DWORD PTR [EBP-12],ESI
            jmp $L_623383
$L_623373::

            add ECX,-32
            or EAX,4294967295
            xor ESI,ESI
            shr EAX,CL
            mov DWORD PTR [EBP-12],ESI
            mov DWORD PTR [EBP-8],EAX
$L_623383::

            mov EAX,DWORD PTR [(_KingBss+999540)]
            mov EBX,EAX
            cmp EBX,EDI
            mov DWORD PTR [EBP+8],EBX
            jae $L_6233aa
$L_623391::

            mov ECX,DWORD PTR [EBX+4]
            mov EDI,DWORD PTR [EBX]
            and ECX,DWORD PTR [EBP-8]
            and EDI,ESI
            or ECX,EDI
            jne $L_6233aa

            add EBX,20
            cmp EBX,DWORD PTR [EBP-4]
            mov DWORD PTR [EBP+8],EBX
            jb $L_623391
$L_6233aa::

            cmp EBX,DWORD PTR [EBP-4]
            jne $L_623428

            mov EBX,EDX
$L_6233b1::

            cmp EBX,EAX
            mov DWORD PTR [EBP+8],EBX
            jae $L_6233cd

            mov ECX,DWORD PTR [EBX+4]
            mov EDI,DWORD PTR [EBX]
            and ECX,DWORD PTR [EBP-8]
            and EDI,ESI
            or ECX,EDI
            jne $L_6233cb

            add EBX,20
            jmp $L_6233b1
$L_6233cb::

            cmp EBX,EAX
$L_6233cd::

            jne $L_623428
$L_6233cf::

            cmp EBX,DWORD PTR [EBP-4]
            jae $L_6233e5

            cmp DWORD PTR [EBX+8],0
            jne $L_6233e2

            add EBX,20
            mov DWORD PTR [EBP+8],EBX
            jmp $L_6233cf
$L_6233e2::

            cmp EBX,DWORD PTR [EBP-4]
$L_6233e5::

            jne $L_62340d

            mov EBX,EDX
$L_6233e9::

            cmp EBX,EAX
            mov DWORD PTR [EBP+8],EBX
            jae $L_6233fd

            cmp DWORD PTR [EBX+8],0
            jne $L_6233fb

            add EBX,20
            jmp $L_6233e9
$L_6233fb::

            cmp EBX,EAX
$L_6233fd::

            jne $L_62340d

            call FUN_62363c

            mov EBX,EAX
            test EBX,EBX
            mov DWORD PTR [EBP+8],EBX
            je $L_623421
$L_62340d::

            push EBX
            call FUN_6236ed

            pop ECX
            mov ECX,DWORD PTR [EBX+16]
            mov DWORD PTR [ECX],EAX
            mov EAX,DWORD PTR [EBX+16]
            cmp DWORD PTR [EAX],-1
            jne $L_623428
$L_623421::

            xor EAX,EAX
            jmp $L_623637
$L_623428::

            mov DWORD PTR [(_KingBss+999540)],EBX
            mov EAX,DWORD PTR [EBX+16]
            mov EDX,DWORD PTR [EAX]
            cmp EDX,-1
            mov DWORD PTR [EBP-4],EDX
            je $L_62344f

            mov ECX,DWORD PTR [EAX+EDX*4+196]
            mov EDI,DWORD PTR [EAX+EDX*4+68]
            and ECX,DWORD PTR [EBP-8]
            and EDI,ESI
            or ECX,EDI
            jne $L_623486
$L_62344f::

            mov EDX,DWORD PTR [EAX+196]
            mov ESI,DWORD PTR [EAX+68]
            and EDX,DWORD PTR [EBP-8]
            and ESI,DWORD PTR [EBP-12]
            and DWORD PTR [EBP-4],0
            lea ECX,DWORD PTR [EAX+68]
            or EDX,ESI
            mov ESI,DWORD PTR [EBP-12]
            jne $L_623483
$L_62346c::

            mov EDX,DWORD PTR [ECX+132]
            inc DWORD PTR [EBP-4]
            and EDX,DWORD PTR [EBP-8]
            add ECX,4
            mov EDI,ESI
            and EDI,DWORD PTR [ECX]
            or EDX,EDI
            je $L_62346c
$L_623483::

            mov EDX,DWORD PTR [EBP-4]
$L_623486::

            mov ECX,EDX
            xor EDI,EDI
            imul ECX,ECX,516
            lea ECX,DWORD PTR [ECX+EAX*1+324]
            mov DWORD PTR [EBP-12],ECX
            mov ECX,DWORD PTR [EAX+EDX*4+68]
            and ECX,ESI
            jne $L_6234af

            mov ECX,DWORD PTR [EAX+EDX*4+196]
            push 32
            and ECX,DWORD PTR [EBP-8]
            pop EDI
$L_6234af::

            test ECX,ECX
            jl $L_6234b8

            shl ECX,1
            inc EDI
            jmp $L_6234af
$L_6234b8::

            mov ECX,DWORD PTR [EBP-12]
            mov EDX,DWORD PTR [ECX+EDI*8+4]
            mov ECX,DWORD PTR [EDX]
            sub ECX,DWORD PTR [EBP-16]
            mov ESI,ECX
            mov DWORD PTR [EBP-8],ECX
            sar ESI,4
            dec ESI
            cmp ESI,63
            jle $L_6234d5

            push 63
            pop ESI
$L_6234d5::

            cmp ESI,EDI
            je $L_6235ea

            mov ECX,DWORD PTR [EDX+4]
            cmp ECX,DWORD PTR [EDX+8]
            jne $L_623546

            cmp EDI,32
            jge $L_623515

            mov EBX,2147483648
            mov ECX,EDI
            shr EBX,CL
            mov ECX,DWORD PTR [EBP-4]
            lea EDI,DWORD PTR [EAX+EDI*1+4]
            not EBX
            mov DWORD PTR [EBP-20],EBX
            and EBX,DWORD PTR [EAX+ECX*4+68]
            mov DWORD PTR [EAX+ECX*4+68],EBX
            dec BYTE PTR [EDI]
            jne $L_623543

            mov EBX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EBP-20]
            and DWORD PTR [EBX],ECX
            jmp $L_623546
$L_623515::

            lea ECX,DWORD PTR [EDI-32]
            mov EBX,2147483648
            shr EBX,CL
            mov ECX,DWORD PTR [EBP-4]
            lea EDI,DWORD PTR [EAX+EDI*1+4]
            lea ECX,DWORD PTR [EAX+ECX*4+196]
            not EBX
            and DWORD PTR [ECX],EBX
            dec BYTE PTR [EDI]
            mov DWORD PTR [EBP-20],EBX
            jne $L_623543

            mov EBX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EBP-20]
            and DWORD PTR [EBX+4],ECX
            jmp $L_623546
$L_623543::

            mov EBX,DWORD PTR [EBP+8]
$L_623546::

            mov ECX,DWORD PTR [EDX+8]
            mov EDI,DWORD PTR [EDX+4]
            cmp DWORD PTR [EBP-8],0
            mov DWORD PTR [ECX+4],EDI
            mov ECX,DWORD PTR [EDX+4]
            mov EDI,DWORD PTR [EDX+8]
            mov DWORD PTR [ECX+8],EDI
            je $L_6235f6

            mov ECX,DWORD PTR [EBP-12]
            mov EDI,DWORD PTR [ECX+ESI*8+4]
            lea ECX,DWORD PTR [ECX+ESI*8]
            mov DWORD PTR [EDX+4],EDI
            mov DWORD PTR [EDX+8],ECX
            mov DWORD PTR [ECX+4],EDX
            mov ECX,DWORD PTR [EDX+4]
            mov DWORD PTR [ECX+8],EDX
            mov ECX,DWORD PTR [EDX+4]
            cmp ECX,DWORD PTR [EDX+8]
            jne $L_6235e7

            mov CL,BYTE PTR [ESI+EAX*1+4]
            cmp ESI,32
            mov BYTE PTR [EBP+11],CL
            jge $L_6235b8

            inc CL
            cmp BYTE PTR [EBP+11],0
            mov BYTE PTR [ESI+EAX*1+4],CL
            jne $L_6235a6

            mov EDI,2147483648
            mov ECX,ESI
            shr EDI,CL
            or DWORD PTR [EBX],EDI
$L_6235a6::

            mov EDI,2147483648
            mov ECX,ESI
            shr EDI,CL
            mov ECX,DWORD PTR [EBP-4]
            or DWORD PTR [EAX+ECX*4+68],EDI
            jmp $L_6235e7
$L_6235b8::

            inc CL
            cmp BYTE PTR [EBP+11],0
            mov BYTE PTR [ESI+EAX*1+4],CL
            jne $L_6235d1

            lea ECX,DWORD PTR [ESI-32]
            mov EDI,2147483648
            shr EDI,CL
            or DWORD PTR [EBX+4],EDI
$L_6235d1::

            mov ECX,DWORD PTR [EBP-4]
            lea EDI,DWORD PTR [EAX+ECX*4+196]
            lea ECX,DWORD PTR [ESI-32]
            mov ESI,2147483648
            shr ESI,CL
            or DWORD PTR [EDI],ESI
$L_6235e7::

            mov ECX,DWORD PTR [EBP-8]
$L_6235ea::

            test ECX,ECX
            je $L_6235f9

            mov DWORD PTR [EDX],ECX
            mov DWORD PTR [ECX+EDX*1-4],ECX
            jmp $L_6235f9
$L_6235f6::

            mov ECX,DWORD PTR [EBP-8]
$L_6235f9::

            mov ESI,DWORD PTR [EBP-16]
            add EDX,ECX
            lea ECX,DWORD PTR [ESI+1]
            mov DWORD PTR [EDX],ECX
            mov DWORD PTR [EDX+ESI*1-4],ECX
            mov ESI,DWORD PTR [EBP-12]
            mov ECX,DWORD PTR [ESI]
            test ECX,ECX
            lea EDI,DWORD PTR [ECX+1]
            mov DWORD PTR [ESI],EDI
            jne $L_62362f

            cmp EBX,DWORD PTR [(_KingBss+999544)]
            jne $L_62362f

            mov ECX,DWORD PTR [EBP-4]
            cmp ECX,DWORD PTR [(_KingBss+999536)]
            jne $L_62362f

            and DWORD PTR [(_KingBss+999544)],0
$L_62362f::

            mov ECX,DWORD PTR [EBP-4]
            mov DWORD PTR [EAX],ECX
            lea EAX,DWORD PTR [EDX+4]
$L_623637::

            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 
FUN_623333 ENDP
ALIGN 4
FUN_62363c PROC

            mov EAX,DWORD PTR [(_KingBss+999548)]
            mov ECX,DWORD PTR [(_KingBss+999532)]
            push ESI
            push EDI
            xor EDI,EDI
            cmp EAX,ECX
            jne $L_62367f

            lea EAX,DWORD PTR [ECX+ECX*4+80]
            shl EAX,2
            push EAX
            push DWORD PTR [(_KingBss+999552)]
            push EDI
            push DWORD PTR [(_KingBss+999560)]
            call DWORD PTR __imp__HeapReAlloc@16

            cmp EAX,EDI
            je $L_6236cf

            add DWORD PTR [(_KingBss+999532)],16
            mov DWORD PTR [(_KingBss+999552)],EAX
            mov EAX,DWORD PTR [(_KingBss+999548)]
$L_62367f::

            mov ECX,DWORD PTR [(_KingBss+999552)]
            push 16836
            push 8
            lea EAX,DWORD PTR [EAX+EAX*4]
            push DWORD PTR [(_KingBss+999560)]
            lea ESI,DWORD PTR [ECX+EAX*4]
            call DWORD PTR __imp__HeapAlloc@12

            cmp EAX,EDI
            mov DWORD PTR [ESI+16],EAX
            je $L_6236cf

            push 4
            push 8192
            push 1048576
            push EDI
            call DWORD PTR __imp__VirtualAlloc@16

            cmp EAX,EDI
            mov DWORD PTR [ESI+12],EAX
            jne $L_6236d3

            push DWORD PTR [ESI+16]
            push EDI
            push DWORD PTR [(_KingBss+999560)]
            call DWORD PTR __imp__HeapFree@12
$L_6236cf::

            xor EAX,EAX
            jmp $L_6236ea
$L_6236d3::

            or DWORD PTR [ESI+8],4294967295
            mov DWORD PTR [ESI],EDI
            mov DWORD PTR [ESI+4],EDI
            inc DWORD PTR [(_KingBss+999548)]
            mov EAX,DWORD PTR [ESI+16]
            or DWORD PTR [EAX],4294967295
            mov EAX,ESI
$L_6236ea::

            pop EDI
            pop ESI
            ret 
FUN_62363c ENDP
FUN_6236ed PROC

            push EBP
            mov EBP,ESP
            push ECX
            mov ECX,DWORD PTR [EBP+8]
            push EBX
            push ESI
            push EDI
            mov ESI,DWORD PTR [ECX+16]
            mov EAX,DWORD PTR [ECX+8]
            xor EBX,EBX
$L_6236ff::

            test EAX,EAX
            jl $L_623708

            shl EAX,1
            inc EBX
            jmp $L_6236ff
$L_623708::

            mov EAX,EBX
            push 63
            imul EAX,EAX,516
            pop EDX
            lea EAX,DWORD PTR [EAX+ESI*1+324]
            mov DWORD PTR [EBP-4],EAX
$L_62371d::

            mov DWORD PTR [EAX+8],EAX
            mov DWORD PTR [EAX+4],EAX
            add EAX,8
            dec EDX
            jne $L_62371d

            mov EDI,EBX
            push 4
            shl EDI,15
            add EDI,DWORD PTR [ECX+12]
            push 4096
            push 32768
            push EDI
            call DWORD PTR __imp__VirtualAlloc@16

            test EAX,EAX
            jne $L_623750

            or EAX,4294967295
            jmp $L_6237e3
$L_623750::

            lea EDX,DWORD PTR [EDI+28672]
            cmp EDI,EDX
            ja $L_623796

            lea EAX,DWORD PTR [EDI+16]
$L_62375d::

            or DWORD PTR [EAX-8],4294967295
            or DWORD PTR [EAX+4076],4294967295
            lea ECX,DWORD PTR [EAX+4092]
            mov DWORD PTR [EAX-4],4080
            mov DWORD PTR [EAX],ECX
            lea ECX,DWORD PTR [EAX-4100]
            mov DWORD PTR [EAX+4],ECX
            mov DWORD PTR [EAX+4072],4080
            add EAX,4096
            lea ECX,DWORD PTR [EAX-16]
            cmp ECX,EDX
            jbe $L_62375d
$L_623796::

            mov EAX,DWORD PTR [EBP-4]
            lea ECX,DWORD PTR [EDI+12]
            add EAX,504
            push 1
            pop EDI
            mov DWORD PTR [EAX+4],ECX
            mov DWORD PTR [ECX+8],EAX
            lea ECX,DWORD PTR [EDX+12]
            mov DWORD PTR [EAX+8],ECX
            mov DWORD PTR [ECX+4],EAX
            and DWORD PTR [ESI+EBX*4+68],0
            mov DWORD PTR [ESI+EBX*4+196],EDI
            mov AL,BYTE PTR [ESI+67]
            mov CL,AL
            inc CL
            test AL,AL
            mov EAX,DWORD PTR [EBP+8]
            mov BYTE PTR [ESI+67],CL
            jne $L_6237d3

            or DWORD PTR [EAX+4],EDI
$L_6237d3::

            mov EDX,2147483648
            mov ECX,EBX
            shr EDX,CL
            not EDX
            and DWORD PTR [EAX+8],EDX
            mov EAX,EBX
$L_6237e3::

            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 
FUN_6236ed ENDP
ALIGN 8
FUN_623ade PROC

            cmp DWORD PTR [(_KingData+109056)],-1
            push EBX
            push EBP
            push ESI
            push EDI
            jne $L_623af2

            mov ESI,OFFSET (_KingData+109040)
            jmp $L_623b0f
$L_623af2::

            push 8224
            push 0
            push DWORD PTR [(_KingBss+999560)]
            call DWORD PTR __imp__HeapAlloc@12

            mov ESI,EAX
            test ESI,ESI
            je $L_623c1b
$L_623b0f::

            mov EBP,DWORD PTR __imp__VirtualAlloc@16
            push 4
            push 8192
            push 400000H
            push 0
            call EBP

            mov EDI,EAX
            test EDI,EDI
            je $L_623c04

            push 4
            mov EBX,65536
            push 4096
            push EBX
            push EDI
            call EBP

            test EAX,EAX
            je $L_623bf6

            mov EAX,OFFSET (_KingData+109040)
            cmp ESI,EAX
            jne $L_623b6e

            cmp DWORD PTR [(_KingData+109040)],0
            jne $L_623b5e

            mov DWORD PTR [(_KingData+109040)],EAX
$L_623b5e::

            cmp DWORD PTR [(_KingData+109044)],0
            jne $L_623b83

            mov DWORD PTR [(_KingData+109044)],EAX
            jmp $L_623b83
$L_623b6e::

            mov DWORD PTR [ESI],EAX
            mov EAX,DWORD PTR [(_KingData+109044)]
            mov DWORD PTR [ESI+4],EAX
            mov DWORD PTR [(_KingData+109044)],ESI
            mov EAX,DWORD PTR [ESI+4]
            mov DWORD PTR [EAX],ESI
$L_623b83::

            lea EAX,DWORD PTR [EDI+400000H]
            lea ECX,DWORD PTR [ESI+152]
            mov DWORD PTR [ESI+20],EAX
            lea EAX,DWORD PTR [ESI+24]
            mov DWORD PTR [ESI+12],ECX
            mov DWORD PTR [ESI+16],EDI
            mov DWORD PTR [ESI+8],EAX
            xor EBP,EBP
            mov ECX,241
$L_623ba5::

            xor EDX,EDX
            cmp EBP,16
            setge DL
            dec EDX
            and EDX,ECX
            dec EDX
            inc EBP
            mov DWORD PTR [EAX],EDX
            mov DWORD PTR [EAX+4],ECX
            add EAX,8
            cmp EBP,1024
            jl $L_623ba5

            push EBX
            push 0
            push EDI
            call _memset

            add ESP,12
$L_623bce::

            mov EAX,DWORD PTR [ESI+16]
            add EAX,EBX
            cmp EDI,EAX
            jae $L_623bf2

            or BYTE PTR [EDI+248],255
            lea EAX,DWORD PTR [EDI+8]
            mov DWORD PTR [EDI],EAX
            mov DWORD PTR [EDI+4],240
            add EDI,4096
            jmp $L_623bce
$L_623bf2::

            mov EAX,ESI
            jmp $L_623c1d
$L_623bf6::

            push 32768
            push 0
            push EDI
            call DWORD PTR __imp__VirtualFree@12
$L_623c04::

            cmp ESI,OFFSET (_KingData+109040)
            je $L_623c1b

            push ESI
            push 0
            push DWORD PTR [(_KingBss+999560)]
            call DWORD PTR __imp__HeapFree@12
$L_623c1b::

            xor EAX,EAX
$L_623c1d::

            pop EDI
            pop ESI
            pop EBP
            pop EBX
            ret 
FUN_623ade ENDP
ALIGN 2
FUN_623dd6 PROC

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            push EBX
            push ESI
            mov ESI,DWORD PTR [(_KingData+117264)]
            push EDI
$L_623de4::

            mov EDX,DWORD PTR [ESI+16]
            cmp EDX,-1
            je $L_623e8f

            mov EDI,DWORD PTR [ESI+8]
            lea ECX,DWORD PTR [ESI+8216]
            mov EAX,EDI
            sub EAX,ESI
            sub EAX,24
            sar EAX,3
            shl EAX,12
            add EAX,EDX
            cmp EDI,ECX
            mov DWORD PTR [EBP-4],EAX
            jae $L_623e49
$L_623e0f::

            mov ECX,DWORD PTR [EDI]
            mov EBX,DWORD PTR [EBP+8]
            cmp ECX,EBX
            jl $L_623e32

            cmp DWORD PTR [EDI+4],EBX
            jbe $L_623e32

            push EBX
            push ECX
            push EAX
            call FUN_623fde

            add ESP,12
            test EAX,EAX
            jne $L_623ea1

            mov EAX,DWORD PTR [EBP-4]
            mov DWORD PTR [EDI+4],EBX
$L_623e32::

            add EDI,8
            lea ECX,DWORD PTR [ESI+8216]
            add EAX,4096
            cmp EDI,ECX
            mov DWORD PTR [EBP-4],EAX
            jb $L_623e0f

            jmp $L_623e4c
$L_623e49::

            mov EBX,DWORD PTR [EBP+8]
$L_623e4c::

            mov EAX,DWORD PTR [ESI+8]
            mov ECX,DWORD PTR [ESI+16]
            lea EDI,DWORD PTR [ESI+24]
            mov DWORD PTR [EBP-8],EAX
            cmp EDI,EAX
            mov DWORD PTR [EBP-4],ECX
            jae $L_623e92
$L_623e5f::

            mov EAX,DWORD PTR [EDI]
            cmp EAX,EBX
            jl $L_623e7e

            cmp DWORD PTR [EDI+4],EBX
            jbe $L_623e7e

            push EBX
            push EAX
            push DWORD PTR [EBP-4]
            call FUN_623fde

            add ESP,12
            test EAX,EAX
            jne $L_623ea1

            mov DWORD PTR [EDI+4],EBX
$L_623e7e::

            add DWORD PTR [EBP-4],4096
            add EDI,8
            cmp EDI,DWORD PTR [EBP-8]
            jb $L_623e5f

            jmp $L_623e92
$L_623e8f::

            mov EBX,DWORD PTR [EBP+8]
$L_623e92::

            mov ESI,DWORD PTR [ESI]
            cmp ESI,DWORD PTR [(_KingData+117264)]
            je $L_623eb1

            jmp $L_623de4
$L_623ea1::

            mov DWORD PTR [(_KingData+117264)],ESI
            sub DWORD PTR [EDI],EBX
            mov DWORD PTR [ESI+8],EDI
            jmp $L_623fd9
$L_623eb1::

            mov EAX,OFFSET (_KingData+109040)
            mov EDI,EAX
$L_623eb8::

            cmp DWORD PTR [EDI+16],-1
            je $L_623ec4

            cmp DWORD PTR [EDI+12],0
            jne $L_623ed0
$L_623ec4::

            mov EDI,DWORD PTR [EDI]
            cmp EDI,EAX
            je $L_623fa5

            jmp $L_623eb8
$L_623ed0::

            mov EBX,DWORD PTR [EDI+12]
            and DWORD PTR [EBP-4],0
            mov ESI,EBX
            mov EAX,EBX
            sub ESI,EDI
            sub ESI,24
            sar ESI,3
            shl ESI,12
            add ESI,DWORD PTR [EDI+16]
            cmp DWORD PTR [EBX],-1
            jne $L_623eff
$L_623eee::

            cmp DWORD PTR [EBP-4],16
            jge $L_623eff

            add EAX,8
            inc DWORD PTR [EBP-4]
            cmp DWORD PTR [EAX],-1
            je $L_623eee
$L_623eff::

            mov EAX,DWORD PTR [EBP-4]
            push 4
            shl EAX,12
            push 4096
            push EAX
            push ESI
            mov DWORD PTR [EBP-8],EAX
            call DWORD PTR __imp__VirtualAlloc@16

            cmp EAX,ESI
            jne $L_623fd7

            push 0
            push DWORD PTR [EBP-8]
            push ESI
            call _memset

            mov EDX,DWORD PTR [EBP-4]
            add ESP,12
            test EDX,EDX
            mov ECX,EBX
            jle $L_623f66

            lea EAX,DWORD PTR [ESI+4]
            mov DWORD PTR [EBP-4],EDX
$L_623f3c::

            or BYTE PTR [EAX+244],255
            lea EDX,DWORD PTR [EAX+4]
            mov DWORD PTR [EAX-4],EDX
            mov EDX,240
            mov DWORD PTR [EAX],EDX
            mov DWORD PTR [ECX],EDX
            mov DWORD PTR [ECX+4],241
            add EAX,4096
            add ECX,8
            dec DWORD PTR [EBP-4]
            jne $L_623f3c
$L_623f66::

            mov DWORD PTR [(_KingData+117264)],EDI
            lea EAX,DWORD PTR [EDI+8216]
$L_623f72::

            cmp ECX,EAX
            jae $L_623f82

            cmp DWORD PTR [ECX],-1
            je $L_623f80

            add ECX,8
            jmp $L_623f72
$L_623f80::

            cmp ECX,EAX
$L_623f82::

            sbb EAX,EAX
            and EAX,ECX
            mov DWORD PTR [EDI+12],EAX
            mov EAX,DWORD PTR [EBP+8]
            mov BYTE PTR [ESI+8],AL
            mov DWORD PTR [EDI+8],EBX
            sub DWORD PTR [EBX],EAX
            sub DWORD PTR [ESI+4],EAX
            lea ECX,DWORD PTR [ESI+EAX*1+8]
            lea EAX,DWORD PTR [ESI+256]
            mov DWORD PTR [ESI],ECX
            jmp $L_623fd9
$L_623fa5::

            call FUN_623ade

            test EAX,EAX
            je $L_623fd7

            mov ECX,DWORD PTR [EAX+16]
            mov BYTE PTR [ECX+8],BL
            lea EDX,DWORD PTR [ECX+EBX*1+8]
            mov DWORD PTR [(_KingData+117264)],EAX
            mov DWORD PTR [ECX],EDX
            mov EDX,240
            sub EDX,EBX
            mov DWORD PTR [ECX+4],EDX
            movzx EDX,BL
            sub DWORD PTR [EAX+24],EDX
            lea EAX,DWORD PTR [ECX+256]
            jmp $L_623fd9
$L_623fd7::

            xor EAX,EAX
$L_623fd9::

            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 
FUN_623dd6 ENDP
ALIGN 2
FUN_623fde PROC

            push EBP
            mov EBP,ESP
            push ECX
            mov ECX,DWORD PTR [EBP+8]
            mov EDX,DWORD PTR [EBP+16]
            push EBX
            push ESI
            mov ESI,DWORD PTR [ECX+4]
            push EDI
            mov EDI,DWORD PTR [ECX]
            lea EBX,DWORD PTR [ECX+248]
            cmp ESI,EDX
            mov DWORD PTR [EBP-4],EDI
            mov EAX,EDI
            mov DWORD PTR [EBP+8],EBX
            jb $L_624023

            lea EAX,DWORD PTR [EDI+EDX*1]
            mov BYTE PTR [EDI],DL
            cmp EAX,EBX
            jae $L_624012

            add DWORD PTR [ECX],EDX
            sub DWORD PTR [ECX+4],EDX
            jmp $L_62401b
$L_624012::

            and DWORD PTR [ECX+4],0
            lea EAX,DWORD PTR [ECX+8]
            mov DWORD PTR [ECX],EAX
$L_62401b::

            lea EAX,DWORD PTR [EDI+8]
            jmp $L_6240f1
$L_624023::

            add ESI,EDI
            cmp BYTE PTR [ESI],0
            je $L_62402c

            mov EAX,ESI
$L_62402c::

            lea ESI,DWORD PTR [EAX+EDX*1]
            cmp ESI,EBX
            jae $L_624076
$L_624033::

            mov BL,BYTE PTR [EAX]
            test BL,BL
            jne $L_624069

            push 1
            lea EBX,DWORD PTR [EAX+1]
            pop ESI
$L_62403f::

            cmp BYTE PTR [EBX],0
            jne $L_624048

            inc EBX
            inc ESI
            jmp $L_62403f
$L_624048::

            cmp ESI,EDX
            jae $L_62409a

            cmp EAX,DWORD PTR [EBP-4]
            jne $L_624056

            mov DWORD PTR [ECX+4],ESI
            jmp $L_624062
$L_624056::

            sub DWORD PTR [EBP+12],ESI
            cmp DWORD PTR [EBP+12],EDX
            jb $L_6240fb
$L_624062::

            mov EDI,DWORD PTR [EBP-4]
            mov EAX,EBX
            jmp $L_62406e
$L_624069::

            movzx ESI,BL
            add EAX,ESI
$L_62406e::

            lea ESI,DWORD PTR [EAX+EDX*1]
            cmp ESI,DWORD PTR [EBP+8]
            jb $L_624033
$L_624076::

            lea ESI,DWORD PTR [ECX+8]
$L_624079::

            cmp ESI,EDI
            jae $L_6240fb

            lea EAX,DWORD PTR [ESI+EDX*1]
            cmp EAX,DWORD PTR [EBP+8]
            jae $L_6240fb

            mov AL,BYTE PTR [ESI]
            test AL,AL
            jne $L_6240cb

            push 1
            lea EBX,DWORD PTR [ESI+1]
            pop EAX
$L_624091::

            cmp BYTE PTR [EBX],0
            jne $L_6240bb

            inc EBX
            inc EAX
            jmp $L_624091
$L_62409a::

            lea EBX,DWORD PTR [EAX+EDX*1]
            cmp EBX,DWORD PTR [EBP+8]
            jae $L_6240ab

            sub ESI,EDX
            mov DWORD PTR [ECX],EBX
            mov DWORD PTR [ECX+4],ESI
            jmp $L_6240b4
$L_6240ab::

            and DWORD PTR [ECX+4],0
            lea ESI,DWORD PTR [ECX+8]
            mov DWORD PTR [ECX],ESI
$L_6240b4::

            mov BYTE PTR [EAX],DL
            add EAX,8
            jmp $L_6240f1
$L_6240bb::

            cmp EAX,EDX
            jae $L_6240d2

            sub DWORD PTR [EBP+12],EAX
            cmp DWORD PTR [EBP+12],EDX
            jb $L_6240fb

            mov ESI,EBX
            jmp $L_624079
$L_6240cb::

            movzx EAX,AL
            add ESI,EAX
            jmp $L_624079
$L_6240d2::

            lea EBX,DWORD PTR [ESI+EDX*1]
            cmp EBX,DWORD PTR [EBP+8]
            jae $L_6240e3

            sub EAX,EDX
            mov DWORD PTR [ECX],EBX
            mov DWORD PTR [ECX+4],EAX
            jmp $L_6240ec
$L_6240e3::

            and DWORD PTR [ECX+4],0
            lea EAX,DWORD PTR [ECX+8]
            mov DWORD PTR [ECX],EAX
$L_6240ec::

            mov BYTE PTR [ESI],DL
            lea EAX,DWORD PTR [ESI+8]
$L_6240f1::

            imul ECX,ECX,15
            shl EAX,4
            sub EAX,ECX
            jmp $L_6240fd
$L_6240fb::

            xor EAX,EAX
$L_6240fd::

            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 
FUN_623fde ENDP
ALIGN 2
FUN_6241d4 PROC

            push EBP
            mov EBP,ESP
            mov EAX,DWORD PTR [EBP+8]
            push ESI
            cmp DWORD PTR [EAX*4+(_KingData+117272)],0
            lea ESI,DWORD PTR [EAX*4+(_KingData+117272)]
            jne $L_62422a

            push EDI
            push 24
            call _malloc

            mov EDI,EAX
            pop ECX
            test EDI,EDI
            jne $L_624203

            push 17
            call FUN_61ffd2

            pop ECX
$L_624203::

            push 17
            call FUN_6241d4

            cmp DWORD PTR [ESI],0
            pop ECX
            push EDI
            jne $L_62421b

            call DWORD PTR __imp__InitializeCriticalSection@4

            mov DWORD PTR [ESI],EDI
            jmp $L_624221
$L_62421b::

            call _free

            pop ECX
$L_624221::

            push 17
            call FUN_624235

            pop ECX
            pop EDI
$L_62422a::

            push DWORD PTR [ESI]
            call DWORD PTR __imp__EnterCriticalSection@4

            pop ESI
            pop EBP
            ret 
FUN_6241d4 ENDP
FUN_624235 PROC

            push EBP
            mov EBP,ESP
            mov EAX,DWORD PTR [EBP+8]
            push DWORD PTR [EAX*4+(_KingData+117272)]
            call DWORD PTR __imp__LeaveCriticalSection@4

            pop EBP
            ret 
FUN_624235 ENDP
$L_62424a PROC

            int 3

            int 3

            push ESI
            inc EBX
            xor DH,BYTE PTR [EAX]
            pop EAX
            inc EBX
            xor BYTE PTR [EAX],DH
$L_624254::

            push EBP
            mov EBP,ESP
            sub ESP,8
            push EBX
            push ESI
            push EDI
            push EBP
            cld 
            mov EBX,DWORD PTR [EBP+12]
            mov EAX,DWORD PTR [EBP+8]
            test DWORD PTR [EAX+4],6
            jne $L_6242f4

            mov DWORD PTR [EBP-8],EAX
            mov EAX,DWORD PTR [EBP+16]
            mov DWORD PTR [EBP-4],EAX
            lea EAX,DWORD PTR [EBP-8]
            mov DWORD PTR [EBX-4],EAX
            mov ESI,DWORD PTR [EBX+12]
            mov EDI,DWORD PTR [EBX+8]
$L_624287::

            cmp ESI,-1
            je $L_6242ed

            lea ECX,DWORD PTR [ESI+ESI*2]
            cmp DWORD PTR [EDI+ECX*4+4],0
            je $L_6242db

            push ESI
            push EBP
            lea EBP,DWORD PTR [EBX+16]
            call DWORD PTR [EDI+ECX*4+4]

            pop EBP
            pop ESI
            mov EBX,DWORD PTR [EBP+12]
            or EAX,EAX
            je $L_6242db

            js $L_6242e6

            mov EDI,DWORD PTR [EBX+8]
            push EBX
            call FUN_61d0a0

            add ESP,4
            lea EBP,DWORD PTR [EBX+16]
            push ESI
            push EBX
            call FUN_61d0e2

            add ESP,8
            lea ECX,DWORD PTR [ESI+ESI*2]
            push 1
            mov EAX,DWORD PTR [EDI+ECX*4+8]
            call FUN_61d176

            mov EAX,DWORD PTR [EDI+ECX*4]
            mov DWORD PTR [EBX+12],EAX
            call DWORD PTR [EDI+ECX*4+8]
$L_6242db::

            mov EDI,DWORD PTR [EBX+8]
            lea ECX,DWORD PTR [ESI+ESI*2]
            mov ESI,DWORD PTR [EDI+ECX*4]
            jmp $L_624287
$L_6242e6::

            mov EAX,0
            jmp $L_624309
$L_6242ed::

            mov EAX,1
            jmp $L_624309
$L_6242f4::

            push EBP
            lea EBP,DWORD PTR [EBX+16]
            push -1
            push EBX
            call FUN_61d0e2

            add ESP,8
            pop EBP
            mov EAX,1
$L_624309::

            pop EBP
            pop EDI
            pop ESI
            pop EBX
            mov ESP,EBP
            pop EBP
            ret 
$L_62424a ENDP
$L_624644 PROC

            fstp ST(0)
$L_624646::

            fstp ST(0)
            fldz 
            ret 
$L_624644 ENDP
$L_62467d PROC

            fxch ST(1)
$L_62467f::

            fstp TBYTE PTR [EBP-158]
            fld TBYTE PTR [EBP-158]
            test BYTE PTR [EBP-151],64
            je $L_62469d

            mov BYTE PTR [EBP-144],7
            jmp $L_6246a4
$L_62469d::

            mov BYTE PTR [EBP-144],1
$L_6246a4::

            faddp ST(1),ST(0)
            ret 
$L_62467d ENDP
$L_6246a7 PROC

            fstp TBYTE PTR [EBP-158]
            fld TBYTE PTR [EBP-158]
            test BYTE PTR [EBP-151],64
            je $L_6246dc

            fxch ST(1)
            fstp TBYTE PTR [EBP-158]
            fld TBYTE PTR [EBP-158]
            test BYTE PTR [EBP-151],64
            je $L_6246dc

            mov BYTE PTR [EBP-144],7
            jmp $L_6246e3
$L_6246dc::

            mov BYTE PTR [EBP-144],1
$L_6246e3::

            faddp ST(1),ST(0)
            ret 
$L_6246a7 ENDP
$L_6246e6 PROC

            fstp ST(0)
$L_6246e8::

            fstp ST(0)
            fld TBYTE PTR [(_KingData+117568)]
            cmp BYTE PTR [EBP-144],0
            jg $L_624700
$L_6246e6 ENDP
FUN_6246f9 PROC

            mov BYTE PTR [EBP-144],1
$L_624700::

            or CL,CL
            ret 
FUN_6246f9 ENDP
$L_624703 PROC

            or CL,CL
            je $L_624709

            fchs 
$L_624709::

            ret 
$L_624703 ENDP
FUN_624710 PROC

            push EBP
            mov EBP,ESP
            add ESP,-32
            mov DWORD PTR [EBP-32],EAX
            mov EAX,DWORD PTR [EBP+24]
            mov DWORD PTR [EBP-16],EAX
            mov EAX,DWORD PTR [EBP+28]
            mov DWORD PTR [EBP-12],EAX
            jmp $L_624730
FUN_624710 ENDP
FUN_624727 PROC

            push EBP
            mov EBP,ESP
            add ESP,-32
            mov DWORD PTR [EBP-32],EAX
$L_624730::

            fstp QWORD PTR [EBP-8]
            mov DWORD PTR [EBP-28],ECX
            mov EAX,DWORD PTR [EBP+16]
            mov ECX,DWORD PTR [EBP+20]
            mov DWORD PTR [EBP-24],EAX
            mov DWORD PTR [EBP-20],ECX
            lea EAX,DWORD PTR [EBP+8]
            lea ECX,DWORD PTR [EBP-32]
            push EAX
            push ECX
            push EDX
            call FUN_629420

            add ESP,12
            fld QWORD PTR [EBP-8]
            cmp WORD PTR [EBP+8],639
            je $L_624761

            fldcw WORD PTR [EBP+8]
$L_624761::

            leave 
            ret 
FUN_624727 ENDP
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
ALIGN 16
FUN_624770 PROC

            fld ST(0)
            frndint 
            fsubr ST(1),ST(0)
            fxch ST(1)
            fchs 
            f2xm1 
            fld1 
            faddp ST(1),ST(0)
            fscale 
            fstp ST(1)
            ret 
FUN_624770 ENDP
FUN_624785 PROC

            mov EDX,DWORD PTR [ESP+4]
            and EDX,768
            or EDX,127
            mov WORD PTR [ESP+6],DX
            fldcw WORD PTR [ESP+6]
            ret 
FUN_624785 ENDP
ALIGN 4
FUN_62479c PROC

            test EAX,524288
            je $L_6247a9

            mov EAX,7
            ret 
$L_6247a9::

            fadd QWORD PTR [(_KingRdata+39504)]
            mov EAX,1
            ret 
FUN_62479c ENDP
FUN_6247b5 PROC

            mov EAX,DWORD PTR [EDX+4]
            and EAX,2146435072
            cmp EAX,2146435072
            je $L_6247c7

            fld QWORD PTR [EDX]
            ret 
$L_6247c7::

            mov EAX,DWORD PTR [EDX+4]
            sub ESP,10
            or EAX,2147418112
            mov DWORD PTR [ESP+6],EAX
            mov EAX,DWORD PTR [EDX+4]
            mov ECX,DWORD PTR [EDX]
            shld EAX,ECX,11
            shl ECX,11
            mov DWORD PTR [ESP+4],EAX
            mov DWORD PTR [ESP],ECX
            fld TBYTE PTR [ESP]
            add ESP,10
            test EAX,0
            mov EAX,DWORD PTR [EDX+4]
            ret 
FUN_6247b5 ENDP
ALIGN 8
FUN_6247f8 PROC

            mov EAX,DWORD PTR [ESP+8]
            and EAX,2146435072
            cmp EAX,2146435072
            je $L_624809

            ret 
$L_624809::

            mov EAX,DWORD PTR [ESP+8]
            ret 
FUN_6247f8 ENDP
$L_62480e PROC

            cmp WORD PTR [ESP],639
            je $L_624819

            fldcw WORD PTR [ESP]
$L_624819::

            pop EDX
            ret 
$L_62480e ENDP
$L_62481b PROC

            mov AX,WORD PTR [ESP]
            cmp AX,639
            je $L_624843

            and AX,32
            je $L_624840

            wait 
            fnstsw AX
            and AX,32
            je $L_624840

            mov EAX,8
            call FUN_624727

            pop EDX
            ret 
$L_624840::

            fldcw WORD PTR [ESP]
$L_624843::

            pop EDX
            ret 
$L_62481b ENDP
$L_624859 PROC

            sub ESP,8
            fst QWORD PTR [ESP]
            mov EAX,DWORD PTR [ESP+4]
            add ESP,8
            and EAX,2146435072
            je $L_6248aa
$L_62486d::

            cmp EAX,2146435072
            je $L_6248d3

            mov AX,WORD PTR [ESP]
            cmp AX,639
            je $L_6248a8

            and AX,32
            jne $L_6248a5

            wait 
            fnstsw AX
            and AX,32
            je $L_6248a5

            mov EAX,8
$L_624892::

            cmp EDX,29
            je $L_62489e

            call FUN_624727

            pop EDX
            ret 
$L_62489e::

            call FUN_624710

            pop EDX
            ret 
$L_6248a5::

            fldcw WORD PTR [ESP]
$L_6248a8::

            pop EDX
            ret 
$L_624859 ENDP
$L_6248aa PROC

            fld QWORD PTR [(_KingRdata+39548)]
            fxch ST(1)
            fscale 
            fstp ST(1)
            fld ST(0)
            fabs 
            fcomp QWORD PTR [(_KingRdata+39532)]
            wait 
            fnstsw AX
            sahf 
            mov EAX,4
            jae $L_624892

            fmul QWORD PTR [(_KingRdata+39564)]
            jmp $L_624892
$L_6248d3::

            fld QWORD PTR [(_KingRdata+39540)]
            fxch ST(1)
            fscale 
            fstp ST(1)
            fld ST(0)
            fabs 
            fcomp QWORD PTR [(_KingRdata+39524)]
            wait 
            fnstsw AX
            sahf 
            mov EAX,3
            jbe $L_624892

            fmul QWORD PTR [(_KingRdata+39556)]
            jmp $L_624892
$L_6248aa ENDP
ALIGN 4
FUN_6248fc PROC

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            fld QWORD PTR [EBP+8]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fld QWORD PTR [EBP+8]
            push ESI
            xor EDX,EDX
            xor ESI,ESI
            fnstsw AX
            sahf 
            jae $L_624919

            fchs 
$L_624919::

            fstp QWORD PTR [EBP-8]
            mov EAX,2146435072
            mov ECX,4293918720
            cmp DWORD PTR [EBP+20],EAX
            jne $L_62495f

            cmp DWORD PTR [EBP+16],EDX
            jne $L_624987

            fld QWORD PTR [EBP-8]
            fcomp QWORD PTR [(_KingRdata+1648)]
            fnstsw AX
            sahf 
            ja $L_6249a3

            fld QWORD PTR [EBP-8]
            fcomp QWORD PTR [(_KingRdata+1648)]
            fnstsw AX
            sahf 
            jb $L_6249b9
$L_62494c::

            fld QWORD PTR [(_KingData+118832)]
            mov EAX,DWORD PTR [EBP+24]
            push 1
            pop ESI
            fstp QWORD PTR [EAX]
            jmp $L_624a27
$L_62495f::

            cmp DWORD PTR [EBP+20],ECX
            jne $L_624987

            cmp DWORD PTR [EBP+16],EDX
            jne $L_624987

            fld QWORD PTR [EBP-8]
            fcomp QWORD PTR [(_KingRdata+1648)]
            fnstsw AX
            sahf 
            ja $L_6249b9

            fld QWORD PTR [EBP-8]
            fcomp QWORD PTR [(_KingRdata+1648)]
            fnstsw AX
            sahf 
            jae $L_62494c

            jmp $L_6249a3
$L_624987::

            cmp DWORD PTR [EBP+12],EAX
            jne $L_6249bd

            cmp DWORD PTR [EBP+8],EDX
            jne $L_624a27

            fld QWORD PTR [EBP+16]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            sahf 
            jbe $L_6249ab
$L_6249a3::

            fld QWORD PTR [(_KingData+118824)]
            jmp $L_624a22
$L_6249ab::

            fld QWORD PTR [EBP+16]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            sahf 
            jae $L_624a20
$L_6249b9::

            fldz 
            jmp $L_624a22
$L_6249bd::

            cmp DWORD PTR [EBP+12],ECX
            jne $L_624a27

            cmp DWORD PTR [EBP+8],EDX
            jne $L_624a27

            fld QWORD PTR [EBP+16]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            call FUN_624a2c

            fld QWORD PTR [EBP+16]
            fcomp QWORD PTR [(_KingRdata+1656)]
            pop ECX
            pop ECX
            mov ECX,EAX
            fnstsw AX
            sahf 
            jbe $L_6249fb

            fld QWORD PTR [(_KingData+118824)]
            cmp ECX,1
            jne $L_6249f3

            fchs 
$L_6249f3::

            fstp QWORD PTR [EBP+16]
            fld QWORD PTR [EBP+16]
            jmp $L_624a22
$L_6249fb::

            fld QWORD PTR [EBP+16]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            sahf 
            jae $L_624a20

            cmp ECX,1
            jne $L_624a16

            fld QWORD PTR [(_KingData+118856)]
            jmp $L_624a18
$L_624a16::

            fldz 
$L_624a18::

            fstp QWORD PTR [EBP+16]
            fld QWORD PTR [EBP+16]
            jmp $L_624a22
$L_624a20::

            fld1 
$L_624a22::

            mov EAX,DWORD PTR [EBP+24]
            fstp QWORD PTR [EAX]
$L_624a27::

            mov EAX,ESI
            pop ESI
            leave 
            ret 
FUN_6248fc ENDP
ALIGN 4
FUN_624a2c PROC

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            call FUN_625e77

            pop ECX
            test AL,144
            pop ECX
            jne $L_624a8d

            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            call FUN_6257ad

            fstp QWORD PTR [EBP-8]
            fld QWORD PTR [EBP-8]
            fcomp QWORD PTR [EBP+8]
            pop ECX
            pop ECX
            fnstsw AX
            sahf 
            jne $L_624a8d

            fld QWORD PTR [EBP+8]
            fdiv QWORD PTR [(_KingRdata+1632)]
            push ECX
            push ECX
            fstp QWORD PTR [EBP+8]
            fld QWORD PTR [EBP+8]
            fstp QWORD PTR [ESP]
            call FUN_6257ad

            fcomp QWORD PTR [EBP+8]
            pop ECX
            pop ECX
            fnstsw AX
            sahf 
            jne $L_624a88

            push 2
            jmp $L_624a8a
$L_624a88::

            push 1
$L_624a8a::

            pop EAX
            leave 
            ret 
$L_624a8d::

            xor EAX,EAX
            leave 
            ret 
FUN_624a2c ENDP
FUN_624c4d PROC

            push EBP
            mov EBP,ESP
            push -1
            push OFFSET (_KingRdata+39576)
            push OFFSET $L_624254
            mov EAX,DWORD PTR FS:[0]
            push EAX
            mov DWORD PTR FS:[0],ESP
            sub ESP,24
            push EBX
            push ESI
            push EDI
            mov ESI,DWORD PTR [EBP+8]
            imul ESI,DWORD PTR [EBP+12]
            mov DWORD PTR [EBP+12],ESI
            mov DWORD PTR [EBP-28],ESI
            cmp ESI,-32
            ja $L_624c96

            xor EBX,EBX
            cmp ESI,EBX
            jne $L_624c8b

            push 1
            pop ESI
$L_624c8b::

            add ESI,15
            and ESI,4294967280
            mov DWORD PTR [EBP+12],ESI
            jmp $L_624c98
$L_624c96::

            xor EBX,EBX
$L_624c98::

            mov DWORD PTR [EBP-32],EBX
            cmp ESI,-32
            ja $L_624d4c

            mov EAX,DWORD PTR [(_KingBss+999564)]
            cmp EAX,3
            jne $L_624cef

            mov EDI,DWORD PTR [EBP-28]
            cmp EDI,DWORD PTR [(_KingBss+999556)]
            ja $L_624d35

            push 9
            call FUN_6241d4

            pop ECX
            mov DWORD PTR [EBP-4],EBX
            push EDI
            call FUN_623333

            pop ECX
            mov DWORD PTR [EBP-32],EAX
            or DWORD PTR [EBP-4],4294967295
            call FUN_624ce6

            cmp DWORD PTR [EBP-32],EBX
            je $L_624d3a

            push DWORD PTR [EBP-28]
            jmp $L_624d29
FUN_624c4d ENDP
$L_624ce1 PROC

            xor EBX,EBX
            mov ESI,DWORD PTR [EBP+12]
$L_624ce1 ENDP
ALIGN 2
FUN_624ce6 PROC

            push 9
            call FUN_624235

            pop ECX
            ret 
FUN_624ce6 ENDP
$L_624cef PROC

            cmp EAX,2
            jne $L_624d35

            cmp ESI,DWORD PTR [(_KingData+117268)]
            ja $L_624d35

            push 9
            call FUN_6241d4

            pop ECX
            mov DWORD PTR [EBP-4],1
            mov EAX,ESI
            shr EAX,4
            push EAX
            call FUN_623dd6

            pop ECX
            mov DWORD PTR [EBP-32],EAX
            or DWORD PTR [EBP-4],4294967295
            call FUN_624d6f

            cmp DWORD PTR [EBP-32],EBX
            je $L_624d3a

            push ESI
$L_624d29::

            push EBX
            push DWORD PTR [EBP-32]
            call _memset

            add ESP,12
$L_624d35::

            cmp DWORD PTR [EBP-32],EBX
            jne $L_624d78
$L_624d3a::

            push ESI
            push 8
            push DWORD PTR [(_KingBss+999560)]
            call DWORD PTR __imp__HeapAlloc@12

            mov DWORD PTR [EBP-32],EAX
$L_624d4c::

            cmp DWORD PTR [EBP-32],EBX
            jne $L_624d78

            cmp DWORD PTR [(_KingBss+670264)],EBX
            je $L_624d78

            push ESI
            call FUN_61fd40

            pop ECX
            test EAX,EAX
            jne $L_624c98

            jmp $L_624d7b
$L_624cef ENDP
$L_624d6a PROC

            xor EBX,EBX
            mov ESI,DWORD PTR [EBP+12]
$L_624d6a ENDP
FUN_624d6f PROC

            push 9
            call FUN_624235

            pop ECX
            ret 
FUN_624d6f ENDP
$L_624d78 PROC

            mov EAX,DWORD PTR [EBP-32]
$L_624d7b::

            mov ECX,DWORD PTR [EBP-16]
            mov DWORD PTR FS:[0],ECX
            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 
$L_624d78 ENDP
FUN_624fca PROC

            mov EAX,DWORD PTR [(_KingBss+670076)]
            cmp EAX,1
            je $L_624fe1

            test EAX,EAX
            jne $L_625002

            cmp DWORD PTR [(_KingData+108184)],1
            jne $L_625002
$L_624fe1::

            push 252
            call FUN_625003

            mov EAX,DWORD PTR [(_KingBss+670332)]
            pop ECX
            test EAX,EAX
            je $L_624ff7

            call EAX
$L_624ff7::

            push 255
            call FUN_625003

            pop ECX
$L_625002::

            ret 
FUN_624fca ENDP
FUN_625003 PROC

            push EBP
            mov EBP,ESP
            sub ESP,420
            mov EDX,DWORD PTR [EBP+8]
            xor ECX,ECX
            mov EAX,OFFSET (_KingData+117928)
$L_625016::

            cmp EDX,DWORD PTR [EAX]
            je $L_625025

            add EAX,8
            inc ECX
            cmp EAX,OFFSET (_KingData+118072)
            jl $L_625016
$L_625025::

            push ESI
            mov ESI,ECX
            shl ESI,3
            cmp EDX,DWORD PTR [ESI+(_KingData+117928)]
            jne $L_625153

            mov EAX,DWORD PTR [(_KingBss+670076)]
            cmp EAX,1
            je $L_62512d

            test EAX,EAX
            jne $L_625056

            cmp DWORD PTR [(_KingData+108184)],1
            je $L_62512d
$L_625056::

            cmp EDX,252
            je $L_625153

            lea EAX,DWORD PTR [EBP-420]
            push 260
            push EAX
            push 0
            call DWORD PTR __imp__GetModuleFileNameA@12

            test EAX,EAX
            jne $L_62508d

            lea EAX,DWORD PTR [EBP-420]
            push OFFSET (_KingRdata+40324)
            push EAX
            call _strcpy

            pop ECX
            pop ECX
$L_62508d::

            lea EAX,DWORD PTR [EBP-420]
            push EDI
            push EAX
            lea EDI,DWORD PTR [EBP-420]
            call _strlen

            inc EAX
            pop ECX
            cmp EAX,60
            jbe $L_6250d0

            lea EAX,DWORD PTR [EBP-420]
            push EAX
            call _strlen

            mov EDI,EAX
            lea EAX,DWORD PTR [EBP-420]
            sub EAX,59
            push 3
            add EDI,EAX
            push OFFSET (_KingData+44200)
            push EDI
            call _strncpy

            add ESP,16
$L_6250d0::

            lea EAX,DWORD PTR [EBP-160]
            push OFFSET (_KingRdata+40296)
            push EAX
            call _strcpy

            lea EAX,DWORD PTR [EBP-160]
            push EDI
            push EAX
            call FUN_61f2b0

            lea EAX,DWORD PTR [EBP-160]
            push OFFSET (_KingRdata+40292)
            push EAX
            call FUN_61f2b0

            push DWORD PTR [ESI+(_KingData+117932)]
            lea EAX,DWORD PTR [EBP-160]
            push EAX
            call FUN_61f2b0

            push 73744
            lea EAX,DWORD PTR [EBP-160]
            push OFFSET (_KingRdata+40252)
            push EAX
            call FUN_62952e

            add ESP,44
            pop EDI
            jmp $L_625153
$L_62512d::

            lea EAX,DWORD PTR [EBP+8]
            lea ESI,DWORD PTR [ESI+(_KingData+117932)]
            push 0
            push EAX
            push DWORD PTR [ESI]
            call _strlen

            pop ECX
            push EAX
            push DWORD PTR [ESI]
            push -12
            call DWORD PTR __imp__GetStdHandle@4

            push EAX
            call DWORD PTR __imp__WriteFile@20
$L_625153::

            pop ESI
            leave 
            ret 
FUN_625003 ENDP
ALIGN 2
FUN_6254d5 PROC

            push EBP
            mov EBP,ESP
            sub ESP,1044
            push EBX
            push ESI
            push EDI
            xor EDI,EDI
            cmp DWORD PTR [EBP+16],EDI
            mov DWORD PTR [EBP-8],EDI
            mov DWORD PTR [EBP-16],EDI
            jne $L_6254f5
$L_6254ee::

            xor EAX,EAX
            jmp $L_62565b
$L_6254f5::

            mov EAX,DWORD PTR [EBP+8]
            sar EAX,5
            lea EBX,DWORD PTR [EAX*4+(_KingBss+999264)]
            mov EAX,DWORD PTR [EBP+8]
            and EAX,31
            lea ESI,DWORD PTR [EAX+EAX*8]
            mov EAX,DWORD PTR [EBX]
            shl ESI,2
            test BYTE PTR [EAX+ESI*1+4],32
            je $L_625525

            push 2
            push EDI
            push DWORD PTR [EBP+8]
            call FUN_6256c5

            add ESP,12
$L_625525::

            mov EAX,DWORD PTR [EBX]
            add EAX,ESI
            test BYTE PTR [EAX+4],128
            je $L_6255f4

            mov EAX,DWORD PTR [EBP+12]
            cmp DWORD PTR [EBP+16],EDI
            mov DWORD PTR [EBP-4],EAX
            mov DWORD PTR [EBP+8],EDI
            jbe $L_62562f
$L_625545::

            lea EAX,DWORD PTR [EBP-1044]
$L_62554b::

            mov ECX,DWORD PTR [EBP-4]
            sub ECX,DWORD PTR [EBP+12]
            cmp ECX,DWORD PTR [EBP+16]
            jae $L_62557f

            mov ECX,DWORD PTR [EBP-4]
            inc DWORD PTR [EBP-4]
            mov CL,BYTE PTR [ECX]
            cmp CL,10
            jne $L_62556a

            inc DWORD PTR [EBP-16]
            mov BYTE PTR [EAX],13
            inc EAX
$L_62556a::

            mov BYTE PTR [EAX],CL
            inc EAX
            mov ECX,EAX
            lea EDX,DWORD PTR [EBP-1044]
            sub ECX,EDX
            cmp ECX,1024
            jl $L_62554b
$L_62557f::

            mov EDI,EAX
            lea EAX,DWORD PTR [EBP-1044]
            sub EDI,EAX
            lea EAX,DWORD PTR [EBP-12]
            push 0
            push EAX
            lea EAX,DWORD PTR [EBP-1044]
            push EDI
            push EAX
            mov EAX,DWORD PTR [EBX]
            push DWORD PTR [EAX+ESI*1]
            call DWORD PTR __imp__WriteFile@20

            test EAX,EAX
            je $L_6255e9

            mov EAX,DWORD PTR [EBP-12]
            add DWORD PTR [EBP-8],EAX
            cmp EAX,EDI
            jl $L_6255bb

            mov EAX,DWORD PTR [EBP-4]
            sub EAX,DWORD PTR [EBP+12]
            cmp EAX,DWORD PTR [EBP+16]
            jb $L_625545
$L_6255bb::

            xor EDI,EDI
$L_6255bd::

            mov EAX,DWORD PTR [EBP-8]
            cmp EAX,EDI
            jne $L_625658

            cmp DWORD PTR [EBP+8],EDI
            je $L_62562f

            push 5
            pop ESI
            cmp DWORD PTR [EBP+8],ESI
            jne $L_625621

            call __errno

            mov DWORD PTR [EAX],9
            call ___doserrno

            mov DWORD PTR [EAX],ESI
            jmp $L_62562a
$L_6255e9::

            call DWORD PTR __imp__GetLastError@0

            mov DWORD PTR [EBP+8],EAX
            jmp $L_6255bb
$L_6255f4::

            lea ECX,DWORD PTR [EBP-12]
            push EDI
            push ECX
            push DWORD PTR [EBP+16]
            push DWORD PTR [EBP+12]
            push DWORD PTR [EAX]
            call DWORD PTR __imp__WriteFile@20

            test EAX,EAX
            je $L_625616

            mov EAX,DWORD PTR [EBP-12]
            mov DWORD PTR [EBP+8],EDI
            mov DWORD PTR [EBP-8],EAX
            jmp $L_6255bd
$L_625616::

            call DWORD PTR __imp__GetLastError@0

            mov DWORD PTR [EBP+8],EAX
            jmp $L_6255bd
$L_625621::

            push DWORD PTR [EBP+8]
            call FUN_620d80

            pop ECX
$L_62562a::

            or EAX,4294967295
            jmp $L_62565b
$L_62562f::

            mov EAX,DWORD PTR [EBX]
            test BYTE PTR [EAX+ESI*1+4],64
            je $L_625644

            mov EAX,DWORD PTR [EBP+12]
            cmp BYTE PTR [EAX],26
            je $L_6254ee
$L_625644::

            call __errno

            mov DWORD PTR [EAX],28
            call ___doserrno

            mov DWORD PTR [EAX],EDI
            jmp $L_62562a
$L_625658::

            sub EAX,DWORD PTR [EBP-16]
$L_62565b::

            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 
FUN_6254d5 ENDP
ALIGN 16
FUN_6256c5 PROC

            push ESI
            mov ESI,DWORD PTR [ESP+8]
            push EDI
            push ESI
            call FUN_628d7d

            cmp EAX,-1
            pop ECX
            jne $L_6256e4

            call __errno

            mov DWORD PTR [EAX],9
            jmp $L_625711
$L_6256e4::

            push DWORD PTR [ESP+20]
            push 0
            push DWORD PTR [ESP+24]
            push EAX
            call DWORD PTR __imp__SetFilePointer@16

            mov EDI,EAX
            cmp EDI,-1
            jne $L_625704

            call DWORD PTR __imp__GetLastError@0

            jmp $L_625706
$L_625704::

            xor EAX,EAX
$L_625706::

            test EAX,EAX
            je $L_625716

            push EAX
            call FUN_620d80

            pop ECX
$L_625711::

            or EAX,4294967295
            jmp $L_625735
$L_625716::

            mov ECX,ESI
            and ESI,31
            sar ECX,5
            mov EAX,ESI
            mov ECX,DWORD PTR [ECX*4+(_KingBss+999264)]
            lea EAX,DWORD PTR [EAX+EAX*8]
            and BYTE PTR [ECX+EAX*4+4],253
            lea EAX,DWORD PTR [ECX+EAX*4+4]
            mov EAX,EDI
$L_625735::

            pop EDI
            pop ESI
            ret 
FUN_6256c5 ENDP
ALIGN 8
FUN_6257ad PROC

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            fld QWORD PTR [EBP+8]
            frndint 
            fstp QWORD PTR [EBP-8]
            fld QWORD PTR [EBP-8]
            leave 
            ret 
FUN_6257ad ENDP
FUN_6257bf PROC

            push EBP
            mov EBP,ESP
            cmp DWORD PTR [(_KingBss+999232)],0
            jne $L_6257f4

            push DWORD PTR [EBP+20]
            fld QWORD PTR [EBP+12]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push ECX
            push ECX
            fldz 
            fstp QWORD PTR [ESP]
            fld QWORD PTR [EBP+12]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push DWORD PTR [EBP+8]
            push 1
            call FUN_625d75

            add ESP,36
            pop EBP
            ret 
$L_6257f4::

            call __errno

            push 65535
            mov DWORD PTR [EAX],33
            push DWORD PTR [EBP+20]
            call FUN_62606a

            fld QWORD PTR [EBP+12]
            pop ECX
            pop ECX
            pop EBP
            ret 
FUN_6257bf ENDP
FUN_625813 PROC

            push EBP
            mov EBP,ESP
            sub ESP,88
            push DWORD PTR [EBP+32]
            lea EAX,DWORD PTR [EBP+24]
            push EAX
            push DWORD PTR [EBP+8]
            call FUN_625b5e

            add ESP,12
            test EAX,EAX
            jne $L_625851

            lea EAX,DWORD PTR [EBP+24]
            and DWORD PTR [EBP-32],4294967294
            push EAX
            lea EAX,DWORD PTR [EBP+16]
            push EAX
            lea EAX,DWORD PTR [EBP+32]
            push DWORD PTR [EBP+12]
            push DWORD PTR [EBP+8]
            push EAX
            lea EAX,DWORD PTR [EBP-88]
            push EAX
            call FUN_6258ab

            add ESP,24
$L_625851::

            push DWORD PTR [EBP+8]
            call FUN_625e4a

            cmp DWORD PTR [(_KingBss+999232)],0
            pop ECX
            jne $L_62588f

            test EAX,EAX
            je $L_62588f

            push DWORD PTR [EBP+32]
            fld QWORD PTR [EBP+24]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push ECX
            push ECX
            fldz 
            fstp QWORD PTR [ESP]
            fld QWORD PTR [EBP+16]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            push DWORD PTR [EBP+12]
            push EAX
            call FUN_625d75

            add ESP,36
            leave 
            ret 
$L_62588f::

            push EAX
            call FUN_625dfd

            mov DWORD PTR [ESP],65535
            push DWORD PTR [EBP+32]
            call FUN_62606a

            fld QWORD PTR [EBP+24]
            pop ECX
            pop ECX
            leave 
            ret 
FUN_625813 ENDP
FUN_6258ab PROC

            push EBP
            mov EBP,ESP
            mov ECX,DWORD PTR [EBP+8]
            xor EAX,EAX
            push EBX
            push ESI
            mov DWORD PTR [ECX+4],EAX
            mov ECX,DWORD PTR [EBP+8]
            push EDI
            push 1
            mov DWORD PTR [ECX+8],EAX
            mov ECX,DWORD PTR [EBP+8]
            pop EBX
            mov DWORD PTR [ECX+12],EAX
            mov CL,BYTE PTR [EBP+16]
            test CL,16
            je $L_6258dd

            mov EAX,DWORD PTR [EBP+8]
            mov DWORD PTR [EBP+16],3221225615
            or DWORD PTR [EAX+4],EBX
$L_6258dd::

            test CL,2
            je $L_6258f0

            mov EAX,DWORD PTR [EBP+8]
            mov DWORD PTR [EBP+16],3221225619
            or DWORD PTR [EAX+4],2
$L_6258f0::

            test BL,CL
            je $L_625902

            mov EAX,DWORD PTR [EBP+8]
            mov DWORD PTR [EBP+16],3221225617
            or DWORD PTR [EAX+4],4
$L_625902::

            test CL,4
            je $L_625915

            mov EAX,DWORD PTR [EBP+8]
            mov DWORD PTR [EBP+16],3221225614
            or DWORD PTR [EAX+4],8
$L_625915::

            test CL,8
            je $L_625928

            mov EAX,DWORD PTR [EBP+8]
            mov DWORD PTR [EBP+16],3221225616
            or DWORD PTR [EAX+4],16
$L_625928::

            mov ESI,DWORD PTR [EBP+12]
            mov EAX,DWORD PTR [EBP+8]
            push 2
            mov ECX,DWORD PTR [ESI]
            mov EDX,DWORD PTR [EAX+8]
            not ECX
            and ECX,EBX
            and EDX,4294967279
            shl ECX,4
            or ECX,EDX
            pop EDI
            mov DWORD PTR [EAX+8],ECX
            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP+8]
            not ECX
            mov EDX,DWORD PTR [EAX+8]
            and ECX,4
            shl ECX,1
            and EDX,4294967287
            or ECX,EDX
            mov DWORD PTR [EAX+8],ECX
            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP+8]
            not ECX
            mov EDX,DWORD PTR [EAX+8]
            shr ECX,1
            and ECX,4
            and EDX,4294967291
            or ECX,EDX
            mov DWORD PTR [EAX+8],ECX
            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP+8]
            not ECX
            mov EDX,DWORD PTR [EAX+8]
            shr ECX,3
            and ECX,EDI
            and EDX,4294967293
            or ECX,EDX
            mov DWORD PTR [EAX+8],ECX
            mov ECX,DWORD PTR [ESI]
            mov EAX,DWORD PTR [EBP+8]
            not ECX
            mov EDX,DWORD PTR [EAX+8]
            shr ECX,5
            and ECX,EBX
            and EDX,4294967294
            or ECX,EDX
            mov DWORD PTR [EAX+8],ECX
            call FUN_62604d

            test BL,AL
            je $L_6259b1

            mov ECX,DWORD PTR [EBP+8]
            or DWORD PTR [ECX+12],16
$L_6259b1::

            test AL,4
            je $L_6259bc

            mov ECX,DWORD PTR [EBP+8]
            or DWORD PTR [ECX+12],8
$L_6259bc::

            test AL,8
            je $L_6259c7

            mov ECX,DWORD PTR [EBP+8]
            or DWORD PTR [ECX+12],4
$L_6259c7::

            test AL,16
            je $L_6259d1

            mov ECX,DWORD PTR [EBP+8]
            or DWORD PTR [ECX+12],EDI
$L_6259d1::

            test AL,32
            je $L_6259db

            mov EAX,DWORD PTR [EBP+8]
            or DWORD PTR [EAX+12],EBX
$L_6259db::

            mov EAX,DWORD PTR [ESI]
            mov ECX,3072
            and EAX,ECX
            je $L_625a1a

            cmp EAX,1024
            je $L_625a0c

            cmp EAX,2048
            je $L_625a00

            cmp EAX,ECX
            jne $L_625a20

            mov EAX,DWORD PTR [EBP+8]
            or DWORD PTR [EAX],3
            jmp $L_625a20
$L_625a00::

            mov EAX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EAX]
            and ECX,4294967294
            or ECX,EDI
            jmp $L_625a16
$L_625a0c::

            mov EAX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EAX]
            and ECX,4294967293
            or ECX,EBX
$L_625a16::

            mov DWORD PTR [EAX],ECX
            jmp $L_625a20
$L_625a1a::

            mov EAX,DWORD PTR [EBP+8]
            and DWORD PTR [EAX],4294967292
$L_625a20::

            mov EAX,DWORD PTR [ESI]
            mov ECX,768
            and EAX,ECX
            je $L_625a4b

            cmp EAX,512
            je $L_625a3e

            cmp EAX,ECX
            jne $L_625a58

            mov EAX,DWORD PTR [EBP+8]
            and DWORD PTR [EAX],4294967267
            jmp $L_625a58
$L_625a3e::

            mov EAX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EAX]
            and ECX,4294967271
            or ECX,4
            jmp $L_625a56
$L_625a4b::

            mov EAX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EAX]
            and ECX,4294967275
            or ECX,8
$L_625a56::

            mov DWORD PTR [EAX],ECX
$L_625a58::

            mov EAX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EBP+20]
            and ECX,4095
            mov EDX,DWORD PTR [EAX]
            shl ECX,5
            and EDX,4294836255
            or ECX,EDX
            mov DWORD PTR [EAX],ECX
            mov EAX,DWORD PTR [EBP+8]
            or DWORD PTR [EAX+32],EBX
            mov EAX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EAX+32]
            and ECX,4294967267
            or ECX,EDI
            mov DWORD PTR [EAX+32],ECX
            mov EAX,DWORD PTR [EBP+24]
            fld QWORD PTR [EAX]
            mov EAX,DWORD PTR [EBP+8]
            fstp QWORD PTR [EAX+16]
            mov EAX,DWORD PTR [EBP+8]
            or DWORD PTR [EAX+80],EBX
            mov EAX,DWORD PTR [EBP+8]
            mov ECX,DWORD PTR [EAX+80]
            and ECX,4294967267
            or ECX,EDI
            mov EDI,DWORD PTR [EBP+28]
            mov DWORD PTR [EAX+80],ECX
            mov EAX,DWORD PTR [EBP+8]
            fld QWORD PTR [EDI]
            fstp QWORD PTR [EAX+64]
            call FUN_62605b

            lea EAX,DWORD PTR [EBP+8]
            push EAX
            push EBX
            push 0
            push DWORD PTR [EBP+16]
            call DWORD PTR __imp__RaiseException@16

            mov EAX,DWORD PTR [EBP+8]
            test BYTE PTR [EAX+8],16
            je $L_625ad2

            and DWORD PTR [ESI],4294967294
$L_625ad2::

            test BYTE PTR [EAX+8],8
            je $L_625adb

            and DWORD PTR [ESI],4294967291
$L_625adb::

            test BYTE PTR [EAX+8],4
            je $L_625ae4

            and DWORD PTR [ESI],4294967287
$L_625ae4::

            test BYTE PTR [EAX+8],2
            je $L_625aed

            and DWORD PTR [ESI],4294967279
$L_625aed::

            test BYTE PTR [EAX+8],BL
            je $L_625af5

            and DWORD PTR [ESI],4294967263
$L_625af5::

            mov ECX,DWORD PTR [EAX]
            mov EDX,4294964223
            and ECX,3
            sub ECX,0
            je $L_625b29

            dec ECX
            je $L_625b1d

            dec ECX
            je $L_625b13

            dec ECX
            jne $L_625b2b

            or BYTE PTR [ESI+1],12
            jmp $L_625b2b
$L_625b13::

            mov ECX,DWORD PTR [ESI]
            and CH,251
            or CH,8
            jmp $L_625b25
$L_625b1d::

            mov ECX,DWORD PTR [ESI]
            and CH,247
            or CH,4
$L_625b25::

            mov DWORD PTR [ESI],ECX
            jmp $L_625b2b
$L_625b29::

            and DWORD PTR [ESI],EDX
$L_625b2b::

            mov ECX,DWORD PTR [EAX]
            shr ECX,2
            and ECX,7
            sub ECX,0
            je $L_625b4b

            dec ECX
            je $L_625b42

            dec ECX
            jne $L_625b54

            and DWORD PTR [ESI],EDX
            jmp $L_625b54
$L_625b42::

            mov ECX,DWORD PTR [ESI]
            and ECX,EDX
            or CH,2
            jmp $L_625b52
$L_625b4b::

            mov ECX,DWORD PTR [ESI]
            and ECX,EDX
            or CH,3
$L_625b52::

            mov DWORD PTR [ESI],ECX
$L_625b54::

            fld QWORD PTR [EAX+64]
            fstp QWORD PTR [EDI]
            pop EDI
            pop ESI
            pop EBX
            pop EBP
            ret 
FUN_6258ab ENDP
ALIGN 2
FUN_625b5e PROC

            push EBP
            mov EBP,ESP
            sub ESP,12
            mov EAX,DWORD PTR [EBP+8]
            push EBX
            push EDI
            mov EDI,EAX
            and EDI,31
            push 1
            test AL,8
            pop EBX
            je $L_625b89

            test BYTE PTR [EBP+16],BL
            je $L_625b89

            push EBX
            call FUN_62608d

            pop ECX
            and EDI,4294967287
            jmp $L_625d53
$L_625b89::

            test AL,4
            je $L_625ba3

            test BYTE PTR [EBP+16],4
            je $L_625ba3

            push 4
            call FUN_62608d

            pop ECX
            and EDI,4294967291
            jmp $L_625d53
$L_625ba3::

            test BL,AL
            je $L_625c7d

            test BYTE PTR [EBP+16],8
            je $L_625c7d

            push 8
            call FUN_62608d

            pop ECX
            mov EAX,3072
            mov ECX,DWORD PTR [EBP+16]
            and ECX,EAX
            je $L_625c55

            cmp ECX,1024
            je $L_625c2d

            cmp ECX,2048
            je $L_625c05

            cmp ECX,EAX
            jne $L_625c75

            mov ECX,DWORD PTR [EBP+12]
            fld QWORD PTR [ECX]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fld QWORD PTR [(_KingData+118840)]
            fnstsw AX
            sahf 
            ja $L_625bfd

            fchs 
$L_625bfd::

            fstp QWORD PTR [EBP-12]
            fld QWORD PTR [EBP-12]
            jmp $L_625c73
$L_625c05::

            mov ECX,DWORD PTR [EBP+12]
            fld QWORD PTR [ECX]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            sahf 
            jbe $L_625c1d

            fld QWORD PTR [(_KingData+118824)]
            jmp $L_625c25
$L_625c1d::

            fld QWORD PTR [(_KingData+118840)]
            fchs 
$L_625c25::

            fstp QWORD PTR [EBP-12]
            fld QWORD PTR [EBP-12]
            jmp $L_625c73
$L_625c2d::

            mov ECX,DWORD PTR [EBP+12]
            fld QWORD PTR [ECX]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            sahf 
            jbe $L_625c45

            fld QWORD PTR [(_KingData+118840)]
            jmp $L_625c4d
$L_625c45::

            fld QWORD PTR [(_KingData+118824)]
            fchs 
$L_625c4d::

            fstp QWORD PTR [EBP-12]
            fld QWORD PTR [EBP-12]
            jmp $L_625c73
$L_625c55::

            mov ECX,DWORD PTR [EBP+12]
            fld QWORD PTR [ECX]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fld QWORD PTR [(_KingData+118824)]
            fnstsw AX
            sahf 
            ja $L_625c6d

            fchs 
$L_625c6d::

            fstp QWORD PTR [EBP-12]
            fld QWORD PTR [EBP-12]
$L_625c73::

            fstp QWORD PTR [ECX]
$L_625c75::

            and EDI,4294967294
            jmp $L_625d53
$L_625c7d::

            test AL,2
            je $L_625d53

            test BYTE PTR [EBP+16],16
            je $L_625d53

            push ESI
            xor ESI,ESI
            test AL,16
            je $L_625c98

            mov ESI,EBX
$L_625c98::

            mov EAX,DWORD PTR [EBP+12]
            fld QWORD PTR [EAX]
            fstp QWORD PTR [EBP-12]
            fld QWORD PTR [EBP-12]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            sahf 
            je $L_625d41

            fld QWORD PTR [EBP-12]
            lea EAX,DWORD PTR [EBP-4]
            push EAX
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            call FUN_625f8c

            mov EAX,DWORD PTR [EBP-4]
            add ESP,12
            fstp QWORD PTR [EBP-12]
            lea ECX,DWORD PTR [EAX-1536]
            cmp ECX,4294966222
            jge $L_625ce3

            fldz 
            mov ESI,EBX
            fstp QWORD PTR [EBP-12]
            jmp $L_625d37
$L_625ce3::

            fld QWORD PTR [EBP-12]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            sahf 
            jae $L_625cf5

            mov EDX,EBX
            jmp $L_625cf7
$L_625cf5::

            xor EDX,EDX
$L_625cf7::

            mov AL,BYTE PTR [EBP-6]
            and EAX,15
            or AL,16
            mov WORD PTR [EBP-6],AX
            mov EAX,4294966275
            cmp ECX,EAX
            jge $L_625d2b

            sub EAX,ECX
$L_625d0e::

            test BYTE PTR [EBP-12],BL
            je $L_625d19

            test ESI,ESI
            jne $L_625d19

            mov ESI,EBX
$L_625d19::

            shr DWORD PTR [EBP-12],1
            test BYTE PTR [EBP-8],BL
            je $L_625d25

            or BYTE PTR [EBP-9],128
$L_625d25::

            shr DWORD PTR [EBP-8],1
            dec EAX
            jne $L_625d0e
$L_625d2b::

            test EDX,EDX
            je $L_625d37

            fld QWORD PTR [EBP-12]
            fchs 
            fstp QWORD PTR [EBP-12]
$L_625d37::

            fld QWORD PTR [EBP-12]
            mov EAX,DWORD PTR [EBP+12]
            fstp QWORD PTR [EAX]
            jmp $L_625d43
$L_625d41::

            mov ESI,EBX
$L_625d43::

            test ESI,ESI
            pop ESI
            je $L_625d50

            push 16
            call FUN_62608d

            pop ECX
$L_625d50::

            and EDI,4294967293
$L_625d53::

            test BYTE PTR [EBP+8],16
            je $L_625d6a

            test BYTE PTR [EBP+16],32
            je $L_625d6a

            push 32
            call FUN_62608d

            pop ECX
            and EDI,4294967279
$L_625d6a::

            xor EAX,EAX
            test EDI,EDI
            pop EDI
            pop EBX
            sete AL
            leave 
            ret 
FUN_625b5e ENDP
FUN_625d75 PROC

            push EBP
            mov EBP,ESP
            sub ESP,32
            push DWORD PTR [EBP+12]
            call FUN_625e25

            test EAX,EAX
            pop ECX
            mov DWORD PTR [EBP-28],EAX
            je $L_625de0

            mov EAX,DWORD PTR [EBP+16]
            push ESI
            mov DWORD PTR [EBP-24],EAX
            mov EAX,DWORD PTR [EBP+20]
            mov DWORD PTR [EBP-20],EAX
            mov EAX,DWORD PTR [EBP+24]
            mov ESI,DWORD PTR [EBP+8]
            mov DWORD PTR [EBP-16],EAX
            mov EAX,DWORD PTR [EBP+28]
            push 65535
            push DWORD PTR [EBP+40]
            mov DWORD PTR [EBP-12],EAX
            mov EAX,DWORD PTR [EBP+32]
            mov DWORD PTR [EBP-32],ESI
            mov DWORD PTR [EBP-8],EAX
            mov EAX,DWORD PTR [EBP+36]
            mov DWORD PTR [EBP-4],EAX
            call FUN_62606a

            lea EAX,DWORD PTR [EBP-32]
            push EAX
            call _World_MathErr

            add ESP,12
            test EAX,EAX
            jne $L_625dda

            push ESI
            call FUN_625dfd

            pop ECX
$L_625dda::

            fld QWORD PTR [EBP-8]
            pop ESI
            leave 
            ret 
$L_625de0::

            push 65535
            push DWORD PTR [EBP+40]
            call FUN_62606a

            push DWORD PTR [EBP+8]
            call FUN_625dfd

            fld QWORD PTR [EBP+32]
            add ESP,12
            leave 
            ret 
FUN_625d75 ENDP
FUN_625dfd PROC

            mov EAX,DWORD PTR [ESP+4]
            cmp EAX,1
            je $L_625e19

            jle $L_625e24

            cmp EAX,3
            jg $L_625e24

            call __errno

            mov DWORD PTR [EAX],34
            ret 
$L_625e19::

            call __errno

            mov DWORD PTR [EAX],33
$L_625e24::

            ret 
FUN_625dfd ENDP
FUN_625e25 PROC

            xor ECX,ECX
            mov EAX,OFFSET (_KingData+118608)
$L_625e2c::

            mov EDX,DWORD PTR [EAX]
            cmp EDX,DWORD PTR [ESP+4]
            je $L_625e42

            add EAX,8
            inc ECX
            cmp EAX,OFFSET (_KingData+118824)
            jl $L_625e2c

            xor EAX,EAX
            ret 
$L_625e42::

            mov EAX,DWORD PTR [ECX*8+(_KingData+118608)+4]
            ret 
FUN_625e25 ENDP
ALIGN 2
FUN_625e4a PROC

            mov AL,BYTE PTR [ESP+4]
            test AL,32
            je $L_625e56

            push 5
            jmp $L_625e6c
$L_625e56::

            test AL,8
            je $L_625e5e

            push 1
            jmp $L_625e6c
$L_625e5e::

            test AL,4
            je $L_625e66

            push 2
            jmp $L_625e6c
$L_625e66::

            test AL,1
            je $L_625e6e

            push 3
$L_625e6c::

            pop EAX
            ret 
$L_625e6e::

            movzx EAX,AL
            and EAX,2
            shl EAX,1
            ret 
FUN_625e4a ENDP
FUN_625e77 PROC

            push EBP
            mov EBP,ESP
            mov EAX,DWORD PTR [EBP+14]
            mov ECX,32752
            mov EDX,EAX
            and EDX,ECX
            cmp DX,CX
            jne $L_625eb7

            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            call FUN_625f32

            pop ECX
            dec EAX
            pop ECX
            je $L_625eb0

            dec EAX
            je $L_625eac

            dec EAX
            je $L_625ea8

            push 1
$L_625ea5::

            pop EAX
            pop EBP
            ret 
$L_625ea8::

            push 2
            jmp $L_625ea5
$L_625eac::

            push 4
            jmp $L_625ea5
$L_625eb0::

            mov EAX,512
            pop EBP
            ret 
$L_625eb7::

            and EAX,32768
            test DX,DX
            mov ECX,EAX
            jne $L_625ee1

            test DWORD PTR [EBP+12],1048575
            jne $L_625ed2

            cmp DWORD PTR [EBP+8],0
            je $L_625ee1
$L_625ed2::

            mov EAX,ECX
            neg EAX
            sbb EAX,EAX
            and AL,144
            add EAX,128
            pop EBP
            ret 
$L_625ee1::

            fld QWORD PTR [EBP+8]
            fcomp QWORD PTR [(_KingRdata+1656)]
            fnstsw AX
            sahf 
            mov EAX,ECX
            jne $L_625efc

            neg EAX
            sbb EAX,EAX
            and AL,224
            add EAX,64
            pop EBP
            ret 
$L_625efc::

            neg EAX
            sbb EAX,EAX
            and AL,8
            add EAX,256
            pop EBP
            ret 
FUN_625e77 ENDP
FUN_625f09 PROC

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            mov EAX,DWORD PTR [EBP+16]
            mov ECX,DWORD PTR [EBP+14]
            fld QWORD PTR [EBP+8]
            add EAX,1022
            and CX,32783
            fstp QWORD PTR [EBP-8]
            shl EAX,4
            or EAX,ECX
            mov WORD PTR [EBP-2],AX
            fld QWORD PTR [EBP-8]
            leave 
            ret 
FUN_625f09 ENDP
ALIGN 2
FUN_625f32 PROC

            push EBP
            mov EBP,ESP
            xor EDX,EDX
            cmp DWORD PTR [EBP+12],2146435072
            jne $L_625f49

            cmp DWORD PTR [EBP+8],EDX
            jne $L_625f5b

            push 1
            jmp $L_625f85
$L_625f49::

            cmp DWORD PTR [EBP+12],4293918720
            jne $L_625f5b

            cmp DWORD PTR [EBP+8],EDX
            jne $L_625f5b

            push 2
            jmp $L_625f85
$L_625f5b::

            mov ECX,DWORD PTR [EBP+14]
            mov EAX,32760
            and ECX,EAX
            cmp CX,AX
            jne $L_625f6e

            push 3
            jmp $L_625f85
$L_625f6e::

            cmp CX,32752
            jne $L_625f88

            test DWORD PTR [EBP+12],524287
            jne $L_625f83

            cmp DWORD PTR [EBP+8],EDX
            je $L_625f88
$L_625f83::

            push 4
$L_625f85::

            pop EAX
            pop EBP
            ret 
$L_625f88::

            xor EAX,EAX
            pop EBP
            ret 
FUN_625f32 ENDP
ALIGN 4
FUN_625f8c PROC

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            fld QWORD PTR [EBP+8]
            fcomp QWORD PTR [(_KingRdata+1656)]
            push ESI
            fnstsw AX
            sahf 
            jne $L_625fac

            fldz 
            xor ESI,ESI
            fstp QWORD PTR [EBP-8]
            jmp $L_626042
$L_625fac::

            xor ECX,ECX
            test WORD PTR [EBP+14],32752
            jne $L_62601b

            test DWORD PTR [EBP+12],1048575
            jne $L_625fc4

            cmp DWORD PTR [EBP+8],ECX
            je $L_62601b
$L_625fc4::

            fld QWORD PTR [EBP+8]
            fcomp QWORD PTR [(_KingRdata+1656)]
            mov ESI,4294966275
            fnstsw AX
            sahf 
            jae $L_625fdc

            push 1
            pop EAX
            jmp $L_625fde
$L_625fdc::

            xor EAX,EAX
$L_625fde::

            test BYTE PTR [EBP+14],16
            jne $L_625ff7

            shl DWORD PTR [EBP+12],1
            test BYTE PTR [EBP+11],128
            je $L_625ff1

            or DWORD PTR [EBP+12],1
$L_625ff1::

            shl DWORD PTR [EBP+8],1
            dec ESI
            jmp $L_625fde
$L_625ff7::

            and WORD PTR [EBP+14],65519
            cmp EAX,ECX
            je $L_626005

            or BYTE PTR [EBP+15],128
$L_626005::

            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            call FUN_625f09

            fstp QWORD PTR [EBP-8]
            add ESP,12
            jmp $L_626042
$L_62601b::

            fld QWORD PTR [EBP+8]
            push ECX
            push ECX
            push ECX
            fstp QWORD PTR [ESP]
            call FUN_625f09

            mov EAX,DWORD PTR [EBP+14]
            add ESP,12
            fstp QWORD PTR [EBP-8]
            shr EAX,4
            and AX,2047
            movsx ESI,AX
            sub ESI,1022
$L_626042::

            mov EAX,DWORD PTR [EBP+16]
            fld QWORD PTR [EBP-8]
            mov DWORD PTR [EAX],ESI
            pop ESI
            leave 
            ret 
FUN_625f8c ENDP
FUN_62604d PROC

            push EBP
            mov EBP,ESP
            push ECX
            wait 
            fnstsw WORD PTR [EBP-2]
            movsx EAX,WORD PTR [EBP-2]
            leave 
            ret 
FUN_62604d ENDP
FUN_62605b PROC

            push EBP
            mov EBP,ESP
            push ECX
            fnstsw WORD PTR [EBP-2]
            fnclex 
            movsx EAX,WORD PTR [EBP-2]
            leave 
            ret 
FUN_62605b ENDP
ALIGN 2
FUN_62606a PROC

            push EBP
            mov EBP,ESP
            push ECX
            wait 
            fnstcw WORD PTR [EBP-4]
            mov EAX,DWORD PTR [EBP+12]
            mov ECX,EAX
            and EAX,DWORD PTR [EBP+8]
            not ECX
            and ECX,DWORD PTR [EBP-4]
            or ECX,EAX
            mov DWORD PTR [EBP+12],ECX
            fldcw WORD PTR [EBP+12]
            movsx EAX,WORD PTR [EBP-4]
            leave 
            ret 
FUN_62606a ENDP
FUN_62608d PROC

            push EBP
            mov EBP,ESP
            push ECX
            push ECX
            mov CL,BYTE PTR [EBP+8]
            test CL,1
            je $L_6260a4

            fld TBYTE PTR [(_KingData+118864)]
            fistp DWORD PTR [EBP+8]
            wait 
$L_6260a4::

            test CL,8
            je $L_6260b9

            wait 
            fnstsw AX
            fld TBYTE PTR [(_KingData+118864)]
            fstp QWORD PTR [EBP-8]
            wait 
            wait 
            fnstsw AX
$L_6260b9::

            test CL,16
            je $L_6260c8

            fld TBYTE PTR [(_KingData+118876)]
            fstp QWORD PTR [EBP-8]
            wait 
$L_6260c8::

            test CL,4
            je $L_6260d6

            fldz 
            fld1 
            fdivrp ST(1),ST(0)
            fstp ST(0)
            wait 
$L_6260d6::

            test CL,32
            je $L_6260e1

            fldpi 
            fstp QWORD PTR [EBP-8]
            wait 
$L_6260e1::

            leave 
            ret 
FUN_62608d ENDP
$L_6263ec PROC

            mov BYTE PTR [EBP-144],254
            fabs 
            fxch ST(1)
            fabs 
            fxch ST(1)
$L_6263fb::

            fpatan 
            or CL,CL
            je $L_626405

            fldpi 
            fsubrp ST(1),ST(0)
$L_626405::

            or CH,CH
            je $L_62640b

            fchs 
$L_62640b::

            ret 
$L_6263ec ENDP
ALIGN 4
FUN_62643c PROC

            fstp ST(0)
            fld TBYTE PTR [(_KingData+117578)]
            ret 
FUN_62643c ENDP
$L_626445 PROC

            fstp ST(0)
            or CL,CL
            je $L_626456

            fstp ST(0)
            fldpi 
            or CH,CH
            je $L_626455

            fchs 
$L_626455::

            ret 
$L_626456::

            jmp $L_624646
$L_626445 ENDP
$L_626462 PROC

            fstp ST(0)
            mov CL,CH
            jmp $L_626468
$L_626468::

            call FUN_62643c

            jmp $L_624703
$L_626462 ENDP
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
FUN_626f76 PROC

            push EAX
            push EBX
            push ECX
            mov EAX,DWORD PTR [ESP+22]
            xor EAX,1792
            test EAX,1792
            jne $L_627110

            shr EAX,11
            and EAX,15
            cmp BYTE PTR [EAX+(_KingData+119196)],0
            je $L_627110

            mov EAX,DWORD PTR [ESP+22]
            and EAX,2147418112
            cmp EAX,2147418112
            je $L_627110

            mov EAX,DWORD PTR [ESP+46]
            and EAX,2147418112
            je $L_627110

            cmp EAX,2147418112
            je $L_627110

            mov EAX,DWORD PTR [ESP+44]
            add EAX,EAX
            jne $L_627110

            mov EAX,DWORD PTR [ESP+20]
            add EAX,EAX
            jne $L_627110

            mov EAX,DWORD PTR [ESP+24]
            and EAX,32767
            add EAX,63
            mov EBX,DWORD PTR [ESP+48]
            and EBX,32767
            sub EBX,EAX
            ja $L_62705e
$L_627000::

            mov EAX,DWORD PTR [ESP+24]
            and EAX,32767
            add EAX,10
            mov EBX,DWORD PTR [ESP+48]
            and EBX,32767
            sub EBX,EAX
            js $L_627110

            fld TBYTE PTR [ESP+40]
            mov EAX,DWORD PTR [ESP+24]
            mov EBX,DWORD PTR [ESP+48]
            and EBX,32767
            mov ECX,EBX
            sub EBX,EAX
            and EBX,7
            or EBX,4
            sub ECX,EBX
            mov EBX,EAX
            and EBX,32768
            or ECX,EBX
            mov DWORD PTR [ESP+24],ECX
            fld TBYTE PTR [ESP+16]
            mov DWORD PTR [ESP+24],EAX
            fxch ST(1)
            fprem 
            fstp TBYTE PTR [ESP+40]
            fstp ST(0)
            jmp $L_627000
$L_62705e::

            test EDX,2
            jne $L_62706e

            fld TBYTE PTR [ESP+16]
            fstp TBYTE PTR [ESP+28]
$L_62706e::

            fnstcw WORD PTR [ESP+52]
            mov EAX,DWORD PTR [ESP+52]
            or EAX,831
            mov DWORD PTR [ESP+56],EAX
            fldcw WORD PTR [ESP+56]
            mov EAX,DWORD PTR [ESP+24]
            and EAX,32767
            mov EBX,DWORD PTR [ESP+48]
            and EBX,32767
            sub EBX,EAX
            and EBX,63
            or EBX,32
            add EBX,1
            mov ECX,EBX
            mov EAX,DWORD PTR [ESP+24]
            mov EBX,DWORD PTR [ESP+48]
            and EBX,32767
            and EAX,32768
            or EBX,EAX
            mov DWORD PTR [ESP+24],EBX
            fld TBYTE PTR [ESP+16]
            fabs 
            fld TBYTE PTR [ESP+40]
            fabs 
$L_6270c8::

            fcom ST(1)
            fnstsw AX
            and EAX,256
            jne $L_6270d5

            fsub ST(0),ST(1)
$L_6270d5::

            fxch ST(1)
            fmul QWORD PTR [(_KingData+119244)]
            fxch ST(1)
            sub ECX,1
            jne $L_6270c8

            mov EBX,DWORD PTR [ESP+48]
            fstp TBYTE PTR [ESP+40]
            fstp ST(0)
            fld TBYTE PTR [ESP+28]
            fld TBYTE PTR [(_KingData+119252)]
            fprem 
            fstp ST(0)
            fld TBYTE PTR [ESP+40]
            fldcw WORD PTR [ESP+52]
            and EBX,32768
            je $L_62711a

            fchs 
            jmp $L_62711a
$L_627110::

            fld TBYTE PTR [ESP+16]
            fld TBYTE PTR [ESP+40]
            fprem 
$L_62711a::

            test EDX,3
            je $L_627178

            fnstsw WORD PTR [ESP+60]
            test EDX,1
            je $L_62714d

            fnstcw WORD PTR [ESP+52]
            mov EAX,DWORD PTR [ESP+52]
            or EAX,768
            mov DWORD PTR [ESP+56],EAX
            fldcw WORD PTR [ESP+56]
            fmul QWORD PTR [(_KingData+119228)]
            fldcw WORD PTR [ESP+52]
$L_62714d::

            mov EAX,DWORD PTR [ESP+60]
            fxch ST(1)
            fstp ST(0)
            fld TBYTE PTR [ESP+28]
            fxch ST(1)
            and EAX,17152
            sub ESP,28
            fnstenv [ESP]
            and DWORD PTR [ESP+4],48383
            or DWORD PTR [ESP+4],EAX
            fldenv [ESP]
            add ESP,28
$L_627178::

            pop ECX
            pop EBX
            pop EAX
            ret 
FUN_626f76 ENDP
ALIGN 4
FUN_62717c PROC

            push EDX
            sub ESP,48
            fstp TBYTE PTR [ESP+24]
            fstp TBYTE PTR [ESP]
            xor EDX,EDX
            mov EAX,DWORD PTR [ESP+6]
            test EAX,2147418112
            je $L_62719e

            call FUN_626f76

            add ESP,48
            pop EDX
            ret 
$L_62719e::

            fld TBYTE PTR [ESP]
            fld TBYTE PTR [ESP+24]
            mov EAX,DWORD PTR [ESP]
            or EAX,DWORD PTR [ESP+4]
            je $L_627227

            fxch ST(1)
            fstp TBYTE PTR [ESP+12]
            fld TBYTE PTR [ESP]
            fxch ST(1)
            or EDX,2
            fnstcw WORD PTR [ESP+36]
            mov EAX,DWORD PTR [ESP+36]
            or EAX,831
            mov DWORD PTR [ESP+40],EAX
            fldcw WORD PTR [ESP+40]
            mov EAX,DWORD PTR [ESP+32]
            and EAX,32767
            cmp EAX,32702
            ja $L_6271f9

            or EDX,1
            fmul QWORD PTR [(_KingData+119220)]
            fstp TBYTE PTR [ESP+24]
            fmul QWORD PTR [(_KingData+119220)]
            fstp TBYTE PTR [ESP]
            jmp $L_627219
$L_6271f9::

            fnstcw WORD PTR [ESP+36]
            mov EAX,DWORD PTR [ESP+36]
            or EAX,768
            mov DWORD PTR [ESP+40],EAX
            fldcw WORD PTR [ESP+40]
            fstp ST(0)
            fmul QWORD PTR [(_KingData+119220)]
            fstp TBYTE PTR [ESP]
$L_627219::

            fldcw WORD PTR [ESP+36]
            call FUN_626f76

            add ESP,48
            pop EDX
            ret 
$L_627227::

            fprem 
            add ESP,48
            pop EDX
            ret 
FUN_62717c ENDP
ALIGN 2
FUN_627dd0 PROC

            push EBP
            mov EBP,ESP
            push EDI
            mov EDI,DWORD PTR [EBP+8]
            xor EAX,EAX
            or ECX,4294967295

            repne scasb 

            inc ECX
            neg ECX
            dec EDI
            mov AL,BYTE PTR [EBP+12]
            std 

            repne scasb 

            inc EDI
            cmp BYTE PTR [EDI],AL
            je $L_627df1

            xor EAX,EAX
            jmp $L_627df3
$L_627df1::

            mov EAX,EDI
$L_627df3::

            cld 
            pop EDI
            leave 
            ret 
FUN_627dd0 ENDP
FUN_628d7d PROC

            mov EAX,DWORD PTR [ESP+4]
            cmp EAX,DWORD PTR [(_KingBss+999520)]
            jae $L_628da8

            mov ECX,EAX
            and EAX,31
            sar ECX,5
            lea EAX,DWORD PTR [EAX+EAX*8]
            mov ECX,DWORD PTR [ECX*4+(_KingBss+999264)]
            test BYTE PTR [ECX+EAX*4+4],1
            lea EAX,DWORD PTR [ECX+EAX*4]
            je $L_628da8

            mov EAX,DWORD PTR [EAX]
            ret 
$L_628da8::

            call __errno

            mov DWORD PTR [EAX],9
            call ___doserrno

            and DWORD PTR [EAX],0
            or EAX,4294967295
            ret 
FUN_628d7d ENDP
FUN_628dbf PROC

            mov EAX,DWORD PTR [ESP+4]
            push EBX
            mov ECX,EAX
            and EAX,31
            sar ECX,5
            push ESI
            push EDI
            mov ESI,DWORD PTR [ECX*4+(_KingBss+999264)]
            lea EBX,DWORD PTR [ECX*4+(_KingBss+999264)]
            lea EDI,DWORD PTR [EAX+EAX*8]
            shl EDI,2
            add ESI,EDI
            cmp DWORD PTR [ESI+8],0
            jne $L_628e0d

            push 17
            call FUN_6241d4

            cmp DWORD PTR [ESI+8],0
            pop ECX
            jne $L_628e05

            lea EAX,DWORD PTR [ESI+12]
            push EAX
            call DWORD PTR __imp__InitializeCriticalSection@4

            inc DWORD PTR [ESI+8]
$L_628e05::

            push 17
            call FUN_624235

            pop ECX
$L_628e0d::

            mov EAX,DWORD PTR [EBX]
            lea EAX,DWORD PTR [EAX+EDI*1+12]
            push EAX
            call DWORD PTR __imp__EnterCriticalSection@4

            pop EDI
            pop ESI
            pop EBX
            ret 
FUN_628dbf ENDP
ALIGN 2
FUN_628e1e PROC

            mov EAX,DWORD PTR [ESP+4]
            mov ECX,EAX
            and EAX,31
            sar ECX,5
            lea EAX,DWORD PTR [EAX+EAX*8]
            mov ECX,DWORD PTR [ECX*4+(_KingBss+999264)]
            lea EAX,DWORD PTR [ECX+EAX*4+12]
            push EAX
            call DWORD PTR __imp__LeaveCriticalSection@4

            ret 
FUN_628e1e ENDP
ALIGN 16
FUN_62937d PROC

            push ESI
            push 1
            pop ESI
            push DWORD PTR [ESP+8]
            call DWORD PTR __imp__IsBadCodePtr@4

            test EAX,EAX
            je $L_629391

            xor ESI,ESI
$L_629391::

            mov EAX,ESI
            pop ESI
            ret 
FUN_62937d ENDP
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
ALIGN 16
FUN_629420 PROC

            push EBP
            mov EBP,ESP
            sub ESP,88
            mov EAX,DWORD PTR [EBP+16]
            push EBX
            push ESI
            mov ESI,DWORD PTR [EBP+12]
            movzx ECX,WORD PTR [EAX]
            mov EAX,DWORD PTR [ESI]
            push EDI
            dec EAX
            mov DWORD PTR [EBP+16],ECX
            je $L_629465

            dec EAX
            je $L_629461

            dec EAX
            je $L_62945d

            dec EAX
            je $L_629459

            dec EAX
            je $L_629465

            dec EAX
            dec EAX
            je $L_629451

            dec EAX
            jne $L_6294bb

            push 16
            jmp $L_629467
$L_629451::

            mov DWORD PTR [ESI],1
            jmp $L_6294bb
$L_629459::

            push 18
            jmp $L_629467
$L_62945d::

            push 17
            jmp $L_629467
$L_629461::

            push 4
            jmp $L_629467
$L_629465::

            push 8
$L_629467::

            pop EBX
            lea EDI,DWORD PTR [ESI+24]
            push ECX
            push EDI
            push EBX
            call FUN_625b5e

            add ESP,12
            test EAX,EAX
            jne $L_6294bb

            mov EAX,DWORD PTR [EBP+8]
            cmp EAX,16
            je $L_629492

            cmp EAX,22
            je $L_629492

            cmp EAX,29
            je $L_629492

            and DWORD PTR [EBP-32],4294967294
            jmp $L_6294a4
$L_629492::

            mov ECX,DWORD PTR [EBP-32]
            fld QWORD PTR [ESI+16]
            and ECX,4294967267
            fstp QWORD PTR [EBP-48]
            or ECX,3
            mov DWORD PTR [EBP-32],ECX
$L_6294a4::

            lea ECX,DWORD PTR [ESI+8]
            push EDI
            push ECX
            push EAX
            lea EAX,DWORD PTR [EBP+16]
            push EBX
            push EAX
            lea EAX,DWORD PTR [EBP-88]
            push EAX
            call FUN_6258ab

            add ESP,24
$L_6294bb::

            push 65535
            push DWORD PTR [EBP+16]
            call FUN_62606a

            cmp DWORD PTR [ESI],8
            pop ECX
            pop ECX
            je $L_6294e3

            cmp DWORD PTR [(_KingBss+999232)],0
            jne $L_6294e3

            push ESI
            call _World_MathErr

            test EAX,EAX
            pop ECX
            jne $L_6294eb
$L_6294e3::

            push DWORD PTR [ESI]
            call FUN_625dfd

            pop ECX
$L_6294eb::

            pop EDI
            pop ESI
            pop EBX
            leave 
            ret 
FUN_629420 ENDP
FUN_62952e PROC

            push EBX
            xor EBX,EBX
            cmp DWORD PTR [(_KingBss+670900)],EBX
            push ESI
            push EDI
            jne $L_62957d

            push OFFSET (_KingRdata+41496)
            call DWORD PTR __imp__LoadLibraryA@4

            mov EDI,EAX
            cmp EDI,EBX
            je $L_6295b3

            mov ESI,DWORD PTR __imp__GetProcAddress@8
            push OFFSET (_KingRdata+41484)
            push EDI
            call ESI

            test EAX,EAX
            mov DWORD PTR [(_KingBss+670900)],EAX
            je $L_6295b3

            push OFFSET (_KingRdata+41468)
            push EDI
            call ESI

            push OFFSET (_KingRdata+41448)
            push EDI
            mov DWORD PTR [(_KingBss+670904)],EAX
            call ESI

            mov DWORD PTR [(_KingBss+670908)],EAX
$L_62957d::

            mov EAX,DWORD PTR [(_KingBss+670904)]
            test EAX,EAX
            je $L_62959c

            call EAX

            mov EBX,EAX
            test EBX,EBX
            je $L_62959c

            mov EAX,DWORD PTR [(_KingBss+670908)]
            test EAX,EAX
            je $L_62959c

            push EBX
            call EAX

            mov EBX,EAX
$L_62959c::

            push DWORD PTR [ESP+24]
            push DWORD PTR [ESP+24]
            push DWORD PTR [ESP+24]
            push EBX
            call DWORD PTR [(_KingBss+670900)]
$L_6295af::

            pop EDI
            pop ESI
            pop EBX
            ret 
$L_6295b3::

            xor EAX,EAX
            jmp $L_6295af
FUN_62952e ENDP
ALIGN 8
FUN_6317e8 PROC

            call _Afx_GetModuleState@0

            mov ECX,DWORD PTR [ESP+4]
            test ECX,ECX
            mov BYTE PTR [EAX+20],CL
            jne $L_631800

            push -3
            call __setmbcp

            pop ECX
$L_631800::

            push 1
            pop EAX
            ret 8
FUN_6317e8 ENDP
$L_631806 PROC

            push 1
            push 0
            call _Afx_TermLocalData@8

            call _Afx_CriticalTerm

            jmp _Afx_TlsRelease
$L_631806 ENDP
$L_631819 PROC

            jmp $L_63181e
$L_63181e::

            push 1536
            push 0
            call FUN_6317e8

            push OFFSET $L_631806
            call _atexit

            pop ECX
            mov BYTE PTR [(_KingBss+998428)],AL
            ret 
$L_631819 ENDP
FUN_63183b PROC

            push ESI
            push EDI
            push 2
            pop EDI
            push EDI
            call FUN_628dbf

            mov ESI,DWORD PTR [ESP+16]
            pop ECX
            test ESI,ESI
            je $L_631871

            cmp BYTE PTR [ESI],0
            je $L_631871

            push ESI
            call _strlen

            push EAX
            push ESI
            push EDI
            call FUN_6254d5

            push EDI
            push OFFSET (_KingRdata+42524)
            push EDI
            call FUN_6254d5

            add ESP,28
$L_631871::

            call __errno

            cmp DWORD PTR [EAX],0
            jl $L_631893

            call __errno

            mov EAX,DWORD PTR [EAX]
            cmp EAX,DWORD PTR [(_KingData+119152)]
            jge $L_631893

            call __errno

            mov EAX,DWORD PTR [EAX]
            jmp $L_631898
$L_631893::

            mov EAX,DWORD PTR [(_KingData+119152)]
$L_631898::

            mov ESI,DWORD PTR [EAX*4+(_KingData+118976)]
            push ESI
            call _strlen

            push EAX
            push ESI
            push EDI
            call FUN_6254d5

            push 1
            push OFFSET (_KingData+1252)
            push EDI
            call FUN_6254d5

            push EDI
            call FUN_628e1e

            add ESP,32
            pop EDI
            pop ESI
            ret 
FUN_63183b ENDP
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
FUN_631d1d PROC

            push DWORD PTR [ESP+4]
            call DWORD PTR __imp__DeleteFileA@4

            test EAX,EAX
            jne $L_631d33

            call DWORD PTR __imp__GetLastError@0

            jmp $L_631d35
$L_631d33::

            xor EAX,EAX
$L_631d35::

            test EAX,EAX
            je $L_631d44

            push EAX
            call FUN_620d80

            pop ECX
            or EAX,4294967295
            ret 
$L_631d44::

            xor EAX,EAX
            ret 
FUN_631d1d ENDP
FUN_631d47 PROC

            push DWORD PTR [ESP+4]
            call FUN_631d1d

            pop ECX
            ret 
FUN_631d47 ENDP
ALIGN 2
FUN_6321e9 PROC

            push EBP
            mov EBP,ESP
            push ECX
            lea EAX,DWORD PTR [EBP-4]
            push OFFSET (_KingRdata+123656)
            push EAX
            mov DWORD PTR [EBP-4],OFFSET (_KingBss+669064)
            call __CxxThrowException@8
FUN_6321e9 ENDP
ALIGN 2
ALIGN 2
ALIGN 2
ALIGN 4
ALIGN 8
ALIGN 8
ALIGN 2
ALIGN 2
ALIGN 2
ALIGN 16
ALIGN 2
??2@YAPAXI@Z PROC ;FUN_632643 void *operator new(unsigned int size)

            push ESI
            push EDI
            mov EDI,DWORD PTR [ESP+12]
$L_632649::

            push EDI
            call _malloc

            mov ESI,EAX
            pop ECX
            test ESI,ESI
            jne $L_632667

            mov EAX,DWORD PTR [(_KingData+106032)]
            test EAX,EAX
            je $L_632667

            push EDI
            call EAX

            test EAX,EAX
            pop ECX
            jne $L_632649
$L_632667::

            mov EAX,ESI
            pop EDI
            pop ESI
            ret 
??2@YAPAXI@Z ENDP
ALIGN 4
??3@YAXPAX@Z PROC ;FUN_63266c void operator delete(void *p)

            push DWORD PTR [ESP+4]
            call _free

            pop ECX
            ret 
??3@YAXPAX@Z ENDP
; main loop: src\engine\main_loop.cpp, nmake ASM_MAIN_LOOP=1 assembles the original
$L_632864 PROC

            mov EAX,OFFSET $L_64a304
            call FUN_62001c

            sub ESP,72
            push EBX
            push ESI
            push EDI
            mov EDI,DWORD PTR [EBP+8]
            lea ECX,DWORD PTR [EBP-80]
            mov DWORD PTR [EBP-16],ESP
            mov ESI,DWORD PTR [EDI+4]
            mov DWORD PTR [EBP-20],ESI
            call @Wnd_Construct@4

            xor EBX,EBX
            mov DWORD PTR [EBP-4],EBX
            mov BYTE PTR [EBP-4],1
            call _Afx_GetThreadState

            mov ECX,DWORD PTR [EDI]
            mov ECX,DWORD PTR [ECX+4]
            mov DWORD PTR [EAX+4],ECX
            call _Afx_GetModuleState@0

            push OFFSET _ModuleThreadState_New
            lea ECX,DWORD PTR [EAX+4208]
            call @ThreadLocal_GetData@12

            mov DWORD PTR [EAX+4],ESI
            call _Afx_InitThread

            call _Afx_GetModuleState@0

            mov EAX,DWORD PTR [EAX+4]
            cmp EAX,EBX
            je $L_63291b

            cmp DWORD PTR [ESI+28],EBX
            jne $L_63291b

            mov EAX,DWORD PTR [EAX+28]
            cmp EAX,EBX
            je $L_63291b

            cmp DWORD PTR [EAX+28],EBX
            je $L_63291b

            push DWORD PTR [EAX+28]
            lea ECX,DWORD PTR [EBP-80]
            call @Wnd_Attach@12

            lea EAX,DWORD PTR [EBP-80]
            mov DWORD PTR [ESI+28],EAX
            jmp $L_63291b
$L_632864 ENDP
$L_6328eb PROC

            lea ECX,DWORD PTR [EBP-80]
            call @Wnd_Detach@4

            mov EAX,DWORD PTR [EBP+8]
            push DWORD PTR [EAX+16]
            mov DWORD PTR [EAX+24],1
            call DWORD PTR __imp__SetEvent@4

            push 0
            push -1
            call _Afx_EndThread@8

            mov EAX,OFFSET $L_632915
            ret 
$L_6328eb ENDP
$L_632915 PROC

            mov EDI,DWORD PTR [EBP+8]
            mov ESI,DWORD PTR [EBP-20]
$L_63291b::

            push DWORD PTR [EDI+16]
            and DWORD PTR [EBP-4],0
            mov EBX,DWORD PTR [EDI+20]
            call DWORD PTR __imp__SetEvent@4

            push -1
            push EBX
            call DWORD PTR __imp__WaitForSingleObject@8

            push EBX
            call DWORD PTR __imp__CloseHandle@4

            mov EAX,DWORD PTR [ESI+80]
            test EAX,EAX
            je $L_63294a

            push DWORD PTR [ESI+76]
            call EAX

            pop ECX
            jmp $L_632961
$L_63294a::

            mov EAX,DWORD PTR [ESI]
            mov ECX,ESI
            call DWORD PTR [EAX+80]

            test EAX,EAX
            mov EAX,DWORD PTR [ESI]
            mov ECX,ESI
            jne $L_63295e

            call DWORD PTR [EAX+104]

            jmp $L_632961
$L_63295e::

            call DWORD PTR [EAX+84]
$L_632961::

            lea ECX,DWORD PTR [EBP-80]
            mov ESI,EAX
            call @Wnd_Detach@4

            push 1
            push ESI
            call _Afx_EndThread@8

            or DWORD PTR [EBP-4],4294967295
            lea ECX,DWORD PTR [EBP-80]
            call @Wnd_Destruct@4

            mov ECX,DWORD PTR [EBP-12]
            pop EDI
            pop ESI
            xor EAX,EAX
            mov DWORD PTR FS:[0],ECX
            pop EBX
            leave 
            ret 4
$L_632915 ENDP
ALIGN 2
ALIGN 8
ALIGN 4
FUN_6371c5 PROC

            push EBP
            mov EBP,ESP
            push ECX
            lea EAX,DWORD PTR [EBP-4]
            push OFFSET (_KingRdata+123504)
            push EAX
            mov DWORD PTR [EBP-4],OFFSET (_KingBss+668672)
            call __CxxThrowException@8
FUN_6371c5 ENDP
ALIGN 2
ALIGN 8
ALIGN 2
FUN_63b4a6 PROC

            mov EAX,OFFSET $L_64a2a0
            call FUN_62001c

            push ECX
            mov DWORD PTR [EBP-16],ECX
            mov DWORD PTR [ECX],OFFSET (_KingRdata+33952)
            and DWORD PTR [EBP-4],0
            add ECX,4
            push ECX
            call _Afx_DeleteObject@4

            mov ECX,DWORD PTR [EBP-12]
            mov DWORD PTR FS:[0],ECX
            leave 
            ret 
FUN_63b4a6 ENDP
ALIGN 2
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
          BYTE 0ccH
_TEXT ENDS
END
