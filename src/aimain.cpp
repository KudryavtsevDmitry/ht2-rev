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
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" void* __cdecl memmove(void* dst, const void* src, size_t n);

static const char s_mainId[] = "$Id: Ai_main.cpp 2001/10/16 14:29:56";
static const char s_mainFile[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\Ai_main.cpp";

extern "C" void __cdecl AI_Error4a52e0(int code, int a);
KING_ALIAS("_AI_Error4a52e0", "FUN_4a52e0")
extern "C" int __cdecl Loading_Step();    // carat.cpp
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
extern "C" bool __cdecl SchValue_IsSymbol(const BYTE* v, const char* name);   // schitem.cpp
extern "C" void __cdecl Player_UpdateFlags(BYTE* p);        // aiplay.cpp
extern "C" void __fastcall CharVec_Assign(char** self, void*, const char* s);    // aiordr.cpp
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
        if (SchValue_IsSymbol((BYTE*)*v + 8, (const char*)key))
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
        Loading_Step();
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
    CharVec_Assign((char**)e, 0, name);
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

extern "C" DWORD* __cdecl Copy32(const DWORD* first, const DWORD* last, DWORD* to);   // aimult.cpp
extern "C" void __cdecl AI_Grow421d90(int* entry, int a, int b);
KING_ALIAS("_AI_Grow421d90", "FUN_421d90")
extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "FUN_401000")
ASM_VAR(PtrVec, g_68a980, $L_68a980)
ASM_VAR(PtrVec, g_68a984, $L_68a984)
ASM_VAR(BYTE*, g_68cca8, $L_68cca8)
typedef BYTE CarTypeBytes[1];
// g_carTypes (car.h) + 76, 2416 bytes per type: +100 here is the type's +176
ASM_VAR(CarTypeBytes, g_carTypes76, $L_697914)
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
            Copy32((const DWORD*)(e + 1), (const DWORD*)g_68a984, (DWORD*)e);
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
    BYTE* t = g_carTypes76 + type * 2416;
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
KING_ALIAS("_Player_Value430760", "_Player_OrdersValue")
extern "C" double __cdecl Player_CargoValueOf(BYTE* p);     // aiplay.cpp
extern "C" void __cdecl Player_EndDeal(BYTE* p);             // aiplay.cpp
extern "C" void __cdecl AI_Task42b060(void* task, int a);
KING_ALIAS("_AI_Task42b060", "_Rec_SetActive")
extern "C" void __cdecl Screen_569310(int id, unsigned on);        // copter.cpp
extern "C" void __cdecl Screen_569410(int id, int on);             // copter.cpp

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
            Screen_569310(*id % 1000, on);
        else
            Screen_569410(*id % 1000, on);
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

// ---------------------------------------------------------------------------
// medium-sized functions of the unit

ASM_VAR(unsigned, g_spinCount, $L_66c83c)       // spins before sleeping: 30, or 1000 after a quick success
ASM_VAR(unsigned, g_spinLast, $L_68d208)
ASM_VAR(char, g_strAiMainFile, $L_66c7b0)       // "C:\Nek\Vrappl\Gi\Htai\Ai_main.cpp"
ASM_VAR(char, g_strAiMainId, $L_66c558)         // "$Id: Ai_main.cpp ..."
ASM_VAR(char, g_strErr05491, $L_66c840)         // "Internal error 05491"
ASM_VAR(BYTE**, g_vec68d0d8, $L_68d0d8)
ASM_VAR(BYTE**, g_vec68d0dc, $L_68d0dc)
ASM_VAR(BYTE**, g_vec68d0c8, $L_68d0c8)
ASM_VAR(BYTE**, g_vec68d0cc, $L_68d0cc)

// the spin lock of the allocators: spins, then sleeps longer and longer
extern "C" void __fastcall Stl_Lock(volatile LONG* l, void*)
{
    if (!InterlockedExchange(l, 1))
        return;
    unsigned n = g_spinCount;
    unsigned half = g_spinLast >> 1;
    volatile int spin = 17;
    for (unsigned i = 0; i < n; i++) {
        if (i >= half && *l == 0) {
            if (!InterlockedExchange(l, 1)) {
                g_spinLast = i;
                g_spinCount = 1000;
                return;
            }
            continue;
        }
        for (int k = 0; k < 4; k++)
            spin = spin * spin;
    }
    g_spinCount = 30;
    for (int k = 6;; k++) {
        int j = k > 27 ? 27 : k;
        if (!InterlockedExchange(l, 1))
            return;
        if (j <= 20)
            Sleep(0);
        else
            Sleep(1 << (j - 20));
    }
}

// whether this position is before p on the same road
extern "C" bool __fastcall Pos_Before(PositionId* self, void*, PositionId p)
{
    if (self->road == p.road && self->node == p.node && x87_eq(self->t, p.t) && self->segment == p.segment
        && self->object == p.object && self->m_24 == p.m_24)
        return false;
    PositionId l = *self;
    int r = l.road;
    if (r == -1)
        r = p.road;
    if (p.road == -1)
        p.road = r;
    if (r == -1 || r != p.road) {
        AiError(0, 100, &g_strAiMainFile, &g_strAiMainId, 403, &g_strErr05491, &g_aiEmpty, &g_aiEmpty);
        return false;
    }
    BYTE* e = (BYTE*)g_roadEdges[r];
    RoadNode* a = g_roadNodes[p.node];
    RoadNode* e74 = F(RoadNode*, e, 116);
    if (a == e74)
        return true;
    RoadNode* e70 = F(RoadNode*, e, 112);
    if (a == e70)
        return false;
    RoadNode* b = g_roadNodes[l.node];
    if (b == e70)
        return true;
    if (b == e74)
        return false;
    if (l.object < p.object)
        return true;
    if (l.object > p.object)
        return false;
    if (l.segment < p.segment)
        return true;
    if (l.segment > p.segment)
        return false;
    return !(l.t >= p.t);
}

// the indices kept in the nodes, edges and the two name lists
extern "C" void __cdecl AI_Renumber()
{
    for (unsigned i = 0; i < (unsigned)(g_roadNodesEnd - g_roadNodes); i++)
        F(unsigned, g_roadNodes[i], 16) = i;
    for (unsigned i = 0; i < (unsigned)(g_roadEdgesEnd - g_roadEdges); i++)
        F(unsigned, g_roadEdges[i], 12) = i;
    for (unsigned i = 0; i < (unsigned)(g_vec68d0dc - g_vec68d0d8); i++)
        F(unsigned, g_vec68d0d8[i], 12) = i;
    for (unsigned i = 0; i < (unsigned)(g_vec68d0cc - g_vec68d0c8); i++)
        F(unsigned, g_vec68d0c8[i], 12) = i;
}

static void FreeVec4(BYTE* begin, BYTE* cap)
{
    unsigned n = (unsigned)((int)(cap - begin) >> 2);
    if (n)
        StlDeallocate(begin, n * 4);
}

static void FreeBytes(BYTE* begin, BYTE* cap)
{
    unsigned n = (unsigned)(cap - begin);
    if (n)
        StlDeallocate(begin, n);
}

extern "C" void __fastcall Rec403810_Destroy(BYTE** self, void*)
{
    FreeVec4(self[10], self[12]);
    FreeVec4(self[7], self[9]);
    FreeBytes(self[4], self[6]);
    FreeBytes(self[0], self[2]);
}

extern "C" void __fastcall Rec4046b0_Destroy(BYTE* self, void*)
{
    FreeVec4(F(BYTE*, self, 360), F(BYTE*, self, 368));
    FreeBytes(F(BYTE*, self, 316), F(BYTE*, self, 324));
    BYTE* o;
    if (!(F(DWORD, self, 312) & 0x10000) && (o = F(BYTE*, self, 304)) != 0) {
        F(int, o, 4)--;
        o = F(BYTE*, self, 304);
        if (F(int, o, 4) == 0 && o)
            CallVirt(o, 12, 1);
    }
}

ASM_PROC(FUN_404ef0)    // a town's position (out, town)
ASM_PROC(FUN_44c470)    // vector<double>: insert (pos, value)
ASM_PROC(FUN_47c650)
ASM_PROC(FUN_50d190)
extern "C" void __fastcall Waypoints_Clear(BYTE* self, void*);   // aiplay.cpp
extern "C" void __cdecl Road_NodeCostBest(int side, int from, int to, double* cost);   // aitrce.cpp

extern "C" void __fastcall Rec404920_Destroy(BYTE** self, void*)
{
    BYTE** a = self ? self + 3 : 0;
    FreeBytes(a[1], a[3]);
    FreeBytes(self[0], self[2]);
}

// the nearest other town to town i
extern "C" void __cdecl Town_NearestOther(int* out, int i)
{
    DVec3 a, b;
    CallC(FUN_404ef0, &a, g_towns[i]);
    int best = -1;
    double bestDist = 1.7976931348623157e308;
    for (void** p = g_towns; p != g_townsEnd; p++) {
        if (i == p - g_towns)
            continue;
        CallC(FUN_404ef0, &b, *p);
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        double dz = a.z - b.z;
        double d = x87_sqrt(dy * dy + dz * dz + dx * dx);
        if (bestDist > d) {
            best = (int)(p - g_towns);
            bestDist = d;
        }
    }
    *out = best;
}

// the cost of an edge for a side and direction: its objects' costs, weighted (cached)
extern "C" double __cdecl Road_EdgeCost(int side, RoadEdge* edge, int dir)
{
    BYTE* e = (BYTE*)edge;
    int k = dir + side * 2;
    if (F(char, e, k * 16 + 56))
        return F(double, e, (k + 3) * 16);
    double sum = 0.0;
    for (unsigned i = 0; i < (unsigned)((F(BYTE*, e, 124) - F(BYTE*, e, 120)) >> 2); i++) {
        BYTE* o = F(BYTE**, e, 120)[i];
        BYTE* d = o ? o + 8 : 0;
        double w = 1.0;
        if (F(int, d, 20) == 2)
            w = 1.5;
        if (F(int, d, 20) == 3)
            w = 2.0;
        if (side != 1) {
            DWORD f = F(DWORD, d, 0);
            if ((f & 64) || (dir == 1 ? (f & 4) : (f & 8)))
                w = w * 10000.0;
        }
        sum = CallVirt<double>(o, 36) * w + sum;
    }
    F(char, e, k * 16 + 56) = 1;
    F(double, e, (k + 3) * 16) = sum;
    return sum;
}

// the costs between all towns, appended to each town's list
extern "C" void __cdecl Towns_Costs()
{
    for (void** p = g_towns; p != g_townsEnd; p++)
        for (void** q = g_towns; q != g_townsEnd; q++) {
            int a, b;
            Town_IdOrFirst(&a, (BYTE*)*p);
            Town_IdOrFirst(&b, (BYTE*)*q);
            double c = 0.0;
            if (a != b && a != -1 && b != -1) {
                Road_NodeCostBest(0, a, b, &c);
                AI_Step3();
            }
            BYTE* v = (BYTE*)*p + 100;
            double* end = F(double*, v, 4);
            if (end != F(double*, v, 8)) {
                if (end)
                    *end = c;
                F(double*, v, 4) = end + 1;
            } else
                CallThis(FUN_44c470, v, end, &c);
        }
}

// whether v is in [first, last); frees the vector's buffer
extern "C" bool __cdecl PtrVec_FindFree(int* first, int* last, int* cap, int v)
{
    int* p = first;
    while (p != last && *p != v)
        p++;
    bool found = last != p;
    FreeVec4((BYTE*)first, (BYTE*)cap);
    return found;
}

// a player stops waiting: off the waiting list, its route dropped
extern "C" void __cdecl Player_StopWaiting(BYTE* pl)
{
    for (void** p = g_68cf60; p != g_68cf64; p++)
        if (pl + 8 == (BYTE*)*p + 8) {
            if (p + 1 != g_68cf64)
                Copy32((const DWORD*)(p + 1), (const DWORD*)g_68cf64, (DWORD*)p);
            g_68cf64--;
            break;
        }
    F(DWORD, pl, 96) &= ~0x4000u;
    if (F(int, pl, 100) == 5) {
        F(int, pl, 100) = 0;
        BYTE* first = F(BYTE*, pl, 24);
        BYTE* last = F(BYTE*, pl, 28);
        BYTE* it = CallC<BYTE*>(FUN_47c650, last, last, first, first, 0);
        BYTE* end = F(BYTE*, pl, 28);
        for (; it != end; it += 12)
            CallC(FUN_50d190, it);
        F(BYTE*, pl, 28) += -((int)(last - first) / 12) * 12;
        F(BYTE, pl, 872) = 1;
        Waypoints_Clear(pl + 72, 0);
    }
    Player_UpdateFlags(pl);
    F(BYTE, pl, 872) = 1;
}

ASM_PROC(FUN_477b60)    // an order's player (in place)
extern "C" void __fastcall Buf_Append(void* s, void*, const void* src, int n);
extern "C" void __fastcall Buf_AppendString(void* s, void*, const char* str);
ASM_VAR(char, g_strStart, $L_66c830)            // "START:"
ASM_VAR(char, g_strCompl, $L_66c7ec)            // "COMPL:"
ASM_VAR(char, g_strCancelSelling, $L_66d498)    // "cancelSellingDealer(PlayerId player)"
ASM_VAR(char, g_strAiPlayH, $L_66d3fc)          // "C:\Nek\Vrappl\Gi\Htai\ai_play.h"
ASM_VAR(char, g_strErr9012, $L_66d41c)          // "Internal error 9012"
ASM_VAR(char, g_strTrailerStorm, $L_66d72c)     // "STrailerStorm"
ASM_VAR(char, g_strTownAA, $L_66d768)           // "TOWN_AA"
ASM_VAR(int, g_68d14c, $L_68d14c)
ASM_VAR(int, g_68d154, $L_68d154)
ASM_VAR(double, g_64b710, $L_64b710)            // 12
ASM_VAR(double, g_64b708, $L_64b708)            // 32

// a player's current order list (+28 of the order at +520, else +516); on an
// error the caller's previous pointer
static BYTE* Player_Orders(BYTE* pl, BYTE* fallback)
{
    BYTE* o = F(BYTE*, pl, 520);
    if (o && F(int, o, 4) != -1)
        return o + 28;
    o = F(BYTE*, pl, 516);
    if (o && F(int, o, 4) != -1)
        return o + 28;
    AiError(0, 1, &g_strAiPlayH, &g_strAiMainId, 740, &g_strErr9012, &g_aiEmpty, &g_aiEmpty);
    return fallback;
}

extern "C" int __cdecl Dealer_CancelSelling(BYTE* pl)
{
    Buf_Append(&g_stateDump, 0, &g_strStart, 8);
    Buf_AppendString(&g_stateDump, 0, &g_strCancelSelling);
    if (F(int, pl, 92) != 9) {
        Buf_Append(&g_stateDump, 0, &g_strCompl, 8);
        Buf_AppendString(&g_stateDump, 0, &g_strCancelSelling);
        return 0;
    }
    if (F(int, pl, 100) == 256) {
        BYTE* o = F(BYTE*, pl, 972);
        F(int, o, 4)--;
        if ((char)Player_Flag4(pl))
            F(int, F(BYTE*, pl, 972), 12)--;
    }
    if (F(int, pl, 100) == 257) {
        g_68d14c--;
        if ((char)Player_Flag4(pl))
            g_68d154--;
    }
    F(int, pl, 100) = 0;
    F(DWORD, pl, 96) &= 0xfffffbbf;
    int r = F(int, pl, 120);
    if (r != -1) {
        F(int, pl, 120) = -1;
        BYTE* d = F(BYTE*, g_68cc98[r], 28) + F(int, pl, 124) * 36;
        F(void*, d, 8) = g_players;
        d = F(BYTE*, g_68cc98[r], 28) + F(int, pl, 124) * 36;
        F(DWORD, d, 4) &= ~3u;
        F(int, g_68cc98[r], 40)--;
    }
    F(int, pl, 972) = 0;
    BYTE* sd = (BYTE*)&g_stateDump;
    char*& buf = F(char*, sd, 12);
    int& len = F(int, sd, 16);
    int cap = F(int, sd, 8);
    if (buf && len + 8 < cap) {
        memcpy(buf + len, &g_strCompl, 8);
        len += 8;
    }
    if (buf)
        for (const char* s = &g_strCancelSelling; *s && len < F(int, sd, 8);)
            buf[len++] = *s++;
    return 0;
}

// the sums of the player's orders: weight and volume of the cargo
extern "C" void __cdecl Player_CargoSums(BYTE* pl, double* a, double* b)
{
    *a = 0;
    *b = 0;
    BYTE* v = Player_Orders(pl, (BYTE*)a);
    BYTE* it = *(BYTE**)v;
    BYTE* last = (BYTE*)a;
    for (;;) {
        last = Player_Orders(pl, last);
        if (it == F(BYTE*, last, 4))
            break;
        BYTE* rec = g_68ccb8 + F(int, it, 0) * 104;
        double n = F(double, it, 56);
        it += 144;
        *a = F(double, rec, 16) * n + *a;
        *b = F(double, rec, 24) * n + *b;
    }
}

extern "C" DWORD* __fastcall Rec48_Construct(DWORD* self, void*, DWORD a0, DWORD a1, DWORD a2, DWORD a3,
                                             DWORD a4, DWORD a5, DWORD a6, DWORD a7, DWORD a8, DWORD a9,
                                             DWORD a10, DWORD a11)
{
    for (int i = 0; i < 12; i++)
        self[i] = 0;
    self[0] = a0; self[1] = a1; self[2] = a2;
    self[3] = a3; self[4] = a4; self[5] = a5;
    self[6] = a6; self[7] = a7; self[8] = a8;
    self[9] = a9; self[10] = a10; self[11] = a11;
    return self;
}

// the value of an order (kind 4) at node *self for its side of the river
extern "C" double __fastcall Order_ValueAt(int* self, void*, int* o)
{
    if (o[8] != 4)
        return 0.0;
    double w = (strcmp((const char*)o[5], &g_strTrailerStorm) == 0 ? g_64b710 : 1.0) * F(double, o, 112);
    int v = (int)o;
    CallC(FUN_477b60, &v);
    double r = w;
    if (v != (int)g_players) {
        BYTE* q = (BYTE*)g_vehicles[F(int, F(BYTE*, v, 516), 4)];
        int* e = F(int*, q, 100);
        int* p = F(int*, q, 96);
        if (p != e) {
            while (p != e && *p != o[0])
                p++;
            if (p != e) {
                w = w + w;
                r = w;
            }
        }
    }
    if (*self == -1)
        return r;
    int t;
    Town_NearestOpen(&t, (const DVec3*)((BYTE*)g_roadNodes[*self] + 48));
    bool c = F(BYTE, g_towns[t], 96) & 1;
    bool a = F(BYTE, o, 4) & 1;
    if (c ? a : !a)
        return w * g_64b708;
    return 0.0;
}

// whether one of the player's orders goes to the town TOWN_AA (flag 0x8000)
extern "C" void __cdecl Player_CheckTownAA(BYTE* pl)
{
    F(DWORD, pl, 96) &= ~0x8000u;
    BYTE* v = Player_Orders(pl, pl);
    BYTE* it = *(BYTE**)v;
    BYTE* last = pl;
    for (;;) {
        last = Player_Orders(pl, last);
        if (it == F(BYTE*, last, 4))
            return;
        BYTE* town = (BYTE*)g_towns[F(int, it, 12)];
        char* s = F(char*, town, 0);
        if (s == F(char*, town, 4))
            s = 0;
        if (strcmp(s, &g_strTownAA) == 0)
            F(DWORD, pl, 96) |= 0x8000;
        it += 144;
    }
}

// ---------------------------------------------------------------------------
// medium-sized functions, batch 2

extern "C" BYTE* __cdecl Player_OfIt(void* it);         // the player of a list iterator (by value)
KING_ALIAS("_Player_OfIt", "_AI_ListData")
extern "C" int __cdecl Vehicle_Exists76(int vrId);
KING_ALIAS("_Vehicle_Exists76", "_Vehicle_Exists")
extern "C" void __cdecl Player_Drop422210(BYTE* pl);
KING_ALIAS("_Player_Drop422210", "FUN_422210")
extern "C" void __cdecl Vehicle_Distances(int i, double* a, double* b);
KING_ALIAS("_Vehicle_Distances", "_Vehicle_Costs")
extern "C" const char* __cdecl AI_Text76(const char* key);
KING_ALIAS("_AI_Text76", "FUN_40e270")
extern "C" const char* __cdecl AI_Text40e4c0(const char* key);
KING_ALIAS("_AI_Text40e4c0", "FUN_40e4c0")
extern "C" const char* __cdecl Node_Name(int node);
KING_ALIAS("_Node_Name", "FUN_436490")
extern "C" void __cdecl Str_56ae60(const char* s);
KING_ALIAS("_Str_56ae60", "_Messages_Add")
extern "C" void __cdecl Item_41cff0(void* p);
KING_ALIAS("_Item_41cff0", "FUN_41cff0")
extern "C" void __cdecl Item_41f970(void* p);
KING_ALIAS("_Item_41f970", "FUN_41f970")
extern "C" void __cdecl Obj_483c10(void* p);
KING_ALIAS("_Obj_483c10", "_AI_UpdateSwitch")
extern "C" void __cdecl Edge_SetFlag446ce0(int edge, int flag);
KING_ALIAS("_Edge_SetFlag446ce0", "_Road_SetObjectFlags")
ASM_PROC(FUN_452900)    // vector<void*>::insert(pos, &value)
ASM_PROC(FUN_458cf0)    // vector<char>: allocate and copy (n, first, last)
ASM_PROC(FUN_404420)
ASM_PROC(FUN_602ca0)
ASM_VAR(double, g_66c6f0, $L_66c6f0)
ASM_VAR(double, g_68d180, $L_68d180)
ASM_VAR(AiText, g_positionText76, $L_77e6e0)

// the AI generator's next value (the table at $L_68b9c8, indices after it)
static unsigned AiRandom()
{
    unsigned* t = (unsigned*)g_rng68b9c8;
    unsigned i = (t[55] + 1) % 55;
    unsigned j = (t[56] + 1) % 55;
    t[55] = i;
    t[56] = j;
    t[i] -= t[j];
    return t[t[55]];
}

// players whose vehicle is far from the camera drop it
extern "C" void __cdecl Players_DropFar()
{
    void** head = (void**)g_players;
    for (void** n = (void**)*head; n != g_players; n = (void**)*n) {
        if (!Player_OfIt(n))
            continue;
        BYTE* p = Player_OfIt(n);
        if (F(int, p, 84) != 9)
            continue;
        DVec3 v = *(DVec3*)(p + ((F(BYTE, p, 88) & 1) ? 616 : 304));
        double dx = g_68bab0.x - v.x;
        double dy = g_68bab0.y - v.y;
        double dz = g_68bab0.z - v.z;
        if (!(x87_sqrt(dy * dy + dz * dz + dx * dx) > 700.0) && F(int, p, 76) != -1
            && Vehicle_Exists76(F(int, p, 76)) != 0)
            continue;
        Player_Drop422210(p);
    }
}

extern "C" void __cdecl AI_RandomTraffic()
{
    int n = (int)(g_66c6f0 * 3.1 + 4.0);
    unsigned r = AiRandom();
    g_68d15c = r % (unsigned)(n - 2) + 3;
    g_68d160 = AiRandom() & 3;
}

// switches a record on or off: its name and items, then its edge's objects
extern "C" int __cdecl Rec_SetActive(BYTE* s, char on)
{
    if (on) {
        if (!F(char, s, 28)) {
            char* b = F(char*, s, 88);
            if (b != F(char*, s, 92) && strlen(b))
                Str_56ae60(b);
            for (void** p = F(void**, s, 112); p != F(void**, s, 116); p++)
                Item_41cff0(*p);
        }
    } else if (F(char, s, 28)) {
        char* b = F(char*, s, 100);
        if (b != F(char*, s, 104) && strlen(b))
            Str_56ae60(b);
        for (void** p = F(void**, s, 112); p != F(void**, s, 116); p++)
            Item_41f970(*p);
    }
    F(char, s, 28) = on;
    Obj_483c10(s ? s + 12 : 0);
    int e = F(int, s, 56);
    if (e != -1) {
        if (on) {
            Edge_SetFlag446ce0(e, 64);
            return 0;
        }
        for (BYTE** p = F(BYTE**, g_roadEdges[e], 120); p != F(BYTE**, g_roadEdges[e], 124); p++)
            if (!(F(DWORD, *p, 8) & 128))
                F(DWORD, *p, 8) &= ~64u;
    }
    return 0;
}

// operator= of a record: an int and a vector<char>
extern "C" int* __fastcall Rec42bfe0_Assign(int* self, void*, const int* src)
{
    self[0] = src[0];
    char** v = (char**)(self + 1);
    char* const* s = (char* const*)(src + 1);
    if ((void*)s == (void*)v)
        return self;
    char* first = s[0];
    char* last = s[1];
    unsigned n = last - first;
    if (n > (unsigned)(v[2] - v[0])) {
        char* p = CallThis<char*>(FUN_458cf0, v, n, first, last);
        if (v[2] - v[0])
            List_FreeNode(v[0], v[2] - v[0]);
        v[0] = p;
        v[2] = p + n;
        v[1] = v[0] + n;
    } else if ((unsigned)(v[1] - v[0]) >= n) {
        memmove(v[0], first, last - first);
        v[1] = v[0] + n;
    } else {
        memmove(v[0], first, v[1] - v[0]);
        char* mid = s[0] + (v[1] - v[0]);
        memmove(v[1], mid, s[1] - mid);
        v[1] = v[0] + n;
    }
    return self;
}

// the vehicles' average ratio of their two distances (under 2000)
extern "C" void __cdecl Vehicles_AverageRatio()
{
    int count = 0;
    g_68d180 = 0;
    for (unsigned i = 0; i < (unsigned)(g_vehiclesEnd - g_vehicles); i++) {
        double a, b;
        Vehicle_Distances(i, &a, &b);
        if (x87_eq(a, 0.0) || x87_eq(b, 0.0))
            continue;
        int type = F(int, g_vehicles[i], 32);
        if (type == 3 || type == 1)
            continue;
        double q = a / b;
        if (!(q >= 2000.0)) {
            g_68d180 = g_68d180 + q;
            count++;
        }
    }
    for (unsigned i = 0; i < (unsigned)(g_vehiclesEnd - g_vehicles); i++) {
        if (F(int, g_vehicles[i], 32) != 3)
            continue;
        double x, y, w, a;
        Vehicle_Distances(i, &x, &y);
        Vehicle_Distances(*F(int*, g_vehicles[i], 96), &w, &a);
        double q = x / a;
        if (!(q >= 2000.0)) {
            g_68d180 = g_68d180 + q;
            count++;
        }
    }
    if (count)
        g_68d180 = g_68d180 / (double)count;
}

// the towns' marked items, and each ambush's links to the named ambushes
extern "C" void __cdecl Ambushes_Link()
{
    for (void** t = g_towns; t != g_townsEnd; t++)
        for (BYTE* it = F(BYTE*, *t, 28); it != F(BYTE*, *t, 32); it += 16)
            if (F(BYTE, it, 4) & 1)
                F(int, *t, 64) = F(int, it, 0);
    for (void** a = g_68a970; a != g_68a974; a++) {
        BYTE* obj = (BYTE*)*a;
        for (BYTE* it = F(BYTE*, obj, 208); it != F(BYTE*, obj, 212); it += 12, obj = (BYTE*)*a) {
            const char* name = F(const char*, it, 0);
            void** q = g_68a970;
            for (; q != g_68a974; q++) {
                CharVec* n = (CharVec*)*q;
                if (n->begin != n->end && n->begin && !strcmp(n->begin, name))
                    break;
            }
            if (q == g_68a974) {
                AiError(0, 100, s_mainFile, s_mainId, 12598, "No such ambush ", name, &g_aiEmpty);
                continue;
            }
            void** end = F(void**, obj, 224);
            if (end != F(void**, obj, 228)) {
                if (end)
                    *end = *q;
                F(void**, obj, 224) = end + 1;
            } else
                CallThis(FUN_452900, obj + 220, end, q);
        }
    }
}

// the value of a player's orders
extern "C" double __cdecl Player_OrdersValue(BYTE* pl)
{
    double sum = 0;
    BYTE* v = Player_Orders(pl, 0);
    BYTE* it = *(BYTE**)v;
    for (;;) {
        v = Player_Orders(pl, v);
        if (it == F(BYTE*, v, 4))
            break;
        int k = F(int, it, 4);
        if (k != -1)
            sum = F(double, g_68ccb8, k * 104 + 80) * 15000.0 + sum;
        it += 144;
    }
    return sum;
}

// the texts of a fine's violations
extern "C" void __cdecl Fine_ViolationText(BYTE* f, char* out)
{
    if (F(BYTE, f, 16) & 1)
        strcpy(out, AI_Text40e4c0("VIOLATION_WRONG_SIDE"));
    if (F(BYTE, f, 16) & 2)
        strcpy(out, AI_Text40e4c0("VIOLATION_REVERSE_GEAR"));
    if (F(BYTE, f, 16) & 4)
        strcpy(out, AI_Text40e4c0("VIOLATION_ILLEGAL_PARKING"));
    if (F(BYTE, f, 16) & 8)
        strcpy(out, AI_Text40e4c0("VIOLATION_CROSSING_FULL_LINE"));
    if (F(BYTE, f, 16) & 16)
        strcpy(out, AI_Text40e4c0("VIOLATION_CHANGING_LANE"));
    if (F(BYTE, f, 16) & 32)
        strcpy(out, AI_Text76("VIOLATION_NEGLIGENCE"));
}

// whether a player has an order with a cargo
extern "C" bool __cdecl Player_HasCargoOrder(BYTE* pl)
{
    BYTE* v = Player_Orders(pl, pl);
    BYTE* it = *(BYTE**)v;
    v = pl;
    for (;;) {
        v = Player_Orders(pl, v);
        if (it == F(BYTE*, v, 4))
            return false;
        if (F(int, it, 4) != -1)
            return true;
        it += 144;
    }
}

// the town, index and kind of the nodes of each town's list
extern "C" void __cdecl Towns_MarkNodes()
{
    for (void** t = g_towns; t != g_townsEnd; t++) {
        BYTE* town = (BYTE*)*t;
        int v = F(int, town, 24);
        int i = 0;
        for (BYTE* it = F(BYTE*, town, 28); it != F(BYTE*, town, 32); it += 16, i++) {
            int n = F(int, it, 0);
            if (n == -1)
                continue;
            F(int, g_roadNodes[n], 168) = t - g_towns;
            DWORD f = F(DWORD, it, 4);
            if (f & 1)
                F(DWORD, g_roadNodes[n], 12) |= 2;
            else if (!(f & 8))
                F(DWORD, g_roadNodes[n], 12) |= 1;
            F(int, g_roadNodes[n], 180) = i;
            F(int, g_roadNodes[n], 176) = v;
        }
    }
}

// the parking places' nodes
extern "C" void __cdecl Parkings_MarkNodes()
{
    for (RoadNode** n = g_roadNodes; n != g_roadNodesEnd; n++)
        F(int, *n, 172) = -1;
    for (void** p = g_68cc98; p != g_68cc9c; p++) {
        int idx = p - g_68cc98;
        BYTE* pk = (BYTE*)g_68cc98[idx];
        for (unsigned k = 0; k < (unsigned)((F(BYTE*, pk, 32) - F(BYTE*, pk, 28)) / 36); k++) {
            int n = F(int, F(BYTE*, pk, 28) + k * 36, 0);
            if (F(int, g_roadNodes[n], 172) != -1)
                AiError(0, 100, s_mainFile, s_mainId, 13766, "Node used for parking, twice:", Node_Name(n), &g_aiEmpty);
            F(int, g_roadNodes[n], 172) = idx;
            F(DWORD, g_roadNodes[n], 12) |= 4;
            F(int, g_roadNodes[n], 180) = k;
            F(int, g_roadNodes[n], 176) = F(int, g_68cc98[idx], 24);
        }
    }
}

// the point of a position: its node's, or its road object's at t
extern "C" DVec3* __cdecl Road_PointOf(DVec3* out, PositionId p)
{
    PositionId c = p;
    if ((!RoadId_Valid(&c.road, 0) && !NodeId_Valid(&c.node, 0)) || !(c.t >= -0.0002) || c.t > 1.0002
        || c.segment < 0 || c.object < 0) {
        sprintf(g_positionText76, "(%d %g %d %d %x)", p.road, p.t, p.segment, p.object, p.m_24);
        AiError(33, 100, s_mainFile, s_mainId, 14389, "Invalid id", "PositionId", g_positionText76);
    }
    if (p.road >= 0 && (unsigned)p.road < (unsigned)(g_roadEdgesEnd - g_roadEdges)) {
        RoadEdge* e = g_roadEdges[p.road];
        if (!(p.object < e->objectsEnd - e->objects && p.segment < e->objects[p.object]->SegmentCount()))
            AiError(33, 100, s_mainFile, s_mainId, 14389, "Invalid Position Id", &g_aiEmpty, &g_aiEmpty);
    }
    if (p.node != -1) {
        *out = *(DVec3*)((BYTE*)g_roadNodes[p.node] + 48);
        return out;
    }
    CallVirt(g_roadEdges[p.road]->objects[p.object], 0, out, p.t, p.segment);
    return out;
}

// a service's info: two ids, the position as floats and its 304-byte table
extern "C" int __cdecl Service_Info(int i, DWORD* out)
{
    static const char sig[] = "getServiceInfo(ServiceId service, ServiceInfo & si)";
    StateDump& d = g_stateDump;
    if (d.data && d.pos + 8 < d.size) {
        memcpy(d.data + d.pos, &g_strStart, 8);
        d.pos += 8;
    }
    Buf_AppendString(&g_stateDump, 0, sig);
    BYTE* r = (BYTE*)g_68cbe8[i];
    float x = (float)F(double, r, 336);
    float y = (float)F(double, r, 344);
    float z = (float)F(double, r, 352);
    ((float*)out)[2] = x;
    ((float*)out)[3] = y;
    ((float*)out)[4] = z;
    out[0] = F(DWORD, g_68cbe8[i], 328);
    out[1] = F(DWORD, g_68cbe8[i], 332);
    memcpy(out + 6, g_68cbe8[i], 304);
    if (d.data && d.pos + 8 < d.size) {
        memcpy(d.data + d.pos, &g_strCompl, 8);
        d.pos += 8;
    }
    if (d.data)
        for (const char* s = sig; *s && d.pos < d.size;)
            d.data[d.pos++] = *s++;
    return 0;
}

// destructors of vectors of 24- and 16-byte records
extern "C" void __fastcall Vec24_Destroy(BYTE** self, void*)
{
    for (BYTE* p = self[0]; p != self[1]; p += 24)
        CallThis(FUN_404420, p + 8);
    int n = (self[2] - self[0]) / 24;
    if (n)
        StlDeallocate(self[0], n * 24);
}

extern "C" void __fastcall Vec16_Destroy(BYTE** self, void*)
{
    for (BYTE* p = self[0]; p != self[1]; p += 16)
        CallThis(FUN_602ca0, p);
    int n = (self[2] - self[0]) >> 4;
    if (n)
        StlDeallocate(self[0], n * 16);
}

// takes the allocator lock (a guard's constructor)
extern "C" void* __fastcall StlLock_Guard(void* self, void*)
{
    Stl_Lock(&g_stlLock, 0);
    return self;
}

// ---------------------------------------------------------------------------
// medium-sized functions, batch 3: copies of records with vectors

ASM_PROC(FUN_44c160)    // vector<double>::operator=
ASM_PROC(FUN_461dc0)    // vector of 16-byte records: allocate and copy (n, first, last)
ASM_PROC(FUN_469430)    // copy 16-byte records (first, last, dest, -, 0)
ASM_PROC(FUN_466300)    // copy-construct 16-byte records (first, last, dest, -)
ASM_PROC(FUN_460d10)    // destroy a 16-byte record
extern "C" BYTE* __fastcall Rec16_DeletingDestructor(BYTE* s, void*, int flags);   // aistl.cpp
ASM_VAR(BYTE, g_vt64b5c0, $L_64b5c0)
ASM_VAR(BYTE, g_vt64b62c, $L_64b62c)

// vector<char>::operator=, inlined; the split copy through memmove or Copy1
static void CharVec_Copy(char** v, char* const* s, bool copy1)
{
    char* first = s[0];
    char* last = s[1];
    unsigned n = last - first;
    if (n > (unsigned)(v[2] - v[0])) {
        char* p = CallThis<char*>(FUN_458cf0, v, n, first, last);
        if (v[2] - v[0])
            List_FreeNode(v[0], v[2] - v[0]);
        v[0] = p;
        v[2] = p + n;
    } else if ((unsigned)(v[1] - v[0]) >= n)
        memmove(v[0], first, last - first);
    else if (copy1) {
        Copy1((BYTE*)first, (BYTE*)first + (v[1] - v[0]), (BYTE*)v[0]);
        Copy1((BYTE*)s[0] + (v[1] - v[0]), (BYTE*)s[1], (BYTE*)v[1]);
    } else {
        memmove(v[0], first, v[1] - v[0]);
        char* mid = s[0] + (v[1] - v[0]);
        memmove(v[1], mid, s[1] - mid);
    }
    v[1] = v[0] + n;
}

static void Copy4(BYTE* d, const BYTE* s, int from, int to)
{
    for (int i = from; i <= to; i += 4)
        F(DWORD, d, i) = F(DWORD, s, i);
}

// operator= of a 192-byte record (three char/double vectors)
extern "C" BYTE* __fastcall Rec192_Assign(BYTE* self, void*, const BYTE* src)
{
    F(DWORD, self, 0) = F(DWORD, src, 0);
    Copy4(self, src, 8, 12);
    memcpy(self + 16, src + 16, 32);
    F(DWORD, self, 48) = F(DWORD, src, 48);
    if (src + 52 != self + 52)
        CharVec_Copy((char**)(self + 52), (char* const*)(src + 52), false);
    if (src + 64 != self + 64)
        CharVec_Copy((char**)(self + 64), (char* const*)(src + 64), true);
    CallThis(FUN_44c160, self + 76, src + 76);
    CallThis(FUN_44c160, self + 88, src + 88);
    Copy4(self, src, 100, 112);
    Copy4(self, src, 120, 180);
    F(BYTE, self, 184) = F(BYTE, src, 184);
    return self;
}

// copies [first, last) of 56-byte records over dest
extern "C" BYTE* __cdecl Rec56_Copy(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = (last - first) / 56; n > 0; n--, first += 56, dest += 56) {
        F(DWORD, dest, 0) = F(DWORD, first, 0);
        if (first + 4 != dest + 4)
            CharVec_Copy((char**)(dest + 4), (char* const*)(first + 4), false);
        Copy4(dest, first, 16, 32);
        Copy4(dest, first, 40, 52);
    }
    return dest;
}

// the vector of 16-byte records at +120 of a 144-byte order
static void Orders_CopyVec(BYTE** d, BYTE* const* s, bool destroy461ae0, int junk)
{
    unsigned n = (s[1] - s[0]) >> 4;
    if (n > (unsigned)((d[2] - d[0]) >> 4)) {
        BYTE* p = CallThis<BYTE*>(FUN_461dc0, d, n, s[0], s[1]);
        for (BYTE* e = d[0]; e != d[1]; e += 16)
            if (destroy461ae0)
                Rec16_DeletingDestructor(e, 0, 0);
            else
                CallC(FUN_460d10, e);
        if ((d[2] - d[0]) >> 4)
            List_FreeNode(d[0], ((d[2] - d[0]) >> 4) << 4);
        d[0] = p;
        d[2] = p + n * 16;
    } else if ((unsigned)((d[1] - d[0]) >> 4) >= n) {
        BYTE* e = CallC<BYTE*>(FUN_469430, s[0], s[1], d[0], destroy461ae0 ? (int)n : junk, 0);
        for (; e != d[1]; e += 16)
            if (destroy461ae0)
                Rec16_DeletingDestructor(e, 0, 0);
            else
                CallC(FUN_460d10, e);
    } else {
        int size = (d[1] - d[0]) >> 4;
        BYTE* from = s[0];
        if (destroy461ae0) {
            BYTE* to = d[0];
            for (int k = size; k > 0; k--, from += 16, to += 16)
                Rec42bfe0_Assign((int*)to, 0, (const int*)from);
        } else
            CallC(FUN_469430, s[0], s[0] + size * 16, d[0], junk, 0);
        CallC(FUN_466300, s[0] + ((d[1] - d[0]) >> 4) * 16, s[1], d[1], destroy461ae0 ? (int)from : junk);
    }
    d[1] = d[0] + n * 16;
}

static void Order144_Assign(BYTE* d, const BYTE* s, bool destroy461ae0)
{
    Copy4(d, s, 0, 16);
    memcpy(d + 24, s + 24, 32);
    Copy4(d, s, 56, 104);
    Copy4(d, s, 112, 116);
    if (s + 120 != d + 120)
        Orders_CopyVec((BYTE**)(d + 120), (BYTE* const*)(s + 120), destroy461ae0, 0);
    Copy4(d, s, 132, 136);
}

// operator= of a 144-byte order
extern "C" BYTE* __fastcall Order144_AssignThis(BYTE* self, void*, const BYTE* src)
{
    Order144_Assign(self, src, true);
    return self;
}

// copies [first, last) of 144-byte orders over dest
extern "C" BYTE* __cdecl Order144_Copy(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = (last - first) / 144; n > 0; n--, first += 144, dest += 144)
        Order144_Assign(dest, first, false);
    return dest;
}

// destructor of a record with two vectors (24- and 8-byte elements)
extern "C" void __fastcall Rec458940_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vt64b5c0;
    BYTE* a = self ? self + 56 : 0;
    BYTE* b = F(BYTE*, a, 32);
    int n = (F(BYTE*, a, 40) - b) / 24;
    if (n)
        StlDeallocate(b, n * 24);
    F(void*, self, 0) = &g_vt64b62c;
    a = self ? self + 8 : 0;
    b = F(BYTE*, a, 32);
    n = (F(BYTE*, a, 40) - b) >> 3;
    if (n)
        StlDeallocate(b, n * 8);
}

// ---------------------------------------------------------------------------
// medium-sized functions, batch 4: vector erase, order lists, the name hash

ASM_PROC(FUN_40c1a0)    // destroy a 104-byte record
ASM_PROC(FUN_40cb60)    // destroy a 24-byte record
ASM_PROC(FUN_45aa00)    // operator= of the vector at +12 of a 24-byte record
ASM_PROC(FUN_440dd0)    // destroy a 192-byte record
ASM_PROC(FUN_4618c0)    // vector of 16-byte records: operator=
ASM_PROC(FUN_41ce50)    // destroy a 144-byte order
ASM_PROC(FUN_44fac0)    // hash table: grow for n entries
ASM_PROC(FUN_44f2c0)    // a string's character (s, i)
ASM_PROC(FUN_45a7d0)    // compare two strings
ASM_PROC(FUN_4592b0)    // hash table: a new node for a key
ASM_PROC(FUN_461b60)    // vector of 24-byte records: allocate and copy (n, first, last)
ASM_PROC(FUN_46c490)    // destroy a 24-byte record
ASM_PROC(FUN_466c50)    // copy 24-byte records (first, last, dest, -, 0)
ASM_PROC(FUN_466200)    // copy-construct 24-byte records (first, last, dest, -)
extern "C" double __cdecl Order_Value(const int* o);        // aiordr.cpp
extern "C" double __cdecl Order_FullValue(const int* o);

// vector<104-byte record>::erase(first, last)
extern "C" BYTE* __fastcall Vec104_Erase(BYTE** self, void*, BYTE* first, BYTE* last)
{
    BYTE* d = first;
    BYTE* last0 = last;
    for (int n = (self[1] - last) / 104; n > 0; n--, d += 104, last += 104) {
        Str_Assign((StrRef*)d, 0, (const StrRef*)last);
        Copy4(d, last, 16, 48);
        Copy4(d, last, 56, 72);
        Copy4(d, last, 80, 84);
        CallThis(FUN_44c160, d + 88, last + 88);
    }
    for (BYTE* e = self[1]; d != e; d += 104)
        CallThis(FUN_40c1a0, d);
    self[1] -= ((last0 - first) / 104) * 104;
    return first;
}


// vector<24-byte record>::erase(first, last)
extern "C" BYTE* __fastcall Vec24_Erase(BYTE** self, void*, BYTE* first, BYTE* last)
{
    BYTE* d = first;
    BYTE* s = last;
    for (int n = (self[1] - last) / 24; n > 0; n--, d += 24, s += 24) {
        ((StrRef*)d)->Assign((const StrRef*)s);
        CallThis(FUN_45aa00, d + 12, s + 12);
    }
    for (BYTE* e = self[1]; d != e; d += 24)
        CallThis(FUN_40cb60, d);
    self[1] -= ((last - first) / 24) * 24;
    return first;
}

// vector<192-byte record>::erase(first, last)
extern "C" BYTE* __fastcall Vec192_Erase(BYTE** self, void*, BYTE* first, BYTE* last)
{
    BYTE* d = first;
    BYTE* s = last;
    for (int n = (self[1] - last) / 192; n > 0; n--, d += 192, s += 192) {
        F(DWORD, d, 0) = F(DWORD, s, 0);
        Copy4(d, s, 8, 12);
        memcpy(d + 16, s + 16, 32);
        F(DWORD, d, 48) = F(DWORD, s, 48);
        CallThis(FUN_44c160, d + 52, s + 52);
        CallThis(FUN_44c160, d + 64, s + 64);
        CallThis(FUN_44c160, d + 76, s + 76);
        CallThis(FUN_44c160, d + 88, s + 88);
        Copy4(d, s, 100, 112);
        Copy4(d, s, 120, 180);
        F(BYTE, d, 184) = F(BYTE, s, 184);
        Rec_FirstNonEmpty(d, 0);
    }
    for (BYTE* e = self[1]; d != e; d += 192)
        CallThis(FUN_440dd0, d);
    self[1] -= ((last - first) / 192) * 192;
    return first;
}

// removes the orders that match from a list, adding up their values
static bool Orders_RemoveIf(BYTE** v, double* sum, bool expired)
{
    bool removed = false;
    for (;;) {
        BYTE* it = v[0];
        for (; it != v[1]; it += 144)
            if (expired ? AI_Now() > F(double, it, 72) : !(F(double, it, 96) >= 0.01))
                break;
        if (it == v[1])
            return removed;
        double x = expired ? Order_FullValue((const int*)it) : Order_Value((const int*)it);
        *sum = x + *sum;
        BYTE* d = it;
        for (int n = (v[1] - (it + 144)) / 144; n > 0; n--, d += 144) {
            BYTE* s = d + 144;
            Copy4(d, s, 0, 16);
            memcpy(d + 24, s + 24, 32);
            Copy4(d, s, 56, 104);
            Copy4(d, s, 112, 116);
            CallThis(FUN_4618c0, d + 120, s + 120);
            Copy4(d, s, 132, 136);
        }
        v[1] -= 144;
        CallThis(FUN_41ce50, v[1]);
        removed = true;
    }
}

extern "C" bool __cdecl Orders_RemoveExpired(BYTE** v, double* sum) { return Orders_RemoveIf(v, sum, true); }
extern "C" bool __cdecl Orders_RemoveEmpty(BYTE** v, double* sum) { return Orders_RemoveIf(v, sum, false); }

// hash_set<string>::insert_unique: out = {node, table, inserted}
extern "C" void** __fastcall NameHash_Insert(BYTE* self, void*, void** out, int** key)
{
    CallThis(FUN_44fac0, self, F(int, self, 16) + 1);
    unsigned h = 0;
    int* s = *key;
    int len;
    if (s && (len = *s) != 0) {
        int a = CallC<char>(FUN_44f2c0, s, 0);
        int b = CallC<char>(FUN_44f2c0, *key, len - 1);
        h = a * 13 + b * 5 + len;
    }
    void** buckets = F(void**, self, 4);
    unsigned i = h % (unsigned)((F(BYTE*, self, 8) - F(BYTE*, self, 4)) >> 2);
    void** head = (void**)buckets[i];
    for (void** n = head; n; n = (void**)*n)
        if (CallC<int>(FUN_45a7d0, n[1], *key) == 0) {
            out[0] = n;
            out[1] = self;
            F(BYTE, out, 8) = 0;
            return out;
        }
    void** n = CallThis<void**>(FUN_4592b0, self, key);
    *n = head;
    F(void**, self, 4)[i] = n;
    F(int, self, 16)++;
    out[0] = n;
    out[1] = self;
    F(BYTE, out, 8) = 1;
    return out;
}

// copies [first, last) of 24-byte records (a string and a vector) over dest
extern "C" BYTE* __cdecl Rec24_Copy(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int k = (last - first) / 24; k > 0; k--, first += 24, dest += 24) {
        ((StrRef*)dest)->Assign((const StrRef*)first);
        BYTE** d = (BYTE**)(dest + 12);
        BYTE* const* s = (BYTE* const*)(first + 12);
        if ((const void*)s == (void*)d)
            continue;
        unsigned n = (s[1] - s[0]) / 24;
        if (n > (unsigned)((d[2] - d[0]) / 24)) {
            BYTE* p = CallThis<BYTE*>(FUN_461b60, d, n, s[0], s[1]);
            for (BYTE* e = d[0]; e != d[1]; e += 24)
                CallC(FUN_46c490, e);
            if ((d[2] - d[0]) / 24)
                List_FreeNode(d[0], ((d[2] - d[0]) / 24) * 24);
            d[0] = p;
            d[2] = p + n * 24;
        } else if ((unsigned)((d[1] - d[0]) / 24) >= n) {
            BYTE* e = CallC<BYTE*>(FUN_466c50, s[0], s[1], d[0], 0, 0);
            for (; e != d[1]; e += 24)
                CallC(FUN_46c490, e);
        } else {
            CallC(FUN_466c50, s[0], s[0] + ((d[1] - d[0]) / 24) * 24, d[0], 0, 0);
            CallC(FUN_466200, s[0] + ((d[1] - d[0]) / 24) * 24, s[1], d[1], 0);
        }
        d[1] = d[0] + n * 24;
    }
    return dest;
}
