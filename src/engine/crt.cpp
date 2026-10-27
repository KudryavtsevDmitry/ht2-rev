// crt.cpp - king.exe on the modern C runtime (static UCRT/vcruntime).
//
// The original links the 1999 C runtime statically. Its entry points that
// keep state (heap, stdio, exit, errno, threads, exception handling) are gone:
// their labels point at the modern runtime or at the shims below. Its pure
// helpers come from the modern runtime too, except where results differ:
// sqrt and cos with their _matherr path, operator new, exception (below).
//
// Start: the modern WinMainCRTStartup calls WinMain below, which does what
// the old _cinit did for the program (its C++ initializer table) and then
// runs MFC's AfxWinMain as the old _WinMain@16 did.

#include "king.h"
#include <stdarg.h>
#include <float.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "x87.h"

typedef char AsmLabel[1];

// The old runtime's C++ initializers: function pointers from $L_66c000 up
// to $L_66c488 (game statics, MFC statics), zeros skipped
KING_VAR(AsmLabel, g_xcBegin, 66c000)
KING_VAR(AsmLabel, g_xcEnd, 66c488)

extern "C" int __stdcall Afx_WinMain(HINSTANCE h, HINSTANCE prev, char* cmdLine, int show);

extern "C" int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdLine, int show) {
    _controlfp(_PC_53, _MCW_PC);                                   // as the old _fpmath
    typedef void (__cdecl* Init)();
    for (Init* f = (Init*)g_xcBegin; f < (Init*)g_xcEnd; f++)
        if (*f) (*f)();
    return Afx_WinMain(inst, prev, cmdLine, show);
}

// ---------------------------------------------------------------------------
// stdio for the remaining assembly: it can still pass the old runtime's
// stdin/stdout/stderr (_iob entries, 32 bytes each from $L_686340); those
// become the modern streams.

KING_VAR(BYTE, g_oldIob, 686340)

static FILE* Stream(FILE* f) {
    UINT_PTR i = (BYTE*)f - &g_oldIob;
    return i < 3 * 32 ? __acrt_iob_func((unsigned)(i / 32)) : f;
}

extern "C" {
int __cdecl vfprintf(FILE* f, const char* format, va_list args);
int __cdecl fputs(const char* s, FILE* f);
int __cdecl fputc(int c, FILE* f);
int __cdecl fgetc(FILE* f);
char* __cdecl fgets(char* s, int n, FILE* f);
size_t __cdecl fread(void* buf, size_t size, size_t n, FILE* f);
size_t __cdecl fwrite(const void* buf, size_t size, size_t n, FILE* f);
long __cdecl ftell(FILE* f);
int __cdecl fseek(FILE* f, long off, int origin);
int __cdecl fflush(FILE* f);
void __cdecl rewind(FILE* f);

int __cdecl OldCrt_fprintf(FILE* f, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int r = vfprintf(Stream(f), format, args);
    va_end(args);
    return r;
}
int __cdecl OldCrt_fclose(FILE* f) { return fclose(Stream(f)); }
int __cdecl OldCrt_fputs(const char* s, FILE* f) { return fputs(s, Stream(f)); }
int __cdecl OldCrt_fputc(int c, FILE* f) { return fputc(c, Stream(f)); }
int __cdecl OldCrt_fgetc(FILE* f) { return fgetc(Stream(f)); }
char* __cdecl OldCrt_fgets(char* s, int n, FILE* f) { return fgets(s, n, Stream(f)); }
size_t __cdecl OldCrt_fread(void* buf, size_t size, size_t n, FILE* f) { return fread(buf, size, n, Stream(f)); }
size_t __cdecl OldCrt_fwrite(const void* buf, size_t size, size_t n, FILE* f) { return fwrite(buf, size, n, Stream(f)); }
long __cdecl OldCrt_ftell(FILE* f) { return ftell(Stream(f)); }
int __cdecl OldCrt_fseek(FILE* f, long off, int origin) { return fseek(Stream(f), off, origin); }
int __cdecl OldCrt_fflush(FILE* f) { return fflush(f ? Stream(f) : f); }
void __cdecl OldCrt_rewind(FILE* f) { rewind(Stream(f)); }
}

// [UnusedTrap] procedures the program never reaches were pointed here
extern "C" void __cdecl UnusedTrap() { __debugbreak(); }
// [$L_61c4c0] an empty destructor (thiscall), jumped to by unwind funclets
extern "C" void __fastcall Eh_EmptyDtor(void*) {}

// ---------------------------------------------------------------------------
// The old runtime's x87 sqrt and cos [FUN_61f6e4, FUN_61f7a4]: the FPU
// instruction, with domain errors (sqrt of a negative, cos of an infinity)
// sent to the game's _matherr, which makes sqrt of a negative 0

extern "C" int __cdecl World_MathErr(int* e);

struct MathException { int type; const char* name; double arg1, arg2, retval; };

static double MathDomainError(const char* name, double x) {
    static const unsigned __int64 indefinite = 0xfff8000000000000ull;
    MathException e = { 1, name, x, 0.0, *(const double*)&indefinite };    // _DOMAIN
    if (!World_MathErr((int*)&e)) errno = EDOM;
    return e.retval;
}

static bool IsNan(double x) {
    unsigned __int64 b = *(unsigned __int64*)&x;
    return (b & 0x7ff0000000000000ull) == 0x7ff0000000000000ull && (b & 0x000fffffffffffffull);
}

#pragma function(sqrt, cos)
extern "C" double __cdecl sqrt(double x) {
    if (IsNan(x)) return x + x;                      // quieted
    if (x < 0.0) return MathDomainError("sqrt", x);  // not -0
    return x87_sqrt(x);
}

extern "C" double __cdecl cos(double x) {
    if (IsNan(x)) return x + x;
    if (!(x - x == 0.0)) return MathDomainError("cos", x);
    if (!(x87_fabs(x) < 9223372036854775808.0))      // fcos needs |x| < 2^63
        x = x87_fmod(x, 6.283185307179586476925);
    return x87_cos(x);
}

// [FUN_632643] operator new: malloc, retried while the new handler
// (_pnhHeap, MFC's AfxNewHandler) says so
typedef int (__cdecl* NewHandler)(size_t);
KING_VAR(NewHandler, g_newHandler, 685e30)

void* __cdecl operator new(size_t size) {
    for (;;) {
        void* p = malloc(size);
        if (p || !g_newHandler || !g_newHandler(size)) return p;
    }
}
// [FUN_63266c]
void __cdecl operator delete(void* p) { free(p); }
void __cdecl operator delete(void* p, size_t) { free(p); }

// ---------------------------------------------------------------------------
// The old runtime's exception class: vtable, what (+4), owns what (+8)

KING_VAR(BYTE, g_exceptionVtable, 654768)
KING_VAR(char, g_unknownException, 654770)          // "Unknown exception"

static char* CopyString(const char* s) {
    char* p = (char*)operator new(strlen(s) + 1);
    if (p) strcpy(p, s);
    return p;
}

extern "C" {
// [FUN_61e0cd] exception::exception(const char* const&)
BYTE* __fastcall Exc_Construct(BYTE* self, void*, const char* const* what) {
    F(void*, self, 0) = &g_exceptionVtable;
    F(char*, self, 4) = CopyString(*what);
    F(int, self, 8) = 1;
    return self;
}
// [FUN_61e10a] exception::exception(const exception&)
BYTE* __fastcall Exc_CopyConstruct(BYTE* self, void*, const BYTE* other) {
    F(void*, self, 0) = &g_exceptionVtable;
    F(int, self, 8) = F(int, other, 8);
    F(const char*, self, 4) = F(int, other, 8) ? CopyString(F(const char*, other, 4)) : F(const char*, other, 4);
    return self;
}
// [FUN_61e154] exception::~exception
void __fastcall Exc_Destroy(BYTE* self) {
    F(void*, self, 0) = &g_exceptionVtable;
    if (F(int, self, 8)) operator delete(F(void*, self, 4));
}
// [$L_61e0b1] its deleting destructor
BYTE* __fastcall Exc_DeletingDestructor(BYTE* self, void*, int flags) {
    Exc_Destroy(self);
    if (flags & 1) operator delete(self);
    return self;
}
// [$L_61e16a] exception::what
const char* __fastcall Exc_What(BYTE* self) {
    return F(const char*, self, 4) ? F(const char*, self, 4) : &g_unknownException;
}

// [FUN_61e177] type_info::~type_info: frees the undecorated name (+4)
// (the old runtime locked _TYPEINFO_LOCK around it)
KING_VAR(BYTE, g_typeInfoVtable, 654788)
void __fastcall TypeInfo_Destroy(BYTE* self) {
    F(void*, self, 0) = &g_typeInfoVtable;
    free(F(void*, self, 4));
}
// [$L_61e1a0]
BYTE* __fastcall TypeInfo_DeletingDestructor(BYTE* self, void*, int flags) {
    TypeInfo_Destroy(self);
    if (flags & 1) operator delete(self);
    return self;
}

// ---------------------------------------------------------------------------
// MFC pieces that were linked among the runtime


// [FUN_6321e9] AfxThrowMemoryException: throws the static CMemoryException
KING_VAR(BYTE, g_memoryException, 72dda8)
KING_VAR(BYTE, g_memoryExceptionThrow, 669308)
void __cdecl Afx_RaiseMemoryException() {
    void* e = &g_memoryException;
    _CxxThrowException(&e, (_ThrowInfo*)&g_memoryExceptionThrow);
}
// [FUN_6371c5] AfxThrowResourceException: throws the static CResourceException
KING_VAR(BYTE, g_resourceException, 72dc20)
KING_VAR(BYTE, g_resourceExceptionThrow, 669270)
void __cdecl Afx_ThrowResourceException() {
    void* e = &g_resourceException;
    _CxxThrowException(&e, (_ThrowInfo*)&g_resourceExceptionThrow);
}

// [FUN_63b4a6] a CGdiObject-like holder's destructor: its vtable, the GDI
// object (+4) deleted
KING_VAR(BYTE, g_gdiHolderVtable, 6534a0)
void __stdcall Afx_DeleteObject(HGDIOBJ* p);
void __fastcall GdiHolder_Destruct(BYTE* self) {
    F(void*, self, 0) = &g_gdiHolderVtable;
    Afx_DeleteObject((HGDIOBJ*)(self + 4));
}

// [$L_632864] _AfxThreadEntry: the new thread's MFC state, then the thread
// function or CWinThread::InitInstance and Run, then AfxEndThread. Its catch
// (exception during start-up: error flag, AfxEndThread(-1)) is gone:
// exceptions are fatal in this program.
BYTE* __cdecl Afx_GetThreadState();
BYTE* __stdcall Afx_GetModuleState();
BYTE* __cdecl ModuleThreadState_New();
void* __fastcall ThreadLocal_GetData(int* self, void*, void* (__stdcall* create)());
void __cdecl Afx_InitThread();
void __stdcall Afx_EndThread(UINT exitCode, BOOL deleteThread);
BOOL __fastcall Wnd_Attach(BYTE* self, void*, HWND h);
HWND __fastcall Wnd_Detach(BYTE* self);
BYTE* __fastcall Wnd_Construct(BYTE* self);
void __fastcall Wnd_Destruct(BYTE* self);

UINT __stdcall Afx_ThreadEntry(BYTE* st) {
    BYTE* thread = F(BYTE*, st, 4);
    BYTE wnd[80];
    Wnd_Construct(wnd);
    F(void*, Afx_GetThreadState(), 4) = F(void*, F(BYTE*, st, 0), 4);
    BYTE* mts = (BYTE*)ThreadLocal_GetData((int*)(Afx_GetModuleState() + 4208), 0,
                                           (void* (__stdcall*)())ModuleThreadState_New);
    F(BYTE*, mts, 4) = thread;
    Afx_InitThread();
    BYTE* app = F(BYTE*, Afx_GetModuleState(), 4);
    if (app && !F(void*, thread, 28)) {
        BYTE* main = F(BYTE*, app, 28);
        if (main && F(HWND, main, 28)) {
            Wnd_Attach(wnd, 0, F(HWND, main, 28));
            F(BYTE*, thread, 28) = wnd;
        }
    }
    HANDLE event2 = F(HANDLE, st, 20);
    SetEvent(F(HANDLE, st, 16));
    WaitForSingleObject(event2, INFINITE);
    CloseHandle(event2);
    UINT code;
    typedef UINT (__cdecl* ThreadProc)(void*);
    typedef UINT (__fastcall* Method)(BYTE*);
    if (F(ThreadProc, thread, 80))
        code = F(ThreadProc, thread, 80)(F(void*, thread, 76));
    else if (!F(Method, F(BYTE*, thread, 0), 80)(thread))     // InitInstance
        code = F(Method, F(BYTE*, thread, 0), 104)(thread);    // ExitInstance
    else
        code = F(Method, F(BYTE*, thread, 0), 84)(thread);     // Run
    Wnd_Detach(wnd);
    Afx_EndThread(code, TRUE);
    Wnd_Destruct(wnd);
    return 0;
}
}

// [FUN_620cf2] __initmbctable, in the C++ initializer table: the multibyte
// code page of the system's ANSI code page, once
extern "C" int __cdecl _setmbcp(int codePage);
KING_VAR(int, g_mbcInitialized, 77eaf0)
extern "C" void __cdecl Crt_InitMbcTable() {
    if (!g_mbcInitialized) {
        _setmbcp(-3);
        g_mbcInitialized = 1;
    }
}

// [FUN_6317e8] AfxInitialize(bDLL, dwVersion): the module state's DLL flag;
// an exe uses the system's ANSI code page
extern "C" BYTE* __stdcall Afx_GetModuleState();
extern "C" BOOL __stdcall Afx_Initialize(BOOL dll, DWORD version) {
    F(BYTE, Afx_GetModuleState(), 20) = (BYTE)dll;
    if (!dll) _setmbcp(-3);
    return TRUE;
}

// [$L_631806] MFC's clean-up at exit: thread data, critical sections, TLS
extern "C" void __stdcall Afx_TermLocalData(HINSTANCE hInst, BOOL all);
extern "C" void __cdecl Afx_CriticalTerm();
extern "C" void __cdecl Afx_TlsRelease();
static void __cdecl Afx_Term() {
    Afx_TermLocalData(0, TRUE);
    Afx_CriticalTerm();
    Afx_TlsRelease();
}

// [$L_631819] in the C++ initializer table: AfxInitialize(FALSE, 0x600) and
// the clean-up at exit (atexit's result kept as a byte)
KING_VAR(char, g_afxTermRegistered, 77e43c)
extern "C" void __cdecl Afx_InitModule() {
    Afx_Initialize(FALSE, 0x600);
    g_afxTermRegistered = (char)atexit(Afx_Term);
}
