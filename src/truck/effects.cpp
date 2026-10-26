// effects.cpp - aeffects.cpp of the game, rewritten from king.masm: the
// small functions of the effects unit: vector helpers, the constructors and
// destructors of effect and renderer objects, vertex buffers, the pool of
// lights, fade states, the reflection stack, and lookups of vehicles.

#define _CRT_SECURE_NO_WARNINGS     // strncpy as in the game
#include <string.h>
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

// vtables of the effect classes
KING_VAR(AsmLabel, g_vtbl64c248, 64c248)
KING_VAR(AsmLabel, g_vtbl64c2d8, 64c2d8)
KING_VAR(AsmLabel, g_vtbl64c3a4, 64c3a4)
KING_VAR(AsmLabel, g_vtbl64c498, 64c498)
KING_VAR(AsmLabel, g_vtbl64c4e4, 64c4e4)
KING_VAR(AsmLabel, g_vtbl64c530, 64c530)
KING_VAR(AsmLabel, g_vtbl64c63c, 64c63c)
KING_VAR(AsmLabel, g_vtbl64c67c, 64c67c)
KING_VAR(AsmLabel, g_vtbl64c694, 64c694)
KING_VAR(AsmLabel, g_vtbl64c6e0, 64c6e0)
KING_VAR(AsmLabel, g_vtbl64c72c, 64c72c)
KING_VAR(AsmLabel, g_vtbl64c758, 64c758)
KING_VAR(AsmLabel, g_vtbl64c89c, 64c89c)
KING_VAR(AsmLabel, g_vtbl64c8a8, 64c8a8)
KING_VAR(AsmLabel, g_vtbl64d574, 64d574)
KING_VAR(AsmLabel, g_strCRenderer, 672764)  // "CRenderer"
KING_VAR(AsmLabel, g_strCopySound, 672cb8)  // "Copy Sound"

KING_VAR(BYTE*, g_effectList, 6961f4)
KING_VAR(BYTE*, g_game, 6d2078)
KING_VAR(BYTE*, g_view, 696224)
typedef void* EffectBuffers[10];
KING_VAR(EffectBuffers, g_effectBuffers, 696704)
KING_VAR(BYTE*, g_truck, 6f33ac)
KING_VAR(int, g_sound694838, 694838)
KING_VAR(int, g_sound694834, 694834)
KING_VAR(void*, g_sounds, 6f63d0)
KING_VAR(int, g_696244, 696244)
KING_VAR(int, g_6f6a78, 6f6a78)
KING_VAR(void*, g_6961fc, 6961fc)
KING_VAR(BYTE*, g_7025b8, 7025b8)
KING_VAR(void*, g_d3d, 72090c)
KING_VAR(void*, g_lightDevice, 696260)
KING_VAR(int, g_lightsOn, 69625c)
KING_VAR(void*, g_68b990, 68b990)
KING_VAR(BYTE*, g_6cec6c, 6cec6c)
KING_VAR(BYTE*, g_world, 696ca0)
KING_VAR(unsigned, g_67b738, 67b738)

extern "C" void FUN_5b6700(); KING_ALIAS("_FUN_5b6700", "@Window_RootSurface@8")    // the palette of a surface
extern "C" void FUN_59aea0(); KING_ALIAS("_FUN_59aea0", "@Spark59aea0_Construct@44")
extern "C" void FUN_59bed0(); KING_ALIAS("_FUN_59bed0", "@Spark59bed0_Construct@32")
extern "C" void FUN_59ad70(); KING_ALIAS("_FUN_59ad70", "@Spark59ad70_Construct@32")
extern "C" void FUN_59b3f0(); KING_ALIAS("_FUN_59b3f0", "@Flash_Construct@36")
KING_VAR(float, g_randScale, 64c254)
KING_VAR(float, g_randBase, 64c304)
KING_VAR(float, g_one_f, 64c250)
KING_VAR(float, g_696074, 696074)
KING_VAR(float, g_696078, 696078)
KING_VAR(int, g_6f6a80, 6f6a80)
KING_VAR(int, g_6f6a7c, 6f6a7c)
KING_VAR(int, g_6f6a88, 6f6a88)
KING_VAR(int, g_696210, 696210)
KING_VAR(AsmLabel, g_694998, 694998)
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
KING_VAR(AsmLabel, g_vtbl64c244, 64c244)
KING_VAR(AsmLabel, g_vtbl64c240, 64c240)
KING_VAR(AsmLabel, g_vtbl64c628, 64c628)
KING_VAR(AsmLabel, g_vtbl64c650, 64c650)
KING_VAR(AsmLabel, g_vtbl64c668, 64c668)
KING_VAR(AsmLabel, g_7025f0, 7025f0)
extern "C" void FUN_4daf40(); KING_ALIAS("_FUN_4daf40", "@Fx4daf40_Construct@20")
extern "C" void FUN_4dd2a0(); KING_ALIAS("_FUN_4dd2a0", "@Glow_Construct@28")
extern "C" void FUN_579380(); KING_ALIAS("_FUN_579380", "_Pool_Prepare")
extern "C" void FUN_59bb90(); KING_ALIAS("_FUN_59bb90", "@Effect59bb90_Construct@36")
extern "C" void FUN_59c420(); KING_ALIAS("_FUN_59c420", "@Effect59c420_Construct@32")
extern "C" void FUN_59d4a0(); KING_ALIAS("_FUN_59d4a0", "@Effect59d4a0_Construct@28")
extern "C" void FUN_4d9460(); KING_ALIAS("_FUN_4d9460", "@Blow_Construct@16")
extern "C" void FUN_5ef780(); KING_ALIAS("_FUN_5ef780", "@Node_Remove@8")
extern "C" void FUN_5f1a70(); KING_ALIAS("_FUN_5f1a70", "@Node5f1a70_Destroy@4")
extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_5a0000(); KING_ALIAS("_FUN_5a0000", "@VehGlowManager_Construct@12")
extern "C" void FUN_5d3110(); KING_ALIAS("_FUN_5d3110", "@Object_Construct@12")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_4cf400(); KING_ALIAS("_FUN_4cf400", "_Car_StoreState")
extern "C" void FUN_544df0(); KING_ALIAS("_FUN_544df0", "@Vehicle_Destruct@4")
extern "C" void FUN_446c80(); KING_ALIAS("_FUN_446c80", "_Player_SetFlag20000")
KING_VAR(int, g_6f6a84, 6f6a84)
KING_VAR(int, g_6f6a8c, 6f6a8c)
KING_VAR(int, g_6f6a98, 6f6a98)
KING_VAR(int, g_696208, 696208)
KING_VAR(AsmLabel, g_695bf0, 695bf0)
KING_VAR(float, g_696054, 696054)
KING_VAR(float, g_64c258, 64c258)
KING_VAR(AsmLabel, g_vtbl64c428, 64c428)
KING_VAR(AsmLabel, g_vtbl64c464, 64c464)
KING_VAR(AsmLabel, g_vtbl64c770, 64c770)
KING_VAR(AsmLabel, g_vtbl64c8b4, 64c8b4)
KING_VAR(AsmLabel, g_vtbl64c90c, 64c90c)
KING_VAR(AsmLabel, s_6728dc, 6728dc)
KING_VAR(AsmLabel, s_67290c, 67290c)
KING_VAR(AsmLabel, g_vtbl64c97c, 64c97c)
KING_VAR(AsmLabel, s_672988, 672988)
KING_VAR(AsmLabel, g_vtbl64ca58, 64ca58)
KING_VAR(AsmLabel, g_vtbl64cdf8, 64cdf8)
KING_VAR(AsmLabel, g_vtbl64cd0c, 64cd0c)
KING_VAR(AsmLabel, g_vtbl64ccc0, 64ccc0)
KING_VAR(AsmLabel, g_vtbl64ccbc, 64ccbc)
KING_VAR(AsmLabel, g_vtbl64cff4, 64cff4)
KING_VAR(AsmLabel, g_vtbl64cf08, 64cf08)
KING_VAR(AsmLabel, g_vtbl64cebc, 64cebc)
KING_VAR(AsmLabel, g_vtbl64ceb8, 64ceb8)
extern "C" double __fastcall Wave_Value(void* self, void*, double phase);   // engine.cpp
extern "C" void FUN_57c450(); KING_ALIAS("_FUN_57c450", "@FF_PoolStart@28")    // sounds: play
extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")    // Task::~Task
extern "C" void FUN_5ef660(); KING_ALIAS("_FUN_5ef660", "@Sce5ef660_Destroy@4")    // module destructor
extern "C" void FUN_4bf510(); KING_ALIAS("_FUN_4bf510", "@Fx_CameraPair@8")    // particles: reset
extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")    // named object: constructor
extern "C" void FUN_4c09d0(); KING_ALIAS("_FUN_4c09d0", "@Effect4c09d0_Destroy@8")    // CRenderer::~CRenderer
extern "C" void FUN_4ccf30(); KING_ALIAS("_FUN_4ccf30", "@Lights_Choose@12")
extern "C" BYTE* __fastcall SceneObj_Matrix1(void* self, void*, void* out);   // engine.cpp: its world matrix
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")    // vehicles: by id
extern "C" void FUN_5e49d0(); KING_ALIAS("_FUN_5e49d0", "@Resource_Load@12")    // vehicle: release
extern "C" void FUN_5fd370(); KING_ALIAS("_FUN_5fd370", "@Sound_Construct@20")    // sound: constructor
extern "C" void __cdecl Fx_Update4910(void* x);
KING_ALIAS("_Fx_Update4910", "_Eff_CreateSmokeFor")
extern "C" void __cdecl Fx_Update4df0(void* x);
KING_ALIAS("_Fx_Update4df0", "_Eff_CreateSparks")
extern "C" void __cdecl Fx_Message(const char* text, int);
KING_ALIAS("_Fx_Message", "_Sys_Log")
extern "C" int __cdecl Fx_Find4e5bf5(int a, int b, int c, char* buf);
KING_ALIAS("_Fx_Find4e5bf5", "_Obj_Lookup4e5b22")
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
extern "C" void FUN_593720(); KING_ALIAS("_FUN_593720", "@Sparks_Burst@20")
extern "C" void FUN_57c490(); KING_ALIAS("_FUN_57c490", "@Shadow_Add@32")
extern "C" void __fastcall Hits_Remember(BYTE* self, void*, const BYTE* hits, unsigned n);
extern "C" Vec3* __fastcall Vec3_ZeroThis(Vec3* self, void*);
KING_VAR(AsmLabel, g_vtbl64c24c, 64c24c)
KING_VAR(void*, g_lib696204, 696204)
KING_VAR(void*, g_lib696200, 696200)
typedef BYTE* Materials[1];
KING_VAR(Materials, g_lightMats, 695cbc)      // 6 materials of the lights
KING_VAR(Materials, g_vaporMats, 695c58)
KING_VAR(int, g_vaporCount, 69620c)
KING_VAR(AsmLabel, g_strErrFormat, 6724b0)    // " File = %s, Line = #%ld, Date = %s "
KING_VAR(AsmLabel, g_strErrFile, 6724d4)      // the source file
KING_VAR(AsmLabel, g_strErrDate, 6724f8)
KING_VAR(AsmLabel, g_strTrucks, 66cd44)       // "trucks"
KING_VAR(float, g_skid1Level, 696030)
KING_VAR(float, g_skid2Level, 69604c)
KING_VAR(float, g_shadowLevel, 69606c)
KING_VAR(void*, g_6f66a8, 6f66a8)
KING_VAR(void*, g_696234, 696234)

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
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")    // shows an error text (text, a)
extern "C" Matrix* __fastcall Matrix_IdentityThis(Matrix* self, void*);   // vecmath.cpp
KING_VAR(BYTE*, g_glows, 696248)              // 100 records of 116 bytes
KING_VAR(int, g_glowCount, 69624c)
typedef char ErrText[1];
KING_VAR(ErrText, g_errText69608c, 69608c)
KING_VAR(AsmLabel, g_strBadGlow, 6727a0)      // "Invalid number of static glow in module %s, ..."

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

extern "C" void FUN_4ccf30(); KING_ALIAS("_FUN_4ccf30", "@Lights_Choose@12")
KING_VAR(ErrText, g_errText695d30, 695d30)

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

extern "C" void FUN_5587d0(); KING_ALIAS("_FUN_5587d0", "@Truck5587d0_Destroy@4")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
KING_VAR(char, g_vtbl64d234, 64d234)
KING_VAR(char, g_vtbl64d148, 64d148)
KING_VAR(char, g_vtbl64d0fc, 64d0fc)
KING_VAR(char, g_vtbl64d0f8, 64d0f8)
KING_VAR(char, g_vtbl64d424, 64d424)
KING_VAR(char, g_vtbl64d4ec, 64d4ec)
KING_VAR(char, g_vtbl64d530, 64d530)
KING_VAR(BYTE*, g_696734, 696734)
KING_VAR(char, g_702650, 702650)

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

extern "C" void FUN_55eaa0(); KING_ALIAS("_FUN_55eaa0", "@Truck55eaa0_Construct@24")
extern "C" void FUN_543e00(); KING_ALIAS("_FUN_543e00", "@Vehicle_Construct@24")
extern "C" void FUN_4cf260(); KING_ALIAS("_FUN_4cf260", "_Fx_AttachVehicle")
extern "C" void FUN_556110(); KING_ALIAS("_FUN_556110", "@Car_ApplyTypeVec@8")
ASM_PROC(FUN_5586f0)
extern "C" void FUN_5fd870(); KING_ALIAS("_FUN_5fd870", "@Sound_UpdateAt@36")    // sound: play (position by value, ...)
extern "C" void FUN_4dd9c0(); KING_ALIAS("_FUN_4dd9c0", "@CopySound_Construct@12")
KING_VAR(char, g_vtbl64cbfc, 64cbfc)
KING_VAR(char, g_vtbl64cb10, 64cb10)
KING_VAR(char, g_vtbl64cac4, 64cac4)
KING_VAR(char, g_vtbl64cac0, 64cac0)

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

KING_VAR(char, g_vtbl64c358, 64c358)

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
extern "C" void FUN_4ebd00(); KING_ALIAS("_FUN_4ebd00", "@PhysBody_Construct@64")
extern "C" void FUN_4ebaa0(); KING_ALIAS("_FUN_4ebaa0", "@Engine_SetKeys@16")
KING_VAR(char, gv64c604, 64c604)
KING_VAR(char, gv64c5f0, 64c5f0)
KING_VAR(char, gv64c59c, 64c59c)
KING_VAR(char, gv64d2f4, 64d2f4)

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

KING_VAR(BYTE*, g_resources696ca0, 696ca0)
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
KING_VAR(FireRamp, g_fireRamp1, 695fac)
KING_VAR(FireRamp, g_fireRamp2, 695324)
KING_VAR(FireRamp, g_fireRamp3, 695e2c)

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

KING_VAR(const double, k_hslEpsilon, 64c410)      // 1e-7

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
KING_VAR(Vec3, g_up702640, 702640)
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
KING_VAR(Vec3, g_ahead702620, 702620)

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

extern "C" void FUN_4be2c0(); KING_ALIAS("_FUN_4be2c0", "_Object_HeightAt")    // the distance of a segment to a box-like object
extern "C" void FUN_616850(); KING_ALIAS("_FUN_616850", "@Terrain_HeightAt@16")    // the distance of a segment to the terrain of a type-8 object

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

KING_VAR(float, g_detail696088, 696088)

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

extern "C" void FUN_4d57c0(); KING_ALIAS("_FUN_4d57c0", "_Gear_Choose")    // the effect's swing step
extern "C" BYTE* __fastcall Car_543bb0(BYTE* self, void*);
extern "C" void FUN_5fd5f0(); KING_ALIAS("_FUN_5fd5f0", "@Sound_PlayAt@40")    // a sound at a point (point, distance, volume, flags)

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
KING_VAR(BYTE*, g_thrownLamp696734, 696734)
KING_VAR(float, g_detail696078, 696078)
KING_VAR(char, g_strThrowLamp, 672c60)        // "ThrowLamp"
KING_VAR(char, g_vtbl64d444, 64d444)

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
KING_VAR(float, g_fx69605c, 69605c)
KING_VAR(float, g_fx696048, 696048)
KING_VAR(BYTE*, g_pool6f6ab4, 6f6ab4)
KING_VAR(BYTE*, g_pool6f6a7c, 6f6a7c)
KING_VAR(BYTE*, g_pool6f6a80, 6f6a80)
KING_VAR(BYTE*, g_pool6f6a88, 6f6a88)
KING_VAR(FxLabel, g_fx694998, 694998)
KING_VAR(FxLabel, g_fx695bf0, 695bf0)
KING_VAR(FxLabel, g_fx695c58, 695c58)
typedef DWORD FxDwords[1];
KING_VAR(FxDwords, g_fx695908, 695908)
KING_VAR(unsigned, g_fx696214, 696214)
KING_VAR(int, g_fx69620c, 69620c)
KING_VAR(int, g_fx696208, 696208)
KING_VAR(int, g_fx696210, 696210)

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

KING_VAR(char, g_strMnFire, 672c58)        // "mnFire"

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
KING_VAR(float, g_fx69603c, 69603c)
KING_VAR(unsigned, g_sparkLimit671700, 671700)
KING_VAR(unsigned, g_sparkCount6f6a6cE, 6f6a6c)
KING_VAR(BYTE*, g_pool6f6a9c, 6f6a9c)
KING_VAR(int, g_fx694830, 694830)
KING_VAR(int, g_fx696218, 696218)
KING_VAR(FxLabel, g_fx694a00, 694a00)

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
KING_VAR(float, g_fx696084, 696084)
KING_VAR(BYTE*, g_fx7025b8, 7025b8)
KING_VAR(int, g_fx6f33c0, 6f33c0)
KING_VAR(BYTE*, g_pool6f6a94, 6f6a94)
KING_VAR(BYTE*, g_pool6f6a90, 6f6a90)
KING_VAR(FxDwords, g_fx695c54, 695c54)
KING_VAR(Vec3, g_axis702630e, 702630)

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
extern "C" void FUN_4cf080(); KING_ALIAS("_FUN_4cf080", "_Car_Launch")
extern "C" void __fastcall Car_SetInMode(BYTE* car, void*, int mode);
extern "C" void __fastcall Flags1196_Clear(BYTE* self, void*, int bits);
extern "C" void FUN_58f290(); KING_ALIAS("_FUN_58f290", "@Body_SetMoveMode@12")
KING_VAR(int, g_noVehicle68b990, 68b990)
KING_VAR(BYTE*, g_fx696340, 696340)
KING_VAR(int, g_fx696328, 696328)
KING_VAR(int, g_fx6962cc, 6962cc)

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

extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")    // the scene object base destructor
extern "C" void FUN_5f1a70(); KING_ALIAS("_FUN_5f1a70", "@Node5f1a70_Destroy@4")
KING_VAR(char, vt_64c90c, 64c90c)
KING_VAR(char, vt_64d49c, 64d49c)
KING_VAR(char, vt_64d4c8, 64d4c8)
KING_VAR(char, vt_64d554, 64d554)
KING_VAR(char, vt_64c358, 64c358)

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

KING_VAR(char, vt_64c63c, 64c63c)
extern "C" void FUN_5ef660(); KING_ALIAS("_FUN_5ef660", "@Sce5ef660_Destroy@4")
extern "C" void FUN_60aa60(); KING_ALIAS("_FUN_60aa60", "_Grd_Message")
KING_VAR(char, g_6724f8, 6724f8)
KING_VAR(char, g_6726e8, 6726e8)
KING_VAR(char, g_6726e0, 6726e0)
KING_VAR(char, g_6726a4, 6726a4)

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
extern "C" void FUN_5f36d0(); KING_ALIAS("_FUN_5f36d0", "@Obj_DrawTested@16")

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

KING_VAR(void*, g_fx6f63d0, 6f63d0)
KING_VAR(void*, g_fx696254, 696254)
KING_VAR(void*, g_fx6961f8, 6961f8)
KING_VAR(int, g_fx696238, 696238)
KING_VAR(int, g_fx69622c, 69622c)
KING_VAR(int, g_fx696230, 696230)
KING_VAR(int, g_fx696234, 696234)
extern "C" void FUN_5c64f0(); KING_ALIAS("_FUN_5c64f0", "_Effects_FreeTrees")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_4bcbe0(); KING_ALIAS("_FUN_4bcbe0", "@LightPool_Free@8")
extern "C" void FUN_4c4780(); KING_ALIAS("_FUN_4c4780", "_Eff_ClearAndDestroyGlows")
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

KING_VAR(char, s_fmt672504, 672504)
KING_VAR(char, s_fmt67254c, 67254c)
KING_VAR(char, s_fmt67255c, 67255c)
KING_VAR(char, s_name67250c, 67250c)
KING_VAR(char, s_name672514, 672514)
KING_VAR(char, s_name672564, 672564)
KING_VAR(char, s_name6725c0, 6725c0)
KING_VAR(int, g_fxCount69621c, 69621c)
KING_VAR(int, g_fxCount696210, 696210)
KING_VAR(int, g_fxCount696218, 696218)
KING_VAR(int, g_fxCount696208, 696208)
typedef BYTE* MatArray[1];
KING_VAR(MatArray, g_mats69618c, 69618c)
KING_VAR(MatArray, g_mats694998, 694998)
KING_VAR(MatArray, g_mats694a00, 694a00)
KING_VAR(MatArray, g_mats695bf0, 695bf0)

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
KING_VAR(int, g_6949fc, 6949fc)

// [FUN_4b9530] gives the shadows to the view's current object again; clears 70ebf4 when 6949fc is set
extern "C" void __cdecl Fx_ResetShadows()
{
    if (Get70ebf8())
        Shadows_SetOwner((BYTE*)Get70ebf8(), 0, F(int, g_view, 2992));
    if (g_6949fc)
        Set70ebf4(0);
}

// ---------------------------------------------------------------------------
KING_VAR(AsmLabel, s_672554, 672554)
KING_VAR(AsmLabel, s_67251c, 67251c)
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


// ---------------------------------------------------------------------------
// [FUN_4c4ac0] 0
extern "C" int __cdecl Fx_Zero()
{
    return 0;
}


// ---------------------------------------------------------------------------
// Small virtual methods of the effect classes and the static initializers of
// effect constants.

extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")    // releases an object (flags)
extern "C" void FUN_4cb8d0(); KING_ALIAS("_FUN_4cb8d0", "_Car_ScrapeSparks")
extern "C" void FUN_5fcdb0(); KING_ALIAS("_FUN_5fcdb0", "@Sound_ResetSlot@8")
extern "C" void FUN_4dd9f0(); KING_ALIAS("_FUN_4dd9f0", "@Obj4dd9f0_Destruct@8")
extern "C" void FUN_4ce6d0(); KING_ALIAS("_FUN_4ce6d0", "_Car_DrawForces")
extern "C" void FUN_547030(); KING_ALIAS("_FUN_547030", "@Car_Draw@16")
extern "C" void FUN_5f0870(); KING_ALIAS("_FUN_5f0870", "@Thunk5f0870@4")
extern "C" void FUN_4dc410(); KING_ALIAS("_FUN_4dc410", "@SmokeBurst_Destroy@4")
extern "C" void FUN_4ddd50(); KING_ALIAS("_FUN_4ddd50", "@Eff4ddd50_Destroy@4")
KING_VAR(float, g_fx696700, 696700)
KING_VAR(float, g_fx69637c, 69637c)
KING_VAR(float, g_fx6966fc, 6966fc)
KING_VAR(float, g_fx6966f8, 6966f8)
KING_VAR(float, g_fx672c48, 672c48)
KING_VAR(float, g_fx672c4c, 672c4c)
KING_VAR(float, g_fx69672c, 69672c)
KING_VAR(float, g_fx69636c, 69636c)
KING_VAR(float, g_fx696368, 696368)
KING_VAR(float, g_fx696374, 696374)
KING_VAR(float, g_fx696378, 696378)
KING_VAR(float, g_fx64c308, 64c308)
KING_VAR(float, g_fx64c1e4, 64c1e4)
KING_VAR(int, g_fx64c1bc, 64c1bc)
KING_VAR(int, g_fx68b990, 68b990)
KING_VAR(FxDwords, g_fx695d20, 695d20)
KING_VAR(FxDwords, g_fx702650, 702650)
KING_VAR(FxDwords, g_fx696740, 696740)

static BYTE* Fx_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [$L_4b9ff0]
extern "C" BYTE* __fastcall Fx4b9ff0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Fx_DestructA); }
// [$L_4bcbb0]
extern "C" BYTE* __fastcall Fx4bcbb0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4bcbd0_Destroy); }
// [$L_4bd7f0]
extern "C" BYTE* __fastcall Fx4bd7f0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Fx_DestructB); }
// [$L_4bea40]
extern "C" BYTE* __fastcall Fx4bea40_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4bedb0_Destroy); }
// [$L_4bef80]
extern "C" BYTE* __fastcall Fx4bef80_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4bf3f0_Destroy); }
// [$L_4c08d0]
extern "C" BYTE* __fastcall Fx4c08d0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, FUN_4c09d0); }
// [$L_4c0910]
extern "C" BYTE* __fastcall Fx4c0910_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())RendererA_Destruct); }
// [$L_4c0950]
extern "C" BYTE* __fastcall Fx4c0950_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())RendererB_Destruct); }
// [$L_4c26c0]
extern "C" BYTE* __fastcall Fx4c26c0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())List_Destruct); }
// [$L_4c2890]
extern "C" BYTE* __fastcall Fx4c2890_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())RendererC_Destruct); }
// [$L_4c29f0]
extern "C" BYTE* __fastcall Fx4c29f0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())RendererD_Destruct); }
// [$L_4c4180]
extern "C" BYTE* __fastcall Fx4c4180_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, FUN_4c1790); }
// [$L_4c41a0]
extern "C" BYTE* __fastcall Fx4c41a0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4c45d0_Destroy); }
// [$L_4c4ad0]
extern "C" BYTE* __fastcall Fx4c4ad0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, FUN_5f0870); }
// [$L_4c4c20]
extern "C" BYTE* __fastcall Fx4c4c20_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4c4c40_Destroy); }
// [$L_4c8170]
extern "C" BYTE* __fastcall Fx4c8170_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Fx_SetVtbl8a8); }
// [$L_4c8870]
extern "C" BYTE* __fastcall Fx4c8870_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4c8890_Destroy); }
// [$L_4c99e0]
extern "C" BYTE* __fastcall Fx4c99e0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4c9a00_Destroy); }
// [$L_4cc9d0]
extern "C" BYTE* __fastcall Fx4cc9d0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4cc9f0_Destroy); }
// [$L_4d9cb0]
extern "C" BYTE* __fastcall Fx4d9cb0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Task4d9ec0_Destroy); }
// [$L_4d9e50]
extern "C" BYTE* __fastcall Fx4d9e50_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4d9e70_Destroy); }
// [$L_4da7b0]
extern "C" BYTE* __fastcall Fx4da7b0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4da7d0_Destroy); }
// [$L_4db870]
extern "C" BYTE* __fastcall Fx4db870_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4db890_Destroy); }
// [$L_4dbf10]
extern "C" BYTE* __fastcall Fx4dbf10_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())TankNotify_Destroy); }
// [$L_4dc3f0]
extern "C" BYTE* __fastcall Fx4dc3f0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, FUN_4dc410); }
// [$L_4dc840]
extern "C" BYTE* __fastcall Fx4dc840_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Notify4dc860_Destroy); }
// [$L_4dd5e0]
extern "C" BYTE* __fastcall Fx4dd5e0_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, (void (*)())Effect4dd600_Destroy); }
// [$L_4ddd30]
extern "C" BYTE* __fastcall Fx4ddd30_DeletingDestructor(BYTE* s, void*, int flags) { return Fx_DeleteAfter(s, flags, FUN_4ddd50); }

// [$L_4c85c0] the base destructor (vtable 64c8a8) inlined
extern "C" BYTE* __fastcall Fx4c85c0_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_vtbl64c8a8;
    if (flags & 1)
        operator delete(s);
    return s;
}

// [$L_4c17a0] the base destructor (vtable 64c63c) inlined
extern "C" BYTE* __fastcall Fx4c17a0_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_vtbl64c63c;
    F(int, s, 4) = 0;
    if (flags & 1)
        operator delete(s);
    return s;
}

// [$L_4dda10] destroys and frees itself after FUN_5fcdb0
extern "C" void __fastcall Fx4dda10_Destroy(BYTE* s)
{
    CallThis(FUN_5fcdb0, s);
    if (s) {
        CallThis(FUN_4dd9f0, s);
        operator delete(s);
    }
}

// [$L_4b9b60]
extern "C" int __fastcall Fx4b9b60_Count(BYTE* s) { return F(int, s, 52); }
// [$L_4bd560]
extern "C" int __fastcall Fx4bd560_Get56(BYTE* s) { return F(int, s, 56); }
// [$L_4bd520]
extern "C" int __fastcall Fx4bd520_Zero(BYTE*, void*, int, int, int) { return 0; }
// [$L_4bd530]
extern "C" int __fastcall Fx4bd530_Zero(BYTE*, void*, int) { return 0; }
// [$L_4c5050]
extern "C" int __fastcall Fx4c5050_Zero(BYTE*, void*, int, int) { return 0; }
// [$L_4bd550]
extern "C" int __fastcall Fx4bd550_None(BYTE*) { return -1; }
// [$L_4c1270]
extern "C" void __fastcall Fx4c1270_Clear4(BYTE* s) { F(int, s, 4) = 0; }
// [$L_4ddd10] clears flag bits 0 and 1 of +16
extern "C" void __fastcall Fx4ddd10_ClearFlags(BYTE* s) { F(int, s, 16) &= ~3; }
// [$L_4ddce0] the words of the record at +12
extern "C" int __fastcall Fx4ddce0_Word0(BYTE* s) { return F(WORD, F(BYTE*, s, 12), 0); }
// [$L_4ddcf0]
extern "C" int __fastcall Fx4ddcf0_Word1(BYTE* s) { return F(WORD, F(BYTE*, s, 12), 2); }
// [$L_4ddd00]
extern "C" int __fastcall Fx4ddd00_Word2(BYTE* s) { return F(WORD, F(BYTE*, s, 12), 4); }
// [$L_4ddd20] forwards to vtbl+52
extern "C" int __fastcall Fx4ddd20_Forward52(BYTE* s, void*, int a) { return CallVirt<int>(s, 52, a); }
// [$L_4bd540] calls fn(self, arg)
extern "C" int __fastcall Fx4bd540_Apply(BYTE* s, void*, void (*fn)(), int arg) { return CallC<int>(fn, s, arg); }
// [$L_4c3440] the member at +64 gets +4 through its vtbl+16
extern "C" int __fastcall Fx4c3440_Pass(BYTE* s) { return CallVirt<int>(s + 64, 16, F(int, s, 4)); }

// [$L_4c30a0] sets +40; a zero also clears +16
extern "C" int __fastcall Fx4c30a0_Set40(BYTE* s, void*, int a)
{
    F(int, s, 40) = a;
    if (!a)
        F(int, s, 16) = a;
    return a;
}

// [$L_4b9b40] the 12 bytes at +20 to out
extern "C" int __fastcall Fx4b9b40_Get20(BYTE* s, void*, DWORD* out)
{
    out[0] = F(DWORD, s, 20);
    out[1] = F(DWORD, s, 24);
    out[2] = F(DWORD, s, 28);
    return 1;
}

// [$L_4b9b70] item i of the list at +48 (count +52), 0 past the end
extern "C" int __fastcall Fx4b9b70_Item(BYTE* s, void*, int i)
{
    if (i >= F(int, s, 52))
        return 0;
    return F(int*, s, 48)[i];
}

// [$L_4ba040]
extern "C" int __fastcall Fx4ba040_Release(BYTE* s)
{
    Fx_ResetShadows();
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// [$L_4b83d0]
extern "C" int __fastcall Fx4b83d0_Draw(BYTE* s)
{
    return CallC<int>(FUN_4cb8d0, F(int, s, 8), s + 12, s + 24, F(int, s, 36), F(int, s, 40));
}

// [$L_4d2760] (a, b): FUN_4ce6d0 on the object 272 bytes before, then FUN_547030
extern "C" int __fastcall Fx4d2760_Pass(BYTE* s, void*, int a, int b)
{
    CallC(FUN_4ce6d0, s - 272, b);
    return CallThis<int>(FUN_547030, s, a, b);
}

// [$L_4c3360] advances p's +20 by k * rate (+36); 1 while it stays past +32
extern "C" int __fastcall Fx4c3360_Advance(BYTE* s, void*, BYTE* p, float k)
{
    double t = (double)k * F(float, s, 36) + F(float, p, 20);
    F(float, p, 20) = (float)t;
    return t > F(float, s, 32) ? 1 : 0;
}

// [$L_4c30c0] advances p's +20 by k * rate (+36); 1 while p's +16 is under +24 and the sum under +32
extern "C" int __fastcall Fx4c30c0_Advance(BYTE* s, void*, BYTE* p, float k)
{
    double t = (double)k * F(float, s, 36) + F(float, p, 20);
    F(float, p, 20) = (float)t;
    if (F(float, p, 16) >= F(float, s, 24))
        return 0;
    if (t >= F(float, s, 32))
        return 0;
    return 1;
}

// [$L_4dbee0] the time (+2960 of the owner +40) added to +44: past +48 vtbl+28 (1), else released
extern "C" int __fastcall Fx4dbee0_Tick(BYTE* s)
{
    double t = F(double, F(BYTE*, s, 40), 2960) + F(float, s, 44);
    F(float, s, 44) = (float)t;
    if (t > F(float, s, 48))
        return CallVirt<int>(s, 28, 1);
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// [$L_4dc810] the same with the owner at +44, time +48, limit +52
extern "C" int __fastcall Fx4dc810_Tick(BYTE* s)
{
    double t = F(double, F(BYTE*, s, 44), 2960) + F(float, s, 48);
    F(float, s, 48) = (float)t;
    if (t > F(float, s, 52))
        return CallVirt<int>(s, 28, 1);
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// [$L_4da410] static initializers of effect values
extern "C" void __cdecl Fx_Init696700() { g_fx696700 = 0; }
// [$L_4da460]
extern "C" void __cdecl Fx_Init69637c() { g_fx69637c = 0; }
// [$L_4dbfa0]
extern "C" void __cdecl Fx_Init6966fc() { g_fx6966fc = 0; }
// [$L_4dbff0]
extern "C" void __cdecl Fx_Init6966f8() { g_fx6966f8 = 0; }
// [$L_4cf240]
extern "C" void __cdecl Fx_Init6962cc() { g_fx6962cc = g_fx68b990; }
// [$L_4b86a0]
extern "C" void __cdecl Fx_Init694830() { g_fx694830 = g_fx64c1bc; }
// [$L_4bc670]
extern "C" void __cdecl Fx_Init695c54() { *(float*)g_fx695c54 = (float)x87_cos(g_fx64c1e4); }
// [$L_4da430]
extern "C" void __cdecl Fx_Init69672c() { g_fx69672c = (float)((double)g_fx672c48 - g_fx696700); }
// [$L_4da480]
extern "C" void __cdecl Fx_Init69636c() { g_fx69636c = (float)((double)g_fx672c48 - g_fx69637c); }
// [$L_4dbfc0]
extern "C" void __cdecl Fx_Init696368() { g_fx696368 = (float)((double)g_fx672c4c - g_fx6966fc); }
// [$L_4dc010]
extern "C" void __cdecl Fx_Init696374() { g_fx696374 = (float)((double)g_fx672c4c - g_fx6966f8); }
// [$L_4dc040]
extern "C" void __cdecl Fx_Init696378() { g_fx696378 = (float)((double)g_fx672c4c * g_fx64c308); }
// [$L_4c80a0]
extern "C" void __cdecl Fx_Init695d20()
{
    DWORD* d = g_fx695d20;
    d[0] = d[1] = d[2] = d[3] = 0;
}
// [$L_4dda80]
extern "C" void __cdecl Fx_Init696740()
{
    DWORD* src = g_fx702650;
    DWORD* dst = g_fx696740;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

KING_VAR(DWORD, g_eff696278, 696278)
KING_VAR(DWORD, g_eff696280, 696280)
KING_VAR(DWORD, g_eff696284, 696284)
KING_VAR(DWORD, g_eff696288, 696288)
KING_VAR(DWORD, g_eff69628c, 69628c)
KING_VAR(DWORD, g_eff696290, 696290)
KING_VAR(DWORD, g_eff696294, 696294)
KING_VAR(DWORD, g_eff696298, 696298)
KING_VAR(DWORD, g_eff69629c, 69629c)
KING_VAR(DWORD, g_eff6962a0, 6962a0)
KING_VAR(DWORD, g_eff6962a4, 6962a4)
KING_VAR(DWORD, g_eff6962a8, 6962a8)
KING_VAR(DWORD, g_eff6962ac, 6962ac)
KING_VAR(DWORD, g_eff6962b0, 6962b0)
KING_VAR(DWORD, g_eff6962b4, 6962b4)
KING_VAR(DWORD, g_eff6962b8, 6962b8)
KING_VAR(float, g_eff64c308, 64c308)
KING_VAR(float, g_eff672c48, 672c48)
KING_VAR(float, g_eff696370, 696370)

// [$L_4d2a70]
extern "C" void __cdecl EffInit_4d2a70() { g_eff696280 = 0x0; g_eff696284 = 0x0; g_eff696288 = 0x0; g_eff69628c = 0x0; g_eff696290 = 0x0; g_eff696294 = 0x0; g_eff696298 = 0x0; g_eff69629c = 0x0; g_eff6962a0 = 0x0; g_eff6962a4 = 0x0; g_eff6962a8 = 0x0; g_eff6962ac = 0x0; g_eff6962b0 = 0x0; g_eff6962b4 = 0x0; g_eff6962b8 = 0x0; g_eff696278 = 0x1; }
// [$L_4da4b0]
extern "C" void __cdecl EffInit_4da4b0() { g_eff696370 = (float)((double)g_eff672c48 * g_eff64c308); }

extern "C" void FUN_4dde10(); KING_ALIAS("_FUN_4dde10", "@Cab_Release@8")
extern "C" void FUN_4cb560(); KING_ALIAS("_FUN_4cb560", "_Dust_Puffs")
extern "C" void FUN_546b60(); KING_ALIAS("_FUN_546b60", "@CarPart_Draw@24")
extern "C" void FUN_60f000(); KING_ALIAS("_FUN_60f000", "@MarineTask_ForwardPoint@12")
KING_VAR(AsmLabel, g_eff695978, 695978)
KING_VAR(int, g_eff69624c, 69624c)

// [$L_4c80e0] static initializer: 40 zero 16-byte records at 695970
extern "C" void __cdecl EffInit_4c80e0()
{
    DWORD* p = (DWORD*)g_eff695978 - 2;
    for (int i = 0; i < 160; i++)
        p[i] = 0;
}

// [$L_4dddd0] after FUN_4dde10 the object at +4860 gets vtbl+36, is deleted and cleared; then vtbl+32
extern "C" int __fastcall Eff_4dddd0(BYTE* s)
{
    CallThis(FUN_4dde10, s);
    if (F(BYTE*, s, 4860)) {
        CallVirt(F(BYTE*, s, 4860), 36);
        if (F(BYTE*, s, 4860))
            CallVirt(F(BYTE*, s, 4860), 0, 1);
        F(int, s, 4860) = 0;
    }
    return CallVirt<int>(s, 32);
}

// [$L_4b84c0]
extern "C" int __fastcall Eff_4b84c0(BYTE* s)
{
    int v = F(int, s, 40) ? F(int, s, 36) : 0;
    return CallC<int>(FUN_4cb560, s + 8, s + 20, F(int, s, 32), v, F(int, s, 44));
}

// [$L_4d24f0] in mode 5 (+21608) +20652 takes +21652 first
extern "C" int __fastcall Eff_4d24f0(BYTE* s, void*, int a, int b, int c, int d)
{
    if (F(int, s, 21608) == 5)
        F(int, s, 20652) = F(int, s, 21652);
    return CallThis<int>(FUN_546b60, s, a, b, c, d);
}

// [$L_4c3450] sets +40; a zero first stops the member at +64 (vtbl+12) and clears +16
extern "C" void __fastcall Eff_4c3450(BYTE* s, void*, int a)
{
    if (!a)
        CallVirt(s + 64, 12);
    F(int, s, 40) = a;
    if (!a)
        F(int, s, 16) = a;
}
// [$L_4c3940] the same with the object pointed to by +64
extern "C" void __fastcall Eff_4c3940(BYTE* s, void*, int a)
{
    if (!a)
        CallVirt(F(BYTE*, s, 64), 12);
    F(int, s, 40) = a;
    if (!a)
        F(int, s, 16) = a;
}

// [$L_4c4bd0] vtbl+36 of each 140-byte item (+32, count +40), then released
extern "C" int __fastcall Eff_4c4bd0(BYTE* s)
{
    for (unsigned i = 0; i < F(unsigned, s, 40); i++)
        CallVirt(F(BYTE*, s, 32) + i * 140, 36);
    F(int, s, 40) = 0;
    g_eff69624c = 0;
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// [$L_4c3840] advances p's +20 by k * rate (+36), p's +12 from FUN_60f000 of +68; 1 while +16 and +20 are under +24 and +32
extern "C" int __fastcall Eff_4c3840(BYTE* s, void*, BYTE* p, float k)
{
    F(float, p, 20) = (float)((double)k * F(float, s, 36) + F(float, p, 20));
    F(float, p, 12) = (float)CallThis<double>(FUN_60f000, F(BYTE*, s, 68), p + 4);
    if (F(float, p, 16) >= F(float, s, 24))
        return 0;
    if (F(float, p, 20) >= F(float, s, 32))
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// More virtual methods of the effects: emitters, drawing, particle sources.

extern "C" void FUN_4c8af0(); KING_ALIAS("_FUN_4c8af0", "_Fx_Beam")
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")
extern "C" void FUN_5f98c0(); KING_ALIAS("_FUN_5f98c0", "@SceneNode5f98c0_Draw@16")
extern "C" void FUN_5ae580(); KING_ALIAS("_FUN_5ae580", "@Render_PushMatrix@16")
extern "C" void FUN_5ae8a0(); KING_ALIAS("_FUN_5ae8a0", "@Clip_Pop@8")
extern "C" void FUN_5f3570(); KING_ALIAS("_FUN_5f3570", "@Obj_Test@16")
extern "C" void FUN_4c5680(); KING_ALIAS("_FUN_4c5680", "@Thunk4c5680@4")
extern "C" void Eff_rand(); KING_ALIAS("_Eff_rand", "_rand")
extern "C" void Eff_VecDtor(); KING_ALIAS("_Eff_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")
KING_VAR(AsmLabel, g_eff6962d0, 6962d0)
KING_VAR(float, g_eff64c254, 64c254)
KING_VAR(float, g_eff64c334, 64c334)
KING_VAR(float, g_eff64c350, 64c350)
KING_VAR(float, g_eff64c748, 64c748)

// [$L_4d2990] static initializer: 6962d0..696310 zero, 696314.. the vector at 702650
extern "C" void __cdecl EffInit_4d2990()
{
    DWORD* d = (DWORD*)g_eff6962d0;
    for (int i = 0; i < 17; i++)
        d[i] = 0;
    d[17] = g_fx702650[0];
    d[18] = g_fx702650[1];
    d[19] = g_fx702650[2];
}

// [$L_4c85e0] draws the strip (+2216 vertices at +4, +2212 indices at +2084) on the device
extern "C" void __fastcall Eff_DrawStrip(BYTE* s, void*, void* dev)
{
    if (!F(int, s, 2216) || !F(int, s, 2212))
        return;
    void** vt = *(void***)dev;
    ((int(__stdcall*)(void*, int, int))vt[20])(dev, 136, 1);
    vt = *(void***)dev;
    ((int(__stdcall*)(void*, int, int, BYTE*, int, BYTE*, int, int))vt[26])(dev, 4, 578, s + 4, F(int, s, 2216), s + 2084, F(int, s, 2212), 0);
}

// [$L_4b9fa0] destroys and frees each item (+32, count +36), then released
extern "C" int __fastcall Eff_4b9fa0(BYTE* s)
{
    for (unsigned i = 0; i < F(unsigned, s, 36); i++) {
        BYTE* o = F(BYTE**, s, 32)[i];
        if (o) {
            CallVirt(o, 0);
            operator delete(F(BYTE**, s, 32)[i]);
        }
    }
    F(int, s, 36) = 0;
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// [$L_4c3320] sets +40; a zero first stops the members at +64 and +4672 (vtbl+12) and clears +16
extern "C" void __fastcall Eff_4c3320(BYTE* s, void*, int a)
{
    if (!a) {
        CallVirt(s + 64, 12);
        CallVirt(s + 4672, 12);
    }
    F(int, s, 40) = a;
    if (!a)
        F(int, s, 16) = a;
}

// [$L_4c4b50] the position: the matrix +88.. (rows of 3) times the vector +20, plus +124
extern "C" int __fastcall Eff_Position(BYTE* s, void*, float* out)
{
    double x = (double)F(float, s, 100) * F(float, s, 24) + (double)F(float, s, 112) * F(float, s, 28)
             + (double)F(float, s, 20) * F(float, s, 88) + F(float, s, 124);
    double y = (double)F(float, s, 104) * F(float, s, 24) + (double)F(float, s, 92) * F(float, s, 20)
             + (double)F(float, s, 116) * F(float, s, 28) + F(float, s, 128);
    double z = (double)F(float, s, 108) * F(float, s, 24) + (double)F(float, s, 96) * F(float, s, 20)
             + (double)F(float, s, 120) * F(float, s, 28) + F(float, s, 132);
    out[0] = (float)x;
    out[1] = (float)y;
    out[2] = (float)z;
    return 0;
}

// [$L_4b82e0]
extern "C" int __fastcall Eff_4b82e0(BYTE* s)
{
    BYTE* v = F(int, s, 256) ? s + 48 : 0;
    return CallC<int>(FUN_4c8af0, s + 8, F(int, s, 20), F(int, s, 24), s + 28, F(int, s, 40), F(int, s, 44), v);
}

// [$L_4c1cd0] a random one of the first items (+4, at most 50) through vtbl+4 (i, a, b)
extern "C" int __fastcall Eff_RandomItem(BYTE* s, void*, int a, int b)
{
    unsigned n = F(unsigned, s, 4);
    if (!n)
        return 0;
    int m = n > 49 ? 49 : (int)n - 1;
    int r = CallC<int>(Eff_rand);
    int i = (int)((double)r * m * g_eff64c254);
    return CallVirt<int>(s, 4, i, a, b);
}

// a particle from the source src (vtbl+0 (pos, dir)): out +4.. the position, +16 the size, +20 w
static int Eff_Particle(BYTE* src, BYTE* out, DWORD size, DWORD w, const float* lift)
{
    float v[3] = {0, 0, 0}, d[3] = {0, 0, 0};
    if (!CallVirt<int>(src, 0, v, d))
        return 0;
    F(DWORD, out, 16) = size;
    F(DWORD, out, 20) = w;
    F(float, out, 4) = v[0];
    F(float, out, 8) = v[1];
    F(float, out, 12) = lift ? (float)(v[2] + (double)*lift) : v[2];
    return 1;
}
// the size +28 scaled by 64c350, as float bits
static DWORD Eff_Scaled28(BYTE* s)
{
    float f = (float)((double)F(float, s, 28) * g_eff64c350);
    return *(DWORD*)&f;
}
// [$L_4c3390] from the member at +64, lifted by 64c334, weight 1
extern "C" int __fastcall Eff_4c3390(BYTE* s, void*, BYTE* out) { return Eff_Particle(s + 64, out, F(DWORD, s, 28), 0x3f800000, &g_eff64c334); }
// [$L_4c3890] from the object at +64, size scaled by 64c350
extern "C" int __fastcall Eff_4c3890(BYTE* s, void*, BYTE* out)
{
    return Eff_Particle(F(BYTE*, s, 64), out, Eff_Scaled28(s), 0, 0);
}
// [$L_4c3ac0] the same from the member at +64
extern "C" int __fastcall Eff_4c3ac0(BYTE* s, void*, BYTE* out)
{
    return Eff_Particle(s + 64, out, Eff_Scaled28(s), 0, 0);
}
// [$L_4c3100] from the member at +4672 when a random number passes 64c748, else from +64
extern "C" int __fastcall Eff_4c3100(BYTE* s, void*, BYTE* out)
{
    int r = CallC<int>(Eff_rand);
    BYTE* src = s + 4672;
    if (!((double)r * g_eff64c254 > g_eff64c748))
        src = s + 64;
    return Eff_Particle(src, out, F(DWORD, s, 28), 0, 0);
}

// vector deleting destructor of an array of n-byte items with destructor dtor
static BYTE* Eff_VecDelete(BYTE* s, int flags, unsigned size, void (*dtor)())
{
    if (flags & 2) {
        BYTE* base = s - 4;
        CallStd(Eff_VecDtor, s, size, F(int, s, -4), dtor);
        if (flags & 1)
            operator delete(base);
        return base;
    }
    CallThis(dtor, s);
    if (flags & 1)
        operator delete(s);
    return s;
}
// [$L_4c4790]
extern "C" BYTE* __fastcall Eff4c4790_VectorDeletingDestructor(BYTE* s, void*, int flags) { return Eff_VecDelete(s, flags, 140, FUN_5f0870); }
// [$L_4c5690]
extern "C" BYTE* __fastcall Eff4c5690_VectorDeletingDestructor(BYTE* s, void*, int flags) { return Eff_VecDelete(s, flags, 116, FUN_4c5680); }

// [$L_4c4af0] frame 1 (+136), b gets +88, FUN_5f98c0 (a, b), b restored, frame 0
extern "C" int __fastcall Eff_4c4af0(BYTE* s, void*, int a, BYTE* b)
{
    CallC(FUN_5e1a10, F(int, s, 136), 1);
    CallThis(FUN_5ae580, b, s + 88, 0);
    int r = CallThis<int>(FUN_5f98c0, s, a, b);
    CallThis(FUN_5ae8a0, b);
    CallC(FUN_5e1a10, F(int, s, 136), 0);
    return r;
}

// [$L_4bd760] the two lights (+88/+92 color, +76/+80 source) go into the owner's (+44) lists of ten
extern "C" int __fastcall Eff_4bd760(BYTE* s, void*, int a, int b)
{
    for (int k = 0; k < 2; k++) {
        unsigned e = F(unsigned, s, 88 + k * 4);
        if (!e)
            continue;
        BYTE* d = F(BYTE*, s, 44);
        unsigned n = F(unsigned, d, 248 + k * 4);
        if (n > 9)
            continue;
        F(int, d, (n + k * 11) * 4 + 72) = F(int, s, 76 + k * 4);
        F(unsigned, d, (F(unsigned, d, 248 + k * 4) + k * 11) * 4 + 160) = e < 1200 ? e : 1200;
        F(unsigned, d, 248 + k * 4)++;
    }
    return CallThis<int>(FUN_5f3570, s, a, b);
}

// ---------------------------------------------------------------------------
// Effect drawing: batched triangle lists, sprite lists, decals; logged
// vehicle work.

extern "C" void FUN_4c0d80(); KING_ALIAS("_FUN_4c0d80", "@EffectMat_Init@16")
extern "C" void FUN_5b1020(); KING_ALIAS("_FUN_5b1020", "@Material_Select@24")
extern "C" void FUN_5f36d0(); KING_ALIAS("_FUN_5f36d0", "@Obj_DrawTested@16")
extern "C" void FUN_4c8030(); KING_ALIAS("_FUN_4c8030", "@RefStack_Empty@8")
extern "C" void FUN_4c8080(); KING_ALIAS("_FUN_4c8080", "@RefStack_Pop@8")
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")
extern "C" void FUN_4cf620(); KING_ALIAS("_FUN_4cf620", "_Ai_CarFrame")
extern "C" void FUN_4cfe70(); KING_ALIAS("_FUN_4cfe70", "_Car_HitsAndFrame")
extern "C" void FUN_556a70(); KING_ALIAS("_FUN_556a70", "@CarBody_Step@4")
extern "C" void FUN_55d5d0(); KING_ALIAS("_FUN_55d5d0", "@Tractor_Frame@4")
KING_VAR(void*, g_eff720908, 720908)
KING_VAR(int, g_eff7208d0, 7208d0)
KING_VAR(int, g_eff68b990, 68b990)
KING_VAR(int, g_eff721768, 721768)
KING_VAR(char*, g_eff71e814, 71e814)
KING_VAR(int, g_eff71e818, 71e818)
KING_VAR(int, g_eff71e810, 71e810)
KING_VAR(AsmLabel, s_eff6729f4, 6729f4)
KING_VAR(AsmLabel, s_eff6729e0, 6729e0)
KING_VAR(AsmLabel, s_eff672a44, 672a44)

// a method of the Direct3D device d (vtable slot at byte offset off)
template <class... A>
static int Eff_Dev(void* d, unsigned off, A... args)
{
    void* fn = *(void**)(*(char**)d + off);
    return ((int(__stdcall*)(void*, A...))fn)(d, args...);
}

// draws the n triangles (+68, 24-byte vertices at +76) in chunks of at most max
static void Eff_DrawChunks(BYTE* s, unsigned max)
{
    unsigned n = F(unsigned, s, 68);
    if (n <= max) {
        Eff_Dev(g_eff720908, 100, 4, 322, F(BYTE*, s, 76), n, 0);
        return;
    }
    unsigned i = 0;
    if (n - max) {
        for (; i < F(unsigned, s, 68) - max; i += max)
            Eff_Dev(g_eff720908, 100, 4, 322, F(BYTE*, s, 76) + i * 24, max, 0);
    }
    if (F(unsigned, s, 68) > i)
        Eff_Dev(g_eff720908, 100, 4, 322, F(BYTE*, s, 76) + i * 24, F(unsigned, s, 68) - i, 0);
}

// the triangles of the material +72 with render state 136 on, then the base drawing
static int Eff_DrawTriangles(BYTE* s, int a, BYTE* b, unsigned max)
{
    if (!F(int, s, 68))
        return CallThis<int>(FUN_4c0d80, s, a, b);
    CallThis(FUN_5b1020, b, F(BYTE*, s, 72), 0, 0, 0);
    Eff_Dev(g_eff720908, 80, 136, 1);
    Eff_DrawChunks(s, max);
    return CallThis<int>(FUN_4c0d80, s, a, b);
}
// [$L_4c0ba0]
extern "C" int __fastcall Eff_Draw4c0ba0(BYTE* s, void*, int a, BYTE* b) { return Eff_DrawTriangles(s, a, b, 1023); }
// [$L_4c2900]
extern "C" int __fastcall Eff_Draw4c2900(BYTE* s, void*, int a, BYTE* b) { return Eff_DrawTriangles(s, a, b, 1023); }
// [$L_4c0c90]
extern "C" int __fastcall Eff_Draw4c0c90(BYTE* s, void*, int a, BYTE* b) { return Eff_DrawTriangles(s, a, b, 900); }

// [$L_4c2770] the same with the material set to full opacity and flags 0x40000 and 32 (bit 1 off)
extern "C" int __fastcall Eff_Draw4c2770(BYTE* s, void*, int a, BYTE* b)
{
    if (!F(int, s, 68))
        return CallThis<int>(FUN_4c0d80, s, a, b);
    BYTE* m = F(BYTE*, s, 72);
    F(DWORD, m, 88) = 0x3f800000;
    F(unsigned, F(BYTE*, s, 72), 32) |= 0x40000;
    F(unsigned, F(BYTE*, s, 72), 32) &= ~2u;
    F(unsigned, F(BYTE*, s, 72), 32) |= 32;
    CallThis(FUN_5b1020, b, F(BYTE*, s, 72), 0, 0, 0);
    Eff_Dev(g_eff720908, 80, 136, 1);
    Eff_DrawChunks(s, 1023);
    return CallThis<int>(FUN_4c0d80, s, a, b);
}

// [$L_4bcaa0] after FUN_5f36d0 the two sprite groups (texture +64/+68, counts +248/+252, lists +160/+204)
// are drawn with render state 22 on, then emptied
extern "C" int __fastcall Eff_DrawSprites(BYTE* s, void*, int a, BYTE* b)
{
    CallThis(FUN_5f36d0, s, a, b);
    Eff_Dev(g_eff720908, 80, 136, 1);
    DWORD old;
    Eff_Dev(g_eff720908, 84, 22, &old);
    for (int k = 0; k < 2; k++) {
        unsigned* cnt = &F(unsigned, s, 248 + k * 4);
        if (!*cnt)
            continue;
        CallThis(FUN_5b1020, b, F(BYTE*, s, 64 + k * 4), 1, 0, 0);
        Eff_Dev(g_eff720908, 80, 22, 1);
        for (unsigned i = 0; i < *cnt; i++) {
            unsigned c = F(unsigned, s, 160 + k * 44 + i * 4);
            if (c)
                Eff_Dev(g_eff720908, 100, 4, 274, F(BYTE*, s, (18 + k * 11 + i) * 4), c, 0);
        }
        *cnt = 0;
    }
    Eff_Dev(g_eff720908, 80, 22, old);
    return 1;
}

// [$L_4cc8f0] after FUN_5f36d0 the decals (+13296 of them: textures +12896, quads +64, 128 bytes each)
extern "C" int __fastcall Eff_DrawDecals(BYTE* s, void*, int a, BYTE* b)
{
    if (!CallThis<int>(FUN_5f36d0, s, a, b) || !F(int, s, 13296)) {
        F(int, s, 13296) = 0;
        return 0;
    }
    DWORD old;
    Eff_Dev(g_eff720908, 84, 22, &old);
    for (unsigned i = 0; i < F(unsigned, s, 13296); i++) {
        CallThis(FUN_5b1020, b, F(BYTE*, s, 12896 + i * 4), 1, 0, 0);
        Eff_Dev(g_eff720908, 80, 22, 1);
        Eff_Dev(g_eff720908, 100, 6, 274, s + 64 + i * 128, 4, 0);
    }
    Eff_Dev(g_eff720908, 80, 22, old);
    F(int, s, 13296) = 0;
    return 1;
}

// [$L_4c88f0] the queued effects (+76) drawn on the device +92 with the textures +80/+84 and the colors +88/+36
extern "C" int __fastcall Eff_DrawQueue(BYTE* s, void*, int, BYTE* b)
{
    if (CallThis<int>(FUN_4c8030, F(BYTE*, s, 76)))
        return 0;
    Eff_Dev(F(void*, s, 92), 80, 136, 1);
    DWORD old;
    Eff_Dev(F(void*, s, 92), 84, 22, &old);
    Eff_Dev(F(void*, s, 92), 80, 22, 1);
    CallVirt(b, 8, F(int, s, 88));
    CallThis(FUN_5b1020, b, g_eff7208d0 ? F(int, s, 80) : F(int, s, 84), 0, 0, 0);
    while (!CallThis<int>(FUN_4c8030, F(BYTE*, s, 76))) {
        BYTE* o = CallThis<BYTE*>(FUN_4c8080, F(BYTE*, s, 76));
        CallVirt(o, 8, F(int, s, 92));
        if (o)
            CallVirt(o, 0, 1);
    }
    CallVirt(b, 8, F(int, s, 36));
    Eff_Dev(F(void*, s, 92), 80, 22, old);
    return 1;
}

// appends a text to the log buffer 71e814 (position 71e818, size 71e810) when logging (721768)
static void Eff_Log(const char* t)
{
    if (!g_eff721768 || !g_eff71e814 || !*t)
        return;
    while (g_eff71e818 < g_eff71e810) {
        g_eff71e814[g_eff71e818] = *t++;
        g_eff71e818++;
        if (!*t)
            break;
    }
}

// the vehicle's step between the logged markers, when FUN_4cf620 allows it for the player (+20620)
static void Eff_LoggedStep(BYTE* s, const char* before, void (*step)())
{
    int h = 0;
    if (F(int, s, 20620) != g_eff68b990)
        h = CallC<int>(FUN_43f620, F(int, s, 20620));
    if (!CallC<int>(FUN_4cf620, s - 16, &h))
        return;
    Eff_Log(before);
    CallThis(step, s);
    Eff_Log((const char*)s_eff6729e0);
    CallC(FUN_4cfe70, s - 16, &h);
}
// [$L_4d1040]
extern "C" void __fastcall Eff_4d1040(BYTE* s) { Eff_LoggedStep(s, (const char*)s_eff6729f4, FUN_556a70); }
// [$L_4d2660]
extern "C" void __fastcall Eff_4d2660(BYTE* s) { Eff_LoggedStep(s, (const char*)s_eff672a44, FUN_55d5d0); }

// ---------------------------------------------------------------------------
// Effect colors taken from the room's palette.

extern "C" void FUN_4be5e0(); KING_ALIAS("_FUN_4be5e0", "_Color_ToHsl")
extern "C" void FUN_4be7f0(); KING_ALIAS("_FUN_4be7f0", "_Color_FromHsl")
KING_VAR(float, g_eff64c488, 64c488)
KING_VAR(float, g_eff64c258b, 64c258)
KING_VAR(float, g_eff64c74c, 64c74c)
KING_VAR(float, g_eff64c754, 64c754)
KING_VAR(float, g_eff64c250, 64c250)
KING_VAR(float, g_eff64c264b, 64c264)
KING_VAR(float, g_eff64c458, 64c458)

// the color (+60) from palette entry 48 of the effect's room (+4): its brightness halved and its
// saturation raised by add (within 0..1); then the member obj gets the room (vtbl+16)
static void Eff_PaletteColor(BYTE* s, const float* add, BYTE* obj)
{
    BYTE* c = CallThis<BYTE*>(FUN_5b6700, F(BYTE*, F(BYTE*, s, 4), 12)) + 48;
    float r = (float)((double)c[0] * g_eff64c488);
    float g = (float)((double)c[1] * g_eff64c488);
    float b = (float)((double)c[2] * g_eff64c488);
    float h, sat, v;
    CallC(FUN_4be5e0, *(DWORD*)&r, *(DWORD*)&g, *(DWORD*)&b, &h, &sat, &v);
    v = (float)((double)v * g_eff64c258b);
    double t = (double)sat + *add;
    sat = (float)t;
    if (t > g_eff64c250)
        sat = 1.0f;
    else if (!(sat >= g_eff64c264b))
        sat = 0;
    float out[3];
    CallC(FUN_4be7f0, *(DWORD*)&h, *(DWORD*)&sat, *(DWORD*)&v, &out[0], &out[1], &out[2]);
    int col = (int)((double)out[0] * g_eff64c458);
    col = (col | 0xffffff00) << 8;
    col = (col | (int)((double)out[1] * g_eff64c458)) << 8;
    col |= (int)((double)out[2] * g_eff64c458);
    F(int, s, 60) = col;
    CallVirt(obj, 16, F(int, s, 4));
}
// [$L_4c31d0]
extern "C" void __fastcall Eff_Color4c31d0(BYTE* s) { Eff_PaletteColor(s, &g_eff64c74c, s + 4672); }
// [$L_4c3970]
extern "C" void __fastcall Eff_Color4c3970(BYTE* s) { Eff_PaletteColor(s, &g_eff64c754, F(BYTE*, s, 64)); }
// [$L_4c3b70]
extern "C" void __fastcall Eff_Color4c3b70(BYTE* s) { Eff_PaletteColor(s, &g_eff64c754, s + 64); }

// ---------------------------------------------------------------------------
// More virtual methods.

KING_VAR(BYTE*, g_eff6cec6c, 6cec6c)
KING_VAR(BYTE*, g_eff696ca0, 696ca0)
KING_VAR(double, g_eff64b658, 64b658)
KING_VAR(double, g_eff64ca78, 64ca78)

// [$L_4ce160] a traffic light: the phase of the clock (12 steps) sets the frames of its two lamps (+40, +44)
extern "C" int __fastcall Eff_TrafficLight(BYTE* s)
{
    double t = F(double, F(BYTE*, s, 32), 2968);
    if (F(int, g_eff6cec6c, 1300) == 2)
        t = F(double, g_eff696ca0, 7968) * g_eff64b658;
    int ph = (int)(t * g_eff64ca78) % 12;
    int a, b;
    if (ph < 4) {
        F(int, s, 36) = 0;
        a = 0;
        b = 3;
    } else if (ph == 4) {
        F(int, s, 36) = 0;
        int k = (int)(t + t) % 2;
        a = k;
        b = (k != 0) + 3;
    } else if (ph == 5) {
        F(int, s, 36) = 0;
        a = 2;
        b = 5;
    } else if (ph < 10) {
        F(int, s, 36) = 1;
        a = 3;
        b = 0;
    } else if (ph == 10) {
        F(int, s, 36) = 1;
        int k = (int)(t + t) % 2;
        a = (k != 0) + 3;
        b = k;
    } else {
        F(int, s, 36) = 0;
        a = 5;
        b = 2;
    }
    CallC(FUN_5e1a10, F(int, s, 40), a);
    CallC(FUN_5e1a10, F(int, s, 44), b);
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// [$L_4c2b30] the particles: live ones (+52 list) are moved (vtbl+4) or go to the free list (+48);
// new ones are emitted every +12 seconds (+16 left) while emitting (+40)
extern "C" void __fastcall Eff_Particles(BYTE* s, void*, float dt)
{
    F(int, s, 44) = 0;
    CallVirt(s, 12);
    BYTE* prev = 0;
    for (BYTE* o = F(BYTE*, s, 52); o;) {
        if (CallVirt<int>(s, 4, o, *(DWORD*)&dt)) {
            F(int, s, 44)++;
            prev = o;
            o = *(BYTE**)o;
        } else {
            BYTE* next = *(BYTE**)o;
            *(BYTE**)o = F(BYTE*, s, 48);
            F(BYTE*, s, 48) = o;
            if (prev)
                *(BYTE**)prev = next;
            if (o == F(BYTE*, s, 52))
                F(BYTE*, s, 52) = next;
            o = next;
        }
    }
    if (!F(int, s, 40) || !(dt > g_eff64c264b))
        return;
    float rem = dt;
    for (;;) {
        if (!(rem > F(float, s, 16))) {
            F(float, s, 16) = (float)((double)F(float, s, 16) - rem);
            return;
        }
        rem = (float)((double)rem - F(float, s, 16));
        F(DWORD, s, 16) = F(DWORD, s, 12);
        BYTE* o = F(BYTE*, s, 48);
        if (o) {
            if (!CallVirt<int>(s, 8, o)) {
                F(int, s, 16) = 0;
                return;
            }
            F(BYTE*, s, 48) = *(BYTE**)F(BYTE*, s, 48);
            *(BYTE**)o = F(BYTE*, s, 52);
            F(BYTE*, s, 52) = o;
            F(int, s, 44)++;
        }
        if (!(rem > g_eff64c264b))
            return;
    }
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

extern "C" void FUN_4eac8e(); KING_ALIAS("_FUN_4eac8e", "@Arrays_Free@8")
extern "C" void FUN_5da790(); KING_ALIAS("_FUN_5da790", "@RoomWindow_Destroy@4")

// [FUN_4ddd50] the four sound holders (+4828..+4724), then the window
extern "C" void __fastcall Eff4ddd50_Destroy(BYTE* s)
{
    CallThis(FUN_4eac8e, s + 4828);
    CallThis(FUN_4eac8e, s + 4788);
    CallThis(FUN_4eac8e, s + 4756);
    CallThis(FUN_4eac8e, s + 4724);
    CallThis(FUN_5da790, s);
}

// ---------------------------------------------------------------------------
// Network messages and small constructors (exception frames dropped).

extern "C" void FUN_5cdc10(); KING_ALIAS("_FUN_5cdc10", "@TreeShadow_Construct@8")
extern "C" void FUN_5cdcd0(); KING_ALIAS("_FUN_5cdcd0", "@Shadow_Attach@24")

// [$L_4cb450] a new 224-byte object (FUN_5cdc10) set up by FUN_5cdcd0 (a, b, c, d); 0 when that fails
extern "C" BYTE* __cdecl Eff_New4cb450(int a, int b, int c, int d)
{
    BYTE* m = (BYTE*)operator new(224);
    BYTE* o = m ? CallThis<BYTE*>(FUN_5cdc10, m) : 0;
    return CallThis<int>(FUN_5cdcd0, o, a, b, c, d) ? o : 0;
}
// [$L_4d2910] the two sound holders (+6992, +6960)
extern "C" void __fastcall Eff4d2910_Destroy(BYTE* s)
{
    CallThis(FUN_4eac8e, s + 6992);
    CallThis(FUN_4eac8e, s + 6960);
}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_4bf470(); KING_ALIAS("_FUN_4bf470", "@Particles_Construct@12")
extern "C" void FUN_4bf6b0(); KING_ALIAS("_FUN_4bf6b0", "@Particles_Alloc@12")
extern "C" void FUN_4c0930(); KING_ALIAS("_FUN_4c0930", "@RendererB_Construct@8")
extern "C" void FUN_4c0a90(); KING_ALIAS("_FUN_4c0a90", "@Renderer_CreateNamed@24")
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")
extern "C" void Eff_VecCtor(); KING_ALIAS("_Eff_VecCtor", "??_L@YGXPAXIHP6EX0@Z1@Z")
KING_VAR(BYTE*, g_eff696244, 696244)
KING_VAR(BYTE*, g_eff696240, 696240)
KING_VAR(AsmLabel, s_eff672678, 672678)
KING_VAR(AsmLabel, s_eff672778, 672778)
KING_VAR(AsmLabel, g_eff64c770, 64c770)
extern "C" BYTE* __fastcall Light4c4a50_Construct(BYTE* s);
KING_VAR(AsmLabel, s_eff67275c, 67275c)
KING_VAR(AsmLabel, g_eff64c464, 64c464)
KING_VAR(int, g_eff64c1f0, 64c1f0)
KING_VAR(AsmLabel, s_eff672750, 672750)
KING_VAR(AsmLabel, s_eff67273c, 67273c)
KING_VAR(AsmLabel, s_eff67266c, 67266c)

// the world's smoke manager (696240): 100 emitters of 140 bytes for the texture 672678
static void Eff_CreateSmoke(BYTE* w)
{
    BYTE* tex = CallC<BYTE*>(FUN_5ef2b0, F(int, w, 524), (const char*)s_eff672678);
    if (!tex) {
        g_eff696240 = 0;
        return;
    }
    BYTE* m = (BYTE*)operator new(44);
    if (!m) {
        g_eff696240 = 0;
        return;
    }
    BYTE* world = g_eff696244;
    CallThis(FUN_5d31e0, m, 1, (const char*)s_eff672778);
    F(void*, m, 0) = g_eff64c770;
    F(BYTE*, m, 36) = tex;
    BYTE* a = (BYTE*)operator new(14004);
    BYTE* e = 0;
    if (a) {
        *(int*)a = 100;
        e = a + 4;
        CallStd(Eff_VecCtor, e, 140, 100, (BYTE*)Light4c4a50_Construct, FUN_5f0870);
    }
    F(BYTE*, m, 32) = e;
    F(int, m, 40) = 0;
    g_eff69624c = 0;
    CallThis(FUN_5d3570, world + 2992, m);
    CallThis(FUN_5d33d0, m, 1);
    g_eff696240 = m;
}
// [FUN_4c4910] (world)
extern "C" void __cdecl Eff_CreateSmokeFor(BYTE* w)
{
    g_eff696244 = w;
    Eff_CreateSmoke(w);
}
// [FUN_4c4ca0] for the current world (696244)
extern "C" void __cdecl Eff_CreateSmokeAgain()
{
    if (!g_eff696244) {
        g_eff696240 = 0;
        return;
    }
    Eff_CreateSmoke(g_eff696244);
}

// [FUN_4bee30] constructor (world, material): the sky effect (vtable 64c464) with its two parts
extern "C" BYTE* __fastcall Sky_Construct(BYTE* s, void*, BYTE* w, BYTE* mat)
{
    CallThis(FUN_5d31e0, s, 98, (const char*)s_eff67275c);
    F(void*, s, 0) = g_eff64c464;
    F(BYTE*, s, 44) = w;
    CallThis(FUN_5d3570, w + 2992, s);
    CallThis(FUN_5d33d0, s, 1);
    F(DWORD, mat, 88) = 0x3f800000;
    F(int, mat, 32) |= 0x40002;
    BYTE* m = (BYTE*)operator new(140);
    F(BYTE*, s, 36) = m ? CallThis<BYTE*>(FUN_4bf470, m, w) : 0;
    CallThis(FUN_4bf6b0, F(BYTE*, s, 36), 0);
    F(int, F(BYTE*, s, 36), 12) = g_eff64c1f0;
    BYTE* m2 = (BYTE*)operator new(84);
    F(BYTE*, s, 32) = m2 ? CallThis<BYTE*>(FUN_4c0930, m2) : 0;
    BYTE* r = CallC<BYTE*>(FUN_5ef2b0, F(int, w, 524), (const char*)s_eff672750);
    if (!r) {
        CallC(FUN_5d2ba0, (const char*)s_eff67273c, 0);
        r = CallC<BYTE*>(FUN_5ef2b0, F(int, w, 524), (const char*)s_eff67266c);
        CallThis(FUN_4c0a90, F(BYTE*, s, 32), r, mat, 2000, 0);
    } else {
        CallThis(FUN_4c0a90, F(BYTE*, s, 32), r, mat, 2000, 1);
    }
    F(int, s, 48) = 1;
    return s;
}

// ---------------------------------------------------------------------------
// Constructors and surfaces (exception frames dropped).

extern "C" void FUN_5d9f60(); KING_ALIAS("_FUN_5d9f60", "@RoomWindow_Construct@24")
extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
extern "C" void FUN_4eab58(); KING_ALIAS("_FUN_4eab58", "@History_Construct@24")
#include <string.h>
KING_VAR(AsmLabel, g_q20_7025f0, 7025f0)
extern "C" double __cdecl Window_Ramp(float x);
KING_VAR(AsmLabel, g_q20_64d5ec, 64d5ec)
KING_VAR(BYTE*, g_q20_696ca0, 696ca0)
KING_VAR(BYTE*, g_q20_696cac, 696cac)
KING_VAR(int, g_q20_69684c, 69684c)

// [FUN_4ddac0] constructor (a 16-byte rectangle by value): the 3D view (vtable 64d5ec) with 80 matrices (+560),
// four fade values (+4724..) and three more matrices
extern "C" BYTE* __fastcall View3D_Construct(BYTE* s, void*, int a, int, int, int)
{
    CallThis(FUN_5d9f60, s, g_q20_69684c, &a, g_q20_696cac, F(int, g_q20_696ca0, 472));
    for (int k = 0; k < 80; k++) {
        CallStd(FUN_4011e0, s + 560 + k * 48, 12, 4, FUN_4bd4d0);
        memcpy(s + 560 + k * 48, g_q20_7025f0, 48);
    }
    void* L = (void*)Window_Ramp;
    CallThis(FUN_4eab58, s + 4724, L, 0x3e99999a, 0, 0);
    CallThis(FUN_4eab58, s + 4756, L, 0x3f800000, L, 0);
    CallThis(FUN_4eab58, s + 4788, L, 0x3f000000, L, 0);
    CallThis(FUN_4eab58, s + 4828, L, 0x3f19999a, 0, 0);
    memcpy(s + 4924, g_q20_7025f0, 48);
    memcpy(s + 4972, g_q20_7025f0, 48);
    memcpy(s + 5020, g_q20_7025f0, 48);
    F(int, s, 5092) = 0;
    F(int, s, 5096) = 0;
    F(int, s, 5100) = 0;
    F(void*, s, 0) = g_q20_64d5ec;
    F(BYTE*, s, 4820) = g_q20_696ca0;
    F(DWORD, g_q20_696cac, 88) = 0x40000000;
    F(DWORD, g_q20_696cac, 84) = 0x3fc00000;
    for (int k = 0; k < 80; k++) {
        F(int, s, 4400 + k * 4) = 0;
        memcpy(s + 560 + k * 48, g_q20_7025f0, 48);
    }
    F(int, s, 4720) = 0;
    F(int, s, 4864) = 0;
    F(int, s, 4860) = 0;
    F(int, s, 4868) = 0;
    F(int, s, 548) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Effect sounds and sparks (exception frames dropped).

extern "C" void FUN_4dd900(); KING_ALIAS("_FUN_4dd900", "@Fx_HandleInit@12")
extern "C" void FUN_5fa4f0(); KING_ALIAS("_FUN_5fa4f0", "@Node_Set48@16")
extern "C" void FUN_4c5680(); KING_ALIAS("_FUN_4c5680", "@Thunk4c5680@4")
KING_VAR(BYTE*, g_q22_696248, 696248)
KING_VAR(BYTE*, g_q22_696250, 696250)
extern "C" BYTE* __fastcall Mesh4c5020_Construct(BYTE* s);
extern "C" AsmLabel g_q22Ctor4c5000; KING_ALIAS("_g_q22Ctor4c5000", "@FxRec_Init@4")
extern "C" AsmLabel g_q22Dtor4c47f0; KING_ALIAS("_g_q22Dtor4c47f0", "@FxRec_Free@4")
KING_VAR(BYTE*, g_q22_696704, 696704)
KING_VAR(BYTE*, g_q22_696708, 696708)
KING_VAR(BYTE*, g_q22_69670c, 69670c)
KING_VAR(BYTE*, g_q22_696710, 696710)
KING_VAR(BYTE*, g_q22_696714, 696714)
KING_VAR(BYTE*, g_q22_696718, 696718)
KING_VAR(BYTE*, g_q22_69671c, 69671c)
KING_VAR(BYTE*, g_q22_696720, 696720)
KING_VAR(BYTE*, g_q22_696724, 696724)
KING_VAR(BYTE*, g_q22_696728, 696728)
KING_VAR(AsmLabel, s_q22_672498, 672498)
KING_VAR(AsmLabel, s_q22_672480, 672480)
KING_VAR(AsmLabel, s_q22_672468, 672468)
KING_VAR(AsmLabel, s_q22_672450, 672450)
KING_VAR(AsmLabel, s_q22_672440, 672440)
KING_VAR(AsmLabel, s_q22_672430, 672430)
KING_VAR(AsmLabel, s_q22_672420, 672420)
KING_VAR(AsmLabel, s_q22_672410, 672410)
KING_VAR(AsmLabel, s_q22_672400, 672400)
KING_VAR(AsmLabel, s_q22_6723f0, 6723f0)

static BYTE* Eff_NewSound(const char* name)
{
    BYTE* m = (BYTE*)operator new(4);
    return m ? CallThis<BYTE*>(FUN_4dd900, m, name) : 0;
}
// [FUN_4b87a0] the ten effect sounds (696704..696728)
extern "C" void __cdecl Eff_CreateSounds()
{
    g_q22_696704 = Eff_NewSound((const char*)s_q22_672498);
    g_q22_696708 = Eff_NewSound((const char*)s_q22_672480);
    g_q22_69670c = Eff_NewSound((const char*)s_q22_672468);
    g_q22_696710 = Eff_NewSound((const char*)s_q22_672450);
    g_q22_696714 = Eff_NewSound((const char*)s_q22_672440);
    g_q22_696718 = Eff_NewSound((const char*)s_q22_672430);
    g_q22_69671c = Eff_NewSound((const char*)s_q22_672420);
    g_q22_696720 = Eff_NewSound((const char*)s_q22_672410);
    g_q22_696724 = Eff_NewSound((const char*)s_q22_672400);
    g_q22_696728 = Eff_NewSound((const char*)s_q22_6723f0);
}

static DWORD* Eff_NewQuad(DWORD a, DWORD b, DWORD c, DWORD d)
{
    DWORD* q = (DWORD*)operator new(16);
    q[0] = a;
    q[1] = b;
    q[2] = c;
    q[3] = d;
    return q;
}
// [FUN_4c4df0] (world): the 100 sparks (696248, 116 bytes each), each a one-quad mesh of 11 units on the smoke texture
extern "C" void __cdecl Eff_CreateSparks(BYTE* w)
{
    g_q22_696250 = w;
    g_eff696244 = w;
    if (!g_q22_696248) {
        BYTE* m = (BYTE*)operator new(11604);
        BYTE* e = 0;
        if (m) {
            *(int*)m = 100;
            e = m + 4;
            CallStd(Eff_VecCtor, e, 116, 100, (BYTE*)Mesh4c5020_Construct, FUN_4c5680);
        }
        g_q22_696248 = e;
    }
    BYTE* tex = CallC<BYTE*>(FUN_5ef2b0, F(int, w, 524), (const char*)s_eff672678);
    for (int off = 0; off < 11600; off += 116) {
        F(int, g_q22_696248 + off, 68) = 0;
        BYTE* it = g_q22_696248 + off;
        BYTE* m = (BYTE*)operator new(44);
        BYTE* e = 0;
        if (m) {
            *(int*)m = 1;
            e = m + 4;
            CallStd(Eff_VecCtor, e, 40, 1, (BYTE*)g_q22Ctor4c5000, (BYTE*)g_q22Dtor4c47f0);
        }
        F(WORD, e, 0) = 2;
        F(WORD, e, 4) = 4;
        F(DWORD*, e, 8) = Eff_NewQuad(0, 0x3f800000, 0x3f800000, 0);
        F(DWORD*, e, 12) = Eff_NewQuad(0x3f800000, 0x3f800000, 0, 0);
        F(DWORD*, e, 16) = Eff_NewQuad(0xc0b00000, 0x40b00000, 0x40b00000, 0xc0b00000);
        F(int, e, 24) = 0;
        F(DWORD*, e, 20) = Eff_NewQuad(0xc0b00000, 0xc0b00000, 0x40b00000, 0x40b00000);
        F(BYTE, e, 1) |= 1;
        F(WORD, e, 34) = 4095;
        F(int, e, 36) = 0;
        CallThis(FUN_5fa4f0, it, e, 1);
        CallVirt(it, 72);
        CallVirt(tex, 20, 0, it);
        F(BYTE, it, 40) |= 1;
        F(int, F(BYTE*, g_q22_696248 + off, 52), 36) = 0;
        F(BYTE, g_q22_696248 + off, 40) |= 1;
    }
}

// ---------------------------------------------------------------------------
// Trail effect constructor, image from file (exception frames dropped).

extern "C" void FUN_4bcc80(); KING_ALIAS("_FUN_4bcc80", "_Fx_FindByNameStd@8")
extern "C" void FUN_5d9290(); KING_ALIAS("_FUN_5d9290", "@Scenes_Find@12")
extern "C" void FUN_50d190(); KING_ALIAS("_FUN_50d190", "@Shell_Nop@8")
extern "C" int __cdecl sprintf(char* buf, const char* format, ...);
KING_VAR(AsmLabel, s_q26_67254c, 67254c)
KING_VAR(AsmLabel, s_q26_672688, 672688)
extern "C" BYTE* __fastcall Rec4bc820_Construct(BYTE* s);

static BYTE* Eff_NewItems240(int n)
{
    BYTE* m = (BYTE*)operator new(n * 240 + 4);
    if (!m)
        return 0;
    *(int*)m = n;
    CallStd(Eff_VecCtor, m + 4, 240, n, (BYTE*)Rec4bc820_Construct, FUN_50d190);
    return m + 4;
}
// [FUN_4bc690] constructor (name, world, n): a trail effect with two materials (+24, +28) named by name and
// 0/1, n slots (+0, +4) and two arrays of n items of 240 bytes (+8, +12)
extern "C" BYTE* __fastcall Trail_Construct(BYTE* s, void*, const char* name, int w, int n)
{
    F(int, s, 16) = w;
    for (int i = 0; i < 2; i++) {
        char buf[260];
        sprintf(buf, (const char*)s_q26_67254c, name, i);
        BYTE* room = CallThis<BYTE*>(FUN_5d9290, F(BYTE*, s, 16), (const char*)s_q26_672688);
        BYTE* r = CallThis<BYTE*>(FUN_4bcc80, s, buf, room);
        F(BYTE*, s, 24 + i * 4) = r;
        F(int, r, 32) |= 0x40000;
    }
    F(int, s, 20) = n;
    F(int*, s, 0) = (int*)operator new(n * 4);
    F(int*, s, 4) = (int*)operator new(F(int, s, 20) * 4);
    F(BYTE*, s, 8) = Eff_NewItems240(F(int, s, 20));
    F(BYTE*, s, 12) = Eff_NewItems240(F(int, s, 20));
    for (int i = 0; i < F(int, s, 20); i++) {
        F(int*, s, 0)[i] = 0;
        F(int*, s, 4)[i] = 0;
    }
    F(int, s, 32) = 0;
    F(int, s, 84) = 0;
    for (int k = 0; k < 10; k++)
        F(int, s, 40 + k * 4) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_4d0bc0(); KING_ALIAS("_FUN_4d0bc0", "@ParkedCar_Destroy@8")
// [$L_4d0ba0]
extern "C" BYTE* __fastcall Dtor4d0ba0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_4d0bc0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_4d0fd0(); KING_ALIAS("_FUN_4d0fd0", "@Vehicle4d0fd0_Destroy@8")
// [$L_4d0ed0]
extern "C" BYTE* __fastcall Dtor4d0ed0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_4d0fd0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_4d12f0(); KING_ALIAS("_FUN_4d12f0", "@Vehicle4d12f0_Destroy@8")
// [$L_4d12d0]
extern "C" BYTE* __fastcall Dtor4d12d0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_4d12f0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_4d25f0(); KING_ALIAS("_FUN_4d25f0", "@Effect4d25f0_Destroy@8")
// [$L_4d25d0]
extern "C" BYTE* __fastcall Dtor4d25d0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_4d25f0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

extern "C" void FUN_4c4ca0(); KING_ALIAS("_FUN_4c4ca0", "_Eff_CreateSmokeAgain")
extern "C" void FUN_4c4de0(); KING_ALIAS("_FUN_4c4de0", "_Fx_Clear696244")
// [$L_4c56f0] marks the 100 glows (116 bytes each) as free (+40 bit 0)
extern "C" void __cdecl Glows_MarkAllFree(void)
{
    if (!g_glows)
        return;
    for (int k = 0; k < 100; k++)
        F(BYTE, g_glows + k * 116, 40) |= 1;
}
// [$L_4c5720] runs slot 36 of each glow, deletes the array (slot 0 with 3) and clears it
extern "C" void __cdecl Glows_Destroy(void)
{
    if (!g_glows)
        return;
    for (int k = 0; k < 100; k++)
        CallVirt(g_glows + k * 116, 36);
    if (g_glows)
        CallVirt(g_glows, 0, 3);
    g_glows = 0;
}
// [FUN_4c4770] creates the smoke again and frees the glows
extern "C" void __cdecl Eff_ResetSmokeAndGlows(void)
{
    CallC(FUN_4c4ca0);
    Glows_MarkAllFree();
}
// [FUN_4c4780] clears the 696244 effects and destroys the glows
extern "C" void __cdecl Eff_ClearAndDestroyGlows(void)
{
    CallC(FUN_4c4de0);
    Glows_Destroy();
}

// ---------------------------------------------------------------------------
// Small methods and static helpers: guarded static construction, releases, frees.

// [$L_4ce6c0] frees the +12 block
extern "C" void __fastcall Eff4ce6c0_FreeBlock(BYTE* s, void*)
{
    if (F(void*, s, 12))
        operator delete(F(void*, s, 12));
}

// ---------------------------------------------------------------------------
// Small methods: message filters, record copies, frees and deleting destructors.

KING_VAR(AsmLabel, g_q50_64c72c, 64c72c)
// [$L_4c2a90] destructor: the 64c72c vtable, frees the +56 block
extern "C" void __fastcall Eff4c2a90_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = g_q50_64c72c;
    if (F(void*, s, 56)) {
        operator delete(F(void*, s, 56));
        F(void*, s, 56) = 0;
    }
}

// ---------------------------------------------------------------------------
// Small methods: table constructors and clears, list teardown, cursor handling, flag maps.

// [FUN_4cfe20] the flag bit for the kind 1..6 (1, 16, 2, 4, 8, 32), else 0
extern "C" int __cdecl Eff_KindFlag(int kind)
{
    static const int bits[6] = {1, 16, 2, 4, 8, 32};
    return kind >= 1 && kind <= 6 ? bits[kind - 1] : 0;
}

// ---------------------------------------------------------------------------
// Static matrices.

extern "C" void __stdcall Array_ForEach(BYTE* p, int size, int count, void (*fn)());    // ai_world.cpp
KING_VAR(AsmLabel, g_q93_7025f0, 7025f0)
KING_VAR(AsmLabel, g_q93_6969d0, 6969d0)
KING_VAR(AsmLabel, g_q93_696b98, 696b98)
KING_VAR(AsmLabel, g_q93_6969a0, 6969a0)
KING_VAR(AsmLabel, g_q93_694b0c, 694b0c)

// [$L_4dda30] static init: the three matrices 6969d0, 696b98 and 6969a0 start as the identity
extern "C" void __cdecl Eff_InitMatrices(void)
{
    memcpy((BYTE*)g_q93_6969d0, (BYTE*)g_q93_7025f0, 48);
    memcpy((BYTE*)g_q93_696b98, (BYTE*)g_q93_7025f0, 48);
    memcpy((BYTE*)g_q93_6969a0, (BYTE*)g_q93_7025f0, 48);
}

// [$L_4c8110] static init: the 40 52-byte records of 694b0c get zeroed vectors and the identity matrix
extern "C" void __cdecl Eff_InitRecords694b0c(void)
{
    BYTE* p = (BYTE*)g_q93_694b0c;
    for (int k = 40; k; k--, p += 52) {
        Array_ForEach(p, 12, 4, (void (*)())Vec3_ZeroThis);
        memcpy(p, (BYTE*)g_q93_7025f0, 48);
    }
}

// ---------------------------------------------------------------------------
// A light node.

extern "C" void FUN_5ef6d0(); KING_ALIAS("_FUN_5ef6d0", "@Node_Construct@16")
KING_VAR(AsmLabel, s_q95_672788, 672788)
KING_VAR(AsmLabel, g_q95_64c7dc, 64c7dc)
KING_VAR(AsmLabel, g_q95_64c790, 64c790)
KING_VAR(AsmLabel, g_q95_7025f0, 7025f0)
// [$L_4c4a50] constructor: a node of type 17, its fields cleared (+84 = -1) and the identity matrix at +88
extern "C" BYTE* __fastcall Light4c4a50_Construct(BYTE* s)
{
    CallThis(FUN_5ef6d0, s, (const char*)s_q95_672788, 17);
    for (int o = 48; o <= 80; o += 4)
        F(int, s, o) = 0;
    F(void*, s, 0) = (BYTE*)g_q95_64c7dc;
    F(int, s, 84) = -1;
    memset(s + 88, 0, 48);
    memcpy(s + 88, (BYTE*)g_q95_7025f0, 48);
    F(void*, s, 0) = (BYTE*)g_q95_64c790;
    return s;
}

// ---------------------------------------------------------------------------
// Collecting lights.

KING_VAR(BYTE*, g_q96_696240, 696240)
// [$L_4c4830] visitor: adds light l of scene object o to the 696240 list (at most 100 records of 140 bytes:
// the light's +44 at +64 and +136, o's current matrix at +88, 16 bytes of the light's +20 at +20) and tells the
// list's owner (+36, vtbl+20); always 1
extern "C" int __cdecl Lights_Collect(void*, BYTE* o, BYTE* l)
{
    BYTE* g = g_q96_696240;
    if (!g)
        return 1;
    BYTE* p = F(BYTE*, l, 44);
    if (F(int, g, 40) == 100)
        return 1;
    BYTE* rec = F(BYTE*, g, 32) + F(int, g, 40) * 140;
    F(BYTE*, rec, 64) = p;
    F(BYTE*, rec, 136) = p;
    memcpy(rec + 88, o + 5216 + F(int, o, 8236) * 48, 48);
    memcpy(rec + 20, p + 20, 16);
    CallVirt(F(BYTE*, g, 36), 20, 0, F(BYTE*, g, 32) + F(int, g, 40) * 140);
    F(int, g, 40)++;
    return 1;
}

// ---------------------------------------------------------------------------
// A 240-byte record with two placements.

KING_VAR(AsmLabel, g_q97_7025f0, 7025f0)
// [$L_4bc820] constructor: +0..+47 cleared; two placements (+56 and +148: 36 cleared bytes and an identity
// matrix after them)
extern "C" BYTE* __fastcall Rec4bc820_Construct(BYTE* s)
{
    memset(s, 0, 48);
    for (int b = 56; b <= 148; b += 92) {
        memset(s + b, 0, 36);
        memcpy(s + b + 36, (BYTE*)g_q97_7025f0, 48);
    }
    return s;
}

extern "C" void FUN_5fa6f0(); KING_ALIAS("_FUN_5fa6f0", "@Mesh_ConstructNamed@12")
KING_VAR(AsmLabel, s_q156_672794, 672794)
KING_VAR(AsmLabel, g_q156_64c828, 64c828)
// [$L_4c5020] constructor: a mesh named 672794 with +104..+112 cleared
extern "C" BYTE* __fastcall Mesh4c5020_Construct(BYTE* s)
{
    CallThis(FUN_5fa6f0, s, (const char*)s_q156_672794);
    F(int, s, 104) = 0;
    F(int, s, 108) = 0;
    F(int, s, 112) = 0;
    F(void*, s, 0) = (BYTE*)g_q156_64c828;
    return s;
}

// [$L_4c0dd0] visitor: a record of kind 16 (a) adds the point of b (+4, +8 and a's +12 height) to b's ring of
// 50 (b +0: count +4, points from +8)
extern "C" int __cdecl Ring50_AddPoint(const BYTE* a, const BYTE* b)
{
    BYTE* r = F(BYTE*, b, 0);
    if (F(int, a, 0) != 16)
        return (int)b;
    float p[3] = {F(float, b, 4), F(float, b, 8), *F(const float*, a, 12)};
    memcpy(r + 8 + (F(unsigned, r, 4) % 50) * 12, p, 12);
    return ++F(int, r, 4);
}

// [$L_4cce30] sort order: by +108, largest first
extern "C" int __cdecl Cmp4cce30_Desc(const BYTE* a, const BYTE* b)
{
    if (F(float, b, 108) > F(float, a, 108))
        return 1;
    if (F(float, b, 108) < F(float, a, 108))
        return -1;
    return 0;
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q198_Dtor4d25d0_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor4d25d0_DeletingDestructor@12", "@Dtor4d25d0_DeletingDestructor@12")
// [$L_4d9370] forwards to Dtor4d25d0_DeletingDestructor for the object 16 bytes before
extern "C" int __fastcall Thunk4d9370(BYTE* s, void* d, int a0) { return q198_Dtor4d25d0_DeletingDestructor(s - 16, d, a0); }
// [$L_4d9380] forwards to Dtor4d25d0_DeletingDestructor for the object 272 bytes before
extern "C" BYTE* __fastcall Thunk4d9380(BYTE* s, void* d, int a0) { return Dtor4d25d0_DeletingDestructor(s - 272, d, a0); }
// [$L_4d9390] forwards to Dtor4d25d0_DeletingDestructor for the object 392 bytes before
extern "C" BYTE* __fastcall Thunk4d9390(BYTE* s, void* d, int a0) { return Dtor4d25d0_DeletingDestructor(s - 392, d, a0); }
// [$L_4d93a0] forwards to Dtor4d0ba0_DeletingDestructor for the object 16 bytes before
extern "C" BYTE* __fastcall Thunk4d93a0(BYTE* s, void* d, int a0) { return Dtor4d0ba0_DeletingDestructor(s - 16, d, a0); }
// [$L_4d93b0] forwards to Dtor4d0ba0_DeletingDestructor for the object 272 bytes before
extern "C" BYTE* __fastcall Thunk4d93b0(BYTE* s, void* d, int a0) { return Dtor4d0ba0_DeletingDestructor(s - 272, d, a0); }
// [$L_4d93c0] forwards to Dtor4d0ba0_DeletingDestructor for the object 392 bytes before
extern "C" BYTE* __fastcall Thunk4d93c0(BYTE* s, void* d, int a0) { return Dtor4d0ba0_DeletingDestructor(s - 392, d, a0); }
// [$L_4d93d0] forwards to Dtor4d12d0_DeletingDestructor for the object 16 bytes before
extern "C" BYTE* __fastcall Thunk4d93d0(BYTE* s, void* d, int a0) { return Dtor4d12d0_DeletingDestructor(s - 16, d, a0); }
// [$L_4d93e0] forwards to Dtor4d12d0_DeletingDestructor for the object 272 bytes before
extern "C" BYTE* __fastcall Thunk4d93e0(BYTE* s, void* d, int a0) { return Dtor4d12d0_DeletingDestructor(s - 272, d, a0); }
// [$L_4d93f0] forwards to Dtor4d12d0_DeletingDestructor for the object 392 bytes before
extern "C" BYTE* __fastcall Thunk4d93f0(BYTE* s, void* d, int a0) { return Dtor4d12d0_DeletingDestructor(s - 392, d, a0); }
// [$L_4d9400] forwards to Dtor4d0ed0_DeletingDestructor for the object 16 bytes before
extern "C" BYTE* __fastcall Thunk4d9400(BYTE* s, void* d, int a0) { return Dtor4d0ed0_DeletingDestructor(s - 16, d, a0); }
// [$L_4d9410] forwards to Dtor4d0ed0_DeletingDestructor for the object 272 bytes before
extern "C" BYTE* __fastcall Thunk4d9410(BYTE* s, void* d, int a0) { return Dtor4d0ed0_DeletingDestructor(s - 272, d, a0); }
// [$L_4d9420] forwards to Dtor4d0ed0_DeletingDestructor for the object 392 bytes before
extern "C" BYTE* __fastcall Thunk4d9420(BYTE* s, void* d, int a0) { return Dtor4d0ed0_DeletingDestructor(s - 392, d, a0); }
// [$L_4d9430] static initializer: 37 records of 24 bytes at 69638c start with -1
KING_VAR(BYTE, fx_69638c, 69638c)
extern "C" void __cdecl Fx_Init4d9430()
{
    for (int i = 0; i < 37; i++)
        *(int*)(&fx_69638c + i * 24) = -1;
}
// [$L_4d8320] an empty handler
extern "C" void __cdecl Fx_Nop4d8320() {}

// [$L_4c47f0] frees the five buffers +8..+24 of a 40-byte effect record
extern "C" void __fastcall FxRec_Free(BYTE* s)
{
    operator delete(F(void*, s, 8));
    operator delete(F(void*, s, 12));
    operator delete(F(void*, s, 16));
    operator delete(F(void*, s, 20));
    operator delete(F(void*, s, 24));
}
// [$L_4c5000] an empty 40-byte effect record
extern "C" BYTE* __fastcall FxRec_Init(BYTE* s)
{
    F(int, s, 24) = 0;
    F(int, s, 20) = 0;
    F(int, s, 16) = 0;
    F(int, s, 12) = 0;
    F(int, s, 8) = 0;
    F(int, s, 36) = 0;
    return s;
}

extern "C" void FUN_4ba130(); KING_ALIAS("_FUN_4ba130", "_Fx_Skid1")
extern "C" void FUN_4ba210(); KING_ALIAS("_FUN_4ba210", "_Fx_Skid2")
extern "C" void FUN_4bb4c0(); KING_ALIAS("_FUN_4bb4c0", "_Fx_DebrisBurst")
extern "C" void FUN_4ba660(); KING_ALIAS("_FUN_4ba660", "_Fx_SparksByStrength")
extern "C" void FUN_4bac90(); KING_ALIAS("_FUN_4bac90", "_Effect_New59c420")
extern "C" void FUN_4baf90(); KING_ALIAS("_FUN_4baf90", "_Effect_NewVapor")
extern "C" void FUN_4bc2d0(); KING_ALIAS("_FUN_4bc2d0", "_Fx_SparkBurst")
extern "C" void FUN_4bc4d0(); KING_ALIAS("_FUN_4bc4d0", "_Effect_Start4d9460")
// [FUN_4bc540] handles an effect message (957): by its kind (byte +0) starts the effect at the position +4 with its
// parameters from +16; 1 when handled
extern "C" int __cdecl Fx_HandleMessage(int msg, BYTE* m)
{
    if (msg != 957)
        return 0;
    BYTE* p = m + 4;
    switch (m[0]) {
    case 1: CallC(FUN_4ba130, p, m + 16, 0); return 1;
    case 2: CallC(FUN_4ba210, p, m + 16, 0); return 1;
    case 3:
        CallC(FUN_4bb4c0, p, m + 16, m + 28, F(int, m, 40), F(int, m, 44), F(int, m, 48), 0);
        return 1;
    case 4: CallC(FUN_4ba660, p, m + 16, F(int, m, 28), 0); return 1;
    case 5: CallC(FUN_4bac90, p, m + 16, F(int, m, 28), 0); return 1;
    case 6: CallC(FUN_4baf90, p, F(int, m, 16), 0); return 1;
    case 7:
    case 8: return 1;
    case 9: CallC(FUN_4bc2d0, p, m + 16, 0); return 1;
    case 10: CallC(FUN_4bc4d0, p, 0); return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q240_Sce5fa720_Destroy(BYTE* s); KING_ALIAS("@q240_Sce5fa720_Destroy@4", "@Sce5fa720_Destroy@4")
// [FUN_4c5680] forwards to Sce5fa720_Destroy
extern "C" int __fastcall Thunk4c5680(BYTE* s) { return q240_Sce5fa720_Destroy(s); }

// === an effect with 14 random values (FUN_4daf40)

KING_VAR(AsmLabel, s_fx672c74, 672c74)
KING_VAR(AsmLabel, g_fxVtbl64d4c8, 64d4c8)
KING_VAR(float, g_fx64c254, 64c254)
KING_VAR(float, g_fx64d4c4, 64d4c4)
KING_VAR(float, g_fx64c350, 64c350)
KING_VAR(float, g_fx64c40c, 64c40c)

// the constant fields (+144 .. +672): float bit patterns
static const DWORD k_fx4daf40[][2] = {
    {144, 3212836864u}, {148, 3239051264u}, {152, 1071225242u}, {156, 1065353216u}, {160, 3238002688u}, {164, 1073741824u}, {168, 1069128090u}, {172, 3235066675u}, {176, 1067869798u}, {180, 3216611738u}, {184, 3227516928u}, {188, 1067869798u}, {192, 1069128090u}, {196, 3225839206u}, {200, 1067869798u}, {204, 3216611738u}, {208, 0u}, {212, 1067869798u}, {216, 0u}, {220, 3222064333u}, {224, 1077936128u}, {228, 1069128090u}, {232, 0u}, {236, 1067869798u}, {240, 3216611738u}, {244, 3212836864u}, {248, 1067869798u}, {252, 0u}, {256, 1072064102u}, {260, 1077936128u}, {264, 3216611738u}, {268, 3225419776u}, {272, 1067869798u}, {276, 1069128090u}, {280, 3231711232u}, {284, 1067869798u}, {288, 3216611738u}, {292, 3233808384u}, {296, 1067869798u}, {300, 3212836864u}, {304, 3238002688u}, {308, 1068708659u}, {368, 3216611738u}, {372, 3235066675u}, {376, 1065353216u}, {380, 3216611738u}, {384, 3234018099u}, {388, 1065353216u}, {392, 3216611738u}, {396, 3232969523u}, {400, 1065353216u}, {404, 3216611738u}, {408, 3231920947u}, {412, 1065353216u}, {416, 3216611738u}, {420, 3230872371u}, {424, 1065353216u}, {428, 0u}, {432, 1072064102u}, {436, 1077936128u}, {440, 1069128090u}, {444, 3235066675u}, {448, 1065353216u}, {452, 1069128090u}, {456, 3234018099u}, {460, 1065353216u}, {464, 1069128090u}, {468, 3232969523u}, {472, 1065353216u}, {476, 1069128090u}, {480, 3231920947u}, {484, 1065353216u}, {488, 1069128090u}, {492, 3230872371u}, {496, 1065353216u}, {500, 0u}, {504, 3222064333u}, {508, 1077936128u}, {512, 3212836864u}, {516, 0u}, {520, 1082130432u}, {524, 3212836864u}, {528, 0u}, {532, 1082130432u}, {536, 3212836864u}, {540, 0u}, {544, 1082130432u}, {548, 3212836864u}, {552, 0u}, {556, 1082130432u}, {560, 3212836864u}, {564, 0u}, {568, 1082130432u}, {572, 0u}, {576, 0u}, {580, 1084227584u}, {584, 1065353216u}, {588, 0u}, {592, 1082130432u}, {596, 1065353216u}, {600, 0u}, {604, 1082130432u}, {608, 1065353216u}, {612, 0u}, {616, 1082130432u}, {620, 1065353216u}, {624, 0u}, {628, 1082130432u}, {632, 1065353216u}, {636, 0u}, {640, 1082130432u}, {644, 0u}, {648, 0u}, {652, 1084227584u}, {656, 1077936128u}, {660, 0u}, {672, 0u}
};

extern "C" {

// [FUN_4daf40] constructor (view, a, b): fixed shape data, 14 random values at +88 (+664 their maximum, +668 that
// plus 64c40c), owned by the view (+2992)
BYTE* __fastcall Fx4daf40_Construct(BYTE* self, void*, BYTE* view, int a, int b)
{
    CallThis(FUN_5d3110, self, (const char*)s_fx672c74);
    memset(self + 144, 0, 512);
    F(void*, self, 0) = g_fxVtbl64d4c8;
    F(BYTE*, self, 676) = view;
    CallThis(FUN_5d3570, view + 2992, self);
    F(int, self, 672) = 0;
    F(int, self, 680) = a;
    F(int, self, 684) = b;
    for (unsigned i = 0; i < sizeof k_fx4daf40 / sizeof k_fx4daf40[0]; i++)
        F(DWORD, self, k_fx4daf40[i][0]) = k_fx4daf40[i][1];
    for (int k = 0; k < 14; k++) {
        F(int, self, 32 + k * 4) = 0;
        F(int, self, 312 + k * 4) = 0;
        float r = (float)(rand() * g_fx64c254);
        int r2 = rand();
        F(float, self, 88 + k * 4) = (float)((r2 * (double)g_fx64c254 - r) * g_fx64d4c4 + (double)k * g_fx64c350);
    }
    F(float, self, 664) = 0;
    double m = 0;
    for (int k = 0; k < 14; k++) {
        float x = F(float, self, 88 + k * 4);
        m = F(float, self, 664) < x ? x : F(float, self, 664);
        F(float, self, 664) = (float)m;
    }
    F(float, self, 668) = (float)(m + g_fx64c40c);
    CallThis(FUN_5d33d0, self, 1);
    return self;
}

} // extern "C"

// === initializing the effects of a world (FUN_4b9570)

KING_VAR(int, g_fi6716f4, 6716f4)
KING_VAR(int, g_fi6949fc, 6949fc)            // a 3dfx card (vendor 0x121a, devices 1 and 2)
KING_VAR(BYTE*, g_fiDraw72090c, 72090c)
KING_VAR(BYTE*, g_fiDevice720908, 720908)
KING_VAR(BYTE*, g_fiWorld696224, 696224)
KING_VAR(BYTE*, g_fi696200, 696200)
KING_VAR(BYTE*, g_fi696204, 696204)
KING_VAR(BYTE*, g_fi6f63d0, 6f63d0)
KING_VAR(BYTE*, g_fi696254, 696254)
KING_VAR(BYTE*, g_fi6f6a68, 6f6a68)
KING_VAR(BYTE*, g_fi69622c, 69622c)
KING_VAR(BYTE*, g_fi696230, 696230)
KING_VAR(BYTE*, g_fi6961f8, 6961f8)
KING_VAR(BYTE*, g_fi696234, 696234)
KING_VAR(int, g_fi6cec68, 6cec68)
KING_VAR(int, g_fi6716fc, 6716fc)
KING_VAR(int, g_fi6716f8, 6716f8)
KING_VAR(float, g_fi64c13c, 64c13c)
KING_VAR(float, g_fi64c140, 64c140)
KING_VAR(float, g_fi64c144, 64c144)
KING_VAR(float, g_fi64c148, 64c148)
KING_VAR(float, g_fi696060, 696060)
KING_VAR(float, g_fi696080, 696080)
KING_VAR(float, g_fi696044, 696044)
KING_VAR(float, g_fi64c258, 64c258)
KING_VAR(int, g_fi696208, 696208)
KING_VAR(int, g_fi69620c, 69620c)
KING_VAR(int, g_fi696210, 696210)
KING_VAR(int, g_fi696214, 696214)
KING_VAR(AsmLabel, g_fi694998, 694998)
KING_VAR(AsmLabel, g_fi694a00, 694a00)
KING_VAR(AsmLabel, g_fi695908, 695908)
KING_VAR(AsmLabel, g_fi695c58, 695c58)
KING_VAR(AsmLabel, g_fi695bf0, 695bf0)
KING_VAR(BYTE*, g_fi696228, 696228)
KING_VAR(int, g_fi671700, 671700)
KING_VAR(DWORD, g_fi694830, 694830)
KING_VAR(DWORD, g_fi64c1c0, 64c1c0)
KING_VAR(DWORD, g_fi64c1c4, 64c1c4)
KING_VAR(AsmLabel, g_fiNames71e820, 71e820)
KING_VAR(AsmLabel, g_fiVtbl64c26c, 64c26c)
extern "C" int __cdecl Ground_Below(BYTE* o, const float* p, float* at, float* n);
extern "C" int __cdecl LampGlows_Draw(void*, BYTE* view, BYTE* node);
ASM_VAR(AsmLabel, g_fiCb4cbec0, _Fx_ScrapeSpark)
ASM_VAR(AsmLabel, g_fiCb4cbfd0, _Fx_ReflectionQuad)
KING_VAR(AsmLabel, s_fi672688, 672688)
KING_VAR(AsmLabel, s_fi66cd44, 66cd44)
KING_VAR(AsmLabel, s_fi672678, 672678)
KING_VAR(AsmLabel, s_fi67266c, 67266c)
KING_VAR(AsmLabel, s_fi6725d0, 6725d0)
KING_VAR(AsmLabel, s_fi672660, 672660)
KING_VAR(AsmLabel, s_fi672658, 672658)
KING_VAR(AsmLabel, s_fi672650, 672650)
KING_VAR(AsmLabel, s_fi672648, 672648)
KING_VAR(AsmLabel, s_fi672640, 672640)
KING_VAR(AsmLabel, s_fi672638, 672638)
KING_VAR(AsmLabel, s_fi67262c, 67262c)
KING_VAR(AsmLabel, s_fi67261c, 67261c)
KING_VAR(AsmLabel, s_fi67260c, 67260c)
KING_VAR(AsmLabel, s_fi6725f8, 6725f8)
KING_VAR(AsmLabel, s_fi6725ec, 6725ec)
extern "C" void FUN_5cb990(); KING_ALIAS("_FUN_5cb990", "_Set70ebf4")
extern "C" void FUN_4b87a0(); KING_ALIAS("_FUN_4b87a0", "_Eff_CreateSounds")
extern "C" void FUN_4b91a0(); KING_ALIAS("_FUN_4b91a0", "_Effects_FromSettings")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_5d9290(); KING_ALIAS("_FUN_5d9290", "@Scenes_Find@12")
extern "C" void FUN_57c290(); KING_ALIAS("_FUN_57c290", "@PoolNode_Construct@16")
extern "C" void FUN_4c8640(); KING_ALIAS("_FUN_4c8640", "@Reflection_Construct@16")
extern "C" void FUN_593040(); KING_ALIAS("_FUN_593040", "@Body593040_Construct@12")
extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")
extern "C" void FUN_5c6400(); KING_ALIAS("_FUN_5c6400", "_Effects_InitTrees")
extern "C" void FUN_5cb9a0(); KING_ALIAS("_FUN_5cb9a0", "_Set70ebe8")
extern "C" void FUN_4bc690(); KING_ALIAS("_FUN_4bc690", "@Trail_Construct@20")
extern "C" void FUN_4bc950(); KING_ALIAS("_FUN_4bc950", "@Effect4bc950_Attach@12")
extern "C" void FUN_4bee30(); KING_ALIAS("_FUN_4bee30", "@Sky_Construct@16")
extern "C" void FUN_4be8d0(); KING_ALIAS("_FUN_4be8d0", "@Snow_Construct@16")
extern "C" void FUN_4c0e30(); KING_ALIAS("_FUN_4c0e30", "@Effects_Construct@20")
extern "C" void FUN_4b8c40(); KING_ALIAS("_FUN_4b8c40", "_Fx_LoadSet695bf0")
extern "C" void FUN_4b8fb0(); KING_ALIAS("_FUN_4b8fb0", "_Fx_LoadVapor")
extern "C" void FUN_4b8b80(); KING_ALIAS("_FUN_4b8b80", "_Fx_LoadNumbered")
extern "C" void FUN_4b8f00(); KING_ALIAS("_FUN_4b8f00", "_Fx_LoadSet694a00")
extern "C" void FUN_4b8ad0(); KING_ALIAS("_FUN_4b8ad0", "_Fx_LoadSet694998")
extern "C" void FUN_4b8a10(); KING_ALIAS("_FUN_4b8a10", "_Fx_LoadSet69618c")
extern "C" void FUN_4b8d00(); KING_ALIAS("_FUN_4b8d00", "_Fx_LoadLightMaterials")
extern "C" void FUN_4b9080(); KING_ALIAS("_FUN_4b9080", "_Fx_LoadShadows")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_4bb390(); KING_ALIAS("_FUN_4bb390", "_Fire_RandomRamps")
extern "C" void FUN_4c4750(); KING_ALIAS("_FUN_4c4750", "_Fx_Update2")
extern "C" void FUN_4cc850(); KING_ALIAS("_FUN_4cc850", "_Effect_AttachNode")
extern "C" void FUN_5d2540(); KING_ALIAS("_FUN_5d2540", "@NameHash_Add@16")
extern "C" void FUN_5d27c0(); KING_ALIAS("_FUN_5d27c0", "@NameHash_ResolvePending@8")

static BYTE* Fi_Find(const char* name)
{
    return Fx_FindByName(name, g_fi696200);
}

static bool Fi_Between(double t, float lo, float hi)
{
    return t > lo && t < hi;
}

extern "C" {

// [FUN_4b9570] sets up the effects of world w: the card quirks, the effect pools and scene nodes, the trail,
// the sky, clouds and sun when enabled, the effect sets and their names
void __cdecl Effects_Init(BYTE* w)
{
    g_fi6716f4 = 0;
    g_fi6949fc = 0;
    CallC(FUN_5cb990, 1);
    DWORD id[268];
    memset(id, 0, sizeof id);
    BYTE* dd = g_fiDraw72090c;
    if ((*(HRESULT(__stdcall**)(BYTE*, DWORD*, int))(*(BYTE**)dd + 108))(dd, id, 0) >= 0 && id[258] == 4634
        && (id[259] == 1 || id[259] == 2))
        g_fi6949fc = 1;
    CallC(FUN_4b87a0);
    CallC(FUN_4b91a0);
    CallC(FUN_5e48f0, w);
    g_fiWorld696224 = w;
    g_fi696200 = CallThis<BYTE*>(FUN_5d9290, w, (const char*)s_fi672688);
    g_fi696204 = CallThis<BYTE*>(FUN_5d9290, g_fiWorld696224, (const char*)s_fi66cd44);
    BYTE* p = (BYTE*)operator new(88);
    g_fi6f63d0 = p ? CallThis<BYTE*>(FUN_57c290, p, g_fiWorld696224, g_fiDevice720908) : 0;
    p = (BYTE*)operator new(96);
    g_fi696254 = p ? CallThis<BYTE*>(FUN_4c8640, p, g_fiWorld696224, g_fiDevice720908) : 0;
    p = (BYTE*)operator new(60);
    p = p ? CallThis<BYTE*>(FUN_593040, p, (const char*)s_fi672678) : 0;
    g_fi6f6a68 = p;
    g_fi69622c = p;
    p = (BYTE*)operator new(64);
    if (p) {
        CallThis(FUN_5f19b0, p, (const char*)s_fi67266c, 5, 1);
        F(void*, p, 0) = g_fiVtbl64c26c;
        F(int, p, 56) = 0;
        F(int, p, 60) = 0;
    }
    g_fi696230 = p;
    BYTE* a = Fi_Find(s_fi6725d0);
    CallC(FUN_5c6400, Fi_Find(s_fi672660), a);
    CallC(FUN_5cb9a0, (void*)Eff_New4cb450, (void*)Ground_Below);
    p = (BYTE*)operator new(88);
    if (p)
        p = CallThis<BYTE*>(FUN_4bc690, p, g_fi6cec68 ? (const char*)s_fi672658 : (const char*)s_fi672650,
                            g_fiWorld696224, 20);
    g_fi6961f8 = p;
    CallThis(FUN_4bc950, p, g_fi696230);
    if (BYTE* o = F(BYTE*, w, 524)) {
        CallVirt(o, 20, 0, g_fi696230);
        CallVirt(F(BYTE*, w, 524), 20, 0, g_fi6f6a68);
    }
    double t = F(double, g_fiWorld696224, 2920);
    g_fi6716fc = Fi_Between(t, g_fi64c13c, g_fi64c140);
    g_fi6716f8 = Fi_Between(t, g_fi64c144, g_fi64c148);
    if (g_fi696060 > g_fi64c258) {
        p = (BYTE*)operator new(52);
        if (p)
            Sky_Construct(p, 0, g_fiWorld696224, Fi_Find(s_fi672648));
    }
    if (g_fi696080 > g_fi64c258) {
        p = (BYTE*)operator new(48);
        if (p)
            CallThis(FUN_4be8d0, p, g_fiWorld696224, Fi_Find(s_fi672640));
    }
    if (g_fi696044 > g_fi64c258) {
        p = (BYTE*)operator new(72);
        if (p) {
            BYTE* f640 = Fi_Find(s_fi672640);
            p = CallThis<BYTE*>(FUN_4c0e30, p, g_fiWorld696224, Fi_Find(s_fi672638), f640);
        }
        g_fi696234 = p;
    }
    g_fi696208 = 0;
    memset(g_fi694998, 0, 100);
    memset(g_fi694a00, 0, 100);
    memset(g_fi695908, 0, 100);
    memset(g_fi695c58, 0, 100);
    memset(g_fi695bf0, 0, 100);
    g_fi69620c = 0;
    g_fi696214 = 0;
    g_fi696210 = 0;
    CallC(FUN_4b8c40);
    CallC(FUN_4b8fb0);
    CallC(FUN_4b8b80);
    CallC(FUN_4b8f00);
    CallC(FUN_4b8ad0);
    CallC(FUN_4b8a10);
    CallC(FUN_4b8d00);
    CallC(FUN_4b9080);
    BYTE* o = CallC<BYTE*>(FUN_5ef2b0, 0, (const char*)s_fi67262c);
    BYTE* m = o ? F(BYTE*, o, 56) : 0;
    g_fi696228 = m;
    g_fi671700 = m == 0 ? 200 : 100;
    g_fi694830 = m == (BYTE*)2 ? g_fi64c1c4 : g_fi64c1c0;
    CallC(FUN_4bb390);
    CallC(FUN_4c4750, g_fiWorld696224);
    CallC(FUN_4cc850, g_fi696230);
    CallThis(FUN_5d2540, (BYTE*)g_fiNames71e820, (const char*)s_fi67261c, (void*)Lights_Collect);
    CallThis(FUN_5d2540, (BYTE*)g_fiNames71e820, (const char*)s_fi67260c, (void*)LampGlows_Draw);
    CallThis(FUN_5d2540, (BYTE*)g_fiNames71e820, (const char*)s_fi6725f8, (void*)g_fiCb4cbec0);
    CallThis(FUN_5d2540, (BYTE*)g_fiNames71e820, (const char*)s_fi6725ec, (void*)g_fiCb4cbfd0);
    CallThis(FUN_5d27c0, (BYTE*)g_fiNames71e820);
}

} // extern "C"

// === the effects manager (FUN_4c0e30)

KING_VAR(AsmLabel, g_em64c604, 64c604)
KING_VAR(AsmLabel, g_em64c5f0, 64c5f0)
KING_VAR(AsmLabel, g_em64c5d4, 64c5d4)
KING_VAR(AsmLabel, g_em64c5b8, 64c5b8)
KING_VAR(AsmLabel, g_em64c59c, 64c59c)
KING_VAR(AsmLabel, g_em64c57c, 64c57c)
KING_VAR(int, g_em6f33b0, 6f33b0)
KING_VAR(AsmLabel, s_em672770, 672770)
KING_VAR(AsmLabel, s_em67266c, 67266c)
KING_VAR(AsmLabel, s_em672750, 672750)
extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_4c16b0(); KING_ALIAS("_FUN_4c16b0", "@Decals4c16b0_Construct@8")
extern "C" void FUN_4c17d0(); KING_ALIAS("_FUN_4c17d0", "@Decals4c17d0_Construct@8")
extern "C" void FUN_4c1d40(); KING_ALIAS("_FUN_4c1d40", "@Decals4c1d40_Construct@8")
extern "C" void FUN_4c2270(); KING_ALIAS("_FUN_4c2270", "@Lights50_Construct@12")
extern "C" void FUN_4c26f0(); KING_ALIAS("_FUN_4c26f0", "@RendererC_Construct@8")
extern "C" void FUN_4c2710(); KING_ALIAS("_FUN_4c2710", "@Renderer_Create3@20")
extern "C" void FUN_4c28e0(); KING_ALIAS("_FUN_4c28e0", "@RendererD_Construct@8")
extern "C" void FUN_4c2ac0(); KING_ALIAS("_FUN_4c2ac0", "@Sparks_Alloc@8")
extern "C" void FUN_4c3cc0(); KING_ALIAS("_FUN_4c3cc0", "@Lights50b_Construct@8")
extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")

// the free list of e's +8 entries of 24 bytes (+56 storage, +48 the first free, +52 none used); the entries
// are constructed when init is set
static void Em_InitPool(BYTE* e, bool init)
{
    int n = F(int, e, 8);
    BYTE* a = (BYTE*)operator new(n * 24);
    if (init && a)
        CallStd(FUN_4011e0, a, 24, n, (void*)Fx_Self);
    F(BYTE*, e, 56) = a;
    for (int i = 0; i < F(int, e, 8) - 1; i++)
        F(BYTE*, F(BYTE*, e, 56) + i * 24, 0) = F(BYTE*, e, 56) + i * 24 + 24;
    F(BYTE*, F(BYTE*, e, 56) + F(int, e, 8) * 24 - 24, 0) = 0;
    F(BYTE*, e, 48) = F(BYTE*, e, 56);
    F(int, e, 52) = 0;
}

// the nodes a and b drawn by the effects: full alpha, flags 0x40000 and 2
static void Em_SetNode(BYTE* n)
{
    F(float, n, 88) = 1.0f;
    F(DWORD, n, 32) |= 0x40000;
    F(DWORD, n, 32) |= 2;
}

// a new renderer (made by ctor) for the scene object 67266c of owner with node n
static BYTE* Em_Renderer(void (*ctor)(), BYTE* owner, BYTE* n)
{
    BYTE* r = (BYTE*)operator new(80);
    if (r)
        r = CallThis<BYTE*>(ctor, r);
    BYTE* o = CallC<BYTE*>(FUN_5ef2b0, F(BYTE*, owner, 524), (const char*)s_em67266c);
    CallThis(FUN_4c2710, r, o, n, 2000);
    return r;
}

extern "C" {

// [FUN_4c0e30] constructor (owner, a, b): an object of kind 98 attached to owner, drawing through nodes a and
// b, with its four particle pools (+40 decals, +48 lights, +44 the weather lights 6f33b0, +52 sparks) and the
// two renderers +32, +36; +68 the scene object 672750
BYTE* __fastcall Effects_Construct(BYTE* s, void*, BYTE* owner, BYTE* a, BYTE* b)
{
    CallThis(FUN_5d31e0, s, 98, (const char*)s_em672770);
    F(void*, s, 0) = g_em64c604;
    F(BYTE*, s, 64) = owner;
    CallThis(FUN_5d3570, owner + 2992, s);
    CallThis(FUN_5d33d0, s, 1);
    F(BYTE*, s, 56) = a;
    Em_SetNode(a);
    F(BYTE*, s, 60) = b;
    Em_SetNode(b);

    BYTE* e = (BYTE*)operator new(9280);
    if (e) {
        Sparks_Construct(e, 0, (int)owner);
        CallThis(FUN_4c16b0, e + 64);
        F(void*, e, 64) = g_em64c5f0;
        CallThis(FUN_4c1d40, e + 4672);
        F(void*, e, 0) = g_em64c5d4;
        F(DWORD, e, 60) = 0xff505050;
    }
    F(BYTE*, s, 40) = e;
    Em_InitPool(e, false);

    e = (BYTE*)operator new(672);
    if (e) {
        Sparks_Construct(e, 0, (int)owner);
        CallThis(FUN_4c3cc0, e + 64);
        F(void*, e, 0) = g_em64c5b8;
        F(DWORD, e, 60) = 0xff505050;
    }
    F(BYTE*, s, 48) = e;
    Em_InitPool(e, false);

    e = (BYTE*)operator new(72);
    if (e) {
        int w = g_em6f33b0;
        Sparks_Construct(e, 0, (int)owner);
        F(void*, e, 0) = g_em64c59c;
        F(int, e, 68) = w;
        BYTE* l = (BYTE*)operator new(612);
        F(BYTE*, e, 64) = l ? CallThis<BYTE*>(FUN_4c2270, l, F(int, e, 68)) : 0;
        F(DWORD, e, 60) = 0xff505050;
    }
    F(BYTE*, s, 44) = e;
    Em_InitPool(e, true);

    e = (BYTE*)operator new(4672);
    if (e) {
        Sparks_Construct(e, 0, (int)owner);
        CallThis(FUN_4c17d0, e + 64);
        F(void*, e, 0) = g_em64c57c;
        F(int, e, 60) = -1;
        F(DWORD, e, 36) = 0xbf800000;
        F(DWORD, e, 28) = 0x3e428f5c;
        F(int, e, 32) = 0;
        F(DWORD, e, 12) = 0x3c23d70a;
    }
    F(BYTE*, s, 52) = e;
    CallThis(FUN_4c2ac0, e);

    F(BYTE*, s, 32) = Em_Renderer(FUN_4c28e0, owner, F(BYTE*, s, 56));
    F(BYTE*, s, 36) = Em_Renderer(FUN_4c26f0, owner, F(BYTE*, s, 60));
    F(BYTE*, s, 68) = CallC<BYTE*>(FUN_5ef2b0, F(BYTE*, owner, 524), (const char*)s_em672750);
    return s;
}

} // extern "C"

// === loading a vehicle resource (FUN_4d8b20) and a present's car object ($L_4d8b60)

KING_VAR(BYTE*, g_pc696ca0, 696ca0)
KING_VAR(BYTE*, g_pc6cec6c, 6cec6c)
KING_VAR(int, g_pc68b990, 68b990)
KING_VAR(AsmLabel, g_pc6d24bc, 6d24bc)
KING_VAR(AsmLabel, g_pc6ced98, 6ced98)
KING_VAR(AsmLabel, g_pc7025f0, 7025f0)
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8")
extern "C" void FUN_41be60(); KING_ALIAS("_FUN_41be60", "_Ai_GetPresentData")
extern "C" void FUN_41d300(); KING_ALIAS("_FUN_41d300", "@Int_Copy@12")
extern "C" void FUN_43f700(); KING_ALIAS("_FUN_43f700", "_Rec_Field8")
extern "C" void FUN_43fbd0(); KING_ALIAS("_FUN_43fbd0", "_Rec_SetField0")
extern "C" void FUN_44b090(); KING_ALIAS("_FUN_44b090", "@Vec1_Free@8")
extern "C" void FUN_568f60(); KING_ALIAS("_FUN_568f60", "_Player_CarId")
extern "C" void FUN_571a80(); KING_ALIAS("_FUN_571a80", "_Net_CreateCarObject")
extern "C" void FUN_571c20(); KING_ALIAS("_FUN_571c20", "_Semitrailer_Create")
extern "C" void FUN_5e4900(); KING_ALIAS("_FUN_5e4900", "@ResTable_Charge@12")
extern "C" void FUN_5e49d0(); KING_ALIAS("_FUN_5e49d0", "@Resource_Load@12")
extern "C" void FUN_5e5210(); KING_ALIAS("_FUN_5e5210", "@Resources_Delete@12")
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")
extern "C" void FUN_5e54c0(); KING_ALIAS("_FUN_5e54c0", "@Resources_AddRecord@20")

struct Pc_M48 { DWORD d[12]; };

// the present data the AI fills (272 bytes + 2 ints): +4 matrix, +64/+68/+72 kinds, +80 effect state,
// +272 / +276 table indices
struct Pc_Data { BYTE b[280]; };

static void Pc_Init(Pc_Data* p)
{
    F(int, p, 0) = 0;
    CallStd(FUN_4011e0, p->b + 4, 12, 4, FUN_4bd4d0);
    memcpy(p->b + 4, (BYTE*)g_pc7025f0, 48);
    for (int k = 52; k <= 68; k += 4)
        F(int, p, k) = 0;
    F(int, p, 72) = -1;
    F(int, p, 76) = -1;
    Fx_InitState(p->b + 80, 0);
    F(int, p, 272) = -1;
    F(int, p, 276) = -1;
}

static void Pc_Free(Pc_Data* p)
{
    CallThis(FUN_44b090, p->b + 168);
    CallThis(FUN_44b090, p->b + 156);
    CallThis(FUN_44b090, p->b + 144);
    CallThis(FUN_404420, p->b + 132);
}

// the present it as the player's car sees it
static void Pc_Get(Pc_Data* p, const int* it)
{
    int t, car;
    CallThis(FUN_41d300, &t, it);
    CallC(FUN_568f60, &car);
    CallC(FUN_41be60, car, t, p);
}

extern "C" {

// [FUN_4d8b20] charges the world resource of id (+704 of the world) when it isn't loaded yet
void __cdecl Vehicle_LoadResource(int id)
{
    BYTE* r = CallThis<BYTE*>(FUN_5e5370, g_pc696ca0 + 704, id);
    if (r && !F(int, r, 28))
        CallThis(FUN_5e4900, r, g_pc696ca0 + 704);
}

// $L_4d8b60 (inside FUN_4d8b20): a present record's event (record it, kind): 1 creates its car object (by the
// present's kind bits: a car, a trailer of table 6d24bc, a cargo) or updates the existing one's texture; 4
// removes its world resource (not for the 101xxx ids); 0 for 1, else -1
int __cdecl Present_OnEvent(BYTE* it, int kind)
{
    BYTE* world = F(BYTE*, g_pc6cec6c, 1356);
    if (kind == 1) {
        Pc_Data p;
        Pc_Init(&p);
        Pc_Get(&p, (int*)&it);
        int id = F(int, F(BYTE*, it, 8), 0);
        if (id < 0) {
            Pc_M48 m = *(Pc_M48*)(p.b + 4);
            int fl = CallC<int>(FUN_43f700, it);
            if (fl & 16)
                id = CallC<int>(FUN_571a80, world, F(int, &p, 64), F(int, &p, 68), m, F(int, &p, 272));
            else if (fl & 32)
                id = CallC<int>(FUN_571c20, world, F(int, &p, 64), F(int, &p, 68), m,
                                F(int, &p, 272) + 1000 * F(int, &p, 276));
            else if (fl & 192)
                id = CallC<int>(FUN_571a80, world, F(int, &p, 64), F(int, &p, 68), m, F(int, &p, 272));
            int t;
            CallThis(FUN_41d300, &t, &it);
            CallC(FUN_43fbd0, t, id);
        } else {
            int fl = CallC<int>(FUN_43f700, it);
            BYTE* r = CallThis<BYTE*>(FUN_5e5370, world + 704, id);
            BYTE* c;
            if (fl & 32) {
                if (r)
                    F(int, F(BYTE*, r, 24), 296) = g_pc68b990;
                c = F(BYTE*, (BYTE*)g_pc6d24bc + F(int, &p, 272) * 1488, 12);
            } else
                c = F(BYTE*, (BYTE*)g_pc6ced98 + F(int, &p, 272) * 44, 0);
            if (c)
                F(int, r, 16) = F(int, c, 16);
        }
        Pc_Free(&p);
        return 0;
    }
    if (kind == 4) {
        Pc_Data p;
        Pc_Init(&p);
        Pc_Get(&p, (int*)&it);
        int id = F(int, F(BYTE*, it, 8), 0);
        BYTE* r = CallThis<BYTE*>(FUN_5e5370, world + 704, id);
        if (r && id / 1000 != 101) {
            if (F(int, r, 28))
                CallThis(FUN_5e49d0, r, world + 704);
            CallThis(FUN_5e54c0, world + 704, 0, r, 0);
            CallThis(FUN_5e5210, world + 704, id);
        }
        Pc_Free(&p);
    }
    return -1;
}

} // extern "C"

// === setting up the world's effects (FUN_4b9b90)

KING_VAR(AsmLabel, g_ew64c2d8, 64c2d8)
KING_VAR(AsmLabel, g_ew64c2b8, 64c2b8)
KING_VAR(BYTE*, g_ew6961f4, 6961f4)
KING_VAR(BYTE*, g_ew696224, 696224)
KING_VAR(BYTE*, g_ew6f33b4, 6f33b4)
KING_VAR(BYTE*, g_ew6cec70, 6cec70)
KING_VAR(BYTE*, g_ew696200, 696200)
KING_VAR(BYTE*, g_ew6f66a8, 6f66a8)
KING_VAR(BYTE*, g_ew696238, 696238)
KING_VAR(BYTE*, g_ew696230, 696230)
KING_VAR(BYTE*, g_ew696234, 696234)
KING_VAR(BYTE*, g_ew6961f8, 6961f8)
KING_VAR(int, g_ew6cec68, 6cec68)
KING_VAR(int, g_ew6716fc, 6716fc)
KING_VAR(int, g_ew6716f8, 6716f8)
KING_VAR(float, g_ew696060, 696060)
KING_VAR(float, g_ew696080, 696080)
KING_VAR(float, g_ew696044, 696044)
KING_VAR(float, g_ew64c268, 64c268)
KING_VAR(float, g_ew64c13c, 64c13c)
KING_VAR(float, g_ew64c140, 64c140)
KING_VAR(float, g_ew64c144, 64c144)
KING_VAR(float, g_ew64c148, 64c148)
KING_VAR(float, g_ew64c258, 64c258)
KING_VAR(AsmLabel, s_ew672698, 672698)
KING_VAR(AsmLabel, s_ew672688, 672688)
KING_VAR(AsmLabel, s_ew672690, 672690)
KING_VAR(AsmLabel, s_ew672648, 672648)
KING_VAR(AsmLabel, s_ew672640, 672640)
KING_VAR(AsmLabel, s_ew672638, 672638)
KING_VAR(AsmLabel, s_ew672658, 672658)
KING_VAR(AsmLabel, s_ew672650, 672650)
extern "C" void FUN_4b86c0(); KING_ALIAS("_FUN_4b86c0", "_Fx_SetPaletteColors")
extern "C" void FUN_4b8720(); KING_ALIAS("_FUN_4b8720", "_Fx_FindByName")
extern "C" void FUN_4b87a0(); KING_ALIAS("_FUN_4b87a0", "_Eff_CreateSounds")
extern "C" void FUN_4b91a0(); KING_ALIAS("_FUN_4b91a0", "_Effects_FromSettings")
extern "C" void FUN_4bc690(); KING_ALIAS("_FUN_4bc690", "@Trail_Construct@20")
extern "C" void FUN_4bc950(); KING_ALIAS("_FUN_4bc950", "@Effect4bc950_Attach@12")
extern "C" void FUN_4bcbe0(); KING_ALIAS("_FUN_4bcbe0", "@LightPool_Free@8")
extern "C" void FUN_4be8d0(); KING_ALIAS("_FUN_4be8d0", "@Snow_Construct@16")
extern "C" void FUN_4bee30(); KING_ALIAS("_FUN_4bee30", "@Sky_Construct@16")
extern "C" void FUN_4c4770(); KING_ALIAS("_FUN_4c4770", "_Eff_ResetSmokeAndGlows")
extern "C" void FUN_4c9970(); KING_ALIAS("_FUN_4c9970", "@Effect4c9970_Construct@16")
extern "C" void FUN_4cb4d0(); KING_ALIAS("_FUN_4cb4d0", "_Effect_Shared6436")
extern "C" void FUN_4cb550(); KING_ALIAS("_FUN_4cb550", "_Fx_Clear6f6a78")
extern "C" void FUN_593430(); KING_ALIAS("_FUN_593430", "@MovBodies_Construct@12")
extern "C" void FUN_59daf0(); KING_ALIAS("_FUN_59daf0", "_DustColor_Create")
extern "C" void FUN_5d3110(); KING_ALIAS("_FUN_5d3110", "@Object_Construct@12")
extern "C" void FUN_5d9290(); KING_ALIAS("_FUN_5d9290", "@Scenes_Find@12")
extern "C" void FUN_6144f0(); KING_ALIAS("_FUN_6144f0", "@Terrain_SetDetail@16")
extern "C" void FUN_614570(); KING_ALIAS("_FUN_614570", "@Terrain_CreateDecals@12")
extern "C" void FUN_5a37f0(); KING_ALIAS("_FUN_5a37f0", "_WorldHandlers_Create")

// the effect texture of name in the scene 696200
static BYTE* Ew_Find(const AsmLabel& name)
{
    return CallC<BYTE*>(FUN_4b8720, (const char*)name, g_ew696200);
}

extern "C" {

// [FUN_4b9b90] sets up the effects of world w: the effect manager objects (6961f4, kind 20; a plain one), the
// sounds and tables, the terrain detail, moving bodies (6f66a8), handlers, 696238; with the weather (696230)
// the season flags 6716fc / 6716f8 from the date (+2920), the sky, clouds and the effects (696234) when their
// amounts are set; the colors, dust and the light trail (6961f8)
void __cdecl Effects_SetupWorld(BYTE* w)
{
    BYTE* o = (BYTE*)operator new(40);
    if (o) {
        CallThis(FUN_5d31e0, o, 20, (const char*)s_ew672698);
        F(void*, o, 0) = g_ew64c2d8;
        BYTE* buf = (BYTE*)operator new(1020);
        F(BYTE*, o, 32) = buf;
        memset(buf, 0, 1020);
        F(int, o, 36) = 0;
        CallThis(FUN_5d3570, w + 2992, o);
        CallThis(FUN_5d33d0, o, 1);
    }
    g_ew6961f4 = o;
    CallC(FUN_4b87a0);
    CallC(FUN_4b91a0);
    g_ew696224 = w;
    CallC(FUN_4cb550);
    CallC(FUN_4cb4d0);
    if (BYTE* t = g_ew6f33b4) {
        float a = (float)(F(short, g_ew6cec70, 420) * (double)g_ew64c268);
        float b = (float)(F(short, g_ew6cec70, 422) * (double)g_ew64c268);
        CallThis(FUN_6144f0, t, a, b);
        CallThis(FUN_614570, g_ew6f33b4, w);
    }
    g_ew696200 = CallThis<BYTE*>(FUN_5d9290, g_ew696224, (const char*)s_ew672688);
    BYTE* mb = (BYTE*)operator new(120);
    g_ew6f66a8 = mb ? CallThis<BYTE*>(FUN_593430, mb, g_ew696224) : 0;
    CallC(FUN_5a37f0, w);
    BYTE* e = (BYTE*)operator new(44);
    g_ew696238 = e ? CallThis<BYTE*>(FUN_4c9970, e, g_ew696224, 0) : 0;
    o = (BYTE*)operator new(32);
    if (o) {
        CallThis(FUN_5d3110, o, (const char*)s_ew672690);
        F(void*, o, 0) = g_ew64c2b8;
        CallThis(FUN_5d3570, w + 2992, o);
        CallThis(FUN_5d33d0, o, 1);
    }
    if (g_ew696230) {
        double d = F(double, g_ew696224, 2920);
        g_ew6716fc = d > g_ew64c13c && d < g_ew64c140;
        g_ew6716f8 = d > g_ew64c144 && d < g_ew64c148;
        if (g_ew696060 > g_ew64c258) {
            BYTE* s = (BYTE*)operator new(52);
            if (s)
                CallThis(FUN_4bee30, s, g_ew696224, Ew_Find(s_ew672648));
        }
        if (g_ew696080 > g_ew64c258) {
            BYTE* s = (BYTE*)operator new(48);
            if (s)
                CallThis(FUN_4be8d0, s, g_ew696224, Ew_Find(s_ew672640));
        }
        if (g_ew696044 > g_ew64c258) {
            BYTE* s = (BYTE*)operator new(72);
            if (s) {
                BYTE* b = Ew_Find(s_ew672648);
                s = Effects_Construct(s, 0, g_ew696224, Ew_Find(s_ew672638), b);
            }
            g_ew696234 = s;
        }
    }
    CallC(FUN_4b86c0);
    CallC(FUN_59daf0, g_ew696224);
    if (BYTE* old = g_ew6961f8) {
        CallThis(FUN_4bcbe0, old);
        operator delete(old);
        BYTE* t = (BYTE*)operator new(88);
        if (t)
            t = CallThis<BYTE*>(FUN_4bc690, t, g_ew6cec68 ? (const char*)s_ew672658 : (const char*)s_ew672650,
                                g_ew696224, 20);
        g_ew6961f8 = t;
        CallThis(FUN_4bc950, t, g_ew696230);
    }
    CallC(FUN_4c4770);
}

} // extern "C"

// === a spinning glow (FUN_4dd2a0)

KING_VAR(AsmLabel, g_sg64d554, 64d554)
KING_VAR(AsmLabel, s_sg672ca4, 672ca4)
KING_VAR(float, g_sg64c254, 64c254)
KING_VAR(float, g_sg64c258, 64c258)
KING_VAR(float, g_sg64c350, 64c350)
KING_VAR(float, g_sg64c408, 64c408)
KING_VAR(double, g_sg64b660, 64b660)
KING_VAR(AsmLabel, g_sg7025f0, 7025f0)

static void Sg_Set(BYTE* o, int off, const DWORD* v) { memcpy(o + off, v, 12); }

extern "C" {

// [FUN_4dd2a0] constructor (thiscall: owner, matrix m, kind, +180, +184) of a spinning glow attached to owner
// at m: three axes (+56, +68, +80, with indices +92..+100) ordered by kind (kind 1 randomly), random spin
// rates +32..+40 (each axis a bit faster), +116 the fastest and +120 that plus 64b660
BYTE* __fastcall Glow_Construct(BYTE* o, void*, BYTE* owner, const BYTE* m, int kind, int a4, int a5)
{
    CallThis(FUN_5d3110, o, (const char*)s_sg672ca4);
    memset(o + 56, 0, 36);
    memset(o + 132, 0, 48);
    memcpy(o + 132, (BYTE*)g_sg7025f0, 48);
    F(void*, o, 0) = g_sg64d554;
    F(BYTE*, o, 128) = owner;
    CallThis(FUN_5d3570, owner + 2992, o);
    F(int, o, 124) = 0;
    memcpy(o + 132, m, 48);
    F(int, o, 180) = a4;
    F(int, o, 184) = a5;
    static const DWORD A[3] = {3239051264u, 1065353216, 1065353216};
    static const DWORD B[3] = {0, 1065353216, 1065353216};
    static const DWORD C[3] = {1090519040, 1065353216, 1065353216};
    if (kind == 0) {
        Sg_Set(o, 56, A);
        F(int, o, 92) = 0;
        Sg_Set(o, 68, B);
        F(int, o, 96) = 1;
        F(int, o, 100) = 2;
        Sg_Set(o, 80, C);
    } else if (kind == 1) {
        Sg_Set(o, 56, B);
        F(int, o, 92) = 1;
        if (rand() * (double)g_sg64c254 > g_sg64c258) {
            F(int, o, 96) = 0;
            F(int, o, 100) = 2;
            Sg_Set(o, 68, A);
            Sg_Set(o, 80, C);
        } else {
            F(int, o, 96) = 2;
            Sg_Set(o, 68, C);
            Sg_Set(o, 80, A);
            F(int, o, 100) = 0;
        }
    } else {
        Sg_Set(o, 56, C);
        F(int, o, 92) = 2;
        F(int, o, 96) = 1;
        Sg_Set(o, 68, B);
        Sg_Set(o, 80, A);
        F(int, o, 100) = 0;
    }
    for (int i = 0; i < 3; i++) {
        F(int, o, 44 + i * 4) = 0;
        F(int, o, 104 + i * 4) = 0;
        float r1 = (float)(rand() * (double)g_sg64c254);
        double r2 = rand() * (double)g_sg64c254;
        F(float, o, 32 + i * 4) = (float)((r2 - r1) * g_sg64c350 + (double)i * g_sg64c408);
    }
    F(float, o, 116) = 0;
    for (int i = 0; i < 3; i++)
        if (F(float, o, 116) < F(float, o, 32 + i * 4))
            F(float, o, 116) = F(float, o, 32 + i * 4);
    F(float, o, 120) = (float)(F(float, o, 116) + g_sg64b660);
    CallThis(FUN_5d33d0, o, 1);
    return o;
}

} // extern "C"

// === placing the vehicles' effect objects (FUN_4d90c0)

extern "C" void FUN_4d7630(); KING_ALIAS("_FUN_4d7630", "_Fx_SetObject")
extern "C" void FUN_4d7680(); KING_ALIAS("_FUN_4d7680", "_Fx_SetObjectMatrix")
extern "C" void FUN_4e5ba7(); KING_ALIAS("_FUN_4e5ba7", "_Query_HitObject")
extern "C" void FUN_541960(); KING_ALIAS("_FUN_541960", "@Obj541960_Self@8")
extern "C" void FUN_541970(); KING_ALIAS("_FUN_541970", "@Obj541970_First@8")
extern "C" void FUN_546a30(); KING_ALIAS("_FUN_546a30", "@Car_GetState@12")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_57def0(); KING_ALIAS("_FUN_57def0", "@SceneObj_Get800@8")
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
extern "C" void FUN_5e54c0(); KING_ALIAS("_FUN_5e54c0", "@Resources_AddRecord@20")
KING_VAR(AsmLabel, g_po697868, 697868)    // the car models' 2416-byte records
KING_VAR(BYTE*, g_po6f653c, 6f653c)       // the moving objects

extern "C" {

// [FUN_4d90c0] places the effect object of every car (kinds 100 and 101 of +360 / 1000) by its body matrix
// (+708, its model's +697868 record, its scene matrix) and the surface under it (the player's car records a
// changed surface name), and copies the name and matrix of each kind-2 moving object (+3568 = 1) into its
// effect record
void __cdecl Fx_PlaceObjects()
{
    for (BYTE* it = CallThis<BYTE*>(FUN_541970, F(BYTE*, g_game, 616)); it;) {
        BYTE* car = CallThis<BYTE*>(FUN_541960, it);
        BYTE* res = CallThis<BYTE*>(FUN_5e5370, g_world + 704, F(int, car, 360));
        int kind = F(int, car, 360) / 1000;
        if (kind == 100 || kind == 101) {
            BYTE m[48], t1[48], t2[48], w[48], hit[16], st[180];
            memcpy(m, car + 708, 48);
            BYTE* body = car + 16;
            BYTE* sm = CallThis<BYTE*>(FUN_57de90, body, t1);
            BYTE* model = (BYTE*)g_po697868 + F(int, car, 10420) * 2416;
            memcpy(w, CallC<BYTE*>(FUN_5aba40, t2, CallC<BYTE*>(FUN_5aba40, w, model, m), sm), 48);
            BYTE t3[48];
            BYTE* pos = CallThis<BYTE*>(FUN_57de90, body, t3) + 36;
            BYTE* q = CallC<BYTE*>(FUN_4e5ba7, CallThis<BYTE*>(FUN_57def0, body), pos, hit);
            DWORD surf = F(DWORD, q, 12);
            CallThis(FUN_546a30, car, st);
            if (car == F(BYTE*, g_game, 616)) {
                const char* name = F(const char*, F(BYTE*, q, 36), 60);
                if (strcmp(name, F(const char*, F(BYTE*, res, 12), 4016)))
                    CallThis(FUN_5e54c0, g_world + 704, name, res, 0);
                CallC(FUN_4d7630, res, surf, w, st);
            } else if (kind == 101)
                CallC(FUN_4d7630, res, surf, w, st);
            else
                CallC(FUN_4d7680, res, w, st);
        }
        it = F(BYTE*, it, 8);
        if (!it)
            break;
        it -= 4;
    }
    BYTE* p = g_po6f653c;
    if (!p)
        return;
    for (p -= 4; p;) {
        BYTE* o = p - 376;
        if (F(int, p, 16) == 2 && F(int, o, 3568) == 1) {
            if (BYTE* res = CallThis<BYTE*>(FUN_5e5370, g_world + 704, F(int, o, 344))) {
                const char* name = F(const char*, CallThis<BYTE*>(FUN_57def0, o), 12);
                BYTE mt[48];
                CallThis(FUN_57de90, o, mt);
                if (BYTE* d = F(BYTE*, res, 24)) {
                    strcpy((char*)d + 92, name);
                    memcpy(d + 44, mt, 48);
                }
            }
        }
        p = F(BYTE*, p, 8);
        if (!p)
            break;
        p -= 4;
    }
}

} // extern "C"

// === subdividing a triangle (FUN_4c7d90)

extern "C" void FUN_4c7850(); KING_ALIAS("_FUN_4c7850", "_Triangle_Split4")    // one subdivision into 4 (vertices, &count, indices, &count, a, b, c, base)

extern "C" {

// [FUN_4c7d90] the triangle (a, b, c; 32-byte vertices) as vertices v and word indices x after `level`
// subdivisions into 4 (0: itself, 1: once, else twice); the vertex and index counts in *nv and *nx
void __cdecl Tri_Subdivide(BYTE* v, int* nv, WORD* x, int* nx, BYTE* a, BYTE* b, BYTE* c, int level)
{
    if (level == 0) {
        memcpy(v, a, 32);
        memcpy(v + 32, b, 32);
        memcpy(v + 64, c, 32);
        x[0] = 0;
        x[1] = 1;
        x[2] = 2;
        *nx = 3;
        *nv = 3;
        return;
    }
    if (level == 1) {
        CallC(FUN_4c7850, v, nv, x, nx, a, b, c, 0);
        return;
    }
    BYTE tv[6 * 32];
    WORD tx[14];
    int n1 = 0, n2 = 0;
    CallC(FUN_4c7850, tv, &n1, tx, &n2, a, b, c, 0);
    for (int t = 0; t < 4; t++)
        CallC(FUN_4c7850, v, &n1, x, &n2, tv + tx[t * 3] * 32, tv + tx[t * 3 + 1] * 32, tv + tx[t * 3 + 2] * 32,
              t * 6);
    *nx = n2 * 4;
    *nv = n1 * 4;
}

} // extern "C"

// === setting up the cab dashboard (FUN_4de290)

extern "C" void FUN_4e497a(); KING_ALIAS("_FUN_4e497a", "_Frame_GetLocalChecked")
extern "C" void FUN_507d00(); KING_ALIAS("_FUN_507d00", "_Carat_Read3")
extern "C" void FUN_5d9ef0(); KING_ALIAS("_FUN_5d9ef0", "@Scenes_SetFirst@12")
KING_VAR(BYTE*, g_cd696cac, 696cac)
KING_VAR(AsmLabel, g_cd702650, 702650)

extern "C" {

// [FUN_4de290] sets up the cab view s for scene list a: 80 identity matrices (+560) with cleared slots (+4400),
// the driver's seat (truck.ini CAR Driver_Seat, negated, +5092), the panel objects (Panel, PanelSpace,
// TachoSpace, SpeedSpace, FuelArrowSpace, the panel's keys and the KMeterNKey digits) and the local matrices of
// the three gauges' spaces (+4924, +4972, +5020)
void __fastcall Cab_Setup(BYTE* s, void*, BYTE* a)
{
    F(BYTE*, s, 4820) = a;
    for (int i = 0; i < 80; i++) {
        F(int, s, 4400 + i * 4) = 0;
        memcpy(s + 560 + i * 48, (BYTE*)g_7025f0, 48);
    }
    F(int, s, 4720) = 0;
    CallThis(FUN_5d9ef0, s, F(int, a, 468));
    float* seat = (float*)(s + 5092);
    CallC(FUN_507d00, "CAR", "Driver_Seat", (BYTE*)g_cd702650, seat, ".\truck.ini");
    seat[0] = -seat[0];
    seat[1] = -seat[1];
    seat[2] = -seat[2];
    F(DWORD, g_cd696cac, 88) = 0x3FCCCCCD;      // 1.6
    F(DWORD, g_cd696cac, 84) = 0x3F99999A;      // 1.2
    F(int, s, 5080) = 0;
    F(int, s, 4864) = 0;
    F(int, s, 4860) = 0;
    F(BYTE*, s, 4868) = CallC<BYTE*>(FUN_5ef2b0, 0, "Panel");
    F(BYTE*, s, 4872) = CallC<BYTE*>(FUN_5ef2b0, 0, "PanelSpace");
    F(BYTE*, s, 4876) = CallC<BYTE*>(FUN_5ef2b0, 0, "TachoSpace");
    F(BYTE*, s, 4880) = CallC<BYTE*>(FUN_5ef2b0, 0, "SpeedSpace");
    F(BYTE*, s, 4884) = CallC<BYTE*>(FUN_5ef2b0, 0, "FuelArrowSpace");
    F(BYTE*, s, 4888) = CallC<BYTE*>(FUN_5ef2b0, F(BYTE*, s, 4868), "GearKey");
    F(BYTE*, s, 4892) = CallC<BYTE*>(FUN_5ef2b0, F(BYTE*, s, 4868), "AvtoKey");
    F(BYTE*, s, 4896) = CallC<BYTE*>(FUN_5ef2b0, F(BYTE*, s, 4868), "ParkKey");
    F(BYTE*, s, 4900) = CallC<BYTE*>(FUN_5ef2b0, F(BYTE*, s, 4868), "FuelLampKey");
    for (int i = 0; i < 5; i++) {
        char name[80];
        sprintf(name, "KMeter%dKey", i);
        F(BYTE*, s, 4904 + i * 4) = CallC<BYTE*>(FUN_5ef2b0, F(BYTE*, s, 4868), name);
    }
    static const int k_spaces[3][2] = {{4876, 4924}, {4880, 4972}, {4884, 5020}};
    for (const auto& k : k_spaces)
        if (BYTE* o = F(BYTE*, s, k[0]))
            CallC(FUN_4e497a, o, s + k[1]);
        else
            memcpy(s + k[1], (BYTE*)g_7025f0, 48);
    F(int, s, 548) = 0;
    F(DWORD, s, 5076) = 1025563004;
    F(DWORD, s, 5068) = 1036059899;
    F(DWORD, s, 5072) = 1075235812;
}

} // extern "C"

// === the reflection effect (FUN_4c8640)

KING_VAR(AsmLabel, s_rf6728d0, 6728d0)    // "Reflection"
KING_VAR(AsmLabel, s_rf6728c0, 6728c0)
KING_VAR(AsmLabel, s_rf6728b0, 6728b0)
KING_VAR(AsmLabel, s_rf67289c, 67289c)
KING_VAR(AsmLabel, g_rf64c8b4, 64c8b4)
extern "C" BYTE* __fastcall RefStack_Clear(BYTE* s, void*);

extern "C" {

// [FUN_4c8640] constructor (scene, a): the reflection node (vtable 64c8b4, kind 24) with its reference stack
// (+76), registered with the scene's list (+524), and the three materials of the trucks model (+88) it drives
// (+80, +84 and the third), their flags set
BYTE* __fastcall Reflection_Construct(BYTE* s, void*, BYTE* scene, int a)
{
    CallThis(FUN_5ef6d0, s, (const char*)s_rf6728d0, 24);
    F(int, s, 72) = 0;
    F(int, s, 56) = 0;
    F(int, s, 48) = 0;
    F(int, s, 52) = 0;
    F(void*, s, 0) = g_rf64c8b4;
    F(int, s, 92) = a;
    BYTE* st = (BYTE*)operator new(404);
    F(BYTE*, s, 76) = st ? RefStack_Clear(st, 0) : 0;
    if (BYTE* l = F(BYTE*, scene, 524))
        CallVirt(l, 20, 0, s);
    BYTE* m = CallThis<BYTE*>(FUN_5d9290, scene, (const char*)g_strTrucks);
    F(BYTE*, s, 88) = m;
    F(BYTE*, s, 80) = 0;
    F(BYTE*, s, 84) = 0;
    BYTE* third = 0;
    for (unsigned i = 0; i < F(unsigned, m, 72); i++) {
        BYTE* r = F(BYTE*, m, 76) + i * 124;
        if (!strcmp((const char*)r, s_rf6728c0))
            F(BYTE*, s, 80) = r;
        if (!strcmp((const char*)r, s_rf6728b0))
            F(BYTE*, s, 84) = r;
        if (!strcmp((const char*)r, s_rf67289c))
            third = r;
    }
    if (!F(BYTE*, s, 80) || !F(BYTE*, s, 84) || !third)
        CallC(FUN_60aad0, 0x88760115, (const char*)g_strErrFormat, (const char*)g_strErrFile, 5385,
              (const char*)g_strErrDate);
    F(DWORD, F(BYTE*, s, 80), 32) = (F(DWORD, F(BYTE*, s, 80), 32) & ~0x80u) | 0x40040;
    F(DWORD, third, 32) |= 64;
    F(DWORD, F(BYTE*, s, 84), 32) = (F(DWORD, F(BYTE*, s, 84), 32) & ~0x80u) | 0x40000;
    return s;
}

} // extern "C"

// === the smoke burst (FUN_4dc070)

KING_VAR(AsmLabel, s_fx672c90, 672c90)
KING_VAR(AsmLabel, g_vtbl64d50c, 64d50c)
KING_VAR(float, g_sb702628, 702628)
KING_VAR(float, g_sb702648, 702648)
KING_VAR(float, g_sb64c408, 64c408)
KING_VAR(float, g_sb64d498, 64d498)
KING_VAR(float, g_sb64c40c, 64c40c)

// a random number in [-1, 1) scaled: the first rand subtracted from the second
static double Sb_RandDiff()
{
    float r1 = (float)(rand() * (double)g_eff64c254);
    int r2 = rand();
    return r2 * (double)g_eff64c254 - r1;
}

extern "C" {

// [FUN_4dc070] constructor (owner, frame m, position p) of a smoke burst: five puffs from p spread sideways
// (along 702620 and 702640) and up (along 702630), turned by m and placed through it
BYTE* __fastcall SmokeBurst_Construct(BYTE* s, void*, BYTE* owner, const Matrix* m, const Vec3* p)
{
    CallThis(FUN_5d3110, s, (const char*)s_fx672c90);
    for (int i = 44; i < 80; i += 4)
        F(int, s, i) = 0;
    for (int i = 0; i < 4; i++)
        CallThis(FUN_4bd4d0, s + 80 + i * 12);
    memcpy(s + 80, g_7025f0, 48);
    F(void*, s, 0) = (BYTE*)g_vtbl64d50c;
    F(BYTE*, s, 40) = owner;
    F(int, s, 36) = 0;
    F(float, s, 32) = g_fx672c4c;
    memcpy(s + 44, p, 12);
    memcpy(s + 68, p, 12);
    F(float, s, 136) = 4.0f;
    F(float, s, 140) = 0.5f;
    F(int, s, 132) = 0;
    const Vec3& up = g_axis702630e;
    F(float, s, 56) = up.x + up.x;
    F(float, s, 60) = up.y + up.y;
    F(float, s, 128) = 0.8f;
    F(float, s, 64) = up.z + up.z;
    memcpy(s + 80, m, 48);
    for (int k = 0; k < 5; k++) {
        double a = Sb_RandDiff();
        float a0 = (float)(g_ahead702620.x * a), a1 = (float)(g_ahead702620.y * a);
        Vec3 p1;
        p1.Set((float)(a0 * (double)g_sb64c408), (float)(a1 * (double)g_sb64c408),
               (float)(a * g_sb702628 * g_sb64c408));
        double b = Sb_RandDiff();
        float b0 = (float)(g_up702640.x * b), b1 = (float)(g_up702640.y * b);
        Vec3 p2;
        p2.Set((float)(b0 * (double)g_sb64c408), (float)(b1 * (double)g_sb64c408),
               (float)(b * g_sb702648 * g_sb64c408));
        double c = rand() * (double)g_sb64d498 + g_sb64c40c;
        float c0 = (float)(c * up.x), c1 = (float)(c * up.y);
        Vec3 p3;
        p3.Set((float)((double)p2.x + c0), (float)((double)c1 + p2.y), (float)(c * up.z + p2.z));
        Vec3 t;
        t.Set((float)((double)p3.x + p1.x), (float)((double)p3.y + p1.y), (float)((double)p3.z + p1.z));
        Vec3 r, w;
        Vec3* rv = Vec3_RotateBy(&r, &t, (const Matrix*)(s + 80));
        Fx_SmokePuffs((int)Vec3_MulMatrix(&w, (const Vec3*)(s + 68), (const Matrix*)(s + 80)), (int)rv);
    }
    CallThis(FUN_5d3570, F(BYTE*, s, 40) + 2992, s);
    return s;
}

} // extern "C"

// === the smoke burst's end (FUN_4dc410)

static int Sb_Bits(float f) { return *(int*)&f; }
KING_VAR(float, g_sb64d52c, 64d52c)
KING_VAR(float, g_sb64c404, 64c404)
KING_VAR(float, g_sb64c690, 64c690)
KING_VAR(float, g_sb64c664, 64c664)

extern "C" {

// [FUN_4dc410] destructor of the smoke burst: outside replays two fires of random sizes start at its position (one
// lifted along 702630 through its frame +80) and a BenzoNotify of the larger size
void __fastcall SmokeBurst_Destroy(BYTE* s)
{
    F(void*, s, 0) = (BYTE*)g_vtbl64d50c;
    if (F(int, g_6cec6c, 1300) != 2) {
        const float* m = (const float*)(s + 80);
        const Vec3& up = g_axis702630e;
        float h1 = (float)(rand() * (double)g_sb64d52c + g_sb64c404);
        BYTE* f1 = (BYTE*)operator new(108);
        if (f1) {
            double z = (double)up.z * g_sb64c408;
            float y = (float)(up.y * (double)g_sb64c408), x = (float)(up.x * (double)g_sb64c408);
            Vec3 v;
            v.x = (float)((double)y * m[3] + z * m[6] + (double)x * m[0]);
            v.y = (float)((double)x * m[1] + (double)y * m[4] + z * m[7]);
            v.z = (float)((double)x * m[2] + (double)y * m[5] + z * m[8]);
            double a = (double)F(float, s, 76) - up.z, b = (double)F(float, s, 72) - up.y,
                   c = (double)F(float, s, 68) - up.x;
            Vec3 w;
            w.Set((float)(b * m[3] + a * m[6] + c * m[0] + m[9]), (float)(c * m[1] + b * m[4] + a * m[7] + m[10]),
                  (float)(c * m[2] + b * m[5] + a * m[8] + m[11]));
            f1 = Fire_Construct(f1, 0, F(BYTE*, s, 40), &w, &v, Sb_Bits(h1));
        }
        F(int, f1, 100) = 1;
        F(float, f1, 104) = 0.7f;
        CallVirt(f1, 24, 1);
        float h2 = (float)(rand() * (double)g_sb64c690 + g_sb64c664);
        BYTE* f2 = (BYTE*)operator new(108);
        if (f2) {
            Vec3 u, v, t, w;
            u.Set((float)(up.x * (double)g_sb64c408), (float)(up.y * (double)g_sb64c408),
                  (float)(up.z * (double)g_sb64c408));
            v.Set((float)((double)u.y * m[3] + (double)u.z * m[6] + (double)u.x * m[0]),
                  (float)((double)u.x * m[1] + (double)u.y * m[4] + (double)u.z * m[7]),
                  (float)((double)u.x * m[2] + (double)u.y * m[5] + (double)u.z * m[8]));
            Vec3* p = Vec3_MulMatrix(&w, Vec3_Sub(&t, (const Vec3*)(s + 68), &up), (const Matrix*)m);
            f2 = Fire_Construct(f2, 0, F(BYTE*, s, 40), p, &v, Sb_Bits(h2));
        }
        CallVirt(f2, 24, 1);
        F(float, f2, 104) = 0.7f;
        if (BYTE* n = (BYTE*)operator new(56))
            BenzoNotify_Construct(n, 0, F(BYTE*, s, 40), F(int, s, 152), F(int, s, 144), F(int, s, 148),
                                  Sb_Bits(h1 < h2 ? h2 : h1));
    }
    CallThis(FUN_5d3290, s);
}

} // extern "C"

// === the dust puffs facing the camera (FUN_4cb560)

extern "C" void FUN_5a0ca0(); KING_ALIAS("_FUN_5a0ca0", "@DustPuff_Construct@28")    // a dust puff's constructor
KING_VAR(BYTE*, g_dp6f6abc, 6f6abc)
KING_VAR(AsmLabel, g_dp695cc0, 695cc0)
KING_VAR(AsmLabel, g_dp695cc4, 695cc4)
KING_VAR(AsmLabel, g_dp695cc8, 695cc8)
KING_VAR(double, g_dp64b678, 64b678)
KING_VAR(float, g_dp64c238, 64c238)
KING_VAR(float, g_dp64c23c, 64c23c)
KING_VAR(float, g_dp64c948, 64c948)
KING_VAR(float, g_dp64c944, 64c944)
KING_VAR(float, g_dp64c940, 64c940)
KING_VAR(float, g_dp64c408, 64c408)

// two dust puffs of kind k at p of size sz (flags 0 then 1, the second with 0 and the first with e)
static void Dp_Pair(const float* p, float sz, const void* k, int e)
{
    if (BYTE* o = CallC<BYTE*>(FUN_579380, 188, g_dp6f6abc))
        CallThis(FUN_5a0ca0, o, p, sz, 0, k, e);
    if (BYTE* o = CallC<BYTE*>(FUN_579380, 188, g_dp6f6abc))
        CallThis(FUN_5a0ca0, o, p, sz, 1, k, 0);
}

extern "C" {

// [FUN_4cb560] dust of kind (0..2) at p moving along dir with size sz on surface q (+52..+60, all zero for none):
// the size blends from double to the height above q with the distance from the camera, scales by how much dir
// faces the camera; no puffs when it faces away
void __cdecl Dust_Puffs(const float* p, const float* dir, float sz, const float* q, int kind)
{
    BYTE* cam = g_view;
    float cx = (float)F(double, cam, 2904), cy = (float)F(double, cam, 2912);
    float d[3];
    d[2] = (float)(F(double, cam, 2920) - p[2]);
    d[1] = (float)((double)cy - p[1]);
    d[0] = (float)((double)cx - p[0]);
    double len = Vec3_Length((const Vec3*)d);
    double two = (double)sz + sz;
    double v;
    if ((q[13] != g_dp64b678 || q[14] != g_eff64c264b || q[15] != g_eff64c264b) && q) {
        double t = (double)p[2] - q[15];
        if (t < 0)
            t = -t;
        float h = (float)(sz * (double)g_dp64c408);
        v = t > h ? h : t;
    } else
        v = sz * (double)g_dp64c408;
    double f = (len - g_dp64c238) / ((double)g_dp64c23c - g_dp64c238);
    if (f > g_eff64c250)
        f = g_eff64c250;
    else if (f < g_eff64c264b)
        f = g_eff64c264b;
    float w = (float)((g_eff64c250 - f) * two + f * v);
    float c = (float)(((double)dir[2] * d[2] + (double)dir[1] * d[1] + (double)dir[0] * d[0]) / len);
    if (c < g_dp64c948)
        return;
    double k = c < g_eff64c74c ? c * (double)g_dp64c944 : g_eff64c250;
    float w2 = (float)(k * w * g_dp64c940);
    float big = (float)(w2 * (double)g_eff64c350);
    if (kind == 0)
        Dp_Pair(p, big, (BYTE*)g_dp695cc8, 1);
    else if (kind == 1)
        Dp_Pair(p, w2, (BYTE*)g_dp695cc0, 1);
    else if (kind == 2)
        Dp_Pair(p, big, (BYTE*)g_dp695cc4, 2);
}

} // extern "C"

// === drawing rain streaks (4c3480)

extern "C" void FUN_5ac0c0(); KING_ALIAS("_FUN_5ac0c0", "_Matrix_RotationZ")
KING_VAR(float, g_rs64c460, 64c460)
KING_VAR(float, g_rs64c45c, 64c45c)
KING_VAR(float, g_rs64c750, 64c750)
KING_VAR(float, g_rs64c458, 64c458)

extern "C" {

// [$L_4c3480] draws the streaks of effect s (list +52: +4 position, +16 length, +20 alpha) into renderer r as a
// triangle each: the top point, then the bottom corners across the view (the camera's heading turned about z,
// crossed with up) at u 0.5 and 1; the grey (7025b8 +548, clamped) kept at +60; 0
int __fastcall Rain_DrawStreaks(BYTE* s, void*, BYTE* r)
{
    float m[12];
    float* rz = CallC<float*>(FUN_5ac0c0, m, (float)F(double, F(BYTE*, s, 4), 2928));
    const Vec3& a = g_ahead702620;
    const Vec3& u = g_axis702630e;
    Vec3 h, q;
    h.Set((float)((double)a.z * rz[6] + (double)a.y * rz[3] + (double)a.x * rz[0] + rz[9]),
          (float)((double)a.z * rz[7] + (double)a.y * rz[4] + (double)a.x * rz[1] + rz[10]),
          (float)((double)a.z * rz[8] + (double)a.y * rz[5] + (double)a.x * rz[2] + rz[11]));
    q.Set((float)((double)u.y * h.z - (double)u.z * h.y), (float)((double)u.z * h.x - (double)u.x * h.z),
          (float)((double)u.x * h.y - (double)u.y * h.x));
    float half[3] = {(float)(q.x * (double)g_64c258), (float)(q.y * (double)g_64c258),
                     (float)(q.z * (double)g_64c258)};
    double g = F(float, g_7025b8, 548) * (double)g_rs64c460;
    if (g > g_rs64c45c)
        g = g_rs64c45c;
    else if (g < g_eff64c334)
        g = g_eff64c334;
    int b = (int)(g * g_rs64c458);
    F(DWORD, s, 60) = ((((DWORD)b | 0xFFFFFF00) << 8 | b) << 8) | b;
    for (BYTE* p = F(BYTE*, s, 52); p; p = F(BYTE*, p, 0)) {
        DWORD col = (DWORD)(int)(F(float, p, 20) * (double)g_rs64c750) << 24 | (F(DWORD, s, 60) & 0xFFFFFF);
        float len = F(float, p, 16);
        const Vec3* pos = (const Vec3*)(p + 4);
        Vec3 v, t;
        v.Set(0, 0, len);
        t.Set((float)((double)v.x + pos->x), (float)((double)v.y + pos->y), (float)((double)v.z + pos->z));
        if (F(int, r, 68) != F(int, r, 64)) {
            BYTE* e = F(BYTE*, r, 76) + F(int, r, 68) * 24;
            memcpy(e, &t, 12);
            F(int, e, 16) = 0;
            F(int, e, 20) = 0;
            F(DWORD, e, 12) = col;
            F(int, r, 68)++;
        }
        Vec3 w1, w, w2, x;
        w1.Set((float)(len * (double)half[0]), (float)(len * (double)half[1]), (float)(len * (double)half[2]));
        w.Set((float)((double)w1.x + pos->x), (float)((double)w1.y + pos->y), (float)((double)w1.z + pos->z));
        Renderer_AddVertexUV(r, 0, &w, &col, 0.5f, 1.0f);
        w2.Set((float)(len * (double)half[0]), (float)(len * (double)half[1]), (float)(len * (double)half[2]));
        Renderer_AddVertexUV(r, 0, Vec3_Diff(&x, pos, &w2), &col, 1.0f, 0);
    }
    Effect_Zero();
    return 0;
}

} // extern "C"

// === choosing the lights (FUN_4ccf30)

extern "C" void FUN_5f25c0(); KING_ALIAS("_FUN_5f25c0", "@Node_GetWorld@12")
extern "C" void Cl_qsort(); KING_ALIAS("_Cl_qsort", "_qsort")
KING_VAR(float, g_cl64c320, 64c320)
KING_VAR(float, g_cl64c9d4, 64c9d4)
KING_VAR(float, g_cl64c9d0, 64c9d0)
KING_VAR(float, g_cl64c9cc, 64c9cc)
KING_VAR(float, g_cl64c338, 64c338)
KING_VAR(float, g_cl64c450, 64c450)
KING_VAR(float, g_cl64c34c, 64c34c)
KING_VAR(float, g_cl64c9c8, 64c9c8)

// the brightness of colour c (three floats)
static double Cl_Lum(const BYTE* c)
{
    return ((double)F(float, c, 8) * g_cl64c9d0 + (double)F(float, c, 4) * g_cl64c9cc +
            (double)F(float, c, 0) * g_cl64c338) * g_cl64c450;
}

extern "C" {

// [FUN_4ccf30] lights of set s (+0 the most, +4 count, +12 the 120-byte lights) for world w: each rated (+108) by
// its kind, its colours and its fade level, a spot also by its distance from the camera in x and y (w+8472 the
// range); the best s+0 are switched on (s+8 their count), the rest off; the last failure
int __fastcall Lights_Choose(DWORD* s, void*, BYTE* w)
{
    if (!s[1]) {
        s[2] = 0;
        return 0;
    }
    for (DWORD i = 0; i < s[1]; i++) {
        BYTE* r = (BYTE*)s[3] + i * 120;
        int kind = F(int, r, 0);
        double base = kind == 2 ? g_cl64c320 : kind == 3 ? g_cl64c9d4 : g_eff64c264b;
        double pri = base + Cl_Lum(r + 4) + Cl_Lum(r + 20) + Cl_Lum(r + 36);
        if (kind == 3) {
            float pf = (float)pri;
            BYTE m[48];
            for (int k = 0; k < 4; k++)
                CallThis(FUN_4bd4d0, m + k * 12);
            memcpy(m, g_7025f0, 48);
            CallThis(FUN_5f25c0, F(BYTE*, F(BYTE*, g_view, 488), 80), m);
            Vec3 v, c;
            v.Set((float)((double)F(float, m, 36) - F(float, r, 52)), (float)((double)F(float, m, 40) - F(float, r, 56)),
                  0);
            c = v;
            double len = Vec3_Length(&c);
            pri = (g_one_f - len / F(double, w, 8472)) * g_cl64c34c + pf;
        }
        F(float, r, 108) = (float)(F(int, F(BYTE*, r, 104), 72) * (double)g_cl64c9c8 + pri);
    }
    CallC(Cl_qsort, (void*)s[3], s[1], 120, (void*)Cmp4cce30_Desc);
    s[2] = s[0] < s[1] ? s[0] : s[1];
    int hr = 0;
    for (DWORD i = 0; i < s[2]; i++) {
        int e = Light_Switch((BYTE*)s[3] + i * 120, 0, 1, 0);
        if (e < 0)
            hr = e;
    }
    for (DWORD i = s[2]; i < s[1]; i++) {
        int e = Light_Switch((BYTE*)s[3] + i * 120, 0, 0, 0);
        if (e < 0)
            hr = e;
    }
    return hr;
}

} // extern "C"

// === how much a sphere shows in the view (FUN_4ca4e0)

KING_VAR(Vec3, g_sv702650, 702650)
KING_VAR(float, g_sv671720, 671720)
KING_VAR(float, g_sv671724, 671724)
KING_VAR(float, g_sv671728, 671728)
KING_VAR(float, g_sv67171c, 67171c)
KING_VAR(float, g_sv64c938, 64c938)

// 1 - t clamped to 0..1
static double Sv_Fade(double t)
{
    if (t > g_one_f)
        t = g_one_f;
    else if (t < g_eff64c264b)
        t = g_eff64c264b;
    return g_one_f - t;
}

extern "C" {

// [FUN_4ca4e0] how much sphere p (+12 the radius) shows from the camera (the game's frame +1424): 1 when the camera is
// inside it, 64c938 when it lies outside the view cone or the depth range (671724, 67171c), else fading with its
// depth (in front by its near side from 671720 to 67171c, behind by its far side up to 671724)
double __cdecl Sphere_Visibility(const float* p)
{
    const float* m = (const float*)(g_game + 1424);
    const Vec3& a = g_ahead702620;
    const Vec3& z = g_sv702650;
    float f[3];
    f[0] = (float)((double)a.z * m[6] + (double)a.y * m[3] + (double)a.x * m[0]);
    f[1] = (float)((double)a.z * m[7] + (double)a.y * m[4] + (double)a.x * m[1]);
    f[2] = (float)((double)a.z * m[8] + (double)a.y * m[5] + (double)a.x * m[2]);
    double ox = (double)z.z * m[6] + (double)z.y * m[3] + (double)z.x * m[0] + m[9];
    float oy = (float)((double)z.z * m[7] + (double)z.y * m[4] + (double)z.x * m[1] + m[10]);
    float oz = (float)((double)z.z * m[8] + (double)z.y * m[5] + (double)z.x * m[2] + m[11]);
    Vec3 d;
    d.x = (float)(p[0] - ox);
    d.y = (float)((double)p[1] - oy);
    d.z = (float)((double)p[2] - oz);
    float len = (float)Vec3_Length(&d);
    if (!(len > g_one_f))
        return g_one_f;
    float along = (float)((double)d.z * f[2] + (double)d.y * f[1] + (double)d.x * f[0]);
    float fr = (float)((double)along + p[3]);
    if (fr < g_sv671724)
        return g_sv64c938;
    float nr = (float)((double)along - p[3]);
    if (nr > g_sv67171c)
        return g_sv64c938;
    float inv = (float)(g_one_f / (double)len);
    float s = (float)(inv * (double)p[3]);
    if (!(s < g_one_f))
        return g_one_f;
    double c = x87_cos(x87_asin(s) + g_sv671728);
    double k = along * (double)inv;
    if (!(along > g_eff64c264b))
        k = -k;
    if (!(k > c))
        return g_sv64c938;
    if (!(along > g_eff64c264b))
        return Sv_Fade(fr / (double)g_sv671724);
    return Sv_Fade(((double)nr - g_sv671720) / ((double)g_sv67171c - g_sv671720));
}

} // extern "C"

// === a series of three explosions (4dd610)

extern "C" void FUN_5dbcc0(); KING_ALIAS("_FUN_5dbcc0", "@Camera_Get@12")
extern "C" void Xs_Inverse(); KING_ALIAS("_Xs_Inverse", "_Matrix_Inverse")
KING_VAR(AsmLabel, g_xsSounds, 696704)
typedef int(__cdecl* Xs_Callback)(int id, int what, int ctx);

extern "C" {

// [$L_4dd610] step of explosion series s: at each of its three times (+32..) a smoke burst at its point (+56..,
// in frame +132) unless the callback (+180, ctx +184) refuses (2), with one of two bangs (696704 +8/+12) heard
// from the camera; once a point has gone off its callback hears 0 three steps later; past +120 it ends
int __fastcall Explosions_Step(BYTE* s)
{
    F(float, s, 124) = (float)(F(double, F(BYTE*, s, 128), 2960) + F(float, s, 124));
    for (int k = 0; k < 3; k++) {
        if (!(F(float, s, 124) > F(float, s, 32 + k * 4)))
            continue;
        const float* p = (const float*)(s + 56 + k * 12);
        int id = F(int, s, 92 + k * 4);
        if (F(int, s, 44 + k * 4)) {
            if (F(int, s, 104 + k * 4) == 3 && F(Xs_Callback, s, 180))
                F(Xs_Callback, s, 180)(id, 0, F(int, s, 184));
            F(int, s, 104 + k * 4)++;
            continue;
        }
        F(int, s, 44 + k * 4) = 1;
        Xs_Callback cb = F(Xs_Callback, s, 180);
        if (!cb || cb(id, 2, F(int, s, 184)))
            continue;
        BYTE* b = (BYTE*)operator new(156);
        if (b)
            b = SmokeBurst_Construct(b, 0, F(BYTE*, s, 128), (const Matrix*)(s + 132), (const Vec3*)p);
        F(int, b, 144) = F(int, s, 180);
        F(int, b, 148) = F(int, s, 184);
        F(int, b, 152) = F(int, s, 92 + k * 4);
        CallVirt(b, 24, 1);
        BYTE cam[48];
        float inv[12];
        CallC(Xs_Inverse, inv, CallThis<BYTE*>(FUN_5dbcc0, F(BYTE*, s, 128), cam));
        const float* m = (const float*)(s + 132);
        float w[3];
        w[0] = (float)((double)m[3] * p[1] + (double)m[6] * p[2] + (double)m[0] * p[0] + m[9]);
        w[1] = (float)((double)m[4] * p[1] + (double)m[7] * p[2] + (double)m[1] * p[0] + m[10]);
        w[2] = (float)((double)m[5] * p[1] + (double)m[8] * p[2] + (double)m[2] * p[0] + m[11]);
        int idx = rand() * (double)g_eff64c254 > g_64c258 ? 2 : 3;
        BYTE** snd = ((BYTE***)(BYTE*)g_xsSounds)[idx];
        if (!snd)
            continue;
        float l[3];
        l[0] = (float)((double)inv[6] * w[2] + (double)inv[3] * w[1] + (double)inv[0] * w[0] + inv[9]);
        l[1] = (float)((double)inv[7] * w[2] + (double)inv[4] * w[1] + (double)inv[1] * w[0] + inv[10]);
        l[2] = (float)((double)inv[8] * w[2] + (double)inv[5] * w[1] + (double)inv[2] * w[0] + inv[11]);
        double near_ = 50.0, gain = 1.0;
        Sound4dd920_Play(snd, 0, F(int, l, 0), F(int, l, 4), F(int, l, 8), ((int*)&near_)[0], ((int*)&near_)[1],
                         ((int*)&gain)[0], ((int*)&gain)[1]);
    }
    if (F(float, s, 124) > F(float, s, 120))
        return CallVirt<int>(s, 28, 1);
    return CallThis<int>(FUN_5d32f0, s, 1);
}

} // extern "C"

// === the smoke cloud (FUN_4da4e0)

KING_VAR(AsmLabel, s_fx672c6c, 672c6c)
KING_VAR(float, g_sc672c48, 672c48)

extern "C" {

// [FUN_4da4e0] constructor (owner, position p) of a smoke cloud: twenty puffs from p spread sideways (along
// 702620 and 702640) and up (along 702630)
BYTE* __fastcall SmokeCloud_Construct(BYTE* s, void*, BYTE* owner, const Vec3* p)
{
    CallThis(FUN_5d3110, s, (const char*)s_fx672c6c);
    for (int i = 44; i < 80; i += 4)
        F(int, s, i) = 0;
    F(void*, s, 0) = &vt_64d49c;
    F(BYTE*, s, 40) = owner;
    F(int, s, 36) = 0;
    F(float, s, 32) = g_sc672c48;
    memcpy(s + 44, p, 12);
    memcpy(s + 68, p, 12);
    F(float, s, 88) = 4.0f;
    F(float, s, 92) = 0.5f;
    F(int, s, 84) = 0;
    const Vec3& up = g_axis702630e;
    F(float, s, 56) = up.x + up.x;
    F(float, s, 60) = up.y + up.y;
    F(float, s, 80) = 0.8f;
    F(float, s, 64) = up.z + up.z;
    for (int k = 0; k < 20; k++) {
        double a = Sb_RandDiff();
        float a0 = (float)(g_ahead702620.x * a), a1 = (float)(g_ahead702620.y * a);
        float p1[3] = {(float)(a0 * (double)g_sb64c408), (float)(a1 * (double)g_sb64c408),
                       (float)(a * g_sb702628 * g_sb64c408)};
        double b = Sb_RandDiff();
        float b0 = (float)(g_up702640.x * b), b1 = (float)(g_up702640.y * b);
        float p2[3] = {(float)(b0 * (double)g_sb64c408), (float)(b1 * (double)g_sb64c408),
                       (float)(b * g_sb702648 * g_sb64c408)};
        double c = rand() * (double)g_sb64d498 + g_sb64c40c;
        float c0 = (float)(c * up.x), c1 = (float)(c * up.y);
        float t0 = (float)((double)c0 + p2[0]), t1 = (float)((double)p2[1] + c1);
        double t2 = c * up.z + p2[2];
        Vec3 v, w;
        v.Set((float)((double)t0 + p1[0]), (float)((double)p1[1] + t1), (float)(p1[2] + t2));
        w = v;
        Fx_SmokePuffs((int)(s + 68), (int)&w);
    }
    CallThis(FUN_5d3570, F(BYTE*, s, 40) + 2992, s);
    return s;
}

} // extern "C"

// === the height of a terrain object under a point (FUN_4be2c0)

extern "C" void FUN_4be0e0(); KING_ALIAS("_FUN_4be0e0", "_Point_InTriangle")    // whether (x, y) lies in a triangle (x, y, nine floats)
extern "C" void FUN_5ab7c0(); KING_ALIAS("_FUN_5ab7c0", "_Plane_Transformed")
extern "C" void FUN_5ab8f0(); KING_ALIAS("_FUN_5ab8f0", "@Plane_Transform@12")
KING_VAR(double, g_oh64c3f8, 64c3f8)
KING_VAR(float, g_oh64c1e0, 64c1e0)

extern "C" {

// [FUN_4be2c0] the height at (x, y) of terrain object o (frame +36): of its plane (kind 5, +32) or of the triangle of
// its last polygon (kind 6, +28 polygons of 48 bytes at +32: +2 the count, +4 the points, +32 the plane) under the
// point, when the plane is not steep; else 64c1e0
double __cdecl Object_HeightAt(float x, float y, BYTE* o)
{
    const float* m = (const float*)(o + 36);
    if (F(int, o, 24) == 5) {
        float pl[4];
        memcpy(pl, F(BYTE*, o, 32), 16);
        CallThis(FUN_5ab8f0, pl, m);
        double c = pl[2] < 0 ? -(double)pl[2] : pl[2];
        if (c > g_oh64c3f8)
            return -(((double)pl[0] * x + (double)pl[1] * y + pl[3]) / pl[2]);
    }
    if (F(int, o, 24) != 6)
        return g_oh64c1e0;
    BYTE* poly = F(BYTE*, o, 32) + F(int, o, 28) * 48 - 48;
    unsigned n = F(WORD, poly, 2);
    if (n < 3)
        return g_oh64c1e0;
    for (unsigned k = 1; k < n - 1; k++) {
        const float* v = F(const float*, poly, 4);
        float t[9];
        t[0] = (float)((double)m[3] * v[1] + (double)m[6] * v[2] + (double)m[0] * v[0] + m[9]);
        t[1] = (float)((double)m[4] * v[1] + (double)m[1] * v[0] + (double)m[7] * v[2] + m[10]);
        t[2] = (float)((double)m[5] * v[1] + (double)m[2] * v[0] + (double)m[8] * v[2] + m[11]);
        const float* b = F(const float*, poly, 4) + k * 3;
        t[3] = (float)((double)m[6] * b[2] + (double)m[3] * b[1] + (double)m[0] * b[0] + m[9]);
        t[4] = (float)((double)m[7] * b[2] + (double)m[4] * b[1] + (double)m[1] * b[0] + m[10]);
        t[5] = (float)((double)m[8] * b[2] + (double)m[5] * b[1] + (double)m[2] * b[0] + m[11]);
        const float* c = F(const float*, poly, 4) + k * 3 + 3;
        t[6] = (float)((double)m[6] * c[2] + (double)m[3] * c[1] + (double)c[0] * m[0] + m[9]);
        t[7] = (float)((double)m[7] * c[2] + (double)m[1] * c[0] + (double)m[4] * c[1] + m[10]);
        t[8] = (float)((double)m[8] * c[2] + (double)m[2] * c[0] + (double)m[5] * c[1] + m[11]);
        if (!CallC<int>(FUN_4be0e0, x, y, t))
            continue;
        float pl[4];
        CallC(FUN_5ab7c0, pl, poly + 32, m);
        double cz = pl[2] < 0 ? -(double)pl[2] : pl[2];
        if (cz > g_oh64c3f8)
            return -(((double)pl[0] * x + (double)pl[1] * y + pl[3]) / pl[2]);
    }
    return g_oh64c1e0;
}

} // extern "C"

// === the rain and snow effects' step (4c41c0)

extern "C" void Rn_Selected(); KING_ALIAS("_Rn_Selected", "_Switch_Selected")
extern "C" void Rn_Wave(); KING_ALIAS("_Rn_Wave", "@Wave_Value@16")
extern "C" void Rn_GetMaterial(); KING_ALIAS("_Rn_GetMaterial", "@MarineTask_GetMaterial@8")
KING_VAR(double, g_rn64b7a8, 64b7a8)
KING_VAR(float, g_rn64c1f4, 64c1f4)
KING_VAR(float, g_rn64c76c, 64c76c)

// all four of s's effects (+40..+52) switched on or off
static void Rn_SwitchAll(BYTE* s, int on, int last)
{
    for (int o = 40; o <= last; o += 4)
        CallVirt(F(BYTE*, s, o), 24, on);
}

extern "C" {

// [$L_4c41c0] step of the precipitation effects s (while switch +68 is on): drops (+40), the two splash kinds (+44,
// +48: the one by whether +44's task has a material) and snow (+52) on with the weather's wet (7025b8 above 64b7a8)
// and the season (6716f8 rain, 6716fc snow), sized by 64c1f4; each steps (vtbl+0 the step of +64) and draws into the
// renderers +32 and +36 unless paused (6f33c0)
int __fastcall Precip_Step(BYTE* s)
{
    if (!F(int, s, 68) || CallC<int>(Rn_Selected, F(int, s, 68))) {
        if (CallThis<double>(Rn_Wave, g_fx7025b8, 0.0) < g_rn64b7a8)
            Rn_SwitchAll(s, 0, 52);
        else {
            F(float, F(BYTE*, s, 40), 12) = (float)(g_rn64c1f4 * (double)g_rn64c76c);
            F(float, F(BYTE*, s, 44), 12) = (float)(g_rn64c1f4 * (double)g_eff64c350);
            F(float, F(BYTE*, s, 48), 12) = (float)(g_rn64c1f4 * (double)g_eff64c350);
            Rn_SwitchAll(s, 1, 52);
        }
        if (!g_fi6716fc)
            CallVirt(F(BYTE*, s, 52), 24, 0);
        if (!g_fi6716f8)
            Rn_SwitchAll(s, 0, 48);
        if (!g_fx6f33c0)
            CallVirt(F(BYTE*, s, 40), 0, (float)F(double, F(BYTE*, s, 64), 2960));
        BYTE* sp = CallThis<int>(Rn_GetMaterial, F(BYTE*, F(BYTE*, s, 44), 68)) ? F(BYTE*, s, 44) : F(BYTE*, s, 48);
        if (!g_fx6f33c0)
            CallVirt(sp, 0, (float)F(double, F(BYTE*, s, 64), 2960));
        Renderer_ResetCount(F(BYTE*, s, 32), 0);
        if (!g_fx6f33c0)
            CallVirt(F(BYTE*, s, 40), 20, F(BYTE*, s, 32));
        sp = CallThis<int>(Rn_GetMaterial, F(BYTE*, F(BYTE*, s, 44), 68)) ? F(BYTE*, s, 44) : F(BYTE*, s, 48);
        if (!g_fx6f33c0)
            CallVirt(sp, 20, F(BYTE*, s, 32));
        Effect_Zero();
        if (!g_fx6f33c0)
            CallVirt(F(BYTE*, s, 52), 0, (float)F(double, F(BYTE*, s, 64), 2960));
        Renderer_ResetCount(F(BYTE*, s, 36), 0);
        if (!g_fx6f33c0)
            CallVirt(F(BYTE*, s, 52), 28, F(BYTE*, s, 36));
        Effect_Zero();
    }
    return CallThis<int>(FUN_5d32f0, s, 1);
}

} // extern "C"

// === trails: following a wheel's track (FUN_4bcd00)

extern "C" void FUN_4bd850(); KING_ALIAS("_FUN_4bd850", "@Trail_Emit@16")
KING_VAR(float, g_tr64c1e8, 64c1e8)
KING_VAR(float, g_tr695c54, 695c54)
KING_VAR(char, s_tr67270c, 67270c)
extern "C" void Tr_NodeConstruct(); KING_ALIAS("_Tr_NodeConstruct", "@Node_Construct@16")
extern "C" void Tr_Self(); KING_ALIAS("_Tr_Self", "@Fx_Self@8")

// a trail record (240 bytes): dir +0, the previous dir +12, the points +24 and +36, their times +48 and +52 and
// hits +56 and +148
static double Tr_Dot(BYTE* r)
{
    return (double)F(float, r, 20) * F(float, r, 8) + (double)F(float, r, 16) * F(float, r, 4)
        + (double)F(float, r, 12) * F(float, r, 0);
}

// the direction p1 - p0 normalized into out
static void Tr_Direction(BYTE* r, BYTE* out)
{
    Vec3 d = {F(float, r, 36) - F(float, r, 24), F(float, r, 40) - F(float, r, 28), F(float, r, 44) - F(float, r, 32)};
    Vec3 t;
    F(Vec3, out, 0) = *Vec3_Normalized(&t, &d);
}

extern "C" {

// [FUN_4bcd00] trail i follows the hit h (its point +52) at time t: a trail not updated within the limit
// (+16 -> +2992) restarts (making the frame node of slot +84 % 10 with two 1201-entry buffers when missing);
// state 0 takes the first point, 1 the second, 2 shifts the newest point in; segments turning more than the
// angle 695c54 restart the trail, others are drawn by FUN_4bd850 (i, a4)
void __fastcall Trail_Follow(BYTE* self, void*, BYTE* h, float t, int i, int a4)
{
    if (i > F(int, self, 20) || i < 0)
        return;
    int lim = F(int, F(BYTE*, self, 16), 2992);
    int* state = F(int*, self, 0);
    if (F(int*, self, 4)[i] + 1 < lim) {
        state[i] = 0;
        unsigned slot = F(unsigned, self, 84) % 10;
        if (!F(BYTE*, self, 40 + slot * 4)) {
            BYTE* n = (BYTE*)operator new(96);
            if (n) {
                BYTE* list = F(BYTE*, self, 36);
                CallThis(Tr_NodeConstruct, n, &s_tr67270c, 24);
                F(int, n, 72) = 0;
                F(int, n, 56) = 0;
                F(int, n, 48) = 0;
                F(int, n, 52) = 0;
                F(void*, n, 0) = g_vtbl64c3a4;
                CallVirt(list, 20, 0, n);
                F(BYTE, n, 40) &= ~1;
                memset(n + 20, 0, 12);
                F(int, n, 32) = 0;
                for (int k = 0; k < 2; k++) {
                    BYTE* b = (BYTE*)operator new(38432);
                    if (b)
                        CallStd(FUN_4011e0, b, 32, 1201, Tr_Self);
                    F(BYTE*, n, 76 + k * 4) = b;
                    F(int, n, 88 + k * 4) = 0;
                }
            }
            F(BYTE*, self, 40 + slot * 4) = n;
        }
    }
    BYTE* r = F(BYTE*, self, 8) + i * 240;
    switch (state[i]) {
    case 0: {
        state[i] = 1;
        F(Vec3Bits, r, 24) = F(Vec3Bits, h, 52);
        BYTE hit[92];
        memset(hit, 0, 24);
        CallStd(FUN_4011e0, hit + 36, 12, 4, FUN_4bd4d0);
        memcpy(hit + 36, g_7025f0, 48);
        if (h)
            Hit_Fill(hit, h);
        memcpy(r + 56, hit, 92);
        F(float, r, 48) = t;
        break;
    }
    case 1: {
        Vec3 d = {F(float, r, 24) - F(float, h, 52), F(float, r, 28) - F(float, h, 56), F(float, r, 32) - F(float, h, 60)};
        if (Vec3_Length(&d) < g_tr64c1e8)
            break;
        F(Vec3Bits, r, 36) = F(Vec3Bits, h, 52);
        Tr_Direction(r, r);
        F(Vec3Bits, r, 12) = F(Vec3Bits, r, 0);
        BYTE hit[92];
        HitRec_Construct(hit, 0, h);
        memcpy(r + 148, hit, 92);
        F(float, r, 52) = t;
        if (g_tr695c54 > Tr_Dot(r))
            state[i] = 0;
        else {
            CallThis(FUN_4bd850, self, i, a4);
            state[i] = 2;
        }
        break;
    }
    case 2: {
        F(Vec3Bits, r, 24) = F(Vec3Bits, h, 52);
        if (Vec3_Length((const Vec3*)(r + 24)) < g_tr64c1e8)
            break;
        F(Vec3Bits, r, 24) = F(Vec3Bits, r, 36);
        F(Vec3Bits, r, 0) = F(Vec3Bits, r, 12);
        memcpy(r + 56, r + 148, 92);
        F(float, r, 48) = F(float, r, 52);
        F(Vec3Bits, r, 36) = F(Vec3Bits, h, 52);
        Tr_Direction(r, r + 12);
        BYTE hit[92];
        HitRec_Construct(hit, 0, h);
        memcpy(r + 148, hit, 92);
        F(float, r, 52) = t;
        if (g_tr695c54 > Tr_Dot(r))
            state[i] = 0;
        else
            CallThis(FUN_4bd850, self, i, a4);
        break;
    }
    }
    F(int*, self, 4)[i] = lim;
}

} // extern "C"

// === trails: emitting a segment (FUN_4bd850)

KING_VAR(float, g_tr64c1e0, 64c1e0)
KING_VAR(float, g_tr64c1dc, 64c1dc)

static void Tr_Vertex(BYTE* out, const Vec3* p, const Vec3* n, float u, float v)
{
    Fx_Set2Vec(out, 0, p, n, *(int*)&u, *(int*)&v);
}

extern "C" {

// [FUN_4bd850] emits the quad of trail i's last segment into buffer k of the frame node (slot +84 % 10): the
// corners are the points +-half the widths (times * 64c258) across the hit normals, lifted to the ground
// (the hits' heights when nothing is under them) plus 64c1dc; u across, v along the length over the time
void __fastcall Trail_Emit(BYTE* self, void*, int i, int k)
{
    BYTE* r = F(BYTE*, self, 8) + i * 240;
    Vec3 c[4];
    for (int j = 0; j < 4; j++)
        Vec3_ZeroThis(&c[j], 0);
    const Vec3* p0 = (const Vec3*)(r + 24);
    const Vec3* p1 = (const Vec3*)(r + 36);
    Vec3 side0, side1, t, s;
    side0.x = F(float, r, 72) * F(float, r, 8) - F(float, r, 76) * F(float, r, 4);
    side0.y = F(float, r, 0) * F(float, r, 76) - F(float, r, 68) * F(float, r, 8);
    side0.z = F(float, r, 68) * F(float, r, 4) - F(float, r, 0) * F(float, r, 72);
    Vec3_Cross(&side1, (const Vec3*)(r + 160), (const Vec3*)(r + 12));
    float len = (float)(Vec3_Length(Vec3_Sub(&t, p1, p0)) / F(float, r, 52));
    float w0 = F(float, r, 48) * g_64c258;
    float w1 = F(float, r, 52) * g_64c258;
    c[0] = *Vec3_Sub(&s, p0, Vec3_Scaled(&t, &side0, w0));
    c[1] = *Vec3_Add(&t, p0, Vec3_Scaled(&s, &side0, w0));
    c[2] = *Vec3_Sub(&t, p1, Vec3_Scaled(&s, &side1, w1));
    c[3] = *Vec3_Add(&t, p1, Vec3_Scaled(&s, &side1, w1));
    Vec3 up = {0.0f, 0.0f, 1.0f};
    BYTE v[4][32];
    Tr_Vertex(v[0], &c[0], &up, 0.0f, 0.0f);
    Tr_Vertex(v[1], &c[1], &up, 1.0f, 0.0f);
    Tr_Vertex(v[2], &c[2], &up, 0.0f, len);
    Tr_Vertex(v[3], &c[3], &up, 1.0f, len);
    for (int j = 0; j < 4; j++)
        F(float, v[j], 8) = (float)Fx_SegmentDistance(F(void*, v[j], 0), F(void*, v[j], 4), r + 148, r + 56);
    for (int j = 0; j < 2; j++)
        if (F(float, v[j], 8) == g_tr64c1e0)
            F(float, v[j], 8) = F(float, r, 64);
    for (int j = 2; j < 4; j++)
        if (F(float, v[j], 8) == g_tr64c1e0)
            F(float, v[j], 8) = (F(float, r, 156) + F(float, r, 64)) * g_64c258;
    for (int j = 0; j < 4; j++)
        F(float, v[j], 8) = g_tr64c1dc + F(float, v[j], 8);
    BYTE* n = F(BYTE*, self, 40 + F(unsigned, self, 84) % 10 * 4);
    BYTE* b = F(BYTE*, n, 76 + k * 4);
    unsigned at = F(unsigned, n, 88 + k * 4) % 1200;
    static const int order[6] = {1, 3, 2, 2, 1, 0};
    for (int j = 0; j < 6; j++)
        memcpy(b + (at + j) * 32, v[order[j]], 32);
    F(int, n, 88 + k * 4) += 6;
}

} // extern "C"

KING_VAR(float, g_tri64c3f0, 64c3f0)
extern "C" Vec3* __fastcall Vec3_SetThis(Vec3* self, void*, float x, float y, float z);
// [FUN_4be0e0] whether (x, y) lies inside triangle t (three points of 12 bytes, x and y used): the three edge
// cross products at most 64c3f0
extern "C" int __cdecl Point_InTriangle(float x, float y, const float* t)
{
    float ax = t[0], ay = t[1], bx = t[3], by = t[4], cx = t[6], cy = t[7];
    float z1 = (float)(((double)by - ay) * ((double)x - ax) - ((double)y - ay) * ((double)bx - ax));
    float ex = (float)((double)cx - bx), ey = (float)((double)cy - by);
    Vec3 d;
    Vec3_SetThis(&d, 0, (float)((double)x - bx), (float)((double)y - by), 0.0f);
    float z2 = (float)((double)d.x * ey - (double)d.y * ex);
    Vec3 a = {ax, ay, 0}, c = {cx, cy, 0}, p = {x, y, 0}, ac, pc, r;
    Vec3_Diff(&ac, &a, &c);
    Vec3_Diff(&pc, &p, &c);
    Vec3_Cross2(&r, &pc, &ac);
    return z1 <= g_tri64c3f0 && z2 <= g_tri64c3f0 && r.z <= g_tri64c3f0;
}

// === splitting a triangle in four (FUN_4c7850)

// the midpoint of the 32-byte vertices p and q (floats halved, the colour +12 per channel)
static void Ts_Mid(float* out, const float* p, const float* q)
{
    for (int k = 0; k < 8; k++) {
        if (k == 3) {
            DWORD a = ((const DWORD*)p)[3], b = ((const DWORD*)q)[3], c = 0;
            for (int sh = 24; sh >= 0; sh -= 8)
                c = c << 8 | (WORD)(int)((__int64)(((a >> sh) & 255) + ((b >> sh) & 255)) * g_64c258);
            ((DWORD*)out)[3] = c;
        } else
            out[k] = (p[k] + q[k]) * g_64c258;
    }
}

extern "C" {

// [FUN_4c7850] splits triangle a, b, c into four: the six vertices (the corners and the midpoints ab, bc, ca) at
// verts[base..], twelve indices from idx + 2 * base; *nv = 6, *ni = 12
void __cdecl Triangle_Split4(BYTE* verts, int* nv, WORD* idx, int* ni, const float* a, const float* b,
    const float* c, int base)
{
    float ab[8], bc[8], ca[8];
    Ts_Mid(ab, a, b);
    Ts_Mid(bc, b, c);
    Ts_Mid(ca, a, c);
    float* v = (float*)(verts + base * 32);
    memcpy(v, a, 32);
    memcpy(v + 8, b, 32);
    memcpy(v + 16, c, 32);
    memcpy(v + 24, ab, 32);
    memcpy(v + 32, bc, 32);
    memcpy(v + 40, ca, 32);
    static const int tri[12] = {0, 3, 5, 3, 1, 4, 4, 2, 5, 5, 3, 4};
    WORD* o = idx + base * 2;
    for (int i = 0; i < 12; i++)
        o[i] = (WORD)(base + tri[i]);
    *nv = 6;
    *ni = 12;
}

} // extern "C"

// === a car's world events (a handler at $L_4ce300)

extern "C" int __cdecl Player_GetState(BYTE* player, void* dest);
extern "C" void Ce_CarId(); KING_ALIAS("_Ce_CarId", "_Player_CarId")
extern "C" void Ce_Matrix1(); KING_ALIAS("_Ce_Matrix1", "@SceneObj_Matrix1@12")
KING_VAR(BYTE*, g_ce696338, 696338)
KING_VAR(float, g_ce64c250, 64c250)
KING_VAR(float, g_ce64c264, 64c264)
KING_VAR(double, g_ce64b678, 64b678)

extern "C" {

// [$L_4ce300] handles world event m for car c: 3264 runs Effect_Call446c80; 16 does when the car is below the
// level *m+12 (one lower for a player in state +296); 29 (with the traffic lights 696338, not for a parked kind 5
// car with +21924) records the light state seen (from the light's phase +36, flipped by the event's code m+4) and
// its side of the plane m+12 (+100 or the distance signed by the heading) in the car's new record +21872, the
// old one moving to +21876
void __cdecl Car_WorldEvent(const int* m, BYTE* c)
{
    if (m[0] == 3264) {
        Effect_Call446c80(c);
        return;
    }
    if (m[0] == 16) {
        float lim = *(const float*)m[3];
        DWORD st[76];
        for (int i = 0; i < 19; i++) {
            st[i * 4] = 0;
            st[i * 4 + 2] = 0;
            st[i * 4 + 3] = 0x3FF00000;
        }
        int id;
        CallC(Ce_CarId, &id);
        Player_GetState((BYTE*)id, st);
        if (st[74])
            lim -= g_ce64c250;
        BYTE t[48];
        if (F(float, CallThis<BYTE*>(Ce_Matrix1, c + 16, t), 44) < lim)
            Effect_Call446c80(c);
        return;
    }
    if (m[0] != 29 || !g_ce696338)
        return;
    BYTE info[192];
    CallC(FUN_43fc40, F(int, c, 20636), info);
    if (F(int, info, 16) == 5 && F(int, c, 21924))
        return;
    int k = m[1];
    int flag = k % 10 ? F(int, g_ce696338, 36) ^ 1 : F(int, g_ce696338, 36);
    switch (k / 10) {
    case 1:
    case 3:
        flag ^= 1;
        break;
    case 4:
        flag = 1;
        break;
    }
    float* p = (float*)operator new(4);
    float side = 100.0f;
    if (const float* pl = (const float*)m[3]) {
        BYTE t[48];
        const float* pos = (const float*)(CallThis<BYTE*>(Ce_Matrix1, c + 16, t) + 36);
        float x = pos[0], y = pos[1], z = pos[2];
        side = (float)((double)z * pl[2] + (double)y * pl[1] + (double)x * pl[0] + pl[3]);
        bool ahead = false;
        if (side < g_ce64c264) {
            BYTE t2[48];
            const float* M = (const float*)CallThis<BYTE*>(Ce_Matrix1, c + 16, t2);
            const Vec3& v = g_ahead702620;
            double a = (double)v.z * M[6] + (double)v.y * M[3] + (double)v.x * M[0];
            double b = (double)v.z * M[7] + (double)v.y * M[4] + (double)v.x * M[1];
            double cc = (double)v.z * M[8] + (double)v.y * M[5] + (double)v.x * M[2];
            ahead = a * pl[0] + b * pl[1] + cc * pl[2] > g_ce64b678;
        }
        side = ahead ? (float)x87_fabs(side) : (float)-x87_fabs(side);
    }
    p[0] = side;
    if (BYTE* old = F(BYTE*, c, 21876)) {
        if (F(void*, old, 12))
            operator delete(F(void*, old, 12));
        operator delete(old);
    }
    F(BYTE*, c, 21876) = F(BYTE*, c, 21872);
    BYTE* r = (BYTE*)operator new(16);
    if (r) {
        F(int, r, 0) = m[0];
        F(int, r, 4) = flag;
        F(int, r, 8) = 1;
        float* q = (float*)operator new(4);
        F(float*, r, 12) = q;
        memcpy(q, p, F(int, r, 8) * 4);
    }
    F(BYTE*, c, 21872) = r;
    if (p)
        operator delete(p);
}

} // extern "C"

// === launching a car along its road (FUN_4cf080)

extern "C" void Lc_Get800(); KING_ALIAS("_Lc_Get800", "@SceneObj_Get800@8")
extern "C" void Lc_SetInMode(); KING_ALIAS("_Lc_SetInMode", "@Car_SetInMode@12")
extern "C" void Lc_Mode3976(); KING_ALIAS("_Lc_Mode3976", "@Mode3976_Set@12")
KING_VAR(double, g_lc64ca88, 64ca88)
KING_VAR(Vec3, g_lc702650, 702650)

extern "C" {

// [FUN_4cf080] sends car c off at its speed (+20964; with a player p its traffic speed, capped to 25 above
// 64ca88) along its road's direction at p's position (forward without one), marking p (flag 1 into *flags when
// asked); the body (+16) gets the velocity and no spin, the car mode 1 and the body mode 3
void __cdecl Car_Launch(BYTE* c, int p, int* flags, int mark)
{
    BYTE* b = c + 16;
    BYTE t[48];
    CallThis(FUN_57de90, b, t);
    CallThis(Lc_Get800, b);
    Vec3 dir = g_ahead702620;
    double speed = F(double, c, 20964);
    if (p != g_noVehicle68b990) {
        BYTE info[192];
        CallC(FUN_43fc40, p, info);
        speed = F(double, info, 160);
        Bytes32 pos;
        CallC(FUN_437520, &pos, p);
        Bytes32 tan;
        dir = *CallC<Vec3*>(FUN_439320, &tan, pos);
        if (mark) {
            *flags |= 1;
            CallC(FUN_43f690, p, *flags);
        }
    }
    if (speed > g_lc64ca88)
        speed = 25.0;
    Vec3 n;
    const Vec3* u = Vec3_Normalized(&n, &dir);
    Vec3 v;
    v.x = (float)(speed * u->x);
    v.y = (float)(speed * u->y);
    v.z = (float)(speed * u->z);
    CallVirt(b, 144, &v, 0);
    CallVirt(b, 148, &g_lc702650, 0);
    CallThis(Lc_SetInMode, c, 1);
    CallThis(Lc_Mode3976, b, 3);
}

} // extern "C"

// === the effect switches from two settings (FUN_4b91a0)

KING_VAR(float, g_wq696030, 696030)
KING_VAR(float, g_wq696034, 696034)
KING_VAR(float, g_wq696038, 696038)
KING_VAR(float, g_wq69603c, 69603c)
KING_VAR(float, g_wq696040, 696040)
KING_VAR(float, g_wq696044, 696044)
KING_VAR(float, g_wq696048, 696048)
KING_VAR(float, g_wq69604c, 69604c)
KING_VAR(float, g_wq696050, 696050)
KING_VAR(float, g_wq696054, 696054)
KING_VAR(float, g_wq696058, 696058)
KING_VAR(float, g_wq69605c, 69605c)
KING_VAR(float, g_wq696060, 696060)
KING_VAR(float, g_wq696064, 696064)
KING_VAR(float, g_wq696068, 696068)
KING_VAR(float, g_wq69606c, 69606c)
KING_VAR(float, g_wq696070, 696070)
KING_VAR(float, g_wq696074, 696074)
KING_VAR(float, g_wq696078, 696078)
KING_VAR(float, g_wq69607c, 69607c)
KING_VAR(float, g_wq696080, 696080)
KING_VAR(float, g_wq696084, 696084)
KING_VAR(float, g_wq696088, 696088)
KING_VAR(float, g_wk64c258, 64c258)
KING_VAR(float, g_wk64c25c, 64c25c)
KING_VAR(float, g_wk64c260, 64c260)
KING_VAR(float, g_wk64c264, 64c264)
KING_VAR(BYTE*, g_wq6cec70, 6cec70)
KING_VAR(float, g_wk64c268, 64c268)

static float Wq_Flag(bool on)
{
    DWORD v = on ? 0x3F800000 : 0;
    return *(float*)&v;
}

extern "C" {

// [FUN_4b91a0] the effect switches 696030.. from the two settings +420 and +422 of 6cec70 (scaled by 64c268): each
// on (1.0) when both lie above its thresholds; some take the scaled values
void __cdecl Effects_FromSettings()
{
    double a = F(short, g_wq6cec70, 420) * (double)g_wk64c268;
    double b = F(short, g_wq6cec70, 422) * (double)g_wk64c268;
    g_wq696030 = Wq_Flag(a > g_wk64c264 && b > g_wk64c264);
    g_wq69604c = Wq_Flag(a > g_wk64c264 && b > g_wk64c264);
    g_wq696068 = Wq_Flag(a > g_wk64c264 && b > g_wk64c264);
    g_wq69607c = Wq_Flag(a > g_wk64c260 && b > g_wk64c25c);
    g_wq696038 = Wq_Flag(a > g_wk64c260 && b > g_wk64c25c);
    g_wq696048 = (float)a;
    g_wq696034 = Wq_Flag(a > g_wk64c264 && b > g_wk64c260);
    g_wq696054 = Wq_Flag(a > g_wk64c264 && b > g_wk64c260);
    g_wq69603c = Wq_Flag(a > g_wk64c260 && b > g_wk64c25c);
    g_wq69606c = (float)b;
    g_wq696088 = Wq_Flag(a > g_wk64c260 && b > g_wk64c258);
    g_wq696050 = Wq_Flag(a > g_wk64c260 && b > g_wk64c25c);
    g_wq696084 = Wq_Flag(a > g_wk64c260 && b > g_wk64c258);
    g_wq696060 = Wq_Flag(a > g_wk64c258 && b > g_wk64c258);
    g_wq696080 = Wq_Flag(a > g_wk64c258 && b > g_wk64c258);
    g_wq696044 = Wq_Flag(a > g_wk64c258 && b > g_wk64c260);
    g_wq696064 = Wq_Flag(a > g_wk64c258 && b > g_wk64c258);
    g_wq696070 = Wq_Flag(a > g_wk64c260 && b > g_wk64c260);
    g_wq696040 = Wq_Flag(a > g_wk64c258 && b > g_wk64c258);
    g_wq696074 = (float)a;
    g_wq69605c = (float)a;
    g_wq696078 = (float)a;
    *(DWORD*)&g_wq696058 = 0x3F800000;
}

} // extern "C"

// === a fire source's step (a method at $L_4d98c0)

extern "C" int __cdecl rand(void);
extern "C" void Fs_RotZ(); KING_ALIAS("_Fs_RotZ", "_Matrix_RotationZ")
extern "C" void Fs_Fire(); KING_ALIAS("_Fs_Fire", "@Fire_Construct@24")
extern "C" void Fs_Post(); KING_ALIAS("_Fs_Post", "@Queue_PostEvent@12")
KING_VAR(float, g_fs64c48c, 64c48c)
KING_VAR(float, g_fs64c254, 64c254)
KING_VAR(float, g_fs64d3f4, 64d3f4)
KING_VAR(float, g_fs702640, 702640)
KING_VAR(float, g_fs702644, 702644)
KING_VAR(float, g_fs702648, 702648)
KING_VAR(float, g_fs702630, 702630)
KING_VAR(float, g_fs702634, 702634)
KING_VAR(float, g_fs702638, 702638)
KING_VAR(float, g_fs64c320, 64c320)
KING_VAR(float, g_fs64d400, 64d400)
KING_VAR(float, g_fs64c304, 64c304)
KING_VAR(float, g_fs64c338, 64c338)

extern "C" {

// [$L_4d98c0] fire source s (+52 its age) throws a flame (108 bytes) in a random direction around 702640 at a
// random spread from its position (+36), moving up (702630 by 64c320), living 64c304 plus a random part; after
// 64c338 the source ends (vtbl+28 (1)), else it is queued again
int __fastcall FireSource_Step(BYTE* s)
{
    F(float, s, 52) = (float)(F(double, F(BYTE*, s, 32), 2960) + F(float, s, 52));
    float r1 = rand() * g_fs64c48c;
    double r = rand() * g_fs64c254;
    float ang = (float)(g_fs64d3f4 * r * 2);
    float m[12];
    const float* R = CallC<const float*>(Fs_RotZ, m, *(DWORD*)&ang);
    double dx = (double)g_fs702648 * R[6] + (double)g_fs702644 * R[3] + (double)g_fs702640 * R[0] + R[9];
    double dy = (double)g_fs702648 * R[7] + (double)g_fs702644 * R[4] + (double)g_fs702640 * R[1] + R[10];
    double dz = (double)g_fs702648 * R[8] + (double)g_fs702644 * R[5] + (double)g_fs702640 * R[2] + R[11];
    double uz = (double)g_fs702638 * g_fs64c320;
    float uy = g_fs702634 * g_fs64c320, ux = g_fs702630 * g_fs64c320;
    float v[3] = {(float)(ux + dx), (float)(uy + dy), (float)(uz + dz)};
    float ox = (float)(dx * r1), oy = (float)(dy * r1), oz = (float)(dz * r1);
    BYTE* f = (BYTE*)operator new(108);
    if (f) {
        float p[3] = {ox + F(float, s, 36), oy + F(float, s, 40), oz + F(float, s, 44)};
        float life = (float)(rand() * g_fs64d400 + g_fs64c304);
        f = CallThis<BYTE*>(Fs_Fire, f, F(BYTE*, s, 32), p, v, *(DWORD*)&life);
    }
    CallVirt(f, 24, 1);
    if (F(float, s, 52) > g_fs64c338)
        return CallVirt<int>(s, 28, 1);
    return CallThis<int>(Fs_Post, s, 1);
}

} // extern "C"

// === light shafts at a list of points (FUN_4bffb0)

KING_VAR(float, g_ls702620, 702620)
KING_VAR(float, g_ls702624, 702624)
KING_VAR(float, g_ls702628, 702628)
KING_VAR(float, g_ls702630, 702630)
KING_VAR(float, g_ls702634, 702634)
KING_VAR(float, g_ls702638, 702638)
KING_VAR(float, g_ls64c304, 64c304)
KING_VAR(float, g_ls64c260, 64c260)
extern "C" void Ls_RotZ(); KING_ALIAS("_Ls_RotZ", "_Matrix_RotationZ")

extern "C" {

// [FUN_4bffb0] adds to renderer r a three-vertex shaft (height +8, colour +96) for every point of s's list (+128)
// in front of the camera (+136: its position and heading) beyond +64, widened with the distance
int __fastcall Shafts_Draw(BYTE* s, void*, BYTE* r)
{
    BYTE* cam = F(BYTE*, s, 136);
    float px = (float)F(double, cam, 2904), py = (float)F(double, cam, 2912), pz = (float)F(double, cam, 2920);
    float ang = (float)F(double, cam, 2928);
    float mb[12];
    const float* R = CallC<const float*>(Ls_RotZ, mb, *(DWORD*)&ang);
    Vec3 f, side, a, b, c, d, e, v1, v2, v3, t1, t2, t3;
    Vec3_SetThis(&f, 0,
        (float)((double)g_ls702628 * R[6] + (double)g_ls702624 * R[3] + (double)g_ls702620 * R[0] + R[9]),
        (float)((double)g_ls702628 * R[7] + (double)g_ls702624 * R[4] + (double)g_ls702620 * R[1] + R[10]),
        (float)((double)g_ls702628 * R[8] + (double)g_ls702624 * R[5] + (double)g_ls702620 * R[2] + R[11]));
    Vec3_SetThis(&side, 0, (float)((double)f.z * g_ls702634 - (double)f.y * g_ls702638),
        (float)((double)f.x * g_ls702638 - (double)f.z * g_ls702630),
        (float)((double)f.y * g_ls702630 - (double)f.x * g_ls702634));
    float dist = (float)-((double)py * f.y + (double)pz * f.z + (double)f.x * px);
    float h = F(float, s, 8);
    Vec3_SetThis(&a, 0, 0, 0, -h);
    Vec3_SetThis(&b, 0, 0, 0, -h);
    Vec3_SetThis(&c, 0, side.x * h, side.y * h, side.z * h);
    Vec3_SetThis(&d, 0, 0, 0, h);
    Renderer_ResetCount(r, 0);
    const DWORD* col = (const DWORD*)(s + 96);
    for (BYTE* n = F(BYTE*, s, 128); n; n = F(BYTE*, n, 0)) {
        const Vec3* pt = (const Vec3*)(n + 4);
        double dd = (double)pt->y * f.y + (double)pt->z * f.z + (double)pt->x * f.x + dist;
        if (!(dd > F(float, s, 64)))
            continue;
        float k = (float)(dd * g_ls64c304 + g_ls64c260);
        Vec3_SetThis(&e, 0, d.x * k, d.y * k, d.z * k);
        Vec3_SetThis(&v1, 0, e.x + pt->x, e.y + pt->y, e.z + pt->z);
        Renderer_AddVertex(r, 0, &v1, col, 0.5f, 0);
        Vec3_SetThis(&v2, 0, c.x * k, c.y * k, c.z * k);
        Renderer_AddVertex(r, 0, Vec3_Sum(&t2, Vec3_Sum(&t1, pt, &a), &v2), col, 1.0f, 1.0f);
        Vec3_Times(&t3, &c, k);
        Vec3 sb;
        Vec3_Sum(&sb, pt, &b);
        Renderer_AddVertex(r, 0, Vec3_Diff(&v3, &sb, &t3), col, 0, 1.0f);
    }
    Effect_Zero();
    return 0;
}

} // extern "C"

// === sprites facing the camera (a method at $L_4c2c30)

// appends vertex p with colour c and (u, v) to renderer r's vertex array (+76, count +68 up to +64)
static void Fs_Put(BYTE* r, const Vec3* p, DWORD c, DWORD u, DWORD v)
{
    if (F(int, r, 68) == F(int, r, 64))
        return;
    BYTE* e = F(BYTE*, r, 76) + F(int, r, 68) * 24;
    F(Vec3, e, 0) = *p;
    F(DWORD, e, 12) = c;
    F(DWORD, e, 16) = u;
    F(DWORD, e, 20) = v;
    F(int, r, 68)++;
}

extern "C" {

// [$L_4c2c30] adds to renderer r a triangle per sprite of s's list (+52: position +4, size +16, distance +20)
// standing up from its position and facing the camera (+4: its heading), in colour +60; sprites beyond half of +32
// written directly, nearer ones by Renderer_AddVertexB with other texture corners
int __fastcall Sprites_Draw(BYTE* s, void*, BYTE* r)
{
    BYTE* cam = F(BYTE*, s, 4);
    float ang = (float)F(double, cam, 2928);
    float mb[12];
    const float* R = CallC<const float*>(Ls_RotZ, mb, *(DWORD*)&ang);
    Vec3 f, side, up, e, v1, g, v2, t1, t2, t3;
    Vec3_SetThis(&f, 0,
        (float)((double)g_ls702628 * R[6] + (double)g_ls702624 * R[3] + (double)g_ls702620 * R[0] + R[9]),
        (float)((double)g_ls702628 * R[7] + (double)g_ls702624 * R[4] + (double)g_ls702620 * R[1] + R[10]),
        (float)((double)g_ls702628 * R[8] + (double)g_ls702624 * R[5] + (double)g_ls702620 * R[2] + R[11]));
    Vec3_SetThis(&side, 0, (float)((double)f.z * g_ls702634 - (double)f.y * g_ls702638),
        (float)((double)f.x * g_ls702638 - (double)f.z * g_ls702630),
        (float)((double)f.y * g_ls702630 - (double)f.x * g_ls702634));
    up.x = 0, up.y = 0, up.z = 1.0f;
    const DWORD* col = (const DWORD*)(s + 60);
    for (BYTE* n = F(BYTE*, s, 52); n; n = F(BYTE*, n, 0)) {
        const Vec3* p = (const Vec3*)(n + 4);
        float k = F(float, n, 16);
        float h = k * g_64c258;
        if (F(float, s, 32) * g_64c258 < F(float, n, 20)) {
            Vec3_SetThis(&e, 0, up.x * k, up.y * k, up.z * k);
            Vec3_SetThis(&v1, 0, e.x + p->x, e.y + p->y, e.z + p->z);
            Fs_Put(r, &v1, *col, 0, 0);
            Vec3_SetThis(&g, 0, side.x * h, side.y * h, side.z * h);
            Vec3_SetThis(&v2, 0, g.x + p->x, g.y + p->y, g.z + p->z);
            Fs_Put(r, &v2, *col, 0x3F800000, 0);
            Vec3_SetThis(&g, 0, side.x * h, side.y * h, side.z * h);
            Renderer_AddVertexB(r, 0, Vec3_Diff(&t3, p, &g), col, 0, 1.0f);
        } else {
            Renderer_AddVertexB(r, 0, Vec3_Sum(&t1, p, Vec3_ScaleBy(&t2, k, &up)), col, 1.0f, 1.0f);
            Renderer_AddVertexB(r, 0, Vec3_Sum(&t1, p, Vec3_ScaleBy(&t2, h, &side)), col, 1.0f, 0);
            Renderer_AddVertexB(r, 0, Vec3_Diff(&t1, p, Vec3_ScaleBy(&t3, h, &side)), col, 0, 1.0f);
        }
    }
    Effect_Zero();
    return 0;
}

} // extern "C"

// === the ground under a point (a function at $L_4cb310)

extern "C" void Gq_Query(); KING_ALIAS("_Gq_Query", "_Query_4e67d1")
extern "C" void Gq_Root(); KING_ALIAS("_Gq_Root", "@Node_Root@8")
KING_VAR(float, g_gq702630, 702630)
KING_VAR(float, g_gq702634, 702634)
KING_VAR(float, g_gq702638, 702638)
KING_VAR(float, g_gq64c320, 64c320)

extern "C" {

// [$L_4cb310] the world surface (under node o's root) crossed by the segment through p along 702630 by 64c320
// either way: its point to *at and normal to *n, 1; 0 when none
int __cdecl Ground_Below(BYTE* o, const float* p, float* at, float* n)
{
    BYTE* root = CallThis<BYTE*>(Gq_Root, o);
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    double ox = (double)g_gq702630 * g_gq64c320;
    float oy = g_gq702634 * g_gq64c320, oz = g_gq702638 * g_gq64c320;
    float a[3] = {(float)(p[0] - ox), p[1] - oy, p[2] - oz};
    float b[3] = {(float)(ox + p[0]), oy + p[1], oz + p[2]};
    if (!CallC<int>(Gq_Query, root, b, a, q))
        return 0;
    memcpy(at, q + 52, 12);
    memcpy(n, q + 64, 12);
    return 1;
}

} // extern "C"

// === a hit spot somewhere ahead in the view (a method at $L_4c18b0)

extern "C" void Hv_MulMatrix(); KING_ALIAS("_Hv_MulMatrix", "_Vec3_MulMatrix")
extern "C" void Hv_RandomPoint(); KING_ALIAS("_Hv_RandomPoint", "_Triangle_RandomPoint")
extern "C" void Hv_HitRec(); KING_ALIAS("_Hv_HitRec", "@HitRec_Construct@12")
extern "C" void Hv_Add(); KING_ALIAS("_Hv_Add", "_Vec3_Add")
extern "C" void Hv_Lookup(); KING_ALIAS("_Hv_Lookup", "_Obj_Lookup4e5b22")
extern "C" void Hv_Query(); KING_ALIAS("_Hv_Query", "_Query_4e67d1")
extern "C" void Hv_Length(); KING_ALIAS("_Hv_Length", "_Vec3_Length")
extern "C" void Hv_MulAssign(); KING_ALIAS("_Hv_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Hv_RotZ(); KING_ALIAS("_Hv_RotZ", "_Matrix_RotationZ")
extern "C" void Hv_HAngle(); KING_ALIAS("_Hv_HAngle", "@MarineCamera_HorizontalAngle@8")
extern "C" int __cdecl rand(void);
KING_VAR(BYTE*, g_hv696224, 696224)
KING_VAR(BYTE*, g_hv6d2078, 6d2078)
KING_VAR(Vec3, g_hv702620, 702620)
KING_VAR(Vec3, g_hv702630, 702630)
KING_VAR(float, g_hv64c258, 64c258)
KING_VAR(float, g_hv64c304, 64c304)
KING_VAR(float, g_hv64c2fc, 64c2fc)
KING_VAR(float, g_hv64c254, 64c254)
KING_VAR(float, g_hv64c448, 64c448)
KING_VAR(float, g_hv64c664, 64c664)

// the forward axis turned by angle a about up into out
static Vec3* Hv_Turn(Vec3* out, const Vec3* v, float a)
{
    BYTE m[48];
    return CallC<Vec3*>(Hv_MulMatrix, out, v, CallC<void*>(Hv_RotZ, m, *(DWORD*)&a));
}

// picks a random spot in the camera's view (a triangle reaching further the faster the player's car goes),
// finds the object there (context's +488 +76 world) and the surface under it (only of the kinds 1, 5, 8, 9, 10
// with kinds), and records the hit in s's ring of 50 92-byte slots (count +4)
static void Hv_Pick(BYTE* s, BYTE* ctx, bool kinds)
{
    BYTE* cam = g_hv696224;
    Vec3 B = {0, 0, 0}, H = {0, 0, 0}, L, R, t;
    float half = (float)(CallThis<double>(Hv_HAngle, F(BYTE*, cam, 488)) * g_hv64c258);
    Hv_Turn(&L, &g_hv702620, half);
    Hv_Turn(&R, &g_hv702620, -half);
    L = *Hv_Turn(&t, &L, (float)F(double, g_hv696224, 2928));
    R = *Hv_Turn(&t, &R, (float)F(double, g_hv696224, 2928));
    Vec3 v = F(Vec3, F(BYTE*, g_hv6d2078, 616), 432);
    float speed = (float)(CallC<double>(Hv_Length, &v) * g_hv64c304 + g_hv64c2fc);
    double r1 = rand() * (double)g_hv64c254;
    float c = (float)x87_cos(half);
    float k = speed / c;
    float l1 = (float)(r1 * g_hv64c448 / c + k);
    CallThis(Hv_MulAssign, &L, &t, *(DWORD*)&l1);
    float l2 = (float)(rand() * (double)g_hv64c254 * g_hv64c448 / c + k);
    CallThis(Hv_MulAssign, &R, &t, *(DWORD*)&l2);
    B.x = (float)F(double, g_hv696224, 2904), B.y = (float)F(double, g_hv696224, 2912),
    B.z = (float)F(double, g_hv696224, 2920);
    Vec3 a1, a2, rp;
    Vec3* p1 = CallC<Vec3*>(Hv_Add, &a1, &B, &R);
    Vec3* p2 = CallC<Vec3*>(Hv_Add, &a2, &B, &L);
    H = *CallC<Vec3*>(Hv_RandomPoint, &rp, &B, p2, p1);
    char miss = 0;
    BYTE* w = F(BYTE*, ctx, 488);
    if (w)
        w = F(BYTE*, w, 76);
    BYTE* o = CallC<BYTE*>(Hv_Lookup, w, &B, &H, &miss);
    if (miss)
        return;
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    float ux = g_hv702630.x * g_hv64c664, uy = g_hv702630.y * g_hv64c664, uz = g_hv702630.z * g_hv64c664;
    Vec3 lo = {H.x - ux, H.y - uy, H.z - uz};
    Vec3 hi = {ux + H.x, uy + H.y, uz + H.z};
    if (!CallC<int>(Hv_Query, o, &hi, &lo, q))
        return;
    if (kinds) {
        const int* m = F(const int*, q, 100);
        if (!m || m[0] != 1)
            return;
        if (m[1] != 1 && m[1] != 5 && m[1] != 8 && m[1] != 9 && m[1] != 10)
            return;
    }
    unsigned slot = F(unsigned, s, 4) % 50;
    BYTE* rec = s + slot * 92;
    BYTE hr[128];
    memcpy(rec + 8, CallThis<BYTE*>(Hv_HitRec, hr, q), 92);
    BYTE* next = s + (slot + 1) * 92;
    if (!F(BYTE*, next, 0))
        F(BYTE*, next, 0) = o;
    if (!F(int, rec, 96))
        F(int, rec, 96) = F(int, F(BYTE*, next, 0), 36);
    F(int, s, 4)++;
}

extern "C" void Hv_GetMaterial(); KING_ALIAS("_Hv_GetMaterial", "@MarineTask_GetMaterial@8")
extern "C" void Hv_ForwardPoint(); KING_ALIAS("_Hv_ForwardPoint", "@MarineTask_ForwardPointCopy@12")

extern "C" {

// [$L_4c2380] with marine task s+8 active: a random spot in the camera's view (as for hit spots) put on the
// task's ground and recorded in s's ring of 50 points (+12, count +4)
void __fastcall SplashSpot_Pick(BYTE* s, void*, int unused)
{
    if (!CallThis<int>(Hv_GetMaterial, F(BYTE*, s, 8)))
        return;
    BYTE* cam = g_hv696224;
    Vec3 B = {0, 0, 0}, H = {0, 0, 0}, L, R, t;
    float half = (float)(CallThis<double>(Hv_HAngle, F(BYTE*, cam, 488)) * g_hv64c258);
    BYTE mb[48];
    const float* M = CallC<const float*>(Hv_RotZ, mb, *(DWORD*)&half);
    const Vec3& f = g_hv702620;
    Vec3_SetThis(&L, 0, (float)((double)f.z * M[6] + (double)f.y * M[3] + (double)f.x * M[0] + M[9]),
        (float)((double)f.z * M[7] + (double)f.y * M[4] + (double)f.x * M[1] + M[10]),
        (float)((double)f.z * M[8] + (double)f.y * M[5] + (double)f.x * M[2] + M[11]));
    Hv_Turn(&R, &g_hv702620, -half);
    L = *Hv_Turn(&t, &L, (float)F(double, g_hv696224, 2928));
    R = *Hv_Turn(&t, &R, (float)F(double, g_hv696224, 2928));
    Vec3 v = F(Vec3, F(BYTE*, g_hv6d2078, 616), 432);
    float speed = (float)(CallC<double>(Hv_Length, &v) * g_hv64c304 + g_hv64c2fc);
    double r1 = rand() * (double)g_hv64c254;
    float c = (float)x87_cos(half);
    float k = speed / c;
    float l1 = (float)(r1 * g_hv64c448 / c + k);
    CallThis(Hv_MulAssign, &L, &t, *(DWORD*)&l1);
    float l2 = (float)(rand() * (double)g_hv64c254 * g_hv64c448 / c + k);
    CallThis(Hv_MulAssign, &R, &t, *(DWORD*)&l2);
    B.x = (float)F(double, g_hv696224, 2904), B.y = (float)F(double, g_hv696224, 2912),
    B.z = (float)F(double, g_hv696224, 2920);
    Vec3 a1, a2, rp;
    Vec3* p1 = CallC<Vec3*>(Hv_Add, &a1, &B, &R);
    Vec3* p2 = CallC<Vec3*>(Hv_Add, &a2, &B, &L);
    H = *CallC<Vec3*>(Hv_RandomPoint, &rp, &B, p2, p1);
    unsigned slot = F(unsigned, s, 4) % 50;
    CallThis(Hv_ForwardPoint, F(BYTE*, s, 8), &H);
    F(Vec3, s, 12 + slot * 12) = H;
    F(int, s, 4)++;
}

// [$L_4c18b0] a hit spot anywhere in the view
void __fastcall HitSpot_Pick(BYTE* s, void*, BYTE* ctx) { Hv_Pick(s, ctx, false); }

// [$L_4c1e20] a hit spot on surfaces of the kinds 1, 5, 8, 9, 10
void __fastcall HitSpot_PickKind(BYTE* s, void*, BYTE* ctx) { Hv_Pick(s, ctx, true); }

} // extern "C"

// === choosing the next gear (FUN_4d57c0)

extern "C" void Gs_Mask(); KING_ALIAS("_Gs_Mask", "_Car_GearBlockMask")
KING_VAR(double, g_gs64b6d0, 64b6d0)
KING_VAR(double, g_gs64b6e0, 64b6e0)
KING_VAR(double, g_gs64b678, 64b678)

// whether gear slot n (two bits at n, wrapping like a shift does) is free in mask m
static bool Gs_Free(unsigned m, int n)
{
    return !(m & (3u << (n & 31)));
}

extern "C" {

// [FUN_4d57c0] the gear to take after gear g at speed v (window +a, gear step k): the mask of blocked gears
// from FUN_4d4a90 goes to *mask; when shifting is on (*flags bit 2) a free lower gear first, then a higher one
// whose speed v reaches, then with the pedal off (pedal below 0, no shift flags 12) lower ones and higher ones
// within the window; none: *flags bit 8; the gear to *out; the cost from FUN_4d4a90 (1000 by default)
double __cdecl Gear_Choose(int a0, int g, double v, double win, double k, int shift, double pedal, int a52, int a56,
    int a60, int a64, double* d, unsigned* flags, unsigned* mask, int* out, int a84)
{
    *flags &= ~8u;
    double res = 1000.0;
    *d = -1.5;
    unsigned m = CallC<unsigned>(Gs_Mask, a0, a52, a56, a60, a64, ((DWORD*)&v)[0], ((DWORD*)&v)[1], ((DWORD*)&k)[0],
        ((DWORD*)&k)[1], d, &res, a84);
    *mask = m;
    unsigned f = *flags;
    if (!(f & 2) && !Gs_Free(m, g + 4))
        *flags = f | 2;
    f = *flags;
    if (!(f & 2)) {
        *out = g;
        return res;
    }
    m = *mask;
    if (Gs_Free(m, g + 4)) {
        *flags = f & ~2u;
        *out = g;
        return res;
    }
    if ((a64 && *d > g_gs64b6d0) || (f & 16)) {
        *flags = f | 8;
        return res;
    }
    for (int i = 1; i <= 4; i++)
        if (g > i - 1 && Gs_Free(m, g + 4 - i)) {
            *out = g - i;
            return res;
        }
    double step[4];
    for (int i = 0; i < 4; i++)
        step[i] = (g + 3 + i) * k * g_gs64b6e0;
    if (!(step[0] > v) && Gs_Free(m, g + 5)) {
        *out = g + 1;
        return res;
    }
    for (int i = 1; i < 4; i++)
        if (step[i] < v && Gs_Free(m, g + 5 + i)) {
            *out = g + 1 + i;
            return res;
        }
    if (!(shift & 12)) {
        for (int i = 1; i <= 4; i++)
            if (Gs_Free(m, g + 4 - i) && pedal < g_gs64b678) {
                *out = g - i;
                return res;
            }
    }
    double lim = v + win;
    for (int i = 0; i < 4; i++)
        if (step[i] < lim && Gs_Free(m, g + 5 + i) && pedal < g_gs64b678) {
            *out = g + 1 + i;
            return res;
        }
    *flags = f | 8;
    return res;
}

} // extern "C"

// === laying a patch onto the surfaces below (a method at $L_4c8190)

extern "C" void Lp_TerrainHeight(); KING_ALIAS("_Lp_TerrainHeight", "@Terrain_HeightAt@16")
extern "C" void Lp_Subdivide(); KING_ALIAS("_Lp_Subdivide", "_Tri_Subdivide")
extern "C" void Lp_PlaneHeight(); KING_ALIAS("_Lp_PlaneHeight", "_Plane_HeightX")
extern "C" void Lp_TrisHeight(); KING_ALIAS("_Lp_TrisHeight", "_Tris_Height")
extern "C" void Lp_RecordsHeight(); KING_ALIAS("_Lp_RecordsHeight", "_Records_Height")
extern "C" void Lp_Inverse(); KING_ALIAS("_Lp_Inverse", "_Matrix_Inverse")
KING_VAR(BYTE, g_lp694b08, 694b08)    // 40 records of 52 bytes: the object, its inverse frame
KING_VAR(BYTE*, g_lp694a64, 694a64)   // per triangle: its frame
KING_VAR(BYTE*, g_lp6953a8, 6953a8)   // per triangle: its plane
KING_VAR(float, g_lp695548, 695548)   // per triangle: the x of its corners
KING_VAR(float, g_lp695728, 695728)   // per triangle: the y of its corners
KING_VAR(float, g_lp64c304, 64c304)
KING_VAR(float, g_lp64c1fc, 64c1fc)

extern "C" {

// [$L_4c8190] lays patch s (points +4 of 32 bytes, count +2216) made from triangle tri onto the n objects arr
// (208 bytes each, below 40) when they are all of surface kinds 1, 5, 8, 9: their polygon faces' triangles in
// their own frames give the heights, else a terrain (kind 8) or a plane (kind 5) under them, else the objects'
// own heights; each point lifted by 64c304; 0 when an object doesn't qualify
int __fastcall Patch_Lay(BYTE* s, void*, BYTE* arr, unsigned n, BYTE* tri, int a4)
{
    if (!n || n >= 40)
        return 0;
    unsigned cnt = 0;
    BYTE* terrain = 0;
    int flat = 0;
    BYTE* plane = 0;
    BYTE* recs = &g_lp694b08;
    for (unsigned i = 0; i < n; i++) {
        BYTE* o = arr + 100 + i * 208;
        unsigned j = 0;
        if (cnt)
            for (; j < cnt; j++) {
                BYTE* ob = F(BYTE*, recs + j * 52, 0);
                if (F(int, ob, 108) == F(int, o, 8) && F(int, ob, 112) == F(int, o, 12))
                    break;
            }
        if (j != cnt)
            continue;
        const int* m = F(const int*, o, 0);
        if (!m || m[0] != 1)
            return 0;
        if (m[1] != 1 && m[1] != 5 && m[1] != 8 && m[1] != 9)
            return 0;
        const int* t = F(const int*, o, 8);
        if (t) {
            if (t[0] == 8)
                terrain = (BYTE*)t[2];
            if (t[0] == 5) {
                flat = 1;
                plane = (BYTE*)t[2];
            }
        }
        BYTE* rec = recs + cnt * 52;
        F(BYTE*, rec, 0) = o - 100;
        BYTE inv[48];
        memcpy(rec + 4, CallC<void*>(Lp_Inverse, inv, o - 96), 48);
        cnt++;
    }
    CallC(Lp_Subdivide, s + 4, s + 2216, s + 2084, s + 2212, tri, tri + 32, tri + 64, a4);
    unsigned k = 0;
    for (unsigned j = 0; j < cnt && k < 40; j++) {
        BYTE* rec = recs + j * 52;
        BYTE* ob = F(BYTE*, rec, 0);
        const int* t = F(const int*, ob, 108);
        if (!t || t[0] != 6)
            continue;
        BYTE* poly = (BYTE*)t[2] + F(int, ob, 112) * 48 - 48;
        if (F(WORD, poly, 2) < 3)
            continue;
        const float* M = (const float*)(rec + 4);
        for (unsigned e = 1; e < (unsigned)F(WORD, poly, 2) - 1 && k < 40; e++, k++) {
            const float* v = F(const float*, poly, 4);
            const float* v1 = v + e * 3;
            const float* v2 = v1 + 3;
            float x0 = (float)((double)M[6] * v[2] + (double)M[0] * v[0] + (double)v[1] * M[3] + M[9]);
            float y0 = (float)((double)v[1] * M[4] + (double)M[7] * v[2] + (double)M[1] * v[0] + M[10]);
            float x1 = (float)((double)v1[2] * M[6] + (double)v1[0] * M[0] + (double)v1[1] * M[3] + M[9]);
            float y1 = (float)((double)v1[2] * M[7] + (double)v1[0] * M[1] + (double)v1[1] * M[4] + M[10]);
            float x2 = (float)((double)v2[2] * M[6] + (double)M[0] * v2[0] + (double)v2[1] * M[3] + M[9]);
            float y2 = (float)((double)v2[2] * M[7] + (double)v2[1] * M[4] + (double)M[1] * v2[0] + M[10]);
            (&g_lp694a64)[k] = (BYTE*)M;
            float* X = &g_lp695548 + k * 3;
            float* Y = &g_lp695728 + k * 3;
            X[0] = x0, X[1] = x1, X[2] = x2;
            Y[0] = y0, Y[1] = y1, Y[2] = y2;
            (&g_lp6953a8)[k] = poly + 32;
        }
    }
    for (unsigned i = 0; i < F(unsigned, s, 2216); i++) {
        float* p = (float*)(s + 4 + i * 32);
        float h;
        double z;
        if (CallC<int>(Lp_TrisHeight, &h, *(DWORD*)&p[0], *(DWORD*)&p[1], &g_lp695548, &g_lp695728, &g_lp6953a8,
                &g_lp694a64, k))
            z = h;
        else if (terrain)
            z = CallThis<float>(Lp_TerrainHeight, terrain, *(DWORD*)&p[0], *(DWORD*)&p[1]);
        else {
            if (flat) {
                p[2] = (float)(CallC<double>(Lp_PlaneHeight, *(DWORD*)&p[0], *(DWORD*)&p[1], plane) + g_lp64c304);
                if (p[2] != g_lp64c1fc)
                    continue;
            }
            z = CallC<double>(Lp_RecordsHeight, *(DWORD*)&p[0], *(DWORD*)&p[1], arr, n);
        }
        p[2] = (float)(z + g_lp64c304);
    }
    return 1;
}

} // extern "C"

// === a beam of light from a point to the camera's side (FUN_4c8af0)

extern "C" void FUN_4c9930(); KING_ALIAS("_FUN_4c9930", "@Fx_Set8@32")
extern "C" void FUN_4c89d0(); KING_ALIAS("_FUN_4c89d0", "@Fx_SpawnAt@20")
extern "C" void FUN_4e67d1(); KING_ALIAS("_FUN_4e67d1", "_Query_4e67d1")
extern "C" void FUN_4e6ac4(); KING_ALIAS("_FUN_4e6ac4", "_Query_4e6ac4")
extern "C" void FUN_4e5bf5(); KING_ALIAS("_FUN_4e5bf5", "_Obj_Lookup4e5b22")
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
KING_VAR(float, g_bm696064, 696064)               // the effects' detail
KING_VAR(BYTE*, g_bm696254, 696254)
KING_VAR(float, g_bm64c258, 64c258)
KING_VAR(float, g_bm64c304, 64c304)
KING_VAR(float, g_bm64c200, 64c200)
KING_VAR(float, g_bm64c264, 64c264)
KING_VAR(float, g_bm64c664, 64c664)
KING_VAR(float, g_bm64c204, 64c204)
KING_VAR(float, g_bm64c208, 64c208)
KING_VAR(float, g_bm64c20c, 64c20c)
KING_VAR(float, g_bm64c908, 64c908)
KING_VAR(float, g_bm64c904, 64c904)
KING_VAR(float, g_bm64c250, 64c250)
KING_VAR(float, g_bm64c458, 64c458)

static Vec3 Bm_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

static Vec3 Bm_Up() { return Bm_Set((double)g_axisZ.x * g_bm64c664, (double)g_axisZ.y * g_bm64c664, (double)g_axisZ.z * g_bm64c664); }

// a vertex record {pos, color, a, b, c, d} made by Fx_Set8
static void Bm_Vertex(BYTE* rec, const Vec3* pos, DWORD color, float a, float b, float c, float d)
{
    BYTE r[32];
    memcpy(rec, CallThis<BYTE*>(FUN_4c9930, r, pos, &color, a, b, c, d), 32);
}

extern "C" {

// [FUN_4c8af0] a beam (width w, length scale len, frame i of the texture) from point p along dir, turned
// toward the camera, cut at the walls of object obj (its ground node, or below p), and drawn as two
// triangles of 696254
int __cdecl Fx_Beam(const Vec3* p, float len, float w, const Vec3* dir, int frame, int obj, const BYTE* node)
{
    if (!obj || g_bm696064 < g_bm64c258)
        return 0;
    float hw = (float)((double)w * g_bm64c258);
    BYTE* view = g_view;
    Vec3 cam = Bm_Set(F(double, view, 2904), F(double, view, 2912), F(double, view, 2920));
    Vec3 d = Bm_Set((double)cam.x - p->x, (double)cam.y - p->y, (double)cam.z - p->z), n, side, across;
    Vec3_Normalized(&n, &d);
    Vec3_Cross(&side, &n, &g_axisZ);
    Vec3_Cross(&across, dir, &g_axisZ);
    float dot = (float)((double)dir->x * n.x + (double)n.z * dir->z + (double)n.y * dir->y);
    if (!(dot > g_bm64c304))
        return 0;
    Matrix rz;
    const float* r = Matrix_RotationZ(&rz, (float)F(double, view, 2928))->m;
    double ra = (double)g_axisY.z * r[6] + (double)g_axisY.y * r[3] + (double)g_axisY.x * r[0] + r[9];
    float rb = (float)((double)g_axisY.z * r[7] + (double)g_axisY.y * r[4] + (double)g_axisY.x * r[1] + r[10]);
    Vec3 flat = Bm_Set(dir->x, dir->y, 0), view2 = Bm_Set(ra, rb, 0), u, t, v988;
    Vec3_Normalized(&u, &flat);
    Vec3_Normalized(&t, &view2);
    v988 = Bm_Set((double)t.y * u.z - (double)u.y * t.z, (double)u.x * t.z - (double)t.x * u.z, (double)u.y * t.x - (double)u.x * t.y);
    float bright = (float)((double)dot * F(float, g_truck, 536));
    Vec3 back = Bm_Set((double)p->x - cam.x, (double)p->y - cam.y, (double)p->z - cam.z);
    float L = (float)(Vec3_Length(&back) * g_bm64c258);
    if (L > g_bm64c200)
        L = g_bm64c200;
    Vec3 q = Bm_Set((double)n.x * L, (double)n.y * L, (double)n.z * L);
    q = Bm_Set((double)q.x + p->x, (double)q.y + p->y, (double)q.z + p->z);
    Vec3 p2 = *p;
    if (node && F(float, node, 60) == g_bm64c264 && F(float, node, 52) == g_bm64c264 && F(float, node, 56) == g_bm64c264)
        node = 0;
    BYTE n880[208], n672[208], n464[208];
    CallThis(FUN_5ef5c0, n880);
    CallThis(FUN_5ef5c0, n672);
    if (!node)
    {
        Vec3 up = Bm_Up();
        Vec3 lo = Bm_Set((double)p->x - up.x, (double)p->y - up.y, (double)p->z - up.z);
        up = Bm_Up();
        Vec3 hi = Bm_Set((double)up.x + p->x, (double)up.y + p->y, (double)up.z + p->z);
        if (!CallC<int>(FUN_4e67d1, obj, &hi, &lo, n880))
            return 0;
    }
    else
        memcpy(n880, node, 208);
    char stop = 0;
    int o2 = CallC<int>(FUN_4e5bf5, obj, &p2, &q, &stop);
    if (stop)
        return 0;
    {
        Vec3 up = Bm_Up();
        Vec3 lo = Bm_Set((double)q.x - up.x, (double)q.y - up.y, (double)q.z - up.z);
        up = Bm_Up();
        Vec3 hi = Bm_Set((double)up.x + q.x, (double)up.y + q.y, (double)up.z + q.z);
        if (!CallC<int>(FUN_4e67d1, o2, &hi, &lo, n672))
            return 0;
    }
    Vec3 a = *p, b = F(Vec3, n672, 52);
    CallThis(FUN_5ef5c0, n464);
    if (CallC<int>(FUN_4e6ac4, o2, &a, &b, n464))
        return 0;
    if (o2 != obj && CallC<int>(FUN_4e6ac4, obj, &a, &b, n464))
        return 0;
    q.z = F(float, n672, 60);
    p2.z = F(float, n880, 60);
    double k = (double)g_bm64c204 * hw / dot;
    float w2 = (float)((double)g_bm64c208 * hw);
    float tilt = (float)-((double)v988.z * hw);
    if (g_eff7208d0)
    {
        k *= g_bm64c20c;
        w2 = (float)((double)w2 * g_bm64c20c);
    }
    float l1 = (float)((double)len * g_bm64c908);
    float l2 = (float)(((double)L + len) * g_bm64c908);
    double fi = frame;
    float u1 = (float)(fi * g_bm64c904);
    float u2 = (float)((fi + g_bm64c250) * g_bm64c904);
    Vec3 s = Bm_Set(across.x * k, across.y * k, across.z * k);
    Vec3 v1 = Bm_Set((double)p2.x - s.x, (double)p2.y - s.y, (double)p2.z - s.z);
    Vec3 v2 = Bm_Set((double)s.x + p2.x, (double)s.y + p2.y, (double)s.z + p2.z);
    Vec3 nl = Bm_Set((double)n.x * L, (double)n.y * L, (double)n.z * L);
    Vec3 cw = Bm_Set((double)side.x * w2, (double)side.y * w2, (double)side.z * w2);
    Vec3 m3 = Bm_Set((double)p2.x - cw.x, (double)p2.y - cw.y, (double)p2.z - cw.z);
    Vec3 v3 = Bm_Set((double)m3.x + nl.x, (double)m3.y + nl.y, (double)m3.z + nl.z);
    DWORD color = ((DWORD)(int)((double)bright * g_bm64c458) << 24) | 0xffffff;
    BYTE tri[96];
    float lt1 = (float)((double)l1 - tilt), lt2 = (float)((double)l1 + tilt);
    F(Vec3, tri, 0) = v1;
    F(DWORD, tri, 12) = color;
    F(float, tri, 16) = u1;
    F(float, tri, 20) = 0;
    F(float, tri, 24) = 0;
    F(float, tri, 28) = lt1;
    F(Vec3, tri, 32) = v2;
    F(DWORD, tri, 44) = color;
    F(float, tri, 48) = u2;
    F(float, tri, 52) = 0;
    F(float, tri, 56) = 1.0f;
    F(float, tri, 60) = lt2;
    Bm_Vertex(tri + 64, &v3, color, u1, 1.0f, 0, l2);
    CallThis(FUN_4c89d0, g_bm696254, n880, 2, tri);
    Vec3 t1, t2, t3, w1, w3, v4, v5, v6;
    v4 = *Vec3_Add(&t1, &p2, &s);
    Vec3_Scaled(&t1, &n, L);
    v5 = *Vec3_Add(&t3, Vec3_Add(&t2, &p2, Vec3_Scaled(&w1, &side, w2)), &t1);
    Vec3_Scaled(&t1, &n, L);
    v6 = *Vec3_Add(&t3, Vec3_Sub(&t2, &p2, Vec3_Scaled(&w3, &side, w2)), &t1);
    Bm_Vertex(tri, &v4, color, u2, 0, 1.0f, lt2);
    Bm_Vertex(tri + 32, &v5, color, u2, 1.0f, 1.0f, l2);
    Bm_Vertex(tri + 64, &v6, color, u1, 1.0f, 0, l2);
    CallThis(FUN_4c89d0, g_bm696254, n880, 2, tri);
    return 0;
}

} // extern "C"

// === a vehicle's shadows (FUN_4ca7e0)

#include "x87.h"
X87 double Sh_tan(double x) { __asm { fld x } __asm { fptan } __asm { fstp st(0) } }

extern "C" void Sh_Get800(); KING_ALIAS("_Sh_Get800", "@SceneObj_Get800@8")
extern "C" void Sh_Matrix1(); KING_ALIAS("_Sh_Matrix1", "@SceneObj_Matrix1@12")
extern "C" void Sh_Matrix2(); KING_ALIAS("_Sh_Matrix2", "@SceneObj_Matrix2@12")
extern "C" void Sh_BoxCenter(); KING_ALIAS("_Sh_BoxCenter", "_Fx_BoxCenter")
extern "C" void Sh_Find(); KING_ALIAS("_Sh_Find", "_Fx_Find")
extern "C" void Sh_Visibility(); KING_ALIAS("_Sh_Visibility", "_Sphere_Visibility")
extern "C" void Sh_Negated(); KING_ALIAS("_Sh_Negated", "_Vec3_Negated")
extern "C" void Sh_Get70ebf8(); KING_ALIAS("_Sh_Get70ebf8", "_Get70ebf8")
extern "C" void Sh_Add(); KING_ALIAS("_Sh_Add", "@Shadows_Add@12")
extern "C" void Fs_Log(); KING_ALIAS("_Fs_Log", "_Sys_Log")
KING_VAR(BYTE*, g_sh6d2078, 6d2078)
KING_VAR(float, g_sh696040, 696040)               // the shadows' detail
KING_VAR(char, g_sh695eb0, 695eb0)
KING_VAR(char, s_sh6728e8, 6728e8)
KING_VAR(float, g_sh64c93c, 64c93c)
KING_VAR(float, g_sh64c420, 64c420)
KING_VAR(float, g_sh64c328, 64c328)
KING_VAR(float, g_sh64c32c, 64c32c)

static Vec3 Sh_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

static Vec3 Sh_Up() { return Sh_Set((double)g_axisZ.x * g_bm64c664, (double)g_axisZ.y * g_bm64c664, (double)g_axisZ.z * g_bm64c664); }

// the point b of car c's frame, unrounded
static void Sh_Place(BYTE* c, const Vec3& b, double* o)
{
    BYTE t[48];
    const float* M = CallThis<const float*>(Sh_Matrix1, c + 16, t);
    o[0] = (double)b.z * M[6] + (double)b.y * M[3] + (double)b.x * M[0] + M[9];
    o[1] = (double)b.z * M[7] + (double)b.y * M[4] + (double)b.x * M[1] + M[10];
    o[2] = (double)b.z * M[8] + (double)b.y * M[5] + (double)b.x * M[2] + M[11];
}

extern "C" {

// [FUN_4ca7e0] casts car's shadows sh1 / sh2 (mode 1: one, 2: both) down from its box center onto what lies
// below, through 70ebf8's shadow list
void __cdecl Fx_CarShadows(BYTE* car, BYTE* sh1, BYTE* sh2, int mode)
{
    if (!car)
        return;
    if (sh1)
        sh1[40] |= 1;
    else if (!sh2)
        return;
    if (sh2)
        sh2[40] |= 1;
    if (car == F(BYTE*, g_sh6d2078, 616))
    {
        if (sh1)
            F(int, sh1, 128) = 150;
        if (sh2)
            F(int, sh1, 128) = 140;
    }
    else
    {
        if (g_sh696040 < g_bm64c258)
            return;
        if (sh1)
            F(int, sh1, 128) = 140;
        if (sh2)
            F(int, sh1, 128) = 130;
    }
    switch (mode)
    {
    case 0:
        return;
    case 1:
        if (sh1)
            sh1[40] &= 0xfe;
        break;
    case 2:
        if (sh1)
            sh1[40] &= 0xfe;
        if (sh2)
            sh2[40] &= 0xfe;
        else
            mode = 1;
        break;
    default:
        sprintf(&g_sh695eb0, &s_sh6728e8, mode);
        CallC(Fs_Log, &g_sh695eb0, 0);
        return;
    }
    unsigned n = (mode != 1) + 1;
    double a = mode == 1 ? g_sh64c93c : g_bm64c664;
    float ang = (float)(x87_atan2(a * g_sh64c420, 1.0) * 2);
    double sp = x87_atan2(a * g_sh64c328, 1.0) * 2 - ang;
    float spread = (float)sp;
    double hv = (sp + ang) * g_bm64c258;
    float radius = (float)(Sh_tan(hv) * g_bm64c664);
    BYTE* obj = CallThis<BYTE*>(Sh_Get800, car + 16);
    BYTE node[208];
    CallThis(FUN_5ef5c0, node);
    Vec3 off = Sh_Set((double)g_axisY.x * g_sh64c32c, (double)g_axisY.y * g_sh64c32c, (double)g_axisY.z * g_sh64c32c);
    Vec3 bc;
    const Vec3* c = CallC<const Vec3*>(Sh_BoxCenter, &bc, car);
    Vec3 base = Sh_Set((double)off.x + c->x, (double)off.y + c->y, (double)off.z + c->z);
    Vec3 up = Sh_Up();
    double p[3];
    Sh_Place(car, base, p);
    Vec3 top = Sh_Set(p[0] + up.x, (double)up.y + (float)p[1], (double)up.z + (float)p[2]);
    BYTE t[48];
    const float* M = CallThis<const float*>(Sh_Matrix1, car + 16, t);
    const Vec3& z0 = g_zeroVec;
    Vec3 at;
    at.y = (float)((double)z0.z * M[7] + (double)z0.y * M[4] + (double)z0.x * M[1] + M[10]);
    at.z = (float)((double)z0.z * M[8] + (double)z0.y * M[5] + (double)z0.x * M[2] + M[11]);
    at.x = (float)((double)z0.z * M[6] + (double)z0.y * M[3] + (double)z0.x * M[0] + M[9]);
    obj = CallC<BYTE*>(Sh_Find, obj, &at, &top);
    Vec3 hit = {0, 0, 0}, nrm = {0, 0, 0};
    BYTE* shs[2] = {sh1, sh2};
    for (unsigned i = 0; i < n; i++)
    {
        BYTE* sh = shs[i];
        F(float, sh, 104) = spread;
        F(float, sh, 100) = ang;
        F(int, sh, 136) = 1;
        F(int, sh, 140) = 1;
        F(int, sh, 56) = 3;
        F(float, sh, 84) = 1.0f;
        F(int, sh, 88) = 0;
        F(DWORD, sh, 92) = 982622900;
        F(float, sh, 108) = 1.0f;
        F(float, sh, 112) = 1.0f;
        F(float, sh, 116) = 1.0f;
        up = Sh_Up();
        Sh_Place(car, base, p);
        Vec3 lo = Sh_Set(p[0] - up.x, p[1] - up.y, p[2] - up.z);
        Vec3 up2 = Sh_Up();
        Sh_Place(car, base, p);
        Vec3 w = Sh_Set(p[0], p[1], p[2]);
        Vec3 hi = Sh_Set((double)up2.x + w.x, (double)up2.y + w.y, (double)up2.z + w.z);
        if (CallC<int>(FUN_4e67d1, obj, &hi, &lo, node))
        {
            hit = F(Vec3, node, 52);
            nrm = F(Vec3, node, 64);
        }
        else
        {
            nrm = g_axisZ;
            Sh_Place(car, base, p);
            hit = Sh_Set(p[0], p[1], p[2]);
        }
        Vec3 dy = Sh_Set((double)g_axisY.x * radius, (double)g_axisY.y * radius, (double)g_axisY.z * radius);
        base = Sh_Set((double)dy.x + base.x, (double)dy.y + base.y, (double)dy.z + base.z);
        Vec3 u3 = Sh_Up(), r;
        BYTE t2[48];
        Vec3_MulMatrix(&r, &base, CallThis<const Matrix*>(Sh_Matrix1, car + 16, t2));
        Vec3 farp = Sh_Set((double)u3.x + r.x, (double)u3.y + r.y, (double)u3.z + r.z);
        BYTE* next = CallC<BYTE*>(Sh_Find, obj, &hit, &farp);
        float sphere[4] = {hit.x, hit.y, hit.z, radius};
        float vis = (float)CallC<double>(Sh_Visibility, sphere);
        if (!(vis > g_bm64c264))
            sh[40] |= 1;
        else
        {
            Matrix m592 = *(const Matrix*)(car + 756), m544;
            BYTE t3[48], t4[48];
            const Matrix* M2 = CallThis<const Matrix*>(Sh_Matrix2, car + 16, t3);
            Vec3 a1, a2, a3, a4;
            Vec3_MulMatrix(&a4, Vec3_MulMatrix(&a3, Vec3_Add(&a2, &hit, Vec3_Scaled(&a1, &nrm, 30.0f)), M2), &m592);
            F(Vec3, sh, 60) = a4;
            m544 = *(const Matrix*)(car + 756);
            const Matrix* M3 = CallThis<const Matrix*>(Sh_Matrix2, car + 16, t4);
            Vec3 b1, b2, b3;
            Vec3_RotateBy(&b3, Vec3_RotateBy(&b2, CallC<const Vec3*>(Sh_Negated, &b1, &nrm), M3), &m544);
            F(Vec3, sh, 72) = b3;
            F(float, sh, 96) = vis;
            if (CallC<BYTE*>(Sh_Get70ebf8))
                CallThis(Sh_Add, CallC<BYTE*>(Sh_Get70ebf8), sphere);
        }
        obj = next;
    }
}

} // extern "C"

// === what lies ahead of an AI car (FUN_4d3880)

extern "C" void Ob_Enum1(); KING_ALIAS("_Ob_Enum1", "_Enum_56f3f0")
extern "C" void Ob_Enum2(); KING_ALIAS("_Ob_Enum2", "_Enum_56f650")
extern "C" void Ob_SubAssign(); KING_ALIAS("_Ob_SubAssign", "@Vec3_SubAssign@16")
extern "C" void Ob_TransformPoint(); KING_ALIAS("_Ob_TransformPoint", "@Vec3_TransformPoint@12")
extern "C" void Ob_IdentityThis(); KING_ALIAS("_Ob_IdentityThis", "@Matrix_IdentityThis@8")
KING_VAR(float, g_ob64c970, 64c970)
KING_VAR(float, g_ob64d2f8, 64d2f8)
KING_VAR(float, g_ob64c354, 64c354)
KING_VAR(double, g_ob64b6e0, 64b6e0)
KING_VAR(double, g_ob64b660, 64b660)
KING_VAR(float, g_ob64c40c, 64c40c)
KING_VAR(float, g_ob64c250, 64c250)
extern "C" void Ob_GetWorld(); KING_ALIAS("_Ob_GetWorld", "@Node_GetWorld@12")

struct ObSphere { float v[4]; };    // center and radius, passed by value

static Vec3 Ob_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

// the flag of probe k (0 left, 1 middle, 2 right) hitting at distance L within r
static void Ob_Hit(int* flags, int k, double L, double r)
{
    if (L < g_ob64b660)
        *flags |= 8;
    if (!(L < r))
        return;
    *flags |= 4;
    if (k == 0)
        *flags |= 1;
    else if (k == 1)
        *flags |= 16;
    else
        *flags |= 2;
}

extern "C" {

// [FUN_4d3880] what is ahead of AI driver ai's car (+16576) on side (0 or the other): three probes from the
// bumper corners (+3848 / +3860) and the middle, then the objects in a sphere ahead: flags 1 left, 2 right,
// 4 in reach, 8 close, 16 middle (reported as right when nothing else is)
int __cdecl Ai_LookAhead(BYTE* ai, int side)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    int flags = 0;
    double h = F(float, ai, 13100);
    double reach = h * h * g_ob64c970 + g_ob64d2f8;
    BYTE* car = F(BYTE*, ai, 16576);
    Matrix m, inv;
    CallThis(Sh_Matrix1, car + 16, &m);
    Matrix_Inverse(&inv, &m);
    BYTE* obj = CallThis<BYTE*>(Sh_Get800, car + 16);
    Vec3 pts[3];
    const float* a = (const float*)(car + (side ? 24 : 0) + 3848);
    const float* b = (const float*)(car + (side ? 24 : 0) + 3860);
    double k354 = g_ob64c354;
    pts[0] = Ob_Set(a[0] * k354, a[1] * k354, a[2] * k354);
    pts[2] = Ob_Set(b[0] * k354, b[1] * k354, b[2] * k354);
    double sx = (double)pts[2].x + pts[0].x;
    float sy = (float)((double)pts[0].y + pts[2].y), sz = (float)((double)pts[0].z + pts[2].z);
    pts[1] = Ob_Set(sx * g_bm64c258, (double)sy * g_bm64c258, (double)sz * g_bm64c258);
    float r = (float)reach;
    float sgn = side ? -1.0f : 1.0f;
    const float* M = m.m;
    for (int k = 0; k < 3; k++)
    {
        memcpy(q + 4, &g_identity, 48);
        const Vec3& p = pts[k];
        float x = (float)((double)M[6] * p.z + (double)M[3] * p.y + (double)M[0] * p.x + M[9]);
        float y = (float)((double)M[7] * p.z + (double)M[4] * p.y + (double)M[1] * p.x + M[10]);
        float z = (float)((double)M[8] * p.z + (double)M[5] * p.y + (double)M[2] * p.x + M[11]);
        F(float, q, 76) = x;
        F(float, q, 80) = y;
        F(float, q, 84) = z;
        double t = k - 1;
        float tx = (float)(t * g_axisX.x), ty = (float)(t * g_axisX.y);
        double tz = t * g_axisX.z;
        float hx = (float)((double)tx * g_bm64c258), hy = (float)((double)ty * g_bm64c258);
        tz *= g_bm64c258;
        float ux = (float)((double)hx + g_axisY.x), uy = (float)((double)hy + g_axisY.y);
        tz += g_axisY.z;
        float dx = (float)((double)ux * r), dy = (float)((double)uy * r);
        tz *= r;
        float xr = (float)((double)dx * M[0] + tz * M[6] + (double)dy * M[3]);
        float yr = (float)((double)M[1] * dx + tz * M[7] + (double)dy * M[4]);
        float ex = (float)((double)xr * sgn), ey = (float)((double)yr * sgn);
        double ez = ((double)M[2] * dx + tz * M[8] + (double)dy * M[5]) * sgn;
        F(Vec3, q, 88) = Ob_Set((double)ex + x, (double)ey + y, ez + z);
        F(int, q, 116) = 0;
        F(int, q, 0) = 0;
        F(BYTE*, q, 104) = obj;
        F(BYTE*, q, 120) = car ? car + 16 : 0;
        if (!(CallVirt<int>(obj, 52, 4, q) & 1))
            continue;
        BYTE* o = F(BYTE*, q, 116);
        if (!o)
        {
            if (!(F(float, q, 72) < g_ob64b6e0))
                continue;
            Vec3 dv = Ob_Set((double)F(float, q, 52) - F(float, q, 76), (double)F(float, q, 56) - F(float, q, 80),
                             (double)F(float, q, 60) - F(float, q, 84));
            Ob_Hit(&flags, k, Vec3_Length(&dv), reach);
            continue;
        }
        const float* v = (const float*)(o + 416);
        Matrix om;
        const float* W = CallThis<const float*>(Sh_Matrix1, o, &om);
        Vec3 w = Ob_Set((double)v[2] * W[6] + (double)v[1] * W[3] + (double)v[0] * W[0],
                        (double)v[0] * W[1] + (double)v[2] * W[7] + (double)v[1] * W[4],
                        (double)v[0] * W[2] + (double)v[2] * W[8] + (double)v[1] * W[5]);
        const float* I = inv.m;
        Vec3 u = Ob_Set((double)I[6] * w.z + (double)I[3] * w.y + (double)I[0] * w.x,
                        (double)I[7] * w.z + (double)I[4] * w.y + (double)I[1] * w.x,
                        (double)I[8] * w.z + (double)I[5] * w.y + (double)I[2] * w.x);
        double hh = F(float, ai, 13100);
        Vec3 sv = Ob_Set(hh * g_axisY.x, hh * g_axisY.y, hh * g_axisY.z), tmp;
        CallThis(Ob_SubAssign, &u, &tmp, &sv);
        if (!(u.y < g_ob64c250))
            continue;
        double r2 = (double)u.y * u.y * g_ob64c970 + g_ob64d2f8;
        Matrix om2;
        Vec3 hp;
        Vec3_MulMatrix(&hp, (const Vec3*)(q + 52), CallThis<const Matrix*>(Sh_Matrix1, o, &om2));
        Vec3 dv = Ob_Set((double)F(float, q, 76) - hp.x, (double)F(float, q, 80) - hp.y, (double)F(float, q, 84) - hp.z);
        Ob_Hit(&flags, k, Vec3_Length(&dv), r2);
    }
    Vec3 c, s;
    const float* a0 = (const float*)(car + 3848);
    const float* b0 = (const float*)(car + 3860);
    Vec3_Add(&s, (const Vec3*)a0, (const Vec3*)b0);
    c = Ob_Set((double)s.x * g_bm64c258, (double)s.y * g_bm64c258, (double)s.z * g_bm64c258);
    CallThis(Ob_TransformPoint, &c, &m);
    ObSphere sp = {{c.x, c.y, c.z, r}};
    int cnt;
    CallC(Ob_Enum1, obj, sp, &cnt);
    for (int i = 0; i < cnt; i++)
    {
        float* pp;
        float d1[4], d2[4], d3[4], d4[4];
        BYTE* where;
        if (CallC<int>(Ob_Enum2, i, &pp, d1, d2, d3, d4, &where) < 1)
            continue;
        Vec3 pt = *(Vec3*)pp;
        if (where)
        {
            Matrix wm;
            Vec3 t;
            CallThis(Ob_IdentityThis, &wm);
            CallThis(Ob_GetWorld, where, &wm);
            pt = *Vec3_MulMatrix(&t, &pt, &wm);
        }
        CallThis(Ob_TransformPoint, &pt, &inv);
        if (x87_fabs(pt.x) > g_ob64b660 || pt.y < g_bm64c264 || pt.y > reach)
            continue;
        if (pt.y < g_ob64c40c)
            flags |= 8;
        if (!(pt.y < reach))
            continue;
        flags |= 4;
        flags |= pt.x > g_bm64c264 ? 2 : 1;
    }
    if ((flags & 16) && !(flags & 3))
        flags = (flags & ~16) | 2;
    return flags;
}

} // extern "C"

// === a particle emitter's step (FUN_4bf7a0)

KING_VAR(float, g_pe64c254, 64c254)
KING_VAR(float, g_pe64c48c, 64c48c)

static Vec3 Pe_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

// emitter point a (3 floats) turned by the view's roll (s +136 +2928)
static Vec3 Pe_Turn(BYTE* s, const float* a, bool second)
{
    Matrix rz;
    const float* R = Matrix_RotationZ(&rz, (float)F(double, F(BYTE*, s, 136), 2928))->m;
    if (!second)
        return Pe_Set((double)R[3] * a[1] + (double)R[6] * a[2] + (double)a[0] * R[0] + R[9],
                      (double)R[4] * a[1] + (double)R[1] * a[0] + (double)R[7] * a[2] + R[10],
                      (double)R[5] * a[1] + (double)R[2] * a[0] + (double)R[8] * a[2] + R[11]);
    return Pe_Set((double)R[3] * a[1] + (double)R[6] * a[2] + (double)R[0] * a[0] + R[9],
                  (double)R[1] * a[0] + (double)R[4] * a[1] + (double)R[7] * a[2] + R[10],
                  (double)R[2] * a[0] + (double)R[5] * a[1] + (double)R[8] * a[2] + R[11]);
}

extern "C" {

// [FUN_4bf7a0] steps emitter s over dt: moves its particles (+128, pooled at +124: next, position, velocity,
// life) and drops those under the ground or dead, then emits new ones every +20 seconds along its move from
// +100 to +112 (at most +44 alive)
void __fastcall Fx_EmitterStep(BYTE* s, void*, float dt)
{
    F(int, s, 40) = 0;
    Matrix gm;
    CallThis(Sh_Matrix1, F(BYTE*, g_game, 616) + 16, &gm);
    float ground = gm.m[11];
    BYTE* prev = 0;
    for (BYTE* p = F(BYTE*, s, 128); p;)
    {
        double vx = (double)dt * F(float, p, 16);
        float vy = (float)((double)dt * F(float, p, 20)), vz = (float)((double)dt * F(float, p, 24));
        F(float, p, 4) = (float)(vx + F(float, p, 4));
        F(float, p, 8) = (float)((double)vy + F(float, p, 8));
        F(float, p, 12) = (float)((double)vz + F(float, p, 12));
        F(float, p, 28) = (float)(F(float, p, 28) - (double)dt * F(float, s, 16));
        if ((double)ground - F(float, s, 56) < F(float, p, 12) && F(float, p, 28) > g_bm64c264)
        {
            F(float, p, 24) = (float)(F(float, p, 24) - (double)dt * F(float, s, 28));
            F(int, s, 40)++;
            prev = p;
            p = F(BYTE*, p, 0);
        }
        else
        {
            BYTE* next = F(BYTE*, p, 0);
            F(BYTE*, p, 0) = F(BYTE*, s, 124);
            F(BYTE*, s, 124) = p;
            if (prev)
                F(BYTE*, prev, 0) = next;
            if (p == F(BYTE*, s, 128))
                F(BYTE*, s, 128) = next;
            p = next;
        }
    }
    if (!F(int, s, 36))
    {
        F(Vec3, s, 100) = F(Vec3, s, 112);
        return;
    }
    Vec3 d = Pe_Set((double)F(float, s, 112) - F(float, s, 100), (double)F(float, s, 116) - F(float, s, 104),
                    (double)F(float, s, 120) - F(float, s, 108));
    Vec3 up = {0, 0, 1.0f}, v252, t;
    Vec3_Scaled(&v252, &g_axisY, F(float, g_7025b8, 532));
    {
        Matrix rz;
        const float* R = Matrix_RotationZ(&rz, (float)F(double, F(BYTE*, s, 136), 2928))->m;
        const float* Y = &g_axisY.x;
        const float* Z = &g_axisZ.x;
        double a = (double)Y[2] * R[6] + (double)Y[1] * R[3] + (double)Y[0] * R[0] + R[9];
        float wy = (float)((double)Y[2] * R[7] + (double)Y[1] * R[4] + (double)Y[0] * R[1] + R[10]);
        float wz = (float)((double)Y[2] * R[8] + (double)Y[1] * R[5] + (double)Y[0] * R[2] + R[11]);
        Vec3 c = Pe_Set((double)Z[2] * wy - (double)Z[1] * wz, (double)Z[0] * wz - Z[2] * a, Z[1] * a - (double)Z[0] * wy);
        Vec3 w = Pe_Set(a, wy, wz), u1, u2;
        Vec3_DivBy(&u1, &w, (float)x87_sqrt(Vec3_LengthSq(&w)));
        Vec3_DivBy(&u2, &c, (float)x87_sqrt(Vec3_LengthSq(&c)));
    }
    Vec3 e1 = Pe_Turn(s, (const float*)(s + 68), false);
    Vec3 e2 = Pe_Turn(s, (const float*)(s + 80), true);
    float rem = dt;
    if (!(dt > g_bm64c264))
        return;
    for (;;)
    {
        if (!(rem > F(float, s, 20)))
        {
            F(float, s, 20) = (float)(F(float, s, 20) - (double)rem);
            F(Vec3, s, 100) = F(Vec3, s, 112);
            return;
        }
        double step = F(float, s, 20);
        Vec3 dd = Pe_Set(d.x * step, d.y * step, d.z * step);
        Vec3 dv = Pe_Set(dd.x / (double)dt, dd.y / (double)dt, dd.z / (double)dt);
        F(float, s, 100) = (float)((double)dv.x + F(float, s, 100));
        F(float, s, 104) = (float)((double)dv.y + F(float, s, 104));
        F(float, s, 108) = (float)((double)dv.z + F(float, s, 108));
        rem = (float)((double)rem - F(float, s, 20));
        F(int, s, 20) = F(int, s, 12);
        BYTE* p = F(BYTE*, s, 124);
        if (p && F(int, s, 44) > F(int, s, 40))
        {
            double k = F(float, s, 52);
            Vec3 lift = Pe_Set(up.x * k, up.y * k, up.z * k);
            F(BYTE*, s, 124) = F(BYTE*, p, 0);
            Vec3* pos = (Vec3*)(p + 4);
            *pos = Pe_Set((double)lift.x + F(float, s, 100), (double)lift.y + F(float, s, 104), (double)lift.z + F(float, s, 108));
            double sp = F(float, s, 92);
            float r1 = (float)(rand() * ((double)g_ob64c250 - sp) * g_pe64c254 + sp);
            double r2 = rand() * ((double)g_ob64c250 - sp) * g_pe64c254 + sp;
            Vec3 a2 = Pe_Set(e2.x * r2, e2.y * r2, e2.z * r2);
            Vec3 a1 = Pe_Set((double)e1.x * r1, (double)e1.y * r1, (double)e1.z * r1), s1, s2;
            *pos = *Vec3_Sum(&s2, Vec3_Sum(&s1, pos, &a1), &a2);
            F(float, p, 28) = 1.0f;
            double k2 = F(float, s, 32);
            Vec3 drift = Pe_Set(v252.x * k2, v252.y * k2, v252.z * k2), sc, vs;
            float rs = (float)((rand() * (double)g_pe64c48c + g_ob64c250) * F(float, s, 24));
            F(Vec3, p, 16) = *Vec3_Sum(&vs, Vec3_ScaleBy(&sc, rs, &up), &drift);
            F(BYTE*, p, 0) = F(BYTE*, s, 128);
            F(BYTE*, s, 128) = p;
            F(int, s, 40)++;
        }
        if (!(rem > g_bm64c264))
            return;
    }
}

} // extern "C"

// === an explosion's step (a method at $L_4dca30)

extern "C" void Ex_PostEvent(); KING_ALIAS("_Ex_PostEvent", "@Queue_PostEvent@12")
extern "C" void Ex_Spawn(); KING_ALIAS("_Ex_Spawn", "_Fx_SpawnByKind")
KING_VAR(float, g_ex64d4c0, 64d4c0)
KING_VAR(float, g_ex64d550, 64d550)
KING_VAR(float, g_ex64c350, 64c350)
KING_VAR(float, g_ex64d3fc, 64d3fc)
KING_VAR(float, g_ex64c2fc, 64c2fc)
KING_VAR(float, g_ex64d4bc, 64d4bc)

static Vec3 Ex_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

static double Ex_Rand() { return rand() * (double)g_pe64c254; }

extern "C" {

// [$L_4dca30] explosion s's step: it drifts (+44 by +56), and while young (+36 below 6966fc) throws 5 pieces
// of debris with sparks from its frame (+80), then (below 6966f8) 15 puffs of smoke from +68; it ends at +32
void __fastcall Fx_ExplosionStep(BYTE* s, void*)
{
    BYTE* w = F(BYTE*, s, 40);
    F(float, s, 36) = (float)(F(double, w, 2960) + F(float, s, 36));
    double dt = F(double, w, 2960);
    Vec3 dv = Ex_Set(dt * F(float, s, 56), dt * F(float, s, 60), dt * F(float, s, 64));
    F(Vec3, s, 44) = Ex_Set((double)dv.x + F(float, s, 44), (double)dv.y + F(float, s, 48), (double)dv.z + F(float, s, 52));
    F(float, s, 132) = (float)(F(double, F(BYTE*, s, 40), 2960) * F(float, s, 128) + F(float, s, 132));
    const float* M = (const float*)(s + 80);
    if (F(float, s, 36) > g_fx6966fc)
    {
        float big = F(float, s, 136) > F(float, s, 140) ? F(float, s, 136) : F(float, s, 140);
        float half = (float)((double)big * g_bm64c258);
        big = F(float, s, 136) > F(float, s, 140) ? F(float, s, 136) : F(float, s, 140);
        float spread = (float)((double)big * g_ex64d4c0);
        for (int n = 5; n; n--)
        {
            float a = (float)Ex_Rand();
            double b = Ex_Rand() - a;
            Vec3 v60 = Ex_Set(b * g_axisY.x, b * g_axisY.y, b * g_axisY.z);
            Vec3 up = Ex_Set((double)v60.x * spread, (double)v60.y * spread, (double)v60.z * spread);
            float a2 = (float)Ex_Rand();
            double b2 = Ex_Rand() - a2;
            float sx = (float)(b2 * g_axisX.x), sy = (float)(b2 * g_axisX.y);
            Vec3 side = Ex_Set((double)sx * spread, (double)sy * spread, b2 * g_axisX.z * spread);
            Vec3 v120 = Ex_Set((double)side.x + F(float, s, 56), (double)side.y + F(float, s, 60), (double)side.z + F(float, s, 64));
            Vec3 vel = Ex_Set((double)v120.x + up.x, (double)v120.y + up.y, (double)v120.z + up.z);
            Matrix rz;
            const float* R = Matrix_RotationZ(&rz, (float)(rand() * (double)g_ex64d550))->m;
            const float* X = &g_axisX.x;
            Vec3 v72 = Ex_Set((double)X[1] * R[3] + (double)X[2] * R[6] + (double)X[0] * R[0] + R[9],
                              (double)X[1] * R[4] + (double)X[2] * R[7] + (double)X[0] * R[1] + R[10],
                              (double)X[1] * R[5] + (double)X[2] * R[8] + (double)X[0] * R[2] + R[11]);
            Vec3 off = Ex_Set((double)v72.x * half, (double)v72.y * half, (double)v72.z * half);
            Vec3 pos = Ex_Set((double)off.x + F(float, s, 44), (double)off.y + F(float, s, 48), (double)off.z + F(float, s, 52));
            Vec3 a264, b276;
            Fx_RaiseSegment(&pos, &vel, &a264, &b276);
            a264 = Ex_Set((double)a264.z * M[6] + (double)a264.y * M[3] + (double)a264.x * M[0] + M[9],
                          (double)a264.z * M[7] + (double)a264.y * M[4] + (double)a264.x * M[1] + M[10],
                          (double)a264.z * M[8] + (double)a264.y * M[5] + (double)a264.x * M[2] + M[11]);
            b276 = Ex_Set((double)b276.x * M[0] + (double)b276.z * M[6] + (double)b276.y * M[3],
                          (double)b276.z * M[7] + (double)b276.y * M[4] + (double)b276.x * M[1],
                          (double)b276.z * M[8] + (double)b276.y * M[5] + (double)b276.x * M[2]);
            int flag = Ex_Rand() > g_bm64c258 ? 1 : 0;
            Vec3 sp = Ex_Set((double)b276.x * g_ex64c350, (double)b276.y * g_ex64c350, (double)b276.z * g_ex64c350);
            float k1 = (float)((double)F(float, s, 140) * g_ex64c2fc), k2 = (float)((double)F(float, s, 140) * g_ex64d3fc);
            CallC(Ex_Spawn, &a264, &sp, k1, k2, 3.0f, 0.75f, flag);
            double r = Ex_Rand();
            if (F(float, s, 36) / ((double)g_fx696368 + g_fx6966fc) < r)
            {
                float k3 = (float)((double)F(float, s, 140) * g_ex64c2fc), k4 = (float)((double)F(float, s, 140) * g_ex64d4bc);
                CallC(Ex_Spawn, &a264, &b276, k3, k4, 3.0f, 0.5f, 2);
            }
        }
    }
    if (F(float, s, 36) > g_fx6966f8)
    {
        for (int n = 15; n; n--)
        {
            Vec3 a = F(Vec3, s, 68), t1, t2, t3, t4, b, r1, r2;
            Matrix rz;
            Matrix_RotationZ(&rz, (float)(rand() * (double)g_ex64d550));
            Vec3_Scaled(&t2, Vec3_MulMatrix(&t1, &g_axisX, &rz), 0.5f);
            b = *Vec3_Add(&t4, Vec3_Scaled(&t3, &g_axisZ, 1.3333334f), &t2);
            a = *Vec3_MulMatrix(&r1, &a, (const Matrix*)M);
            b = *Vec3_RotateBy(&r2, &b, (const Matrix*)M);
            int flag = Ex_Rand() > g_bm64c258 ? 1 : 0;
            float k = (float)((double)F(float, s, 140) * g_ex64c2fc);
            CallC(Ex_Spawn, &a, &b, 1.0f, k, 0, 3.0f, flag);
        }
    }
    if (F(float, s, 36) > F(float, s, 32))
        CallVirt(s, 28, 1);
    else
        CallThis(Ex_PostEvent, s, 1);
}

} // extern "C"

// === a reflection quad facing the view (a callback at $L_4cbfd0)

extern "C" void Rq_Find(); KING_ALIAS("_Rq_Find", "_Table124_Find")
extern "C" void Rq_Times(); KING_ALIAS("_Rq_Times", "_Vec3_Times")
extern "C" void Rq_Negate(); KING_ALIAS("_Rq_Negate", "_Vec3_Negate")
extern "C" void Rq_Set2Vec(); KING_ALIAS("_Rq_Set2Vec", "@Fx_Set2Vec@24")
extern "C" void Rq_AddReflection(); KING_ALIAS("_Rq_AddReflection", "@Fx_AddReflection@16")
KING_VAR(BYTE*, g_rq6961fc, 6961fc)               // the reflections
KING_VAR(char, s_rq672900, 672900)
KING_VAR(double, g_rq64b748, 64b748)
KING_VAR(BYTE*, g_rq6cec6c, 6cec6c)

static Vec3 Rq_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

static Vec3 Rq_Mul(const Vec3& v, double k) { return Rq_Set(v.x * k, v.y * k, v.z * k); }

// corner = pos + ((right * a + fwd * b) + up * c), by Vec3_Times / Vec3_Sum
static const Vec3* Rq_Corner(Vec3* out, const Vec3* pos, const Vec3* right, const Vec3* fwd, const Vec3* up, float a, float b, float c)
{
    Vec3 t1, t2, t3, s1, s2;
    CallC(Rq_Times, &t1, up, c);
    CallC(Rq_Times, &t2, fwd, b);
    CallC(Rq_Times, &t3, right, a);
    Vec3_Sum(&s1, &t3, &t2);
    Vec3_Sum(&s2, &s1, &t1);
    return Vec3_Sum(out, &s2, pos);
}

extern "C" {

// [$L_4cbfd0] a reflection of object obj (kind 22 at +60, its description +72: corner offsets, texture
// coordinates and the texture +84, found by name +80 when missing) as a quad facing view v; 1 when added
int __cdecl Fx_ReflectionQuad(int, BYTE* v, BYTE* obj)
{
    if (!obj || !v || !g_rq6961fc || F(int, g_rq6cec6c, 1300) == 2)
        return 0;
    BYTE* e = obj + 60;
    if (F(int, e, 8) != 22)
        return 0;
    BYTE* d = F(BYTE*, e, 12);
    int tex = F(int, d, 84);
    Vec3 pos = F(Vec3, obj, 20);
    if (!tex)
    {
        char name[48];
        BYTE tmp[48];
        sprintf(name, &s_rq672900, F(char*, d, 80));
        tex = CallC<int>(Rq_Find, name, F(int, obj, 36), tmp);
        if (!tex)
            return 0;
        F(int, d, 84) = tex;
    }
    Vec3 up = g_axisZ;
    Matrix m1, vm;
    Matrix_Inverse(&vm, Matrix_Multiply(&m1, (const Matrix*)(v + 5216 + F(int, v, 8236) * 48), (const Matrix*)(v + 4208)));
    Vec3 f, fwd, c, side;
    Vec3_RotateBy(&f, &g_axisY, &vm);
    Vec3_Normalized(&fwd, &f);
    c = Rq_Set((double)up.z * fwd.y - (double)up.y * fwd.z, (double)up.x * fwd.z - (double)up.z * fwd.x,
               (double)up.y * fwd.x - (double)up.x * fwd.y);
    if (Vec3_Length(&c) < g_rq64b748)
        side = g_axisX;
    else
    {
        Vec3 t;
        side = *Vec3_Normalized(&t, &c);
    }
    Vec3 right = side;
    BYTE verts[128];
    Vec3 neg = Rq_Set(-(double)fwd.x, -(double)fwd.y, -(double)fwd.z);
    for (int k = 0; k < 2; k++)
    {
        const float* o = (const float*)(d + k * 20);
        Vec3 a = Rq_Mul(up, o[2]), b = Rq_Mul(fwd, o[1]), cc = Rq_Mul(right, o[0]);
        Vec3 s1 = Rq_Set((double)b.x + cc.x, (double)b.y + cc.y, (double)b.z + cc.z);
        Vec3 s2 = Rq_Set((double)a.x + s1.x, (double)a.y + s1.y, (double)a.z + s1.z);
        Vec3 p = k ? Rq_Set((double)s2.x + pos.x, (double)s2.y + pos.y, (double)s2.z + pos.z)
                   : Rq_Set((double)s2.x + pos.x, (double)pos.y + s2.y, (double)pos.z + s2.z);
        float* r = (float*)(verts + k * 32);
        *(Vec3*)r = p;
        *(Vec3*)(r + 3) = neg;
        r[6] = F(float, d, 12 + k * 20);
        r[7] = F(float, d, 16 + k * 20);
        neg = Rq_Set(-(double)fwd.x, -(double)fwd.y, -(double)fwd.z);
    }
    for (int k = 0; k < 2; k++)
    {
        const float* o = (const float*)(d + 40 + k * 20);
        Vec3 n, corner;
        if (k)
            CallC(Rq_Negate, &n, &fwd);
        else
            n = neg;
        Rq_Corner(&corner, &pos, &right, &fwd, &up, o[0], o[1], o[2]);
        BYTE rec[32];
        memcpy(verts + 64 + k * 32, CallThis<BYTE*>(Rq_Set2Vec, rec, &corner, &n, F(int, o, 12), F(int, o, 16)), 32);
    }
    CallThis(Rq_AddReflection, g_rq6961fc, verts, tex);
    return 1;
}

} // extern "C"

// === a burst's step (a method at $L_4da7e0)

KING_VAR(float, g_bs696700, 696700)
KING_VAR(float, g_bs69672c, 69672c)
KING_VAR(float, g_bs69637c, 69637c)
KING_VAR(float, g_bs64d3f4, 64d3f4)
KING_VAR(float, g_bs64d498, 64d498)

extern "C" {

// [$L_4da7e0] burst s's step: it drifts (+44 by +56), and while young (+36 below 696700) throws 15 burning
// pieces from below its centre, then (below 69637c) 15 puffs of smoke from +68; it ends at +32
void __fastcall Fx_BurstStep(BYTE* s, void*)
{
    BYTE* w = F(BYTE*, s, 40);
    const float* V = (const float*)(s + 56);
    F(float, s, 36) = (float)(F(double, w, 2960) + F(float, s, 36));
    double dt = F(double, w, 2960);
    Vec3 dv = Ex_Set(dt * V[0], dt * V[1], dt * V[2]);
    F(Vec3, s, 44) = Ex_Set((double)dv.x + F(float, s, 44), (double)dv.y + F(float, s, 48), (double)dv.z + F(float, s, 52));
    F(float, s, 84) = (float)(F(double, F(BYTE*, s, 40), 2960) * F(float, s, 80) + F(float, s, 84));
    if (F(float, s, 36) > g_bs696700)
    {
        float big = F(float, s, 88) > F(float, s, 92) ? F(float, s, 88) : F(float, s, 92);
        float half = (float)((double)big * g_bm64c258);
        big = F(float, s, 88) > F(float, s, 92) ? F(float, s, 88) : F(float, s, 92);
        float spread = (float)((double)big * g_ex64d4c0);
        for (int n = 15; n; n--)
        {
            float a = (float)Ex_Rand();
            double b = Ex_Rand() - a;
            Vec3 v132 = Ex_Set(b * g_axisY.x, b * g_axisY.y, b * g_axisY.z);
            Vec3 up = Ex_Set((double)v132.x * spread, (double)v132.y * spread, (double)v132.z * spread);
            float a2 = (float)Ex_Rand();
            double b2 = Ex_Rand() - a2;
            Vec3 v144 = Ex_Set(b2 * g_axisX.x, b2 * g_axisX.y, b2 * g_axisX.z);
            Vec3 side = Ex_Set((double)v144.x * spread, (double)v144.y * spread, (double)v144.z * spread);
            Vec3 v96 = Ex_Set((double)side.x + V[0], (double)side.y + V[1], (double)side.z + V[2]);
            Vec3 vel = Ex_Set((double)v96.x + up.x, (double)v96.y + up.y, (double)v96.z + up.z);
            Matrix rz;
            const float* R = Matrix_RotationZ(&rz, (float)(rand() * (double)g_bs64d3f4 * g_bs64d498))->m;
            const float* X = &g_axisX.x;
            Vec3 v84 = Ex_Set((double)X[2] * R[6] + (double)X[1] * R[3] + (double)X[0] * R[0] + R[9],
                              (double)X[2] * R[7] + (double)X[1] * R[4] + (double)X[0] * R[1] + R[10],
                              (double)X[2] * R[8] + (double)X[1] * R[5] + (double)X[0] * R[2] + R[11]);
            Vec3 off = Ex_Set((double)v84.x * half, (double)v84.y * half, (double)v84.z * half);
            double h = F(float, s, 92);
            Vec3 low = Ex_Set(h * g_axisZ.x, h * g_axisZ.y, h * g_axisZ.z);
            Vec3 v156 = Ex_Set((double)F(float, s, 44) - low.x, (double)F(float, s, 48) - low.y, (double)F(float, s, 52) - low.z);
            Vec3 pos = Ex_Set((double)v156.x + off.x, (double)v156.y + off.y, (double)v156.z + off.z);
            int flag = Ex_Rand() > g_bm64c258 ? 1 : 0;
            Vec3 sp = Ex_Set((double)vel.x * g_ex64c350, (double)vel.y * g_ex64c350, (double)vel.z * g_ex64c350);
            float k1 = (float)((double)F(float, s, 92) * g_ex64c2fc), k2 = (float)((double)F(float, s, 92) * g_ex64d3fc);
            CallC(Ex_Spawn, &pos, &sp, k1, k2, 3.0f, 0.75f, flag);
            double r = Ex_Rand();
            if (F(float, s, 36) / ((double)g_bs69672c + g_bs696700) < r)
            {
                float k3 = (float)((double)F(float, s, 92) * g_ex64c2fc), k4 = (float)((double)F(float, s, 92) * g_ex64d4bc);
                CallC(Ex_Spawn, &pos, &vel, k3, k4, 3.0f, 0.5f, 2);
            }
        }
    }
    if (F(float, s, 36) > g_bs69637c)
    {
        for (int n = 15; n; n--)
        {
            Vec3 a = F(Vec3, s, 68), t1, u, b;
            Matrix rz;
            Matrix_RotationZ(&rz, (float)(rand() * (double)g_bs64d3f4 * g_bs64d498));
            const Vec3* t = Vec3_MulMatrix(&t1, &g_axisX, &rz);
            u = Ex_Set((double)t->x * g_bm64c258, (double)t->y * g_bm64c258, (double)t->z * g_bm64c258);
            Vec3_Add(&b, (const Vec3*)V, &u);
            int flag = Ex_Rand() > g_bm64c258 ? 1 : 0;
            float k = (float)((double)F(float, s, 92) * g_ex64c2fc);
            CallC(Ex_Spawn, &a, &b, 1.0f, k, 0, 3.0f, flag);
        }
    }
    if (F(float, s, 36) > F(float, s, 32))
        CallVirt(s, 28, 1);
    else
        CallThis(Ex_PostEvent, s, 1);
}

} // extern "C"

// === a glare facing the camera (a method at $L_4c5060)

extern "C" void Gl_DrawAt(); KING_ALIAS("_Gl_DrawAt", "@Sprite_DrawStrip@24")
extern "C" void Gl_TestInfo(); KING_ALIAS("_Gl_TestInfo", "@Scene_TestInfo@16")
extern "C" int __fastcall View_SphereClip(BYTE* s, void*, const float* sp);
KING_VAR(float, g_gl64c874, 64c874)
KING_VAR(float, g_gl64c878, 64c878)
KING_VAR(float, g_gl671704, 671704)
KING_VAR(float, g_gl671708, 671708)
KING_VAR(float, g_gl67170c, 67170c)
KING_VAR(float, g_gl671710, 671710)
KING_VAR(float, g_gl671714, 671714)
KING_VAR(float, g_gl671718, 671718)

struct Gl_V3 { float x, y, z; };

static double Gl_Clamp01(double t)
{
    if (t > g_eff64c250)
        return g_eff64c250;
    if (t < g_eff64c264b)
        return g_eff64c264b;
    return t;
}

extern "C" {

// [$L_4c5060] glare s on view v: its quad (object +52: corners +16 / +20, texture +8 / +12) sized by the
// camera's distance (+80 / +84 between +92 and +88), of kind +68 (0 swirling, 1 leaning with the view), placed
// between +104 and the camera at +96 .. +100 (+56), drawn faded by distance when seen; 1 when drawn
int __fastcall Fx_GlareDraw(BYTE* s, void*, int info, BYTE* v)
{
    BYTE* w = g_q22_696250;
    float cam[3] = {(float)F(double, w, 2904), (float)F(double, w, 2912), (float)F(double, w, 2920)};
    Matrix rz;
    const float* m = Matrix_RotationZ(&rz, (float)F(double, w, 2928))->m;
    const float* Y = &g_axisY.x;
    float fx = (float)((double)Y[2] * m[6] + (double)Y[1] * m[3] + (double)Y[0] * m[0] + m[9]);
    float fy = (float)((double)Y[2] * m[7] + (double)Y[1] * m[4] + (double)Y[0] * m[1] + m[10]);
    float fz = (float)((double)Y[2] * m[8] + (double)Y[1] * m[5] + (double)Y[0] * m[2] + m[11]);
    float* P = (float*)(s + 56);
    Vec3 d, n;
    d.x = (float)((double)cam[0] - P[0]);
    d.y = (float)((double)cam[1] - P[1]);
    d.z = (float)((double)cam[2] - P[2]);
    Vec3_Normalized(&n, &d);
    double c = (double)n.z * fz + (double)n.y * fy + (double)n.x * fx;
    float sn = (float)x87_sqrt(g_eff64c250 - c * c);
    double dx = (double)cam[0] - P[0], dy = (double)cam[1] - P[1];
    if (dy * fx - fy * dx > g_eff64c264b)
        sn = (float)-(double)sn;
    double ez = (double)cam[2] - F(float, s, 64), ey = (double)cam[1] - F(float, s, 60), ex = (double)cam[0] - P[0];
    float d2 = (float)(ex * ex + ey * ey + ez * ez);
    double tc = Gl_Clamp01(((double)d2 - F(float, s, 84)) / ((double)F(float, s, 80) - F(float, s, 84)));
    double ang = F(double, w, 2928) * F(float, s, 76) + F(float, s, 72);
    float c16[4] = {0, 0, 0, 0}, c20[4] = {0, 0, 0, 0}, t8[4] = {0, 0, 0, 0}, t12[4] = {0, 0, 0, 0};
    int kind = F(int, s, 68);
    if (kind == 0)
    {
        double sz = ((double)F(float, s, 88) - F(float, s, 92)) * tc + F(float, s, 92);
        float a = (float)(sz * g_gl64c878), b = (float)(sz * g_eff64c258b);
        c20[0] = c20[1] = c16[0] = c16[3] = a;
        c20[3] = c20[2] = c16[2] = c16[1] = b;
        const double base[3] = {g_gl671704, g_gl671710, g_gl671708};
        for (int k = 0; k < 4; k++)
        {
            double ak = k < 3 ? base[k] + ang : ang + g_gl67170c;
            t8[k] = (float)(x87_cos(ak) * g_gl671714 + g_eff64c258b);
            t12[k] = (float)(x87_sin(ak) * g_gl671718 + g_eff64c258b);
        }
    }
    else if (kind == 1)
    {
        double sz = ((double)F(float, s, 88) - F(float, s, 92)) * tc + F(float, s, 92);
        DWORD tb[4] = {0x3ee66666, 0x3f0ccccd, 0x3f0ccccd, 0x3ee66666};
        memcpy(t8, tb, 16);
        t12[0] = t12[1] = 1.0f;
        float a = (float)(sz * g_gl64c878);
        c20[0] = c20[1] = a;
        float half = (float)(sz * g_eff64c258b);
        c20[3] = c20[2] = half;
        c16[1] = (float)(((double)sn + g_gl64c874) * half);
        c16[0] = (float)((double)half * sn - (double)half * g_gl64c874);
        c16[3] = (float)-(double)c16[1];
        c16[2] = (float)-(double)c16[0];
    }
    float r = (float)(((double)F(float, s, 100) - F(float, s, 96)) * tc + F(float, s, 96));
    Vec3 e, u;
    e.x = (float)((double)cam[0] - F(float, s, 104));
    e.y = (float)((double)cam[1] - F(float, s, 108));
    e.z = (float)((double)cam[2] - F(float, s, 112));
    Vec3_Normalized(&u, &e);
    float uy = (float)((double)r * u.y), uz = (float)((double)r * u.z);
    P[0] = (float)((double)r * u.x + F(float, s, 104));
    P[1] = (float)((double)uy + F(float, s, 108));
    P[2] = (float)((double)uz + F(float, s, 112));
    BYTE* o = F(BYTE*, s, 52);
    for (int k = 0; k < 4; k++)
    {
        F(float*, o, 16)[k] = c16[k];
        F(float*, o, 20)[k] = c20[k];
        F(float*, o, 8)[k] = t8[k];
        F(float*, o, 12)[k] = t12[k];
    }
    CallVirt(s, 72);
    if (!View_SphereClip(v, 0, (const float*)(s + 20)) || !CallThis<int>(Gl_TestInfo, s, info, v))
    {
        s[40] |= 1;
        return 0;
    }
    double h = (double)F(float, v, 8540) * g_eff64c258b;
    F(float, F(BYTE*, F(BYTE*, s, 52), 36), 88) = (float)Gl_Clamp01((x87_sqrt(d2) - h) / ((double)F(float, v, 8536) - h));
    F(DWORD, F(BYTE*, F(BYTE*, s, 52), 36), 32) |= 32;
    Gl_V3 p = *(const Gl_V3*)P;
    CallThis(Gl_DrawAt, v, F(BYTE*, s, 52), p);
    F(DWORD, F(BYTE*, F(BYTE*, s, 52), 36), 32) &= ~32u;
    s[40] |= 1;
    return 1;
}

} // extern "C"

// === a fire's step (a method at $L_4d9f40)

extern "C" void Fr_PostEvent(); KING_ALIAS("_Fr_PostEvent", "@Queue_PostEvent@12")
KING_VAR(float, g_fr64d350, 64d350)
KING_VAR(float, g_fr64c40c, 64c40c)
KING_VAR(float, g_fr64c48c, 64c48c)
KING_VAR(float, g_fr64c330, 64c330)
KING_VAR(float, g_fr64c304, 64c304)
KING_VAR(float, g_fr64c310, 64c310)
KING_VAR(float, g_fr64d380, 64d380)
KING_VAR(float, g_fr64c334, 64c334)
KING_VAR(float, g_fr64c258, 64c258)
KING_VAR(float, g_fr64c264, 64c264)
KING_VAR(float, g_fr695fb0, 695fb0)
KING_VAR(float, g_fr695328, 695328)
KING_VAR(float, g_fr695e30, 695e30)

extern "C" double __cdecl floor(double);

static double Fr_Rand() { return rand() * (double)g_randScale; }

static double Fr_Clamp(double t)
{
    if (t < g_fr64c264)
        return g_fr64c264;
    if (t > g_one_f)
        return g_one_f;
    return t;
}

extern "C" {

// [$L_4d9f40] fire s's step: gone when the frame rate is low and the camera close; it burns for +60 (age +64),
// its wind (+76 .. +84, eased from +88 .. at +72 of +68) blows sparks (+100 set) or smoke (+104) from +36 / +48,
// and it lights the thrown lamp with flickering colours; it ends with its time
void __fastcall Fx_FireStep(BYTE* s, void*)
{
    BYTE* w = F(BYTE*, s, 32);
    if ((double)g_one_f / F(double, w, 2960) < g_fr64d350)
    {
        float cx = (float)F(double, w, 2904);
        float cy = (float)F(double, w, 2912), cz = (float)F(double, w, 2920);
        Vec3 d;
        d.x = (float)((double)cx - F(float, s, 36));
        d.y = (float)((double)cy - F(float, s, 40));
        d.z = (float)((double)cz - F(float, s, 44));
        if (Vec3_Length(&d) < g_fr64c40c)
        {
            CallVirt(s, 28, 1);
            return;
        }
    }
    double age = F(double, w, 2960) + F(float, s, 64);
    F(float, s, 64) = (float)age;
    float t = (float)(((double)F(float, s, 60) - age) / F(float, s, 60));
    if (t < g_fr64c264)
        t = 0;
    else if (t > g_one_f)
        t = 1.0f;
    double ph = F(double, w, 2960) + F(float, s, 72);
    F(float, s, 72) = (float)ph;
    if (ph > F(float, s, 68))
    {
        F(float, s, 72) = 0;
        F(float, s, 88) = F(float, s, 76);
        F(float, s, 92) = F(float, s, 80);
        F(float, s, 96) = F(float, s, 84);
        float r1 = (float)Fr_Rand();
        F(float, s, 76) = (float)((Fr_Rand() - r1) * 2);
        r1 = (float)Fr_Rand();
        F(float, s, 80) = (float)((Fr_Rand() - r1) * 2);
        F(float, s, 84) = (float)(rand() * (double)g_fr64c48c + g_one_f);
    }
    double u = Fr_Clamp(((double)F(float, s, 68) - F(float, s, 72)) / F(float, s, 68));
    double iu = g_one_f - u;
    float A = (float)(iu * F(float, s, 88) + u * F(float, s, 76));
    float B = (float)(iu * F(float, s, 92) + u * F(float, s, 80));
    double C = iu * F(float, s, 96) + u * F(float, s, 84);
    double g = t > g_fr64c330 ? t : g_fr64c330;
    float yy = (float)(g_axisY.y * (double)B), yz = (float)(g_axisY.z * (double)B);
    double yx = g_axisY.x * (double)B;
    float xy = (float)(g_axisX.y * (double)A), xz = (float)(g_axisX.z * (double)A);
    double xx = g_axisX.x * (double)A;
    float vx = (float)(xx + F(float, s, 48) + yx);
    float vy = (float)((double)yy + (float)((double)xy + F(float, s, 52)));
    float vz = (float)((double)yz + (float)((double)xz + F(float, s, 56)));
    bool sparks = F(int, s, 100) != 0;
    double g2 = g + (sparks ? g_fr64c304 : g_fr64c310);
    Vec3 vel;
    vel.x = (float)((float)(vx * g2) * C);
    vel.y = (float)((float)(vy * g2) * C);
    vel.z = (float)(vz * g2 * C);
    if (sparks)
        Fx_SparkOrFlash((int)(s + 36), &vel, 1.0f);
    else
        Fx_SpawnPuff((int)(s + 36), (int)&vel, F(float, s, 104));
    if (!g_696734)
        Fx_ThrowLamp(s, 0);
    if (g_696734)
    {
        unsigned i = (unsigned)(int)floor(F(float, s, 64) / (double)g_fr64d380) % 31;
        double k;
        if (t > g_fr64c258)
            k = t;
        else if (t <= g_fr64c334)
            k = g_fr64c258;
        else
            k = (double)t + g_fr64c334;
        F(float, g_696734, 108) = (float)(k * (&g_fr695fb0)[i]);
        F(float, g_696734, 112) = (float)(k * (&g_fr695328)[i]);
        F(float, g_696734, 116) = (float)(k * (&g_fr695e30)[i]);
    }
    if (F(float, s, 64) > F(float, s, 60))
        CallVirt(s, 28, 1);
    else
        CallThis(Fr_PostEvent, s, 1);
}

} // extern "C"

// === drifting smoke sheets (FUN_4c03d0)

KING_VAR(float, g_ds64c494, 64c494)
KING_VAR(float, g_ds64c334, 64c334)
KING_VAR(float, g_ds64c258, 64c258)
KING_VAR(float, g_ds64c490, 64c490)
KING_VAR(BYTE*, g_ds7025b8, 7025b8)

static Vec3 Ds_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_4c03d0] effect s's puffs (list +128, positions at +4) in front of the camera (farther than +64 along its
// view) drawn on r as three-vertex sheets (colour +96) drifting with the wind, raised by +8 and widened with
// distance
int __fastcall Fx_SmokeSheets(BYTE* s, void*, BYTE* r)
{
    BYTE* w = F(BYTE*, s, 136);
    Matrix rz;
    const float* R = Matrix_RotationZ(&rz, (float)F(double, w, 2928))->m;
    const float* Y = &g_axisY.x;
    const float* Z = &g_axisZ.x;
    Vec3 fv = Ds_Set((double)Y[2] * R[6] + (double)Y[1] * R[3] + (double)Y[0] * R[0] + R[9],
                     (double)Y[2] * R[7] + (double)Y[1] * R[4] + (double)Y[0] * R[1] + R[10],
                     (double)Y[2] * R[8] + (double)Y[1] * R[5] + (double)Y[0] * R[2] + R[11]);
    float cam[3] = {(float)F(double, w, 2904), (float)F(double, w, 2912), (float)F(double, w, 2920)};
    Vec3 sd = Ds_Set((double)fv.z * Z[1] - (double)fv.y * Z[2], (double)fv.x * Z[2] - (double)fv.z * Z[0],
                     (double)fv.y * Z[0] - (double)fv.x * Z[1]);
    Vec3 wind, tmp;
    Vec3_Scaled(&wind, &g_axisY, F(float, g_ds7025b8, 532));
    Vec3 wv = Ds_Set(wind.x * (double)g_ds64c494, wind.y * (double)g_ds64c494, wind.z * (double)g_ds64c494);
    float plane = (float)-((double)cam[1] * fv.y + (double)cam[2] * fv.z + (double)fv.x * cam[0]);
    float h = (float)-(double)F(float, s, 8);
    Vec3 A = Ds_Set((double)wv.x + 0, (double)wv.y + 0, (double)wv.z + h);
    Vec3 B = A;
    double k = F(float, s, 8);
    sd = Ds_Set(k * sd.x, sd.y * k, sd.z * k);
    Vec3 up = Ds_Set(0, 0, F(float, s, 8));
    Renderer_ResetCount(r, 0);
    for (BYTE* e = F(BYTE*, s, 128); e; e = F(BYTE*, e, 0))
    {
        const Vec3* p = (const Vec3*)(e + 4);
        double d = (double)p->y * fv.y + (double)p->z * fv.z + (double)p->x * fv.x + plane;
        if (!(d > F(float, s, 64)))
            continue;
        float t = (float)(d * g_ds64c494 + g_ds64c334);
        double u = d * g_ds64c258 + g_ds64c490;
        Vec3 v1 = Ds_Set(up.x * u, up.y * u, up.z * u), q, q2;
        const DWORD* col = (const DWORD*)(s + 96);
        Renderer_AddVertex(r, 0, Vec3_Sum(&q, p, &v1), col, 0.5f, 0);
        Vec3 v2 = Ds_Set((double)sd.x * t, (double)sd.y * t, (double)sd.z * t);
        Renderer_AddVertex(r, 0, Vec3_Sum(&q2, Vec3_Sum(&q, p, &A), &v2), col, 1.0f, 1.0f);
        Vec3 tm;
        Vec3_Times(&tm, &sd, t);
        Renderer_AddVertex(r, 0, Vec3_Diff(&q2, Vec3_Sum(&q, p, &B), &tm), col, 0, 1.0f);
    }
    Effect_Zero();
    return 0;
}

} // extern "C"

// === the ground mist's step (a method at $L_4befa0)

extern "C" void Gm_PostEvent(); KING_ALIAS("_Gm_PostEvent", "@Queue_PostEvent@12")
KING_VAR(float, g_gm64c338, 64c338)
KING_VAR(float, g_gm64c488, 64c488)
KING_VAR(float, g_gm64c484, 64c484)
KING_VAR(float, g_gm64c458, 64c458)
KING_VAR(double, g_gm64b7a8, 64b7a8)
KING_VAR(float, g_gm64c308, 64c308)
KING_VAR(float, g_gm64c454, 64c454)
KING_VAR(float, g_gm64c450, 64c450)
KING_VAR(float, g_gm64c1f0, 64c1f0)
KING_VAR(float, g_gm64c320, 64c320)
KING_VAR(float, g_gm64c44c, 64c44c)
KING_VAR(float, g_gm64c448, 64c448)
KING_VAR(float, g_gm64c144, 64c144)
KING_VAR(float, g_gm64c148, 64c148)
KING_VAR(BYTE*, g_gm7025b8, 7025b8)

extern "C" {

// [$L_4befa0] the mist s (shown with +48 and its switch +32 +80 selected): its emitter (+36) placed ahead of the
// camera by the game's view distance, coloured from the palette (lighter, more saturated), enabled by the wave
// and the season (6716f8), stepped and drawn with sheets, its particle count (+44, 50 .. 2000) by the frame rate
void __fastcall Fx_MistStep(BYTE* s, void*)
{
    if (!F(int, s, 48))
    {
        CallThis(Gm_PostEvent, s, 1);
        return;
    }
    BYTE* sw = F(BYTE*, F(BYTE*, s, 32), 80);
    if (sw && !CallC<int>(Rn_Selected, sw))
    {
        CallThis(Gm_PostEvent, s, 1);
        return;
    }
    BYTE* w = F(BYTE*, s, 44);
    Matrix rz;
    const float* R = Matrix_RotationZ(&rz, (float)F(double, w, 2928))->m;
    const float* Y = &g_axisY.x;
    float fx = (float)((double)Y[2] * R[6] + (double)Y[1] * R[3] + (double)Y[0] * R[0] + R[9]);
    float fy = (float)((double)Y[2] * R[7] + (double)Y[1] * R[4] + (double)Y[0] * R[1] + R[10]);
    float fz = (float)((double)Y[2] * R[8] + (double)Y[1] * R[5] + (double)Y[0] * R[2] + R[11]);
    Vec3 vv = F(Vec3, F(BYTE*, g_game, 616), 432);
    double L = Vec3_Length(&vv);
    float cz = (float)F(double, w, 2920), cy = (float)F(double, w, 2912), cx = (float)F(double, w, 2904);
    float ky = (float)(fy * (double)g_gm64c338), kz = (float)(fz * (double)g_gm64c338);
    float px = (float)(fx * (double)g_gm64c338 * L);
    float py = (float)(ky * L);
    Vec3 pos;
    pos.x = (float)((double)px + cx);
    pos.y = (float)((double)py + cy);
    pos.z = (float)(L * kz + cz);
    float wv = (float)Wave_Value(g_gm7025b8, 0, 0.0);
    BYTE* pal = CallThis<BYTE*>(FUN_5b6700, F(BYTE*, w, 12)) + 48;
    float r = (float)(pal[0] * (double)g_gm64c488), g = (float)(pal[1] * (double)g_gm64c488), b = (float)(pal[2] * (double)g_gm64c488);
    float h, l, sat;
    Color_ToHsl(r, g, b, &h, &l, &sat);
    sat = (float)(sat * (double)g_64c258);
    l = (float)(l + (double)g_gm64c484);
    if (l > g_one_f)
        l = 1.0f;
    else if (l < g_eff64c264b)
        l = 0;
    Color_FromHsl(h, l, sat, &r, &g, &b);
    DWORD c = (DWORD)(int)(r * (double)g_gm64c458) | 0xFFFFFF00u;
    c = (c << 8) | (DWORD)(int)(g * (double)g_gm64c458);
    c = (c << 8) | (DWORD)(int)(b * (double)g_gm64c458);
    BYTE* e = F(BYTE*, s, 36);
    F(DWORD, e, 96) = c;
    if (wv < g_gm64b7a8)
        Renderer_SetEnabled(e, 0, 0);
    else
    {
        Renderer_SetEnabled(e, 0, 1);
        double x = (wv - (double)g_gm64c308) * g_gm64c454;
        if (x < g_64c258)
            x = g_64c258;
        double y = g_one_f - x;
        F(float, e, 12) = (float)((y * y * g_gm64c450 + g_one_f) * g_gm64c1f0);
    }
    double z = F(double, w, 2920);
    if (z > g_gm64c144 && z < g_gm64c148)
        g_fi6716f8 = 1;
    else
    {
        g_fi6716f8 = 0;
        Renderer_SetEnabled(F(BYTE*, s, 36), 0, 0);
    }
    float dt = (float)F(double, F(BYTE*, s, 44), 2960);
    Particles_SetOrigin(F(BYTE*, s, 36), 0, &pos);
    Fx_EmitterStep(F(BYTE*, s, 36), 0, dt);
    Fx_SmokeSheets(F(BYTE*, s, 36), 0, F(BYTE*, s, 32));
    CallThis(Gm_PostEvent, s, 1);
    double fps = g_one_f / (double)dt;
    BYTE* em = F(BYTE*, s, 36);
    if (fps > g_gm64c320)
        F(int, em, 44) = (int)((fps - g_gm64c320) * g_gm64c44c + g_gm64c448);
    else
        F(int, em, 44) = 50;
    if (F(int, em, 44) < 50)
        F(int, em, 44) = 50;
    else if (F(int, em, 44) > 2000)
        F(int, em, 44) = 2000;
}

} // extern "C"

// === where an emitter source puts its next particle (a method at $L_4c1280)

extern "C" void Es_GroundHeight(); KING_ALIAS("_Es_GroundHeight", "@Terrain_HeightAt@16")
KING_VAR(BYTE*, g_es696224, 696224)
KING_VAR(float, g_es64c1f8, 64c1f8)
KING_VAR(float, g_es64c624, 64c624)
KING_VAR(float, g_es64c264, 64c264)

static Vec3 Es_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [$L_4c1280] source i of emitter s (92-byte records at +8: point, normal +12, kind +24, mesh +28 / +32, frame
// +36, on +84, owner +88): when on, the game's current one and near the camera, a point (*p) and direction (*n)
// by kind: 0 the point, 5 a random spot of the disc about the normal, 6 a random point of one of the mesh's
// triangles, 8 a random spot on the ground (straight up); 1 when given
int __fastcall Fx_SourcePoint(BYTE* s, void*, int i, Vec3* p, Vec3* n)
{
    BYTE* E = s + 8 + i * 92;
    if (!F(int, E, 84) || F(int, E, 88) != F(int, F(BYTE*, g_game, 616), 308))
        return 0;
    BYTE* w = g_es696224;
    double cx = F(double, w, 2904);
    float cy = (float)F(double, w, 2912), cz = (float)F(double, w, 2920);
    const float* P = (const float*)E;
    Vec3 d = Es_Set(P[0] - cx, (double)P[1] - cy, (double)P[2] - cz);
    if (!(Vec3_Length(&d) < g_es64c1f8))
        return 0;
    int kind = F(int, E, 24);
    const float* N = (const float*)(E + 12);
    if (kind == 5)
    {
        float u = (float)(rand() * (double)g_es64c624);
        double v = rand() * (double)g_es64c624;
        double z0 = g_es64c264;
        float a44 = (float)(z0 - N[2]), a40 = (float)(N[1] - z0), a36 = (float)(N[2] - z0);
        float zero = 0;
        Vec3 t1 = Es_Set((double)a36 * v, (double)zero * v, (z0 - N[0]) * v);
        Vec3 t2 = Es_Set((double)zero * u, (double)a44 * u, (double)a40 * u);
        Vec3 t3 = Es_Set((double)t2.x + P[0], (double)t2.y + P[1], (double)t2.z + P[2]);
        *p = Es_Set((double)t3.x + t1.x, (double)t3.y + t1.y, (double)t3.z + t1.z);
        *n = *(const Vec3*)N;
    }
    if (kind == 6)
    {
        *n = *(const Vec3*)N;
        BYTE* T = F(BYTE*, E, 32) + F(int, E, 28) * 48 - 48;
        if (F(WORD, T, 2) >= 3)
        {
            int k = (int)(rand() * (double)(F(WORD, T, 2) - 3) * g_randScale + g_one_f);
            const float* vs = F(const float*, T, 4);
            const float* M = (const float*)(E + 36);
            const float* v = vs + (k + 1) * 3;
            Vec3 p1 = Es_Set((double)M[6] * v[2] + (double)M[3] * v[1] + (double)v[0] * M[0] + M[9],
                             (double)M[1] * v[0] + (double)M[7] * v[2] + (double)M[4] * v[1] + M[10],
                             (double)M[2] * v[0] + (double)M[8] * v[2] + (double)M[5] * v[1] + M[11]);
            Vec3 p2, p3, r;
            Vec3_MulMatrix(&p2, (const Vec3*)(vs + k * 3), (const Matrix*)M);
            Vec3_MulMatrix(&p3, (const Vec3*)vs, (const Matrix*)M);
            *p = *Triangle_RandomPoint(&r, &p3, &p2, &p1);
        }
        else
            *p = *(const Vec3*)P;
    }
    if (kind == 8)
    {
        p->x = (float)(rand() * (double)g_es64c624 + P[0]);
        p->y = (float)(rand() * (double)g_es64c624 + P[1]);
        p->z = (float)CallThis<double>(Es_GroundHeight, F(BYTE*, E, 32), p->x, p->y);
        n->x = 0;
        n->y = 0;
        n->z = 1.0f;
    }
    if (kind == 0)
    {
        *p = *(const Vec3*)P;
        *n = *(const Vec3*)N;
    }
    return 1;
}

} // extern "C"

// === the light shafts' step (a method at $L_4bea60)

KING_VAR(float, g_ls64c460, 64c460)
KING_VAR(float, g_ls64c45c, 64c45c)
KING_VAR(float, g_ls64c334, 64c334)
KING_VAR(float, g_ls64c1ec, 64c1ec)
KING_VAR(float, g_ls64c13c, 64c13c)
KING_VAR(float, g_ls64c140, 64c140)
KING_VAR(int, g_ls6716fc, 6716fc)

extern "C" {

// [$L_4bea60] the shafts s: their emitter (+36) placed ahead of the camera by the game's view distance, grey by
// the weather's brightness (7025b8 +548), enabled by the wave and the season (6716fc), stepped and drawn as
// shafts, its particle count (+44, 50 .. 2000) by the frame rate
void __fastcall Fx_ShaftsStep(BYTE* s, void*)
{
    BYTE* w = F(BYTE*, s, 40);
    Matrix rz;
    const float* R = Matrix_RotationZ(&rz, (float)F(double, w, 2928))->m;
    const float* Y = &g_axisY.x;
    float fx = (float)((double)Y[2] * R[6] + (double)Y[1] * R[3] + (double)Y[0] * R[0] + R[9]);
    float fy = (float)((double)Y[2] * R[7] + (double)Y[1] * R[4] + (double)Y[0] * R[1] + R[10]);
    float fz = (float)((double)Y[2] * R[8] + (double)Y[1] * R[5] + (double)Y[0] * R[2] + R[11]);
    Vec3 vv = F(Vec3, F(BYTE*, g_game, 616), 432);
    double L = Vec3_Length(&vv);
    w = F(BYTE*, s, 40);
    float cz = (float)F(double, w, 2920), cy = (float)F(double, w, 2912), cx = (float)F(double, w, 2904);
    float ky = (float)(fy * (double)g_gm64c338), kz = (float)(fz * (double)g_gm64c338);
    float px = (float)(fx * (double)g_gm64c338 * L);
    float py = (float)(ky * L);
    Vec3 pos;
    pos.x = (float)((double)px + cx);
    pos.y = (float)((double)py + cy);
    pos.z = (float)(L * kz + cz);
    double wave = Wave_Value(g_gm7025b8, 0, 0.0);
    float wv = (float)wave;
    double gv = F(float, g_gm7025b8, 548) * (double)g_ls64c460;
    if (gv > g_ls64c45c)
        gv = g_ls64c45c;
    else if (gv < g_ls64c334)
        gv = g_ls64c334;
    DWORD gi = (DWORD)(int)(gv * g_gm64c458);
    BYTE* e = F(BYTE*, s, 36);
    F(DWORD, e, 96) = ((((gi | 0xFFFFC000u) << 8) | gi) << 8) | gi;
    if (wave < g_gm64b7a8)
        Renderer_SetEnabled(e, 0, 0);
    else
    {
        Renderer_SetEnabled(e, 0, 1);
        double x = (wv - (double)g_gm64c308) * g_gm64c454;
        if (x < g_64c258)
            x = g_64c258;
        double y = g_one_f - x;
        F(float, F(BYTE*, s, 36), 12) = (float)((y * y * g_gm64c450 + g_one_f) * g_ls64c1ec);
    }
    double z = F(double, F(BYTE*, s, 40), 2920);
    if (z > g_ls64c13c && z < g_ls64c140)
        g_ls6716fc = 1;
    else
    {
        g_ls6716fc = 0;
        Renderer_SetEnabled(F(BYTE*, s, 36), 0, 0);
    }
    float dt = (float)F(double, F(BYTE*, s, 40), 2960);
    Particles_SetOrigin(F(BYTE*, s, 36), 0, &pos);
    Fx_EmitterStep(F(BYTE*, s, 36), 0, dt);
    Shafts_Draw(F(BYTE*, s, 36), 0, F(BYTE*, s, 32));
    CallThis(Gm_PostEvent, s, 1);
    double fps = g_one_f / (double)dt;
    BYTE* em = F(BYTE*, s, 36);
    if (fps > g_gm64c320)
        F(int, em, 44) = (int)((fps - g_gm64c320) * g_gm64c44c + g_gm64c448);
    else
        F(int, em, 44) = 50;
    if (F(int, em, 44) < 50)
        F(int, em, 44) = 50;
    else if (F(int, em, 44) > 2000)
        F(int, em, 44) = 2000;
    CallThis(Gm_PostEvent, s, 1);
}

} // extern "C"

// === sparks from a scraping object (a callback at $L_4cbec0)

extern "C" void Ss_Skid1(); KING_ALIAS("_Ss_Skid1", "_Fx_Skid1")
KING_VAR(float, g_ss64c33c, 64c33c)
KING_VAR(float, g_ss64c978, 64c978)
KING_VAR(float, g_ss64c974, 64c974)
KING_VAR(float, g_ss64c260, 64c260)

extern "C" {

// [$L_4cbec0] object o (kind 6 at +60, its description +72: point, direction) now and then throws a skid spark
// from its point along its direction; 1 when of that kind
int __cdecl Fx_ScrapeSpark(int, int, BYTE* o)
{
    BYTE* e = o + 60;
    if (!e)
        return 0;
    if (F(int, e, 8) != 6)
        return 0;
    const float* d = F(const float*, e, 12);
    float P[3] = {d[0], d[1], d[2]};
    float N[3] = {d[3], d[4], d[5]};
    if (rand() * (double)g_randScale > g_ss64c33c)
    {
        float n2[3] = {(float)(N[0] * (double)g_ss64c978), (float)(N[1] * (double)g_ss64c978), (float)(N[2] * (double)g_ss64c978)};
        double k = rand() * (double)g_ss64c974 + g_ss64c260;
        float V[3] = {(float)(n2[0] * k), (float)(n2[1] * k), (float)(n2[2] * k)};
        CallC(Ss_Skid1, P, V, 1);
    }
    return 1;
}

} // extern "C"

// === random spots on a source's square (two methods at $L_4c3cf0 / $L_4c22b0)

extern "C" void Rp_ForwardPoint(); KING_ALIAS("_Rp_ForwardPoint", "@MarineTask_ForwardPoint@12")
KING_VAR(float, g_rp64c690, 64c690)

extern "C" {

// [$L_4c3cf0] source i of emitter s (12-byte points at +8): a random spot in the square spanned by the x and y
// axes from it into *p, straight up into *n; 1
int __fastcall Fx_SquareSpot(BYTE* s, void*, int i, Vec3* p, Vec3* n)
{
    *n = g_axisZ;
    float r1 = (float)(rand() * (double)g_rp64c690);
    double r2 = rand() * (double)g_rp64c690;
    const float* b = (const float*)(s + 8 + i * 12);
    float ax = (float)(g_axisY.x * r2), ay = (float)(g_axisY.y * r2);
    double az = g_axisY.z * r2;
    double bx = g_axisX.x * (double)r1;
    float by = (float)(g_axisX.y * (double)r1), bz = (float)(g_axisX.z * (double)r1);
    float sy = (float)((double)by + b[1]), sz = (float)((double)bz + b[2]);
    p->x = (float)(bx + b[0] + ax);
    p->y = (float)((double)ay + sy);
    p->z = (float)(az + sz);
    return 1;
}

// [$L_4c22b0] the same on the sea: source i of emitter s (points at +12, the sea +8), the spot's height from the
// sea surface at it
int __fastcall Fx_SeaSpot(BYTE* s, void*, int i, Vec3* p, Vec3* n)
{
    *n = g_axisZ;
    float r1 = (float)(rand() * (double)g_rp64c690);
    float r2 = (float)(rand() * (double)g_rp64c690);
    const float* b = (const float*)(s + 12 + i * 12);
    float q[3];
    q[2] = 0;
    q[0] = (float)((double)r1 + b[0]);
    q[1] = (float)((double)r2 + b[0]);
    double h = CallThis<double>(Rp_ForwardPoint, F(BYTE*, s, 8), q);
    p->x = (float)((double)r1 + b[0]);
    p->y = (float)((double)r2 + b[1]);
    p->z = (float)(h + b[2]);
    return 1;
}

} // extern "C"

// === the car's debug force lines (FUN_4ce6d0)

extern "C" void Df_Line(); KING_ALIAS("_Df_Line", "@Render_Line3D@36")
extern "C" void Df_SetThis(); KING_ALIAS("_Df_SetThis", "@Vec3_SetThis@20")
extern "C" void Df_AddAssign(); KING_ALIAS("_Df_AddAssign", "@Vec3_AddAssign@16")
KING_VAR(int, g_df672980, 672980)                 // the switch read: -1 not yet
KING_VAR(const char*, g_df696868, 696868)         // the ini file
KING_VAR(char, g_df672998, 672998)
KING_VAR(char, g_df6729a0, 6729a0)
KING_VAR(char, g_df6729a8, 6729a8)
KING_VAR(char, g_df672994, 672994)
KING_VAR(float, g_df64c308, 64c308)
KING_VAR(float, g_df64c338, 64c338)
KING_VAR(double, g_df64b678, 64b678)

static void Df_Draw(BYTE* r, const Vec3& a, const Vec3& b, int color)
{
    CallThis(Df_Line, r, a.x, a.y, a.z, b.x, b.y, b.z, color);
}

static Vec3 Df_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_4ce6d0] with the ini switch on, car c's forces drawn by renderer r as lines from its position: the engine's
// pull (+9996) up, the brakes' (+10000) down, then the gear marks (+16496 flag 8 or turning +16560: one up mark;
// else the gears 2.. up to +16480 + 4 stacked, set ones +16484 bright, the current +16476 with a tick, gear 4 with
// the axes)
void __cdecl Car_DrawForces(BYTE* c, BYTE* r)
{
    if (g_df672980 == -1)
    {
        char buf[50];
        GetPrivateProfileStringA(&g_df672998, &g_df6729a0, &g_df6729a8, buf, 50, g_df696868);
        if (strcmp(buf, &g_df672994))
        {
            g_df672980 = 0;
            return;
        }
        g_df672980 = 1;
    }
    else if (!g_df672980)
        return;
    BYTE* b = F(BYTE*, c, 21600);
    Matrix m;
    SceneObj_Matrix1(c + 16, 0, &m);
    Vec3 base;
    base.x = m.m[9];
    base.y = m.m[10];
    base.z = m.m[11];
    const Vec3& X = g_axisX;
    const Vec3& Z = g_axisZ;
    // the pull
    float f = F(float, b, 9996);
    float ux = (float)((double)f * Z.x), uy = (float)((double)f * Z.y);
    float tx = (float)((double)base.x + ux), ty = (float)((double)base.y + uy);
    float tz = (float)((double)base.z + f * (double)Z.z);
    Df_Draw(r, Df_Set((double)base.x + X.x, (double)base.y + X.y, (double)base.z + X.z),
            Df_Set((double)tx + X.x, (double)ty + X.y, (double)tz + X.z), 5);
    // the brakes
    f = F(float, b, 10000);
    float dx = (float)((double)f * Z.x), dy = (float)((double)f * Z.y);
    Vec3 p = Df_Set((double)base.x - dx, (double)base.y - dy, (double)base.z - f * (double)Z.z);
    Df_Draw(r, Df_Set((double)base.x + X.x, (double)base.y + X.y, (double)base.z + X.z),
            Df_Set((double)p.x + X.x, (double)p.y + X.y, (double)p.z + X.z), 7);
    if ((b[16496] & 8) || F(double, b, 16560) > g_df64b678)
    {
        Vec3 up = Df_Set((double)Z.x + Z.x, (double)Z.y + Z.y, (double)Z.z + Z.z);
        Df_Draw(r, base, Df_Set((double)up.x + base.x, (double)up.y + base.y, (double)up.z + base.z),
                (b[16496] & 8) ? 7 : 5);
        return;
    }
    for (int k = 2; k < F(int, b, 16480) + 4;)
    {
        Vec3 v = Df_Set((double)Z.x * g_df64c308, (double)Z.y * g_df64c308, (double)Z.z * g_df64c308);
        Vec3 t = Df_Set((double)v.x + base.x, (double)v.y + base.y, (double)v.z + base.z);
        Df_Draw(r, base, t, (F(int, b, 16484) & (1 << k)) ? 5 : 6);
        if (F(int, b, 16476) + 4 == k)
        {
            Vec3 w = Df_Set((double)Z.x * g_df64c338, (double)Z.y * g_df64c338, (double)Z.z * g_df64c338), sum;
            Df_Draw(r, t, *Vec3_Add(&sum, &t, &w), 7);
        }
        if (k == 4)
        {
            Vec3 a, e;
            Vec3_Scaled(&a, &g_axisX, 0.5f);
            Df_Draw(r, base, *Vec3_Add(&e, &base, &a), 4);
            Vec3_Scaled(&a, &g_axisY, 0.5f);
            Df_Draw(r, base, *Vec3_Add(&e, &base, &a), 4);
        }
        Vec3 tmp;
        CallThis(Df_AddAssign, &base, &tmp, &g_axisZ);
        if (++k >= 10)
            break;
    }
}

} // extern "C"

// === a debug trail of lines (FUN_4d3240)

extern "C" void __fastcall Tl_Line(BYTE* s, void*, Vec3 a, Vec3 b, int color); KING_ALIAS("@Tl_Line@36", "@Render_Line3D@36")
KING_VAR(Vec3, g_tlUp, 702630)

static Vec3 Tl_Off(const Vec3& p, const Vec3& n, double w, bool plus)
{
    Vec3 a, r;
    a.x = (float)(n.x * w);
    a.y = (float)(n.y * w);
    a.z = (float)(n.z * w);
    r.x = (float)(plus ? (double)p.x + a.x : (double)p.x - a.x);
    r.y = (float)(plus ? (double)p.y + a.y : (double)p.y - a.y);
    r.z = (float)(plus ? (double)p.z + a.z : (double)p.z - a.z);
    return r;
}

// the line from the last point a to b, stretched by Vec3_Scaled(1)
static void Tl_Stretch(BYTE* s, const Vec3* a, const Vec3& b, int color)
{
    Vec3 d, e, f;
    Vec3_Add(&f, a, Vec3_Scaled(&e, Vec3_Sub(&d, &b, a), 1.0f));
    Tl_Line(F(BYTE*, s, 68), 0, *a, f, color);
}

extern "C" {

// [FUN_4d3240] trail s (renderer +68) goes on to point p heading along d: the centre line from the last point
// (+8), and with flags bit 1 / 2 the left / right edges at widths w1 and w2 across the heading (+20.. / +44..),
// unless the trail was just restarted (+0)
void __fastcall Trail_Add(BYTE* s, void*, Vec3 p, Vec3 d, double w1, double w2, int flags)
{
    const Vec3& U = g_tlUp;
    Vec3 c, n;
    c.x = (float)((double)d.y * U.z - (double)d.z * U.y);
    c.y = (float)((double)d.z * U.x - (double)d.x * U.z);
    c.z = (float)((double)d.x * U.y - (double)d.y * U.x);
    Vec3_Normalized(&n, &c);
    Vec3 L1 = {0, 0, 0}, L2 = {0, 0, 0}, R1 = {0, 0, 0}, R2 = {0, 0, 0};
    if (flags & 1)
    {
        L1 = Tl_Off(p, n, w1, true);
        L2 = Tl_Off(p, n, w2, true);
    }
    if (flags & 2)
    {
        R1 = Tl_Off(p, n, w1, false);
        R2 = Tl_Off(p, n, w2, false);
    }
    if (F(int, s, 0))
        F(int, s, 0) = 0;
    else
    {
        Tl_Line(F(BYTE*, s, 68), 0, F(Vec3, s, 8), p, 5);
        if ((flags & 1) && (F(BYTE, s, 4) & 1))
        {
            const Vec3& o = F(Vec3, s, 20);
            Vec3 t, m;
            t.x = (float)((double)L1.x - o.x);
            t.y = (float)((double)L1.y - o.y);
            t.z = (float)((double)L1.z - o.z);
            m.x = (float)((double)t.x + o.x);
            m.y = (float)((double)t.y + o.y);
            m.z = (float)((double)t.z + o.z);
            Tl_Line(F(BYTE*, s, 68), 0, o, m, 6);
            Tl_Stretch(s, &F(Vec3, s, 32), L2, 7);
        }
        if ((flags & 2) && (F(BYTE, s, 4) & 2))
        {
            Tl_Stretch(s, &F(Vec3, s, 44), R1, 6);
            Tl_Stretch(s, &F(Vec3, s, 56), R2, 7);
        }
    }
    F(int, s, 4) = flags;
    F(Vec3, s, 8) = p;
    if (flags & 1)
    {
        F(Vec3, s, 20) = L1;
        F(Vec3, s, 32) = L2;
    }
    if (flags & 2)
    {
        F(Vec3, s, 44) = R1;
        F(Vec3, s, 56) = R2;
    }
}

} // extern "C"

// === the rain and spray ahead of the lamps (a method at $L_4c9a10)

extern "C" void Rs_Emit(); KING_ALIAS("_Rs_Emit", "_Fx_Burst")                           // a particle burst (pos, dir, vel, ..., count, size, ...)
extern "C" void Rs_Root(); KING_ALIAS("_Rs_Root", "@Window_RootSurface@8")
extern "C" void Rs_M1(); KING_ALIAS("_Rs_M1", "@SceneObj_Matrix1@12")
extern "C" void Rs_Post(); KING_ALIAS("_Rs_Post", "@Queue_PostEvent@12")
KING_VAR(BYTE*, g_rs6d2078, 6d2078)
KING_VAR(int, g_rs6f33c0, 6f33c0)
KING_VAR(BYTE*, g_rsWind, 7025b8)
KING_VAR(Vec3, g_rsAhead, 702620)
KING_VAR(Vec3, g_rsUp, 702630)
KING_VAR(float, g_rs64c320, 64c320)
KING_VAR(double, g_rs64b7a8, 64b7a8)
KING_VAR(float, g_rs64c934, 64c934)
KING_VAR(float, g_rs64c250, 64c250)
KING_VAR(float, g_rs64c264, 64c264)
KING_VAR(float, g_rs64c254, 64c254)
KING_VAR(float, g_rs64c258, 64c258)
KING_VAR(float, g_rs64c224, 64c224)
KING_VAR(float, g_rs64c228, 64c228)
KING_VAR(float, g_rs64c230, 64c230)
KING_VAR(float, g_rs64c22c, 64c22c)
KING_VAR(float, g_rs64c334, 64c334)
KING_VAR(float, g_rs64c234, 64c234)
KING_VAR(float, g_rs64c930, 64c930)
KING_VAR(float, g_rs64c76c, 64c76c)
KING_VAR(float, g_rs64c40c, 64c40c)
KING_VAR(float, g_rs64c210, 64c210)
KING_VAR(float, g_rs64c214, 64c214)
KING_VAR(float, g_rs64c21c, 64c21c)
KING_VAR(float, g_rs64c218, 64c218)
KING_VAR(float, g_rs64c304, 64c304)
KING_VAR(float, g_rs64c92c, 64c92c)
KING_VAR(float, g_rs64c350, 64c350)
KING_VAR(float, g_rs64c220, 64c220)

static float Rs_Rand()
{
    return (float)(rand() * (double)g_rs64c254);
}

// m (rows +0 / +12 / +24, the position +36) applied to v in the float steps of Vec3_SetThis
static Vec3 Rs_Mul(const float* m, const Vec3& v)
{
    Vec3 r;
    r.x = (float)((double)v.z * m[6] + (double)v.y * m[3] + (double)v.x * m[0] + m[9]);
    r.y = (float)((double)v.z * m[7] + (double)v.y * m[4] + (double)v.x * m[1] + m[10]);
    r.z = (float)((double)v.x * m[2] + (double)v.z * m[8] + (double)v.y * m[5] + m[11]);
    return r;
}

static Vec3 Rs_Scale(const Vec3& v, double k)
{
    Vec3 r;
    r.x = (float)(v.x * k);
    r.y = (float)(v.y * k);
    r.z = (float)(v.z * k);
    return r;
}

static Vec3 Rs_Add(const Vec3& a, const Vec3& b)
{
    Vec3 r;
    r.x = (float)((double)a.x + b.x);
    r.y = (float)((double)a.y + b.y);
    r.z = (float)((double)a.z + b.z);
    return r;
}

extern "C" {

// [$L_4c9a10] weather s: a random tint of the root surface (+771), then for the player's lit lamp (the first of
// +10274 on) two bursts in the wedge ahead of it (60 degrees each side of the wind's heading): rain by the wind
// strength 7025b8 +532 above 64c320 with a calm wave, spray above 64c40c with a strong one
void __fastcall Weather_LampRain(BYTE* s, void*)
{
    BYTE* p = CallThis<BYTE*>(Rs_Root, F(BYTE*, F(BYTE*, s, 32), 12)) + 771;
    p[0] = (BYTE)(int)(Fx_Scale75() * g_rs64c458);
    p[1] = (BYTE)(int)(Fx_Scale75() * g_rs64c458);
    p[2] = (BYTE)(int)(Fx_Scale75() * g_rs64c458);
    BYTE* pl = g_rs6d2078;
    if (pl && F(BYTE*, pl, 616) && !g_rs6f33c0)
    {
        float w = F(float, g_rsWind, 532);
        Vec3 D;
        Vec3_Scaled(&D, &g_rsAhead, w);
        BYTE* car = F(BYTE*, pl, 616);
        int k = 0;
        while (k < 4 && !F(BYTE, car, 10274 + k))
            k++;
        if (k != 4)
        {
            Matrix M = F(Matrix, car, 708);
            Matrix T, R;
            const Vec3& lamp = F(Vec3, car, 10284 + 12 * k);
            Vec3 a = Rs_Mul(CallThis<const Matrix*>(Rs_M1, car + 16, &T)->m, lamp);
            Vec3 b = Rs_Mul(M.m, a);
            const float* Rz = Matrix_RotationZ(&T, (float)F(double, F(BYTE*, s, 32), 2928))->m;
            Vec3 c = Rs_Mul(Rz, g_rsAhead);
            if (w > g_rs64c320 && Wave_Value(g_rsWind, 0, 0) < g_rs64b7a8)
            {
                // rain
                float r = (float)(((double)w - g_rs64c320) * g_rs64c934);
                if (r > g_rs64c250)
                    r = 1.0f;
                else if (r < g_rs64c264)
                    r = 0;
                if (Rs_Rand() > r * (double)g_rs64c258)
                {
                    int cnt = (int)(((double)g_rs64c224 - g_rs64c228) * r + g_rs64c228);
                    float sz = (float)(((double)g_rs64c230 - g_rs64c22c) * r + g_rs64c22c);
                    float r1 = Rs_Rand(), r2 = Rs_Rand();
                    Vec3 u = Rs_Scale(g_rsUp, g_rs64c334);
                    double rr = rand() * (double)g_rs64c254 + g_rs64c250;
                    Vec3 V = Rs_Add(Rs_Scale(D, rr), u);
                    Vec3 P1 = Rs_Add(Rs_Scale(Rs_Mul(Matrix_RotationZ(&T, -1.0471976f)->m, c), g_rs64c234), b);
                    Vec3 P2 = Rs_Add(Rs_Scale(Rs_Mul(Matrix_RotationZ(&R, 1.0471976f)->m, c), g_rs64c234), b);
                    Vec3 pos;
                    Triangle_RandomPoint(&pos, &b, &P2, &P1);
                    float spread = (float)((rand() * (double)g_rs64c254 - r1) * g_rs64c930);
                    CallC(Rs_Emit, &pos, &g_rsAhead, &V, (float)(r2 * (double)g_rs64c76c), cnt, sz, spread, 0.86f,
                          1, 1);
                }
            }
            if (w > g_rs64c40c && Wave_Value(g_rsWind, 0, 0) > g_rs64b7a8)
            {
                // spray
                float r = (float)(((double)w - g_rs64c40c) * g_rs64c258);
                if (r > g_rs64c250)
                    r = 1.0f;
                if (Rs_Rand() > r * (double)g_rs64c258)
                {
                    int cnt = (int)(((double)g_rs64c210 - g_rs64c214) * r + g_rs64c214);
                    float sz = (float)(((double)g_rs64c21c - g_rs64c218) * r + g_rs64c218);
                    float q1 = Rs_Rand(), q2 = Rs_Rand();
                    Vec3 u = Rs_Scale(g_rsUp, g_rs64c304);
                    float q3 = Rs_Rand();
                    float spread = (float)((rand() * (double)g_rs64c254 - q1) * g_rs64c92c);
                    Vec3 t1, V, m1, s1, P1, m2, s2, P2, pos;
                    Vec3_Add(&V, Vec3_Scaled(&t1, &D, (float)(q3 * (double)g_rs64c350 + g_rs64c250)), &u);
                    Vec3_MulMatrix(&m1, &c, Matrix_RotationZ(&T, -1.0471976f));
                    Vec3_Add(&P1, &b, Vec3_Scaled(&s1, &m1, g_rs64c220));
                    Vec3_MulMatrix(&m2, &c, Matrix_RotationZ(&R, 1.0471976f));
                    Vec3_Add(&P2, &b, Vec3_Scaled(&s2, &m2, g_rs64c220));
                    Triangle_RandomPoint(&pos, &b, &P2, &P1);
                    CallC(Rs_Emit, &pos, &g_rsAhead, &V, (float)(q2 * (double)g_rs64c76c), cnt, sz, spread, 0.7f,
                          1, 0);
                }
            }
        }
    }
    CallThis(Rs_Post, s, 1);
}

} // extern "C"

// === a frame's offset across its road (FUN_4cd770)

struct LrPos { DWORD d[8]; };
struct LrRoom
{
    DWORD flags, m_4;
    double width;
    const char* module;
    const char* room;
    double m_24;
    int m_32, m_36, m_40, m_44;
};
struct LrMat { float m[12]; };
extern "C" float* __cdecl Lr_Coords(float* out, LrPos pos); KING_ALIAS("_Lr_Coords", "_Ai_GetCoordinatesPos")
extern "C" float* __cdecl Lr_Tangent(float* out, LrPos pos); KING_ALIAS("_Lr_Tangent", "_Ai_GetTangent")
extern "C" void __cdecl Lr_Room(LrPos pos, LrRoom* out); KING_ALIAS("_Lr_Room", "_Road_GetRoadParameters")
KING_VAR(Vec3, g_lrUp, 702630)
KING_VAR(char, g_lr66d638, 66d638)
KING_VAR(char, g_lr66d628, 66d628)
KING_VAR(double, g_lr64b678, 64b678)
KING_VAR(double, g_lr64b6e0, 64b6e0)
KING_VAR(double, g_lr64b780, 64b780)

extern "C" {

// [FUN_4cd770] how far frame m's position lies across the road at AI position pos (its tangent reversed when
// back), signed by the side; the road's half width, its +24 value scaled and its flags written out
double __cdecl Road_LateralOffset(LrPos pos, LrMat m, int, int back, double* halfWidth, double* v24, int* flags)
{
    float c[3];
    Lr_Coords(c, pos);
    Vec3 d;
    d.x = (float)((double)m.m[9] - c[0]);
    d.y = (float)((double)m.m[10] - c[1]);
    d.z = 0;
    Vec3 tg;
    Lr_Tangent(&tg.x, pos);
    if (back)
    {
        tg.x = -tg.x;
        tg.y = -tg.y;
        tg.z = -tg.z;
    }
    tg.z = 0;
    Vec3 t2;
    tg = *Vec3_Normalized(&t2, &tg);
    double proj = (double)tg.y * d.y + (double)tg.x * d.x + (double)tg.z * d.z;
    float px = (float)(tg.x * proj), py = (float)(tg.y * proj);
    Vec3 perp;
    perp.x = (float)((double)d.x - px);
    perp.y = (float)((double)d.y - py);
    perp.z = (float)(d.z - tg.z * proj);
    double lat = Vec3_Length(&perp);
    double A = (double)tg.y * d.z - (double)d.y * tg.z;
    double B = (double)d.x * tg.z - (double)tg.x * d.z;
    double C = (double)d.y * tg.x - (double)tg.y * d.x;
    if (C * g_lrUp.z + B * g_lrUp.y + A * g_lrUp.x < g_lr64b678)
        lat = -lat;
    LrRoom info;
    info.flags = 0;
    info.width = 0;
    info.module = &g_lr66d638;
    info.room = &g_lr66d628;
    info.m_24 = 0;
    info.m_36 = -1;
    info.m_40 = -1;
    Lr_Room(pos, &info);
    *halfWidth = info.width * g_lr64b6e0;
    *v24 = info.m_24 * g_lr64b780;
    *flags = info.flags;
    return lat;
}

} // extern "C"

// === the road's turn ahead (FUN_4cdc90)

extern "C" LrPos* __cdecl Lr_Advance(LrPos* out, int player, LrPos pos, double* offset, int msg);
KING_ALIAS("_Lr_Advance", "_Ai_AdvancePlayerPosition")
KING_VAR(double, g_lr64b738, 64b738)
KING_VAR(double, g_lr64b7c0, 64b7c0)
KING_VAR(double, g_lr64b670, 64b670)
KING_VAR(double, g_lr64ca48, 64ca48)
KING_VAR(double, g_lr64ca50, 64ca50)

// the road's tangent at pos (reversed when back), flattened and normalised
static Vec3 Lr_FlatDir(const LrPos& pos, bool back, Vec3* raw)
{
    Vec3 t, n;
    Lr_Tangent(&t.x, pos);
    if (back)
    {
        t.x = -t.x;
        t.y = -t.y;
        t.z = -t.z;
    }
    *raw = t;
    t.z = 0;
    return *Vec3_Normalized(&n, &t);
}

extern "C" {

// [FUN_4cdc90] the road from AI position pos of player over dist (back: the other way): the turn rate (*turn,
// its angle by the headings at the start, the middle and the end, signed by the side, per distance), the climb
// (*slope) and the distance left (*left); flags 1 for a sharp turn, 4 when the end was passed half way
int __cdecl Road_TurnAhead(LrPos pos, int player, double* turn, double* slope, double* left, double dist, int back)
{
    double half = dist * g_lr64b738;
    double whole = dist;
    if (back)
    {
        dist = -dist;
        half = -half;
    }
    Vec3 t0, t1, t2;
    Vec3 A = Lr_FlatDir(pos, back != 0, &t0);
    LrPos p1, p2;
    Lr_Advance(&p1, player, pos, &half, 0);
    Vec3 B = Lr_FlatDir(p1, back != 0, &t1);
    double cAB = (double)B.y * A.y + (double)B.x * A.x + (double)B.z * A.z;
    Lr_Advance(&p2, player, pos, &dist, 0);
    dist = x87_fabs(dist);
    Vec3 C = Lr_FlatDir(p2, back != 0, &t2);
    double cBC = (double)C.y * B.y + (double)C.x * B.x + (double)C.z * B.z;
    if (x87_fabs(cAB) * g_lr64b7c0 < x87_fabs(cBC))
        cAB = cBC * g_lr64b738;
    if (cAB > g_lr64b670)
        cAB = 1.0;
    else if (cAB < g_lr64ca48)
        cAB = -1.0;
    double ang = x87_acos(cAB);
    double X = (double)t1.z * t0.y - (double)t1.y * t0.z;
    double Y = (double)t1.x * t0.z - (double)t1.z * t0.x;
    double Z = (double)t1.y * t0.x - (double)t0.y * t1.x;
    if (Z * g_lrUp.z + Y * g_lrUp.y + X * g_lrUp.x < g_lr64b678)
        ang = -ang;
    double k = g_lr64b7c0 / whole;
    *turn = k * ang;
    Vec3 n1, n0;
    Vec3_Normalized(&n1, &t1);
    Vec3_Normalized(&n0, &t0);
    *slope = x87_asin((double)n1.z - n0.z) * k;
    *left = whole - dist;
    int flags = 0;
    if (x87_fabs(*turn) > g_lr64ca50)
        flags = 1;
    if (dist > whole * g_lr64b6e0)
        flags |= 4;
    return flags;
}

} // extern "C"

// === an AI car's target speed (FUN_4d4700)

extern "C" int __cdecl Ts_Flags(int player); KING_ALIAS("_Ts_Flags", "_Player_Flags")
extern "C" int** __cdecl Ts_CarId(int** out); KING_ALIAS("_Ts_CarId", "_Player_CarId")
extern "C" LrPos* __cdecl Ts_PositionId(LrPos* out, int player); KING_ALIAS("_Ts_PositionId", "_Ai_GetPositionId")
extern "C" int __cdecl Road_TurnAhead(LrPos pos, int player, double* turn, double* slope, double* left, double dist,
                                      int back);
KING_VAR(double, g_ts64b6f0, 64b6f0)
KING_VAR(double, g_ts64b770, 64b770)
KING_VAR(double, g_ts64b6b8, 64b6b8)
KING_VAR(double, g_ts64b6e0, 64b6e0)
KING_VAR(double, g_ts64b678, 64b678)
KING_VAR(double, g_ts64b670, 64b670)
KING_VAR(double, g_ts64b5b8, 64b5b8)
KING_VAR(double, g_ts64d330, 64d330)
KING_VAR(double, g_ts64b6d0, 64b6d0)
KING_VAR(double, g_ts64b898, 64b898)
KING_VAR(double, g_ts64b698, 64b698)
KING_VAR(double, g_ts64d328, 64d328)
KING_VAR(double, g_ts64d320, 64d320)
KING_VAR(double, g_ts64b938, 64b938)
KING_VAR(double, g_ts64b958, 64b958)
KING_VAR(double, g_ts64b780, 64b780)
KING_VAR(double, g_ts64d318, 64d318)

extern "C" {

// [FUN_4d4700] the speed an AI player aims for from speed: capped for flag bits 0x4000 / 0x1000000, scaled to keep
// up with the player's car (0x80), cut for the road's turn, climb and end ahead, by the lateral speed (v24, v40)
// and the gap v32, and limited by the room left v60
double __cdecl Ai_TargetSpeed(BYTE* player, int f12, double speed, double v24, double v32, double v40, int back,
                              double fallback, double v60)
{
    if (!(Ts_Flags((int)player) & 0x4000) && f12 && speed > g_ts64b6f0)
        speed = 20.0;
    if (Ts_Flags((int)player) & 0x80)
    {
        int* id;
        BYTE* car = (BYTE*)*Ts_CarId(&id);
        double x = (F(double, car, 504) - F(double, player, 504)) * g_ts64b770;
        x = x * x87_fabs(x);
        if (x < g_ts64b6b8)
            x = g_ts64b6b8 * g_ts64b6e0;
        else if (x > g_ts64b6e0)
            x = g_ts64b6e0;
        else if (x < g_ts64b678)
            x *= g_ts64b6e0;
        x += g_ts64b670;
        speed *= x;
        fallback *= x;
    }
    int lat = 0;
    if (x87_fabs(v24) > g_ts64b5b8)
        lat = 2;
    if (x87_fabs(v40) > g_ts64d330)
        lat |= 4;
    double turn = 0, slope = 0, left = 50.0;
    LrPos pid;
    Ts_PositionId(&pid, (int)player);
    int r = Road_TurnAhead(pid, (int)player, &turn, &slope, &left, speed * g_ts64b6d0, back);
    if (!r && !(v32 < g_ts64b898) && !lat)
    {
        if (!f12)
            speed = fallback;
    }
    else
    {
        double st = speed;
        if (st > g_ts64b6d0)
        {
            if (r & 1)
            {
                double lim = x87_sqrt(x87_fabs(g_ts64b6d0 / turn));
                if (lim < g_ts64b698)
                    lim = 8.0;
                if (st > lim)
                    st = lim;
            }
            if ((r & 2) && slope < g_ts64d328)
                st = st / (x87_fabs(slope) * g_ts64d320);
            if ((r & 4) && left < g_ts64b938)
                st = st * left * g_ts64b958 + g_ts64b670;
            if (!(v32 < g_ts64b898))
            {
                double t = (v32 - g_ts64b898) * g_ts64b6d0;
                st = (st - g_ts64b6d0) * t * t + g_ts64b6d0;
                speed = st;
                if (st < g_ts64b6d0)
                    speed = st = g_ts64b6d0;
            }
            else
                speed = st = g_ts64b6d0;
        }
        if (lat && st > g_ts64b780)
            speed = st * g_ts64d318;
    }
    if ((Ts_Flags((int)player) & 0x1000000) && speed > g_ts64b6f0)
        speed = 20.0;
    if (!(v60 > g_ts64b678))
        return speed;
    if (v60 < g_ts64b780)
        return 0;
    double lim = x87_sqrt((v60 - g_ts64b780) * g_ts64b780);
    if (lim < g_ts64b6d0)
        lim = g_ts64b6d0;
    return speed > lim ? lim : speed;
}

} // extern "C"

// === an AI car's throttle and brake (FUN_4d60c0)

extern "C" double __cdecl Ai_TargetSpeed(BYTE* player, int f12, double speed, double v24, double v32, double v40,
                                         int back, double fallback, double v60);
extern "C" void Tb_Grip(); KING_ALIAS("_Tb_Grip", "@Wheels_MinGrip@8")
extern "C" void Tb_Gear(); KING_ALIAS("_Tb_Gear", "@Car_BestGearNow@8")
ASM_FUNC(Tb_4d2970, @Car_SlipRatio@8)
KING_VAR(double, g_tb64b5b8, 64b5b8)
KING_VAR(double, g_tb64b840, 64b840)
KING_VAR(double, g_tb64b670, 64b670)
KING_VAR(double, g_tb64c958, 64c958)
KING_VAR(float, g_tb64c450, 64c450)
KING_VAR(double, g_tb64d348, 64d348)
KING_VAR(Vec3, g_tbUp, 702630)

extern "C" {

// [FUN_4d60c0] AI car c sets its brake (+10000) or throttle (+9996) toward its target speed from v40 / v48, how
// upright it stands and its gear: brake by the excess over it (times the grip), throttle by the shortfall (full
// when nearly stopped with v56 high, a touch when steering hard on its wheels' flags)
void __fastcall Ai_ThrottleBrake(BYTE* c, void*, int, int, int, int, int, int, int, int, double v40, double v48,
                                 double v56)
{
    double grip = CallThis<double>(Tb_Grip, c);
    double h = CallThis<double>(Tb_4d2970, c);
    F(int, c, 10000) = 0;
    F(int, c, 9996) = 0;
    Matrix M = F(Matrix, c, 16);
    const Vec3& U = g_tbUp;
    double a = (double)U.z * M.m[6] + (double)U.y * M.m[3] + (double)U.x * M.m[0];
    double b = (double)U.z * M.m[7] + (double)U.y * M.m[4] + (double)U.x * M.m[1];
    double d = (double)M.m[8] * U.z + (double)M.m[5] * U.y + (double)M.m[2] * U.x;
    double up = d * U.z + b * U.y + a * U.x;
    double gear = CallThis<double>(Tb_Gear, c);
    BYTE* player = (BYTE*)F(int, F(BYTE*, c, 16576), 20636);
    double tgt = Ai_TargetSpeed(player, F(int, c, 16504), v40, v48, up, F(double, c, 16568), F(int, c, 16500), gear,
                                F(double, c, 16560));
    if (tgt < g_tb64b5b8)
    {
        F(float, c, 10000) = 1.0f;
        return;
    }
    float sp = F(float, c, 13100);
    if (sp > tgt)
    {
        double x = ((double)sp - tgt) * g_tb64b840;
        if (x > g_tb64b670)
            x = g_tb64b670;
        F(float, c, 10000) = (float)(x * grip);
        return;
    }
    if (!(tgt * g_tb64c958 > sp))
        return;
    float thr = (float)((tgt - sp) * g_tb64b840 * h);
    F(float, c, 9996) = thr;
    if (sp < g_tb64c450 && v56 > g_tb64d348)
    {
        F(float, c, 9996) = 1.0f;
        return;
    }
    if ((F(int, c, 13420) || F(int, c, 13424) || F(int, c, 13428) || F(int, c, 13432)) &&
        x87_fabs(F(float, c, 10012)) > g_tb64b5b8 && thr > g_tb64b5b8)
        F(float, c, 9996) = 0.1f;
}

} // extern "C"

// === a car's state stored in the resources (FUN_4cf400)

extern "C" BYTE* __fastcall Fx_ObjectInit(BYTE* s, void*, int a, int b, const char* name, const Matrix* m, int c,
                                          const void* data, int d, int e, int f);
extern "C" void Cs_GetState(); KING_ALIAS("_Cs_GetState", "@Car_GetState@12")
extern "C" void Cs_Set(); KING_ALIAS("_Cs_Set", "@Resources_SetData@24")
extern "C" void Cs_Get(); KING_ALIAS("_Cs_Get", "@Resources_Get@12")
extern "C" void Cs_Room(); KING_ALIAS("_Cs_Room", "_AI_RoomFlags4096")
extern "C" void Cs_M1(); KING_ALIAS("_Cs_M1", "@SceneObj_Matrix1@12")
extern "C" void Cs_Get800(); KING_ALIAS("_Cs_Get800", "@SceneObj_Get800@8")
extern "C" void Cs_Get12(); KING_ALIAS("_Cs_Get12", "@Fx_Get12@8")
extern "C" void Cs_Get9544(); KING_ALIAS("_Cs_Get9544", "@Fx_Get9544@8")
extern "C" void Cs_Get10420(); KING_ALIAS("_Cs_Get10420", "@Fx_Get10420@8")
extern "C" void Cs_Get20396(); KING_ALIAS("_Cs_Get20396", "@Fx_Get20396@8")
KING_VAR(int, g_cs68b990, 68b990)
KING_VAR(BYTE*, g_cs696ca0, 696ca0)

extern "C" {

// [FUN_4cf400] car c (with a record +308) stores its state (300 bytes under id +360): a fresh record for a
// player-less car, else its record updated with the state, the value of vtbl+84 and the player
void __cdecl Car_StoreState(BYTE* c)
{
    if (!F(BYTE*, c, 308))
        return;
    BYTE state[196];
    CallThis(Cs_GetState, c, state);
    BYTE rec[300];
    if (F(int, c, 20636) == g_cs68b990)
    {
        float v = CallVirt<float>(c, 84);
        Matrix t;
        const Matrix* m = CallThis<const Matrix*>(Cs_M1, c + 16, &t);
        const char* name = CallThis<const char*>(Cs_Get12, CallThis<BYTE*>(Cs_Get800, c + 16));
        int b = CallThis<int>(Cs_Get9544, c);
        int hi = CallThis<int>(Cs_Get20396, c);
        int a = CallThis<int>(Cs_Get10420, c) + hi * 1000;
        Fx_ObjectInit(rec, 0, a, b, name, m, F(int, c, 20636), state + 8, *(int*)&v, 0, 0);
        CallThis(Cs_Set, F(BYTE*, c, 52) + 704, F(int, c, 360), F(int, F(BYTE*, c, 308), 60), 300, rec);
        return;
    }
    BYTE* res = CallThis<BYTE*>(Cs_Get, g_cs696ca0 + 704, F(int, c, 360));
    if (!res)
        return;
    CallC(Cs_Room, F(int, c, 20636));
    memcpy(rec, F(BYTE*, res, 24), 300);
    memcpy(rec + 48, state, 196);
    F(float, rec, 244) = CallVirt<float>(c, 84);
    F(int, rec, 296) = F(int, c, 20636);
    CallThis(Cs_Set, F(BYTE*, c, 52) + 704, F(int, c, 360), F(int, F(BYTE*, res, 12), 4016), 300, rec);
}

} // extern "C"

// === an AI car's frame on its road (FUN_4cf620)

extern "C" void As_Zero(); KING_ALIAS("_As_Zero", "_Effect_Zero")
extern "C" void As_Flags(); KING_ALIAS("_As_Flags", "_Player_Flags")
extern "C" void As_HasItems(); KING_ALIAS("_As_HasItems", "_Fx_HasItems")
extern "C" void As_Log(); KING_ALIAS("_As_Log", "_Sys_Log")
extern "C" void As_M1(); KING_ALIAS("_As_M1", "@SceneObj_Matrix1@12")
extern "C" void As_AddAssign(); KING_ALIAS("_As_AddAssign", "@Vec3_AddAssign@16")
extern "C" void As_TooFar(); KING_ALIAS("_As_TooFar", "_Effect_TooFar4cee70")
extern "C" void As_Park(); KING_ALIAS("_As_Park", "_Fx_ParkVehicle")
extern "C" void As_Near(); KING_ALIAS("_As_Near", "_Fx_NearTruck")
extern "C" void As_Launch(); KING_ALIAS("_As_Launch", "_Car_Launch")
extern "C" void As_PositionId(); KING_ALIAS("_As_PositionId", "_Ai_GetPositionId")
extern "C" void __cdecl As_Room(LrPos pos, LrRoom* out); KING_ALIAS("_As_Room", "_Road_GetRoadParameters")
extern "C" void As_Level(); KING_ALIAS("_As_Level", "@Fx_LevelForHeight@16")
extern "C" BYTE* __cdecl As_Orient(BYTE* out, int player, LrPos pos); KING_ALIAS("_As_Orient", "_Ai_GetOrientationSmoothedF")
extern "C" void As_Set884(); KING_ALIAS("_As_Set884", "_Player_Set884")
extern "C" void As_Set976(); KING_ALIAS("_As_Set976", "_Player_Set976")
extern "C" void As_Root(); KING_ALIAS("_As_Root", "@Node_Root@8")
extern "C" int __stdcall As_Light(BYTE* node, Vec3 p, void* fn, BYTE* c); KING_ALIAS("_As_Light@24", "_Light_Set@24")
extern "C" void As_Tables(); KING_ALIAS("_As_Tables", "@Tables2_Set@16")
void __cdecl Car_WorldEvent(const int* m, BYTE* c);
KING_VAR(int, g_as68b990, 68b990)
KING_VAR(BYTE*, g_as6d2078, 6d2078)
KING_VAR(BYTE*, g_as696ca0, 696ca0)
KING_VAR(BYTE*, g_as6f33ac, 6f33ac)
KING_VAR(Vec3, g_as702640, 702640)
KING_VAR(Vec3, g_asEye, 702650)
KING_VAR(char, g_as6729ac, 6729ac)
KING_VAR(double, g_as64caa8, 64caa8)
KING_VAR(double, g_as64caa0, 64caa0)
KING_VAR(double, g_as64ca98, 64ca98)
KING_VAR(double, g_as64b780, 64b780)
KING_VAR(double, g_as64b6e0, 64b6e0)
KING_VAR(float, g_as64c250, 64c250)
KING_VAR(float, g_as64ca90, 64ca90)
KING_VAR(double, g_as64b678, 64b678)

// k ^ (t * m) as the original computes it, all on the FPU stack
static __declspec(noinline) double As_Pow(double k, double t, double m)
{
    __asm {
        fldln2
        fld k
        fyl2x
        fmul t
        fmul m
        fldl2e
        fmulp st(1), st
        fld st(0)
        frndint
        fxch st(1)
        fsub st, st(1)
        f2xm1
        fld1
        faddp st(1), st
        fscale
        fstp st(1)
    }
}

extern "C" {

// [FUN_4cf620] AI car c of its player (+20636) with flags *flags: its lamps by the time of day, its frame offset
// (+21040) checked for parking or launching, then on its road (unless *flags & 1) its smoothed frame (+20640)
// lifted by its level, its heading (+20688) and the camera (+20712); its light, its stored gear speed (+16560)
// from an event 29 (+21872) and the player's note of it; 1 (0 when the effect is done)
int __cdecl Ai_CarFrame(BYTE* c, int* flags)
{
    int player = F(int, c, 20636);
    if (CallC<int>(As_Zero, c, player))
        return 0;
    if (player != g_as68b990)
        *flags = CallC<int>(As_Flags, player);
    if (F(BYTE*, g_as6d2078, 616) == c)
        return 1;
    player = F(int, c, 20636);
    if (player != g_as68b990 && !(*flags & 4) && !(CallC<int>(As_HasItems, player) & 0xff))
        CallC(As_Log, &g_as6729ac, F(int, g_as696ca0, 2992));
    double tod = F(double, g_as6f33ac, 88);
    if ((tod > g_as64caa8 || tod < g_as64caa0) && !(CallC<int>(As_Flags, F(int, c, 20636)) & 4))
        F(int, c, 20920) = 1;
    else
        F(int, c, 20920) = 0;
    Matrix M, T;
    M = *CallThis<const Matrix*>(As_M1, c + 16, &T);
    const float* o = (const float*)(c + 21040);
    Vec3 off, tmp;
    off.x = (float)((double)M.m[0] * o[0] + (double)M.m[6] * o[2] + (double)M.m[3] * o[1]);
    off.y = (float)((double)M.m[1] * o[0] + (double)M.m[7] * o[2] + (double)M.m[4] * o[1]);
    off.z = (float)((double)M.m[8] * o[2] + (double)M.m[5] * o[1] + (double)M.m[2] * o[0]);
    CallThis(As_AddAssign, &M.m[9], &tmp, &off);
    if (*flags & 1)
    {
        if (CallC<int>(As_TooFar, c))
            CallC(As_Park, c, F(int, c, 20636), flags);
    }
    else if (CallC<int>(As_Near, c))
        CallC(As_Launch, c, F(int, c, 20636), flags, 1);
    BYTE* cab = F(BYTE*, c, 21600);
    F(int, cab, 16504) = (*flags & 0x80) ? 0 : 1;
    if (!(*flags & 1))
    {
        LrPos pid;
        CallC(As_PositionId, &pid, F(int, c, 20636));
        LrRoom info;
        info.flags = 0;
        info.width = 0;
        info.module = &g_lr66d638;
        info.room = &g_lr66d628;
        info.m_24 = 0;
        info.m_36 = -1;
        info.m_40 = -1;
        As_Room(pid, &info);
        int lvl = CallThis<int>(As_Level, cab, info.width * g_as64b6e0);
        Matrix O, I, Tr, X;
        As_Orient((BYTE*)&O, F(int, c, 20636), pid);
        const float* R = Matrix_Inverse(&I, &O)->m;
        double hx = (double)R[6] * F(float, c, 20684) + (double)R[3] * F(float, c, 20680) +
                    (double)R[0] * F(float, c, 20676) + R[9];
        double f = As_Pow(g_as64ca98, F(double, g_as696ca0, 2960), g_as64b780);
        double val = (lvl + g_as64b6e0) * F(double, cab, 16552) * (g_as64c250 - f) + hx * f;
        Vec3 v;
        v.x = (float)(val * g_as702640.x);
        v.y = (float)(val * g_as702640.y);
        v.z = (float)(val * g_as702640.z);
        memcpy(c + 20640, Matrix_Multiply(&X, Matrix_Translation(&Tr, v), &O), 48);
        Vec3 tg, tn;
        Lr_Tangent(&tg.x, pid);
        Vec3_Normalized(&tn, &tg);
        F(float, c, 20688) = (float)(tn.x * (double)g_as64ca90);
        F(float, c, 20692) = (float)(tn.y * (double)g_as64ca90);
        F(float, c, 20696) = (float)(tn.z * (double)g_as64ca90);
        F(Vec3, c, 20712) = g_asEye;
        CallC(As_Set884, F(int, c, 20636), lvl);
        cab = F(BYTE*, c, 21600);
    }
    Matrix P;
    Vec3 pos = *(const Vec3*)&CallThis<const Matrix*>(As_M1, c + 16, &P)->m[9];
    As_Light(CallThis<BYTE*>(As_Root, c + 272), pos, (void*)Car_WorldEvent, c);
    F(double, cab, 16560) = -1.0;
    BYTE* e = F(BYTE*, c, 21872);
    if (!F(int, c, 21876) && e && F(int, e, 0) == 29)
    {
        float v = *F(float*, e, 12);
        if (v > g_as64b678 && !F(int, e, 4))
            F(double, F(BYTE*, c, 21600), 16560) = v;
        CallThis(As_Tables, c + 21872, 0, 0);
    }
    if (!(*flags & 1))
    {
        double t = F(double, cab, 16560);
        CallC(As_Set976, F(int, c, 20636), t > g_as64b678 ? 1 : 0, t);
    }
    return 1;
}

} // extern "C"


// === the AI driver's step (a method at $L_4d6510)

extern "C" void __fastcall Ai_ThrottleBrake(BYTE* c, void*, int, int, int, int, int, int, int, int, double v40,
                                            double v48, double v56);
extern "C" void Ad_TakeMode(); KING_ALIAS("_Ad_TakeMode", "@Body_TakeMode@8")
extern "C" void Ad_CopyMatrix(); KING_ALIAS("_Ad_CopyMatrix", "@Fx_CopyMatrix@12")
extern "C" Vec3* __cdecl Ad_Direction(Vec3* out, LrPos pos, int negate, double* angle); KING_ALIAS("_Ad_Direction", "_Effect_Direction4cda20")
extern "C" double __cdecl Ad_TurnRate(LrPos pos, double d, int id, int back); KING_ALIAS("_Ad_TurnRate", "_Fx_TurnRate")
extern "C" void Ad_Swing(); KING_ALIAS("_Ad_Swing", "@Fx_SwingStep@36")
extern "C" void Ad_Steer(); KING_ALIAS("_Ad_Steer", "@Fx_SteerFromSwing@32")
extern "C" void Ad_Shown(); KING_ALIAS("_Ad_Shown", "@Car_SetSteerShown@16")
extern "C" void Ad_Look(); KING_ALIAS("_Ad_Look", "_Ai_LookAhead")
extern "C" void Ad_Fatal(); KING_ALIAS("_Ad_Fatal", "_Sys_Fatal")
extern "C" void Ad_Now(); KING_ALIAS("_Ad_Now", "_Tsc_Now")
extern "C" void Ad_Set884(); KING_ALIAS("_Ad_Set884", "_Player_Set884")
extern "C" double __cdecl Road_LateralOffset(LrPos pos, LrMat m, int, int back, double* halfWidth, double* v24,
                                             int* flags);
KING_VAR(int, g_ad721768, 721768)                     // the trace buffer
KING_VAR(char*, g_ad71e814, 71e814)
KING_VAR(int, g_ad71e818, 71e818)
KING_VAR(int, g_ad71e810, 71e810)
KING_VAR(BYTE*, g_ad6cec6c, 6cec6c)
KING_VAR(BYTE*, g_ad6cec98, 6cec98)
KING_VAR(BYTE*, g_ad6d2078, 6d2078)
KING_VAR(int, g_ad696328, 696328)
KING_VAR(int, g_ad6962c0, 6962c0)
KING_VAR(int, g_ad6962c4, 6962c4)
KING_VAR(double, g_ad696348, 696348)
KING_VAR(BYTE, g_ad697db8, 697db8)
KING_VAR(Vec3, g_adAhead, 702620)
KING_VAR(Vec3, g_adUp, 702630)
KING_VAR(char, g_ad672acc, 672acc)
KING_VAR(char, g_ad672abc, 672abc)
KING_VAR(char, g_ad672aa8, 672aa8)
KING_VAR(char, g_ad672a98, 672a98)
KING_VAR(char, g_ad672a78, 672a78)
KING_VAR(char, g_ad672a68, 672a68)
KING_VAR(double, g_ad64b6b8, 64b6b8)
KING_VAR(double, g_ad64b6e0, 64b6e0)
KING_VAR(double, g_ad64b678, 64b678)
KING_VAR(double, g_ad64d0c0, 64d0c0)
KING_VAR(float, g_ad64c40c, 64c40c)
KING_VAR(double, g_ad64b6d0, 64b6d0)
KING_VAR(double, g_ad64b670, 64b670)
KING_VAR(double, g_ad64ca48, 64ca48)
KING_VAR(float, g_ad64c320, 64c320)
KING_VAR(float, g_ad64d350, 64d350)
KING_VAR(double, g_ad64b698, 64b698)
KING_VAR(double, g_ad64b7c0, 64b7c0)

// a text appended to the trace buffer
static void Ad_Trace(const char* s)
{
    if (!g_ad721768 || !g_ad71e814 || !*s)
        return;
    while (g_ad71e818 < g_ad71e810)
    {
        g_ad71e814[g_ad71e818] = *s++;
        g_ad71e818++;
        if (!*s)
            break;
    }
}

static int Ad_Clock()
{
    return (int)CallC<double>(Ad_Now);
}

static void Ad_Finish(BYTE* s, const char* t)
{
    Ad_Trace(t);
    CallThis(Ad_TakeMode, s);
    Ad_Trace(&g_ad672aa8);
}

static void Ad_Show(BYTE* s, double v)
{
    CallThis(Ad_Shown, s, v);
}

// the dot of a with the cross product b x c, all floats
static Vec3 Ad_Cross(const Vec3& b, const Vec3& c)
{
    Vec3 r;
    r.x = (float)((double)b.y * c.z - (double)b.z * c.y);
    r.y = (float)((double)b.z * c.x - (double)b.x * c.z);
    r.z = (float)((double)b.x * c.y - (double)b.y * c.x);
    return r;
}

extern "C" {

// [$L_4d6510] AI driver s of car +16576: unless it is the player's own car (only its lateral offset measured) or
// the player stopped (+4 flag: braking), it reads the road ahead (its position moved on by the speed, the offset
// across it +16568, the heading, the swing), then by its state +16492 (0 parked, 1 driving, 2 reversing out,
// 3 turning round, 4 nothing) steers, shows the steering and sets the throttle / brake; the mode taken after
void __fastcall Ai_DriverStep(BYTE* s, void*)
{
    Ad_Trace(&g_ad672acc);
    LrPos pid = {{(DWORD)-1, (DWORD)-1, 0, 0, 0, 0, 0, (DWORD)-1}};
    BYTE* car = F(BYTE*, s, 16576);
    if (!F(int, g_ad6cec6c, 1316) || F(int, g_ad6cec98, 384) != 2)
        Ts_PositionId(&pid, F(int, car, 20636));
    if (F(BYTE*, g_ad6d2078, 616) == F(BYTE*, s, 16576) && !g_ad696328)
    {
        // the player's own car: its offset only
        g_ad6962c0 = Ad_Clock();
        g_ad6962c4 = Ad_Clock();
        if (F(int, g_ad6cec6c, 1316) && F(int, g_ad6cec98, 384) == 2)
            g_ad696348 = 10.0;
        else
        {
            double hw, v24;
            int fl;
            LrMat m;
            memcpy(&m, CallThis<const void*>(Ad_CopyMatrix, s, &m), 48);
            double r = Road_LateralOffset(pid, m, F(int, F(BYTE*, s, 16576), 20636), F(int, s, 16500), &hw, &v24, &fl);
            g_ad696348 = x87_fabs(r) - (hw + v24);
        }
        Ad_Finish(s, &g_ad672abc);
        return;
    }
    g_ad6962c0 = Ad_Clock();
    if (Ts_Flags(F(int, F(BYTE*, s, 16576), 20636)) & 4)
    {
        F(float, s, 10000) = 1.0f;
        F(int, s, 9996) = 0;
        F(int, s, 10012) = 0;
        g_ad6962c4 = Ad_Clock();
        Ad_Finish(s, &g_ad672a98);
        return;
    }
    car = F(BYTE*, s, 16576);
    BYTE* pl = F(BYTE*, car, 20636);
    if (F(int, car, 360) != F(int, pl, 84))
    {
        char buf[128];
        sprintf(buf, &g_ad672a78, F(int, car, 360), F(int, pl, 84));
        CallC(Ad_Fatal, buf, 0);
    }
    double target = 30.0;
    double grip = CallThis<double>(Tb_Grip, s);
    double h = CallThis<double>(Tb_4d2970, s);
    car = F(BYTE*, s, 16576);
    Vec3 off = F(Vec3, car, 21040);
    double k = F(float, &g_ad697db8 + 2416 * F(int, car, 10420), 4) * (F(int, s, 16500) ? g_ad64b6b8 : g_ad64b6e0);
    double kd = k;
    float ax = (float)(k * g_adAhead.x), ay = (float)(k * g_adAhead.y);
    Matrix M = F(Matrix, s, 16), T, X;
    Vec3 tr;
    tr.x = (float)((double)off.x + ax);
    tr.y = (float)((double)ay + off.y);
    tr.z = (float)(k * g_adAhead.z + off.z);
    LrMat look;
    memcpy(&look, Matrix_Multiply(&X, Matrix_Translation(&T, tr), &M), 48);
    LrPos ahead;
    Lr_Advance(&ahead, F(int, F(BYTE*, s, 16576), 20636), pid, &kd, 0);
    pid = ahead;
    double hw, v24;
    int fl;
    F(double, s, 16568) =
        Road_LateralOffset(pid, look, F(int, F(BYTE*, s, 16576), 20636), F(int, s, 16500), &hw, &v24, &fl);
    double v332;
    Vec3 dir = *Ad_Direction(&dir, pid, F(int, s, 16500), &v332);
    int lvl = CallThis<int>(As_Level, s, hw);
    float lat = (float)F(double, s, 16568);
    double sw = CallThis<double>(Ad_Swing, s, lvl * 2, hw, v24, fl, *(int*)&lat);
    if (sw > g_ad64b678 && g_ad64d0c0 > sw && F(int, s, 16504))
        target = sw;
    CallC(Ad_Set884, F(int, F(BYTE*, s, 16576), 20636), (int)(F(int, s, 16476) * g_ad64b6e0));
    double turnRate = Ad_TurnRate(pid, 15.0, F(int, F(BYTE*, s, 16576), 20636), F(int, s, 16500));
    double oldLat = F(double, s, 16568);
    Matrix M2 = F(Matrix, s, 16);
    F(double, s, 16568) = (F(int, s, 16476) + 1) * F(double, s, 16552) * g_ad64b6e0 + F(double, s, 16568);
    // the car's heading flattened
    Vec3* fw = (Vec3*)(s + 16464);
    const Vec3& A = g_adAhead;
    fw->x = (float)((double)M2.m[3] * A.y + (double)M2.m[6] * A.z + (double)M2.m[0] * A.x);
    fw->y = (float)((double)M2.m[4] * A.y + (double)M2.m[7] * A.z + (double)M2.m[1] * A.x);
    fw->z = 0;
    Vec3 tmp;
    *fw = *Vec3_Normalized(&tmp, fw);
    double dot = (double)fw->z * dir.z + (double)fw->y * dir.y + (double)dir.x * fw->x;
    Vec3 c = Ad_Cross(dir, *fw), n;
    Vec3_Normalized(&n, &c);
    if ((double)n.z * g_adUp.z + (double)n.y * g_adUp.y + (double)n.x * g_adUp.x < g_ad64b678)
    {
        n.x = -n.x;
        n.y = -n.y;
        n.z = -n.z;
    }
    int st = F(int, s, 16492);
    if (dot > g_ad64b678)
    {
        if (st == 3)
        {
            F(short, s, 10054) = 100;
            F(int, s, 16492) = 1;
        }
    }
    else if (st == 1)
    {
        F(double, s, 16456) = F(double, s, 16568);
        F(int, s, 16492) = 3;
    }
    double X2 = (double)dir.y * fw->z - (double)dir.z * fw->y;
    double Y2 = (double)dir.z * fw->x - (double)dir.x * fw->z;
    double Z2 = (double)dir.x * fw->y - (double)dir.y * fw->x;
    double sa = Z2 * n.z + Y2 * n.y + X2 * n.x;
    if (!(sa < g_ad64b670))
        *(unsigned __int64*)&sa = 0x3FEFFF2E48E8A71Eull;
    else if (!(sa > g_ad64ca48))
        *(unsigned __int64*)&sa = 0xBFEFFF2E48E8A71Eull;
    sa = x87_asin(sa);
    // a stuck car backs out
    if (F(float, s, 13100) < g_ad64c40c && F(int, s, 16492) != 2 && !(F(BYTE, s, 16496) & 8) &&
        F(double, s, 16560) < g_ad64b678 && !(CallC<int>(Ad_Look, s, 1) & 8))
    {
        if (!F(int, s, 16428))
        {
            F(double, s, 16440) = 0;
            F(int, s, 16428) = 1;
        }
        else if (F(double, s, 16440) > g_ad64b6d0)
        {
            F(double, s, 16440) = 3.0;
            F(int, s, 16428) = 0;
            F(short, s, 10054) = 20;
            F(int, s, 16492) = 2;
        }
        else
            F(double, s, 16440) = F(double, F(BYTE*, F(BYTE*, s, 16576), 52), 2960) + F(double, s, 16440);
    }
    else
        F(int, s, 16428) = 0;
    if (Ts_Flags(F(int, F(BYTE*, s, 16576), 20636)) & 0x8000000)
    {
        if (x87_fabs(F(double, s, 16568)) < g_ad64b6e0)
            F(int, s, 16488) |= 4;
    }
    else
        F(int, s, 16488) &= ~4;
    if (F(int, s, 16488))
        F(int, s, 16492) = 0;
    else if (!F(int, s, 16492))
        F(int, s, 16492) = 1;
    float sp = F(float, s, 13100);
    switch (F(int, s, 16492))
    {
    case 0:
        Ad_Show(s, 0);
        F(int, s, 9996) = 0;
        F(float, s, 10000) = (float)grip;
        break;
    case 3:
    {
        // turning round
        double edge = v24 * g_ad64b6e0 + hw;
        int v;
        if (F(int, s, 15320) > 0)
            v = F(double, s, 16456) > g_ad64b678 ? 1 : -1;
        else
            v = F(double, s, 16456) > g_ad64b678 ? -1 : 1;
        Ad_Show(s, v);
        double lt = F(double, s, 16568);
        if (-edge > lt)
            F(short, s, 10054) = sa > g_ad64b678 ? 100 : 20;
        else if (lt > edge)
            F(short, s, 10054) = sa < g_ad64b678 ? 100 : 20;
        if (F(float, s, 13100) < g_ad64c320)
        {
            F(int, s, 10000) = 0;
            F(float, s, 9996) = (float)h;
        }
        else if (F(float, s, 13100) > g_ad64d350)
        {
            F(int, s, 9996) = 0;
            F(float, s, 10000) = (float)grip;
        }
        else
        {
            F(int, s, 10000) = 0;
            F(int, s, 9996) = 0;
        }
        break;
    }
    case 2:
    {
        // backing out
        Ad_Show(s, 0);
        double a = 1.0;
        if (F(double, s, 16440) < g_ad64b670)
            a = F(double, s, 16440);
        if (a * g_ad64b698 < sp)
        {
            F(int, s, 9996) = 0;
            F(float, s, 10000) = (float)grip;
        }
        if (a * g_ad64b7c0 > sp)
        {
            F(int, s, 10000) = 0;
            F(float, s, 9996) = (float)h;
        }
        double left = F(double, s, 16440) - F(double, F(BYTE*, F(BYTE*, s, 16576), 52), 2960);
        F(double, s, 16440) = left;
        if (left < g_ad64b678 || (CallC<int>(Ad_Look, s, 1) & 8))
        {
            F(short, s, 10054) = 100;
            F(int, s, 16492) = 1;
        }
        break;
    }
    case 1:
    {
        // driving
        DWORD f = F(DWORD, s, 16496);
        if ((f & 2) && (f & 8) && !(Ts_Flags(F(int, F(BYTE*, s, 16576), 20636)) & 0x4000))
            F(double, s, 16560) = 9.0;
        if (F(double, s, 16552) * g_ad64b6e0 + hw < x87_fabs(oldLat))
        {
            int r = CallC<int>(Ad_Look, s, 0);
            if (r)
            {
                if (r & 8)
                {
                    Ad_Show(s, 0);
                    F(float, s, 10000) = 1.0f;
                    F(int, s, 9996) = 0;
                }
                else
                {
                    F(int, s, 10000) = 0;
                    Ad_Show(s, (r & 1) ? -0.3 : 0.3);
                    if (F(float, s, 13100) > g_ad64c320)
                        F(float, s, 9996) = 0.3f;
                    else
                        F(int, s, 9996) = 0;
                }
                break;
            }
        }
        if (F(BYTE, s, 16496) & 8)
            CallThis(Ad_Steer, s, 0.0, 0.0, 0.0);
        else
            CallThis(Ad_Steer, s, turnRate, F(double, s, 16568), sa);
        Ai_ThrottleBrake(s, 0, pid.d[0], pid.d[1], pid.d[2], pid.d[3], pid.d[4], pid.d[5], pid.d[6], pid.d[7], target,
                         sa, v332);
        break;
    }
    }
    g_ad6962c4 = Ad_Clock();
    Ad_Finish(s, &g_ad672a68);
}

} // extern "C"

// === the target speed of a car by its id (FUN_4d5c50)

extern "C" void Sg_ResGet(); KING_ALIAS("_Sg_ResGet", "@Resources_Get@12")
extern "C" void Sg_Gear(); KING_ALIAS("_Sg_Gear", "_Gear_Choose")
KING_VAR(BYTE*, g_sg696ca0, 696ca0)
KING_VAR(int, g_sg68b990, 68b990)
KING_VAR(double, g_sg64ca48, 64ca48)
KING_VAR(double, g_sg64d0c0, 64d0c0)

extern "C" {

// [FUN_4d5c50] the speed the AI car stored under id should aim for (64ca48 when there is none, 0 for a player with
// flag 0x8000000): its offset across the road, the gear chosen for it and the target speed from that
double __cdecl Ai_CarTargetSpeed(int id)
{
    BYTE* res = CallThis<BYTE*>(Sg_ResGet, g_sg696ca0 + 704, id);
    if (!res)
        return g_sg64ca48;
    BYTE* car = F(BYTE*, res, 28);
    if (!car)
        return g_sg64ca48;
    BYTE* cab = F(BYTE*, car, 21600);
    if (!cab)
        return g_sg64ca48;
    int player = F(int, car, 20636);
    if (player == g_sg68b990)
        return g_sg64ca48;
    int gflags = 0, mask = 0;
    double best = -1.0;
    double len = Vec3_Length(&F(Vec3, car, 20700));
    double pedal = F(double, cab, 16560);
    Matrix t;
    LrMat m;
    memcpy(&m, CallThis<const Matrix*>(Cs_M1, car + 16, &t), 48);
    LrPos pid;
    Ts_PositionId(&pid, player);
    double hw, v24;
    int fl;
    double lat = Road_LateralOffset(pid, m, player, 0, &hw, &v24, &fl);
    double target = 30.0;
    if (Ts_Flags(player) & 0x8000000)
        return 0;
    int lvl2 = CallThis<int>(As_Level, cab, hw) << 1;
    double k = F(double, cab, 16552);
    float latf = (float)lat;
    CallC<double>(Sg_Gear, car, lvl2, hw, v24, k, fl, pedal, ((int*)&len)[0], ((int*)&len)[1], F(int, cab, 16500),
                  F(int, cab, 16504), &best, &gflags, &mask, &lvl2, *(int*)&latf);
    if (!(Ts_Flags(player) & 0x4000))
    {
        if (best > g_ad64b678 && best < g_sg64d0c0 && F(int, cab, 16504))
            target = best;
        if (gflags & 8)
            pedal = 9.0;
    }
    double v40 = (lvl2 + 1) * k * g_ad64b6e0 + lat;
    return Ai_TargetSpeed((BYTE*)player, F(int, cab, 16504), target, 0.0, 1.0, v40, 0, 30.0, pedal);
}

} // extern "C"

// === a burst of debris and puffs (FUN_4bba00)

extern "C" void Fb_Pool(); KING_ALIAS("_Fb_Pool", "_Pool_Prepare")
extern "C" void Fb_Debris(); KING_ALIAS("_Fb_Debris", "@Debris2_Construct@32")
extern "C" void Fb_Puff(); KING_ALIAS("_Fb_Puff", "@Effect59e9a0_Construct@36")
extern "C" void Fb_Material(); KING_ALIAS("_Fb_Material", "@MarineTask_GetMaterial@8")
extern "C" void Fb_AaTr(); KING_ALIAS("_Fb_AaTr", "_Fx_VehicleIsAaTr")
extern "C" void Fb_DbAc(); KING_ALIAS("_Fb_DbAc", "_Fx_VehicleIsDbAc")
extern "C" void Fb_MulAssign(); KING_ALIAS("_Fb_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Fb_AddAssign(); KING_ALIAS("_Fb_AddAssign", "@Vec3_AddAssign@16")
KING_VAR(Vec3, g_fbUp, 702630)
KING_VAR(Vec3, g_fbDown, 702640)
KING_VAR(float, g_fb64c348, 64c348)
KING_VAR(float, g_fb64c254, 64c254)
KING_VAR(float, g_fb64c1d0, 64c1d0)
KING_VAR(float, g_fb64c250, 64c250)
KING_VAR(float, g_fb64c350, 64c350)
KING_VAR(float, g_fb69607c, 69607c)
KING_VAR(float, g_fb696038, 696038)
KING_VAR(float, g_fb64c258, 64c258)
KING_VAR(float, g_fb64c330, 64c330)
KING_VAR(float, g_fb64c34c, 64c34c)
KING_VAR(BYTE*, g_fb6f33b0, 6f33b0)
KING_VAR(BYTE*, g_fb6f6aa4, 6f6aa4)
KING_VAR(BYTE*, g_fb6f6ab0, 6f6ab0)
KING_VAR(int, g_fb696224, 696224)
KING_VAR(int, g_fb69621c, 69621c)
KING_VAR(int, g_fb696218, 696218)
KING_VAR(int, g_fb696214, 696214)
KING_VAR(BYTE, g_fb69618c, 69618c)
KING_VAR(int, g_fb6949f8, 6949f8)
KING_VAR(BYTE*, g_fb6d2078, 6d2078)

static float Fb_Rand()
{
    return (float)(rand() * (double)g_fb64c254);
}

// a puff from the pool 6f6ab0 at pos, moving with the player's car plus e; 0 when the pool is empty
static BYTE* Fb_NewPuff(const Vec3* pos, const Vec3& e, float w, int table, int last)
{
    BYTE* q = CallC<BYTE*>(Fb_Pool, 212, g_fb6f6ab0);
    if (!q)
        return 0;
    const Vec3& base = F(Vec3, F(BYTE*, g_fb6d2078, 616), 432);
    Vec3 pv;
    pv.x = (float)((double)base.x + e.x);
    pv.y = (float)((double)base.y + e.y);
    pv.z = (float)((double)base.z + e.z);
    float wk = (float)(w * (double)g_fb64c34c);
    return CallThis<BYTE*>(Fb_Puff, q, g_fb696224, pos, &pv, *(int*)&wk, &(&g_fb6949f8)[table], 1, last);
}

extern "C" {

// [FUN_4bba00] a burst of n bits at pos spreading around dir (speed spd, size size): debris (pool 6f6aa4) with
// debris on, puffs (pool 6f6ab0) with smoke on, each carrying a28 / a32; with a40 debris and puffs, else only
// puffs when a36
void __cdecl Fx_Burst(const Vec3* pos, const Vec3* dir, const Vec3* vel, float spd, unsigned n, float size, int a28,
                      int a32, int a36, int a40)
{
    Vec3 e = {0, 0, 0};
    const Vec3& R = dir->x > g_fb64c348 || dir->y > g_fb64c348 ? g_fbUp : g_fbDown;
    Vec3 c, a, t, b0, b;
    c.x = (float)((double)R.z * dir->y - (double)R.y * dir->z);
    c.y = (float)((double)R.x * dir->z - (double)R.z * dir->x);
    c.z = (float)((double)R.y * dir->x - (double)R.x * dir->y);
    a = *Vec3_Normalized(&t, &c);
    b0.x = (float)((double)a.z * dir->y - (double)a.y * dir->z);
    b0.y = (float)((double)a.x * dir->z - (double)a.z * dir->x);
    b0.z = (float)((double)a.y * dir->x - (double)a.x * dir->y);
    b = *Vec3_Normalized(&t, &b0);
    for (unsigned i = 0; i < n; i++)
    {
        float r1 = Fb_Rand();
        float w = (float)((((double)Fb_Rand() - r1) * g_fb64c1d0 + g_fb64c250) * size);
        double r3 = rand() * (double)g_fb64c254;
        float bz = (float)(b.z * r3), by = (float)(b.y * r3), bx = (float)(b.x * r3);
        double r4 = rand() * (double)g_fb64c254;
        float az = (float)(a.z * r4), ay = (float)(a.y * r4), ax = (float)(a.x * r4);
        e.x = (float)((double)ax + bx);
        e.y = (float)((double)ay + by);
        e.z = (float)((double)az + bz);
        float r5 = Fb_Rand();
        float k = (float)((((double)Fb_Rand() - r5) * g_fb64c350 + g_fb64c250) * spd);
        CallThis(Fb_MulAssign, &e, &t, k);
        float r6 = Fb_Rand();
        double md = ((double)Fb_Rand() - r6) * g_fb64c350 + g_fb64c250;
        float m = (float)md;
        Vec3 v;
        v.z = (float)(md * vel->z);
        v.y = (float)((double)m * vel->y);
        v.x = (float)((double)m * vel->x);
        CallThis(Fb_AddAssign, &e, &t, &v);
        if (a40)
        {
            if (!a36)
                continue;
            if (g_fb69607c > g_fb64c258 && !CallThis<int>(Fb_Material, g_fb6f33b0) && !CallC<int>(Fb_AaTr) &&
                !CallC<int>(Fb_DbAc))
            {
                BYTE* p = CallC<BYTE*>(Fb_Pool, 204, g_fb6f6aa4);
                if (p)
                    p = CallThis<BYTE*>(Fb_Debris, p, g_fb696224, pos, &e, *(int*)&w, &g_fb69618c, g_fb69621c);
                if (!p)
                    return;
                F(int, p, 192) = a28;
            }
            if (g_fb696038 > g_fb64c258 && Fb_Rand() > g_fb64c330)
            {
                BYTE* q = Fb_NewPuff(pos, e, w, g_fb696218, 1);
                if (!q)
                    return;
                F(int, q, 192) = a28;
                F(int, q, 200) = a32;
            }
        }
        if (a36 && !a40 && g_fb69607c > g_fb64c258 && Fb_Rand() > g_fb64c330)
        {
            BYTE* q = Fb_NewPuff(pos, e, w, g_fb696214, 0);
            if (!q)
                return;
            F(int, q, 192) = a28;
            F(int, q, 200) = a32;
        }
    }
}

} // extern "C"

// === sparks along a scrape (FUN_4cb8d0)

extern "C" void Sc_Get104(); KING_ALIAS("_Sc_Get104", "@Table124_Get104@16")
extern "C" void Sc_Add(); KING_ALIAS("_Sc_Add", "@Table124_Add@28")
extern "C" void Sc_Spark(); KING_ALIAS("_Sc_Spark", "@Spark_Construct@40")
KING_VAR(BYTE*, g_sc6f6a78, 6f6a78)
KING_VAR(BYTE*, g_sc6f6ab8, 6f6ab8)
KING_VAR(BYTE*, g_sc6d2078, 6d2078)
KING_VAR(BYTE, g_sc695cd0, 695cd0)
KING_VAR(BYTE, g_sc695cbc, 695cbc)
KING_VAR(BYTE, g_sc695ccc, 695ccc)
KING_VAR(float, g_sc64c328, 64c328)
KING_VAR(float, g_sc64c970, 64c970)
KING_VAR(float, g_sc64c264, 64c264)
KING_VAR(float, g_sc64c96c, 64c96c)
KING_VAR(float, g_sc64c968, 64c968)
KING_VAR(float, g_sc64c450, 64c450)
KING_VAR(float, g_sc64c964, 64c964)
KING_VAR(float, g_sc64c254, 64c254)
KING_VAR(float, g_sc64c258, 64c258)
KING_VAR(float, g_sc64c960, 64c960)
KING_VAR(double, g_sc64c958, 64c958)
KING_VAR(float, g_sc64c954, 64c954)
KING_VAR(float, g_sc64c950, 64c950)
KING_VAR(float, g_sc64c94c, 64c94c)
KING_VAR(float, g_sc64c250, 64c250)

extern "C" {

// [FUN_4cb8d0] car c scraping at pos along dir (contact key, kind 0-2): the scrape recorded (table 6f6a78) and as
// many new sparks (pool 6f6ab8) as its count grew by, a wobbling number by the place, thrown along dir and with the
// car's speed (not for the player's own car seen from the cab)
int __cdecl Car_ScrapeSparks(BYTE* c, const Vec3* pos, const Vec3* dir, int key, int kind)
{
    if (c == F(BYTE*, g_sc6d2078, 616) && !F(int, g_sc6d2078, 1400))
        return 0;
    unsigned prev = CallThis<unsigned>(Sc_Get104, g_sc6f6a78, c, key);
    CallThis(Sc_Add, g_sc6f6a78, c, pos, dir, key, kind);
    double A = x87_sin(x87_sin(pos->y * (double)g_sc64c328) + pos->x * (double)g_sc64c970);
    double B = x87_sin(x87_sin(pos->x * (double)g_sc64c328) + pos->y * (double)g_sc64c970);
    if (!(A > g_sc64c264))
        A = -A;
    if (!(B > g_sc64c264))
        B = -B;
    int n;
    if (kind == 0 || kind == 2)
    {
        int b = (int)(B * g_sc64c96c);
        n = (int)(A + A) - b + 5;
    }
    else if (kind == 1)
    {
        int b = (int)(B * g_sc64c968);
        n = (int)(A * g_sc64c450) - b + 30;
    }
    else
        return 0;
    if (n < 0 || prev >= (unsigned)n)
        return 0;
    unsigned cnt = n - prev;
    Vec3 vel = F(Vec3, c, 432);
    float sp = (float)(Vec3_Length(&vel) * g_sc64c970);
    if (sp > g_sc64c964)
        sp = 0.9f;
    float scale = 0, off = 0;
    if (kind == 0 || kind == 2)
    {
        off = 0.05f;
        scale = 1.5f;
    }
    else if (kind == 1)
    {
        off = 0.05f;
        scale = 3.0f;
    }
    const BYTE* tables[3] = {&g_sc695ccc, &g_sc695cbc, &g_sc695cd0};
    while (cnt--)
    {
        double r;
        if (rand() * (double)g_sc64c254 < g_sc64c258)
            r = rand() * (double)g_sc64c960;
        else if (rand() * (double)g_sc64c254 < g_sc64c958)
            r = rand() * (double)g_sc64c954 + g_sc64c950;
        else
            r = rand() * (double)g_sc64c954 + g_sc64c94c;
        float d = (float)(r * scale + off);
        BYTE* p = CallC<BYTE*>(Fb_Pool, 188, g_sc6f6ab8);
        if (!p)
            continue;
        Vec3 v, q;
        v.x = (float)(vel.x * (double)g_sc64c964);
        v.y = (float)(vel.y * (double)g_sc64c964);
        v.z = (float)(vel.z * (double)g_sc64c964);
        float dy = (float)((double)d * dir->y), dx = (float)((double)d * dir->x);
        q.z = (float)((double)d * dir->z + pos->z);
        q.y = (float)((double)dy + pos->y);
        q.x = (float)((double)dx + pos->x);
        float spread = (float)((double)g_sc64c250 - sp);
        CallThis(Sc_Spark, p, c, key, &q, &v, *(int*)&spread, 1065353216, tables[kind], 1);
    }
    return 0;
}

} // extern "C"

// === the player's car hitting things, and its frame given to the AI (FUN_4cfe70)

extern "C" void Ph_54f000(); KING_ALIAS("_Ph_54f000", "@Car_54f000@16")
extern "C" void Ph_PlayerData(); KING_ALIAS("_Ph_PlayerData", "_Ai_GetPlayerData")
extern "C" void Ph_KindFlag(); KING_ALIAS("_Ph_KindFlag", "_Eff_KindFlag")
extern "C" void Ph_Near(); KING_ALIAS("_Ph_Near", "_Fx_VehicleNear")
extern "C" void Ph_First(); KING_ALIAS("_Ph_First", "_List68cf6c_First")
extern "C" void Ph_Next(); KING_ALIAS("_Ph_Next", "_List68cf6c_Next")
extern "C" void Ph_Field8(); KING_ALIAS("_Ph_Field8", "_Rec_Field8")
extern "C" void Ph_ForEach(); KING_ALIAS("_Ph_ForEach", "_Array_ForEach@16")
extern "C" void Ph_InitState(); KING_ALIAS("_Ph_InitState", "@Fx_InitState@8")
extern "C" void Ph_Present(); KING_ALIAS("_Ph_Present", "_Ai_GetPresentData")
extern "C" void Ph_Free(); KING_ALIAS("_Ph_Free", "@VecB_FreeStorage@8")
extern "C" void Ph_CarId(); KING_ALIAS("_Ph_CarId", "_Player_CarId")
extern "C" void Ph_SetMatrix(); KING_ALIAS("_Ph_SetMatrix", "_Ai_SetPlayerMatrix")
extern "C" void Ph_SetMatrixRoom(); KING_ALIAS("_Ph_SetMatrixRoom", "_Ai_SetPlayerMatrixInRoom")
extern "C" void Ph_Get800(); KING_ALIAS("_Ph_Get800", "@SceneObj_Get800@8")
extern "C" void Ph_M1(); KING_ALIAS("_Ph_M1", "@SceneObj_Matrix1@12")
extern "C" void Ph_Find(); KING_ALIAS("_Ph_Find", "_Scene_FindObject")
extern "C" void Ph_HitObject(); KING_ALIAS("_Ph_HitObject", "_Query_HitObject")
extern "C" void Ph_Make(); KING_ALIAS("_Ph_Make", "_AI_Make449c40")
extern "C" void Ph_Now(); KING_ALIAS("_Ph_Now", "_Tsc_Now")
extern "C" void Ph_ZeroThis(); KING_ALIAS("_Ph_ZeroThis", "@Vec3_ZeroThis@8")
KING_VAR(BYTE*, g_ph6d2078, 6d2078)
KING_VAR(int, g_ph6f33c0, 6f33c0)
KING_VAR(int, g_ph68b990, 68b990)
KING_VAR(void*, g_ph68cf6c, 68cf6c)
KING_VAR(BYTE, g_ph7025f0, 7025f0)
KING_VAR(double, g_ph64b848, 64b848)
KING_VAR(double, g_ph64b730, 64b730)
KING_VAR(double, g_ph64b670, 64b670)
KING_VAR(double, g_ph64b5b8, 64b5b8)
KING_VAR(double, g_ph64b690, 64b690)
KING_VAR(double, g_ph64cab8, 64cab8)
KING_VAR(double, g_ph64b6a0, 64b6a0)
KING_VAR(double, g_ph64b678, 64b678)

// a hit on the player's tally at off (value, flags at off + 8)
static void Ph_Hit(BYTE* pl, int off, double v, int kind, BYTE* cab, bool stamp)
{
    F(double, pl, off) = v + F(double, pl, off);
    F(int, pl, off + 8) |= CallC<int>(Ph_KindFlag, kind);
    if (stamp)
        F(double, cab, 16544) = CallC<double>(Ph_Now);
}

static bool Ph_IsNear(BYTE* c)
{
    Matrix t;
    const float* m = CallThis<const Matrix*>(Ph_M1, c + 16, &t)->m;
    return CallC<int>(Ph_Near, m[9], m[10], m[11]) != 0;
}

static bool Ph_Calm(BYTE* pl)
{
    return !(F(double, pl, 208) > g_ph64b678) && !F(BYTE, pl, 200) && !(F(double, pl, 240) > g_ph64b678) &&
           !(F(double, pl, 248) > g_ph64b678) && !(F(double, pl, 256) > g_ph64b678) && !F(BYTE, pl, 264);
}

extern "C" {

// [FUN_4cfe70] car c with AI flags *flags: for the player's own car the hits it takes from cars, AI players and
// bits of road (+208.. on its player, the cab's time +16544) set the damage state +20944 (11/1 hurt, 21 crashed,
// back to 0 when calm); then a car following its route (*flags & 1) gives its frame (+21040 offset) to the AI, in
// its room when it has one
void __cdecl Car_HitsAndFrame(BYTE* c, DWORD* flags)
{
    if (c == F(BYTE*, g_ph6d2078, 616))
    {
        g_ph6f33c0 = 0;
        BYTE* cab = F(BYTE*, c, 21600);
        BYTE* pl = F(BYTE*, c, 20636);
        if (pl == (BYTE*)g_ph68b990 || (*flags & 0x4000))
            goto route;
        BYTE* oc = 0;
        int kind;
        float amount;
        BYTE* other = CallThis<BYTE*>(Ph_54f000, c, &kind, &amount);
        if (other && F(int, other, 3568) == 2)
            oc = other - 16;
        if (kind)
        {
            if (oc)
            {
                double v = CallVirt<double>(oc, 84) * g_ph64b848 + g_ph64b730;
                int opl = F(int, oc, 20636);
                bool done = false;
                if (opl != g_ph68b990)
                {
                    BYTE data[200];
                    CallC(Ph_PlayerData, opl, data);
                    int st = F(int, data, 8);
                    if (st == 6 || st == 5)
                    {
                        if (v > g_ph64b670)
                            Ph_Hit(pl, st == 6 ? 224 : 208, v + v, kind, cab, true);
                        done = true;
                    }
                }
                if (!done && Ph_IsNear(c) && v > g_ph64b670)
                    Ph_Hit(pl, 208, v, kind, cab, true);
            }
            else if (other && Ph_IsNear(c))
            {
                int id = F(int, other, 344) / 1000;
                double x = 0;
                bool have = false;
                if (id == 231)
                {
                    void* it;
                    CallC(Ph_First, &it);
                    while (it != g_ph68cf6c)
                    {
                        int f = CallC<int>(Ph_Field8, it);
                        if ((f & 64) && !(f & 32) && *F(int*, it, 8) == F(int, other, 344))
                            break;
                        void* nx;
                        it = *CallC<void**>(Ph_Next, &nx, it);
                    }
                    if (it == g_ph68cf6c)
                        goto calm;
                    __declspec(align(8)) BYTE P[288];
                    F(int, P, 0) = 0;
                    CallC(Ph_ForEach, P + 4, 12, 4, Ph_ZeroThis);
                    memcpy(P + 4, &g_ph7025f0, 48);
                    memset(P + 52, 0, 12);
                    F(int, P, 64) = 0;
                    F(int, P, 68) = 0;
                    F(int, P, 72) = -1;
                    F(int, P, 76) = -1;
                    CallThis(Ph_InitState, P + 80);
                    F(int, P, 272) = -1;
                    F(int, P, 276) = -1;
                    int carId;
                    CallC(Ph_CarId, &carId);
                    CallC(Ph_Present, carId, it, P);
                    x = amount * F(double, P, 200) * g_ph64b5b8;
                    CallThis(Ph_Free, P + 168);
                    CallThis(Ph_Free, P + 156);
                    CallThis(Ph_Free, P + 144);
                    CallThis(Ph_Free, P + 132);
                    have = true;
                }
                else if (id == 201 || id == 202)
                {
                    x = amount * g_ph64b690;
                    have = true;
                }
                if (have && x > g_ph64b670)
                    Ph_Hit(pl, 208, x, kind, cab, false);
            }
        }
    calm:
        {
            double t = F(double, cab, 16544) + g_ph64cab8;
            if (CallC<double>(Ph_Now) > t)
            {
                F(double, pl, 208) = 0;
                F(int, pl, 216) = 0;
                F(double, pl, 224) = 0;
                F(int, pl, 232) = 0;
            }
            int& st = F(int, c, 20944);
            if (st < 30)
            {
                if (!(F(double, pl, 208) > g_ph64b6a0))
                {
                    if (st < 20 && Ph_Calm(pl))
                        st = 0;
                }
                else if (!st || st >= 20)
                    st = 11;
                else if (st > 10 && F(double, pl, 208) > g_ph64b730)
                    st -= 10;
            }
            if (!Ph_Calm(pl) && st < 30)
            {
                if (!st || st >= 20)
                    st = 1;
                else if (st > 10)
                    st -= 10;
            }
            if (!(F(double, pl, 224) > g_ph64b6a0))
            {
                if (st >= 20 && st < 30 && !(F(double, pl, 224) > g_ph64b678) && !F(BYTE, pl, 201))
                    st = 0;
            }
            else if (!st)
                st = 21;
            if ((F(double, pl, 224) > g_ph64b678 || F(BYTE, pl, 201)) && !st)
                st = 21;
        }
    }
route:
    if (!(*flags & 1))
        return;
    int pl = F(int, c, 20636);
    if (pl == g_ph68b990)
        return;
    Vec3 o = F(Vec3, c, 21040);
    Matrix m1, T, W, m2;
    const Matrix* M1 = CallThis<const Matrix*>(Ph_M1, c + 16, &m1);
    Matrix_Multiply(&W, Matrix_Translation(&T, o), M1);
    BYTE* obj = CallC<BYTE*>(Ph_Find, 0, F(int, CallThis<BYTE*>(Ph_Get800, c + 16), 12));
    const float* p = &CallThis<const Matrix*>(Ph_M1, c + 16, &m2)->m[9];
    BYTE* q = CallC<BYTE*>(Ph_HitObject, obj, p, &W.m[9]);
    int room;
    CallC(Ph_Make, &room, F(int, q, 12));
    if (room == -1)
    {
        if (F(BYTE*, g_ph6d2078, 616) == c)
            return;
        CallC(Ph_SetMatrix, pl, W.m);
        return;
    }
    CallC(Ph_SetMatrixRoom, room, pl, W.m);
}

} // extern "C"

// === a blow: ten fires and some debris around a point (FUN_4d9460)

extern "C" void Bw_Kind(); KING_ALIAS("_Bw_Kind", "@Object_ConstructKind@16")
extern "C" void Bw_Attach(); KING_ALIAS("_Bw_Attach", "@Owner_Attach@12")
extern "C" void Bw_Set(); KING_ALIAS("_Bw_Set", "@Vec3_SetThis@20")
extern "C" void Bw_Camera(); KING_ALIAS("_Bw_Camera", "@Camera_Get@12")
extern "C" void Bw_Play(); KING_ALIAS("_Bw_Play", "@Sound4dd920_Play@36")
KING_VAR(AsmLabel, g_bw64d404, 64d404)
KING_VAR(Vec3, g_bwDown, 702640)
KING_VAR(Vec3, g_bwUp, 702630)
KING_VAR(float, g_bw64c48c, 64c48c)
KING_VAR(float, g_bw64c254, 64c254)
KING_VAR(float, g_bw64d3f4, 64d3f4)
KING_VAR(float, g_bw64c320, 64c320)
KING_VAR(float, g_bw64d400, 64d400)
KING_VAR(float, g_bw64c304, 64c304)
KING_VAR(float, g_bw64c308, 64c308)
KING_VAR(float, g_bw64d3fc, 64d3fc)
KING_VAR(BYTE**, g_bw696720, 696720)

static void Bw_Vec(Vec3* v, float x, float y, float z)
{
    CallThis(Bw_Set, v, x, y, z);
}

extern "C" {

// [FUN_4d9460] blow s of scene v at pos ("_Blow", kind 105): ten fires thrown out at random headings around pos
// (and sometimes debris), the blow sound at the camera, then s given to the scene's owner list (+2992)
BYTE* __fastcall Blow_Construct(BYTE* s, void*, BYTE* v, const Vec3* pos)
{
    CallThis(Bw_Kind, s, 105, "_Blow");
    Vec3& at = F(Vec3, s, 36);
    at = *pos;
    F(void*, s, 0) = &g_bw64d404;
    Vec3 dir = {0, 0, 0};
    F(BYTE*, s, 32) = v;
    for (int i = 0; i < 10; i++)
    {
        float r = (float)(rand() * (double)g_bw64c48c);
        double a = rand() * (double)g_bw64c254;
        Matrix R;
        const float* m = Matrix_RotationZ(&R, (float)(g_bw64d3f4 * a + g_bw64d3f4 * a))->m;
        const Vec3& d = g_bwDown;
        float hx = (float)((double)d.z * m[6] + (double)d.y * m[3] + (double)d.x * m[0] + m[9]);
        float hy = (float)((double)d.z * m[7] + (double)d.y * m[4] + (double)d.x * m[1] + m[10]);
        float hz = (float)((double)d.z * m[8] + (double)d.y * m[5] + (double)d.x * m[2] + m[11]);
        double uz = g_bwUp.z * (double)g_bw64c320;
        float uy = (float)(g_bwUp.y * (double)g_bw64c320);
        float ux = (float)(g_bwUp.x * (double)g_bw64c320);
        Vec3 t;
        Bw_Vec(&t, (float)((double)ux + hx), (float)((double)uy + hy), (float)(uz + hz));
        dir = t;
        Vec3 off;
        Bw_Vec(&off, (float)(hx * (double)r), (float)(hy * (double)r), (float)(hz * (double)r));
        BYTE* f = (BYTE*)operator new(108);
        if (f)
        {
            Vec3 p, q;
            Bw_Vec(&p, (float)((double)off.x + at.x), (float)((double)off.y + at.y), (float)((double)off.z + at.z));
            q = p;
            float size = (float)(rand() * (double)g_bw64d400 + g_bw64c304);
            f = Fire_Construct(f, 0, F(BYTE*, s, 32), &q, &dir, *(int*)&size);
        }
        CallVirt(f, 24, 1);
        if (rand() * (double)g_bw64c254 > g_bw64c308)
        {
            Vec3 b1, b2, b3, n;
            Bw_Vec(&b1, (float)(off.x * (double)g_bw64c320), (float)(off.y * (double)g_bw64c320),
                   (float)(off.z * (double)g_bw64c320));
            Bw_Vec(&b2, (float)((double)b1.x + g_bwUp.x), (float)((double)b1.y + g_bwUp.y),
                   (float)((double)b1.z + g_bwUp.z));
            Bw_Vec(&b3, (float)(b2.x * (double)g_bw64d3fc), (float)(b2.y * (double)g_bw64d3fc),
                   (float)(b2.z * (double)g_bw64d3fc));
            Vec3 vel = b3;
            CallC(FUN_4bb4c0, &at, Vec3_Normalized(&n, &dir), &vel, 0x40400000, 2, 0x3E99999A, 1);
        }
    }
    F(int, s, 52) = 0;
    Matrix cm, inv;
    Matrix_Inverse(&inv, CallThis<const Matrix*>(Bw_Camera, F(BYTE*, s, 32), &cm));
    rand();
    if (g_bw696720)
    {
        Vec3 h;
        Vec3_MulMatrix(&h, &at, &inv);
        CallThis(Bw_Play, g_bw696720, h.x, h.y, h.z, 0, 0x40490000, 0, 0x3FF00000);
    }
    CallThis(FUN_5d3570, F(BYTE*, s, 32) + 2992, s);
    return s;
}

} // extern "C"

// === a burst of debris (FUN_4bb4c0)

extern "C" void Db_Write(); KING_ALIAS("_Db_Write", "@Msg_Write120@20")
extern "C" void Db_Debris(); KING_ALIAS("_Db_Debris", "@Debris_Construct@32")
KING_VAR(BYTE*, g_db6cec6c, 6cec6c)
KING_VAR(BYTE*, g_db696224, 696224)
KING_VAR(float, g_db696068, 696068)
KING_VAR(float, g_db64c1d4, 64c1d4)
KING_VAR(float, g_db64c1d8, 64c1d8)
KING_VAR(BYTE*, g_db6f6aa0, 6f6aa0)
KING_VAR(int, g_db696214, 696214)
KING_VAR(BYTE, g_db695908, 695908)

static void Db_Set(Vec3* v, double x, double y, double z)
{
    CallThis(Bw_Set, v, (float)x, (float)y, (float)z);
}

extern "C" {

// [FUN_4bb4c0] n bits of debris (pool 6f6aa0) at pos spreading around dir with speed spd, the car's velocity vel
// and size size; with net set in a network game the burst is sent on (message 957) first; none with debris off
void __cdecl Fx_DebrisBurst(const Vec3* pos, const Vec3* dir, const Vec3* vel, float spd, unsigned n, float size,
                            int net)
{
    if (net && (F(int, g_db6cec6c, 1300) == 1 || F(int, g_db6cec6c, 1316)))
    {
        BYTE rec[52];
        rec[0] = 3;
        F(Vec3, rec, 4) = *pos;
        F(Vec3, rec, 16) = *dir;
        F(Vec3, rec, 28) = *vel;
        F(float, rec, 40) = spd;
        F(unsigned, rec, 44) = n;
        F(float, rec, 48) = size;
        CallThis(Db_Write, g_db696224 + 8336, 957, rec, 52);
    }
    if (g_db696068 < g_fb64c258)
        return;
    Vec3 e = {0, 0, 0};
    const Vec3& R = dir->x > g_fb64c348 || dir->y > g_fb64c348 ? g_fbUp : g_fbDown;
    Vec3 c, t, a, b0, b;
    Db_Set(&c, (double)R.z * dir->y - (double)R.y * dir->z, (double)R.x * dir->z - (double)R.z * dir->x,
           (double)R.y * dir->x - (double)R.x * dir->y);
    Vec3 c2 = c;
    a = *Vec3_Normalized(&t, &c2);
    Db_Set(&b0, (double)a.z * dir->y - (double)a.y * dir->z, (double)a.x * dir->z - (double)a.z * dir->x,
           (double)a.y * dir->x - (double)a.x * dir->y);
    Vec3 b2 = b0;
    b = *Vec3_Normalized(&t, &b2);
    for (; n; n--)
    {
        float r1 = Fb_Rand();
        float w = (float)((((double)Fb_Rand() - r1) * g_fb64c1d0 + g_fb64c250) * size);
        double r3 = rand() * (double)g_fb64c254;
        Vec3 bt;
        Db_Set(&bt, r3 * b.x, b.y * r3, b.z * r3);
        Vec3 bt2 = bt;
        double r4 = rand() * (double)g_fb64c254;
        float az = (float)(a.z * r4), ay = (float)(a.y * r4);
        double ax = r4 * a.x;
        Vec3 e2;
        Db_Set(&e2, bt2.x + ax, (double)bt2.y + ay, (double)bt2.z + az);
        e = e2;
        float r5 = Fb_Rand();
        float k = (float)((((double)Fb_Rand() - r5) * g_db64c1d4 + g_fb64c250) * spd);
        CallThis(Fb_MulAssign, &e, &t, k);
        float r7 = Fb_Rand();
        double md = ((double)Fb_Rand() - r7) * g_db64c1d8 + g_fb64c250;
        float m = (float)md;
        Vec3 v;
        v.z = (float)(md * vel->z);
        v.y = (float)((double)m * vel->y);
        v.x = (float)((double)m * vel->x);
        CallThis(Fb_AddAssign, &e, &t, &v);
        BYTE* p = CallC<BYTE*>(Fb_Pool, 180, g_db6f6aa0);
        if (p)
            CallThis(Db_Debris, p, g_db696224, pos, &e, *(int*)&w, &g_db695908, g_db696214);
    }
}

} // extern "C"

// === a wreck burning out (a method at $L_4db8a0)

extern "C" void Wk_M1(); KING_ALIAS("_Wk_M1", "@SceneObj_Matrix1@12")
extern "C" void Wk_Cloud(); KING_ALIAS("_Wk_Cloud", "@SmokeCloud_Construct@16")
extern "C" void Wk_Notify(); KING_ALIAS("_Wk_Notify", "@TankNotify_Construct@24")
KING_VAR(BYTE*, g_wk696704, 696704)
KING_VAR(float, g_wk64c624, 64c624)
KING_VAR(float, g_wk64d4e8, 64d4e8)

// point p of car c's body (+16) in the world, then through the car's matrix +708: x stays on the FPU, as the
// original's chains keep it
static void Wk_Place(BYTE* c, const float* p, float* q)
{
    float Mo[12];
    memcpy(Mo, c + 16 + 692, 48);
    Matrix t;
    const float* m = CallThis<const Matrix*>(Wk_M1, c + 16, &t)->m;
    double px = (double)m[6] * p[2] + (double)m[3] * p[1] + (double)m[0] * p[0] + m[9];
    float py = (float)((double)m[7] * p[2] + (double)m[1] * p[0] + (double)m[4] * p[1] + m[10]);
    float pz = (float)((double)m[8] * p[2] + (double)m[2] * p[0] + (double)m[5] * p[1] + m[11]);
    double qx = (double)Mo[6] * pz + (double)Mo[3] * py + Mo[0] * px + Mo[9];
    q[1] = (float)((double)Mo[7] * pz + (double)Mo[4] * py + Mo[1] * px + Mo[10]);
    q[2] = (float)((double)Mo[8] * pz + (double)Mo[5] * py + Mo[2] * px + Mo[11]);
    q[0] = (float)qx;
}

typedef void(__cdecl* WkHook)(const float* at, int, BYTE* car);

// the same for the fires, whose chains add their terms in another order
static void Wk_PlaceFire(BYTE* c, const float* p, float* q)
{
    float Mo[12];
    memcpy(Mo, c + 16 + 692, 48);
    Matrix t;
    const float* m = CallThis<const Matrix*>(Wk_M1, c + 16, &t)->m;
    double px = (double)m[3] * p[1] + (double)m[6] * p[2] + (double)p[0] * m[0] + m[9];
    float py = (float)((double)m[4] * p[1] + (double)m[7] * p[2] + (double)m[1] * p[0] + m[10]);
    float pz = (float)((double)m[5] * p[1] + (double)m[8] * p[2] + (double)m[2] * p[0] + m[11]);
    double qx = (double)Mo[3] * py + (double)Mo[6] * pz + Mo[0] * px + Mo[9];
    q[1] = (float)((double)Mo[4] * py + Mo[1] * px + (double)Mo[7] * pz + Mo[10]);
    q[2] = (float)((double)Mo[5] * py + Mo[2] * px + (double)Mo[8] * pz + Mo[11]);
    q[0] = (float)qx;
}

extern "C" {

// [$L_4db8a0] wreck w of car +680 in scene +676: at each of its 14 times (+88..) a smoke cloud at its point
// (+144.., in the car's frame) with a blast sound (one of two at random), after it the hook +684 told on the third
// frame; at +656 twelve fires at the points +368.. along +512.. and the notice to the tank with the biggest; at +668
// gone
void __fastcall Wreck_Frame(BYTE* w, void*)
{
    F(float, w, 672) = (float)(F(double, F(BYTE*, w, 676), 2960) + F(float, w, 672));
    for (int i = 0; i < 14; i++)
    {
        const float* pt = &F(float, w, 144 + 12 * i);
        if (!(F(float, w, 672) > F(float, w, 88 + 4 * i)))
            continue;
        if (F(int, w, 32 + 4 * i))
        {
            int& n = F(int, w, 312 + 4 * i);
            if (n == 3 && F(void*, w, 684))
                F(WkHook, w, 684)(pt, 0, F(BYTE*, w, 680));
            n++;
            continue;
        }
        F(int, w, 32 + 4 * i) = 1;
        float q[3];
        Wk_Place(F(BYTE*, w, 680), pt, q);
        BYTE* c = (BYTE*)operator new(96);
        if (c)
            c = CallThis<BYTE*>(Wk_Cloud, c, F(BYTE*, w, 676), q);
        CallVirt(c, 24, 1);
        Matrix cm, inv;
        Matrix_Inverse(&inv, CallThis<const Matrix*>(Bw_Camera, F(BYTE*, w, 676), &cm));
        BYTE* snd = (&g_wk696704)[rand() * (double)g_fb64c254 > g_fb64c258 ? 1 : 0];
        if (snd)
        {
            const float* M = inv.m;
            double hx = (double)M[6] * q[2] + (double)M[3] * q[1] + (double)q[0] * M[0] + M[9];
            float hy = (float)((double)M[7] * q[2] + (double)M[4] * q[1] + (double)M[1] * q[0] + M[10]);
            float hz = (float)((double)M[8] * q[2] + (double)M[5] * q[1] + (double)M[2] * q[0] + M[11]);
            CallThis(Bw_Play, snd, (float)hx, hy, hz, 0, 0x40490000, 0, 0x3FF00000);
        }
    }
    if (F(float, w, 672) > F(float, w, 656) && F(float, w, 660) == 0)
    {
        F(float, w, 660) = 1.0f;
        float big = 0;
        for (int j = 0; j < 12; j++)
        {
            float s = (float)(rand() * (double)g_wk64c624 + g_wk64d4e8);
            if (s > big)
                big = s;
            BYTE* f = (BYTE*)operator new(108);
            if (f)
            {
                float q[3];
                Wk_PlaceFire(F(BYTE*, w, 680), &F(float, w, 368 + 12 * j), q);
                f = Fire_Construct(f, 0, F(BYTE*, w, 676), (const Vec3*)q, &F(Vec3, w, 512 + 12 * j), *(int*)&s);
            }
            CallVirt(f, 24, 1);
        }
        BYTE* n = (BYTE*)operator new(52);
        if (n)
            CallThis(Wk_Notify, n, F(BYTE*, w, 676), F(void*, w, 684), F(BYTE*, w, 680), big);
    }
    if (F(float, w, 672) > F(float, w, 668))
    {
        CallVirt(w, 28, 1);
        return;
    }
    CallThis(FUN_5d32f0, w, 1);
}

} // extern "C"

// === a random point ahead of the camera onto the ground below it (a method at $L_4c3df0)

extern "C" void Sp_HAngle(); KING_ALIAS("_Sp_HAngle", "@MarineCamera_HorizontalAngle@8")
extern "C" void Sp_Lookup(); KING_ALIAS("_Sp_Lookup", "_Obj_Lookup4e5b22")
extern "C" void Sp_Visit(); KING_ALIAS("_Sp_Visit", "_Ring50_AddPoint")
extern "C" void Sp_Set(); KING_ALIAS("_Sp_Set", "@Vec3_SetThis@20")
extern "C" void Sp_Light(); KING_ALIAS("_Sp_Light", "_Light_Set@24")
KING_VAR(BYTE*, g_sp696224, 696224)
KING_VAR(BYTE*, g_sp6d2078, 6d2078)
KING_VAR(Vec3, g_spAhead, 702620)
KING_VAR(Vec3, g_spUp, 702630)
KING_VAR(float, g_sp64c258, 64c258)
KING_VAR(float, g_sp64c304, 64c304)
KING_VAR(float, g_sp64c2fc, 64c2fc)
KING_VAR(float, g_sp64c254, 64c254)
KING_VAR(float, g_sp64c448, 64c448)
KING_VAR(float, g_sp64c664, 64c664)

extern "C" {

// [$L_4c3df0] ring s gets a point in the camera's view wedge (half its horizontal angle each side, reaching further
// with the player's speed), dropped onto the scene's room (+488 +76) below it: the room's polygons visited with
// Ring50_AddPoint unless the drop is blocked
void __fastcall Ring50_DropPoint(BYTE* s, void*, BYTE* scene)
{
    Vec3 A = {0, 0, 0}, P = {0, 0, 0};
    BYTE* cam = g_sp696224;
    float ah = (float)(CallThis<double>(Sp_HAngle, F(BYTE*, cam, 488)) * g_sp64c258);
    Matrix R;
    Vec3 L, L2, t;
    Vec3_MulMatrix(&L, &g_spAhead, Matrix_RotationZ(&R, ah));
    Vec3_MulMatrix(&L2, &g_spAhead, Matrix_RotationZ(&R, -ah));
    L = *Vec3_MulMatrix(&t, &L, Matrix_RotationZ(&R, (float)F(double, g_sp696224, 2928)));
    L2 = *Vec3_MulMatrix(&t, &L2, Matrix_RotationZ(&R, (float)F(double, g_sp696224, 2928)));
    Vec3 vel = F(Vec3, F(BYTE*, g_sp6d2078, 616), 432);
    float reach = (float)(Vec3_Length(&vel) * g_sp64c304 + g_sp64c2fc);
    double r1 = rand() * (double)g_sp64c254;
    float c = (float)x87_cos(ah);
    float base = (float)(reach / (double)c);
    float k1 = (float)(r1 * g_sp64c448 / c + base);
    CallThis(Fb_MulAssign, &L, &t, k1);
    float k2 = (float)(rand() * (double)g_sp64c254 * g_sp64c448 / c + base);
    CallThis(Fb_MulAssign, &L2, &t, k2);
    BYTE* g = g_sp696224;
    Vec3 cp;
    cp.x = (float)F(double, g, 2904);
    cp.y = (float)F(double, g, 2912);
    cp.z = (float)F(double, g, 2920);
    A = cp;
    Vec3 p0;
    CallThis(Sp_Set, &p0, (float)((double)L2.x + cp.x), (float)((double)L2.y + cp.y), (float)((double)L2.z + cp.z));
    Vec3 p1 = p0, q, r;
    P = *Triangle_RandomPoint(&r, &A, Vec3_Add(&q, &A, &L), &p1);
    BYTE* room = F(BYTE*, scene, 488);
    if (room)
        room = F(BYTE*, room, 76);
    char blocked = 0;
    CallC(Sp_Lookup, room, &A, &P, &blocked);
    if (blocked)
        return;
    double ux = g_spUp.x * (double)g_sp64c664;
    float uy = (float)(g_spUp.y * (double)g_sp64c664);
    float uz = (float)(g_spUp.z * (double)g_sp64c664);
    struct { BYTE* ring; Vec3 at; } ctx;
    ctx.ring = s;
    ctx.at.x = (float)(P.x - ux);
    ctx.at.y = (float)((double)P.y - uy);
    ctx.at.z = (float)((double)P.z - uz);
    CallStd(Sp_Light, room, ctx.at.x, ctx.at.y, ctx.at.z, Sp_Visit, &ctx);
}

} // extern "C"

// === the AI debug overlay: numbers, bars, the aim cross and the road trails (FUN_4d2b30)

struct DbPos { int road, node; double t; int segment, object, m_24, m_28; };
extern "C" int __cdecl Db_RoadParams(DbPos pos, BYTE* info); KING_ALIAS("_Db_RoadParams", "_Road_GetRoadParameters")
extern "C" float* __cdecl Db_Coords(float* out, DbPos pos); KING_ALIAS("_Db_Coords", "_Ai_GetCoordinatesPos")
extern "C" float* __cdecl Db_Tangent(float* out, DbPos pos); KING_ALIAS("_Db_Tangent", "_Ai_GetTangent")
extern "C" void Db_Text(); KING_ALIAS("_Db_Text", "_Text_Draw")
extern "C" void Db_Line(); KING_ALIAS("_Db_Line", "_Window_DrawLine")
extern "C" void Db_CamApply(); KING_ALIAS("_Db_CamApply", "@CamNode_Apply@12")
extern "C" void Db_RoadFirst(); KING_ALIAS("_Db_RoadFirst", "_Road_FirstId")
extern "C" void Db_RoadNext(); KING_ALIAS("_Db_RoadNext", "_Road_NextId")
extern "C" void Db_Get800(); KING_ALIAS("_Db_Get800", "@SceneObj_Get800@8")
KING_VAR(int, g_db696334, 696334)
KING_VAR(double, g_db6962d0, 6962d0)
KING_VAR(double, g_db6962d8, 6962d8)
KING_VAR(double, g_db696300, 696300)
KING_VAR(double, g_db696308, 696308)
KING_VAR(int, g_db696310, 696310)
KING_VAR(Vec3, g_db696314, 696314)
KING_VAR(double, g_db6962e0, 6962e0)
KING_VAR(double, g_db6962e8, 6962e8)
KING_VAR(double, g_db6962f0, 6962f0)
KING_VAR(double, g_db6962f8, 6962f8)
KING_VAR(BYTE*, g_db6962bc, 6962bc)
KING_VAR(BYTE, g_dbTrail, 696278)
KING_VAR(BYTE*, g_db6ced24, 6ced24)
KING_VAR(BYTE*, g_db696ca0, 696ca0)
KING_VAR(BYTE**, g_db68cf38, 68cf38)
KING_VAR(BYTE*, g_db6d2078, 6d2078)
KING_VAR(double, g_db64b678, 64b678)
KING_VAR(double, g_db64b6a0, 64b6a0)
KING_VAR(double, g_db64b6e0, 64b6e0)
KING_VAR(double, g_db64b780, 64b780)
KING_VAR(float, g_db64c250, 64c250)

// a horizontal bar of value v on row y from the middle x0 (5 when positive, else 7)
static void Db_Bar(BYTE* surf, int x0, int y, double base, double v)
{
    int c = v > g_db64b678 ? 5 : 7;
    CallC(Db_Line, surf, x0, y, (int)(base - v * g_db64b6a0), y, c, 0);
}

// a trail point at pos with the road's tangent (reversed for one-way roads backwards) and its widths
static void Db_TrailPoint(const DbPos& pos, const BYTE* info, int mode, int color)
{
    float a[3], t[3], nb[3];
    Vec3 A, N;
    Db_Coords(a, pos);
    const float* n = (const float*)Vec3_Normalized((Vec3*)nb, (const Vec3*)Db_Tangent(t, pos));
    memcpy(&A, a, 12);
    memcpy(&N, n, 12);
    if (mode == 1)
    {
        Vec3 r;
        CallThis(Bw_Set, &r, -N.x, -N.y, -N.z);
        N = r;
    }
    double w1 = F(double, info, 8) * g_db64b6e0;
    double w2 = F(double, info, 24) * g_db64b780 + w1;
    Trail_Add(&g_dbTrail, 0, A, N, w1, w2, color);
}

extern "C" {

// [FUN_4d2b30] with the AI debug view on (696334): three numbers and five bars of the AI's state at the top of
// surface surf, a cross where the AI's target (696314) falls in the camera's view, and trails along the roads of the
// player's room (one-way roads one way: 1 / 3)
void __cdecl Ai_DebugOverlay(WORD* surf)
{
    if (!g_db696334)
        return;
    double dt = g_db6962d0 - g_db6962d8;
    int x0 = surf[0] >> 1;
    char buf[100];
    sprintf(buf, "%d %d %d", (int)g_db696300, (int)g_db696308, g_db696310);
    CallC(Db_Text, surf, x0, 90, g_db6ced24, buf, 5, 0);
    double base = x0;
    CallC(Db_Line, surf, x0, 100, (int)(dt * g_db64b6a0 + base), 100, dt > g_db64b678 ? 5 : 7, 0);
    Db_Bar((BYTE*)surf, x0, 120, base, g_db6962e0);
    Db_Bar((BYTE*)surf, x0, 130, base, g_db6962e8);
    Db_Bar((BYTE*)surf, x0, 140, base, g_db6962f0);
    Db_Bar((BYTE*)surf, x0, 150, base, g_db6962f8);
    BYTE* r = g_db696ca0;
    BYTE* cam = F(BYTE*, r, 464);
    WORD* s2 = F(WORD*, r, 12);
    CallThis(Db_CamApply, F(BYTE*, r, 488), cam);
    Vec3 P;
    CallThis(Bw_Set, &P, 0.0f, 0.0f, 0.0f);
    Vec3 t;
    P = *Vec3_MulMatrix(&t, &g_db696314, (const Matrix*)(cam + 2240 + F(int, cam, 8224) * 48));
    int h2 = s2[1] >> 1;
    if (P.y > g_db64c250)
    {
        int x = (int)(((double)P.x / P.y + g_db64c250) * (s2[0] >> 1));
        int y = (int)(h2 - (double)P.z / P.y * h2);
        CallC(Db_Line, s2, x - 5, y, x + 5, y, 6, 0);
        CallC(Db_Line, s2, x, y - 5, x, y + 5, 6, 0);
    }
    r = g_db696ca0;
    g_db6962bc = F(BYTE*, r, 464);
    CallThis(Db_CamApply, F(BYTE*, r, 488), F(BYTE*, r, 464));
    int id;
    for (CallC(Db_RoadFirst, &id); id != -1; id = *CallC<int*>(Db_RoadNext, &id, id))
    {
        for (int k = 0; k < (F(BYTE*, g_db68cf38[id], 124) - F(BYTE*, g_db68cf38[id], 120)) >> 2; k++)
        {
            DbPos pos = {id, -1, 0.0, 0, k, 0, -1};
            BYTE info[48];
            memset(info, 0, sizeof info);
            F(const char*, info, 16) = "UNDEFINED_MODULE";
            F(const char*, info, 20) = "UNDEFINED_ROOM";
            F(int, info, 36) = -1;
            F(int, info, 40) = -1;
            Db_RoadParams(pos, info);
            BYTE* pl = CallThis<BYTE*>(Db_Get800, F(BYTE*, g_db6d2078, 616) + 16);
            if (strcmp(F(const char*, info, 20), F(const char*, pl, 12)))
                continue;
            *(int*)&g_dbTrail = 1;
            int fl = info[0];
            int mode = fl & 8 ? 1 : (((~fl & 0xFF) & 4) | 8) >> 2;
            int color = mode == 3 ? 3 : 1;
            for (;;)
            {
                Db_TrailPoint(pos, info, mode, color);
                BYTE* e = F(BYTE**, g_db68cf38[id], 120)[k];
                if (CallVirt<int>(e, 20, &pos.t, 619849680, 1068079849, &pos.segment))
                    break;
            }
            Db_TrailPoint(pos, info, mode, color);
        }
    }
}

} // extern "C"

// === the cab view set up for a car (FUN_4dde70 with its tail $L_4de0e7)

extern "C" void Cv_Find(); KING_ALIAS("_Cv_Find", "_Scene_FindObject")
extern "C" void Cv_Node(); KING_ALIAS("_Cv_Node", "@Node_Construct@16")
extern "C" void Cv_Local(); KING_ALIAS("_Cv_Local", "_Frame_GetLocalChecked")
extern "C" void Cv_Fatal(); KING_ALIAS("_Cv_Fatal", "_World_Fatal")
extern "C" void Cv_SetState(); KING_ALIAS("_Cv_SetState", "@Cab_SetState@12")
extern "C" void Cv_Fill(); KING_ALIAS("_Cv_Fill", "@Arrays_Fill@16")
extern "C" void Cv_Time(); KING_ALIAS("_Cv_Time", "_Game_Time")
KING_VAR(int, g_cv6cec68, 6cec68)
KING_VAR(char, g_cvNames, 69817a)
KING_VAR(char, g_cvNames2, 698aea)
KING_VAR(int, g_cv6ce338, 6ce338)
KING_VAR(AsmLabel, g_cv64c7dc, 64c7dc)
KING_VAR(BYTE*, g_cv696cac, 696cac)
KING_VAR(BYTE, g_cvSpecs, 697f9c)
KING_VAR(BYTE, g_cvSpecs2, 69809c)
KING_VAR(BYTE*, g_cv696cb4, 696cb4)
KING_VAR(BYTE*, g_cv69684c, 69684c)
KING_VAR(int, g_cv672d7c, 672d7c)

static BYTE* Cv_FindFmt(char* buf, const char* fmt, const char* name)
{
    sprintf(buf, fmt, name);
    return CallC<BYTE*>(Cv_Find, 0, buf);
}

static void Cv_FillNow(BYTE* a)
{
    float t = (float)CallC<double>(Cv_Time);
    CallThis(Cv_Fill, a, t, 0);
}

extern "C" {

// [FUN_4dde70..$L_4de0e7] cab view s for car c (with 6cec68 set): the type's cab objects "<type>Cab" (else the
// default type's), its node, "<type>CabSpace" and "<type>RuleSpace" (its local frame kept per type), a fatal error
// when one is missing; then the view's node in 696cac, the tilt angle and the seven view values of the type, the
// state from the car's +20396, three ramps restarted, DamageFCKey and the view rectangle
void __fastcall CabView_Setup(BYTE* s, void*, BYTE* c)
{
    if (!g_cv6cec68)
    {
        return;
    }
    int type = F(int, c, 10420);
    F(int, s, 4720) = type;
    char name[100], cab[100], space[100], rule[100], msg[256];
    strcpy(name, &g_cvNames + type * 2416);
    F(BYTE*, s, 544) = Cv_FindFmt(cab, "%sCab", name);
    if (!F(BYTE*, s, 544))
    {
        strcpy(name, &g_cvNames2 + g_cv6ce338 * 2416);
        F(BYTE*, s, 544) = Cv_FindFmt(cab, "%sCab", name);
    }
    BYTE* n = (BYTE*)operator new(88);
    if (n)
    {
        BYTE* o = F(BYTE*, s, 544);
        CallThis(Cv_Node, n, "callCabNode", 17);
        memset(n + 48, 0, 16);
        F(void*, n, 0) = &g_cv64c7dc;
        F(BYTE*, n, 64) = o;
        memset(n + 68, 0, 16);
        F(int, n, 84) = -1;
        F(int, n, 36) = F(int, o, 36);
    }
    F(BYTE*, s, 548) = n;
    F(BYTE*, s, 552) = Cv_FindFmt(space, "%sCabSpace", name);
    F(BYTE*, s, 556) = Cv_FindFmt(rule, "%sRuleSpace", name);
    if (F(BYTE*, s, 556) && !F(int, s, 4400 + 4 * F(int, s, 4720)))
    {
        CallC(Cv_Local, F(BYTE*, s, 556), s + 560 + F(int, s, 4720) * 48);
        F(int, s, 4400 + 4 * F(int, s, 4720)) = 1;
    }
    if (!F(BYTE*, s, 544) || !F(BYTE*, s, 552) || !F(BYTE*, s, 556))
    {
        sprintf(msg, " %s==%08X \n %s==%08X \n %s==%08X", cab, F(int, s, 544), space, F(int, s, 552), rule,
                F(int, s, 556));
        CallC(Cv_Fatal, msg);
    }
    F(BYTE*, g_cv696cac, 76) = F(BYTE*, s, 548);
    const BYTE* sp = &g_cvSpecs + F(int, s, 4720) * 2416;
    F(float, s, 5080) = (float)-x87_atan2((double)F(float, sp, 236) / F(float, sp, 240), 1.0);
    for (int k = 0; k < 7; k++)
        F(int, s, 5124 + 4 * k) = F(int, &g_cvSpecs2 + F(int, s, 4720) * 2416, 16 + 4 * k);
    F(int, s, 4824) = -1;
    CallThis(Cv_SetState, s, F(int, c, 20396) + 1);
    F(DWORD, s, 16) |= 3;
    Cv_FillNow(s + 4724);
    Cv_FillNow(s + 4756);
    Cv_FillNow(s + 4788);
    g_cv696cb4 = CallC<BYTE*>(Cv_Find, 0, "DamageFCKey");
    int w = CallVirt<int>(g_cv69684c, 20);
    int h = CallVirt<int>(g_cv69684c, 24);
    CallVirt(s, 44, 0, g_cv672d7c, w, h);
    CallThis(Cv_Fill, s + 4828, 0, 0);
}

} // extern "C"

// === a semitrailer's world object made (FUN_571c20 with its tail $L_571cbc)

extern "C" void St_FreeId(); KING_ALIAS("_St_FreeId", "_Objects_FreeId")
extern "C" void St_Create(); KING_ALIAS("_St_Create", "@Resources_Create@16")
extern "C" void St_Get(); KING_ALIAS("_St_Get", "@Resources_Get@12")
extern "C" void St_SetData(); KING_ALIAS("_St_SetData", "@Resources_SetData@24")
extern "C" void St_Fatal(); KING_ALIAS("_St_Fatal", "_Sys_Fatal")
extern "C" void St_Players(); KING_ALIAS("_St_Players", "@Iter_Players@8")
KING_VAR(BYTE, g_st6d24bc, 6d24bc)

struct St_M48 { DWORD d[12]; };

extern "C" {

// [FUN_571c20..$L_571cbc] world w's semitrailer object (a free id from 101000, kind) named name at frame m for
// cluster num (num / 1000 at the +num % 1000 table of 6d24bc); its 300 bytes of data set; the id
int __cdecl Semitrailer_Create(BYTE* w, int kind, const char* name, St_M48 m, int num)
{
    int id = 101000;
    CallC(St_FreeId, &id);
    BYTE* res = w + 704;
    CallThis(St_Create, res, id, kind);
    BYTE* rec = CallThis<BYTE*>(St_Get, res, id);
    BYTE* cl = ((BYTE**)(&g_st6d24bc + 12))[num / 1000 + num % 1000 * 372];
    if (!cl)
        CallC(St_Fatal, "Not found cluster for semitrailer", 0);
    F(int, rec, 16) = F(int, cl, 16);
    void* players;
    CallThis(St_Players, &players);
    BYTE data[300];
    Fx_ObjectInit(data, 0, num, id % 1000, name, (const Matrix*)&m, (int)players, 0, 0, 0, 0);
    CallThis(St_SetData, res, id, kind, 300, data);
    return id;
}

} // extern "C"

// === the snow effect made (FUN_4be8d0 with its tail $L_4be938)

extern "C" void Sw_Object(); KING_ALIAS("_Sw_Object", "@Object_ConstructKind@16")
extern "C" void Sw_Attach(); KING_ALIAS("_Sw_Attach", "@Owner_Attach@12")
extern "C" void Sw_Post(); KING_ALIAS("_Sw_Post", "@Queue_PostEvent2@12")
extern "C" void Sw_Fatal(); KING_ALIAS("_Sw_Fatal", "_Sys_Fatal")
extern "C" void Sw_Particles(); KING_ALIAS("_Sw_Particles", "@Particles_Construct@12")
extern "C" void Sw_Alloc(); KING_ALIAS("_Sw_Alloc", "@Particles_Alloc@12")
extern "C" void Sw_Renderer(); KING_ALIAS("_Sw_Renderer", "@RendererA_Construct@8")
extern "C" void Sw_Create(); KING_ALIAS("_Sw_Create", "@Renderer_Create@24")
extern "C" void Sw_Find(); KING_ALIAS("_Sw_Find", "_Scene_FindObject")
KING_VAR(AsmLabel, g_sw64c428, 64c428)
KING_VAR(DWORD, g_sw64c1ec, 64c1ec)

extern "C" {

// [FUN_4be8d0..$L_4be938] snow s ("CSnow", kind 98) of world w with material m: posted, its particles (+36:
// falling 7 from -7 at 0.05, 30 wide) and renderer (+32, 2000 after the scene's "afterThis")
BYTE* __fastcall Snow_Construct(BYTE* s, void*, BYTE* w, BYTE* m)
{
    CallThis(Sw_Object, s, 98, "CSnow");
    F(void*, s, 0) = &g_sw64c428;
    F(BYTE*, s, 40) = w;
    CallThis(Sw_Attach, w + 2992, s);
    CallThis(Sw_Post, s, 1);
    if (!m)
        CallC(Sw_Fatal, "Snow material not found.", 0);
    F(DWORD, m, 32) |= 0x40002;
    F(float, m, 88) = 1.0f;
    BYTE* p = (BYTE*)operator new(140);
    if (p)
        p = CallThis<BYTE*>(Sw_Particles, p, w);
    F(BYTE*, s, 36) = p;
    CallThis(Sw_Alloc, p, -1);
    p = F(BYTE*, s, 36);
    F(DWORD, p, 12) = g_sw64c1ec;
    F(DWORD, F(BYTE*, s, 36), 24) = 0xC0E00000;
    F(int, F(BYTE*, s, 36), 28) = 0;
    F(DWORD, F(BYTE*, s, 36), 8) = 0x3D4CCCCD;
    F(DWORD, F(BYTE*, s, 36), 60) = 0x41F00000;
    F(DWORD, F(BYTE*, s, 36), 52) = 0x40E00000;
    BYTE* r = (BYTE*)operator new(80);
    if (r)
        r = CallThis<BYTE*>(Sw_Renderer, r);
    F(BYTE*, s, 32) = r;
    BYTE* after = CallC<BYTE*>(Sw_Find, F(BYTE*, w, 524), "afterThis");
    CallThis(Sw_Create, F(BYTE*, s, 32), after, m, 2000, 0);
    F(int, s, 44) = 1;
    return s;
}

} // extern "C"

// === a parked car's destructor, saving its record first (FUN_4d0bc0)

extern "C" void Pd_State(); KING_ALIAS("_Pd_State", "@Car_GetState@12")
extern "C" void Pd_M1(); KING_ALIAS("_Pd_M1", "@SceneObj_Matrix1@12")
extern "C" void Pd_Get800(); KING_ALIAS("_Pd_Get800", "@SceneObj_Get800@8")
extern "C" void Pd_Hit(); KING_ALIAS("_Pd_Hit", "_Query_HitObject")
extern "C" void Pd_Value(); KING_ALIAS("_Pd_Value", "@Car_555500@8")
extern "C" void Pd_Name(); KING_ALIAS("_Pd_Name", "@Fx_Get12@8")
extern "C" void Pd_9544(); KING_ALIAS("_Pd_9544", "@Fx_Get9544@8")
extern "C" void Pd_20396(); KING_ALIAS("_Pd_20396", "@Fx_Get20396@8")
extern "C" void Pd_Type(); KING_ALIAS("_Pd_Type", "@Fx_Get10420@8")
extern "C" void Pd_ResGet(); KING_ALIAS("_Pd_ResGet", "@Resources_Get@12")
extern "C" void Pd_SetData(); KING_ALIAS("_Pd_SetData", "@Resources_SetData@24")
extern "C" void Pd_Destroy(); KING_ALIAS("_Pd_Destroy", "@Truck55ebf0_Destroy@4")
KING_VAR(AsmLabel, g_pd64cbfc, 64cbfc)
KING_VAR(AsmLabel, g_pd64cb10, 64cb10)
KING_VAR(AsmLabel, g_pd64cac4, 64cac4)
KING_VAR(AsmLabel, g_pd64cac0, 64cac0)
KING_VAR(int, g_pd68b990, 68b990)
KING_VAR(BYTE*, g_pd696ca0, 696ca0)
KING_VAR(BYTE, g_pdSpecs, 697868)

extern "C" {

// [FUN_4d0bc0] car s (its item +16) destroyed: with a record (+308) its state saved as a 300-byte object record
// first: without a player at its parking place (the type's frame through +708 and the item's frame, in the object
// hit there), else at its frame in its own object
void __fastcall ParkedCar_Destroy(BYTE* s, void*)
{
    BYTE* it = s + 16;
    F(void*, s, 0) = &g_pd64cbfc;
    F(void*, it, 0) = &g_pd64cb10;
    F(void*, s, 272) = &g_pd64cac4;
    F(void*, s, 392) = &g_pd64cac0;
    if (F(BYTE*, s, 308))
    {
        __declspec(align(8)) BYTE state[196];
        CallThis(Pd_State, s, state);
        __declspec(align(8)) BYTE data[300];
        bool save = true;
        if (F(int, s, 20636) == g_pd68b990)
        {
            Matrix M, a, F_, W, W2;
            memcpy(&M, s + 708, 48);
            const Matrix* w1 = CallThis<const Matrix*>(Pd_M1, it, &W);
            const Matrix* spec = (const Matrix*)(&g_pdSpecs + F(int, s, 10420) * 2416);
            Matrix_Multiply(&F_, Matrix_Multiply(&a, spec, &M), w1);
            const float* wt = CallThis<const float*>(Pd_M1, it, &W2) + 9;
            BYTE* o = CallThis<BYTE*>(Pd_Get800, it);
            const char* name = F(const char*, CallC<BYTE*>(Pd_Hit, o, wt, &F_.m[9]), 12);
            float f = (float)CallThis<double>(Pd_Value, s);
            int pl = F(int, s, 20636);
            int b = CallThis<int>(Pd_9544, s);
            int r = CallThis<int>(Pd_20396, s);
            int k = CallThis<int>(Pd_Type, s) + r * 1000;
            Fx_ObjectInit(data, 0, k, b, name, &F_, pl, state, *(int*)&f, 0, 0);
        }
        else if (CallThis<BYTE*>(Pd_ResGet, g_pd696ca0 + 704, F(int, s, 360)))
        {
            float f = (float)CallThis<double>(Pd_Value, s);
            int pl = F(int, s, 20636);
            Matrix W;
            const Matrix* w = CallThis<const Matrix*>(Pd_M1, it, &W);
            const char* name = CallThis<const char*>(Pd_Name, CallThis<BYTE*>(Pd_Get800, it));
            int b = CallThis<int>(Pd_9544, s);
            int r = CallThis<int>(Pd_20396, s);
            int k = CallThis<int>(Pd_Type, s) + r * 1000;
            Fx_ObjectInit(data, 0, k, b, name, w, pl, state, *(int*)&f, 0, 0);
        }
        else
            save = false;
        if (save)
            CallThis(Pd_SetData, F(BYTE*, s, 52) + 704, F(int, s, 360), F(int, F(BYTE*, s, 308), 60), 300, data);
    }
    CallThis(Pd_Destroy, s);
}

} // extern "C"

// === the gears blocked by the cars ahead (FUN_4d4a90)

struct GmPos { DWORD d[8]; };
extern "C" GmPos* __cdecl Gm_PosId(GmPos* out, int player); KING_ALIAS("_Gm_PosId", "_Ai_GetPositionId")
extern "C" float* __cdecl Gm_Tangent(float* out, GmPos p); KING_ALIAS("_Gm_Tangent", "_Ai_GetTangent")
extern "C" float* __cdecl Gm_Coords(float* out, GmPos p); KING_ALIAS("_Gm_Coords", "_Ai_GetCoordinatesPos")
extern "C" GmPos* __cdecl Gm_OnTraj(GmPos* out, int player, const float* v);
KING_ALIAS("_Gm_OnTraj", "_Ai_FindPositionOnTrajectory")
extern "C" GmPos* __cdecl Gm_OnTraj2(GmPos* out, int player, const float* v, GmPos from, GmPos to);
KING_ALIAS("_Gm_OnTraj2", "_Ai_FindPositionOnATrajectory2")
extern "C" GmPos* __cdecl Gm_Advance(GmPos* out, int player, GmPos pos, double* off, int send);
KING_ALIAS("_Gm_Advance", "_Ai_AdvancePlayerPosition")
extern "C" bool __cdecl Gm_HasItems(int player); KING_ALIAS("_Gm_HasItems", "_Fx_HasItems")
extern "C" void Gm_Length(); KING_ALIAS("_Gm_Length", "_Vec3_Length")
extern "C" void Gm_Norm(); KING_ALIAS("_Gm_Norm", "_Vec3_Normalized")
extern "C" void Gm541970_First(); KING_ALIAS("_Gm541970_First", "@Obj541970_First@8")
extern "C" void Gm_Self(); KING_ALIAS("_Gm_Self", "@Obj541960_Self@8")
KING_VAR(BYTE, g_gmSpecs, 697db8)
KING_VAR(Vec3, g_gmUp, 702630)
KING_VAR(Vec3, g_gmSide, 702620)
KING_VAR(double, g_gm64b840, 64b840)
KING_VAR(double, g_gm64b728, 64b728)
KING_VAR(double, g_gm64d0c0, 64d0c0)
KING_VAR(double, g_gm64b5b8, 64b5b8)
KING_VAR(double, g_gm64b780, 64b780)
KING_VAR(double, g_gm64b6f0, 64b6f0)
KING_VAR(double, g_gm64b788, 64b788)
KING_VAR(double, g_gm64b938, 64b938)
KING_VAR(double, g_gm64b7c0, 64b7c0)
KING_VAR(double, g_gm64b6d0, 64b6d0)
KING_VAR(double, g_gm64b6e0, 64b6e0)
KING_VAR(double, g_gm64b670, 64b670)
KING_VAR(float, g_gm64d2f8, 64d2f8)
KING_VAR(float, g_gm64c264, 64c264)
KING_VAR(float, g_gm64c250, 64c250)

static double Gm_Len(const float* v)
{
    return CallC<double>(Gm_Length, v);
}

// car o's world place: its frame (+20640) applied to its point +21040, x87 style: x not rounded before the
// frame's place is added, y and z rounded first; own picks the asking car's order of the sums
static void Gm_Place(const BYTE* o, float* p, bool own)
{
    const float* M = &F(float, o, 20640);
    const float* L = &F(float, o, 21040);
    double x = (double)M[6] * L[2] + (double)M[3] * L[1] + (double)M[0] * L[0];
    float y, z;
    if (own)
    {
        y = (float)((double)M[1] * L[0] + (double)M[7] * L[2] + (double)M[4] * L[1]);
        z = (float)((double)M[2] * L[0] + (double)M[8] * L[2] + (double)M[5] * L[1]);
    }
    else
    {
        y = (float)((double)M[7] * L[2] + (double)M[4] * L[1] + (double)M[1] * L[0]);
        z = (float)((double)M[8] * L[2] + (double)M[5] * L[1] + (double)M[2] * L[0]);
    }
    p[0] = (float)(x + M[9]);
    p[1] = (float)((double)y + M[10]);
    p[2] = (float)((double)z + M[11]);
}

extern "C" {

// [FUN_4d4a90] AI car c's gears (bit per gear 0 .. 8) that would run it into a car ahead in its lane: for each
// other car near along the road (angle limit 64b840, within 64b728) its place on c's route (from the trajectory,
// or found between 30 back and ahead on c's road), the lateral offset and the closing speed against a margin of
// both widths; the nearest blocking one sets *d (its speed) and *res (its distance); the mask
unsigned __cdecl Car_GearBlockMask(BYTE* c, double a52, int rev, int fol, double v, double k, double* d,
                                   double* res, float a84)
{
    int me = F(int, c, 20636);
    unsigned mask = 0;
    double wc = ((const float*)(&g_gmSpecs + F(int, c, 10420) * 2416))[1];
    float p0[3];
    Gm_Place(c, p0, true);
    GmPos pc;
    Gm_PosId(&pc, me);
    float t0[3], D[3];
    CallC(Gm_Norm, D, Gm_Tangent(t0, pc));
    float vc[3] = {F(float, c, 20700), F(float, c, 20704), F(float, c, 20708)};
    if (rev)
        for (int i = 0; i < 3; i++)
            D[i] = -D[i];
    double best = 1000.0;
    BYTE* it = CallThis<BYTE*>(Gm541970_First, c);
    while (it)
    {
        BYTE* o = CallThis<BYTE*>(Gm_Self, it);
        if (o != c)
            do
            {
                const float* so = (const float*)(&g_gmSpecs + F(int, o, 10420) * 2416);
                double wo = so[1], lo = so[0];
                double wsum = (wo + wc) * g_gm64b6e0;
                float P[3];
                Gm_Place(o, P, false);
                float R[3] = {(float)((double)P[0] - p0[0]), (float)((double)P[1] - p0[1]),
                              (float)((double)P[2] - p0[2])};
                double dist = Gm_Len(R);
                if (x87_fabs(R[2]) / dist > g_gm64b840 || dist > g_gm64b728)
                    break;
                float vo[3] = {F(float, o, 20700), F(float, o, 20704), F(float, o, 20708)};
                float VR[3] = {(float)((double)vo[0] - vc[0]), (float)((double)vo[1] - vc[1]),
                               (float)((double)vo[2] - vc[2])};
                double vr = Gm_Len(VR);
                double dot = ((double)vo[1] - vc[1]) * D[1] + ((double)vo[0] - vc[0]) * D[0];
                dot += ((double)vo[2] - vc[2]) * D[2];
                if (dot < 0.0)
                    vr = -vr;
                GmPos po, tmp;
                if (!Gm_HasItems(me))
                    po = *Gm_OnTraj(&tmp, me, P);
                else
                    for (double off = 30.0;;)
                    {
                        double o1 = -off;
                        GmPos pm, pa, pb;
                        Gm_PosId(&pm, me);
                        Gm_Advance(&pa, me, pm, &o1, 0);
                        o1 = off;
                        Gm_PosId(&pm, me);
                        Gm_Advance(&pb, me, pm, &o1, 0);
                        po = *Gm_OnTraj2(&tmp, me, P, pa, pb);
                        float c1[3], c2[3];
                        const float* co = Gm_Coords(c1, po);
                        const float* cb = Gm_Coords(c2, pb);
                        float dl[3] = {(float)((double)cb[0] - co[0]), (float)((double)cb[1] - co[1]),
                                       (float)((double)cb[2] - co[2])};
                        if (!(Gm_Len(dl) < g_gm64c250))
                        {
                            float c3[3], c4[3];
                            const float* co2 = Gm_Coords(c3, po);
                            const float* ca = Gm_Coords(c4, pa);
                            float d2[3] = {(float)((double)ca[0] - co2[0]), (float)((double)ca[1] - co2[1]),
                                           (float)((double)ca[2] - co2[2])};
                            Gm_Len(d2);
                        }
                        off += g_gm64d0c0;
                        if (!(off < g_gm64b728))
                            break;
                    }
                float tt[3], T[3];
                CallC(Gm_Norm, T, Gm_Tangent(tt, po));
                if ((double)T[0] * D[0] + (double)T[1] * D[1] + (double)T[2] * D[2] < 0.0)
                    for (int i = 0; i < 3; i++)
                        T[i] = -T[i];
                float cq[3];
                const float* q0 = Gm_Coords(cq, po);
                float Q[3] = {(float)((double)P[0] - q0[0]), (float)((double)P[1] - q0[1]),
                              (float)((double)P[2] - q0[2])};
                double proj = (double)Q[1] * T[1] + (double)Q[0] * T[0];
                float tx = (float)(T[0] * proj), ty = (float)(T[1] * proj);
                float N[3] = {(float)((double)Q[0] - tx), (float)((double)Q[1] - ty), (float)-(proj * T[2])};
                double lat = Gm_Len(N);
                double cz = (double)Q[1] * T[0] - (double)T[1] * Q[0];
                double side = cz * g_gmUp.z + (double)T[2] * Q[0] * g_gmUp.y;
                side += -((double)Q[1] * T[2]) * g_gmUp.x;
                if (side > 0.0)
                    lat = -lat;
                double thr = vr * vr * g_gm64b5b8 + wsum + g_gm64b780;
                if (lat > 0.0 && lat < v && fol)
                {
                    double sp = Gm_Len(vo);
                    double rd = (double)R[0] * D[0] + (double)D[2] * R[2] + (double)R[1] * D[1];
                    if (sp > g_gm64b6d0 && dist < best && dist > wsum && rd > 0.0)
                    {
                        best = dist;
                        if (dist < thr)
                            *d = sp - g_gm64b780;
                        else if (thr + g_gm64b6f0 > dist)
                            *d = sp;
                        if (*d < 0.0)
                            *d = 5.0;
                    }
                }
                double m = 0.0;
                if (vr < 0.0)
                    m = vr + vr;
                wsum = -wsum;
                if (m < wsum)
                    m = wsum;
                double rd = (double)R[0] * D[0] + (double)D[2] * R[2] + (double)R[1] * D[1];
                if (rd < m)
                    break;
                if (vr > g_gm64b670 && a52 > g_gm64b788)
                    break;
                if (a84 > g_gm64c264 || (a52 < g_gm64b788 && Gm_Len(vo) > g_gm64d2f8))
                    thr += g_gm64b938;
                if (dist > thr)
                    break;
                const float* Mo = &F(float, o, 20640);
                double a0 = (double)g_gmSide.z * Mo[6] + (double)g_gmSide.y * Mo[3] + (double)g_gmSide.x * Mo[0];
                double a1 = (double)g_gmSide.z * Mo[7] + (double)g_gmSide.y * Mo[4] + (double)g_gmSide.x * Mo[1];
                double a2 = (double)g_gmSide.z * Mo[8] + (double)g_gmSide.y * Mo[5] + (double)g_gmSide.x * Mo[2];
                double cc = x87_fabs(a2 * T[2] + a1 * T[1] + a0 * T[0]);
                double hk = k * g_gm64b6e0;
                double ext = (x87_sqrt(g_gm64b670 - cc * cc) * wo + cc * lo) / hk;
                double latg = lat / hk + g_gm64b7c0;
                double he = ext * g_gm64b6e0;
                int lo_g = (int)(latg - he);
                int hi_g = (int)(he + latg);
                if (hi_g < 0 || lo_g > 8)
                    break;
                if (*res > dist)
                    *res = dist;
                if (hi_g > 8)
                    hi_g = 8;
                if (lo_g < 0)
                    lo_g = 0;
                for (int g = lo_g; g <= hi_g; g++)
                    mask |= 1u << g;
            } while (0);
        BYTE* nx = F(BYTE*, it, 8);
        if (!nx)
            break;
        it = nx - 4;
    }
    return mask;
}

} // extern "C"

// === a lamp's glows drawn ($L_4c5b30, the render callback named in 71e820)

extern "C" void Lg_Norm(); KING_ALIAS("_Lg_Norm", "_Vec3_Normalized")
extern "C" void Lg_Error(); KING_ALIAS("_Lg_Error", "_Grd_ReportError")
extern "C" void Lg_Message(); KING_ALIAS("_Lg_Message", "_Grd_Message")
extern "C" void Lg_Log(); KING_ALIAS("_Lg_Log", "_Sys_Log")
KING_VAR(BYTE*, g_lg6cec6c, 6cec6c)
KING_VAR(BYTE*, g_lg696224, 696224)
KING_VAR(BYTE*, g_lg696244, 696244)
KING_VAR(char, g_lgDate, 6724f8)
KING_VAR(char, g_lgFile, 6724d4)
KING_VAR(char, g_lgWhere, 6724b0)
KING_VAR(char, g_lgBadMod, 672858)
KING_VAR(char, g_lgBad, 67283c)
KING_VAR(char, g_lgTypeMod, 6727f8)
KING_VAR(char, g_lgType, 6727e0)
KING_VAR(char, g_lgProblem, 67281c)
KING_VAR(float, g_lg64c258, 64c258)
KING_VAR(float, g_lg64c350, 64c350)
KING_VAR(float, g_lg64c334, 64c334)
KING_VAR(float, g_lg64c878, 64c878)
KING_VAR(float, g_lg64c888, 64c888)
KING_VAR(float, g_lg64c880, 64c880)
KING_VAR(float, g_lg64c898, 64c898)
KING_VAR(float, g_lg64c894, 64c894)
KING_VAR(float, g_lg64c890, 64c890)
KING_VAR(float, g_lg64c88c, 64c88c)
KING_VAR(float, g_lg64c884, 64c884)
KING_VAR(float, g_lg64c87c, 64c87c)

// a lamp's glow parameters: P0, P1 (radii), P2, P3 (doubled for the core), P4 (the trail's scale), P5 (size),
// the material and Glow_Add2's two extra values
struct LgParams {
    float p[6];
    int mat;
    int g9, g10;
    bool two;
};

static int Lg_Bits(float f)
{
    return *(int*)&f;
}

// the material of params q (cached at +matOff, else found by the name at +nameOff)
static int Lg_Material(BYTE* q, int matOff, int nameOff, BYTE* node, int line)
{
    if (!F(BYTE*, q, matOff))
    {
        F(BYTE*, q, matOff) = Fx_FindByName((const char*)q + nameOff, F(BYTE*, node, 36));
        if (!F(BYTE*, q, matOff))
            CallC(Lg_Error, 0x88760115, &g_lgWhere, &g_lgFile, line, &g_lgDate);
        F(DWORD, F(BYTE*, q, matOff), 32) |= 0x40300;
    }
    return F(int, q, matOff);
}

static int Lg_Glow(const LgParams& g, int a, const float* at, float b, int c, int d, float f)
{
    if (g.two)
        return Glow_Add2(a, (const Vec3Bits*)at, b, g.p[0], g.p[1], c, d, g.mat, g.g9, g.g10, f);
    return Glow_Add(a, (const Vec3Bits*)at, b, g.p[0], g.p[1], c, d, g.mat, f);
}

// the first glow at the lamp
static int Lg_First(const LgParams& g, const float* V)
{
    return Lg_Glow(g, 0, V, g.p[5], Lg_Bits(g.p[2]), Lg_Bits(g.p[3]), g.p[4]) | 1;
}

// the core: twice the sizes, full strength
static int Lg_Core(const LgParams& g, const float* V)
{
    return Lg_Glow(g, 1, V, 0, Lg_Bits((float)((double)g.p[2] + g.p[2])), Lg_Bits((float)((double)g.p[3] + g.p[3])),
                   1.0f);
}

// six more glows along the view direction, three before and three behind the lamp
static int Lg_Trail(const LgParams& g, const float* V)
{
    BYTE* cam = g_lg696244;
    double cx = F(double, cam, 2904);
    float cy = (float)F(double, cam, 2912), cz = (float)F(double, cam, 2920);
    float d[3] = {(float)(V[0] - cx), (float)((double)V[1] - cy), (float)((double)V[2] - cz)};
    float dn[3];
    CallC(Lg_Norm, dn, d);
    static const int n = 6;
    double ks[n] = {g_lg64c258, g_lg64c350, 2.0, g_lg64c258, g_lg64c350, 2.0};
    float bks[n] = {g_lg64c898, g_lg64c894, g_lg64c890, g_lg64c88c, g_lg64c884, g_lg64c87c};
    int r = 0;
    for (int i = 0; i < n; i++)
    {
        double k = ks[i];
        double ox = k == 2.0 ? (double)dn[0] + dn[0] : dn[0] * k;
        float oy = k == 2.0 ? (float)((double)dn[1] + dn[1]) : (float)(dn[1] * k);
        float oz = k == 2.0 ? (float)((double)dn[2] + dn[2]) : (float)(dn[2] * k);
        float q[3];
        if (i < 3)
        {
            q[0] = (float)(ox + V[0]);
            q[1] = (float)((double)oy + V[1]);
            q[2] = (float)((double)oz + V[2]);
        }
        else
        {
            q[0] = (float)(V[0] - ox);
            q[1] = (float)((double)V[1] - oy);
            q[2] = (float)((double)V[2] - oz);
        }
        float f;
        switch (i)
        {
        case 0: f = -g.p[4]; break;
        case 1: f = (float)((double)g.p[4] * g_lg64c258); break;
        case 2: f = (float)((double)g.p[4] * g_lg64c334); break;
        case 3: f = (float)((double)g.p[4] * g_lg64c878); break;
        case 4: f = (float)((double)g.p[4] * g_lg64c888); break;
        default: f = (float)((double)g.p[4] * g_lg64c880); break;
        }
        r |= Lg_Glow(g, 0, q, (float)((double)g.p[5] + bks[i]), Lg_Bits(g.p[2]), Lg_Bits(g.p[3]), f);
    }
    return r;
}

extern "C" {

// [$L_4c5b30] lamp node (its place +20, its glow type +60, its kind +68, its parameters +72) drawn in view (only
// the main view): its place through the view's frame, then by type: 0 / 2 / 4 a lamp with a trail and / or a core
// (kind 9), 1 just the lamp, 3 just the core (kind 7), 5 .. 9 the same with Glow_Add2 (kind 11, 8 kind 9);
// errors for a wrong kind or type; 0
int __cdecl LampGlows_Draw(void*, BYTE* view, BYTE* node)
{
    if (view != F(BYTE*, F(BYTE*, g_lg6cec6c, 1356), 464))
        return 0;
    const float* p = &F(float, node, 20);
    const float* M = (const float*)(view + 5216 + F(int, view, 8236) * 48);
    float V[3];
    double y = (double)p[0] * M[1] + (double)p[2] * M[7] + (double)p[1] * M[4] + M[10];
    V[2] = (float)((double)p[0] * M[2] + (double)p[2] * M[8] + (double)p[1] * M[5] + M[11]);
    V[0] = (float)((double)p[2] * M[6] + (double)p[1] * M[3] + (double)p[0] * M[0] + M[9]);
    V[1] = (float)y;
    int type = F(int, node, 60);
    int kind = F(int, node, 68);
    static const int kinds[10] = {9, 9, 9, 7, 9, 11, 11, 11, 9, 11};
    static const int lines[10] = {4606, 4646, 4678, 4717, 4749, 4783, 4825, 4859, 4900, 4935};
    const char* mod = 0;
    if (g_lg696224 && F(BYTE*, g_lg696224, 468))
        mod = F(const char*, F(BYTE*, g_lg696224, 468), 60);
    char buf[256];
    if (type < 0 || type > 9)
    {
        if (mod)
            sprintf(buf, &g_lgTypeMod, type, mod);
        else
            sprintf(buf, &g_lgType, type);
        CallC(Lg_Message, 4, buf);
        return 0;
    }
    if (kind != kinds[type])
    {
        if (mod)
            sprintf(buf, &g_lgBadMod, type, mod);
        else
            sprintf(buf, &g_lgBad, type);
        CallC(Lg_Message, 4, buf);
        return 0;
    }
    BYTE* q = F(BYTE*, node, 72);
    LgParams g;
    memcpy(g.p, q, 24);
    g.two = type >= 5;
    bool small = type == 3 || type == 8;
    g.mat = small ? Lg_Material(q, 22, 16, node, lines[type]) : Lg_Material(q, 30, 24, node, lines[type]);
    g.g9 = small ? F(int, q, 26) : F(int, q, 34);
    g.g10 = small ? F(int, q, 30) : F(int, q, 38);
    int r = 0;
    switch (type % 5)
    {
    case 0:
        r = Lg_First(g, V);
        r |= Lg_Trail(g, V);
        r |= Lg_Core(g, V);
        break;
    case 1:
        r = Lg_First(g, V);
        break;
    case 2:
        r = Lg_First(g, V);
        r |= Lg_Trail(g, V);
        break;
    case 3:
        r = Lg_Glow(g, 1, V, 0, Lg_Bits(g.p[2]), Lg_Bits(g.p[3]), 1.0f) | 1;
        break;
    default:
        r = Lg_First(g, V);
        r |= Lg_Core(g, V);
        break;
    }
    if (!r && node && F(BYTE*, node, 36) && F(const char*, F(BYTE*, node, 36), 60))
    {
        char msg[256];
        sprintf(msg, &g_lgProblem, F(const char*, F(BYTE*, node, 36), 60));
        CallC(Lg_Log, msg, r);
    }
    return 0;
}

} // extern "C"

// === a police car's step: chasing, warning and shooting at the player ($L_4d1360, a vtable entry)

extern "C" void Cp_Radio(); KING_ALIAS("_Cp_Radio", "_Copter_RadioCommand")
extern "C" void Cp_Fly(); KING_ALIAS("_Cp_Fly", "_Bullet_Fly")
extern "C" void Cp_Near(); KING_ALIAS("_Cp_Near", "_Fx_VehicleNear")
extern "C" void Cp_Order(); KING_ALIAS("_Cp_Order", "@Car_WorkingOrder@4")
extern "C" void Cp_Limit(); KING_ALIAS("_Cp_Limit", "_Ai_QuerySpeedLimit")
extern "C" void Cp_Ambush(); KING_ALIAS("_Cp_Ambush", "_Ai_AmbushAttack")
extern "C" void Cp_State(); KING_ALIAS("_Cp_State", "_Player_GetState")
extern "C" void Cp_Get800(); KING_ALIAS("_Cp_Get800", "@SceneObj_Get800@8")
extern "C" void Cp_Dir(); KING_ALIAS("_Cp_Dir", "@Vec3_TransformDir@12")
extern "C" void Cp_Matrix(); KING_ALIAS("_Cp_Matrix", "@SceneObj_Matrix1@12")
KING_VAR(int, g_cp6f3488, 6f3488)
KING_VAR(int, g_cp6f348c, 6f348c)
KING_VAR(char, s_cp672a30, 672a30)
KING_VAR(double, g_cp64d0f0, 64d0f0)
KING_VAR(double, g_cp64b6a0, 64b6a0)
KING_VAR(double, g_cp64b5a0, 64b5a0)
KING_VAR(double, g_cp64b938, 64b938)
KING_VAR(double, g_cp64d0e8, 64d0e8)
KING_VAR(double, g_cp64b6d0, 64b6d0)
KING_VAR(double, g_cp64bb90, 64bb90)
KING_VAR(double, g_cp64b6f0, 64b6f0)
KING_VAR(double, g_cp64d0e0, 64d0e0)
KING_VAR(double, g_cp64b868, 64b868)
KING_VAR(double, g_cp64d0d8, 64d0d8)
KING_VAR(double, g_cp64d0d0, 64d0d0)
KING_VAR(double, g_cp64b728, 64b728)
KING_VAR(double, g_cp64b658, 64b658)
KING_VAR(double, g_cp64d0c0, 64d0c0)
KING_VAR(double, g_cp64b678, 64b678)
KING_VAR(double, g_cp64d0b8, 64d0b8)
KING_VAR(float, g_cp64c250, 64c250)
KING_VAR(float, g_cp64d0c8, 64d0c8)
KING_VAR(float, g_cp64c264, 64c264)
KING_VAR(float, g_cp64c664, 64c664)

static double Cp_Now()
{
    return CallC<double>(FUN_6037e0);
}

// a shot from s towards the player's place pp, turned by up to 50 random steps of k about z (the time in pc
// +16536)
static void Cp_Shoot(BYTE* s, const float* pp, double k, BYTE* pc)
{
    float M[12];
    const float* m = CallThis<float*>(Cp_Matrix, s, M) + 9;
    float d[3] = {(float)((double)pp[0] - m[0]), (float)((double)pp[1] - m[1]), (float)((double)pp[2] - m[2])};
    Vec3 dir;
    Vec3_Normalized(&dir, (const Vec3*)d);
    m = CallThis<float*>(Cp_Matrix, s, M) + 9;
    float from[3] = {m[0], m[1], m[2]};
    int r = rand() % 100 - 50;
    Matrix R;
    Matrix_RotationZ(&R, (float)(r * k));
    CallThis(Cp_Dir, &dir, &R);
    F(double, pc, 16536) = Cp_Now();
    double vx = (double)dir.x * g_cp64d0c8;
    float vy = (float)((double)dir.y * g_cp64d0c8);
    float vz = (float)((double)dir.z * g_cp64d0c8);
    float to[3] = {(float)(from[0] + vx), (float)((double)vy + from[1]), (float)((double)vz + from[2])};
    CallC(Cp_Fly, CallThis<void*>(Cp_Get800, s), from, to, s, 0x3DCCCCCDu);
}

// the player's place pp in s's own frame when s's player is set to ambush: an attack when it is ahead (y)
// and near; b picks the asm's two orders of the sums
static void Cp_Ambush(BYTE* s, const float* pp, bool b)
{
    if (!(CallC<int>(FUN_43f620, F(int, s, 20620)) & 0x80000))
        return;
    float M[12];
    Matrix Mi;
    const float* m = Matrix_Inverse(&Mi, CallThis<Matrix*>(Cp_Matrix, s, M))->m;
    float l[3];
    if (!b)
    {
        l[0] = (float)((double)pp[1] * m[3] + (double)pp[2] * m[6] + (double)pp[0] * m[0] + m[9]);
        l[1] = (float)((double)pp[1] * m[4] + (double)pp[0] * m[1] + (double)pp[2] * m[7] + m[10]);
        l[2] = (float)((double)pp[1] * m[5] + (double)pp[0] * m[2] + (double)pp[2] * m[8] + m[11]);
    }
    else
    {
        l[0] = (float)((double)pp[2] * m[6] + (double)pp[1] * m[3] + (double)pp[0] * m[0] + m[9]);
        l[1] = (float)((double)pp[2] * m[7] + (double)pp[1] * m[4] + (double)pp[0] * m[1] + m[10]);
        l[2] = (float)((double)pp[2] * m[8] + (double)pp[1] * m[5] + (double)pp[0] * m[2] + m[11]);
    }
    if (l[1] > g_cp64c264 && Vec3_Length((const Vec3*)l) < g_cp64c664)
        CallC(Cp_Ambush, F(int, s, 20620));
}

// whether c should keep its lane (+16500): the road's tangent at s's player against the player's offset rel
// (both in the player's frame Pm)
static void Cp_Lane(BYTE* s, BYTE* c, const float* Pm, const float* rel, double dist)
{
    Matrix Pi;
    const float* m = Matrix_Inverse(&Pi, (const Matrix*)Pm)->m;
    LrPos pos;
    Ts_PositionId(&pos, F(int, s, 20620));
    float t[3];
    const float* g = Lr_Tangent(t, pos);
    double a = (double)m[6] * g[2] + (double)m[3] * g[1] + (double)g[0] * m[0];
    double b = (double)m[7] * g[2] + (double)m[1] * g[0] + (double)m[4] * g[1];
    double cz = (double)m[8] * g[2] + (double)m[2] * g[0] + (double)m[5] * g[1];
    double dot = b * rel[1] + (cz * rel[2] + a * rel[0]);
    bool ahead = dot > g_cp64b678;
    if (rel[1] > g_cp64c264 && !(dist > g_cp64d0c0))
        F(int, c, 16500) = !ahead;
    else
        F(int, c, 16500) = ahead;
}

extern "C" {

// [$L_4d1360] police car s (its body s - 16, its car record +21584, its kind +21864: 5 chasing, 6 stopping)
// against the player (game +616, its record +21600, its warning count +20944): the player's offset in its
// own frame and distance; broken down (s's working order) it stops; chasing a speeder within range it
// raises the fine (6f3488 / 6f348c), radios warnings counted by tens and shoots; stopping it checks the
// player stopped and stands, or radios and shoots; its lane (+16500) and brake (+16488 bits 1, 2) set, then
// its body stepped between log marks and its hits taken
int __fastcall Cop_Step(BYTE* s)
{
    BYTE* body = s - 16;
    BYTE* c = F(BYTE*, s, 21584);
    int fr = 0;
    if (!CallC<int>(FUN_4cf620, body, &fr))
        return 0;
    BYTE* P = F(BYTE*, g_game, 616);
    BYTE* pc = F(BYTE*, P, 21600);
    int* cnt = (int*)(P + 20944);
    int kind = F(int, s, 21864);
    F(int, pc, 16504) = (kind == 5 || kind == 6) && cnt ? 0 : 1;
    float Pm[12], Sm[12];
    CallThis(Cp_Matrix, P + 16, Pm);
    const float* p = CallThis<float*>(Cp_Matrix, s, Sm) + 9;
    Matrix Mi;
    const float* m = Matrix_Inverse(&Mi, (const Matrix*)Pm)->m;
    float rel[3];
    rel[1] = (float)((double)m[7] * p[2] + (double)m[4] * p[1] + (double)m[1] * p[0] + m[10]);
    rel[2] = (float)((double)m[8] * p[2] + (double)m[5] * p[1] + (double)m[2] * p[0] + m[11]);
    rel[0] = (float)((double)m[6] * p[2] + (double)m[3] * p[1] + (double)p[0] * m[0] + m[9]);
    double dist = Vec3_Length((const Vec3*)rel);
    const float* pp = Pm + 9;
    F(DWORD, c, 16488) &= ~2u;
    DWORD fl;
    if (!(CallThis<double>(Cp_Order, body) > g_cp64d0f0))
    {
        F(int, s, 21908) = 0;
        F(int, c, 16500) = 0;
        fl = F(DWORD, c, 16488) | 1;
    }
    else
    {
        if (kind == 5 && (CallC<int>(FUN_43f620, F(int, s, 20620)) & 0x80000))
        {
            double lim = CallC<double>(Cp_Limit, F(int, s, 20620));
            if (dist < g_cp64b6a0)
                F(float, P, 20952) = (float)(dist * g_cp64b5a0);
            BYTE st[19 * 16];
            for (int i = 0; i < 19; i++)
            {
                F(int, st, i * 16 + 8) = 0;
                F(int, st, i * 16) = 0;
                F(int, st, i * 16 + 12) = 0x3FF00000;
            }
            int* id;
            Ts_CarId(&id);
            CallC(Cp_State, id, st);
            if (!F(int, st, 256) && dist < g_cp64b938 && lim * g_cp64d0e8 < F(float, pc, 13100) && *cnt < 30)
            {
                double t = lim - g_cp64b6d0;
                float v = F(float, pc, 13100);
                int lvl = 1;
                if ((t + g_cp64bb90) * g_cp64d0e8 < v)
                    lvl = 3;
                else if ((t + g_cp64b6f0) * g_cp64d0e8 < v)
                    lvl = 2;
                g_cp6f3488 = (int)t;
                g_cp6f348c = (int)(F(float, F(BYTE*, P, 21600), 13100) * g_cp64d0e0);
                int n = *cnt;
                if (!n || n >= 20)
                    *cnt = 1;
                else if (n > 10)
                    *cnt = n - 10;
                double fine = lvl * g_cp64b868;
                int* car;
                F(double, (BYTE*)*Ts_CarId(&car), 240) = fine;
            }
        }
        if (kind == 5 && *cnt > 0 && *cnt < 20 && dist < g_cp64b6a0)
        {
            F(double, s, 21912) += F(double, F(BYTE*, s, 36), 2960);
            F(int, s, 21908) = 1;
            if (*cnt % 10 == 1)
            {
                CallC(Cp_Radio, 0);
                F(double, pc, 16536) = Cp_Now();
                ++*cnt;
            }
            if (*cnt % 10 == 2)
            {
                double t = F(double, pc, 16536) + g_cp64d0d8;
                if (Cp_Now() > t && F(float, F(BYTE*, P, 21600), 13100) > g_cp64c250)
                {
                    F(double, pc, 16536) = Cp_Now();
                    if (*cnt < 10)
                    {
                        CallC(Cp_Radio, 1);
                        ++*cnt;
                    }
                    else
                        CallC(Cp_Radio, 0);
                }
            }
            if (*cnt % 10 == 3)
            {
                double t = F(double, pc, 16536) + g_cp64d0d0;
                if (Cp_Now() > t && F(float, F(BYTE*, P, 21600), 13100) > g_cp64c250)
                {
                    ++*cnt;
                    F(double, pc, 16536) = Cp_Now();
                }
            }
            if (*cnt % 10 == 4 && F(float, F(BYTE*, P, 21600), 13100) > g_cp64c250)
            {
                double t = F(double, pc, 16536) + g_cp64b728;
                if (Cp_Now() > t)
                    Cp_Shoot(s, pp, g_cp64b658, pc);
            }
            if (dist < g_cp64bb90 && F(float, c, 13100) < g_cp64c250 &&
                F(float, F(BYTE*, P, 21600), 13100) < g_cp64c250)
            {
                CallC(Cp_Radio, 2);
                *cnt = 0;
            }
            Cp_Ambush(s, pp, false);
            Cp_Lane(s, c, Pm, rel, dist);
        }
        else if (kind == 6 && dist < g_cp64b6a0 && *cnt >= 20 && *cnt < 30 &&
                 !CallC<int>(Cp_Near, *(Vec3*)p))
        {
            int mode = 0;
            int* car;
            if (F(double, (BYTE*)*Ts_CarId(&car) + 8, 216) > g_cp64b6a0)
                mode = 1;
            BYTE* o = (BYTE*)*Ts_CarId(&car) + 200;
            if (F(double, o, 24) > g_cp64b678 || F(BYTE, o, 1))
                mode = 2;
            if (mode == 1)
            {
                double t = F(double, pc, 16536) + g_cp64b728;
                if (Cp_Now() > t)
                    Cp_Shoot(s, pp, g_cp64d0b8, pc);
            }
            else if (mode == 2)
            {
                if (*cnt % 10 == 1)
                {
                    CallC(Cp_Radio, 4);
                    F(double, pc, 16536) = Cp_Now();
                    ++*cnt;
                }
                if (*cnt % 10 == 2)
                {
                    double t = F(double, pc, 16536) + g_cp64d0d0;
                    if (Cp_Now() > t && F(float, F(BYTE*, P, 21600), 13100) > g_cp64c250)
                    {
                        F(double, pc, 16536) = Cp_Now();
                        ++*cnt;
                    }
                }
                if (*cnt % 10 == 3 && F(float, F(BYTE*, P, 21600), 13100) > g_cp64c250)
                {
                    double t = F(double, pc, 16536) + g_cp64b728;
                    if (Cp_Now() > t)
                        Cp_Shoot(s, pp, g_cp64d0b8, pc);
                }
                if (dist < g_cp64bb90 && F(float, c, 13100) < g_cp64c250 &&
                    F(float, F(BYTE*, P, 21600), 13100) < g_cp64c250)
                {
                    CallC(Cp_Radio, 3);
                    *cnt = 0;
                }
            }
            Cp_Ambush(s, pp, true);
            if (!mode)
                goto release;
            Cp_Lane(s, c, Pm, rel, dist);
        }
        else
        {
            F(int, c, 16500) = 0;
            F(int, s, 21908) = 0;
            goto release;
        }
        if (dist < g_cp64bb90 && (F(float, F(BYTE*, P, 21600), 13100) < g_cp64c250 ||
                                  (rel[1] > g_cp64c264 && dist > g_cp64b6f0)))
            fl = (F(DWORD, c, 16488) | 2) & ~1u;
        else
        release:
            fl = F(DWORD, c, 16488) & ~1u;
    }
    F(DWORD, c, 16488) = fl;
    Eff_Log(&s_cp672a30);
    CallThis(FUN_556a70, s);
    Eff_Log((const char*)&s_eff6729e0);
    CallC(FUN_4cfe70, body, &fr);
    return 0;
}

} // extern "C"

// ---------------------------------------------------------------------------
// Batch 32: the world side of the AI's player events

extern "C" void Ev32_NodeByName(); KING_ALIAS("_Ev32_NodeByName", "_Node_ByName")
extern "C" void Ev32_PlayerData(); KING_ALIAS("_Ev32_PlayerData", "_Ai_GetPlayerData")
extern "C" void Ev32_Flags(); KING_ALIAS("_Ev32_Flags", "_Player_Flags")
extern "C" void Ev32_SetVehicle(); KING_ALIAS("_Ev32_SetVehicle", "_Player_SetVehicleId")
extern "C" void Ev32_FreeId(); KING_ALIAS("_Ev32_FreeId", "_Objects_FreeId")
extern "C" void Ev32_MatrixAt(); KING_ALIAS("_Ev32_MatrixAt", "_Fx_VehicleMatrixAt")
extern "C" void Ev32_Matrix(); KING_ALIAS("_Ev32_Matrix", "_Fx_VehicleMatrix")
extern "C" void Ev32_ResCreate(); KING_ALIAS("_Ev32_ResCreate", "@Resources_Create@16")
extern "C" void Ev32_ResGet(); KING_ALIAS("_Ev32_ResGet", "@Resources_Get@12")
extern "C" void Ev32_ResSetData(); KING_ALIAS("_Ev32_ResSetData", "@Resources_SetData@24")
extern "C" void Ev32_ResAdd(); KING_ALIAS("_Ev32_ResAdd", "@Resources_AddRecord@20")
extern "C" void Ev32_ResDelete(); KING_ALIAS("_Ev32_ResDelete", "@Resources_Delete@12")
extern "C" void Ev32_ResLoad(); KING_ALIAS("_Ev32_ResLoad", "@Resource_Load@12")
extern "C" void Ev32_SetImage(); KING_ALIAS("_Ev32_SetImage", "_Ai_SetPlayerImage")
extern "C" void Ev32_ObjectInit(); KING_ALIAS("_Ev32_ObjectInit", "@Fx_ObjectInit@44")
extern "C" void Ev32_PlaceField(); KING_ALIAS("_Ev32_PlaceField", "_Player_PlaceField20")
extern "C" void Ev32_SurfCopy(); KING_ALIAS("_Ev32_SurfCopy", "_Surface_Copy16")
extern "C" void Ev32_Fatal(); KING_ALIAS("_Ev32_Fatal", "_Sys_Fatal")
extern "C" void Ev32_SetObject(); KING_ALIAS("_Ev32_SetObject", "_Fx_SetObject")
extern "C" void Ev32_SetObjectM(); KING_ALIAS("_Ev32_SetObjectM", "_Fx_SetObjectMatrix")
extern "C" void Ev32_PosId(); KING_ALIAS("_Ev32_PosId", "_Ai_GetPositionId")
extern "C" void Ev32_Orient(); KING_ALIAS("_Ev32_Orient", "_Ai_GetOrientationF")
extern "C" void Ev32_RoadParams(); KING_ALIAS("_Ev32_RoadParams", "_Road_GetRoadParameters")
extern "C" void Ev32_Route3(); KING_ALIAS("_Ev32_Route3", "FUN_487290")
extern "C" void Ev32_Route2(); KING_ALIAS("_Ev32_Route2", "_Route_Call487230")
extern "C" void Ev32_CarId(); KING_ALIAS("_Ev32_CarId", "_Player_CarId")
extern "C" void Ev32_Movie(); KING_ALIAS("_Ev32_Movie", "_Movie_Play")
extern "C" void Ev32_Notice(); KING_ALIAS("_Ev32_Notice", "_Notice_Show")
extern "C" void Ev32_SetMode(); KING_ALIAS("_Ev32_SetMode", "@Shell_SetMode@12")
extern "C" void Ev32_ObjFind(); KING_ALIAS("_Ev32_ObjFind", "_Objects_Find")
extern "C" void Ev32_SoundRestart(); KING_ALIAS("_Ev32_SoundRestart", "@Sound_Restart@16")

KING_VAR(BYTE, ev_guard696270, 696270)
KING_VAR(int, ev_node696274, 696274)
KING_VAR(int, ev_node6962c8, 6962c8)
KING_VAR(int, ev_node696320, 696320)
KING_VAR(int, ev_route696330, 696330)
KING_VAR(int, ev_route69632c, 69632c)
KING_VAR(int, ev_follow696328, 696328)
KING_VAR(BYTE*, ev_followed6962cc, 6962cc)
KING_VAR(int, ev_next6f340c, 6f340c)
KING_VAR(int, ev_last67b734, 67b734)
KING_VAR(BYTE*, ev_world6cec6c, 6cec6c)
KING_VAR(BYTE*, ev_6cecd8, 6cecd8)
KING_VAR(BYTE*, ev_6cecbc, 6cecbc)
KING_VAR(BYTE*, ev_shell6ceca8, 6ceca8)
KING_VAR(int, ev_6f3480, 6f3480)
KING_VAR(int, ev_6f3484, 6f3484)
KING_VAR(BYTE, ev_trailers6d24bc, 6d24bc)
KING_VAR(BYTE, ev_identity7025f0, 7025f0)
KING_VAR(double, ev_hundred64b6a0, 64b6a0)
KING_VAR(double, ev_64b6e0, 64b6e0)
KING_VAR(double, ev_one64b670, 64b670)
KING_VAR(char, ev_s66d168, 66d168)
KING_VAR(char, ev_s66d148, 66d148)
KING_VAR(char, ev_s66d158, 66d158)
KING_VAR(char, ev_s672b80, 672b80)
KING_VAR(char, ev_s672b70, 672b70)
KING_VAR(char, ev_s672b48, 672b48)
KING_VAR(char, ev_s672b2c, 672b2c)
KING_VAR(char, ev_s672b14, 672b14)
KING_VAR(char, ev_s672b08, 672b08)
KING_VAR(char, ev_s672afc, 672afc)
KING_VAR(char, ev_s672af0, 672af0)
KING_VAR(char, ev_s672ae0, 672ae0)
KING_VAR(char, ev_undefinedModule, 66d638)
KING_VAR(char, ev_undefinedRoom, 66d628)

struct Ev32_Pos { BYTE b[32]; };   // a road position, passed by value

extern "C" {

static void __cdecl Ev32_NoDtor() {}   // the asm's three empty static destructors

// a road room record as Road_GetRoadParameters fills it
static void Ev32_RoomInit(BYTE* r)
{
    F(int, r, 36) = -1;
    F(int, r, 40) = -1;
    F(int, r, 0) = 0;
    F(int, r, 8) = 0;
    F(int, r, 12) = 0;
    F(const char*, r, 16) = &ev_undefinedModule;
    F(const char*, r, 20) = &ev_undefinedRoom;
    F(int, r, 24) = 0;
    F(int, r, 28) = 0;
}

// [$L_4d76b0..$L_4d7d2c] the world's handler of an AI player event (player it, kind, data): 1 makes the
// player's car object (a new world resource of id 100000+, its matrix on the road, the image of the start
// car or the followed player's room), or takes the existing one, and its trailer type; 5 puts a loaded car
// back on the road; 4 sets its matrix; 9/10 run the route of the two (three) named nodes and rotate them;
// 11 the delivery notices; 12 the end of the game; 13 restarts a sound; 7 drops the resource; 15/14/17 the
// engine's idle state; -1 when not handled
int __cdecl Media_AiEvent(BYTE* it, int kind, BYTE* data)
{
    if (!(ev_guard696270 & 1)) {
        ev_guard696270 |= 1;
        ev_node696274 = -1;
        atexit(Ev32_NoDtor);
    }
    if (!(ev_guard696270 & 2)) {
        ev_node6962c8 = -1;
        ev_guard696270 |= 2;
        atexit(Ev32_NoDtor);
    }
    if (!(ev_guard696270 & 4)) {
        ev_node696320 = -1;
        ev_guard696270 |= 4;
        atexit(Ev32_NoDtor);
    }
    int t;
    if (ev_route696330 && ev_node696274 == -1) {
        ev_node696274 = *CallC<int*>(Ev32_NodeByName, &t, (const char*)&ev_s66d168);
        ev_node6962c8 = *CallC<int*>(Ev32_NodeByName, &t, (const char*)&ev_s66d148);
        ev_node696320 = *CallC<int*>(Ev32_NodeByName, &t, (const char*)&ev_s66d158);
    }
    if (ev_route69632c && ev_node696274 == -1) {
        ev_node696274 = *CallC<int*>(Ev32_NodeByName, &t, (const char*)&ev_s672b80);
        ev_node6962c8 = *CallC<int*>(Ev32_NodeByName, &t, (const char*)&ev_s672b70);
    }
    BYTE* world = F(BYTE*, ev_world6cec6c, 1356);
    BYTE* res = world + 704;
    BYTE d[528];
    CallC(Ev32_PlayerData, it, d);
    int type = F(int, d, 8);
    BYTE room[48];
    BYTE mat[48];
    if (kind == 1) {
        if (CallC<int>(Ev32_Flags, it) & 0x100) {
            CallC(Ev32_SetVehicle, it, -1);
            return 0;
        }
        int vid = F(int, it, 84);
        Ev32_RoomInit(room);
        memset(mat, 0, 48);
        memcpy(mat, &ev_identity7025f0, 48);
        int sub = F(int, d, 16), grp = F(int, d, 12);
        int id;
        BYTE* r;
        if (vid < 0) {
            id = ev_next6f340c + 100000;
            ev_next6f340c = (ev_next6f340c + 1) % 500;
            CallC(Ev32_FreeId, &id);
            BYTE m[48];
            memcpy(mat, CallC<void*>(Ev32_MatrixAt, m, it, room), 48);
            CallThis(Ev32_ResCreate, res, id, F(const char*, room, 16));
            r = CallThis<BYTE*>(Ev32_ResGet, res, id);
            if (type == 1) {
                F(int, it, 156) = F(int, ev_6cecd8, 616);
                CallC(Ev32_SetImage, it, F(int, ev_6cecd8, 624) + 1);
            } else if (ev_follow696328 && type == 2) {
                ev_followed6962cc = it;
                memcpy(world + 652, mat, 48);
                strcpy(F(char*, world, 488) + 44, F(const char*, room, 20));
                strcpy(F(char*, world, 488) + 12, F(const char*, room, 16));
            }
            BYTE obj[300];
            CallThis(Ev32_ObjectInit, obj, grp + sub * 1000, id % 1000, F(const char*, room, 20), (const void*)mat,
                     it, 0, 0, 0, 0);
            CallThis(Ev32_ResSetData, res, id, F(int, room, 16), 300, (const void*)obj);
            CallC(Ev32_SetVehicle, it, id);
            if (type != 1) {
                F(BYTE, r, 8) |= 1;
            } else {
                strcpy(F(char*, world, 488) + 44, F(const char*, room, 20));
                strcpy(F(char*, world, 488) + 12, F(const char*, room, 16));
                goto shown;
            }
        } else {
            id = vid;
            r = CallThis<BYTE*>(Ev32_ResGet, res, vid);
            if (!r)
                CallC(Ev32_Fatal, (const char*)&ev_s672b48, 0);
            memcpy(mat, F(BYTE*, r, 24) + 248, 48);
            F(BYTE*, F(BYTE*, r, 24), 296) = it;
            if (type != 1) {
                F(BYTE, r, 8) |= 1;
            } else {
                strcpy(F(char*, world, 488) + 44, (const char*)(F(BYTE*, r, 24) + 16));
                strcpy(F(char*, world, 488) + 12, F(const char*, F(BYTE*, r, 12), 4016));
            shown:
                memcpy(world + 652, mat, 48);
                ev_last67b734 = id;
                int k = CallC<int>(Ev32_PlaceField, it);
                BYTE* sh = ev_6cecbc;
                CallC(Ev32_SurfCopy, F(BYTE*, F(BYTE*, sh, 672), 72), F(BYTE*, F(BYTE*, sh, k * 4 + 368), 72), 6);
            }
        }
        BYTE* tr = ((BYTE**)(&ev_trailers6d24bc + 12))[sub + grp * 372];
        if (tr) {
            F(int, r, 16) = F(int, tr, 16);
            return 0;
        }
        CallC(Ev32_Fatal, (const char*)&ev_s672b2c, 0);
        return 0;
    }
    if (kind == 5) {
        BYTE* r = CallThis<BYTE*>(Ev32_ResGet, res, F(int, d, 36));
        if (!r || type == 1)
            return 0;
        Ev32_RoomInit(room);
        BYTE m[48];
        CallC(Ev32_MatrixAt, m, it, room);
        CallC(Ev32_SetObject, r, F(const char*, room, 20), (const void*)m, (const void*)0);
        if (CallThis<int>(Ev32_ResAdd, res, F(int, room, 16), r, 1) == -1)
            CallC(Ev32_Fatal, (const char*)&ev_s672b14, F(const char*, room, 16));
        if (!ev_follow696328 || it != ev_followed6962cc)
            return 0;
        BYTE pos[32];
        BYTE o[48];
        CallC(Ev32_PosId, pos, it);
        memcpy(world + 652, CallC<void*>(Ev32_Orient, o, *(Ev32_Pos*)pos), 48);
        strcpy(F(char*, world, 488) + 44, F(const char*, room, 20));
        strcpy(F(char*, world, 488) + 12, F(const char*, room, 16));
        return 0;
    }
    if (kind == 4) {
        BYTE* r = CallThis<BYTE*>(Ev32_ResGet, res, F(int, d, 36));
        if (!r || F(int, r, 28))
            return 0;
        BYTE m[48];
        CallC(Ev32_Matrix, m, it);
        CallC(Ev32_SetObjectM, r, (const void*)m, (const void*)0);
        return 0;
    }
    if (kind == 9 || kind == 10) {
        if (ev_route696330) {
            if (kind == 9)
                CallC(Ev32_Route3, it, ev_node696274, ev_node696320);
            else
                CallC(Ev32_Route2, it, ev_node696274);
            int a = ev_node696274, b = ev_node696320, c = ev_node6962c8;
            ev_node6962c8 = a;
            ev_node696274 = b;
            ev_node696320 = c;
            return 0;
        }
        if (!ev_route69632c || type == 1)
            return -1;
        if (kind == 9)
            CallC(Ev32_Route3, it, ev_node696274, ev_node6962c8);
        else
            CallC(Ev32_Route2, it, ev_node696274);
        int a = ev_node696274;
        ev_node696274 = ev_node6962c8;
        ev_node6962c8 = a;
        return 0;
    }
    if (kind == 11) {
        if (!data)
            return -1;
        int car;
        if (it != (BYTE*)*CallC<int*>(Ev32_CarId, &car))
            return -1;
        if (F(BYTE, data, 0) & 1) {
            if (F(int, ev_world6cec6c, 1316))
                return 0;
            CallC(Ev32_Movie, (const char*)&ev_s672b08, 5);
            int n = (int)F(double, data, 16);
            CallC(Ev32_Notice, 5, (const char*)0, F(int, data, 4), F(int, data, 8) + n * 100);
            return 0;
        }
        if (F(int, ev_world6cec6c, 1316))
            return 0;
        int c2;
        int* cp = CallC<int*>(Ev32_CarId, &c2);
        if (CallC<int>(Ev32_Flags, (BYTE*)*cp) & 0x4000)
            return 0;
        ev_6f3480 = (int)F(double, data, 48);
        ev_6f3484 = (int)F(double, data, 56);
        int v = (int)(ev_hundred64b6a0 - F(double, data, 40) * ev_hundred64b6a0);
        CallC(Ev32_Notice, 5, (const char*)0, 0, v);
        return 0;
    }
    if (kind == 12) {
        int car;
        if (it == (BYTE*)*CallC<int*>(Ev32_CarId, &car) && (CallC<int>(Ev32_Flags, it) & 0x10000)) {
            CallC(Ev32_Movie, (const char*)&ev_s672afc, 4);
            return -1;
        }
        if (it == (BYTE*)*CallC<int*>(Ev32_CarId, &car)) {
            CallC(Ev32_Movie, (const char*)&ev_s672af0, 3);
            return -1;
        }
        CallThis(Ev32_SetMode, ev_shell6ceca8, 1);
        return -1;
    }
    if (kind == 13) {
        CallThis(Ev32_SoundRestart, CallC<BYTE*>(Ev32_ObjFind, (const char*)&ev_s672ae0), 1.0f, 1.0f);
        return -1;
    }
    if (kind == 7) {
        int rid = F(int, d, 36);
        BYTE* r = CallThis<BYTE*>(Ev32_ResGet, res, rid);
        if (!r)
            return -1;
        if (F(int, r, 28))
            CallThis(Ev32_ResLoad, r, res);
        CallThis(Ev32_ResAdd, res, 0, r, 0);
        CallThis(Ev32_ResDelete, res, rid);
        return -1;
    }
    if (kind == 15 || kind == 14 || kind == 17) {
        BYTE* r = CallThis<BYTE*>(Ev32_ResGet, res, F(int, d, 36));
        if (!r || !F(BYTE*, r, 28))
            return 0;
        BYTE* e = F(BYTE*, F(BYTE*, r, 28), 21600);
        if (kind == 15) {
            F(int, e, 16508) = 0;
            F(int, e, 16476) = 0;
            F(DWORD, e, 16496) |= 16;
        } else if (kind == 14) {
            Ev32_RoomInit(room);
            BYTE pos[32];
            CallC(Ev32_PosId, pos, it);
            CallC(Ev32_RoadParams, *(Ev32_Pos*)pos, room);
            int v = (int)(F(double, room, 8) * ev_64b6e0 / F(double, e, 16552) - ev_one64b670);
            F(int, e, 16508) = v;
            F(int, e, 16476) = v * 2;
            F(DWORD, e, 16496) |= 16;
        } else {
            int v = rand() % 4;
            F(int, e, 16508) = v;
            F(int, e, 16476) = v * 2;
            F(DWORD, e, 16496) &= ~16u;
        }
        return 0;
    }
    return -1;
}

}

// ---------------------------------------------------------------------------
// Batch 33: a vehicle object's resource

extern "C" void Vr33_Attach(); KING_ALIAS("_Vr33_Attach", "@Obj_AttachResource@16")
extern "C" void Vr33_PlayerData(); KING_ALIAS("_Vr33_PlayerData", "_Ai_GetPlayerData")
KING_VAR(BYTE*, vr33_world6cec6c, 6cec6c)
KING_VAR(BYTE*, vr33_net6cec98, 6cec98)

extern "C" {

// [$L_4d0ef0] a vehicle object attaches resource r (vtable, r, x): in a network race (kind 2) or a network
// game, or for the player's own car (data +8 = 1), the plain attach; otherwise r is told the object (+256 of
// the outer object at -16, or nothing) through its vtable +28
int __fastcall VehicleObj_Attach(BYTE* self, void*, BYTE* r, int x)
{
    if (F(int, vr33_world6cec6c, 1316) && F(int, vr33_net6cec98, 384) == 2)
        return CallThis<int>(Vr33_Attach, self, r, x);
    BYTE d[192];
    CallC(Vr33_PlayerData, F(BYTE*, self, 20620), d);
    if (F(int, vr33_world6cec6c, 1300) > 1 || F(int, d, 8) == 1)
        return CallThis<int>(Vr33_Attach, self, r, x);
    BYTE* outer = self - 16;
    return CallVirt<int>(r, 28, outer ? self + 256 : (BYTE*)0);
}

}
