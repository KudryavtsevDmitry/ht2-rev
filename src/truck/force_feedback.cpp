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
KING_ALIAS("_Carat_Message", "_Sys_Log")

ASM_PROC(FUN_578100)
ASM_PROC(FUN_5d31e0)
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_579420(); KING_ALIAS("_FUN_579420", "@Pool_Destruct@8")
ASM_PROC(FUN_5d3290)
extern "C" void FUN_4bd510(); KING_ALIAS("_FUN_4bd510", "@Fx_Self@8")
extern "C" void FUN_5799c0(); KING_ALIAS("_FUN_5799c0", "@Triangles_Init@8")
extern "C" void FUN_5ef780(); KING_ALIAS("_FUN_5ef780", "@Node_Remove@8")
extern "C" void FUN_579850(); KING_ALIAS("_FUN_579850", "@Pool6928_Free@8")
ASM_PROC(FUN_5ef660)
ASM_VAR(AsmLabel, s_67cfc0, $L_67cfc0)
ASM_VAR(AsmLabel, s_67d04c, $L_67d04c)
ASM_VAR(AsmLabel, g_vtbl650340, $L_650340)
ASM_VAR(AsmLabel, g_vtbl650430, $L_650430)
ASM_VAR(AsmLabel, g_vtbl6503e4, $L_6503e4)
extern "C" void FUN_60aa60(); KING_ALIAS("_FUN_60aa60", "_Grd_Message")    // assertion failed (level, format, expression, file, line, date)
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")    // error (code, format, file, line, date)
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

extern "C" BYTE* __fastcall Node_Construct(BYTE* self, void*, const char* name, int kind);   // scene.cpp

extern "C" {

// [FUN_579570] the frame-stack task on the vehicle v
BYTE* __fastcall FrameTask_Construct(BYTE* self, void*, BYTE* v, int kind)
{
    CallThis(FUN_5d31e0, self, kind, (const char*)s_67cfc0);
    F(void*, self, 0) = g_vtbl650340;
    CallThis(FUN_5d3570, v + 2992, self);
    F(int, self, 432) = 0;
    return self;
}

// [FUN_579650] destructor: the frames at +32 (count at +432)
void __fastcall FrameTask_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl650340;
    for (unsigned i = 0; i < F(unsigned, self, 432); i++) {
        BYTE* p = F(BYTE*, self, 32 + i * 4);
        if (p) {
            CallThis(FUN_579420, p);
            operator delete(p);
        }
    }
    F(int, self, 432) = 0;
    CallThis(FUN_5d3290, self);
}

// [FUN_5797d0] a pool of 50 6928-byte objects (count before the array)
DWORD* __fastcall Pool50_Construct(DWORD* self, void*)
{
    BYTE* a = (BYTE*)operator new(346404);
    BYTE* p = 0;
    if (a) {
        F(int, a, 0) = 50;
        p = a + 4;
        for (int i = 0; i < 50; i++)
            CallThis(FUN_4bd510, p + i * 6928);
    }
    self[0] = (DWORD)p;
    self[1] = 0;
    return self;
}

// [FUN_57c290] the pooled-objects node attached to the vehicle v
BYTE* __fastcall PoolNode_Construct(BYTE* self, void*, BYTE* v, int b)
{
    Node_Construct(self, 0, (const char*)s_67d04c, 24);
    F(int, self, 72) = 0;
    F(void*, self, 0) = g_vtbl650430;
    F(int, self, 56) = 0;
    F(int, self, 48) = 0;
    F(int, self, 52) = 0;
    Pool50_Construct((DWORD*)(self + 76), 0);
    F(void*, self, 0) = g_vtbl6503e4;
    F(int, self, 84) = b;
    BYTE* c = F(BYTE*, v, 524);
    if (c)
        CallVirt(c, 20, 0, self);
    CallC(FUN_5799c0);
    return self;
}

// [FUN_57c360]
void __fastcall PoolNode_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl6503e4;
    CallThis(FUN_5ef780, self);
    CallThis(FUN_579850, self + 76);
    if (F(void*, self, 72))
        operator delete(F(void*, self, 72));
    CallThis(FUN_5ef660, self);
}

}

// ---------------------------------------------------------------------------
// x87 functions of the unit: effect parameter updates

template <class... A>
static int ComCall577(void* obj, unsigned off, A... args)
{
    void* fn = *(void**)(*(char**)obj + off);
    return ((int(__stdcall*)(void*, A...))fn)(obj, args...);
}

// IDirectInputEffect: +24 SetParameters, +28 Start, +32 Stop

// [FUN_577530] a periodic effect: magnitude a, offset b, frequency c (updated when the period changes by over 10%)
extern "C" void __fastcall FfEffect_SetPeriodic(DWORD* fx, void*, float a, float b, float c)
{
    void* dev = (void*)fx[1];
    if (!dev)
        return;
    DWORD eff[14] = {};
    DWORD periodic[4] = {};
    periodic[0] = (int)((double)a * 5000.0f);
    periodic[1] = (int)((double)b * 5000.0f);
    unsigned period = (int)(1000000.0f / (double)c);
    periodic[2] = 0;
    periodic[3] = period;
    eff[0] = 56;
    unsigned old = fx[3];
    unsigned big = old > period ? old : period;
    int diff = (int)(old - period);
    if (diff < 0) diff = -diff;
    if ((double)diff > (double)(__int64)big * 0.1) {
        fx[3] = period;
        eff[11] = 16;
        eff[12] = (DWORD)periodic;
        ComCall577(dev, 24, eff, 256);
    }
    if (fx[0] == 0) {
        fx[0] = 1;
        ComCall577((void*)fx[1], 28, -1, 0);
    }
}

// [FUN_5777a0] a condition effect: gain a, first coefficient b (updated when either changes by over 5%)
extern "C" void __fastcall FfEffect_SetCondition(DWORD* fx, void*, float a, float b)
{
    void* dev = (void*)fx[1];
    if (!dev)
        return;
    DWORD eff[14] = {};
    DWORD cond[6];
    memcpy(cond, fx + 4, 24);
    int coef = (int)((double)b * 10000.0f);
    eff[0] = 56;
    cond[0] = coef;
    int gain = (int)((double)a * 10000.0f);
    eff[12] = (DWORD)cond;
    eff[4] = gain;
    eff[11] = 24;
    int d1 = gain - (int)fx[2];
    if (d1 < 0) d1 = -d1;
    bool update = !((double)(int)fx[2] * 0.05 >= (double)d1);
    if (!update) {
        int d2 = coef - (int)fx[3];
        if (d2 < 0) d2 = -d2;
        update = !((double)(int)fx[3] * 0.05 >= (double)d2);
    }
    if (update) {
        fx[2] = gain;
        fx[3] = coef;
        ComCall577(dev, 24, eff, 4);
    }
    if (fx[0] == 0) {
        fx[0] = 1;
        ComCall577((void*)fx[1], 28, -1, 0);
    }
}

ASM_VAR(char, g_fmtFfError, $L_67c9a0)

// [FUN_577e50] restarts a periodic effect with magnitude a and period b (seconds), logging the known errors
extern "C" void __fastcall FfEffect_Restart(DWORD* fx, void*, float a, float b)
{
    void* dev = (void*)fx[1];
    if (!dev)
        return;
    ComCall577(dev, 32);
    DWORD eff[14] = {};
    DWORD env[5] = { 20, 10000, 0, 1000, 500000 };
    DWORD periodic[4];
    eff[0] = 56;
    eff[11] = 16;
    eff[12] = (DWORD)periodic;
    eff[10] = (DWORD)env;
    periodic[3] = (int)((double)b * 1000000.0f);
    periodic[2] = 0;
    periodic[1] = 0;
    periodic[0] = (int)((double)a * 10000.0f);
    unsigned hr = ComCall577((void*)fx[1], 24, eff, 256);
    if (hr == 0x80070015 || hr == 0x80040206 || hr == 0x8007001E || hr == 0x80070057 || hr == 0x80040208) {
        char text[256];
        sprintf(text, &g_fmtFfError, hr, periodic[0], periodic[3]);
        Joy_Log(text);
    }
    ComCall577((void*)fx[1], 28, 1, 0);
}

extern "C" char* __cdecl strerror(int e);
ASM_PROC(FUN_576d70)    // the DirectInput version text
ASM_PROC(FUN_5787a0)    // opens joystick n
ASM_PROC(FUN_6303a0)    // DirectInputCreateA
ASM_FUNC(Joy_EnumCallback, $L_576f90)
typedef char JoyLabel[1];
ASM_VAR(JoyLabel, g_joyEnabled6f34e8, $L_6f34e8)        // 10 ints
ASM_VAR(JoyLabel, g_joyRecords, $L_6f3528)              // 10 records of 568 bytes
ASM_VAR(JoyLabel, g_joy6f4b58, $L_6f4b58)               // 10 ints
ASM_VAR(int, g_joy6f4b80, $L_6f4b80)
ASM_VAR(int, g_joy6f4b8c, $L_6f4b8c)
ASM_VAR(int, g_joyCount, $L_6f4b90)
ASM_VAR(char, g_fmtJoyVersion, $L_67cec4)
ASM_VAR(char, g_strJoy67cf0c, $L_67cf0c)
ASM_VAR(char, g_fmtJoyError, $L_67cefc)                 // "error=%s, %0X"
ASM_VAR(char, g_strJoy677268, $L_677268)
ASM_VAR(char, g_strJoy67cee0, $L_67cee0)

// [FUN_578d80] DirectInput start-up: clears the joystick table, enumerates the attached joysticks and opens the preferred one
extern "C" int __cdecl Joy_Init(HWND wnd, int range)
{
    memset(g_joyEnabled6f34e8, 0, 40);
    memset(g_joy6f4b58, 0, 40);
    for (int k = 0; k < 10; k++) {
        BYTE* r = (BYTE*)g_joyRecords + k * 568;
        F(int, r, 0) = 0;
        F(BYTE, r, 5) = 0;
        F(BYTE, r, 4) = 0;
        F(BYTE, r, 6) = 0;
        for (int o = 8; o <= 28; o += 4) F(int, r, o) = 0;
        F(int, r, 36) = 0;
        F(int, r, 32) = 0;
        F(WORD, r, 44) = 0;
        F(WORD, r, 46) = 0;
        F(WORD, r, 42) = 0;
        F(WORD, r, 40) = 0;
    }
    g_joy6f4b80 = 0;
    g_joy6f4b8c = 0;
    g_joyDevice = 0;
    char ver[100], text[300];
    CallC(FUN_576d70, ver);
    sprintf(text, &g_fmtJoyVersion, ver);
    Joy_Log(text);
    sprintf(text, &g_strJoy67cf0c);
    Joy_Log(text);
    int hr = CallStd<int>(FUN_6303a0, GetWindowLongA(wnd, -6), 0x700, &g_joyInput, 0);
    if (hr < 0) {
        sprintf(text, &g_fmtJoyError, strerror(hr), hr);
        Joy_Log(text);
        return 0x80004005;
    }
    Joy_Log(&g_strJoy677268);
    Joy_Log(&g_strJoy67cee0);
    ComCall577(g_joyInput, 16, 4, Joy_EnumCallback, 0, 1);
    int n = g_joyCount;
    if (n) {
        int pick = -1;
        for (int i = 0; i < n; i++)
            if (F(int, (BYTE*)g_joyRecords + i * 568, 0) & 0x600) {
                pick = i;
                break;
            }
        if (pick < 0)
            for (int i = 0; i < n; i++)
                if (((int*)g_joyEnabled6f34e8)[i] == 1) {
                    pick = i;
                    break;
                }
        if (pick < 0) {
            int best = 0;
            for (int i = 0; i < n; i++) {
                int w = F(WORD, (BYTE*)g_joyRecords + i * 568, 40);
                if (w > best) {
                    pick = i;
                    best = w;
                }
            }
            if (pick < 0)
                pick = 0;
        }
        CallC(FUN_5787a0, pick, wnd, (float)range, 0);
    }
    return g_joyDevice ? 0 : 0x80004005;
}

ASM_PROC(FUN_579aa0)    // a shadow's set-up
ASM_VAR(BYTE*, g_game57c490, $L_6d2078)

// [FUN_57c490] a new shadow at a point, its detail (32, 16 or 4 steps) by the distance to the camera (at least minLevel)
extern "C" void __fastcall Shadow_Add(BYTE* s, void*, int a0, int a1, const float* at, unsigned minLevel, int a4, int a5)
{
    BYTE* stack = s + 76;
    BYTE* o = ShadowStack_Push(stack, 0);
    if (!o)
        return;
    const float* cam = (const float*)(g_game57c490 + 1460);
    Vec3 d;
    d.x = (float)((double)at[0] - cam[0]);
    d.y = (float)((double)at[1] - cam[1]);
    d.z = (float)((double)at[2] - cam[2]);
    float len = (float)Vec3_Length(&d);
    unsigned level = 2;
    if (!(len >= 20.0f)) level = 0;
    else if (!(len >= 50.0f)) level = 1;
    else if (!(len >= 90.0f)) level = 2;
    else if (!(len >= 150.0f)) level = 3;
    if (level < minLevel)
        level = minLevel;
    int steps;
    switch (level) {
    case 0: steps = 32; break;
    case 1: steps = 16; break;
    case 2: case 3: steps = 4; break;
    default: steps = 3; break;
    }
    if (!CallThis<int>(FUN_579aa0, o, a0, a1, at, steps, a4, a5))
        ShadowStack_Pop(stack, 0);
}

ASM_PROC(FUN_5787a0)
extern "C" void FUN_579890(); KING_ALIAS("_FUN_579890", "@ShadowStack_Push@8")
ASM_PROC(FUN_57b2d0)
extern "C" void FUN_5798d0(); KING_ALIAS("_FUN_5798d0", "@ShadowStack_Pop@8")
ASM_VAR(int, g_6f4bb4, $L_6f4bb4)
ASM_VAR(int, g_6f4bb8, $L_6f4bb8)
ASM_VAR(int, g_6f4bbc, $L_6f4bbc)

// [FUN_578d50] FUN_5787a0 with the three globals at 6f4bb4
extern "C" int __cdecl FF_578d50(int a)
{
    return CallC<int>(FUN_5787a0, a, g_6f4bb4, g_6f4bb8, g_6f4bbc);
}

// [FUN_57c450] takes an object from the pool at +76 and starts it; puts it back when that fails
extern "C" int __fastcall FF_PoolStart(BYTE* self, int, int a, int b, int c, int d, int e)
{
    BYTE* pool = self + 76;
    void* obj = CallThis<void*>(FUN_579890, pool);
    if (!obj)
        return 0;
    int r = CallThis<int>(FUN_57b2d0, obj, a, b, c, 16, d, e);
    if (!r)
        r = CallThis<int>(FUN_5798d0, pool);
    return r;
}

// [FUN_579850] frees the new[]'d array of 6928-byte items (their destructor does nothing) and clears the count
extern "C" void __fastcall Pool6928_Free(BYTE** self, void*)
{
    BYTE* a = self[0];
    self[1] = 0;
    if (a)
        operator delete(a - 4);
}

// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_4bd570(); KING_ALIAS("_FUN_4bd570", "@EffectBuf_Destroy@8")
extern "C" void FUN_579650(); KING_ALIAS("_FUN_579650", "@FrameTask_Destroy@8")
extern "C" void FUN_57c360(); KING_ALIAS("_FUN_57c360", "@PoolNode_Destroy@8")
ASM_VAR(DWORD, g_forfee6f63b8, $L_6f63b8)
ASM_VAR(DWORD, g_forfee6f63bc, $L_6f63bc)
ASM_VAR(DWORD, g_forfee6f63c0, $L_6f63c0)
ASM_VAR(DWORD, g_forfee6f63c4, $L_6f63c4)
ASM_VAR(DWORD, g_forfee6f63d8, $L_6f63d8)
ASM_VAR(DWORD, g_forfee6f63dc, $L_6f63dc)
ASM_VAR(DWORD, g_forfee6f63e0, $L_6f63e0)

static BYTE* Forfee_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_5795e0]
extern "C" BYTE* __fastcall Forfee5795e0_DeletingDestructor(BYTE* s, void*, int flags) { return Forfee_DeleteAfter(s, flags, FUN_579650); }
// [$L_57c320]
extern "C" BYTE* __fastcall Forfee57c320_DeletingDestructor(BYTE* s, void*, int flags) { return Forfee_DeleteAfter(s, flags, FUN_4bd570); }
// [$L_57c340]
extern "C" BYTE* __fastcall Forfee57c340_DeletingDestructor(BYTE* s, void*, int flags) { return Forfee_DeleteAfter(s, flags, FUN_57c360); }
// [$L_57c970]
extern "C" void __cdecl ForfeeInit_57c970() { g_forfee6f63d8 = 0x0; g_forfee6f63dc = 0x0; g_forfee6f63e0 = 0xc11ccccd; }
// [$L_579900]
extern "C" void __cdecl ForfeeInit_579900() { g_forfee6f63b8 = 0x0; g_forfee6f63bc = 0x0; g_forfee6f63c0 = 0x0; g_forfee6f63c4 = 0x0; }

extern "C" void FUN_5793f0(); KING_ALIAS("_FUN_5793f0", "_Pooled_Release")
ASM_VAR(AsmLabel, g_forfee650334, $L_650334)
ASM_VAR(AsmLabel, g_forfee6f6140, $L_6f6140)

// [$L_579340] the base destructor (vtable 650334) inlined; freed with FUN_5793f0
extern "C" BYTE* __fastcall Forfee579340_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_forfee650334;
    F(int, s, 4) = 0;
    if (flags & 1)
        CallC(FUN_5793f0, s);
    return s;
}

// [$L_579940] static initializer: 40 zero 16-byte records at 6f6138
extern "C" void __cdecl ForfeeInit_579940()
{
    DWORD* p = (DWORD*)g_forfee6f6140 - 2;
    for (int i = 0; i < 160; i++)
        p[i] = 0;
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

extern "C" void FUN_579880(); KING_ALIAS("_FUN_579880", "@Pool_HasNoFree@8")
extern "C" void FUN_5798d0(); KING_ALIAS("_FUN_5798d0", "@ShadowStack_Pop@8")
extern "C" void FUN_579a30(); KING_ALIAS("_FUN_579a30", "@Triangles_Draw@16")

// a COM method of the device d (vtable slot at byte offset off)
template <class... A>
static int Ff_Com(void* d, unsigned off, A... args)
{
    void* fn = *(void**)(*(char**)d + off);
    return ((int(__stdcall*)(void*, A...))fn)(d, args...);
}

// [$L_57c3d0] draws the effects of the list (+76) with render state 22 at 1, then restored
extern "C" int __fastcall Ff_DrawList(BYTE* s, void*, int, int b)
{
    Ff_Com(F(void*, s, 84), 80, 136, 1);
    DWORD old;
    Ff_Com(F(void*, s, 84), 84, 22, &old);
    Ff_Com(F(void*, s, 84), 80, 22, 1);
    BYTE* l = s + 76;
    while (!CallThis<int>(FUN_579880, l)) {
        BYTE* o = CallThis<BYTE*>(FUN_5798d0, l);
        CallThis(FUN_579a30, o, b, F(int, s, 84));
    }
    Ff_Com(F(void*, s, 84), 80, 22, old);
    return 1;
}

// ---------------------------------------------------------------------------
// More virtual methods.

extern "C" void FUN_579460(); KING_ALIAS("_FUN_579460", "@Pool_NextUsed@12")

extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")

// [$L_5796e0] the pools (+32, count +432): each used item gets vtbl+4 until no new first item appears, then the
// items marked +4 are deleted; then released
extern "C" int __fastcall Ff_ClearPools(BYTE* s)
{
    for (unsigned i = 0; i < F(unsigned, s, 432); i++) {
        BYTE* pool = F(BYTE*, s, 32 + i * 4);
        BYTE* prev = 0;
        BYTE* first;
        do {
            first = CallThis<BYTE*>(FUN_579460, pool, 0);
            if (first != prev) {
                BYTE* p = first;
                BYTE* n;
                do {
                    n = CallThis<BYTE*>(FUN_579460, pool, p);
                    CallVirt(p, 4);
                    p = n;
                } while (n != prev);
            }
            prev = first;
        } while (CallThis<BYTE*>(FUN_579460, pool, 0) != first);
    }
    for (unsigned i = 0; i < F(unsigned, s, 432); i++) {
        BYTE* pool = F(BYTE*, s, 32 + i * 4);
        for (BYTE* p = CallThis<BYTE*>(FUN_579460, pool, 0); p;) {
            BYTE* n = CallThis<BYTE*>(FUN_579460, pool, p);
            if (F(int, p, 4) && p)
                CallVirt(p, 0, 1);
            p = n;
        }
    }
    return CallThis<int>(FUN_5d32f0, s, 1);
}
