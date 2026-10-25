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
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_5d3430(); KING_ALIAS("_FUN_5d3430", "@Handle_Call3790@8")
extern "C" void FUN_4e48e2(); KING_ALIAS("_FUN_4e48e2", "_Frame_ApplyChecked")
extern "C" void FUN_598bd0(); KING_ALIAS("_FUN_598bd0", "@WorldObj_SetTarget@12")
extern "C" void FUN_59a780(); KING_ALIAS("_FUN_59a780", "@MotionTexture_Construct@52")    // constructor of the effects' base class
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

extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")    // named object: constructor (id, name)
ASM_PROC(FUN_4e41d9)    // an angle checked for NaN (angle, where)
extern "C" void FUN_603cb0(); KING_ALIAS("_FUN_603cb0", "_Ini_GetDouble")    // a double of an ini file (section, key, default, out, file)
extern "C" void FUN_4e5dce(); KING_ALIAS("_FUN_4e5dce", "_Model_FindPart2")
extern "C" void FUN_4e5c77(); KING_ALIAS("_FUN_4e5c77", "_Model_FindPart1")
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
extern "C" void FUN_5ef800(); KING_ALIAS("_FUN_5ef800", "@Scene_TestInfo@16")
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

extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_5fa720(); KING_ALIAS("_FUN_5fa720", "@Sce5fa720_Destroy@4")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5ef780(); KING_ALIAS("_FUN_5ef780", "@Node_Remove@8")
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

extern "C" void FUN_5d9f60(); KING_ALIAS("_FUN_5d9f60", "@RoomWindow_Construct@24")
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

extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")    // the scene object base destructor
ASM_VAR(char, vt_650f5c, $L_650f5c)

// [FUN_59dbb0] destructor of a world object class (vtable 650f5c)
extern "C" void __fastcall WorldObj59dbb0_Destroy(void** self, int)
{
    self[0] = &vt_650f5c;
    CallThis(FUN_5d3290, self);
}

ASM_VAR(char, vt_651188, $L_651188)
ASM_VAR(char, vt_650c24, $L_650c24)
ASM_VAR(char, vt_650bd8, $L_650bd8)
ASM_VAR(char, vt_650f34, $L_650f34)
ASM_VAR(char, vt_650ee8, $L_650ee8)
ASM_VAR(char, vt_64d81c, $L_64d81c)
ASM_VAR(int, g_count6f6a6c, $L_6f6a6c)
ASM_VAR(char, g_6f68b0, $L_6f68b0)
extern "C" void FUN_5da790(); KING_ALIAS("_FUN_5da790", "@RoomWindow_Destroy@4")
extern "C" void FUN_59aa90(); KING_ALIAS("_FUN_59aa90", "@Wasm59aa90_Destroy@8")
extern "C" void $L_5973a0(); KING_ALIAS("_$L_5973a0", "@Rec5973a0_Construct@4")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_5480f0(); KING_ALIAS("_FUN_5480f0", "@Obj5480f0_Get72@8")
extern "C" void FUN_598d30(); KING_ALIAS("_FUN_598d30", "@Wasm_598d30@16")

// [FUN_595c60] destroys the scene object base
extern "C" void __fastcall WorldObj_DestroyBase(void* self, int)
{
    CallThis(FUN_5d3290, self);
}

// [FUN_5a00c0] destructor (vtable 651188), clears +6432
extern "C" void __fastcall WorldObj5a00c0_Destroy(void** self, int)
{
    self[0] = &vt_651188;
    self[1608] = 0;
    CallThis(FUN_5d3290, self);
}

// [FUN_595230] destructor of the member at +544
extern "C" void __fastcall WorldObj595230_Destroy(BYTE* self, int)
{
    *(void**)(self + 544) = &vt_64d81c;
    CallThis(FUN_5da790, self);
}

// [FUN_5a2a30] destructor (vtables 650c24, 650bd8 at +20)
extern "C" void __fastcall WorldObj5a2a30_Destroy(void** self, int)
{
    self[0] = &vt_650c24;
    self[5] = &vt_650bd8;
    CallThis(FUN_59aa90, self);
}

// [FUN_5a2ca0] destructor of the counted subclass (vtables 650f34, 650ee8)
extern "C" void __fastcall WorldObj5a2ca0_Destroy(void** self, int)
{
    self[0] = &vt_650f34;
    self[5] = &vt_650ee8;
    g_count6f6a6c--;
    self[0] = &vt_650c24;
    self[5] = &vt_650bd8;
    CallThis(FUN_59aa90, self);
}

// [FUN_594dd0] destroys the global at 6f68b0
extern "C" void __cdecl WorldObj_Destroy6f68b0()
{
    CallThis($L_5973a0, &g_6f68b0);
}

// [FUN_597570] attaches a source object (+96) and takes its name
extern "C" void __fastcall WorldObj_Attach(BYTE* self, int, BYTE* src)
{
    BYTE buf[48];
    *(BYTE**)(self + 96) = src;
    if (!src)
        return;
    int m = CallThis<int>(FUN_57de90, src + 16, buf);
    int s = CallThis<int>(FUN_5480f0, *(void**)(self + 96));
    CallThis(FUN_598d30, self, s, m);
}

ASM_VAR(BYTE*, g_faderList6f66b0, $L_6f66b0)
extern "C" void FUN_594c00(); KING_ALIAS("_FUN_594c00", "@Fader_Step@12")
extern "C" void FUN_594ba0(); KING_ALIAS("_FUN_594ba0", "@Fader_Unlink@8")
extern "C" void FUN_594bd0(); KING_ALIAS("_FUN_594bd0", "@Fader_Release@8")
extern "C" void FUN_595c70(); KING_ALIAS("_FUN_595c70", "@Wasm_595c70@8")
extern "C" void FUN_598c30(); KING_ALIAS("_FUN_598c30", "@WObj_SetMode@12")

// [FUN_594c40] steps all faders, deleting the finished ones
extern "C" void __cdecl Faders_Step(int dt)
{
    BYTE* f = g_faderList6f66b0;
    while (f) {
        if (CallThis<int>(FUN_594c00, f, dt)) {
            f = *(BYTE**)f;
            continue;
        }
        BYTE* next = *(BYTE**)f;
        CallThis(FUN_594ba0, f);
        if (f) {
            CallThis(FUN_594bd0, f);
            operator delete(f);
        }
        f = next;
    }
}

// [FUN_598bd0] attaches a target once (+104): marks the object at +112, resets the view at +124
extern "C" void __fastcall WorldObj_SetTarget(BYTE* self, int, void* t)
{
    if (*(void**)(self + 104) || !t)
        return;
    *(void**)(self + 104) = t;
    BYTE* o = *(BYTE**)(self + 112);
    if (o)
        *(DWORD*)(o + 16) |= 3;
    BYTE* v = *(BYTE**)(self + 124);
    if (!v)
        return;
    CallThis(FUN_595c70, v);
    CallThis(FUN_598c30, self, 0);
    v = *(BYTE**)(self + 124);
    CallThis(FUN_598d30, self, *(int*)(v + 316), v + 268);
}

ASM_VAR(BYTE*, g_world6d2078, $L_6d2078)
extern "C" void FUN_5ef640(); KING_ALIAS("_FUN_5ef640", "@Node_Root@8")
extern "C" void FUN_5a3fd0(); KING_ALIAS("_FUN_5a3fd0", "@Node_Matrix@12")
ASM_PROC(FUN_59a030)

// [FUN_598e70] resets a world object: scene node, position copy, defaults, initial state
extern "C" void __fastcall WorldObj_Reset(BYTE* self, int)
{
    *(void**)(self + 96) = CallThis<void*>(FUN_5ef640, F(BYTE*, g_world6d2078, 616) + 272);
    CallThis(FUN_5a3fd0, F(BYTE*, g_world6d2078, 616) + 16, self);
    F(DWORD, self, 148) = F(DWORD, self, 36);
    F(DWORD, self, 152) = F(DWORD, self, 40);
    F(int, self, 104) = 0;
    F(int, self, 112) = 0;
    F(DWORD, self, 156) = F(DWORD, self, 44);
    F(DWORD, self, 108) = 0x40270000;
    F(int, self, 116) = 0;
    void* r = CallThis<void*>(FUN_59a030, self, self, 0, 0);
    F(int, self, 120) = 0;
    F(int, self, 128) = 0;
    F(int, self, 124) = 0;
    F(int, self, 132) = 0;
    *(void**)(self + 96) = r;
    F(int, self, 160) = 1;
}

// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_595230(); KING_ALIAS("_FUN_595230", "@WorldObj595230_Destroy@8")
extern "C" void FUN_59dbb0(); KING_ALIAS("_FUN_59dbb0", "@WorldObj59dbb0_Destroy@8")
extern "C" void FUN_5a00c0(); KING_ALIAS("_FUN_5a00c0", "@WorldObj5a00c0_Destroy@8")
ASM_VAR(float, g_worobj64c330, $L_64c330)
ASM_VAR(float, g_worobj64c334, $L_64c334)
ASM_VAR(float, g_worobj64c460, $L_64c460)
ASM_VAR(float, g_worobj650b68, $L_650b68)
ASM_VAR(float, g_worobj650b80, $L_650b80)
ASM_VAR(float, g_worobj6f6a54, $L_6f6a54)
ASM_VAR(float, g_worobj6f6a58, $L_6f6a58)
ASM_VAR(float, g_worobj6f6a5c, $L_6f6a5c)
ASM_VAR(float, g_worobj6f6a60, $L_6f6a60)

static BYTE* Worobj_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_59eab0]
extern "C" int __fastcall Worobj_59eab0(BYTE* s, void*, int, int) { return 1; }
// [$L_595210]
extern "C" BYTE* __fastcall Worobj595210_DeletingDestructor(BYTE* s, void*, int flags) { return Worobj_DeleteAfter(s, flags, FUN_595230); }
// [$L_59db90]
extern "C" BYTE* __fastcall Worobj59db90_DeletingDestructor(BYTE* s, void*, int flags) { return Worobj_DeleteAfter(s, flags, FUN_59dbb0); }
// [$L_5a00a0]
extern "C" BYTE* __fastcall Worobj5a00a0_DeletingDestructor(BYTE* s, void*, int flags) { return Worobj_DeleteAfter(s, flags, FUN_5a00c0); }
// [$L_59a6c0]
extern "C" void __cdecl WorobjInit_59a6c0() { g_worobj6f6a58 = (float)((double)g_worobj650b68 * g_worobj64c460); }
// [$L_59a6f0]
extern "C" void __cdecl WorobjInit_59a6f0() { g_worobj6f6a60 = (float)((double)g_worobj650b68 * g_worobj64c334); }
// [$L_59a720]
extern "C" void __cdecl WorobjInit_59a720() { g_worobj6f6a54 = (float)((double)g_worobj650b68 * g_worobj64c330); }
// [$L_59a750]
extern "C" void __cdecl WorobjInit_59a750() { g_worobj6f6a5c = (float)((double)g_worobj650b68 * g_worobj650b80); }

extern "C" void FUN_59a9a0(); KING_ALIAS("_FUN_59a9a0", "@Effect_Draw@16")
extern "C" void FUN_59ac40(); KING_ALIAS("_FUN_59ac40", "@Effect_DrawFade@16")
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")
extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")
ASM_VAR(BYTE*, g_worobj696ca0, $L_696ca0)
ASM_VAR(AsmLabel, g_worobj6f67e0, $L_6f67e0)

// the current item (+88) of the object list at +100
static BYTE* Worobj_Current(BYTE* s) { return F(BYTE**, s, 100)[F(int, s, 88)]; }

// [$L_59e2d0]
extern "C" int __fastcall Worobj_59e2d0(BYTE* s, void*, int a, int b) { return CallThis<int>(FUN_59a9a0, s, a, b); }

// [$L_59f760] clears +88 of the current item first
extern "C" int __fastcall Worobj_59f760(BYTE* s, void*, int a, int b)
{
    F(int, Worobj_Current(s), 88) = 0;
    return CallThis<int>(FUN_59a9a0, s, a, b);
}

// [$L_59d760] clears flag 0x1000 of the current item
extern "C" int __fastcall Worobj_59d760(BYTE* s, void*, int a, int b)
{
    F(int, Worobj_Current(s), 32) &= ~0x1000;
    return CallThis<int>(FUN_59ac40, s, a, b);
}

// [$L_59dec0] sets flag 0x1000 of the current item and gives it +176
extern "C" int __fastcall Worobj_59dec0(BYTE* s, void*, int a, int b)
{
    F(int, Worobj_Current(s), 32) |= 0x1000;
    F(int, Worobj_Current(s), 40) = F(int, s, 176);
    return CallThis<int>(FUN_59ac40, s, a, b);
}

// [$L_595240]
extern "C" int __fastcall Worobj_595240(BYTE*, void*, int a)
{
    CallVirt(g_worobj696ca0, 52, a);
    return 1;
}

// [$L_595ca0] static initializer: six zero 12-byte records at 6f67d8
extern "C" void __cdecl WorobjInit_595ca0()
{
    DWORD* p = (DWORD*)g_worobj6f67e0 - 2;
    for (int i = 0; i < 18; i++)
        p[i] = 0;
}

// [$L_5a3e90] the frame of the animation: the fraction of time / period (+48) times the count (+40)
extern "C" int __fastcall Worobj_5a3e90(BYTE* s)
{
    double t = F(double, F(BYTE*, s, 32), 2968) / F(double, s, 48);
    int whole = (int)t;
    int frame = (int)((t - whole) * F(int, s, 40));
    CallC(FUN_5e1a10, F(int, s, 36), frame);
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

extern "C" void FUN_5b6700(); KING_ALIAS("_FUN_5b6700", "@Window_RootSurface@8")
extern "C" void FUN_4cca70(); KING_ALIAS("_FUN_4cca70", "_Fx_Scale75")
ASM_VAR(BYTE*, g_worobj696224, $L_696224)
ASM_VAR(float, g_worobj64c458, $L_64c458)
ASM_VAR(AsmLabel, g_worobj650f3c, $L_650f3c)

// [$L_59e8f0] the current item (+88 of +100) gets a random color (771 of the palette) while it is drawn
extern "C" void __fastcall Worobj_59e8f0(BYTE* s, void*, int a, int b)
{
    F(int, Worobj_Current(s), 88) = 0;
    F(int, Worobj_Current(s), 40) = 249;
    BYTE* p = CallThis<BYTE*>(FUN_5b6700, F(BYTE*, g_worobj696224, 12)) + 771;
    p[0] = (BYTE)(int)(CallC<double>(FUN_4cca70) * g_worobj64c458);
    p[1] = (BYTE)(int)(CallC<double>(FUN_4cca70) * g_worobj64c458);
    p[2] = (BYTE)(int)(CallC<double>(FUN_4cca70) * g_worobj64c458);
    F(int, Worobj_Current(s), 32) |= 0x1000;
    CallThis(FUN_59a9a0, s, a, b);
    F(int, Worobj_Current(s), 32) &= ~0x1000;
}

// [$L_59da00] palette entries 780.. (three colors) scaled by one random value (factors at 650f3c..650f58)
extern "C" int __fastcall Worobj_59da00(BYTE* s)
{
    float v = (float)CallC<double>(FUN_4cca70);
    const float* k = (const float*)g_worobj650f3c;  // 650f3c..650f58: k[0]..k[7]
    BYTE* p = CallThis<BYTE*>(FUN_5b6700, F(BYTE*, F(BYTE*, s, 32), 12)) + 780;
    p[0] = (BYTE)(int)((double)v * k[7]);
    BYTE g = (BYTE)(int)((double)v * k[6]);
    p[1] = g;
    p[2] = (BYTE)(int)((double)v * k[5]);
    p = CallThis<BYTE*>(FUN_5b6700, F(BYTE*, F(BYTE*, s, 32), 12)) + 783;
    p[0] = (BYTE)(int)((double)v * k[4]);
    p[1] = g;
    p[2] = (BYTE)(int)((double)v * k[3]);
    p = CallThis<BYTE*>(FUN_5b6700, F(BYTE*, F(BYTE*, s, 32), 12)) + 786;
    p[0] = (BYTE)(int)((double)v * k[2]);
    p[1] = (BYTE)(int)((double)v * k[1]);
    p[2] = (BYTE)(int)((double)v * k[0]);
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// ---------------------------------------------------------------------------
// More virtual methods.

extern "C" void FUN_5cda50(); KING_ALIAS("_FUN_5cda50", "@Shade_Assign@12")

// [$L_5a00e0] keeps only the 124-byte records (+32, count +6432) in use (+108), packed to the front, then released
extern "C" int __fastcall Worobj_Pack(BYTE* s)
{
    BYTE** list = (BYTE**)(s + 6232);
    unsigned k = 0;
    for (unsigned i = 0; i < F(unsigned, s, 6432); i++) {
        BYTE* r = s + 32 + i * 124;
        if (F(int, r, 108))
            list[k++] = r;
    }
    for (unsigned i = 0; i < k; i++) {
        BYTE* d = s + 32 + i * 124;
        BYTE* src = list[i];
        F(int, d, 0) = F(int, src, 0);
        F(int, d, 4) = F(int, src, 4);
        for (int j = 0; j < 6; j++)
            CallThis(FUN_5cda50, d + 8 + j * 16, src + 8 + j * 16);
        for (int o = 104; o <= 120; o += 4)
            F(int, d, o) = F(int, src, o);
        F(int, d, 108) = 0;
    }
    F(unsigned, s, 6432) = k;
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_5a3f50(); KING_ALIAS("_FUN_5a3f50", "@Light5a3f50_Attach@12")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5d3110(); KING_ALIAS("_FUN_5d3110", "@Object_Construct@12")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
ASM_VAR(AsmLabel, s_worobj67e8e8, $L_67e8e8)
ASM_VAR(AsmLabel, s_worobj67e8e0, $L_67e8e0)
ASM_VAR(AsmLabel, g_worobj6512c0, $L_6512c0)
ASM_VAR(AsmLabel, g_worobj6512a0, $L_6512a0)

// [FUN_5a3ee0] constructor (world, name, c): registered with the world's objects (+2992)
extern "C" BYTE* __fastcall Worobj5a3ee0_Construct(BYTE* s, void*, BYTE* w, int name, int c)
{
    CallThis(FUN_5d31e0, s, name, (const char*)s_worobj67e8e8);
    F(void*, s, 0) = g_worobj6512c0;
    CallThis(FUN_5a3f50, s, c);
    F(BYTE*, s, 36) = w;
    CallThis(FUN_5d3570, w + 2992, s);
    return s;
}
// [FUN_5a3e00] constructor (world, b, c, d, e): an animation (FUN_5ef2b0 (0, b)) registered with the world
extern "C" BYTE* __fastcall Worobj5a3e00_Construct(BYTE* s, void*, BYTE* w, int b, int c, int d, int e)
{
    CallThis(FUN_5d3110, s, (const char*)s_worobj67e8e0);
    F(int, s, 40) = c;
    F(int, s, 48) = d;
    F(void*, s, 0) = g_worobj6512a0;
    F(int, s, 52) = e;
    F(BYTE*, s, 32) = w;
    F(int, s, 36) = CallC<int>(FUN_5ef2b0, 0, b);
    CallThis(FUN_5d3570, w + 2992, s);
    return s;
}

// ---------------------------------------------------------------------------
// Constructors of the world object classes (exception frames dropped).

extern "C" void FUN_579310(); KING_ALIAS("_FUN_579310", "@Pooled_Construct@8")
extern "C" void FUN_5fa6a0(); KING_ALIAS("_FUN_5fa6a0", "@Mesh_Construct@8")
ASM_VAR(AsmLabel, g_worobjVt650c2c, $L_650c2c)
ASM_VAR(AsmLabel, g_worobjVt650c78, $L_650c78)
ASM_VAR(AsmLabel, g_worobjVt650c8c, $L_650c8c)
ASM_VAR(AsmLabel, g_worobjVt650cd8, $L_650cd8)
ASM_VAR(AsmLabel, g_worobjVt650ce0, $L_650ce0)
ASM_VAR(AsmLabel, g_worobjVt650d2c, $L_650d2c)
ASM_VAR(AsmLabel, g_worobjVt650d34, $L_650d34)
ASM_VAR(AsmLabel, g_worobjVt650d80, $L_650d80)
ASM_VAR(AsmLabel, g_worobjVt650d88, $L_650d88)
ASM_VAR(AsmLabel, g_worobjVt650dd4, $L_650dd4)
ASM_VAR(AsmLabel, g_worobjVt650ddc, $L_650ddc)
ASM_VAR(AsmLabel, g_worobjVt650e28, $L_650e28)
ASM_VAR(AsmLabel, g_worobjVt650e38, $L_650e38)
ASM_VAR(AsmLabel, g_worobjVt650e84, $L_650e84)
ASM_VAR(AsmLabel, g_worobjVt650e8c, $L_650e8c)
ASM_VAR(AsmLabel, g_worobjVt650ed8, $L_650ed8)
ASM_VAR(AsmLabel, g_worobjVt650ee8, $L_650ee8)
ASM_VAR(AsmLabel, g_worobjVt650f34, $L_650f34)
ASM_VAR(AsmLabel, g_worobjVt650f7c, $L_650f7c)
ASM_VAR(AsmLabel, g_worobjVt650fc8, $L_650fc8)
ASM_VAR(AsmLabel, g_worobjVt650fd4, $L_650fd4)
ASM_VAR(AsmLabel, g_worobjVt651020, $L_651020)
ASM_VAR(AsmLabel, g_worobjVt65102c, $L_65102c)
ASM_VAR(AsmLabel, g_worobjVt651078, $L_651078)
ASM_VAR(AsmLabel, g_worobjVt651080, $L_651080)
ASM_VAR(AsmLabel, g_worobjVt6510cc, $L_6510cc)
ASM_VAR(AsmLabel, g_worobjVt6510d4, $L_6510d4)
ASM_VAR(AsmLabel, g_worobjVt651120, $L_651120)
ASM_VAR(AsmLabel, g_worobjVt651128, $L_651128)
ASM_VAR(AsmLabel, g_worobjVt651174, $L_651174)
ASM_VAR(AsmLabel, g_worobjVt6511ac, $L_6511ac)
ASM_VAR(AsmLabel, g_worobjVt6511f8, $L_6511f8)
ASM_VAR(AsmLabel, g_worobjVt651200, $L_651200)
ASM_VAR(AsmLabel, g_worobjVt65124c, $L_65124c)

// the base (FUN_579310) and the node at +20 (FUN_5fa6a0); the fields from first to last zeroed, then the
// two vtables
static BYTE* Worobj_ConstructObject(BYTE* s, int first, int last, void* vt0, void* vt20)
{
    CallThis(FUN_579310, s);
    CallThis(FUN_5fa6a0, s + 20);
    for (int o = first; o <= last; o += 4)
        F(int, s, o) = 0;
    F(void*, s, 20) = vt20;
    F(void*, s, 0) = vt0;
    return s;
}
// [$L_5a2e90]
extern "C" BYTE* __fastcall Worobj5a2e90_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 156, g_worobjVt650fc8, g_worobjVt650f7c); }
// [$L_5a34e0]
extern "C" BYTE* __fastcall Worobj5a34e0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 156, g_worobjVt651120, g_worobjVt6510d4); }
// [$L_5a19e0]
extern "C" BYTE* __fastcall Worobj5a19e0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt651174, g_worobjVt651128); }
// [$L_5a1bc0]
extern "C" BYTE* __fastcall Worobj5a1bc0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt650d80, g_worobjVt650d34); }
// [$L_5a1e00]
extern "C" BYTE* __fastcall Worobj5a1e00_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt650c78, g_worobjVt650c2c); }
// [$L_5a1fe0]
extern "C" BYTE* __fastcall Worobj5a1fe0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt650d2c, g_worobjVt650ce0); }
// [$L_5a2410]
extern "C" BYTE* __fastcall Worobj5a2410_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt650dd4, g_worobjVt650d88); }
// [$L_5a25f0]
extern "C" BYTE* __fastcall Worobj5a25f0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt650e84, g_worobjVt650e38); }
// [$L_5a27d0]
extern "C" BYTE* __fastcall Worobj5a27d0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt650e28, g_worobjVt650ddc); }
// [$L_5a29b0]
extern "C" BYTE* __fastcall Worobj5a29b0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt650ed8, g_worobjVt650e8c); }
// [$L_5a3060]
extern "C" BYTE* __fastcall Worobj5a3060_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt651020, g_worobjVt650fd4); }
// [$L_5a32a0]
extern "C" BYTE* __fastcall Worobj5a32a0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt6510cc, g_worobjVt651080); }
// [$L_5a3710]
extern "C" BYTE* __fastcall Worobj5a3710_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 168, g_worobjVt651078, g_worobjVt65102c); }
// [$L_5a2220]
extern "C" BYTE* __fastcall Worobj5a2220_Construct(BYTE* s)
{
    Worobj_ConstructObject(s, 148, 168, g_worobjVt650cd8, g_worobjVt650c8c);
    for (int o = 192; o <= 200; o += 4)
        F(int, s, o) = 0;
    return s;
}
// [$L_5a2c10]
extern "C" BYTE* __fastcall Worobj5a2c10_Construct(BYTE* s) { return Worobj_ConstructObject(s, 148, 180, g_worobjVt650f34, g_worobjVt650ee8); }
// [$L_5a15e0]
extern "C" BYTE* __fastcall Worobj5a15e0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 156, 164, g_worobjVt65124c, g_worobjVt651200); }
// [$L_5a17b0]
extern "C" BYTE* __fastcall Worobj5a17b0_Construct(BYTE* s) { return Worobj_ConstructObject(s, 156, 164, g_worobjVt6511f8, g_worobjVt6511ac); }

// ---------------------------------------------------------------------------
// Object pools of the world object classes (exception frames dropped).

extern "C" void Worobj_VecCtor(); KING_ALIAS("_Worobj_VecCtor", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_5a2a30(); KING_ALIAS("_FUN_5a2a30", "@WorldObj5a2a30_Destroy@8")
extern "C" void FUN_5a2ca0(); KING_ALIAS("_FUN_5a2ca0", "@WorldObj5a2ca0_Destroy@8")
ASM_VAR(AsmLabel, s_worobj6724f8, $L_6724f8)
ASM_VAR(AsmLabel, s_worobj67e428, $L_67e428)
ASM_VAR(AsmLabel, s_worobj6724b0, $L_6724b0)

// a pool (+16) of n objects of size S made by ctor (dtor for unwinding), linked both ways (+8 next,
// +12 previous) and owned by the pool (+16); +4 the free head, +8 none in use. A failed allocation is
// reported with the source line.
static void Worobj_MakePool(BYTE* s, int n, int S, void* ctor, void (*dtor)(), int line)
{
    BYTE* m = (BYTE*)operator new(n * S + 4);
    BYTE* e = 0;
    if (m) {
        *(int*)m = n;
        e = m + 4;
        CallStd(Worobj_VecCtor, e, S, n, ctor, dtor);
    }
    F(BYTE*, s, 16) = e;
    if (!e)
        CallC(FUN_60aad0, 2289444627u, (const char*)s_worobj6724b0, (const char*)s_worobj67e428, line, (const char*)s_worobj6724f8);
    unsigned cnt = F(unsigned, s, 12);
    for (unsigned i = 0; i + 1 < cnt; i++)
        F(BYTE*, F(BYTE*, s, 16) + i * S, 8) = F(BYTE*, s, 16) + (i + 1) * S;
    F(int, F(BYTE*, s, 16) + F(unsigned, s, 12) * S - S, 8) = 0;
    for (unsigned i = 1; i < F(unsigned, s, 12); i++)
        F(BYTE*, F(BYTE*, s, 16) + i * S, 12) = F(BYTE*, s, 16) + (i - 1) * S;
    F(int, F(BYTE*, s, 16), 12) = 0;
    for (unsigned i = 0; i < F(unsigned, s, 12); i++)
        F(BYTE*, F(BYTE*, s, 16) + i * S, 16) = s;
    F(int, s, 8) = 0;
    F(BYTE*, s, 4) = F(BYTE*, s, 16);
    CallThis(FUN_5e48f0, s);
}
// [$L_5a14a0]
extern "C" void __fastcall Worobj5a14a0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 188, (void*)Worobj5a15e0_Construct, FUN_5a2a30, 1680); }
// [$L_5a1670]
extern "C" void __fastcall Worobj5a1670_MakePool(BYTE* s) { Worobj_MakePool(s, 1000, 188, (void*)Worobj5a17b0_Construct, FUN_5a2a30, 1724); }
// [$L_5a18a0]
extern "C" void __fastcall Worobj5a18a0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 196, (void*)Worobj5a19e0_Construct, FUN_5a2a30, 1769); }
// [$L_5a1a80]
extern "C" void __fastcall Worobj5a1a80_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 192, (void*)Worobj5a1bc0_Construct, FUN_5a2a30, 1814); }
// [$L_5a1cc0]
extern "C" void __fastcall Worobj5a1cc0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 192, (void*)Worobj5a1e00_Construct, FUN_5a2a30, 1858); }
// [$L_5a1ea0]
extern "C" void __fastcall Worobj5a1ea0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 176, (void*)Worobj5a1fe0_Construct, FUN_5a2a30, 1902); }
// [$L_5a20e0]
extern "C" void __fastcall Worobj5a20e0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 212, (void*)Worobj5a2220_Construct, FUN_5a2a30, 1945); }
// [$L_5a22d0]
extern "C" void __fastcall Worobj5a22d0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 176, (void*)Worobj5a2410_Construct, FUN_5a2a30, 1989); }
// [$L_5a24b0]
extern "C" void __fastcall Worobj5a24b0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 192, (void*)Worobj5a25f0_Construct, FUN_5a2a30, 2033); }
// [$L_5a2690]
extern "C" void __fastcall Worobj5a2690_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 192, (void*)Worobj5a27d0_Construct, FUN_5a2a30, 2078); }
// [$L_5a2870]
extern "C" void __fastcall Worobj5a2870_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 180, (void*)Worobj5a29b0_Construct, FUN_5a2a30, 2122); }
// [$L_5a2ad0]
extern "C" void __fastcall Worobj5a2ad0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 200, (void*)Worobj5a2c10_Construct, FUN_5a2ca0, 2165); }
// [$L_5a2d50]
extern "C" void __fastcall Worobj5a2d50_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 180, (void*)Worobj5a2e90_Construct, FUN_5a2a30, 2209); }
// [$L_5a2f20]
extern "C" void __fastcall Worobj5a2f20_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 204, (void*)Worobj5a3060_Construct, FUN_5a2a30, 2253); }
// [$L_5a3160]
extern "C" void __fastcall Worobj5a3160_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 196, (void*)Worobj5a32a0_Construct, FUN_5a2a30, 2296); }
// [$L_5a33a0]
extern "C" void __fastcall Worobj5a33a0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 164, (void*)Worobj5a34e0_Construct, FUN_5a2a30, 2339); }
// [$L_5a35d0]
extern "C" void __fastcall Worobj5a35d0_MakePool(BYTE* s) { Worobj_MakePool(s, 500, 212, (void*)Worobj5a3710_Construct, FUN_5a2a30, 2382); }

// ---------------------------------------------------------------------------
// Constructors, destructors and messages (exception frames dropped).

#include <string.h>
extern "C" void FUN_5a4250(); KING_ALIAS("_FUN_5a4250", "@WeatherNode_Attach@12")
ASM_VAR(AsmLabel, g_worobj7025f0, $L_7025f0)
ASM_VAR(AsmLabel, g_worobj6512e4, $L_6512e4)

// [FUN_5a4120] constructor (world, name, c, d, e): an object with two identity matrices (+60, +108)
extern "C" BYTE* __fastcall Worobj5a4120_Construct(BYTE* s, void*, BYTE* w, int name, int c, int d, int e)
{
    Worobj5a3ee0_Construct(s, 0, w, name, c);
    memcpy(s + 60, g_worobj7025f0, 48);
    memcpy(s + 108, g_worobj7025f0, 48);
    F(int, s, 176) = 0;
    F(int, s, 44) = e;
    F(int, s, 168) = 0;
    F(int, s, 160) = 0;
    F(void*, s, 0) = g_worobj6512e4;
    static const int zero[] = {180, 172, 164, 228, 224, 220, 216, 212, 248, 244, 240, 236, 232};
    for (int o : zero)
        F(int, s, o) = 0;
    F(int, s, 40) = d;
    CallThis(FUN_5a4250, s, c);
    F(int, s, 208) = 0;
    F(int, s, 48) = 0;
    F(int, s, 52) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Small methods: message filters, record copies, frees and deleting destructors.

extern "C" void FUN_5793f0(); KING_ALIAS("_FUN_5793f0", "_Pooled_Release")
extern "C" void FUN_59aa90(); KING_ALIAS("_FUN_59aa90", "@Wasm59aa90_Destroy@8")
extern "C" void FUN_5a2a30(); KING_ALIAS("_FUN_5a2a30", "@WorldObj5a2a30_Destroy@8")
// [$L_59a980] deleting destructor (pooled)
extern "C" BYTE* __fastcall Wasm59a980_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_59aa90, s);
    if (flags & 1)
        CallC(FUN_5793f0, s);
    return s;
}
// [$L_59ab90] deleting destructor (pooled)
extern "C" BYTE* __fastcall WorldObj59ab90_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5a2a30, s);
    if (flags & 1)
        CallC(FUN_5793f0, s);
    return s;
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

// [$L_5975b0] deletes the +16 and +116 objects
extern "C" void __fastcall Wasm5975b0_FreeParts(BYTE* s, void*)
{
    if (F(BYTE*, s, 16)) {
        CallVirt(F(BYTE*, s, 16), 0, 1);
        F(BYTE*, s, 16) = 0;
    }
    if (F(BYTE*, s, 116)) {
        CallVirt(F(BYTE*, s, 116), 0, 1);
        F(BYTE*, s, 116) = 0;
    }
}

// ---------------------------------------------------------------------------
// Vector deleting destructors of the world objects.

extern "C" void FUN_5a2ca0(); KING_ALIAS("_FUN_5a2ca0", "@WorldObj5a2ca0_Destroy@8")
extern "C" void WObj_VecDtor(); KING_ALIAS("_WObj_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")
// vector deleting destructor: flag 2 destroys the array (count before it), flag 1 frees it (pooled when single)
static BYTE* WObj_VecDelete(BYTE* s, unsigned flags, int size, void (*dtor)())
{
    if (flags & 2) {
        BYTE* blk = s - 4;
        CallStd(WObj_VecDtor, s, size, F(int, s, -4), dtor);
        if (flags & 1)
            operator delete(blk);
        return blk;
    }
    CallThis(dtor, s);
    if (flags & 1)
        CallC(FUN_5793f0, s);
    return s;
}
// [$L_5a1820] vector deleting destructor (188-byte objects)
extern "C" BYTE* __fastcall WObj5a1820_VecDeletingDestructor(BYTE* s, void*, unsigned flags)
{
    return WObj_VecDelete(s, flags, 188, FUN_5a2a30);
}
// [$L_5a1c40] vector deleting destructor (192-byte objects)
extern "C" BYTE* __fastcall WObj5a1c40_VecDeletingDestructor(BYTE* s, void*, unsigned flags)
{
    return WObj_VecDelete(s, flags, 192, FUN_5a2a30);
}
// [$L_5a2060] vector deleting destructor (176-byte objects)
extern "C" BYTE* __fastcall WObj5a2060_VecDeletingDestructor(BYTE* s, void*, unsigned flags)
{
    return WObj_VecDelete(s, flags, 176, FUN_5a2a30);
}
// [$L_5a2a50] vector deleting destructor (180-byte objects)
extern "C" BYTE* __fastcall WObj5a2a50_VecDeletingDestructor(BYTE* s, void*, unsigned flags)
{
    return WObj_VecDelete(s, flags, 180, FUN_5a2a30);
}
// [$L_5a2cd0] vector deleting destructor (200-byte objects)
extern "C" BYTE* __fastcall WObj5a2cd0_VecDeletingDestructor(BYTE* s, void*, unsigned flags)
{
    return WObj_VecDelete(s, flags, 200, FUN_5a2ca0);
}
// [$L_5a30e0] vector deleting destructor (204-byte objects)
extern "C" BYTE* __fastcall WObj5a30e0_VecDeletingDestructor(BYTE* s, void*, unsigned flags)
{
    return WObj_VecDelete(s, flags, 204, FUN_5a2a30);
}
// [$L_5a3320] vector deleting destructor (196-byte objects)
extern "C" BYTE* __fastcall WObj5a3320_VecDeletingDestructor(BYTE* s, void*, unsigned flags)
{
    return WObj_VecDelete(s, flags, 196, FUN_5a2a30);
}
// [$L_5a3550] vector deleting destructor (164-byte objects)
extern "C" BYTE* __fastcall WObj5a3550_VecDeletingDestructor(BYTE* s, void*, unsigned flags)
{
    return WObj_VecDelete(s, flags, 164, FUN_5a2a30);
}
// [$L_5a3790] vector deleting destructor (212-byte objects)
extern "C" BYTE* __fastcall WObj5a3790_VecDeletingDestructor(BYTE* s, void*, unsigned flags)
{
    return WObj_VecDelete(s, flags, 212, FUN_5a2a30);
}

// ---------------------------------------------------------------------------
// Light attachment by node kind.

// [FUN_5a3f50] attaches the light to the node n (+32): by the node type +16 into its +116 (2), +112 (3), +64 (4),
// +60 (5) or +72 (17); other types detach it
extern "C" void __fastcall Light5a3f50_Attach(BYTE* s, void*, BYTE* n)
{
    if (!n)
        return;
    F(BYTE*, s, 32) = n;
    switch (F(int, n, 16)) {
    case 2: F(BYTE*, n, 116) = s; return;
    case 3: F(BYTE*, n, 112) = s; return;
    case 4: F(BYTE*, n, 64) = s; return;
    case 5: F(BYTE*, n, 60) = s; return;
    case 17: F(BYTE*, n, 72) = s; return;
    }
    F(BYTE*, s, 32) = 0;
}

// ---------------------------------------------------------------------------
// Static matrices and the camera record.

extern "C" void __stdcall Array_ForEach(BYTE* p, int size, int count, void (*fn)());    // ai_world.cpp
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
ASM_VAR(AsmLabel, g_q103_7025f0, $L_7025f0)
ASM_VAR(AsmLabel, g_q103_6f66b8, $L_6f66b8)
// [$L_595cd0] static init: the 6 matrices of 6f66b8 start as the identity
extern "C" void __cdecl WObj_InitMatrices(void)
{
    BYTE* p = (BYTE*)g_q103_6f66b8;
    for (int k = 6; k; k--, p += 48) {
        Array_ForEach(p, 12, 4, FUN_4bd4d0);
        memcpy(p, (BYTE*)g_q103_7025f0, 48);
    }
}

// [$L_5973a0] constructor: cleared, the identity matrix at +24
extern "C" BYTE* __fastcall Rec5973a0_Construct(BYTE* s)
{
    memset(s, 0, 16);
    memset(s + 24, 0, 48);
    memcpy(s + 24, (BYTE*)g_q103_7025f0, 48);
    F(int, s, 100) = 0;
    F(int, s, 104) = 0;
    F(int, s, 124) = 0;
    F(int, s, 108) = 0;
    F(int, s, 16) = 0;
    F(int, s, 116) = 0;
    F(int, s, 72) = 0;
    F(int, s, 96) = 0;
    F(int, s, 128) = 0;
    F(int, s, 84) = 0;
    F(int, s, 80) = 0;
    return s;
}

// [$L_598db0] constructor: two identity matrices (+0, +48), the distance (+104) 5, the rest cleared, +160 = 1
extern "C" BYTE* __fastcall Rec598db0_Construct(BYTE* s)
{
    memset(s, 0, 48);
    memcpy(s, (BYTE*)g_q103_7025f0, 48);
    memset(s + 48, 0, 48);
    memcpy(s + 48, (BYTE*)g_q103_7025f0, 48);
    memset(s + 136, 0, 24);
    F(int, s, 96) = 0;
    memcpy(s, (BYTE*)g_q103_7025f0, 48);
    F(double, s, 104) = 5.0;
    F(double, s, 112) = 0;
    F(double, s, 120) = 0;
    F(double, s, 128) = 0;
    F(int, s, 160) = 1;
    return s;
}

// ---------------------------------------------------------------------------
// Placing a world object's model.

extern "C" void FUN_596470(); KING_ALIAS("_FUN_596470", "@TvCamera_SetMode@60")
ASM_PROC(FUN_595eb0)
extern "C" void FUN_594e00(); KING_ALIAS("_FUN_594e00", "_CarType_Point")
extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
extern "C" void FUN_5ac0c0(); KING_ALIAS("_FUN_5ac0c0", "_Matrix_RotationZ")
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
struct WObj107_V3 { DWORD v[3]; };
struct WObj107_M { DWORD v[12]; };
// [FUN_598c30] sets mode a (+100) of object s; with a model (+124): mode 0 places it at point 0 of car +96 turned
// half round, other modes go to 595eb0
extern "C" void __fastcall WObj_SetMode(BYTE* s, void*, int a)
{
    F(int, s, 100) = a;
    if (!F(BYTE*, s, 124))
        return;
    if (a) {
        CallThis(FUN_595eb0, F(BYTE*, s, 124), 0);
        return;
    }
    BYTE t1[12], t2[48], t3[48], t4[48];
    WObj107_V3 v = *CallC<WObj107_V3*>(FUN_594e00, t1, F(BYTE*, s, 96), 0);
    BYTE* r2 = CallC<BYTE*>(FUN_5abc40, t2, v);
    BYTE* r3 = CallC<BYTE*>(FUN_5ac0c0, t3, 3.14159274f, r2);
    BYTE* r4 = CallC<BYTE*>(FUN_5aba40, t4, r3, r2);
    CallThis(FUN_596470, F(BYTE*, s, 124), 0, *(WObj107_M*)r4);
}

ASM_VAR(AsmLabel, g_q156_6f6968, $L_6f6968)
// [$L_59a6a0] static init: constructs the 6f6968 camera record
extern "C" BYTE* __cdecl WObj_Init6f6968(void) { return Rec598db0_Construct((BYTE*)g_q156_6f6968); }

// ---------------------------------------------------------------------------
// Thunks.

ASM_VAR(char, g_q198_6f68b0, $L_6f68b0)
extern "C" int __fastcall q198_Dtor593650_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor593650_DeletingDestructor@12", "@Dtor593650_DeletingDestructor@12")
extern "C" int __fastcall q198_WObj5a3320_VecDeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_WObj5a3320_VecDeletingDestructor@12", "@WObj5a3320_VecDeletingDestructor@12")
extern "C" int __fastcall q198_Wasm5975b0_FreeParts(BYTE* s, void* d); KING_ALIAS("@q198_Wasm5975b0_FreeParts@8", "@Wasm5975b0_FreeParts@8")
// [$L_594db0] forwards to Dtor593650_DeletingDestructor for the object 32 bytes before
extern "C" int __fastcall Thunk594db0(BYTE* s, void* d, int a0) { return q198_Dtor593650_DeletingDestructor(s - 32, d, a0); }
// [$L_594df0] Wasm5975b0_FreeParts on the static object 6f68b0
extern "C" void __cdecl Static6f68b0_Destroy(void) { q198_Wasm5975b0_FreeParts((BYTE*)&g_q198_6f68b0, 0); }
// [$L_5a3d40] forwards to WObj5a3320_VecDeletingDestructor for the object 20 bytes before
extern "C" int __fastcall Thunk5a3d40(BYTE* s, void* d, int a0) { return q198_WObj5a3320_VecDeletingDestructor(s - 20, d, a0); }

ASM_VAR(char, g_q199_650334, $L_650334)
// [$L_59a010] nothing to do
extern "C" void __fastcall WObj59a010_Nop(BYTE*) {}
// [$L_59a020] nothing to do
extern "C" void __fastcall WObj59a020_Nop(BYTE*) {}
// [$L_59a970] the base part: vtable 650334, +4 cleared
extern "C" void __fastcall WObj59a970_Init(BYTE* s)
{
    F(void*, s, 0) = &g_q199_650334;
    F(int, s, 4) = 0;
}

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void q241_L_594df0(); KING_ALIAS("_q241_L_594df0", "_Static6f68b0_Destroy")
// [$L_594de0] registers 594df0 at exit
extern "C" int __cdecl Static594de0_AtExit(void) { return atexit((void(__cdecl*)(void))q241_L_594df0); }

// === the world object handlers (FUN_5a37f0)

ASM_VAR(BYTE*, g_wh6f4bd8, $L_6f4bd8)
ASM_VAR(AsmLabel, g_whSlots6f6a7c, $L_6f6a7c)    // 17 handlers
extern "C" void FUN_579570(); KING_ALIAS("_FUN_579570", "@FrameTask_Construct@16")
extern "C" void FUN_579600(); KING_ALIAS("_FUN_579600", "@Mmt_Push100@12")
extern "C" void FUN_5a1a60(); KING_ALIAS("_FUN_5a1a60", "@WasmObj5a1a60_Construct@8")
extern "C" void FUN_5a1ca0(); KING_ALIAS("_FUN_5a1ca0", "@WasmObj5a1ca0_Construct@8")
extern "C" void FUN_5a1e80(); KING_ALIAS("_FUN_5a1e80", "@WasmObj5a1e80_Construct@8")
extern "C" void FUN_5a20c0(); KING_ALIAS("_FUN_5a20c0", "@WasmObj5a20c0_Construct@8")
extern "C" void FUN_5a22b0(); KING_ALIAS("_FUN_5a22b0", "@WasmObj5a22b0_Construct@8")
extern "C" void FUN_5a2490(); KING_ALIAS("_FUN_5a2490", "@WasmObj5a2490_Construct@8")
extern "C" void FUN_5a2670(); KING_ALIAS("_FUN_5a2670", "@WasmObj5a2670_Construct@8")
extern "C" void FUN_5a2850(); KING_ALIAS("_FUN_5a2850", "@WasmObj5a2850_Construct@8")
extern "C" void FUN_5a2ab0(); KING_ALIAS("_FUN_5a2ab0", "@WasmObj5a2ab0_Construct@8")
extern "C" void FUN_5a2d30(); KING_ALIAS("_FUN_5a2d30", "@WasmObj5a2d30_Construct@8")
extern "C" void FUN_5a2f00(); KING_ALIAS("_FUN_5a2f00", "@WasmObj5a2f00_Construct@8")
extern "C" void FUN_5a3140(); KING_ALIAS("_FUN_5a3140", "@WasmObj5a3140_Construct@8")
extern "C" void FUN_5a3380(); KING_ALIAS("_FUN_5a3380", "@WasmObj5a3380_Construct@8")
extern "C" void FUN_5a35b0(); KING_ALIAS("_FUN_5a35b0", "@WasmObj5a35b0_Construct@8")
extern "C" void FUN_5a1880(); KING_ALIAS("_FUN_5a1880", "@WasmObj5a1880_Construct@8")
extern "C" void FUN_5a1650(); KING_ALIAS("_FUN_5a1650", "@WasmObj5a1650_Construct@8")
extern "C" void FUN_5a1480(); KING_ALIAS("_FUN_5a1480", "@WasmObj5a1480_Construct@8")
static void (*const k_whCtors[17])() = {FUN_5a1a60, FUN_5a1ca0, FUN_5a1e80, FUN_5a20c0, FUN_5a22b0, FUN_5a2490, FUN_5a2670, FUN_5a2850, FUN_5a2ab0, FUN_5a2d30, FUN_5a2f00, FUN_5a3140, FUN_5a3380, FUN_5a35b0, FUN_5a1880, FUN_5a1650, FUN_5a1480, };

extern "C" {

// [FUN_5a37f0] creates the frame task (arg, 15) and the 17 world object handlers (6f6a7c..), initializes them
// (vtbl+0) and adds them to the task, 6f6ab4 first
void __cdecl WorldHandlers_Create(int arg)
{
    BYTE* t = (BYTE*)operator new(436);
    t = t ? CallThis<BYTE*>(FUN_579570, t, arg, 15) : 0;
    g_wh6f4bd8 = t;
    CallVirt(t, 24, 1);
    BYTE** slot = (BYTE**)(char*)g_whSlots6f6a7c;
    for (int i = 0; i < 17; i++) {
        BYTE* p = (BYTE*)operator new(20);
        slot[i] = p ? CallThis<BYTE*>(k_whCtors[i], p) : 0;
    }
    for (int i = 0; i < 17; i++)
        CallVirt(slot[i], 0);
    CallThis(FUN_579600, g_wh6f4bd8, slot[14]);
    for (int i = 0; i < 17; i++)
        if (i != 14)
            CallThis(FUN_579600, g_wh6f4bd8, slot[i]);
}

} // extern "C"

// === placing the scene (FUN_5975e0)

ASM_VAR(AsmLabel, s_ps67e334, $L_67e334)
ASM_VAR(AsmLabel, s_ps67e324, $L_67e324)
ASM_VAR(AsmLabel, s_ps67e314, $L_67e314)
ASM_VAR(AsmLabel, s_ps67e304, $L_67e304)
ASM_VAR(AsmLabel, s_ps67e2f4, $L_67e2f4)
ASM_VAR(AsmLabel, s_ps67e2e4, $L_67e2e4)
ASM_VAR(AsmLabel, s_ps67e2d4, $L_67e2d4)
ASM_VAR(AsmLabel, s_ps67e2c4, $L_67e2c4)
ASM_VAR(AsmLabel, s_ps67e2b4, $L_67e2b4)
ASM_VAR(AsmLabel, s_ps67e2a4, $L_67e2a4)
ASM_VAR(AsmLabel, s_ps67e294, $L_67e294)
ASM_VAR(AsmLabel, s_ps67e284, $L_67e284)
ASM_VAR(AsmLabel, s_ps67e274, $L_67e274)
ASM_VAR(AsmLabel, s_ps67e264, $L_67e264)
ASM_VAR(AsmLabel, s_ps67e254, $L_67e254)
ASM_VAR(AsmLabel, s_ps67e244, $L_67e244)
ASM_VAR(AsmLabel, s_ps67e234, $L_67e234)
ASM_VAR(AsmLabel, s_ps67e224, $L_67e224)
ASM_VAR(AsmLabel, s_ps67e214, $L_67e214)
ASM_VAR(AsmLabel, s_ps67e204, $L_67e204)
ASM_VAR(AsmLabel, s_ps67e1f4, $L_67e1f4)
ASM_VAR(AsmLabel, s_ps67e1e4, $L_67e1e4)
ASM_VAR(AsmLabel, s_ps67e1d4, $L_67e1d4)
ASM_VAR(AsmLabel, s_ps67e1c4, $L_67e1c4)
ASM_VAR(int, g_ps6f6a3c, $L_6f6a3c)
ASM_VAR(int, g_ps6f6a40, $L_6f6a40)
ASM_VAR(int, g_ps6f6a1c, $L_6f6a1c)
ASM_VAR(int, g_ps6f6a14, $L_6f6a14)
ASM_VAR(int, g_ps6f694c, $L_6f694c)
ASM_VAR(int, g_ps6f6940, $L_6f6940)
ASM_VAR(int, g_ps6f6a20, $L_6f6a20)
ASM_VAR(int, g_ps6f6a10, $L_6f6a10)
ASM_VAR(int, g_ps6f6950, $L_6f6950)
ASM_VAR(int, g_ps6f693c, $L_6f693c)
ASM_VAR(int, g_ps6f6a24, $L_6f6a24)
ASM_VAR(int, g_ps6f6a18, $L_6f6a18)
ASM_VAR(int, g_ps6f6954, $L_6f6954)
ASM_VAR(int, g_ps6f6944, $L_6f6944)
ASM_VAR(AsmLabel, s_ps67e1b8, $L_67e1b8)
ASM_VAR(BYTE*, g_ps6cec70, $L_6cec70)
ASM_VAR(BYTE*, g_ps6f68fc, $L_6f68fc)
extern "C" void FUN_4e48e2(); KING_ALIAS("_FUN_4e48e2", "_Frame_ApplyChecked")
extern "C" void FUN_568fd0(); KING_ALIAS("_FUN_568fd0", "_Game_568fd0")
extern "C" void FUN_5d9ef0(); KING_ALIAS("_FUN_5d9ef0", "@Scenes_SetFirst@12")
extern "C" void FUN_5d9f10(); KING_ALIAS("_FUN_5d9f10", "@Scene_FindSpecial@8")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")

static const char* const s_psNames[24] = {
    (const char*)s_ps67e334,
    (const char*)s_ps67e324,
    (const char*)s_ps67e314,
    (const char*)s_ps67e304,
    (const char*)s_ps67e2f4,
    (const char*)s_ps67e2e4,
    (const char*)s_ps67e2d4,
    (const char*)s_ps67e2c4,
    (const char*)s_ps67e2b4,
    (const char*)s_ps67e2a4,
    (const char*)s_ps67e294,
    (const char*)s_ps67e284,
    (const char*)s_ps67e274,
    (const char*)s_ps67e264,
    (const char*)s_ps67e254,
    (const char*)s_ps67e244,
    (const char*)s_ps67e234,
    (const char*)s_ps67e224,
    (const char*)s_ps67e214,
    (const char*)s_ps67e204,
    (const char*)s_ps67e1f4,
    (const char*)s_ps67e1e4,
    (const char*)s_ps67e1d4,
    (const char*)s_ps67e1c4};
static int* const g_psVars[14] = {&g_ps6f6a3c, &g_ps6f6a40, &g_ps6f6a1c, &g_ps6f6a14, &g_ps6f694c, &g_ps6f6940, &g_ps6f6a20, &g_ps6f6a10, &g_ps6f6950, &g_ps6f693c, &g_ps6f6a24, &g_ps6f6a18, &g_ps6f6954, &g_ps6f6944};
static const int s_psValues[4][14] = {
    {468, 50, 7, 6, 156, 50, 5, 7, 51, 153, 4, 3, 25, 72},
    {572, 50, 8, 8, 210, 60, 7, 9, 63, 190, 6, 4, 30, 90},
    {680, 50, 10, 10, 280, 80, 9, 12, 81, 243, 8, 6, 38, 114},
    {1117, 50, 10, 10, 460, 150, 14, 19, 126, 379, 14, 9, 60, 182}};

extern "C" {

// [FUN_5975e0] places scene object s (thiscall: id, matrix m, unused): its frame +20 at m, the camera +120 reset,
// the scene +112 started from +92's +468; for detail levels 16..64 (6cec70 +408) the three marker objects
// (+80..+88, another set in game mode 2) and the level's draw limits; 6f68fc the 67e1b8 object
void __fastcall SceneObj_Place(BYTE* s, void*, int id, const BYTE* m, int)
{
    F(int, s, 20) = F(int, s, 16);
    F(int, s, 112) = F(int, s, 108);
    BYTE* cam = F(BYTE*, s, 116);
    F(BYTE*, s, 120) = cam;
    F(int, s, 104) = 0;
    F(int, s, 72) = id;
    memcpy(s + 24, m, 48);
    F(int, cam, 76) = id;
    F(float, F(BYTE*, s, 120), 88) = 1.0f;
    F(float, F(BYTE*, s, 120), 84) = 1.0f;
    CallC(FUN_4e48e2, F(BYTE*, s, 20), 0, s + 24);
    CallThis(FUN_5d9ef0, F(BYTE*, s, 112), F(int, F(BYTE*, s, 92), 468));
    CallThis(FUN_5d9f10, F(BYTE*, s, 112));
    int level = F(int, g_ps6cec70, 408) & 240;
    if (level >= 16 && level <= 64 && !(level & 15)) {
        int k = level / 16 - 1;
        const char* const* n = s_psNames + k * 6 + (CallC<int>(FUN_568fd0) == 2 ? 0 : 3);
        F(BYTE*, s, 80) = CallC<BYTE*>(FUN_5ef3d0, n[0]);
        F(BYTE*, s, 84) = CallC<BYTE*>(FUN_5ef3d0, n[1]);
        F(BYTE*, s, 88) = CallC<BYTE*>(FUN_5ef3d0, n[2]);
        for (int i = 0; i < 14; i++)
            *g_psVars[i] = s_psValues[k][i];
    }
    g_ps6f68fc = CallC<BYTE*>(FUN_5ef2b0, 0, (const char*)s_ps67e1b8);
}

} // extern "C"

// === a TV camera's mode (FUN_596470)

ASM_VAR(BYTE*, g_tc6f6910, $L_6f6910)
ASM_VAR(float, g_tc702630, $L_702630)
ASM_VAR(float, g_tc702634, $L_702634)
ASM_VAR(float, g_tc702638, $L_702638)
ASM_VAR(float, g_tc64c408, $L_64c408)
ASM_VAR(double, g_tc64c958, $L_64c958)
ASM_VAR(double, g_tc64b660, $L_64b660)
extern "C" void FUN_594e70(); KING_ALIAS("_FUN_594e70", "_Object_TraceRay")
extern "C" void FUN_5969e0(); KING_ALIAS("_FUN_5969e0", "@TvCamera_Circle@12")

// mode 3 / 4: the camera at the offset 702630 (scaled by 64c408) in the target's frame, its spin cleared
static void Tc_Offset(BYTE* cam)
{
    WObj107_V3 v;
    float o[3] = {(float)(g_tc702630 * (double)g_tc64c408), (float)(g_tc702634 * (double)g_tc64c408),
                  (float)(g_tc702638 * (double)g_tc64c408)};
    memcpy(&v, o, 12);
    BYTE t[48], r[48];
    BYTE* m = CallC<BYTE*>(FUN_5aba40, r, CallC<BYTE*>(FUN_5abc40, t, v), cam + 80);
    memcpy(cam + 32, m, 48);
    memcpy(cam + 128, m, 48);
    F(int, cam, 248) = 0;
    F(int, cam, 240) = 0;
    F(int, cam, 252) = 0;
    F(int, cam, 244) = 0;
    memcpy(cam + 256, cam + 116, 12);
}

extern "C" {

// [FUN_596470] sets TV camera cam to mode (thiscall, with matrix m by value) at the current target car
// (6f6910, its matrix into +80): 0 m in the car's frame, 1 at the car's camera point, 2 circling (+268), 3 / 4
// at a fixed offset, 5 m itself; then the ray from the camera (+116) to the car finds the visible object,
// and the scene effect starts with the camera matrix (+32)
void __fastcall TvCamera_SetMode(BYTE* cam, void*, int mode, WObj107_M m)
{
    if (!g_tc6f6910)
        return;
    F(int, cam, 328) = mode;
    BYTE* root = CallThis<BYTE*>(FUN_5ef640, g_tc6f6910 + 272);
    float hit[3] = {0, 0, 0};
    CallThis(FUN_5a3fd0, g_tc6f6910 + 16, cam + 80);
    BYTE t[48], r[48];
    switch (mode) {
    case 0:
        memcpy(cam + 128, &m, 48);
        memcpy(cam + 32, CallC<BYTE*>(FUN_5aba40, r, cam + 128, cam + 80), 48);
        break;
    case 1: {
        memcpy(cam + 128, &m, 48);
        BYTE pt[12];
        const float* p = CallC<const float*>(FUN_594e00, pt, g_tc6f6910, -1);
        float o[3] = {(float)(p[0] - g_tc64c958), (float)(p[1] - g_tc64b660), (float)(p[2] + g_tc64b660)};
        WObj107_V3 v;
        memcpy(&v, o, 12);
        memcpy(cam + 176, CallC<BYTE*>(FUN_5abc40, t, v), 48);
        memcpy(cam + 32, CallC<BYTE*>(FUN_5aba40, r, cam + 176, cam + 80), 48);
        break;
    }
    case 2: {
        BYTE* c = CallThis<BYTE*>(FUN_5969e0, cam, r);
        memcpy(cam + 268, c, 48);
        memcpy(cam + 32, c, 48);
        break;
    }
    case 3:
    case 4:
        Tc_Offset(cam);
        break;
    case 5:
        memcpy(cam + 128, &m, 48);
        memcpy(cam + 32, &m, 48);
        break;
    }
    memcpy(hit, cam + 68, 12);
    WObj107_V3 from;
    memcpy(&from, cam + 116, 12);
    BYTE* o = CallC<BYTE*>(FUN_594e70, 4, g_tc6f6910 ? g_tc6f6910 + 16 : (BYTE*)0, root, from, hit);
    memcpy(cam + 68, hit, 12);
    CallThis(FUN_596850, cam, o, cam + 32);
}

} // extern "C"

// === a TV camera's update ($L_596f80)

ASM_VAR(AsmLabel, g_tc7025f0_tu, $L_7025f0)
ASM_VAR(BYTE*, g_tu6f690c, $L_6f690c)
ASM_VAR(int, g_tu6f6914, $L_6f6914)
ASM_VAR(float, g_tu64c450, $L_64c450)
ASM_VAR(float, g_tu64d4e8, $L_64d4e8)
ASM_VAR(float, g_tu64d350, $L_64d350)
ASM_VAR(AsmLabel, g_tu6f68b0, $L_6f68b0)
ASM_PROC(FUN_595d20)
extern "C" void FUN_5968c0(); KING_ALIAS("_FUN_5968c0", "@TvCamera_Follow@12")
ASM_PROC(FUN_596b30)
extern "C" void FUN_596e50(); KING_ALIAS("_FUN_596e50", "@Object_AimMatrix@16")
extern "C" void FUN_598d30(); KING_ALIAS("_FUN_598d30", "@Wasm_598d30@16")
ASM_PROC(FUN_599210)
extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")

extern "C" {

// [$L_596f80] updates TV camera cam (thiscall) unless stopped (+320): without a target car (6f6910) it posts its
// event; else the car's matrix (+80) and root (+316), the shot time +228 grows, the mode (+328) makes the
// camera matrix (5 aimed from +128, 2 circling, 0 in the car's frame, 1 following, 3 / 4 chasing; past their
// time with 6f6914 set +324 asks for the next shot), the visible object is traced, and the matrix (+268, +32)
// goes to the scene 6f68b0
void __fastcall TvCamera_Update(BYTE* cam)
{
    if (F(int, cam, 320))
        return;
    if (!g_tc6f6910) {
        CallThis(FUN_5d32f0, cam, 1);
        return;
    }
    CallThis(FUN_5a3fd0, g_tc6f6910 + 16, cam + 80);
    if (F(int, cam, 328) != 5)
        F(BYTE*, cam, 316) = CallThis<BYTE*>(FUN_5ef640, g_tc6f6910 + 272);
    CallThis(FUN_595d20, cam);
    F(float, cam, 228) = (float)(F(float, cam, 228) + F(double, g_tu6f690c, 2960));
    BYTE m[48], t[48];
    memcpy(m, (BYTE*)g_tc7025f0_tu, 48);
    int mode = F(int, cam, 328);
    float limit = 0;
    bool timed = false;
    switch (mode) {
    case 5:
        memcpy(m, cam + 128, 48);
        memcpy(m, CallThis<BYTE*>(FUN_596e50, cam, t, m), 48);
        limit = g_tu64c450;
        timed = true;
        break;
    case 2:
        memcpy(m, CallThis<BYTE*>(FUN_5969e0, cam, t), 48);
        limit = g_tu64d4e8;
        timed = true;
        break;
    case 0:
        memcpy(m, CallC<BYTE*>(FUN_5aba40, t, cam + 128, cam + 80), 48);
        limit = g_tu64d350;
        timed = true;
        break;
    case 1:
        memcpy(m, CallThis<BYTE*>(FUN_5968c0, cam, t), 48);
        break;
    case 3:
    case 4:
        memcpy(m, CallThis<BYTE*>(FUN_596b30, cam, t), 48);
        limit = g_tu64c450;
        timed = true;
        break;
    }
    if (timed && F(float, cam, 228) > limit && g_tu6f6914 == 1)
        F(int, cam, 324) = 1;
    mode = F(int, cam, 328);
    if (mode >= 1 && mode <= 4) {
        BYTE* node = g_tc6f6910 ? g_tc6f6910 + 16 : 0;
        BYTE pt[12];
        const float* p = CallC<const float*>(FUN_594e00, pt, g_tc6f6910, 1);
        float v[3] = {F(float, cam, 116) + p[0], F(float, cam, 120) + p[1], F(float, cam, 124) + p[2]};
        F(BYTE*, cam, 316) = CallC<BYTE*>(FUN_599210, node, F(BYTE*, cam, 316), cam + 80, v, m + 36, 0);
    } else if (mode == 0) {
        BYTE* node = g_tc6f6910 ? g_tc6f6910 + 16 : 0;
        WObj107_V3 from;
        memcpy(&from, cam + 116, 12);
        F(BYTE*, cam, 316) = CallC<BYTE*>(FUN_594e70, 4, node, F(BYTE*, cam, 316), from, (float*)(m + 36));
    }
    mode = F(int, cam, 328);
    if (mode == 2 || mode == 3 || mode == 4)
        memcpy(cam + 268, CallThis<BYTE*>(FUN_596e50, cam, t, m), 48);
    else
        memcpy(cam + 268, m, 48);
    memcpy(cam + 32, cam + 268, 48);
    CallThis(FUN_5d32f0, cam, 1);
    CallThis(FUN_598d30, (BYTE*)g_tu6f68b0, F(BYTE*, cam, 316), cam + 268);
}

} // extern "C"

// === the motion texture object (FUN_59a780)

extern "C" void FUN_5fa4f0(); KING_ALIAS("_FUN_5fa4f0", "@Node_Set48@16")
extern "C" void FUN_5fa5f0(); KING_ALIAS("_FUN_5fa5f0", "@Chain_MoveTo@8")
extern "C" void FUN_5fa6f0(); KING_ALIAS("_FUN_5fa6f0", "@Mesh_ConstructNamed@12")
extern "C" void Wo_VecCtorEh(); KING_ALIAS("_Wo_VecCtorEh", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" void __fastcall FxRec_Init(BYTE* s);
extern "C" void __fastcall FxRec_Free(BYTE* s);
ASM_VAR(float, g_mt64c878, $L_64c878)

// a new 16-byte record of four floats
static float* Mt_Quad(float a, float b, float c, float d)
{
    float* q = (float*)operator new(16);
    q[0] = a;
    q[1] = b;
    q[2] = c;
    q[3] = d;
    return q;
}

extern "C" {

// [FUN_59a780] constructor (a, b, x, y, z, w, h, owner, t, c, d) of a moving texture (vtables 650bd0, 650b84 at
// +20, the mesh motionTexture): one 40-byte face of two triangles and four corners with the texture coordinates
// scaled by w and h, owned by owner's (+0) material, linked into b's list
BYTE* __fastcall MotionTexture_Construct(BYTE* s, void*, int a, BYTE* b, DWORD x, DWORD y, DWORD z, float w,
                                         float h, DWORD* owner, int t, int c, int d)
{
    CallThis(FUN_579310, s);
    BYTE* m = s + 20;
    CallThis(FUN_5fa6f0, m, "motionTexture");
    F(int, s, 88) = a;
    F(int, s, 96) = c;
    F(BYTE*, s, 92) = b;
    F(int, s, 100) = d;
    F(DWORD*, s, 120) = owner;
    F(DWORD, s, 76) = x;
    F(void*, s, 0) = &g_vtbl650bd0;
    F(DWORD, s, 80) = y;
    F(void*, m, 0) = &g_vtbl650b84;
    F(int, s, 128) = 0;
    F(DWORD, s, 84) = z;
    BYTE* f = 0;
    if (BYTE* arr = (BYTE*)operator new(44)) {
        F(int, arr, 0) = 1;
        f = arr + 4;
        CallStd(Wo_VecCtorEh, f, 40, 1, (void*)FxRec_Init, (void*)FxRec_Free);
    }
    F(WORD, f, 0) = 2;
    F(WORD, f, 4) = 4;
    F(float*, f, 8) = Mt_Quad(0, 1.0f, 1.0f, 0);
    F(float*, f, 12) = Mt_Quad(1.0f, 1.0f, 0, 0);
    float uw = (float)(w * g_mt64c878), uh = (float)(w * g_64c258);
    F(float*, f, 16) = Mt_Quad(uw, uh, uh, uw);
    F(int, f, 24) = 0;
    float vw = (float)(h * g_mt64c878), vh = (float)(h * g_64c258);
    F(float*, f, 20) = Mt_Quad(vw, vw, vh, vh);
    F(WORD, f, 34) = 4095;
    F(DWORD, f, 36) = *owner;
    F(BYTE, f, 1) |= 1;
    CallThis(FUN_5fa4f0, m, f, 1);
    CallThis(FUN_5fa5f0, m);
    CallVirt(b, 20, 0, s ? m : 0);
    F(int, s, 104) = 0;
    F(BYTE, s, 60) &= ~1;
    F(int, s, 116) = 0;
    F(int, s, 108) = 0;
    F(int, s, 112) = t;
    F(int, s, 124) = 0;
    return s;
}

} // extern "C"

// === the sky tinted draw (59c830)

extern "C" void FUN_4be5e0(); KING_ALIAS("_FUN_4be5e0", "_Color_ToHsl")
extern "C" void FUN_4be7f0(); KING_ALIAS("_FUN_4be7f0", "_Color_FromHsl")
ASM_VAR(float, g_st64c488, $L_64c488)
ASM_VAR(float, g_st64d4c0, $L_64d4c0)
ASM_VAR(float, g_st650e34, $L_650e34)
ASM_VAR(float, g_st650e30, $L_650e30)
ASM_VAR(float, g_st64c304, $L_64c304)
ASM_VAR(float, g_st64c264, $L_64c264)

// the colour of hue h, saturation sat and lightness l into the three bytes at d
static void St_Hsl(BYTE* d, float h, float sat, float l)
{
    float r, g, b;
    CallC(FUN_4be7f0, h, sat, l, &r, &g, &b);
    d[0] = (BYTE)(int)((double)r * g_worobj64c458);
    d[1] = (BYTE)(int)((double)g * g_worobj64c458);
    d[2] = (BYTE)(int)((double)b * g_worobj64c458);
}

extern "C" {

// [$L_59c830] draws effect s with its current frame (+100[+88]) lit: the screen's colour (root surface +48..+50)
// at half its lightness and a little more saturation becomes the colours at +777 and +759, then three more of
// rising saturation at +762, +765, +768
int __fastcall Sky_DrawTinted(BYTE* s, void*, int a, int b)
{
    BYTE* cur = F(BYTE**, s, 100)[F(int, s, 88)];
    F(float, cur, 88) = 0.76f;
    F(int, F(BYTE**, s, 100)[F(int, s, 88)], 40) = 251;
    BYTE* w = CallThis<BYTE*>(FUN_5b6700, F(BYTE*, g_696224, 12));
    float r = (float)(F(BYTE, w, 48) * (double)g_st64c488);
    float g = (float)(F(BYTE, w, 49) * (double)g_st64c488);
    float bl = (float)(F(BYTE, w, 50) * (double)g_st64c488);
    float h, sat, l;
    CallC(FUN_4be5e0, r, g, bl, &h, &sat, &l);
    l = (float)(l * (double)g_64c258);
    sat = (float)((double)sat + g_st64d4c0);
    if (sat > g_effectOne)
        sat = 1.0f;
    else if (sat < g_st64c264)
        sat = 0;
    St_Hsl(w + 777, h, sat, l);
    St_Hsl(w + 759, h, sat, l);
    float v = (float)(sat * (double)g_st650e34);
    if (v > g_effectOne)
        v = 1.0f;
    St_Hsl(w + 762, h, v, l);
    v = (float)(sat * (double)g_st650e30);
    if (v > g_effectOne)
        v = 1.0f;
    St_Hsl(w + 765, h, v, l);
    v = (float)((double)sat - sat * (double)g_st64c304);
    if (v > g_effectOne)
        v = 1.0f;
    St_Hsl(w + 768, h, v, l);
    F(DWORD, F(BYTE**, s, 100)[F(int, s, 88)], 32) |= 0x1000;
    int res = CallThis<int>(FUN_59a9a0, s, a, b);
    F(DWORD, F(BYTE**, s, 100)[F(int, s, 88)], 32) &= ~0x1000u;
    return res;
}

} // extern "C"

// === the dust puff (FUN_5a0ca0)

ASM_VAR(AsmLabel, g_vtbl65124c, $L_65124c)
ASM_VAR(AsmLabel, g_vtbl651200, $L_651200)

extern "C" {

// [FUN_5a0ca0] constructor (p, size, second, kind, near) of a dust puff of the effect pool: at p (+156), pulled
// towards the camera (+76) by 64c304 (near 0), 64c260 (near 1) or 64c338 (near 2), fading by +168/+172
BYTE* __fastcall DustPuff_Construct(BYTE* s, void*, const float* p, int size, int second, int kind, int near_)
{
    Effect_Construct(s, 0, (int)g_696224, (const Vec3Bits*)p, 0x3f800000, 0x3f800000, kind, F(DWORD, s, 112),
                     0x40c00000, 0x40c00000, 100);
    float* q = (float*)(s + 156);
    F(void*, s, 0) = (BYTE*)g_vtbl65124c;
    F(void*, s, 20) = (BYTE*)g_vtbl651200;
    F(int, s, 148) = second;
    memcpy(q, p, 12);
    F(int, s, 176) = size;
    F(int, s, 152) = near_;
    if (!second) {
        F(float, s, 172) = 0;
        F(float, s, 168) = 1.0f;
    } else {
        F(float, s, 172) = 0.5f;
        F(float, s, 168) = 0.1f;
    }
    F(float, s, 184) = 0.71f;
    F(float, s, 180) = 0.71f;
    BYTE* cam = g_696224;
    double cx = F(double, cam, 2904);
    float cy = (float)F(double, cam, 2912), cz = (float)F(double, cam, 2920);
    float* o = (float*)(s + 76);
    if (near_ == 2) {
        Vec3 d, nb, v, w;
        d.Set((float)(cx - q[0]), (float)((double)cy - q[1]), (float)((double)cz - q[2]));
        const Vec3* n = Vec3_Normalized(&nb, &d);
        v.Set((float)(n->x * (double)g_64c338), (float)(n->y * (double)g_64c338), (float)(n->z * (double)g_64c338));
        w.Set((float)((double)v.x + q[0]), (float)((double)v.y + q[1]), (float)((double)v.z + q[2]));
        memcpy(o, &w, 12);
    } else if (near_ == 1 || near_ == 0) {
        double k = near_ ? g_effectFadeBase : g_st64c304;
        Vec3 d, nb;
        d.x = (float)(cx - q[0]);
        d.y = (float)((double)cy - q[1]);
        d.z = (float)((double)cz - q[2]);
        const Vec3* n = Vec3_Normalized(&nb, &d);
        double x = n->x * k;
        float y = (float)(n->y * k), z = (float)(n->z * k);
        o[0] = (float)(x + q[0]);
        o[1] = (float)((double)y + q[1]);
        o[2] = (float)((double)z + q[2]);
    }
    return s;
}

} // extern "C"

// === the drifting swirl (59cba0)

extern "C" void FUN_59ac90(); KING_ALIAS("_FUN_59ac90", "@Effect_AnimateQuad@8")
extern "C" void Ws_Normalized(); KING_ALIAS("_Ws_Normalized", "_Vec3_Normalized")
extern "C" void Ws_AddAssign(); KING_ALIAS("_Ws_AddAssign", "@Vec3_AddAssign@16")
ASM_VAR(AsmLabel, g_ws702650, $L_702650)

// the texture coordinates of corner k of quad s (+72: u at +8, v at +12) on the circle at angle a
static void Ws_Corner(BYTE* s, int k, double a)
{
    F(float*, F(BYTE*, s, 72), 8)[k] = (float)(x87_cos(a) * F(float, s, 184) + g_64c258);
}

extern "C" {

// [$L_59cba0] step of a drifting swirl s (unless plain, +124): its speed (+160) slowed by +172 until it turns, its
// position (+76, +40) moved by it, then its quad animated and its texture turned (+176) by +180 times the square of
// the life left (+100, +104), the corners on a circle of radii +184/+188 at the angles 6f6a58..
int __fastcall Swirl_Step(BYTE* s)
{
    if (!F(int, s, 124))
        return CallThis<int>(FUN_59ac90, s);
    float* v = (float*)(s + 160);
    if (F(float, s, 172) != 0) {
        float nb[3];
        const float* n = CallC<const float*>(Ws_Normalized, nb, v);
        double k = F(float, s, 172);
        double ax = -n[0] * k;
        float ay = (float)(-n[1] * k), az = (float)(-n[2] * k);
        float dt = (float)F(double, F(BYTE*, s, 88), 2960);
        float a[3] = {(float)(ax * dt), (float)(ay * (double)dt), (float)(az * (double)dt)};
        float t[3];
        CallThis(Ws_AddAssign, v, t, a);
        if ((double)v[2] * a[2] + (double)v[1] * a[1] + (double)v[0] * a[0] > g_effectZero) {
            F(int, s, 172) = 0;
            memcpy(v, (BYTE*)g_ws702650, 12);
        }
    }
    double dt = F(double, F(BYTE*, s, 88), 2960);
    float tx = (float)(dt * v[0]), ty = (float)(dt * v[1]);
    double tz = dt * v[2];
    F(float, s, 76) = (float)((double)tx + F(float, s, 76));
    F(float, s, 80) = (float)((double)ty + F(float, s, 80));
    F(float, s, 84) = (float)(tz + F(float, s, 84));
    memcpy(s + 40, s + 76, 12);
    CallThis(FUN_59ac90, s);
    double f = ((double)F(float, s, 100) - F(float, s, 104)) / F(float, s, 100);
    if (f < g_effectZero)
        f = g_effectZero;
    else if (f > g_effectDoubleOne)
        f = g_effectDoubleOne;
    double ang = F(float, s, 180) * f * f * F(double, F(BYTE*, s, 88), 2960) + F(float, s, 176);
    F(float, s, 176) = (float)ang;
    BYTE* q = F(BYTE*, s, 72);
    F(float*, q, 8)[0] = (float)(x87_cos(ang + g_worobj6f6a58) * F(float, s, 184) + g_64c258);
    const float base[4] = {g_worobj6f6a58, g_worobj6f6a5c, g_worobj6f6a60, g_worobj6f6a54};
    for (int k = 0; k < 4; k++) {
        if (k)
            Ws_Corner(s, k, (double)base[k] + F(float, s, 176));
        F(float*, F(BYTE*, s, 72), 12)[k] =
            (float)(x87_sin((double)base[k] + F(float, s, 176)) * F(float, s, 188) + g_64c258);
    }
    return (int)F(BYTE*, s, 72);
}

} // extern "C"

// [$L_59d1a0] the same step for the other swirl class
extern "C" int __fastcall Swirl_Step2(BYTE* s) { return Swirl_Step(s); }

// === the rising smoke puff (59dbc0)

extern "C" void FUN_59aa10(); KING_ALIAS("_FUN_59aa10", "@SceneTexture_Advance@8")
extern "C" void Rp_MulAssign(); KING_ALIAS("_Rp_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Rp_TransformPoint(); KING_ALIAS("_Rp_TransformPoint", "@Vec3_TransformPoint@12")
extern "C" void Rp_LookAt(); KING_ALIAS("_Rp_LookAt", "_Matrix_LookAt")
ASM_VAR(BYTE*, g_rp7025b8, $L_7025b8)
ASM_VAR(float, g_rp64c904, $L_64c904)
ASM_VAR(float, g_rp64c330, $L_64c330)
ASM_VAR(float, g_rp64c264, $L_64c264)
struct Rp_V3 { float x, y, z; };
ASM_VAR(Vec3, g_rp702620, $L_702620)

extern "C" {

// [$L_59dbc0] step of a rising smoke puff s: its spin (+172) turned about its rise (+160) by +184 a second and
// grown by +188, added to its position (+76) with the rise (and the wind 7025b8 +532 along 702620); its size
// (+132..+144) blends with its life (+100 from 6f6a70, +104), its height kept within its size around +156; then the
// texture advances
int __fastcall SmokePuff_Step(BYTE* s)
{
    F(int, s, 100) = g_6f6a70;
    float dt = (float)F(double, F(BYTE*, s, 88), 2960);
    Rp_V3 v = *(const Rp_V3*)(s + 160);
    BYTE m[48];
    CallC(Rp_LookAt, m, *(const Rp_V3*)(BYTE*)g_ws702650, v, (float)(dt * (double)F(float, s, 184)));
    BYTE* sp = s + 172;
    CallThis(Rp_TransformPoint, sp, m);
    float t[3];
    CallThis(Rp_MulAssign, sp, t, (float)(dt * (double)F(float, s, 188) + g_effectOne));
    CallThis(Ws_AddAssign, s + 76, t, sp);
    float* p = (float*)(s + 76);
    const float* r = (const float*)(s + 160);
    if (BYTE* w = g_rp7025b8) {
        double wind = F(float, w, 532);
        const Vec3& a = g_rp702620;
        float w0 = (float)(a.x * wind), w1 = (float)(a.y * wind);
        double w2 = wind * a.z;
        float h0 = (float)(w0 * (double)g_effectFadeBase), h1 = (float)(w1 * (double)g_effectFadeBase);
        double h2 = w2 * g_effectFadeBase;
        float k0 = (float)((double)h0 + r[0]), k1 = (float)((double)h1 + r[1]);
        double k2 = h2 + r[2];
        float m0 = (float)(k0 * (double)dt), m1 = (float)(k1 * (double)dt);
        double m2 = k2 * dt;
        p[0] = (float)((double)m0 + p[0]);
        p[1] = (float)((double)m1 + p[1]);
        p[2] = (float)(m2 + p[2]);
    } else {
        double a0 = (double)dt * r[0];
        float a1 = (float)((double)dt * r[1]), a2 = (float)((double)dt * r[2]);
        p[0] = (float)(a0 + p[0]);
        p[1] = (float)((double)a1 + p[1]);
        p[2] = (float)((double)a2 + p[2]);
    }
    double f = ((double)F(float, s, 100) - F(float, s, 104)) / F(float, s, 100);
    if (f < g_rp64c264)
        f = g_rp64c264;
    else if (f > g_effectOne)
        f = g_effectOne;
    double g = g_effectOne - f;
    float sz = (float)(g * F(float, s, 136) + f * F(float, s, 132));
    double h = g * F(float, s, 144) + f * F(float, s, 140);
    double lo = h * g_rp64c904 + F(float, s, 156);
    float hi = (float)(h * g_rp64c330 + F(float, s, 156));
    double z = lo > F(float, s, 84) ? lo : F(float, s, 84);
    if (z > hi)
        z = hi;
    F(float, s, 84) = (float)z;
    BYTE* q = F(BYTE*, s, 72);
    float* a = F(float*, q, 16);
    a[0] = a[3] = (float)(sz * (double)g_mt64c878);
    a[1] = a[2] = (float)(sz * (double)g_64c258);
    float* b = F(float*, F(BYTE*, s, 72), 20);
    b[0] = b[1] = (float)(h * g_mt64c878);
    b[2] = b[3] = (float)(h * g_64c258);
    memcpy(s + 40, s + 76, 12);
    return CallThis<int>(FUN_59aa10, s);
}

} // extern "C"

// === a falling spinning flake (59e050)

// quad s's texture corners on the circle of radii +r1/+r2 at angle a (the first corner at the unrounded a)
static void Fl_TurnQuad(BYTE* s, double a, int ang, int r1, int r2)
{
    BYTE* q = F(BYTE*, s, 72);
    F(float*, q, 8)[0] = (float)(x87_cos(a + g_worobj6f6a58) * F(float, s, r1) + g_64c258);
    const float base[4] = {g_worobj6f6a58, g_worobj6f6a5c, g_worobj6f6a60, g_worobj6f6a54};
    for (int k = 0; k < 4; k++) {
        if (k)
            F(float*, F(BYTE*, s, 72), 8)[k] =
                (float)(x87_cos((double)base[k] + F(float, s, ang)) * F(float, s, r1) + g_64c258);
        F(float*, F(BYTE*, s, 72), 12)[k] =
            (float)(x87_sin((double)base[k] + F(float, s, ang)) * F(float, s, r2) + g_64c258);
    }
}

extern "C" {

// [$L_59e050] step of a falling flake s: its position (+76, +40) moved by its speed (+148) and the fall +176 along
// 702630 over the step, the speed then the fall added; its texture turned (+164) by +160 a second
int __fastcall Flake_Step(BYTE* s)
{
    double dt = F(double, F(BYTE*, s, 88), 2960);
    double g = F(float, s, 176);
    const Vec3& u = g_axis702630w;
    float a0 = (float)(u.x * g), a1 = (float)(u.y * g);
    double gz = g * u.z;
    float ax = (float)(a0 * dt), ay = (float)(a1 * dt);
    double az = gz * dt;
    float bx = (float)(ax * dt), by = (float)(ay * dt);
    double bz = az * dt;
    float* v = (float*)(s + 148);
    float* p = (float*)(s + 76);
    double vx = dt * v[0];
    float vy = (float)(dt * v[1]), vz = (float)(dt * v[2]);
    double px = vx + p[0];
    float py = (float)((double)vy + p[1]), pz = (float)((double)vz + p[2]);
    p[0] = (float)(px + bx);
    p[1] = (float)((double)by + py);
    p[2] = (float)(bz + pz);
    g = F(float, s, 176);
    float c0 = (float)(u.x * g), c1 = (float)(u.y * g);
    double cz = g * u.z;
    float acc[3] = {(float)(c0 * dt), (float)(c1 * dt), (float)(cz * dt)};
    float t[3];
    CallThis(Ws_AddAssign, v, t, acc);
    memcpy(s + 40, p, 12);
    double ang = F(float, s, 160) * F(double, F(BYTE*, s, 88), 2960) + F(float, s, 164);
    F(float, s, 164) = (float)ang;
    Fl_TurnQuad(s, ang, 164, 168, 172);
    return CallThis<int>(FUN_59aa10, s);
}

} // extern "C"

// === a drifting leaf (59bc50)

ASM_VAR(float, g_lf650c80, $L_650c80)

// slows speed v by +172 until it turns (then it stops); dt the step
static void Lf_Slow(BYTE* s, float* v)
{
    if (F(float, s, 172) == 0)
        return;
    float nb[3];
    const float* n = CallC<const float*>(Ws_Normalized, nb, v);
    double k = F(float, s, 172);
    double ax = -n[0] * k;
    float ay = (float)(-n[1] * k), az = (float)(-n[2] * k);
    float dt = (float)F(double, F(BYTE*, s, 88), 2960);
    float a[3] = {(float)(ax * dt), (float)(ay * (double)dt), (float)(az * (double)dt)};
    float t[3];
    CallThis(Ws_AddAssign, v, t, a);
    if ((double)v[2] * a[2] + (double)v[1] * a[1] + (double)v[0] * a[0] > g_effectZero) {
        F(int, s, 172) = 0;
        memcpy(v, (BYTE*)g_ws702650, 12);
    }
}

extern "C" {

// [$L_59bc50] step of a drifting leaf s: its speed (+160) slowed as a swirl's, its position (+76, +40) moved by it and
// by the wind (7025b8 +532 along 702620, scaled by 650c80); then its quad animated
int __fastcall Leaf_Step(BYTE* s)
{
    float* v = (float*)(s + 160);
    Lf_Slow(s, v);
    float* p = (float*)(s + 76);
    if (BYTE* w = g_rp7025b8) {
        double wind = F(float, w, 532);
        const Vec3& a = g_rp702620;
        Vec3 wv;
        wv.Set((float)(wind * a.x), (float)(wind * a.y), (float)(wind * a.z));
        double hx = wv.x * (double)g_lf650c80;
        float hy = (float)(wv.y * (double)g_lf650c80), hz = (float)(wv.z * (double)g_lf650c80);
        double sx = hx + v[0];
        float sy = (float)((double)hy + v[1]), sz = (float)((double)hz + v[2]);
        float dt = (float)F(double, F(BYTE*, s, 88), 2960);
        Vec3 r;
        r.Set((float)(sx * dt + p[0]), (float)(sy * (double)dt + p[1]), (float)(sz * (double)dt + p[2]));
        memcpy(p, &r, 12);
    } else {
        double dt = F(double, F(BYTE*, s, 88), 2960);
        Vec3 d, r;
        d.Set((float)(dt * v[0]), (float)(dt * v[1]), (float)(dt * v[2]));
        memcpy(p, Vec3_Add(&r, (const Vec3*)p, &d), 12);
    }
    memcpy(s + 40, p, 12);
    return CallThis<int>(FUN_59ac90, s);
}

} // extern "C"
