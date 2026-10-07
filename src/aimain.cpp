// aimain.cpp - Ai_main.cpp of the AI, rewritten from king.masm: its small
// functions: the reference-counted string handle, positions and their
// checks, the nearest node or town to a point, lookups of towns and
// vehicles, the AI's random generator, and the constructors of its records.
//
// Building with  nmake ASM_AIMAIN=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "road.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" void* __cdecl memmove(void* dst, const void* src, size_t n);

static const char s_mainId[] = "$Id: Ai_main.cpp 2001/10/16 14:29:56";
static const char s_mainFile[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\Ai_main.cpp";

extern "C" void __cdecl AI_Error4a52e0(int code, int a);
KING_ALIAS("_AI_Error4a52e0", "FUN_4a52e0")
extern "C" void __cdecl AI_510690();
KING_ALIAS("_AI_510690", "FUN_510690")
extern "C" void** __cdecl Towns_Find453bd0(void** first, void** last, int key, void* rng);
KING_ALIAS("_Towns_Find453bd0", "FUN_453bd0")
extern "C" void** __cdecl Towns_Find453d70(void** first, void** last, int a, int b, int c);
KING_ALIAS("_Towns_Find453d70", "FUN_453d70")
extern "C" void** __cdecl Towns_Find453f10(void** first, void** last, int a, int b);
KING_ALIAS("_Towns_Find453f10", "FUN_453f10")
extern "C" void** __cdecl Towns_Find4540a0(void** first, void** last, int* a, int b);
KING_ALIAS("_Towns_Find4540a0", "FUN_4540a0")
extern "C" void** __cdecl Towns_Find454230(void** first, void** last, int a, int b, void* rng);
KING_ALIAS("_Towns_Find454230", "FUN_454230")
extern "C" DVec3* __cdecl Town_Position(DVec3* out, void* town);
KING_ALIAS("_Town_Position", "FUN_404ef0")
extern "C" DVec3* __cdecl Road_PointOf(DVec3* out, PositionId pos);
KING_ALIAS("_Road_PointOf", "FUN_435bf0")
extern "C" bool __cdecl Vehicle_Matches(void* v, int key);
KING_ALIAS("_Vehicle_Matches", "FUN_4b65e0")
extern "C" void __cdecl Player_UpdateFlags(BYTE* p);        // aiplay.cpp
ASM_PROC(FUN_470950)    // road: set name
ASM_PROC(FUN_44c220)    // vector<char>::insert(pos, first, last)

typedef void** PtrVec;
ASM_VAR(PtrVec, g_towns, $L_68cec0)
ASM_VAR(PtrVec, g_townsEnd, $L_68cec4)
ASM_VAR(PtrVec, g_vehicles, $L_68cfd8)
ASM_VAR(PtrVec, g_vehiclesEnd, $L_68cfdc)
ASM_VAR(PtrVec, g_68cc98, $L_68cc98)
ASM_VAR(PtrVec, g_68cc9c, $L_68cc9c)
ASM_VAR(int, g_carsTotal, $L_68d140)
ASM_VAR(int, g_68cef8, $L_68cef8)
ASM_VAR(int, g_68d150, $L_68d150)
ASM_VAR(int, g_68cee8, $L_68cee8)
typedef int Ints3[3];
ASM_VAR(Ints3, g_68ced0, $L_68ced0)
ASM_VAR(int, g_uniqueCounter, $L_68d158)
typedef char AiText[1];
ASM_VAR(AiText, g_uniqueName, $L_68bad4)
ASM_VAR(AiText, g_uniqueName2, $L_68d020)
ASM_VAR(AiText, g_rng68b9c8, $L_68b9c8)
ASM_VAR(BYTE*, g_68ccb8, $L_68ccb8)
ASM_VAR(void*, g_players, $L_68b990)
struct VecOfPtr { void** begin; void** end; void** cap; };
ASM_VAR(VecOfPtr*, g_68cbf8, $L_68cbf8)

static int Index(void** it, void** first) { return it - first; }

// ---------------------------------------------------------------------------
// The string handle: a pointer to a reference-counted string object (count
// at +4, deleted through vtable +12), a length, and flags; with 0x10000 the
// handle doesn't own the string.

struct StrRef {
    BYTE* ptr;
    int   len;
    DWORD flags;

    bool Owns() const { return !(flags & 0x10000); }
    void CopyFrom(const StrRef* s);
    void Assign(const StrRef* s);
    StrRef* Construct(BYTE* p, int n, DWORD f);
};

void StrRef::CopyFrom(const StrRef* s)
{
    if (!s->Owns()) {
        flags = s->flags;
        ptr = s->ptr;
        len = s->len;
        return;
    }
    len = 0;
    if (s->ptr) {
        ptr = s->ptr;
        flags = s->flags;
        F(int, ptr, 4)++;
    } else {
        ptr = 0;
        flags = 0;
    }
}

void StrRef::Assign(const StrRef* s)
{
    if (ptr && Owns()) {
        F(int, ptr, 4)--;
        if (F(int, ptr, 4) == 0 && !(ptr == s->ptr && flags == s->flags && len == s->len) && ptr)
            CallVirt(ptr, 12, 1);
    }
    CopyFrom(s);
}

StrRef* StrRef::Construct(BYTE* p, int n, DWORD f)
{
    StrRef tmp;
    tmp.ptr = p;
    tmp.len = n;
    tmp.flags = f;
    CopyFrom(&tmp);
    if ((f & 0x10000) == 0 && p) {
        if (--F(int, p, 4) == 0)
            CallVirt(p, 12, 1);
    }
    return this;
}

// a copy of the string at +off of the object a handle points to
static StrRef* MemberString(StrRef* out, const StrRef* h, int off)
{
    out->CopyFrom((const StrRef*)(h->ptr + off));
    return out;
}

static void CheckHandle(const StrRef* h)
{
    if (h->ptr == 0)
        AI_Error4a52e0(0, 1);
    if (h->flags & 0x10000)
        AI_Error4a52e0(4, 1);
}

extern "C" StrRef* __cdecl Str_Member20(StrRef* out, const StrRef* h) { return MemberString(out, h, 20); }
extern "C" StrRef* __cdecl Str_Member20Checked(StrRef* out, const StrRef* h) { CheckHandle(h); return MemberString(out, h, 20); }
extern "C" StrRef* __cdecl Str_Member32Checked(StrRef* out, const StrRef* h) { CheckHandle(h); return MemberString(out, h, 32); }
extern "C" StrRef* __cdecl Str_Member32(StrRef* out, const StrRef* h) { return MemberString(out, h, 32); }

extern "C" int __cdecl Str_Same(const StrRef* a, const StrRef* b)
{
    return a->ptr == b->ptr && a->flags == b->flags && a->len == b->len;
}

// ---------------------------------------------------------------------------
// Positions

static bool ValidRoad(int r) { return r >= 0 && (unsigned)r < (unsigned)(g_roadEdgesEnd - g_roadEdges); }
static bool ValidNode(int n) { return n >= 0 && (unsigned)n < (unsigned)(g_roadNodesEnd - g_roadNodes); }

static int PositionValid(const PositionId* p)
{
    if (!ValidRoad(p->road) && !ValidNode(p->node))
        return 0;
    if (!(p->t >= -0.0002) || p->t > 1.0002)
        return 0;
    return p->segment >= 0 && p->object >= 0;
}

static PositionId* PositionInit(PositionId* p, int node)
{
    p->node = node;
    p->t = 0;
    p->road = -1;
    p->segment = 0;
    p->object = 0;
    p->m_24 = 0;
    p->m_28 = -1;
    return p;
}

static PositionId* PositionNone(PositionId* p)
{
    p->road = -1;
    p->node = -1;
    p->t = 0;
    p->segment = 0;
    p->object = 0;
    p->m_24 = 0;
    p->m_28 = -1;
    return p;
}

static double Distance(const DVec3* a, double bx, double by, double bz)
{
    double dx = a->x - bx;
    double dy = a->y - by;
    double dz = a->z - bz;
    return x87_sqrt((dy * dy + dz * dz) + dx * dx);
}

// the first node closer than 0.1 to a point that has a route table
extern "C" RoadNode* __cdecl Road_NodeAt(DVec3 p)
{
    RoadNode** n = g_roadNodes;
    for (; n != g_roadNodesEnd; n++) {
        DVec3* at = (DVec3*)((BYTE*)*n + 48);
        if (!(Distance(&p, at->x, at->y, at->z) >= 0.1) && !((*n)->flags & 8))
            break;
    }
    return n != g_roadNodesEnd ? *n : 0;
}

extern "C" int* __cdecl Road_NearestNode(int* out, const DVec3* p)
{
    DWORD bestBits[2] = { 0xcf7920e3u, 0x476812f9u };
    double best = *(double*)bestBits;
    int index = 0;
    unsigned count = g_roadNodesEnd - g_roadNodes;
    for (unsigned i = 0; i < count; i++) {
        DVec3* at = (DVec3*)((BYTE*)g_roadNodes[i] + 48);
        double d = Distance(p, at->x, at->y, at->z);
        if (!(d >= best)) {
            best = d;
            index = i;
        }
    }
    *out = index;
    return out;
}

// the node a player is at or nearest to, or the target of its last order
extern "C" int* __cdecl Player_Node(int* out, BYTE* p)
{
    if (F(void*, p, 16) == F(void*, p, 20)) {
        PositionId pos = F(PositionId, p, 32);
        if (pos.node != -1) {
            *out = pos.node;
            return out;
        }
        DVec3 pt;
        Road_PointOf(&pt, pos);
        Road_NearestNode(out, &pt);
        return out;
    }
    *out = F(int, F(BYTE*, p, 20), -12);
    return out;
}

extern "C" int* __cdecl Player_NodeOf(int* out, BYTE* p) { Player_Node(out, p + 8); return out; }

// no node on a route has flag 16 in its data at +192
extern "C" bool __cdecl Route_Open(BYTE* r)
{
    for (BYTE* h = F(BYTE*, r, 24); h != F(BYTE*, r, 28); h += 12) {
        BYTE* data = F(BYTE*, g_roadNodes[F(int, h, 0)], 192);
        if (data && (*data & 16))
            return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Towns

static double TownDistance(void* town, const DVec3* p)
{
    DVec3 at;
    Town_Position(&at, town);
    return Distance(p, at.x, at.y, at.z);
}

static int* NearestTown(int* out, const DVec3* p, int skipFlags)
{
    DWORD maxBits[2] = { 0xffffffff, 0x7fefffff };
    double best = *(double*)maxBits;
    int index = -1;
    for (void** t = g_towns; t != g_townsEnd; t++) {
        if (skipFlags && (F(BYTE, *t, 96) & skipFlags))
            continue;
        double d = TownDistance(*t, p);
        if (best > d) {
            best = d;
            index = t - g_towns;
        }
    }
    *out = index;
    return out;
}

extern "C" int* __cdecl Town_Nearest(int* out, const DVec3* p) { return NearestTown(out, p, 0); }
extern "C" int* __cdecl Town_NearestOpen(int* out, const DVec3* p) { return NearestTown(out, p, 2); }
extern "C" int* __cdecl Town_NearestFree(int* out, const DVec3* p) { return NearestTown(out, p, 3); }

extern "C" int* __cdecl Town_ById(int* out, int id)
{
    void** t = g_towns;
    while (t != g_townsEnd && F(int, *t, 24) != id)
        t++;
    if (t == g_townsEnd) {
        char text[32];
        sprintf(text, "%d", id);
        AiError(78, 1, s_mainFile, s_mainId, 759, "Can not find town with id", text, &g_aiEmpty);
    }
    *out = t - g_towns;
    return out;
}

static int* TownIndex(int* out, void** it)
{
    *out = it != g_townsEnd ? Index(it, g_towns) : -1;
    return out;
}

extern "C" int* __cdecl Town_Find1(int* out, int key) { return TownIndex(out, Towns_Find453bd0(g_towns, g_townsEnd, key, g_rng68b9c8)); }
extern "C" int* __cdecl Town_Find2(int* out, int a, int b, int c) { return TownIndex(out, Towns_Find453d70(g_towns, g_townsEnd, b, a, c)); }
extern "C" int* __cdecl Town_Find3(int* out, int a, int b) { return TownIndex(out, Towns_Find453f10(g_towns, g_townsEnd, a, b)); }
extern "C" int* __cdecl Town_Find4(int* out, int a) { return TownIndex(out, Towns_Find4540a0(g_towns, g_townsEnd, out, a)); }
extern "C" int* __cdecl Town_Find5(int* out, int a, int b) { return TownIndex(out, Towns_Find454230(g_towns, g_townsEnd, a, b, g_rng68b9c8)); }

// whether a town sells goods of a kind, from its first trading item
extern "C" int __cdecl Town_Trades(BYTE* town, int kind)
{
    BYTE* item = F(BYTE*, town, 28);
    BYTE* end = F(BYTE*, town, 32);
    for (; item != end; item += 16)
        if (F(BYTE, item, 4) & 1)
            break;
    if (item == end)
        return 0;
    DWORD v = F(DWORD, g_68ccb8, kind * 104 + 48);
    if (F(BYTE, item, 4) & 4)
        return (v >> 2) & 1;
    return (~v >> 2) & 1;
}

extern "C" int* __cdecl Town_Id(int* out, BYTE* town) { *out = F(int, town, 64); return out; }

extern "C" int* __cdecl Town_IdOrFirst(int* out, BYTE* town)
{
    int id;
    Town_Id(&id, town);
    if (id != -1) {
        *out = id;
        return out;
    }
    for (BYTE* item = F(BYTE*, town, 28); item != F(BYTE*, town, 32); item += 16)
        if (F(int, item, 0) != -1) {
            *out = F(int, item, 0);
            return out;
        }
    *out = -1;
    return out;
}

extern "C" void __cdecl Cars_CheckCount()
{
    int total = 0;
    g_carsTotal = 0;
    for (void** t = g_68cc98; t != g_68cc9c; t++) {
        total += (F(BYTE*, *t, 32) - F(BYTE*, *t, 28)) / 36;
        g_carsTotal = total;
    }
    if (g_68cef8 + g_68d150 + g_68cee8 + g_68ced0[2] > total)
        AiError(0, 1, s_mainFile, s_mainId, 2058, "Too many cars for sale and for hire", &g_aiEmpty, &g_aiEmpty);
}

// ---------------------------------------------------------------------------
// Vehicles

extern "C" int* __cdecl Vehicle_IndexByKey(int* out, int key)
{
    void** v = g_vehicles;
    for (; v != g_vehiclesEnd; v++)
        if (Vehicle_Matches((BYTE*)*v + 8, key))
            break;
    if (v == g_vehiclesEnd)
        AiError(5, 1, s_mainFile, s_mainId, 3675, "Vehicle not found", (const char*)key, &g_aiEmpty);
    *out = v - g_vehicles;
    return out;
}

extern "C" int* __cdecl Vehicle_IndexByName(int* out, const char* name, int owner)
{
    void** v = g_vehicles;
    for (; v != g_vehiclesEnd; v++)
        if (strcmp(name, F(const char*, *v, 20)) == 0 && owner == F(int, *v, 68))
            break;
    if (v == g_vehiclesEnd)
        AiError(5, 1, s_mainFile, s_mainId, 3686, "Vehicle not found", name, &g_aiEmpty);
    *out = v - g_vehicles;
    return out;
}

// ---------------------------------------------------------------------------
// Small helpers

extern "C" void __cdecl AI_Step3()
{
    for (int i = 0; i < 3; i++)
        AI_510690();
}

extern "C" LONG __cdecl AI_Exchange(LONG* target, LONG value) { return InterlockedExchange(target, value); }

extern "C" void __cdecl AI_Backoff(int n)
{
    if (n <= 20)
        Sleep(0);
    else
        Sleep(1 << (n - 20));
}

extern "C" const char* __cdecl AI_UniqueName(const char* base)
{
    sprintf(g_uniqueName, "$$$_%s_%d", base, g_uniqueCounter);
    g_uniqueCounter++;
    return g_uniqueName;
}

extern "C" const char* __cdecl AI_UniqueName2(const char* base, int x)
{
    sprintf(g_uniqueName2, "$$$_%s_%d_%d", base, g_uniqueCounter, x);
    g_uniqueCounter++;
    return g_uniqueName2;
}

extern "C" BYTE* __cdecl Copy4(const BYTE* first, const BYTE* last, BYTE* dest)
{
    int n = (int)(last - first) >> 2 << 2;
    memmove(dest, first, n);
    return dest + n;
}

extern "C" BYTE* __cdecl Copy8(const BYTE* first, const BYTE* last, BYTE* dest)
{
    int n = (int)(last - first) >> 3 << 3;
    memmove(dest, first, n);
    return dest + n;
}

extern "C" BYTE* __cdecl Copy1(const BYTE* first, const BYTE* last, BYTE* dest)
{
    int n = (int)(last - first);
    memmove(dest, first, n);
    return dest + n;
}

extern "C" void __cdecl Road_NameEdge(RoadEdge* e)
{
    char name[128];
    sprintf(name, "$$$_road_%d", e->id);
    CallThis(FUN_470950, e, name);
}

extern "C" void __cdecl Road_NameEdges()
{
    for (RoadEdge** e = g_roadEdges; e != g_roadEdgesEnd; e++)
        Road_NameEdge(*e);
}

extern "C" BYTE* __cdecl AI_ListData(BYTE* node) { return node + 8; }

extern "C" void* __cdecl Player_CopyState(void* out, BYTE* p)
{
    memcpy(out, p + ((F(BYTE, p, 96) & 1) ? 624 : 312), 24);
    return out;
}

extern "C" DVec3* __cdecl DVec3_Add(DVec3* out, const DVec3* a, const DVec3* b)
{
    double x = a->x + b->x, y = b->y + a->y, z = b->z + a->z;
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

extern "C" DVec3* __cdecl DVec3_Scale(DVec3* out, double s, const DVec3* v)
{
    double x = v->x * s, y = v->y * s, z = v->z * s;
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

extern "C" double __cdecl DVec3_Dot(const DVec3* a, const DVec3* b)
{
    return (a->z * b->z + a->y * b->y) + a->x * b->x;
}

extern "C" int* __cdecl AI_StoreInt(int* out, int v) { *out = v; return out; }

extern "C" void* __cdecl AI_PickFrom(int list, unsigned r)
{
    VecOfPtr* v = &g_68cbf8[list];
    return v->begin[r % (unsigned)(v->end - v->begin)];
}

extern "C" int __cdecl Player_Flag4(BYTE* p) { return (F(DWORD, p, 96) >> 2) & 1; }

extern "C" bool __cdecl Player_Idle(BYTE* p)
{
    if (F(int, p, 92) == 1)
        return false;
    if ((BYTE)Player_Flag4(p))
        return false;
    return F(void*, p, 72) == F(void*, p, 76);
}

extern "C" BYTE __cdecl Player_UpdateFlagsGet(BYTE* p)
{
    Player_UpdateFlags(p);
    return F(BYTE, p, 496);
}

// ---------------------------------------------------------------------------
// thiscall methods

struct AiRecord {
    void Append(const void* src, int n);
    void AppendString(const char* s);
    BYTE* Init407380();
    double PriceFactor(const DWORD* item);
    BYTE* Init40e240();
    void SetText(const char* s);
    unsigned Random(unsigned n);
};

// a fixed buffer: capacity +8, data +12, length +16
void AiRecord::Append(const void* src, int n)
{
    BYTE* data = F(BYTE*, this, 12);
    if (!data || F(int, this, 16) + n >= F(int, this, 8))
        return;
    memcpy(data + F(int, this, 16), src, n);
    F(int, this, 16) += n;
}

void AiRecord::AppendString(const char* s)
{
    if (!F(BYTE*, this, 12))
        return;
    for (char c = *s; c && F(int, this, 16) < F(int, this, 8); c = *++s)
        F(BYTE*, this, 12)[F(int, this, 16)++] = c;
}

BYTE* AiRecord::Init407380()
{
    BYTE* s = (BYTE*)this;
    F(int, s, 0) = 0;
    F(int, s, 4) = -1;
    F(int, s, 24) = 0;
    F(int, s, 16) = -1;
    F(int, s, 20) = 0;
    F(int, s, 28) = 0x3ff00000;
    F(int, s, 32) = 0;
    F(int, s, 36) = 0;
    F(int, s, 40) = 0;
    return s;
}

double AiRecord::PriceFactor(const DWORD* item)
{
    double v = 1.0;
    if (item[5] == 2)
        v = 1.5;
    if (item[5] == 3)
        v = 2.0;
    if (F(int, this, 0) == 1)
        return v;
    DWORD f = item[0];
    if (f & 64)
        return v * 10000.0;
    if (F(int, this, 4) == 1)
        return (f & 4) ? v * 10000.0 : v;
    return (f & 8) ? v * 10000.0 : v;
}

BYTE* AiRecord::Init40e240()
{
    BYTE* s = (BYTE*)this;
    for (int off = 0; off <= 48; off += 4)
        if (off != 36)                  // +36 is left as it is
            F(int, s, off) = 0;
    F(int, s, 44) = 0x3ff00000;
    F(int, s, 52) = 0x3ff00000;
    return s;
}

// a vector<char> at +264 set to the string with its terminator
void AiRecord::SetText(const char* s)
{
    BYTE** v = (BYTE**)((BYTE*)this + 264);
    v[1] = v[1] + (v[0] - v[1]);
    CallThis(FUN_44c220, v, v[1], s, s + strlen(s) + 1);
}

// the STL's subtractive generator: 55 entries, indices at +220 and +224
unsigned AiRecord::Random(unsigned n)
{
    unsigned* t = (unsigned*)this;
    unsigned i = (t[55] + 1) % 55;
    unsigned j = (t[56] + 1) % 55;
    t[55] = i;
    t[56] = j;
    t[i] -= t[j];
    return t[t[55]] % n;
}

static BYTE* Fill(BYTE* s, const int* v, int n)
{
    for (int i = 0; i < n; i++)
        F(int, s, i * 4) = v[i];
    return s;
}

#define R(p) ((AiRecord*)(p))

extern "C" {
void __fastcall Buf_Append(void* s, void*, const void* src, int n) { R(s)->Append(src, n); }               // [FUN_401870]
void __fastcall Buf_AppendString(void* s, void*, const char* str) { R(s)->AppendString(str); }           // [FUN_4018b0]
BYTE* __fastcall Rec_Set6(BYTE* s, void*, int a, int b, int c, int d, int e, int f)                      // [FUN_405f40]
{
    int v[] = { a, b, c, d, e, f };
    return Fill(s, v, 6);
}
DVec3* __fastcall DVec3_Assign(DVec3* s, void*, const DVec3* p) { s->x = p->x; s->y = p->y; s->z = p->z; return s; } // [FUN_405f70]
bool __fastcall CharVec_Equals(const char** s, void*, const CharVec* v)                                  // [FUN_407320]
{
    if (v->begin == v->end || !v->begin)
        return false;
    return strcmp(v->begin, *s) == 0;
}
BYTE* __fastcall Rec_Init407380(BYTE* s, void*) { return R(s)->Init407380(); }                           // [FUN_407380]
void __fastcall Str_Assign(StrRef* s, void*, const StrRef* src) { s->Assign(src); }                      // [FUN_40c100]
StrRef* __fastcall Str_Construct(StrRef* s, void*, BYTE* p, int n, DWORD f) { return s->Construct(p, n, f); } // [FUN_40e660]
double __fastcall Rec_PriceFactor(BYTE* s, void*, const DWORD* item) { return R(s)->PriceFactor(item); } // [FUN_409f10]
PositionId* __fastcall Position_InitNode(PositionId* s, void*, int node) { return PositionInit(s, node); } // [FUN_40a4d0]
int __fastcall Position_Valid(const PositionId* s, void*) { return PositionValid(s); }                    // [FUN_40a500]
int* __fastcall Pair_Set(int* s, void*, const int* a, const int* b) { s[0] = *a; s[1] = *b; return s; }   // [FUN_40a580]
BYTE* __fastcall Rec_Init40e240(BYTE* s, void*) { return R(s)->Init40e240(); }                           // [FUN_40e240]
int* __fastcall Rec_Init40f820(int* s, void*) { s[0] = -1; s[1] = s[2] = s[3] = 0; return s; }           // [FUN_40f820]
const char* __fastcall CharVec_Str(const CharVec* s, void*) { return Str(s); }                           // [FUN_410df0]
int* __fastcall Rec_Init410e10(int* s, void*)                                                             // [FUN_410e10]
{
    s[0] = -1;
    s[1] = 0;
    s[2] = (int)g_players;
    for (int i = 3; i <= 8; i++)
        s[i] = 0;
    return s;
}
int* __fastcall Rec_Init411be0(int* s, void*) { s[0] = -1; s[1] = s[2] = s[3] = 0; F(BYTE, s, 16) = 0; return s; } // [FUN_411be0]
int* __fastcall Rec_Init412300(int* s, void*)                                                             // [FUN_412300]
{
    s[0] = s[1] = s[2] = 0;
    s[3] = -1;
    s[4] = s[5] = s[6] = 0;
    F(BYTE, s, 28) = 0;
    return s;
}
PositionId* __fastcall Position_InitNone(PositionId* s, void*) { return PositionNone(s); }               // [FUN_4133a0]
int* __fastcall Rec_Init4140d0(int* s, void*) { s[8] = 0; s[0] = 0; s[9] = s[10] = s[11] = s[12] = 0; return s; } // [FUN_4140d0]
void __fastcall Rec_SetText(void* s, void*, const char* str) { R(s)->SetText(str); }                     // [FUN_4151d0]
unsigned __fastcall Rng_Random(void* s, void*, unsigned n) { return R(s)->Random(n); }                   // [FUN_4156e0]
}

// ---------------------------------------------------------------------------
// Batch 2: players, vehicles, towns

extern "C" void __cdecl AI_Remove46b300(void** first, void** last, void** dest, int a, int b);
KING_ALIAS("_AI_Remove46b300", "FUN_46b300")
extern "C" void __cdecl AI_Grow421d90(int* entry, int a, int b);
KING_ALIAS("_AI_Grow421d90", "FUN_421d90")
extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "FUN_401000")
ASM_VAR(PtrVec, g_68a980, $L_68a980)
ASM_VAR(PtrVec, g_68a984, $L_68a984)
ASM_VAR(BYTE*, g_68cca8, $L_68cca8)
typedef BYTE CarTypes[1];
ASM_VAR(CarTypes, g_carTypes, $L_697914)          // 2416 bytes per type
ASM_VAR(AiText, g_strPresentPick, $L_66d7f8)
ASM_VAR(AiText, g_vtbl64b71c, $L_64b71c)
ASM_VAR(AiText, g_vtbl64b718, $L_64b718)
ASM_VAR(AiText, g_undefinedModule, $L_66d638)
ASM_VAR(AiText, g_undefinedRoom, $L_66d628)

// drops a player from the list at $L_68a980 and resets its trading state
extern "C" void __cdecl Player_Leave(BYTE* p, int a)
{
    void** e = g_68a980;
    for (; e != g_68a984; e++)
        if ((BYTE*)*e + 8 == p + 8)
            break;
    if (e != g_68a984) {
        if (e + 1 != g_68a984)
            AI_Remove46b300(e + 1, g_68a984, e, a, 0);
        g_68a984--;
    }
    F(BYTE, p, 496) = 0;
    F(int, p, 524) = 0;
    F(int, p, 528) = 0;
    F(int, p, 532) = -1;
    Player_UpdateFlags(p);
    F(BYTE, p, 872) = 1;
}

extern "C" int __cdecl Player_IsHuman(BYTE* p) { return F(int, p, 92) == 1 && !(F(DWORD, p, 96) & 0x100); }

// the distance from a node to the nearest other player's node
// node: an iterator to a record whose first int is the node index
extern "C" double __cdecl Players_NearestTo(const int* const* node, const BYTE** list)
{
    double best = 1000000.0;
    for (const BYTE* e = list[0]; e != list[1]; e += 36) {
        if (F(void*, e, 8) == g_players)
            continue;
        DVec3* a = (DVec3*)((BYTE*)g_roadNodes[F(int, e, 0)] + 48);
        DVec3* b = (DVec3*)((BYTE*)g_roadNodes[**node] + 48);
        double d = Distance(b, a->x, a->y, a->z);
        if (best > d)
            best = d;
    }
    return best;
}

// a trading truck (kind 2) waiting in a town without orders
extern "C" bool __cdecl Player_WaitsInTown(BYTE* p)
{
    if (!F(void*, p, 508) || F(void*, p, 168) != g_players || F(void*, p, 152) != F(void*, p, 156))
        return false;
    int kind = F(int, p, 84);
    if (kind == 1 || kind != 2)
        return false;
    return F(int, p, 92) != 257;
}

extern "C" bool __cdecl Player_WaitsInTown2(BYTE* p) { return Player_WaitsInTown(p); }
extern "C" bool __cdecl Player_WaitsInTownOf(BYTE* p) { return Player_WaitsInTown2(p + 8); }

extern "C" int __cdecl Player_FreeTrader(BYTE* p)
{
    int s = F(int, p, 100);
    if (s == 256 || s == 257)
        return 0;
    return Player_WaitsInTown(p + 8) ? 1 : 0;
}

extern "C" void __cdecl Price_Set(BYTE* item, BYTE* table)
{
    F(double, item, 24) = F(double*, table, 100)[F(int, item, 12)] * 0.1;
}

// adds a vehicle's price and its load cost (types other than 4 and 3)
extern "C" void __cdecl Vehicle_AddCosts(int v, double* price, double* load)
{
    if (v == -1)
        return;
    BYTE* veh = (BYTE*)g_vehicles[v];
    int type = F(int, veh, 64);
    BYTE* t = g_carTypes + type * 2416;
    if (F(int, veh, 32) != 4)
        *price = F(float, t, 100) + *price;
    if (F(int, g_vehicles[v], 32) != 3)
        *load = (double)F(float, t, 112) * F(float, t, 108) + *load;
}

extern "C" void __cdecl Vehicle_Costs(int v, double* price, double* load)
{
    *price = 0;
    *load = 0;
    Vehicle_AddCosts(v, price, load);
}

extern "C" void __cdecl Player_VehicleCosts(BYTE* p, double* price, double* load)
{
    BYTE* x = F(BYTE*, p, 516);
    if (x) {
        Vehicle_Costs(F(int, x, 4), price, load);
        return;
    }
    *price = 0;
    *load = 0;
}

extern "C" void __cdecl Player_AllCosts(BYTE* p, double* price, double* load)
{
    *price = 0;
    *load = 0;
    BYTE* x = F(BYTE*, p, 516);
    if (x && F(int, x, 4) != -1)
        Vehicle_AddCosts(F(int, x, 4), price, load);
    x = F(BYTE*, p, 520);
    if (x && F(int, x, 4) != -1) {
        double ignored = 0;
        Vehicle_AddCosts(F(int, x, 4), &ignored, load);
    }
}

extern "C" int __cdecl Player_VehicleData(BYTE* p)
{
    int r = 0;
    BYTE* x = F(BYTE*, p, 516);
    if (x && F(int, x, 4) != -1)
        r = F(int, g_vehicles[F(int, x, 4)], 4);
    x = F(BYTE*, p, 520);
    if (x && F(int, x, 4) != -1)
        r = F(int, g_vehicles[F(int, x, 4)], 4);
    return r;
}

// the price and load of a vehicle limited by a deal's ratio, 85% of them
extern "C" void __cdecl Vehicle_DealCosts(int v, double* price, double* load, double* out, const BYTE* deal)
{
    Vehicle_Costs(v, price, load);
    double a = F(double, deal, 16), b = F(double, deal, 24);
    double x = *load * a / b;
    if (!(x >= *price))
        *price = x;
    x = b * *price / a;
    if (!(x >= *load))
        *load = x;
    *price = *price * 0.85;
    *load = *load * 0.85;
    *out = *price / a;
}

// the player stands at the target of its last order, or has none
extern "C" int __cdecl Player_AtTarget(BYTE* p)
{
    if ((F(DWORD, p, 88) >> 2) & 1)
        return 1;
    if (F(void*, p, 16) == F(void*, p, 20))
        return 1;
    int target = F(int, F(BYTE*, p, 20), -12);
    PositionId pos = F(PositionId, p, 32);
    return pos.node == target;
}

extern "C" int __cdecl Player_AtTargetOf(BYTE* p) { return Player_AtTarget(p + 8); }

extern "C" int __cdecl Player_DoneWaiting(BYTE* p)
{
    return ((F(DWORD, p, 88) >> 2) & 1) && F(void*, p, 16) == F(void*, p, 20);
}

extern "C" int __cdecl Player_DoneWaitingOf(BYTE* p) { return Player_DoneWaiting(p + 8); }

extern "C" void* __cdecl Player_CopyState2(void* out, BYTE* p)
{
    memcpy(out, p + ((F(BYTE, p, 88) & 1) ? 616 : 304), 24);
    return out;
}

// lets the three markets at $L_68ced0 (16 bytes each) grow, up to 100 steps each
extern "C" void __cdecl Markets_Grow(int a, int b)
{
    for (int* e = g_68ced0; e + 1 < (int*)&g_roadNodesEnd; e += 4)
        for (int i = 0; e[1] < e[2] && i < 100; i++)
            AI_Grow421d90(e, a, b);
}

extern "C" DWORD* __cdecl Player_Register(DWORD* out, BYTE* p)
{
    BYTE* x = F(BYTE*, p, 508);
    if (x)
        F(int, g_vehicles[F(int, x, 4)], 108)++;
    ((int*)g_68cca8)[F(int, p, 84)]++;
    *out = F(DWORD, p, 96);
    return out;
}

extern "C" void __cdecl Player_Unregister(BYTE* p)
{
    BYTE* x = F(BYTE*, p, 516);
    if (x)
        F(int, g_vehicles[F(int, x, 4)], 108)++;
    ((int*)g_68cca8)[F(int, p, 92)]--;
}

struct RoomInfo {
    DWORD       flags;
    DWORD       m_4, m_8, m_12;
    const char* module;
    const char* room;
    DWORD       m_24, m_28, m_32;
    int         m_36, m_40;
    DWORD       m_44;
};
extern "C" PositionId* __cdecl Road_RandomPosition(PositionId* out);       // airoad.cpp
extern "C" void __cdecl Road_RoomInfo(PositionId pos, RoomInfo* info);
KING_ALIAS("_Road_RoomInfo", "FUN_438d70")

// a random position that is not in a room with flags 0x60
extern "C" PositionId* __cdecl Road_RandomFreePosition(PositionId* out)
{
    RoomInfo info;
    info.flags = 0;
    info.m_8 = 0;
    info.m_12 = 0;
    info.module = g_undefinedModule;
    info.room = g_undefinedRoom;
    info.m_24 = 0;
    info.m_28 = 0;
    info.m_36 = -1;
    info.m_40 = -1;
    PositionId pos;
    do {
        PositionId tmp;
        pos = *Road_RandomPosition(&tmp);
        Road_RoomInfo(pos, &info);
    } while (info.flags & 0x60);
    *out = pos;
    return out;
}

extern "C" {
int* __fastcall Pair_SetFromPtr(int* s, void*, int a, const int* b) { s[0] = a; s[1] = *b; return s; }   // [FUN_417b50]
BYTE* __fastcall Iter_Data(BYTE** s, void*) { return *s + 8; }                                          // [FUN_418880]
void* __fastcall Market_Get(const int* s, void*) { return g_68cc98[*s]; }                                // [FUN_419490]
void* __fastcall Rec_Get24(BYTE* s, void*, void* out, int i) { memcpy(out, s + i * 24, 24); return out; } // [FUN_41ce30]
int* __fastcall Int_Copy(int* s, void*, const int* src) { *s = *src; return s; }                        // [FUN_41d300]
void** __fastcall Iter_Players(void** s, void*) { *s = g_players; return s; }                           // [FUN_41eb70]
BYTE* __fastcall Pick_Construct(BYTE* s, void*, int a, int b)                                           // [FUN_41f8d0]
{
    F(int, s, 8) = 0;
    double t = AI_Now();
    DWORD f = F(DWORD, s, 8);
    F(int, s, 24) = 0;
    F(double, s, 16) = t;
    F(int, s, 28) = 0;
    F(const char*, s, 32) = g_strPresentPick;
    F(int, s, 40) = 0;
    F(void*, s, 0) = g_vtbl64b71c;
    F(DWORD, s, 8) = f | 1;
    F(double, s, 16) = AI_Now();
    F(int, s, 48) = a;
    F(int, s, 52) = b;
    F(void*, s, 0) = g_vtbl64b718;
    return s;
}
int __fastcall RoadId_Valid(const int* s, void*) { return ValidRoad(*s); }                              // [FUN_423a30]
int __fastcall NodeId_Valid(const int* s, void*) { return ValidNode(*s); }                              // [FUN_423a60]
}

// ---------------------------------------------------------------------------
// Batch 3

extern "C" int* __cdecl Player_FromId(int* io);   // replaces a player id by the player
KING_ALIAS("_Player_FromId", "FUN_477b60")
extern "C" double __cdecl Player_Value430760(BYTE* p);
KING_ALIAS("_Player_Value430760", "FUN_430760")
extern "C" double __cdecl Player_CargoValueOf(BYTE* p);     // aiplay.cpp
extern "C" void __cdecl Player_EndDeal(BYTE* p);             // aiplay.cpp
extern "C" void __cdecl AI_Task42b060(void* task, int a);
KING_ALIAS("_AI_Task42b060", "FUN_42b060")
extern "C" void __cdecl AI_Light569310(unsigned id, int on);
KING_ALIAS("_AI_Light569310", "FUN_569310")
extern "C" void __cdecl AI_Light569410(unsigned id, int on);
KING_ALIAS("_AI_Light569410", "FUN_569410")

ASM_VAR(int, g_68d188, $L_68d188)
ASM_VAR(BYTE, g_68d168, $L_68d168)
ASM_VAR(int, g_66c6e0, $L_66c6e0)
ASM_VAR(int, g_66c6e4, $L_66c6e4)
ASM_VAR(PtrVec, g_68cf60, $L_68cf60)
ASM_VAR(PtrVec, g_68cf64, $L_68cf64)
ASM_VAR(unsigned, g_66c6d0, $L_66c6d0)
ASM_VAR(unsigned, g_68d164, $L_68d164)
ASM_VAR(unsigned, g_68d160, $L_68d160)
ASM_VAR(unsigned, g_68d15c, $L_68d15c)
ASM_VAR(DVec3, g_68bab0, $L_68bab0)
ASM_VAR(BYTE*, g_68cf28, $L_68cf28)
ASM_VAR(double, g_68d190, $L_68d190)
ASM_VAR(int, g_68d18c, $L_68d18c)
ASM_VAR(PtrVec, g_68cc18, $L_68cc18)
ASM_VAR(PtrVec, g_68cc1c, $L_68cc1c)
ASM_VAR(PtrVec, g_68a970, $L_68a970)
ASM_VAR(PtrVec, g_68a974, $L_68a974)
ASM_VAR(AiText, g_str66da80, $L_66da80)
ASM_VAR(AiText, g_strStartKey, $L_66da84)
ASM_VAR(AiText, g_strSeeIfBankrupted, $L_66da00)
ASM_VAR(AiText, g_strFineForBribing, $L_66e0b8)
ASM_VAR(AiText, g_vtbl64b7f0, $L_64b7f0)
ASM_VAR(AiText, g_vtbl64b8c0, $L_64b8c0)

struct PlayerListNode {             // the list of players at $L_68b990
    PlayerListNode* next;
    PlayerListNode* prev;
};

static BYTE* VehicleOf(BYTE* cargo) { return (BYTE*)g_vehicles[F(int, cargo, 4)]; }

static bool IsTrader(BYTE* p)
{
    if (!p || !F(BYTE*, p, 508))
        return false;
    int t = F(int, VehicleOf(F(BYTE*, p, 508)), 32);
    return t == 2 || t == 3;
}

// the strongest of the two directions, as 1/length (lengths below 1 count
// as 1, above 400 three times)
extern "C" double __cdecl AI_MaxInverse(const double* v)
{
    double best = 0;
    for (int k = 0; k < 2; k++, v += 3) {
        double x = v[0], y = v[1], z = v[2];
        double len = x87_sqrt((x * x + z * z) + y * y);
        if (!(len >= 1.0))
            len = 1.0;
        else if (len > 400.0)
            len = len * 3.0;
        double inv = 1.0 / len;
        if (!(best >= inv))
            best = inv;
    }
    return best;
}

extern "C" DVec3* __cdecl DVec3_Times(DVec3* out, const DVec3* v, double s)
{
    double x = v->x, y = v->y, z = v->z;
    out->x = x * s;
    out->y = y * s;
    out->z = z * s;
    return out;
}

extern "C" void __cdecl Markets_GrowOnce()
{
    if (!g_68d188)
        Markets_Grow(0, 0);
}

// still room for players: fewer than the minimum, or (below the maximum)
// farther than 400 from the point
extern "C" bool __cdecl AI_RoomForPlayer(const DVec3* p)
{
    unsigned n = (g_68a984 - g_68a980) + g_68d164;
    if (n < g_68d160)
        return true;
    if (n >= g_68d15c)
        return false;
    double dx = g_68bab0.x - p->x;
    double dy = g_68bab0.y - p->y;
    double dz = g_68bab0.z - p->z;
    return x87_sqrt((dy * dy + dz * dz) + dx * dx) > 400.0;
}

extern "C" bool __cdecl AI_FewWaiting() { return (unsigned)(g_68cf64 - g_68cf60) < g_66c6d0; }

static bool TraderReady(BYTE* p, int notType)
{
    BYTE* c = F(BYTE*, p, 508);
    if (!c || F(BYTE, p, 488) || F(int, p, 84) != 2)
        return false;
    int s = F(int, p, 92);
    if (s == 256 || s == 257 || s == 5)
        return false;
    return F(int, VehicleOf(c), 32) != notType;
}

extern "C" bool __cdecl Player_CanTrade(BYTE* p)
{
    if (!TraderReady(p, 2))
        return false;
    return !((F(DWORD, p, 88) >> 2) & 1);
}

extern "C" int __cdecl Player_CanTrade2(BYTE* p) { return TraderReady(p, 3) ? 1 : 0; }

// a player in the list without a trading vehicle (types 2 and 3)
static bool AnyNonTrader(void** first, void** last)
{
    for (void** e = first; e != last; e++) {
        BYTE* p = (BYTE*)*e + 8;
        if (!p)
            continue;
        BYTE* c = F(BYTE*, p, 508);
        if (!c)
            return true;
        int t = F(int, VehicleOf(c), 32);
        if (t != 2 && t != 3)
            return true;
    }
    return false;
}

extern "C" bool __cdecl AI_AnyNonTrader() { return AnyNonTrader(g_68a980, g_68a984); }
extern "C" bool __cdecl AI_AnyNonTraderWaiting() { return AnyNonTrader(g_68cf60, g_68cf64); }

static int PairOk(BYTE* a, BYTE* b, bool (*any)())
{
    if (IsTrader(a))
        return IsTrader(b) ? 1 : 0;
    if (!b)
        return any() ? 1 : 0;
    if (!F(BYTE*, b, 508))
        return 1;
    if (IsTrader(b))
        return any() ? 1 : 0;
    return 1;
}

extern "C" int __cdecl AI_PairOk(BYTE* a, BYTE* b) { return PairOk(a, b, AI_AnyNonTrader); }
extern "C" int __cdecl AI_PairOkWaiting(BYTE* a, BYTE* b) { return PairOk(a, b, AI_AnyNonTraderWaiting); }

// kind 5 players that point at x point at the list's head instead
extern "C" void __cdecl Players_Forget(void* x)
{
    PlayerListNode* head = (PlayerListNode*)g_players;
    for (PlayerListNode* n = head->next; n != (PlayerListNode*)g_players; n = n->next) {
        BYTE* d = (BYTE*)n + 8;
        if (d && F(int, d, 84) == 5 && F(void*, d, 960) == x)
            F(void*, d, 960) = g_players;
    }
}

extern "C" int __cdecl AI_EndWaiting()
{
    if (g_68d188)
        return 0;
    if (!g_68d168)
        return -1;
    g_68d168 = 0;
    g_66c6e0 = 1;
    for (void** e = g_68cf60; e != g_68cf64; e++) {
        Player_EndDeal((BYTE*)*e + 8);
        F(double, (BYTE*)*e + 8, 784) = AI_Now();
    }
    return 0;
}

extern "C" void __cdecl AI_SetStartKey(int x)
{
    g_66c6e4 = x;
    updateSwitch(g_str66da80, g_strStartKey, x != 1);  // module "dr"
}

extern "C" bool __cdecl AI_True() { return true; }

extern "C" void* __cdecl AI_FirstActive()
{
    for (void** e = g_68cc18; e != g_68cc1c; e++)
        if (F(BYTE, *e, 28))
            return *e;
    return 0;
}

extern "C" void __cdecl AI_RunActive()
{
    for (void** e = g_68cc18; e != g_68cc1c; e++)
        if (F(BYTE, *e, 28))
            AI_Task42b060(*e, 0);
}

static void SwitchLights(BYTE* e, int on)
{
    int kind = F(int, e, 36);
    if (kind != 2 && kind != 1)
        return;
    for (unsigned* id = F(unsigned*, e, 112); id != F(unsigned*, e, 116); id++)
        if (kind == 2)
            AI_Light569310(*id % 1000, on);
        else
            AI_Light569410(*id % 1000, on);
}

extern "C" void __cdecl AI_LightsOn(BYTE* e) { if (F(BYTE, e, 28)) SwitchLights(e, 1); }
extern "C" void __cdecl AI_LightsOff(BYTE* e) { SwitchLights(e, 0); }

static void LightsOffOfKind(int kind)
{
    for (void** e = g_68a970; e != g_68a974; e++)
        if (F(int, *e, 36) == kind)
            AI_LightsOff((BYTE*)*e);
}

extern "C" void __cdecl AI_LightsOff1() { LightsOffOfKind(1); }
extern "C" void __cdecl AI_LightsOff2() { LightsOffOfKind(2); }

extern "C" double __cdecl Player_TotalValue(BYTE* p)
{
    double r = Player_Value430760(p);
    double s = ((F(double, p, 256) + F(double, p, 248)) + F(double, p, 240)) + F(double, p, 208);
    double total = r + s;
    if (F(BYTE, p, 264))
        return Player_CargoValueOf(p) + total;
    return total;
}

extern "C" int __cdecl AI_CountSet(const BYTE* x)
{
    int n = x[0] != 0;
    if (F(double, x, 8) > 0)
        n++;
    if (F(double, x, 40) > 0)
        n++;
    if (F(double, x, 48) > 0)
        n++;
    if (F(double, x, 56) > 0)
        n++;
    if (x[64])
        n++;
    return n;
}

// the distance to the nearer of the two bases
extern "C" double __cdecl AI_BaseDistance(DVec3 p)
{
    double dx = p.x - -1720.0, dy = p.y - 3950.0, dz = p.z - 3.0;
    double d1 = x87_sqrt((dy * dy + dz * dz) + dx * dx);
    dx = p.x - -3000.0;
    dy = p.y - -4715.0;
    dz = p.z - 20.0;
    double d2 = x87_sqrt((dy * dy + dz * dz) + dx * dx);
    return !(d1 >= d2) ? d1 : d2;
}

extern "C" double __cdecl AI_BaseFactor(float x, float y, float z)
{
    DVec3 p;
    p.x = x;
    p.y = y;
    p.z = z;
    double v = ((AI_BaseDistance(p) * 0.00025 + 1.0) * 20.0 + 1.0) - 1.0;
    int i = (int)v;
    return i * 0.1;
}

extern "C" void __cdecl Towns_MarkSpecial()
{
    for (void** t = g_towns; t != g_townsEnd; t++) {
        int* i = F(int*, *t, 40);
        int* end = F(int*, *t, 44);
        for (; i != end; i++)
            if ((double)(int)(F(DWORD, g_68ccb8, *i * 104 + 48) & 4) != 0)
                break;
        if (i != end)
            F(DWORD, *t, 96) |= 1;
    }
}

extern "C" void __cdecl Players_ClearFlags440()
{
    PlayerListNode* head = (PlayerListNode*)g_players;
    for (PlayerListNode* n = head->next; n != (PlayerListNode*)g_players; n = n->next) {
        BYTE* d = (BYTE*)n + 8;
        if (d && !(F(BYTE, d, 88) & 4))
            F(DWORD, d, 88) &= 0xfffffbbf;
    }
}


// a task object: the time it's due at +16, its name at +32
static BYTE* TaskConstruct(BYTE* s, int hi, const char* name, double delay, void* vtbl, int a)
{
    F(int, s, 8) = 0;
    double t = AI_Now();
    DWORD f = F(DWORD, s, 8);
    F(int, s, 24) = 0;
    F(double, s, 16) = t;
    F(int, s, 28) = hi;
    F(const char*, s, 32) = name;
    F(int, s, 40) = 0;
    F(void*, s, 0) = g_vtbl64b71c;
    F(DWORD, s, 8) = f | 1;
    F(double, s, 16) = AI_Now() + delay;
    F(int, s, 48) = a;
    F(void*, s, 0) = vtbl;
    return s;
}

static int PlayerIdOver(int id, double limit)
{
    int s = id;
    Player_FromId(&s);
    return F(double, (BYTE*)s, 768) > limit;
}

static int FarFrom(BYTE* x, int offs, double limit, bool playerFirst)
{
    if (!x)
        return 1;
    int s = (int)x;
    Player_FromId(&s);
    BYTE* p = (BYTE*)s;
    if (p == (BYTE*)g_players)
        return 1;
    double dx, dy, dz;
    if (playerFirst) {
        dx = F(double, p, 624) - F(double, x, offs);
        dy = F(double, p, 632) - F(double, x, offs + 8);
        dz = F(double, p, 640) - F(double, x, offs + 16);
    } else {
        dx = F(double, x, offs) - F(double, p, 624);
        dy = F(double, x, offs + 8) - F(double, p, 632);
        dz = F(double, x, offs + 16) - F(double, p, 640);
    }
    return x87_sqrt((dy * dy + dz * dz) + dx * dx) > limit;
}

extern "C" int __cdecl AI_FarFrom500(BYTE* x) { return FarFrom(x, 64, 500.0, true); }
extern "C" int __cdecl AI_FarFrom600(BYTE* x) { return FarFrom(x, 56, 600.0, false); }

extern "C" {
RoadNode* __fastcall NodeId_Node(const int* s, void*) { return g_roadNodes[*s]; }                       // [FUN_423bf0]
RoadEdge* __fastcall RoadId_Edge(const int* s, void*) { return g_roadEdges[*s]; }                       // [FUN_423f10]
void* __fastcall Rec_Copy32(BYTE* s, void*, void* out) { memcpy(out, s + 16, 32); return out; }         // [FUN_423f20]
int* __fastcall Rec_Init425580(int* s, void*, BYTE* a, int b, int c, int d, int e)                      // [FUN_425580]
{
    s[2] = b; s[3] = c; s[4] = d; s[5] = e;
    s[0] = -1;
    s[6] = a + 8 ? (int)(a + 84) : 0;
    s[7] = (int)a;
    return s;
}
int* __fastcall Rec_Init4255d0(int* s, void*, BYTE* a, int b, int c, int d, int e)                      // [FUN_4255d0]
{
    s[2] = b; s[3] = c; s[4] = d;
    s[0] = -1;
    s[5] = e;
    s[6] = F(int, a, 8);
    s[7] = (int)g_players;
    return s;
}
void* __fastcall TownId_Town(const int* s, void*) { return g_towns[*s]; }                               // [FUN_426270]
int* __fastcall Int_Set(int* s, void*, int a) { *s = a; return s; }                                     // [FUN_426280]
int* __fastcall Int_Set2(int* s, void*, int a, int) { *s = a; return s; }                               // [FUN_426290]
int* __fastcall Int_SetPair(int* s, void*, int a, int b) { s[0] = a; s[1] = b; return s; }              // [FUN_426a90]
BYTE* __fastcall Bankrupt_Construct(BYTE* s, void*, int a) { return TaskConstruct(s, 0x40200000, g_strSeeIfBankrupted, 8.0, g_vtbl64b7f0, a); } // [FUN_426fc0]
BYTE* __fastcall Bribe_Construct(BYTE* s, void*, int a) { return TaskConstruct(s, 0x40000000, g_strFineForBribing, 2.0, g_vtbl64b8c0, a); }   // [FUN_432d50]
BYTE* __fastcall Rec_InitTimes(BYTE* s, void*, double t)                                                // [FUN_428570]
{
    F(double, s, 64) = t;
    F(double, s, 32) = t;
    F(double, s, 0) = t;
    for (int off = 8; off < 96; off += 4)
        if (off >= 32 && off < 40 || off >= 64 && off < 72)
            continue;
        else
            F(int, s, off) = 0;
    return s;
}
int __fastcall AI_FreeCars(void* s, void*)                                                               // [FUN_429340]
{
    int n = F(int, g_68cf28, 8) - g_68d150;
    double v = n;
    if (!(g_68d190 >= 42000.0))
        v = v * (g_68d190 / 42000.0);
    return (int)v;
}
int __fastcall Players_CountBusyTraders(void* s, void*)                                                  // [FUN_429380]
{
    int n = 0;
    PlayerListNode* head = (PlayerListNode*)g_players;
    for (PlayerListNode* e = head->next; e != (PlayerListNode*)g_players; e = e->next) {
        BYTE* d = (BYTE*)e + 8;
        if (F(int, d, 84) != 2)
            continue;
        if (F(void*, d, 152) != F(void*, d, 156) || F(void*, d, 168) != g_players)
            n++;
    }
    return n;
}
int __fastcall PlayerId_Rank(int id, void*)                                                              // [FUN_429760]
{
    int s = id;
    if ((void*)*Player_FromId(&s) == g_players)
        return g_68d18c;
    BYTE* p = (BYTE*)*Player_FromId(&s);
    if (F(void*, p, 160) != F(void*, p, 164))
        return g_68d18c - 1;
    return g_68d18c;
}
int __fastcall PlayerId_Over025(int id, void*) { return PlayerIdOver(id, 0.25); }                       // [FUN_4297c0]
int __fastcall PlayerId_Over015(int id, void*) { return PlayerIdOver(id, 0.15); }                       // [FUN_4297f0]
int __fastcall PlayerId_Over020(int id, void*) { return PlayerIdOver(id, 0.2); }                        // [FUN_429820]
void __fastcall Int_Clear(int* s, void*) { *s = 0; }                                                     // [FUN_433f50]
int* __fastcall Rec_Zero10(int* s, void*) { for (int i = 0; i < 10; i++) s[i] = 0; return s; }          // [FUN_433f60]
double __fastcall Height_Closeness(const double* s, void*, BYTE*** node)                                 // [FUN_434700]
{
    return 1.0 / (x87_fabs(F(double, **node, 48) - *s) + 1000.0);
}
}

// ---------------------------------------------------------------------------
// Batch 4: id iterators, name lookups, STL instances

extern "C" void __cdecl AI_42db90();
KING_ALIAS("_AI_42db90", "FUN_42db90")
extern "C" void __cdecl AI_4031e0();
KING_ALIAS("_AI_4031e0", "FUN_4031e0")
extern "C" void __cdecl AI_449c40(void* out, void* x);
KING_ALIAS("_AI_449c40", "FUN_449c40")
extern "C" void __cdecl Nodes_FindByName(int* out, const char* name);
KING_ALIAS("_Nodes_FindByName", "FUN_449f00")
extern "C" void __cdecl Modules_FindByName(int* out, const char* name);
KING_ALIAS("_Modules_FindByName", "FUN_44a1c0")
extern "C" void __cdecl Town_440390(void* out, void* town);
KING_ALIAS("_Town_440390", "FUN_440390")
extern "C" void __cdecl List_FreeNode(void* node, int size);
KING_ALIAS("_List_FreeNode", "FUN_44b4a0")
extern "C" void __cdecl Copy104(void* dst, const void* src);
KING_ALIAS("_Copy104", "FUN_460960")
extern "C" void __cdecl Copy24a(void* dst, const void* src);
KING_ALIAS("_Copy24a", "FUN_460aa0")
extern "C" void __cdecl Copy24b(void* dst, const void* src);
KING_ALIAS("_Copy24b", "FUN_460ba0")
ASM_PROC(FUN_44c470)    // vector<8 bytes>::insert_aux
ASM_PROC(FUN_44da60)    // vector<104 bytes>::insert_aux
ASM_PROC(FUN_44e3f0)    // vector<4 bytes>::insert_aux
ASM_PROC(FUN_44e970)
ASM_PROC(FUN_44eef0)
ASM_PROC(FUN_450330)    // vector<24 bytes>::insert_aux
ASM_PROC(FUN_4505a0)

ASM_VAR(PtrVec, g_68cbe8, $L_68cbe8)
ASM_VAR(PtrVec, g_68cbec, $L_68cbec)
ASM_VAR(void*, g_68cf6c, $L_68cf6c)
typedef CharVec* NameList[1];
ASM_VAR(NameList*, g_68d0d8, $L_68d0d8)
ASM_VAR(AiText, g_vtbl64b920, $L_64b920)
ASM_VAR(AiText, g_vtbl64b948, $L_64b948)
ASM_VAR(AiText, g_strForfeit, $L_66f320)

static const char s_playHeaderM[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\ai_play.h";

extern "C" int __cdecl AI_Reset()
{
    AI_42db90();
    AI_4031e0();
    return 0;
}

// the first id of a vector and the id after i; -1 at the end
static int* FirstId(int* out, unsigned count) { *out = count ? 0 : -1; return out; }
static int* NextId(int* out, int i, unsigned count) { *out = (unsigned)(i + 1) < count ? i + 1 : -1; return out; }

extern "C" int* __cdecl Road_FirstId(int* out) { return FirstId(out, g_roadEdgesEnd - g_roadEdges); }
extern "C" int* __cdecl Road_NextId(int* out, int i) { return NextId(out, i, g_roadEdgesEnd - g_roadEdges); }
extern "C" int* __cdecl Node_FirstId(int* out) { return FirstId(out, g_roadNodesEnd - g_roadNodes); }
extern "C" int* __cdecl Node_NextId(int* out, int i) { return NextId(out, i, g_roadNodesEnd - g_roadNodes); }
extern "C" int* __cdecl Town_FirstId(int* out) { return FirstId(out, g_townsEnd - g_towns); }
extern "C" int* __cdecl Town_NextId(int* out, int i) { return NextId(out, i, g_townsEnd - g_towns); }
extern "C" int* __cdecl List68cbe8_FirstId(int* out) { return FirstId(out, g_68cbec - g_68cbe8); }
extern "C" int* __cdecl List68cbe8_NextId(int* out, int i) { return NextId(out, i, g_68cbec - g_68cbe8); }
extern "C" int* __cdecl List68cc18_FirstId(int* out) { return FirstId(out, g_68cc1c - g_68cc18); }
extern "C" int* __cdecl List68cc18_NextId(int* out, int i) { return NextId(out, i, g_68cc1c - g_68cc18); }

// list iterators: the first node, and the node after one (the head stays)
extern "C" void** __cdecl Players_First(void** out) { *out = *(void**)g_players; return out; }
extern "C" void** __cdecl Players_Next(void** out, void** it) { *out = it == g_players ? (void*)it : *it; return out; }
extern "C" void** __cdecl List68cf6c_First(void** out) { *out = *(void**)g_68cf6c; return out; }
extern "C" void** __cdecl List68cf6c_Next(void** out, void** it) { *out = it == g_68cf6c ? (void*)it : *it; return out; }

struct Vec3f { float x, y, z; };

extern "C" Vec3f* __cdecl Node_Position(Vec3f* out, int node)
{
    DVec3* p = (DVec3*)((BYTE*)g_roadNodes[node] + 48);
    double x = p->x, y = p->y, z = p->z;
    out->x = (float)x;
    out->y = (float)y;
    out->z = (float)z;
    return out;
}

extern "C" const char* __cdecl Node_Name(int node) { return Str(&g_roadNodes[node]->name); }
extern "C" const char* __cdecl Name68d0d8(int i) { return Str((*g_68d0d8)[i]); }
extern "C" const char* __cdecl Rec_Name8(BYTE* p) { return Str((CharVec*)(p + 8)); }

extern "C" void* __cdecl AI_Make449c40(void* out, void* x)
{
    AI_449c40(out, x);
    return out;
}

static int* FindByName(int* out, const char* name, void (__cdecl* find)(int*, const char*), int line, const char* msg)
{
    int id = (int)name;
    find(&id, name);
    if (id == -1)
        AiError(3, 200, s_mainFile, s_mainId, line, msg, name, &g_aiEmpty);
    *out = id;
    return out;
}

extern "C" int* __cdecl Node_ByName(int* out, const char* name) { return FindByName(out, name, Nodes_FindByName, 14678, "Node not found: "); }
extern "C" int* __cdecl Module_ByName(int* out, const char* name) { return FindByName(out, name, Modules_FindByName, 14688, "Module not found: "); }

// marks the human player's player (kind 5) during state 2
extern "C" void __cdecl AI_MarkHuman(int id)
{
    if (g_68d188 != 2 || id != 1)
        return;
    int s = id;
    BYTE* p = (BYTE*)*Player_FromId(&s) + 8;
    if (F(int, p, 92) != 5)
        return;
    F(int, (BYTE*)*Player_FromId(&s), 724) = 1;
}

extern "C" void* __cdecl Town_Get440390(void* out, int town)
{
    Town_440390(out, g_towns[town]);
    return out;
}

extern "C" DWORD __cdecl Town_Flags(int town) { return F(DWORD, g_towns[town], 96); }
extern "C" int __cdecl Rec_Field12(BYTE* x) { return F(int, x, 12); }
extern "C" int __cdecl Town_Field12(int town) { return Rec_Field12((BYTE*)g_towns[town]); }
extern "C" int __cdecl Player_HasTrader(BYTE* p) { return IsTrader(p) ? 1 : 0; }

extern "C" void __cdecl Road_SetObjectFlags(int road, DWORD flags)
{
    RoadObject** o = g_roadEdges[road]->objects;
    while (o != g_roadEdges[road]->objectsEnd) {
        (*o)->id |= flags;
        o++;
    }
}

extern "C" void** __stdcall List_Erase(void** out, void** node)
{
    void** prev = (void**)node[1];
    void** next = (void**)node[0];
    prev[0] = next;
    next[1] = prev;
    List_FreeNode(node, 12);
    *out = next;
    return out;
}

struct Blob96 { DWORD w[24]; };

struct VecMethods {
    BYTE* begin;
    BYTE* end;
    BYTE* capacityEnd;

    BYTE* EraseBytes(BYTE* first, BYTE* last);
    BYTE* ClearBytes();
    void SeedRandom(unsigned seed);
    void FirstNonEmpty();
    void* ActiveCargo();
};

BYTE* VecMethods::EraseBytes(BYTE* first, BYTE* last)
{
    BYTE* d = first;
    for (int n = end - last; n > 0; n--, d++)
        *d = d[last - first];
    end += first - last;
    return first;
}

BYTE* VecMethods::ClearBytes()
{
    end += begin - end;
    return end;
}

// the STL's subtractive generator: 55 entries, the seed at +216
void VecMethods::SeedRandom(unsigned seed)
{
    unsigned* t = (unsigned*)this;
    for (int i = 0; i < 55; i++)
        t[i] = 0;
    t[54] = seed;
    unsigned k = 1, s = seed;
    for (unsigned i = 0; i < 1134; ) {
        unsigned idx = i % 55;
        s -= k;
        i += 21;
        t[idx] = k;
        k = s;
        s = t[idx];
    }
    for (int round = 0; round < 4; round++)
        for (unsigned i = 0; i < 55; i++)
            t[i] -= t[(i + 31) % 55];
    t[55] = 0;
    t[56] = 31;
}

// the first element of each of four vectors at +52, +64, +76, +88
void VecMethods::FirstNonEmpty()
{
    BYTE* s = (BYTE*)this;
    for (int k = 0; k < 4; k++)
        F(void*, s, 100 + k * 4) = 0;
    for (int k = 0; k < 4; k++) {
        BYTE* b = F(BYTE*, s, 52 + k * 12);
        if (b != F(BYTE*, s, 56 + k * 12))
            F(BYTE*, s, 100 + k * 4) = b;
    }
}

void* VecMethods::ActiveCargo()
{
    BYTE* s = (BYTE*)this;
    BYTE* c = F(BYTE*, s, 512);
    if (c && F(int, c, 4) != -1)
        return c + 28;
    c = F(BYTE*, s, 508);
    if (c && F(int, c, 4) != -1)
        return c + 28;
    AiError(0, 1, s_playHeaderM, s_mainId, 740, "Internal error 9012", &g_aiEmpty, &g_aiEmpty);
    return 0;
}

template <int N> static void PushBack(VecMethods* v, const void* x, void (*proc)())
{
    if (v->end != v->capacityEnd) {
        if (v->end)
            memcpy(v->end, x, N);
        v->end += N;
    } else
        CallThis(proc, v, v->end, x);
}

static void PushBackConstruct(VecMethods* v, const void* x, void (__cdecl* copy)(void*, const void*), int n, void (*proc)())
{
    if (v->end != v->capacityEnd) {
        copy(v->end, x);
        v->end += n;
    } else
        CallThis(proc, v, v->end, x);
}

#define V(p) ((VecMethods*)(p))

extern "C" {
BYTE* __fastcall Note_Construct(BYTE* s, void*, int a, int b, int c, int d)                             // [FUN_435650]
{
    F(int, s, 8) = 0;
    double t = AI_Now();
    F(int, s, 24) = b;
    F(int, s, 28) = c;
    F(double, s, 16) = t;
    F(int, s, 40) = a;
    F(int, s, 32) = d;
    F(void*, s, 0) = g_vtbl64b920;
    return s;
}
Blob96* __fastcall Blob96_Set(Blob96* s, void*, Blob96 v) { *s = v; return s; }                         // [FUN_43f190]
void __fastcall Rng_Seed(void* s, void*, unsigned seed) { V(s)->SeedRandom(seed); }                    // [FUN_440ee0]
void __fastcall Rec_FirstNonEmpty(void* s, void*) { V(s)->FirstNonEmpty(); }                           // [FUN_442800]
void* __fastcall Player_ActiveCargo(void* s, void*) { return V(s)->ActiveCargo(); }                    // [FUN_443800]
BYTE* __fastcall Forfeit_Construct(BYTE* s, void*, int a, int b, int c)                                // [FUN_445a90]
{
    TaskConstruct(s, 0x40200000, g_strForfeit, 8.0, g_vtbl64b948, a);
    F(int, s, 56) = b;
    F(int, s, 60) = c;
    return s;
}
BYTE* __fastcall CharVec_Erase(void* s, void*, BYTE* first, BYTE* last) { return V(s)->EraseBytes(first, last); } // [FUN_4471f0]
BYTE* __fastcall CharVec_Clear(void* s, void*) { return V(s)->ClearBytes(); }                          // [FUN_447230]
void __fastcall Vec8_PushBack(void* s, void*, const void* x) { PushBack<8>(V(s), x, FUN_44c470); }     // [FUN_447530]
void __fastcall Vec104_PushBack(void* s, void*, const void* x) { PushBackConstruct(V(s), x, Copy104, 104, FUN_44da60); } // [FUN_447920]
void** __fastcall Vec4_At(void* s, void*, int i) { return (void**)V(s)->begin + i; }                   // [FUN_447960]
void __fastcall Vec4_PushBackA(void* s, void*, const void* x) { PushBack<4>(V(s), x, FUN_44e3f0); }    // [FUN_447ad0]
void __fastcall Vec4_PushBackB(void* s, void*, const void* x) { PushBack<4>(V(s), x, FUN_44e970); }    // [FUN_447bf0]
void __fastcall Vec4_PushBackC(void* s, void*, const void* x) { PushBack<4>(V(s), x, FUN_44eef0); }    // [FUN_447c20]
int __fastcall Vec4_SegmentCount(void* s, void*, int i) { return CallVirt<int>(((void**)V(s)->begin)[i], 24); } // [FUN_447ec0]
int __fastcall Vec4_Size(void* s, void*) { return (V(s)->end - V(s)->begin) >> 2; }                    // [FUN_447ee0]
void __fastcall Vec12_Clear(void* s, void*)                                                              // [FUN_448060]
{
    VecMethods* v = V(s);
    v->end -= (v->end - v->begin) / 12 * 12;
}
void __fastcall Vec24_PushBackA(void* s, void*, const void* x) { PushBackConstruct(V(s), x, Copy24a, 24, FUN_450330); } // [FUN_4480f0]
void __fastcall Vec24_PushBackB(void* s, void*, const void* x) { PushBackConstruct(V(s), x, Copy24b, 24, FUN_4505a0); } // [FUN_448490]
}

// ---------------------------------------------------------------------------
// Batch 5: more STL instances and small helpers

extern "C" void* __cdecl Stl_Allocate(unsigned bytes);
KING_ALIAS("_Stl_Allocate", "FUN_44b740")
extern "C" void __cdecl Copy36(void* dst, const void* src);
KING_ALIAS("_Copy36", "FUN_4610b0")
extern "C" void __cdecl Copy12(void* dst, const void* src);
KING_ALIAS("_Copy12", "FUN_4611e0")
extern "C" void __cdecl Copy56(void* dst, const void* src);
KING_ALIAS("_Copy56", "FUN_4612d0")
extern "C" void __cdecl AI_Error4a54e0(int code, int a, const char* name);
KING_ALIAS("_AI_Error4a54e0", "FUN_4a54e0")
ASM_PROC(FUN_451230)    // vector<36 bytes>::insert_aux
ASM_PROC(FUN_4514b0)    // vector<4 bytes>::insert_aux
ASM_PROC(FUN_451740)    // vector<16 bytes>::insert_aux
ASM_PROC(FUN_451a80)
ASM_PROC(FUN_451d40)
ASM_PROC(FUN_452000)
ASM_PROC(FUN_452610)    // vector<12 bytes>::insert_aux
ASM_PROC(FUN_452bc0)    // vector<56 bytes>::insert_aux
ASM_PROC(FUN_44b0f0)    // list: a new node holding x
ASM_PROC(FUN_44b250)
ASM_PROC(FUN_44b310)
ASM_PROC(FUN_402ec0)    // a hashtable value's destructor
ASM_VAR(AiText, g_strPair, $L_66cb08)

static BYTE** ListInsert(void* list, BYTE** out, BYTE** pos, const void* x, void (*make)())
{
    BYTE** n = CallThis<BYTE**>(make, list, x);
    n[0] = (BYTE*)pos;
    n[1] = pos[1];
    ((BYTE**)pos[1])[0] = (BYTE*)n;
    pos[1] = (BYTE*)n;
    *out = (BYTE*)n;
    return out;
}

static VecMethods* VecReserve(VecMethods* v, unsigned bytes)
{
    v->begin = v->end = v->capacityEnd = 0;
    BYTE* p = 0;
    if (bytes)
        p = (BYTE*)Stl_Allocate(bytes);
    v->begin = p;
    v->end = p;
    v->capacityEnd = p + bytes;
    return v;
}

static BYTE* EraseDwords(VecMethods* v, BYTE* first, BYTE* last)
{
    DWORD* d = (DWORD*)first;
    for (int n = (v->end - last) >> 2; n > 0; n--, d++)
        *d = *(DWORD*)((BYTE*)d + (last - first));
    v->end -= ((last - first) >> 2) << 2;
    return first;
}

extern "C" unsigned __cdecl Stl_FreeListIndex(unsigned bytes) { return ((bytes + 7) >> 3) - 1; }

extern "C" void __cdecl Stl_Free(void* p, unsigned n) { if (n) List_FreeNode(p, n); }
extern "C" void __cdecl Stl_Free12(void* p, unsigned n) { if (n) List_FreeNode(p, n * 12); }

// the first of the sorted unsigned values not below the key
extern "C" const unsigned* __cdecl LowerBound(const unsigned* first, const unsigned* last, const unsigned* key)
{
    int n = last - first;
    unsigned k = *key;
    while (n > 0) {
        int half = n >> 1;
        if (first[half] < k) {
            first += half + 1;
            n += -1 - half;
        } else
            n = half;
    }
    return first;
}

// the first name in a vector of names equal to a string
extern "C" CharVec** __cdecl NameVec_Find(CharVec** first, CharVec** last, const char* name)
{
    for (; first != last; first++) {
        const CharVec* v = *first;
        if (v->begin != v->end && v->begin && strcmp(v->begin, name) == 0)
            break;
    }
    return first;
}

extern "C" int __cdecl Player_NotValid(BYTE* p) { return F(DWORD, p, 88) != 0x12345678; }

extern "C" int __cdecl Rec_NotValid(BYTE* p)
{
    BYTE* x = F(BYTE*, p, 8);
    int ok = x && F(DWORD, x, 4) == 0x12345678 ? 1 : 0;
    return ok == 0;
}

extern "C" BYTE* __cdecl Find12ByField4(BYTE* first, BYTE* last, int key)
{
    for (; first != last && F(int, first, 4) != key; first += 12)
        ;
    return first;
}

extern "C" BYTE* __cdecl Find12ByField0(BYTE* first, BYTE* last, int key)
{
    for (; first != last && F(int, first, 0) != key; first += 12)
        ;
    return first;
}

extern "C" double __cdecl DVec3_Length(const DVec3* v)
{
    double x = v->x, y = v->y, z = v->z;
    return x87_sqrt((x * x + y * y) + z * z);
}

extern "C" void __cdecl AI_Free(void* p) { free(p); }

extern "C" DWORD* __cdecl CopyDwords(const DWORD* first, const DWORD* last, DWORD* dest)
{
    for (int n = last - first; n > 0; n--)
        *dest++ = *first++;
    return dest;
}

struct HashTable {
    BYTE   m_0[4];
    BYTE** buckets;                 // +4, a vector
    BYTE** bucketsEnd;
    BYTE   m_12[4];
    int    count;                   // +16
};

extern "C" {
BYTE* __fastcall Vec_EndMinus16(void* s, void*) { return V(s)->end - 16; }                              // [FUN_448790]
void** __fastcall Rec_InitPlayerIter(void** s, void*) { s[0] = g_players; s[1] = s[2] = s[3] = 0; return s; } // [FUN_448a40]
BYTE* __fastcall Vec36_At(void* s, void*, int i) { return V(s)->begin + i * 36; }                       // [FUN_448a60]
void __fastcall Vec36_PushBack(void* s, void*, const void* x) { PushBackConstruct(V(s), x, Copy36, 36, FUN_451230); }   // [FUN_448a70]
void __fastcall Vec4_PushBackD(void* s, void*, const void* x) { PushBack<4>(V(s), x, FUN_4514b0); }   // [FUN_448c00]
void __fastcall Vec16_PushBack(void* s, void*, const void* x) { PushBack<16>(V(s), x, FUN_451740); }  // [FUN_448c30]
void __fastcall Vec4_PushBackE(void* s, void*, const void* x) { PushBack<4>(V(s), x, FUN_451a80); }   // [FUN_448c70]
void __fastcall Vec4_PushBackF(void* s, void*, const void* x) { PushBack<4>(V(s), x, FUN_451d40); }   // [FUN_448ca0]
void __fastcall Vec4_PushBackG(void* s, void*, const void* x) { PushBack<4>(V(s), x, FUN_452000); }   // [FUN_448f00]
void __fastcall Vec12_PushBackCopy(void* s, void*, const void* x) { PushBackConstruct(V(s), x, Copy12, 12, FUN_452610); }   // [FUN_4490f0]
void __fastcall Vec56_PushBack(void* s, void*, const void* x) { PushBackConstruct(V(s), x, Copy56, 56, FUN_452bc0); }   // [FUN_4492c0]
BYTE* __fastcall Ptr_Add4(BYTE* s, void*, int i) { return s + i * 4; }                                  // [FUN_449c30]
BYTE* __fastcall Ptr_Add32(BYTE* s, void*, int i) { return s + (i << 5); }                              // [FUN_44a480]
VecMethods* __fastcall Vec1_Reserve(VecMethods* s, void*, unsigned n, int) { return VecReserve(s, n); }      // [FUN_44a490]
VecMethods* __fastcall Vec24_Reserve(VecMethods* s, void*, unsigned n, int) { return VecReserve(s, n * 24); } // [FUN_44a4c0]
VecMethods* __fastcall Vec4_Reserve(VecMethods* s, void*, unsigned n, int) { return VecReserve(s, n * 4); }   // [FUN_44a550]
BYTE** __fastcall ListA_Insert(void* s, void*, BYTE** out, BYTE** pos, const void* x) { return ListInsert(s, out, pos, x, FUN_44b0f0); } // [FUN_44a6f0]
BYTE** __fastcall ListB_Insert(void* s, void*, BYTE** out, BYTE** pos, const void* x) { return ListInsert(s, out, pos, x, FUN_44b250); } // [FUN_44afc0]
BYTE** __fastcall ListC_Insert(void* s, void*, BYTE** out, BYTE** pos, const void* x) { return ListInsert(s, out, pos, x, FUN_44b310); } // [FUN_44b020]
BYTE** __fastcall Ref_Copy(BYTE** s, void*, BYTE* const* src, int)                                       // [FUN_44a820]
{
    BYTE* p = *src;
    *s = p;
    if (p)
        InterlockedIncrement((LONG*)(p + 4));
    return s;
}
BYTE* __fastcall Vec4_Erase(void* s, void*, BYTE* first, BYTE* last) { return EraseDwords(V(s), first, last); }  // [FUN_44ac10]
BYTE* __fastcall Vec4_Erase2(void* s, void*, BYTE* first, BYTE* last) { return EraseDwords(V(s), first, last); } // [FUN_44af80]
void __fastcall Vec32_Free(void* s, void*)                                                               // [FUN_44b0b0]
{
    unsigned n = (V(s)->capacityEnd - V(s)->begin) >> 5;
    if (n)
        List_FreeNode(V(s)->begin, n << 5);
}
void __fastcall Hash_Clear(HashTable* s, void*)                                                          // [FUN_44fe20]
{
    for (unsigned i = 0; i < (unsigned)(s->bucketsEnd - s->buckets); i++) {
        for (BYTE* n = s->buckets[i]; n; ) {
            BYTE* next = F(BYTE*, n, 0);
            CallThis(FUN_402ec0, n + 4);
            List_FreeNode(n, 12);
            n = next;
        }
        s->buckets[i] = 0;
    }
    s->count = 0;
}
int* __fastcall Rec_InitFromPtr(int* s, void*, const int* a) { s[0] = *a; s[1] = s[2] = s[3] = 0; return s; } // [FUN_450f50]
StrRef* __fastcall Str_CopyConstruct(StrRef* s, void*, const StrRef* src) { s->CopyFrom(src); return s; }    // [FUN_454580]
int __fastcall Rec_Get0(const int* s, void*) { return s[0]; }                                           // [FUN_455940]
int __fastcall Rec_Get1(const int* s, void*) { return s[1]; }                                           // [FUN_455950]
int* __fastcall Pair_Copy(int* s, void*, const int* src) { s[0] = src[0]; s[1] = src[1]; return s; }    // [FUN_458a20]
int* __fastcall Rec_Copy8Skip1(int* s, void*, const int* src)                                           // [FUN_458a40]
{
    s[0] = src[0];
    for (int i = 2; i < 8; i++)
        s[i] = src[i];
    return s;
}
// assigns a string handle whose object must be a pair (type 2)
void __fastcall Str_AssignPair(StrRef* s, void*, const StrRef* src)                                     // [FUN_44cb40]
{
    if (s->ptr) {
        F(int, s->ptr, 4)--;
        if (F(int, s->ptr, 4) == 0 && !(s->ptr == src->ptr && s->flags == src->flags && s->len == src->len) && s->ptr)
            CallVirt(s->ptr, 12, 1);
    }
    BYTE* p = src->ptr;
    if (!p) {
        s->ptr = 0;
        s->flags = 0;
        s->len = 0;
        return;
    }
    if (CallVirt<int>(p, 0) != 2) {
        AI_Error4a54e0(2, 1, CallVirt<const char*>(src->ptr, 4, (const char*)g_strPair));
        return;
    }
    s->ptr = p;
    s->flags = src->flags;
    F(int, p, 4)++;
    s->len = 0;
}
}
