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
extern "C" void __fastcall Effect_AnimateQuad(BYTE*, void*);
extern "C" void __fastcall Effect_MoveQuad(BYTE*, void*);
extern "C" void __fastcall Reference_AnimateQuad(BYTE*, void*);
extern "C" void __fastcall Reference_MoveQuad(BYTE*, void*);
extern "C" void __fastcall SceneTexture_Advance(BYTE* self, void*) { ++F(DWORD, self, 124); }
static BYTE actual[200], expected[200], resource[40], timer[2968];
static float x[4], y[4], savedX[4], savedY[4];
extern "C" void TestEntry()
{
    const float elapsed[] = { 0, -1, 0.33333334f, 1, 2, 1e-12f };
    const float lifetime[] = { 0, 1, 7.77f };
    for (unsigned move = 0; move < 2; ++move)
        for (unsigned t = 0; t < 6; ++t)
            for (unsigned l = 0; l < 3; ++l)
                for (unsigned scale = 0; scale < 3; ++scale) {
                    for (unsigned i = 0; i < sizeof(actual); ++i) actual[i] = 0xa5;
                    F(BYTE*, actual, 72) = resource;
                    F(float*, resource, 16) = x;
                    F(float*, resource, 20) = y;
                    F(float, actual, 76) = 12.345f;
                    F(float, actual, 80) = -98.765f;
                    F(float, actual, 84) = 1e-7f;
                    F(float, actual, 100) = lifetime[l];
                    F(float, actual, 104) = elapsed[t];
                    float multiplier = scale == 0 ? 1.0f : scale == 1 ? -0.00001f : 1e20f;
                    F(float, actual, 132) = 7.111f * multiplier;
                    F(float, actual, 136) = 0.313f * multiplier;
                    F(float, actual, 140) = -9.731f * multiplier;
                    F(float, actual, 144) = 2.111f * multiplier;
                    F(float, actual, 148) = 11.113f * multiplier;
                    F(float, actual, 152) = -7.733f * multiplier;
                    F(float, actual, 156) = 0.923f * multiplier;
                    F(BYTE*, actual, 160) = timer;
                    F(double, timer, 2960) = 0.016666666666666666;
                    for (unsigned i = 0; i < sizeof(actual); ++i) expected[i] = actual[i];
                    if (move) Effect_MoveQuad(actual, 0); else Effect_AnimateQuad(actual, 0);
                    for (unsigned i = 0; i < 4; ++i) { savedX[i] = x[i]; savedY[i] = y[i]; }
                    if (move) Reference_MoveQuad(expected, 0); else Reference_AnimateQuad(expected, 0);
                    for (unsigned i = 0; i < 4; ++i)
                        if (((DWORD*)savedX)[i] != ((DWORD*)x)[i] ||
                            ((DWORD*)savedY)[i] != ((DWORD*)y)[i]) ExitProcess(1);
                    for (unsigned i = 0; i < sizeof(actual); ++i)
                        if (actual[i] != expected[i]) ExitProcess(2);
                }
    ExitProcess(0);
}
