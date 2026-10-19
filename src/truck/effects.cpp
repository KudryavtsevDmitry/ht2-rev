// effects.cpp - aeffects.cpp of the game, rewritten from king.masm: the
// small functions of the effects unit: vector helpers, the constructors and
// destructors of effect and renderer objects, vertex buffers, the pool of
// lights, fade states, the reflection stack, and lookups of vehicles.
//
// Building with  nmake ASM_EFFECTS=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS     // strncpy as in the game
#include <string.h>
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

// vtables of the effect classes
ASM_VAR(AsmLabel, g_vtbl64c248, $L_64c248)
ASM_VAR(AsmLabel, g_vtbl64c2d8, $L_64c2d8)
ASM_VAR(AsmLabel, g_vtbl64c3a4, $L_64c3a4)
ASM_VAR(AsmLabel, g_vtbl64c498, $L_64c498)
ASM_VAR(AsmLabel, g_vtbl64c4e4, $L_64c4e4)
ASM_VAR(AsmLabel, g_vtbl64c530, $L_64c530)
ASM_VAR(AsmLabel, g_vtbl64c63c, $L_64c63c)
ASM_VAR(AsmLabel, g_vtbl64c67c, $L_64c67c)
ASM_VAR(AsmLabel, g_vtbl64c694, $L_64c694)
ASM_VAR(AsmLabel, g_vtbl64c6e0, $L_64c6e0)
ASM_VAR(AsmLabel, g_vtbl64c72c, $L_64c72c)
ASM_VAR(AsmLabel, g_vtbl64c758, $L_64c758)
ASM_VAR(AsmLabel, g_vtbl64c89c, $L_64c89c)
ASM_VAR(AsmLabel, g_vtbl64c8a8, $L_64c8a8)
ASM_VAR(AsmLabel, g_vtbl64d574, $L_64d574)
ASM_VAR(AsmLabel, g_strCRenderer, $L_672764)  // "CRenderer"
ASM_VAR(AsmLabel, g_strCopySound, $L_672cb8)  // "Copy Sound"

ASM_VAR(BYTE*, g_effectList, $L_6961f4)
ASM_VAR(BYTE*, g_game, $L_6d2078)
ASM_VAR(BYTE*, g_view, $L_696224)
typedef void* EffectBuffers[10];
ASM_VAR(EffectBuffers, g_effectBuffers, $L_696704)
ASM_VAR(BYTE*, g_truck, $L_6f33ac)
ASM_VAR(int, g_sound694838, $L_694838)
ASM_VAR(int, g_sound694834, $L_694834)
ASM_VAR(void*, g_sounds, $L_6f63d0)
ASM_VAR(int, g_696244, $L_696244)
ASM_VAR(int, g_6f6a78, $L_6f6a78)
ASM_VAR(void*, g_6961fc, $L_6961fc)
ASM_VAR(BYTE*, g_7025b8, $L_7025b8)
ASM_VAR(void*, g_d3d, $L_72090c)
ASM_VAR(void*, g_lightDevice, $L_696260)
ASM_VAR(int, g_lightsOn, $L_69625c)
ASM_VAR(void*, g_68b990, $L_68b990)
ASM_VAR(BYTE*, g_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_world, $L_696ca0)
ASM_VAR(unsigned, g_67b738, $L_67b738)

ASM_PROC(FUN_5b6700)    // the palette of a surface
extern "C" void FUN_59aea0(); KING_ALIAS("_FUN_59aea0", "@Spark59aea0_Construct@44")
extern "C" void FUN_59bed0(); KING_ALIAS("_FUN_59bed0", "@Spark59bed0_Construct@32")
extern "C" void FUN_59ad70(); KING_ALIAS("_FUN_59ad70", "@Spark59ad70_Construct@32")
extern "C" void FUN_59b3f0(); KING_ALIAS("_FUN_59b3f0", "@Flash_Construct@36")
ASM_VAR(float, g_randScale, $L_64c254)
ASM_VAR(float, g_randBase, $L_64c304)
ASM_VAR(float, g_one_f, $L_64c250)
ASM_VAR(float, g_696074, $L_696074)
ASM_VAR(float, g_696078, $L_696078)
ASM_VAR(int, g_6f6a80, $L_6f6a80)
ASM_VAR(int, g_6f6a7c, $L_6f6a7c)
ASM_VAR(int, g_6f6a88, $L_6f6a88)
ASM_VAR(int, g_696210, $L_696210)
ASM_VAR(AsmLabel, g_694998, $L_694998)
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
ASM_VAR(AsmLabel, g_vtbl64c244, $L_64c244)
ASM_VAR(AsmLabel, g_vtbl64c240, $L_64c240)
ASM_VAR(AsmLabel, g_vtbl64c628, $L_64c628)
ASM_VAR(AsmLabel, g_vtbl64c650, $L_64c650)
ASM_VAR(AsmLabel, g_vtbl64c668, $L_64c668)
ASM_VAR(AsmLabel, g_7025f0, $L_7025f0)
ASM_PROC(FUN_4daf40)
ASM_PROC(FUN_4dd2a0)
extern "C" void FUN_579380(); KING_ALIAS("_FUN_579380", "_Pool_Prepare")
extern "C" void FUN_59bb90(); KING_ALIAS("_FUN_59bb90", "@Effect59bb90_Construct@36")
extern "C" void FUN_59c420(); KING_ALIAS("_FUN_59c420", "@Effect59c420_Construct@32")
extern "C" void FUN_59d4a0(); KING_ALIAS("_FUN_59d4a0", "@Effect59d4a0_Construct@28")
ASM_PROC(FUN_4d9460)
ASM_PROC(FUN_5ef780)
ASM_PROC(FUN_5f1a70)
ASM_PROC(FUN_5d31e0)
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_5a0000(); KING_ALIAS("_FUN_5a0000", "@VehGlowManager_Construct@12")
ASM_PROC(FUN_5d3110)
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
ASM_PROC(FUN_4cf400)
ASM_PROC(FUN_544df0)
extern "C" void FUN_446c80(); KING_ALIAS("_FUN_446c80", "_Player_SetFlag20000")
ASM_VAR(int, g_6f6a84, $L_6f6a84)
ASM_VAR(int, g_6f6a8c, $L_6f6a8c)
ASM_VAR(int, g_6f6a98, $L_6f6a98)
ASM_VAR(int, g_696208, $L_696208)
ASM_VAR(AsmLabel, g_695bf0, $L_695bf0)
ASM_VAR(float, g_696054, $L_696054)
ASM_VAR(float, g_64c258, $L_64c258)
ASM_VAR(AsmLabel, g_vtbl64c428, $L_64c428)
ASM_VAR(AsmLabel, g_vtbl64c464, $L_64c464)
ASM_VAR(AsmLabel, g_vtbl64c770, $L_64c770)
ASM_VAR(AsmLabel, g_vtbl64c8b4, $L_64c8b4)
ASM_VAR(AsmLabel, g_vtbl64c90c, $L_64c90c)
ASM_VAR(AsmLabel, s_6728dc, $L_6728dc)
ASM_VAR(AsmLabel, s_67290c, $L_67290c)
ASM_VAR(AsmLabel, g_vtbl64c97c, $L_64c97c)
ASM_VAR(AsmLabel, s_672988, $L_672988)
ASM_VAR(AsmLabel, g_vtbl64ca58, $L_64ca58)
ASM_VAR(AsmLabel, g_vtbl64cdf8, $L_64cdf8)
ASM_VAR(AsmLabel, g_vtbl64cd0c, $L_64cd0c)
ASM_VAR(AsmLabel, g_vtbl64ccc0, $L_64ccc0)
ASM_VAR(AsmLabel, g_vtbl64ccbc, $L_64ccbc)
ASM_VAR(AsmLabel, g_vtbl64cff4, $L_64cff4)
ASM_VAR(AsmLabel, g_vtbl64cf08, $L_64cf08)
ASM_VAR(AsmLabel, g_vtbl64cebc, $L_64cebc)
ASM_VAR(AsmLabel, g_vtbl64ceb8, $L_64ceb8)
extern "C" double __fastcall Wave_Value(void* self, void*, double phase);   // engine.cpp
extern "C" void FUN_57c450(); KING_ALIAS("_FUN_57c450", "@FF_PoolStart@28")    // sounds: play
ASM_PROC(FUN_5d3290)    // Task::~Task
ASM_PROC(FUN_5ef660)    // module destructor
extern "C" void FUN_4bf510(); KING_ALIAS("_FUN_4bf510", "@Fx_CameraPair@8")    // particles: reset
extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")    // named object: constructor
extern "C" void FUN_4c09d0(); KING_ALIAS("_FUN_4c09d0", "@Effect4c09d0_Destroy@8")    // CRenderer::~CRenderer
ASM_PROC(FUN_4ccf30)
extern "C" BYTE* __fastcall SceneObj_Matrix1(void* self, void*, void* out);   // engine.cpp: its world matrix
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")    // vehicles: by id
extern "C" void FUN_5e49d0(); KING_ALIAS("_FUN_5e49d0", "@Resource_Load@12")    // vehicle: release
extern "C" void FUN_5fd370(); KING_ALIAS("_FUN_5fd370", "@Sound_Construct@20")    // sound: constructor
extern "C" void __cdecl Fx_Update4910(void* x);
KING_ALIAS("_Fx_Update4910", "FUN_4c4910")
extern "C" void __cdecl Fx_Update4df0(void* x);
KING_ALIAS("_Fx_Update4df0", "FUN_4c4df0")
extern "C" void __cdecl Fx_Message(const char* text, int);
KING_ALIAS("_Fx_Message", "FUN_5d2ba0")
extern "C" int __cdecl Fx_Find4e5bf5(int a, int b, int c, char* buf);
KING_ALIAS("_Fx_Find4e5bf5", "FUN_4e5bf5")
extern "C" void __cdecl Fx_DefaultData(void* data);
KING_ALIAS("_Fx_DefaultData", "_Car_ResetSetup")
extern "C" int __cdecl Fx_Handle(int a);
KING_ALIAS("_Fx_Handle", "_Objects_Find")

// a COM method: this on the stack
static int Com(void* obj, int offset, int a, int b)
{
    typedef int (__stdcall* Method)(void*, int, int);
    return ((Method)(*(void***)obj)[offset / 4])(obj, a, b);
}

static const DWORD ONE = 0x3f800000;   // 1.0f

// ---------------------------------------------------------------------------
// Vector helpers

extern "C" Vec3* __cdecl Vec3_Diff(Vec3* out, const Vec3* a, const Vec3* b)
{
    float z = (float)((double)a->z - b->z), y = (float)((double)a->y - b->y), x = (float)((double)a->x - b->x);
    out->x = x; out->y = y; out->z = z;
    return out;
}

extern "C" Vec3* __cdecl Vec3_Sum(Vec3* out, const Vec3* a, const Vec3* b)
{
    float z = (float)((double)a->z + b->z), y = (float)((double)a->y + b->y), x = (float)((double)a->x + b->x);
    out->x = x; out->y = y; out->z = z;
    return out;
}

extern "C" Vec3* __cdecl Vec3_ScaleBy(Vec3* out, float s, const Vec3* v)
{
    float z = (float)((double)s * v->z), y = (float)((double)s * v->y), x = (float)((double)s * v->x);
    out->x = x; out->y = y; out->z = z;
    return out;
}

extern "C" Vec3* __cdecl Vec3_Times(Vec3* out, const Vec3* v, float s)
{
    return Vec3_ScaleBy(out, s, v);
}

extern "C" Vec3* __cdecl Vec3_DivBy(Vec3* out, const Vec3* v, float s)
{
    float z = (float)((double)v->z / s), y = (float)((double)v->y / s), x = (float)((double)v->x / s);
    out->x = x; out->y = y; out->z = z;
    return out;
}

extern "C" double __cdecl Vec3_LengthSq(const Vec3* v)
{
    double x = v->x, y = v->y, z = v->z;
    return (x * x + y * y) + z * z;
}

extern "C" Vec3* __cdecl Vec3_Negated(Vec3* out, const Vec3* v)
{
    Vec3 r;
    r.x = -v->x;
    r.y = -v->y;
    r.z = -v->z;
    *out = r;
    return out;
}

extern "C" Vec3* __cdecl Vec3_Negate(Vec3* out, const Vec3* v)
{
    float z = -v->z, y = -v->y, x = -v->x;
    out->x = x; out->y = y; out->z = z;
    return out;
}

extern "C" Vec3* __cdecl Vec3_Cross2(Vec3* out, const Vec3* a, const Vec3* b)
{
    Vec3 r;
    r.x = (float)((double)b->z * a->y - (double)a->z * b->y);
    r.y = (float)((double)a->z * b->x - (double)b->z * a->x);
    r.z = (float)((double)a->x * b->y - (double)b->x * a->y);
    *out = r;
    return out;
}

// the midpoint of the two corners of an object's box
extern "C" Vec3* __cdecl Fx_BoxCenter(Vec3* out, BYTE* obj)
{
    Vec3 a = F(Vec3, obj, 3848), b = F(Vec3, obj, 3860);
    double x = (double)a.x + b.x;
    float y = (float)((double)a.y + b.y);
    float z = (float)((double)a.z + b.z);
    Vec3 r;
    r.x = (float)(x * 0.5f);
    r.y = (float)(y * 0.5f);
    r.z = (float)(z * 0.5f);
    *out = r;
    return out;
}

extern "C" float __cdecl Max3f(float a, float b, float c)
{
    float m = a > b ? a : b;
    if (c > m)
        m = c;
    return m;
}

extern "C" float __cdecl Min3f(float a, float b, float c)
{
    float m = !(a >= b) ? a : b;
    if (!(c >= m))
        m = c;
    return m;
}

// a colour component from hue h (degrees) between a and b
extern "C" double __cdecl HueComponent(float a, float b, float hue)
{
    double h = hue;
    if (h > 360.0f)
        h = h - 360.0f;
    if (!(h >= 0.0f))
        h = h + 360.0f;
    if (!(h >= 60.0f))
        return ((double)b - a) * h * 0.016666668f + a;
    if (!(h >= 180.0f))
        return b;
    if (!(h >= 240.0f))
        return ((double)b - a) * (240.0f - h) * 0.016666668f + a;
    return a;
}

extern "C" int __cdecl Fx_InRange(const float* v)
{
    if (!(v[1] >= 0.0f) || v[1] > 12.0f)
        return 0;
    if (!(v[0] >= -10.0f) || v[0] > 10.0f)
        return 0;
    return 1;
}

extern "C" int __cdecl Fx_EvenStep(int x)
{
    return ((x < 4 ? x + 1 : x) % 2 != 0) ? 0 : -1;
}

// ---------------------------------------------------------------------------
// Effects

// a light effect added to the effect list (up to 255)
extern "C" void __cdecl Fx_AddLight(int kind, const Vec3* pos, const Vec3* color, int a, int b)
{
    if (!g_effectList)
        return;
    BYTE* e = (BYTE*)operator new(44);
    if (e) {
        F(int, e, 4) = 0;
        F(int, e, 12) = 0; F(int, e, 16) = 0; F(int, e, 20) = 0;
        F(int, e, 24) = 0; F(int, e, 28) = 0; F(int, e, 32) = 0;
        F(void*, e, 0) = g_vtbl64c248;
        F(int, e, 8) = kind;
        F(Vec3, e, 12) = *pos;
        F(Vec3, e, 24) = *color;
        F(int, e, 36) = a;
        F(int, e, 40) = b;
        F(int, e, 4) = 2;
    }
    unsigned n = F(unsigned, g_effectList, 36);
    if (n < 255) {
        F(BYTE**, g_effectList, 32)[n] = e;
        F(unsigned, g_effectList, 36) = n + 1;
    }
}

// the player's vehicle model by the first letters of its name
static const char* VehicleName()
{
    BYTE* p = g_game;
    if (!p || !(p = F(BYTE*, p, 616)) || !(p = F(BYTE*, p, 308)))
        return 0;
    return F(const char*, p, 60);
}

extern "C" int __cdecl Fx_VehicleIsAaTr()
{
    const char* n = VehicleName();
    return n && ((n[0] == 'a' && n[1] == 'a') || (n[0] == 't' && n[1] == 'r'));
}

extern "C" int __cdecl Fx_VehicleIsDbAc()
{
    const char* n = VehicleName();
    return n && ((n[0] == 'd' && n[1] == 'b') || (n[0] == 'a' && n[1] == 'c'));
}

extern "C" void __cdecl Fx_SetPaletteColors()
{
    BYTE* p = CallThis<BYTE*>(FUN_5b6700, F(void*, g_view, 12)) + 780;
    p[0] = 189; p[1] = 164; p[2] = 109;
    p = CallThis<BYTE*>(FUN_5b6700, F(void*, g_view, 12)) + 783;
    p[0] = 179; p[1] = 164; p[2] = 134;
    p = CallThis<BYTE*>(FUN_5b6700, F(void*, g_view, 12)) + 786;
    p[0] = 178; p[1] = 152; p[2] = 106;
}

// an entry of 124 bytes, its name first, by name
static BYTE* FindByName(const char* name, BYTE* table)
{
    unsigned n = F(unsigned, table, 72);
    BYTE* e = F(BYTE*, table, 76);
    for (unsigned i = 0; i < n; i++, e += 124)
        if (strcmp((const char*)e, name) == 0)
            return F(BYTE*, table, 76) + i * 124;
    return 0;
}

extern "C" BYTE* __cdecl Fx_FindByName(const char* name, BYTE* table) { return FindByName(name, table); }
extern "C" BYTE* __stdcall Fx_FindByNameStd(const char* name, BYTE* table) { return FindByName(name, table); }

extern "C" void __cdecl Fx_FreeBuffers()
{
    for (int i = 0; i < 10; i++)
        if (g_effectBuffers[i]) {
            operator delete(g_effectBuffers[i]);
            g_effectBuffers[i] = 0;
        }
}

// the truck stands: grip at most 0.7 and no speed
extern "C" int __cdecl Fx_TruckStill()
{
    if (Wave_Value(g_truck, 0, 0.0) > 0.7)
        return 0;
    if (F(float, g_truck, 536) > 0.001f)
        return 0;
    return 1;
}

extern "C" int __cdecl Fx_PlaySound(int a, int b, int c, int d, int alt)
{
    int sound = alt ? g_sound694838 : g_sound694834;
    return CallThis<int>(FUN_57c450, g_sounds, a, b, c, d, sound);
}

extern "C" void __cdecl Fx_Update2(void* x)
{
    Fx_Update4910(x);
    Fx_Update4df0(x);
}

extern "C" void __cdecl Fx_Clear696244() { g_696244 = 0; }
extern "C" void __cdecl Fx_Clear6f6a78() { g_6f6a78 = 0; }

extern "C" void __cdecl Fx_Delete6961fc()
{
    if (g_6961fc) {
        CallVirt(g_6961fc, 0, 1);
        g_6961fc = 0;
    }
}

extern "C" double __cdecl Fx_Scale75() { return (double)F(float, g_7025b8, 548) * 0.75f; }

// the Direct3D adapter is an ATI Radeon (vendor 0x1002, device 0x5144)
extern "C" int __cdecl Fx_IsRadeon()
{
    DWORD id[268];
    memset(id, 0, sizeof id);
    if (Com(g_d3d, 108, (int)id, 0) >= 0 && id[258] == 4098 && id[259] == 20804)
        return 1;
    return 0;
}

extern "C" int __cdecl Fx_Find(int a, int b, int c)
{
    char buf[256];
    buf[0] = 0;
    int r = Fx_Find4e5bf5(a, b, c, buf);
    return buf[0] ? a : r;
}

// the truck is closer than 80 to the object
extern "C" int __cdecl Fx_NearTruck(BYTE* obj)
{
    BYTE m1[52], m2[52];
    BYTE* a = SceneObj_Matrix1(F(BYTE*, g_game, 616) + 16, 0, m1);
    BYTE* b = SceneObj_Matrix1(obj + 16, 0, m2);
    Vec3 d;
    d.x = (float)((double)F(float, a, 36) - F(float, b, 36));
    d.y = (float)((double)F(float, a, 40) - F(float, b, 40));
    d.z = (float)((double)F(float, a, 44) - F(float, b, 44));
    return !(Vec3_Length(&d) >= 80.0);
}

extern "C" bool __cdecl Fx_HasItems(BYTE* p) { return F(void*, p, 24) != F(void*, p, 28); }

// ---------------------------------------------------------------------------
// Fades: state +76 (0 off, 1 on, 2, 3 fading out, 4 fading in, 5), level +64

static double FrameTime() { return F(double, g_view, 2960); }

extern "C" int __cdecl Fade_Step(BYTE* f)
{
    int state = F(int, f, 76);
    if (state == 3) {
        double v = F(float, f, 64) - FrameTime() * 0.5f;
        F(float, f, 64) = (float)v;
        if (v > 0.0f)
            return 1;
        F(int, f, 76) = 0;
        F(int, f, 64) = 0;
        return 0;
    }
    if (state == 4) {
        double v = FrameTime() * 0.5f + F(float, f, 64);
        F(float, f, 64) = (float)v;
        if (!(v >= 1.0f))
            return 1;
        F(int, f, 76) = 1;
        F(DWORD, f, 64) = ONE;
        return 1;
    }
    return 0;
}

static int Frame() { return F(int, g_view, 2992); }

// 0: done this frame, 1: done last frame, 2: older
extern "C" int __cdecl Fade_Age(BYTE* f)
{
    int now = Frame();
    int at = F(int, f, 68);
    if (at == now)
        return 0;
    if (now != 0 && now - 1 == at)
        return 1;
    return 2;
}

extern "C" void __cdecl Fade_Touch(BYTE* f) { F(int, f, 68) = Frame(); }

static void SetFade(BYTE* f, int state, DWORD level)
{
    F(int, f, 76) = state;
    F(DWORD, f, 64) = level;
}

extern "C" int __cdecl Fade_Switch(BYTE* f, int on)
{
    switch (F(int, f, 76)) {
    case 0:
        SetFade(f, 0, 0);
        return 0;
    case 1:
        SetFade(f, on ? 1 : 3, ONE);
        return 1;
    case 2:
        if (!on) {
            SetFade(f, 0, 0);
            return 0;
        }
        F(int, f, 64) = 0;
        F(int, f, 76) = 2;
        return 0;
    case 3:
        F(int, f, 76) = 3;
        return 1;
    case 4:
        F(int, f, 76) = on ? 4 : 3;
        return 1;
    case 5:
        SetFade(f, on ? 1 : 3, ONE);
        return 1;
    }
    return 1;
}

extern "C" int __cdecl Fade_SwitchStep(BYTE* f, int on)
{
    switch (F(int, f, 76)) {
    case 0:
        SetFade(f, 0, 0);
        return 0;
    case 1:
        SetFade(f, on ? 1 : 3, ONE);
        return 1;
    case 2:
        if (!on) {
            SetFade(f, 0, 0);
            return 0;
        }
        F(int, f, 64) = 0;
        F(int, f, 76) = 4;
        return 1;
    case 3:
        F(int, f, 76) = 3;
        return Fade_Step(f);
    case 4:
        F(int, f, 76) = on ? 4 : 3;
        return Fade_Step(f);
    case 5:
        if (!on) {
            SetFade(f, 0, 0);
            return 0;
        }
        SetFade(f, 1, ONE);
        return 1;
    }
    return 1;
}

extern "C" int __cdecl Fade_Set(BYTE* f, int on)
{
    if (on) {
        SetFade(f, 1, ONE);
        return 1;
    }
    SetFade(f, 0, 0);
    return 0;
}

// scales the colours of a material by the fade level, once per frame
extern "C" int __cdecl Fade_Apply(BYTE* f, float* c)
{
    if (!Fade_Age(f))
        return 0;
    Fade_Touch(f);
    int state = F(int, f, 76);
    if (state != 3 && state != 4)
        return 0;
    static const int idx[] = { 1, 2, 3, 5, 6, 7, 9, 10, 11 };
    for (int i = 0; i < 9; i++)
        c[idx[i]] = c[idx[i]] * F(float, f, 64);
    return 1;
}

// ---------------------------------------------------------------------------
// Vehicles by id

static BYTE* VehicleById(BYTE* list, int id) { return CallThis<BYTE*>(FUN_5e5370, list + 704, id); }

extern "C" void* __cdecl Vehicle_Object(int id) { return F(void*, VehicleById(F(BYTE*, g_6cec6c, 1356), id), 28); }
extern "C" int __cdecl Vehicle_Exists(int id) { return Vehicle_Object(id) != 0; }

extern "C" int __cdecl Vehicle_Flag4012(int id)
{
    BYTE* v = VehicleById(F(BYTE*, g_6cec6c, 1356), id);
    if (!v)
        return 0;
    return (signed char)F(BYTE, F(BYTE*, v, 12), 4012) & 1;
}

extern "C" int __cdecl Vehicle_TimeOver(int id)
{
    BYTE* v = VehicleById(F(BYTE*, g_6cec6c, 1356), id);
    unsigned now = F(unsigned, g_world, 2704);
    if (v && now >= F(unsigned, v, 16))
        return 1;
    return now >= g_67b738;
}

extern "C" void __cdecl Vehicle_Release(int id)
{
    BYTE* v = VehicleById(g_world, id);
    if (v && F(void*, v, 28))
        CallThis(FUN_5e49d0, v, g_world + 704);
}

// ---------------------------------------------------------------------------
// Object data blocks: a name, a matrix at +248, 196 bytes of data at +48

extern "C" void __cdecl Fx_SetObject(BYTE* owner, const char* name, const Matrix* m, const void* data)
{
    BYTE* o = F(BYTE*, owner, 24);
    if (name)
        strncpy((char*)o + 16, name, 32);
    F(Matrix, o, 248) = *m;
    if (data)
        memcpy(o + 48, data, 196);
}

extern "C" void __cdecl Fx_SetObjectMatrix(BYTE* owner, const Matrix* m, const void* data)
{
    BYTE* o = F(BYTE*, owner, 24);
    F(Matrix, o, 248) = *m;
    if (data)
        memcpy(o + 48, data, 196);
}

// ---------------------------------------------------------------------------
// thiscall methods

#define SELF ((BYTE*)this)

struct FxMethods {
    BYTE* Self() { return SELF; }
    void DestructA();
    void DestructB();
    BYTE* InitFrame();
    BYTE* Set2Vec(const Vec3* a, const Vec3* b, int c, int d);
    BYTE* ParticlesConstruct(int a);
    int ParticlesAlloc(int a);
    void ParticlesFree();
    void ParticlesSetOrigin(const Vec3* v);
    void RendererSetEnabled(int a);
    BYTE* RendererConstruct();
    BYTE* RendererConstructAs(void* vtbl, bool clear76);
    void RendererDestructAs(void* vtbl);
    int RendererInit(void* dev, int a, int n, int b);
    int RendererCreate(void* dev, int a, int n, int b);
    int RendererCreateNamed(int name, int a, int n, int b);
    int RendererCreate3(void* dev, int a, int n);
    int RendererResetCount() { F(int, SELF, 68) = 0; return 0; }
    void AddVertex(const Vec3* p, const DWORD* c, float u, float v, bool twoUV);
    BYTE* Lights50(int a);
    void ListDestruct();
    BYTE* Lights50b();
    BYTE* SparkInit(int a);
    void SparksAlloc();
    BYTE* RefClear() { F(int, SELF, 400) = 0; return SELF; }
    int RefEmpty() { return F(int, SELF, 400) == 0; }
    void RefPush(void* p);
    void* RefPop();
    BYTE* Set8(const Vec3* a, const DWORD* b, int c, int d, int e, int f);
    void AddReflection(const void* src, int a);
    BYTE* LightsConstruct(unsigned n, void* dev);
    void LightsDestruct();
    int LightsRelease(int a);
    BYTE* ObjectInit(int a, int b, const char* name, const Matrix* m, int c, const void* data, int d, int e, int f);
    BYTE* InitState();
    void* CopyMatrix(void* out) { memcpy(out, SELF + 16, 48); return out; }
    BYTE* HandleInit(int a) { F(int, SELF, 0) = Fx_Handle(a); return SELF; }
    BYTE* CopySoundInit(BYTE* src);
};

void FxMethods::DestructA()
{
    F(void*, SELF, 0) = g_vtbl64c2d8;
    if (F(void*, SELF, 32)) {
        operator delete(F(void*, SELF, 32));
        F(void*, SELF, 32) = 0;
    }
    CallThis(FUN_5d3290, SELF);
}

void FxMethods::DestructB()
{
    F(void*, SELF, 0) = g_vtbl64c3a4;
    for (int i = 0; i < 2; i++)
        operator delete(F(void*, SELF, 76 + i * 4));
    if (F(void*, SELF, 72))
        operator delete(F(void*, SELF, 72));
    CallThis(FUN_5ef660, SELF);
}

// three zero vectors and an identity matrix
BYTE* FxMethods::InitFrame()
{
    for (int i = 0; i < 9; i++)
        F(int, SELF, i * 4) = 0;
    F(Matrix, SELF, 36) = g_identity;
    return SELF;
}

BYTE* FxMethods::Set2Vec(const Vec3* a, const Vec3* b, int c, int d)
{
    F(Vec3, SELF, 0) = *a;
    F(Vec3, SELF, 12) = *b;
    F(int, SELF, 24) = c;
    F(int, SELF, 28) = d;
    return SELF;
}

BYTE* FxMethods::ParticlesConstruct(int a)
{
    static const int zero[] = { 68, 72, 76, 80, 84, 88, 40, 20, 132, 124, 128 };
    for (int i = 0; i < 11; i++)
        F(int, SELF, zero[i]) = 0;
    F(DWORD, SELF, 56) = 0x40000000;    // 2.0f
    F(DWORD, SELF, 64) = 0x40000000;
    F(DWORD, SELF, 16) = 0x3d75c28f;    // 0.06f
    F(DWORD, SELF, 12) = 0x38d1b717;    // 0.0001f
    F(DWORD, SELF, 8) = 0x3dcccccd;     // 0.1f
    F(DWORD, SELF, 28) = 0x3f4ccccd;    // 0.8f
    F(DWORD, SELF, 24) = 0xc1f00000;    // -30.0f
    F(int, SELF, 0) = 2000;
    F(int, SELF, 4) = 64;
    F(int, SELF, 36) = 1;
    F(DWORD, SELF, 32) = 0x3e4ccccd;    // 0.2f
    F(DWORD, SELF, 52) = 0x41200000;    // 10.0f
    F(DWORD, SELF, 60) = 0x41700000;    // 15.0f
    F(int, SELF, 136) = a;
    CallThis(FUN_4bf510, SELF);
    return SELF;
}

// a free list of particles, 32 bytes each
int FxMethods::ParticlesAlloc(int a)
{
    F(int, SELF, 96) = a;
    int n = F(int, SELF, 0);
    F(int, SELF, 44) = n;
    BYTE* p = (BYTE*)operator new(n << 5);
    F(BYTE*, SELF, 132) = p;
    if (!p)
        return 0x8007000e;              // E_OUTOFMEMORY
    for (int i = 0; i < F(int, SELF, 0) - 1; i++)
        F(BYTE*, F(BYTE*, SELF, 132), i * 32) = F(BYTE*, SELF, 132) + i * 32 + 32;
    F(BYTE*, F(BYTE*, SELF, 132), (F(int, SELF, 0) << 5) - 32) = 0;
    F(BYTE*, SELF, 124) = F(BYTE*, SELF, 132);
    F(int, SELF, 128) = 0;
    return 0;
}

void FxMethods::ParticlesFree()
{
    if (F(void*, SELF, 132))
        operator delete(F(void*, SELF, 132));
}

void FxMethods::ParticlesSetOrigin(const Vec3* v)
{
    F(Vec3, SELF, 100) = *v;
    F(Vec3, SELF, 112) = *v;
}

void FxMethods::RendererSetEnabled(int a)
{
    F(int, SELF, 36) = a;
    if (!a)
        F(int, SELF, 20) = 0;
}

BYTE* FxMethods::RendererConstruct()
{
    CallThis(FUN_5f19b0, SELF, (const char*)g_strCRenderer, 5, 1);
    F(void*, SELF, 0) = g_vtbl64c498;
    F(int, SELF, 56) = 0;
    F(int, SELF, 60) = 0;
    F(int, SELF, 68) = 0;
    F(int, SELF, 64) = 0;
    return SELF;
}

BYTE* FxMethods::RendererConstructAs(void* vtbl, bool clear76)
{
    RendererConstruct();
    F(void*, SELF, 0) = vtbl;
    if (clear76)
        F(int, SELF, 76) = 0;
    return SELF;
}

void FxMethods::RendererDestructAs(void* vtbl)
{
    F(void*, SELF, 0) = vtbl;
    if (F(void*, SELF, 76)) {
        operator delete(F(void*, SELF, 76));
        F(void*, SELF, 76) = 0;
    }
    CallThis(FUN_4c09d0, SELF);
}

int FxMethods::RendererInit(void* dev, int a, int n, int b)
{
    F(int, SELF, 72) = a;
    F(int, SELF, 64) = n * 3;
    CallVirt(dev, 20, b, SELF);
    return 0;
}

// a vertex buffer of 3n + 1 vertices of 24 bytes
static BYTE* NewVertices(BYTE* self, int n)
{
    BYTE* v = (BYTE*)operator new((n * 3 + 1) * 3 * 8);
    F(BYTE*, self, 76) = v;
    return v;
}

int FxMethods::RendererCreate(void* dev, int a, int n, int b)
{
    if (!NewVertices(SELF, n))
        return 0x80004005;              // E_FAIL
    return RendererInit(dev, a, n, b);
}

int FxMethods::RendererCreateNamed(int name, int a, int n, int b)
{
    F(int, SELF, 80) = name;
    if (!NewVertices(SELF, n))
        return 0x80004005;
    return RendererInit((void*)name, a, n, b);
}

int FxMethods::RendererCreate3(void* dev, int a, int n)
{
    if (!NewVertices(SELF, n))
        return 0x80004005;
    return RendererInit(dev, a, n, 0);
}

void FxMethods::AddVertex(const Vec3* p, const DWORD* c, float u, float v, bool twoUV)
{
    int n = F(int, SELF, 68);
    if (n == F(int, SELF, 64))
        return;
    BYTE* vx = F(BYTE*, SELF, 76) + n * 24;
    F(Vec3, vx, 0) = *p;
    F(DWORD, vx, 12) = *c;
    F(float, vx, 16) = u;
    F(float, vx, 20) = twoUV ? v : u;
    F(int, SELF, 68) = n + 1;
}

BYTE* FxMethods::Lights50(int a)
{
    F(void*, SELF, 0) = g_vtbl64c63c;
    F(int, SELF, 4) = 0;
    memset(SELF + 12, 0, 50 * 12);
    F(void*, SELF, 0) = g_vtbl64c67c;
    F(int, SELF, 8) = a;
    return SELF;
}

void FxMethods::ListDestruct()
{
    F(void*, SELF, 0) = g_vtbl64c63c;
    F(int, SELF, 8) = 0;
    F(int, SELF, 4) = 0;
}

BYTE* FxMethods::Lights50b()
{
    F(void*, SELF, 0) = g_vtbl64c63c;
    F(int, SELF, 4) = 0;
    memset(SELF + 8, 0, 50 * 12);
    F(void*, SELF, 0) = g_vtbl64c758;
    return SELF;
}

BYTE* FxMethods::SparkInit(int a)
{
    F(int, SELF, 4) = a;
    F(void*, SELF, 0) = g_vtbl64c72c;
    F(int, SELF, 8) = 1000;
    F(DWORD, SELF, 12) = 0x3a83126f;    // 0.001f
    F(int, SELF, 44) = 0;
    F(int, SELF, 16) = 0;
    F(int, SELF, 20) = 0;
    F(DWORD, SELF, 24) = 0x40b00000;    // 5.5f
    F(DWORD, SELF, 28) = 0x3e800000;    // 0.25f
    F(DWORD, SELF, 32) = 0x3e19999a;    // 0.15f
    F(DWORD, SELF, 36) = ONE;
    F(int, SELF, 56) = 0;
    F(int, SELF, 48) = 0;
    F(int, SELF, 52) = 0;
    return SELF;
}

// a free list of sparks, 24 bytes each
void FxMethods::SparksAlloc()
{
    int n = F(int, SELF, 8);
    F(BYTE*, SELF, 56) = (BYTE*)operator new(n * 24);
    for (int i = 0; i < F(int, SELF, 8) - 1; i++)
        F(BYTE*, F(BYTE*, SELF, 56), i * 24) = F(BYTE*, SELF, 56) + i * 24 + 24;
    F(BYTE*, F(BYTE*, SELF, 56), F(int, SELF, 8) * 24 - 24) = 0;
    F(BYTE*, SELF, 48) = F(BYTE*, SELF, 56);
    F(int, SELF, 52) = 0;
}

void FxMethods::RefPush(void* p)
{
    unsigned n = F(unsigned, SELF, 400);
    if (n >= 100) {
        Fx_Message("Reflection stack overflow", 0);
        return;
    }
    F(void*, SELF, n * 4) = p;
    F(unsigned, SELF, 400) = n + 1;
}

void* FxMethods::RefPop()
{
    int n = F(int, SELF, 400);
    if (n == 0)
        return 0;
    F(int, SELF, 400) = --n;
    return F(void*, SELF, n * 4);
}

BYTE* FxMethods::Set8(const Vec3* a, const DWORD* b, int c, int d, int e, int f)
{
    F(Vec3, SELF, 0) = *a;
    F(DWORD, SELF, 12) = *b;
    F(int, SELF, 16) = c;
    F(int, SELF, 20) = d;
    F(int, SELF, 24) = e;
    F(int, SELF, 28) = f;
    return SELF;
}

void FxMethods::AddReflection(const void* src, int a)
{
    int n = F(int, SELF, 13296);
    if (n == 100)
        return;
    memcpy(SELF + n * 128 + 64, src, 128);
    F(int, SELF, 12896 + F(int, SELF, 13296) * 4) = a;
    F(int, SELF, 13296)++;
}

// the pool of 255 hardware lights, all switched off
BYTE* FxMethods::LightsConstruct(unsigned n, void* dev)
{
    g_lightDevice = dev;
    if (n > 8)
        n = 8;
    F(unsigned, SELF, 0) = n;
    F(int, SELF, 8) = 0;
    F(int, SELF, 4) = 0;
    BYTE* p = (BYTE*)operator new(30600);
    if (p)
        for (int i = 0; i < 255; i++)
            F(int, p, i * 120 + 112) = 0;
    F(BYTE*, SELF, 12) = p;
    for (int i = 0; i < 255; i++) {
        F(int, F(BYTE*, SELF, 12), i * 120 + 112) = 0;
        F(int, F(BYTE*, SELF, 12), i * 120 + 104) = 0;
        F(int, F(BYTE*, SELF, 12), i * 120 + 108) = 0;
        F(int, F(BYTE*, SELF, 12), i * 120 + 116) = -1;
        Com(dev, 176, i, 0);            // LightEnable(i, FALSE)
    }
    return SELF;
}

void FxMethods::LightsDestruct()
{
    if (F(void*, SELF, 12)) {
        operator delete(F(void*, SELF, 12));
        F(void*, SELF, 12) = 0;
    }
}

int FxMethods::LightsRelease(int a)
{
    if (!F(int, SELF, 4) || !g_view)
        return 0x80004005;
    int i = --F(int, SELF, 4);
    BYTE* e = F(BYTE*, SELF, 12) + i * 120;
    if (F(int, F(BYTE*, e, 104), 72) > 130) {
        if (F(int, e, 112)) {
            Com(g_lightDevice, 176, F(int, e, 116), 0);
            F(int, e, 112) = 0;
        }
    } else if (F(int, e, 112) == 1) {
        g_lightsOn--;
        Com(g_lightDevice, 176, F(int, e, 116), 0);
    }
    return CallThis<int>(FUN_4ccf30, SELF, a);
}

BYTE* FxMethods::ObjectInit(int a, int b, const char* name, const Matrix* m, int c, const void* data, int d, int e, int f)
{
    F(Matrix, SELF, 248) = g_identity;
    F(void*, SELF, 296) = g_68b990;
    F(int, SELF, 0) = a;
    F(int, SELF, 4) = b;
    strncpy((char*)SELF + 16, name, 32);
    F(Matrix, SELF, 248) = *m;
    F(int, SELF, 296) = c;
    F(int, SELF, 8) = e;
    F(int, SELF, 12) = f;
    F(int, SELF, 244) = d;
    if (data)
        memcpy(SELF + 48, data, 196);
    else
        Fx_DefaultData(SELF + 48);
    return SELF;
}

BYTE* FxMethods::InitState()
{
    static const BYTE minus1[] = { 0, 16, 20, 44, 48 };
    for (int off = 8; off <= 176; off += 4)
        if (off != 116)                 // +116 is left as it is
            F(int, SELF, off) = 0;
    for (int i = 0; i < 5; i++)
        F(int, SELF, minus1[i]) = -1;
    F(int, SELF, 180) = 0x3ff00000;     // with +176: 1.0
    F(BYTE, SELF, 184) = 0;
    return SELF;
}

BYTE* FxMethods::CopySoundInit(BYTE* src)
{
    CallThis(FUN_5fd370, SELF, (const char*)g_strCopySound, F(int, src, 28), 0);
    F(void*, SELF, 0) = g_vtbl64d574;
    return SELF;
}

#define M(p) ((FxMethods*)(p))

extern "C" {
BYTE* __fastcall Fx_Self(BYTE* s, void*) { return M(s)->Self(); }                                          // [FUN_4bd510]
void __fastcall Fx_DestructA(BYTE* s, void*) { M(s)->DestructA(); }                                        // [FUN_4ba010]
void __fastcall Fx_DestructB(BYTE* s, void*) { M(s)->DestructB(); }                                        // [FUN_4bd810]
BYTE* __fastcall Fx_InitFrame(BYTE* s, void*) { return M(s)->InitFrame(); }                                // [FUN_4bd590]
BYTE* __fastcall Fx_Set2Vec(BYTE* s, void*, const Vec3* a, const Vec3* b, int c, int d) { return M(s)->Set2Vec(a, b, c, d); } // [FUN_4bdeb0]
BYTE* __fastcall Particles_Construct(BYTE* s, void*, int a) { return M(s)->ParticlesConstruct(a); }        // [FUN_4bf470]
int __fastcall Particles_Alloc(BYTE* s, void*, int a) { return M(s)->ParticlesAlloc(a); }                  // [FUN_4bf6b0]
void __fastcall Particles_Free(BYTE* s, void*) { M(s)->ParticlesFree(); }                                  // [FUN_4bf740]
void __fastcall Particles_SetOrigin(BYTE* s, void*, const Vec3* v) { M(s)->ParticlesSetOrigin(v); }        // [FUN_4bf760]
void __fastcall Renderer_SetEnabled(BYTE* s, void*, int a) { M(s)->RendererSetEnabled(a); }                // [FUN_4c0880]
BYTE* __fastcall Renderer_Construct(BYTE* s, void*) { return M(s)->RendererConstruct(); }                  // [FUN_4c08a0]
BYTE* __fastcall RendererA_Construct(BYTE* s, void*) { return M(s)->RendererConstructAs(g_vtbl64c4e4, true); } // [FUN_4c08f0]
BYTE* __fastcall RendererB_Construct(BYTE* s, void*) { return M(s)->RendererConstructAs(g_vtbl64c530, true); } // [FUN_4c0930]
void __fastcall RendererA_Destruct(BYTE* s, void*) { M(s)->RendererDestructAs(g_vtbl64c4e4); }             // [FUN_4c0970]
void __fastcall RendererB_Destruct(BYTE* s, void*) { M(s)->RendererDestructAs(g_vtbl64c530); }             // [FUN_4c09a0]
BYTE* __fastcall RendererC_Construct(BYTE* s, void*) { return M(s)->RendererConstructAs(g_vtbl64c694, false); } // [FUN_4c26f0]
void __fastcall RendererC_Destruct(BYTE* s, void*) { M(s)->RendererDestructAs(g_vtbl64c694); }             // [FUN_4c28b0]
BYTE* __fastcall RendererD_Construct(BYTE* s, void*) { return M(s)->RendererConstructAs(g_vtbl64c6e0, false); } // [FUN_4c28e0]
void __fastcall RendererD_Destruct(BYTE* s, void*) { M(s)->RendererDestructAs(g_vtbl64c6e0); }             // [FUN_4c2a10]
int __fastcall Renderer_Create(BYTE* s, void*, void* dev, int a, int n, int b) { return M(s)->RendererCreate(dev, a, n, b); }   // [FUN_4c0a30]
int __fastcall Renderer_CreateNamed(BYTE* s, void*, int name, int a, int n, int b) { return M(s)->RendererCreateNamed(name, a, n, b); } // [FUN_4c0a90]
int __fastcall Renderer_Init(BYTE* s, void*, void* dev, int a, int n, int b) { return M(s)->RendererInit(dev, a, n, b); }       // [FUN_4c0af0]
int __fastcall Renderer_Create3(BYTE* s, void*, void* dev, int a, int n) { return M(s)->RendererCreate3(dev, a, n); }          // [FUN_4c2710]
int __fastcall Renderer_ResetCount(BYTE* s, void*) { return M(s)->RendererResetCount(); }                  // [FUN_4c0b20]
void __fastcall Renderer_AddVertex(BYTE* s, void*, const Vec3* p, const DWORD* c, float u, float v) { M(s)->AddVertex(p, c, u, v, false); }  // [FUN_4c0b30]
void __fastcall Renderer_AddVertexB(BYTE* s, void*, const Vec3* p, const DWORD* c, float u, float v) { M(s)->AddVertex(p, c, u, v, false); } // [FUN_4c3030]
void __fastcall Renderer_AddVertexUV(BYTE* s, void*, const Vec3* p, const DWORD* c, float u, float v) { M(s)->AddVertex(p, c, u, v, true); }  // [FUN_4c37d0]
BYTE* __fastcall Lights50_Construct(BYTE* s, void*, int a) { return M(s)->Lights50(a); }                   // [FUN_4c2270]
void __fastcall List_Destruct(BYTE* s, void*) { M(s)->ListDestruct(); }                                    // [FUN_4c26e0]
BYTE* __fastcall Lights50b_Construct(BYTE* s, void*) { return M(s)->Lights50b(); }                         // [FUN_4c3cc0]
BYTE* __fastcall Sparks_Construct(BYTE* s, void*, int a) { return M(s)->SparkInit(a); }                    // [FUN_4c2a40]
void __fastcall Sparks_Alloc(BYTE* s, void*) { M(s)->SparksAlloc(); }                                      // [FUN_4c2ac0]
BYTE* __fastcall RefStack_Clear(BYTE* s, void*) { return M(s)->RefClear(); }                               // [FUN_4c8020]
int __fastcall RefStack_Empty(BYTE* s, void*) { return M(s)->RefEmpty(); }                                 // [FUN_4c8030]
void __fastcall RefStack_Push(BYTE* s, void*, void* p) { M(s)->RefPush(p); }                               // [FUN_4c8040]
void* __fastcall RefStack_Pop(BYTE* s, void*) { return M(s)->RefPop(); }                                   // [FUN_4c8080]
BYTE* __fastcall Fx_SetVtbl89c(BYTE* s, void*) { F(void*, s, 0) = g_vtbl64c89c; return s; }                // [FUN_4c8160]
void __fastcall Fx_SetVtbl8a8(BYTE* s, void*) { F(void*, s, 0) = g_vtbl64c8a8; }                           // [FUN_4c85b0]
BYTE* __fastcall Fx_Set8(BYTE* s, void*, const Vec3* a, const DWORD* b, int c, int d, int e, int f) { return M(s)->Set8(a, b, c, d, e, f); } // [FUN_4c9930]
void __fastcall Fx_AddReflection(BYTE* s, void*, const void* src, int a) { M(s)->AddReflection(src, a); }  // [FUN_4cc800]
BYTE* __fastcall Lights_Construct(BYTE* s, void*, unsigned n, void* dev) { return M(s)->LightsConstruct(n, dev); } // [FUN_4cce70]
void __fastcall Lights_Destruct(BYTE* s, void*) { M(s)->LightsDestruct(); }                                // [FUN_4ccf10]
int __fastcall Lights_Release(BYTE* s, void*, int a) { return M(s)->LightsRelease(a); }                    // [FUN_4cd3c0]
BYTE* __fastcall Fx_ObjectInit(BYTE* s, void*, int a, int b, const char* name, const Matrix* m, int c, const void* data, int d, int e, int f) { return M(s)->ObjectInit(a, b, name, m, c, data, d, e, f); } // [FUN_4cd670]
int __fastcall Fx_Get12(BYTE* s, void*) { return F(int, s, 12); }                                          // [FUN_4cf5e0]
int __fastcall Fx_Get9544(BYTE* s, void*) { return F(int, s, 9544); }                                      // [FUN_4cf5f0]
int __fastcall Fx_Get10420(BYTE* s, void*) { return F(int, s, 10420); }                                    // [FUN_4cf600]
int __fastcall Fx_Get20396(BYTE* s, void*) { return F(int, s, 20396); }                                    // [FUN_4cf610]
BYTE* __fastcall Fx_InitState(BYTE* s, void*) { return M(s)->InitState(); }                                // [FUN_4d06a0]
void* __fastcall Fx_CopyMatrix(BYTE* s, void*, void* out) { return M(s)->CopyMatrix(out); }                // [FUN_4d74c0]
BYTE* __fastcall Fx_HandleInit(BYTE* s, void*, int a) { return M(s)->HandleInit(a); }                      // [FUN_4dd900]
BYTE* __fastcall CopySound_Construct(BYTE* s, void*, BYTE* src) { return M(s)->CopySoundInit(src); }       // [FUN_4dd9c0]
}

// ---------------------------------------------------------------------------
// medium-sized functions of the unit

struct Vec3Bits { DWORD x, y, z; };
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")    // error (code, format, file, line, date)
extern "C" void FUN_5d9290(); KING_ALIAS("_FUN_5d9290", "@Scenes_Find@12")    // a model by name
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")    // network: send (type, data, size)
ASM_PROC(FUN_593720)
extern "C" void FUN_57c490(); KING_ALIAS("_FUN_57c490", "@Shadow_Add@32")
extern "C" void __fastcall Hits_Remember(BYTE* self, void*, const BYTE* hits, unsigned n);
extern "C" Vec3* __fastcall Vec3_ZeroThis(Vec3* self, void*);
ASM_VAR(AsmLabel, g_vtbl64c24c, $L_64c24c)
ASM_VAR(void*, g_lib696204, $L_696204)
ASM_VAR(void*, g_lib696200, $L_696200)
typedef BYTE* Materials[1];
ASM_VAR(Materials, g_lightMats, $L_695cbc)      // 6 materials of the lights
ASM_VAR(Materials, g_vaporMats, $L_695c58)
ASM_VAR(int, g_vaporCount, $L_69620c)
ASM_VAR(AsmLabel, g_strErrFormat, $L_6724b0)    // " File = %s, Line = #%ld, Date = %s "
ASM_VAR(AsmLabel, g_strErrFile, $L_6724d4)      // the source file
ASM_VAR(AsmLabel, g_strErrDate, $L_6724f8)
ASM_VAR(AsmLabel, g_strTrucks, $L_66cd44)       // "trucks"
ASM_VAR(float, g_skid1Level, $L_696030)
ASM_VAR(float, g_skid2Level, $L_69604c)
ASM_VAR(float, g_shadowLevel, $L_69606c)
ASM_VAR(void*, g_6f66a8, $L_6f66a8)
ASM_VAR(void*, g_696234, $L_696234)

static void Fx_Error(int line)
{
    CallC(FUN_60aad0, 0x88762715, (const char*)g_strErrFormat, (const char*)g_strErrFile, line,
          (const char*)g_strErrDate);
}

static inline BYTE* Fx_Material(const char* name, void* lib) { return Fx_FindByName(name, (BYTE*)lib); }

// adds a decal (a mark at p along n) to the effect list
extern "C" void __cdecl Fx_AddDecal(const Vec3Bits* p, const Vec3Bits* n, int a, int b, int c)
{
    if (!g_effectList)
        return;
    DWORD* o = (DWORD*)operator new(48);
    if (o) {
        for (int i = 1; i <= 7; i++)
            o[i] = 0;
        o[0] = (DWORD)(void*)g_vtbl64c24c;
        F(Vec3Bits, o, 8) = *p;
        F(Vec3Bits, o, 20) = *n;
        o[8] = a;
        o[9] = b ? b : 0;
        o[10] = b ? 1 : 0;
        o[11] = c;
        o[1] = 3;
    }
    BYTE* l = g_effectList;
    if (F(unsigned, l, 36) < 255) {
        F(DWORD**, l, 32)[F(unsigned, l, 36)] = o;
        F(int, l, 36)++;
    }
}

extern "C" void __cdecl Fx_LoadLightMaterials()
{
    static const char* const names[6] = { "mat_l_noiseHEAD", "mat_lg_head", "mat_lg_rvrs", "mat_lg_stop",
                                          "mat_l_noiseSTOP", "mat_l_noiseRVRS" };
    static const int lines[6] = { 630, 639, 647, 655, 663, 671 };
    for (int i = 0; i < 6; i++) {
        g_lightMats[i] = Fx_Material(names[i], g_lib696204);
        if (!g_lightMats[i])
            Fx_Error(lines[i]);
        F(DWORD, g_lightMats[i], 88) = 0x3f800000;
        F(DWORD, g_lightMats[i], 32) |= 0x40220;
    }
}

extern "C" void __cdecl Fx_LoadVapor()
{
    char buf[256];
    g_vaporCount = 0;
    for (int i = 0;; i++) {
        sprintf(buf, i > 9 ? "%s%d" : "%s0%d", "mvapor", i);
        BYTE* m = Fx_Material(buf, g_lib696200);
        g_vaporMats[i] = m;
        if (!m)
            break;
        F(DWORD, m, 88) = 0x3f800000;
        F(DWORD, g_vaporMats[i], 32) |= 0x40220;
        g_vaporCount++;
    }
    if (!g_vaporCount)
        Fx_Error(726);
}

extern "C" void __cdecl Fx_LoadShadows()
{
    BYTE* o = CallThis<BYTE*>(FUN_5d9290, g_view, (const char*)g_strTrucks);
    g_sound694838 = 0;
    g_sound694834 = 0;
    bool found = false;
    BYTE* arr = F(BYTE*, o, 76);
    for (unsigned i = 0; i < F(unsigned, o, 72); i++)
        if (strcmp((const char*)arr + i * 124, "effect_Shadow") == 0) {
            g_sound694834 = (int)(arr + i * 124);
            found = g_sound694834 != 0;
            break;
        }
    if (!found)
        Fx_Error(744);
    g_sound694838 = (int)Fx_Material("shadtree0", g_lib696200);
    if (!g_sound694838)
        Fx_Error(750);
    F(DWORD, g_sound694834, 32) |= 0x40200;
    F(DWORD, g_sound694838, 32) |= 0x40200;
}

static int Fx_Skid(float level, BYTE type, const Vec3Bits* p, const Vec3Bits* v, int net)
{
    if (!(level > 0.5f))
        return 0;
    if (net && (F(int, g_6cec6c, 1300) == 1 || F(int, g_6cec6c, 1316))) {
        BYTE pkt[28] = { 0 };
        pkt[0] = type;
        F(Vec3Bits, pkt, 4) = *p;
        F(Vec3Bits, pkt, 16) = *v;
        CallThis(FUN_603110, g_view + 8336, 957, pkt, 28);
    }
    double len = Vec3_Length((const Vec3*)v) * 6.0f;
    if (len > 100.0f)
        len = 100.0f;
    return CallThis<int>(FUN_593720, g_6f66a8, p, v, (int)len);
}

extern "C" int __cdecl Fx_Skid1(const Vec3Bits* p, const Vec3Bits* v, int net)
{
    return Fx_Skid(g_skid1Level, 1, p, v, net);
}

extern "C" int __cdecl Fx_Skid2(const Vec3Bits* p, const Vec3Bits* v, int net)
{
    return Fx_Skid(g_skid2Level, 2, p, v, net);
}

// a vehicle's shadow at the detail level of the options
extern "C" void __cdecl Fx_Shadow(int a, int b, int c, int d, int tree)
{
    void* m = (void*)(tree ? g_sound694838 : g_sound694834);
    float k = g_shadowLevel;
    int lvl = !(k > 0.25f) ? 3 : !(k > 0.5f) ? 2 : !(k > 0.75f) ? 1 : 0;
    CallThis(FUN_57c490, g_sounds, a, b, c, lvl, d, m);
    if (g_696234)
        Hits_Remember((BYTE*)g_696234, 0, (const BYTE*)a, b);
}

extern "C" void FUN_4ccd50(); KING_ALIAS("_FUN_4ccd50", "_Fade_RunStage")
ASM_PROC(FUN_5d2ba0)    // shows an error text (text, a)
extern "C" Matrix* __fastcall Matrix_IdentityThis(Matrix* self, void*);   // vecmath.cpp
ASM_VAR(BYTE*, g_glows, $L_696248)              // 100 records of 116 bytes
ASM_VAR(int, g_glowCount, $L_69624c)
typedef char ErrText[1];
ASM_VAR(ErrText, g_errText69608c, $L_69608c)
ASM_VAR(AsmLabel, g_strBadGlow, $L_6727a0)      // "Invalid number of static glow in module %s, ..."

// a hit record of a query: the point, normal, object kind and matrix
static void Hit_Fill(BYTE* r, const BYTE* hit)
{
    F(Vec3Bits, r, 0) = F(Vec3Bits, hit, 52);
    F(Vec3Bits, r, 12) = F(Vec3Bits, hit, 64);
    int* c = F(int*, hit, 108);
    int k;
    if (c && ((k = c[0]) == 5 || k == 8 || k == 6)) {
        F(int, r, 24) = k;
        F(int, r, 32) = c[2];
        F(int, r, 28) = F(int, hit, 112);
        memcpy(r + 36, hit + 4, 48);
    } else
        F(int, r, 24) = 0;
    BYTE* o = F(BYTE*, hit, 104);
    F(BYTE*, r, 84) = o;
    if (o)
        F(int, r, 88) = F(int, o, 36);
}

extern "C" BYTE* __fastcall HitRec_Construct(BYTE* self, void*, const BYTE* hit)
{
    for (int i = 0; i < 24; i += 4)
        F(int, self, i) = 0;
    for (int k = 0; k < 4; k++)
        Vec3_ZeroThis((Vec3*)(self + 36 + k * 12), 0);
    F(Matrix, self, 36) = g_identity;
    if (hit)
        Hit_Fill(self, hit);
    return self;
}

// remembers the hits on new objects of some kinds (a ring of 50)
extern "C" void __fastcall Hits_Remember(BYTE* self, void*, const BYTE* hits, unsigned n)
{
    BYTE* base = F(BYTE*, self, 40) + 64;
    const BYTE* h = hits + 100;
    for (; n; n--, h += 208) {
        int* o = F(int*, h, 0);
        if (!o || o[0] != 1)
            continue;
        int k = o[1];
        if (k != 1 && k != 5 && k != 8 && k != 9 && k != 10)
            continue;
        unsigned lim = F(unsigned, base, 4);
        if (lim > 50)
            lim = 50;
        unsigned i = 0;
        for (; i < lim; i++) {
            BYTE* rec = base + 36 + i * 92;
            if (F(int, rec, 4) == F(int, h, 8) && F(int, rec, 0) == F(int, h, 12))
                break;
        }
        if (i != lim)
            continue;
        unsigned slot = F(unsigned, base, 4) % 50;
        BYTE r[92];
        for (int j = 0; j < 24; j += 4)
            F(int, r, j) = 0;
        Matrix_IdentityThis((Matrix*)(r + 36), 0);
        Hit_Fill(r, hits);
        BYTE* d = base + slot * 92;
        memcpy(d + 8, r, 92);
        BYTE** next = (BYTE**)(base + (slot + 1) * 92);
        if (!*next) {
            BYTE* w = F(BYTE*, g_view, 488);
            BYTE* v = w ? F(BYTE*, w, 76) : 0;
            *next = v;
            F(int, d, 96) = F(int, v, 36);
        }
        F(int, base, 4)++;
    }
}

extern "C" int __cdecl Glow_Add(int a, const Vec3Bits* p, float b, float r1, float r2, int c, int d, int e, float f)
{
    if (g_glowCount == 100) {
        BYTE* v = g_view;
        BYTE* m;
        int name;
        if (v && (m = F(BYTE*, v, 468)) != 0 && (name = F(int, m, 60)) != 0) {
            sprintf(g_errText69608c, (const char*)g_strBadGlow, name, F(double, v, 2904), F(double, v, 2912),
                    F(double, v, 2920));
            CallC(FUN_5d2ba0, (char*)g_errText69608c, 0);
        }
        return 0;
    }
    BYTE* g = g_glows + g_glowCount * 116;
    F(int, g, 100) = 0;
    F(int, g, 96) = 0;
    F(int, g, 68) = a;
    F(Vec3Bits, g, 20) = *p;
    F(Vec3Bits, g, 56) = *p;
    F(Vec3Bits, g, 104) = *p;
    F(float, g, 72) = b;
    F(float, g, 80) = (float)((double)r2 * r2);
    F(int, g, 88) = d;
    F(int, g, 92) = c;
    F(float, g, 84) = (float)((double)r1 * r1);
    F(int, F(BYTE*, g, 52), 36) = e;
    F(float, g, 76) = f;
    F(BYTE, g, 40) &= 0xfe;
    g_glowCount++;
    return 1;
}

typedef int(__stdcall* LightCall2)(void* dev, int id, void* a);
typedef int(__stdcall* LightCall176)(void* dev, int id, int on);

static int LightSet(int id, BYTE* self) { return (*(LightCall2*)(*(BYTE**)g_lightDevice + 72))(g_lightDevice, id, self); }
static int LightEnable(int id, int on) { return (*(LightCall176*)(*(BYTE**)g_lightDevice + 176))(g_lightDevice, id, on); }

// turns a hardware light on or off as its fade wants
extern "C" int __fastcall Light_Switch(BYTE* self, void*, int on, int keep)
{
    BYTE* s = F(BYTE*, self, 104);
    int id = F(int, self, 116);
    if (F(int, s, 72) > 130) {
        if (on == F(int, self, 112))
            return 0;
        if (!on) {
            int r = LightEnable(id, 0);
            F(int, self, 112) = 0;
            return r;
        }
        int r = LightSet(id, self);
        if (r < 0)
            return r;
        r = LightEnable(F(int, self, 116), 1);
        if (r < 0)
            return r;
        F(int, self, 112) = 1;
        return r;
    }
    if (!on && keep) {
        if (F(int, self, 112) != 1)
            return 0;
    } else {
        int k = CallC<int>(FUN_4ccd50, s, on);
        if (k) {
            Fade_Apply(F(BYTE*, self, 104), (float*)self);
            LightSet(F(int, self, 116), self);
        } else
            Fade_Touch(F(BYTE*, self, 104));
        if (k == F(int, self, 112))
            return 0;
        if (k) {
            F(int, self, 112) = 1;
            g_lightsOn++;
            return LightEnable(F(int, self, 116), 1);
        }
        F(int, self, 112) = 0;
    }
    g_lightsOn--;
    return LightEnable(F(int, self, 116), 0);
}

ASM_PROC(FUN_4ccf30)
ASM_VAR(ErrText, g_errText695d30, $L_695d30)

extern "C" int __cdecl Glow_Add2(int a, const Vec3Bits* p, float b, float r1, float r2, int c, int d, int e,
                                 int g9, int g10, float f)
{
    if (g_glowCount == 100) {
        BYTE* v = g_view;
        BYTE* m;
        int name;
        if (v && (m = F(BYTE*, v, 468)) != 0 && (name = F(int, m, 60)) != 0) {
            sprintf(g_errText695d30, (const char*)g_strBadGlow, name, F(double, v, 2904), F(double, v, 2912),
                    F(double, v, 2920));
            CallC(FUN_5d2ba0, (char*)g_errText695d30, 0);
        }
        return 0;
    }
    BYTE* g = g_glows + g_glowCount * 116;
    F(int, g, 100) = g10;
    F(int, g, 96) = g9;
    F(int, g, 68) = a;
    F(Vec3Bits, g, 20) = *p;
    F(Vec3Bits, g, 56) = *p;
    F(Vec3Bits, g, 104) = *p;
    F(float, g, 72) = b;
    F(float, g, 80) = (float)((double)r2 * r2);
    F(int, g, 88) = d;
    F(int, g, 92) = c;
    F(float, g, 84) = (float)((double)r1 * r1);
    F(int, F(BYTE*, g, 52), 36) = e;
    F(float, g, 76) = f;
    F(BYTE, g, 40) &= 0xfe;
    g_glowCount++;
    return 1;
}

// adds a hardware light (rec: 104 bytes) and turns it on while there are free ones
extern "C" int __fastcall Lights_Add(unsigned* self, void*, const void* rec, int a, int fade)
{
    unsigned n = self[1];
    if (!((double)n < 255.0f) || !fade || !g_view)
        return 0x80004005;
    BYTE* e = (BYTE*)self[3] + n * 120;
    F(unsigned, e, 116) = n;
    F(int, e, 112) = 0;
    memcpy(e, rec, 104);
    F(int, e, 104) = fade;
    if (!(self[2] < self[0])) {
        self[1]++;
        return CallThis<int>(FUN_4ccf30, self, a);
    }
    e = (BYTE*)self[3] + self[1] * 120;
    BYTE* s = F(BYTE*, e, 104);
    int r;
    if (F(int, s, 72) > 130) {
        if (F(int, e, 112) == 1)
            r = 0;
        else {
            r = LightSet(F(int, e, 116), e);
            if (r >= 0) {
                r = LightEnable(F(int, e, 116), 1);
                if (r >= 0)
                    F(int, e, 112) = 1;
            }
        }
    } else {
        int k = CallC<int>(FUN_4ccd50, s, 1);
        if (k) {
            Fade_Apply(F(BYTE*, e, 104), (float*)e);
            LightSet(F(int, e, 116), e);
        } else
            Fade_Touch(F(BYTE*, e, 104));
        if (k == F(int, e, 112))
            r = 0;
        else if (k) {
            F(int, e, 112) = 1;
            g_lightsOn++;
            r = LightEnable(F(int, e, 116), 1);
        } else {
            F(int, e, 112) = 0;
            g_lightsOn--;
            r = LightEnable(F(int, e, 116), 0);
        }
    }
    self[2]++;
    self[1]++;
    return r;
}

// --- effect constructors/destructors with exception frames (the frames only rethrow)

static void EffectTask_Destroy(BYTE* self, void* vtbl)
{
    F(void*, self, 0) = vtbl;
    BYTE* o = F(BYTE*, self, 32);
    if (o) {
        CallVirt(o, 0, 1);
        F(BYTE*, self, 32) = 0;
    }
    BYTE* p = F(BYTE*, self, 36);
    if (p) {
        Particles_Free(p, 0);
        operator delete(p);
        F(BYTE*, self, 36) = 0;
    }
    CallThis(FUN_5d3290, self);
}

static void Vehicle4_SetVtbls(BYTE* self, void* a, void* b, void* c, void* d)
{
    F(void*, self, 0) = a;
    F(void*, self, 16) = b;
    F(void*, self, 272) = c;
    F(void*, self, 392) = d;
}

extern "C" {

// [FUN_4ba060] a new 688-byte effect
BYTE* __cdecl Effect_New4daf40(int a, int b)
{
    BYTE* o = (BYTE*)operator new(688);
    return o ? CallThis<BYTE*>(FUN_4daf40, o, g_view, a, b) : 0;
}

// [FUN_4ba0c0] a new 188-byte effect
BYTE* __cdecl Effect_New4dd2a0(int a, int b, int c, int d)
{
    BYTE* o = (BYTE*)operator new(188);
    return o ? CallThis<BYTE*>(FUN_4dd2a0, o, g_view, a, b, c, d) : 0;
}

// [FUN_4ba820] a pooled 176-byte effect
BYTE* __cdecl Effect_New59bb90(int a, int b, int c, int d)
{
    BYTE* o = CallC<BYTE*>(FUN_579380, 176, g_6f6a84);
    return o ? CallThis<BYTE*>(FUN_59bb90, o, g_view, a, b, c, d, (void*)g_695bf0, g_696208) : 0;
}

// [FUN_4bac90]
BYTE* __cdecl Effect_New59c420(int a, int b)
{
    BYTE* o = CallC<BYTE*>(FUN_579380, 176, g_6f6a8c);
    return o ? CallThis<BYTE*>(FUN_59c420, o, g_view, a, b, 0x3f800000, (void*)g_695bf0, g_696208) : 0;
}

// [FUN_4baf90] a pooled vapor effect, when vapor is enabled
BYTE* __cdecl Effect_NewVapor(int a, int b)
{
    if (!(g_696054 > g_64c258))
        return 0;
    BYTE* o = CallC<BYTE*>(FUN_579380, 180, g_6f6a98);
    return o ? CallThis<BYTE*>(FUN_59d4a0, o, g_view, a, b, (void*)g_vaporMats, g_vaporCount) : 0;
}

// [FUN_4bc4d0] a new 60-byte effect, started at once
void __cdecl Effect_Start4d9460(int a)
{
    BYTE* o = (BYTE*)operator new(60);
    BYTE* e = o ? CallThis<BYTE*>(FUN_4d9460, o, g_view, a) : 0;
    CallVirt(e, 24, 1);
}

// [FUN_4bedb0]
void __fastcall Effect4bedb0_Destroy(BYTE* self, void*) { EffectTask_Destroy(self, g_vtbl64c428); }

// [FUN_4bf3f0]
void __fastcall Effect4bf3f0_Destroy(BYTE* self, void*) { EffectTask_Destroy(self, g_vtbl64c464); }

// [FUN_4c09d0]
void __fastcall Effect4c09d0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64c498;
    F(int, self, 68) = 0;
    F(int, self, 64) = 0;
    CallThis(FUN_5ef780, self);
    CallThis(FUN_5f1a70, self);
}

// [FUN_4c4c40]
void __fastcall Effect4c4c40_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64c770;
    BYTE* o = F(BYTE*, self, 32);
    if (o) {
        CallVirt(o, 0, 3);
        F(BYTE*, self, 32) = 0;
    }
    F(int, self, 40) = 0;
    F(int, self, 36) = 0;
    CallThis(FUN_5d3290, self);
}

// [FUN_4c8890]
void __fastcall Effect4c8890_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64c8b4;
    CallThis(FUN_5ef780, self);
    if (F(void*, self, 72))
        operator delete(F(void*, self, 72));
    CallThis(FUN_5ef660, self);
}

// [FUN_4c9970] a task on the vehicle v
BYTE* __fastcall Effect4c9970_Construct(BYTE* self, void*, BYTE* v, int b)
{
    CallThis(FUN_5d31e0, self, 98, (const char*)s_6728dc);
    F(int, self, 36) = b;
    F(void*, self, 0) = g_vtbl64c90c;
    F(BYTE*, self, 32) = v;
    CallThis(FUN_5d3570, v + 2992, self);
    CallThis(FUN_5d33d0, self, 1);
    return self;
}

// [FUN_4cb4d0] the shared 6436-byte effect, made on first use
int __cdecl Effect_Shared6436()
{
    if (!g_6f6a78) {
        BYTE* o = (BYTE*)operator new(6436);
        g_6f6a78 = o ? (int)CallThis<BYTE*>(FUN_5a0000, o, g_view) : 0;
    }
    return g_6f6a78;
}

// [FUN_4cc850] creates the 13300-byte node and hands it to obj
void __cdecl Effect_AttachNode(BYTE* obj)
{
    if (!obj)
        g_6961fc = 0;
    BYTE* o = (BYTE*)operator new(13300);
    if (o) {
        CallThis(FUN_5f19b0, o, (const char*)s_67290c, 5, 1);
        F(int, o, 56) = 0;
        F(int, o, 60) = 0;
        F(void*, o, 0) = g_vtbl64c97c;
        F(int, o, 13296) = 0;
    }
    g_6961fc = o;
    CallVirt(obj, 20, 0, o);
}

// [FUN_4cc9f0]
void __fastcall Effect4cc9f0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64c97c;
    CallThis(FUN_5ef780, self);
    F(int, self, 13296) = 0;
    CallThis(FUN_5f1a70, self);
}

// [FUN_4ce0d0] a task on the vehicle v with two named objects
BYTE* __fastcall Effect4ce0d0_Construct(BYTE* self, void*, BYTE* v, int b, int c)
{
    CallThis(FUN_5d3110, self, (const char*)s_672988);
    F(void*, self, 0) = g_vtbl64ca58;
    F(BYTE*, self, 32) = v;
    F(int, self, 40) = CallC<int>(FUN_5ef2b0, 0, b);
    F(int, self, 44) = CallC<int>(FUN_5ef2b0, 0, c);
    CallThis(FUN_5d3570, F(BYTE*, self, 32) + 2992, self);
    F(int, self, 36) = 0;
    return self;
}

// [FUN_4d0fd0]
void __fastcall Vehicle4d0fd0_Destroy(BYTE* self, void*)
{
    Vehicle4_SetVtbls(self, g_vtbl64cdf8, g_vtbl64cd0c, g_vtbl64ccc0, g_vtbl64ccbc);
    CallC(FUN_4cf400, self);
    CallThis(FUN_544df0, self);
}

// [FUN_4d12f0]
void __fastcall Vehicle4d12f0_Destroy(BYTE* self, void*)
{
    Vehicle4_SetVtbls(self, g_vtbl64cff4, g_vtbl64cf08, g_vtbl64cebc, g_vtbl64ceb8);
    CallC(FUN_4cf400, self);
    CallThis(FUN_544df0, self);
}

// [FUN_4ce2e0]
int __cdecl Effect_Call446c80(const BYTE* x)
{
    return CallC<int>(FUN_446c80, F(int, x, 20636));
}

}

extern "C" {

// [FUN_4b81c0] adds a new 264-byte light to the effect list (up to 255)
void __cdecl EffectList_AddLight(const DWORD* a, int b, int c, const DWORD* d, int e, int g, const void* params)
{
    BYTE* lights = g_effectList;
    if (!lights)
        return;
    BYTE* o = (BYTE*)operator new(264);
    if (o) {
        F(void*, o, 0) = g_vtbl64c244;
        F(int, o, 4) = 0;
        F(int, o, 8) = 0;
        F(int, o, 12) = 0;
        F(int, o, 16) = 0;
        F(int, o, 28) = 0;
        F(int, o, 32) = 0;
        F(int, o, 36) = 0;
        CallThis(FUN_5ef5c0, o + 48);
        F(void*, o, 0) = g_vtbl64c240;
        F(DWORD, o, 8) = a[0];
        F(DWORD, o, 12) = a[1];
        F(DWORD, o, 16) = a[2];
        F(int, o, 20) = b;
        F(int, o, 24) = c;
        F(DWORD, o, 28) = d[0];
        F(DWORD, o, 32) = d[1];
        F(DWORD, o, 36) = d[2];
        F(int, o, 40) = e;
        F(int, o, 44) = g;
        if (params) {
            memcpy(o + 48, params, 208);
            F(int, o, 256) = 1;
        } else {
            F(int, o, 256) = 0;
        }
        F(int, o, 4) = 1;
    }
    lights = g_effectList;
    unsigned n = F(unsigned, lights, 36);
    if (n < 255) {
        F(BYTE**, lights, 32)[n] = o;
        F(unsigned, lights, 36) = n + 1;
    }
}

static BYTE* Decals50_Construct(BYTE* self, void* vtbl)
{
    F(void*, self, 0) = g_vtbl64c63c;
    F(int, self, 4) = 0;
    BYTE* e = self + 8;
    for (int i = 0; i < 50; i++, e += 92) {
        memset(e, 0, 36);
        CallStd(FUN_4011e0, e + 36, 12, 4, FUN_4bd4d0);
        memcpy(e + 36, g_7025f0, 48);
    }
    F(void*, self, 0) = vtbl;
    return self;
}

// [FUN_4c16b0] 50 decal slots
BYTE* __fastcall Decals4c16b0_Construct(BYTE* self, void*) { return Decals50_Construct(self, g_vtbl64c628); }

// [FUN_4c17d0]
BYTE* __fastcall Decals4c17d0_Construct(BYTE* self, void*) { return Decals50_Construct(self, g_vtbl64c650); }

// [FUN_4c1d40]
BYTE* __fastcall Decals4c1d40_Construct(BYTE* self, void*) { return Decals50_Construct(self, g_vtbl64c668); }

// [FUN_4cfc30] replaces slot i (0 or 1) with a copy of the table src
void __fastcall Tables2_Set(BYTE** self, void*, const BYTE* src, unsigned i)
{
    if (i >= 2)
        return;
    BYTE* old = self[i];
    if (old) {
        if (F(void*, old, 12))
            operator delete(F(void*, old, 12));
        operator delete(old);
    }
    if (!src) {
        self[i] = 0;
        return;
    }
    BYTE* n = (BYTE*)operator new(16);
    if (n) {
        F(DWORD, n, 0) = F(DWORD, src, 0);
        F(DWORD, n, 4) = F(DWORD, src, 4);
        F(DWORD, n, 8) = F(DWORD, src, 8);
        void* p = operator new(F(int, n, 8) * 4);
        F(void*, n, 12) = p;
        memcpy(p, F(void*, src, 12), F(int, n, 8) * 4);
    }
    self[i] = n;
}

}

// --- effect spawners: a random number in [0.1, 1.1) against a frequency

extern "C" int __cdecl rand();

static double Fx_Random()
{
    return (double)rand() * g_randScale + g_randBase;
}

extern "C" {

// [FUN_4ba520] spawns one of the three 192-byte effects (kind 0..2)
void __cdecl Fx_SpawnByKind(int a1, int a2, int a3, int a4, int a5, int a6, int kind)
{
    double r = Fx_Random();
    if ((double)g_one_f - g_696074 >= r)
        return;
    void* const tables[3] = {(void*)g_695bf0, (void*)&g_vaporMats, (void*)g_694998};
    int counts[3] = {g_696208, g_vaporCount, g_696210};
    if (kind < 0 || kind > 2)
        return;
    BYTE* o = CallC<BYTE*>(FUN_579380, 192, g_6f6a80);
    if (o)
        CallThis(FUN_59aea0, o, g_view, a1, a2, a3, a4, tables[kind], counts[kind], a6, a5);
}

// [FUN_4baae0] spawns a vapor or a smoke puff
void __cdecl Fx_SpawnPuff(int a1, int a2, float threshold)
{
    double r = Fx_Random();
    if ((double)g_one_f - g_696078 >= r)
        return;
    double r2 = (double)rand() * g_randScale;
    if (r2 > threshold) {
        BYTE* o = CallC<BYTE*>(FUN_579380, 192, g_6f6a7c);
        if (o)
            CallThis(FUN_59bed0, o, g_view, a1, a2, (void*)&g_vaporMats, g_vaporCount, 0x3fc00000);
    } else {
        BYTE* o = CallC<BYTE*>(FUN_579380, 192, g_6f6a80);
        if (o)
            CallThis(FUN_59ad70, o, g_view, a1, a2, (void*)g_695bf0, g_696208, 0x3fc00000);
    }
}

// [FUN_4bc160] spawns a puff and, sometimes, a spark
void __cdecl Fx_SpawnPuffSpark(int a1, int a2)
{
    float f = (float)Fx_Random();
    double r2 = (double)rand() * g_randScale;
    if (r2 > f) {
        BYTE* o = CallC<BYTE*>(FUN_579380, 192, g_6f6a7c);
        if (o)
            CallThis(FUN_59bed0, o, g_view, a1, a2, (void*)&g_vaporMats, g_vaporCount, 0x3f400000);
    } else {
        BYTE* o = CallC<BYTE*>(FUN_579380, 192, g_6f6a80);
        if (o)
            CallThis(FUN_59ad70, o, g_view, a1, a2, (void*)g_695bf0, g_696208, 0x3f400000);
    }
    if (f > g_64c258) {
        BYTE* o = CallC<BYTE*>(FUN_579380, 212, g_6f6a88);
        if (o)
            CallThis(FUN_59b3f0, o, g_view, a1, a2, (void*)g_694998, g_696210, 0x3f800000, 0x3f000000);
    }
}

}

// ---------------------------------------------------------------------------
// Constructors and destructors with exception frames (frames dropped).

ASM_PROC(FUN_5587d0)
ASM_PROC(FUN_5e48f0)
ASM_VAR(char, g_vtbl64d234, $L_64d234)
ASM_VAR(char, g_vtbl64d148, $L_64d148)
ASM_VAR(char, g_vtbl64d0fc, $L_64d0fc)
ASM_VAR(char, g_vtbl64d0f8, $L_64d0f8)
ASM_VAR(char, g_vtbl64d424, $L_64d424)
ASM_VAR(char, g_vtbl64d4ec, $L_64d4ec)
ASM_VAR(char, g_vtbl64d530, $L_64d530)
ASM_VAR(BYTE*, g_696734, $L_696734)
ASM_VAR(char, g_702650, $L_702650)

typedef void (__cdecl* NotifyFn)(void* a, int b, void* c);

extern "C" {

// [FUN_4d25f0] destructor
void __fastcall Effect4d25f0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vtbl64d234;
    F(void*, self, 16) = &g_vtbl64d148;
    F(void*, self, 272) = &g_vtbl64d0fc;
    F(void*, self, 392) = &g_vtbl64d0f8;
    CallC(FUN_4cf400, self);
    CallThis(FUN_5587d0, self);
}

// [FUN_4d9e70] destructor
void __fastcall Effect4d9e70_Destroy(BYTE* self, void*)
{
    CallThis(FUN_5e48f0, self + 56);
    CallThis(FUN_5f1a70, self);
}

// [FUN_4d9ec0] destructor: also deletes the shared object at $L_696734
void __fastcall Task4d9ec0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vtbl64d424;
    if (g_696734) {
        CallVirt(g_696734, 36);
        if (g_696734)
            CallVirt(g_696734, 0, 1);
        g_696734 = 0;
    }
    CallThis(FUN_5d3290, self);
}

// [FUN_4dbe60] constructor of the TankNotify task
BYTE* __fastcall TankNotify_Construct(BYTE* self, void*, BYTE* owner, int fn, int arg, int d)
{
    CallThis(FUN_5d3110, self, "TankNotify");
    F(int, self, 44) = 0;
    F(int, self, 48) = d;
    F(int, self, 32) = fn;
    F(void*, self, 0) = &g_vtbl64d4ec;
    F(BYTE*, self, 40) = owner;
    F(int, self, 36) = arg;
    CallThis(FUN_5d3570, owner + 2992, self);
    CallThis(FUN_5d33d0, self, 1);
    return self;
}

// [FUN_4dbf30] destructor of the TankNotify task: calls its function
void __fastcall TankNotify_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vtbl64d4ec;
    NotifyFn f = F(NotifyFn, self, 32);
    if (f)
        f(&g_702650, 1, F(void*, self, 36));
    CallThis(FUN_5d3290, self);
}

// [FUN_4dc860] destructor: calls its function
void __fastcall Notify4dc860_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vtbl64d530;
    NotifyFn f = F(NotifyFn, self, 32);
    if (f)
        f(F(void*, self, 40), 1, F(void*, self, 36));
    CallThis(FUN_5d3290, self);
}

}

ASM_PROC(FUN_55eaa0)
ASM_PROC(FUN_543e00)
extern "C" void FUN_4cf260(); KING_ALIAS("_FUN_4cf260", "_Fx_AttachVehicle")
ASM_PROC(FUN_556110)
ASM_PROC(FUN_5586f0)
ASM_PROC(FUN_5fd870)    // sound: play (position by value, ...)
extern "C" void FUN_4dd9c0(); KING_ALIAS("_FUN_4dd9c0", "@CopySound_Construct@12")
ASM_VAR(char, g_vtbl64cbfc, $L_64cbfc)
ASM_VAR(char, g_vtbl64cb10, $L_64cb10)
ASM_VAR(char, g_vtbl64cac4, $L_64cac4)
ASM_VAR(char, g_vtbl64cac0, $L_64cac0)

static void SetVtbls4(BYTE* self, void* a, void* b, void* c, void* d)
{
    F(void*, self, 0) = a;
    F(void*, self, 16) = b;
    F(void*, self, 272) = c;
    F(void*, self, 392) = d;
}

extern "C" {

// [FUN_4d0b10] constructor
BYTE* __fastcall Effect4d0b10_Construct(BYTE* self, void*, int a1, int a2, int a3)
{
    CallThis(FUN_55eaa0, self, a1, a2, a3, 0);
    SetVtbls4(self, &g_vtbl64cbfc, &g_vtbl64cb10, &g_vtbl64cac4, &g_vtbl64cac0);
    CallThis(FUN_5d33d0, self + 16, 1);
    return self;
}

// [FUN_4d0e30] constructor
BYTE* __fastcall Effect4d0e30_Construct(BYTE* self, void*, int a1, int a2, int a3)
{
    CallThis(FUN_543e00, self, a1, a2, a3, 0);
    SetVtbls4(self, &g_vtbl64cdf8, &g_vtbl64cd0c, &g_vtbl64ccc0, &g_vtbl64ccbc);
    CallC(FUN_4cf260, self, a2, a3);
    CallThis(FUN_556110, self);
    return self;
}

// [FUN_4d2530] constructor (its destructor is FUN_4d25f0)
BYTE* __fastcall Effect4d25f0_Construct(BYTE* self, void*, int a1, int a2, int a3)
{
    CallThis(FUN_5586f0, self, a1, a2, a3, 0);
    SetVtbls4(self, &g_vtbl64d234, &g_vtbl64d148, &g_vtbl64d0fc, &g_vtbl64d0f8);
    CallC(FUN_4cf260, self, a2, a3);
    return self;
}

// [FUN_4dc780] constructor of the BenzoNotify task
BYTE* __fastcall BenzoNotify_Construct(BYTE* self, void*, BYTE* owner, int a2, int fn, int a4, int a5)
{
    CallThis(FUN_5d3110, self, "BenzoNotify");
    F(int, self, 48) = 0;
    F(int, self, 52) = a5;
    F(int, self, 40) = a2;
    F(int, self, 32) = fn;
    F(void*, self, 0) = &g_vtbl64d530;
    F(int, self, 36) = a4;
    F(BYTE*, self, 44) = owner;
    CallThis(FUN_5d3570, owner + 2992, self);
    CallThis(FUN_5d33d0, self, 1);
    return self;
}

// [FUN_4dd920] plays a copy of the sound at +0 (if any)
BYTE* __fastcall Sound4dd920_Play(BYTE** self, void*, int p0, int p1, int p2, int a4, int a5, int a6, int a7)
{
    if (!self[0])
        return 0;
    BYTE* o = (BYTE*)operator new(32);
    BYTE* s = o ? CallThis<BYTE*>(FUN_4dd9c0, o, self[0]) : 0;
    return CallThis<BYTE*>(FUN_5fd870, s, p0, p1, p2, a4, a5, a6, a7);
}

}

ASM_VAR(char, g_vtbl64c358, $L_64c358)

extern "C" {

// [FUN_4bc950] attaches the "_TDTN" node to the object a (creating it when
// the object has none)
void __fastcall Effect4bc950_Attach(BYTE* self, void*, BYTE* a)
{
    if (F(BYTE*, self, 32) == a)
        return;
    F(BYTE*, self, 32) = a;
    BYTE* o = CallC<BYTE*>(FUN_5ef2b0, a, "_TDTN");
    F(BYTE*, self, 36) = o;
    if (o) {
        CallVirt(F(BYTE*, self, 32), 20, 0, o);
        return;
    }
    BYTE* n = (BYTE*)operator new(256);
    if (!n) {
        F(BYTE*, self, 36) = 0;
        return;
    }
    BYTE* parent = F(BYTE*, self, 32);
    CallThis(FUN_5f19b0, n, "_TDTN", 5, 1);
    F(int, n, 56) = 0;
    F(int, n, 60) = 0;
    F(void*, n, 0) = &g_vtbl64c358;
    if (parent)
        CallVirt(parent, 20, 0, n);
    F(BYTE, n, 40) &= 0xfe;
    F(int, n, 20) = 0;
    F(int, n, 32) = 0;
    F(int, n, 24) = 0;
    F(int, n, 28) = 0;
    for (int i = 0; i < 2; i++) {
        F(int, n, 64 + i * 4) = F(int, self, 24 + i * 4);
        F(int, n, 248 + i * 4) = 0;
    }
    F(BYTE*, self, 36) = n;
}

}

extern "C" void FUN_4c1790(); KING_ALIAS("_FUN_4c1790", "@EffectLink_Init@8")
extern "C" void FUN_43fc40(); KING_ALIAS("_FUN_43fc40", "_Ai_GetPlayerData")    // vehicle info by id (id, info)
ASM_PROC(FUN_4ebd00)
extern "C" void FUN_4ebaa0(); KING_ALIAS("_FUN_4ebaa0", "@Engine_SetKeys@16")
ASM_VAR(char, gv64c604, $L_64c604)
ASM_VAR(char, gv64c5f0, $L_64c5f0)
ASM_VAR(char, gv64c59c, $L_64c59c)
ASM_VAR(char, gv64d2f4, $L_64d2f4)

// deletes a renderer helper object (buffer at +56) after setting vtbls
static void Helper_Delete(BYTE* o)
{
    F(void*, o, 0) = g_vtbl64c72c;
    if (F(void*, o, 56)) {
        operator delete(F(void*, o, 56));
        F(void*, o, 56) = 0;
    }
    operator delete(o);
}

static void Helper2_Delete(BYTE* o)
{
    F(void*, o, 64) = g_vtbl64c63c;
    F(int, o, 68) = 0;
    Helper_Delete(o);
}

extern "C" {

// [FUN_4c45d0] destructor
void __fastcall Effect4c45d0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &gv64c604;
    BYTE* a = F(BYTE*, self, 40);
    if (a) {
        F(void*, a, 4672) = g_vtbl64c668;
        CallThis(FUN_4c1790, a + 4672);
        F(void*, a, 64) = &gv64c5f0;
        CallThis(FUN_4c1790, a + 64);
        Helper_Delete(a);
        F(BYTE*, self, 40) = 0;
    }
    if (F(BYTE*, self, 48)) {
        Helper2_Delete(F(BYTE*, self, 48));
        F(BYTE*, self, 48) = 0;
    }
    BYTE* c = F(BYTE*, self, 44);
    if (c) {
        F(void*, c, 0) = &gv64c59c;
        if (F(BYTE*, c, 64)) {
            CallVirt(F(BYTE*, c, 64), 8, 1);
            F(BYTE*, c, 64) = 0;
        }
        Helper_Delete(c);
        F(BYTE*, self, 44) = 0;
    }
    if (F(BYTE*, self, 32)) {
        CallVirt(F(BYTE*, self, 32), 0, 1);
        F(BYTE*, self, 32) = 0;
    }
    if (F(BYTE*, self, 36)) {
        CallVirt(F(BYTE*, self, 36), 0, 1);
        F(BYTE*, self, 36) = 0;
    }
    if (F(BYTE*, self, 52)) {
        Helper2_Delete(F(BYTE*, self, 52));
        F(BYTE*, self, 52) = 0;
    }
    CallThis(FUN_5d3290, self);
}

// [FUN_4d1140] constructor of the police car (light and siren keys)
BYTE* __fastcall CopCar_Construct(BYTE* self, void*, int a1, int a2, int a3)
{
    CallThis(FUN_543e00, self, a1, a2, a3, 0);
    for (int i = 0; i < 6; i++)
        F(int, self, 21900 + i * 4) = 0;
    SetVtbls4(self, g_vtbl64cff4, g_vtbl64cf08, g_vtbl64cebc, g_vtbl64ceb8);
    CallThis(FUN_556110, self);
    for (int i = 0; i < 7; i++)
        F(int, self, 21924 + i * 4) = 0;
    F(BYTE*, self, 21884) = CallC<BYTE*>(FUN_5ef2b0, F(BYTE*, self, 336), "CopLightKey");
    F(BYTE*, self, 21888) = CallC<BYTE*>(FUN_5ef2b0, F(BYTE*, self, 336), "CopSirenKey");
    F(BYTE*, self, 21892) = CallC<BYTE*>(FUN_5ef2b0, 0, "clRedSpace");
    BYTE info[192];
    CallC(FUN_43fc40, a3, info);
    F(int, self, 21880) = F(int, info, 8);
    CallC(FUN_4cf260, self, a2, a3);
    return self;
}

// [FUN_4d2790] constructor
BYTE* __fastcall Effect4d2790_Construct(BYTE* self, void*, int a, int b, DWORD d0, DWORD d1, DWORD d2,
                                        DWORD d3, DWORD d4, DWORD d5, DWORD d6, DWORD d7, DWORD d8,
                                        DWORD d9, DWORD d10, DWORD d11)
{
    CallThis(FUN_4ebd00, self, b, d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, a % 1000);
    static const int zero1[] = {16464, 16468, 16472, 16568, 16440, 16448, 16432, 16424, 16572, 16444,
                                16452, 16428, 16492, 16488, 16496, 16500};
    for (int i = 0; i < 16; i++)
        F(int, self, zero1[i]) = 0;
    F(void*, self, 0) = &gv64d2f4;
    F(int, self, 10088) = F(int, g_6cec6c, 1304) == 0;
    CallThis(FUN_4ebaa0, self + 9984, 1, 1);
    for (int off = 16520; off <= 16548; off += 4)
        F(int, self, off) = 0;
    int r = rand() % 4;
    F(int, self, 16508) = r;
    F(int, self, 16552) = 0;
    F(int, self, 16504) = 0;
    F(int, self, 16516) = -1;
    F(int, self, 16512) = -1;
    F(int, self, 16484) = 0;
    F(int, self, 16476) = r * 2;
    F(DWORD, self, 16556) = 0x400c0000;
    F(int, self, 16480) = 10;
    return self;
}

}

// ---------------------------------------------------------------------------
// x87 functions of the unit

#include "x87.h"

ASM_VAR(BYTE*, g_resources696ca0, $L_696ca0)
extern "C" void FUN_439320(); KING_ALIAS("_FUN_439320", "_Ai_GetTangent")
struct Bytes32 { DWORD d[8]; };

// [FUN_4cd730] a vehicle's value by id: its object's virtual +84, or the float at +244 of its record
extern "C" double __cdecl Vehicle_Value4cd730(int id)
{
    BYTE* r = CallThis<BYTE*>(FUN_5e5370, g_resources696ca0 + 704, id);
    if (!r)
        return 0.0;
    BYTE* o = F(BYTE*, r, 28);
    if (o)
        return CallVirt<double>(o, 84);
    return F(float, F(BYTE*, r, 24), 244);
}

// [FUN_4cda20] a direction from a 32-byte position (negated if asked), flattened and normalized; its slope angle to *angle
extern "C" Vec3* __cdecl Effect_Direction4cda20(Vec3* out, Bytes32 pos, int negate, double* angle)
{
    CallC(FUN_439320, &pos, pos);
    Vec3* v = (Vec3*)&pos;
    if (negate) {
        float x = -v->x, y = -v->y, z = -v->z;
        v->x = x; v->y = y; v->z = z;
    }
    Vec3 t;
    if (angle)
        *angle = x87_asin(Vec3_Normalized(&t, v)->z);
    v->z = 0;
    *v = *Vec3_Normalized(&t, v);
    *out = *v;
    return out;
}

// [FUN_4cee70] whether a vehicle is too far from the player for an effect (over 300), or over 100 and standing still
extern "C" int __cdecl Effect_TooFar4cee70(BYTE* car)
{
    Matrix m1, m2;
    BYTE* a = (BYTE*)SceneObj_Matrix1(F(BYTE*, g_game, 616) + 16, 0, &m1);
    BYTE* b = (BYTE*)SceneObj_Matrix1(car + 16, 0, &m2);
    Vec3 d;
    d.x = (float)((double)F(float, a, 36) - F(float, b, 36));
    d.y = (float)((double)F(float, a, 40) - F(float, b, 40));
    d.z = (float)((double)F(float, a, 44) - F(float, b, 44));
    double len = Vec3_Length(&d);
    if (len > 300.0)
        return 1;
    if (!(len >= 100.0))
        return 0;
    BYTE* e = F(BYTE*, car, 21600);
    double sp = F(double, e, 16568);
    if (sp < 0) sp = -sp;
    if (sp > 0.5)
        return 0;
    if (F(BYTE, e, 16496) & 2)
        return 0;
    return 1;
}

extern "C" double __cdecl HueComponent(float p, float q, float h);

// [FUN_4be7f0] HSL to RGB: hue h (degrees), lightness l, saturation s
extern "C" void __cdecl Color_FromHsl(float h, float l, float s, float* r, float* g, float* b)
{
    float q;
    if (l > 0.5f)
        q = (float)(((double)l + s) - (double)l * s);
    else
        q = (float)(((double)s + 1.0f) * l);
    float p = (float)(((double)l + l) - q);
    if (!(s < 0 || s > 0)) {     // equal or unordered
        *r = l; *g = l; *b = l;
        return;
    }
    *r = (float)HueComponent(p, q, (float)((double)h + 120.0f));
    *g = (float)HueComponent(p, q, h);
    *b = (float)HueComponent(p, q, (float)((double)h - 120.0f));
}

// [FUN_4dc910] a segment raised to 4 m when the point 3/4 along it is in range; else the segment unchanged
extern "C" void __stdcall Fx_RaiseSegment(const Vec3* a, const Vec3* b, Vec3* outA, Vec3* outB)
{
    double t = (double)b->z * 0.75f;
    if (t + a->z >= 4.0f) {
        double x = (double)b->x * 0.75f;
        float y = (float)((double)b->y * 0.75f);
        float z = (float)t;
        Vec3 v;
        v.x = (float)(x + a->x);
        v.y = (float)((double)y + a->y);
        v.z = (float)((double)z + a->z);
        if (Fx_InRange((const float*)&v)) {
            outA->x = a->x; outA->y = a->y; outA->z = 4.0f;
            outB->x = b->x; outB->y = b->y; outB->z = 0;
            return;
        }
    }
    *outA = *a;
    *outB = *b;
}

extern "C" BYTE* __fastcall Fx_SetVtbl89c(BYTE* s, void*);
extern "C" void __fastcall RefStack_Push(BYTE* s, void*, void* p);
extern "C" void* __fastcall RefStack_Pop(BYTE* s, void*);

// [FUN_4c89d0] a new 2220-byte effect at a point, its detail by the distance to the camera; dropped again if it fails
extern "C" void __fastcall Fx_SpawnAt(BYTE* self, void*, int a, int b, const Vec3* at)
{
    BYTE* o = (BYTE*)operator new(2220);
    if (o)
        o = Fx_SetVtbl89c(o, 0);
    RefStack_Push(F(BYTE*, self, 76), 0, o);
    const Vec3* cam = (const Vec3*)(g_game + 1460);
    Vec3 d;
    d.x = (float)((double)at->x - cam->x);
    d.y = (float)((double)at->y - cam->y);
    d.z = (float)((double)at->z - cam->z);
    double len = Vec3_Length(&d);
    int level;
    if (!(len >= 20.0f)) level = 2;
    else if (!(len >= 50.0f)) level = 1;
    else if (!(len >= 90.0f)) level = 0;
    else level = 2;
    if (!CallVirt<int>(o, 4, a, b, at, level)) {
        void* x = RefStack_Pop(F(BYTE*, self, 76), 0);
        if (x)
            CallVirt(x, 0, 1);
    }
}

extern "C" int __cdecl rand(void);

// [FUN_4b84f0] a random point in the triangle a, b, c
extern "C" Vec3* __cdecl Triangle_RandomPoint(Vec3* out, const Vec3* a, const Vec3* b, const Vec3* c)
{
    const float k = 3.0518509447574615e-05f;    // 1/32767
    float u = (float)((double)rand() * k);
    double v = (double)rand() * k;
    if (v + u > 1.0f) {
        u = (float)(1.0f - (double)u);
        v = 1.0f - v;
    }
    double w = (1.0f - (double)u) - v;
    float wcx = (float)(w * c->x), wcy = (float)(w * c->y), wcz = (float)(w * c->z);
    float vbx = (float)(v * b->x), vby = (float)(v * b->y);
    double vbz = v * b->z;
    float uay = (float)((double)u * a->y), uaz = (float)((double)u * a->z);
    float sx = (float)((double)u * a->x + vbx);
    float sy = (float)((double)vby + uay);
    out->x = (float)((double)sx + wcx);
    out->y = (float)((double)wcy + sy);
    out->z = (float)((double)wcz + (vbz + uaz));
    return out;
}

typedef float FireRamp[33];
ASM_VAR(FireRamp, g_fireRamp1, $L_695fac)
ASM_VAR(FireRamp, g_fireRamp2, $L_695324)
ASM_VAR(FireRamp, g_fireRamp3, $L_695e2c)

// [FUN_4bb390] three random brightness ramps of 32 steps (runs of 1..5 steps between random levels)
extern "C" void __cdecl Fire_RandomRamps()
{
    const float k1 = 0.00012207403779029846f, k2 = 2.288888208568096e-05f;
    int len = (int)((double)rand() * k1 + 1.0f);
    unsigned left = len;
    float from = (float)((double)rand() * k2 + 0.25f);
    float first = from;
    float to = (float)((double)rand() * k2 + 0.25f);
    for (unsigned i = 0; i < 32; ) {
        if (left == 0) {
            len = (int)((double)rand() * k1 + 1.0f);
            left = len;
            if ((unsigned)len >= 32 - i) {
                len = 32 - i;
                left = len;
                from = first;
            } else
                from = to;
            to = (float)((double)rand() * k2 + 0.25f);
        }
        double t = (double)(__int64)left / len;
        left--;
        i++;
        double v = (1.0f - t) * to + t * from;
        g_fireRamp1[i] = (float)v;
        g_fireRamp2[i] = (float)(v * 0.6f);
        g_fireRamp3[i] = (float)(v * 0.3f);
    }
}

extern "C" double __fastcall Car_BestGearNow(BYTE* car, void*);

// [FUN_4d74e0] the steering deflection shown for an input x, damped at speed (over 10)
extern "C" void __fastcall Car_SetSteerShown(BYTE* s, void*, double x)
{
    double a = x * 1.2732395664542882;
    if (a > 1.0) a = 1.0;
    else if (!(a >= -1.0)) a = -1.0;
    double v = a;
    if (!F(int, s, 10060) && F(float, s, 13100) >= 10.0f) {
        double sp = F(float, s, 13100);
        if (Car_BestGearNow(s, 0) * 0.7 > sp) {
            double y = ((double)F(float, s, 13100) - 10.0f) * 0.5;
            v = (1.0 - y / (Car_BestGearNow(s, 0) * 0.7 - 10.0)) * a;
        } else
            v = a * 0.5;
        if (!(v >= -0.4)) v = -0.4;
        else if (v > 0.4) v = 0.4;
    }
    if (!(F(float, s, 13104) >= 0.0f))
        v = -v;
    F(float, s, 10012) = (float)v;
}

ASM_VAR(const double, k_hslEpsilon, $L_64c410)      // 1e-7

// [FUN_4be5e0] RGB to HSL: hue in degrees (FLT_MAX for grey), lightness, saturation
extern "C" void __cdecl Color_ToHsl(float r, float g, float b, float* h, float* l, float* s)
{
    float mx = Max3f(r, g, b);
    float mn = Min3f(r, g, b);
    float d = (float)((double)mx - mn);
    float sum = (float)((double)mn + mx);
    double light = (double)sum * 0.5f;
    *l = (float)light;
    double ad = d < 0 ? -(double)d : (double)d;
    if (!(ad > k_hslEpsilon)) {
        *(DWORD*)h = 0x7F7FFFFF;
        *s = 0;
        return;
    }
    if (!(light >= 0.5f))
        *s = (float)((double)d / sum);
    else
        *s = (float)((double)d / ((2.0f - (double)mx) - mn));
    double rc = ((double)mx - r) / d;
    float gc = (float)(((double)mx - g) / d);
    float bc = (float)(((double)mx - b) / d);
    if (!(r < mx || r > mx))
        *h = (float)((double)bc - gc);
    else if (!(g < mx || g > mx))
        *h = (float)((rc + 2.0f) - bc);
    else
        *h = (float)(((double)gc + 4.0f) - rc);
    double hd = (double)*h * 60.0f;
    *h = (float)hd;
    if (!(hd >= 0.0f))
        *h = (float)(hd + 360.0f);
}

extern "C" void FUN_437520(); KING_ALIAS("_FUN_437520", "_Ai_GetPositionId")
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")    // a vehicle's flags by id
extern "C" void FUN_438d70(); KING_ALIAS("_FUN_438d70", "_Road_GetRoadParameters")
extern "C" void FUN_43a780(); KING_ALIAS("_FUN_43a780", "_Ai_GetOrientationF")    // the matrix of a position
ASM_VAR(Vec3, g_up702640, $L_702640)
struct Bytes32b { DWORD d[8]; };

// the shared tail of FUN_4d43e0 and FUN_4d4520: the vehicle's position matrix lifted by (level + 0.5) * 3.5 along the up axis
static Matrix* Fx_LiftedMatrix(Matrix* out, const Bytes32b* pos, int level)
{
    double h = ((double)level + 0.5) * 3.5;
    Vec3 v;
    v.x = (float)(g_up702640.x * h);
    v.y = (float)(g_up702640.y * h);
    v.z = (float)(g_up702640.z * h);
    Matrix m;
    Matrix* r = CallC<Matrix*>(FUN_43a780, &m, *pos);
    Matrix t;
    Matrix* tr = Matrix_Translation(&t, v);
    return Matrix_Multiply(out, tr, r);
}

// [FUN_4d4520] a vehicle's matrix lifted by its effect level
extern "C" Matrix* __cdecl Fx_VehicleMatrix(Matrix* out, BYTE* car)
{
    Bytes32b pos;
    CallC(FUN_437520, &pos, car);
    int level = 0;
    if ((CallC<int>(FUN_43f620, car) & 0x4000) || F(int, g_6cec6c, 1316))
        level = Fx_EvenStep(F(int, car, 728));
    return Fx_LiftedMatrix(out, &pos, level);
}

// [FUN_4d43e0] the same, the level from a height (+8 of h) when the vehicle has no fixed one
extern "C" Matrix* __cdecl Fx_VehicleMatrixAt(Matrix* out, BYTE* car, BYTE* h)
{
    Bytes32b pos;
    CallC(FUN_437520, &pos, car);
    CallC(FUN_438d70, pos, h);
    int level = 0;
    if (!(CallC<int>(FUN_43f620, car) & 0x4000) && !F(int, g_6cec6c, 1316)) {
        int n = (int)((F(double, h, 8) * 0.5 + 1.75) * 0.2857142857142857);
        if (n)
            level = n - 1;
    } else
        level = Fx_EvenStep(F(int, car, 728));
    return Fx_LiftedMatrix(out, &pos, level);
}

// [FUN_4d4620] the effect's level for a height x (its step into +16552), at most +16508
extern "C" int __fastcall Fx_LevelForHeight(BYTE* s, void*, double x)
{
    int id = F(int, F(BYTE*, s, 16576), 20636);
    if (CallC<int>(FUN_43f620, id) & 0x4000)
        return Fx_EvenStep(F(int, F(BYTE*, F(BYTE*, s, 16576), 20636), 728));
    int n = (int)((x + 1.75) * 0.2857142857142857);
    int level = n;
    if (n) {
        F(double, s, 16552) = x / n;
        level = n - 1;
    } else
        F(double, s, 16552) = x;
    if (CallC<int>(FUN_43f620, F(int, F(BYTE*, s, 16576), 20636)) & 0x80000)
        return level + 1;
    if (CallC<int>(FUN_43f620, F(int, F(BYTE*, s, 16576), 20636)) & 0x8000000)
        return level + 1;
    return F(int, s, 16508) <= level ? F(int, s, 16508) : level;
}

extern "C" double __fastcall MarineCamera_HorizontalAngle(BYTE* self, void*);
extern "C" void FUN_5ab620(); KING_ALIAS("_FUN_5ab620", "@Vec3_MulAssign@16")
ASM_VAR(Vec3, g_ahead702620, $L_702620)

static void Fx_Transform(float* out, const Matrix* m)
{
    const float* M = (const float*)m;
    const Vec3& g = g_ahead702620;
    out[0] = (float)(((double)g.z * M[6] + (double)g.y * M[3]) + (double)g.x * M[0] + M[9]);
    out[1] = (float)(((double)g.z * M[7] + (double)g.y * M[4]) + (double)g.x * M[1] + M[10]);
    out[2] = (float)(((double)g.z * M[8] + (double)g.y * M[5]) + (double)g.x * M[2] + M[11]);
}

// [FUN_4bf510] the two side points of a camera-relative effect, turned by 0.7 of the camera's angle and scaled to the width
extern "C" void __fastcall Fx_CameraPair(BYTE* s, void*)
{
    double a = MarineCamera_HorizontalAngle(F(BYTE*, F(BYTE*, s, 136), 488), 0) * 0.699999988079071f;
    float af = (float)a;
    F(float, s, 48) = (float)a;
    Matrix m;
    Fx_Transform((float*)(s + 68), Matrix_RotationZ(&m, af));
    Fx_Transform((float*)(s + 80), Matrix_RotationZ(&m, -F(float, s, 48)));
    float k = (float)(F(float, s, 60) / x87_cos(F(float, s, 48)));
    Vec3 t;
    CallThis(FUN_5ab620, s + 68, &t, k);
    CallThis(FUN_5ab620, s + 80, &t, k);
    F(float, s, 92) = (float)((double)F(float, s, 64) / F(float, s, 60));
}

// [FUN_4d5f50] the shown steering from the effect's swing: distance * a, less the turn from b and c
extern "C" void __fastcall Fx_SteerFromSwing(BYTE* s, void*, double a, double b, double c)
{
    Vec3 d;
    d.x = (float)((double)F(float, s, 13252) - F(float, s, 13276));
    d.y = (float)((double)F(float, s, 13256) - F(float, s, 13280));
    d.z = (float)((double)F(float, s, 13260) - F(float, s, 13284));
    double p = Vec3_Length(&d) * a;
    double r = (b > 0.0 ? b * 1.3 : b * 0.9) / F(double, s, 16552);
    if (!(r >= -0.5))
        r = -0.5;
    else if (r > 0.5)
        r = 0.5;
    else if (!((r < 0 ? -r : r) >= 0.05))
        r = 0.0;
    if (!(F(BYTE, s, 16496) & 2)) {
        double t = 1.0 - c * c;
        if (!(t >= 0.0))
            t = 0.0;
        r = r * t * 0.5;
    }
    double v = (double)F(float, s, 13100) * 0.1f;
    if (!(v >= 0.5))
        v = 0.5;
    double w = r / (v + v);
    double cc = c;
    if (!((c < 0 ? -c : c) >= 0.1))
        cc = 0.0;
    Car_SetSteerShown(s, 0, (p - w) - cc);
}

ASM_PROC(FUN_4be2c0)    // the distance of a segment to a box-like object
ASM_PROC(FUN_616850)    // the distance of a segment to the terrain of a type-8 object

static double Fx_SegDist(void* a, void* b, BYTE* o)
{
    if (F(int, o, 24) == 8)
        return CallThis<double>(FUN_616850, F(BYTE*, o, 32), a, b);
    return CallC<double>(FUN_4be2c0, a, b, o);
}

// [FUN_4bdf10] the distance of the segment a-b to the nearer of two objects (9999.9 when neither is set)
extern "C" double __cdecl Fx_SegmentDistance(void* a, void* b, BYTE* c, BYTE* d)
{
    const float none = 9999.900390625f;
    int ta = F(int, c, 24), tb = F(int, d, 24);
    if (ta && tb) {
        if (ta == 8 && tb == 8)
            return CallThis<double>(FUN_616850, F(BYTE*, c, 32), a, b);
        if (ta == 8) {
            if (tb == 6) {
                double r = CallC<double>(FUN_4be2c0, a, b, d);
                if (r < none || r > none)
                    return r;
            }
            return CallThis<double>(FUN_616850, F(BYTE*, c, 32), a, b);
        }
        if (tb == 8) {
            if (ta == 6) {
                double r = CallC<double>(FUN_4be2c0, a, b, c);
                if (r < none || r > none)
                    return r;
            }
            return CallThis<double>(FUN_616850, F(BYTE*, d, 32), a, b);
        }
        if (ta == 6) {
            double r = CallC<double>(FUN_4be2c0, a, b, c);
            if (r < none || r > none)
                return r;
            return CallC<double>(FUN_4be2c0, a, b, d);
        }
        double r = CallC<double>(FUN_4be2c0, a, b, d);
        if (r < none || r > none)
            return r;
        return CallC<double>(FUN_4be2c0, a, b, c);
    }
    if (ta)
        return Fx_SegDist(a, b, c);
    if (tb)
        return Fx_SegDist(a, b, d);
    return none;
}

ASM_VAR(float, g_detail696088, $L_696088)

// [FUN_4bc2d0] a burst of five sparks at a with velocities 0.5..1.5 of v (sent to the other players when asked)
extern "C" void __cdecl Fx_SparkBurst(const Vec3* at, const Vec3* v, int send)
{
    if (send && (F(int, g_6cec6c, 1300) == 1 || F(int, g_6cec6c, 1316))) {
        BYTE pkt[28] = {};
        pkt[0] = 9;
        memcpy(pkt + 4, at, 12);
        memcpy(pkt + 16, v, 12);
        CallThis(FUN_603110, g_view + 8336, 957, pkt, 28);
    }
    if (!(g_detail696088 > 0.5f))
        return;
    static const float k[5] = { 0.5f, 0.800000011920929f, 1.0f, 1.2000000476837158f, 1.5f };
    for (int i = 0; i < 5; i++) {
        Vec3 w;
        if (i == 2)
            w = *v;
        else {
            w.x = (float)((double)v->x * k[i]);
            w.y = (float)((double)v->y * k[i]);
            w.z = (float)((double)v->z * k[i]);
        }
        Fx_SpawnPuffSpark((int)at, (int)&w);
    }
}

ASM_PROC(FUN_4d57c0)    // the effect's swing step
extern "C" BYTE* __fastcall Car_543bb0(BYTE* self, void*);
ASM_PROC(FUN_5fd5f0)    // a sound at a point (point, distance, volume, flags)

// the number of swing steps over a1 + a2 (kept out of line, see Fx_SwingStep)
static __declspec(noinline) int Fx_SwingSteps(double a1, double a2, double period)
{
    return (int)((a1 + a2) / (period * 0.5));
}

// [FUN_4d6340] one swing step of a vehicle effect; on a hard bump its sound at the truck seen from the camera; the step's result
extern "C" double __fastcall Fx_SwingStep(BYTE* s, void*, int a0, double a1, double a2, int a3, int a4)
{
    // in a real call: inline, MSVC 14.29 kept a1, a2 and the period on the x87
    // stack across __ftol2_sse and popped one value too many (a NaN argument
    // below, then an x87 stack fault that broke the AI cars' physics)
    F(int, s, 16480) = Fx_SwingSteps(a1, a2, F(double, s, 16552));
    double result = -1.0;
    CallC<double>(FUN_4d57c0, F(int, s, 16576), a0, a1, a2, F(double, s, 16552), a3, F(double, s, 16560),
                  (double)F(float, s, 13100), F(int, s, 16500), F(int, s, 16504), &result,
                  s + 16496, s + 16484, s + 16476, a4);
    if ((F(BYTE, s, 16496) & 8) && !(F(double, s, 16560) >= 0.0)) {
        Matrix m = F(Matrix, s, 16);
        const float* M = (const float*)&m;
        BYTE buf[48];
        Matrix inv;
        const float* I = (const float*)Matrix_Inverse(&inv, (const Matrix*)SceneObj_Matrix1(F(BYTE*, g_game, 616) + 16, 0, buf));
        Vec3 p;
        p.x = (float)(((double)M[10] * I[3] + (double)M[11] * I[6]) + (double)M[9] * I[0] + I[9]);
        p.y = (float)(((double)M[9] * I[1] + (double)M[10] * I[4]) + (double)M[11] * I[7] + I[10]);
        p.z = (float)(((double)M[9] * I[2] + (double)M[10] * I[5]) + (double)M[11] * I[8] + I[11]);
        CallThis(FUN_5fd5f0, Car_543bb0(F(BYTE*, s, 16576), 0), p, 20.0, 1.0, 2);
    }
    return result;
}

extern "C" BYTE* __fastcall NodeN_Construct(BYTE* self, void*, const char* name, int a, int b);
extern "C" void __fastcall Light_Init(BYTE* self, void*);
ASM_VAR(BYTE*, g_thrownLamp696734, $L_696734)
ASM_VAR(float, g_detail696078, $L_696078)
ASM_VAR(char, g_strThrowLamp, $L_672c60)        // "ThrowLamp"
ASM_VAR(char, g_vtbl64d444, $L_64d444)

// a new "ThrowLamp" light object
static BYTE* ThrowLamp_New()
{
    BYTE* o = (BYTE*)operator new(144);
    if (!o)
        return 0;
    NodeN_Construct(o, 0, &g_strThrowLamp, 21, 1);
    Light_Init(o + 56, 0);
    F(void*, o, 0) = &g_vtbl64d444;
    F(int, o, 136) = 1;
    F(int, o, 140) = 0;
    return o;
}

// [FUN_4d9cd0] the shared thrown-lamp light at the effect's position (once, at detail over 0.5)
extern "C" void __fastcall Fx_ThrowLamp(BYTE* s, void*)
{
    if (g_thrownLamp696734 || !(g_detail696078 > 0.5f))
        return;
    BYTE* o = ThrowLamp_New();
    g_thrownLamp696734 = o;
    F(int, o, 136) = 1;
    F(int, g_thrownLamp696734, 140) = 1;
    F(int, g_thrownLamp696734, 56) = 2;
    F(Vec3, g_thrownLamp696734, 60) = F(Vec3, s, 36);
    F(DWORD, g_thrownLamp696734, 96) = 0x3F800000;
    F(float, g_thrownLamp696734, 108) = g_fireRamp1[1];
    F(float, g_thrownLamp696734, 112) = g_fireRamp2[1];
    F(float, g_thrownLamp696734, 116) = g_fireRamp3[1];
    F(DWORD, g_thrownLamp696734, 84) = 0x3F800000;
    F(int, g_thrownLamp696734, 88) = 0;
    F(DWORD, g_thrownLamp696734, 92) = 0x3C23D70A;     // 0.01f
    F(int, g_thrownLamp696734, 128) = 70;
    BYTE* lights = F(BYTE*, F(BYTE*, s, 32), 528);
    if (lights)
        CallVirt(lights, 20, 0, g_thrownLamp696734);
}

extern "C" void FUN_59f070(); KING_ALIAS("_FUN_59f070", "@Effect59f070_Construct@32")
extern "C" void FUN_59bed0(); KING_ALIAS("_FUN_59bed0", "@Spark59bed0_Construct@32")
extern "C" void FUN_59ad70(); KING_ALIAS("_FUN_59ad70", "@Spark59ad70_Construct@32")
extern "C" void FUN_59b3f0(); KING_ALIAS("_FUN_59b3f0", "@Flash_Construct@36")
typedef char FxLabel[1];
ASM_VAR(float, g_fx69605c, $L_69605c)
ASM_VAR(float, g_fx696048, $L_696048)
ASM_VAR(BYTE*, g_pool6f6ab4, $L_6f6ab4)
ASM_VAR(BYTE*, g_pool6f6a7c, $L_6f6a7c)
ASM_VAR(BYTE*, g_pool6f6a80, $L_6f6a80)
ASM_VAR(BYTE*, g_pool6f6a88, $L_6f6a88)
ASM_VAR(FxLabel, g_fx694998, $L_694998)
ASM_VAR(FxLabel, g_fx695bf0, $L_695bf0)
ASM_VAR(FxLabel, g_fx695c58, $L_695c58)
typedef DWORD FxDwords[1];
ASM_VAR(FxDwords, g_fx695908, $L_695908)
ASM_VAR(unsigned, g_fx696214, $L_696214)
ASM_VAR(int, g_fx69620c, $L_69620c)
ASM_VAR(int, g_fx696208, $L_696208)
ASM_VAR(int, g_fx696210, $L_696210)

static const float kRand = 3.0518509447574615e-05f;     // 1/32767

static BYTE* Fx_PoolNew(int size, BYTE* pool) { return CallC<BYTE*>(FUN_579380, size, pool); }

// [FUN_4ba310] smoke puffs at a point: two, one, or one of a random colour (chance by the detail at 0x69605c)
extern "C" void __cdecl Fx_SmokePuffs(int a, int b)
{
    if (!((double)1.0f - g_fx69605c < (double)rand() * kRand + 0.10000000149011612f))
        return;
    double r = (double)rand() * kRand;
    if (!(r >= 0.4000000059604645f)) {
        BYTE* o = Fx_PoolNew(196, g_pool6f6ab4);
        if (o)
            CallThis(FUN_59f070, o, g_view, a, b, 0.25f, g_fx694998, 1);
        o = Fx_PoolNew(196, g_pool6f6ab4);
        if (o) {
            BYTE* e = CallThis<BYTE*>(FUN_59f070, o, g_view, a, b, 0.25f, g_fx695bf0, 1);
            if (e)
                F(int, e, 192) = 0;
        }
    } else if (!(r >= 0.800000011920929f)) {
        BYTE* o = Fx_PoolNew(196, g_pool6f6ab4);
        if (o)
            CallThis(FUN_59f070, o, g_view, a, b, 0.25f, g_fx695bf0, 1);
    } else {
        BYTE* o = Fx_PoolNew(196, g_pool6f6ab4);
        if (o) {
            double k = (double)rand() * kRand;
            int i = (int)((double)(__int64)g_fx696214 * k);
            BYTE* e = CallThis<BYTE*>(FUN_59f070, o, g_view, a, b, 0.25f, g_fx695908 + i, 1);
            if (e)
                F(int, e, 192) = 0;
        }
    }
}

// [FUN_4ba660] sparks at a point by strength (over 0.4), and a flash over 0.7
extern "C" void __cdecl Fx_SparksByStrength(int a, const Vec3* at, float strength)
{
    if (!((double)1.0f - g_fx696048 < (double)rand() * kRand + 0.20000000298023224f))
        return;
    if (!(strength >= 0.4000000059604645f))
        return;
    float q = (float)(((double)strength - 0.4000000059604645f) * 1.6666666269302368f);
    if ((double)rand() * kRand > q) {
        BYTE* o = Fx_PoolNew(192, g_pool6f6a7c);
        if (o)
            CallThis(FUN_59bed0, o, g_view, a, at, g_fx695c58, g_fx69620c, 1.5f);
    } else {
        BYTE* o = Fx_PoolNew(192, g_pool6f6a80);
        if (o)
            CallThis(FUN_59ad70, o, g_view, a, at, g_fx695bf0, g_fx696208, 1.5f);
    }
    if (strength > 0.699999988079071f) {
        Vec3 v;
        v.x = at->x;
        v.y = at->y;
        v.z = (float)((double)at->z * 0.800000011920929f);
        BYTE* o = Fx_PoolNew(212, g_pool6f6a88);
        if (o)
            CallThis(FUN_59b3f0, o, g_view, a, &v, g_fx694998, g_fx696210, 0.5f, 0.75f);
    }
}

// [FUN_4ba8a0] sparks or a flash at a point by strength (over 0.4; the flash over 0.7)
extern "C" void __cdecl Fx_SparkOrFlash(int a, const Vec3* at, float strength)
{
    double r = (double)rand() * kRand;
    float rf = (float)r;
    if (!((double)1.0f - g_detail696078 < r + 0.10000000149011612f))
        return;
    if (!(strength >= 0.4000000059604645f))
        return;
    double q = ((double)strength - 0.4000000059604645f) * 1.6666666269302368f;
    if (!(strength >= 0.699999988079071f)) {
        if (!(q >= rf)) {
            BYTE* o = Fx_PoolNew(192, g_pool6f6a7c);
            if (o)
                CallThis(FUN_59bed0, o, g_view, a, at, g_fx695c58, g_fx69620c, 1.5f);
            return;
        }
        BYTE* o = Fx_PoolNew(192, g_pool6f6a80);
        if (o)
            CallThis(FUN_59ad70, o, g_view, a, at, g_fx695bf0, g_fx696208, 1.5f);
        return;
    }
    Vec3 v;
    v.x = at->x;
    v.y = at->y;
    v.z = (float)((double)at->z * 0.800000011920929f);
    if (0.5f < rf) {
        BYTE* o = Fx_PoolNew(212, g_pool6f6a88);
        if (o)
            CallThis(FUN_59b3f0, o, g_view, a, &v, g_fx694998, g_fx696210, 0.5f, 0.75f);
        return;
    }
    BYTE* o = Fx_PoolNew(192, g_pool6f6a80);
    if (o)
        CallThis(FUN_59ad70, o, g_view, a, at, g_fx695bf0, g_fx696208, 1.5f);
}

ASM_VAR(char, g_strMnFire, $L_672c58)        // "mnFire"
ASM_VAR(char, g_vtbl64d424, $L_64d424)

static float Fx_RandSpread()
{
    float a = (float)((double)rand() * kRand);
    double b = (double)rand() * kRand - a;
    return (float)(b + b);
}

// [FUN_4d9af0] constructs a fire effect between two points with random flicker, its lamp, added to the scene
extern "C" BYTE* __fastcall Fire_Construct(BYTE* s, void*, BYTE* scene, const Vec3* a, const Vec3* b, int c)
{
    CallThis(FUN_5d3110, s, &g_strMnFire);
    for (int o = 36; o <= 56; o += 4) F(int, s, o) = 0;
    F(void*, s, 0) = &g_vtbl64d424;
    F(BYTE*, s, 32) = scene;
    F(Vec3, s, 36) = *a;
    F(Vec3, s, 48) = *b;
    F(int, s, 60) = c;
    F(DWORD, s, 104) = 0x3F000000;
    F(int, s, 64) = 0;
    F(int, s, 72) = 0;
    F(DWORD, s, 68) = 0x3E800000;
    F(float, s, 76) = Fx_RandSpread();
    F(float, s, 80) = Fx_RandSpread();
    F(float, s, 84) = (float)((double)rand() * 1.5259254723787308e-05f + 1.0f);
    F(float, s, 88) = Fx_RandSpread();
    F(float, s, 92) = Fx_RandSpread();
    F(float, s, 96) = (float)((double)rand() * 1.5259254723787308e-05f + 1.0f);
    Fx_ThrowLamp(s, 0);
    CallThis(FUN_5d3570, F(BYTE*, s, 32) + 2992, s);
    return s;
}

extern "C" int __cdecl Fx_TruckStill();
extern "C" void FUN_59d790(); KING_ALIAS("_FUN_59d790", "@Spark_Construct2@44")
extern "C" void FUN_59d9f0(); KING_ALIAS("_FUN_59d9f0", "_Wasm_Set6f6a70")
ASM_VAR(float, g_fx69603c, $L_69603c)
ASM_VAR(unsigned, g_sparkLimit671700, $L_671700)
ASM_VAR(unsigned, g_sparkCount6f6a6cE, $L_6f6a6c)
ASM_VAR(BYTE*, g_pool6f6a9c, $L_6f6a9c)
ASM_VAR(int, g_fx694830, $L_694830)
ASM_VAR(int, g_fx696218, $L_696218)
ASM_VAR(FxLabel, g_fx694a00, $L_694a00)

// [FUN_4bb050] dust sparks from a wheel: speed-scaled sizes, a limit by frame time, a random velocity of v
extern "C" void __cdecl Fx_WheelDust(const Vec3* at, const Vec3* v, float strength)
{
    if (!Fx_TruckStill())
        return;
    if (!(g_fx69603c >= 0.5f))
        return;
    if ((double)rand() * kRand > 0.800000011920929f)
        return;
    if (strength > 1.0f)
        strength = 1.0f;
    double inv = 1.0f / F(double, g_view, 2960);
    if (inv > 20.0f)
        inv = 20.0f;
    if (g_sparkCount6f6a6cE > (unsigned)(int)((double)(__int64)g_sparkLimit671700 * inv * 0.05000000074505806f))
        return;
    float s2 = (float)((double)strength * 0.5f + 1.0f);
    double t1 = ((double)rand() * 0.5f * kRand + 1.0f) * s2;
    float v1 = (float)(t1 + t1);
    float v2 = (float)(((double)rand() * 0.5f * kRand + 1.0f) * s2 * 3.5f);
    BYTE* o = Fx_PoolNew(200, g_pool6f6a9c);
    if (o) {
        double f = (double)rand() * kRand * 0.10000000149011612f + 0.800000011920929f;
        float ff = (float)f;
        Vec3 w;
        w.z = (float)(f * v->z);
        w.y = (float)((double)ff * v->y);
        w.x = (float)((double)ff * v->x);
        CallThis(FUN_59d790, o, g_view, at, &w, v1, v2, g_fx694a00, g_fx696218, 0, g_fx694830);
    }
    CallC(FUN_59d9f0, g_fx694830);
}

extern "C" void FUN_59c6c0(); KING_ALIAS("_FUN_59c6c0", "@Effect59c6c0_Construct@36")
extern "C" void FUN_59cea0(); KING_ALIAS("_FUN_59cea0", "@Effect59cea0_Construct@36")
ASM_VAR(float, g_fx696084, $L_696084)
ASM_VAR(BYTE*, g_fx7025b8, $L_7025b8)
ASM_VAR(int, g_fx6f33c0, $L_6f33c0)
ASM_VAR(BYTE*, g_pool6f6a94, $L_6f6a94)
ASM_VAR(BYTE*, g_pool6f6a90, $L_6f6a90)
ASM_VAR(FxDwords, g_fx695c54, $L_695c54)
ASM_VAR(Vec3, g_axis702630e, $L_702630)

// [FUN_4bad10] a crash burst at p: a flash and debris pushed along the up axis by the size (fast impacts only)
extern "C" void __cdecl Fx_CrashBurst(Vec3 p, Vec3 v, Vec3 w, int flags, float size)
{
    if (!(g_fx696084 > 0.5f))
        return;
    if (!(F(float, g_fx7025b8, 536) > 0.2))
        return;
    if (g_fx6f33c0 || !(flags & 2))
        return;
    if (!(Vec3_Length(&v) >= 5.0f))
        return;
    float h = (float)((double)size * 0.8620690107345581f);
    float s1 = (float)((double)size * 2.5862069129943848f);
    float s2 = (float)((double)size * 4.31034517288208f);
    const Vec3& g = g_axis702630e;
    BYTE* o = Fx_PoolNew(192, g_pool6f6a94);
    if (o) {
        double gz = (double)g.z * h;
        float gy = (float)((double)g.y * h);
        float gx = (float)((double)g.x * h);
        Vec3 q;
        q.z = (float)(gz + p.z);
        q.y = (float)((double)p.y + gy);
        q.x = (float)((double)p.x + gx);
        CallThis(FUN_59c6c0, o, g_view, &q, &v, &w, s1, g_fx695c54 + g_fx69620c, 1);
    }
    o = Fx_PoolNew(192, g_pool6f6a90);
    if (o) {
        double wx = (double)w.x * 0.15000000596046448f;
        float wy = (float)((double)w.y * 0.15000000596046448f);
        float wz = (float)((double)w.z * 0.15000000596046448f);
        double gx = (double)g.x * h;
        float gy = (float)((double)g.y * h);
        float gz = (float)((double)g.z * h);
        Vec3 r;
        r.x = (float)((gx + p.x) + wx);
        r.y = (float)((double)(float)((double)gy + p.y) + wy);
        r.z = (float)((double)(float)((double)gz + p.z) + wz);
        CallThis(FUN_59cea0, o, g_view, &r, &v, &w, s2, g_fx695c54 + g_fx69620c, 1);
    }
}

extern "C" void FUN_43fc40(); KING_ALIAS("_FUN_43fc40", "_Ai_GetPlayerData")    // a vehicle's info by id (id, out)
extern "C" void FUN_43f690(); KING_ALIAS("_FUN_43f690", "_Player_SetFlags")    // sets a vehicle's flags (id, flags)
ASM_PROC(FUN_4cf080)
extern "C" void __fastcall Car_SetInMode(BYTE* car, void*, int mode);
extern "C" void __fastcall Flags1196_Clear(BYTE* self, void*, int bits);
extern "C" void FUN_58f290(); KING_ALIAS("_FUN_58f290", "@Body_SetMoveMode@12")
ASM_VAR(int, g_noVehicle68b990, $L_68b990)
ASM_VAR(BYTE*, g_fx696340, $L_696340)
ASM_VAR(int, g_fx696328, $L_696328)
ASM_VAR(int, g_fx6962cc, $L_6962cc)

// [FUN_4cef60] parks a vehicle object: mode 2, its matrix saved, its anchor point placed, the flag 1 cleared
extern "C" void __cdecl Fx_ParkVehicle(BYTE* car, int id, DWORD* flags)
{
    Car_SetInMode(car, 0, 2);
    BYTE* o = car + 16;
    Flags1196_Clear(o, 0, 1);
    CallThis(FUN_58f290, o, 0);
    BYTE buf[48];
    memcpy(car + 20640, SceneObj_Matrix1(o, 0, buf), 48);
    const float* m = (const float*)SceneObj_Matrix1(o, 0, buf);
    double a = F(float, car, 21040), b = F(float, car, 21044), c = F(float, car, 21048);
    F(float, car, 20676) = (float)(((double)m[3] * b + (double)m[6] * c) + a * m[0] + m[9]);
    F(float, car, 20680) = (float)(((double)m[4] * b + (double)m[1] * a) + (double)m[7] * c + m[10]);
    F(float, car, 20684) = (float)(((double)m[5] * b + (double)m[2] * a) + (double)m[8] * c + m[11]);
    DWORD f = *flags & ~1u;
    *flags = f;
    if (id != g_noVehicle68b990)
        CallC(FUN_43f690, id, f);
}

// [FUN_4cf260] attaches the effect to a vehicle: the shown mode from its type and flags, its parking
extern "C" void __cdecl Fx_AttachVehicle(BYTE* car, BYTE* fx, int id)
{
    F(BYTE*, fx, 16576) = car;
    int active = 1;
    DWORD f = 0;
    if (id != g_noVehicle68b990) {
        BYTE info[256];
        CallC(FUN_43fc40, id, info);
        f = CallC<DWORD>(FUN_43f620, id);
        int type = F(int, info, 8);
        if (type == 1)
            CallC(FUN_43f690, id, f | 1);
        else {
            active = 0;
            F(int, fx, 16504) = type == 7 || type == 8 ? 1 : 0;
        }
    }
    F(int, fx, 10108) = 1;
    if (active) {
        F(int, car, 20944) = 0;
        F(WORD, fx, 10048) = 1;
        F(int, fx, 10080) = F(int, g_6cec6c, 1316) == 0;
        g_fx696340 = car;
        CallC(FUN_4cf080, car, id, &f, 0);
    } else {
        F(WORD, fx, 10048) = 0;
        F(int, fx, 10080) = 0;
        if (g_fx696328) {
            CallC(FUN_4cf080, car, id, &f, 0);
            if (id == g_fx6962cc)
                g_fx696340 = car;
        } else
            Fx_ParkVehicle(car, id, &f);
    }
    F(double, fx, 16544) = Timer_Now();
}

extern "C" BYTE* __fastcall Obj541970_First(BYTE* self, void*);
extern "C" BYTE* __fastcall Obj541960_Self(BYTE* self, void*);

// [FUN_4cfd10] whether a moving or type-5 vehicle is within 200 of a point
extern "C" int __cdecl Fx_VehicleNear(float x, float y, float z)
{
    for (BYTE* it = Obj541970_First(F(BYTE*, g_game, 616), 0); it; ) {
        BYTE* o = Obj541960_Self(it, 0);
        BYTE buf[48];
        BYTE* m = (BYTE*)SceneObj_Matrix1(o + 16, 0, buf);
        Vec3 d;
        d.x = (float)((double)x - F(float, m, 36));
        d.y = (float)((double)y - F(float, m, 40));
        d.z = (float)((double)z - F(float, m, 44));
        if (!(Vec3_Length(&d) > 200.0f)) {
            if (F(int, o, 21604))
                return 1;
            int id = F(int, o, 20636);
            if (id != g_noVehicle68b990) {
                BYTE info[256];
                CallC(FUN_43fc40, id, info);
                if (F(int, info, 8) == 5)
                    return 1;
            }
        }
        it = F(BYTE*, it, 8);
        if (!it)
            break;
        it -= 4;
    }
    return 0;
}

extern "C" void FUN_43bf10(); KING_ALIAS("_FUN_43bf10", "_Ai_AdvancePlayerPosition")    // the position a distance along a route (out, id, pos, &d, -)

// [FUN_4cdaf0] the turn rate along a route: the angle between the directions at pos and d further, over the distance made
extern "C" double __cdecl Fx_TurnRate(Bytes32 pos, double d, int id, int back)
{
    double orig = d;
    if (back)
        d = -d;
    Bytes32 res;
    CallC(FUN_43bf10, &res, id, pos, &d, 0);
    double d2 = d;
    if (back)
        d2 = -d2;
    double diff = orig - d2;
    Vec3 a, b;
    Effect_Direction4cda20(&a, pos, back, 0);
    Effect_Direction4cda20(&b, res, back, 0);
    double dot = ((double)b.z * a.z + (double)b.y * a.y) + (double)b.x * a.x;
    if (dot > 1.0) dot = 1.0;
    else if (!(dot >= -1.0)) dot = -1.0;
    double ang = x87_acos(dot);
    double cx = (double)b.z * a.y - (double)a.z * b.y;
    double cy = (double)a.z * b.x - (double)b.z * a.x;
    double cz = (double)b.y * a.x - (double)a.y * b.x;
    const Vec3& g = g_axis702630e;
    double t = (cz * g.z + cy * g.y) + cx * g.x;
    if (!(t >= 0.0))
        ang = -ang;
    if (!(diff >= 0.1))
        diff = 0.1;
    return ang / diff;
}

ASM_PROC(FUN_5d3290)    // the scene object base destructor
ASM_PROC(FUN_5f1a70)
ASM_VAR(char, vt_64c90c, $L_64c90c)
ASM_VAR(char, vt_64d49c, $L_64d49c)
ASM_VAR(char, vt_64d4c8, $L_64d4c8)
ASM_VAR(char, vt_64d554, $L_64d554)
ASM_VAR(char, vt_64c358, $L_64c358)

// [FUN_4c9a00] destructor of an effect class (vtable 64c90c)
extern "C" void __fastcall Effect4c9a00_Destroy(void** self, int)
{
    self[0] = &vt_64c90c;
    CallThis(FUN_5d3290, self);
}

// [FUN_4da7d0] destructor of an effect class (vtable 64d49c)
extern "C" void __fastcall Effect4da7d0_Destroy(void** self, int)
{
    self[0] = &vt_64d49c;
    CallThis(FUN_5d3290, self);
}

// [FUN_4db890] destructor of an effect class (vtable 64d4c8)
extern "C" void __fastcall Effect4db890_Destroy(void** self, int)
{
    self[0] = &vt_64d4c8;
    CallThis(FUN_5d3290, self);
}

// [FUN_4dd600] destructor of an effect class (vtable 64d554)
extern "C" void __fastcall Effect4dd600_Destroy(void** self, int)
{
    self[0] = &vt_64d554;
    CallThis(FUN_5d3290, self);
}

// [FUN_4bcbd0] destructor of an effect class (vtable 64c358)
extern "C" void __fastcall Effect4bcbd0_Destroy(void** self, int)
{
    self[0] = &vt_64c358;
    CallThis(FUN_5f1a70, self);
}

ASM_VAR(char, vt_64c63c, $L_64c63c)
ASM_PROC(FUN_5ef660)
extern "C" void FUN_60aa60(); KING_ALIAS("_FUN_60aa60", "_Grd_Message")
ASM_VAR(char, g_6724f8, $L_6724f8)
ASM_VAR(char, g_6726e8, $L_6726e8)
ASM_VAR(char, g_6726e0, $L_6726e0)
ASM_VAR(char, g_6726a4, $L_6726a4)

// [FUN_4c1790] constructor of an empty effect link (vtable 64c63c)
extern "C" void __fastcall EffectLink_Init(void** self, int)
{
    self[0] = &vt_64c63c;
    self[1] = 0;
}

// [FUN_4cef50] always 0
extern "C" int __cdecl Effect_Zero()
{
    return 0;
}

// [FUN_4bd570] frees the buffer at +72, then destroys the base
extern "C" void __fastcall EffectBuf_Destroy(BYTE* self, int)
{
    void* p = *(void**)(self + 72);
    if (p)
        operator delete(p);
    CallThis(FUN_5ef660, self);
}

// [FUN_4ba2e0] registers the effects' settings table
extern "C" int __cdecl Effects_Register()
{
    return CallC<int>(FUN_60aa60, 2, &g_6726a4, &g_6726e0, &g_6726e8, 32, &g_6724f8);
}

extern "C" void FUN_4ccb70(); KING_ALIAS("_FUN_4ccb70", "_Fade_Age")
extern "C" void FUN_4ccbc0(); KING_ALIAS("_FUN_4ccbc0", "_Fade_Switch")
extern "C" void FUN_4ccc70(); KING_ALIAS("_FUN_4ccc70", "_Fade_SwitchStep")
extern "C" void FUN_4ccd20(); KING_ALIAS("_FUN_4ccd20", "_Fade_Set")
ASM_PROC(FUN_5f36d0)

// [FUN_4ccd50] runs the step of a fade for its stage
extern "C" int __cdecl Fade_RunStage(void* f, int t)
{
    switch (CallC<int>(FUN_4ccb70, f)) {
    case 0: return CallC<int>(FUN_4ccbc0, f, t);
    case 1: return CallC<int>(FUN_4ccc70, f, t);
    case 2: return CallC<int>(FUN_4ccd20, f, t);
    }
    return 0;
}

// [FUN_4c0d80] initializes the base, then the material at +72: alpha 1, flags 0x40000 and 2
extern "C" void __fastcall EffectMat_Init(BYTE* self, int, int a, int b)
{
    CallThis(FUN_5f36d0, self, a, b);
    BYTE* m = F(BYTE*, self, 72);
    F(DWORD, m, 88) = 0x3f800000;
    F(DWORD, F(BYTE*, self, 72), 32) |= 0x40000;
    F(DWORD, F(BYTE*, self, 72), 32) |= 2;
    F(int, self, 68) = 0;
}

ASM_VAR(void*, g_fx6f63d0, $L_6f63d0)
ASM_VAR(void*, g_fx696254, $L_696254)
ASM_VAR(void*, g_fx6961f8, $L_6961f8)
ASM_VAR(int, g_fx696238, $L_696238)
ASM_VAR(int, g_fx69622c, $L_69622c)
ASM_VAR(int, g_fx696230, $L_696230)
ASM_VAR(int, g_fx696234, $L_696234)
extern "C" void FUN_5c64f0(); KING_ALIAS("_FUN_5c64f0", "_Effects_FreeTrees")
ASM_PROC(FUN_5e48f0)
extern "C" void FUN_4bcbe0(); KING_ALIAS("_FUN_4bcbe0", "@LightPool_Free@8")
ASM_PROC(FUN_4c4780)
extern "C" void FUN_4b89e0(); KING_ALIAS("_FUN_4b89e0", "_Fx_FreeBuffers")
extern "C" void FUN_4cca50(); KING_ALIAS("_FUN_4cca50", "_Fx_Delete6961fc")

// [FUN_4bac00] frees the effects' global objects and buffers
extern "C" void __cdecl Effects_Shutdown()
{
    CallC(FUN_5c64f0);
    CallC(FUN_5e48f0);
    if (g_fx6f63d0) {
        CallVirt(g_fx6f63d0, 0, 1);
        g_fx6f63d0 = 0;
    }
    if (g_fx696254) {
        CallVirt(g_fx696254, 0, 1);
        g_fx696254 = 0;
    }
    void* p = g_fx6961f8;
    if (p) {
        CallThis(FUN_4bcbe0, p);
        operator delete(p);
        g_fx6961f8 = 0;
    }
    g_fx696238 = 0;
    g_fx69622c = 0;
    g_fx696230 = 0;
    g_fx696234 = 0;
    CallC(FUN_4c4780);
    CallC(FUN_4b89e0);
    CallC(FUN_4cca50);
}

ASM_VAR(char, s_fmt672504, $L_672504)
ASM_VAR(char, s_fmt67254c, $L_67254c)
ASM_VAR(char, s_fmt67255c, $L_67255c)
ASM_VAR(char, s_name67250c, $L_67250c)
ASM_VAR(char, s_name672514, $L_672514)
ASM_VAR(char, s_name672564, $L_672564)
ASM_VAR(char, s_name6725c0, $L_6725c0)
ASM_VAR(int, g_fxCount69621c, $L_69621c)
ASM_VAR(int, g_fxCount696210, $L_696210)
ASM_VAR(int, g_fxCount696218, $L_696218)
ASM_VAR(int, g_fxCount696208, $L_696208)
typedef BYTE* MatArray[1];
ASM_VAR(MatArray, g_mats69618c, $L_69618c)
ASM_VAR(MatArray, g_mats694998, $L_694998)
ASM_VAR(MatArray, g_mats694a00, $L_694a00)
ASM_VAR(MatArray, g_mats695bf0, $L_695bf0)

// [FUN_4b8a10] loads the numbered materials of set 67250c from 1 until one is missing
extern "C" void __cdecl Fx_LoadSet69618c()
{
    char buf[256];
    g_fxCount69621c = 0;
    for (int i = 1;; i++) {
        sprintf(buf, &s_fmt672504, &s_name67250c, i);
        BYTE* m = Fx_Material(buf, g_lib696200);
        g_mats69618c[g_fxCount69621c] = m;
        if (!m)
            break;
        F(DWORD, m, 88) = 0x3f800000;
        F(DWORD, g_mats69618c[g_fxCount69621c], 32) |= 0x40220;
        g_fxCount69621c++;
    }
}

// [FUN_4b8ad0] loads the numbered materials of set 672514 from 1 until one is missing
extern "C" void __cdecl Fx_LoadSet694998()
{
    char buf[256];
    g_fxCount696210 = 0;
    BYTE** p = g_mats694998;
    for (int i = 1;; i++, p++) {
        sprintf(buf, &s_fmt672504, &s_name672514, i);
        BYTE* m = Fx_Material(buf, g_lib696200);
        *p = m;
        if (!m)
            break;
        g_fxCount696210++;
        F(DWORD, m, 88) = 0x3f800000;
        F(DWORD, *p, 32) |= 0x40220;
    }
}

// [FUN_4b8f00] loads the numbered materials of set 6725c0 from 0 until one is missing
extern "C" void __cdecl Fx_LoadSet694a00()
{
    char buf[256];
    g_fxCount696218 = 0;
    BYTE** p = g_mats694a00;
    for (int i = 0;; i++, p++) {
        sprintf(buf, &s_fmt672504, &s_name6725c0, i);
        BYTE* m = Fx_Material(buf, g_lib696200);
        *p = m;
        if (!m)
            break;
        g_fxCount696218++;
        F(DWORD, m, 88) = 0x3f800000;
        F(DWORD, *p, 32) |= 0x40220;
    }
    if (!g_fxCount696218)
        Fx_Error(698);
}

// [FUN_4b8c40] loads the numbered materials of set 672564 from 0 (two-digit names for the first ten)
extern "C" void __cdecl Fx_LoadSet695bf0()
{
    char buf[256];
    g_fxCount696208 = 0;
    BYTE** p = g_mats695bf0;
    for (int i = 0;; i++, p++) {
        sprintf(buf, p > g_mats695bf0 + 9 ? &s_fmt67254c : &s_fmt67255c, &s_name672564, i);
        BYTE* m = Fx_Material(buf, g_lib696200);
        *p = m;
        if (!m)
            break;
        g_fxCount696208++;
        F(DWORD, m, 88) = 0x3f800000;
        F(DWORD, *p, 32) |= 0x40220;
    }
    if (!g_fxCount696208)
        Fx_Error(622);
}

// [FUN_4bcbe0] frees a light pool's buffers and its device object
extern "C" void __fastcall LightPool_Free(BYTE* self, int)
{
    operator delete(F(void*, self, 0));
    operator delete(F(void*, self, 4));
    // the element destructor of the two arrays (FUN_50d190) does nothing
    BYTE* a = F(BYTE*, self, 8);
    if (a)
        operator delete(a - 4);
    a = F(BYTE*, self, 12);
    if (a)
        operator delete(a - 4);
    void* d = F(void*, self, 36);
    if (d) {
        CallVirt(d, 36);
        d = F(void*, self, 36);
        if (d)
            CallVirt(d, 0, 1);
        F(void*, self, 36) = 0;
    }
    for (int i = 0; i < 10; i++)
        F(int, self, 40 + i * 4) = 0;
    F(int, self, 84) = 0;
}

extern "C" int __cdecl Get70ebf8();                                  // media.cpp
extern "C" void __fastcall Shadows_SetOwner(BYTE* self, void*, int owner);
extern "C" void __cdecl Set70ebf4(int v);
ASM_VAR(int, g_6949fc, $L_6949fc)

// [FUN_4b9530] gives the shadows to the view's current object again; clears 70ebf4 when 6949fc is set
extern "C" void __cdecl Fx_ResetShadows()
{
    if (Get70ebf8())
        Shadows_SetOwner((BYTE*)Get70ebf8(), 0, F(int, g_view, 2992));
    if (g_6949fc)
        Set70ebf4(0);
}

// ---------------------------------------------------------------------------
ASM_VAR(AsmLabel, s_672554, $L_672554)
ASM_VAR(AsmLabel, s_67251c, $L_67251c)
extern "C" void FUN_4b8720(); KING_ALIAS("_FUN_4b8720", "_Fx_FindByName")
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// [FUN_4b8b80..$L_4b8c26] loads the numbered effects "<672554><n>" from 1 on into 695908 until one is missing (fatal when not even the first exists)
extern "C" void __cdecl Fx_LoadNumbered()
{
    int more = 1;
    g_fx696214 = 0;
    int i = 1;
    BYTE** slot = (BYTE**)&g_fx695908;
    do
    {
        char name[256];
        sprintf(name, &s_fmt67254c, (char*)s_672554, i);
        BYTE* fx = CallC<BYTE*>(FUN_4b8720, name, g_lib696200);
        *slot = fx;
        if (!fx)
            more = 0;
        else
        {
            g_fx696214++;
            F(int, fx, 40) = 249;
            F(DWORD, *slot, 32) |= 0x1000;
            F(DWORD, *slot, 32) |= 0x40000;
            F(DWORD, *slot, 32) |= 0x200;
            i++;
            slot++;
        }
        if (!g_fx696214)
            Sys_Fatal(s_67251c, 0);
    } while (more);
}

