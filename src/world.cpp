// world.cpp - small functions of the unit at 0x4e24c0 of king.masm: the game
// time and frame time, a degree table, matrix decomposition wrappers, the
// queries against world objects (a 208-byte query with the start point at
// +76 and the direction at +88), and the warn.log / vehprnNN.log files.
//
// Building with  nmake ASM_WORLD=1  makes king.masm assemble the original
// procedures again.

#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl fflush(FILE* f);

ASM_PROC(FUN_5de950)    // decomposes a matrix (by value) into six doubles
ASM_PROC(FUN_5da950)
extern "C" int __fastcall Obj4eb150_Kind(BYTE* self, void*);    // carphys.cpp
ASM_PROC(FUN_5e1a10)    // set a switch
ASM_PROC(FUN_5fd430)    // sound: play (volume, pitch, a)
ASM_PROC(FUN_5fdaf0)    // sound: play (volume, pitch)
ASM_PROC(FUN_5ef5c0)    // query: constructor
ASM_PROC(FUN_4e5698)    // query: finish a hit
ASM_PROC(FUN_4e6059)
ASM_PROC(FUN_4e623e)
ASM_PROC(FUN_4e7168)
ASM_PROC(FUN_4e7476)

ASM_VAR(BYTE*, g_6974c4, $L_6974c4)             // +2960 the frame time, +2968 the time (doubles)
ASM_VAR(float, g_radToDeg, $L_6735d0)
typedef float Floats[1];
ASM_VAR(Floats, g_table696ec0, $L_696ec0)       // 360 entries, one per degree
ASM_VAR(int, g_697520, $L_697520)
ASM_VAR(int, g_697524, $L_697524)
ASM_VAR(BYTE, g_697528, $L_697528)
ASM_VAR(int, g_6974f0, $L_6974f0)
ASM_VAR(int, g_6974f4, $L_6974f4)
ASM_VAR(BYTE, g_6974f8, $L_6974f8)
ASM_VAR(int, g_697550, $L_697550)
ASM_VAR(int, g_697554, $L_697554)
ASM_VAR(BYTE, g_697558, $L_697558)
ASM_VAR(int, g_warnLogOn, $L_6974d0)
ASM_VAR(FILE*, g_warnLog, $L_6975a8)
ASM_VAR(int, g_vehLogsOn, $L_6974cc)
typedef FILE* Files[10];
ASM_VAR(Files, g_vehLogs, $L_697580)
ASM_VAR(AsmLabel, g_strWarnLog, $L_674114)      // "warn.log"
ASM_VAR(AsmLabel, g_strWarnLogMode, $L_674110)
ASM_VAR(AsmLabel, g_strVehLog, $L_674120)       // "vehprn%02d.log"
ASM_VAR(AsmLabel, g_strVehLogMode, $L_674130)

struct Vec3Bits { DWORD x, y, z; };
struct Mat48 { DWORD m[12]; };                  // a matrix passed by value

// ---------------------------------------------------------------------------
// Time

// the time of the last frame, at most 0.25 s (g_6974c2 ms when it is set)
extern "C" double __cdecl Game_FrameTime()
{
    if (g_6974c2)
        return (int)g_6974c2 / 1000.0;
    double t = F(double, g_6974c4, 2960);
    if (t > 0.25)
        return 0.25f;
    return t;
}

// the game time: g_6974bc while g_6974c2 is set
extern "C" double __cdecl Game_Time()
{
    if (g_6974c2)
        return g_6974bc;
    return F(double, g_6974c4, 2968);
}

// the table entry of the angle (radians) in whole degrees, 0 outside 0..359
extern "C" float __cdecl Table360_At(float angle)
{
    int i = (int)(angle * (double)g_radToDeg);
    if (i < 0 || i >= 360)
        return 0.0f;
    return g_table696ec0[i];
}

// v . v (as aeffects.cpp Vec3_LengthSq)
extern "C" double __cdecl Vec3_LengthSq4e298b(const Vec3* v)
{
    return Vec3_Dot(v, v);
}

extern "C" void __cdecl Matrix_Decompose6(const Mat48* m, double* out)
{
    CallC(FUN_5de950, *m, out, out + 1, out + 2, out + 3, out + 4, out + 5);
}

extern "C" void __cdecl Matrix_Decompose6F(const Mat48* m, float* a, float* b, float* c, float* d, float* e,
                                           float* f)
{
    double v[6];
    CallC(FUN_5de950, *m, &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]);
    *a = (float)v[0];
    *b = (float)v[1];
    *c = (float)v[2];
    *d = (float)v[3];
    *e = (float)v[4];
    *f = (float)v[5];
}

extern "C" void __cdecl Matrix_Decompose6FArray(const Mat48* m, float* p)
{
    Matrix_Decompose6F(m, p, p + 1, p + 2, p + 3, p + 4, p + 5);
}

extern "C" void* __cdecl World_4e3021(void* out, const int* p)
{
    CallC(FUN_5da950, out, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11]);
    return out;
}

// switch v of obj, when obj is a switch (kind 18) with more than v states
extern "C" void __cdecl Switch_SetChecked(BYTE* obj, int v)
{
    if (obj && Obj4eb150_Kind(obj, 0) == 18 && v >= 0 && v < CallVirt<int>(obj, 60))
        CallC(FUN_5e1a10, obj, v);
}

extern "C" void __cdecl Sound_PlayIf3(void* s, DWORD a, DWORD b, DWORD c)
{
    if (s)
        CallThis(FUN_5fd430, s, a, b, c);
}

extern "C" void __cdecl Sound_PlayIf(void* s, DWORD a, DWORD b)
{
    if (s)
        CallThis(FUN_5fdaf0, s, a, b);
}

// ---------------------------------------------------------------------------
// Queries: +0 a distance, +4 a matrix, +76 the start, +88 the direction,
// +100, +104, +116 cleared, +120 the object to skip; vtbl+52 of a world
// object runs one (mode 0, 4 or 6), bit 0 of the result: hit

static void QueryInit(BYTE* q, const Vec3Bits* p, const Vec3Bits* d)
{
    F(Matrix, q, 4) = g_identity;
    F(Vec3Bits, q, 76) = *p;
    F(Vec3Bits, q, 88) = *d;
}

static int Run(void* obj, int mode, BYTE* q) { return CallVirt<int>(obj, 52, mode, q); }

extern "C" int __cdecl Query_4e52fc(void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    if (!obj)
        return 0;
    QueryInit(q, p, d);
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    F(int, q, 0) = 0;
    F(int, q, 100) = 0;
    return Run(obj, 4, q);
}

extern "C" int __cdecl Query_4e5415(int skip, void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    if (!obj)
        return 0;
    QueryInit(q, p, d);
    F(int, q, 120) = skip;
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    F(int, q, 0) = 0;
    F(int, q, 100) = 0;
    return Run(obj, 4, q);
}

extern "C" int __cdecl Query_4e54b0(void* obj, int skip, const Vec3Bits* p, const Vec3Bits* d, float dist,
                                    BYTE* q)
{
    if (!obj)
        return 0;
    QueryInit(q, p, d);
    F(int, q, 120) = skip;
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    F(float, q, 0) = dist;
    F(int, q, 100) = 0;
    return Run(obj, dist > 0.0 ? 6 : 4, q);
}

extern "C" int __cdecl Query_4e560a(void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q, int dist)
{
    if (!obj)
        return 0;
    QueryInit(q, p, d);
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    F(int, q, 0) = dist;
    F(int, q, 100) = 0;
    return Run(obj, 6, q);
}

extern "C" int __cdecl Query_4e570f(void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    int r = Query_4e52fc(obj, p, d, q);
    if (!(r & 1))
        return 0;
    CallC(FUN_4e5698, q);
    return r;
}

extern "C" int __cdecl Query_4e57e8(int skip, void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    if (!(Query_4e5415(skip, obj, p, d, q) & 1))
        return 0;
    CallC(FUN_4e5698, q);
    return 1;
}

extern "C" int __cdecl Query_4e58ca(int skip, void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q, int dist)
{
    if (!obj)
        return 0;
    QueryInit(q, p, d);
    F(int, q, 120) = skip;
    F(int, q, 116) = 0;
    F(int, q, 0) = dist;
    F(int, q, 100) = 0;
    F(int, q, 104) = 0;
    return Run(obj, 6, q);
}

extern "C" int __cdecl Query_4e5964(int skip, void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q, int dist)
{
    int r = Query_4e58ca(skip, obj, p, d, q, dist);
    if (r & 1)
        CallC(FUN_4e5698, q);
    return r;
}

extern "C" int __cdecl Query_4e5a45(void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q, int dist)
{
    int r = Query_4e560a(obj, p, d, q, dist);
    if (r & 1)
        CallC(FUN_4e5698, q);
    return r;
}

extern "C" int __cdecl Query_4e5b22(void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    if (!obj)
        return 0;
    QueryInit(q, p, d);
    F(int, q, 116) = 0;
    F(int, q, 0) = 0;
    F(int, q, 104) = 0;
    return Run(obj, 0, q);
}

// the object hit along d from p (+104 of the query), else obj
extern "C" void* __cdecl Query_HitObject(void* obj, const Vec3Bits* p, const Vec3Bits* d)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    if (Query_4e5b22(obj, p, d, q) & 2)
        return F(void*, q, 104);
    return obj;
}

extern "C" void __cdecl World_ClearHits()
{
    g_697520 = 0;
    g_697524 = 0;
    g_697528 = 0;
    g_6974f0 = 0;
    g_6974f4 = 0;
    g_6974f8 = 0;
    g_697550 = 0;
    g_697554 = 0;
    g_697558 = 0;
}

extern "C" int __cdecl World_4e634f()
{
    return 0;
}

extern "C" int __cdecl Query_4e6578(int skip, void* obj, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    QueryInit(q, p, d);
    F(int, q, 0) = 0;
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    F(int, q, 120) = skip;
    if (!(Run(obj, 4, q) & 1))
        return 0;
    CallC(FUN_4e5698, q);
    return 1;
}

extern "C" int __cdecl Query_4e6621(int skip, int a, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    QueryInit(q, p, d);
    F(int, q, 0) = 0;
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    F(int, q, 120) = skip;
    return (CallC<int>(FUN_4e6059, a, 4, q) & 1) ? 1 : 0;
}

// the hit point and normal of the query (+52, +64)
static int HitResult(int r, const BYTE* q, Vec3Bits* point, Vec3Bits* normal)
{
    if (r & 1) {
        *point = F(Vec3Bits, q, 52);
        *normal = F(Vec3Bits, q, 64);
    }
    return r;
}

extern "C" int __cdecl Query_4e6749(int a, const Vec3Bits* p, const Vec3Bits* d, Vec3Bits* point, Vec3Bits* normal)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    return HitResult(Query_4e6621(0, a, p, d, q), q, point, normal);
}

extern "C" int __cdecl Query_4e67d1(int a, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    return Query_4e6621(0, a, p, d, q);
}

extern "C" int __cdecl Query_4e67f9(int skip, int a, int dist, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    QueryInit(q, p, d);
    F(int, q, 0) = dist;
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    F(int, q, 120) = skip;
    int r = CallC<int>(FUN_4e6059, a, 6, q);
    return (r & 1) ? r : 0;
}

extern "C" int __cdecl Query_4e6ac4(int a, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    QueryInit(q, p, d);
    F(int, q, 0) = 0;
    F(int, q, 120) = 0;
    int r = CallC<int>(FUN_4e623e, a, 4, q);
    return (r & 1) ? r : 0;
}

extern "C" int __cdecl Query_4e6b4d(int skip, int a, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    QueryInit(q, p, d);
    F(int, q, 0) = 0;
    F(int, q, 120) = skip;
    int r = CallC<int>(FUN_4e623e, a, 4, q);
    return (r & 1) ? r : 0;
}

extern "C" int __cdecl Query_4e6d41(int skip, int a, int dist, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    QueryInit(q, p, d);
    F(int, q, 0) = dist;
    F(int, q, 120) = skip;
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    int r = CallC<int>(FUN_4e623e, a, 6, q);
    return (r & 1) ? r : 0;
}

extern "C" int __cdecl Query_4e7261(int a, int b, int c, int d, int e, Vec3Bits* point, Vec3Bits* normal)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    return HitResult(CallC<int>(FUN_4e7168, a, b, c, d, e, q), q, point, normal);
}

extern "C" int __cdecl Query_4e7566(int a, int b, int c, int d, int e, Vec3Bits* point, Vec3Bits* normal)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    return HitResult(CallC<int>(FUN_4e7476, a, b, c, d, e, q), q, point, normal);
}

extern "C" int __cdecl Query_4e774a(void* obj, int dist, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    if (!obj)
        return 0;
    QueryInit(q, p, d);
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    F(int, q, 0) = dist;
    F(int, q, 100) = 0;
    int r = Run(obj, 6, q);
    if (r)
        CallC(FUN_4e5698, q);
    return r;
}

// ---------------------------------------------------------------------------
// Log files

extern "C" void __cdecl WarnLog_Open()
{
    g_warnLogOn = 1;
    if (!g_warnLog)
        g_warnLog = fopen(g_strWarnLog, g_strWarnLogMode);
}

extern "C" void __cdecl VehLogs_Enable()
{
    g_vehLogsOn = 1;
}

extern "C" void __cdecl VehLogs_Close()
{
    g_vehLogsOn = 0;
    for (int i = 0; i < 10; i++) {
        if (g_vehLogs[i])
            fclose(g_vehLogs[i]);
        g_vehLogs[i] = 0;
    }
}

extern "C" void __cdecl WarnLog_Close()
{
    g_warnLogOn = 0;
    if (g_warnLog)
        fclose(g_warnLog);
    g_warnLog = 0;
}

extern "C" void __cdecl WarnLog_Flush()
{
    if (g_warnLogOn && g_warnLog)
        fflush(g_warnLog);
}

// opens vehprnNN.log for vehicle i (0..10); 1 when it is open
extern "C" int __cdecl VehLog_Open(int i)
{
    if (!g_vehLogsOn || i < 0 || i > 10)
        return 0;
    if (g_vehLogs[i])
        return 1;
    char name[52];
    sprintf(name, g_strVehLog, i);
    g_vehLogs[i] = fopen(name, g_strVehLogMode);
    return g_vehLogs[i] ? 1 : 0;
}

// sets all +28 entries of the arrays at +8 and +12 to a and b
extern "C" void __fastcall Arrays_Fill(BYTE* self, void*, DWORD a, DWORD b)
{
    for (int i = 0; i < F(int, self, 28); i++) {
        F(DWORD*, self, 8)[i] = a;
        F(DWORD*, self, 12)[i] = b;
    }
}

extern "C" void __fastcall Arrays_FillLongAgo(BYTE* self, void*)
{
    float t = (float)(F(double, g_6974c4, 2968) - 1e10);
    Arrays_Fill(self, 0, F(DWORD, &t, 0), 0);
}
