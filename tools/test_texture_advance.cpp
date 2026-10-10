// Compare the actual C++ routine with its preserved MASM implementation.
#include "../src/king.h"
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
extern "C" int _fltused = 0;
extern "C" void* __cdecl memset(void*, int, size_t);
#pragma function(memset)
extern "C" void* __cdecl memset(void* dst, int value, size_t size)
{
    volatile BYTE* bytes = (BYTE*)dst;
    for (size_t i = 0; i < size; ++i) bytes[i] = (BYTE)value;
    return dst;
}
extern "C" void __fastcall SceneTexture_Advance(BYTE*, void*);
extern "C" void __fastcall Reference_Advance(BYTE*, void*);
extern "C" void __fastcall Pooled_Set4(BYTE* self, void*) { F(int, self, 4) = 1; }
static BYTE actual[132], expected[132], resource[40], timer[2968];
static DWORD frames[9] = { 11, 22, 33, 44, 55, 66, 77, 88, 99 };
extern "C" void TestEntry()
{
    // Include frame boundaries, fractional/negative offsets, clamp/wrap,
    // expiration, unordered expiration, and a wrapping update counter.
    const float times[] = { 0, 0.09999999f, 0.1f, 0.89999998f, 1.0f, -0.01f };
    for (unsigned mode = 0; mode < 2; ++mode)
        for (int offset = 1; offset < 4; ++offset)
            for (unsigned t = 0; t < 6; ++t)
                for (unsigned expiration = 0; expiration < 3; ++expiration) {
                    for (unsigned i = 0; i < sizeof(actual); ++i)
                        actual[i] = expected[i] = 0xa5;
                    F(BYTE*, actual, 72) = resource;
                    F(BYTE*, actual, 88) = timer;
                    F(float, actual, 96) = 0.1f;
                    F(float, actual, 100) = expiration == 0 ? 0.5f : 2.0f;
                    if (expiration == 2) F(DWORD, actual, 100) = 0x7fc00000;
                    F(float, actual, 104) = times[t];
                    F(int, actual, 112) = 9;
                    F(int, actual, 116) = offset;
                    F(DWORD*, actual, 120) = frames;
                    F(DWORD, actual, 124) = 0xffffffff;
                    F(BYTE, actual, 128) = (BYTE)mode;
                    F(double, timer, 2960) = 0.016666666666666666;
                    for (unsigned i = 0; i < sizeof(actual); ++i)
                        expected[i] = actual[i];
                    SceneTexture_Advance(actual, 0);
                    DWORD texture = F(DWORD, resource, 36);
                    Reference_Advance(expected, 0);
                    if (texture != F(DWORD, resource, 36)) ExitProcess(1);
                    for (unsigned i = 0; i < sizeof(actual); ++i)
                        if (actual[i] != expected[i]) ExitProcess(2);
                }
    ExitProcess(0);
}
