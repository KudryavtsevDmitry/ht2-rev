// aeffects.cpp - aeffects.cpp of the game, rewritten from king.masm: the
// small functions of the effects unit: vector helpers, the constructors and
// destructors of effect and renderer objects, vertex buffers, the pool of
// lights, fade states, the reflection stack, and lookups of vehicles.
//
// Building with  nmake ASM_AEFFECTS=1  makes king.masm assemble the original
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
extern "C" double __fastcall Wave_Value(void* self, void*, double phase);   // engine.cpp
ASM_PROC(FUN_57c450)    // sounds: play
ASM_PROC(FUN_5d3290)    // Task::~Task
ASM_PROC(FUN_5ef660)    // module destructor
ASM_PROC(FUN_4bf510)    // particles: reset
ASM_PROC(FUN_5f19b0)    // named object: constructor
ASM_PROC(FUN_4c09d0)    // CRenderer::~CRenderer
ASM_PROC(FUN_4ccf30)
extern "C" BYTE* __fastcall SceneObj_Matrix1(void* self, void*, void* out);   // engine.cpp: its world matrix
ASM_PROC(FUN_5e5370)    // vehicles: by id
ASM_PROC(FUN_5e49d0)    // vehicle: release
ASM_PROC(FUN_5fd370)    // sound: constructor
extern "C" void __cdecl Fx_Update4910(void* x);
KING_ALIAS("_Fx_Update4910", "FUN_4c4910")
extern "C" void __cdecl Fx_Update4df0(void* x);
KING_ALIAS("_Fx_Update4df0", "FUN_4c4df0")
extern "C" void __cdecl Fx_Message(const char* text, int);
KING_ALIAS("_Fx_Message", "FUN_5d2ba0")
extern "C" int __cdecl Fx_Find4e5bf5(int a, int b, int c, char* buf);
KING_ALIAS("_Fx_Find4e5bf5", "FUN_4e5bf5")
extern "C" void __cdecl Fx_DefaultData(void* data);
KING_ALIAS("_Fx_DefaultData", "FUN_5469e0")
extern "C" int __cdecl Fx_Handle(int a);
KING_ALIAS("_Fx_Handle", "FUN_5ef3d0")

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
