// force_feedback.cpp - mmt.cpp of the game, rewritten from king.masm: small functions
// of its multimedia helpers: the joystick's force feedback (DirectInput
// effects through COM), object pools and pooled lists, a shadow-stack of
// 6928-byte frames, a triangle index table, and plane heights.
//
// Building with  nmake ASM_FORCE_FEEDBACK=1  makes king.masm assemble the original
// procedures again.

#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" void __fastcall Chain_DeleteAll(BYTE* self, void*);    // movingb.cpp
extern "C" void __cdecl Carat_Message(const char* text, int);
KING_ALIAS("_Carat_Message", "FUN_5d2ba0")

ASM_PROC(FUN_578100)
ASM_PROC(FUN_60aa60)    // assertion failed (level, format, expression, file, line, date)
ASM_PROC(FUN_60aad0)    // error (code, format, file, line, date)
ASM_PROC(FUN_5b1020)
ASM_PROC(FUN_57c5f0)

typedef void (__cdecl* LogFn)(const char* text);
ASM_VAR(int, g_6f4ba4, $L_6f4ba4)
ASM_VAR(int, g_6f4ba8, $L_6f4ba8)
ASM_VAR(LogFn, g_joyLog, $L_6f4bac)
ASM_VAR(void*, g_joyDevice, $L_6f4b88)          // IDirectInputDevice2
ASM_VAR(int, g_joyForceFeedback, $L_6f4b80)
ASM_VAR(void*, g_joyInput, $L_6f4b84)           // IDirectInput
ASM_VAR(int, g_6f4b90, $L_6f4b90)
ASM_VAR(int, g_6f4b94, $L_6f4b94)
ASM_VAR(int, g_6f4b98, $L_6f4b98)
ASM_VAR(int, g_6f4b9c, $L_6f4b9c)
ASM_VAR(int, g_6f4ba0, $L_6f4ba0)
ASM_VAR(int, g_6f4bb0, $L_6f4bb0)
ASM_VAR(int, g_67c4bc, $L_67c4bc)
ASM_VAR(AsmLabel, g_joyEffects, $L_6f3510)      // the list of effects
ASM_VAR(AsmLabel, g_strFfbCommand, $L_67cbac)   // "JOYSTICK joySendFFBCommand ..."
ASM_VAR(int, g_poolLast8, $L_6f4bc8)            // copied into new pooled objects
ASM_VAR(int, g_poolLast12, $L_6f4bcc)
ASM_VAR(int, g_poolLast16, $L_6f4bd0)
ASM_VAR(int, g_poolLast20, $L_6f4bd4)
ASM_VAR(AsmLabel, g_vtbl650334, $L_650334)
ASM_VAR(AsmLabel, g_vtbl65033c, $L_65033c)
ASM_VAR(AsmLabel, g_strDate, $L_6724f8)         // "Jan 16 2003"
ASM_VAR(AsmLabel, g_strMmtFile, $L_67cf90)      // "C:\Nek\VRAPPLVS\TRUCK\mmt.cpp"
ASM_VAR(AsmLabel, g_strPoolNotNull, $L_67cf80)  // "pool != NULL"
ASM_VAR(AsmLabel, g_strThisNotNull, $L_67cfb0)  // "pThis != NULL"
ASM_VAR(AsmLabel, g_strAssert, $L_6726a4)       // "Assertion violated: %s, File = %s, ..."
ASM_VAR(AsmLabel, g_strFileLine, $L_6724b0)     // " File = %s, Line = #%ld, Date = %s "
ASM_VAR(AsmLabel, g_strShadowOverflow, $L_67d034)   // "Shadow stack overflow"
typedef short Shorts[1];
ASM_VAR(Shorts, g_triangles, $L_6f4be0)
ASM_VAR(double, g_epsilon, $L_64c3f8)           // FLT_EPSILON
ASM_VAR(float, g_noHeight, $L_65047c)           // -9999.9

enum { E_NOTREADY = 0x80070015 };

// COM method at vtbl+off of o, stdcall with o first
static void (*Method(void* o, int off))() { return *(void (**)())(*(BYTE**)o + off); }

extern "C" void __cdecl Mmt_Set6f4ba4(int a, int b)
{
    g_6f4ba4 = a;
    g_6f4ba8 = b;
}

extern "C" void __cdecl Joy_Log(const char* text)
{
    if (g_joyLog)
        g_joyLog(text);
}

// ---------------------------------------------------------------------------
// A force feedback effect: +0 started, +4 the effect (IDirectInputEffect),
// +8 the last value sent

extern "C" void __fastcall JoyEffect_Stop(BYTE* self, void*)
{
    void* e = F(void*, self, 4);
    if (e) {
        CallStd(Method(e, 32), e);
        F(int, self, 0) = 0;
    }
}

extern "C" int __fastcall JoyEffect_GetParameters(BYTE* self, void*, DWORD* params, int flags)
{
    params[0] = 56;
    void* e = F(void*, self, 4);
    if (!e)
        return E_NOTREADY;
    return CallStd<int>(Method(e, 20), e, params, flags);
}

extern "C" int __fastcall JoyEffect_GetStatus(BYTE* self, void*, int* status)
{
    *status = 0;
    void* e = F(void*, self, 4);
    if (!e)
        return E_NOTREADY;
    int v;
    int r = CallStd<int>(Method(e, 36), e, &v);
    if (r == 0) {
        *status = v;
        return 0;
    }
    return r;
}

// starts the effect once; sends v * 10000 when it differs from the last
// value by more than part of it
static void SetMagnitude(BYTE* self, int v, double part, DWORD* params, int flags)
{
    int old = F(int, self, 8);
    int diff = v - old;
    if (diff < 0)
        diff = -diff;
    int prev = old < 0 ? -old : old;
    if ((double)prev * part < diff) {
        F(int, self, 8) = v;
        CallStd(Method(F(void*, self, 4), 24), F(void*, self, 4), params, flags);
    }
    if (!F(int, self, 0)) {
        F(int, self, 0) = 1;
        void* e = F(void*, self, 4);
        CallStd(Method(e, 28), e, -1, 0);
    }
}

extern "C" void __fastcall JoyEffect_SetGain(BYTE* self, void*, float v)
{
    if (!F(void*, self, 4))
        return;
    DWORD params[14] = {0};
    params[0] = 56;
    int i = (int)(v * (double)10000.0f);
    params[4] = i;
    SetMagnitude(self, i, 0.05, params, 4);
}

extern "C" void __fastcall JoyEffect_SetMagnitude(BYTE* self, void*, float v)
{
    if (!F(void*, self, 4))
        return;
    DWORD params[14] = {0};
    int magnitude;
    params[0] = 56;
    params[11] = 4;
    params[12] = (DWORD)&magnitude;
    magnitude = (int)(v * (double)10000.0f);
    SetMagnitude(self, magnitude, 0.02, params, 256);
}

// releases the effects of the list and deletes its nodes
extern "C" void __fastcall JoyEffects_Clear(BYTE* self, void*)
{
    for (BYTE* n = F(BYTE*, self, 4); n;) {
        BYTE* next = F(BYTE*, n, 4);
        void* e = F(void*, F(BYTE*, n, 8), 4);
        CallStd(Method(e, 8), e);
        operator delete(F(void*, n, 8));
        n = next;
    }
    Chain_DeleteAll(self, 0);
}

extern "C" int __cdecl Joy_SendCommand(int command)
{
    if (!g_joyDevice || !g_joyForceFeedback)
        return E_NOTREADY;
    char text[100];
    sprintf(text, g_strFfbCommand, command);
    Joy_Log(text);
    return CallStd<int>(Method(g_joyDevice, 88), g_joyDevice, command);
}

extern "C" void __cdecl Joy_ClearEffects()
{
    JoyEffects_Clear((BYTE*)g_joyEffects, 0);
    g_6f4b98 = 0;
    g_6f4b9c = 0;
    g_6f4ba0 = 0;
    g_6f4bb0 = 0;
}

extern "C" int __cdecl Joy_Shutdown()
{
    g_6f4b94 = 0;
    g_6f4b90 = 0;
    Joy_ClearEffects();
    if (g_joyDevice) {
        CallStd(Method(g_joyDevice, 32), g_joyDevice);     // Unacquire
        if (g_joyForceFeedback == 1)
            CallC(FUN_578100, (int)(g_67c4bc == 1));
        CallStd(Method(g_joyDevice, 8), g_joyDevice);      // Release
        g_joyDevice = 0;
    }
    if (g_joyInput) {
        CallStd(Method(g_joyInput, 8), g_joyInput);
        g_joyInput = 0;
    }
    g_joyForceFeedback = 0;
    g_pJoyInfo = 0;
    return 0;
}

// ---------------------------------------------------------------------------
// Pools: +4 the free objects, +8 the used ones, linked through +8 (next)
// and +12 (prev) of the objects; a pooled object keeps its pool at +16

extern "C" BYTE* __fastcall Pooled_Construct(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl650334;
    F(int, self, 4) = g_poolLast8;
    F(int, self, 8) = g_poolLast12;
    F(int, self, 12) = g_poolLast16;
    F(int, self, 16) = g_poolLast20;
    return self;
}

extern "C" void __fastcall Pooled_Set4(BYTE* self, void*)
{
    F(int, self, 4) = 1;
}

// a free object of the pool moved to the used ones, NULL if there is none
extern "C" BYTE* __fastcall Pool_Take(BYTE* self, void*)
{
    BYTE* e = F(BYTE*, self, 4);
    if (!e)
        return 0;
    BYTE* first = F(BYTE*, self, 4);
    BYTE* used = F(BYTE*, self, 8);
    BYTE* prev = F(BYTE*, first, 12);
    BYTE* next = F(BYTE*, e, 8);
    if (used) {
        F(BYTE*, first, 12) = F(BYTE*, used, 12);
        F(BYTE*, F(BYTE*, self, 4), 8) = F(BYTE*, self, 8);
        F(BYTE*, F(BYTE*, self, 8), 12) = F(BYTE*, self, 4);
        F(BYTE*, self, 8) = F(BYTE*, self, 4);
    } else {
        F(BYTE*, self, 8) = first;
        F(BYTE*, first, 8) = 0;
        F(BYTE*, F(BYTE*, self, 4), 12) = 0;
    }
    F(BYTE*, self, 4) = next;
    if (next)
        F(BYTE*, next, 12) = prev;
    return e;
}

static void Assert(const char* expr, int line)
{
    CallC(FUN_60aa60, 2, (const char*)g_strAssert, expr, (const char*)g_strMmtFile, line, (const char*)g_strDate);
}

// moves e back to the free objects
extern "C" void __fastcall Pool_Return(BYTE* self, void*, BYTE* e)
{
    if (!e)
        Assert(g_strThisNotNull, 108);
    if (F(BYTE*, self, 8) == e)
        F(BYTE*, self, 8) = F(BYTE*, e, 8);
    if (F(BYTE*, e, 8))
        F(BYTE*, F(BYTE*, e, 8), 12) = F(BYTE*, e, 12);
    if (F(BYTE*, e, 12))
        F(BYTE*, F(BYTE*, e, 12), 8) = F(BYTE*, e, 8);
    BYTE* free = F(BYTE*, self, 4);
    if (!free) {
        F(BYTE*, self, 4) = e;
        F(BYTE*, e, 8) = 0;
        F(BYTE*, e, 12) = 0;
        return;
    }
    F(BYTE*, e, 12) = F(BYTE*, free, 12);
    F(BYTE*, e, 8) = F(BYTE*, self, 4);
    F(BYTE*, F(BYTE*, self, 4), 12) = e;
    F(BYTE*, self, 4) = e;
}

// takes an object of the pool for the next Pooled_Construct (operator new
// of the pooled classes); the object, NULL if the pool is empty
extern "C" BYTE* __cdecl Pool_Prepare(int size, BYTE* pool)
{
    if (!pool)
        Assert(g_strPoolNotNull, 25);
    BYTE* e = Pool_Take(pool, 0);
    if (!e)
        return 0;
    g_poolLast8 = 0;
    g_poolLast12 = F(int, e, 8);
    g_poolLast16 = F(int, e, 12);
    g_poolLast20 = F(int, e, 16);
    F(BYTE*, e, 16) = pool;
    F(int, e, 4) = 0;
    return e;
}

extern "C" void __cdecl Pooled_Release(BYTE* e)
{
    Pool_Return(F(BYTE*, e, 16), 0, e);
}

extern "C" BYTE* __fastcall Pool_Construct(BYTE* self, void*, int size)
{
    F(int, self, 8) = 0;
    F(int, self, 4) = 0;
    F(void*, self, 0) = g_vtbl65033c;
    F(int, self, 12) = size;
    return self;
}

extern "C" void __fastcall Pool_Destruct(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl65033c;
    for (BYTE* e = F(BYTE*, self, 8); e;) {
        BYTE* next = F(BYTE*, e, 8);
        if (e)
            CallVirt(e, 0, 1);
        e = next;
    }
    F(int, self, 8) = 0;
    F(int, self, 4) = 0;
    F(int, self, 12) = 0;
}

extern "C" BYTE* __fastcall Pool_NextUsed(BYTE* self, void*, BYTE* e)
{
    if (!e)
        return F(BYTE*, self, 8);
    return F(BYTE*, e, 8);
}

// up to 100 entries from +32, their count at +432
extern "C" void __fastcall Mmt_Push100(BYTE* self, void*, int v)
{
    int n = F(int, self, 432);
    if (n == 100) {
        CallC(FUN_60aad0, 0x88762711, (const char*)g_strFileLine, (const char*)g_strMmtFile, 142,
              (const char*)g_strDate);
        return;
    }
    F(int, self, 32 + n * 4) = v;
    F(int, self, 432)++;
}

extern "C" int __fastcall Pool_HasNoFree(BYTE* self, void*)
{
    return F(int, self, 4) == 0;
}

// the shadow stack: 50 frames of 6928 bytes at +0, the depth at +4
extern "C" BYTE* __fastcall ShadowStack_Push(BYTE* self, void*)
{
    unsigned n = F(unsigned, self, 4);
    if (n >= 50) {
        Carat_Message(g_strShadowOverflow, 0);
        return 0;
    }
    F(unsigned, self, 4) = n + 1;
    return F(BYTE*, self, 0) + n * 6928;
}

extern "C" BYTE* __fastcall ShadowStack_Pop(BYTE* self, void*)
{
    if (!F(int, self, 4))
        return 0;
    int n = F(int, self, 4) - 1;
    F(int, self, 4) = n;
    return F(BYTE*, self, 0) + n * 6928;
}

// two triangles per cell of a 6 x 36 vertex grid
extern "C" void __fastcall Triangles_Init(void* self, void*)
{
    for (int row = 1; row < 36; row++) {
        for (int col = 1; col < 6; col++) {
            short* t = g_triangles + (row - 1) * 30 + (col - 1) * 6;
            short v = (short)(row * 6 + col);
            t[0] = v - 7;
            t[1] = v;
            t[2] = v - 6;
            t[3] = v - 7;
            t[4] = v - 1;
            t[5] = v;
        }
    }
}

extern "C" void __fastcall Triangles_Draw(BYTE* self, void*, void* texture, void* device)
{
    if (!F(int, self, 6916) || !F(int, self, 6912) || !F(int, self, 6920))
        return;
    CallThis(FUN_5b1020, texture, F(int, self, 6920), 1, 0, 0);
    CallStd(Method(device, 80), device, 136, 1);
    CallStd(Method(device, 104), device, 4, 274, self, F(int, self, 6916), (short*)g_triangles,
            F(int, self, 6912), 0);
}

extern "C" void __fastcall Vec3_DivideBy(float* self, void*, float d)
{
    self[0] = self[0] / d;
    self[1] = self[1] / d;
    self[2] = self[2] / d;
}

// ---------------------------------------------------------------------------
// Heights: z of the plane a x + b y + c z + d = 0 at (x, y), -9999.9 when
// the plane is (almost) vertical

struct PlaneBits { float a, b, c, d; };     // a Plane

extern "C" double __cdecl Plane_HeightAfter(float x, float y, const PlaneBits* plane, const Matrix* m)
{
    PlaneBits p = *plane;
    Matrix local = *m;
    ((Plane*)&p)->Transform(&local);
    if (!(x87_fabs(p.c) > g_epsilon))
        return g_noHeight;
    return -(((double)p.a * x + (double)p.b * y + p.d) / p.c);
}

extern "C" double __cdecl Plane_Height(float x, float y, float a, float b, float c, float d)
{
    if (!(x87_fabs(c) > g_epsilon))
        return g_noHeight;
    return -(((double)a * x + (double)b * y + d) / c);
}

extern "C" double __cdecl Plane_HeightX(float x, int unused, const float* p)
{
    if (!(x87_fabs(p[2]) > g_epsilon))
        return g_noHeight;
    return -(((double)x * p[0] + p[3] + p[1]) / p[2]);
}

// the height in the first of the n triangles (12 bytes apart from tris)
// that contains (x, y)
extern "C" int __cdecl Tris_Height(float* out, float x, float y, BYTE* tris, BYTE* tris2, const PlaneBits** planes,
                                   const Matrix** matrices, unsigned n)
{
    for (unsigned i = 0; i < n; i++, tris += 12, tris2 += 12) {
        if (CallC<int>(FUN_57c5f0, x, y, tris, tris2)) {
            *out = (float)Plane_HeightAfter(x, y, planes[i], matrices[i]);
            return 1;
        }
    }
    return 0;
}

// the height on the plane of the nearest of the n 208-byte records (point
// at +52, normal at +64)
extern "C" double __cdecl Records_Height(float x, float y, BYTE* recs, unsigned n)
{
    unsigned best = 0;
    float bestDist = 3.40282347e+38f;
    for (unsigned i = 0; i < n; i++) {
        BYTE* r = recs + i * 208;
        double dx = (double)x - F(float, r, 52);
        double dy = (double)y - F(float, r, 56);
        double dist = x87_sqrt(dx * dx + dy * dy);
        if (!(dist >= bestDist)) {
            bestDist = (float)dist;
            best = i;
        }
    }
    BYTE* r = recs + best * 208;
    float d = (float)-(((double)F(float, r, 72) * F(float, r, 60) + (double)F(float, r, 68) * F(float, r, 56))
                       + (double)F(float, r, 64) * F(float, r, 52));
    return Plane_Height(x, y, F(float, r, 64), F(float, r, 68), F(float, r, 72), d);
}

// ---------------------------------------------------------------------------
// a scene object's initial state: gravity along the vector at 702630, three
// zero vectors, the two identity matrices; returns what SceneObj_57e030 does

extern "C" int __fastcall SceneObj_57e030(BYTE* self, void*, int* out);   // engine.cpp
ASM_VAR(float, g_mmt702630, $L_702630)
ASM_VAR(float, g_mmt702634, $L_702634)
ASM_VAR(float, g_mmt702638, $L_702638)
ASM_VAR(DWORD, g_mmt702650, $L_702650)
ASM_VAR(DWORD, g_mmt702654, $L_702654)
ASM_VAR(DWORD, g_mmt702658, $L_702658)
ASM_VAR(float, g_gravity64d998, $L_64d998)      // -9.8
ASM_VAR(AsmLabel, g_mmtIdentity, $L_7025f0)

extern "C" int __fastcall SceneObj_InitState(BYTE* self, void*, DWORD a, DWORD b, DWORD c, DWORD d)
{
    F(float, self, 788) = g_mmt702630 * g_gravity64d998;
    F(float, self, 792) = g_mmt702634 * g_gravity64d998;
    F(float, self, 796) = g_mmt702638 * g_gravity64d998;
    for (int off = 416; off <= 440; off += 12) {
        F(DWORD, self, off) = g_mmt702650;
        F(DWORD, self, off + 4) = g_mmt702654;
        F(DWORD, self, off + 8) = g_mmt702658;
    }
    F(DWORD, self, 396) = a;
    F(DWORD, self, 404) = b;
    F(DWORD, self, 408) = c;
    F(DWORD, self, 412) = d;
    memcpy(self + 692, g_mmtIdentity, 48);
    memcpy(self + 740, g_mmtIdentity, 48);
    return SceneObj_57e030(self, 0, (int*)(self + 3268));
}
