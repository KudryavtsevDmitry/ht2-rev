// aitrce.cpp - ai_trce.cpp of the AI, rewritten from king.masm: routes on
// the road graph (route tables, route costs, the checks and debug dumps of
// hop sequences), the AI's double precision geometry (cross product, 4x3
// transforms, bounding boxes), the CRC32 the AI uses to check files, and the
// smoothing filters of its controls. The route search itself and the unit's
// static initializers are still assembly.
//
// Building with  nmake ASM_AITRCE=1  makes king.masm assemble the original
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
ASM_PROC(FUN_5a9d40)

extern "C" int __cdecl AI_TruckGripLow()
{
    double grip = CallThis<double>(FUN_5a9d40, g_aiTruck, 0.0);
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
