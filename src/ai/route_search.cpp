// route_search.cpp - ai_trce.cpp of the AI, rewritten from king.masm: routes on
// the road graph (route tables, route costs, the checks and debug dumps of
// hop sequences), the AI's double precision geometry (cross product, 4x3
// transforms, bounding boxes), the CRC32 the AI uses to check files, and the
// smoothing filters of its controls. The route search itself and the unit's
// static initializers are still assembly.
//
// Building with  nmake ASM_ROUTE_SEARCH=1  makes king.masm assemble the original
// procedures again.

#include <fcntl.h>
#include <string.h>
#include "road.h"

// ---------------------------------------------------------------------------
// Double precision geometry

struct DMatrix {
    double m[4][3];
};

struct DBox {
    DVec3 min, max;

    DBox* Init(const DVec3* a, const DVec3* b);
    DBox* Extend(const DVec3* p);
};

extern "C" DVec3* __cdecl DVec3_Cross(DVec3* out, const DVec3* a, const DVec3* b)
{
    double ax = a->x, ay = a->y, az = a->z;
    double bx = b->x, by = b->y, bz = b->z;
    out->x = ay * bz - az * by;
    out->y = az * bx - ax * bz;
    out->z = ax * by - ay * bx;
    return out;
}

// out = a * b, the translation of b added to the last row
extern "C" DMatrix* __cdecl DMatrix_Multiply(DMatrix* out, const DMatrix* a, const DMatrix* b)
{
    DMatrix r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 3; j++)
            r.m[i][j] = (a->m[i][2] * b->m[2][j] + a->m[i][0] * b->m[0][j]) + a->m[i][1] * b->m[1][j];
    r.m[3][0] = r.m[3][0] + b->m[3][0];
    r.m[3][1] = r.m[3][1] + b->m[3][1];
    r.m[3][2] = r.m[3][2] + b->m[3][2];
    *out = r;
    return out;
}

// the box of two corners in any order
DBox* DBox::Init(const DVec3* a, const DVec3* b)
{
    min = *a;
    max = *b;
    if (!(b->x >= a->x)) { min.x = b->x; max.x = a->x; }
    if (!(b->y >= a->y)) { min.y = b->y; max.y = a->y; }
    if (!(b->z >= a->z)) { min.z = b->z; max.z = a->z; }
    return this;
}

DBox* DBox::Extend(const DVec3* p)
{
    if (!(p->x >= min.x)) min.x = p->x;
    if (!(p->y >= min.y)) min.y = p->y;
    if (!(p->z >= min.z)) min.z = p->z;
    if (p->x > max.x) max.x = p->x;
    if (p->y > max.y) max.y = p->y;
    if (p->z > max.z) max.z = p->z;
    return this;
}

// ---------------------------------------------------------------------------
// CRC32 (polynomial 0x04C11DB7, most significant bit first)

typedef unsigned CrcTable[256];
ASM_VAR(CrcTable, g_crcTable, $L_6918c0)

extern "C" void __cdecl Crc_InitTable()
{
    for (unsigned i = 0; i < 256; i++) {
        unsigned c = i << 24;
        for (int k = 0; k < 8; k++)
            c = (c & 0x80000000) ? (c + c) ^ 0x04C11DB7 : c << 1;
        g_crcTable[i] = c;
    }
}

extern "C" void __cdecl Crc_Update(unsigned* crc, const unsigned char* data, int len)
{
    unsigned c = *crc;
    if (g_crcTable[1] == 0)
        Crc_InitTable();
    for (; len > 0; len--)
        c = (c << 8) ^ g_crcTable[(c >> 24) ^ *data++];
    *crc = c;
}

// Adds the file's length to *size and its bytes to *crc, then inverts *crc.
// Returns 0, 1 if the file doesn't open, 2 without memory.
extern "C" int __cdecl Crc_File(unsigned* crc, unsigned* size, const char* path)
{
    unsigned char* buf = (unsigned char*)malloc(16384);
    if (!buf)
        return 2;
    int fd = _open(path, _O_RDONLY | _O_BINARY);
    if (fd == -1)
        return 1;                       // the buffer leaks, as in the game
    for (int n; (n = _read(fd, buf, 16384)) != 0; ) {
        *size += n;
        Crc_Update(crc, buf, n);
    }
    *crc = ~*crc;
    _close(fd);
    free(buf);
    return 0;
}

// ---------------------------------------------------------------------------
// Filters

// first order: value = keep * value + gain * x
struct Filter1 {
    double gain, keep, value;

    Filter1* Construct();
    void SetLength(int n);
    void Reset() { value = 0; }
    double Update(double x);
    void Set(double v) { value = v; }
};

Filter1* Filter1::Construct()
{
    gain = 1.0;
    keep = 0;
    Reset();
    return this;
}

// averages over about n steps; n <= 0 passes the input through
void Filter1::SetLength(int n)
{
    if (n <= 0) {
        gain = 1.0;
        keep = 0;
        return;
    }
    double g = 1.0 / n;
    gain = g;
    keep = 1.0 - g;
}

double Filter1::Update(double x)
{
    value = keep * value + x * gain;
    return value;
}

// second order: y = c0 x + c1 y1 + c2 y2
struct Filter2 {
    double c0, c1, c2, y1, y2;

    Filter2* Construct();
    void Reset() { y1 = 0; y2 = 0; }
    void SetPoles(double re, double im);
    void SetResponse(double t);
    double Update(double x);
    void Set(double v) { y2 = v; y1 = v; }
};

Filter2* Filter2::Construct()
{
    c0 = 1.0;
    c1 = 0;
    c2 = 0;
    Reset();
    return this;
}

// poles re +- i im, unit gain at zero frequency
void Filter2::SetPoles(double re, double im)
{
    double twoRe = re + re;
    c1 = twoRe;
    double c = re * -re - (-im) * (-im);
    c2 = c;
    c0 = (1.0 - twoRe) - c;
}

// a third order Butterworth-like pair of poles for the response time t
void Filter2::SetResponse(double t)
{
    double u = t * -1.5 * 0.7353265585246408;
    double v = t * 0.866 * 0.7353265585246408;
    double a = 1.0 - u;
    double nv = -v;
    double b = u + 1.0;
    double d = nv * nv + a * a;
    double re = (v * nv + b * a) / d;
    double im = (a * v - nv * b) / d;
    SetPoles(re, im);
}

double Filter2::Update(double x)
{
    double old = y2;
    y2 = y1;
    double r = (y1 * c1 + old * c2) + x * c0;
    y1 = r;
    return r;
}


// Clears the route tables of one side: a fresh table for every node that
// has one, and no cost on any edge.
extern "C" void __cdecl Road_ResetSide(int side)
{
    for (RoadNode** n = g_roadNodes; n != g_roadNodesEnd; n++) {
        if ((*n)->routeTable[side])
            operator delete((*n)->routeTable[side]);
        (*n)->routeTable[side] = 0;
        if (!((*n)->flags & 8)) {
            unsigned count = RoadNodeCount();
            (*n)->routeTable[side] = (BYTE*)operator new(count);
            memset((*n)->routeTable[side], 0xff, count);
        }
    }
    RoadCost none;
    memset(&none, 0, sizeof none);
    for (RoadEdge** e = g_roadEdges; e != g_roadEdgesEnd; e++) {
        (*e)->cost[0][side] = none;
        (*e)->cost[1][side] = none;
    }
}

extern "C" void __cdecl Road_ResetSides()
{
    for (int side = 0; side < 2; side++)
        Road_ResetSide(side);
}

extern "C" void __cdecl Road_ResetCosts()
{
    unsigned count = RoadNodeCount();
    if (count) {
        DWORD* d = (DWORD*)g_nodeCosts;
        d[0] = 0xffffffff;          // DBL_MAX
        d[1] = 0x7fefffff;
        for (unsigned i = 2; i < count * 2; i++)
            d[i] = d[i - 2];
    }
    Road_ResetSides();
}

// The sharpest turn of a route: the smallest cosine between the directions
// where one hop leaves an edge and the next enters the next one; 1 if none.
extern "C" double __cdecl Road_MinTurnCos(const HopVec* hops)
{
    double minCos = 1.0;
    const Hop* a = hops->begin;
    if (a == hops->end)
        return minCos;
    for (const Hop* b = a + 1; b != hops->end; a = b, b++) {
        if (!a->edge || !b->edge)
            break;
        PositionId pos;
        DVec3 t;
        double ax, ay, az, bx, by, bz;
        if (a->dir & 1) {
            Road_Tangent(&t, *Road_EdgeStart(&pos, a->edge));
            ax = -t.x; ay = -t.y; az = -t.z;
        } else {
            Road_Tangent(&t, *Road_EdgeEnd(&pos, a->edge));
            ax = t.x; ay = t.y; az = t.z;
        }
        if (b->dir & 1) {
            Road_Tangent(&t, *Road_EdgeEnd(&pos, b->edge));
            bx = -t.x; by = -t.y; bz = -t.z;
        } else {
            Road_Tangent(&t, *Road_EdgeStart(&pos, b->edge));
            bx = t.x; by = t.y; bz = t.z;
        }
        double inv = 1.0 / x87_sqrt((ay * ay + az * az) + ax * ax);
        ax = inv * ax;
        ay = ay * inv;
        az = az * inv;
        double invb = 1.0 / x87_sqrt((by * by + bz * bz) + bx * bx);
        double c = (bz * invb * az + by * invb * ay) + bx * invb * ax;
        if (minCos > c)
            minCos = c;
    }
    return minCos;
}

// Debug dumps of a route
extern "C" void __cdecl Road_PrintEdge(const RoadEdge* e, FILE* f)
{
    if (!e) {
        fprintf(f, "(NULL)");
        fprintf(f, "\n");
        return;
    }
    fprintf(f, " 0x%x ", e->objects[0]->id);
    fprintf(f, "( %s <- %s -> %s )", Str(&e->from->name), Str(&e->name), Str(&e->to->name));
    fprintf(f, "\n");
}

extern "C" void __cdecl Road_PrintHops(const HopVec* hops, FILE* f)
{
    for (const Hop* h = hops->begin; h != hops->end; h++) {
        fprintf(f, "%s %x ", Str(&g_roadNodes[h->node]->name), (int)h->dir);
        Road_PrintEdge(h->edge, f);
    }
}

extern "C" void __cdecl Road_DumpHops(const HopVec* hops, const char* path)
{
    FILE* f = fopen(path, "wt");
    if (f) {
        Road_PrintHops(hops, f);
        fclose(f);
    }
}

static const char s_trceId[] = "$Id: ai_trce.cpp 2001/5/23 19:48:54";
static const char s_trceFile[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\ai_trce.cpp";

// Sets the direction bit of every hop from how its edge joins the next
// node; reports a route whose hops don't connect.
extern "C" void __cdecl Road_OrientHops(HopVec* hops)
{
    for (Hop* h = hops->begin; h != hops->end; h++)
        if (h->edge)
            h->edge->flags |= 0x400;
    for (Hop* h = hops->begin; h != hops->end; h++)
        if (h->edge)
            h->edge->flags &= ~0x400;
    for (Hop* h = hops->begin; h != hops->end; ) {
        RoadEdge* e = h->edge;
        if (!e)
            break;
        Hop* next = h + 1;              // read even at the end, as in the game
        int node = h->node, nextNode = next->node;
        if (e == next->edge)
            Road_DumpHops(hops, "succroads.txt");
        if (e->from == g_roadNodes[node]) {
            if (e->to != g_roadNodes[nextNode]) {
                Road_DumpHops(hops, "ilhops.txt");
                AiError(0, 100, s_trceFile, s_trceId, 272, "Incorrect hops sequence 10", &g_aiEmpty, &g_aiEmpty);
            }
            *(BYTE*)&h->dir &= ~1;
        } else if (e->from == g_roadNodes[nextNode]) {
            if (e->to != g_roadNodes[node]) {
                Road_DumpHops(hops, "ilhops.txt");
                AiError(0, 100, s_trceFile, s_trceId, 282, "Incorrect hops sequence 11", &g_aiEmpty, &g_aiEmpty);
            }
            *(BYTE*)&h->dir |= 1;
        } else {
            Road_DumpHops(hops, "ilhops.txt");
            AiError(0, 100, s_trceFile, s_trceId, 289, "Incorrect hops sequence 12", &g_aiEmpty, &g_aiEmpty);
        }
        h = next;
    }
}

// ---------------------------------------------------------------------------
// Routes: every node keeps, per side, the first step toward every other
// node; Road_FindRoute searches and fills them.


// The cost of the known route from one node to another, following the
// route tables; DBL_MAX where a table says the route is still unknown.
extern "C" double __cdecl Road_RouteCost(int side, int node, int to)
{
    double sum = 0;
    BYTE code = g_roadNodes[node]->routeTable[side][to];
    while (code != 127) {
        if (code == 255)
            AiError(0, 1, s_trceFile, s_trceId, 408, "Internal error 7893", &g_aiEmpty, &g_aiEmpty);
        int dir = code >> 7;
        RoadEdge* e = g_roadNodes[node]->edges[dir].begin[code & 127];
        sum = Road_EdgeCost(side, e, dir) + sum;
        int next = (&e->from)[dir ^ 1]->id;
        if (next == to)
            return sum;
        node = next;
        code = g_roadNodes[node]->routeTable[side][to];
    }
    return s_noRoute;
}

// remembers the first step of a route at every node it passes
extern "C" void __cdecl Road_StoreRoute(int side, const HopVec* hops)
{
    const Hop* begin = hops->begin;
    const Hop* end = hops->end;
    if (begin == end || (unsigned)((end - begin)) < 2)
        return;
    for (const Hop* to = begin + 1; to != hops->end; to++) {
        int target = to->node;
        for (const Hop* h = hops->begin; h != to; h++)
            g_roadNodes[h->node]->routeTable[side][target] = h->code;
    }
}

extern "C" void __cdecl Road_MarkUnknown(int side, int node, int to)
{
    g_roadNodes[node]->routeTable[side][to] = 127;
}

// the cost from one node to another, searching the route if needed
extern "C" void __cdecl Road_NodeCost(int side, int from, int to, double* cost)
{
    if (from == -1)
        AiError(0, 1, s_trceFile, s_trceId, 568, "Internal error 437", &g_aiEmpty, &g_aiEmpty);
    if (to == -1)
        AiError(0, 1, s_trceFile, s_trceId, 572, "Internal error 438", &g_aiEmpty, &g_aiEmpty);
    *cost = 0;
    if (from == to)
        return;
    if (g_roadNodes[from]->routeTable[side][to] != 255) {
        *cost = Road_RouteCost(side, from, to);
        return;
    }
    HopVecFull route;
    Road_FindRoute(&route, side, from, to, cost);
    unsigned n = route.capacityEnd - route.begin;
    if (n)
        StlDeallocate(route.begin, n * sizeof(Hop));
}

// the cost for the side, replaced by the cost for side 1 when that is less
// than a tenth of it
extern "C" void __cdecl Road_NodeCostBest(int side, int from, int to, double* cost)
{
    double other;
    Road_NodeCost(side, from, to, cost);
    Road_NodeCost(1, from, to, &other);
    if (!(other * 10.0 >= *cost))
        *cost = other;
}

extern "C" void __cdecl Road_PositionCostBest(int side, PositionId from, PositionId to, double* cost)
{
    double other;
    Road_PositionCost(side, from, to, cost);
    Road_PositionCost(1, from, to, &other);
    if (!(other * 10.0 >= *cost))
        *cost = other;
}

// ---------------------------------------------------------------------------
// The AI's view of the game

ASM_VAR(void*, g_aiTruck, $L_6f33ac)
ASM_VAR(int, g_6ced70, $L_6ced70)
extern "C" double __fastcall Wave_Value(void* self, void*, double phase);   // engine.cpp

extern "C" int __cdecl AI_TruckGripLow()
{
    double grip = Wave_Value(g_aiTruck, 0, 0.0);
    return !(grip >= 0.2);
}

// the truck's value at +88 between 8 and 21
extern "C" int __cdecl AI_TruckInRange()
{
    BYTE* t = (BYTE*)g_aiTruck;
    if (!t)
        return 0;
    double v = *(double*)(t + 88);
    return v > 8.0 && !(v >= 21.0);
}

extern "C" int __cdecl AI_Level3()
{
    return g_6ced70 >= 3;
}

// ---------------------------------------------------------------------------
// Entry points for king.masm.

extern "C" {
// an empty STL vector; the argument is its allocator  [FUN_487730]
void** __fastcall Vec_Construct(void** self, void*, int) { self[0] = self[1] = self[2] = 0; return self; }
DBox* __fastcall DBox_Init(DBox* self, void*, const DVec3* a, const DVec3* b) { return self->Init(a, b); }     // [FUN_487ee0]
DBox* __fastcall DBox_Extend(DBox* self, void*, const DVec3* p) { return self->Extend(p); }                   // [FUN_487f60]
void __fastcall Filter1_SetLength(Filter1* self, void*, int n) { self->SetLength(n); }                        // [FUN_488070]
Filter1* __fastcall Filter1_Construct(Filter1* self, void*) { return self->Construct(); }                    // [FUN_4880b0]
void __fastcall Filter1_Reset(Filter1* self, void*) { self->Reset(); }                                       // [FUN_4880d0]
double __fastcall Filter1_Update(Filter1* self, void*, double x) { return self->Update(x); }                 // [FUN_4880e0]
void __fastcall Filter1_Set(Filter1* self, void*, double v) { self->Set(v); }                                // [FUN_488100]
void __fastcall Filter2_SetPoles(Filter2* self, void*, double re, double im) { self->SetPoles(re, im); }     // [FUN_488120]
Filter2* __fastcall Filter2_Construct(Filter2* self, void*) { return self->Construct(); }                    // [FUN_488170]
void __fastcall Filter2_Reset(Filter2* self, void*) { self->Reset(); }                                       // [FUN_4881a0]
double __fastcall Filter2_Update(Filter2* self, void*, double x) { return self->Update(x); }                 // [FUN_4881b0]
void __fastcall Filter2_Set(Filter2* self, void*, double v) { self->Set(v); }                                // [FUN_4881e0]
void __fastcall Filter2_SetResponse(Filter2* self, void*, double t) { self->SetResponse(t); }                // [FUN_4881f0]
}

extern "C" void* __cdecl Stl_Allocate(unsigned bytes);
extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")    // uninitialized copy of 12-byte records

extern "C" {

// [FUN_4875f0] copy constructor of a vector of 12-byte hops
BYTE** __fastcall Hops_Copy(BYTE** self, void*, BYTE* const* src)
{
    int n = (src[1] - src[0]) / 12;
    self[0] = self[1] = self[2] = 0;
    BYTE* p = n ? (BYTE*)Stl_Allocate(n * 12) : 0;
    self[0] = p;
    self[1] = p;
    self[2] = p + n * 12;
    self[1] = CallC<BYTE*>(FUN_47e3e0, src[0], src[1], p, n);
    return self;
}

}

extern "C" void FUN_487d20(); KING_ALIAS("_FUN_487d20", "@Slots8_Construct@12")
ASM_VAR(char, g_691cc0, $L_691cc0)

// [FUN_488320] FUN_487d20(20) on the global at 691cc0
extern "C" int __cdecl Trce_488320()
{
    return CallThis<int>(FUN_487d20, &g_691cc0, 20);
}

// [FUN_487cc0] copy-constructs a 12-byte record in place (placement new)
extern "C" DWORD* __cdecl Rec12_Construct(DWORD* p, const DWORD* src)
{
    if (p) {
        p[0] = src[0];
        p[1] = src[1];
        p[2] = src[2];
    }
    return p;
}

extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")    // the STL node allocator: allocate (bytes)
extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")    // uninitialized copy of 12-byte records (first, last, dest, -)

// [FUN_487740] constructs count vectors of 12-byte records in this array, each a copy of src; the count at +24576
extern "C" void __fastcall Vec12Array_Fill(BYTE* self, int, unsigned count, BYTE** src)
{
    for (unsigned i = 0; i < count; i++) {
        BYTE** e = (BYTE**)(self + i * 12);
        if (!e)
            continue;
        int n = (int)(src[1] - src[0]) / 12;
        e[0] = e[1] = e[2] = 0;
        BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n * 12) : 0;
        e[0] = p;
        e[1] = p;
        e[2] = p + n * 12;
        e[1] = CallC<BYTE*>(FUN_47e3e0, src[0], src[1], p, src);
    }
    *(unsigned*)(self + 24576) = count;
}

extern "C" int __cdecl fputs(const char* s, FILE* f);
extern "C" __declspec(noreturn) void __cdecl abort();
typedef char AsmLabel5[1];
ASM_VAR(AsmLabel5, g_stderr686380, $L_686380)
ASM_VAR(AsmLabel5, s_670328, $L_670328)
ASM_VAR(AsmLabel5, s_66c4e4, $L_66c4e4)

// [FUN_4882c0] writes "a<670328>b<66c4e4>" to stderr and aborts
extern "C" void __cdecl Ai_Abort(const char* a, const char* b)
{
    FILE* f = (FILE*)g_stderr686380;
    fputs(a, f);
    fputs(s_670328, f);
    fputs(b, f);
    fputs(s_66c4e4, f);
    abort();
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

extern "C" void FUN_447ef0(); KING_ALIAS("_FUN_447ef0", "@Vec12_FreeStorage@8")
// [$L_487700] frees the +24576 count of 12-byte vectors at the start
extern "C" void __fastcall Vec12Array_FreeAll(BYTE* s, void*)
{
    BYTE* e = s + *(int*)(s + 24576) * 12;
    for (BYTE* p = s; p != e; p += 12)
        CallThis(FUN_447ef0, p);
}

// ---------------------------------------------------------------------------
// Static initializers: construct a global, register its teardown at exit.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
ASM_VAR(char, g_q197_691cc0, $L_691cc0)
extern "C" void q197_FUN_488320(); KING_ALIAS("_q197_FUN_488320", "_Trce_488320")
extern "C" void q197_L_487db0(); KING_ALIAS("_q197_L_487db0", "@Thunk487db0@8")
// [$L_488340] destroys the static object 691cc0 at exit ($L_487db0)
extern "C" void __cdecl Static691cc0_Destroy(void) { CallThis(q197_L_487db0, &g_q197_691cc0); }
// [$L_488330] registers the teardown at exit
extern "C" int __cdecl Static691cc0_AtExit(void) { return atexit((void(__cdecl*)(void))Static691cc0_Destroy); }
// [$L_488310] the static initializer: FUN_488320, then the teardown at exit
extern "C" int __cdecl Static691cc0_Init(void)
{
    CallC(q197_FUN_488320);
    return Static691cc0_AtExit();
}

// ---------------------------------------------------------------------------
// Searching a route

ASM_PROC(FUN_484810)    // the route from the route tables (out, side, from, to, cost)
extern "C" void FUN_4593b0(); KING_ALIAS("_FUN_4593b0", "_Vec12b_AllocCopyA@12")
extern "C" void FUN_47c650(); KING_ALIAS("_FUN_47c650", "_Rec12_Copy")
extern "C" void FUN_45a9b0(); KING_ALIAS("_FUN_45a9b0", "_Copy12s")
extern "C" void FUN_4abce0(); KING_ALIAS("_FUN_4abce0", "@Vec12_Reserve@16")
extern "C" void FUN_47b790(); KING_ALIAS("_FUN_47b790", "@Vec12_Insert47b790@16")
extern "C" void FUN_457b50(); KING_ALIAS("_FUN_457b50", "_CopyDwords")
extern "C" void FUN_487d00(); KING_ALIAS("_FUN_487d00", "@Rec487d00_Delete@12")
extern "C" void FUN_44aff0(); KING_ALIAS("_FUN_44aff0", "@Vec12_Free@8")
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")

struct RouteVecs {                  // the routes found so far, one per node
    HopVecFull v[2048];
    int        count;               // +24576
};

// v = src
static void Route_Assign(HopVecFull* v, const HopVecFull* src)
{
    if (v == src)
        return;
    BYTE* first = (BYTE*)src->begin;
    BYTE* last = (BYTE*)src->end;
    int n = (int)(last - first) / 12;
    unsigned cap = (int)((BYTE*)v->capacityEnd - (BYTE*)v->begin) / 12;
    if ((unsigned)n > cap) {
        BYTE* p = CallStd<BYTE*>(FUN_4593b0, n, first, last);
        if (cap)
            CallC(FUN_44b4a0, v->begin, cap * 12);
        v->begin = (Hop*)p;
        v->capacityEnd = (Hop*)(p + n * 12);
    } else {
        unsigned have = (int)((BYTE*)v->end - (BYTE*)v->begin) / 12;
        if (have >= (unsigned)n) {
            CallC(FUN_47c650, first, last, v->begin, v->end, 0);
        } else {
            CallC(FUN_45a9b0, first, first + have * 12, v->begin);
            CallC(FUN_47e3e0, first + have * 12, last, v->end, 0);
        }
    }
    v->end = (Hop*)((BYTE*)v->begin + n * 12);
}

static void Route_Push(HopVecFull* v, const Hop* h)
{
    if (v->end != v->capacityEnd) {
        if (v->end)
            *v->end = *h;
        v->end++;
    } else {
        CallThis(FUN_47b790, v, v->end, h);
    }
}

// out = a copy of v
static void Route_Copy(HopVecFull* out, const HopVecFull* v)
{
    char a;
    CallThis(FUN_4abce0, out, v->end - v->begin, &a);
    out->end = (Hop*)CallC<BYTE*>(FUN_47e3e0, v->begin, v->end, out->begin, 0);
}

extern "C" {

// [FUN_484c80] the route from one node to another as hops, and its cost: from the route tables when the side's
// table knows it, else searched (Dijkstra over all nodes) and stored into the tables; empty without a route
HopVecFull* __cdecl Road_FindRoute(HopVecFull* out, int side, int from, int to, double* cost)
{
    if (from == -1)
        AiError(0, 1, s_trceFile, s_trceId, 457, "Internal error 435", &g_aiEmpty, &g_aiEmpty);
    if (to == -1)
        AiError(0, 1, s_trceFile, s_trceId, 461, "Internal error 436", &g_aiEmpty, &g_aiEmpty);
    *cost = 0;
    Hop h;
    if (from == to) {
        HopVecFull v;
        char a;
        Vec_Construct((void**)&v, 0, (int)&a);
        h.node = from;
        h.edge = 0;
        h.dir = 0;
        h.code = 255;
        Route_Push(&v, &h);
        Route_Copy(out, &v);
        CallThis(FUN_44aff0, &v);
        return out;
    }
    if (g_roadNodes[from]->routeTable[side][to] != 255) {
        CallC(FUN_484810, out, side, from, to, cost);
        return out;
    }
    RouteVecs routes;
    int open[2048];
    BYTE done[2048];
    {
        HopVecFull e;
        char a;
        Vec_Construct((void**)&e, 0, (int)&a);
        Vec12Array_Fill((BYTE*)&routes, 0, RoadNodeCount(), (BYTE**)&e);
        CallThis(FUN_44aff0, &e);
    }
    double* costs = g_nodeCosts;
    for (unsigned i = 0; i < RoadNodeCount(); i++)
        costs[i] = s_noRoute;
    int nOpen = 0, nDone = 0;
    int start = g_roadNodes[from]->id;
    for (int i = 0; i < start; i++) {
        open[nOpen++] = i;
        done[nDone++] = 0;
    }
    done[nDone++] = 1;
    open[nOpen++] = start;
    for (unsigned i = start + 1; i < RoadNodeCount(); i++) {
        open[nOpen++] = i;
        done[nDone++] = 0;
    }
    costs[g_roadNodes[from]->id] = 0;
    while (nOpen) {
        int* best = open;
        for (int* p = open + 1; p < open + nOpen; p++)
            if (costs[*p] < costs[*best])
                best = p;
        int u = *best;
        if (!(costs[u] < s_noRoute)) {
            Road_MarkUnknown(side, from, to);
            *cost = s_noRoute;
            out->begin = out->end = out->capacityEnd = 0;
            for (int i = 0; i < routes.count; i++)
                CallThis(FUN_487d00, &routes.v[i], 0);
            return out;
        }
        if (u == to) {
            h.node = to;
            h.edge = 0;
            h.dir = 0;
            h.code = 255;
            HopVecFull* r = &routes.v[u];
            Route_Push(r, &h);
            *cost = costs[u];
            Road_StoreRoute(side, (HopVec*)r);
            if (*cost < s_noRoute)
                Route_Copy(out, r);
            else
                out->begin = out->end = out->capacityEnd = 0;
            for (int i = 0; i < routes.count; i++)
                CallThis(FUN_487d00, &routes.v[i], 0);
            return out;
        }
        if (best + 1 != open + nOpen)
            CallC(FUN_457b50, best + 1, open + nOpen, best);
        nOpen--;
        done[u] = 1;
        HopVecFull* ru = &routes.v[u];
        Road_StoreRoute(side, (HopVec*)ru);
        for (int k = 0; k < 2; k++) {
            RoadNode* nu = g_roadNodes[u];
            for (RoadEdge** it = nu->edges[k].begin; it != g_roadNodes[u]->edges[k].end; it++) {
                RoadEdge* e = *it;
                RoadNode* nb = (&e->from)[k ^ 1];
                if (done[nb->id])
                    continue;
                double c = Road_EdgeCost(side, e, k) + costs[u];
                if (!(c < costs[nb->id]))
                    continue;
                costs[nb->id] = c;
                HopVecFull* rn = &routes.v[nb->id];
                Route_Assign(rn, ru);
                RoadEdge** lo = g_roadNodes[u]->edges[k].begin;
                h.node = u;
                h.edge = lo[it - lo];
                h.dir = 0;
                h.code = (BYTE)((k << 7) | (int)(it - lo));
                Route_Push(rn, &h);
            }
        }
    }
    out->begin = out->end = out->capacityEnd = 0;
    for (int i = 0; i < routes.count; i++)
        CallThis(FUN_447ef0, &routes.v[i]);
    return out;
}

}  // extern "C"

// === the route between two positions (FUN_4858b0)

extern "C" void FUN_409aa0(); KING_ALIAS("_FUN_409aa0", "_Ai_EdgePricedLength")    // the priced length of an edge between two positions (side, edge, PositionId32, PositionId32)
extern "C" void FUN_409660(); KING_ALIAS("_FUN_409660", "_Road_DistanceBetween")    // the distance between two positions on an edge (edge, PositionId32, PositionId32)
extern "C" void FUN_40a4d0(); KING_ALIAS("_FUN_40a4d0", "@Position_InitNode@12")    // a position at a node
extern "C" void FUN_47abf0(); KING_ALIAS("_FUN_47abf0", "@Vec12_PushBack@12")    // push_back of a 12-byte record
extern "C" void FUN_448060(); KING_ALIAS("_FUN_448060", "@Vec12_Clear@8")    // clear of a vector of 12-byte records
extern "C" void FUN_4867e0(); KING_ALIAS("_FUN_4867e0", "@Waypoint_Make@16")    // a hop (node, edge) with no kind
ASM_PROC(FUN_4878b0)    // insert of a range of 12-byte records (pos, first, last)
ASM_VAR(double, g_trce64b5b8, $L_64b5b8)       // 0.1: a position this close to a node is at the node

static PositionId Pos_AtNode(int node)
{
    PositionId p;
    p.road = -1;
    p.node = node;
    p.t = 0;
    p.segment = 0;
    p.object = 0;
    p.m_24 = 0;
    p.m_28 = -1;
    return p;
}

static double Pos_EdgeLength(int side, RoadEdge* e, PositionId a, PositionId b)
{
    return CallC<double>(FUN_409aa0, side, e, a, b);
}

static void Hop_Set(Hop* h, int node, RoadEdge* e)
{
    h->node = node;
    h->edge = e;
    h->dir = 0;
    h->code = 255;
}

static int Hops_Count(const HopVecFull* v)
{
    return (int)((BYTE*)v->end - (BYTE*)v->begin) / 12;
}

extern "C" {

// [FUN_4858b0] the route between two positions as hops, and its cost: by nodes when both are at nodes, else the
// cheapest of the routes between the ends of their edges plus the priced lengths to them; a position within 0.1 of
// an edge end is searched again from that node
HopVecFull* __cdecl Road_FindPositionRoute(HopVecFull* out, int side, PositionId from, PositionId to, double* cost)
{
    if (from.road == -1 && from.node == -1 && !(from.t < 0.0 || from.t > 0.0) && from.segment == 0
        && from.object == 0 && from.m_24 == 0)
        AiError(0, 512, s_trceFile, s_trceId, 597, "Internal error 435", &g_aiEmpty, &g_aiEmpty);
    if (from.node != -1 && to.node != -1)
        return Road_FindRoute(out, side, from.node, to.node, cost);
    *cost = 0;
    char a;
    Hop h;
    if (from.node != -1 && from.node == to.node) {
        HopVecFull v;
        Vec_Construct((void**)&v, 0, (int)&a);
        Hop_Set(&h, from.node, 0);
        Route_Push(&v, &h);
        Route_Copy(out, &v);
        CallThis(FUN_44aff0, &v);
        return out;
    }
    if (from.road == to.road && from.node == to.node && !(from.t < to.t || from.t > to.t)
        && from.segment == to.segment && from.object == to.object) {
        Vec_Construct((void**)out, 0, (int)&a);
        return out;
    }

    // the nodes to start and end at, their costs, and the hops to them (v1 per start, v2 per end)
    int starts[2], ends[2];
    double startCost[2], endCost[2];
    int nStarts = 0, nEnds = 0;
    HopVecFull v1, v2;
    Vec_Construct((void**)&v1, 0, (int)&a);
    Vec_Construct((void**)&v2, 0, (int)&a);
    if (from.node != -1) {
        starts[nStarts] = from.node;
        startCost[nStarts++] = 0;
    } else {
        RoadEdge* e = g_roadEdges[from.road];
        int n0 = e->from->id, n1 = e->to->id;
        starts[nStarts] = n0;
        startCost[nStarts] = Pos_EdgeLength(side, e, from, Pos_AtNode(n0));
        if (!(startCost[nStarts++] >= g_trce64b5b8)) {
            Road_FindPositionRoute(out, side, Pos_AtNode(n0), to, cost);
            CallThis(FUN_44aff0, &v2);
            CallThis(FUN_44aff0, &v1);
            return out;
        }
        Hop_Set(&h, n1, e);
        Route_Push(&v1, &h);
        starts[nStarts] = n1;
        startCost[nStarts] = Pos_EdgeLength(side, e, from, Pos_AtNode(n1));
        if (!(startCost[nStarts++] >= g_trce64b5b8)) {
            Road_FindPositionRoute(out, side, Pos_AtNode(n1), to, cost);
            CallThis(FUN_44aff0, &v2);
            CallThis(FUN_44aff0, &v1);
            return out;
        }
        Hop_Set(&h, n0, e);
        Route_Push(&v1, &h);
    }
    if (to.node != -1) {
        ends[nEnds] = to.node;
        endCost[nEnds++] = 0;
    } else {
        RoadEdge* e = g_roadEdges[to.road];
        int n0 = e->from->id, n1 = e->to->id;
        ends[nEnds] = n0;
        endCost[nEnds] = Pos_EdgeLength(side, e, Pos_AtNode(n0), to);
        if (!(endCost[nEnds++] >= g_trce64b5b8)) {
            Road_FindPositionRoute(out, side, from, Pos_AtNode(n0), cost);
            CallThis(FUN_44aff0, &v2);
            CallThis(FUN_44aff0, &v1);
            return out;
        }
        Hop_Set(&h, n1, 0);
        Route_Push(&v2, &h);
        ends[nEnds] = n1;
        endCost[nEnds] = Pos_EdgeLength(side, e, Pos_AtNode(n1), to);
        if (!(endCost[nEnds++] >= g_trce64b5b8)) {
            Road_FindPositionRoute(out, side, from, Pos_AtNode(n1), cost);
            CallThis(FUN_44aff0, &v2);
            CallThis(FUN_44aff0, &v1);
            return out;
        }
        Hop_Set(&h, n0, 0);
        Route_Push(&v2, &h);
    }

    *cost = s_noRoute;
    HopVecFull r;
    Vec_Construct((void**)&r, 0, (int)&a);
    if (from.road == to.road) {
        // both on one edge: the direct run, towards the edge's end that to is nearer
        RoadEdge* e = g_roadEdges[from.road];
        int n0 = e->from->id, n1 = e->to->id;
        *cost = Pos_EdgeLength(side, e, from, to);
        double d1 = CallC<double>(FUN_409660, g_roadEdges[from.road], from, Pos_AtNode(n1));
        PositionId p;
        CallThis(FUN_40a4d0, &p, n1);
        double d2 = CallC<double>(FUN_409660, g_roadEdges[from.road], to, p);
        if (!(d2 >= d1)) {
            Hop_Set(&h, n0, g_roadEdges[from.road]);
            Route_Push(&r, &h);
            Hop_Set(&h, n1, 0);
            Route_Push(&r, &h);
        } else {
            Hop_Set(&h, n1, g_roadEdges[from.road]);
            CallThis(FUN_47abf0, &r, &h);
            Hop_Set(&h, n0, 0);
            CallThis(FUN_47abf0, &r, &h);
        }
    }
    for (int i = 0; i < nStarts; i++)
        for (int j = 0; j < nEnds; j++) {
            HopVecFull route;
            double c = 0;
            Road_FindRoute(&route, side, starts[i], ends[j], &c);
            c = c + startCost[i] + endCost[j];
            if (!(c >= *cost)) {
                CallThis(FUN_448060, &r);
                *cost = c;
                if (Hops_Count(&v1))
                    CallThis(FUN_47abf0, &r, &v1.begin[i]);
                if (route.begin == route.end) {
                    Hop w;
                    CallThis(FUN_47abf0, &r, CallThis<Hop*>(FUN_4867e0, &w, starts[i], 0));
                } else
                    CallThis(FUN_4878b0, &r, r.end, route.begin, route.end);
                if (Hops_Count(&v2)) {
                    r.end[-1].edge = g_roadEdges[to.road];
                    CallThis(FUN_47abf0, &r, &v2.begin[j]);
                }
            }
            CallThis(FUN_447ef0, &route);
        }
    Hops_Copy((BYTE**)out, 0, (BYTE* const*)&r);
    CallThis(FUN_447ef0, &r);
    CallThis(FUN_447ef0, &v2);
    CallThis(FUN_447ef0, &v1);
    return out;
}

}  // extern "C"

// === the cost between two positions (FUN_486800)

extern "C" {

// [FUN_486800] the cost of the route between two positions: by nodes when both are at nodes, else the cheapest
// over the ends of their edges plus the priced lengths to them (the direct run when both are on one edge); a
// position within 0.1 of an edge end is priced again from that node
void __cdecl Road_PositionCost(int side, PositionId from, PositionId to, double* cost)
{
    if (from.road == -1 && from.node == -1 && !(from.t < 0.0 || from.t > 0.0) && from.segment == 0
        && from.object == 0 && from.m_24 == 0)
        AiError(0, 512, s_trceFile, s_trceId, 744, "Internal error 435", &g_aiEmpty, &g_aiEmpty);
    if (from.node != -1 && to.node != -1) {
        Road_NodeCost(side, from.node, to.node, cost);
        return;
    }
    *cost = 0;
    if (from.node != -1 && from.node == to.node)
        return;
    if (from.road == to.road && from.node == to.node && !(from.t < to.t || from.t > to.t)
        && from.segment == to.segment && from.object == to.object)
        return;
    int starts[2], ends[2];
    double startCost[2], endCost[2];
    int nStarts = 0, nEnds = 0;
    if (from.node != -1) {
        starts[nStarts] = from.node;
        startCost[nStarts++] = 0;
    } else {
        RoadEdge* e = g_roadEdges[from.road];
        int n0 = e->from->id, n1 = e->to->id;
        starts[nStarts] = n0;
        startCost[nStarts] = Pos_EdgeLength(side, e, from, Pos_AtNode(n0));
        if (!(startCost[nStarts++] >= g_trce64b5b8)) {
            Road_PositionCost(side, Pos_AtNode(n0), to, cost);
            return;
        }
        starts[nStarts] = n1;
        startCost[nStarts] = Pos_EdgeLength(side, e, from, Pos_AtNode(n1));
        if (!(startCost[nStarts++] >= g_trce64b5b8)) {
            Road_PositionCost(side, Pos_AtNode(n1), to, cost);
            return;
        }
    }
    if (to.node != -1) {
        ends[nEnds] = to.node;
        endCost[nEnds++] = 0;
    } else {
        RoadEdge* e = g_roadEdges[to.road];
        int n0 = e->from->id, n1 = e->to->id;
        ends[nEnds] = n0;
        endCost[nEnds] = Pos_EdgeLength(side, e, Pos_AtNode(n0), to);
        if (!(endCost[nEnds++] >= g_trce64b5b8)) {
            Road_PositionCost(side, from, Pos_AtNode(n0), cost);
            return;
        }
        ends[nEnds] = n1;
        endCost[nEnds] = Pos_EdgeLength(side, e, Pos_AtNode(n1), to);
        if (!(endCost[nEnds++] >= g_trce64b5b8)) {
            Road_PositionCost(side, from, Pos_AtNode(n1), cost);
            return;
        }
    }
    *cost = s_noRoute;
    if (from.road == to.road)
        *cost = Pos_EdgeLength(side, g_roadEdges[from.road], from, to);
    for (int i = 0; i < nStarts; i++)
        for (int j = 0; j < nEnds; j++) {
            double c = 0;
            Road_NodeCost(side, starts[i], ends[j], &c);
            c = c + startCost[i] + endCost[j];
            if (!(c >= *cost))
                *cost = c;
        }
}

}  // extern "C"
