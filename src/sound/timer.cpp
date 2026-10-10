// timer.cpp - rdtsc.cpp of the Gi\Srmidp library, rewritten from king.masm:
// the floating point .ini helpers and the worker thread object. The
// processor clock calibration itself stays assembly.
//
// Building with  nmake ASM_TIMER=1  makes king.masm assemble the original
// procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);
extern "C" double __cdecl atof(const char* s);

ASM_FUNC(Thread_Main, $L_603df0)

ASM_VAR(AsmLabel, s_N, $L_6761e8)                // "N", the missing value
ASM_VAR(AsmLabel, s_g, $L_671350)                // "%g"
ASM_VAR(AsmLabel, g_vtbl652de8, $L_652de8)

static int Ini_Get(const char* section, const char* key, const char* file, char* buf)
{
    GetPrivateProfileStringA(section, key, s_N, buf, 96, file);
    return strlen(buf) && strncmp(buf, s_N, 1);
}

extern "C" {

// [FUN_603cb0]
int __cdecl Ini_GetDouble(const char* section, const char* key, double def, double* out, const char* file)
{
    char buf[96];
    if (Ini_Get(section, key, file, buf))
        *out = atof(buf);
    else
        *out = def;
    return 0;
}

// [FUN_603d30]
int __cdecl Ini_GetFloat(const char* section, const char* key, double def, float* out, const char* file)
{
    char buf[96];
    if (Ini_Get(section, key, file, buf))
        *out = (float)atof(buf);
    else
        *out = (float)def;
    return 0;
}

// [FUN_603db0]
BOOL __cdecl Ini_PutDouble(const char* section, const char* key, double v, const char* file)
{
    char buf[96];
    sprintf(buf, s_g, v);
    return WritePrivateProfileStringA(section, key, buf, file);
}

// The worker thread: vtbl, +8 flags (bit 0: stop), +12 handle, +16 id.

// [FUN_603e20]
BYTE* __fastcall Thread_Construct(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl652de8;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    return self;
}

// [FUN_603e60]
void __fastcall Thread_Start(BYTE* self, void*)
{
    F(HANDLE, self, 12) =
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Thread_Main, self, 0, (DWORD*)(self + 16));
}

// [FUN_603e80]
DWORD __fastcall Thread_Wait(BYTE* self, void*)
{
    return WaitForSingleObject(F(HANDLE, self, 12), INFINITE);
}

// [FUN_603e90]
void __fastcall Thread_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl652de8;
    F(DWORD, self, 8) |= 1;
    Thread_Wait(self, 0);
    CloseHandle(F(HANDLE, self, 12));
}

}
