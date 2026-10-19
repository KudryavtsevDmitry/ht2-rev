// world.cpp - small functions of the unit at 0x4e24c0 of king.masm: the game
// time and frame time, a degree table, matrix decomposition wrappers, the
// queries against world objects (a 208-byte query with the start point at
// +76 and the direction at +88), and the warn.log / vehprnNN.log files.
//
// Building with  nmake ASM_WORLD=1  makes king.masm assemble the original
// procedures again.

#include "vecmath.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl fflush(FILE* f);

ASM_PROC(FUN_5de950)    // decomposes a matrix (by value) into six doubles
ASM_PROC(FUN_5da950)
extern "C" int __fastcall Obj4eb150_Kind(BYTE* self, void*);    // carphys.cpp
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")    // set a switch
ASM_PROC(FUN_5fd430)    // sound: play (volume, pitch, a)
ASM_PROC(FUN_5fdaf0)    // sound: play (volume, pitch)
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")    // query: constructor
ASM_PROC(FUN_4e5698)    // query: finish a hit

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
extern "C" unsigned __cdecl Query_Parts2(int a, int mode, BYTE* q);
extern "C" unsigned __cdecl Query_Parts1(int a, int mode, BYTE* q);
extern "C" unsigned __cdecl Query_4e7168(int a, int skip, float dist, const Vec3Bits* p, const Vec3Bits* d, BYTE* q);
extern "C" unsigned __cdecl Query_4e7476(void* obj, int skip, float dist, const Vec3Bits* p, const Vec3Bits* d, BYTE* q);
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
    return (Query_Parts2(a, 4, q) & 1) ? 1 : 0;
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
    int r = Query_Parts2(a, 6, q);
    return (r & 1) ? r : 0;
}

extern "C" int __cdecl Query_4e6ac4(int a, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    QueryInit(q, p, d);
    F(int, q, 0) = 0;
    F(int, q, 120) = 0;
    int r = Query_Parts1(a, 4, q);
    return (r & 1) ? r : 0;
}

extern "C" int __cdecl Query_4e6b4d(int skip, int a, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    QueryInit(q, p, d);
    F(int, q, 0) = 0;
    F(int, q, 120) = skip;
    int r = Query_Parts1(a, 4, q);
    return (r & 1) ? r : 0;
}

extern "C" int __cdecl Query_4e6d41(int skip, int a, int dist, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    QueryInit(q, p, d);
    F(int, q, 0) = dist;
    F(int, q, 120) = skip;
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    int r = Query_Parts1(a, 6, q);
    return (r & 1) ? r : 0;
}

extern "C" int __cdecl Query_4e7261(int a, int b, int c, int d, int e, Vec3Bits* point, Vec3Bits* normal)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    return HitResult(Query_4e7168(a, b, *(float*)&c, (const Vec3Bits*)d, (const Vec3Bits*)e, q), q, point, normal);
}

extern "C" int __cdecl Query_4e7566(int a, int b, int c, int d, int e, Vec3Bits* point, Vec3Bits* normal)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    return HitResult(Query_4e7476((void*)a, b, *(float*)&c, (const Vec3Bits*)d, (const Vec3Bits*)e, q), q, point, normal);
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

// ---------------------------------------------------------------------------
// medium-sized functions of the unit

extern "C" void FUN_4e5dce(); KING_ALIAS("_FUN_4e5dce", "_Model_FindPart2")    // an object's first part to query
extern "C" void FUN_4e5c77(); KING_ALIAS("_FUN_4e5c77", "_Model_FindPart1")    // an object's other part to query
ASM_PROC(FUN_4e5d60)
ASM_PROC(FUN_4e6362)
extern "C" Vec3* __fastcall Vec3_DivAssign(Vec3* self, void*, Vec3* ret, float s);   // vecmath.cpp
ASM_VAR(double, g_64b658, $L_64b658)            // 0.001

// the object at a point: a query of mode 0 (r == 0) or 2
extern "C" int __cdecl World_ObjectAt(void* obj, float r, const Vec3Bits* p, const Vec3Bits* d)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    QueryInit(q, p, d);
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
    F(float, q, 0) = r;
    F(int, q, 100) = 0;
    int res = Run(obj, x87_eq(r, 0.0) ? 0 : 2, q);
    return (res & 2) == 2 ? F(int, q, 104) : 0;
}

static inline void* Part2(void* o) { return CallC<void*>(FUN_4e5c77, o); }
static inline void* Part1(void* o) { return CallC<void*>(FUN_4e5dce, o); }

// a query of the object's second part (and its sub-part), else of the object found there
extern "C" unsigned __cdecl Query_Parts2(int a, int mode, BYTE* q)
{
    BYTE saved[208];
    unsigned r = 0;
    memcpy(saved, q, 208);
    void* p = Part2((void*)a);
    if (p) {
        void* s = CallC<void*>(FUN_4e5d60, p);
        if (s) {
            r = Run(s, mode, q);
            if (r & 1) {
                CallC(FUN_4e5698, q);
                return r;
            }
            memcpy(q, saved, 208);
        }
        r = Run(p, mode, q);
        if (r & 1) {
            CallC(FUN_4e5698, q);
            return r;
        }
    }
    int o = World_ObjectAt((void*)a, F(float, q, 0), (const Vec3Bits*)(q + 76), (const Vec3Bits*)(q + 88));
    if (o && (p = Part2((void*)o)) != 0) {
        void* s = CallC<void*>(FUN_4e5d60, p);
        if (s) {
            r = Run(s, mode, q) | 2;
            F(int, q, 104) = o;
            if (r & 1) {
                CallC(FUN_4e5698, q);
                return r;
            }
            memcpy(q, saved, 208);
        }
        r = Run(p, mode, q) | 2;
        if (r & 1)
            CallC(FUN_4e5698, q);
        F(int, q, 104) = o;
    }
    return r;
}

// a query of the object's first part, else of the object found there
extern "C" unsigned __cdecl Query_Parts1(int a, int mode, BYTE* q)
{
    BYTE saved[208];
    unsigned r = 0;
    memcpy(saved, q, 208);
    void* p = Part1((void*)a);
    if (p) {
        r = Run(p, mode, q);
        if (r & 1) {
            CallC(FUN_4e5698, q);
            return r;
        }
    }
    int o = World_ObjectAt((void*)a, F(float, q, 0), (const Vec3Bits*)(q + 76), (const Vec3Bits*)(q + 88));
    if (o && (p = Part1((void*)o)) != 0) {
        memcpy(q, saved, 208);
        r = Run(p, mode, q) | 2;
        F(int, q, 104) = o;
        if (r & 1)
            CallC(FUN_4e5698, q);
    }
    return r;
}

// the point on the line p4..p5 nearest along n to p2 (or FUN_4e6362's when short)
extern "C" Vec3* __cdecl Line_Project(Vec3* out, const Vec3* p2, const Vec3* n, const Vec3* p4, const Vec3* p5)
{
    Vec3 d, t, t2, t3;
    Vec3_Sub(&d, p5, p4);
    float len = (float)Vec3_Length(&d);
    if (!(len >= g_64b658)) {
        CallC(FUN_4e6362, out, p2, n, p4);
        return out;
    }
    Vec3_DivAssign(&d, 0, &t, len);
    double x = Vec3_Dot(n, Vec3_Sub(&t2, p2, p4));
    double r = x / Vec3_Dot(n, &d);
    Vec3_Add(out, p4, Vec3_Scaled(&t3, &d, (float)r));
    return out;
}

extern "C" int __cdecl World_Hit(int a, const Vec3Bits* p, const Vec3Bits* d, Vec3Bits* point, Vec3Bits* normal)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    QueryInit(q, p, d);
    F(int, q, 0) = 0;
    F(int, q, 120) = 0;
    if (!(Query_Parts1(a, 4, q) & 1))
        return 0;
    *point = F(Vec3Bits, q, 52);
    *normal = F(Vec3Bits, q, 64);
    return 1;
}

extern "C" unsigned __cdecl Query_4e7168(int a, int skip, float dist, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    BYTE saved[208];
    memcpy(saved, q, 208);
    unsigned r;
    if (dist > 0.0) {
        r = Query_4e67f9(skip, a, *(int*)&dist, p, d, q);
        if (r & 1)
            return r;
        memcpy(q, saved, 208);
        return Query_4e6d41(skip, a, *(int*)&dist, p, d, q);
    }
    r = Query_4e6621(skip, a, p, d, q);
    if (r & 1)
        return r;
    memcpy(q, saved, 208);
    return Query_4e6b4d(skip, a, p, d, q);
}

extern "C" unsigned __cdecl Query_4e72ef(int a, int skip, float dist, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    BYTE saved[208];
    memcpy(saved, q, 208);
    unsigned r;
    if (dist > 0.0) {
        r = Query_4e6d41(skip, a, *(int*)&dist, p, d, q);
        if (r & 1)
            return r;
        memcpy(q, saved, 208);
        return Query_4e67f9(skip, a, *(int*)&dist, p, d, q);
    }
    r = Query_4e6b4d(skip, a, p, d, q);
    if (r & 1)
        return r;
    memcpy(q, saved, 208);
    return Query_4e6621(skip, a, p, d, q);
}

extern "C" unsigned __cdecl Query_4e7476(void* obj, int skip, float dist, const Vec3Bits* p, const Vec3Bits* d,
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
    F(int, q, 104) = 0;
    unsigned r = Run(obj, dist > 0.0 ? 6 : 4, q);
    if (r & 1)
        CallC(FUN_4e5698, q);
    return r;
}

// the game's _matherr: acos just outside -1..1 and sqrt of a negative are fixed
extern "C" int __cdecl World_MathErr(int* e)
{
    double arg = F(double, e, 8);
    if (e[0] == 1 && strcmp((const char*)e[1], "acos") == 0) {
        if (!(arg >= -1.0) && arg > -1.01) {
            e[6] = 0x4d12d84a;      // 3.14159265
            e[7] = 0x400921fb;
            return 1;
        }
        if (arg > 1.0 && arg < 1.01) {
            e[6] = 0;
            e[7] = 0;
            return 1;
        }
    }
    if (e[0] == 1 && strcmp((const char*)e[1], "sqrt") == 0) {
        e[6] = 0;
        e[7] = 0;
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Fatal errors with the world state, and checked object accessors.
extern "C" void FUN_4cf5e0(); KING_ALIAS("_FUN_4cf5e0", "@Fx_Get12@8")       // an object's name
extern "C" void FUN_4eb150(); KING_ALIAS("_FUN_4eb150", "@Obj4eb150_Kind@8")
extern "C" void FUN_5e19b0(); KING_ALIAS("_FUN_5e19b0", "_Frame_Apply")
extern "C" void FUN_5e19f0(); KING_ALIAS("_FUN_5e19f0", "_Frame_GetLocal")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
ASM_PROC(FUN_4e4468)    // writes the world state text
ASM_PROC(FUN_4e2556)    // writes a text to the log
ASM_PROC(FUN_53f0c0)
ASM_VAR(AsmLabel, g_state696e5c, $L_696e5c)
ASM_VAR(char*, g_fatalText696d20, $L_696d20)
ASM_VAR(AsmLabel, g_fatalBuf696d24, $L_696d24)
ASM_VAR(AsmLabel, g_fatalBufEnd696e23, $L_696e23)
ASM_VAR(char*, g_cmdLine672d10, $L_672d10)
ASM_VAR(AsmLabel, g_level697460, $L_697460)
ASM_VAR(AsmLabel, g_levelEnd69746f, $L_69746f)
ASM_VAR(AsmLabel, s_673680, $L_673680)
ASM_VAR(AsmLabel, s_6736a8, $L_6736a8)
ASM_VAR(AsmLabel, s_673a30, $L_673a30)
ASM_VAR(AsmLabel, s_673a44, $L_673a44)
ASM_VAR(AsmLabel, s_673ad4, $L_673ad4)
ASM_VAR(AsmLabel, s_673ae4, $L_673ae4)
ASM_VAR(AsmLabel, s_673b0c, $L_673b0c)
ASM_VAR(AsmLabel, s_673b30, $L_673b30)
ASM_VAR(AsmLabel, s_673b40, $L_673b40)
ASM_VAR(AsmLabel, s_673b94, $L_673b94)
ASM_VAR(AsmLabel, s_673ba0, $L_673ba0)
ASM_VAR(AsmLabel, s_673a68, $L_673a68)
ASM_VAR(AsmLabel, s_673a6c, $L_673a6c)
ASM_VAR(AsmLabel, s_673a9c, $L_673a9c)
ASM_VAR(AsmLabel, s_673cc8, $L_673cc8)
ASM_VAR(AsmLabel, s_673d18, $L_673d18)
ASM_VAR(AsmLabel, s_673d2c, $L_673d2c)
ASM_VAR(AsmLabel, s_673d58, $L_673d58)
extern "C" char* __cdecl strncpy(char* d, const char* s, size_t n);
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// [FUN_4e260c..$L_4e2741] the world's fatal error: logs the world state, the level name and msg, then ends the program
extern "C" void __cdecl World_Fatal(const char* msg)
{
    CallC(FUN_4e4468, (char*)g_state696e5c);
    int n = (int)(strlen(g_state696e5c) + strlen(msg) + 161);
    if (n > 255)
        g_fatalText696d20 = (char*)operator new(n);
    else
        g_fatalText696d20 = g_fatalBuf696d24;
    int i;
    for (i = 0; i < 50; i++)
        if (g_cmdLine672d10[i] == '=')
            break;
    strncpy(g_level697460, g_cmdLine672d10 + i + 1, 15);
    g_levelEnd69746f[0] = 0;
    sprintf(g_fatalText696d20, s_673680, (char*)g_state696e5c, (char*)g_level697460, msg);
    CallC(FUN_4e2556, g_fatalText696d20);
    CallC(FUN_53f0c0);
    if (n > 255)
    {
        strncpy(g_fatalBuf696d24, g_fatalText696d20, 255);
        g_fatalBufEnd696e23[0] = 0;
        operator delete(g_fatalText696d20);
    }
    Sys_Fatal(s_6736a8, 0);
}

// [FUN_4e460b..$L_4e466a] the world's fatal error about an object (its name and kind) or about none
extern "C" void __cdecl World_ObjFatal(BYTE* obj, const char* msg)
{
    char text[152];
    if (!obj)
        sprintf(text, s_673a30, msg);
    else
    {
        int kind = CallThis<int>(FUN_4eb150, obj);
        sprintf(text, s_673a44, CallThis<const char*>(FUN_4cf5e0, obj), kind, msg);
    }
    World_Fatal(text);
}

// [FUN_4e480c..$L_4e488b] selects child idx of a switch object (kind 18); fatal for anything else or a bad index
extern "C" int __cdecl Switch_SelectChecked(BYTE* obj, int idx)
{
    if (!obj)
        World_ObjFatal(obj, s_673ad4);
    if (CallThis<int>(FUN_4eb150, obj) != 18)
        World_ObjFatal(obj, s_673ae4);
    if (idx < 0 || idx >= CallVirt<int>(obj, 60))
    {
        char text[100];
        int count = CallVirt<int>(obj, 60);
        sprintf(text, s_673b0c, idx, count);
        World_ObjFatal(obj, text);
    }
    return CallC<int>(FUN_5e1a10, obj, idx);
}

// [FUN_4e48e2..$L_4e4917] Frame_Apply on a frame object (kind 6); fatal for anything else
extern "C" int __cdecl Frame_ApplyChecked(BYTE* obj, int a, int b)
{
    if (!obj)
        World_ObjFatal(obj, s_673b30);
    if (CallThis<int>(FUN_4eb150, obj) != 6)
        World_ObjFatal(obj, s_673b40);
    return CallC<int>(FUN_5e19b0, obj, a, b);
}

// [FUN_4e497a..$L_4e49af] Frame_GetLocal of a frame object (kind 6); fatal for anything else
extern "C" int __cdecl Frame_GetLocalChecked(BYTE* obj, int a)
{
    if (!obj)
        World_ObjFatal(obj, s_673b94);
    if (CallThis<int>(FUN_4eb150, obj) != 6)
        World_ObjFatal(obj, s_673ba0);
    return CallC<int>(FUN_5e19f0, obj, a);
}

// [FUN_4e4750..$L_4e4802] the scene object name under parent; fatal (with an optional prefix text) when missing
extern "C" BYTE* __cdecl Scene_FindOrFail(BYTE* parent, const char* name, const char* prefix)
{
    BYTE* r = CallC<BYTE*>(FUN_5ef2b0, parent, name);
    if (!r)
    {
        char text[208];
        int len = 0;
        if (prefix)
        {
            sprintf(text, s_673a68, prefix);
            len = (int)strlen(text);
        }
        if (!parent)
            sprintf(text + len, s_673a6c, name);
        else
            sprintf(text + len, s_673a9c, name, CallThis<const char*>(FUN_4cf5e0, parent));
        World_Fatal(text);
    }
    return r;
}

// the object named fmt % (obj's name minus its 4-letter prefix) under obj, remembered for the next call
static BYTE* Model_FindCached(BYTE* obj, const char* fmt, const char* shortFmt, int* lastObj, BYTE** last, char* lastName)
{
    const char* name = CallThis<const char*>(FUN_4cf5e0, obj);
    if (*lastObj == (int)obj && !strcmp(lastName, name))
        return *last;
    char text[200];
    if ((int)strlen(name) < 4)
    {
        sprintf(text, shortFmt, name);
        World_Fatal(text);
    }
    char rest[32];
    strcpy(rest, name + 4);
    sprintf(text, fmt, rest);
    BYTE* r = CallC<BYTE*>(FUN_5ef2b0, obj, text);
    if (r)
    {
        *lastObj = (int)obj;
        *last = r;
        strcpy(lastName, name);
    }
    return r;
}

// [FUN_4e5c77..$L_4e5ced] obj's sub-object named by format 673d18 (cached in 6974f0..)
extern "C" BYTE* __cdecl Model_FindPart1(BYTE* obj)
{
    return Model_FindCached(obj, s_673d18, s_673cc8, &g_6974f0, (BYTE**)&g_6974f4, (char*)&g_6974f8);
}

// [FUN_4e5dce..$L_4e5e3b] obj's sub-object named by format 673d58 (cached in 697550..)
extern "C" BYTE* __cdecl Model_FindPart2(BYTE* obj)
{
    return Model_FindCached(obj, s_673d58, s_673d2c, &g_697550, (BYTE**)&g_697554, (char*)&g_697558);
}

