// crt.cpp - king.exe on the modern C runtime (static UCRT/vcruntime).
//
// The original links the 1999 C runtime statically; king.masm still contains
// it. Built without ASM_CRT, king.masm leaves out the old runtime's entry
// points that keep state (heap, stdio, exit, errno, threads, exception
// handling) and points their labels at the modern runtime or at the shims
// below. Its pure helpers (x87 math, _ftol, string functions) stay in asm.
// nmake ASM_CRT=1 builds the old runtime again (this file is left out).
//
// Start: the modern WinMainCRTStartup calls WinMain below, which does what
// the old _cinit did for the program (its C++ initializer table) and then
// runs MFC's AfxWinMain as the old _WinMain@16 did.

#include "king.h"
#include <stdarg.h>
#include <float.h>

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
