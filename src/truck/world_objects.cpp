// world_objects.cpp - wasm.cpp of the game, rewritten from king.masm: small
// functions of its objects: the list of fading objects, points of the
// vehicle types, the constructors of an effect class (FUN_59ab10) and of its
// subclasses, a plane through a point, and lookups in a table of 124-byte
// entries.
//
// Building with  nmake ASM_WORLD_OBJECTS=1  makes king.masm assemble the original
// procedures again.

#include "car.h"
#include "vecmath.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl rand();

extern "C" void FUN_5d3440(); KING_ALIAS("_FUN_5d3440", "@Handle_Count@8")
ASM_PROC(FUN_5d33d0)
extern "C" void FUN_5d3430(); KING_ALIAS("_FUN_5d3430", "@Handle_Call3790@8")
ASM_PROC(FUN_4e48e2)
ASM_PROC(FUN_598bd0)
ASM_PROC(FUN_59a780)    // constructor of the effects' base class
extern "C" void FUN_59e2f0(); KING_ALIAS("_FUN_59e2f0", "@Debris2_Construct@32")    // constructor of the base class of FUN_59e9a0
extern "C" void FUN_5cda50(); KING_ALIAS("_FUN_5cda50", "@Shade_Assign@12")    // sets a plane (a, b, c, d)
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

// ---------------------------------------------------------------------------
// medium-sized functions of the unit

ASM_PROC(FUN_5d31e0)    // named object: constructor (id, name)
ASM_PROC(FUN_4e41d9)    // an angle checked for NaN (angle, where)
extern "C" void FUN_603cb0(); KING_ALIAS("_FUN_603cb0", "_Ini_GetDouble")    // a double of an ini file (section, key, default, out, file)
ASM_PROC(FUN_4e5dce)
ASM_PROC(FUN_4e5c77)
extern "C" Vec3* __fastcall Vec3_ZeroThis(Vec3* self, void*);
ASM_VAR(AsmLabel, g_strTvcamera, $L_67e190)     // "tvcamera"
ASM_VAR(AsmLabel, g_vtbl650a6c, $L_650a6c)
ASM_VAR(AsmLabel, g_vtbl650c78, $L_650c78)
ASM_VAR(AsmLabel, g_vtbl650c2c, $L_650c2c)
ASM_VAR(AsmLabel, g_vtbl650d80, $L_650d80)
ASM_VAR(AsmLabel, g_vtbl650d34, $L_650d34)
ASM_VAR(AsmLabel, g_strGetCirclePos, $L_67e19c) // "getCirclePos"
ASM_VAR(AsmLabel, g_strXpyramid, $L_67e1ac)     // "xpyramid"
ASM_VAR(AsmLabel, g_strEffects, $L_672998)      // "EFFECTS"
ASM_VAR(double, g_64fd20, $L_64fd20)            // 0.92
ASM_VAR(double, g_650a98, $L_650a98)            // 0.97
ASM_VAR(double, g_64b5b8, $L_64b5b8)            // 0.1
ASM_VAR(int, g_6f6914, $L_6f6914)
ASM_VAR(float, g_64c328, $L_64c328)             // 0.05
ASM_VAR(double, g_650aa8, $L_650aa8)            // 0.31415926
ASM_VAR(double, g_650aa0, $L_650aa0)            // 1/3
ASM_VAR(float, g_pi650b68, $L_650b68)
ASM_VAR(BYTE*, g_game, $L_6d2078)
ASM_VAR(double, g_xpyramid, $L_67e180)          // -1 until read
ASM_VAR(void*, g_696868, $L_696868)             // the ini file

static void Matrix_Construct(Matrix* m)
{
    for (int k = 0; k < 4; k++)
        Vec3_ZeroThis((Vec3*)((BYTE*)m + k * 12), 0);
    *m = g_identity;
}

extern "C" BYTE* __fastcall TvCamera_Construct(BYTE* self, void*)
{
    CallThis(FUN_5d31e0, self, 99, (const char*)g_strTvcamera);
    Matrix_Construct(&F(Matrix, self, 32));
    Matrix_Construct(&F(Matrix, self, 80));
    Matrix_Construct(&F(Matrix, self, 128));
    Matrix_Construct(&F(Matrix, self, 176));
    F(int, self, 256) = 0;
    F(int, self, 260) = 0;
    F(int, self, 264) = 0;
    Matrix_Construct(&F(Matrix, self, 268));
    F(int, self, 316) = 0;
    F(int, self, 228) = 0;
    F(void*, self, 0) = g_vtbl650a6c;
    return self;
}

// the camera follows its target a part of the way
extern "C" Matrix* __fastcall TvCamera_Follow(BYTE* self, void*, Matrix* out)
{
    float dx = F(float, self, 212) - F(float, self, 164);
    float dy = F(float, self, 216) - F(float, self, 168);
    float dz = F(float, self, 220) - F(float, self, 172);
    Vec3 s;
    s.x = (float)(dx * g_64fd20);
    s.y = (float)(dy * g_650a98);
    s.z = (float)(dz * g_650a98);
    if (!(Vec3_Length(&s) >= g_64b5b8) && g_6f6914 == 1)
        F(int, self, 324) = 1;
    Matrix t1, t2;
    F(Matrix, self, 176) = *Matrix_Multiply(&t2, Matrix_Translation(&t1, s), (const Matrix*)(self + 128));
    Matrix_Multiply(out, (const Matrix*)(self + 176), (const Matrix*)(self + 80));
    return out;
}

// the camera circling its target
extern "C" Matrix* __fastcall TvCamera_Circle(BYTE* self, void*, Matrix* out)
{
    double sv = x87_sin((Game_Time() - F(float, self, 224)) * g_64c328);
    double ft = Game_FrameTime();
    double a = (sv < 0 ? -sv : sv) * sv * g_650aa8;
    float arg = (float)(a * ft + F(float, self, 232));
    F(float, self, 232) = (float)CallC<double>(FUN_4e41d9, arg, (const char*)g_strGetCirclePos);
    Matrix m1, m2, m3, m4, m5, m6;
    Matrix_Translation(&m1, F(Vec3, self, 116));
    Matrix_RotationZ(&m2, F(float, self, 232));
    Vec3 up;
    F(DWORD, &up, 0) = 0;
    F(DWORD, &up, 4) = 0xc1200000;      // -10
    F(DWORD, &up, 8) = 0x40555555;      // 3.3333333
    Matrix_Translation(&m3, up);
    Matrix_RotationX(&m4, (float)-x87_atan2(g_650aa0, 1.0));
    Matrix_Multiply(&m5, &m4, &m3);
    Matrix_Multiply(&m6, &m5, &m2);
    Matrix_Multiply(out, &m6, &m1);
    return out;
}

// the pyramid's size from the ini file, and its width by the picture's aspect
extern "C" void __cdecl Pyramid_SetSize(BYTE* o)
{
    BYTE* b = F(BYTE*, o, 488);
    if (F(int, g_game, 1400))
        F(float, b, 88) = 2.0f;
    else {
        if (!(g_xpyramid >= 0.0))
            CallC(FUN_603cb0, (const char*)g_strEffects, (const char*)g_strXpyramid, 1.6, &g_xpyramid, g_696868);
        F(float, F(BYTE*, o, 488), 88) = (float)g_xpyramid;
    }
    WORD* w = F(WORD*, o, 12);
    b = F(BYTE*, o, 488);
    F(float, b, 84) = (float)((double)w[1] * F(float, b, 88) / (double)w[0]);
}

// asks the objects around a vehicle about a collision
extern "C" unsigned __cdecl Car_AskNear(int* obj, BYTE* q, BYTE* car, BYTE* skip)
{
    unsigned r = 0;
    for (int i = 0; i < 6; i++) {
        BYTE* e = F(BYTE*, car, 3508 + i * 4);
        if (!e)
            break;
        if (e != skip && (r = CallVirt<unsigned>(e + 256, 52, 6, q)) != 0)
            return r;
    }
    void* p = CallC<void*>(FUN_4e5dce, obj);
    if (p)
        r = CallVirt<unsigned>(p, 52, 6, q);
    if (r)
        return r;
    p = CallC<void*>(FUN_4e5c77, obj);
    unsigned t;
    if (p && (t = CallVirt<unsigned>(p, 52, 6, q)) != 0)
        return t;
    r = CallVirt<unsigned>(obj, 52, 0, q);
    if (!(r & 2))
        return r;
    p = CallC<void*>(FUN_4e5dce, F(void*, q, 104));
    if (p && (t = CallVirt<unsigned>(p, 52, 6, q)) != 0)
        return t | r;
    p = CallC<void*>(FUN_4e5c77, F(void*, q, 104));
    if (p && (t = CallVirt<unsigned>(p, 52, 6, q)) != 0)
        return t | r;
    return r;
}

static BYTE* Spark_Init(BYTE* self, void* vtbl, void* vtbl2, const Vec3Bits* p, const Vec3Bits* q, DWORD k)
{
    for (int i = 148; i <= 168; i += 4)
        F(int, self, i) = 0;
    F(void*, self, 0) = vtbl;
    F(void*, self, 20) = vtbl2;
    F(DWORD, self, 128) |= 1;
    F(Vec3Bits, self, 148) = *p;
    F(Vec3Bits, self, 160) = *q;
    F(DWORD, self, 172) = k;
    F(int, self, 176) = 0;
    float r1 = (float)(rand() * (double)g_rand1);
    double r2 = rand() * (double)g_rand1;
    double v = (r1 - r2) * g_pi650b68;
    F(DWORD, self, 188) = 0x3f3504f3;   // 0.70710677
    F(DWORD, self, 184) = 0x3f3504f3;
    F(float, self, 180) = (float)(v + v);
    return self;
}

static inline DWORD FloatBits(float f) { return *(DWORD*)&f; }

extern "C" BYTE* __fastcall Spark59ad70_Construct(BYTE* self, void*, int a, const Vec3Bits* p, const Vec3Bits* q,
                                                  int c, unsigned n, float life)
{
    Effect_Construct(self, 0, a, p, 0x3f800000, 0x40a00000, c, n, FloatBits((float)(life / (double)n)),
                     FloatBits(life), 100);
    return Spark_Init(self, g_vtbl650c78, g_vtbl650c2c, p, q, 0x40400000);
}

extern "C" BYTE* __fastcall Spark59aea0_Construct(BYTE* self, void*, int a, const Vec3Bits* p, const Vec3Bits* q,
                                                  int b, int c, int d, unsigned n, float life, DWORD k)
{
    Effect_Construct(self, 0, a, p, b, c, d, n, FloatBits((float)(life / (double)n)), FloatBits(life), 100);
    return Spark_Init(self, g_vtbl650c78, g_vtbl650c2c, p, q, k);
}

extern "C" BYTE* __fastcall Spark59bed0_Construct(BYTE* self, void*, int a, const Vec3Bits* p, const Vec3Bits* q,
                                                  int c, unsigned n, float life)
{
    Effect_Construct(self, 0, a, p, 0x3f800000, 0x40a00000, c, n, FloatBits((float)(life / (double)n)),
                     FloatBits(life), 100);
    return Spark_Init(self, g_vtbl650d80, g_vtbl650d34, p, q, 0x40400000);
}

ASM_PROC(FUN_59f870)
ASM_VAR(float, g_64c330, $L_64c330)             // 0.75
ASM_VAR(float, g_64c408, $L_64c408)             // 4
ASM_VAR(float, g_64c258, $L_64c258)             // 0.5
ASM_VAR(float, g_64c25c, $L_64c25c)             // 0.8
ASM_VAR(float, g_64c338, $L_64c338)             // 0.3
ASM_VAR(float, g_650ee0, $L_650ee0)             // 1.28
ASM_VAR(AsmLabel, g_vtbl650dd4, $L_650dd4)
ASM_VAR(AsmLabel, g_vtbl650d88, $L_650d88)
ASM_VAR(AsmLabel, g_vtbl650e28, $L_650e28)
ASM_VAR(AsmLabel, g_vtbl650ddc, $L_650ddc)
ASM_VAR(AsmLabel, g_vtbl650e84, $L_650e84)
ASM_VAR(AsmLabel, g_vtbl650e38, $L_650e38)
ASM_VAR(AsmLabel, g_vtbl650ed8, $L_650ed8)
ASM_VAR(AsmLabel, g_vtbl650e8c, $L_650e8c)
ASM_VAR(AsmLabel, g_vtbl651174, $L_651174)
ASM_VAR(AsmLabel, g_vtbl651128, $L_651128)

static inline DWORD Per(float c, unsigned n) { return FloatBits((float)(c / (double)n)); }

static float RandAngle2()
{
    float r1 = (float)(rand() * (double)g_rand1);
    double r2 = rand() * (double)g_rand1;
    double v = (r1 - r2) * g_pi650b68;
    return (float)(v + v);
}

static void Effect_SetVtbls(BYTE* self, void* v0, void* v20, bool flag)
{
    for (int i = 148; i <= 168; i += 4)
        F(int, self, i) = 0;
    F(void*, self, 0) = v0;
    if (flag)
        F(DWORD, self, 128) |= 1;
    F(void*, self, 20) = v20;
}

extern "C" BYTE* __fastcall Effect59c420_Construct(BYTE* self, void*, int a, const Vec3Bits* p, const Vec3Bits* q,
                                                   float s, int c, unsigned n)
{
    Effect_Construct(self, 0, a, p, FloatBits((float)(s * g_64c258)), FloatBits((float)(s * g_64c408)), c, n,
                     Per(g_64c330, n), 0x3f400000, 100);
    Effect_SetVtbls(self, g_vtbl650dd4, g_vtbl650d88, true);
    F(Vec3Bits, self, 148) = *p;
    F(Vec3Bits, self, 160) = *q;
    F(DWORD, self, 172) = 0x40400000;   // 3
    return self;
}

extern "C" BYTE* __fastcall Effect59c6c0_Construct(BYTE* self, void*, int a, const Vec3Bits* p, const float* u,
                                                   const float* v, float s, int c, unsigned n)
{
    Effect_Construct(self, 0, a, p, FloatBits((float)(s * g_64c258)), FloatBits(s + s), c, n,
                     Per(g_64c25c, n), 0x3f4ccccd, 100);
    Effect_SetVtbls(self, g_vtbl650e28, g_vtbl650ddc, true);
    F(Vec3Bits, self, 148) = *p;
    F(float, self, 160) = u[0] - v[0];
    F(float, self, 164) = u[1] - v[1];
    F(DWORD, self, 172) = 0x41700000;   // 15
    F(int, self, 176) = 0;
    F(float, self, 168) = u[2] - v[2];
    F(float, self, 180) = RandAngle2();
    F(DWORD, self, 188) = 0x3f3504f3;
    F(DWORD, self, 184) = 0x3f3504f3;
    return self;
}

extern "C" BYTE* __fastcall Effect59cea0_Construct(BYTE* self, void*, int a, const Vec3Bits* p, const Vec3Bits* q,
                                                   int unused, DWORD s, int c, unsigned n)
{
    Effect_Construct(self, 0, a, p, s, s, c, n, Per(g_64c338, n), 0x3e99999a, 100);
    Effect_SetVtbls(self, g_vtbl650e84, g_vtbl650e38, true);
    F(Vec3Bits, self, 148) = *p;
    F(Vec3Bits, self, 160) = *q;
    F(int, self, 172) = 0;
    F(float, self, 176) = RandAngle2();
    F(float, self, 180) = RandAngle2();
    F(DWORD, self, 188) = 0x3f3504f3;
    F(DWORD, self, 184) = 0x3f3504f3;
    return self;
}

extern "C" BYTE* __fastcall Effect59d4a0_Construct(BYTE* self, void*, int a, const Vec3Bits* p, float s, int c,
                                                   unsigned n)
{
    Effect_Construct(self, 0, a, p, 0, FloatBits((float)(s * g_64c408)), c, n, Per(g_650ee0, n), 0x3fa3d70a, 100);
    Effect_SetVtbls(self, g_vtbl650ed8, g_vtbl650e8c, true);
    F(Vec3Bits, self, 148) = *p;
    F(Vec3, self, 160) = g_zeroVec;
    F(int, self, 172) = 0;
    F(int, self, 176) = 0;
    return self;
}

extern "C" BYTE* __fastcall Effect59f070_Construct(BYTE* self, void*, int a, const Vec3Bits* p, const Vec3Bits* q,
                                                   DWORD k, int c, int n)
{
    Effect_Construct(self, 0, a, p, k, k, c, n, 0x42c80000, 0x42c80000, 100);   // 100.0f
    Effect_SetVtbls(self, g_vtbl651174, g_vtbl651128, false);
    F(Vec3Bits, self, 160) = *p;
    F(Vec3Bits, self, 148) = *q;
    F(DWORD, self, 188) = k;
    F(int, self, 176) = 0;
    F(float, self, 172) = RandAngle2();
    F(DWORD, self, 184) = 0x3f3504f3;
    F(DWORD, self, 180) = 0x3f3504f3;
    F(int, self, 192) = 1;
    return self;
}

// adds an effect to the table of 50 (or restarts the one of the same key)
extern "C" void __fastcall Table124_Add(BYTE* self, void*, int a, int b, const Vec3Bits* v, int d, int e)
{
    if (F(int, self, 6432) == 50)
        return;
    BYTE* o = Table124_Find(self, 0, a, d);
    if (!o) {
        int n = F(int, self, 6432);
        CallThis(FUN_59f870, self + 32 + n * 124, a, b, *v, d, e);
        F(int, self, 136 + F(int, self, 6432) * 124) = 0;
        F(int, self, 140 + F(int, self, 6432) * 124) = 1;
        F(int, self, 6432)++;
        return;
    }
    CallThis(FUN_59f870, o, a, b, *v, d, e);
    F(int, o, 104) = 0;
    F(int, o, 108) = 1;
}

// Effect drawing: validate its secondary render base, submit it, and apply
// the position stored by the effect before completing the draw.
ASM_PROC(FUN_5b4fc0)
ASM_PROC(FUN_5ef800)
ASM_PROC(FUN_5b3010)
extern "C" void __fastcall SceneTexture_Advance(BYTE* self, void*); // engine.cpp
ASM_VAR(float, g_effectOne, $L_64c250)
ASM_VAR(float, g_effectFadeScale, $L_64c33c)
ASM_VAR(float, g_effectFadeBase, $L_64c260)
ASM_VAR(double, g_effectZero, $L_64b678)
ASM_VAR(double, g_effectDoubleOne, $L_64b670)
ASM_VAR(double, g_effectNegativeHalf, $L_64b6b8)
ASM_VAR(double, g_effectHalf, $L_64b6e0)

extern "C" int __fastcall Effect_Draw(BYTE* self, void*, DWORD context, BYTE* renderer)
{
    CallVirt(self, 72);
    if (!CallThis<int>(FUN_5b4fc0, renderer, self + 20))
        return 0;
    if (!CallThis<int>(FUN_5ef800, self, context, renderer))
        return 0;
    CallThis(FUN_5b3010, renderer, F(DWORD, self, 52), F(Vec3Bits, self, 56));
    return 1;
}

static BYTE* Effect_CurrentFrame(BYTE* self)
{
    return F(BYTE**, self, 100)[F(int, self, 88)];
}

extern "C" int __fastcall Effect_DrawFade(BYTE* self, void*, DWORD context, BYTE* renderer)
{
    double opacity = (g_effectOne - ((double)F(float, self, 80) - F(float, self, 84))
                                    / F(float, self, 80)) * g_64c25c;
    if (opacity > g_64c25c)
        opacity = g_64c25c;
    F(float, Effect_CurrentFrame(self), 88) = (float)opacity;
    return Effect_Draw(self, 0, context, renderer);
}

extern "C" int __fastcall Effect_DrawSoftFade(BYTE* self, void*, DWORD context, BYTE* renderer)
{
    F(DWORD, Effect_CurrentFrame(self), 32) &= ~0x1000u;
    double opacity = (g_effectOne - ((double)F(float, self, 80) - F(float, self, 84))
                                    / F(float, self, 80)) * g_effectFadeScale + g_effectFadeBase;
    if (opacity > g_64c25c)
        opacity = g_64c25c;
    F(float, Effect_CurrentFrame(self), 88) = (float)opacity;
    return Effect_Draw(self, 0, context, renderer);
}

extern "C" int __fastcall Effect_DrawTinted(BYTE* self, void*, DWORD context, BYTE* renderer)
{
    F(DWORD, Effect_CurrentFrame(self), 88) = 1063004406;
    F(DWORD, Effect_CurrentFrame(self), 40) = F(DWORD, self, 184);
    F(DWORD, Effect_CurrentFrame(self), 32) |= 0x1000;
    int result = Effect_Draw(self, 0, context, renderer);
    F(DWORD, Effect_CurrentFrame(self), 32) &= ~0x1000u;
    return result;
}

// Interpolate the two size axes and update the quad's signed extents.
extern "C" void __fastcall Effect_AnimateQuad(BYTE* self, void*)
{
    double t = ((double)F(float, self, 100) - F(float, self, 104)) / F(float, self, 100);
    if (!(t >= g_effectZero))
        t = g_effectZero;
    else if (t > g_effectDoubleOne)
        t = g_effectDoubleOne;
    double remaining = g_effectDoubleOne - t;
    double width = F(float, self, 136) * remaining + F(float, self, 132) * t;
    double height = F(float, self, 144) * remaining + F(float, self, 140) * t;
    float* x = F(float*, F(BYTE*, self, 72), 16);
    x[0] = (float)(width * g_effectNegativeHalf);
    x[1] = (float)(width * g_effectHalf);
    x[2] = x[1];
    x[3] = x[0];
    float* y = F(float*, F(BYTE*, self, 72), 20);
    y[0] = (float)(height * g_effectNegativeHalf);
    y[1] = y[0];
    y[2] = (float)(height * g_effectHalf);
    y[3] = y[2];
    SceneTexture_Advance(self, 0);
}

extern "C" void __fastcall Effect_MoveQuad(BYTE* self, void*)
{
    double step = F(double, F(BYTE*, self, 160), 2960);
    // The original rounds the x/y deltas to float before adding them.
    float dx = (float)(step * F(float, self, 148));
    float dy = (float)(step * F(float, self, 152));
    double dz = step * F(float, self, 156);
    F(float, self, 76) = (float)((double)dx + F(float, self, 76));
    F(float, self, 80) = (float)((double)dy + F(float, self, 80));
    F(float, self, 84) = (float)(dz + F(float, self, 84));
    Effect_AnimateQuad(self, 0);
}

ASM_PROC(FUN_5e48f0)
ASM_PROC(FUN_5fa720)
ASM_PROC(FUN_5d3570)
ASM_PROC(FUN_5ef780)
extern "C" void* __cdecl memset(void* d, int c, size_t n);
extern "C" void FUN_59f790(); KING_ALIAS("_FUN_59f790", "@Wasm59f790_Construct@8")
ASM_VAR(char, g_vtbl650bd0, $L_650bd0)
ASM_VAR(char, g_vtbl650b84, $L_650b84)
ASM_VAR(char, g_vtbl650334, $L_650334)
ASM_VAR(char, g_vtbl650f5c, $L_650f5c)
ASM_VAR(char, g_vtbl651188, $L_651188)
ASM_VAR(BYTE*, g_dustColor, $L_6f6a74)
ASM_VAR(BYTE*, g_696224, $L_696224)

extern "C" {

// [FUN_59aa90] destructor
void __fastcall Wasm59aa90_Destroy(BYTE* self, void*)
{
    BYTE* sub = self + 20;
    F(void*, self, 0) = &g_vtbl650bd0;
    F(void*, sub, 0) = &g_vtbl650b84;
    CallThis(FUN_5e48f0, F(BYTE*, self, 92));
    CallThis(FUN_5ef780, sub);
    CallThis(FUN_5fa720, self ? sub : 0);
    F(void*, self, 0) = &g_vtbl650334;
    F(int, self, 4) = 0;
}

// [FUN_59daf0] creates the DustColor task once
void __cdecl DustColor_Create(int a)
{
    if (g_dustColor)
        return;
    BYTE* o = (BYTE*)operator new(36);
    if (o) {
        CallThis(FUN_5d31e0, o, 98, "DustColor");
        F(void*, o, 0) = &g_vtbl650f5c;
        F(int, o, 32) = a;
    }
    BYTE* owner = g_696224;
    g_dustColor = o;
    CallThis(FUN_5d3570, owner + 2992, o);
    CallVirt(g_dustColor, 24, 1);
}

// [FUN_5a0000] constructor of the VehGlowManager task: 50 glows
BYTE* __fastcall VehGlowManager_Construct(BYTE* self, void*, BYTE* owner)
{
    CallThis(FUN_5d31e0, self, 390, "VehGlowManager");
    for (int i = 0; i < 50; i++)
        CallThis(FUN_59f790, self + 32 + i * 124);
    F(void*, self, 0) = &g_vtbl651188;
    F(int, self, 6432) = 0;
    memset(self + 6232, 0, 200);
    CallThis(FUN_5d3570, owner + 2992, self);
    CallThis(FUN_5d33d0, self, 1);
    return self;
}

}

extern "C" void FUN_595b60(); KING_ALIAS("_FUN_595b60", "@TvCamera_Construct@8")
extern "C" void FUN_596850(); KING_ALIAS("_FUN_596850", "@SceneEffect_Start@16")
extern "C" void FUN_595c70(); KING_ALIAS("_FUN_595c70", "@Wasm_595c70@8")
ASM_VAR(char, g_vtbl650ab0, $L_650ab0)

extern "C" {

// [FUN_597a20] creates the TV camera monitor and its task
void __fastcall TvCamera_CreateMonitor(BYTE* self, void*)
{
    BYTE* o = (BYTE*)operator new(336);
    BYTE* m = o ? CallThis<BYTE*>(FUN_595b60, o) : 0;
    BYTE* owner = F(BYTE*, self, 92);
    F(BYTE*, self, 124) = m;
    CallThis(FUN_5d3570, owner + 2992, m);
    CallThis(FUN_596850, F(BYTE*, self, 124), F(int, self, 72), self + 24);
    CallThis(FUN_595c70, F(BYTE*, self, 124));
    BYTE* t = (BYTE*)operator new(32);
    if (t) {
        CallThis(FUN_5d31e0, t, 98, "tvcameramonitor");
        F(void*, t, 0) = &g_vtbl650ab0;
    }
    CallThis(FUN_5d3570, F(BYTE*, self, 92) + 2992, t);
    CallVirt(t, 24, 1);
}

}

ASM_PROC(FUN_5d9f60)
extern "C" void* __cdecl memcpy(void* d, const void* s, size_t n);
extern "C" void FUN_5eb1e0(); KING_ALIAS("_FUN_5eb1e0", "@Camera_Init@8")
extern "C" void FUN_5e4210(); KING_ALIAS("_FUN_5e4210", "@Windows_Add@12")
extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")
ASM_VAR(BYTE*, g_world696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_69684c, $L_69684c)
ASM_VAR(char, g_696750, $L_696750)
ASM_VAR(char, g_empty68d1c0, $L_68d1c0)
typedef DWORD Dwords12w[12];
ASM_VAR(Dwords12w, g_rayDefaultsW, $L_7025f0)
ASM_VAR(char, g_vtbl6509f8, $L_6509f8)
ASM_VAR(char, g_vtbl64d7d0, $L_64d7d0)
ASM_VAR(char, g_vtbl64d7bc, $L_64d7bc)

extern "C" {

// [FUN_5950b0] constructor: its +544 part copies the fields of the part the
// base set up at +472 and replaces it
BYTE* __fastcall Wasm5950b0_Construct(BYTE* self, void*, int a, int b, int c)
{
    CallThis(FUN_5d9f60, self, a, b, c, F(int, g_world696ca0, 472));
    BYTE* sub = self + 544;
    CallThis(FUN_5eb1e0, sub);
    BYTE* old = F(BYTE*, self, 472);
    F(void*, self, 0) = &g_vtbl6509f8;
    for (int off = 4; off <= 132; off += 4)
        if (off != 12 && off != 92)
            F(int, sub, off) = F(int, old, off);
    F(int, self, 620) = 1;
    F(BYTE*, self, 472) = sub;
    return self;
}

// [FUN_597410] creates the objects of a screen element (430, 43)-(530, 118)
void __fastcall Wasm597410_Create(BYTE* self, void*)
{
    BYTE* o = (BYTE*)operator new(112);
    if (o) {
        CallThis(FUN_5f19b0, o, &g_empty68d1c0, 6, 1);
        for (int i = 0; i < 12; i++)
            F(int, o, 64 + i * 4) = 0;
        memcpy(o + 64, g_rayDefaultsW, 48);
        F(void*, o, 0) = &g_vtbl64d7d0;
        F(int, o, 56) = 0;
        F(int, o, 60) = 0;
    }
    F(BYTE*, self, 16) = o;
    F(BYTE*, self, 20) = o;
    BYTE* p = (BYTE*)operator new(104);
    if (p) {
        F(int, p, 4) = 0;
        F(BYTE*, p, 80) = F(BYTE*, self, 16);
        F(int, p, 8) = 0;
        F(void*, p, 0) = &g_vtbl64d7bc;
        F(int, p, 76) = 0;
        F(DWORD, p, 88) = 0x3f800000;
        F(DWORD, p, 84) = 0x3f800000;
        F(int, p, 96) = 0;
        F(int, p, 92) = 0;
        F(BYTE, p, 44) = 0;
        F(BYTE, p, 12) = 0;
        F(int, p, 100) = 0;
    }
    F(BYTE*, self, 116) = p;
    F(BYTE*, self, 120) = p;
    F(int, self, 0) = 430;
    F(int, self, 8) = 530;
    F(int, self, 4) = 43;
    F(int, self, 12) = 118;
    BYTE* q = (BYTE*)operator new(680);
    if (q)
        q = Wasm5950b0_Construct(q, 0, (int)g_69684c, (int)self, F(int, self, 116));
    F(BYTE*, self, 108) = q;
    F(BYTE*, self, 112) = q;
    CallThis(FUN_5e4210, &g_696750, q);
}

}

// [FUN_59ff20] clips the segment a-b against the six planes: 0 (and *t = 0) if a is outside one, else 1 and the entry fraction (2 if none)
extern "C" int __fastcall Box_ClipSegment(BYTE* self, void*, const float* a, const float* b, float* t)
{
    float tk[6];
    for (int k = 0; k < 6; k++) {
        const float* p = (const float*)(self + 8 + k * 16);
        double da = -((((double)p[2] * a[2] + (double)p[0] * a[0]) + (double)p[1] * a[1]) + p[3]);
        double db = -((((double)p[1] * b[1] + (double)p[0] * b[0]) + (double)p[2] * b[2]) + p[3]);
        if (!(da > 0.0f)) {
            *t = 0;
            return 0;
        }
        if (!(db >= 0.0f))
            tk[k] = (float)(da / (da - db));
        else
            tk[k] = 1.0f;
    }
    *t = 2.0f;
    for (int i = 0; i < 6; i++)
        if (!(tk[i] >= *t))
            *t = tk[i];
    return 1;
}

ASM_VAR(const double, k_halfPi596e50, $L_64da00)      // 1.5707963

// [FUN_596e50] the matrix that turns towards the object's aim point (+116) seen from m
extern "C" Matrix* __fastcall Object_AimMatrix(BYTE* s, void*, Matrix* out, const Matrix* m)
{
    Matrix inv;
    const float* M = (const float*)Matrix_Inverse(&inv, m);
    Vec3 p;
    p.x = (float)(((double)M[6] * F(float, s, 124) + (double)M[3] * F(float, s, 120)) + (double)F(float, s, 116) * M[0] + M[9]);
    p.y = (float)(((double)M[1] * F(float, s, 116) + (double)M[7] * F(float, s, 124)) + (double)M[4] * F(float, s, 120) + M[10]);
    p.z = (float)(((double)M[2] * F(float, s, 116) + (double)M[8] * F(float, s, 124)) + (double)M[5] * F(float, s, 120) + M[11]);
    float az, pol;
    Vec3_ToAngles(p, &az, &pol);
    float tp = (float)(pol - k_halfPi596e50);
    float ta = (float)(az - k_halfPi596e50);
    Matrix* rz = Matrix_RotationZ(&inv, ta);
    Matrix rx, a, r;
    Matrix* x = Matrix_RotationX(&rx, -tp);
    Matrix_Multiply(&r, Matrix_Multiply(&a, x, rz), m);
    *out = r;
    return out;
}

ASM_VAR(Vec3, g_fwd702620, $L_702620)
ASM_VAR(Vec3, g_up702640b, $L_702640)
extern "C" Vec3Bits* __cdecl CarType_Point(Vec3Bits* out, const BYTE* car, int i);

// [FUN_598f00] the matrix of a wheel-like object: turned to m's heading plus +112, offset by the player's car point 1 and by +104 sideways
extern "C" Matrix* __fastcall Object_WheelMatrix(BYTE* s, void*, Matrix* out, const Matrix* m)
{
    const float* M = (const float*)m;
    const Vec3& g = g_fwd702620;
    Vec3 v, nb;
    v.x = (float)(((double)g.z * M[6] + (double)g.y * M[3]) + (double)g.x * M[0]);
    v.y = (float)(((double)g.z * M[7] + (double)g.y * M[4]) + (double)g.x * M[1]);
    v.z = 0;
    Vec3 n = *Vec3_Normalized(&nb, &v);
    double d = ((double)g.x * n.x + (double)g.y * n.y) + (double)g.z * n.z;
    if (d > 1.0) d = 1.0;
    else if (!(d >= -1.0)) d = -1.0;
    double ang = x87_acos(d);
    const Vec3& u = g_up702640b;
    if (((double)u.x * n.x + (double)u.y * n.y) + (double)u.z * n.z > 0.0)
        ang = -ang;
    Vec3Bits pb;
    const Vec3* r = (const Vec3*)CarType_Point(&pb, F(BYTE*, g_game, 616), 1);
    Vec3 p;
    p.x = (float)((double)r->x + M[9]);
    p.y = (float)((double)r->y + M[10]);
    p.z = (float)((double)r->z + M[11]);
    Vec3 side;
    side.x = 0;
    side.y = (float)-F(double, s, 104);
    side.z = (float)(F(double, s, 104) * F(double, s, 104) * 0.001);
    Matrix t1, rz, t2, a;
    Matrix* T1 = Matrix_Translation(&t1, p);
    Matrix* Rz = Matrix_RotationZ(&rz, (float)(ang + F(double, s, 112)));
    Matrix* T2 = Matrix_Translation(&t2, side);
    Matrix_Multiply(out, Matrix_Multiply(&a, T2, Rz), T1);
    return out;
}

extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
extern "C" int __cdecl rand(void);
extern "C" void FUN_59ab10(); KING_ALIAS("_FUN_59ab10", "@Effect_Construct@44")
ASM_VAR(BYTE*, g_view5a0370, $L_696224)
ASM_VAR(char, g_vtbl6511f8, $L_6511f8)
ASM_VAR(char, g_vtbl6511ac, $L_6511ac)

// [FUN_5a0370] constructs a spark effect: a random size and spin, placed at a random spot within a4 of *at
extern "C" BYTE* __fastcall Spark_Construct(BYTE* s, void*, int a0, int a1, int a2, const float* at, float spread, int a5, int a6, int a7)
{
    const float k = 3.0518509447574615e-05f;    // 1/32767
    float r1 = (float)((double)rand() * k);
    float v = (float)(((double)rand() * k - r1) * 1.5f + 2.0f);
    CallThis(FUN_59ab10, s, g_view5a0370, a2, a5, a5, a6, a7, 6.599999904632568f, v, 100);
    F(int, s, 156) = 0;
    F(int, s, 160) = 0;
    F(int, s, 164) = 0;
    F(void*, s, 0) = &g_vtbl6511f8;
    F(void*, s, 20) = &g_vtbl6511ac;
    F(int, s, 152) = a1;
    F(int, s, 148) = a0;
    int r = rand();
    F(DWORD, s, 180) = 0x3F3504F3;
    F(DWORD, s, 176) = 0x3F3504F3;
    F(float, s, 168) = (float)((double)r * 0.00019175389024894685f);
    float x1 = (float)((double)rand() * k);
    float x2 = (float)((double)rand() * k);
    double x3 = (double)rand() * k;
    float p2 = (float)((double)x2 * 0.10000000149011612f);
    float p1 = (float)((double)x1 * 0.10000000149011612f);
    double q3 = x3 * 0.10000000149011612f;
    float dx = (float)((double)p2 * spread);
    float dy = (float)((double)p1 * spread);
    double dz = q3 * spread;
    F(float, s, 156) = (float)((double)dx + at[0]);
    F(float, s, 160) = (float)((double)dy + at[1]);
    F(float, s, 164) = (float)(dz + at[2]);
    F(int, s, 184) = a5;
    return s;
}

ASM_VAR(Matrix, g_identity7025f0, $L_7025f0)

// [FUN_594e70] follows a ray through an object (virtual +52 steps) up to 10 times; the end point to *out, the object reached returned
extern "C" BYTE* __cdecl Object_TraceRay(int a0, int a1, BYTE* obj, int a3, int a4, int a5, float* out)
{
    BYTE node[256];
    CallThis(FUN_5ef5c0, node);
    if (!obj)
        return 0;
    F(Matrix, node, 4) = g_identity7025f0;
    F(int, node, 76) = a3;
    F(int, node, 80) = a4;
    F(int, node, 84) = a5;
    F(DWORD, node, 88) = ((DWORD*)out)[0];
    F(DWORD, node, 92) = ((DWORD*)out)[1];
    F(int, node, 116) = 0;
    F(DWORD, node, 0) = 0x3E99999A;     // 0.3f
    F(DWORD, node, 96) = ((DWORD*)out)[2];
    F(BYTE*, node, 104) = obj;
    F(int, node, 120) = a1;
    int flags = a0 | 2;
    DWORD x, y, z;
    BYTE* hit;
    for (int i = 0; ; i++) {
        if (!(CallVirt<int>(obj, 52, flags, node) & 1)) {
            z = F(DWORD, node, 96); y = F(DWORD, node, 92); x = F(DWORD, node, 88);
            hit = F(BYTE*, node, 104);
            break;
        }
        if (i == 9) {
            x = F(DWORD, node, 76); y = F(DWORD, node, 80); z = F(DWORD, node, 84);
            hit = obj;
            break;
        }
        float sc = F(float, node, 0);
        float tx = (float)((double)F(float, node, 64) * sc);
        float ty = (float)((double)F(float, node, 68) * sc);
        float tz = (float)((double)F(float, node, 72) * sc);
        Matrix inv;
        const float* M = (const float*)Matrix_Inverse(&inv, (Matrix*)(node + 4));
        double wx = ((double)tz * M[6] + (double)ty * M[3]) + (double)tx * M[0] + M[9];
        float wy = (float)(((double)tz * M[7] + (double)ty * M[4]) + (double)tx * M[1] + M[10]);
        float wz = (float)(((double)tz * M[8] + (double)ty * M[5]) + (double)tx * M[2] + M[11]);
        F(BYTE*, node, 104) = obj;
        double wx1 = wx * 1.0099999904632568f;
        float wy1 = (float)((double)wy * 1.0099999904632568f);
        float wz1 = (float)((double)wz * 1.0099999904632568f);
        float nx = (float)((double)F(float, node, 52) + wx1);
        F(float, node, 88) = nx;
        float ny = (float)((double)wy1 + F(float, node, 56));
        F(float, node, 92) = ny;
        float nz = (float)((double)wz1 + F(float, node, 60));
        F(float, node, 96) = nz;
        if (i + 1 >= 10) {
            x = *(DWORD*)&nx; y = *(DWORD*)&ny; z = *(DWORD*)&nz;
            hit = obj;
            break;
        }
    }
    ((DWORD*)out)[0] = x;
    ((DWORD*)out)[1] = y;
    ((DWORD*)out)[2] = z;
    return hit;
}

extern "C" void FUN_5949f0(); KING_ALIAS("_FUN_5949f0", "@Fader_Construct@28")    // a fader's constructor
extern "C" void FUN_594b60(); KING_ALIAS("_FUN_594b60", "@Fader_Link@12")
ASM_VAR(int, g_faders6f66b0, $L_6f66b0)

// [FUN_594c90] a new fader (strength clamped to 1; none under 0.05), linked into the fader list
extern "C" void __cdecl Fader_Start(int a0, int a1, float strength, int a3, int a4)
{
    if (!(strength >= 0.05000000074505806f))
        return;
    BYTE* o = (BYTE*)operator new(32);
    if (o) {
        if (strength > 1.0f)
            o = CallThis<BYTE*>(FUN_5949f0, o, a0, a1, 1.0f, a3, a4);
        else
            o = CallThis<BYTE*>(FUN_5949f0, o, a0, a1, strength, a3, a4);
    }
    CallThis(FUN_594b60, o, g_faders6f66b0);
}

extern "C" BYTE* __fastcall NodeN_Construct(BYTE* self, void*, const char* name, int a, int b);
extern "C" void __fastcall Light_Init(BYTE* self, void*);
ASM_VAR(char, g_strThrowLamp5949f0, $L_672c60)        // "ThrowLamp"
ASM_VAR(char, g_vtbl64d444w, $L_64d444)
ASM_VAR(const float, k_green650978, $L_650978)
ASM_VAR(const float, k_blue650974, $L_650974)

// [FUN_5949f0] a fader: its own "ThrowLamp" light of strength k at p, added to the car's lights
extern "C" BYTE* __fastcall Fader_Construct(BYTE* s, void*, BYTE* car, const Vec3* p, float k, int a3, int a4)
{
    F(int, s, 12) = 0;
    F(int, s, 16) = 0;
    F(int, s, 20) = 0;
    F(int, s, 4) = 0;
    F(int, s, 0) = 0;
    F(Vec3, s, 12) = *p;
    BYTE* o = (BYTE*)operator new(144);
    if (o) {
        NodeN_Construct(o, 0, &g_strThrowLamp5949f0, 21, 1);
        Light_Init(o + 56, 0);
        F(void*, o, 0) = &g_vtbl64d444w;
        F(int, o, 136) = 1;
        F(int, o, 140) = 0;
    }
    F(BYTE*, s, 24) = o;
    F(int, o, 136) = 1;
    BYTE* l = F(BYTE*, s, 24);
    F(int, l, 140) = 1;
    F(int, l, 56) = 2;
    F(Vec3, l, 60) = F(Vec3, s, 12);
    F(DWORD, l, 96) = 0x3F800000;
    F(float, l, 108) = k;
    F(float, l, 112) = (float)((double)k * k_green650978);
    F(float, l, 116) = (float)((double)k * k_blue650974);
    F(DWORD, l, 84) = 0x3F800000;
    F(int, l, 88) = 0;
    F(int, l, 92) = a4;
    F(int, l, 128) = 70;
    BYTE* lights = F(BYTE*, car, 528);
    if (lights)
        CallVirt(lights, 20, 0, F(BYTE*, s, 24));
    F(int, s, 8) = a3;
    F(float, s, 28) = (float)F(double, car, 2968);
    return s;
}

extern "C" void FUN_59abb0(); KING_ALIAS("_FUN_59abb0", "@Effect_Construct2@52")
ASM_FUNC(Crt_floor, _floor)
ASM_VAR(char, g_vtbl650fc8, $L_650fc8)
ASM_VAR(char, g_vtbl650f7c, $L_650f7c)

// [FUN_59df00] constructs a debris effect: a random one of n frames, a random spin, falling (-9.81)
extern "C" BYTE* __fastcall Debris_Construct(BYTE* s, void*, int a0, int a1, const Vec3* at, int a3, int a4, int n)
{
    CallThis(FUN_59abb0, s, a0, a1, a3, a3, a3, a3, a4, n, 1000.0f, 3.0f, 100);
    F(int, s, 148) = 0;
    F(int, s, 152) = 0;
    F(int, s, 156) = 0;
    F(DWORD, s, 128) |= 1;
    F(void*, s, 0) = &g_vtbl650fc8;
    F(void*, s, 20) = &g_vtbl650f7c;
    F(Vec3, s, 148) = *at;
    int r = rand();
    int f = (int)CallC<double>(Crt_floor, (double)r * n * 3.0518509447574615e-05f);
    F(int, s, 116) = f;
    if (f > n - 1)
        F(int, s, 116) = n - 1;
    Vec3_Length(at);
    r = rand();
    F(DWORD, s, 176) = 0xC11CF5C3;      // -9.81f
    F(DWORD, s, 172) = 0x3F3504F3;
    F(DWORD, s, 168) = 0x3F3504F3;
    F(int, s, 164) = 0;
    F(float, s, 160) = (float)(((double)r * 3.0518509447574615e-05f + 1.0f) * 3.141592502593994f);
    return s;
}

ASM_VAR(char, g_vtbl651020, $L_651020)
ASM_VAR(char, g_vtbl650fd4, $L_650fd4)

// [FUN_59e2f0] constructs a moving debris effect from a to b: a random one of n frames, a random spin
extern "C" BYTE* __fastcall Debris2_Construct(BYTE* s, void*, int a0, const Vec3* from, const Vec3* to, int a3, int a4, int n)
{
    float nf = (float)(double)(__int64)(unsigned)n;
    CallThis(FUN_59abb0, s, a0, from, a3, a3, a3, a3, a4, n, (float)((double)nf * 0.25f), 4.0f, 100);
    for (int o = 148; o <= 168; o += 4) F(int, s, o) = 0;
    F(void*, s, 0) = &g_vtbl651020;
    F(void*, s, 20) = &g_vtbl650fd4;
    F(Vec3, s, 148) = *from;
    F(DWORD, s, 128) |= 1;
    F(Vec3, s, 160) = *to;
    int r = rand();
    int f = (int)CallC<double>(Crt_floor, (double)r * nf * 3.0518509447574615e-05f);
    F(int, s, 116) = f;
    if (f > n - 1)
        F(int, s, 116) = n - 1;
    Vec3_Length(to);
    r = rand();
    F(DWORD, s, 188) = 0x40400000;      // 3.0f
    F(DWORD, s, 184) = 0x3F3504F3;
    F(DWORD, s, 180) = 0x3F3504F3;
    F(int, s, 176) = 0;
    F(int, s, 192) = 0;
    F(int, s, 196) = 0;
    F(int, s, 200) = 0;
    F(float, s, 172) = (float)(((double)r * 3.0518509447574615e-05f + 1.0f) * 3.141592502593994f);
    return s;
}

extern "C" void FUN_59ab10(); KING_ALIAS("_FUN_59ab10", "@Effect_Construct@44")
extern "C" Vec3* __fastcall Vec3_DivAssign(Vec3* self, void*, Vec3* ret, float k);
extern "C" Vec3* __fastcall Vec3_MulAssign(Vec3* self, void*, Vec3* ret, float k);
ASM_VAR(Vec3, g_axis702630w, $L_702630)
ASM_VAR(Vec3, g_up702640w, $L_702640)
ASM_VAR(char, g_vtbl650cd8, $L_650cd8)
ASM_VAR(char, g_vtbl650c8c, $L_650c8c)
ASM_VAR(char, g_vtbl650f34, $L_650f34)
ASM_VAR(char, g_vtbl650ee8, $L_650ee8)
ASM_VAR(const float, k_spin650c88, $L_650c88)
ASM_VAR(const float, k_spin650c84, $L_650c84)
ASM_VAR(const float, k_spin64d400, $L_64d400)
ASM_VAR(unsigned, g_sparkCount6f6a6c, $L_6f6a6c)

// the unit axis across dir and the up axis at +offset (up itself when they are parallel), scaled by k
static void Spark_SideAxis(BYTE* s, int offset, const float* d, float k)
{
    const Vec3& g = g_axis702630w;
    Vec3* a = (Vec3*)(s + offset);
    a->x = (float)((double)g.z * d[1] - (double)g.y * d[2]);
    a->y = (float)((double)g.x * d[2] - (double)g.z * d[0]);
    a->z = (float)((double)g.y * d[0] - (double)g.x * d[1]);
    double len = Vec3_Length(a);
    float lf = (float)len;
    Vec3 t;
    if (!(len >= 0.01))
        *a = g_up702640w;
    else
        Vec3_DivAssign(a, 0, &t, lf);
    Vec3_MulAssign(a, 0, &t, k);
}

// [FUN_59b3f0] constructs a flash effect at p moving along d, with a random spin and a side axis
extern "C" BYTE* __fastcall Flash_Construct(BYTE* s, void*, BYTE* view, const Vec3* p, const float* d, int a3, unsigned n, float size, float total)
{
    CallThis(FUN_59ab10, s, view, p, size, (float)((double)size * 5.0f), a3, n,
             (float)((double)total / (double)(__int64)n), total, 100);
    for (int o = 148; o <= 168; o += 4) F(int, s, o) = 0;
    F(int, s, 192) = 0;
    F(int, s, 196) = 0;
    F(int, s, 200) = 0;
    F(void*, s, 0) = &g_vtbl650cd8;
    F(DWORD, s, 128) |= 1;
    F(void*, s, 20) = &g_vtbl650c8c;
    F(Vec3, s, 148) = *p;
    F(Vec3, s, 160) = *(const Vec3*)d;
    F(DWORD, s, 172) = 0x40400000;      // 3.0f
    F(int, s, 176) = 0;
    float r1 = (float)((double)rand() * 3.0518509447574615e-05f);
    double r2 = (double)rand() * 3.0518509447574615e-05f;
    double sp = ((double)r1 - r2) * 3.141592502593994f;
    F(float, s, 180) = (float)(sp + sp);
    F(DWORD, s, 188) = 0x3F3504F3;
    F(DWORD, s, 184) = 0x3F3504F3;
    Spark_SideAxis(s, 192, d, 0.01f);
    float r3 = (float)((double)rand() * 3.0518509447574615e-05f);
    F(float, s, 204) = (float)(((double)rand() * 3.0518509447574615e-05f - r3) * k_spin650c88);
    F(float, s, 208) = (float)((double)rand() * k_spin650c84);
    return s;
}

// [FUN_59d790] constructs a spark at p moving with v (+160), its side axis, a random spin and colour slot
extern "C" BYTE* __fastcall Spark_Construct2(BYTE* s, void*, BYTE* view, const Vec3* p, const Vec3* v, float a3, float a4,
                                             int a5, unsigned n, int kind, float total)
{
    CallThis(FUN_59abb0, s, view, p, (float)((double)a3 + a3), (float)((double)a4 + a4), a3, a4, a5, n,
             (float)((double)total / (double)(__int64)n), total, 100);
    for (int o = 148; o <= 180; o += 4) F(int, s, o) = 0;
    F(void*, s, 0) = &g_vtbl650f34;
    F(DWORD, s, 128) |= 1;
    F(void*, s, 20) = &g_vtbl650ee8;
    F(Vec3, s, 160) = *v;
    Spark_SideAxis(s, 172, (const float*)(s + 160), 0.5f);
    F(int, s, 192) = 0;
    float r1 = (float)((double)rand() * 3.0518509447574615e-05f);
    F(float, s, 184) = (float)(((double)rand() * 3.0518509447574615e-05f - r1) * 3.141592502593994f);
    F(float, s, 188) = (float)((double)rand() * k_spin64d400);
    F(Vec3, s, 148) = *p;
    g_sparkCount6f6a6c++;
    if (kind == 0) F(int, s, 196) = 252;
    else if (kind == 1) F(int, s, 196) = 253;
    else if (kind == 2) F(int, s, 196) = 254;
    return s;
}
