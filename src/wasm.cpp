// wasm.cpp - wasm.cpp of the game, rewritten from king.masm: small
// functions of its objects: the list of fading objects, points of the
// vehicle types, the constructors of an effect class (FUN_59ab10) and of its
// subclasses, a plane through a point, and lookups in a table of 124-byte
// entries.
//
// Building with  nmake ASM_WASM=1  makes king.masm assemble the original
// procedures again.

#include "car.h"
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl rand();

ASM_PROC(FUN_5d3440)
ASM_PROC(FUN_5d33d0)
ASM_PROC(FUN_5d3430)
ASM_PROC(FUN_4e48e2)
ASM_PROC(FUN_598bd0)
ASM_PROC(FUN_59a780)    // constructor of the effects' base class
ASM_PROC(FUN_59e2f0)    // constructor of the base class of FUN_59e9a0
ASM_PROC(FUN_5cda50)    // sets a plane (a, b, c, d)
extern "C" BYTE* __fastcall Pool_Construct(BYTE* self, void*, int size);    // mmt.cpp

ASM_VAR(BYTE*, g_faders, $L_6f66b0)             // the first fading object
ASM_VAR(int, g_67e178, $L_67e178)
ASM_VAR(int, g_6f6a68, $L_6f6a68)
ASM_VAR(int, g_6f6a70, $L_6f6a70)
ASM_VAR(float, g_rand1, $L_64c254)              // 1 / 32767: rand() to 0..1
ASM_VAR(AsmLabel, g_vtbl650c24, $L_650c24)
ASM_VAR(AsmLabel, g_vtbl650bd8, $L_650bd8)
ASM_VAR(AsmLabel, g_vtbl650d2c, $L_650d2c)
ASM_VAR(AsmLabel, g_vtbl650ce0, $L_650ce0)
ASM_VAR(AsmLabel, g_vtbl651078, $L_651078)
ASM_VAR(AsmLabel, g_vtbl65102c, $L_65102c)
ASM_VAR(AsmLabel, g_vtbl651254, $L_651254)
ASM_VAR(AsmLabel, g_vtbl651258, $L_651258)
ASM_VAR(AsmLabel, g_vtbl65125c, $L_65125c)
ASM_VAR(AsmLabel, g_vtbl651260, $L_651260)
ASM_VAR(AsmLabel, g_vtbl651264, $L_651264)
ASM_VAR(AsmLabel, g_vtbl651268, $L_651268)
ASM_VAR(AsmLabel, g_vtbl65126c, $L_65126c)
ASM_VAR(AsmLabel, g_vtbl651270, $L_651270)
ASM_VAR(AsmLabel, g_vtbl651274, $L_651274)
ASM_VAR(AsmLabel, g_vtbl651278, $L_651278)
ASM_VAR(AsmLabel, g_vtbl65127c, $L_65127c)
ASM_VAR(AsmLabel, g_vtbl651280, $L_651280)
ASM_VAR(AsmLabel, g_vtbl651284, $L_651284)
ASM_VAR(AsmLabel, g_vtbl651288, $L_651288)
ASM_VAR(AsmLabel, g_vtbl65128c, $L_65128c)
ASM_VAR(AsmLabel, g_vtbl651290, $L_651290)
ASM_VAR(AsmLabel, g_vtbl651294, $L_651294)

struct Vec3Bits { DWORD x, y, z; };     // a Vec3 copied or passed by value

// ---------------------------------------------------------------------------
// Fading objects: +0 the next, +4 the previous, +8 the fading time, +24 the
// object (+96 its opacity), +28 the start time

extern "C" void __fastcall Fader_Link(BYTE* self, void*, BYTE* next)
{
    if (!next) {
        g_faders = self;
        return;
    }
    F(BYTE*, self, 0) = next;
    F(BYTE*, self, 4) = F(BYTE*, next, 4);
    BYTE* prev = F(BYTE*, next, 4);
    if (!prev) {
        g_faders = self;
        F(BYTE*, next, 4) = self;
        return;
    }
    F(BYTE*, prev, 0) = self;
    F(BYTE*, next, 4) = self;
}

extern "C" void __fastcall Fader_Unlink(BYTE* self, void*)
{
    if (self == g_faders)
        g_faders = F(BYTE*, self, 0);
    if (F(BYTE*, self, 0))
        F(BYTE*, F(BYTE*, self, 0), 4) = F(BYTE*, self, 4);
    if (F(BYTE*, self, 4))
        F(BYTE*, F(BYTE*, self, 4), 0) = F(BYTE*, self, 0);
    F(BYTE*, self, 4) = 0;
    F(BYTE*, self, 0) = 0;
}

extern "C" void __fastcall Fader_Release(BYTE* self, void*)
{
    if (!F(void*, self, 24))
        return;
    CallVirt(F(void*, self, 24), 36);
    if (F(void*, self, 24))
        CallVirt(F(void*, self, 24), 0, 1);     // scalar deleting destructor
    F(void*, self, 24) = 0;
}

// the opacity at time t; 0 when the fade is over
extern "C" int __fastcall Fader_Step(BYTE* self, void*, float t)
{
    double x = ((double)t - F(float, self, 28)) / F(float, self, 8);
    if (x > 1.0f)
        return 0;
    F(float, F(BYTE*, self, 24), 96) = (float)(1.0f - x);
    return 1;
}

extern "C" void __cdecl Faders_DeleteAll()
{
    BYTE* p = g_faders;
    if (!p) {
        g_faders = 0;
        return;
    }
    BYTE* next;
    do {
        next = F(BYTE*, p, 0);
        if (p) {
            Fader_Release(p, 0);
            operator delete(p);
        }
        p = next;
    } while (next);
    g_faders = next;
}

// a point of the vehicle's type: -1 the one at +1748, else point i of those
// at +1684
extern "C" Vec3Bits* __cdecl CarType_Point(Vec3Bits* out, const BYTE* car, int i)
{
    const BYTE* t = (const BYTE*)&g_carTypes[F(int, car, 10420)];
    const Vec3Bits* p = i == -1 ? (const Vec3Bits*)(t + 1748) : (const Vec3Bits*)(t + 1684 + i * 12);
    *out = *p;
    return out;
}

// ---------------------------------------------------------------------------

extern "C" void __fastcall Wasm_595260(BYTE* self, void*)
{
    g_67e178 = 0;
    CallVirt(F(void*, self, 112), 60);
    CallVirt(F(void*, self, 112), 48);
    g_67e178 = 1;
}

extern "C" void __fastcall Wasm_595c70(BYTE* self, void*)
{
    F(int, self, 320) = 0;
    F(int, self, 232) = 0;
    if (!CallThis<int>(FUN_5d3440, self))
        CallThis(FUN_5d33d0, self, 1);
}

extern "C" void __fastcall Wasm_Set92(BYTE* self, void*, int v)
{
    F(int, self, 92) = v;
}

extern "C" void __fastcall Wasm_598ce0(BYTE* self, void*, int mode)
{
    int state = F(int, self, 104);
    if (!state)
        return;
    if (mode == 2) {
        F(int, self, 104) = 0;
        return;
    }
    if (state == 2)
        return;
    void* o = F(void*, self, 108);
    if (!o)
        return;
    F(int, self, 104) = 0;
    CallVirt(o, 32);
    if (F(void*, self, 124))
        CallThis(FUN_5d3430, F(void*, self, 124));
}

extern "C" void __fastcall Wasm_598d30(BYTE* self, void*, int a, int b)
{
    int state = F(int, self, 104);
    if (!state || state == 2)
        return;
    CallC(FUN_4e48e2, F(int, self, 20), 0, b);
    F(int, F(BYTE*, self, 120), 76) = a;
}

extern "C" void __fastcall Wasm_598d60(BYTE* self, void*)
{
    if (F(int, self, 104) == 2)
        return;
    F(DWORD, F(BYTE*, self, 112), 16) |= 3;
    CallThis(FUN_598bd0, self, 1);
}

extern "C" void __fastcall Wasm_598d80(BYTE* self, void*)
{
    if (F(int, self, 104) == 2)
        return;
    Wasm_598ce0(self, 0, 1);
    CallVirt(F(void*, self, 112), 32);
}

extern "C" int __fastcall Wasm_Get108(BYTE* self, void*)
{
    return F(int, self, 108);
}

// ---------------------------------------------------------------------------
// Effects

extern "C" BYTE* __fastcall Effect_Construct(BYTE* self, void*, int a, const Vec3Bits* pos, int b, int c,
                                             DWORD d, DWORD e, DWORD f, DWORD g, int unused)
{
    CallThis(FUN_59a780, self, a, g_6f6a68, *pos, b, b, d, e, f, g);
    F(int, self, 140) = b;
    F(int, self, 144) = c;
    F(void*, self, 0) = g_vtbl650c24;
    F(void*, self, 20) = g_vtbl650bd8;
    F(int, self, 132) = b;
    F(int, self, 136) = c;
    return self;
}

extern "C" BYTE* __fastcall Effect_Construct2(BYTE* self, void*, int a, const Vec3Bits* pos, int b, int c, int d,
                                              int e, DWORD f, DWORD g, DWORD h, DWORD i, int unused)
{
    CallThis(FUN_59a780, self, a, g_6f6a68, *pos, b, d, f, g, h, i);
    F(int, self, 132) = b;
    F(void*, self, 0) = g_vtbl650c24;
    F(int, self, 144) = e;
    F(void*, self, 20) = g_vtbl650bd8;
    F(int, self, 136) = c;
    F(int, self, 140) = d;
    return self;
}

extern "C" BYTE* __fastcall Effect59bb90_Construct(BYTE* self, void*, int a, const Vec3Bits* p, const Vec3Bits* q,
                                                   int b, int c, DWORD d, DWORD e)
{
    Effect_Construct(self, 0, a, p, b, c, d, e, 0x3d4ccccd, 0x3f400000, 100);  // 0.05f, 0.75f
    for (int i = 148; i <= 168; i += 4)
        F(int, self, i) = 0;
    F(void*, self, 0) = g_vtbl650d2c;
    F(DWORD, self, 128) |= 1;
    F(void*, self, 20) = g_vtbl650ce0;
    F(Vec3Bits, self, 148) = *p;
    F(Vec3Bits, self, 160) = *q;
    F(int, self, 172) = 0;
    return self;
}

extern "C" void __cdecl Wasm_Set6f6a70(int v)
{
    g_6f6a70 = v;
}

extern "C" BYTE* __fastcall Effect59e9a0_Construct(BYTE* self, void*, int a, int b, int c, int d, int e, int f,
                                                   int g)
{
    CallThis(FUN_59e2f0, self, a, b, c, d, e, f);
    F(void*, self, 0) = g_vtbl651078;
    F(void*, self, 20) = g_vtbl65102c;
    F(int, self, 208) = g;
    double r = rand() * (double)g_rand1;
    if (r < 0.33f)
        F(int, self, 204) = 252;
    else if (r < 0.66f)
        F(int, self, 204) = 253;
    else
        F(int, self, 204) = 254;
    return self;
}

extern "C" BYTE* __fastcall Wasm59f790_Construct(BYTE* self, void*)
{
    for (int i = 8; i < 104; i += 4)
        F(int, self, i) = 0;
    F(int, self, 104) = 0;
    F(int, self, 4) = 0;
    F(int, self, 0) = 0;
    F(int, self, 108) = 0;
    return self;
}

// sets obj's plane to the one with normal n through p
extern "C" void __cdecl Plane_ThroughPoint(const float* n, const float* p, void* obj)
{
    float plane[4];
    F(DWORD, plane, 0) = F(DWORD, n, 0);
    F(DWORD, plane, 4) = F(DWORD, n, 4);
    F(DWORD, plane, 8) = F(DWORD, n, 8);
    plane[3] = (float)-(((double)n[2] * p[2] + (double)n[1] * p[1]) + (double)n[0] * p[0]);
    CallThis(FUN_5cda50, obj, plane);
}

// |distance of p from the plane at +72| / (+116 - +112), clamped to 0..1
extern "C" double __fastcall Wasm59f810_Ratio(BYTE* self, void*, const float* p)
{
    double d = ((double)F(float, self, 80) * p[2] + (double)F(float, self, 76) * p[1])
               + (double)F(float, self, 72) * p[0];
    double x = x87_fabs(d + F(float, self, 84)) / ((double)F(float, self, 116) - F(float, self, 112));
    if (x > 1.0f)
        return 1.0f;
    if (!(x >= 0.0f))
        return 0.0f;
    return x;
}

// entries of 124 bytes from +32, their count at +6432
extern "C" BYTE* __fastcall Table124_Find(BYTE* self, void*, int a, int b)
{
    for (unsigned i = 0; i < F(unsigned, self, 6432); i++) {
        BYTE* e = self + 32 + i * 124;
        if (F(int, e, 0) == a && F(int, e, 4) == b && F(int, e, 108))
            return e;
    }
    return 0;
}

extern "C" int __fastcall Table124_Get104(BYTE* self, void*, int a, int b)
{
    for (unsigned i = 0; i < F(unsigned, self, 6432); i++) {
        BYTE* e = self + 32 + i * 124;
        if (F(int, e, 0) == a && F(int, e, 4) == b)
            return F(int, e, 104);
    }
    return 0;
}

// the subclasses of FUN_579400's class (500 entries) that only set their
// vtable
static BYTE* With500(BYTE* self, void* vtbl)
{
    Pool_Construct(self, 0, 500);
    F(void*, self, 0) = vtbl;
    return self;
}
extern "C" BYTE* __fastcall WasmObj5a1480_Construct(BYTE* self, void*) { return With500(self, g_vtbl651254); }
extern "C" BYTE* __fastcall WasmObj5a1650_Construct(BYTE* self, void*) { return With500(self, g_vtbl651258); }
extern "C" BYTE* __fastcall WasmObj5a1880_Construct(BYTE* self, void*) { return With500(self, g_vtbl65125c); }
extern "C" BYTE* __fastcall WasmObj5a1a60_Construct(BYTE* self, void*) { return With500(self, g_vtbl651260); }
extern "C" BYTE* __fastcall WasmObj5a1ca0_Construct(BYTE* self, void*) { return With500(self, g_vtbl651264); }
extern "C" BYTE* __fastcall WasmObj5a1e80_Construct(BYTE* self, void*) { return With500(self, g_vtbl651268); }
extern "C" BYTE* __fastcall WasmObj5a20c0_Construct(BYTE* self, void*) { return With500(self, g_vtbl65126c); }
extern "C" BYTE* __fastcall WasmObj5a22b0_Construct(BYTE* self, void*) { return With500(self, g_vtbl651270); }
extern "C" BYTE* __fastcall WasmObj5a2490_Construct(BYTE* self, void*) { return With500(self, g_vtbl651274); }
extern "C" BYTE* __fastcall WasmObj5a2670_Construct(BYTE* self, void*) { return With500(self, g_vtbl651278); }
extern "C" BYTE* __fastcall WasmObj5a2850_Construct(BYTE* self, void*) { return With500(self, g_vtbl65127c); }
extern "C" BYTE* __fastcall WasmObj5a2ab0_Construct(BYTE* self, void*) { return With500(self, g_vtbl651280); }
extern "C" BYTE* __fastcall WasmObj5a2d30_Construct(BYTE* self, void*) { return With500(self, g_vtbl651284); }
extern "C" BYTE* __fastcall WasmObj5a2f00_Construct(BYTE* self, void*) { return With500(self, g_vtbl651288); }
extern "C" BYTE* __fastcall WasmObj5a3140_Construct(BYTE* self, void*) { return With500(self, g_vtbl65128c); }
extern "C" BYTE* __fastcall WasmObj5a3380_Construct(BYTE* self, void*) { return With500(self, g_vtbl651290); }
extern "C" BYTE* __fastcall WasmObj5a35b0_Construct(BYTE* self, void*) { return With500(self, g_vtbl651294); }
