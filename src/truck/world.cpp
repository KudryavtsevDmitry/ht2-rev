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

extern "C" void FUN_5de950(); KING_ALIAS("_FUN_5de950", "_Matrix_ToAngles")    // decomposes a matrix (by value) into six doubles
extern "C" void FUN_5da950(); KING_ALIAS("_FUN_5da950", "_Matrix_FromPose")
extern "C" int __fastcall Obj4eb150_Kind(BYTE* self, void*);    // carphys.cpp
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")    // set a switch
extern "C" void FUN_5fd430(); KING_ALIAS("_FUN_5fd430", "@Sound_Play@20")    // sound: play (volume, pitch, a)
extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")    // sound: play (volume, pitch)
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")    // query: constructor
extern "C" void FUN_4e5698(); KING_ALIAS("_FUN_4e5698", "_Obj_TransformVecs")    // query: finish a hit

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
extern "C" void FUN_4e5d60(); KING_ALIAS("_FUN_4e5d60", "_Obj_FindPart673d20")
extern "C" void FUN_4e6362(); KING_ALIAS("_FUN_4e6362", "_Vec3_RejectFrom")
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
extern "C" void FUN_4e4468(); KING_ALIAS("_FUN_4e4468", "_Clock_Text")    // writes the world state text
extern "C" void FUN_4e2556(); KING_ALIAS("_FUN_4e2556", "_Dump_Text")    // writes a text to the log
extern "C" void FUN_53f0c0(); KING_ALIAS("_FUN_53f0c0", "_Cars_DumpHistory")
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


// ---------------------------------------------------------------------------
ASM_PROC(FUN_4e4f78)    // (a, b, c, &lo, &hi)
extern "C" void FUN_4e28b0(); KING_ALIAS("_FUN_4e28b0", "_Vec3_OutOfBox")
extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")
extern "C" void FUN_5fd430(); KING_ALIAS("_FUN_5fd430", "@Sound_Play@20")
extern "C" void FUN_4eac5f(); KING_ALIAS("_FUN_4eac5f", "@Arrays_FillLongAgo@8")
extern "C" void FUN_4bde00(); KING_ALIAS("_FUN_4bde00", "_Vec3_Sub")
extern "C" void FUN_610f00(); KING_ALIAS("_FUN_610f00", "_Vec3_Dot")
extern "C" void FUN_4bdd60(); KING_ALIAS("_FUN_4bdd60", "_Vec3_Scaled")
extern "C" void FUN_4bddb0(); KING_ALIAS("_FUN_4bddb0", "_Vec3_Add")
ASM_VAR(double, g_64b5a0, $L_64b5a0)
ASM_VAR(double, g_64d668, $L_64d668)
ASM_VAR(float, g_64c264, $L_64c264)
ASM_VAR(float, g_64c250, $L_64c250)
ASM_VAR(AsmLabel, s_6736e4, $L_6736e4)
ASM_VAR(AsmLabel, s_67431c, $L_67431c)

static __declspec(noinline) int World_Trunc(double x)
{
    return (int)x;
}

static void World_PlayRange(BYTE* obj, int a, int b, int c, bool three)
{
    if (!obj)
        return;
    float lo, hi;
    CallC(FUN_4e4f78, a, b, c, &lo, &hi);
    float mx = lo > hi ? lo : hi;
    if (!(mx > g_64b5a0))
        return;
    if (three)
        CallThis(FUN_5fd430, obj, lo, hi, 0);
    else
        CallThis(FUN_5fdaf0, obj, lo, hi);
}

// [FUN_4e51b1..$L_4e51d8] plays a sound at the range of (a, b, c) when it is loud enough
extern "C" void __cdecl Sound_PlayRange(BYTE* obj, int a, int b, int c)
{
    World_PlayRange(obj, a, b, c, false);
}

// [FUN_4e5140..$L_4e5167] plays a sound (5fd430) at the range of (a, b, c) when it is loud enough
extern "C" void __cdecl Sound_PlayRange3(BYTE* obj, int a, int b, int c)
{
    World_PlayRange(obj, a, b, c, true);
}

// [FUN_4e2821..$L_4e28a9] fatal when the point (z ignored) fails the 4e28b0 check against lim
extern "C" void __cdecl World_CheckPoint(float* v, const char* name, float lim)
{
    float p[3] = { v[0], v[1], 0 };
    if (CallC<int>(FUN_4e28b0, p, lim))
    {
        char text[1012];
        sprintf(text, s_6736e4, name, (double)lim, (double)v[0], (double)v[1], (double)v[2]);
        World_Fatal(text);
    }
}

// [FUN_4eab58..$L_4eab7b] a history of length/step samples: two arrays, step at +0, rate at +4
extern "C" BYTE* __fastcall History_Construct(BYTE* self, void*, int a, float length, int c, int d)
{
    if (!(length > g_64c264))
        World_Fatal(s_67431c);
    F(int, self, 20) = a;
    F(int, self, 24) = c;
    F(int, self, 28) = World_Trunc(length / g_64d668);
    F(float, self, 0) = length;
    F(float, self, 4) = (float)(g_64c250 / F(float, self, 0));
    F(void*, self, 8) = operator new(F(int, self, 28) * 4);
    F(void*, self, 12) = operator new(F(int, self, 28) * 4);
    F(int, self, 16) = d;
    CallThis(FUN_4eac5f, self);
    return self;
}

// [FUN_4e6362] out = c - b*dot(b, c - a)
extern "C" float* __cdecl Vec3_RejectFrom(float* out, float* a, float* b, float* c)
{
    float t[3], s[3];
    double d = CallC<double>(FUN_610f00, b, CallC<float*>(FUN_4bde00, t, c, a));
    CallC(FUN_4bdd60, s, b, (float)d);
    CallC(FUN_4bde00, out, c, s);
    return out;
}

// [FUN_4e6490] the point c + e*t on the plane (a, normal b): t = dot(b, c - a) / dot(b, e)
extern "C" float* __cdecl Vec3_PlaneHit(float* out, float* a, float* b, float* c, float* e)
{
    float t[3], s[3];
    double n = CallC<double>(FUN_610f00, b, CallC<float*>(FUN_4bde00, t, c, a));
    double r = n / CallC<double>(FUN_610f00, b, e);
    CallC(FUN_4bdd60, s, e, (float)r);
    CallC(FUN_4bddb0, out, c, s);
    return out;
}

// [FUN_4e6502] Vec3_PlaneHit that also stores t
extern "C" float* __cdecl Vec3_PlaneHitT(float* out, float* a, float* b, float* c, float* e, double* tOut)
{
    float t[3], s[3];
    double n = CallC<double>(FUN_610f00, b, CallC<float*>(FUN_4bde00, t, c, a));
    *tOut = n / CallC<double>(FUN_610f00, b, e);
    CallC(FUN_4bdd60, s, e, (float)*tOut);
    CallC(FUN_4bddb0, out, c, s);
    return out;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_4e9ce4(); KING_ALIAS("_FUN_4e9ce4", "_WarnLog_Open")    // log: lock
extern "C" void FUN_4e9d90(); KING_ALIAS("_FUN_4e9d90", "_WarnLog_Close")    // log: unlock
ASM_VAR(int, g_wd6974d4, $L_6974d4)
ASM_VAR(int, g_wd6974d0, $L_6974d0)
ASM_VAR(FILE*, g_wd6975a8, $L_6975a8)
ASM_VAR(AsmLabel, s_wd6741e0, $L_6741e0)
ASM_VAR(AsmLabel, s_wd6741e4, $L_6741e4)
extern "C" int __cdecl fprintf(FILE* f, const char* format, ...);

// [FUN_4eac8e] frees the two buffers (+8, +12) when +28 is set
extern "C" void __fastcall Arrays_Free(BYTE* self, void*)
{
    if (F(int, self, 28))
    {
        operator delete(F(void*, self, 8));
        operator delete(F(void*, self, 12));
    }
}

// writes a value with a format to the vehicle log when it is open
static void Wd_LogValue(const char* fmt, int v)
{
    if (g_wd6974d4)
        CallC(FUN_4e9ce4);
    if (!g_wd6974d0 || !g_wd6975a8)
        return;
    fprintf(g_wd6975a8, fmt, v);
    if (g_wd6974d4)
        CallC(FUN_4e9d90);
}

// [FUN_4ea27e] writes a value to the vehicle log (format 6741e0)
extern "C" void __cdecl VehLog_Value(int v) { Wd_LogValue(s_wd6741e0, v); }

// [FUN_4ea2cb] writes a value to the vehicle log (format 6741e4)
extern "C" void __cdecl VehLog_Value2(int v) { Wd_LogValue(s_wd6741e4, v); }


// ---------------------------------------------------------------------------
extern "C" void FUN_4cf5e0(); KING_ALIAS("_FUN_4cf5e0", "@Fx_Get12@8")    // an object's name
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_43f5b0(); KING_ALIAS("_FUN_43f5b0", "_Vec3_MulMatrix")
extern "C" void FUN_4cb2b0(); KING_ALIAS("_FUN_4cb2b0", "_Vec3_RotateBy")
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
extern "C" void FUN_4e5b22(); KING_ALIAS("_FUN_4e5b22", "_Query_4e5b22")
extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
extern "C" void FUN_5ac0c0(); KING_ALIAS("_FUN_5ac0c0", "_Matrix_RotationZ")
extern "C" void FUN_5abee0(); KING_ALIAS("_FUN_5abee0", "_Matrix_RotationX")
extern "C" void FUN_5abfd0(); KING_ALIAS("_FUN_5abfd0", "_Matrix_RotationY")
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
ASM_VAR(BYTE*, g_wd697520, $L_697520)
ASM_VAR(BYTE*, g_wd697524, $L_697524)
ASM_VAR(AsmLabel, g_wd697528, $L_697528)
ASM_VAR(AsmLabel, s_wd673d20, $L_673d20)

// [FUN_4e5d60] the sub-object 673d20 of an object, cached by object and name
extern "C" BYTE* __cdecl Obj_FindPart673d20(BYTE* o)
{
    const char* name = CallThis<const char*>(FUN_4cf5e0, o);
    if (g_wd697520 == o && !strcmp(g_wd697528, name))
        return g_wd697524;
    strcpy(g_wd697528, name);
    g_wd697524 = CallC<BYTE*>(FUN_5ef2b0, o, (char*)s_wd673d20);
    g_wd697520 = o;
    return g_wd697524;
}

// [FUN_4e5698] transforms the two vectors at +52 and +64 by the matrix made from +4
extern "C" void __cdecl Obj_TransformVecs(BYTE* o)
{
    BYTE m[48];
    Vec3 t;
    CallC(FUN_5ac2e0, m, o + 4);
    F(Vec3, o, 52) = *CallC<Vec3*>(FUN_43f5b0, &t, o + 52, m);
    F(Vec3, o, 64) = *CallC<Vec3*>(FUN_4cb2b0, &t, o + 64, m);
}

// [FUN_4e5bf5] looks a point up through 4e5b22 when out is empty: flag 2 returns the found object (+104), flag 4 copies its name (+124) to out, else clears out; returns a
extern "C" int __cdecl Obj_Lookup4e5b22(int a, int b, int c, char* out)
{
    if (*out)
        return a;
    BYTE r[208];
    CallThis(FUN_5ef5c0, r);
    int f = CallC<int>(FUN_4e5b22, a, b, c, r);
    if (f & 2)
        return F(int, r, 104);
    if (f & 4)
        strcpy(out, (char*)r + 124);
    else
        *out = 0;
    return a;
}

// [FUN_4e3537] out = translation(p) * (5abfd0(rz) * 5abee0(ry)) * 5ac0c0(rx)
extern "C" BYTE* __cdecl Matrix_FromPosAngles(BYTE* out, Vec3 p, float rx, float ry, float rz)
{
    BYTE t[48], a[48], b[48], c[48], d[48], e[48];
    void* T = CallC<void*>(FUN_5abc40, t, p);
    void* R1 = CallC<void*>(FUN_5ac0c0, a, rx);
    void* R2 = CallC<void*>(FUN_5abee0, b, ry);
    void* R3 = CallC<void*>(FUN_5abfd0, c, rz);
    void* M1 = CallC<void*>(FUN_5aba40, d, R3, R2);
    void* M2 = CallC<void*>(FUN_5aba40, e, M1, R1);
    CallC(FUN_5aba40, out, M2, T);
    return out;
}


// ---------------------------------------------------------------------------
ASM_VAR(AsmLabel, s_wd67361c, $L_67361c)
ASM_VAR(AsmLabel, s_wd673620, $L_673620)
ASM_VAR(AsmLabel, s_wd67362c, $L_67362c)
extern "C" size_t __cdecl fwrite(const void* buf, size_t size, size_t n, FILE* f);

// [FUN_4e2556] writes a text to the dump file 673620 (a long one as its length line and 50 bytes)
extern "C" void __cdecl Dump_Text(const char* text)
{
    FILE* f = fopen(s_wd673620, s_wd67361c);
    int n = strlen(text);
    if (n < 0 || n > 10000)
    {
        char buf[100];
        sprintf(buf, s_wd67362c, n, n);
        fwrite(buf, strlen(buf), 1, f);
        fwrite(text, 50, 1, f);
    }
    else
        fwrite(text, strlen(text), 1, f);
    fclose(f);
}


extern "C" void FUN_4e4aef(); KING_ALIAS("_FUN_4e4aef", "_World_InitSideDirections")

// [$L_4e4ae5]
extern "C" void __cdecl World_4e4ae5() { CallC(FUN_4e4aef); }

// ---------------------------------------------------------------------------
// Small setup procs: static vectors, a light array, the race start, a stream read.

extern "C" void FUN_4bdef0(); KING_ALIAS("_FUN_4bdef0", "@Vec3_SetThis@20")
extern "C" void FUN_5ab400(); KING_ALIAS("_FUN_5ab400", "_Vec3_Normalized")
ASM_VAR(AsmLabel, g_q48_696e28, $L_696e28)
ASM_VAR(AsmLabel, g_q48_696e38, $L_696e38)
// [FUN_4e4aef] the static directions 696e28 = normalized (-1, 0.5, 0) and 696e38 = normalized (1, 0.5, 0)
extern "C" void __cdecl World_InitSideDirections(void)
{
    float a[3], b[3];
    CallC(FUN_5ab400, (void*)g_q48_696e28, CallThis<float*>(FUN_4bdef0, a, -1.0f, 0.5f, 0.0f));
    CallC(FUN_5ab400, (void*)g_q48_696e38, CallThis<float*>(FUN_4bdef0, b, 1.0f, 0.5f, 0.0f));
}

// ---------------------------------------------------------------------------
// Symbols from the developers' linker map

extern "C" char* __cdecl fgets(char* s, int n, FILE* f);
extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);
extern "C" unsigned long __cdecl strtoul(const char* s, char** end, int base);
extern "C" int __cdecl abs(int n);
ASM_VAR(DWORD, g_mapBase6735d4, $L_6735d4)          // the code base, FUN_401000 until the map gives it
ASM_VAR(const char*, g_mapNames6735d8, $L_6735d8)   // the three symbol names
ASM_VAR(DWORD, g_mapStart6974d8, $L_6974d8)         // their addresses, [3]
ASM_VAR(DWORD, g_mapEnd6974e4, $L_6974e4)           // the addresses of the symbols after them, [3]
ASM_VAR(char, s_exe6737e4, $L_6737e4)               // "htruck.exe"
ASM_VAR(char, s_exe6737f0, $L_6737f0)               // "htruck2.exe"
ASM_VAR(char, s_map6737fc, $L_6737fc)               // "htruck.map"
ASM_VAR(char, s_map673808, $L_673808)               // "htruck2.map"
ASM_VAR(char, s_rt673814, $L_673814)                // "rt"
ASM_VAR(char, s_map673818, $L_673818)               // "htruck.map"
ASM_VAR(char, s_prefix673824, $L_673824)            // " 0001:00000000       "

extern "C" {

// [FUN_4e35cf] when htruck.exe (or htruck2.exe) and its .map file of the same time are present, reads from the
// map where three functions start and end; once read, does nothing
void __cdecl World_ReadMapSymbols()
{
    DWORD* start = &g_mapStart6974d8;
    DWORD* end = &g_mapEnd6974e4;
    const char** names = &g_mapNames6735d8;
    for (int i = 0; i < 3; i++)
        if (start[i])
            return;
    WIN32_FIND_DATAA exe, map;
    int first = 1;
    HANDLE h = FindFirstFileA(&s_exe6737e4, &exe);
    if (h == INVALID_HANDLE_VALUE) {
        first = 0;
        h = FindFirstFileA(&s_exe6737f0, &exe);
    }
    if (h == INVALID_HANDLE_VALUE)
        return;
    FindClose(h);
    FILE* f = 0;
    h = FindFirstFileA(first ? &s_map6737fc : &s_map673808, &map);
    SYSTEMTIME tm, te;
    if (h != INVALID_HANDLE_VALUE) {
        FindClose(h);
        FileTimeToSystemTime(&map.ftLastWriteTime, &tm);
        FileTimeToSystemTime(&exe.ftLastWriteTime, &te);
        f = fopen(&s_map673818, &s_rt673814);
    }
    if (!f || tm.wYear != te.wYear || tm.wMonth != te.wMonth || tm.wDay != te.wDay
        || (tm.wHour != te.wHour && abs(tm.wHour - te.wHour) > 1 && abs(tm.wMinute - te.wMinute) > 2)
        || abs(tm.wMinute - te.wMinute) > 2) {
        if (g_6974c4) {
            start[0] = 0;
            start[1] = 0;
        }
        return;
    }
    const char* prefix = &s_prefix673824;
    int len[3], cnt[3], alive[3];
    for (int i = 0; i < 3; i++) {
        len[i] = strlen(names[i]);
        cnt[i] = 0;
    }
    int plen = strlen(prefix);
    int found = 0, least = 0, cur = -1;
    char line[200];
    while (fgets(line, 200, f) && (least < 2 || !found)) {
        int pos = 0;
        if (!found && !strncmp(prefix, line, plen)) {
            found = 1;
            int j = plen + 1;
            while (line[j] && line[j] != ' ')
                j++;
            g_mapBase6735d4 = strtoul(line + j, 0, 16);
        }
        if (least >= 2)
            continue;
        for (int k = 0; k < 3; k++)
            alive[k] = 1;
        for (int c = 0; line[c] && c < 30; c++) {
            int m = -1;
            for (int k = 0; k < 3; k++) {
                if (line[c + 21] == names[k][pos] && alive[k] && !cnt[k])
                    m = k;
                else
                    alive[k] = 0;
            }
            if (m == -1 && cur == -1)
                break;
            if (m != -1)
                pos++;
            if ((m != -1 && pos == len[m]) || (cur != -1 && cnt[cur] == 1)) {
                int j = 1;
                while (line[j] && line[j - 1] != ':')
                    j++;
                if (cur == -1 && m != -1 && !cnt[m]) {
                    start[m] = strtoul(line + j, 0, 16);
                    cur = m;
                    cnt[cur]++;
                } else {
                    end[cur] = strtoul(line + j, 0, 16);
                    cnt[cur]++;
                    cur = -1;
                }
                least = 2;
                for (int k = 0; k < 3; k++)
                    if (cnt[k] < least)
                        least = cnt[k];
                break;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        start[i] += g_mapBase6735d4;
        end[i] += g_mapBase6735d4;
    }
    fclose(f);
}

}  // extern "C"

// === four unused hit queries (4e6e6a)
// The world's query runner here (World_4e634f) always answers 0, so they only fill their query.

ASM_VAR(AsmLabel, g_wq7025f0, $L_7025f0)

// query q of kind 0 from p along d: the identity frame, owner a, skip b
static void Wq_Fill(BYTE* q, const Vec3Bits* p, const Vec3Bits* d, int a, int skip)
{
    memcpy(q + 4, (BYTE*)g_wq7025f0, 48);
    memcpy(q + 76, p, 12);
    memcpy(q + 88, d, 12);
    F(int, q, 0) = a;
    F(int, q, 120) = skip;
    F(int, q, 116) = 0;
    F(int, q, 104) = 0;
}

extern "C" {

// [$L_4e6e6a] the hit point and normal (+52, +64 of the query) of a kind 4 query of world w from p along d; 0 none
int __cdecl World_HitPoint(void* w, const Vec3Bits* p, const Vec3Bits* d, Vec3Bits* at, Vec3Bits* n)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    Wq_Fill(q, p, d, 0, 0);
    if (!(World_4e634f() & 1))
        return 0;
    memcpy(at, q + 52, 12);
    memcpy(n, q + 64, 12);
    return 1;
}

// (after it) a kind 4 query q of world w skipping skip; its answer
int __cdecl World_Query4(int skip, void* w, int, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    Wq_Fill(q, p, d, 0, skip);
    int r = World_4e634f();
    return r & 1 ? r : 0;
}

// (after it) World_HitPoint for a kind 6 query owned by a
int __cdecl World_HitPoint6(void* w, int a, const Vec3Bits* p, const Vec3Bits* d, Vec3Bits* at, Vec3Bits* n)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    Wq_Fill(q, p, d, a, 0);
    if (!(World_4e634f() & 1))
        return 0;
    memcpy(at, q + 52, 12);
    memcpy(n, q + 64, 12);
    return 1;
}

// (after it) a kind 6 query q of world w owned by a, skipping skip; 1 on a hit
int __cdecl World_Query6(int skip, void* w, int a, const Vec3Bits* p, const Vec3Bits* d, BYTE* q)
{
    Wq_Fill(q, p, d, a, skip);
    return World_4e634f() & 1 ? 1 : 0;
}

} // extern "C"

// === the start position check report (debug build, unreferenced)

extern "C" void Cp_Next(); KING_ALIAS("_Cp_Next", "@Obj4eb170_Get8@8")
extern "C" void Cp_Get128(); KING_ALIAS("_Cp_Get128", "@Obj_Get128@8")
extern "C" void Cp_Point(); KING_ALIAS("_Cp_Point", "@Body_Point@16")
ASM_VAR(char, s_cp673708, $L_673708)
ASM_VAR(char, s_cp673730, $L_673730)
ASM_VAR(char, s_cp673734, $L_673734)
ASM_VAR(char, s_cp673738, $L_673738)
ASM_VAR(char, s_cp673788, $L_673788)
ASM_VAR(char, s_cp6737a0, $L_6737a0)
ASM_VAR(double, g_cp64b748, $L_64b748)

static const char* Cp_Name(void* o) { return CallThis<const char*>(FUN_4cf5e0, o); }

static Vec3 Cp_Corner(void* door, int k)
{
    Vec3 p;
    CallThis(Cp_Point, door, &p, k);
    return p;
}

extern "C" {

// [$L_4e29a6] checks the position of hit h (+36) in room o along d (40 up and down): when either probe leaves
// the room, writes a report (title, the position, the room left, the door between the two rooms with its corners)
// to out (size n) and returns its length when the position is more than 64b748 off the door's plane, else 0
int __cdecl World_CheckPosition(BYTE* o, BYTE* h, const Vec3* d, const char* title, char* out, int n)
{
    int len = 0;
    Vec3 pos = F(Vec3, h, 36), t;
    Vec3 up, dn;
    up = *Vec3_Add(&up, Vec3_Scaled(&t, d, 40.0f), &pos);
    dn = *Vec3_Add(&dn, Vec3_Scaled(&t, d, -40.0f), &pos);
    void* ha = Query_HitObject(o, (const Vec3Bits*)&up, (const Vec3Bits*)&pos);
    void* hb = Query_HitObject(o, (const Vec3Bits*)&dn, (const Vec3Bits*)&pos);
    BYTE* door = 0;
    if (ha == o && hb == o)
        return len;
    char s256[200], s456[200], s656[220], s860[200];
    sprintf(s256, &s_cp673708, Cp_Name(o), (double)pos.x, (double)pos.y, (double)pos.z);
    if (ha != o)
        sprintf(s456, &s_cp673730, Cp_Name(ha));
    else
        sprintf(s456, &s_cp673734, Cp_Name(hb));
    for (int i = 0; i < CallVirt<int>(o, 60); i++)
        for (BYTE* e = CallVirt<BYTE*>(o, 64, i); e; e = CallThis<BYTE*>(Cp_Next, e)) {
            if (CallThis<int>(FUN_4eb150, e) != 14)
                continue;
            BYTE* g = CallThis<BYTE*>(Cp_Get128, e);
            if (!g)
                continue;
            if (CallThis<int>(FUN_4eb150, g) == 15 && (g == ha || g == hb))
                door = e;
        }
    s656[0] = 0;
    float dist = 0;
    if (door) {
        Vec3 c1 = Cp_Corner(door, 1), c0 = Cp_Corner(door, 0);
        sprintf(s656, &s_cp673738, Cp_Name(o), Cp_Name(CallThis<BYTE*>(Cp_Get128, door)), (double)c0.x, (double)c0.y,
            (double)c0.z, (double)c1.x, (double)c1.y, (double)c1.z);
        Vec3 a = Cp_Corner(door, 0), b = Cp_Corner(door, 1), c = Cp_Corner(door, 2);
        a.z = pos.z;
        b.z = pos.z;
        Vec3 ca, ab, x, nrm, hit;
        Vec3_Sub(&ca, &c, &a);
        Vec3_Sub(&ab, &a, &b);
        Vec3_Normalized(&nrm, Vec3_Cross(&x, &ab, &ca));
        Vec3_PlaneHit(&hit.x, &a.x, &nrm.x, &pos.x, (float*)d);
        dist = (float)Vec3_Length(Vec3_Sub(&t, &hit, &pos));
    }
    const char* fmt = &s_cp673788;
    sprintf(s860, &s_cp6737a0);
    len = (int)(strlen(fmt) + strlen(title) + strlen(s256) + strlen(s860) + strlen(Cp_Name(o)) + strlen(s456)
        + strlen(s656));
    char* buf = (char*)operator new(len + 15);
    sprintf(buf, fmt, title, s256, s860, Cp_Name(o), s456, s656);
    if (len >= n) {
        strncpy(out, buf, n - 1);
        out[n - 1] = 0;
    } else
        strcpy(out, buf);
    operator delete(buf);
    if (dist > g_cp64b748)
        return len;
    return 0;
}

} // extern "C"

// === the stack trace log (debug build, unreferenced)

extern "C" int __cdecl printf(const char* f, ...);
extern "C" int __cdecl fprintf(FILE* f, const char* fmt, ...);
ASM_VAR(char, s_st67383c, $L_67383c)
ASM_VAR(char, s_st673854, $L_673854)
ASM_VAR(char, s_st673858, $L_673858)
ASM_VAR(char, s_st673864, $L_673864)
ASM_VAR(char, s_st673870, $L_673870)
ASM_VAR(char, s_st673878, $L_673878)
ASM_VAR(char, s_st673890, $L_673890)
ASM_VAR(char, s_st6738ac, $L_6738ac)
ASM_VAR(char, s_st6738c4, $L_6738c4)
ASM_VAR(char, s_st6738d8, $L_6738d8)
ASM_VAR(char, s_st673900, $L_673900)
ASM_VAR(char, s_st67391c, $L_67391c)
ASM_VAR(char, s_st673950, $L_673950)
ASM_VAR(char, s_st673970, $L_673970)
ASM_VAR(char, s_st673994, $L_673994)
ASM_VAR(char, s_st6739a8, $L_6739a8)

static void St_Say(FILE* f, const char* s)
{
    if (f)
        fprintf(f, s);
}

extern "C" {

// [$L_4e3cff] writes the call stack (up to 100 frames, walked through EBP) to a log file, with the message msg,
// the game times and, between frames, the stack words; it stops at the code of the map's modules
// (6974d8..6974e4) or at a bad frame; the map symbols are read first when missing
void __cdecl World_LogStack(const char* msg)
{
    BYTE ctx[716];
    memset(ctx, 0, 716);
    F(DWORD, ctx, 0) = 0x10007;
    DWORD* start = &g_mapStart6974d8;
    DWORD* end = &g_mapEnd6974e4;
    for (int i = 0; i < 3; i++)
        if (!start[i]) {
            for (int j = 0; j < 3; j++)
                start[j] = 0;
            World_ReadMapSymbols();
            break;
        }
    for (int i = 0; i < 3; i++)
        if (!start[i])
            return;
    if (!GetThreadContext(GetCurrentThread(), (CONTEXT*)ctx)) {
        printf(&s_st67383c);
        return;
    }
    DWORD& ebp = F(DWORD, ctx, 180);
    DWORD& eip = F(DWORD, ctx, 184);
    DWORD lastEip = eip, lastEbp = ebp;
    FILE* f = fopen(&s_st673858, &s_st673854);
    if (f) {
        fprintf(f, &s_st673864);
        if (msg)
            fprintf(f, &s_st673870, msg);
        fprintf(f, &s_st673878, (double)g_6974bc);
        if (g_6974c4) {
            fprintf(f, &s_st673890, F(DWORD, g_6974c4, 2968), F(DWORD, g_6974c4, 2972));
            fprintf(f, &s_st6738ac, Game_FrameTime());
        }
    }
    for (int k = 0; k < 100; k++) {
        if (f) {
            fprintf(f, &s_st6738c4, eip - g_mapBase6735d4, ebp);
            fflush(f);
        }
        int i = 0;
        for (; i < 3; i++)
            if (start[i] && eip >= start[i] && eip < end[i]) {
                St_Say(f, &s_st6738d8);
                break;
            }
        if (i != 3)
            break;
        if (ebp & 3) {
            St_Say(f, &s_st673900);
            break;
        }
        if (k && ebp <= lastEbp) {
            St_Say(f, &s_st67391c);
            break;
        }
        if (ebp > 0x800000) {
            St_Say(f, &s_st673950);
            break;
        }
        if (ebp < F(DWORD, ctx, 196)) {
            St_Say(f, &s_st673970);
            break;
        }
        if (start[0] && f && k && lastEbp <= ebp && (int)(lastEbp - ebp) >> 2 < 4096)
            for (DWORD p = lastEbp; p <= ebp; p += 4) {
                fprintf(f, &s_st673994, p, *(DWORD*)p, (double)*(float*)p);
                fflush(f);
            }
        lastEip = eip;
        lastEbp = ebp;
        SIZE_T got = 8;
        DWORD frame[2];
        if (!ReadProcessMemory(GetCurrentProcess(), (void*)ebp, frame, 8, &got))
            break;
        eip = frame[1];
        ebp = frame[0];
        if (eip == 1) {
            St_Say(f, &s_st6739a8);
            break;
        }
    }
    if (f)
        fclose(f);
}

} // extern "C"

// === a sound panned by its direction (debug build, unreferenced)

extern "C" void Sp_Restart(); KING_ALIAS("_Sp_Restart", "@Sound_Restart@16")
ASM_VAR(double, g_sp64d940, $L_64d940)
ASM_VAR(double, g_sp64d938, $L_64d938)
ASM_VAR(double, g_sp64d930, $L_64d930)
ASM_VAR(double, g_sp64d928, $L_64d928)
ASM_VAR(double, g_sp64d900, $L_64d900)
ASM_VAR(double, g_sp64b670, $L_64b670)
ASM_VAR(double, g_sp64b660, $L_64b660)
ASM_VAR(double, g_sp64b6e0, $L_64b6e0)
ASM_VAR(float, g_sp64c40c, $L_64c40c)
ASM_VAR(float, g_sp64c250, $L_64c250)
ASM_VAR(float, g_sp64c264, $L_64c264)

static float Sp_Unit(float v)
{
    if (v < g_sp64c264)
        v = 0;
    if (v > g_sp64c250)
        v = 1.0f;
    return v;
}

extern "C" {

// [$L_4e4c0e] plays sound s at volume vol from direction d: the angle folded to the front and mapped to 0..1
// (curved), the left and right volumes from it; a negative mode restarts the sound looping
void __cdecl Sound_PlayPanned(BYTE* s, float vol, const float* d, int mode)
{
    if (!s)
        return;
    float a = (float)-x87_atan2(d[0], d[1]);
    if (a > g_sp64d940)
        a = (float)(g_sp64d938 - a);
    else if (a < g_sp64d930)
        a = (float)(g_sp64d928 - a);
    a = (float)((a / g_sp64d900 + g_sp64b670) / g_sp64b660);
    if (a > g_sp64b6e0)
        a = (float)(x87_sqrt((a - g_sp64b6e0) * 2) * g_sp64b6e0 + g_sp64b6e0);
    else
        a = g_sp64c40c * a * a;
    float l = Sp_Unit(vol * a);
    float r = Sp_Unit((g_sp64c250 - a) * vol);
    if (mode < 0)
        CallThis(Sp_Restart, s, *(DWORD*)&l, *(DWORD*)&r);
    else
        CallThis(FUN_5fd430, s, *(DWORD*)&l, *(DWORD*)&r, mode);
}

} // extern "C"

// === a frame report (debug build, FUN_4e7a19)

ASM_VAR(char, s_fr673d98, $L_673d98)

extern "C" {

// [FUN_4e7a19] formats name, frame m and the vectors a and b; more than n characters gives n, else 0 (the text
// is not stored: the original copies out onto itself)
int __cdecl Frame_Report(char* out, int n, const char* name, const float* m, const float* a, const float* b)
{
    char buf[1000];
    sprintf(buf, &s_fr673d98, name, (double)m[0], (double)m[1], (double)m[2], (double)m[3], (double)m[4],
        (double)m[5], (double)m[6], (double)m[7], (double)m[8], (double)m[9], (double)m[10], (double)m[11],
        (double)a[0], (double)a[1], (double)a[2], (double)b[0], (double)b[1], (double)b[2]);
    int len = (int)strlen(buf);
    if (len > n)
        return n;
    memmove(out, out, strlen(out) + 1);
    return 0;
}

} // extern "C"

// === a filtered value of timed samples (debug build, FUN_4eaf98)

ASM_VAR(float, g_fv64c938, $L_64c938)
ASM_VAR(double, g_fv64b658, $L_64b658)
ASM_VAR(float, g_fv64c250, $L_64c250)

extern "C" {

// [FUN_4eaf98] the weighted mean of filter s's samples (count +28, times +8, values +12) by the window +20 of their
// age before now (scaled by +4, within 64c938..64b658), blended toward +16 by the optional +24; 0 without samples
float __fastcall Filter_Value(BYTE* s)
{
    float acc = 0, wsum = 0, b = 1.0f;
    int cnt = 0;
    typedef double (__cdecl* Fn)(float);
    for (int i = 0; i < F(int, s, 28); i++) {
        float now = (float)F(double, g_6974c4, 2968);
        float x = (F(float*, s, 8)[i] - now) * F(float, s, 4);
        if (!(x > g_fv64c938) || x > g_fv64b658)
            continue;
        float w = (float)F(Fn, s, 20)(x);
        if (F(Fn, s, 24)) {
            b = (float)F(Fn, s, 24)(x);
            acc = (float)((((double)g_fv64c250 - b) * F(float, s, 16) + (double)b * F(float*, s, 12)[i]) * w + acc);
        } else
            acc = w * F(float*, s, 12)[i] + acc;
        wsum = wsum + w;
        cnt++;
    }
    return cnt ? acc / wsum : 0;
}

} // extern "C"

// === vehicle log writers ($L_4e9e97; only the first has a label, the others are unreferenced in the game)

ASM_VAR(char, s_vl674138, $L_674138)
ASM_VAR(char, s_vl67413c, $L_67413c)
ASM_VAR(char, s_vl674144, $L_674144)
ASM_VAR(char, s_vl67414c, $L_67414c)
ASM_VAR(char, s_vl674154, $L_674154)
ASM_VAR(char, s_vl67415c, $L_67415c)
ASM_VAR(char, s_vl674168, $L_674168)
ASM_VAR(char, s_vl674178, $L_674178)
ASM_VAR(char, s_vl674180, $L_674180)
ASM_VAR(char, s_vl67418c, $L_67418c)
ASM_VAR(char, s_vl674198, $L_674198)
ASM_VAR(char, s_vl6741a4, $L_6741a4)
ASM_VAR(char, s_vl6741b0, $L_6741b0)
ASM_VAR(char, s_vl6741bc, $L_6741bc)
ASM_VAR(char, s_vl6741c8, $L_6741c8)
ASM_VAR(char, s_vl6741d4, $L_6741d4)

// the three floats at v with format fmt to log i
static void Vl_Three(int i, const char* fmt, const float* v)
{
    fprintf(g_vehLogs[i], fmt, (double)v[0], (double)v[1], (double)v[2]);
}

// the rows of the 4x3 matrix m with the four formats
static void Vl_Rows(int i, const char* f0, const char* f1, const char* f2, const char* f3, const float* m)
{
    Vl_Three(i, f0, m);
    Vl_Three(i, f1, m + 3);
    Vl_Three(i, f2, m + 6);
    Vl_Three(i, f3, m + 9);
}

extern "C" {

// [$L_4e9e97] writes the number v to vehicle log i when it is open
void __cdecl VehLog_Int(int i, int v)
{
    if (VehLog_Open(i))
        fprintf(g_vehLogs[i], &s_vl674138, v);
}

// the rest of the same block, never called
void __cdecl VehLog_Int2(int i, int v)
{
    if (VehLog_Open(i))
        fprintf(g_vehLogs[i], &s_vl67413c, v);
}

void __cdecl VehLog_Pair(int i, int a, int b)
{
    if (VehLog_Open(i))
        fprintf(g_vehLogs[i], &s_vl674144, a, b);
}

void __cdecl VehLog_Float(int i, float f)
{
    if (VehLog_Open(i))
        fprintf(g_vehLogs[i], &s_vl67414c, (double)f);
}

void __cdecl VehLog_NamedFloat(int i, int a, float f)
{
    if (VehLog_Open(i))
        fprintf(g_vehLogs[i], &s_vl674154, a, (double)f);
}

void __cdecl VehLog_Vector(int i, const float* v)
{
    if (VehLog_Open(i))
        Vl_Three(i, &s_vl67415c, v);
}

void __cdecl VehLog_NamedVector(int i, int a, const float* v)
{
    if (VehLog_Open(i))
        fprintf(g_vehLogs[i], &s_vl674168, a, (double)v[0], (double)v[1], (double)v[2]);
}

void __cdecl VehLog_NamedMatrix(int i, int a, const float* m)
{
    if (!VehLog_Open(i))
        return;
    fprintf(g_vehLogs[i], &s_vl674178, a);
    Vl_Rows(i, &s_vl674180, &s_vl67418c, &s_vl674198, &s_vl6741a4, m);
}

void __cdecl VehLog_Matrix(int i, const float* m)
{
    if (VehLog_Open(i))
        Vl_Rows(i, &s_vl6741b0, &s_vl6741bc, &s_vl6741c8, &s_vl6741d4, m);
}

} // extern "C"

// === warning log writers, the debug socket and filter windows ($L_4ea365; the writers and the socket are
// unreferenced in the game, the windows at $L_4eaa41.. are used by the filters)

ASM_FUNC(Ws_closesocket, FUN_6300b0)
ASM_FUNC(Ws_connect, FUN_6300b6)
ASM_FUNC(Ws_htons, FUN_6300bc)
ASM_FUNC(Ws_inet_addr, FUN_6300c2)
ASM_FUNC(Ws_socket, FUN_6300c8)
ASM_FUNC(Ws_WSAGetLastError, FUN_6300ce)
ASM_FUNC(Ws_WSAStartup, FUN_6300d4)
ASM_FUNC(Ws_send, FUN_6300da)
extern "C" int __cdecl wsprintfA(char*, const char*, ...);
extern "C" void Wl_DumpText(); KING_ALIAS("_Wl_DumpText", "_Dump_Text")
ASM_VAR(char, s_wl6741f0, $L_6741f0)
ASM_VAR(char, s_wl6741f8, $L_6741f8)
ASM_VAR(char, s_wl674200, $L_674200)
ASM_VAR(char, s_wl67420c, $L_67420c)
ASM_VAR(char, s_wl674214, $L_674214)
ASM_VAR(char, s_wl67421c, $L_67421c)
ASM_VAR(char, s_wl674224, $L_674224)
ASM_VAR(char, s_wl674230, $L_674230)
ASM_VAR(char, s_wl674240, $L_674240)
ASM_VAR(char, s_wl674248, $L_674248)
ASM_VAR(char, s_wl674254, $L_674254)
ASM_VAR(char, s_wl674260, $L_674260)
ASM_VAR(char, s_wl67426c, $L_67426c)
ASM_VAR(char, s_wl674278, $L_674278)
ASM_VAR(char, s_wl674284, $L_674284)
ASM_VAR(char, s_wl674290, $L_674290)
ASM_VAR(char, s_wl67429c, $L_67429c)
ASM_VAR(char, s_wl6742a8, $L_6742a8)
ASM_VAR(char, s_wl6742c4, $L_6742c4)
ASM_VAR(char, s_wl6742dc, $L_6742dc)
ASM_VAR(char, s_wl6742ec, $L_6742ec)
ASM_VAR(char, s_wl674308, $L_674308)
ASM_VAR(WORD, g_wl696e48, $L_696e48)    // a sockaddr_in: family, port +2, address +4
ASM_VAR(int, g_wl696e58, $L_696e58)     // the socket
ASM_VAR(double, g_wl64b658, $L_64b658)
ASM_VAR(float, g_wl64c938, $L_64c938)
ASM_VAR(float, g_wl64c264, $L_64c264)
ASM_VAR(float, g_wl64c250, $L_64c250)
ASM_VAR(double, g_wl64b670, $L_64b670)
ASM_VAR(double, g_wl64d938, $L_64d938)
ASM_VAR(double, g_wl64b6e0, $L_64b6e0)
extern "C" double __cdecl Wl_sqrt(double); KING_ALIAS("_Wl_sqrt", "_sqrt")
extern "C" double __cdecl Wl_fabs(double); KING_ALIAS("_Wl_fabs", "_fabs")
extern "C" double __cdecl Wl_cos(double); KING_ALIAS("_Wl_cos", "_cos")

// whether the warning log takes a line now (locking it when that is asked for)
static bool Wl_Begin()
{
    if (g_wd6974d4)
        CallC(FUN_4e9ce4);
    return g_wd6974d0 && g_wd6975a8;
}

static void Wl_End()
{
    if (g_wd6974d4)
        CallC(FUN_4e9d90);
}

static void Wl_Three(const char* fmt, const float* v)
{
    fprintf(g_wd6975a8, fmt, (double)v[0], (double)v[1], (double)v[2]);
}

static void Wl_Rows(const char* f0, const char* f1, const char* f2, const char* f3, const float* m)
{
    Wl_Three(f0, m);
    Wl_Three(f1, m + 3);
    Wl_Three(f2, m + 6);
    Wl_Three(f3, m + 9);
}

// whether x lies in the filter window (64c938, 64b658]
static bool Wl_In(float x)
{
    return !(x > g_wl64b658) && x > g_wl64c938;
}

extern "C" {

// [$L_4ea365] writes two numbers to the warning log
void __cdecl WarnLog_Pair(int a, int b)
{
    if (!Wl_Begin())
        return;
    fprintf(g_wd6975a8, &s_wl6741f0, a, b);
    Wl_End();
}

void __cdecl WarnLog_Int(int a)
{
    if (!Wl_Begin())
        return;
    fprintf(g_wd6975a8, &s_wl6741f8, a);
    Wl_End();
}

void __cdecl WarnLog_Pair2(int a, int b)
{
    if (!Wl_Begin())
        return;
    fprintf(g_wd6975a8, &s_wl674200, a, b);
    Wl_End();
}

void __cdecl WarnLog_Float(float f)
{
    if (!Wl_Begin())
        return;
    fprintf(g_wd6975a8, &s_wl67420c, (double)f);
    Wl_End();
}

void __cdecl WarnLog_NamedFloat(int a, float f)
{
    if (!Wl_Begin())
        return;
    fprintf(g_wd6975a8, &s_wl674214, a, (double)f);
    Wl_End();
}

void __cdecl WarnLog_Triple(int a, int b, int c)
{
    if (!Wl_Begin())
        return;
    fprintf(g_wd6975a8, &s_wl67421c, a, b, c);
    Wl_End();
}

void __cdecl WarnLog_Vector(const float* v)
{
    if (!Wl_Begin())
        return;
    Wl_Three(&s_wl674224, v);
    Wl_End();
}

void __cdecl WarnLog_NamedVector(int a, const float* v)
{
    if (!Wl_Begin())
        return;
    fprintf(g_wd6975a8, &s_wl674230, a, (double)v[0], (double)v[1], (double)v[2]);
    Wl_End();
}

void __cdecl WarnLog_NamedMatrix(int a, const float* m)
{
    if (!Wl_Begin())
        return;
    fprintf(g_wd6975a8, &s_wl674240, a);
    Wl_Rows(&s_wl674248, &s_wl674254, &s_wl674260, &s_wl67426c, m);
    Wl_End();
}

void __cdecl WarnLog_Matrix(const float* m)
{
    if (!Wl_Begin())
        return;
    Wl_Rows(&s_wl674278, &s_wl674284, &s_wl674290, &s_wl67429c, m);
    Wl_End();
}

// opens the debug socket to 6742dc port 5000: 0, or -1 with the error formatted (and shown when connecting fails)
int __cdecl DebugSocket_Open()
{
    BYTE data[400];
    char text[300];
    if (CallStd<int>(Ws_WSAStartup, 257, data)) {
        wsprintfA(text, &s_wl6742a8, CallStd<int>(Ws_WSAGetLastError));
        return -1;
    }
    g_wl696e58 = CallStd<int>(Ws_socket, 2, 1, 0);
    if (g_wl696e58 == -1) {
        wsprintfA(text, &s_wl6742c4, CallStd<int>(Ws_WSAGetLastError));
        return -1;
    }
    (&g_wl696e48)[0] = 2;
    *(int*)(&g_wl696e48 + 2) = CallStd<int>(Ws_inet_addr, &s_wl6742dc);
    (&g_wl696e48)[1] = CallStd<WORD>(Ws_htons, 5000);
    if (CallStd<int>(Ws_connect, g_wl696e58, &g_wl696e48, 16) >= 0)
        return 0;
    int e = CallStd<int>(Ws_WSAGetLastError);
    wsprintfA(text, &s_wl6742ec, e, e);
    CallStd(Ws_closesocket, g_wl696e58);
    CallC(Wl_DumpText, text);
    return -1;
}

// sends the string s (with its terminator) on the debug socket: 0, or -1 with the error shown
int __cdecl DebugSocket_Send(int unused, const char* s)
{
    char text[400];
    int n = (int)strlen(s) + 1;
    if (CallStd<int>(Ws_send, g_wl696e58, s, n, 0))
        return 0;
    wsprintfA(text, &s_wl674308, CallStd<int>(Ws_WSAGetLastError));
    CallC(Wl_DumpText, text);
    return -1;
}

// filter windows over (64c938, 64b658]: a box
double __cdecl Window_Box(float x)
{
    return Wl_In(x) ? g_wl64c250 : g_wl64c264;
}

// ($L_4eaa41) a ramp 1 + x
double __cdecl Window_Ramp(float x)
{
    return Wl_In(x) ? (double)g_wl64c250 + x : g_wl64c264;
}

// ($L_4eaa77) 1 - x * x
double __cdecl Window_Parabola(float x)
{
    return Wl_In(x) ? g_wl64c250 - (double)x * x : g_wl64c264;
}

// ($L_4eaab0) 1 - sqrt |x|
double __cdecl Window_Root(float x)
{
    return Wl_In(x) ? g_wl64b670 - Wl_sqrt(Wl_fabs(x)) : g_wl64c264;
}

// ($L_4eab02) (cos(x * 64d938) + 1) / 2
double __cdecl Window_Cosine(float x)
{
    return Wl_In(x) ? (Wl_cos(x * g_wl64d938) + g_wl64b670) * g_wl64b6e0 : g_wl64c264;
}

} // extern "C"

// === the date and time text (FUN_4e4468)

ASM_VAR(AsmLabel, s_dt6739e4, $L_6739e4)          // the month names, 4 bytes each
ASM_VAR(char, s_dt673a14, $L_673a14)

extern "C" {

// [FUN_4e4468] the local date and time into out: day, month name, year, hours, minutes, seconds
void __cdecl Clock_Text(char* out)
{
    SYSTEMTIME t;
    GetLocalTime(&t);
    char month[12];
    if (t.wMonth >= 1 && t.wMonth <= 12)
        strcpy(month, (const char*)&s_dt6739e4 + (t.wMonth - 1) * 4);
    sprintf(out, &s_dt673a14, (int)t.wDay, month, (int)t.wYear, (int)t.wHour, (int)t.wMinute, (int)t.wSecond);
}

} // extern "C"

// === a vector out of a box (FUN_4e28b0)

extern "C" {

// [FUN_4e28b0] 1 when a component of v lies outside [-lim, lim]
int __cdecl Vec3_OutOfBox(const float* v, float lim)
{
    float a = v[0], b = v[1], c = v[2];
    double nl = -(double)lim;
    if (a > lim || b > lim || c > lim || lim < a || lim < b || lim < c)
        return 1;
    if (-(double)a > lim || -(double)b > lim || -(double)c > lim)
        return 1;
    if (nl > a || nl > b || nl > c)
        return 1;
    return 0;
}

} // extern "C"

// === softening a long vector (FUN_4e794f)

extern "C" void Sv_Scaled(); KING_ALIAS("_Sv_Scaled", "_Vec3_Scaled")
extern "C" void Sv_DivScalar(); KING_ALIAS("_Sv_DivScalar", "_Vec3_DivScalar")
extern "C" void Sv_Length(); KING_ALIAS("_Sv_Length", "_Vec3_Length")

extern "C" {

// [FUN_4e794f] v longer than 5 is shortened smoothly: from 5 to 35 to (L - 5) / 2 + 5, beyond to 20
void __cdecl Vec3_Soften(float* v)
{
    float L = (float)CallC<double>(Sv_Length, v);
    const float hi = 35.0f, lo = 5.0f, half = 0.5f;
    float mid = (float)(((double)hi - lo) * half + lo);
    float t[3], u[3];
    if (L > hi)
    {
        const float* r = CallC<const float*>(Sv_Scaled, t, v, (float)(mid / (double)L));
        v[0] = r[0];
        v[1] = r[1];
        v[2] = r[2];
    }
    else if (L > lo)
    {
        float k = (float)(((double)L - lo) * half + lo);
        const float* r = CallC<const float*>(Sv_DivScalar, u, CallC<const float*>(Sv_Scaled, t, v, k), L);
        v[0] = r[0];
        v[1] = r[1];
        v[2] = r[2];
    }
}

} // extern "C"

// === the cars' histories dumped to the log (FUN_53f0c0)

extern "C" void __fastcall Car_History(BYTE* self, void*, int back, int* a, int* b, void* m1, void* v1, void* m2,
                                       int* c, char* name, void* v2, void* v3, int* d);
extern "C" void __cdecl SceneObjs_Dump();
extern "C" void __cdecl VehLog_Value2(int v);
ASM_VAR(BYTE*, g_hd696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_hd6d2078, $L_6d2078)
ASM_VAR(BYTE*, g_hd6ef1f8, $L_6ef1f8)                   // the car list
ASM_VAR(char, g_hd6795e0, $L_6795e0)
ASM_VAR(char, g_hd6794a4, $L_6794a4)
ASM_VAR(char, g_hd67948c, $L_67948c)
ASM_VAR(char, g_hd6795cc, $L_6795cc)
ASM_VAR(char, g_hd679464, $L_679464)
ASM_VAR(char, g_hd6795b4, $L_6795b4)
ASM_VAR(char, g_hd679458, $L_679458)
ASM_VAR(char, g_hd679440, $L_679440)
ASM_VAR(char, g_hd679590, $L_679590)
ASM_VAR(char, g_hd67941c, $L_67941c)
ASM_VAR(char, g_hd679404, $L_679404)
ASM_VAR(char, g_hd679578, $L_679578)
ASM_VAR(char, g_hd679434, $L_679434)
ASM_VAR(char, g_hd6793e4, $L_6793e4)
ASM_VAR(char, g_hd679558, $L_679558)
ASM_VAR(char, g_hd6793a0, $L_6793a0)
ASM_VAR(char, g_hd679380, $L_679380)
ASM_VAR(char, g_hd679360, $L_679360)
ASM_VAR(char, g_hd679340, $L_679340)
ASM_VAR(char, g_hd679328, $L_679328)

static void Hd_Out(const char* t)
{
    VehLog_Value2((int)t);
}

static void Hd_Vec(const char* fmt, const float* v)
{
    char buf[256];
    sprintf(buf, fmt, (double)v[0], (double)v[1], (double)v[2]);
    Hd_Out(buf);
}

// a matrix's rows, the first with its own heading
static void Hd_Matrix(const float* m, const char* first)
{
    Hd_Vec(first, m);
    for (int r = 1; r < 4; r++)
        Hd_Vec(&g_hd67948c, m + 3 * r);
}

extern "C" {

// [FUN_53f0c0] the clock, the camera's frame and the last 50 records of every car's history (frames, speeds,
// spins, state) written to the warning log, then the scene objects
void __cdecl Cars_DumpHistory()
{
    if (!g_hd696ca0 || !g_hd6d2078)
        return;
    Matrix A = g_identity, B = g_identity;
    float v328[3] = {0, 0, 0}, v316[3] = {0, 0, 0}, v340[3] = {0, 0, 0};
    char buf[256], name[40];
    WarnLog_Open();
    BYTE* g = g_hd696ca0;
    sprintf(buf, &g_hd6795e0, F(int, g, 2968), F(int, g, 2972), F(int, g, 2992));
    Hd_Out(buf);
    A = F(Matrix, g_hd6d2078, 1424);
    Hd_Matrix(A.m, &g_hd6794a4);
    static const char* const states[6] = {&g_hd6793e4, &g_hd679558, &g_hd6793a0, &g_hd679380, &g_hd679360,
                                          &g_hd679340};
    for (BYTE* c = g_hd6ef1f8 ? g_hd6ef1f8 - 4 : 0; c; c = F(BYTE*, c, 8) ? F(BYTE*, c, 8) - 4 : 0)
    {
        sprintf(buf, &g_hd6795cc, F(int, F(BYTE*, c, 336), 12), c);
        Hd_Out(buf);
        if (F(int, c, 1220))
            Hd_Out(&g_hd679464);
        for (int k = 0; k < 50; k++)
        {
            int id, state;
            float speed, spin;
            Car_History(c, 0, k, &id, (int*)&speed, &A, v328, &B, &state, name, v316, v340, (int*)&spin);
            sprintf(buf, &g_hd6795b4, k, id, (double)speed);
            Hd_Out(buf);
            sprintf(buf, &g_hd679458, name);
            Hd_Out(buf);
            Hd_Matrix(A.m, &g_hd679440);
            A = B;
            Hd_Matrix(A.m, &g_hd679590);
            Hd_Vec(&g_hd67941c, v316);
            Hd_Vec(&g_hd679404, v340);
            Hd_Vec(&g_hd679578, v328);
            sprintf(buf, &g_hd679434, (double)spin);
            Hd_Out(buf);
            Hd_Out((unsigned)state <= 5 ? states[state] : &g_hd679328);
        }
    }
    WarnLog_Close();
    SceneObjs_Dump();
}

} // extern "C"

// === a value recorded in a timed history (FUN_4eacce with its tail $L_4eaf8f)

ASM_VAR(BYTE*, g_hr6974c4, $L_6974c4)
ASM_VAR(char, g_hrFmt, $L_674330)
ASM_VAR(char, g_hrEven, $L_67437c)
ASM_VAR(char, g_hrOdd, $L_674390)
ASM_VAR(char, g_hrLast, $L_6743a4)

extern "C" {

// [FUN_4eacce..$L_4eaf8f] history h (span +0, times +8, values +12, count +28): v stored at the slot of now
// (the world's time 6974c4 +2968), else the first slot older than the span, else the oldest; fatal with the
// whole history (named name) when no slot is older
void __fastcall History_Record(BYTE* h, void*, DWORD v, const char* name)
{
    float now = (float)F(double, g_hr6974c4, 2968);
    float old = (float)((double)now - F(float, h, 0));
    float best = now;
    int bi = -1;
    float* times = F(float*, h, 8);
    DWORD* vals = F(DWORD*, h, 12);
    int n = F(int, h, 28);
    for (int i = 0; i < n; i++)
    {
        if (now == times[i])
        {
            vals[i] = v;
            return;
        }
        if (old > times[i])
        {
            times[i] = now;
            vals[i] = v;
            return;
        }
        if (best > times[i])
        {
            best = times[i];
            bi = i;
        }
    }
    if (bi != -1)
    {
        times[bi] = now;
        vals[bi] = v;
        return;
    }
    char* buf = (char*)operator new(1200);
    sprintf(buf, &g_hrFmt, (double)*(float*)&v, name, n, (double)F(float, h, 0), F(double, g_hr6974c4, 2960),
            (double)now, Game_Time());
    float* fv = (float*)vals;
    for (int i = 0; i < n / 2; i++)
    {
        sprintf(buf + strlen(buf), &g_hrEven, 2 * i, (double)times[2 * i], (double)fv[2 * i]);
        sprintf(buf + strlen(buf), &g_hrOdd, 2 * i + 1, (double)times[2 * i + 1], (double)fv[2 * i + 1]);
    }
    if (n & 1)
        sprintf(buf + strlen(buf), &g_hrLast, n - 1, (double)times[n - 1], (double)fv[n - 1]);
    World_Fatal(buf);
}

} // extern "C"
