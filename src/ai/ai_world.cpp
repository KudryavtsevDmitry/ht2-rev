// ai_world.cpp - Ai_main.cpp of the AI, rewritten from king.masm: its small
// functions: the reference-counted string handle, positions and their
// checks, the nearest node or town to a point, lookups of towns and
// vehicles, the AI's random generator, and the constructors of its records.

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "road.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" void* __cdecl memmove(void* dst, const void* src, size_t n);

static const char s_mainId[] = "$Id: Ai_main.cpp 2001/10/16 14:29:56";
static const char s_mainFile[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\Ai_main.cpp";

extern "C" void __cdecl AI_Error4a52e0(int code, int a);
KING_ALIAS("_AI_Error4a52e0", "_Sch_ErrorCode")
extern "C" int __cdecl Loading_Step();    // carat.cpp
extern "C" void** __cdecl Towns_Find453bd0(void** first, void** last, int key, void* rng);
extern "C" void** __cdecl Towns_Find453d70(void** first, void** last, int a, int b, int c);
extern "C" void** __cdecl Towns_Find453f10(void** first, void** last, int a, int b);
KING_ALIAS("_Towns_Find453f10", "_Pick_RandomOther")
extern "C" void** __cdecl Towns_Find4540a0(void** first, void** last, int* a, int b);
KING_ALIAS("_Towns_Find4540a0", "_Pick_RandomIdle")
extern "C" void** __cdecl Towns_Find454230(void** first, void** last, int a, int b, void* rng);
KING_ALIAS("_Towns_Find454230", "_Towns_PickWeighted")
extern "C" DVec3* __cdecl Town_Position(DVec3* out, void* town);
KING_ALIAS("_Town_Position", "_Town_Point")
extern "C" DVec3* __cdecl Road_PointOf(DVec3* out, PositionId pos);
extern "C" bool __cdecl SchValue_IsSymbol(const BYTE* v, const char* name);   // schitem.cpp
extern "C" void __cdecl Player_UpdateFlags(BYTE* p);        // aiplay.cpp
extern "C" void __fastcall CharVec_Assign(char** self, void*, const char* s);    // aiordr.cpp
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert(pos, first, last)

typedef void** PtrVec;
KING_VAR(PtrVec, g_towns, 68cec0)
KING_VAR(PtrVec, g_townsEnd, 68cec4)
KING_VAR(PtrVec, g_vehicles, 68cfd8)
KING_VAR(PtrVec, g_vehiclesEnd, 68cfdc)
KING_VAR(PtrVec, g_68cc98, 68cc98)
KING_VAR(PtrVec, g_68cc9c, 68cc9c)
KING_VAR(int, g_carsTotal, 68d140)
KING_VAR(int, g_68cef8, 68cef8)
KING_VAR(int, g_68d150, 68d150)
KING_VAR(int, g_68cee8, 68cee8)
typedef int Ints3[3];
KING_VAR(Ints3, g_68ced0, 68ced0)
KING_VAR(int, g_uniqueCounter, 68d158)
typedef char AiText[1];
KING_VAR(AiText, g_uniqueName, 68bad4)
KING_VAR(AiText, g_uniqueName2, 68d020)
KING_VAR(AiText, g_rng68b9c8, 68b9c8)
KING_VAR(BYTE*, g_68ccb8, 68ccb8)
KING_VAR(void*, g_players, 68b990)
struct VecOfPtr { void** begin; void** end; void** cap; };
KING_VAR(VecOfPtr*, g_68cbf8, 68cbf8)

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

typedef char AsmLabel[1];
extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")    // the STL node allocator: allocate (bytes)
extern "C" void FUN_480550(); KING_ALIAS("_FUN_480550", "@SvRec480550_Construct@12")    // 52-byte record constructor (name)
extern "C" void FUN_44d4e0(); KING_ALIAS("_FUN_44d4e0", "@Vec4b_Insert44d4e0@16")    // vector<pointer>::insert_aux
extern "C" void FUN_480580(); KING_ALIAS("_FUN_480580", "@SvRec480580_Construct@12")    // 24-byte record constructor (name)
extern "C" void FUN_44d7a0(); KING_ALIAS("_FUN_44d7a0", "@Vec4b_Insert44d7a0@16")
extern "C" void FUN_46b0e0(); KING_ALIAS("_FUN_46b0e0", "_UninitCopyDwords")
extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")    // script error (kind, fatal, got, expected)
extern "C" void FUN_4b59f0(); KING_ALIAS("_FUN_4b59f0", "@FilePort_Construct@16")    // port constructor
extern "C" void FUN_4ad2f0(); KING_ALIAS("_FUN_4ad2f0", "_Pool32_New")    // pool allocation
extern "C" void FUN_4b0b60(); KING_ALIAS("_FUN_4b0b60", "@SchNumber_FromInt@16")    // number constructor
extern "C" void FUN_407f00(); KING_ALIAS("_FUN_407f00", "_Str_Member20")
extern "C" void FUN_4b65e0(); KING_ALIAS("_FUN_4b65e0", "_SchValue_IsSymbol")
KING_VAR(BYTE**, g_registry52, 68d0d8)
KING_VAR(BYTE**, g_registry52End, 68d0dc)
KING_VAR(BYTE**, g_registry52Cap, 68d0e0)
KING_VAR(BYTE**, g_registry24, 68d0c8)
KING_VAR(BYTE**, g_registry24End, 68d0cc)
KING_VAR(BYTE**, g_registry24Cap, 68d0d0)
KING_VAR(AsmLabel, s_pair, 66cb08)
KING_VAR(AsmLabel, s_number, 66ca60)
KING_VAR(AsmLabel, s_string, 66ca58)
KING_VAR(AsmLabel, s_port, 66cb28)

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // the AI's allocator: free (p, size)
extern "C" void FUN_447970(); KING_ALIAS("_FUN_447970", "@Vec4_FreeStorage@8")
extern "C" void FUN_410e40(); KING_ALIAS("_FUN_410e40", "@Rec410e40_FreeVectors@8")
ASM_FUNC(FUN_466200, _Rec24_UninitCopy)    // uninitialized copy of 24-byte records
extern "C" void FUN_4090a0(); KING_ALIAS("_FUN_4090a0", "_Str_Member32")
extern "C" void FUN_4a52e0(); KING_ALIAS("_FUN_4a52e0", "_Sch_ErrorCode")    // script error (code, fatal)
ASM_FUNC(FUN_401000, _Ai_Clock)    // the AI clock, returns double
extern "C" void FUN_4172e0(); KING_ALIAS("_FUN_4172e0", "_Player_UpdateFlagsGet")
extern "C" void FUN_417530(); KING_ALIAS("_FUN_417530", "_Player_Leave")
extern "C" void FUN_483cb0(); KING_ALIAS("_FUN_483cb0", "_displayScreenMessage")    // shows a screen message
KING_VAR(BYTE*, g_aiMessages, 68a850)
KING_VAR(BYTE*, g_aiMessagesEnd, 68a854)
KING_VAR(AsmLabel, s_noMessage, 66cdd8)
KING_VAR(AsmLabel, g_trace, 71e808)
KING_VAR(BYTE*, g_traceData, 71e814)
KING_VAR(int, g_traceSize, 71e810)
KING_VAR(int, g_tracePos, 71e818)
KING_VAR(AsmLabel, g_tagArt, 66c830)
KING_VAR(AsmLabel, g_tagMpl, 66c7ec)
KING_VAR(AsmLabel, s_finishRacingPlayer, 66cc8c)
KING_VAR(AsmLabel, s_competitorArrived, 66cc78)
extern "C" bool __fastcall Item_Matches(BYTE** self, void*, BYTE* x);

extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")    // the object of a list iterator
extern "C" void FUN_427220(); KING_ALIAS("_FUN_427220", "_Players_Forget")
extern "C" void FUN_4566c0(); KING_ALIAS("_FUN_4566c0", "_Player_NotValid")    // whether a player id is invalid
extern "C" void FUN_436ef0(); KING_ALIAS("_FUN_436ef0", "_Ai_GetPositionIdVRoom")
extern "C" void FUN_404c70(); KING_ALIAS("_FUN_404c70", "_Town_ById")    // the index of a VrIndex (out, id)
extern "C" void FUN_440970(); KING_ALIAS("_FUN_440970", "_Ai_GetOrderList")
extern "C" void FUN_441120(); KING_ALIAS("_FUN_441120", "_Ai_GetOrderInfoIdx")
KING_VAR(BYTE*, g_playersHead, 68b990)
KING_VAR(AsmLabel, s_clearPlayerFromMafia, 66da10)
KING_VAR(AsmLabel, s_clearPlayerFromTail, 66da38)
KING_VAR(AsmLabel, s_getPositionId, 66e57c)
KING_VAR(AsmLabel, s_setPlayerPositionV, 66eb28)
KING_VAR(AsmLabel, s_getTownInfo, 66eec8)
KING_VAR(AsmLabel, s_getOrderInfo, 66ef68)
KING_VAR(AsmLabel, g_numberText, 77e660)
KING_VAR(AsmLabel, s_idFormat, 66d4e4)
KING_VAR(AsmLabel, s_invalidId, 66cbf4)
KING_VAR(AsmLabel, s_playerIdType, 66d4d8)
KING_VAR(BYTE**, g_townsBegin, 68cec0)

extern "C" void FUN_4778a0(); KING_ALIAS("_FUN_4778a0", "_Player_ClearOrderLists")
extern "C" void FUN_442c60(); KING_ALIAS("_FUN_442c60", "_Ai_AccountForChangeInLoadState")
extern "C" void FUN_426280(); KING_ALIAS("_FUN_426280", "@Int_Set@12")    // the PlayerId of a list iterator (thiscall out, iterator)
extern "C" void FUN_4466a0(); KING_ALIAS("_FUN_4466a0", "_Ai_RemovePlayer")    // kills a player
extern "C" void FUN_466400(); KING_ALIAS("_FUN_466400", "_UninitFill16")    // uninitialized fill of 16-byte records
extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")    // uninitialized copy of 12-byte records
KING_VAR(AsmLabel, s_dropOrders, 66f2e0)
KING_VAR(AsmLabel, s_killDelayedPlayers, 66f3f0)

extern "C" void FUN_45aed0(); KING_ALIAS("_FUN_45aed0", "@Vec4b_InsertA@16")    // vector<void*>::insert(pos, &v)
extern "C" void FUN_45c610(); KING_ALIAS("_FUN_45c610", "@Vec4b_InsertB@16")
extern "C" void FUN_45d370(); KING_ALIAS("_FUN_45d370", "@Vec4b_InsertD@16")

extern "C" void FUN_458910(); KING_ALIAS("_FUN_458910", "@AI_Nop458910@8")    // destructor of a 1592-byte record
ASM_FUNC(FUN_466300, _Rec16_UninitCopy)    // uninitialized copy of 16-byte records
KING_VAR(AsmLabel, g_vtbl64b600, 64b600)
KING_VAR(AsmLabel, g_vtbl64b62c, 64b62c)

extern "C" void FUN_603540(); KING_ALIAS("_FUN_603540", "_Bytes_Copy")
extern "C" void FUN_467780(); KING_ALIAS("_FUN_467780", "_UninitCopyStrRefs")
extern "C" void FUN_4b6610(); KING_ALIAS("_FUN_4b6610", "_SchValue_Same")
extern "C" void FUN_418f70(); KING_ALIAS("_FUN_418f70", "_Player_WaitsInTown")
extern "C" void FUN_45dde0(); KING_ALIAS("_FUN_45dde0", "@Vec4b_InsertE@16")
extern "C" void FUN_4072f0(); KING_ALIAS("_FUN_4072f0", "_Copy1")
extern "C" void FUN_44dd90(); KING_ALIAS("_FUN_44dd90", "@VecChar_Copy@12")
extern "C" void FUN_46b110(); KING_ALIAS("_FUN_46b110", "_UninitFill4s")
extern "C" void FUN_462d10(); KING_ALIAS("_FUN_462d10", "_Str_ClearFirst@4")
extern "C" void FUN_40a580(); KING_ALIAS("_FUN_40a580", "@Pair_Set@16")
extern "C" void FUN_4d8a40(); KING_ALIAS("_FUN_4d8a40", "_Vehicle_Exists")

ASM_FUNC(FUN_45b190, _Find_ByHandle104)
extern "C" void FUN_45adc0(); KING_ALIAS("_FUN_45adc0", "_List_FindKey8")
extern "C" void FUN_45b220(); KING_ALIAS("_FUN_45b220", "_List_FindNear")
extern "C" void FUN_45ddb0(); KING_ALIAS("_FUN_45ddb0", "_List_FindKey24")
extern "C" void FUN_4607d0(); KING_ALIAS("_FUN_4607d0", "_List_FindName")

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
extern "C" void __cdecl AI_Grow421d90(int* entry, int a, int b);   // = Ai_CreateDealerForSale (double as two ints)
KING_ALIAS("_AI_Grow421d90", "_Ai_CreateDealerForSale")
extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "_Ai_Clock")
KING_VAR(PtrVec, g_68a980, 68a980)
KING_VAR(PtrVec, g_68a984, 68a984)
KING_VAR(BYTE*, g_68cca8, 68cca8)
typedef BYTE CarTypeBytes[1];
// g_carTypes (car.h) + 76, 2416 bytes per type: +100 here is the type's +176
KING_VAR(CarTypeBytes, g_carTypes76, 697914)
KING_VAR(AiText, g_strPresentPick, 66d7f8)
KING_VAR(AiText, g_vtbl64b71c, 64b71c)
KING_VAR(AiText, g_vtbl64b718, 64b718)
KING_VAR(AiText, g_undefinedModule, 66d638)
KING_VAR(AiText, g_undefinedRoom, 66d628)

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
KING_ALIAS("_Road_RoomInfo", "_Road_GetRoadParameters")

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
extern "C" double __cdecl Player_Value430760(BYTE* p);
KING_ALIAS("_Player_Value430760", "_Player_OrdersValue")
extern "C" double __cdecl Player_CargoValueOf(BYTE* p);     // aiplay.cpp
extern "C" void __cdecl Player_EndDeal(BYTE* p);             // aiplay.cpp
extern "C" void __cdecl AI_Task42b060(void* task, int a);
KING_ALIAS("_AI_Task42b060", "_Rec_SetActive")
extern "C" void __cdecl Screen_569310(int id, unsigned on);        // copter.cpp
extern "C" void __cdecl Screen_569410(int id, int on);             // copter.cpp

KING_VAR(int, g_68d188, 68d188)
KING_VAR(BYTE, g_68d168, 68d168)
KING_VAR(int, g_66c6e0, 66c6e0)
KING_VAR(int, g_66c6e4, 66c6e4)
KING_VAR(PtrVec, g_68cf60, 68cf60)
KING_VAR(PtrVec, g_68cf64, 68cf64)
KING_VAR(unsigned, g_66c6d0, 66c6d0)
KING_VAR(unsigned, g_68d164, 68d164)
KING_VAR(unsigned, g_68d160, 68d160)
KING_VAR(unsigned, g_68d15c, 68d15c)
KING_VAR(DVec3, g_68bab0, 68bab0)
KING_VAR(BYTE*, g_68cf28, 68cf28)
KING_VAR(double, g_68d190, 68d190)
KING_VAR(int, g_68d18c, 68d18c)
KING_VAR(PtrVec, g_68cc18, 68cc18)
KING_VAR(PtrVec, g_68cc1c, 68cc1c)
KING_VAR(PtrVec, g_68a970, 68a970)
KING_VAR(PtrVec, g_68a974, 68a974)
KING_VAR(AiText, g_str66da80, 66da80)
KING_VAR(AiText, g_strStartKey, 66da84)
KING_VAR(AiText, g_strSeeIfBankrupted, 66da00)
KING_VAR(AiText, g_strFineForBribing, 66e0b8)
KING_VAR(AiText, g_vtbl64b7f0, 64b7f0)
KING_VAR(AiText, g_vtbl64b8c0, 64b8c0)

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
// farther than 400 from the point (the player argument is unused)
extern "C" bool __cdecl AI_RoomForPlayer(BYTE*, const DVec3* p)
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
KING_ALIAS("_AI_42db90", "_AI_FreeList68b99c")
extern "C" void __cdecl AI_4031e0();
KING_ALIAS("_AI_4031e0", "_Ai_ClearWorld")
extern "C" void __cdecl AI_449c40(void* out, void* x);
KING_ALIAS("_AI_449c40", "_NameMap68cff0_Find")
extern "C" void __cdecl Nodes_FindByName(int* out, const char* name);
KING_ALIAS("_Nodes_FindByName", "_NameMap68d008_Find")
extern "C" void __cdecl Modules_FindByName(int* out, const char* name);
KING_ALIAS("_Modules_FindByName", "_NameMap68cf10_Find")
extern "C" void __cdecl Town_440390(void* out, void* town);
KING_ALIAS("_Town_440390", "_Ai_GetCoordinates")
extern "C" void __cdecl List_FreeNode(void* node, int size);
KING_ALIAS("_List_FreeNode", "_Ai_PoolFree")
extern "C" void __cdecl Copy104(void* dst, const void* src);
KING_ALIAS("_Copy104", "_Rec40c220_Construct")
extern "C" void __cdecl Copy24a(void* dst, const void* src);
KING_ALIAS("_Copy24a", "_Rec4581f0_Construct")
extern "C" void __cdecl Copy24b(void* dst, const void* src);
KING_ALIAS("_Copy24b", "_Rec40cda0_Construct")
extern "C" void FUN_44c470(); KING_ALIAS("_FUN_44c470", "@Vec8_Insert44c470@16")    // vector<8 bytes>::insert_aux
extern "C" void FUN_44da60(); KING_ALIAS("_FUN_44da60", "@Rec104Vec_InsertAux@16")    // vector<104 bytes>::insert_aux
extern "C" void FUN_44e3f0(); KING_ALIAS("_FUN_44e3f0", "@Vec4b_Insert44e3f0@16")    // vector<4 bytes>::insert_aux
extern "C" void FUN_44e970(); KING_ALIAS("_FUN_44e970", "@Vec4b_Insert44e970@16")
extern "C" void FUN_44eef0(); KING_ALIAS("_FUN_44eef0", "@Vec4b_Insert44eef0@16")
extern "C" void FUN_450330(); KING_ALIAS("_FUN_450330", "@Rec24Vec_InsertAux@16")    // vector<24 bytes>::insert_aux
extern "C" void FUN_4505a0(); KING_ALIAS("_FUN_4505a0", "@Rec24bVec_InsertAux@16")

KING_VAR(PtrVec, g_68cbe8, 68cbe8)
KING_VAR(PtrVec, g_68cbec, 68cbec)
KING_VAR(void*, g_68cf6c, 68cf6c)
typedef CharVec* NameList[1];
KING_VAR(NameList*, g_68d0d8, 68d0d8)
KING_VAR(AiText, g_vtbl64b920, 64b920)
KING_VAR(AiText, g_vtbl64b948, 64b948)
KING_VAR(AiText, g_strForfeit, 66f320)

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
extern "C" void __cdecl Copy36(void* dst, const void* src);
KING_ALIAS("_Copy36", "_Rec458500_Construct")
extern "C" void __cdecl Copy12(void* dst, const void* src);
KING_ALIAS("_Copy12", "_Bytes_Construct")
extern "C" void __cdecl Copy56(void* dst, const void* src);
KING_ALIAS("_Copy56", "_Rec458660_Construct")
extern "C" void __cdecl AI_Error4a54e0(int code, int a, const char* name);
KING_ALIAS("_AI_Error4a54e0", "_Sch_Error")
extern "C" void FUN_451230(); KING_ALIAS("_FUN_451230", "@Rec36Vec_InsertAux@16")    // vector<36 bytes>::insert_aux
extern "C" void FUN_4514b0(); KING_ALIAS("_FUN_4514b0", "@Vec4b_Insert4514b0@16")    // vector<4 bytes>::insert_aux
extern "C" void FUN_451740(); KING_ALIAS("_FUN_451740", "@Vec16_Insert451740@16")    // vector<16 bytes>::insert_aux
extern "C" void FUN_451a80(); KING_ALIAS("_FUN_451a80", "@Vec4b_Insert451a80@16")
extern "C" void FUN_451d40(); KING_ALIAS("_FUN_451d40", "@Vec4b_Insert451d40@16")
extern "C" void FUN_452000(); KING_ALIAS("_FUN_452000", "@Vec4b_Insert452000@16")
extern "C" void FUN_452610(); KING_ALIAS("_FUN_452610", "@CharVecVec_InsertAux@16")    // vector<12 bytes>::insert_aux
extern "C" void FUN_452bc0(); KING_ALIAS("_FUN_452bc0", "@Rec56Vec_InsertAux@16")    // vector<56 bytes>::insert_aux
extern "C" void FUN_44b0f0(); KING_ALIAS("_FUN_44b0f0", "_ListNode16_Make@4")    // list: a new node holding x
extern "C" void FUN_44b250(); KING_ALIAS("_FUN_44b250", "_ListNode16_MakeB@4")
extern "C" void FUN_44b310(); KING_ALIAS("_FUN_44b310", "_ListNode64_Make@4")
extern "C" void FUN_402ec0(); KING_ALIAS("_FUN_402ec0", "@RopeRef_Destroy@8")    // a hashtable value's destructor
KING_VAR(AiText, g_strPair, 66cb08)

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

KING_VAR(unsigned, g_spinCount, 66c83c)       // spins before sleeping: 30, or 1000 after a quick success
KING_VAR(unsigned, g_spinLast, 68d208)
KING_VAR(char, g_strAiMainFile, 66c7b0)       // "C:\Nek\Vrappl\Gi\Htai\Ai_main.cpp"
KING_VAR(char, g_strAiMainId, 66c558)         // "$Id: Ai_main.cpp ..."
KING_VAR(char, g_strErr05491, 66c840)         // "Internal error 05491"
KING_VAR(BYTE**, g_vec68d0d8, 68d0d8)
KING_VAR(BYTE**, g_vec68d0dc, 68d0dc)
KING_VAR(BYTE**, g_vec68d0c8, 68d0c8)
KING_VAR(BYTE**, g_vec68d0cc, 68d0cc)

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

extern "C" void FUN_404ef0(); KING_ALIAS("_FUN_404ef0", "_Town_Point")    // a town's position (out, town)
extern "C" void FUN_44c470(); KING_ALIAS("_FUN_44c470", "@Vec8_Insert44c470@16")    // vector<double>: insert (pos, value)
extern "C" void FUN_47c650(); KING_ALIAS("_FUN_47c650", "_Rec12_Copy")
extern "C" void FUN_50d190(); KING_ALIAS("_FUN_50d190", "@Shell_Nop@8")
extern "C" void __fastcall Waypoints_Clear(BYTE* self, void*);   // aiplay.cpp
extern "C" void __cdecl Road_NodeCostBest(int side, int from, int to, double* cost);   // aitrce.cpp

extern "C" void __fastcall Rec404920_Destroy(BYTE** self, void*)
{
    BYTE** a = self ? self + 3 : 0;
    FreeBytes(a[1], a[3]);
    FreeBytes(self[0], self[2]);
}

// the nearest other town to town i
extern "C" int* __cdecl Town_NearestOther(int* out, int i)
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
    return out;
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

ASM_FUNC(FUN_477b60, _Player_FromId)    // an order's player (in place)
extern "C" void __fastcall Buf_Append(void* s, void*, const void* src, int n);
extern "C" void __fastcall Buf_AppendString(void* s, void*, const char* str);
KING_VAR(char, g_strStart, 66c830)            // "START:"
KING_VAR(char, g_strCompl, 66c7ec)            // "COMPL:"
KING_VAR(char, g_strCancelSelling, 66d498)    // "cancelSellingDealer(PlayerId player)"
KING_VAR(char, g_strAiPlayH, 66d3fc)          // "C:\Nek\Vrappl\Gi\Htai\ai_play.h"
KING_VAR(char, g_strErr9012, 66d41c)          // "Internal error 9012"
KING_VAR(char, g_strTrailerStorm, 66d72c)     // "STrailerStorm"
KING_VAR(char, g_strTownAA, 66d768)           // "TOWN_AA"
KING_VAR(int, g_68d14c, 68d14c)
KING_VAR(int, g_68d154, 68d154)
KING_VAR(double, g_64b710, 64b710)            // 12
KING_VAR(double, g_64b708, 64b708)            // 32

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
KING_ALIAS("_Player_Drop422210", "_Player_PlaceAtDealer")
extern "C" void __cdecl Vehicle_Distances(int i, double* a, double* b);
KING_ALIAS("_Vehicle_Distances", "_Vehicle_Costs")
extern "C" const char* __cdecl AI_Text76(const char* key);
KING_ALIAS("_AI_Text76", "_Message_Text")
extern "C" const char* __cdecl AI_Text40e4c0(const char* key);
KING_ALIAS("_AI_Text40e4c0", "_Message_TextWithSuffix")
extern "C" const char* __cdecl Node_Name(int node);
KING_ALIAS("_Node_Name", "FUN_436490")
extern "C" void __cdecl Str_56ae60(const char* s);
KING_ALIAS("_Str_56ae60", "_Messages_Add")
extern "C" void __cdecl Item_41cff0(void* p);
KING_ALIAS("_Item_41cff0", "_Ai_PresentAtNode")
extern "C" void __cdecl Item_41f970(void* p);
KING_ALIAS("_Item_41f970", "_Present_DestroyFound")
extern "C" void __cdecl Obj_483c10(void* p);
KING_ALIAS("_Obj_483c10", "_AI_UpdateSwitch")
extern "C" void __cdecl Edge_SetFlag446ce0(int edge, int flag);
KING_ALIAS("_Edge_SetFlag446ce0", "_Road_SetObjectFlags")
extern "C" void FUN_452900(); KING_ALIAS("_FUN_452900", "@Vec4b_Insert452900@16")    // vector<void*>::insert(pos, &value)
extern "C" void FUN_458cf0(); KING_ALIAS("_FUN_458cf0", "_Bytes_AllocCopy@12")    // vector<char>: allocate and copy (n, first, last)
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8")
extern "C" void FUN_602ca0(); KING_ALIAS("_FUN_602ca0", "@MsgBuf_FreeStorage@8")
KING_VAR(double, g_66c6f0, 66c6f0)
KING_VAR(double, g_68d180, 68d180)
KING_VAR(AiText, g_positionText76, 77e6e0)

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

extern "C" void FUN_44c160(); KING_ALIAS("_FUN_44c160", "@PodVec_Assign@12")    // vector<double>::operator=
extern "C" void FUN_461dc0(); KING_ALIAS("_FUN_461dc0", "_Vec16b_AllocCopy@12")    // vector of 16-byte records: allocate and copy (n, first, last)
extern "C" void FUN_469430(); KING_ALIAS("_FUN_469430", "_Rec16_Copy")    // copy 16-byte records (first, last, dest, -, 0)
ASM_FUNC(FUN_466300, _Rec16_UninitCopy)    // copy-construct 16-byte records (first, last, dest, -)
extern "C" void FUN_460d10(); KING_ALIAS("_FUN_460d10", "_MsgBuf_FreeThunk")    // destroy a 16-byte record
extern "C" BYTE* __fastcall Rec16_DeletingDestructor(BYTE* s, void*, int flags);   // aistl.cpp
KING_VAR(BYTE, g_vt64b5c0, 64b5c0)
KING_VAR(BYTE, g_vt64b62c, 64b62c)

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

extern "C" void __cdecl Order144_AssignPlain(BYTE* d, const BYTE* s) { Order144_Assign(d, s, false); }

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

extern "C" void FUN_40c1a0(); KING_ALIAS("_FUN_40c1a0", "@ValueBuf_Destroy@8")    // destroy a 104-byte record
extern "C" void FUN_40cb60(); KING_ALIAS("_FUN_40cb60", "@Rec40cb60_Destroy@8")    // destroy a 24-byte record
extern "C" BYTE** __fastcall Vec24_Assign(BYTE** d, void*, BYTE* const* s);   // aistl.cpp
extern "C" void FUN_440dd0(); KING_ALIAS("_FUN_440dd0", "@Rec440dd0_FreeVectors@8")    // destroy a 192-byte record
extern "C" BYTE** __fastcall Vec16_AssignB(BYTE** d, void*, BYTE* const* s);   // aistl.cpp
extern "C" void FUN_41ce50(); KING_ALIAS("_FUN_41ce50", "@Rec41ce50_DestroyVector@8")    // destroy a 144-byte order
extern "C" void FUN_44fac0(); KING_ALIAS("_FUN_44fac0", "@NameHash_Resize@12")    // hash table: grow for n entries
extern "C" void FUN_44f2c0(); KING_ALIAS("_FUN_44f2c0", "_Rope_CharAt")
ASM_FUNC(FUN_45a7d0, _Rope_Compare)    // compare two strings
extern "C" void FUN_4592b0(); KING_ALIAS("_FUN_4592b0", "@Hash_NewNode@12")    // hash table: a new node for a key
extern "C" void FUN_461b60(); KING_ALIAS("_FUN_461b60", "_Vec24b_AllocCopy@12")    // vector of 24-byte records: allocate and copy (n, first, last)
extern "C" void FUN_46c490(); KING_ALIAS("_FUN_46c490", "_Rec_DestroyThunk46c490")    // destroy a 24-byte record
extern "C" void FUN_466c50(); KING_ALIAS("_FUN_466c50", "_Rec24b_Copy")    // copy 24-byte records (first, last, dest, -, 0)
ASM_FUNC(FUN_466200, _Rec24_UninitCopy)    // copy-construct 24-byte records (first, last, dest, -)
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
        Vec24_Assign((BYTE**)(d + 12), 0, (BYTE* const*)(s + 12));
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
            Vec16_AssignB((BYTE**)(d + 120), 0, (BYTE* const*)(s + 120));
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

// ---------------------------------------------------------------------------
// medium-sized functions, batch 5

extern "C" void FUN_487230(); KING_ALIAS("_FUN_487230", "_Route_Call4870e0")    // (player, node)
extern "C" void FUN_41bca0(); KING_ALIAS("_FUN_41bca0", "_Ai_CargoAmounts")    // an order's three sums (iterator, &a, &b, &c, cargo record, arg)
extern "C" void FUN_471200(); KING_ALIAS("_FUN_471200", "_Order_SetPrice")    // (player list node, order, direct, urgent)

// a player's route must start at the node of its last waypoint
extern "C" void __cdecl Player_CheckRouteStart(BYTE* p)
{
    if (F(BYTE*, p, 24) == F(BYTE*, p, 28))
        return;
    int node = F(int, F(BYTE*, p, 28), -12);
    if (F(BYTE*, p, 72) == F(BYTE*, p, 76))
        return;
    int e = F(int, F(BYTE*, p, 76), -36);
    if (e != -1 && e != node)
        AiError(0, 100, s_mainFile, s_mainId, 2199, "Internal error 0753", &g_aiEmpty, &g_aiEmpty);
    PositionId pos = F(PositionId, p, 40);
    if (pos.node != node)
        CallC(FUN_487230, p, node);
    if (F(BYTE*, p, 24) == F(BYTE*, p, 28))
        AiError(0, 100, s_mainFile, s_mainId, 2205, "Internal error 45712", &g_aiEmpty, &g_aiEmpty);
}

// an order's quantity (+56) from its cargo record; negative ones are reported
extern "C" void __cdecl Order_SetQuantity(int it, int* order, int arg)
{
    double a = 0, b = 0, q = 0;
    CallC(FUN_41bca0, it, &a, &b, &q, g_68ccb8 + *order * 104, arg);
    F(double, order, 56) = q;
    if (!(q >= 0.0))
        AiError(0, 512, s_mainFile, s_mainId, 6208, "Q<0", &g_aiEmpty, &g_aiEmpty);
    CallC(FUN_471200, it, order, 1, 1);
}

// a position's two keys (+36, +40 of out): its node's, or its road object's
extern "C" int __cdecl Position_Keys(PositionId p, BYTE* out)
{
    if (p.road == -1) {
        if (p.node == -1)
            return 0;
        F(int, out, 36) = F(int, F(BYTE*, g_roadNodes[p.node], 184), 12);
        F(int, out, 40) = F(int, F(BYTE*, g_roadNodes[p.node], 188), 12);
        return 0;
    }
    PositionId c = p;
    if (((p.road < 0 || (unsigned)p.road >= (unsigned)(g_roadEdgesEnd - g_roadEdges)) && !NodeId_Valid(&c.node, 0))
        || !(c.t >= -0.0002) || c.t > 1.0002 || c.segment < 0 || c.object < 0) {
        sprintf(g_positionText76, "(%d %g %d %d %x)", p.road, p.t, p.segment, p.object, p.m_24);
        AiError(33, 100, s_mainFile, s_mainId, 11534, "Invalid id", "PositionId", g_positionText76);
    }
    BYTE* e = (BYTE*)g_roadEdges[p.road];
    BYTE* o = F(BYTE**, e, 120)[p.object];
    BYTE* k = o ? o + 8 : 0;
    F(int, out, 36) = F(int, F(BYTE*, k, 8), 12);
    o = F(BYTE**, e, 120)[p.object];
    F(int, out, 40) = F(int, F(BYTE*, o ? o + 20 : (BYTE*)12, 0), 12);
    return 0;
}

extern "C" void FUN_429530(); KING_ALIAS("_FUN_429530", "_Ai_GetPlayerCapital")    // a player's key (list node by value)

// the list node of the player with the smallest key (players of kind 1 and
// those without waypoints are left out); the list's head if none
extern "C" void __cdecl Players_MinKey(void** out)
{
    void** best = (void**)g_players;
    double bestKey = 1.7976931348623157e308;
    for (void** n = (void**)*(void**)g_players; n != g_players; n = (void**)*n) {
        BYTE* p = (BYTE*)n + 8;
        if (F(int, p, 84) == 1 || F(BYTE*, p, 152) == F(BYTE*, p, 156))
            continue;
        double k = CallC<double>(FUN_429530, n);
        if (bestKey > k) {
            bestKey = k;
            best = n;
        }
    }
    *out = best;
}

// --- Functions with exception frames (the frames only rethrow; see README)

static void* AiAlloc(unsigned bytes)
{
    return bytes ? CallC<void*>(FUN_44b740, bytes) : 0;
}

extern "C" {

static BYTE* Registry_FindOrAdd(BYTE** begin, BYTE*** endp, BYTE*** capp, const char* name, unsigned size,
                                void (*ctor)(), void (*insert)(), void* vec)
{
    BYTE** p = begin;
    for (; p != *endp; p++) {
        const char* b = F(const char*, *p, 0);
        const char* e = F(const char*, *p, 4);
        if (b == e || !b)
            continue;
        if (!strcmp(b, name))
            break;
    }
    if (p != *endp)
        return *p;
    BYTE* o = (BYTE*)operator new(size);
    BYTE* made = o ? CallThis<BYTE*>(ctor, o, name) : 0;
    if (*endp != *capp) {
        if (*endp)
            **endp = made;
        (*endp)++;
    } else {
        CallThis(insert, vec, *endp, &made);
    }
    return (*endp)[-1];
}

// [FUN_4054a0] the 52-byte record of this name (made on first use)
BYTE* __cdecl Registry52_Get(const char* name)
{
    return Registry_FindOrAdd(g_registry52, &g_registry52End, &g_registry52Cap, name, 52, FUN_480550,
                              FUN_44d4e0, &g_registry52);
}

// [FUN_4055c0] the 24-byte record of this name (made on first use)
BYTE* __cdecl Registry24_Get(const char* name)
{
    return Registry_FindOrAdd(g_registry24, &g_registry24End, &g_registry24Cap, name, 24, FUN_480580,
                              FUN_44d7a0, &g_registry24);
}

static void Bytes_CopyInto(BYTE* self, const BYTE* b, const BYTE* e)
{
    unsigned n = e - b;
    BYTE* p = (BYTE*)AiAlloc(n);
    F(BYTE*, self, 0) = p;
    F(BYTE*, self, 4) = p;
    F(BYTE*, self, 8) = p + n;
    memmove(p, b, e - b);
    F(BYTE*, self, 4) = p + (e - b);
}

// [FUN_40ca70]
BYTE* __fastcall Rec40ca70_Construct(BYTE* self, void*, const BYTE** src, int a, int b)
{
    F(int, self, 0) = a;
    F(int, self, 4) = b;
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    Bytes_CopyInto(self + 8, src[0], src[1]);
    return self;
}

// [FUN_4581f0]
BYTE* __fastcall Rec4581f0_Copy(BYTE* self, void*, const BYTE* s)
{
    F(int, self, 0) = F(int, s, 0);
    F(int, self, 4) = F(int, s, 4);
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    Bytes_CopyInto(self + 8, F(const BYTE*, s, 8), F(const BYTE*, s, 12));
    return self;
}

// [FUN_448e10] vector<4 bytes> copy constructor
BYTE* __fastcall Vec4b_Copy(BYTE* self, void*, const BYTE** s)
{
    F(int, self, 0) = 0;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    unsigned n = (s[1] - s[0]) >> 2;
    BYTE* p = (BYTE*)AiAlloc(n * 4);
    F(BYTE*, self, 0) = p;
    F(BYTE*, self, 4) = p;
    F(BYTE*, self, 8) = p + n * 4;
    unsigned bytes = ((s[1] - s[0]) >> 2) << 2;
    memmove(p, s[0], bytes);
    F(BYTE*, self, 4) = p + bytes;
    return self;
}

// [FUN_44ac60] n 4-byte elements, the first ones copied from [first, last)
void* __stdcall Vec4b_AllocCopy(unsigned n, const BYTE* first, const BYTE* last)
{
    void* p = AiAlloc(n * 4);
    memmove(p, first, ((last - first) >> 2) << 2);
    return p;
}

// [FUN_44ba90] n 4-byte elements filled by [FUN_46b0e0]
void* __stdcall Vec4b_AllocFill(unsigned n, int a, int b)
{
    void* p = AiAlloc(n * 4);
    CallC(FUN_46b0e0, a, b, p, n);
    return p;
}

// [FUN_44b0f0] a 16-byte list node holding v
DWORD* __stdcall ListNode16_Make(const DWORD* v)
{
    DWORD* n = (DWORD*)AiAlloc(16);
    if (n + 2)
        n[2] = *v;
    return n;
}

// [FUN_44b250]
DWORD* __stdcall ListNode16_MakeB(const DWORD* v)
{
    DWORD* n = (DWORD*)AiAlloc(16);
    if (n + 2)
        n[2] = *v;
    return n;
}

// [FUN_447b00] an empty list (16-byte nodes)
BYTE** __fastcall List16_Construct(BYTE** self, void*, int unused)
{
    BYTE* n = (BYTE*)AiAlloc(16);
    self[0] = n;
    F(BYTE*, n, 0) = n;
    F(BYTE*, self[0], 4) = self[0];
    return self;
}

// [FUN_449680] an empty list (64-byte nodes)
BYTE** __fastcall List64_Construct(BYTE** self, void*, int unused)
{
    BYTE* n = (BYTE*)AiAlloc(64);
    self[0] = n;
    F(BYTE*, n, 0) = n;
    F(BYTE*, self[0], 4) = self[0];
    return self;
}

// Script value handles: +0 the object (reference count at +4), +8 flags.

static void SchValue_Release(DWORD* v)
{
    if (!(v[2] & 0x10000) && v[0]) {
        BYTE* o = (BYTE*)v[0];
        F(int, o, 4) = F(int, o, 4) - 1;
        if (!F(int, (BYTE*)v[0], 4) && v[0])
            CallVirt((BYTE*)v[0], 12, 1);
    }
}

static DWORD* Handle_FromValue(DWORD* self, const DWORD* v, int type, const char* name)
{
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    BYTE* o = (BYTE*)v[0];
    if (!o) {
        self[0] = 0;
        self[2] = 0;
        return self;
    }
    if (CallVirt<int>(o, 0) != type) {
        const char* has = CallVirt<const char*>((BYTE*)v[0], 4);
        CallC(FUN_4a54e0, 2, 1, has, name);
        return self;
    }
    self[0] = v[0];
    self[2] = v[2];
    F(int, (BYTE*)self[0], 4) = F(int, (BYTE*)self[0], 4) + 1;
    return self;
}

// [FUN_44cab0]
DWORD* __fastcall PairHandle_From(DWORD* self, void*, const DWORD* v)
{
    return Handle_FromValue(self, v, 2, s_pair);
}

// [FUN_4556d0]
DWORD* __fastcall NumberHandle_From(DWORD* self, void*, const DWORD* v)
{
    return Handle_FromValue(self, v, 6, s_number);
}

// [FUN_455760]
DWORD* __fastcall StringHandle_From(DWORD* self, void*, const DWORD* v)
{
    return Handle_FromValue(self, v, 3, s_string);
}

static DWORD* Handle_Take(DWORD* out, BYTE* o, int type, const char* name)
{
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    if (!o) {
        out[0] = 0;
        out[2] = 0;
        return out;
    }
    if (CallVirt<int>(o, 0) != type) {
        const char* has = CallVirt<const char*>(o, 4);
        CallC(FUN_4a54e0, 2, 1, has, name);
        return out;
    }
    out[0] = (DWORD)o;
    out[2] = CallVirt<int>(o, 0) & 255;
    F(int, o, 4) = F(int, o, 4) + 1;
    return out;
}

static BYTE* Port_New(int a, int b)
{
    BYTE* o = (BYTE*)operator new(296);
    return o ? CallThis<BYTE*>(FUN_4b59f0, o, a, b) : 0;
}

// [FUN_4557f0] a port from (*a, *b)
DWORD* __cdecl Port_MakeA(DWORD* out, int unused, const int* a, const int* b)
{
    return Handle_Take(out, Port_New(*a, *b), 11, s_port);
}

// [FUN_455f00]
DWORD* __cdecl Port_MakeB(DWORD* out, int unused, const int* a, const int* b)
{
    return Handle_Take(out, Port_New(*a, *b), 11, s_port);
}

// [FUN_455960] a port from (a, *b)
DWORD* __cdecl Port_MakeC(DWORD* out, int unused, int a, const int* b)
{
    return Handle_Take(out, Port_New(a, *b), 11, s_port);
}

// [FUN_455a50]
DWORD* __cdecl Port_MakeD(DWORD* out, int unused, int a, const int* b)
{
    return Handle_Take(out, Port_New(a, *b), 11, s_port);
}

// [FUN_455b40]
DWORD* __cdecl Port_MakeE(DWORD* out, int unused, int a, const int* b)
{
    return Handle_Take(out, Port_New(a, *b), 11, s_port);
}

// [FUN_455c30]
DWORD* __cdecl Port_MakeF(DWORD* out, int unused, int a, const int* b)
{
    return Handle_Take(out, Port_New(a, *b), 11, s_port);
}

// [FUN_4573f0]
DWORD* __cdecl Port_MakeG(DWORD* out, int unused, int a, const int* b)
{
    return Handle_Take(out, Port_New(a, *b), 11, s_port);
}

// [FUN_4574e0] a number from (*a, *b)
DWORD* __cdecl Number_Make(DWORD* out, int unused, const int* a, const int* b)
{
    BYTE* o = CallC<BYTE*>(FUN_4ad2f0, 32);
    BYTE* n = o ? CallThis<BYTE*>(FUN_4b0b60, o, *a, *b) : 0;
    return Handle_Take(out, n, 6, s_number);
}

// [FUN_40e410] whether the script value of x (a kind 2 item) matches *self
bool __fastcall Item_Matches(BYTE** self, void*, BYTE* x)
{
    if (F(BYTE, x, 8) != 2)
        return false;
    DWORD v[3];
    CallC(FUN_407f00, v, x);
    bool r = CallC<bool>(FUN_4b65e0, v, self[0]);
    SchValue_Release(v);
    return r;
}

}

static void Bytes_Free(BYTE* b, BYTE* e)
{
    if (e != b)
        StlDeallocate(b, e - b);
}

static void Dwords_Free(BYTE* b, BYTE* e, int size)
{
    int n = (e - b) / size;
    if (n)
        StlDeallocate(b, n * size);
}

static void SchValue_Copy(DWORD* d, const DWORD* s)
{
    if (s[2] & 0x10000) {
        d[2] = s[2];
        d[0] = s[0];
        d[1] = s[1];
        return;
    }
    d[1] = 0;
    if (s[0]) {
        d[0] = s[0];
        d[2] = s[2];
        F(int, (BYTE*)s[0], 4) = F(int, (BYTE*)s[0], 4) + 1;
    } else {
        d[0] = 0;
        d[2] = 0;
    }
}

// the trace buffer of the AI (state.dump): +0 data, +8 size, +16 position
static void Trace_Tag(const DWORD* tag)
{
    BYTE* buf = g_traceData;
    if (buf && g_tracePos + 8 < g_traceSize) {
        F(DWORD, buf, g_tracePos) = tag[0];
        F(DWORD, buf, g_tracePos + 4) = tag[1];
        g_tracePos += 8;
    }
}

extern "C" {

// [FUN_403680]
void __fastcall Rec403680_Destroy(BYTE* self, void*)
{
    Dwords_Free(F(BYTE*, self, 96), F(BYTE*, self, 104), 4);
    SchValue_Release((DWORD*)(self + 84));
    Bytes_Free(F(BYTE*, self, 72), F(BYTE*, self, 80));
    int n = (F(BYTE*, self, 44) - F(BYTE*, self, 36)) >> 2;
    if (n)
        CallC(FUN_44b4a0, F(BYTE*, self, 36), n * 4);
    Bytes_Free(F(BYTE*, self, 20), F(BYTE*, self, 28));
    SchValue_Release((DWORD*)(self + 8));
}

// [FUN_404290]
void __fastcall Rec404290_Destroy(BYTE* self, void*)
{
    Dwords_Free(F(BYTE*, self, 100), F(BYTE*, self, 108), 8);
    for (int i = 1; i >= 0; i--)
        CallThis(FUN_447970, self + 72 + i * 12);
    Dwords_Free(F(BYTE*, self, 40), F(BYTE*, self, 48), 4);
    Dwords_Free(F(BYTE*, self, 28), F(BYTE*, self, 36), 16);
    Bytes_Free(F(BYTE*, self, 12), F(BYTE*, self, 20));
    int n = F(BYTE*, self, 8) - F(BYTE*, self, 0);
    if (n)
        CallC(FUN_44b4a0, F(BYTE*, self, 0), n);
}

// [FUN_404570]
void __fastcall Rec404570_Destroy(BYTE* self, void*)
{
    BYTE* b = F(BYTE*, self, 28);
    for (BYTE* e = b; e != F(BYTE*, self, 32); e += 36)
        CallThis(FUN_410e40, e);
    Dwords_Free(F(BYTE*, self, 28), F(BYTE*, self, 36), 36);
    Bytes_Free(F(BYTE*, self, 12), F(BYTE*, self, 20));
    SchValue_Release((DWORD*)self);
}

// [FUN_404790]
void __fastcall Rec404790_Destroy(BYTE* self, void*)
{
    Dwords_Free(F(BYTE*, self, 112), F(BYTE*, self, 120), 4);
    Bytes_Free(F(BYTE*, self, 100), F(BYTE*, self, 108));
    int n = F(BYTE*, self, 96) - F(BYTE*, self, 88);
    if (n)
        CallC(FUN_44b4a0, F(BYTE*, self, 88), n);
    SchValue_Release((DWORD*)(self + 36));
    BYTE* v = self ? self + 12 : 0;
    n = F(BYTE*, v, 12) - F(BYTE*, v, 4);
    if (n)
        CallC(FUN_44b4a0, F(BYTE*, v, 4), n);
    Bytes_Free(F(BYTE*, self, 0), F(BYTE*, self, 8));
}

// [FUN_40c220] copy constructor
BYTE* __fastcall Rec40c220_Copy(BYTE* self, void*, const BYTE* s)
{
    SchValue_Copy((DWORD*)self, (const DWORD*)s);
    static const int k[] = {16, 20, 24, 28, 32, 36, 40, 44, 48, 56, 60, 64, 68, 72, 80, 84};
    for (int i = 0; i < 16; i++)
        F(int, self, k[i]) = F(int, s, k[i]);
    F(int, self, 88) = 0;
    F(int, self, 92) = 0;
    F(int, self, 96) = 0;
    Bytes_CopyInto(self + 88, F(const BYTE*, s, 88), F(const BYTE*, s, 92));
    return self;
}

// [FUN_40cda0] copy constructor
BYTE* __fastcall Rec40cda0_Copy(BYTE* self, void*, const BYTE* s)
{
    SchValue_Copy((DWORD*)self, (const DWORD*)s);
    int n = (F(const BYTE*, s, 16) - F(const BYTE*, s, 12)) / 24;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    F(int, self, 20) = 0;
    BYTE* p = (BYTE*)AiAlloc(n * 24);
    F(BYTE*, self, 12) = p;
    F(BYTE*, self, 16) = p;
    F(BYTE*, self, 20) = p + n * 24;
    F(BYTE*, self, 16) = CallC<BYTE*>(FUN_466200, F(const BYTE*, s, 12), F(const BYTE*, s, 16), p, 0);
    return self;
}

// [FUN_40e270] the text of the message with this id
const char* __cdecl Message_Text(const char* id)
{
    const char* key = id;
    BYTE* e = g_aiMessages;
    for (; e != g_aiMessagesEnd; e += 12)
        if (Item_Matches((BYTE**)&key, 0, e))
            break;
    if (e == g_aiMessagesEnd) {
        AiError(35, 1, s_mainFile, s_mainId, 2859, s_noMessage, id, &g_aiEmpty);
        return &g_aiEmpty;
    }
    DWORD tmp[3];
    DWORD* v = CallC<DWORD*>(FUN_4090a0, tmp, e);
    DWORD h[3] = {0, 0, 0};
    BYTE* o = (BYTE*)v[0];
    if (o) {
        if (CallVirt<int>(o, 0) != 3) {
            const char* has = CallVirt<const char*>((BYTE*)v[0], 4);
            CallC(FUN_4a54e0, 2, 1, has, (const char*)s_string);
        } else {
            h[0] = v[0];
            h[2] = v[2];
            F(int, (BYTE*)h[0], 4) = F(int, (BYTE*)h[0], 4) + 1;
        }
    }
    SchValue_Release(tmp);
    if (!h[0])
        CallC(FUN_4a52e0, 0, 1);
    if (h[2] & 0x10000)
        CallC(FUN_4a52e0, 4, 1);
    const char* text = F(const char*, (BYTE*)h[0], 12);
    if (!(h[2] & 0x10000) && h[0]) {
        BYTE* p = (BYTE*)h[0];
        if (--F(int, p, 4) == 0)
            CallVirt(p, 12, 1);
    }
    return text;
}

// [FUN_40ad00] finishRacingPlayer(PlayerId player)
void __cdecl Ai_FinishRacingPlayer(BYTE* player)
{
    Trace_Tag((const DWORD*)g_tagArt);
    Buf_AppendString(&g_trace, 0, s_finishRacingPlayer);
    DWORD f = F(DWORD, player, 96);
    if (f & 0x4000) {
        f &= ~0x4000u;
        F(DWORD, player, 96) = f;
        if (!(f & 0x100))
            F(double, player, 808) = CallC<double>(FUN_401000) - F(double, player, 792);
        if (CallC<bool>(FUN_4172e0, player)) {
            g_68d164++;
            CallC(FUN_417530, player);
            CallC(FUN_483cb0, Message_Text(s_competitorArrived));
        }
        F(BYTE, player, 872) = 1;
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    BYTE* buf = g_traceData;
    if (buf && *s_finishRacingPlayer)
        for (const char* c = s_finishRacingPlayer; g_tracePos < g_traceSize;) {
            buf[g_tracePos] = *c++;
            g_tracePos++;
            if (!*c)
                break;
            buf = g_traceData;
        }
}

}

// The state.dump trace around the AI's entry points: "ART:" and the
// signature on entry, "MPL:" and the signature on exit. The original writes
// them in three ways (inline, or through Buf_Append / Buf_AppendString).

static void Trace_CopyName(const char* name)
{
    BYTE* buf = g_traceData;
    if (!buf || !*name)
        return;
    for (const char* c = name; g_tracePos < g_traceSize;) {
        buf[g_tracePos] = *c++;
        g_tracePos++;
        if (!*c)
            break;
        buf = g_traceData;
    }
}

static void Trace_EnterInline(const char* name)          // tag and name inline
{
    Trace_Tag((const DWORD*)g_tagArt);
    Trace_CopyName(name);
}

static void Trace_EnterAppend(const char* name)          // tag inline, name appended
{
    Trace_Tag((const DWORD*)g_tagArt);
    Buf_AppendString(&g_trace, 0, name);
}

static void Trace_EnterBuffered(const char* name)        // both appended
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, name);
}

static void Trace_Leave(const char* name)
{
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(name);
}

extern "C" {

// [FUN_427090] clearPlayerFromMafia(PlayerId player)
void __cdecl Ai_ClearPlayerFromMafia(int player)
{
    Trace_EnterBuffered(s_clearPlayerFromMafia);
    BYTE* head = g_playersHead;
    for (BYTE* it = *(BYTE**)head; it != g_playersHead; it = *(BYTE**)it) {
        if (!CallC<BYTE*>(FUN_40a9f0, it))
            continue;
        if (F(int, CallC<BYTE*>(FUN_40a9f0, it), 84) != 6)
            continue;
        if (F(int, CallC<BYTE*>(FUN_40a9f0, it), 960) != player)
            continue;
        BYTE* none = g_playersHead;
        F(BYTE*, CallC<BYTE*>(FUN_40a9f0, it), 960) = none;
    }
    Trace_Leave(s_clearPlayerFromMafia);
}

// [FUN_4272d0] clearPlayerFromTail(PlayerId player)
void __cdecl Ai_ClearPlayerFromTail(int player)
{
    Trace_EnterInline(s_clearPlayerFromTail);
    Ai_ClearPlayerFromMafia(player);
    CallC(FUN_427220, player);
    Trace_Leave(s_clearPlayerFromTail);
}

static void InvalidPlayerId()
{
    sprintf(g_numberText, s_idFormat);
    AiError(33, 100, s_mainFile, s_mainId, 14886, s_invalidId, s_playerIdType, g_numberText);
}

// [FUN_437520] getPositionId(PlayerId player)
DWORD* __cdecl Ai_GetPositionId(DWORD* out, BYTE* player)
{
    Trace_EnterAppend(s_getPositionId);
    if (CallC<int>(FUN_4566c0, player))
        InvalidPlayerId();
    for (int k = 0; k < 8; k++)
        out[k] = F(DWORD, player, 40 + k * 4);
    Trace_Leave(s_getPositionId);
    return out;
}

// [FUN_43e4d0] setPlayerPositionV(RoomId room, PlayerId player, dvector3 const & v)
void __cdecl Ai_SetPlayerPositionV(int room, BYTE* player, const void* v)
{
    Trace_EnterBuffered(s_setPlayerPositionV);
    const DWORD* src;
    DWORD tmp[8];
    BYTE* r;
    if (F(int, player, 24) == F(int, player, 28) && room != -1 &&
        (r = g_registry52[room], F(int, r, 28) != F(int, r, 32)))
        src = CallC<const DWORD*>(FUN_436ef0, tmp, room, v);
    else
        src = (const DWORD*)(player + 40);
    for (int k = 0; k < 8; k++)
        F(DWORD, player, 816 + k * 4) = src[k];
    Trace_Leave(s_setPlayerPositionV);
}

// [FUN_440f70] getTownInfo(PlayerId player, VrIndex _id, TownInfo & ti)
int __cdecl Ai_GetTownInfo(int player, int id, BYTE* ti)
{
    Trace_EnterInline(s_getTownInfo);
    int slot = id;
    int* idx = CallC<int*>(FUN_404c70, &slot, id);
    BYTE* town = g_townsBegin[*idx];
    int r = CallC<int>(FUN_440970, player, town, ti);
    Trace_Leave(s_getTownInfo);
    return r;
}

// [FUN_441950] getOrderInfo(PlayerId player, VrIndex _townVrIndex, int _orderid, OrderInfo & oi)
int __cdecl Ai_GetOrderInfo(int player, int town, int order, BYTE* oi)
{
    Trace_EnterInline(s_getOrderInfo);
    F(BYTE, oi, 184) = 0;
    int idx;
    CallC(FUN_404c70, &idx, town);
    int r = CallC<int>(FUN_441120, player, idx, order, oi);
    Trace_Leave(s_getOrderInfo);
    return r;
}

}

extern "C" {

// [FUN_4455b0] __dropOrders(PlayerId player)
int __cdecl Ai_DropOrders(BYTE* player)
{
    Trace_EnterAppend(s_dropOrders);
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 18042, s_invalidId, s_playerIdType, g_numberText);
    }
    CallC(FUN_4778a0, player + 8);
    F(BYTE, player, 200) = 0;
    F(BYTE, player, 201) = 0;
    CallC(FUN_442c60, player);
    Trace_Leave(s_dropOrders);
    return 0;
}

// [FUN_446af0] killDelayedPlayers()
void __cdecl Ai_KillDelayedPlayers()
{
    Trace_EnterAppend(s_killDelayedPlayers);
    for (;;) {
        BYTE* it = *(BYTE**)g_playersHead;
        for (; it != g_playersHead; it = *(BYTE**)it)
            if (CallC<BYTE*>(FUN_40a9f0, it) && (F(DWORD, CallC<BYTE*>(FUN_40a9f0, it), 88) & 0x20000))
                break;
        if (it == g_playersHead)
            break;
        int pid;
        CallThis(FUN_426280, &pid, it);
        CallC(FUN_4466a0, pid);
    }
    Trace_Leave(s_killDelayedPlayers);
}

// [FUN_4487a0] vector<16 bytes>(n, value)
DWORD* __fastcall Vec16b_Construct(DWORD* self, void*, unsigned n, const void* v, int unused)
{
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    BYTE* p = (BYTE*)AiAlloc(n * 16);
    self[0] = (DWORD)p;
    self[1] = (DWORD)p;
    self[2] = (DWORD)(p + n * 16);
    self[1] = CallC<DWORD>(FUN_466400, p, n, v, n);
    return self;
}

// [FUN_448cd0] vector<4 bytes>(n, *v)
DWORD* __fastcall Vec4b_Construct(DWORD* self, void*, unsigned n, const DWORD* v, int unused)
{
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    DWORD* p = (DWORD*)AiAlloc(n * 4);
    self[0] = (DWORD)p;
    self[1] = (DWORD)p;
    self[2] = (DWORD)(p + n);
    for (unsigned i = 0; i < n; i++)
        *p++ = *v;
    self[1] = (DWORD)p;
    return self;
}

// [FUN_44b310] a 64-byte list node holding a copy of the record s
BYTE* __stdcall ListNode64_Make(const BYTE* s)
{
    BYTE* node = (BYTE*)AiAlloc(64);
    BYTE* v = node + 8;
    if (v) {
        F(DWORD, v, 0) = F(DWORD, s, 0);
        for (int k = 8; k < 32; k += 4)
            F(DWORD, v, k) = F(DWORD, s, k);
        F(DWORD, v, 32) = F(DWORD, s, 32);
        int n = (F(const BYTE*, s, 40) - F(const BYTE*, s, 36)) / 12;
        F(int, v, 36) = 0;
        F(int, v, 40) = 0;
        F(int, v, 44) = 0;
        BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n * 12) : 0;
        F(BYTE*, v, 36) = p;
        F(BYTE*, v, 40) = p;
        F(BYTE*, v, 44) = p + n * 12;
        F(BYTE*, v, 40) = CallC<BYTE*>(FUN_47e3e0, F(const BYTE*, s, 36), F(const BYTE*, s, 40), p, v + 36);
        F(DWORD, v, 48) = F(DWORD, s, 48);
        F(DWORD, v, 52) = F(DWORD, s, 52);
    }
    return node;
}

}

// A random pick among the matching elements of a sequence: the matches are
// gathered into a temporary vector and one is chosen with the subtractive
// generator at rng (55 words, indexes at +220 and +224).

static void PtrVec_Push(DWORD* v, void (*insert)(), BYTE* x)
{
    DWORD* last = (DWORD*)v[1];
    if (last != (DWORD*)v[2]) {
        if (last)
            *last = (DWORD)x;
        v[1] = (DWORD)(last + 1);
    } else {
        CallThis(insert, v, last, &x);
    }
}

static BYTE* PickRandom(DWORD* v, BYTE* end, DWORD* rng)
{
    BYTE** b = (BYTE**)v[0];
    unsigned n = (BYTE**)v[1] - b;
    if (!n) {
        if (v[2] != v[0])
            CallC(FUN_44b4a0, b, ((v[2] - v[0]) >> 2) << 2);
        return end;
    }
    unsigned i = (rng[55] + 1) % 55;
    unsigned j = (rng[56] + 1) % 55;
    rng[55] = i;
    rng[56] = j;
    rng[i] -= rng[j];
    BYTE* r = b[rng[rng[55]] % n];
    if (v[2] != v[0])
        StlDeallocate(b, ((v[2] - v[0]) >> 2) << 2);
    return r;
}

extern "C" {

// [FUN_453f10] a random element of [first, last) other than x
BYTE** __cdecl Pick_RandomOther(BYTE** first, BYTE** last, BYTE* x, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE** it = first; it != last; it++)
        if (*it != x)
            PtrVec_Push(v, FUN_45aed0, (BYTE*)it);
    return (BYTE**)PickRandom(v, (BYTE*)last, rng);
}

// [FUN_4540a0] a random player of [first, last) with neither flag 1 nor 2
BYTE** __cdecl Pick_RandomIdle(BYTE** first, BYTE** last, int unused, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE** it = first; it != last; it++)
        if (!(F(BYTE, *it, 96) & 3))
            PtrVec_Push(v, FUN_45aed0, (BYTE*)it);
    return (BYTE**)PickRandom(v, (BYTE*)last, rng);
}

// [FUN_456880] a random 104-byte record with (flags & mask) == value
BYTE* __cdecl Pick_RandomMasked(BYTE* first, BYTE* last, DWORD value, DWORD mask, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* it = first; it != last; it += 104)
        if ((F(DWORD, it, 48) & mask) == value)
            PtrVec_Push(v, FUN_45d370, it);
    return PickRandom(v, last, rng);
}

// [FUN_455ff0] the 56-byte records with +24 == a and (b == -1 or +20 == b)
DWORD* __cdecl Select_Records56(DWORD* out, BYTE* first, BYTE* last, int a, int b)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* it = first; it != last; it += 56)
        if (F(int, it, 24) == a && (b == -1 || F(int, it, 20) == b))
            PtrVec_Push(v, FUN_45c610, it);
    unsigned n = (v[1] - v[0]) >> 2;
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    DWORD* p = (DWORD*)AiAlloc(n * 4);
    out[0] = (DWORD)p;
    out[1] = (DWORD)p;
    out[2] = (DWORD)(p + n);
    out[1] = CallC<DWORD>(FUN_46b0e0, v[0], v[1], p, 0);
    if (v[2] != v[0])
        StlDeallocate((void*)v[0], ((v[2] - v[0]) >> 2) << 2);
    return out;
}

// [FUN_458500] copy constructor: three words, a byte vector, a 4-byte vector
BYTE* __fastcall Rec458500_Copy(BYTE* self, void*, const BYTE* s)
{
    F(DWORD, self, 0) = F(DWORD, s, 0);
    F(DWORD, self, 4) = F(DWORD, s, 4);
    F(DWORD, self, 8) = F(DWORD, s, 8);
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    F(int, self, 20) = 0;
    Bytes_CopyInto(self + 12, F(const BYTE*, s, 12), F(const BYTE*, s, 16));
    unsigned n = (F(const BYTE*, s, 28) - F(const BYTE*, s, 24)) >> 2;
    F(int, self, 24) = 0;
    F(int, self, 28) = 0;
    F(int, self, 32) = 0;
    DWORD* p = (DWORD*)AiAlloc(n * 4);
    F(DWORD*, self, 24) = p;
    F(DWORD*, self, 28) = p;
    F(DWORD*, self, 32) = p + n;
    F(DWORD, self, 28) = CallC<DWORD>(FUN_46b0e0, F(const BYTE*, s, 24), F(const BYTE*, s, 28), p, 0);
    return self;
}

}

extern "C" {

// [FUN_458660] copy constructor: a word, a byte vector, then +16..+32 and +40..+52
BYTE* __fastcall Rec458660_Copy(BYTE* self, void*, const BYTE* s)
{
    F(DWORD, self, 0) = F(DWORD, s, 0);
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    Bytes_CopyInto(self + 4, F(const BYTE*, s, 4), F(const BYTE*, s, 8));
    for (int k = 16; k <= 32; k += 4)
        F(DWORD, self, k) = F(DWORD, s, k);
    for (int k = 40; k <= 52; k += 4)
        F(DWORD, self, k) = F(DWORD, s, k);
    return self;
}

// [FUN_4587d0] destructor: a vector of 1592-byte records at +56, then the base
void __fastcall Rec4587d0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64b600;
    BYTE* b = F(BYTE*, self, 56);
    for (BYTE* e = b; e != F(BYTE*, self, 60); e += 1592)
        CallThis(FUN_458910, e);
    Dwords_Free(F(BYTE*, self, 56), F(BYTE*, self, 64), 1592);
    F(void*, self, 0) = g_vtbl64b62c;
    Dwords_Free(F(BYTE*, self, 40), F(BYTE*, self, 48), 8);
}

// [FUN_458a80] copy constructor with a vector of 16-byte records at +120
BYTE* __fastcall Rec458a80_Copy(BYTE* self, void*, const BYTE* s)
{
    for (int k = 0; k <= 16; k += 4)
        F(DWORD, self, k) = F(DWORD, s, k);
    for (int k = 24; k <= 104; k += 4)
        F(DWORD, self, k) = F(DWORD, s, k);
    F(DWORD, self, 112) = F(DWORD, s, 112);
    F(DWORD, self, 116) = F(DWORD, s, 116);
    unsigned n = (F(const BYTE*, s, 124) - F(const BYTE*, s, 120)) >> 4;
    F(int, self, 120) = 0;
    F(int, self, 124) = 0;
    F(int, self, 128) = 0;
    BYTE* p = (BYTE*)AiAlloc(n * 16);
    F(BYTE*, self, 120) = p;
    F(BYTE*, self, 124) = p;
    F(BYTE*, self, 128) = p + n * 16;
    F(BYTE*, self, 124) = CallC<BYTE*>(FUN_466300, F(const BYTE*, s, 120), F(const BYTE*, s, 124), p, self + 120);
    F(DWORD, self, 132) = F(DWORD, s, 132);
    F(DWORD, self, 136) = F(DWORD, s, 136);
    return self;
}

}

// --- STL instances with exception frames (unit 0x458bc0)

// vector<4-byte>::insert(pos, x); eight identical instances
static void Vec4b_InsertAt(DWORD* self, DWORD* pos, const DWORD* x)
{
    DWORD* last = (DWORD*)self[1];
    if (last != (DWORD*)self[2]) {
        if (last)
            *last = last[-1];
        self[1] = (DWORD)(last + 1);
        DWORD v = *x;
        for (DWORD* p = last - 1; p > pos; p--)
            *p = p[-1];
        *pos = v;
        return;
    }
    unsigned n = last - (DWORD*)self[0];
    unsigned cap = n ? n * 2 : 1;
    DWORD* p = (DWORD*)CallC<void*>(FUN_44b740, cap * 4);
    DWORD* q = CallC<DWORD*>(FUN_46b0e0, self[0], pos, p, pos);
    if (q)
        *q = *x;
    DWORD* e = CallC<DWORD*>(FUN_46b0e0, pos, self[1], q + 1, pos);
    if (self[2] != self[0])
        StlDeallocate((void*)self[0], ((self[2] - self[0]) >> 2) << 2);
    self[0] = (DWORD)p;
    self[1] = (DWORD)e;
    self[2] = (DWORD)(p + cap);
}

static void Ptrs_ToVector(DWORD* out, DWORD* v)
{
    unsigned n = (v[1] - v[0]) >> 2;
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    DWORD* p = (DWORD*)AiAlloc(n * 4);
    out[0] = (DWORD)p;
    out[1] = (DWORD)p;
    out[2] = (DWORD)(p + n);
    out[1] = CallC<DWORD>(FUN_46b0e0, v[0], v[1], p, 0);
    if (v[2] != v[0])
        StlDeallocate((void*)v[0], ((v[2] - v[0]) >> 2) << 2);
}

// a reference-counted script string (24 bytes) over buf
static BYTE* SchString_New(BYTE* buf, unsigned len)
{
    BYTE* s = CallC<BYTE*>(FUN_44b740, 24);
    if (!s)
        return 0;
    F(unsigned, s, 0) = len;
    F(int, s, 4) = 1;
    F(BYTE, s, 8) = 0;
    F(BYTE*, s, 20) = buf;
    F(BYTE*, s, 16) = buf;
    F(BYTE, s, 12) = 1;
    F(BYTE, s, 13) = 0;
    return s;
}

// vector<char> assignment as the sort helpers inline it
static void Bytes_Assign(BYTE* d, const BYTE* b, const BYTE* e)
{
    unsigned n = e - b;
    BYTE* db = F(BYTE*, d, 0);
    if (n > (unsigned)(F(BYTE*, d, 8) - db)) {
        BYTE* p = CallStd<BYTE*>(FUN_458cf0, n, b, e);
        if (F(BYTE*, d, 8) != db)
            CallC(FUN_44b4a0, db, F(BYTE*, d, 8) - db);
        F(BYTE*, d, 0) = p;
        F(BYTE*, d, 8) = p + n;
    } else {
        unsigned size = F(BYTE*, d, 4) - db;
        if (size >= n) {
            memmove(db, b, n);
        } else {
            memmove(db, b, size);
            memmove(F(BYTE*, d, 4), b + size, n - size);
        }
    }
    F(BYTE*, d, 4) = F(BYTE*, d, 0) + n;
}

extern "C" {

// [FUN_45aed0]
void __fastcall Vec4b_InsertA(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_45c610]
void __fastcall Vec4b_InsertB(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_45cb00]
void __fastcall Vec4b_InsertC(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_45d370]
void __fastcall Vec4b_InsertD(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_45dde0]
void __fastcall Vec4b_InsertE(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_45fb10]
void __fastcall Vec4b_InsertF(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_4604b0]
void __fastcall Vec4b_InsertG(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_464230]
void __fastcall Vec4b_InsertH(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}


// [FUN_458cf0] n bytes holding a copy of [first, last)
BYTE* __stdcall Bytes_AllocCopy(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)AiAlloc(n);
    memmove(p, first, last - first);
    return p;
}

// [FUN_458e00] n 8-byte elements holding a copy of [first, last)
BYTE* __stdcall Vec8b_AllocCopy(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)AiAlloc(n * 8);
    memmove(p, first, ((last - first) >> 3) << 3);
    return p;
}

// [FUN_459150] a script string holding a copy of len characters
BYTE* __cdecl SchString_FromChars(const char* src, unsigned len)
{
    if (!len)
        return 0;
    BYTE* buf = (BYTE*)AiAlloc((len + 8) & ~7u);
    memmove(buf, src, len);
    buf[len] = 0;
    return SchString_New(buf, len);
}

// [FUN_462700] a script string holding s followed by len characters of b
BYTE* __cdecl SchString_Concat(BYTE* s, const char* b, unsigned len)
{
    unsigned n = F(unsigned, s, 0);
    unsigned total = n + len;
    BYTE* buf = (BYTE*)AiAlloc((total + 8) & ~7u);
    BYTE* data = F(BYTE*, s, 20);
    CallC(FUN_603540, data, data + n, buf);
    const char* bEnd = b + len;
    memmove(buf + n, b, len);
    BYTE* dEnd = buf + n + len;
    DWORD tmp[2];
    CallThis(FUN_40a580, tmp, &bEnd, &dEnd);
    buf[n + len] = 0;
    return SchString_New(buf, total);
}

// [FUN_4593b0] n 12-byte records copied from [first, last)
BYTE* __stdcall Vec12b_AllocCopyA(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)AiAlloc(n * 12);
    CallC(FUN_47e3e0, first, last, p, n);
    return p;
}

// [FUN_459710]
BYTE* __stdcall Vec12b_AllocCopyB(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)AiAlloc(n * 12);
    CallC(FUN_467780, first, last, p, n);
    return p;
}

// [FUN_461b60] n 24-byte records copied from [first, last)
BYTE* __stdcall Vec24b_AllocCopy(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)AiAlloc(n * 24);
    CallC(FUN_466200, first, last, p, n);
    return p;
}

// [FUN_461dc0] n 16-byte records copied from [first, last)
BYTE* __stdcall Vec16b_AllocCopy(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)AiAlloc(n * 16);
    CallC(FUN_466300, first, last, p, n);
    return p;
}

// [FUN_462010] n 4-byte elements copied from [first, last)
BYTE* __stdcall Vec4b_AllocCopyB(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)AiAlloc(n * 4);
    CallC(FUN_46b0e0, first, last, p, n);
    return p;
}

// [FUN_459970]
DWORD* __stdcall Vec4b_AllocCopyC(unsigned n, const DWORD* first, const DWORD* last)
{
    DWORD* p = (DWORD*)AiAlloc(n * 4);
    for (int i = 0, k = last - first; i < k; i++)
        p[i] = first[i];
    return p;
}

// [FUN_459ac0]
BYTE* __stdcall Bytes_AllocCopyB(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)AiAlloc(n);
    for (int i = 0, k = last - first; i < k; i++)
        p[i] = first[i];
    return p;
}

// [FUN_459610] vector<4 bytes> copy constructor
DWORD* __fastcall Vec4b_CopyB(DWORD* self, void*, const DWORD* s)
{
    unsigned n = (s[1] - s[0]) >> 2;
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    DWORD* p = (DWORD*)AiAlloc(n * 4);
    self[0] = (DWORD)p;
    self[1] = (DWORD)p;
    self[2] = (DWORD)(p + n);
    self[1] = CallC<DWORD>(FUN_46b0e0, s[0], s[1], p, n);
    return self;
}

// [FUN_45bee0] the first item whose value (+8) matches the handle h
BYTE** __cdecl Find_ByHandleA(BYTE** first, BYTE** last, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    BYTE** it = first;
    for (; it != last; it++)
        if (CallC<bool>(FUN_4b6610, *it + 8, h))
            break;
    SchValue_Release(h);
    return it;
}

// [FUN_45b190] the same over 104-byte records
extern "C" BYTE* __cdecl Find_ByHandle104(BYTE* first, BYTE* last, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    BYTE* it = first;
    for (; it != last; it += 104)
        if (CallC<bool>(FUN_4b6610, it, h))
            break;
    SchValue_Release(h);
    return it;
}

// [FUN_45c0c0] the same over 24-byte records
BYTE* __cdecl Find_ByHandleB(BYTE* first, BYTE* last, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    BYTE* it = first;
    for (; it != last; it += 24)
        if (CallC<bool>(FUN_4b6610, it, h))
            break;
    SchValue_Release(h);
    return it;
}

// [FUN_45bf80] the first kind 2 item (12 bytes) whose script value is the symbol h
BYTE* __cdecl Find_ItemBySymbol(BYTE* first, BYTE* last, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    BYTE* o = (BYTE*)h0;
    BYTE* it = first;
    for (; it != last; it += 12) {
        if (F(BYTE, it, 8) != 2)
            continue;
        DWORD v[3];
        CallC(FUN_407f00, v, it);
        bool match;
        if (v[0] == h0 && v[2] == h2 && v[1] == h1) {
            match = true;
        } else {
            if (!o)
                CallC(FUN_4a52e0, 0, 1);
            if (h2 & 0x10000)
                CallC(FUN_4a52e0, 4, 1);
            match = CallC<bool>(FUN_4b65e0, v, F(DWORD, o, 12));
        }
        SchValue_Release(v);
        if (match)
            break;
    }
    SchValue_Release(h);
    return it;
}

// [FUN_45d1b0] a random list node of kind 257 with flag 4 whose time has come
BYTE** __cdecl Pick_RandomDueNode(BYTE** out, BYTE* first, BYTE* last, int unused, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* it = first; it != last; it = *(BYTE**)it)
        if (F(int, it, 100) == 257 && (F(DWORD, it, 96) >> 2 & 1) && CallC<double>(FUN_401000) >= F(double, it, 112))
            PtrVec_Push(v, FUN_45dde0, it);
    *out = PickRandom(v, last, rng);
    return out;
}

// [FUN_45ef70] a random list node not of kind 256/257 that passes [FUN_418f70]
BYTE** __cdecl Pick_RandomNodeB(BYTE** out, BYTE* first, BYTE* last, int unused, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* it = first; it != last; it = *(BYTE**)it) {
        int k = F(int, it, 100);
        if (k != 256 && k != 257 && CallC<bool>(FUN_418f70, it + 8))
            PtrVec_Push(v, FUN_45dde0, it);
    }
    *out = PickRandom(v, last, rng);
    return out;
}

static DWORD* Select_FreeNodes(DWORD* out, BYTE* first, BYTE* last, int kind)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* it = first; it != last; it = *(BYTE**)it) {
        DWORD f = F(DWORD, it, 96);
        if (F(int, it, 92) != kind || (f & 4) || (f & 0x80000))
            continue;
        int veh = F(int, it, 84);
        if (veh != -1 && CallC<int>(FUN_4d8a40, veh))
            continue;
        if (F(BYTE, it, 96) & 1)
            continue;
        PtrVec_Push(v, FUN_45dde0, it);
    }
    Ptrs_ToVector(out, v);
    return out;
}

// [FUN_45f130] the list nodes of kind 5 that are free
DWORD* __cdecl Select_FreeNodes5(DWORD* out, BYTE* first, BYTE* last) { return Select_FreeNodes(out, first, last, 5); }

// [FUN_45f740] the list nodes of kind 6 that are free
DWORD* __cdecl Select_FreeNodes6(DWORD* out, BYTE* first, BYTE* last) { return Select_FreeNodes(out, first, last, 6); }

// [FUN_460960] construct in place
void __cdecl Rec40c220_Construct(BYTE* p, const BYTE* s) { if (p) Rec40c220_Copy(p, 0, s); }

// [FUN_460aa0]
void __cdecl Rec4581f0_Construct(BYTE* p, const BYTE* s) { if (p) Rec4581f0_Copy(p, 0, s); }

// [FUN_460ba0]
void __cdecl Rec40cda0_Construct(BYTE* p, const BYTE* s) { if (p) Rec40cda0_Copy(p, 0, s); }

// [FUN_460d20]
void __cdecl Rec458a80_Construct(BYTE* p, const BYTE* s) { if (p) Rec458a80_Copy(p, 0, s); }

// [FUN_4610b0]
void __cdecl Rec458500_Construct(BYTE* p, const BYTE* s) { if (p) Rec458500_Copy(p, 0, s); }

// [FUN_4612d0]
void __cdecl Rec458660_Construct(BYTE* p, const BYTE* s) { if (p) Rec458660_Copy(p, 0, s); }

// [FUN_4611e0] construct a vector<char> copy in place
void __cdecl Bytes_Construct(BYTE* p, const BYTE* s)
{
    if (!p)
        return;
    F(int, p, 0) = 0;
    F(int, p, 4) = 0;
    F(int, p, 8) = 0;
    Bytes_CopyInto(p, F(const BYTE*, s, 0), F(const BYTE*, s, 4));
}

// [FUN_461400] construct in place: a record with two byte vectors and two
// vectors copied by [FUN_44dd90]; +100..+112 cache their first elements
void __cdecl Rec461400_Construct(BYTE* p, const BYTE* s)
{
    if (!p)
        return;
    F(DWORD, p, 0) = F(DWORD, s, 0);
    F(DWORD, p, 8) = F(DWORD, s, 8);
    F(DWORD, p, 12) = F(DWORD, s, 12);
    for (int k = 16; k < 48; k += 4)
        F(DWORD, p, k) = F(DWORD, s, k);
    F(DWORD, p, 48) = F(DWORD, s, 48);
    const BYTE* b = F(const BYTE*, s, 52);
    const BYTE* e = F(const BYTE*, s, 56);
    VecReserve((VecMethods*)(p + 52), e - b);
    memmove(F(BYTE*, p, 52), b, e - b);
    F(BYTE*, p, 56) = F(BYTE*, p, 52) + (e - b);
    VecReserve((VecMethods*)(p + 64), F(const BYTE*, s, 68) - F(const BYTE*, s, 64));
    F(BYTE*, p, 68) = CallC<BYTE*>(FUN_4072f0, F(const BYTE*, s, 64), F(const BYTE*, s, 68), F(BYTE*, p, 64));
    CallThis(FUN_44dd90, p + 76, s + 76);
    CallThis(FUN_44dd90, p + 88, s + 88);
    for (int k = 100; k <= 112; k += 4)
        F(DWORD, p, k) = F(DWORD, s, k);
    for (int k = 120; k <= 180; k += 4)
        F(DWORD, p, k) = F(DWORD, s, k);
    F(BYTE, p, 184) = F(BYTE, s, 184);
    F(DWORD, p, 100) = 0;
    F(DWORD, p, 104) = 0;
    F(DWORD, p, 108) = 0;
    F(DWORD, p, 112) = 0;
    static const int vec[4] = {52, 64, 76, 88};
    for (int i = 0; i < 4; i++)
        if (F(DWORD, p, vec[i]) != F(DWORD, p, vec[i] + 4))
            F(DWORD, p, 100 + i * 4) = F(DWORD, p, vec[i]);
}

// [FUN_462270] a temporary buffer for [first, last): halves the size until malloc succeeds
int* __fastcall TempBuf_Construct(int* self, void*, BYTE* first, BYTE* last)
{
    int n = (last - first) >> 2;
    self[1] = n;
    self[0] = n;
    self[2] = 0;
    if (n > 0x1fffffff)
        self[1] = 0x1fffffff;
    while (self[1] > 0) {
        self[2] = (int)malloc(self[1] * 4);
        if (self[2])
            break;
        self[1] /= 2;
    }
    if (self[1] > 0)
        CallC(FUN_46b110, self[2], self[1], first, last);
    return self;
}

// [FUN_462cc0]
void __fastcall Tree_DestroyRoot(BYTE* self, void*)
{
    CallStd(FUN_462d10, F(BYTE*, self, 4));
}

// [FUN_4640d0] quicksort partition of 12-byte name vectors around pivot (case-insensitive)
BYTE* __cdecl Names_Partition(BYTE* first, BYTE* last, char* pivot, char* pivotEnd, char* pivotCap)
{
    for (;;) {
        while (_stricmp(F(char*, first, 0), pivot) < 0)
            first += 12;
        last -= 12;
        while (_stricmp(pivot, F(char*, last, 0)) < 0)
            last -= 12;
        if (first >= last)
            break;
        DWORD tmp[3];
        const BYTE* b = F(const BYTE*, first, 0);
        const BYTE* e = F(const BYTE*, first, 4);
        VecReserve((VecMethods*)tmp, e - b);
        tmp[1] = CallC<DWORD>(FUN_4072f0, b, e, tmp[0]);
        CallThis(FUN_44c160, first, last);
        CallThis(FUN_44c160, last, tmp);
        if (tmp[2] != tmp[0])
            CallC(FUN_44b4a0, tmp[0], tmp[2] - tmp[0]);
        first += 12;
    }
    if (pivotCap != pivot)
        StlDeallocate(pivot, pivotCap - pivot);
    return first;
}

// [FUN_469850] insertion sort step: moves the name val back from last
void __cdecl Names_InsertLinear(BYTE* last, char* val, char* valEnd, char* valCap)
{
    BYTE* cur = last;
    BYTE* prev = last - 12;
    while (_stricmp(val, F(char*, prev, 0)) < 0) {
        if (prev != cur)
            Bytes_Assign(cur, F(const BYTE*, prev, 0), F(const BYTE*, prev, 4));
        cur = prev;
        prev -= 12;
    }
    Bytes_Assign(cur, (const BYTE*)val, (const BYTE*)valEnd);
    if (valCap != val)
        CallC(FUN_44b4a0, val, valCap - val);
}

}

// Searches that take the script value h by value: the callee gets a copy
// (and the selector, an empty functor), the caller's h is released after.
static int FindWithValueCopy(void (*find)(), BYTE* first, BYTE* last, DWORD* h)
{
    struct Value3 { DWORD a, b, c; } t;
    Str_CopyConstruct((StrRef*)&t, 0, (const StrRef*)h);
    int r = CallC<int>(find, first, last, t, last);
    SchValue_Release(h);
    return r;
}

// Range calls that take two iterators by value (copied with Int_Copy).
static BYTE* RangeCall(void (*fn)(), BYTE* out, DWORD first, DWORD last, int v)
{
    DWORD a, b;
    Int_Copy((int*)&b, 0, (const int*)&last);
    Int_Copy((int*)&a, 0, (const int*)&first);
    CallC(fn, out, a, b, v, v);
    return out;
}

extern "C" {

// vector<4-byte>::insert(pos, x), six more identical instances
// [FUN_44cbf0]
void __fastcall Vec4b_InsertI(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44de70]
void __fastcall Vec4b_InsertJ(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44fe90]
void __fastcall Vec4b_InsertK(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_450f70]
void __fastcall Vec4b_InsertL(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_4538a0]
void __fastcall Vec4b_InsertM(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_4561a0]
void __fastcall Vec4b_InsertN(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}


// [FUN_4544e0]
int __cdecl Find_ValueA(BYTE* f, BYTE* l, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    return FindWithValueCopy(FUN_45b190, f, l, h);
}

// [FUN_455d20]
int __cdecl Find_ValueB(BYTE* f, BYTE* l, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    return FindWithValueCopy((void (*)())Find_ByHandleA, f, l, h);
}

// [FUN_455dc0]
int __cdecl Find_ValueC(BYTE* f, BYTE* l, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    return FindWithValueCopy((void (*)())Find_ItemBySymbol, f, l, h);
}

// [FUN_455e60]
int __cdecl Find_ValueD(BYTE* f, BYTE* l, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    return FindWithValueCopy((void (*)())Find_ByHandleB, f, l, h);
}


// [FUN_453b90]
BYTE* __cdecl Range_CallA(BYTE* out, DWORD f, DWORD l, int v)
{
    return RangeCall(FUN_45adc0, out, f, l, v);
}

// [FUN_454960]
BYTE* __cdecl Range_CallB(BYTE* out, DWORD f, DWORD l, int v)
{
    return RangeCall(FUN_45b220, out, f, l, v);
}

// [FUN_456ce0]
BYTE* __cdecl Range_CallC(BYTE* out, DWORD f, DWORD l, int v)
{
    return RangeCall(FUN_45ddb0, out, f, l, v);
}

// [FUN_4579e0]
BYTE* __cdecl Range_CallD(BYTE* out, DWORD f, DWORD l, int v)
{
    return RangeCall(FUN_4607d0, out, f, l, v);
}


}

// ---------------------------------------------------------------------------
// Functions with exception frames (the frames are dropped; the normal path
// calls the destructors explicitly).

extern "C" void FUN_4a36c0(); KING_ALIAS("_FUN_4a36c0", "@SchHandle_Release@8")    // a value's destructor
extern "C" void FUN_408080(); KING_ALIAS("_FUN_408080", "_Ai_ReadSetting")    // reads the player types of a "Pair" value
extern "C" void FUN_4581a0(); KING_ALIAS("_FUN_4581a0", "_Ai_OperatorNew")    // out of memory: retries the allocation
KING_VAR(int, g_68a860, 68a860)
extern "C" BYTE* __fastcall Vec4b_Copy(BYTE* self, void*, const BYTE** s);
extern "C" BYTE* __cdecl UninitFill16(BYTE* d, unsigned n, const void* v);

static void Str_Release(StrRef* s)
{
    if (s->Owns() && s->ptr) {
        if (--F(int, s->ptr, 4) == 0 && s->ptr)
            CallVirt(s->ptr, 12, 1);
    }
}

extern "C" {

// [FUN_404ab0] destructor
void __fastcall Rec404ab0_Destroy(BYTE* self, void*)
{
    int n = (F(BYTE*, self, 228) - F(BYTE*, self, 220)) >> 2;
    if (n)
        CallC(FUN_44b4a0, F(BYTE*, self, 220), n * 4);
    BYTE* b = F(BYTE*, self, 208);
    for (BYTE* e = b; e != F(BYTE*, self, 212); e += 12)
        CallThis(FUN_404420, e);
    b = F(BYTE*, self, 208);
    n = (F(BYTE*, self, 216) - b) / 12;
    if (n)
        CallC(FUN_44b4a0, b, n * 12);
    n = F(BYTE*, self, 136) - F(BYTE*, self, 128);
    if (n)
        CallC(FUN_44b4a0, F(BYTE*, self, 128), n);
    n = (F(BYTE*, self, 120) - F(BYTE*, self, 112)) >> 2;
    if (n)
        CallC(FUN_44b4a0, F(BYTE*, self, 112), n * 4);
    SchValue_Release((DWORD*)(self + 40));
    Bytes_Free(F(BYTE*, self, 16), F(BYTE*, self, 24));
    Bytes_Free(F(BYTE*, self, 0), F(BYTE*, self, 8));
}

// [FUN_409100] walks a list of pairs (each value's +20 member, while it is
// a pair) and reads the player types of each
void __cdecl Pairs_ReadPlayerTypes(int arg, StrRef h)
{
    StrRef l;
    l.CopyFrom(&h);
    while ((BYTE)l.flags == 2) {
        StrRef m;
        Str_Member20(&m, &l);
        bool pair = (BYTE)m.flags == 2;
        Str_Release(&m);
        if (pair) {
            StrRef m2;
            Str_Member20(&m2, &l);
            DWORD pv[3];
            PairHandle_From(pv, 0, (const DWORD*)&m2);
            CallC(FUN_408080, arg, pv[0], pv[1], pv[2]);
            CallThis(FUN_4a36c0, &m2);
        }
        StrRef next;
        StrRef* nx = Str_Member32(&next, &l);
        if (l.ptr && l.Owns()) {
            if (--F(int, l.ptr, 4) == 0 && !Str_Same(&l, nx) && l.ptr)
                CallVirt(l.ptr, 12, 1);
        }
        l.CopyFrom(nx);
        Str_Release(&next);
    }
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_4a36c0, &h);
}

// [FUN_40e500] index of the vehicle with an id
int __cdecl Vehicle_IndexOf(StrRef h)
{
    StrRef c1, key, c2;
    Str_CopyConstruct(&c1, 0, &h);
    StrRef* k = Str_Construct(&key, 0, c1.ptr, c1.len, c1.flags);
    Str_CopyConstruct(&c2, 0, k);
    BYTE* end = (BYTE*)Rec_Get1((const int*)&g_vehicles, 0);
    BYTE* begin = (BYTE*)Rec_Get0((const int*)&g_vehicles, 0);
    void** it = (void**)Find_ValueB(begin, end, (DWORD)c2.ptr, c2.len, c2.flags);
    Str_Release(&key);
    if (it == g_vehiclesEnd) {
        CheckHandle(&h);
        AiError(35, 1, s_mainFile, s_mainId, 2906, "Cannot find vehicle with id", F(const char*, h.ptr, 12), &g_aiEmpty);
    }
    int idx = it - g_vehicles;
    Str_Release(&h);
    return idx;
}

// [FUN_415100] constructor: 19 weights of 1.0
BYTE* __fastcall Rec415100_Construct(BYTE* self, void*, int a)
{
    F(int, self, 0) = -1;
    F(int, self, 4) = a;
    F(double, self, 8) = 1.0;
    struct { int a, b; double w; } t = {0, 0, 1.0};
    BYTE** v = (BYTE**)(self + 16);
    v[0] = v[1] = v[2] = 0;
    BYTE* p = (BYTE*)malloc(304);
    if (!p)
        p = CallC<BYTE*>(FUN_4581a0, 304);
    v[0] = p;
    v[1] = p;
    v[2] = p + 304;
    v[1] = UninitFill16(p, 19, &t);
    F(int, self, 28) = 0;
    F(int, self, 32) = 0;
    F(int, self, 36) = 0;
    F(int, self, 40) = 0;
    return self;
}

// [FUN_415740]
int __stdcall Rec415740_Check(BYTE* p)
{
    if (F(int, p, 32) != 3)
        return 0;
    BYTE* v[3];
    Vec4b_Copy((BYTE*)v, 0, (const BYTE**)(p + 36));
    if (!PtrVec_FindFree((int*)v[0], (int*)v[1], (int*)v[2], 2))
        return 0;
    return F(BYTE*, p, 0)[g_68a860] != 0;
}

}

extern "C" void FUN_44b090(); KING_ALIAS("_FUN_44b090", "@Vec1_Free@8")    // an order info's destructor part
extern "C" void FUN_409fc0(); KING_ALIAS("_FUN_409fc0", "_Ai_RetraceCompetitorRegular")    // retraceCompetitorRegular(PlayerId player)
KING_VAR(AsmLabel, g_text77e5e0, 77e5e0)

KING_VAR(BYTE**, g_68cf00, 68cf00)
KING_VAR(int, g_68a968, 68a968)
KING_VAR(int, g_68d01c, 68d01c)
KING_VAR(int, g_68cfe4, 68cfe4)
KING_VAR(int, g_68d0e4, 68d0e4)
KING_VAR(int, g_68cf34, 68cf34)
KING_VAR(unsigned, g_rngI, 68a94c)
KING_VAR(unsigned, g_rngJ, 68a950)
typedef DWORD RngTable[55];
KING_VAR(RngTable, g_rng68a870, 68a870)
extern "C" BYTE* __fastcall Fx_InitState(BYTE* s, void*);
extern "C" DWORD* __cdecl UninitCopyDwords(const DWORD* first, const DWORD* last, DWORD* dest);
extern "C" bool __fastcall Waypoints_RemoveNode(void* self, void*, int node);
extern "C" void __cdecl Player_UpdateOrders(BYTE* p);

static const char s_actOnSemiArrival[] = "actOnSemiArrival(PlayerId player)";
static const char s_destroyPresentSilently[] = "destroyPresentSilently(PresentId present)";

extern "C" {

// [FUN_417300] actOnSemiArrival(PlayerId player)
int __cdecl Ai_ActOnSemiArrival(BYTE* p)
{
    Trace_EnterBuffered(s_actOnSemiArrival);
    BYTE** wp = (BYTE**)(p + 72);
    if (F(BYTE*, p, 24) == F(BYTE*, p, 28) && wp[0] != wp[1])
        AiError(0, 1, s_mainFile, s_mainId, 4260, "Internal error 325696", &g_aiEmpty, &g_aiEmpty);
    if ((unsigned)((wp[1] - wp[0]) / 40) > 2)
        AiError(0, 1, s_mainFile, s_mainId, 4271, "Internal error 2562", &g_aiEmpty, &g_aiEmpty);
    if (F(BYTE*, p, 24) != F(BYTE*, p, 28) && wp[0] != wp[1]) {
        int node = F(int, p, 44);
        if (node == F(int, F(BYTE*, p, 28), -12)) {
            if (!Waypoints_RemoveNode(wp, 0, node)) {
                if (wp[0] == wp[1])
                    AiError(0, 1, s_mainFile, s_mainId, 4279, "Internal error 23464", &g_aiEmpty, &g_aiEmpty);
                wp[1] -= 40;
            }
            Player_UpdateOrders(p);
            if (!g_68d188)
                CallC(FUN_409fc0, p);
        }
    }
    Trace_Leave(s_actOnSemiArrival);
    return 0;
}

// [FUN_418400] a random order (0..8) the player can take in a town, -1
// after 100 tries
int __cdecl Ai_RandomOrder(int player, int town)
{
    for (int k = 0; k < 100; k++) {
        unsigned i = (g_rngI + 1) % 55;
        g_rngI = i;
        unsigned j = (g_rngJ + 1) % 55;
        g_rngJ = j;
        g_rng68a870[i] -= g_rng68a870[j];
        int r = g_rng68a870[g_rngI] % 9;
        BYTE oi[192];
        Fx_InitState(oi, 0);
        Ai_GetOrderInfo(player, town, r, oi);
        bool ok = oi[184] != 0;
        int n = F(BYTE*, oi, 96) - F(BYTE*, oi, 88);
        if (n)
            CallC(FUN_44b4a0, F(BYTE*, oi, 88), n);
        n = F(BYTE*, oi, 84) - F(BYTE*, oi, 76);
        if (n)
            CallC(FUN_44b4a0, F(BYTE*, oi, 76), n);
        if (ok) {
            CallThis(FUN_44b090, oi + 64);
            CallThis(FUN_44b090, oi + 52);
            return r;
        }
        n = F(BYTE*, oi, 72) - F(BYTE*, oi, 64);
        if (n)
            CallC(FUN_44b4a0, F(BYTE*, oi, 64), n);
        CallThis(FUN_44b090, oi + 52);
    }
    return -1;
}

// [FUN_41ed60] destroyPresentSilently(PresentId present)
void __cdecl Ai_DestroyPresentSilently(void** present)
{
    Trace_EnterBuffered(s_destroyPresentSilently);
    if (Rec_NotValid((BYTE*)present)) {
        sprintf(g_text77e5e0, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 6794, s_invalidId, "PresentId", g_text77e5e0);
    }
    BYTE* rec = (BYTE*)present[2];
    int t = F(int, rec, 24);
    if (t != -1)
        F(DWORD, g_68cf00[t], 12) &= ~0x10u;
    switch (F(DWORD, (BYTE*)present[2], 8) & 0xf0) {
    case 16:  g_68a968--; break;
    case 32:  g_68d01c--; break;
    case 64:  g_68cfe4--; break;
    case 96:  g_68d0e4--; break;
    case 128: g_68cf34--; break;
    }
    void** out;
    List_Erase((void**)&out, present);
    Trace_Leave(s_destroyPresentSilently);
}

// [FUN_421690] the id at +12 of the first entry of a record's first list
// (else of its second list), -1 when both are empty
int* __cdecl Rec_FirstId(int* out, int idx)
{
    BYTE* r = g_68cf00[idx];
    VecMethods a, b;
    char alloc;
    Vec4_Reserve(&a, 0, (F(BYTE*, r, 24) - F(BYTE*, r, 20)) >> 2, (int)&alloc);
    a.end = (BYTE*)UninitCopyDwords(F(const DWORD*, r, 20), F(const DWORD*, r, 24), (DWORD*)a.begin);
    r = g_68cf00[idx];
    Vec4_Reserve(&b, 0, (F(BYTE*, r, 36) - F(BYTE*, r, 32)) >> 2, (int)&alloc);
    b.end = (BYTE*)UninitCopyDwords(F(const DWORD*, r, 32), F(const DWORD*, r, 36), (DWORD*)b.begin);
    if (a.begin != a.end)
        *out = F(int, *(BYTE**)a.begin, 12);
    else if (b.begin != b.end)
        *out = F(int, *(BYTE**)b.begin, 12);
    else
        *out = -1;
    int n = (b.capacityEnd - b.begin) >> 2;
    if (n)
        CallC(FUN_44b4a0, b.begin, n * 4);
    n = (a.capacityEnd - a.begin) >> 2;
    if (n)
        CallC(FUN_44b4a0, a.begin, n * 4);
    return out;
}

}

extern "C" void FUN_43bf10(); KING_ALIAS("_FUN_43bf10", "_Ai_AdvancePlayerPosition")    // a player's next position along its route
extern "C" void FUN_44c160(); KING_ALIAS("_FUN_44c160", "@PodVec_Assign@12")    // vector<int>::operator=
KING_VAR(AsmLabel, g_68b99c, 68b99c)
extern "C" BYTE** __fastcall SchList_End(BYTE** self, void*, BYTE** out);
extern "C" void __cdecl Player_EndDealOf(BYTE* p);
extern "C" void __fastcall Player_SetPosition(void* self, void*, const PositionId* p);

static const char s_trimDealerPosition[] = "trimDealerPosition(PositionId & src)";
static const char s_releaseAttachedPlayer[] = "releaseAttachedPlayer(AI_Ambush * ambush)";
static const char s_advancePlayer[] = "_advancePlayer(PlayerId player, double & d)";

extern "C" {

// [FUN_421880] trimDealerPosition(PositionId & src)
void __cdecl Ai_TrimDealerPosition(PositionId* src)
{
    Trace_EnterAppend(s_trimDealerPosition);
    if (src->road == -1 && src->node != -1) {
        int id;
        src->road = *Rec_FirstId(&id, src->node);
    }
    RoomInfo info;
    info.m_36 = -1;
    info.m_40 = -1;
    info.flags = 0;
    info.m_8 = 0;
    info.m_12 = 0;
    info.module = "UNDEFINED_MODULE";
    info.room = "UNDEFINED_ROOM";
    info.m_24 = 0;
    info.m_28 = 0;
    Road_RoomInfo(*src, &info);
    if (info.flags & 8)
        src->m_24 |= 1;
    Trace_Leave(s_trimDealerPosition);
}

// [FUN_426f20] starts the bankruptcy check task of a player whose money
// went below 0
void __cdecl Player_CheckBankrupt(BYTE* p)
{
    if (F(double, p, 136) >= 0.0)
        return;
    BYTE* o = (BYTE*)operator new(56);
    BYTE* task = o ? Bankrupt_Construct(o, 0, (int)p) : 0;
    BYTE* end;
    SchList_End((BYTE**)&g_68b99c, 0, &end);
    BYTE* it;
    ListB_Insert(&g_68b99c, 0, &it, (BYTE**)end, &task);
}

// [FUN_42b870] releaseAttachedPlayer(AI_Ambush * ambush)
void __cdecl Ai_ReleaseAttachedPlayer(BYTE* amb)
{
    Trace_EnterInline(s_releaseAttachedPlayer);
    if (F(void*, amb, 124) != g_players) {
        Player_EndDealOf(F(BYTE*, amb, 124));
        BYTE* v[3] = {0, 0, 0};
        F(DWORD, F(BYTE*, amb, 124), 96) &= 0xfff7ffff;
        F(void*, amb, 124) = g_players;
        CallThis(FUN_44c160, amb + 128, v);
        Bytes_Free(v[0], v[2]);
    }
    Trace_Leave(s_releaseAttachedPlayer);
}

// [FUN_435a90] _advancePlayer(PlayerId player, double & d)
void __cdecl Ai_AdvancePlayer(BYTE* player, double* d)
{
    Trace_EnterAppend(s_advancePlayer);
    const DWORD* pos = (const DWORD*)(player + 40);
    BYTE local[32];
    BYTE* next = CallC<BYTE*>(FUN_43bf10, local, player, pos[0], pos[1], pos[2], pos[3],
                              pos[4], pos[5], pos[6], pos[7], d, 1);
    Player_SetPosition(Iter_Data(&player, 0), 0, (const PositionId*)next);
    Trace_Leave(s_advancePlayer);
}

}

extern "C" void FUN_45aed0(); KING_ALIAS("_FUN_45aed0", "@Vec4b_InsertA@16")
extern "C" void FUN_45d370(); KING_ALIAS("_FUN_45d370", "@Vec4b_InsertD@16")

// a random element of the candidates collected in v (using the subtractive
// generator rng: table, +220 and +224 indexes); frees v
static void* PickRandom(void** v, void* none, DWORD* rng)
{
    if (v[0] == v[1]) {
        int n = ((BYTE*)v[2] - (BYTE*)v[0]) >> 2;
        if (n)
            CallC(FUN_44b4a0, v[0], n * 4);
        return none;
    }
    unsigned n = ((BYTE*)v[1] - (BYTE*)v[0]) >> 2;
    unsigned i = (rng[55] + 1) % 55;
    unsigned j = (rng[56] + 1) % 55;
    rng[55] = i;
    rng[56] = j;
    rng[i] -= rng[j];
    void* r = ((void**)v[0])[rng[rng[55]] % n];
    int bytes = (((BYTE*)v[2] - (BYTE*)v[0]) >> 2) << 2;
    if (bytes)
        CallC(FUN_44b4a0, v[0], bytes);
    return r;
}

// appends x to the vector of pointers v (growing through grow)
static void Ptrs_Push(void** v, void* x, void (*grow)())
{
    if (v[1] != v[2]) {
        if (v[1])
            *(void**)v[1] = x;
        v[1] = (BYTE*)v[1] + 4;
    } else
        CallThis(grow, v, v[1], &x);
}

extern "C" {

// [FUN_453bd0] a random town of [first, last) that trades in key
void** __cdecl Towns_Find453bd0(void** first, void** last, int key, void* rng)
{
    void* v[3] = {0, 0, 0};
    for (void** it = first; it != last; it++)
        if ((BYTE)Town_Trades((BYTE*)*it, key))
            Ptrs_Push(v, it, FUN_45aed0);
    return (void**)PickRandom(v, last, (DWORD*)rng);
}

// [FUN_453d70] the same, skipping the town except
void** __cdecl Towns_Find453d70(void** first, void** last, int key, int except, int rng)
{
    void* v[3] = {0, 0, 0};
    for (void** it = first; it != last; it++)
        if (*it != (void*)except && (BYTE)Town_Trades((BYTE*)*it, key))
            Ptrs_Push(v, it, FUN_45aed0);
    return (void**)PickRandom(v, last, (DWORD*)rng);
}

// [FUN_4566e0] a random 104-byte record whose flags (+48) lack bit 0 and
// match value under mask
BYTE* __cdecl Recs104_PickRandom(BYTE* first, BYTE* last, DWORD value, DWORD mask, void* rng)
{
    void* v[3] = {0, 0, 0};
    for (BYTE* it = first; it != last; it += 104) {
        DWORD f = F(DWORD, it, 48);
        if (!(f & 1) && (f & mask) == value)
            Ptrs_Push(v, it, FUN_45d370);
    }
    return (BYTE*)PickRandom(v, last, (DWORD*)rng);
}

}

ASM_FUNC(FUN_466300, _Rec16_UninitCopy)    // uninitialized copy of 16-byte records

extern "C" {

// [FUN_443860] copy constructor of a 144-byte order
BYTE* __fastcall Order144_CopyConstruct(BYTE* self, void*, const BYTE* s)
{
    for (int off = 0; off <= 16; off += 4)
        F(int, self, off) = F(int, s, off);
    for (int off = 24; off <= 104; off += 4)
        F(int, self, off) = F(int, s, off);
    F(int, self, 112) = F(int, s, 112);
    F(int, self, 116) = F(int, s, 116);
    BYTE** v = (BYTE**)(self + 120);
    int n = (F(BYTE*, s, 124) - F(BYTE*, s, 120)) >> 4;
    v[0] = v[1] = v[2] = 0;
    BYTE* p = n ? (BYTE*)Stl_Allocate(n * 16) : 0;
    v[0] = p;
    v[1] = p;
    v[2] = p + n * 16;
    v[1] = CallC<BYTE*>(FUN_466300, F(BYTE*, s, 120), F(BYTE*, s, 124), p, v);
    F(int, self, 132) = F(int, s, 132);
    F(int, self, 136) = F(int, s, 136);
    return self;
}

}

extern "C" void FUN_440590(); KING_ALIAS("_FUN_440590", "_Ai_GetTownNameCoordinates")    // default name coordinates of a town (out, town)
extern "C" void FUN_441ae0(); KING_ALIAS("_FUN_441ae0", "_Ai_GetParkingPointInfo")    // parking info (market, slot, info)
KING_VAR(BYTE*, g_68ccbc, 68ccbc)
KING_VAR(AsmLabel, g_text77e460, 77e460)

static const char s_getMerchandiseProperties[] = "getMerchandiseProperties(MerchandiseId id, MerchandiseProperties & mp)";
static const char s_getCoordinates[] = "getCoordinates(AI_Town * town)";
static const char s_getNameCoordinates[] = "getNameCoordinates(TownId town, int mapResolution, int screenResolution)";
static const char s_getParkingInfo[] = "getParkingInfo(PlayerId player, ParkingInfo & pi)";

extern "C" {

// [FUN_4401d0] getMerchandiseProperties(MerchandiseId id, MerchandiseProperties & mp)
int __cdecl Ai_GetMerchandiseProperties(int id, BYTE* mp)
{
    Trace_EnterAppend(s_getMerchandiseProperties);
    if (id < 0 || (unsigned)id >= (unsigned)((g_68ccbc - g_68ccb8) / 104)) {
        sprintf(g_text77e460, "(%d)", id);
        AiError(33, 100, s_mainFile, s_mainId, 16837, s_invalidId, "MerchandiseId", g_text77e460);
    }
    BYTE* r = g_68ccb8 + id * 104;
    F(int, mp, 8) = F(int, r, 48);
    F(int, mp, 16) = F(int, r, 16);
    F(int, mp, 20) = F(int, r, 20);
    F(int, mp, 24) = F(int, r, 24);
    F(int, mp, 28) = F(int, r, 28);
    F(int, mp, 32) = F(int, r, 64);
    F(int, mp, 36) = F(int, r, 68);
    F(int, mp, 4) = F(int, r, 72);
    F(int, mp, 0) = F(int, r, 88);
    Trace_Leave(s_getMerchandiseProperties);
    return 0;
}

// [FUN_440390] getCoordinates(AI_Town * town)
Vec3f* __cdecl Ai_GetCoordinates(Vec3f* out, BYTE* town)
{
    Trace_EnterAppend(s_getCoordinates);
    int id;
    Town_IdOrFirst(&id, town);
    if (id == -1) {
        const char* name = F(BYTE*, town, 0) != F(BYTE*, town, 4) ? F(const char*, town, 0) : 0;
        AiError(0, 1, s_mainFile, s_mainId, 16869, "No coordinates defined for town", name, &g_aiEmpty);
        Trace_Leave(s_getCoordinates);
        return out;
    }
    Node_Position(out, id);
    Trace_Leave(s_getCoordinates);
    return out;
}

// [FUN_440790] getNameCoordinates(TownId town, int mapResolution, int screenResolution)
DWORD* __cdecl Ai_GetNameCoordinates(DWORD* out, int town, int mapRes, int screenRes)
{
    Trace_EnterAppend(s_getNameCoordinates);
    DWORD c[3];
    CallC(FUN_440590, c, g_towns[town]);
    BYTE** v = (BYTE**)((BYTE*)g_towns[town] + (mapRes * 3 + 18) * 4);
    if ((unsigned)((v[1] - v[0]) >> 2) > (unsigned)screenRes) {
        DWORD t[3];
        const DWORD* p = (const DWORD*)Node_Position((Vec3f*)t, ((int*)v[0])[screenRes]);
        c[0] = p[0];
        c[1] = p[1];
        c[2] = p[2];
    }
    out[0] = c[0];
    out[1] = c[1];
    out[2] = c[2];
    Trace_Leave(s_getNameCoordinates);
    return out;
}

// [FUN_441d70] getParkingInfo(PlayerId player, ParkingInfo & pi)
int __cdecl Ai_GetParkingInfo(BYTE* p, BYTE* pi)
{
    Trace_EnterBuffered(s_getParkingInfo);
    int m = F(int, p, 120);
    if (m != -1) {
        BYTE* market = (BYTE*)g_68cc98[m];
        if (F(BYTE, F(BYTE*, market, 28) + F(int, p, 124) * 36, 4) & 3) {
            F(int, pi, 48) = F(int, market, 24);
            F(int, pi, 52) = F(int, p, 124);
            int r = CallC<int>(FUN_441ae0, F(int, p, 120), F(int, p, 124), pi);
            Trace_Leave(s_getParkingInfo);
            return r;
        }
    }
    Trace_Leave(s_getParkingInfo);
    return -1;
}

}

extern "C" void FUN_423f20(); KING_ALIAS("_FUN_423f20", "@Rec_Copy32@12")
extern "C" void FUN_4471f0(); KING_ALIAS("_FUN_4471f0", "@CharVec_Erase@16")
extern "C" void FUN_473680(); KING_ALIAS("_FUN_473680", "_Player_Profit")
extern "C" void FUN_41b8d0(); KING_ALIAS("_FUN_41b8d0", "_Player_VehicleCosts")
extern "C" void FUN_4773d0(); KING_ALIAS("_FUN_4773d0", "_Player_Call477350")
extern "C" void FUN_42bfe0(); KING_ALIAS("_FUN_42bfe0", "@Rec42bfe0_Assign@12")

static const char s_getNameCoordinatesT[] = "getNameCoordinates(AI_Town * town)";
static const char s_clearPlayerId[] = "clearPlayerId(VehicleInstance * vehicle, PlayerId player)";
static const char s_resetPlayerRooms[] = "resetPlayerRooms()";
static const char s_getParkingInfo3[] = "getParkingInfo(ParkingId parking, int pointid, ParkingInfo & pi)";

extern "C" {

// [FUN_440590] getNameCoordinates(AI_Town * town)
Vec3f* __cdecl Ai_GetTownNameCoordinates(Vec3f* out, BYTE* town)
{
    Trace_EnterAppend(s_getNameCoordinatesT);
    int id = F(int, town, 68);
    if (id == -1) {
        int t;
        id = *Town_IdOrFirst(&t, town);
        if (id == -1) {
            const char* name = F(BYTE*, town, 0) != F(BYTE*, town, 4) ? F(const char*, town, 0) : 0;
            AiError(0, 1, s_mainFile, s_mainId, 16880, "No coordinates defined for town", name, &g_aiEmpty);
            Trace_Leave(s_getNameCoordinatesT);
            return out;
        }
    }
    Node_Position(out, id);
    Trace_Leave(s_getNameCoordinatesT);
    return out;
}

// [FUN_4464c0] clearPlayerId(VehicleInstance * vehicle, PlayerId player)
void __cdecl Ai_ClearPlayerId(BYTE* vehicle, int player)
{
    Trace_EnterAppend(s_clearPlayerId);
    if (!vehicle) {
        Trace_Tag((const DWORD*)g_tagMpl);
        Buf_AppendString(&g_trace, 0, s_clearPlayerId);
        return;
    }
    for (BYTE* o = F(BYTE*, vehicle, 28); o != F(BYTE*, vehicle, 32); o += 144) {
        BYTE* e = F(BYTE*, o, 120);
        BYTE* end = F(BYTE*, o, 124);
        while (e != end && F(int, e, 0) != player)
            e += 16;
        if (e == end)
            continue;
        for (int n = (end - (e + 16)) >> 4; n > 0; n--, e += 16)
            CallThis(FUN_42bfe0, e, e + 16);
        F(BYTE*, o, 124) -= 16;
        CallThis(FUN_44b090, F(BYTE*, o, 124) + 4);
    }
    Trace_Leave(s_clearPlayerId);
}

// [FUN_42cd70] resetPlayerRooms()
void __cdecl Ai_ResetPlayerRooms()
{
    Trace_EnterBuffered(s_resetPlayerRooms);
    BYTE* head = (BYTE*)g_players;
    for (BYTE* n = *(BYTE**)head; n != (BYTE*)g_players; n = *(BYTE**)n) {
        RoomInfo info;
        info.flags = 0;
        info.m_8 = 0;
        info.m_12 = 0;
        info.module = "UNDEFINED_MODULE";
        info.room = "UNDEFINED_ROOM";
        info.m_24 = 0;
        info.m_28 = 0;
        info.m_36 = -1;
        info.m_40 = -1;
        BYTE* p = CallC<BYTE*>(FUN_40a9f0, n);
        PositionId tmp;
        PositionId* pos = CallThis<PositionId*>(FUN_423f20, p + 16, &tmp);
        Position_Keys(*pos, (BYTE*)&info);
        F(int, CallC<BYTE*>(FUN_40a9f0, n), 848) = info.m_40;
        F(int, CallC<BYTE*>(FUN_40a9f0, n), 844) = info.m_36;
    }
    Trace_Leave(s_resetPlayerRooms);
}

// [FUN_441ae0] getParkingInfo(ParkingId parking, int pointid, ParkingInfo & pi)
int __cdecl Ai_GetParkingPointInfo(int parking, int point, BYTE* pi)
{
    Trace_EnterBuffered(s_getParkingInfo3);
    CallThis(FUN_4471f0, pi, F(BYTE*, pi, 0), F(BYTE*, pi, 4));
    F(int, pi, 12) = 0;
    F(int, pi, 64) = 0;
    BYTE** v = (BYTE**)((BYTE*)g_68cc98[parking] + 28);
    if ((unsigned)((v[1] - v[0]) / 36) <= (unsigned)point) {
        char buf[64];
        sprintf(buf, " %d", point);
        AiError(544, 100, s_mainFile, s_mainId, 17176, "There's no such parking place: ", buf, &g_aiEmpty);
    }
    BYTE* pt = v[0] + point * 36;
    F(int, pi, 44) = F(int, pt, 0);
    BYTE* pl = F(BYTE*, pt, 8);
    if (pl != (BYTE*)g_players) {
        BYTE* q = pl + 8;
        F(BYTE*, pi, 40) = pl;
        if (F(BYTE*, q, 508)) {
            F(double, pi, 24) = CallC<double>(FUN_473680, q);
            F(int, pi, 12) = F(int, (BYTE*)g_vehicles[F(int, F(BYTE*, q, 508), 4)], 72);
            DWORD loc[3];
            loc[1] = loc[2] = 0;
            CallC(FUN_41b8d0, F(int, pt, 8), pi + 16, loc);
            BYTE* veh = (BYTE*)g_vehicles[F(int, F(BYTE*, q, 508), 4)];
            F(int, pi, 32) = F(int, veh, 56);
            F(int, pi, 36) = F(int, veh, 60);
            DWORD f = F(DWORD, pt, 4);
            if (f & 1) {
                F(int, pi, 56) = 0;
                F(int, pi, 60) = 0;
                F(DWORD, pi, 64) |= 1;
            } else if (f & 2) {
                F(double, pi, 56) = CallC<double>(FUN_4773d0, F(BYTE*, pi, 40));
                F(DWORD, pi, 64) |= 2;
            }
            if (F(BYTE, F(BYTE*, q, 508), 40) & 2)
                F(DWORD, pi, 64) |= 4;
            if (F(BYTE, F(BYTE*, q, 508), 40) & 1)
                F(DWORD, pi, 64) |= 8;
        }
    }
    Trace_Leave(s_getParkingInfo3);
    return 0;
}

}

extern "C" void FUN_41ce50(); KING_ALIAS("_FUN_41ce50", "@Rec41ce50_DestroyVector@8")    // 144-byte record destructor
extern "C" void FUN_474160(); KING_ALIAS("_FUN_474160", "@Player_SetPlace@12")    // player: set image (index)
KING_VAR(BYTE*, g_images68a958, 68a958)
KING_VAR(BYTE*, g_images68a95c, 68a95c)

static const char s_setPlayerImage[] = "setPlayerImage(PlayerId player, int imageIndex)";

extern "C" {

// [FUN_41f3d0] destructor
void __fastcall Rec41f3d0_Destroy(BYTE* self, void*)
{
    BYTE** v = (BYTE**)(self + 28);
    for (BYTE* e = v[0]; e != v[1]; e += 144)
        CallThis(FUN_41ce50, e);
    int n = (v[2] - v[0]) / 144;
    if (n)
        CallC(FUN_44b4a0, v[0], n * 144);
    int m = (F(BYTE*, self, 24) - F(BYTE*, self, 16)) >> 4;
    if (m)
        CallC(FUN_44b4a0, F(BYTE*, self, 16), m << 4);
}

// [FUN_43f950] setPlayerImage(PlayerId player, int imageIndex)
int __cdecl Ai_SetPlayerImage(BYTE* player, int image)
{
    Trace_EnterBuffered(s_setPlayerImage);
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16645, s_invalidId, "PlayerId", g_numberText);
    }
    int kind = F(int, player, 92) == 5 ? 3 : (F(int, player, 92) == 6 ? 2 : 1);
    BYTE* r = g_images68a958;
    for (; r != g_images68a95c; r += 56)
        if (F(int, r, 24) == kind && (image == -1 || image == F(int, r, 20)))
            break;
    if (r == g_images68a95c)
        AiError(0, 256, s_mainFile, s_mainId, 16658, "Can not find the requested identity", &g_aiEmpty, &g_aiEmpty);
    int k = (r - g_images68a958) / 56;
    if (F(int, player, 992) != k) {
        if (F(int, r, 28) > 0)
            AiError(0, 256, s_mainFile, s_mainId, 16665, "Identity is occupied", &g_aiEmpty, &g_aiEmpty);
        CallThis(FUN_474160, player + 8, k);
    }
    Trace_Leave(s_setPlayerImage);
    return 0;
}

}

extern "C" void FUN_4611c0(); KING_ALIAS("_FUN_4611c0", "_Construct4")
extern "C" void FUN_4561a0(); KING_ALIAS("_FUN_4561a0", "@Vec4b_InsertN@16")
extern "C" void FUN_45cf80(); KING_ALIAS("_FUN_45cf80", "_Near_StableSortBuf")    // random shuffle of pointers

extern "C" {

// [FUN_418930] a free parking place of a random market (up to 1000 tries):
// *market and *point, both -1 (and an error) when none is found
int* __cdecl Parking_RandomFree(int* market, int* point)
{
    DWORD* rng = (DWORD*)g_rng68b9c8;
    for (int tries = 0; tries < 1000; tries++) {
        unsigned count = g_68cc9c - g_68cc98;
        unsigned i = (rng[55] + 1) % 55;
        rng[55] = i;
        unsigned j = (rng[56] + 1) % 55;
        rng[56] = j;
        rng[i] -= rng[j];
        int m = rng[rng[55]] % count;
        BYTE** pts = (BYTE**)((BYTE*)g_68cc98[m] + 28);
        BYTE* v[3] = {0, 0, 0};
        for (BYTE* it = pts[0]; it != pts[1]; it += 36) {
            if (F(BYTE*, it, 8) != (BYTE*)g_players)
                continue;
            if (v[1] != v[2]) {
                CallC(FUN_4611c0, v[1], &it);
                v[1] += 4;
            } else
                CallThis(FUN_4561a0, v, v[1], &it);
        }
        if (v[0] != v[1]) {
            CallC(FUN_45cf80, v[0], v[1], 0, 0, pts);
            *point = (F(BYTE*, v[1], -4) - pts[0]) / 36;
            *market = m;
            int n = (v[2] - v[0]) >> 2;
            if (n)
                CallC(FUN_44b4a0, v[0], n * 4);
            return market;
        }
        int n = (v[2] - v[0]) >> 2;
        if (n)
            CallC(FUN_44b4a0, v[0], n * 4);
    }
    AiError(0, 100, s_mainFile, s_mainId, 4618, "AI_Parking place limit exhausted", &g_aiEmpty, &g_aiEmpty);
    *point = -1;
    *market = -1;
    return market;
}

}

extern "C" int* __cdecl Node_ByName(int* out, const char* name);
extern "C" void* __cdecl Player_CopyState(void* out, BYTE* p);

// [FUN_4171a0] a racer's progress key (+504): minus the distance to "finish1_1", or via "check1_1" before the checkpoint
extern "C" void __cdecl Race_UpdateProgress(BYTE* p)
{
    int finish, check;
    Node_ByName(&finish, "finish1_1");
    double buf[3];
    if (F(int, p, 724) == 1) {
        const double* r = (const double*)Player_CopyState(buf, p);
        const double* f = (const double*)(g_68cf00[finish] + 48);
        double dx = f[0] - r[0], dy = f[1] - r[1], dz = f[2] - r[2];
        F(double, p, 504) = -x87_sqrt((dy * dy + dz * dz) + dx * dx);
        return;
    }
    Node_ByName(&check, "check1_1");
    const double* r = (const double*)Player_CopyState(buf, p);
    const double* c = (const double*)(g_68cf00[check] + 48);
    const double* f = (const double*)(g_68cf00[finish] + 48);
    double ex = c[0] - r[0], ey = c[1] - r[1], ez = c[2] - r[2];
    double fx = c[0] - f[0], fy = c[1] - f[1], fz = c[2] - f[2];
    double le = x87_sqrt((ey * ey + ez * ez) + ex * ex);
    double lf = x87_sqrt((fx * fx + fy * fy) + fz * fz);
    F(double, p, 504) = -(le + lf);
}


// [FUN_456d20] the list nodes in [first, last) whose vehicle exists, isn't flagged 4 and is slower than 5, as a new vector
extern "C" BYTE** __cdecl Players_SlowList(BYTE** out, BYTE* first, BYTE* last)
{
    BYTE* v[4] = { 0, 0, 0, 0 };    // allocator, begin, end, capacity
    BYTE* it = first;
    while (it != last) {
        int id = F(int, it, 84);
        if (id != -1 && CallC<int>(FUN_4d8a40, id) && !((F(DWORD, it, 96) >> 2) & 1) && !(F(double, it, 344) >= 5.0)) {
            if (v[2] != v[3]) {
                if (v[2]) *(BYTE**)v[2] = it;
                v[2] += 4;
            } else
                CallThis(FUN_45dde0, &v[1], v[2], &it);
        }
        it = *(BYTE**)it;
    }
    out[0] = out[1] = out[2] = 0;
    unsigned n = (unsigned)(v[2] - v[1]) >> 2;
    BYTE* mem = n ? (BYTE*)Stl_Allocate(n * 4) : 0;
    out[0] = mem;
    out[1] = mem;
    out[2] = mem + n * 4;
    out[1] = CallC<BYTE*>(FUN_46b0e0, v[1], v[2], mem, it);
    unsigned cap = (unsigned)(v[3] - v[1]) >> 2;
    if (cap)
        List_FreeNode(v[1], cap * 4);
    return out;
}

extern "C" void FUN_436900(); KING_ALIAS("_FUN_436900", "_Ai_GetPositionIdAll")    // getPositionId(vrVector)
extern "C" void FUN_436bd0(); KING_ALIAS("_FUN_436bd0", "_Ai_GetPositionIdRoom")    // getPositionId(RoomId, vrVector)
KING_VAR(char, s_getPositionIdV, 66e538)       // "getPositionId(vrVector _v)"
KING_VAR(char, s_getPositionIdRV, 66e554)      // "getPositionId(RoomId room, vrVector _v)"
KING_VAR(BYTE*, g_roomsBegin68d0d8, 68d0d8)
KING_VAR(BYTE*, g_roomsEnd68d0dc, 68d0dc)

// [FUN_437220] getPositionId(vrVector _v): traced, the float vector widened
extern "C" int* __cdecl Ai_GetPositionIdV(int* out, float x, float y, float z)
{
    Trace_EnterInline(&s_getPositionIdV);
    double v[3] = { x, y, z };
    CallC(FUN_436900, out, v);
    Trace_Leave(&s_getPositionIdV);
    return out;
}

// [FUN_437390] getPositionId(RoomId room, vrVector _v): traced, the room checked
extern "C" int* __cdecl Ai_GetPositionIdRV(int* out, int room, float x, float y, float z)
{
    Trace_EnterAppend(&s_getPositionIdRV);
    if (room < 0 || (unsigned)room >= (unsigned)((g_roomsEnd68d0dc - g_roomsBegin68d0d8) >> 2)) {
        sprintf(g_text77e460, "(%d)", room);
        AiError(33, 100, s_mainFile, s_mainId, 14878, s_invalidId, "RoomId", g_text77e460);
    }
    double v[3] = { x, y, z };
    CallC(FUN_436bd0, out, room, v);
    Trace_Leave(&s_getPositionIdRV);
    return out;
}

KING_VAR(char, s_querySpeedLimit, 66dcf0)      // "querySpeedLimit(PlayerId player)"
KING_VAR(char, s__getTangent, 66e640)          // "_getTangent(PositionId pos)"
KING_VAR(char, s_getTangent, 66e65c)           // "getTangent(PositionId pos)"
KING_VAR(char, s_getCoordinatesPos, 66e678)       // "getCoordinates(PositionId pos)"
KING_VAR(char, s_findOnTrajectory, 66ea1c)     // "findPositionOnATrajectory(PlayerId player, vrVector const & v)"
KING_VAR(BYTE**, g_limits68a970, 68a970)
KING_VAR(BYTE**, g_limits68a974, 68a974)
KING_VAR(const double, k_noSpeedLimit, 64b880)
extern "C" void FUN_43d1f0(); KING_ALIAS("_FUN_43d1f0", "_Ai_FindPlayerPosition")

// [FUN_42c780] querySpeedLimit(PlayerId player): the limit of the zone the player is in (type 5 only)
extern "C" double __cdecl Ai_QuerySpeedLimit(BYTE* player)
{
    Trace_EnterAppend(&s_querySpeedLimit);
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 11493, s_invalidId, s_playerIdType, g_numberText);
    }
    if (F(int, player, 92) == 5)
        for (BYTE** it = g_limits68a970; it != g_limits68a974; it++)
            if (F(BYTE*, *it, 124) == player) {
                double r = F(double, *it, 232);
                Trace_Leave(&s_querySpeedLimit);
                return r;
            }
    Trace_Leave(&s_querySpeedLimit);
    return k_noSpeedLimit;
}

// the PositionId argument checks of the traced position functions
static void CheckPositionArg(const PositionId& p, bool valid, int line)
{
    if (!valid) {
        sprintf(g_positionText76, "(%d %g %d %d %x)", p.road, p.t, p.segment, p.object, p.m_24);
        AiError(33, 100, s_mainFile, s_mainId, line, "Invalid id", "PositionId", g_positionText76);
    }
    if (p.road >= 0 && (unsigned)p.road < (unsigned)(g_roadEdgesEnd - g_roadEdges)) {
        RoadEdge* e = g_roadEdges[p.road];
        if (!(p.object < e->objectsEnd - e->objects && p.segment < e->objects[p.object]->SegmentCount()))
            AiError(33, 100, s_mainFile, s_mainId, line, "Invalid Position Id", &g_aiEmpty, &g_aiEmpty);
    }
}

static bool PositionArgOk(const PositionId& p)
{
    PositionId c = p;
    return (RoadId_Valid(&c.road, 0) || NodeId_Valid(&c.node, 0)) && c.t >= -0.0002 && !(c.t > 1.0002)
        && c.segment >= 0 && c.object >= 0;
}

// [FUN_4390a0] _getTangent(PositionId pos): the road's direction at the position (reversed for backward positions)
extern "C" DVec3* __cdecl Ai__GetTangent(DVec3* out, PositionId pos)
{
    Trace_EnterBuffered(&s__getTangent);
    PositionId c = pos;
    CheckPositionArg(pos, Position_Valid(&c, 0) != 0, 15353);
    DVec3 r;
    if (pos.road != -1) {
        DVec3 t;
        CallVirt(g_roadEdges[pos.road]->objects[pos.object], 4, &t, pos.t, pos.segment);
        r = t;
    } else {
        r.x = 0; r.y = 1.0; r.z = 0;
    }
    if (pos.m_24 & 1) {
        r.x = -r.x; r.y = -r.y; r.z = -r.z;
    }
    *out = r;
    Trace_Leave(&s__getTangent);
    return out;
}

// [FUN_439320] getTangent(PositionId pos) as floats
extern "C" float* __cdecl Ai_GetTangent(float* out, PositionId pos)
{
    Trace_EnterAppend(&s_getTangent);
    CheckPositionArg(pos, PositionArgOk(pos), 15371);
    DVec3 t;
    const DVec3* r = Ai__GetTangent(&t, pos);
    out[0] = (float)r->x;
    out[1] = (float)r->y;
    out[2] = (float)r->z;
    Trace_Leave(&s_getTangent);
    return out;
}

// [FUN_439580] getCoordinates(PositionId pos) as floats
extern "C" float* __cdecl Ai_GetCoordinatesPos(float* out, PositionId pos)
{
    Trace_EnterAppend(&s_getCoordinatesPos);
    CheckPositionArg(pos, PositionArgOk(pos), 15378);
    DVec3 t;
    const DVec3* r = Road_PointOf(&t, pos);
    out[0] = (float)r->x;
    out[1] = (float)r->y;
    out[2] = (float)r->z;
    Trace_Leave(&s_getCoordinatesPos);
    return out;
}

// [FUN_43d7c0] findPositionOnATrajectory(PlayerId player, vrVector const & v)
extern "C" void* __cdecl Ai_FindPositionOnTrajectory(void* out, BYTE* player, const float* v)
{
    Trace_EnterAppend(&s_findOnTrajectory);
    double d[3] = { v[0], v[1], v[2] };
    CallC(FUN_43d1f0, out, player + 24, *(PositionId*)(player + 40), d);
    Trace_Leave(&s_findOnTrajectory);
    return out;
}

extern "C" double __cdecl Player_CargoValueOf(BYTE* p);
extern "C" double __cdecl Player_ProfitOf(BYTE* p);
KING_VAR(char, s_getPlayerVirtualCapital, 66db68)  // "getPlayerVirtualCapital(PlayerId player)"
KING_VAR(char, s_getPlayerCapital, 66db94)         // "getPlayerCapital(PlayerId player)"

// a player's money plus a value of its own vehicle (if it drives) and of its other vehicles
static double Player_CapitalWith(BYTE* p, double (__cdecl* value)(BYTE*))
{
    double sum = F(double, p, 136);
    if (F(BYTE*, p, 176) == g_playersHead) {
        sum = value(p) + sum;
        for (BYTE** it = F(BYTE**, p, 160); it != F(BYTE**, p, 164); it += 4)
            sum = value(*it) + sum;
    }
    return sum;
}

// [FUN_4293d0] getPlayerVirtualCapital(PlayerId player): money and cargo values
extern "C" double __cdecl Ai_GetPlayerVirtualCapital(BYTE* p)
{
    Trace_EnterBuffered(&s_getPlayerVirtualCapital);
    double r = Player_CapitalWith(p, Player_CargoValueOf);
    Trace_Leave(&s_getPlayerVirtualCapital);
    return r;
}

// [FUN_429530] getPlayerCapital(PlayerId player): money and profits
extern "C" double __cdecl Ai_GetPlayerCapital(BYTE* p)
{
    Trace_EnterBuffered(&s_getPlayerCapital);
    double r = Player_CapitalWith(p, Player_ProfitOf);
    Trace_Leave(&s_getPlayerCapital);
    return r;
}

KING_VAR(BYTE*, g_anchors68cf58, 68cf58)
KING_VAR(BYTE**, g_roadNodesCap68cf08, 68cf08)
KING_VAR(char, s_anode, 66c920)                    // "anode"
extern "C" void FUN_4709b0(); KING_ALIAS("_FUN_4709b0", "@RoadNode_Construct@12")    // a road node's constructor (name)
extern "C" void FUN_405c00(); KING_ALIAS("_FUN_405c00", "_Ai_BuildEdgesFrom")
extern "C" void __cdecl Construct4(DWORD* d, const DWORD* s);
extern "C" void FUN_44cbf0(); KING_ALIAS("_FUN_44cbf0", "@Vec4b_InsertI@16")

// [FUN_405f90] gives every anchor a road node (a new "anode" unless it lies on one or has two links), then resolves them
extern "C" void __cdecl Anchors_AttachNodes()
{
    for (BYTE* n = *(BYTE**)g_anchors68cf58; n != g_anchors68cf58; n = *(BYTE**)n) {
        DVec3 p = *(DVec3*)(n + 16);
        BYTE* node = (BYTE*)Road_NodeAt(p);
        if (!node && (F(int, n, 48) - F(int, n, 44)) / 12 != 2) {
            BYTE* o = (BYTE*)operator new(208);
            if (o)
                o = CallThis<BYTE*>(FUN_4709b0, o, AI_UniqueName(&s_anode));
            BYTE* nd = o;
            if (g_roadNodesEnd != (RoadNode**)g_roadNodesCap68cf08) {
                Construct4((DWORD*)g_roadNodesEnd, (DWORD*)&nd);
                g_roadNodesEnd++;
            } else
                CallThis(FUN_44cbf0, &g_roadNodes, g_roadNodesEnd, &nd);
            F(int, nd, 184) = F(int, n, 56);
            F(int, nd, 188) = F(int, n, 60);
            *(DVec3*)(nd + 48) = *(DVec3*)(n + 16);
            node = nd;
        }
        F(BYTE*, n, 40) = node;
    }
    BYTE* e = *(BYTE**)g_anchors68cf58;
    while (e != g_anchors68cf58) {
        if (!F(int, e, 8) && F(int, e, 40)) {
            CallC(FUN_405c00, e + 8);
            e = *(BYTE**)g_anchors68cf58;
        } else
            e = *(BYTE**)e;
    }
}

extern "C" void* __cdecl Player_FirstGoods(BYTE* p);
extern "C" void FUN_486800(); KING_ALIAS("_FUN_486800", "_Road_PositionCost")    // the route length between two positions
KING_VAR(char, s_updateAdvancementRegular, 66d508) // "updateAdvancementRegular(AI_Player * p, PositionId position)"

// [FUN_41a7d0] updateAdvancementRegular(AI_Player * p, PositionId position): minus the distance to the goods' town (by road beyond 100)
extern "C" void __cdecl Ai_UpdateAdvancementRegular(BYTE* p, PositionId position)
{
    Trace_EnterAppend(&s_updateAdvancementRegular);
    double r;
    if (!Player_FirstGoods(p))
        r = 0.0;
    else {
        int town;
        Town_Id(&town, g_townsBegin[F(int, Player_FirstGoods(p), 12)]);
        DVec3 pos = *(DVec3*)(p + ((F(BYTE, p, 88) & 1) ? 616 : 304));
        const DVec3* t = (const DVec3*)((BYTE*)g_roadNodes[town] + 48);
        double dx = t->x - pos.x, dy = t->y - pos.y, dz = t->z - pos.z;
        r = x87_sqrt((dy * dy + dx * dx) + dz * dz);
        if (r > 100.0) {
            double len = 0.0;
            PositionId to;
            Position_InitNode(&to, 0, town);
            CallC(FUN_486800, 0, position, to, &len);
            r = len;
        }
    }
    F(double, p, 496) = -r;
    Trace_Leave(&s_updateAdvancementRegular);
}


// [FUN_41fca0] a driving player's first cost total (0 otherwise)
extern "C" double __cdecl Player_CostA(BYTE* p)
{
    if (F(int, p, 92) == 1 || F(int, p, 92) == 2) {
        double a, b;
        Player_AllCosts(p, &a, &b);
        return a;
    }
    return 0.0;
}

// [FUN_41ba50] limits two cost estimates by each other through a record's ratio, then lowers both by 15%
extern "C" void __cdecl Costs_Balance(BYTE* p, double* a, double* b, double* out, BYTE* rec)
{
    Player_AllCosts(p, a, b);
    double t = *b * F(double, rec, 16) / F(double, rec, 24);
    if (!(t >= *a))
        *a = t;
    t = F(double, rec, 24) * *a / F(double, rec, 16);
    if (!(t >= *b))
        *b = t;
    *a = *a * 0.85;
    *b = *b * 0.85;
    *out = *a / F(double, rec, 16);
}

extern "C" void FUN_446c80(); KING_ALIAS("_FUN_446c80", "_Player_SetFlag20000")
extern "C" void __cdecl Ai_KillDelayedPlayers();
KING_VAR(double, g_camX68bab0, 68bab0)
KING_VAR(double, g_camY68bab8, 68bab8)
KING_VAR(double, g_camZ68bac0, 68bac0)

// [FUN_421c00] waiting players (state 9) far from the camera (over 2000) and idle for 900..1020 s are removed
extern "C" void __cdecl Players_DropIdleFar()
{
    for (BYTE* it = *(BYTE**)g_playersHead; it != g_playersHead; it = *(BYTE**)it) {
        if (!AI_ListData(it))
            continue;
        BYTE* p = (BYTE*)AI_ListData(it);
        if (F(int, p, 84) != 9)
            continue;
        unsigned r = AiRandom() % 120;
        double idle = CallC<double>(FUN_401000) - F(double, p, 944);
        if (!((double)(__int64)(r + 900) < idle))
            continue;
        const DVec3* pos = (const DVec3*)(p + ((F(BYTE, p, 88) & 1) ? 616 : 304));
        double dx = g_camX68bab0 - pos->x, dy = g_camY68bab8 - pos->y, dz = g_camZ68bac0 - pos->z;
        if (!(x87_sqrt((dx * dx + dy * dy) + dz * dz) > 2000.0))
            continue;
        int id;
        Int_Set(&id, 0, (int)it);
        CallC(FUN_446c80, id);
    }
    Ai_KillDelayedPlayers();
}

extern "C" PositionId* __cdecl Road_RandomPosition(PositionId* out);
extern "C" void FUN_438d70(); KING_ALIAS("_FUN_438d70", "_Road_GetRoadParameters")    // the room info of a position
KING_VAR(char, s_undefinedModule, 66d638)
KING_VAR(char, s_undefinedRoom, 66d628)

// [FUN_422e00] a random road position at least 400 from the camera, not in a room flagged 0x60
extern "C" PositionId* __cdecl Road_RandomFarPosition(PositionId* out)
{
    DWORD q[18] = {};
    q[0] = 0;
    q[2] = 0;
    q[3] = 0;
    q[4] = (DWORD)&s_undefinedModule;
    q[5] = (DWORD)&s_undefinedRoom;
    q[6] = 0;
    q[7] = 0;
    q[9] = (DWORD)-1;
    q[10] = (DWORD)-1;
    PositionId pos;
    do {
        PositionId t;
        pos = *Road_RandomPosition(&t);
        DVec3 pt;
        Road_PointOf(&pt, pos);
        double dx = pt.x - g_camX68bab0, dy = pt.y - g_camY68bab8, dz = pt.z - g_camZ68bac0;
        if (!(400.0 > x87_sqrt((dy * dy + dz * dz) + dx * dx)))
            CallC(FUN_438d70, pos, q);
    } while (q[0] & 0x60);
    *out = pos;
    return out;
}

// ---- static initializers of the AI globals ----

extern "C" void FUN_438c90(); KING_ALIAS("_FUN_438c90", "_Player_Set884")
extern "C" void FUN_447b00(); KING_ALIAS("_FUN_447b00", "@List16_Construct@12")
extern "C" void FUN_4484d0(); KING_ALIAS("_FUN_4484d0", "@PlayerList_Construct@12")
extern "C" void FUN_448cd0(); KING_ALIAS("_FUN_448cd0", "@Vec4b_Construct@20")
extern "C" void FUN_449680(); KING_ALIAS("_FUN_449680", "@List64_Construct@12")
extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")
extern "C" void FUN_4ad440(); KING_ALIAS("_FUN_4ad440", "@Sch_Construct@16")
ASM_FUNC($L_401b90, @Ai_ListConstruct@4)
extern "C" void $L_447d50(); KING_ALIAS("_$L_447d50", "@HashTable_Construct@8")
KING_VAR(char, g_init68a830, 68a830)
KING_VAR(char, g_init68a840, 68a840)
KING_VAR(char, g_init68a850, 68a850)
KING_VAR(char, g_init68a860, 68a860)
KING_VAR(char, g_init68a958, 68a958)
KING_VAR(char, g_init68a970, 68a970)
KING_VAR(char, g_init68a980, 68a980)
KING_VAR(char, g_init68b990, 68b990)
KING_VAR(char, g_init68b998, 68b998)
KING_VAR(char, g_init68b9b8, 68b9b8)
KING_VAR(char, g_init68bac8, 68bac8)
KING_VAR(char, g_init68bb58, 68bb58)
KING_VAR(char, g_init68cbe8, 68cbe8)
KING_VAR(char, g_init68cc08, 68cc08)
KING_VAR(char, g_init68cc18, 68cc18)
KING_VAR(char, g_init68cc88, 68cc88)
KING_VAR(char, g_init68cc98, 68cc98)
KING_VAR(char, g_init68cca8, 68cca8)
KING_VAR(char, g_init68ccb8, 68ccb8)
KING_VAR(char, g_init68ceb0, 68ceb0)
KING_VAR(char, g_init68cec0, 68cec0)
KING_VAR(char, g_init68cf00, 68cf00)
KING_VAR(char, g_init68cf10, 68cf10)
KING_VAR(char, g_init68cf28, 68cf28)
KING_VAR(char, g_init68cf38, 68cf38)
KING_VAR(char, g_init68cf48, 68cf48)
KING_VAR(char, g_init68cf58, 68cf58)
KING_VAR(char, g_init68cf60, 68cf60)
KING_VAR(char, g_init68cf6c, 68cf6c)
KING_VAR(char, g_init68cfd8, 68cfd8)
KING_VAR(char, g_init68cff0, 68cff0)
KING_VAR(char, g_init68d008, 68d008)
KING_VAR(char, g_init68d0a8, 68d0a8)
KING_VAR(char, g_init68d0b8, 68d0b8)
KING_VAR(char, g_init68d0c8, 68d0c8)
KING_VAR(char, g_init68d0d8, 68d0d8)

// a container constructed with a (stateless) allocator argument
static void Init_WithAllocator(void (*ctor)(), void* obj)
{
    char alloc;
    CallThis(ctor, obj, &alloc);
}

// [FUN_4012e0] constructs the global at 68bb58
extern "C" void __cdecl AI_Init68bb58()
{
    CallThis(FUN_4ad440, &g_init68bb58, 0, 0);
}

// [FUN_401360] constructs the global container at 68cf28
extern "C" void __cdecl AI_Init68cf28()
{
    Init_WithAllocator(FUN_487730, &g_init68cf28);
}

// [FUN_4013b0] constructs the global container at 68a840
extern "C" void __cdecl AI_Init68a840()
{
    Init_WithAllocator(FUN_487730, &g_init68a840);
}

// [FUN_401400] constructs the global container at 68cf48
extern "C" void __cdecl AI_Init68cf48()
{
    Init_WithAllocator(FUN_487730, &g_init68cf48);
}

// [FUN_401db0] constructs the global container at 68bac8
extern "C" void __cdecl AI_Init68bac8()
{
    Init_WithAllocator(FUN_487730, &g_init68bac8);
}

// [FUN_401e00] constructs the global container at 68ccb8
extern "C" void __cdecl AI_Init68ccb8()
{
    Init_WithAllocator(FUN_487730, &g_init68ccb8);
}

// [FUN_401e50] constructs the global container at 68cc08
extern "C" void __cdecl AI_Init68cc08()
{
    Init_WithAllocator(FUN_487730, &g_init68cc08);
}

// [FUN_401ea0] constructs the global container at 68cf00
extern "C" void __cdecl AI_Init68cf00()
{
    Init_WithAllocator(FUN_487730, &g_init68cf00);
}

// [FUN_401ef0] constructs the global container at 68cf38
extern "C" void __cdecl AI_Init68cf38()
{
    Init_WithAllocator(FUN_487730, &g_init68cf38);
}

// [FUN_401f40] constructs the global container at 68b990
extern "C" void __cdecl AI_Init68b990()
{
    Init_WithAllocator(FUN_4484d0, &g_init68b990);
}

// [FUN_401f90] constructs the global container at 68d0d8
extern "C" void __cdecl AI_Init68d0d8()
{
    Init_WithAllocator(FUN_487730, &g_init68d0d8);
}

// [FUN_401fe0] constructs the global container at 68d0c8
extern "C" void __cdecl AI_Init68d0c8()
{
    Init_WithAllocator(FUN_487730, &g_init68d0c8);
}

// [FUN_402030] constructs the global container at 68cf6c
extern "C" void __cdecl AI_Init68cf6c()
{
    Init_WithAllocator(FUN_447b00, &g_init68cf6c);
}

// [FUN_402080] constructs the global container at 68cec0
extern "C" void __cdecl AI_Init68cec0()
{
    Init_WithAllocator(FUN_487730, &g_init68cec0);
}

// [FUN_4020d0] constructs the global container at 68cc98
extern "C" void __cdecl AI_Init68cc98()
{
    Init_WithAllocator(FUN_487730, &g_init68cc98);
}

// [FUN_402120] constructs the global container at 68cbe8
extern "C" void __cdecl AI_Init68cbe8()
{
    Init_WithAllocator(FUN_487730, &g_init68cbe8);
}

// [FUN_402170] constructs the global container at 68cfd8
extern "C" void __cdecl AI_Init68cfd8()
{
    Init_WithAllocator(FUN_487730, &g_init68cfd8);
}

// [FUN_4021c0] constructs the global container at 68d0b8
extern "C" void __cdecl AI_Init68d0b8()
{
    Init_WithAllocator(FUN_487730, &g_init68d0b8);
}

// [FUN_402210] constructs the global container at 68cc18
extern "C" void __cdecl AI_Init68cc18()
{
    Init_WithAllocator(FUN_487730, &g_init68cc18);
}

// [FUN_402260] constructs the global container at 68ceb0
extern "C" void __cdecl AI_Init68ceb0()
{
    Init_WithAllocator(FUN_487730, &g_init68ceb0);
}

// [FUN_4022b0] constructs the global container at 68a970
extern "C" void __cdecl AI_Init68a970()
{
    Init_WithAllocator(FUN_487730, &g_init68a970);
}

// [FUN_402300] constructs the global container at 68a850
extern "C" void __cdecl AI_Init68a850()
{
    Init_WithAllocator(FUN_487730, &g_init68a850);
}

// [FUN_402350] constructs the global container at 68b9b8
extern "C" void __cdecl AI_Init68b9b8()
{
    Init_WithAllocator(FUN_487730, &g_init68b9b8);
}

// [FUN_4023a0] constructs the global container at 68a958
extern "C" void __cdecl AI_Init68a958()
{
    Init_WithAllocator(FUN_487730, &g_init68a958);
}

// [FUN_4025d0] constructs the global container at 68d0a8
extern "C" void __cdecl AI_Init68d0a8()
{
    Init_WithAllocator(FUN_487730, &g_init68d0a8);
}

// [FUN_402660] constructs the global container at 68a980
extern "C" void __cdecl AI_Init68a980()
{
    Init_WithAllocator(FUN_487730, &g_init68a980);
}

// [FUN_4026b0] constructs the global container at 68cf60
extern "C" void __cdecl AI_Init68cf60()
{
    Init_WithAllocator(FUN_487730, &g_init68cf60);
}

// [FUN_402700] constructs the global container at 68cc88
extern "C" void __cdecl AI_Init68cc88()
{
    Init_WithAllocator(FUN_487730, &g_init68cc88);
}

// [FUN_4056f0] constructs the global container at 68cf58
extern "C" void __cdecl AI_Init68cf58()
{
    Init_WithAllocator(FUN_449680, &g_init68cf58);
}

// [FUN_415260] constructs the global container at 68a860
extern "C" void __cdecl AI_Init68a860()
{
    Init_WithAllocator(FUN_487730, &g_init68a860);
}

// [FUN_401970] destroys the global at 68b998
extern "C" void __cdecl AI_Destroy68b998()
{
    CallThis($L_401b90, &g_init68b998);
}

// [FUN_4028b0] destroys the global at 68cff0
extern "C" void __cdecl AI_Destroy68cff0()
{
    CallThis($L_447d50, &g_init68cff0);
}

// [FUN_402b50] destroys the global at 68d008
extern "C" void __cdecl AI_Destroy68d008()
{
    CallThis($L_447d50, &g_init68d008);
}

// [FUN_402ba0] destroys the global at 68cf10
extern "C" void __cdecl AI_Destroy68cf10()
{
    CallThis($L_447d50, &g_init68cf10);
}

// [FUN_4023f0] constructs the global at 68cca8 with 12 zero elements
extern "C" void __cdecl AI_Init68cca8()
{
    DWORD zero = 0;
    char alloc;
    CallThis(FUN_448cd0, &g_init68cca8, 12, &zero, &alloc);
}

// [FUN_402450] constructs the global at 68a830 with 12 zero elements
extern "C" void __cdecl AI_Init68a830()
{
    DWORD zero = 0;
    char alloc;
    CallThis(FUN_448cd0, &g_init68a830, 12, &zero, &alloc);
}
KING_VAR(int, g_stlLock68d204, 68d204)

// [FUN_44ba80] releases the STL lock
extern "C" void __cdecl Stl_Unlock()
{
    g_stlLock68d204 = 0;
}

// [FUN_458910] does nothing (an inlined accessor whose result is unused)
extern "C" void __fastcall AI_Nop458910(void* self, int)
{
}

// [FUN_44a530] frees the storage of a vector of 8-byte elements
extern "C" void __fastcall Vec8_Free(BYTE** v, int)
{
    int n = (v[2] - v[0]) >> 3;
    if (n)
        CallC(FUN_44b4a0, v[0], n << 3);
}

// [FUN_44ab90] frees the storage of a vector of 16-byte elements
extern "C" void __fastcall Vec16_Free(BYTE** v, int)
{
    int n = (v[2] - v[0]) >> 4;
    if (n)
        CallC(FUN_44b4a0, v[0], n << 4);
}

// [FUN_44b090] frees the storage of a vector of bytes
extern "C" void __fastcall Vec1_Free(BYTE** v, int)
{
    int n = v[2] - v[0];
    if (n)
        CallC(FUN_44b4a0, v[0], n);
}

// [FUN_44b0d0] frees the storage of a vector of 4-byte elements
extern "C" void __fastcall Vec4_Free(BYTE** v, int)
{
    int n = (v[2] - v[0]) >> 2;
    if (n)
        CallC(FUN_44b4a0, v[0], n << 2);
}

// [FUN_44a500] frees the storage of a vector of 24-byte elements
extern "C" void __fastcall Vec24_Free(BYTE** v, int)
{
    int n = (int)(v[2] - v[0]) / 24;
    if (n)
        CallC(FUN_44b4a0, v[0], n * 24);
}

// [FUN_44aff0] frees the storage of a vector of 12-byte elements
extern "C" void __fastcall Vec12_Free(BYTE** v, int)
{
    int n = (int)(v[2] - v[0]) / 12;
    if (n)
        CallC(FUN_44b4a0, v[0], n * 12);
}

// [FUN_453b60] frees all nodes (12 bytes) of a list, leaving the head empty
extern "C" void __fastcall List12_Clear(void*** list, int)
{
    void** node = (void**)**list;
    while (node != *list) {
        void** next = (void**)*node;
        CallC(FUN_44b4a0, node, 12);
        node = next;
    }
    void** head = *list;
    head[0] = head;
    head[1] = head;
}

// [FUN_44f650] the length of a string
extern "C" int __cdecl AI_StrLen(const char* s)
{
    const char* p = s;
    while (*p)
        ++p;
    return (int)(p - s);
}

// [FUN_4011e0] calls a thiscall function on each of count elements of size bytes
extern "C" void __stdcall Array_ForEach(BYTE* p, int size, int count, void (*fn)())
{
    for (; count > 0; --count) {
        CallThis(fn, p);
        p += size;
    }
}

// [FUN_438d00] the room flags of an id, passed by value, with mask 4096
extern "C" int __cdecl AI_RoomFlags4096(int id)
{
    return CallC<int>(FUN_438c90, id, 4096);
}

KING_VAR(AsmLabel, s_recIdType66d64c, 66d64c)
KING_VAR(AsmLabel, s_suffix66c4e4, 66c4e4)
KING_VAR(AsmLabel, s_fmt66d9c8, 66d9c8)
KING_VAR(AsmLabel, s_fmt66d9c4, 66d9c4)
KING_VAR(AsmLabel, s_fmt66daa8, 66daa8)
KING_VAR(AsmLabel, s_prefix66c7ec, 66c7ec)
KING_VAR(char, g_text68cdac, 68cdac)
KING_VAR(double, g_time68d1b8, 68d1b8)
KING_VAR(void**, g_list68b99c, 68b99c)
extern "C" void FUN_477210(); KING_ALIAS("_FUN_477210", "_Player_UpdateFlags")    // updates a player's flags
extern "C" void FUN_56ae60(); KING_ALIAS("_FUN_56ae60", "_Messages_Add")    // adds a message to the message list
extern "C" void FUN_403940(); KING_ALIAS("_FUN_403940", "_Ai_Reset")

// [FUN_43f700] the value at +8 of a record id's record
extern "C" int __cdecl Rec_Field8(BYTE* id)
{
    if (Rec_NotValid(id)) {
        sprintf(g_text77e5e0, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16594, s_invalidId, s_recIdType66d64c, g_text77e5e0);
    }
    return F(int, F(BYTE*, id, 8), 8);
}

// [FUN_43f760] the double at +136 of a player
extern "C" double __cdecl Player_Value136(BYTE* id)
{
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16607, s_invalidId, s_playerIdType, g_numberText);
    }
    return F(double, id, 136);
}

// [FUN_446c80] sets player flag 0x20000
extern "C" void __cdecl Player_SetFlag20000(BYTE* id)
{
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 18335, s_invalidId, s_playerIdType, g_numberText);
    }
    F(DWORD, id, 96) |= 0x20000;
}

// [FUN_438c90] sets the player's value at +884
extern "C" void __cdecl Player_Set884(BYTE* id, int v)
{
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 15275, s_invalidId, s_playerIdType, g_numberText);
    }
    F(int, id, 884) = v;
}

// [FUN_43f620] a player's flags (0 for an invalid player), updated first
extern "C" int __cdecl Player_Flags(BYTE* id)
{
    if (F(DWORD, id, 88) != 0x12345678)
        return 0;
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16571, s_invalidId, s_playerIdType, g_numberText);
    }
    CallC(FUN_477210, id);
    return F(int, id, 96);
}

// [FUN_43f690] sets a player's flags and updates them
extern "C" void __cdecl Player_SetFlags(BYTE* id, int flags)
{
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16578, s_invalidId, s_playerIdType, g_numberText);
    }
    F(int, id, 96) = flags;
    CallC(FUN_477210, id);
}

// [FUN_40e4c0] the text of a message followed by the suffix, in a static buffer
extern "C" char* __cdecl Message_TextWithSuffix(const char* id)
{
    strncpy(&g_text68cdac, Message_Text(id), 256);
    strncat(&g_text68cdac, s_suffix66c4e4, 256);
    return &g_text68cdac;
}

// [FUN_426ab0] adds the message of a numbered id to the message list
extern "C" void __cdecl Messages_AddNumbered(int n)
{
    char buf[128];
    sprintf(buf, s_fmt66d9c8, n);
    sprintf(buf, s_fmt66d9c4, Message_Text(buf));
    CallC(FUN_56ae60, buf);
}

// [FUN_427ee0] notes the time and shows the message of a numbered id on screen
extern "C" void __cdecl Messages_ShowNumbered(int n)
{
    char buf[128];
    g_time68d1b8 = CallC<double>(FUN_401000);
    sprintf(buf, s_fmt66daa8, n);
    sprintf(buf, s_fmt66d9c4, Message_Text(buf));
    CallC(FUN_483cb0, buf);
}

// [FUN_42db90] frees the list at 68b99c: each node's buffer, then the nodes (12 bytes)
extern "C" void __cdecl AI_FreeList68b99c()
{
    CallC(FUN_403940);
    void** head = g_list68b99c;
    for (void** n = (void**)*head; n != g_list68b99c; n = (void**)*n)
        operator delete(n[2]);
    head = g_list68b99c;
    void** n = (void**)*head;
    while (n != g_list68b99c) {
        void** next = (void**)*n;
        CallC(FUN_44b4a0, n, 12);
        n = next;
    }
    head = g_list68b99c;
    head[0] = head;
    g_list68b99c[1] = g_list68b99c;
}

// [FUN_4018f0] appends the 8-byte prefix and then the string to a text buffer {+8 capacity, +12 data, +16 length}
extern "C" void __fastcall Buf_AppendPrefixed(void** self, int)
{
    BYTE* b = (BYTE*)self[1];
    if (F(char*, b, 12) && F(int, b, 16) + 8 < F(int, b, 8)) {
        char* d = F(char*, b, 12) + F(int, b, 16);
        ((DWORD*)d)[0] = ((DWORD*)s_prefix66c7ec)[0];
        ((DWORD*)d)[1] = ((DWORD*)s_prefix66c7ec)[1];
        F(int, b, 16) += 8;
    }
    b = (BYTE*)self[1];
    const char* s = (const char*)self[0];
    if (!F(char*, b, 12))
        return;
    for (char c = *s; c && F(int, b, 16) < F(int, b, 8); c = *s) {
        ++s;
        F(char*, b, 12)[F(int, b, 16)] = c;
        F(int, b, 16)++;
    }
}

extern "C" void FUN_473540(); KING_ALIAS("_FUN_473540", "@Player_SetVrId@12")    // sets a player's vehicle record id

// [FUN_43fb60] sets a player's vehicle id, also in the vehicle at +516
extern "C" void __cdecl Player_SetVehicleId(BYTE* id, int v)
{
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16675, s_invalidId, s_playerIdType, g_numberText);
    }
    CallThis(FUN_473540, id + 8, v);
    int* p = F(int*, id, 516);
    if (p)
        *p = v;
}

// [FUN_43fbd0] sets the first field of a record, also in its mirror at +20
extern "C" void __cdecl Rec_SetField0(BYTE* id, int v)
{
    if (Rec_NotValid(id)) {
        sprintf(g_text77e5e0, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16685, s_invalidId, s_recIdType66d64c, g_text77e5e0);
    }
    F(int, F(BYTE*, id, 8), 0) = v;
    int* m = F(int*, F(BYTE*, id, 8), 20);
    if (m)
        *m = v;
}

// [FUN_447180] sets a player's byte at +976 and the pair at +984
extern "C" void __cdecl Player_Set976(BYTE* id, char c, int a, int b)
{
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 18441, s_invalidId, s_playerIdType, g_numberText);
    }
    F(char, id, 976) = c;
    F(int, id, 984) = a;
    F(int, id, 988) = b;
}

extern "C" void FUN_426f20(); KING_ALIAS("_FUN_426f20", "_Player_CheckBankrupt")    // checks whether a player went bankrupt
KING_VAR(BYTE*, g_places68a958b, 68a958)
KING_VAR(BYTE*, g_players68b990b, 68b990)

// [FUN_40c1a0] destructor of a value with a buffer {+88 begin, +96 capacity}: frees the buffer, releases the handle
extern "C" void __fastcall ValueBuf_Destroy(void** self, int)
{
    BYTE* b = F(BYTE*, self, 88);
    int n = F(BYTE*, self, 96) - b;
    if (n)
        CallC(FUN_44b4a0, b, n);
    if (F(DWORD, self, 8) & 0x10000)
        return;
    BYTE* obj = (BYTE*)self[0];
    if (!obj)
        return;
    --F(int, obj, 4);
    obj = (BYTE*)self[0];
    if (F(int, obj, 4) == 0 && obj)
        CallThis((*(void (***)())obj)[3], obj, 1);
}

// [FUN_43f7c0] sets a player's double at +136 and checks for bankruptcy
extern "C" void __cdecl Player_Set136(BYTE* id, DWORD lo, DWORD hi)
{
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16613, s_invalidId, s_playerIdType, g_numberText);
    }
    F(DWORD, id, 136) = lo;
    F(DWORD, id, 140) = hi;
    CallC(FUN_426f20, id);
}

// [FUN_43f8d0] the value at +20 of the player's place (-1 without a place)
extern "C" int __cdecl Player_PlaceField20(BYTE* id)
{
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16634, s_invalidId, s_playerIdType, g_numberText);
    }
    int i = F(int, id, 992);
    if (i == -1)
        return -1;
    return F(int, g_places68a958b + i * 56, 20);
}

// [FUN_43f840] the player's value at +156, or its leader's; -1 for a player without leader and with an empty list
extern "C" int __cdecl Player_Value156(BYTE* id)
{
    if (Player_NotValid(id)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 16620, s_invalidId, s_playerIdType, g_numberText);
    }
    BYTE* leader = F(BYTE*, id, 176);
    if (leader == g_players68b990b && F(int, id, 160) == F(int, id, 164))
        return -1;
    if (leader != g_players68b990b)
        return F(int, leader, 156);
    return F(int, id, 156);
}

extern "C" void FUN_417660(); KING_ALIAS("_FUN_417660", "_Player_IsHuman")    // whether a player is human
extern "C" void FUN_4611c0(); KING_ALIAS("_FUN_4611c0", "_Construct4")    // constructs a dword at p
extern "C" void FUN_4538a0(); KING_ALIAS("_FUN_4538a0", "@Vec4b_InsertM@16")    // vector of dwords: insert (pos, &x)
extern "C" void FUN_4566e0(); KING_ALIAS("_FUN_4566e0", "_Recs104_PickRandom")    // picks a random record of 104 bytes
extern "C" void FUN_456880(); KING_ALIAS("_FUN_456880", "_Pick_RandomMasked")    // picks a random record of 104 bytes by mask
KING_VAR(BYTE**, g_vec68cf60, 68cf60)
KING_VAR(BYTE**, g_vec68cf64, 68cf64)
KING_VAR(BYTE**, g_vec68cf68, 68cf68)
KING_VAR(BYTE**, g_vec68a980, 68a980)
KING_VAR(BYTE**, g_vec68a984, 68a984)
KING_VAR(BYTE**, g_vec68a988, 68a988)
KING_VAR(BYTE*, g_recs68ccb8, 68ccb8)
KING_VAR(BYTE*, g_recs68ccbc, 68ccbc)
KING_VAR(char, g_rand68b9c8, 68b9c8)
KING_VAR(AsmLabel, s_fmt66d58c, 66d58c)
KING_VAR(char, g_empty68d1c0, 68d1c0)

// adds a player to the vector at vec (begin, end, capacity) unless it is in it
static void PlayerVec_AddUnique(BYTE*** vec, BYTE* id)
{
    BYTE** e = vec[1];
    BYTE** p = vec[0];
    for (; p != e && *p != id; p++)
        ;
    if (p != e)
        return;
    if (e != vec[2]) {
        CallC(FUN_4611c0, e, &id);
        vec[1] = vec[1] + 1;
    } else {
        CallThis(FUN_4538a0, vec, e, &id);
    }
}

// [FUN_417770] takes a computer player out of the game (state 5, flag 0x4000) and lists it
extern "C" void __cdecl Player_Retire(BYTE* id)
{
    if (!CallC<char>(FUN_417660, id))
        PlayerVec_AddUnique((BYTE***)&g_vec68cf60, id);
    F(int, id, 100) = 5;
    F(DWORD, id, 96) |= 0x4000;
    CallC(FUN_477210, id);
    F(BYTE, id, 872) = 1;
}

// [FUN_4175c0] marks a computer player (+496) and lists it
extern "C" void __cdecl Player_MarkListed(BYTE* id)
{
    if (!CallC<char>(FUN_417660, id)) {
        PlayerVec_AddUnique((BYTE***)&g_vec68a980, id);
        F(BYTE, id, 496) = 1;
    }
    CallC(FUN_477210, id);
    F(BYTE, id, 872) = 1;
}

// the index of a 104-byte record, or an error and -1
static int* Recs104_IndexOr(int* out, BYTE* r, int a, int b, int line)
{
    if (r != g_recs68ccbc) {
        *out = (int)(r - g_recs68ccb8) / 104;
        return out;
    }
    char buf[128];
    sprintf(buf, s_fmt66d58c, a, b);
    AiError(0, 200, s_mainFile, s_mainId, line, buf, &g_empty68d1c0, &g_empty68d1c0);
    *out = -1;
    return out;
}

// [FUN_41b120] the index of a random record of 104 bytes matching (a, b)
extern "C" int* __cdecl Recs104_Random(int* out, int a, int b)
{
    BYTE* r = CallC<BYTE*>(FUN_4566e0, g_recs68ccb8, g_recs68ccbc, a, b, &g_rand68b9c8);
    return Recs104_IndexOr(out, r, a, b, 5689);
}

// [FUN_41b1e0] the index of a random record of 104 bytes matching (a, b) by mask
extern "C" int* __cdecl Recs104_RandomMasked(int* out, int a, int b)
{
    BYTE* r = CallC<BYTE*>(FUN_456880, g_recs68ccb8, g_recs68ccbc, a, b, &g_rand68b9c8);
    return Recs104_IndexOr(out, r, a, b, 5726);
}

extern "C" void FUN_44ac60(); KING_ALIAS("_FUN_44ac60", "_Vec4b_AllocCopy@12")    // vector of dwords: allocate n and copy [first, last)

// [FUN_440dd0] frees the four vectors at +52, +64, +76 and +88
extern "C" void __fastcall Rec440dd0_FreeVectors(BYTE* self, int)
{
    for (int off = 88; off >= 52; off -= 12) {
        BYTE* b = F(BYTE*, self, off);
        int n = F(BYTE*, self, off + 8) - b;
        if (n)
            CallC(FUN_44b4a0, b, n);
    }
}

// [FUN_452290] vector<dword>::operator=
extern "C" DWORD** __fastcall VecDword_Assign(DWORD** self, int, DWORD** src)
{
    if (src == self)
        return self;
    DWORD* sb = src[0];
    DWORD* se = src[1];
    unsigned n = se - sb;
    DWORD* b = self[0];
    if (n > (unsigned)(self[2] - b)) {
        DWORD* p = CallStd<DWORD*>(FUN_44ac60, n, sb, se);
        unsigned cap = self[2] - self[0];
        if (cap)
            CallC(FUN_44b4a0, self[0], cap * 4);
        self[2] = p + n;
        self[0] = p;
        self[1] = p + n;
        return self;
    }
    unsigned size = self[1] - b;
    if (size >= n) {
        memmove(b, sb, n * 4);
        self[1] = self[0] + n;
        return self;
    }
    memmove(b, sb, size * 4);
    DWORD* e = self[1];
    DWORD* mid = src[0] + (e - self[0]);
    memmove(e, mid, (src[1] - mid) * 4);
    self[1] = self[0] + n;
    return self;
}

extern "C" void FUN_4015a0(); KING_ALIAS("_FUN_4015a0", "_Ai_PlayerEvent")
extern "C" void FUN_457b50(); KING_ALIAS("_FUN_457b50", "_CopyDwords")    // copies dwords (first, last, dest)
extern "C" void FUN_409fc0(); KING_ALIAS("_FUN_409fc0", "_Ai_RetraceCompetitorRegular")
extern "C" void FUN_42b060(); KING_ALIAS("_FUN_42b060", "_Rec_SetActive")    // marks a record (in)active
extern "C" void FUN_484280(); KING_ALIAS("_FUN_484280", "_Road_ResetSides")    // resets the road sides
extern "C" void FUN_4bd510(); KING_ALIAS("_FUN_4bd510", "@Fx_Self@8")
extern "C" void FUN_455950(); KING_ALIAS("_FUN_455950", "@Rec_Get1@8")
extern "C" void FUN_455940(); KING_ALIAS("_FUN_455940", "@Rec_Get0@8")
extern "C" void FUN_40a1d0(); KING_ALIAS("_FUN_40a1d0", "_Player_CheckRouteStart")    // checks a player's route start
extern "C" void FUN_425610(); KING_ALIAS("_FUN_425610", "_AI_RandomTraffic")    // random traffic
KING_VAR(int, g_count68d164, 68d164)
typedef void (__cdecl* PresentCb)(int, int);
KING_VAR(PresentCb, g_presentCb68d11c, 68d11c)
KING_VAR(int*, g_edges68cf38, 68cf38)
KING_VAR(BYTE**, g_recs68cc18b, 68cc18)
KING_VAR(BYTE**, g_recs68cc1cb, 68cc1c)

// [FUN_420000] FUN_4015a0 of a record with 1 and with 9
extern "C" void __cdecl Rec_4015a0Both(int id)
{
    CallC(FUN_4015a0, id, 1, 0);
    CallC(FUN_4015a0, id, 9, 0);
}

// [FUN_40a180] FUN_409fc0 for every player
extern "C" void __cdecl Players_Each409fc0()
{
    for (BYTE** it = *(BYTE***)g_players68b990b; it != (BYTE**)g_players68b990b; it = (BYTE**)*it)
        CallC(FUN_409fc0, (int)it);
}

// [FUN_41ef10] destroys a present: the callback (kind 4), then silently
extern "C" void __cdecl Present_Destroy(BYTE* id)
{
    if (Rec_NotValid(id)) {
        sprintf(g_text77e5e0, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 6830, s_invalidId, s_recIdType66d64c, g_text77e5e0);
    }
    g_presentCb68d11c((int)id, 4);
    Ai_DestroyPresentSilently((void**)id);
}

// [FUN_4256f0] empties the listed players, clears every player's mark at +488 and updates it, then random traffic
extern "C" void __cdecl Players_ResetMarks()
{
    g_count68d164 = 0;
    BYTE** b = (BYTE**)g_vec68a980;
    BYTE** e = (BYTE**)g_vec68a984;
    CallC(FUN_457b50, e, e, b);
    g_vec68a984 = (BYTE**)((BYTE**)g_vec68a984 - (e - b));
    for (BYTE** it = *(BYTE***)g_players68b990b; it != (BYTE**)g_players68b990b; it = (BYTE**)*it) {
        BYTE* r = AI_ListData((BYTE*)it);
        F(BYTE, r, 488) = 0;
        CallC(FUN_477210, (int)it);
    }
    CallC(FUN_425610);
}

// [FUN_42b170] activates one record of the list at 68cc18 (the others off); then checks the route starts on its edge
extern "C" int __cdecl Rec_ActivateOnly(BYTE* r)
{
    for (BYTE** p = g_recs68cc18b; p != g_recs68cc1cb; p++)
        if (*p != r)
            CallC(FUN_42b060, *p, 0);
    CallC(FUN_42b060, r, 1);
    if (F(int, r, 56) == -1)
        return 0;
    CallC(FUN_484280);
    for (BYTE** it = *(BYTE***)g_players68b990b; it != (BYTE**)g_players68b990b; it = (BYTE**)*it) {
        int edge = g_edges68cf38[F(int, r, 56)];
        BYTE* x = CallThis<BYTE*>(FUN_4bd510, AI_ListData((BYTE*)it) + 16);
        BYTE* last = CallThis<BYTE*>(FUN_455950, x);
        x = CallThis<BYTE*>(FUN_4bd510, AI_ListData((BYTE*)it) + 16);
        BYTE* s = CallThis<BYTE*>(FUN_455940, x);
        for (; s != last && F(int, s, 4) != edge; s += 12)
            ;
        if (s != F(BYTE*, AI_ListData((BYTE*)it), 20))
            CallC(FUN_40a1d0, (int)it);
    }
    return 0;
}

KING_VAR(char, s_type66ce4c, 66ce4c)
ASM_FUNC(FUN_466300, _Rec16_UninitCopy)    // copy-constructs 16-byte records (first, last, dest, -)
ASM_FUNC(FUN_466200, _Rec24_UninitCopy)    // copy-constructs 24-byte records (first, last, dest, -)
extern "C" void FUN_40cc50(); KING_ALIAS("_FUN_40cc50", "@Rec40cc50_Destroy@8")    // destroys a 24-byte element

// [FUN_44ca00] a handle copied from a value of type 9, an error for another type
extern "C" void** __fastcall Handle9_Init(void** self, int, void** v)
{
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    BYTE* obj = (BYTE*)v[0];
    if (!obj) {
        self[0] = 0;
        self[2] = 0;
        return self;
    }
    if (CallVirt<int>(obj, 0) != 9) {
        int name = CallVirt<int>(v[0], 4);
        CallC(FUN_4a54e0, 2, 1, name, &s_type66ce4c);
        return self;
    }
    self[0] = v[0];
    self[2] = v[2];
    ++*(int*)((BYTE*)v[0] + 4);
    return self;
}

// [FUN_44dd90] vector<char> copy constructor
extern "C" BYTE** __fastcall VecChar_Copy(BYTE** self, int, BYTE* const* src)
{
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    int n = src[1] - src[0];
    BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n) : 0;
    self[0] = p;
    self[1] = p;
    self[2] = p + n;
    BYTE* b = src[0];
    BYTE* e = src[1];
    memmove(p, b, e - b);
    self[1] = p + (e - b);
    return self;
}

// [FUN_450ce0] copy constructor of a vector of 16-byte records with a trailing field
extern "C" BYTE** __fastcall Vec16Rec_Copy(BYTE** self, int, BYTE* const* src)
{
    unsigned n = (src[1] - src[0]) >> 4;
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n * 16) : 0;
    self[0] = p;
    self[1] = p;
    self[2] = p + n * 16;
    self[1] = CallC<BYTE*>(FUN_466300, src[0], src[1], p, 0);
    self[3] = src[3];
    return self;
}

// [FUN_450850] copy constructor of a vector of 24-byte records
extern "C" BYTE** __fastcall Vec24Rec_Copy(BYTE** self, int, BYTE* const* src)
{
    int n = (int)(src[1] - src[0]) / 24;
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n * 24) : 0;
    self[0] = p;
    self[1] = p;
    self[2] = p + n * 24;
    self[1] = CallC<BYTE*>(FUN_466200, src[0], src[1], p, n);
    return self;
}

// [FUN_40cb60] destructor: the vector of 24-byte elements at +12, then the handle
extern "C" void __fastcall Rec40cb60_Destroy(BYTE* self, int)
{
    BYTE* b = F(BYTE*, self, 12);
    BYTE* e = F(BYTE*, self, 16);
    for (BYTE* p = b; p != e; p += 24)
        CallThis(FUN_40cc50, p);
    b = F(BYTE*, self, 12);
    int n = (int)(F(BYTE*, self, 20) - b) / 24;
    if (n)
        CallC(FUN_44b4a0, b, n * 24);
    if (F(DWORD, self, 8) & 0x10000)
        return;
    BYTE* obj = F(BYTE*, self, 0);
    if (!obj)
        return;
    --F(int, obj, 4);
    obj = F(BYTE*, self, 0);
    if (F(int, obj, 4) == 0 && obj)
        CallThis((*(void (***)())obj)[3], obj, 1);
}

KING_VAR(char, vt_64b62c, 64b62c)

// frees the storage [begin, capacity) of a vector of elem-byte elements
static void VecStorage_Free(BYTE* begin, BYTE* cap, int elem)
{
    int n = (int)(cap - begin) / elem;
    if (n)
        CallC(FUN_44b4a0, begin, n * elem);
}

// [FUN_404420] frees the storage of a vector of bytes
extern "C" void __fastcall VecB_FreeStorage(BYTE** v, int)
{
    VecStorage_Free(v[0], v[2], 1);
}

// [FUN_40cc50] frees the storage of the byte vector at +8
extern "C" void __fastcall Rec40cc50_Destroy(BYTE* self, int)
{
    VecStorage_Free(F(BYTE*, self, 8), F(BYTE*, self, 16), 1);
}

// [FUN_447970] frees the storage of a vector of 4-byte elements
extern "C" void __fastcall Vec4_FreeStorage(BYTE** v, int)
{
    VecStorage_Free(v[0], v[2], 4);
}

// [FUN_448ab0] frees the storage of a vector of 4-byte elements
extern "C" void __fastcall Vec4_FreeStorageB(BYTE** v, int)
{
    VecStorage_Free(v[0], v[2], 4);
}

// [FUN_4473e0] frees the storage of a vector of 8-byte elements
extern "C" void __fastcall Vec8_FreeStorage(BYTE** v, int)
{
    VecStorage_Free(v[0], v[2], 8);
}

// [FUN_447ef0] frees the storage of a vector of 12-byte elements
extern "C" void __fastcall Vec12_FreeStorage(BYTE** v, int)
{
    VecStorage_Free(v[0], v[2], 12);
}

// [FUN_4488e0] frees the storage of a vector of 16-byte elements
extern "C" void __fastcall Vec16_FreeStorage(BYTE** v, int)
{
    VecStorage_Free(v[0], v[2], 16);
}

// [FUN_447270] frees the storage of a vector of 24-byte elements
extern "C" void __fastcall Vec24_FreeStorage(BYTE** v, int)
{
    VecStorage_Free(v[0], v[2], 24);
}

// [FUN_405870] frees the storage of the vector of 12-byte elements at +36
extern "C" void __fastcall Rec405870_Destroy(BYTE* self, int)
{
    VecStorage_Free(F(BYTE*, self, 36), F(BYTE*, self, 44), 12);
}

// [FUN_4549a0] destructor (vtable 64b62c): frees the vector of 8-byte elements at +40 of the base at +8
extern "C" void __fastcall Rec4549a0_Destroy(BYTE* self, int)
{
    *(void**)self = &vt_64b62c;
    BYTE* base = self ? self + 8 : 0;
    VecStorage_Free(F(BYTE*, base, 32), F(BYTE*, base, 40), 8);
}

// [FUN_448610] destructor of a vector of 16-byte elements that each own a byte vector at +4
extern "C" void __fastcall VecByteVec16_Destroy(BYTE** v, int)
{
    BYTE* e = v[1];
    for (BYTE* p = v[0]; p != e; p += 16)
        VecB_FreeStorage((BYTE**)(p + 4), 0);
    VecStorage_Free(v[0], v[2], 16);
}

extern "C" void __fastcall CharVec_InsertRange(BYTE** self, void*, BYTE* pos, BYTE* first, BYTE* last);

// [FUN_44ceb0] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert44ceb0(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44d4e0] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert44d4e0(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44d7a0] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert44d7a0(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44e130] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert44e130(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44e3f0] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert44e3f0(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44e6b0] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert44e6b0(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44e970] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert44e970(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44ec30] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert44ec30(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44eef0] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert44eef0(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_451a80] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert451a80(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_451d40] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert451d40(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_452900] vector<4 bytes>::insert_aux (the same template as FUN_45cb00)
extern "C" void __fastcall Vec4b_Insert452900(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_44c220] vector<char>::insert(pos, first, last) (the same template as FUN_46e460)
extern "C" void __fastcall CharVec_InsertRange44c220(BYTE** self, void*, BYTE* pos, BYTE* first, BYTE* last)
{
    CharVec_InsertRange(self, 0, pos, first, last);
}

extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")    // uninitialized copy of 12-byte records (first, last, dest, -)
extern "C" void FUN_47e220(); KING_ALIAS("_FUN_47e220", "_Stl_UninitCopy16")    // uninitialized copy of 16-byte records (first, last, dest, -)

// vector<n-byte POD>::insert_aux(pos, x), the STL template behind several procs;
// copy is the instance's (uninitialized) copy (first, last, dest, -, 0)
extern "C" void __cdecl VecPod_InsertAux(BYTE** v, BYTE* pos, const BYTE* x, int n, void (*copy)())
{
    if (v[1] != v[2]) {
        if (v[1])
            memcpy(v[1], v[1] - n, n);
        v[1] += n;
        BYTE t[64];
        memcpy(t, x, n);
        BYTE* src = v[1] - 2 * n;
        BYTE* dst = v[1] - n;
        for (int k = (int)(src - pos) / n; k > 0; k--) {
            src -= n;
            dst -= n;
            memcpy(dst, src, n);
        }
        memcpy(pos, t, n);
        return;
    }
    int size = (int)(v[1] - v[0]) / n;
    int len = size ? 2 * size : 1;
    BYTE* p = len ? CallC<BYTE*>(FUN_44b740, len * n) : 0;
    BYTE* q = CallC<BYTE*>(copy, v[0], pos, p, pos, 0);
    if (q)
        memcpy(q, x, n);
    BYTE* e = CallC<BYTE*>(copy, pos, v[1], q + n, pos, 0);
    int cap = (int)(v[2] - v[0]) / n;
    if (cap)
        CallC(FUN_44b4a0, v[0], cap * n);
    v[0] = p;
    v[1] = e;
    v[2] = p + len * n;
}

// [FUN_4514b0] vector<4 bytes>::insert_aux
extern "C" void __fastcall Vec4b_Insert4514b0(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_452000] vector<4 bytes>::insert_aux
extern "C" void __fastcall Vec4b_Insert452000(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_4545d0] vector<12 bytes>::insert_aux
extern "C" void __fastcall Vec12_Insert4545d0(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    VecPod_InsertAux(v, pos, x, 12, FUN_47e3e0);
}

// [FUN_451740] vector<16 bytes>::insert_aux
extern "C" void __fastcall Vec16_Insert451740(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    VecPod_InsertAux(v, pos, x, 16, FUN_47e220);
}

extern "C" void FUN_47da70(); KING_ALIAS("_FUN_47da70", "_Stl_Copy8")    // copy of 8-byte values (first, last, dest, -)
extern "C" void FUN_46ecd0(); KING_ALIAS("_FUN_46ecd0", "_CopyBytes")    // copy of bytes (first, last, dest, -)

// [FUN_44c470] vector<double>::insert_aux
extern "C" void __fastcall Vec8_Insert44c470(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    VecPod_InsertAux(v, pos, x, 8, FUN_47da70);
}

// [FUN_44bef0] vector<char>::insert_aux
extern "C" void __fastcall VecChar_Insert44bef0(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    VecPod_InsertAux(v, pos, x, 1, FUN_46ecd0);
}

extern "C" void FUN_405730(); KING_ALIAS("_FUN_405730", "_Anchors_At")    // the road node at a point (out, point, a, b)
extern "C" void FUN_4545d0(); KING_ALIAS("_FUN_4545d0", "@Vec12_Insert4545d0@16")    // vector<12 bytes>::insert_aux

// appends a 12-byte record to a vector at v
static void Vec12_PushBack(BYTE** v, const DWORD* r)
{
    BYTE* e = v[1];
    if (e != v[2]) {
        if (e)
            memcpy(e, r, 12);
        v[1] += 12;
    } else
        CallThis(FUN_4545d0, v, e, r);
}

// [FUN_405b00] links a road object to the nodes at its two ends: {object, node A + 8, node B + 8} in both nodes' lists at +44
extern "C" void __cdecl Road_LinkObject(BYTE* obj, int a, int b)
{
    BYTE p0[24], p1[24];
    CallVirt(obj, 0, p0, 0.0, 0);
    int n = CallVirt<int>(obj, 24);
    CallVirt(obj, 0, p1, 1.0, n - 1);
    BYTE* na[2];
    CallC(FUN_405730, na, p0, a, b);
    BYTE* nb[2];
    CallC(FUN_405730, nb, p1, a, b);
    DWORD r[3] = { (DWORD)obj, (DWORD)(na[0] + 8), (DWORD)(nb[0] + 8) };
    Vec12_PushBack((BYTE**)(na[0] + 44), r);
    Vec12_PushBack((BYTE**)(nb[0] + 44), r);
}

// [FUN_410e40] frees the vector of 4-byte elements at +24 and the byte vector at +12
extern "C" void __fastcall Rec410e40_FreeVectors(BYTE* self, int)
{
    VecStorage_Free(F(BYTE*, self, 24), F(BYTE*, self, 32), 4);
    VecStorage_Free(F(BYTE*, self, 12), F(BYTE*, self, 20), 1);
}

extern "C" void FUN_461ae0(); KING_ALIAS("_FUN_461ae0", "@Rec16_DeletingDestructor@12")    // destroys a 16-byte element (flag)

// [FUN_41ce50] destroys the vector of 16-byte elements at +120
extern "C" void __fastcall Rec41ce50_DestroyVector(BYTE* self, int)
{
    BYTE** v = (BYTE**)(self + 120);
    BYTE* e = v[1];
    for (BYTE* p = v[0]; p != e; p += 16)
        CallThis(FUN_461ae0, p, 0);
    VecStorage_Free(v[0], v[2], 16);
}

// [FUN_422210] puts a player at its market's dealer: the first id of its dealer record (+116) of the market (+112)
extern "C" void __cdecl Player_PlaceAtDealer(BYTE* p)
{
    BYTE* recs = F(BYTE*, g_68cc98[F(int, p, 112)], 28);
    PositionId pos;
    DWORD* d = (DWORD*)&pos;
    d[0] = (DWORD)-1;
    d[1] = F(DWORD, recs, F(int, p, 116) * 36);
    d[2] = d[3] = d[4] = d[5] = d[6] = 0;
    d[7] = (DWORD)-1;
    Ai_TrimDealerPosition(&pos);
    Player_SetPosition(p, 0, &pos);
}

// [FUN_404ef0] the point of a town's node (its id, or its first item's)
extern "C" DVec3* __cdecl Town_Point(DVec3* out, BYTE* town)
{
    int id;
    Town_IdOrFirst(&id, town);
    if (id == -1) {
        const char* b = F(const char*, town, 0);
        AiError(0, 1, s_mainFile, s_mainId, 932, "No coordinates defined for town",
                b != F(const char*, town, 4) ? b : 0, &g_aiEmpty);
        return out;
    }
    PositionId pos;
    DWORD* d = (DWORD*)&pos;
    d[0] = (DWORD)-1;
    d[1] = id;
    d[2] = d[3] = d[4] = d[5] = d[6] = 0;
    d[7] = (DWORD)-1;
    Road_PointOf(out, pos);
    return out;
}

extern "C" void FUN_458cf0(); KING_ALIAS("_FUN_458cf0", "_Bytes_AllocCopy@12")    // allocates n bytes and copies [first, last) (stdcall)

// [FUN_44c160] vector<POD>::operator= on byte counts
extern "C" BYTE** __fastcall PodVec_Assign(BYTE** self, void*, BYTE* const* src)
{
    if (src == self)
        return self;
    BYTE* sb = src[0];
    BYTE* se = src[1];
    unsigned n = (unsigned)(se - sb);
    if (n > (unsigned)(self[2] - self[0])) {
        BYTE* p = CallThis<BYTE*>(FUN_458cf0, self, n, sb, se);
        if (self[2] != self[0])
            CallC(FUN_44b4a0, self[0], (int)(self[2] - self[0]));
        self[0] = p;
        self[2] = p + n;
    } else if ((unsigned)(self[1] - self[0]) >= n) {
        memmove(self[0], sb, n);
    } else {
        unsigned size = (unsigned)(self[1] - self[0]);
        memmove(self[0], sb, size);
        memmove(self[1], sb + size, n - size);
    }
    self[1] = self[0] + n;
    return self;
}

extern "C" void __cdecl free(void* p);

// [FUN_44b4a0] the AI's allocator: free (p, size); blocks over 128 bytes go to free(), smaller ones onto their free list
extern "C" void __cdecl Ai_PoolFree(void* p, unsigned size)
{
    if (size > 128) {
        free(p);
        return;
    }
    void** list = &g_stlFreeLists[(size + 7) >> 3];
    Stl_Lock(&g_stlLock, 0);
    *(void**)p = *list;
    *list = p;
    g_stlLock = 0;
}

KING_VAR(BYTE*, g_stlStart68d214, 68d214)
KING_VAR(BYTE*, g_stlEnd68d218, 68d218)
KING_VAR(unsigned, g_stlHeap68d210, 68d210)

// [FUN_44bcf0] malloc; on failure the out-of-memory loop (handler, retry, throw)
extern "C" void* __cdecl Stl_OomMalloc(unsigned bytes)
{
    void* p = malloc(bytes);
    return p ? p : CallC<void*>(FUN_4581a0, bytes);
}

// [FUN_45a0f0] the allocator's chunk_alloc: count nodes of size bytes from the pool (fewer if it runs
// short), refilling the pool from malloc or from the larger free lists
extern "C" BYTE* __cdecl Stl_ChunkAlloc(unsigned size, int* count)
{
    for (;;) {
        unsigned total = size * *count;
        unsigned left = (unsigned)(g_stlEnd68d218 - g_stlStart68d214);
        BYTE* r = g_stlStart68d214;
        if (left >= total) {
            g_stlStart68d214 += total;
            return r;
        }
        if (left >= size) {
            *count = left / size;
            g_stlStart68d214 += *count * size;
            return r;
        }
        unsigned get = 2 * total + (((g_stlHeap68d210 >> 4) + 7) & ~7u);
        if (left) {
            void** l = &g_stlFreeLists[(left + 7) >> 3];
            *(void**)g_stlStart68d214 = *l;
            *l = g_stlStart68d214;
        }
        g_stlStart68d214 = (BYTE*)malloc(get);
        if (!g_stlStart68d214) {
            bool found = false;
            for (unsigned i = size; i <= 128; i += 8) {
                void** l = &g_stlFreeLists[(i + 7) >> 3];
                BYTE* p = (BYTE*)*l;
                if (p) {
                    *l = *(void**)p;
                    g_stlStart68d214 = p;
                    g_stlEnd68d218 = p + i;
                    found = true;
                    break;
                }
            }
            if (found)
                continue;
            g_stlEnd68d218 = 0;
            g_stlStart68d214 = (BYTE*)Stl_OomMalloc(get);
        }
        g_stlHeap68d210 += get;
        g_stlEnd68d218 = g_stlStart68d214 + get;
    }
}

// [FUN_44bd50] the allocator's refill: up to 20 nodes of n bytes (a multiple of 8); returns one and puts
// the rest on the free list
extern "C" void* __cdecl Stl_Refill(unsigned n)
{
    int count = 20;
    BYTE* r = Stl_ChunkAlloc(n, &count);
    if (count != 1) {
        BYTE* next = r + n;
        g_stlFreeLists[(n + 7) >> 3] = next;
        for (int i = 1;; i++) {
            BYTE* cur = next;
            next += n;
            if (count - 1 == i) {
                *(void**)cur = 0;
                break;
            }
            *(void**)cur = next;
        }
    }
    return r;
}

// [FUN_44b740] the STL node allocator: allocate (bytes); over 128 bytes from malloc, else from the free
// list of its 8-byte class, refilled when empty
extern "C" void* __cdecl Stl_Allocate(unsigned bytes)
{
    if (bytes > 128)
        return Stl_OomMalloc(bytes);
    void** list = &g_stlFreeLists[(bytes + 7) >> 3];
    Stl_Lock(&g_stlLock, 0);
    void* r = *list;
    if (r)
        *list = *(void**)r;
    else
        r = Stl_Refill((bytes + 7) & ~7u);
    g_stlLock = 0;
    return r;
}

extern "C" void FUN_459e90(); KING_ALIAS("_FUN_459e90", "@RopeSubstr_Destroy@8")    // a substring rope node's destructor
extern "C" unsigned __cdecl Str_Rounded(unsigned n);              // a rope leaf's allocated size
extern "C" void __cdecl Str_FreeRounded(void* p, unsigned size);   // frees a rope leaf's characters

// the rope node frees below: tag at +8 (0 leaf, 1 concatenation, 2 substring, 3 function), refcount at +4,
// size at +0, the C string at +16; the leaf's characters, or the children / function, at +20 and +24

// frees a node's C string (allocated with size + 1)
static void Rope_FreeCString(BYTE* n)
{
    void* c = F(void*, n, 16);
    if (c && F(int, n, 0) + 1 != 0)
        Ai_PoolFree(c, F(int, n, 0) + 1);
}

extern "C" void __fastcall Rope_Unref(BYTE* n, void*);

// [FUN_44f1b0] frees a rope node and drops its children
extern "C" void __fastcall Rope_FreeTree(BYTE* n, void*)
{
    switch ((signed char)F(int, n, 8)) {
    case 0: {
        if (F(void*, n, 20) != F(void*, n, 16))
            Rope_FreeCString(n);
        BYTE* data = F(BYTE*, n, 20);
        unsigned size = Str_Rounded(F(int, n, 0));
        if (size)
            Ai_PoolFree(data, size);
        Ai_PoolFree(n, 24);
        break;
    }
    case 1: {
        Rope_FreeCString(n);
        BYTE* left = F(BYTE*, n, 20);
        if (!InterlockedDecrement((LONG*)(left + 4)))
            Rope_FreeTree(left, 0);
        Rope_Unref(F(BYTE*, n, 24), 0);
        Ai_PoolFree(n, 28);
        break;
    }
    case 3:
        Rope_FreeCString(n);
        if (F(char, n, 24) && F(void*, n, 20))
            CallVirt(F(void*, n, 20), 0, 1);
        Ai_PoolFree(n, 28);
        break;
    case 2: {
        BYTE* p = n ? n - 4 : 0;
        CallThis(FUN_459e90, p);
        Ai_PoolFree(p, 40);
        break;
    }
    }
}

// [FUN_44b5e0] drops a reference to a rope node; frees it at 0 (its leaf characters with Str_FreeRounded)
extern "C" void __fastcall Rope_Unref(BYTE* n, void*)
{
    if (InterlockedDecrement((LONG*)(n + 4)))
        return;
    switch ((signed char)F(int, n, 8)) {
    case 0:
        if (F(void*, n, 20) != F(void*, n, 16))
            Rope_FreeCString(n);
        Str_FreeRounded(F(void*, n, 20), F(int, n, 0));
        Ai_PoolFree(n, 24);
        break;
    case 1:
        Rope_FreeCString(n);
        Rope_Unref(F(BYTE*, n, 20), 0);
        Rope_Unref(F(BYTE*, n, 24), 0);
        Ai_PoolFree(n, 28);
        break;
    case 3:
        Rope_FreeCString(n);
        if (F(char, n, 24) && F(void*, n, 20))
            CallVirt(F(void*, n, 20), 0, 1);
        Ai_PoolFree(n, 28);
        break;
    case 2: {
        BYTE* p = n ? n - 4 : 0;
        CallThis(FUN_459e90, p);
        Ai_PoolFree(p, 40);
        break;
    }
    }
}

// [FUN_4484d0] constructs the player list: an empty circular list with a 1000-byte head node
extern "C" BYTE** __fastcall PlayerList_Construct(BYTE** self, void*, int)
{
    BYTE* head = (BYTE*)Stl_OomMalloc(1000);
    *self = head;
    F(BYTE*, head, 0) = head;
    F(BYTE*, *self, 4) = *self;
    return self;
}

extern "C" void FUN_4afbf0(); KING_ALIAS("_FUN_4afbf0", "@SchList_Begin@12")
extern "C" void FUN_4afc20(); KING_ALIAS("_FUN_4afc20", "@SchList_End@12")
extern "C" void FUN_45e520(); KING_ALIAS("_FUN_45e520", "_Player_RandomTradePartner2")    // find in the players (out, first, last, a, b)
extern "C" void FUN_45e710(); KING_ALIAS("_FUN_45e710", "_Player_RandomTradePartner4")
extern "C" void FUN_45e8f0(); KING_ALIAS("_FUN_45e8f0", "_Player_RandomPartner")    // the same with a pair and a third value
extern "C" void FUN_45eba0(); KING_ALIAS("_FUN_45eba0", "_Player_RandomTradePartner")
extern "C" void FUN_45ed90(); KING_ALIAS("_FUN_45ed90", "_Player_RandomTradePartner3")
extern "C" void FUN_45b8b0(); KING_ALIAS("_FUN_45b8b0", "_Nodes_RandomOther")    // find in the road nodes (first, last, a, b, c)
extern "C" void FUN_45cdc0(); KING_ALIAS("_FUN_45cdc0", "_Pick_RandomRoad")

// the player list iterator found by a search over the whole list; the end when nothing matches
template <class... A>
static int* Players_Search(void (*find)(), int* out, A... a)
{
    void* end;
    void* begin;
    CallThis(FUN_4afc20, &g_players, &end);
    CallThis(FUN_4afbf0, &g_players, &begin);
    int r;
    CallC(find, &r, begin, end, a...);
    *out = r;
    return out;
}

// [FUN_4571a0]
extern "C" int* __cdecl Players_Find4571a0(int* out, int a, int b) { return Players_Search(FUN_45e520, out, a, b); }
// [FUN_457210]
extern "C" int* __cdecl Players_Find457210(int* out, int a, int b) { return Players_Search(FUN_45e710, out, a, b); }
// [FUN_4572f0]
extern "C" int* __cdecl Players_Find4572f0(int* out, int a, int b) { return Players_Search(FUN_45eba0, out, a, b); }
// [FUN_457360]
extern "C" int* __cdecl Players_Find457360(int* out, int a, int b) { return Players_Search(FUN_45ed90, out, a, b); }
// [FUN_457280]
extern "C" int* __cdecl Players_Find457280(int* out, int a, int b, int c) { return Players_Search(FUN_45e8f0, out, a, b, c); }

// the index of the road node found by a search, -1 if none
static int* Nodes_Search(void (*find)(), int* out, int a, int b, int c)
{
    RoadNode** r = CallC<RoadNode**>(find, g_roadNodes, g_roadNodesEnd, a, b, c);
    *out = r == g_roadNodesEnd ? -1 : (int)(r - g_roadNodes);
    return out;
}

// [FUN_4558e0]
extern "C" int* __cdecl Nodes_Find4558e0(int* out, int a, int b, int c) { return Nodes_Search(FUN_45b8b0, out, a, b, c); }
// [FUN_456140]
extern "C" int* __cdecl Nodes_Find456140(int* out, int a, int b, int c) { return Nodes_Search(FUN_45cdc0, out, a, b, c); }

// [FUN_41f970] destroys the present found by the value; error 07391 if there is none
extern "C" void __cdecl Present_DestroyFound(int v)
{
    void* end;
    void* begin;
    CallThis(FUN_4afc20, &g_68cf6c, &end);
    CallThis(FUN_4afbf0, &g_68cf6c, &begin);
    BYTE* r;
    Range_CallC((BYTE*)&r, (DWORD)begin, (DWORD)end, v);
    if (r == (BYTE*)g_68cf6c) {
        AiError(0, 512, s_mainFile, s_mainId, 6980, "Internal error 07391", &g_aiEmpty, &g_aiEmpty);
        return;
    }
    Present_Destroy(r);
}

extern "C" void FUN_44f340(); KING_ALIAS("_FUN_44f340", "_Rope_DestrConcatChars")    // a rope's tree with (a, b) applied (tree, a, b)

// [FUN_44a840] replaces a rope's tree with the result of FUN_44f340 (a, b), dropping the old tree
extern "C" BYTE** __fastcall Rope_Apply44a840(BYTE** self, void*, int a, int b)
{
    BYTE* r = CallC<BYTE*>(FUN_44f340, *self, a, b);
    if (*self)
        Rope_Unref(*self, 0);
    *self = r;
    return self;
}

// [FUN_44f2c0] a rope's character i: from its C string if it has one, else down the tree
extern "C" char __cdecl Rope_CharAt(BYTE* n, unsigned i)
{
    if (F(char*, n, 16))
        return F(char*, n, 16)[i];
    for (;;) {
        switch ((signed char)F(int, n, 8)) {
        case 0:
            return F(char*, n, 20)[i];
        case 1: {
            BYTE* left = F(BYTE*, n, 20);
            if (i < F(unsigned, left, 0))
                n = left;
            else {
                i -= F(unsigned, left, 0);
                n = F(BYTE*, n, 24);
            }
            break;
        }
        case 2:
        case 3: {
            char c;
            CallVirt(F(void*, n, 20), 4, i, 1, &c);
            return c;
        }
        }
    }
}

// [FUN_425790] when the AI runs and the player isn't in state 5: a kind-1 player without goods (or whose
// goods aren't kind 1) clears the marks and its own (+524..+532); others leave when their flags say so
extern "C" void __cdecl Player_CheckGoods(BYTE* p)
{
    if (g_68d188 || F(int, p, 100) == 5)
        return;
    if (F(int, p, 92) == 1) {
        BYTE* g = (BYTE*)Player_FirstGoods(p + 8);
        if (!g || F(int, g, 104) != 1) {
            Players_ResetMarks();
            F(int, p, 524) = 0;
            F(int, p, 528) = 0;
            F(int, p, 532) = -1;
        }
    } else if (Player_UpdateFlagsGet(p) && !Player_FirstGoods(p + 8))
        Player_Leave(p, 0);
}

// [FUN_44fcd0] the string hash: 13 * first + 5 * last character + length, 0 for an empty string
extern "C" unsigned __fastcall NameHash_Hash(void* self, void*, int** key)
{
    int* s = *key;
    int len;
    if (!s || (len = *s) == 0)
        return 0;
    int a = Rope_CharAt((BYTE*)s, 0);
    int b = Rope_CharAt((BYTE*)*key, len - 1);
    return a * 13 + b * 5 + len;
}

static unsigned NameHash_Bucket(BYTE* self, int** key)
{
    return NameHash_Hash(0, 0, key) % (unsigned)((F(BYTE*, self, 8) - F(BYTE*, self, 4)) >> 2);
}

// [FUN_44b1b0] hash_set<string>::find: out = {node or 0, table}
extern "C" void** __fastcall NameHash_Find(BYTE* self, void*, void** out, int** key)
{
    void** n = (void**)F(void**, self, 4)[NameHash_Bucket(self, key)];
    while (n && CallC<int>(FUN_45a7d0, n[1], *key) != 0)
        n = (void**)*n;
    out[0] = n;
    out[1] = self;
    return out;
}

// [FUN_44f9e0] hash_set<string>::insert_unique_noresize: out = {node, table, inserted}
extern "C" void** __fastcall NameHash_InsertNoResize(BYTE* self, void*, void** out, int** key)
{
    unsigned i = NameHash_Bucket(self, key);
    void** head = (void**)F(void**, self, 4)[i];
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

extern "C" BYTE** __stdcall List64_Erase(BYTE** out, BYTE** node);   // ai_containers.cpp
extern "C" void FUN_456460(); KING_ALIAS("_FUN_456460", "_Recs_FarFromCamera")    // a search over [first, last) with a 16-byte predicate (out, first, last, pred)
extern "C" void FUN_456a10(); KING_ALIAS("_FUN_456a10", "_Goods_PickWeighted")    // a search over [first, last) with a 12-byte predicate and the RNG text (first, last, pred, text)

// [FUN_41bd30] appends v to the presents list; out = the new last element
extern "C" int* __cdecl Presents_Append(int* out, int v)
{
    BYTE** end;
    CallThis(FUN_4afc20, &g_68cf6c, &end);
    BYTE** tmp;
    ListA_Insert(&g_68cf6c, 0, (BYTE**)&tmp, (BYTE**)end, &v);
    *out = F(int, g_68cf6c, 4);
    return out;
}

// [FUN_454920] erases the list nodes [first, last); out = last
extern "C" void** __fastcall List64_EraseRange(void* self, void*, void** out, BYTE** first, BYTE** last)
{
    while (first != last) {
        BYTE** it = first;
        first = (BYTE**)*first;
        BYTE** tmp;
        List64_Erase((BYTE**)&tmp, it);
    }
    *out = last;
    return out;
}

// [FUN_418bb0] a search over the records of v (+28..+32) for {*key, -, b, c}
extern "C" void* __cdecl Recs_Search418bb0(void* out, BYTE* v, const int* key, int b, int c)
{
    int pred[4] = {*key, 0, b, c};
    CallC(FUN_456460, out, F(void*, v, 28), F(void*, v, 32), pred[0], pred[1], pred[2], pred[3]);
    return out;
}

// [FUN_41b2a0] the id at the item of p's list (+40..+44) matching {p, a, b}; -1 if none
extern "C" int* __cdecl Items_Find41b2a0(int* out, BYTE* p, int a, int b)
{
    int* r = CallC<int*>(FUN_456a10, F(void*, p, 40), F(void*, p, 44), p, a, b, &g_rng68b9c8);
    *out = r == F(int*, p, 44) ? -1 : *r;
    return out;
}

ASM_FUNC(FUN_466440, _IntVec_UninitFillN)    // uninitialized_fill_n of 12-byte vectors (first, n, value, -)

// [FUN_4024b0] the static vector at 68cbf8: 12 empty vectors
extern "C" void __cdecl AI_Init68cbf8()
{
    BYTE** g = (BYTE**)&g_68cbf8;
    g[0] = g[1] = g[2] = 0;
    BYTE* p = (BYTE*)Stl_OomMalloc(144);
    g[0] = g[1] = p;
    g[2] = p + 144;
    DWORD empty[3] = {0, 0, 0};
    g[1] = CallC<BYTE*>(FUN_466440, p, 12, empty, 0);
}


// [FUN_457820] the positions in [first, last) of the players whose +36 list has a free kind-2 slot
// or whose name is one of the BMW_M5_1..4
extern "C" BYTE** __cdecl Players_SelectBmw(BYTE** out, BYTE** first, BYTE** last)
{
    DWORD tmp[3] = {0, 0, 0};
    for (BYTE** it = first; it != last; it++) {
        BYTE* p = *it;
        BYTE* v[3];
        Vec4b_Copy((BYTE*)v, 0, (const BYTE**)(p + 36));
        if (PtrVec_FindFree((int*)v[0], (int*)v[1], (int*)v[2], 2) || SchValue_IsSymbol(p + 8, "BMW_M5_1")
            || SchValue_IsSymbol(p + 8, "BMW_M5_2") || SchValue_IsSymbol(p + 8, "BMW_M5_3")
            || SchValue_IsSymbol(p + 8, "BMW_M5_4")) {
            if (tmp[1] != tmp[2]) {
                *(BYTE***)tmp[1] = it;
                tmp[1] += 4;
            } else
                Vec4b_InsertC(tmp, 0, (DWORD*)tmp[1], (const DWORD*)&it);
        }
    }
    unsigned n = (tmp[1] - tmp[0]) >> 2;
    BYTE* p = n ? (BYTE*)Stl_Allocate(n * 4) : 0;
    out[0] = out[1] = p;
    out[2] = p + n * 4;
    out[1] = (BYTE*)UninitCopyDwords((const DWORD*)tmp[0], (const DWORD*)tmp[1], (DWORD*)p);
    if (tmp[2] != tmp[0])
        Ai_PoolFree((void*)tmp[0], tmp[2] - tmp[0]);
    return out;
}

// ---------------------------------------------------------------------------
// placing the presents

extern "C" void FUN_45dbb0(); KING_ALIAS("_FUN_45dbb0", "_Objects_RandomFar")    // the first road node matching a 40-byte spot predicate (first, last, pred, rng)
ASM_FUNC(FUN_471780, _Order_RouteCost)    // an order's route for a position (order, PositionId32)
extern "C" void FUN_450960(); KING_ALIAS("_FUN_450960", "@OrderVec_InsertAux@16")    // vector<order>::insert_aux (thiscall pos, order)
extern "C" void FUN_41b320(); KING_ALIAS("_FUN_41b320", "_Goods_SetDistance")    // a cargo present's place (cargo, PositionId32)
extern "C" void FUN_44ab90(); KING_ALIAS("_FUN_44ab90", "@Vec16_Free@8")
extern "C" BYTE* __fastcall Order_Construct(BYTE* self, void*);                 // cargo_orders.cpp
extern "C" void __cdecl Order_SetPrice(BYTE* pl, int* o, char direct, char urgent);
KING_VAR(PtrVec, g_spots68d0b8, 68d0b8)
KING_VAR(PtrVec, g_spots68d0bc, 68d0bc)
KING_VAR(int, g_hiddenMax68a98c, 68a98c)
KING_VAR(int, g_baseMax68cf54, 68cf54)
KING_VAR(int, g_roadMax68d0a0, 68d0a0)
KING_VAR(int, g_cargoMax68a964, 68a964)
KING_VAR(int, g_parkingMax68bb54, 68bb54)
typedef void (__cdecl* PresentHook)(int id, int added);
KING_VAR(PresentHook, g_presentHook68d11c, 68d11c)

// a spot's weight for a kind: 0.75 ^ its use count when the spot is of the kind
static double Spot_Weight(BYTE* spot, int* kind)
{
    return F(int, spot, 28) == *kind ? x87_pow(0.75, (double)F(int, spot, 40)) : 0.0;
}

static double Vehicle_Weight(BYTE* v, int* node)
{
    return Order_ValueAt(node, 0, (int*)v);
}

// picks an element of [first, last) at random by its positive weight; last if none has one
static void** PickWeighted(void** first, void** last, double (*weight)(BYTE*, int*), int* ctx)
{
    int n = (int)(last - first);
    void*** its = (void***)operator new(n * sizeof(void**) + 4);
    double* ws = (double*)operator new(n * sizeof(double) + 8);
    int k = 0;
    double sum = 0.0;
    for (void** it = first; it != last; it++) {
        double w = weight((BYTE*)*it, ctx);
        if (w > 0.0) {
            sum = w + sum;
            its[k] = it;
            ws[k++] = w;
        }
    }
    void** r = last;
    if (k == 1)
        r = its[0];
    else if (k > 1) {
        double target = (double)Rng_Random(&g_rng68b9c8, 0, 1000000) * sum * 1e-6;
        double acc = 0.0;
        for (int i = 0; i < k; i++) {
            if (!(target < acc) && !(acc + ws[i] < target)) {
                r = its[i];
                break;
            }
            acc += ws[i];
        }
    }
    operator delete(its);
    operator delete(ws);
    return r;
}

// the road node for a kind of spot, -1 if none
static int Present_FindNode(const char* kind, const DWORD* state, double t)
{
    DWORD pred[10];
    pred[0] = (DWORD)kind;
    pred[1] = 0;
    memcpy(pred + 2, state, 24);
    memcpy(pred + 8, &t, 8);
    RoadNode** r = CallC<RoadNode**>(FUN_45dbb0, g_roadNodes, g_roadNodesEnd, pred[0], pred[1], pred[2], pred[3],
                                     pred[4], pred[5], pred[6], pred[7], pred[8], pred[9], &g_rng68b9c8);
    if (r == g_roadNodesEnd)
        return -1;
    return (int)(r - g_roadNodes);
}

// a new 112-byte present at a road node with flags
static BYTE* Present_New(int node, DWORD flags)
{
    BYTE* p = (BYTE*)operator new(112);
    if (p) {
        F(int, p, 0) = -1;
        F(int, p, 4) = 0x12345678;
        F(int, p, 8) = 0;
        F(int, p, 12) = -1;
        F(int, p, 16) = -1;
        F(int, p, 20) = 0;
        F(int, p, 24) = -1;
        Position_InitNone((PositionId*)(p + 56), 0);
        F(int, p, 96) = -1;
        F(int, p, 88) = 0;
        F(int, p, 100) = 0x7fefffff;
        F(BYTE, p, 104) = 0;
        F(int, p, 108) = -1;
    }
    F(int, p, 24) = node;
    F(DWORD, p, 8) |= flags;
    return p;
}

// a present's position: its node's point and that point's PositionId
static void Present_SetPosition(BYTE* p, int node)
{
    memcpy(p + 32, g_68cf00[node] + 48, 24);
    BYTE tmp[32];
    memcpy(p + 56, CallC<BYTE*>(FUN_436900, tmp, p + 32), 32);
}

// a vehicle's 48-byte record of a present: the vehicle, 1.0, 19 entries of {0, 1.0}, an empty vector
static BYTE* PresentVehicle_New(int vehicle)
{
    BYTE* o = (BYTE*)operator new(48);
    if (!o)
        return 0;
    F(int, o, 0) = -1;
    F(int, o, 4) = vehicle;
    F(DWORD, o, 8) = 0;
    F(DWORD, o, 12) = 0x3ff00000;
    DWORD v[4] = {0, 0, 0, 0x3ff00000};
    char alloc;
    Vec16b_Construct((DWORD*)(o + 16), 0, 19, v, 0);
    CallThis(FUN_487730, o + 28, &alloc);
    F(int, o, 40) = 0;
    return o;
}

// gives a present a spot (weighted for the kind) and counts the spot's use; error when there is none
static void Present_TakeSpot(BYTE* p, int kind, int line)
{
    void** s = PickWeighted(g_spots68d0b8, g_spots68d0bc, Spot_Weight, &kind);
    if (s == g_spots68d0bc) {
        AiError(0, 100, s_mainFile, s_mainId, line, "Internal error 9874", &g_aiEmpty, &g_aiEmpty);
        return;
    }
    F(int, p, 108) = (int)(s - g_spots68d0b8);
    F(int, *s, 40)++;
}

// appends a present to the list and tells the hook; at t = 0 also steps the AI
static void Present_Add(BYTE* p, double t)
{
    int id;
    Presents_Append(&id, (int)p);
    g_presentHook68d11c(id, 1);
    if (x87_eq(t, 0.0))
        AI_Step3();
}

static void Order_Destroy(BYTE* o)
{
    for (BYTE* e = F(BYTE*, o, 120); e != F(BYTE*, o, 124); e += 16)
        CallC(FUN_460d10, e);
    CallThis(FUN_44ab90, o + 120);
}

// [FUN_41d340] places the missing presents of each kind (at most 1000 per kind and call): hidden ones, vehicles at a
// base, vehicles on a road with an order, hidden cargo and parked ones
extern "C" void __cdecl Ai_PlacePresents(double t)
{
    if (g_roadNodes == g_roadNodesEnd)
        return;
    DWORD state[6] = {0, 0, 0, 0, 0, 0};
    int cur;
    if (*Player_FromId(&cur) != (int)g_players) {
        int pl;
        Player_FromId(&pl);
        BYTE buf[24];
        memcpy(state, Player_CopyState(buf, (BYTE*)pl), 24);
    }

    for (int i = 0; g_68a968 < g_hiddenMax68a98c; ) {
        int node = Present_FindNode("pos_hidden", state, t);
        if (node == -1)
            break;
        BYTE* p = Present_New(node, 16);
        Present_SetPosition(p, node);
        F(DWORD, g_68cf00[node], 12) |= 16;
        g_68a968++;
        Present_TakeSpot(p, 2, 6573);
        Present_Add(p, t);
        if (++i >= 1000)
            break;
    }

    for (int i = 0; g_68d01c < g_baseMax68cf54; ) {
        int node = Present_FindNode("pos_base", state, t);
        if (node == -1)
            break;
        void** v = PickWeighted(g_vehicles, g_vehiclesEnd, Vehicle_Weight, &node);
        if (v == g_vehiclesEnd) {
            AiError(0, 1, s_mainFile, s_mainId, 6596, "Internal error 1945", &g_aiEmpty, &g_aiEmpty);
            break;
        }
        F(double, *v, 112) *= 0.75;
        BYTE* p = Present_New(node, 0);
        F(BYTE*, p, 20) = PresentVehicle_New((int)(v - g_vehicles));
        F(DWORD, p, 8) |= 32;
        Present_SetPosition(p, node);
        F(DWORD, g_68cf00[node], 12) |= 16;
        g_68d01c++;
        Present_Add(p, t);
        if (++i >= 1000)
            break;
    }

    for (int i = 0; g_68d0e4 < g_roadMax68d0a0; ) {
        int node = Present_FindNode("pos_road", state, t);
        if (node == -1)
            break;
        int none = -1;
        void** v = PickWeighted(g_vehicles, g_vehiclesEnd, Vehicle_Weight, &none);
        if (v == g_vehiclesEnd) {
            AiError(0, 1, s_mainFile, s_mainId, 6627, "Internal error 1945", &g_aiEmpty, &g_aiEmpty);
            break;
        }
        F(double, *v, 112) *= 0.75;
        BYTE* p = Present_New(node, 0);
        BYTE* o = PresentVehicle_New((int)(v - g_vehicles));
        F(BYTE*, p, 20) = o;
        F(DWORD, p, 8) |= 96;
        Present_SetPosition(p, node);
        F(DWORD, g_68cf00[node], 12) |= 16;
        g_68d0e4++;

        BYTE order[144];
        Order_Construct(order, 0);
        int town, kind, r;
        Town_Nearest(&town, (const DVec3*)(p + 32));
        if (F(BYTE, *v, 4) & 1)
            kind = *Recs104_Random(&r, 4, 4);
        else {
            kind = *Items_Find41b2a0(&r, (BYTE*)g_towns[town], 0, 4);
            if (kind == -1) {
                town = *Town_NearestOther(&r, town);
                kind = *Items_Find41b2a0(&r, (BYTE*)g_towns[town], 0, 4);
                if (kind == -1)
                    kind = *Recs104_Random(&r, 0, 4);
            }
        }
        F(int, order, 0) = kind;
        F(int, order, 12) = *Town_Find2(&r, (int)g_towns[town], kind, (int)&g_rng68b9c8);
        if (F(int, order, 12) == -1) {
            Order_Destroy(order);
            break;
        }
        DWORD pos[8];
        memcpy(order + 24, p + 56, 32);
        memcpy(pos, p + 56, 32);
        CallC(FUN_471780, order, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7]);
        F(double, order, 88) *= 1.3;
        F(int, order, 104) = 2;
        double price = 0.0, load = 0.0, out = 0.0;
        Vehicle_DealCosts(F(int, o, 4), &price, &load, &out, g_68ccb8 + F(int, order, 0) * 104);
        F(double, order, 56) = out;
        if (out < 0.0)
            AiError(0, 512, s_mainFile, s_mainId, 6688, "Q<0", &g_aiEmpty, &g_aiEmpty);
        Order_SetPrice((BYTE*)g_players, (int*)order, 1, 1);
        BYTE** orders = (BYTE**)(o + 28);
        if (orders[1] != orders[2]) {
            Rec458a80_Construct(orders[1], order);
            orders[1] += 144;
        } else
            CallThis(FUN_450960, orders, orders[1], order);
        Present_Add(p, t);
        Order_Destroy(order);
        if (++i >= 1000)
            break;
    }

    for (int i = 0; g_68cfe4 < g_cargoMax68a964; ) {
        int node = Present_FindNode("pos_hidden", state, t);
        if (node == -1)
            break;
        BYTE* p = Present_New(node, 64);
        F(DWORD, g_68cf00[node], 12) |= 16;
        Present_SetPosition(p, node);
        g_68cfe4++;
        int kind, r;
        Recs104_Random(&kind, 0, 4);
        BYTE* c = (BYTE*)operator new(32);
        if (c) {
            int town = *Town_Find1(&r, kind);
            F(int, c, 0) = kind;
            F(int, c, 4) = -1;
            F(int, c, 8) = -1;
            F(int, c, 12) = town;
            F(int, c, 16) = 0;
            F(int, c, 24) = 0;
            F(int, c, 20) = 0;
            F(int, c, 28) = 0;
        }
        F(BYTE*, p, 88) = c;
        PositionId pos;
        DWORD* d = (DWORD*)&pos;
        d[0] = (DWORD)-1;
        d[1] = node;
        d[2] = d[3] = d[4] = d[5] = d[6] = 0;
        d[7] = (DWORD)-1;
        DVec3 pt;
        Road_PointOf(&pt, pos);
        BYTE tmp[32];
        DWORD id[8];
        memcpy(id, CallC<BYTE*>(FUN_436900, tmp, &pt), 32);
        CallC(FUN_41b320, c, id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
        Present_TakeSpot(p, 3, 6734);
        Present_Add(p, t);
        if (++i >= 1000)
            break;
    }

    for (int i = 0; g_68cf34 < g_parkingMax68bb54; ) {
        int node = Present_FindNode("pos_parking", state, t);
        if (node == -1)
            break;
        BYTE* p = Present_New(node, 128);
        Present_SetPosition(p, node);
        F(DWORD, g_68cf00[node], 12) |= 16;
        g_68cf34++;
        Present_TakeSpot(p, 1, 6766);
        F(double, p, 96) = CallC<double>(FUN_401000);
        Present_Add(p, t);
        if (++i >= 1000)
            break;
    }
}

// ---------------------------------------------------------------------------
// loadOrder

extern "C" void FUN_4774e0(); KING_ALIAS("_FUN_4774e0", "_Ai_AddArrivals")    // a competitor's arrival (player, double delay, count)
extern "C" void FUN_429850(); KING_ALIAS("_FUN_429850", "_Ai_MaybeArrival")    // a computer player's order update (player)
extern "C" void FUN_4262a0(); KING_ALIAS("_FUN_4262a0", "_Ai_AddCompetitor")    // adds a competitor
extern "C" void FUN_457a40(); KING_ALIAS("_FUN_457a40", "_Orders_FindEqual")    // finds an order equal to one passed by value (first, last, order144)
extern "C" void FUN_4715f0(); KING_ALIAS("_FUN_4715f0", "_Order_Init4715f0")    // (order, template, town, PositionId32, double q)
extern "C" double __cdecl Order_Margin(int* o, double done, char bonus, double* price, double* value);
extern "C" BYTE* __cdecl Order144_CopyA(const BYTE* f, const BYTE* l, BYTE* d);
extern "C" void __cdecl Destroy16s(BYTE* first, BYTE* last);
extern "C" BYTE* __fastcall Rec41ce50_DeletingDestructor(BYTE* s, void*, int flags);
extern "C" BYTE* __fastcall Order144_CopyConstruct(BYTE* self, void*, const BYTE* s);
extern "C" void* __fastcall Player_ActiveCargo(void* s, void*);
KING_VAR(double, g_lastHumanLoad68d100, 68d100)
struct Blob144 { BYTE b[144]; };

static const char s_loadOrder[] =
    "loadOrder(PlayerId player, VrIndex townVrIndex, int orderid, double & quantity, int & playerorderid)";

static __declspec(noinline) int Ftol(double x) { return (int)x; }

static void LoadOrder_LeaveBuffered()
{
    Buf_Append(&g_trace, 0, g_tagMpl, 8);
    Buf_AppendString(&g_trace, 0, s_loadOrder);
}

// a message with a number formatted by the text id
static void LoadOrder_Message(const char* id, double v)
{
    char text[128];
    sprintf(text, Message_Text(id), v);
    CallC(FUN_56ae60, text);
}

// [FUN_443d50] loadOrder: loads (quantity > 0) or delivers (quantity < 0) an order of a town at a player;
// 0, or -1 (bad call), -3/-4 (over the limits), -8 (no space), -10/-11 (wrong trailer for a human)
extern "C" int __cdecl Ai_LoadOrder(BYTE* player, int townId, unsigned orderid, double* quantity, int* playerorderid)
{
    Trace_EnterBuffered(s_loadOrder);
    if (Player_NotValid(player)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 17678, s_invalidId, s_playerIdType, g_numberText);
    }
    if (Player_IsHuman(player))
        g_lastHumanLoad68d100 = CallC<double>(FUN_401000);
    *playerorderid = -1;
    int town;
    Town_ById(&town, townId);
    BYTE* t = (BYTE*)g_towns[town];
    if ((unsigned)((F(BYTE*, t, 32) - F(BYTE*, t, 28)) >> 4) <= orderid) {
        LoadOrder_LeaveBuffered();
        return -1;
    }
    BYTE* entry = F(BYTE*, t, 28) + orderid * 16;
    BYTE* tmpl = 0;
    BYTE* data = player + 8;
    if (data) {
        BYTE* d = F(BYTE*, data, 508);
        if (d) {
            int kind = F(int, g_vehicles[F(int, d, 4)], 32);
            if (kind == 2 || kind == 3)
                tmpl = F(BYTE*, entry, 8);
        }
        if (!Player_HasTrader(data))
            tmpl = F(BYTE*, entry, 12);
    }

    if (F(BYTE, entry, 4) & 1) {
        // delivering
        if (!(*quantity < 0.0)) {
            LoadOrder_LeaveBuffered();
            return -1;
        }
        bool delivered = false, bonus = false;
        double sum = 0.0, price = 0.0, value = 0.0;
        struct {
            DWORD flags;
            int count;
            int level;
            DWORD pad;
            double delay;
            double zero;
            double sum;
            double value96;
            double price;
            double value;
        } info = {0, 0, 0, 0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0};
        BYTE* orders = 0;
        for (;;) {
            orders = Player_Orders(player, orders);
            BYTE* end = F(BYTE*, orders, 4);
            BYTE* o = F(BYTE*, orders, 0);
            while (o != end && F(int, o, 12) != town)
                o += 144;
            orders = Player_Orders(player, orders);
            if (o == F(BYTE*, orders, 4))
                break;
            if (Player_IsHuman(player))
                LoadOrder_Message("X_INTEGRITY", (double)Ftol(F(double, o, 96) * 100.0));
            if (!bonus)
                bonus = F(int, o, 104) == 1;
            info.value96 = F(double, o, 96);
            char b = 0;
            if (Player_UpdateFlagsGet(player) || Player_IsHuman(player))
                b = F(int, o, 104) == 1;
            sum = Order_Margin((int*)o, (double)(int)g_68d164, b, &price, &value) + sum;
            delivered = true;
            orders = Player_Orders(player, orders);
            if (o + 144 != F(BYTE*, orders, 4))
                Order144_Copy(o + 144, F(BYTE*, orders, 4), o);
            F(BYTE*, orders, 4) -= 144;
            Rec41ce50_DeletingDestructor(F(BYTE*, orders, 4), 0, 0);
        }
        BYTE* other = F(BYTE*, player, 176);
        if (other == (BYTE*)g_players)
            F(double, player, 136) = sum + F(double, player, 136);
        else {
            F(double, player, 544) = sum + F(double, player, 544);
            F(double, other, 136) = sum + F(double, other, 136);
        }
        F(double, player, 760) = sum + F(double, player, 760);
        info.delay = (double)(Rng_Random(&g_rng68b9c8, 0, 360) + 360);
        info.sum = sum;
        info.value = value;
        info.price = price;
        if (Player_UpdateFlagsGet(player)) {
            if (g_68d164 == 0 && delivered && g_68a980 != g_68a984)
                CallC(FUN_4774e0, player, info.delay, 1);
            g_68d164++;
            CallC(FUN_483cb0, Message_Text("COMPETITOR_ARRIVED"));
        } else if (F(int, player, 92) == 1) {
            if (g_68d164 == 0 && delivered && bonus && g_68a980 != g_68a984) {
                int n = ++F(int, player, 536);
                info.count = n;
                info.flags |= n <= 1 ? 1 : 3;
                int level = Ftol(x87_sqrt((double)(n + 1)));
                if (level > 3)
                    level = 3;
                info.level = level;
                CallC(FUN_4774e0, player, info.delay, level);
            } else
                F(int, player, 536) = 0;
        } else
            CallC(FUN_429850, player);
        if (Player_IsHuman(player) && !x87_eq(sum, 0.0))
            LoadOrder_Message("YOU_MADE", (double)Ftol(sum * 100.0) * 0.01);
        if (delivered)
            CallC(FUN_4015a0, player, 11, &info);
        *playerorderid = -1;
    } else {
        // loading
        if (!(*quantity > 0.0)) {
            LoadOrder_LeaveBuffered();
            return -1;
        }
        if (!tmpl) {
            *playerorderid = -1;
            LoadOrder_LeaveBuffered();
            return -1;
        }
        if (Player_IsHuman(player)) {
            bool oil = (F(BYTE, g_68ccb8 + F(int, tmpl, 0) * 104, 48) & 4) != 0;
            if (oil && !(Player_VehicleData(player) & 1)) {
                CallC(FUN_483cb0, Message_Text("NEED_A_PETROL_SEMITRAILER"));
                LoadOrder_LeaveBuffered();
                return -10;
            }
            oil = (F(BYTE, g_68ccb8 + F(int, tmpl, 0) * 104, 48) & 4) != 0;
            if (!oil && (Player_VehicleData(player) & 1)) {
                CallC(FUN_483cb0, Message_Text("LOAD_AT_AN_OIL_TANK"));
                LoadOrder_LeaveBuffered();
                return -11;
            }
        }
        double q = 0.0, weight = 0.0, volume = 0.0;
        BYTE* orders = Player_Orders(player, 0);
        for (BYTE* o = F(BYTE*, orders, 0);; o += 144) {
            orders = Player_Orders(player, orders);
            if (o == F(BYTE*, orders, 4))
                break;
            BYTE* rec = g_68ccb8 + F(int, o, 0) * 104;
            weight = F(double, rec, 16) * F(double, o, 56) + weight;
            volume = F(double, rec, 24) * F(double, o, 56) + volume;
        }
        double maxWeight, maxVolume;
        Player_AllCosts(player, &maxWeight, &maxVolume);
        double addWeight, addVolume;
        Costs_Balance(player, &addWeight, &addVolume, &q, g_68ccb8 + F(int, tmpl, 0) * 104);
        if (addWeight + weight > maxWeight || addVolume + volume > maxVolume) {
            if (Player_IsHuman(player))
                CallC(FUN_56ae60, Message_Text("NOT_ENOUGH_SPACE"));
            LoadOrder_LeaveBuffered();
            return -8;
        }
        double w = weight - maxWeight;
        volume = volume - maxVolume;
        if (!(w < 0.0)) {
            LoadOrder_LeaveBuffered();
            return -3;
        }
        if (!(volume < 0.0)) {
            Trace_Tag((const DWORD*)g_tagMpl);
            Buf_AppendString(&g_trace, 0, s_loadOrder);
            return -4;
        }
        *quantity = q;
        BYTE order[144];
        Order_Construct(order, 0);
        DWORD pos[8] = {(DWORD)-1, F(DWORD, entry, 0), 0, 0, 0, 0, 0, (DWORD)-1};
        DWORD qd[2];
        memcpy(qd, quantity, 8);
        CallC(FUN_4715f0, order, tmpl, town, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7], qd[0], qd[1]);
        BYTE copy[144];
        Order144_CopyConstruct(copy, 0, order);
        BYTE* data2 = Iter_Data(&player, 0);
        void* cargo = Player_ActiveCargo(data2, 0);
        BYTE* cend = CallThis<BYTE*>(FUN_455950, cargo);
        data2 = Iter_Data(&player, 0);
        cargo = Player_ActiveCargo(data2, 0);
        BYTE* cbegin = CallThis<BYTE*>(FUN_455940, cargo);
        BYTE* o = CallC<BYTE*>(FUN_457a40, cbegin, cend, *(Blob144*)copy);
        orders = Player_Orders(player, orders);
        if (o == F(BYTE*, orders, 4)) {
            orders = Player_Orders(player, orders);
            BYTE** v = (BYTE**)orders;
            if (v[1] != v[2]) {
                Rec458a80_Construct(v[1], order);
                v[1] += 144;
            } else
                CallThis(FUN_450960, v, v[1], order);
            orders = Player_Orders(player, orders);
            o = F(BYTE*, orders, 4) - 144;
        } else
            F(double, o, 56) = F(double, o, 56) + *quantity;
        orders = Player_Orders(player, orders);
        *playerorderid = (int)(o - F(BYTE*, orders, 0)) / 144;
        if (x87_eq(F(double, o, 56), 0.0)) {
            *playerorderid = -1;
            orders = Player_Orders(player, orders);
            if (o + 144 != F(BYTE*, orders, 4))
                Order144_CopyA(o + 144, F(BYTE*, orders, 4), o);
            F(BYTE*, orders, 4) -= 144;
            BYTE* last = F(BYTE*, orders, 4);
            Destroy16s(F(BYTE*, last, 120), F(BYTE*, last, 124));
            CallThis(FUN_44ab90, last + 120);
        } else {
            BYTE* c = F(BYTE*, player, 520);
            Order_SetPrice(player, (int*)o, 0, c != 0);
            if (F(BYTE*, player, 520)) {
                F(double, o, 88) = 1.3 * F(double, o, 88);
                F(double, o, 72) = (1.3 - 1.0) * F(double, tmpl, 24) + F(double, o, 72);
            }
        }
        Order_Destroy(order);
    }

    if (Player_IsHuman(player)) {
        unsigned n = g_68a984 - g_68a980;
        while (n + g_68d164 < g_68d160) {
            unsigned before = n;
            CallC(FUN_4262a0);
            n = g_68a984 - g_68a980;
            if (before == n)
                break;
        }
    }
    CallC(FUN_442c60, player);
    Trace_Leave(s_loadOrder);
    return 0;
}

// ---------------------------------------------------------------------------
// loading the script lists ms.sch, vd.sch and names.sch

extern "C" void FUN_4b4c20(); KING_ALIAS("_FUN_4b4c20", "_Port_ReadDatum")    // reads a value from a port (out, reader, port by value)
extern "C" void FUN_44c710(); KING_ALIAS("_FUN_44c710", "@StrRefVec_InsertAux@16")    // vector<StrRef>::insert_aux (thiscall pos, x)
extern "C" void FUN_4b0ea0(); KING_ALIAS("_FUN_4b0ea0", "@Value_ToInt@8")    // a number's int value (thiscall)
extern "C" void FUN_4aa430(); KING_ALIAS("_FUN_4aa430", "@SchValue_Object@8")
extern "C" void FUN_4b0e90(); KING_ALIAS("_FUN_4b0e90", "@SchNumber_Value@8")
extern "C" BYTE* __fastcall MultValue_DeletingDestructor(BYTE* self, void*, int flags);   // ai_traffic.cpp
extern "C" void __cdecl ConstructStrRef(DWORD* d, const DWORD* s);                        // ai_containers.cpp
KING_VAR(AsmLabel, g_reader68bb58, 68bb58)
KING_VAR(BYTE*, g_lists68b9b8, 68b9b8)

// the list after the head: *l = its +32 member
static void List_Advance(StrRef* l)
{
    StrRef next;
    StrRef* nx = Str_Member32(&next, l);
    if (l->ptr && l->Owns()) {
        if (--F(int, l->ptr, 4) == 0 && !Str_Same(l, nx) && l->ptr)
            CallVirt(l->ptr, 12, 1);
    }
    l->CopyFrom(nx);
    Str_Release(&next);
}

// advances a pair handle to its +32 member (checked)
static void Pair_Advance(StrRef* p)
{
    StrRef n;
    Str_AssignPair(p, 0, Str_Member32Checked(&n, p));
    CallThis(FUN_4a36c0, &n);
}

// opens a script file as a port and reads its value
static void Script_Read(StrRef* value, StrRef* port, const char* name, bool f, int line)
{
    DWORD tmp[3];
    int one = 1;
    DWORD* h = f ? Port_MakeF(tmp, 0, (int)name, &one) : Port_MakeE(tmp, 0, (int)name, &one);
    port->ptr = 0;
    port->len = 0;
    port->flags = 0;
    BYTE* o = (BYTE*)h[0];
    if (o) {
        if (CallVirt<int>(o, 0) == 11) {
            port->ptr = o;
            port->flags = h[2];
            F(int, o, 4)++;
        } else
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(o, 4), "Port");
    }
    CallThis(FUN_4a36c0, tmp);
    CheckHandle(port);
    if (CallVirt<int>(port->ptr, 40))
        AiError(23, 100, s_mainFile, s_mainId, line, "Cannot open", name, &g_aiEmpty);
    StrRef copy;
    Str_CopyConstruct(&copy, 0, port);
    CallC(FUN_4b4c20, value, g_reader68bb58, copy.ptr, copy.len, copy.flags);
}

// reads a script file's list of strings into a vector of string handles
static void Script_ReadStrings(BYTE** v, const char* name, int line)
{
    for (BYTE* e = v[0]; e != v[1]; e += 12)
        MultValue_DeletingDestructor(e, 0, 0);
    v[1] = v[0];
    StrRef port, value;
    Script_Read(&value, &port, name, false, line);
    StrRef l;
    l.CopyFrom(&value);
    while ((BYTE)l.flags == 2) {
        StrRef m;
        Str_Member20(&m, &l);
        if (v[1] != v[2]) {
            ConstructStrRef((DWORD*)v[1], (const DWORD*)&m);
            v[1] += 12;
        } else
            CallThis(FUN_44c710, v, v[1], &m);
        Str_Release(&m);
        List_Advance(&l);
    }
    Str_Release(&l);
    Str_Release(&value);
    CallThis(FUN_4a36c0, &port);
}

// the symbol of the pair's head is the name
static bool Pair_HeadIs(StrRef* p, const char* name)
{
    StrRef s;
    Str_Member20Checked(&s, p);
    bool r = SchValue_IsSymbol((BYTE*)&s, name);
    CallThis(FUN_4a36c0, &s);
    return r;
}

// the number handle of the pair's head
static void Pair_HeadNumber(StrRef* out, StrRef* p)
{
    StrRef s;
    Str_Member20Checked(&s, p);
    NumberHandle_From((DWORD*)out, 0, (const DWORD*)&s);
    CallThis(FUN_4a36c0, &s);
}

static bool Ref_Set(const StrRef* r) { return r->flags || r->ptr; }

// [FUN_40d1b0] loads ms.sch and vd.sch (lists of strings) and names.sch: per entry (name sex role [n [m
// (a b)]]) a 56-byte record
extern "C" void __cdecl Ai_LoadNames()
{
    Script_ReadStrings((BYTE**)&g_aiMessages, "ms.sch", 2750);
    Script_ReadStrings((BYTE**)&g_lists68b9b8, "vd.sch", 2765);

    BYTE** v = (BYTE**)&g_images68a958;
    for (BYTE* e = v[0]; e != v[1]; e += 56)
        Rec16_DeletingDestructor(e, 0, 0);
    v[1] = v[0];
    const char* name = "names.sch";
    StrRef port, value;
    Script_Read(&value, &port, name, true, 2780);
    StrRef l = {0, 0, 0};
    Str_Assign(&l, 0, &value);
    while ((BYTE)l.flags == 2) {
        StrRef m;
        Str_Member20(&m, &l);
        bool pair = (BYTE)m.flags == 2;
        CallThis(FUN_4a36c0, &m);
        if (pair) {
            StrRef t, p;
            Str_Member20(&t, &l);
            PairHandle_From((DWORD*)&p, 0, (const DWORD*)&t);
            CallThis(FUN_4a36c0, &t);
            BYTE rec[56];
            Rec_Init40e240(rec, 0);
            StrRef s;
            Str_Member20Checked(&s, &p);
            PodVec_Assign((BYTE**)(rec + 4), 0, (BYTE* const*)(s.ptr + 12));
            CallThis(FUN_4a36c0, &s);
            Pair_Advance(&p);
            if (Pair_HeadIs(&p, "male"))
                F(int, rec, 0) = 1;
            else if (Pair_HeadIs(&p, "female"))
                F(int, rec, 0) = 2;
            else
                AiError(0, 1, s_mainFile, s_mainId, 2802, "Unknown sex", &g_aiEmpty, &g_aiEmpty);
            Pair_Advance(&p);
            if (Pair_HeadIs(&p, "trucker"))
                F(int, rec, 24) = 1;
            else if (Pair_HeadIs(&p, "police"))
                F(int, rec, 24) = 3;
            else if (Pair_HeadIs(&p, "mafia"))
                F(int, rec, 24) = 2;
            else
                AiError(0, 1, s_mainFile, s_mainId, 2815, "Unknown role", &g_aiEmpty, &g_aiEmpty);
            Pair_Advance(&p);
            StrRef n1;
            Pair_HeadNumber(&n1, &p);
            if (Ref_Set(&n1))
                F(int, rec, 20) = CallThis<int>(FUN_4b0ea0, CallThis<BYTE*>(FUN_4aa430, &n1));
            Pair_Advance(&p);
            if (Ref_Set(&p)) {
                StrRef n2;
                Pair_HeadNumber(&n2, &p);
                if (Ref_Set(&n2))
                    F(int, rec, 16) = CallThis<int>(FUN_4b0ea0, CallThis<BYTE*>(FUN_4aa430, &n2));
                CallThis(FUN_4a36c0, &n2);
                if (Ref_Set(&p)) {
                    Pair_Advance(&p);
                    if (Ref_Set(&p)) {
                        StrRef q, a, b, c, bn;
                        Str_Member20Checked(&s, &p);
                        PairHandle_From((DWORD*)&q, 0, (const DWORD*)&s);
                        CallThis(FUN_4a36c0, &s);
                        Pair_HeadNumber(&a, &q);
                        F(double, rec, 40) = CallThis<double>(FUN_4b0e90, CallThis<BYTE*>(FUN_4aa430, &a));
                        Str_Member32Checked(&b, &q);
                        Str_Member20(&c, &b);
                        NumberHandle_From((DWORD*)&bn, 0, (const DWORD*)&c);
                        CallThis(FUN_4a36c0, &c);
                        CallThis(FUN_4a36c0, &b);
                        F(double, rec, 48) = CallThis<double>(FUN_4b0e90, CallThis<BYTE*>(FUN_4aa430, &bn));
                        CallThis(FUN_4a36c0, &bn);
                        CallThis(FUN_4a36c0, &a);
                        CallThis(FUN_4a36c0, &q);
                    }
                }
            }
            Vec56_PushBack(v, 0, rec);
            CallThis(FUN_4a36c0, &n1);
            CallThis(FUN_404420, rec + 4);
            CallThis(FUN_4a36c0, &p);
        }
        StrRef n;
        Str_Assign(&l, 0, Str_Member32(&n, &l));
        Str_Release(&n);
    }
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_4a36c0, &value);
    CallThis(FUN_4a36c0, &port);
}

// ---------------------------------------------------------------------------
// reading an ambush

extern "C" void** __fastcall Handle9_Init(void** self, int, void** v);
extern "C" DVec3* __cdecl Ai__GetTangent(DVec3* out, PositionId pos);
extern "C" double __cdecl DVec3_Dot(const DVec3* a, const DVec3* b);
extern "C" int* __cdecl Module_ByName(int* out, const char* name);
extern "C" int* __cdecl Node_ByName(int* out, const char* name);

// *d = *s, dropping d's old string (unless it is the same)
static void Ref_Assign(StrRef* d, const StrRef* s)
{
    if (d->ptr && d->Owns()) {
        if (--F(int, d->ptr, 4) == 0 && !Str_Same(d, s) && d->ptr)
            CallVirt(d->ptr, 12, 1);
    }
    d->CopyFrom(s);
}

// the type of the list head
static BYTE Head_Type(StrRef* l)
{
    StrRef m;
    Str_Member20(&m, l);
    BYTE t = (BYTE)m.flags;
    CallThis(FUN_4a36c0, &m);
    return t;
}

static bool Head_Is(StrRef* l, const char* name)
{
    StrRef m;
    Str_Member20(&m, l);
    bool r = SchValue_IsSymbol((BYTE*)&m, name);
    Str_Release(&m);
    return r;
}

// the C string of the head's object
static const char* Head_Text(StrRef* l)
{
    StrRef m;
    Str_Member20(&m, l);
    const char* s = F(const char*, m.ptr, 12);
    CallThis(FUN_4a36c0, &m);
    return s;
}

static void List_Next(StrRef* l)
{
    StrRef n;
    Str_Assign(l, 0, Str_Member32(&n, l));
    CallThis(FUN_4a36c0, &n);
}

// [FUN_412320] reads an ambush: (name type module [bytes] node direction (numbers) (items) [value])
extern "C" BYTE* __cdecl Ai_ReadAmbush(void* reader, StrRef h)
{
    BYTE* o = (BYTE*)operator new(248);
    if (o) {
        char alloc;
        memset(o, 0, 12);
        Rec_Init411be0((int*)(o + 12), 0);
        F(int, o, 32) = 0;
        F(int, o, 36) = 0;
        memset(o + 40, 0, 12);
        F(int, o, 52) = -1;
        CallThis(FUN_487730, o + 112, &alloc);
        Rec_InitPlayerIter((void**)(o + 124), 0);
        Position_InitNone((PositionId*)(o + 144), 0);
        Position_InitNone((PositionId*)(o + 176), 0);
        CallThis(FUN_487730, o + 208, &alloc);
        CallThis(FUN_487730, o + 220, &alloc);
        F(DWORD, o, 232) = 790565505;
        F(DWORD, o, 236) = 1170221031;
        F(BYTE, o, 240) = 0;
    }
    List_Advance(&h);
    StrRef m;
    Str_Member20(&m, &h);
    Ref_Assign((StrRef*)(o + 40), &m);
    Str_Release(&m);
    Str_Member20(&m, &h);
    PodVec_Assign((BYTE**)o, 0, (BYTE* const*)(m.ptr + 12));
    Str_Release(&m);
    List_Advance(&h);
    if (Head_Is(&h, "MAFIA"))
        F(int, o, 36) = 1;
    else if (Head_Is(&h, "POLICE"))
        F(int, o, 36) = 2;
    else {
        Str_Member20(&m, &h);
        AiError(0, 100, s_mainFile, s_mainId, 3316, "Unknown ambush type", F(const char*, m.ptr, 12), &g_aiEmpty);
        Str_Release(&m);
    }
    List_Advance(&h);
    if (Head_Type(&h) == 3) {
        Str_Member20(&m, &h);
        int r;
        F(int, o, 12) = *Module_ByName(&r, F(const char*, m.ptr, 12));
        Str_Release(&m);
    }
    List_Advance(&h);
    if (Head_Type(&h) == 3) {
        Str_Member20(&m, &h);
        PodVec_Assign((BYTE**)(o + 16), 0, (BYTE* const*)(m.ptr + 12));
        CallThis(FUN_4a36c0, &m);
    }
    List_Next(&h);
    const char* node = 0;
    if (Head_Type(&h) == 3) {
        node = Head_Text(&h);
        if (strlen(node)) {
            int r;
            F(int, o, 52) = *Node_ByName(&r, node);
        } else
            F(int, o, 52) = -1;
    } else
        AiError(0, 256, s_mainFile, s_mainId, 3338, "Node not specified for ambush ", F(const char*, o, 0), &g_aiEmpty);
    if (F(int, o, 52) != -1) {
        DVec3_Assign((DVec3*)(o + 56), 0, (const DVec3*)(g_68cf00[F(int, o, 52)] + 48));
        BYTE tmp[32];
        memcpy(o + 144, CallC<BYTE*>(FUN_436900, tmp, o + 56), 32);
    } else
        AiError(0, 1, s_mainFile, s_mainId, 3347, "Fatal error: ambush node not present ", node ? node : "", &g_aiEmpty);
    List_Next(&h);
    StrRef dir, t;
    Str_Member20(&t, &h);
    Handle9_Init((void**)&dir, 0, (void**)&t);
    CallThis(FUN_4a36c0, &t);
    DWORD v[6] = {0, 0, 0, 0, 0, 0};
    if (SchValue_IsSymbol((BYTE*)&dir, "NORTH")) {
        F(int, o, 80) = 1;
        v[3] = 0x3ff00000;
    } else if (SchValue_IsSymbol((BYTE*)&dir, "SOUTH")) {
        F(int, o, 80) = 2;
        v[3] = 0xbff00000;
    } else if (SchValue_IsSymbol((BYTE*)&dir, "EAST")) {
        F(int, o, 80) = 3;
        v[1] = 0x3ff00000;
    } else if (SchValue_IsSymbol((BYTE*)&dir, "WEST")) {
        F(int, o, 80) = 4;
        v[1] = 0xbff00000;
    } else
        AiError(0, 100, s_mainFile, s_mainId, 3376, "Unknown ambush direction",
                F(const char*, CallThis<BYTE*>(FUN_4aa430, &dir), 12), &g_aiEmpty);
    if (F(int, o, 80) >= 1 && F(int, o, 80) <= 4)
        DVec3_Assign((DVec3*)(o + 88), 0, (const DVec3*)v);
    memcpy(o + 176, o + 144, 32);
    PositionId pos;
    memcpy(&pos, o + 144, 32);
    DVec3 tan;
    Ai__GetTangent(&tan, pos);
    if (DVec3_Dot(&tan, (const DVec3*)(o + 88)) < 0.0)
        F(DWORD, o, 200) |= 1;
    List_Next(&h);
    StrRef nums;
    Str_Member20(&nums, &h);
    if ((BYTE)nums.flags == 2) {
        StrRef l = {0, 0, 0};
        Str_Assign(&l, 0, &nums);
        while ((BYTE)l.flags == 2) {
            StrRef e;
            Str_Member20(&e, &l);
            if ((BYTE)e.flags == 6) {
                StrRef n;
                NumberHandle_From((DWORD*)&n, 0, (const DWORD*)&e);
                int x = CallThis<int>(FUN_4b0ea0, CallThis<BYTE*>(FUN_4aa430, &n));
                Vec4_PushBackD(o + 112, 0, &x);
                CallThis(FUN_4a36c0, &n);
            }
            CallThis(FUN_4a36c0, &e);
            List_Next(&l);
        }
        CallThis(FUN_4a36c0, &l);
    }
    List_Next(&h);
    StrRef items;
    Str_Member20(&items, &h);
    if ((BYTE)items.flags == 2) {
        StrRef l = {0, 0, 0};
        Str_Assign(&l, 0, &items);
        while ((BYTE)l.flags == 2) {
            StrRef e;
            Str_Member20(&e, &l);
            if ((BYTE)e.flags == 9) {
                StrRef n;
                Handle9_Init((void**)&n, 0, (void**)&e);
                Vec12_PushBackCopy(o + 208, 0, CallThis<BYTE*>(FUN_4aa430, &n) + 12);
                CallThis(FUN_4a36c0, &n);
            }
            CallThis(FUN_4a36c0, &e);
            List_Next(&l);
        }
        CallThis(FUN_4a36c0, &l);
    }
    CallThis(FUN_4a36c0, &items);
    List_Next(&h);
    if ((BYTE)h.flags == 2 && Head_Type(&h) == 6) {
        StrRef n;
        Str_Member20(&m, &h);
        NumberHandle_From((DWORD*)&n, 0, (const DWORD*)&m);
        CallThis(FUN_4a36c0, &m);
        if (n.flags || n.ptr)
            F(double, o, 232) = CallThis<double>(FUN_4b0e90, CallThis<BYTE*>(FUN_4aa430, &n));
        CallThis(FUN_4a36c0, &n);
    }
    CallThis(FUN_4a36c0, &nums);
    CallThis(FUN_4a36c0, &dir);
    CallThis(FUN_4a36c0, &h);
    return o;
}

// ---------------------------------------------------------------------------
// xLoadEntities

extern "C" void FUN_43bf10(); KING_ALIAS("_FUN_43bf10", "_Ai_AdvancePlayerPosition")    // a player's next position (out, player, PositionId32, double*, int)
extern "C" void FUN_4376a0(); KING_ALIAS("_FUN_4376a0", "_Ai_GetNoncontradictoryPosition")    // a player's position (out, player)
extern "C" void FUN_456ea0(); KING_ALIAS("_FUN_456ea0", "@Entry32Vec_InsertAux@16")    // vector<32-byte entry>::insert_aux (thiscall pos, x)
extern "C" void FUN_45e2a0(); KING_ALIAS("_FUN_45e2a0", "_IntroSort_ByKey")    // introsort loop of entries (first, last, 0, depth, cmp)
extern "C" void FUN_45e370(); KING_ALIAS("_FUN_45e370", "_Entries32_FinalSort")    // final insertion sort of entries (first, last, cmp)
extern "C" void FUN_4d8b20(); KING_ALIAS("_FUN_4d8b20", "_Vehicle_LoadResource")    // loads a vehicle (id)
extern "C" int __cdecl Vehicle_Exists(int id);
extern "C" int __cdecl Vehicle_Flag4012(int id);
extern "C" int __cdecl Vehicle_TimeOver(int id);
extern "C" void __cdecl Vehicle_Release(int id);
extern "C" int __cdecl Way_Point(int i, DWORD* pos, int* id);
extern "C" int* __cdecl Player_CarId(int* out);
extern "C" void __cdecl Construct32Skip1(DWORD* d, const DWORD* s);
extern "C" void FUN_44b0b0(); KING_ALIAS("_FUN_44b0b0", "@Vec32_Free@8")
KING_VAR(int, g_loadTick68d13c, 68d13c)
KING_VAR(int, g_loadCount68d1b4, 68d1b4)
KING_VAR(double, g_loadRadius68d128, 68d128)
KING_VAR(BYTE, g_loadDone68d130, 68d130)
KING_VAR(BYTE, g_loadNear68d138, 68d138)
KING_VAR(int, g_wayPoints67b2b8, 67b2b8)

static const char s_xLoadEntities[] = "xLoadEntities()";

// an entry of the load list: a vehicle (or -1 and a pointer to it), its weight and distance, a player
struct LoadEntry {
    int vehicle;
    int pad;
    double weight;
    double dist;
    int* ref;
    BYTE* player;
};

static int Entry_Vehicle(const LoadEntry* e) { return e->vehicle != -1 ? e->vehicle : *e->ref; }

static void Entries_Push(BYTE** v, const LoadEntry* e)
{
    if (v[1] != v[2]) {
        Construct32Skip1((DWORD*)v[1], (const DWORD*)e);
        v[1] += 32;
    } else
        CallThis(FUN_456ea0, v, v[1], e);
}

// out = a - b
static void Vec_Diff(DVec3* out, const DVec3* a, const DVec3* b)
{
    double z = a->z - b->z;
    double y = a->y - b->y;
    double x = a->x - b->x;
    out->x = x;
    out->y = y;
    out->z = z;
}

static int s_noVehicle = -1;

// [FUN_424550] xLoadEntities: every ninth call, ranks the computer players, the vehicle presents and the way
// points by their nearness to the player's car, drops the far vehicles and loads the near ones
extern "C" void __cdecl Ai_LoadEntities()
{
    Trace_EnterBuffered(s_xLoadEntities);
    if (++g_loadTick68d13c > 8)
        g_loadTick68d13c = 0;
    else if (g_loadTick68d13c != 0) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_xLoadEntities);
        return;
    }
    g_loadCount68d1b4++;
    BYTE* v[3];
    char alloc;
    CallThis(FUN_487730, v, &alloc);
    BYTE* pl;
    Player_FromId((int*)&pl);
    if (pl == (BYTE*)g_players) {
        int r;
        pl = (BYTE*)*Player_CarId(&r);
        if (F(int, pl, 88) != 0x12345678) {
            CallThis(FUN_44b0b0, v);
            Buf_Append(&g_trace, 0, g_tagMpl, 8);
            Buf_AppendString(&g_trace, 0, s_xLoadEntities);
            return;
        }
    }
    DVec3 c = {F(double, pl, 624), F(double, pl, 632), F(double, pl, 640)};
    DVec3 size;
    memcpy(&size, pl + 352, 24);

    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = AI_ListData(it);
        if (F(int, d, 76) != -1 && !Vehicle_Flag4012(F(int, d, 76)))
            continue;
        if (Player_IsHuman(it))
            continue;
        DVec3 rel[2];
        DVec3 q;
        if (!(F(DWORD, AI_ListData(it), 88) & 1) && !((F(DWORD, AI_ListData(it), 88) >> 2) & 1)) {
            BYTE pos[32], pos2[32], next[32];
            Rec_Copy32(AI_ListData(it) + 16, 0, pos);
            DVec3 p;
            PositionId pid;
            memcpy(&pid, pos, 32);
            Road_PointOf(&p, pid);
            Vec_Diff(&rel[0], &p, &c);
            double speed = F(double, AI_ListData(it), 336) * 0.25;
            Rec_Copy32(AI_ListData(it) + 16, 0, pos2);
            DWORD* pd = (DWORD*)pos2;
            CallC(FUN_43bf10, next, it, pd[0], pd[1], pd[2], pd[3], pd[4], pd[5], pd[6], pd[7], &speed, 0);
            q.x = size.x * 0.25;
            q.y = size.y * 0.25;
            q.z = size.z * 0.25;
            memcpy(&pid, next, 32);
            Road_PointOf(&p, pid);
            Vec_Diff(&rel[1], &p, &q);
        } else {
            BYTE st[24];
            const DVec3* s = (const DVec3*)Player_CopyState(st, it);
            Vec_Diff(&rel[0], s, &c);
            q.x = size.x * 0.25;
            q.y = size.y * 0.25;
            q.z = size.z * 0.25;
            DVec3 t, sum;
            DVec3_Times(&t, (const DVec3*)(AI_ListData(it) + 344), 0.25);
            DVec3_Add(&sum, &rel[0], &t);
            Vec_Diff(&rel[1], &sum, &q);
        }
        double w = AI_MaxInverse((const double*)rel);
        BYTE st[24];
        const DVec3* s = (const DVec3*)Player_CopyState(st, it);
        DVec3 r;
        Vec_Diff(&r, s, &c);
        double dist = x87_sqrt((r.y * r.y + r.z * r.z) + r.x * r.x);
        d = AI_ListData(it);
        if (F(int, d, 76) != -1 && Vehicle_Exists(F(int, d, 76)))
            w = w * 1.2;
        if (Player_UpdateFlagsGet(it))
            w = w * 1.25;
        if (F(int, AI_ListData(it), 84) == 8)
            w = w * 0.7;
        if (F(DWORD, AI_ListData(it), 88) & 0x80000)
            w = w * 1.05;
        if (g_loadNear68d138) {
            int veh = F(int, AI_ListData(it), 76);
            if (!(veh != -1 && Vehicle_Exists(veh) && !(dist > 450.0)) && !Player_UpdateFlagsGet(it)) {
                int cur;
                int* pc = Player_FromId(&cur);
                if (F(int, AI_ListData(it), 168) != *pc)
                    w = 0.0;
            }
        }
        LoadEntry e;
        DWORD* wd = (DWORD*)&w;
        DWORD* dd = (DWORD*)&dist;
        Rec_Init425580((int*)&e, 0, it, wd[0], wd[1], dd[0], dd[1]);
        Entries_Push(v, &e);
    }

    BYTE* head = (BYTE*)g_68cf6c;
    for (BYTE* n = *(BYTE**)head; n != head; n = *(BYTE**)n) {
        BYTE* p = F(BYTE*, n, 8);
        if (!(F(BYTE, p, 8) & 32))
            continue;
        if (F(int, p, 0) != -1 && !Vehicle_Flag4012(F(int, p, 0)))
            continue;
        DVec3 rel[2], q;
        Vec_Diff(&rel[0], (const DVec3*)(p + 32), &c);
        q.x = size.x * 0.25;
        q.y = size.y * 0.25;
        q.z = size.z * 0.25;
        Vec_Diff(&rel[1], &rel[0], &q);
        double w = AI_MaxInverse((const double*)rel);
        double dist = x87_sqrt((rel[0].x * rel[0].x + rel[0].y * rel[0].y) + rel[0].z * rel[0].z);
        int veh = F(int, F(BYTE*, n, 8), 0);
        if (veh != -1 && Vehicle_Exists(veh))
            w = w * 1.2;
        LoadEntry e;
        DWORD* wd = (DWORD*)&w;
        DWORD* dd = (DWORD*)&dist;
        Rec_Init4255d0((int*)&e, 0, n, wd[0], wd[1], dd[0], dd[1]);
        Entries_Push(v, &e);
    }

    for (int i = 0; i < g_wayPoints67b2b8; i++) {
        int id = -1;
        float fp[3] = {0, 0, 0};
        if (Way_Point(i, (DWORD*)fp, &id))
            continue;
        if (id != -1 && !Vehicle_Flag4012(id))
            continue;
        DVec3 p = {fp[0], fp[1], fp[2]};
        DVec3 rel[2], q;
        Vec_Diff(&rel[0], &p, &c);
        q.x = size.x * 0.25;
        q.y = size.y * 0.25;
        q.z = size.z * 0.25;
        Vec_Diff(&rel[1], &rel[0], &q);
        double w = AI_MaxInverse((const double*)rel);
        double dist = x87_sqrt((rel[0].x * rel[0].x + rel[0].y * rel[0].y) + rel[0].z * rel[0].z);
        w = w * 1.05;
        if (id != -1 && Vehicle_Exists(id))
            w = w * 1.2;
        LoadEntry e;
        e.vehicle = id;
        e.pad = 0;
        e.weight = w;
        e.dist = dist;
        e.ref = &s_noVehicle;
        e.player = (BYTE*)g_players;
        Entries_Push(v, &e);
    }

    if (v[0] != v[1]) {
        int k = 0;
        for (int n = (int)(v[1] - v[0]) >> 5; n != 1; n >>= 1)
            k++;
        CallC(FUN_45e2a0, v[0], v[1], 0, k * 2, 0);
        CallC(FUN_45e370, v[0], v[1], 0);
    }
    // drop the vehicles of the entries without weight (from the end)
    while (v[0] != v[1]) {
        LoadEntry* e = (LoadEntry*)(v[1] - 32);
        if (e->weight > 1e-8)
            break;
        int veh = Entry_Vehicle(e);
        if (veh != -1 && Vehicle_Exists(veh)) {
            veh = Entry_Vehicle(e);
            if (veh != -1)
                Vehicle_Release(veh);
        }
        v[1] -= 32;
    }
    // load the near ones, making room by dropping the far ones from the end
    LoadEntry* last = (LoadEntry*)(v[1] - 32);
    for (LoadEntry* e = (LoadEntry*)v[0]; e != (LoadEntry*)v[1]; e++) {
        int veh = Entry_Vehicle(e);
        if (veh != -1 && Vehicle_Exists(veh))
            continue;
        if (!(e->dist > g_loadRadius68d128))
            continue;
        for (;;) {
            if (Vehicle_TimeOver(Entry_Vehicle(e)))
                break;
            for (;;) {
                if (last <= e)
                    goto done;
                int lv = Entry_Vehicle(last);
                if (lv != -1 && Vehicle_Exists(lv) && last->dist > g_loadRadius68d128)
                    break;
                last--;
            }
            int lv = Entry_Vehicle(last);
            if (lv != -1)
                Vehicle_Release(lv);
            last--;
        }
        veh = Entry_Vehicle(e);
        if (veh == -1)
            continue;
        if (e->player != (BYTE*)g_players) {
            BYTE pos[32];
            PositionId* pp = CallC<PositionId*>(FUN_4376a0, pos, e->player);
            Player_SetPosition(e->player + 8, 0, pp);
        }
        CallC(FUN_4d8b20, Entry_Vehicle(e));
    }
done:
    F(DWORD, &g_loadRadius68d128, 0) = 0;
    F(DWORD, &g_loadRadius68d128, 4) = 1080213504;
    g_loadDone68d130 = 1;
    if (v[2] != v[0])
        Ai_PoolFree(v[0], v[2] - v[0]);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_xLoadEntities);
}

// ---------------------------------------------------------------------------
// the world settings

extern "C" void FUN_407570(); KING_ALIAS("_FUN_407570", "_Ai_ReadPlayerType")    // reads a player type (arg, pair handle by value)
KING_VAR(double, g_startX68b9a0, 68b9a0)
KING_VAR(double, g_startY68b9a8, 68b9a8)
KING_VAR(double, g_startZ68b9b0, 68b9b0)
KING_VAR(DVec3, g_start68bab0, 68bab0)
typedef int IntTable4[4];
KING_VAR(IntTable4, g_forSale68ced0, 68ced0)
KING_VAR(int, g_forSaleUsed68cee8, 68cee8)
KING_VAR(int, g_forSaleStolen68cef8, 68cef8)
KING_VAR(int, g_forHire68d150, 68d150)

// the number value of the head of the list after n steps of pair (n >= 1, the first one checked)
static double Setting_Number(StrRef* pair, int n)
{
    StrRef t[4];
    Str_Member32Checked(&t[0], pair);
    for (int i = 1; i < n; i++)
        Str_Member32(&t[i], &t[i - 1]);
    StrRef m, num;
    Str_Member20(&m, &t[n - 1]);
    NumberHandle_From((DWORD*)&num, 0, (const DWORD*)&m);
    Str_Release(&m);
    for (int i = n - 1; i >= 0; i--)
        Str_Release(&t[i]);
    CheckHandle(&num);
    double v = CallThis<double>(FUN_4b0e90, num.ptr);
    CallThis(FUN_4a36c0, &num);
    return v;
}

// the int value of the head after the key
static int Setting_Int(StrRef* pair)
{
    StrRef t, m, num;
    Str_Member32Checked(&t, pair);
    Str_Member20(&m, &t);
    NumberHandle_From((DWORD*)&num, 0, (const DWORD*)&m);
    CallThis(FUN_4a36c0, &m);
    CallThis(FUN_4a36c0, &t);
    int v = CallThis<int>(FUN_4b0ea0, CallThis<BYTE*>(FUN_4aa430, &num));
    CallThis(FUN_4a36c0, &num);
    return v;
}

// [FUN_408080] reads a world setting pair (key value...): the player types, the start position, the
// numbers of vehicles and presents, the scenario
extern "C" void __cdecl Ai_ReadSetting(int arg, StrRef pair)
{
    StrRef l = {0, 0, 0};
    if (Pair_HeadIs(&pair, "playertypes")) {
        StrRef t;
        Ref_Assign(&l, Str_Member32Checked(&t, &pair));
        Str_Release(&t);
        while ((BYTE)l.flags == 2) {
            if (Head_Type(&l) == 2) {
                StrRef e, h;
                Str_Member20(&e, &l);
                PairHandle_From((DWORD*)&h, 0, (const DWORD*)&e);
                CallC(FUN_407570, arg, h.ptr, h.len, h.flags);
                Str_Release(&e);
            }
            List_Advance(&l);
        }
    } else if (Pair_HeadIs(&pair, "startposition")) {
        g_startX68b9a0 = Setting_Number(&pair, 1);
        g_startY68b9a8 = Setting_Number(&pair, 2);
        g_startZ68b9b0 = Setting_Number(&pair, 3);
        g_start68bab0.x = g_startX68b9a0;
        g_start68bab0.y = g_startY68b9a8;
        g_start68bab0.z = g_startZ68b9b0;
    } else if (Pair_HeadIs(&pair, "forsale"))
        g_forSale68ced0[2] = Setting_Int(&pair);
    else if (Pair_HeadIs(&pair, "forsale_used"))
        g_forSaleUsed68cee8 = Setting_Int(&pair);
    else if (Pair_HeadIs(&pair, "forsale_stolen"))
        g_forSaleStolen68cef8 = Setting_Int(&pair);
    else if (Pair_HeadIs(&pair, "forhire"))
        g_forHire68d150 = Setting_Int(&pair);
    else if (Pair_HeadIs(&pair, "fuel"))
        g_hiddenMax68a98c = Setting_Int(&pair);
    else if (Pair_HeadIs(&pair, "containers"))
        g_cargoMax68a964 = Setting_Int(&pair);
    else if (Pair_HeadIs(&pair, "semitrailers"))
        g_baseMax68cf54 = Setting_Int(&pair);
    else if (Pair_HeadIs(&pair, "missions"))
        g_roadMax68d0a0 = Setting_Int(&pair);
    else if (Pair_HeadIs(&pair, "parkings"))
        g_parkingMax68bb54 = Setting_Int(&pair);
    else if (Pair_HeadIs(&pair, "scenario")) {
        StrRef t, m, h;
        Str_Member32Checked(&t, &pair);
        Str_Member20(&m, &t);
        Handle9_Init((void**)&h, 0, (void**)&m);
        CallThis(FUN_4a36c0, &m);
        CallThis(FUN_4a36c0, &t);
        g_68d188 = 0;
        if (SchValue_IsSymbol((BYTE*)&h, "races"))
            g_68d188 = 1;
        if (SchValue_IsSymbol((BYTE*)&h, "trial"))
            g_68d188 = 2;
        CallThis(FUN_4a36c0, &h);
    }
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_4a36c0, &pair);
}

// ---------------------------------------------------------------------------
// queryBribeSum

extern "C" void FUN_4715a0(); KING_ALIAS("_FUN_4715a0", "_Order_PayTotal")    // an order's pay (order, double done, int)
extern "C" int __cdecl Player_Driver(BYTE* p);
extern "C" int __cdecl Player_HasPlaceFlag1(BYTE* p);
extern "C" double __cdecl Order_Pay(const BYTE* o, double late, int bonus);
extern "C" double __cdecl Vehicle_Value4cd730(int id);

static const char s_queryBribeSum[] =
    "queryBribeSum(PlayerId giver, PlayerId taker, unsigned long int flags, double & sum)";

X87 double Log10_x87(double x) { __asm { fldlg2 } __asm { fld x } __asm { fyl2x } }

// rounds a sum up to its one or two leading digits (one digit below 100, ...)
static double Bribe_Round(double s)
{
    int d = Ftol(Log10_x87(s) - 0.302);
    if (d > 2)
        d = 2;
    int m = 1;
    for (; d > 0; d--)
        m *= 10;
    double md = (double)m;
    return (double)(Ftol((md + s - 1.0) / md) * m);
}

// the value of the giver's cargo: its driver's pay for the done part, else +960; plus +224
static double Bribe_Cargo(BYTE* giver)
{
    BYTE* drv = (BYTE*)Player_Driver(giver);
    double a = drv ? CallC<double>(FUN_4715a0, drv, (double)F(int, giver, 528), 1) : F(double, giver, 960);
    return a + F(double, giver, 224);
}

// the giver's order margin (pay now minus pay late), or def without a driver
static double Bribe_Margin(BYTE* giver, double def)
{
    BYTE* drv = (BYTE*)Player_Driver(giver);
    if (!drv)
        return def;
    double p = Order_Pay(drv, 0.0, 1);
    return p - Order_Pay(drv, 1.0, 0);
}

static bool Orders_Empty(BYTE* pl)
{
    BYTE* o = Player_Orders(pl, 0);
    return F(BYTE*, o, 0) == F(BYTE*, o, 4);
}

static int Bribe_Leave(int r)
{
    Buf_Append(&g_trace, 0, g_tagMpl, 8);
    Buf_AppendString(&g_trace, 0, s_queryBribeSum);
    return r;
}

static int Bribe_LeaveInline(int r)
{
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_queryBribeSum);
    return r;
}

static int Bribe_LeaveTag(int r)
{
    Trace_Tag((const DWORD*)g_tagMpl);
    Buf_AppendString(&g_trace, 0, s_queryBribeSum);
    return r;
}

// the half-rule: (a - (base + a) / 2) * 2
static double Bribe_Half(double a, double base) { return (a - (base + a) * (1.0 - 0.5)) / 0.5; }

// [FUN_4315f0] queryBribeSum: the bribe the giver must pay the taker for the flags' offences, rounded;
// 0 accepted, 1 not possible, 2 refused
extern "C" int __cdecl Ai_QueryBribeSum(BYTE* giver, BYTE* taker, unsigned flags, double* sum)
{
    Trace_EnterBuffered(s_queryBribeSum);
    if (Player_NotValid(giver)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 12906, s_invalidId, s_playerIdType, g_numberText);
    }
    if (Player_NotValid(taker)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 12907, s_invalidId, s_playerIdType, g_numberText);
    }
    *sum = 0.0;
    switch (F(int, taker, 92)) {
    case 5:
        if (flags & 1) {
            BYTE* drv = (BYTE*)Player_Driver(giver);
            double a = Player_TotalValue(giver);
            if (Player_HasCargoOrder(giver))
                a = CallC<double>(FUN_4715a0, drv, (double)F(int, giver, 528), 1) + a;
            *sum = Bribe_Half(a, 200.0) + *sum;
            *sum = Bribe_Round(*sum);
            if (!(*sum > 0.0))
                return Bribe_Leave(1);
        }
        if (flags & 2) {
            *sum = Bribe_Half(Bribe_Cargo(giver), 500.0) + *sum;
            *sum = Bribe_Round(*sum);
            if (!(*sum > 0.0))
                return Bribe_Leave(1);
        }
        if (flags & 4) {
            if (Orders_Empty(giver) || g_68a980 == g_68a984)
                return Bribe_LeaveInline(1);
            *sum = Bribe_Half(Bribe_Margin(giver, 200.0), 500.0) + *sum;
            *sum = Bribe_Round(*sum);
            if (!(*sum > 0.0))
                return Bribe_Leave(1);
        }
        return Bribe_Leave(Player_HasPlaceFlag1(taker + 8) ? 0 : 2);
    case 6:
        if (flags & 8) {
            *sum = Bribe_Cargo(giver) * 0.7 + *sum;
            *sum = Bribe_Round(*sum);
            if (!(*sum > 0.0))
                return Bribe_Leave(1);
        }
        if (flags & 4) {
            if (Orders_Empty(giver) || g_68a980 == g_68a984)
                return Bribe_LeaveInline(1);
            *sum = Bribe_Margin(giver, 200.0) * 0.7 + *sum;
            *sum = Bribe_Round(*sum);
            if (!(*sum > 0.0))
                return Bribe_Leave(1);
        }
        return Bribe_Leave(0);
    case 2:
        if (flags & 16) {
            if (!Player_UpdateFlagsGet(taker) || Orders_Empty(taker) || Orders_Empty(giver))
                return Bribe_LeaveInline(1);
            *sum = Bribe_Margin(giver, 200.0) * 0.5 + *sum;
            *sum = Bribe_Round(*sum);
            if (!(*sum > 0.0))
                return Bribe_Leave(1);
        }
        if (flags & 32) {
            if (!Player_UpdateFlagsGet(taker) || Orders_Empty(giver))
                return Bribe_LeaveInline(1);
            double a = Bribe_Margin(giver, 1200.0);
            if (x87_eq(a, 0.0))
                a = 1200.0;
            double r = Bribe_Round(*sum);
            *sum = r;
            *sum = a * 0.5 + r;
            if (!(*sum > 0.0))
                return Bribe_Leave(1);
        }
        if (!((double)Rng_Random(&g_rng68b9c8, 0, 1000000) < 0.5 * 1000000.0)) {
            Buf_Append(&g_trace, 0, g_tagMpl, 8);
            Buf_AppendString(&g_trace, 0, s_queryBribeSum);
            return 2;
        }
        // fall through
    case 7:
    case 8:
    case 9:
        if (!(flags & 64))
            return Bribe_LeaveInline(0);
        {
            int v = F(int, taker, 84);
            double a = v == -1 ? 0.0 : Vehicle_Value4cd730(v);
            *sum = a * 5000.0 + *sum + 500.0;
            *sum = Bribe_Round(*sum);
            if (!(*sum > 0.0))
                return Bribe_LeaveTag(1);
        }
        if (F(int, g_images68a958 + F(int, taker, 992) * 56, 0) == 2 || Player_UpdateFlagsGet(taker))
            return Bribe_LeaveTag(2);
        {
            double limit = F(int, g_images68a958 + F(int, giver, 992) * 56, 0) == 2 ? 200000.0 : 500000.0;
            if ((double)Rng_Random(&g_rng68b9c8, 0, 1000000) < limit)
                return Bribe_LeaveInline(2);
        }
        return Bribe_LeaveInline(0);
    default:
        return Bribe_LeaveInline(1);
    }
}

// ---------------------------------------------------------------------------
// reading a town

extern "C" void FUN_4052f0(); KING_ALIAS("_FUN_4052f0", "_Merch_IndexOf")    // a town's index by its identifier (out, identifier handle by value)
extern "C" char** __fastcall CharVec_Construct(char** self, void*, const char* s);   // cargo_orders.cpp
extern "C" void FUN_4af480(); KING_ALIAS("_FUN_4af480", "_SchList_Nth")    // the n-th element of a list (out, n, list by value)
extern "C" void __fastcall Vec16_PushBack(void* s, void*, const void* x);
extern "C" void __fastcall Vec4_PushBackE(void* s, void*, const void* x);
extern "C" void __fastcall Vec4_PushBackF(void* s, void*, const void* x);

// the C string of a handle's object
static const char* Ref_Text(StrRef* r) { return F(const char*, CallThis<BYTE*>(FUN_4aa430, r), 12); }

// [FUN_40e8e0] reads a town: (id name side [n] (node...) - - (town...) - (node | (node...))...)
extern "C" BYTE* __cdecl Ai_ReadTown(void* reader, StrRef h)
{
    List_Advance(&h);
    StrRef id = {0, 0, 0};
    {
        StrRef e;
        Str_Member20(&e, &h);
        Handle9_Init((void**)&id, 0, (void**)&e);
        Str_Release(&e);
    }
    BYTE* o = (BYTE*)operator new(112);
    if (o) {
        char alloc;
        CheckHandle(&id);
        CharVec_Construct((char**)o, 0, F(const char*, id.ptr, 12));
        CallThis(FUN_487730, o + 12, &alloc);
        F(int, o, 24) = -1;
        CallThis(FUN_487730, o + 28, &alloc);
        CallThis(FUN_487730, o + 40, &alloc);
        F(int, o, 64) = -1;
        F(int, o, 68) = -1;
        CallThis(FUN_487730, o + 72, &alloc);
        CallThis(FUN_487730, o + 84, &alloc);
        F(int, o, 96) = 0;
        CallThis(FUN_487730, o + 100, &alloc);
    }
    List_Advance(&h);
    StrRef m;
    Str_Member20(&m, &h);
    PodVec_Assign((BYTE**)(o + 12), 0, (BYTE* const*)(m.ptr + 12));
    Str_Release(&m);
    List_Advance(&h);
    Str_Member20(&m, &h);
    F(int, o, 24) = CallThis<int>(FUN_4b0ea0, m.ptr);
    Str_Release(&m);
    List_Advance(&h);
    if (Head_Type(&h) == 6) {
        Str_Member20(&m, &h);
        F(int, o, 96) = CallThis<int>(FUN_4b0ea0, m.ptr);
        Str_Release(&m);
    }
    List_Next(&h);

    // the nodes: (node name flags...)
    StrRef l;
    Str_Member20(&l, &h);
    while ((BYTE)l.flags == 2) {
        StrRef e;
        Str_Member20(&e, &l);
        if (Head_Is(&e, "node")) {
            List_Next(&e);
            StrRef name, t;
            Str_Member20(&t, &e);
            StringHandle_From((DWORD*)&name, 0, (const DWORD*)&t);
            CallThis(FUN_4a36c0, &t);
            BYTE rec[16];
            Vec16_PushBack(o + 28, 0, Rec_Init40f820((int*)rec, 0));
            if ((name.flags || name.ptr) && strlen(Ref_Text(&name))) {
                BYTE* last = F(BYTE*, o, 32);
                int r;
                F(int, last, -16) = *Node_ByName(&r, Ref_Text(&name));
                if (F(BYTE, g_68cf00[F(int, F(BYTE*, o, 32), -16)], 12) & 8)
                    AiError(0, 200, s_mainFile, s_mainId, 2949, "Isolated node used as a town node ", Ref_Text(&name),
                            &g_aiEmpty);
            }
            List_Next(&e);
            while ((BYTE)e.flags == 2) {
                StrRef s;
                Str_Member20(&s, &e);
                if (s.flags || s.ptr) {
                    DWORD bit = 0;
                    if (SchValue_IsSymbol((BYTE*)&s, "unload"))
                        bit = 1;
                    else if (SchValue_IsSymbol((BYTE*)&s, "hidden"))
                        bit = 2;
                    else if (SchValue_IsSymbol((BYTE*)&s, "fuel"))
                        bit = 4;
                    else if (SchValue_IsSymbol((BYTE*)&s, "dummy"))
                        bit = 8;
                    if (bit)
                        F(DWORD, F(BYTE*, o, 32), -12) |= bit;
                }
                List_Next(&e);
                CallThis(FUN_4a36c0, &s);
            }
            CallThis(FUN_4a36c0, &name);
        }
        CallThis(FUN_4a36c0, &e);
        List_Next(&l);
    }
    CallThis(FUN_4a36c0, &l);
    List_Next(&h);
    List_Next(&h);

    // the linked towns
    Str_Member20(&l, &h);
    while ((BYTE)l.flags == 2) {
        StrRef e, c;
        Str_Member20(&e, &l);
        Str_CopyConstruct(&c, 0, &e);
        StrRef nth;
        CallC(FUN_4af480, &nth, 0, c.ptr, c.len, c.flags);
        StrRef idh;
        Handle9_Init((void**)&idh, 0, (void**)&nth);
        int town;
        CallC(FUN_4052f0, &town, idh.ptr, idh.len, idh.flags);
        CallThis(FUN_4a36c0, &nth);
        Vec4_PushBackE(o + 40, 0, &town);
        CallThis(FUN_4a36c0, &e);
        List_Next(&l);
    }
    CallThis(FUN_4a36c0, &l);
    List_Next(&h);

    // the node and the node lists
    StrRef k;
    Str_Member20(&k, &h);
    BYTE* lists = o + 72;
    while ((BYTE)k.flags == 2) {
        StrRef s;
        Str_Member20(&s, &k);
        BYTE t = (BYTE)s.flags;
        if (t == 3) {
            StrRef n;
            StringHandle_From((DWORD*)&n, 0, (const DWORD*)&s);
            int r;
            F(int, o, 68) = *Node_ByName(&r, Ref_Text(&n));
            CallThis(FUN_4a36c0, &n);
        } else if (t == 2) {
            StrRef q;
            Str_CopyConstruct(&q, 0, &s);
            while ((BYTE)q.flags == 2) {
                StrRef e;
                Str_Member20(&e, &q);
                if ((BYTE)e.flags == 3) {
                    StrRef n;
                    StringHandle_From((DWORD*)&n, 0, (const DWORD*)&e);
                    int node;
                    Node_ByName(&node, Ref_Text(&n));
                    Vec4_PushBackF(lists, 0, &node);
                    CallThis(FUN_4a36c0, &n);
                }
                CallThis(FUN_4a36c0, &e);
                List_Next(&q);
            }
            lists += 12;
            CallThis(FUN_4a36c0, &q);
        }
        CallThis(FUN_4a36c0, &s);
        List_Next(&k);
    }
    CallThis(FUN_4a36c0, &k);
    CallThis(FUN_4a36c0, &id);
    CallThis(FUN_4a36c0, &h);
    return o;
}

// ---------------------------------------------------------------------------
// getPresentData

extern "C" void FUN_438d70(); KING_ALIAS("_FUN_438d70", "_Road_GetRoadParameters")    // the module and room of a position (PositionId32, out44)
extern "C" void FUN_4397e0(); KING_ALIAS("_FUN_4397e0", "_Ai_OrientationFrom")    // a look-at matrix (out, const DVec3* pos, const DVec3* dir)
extern "C" void FUN_41bbe0(); KING_ALIAS("_FUN_41bbe0", "_Ai_CargoCosts")    // a player's costs for a cargo (player, double*, double*, double*, rec104, int)
extern "C" double __cdecl Order_Margin(int* o, double done, char bonus, double* price, double* value);
KING_VAR(AsmLabel, g_text77e5e0b, 77e5e0)

static const char s_getPresentData[] = "getPresentData(PlayerId player, PresentId present, GetPresentData & pd )";

// the 4x3 matrix (rows of doubles) as floats at pd+4
static void PresentData_Matrix(BYTE* pd, const double* m)
{
    for (int i = 0; i < 12; i++)
        F(float, pd, 4 + i * 4) = (float)m[i];
}

// the four name pointers (+180..) of the name vectors (+132, +144, +156, +168), 0 when empty
static void PresentData_Names(BYTE* pd)
{
    for (int i = 0; i < 4; i++)
        F(BYTE*, pd, 180 + i * 4) = 0;
    for (int i = 0; i < 4; i++)
        if (F(BYTE*, pd, 132 + i * 12) != F(BYTE*, pd, 136 + i * 12))
            F(BYTE*, pd, 180 + i * 4) = F(BYTE*, pd, 132 + i * 12);
}

// [FUN_41be60] getPresentData: a present's flags, matrix, position, module and room, and its cargo or
// vehicle's order data as the player sees it
extern "C" void __cdecl Ai_GetPresentData(BYTE* player, BYTE* present, BYTE* pd)
{
    Trace_EnterBuffered(s_getPresentData);
    if (player != (BYTE*)g_players && Player_NotValid(player)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 6221, s_invalidId, s_playerIdType, g_numberText);
    }
    if (Rec_NotValid(present)) {
        sprintf(g_text77e5e0b, "()");
        AiError(33, 100, s_mainFile, s_mainId, 6224, s_invalidId, "PresentId", g_text77e5e0b);
    }
    BYTE* rec = F(BYTE*, present, 8);
    F(DWORD, pd, 0) = F(DWORD, rec, 8);
    F(float, pd, 52) = (float)F(double, rec, 32);
    F(float, pd, 56) = (float)F(double, rec, 40);
    F(float, pd, 60) = (float)F(double, rec, 48);
    int node = F(int, rec, 24);
    bool done = false;
    if (node != -1) {
        BYTE* n = g_68cf00[node];
        F(int, pd, 72) = F(int, F(BYTE*, n, 184), 12);
        F(int, pd, 76) = F(int, F(BYTE*, n, 188), 12);
        BYTE* v = F(BYTE*, n, 188);
        F(BYTE*, pd, 64) = F(BYTE*, v, 0) != F(BYTE*, v, 4) ? F(BYTE*, v, 0) : 0;
        v = F(BYTE*, n, 184);
        F(BYTE*, pd, 68) = F(BYTE*, v, 0) != F(BYTE*, v, 4) ? F(BYTE*, v, 0) : 0;
        if (F(BYTE, n, 12) & 128) {
            PresentData_Matrix(pd, (const double*)(n + 72));
            done = true;
        }
    } else {
        DWORD r[11];
        r[0] = 0;
        r[2] = 0;
        r[3] = 0;
        r[4] = (DWORD) "UNDEFINED_MODULE";
        r[5] = (DWORD) "UNDEFINED_ROOM";
        r[6] = 0;
        r[7] = 0;
        r[9] = (DWORD)-1;
        r[10] = (DWORD)-1;
        DWORD* pos = (DWORD*)(rec + 56);
        CallC(FUN_438d70, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7], r);
        F(DWORD, pd, 72) = r[9];
        F(DWORD, pd, 76) = r[10];
        F(DWORD, pd, 64) = r[4];
        F(DWORD, pd, 68) = r[5];
    }
    if (!done) {
        PositionId pid;
        memcpy(&pid, rec + 56, 32);
        DVec3 pt;
        Road_PointOf(&pt, pid);
        DVec3 d;
        Vec_Diff(&d, &pt, (const DVec3*)(rec + 32));
        BYTE m[192];
        const double* mm = CallC<const double*>(FUN_4397e0, m, rec + 32, &d);
        PresentData_Matrix(pd, mm);
    }

    rec = F(BYTE*, present, 8);
    BYTE* c = F(BYTE*, rec, 88);
    if (c && !F(BYTE*, rec, 20)) {
        F(int, pd, 80) = F(int, c, 0);
        PodVec_Assign((BYTE**)(pd + 168), 0, (BYTE* const*)((BYTE*)g_towns[F(int, c, 12)] + 12));
        PodVec_Assign((BYTE**)(pd + 132), 0, (BYTE* const*)(g_68ccb8 + F(int, c, 0) * 104 + 88));
        F(double, pd, 216) = F(double, c, 24);
        if (player != (BYTE*)g_players) {
            double a, b, q;
            CallC(FUN_41bbe0, player, &a, &b, &q, g_68ccb8 + F(int, c, 0) * 104, -1);
            F(double, pd, 224) = a;
            F(double, pd, 232) = b;
            int town;
            Town_Id(&town, (BYTE*)g_towns[F(int, c, 12)]);
            BYTE order[144];
            Order_Construct(order, 0);
            DWORD* pos = (DWORD*)(rec + 56);
            DWORD* qd = (DWORD*)&q;
            CallC(FUN_4715f0, order, c, -1, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7], qd[0], qd[1]);
            F(int, order, 104) = 3;
            Order_SetPrice(player, (int*)order, 1, F(BYTE*, player, 520) != 0);
            if (F(BYTE*, player, 520))
                F(double, pd, 216) = 1.3 * F(double, pd, 216);
            double pay = Order_Pay(order, 0.0, 0);
            F(double, pd, 200) = pay;
            F(double, pd, 208) = pay;
            Order_Destroy(order);
        }
        PresentData_Names(pd);
    }

    F(int, pd, 272) = -1;
    F(int, pd, 276) = -1;
    rec = F(BYTE*, present, 8);
    BYTE* v = F(BYTE*, rec, 20);
    if (v) {
        BYTE* veh = (BYTE*)g_vehicles[F(int, v, 4)];
        F(int, pd, 272) = F(int, veh, 64);
        F(int, pd, 276) = F(int, (BYTE*)g_vehicles[F(int, v, 4)], 68);
        if (F(BYTE*, v, 28) != F(BYTE*, v, 32)) {
            BYTE* ord = F(BYTE*, v, 28);
            if (player != (BYTE*)g_players)
                Order_SetQuantity((int)player, (int*)ord, F(int, v, 4));
            F(int, pd, 80) = F(int, ord, 0);
            PodVec_Assign((BYTE**)(pd + 168), 0, (BYTE* const*)((BYTE*)g_towns[F(int, ord, 12)] + 12));
            PodVec_Assign((BYTE**)(pd + 132), 0, (BYTE* const*)(g_68ccb8 + F(int, ord, 0) * 104 + 88));
            if (F(BYTE, F(BYTE*, present, 8), 104)) {
                F(double, pd, 216) = F(double, ord, 72) - CallC<double>(FUN_401000);
                F(double, pd, 248) = F(double, ord, 72) - CallC<double>(FUN_401000);
            } else {
                F(double, pd, 216) = F(double, ord, 88);
                F(double, pd, 248) = F(double, ord, 88);
            }
            BYTE* r104 = g_68ccb8 + F(int, ord, 0) * 104;
            F(double, pd, 224) = F(double, r104, 16) * F(double, ord, 56);
            F(double, pd, 232) = F(double, g_68ccb8 + F(int, ord, 0) * 104, 24) * F(double, ord, 56);
            F(double, pd, 200) = Order_Pay(ord, 0.0, F(int, ord, 104) == 1);
            F(double, pd, 208) = Order_Margin((int*)ord, 0.0, F(int, ord, 104) == 1, 0, 0);
            memcpy(pd + 96, ord + 24, 32);
            F(double, pd, 88) = F(double, ord, 56);
            F(double, pd, 240) = F(double, g_68ccb8 + F(int, ord, 0) * 104, 64);
            F(BYTE, pd, 264) = 1;
            PresentData_Names(pd);
        }
    } else if (F(int, rec, 108) != -1) {
        F(int, pd, 272) = F(int, (BYTE*)g_spots68d0b8[F(int, rec, 108)], 32);
        F(int, pd, 276) = F(int, (BYTE*)g_spots68d0b8[F(int, F(BYTE*, present, 8), 108)], 36);
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_getPresentData);
}

// ---------------------------------------------------------------------------
// loading a world file

extern "C" void FUN_410340(); KING_ALIAS("_FUN_410340", "_Ai_ReadParking")    // reads a parking (reader, entry by value)
extern "C" void FUN_40f840(); KING_ALIAS("_FUN_40f840", "_Ai_ReadService")    // reads a service
extern "C" void FUN_410fe0(); KING_ALIAS("_FUN_410fe0", "_Ai_ReadObstacle")    // reads an obstacle
extern "C" void FUN_411c00(); KING_ALIAS("_FUN_411c00", "_Ai_ReadPeople")    // reads people
extern "C" void FUN_44de70(); KING_ALIAS("_FUN_44de70", "@Vec4b_InsertJ@16")
extern "C" void FUN_44e130(); KING_ALIAS("_FUN_44e130", "@Vec4b_Insert44e130@16")
extern "C" void FUN_44e6b0(); KING_ALIAS("_FUN_44e6b0", "@Vec4b_Insert44e6b0@16")
extern "C" void FUN_44ec30(); KING_ALIAS("_FUN_44ec30", "@Vec4b_Insert44ec30@16")
extern "C" void FUN_4140d0(); KING_ALIAS("_FUN_4140d0", "@Rec_Init4140d0@8")
KING_VAR(PtrVec, g_68cbe8v, 68cbe8)
KING_VAR(PtrVec, g_68cc18v, 68cc18)
KING_VAR(PtrVec, g_68a970v, 68a970)

// appends a pointer to a vector (insert_aux when full)
static void PtrVec_Push(void** v, void* x, void (*insert)())
{
    if (v[1] != v[2]) {
        Construct4((DWORD*)v[1], (const DWORD*)&x);
        v[1] = (BYTE*)v[1] + 4;
    } else
        CallThis(insert, v, v[1], &x);
}

// reads an entry with a reader proc (reader, entry by value)
static BYTE* Entry_Read(void (*read)(), StrRef* m)
{
    StrRef c;
    c.CopyFrom(m);
    return CallC<BYTE*>(read, g_reader68bb58, c.ptr, c.len, c.flags);
}

// the n-th element of a list (a copy of it)
static void List_Nth(StrRef* out, StrRef* l, int n)
{
    StrRef c;
    Str_CopyConstruct(&c, 0, l);
    CallC(FUN_4af480, out, n, c.ptr, c.len, c.flags);
}

// [FUN_4133c0] loads a world file: towns, parkings, services, obstacles, people, ambushes and node events
extern "C" void __cdecl Ai_LoadWorld(const char* file)
{
    int one = 1;
    DWORD tmp[3];
    DWORD* h = Port_MakeB(tmp, 0, (const int*)&file, &one);
    StrRef port = {0, 0, 0};
    BYTE* o = (BYTE*)h[0];
    if (o) {
        if (CallVirt<int>(o, 0) == 11) {
            port.ptr = o;
            port.flags = h[2];
            F(int, o, 4)++;
        } else
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(o, 4), "Port");
    }
    CallThis(FUN_4a36c0, tmp);
    CheckHandle(&port);
    if (CallVirt<int>(port.ptr, 40))
        AiError(23, 100, s_mainFile, s_mainId, 3456, "Cannot open", file, &g_aiEmpty);
    StrRef value;
    {
        StrRef copy;
        Str_CopyConstruct(&copy, 0, &port);
        CallC(FUN_4b4c20, &value, g_reader68bb58, copy.ptr, copy.len, copy.flags);
    }
    StrRef l;
    l.CopyFrom(&value);
    while ((BYTE)l.flags == 2) {
        StrRef m;
        Str_Member20(&m, &l);
        if (Head_Is(&m, "town"))
            PtrVec_Push((void**)&g_towns, Entry_Read((void (*)())Ai_ReadTown, &m), FUN_44de70);
        else if (Head_Is(&m, "parking"))
            PtrVec_Push((void**)&g_68cc98, Entry_Read(FUN_410340, &m), FUN_44e130);
        else if (Head_Is(&m, "service"))
            PtrVec_Push((void**)&g_68cbe8v, Entry_Read(FUN_40f840, &m), FUN_44e6b0);
        else if (Head_Is(&m, "obstacle"))
            PtrVec_Push((void**)&g_68cc18v, Entry_Read(FUN_410fe0, &m), FUN_44ec30);
        else if (Head_Is(&m, "people"))
            Entry_Read(FUN_411c00, &m);
        else if (Head_Is(&m, "ambush"))
            PtrVec_Push((void**)&g_68a970v, Entry_Read((void (*)())Ai_ReadAmbush, &m), FUN_452900);
        else {
            StrRef k;
            Str_Member20(&k, &m);
            bool ev = SchValue_IsSymbol((BYTE*)&k, "nodeevent");
            CallThis(FUN_4a36c0, &k);
            if (ev) {
                StrRef e, t, type, node, dir, name;
                Str_CopyConstruct(&e, 0, &m);
                List_Nth(&t, &e, 2);
                Handle9_Init((void**)&type, 0, (void**)&t);
                CallThis(FUN_4a36c0, &t);
                List_Nth(&t, &e, 3);
                StringHandle_From((DWORD*)&node, 0, (const DWORD*)&t);
                CallThis(FUN_4a36c0, &t);
                List_Nth(&t, &e, 4);
                Handle9_Init((void**)&dir, 0, (void**)&t);
                CallThis(FUN_4a36c0, &t);
                List_Nth(&t, &e, 5);
                StringHandle_From((DWORD*)&name, 0, (const DWORD*)&t);
                CallThis(FUN_4a36c0, &t);
                BYTE* ne = (BYTE*)operator new(56);
                if (ne)
                    ne = CallThis<BYTE*>(FUN_4140d0, ne);
                if (SchValue_IsSymbol((BYTE*)&type, "STATEBORDER"))
                    F(int, ne, 0) = 16;
                else if (!SchValue_IsSymbol((BYTE*)&type, "UNCONDITIONAL"))
                    AiError(0, 100, s_mainFile, s_mainId, 3518, "Unknown node event type", Ref_Text(&type), &g_aiEmpty);
                DWORD v[6] = {0, 0, 0, 0, 0, 0};
                int d = -1;
                if (SchValue_IsSymbol((BYTE*)&dir, "NORTH")) {
                    d = 1;
                    v[3] = 0x3ff00000;
                } else if (SchValue_IsSymbol((BYTE*)&dir, "SOUTH")) {
                    d = 2;
                    v[3] = 0xbff00000;
                } else if (SchValue_IsSymbol((BYTE*)&dir, "EAST")) {
                    d = 3;
                    v[1] = 0x3ff00000;
                } else if (SchValue_IsSymbol((BYTE*)&dir, "WEST")) {
                    d = 4;
                    v[1] = 0xbff00000;
                } else if (SchValue_IsSymbol((BYTE*)&dir, "ALL"))
                    F(int, ne, 4) = 0;
                else
                    AiError(0, 100, s_mainFile, s_mainId, 3560, "Unknown node event direction", Ref_Text(&dir),
                            &g_aiEmpty);
                if (d != -1) {
                    F(int, ne, 4) = d;
                    DVec3_Assign((DVec3*)(ne + 8), 0, (const DVec3*)v);
                }
                PodVec_Assign((BYTE**)(ne + 40), 0, (BYTE* const*)(CallThis<BYTE*>(FUN_4aa430, &name) + 12));
                int nd;
                Node_ByName(&nd, Ref_Text(&node));
                if (nd != -1)
                    F(BYTE*, g_68cf00[nd], 192) = ne;
                CallThis(FUN_4a36c0, &name);
                CallThis(FUN_4a36c0, &dir);
                CallThis(FUN_4a36c0, &node);
                CallThis(FUN_4a36c0, &type);
                CallThis(FUN_4a36c0, &e);
            }
        }
        CallThis(FUN_4a36c0, &m);
        List_Advance(&l);
    }
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_4a36c0, &value);
    CallThis(FUN_4a36c0, &port);
}

// ---------------------------------------------------------------------------
// loading the road network (*.way)

extern "C" void FUN_44a490(); KING_ALIAS("_FUN_44a490", "@Vec1_Reserve@16")
extern "C" void FUN_4611e0(); KING_ALIAS("_FUN_4611e0", "_Bytes_Construct")
extern "C" void FUN_45b3d0(); KING_ALIAS("_FUN_45b3d0", "_Names_IntroSort")    // introsort loop of 12-byte records (first, last, 0, depth, cmp)
extern "C" void FUN_45b4e0(); KING_ALIAS("_FUN_45b4e0", "_Sort_FinalInsertion12")
extern "C" void FUN_488620(); KING_ALIAS("_FUN_488620", "@WayParser_Construct@8")    // a way file parser's constructor
extern "C" void FUN_4895b0(); KING_ALIAS("_FUN_4895b0", "@Rt_ReadFile@12")    // parses a way file (name)
extern "C" void FUN_4886e0(); KING_ALIAS("_FUN_4886e0", "@RoadIndex_Destroy@4")    // the parser's destructor
extern "C" void FUN_488350(); KING_ALIAS("_FUN_488350", "_Crc_File")
extern "C" void FUN_4054a0(); KING_ALIAS("_FUN_4054a0", "_Registry52_Get")
extern "C" void FUN_4055c0(); KING_ALIAS("_FUN_4055c0", "_Registry24_Get")
extern "C" void FUN_4904e0(); KING_ALIAS("_FUN_4904e0", "_BLine_FromPoints")    // a B-line from points (out, points)
extern "C" void FUN_48e8e0(); KING_ALIAS("_FUN_48e8e0", "@Rec_Make48d8f0@12")
extern "C" void FUN_48dbb0(); KING_ALIAS("_FUN_48dbb0", "@Curve_Construct@12")    // a B-line piece from a 580-byte item (thiscall item)
extern "C" void FUN_459110(); KING_ALIAS("_FUN_459110", "@Vec1592_PushBack@12")
extern "C" void FUN_46c960(); KING_ALIAS("_FUN_46c960", "@Mult46c960_DeletingDestructor@12")
extern "C" void FUN_44a530(); KING_ALIAS("_FUN_44a530", "@Vec8_Free@8")
extern "C" void FUN_447270(); KING_ALIAS("_FUN_447270", "@Vec24_FreeStorage@8")
extern "C" void FUN_48a6e0(); KING_ALIAS("_FUN_48a6e0", "_Copy24s")
extern "C" void FUN_48c620(); KING_ALIAS("_FUN_48c620", "@Polyline_Construct@12")    // a polyline from points (thiscall points)
extern "C" void FUN_458e00(); KING_ALIAS("_FUN_458e00", "_Vec8b_AllocCopy@12")
extern "C" void FUN_405b00(); KING_ALIAS("_FUN_405b00", "_Road_LinkObject")
extern "C" void FUN_405f90(); KING_ALIAS("_FUN_405f90", "_Anchors_AttachNodes")
extern "C" void FUN_406160(); KING_ALIAS("_FUN_406160", "_Road_FinishNetwork")    // finishes the road network
extern "C" void FUN_407380(); KING_ALIAS("_FUN_407380", "@Rec_Init407380@8")
KING_VAR(BYTE*, g_wayDir68bac8, 68bac8)
KING_VAR(BYTE*, g_wayDirEnd68bacc, 68bacc)
KING_VAR(DWORD, g_wayCrc66c730, 66c730)
KING_VAR(int, g_wayCount68d1a4, 68d1a4)
KING_VAR(AsmLabel, vt_roadObject64b62c, 64b62c)
KING_VAR(AsmLabel, vt_bline64b600, 64b600)
KING_VAR(AsmLabel, vt_polyline64b5c0, 64b5c0)

// vector<8-byte>::operator=
static void Vec8_Assign(BYTE** v, BYTE* const* src)
{
    if (v == (BYTE**)src)
        return;
    BYTE* sb = src[0];
    BYTE* se = src[1];
    unsigned n = (unsigned)(se - sb) >> 3;
    if (n > (unsigned)(v[2] - v[0]) >> 3) {
        BYTE* p = CallThis<BYTE*>(FUN_458e00, v, n, sb, se);
        unsigned cap = (unsigned)(v[2] - v[0]) >> 3;
        if (cap)
            Ai_PoolFree(v[0], cap * 8);
        v[0] = p;
        v[2] = p + n * 8;
    } else
        memmove(v[0], sb, n * 8);
    v[1] = v[0] + n * 8;
}

static const char* CharVec_Text(BYTE* v) { return F(BYTE*, v, 0) != F(BYTE*, v, 4) ? F(const char*, v, 0) : 0; }

// [FUN_406600] loads the road network: every *.way file of the world directory (sorted), its modules'
// segments (B-lines and polylines, short steps dropped) and nodes
extern "C" void __cdecl Ai_LoadRoads()
{
    g_wayCrc66c730 = 0xffffffff;
    g_wayCount68d1a4 = 0;
    BYTE* files[3] = {0, 0, 0};
    char alloc;
    BYTE* path[3];
    CallThis(FUN_44a490, path, (unsigned)(g_wayDirEnd68bacc - g_wayDir68bac8), &alloc);
    memmove(path[0], g_wayDir68bac8, g_wayDirEnd68bacc - g_wayDir68bac8);
    path[1] = path[0] + (g_wayDirEnd68bacc - g_wayDir68bac8);
    char pattern[6] = {'*', '.', 'w', 'a', 'y', 0};
    CallThis(FUN_44c220, path, path[1], pattern, pattern + 6);
    WIN32_FIND_DATAA fd;
    HANDLE fh = FindFirstFileA((const char*)path[0], &fd);
    if (fh != INVALID_HANDLE_VALUE) {
        do {
            BYTE* name[3] = {0, 0, 0};
            size_t n = strlen(fd.cFileName);
            CallThis(FUN_44c220, name, (BYTE*)0, fd.cFileName, fd.cFileName + n + 1);
            if (files[1] != files[2]) {
                CallC(FUN_4611e0, files[1], &g_wayDir68bac8);
                files[1] += 12;
            } else
                CallThis(FUN_452610, files, files[1], &g_wayDir68bac8);
            BYTE* last = files[1] - 12;
            CallThis(FUN_44c220, last, F(BYTE*, last, 4), name[0], name[1]);
            if (name[2] != name[0])
                Ai_PoolFree(name[0], name[2] - name[0]);
        } while (FindNextFileA(fh, &fd));
        FindClose(fh);
    }
    if (files[0] != files[1]) {
        int k = 0;
        for (int n = (int)(files[1] - files[0]) / 12; n != 1; n >>= 1)
            k++;
        CallC(FUN_45b3d0, files[0], files[1], 0, k * 2, 0);
        CallC(FUN_45b4e0, files[0], files[1], 0);
    }

    for (BYTE* f = files[0]; f != files[1]; f += 12) {
        AI_Step3();
        BYTE parser[640];
        CallThis(FUN_488620, parser);
        CallC(FUN_488350, &g_wayCrc66c730, &g_wayCount68d1a4, F(BYTE*, f, 0));
        CallThis(FUN_4895b0, parser, F(BYTE*, f, 0));
        BYTE* head = F(BYTE*, parser, 24);
        for (BYTE* it = F(BYTE*, head, 0); it != head; it = F(BYTE*, it, 0)) {
            const char* modName = CharVec_Text(it + 8);
            BYTE* reg52 = CallC<BYTE*>(FUN_4054a0, modName);
            BYTE* reg24 = CallC<BYTE*>(FUN_4055c0, F(BYTE*, parser, 0));
            if ((BYTE*)parser != reg52 + 16)
                PodVec_Assign((BYTE**)(reg52 + 16), 0, (BYTE* const*)parser);

            for (BYTE** sp = F(BYTE**, it, 20); sp != F(BYTE**, it, 24); sp++) {
                BYTE* seg = *sp;
                if ((F(BYTE*, seg, 20) - F(BYTE*, seg, 16)) / 24 <= 1)
                    AiError(563, 1, s_mainFile, s_mainId, 1729, "Empty or single-point road segment found",
                            CharVec_Text(it + 8), &g_aiEmpty);
                BYTE* obj;
                if (F(BYTE, seg, 12) & 1) {
                    obj = (BYTE*)operator new(72);
                    BYTE bline[40];
                    BYTE items[16];
                    bool made = false;
                    if (obj) {
                        BYTE* t = CallC<BYTE*>(FUN_4904e0, bline, F(BYTE*, seg, 16));
                        BYTE** v = CallThis<BYTE**>(FUN_48e8e0, t, items);
                        made = true;
                        CallThis(FUN_407380, obj + 8);
                        F(void*, obj, 0) = vt_roadObject64b62c;
                        CallThis(FUN_487730, obj + 56, &alloc);
                        F(void*, obj, 0) = vt_bline64b600;
                        for (BYTE* e = v[0]; e != v[1]; e += 580) {
                            BYTE piece[1600];
                            BYTE* pc = CallThis<BYTE*>(FUN_48dbb0, piece, e);
                            CallThis(FUN_459110, obj + 56, pc);
                            CallThis(FUN_458910, piece);
                        }
                    }
                    if (made) {
                        BYTE** iv = (BYTE**)items;
                        for (BYTE* e = iv[0]; e != iv[1]; e += 580)
                            CallThis(FUN_46c960, e, 0);
                        unsigned cnt = (unsigned)(iv[2] - iv[0]) / 580;
                        if (cnt)
                            Ai_PoolFree(iv[0], cnt * 580);
                        CallThis(FUN_44a530, bline + 16);
                        CallThis(FUN_447270, bline + 4);
                    }
                } else {
                    BYTE** pts = (BYTE**)(seg + 16);
                restart:
                    {
                        BYTE* a = pts[0];
                        int idx = 0;
                        for (BYTE* b = a + 24; a != pts[1] && b != pts[1]; a += 24, b += 24, idx++) {
                            DVec3 d;
                            Vec_Diff(&d, (const DVec3*)b, (const DVec3*)a);
                            double len = x87_sqrt((d.y * d.y + d.z * d.z) + d.x * d.x);
                            if (len < 1e-10) {
                                if (b + 24 != pts[1])
                                    CallC(FUN_48a6e0, b + 24, pts[1], b, 0, 0);
                                pts[1] -= 24;
                                goto restart;
                            }
                            if (len > 10000.0) {
                                char text[256];
                                const double* p = (const double*)a;
                                const double* q = (const double*)b;
                                sprintf(text, "%d [%g %g %g] [%g %g %g]", idx, p[0], p[1], p[2], q[0], q[1], q[2]);
                                AiError(563, 100, s_mainFile, s_mainId, 1771, "Erroneous Bline segment data found ",
                                        CharVec_Text(it + 8), text);
                            }
                        }
                    }
                    obj = (BYTE*)operator new(104);
                    if (obj) {
                        CallThis(FUN_407380, obj + 8);
                        F(void*, obj, 0) = vt_roadObject64b62c;
                        CallThis(FUN_48c620, obj + 56, seg + 16);
                        F(void*, obj, 0) = vt_polyline64b5c0;
                    }
                    if (CallVirt<int>(obj, 24) < 1)
                        AiError(563, 1, s_mainFile, s_mainId, 1789, "Empty or single-point road segment found (1)",
                                CharVec_Text(it + 8), &g_aiEmpty);
                }
                F(DWORD, obj, 28) = F(DWORD, seg, 8);
                F(DWORD, obj, 32) = F(DWORD, seg, 0);
                F(DWORD, obj, 36) = F(DWORD, seg, 4);
                Vec8_Assign((BYTE**)(obj + 40), (BYTE* const*)(seg + 28));
                F(BYTE*, obj, 16) = reg52;
                F(BYTE*, obj, 20) = reg24;
                DWORD sf = F(DWORD, seg, 12);
                if (sf & 4)
                    F(DWORD, obj, 8) |= 4;
                if (sf & 8)
                    F(DWORD, obj, 8) = (F(DWORD, obj, 8) & ~4u) | 8;
                if (sf & 16)
                    F(DWORD, obj, 8) |= 16;
                if (sf & 32)
                    F(DWORD, obj, 8) |= 32;
                if (sf & 64)
                    F(DWORD, obj, 8) |= 192;
                F(double, reg24, 16) = CallVirt<double>(obj, 36) + F(double, reg24, 16);
                if (!(F(BYTE, seg, 12) & 1) && CallVirt<int>(obj, 24) < 1)
                    AiError(563, 1, s_mainFile, s_mainId, 1824, "Empty or single-point road segment found (2)",
                            CharVec_Text(it + 8), &g_aiEmpty);
                CallC(FUN_405b00, obj, reg52, reg24);
            }

            for (BYTE** np = F(BYTE**, it, 32); np != F(BYTE**, it, 36); np++) {
                BYTE* nd = *np;
                const char* name = CharVec_Text(nd);
                RoadNode** e = g_roadNodes;
                for (; e != g_roadNodesEnd; e++)
                    if (CharVec_Equals(&name, 0, (const CharVec*)*e))
                        break;
                RoadNode** slot;
                if (e == g_roadNodesEnd) {
                    BYTE* n = (BYTE*)operator new(208);
                    if (n)
                        n = CallThis<BYTE*>(FUN_4709b0, n, CharVec_Text(nd));
                    if (g_roadNodesEnd != (RoadNode**)g_roadNodesCap68cf08) {
                        if (g_roadNodesEnd)
                            *(BYTE**)g_roadNodesEnd = n;
                        g_roadNodesEnd++;
                    } else
                        CallThis(FUN_44cbf0, &g_roadNodes, g_roadNodesEnd, &n);
                    if (F(BYTE, nd, 136) & 1)
                        F(DWORD, g_roadNodesEnd[-1], 12) |= 8;
                    if (F(BYTE, nd, 136) & 2)
                        F(DWORD, g_roadNodesEnd[-1], 12) |= 64;
                    slot = g_roadNodesEnd - 1;
                } else {
                    AiError(0, 200, s_mainFile, s_mainId, 1848, "Duplicate node found", CharVec_Text(nd), &g_aiEmpty);
                    slot = e;
                }
                BYTE* node = (BYTE*)*slot;
                memcpy(node + 48, nd + 16, 24);
                F(BYTE*, node, 184) = reg52;
                F(BYTE*, node, 188) = reg24;
                if (F(BYTE, nd, 140)) {
                    F(DWORD, node, 12) |= 128;
                    memcpy(node + 72, nd + 40, 96);
                }
            }
        }
        CallThis(FUN_4886e0, parser);
    }

    CallC(FUN_405f90);
    {
        BYTE* last = g_anchors68cf58;
        BYTE* first;
        CallThis(FUN_4afbf0, &g_anchors68cf58, &first);
        void* out;
        List64_EraseRange(&g_anchors68cf58, 0, &out, (BYTE**)first, (BYTE**)last);
    }
    CallC(FUN_406160);
    if (path[2] != path[0])
        Ai_PoolFree(path[0], path[2] - path[0]);
    for (BYTE* f = files[0]; f != files[1]; f += 12)
        CallThis(FUN_404420, f);
    unsigned n = (unsigned)(files[2] - files[0]) / 12;
    if (n)
        Ai_PoolFree(files[0], n * 12);
}

// ---------------------------------------------------------------------------
// createNewPlayer

extern "C" void FUN_4148a0(); KING_ALIAS("_FUN_4148a0", "_Ai_PickImage")    // a new player's place (player)
extern "C" BYTE* __cdecl Players_Append(int kind);                                  // ai_drivers.cpp
KING_VAR(PtrVec, g_typeNames68cf48, 68cf48)
typedef unsigned* UintVec;
KING_VAR(UintVec, g_typeCounters68a830, 68a830)
KING_VAR(double, g_newPlayer68cfe8, 68cfe8)
KING_VAR(int, g_humanVehicleKey68d144, 68d144)
KING_VAR(int, g_humanOwner68d148, 68d148)
KING_VAR(BYTE, g_jeep68d178, 68d178)
KING_VAR(BYTE, g_allowKind7_68d1a0, 68d1a0)
KING_VAR(char*, g_humanName68d0a8, 68d0a8)
typedef char TruckName[128];
KING_VAR(TruckName, g_truckName66c60c, 66c60c)

static const char s_createNewPlayer[] = "createNewPlayer(int type, int order, VehicleId veh)";

// initializes a new 48-byte vehicle record (see PresentVehicle_New)
static BYTE* PresentVehicle_New_At(BYTE* o, int vehicle)
{
    F(int, o, 0) = -1;
    F(int, o, 4) = vehicle;
    F(DWORD, o, 8) = 0;
    F(DWORD, o, 12) = 0x3ff00000;
    DWORD v[4] = {0, 0, 0, 0x3ff00000};
    char alloc;
    Vec16b_Construct((DWORD*)(o + 16), 0, 19, v, 0);
    CallThis(FUN_487730, o + 28, &alloc);
    F(int, o, 40) = 0;
    return o;
}

// whether vehicle vi has a free slot of the kind
static bool Vehicle_HasFree(int vi, int kind)
{
    BYTE* v[3];
    Vec4b_Copy((BYTE*)v, 0, (const BYTE**)((BYTE*)g_vehicles[vi] + 36));
    return PtrVec_FindFree((int*)v[0], (int*)v[1], (int*)v[2], kind);
}

static int Vehicle_Pick(int type) { return (int)AI_PickFrom(type, g_typeCounters68a830[type]); }

static BYTE* Players_AppendNamed(int type)
{
    return Players_Append((int)AI_UniqueName((const char*)g_typeNames68cf48[type]));
}

// [FUN_415940] createNewPlayer: a new player of a type (1 human, 2.., 5/6, 7, 8, 9) with a vehicle picked for
// the type (veh, or the next suitable one), a trailer for type 2, and its name and place
extern "C" BYTE* __cdecl Ai_CreateNewPlayer(int type, int order, int veh)
{
    Trace_EnterBuffered(s_createNewPlayer);
    BYTE* p = (BYTE*)order;
    switch (type) {
    case 1: {
        p = Players_Append((int)AI_UniqueName2((const char*)g_typeNames68cf48[type], order));
        F(double, p, 128) = g_newPlayer68cfe8;
        int r;
        if (g_humanVehicleKey68d144) {
            BYTE* o = (BYTE*)operator new(48);
            F(BYTE*, p, 508) = o ? PresentVehicle_New_At(o, *Vehicle_IndexByKey(&r, g_humanVehicleKey68d144)) : 0;
        } else if (g_jeep68d178) {
            BYTE* o = (BYTE*)operator new(48);
            F(BYTE*, p, 508) = o ? PresentVehicle_New_At(o, *Vehicle_IndexByKey(&r, (int)"JEEP_TOYOTA_GREEN")) : 0;
        } else {
            BYTE* o = (BYTE*)operator new(48);
            F(BYTE*, p, 508) = o ? PresentVehicle_New_At(o, *Vehicle_IndexByName(&r, g_truckName66c60c, g_humanOwner68d148)) : 0;
        }
        break;
    }
    case 2: {
        p = Players_AppendNamed(type);
        F(double, p, 128) = g_newPlayer68cfe8;
        int vi;
        for (;;) {
            vi = Vehicle_Pick(type);
            if (!(g_68d188 && F(int, g_vehicles[vi], 32) != 3) && Vehicle_HasFree(vi, 2))
                break;
            g_typeCounters68a830[type]++;
        }
        int v;
        AI_StoreInt(&v, vi);
        BYTE* o = (BYTE*)operator new(48);
        F(BYTE*, p, 508) = o ? PresentVehicle_New_At(o, v) : 0;
        BYTE* rec = (BYTE*)g_vehicles[v];
        if (F(BYTE*, rec, 96) != F(BYTE*, rec, 100)) {
            int* list = F(int*, rec, 96);
            unsigned n = (unsigned)(F(int*, rec, 100) - list);
            unsigned k = Rng_Random(&g_rng68b9c8, 0, n);
            BYTE* t = (BYTE*)operator new(48);
            int tv;
            F(BYTE*, p, 512) = t ? PresentVehicle_New_At(t, *AI_StoreInt(&tv, list[k])) : 0;
        }
        g_typeCounters68a830[type]++;
        break;
    }
    case 7: {
        p = Players_AppendNamed(type);
        if (veh == -1) {
            int vi;
            for (;;) {
                vi = Vehicle_Pick(type);
                if (Vehicle_HasFree(vi, 7) || Vehicle_HasFree(vi, 2))
                    break;
                g_typeCounters68a830[type]++;
            }
            veh = *AI_StoreInt(&veh, vi);
            g_typeCounters68a830[type]++;
        }
        BYTE* o = (BYTE*)operator new(48);
        F(BYTE*, p, 508) = o ? PresentVehicle_New_At(o, veh) : 0;
        break;
    }
    case 8: {
        p = Players_AppendNamed(type);
        int vi;
        for (;;) {
            vi = Vehicle_Pick(type);
            if (Vehicle_HasFree(vi, 7) || Vehicle_HasFree(vi, 8) || Vehicle_HasFree(vi, 2))
                break;
            g_typeCounters68a830[type]++;
        }
        BYTE* o = (BYTE*)operator new(48);
        int v;
        F(BYTE*, p, 508) = o ? PresentVehicle_New_At(o, *AI_StoreInt(&v, vi)) : 0;
        g_typeCounters68a830[type]++;
        break;
    }
    case 9: {
        p = Players_AppendNamed(type);
        if (veh == -1) {
            int vi;
            for (;;) {
                vi = Vehicle_Pick(type);
                if (Vehicle_HasFree(vi, 9) || Vehicle_HasFree(vi, 2) || (Vehicle_HasFree(vi, 7) && g_allowKind7_68d1a0))
                    break;
                g_typeCounters68a830[type]++;
            }
            veh = *AI_StoreInt(&veh, vi);
            g_typeCounters68a830[type]++;
        }
        BYTE* o = (BYTE*)operator new(48);
        F(BYTE*, p, 508) = o ? PresentVehicle_New_At(o, veh) : 0;
        break;
    }
    case 5:
    case 6: {
        p = Players_AppendNamed(type);
        unsigned i = g_typeCounters68a830[type];
        unsigned count = (unsigned)(g_vehiclesEnd - g_vehicles);
        while (!Vehicle_HasFree(i, type))
            i = (i + 1) % count;
        BYTE* o = (BYTE*)operator new(48);
        int v;
        F(BYTE*, p, 508) = o ? PresentVehicle_New_At(o, *AI_StoreInt(&v, i)) : 0;
        g_typeCounters68a830[type] = (i + 1) % (unsigned)(g_vehiclesEnd - g_vehicles);
        break;
    }
    }
    F(int, p, 84) = type;
    if (type == 1) {
        F(BYTE*, p, 268) = F(BYTE*, p, 264);
        const char* name = g_humanName68d0a8;
        CallThis(FUN_44c220, p + 264, F(BYTE*, p, 268), name, name + strlen(name) + 1);
    } else {
        CallThis(FUN_474160, p, CallC<int>(FUN_4148a0, p));
        PodVec_Assign((BYTE**)(p + 264), 0, (BYTE* const*)(g_images68a958 + F(int, p, 984) * 56 + 4));
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_createNewPlayer);
    return p;
}

// ---------------------------------------------------------------------------
// reading an obstacle

extern "C" int* __cdecl Road_Between(int* out, int nodeA, int nodeB);              // road_graph.cpp
extern "C" void FUN_447230(); KING_ALIAS("_FUN_447230", "@CharVec_Clear@8")

// a node index by a name, -1 for an empty name
static int Node_ByNameOrNone(const char* s)
{
    if (!strlen(s))
        return -1;
    int r;
    return *Node_ByName(&r, s);
}

// [FUN_410fe0] reads an obstacle: (id name - - module text nodeA nodeB [text [text [(node...)]]]); its
// position is between its two nodes (or at one of them), then at the last listed node
extern "C" BYTE* __cdecl Ai_ReadObstacle(void* reader, StrRef h)
{
    BYTE* o = (BYTE*)operator new(128);
    if (o) {
        char alloc;
        memset(o, 0, 12);
        Rec_Init411be0((int*)(o + 12), 0);
        F(int, o, 32) = 0;
        memset(o + 36, 0, 12);
        F(int, o, 48) = -1;
        F(int, o, 52) = -1;
        F(int, o, 56) = -1;
        memset(o + 64, 0, 24);
        CallThis(FUN_487730, o + 88, &alloc);
        CallThis(FUN_487730, o + 100, &alloc);
        CallThis(FUN_487730, o + 112, &alloc);
    }
    List_Advance(&h);
    StrRef m;
    Str_Member20(&m, &h);
    Ref_Assign((StrRef*)(o + 36), &m);
    Str_Release(&m);
    Str_Member20(&m, &h);
    PodVec_Assign((BYTE**)o, 0, (BYTE* const*)(m.ptr + 12));
    Str_Release(&m);
    List_Advance(&h);
    List_Advance(&h);
    Str_Member20(&m, &h);
    int r;
    F(int, o, 12) = *Module_ByName(&r, F(const char*, m.ptr, 12));
    Str_Release(&m);
    List_Advance(&h);
    Str_Member20(&m, &h);
    PodVec_Assign((BYTE**)(o + 16), 0, (BYTE* const*)(m.ptr + 12));
    CallThis(FUN_4a36c0, &m);
    List_Next(&h);
    F(int, o, 48) = Node_ByNameOrNone(Head_Text(&h));
    List_Next(&h);
    F(int, o, 52) = Node_ByNameOrNone(Head_Text(&h));
    if ((BYTE)h.flags == 2) {
        List_Next(&h);
        for (int k = 0; k < 2; k++) {
            BYTE* text = o + 88 + k * 12;
            if ((BYTE)h.flags == 2 && Head_Type(&h) == 3) {
                Str_Member20(&m, &h);
                PodVec_Assign((BYTE**)text, 0, (BYTE* const*)(m.ptr + 12));
                CallThis(FUN_4a36c0, &m);
            } else
                CallThis(FUN_447230, text);
            if ((BYTE)h.flags != 2)
                break;
            List_Next(&h);
        }
        if ((BYTE)h.flags == 2 && Head_Type(&h) == 2) {
            StrRef p, t;
            Str_Member20(&t, &h);
            PairHandle_From((DWORD*)&p, 0, (const DWORD*)&t);
            CallThis(FUN_4a36c0, &t);
            while ((BYTE)p.flags == 2) {
                StrRef s;
                Str_Member20Checked(&t, &p);
                StringHandle_From((DWORD*)&s, 0, (const DWORD*)&t);
                CallThis(FUN_4a36c0, &t);
                const char* name = Ref_Text(&s);
                if (strlen(name)) {
                    int n;
                    Vec4_PushBackF(o + 112, 0, Node_ByName(&n, name));
                }
                CallThis(FUN_4a36c0, &s);
                Pair_Advance(&p);
            }
            CallThis(FUN_4a36c0, &p);
        }
    }
    int a = F(int, o, 48), b = F(int, o, 52);
    if (a != -1 && b != -1) {
        F(int, o, 56) = *Road_Between(&r, a, b);
        DVec3 sum, mid;
        DVec3_Add(&sum, (const DVec3*)(g_68cf00[F(int, o, 48)] + 48), (const DVec3*)(g_68cf00[F(int, o, 52)] + 48));
        DVec3_Assign((DVec3*)(o + 64), 0, DVec3_Scale(&mid, 0.5, &sum));
    } else if (a != -1)
        DVec3_Assign((DVec3*)(o + 64), 0, (const DVec3*)(g_68cf00[a] + 48));
    else if (b != -1)
        DVec3_Assign((DVec3*)(o + 64), 0, (const DVec3*)(g_68cf00[b] + 48));
    for (int* n = F(int*, o, 112); n != F(int*, o, 116); n++)
        DVec3_Assign((DVec3*)(o + 64), 0, (const DVec3*)(g_68cf00[*n] + 48));
    CallThis(FUN_4a36c0, &h);
    return o;
}

// ---------------------------------------------------------------------------
// people, parkings and services

extern "C" void FUN_412300(); KING_ALIAS("_FUN_412300", "@Rec_Init412300@8")
extern "C" void FUN_447c20(); KING_ALIAS("_FUN_447c20", "@Vec4_PushBackC@12")
extern "C" void FUN_410e10(); KING_ALIAS("_FUN_410e10", "@Rec_Init410e10@8")
extern "C" void FUN_448a70(); KING_ALIAS("_FUN_448a70", "@Vec36_PushBack@12")
extern "C" void FUN_410df0(); KING_ALIAS("_FUN_410df0", "@CharVec_Str@8")
extern "C" void FUN_448c00(); KING_ALIAS("_FUN_448c00", "@Vec4_PushBackD@12")
KING_VAR(AsmLabel, g_people68ceb0, 68ceb0)
KING_VAR(BYTE*, g_resources68cc08, 68cc08)
KING_VAR(BYTE*, g_resourcesEnd68cc0c, 68cc0c)

// [FUN_411c00] reads people: (id module (kind name name' value)...) appended to the people list
extern "C" void __cdecl Ai_ReadPeople(void* reader, StrRef h)
{
    List_Advance(&h);
    int module;
    {
        StrRef m;
        Str_Member20(&m, &h);
        Module_ByName(&module, F(const char*, m.ptr, 12));
        Str_Release(&m);
    }
    List_Advance(&h);
    while ((BYTE)h.flags == 2 && Head_Type(&h) == 2) {
        StrRef e;
        Str_Member20(&e, &h);
        BYTE* rec = (BYTE*)operator new(48);
        if (rec)
            rec = CallThis<BYTE*>(FUN_412300, rec);
        if (Head_Is(&e, "UNCONDITIONAL"))
            F(int, rec, 32) = 1;
        else if (Head_Is(&e, "DAYONLY"))
            F(int, rec, 32) = 2;
        else if (Head_Is(&e, "GOODWEATHER"))
            F(int, rec, 32) = 3;
        else if (Head_Is(&e, "DAYGW"))
            F(int, rec, 32) = 4;
        else
            AiError(0, 1, s_mainFile, s_mainId, 3283, "Undefined", &g_aiEmpty, &g_aiEmpty);
        List_Next(&e);
        StrRef m;
        Str_Member20(&m, &e);
        PodVec_Assign((BYTE**)rec, 0, (BYTE* const*)(m.ptr + 12));
        CallThis(FUN_4a36c0, &m);
        Str_Member20(&m, &e);
        PodVec_Assign((BYTE**)(rec + 16), 0, (BYTE* const*)(m.ptr + 12));
        CallThis(FUN_4a36c0, &m);
        List_Next(&e);
        Str_Member20(&m, &e);
        F(double, rec, 40) = CallThis<double>(FUN_4b0e90, m.ptr);
        CallThis(FUN_4a36c0, &m);
        F(int, rec, 12) = module;
        CallThis(FUN_447c20, g_people68ceb0, &rec);
        CallThis(FUN_4a36c0, &e);
        List_Advance(&h);
    }
    CallThis(FUN_4a36c0, &h);
}

// [FUN_410340] reads a parking: (id name - - text value - - ((name [flags [location]])...) [value])
extern "C" BYTE* __cdecl Ai_ReadParking(void* reader, StrRef h)
{
    BYTE* o = (BYTE*)operator new(80);
    if (o) {
        char alloc;
        memset(o, 0, 12);
        CallThis(FUN_487730, o + 12, &alloc);
        F(int, o, 24) = -1;
        CallThis(FUN_487730, o + 28, &alloc);
        F(int, o, 48) = 0;
        F(int, o, 40) = 0;
        F(int, o, 52) = 0;
        memset(o + 56, 0, 24);
    }
    List_Advance(&h);
    StrRef m;
    Str_Member20(&m, &h);
    Ref_Assign((StrRef*)o, &m);
    Str_Release(&m);
    List_Advance(&h);
    Str_Member20(&m, &h);
    PodVec_Assign((BYTE**)(o + 12), 0, (BYTE* const*)(m.ptr + 12));
    Str_Release(&m);
    List_Advance(&h);
    Str_Member20(&m, &h);
    F(int, o, 24) = CallThis<int>(FUN_4b0ea0, m.ptr);
    Str_Release(&m);
    List_Next(&h);
    List_Next(&h);
    StrRef l = {0, 0, 0};
    Str_Member20(&m, &h);
    Str_Assign(&l, 0, &m);
    CallThis(FUN_4a36c0, &m);
    while ((BYTE)l.flags == 2) {
        BYTE rec[36];
        CallThis(FUN_410e10, rec);
        StrRef t, s;
        Str_Member20(&t, &l);
        List_Next(&t);
        Str_Member20(&m, &t);
        StringHandle_From((DWORD*)&s, 0, (const DWORD*)&m);
        CallThis(FUN_4a36c0, &m);
        CharVec** it = NameVec_Find((CharVec**)g_roadNodes, (CharVec**)g_roadNodesEnd, Ref_Text(&s));
        if (it == (CharVec**)g_roadNodesEnd)
            AiError(90, 1, s_mainFile, s_mainId, 3114, "Can not find node named", Ref_Text(&s), &g_aiEmpty);
        else {
            F(int, rec, 0) = (int)(it - (CharVec**)g_roadNodes);
            List_Next(&t);
            if ((BYTE)t.flags == 2) {
                StrRef n;
                Str_Member20(&m, &t);
                NumberHandle_From((DWORD*)&n, 0, (const DWORD*)&m);
                CallThis(FUN_4a36c0, &m);
                if (n.flags || n.ptr)
                    F(int, rec, 4) = CallThis<int>(FUN_4b0ea0, CallThis<BYTE*>(FUN_4aa430, &n));
                List_Next(&t);
                if ((BYTE)t.flags == 2) {
                    StrRef q;
                    Str_Member20(&m, &t);
                    StringHandle_From((DWORD*)&q, 0, (const DWORD*)&m);
                    CallThis(FUN_4a36c0, &m);
                    if (q.flags || q.ptr)
                        PodVec_Assign((BYTE**)(rec + 12), 0, (BYTE* const*)(CallThis<BYTE*>(FUN_4aa430, &q) + 12));
                    CallThis(FUN_4a36c0, &q);
                }
                if (F(BYTE*, rec, 12) == F(BYTE*, rec, 16) && (F(BYTE, rec, 4) & 4))
                    AiError(0, 1, s_mainFile, s_mainId, 3137, "Location name not provided",
                            CallThis<const char*>(FUN_410df0, g_roadNodes[F(int, rec, 0)]), &g_aiEmpty);
                CallThis(FUN_4a36c0, &n);
            }
            CallThis(FUN_448a70, o + 28, rec);
        }
        CallThis(FUN_4a36c0, &s);
        CallThis(FUN_4a36c0, &t);
        CallThis(FUN_410e40, rec);
        List_Next(&l);
    }
    List_Next(&h);
    F(double, o, 48) = 4.0;
    if ((BYTE)h.flags == 2) {
        StrRef n;
        Str_Member20(&m, &h);
        NumberHandle_From((DWORD*)&n, 0, (const DWORD*)&m);
        CallThis(FUN_4a36c0, &m);
        if (n.flags || n.ptr)
            F(double, o, 48) = CallThis<double>(FUN_4b0e90, CallThis<BYTE*>(FUN_4aa430, &n));
        CallThis(FUN_4a36c0, &n);
    }
    if (F(BYTE*, o, 28) != F(BYTE*, o, 32))
        DVec3_Assign((DVec3*)(o + 56), 0, (const DVec3*)(g_68cf00[F(int, F(BYTE*, o, 28), 0)] + 48));
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_4a36c0, &h);
    return o;
}

// [FUN_40f840] reads a service: (id name - text value node ((resource number)...) [(number...)]); the
// resources set flags in the 19 resource entries
extern "C" BYTE* __cdecl Ai_ReadService(void* reader, StrRef h)
{
    BYTE* o = (BYTE*)operator new(376);
    if (o) {
        char alloc;
        for (int i = 0; i < 19; i++) {
            F(DWORD, o, i * 16 + 8) = 0;
            F(DWORD, o, i * 16) = 0;
            F(DWORD, o, i * 16 + 12) = 0x3ff00000;
        }
        memset(o + 304, 0, 12);
        CallThis(FUN_487730, o + 316, &alloc);
        F(int, o, 332) = -1;
        CallThis(FUN_487730, o + 360, &alloc);
    }
    List_Advance(&h);
    StrRef m;
    Str_Member20(&m, &h);
    Ref_Assign((StrRef*)(o + 304), &m);
    Str_Release(&m);
    List_Advance(&h);
    Str_Member20(&m, &h);
    PodVec_Assign((BYTE**)(o + 316), 0, (BYTE* const*)(m.ptr + 12));
    Str_Release(&m);
    List_Advance(&h);
    Str_Member20(&m, &h);
    F(int, o, 328) = CallThis<int>(FUN_4b0ea0, m.ptr);
    Str_Release(&m);
    List_Next(&h);
    {
        Str_Member20(&m, &h);
        int r;
        F(int, o, 332) = *Node_ByName(&r, F(const char*, m.ptr, 12));
        CallThis(FUN_4a36c0, &m);
        if (F(int, o, 332) == -1) {
            Str_Member20(&m, &h);
            AiError(0, 100, s_mainFile, s_mainId, 3043, "Node not found", F(const char*, m.ptr, 12), &g_aiEmpty);
            CallThis(FUN_4a36c0, &m);
        } else
            DVec3_Assign((DVec3*)(o + 336), 0, (const DVec3*)(g_68cf00[F(int, o, 332)] + 48));
    }
    List_Next(&h);
    StrRef l = {0, 0, 0};
    Str_Member20(&m, &h);
    Str_Assign(&l, 0, &m);
    CallThis(FUN_4a36c0, &m);
    while ((BYTE)l.flags == 2) {
        StrRef e;
        Str_Member20(&e, &l);
        if ((BYTE)e.flags == 2) {
            StrRef id, n, nth, c;
            Str_Member20(&m, &e);
            Handle9_Init((void**)&id, 0, (void**)&m);
            CallThis(FUN_4a36c0, &m);
            Str_CopyConstruct(&c, 0, &e);
            CallC(FUN_4af480, &nth, 1, c.ptr, c.len, c.flags);
            NumberHandle_From((DWORD*)&n, 0, (const DWORD*)&nth);
            CallThis(FUN_4a36c0, &nth);
            StrRef c1, key, c2;
            Str_CopyConstruct(&c1, 0, &id);
            StrRef* k = Str_Construct(&key, 0, c1.ptr, c1.len, c1.flags);
            Str_CopyConstruct(&c2, 0, k);
            BYTE* end = (BYTE*)Rec_Get1((const int*)&g_resources68cc08, 0);
            BYTE* begin = (BYTE*)Rec_Get0((const int*)&g_resources68cc08, 0);
            BYTE* it = (BYTE*)Find_ValueD(begin, end, (DWORD)c2.ptr, c2.len, c2.flags);
            CallThis(FUN_4a36c0, &key);
            if (it == g_resourcesEnd68cc0c)
                AiError(356, 1, s_mainFile, s_mainId, 3061, "Resource not found", Ref_Text(&id), &g_aiEmpty);
            int idx = (int)(it - g_resources68cc08) / 24;
            F(DWORD, o, idx * 16) |= CallThis<int>(FUN_4b0ea0, CallThis<BYTE*>(FUN_4aa430, &n));
            CallThis(FUN_4a36c0, &n);
            CallThis(FUN_4a36c0, &id);
        }
        CallThis(FUN_4a36c0, &e);
        List_Next(&l);
    }
    List_Next(&h);
    if (h.flags || h.ptr) {
        StrRef q;
        Str_Member20(&q, &h);
        if ((BYTE)q.flags == 2) {
            StrRef p = {0, 0, 0};
            Str_Assign(&p, 0, &q);
            while ((BYTE)p.flags == 2) {
                StrRef e;
                Str_Member20(&e, &p);
                if ((BYTE)e.flags == 6) {
                    StrRef n;
                    NumberHandle_From((DWORD*)&n, 0, (const DWORD*)&e);
                    int v = CallThis<int>(FUN_4b0ea0, CallThis<BYTE*>(FUN_4aa430, &n));
                    CallThis(FUN_448c00, o + 360, &v);
                    CallThis(FUN_4a36c0, &n);
                }
                CallThis(FUN_4a36c0, &e);
                List_Next(&p);
            }
            CallThis(FUN_4a36c0, &p);
        }
        CallThis(FUN_4a36c0, &q);
    }
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_4a36c0, &h);
    return o;
}

// ---------------------------------------------------------------------------
// getNoncontradictoryPositionLong

extern "C" void FUN_437520(); KING_ALIAS("_FUN_437520", "_Ai_GetPositionId")    // a player's PositionId (out, player)
extern "C" void FUN_43b570(); KING_ALIAS("_FUN_43b570", "_Ai_AdvancePosition")    // a position moved along the road (out, players, data, PositionId32, double* step, int)
extern "C" void FUN_44a4c0(); KING_ALIAS("_FUN_44a4c0", "@Vec24_Reserve@16")
extern "C" void FUN_4661d0(); KING_ALIAS("_FUN_4661d0", "_UninitFill24")
extern "C" void FUN_44a500(); KING_ALIAS("_FUN_44a500", "@Vec24_Free@8")
extern "C" void FUN_40a500(); KING_ALIAS("_FUN_40a500", "@Position_Valid@8")
KING_VAR(AsmLabel, g_positionText77e6e0, 77e6e0)

static const char s_getNoncontradictoryPositionLong[] = "getNoncontradictoryPositionLong(PlayerId player)";

// [FUN_4376a0] getNoncontradictoryPositionLong: a computer player's position, pushed along its road away
// from the other players within 160 (pairs closer than 20 repel, with a little jitter; up to 256 rounds)
extern "C" PositionId* __cdecl Ai_GetNoncontradictoryPosition(PositionId* out, BYTE* player)
{
    Trace_EnterBuffered(s_getNoncontradictoryPositionLong);
    if (Player_NotValid(player)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 15056, s_invalidId, s_playerIdType, g_numberText);
    }
    PositionId pos;
    CallC(FUN_437520, &pos, player);
    if (F(int, player, 92) == 1 || (F(BYTE, player, 96) & 1)) {
        *out = pos;
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_getNoncontradictoryPositionLong);
        return out;
    }
    DVec3 c;
    Road_PointOf(&c, pos);
    static BYTE* nodes[512];
    static DVec3 points[512];
    static PositionId posids[512];
    unsigned n = 0;
    int self = -1;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        if (!AI_ListData(it))
            continue;
        BYTE* d = AI_ListData(it);
        if (player + 8 == d) {
            nodes[n] = d;
            points[n] = c;
            posids[n] = pos;
            self = n;
            n++;
        } else {
            PositionId p;
            memcpy(&p, d + 32, 32);
            DVec3 q, v;
            Road_PointOf(&q, p);
            Vec_Diff(&v, &q, &c);
            if (x87_sqrt((v.x * v.x + v.y * v.y) + v.z * v.z) < 160.0) {
                nodes[n] = d;
                points[n] = q;
                posids[n] = p;
                n++;
            }
        }
    }
    if (n >= 2) {
        if (self == -1)
            AiError(0, 1, s_mainFile, s_mainId, 15109, "Noncontradictory position error", &g_aiEmpty, &g_aiEmpty);
        for (int round = 0; round < 256; round++) {
            BYTE* f[3];
            char alloc;
            CallThis(FUN_44a4c0, f, n, &alloc);
            DVec3 zero = {0.0, 0.0, 0.0};
            f[1] = CallC<BYTE*>(FUN_4661d0, f[0], n, &zero, self);
            DVec3* force = (DVec3*)f[0];
            bool moved = false;
            for (unsigned i = 0; i < n; i++)
                for (unsigned j = 0; j < i; j++) {
                    DVec3 d;
                    Vec_Diff(&d, &points[i], &points[j]);
                    double len = x87_sqrt((d.x * d.x + d.y * d.y) + d.z * d.z);
                    if (!(len < 20.0))
                        continue;
                    if (len > 1e-6) {
                        double inv = 1.0 / len;
                        d.x = inv * d.x;
                        d.y = d.y * inv;
                        d.z = d.z * inv;
                    }
                    Rng_Random(&g_rng68b9c8, 0, 1);
                    Rng_Random(&g_rng68b9c8, 0, 1);
                    double r = (double)Rng_Random(&g_rng68b9c8, 0, 100);
                    DVec3 jit = {r * 0.0001, r * 0.0001, r * 0.0001};
                    d.x = jit.x + d.x;
                    d.y = jit.y + d.y;
                    d.z = jit.z + d.z;
                    moved = true;
                    force[i].x = d.x + force[i].x;
                    force[i].y = d.y + force[i].y;
                    force[i].z = d.z + force[i].z;
                    force[j].x = force[j].x - d.x;
                    force[j].y = force[j].y - d.y;
                    force[j].z = force[j].z - d.z;
                }
            if (moved) {
                for (unsigned k = 0; k < n; k++) {
                    DWORD fl = F(DWORD, nodes[k], 88);
                    if ((fl & 1) || ((fl >> 2) & 1))
                        continue;
                    DVec3 tan;
                    Ai__GetTangent(&tan, posids[k]);
                    double dot = (tan.z * force[k].z + tan.y * force[k].y) + tan.x * force[k].x;
                    double step;
                    if (!(dot >= 0.0))
                        step = -2.0;
                    else if (dot > 0.0)
                        step = 2.0;
                    else
                        continue;
                    DWORD* pd = (DWORD*)&posids[k];
                    void* head;
                    Iter_Players(&head, 0);
                    PositionId np;
                    PositionId* r = CallC<PositionId*>(FUN_43b570, &np, head, nodes[k], pd[0], pd[1], pd[2], pd[3], pd[4],
                                                       pd[5], pd[6], pd[7], &step, 0);
                    posids[k] = *r;
                    Road_PointOf(&points[k], posids[k]);
                }
            }
            CallThis(FUN_44a500, f);
            if (!moved)
                break;
        }
    } else {
        *out = pos;
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_getNoncontradictoryPositionLong);
        return out;
    }
    PositionId* res = &posids[self];
    PositionId chk = *res;
    if (!CallThis<int>(FUN_40a500, &chk)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", res->road, res->t, res->segment, res->object, res->m_24);
        AiError(33, 100, s_mainFile, s_mainId, 15154, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    int road = res->road;
    if (road >= 0 && (unsigned)road < (unsigned)(g_roadEdgesEnd - g_roadEdges)) {
        BYTE* e = (BYTE*)g_roadEdges[road];
        BYTE** objs = F(BYTE**, e, 120);
        if (!(res->object < (int)(F(BYTE**, e, 124) - objs) && res->segment < CallVirt<int>(objs[res->object], 24)))
            AiError(33, 100, s_mainFile, s_mainId, 15154, "Invalid Position Id", &g_aiEmpty, &g_aiEmpty);
    }
    *out = *res;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_getNoncontradictoryPositionLong);
    return out;
}

// ---------------------------------------------------------------------------
// actOnArrival

extern "C" void FUN_417820(); KING_ALIAS("_FUN_417820", "_Ai_ArriveAt")    // a player's arrival at a position (player, PositionId32)
extern "C" void FUN_4194a0(); KING_ALIAS("_FUN_4194a0", "_Ai_RestartWaitTimer")
extern "C" void FUN_416b80(); KING_ALIAS("_FUN_416b80", "_Ai_RaceNextCheckpoint")
extern "C" void FUN_419760(); KING_ALIAS("_FUN_419760", "_Ai_GoIdle")
extern "C" void FUN_4194e0(); KING_ALIAS("_FUN_4194e0", "_Ai_SendForSomething")
extern "C" void FUN_4185d0(); KING_ALIAS("_FUN_4185d0", "_Ai_TruckerNextOrder")
extern "C" void FUN_417ed0(); KING_ALIAS("_FUN_417ed0", "_Ai_GoToGoodsTown")
extern "C" void FUN_40a2a0(); KING_ALIAS("_FUN_40a2a0", "_Ai_GoToNode")    // sends a player to a node (player, node)
extern "C" void FUN_4180f0(); KING_ALIAS("_FUN_4180f0", "_Ai_GoToOrder")    // sends a player to a town's order (player, town, order)
extern "C" void __cdecl Player_SetDeadline(BYTE* p, double t);
extern "C" int __cdecl Player_Call477bd0(BYTE* p, int a, int b);
extern "C" int __cdecl Player_Call477c30(BYTE* p);
extern "C" int __cdecl Ai_RandomOrder(int player, int town);
KING_VAR(int, g_maxRaceArrivals68d150b, 68d150)
KING_VAR(int, g_raceArrivals68d154, 68d154)

static const char s_actOnArrival[] = "actOnArrival(PlayerId player)";

// a competitor arrives: counts it, drops it and tells the player
static void Competitor_Arrived(BYTE* p)
{
    g_68d164++;
    Player_Leave(p, 0);
    CallC(FUN_483cb0, Message_Text("COMPETITOR_ARRIVED"));
}

static void ArrivalSendTo(BYTE* p)
{
    int n;
    Player_NodeOf(&n, p);
    CallC(FUN_40a2a0, p, n);
}

// [FUN_419cb0] actOnArrival: a computer player reached its target: race bookkeeping, then the next step of
// its job (cargo loading/unloading, a new order or a new node)
extern "C" int __cdecl Ai_ActOnArrival(BYTE* p)
{
    Trace_EnterBuffered(s_actOnArrival);
    if (Player_NotValid(p)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 5135, s_invalidId, s_playerIdType, g_numberText);
    }
    if (F(int, p, 92) == 1) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_actOnArrival);
        return 0;
    }
    if (F(int, p, 92) == 8) {
        DWORD* pos = (DWORD*)(p + 40);
        CallC(FUN_417820, p, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7]);
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_actOnArrival);
        return 0;
    }
    int mode = g_68d188;
    if (mode == 1 || (mode == 0 && F(int, p, 100) == 5)) {
        int count = F(int, p, 724);
        if (count == 10) {
            struct {
                DWORD flags;
                int count;
                int level;
                DWORD pad;
                double delay;
                double time;
                double sum;
                double value96;
                double price;
                double value;
            } info = {0, 0, 0, 0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0};
            info.delay = (double)(Rng_Random(&g_rng68b9c8, 0, 360) + 360);
            if (F(DWORD, p, 96) & 0x4000)
                F(double, p, 808) = CallC<double>(FUN_401000) - F(double, p, 792);
            info.time = F(double, p, 808);
            if (g_68d164 == 0 && g_68a980 != g_68a984) {
                int n = ++F(int, p, 536);
                info.count = n;
                info.flags |= n <= 1 ? 1 : 3;
                int level = Ftol(x87_sqrt((double)(n + 1)));
                if (level > 3)
                    level = 3;
                info.level = level;
                CallC(FUN_4774e0, p, info.delay, level);
            } else
                F(int, p, 536) = 0;
            if (F(int, p, 92) != 1)
                CallC(FUN_4015a0, p, 11, &info);
            F(int, p, 724)++;
            if (Player_UpdateFlagsGet(p))
                Competitor_Arrived(p);
            if (!Player_IsHuman(p))
                Player_StopWaiting(p);
            bool racing = false;
            if (g_68d188) {
                if (!Player_IsHuman(p))
                    Player_Call477c30(p);
                racing = g_68d188 != 0;
            }
            if (racing)
                mode = g_68d188;
            else if (!Player_IsHuman(p)) {
                Player_StopWaiting(p);
                F(int, p, 100) = 0;
                Player_Call477bd0(p, 0, 0x40340000);
                DWORD* pos = (DWORD*)(p + 40);
                CallC(FUN_417820, p, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7]);
                F(int, p, 724) = 0;
                Player_UpdateFlags(p);
                CallC(FUN_4194a0);
                Buf_Append(&g_trace, 0, g_tagMpl, 8);
                Buf_AppendString(&g_trace, 0, s_actOnArrival);
                return 0;
            } else
                mode = g_68d188;
        } else if (count > 10) {
            Buf_Append(&g_trace, 0, g_tagMpl, 8);
            Buf_AppendString(&g_trace, 0, s_actOnArrival);
            return 0;
        }
    } else if (mode == 0)
        goto steps;
    if (mode == 2 && F(int, p, 724) < 11) {
        if (F(int, p, 92) != 1)
            CallC(FUN_4015a0, p, 11, 0);
        F(int, p, 724) = 11;
        if (!Player_IsHuman(p)) {
            if (Player_UpdateFlagsGet(p))
                Competitor_Arrived(p);
            Player_StopWaiting(p);
            Player_Call477c30(p);
        }
    }
steps:
    CallC(FUN_4015a0, p, 8, 0);
    if (!CallC<int>(FUN_4015a0, p, 10, 0)) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_actOnArrival);
        return 0;
    }
    mode = g_68d188;
    if (mode == 1 || (mode == 0 && F(int, p, 100) == 5)) {
        CallC(FUN_416b80, p);
        Trace_Tag((const DWORD*)g_tagMpl);
        Buf_AppendString(&g_trace, 0, s_actOnArrival);
        return 0;
    }
    if (mode == 2) {
        Trace_Tag((const DWORD*)g_tagMpl);
        Buf_AppendString(&g_trace, 0, s_actOnArrival);
        return 0;
    }
    {
        int s = F(int, p, 100);
        if (s == 256)
            AiError(0, 1, s_mainFile, s_mainId, 5270, "Internal error 073415", &g_aiEmpty, &g_aiEmpty);
        else if (s == 257) {
            F(DWORD, p, 96) |= 0x400;
            double t = (double)(Rng_Random(&g_rng68b9c8, 0, 1500) + 1200);
            Player_SetDeadline(p + 8, t);
            BYTE* d = F(BYTE*, g_68cc98[F(int, p, 120)], 28) + F(int, p, 124) * 36;
            F(DWORD, d, 4) |= 2;
            if (++g_raceArrivals68d154 > g_maxRaceArrivals68d150b - 2) {
                void* end;
                void* begin;
                CallThis(FUN_4afc20, &g_players, &end);
                CallThis(FUN_4afbf0, &g_players, &begin);
                BYTE* r;
                Pick_RandomDueNode(&r, (BYTE*)begin, (BYTE*)end, 0, (DWORD*)g_rng68b9c8);
                if (r != (BYTE*)g_players)
                    CallC(FUN_419760, r);
            }
        } else if (F(int, p, 92) == 2) {
            if (s == 4) {
                if (!Player_FirstGoods(p + 8)) {
                    F(int, p, 100) = 0;
                    CallC(FUN_4194e0, p);
                } else {
                    int pid = -1;
                    double q = -1000000.0;
                    BYTE* drv = (BYTE*)Player_Driver(p);
                    BYTE* town = (BYTE*)g_towns[F(int, drv, 12)];
                    Ai_LoadOrder(p, F(int, town, 24), 0, &q, &pid);
                    F(int, p, 100) = 0;
                    Player_Call477bd0(p, 0, 0x3ff00000);
                    CallC(FUN_4194e0, p);
                    Player_UpdateFlags(p);
                }
            } else if (s == 3) {
                int pid = -1;
                double q = 1000000.0;
                Ai_LoadOrder(p, F(int, p, 128), F(int, p, 132), &q, &pid);
                if (!Player_FirstGoods(p + 8))
                    CallC(FUN_4185d0, p);
                else {
                    CallC(FUN_417ed0, p);
                    F(int, p, 100) = 4;
                }
                Player_Call477bd0(p, 0, 0x3ff00000);
            } else {
                int town;
                Town_Find4(&town, (int)g_rng68b9c8);
                F(int, p, 128) = F(int, g_towns[town], 24);
                int o = Ai_RandomOrder((int)p, F(int, p, 128));
                F(int, Iter_Data(&p, 0), 124) = o;
                if (F(int, p, 132) == -1)
                    ArrivalSendTo(p);
                else {
                    CallC(FUN_4180f0, p, F(int, p, 128), F(int, p, 132));
                    F(int, p, 100) = 3;
                }
                Player_Call477bd0(p, 0, 0x3ff00000);
            }
        } else
            ArrivalSendTo(p);
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_actOnArrival);
    return 0;
}

// ---------------------------------------------------------------------------
// reUncontradict

static const char s_reUncontradict[] = "reUncontradict()";

// the position relaxation of getNoncontradictoryPositionLong: up to 256 rounds of pairwise repulsion of
// points closer than 20, each moving its position along its road
static void Positions_Relax(unsigned n, BYTE** nodes, DVec3* points, PositionId* posids)
{
    for (int round = 0; round < 256; round++) {
        BYTE* f[3];
        char alloc;
        CallThis(FUN_44a4c0, f, n, &alloc);
        DVec3 zero = {0.0, 0.0, 0.0};
        f[1] = CallC<BYTE*>(FUN_4661d0, f[0], n, &zero, 0);
        DVec3* force = (DVec3*)f[0];
        bool moved = false;
        for (unsigned i = 0; i < n; i++)
            for (unsigned j = 0; j < i; j++) {
                DVec3 d;
                Vec_Diff(&d, &points[i], &points[j]);
                double len = x87_sqrt((d.x * d.x + d.y * d.y) + d.z * d.z);
                if (!(len < 20.0))
                    continue;
                if (len > 1e-6) {
                    double inv = 1.0 / len;
                    d.x = inv * d.x;
                    d.y = d.y * inv;
                    d.z = d.z * inv;
                }
                Rng_Random(&g_rng68b9c8, 0, 1);
                Rng_Random(&g_rng68b9c8, 0, 1);
                double r = (double)Rng_Random(&g_rng68b9c8, 0, 100);
                DVec3 jit = {r * 0.0001, r * 0.0001, r * 0.0001};
                d.x = jit.x + d.x;
                d.y = jit.y + d.y;
                d.z = jit.z + d.z;
                moved = true;
                force[i].x = d.x + force[i].x;
                force[i].y = d.y + force[i].y;
                force[i].z = d.z + force[i].z;
                force[j].x = force[j].x - d.x;
                force[j].y = force[j].y - d.y;
                force[j].z = force[j].z - d.z;
            }
        if (moved) {
            for (unsigned k = 0; k < n; k++) {
                DWORD fl = F(DWORD, nodes[k], 88);
                if ((fl & 1) || ((fl >> 2) & 1))
                    continue;
                DVec3 tan;
                Ai__GetTangent(&tan, posids[k]);
                double dot = (tan.z * force[k].z + tan.y * force[k].y) + tan.x * force[k].x;
                double step;
                if (!(dot >= 0.0))
                    step = -2.0;
                else if (dot > 0.0)
                    step = 2.0;
                else
                    continue;
                DWORD* pd = (DWORD*)&posids[k];
                void* head;
                Iter_Players(&head, 0);
                PositionId np;
                PositionId* r = CallC<PositionId*>(FUN_43b570, &np, head, nodes[k], pd[0], pd[1], pd[2], pd[3], pd[4],
                                                   pd[5], pd[6], pd[7], &step, 0);
                posids[k] = *r;
                Road_PointOf(&points[k], posids[k]);
            }
        }
        CallThis(FUN_44a500, f);
        if (!moved)
            break;
    }
}

// checks a PositionId: valid, and its object and segment exist on its road
static void Position_Check(PositionId* res, int line)
{
    PositionId chk = *res;
    if (!CallThis<int>(FUN_40a500, &chk)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", res->road, res->t, res->segment, res->object, res->m_24);
        AiError(33, 100, s_mainFile, s_mainId, line, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    int road = res->road;
    if (road >= 0 && (unsigned)road < (unsigned)(g_roadEdgesEnd - g_roadEdges)) {
        BYTE* e = (BYTE*)g_roadEdges[road];
        BYTE** objs = F(BYTE**, e, 120);
        if (!(res->object < (int)(F(BYTE**, e, 124) - objs) && res->segment < CallVirt<int>(objs[res->object], 24)))
            AiError(33, 100, s_mainFile, s_mainId, line, "Invalid Position Id", &g_aiEmpty, &g_aiEmpty);
    }
}

// [FUN_438230] reUncontradict: spreads the players within 160 of the current one apart along their roads and
// moves them there
extern "C" void __cdecl Ai_ReUncontradict()
{
    Trace_EnterBuffered(s_reUncontradict);
    int cur;
    Player_FromId(&cur);
    PositionId pos;
    CallC(FUN_437520, &pos, cur);
    DVec3 c;
    Road_PointOf(&c, pos);
    static BYTE* nodes[512];
    static DVec3 points[512];
    static PositionId posids[512];
    unsigned n = 0;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        if (!AI_ListData(it))
            continue;
        BYTE* d = AI_ListData(it);
        PositionId p;
        memcpy(&p, d + 32, 32);
        DVec3 q, v;
        Road_PointOf(&q, p);
        Vec_Diff(&v, &q, &c);
        if (x87_sqrt((v.x * v.x + v.y * v.y) + v.z * v.z) < 160.0) {
            nodes[n] = d;
            points[n] = q;
            posids[n] = p;
            n++;
        }
    }
    if (n < 2) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_reUncontradict);
        return;
    }
    Positions_Relax(n, nodes, points, posids);
    for (unsigned k = 0; k < n; k++) {
        Position_Check(&posids[k], 15237);
        DWORD fl = F(DWORD, nodes[k], 88);
        if (!(fl & 1) && !((fl >> 2) & 1))
            Player_SetPosition(nodes[k], 0, &posids[k]);
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_reUncontradict);
}

// ---------------------------------------------------------------------------
// reading a vehicle of the tech file

extern "C" void FUN_42dbf0(); KING_ALIAS("_FUN_42dbf0", "_Ai_ReadPlayerKinds")    // a list of ints from a value (out vector, value by value)
extern "C" void FUN_40e6d0(); KING_ALIAS("_FUN_40e6d0", "_Lists_NameOf")    // the name vector of an identifier (identifier by value)
extern "C" void FUN_452290(); KING_ALIAS("_FUN_452290", "@VecDword_Assign@12")
extern "C" void FUN_448ab0(); KING_ALIAS("_FUN_448ab0", "@Vec4_FreeStorageB@8")
extern "C" void FUN_447ad0(); KING_ALIAS("_FUN_447ad0", "@Vec4_PushBackA@12")
KING_VAR(AsmLabel, s_needNumber66dde8, 66dde8)

// *d = *s when s is of the type (an error otherwise), dropping d's old object
static void Ref_AssignTyped(StrRef* d, const StrRef* s, int type, const char* typeName)
{
    if (d->ptr) {
        if (--F(int, d->ptr, 4) == 0 && !Str_Same(d, s) && d->ptr)
            CallVirt(d->ptr, 12, 1);
    }
    BYTE* p = s->ptr;
    if (!p) {
        d->ptr = 0;
        d->flags = 0;
        d->len = 0;
        return;
    }
    if (CallVirt<int>(p, 0) != type) {
        CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(p, 4), typeName);
        return;
    }
    d->ptr = p;
    d->flags = s->flags;
    F(int, p, 4)++;
    d->len = 0;
}

// the head of a pair (checked)
static void Pair_Head(StrRef* out, StrRef* p) { Str_Member20Checked(out, p); }

// [FUN_42dff0] reads a vehicle of the tech file: (- id name owner kind (ints) number number pair [int]);
// appended to the vehicles
extern "C" BYTE* __cdecl Ai_ReadVehicle(StrRef h)
{
    BYTE* o = (BYTE*)operator new(120);
    if (o) {
        char alloc;
        F(int, o, 0) = -1;
        F(int, o, 4) = 0;
        memset(o + 8, 0, 12);
        CallThis(FUN_487730, o + 20, &alloc);
        F(int, o, 32) = -1;
        CallThis(FUN_487730, o + 36, &alloc);
        F(int, o, 48) = 0;
        F(int, o, 56) = 0;
        F(int, o, 52) = 0;
        F(int, o, 60) = 0;
        F(int, o, 64) = -1;
        F(int, o, 68) = -1;
        CallThis(FUN_487730, o + 72, &alloc);
        memset(o + 84, 0, 12);
        CallThis(FUN_487730, o + 96, &alloc);
        F(int, o, 112) = 0;
        F(int, o, 108) = 0;
        F(int, o, 116) = 0x3ff00000;
    }
    StrRef m;
    Pair_Advance(&h);
    Pair_Head(&m, &h);
    Ref_AssignTyped((StrRef*)(o + 8), &m, 9, "Identifier");
    Str_Release(&m);
    Pair_Advance(&h);
    Pair_Head(&m, &h);
    PodVec_Assign((BYTE**)(o + 20), 0, (BYTE* const*)(m.ptr + 12));
    Str_Release(&m);
    Pair_Advance(&h);
    Pair_Head(&m, &h);
    F(int, o, 68) = CallThis<int>(FUN_4b0ea0, m.ptr);
    CallThis(FUN_4a36c0, &m);
    Pair_Advance(&h);
    if (Pair_HeadIs(&h, "car"))
        F(int, o, 32) = 1;
    else if (Pair_HeadIs(&h, "lorry"))
        F(int, o, 32) = 2;
    else if (Pair_HeadIs(&h, "tractor"))
        F(int, o, 32) = 3;
    else if (Pair_HeadIs(&h, "semitrailer"))
        F(int, o, 32) = 4;
    else {
        Pair_Head(&m, &h);
        AiError(4, 100, s_mainFile, s_mainId, 12164, "Unsupported type", F(const char*, m.ptr, 12), &g_aiEmpty);
        CallThis(FUN_4a36c0, &m);
    }
    Pair_Advance(&h);
    {
        StrRef v;
        Pair_Head(&v, &h);
        BYTE ints[12];
        CallC(FUN_42dbf0, ints, v.ptr, v.len, v.flags);
        CallThis(FUN_452290, o + 36, ints);
        CallThis(FUN_448ab0, ints);
    }
    Pair_Advance(&h);
    Pair_Head(&m, &h);
    F(double, o, 48) = CallThis<double>(FUN_4b0e90, m.ptr);
    CallThis(FUN_4a36c0, &m);
    Pair_Advance(&h);
    Pair_Head(&m, &h);
    bool notNumber = (BYTE)m.flags != 6;
    CallThis(FUN_4a36c0, &m);
    if (notNumber)
        AiError(0, 100, s_mainFile, s_mainId, 12177, s_needNumber66dde8, &g_aiEmpty, &g_aiEmpty);
    Pair_Head(&m, &h);
    F(double, o, 56) = CallThis<double>(FUN_4b0e90, m.ptr);
    CallThis(FUN_4a36c0, &m);
    Pair_Advance(&h);
    Pair_Head(&m, &h);
    Str_AssignPair((StrRef*)(o + 84), 0, &m);
    CallThis(FUN_4a36c0, &m);
    Pair_Advance(&h);
    if (h.flags || h.ptr) {
        Pair_Head(&m, &h);
        bool set = m.flags || m.ptr;
        CallThis(FUN_4a36c0, &m);
        if (set) {
            Pair_Head(&m, &h);
            bool num = (BYTE)m.flags == 6;
            CallThis(FUN_4a36c0, &m);
            if (num) {
                Pair_Head(&m, &h);
                F(int, o, 4) = CallThis<int>(FUN_4b0ea0, m.ptr);
                CallThis(FUN_4a36c0, &m);
            }
        }
    }
    {
        StrRef c;
        Str_CopyConstruct(&c, 0, (StrRef*)(o + 8));
        BYTE* name = CallC<BYTE*>(FUN_40e6d0, c.ptr, c.len, c.flags);
        PodVec_Assign((BYTE**)(o + 72), 0, (BYTE* const*)name);
    }
    F(int, o, 0) = (int)(g_vehiclesEnd - g_vehicles);
    CallThis(FUN_447ad0, &g_vehicles, &o);
    CallThis(FUN_4a36c0, &h);
    return o;
}

// ---------------------------------------------------------------------------
// loadOrder of a present

KING_VAR(double, g_bonus66f898, 66f898)       // 1.3

static const char s_loadPresent[] = "loadOrder(PlayerId player, PresentId present, int & playerorderid)";

// [FUN_442e10] loadOrder (present): loads a present's cargo onto a player as far as its weight and volume
// allow; returns 0 or a negative reason (-3 weight, -4 volume, -10/-11 wrong trailer)
extern "C" int __cdecl Ai_LoadPresent(BYTE* player, BYTE* present, int* playerorderid)
{
    Trace_EnterBuffered(s_loadPresent);
    if (Player_NotValid(player)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 17519, s_invalidId, s_playerIdType, g_numberText);
    }
    if (Rec_NotValid(present)) {
        sprintf((char*)g_text77e5e0, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 17520, s_invalidId, s_recIdType66d64c, (char*)g_text77e5e0);
    }
    Player_IsHuman(player);
    BYTE* item = F(BYTE*, present, 8);
    int kind = F(int, item, 8) & 0xF0;
    if (kind == 0x80 || kind == 0x20 || kind == 0x10) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_loadPresent);
        return 0;
    }
    BYTE* tmpl = F(BYTE*, item, 88);
    if (tmpl) {
        if (Player_IsHuman(player)) {
            bool oil = (F(BYTE, g_68ccb8 + F(int, tmpl, 0) * 104, 48) & 4) != 0;
            if (oil && !(Player_VehicleData(player) & 1)) {
                CallC(FUN_483cb0, Message_Text("NEED_A_PETROL_SEMITRAILER"));
                Buf_Append(&g_trace, 0, g_tagMpl, 8);
                Buf_AppendString(&g_trace, 0, s_loadPresent);
                return -10;
            }
            oil = (F(BYTE, g_68ccb8 + F(int, tmpl, 0) * 104, 48) & 4) != 0;
            if (!oil && (Player_VehicleData(player) & 1)) {
                CallC(FUN_483cb0, Message_Text("LOAD_AT_AN_OIL_TANK"));
                Buf_Append(&g_trace, 0, g_tagMpl, 8);
                Buf_AppendString(&g_trace, 0, s_loadPresent);
                return -11;
            }
        }
        double ba, bb, q;
        Costs_Balance(player, &ba, &bb, &q, g_68ccb8 + F(int, tmpl, 0) * 104);
        BYTE order[144];
        Order_Construct(order, 0);
        F(int, order, 0) = F(int, tmpl, 0);
        F(int, order, 12) = F(int, tmpl, 12);
        F(int, order, 4) = F(int, tmpl, 4);
        memcpy(order + 24, item + 56, 32);
        F(int, order, 104) = 3;
        double factor = F(BYTE*, player, 520) ? g_bonus66f898 : 1.0;
        F(double, order, 72) = CallC<double>(FUN_401000) + factor * F(double, tmpl, 24);
        double a = 0.0, b = 0.0;
        Player_CargoSums(player, &a, &b);
        double price, load;
        Player_AllCosts(player, &price, &load);
        a = a - price;
        b = b - load;
        if (!(a < 0.0)) {
            Order_Destroy(order);
            Buf_Append(&g_trace, 0, g_tagMpl, 8);
            Buf_AppendString(&g_trace, 0, s_loadPresent);
            return -3;
        }
        if (!(b < 0.0)) {
            Order_Destroy(order);
            Buf_Append(&g_trace, 0, g_tagMpl, 8);
            Buf_AppendString(&g_trace, 0, s_loadPresent);
            return -4;
        }
        BYTE copy[144];
        Order144_CopyConstruct(copy, 0, order);
        BYTE* data2 = Iter_Data(&player, 0);
        void* cargo = Player_ActiveCargo(data2, 0);
        BYTE* cend = CallThis<BYTE*>(FUN_455950, cargo);
        data2 = Iter_Data(&player, 0);
        cargo = Player_ActiveCargo(data2, 0);
        BYTE* cbegin = CallThis<BYTE*>(FUN_455940, cargo);
        BYTE* o = CallC<BYTE*>(FUN_457a40, cbegin, cend, *(Blob144*)copy);
        BYTE* orders = Player_Orders(player, 0);
        if (o == F(BYTE*, orders, 4)) {
            orders = Player_Orders(player, orders);
            BYTE** v = (BYTE**)orders;
            if (v[1] != v[2]) {
                Rec458a80_Construct(v[1], order);
                v[1] += 144;
            } else
                CallThis(FUN_450960, v, v[1], order);
            orders = Player_Orders(player, orders);
            o = F(BYTE*, orders, 4) - 144;
        }
        orders = Player_Orders(player, orders);
        *playerorderid = (int)(o - F(BYTE*, orders, 0)) / 144;
        double old = F(double, o, 56);
        F(double, o, 56) = q + old;
        if (F(double, o, 56) < 0.0)
            F(double, o, 56) = 0.0;
        BYTE* rec = g_68ccb8 + F(int, o, 0) * 104;
        double r16 = F(double, rec, 16), r24 = F(double, rec, 24);
        double d = (F(double, o, 56) - old) * r16 + a;
        if (d > 0.0)
            F(double, o, 56) = F(double, o, 56) - d / r16;
        d = (F(double, o, 56) - old) * r24 + b;
        if (d > 0.0)
            F(double, o, 56) = F(double, o, 56) - d / r24;
        if (x87_eq(F(double, o, 56), 0.0)) {
            *playerorderid = -1;
            orders = Player_Orders(player, orders);
            BYTE* end = F(BYTE*, orders, 4);
            for (int n = (int)(end - (o + 144)) / 144; n > 0; n--, o += 144)
                Order144_AssignThis(o, 0, o + 144);
            end = F(BYTE*, orders, 4) - 144;
            F(BYTE*, orders, 4) = end;
            VecByteVec16_Destroy((BYTE**)(end + 120), 0);
        } else
            Order_SetPrice(player, (int*)o, 1, F(BYTE*, player, 520) != 0);
        Order_Destroy(order);
    }
    CallC(FUN_442c60, player);
    Trace_Leave(s_loadPresent);
    return 0;
}

// ---------------------------------------------------------------------------
// resetting the AI state

extern "C" void FUN_4734f0(); KING_ALIAS("_FUN_4734f0", "_Players_Clear")
extern "C" void FUN_404290(); KING_ALIAS("_FUN_404290", "@Rec404290_Destroy@8")
extern "C" void FUN_404570(); KING_ALIAS("_FUN_404570", "@Rec404570_Destroy@8")
extern "C" void FUN_4046b0(); KING_ALIAS("_FUN_4046b0", "@Rec4046b0_Destroy@8")
extern "C" void FUN_404790(); KING_ALIAS("_FUN_404790", "@Rec404790_Destroy@8")
extern "C" void FUN_404920(); KING_ALIAS("_FUN_404920", "@Rec404920_Destroy@8")
extern "C" void FUN_404ab0(); KING_ALIAS("_FUN_404ab0", "@Rec404ab0_Destroy@8")
extern "C" void FUN_44af80(); KING_ALIAS("_FUN_44af80", "@Vec4_Erase2@16")
extern "C" void FUN_426280(); KING_ALIAS("_FUN_426280", "@Int_Set@12")
extern "C" BYTE** __fastcall Values_Assign(BYTE** d, void*, BYTE* const* s);       // evaluator.cpp
typedef DWORD Dwords55[55];
typedef void* Ptrs3[3];
KING_VAR(Dwords55, r_rng68b9c8, 68b9c8)
KING_VAR(DWORD, r_rngI68baa4, 68baa4)
KING_VAR(DWORD, r_rngJ68baa8, 68baa8)
KING_VAR(BYTE, r_68d0ec, 68d0ec)
KING_VAR(double, r_68cfe8, 68cfe8)
KING_VAR(double, r_66c6f0, 66c6f0)
KING_VAR(BYTE, r_68d178, 68d178)
KING_VAR(Ptrs3, r_68cf28, 68cf28)
KING_VAR(Ptrs3, r_68a840, 68a840)
KING_VAR(Ptrs3, r_68a980, 68a980)
KING_VAR(Ptrs3, r_68cf60, 68cf60)
KING_VAR(Ptrs3, r_68cec0, 68cec0)
KING_VAR(Ptrs3, r_68cc98, 68cc98)
KING_VAR(Ptrs3, r_68cbe8, 68cbe8)
KING_VAR(Ptrs3, r_68cc18, 68cc18)
KING_VAR(Ptrs3, r_68ceb0, 68ceb0)
KING_VAR(Ptrs3, r_68a970, 68a970)
KING_VAR(Ptrs3, r_68cca8, 68cca8)
KING_VAR(Ptrs3, r_68a830, 68a830)
KING_VAR(Ptrs3, r_68cc88, 68cc88)
KING_VAR(Ptrs3, r_68a958, 68a958)
KING_VAR(int, r_68d188, 68d188)
KING_VAR(int, r_68d158, 68d158)
KING_VAR(BYTE, r_66c68c, 66c68c)
typedef DWORD Dwords12[12];
KING_VAR(Dwords12, r_68ced0, 68ced0)      // 68ced0..68ceff: three 16-byte records
KING_VAR(int, r_68d14c, 68d14c)
KING_VAR(int, r_68d160, 68d160)
KING_VAR(int, r_68d164, 68d164)
KING_VAR(DWORD, r_66c6d8, 66c6d8)
KING_VAR(DWORD, r_66c6dc, 66c6dc)
KING_VAR(int, r_68cf54, 68cf54)
KING_VAR(int, r_68a964, 68a964)
KING_VAR(int, r_68d0a0, 68d0a0)
KING_VAR(int, r_68bb54, 68bb54)
KING_VAR(int, r_68a968, 68a968)
KING_VAR(int, r_68a98c, 68a98c)
KING_VAR(int, r_68d01c, 68d01c)
KING_VAR(int, r_68cfe4, 68cfe4)
KING_VAR(int, r_68d0e4, 68d0e4)
KING_VAR(int, r_68cf34, 68cf34)
KING_VAR(BYTE, r_66f62a, 66f62a)
KING_VAR(BYTE, r_68d31c, 68d31c)
KING_VAR(int, r_68d18c, 68d18c)
KING_VAR(double, r_68d190, 68d190)
KING_VAR(int, r_66c700, 66c700)
KING_VAR(int, r_66c6e0, 66c6e0)
KING_VAR(double, r_66c6f8, 66c6f8)
KING_VAR(double, r_68d128, 68d128)
KING_VAR(BYTE, r_68d130, 68d130)
KING_VAR(BYTE, r_66c60a, 66c60a)
KING_VAR(int, r_68d134, 68d134)
KING_VAR(BYTE, r_68d138, 68d138)
KING_VAR(double, r_66c710, 66c710)

// deletes every object of a pointer vector (destructor + operator delete)
static void Ptrs_DeleteAll(Ptrs3& v, void (*dtor)())
{
    for (void** e = (void**)v[0]; e != (void**)v[1]; e++)
        if (*e) {
            CallThis(dtor, *e);
            operator delete(*e);
        }
}

// *v = n zero dwords
static void Dwords_AssignZeros(Ptrs3& v, int n)
{
    DWORD z[12] = {0};
    DWORD* t[2] = {z, z + n};
    VecDword_Assign((DWORD**)v, 0, t);
}

// [FUN_403940] resets the AI: random generator, settings, tables and lists; deletes towns, roads and the rest
extern "C" void __cdecl Ai_Reset()
{
    r_68d0ec = 0;
    memset(r_rng68b9c8, 0, sizeof r_rng68b9c8);
    DWORD seed = 161803398, current = 1;
    r_rng68b9c8[54] = seed;
    for (unsigned offset = 0; offset < 1134; offset += 21) {
        unsigned slot = offset % 55;
        seed -= current;
        r_rng68b9c8[slot] = current;
        current = seed;
        seed = r_rng68b9c8[slot];
    }
    for (int pass = 0; pass < 4; pass++)
        for (unsigned i = 0; i < 55; i++)
            r_rng68b9c8[i] -= r_rng68b9c8[(i + 31) % 55];
    r_rngI68baa4 = 0;
    r_rngJ68baa8 = 31;
    r_68cfe8 = 40000.0;
    r_66c6f0 = 1.0;
    r_68d178 = 0;

    Dwords_AssignZeros(r_68cf28, 12);
    DWORD* c = (DWORD*)r_68cf28[0];
    c[1] = 1;
    c[2] = 8;
    c[5] = 3;
    c[6] = 3;
    c[7] = 0;
    c[8] = 50;
    c[9] = 150;
    {
        BYTE z[144] = {0};
        BYTE* t[2] = {z, z + 144};
        Values_Assign((BYTE**)r_68a840, 0, t);
    }

    r_68d188 = 0;
    r_68d158 = 0;
    r_66c68c = 1;
    memset(r_68ced0, 0, sizeof r_68ced0);
    r_68ced0[8] = 2;
    r_68ced0[4] = 1;
    r_68d14c = 0;
    r_68d160 = 3;
    CallThis(FUN_44af80, r_68a980, r_68a980[0], r_68a980[1]);
    r_68d164 = 0;
    CallThis(FUN_44af80, r_68cf60, r_68cf60[0], r_68cf60[1]);
    r_66c6d8 = 0xFFFFFFFF;
    r_66c6dc = 0x7FEFFFFF;
    Ptrs_DeleteAll(r_68cec0, FUN_404290);
    r_68cec0[1] = r_68cec0[0];
    Ptrs_DeleteAll(r_68cc98, FUN_404570);
    r_68cc98[1] = r_68cc98[0];
    Ptrs_DeleteAll(r_68cbe8, FUN_4046b0);
    r_68cbe8[1] = r_68cbe8[0];
    Ptrs_DeleteAll(r_68cc18, FUN_404790);
    CallThis(FUN_44af80, r_68cc18, r_68cc18[0], r_68cc18[1]);
    Ptrs_DeleteAll(r_68ceb0, FUN_404920);
    CallThis(FUN_44af80, r_68ceb0, r_68ceb0[0], r_68ceb0[1]);
    Ptrs_DeleteAll(r_68a970, FUN_404ab0);
    CallThis(FUN_44af80, r_68a970, r_68a970[0], r_68a970[1]);
    CallC(FUN_4734f0);
    while (*(void**)g_68cf6c != g_68cf6c) {
        void* begin;
        CallThis(FUN_4afbf0, &g_68cf6c, &begin);
        Ai_DestroyPresentSilently((void**)begin);
    }
    List12_Clear((void***)&g_68cf6c, 0);

    r_68cf54 = r_68a964 = r_68d0a0 = r_68bb54 = 5;
    r_68a968 = 0;
    r_68a98c = 2;
    r_68d01c = 0;
    r_68cfe4 = 0;
    r_68d0e4 = 0;
    r_68cf34 = 0;
    Dwords_AssignZeros(r_68cca8, 12);
    Dwords_AssignZeros(r_68a830, 12);
    r_66f62a = 1;
    r_68d31c = 0;
    r_68d18c = 0;
    Dwords_AssignZeros(r_68cc88, 8);
    r_68d190 = 0.0;
    r_66c700 = 7;
    r_66c6e0 = 3;
    r_66c6f8 = 300.0;
    r_68d128 = 0.0;
    r_68d130 = 0;
    r_66c60a = 1;
    r_68d134 = 0;
    r_68d138 = 0;
    r_66c710 = 0.51;
    for (BYTE* e = (BYTE*)r_68a958[0]; e != (BYTE*)r_68a958[1]; e += 56) {
        F(int, e, 28) = 0;
        F(int, e, 32) = 0;
    }
}

// ---------------------------------------------------------------------------
// reading a player type of the world settings

KING_VAR(double, g_round64b658, 64b658)      // 0.001

// the n-th element of the list, typed (left empty on a type error)
static void Pt_NthTyped(StrRef* d, StrRef* h, int n, int type, const char* typeName)
{
    StrRef t;
    List_Nth(&t, h, n);
    d->ptr = 0;
    d->len = 0;
    d->flags = 0;
    Ref_AssignTyped(d, &t, type, typeName);
    Str_Release(&t);
}

// the number after the key (int, through the value object)
static BYTE* Pt_KeyNumber(StrRef* num, StrRef* h)
{
    StrRef t, m;
    Str_Member32Checked(&t, h);
    Str_Member20(&m, &t);
    NumberHandle_From((DWORD*)num, 0, (const DWORD*)&m);
    CallThis(FUN_4a36c0, &m);
    CallThis(FUN_4a36c0, &t);
    return CallThis<BYTE*>(FUN_4aa430, num);
}

// the second element of the list into a string slot of the 68a840 table
static void Pt_AssignSecond(int offset, StrRef* h)
{
    StrRef t;
    List_Nth(&t, h, 2);
    Str_AssignPair((StrRef*)(*(BYTE**)&g_init68a840 + offset), 0, &t);
    CallThis(FUN_4a36c0, &t);
}

// [FUN_407570] reads a player type: (live number [truck name] [owner]), (aitruck number type),
// (police/mafia number name), (ghost/dealer number)
extern "C" void __cdecl Ai_ReadPlayerType(int, StrRef h)
{
    int* settings = (int*)g_68cf28;
    if (Pair_HeadIs(&h, "live")) {
        StrRef n, s, o;
        Pt_NthTyped(&n, &h, 1, 6, "Number");
        CheckHandle(&n);
        settings[1] = CallThis<int>(FUN_4b0ea0, n.ptr);
        Pt_NthTyped(&s, &h, 2, 3, "String");
        if (s.flags || s.ptr) {
            CheckHandle(&s);
            strncpy(g_truckName66c60c, F(const char*, s.ptr, 12), 128);
        }
        Pt_NthTyped(&o, &h, 3, 6, "Number");
        if (o.flags || o.ptr) {
            CheckHandle(&o);
            g_humanOwner68d148 = CallThis<int>(FUN_4b0ea0, o.ptr);
        }
        CallThis(FUN_4a36c0, &o);
        CallThis(FUN_4a36c0, &s);
        CallThis(FUN_4a36c0, &n);
    }
    if (Pair_HeadIs(&h, "aitruck")) {
        StrRef t, m, n;
        Str_Member32Checked(&t, &h);
        Str_Member20(&m, &t);
        n.ptr = 0;
        n.len = 0;
        n.flags = 0;
        Ref_AssignTyped(&n, &m, 6, "Number");
        Str_Release(&m);
        Str_Release(&t);
        CheckHandle(&n);
        settings[2] = CallThis<int>(FUN_4b0ea0, n.ptr);
        Pt_AssignSecond(24, &h);
        CallThis(FUN_4a36c0, &n);
    }
    if (Pair_HeadIs(&h, "police")) {
        StrRef num;
        settings[5] = CallThis<int>(FUN_4b0ea0, Pt_KeyNumber(&num, &h));
        Pt_AssignSecond(60, &h);
        CallThis(FUN_4a36c0, &num);
    }
    if (Pair_HeadIs(&h, "mafia")) {
        StrRef num;
        settings[6] = CallThis<int>(FUN_4b0ea0, Pt_KeyNumber(&num, &h));
        Pt_AssignSecond(72, &h);
        CallThis(FUN_4a36c0, &num);
    }
    if (Pair_HeadIs(&h, "ghost")) {
        StrRef num;
        settings[8] = Ftol(CallThis<double>(FUN_4b0e90, Pt_KeyNumber(&num, &h)) + g_round64b658);
        CallThis(FUN_4a36c0, &num);
    }
    if (Pair_HeadIs(&h, "dealer")) {
        StrRef num;
        settings[9] = Ftol(CallThis<double>(FUN_4b0e90, Pt_KeyNumber(&num, &h)) + g_round64b658);
        CallThis(FUN_4a36c0, &num);
    }
    CallThis(FUN_4a36c0, &h);
}

// ---------------------------------------------------------------------------
// advancePosition

extern "C" void FUN_40b100(); KING_ALIAS("_FUN_40b100", "_Ai_OnNodePass")    // (playerid, node, 24-byte struct by value)
extern "C" void FUN_43ab10(); KING_ALIAS("_FUN_43ab10", "_Ai_FindPositionHops")    // the hop index of a position (hops, PositionId32 by value)
extern "C" PositionId* __cdecl Road_StartOf(PositionId* out, int road);
extern "C" PositionId* __cdecl Road_EndOf(PositionId* out, int road);
extern "C" void __cdecl Road_DumpHops(const HopVec* hops, const char* path);
extern "C" BYTE* __cdecl Find12ByField0(BYTE* first, BYTE* last, int key);
extern "C" PositionId* __cdecl Road_Advance(PositionId* out, PositionId pos, double* distance, int);
KING_VAR(double, g_tMin64b688, 64b688)        // -0.0002
KING_VAR(double, g_tMax64b680, 64b680)        // 1.0002
KING_VAR(AsmLabel, s_failHops66e800, 66e800)
KING_VAR(AsmLabel, s_hops1_66e7e4, 66e7e4)
KING_VAR(AsmLabel, s_hops4_66e7c8, 66e7c8)

static const char s_advancePosition[] =
    "advancePosition(PlayerId playerid, AI_Player * player, PositionId pos, double & offset, bool sendmessage)";

static PositionId Adv_Step(PositionId pos, double* step)
{
    PositionId r;
    return *Road_Advance(&r, pos, step, 0);
}

// the PositionId checks of advancePosition
static void Adv_Check(const PositionId& p, int line)
{
    int n = (int)(g_roadEdgesEnd - g_roadEdges);
    bool roadOk = p.road >= 0 && p.road < n;
    PositionId c = p;
    if ((!roadOk && !NodeId_Valid(&c.node, 0)) || p.t < g_tMin64b688 || p.t > g_tMax64b680 || p.segment < 0
        || p.object < 0) {
        sprintf(g_positionText76, "(%d %g %d %d %x)", p.road, p.t, p.segment, p.object, p.m_24);
        AiError(33, 100, s_mainFile, s_mainId, line, "Invalid id", "PositionId", g_positionText76);
    }
    if (roadOk) {
        RoadEdge* e = g_roadEdges[p.road];
        if (!(p.object < e->objectsEnd - e->objects && p.segment < e->objects[p.object]->SegmentCount()))
            AiError(33, 100, s_mainFile, s_mainId, line, "Invalid Position Id", &g_aiEmpty, &g_aiEmpty);
    }
}

static PositionId* Adv_Leave(PositionId* out, const PositionId& pos, const char* name)
{
    *out = pos;
    Buf_Append(&g_trace, 0, g_tagMpl, 8);
    Buf_AppendString(&g_trace, 0, name);
    return out;
}

// advancePosition: moves a position by *offset along the hops, passing from road to road at the hop nodes
// (telling the player when send); lines: the error lines (entry, hops 1, hops 4, sent, exit)
static PositionId* Adv_Run(PositionId* out, HopVec* hops, int playerid, BYTE* player, PositionId pos, double* offset,
                           bool sendmessage, const char* name, const int* lines)
{
    Trace_EnterBuffered(name);
    {
        PositionId c = pos;
        if (!Position_Valid(&c, 0)) {
            sprintf(g_positionText76, "(%d %g %d %d %x)", pos.road, pos.t, pos.segment, pos.object, pos.m_24);
            AiError(33, 100, s_mainFile, s_mainId, lines[0], "Invalid id", "PositionId", g_positionText76);
        }
    }
    if (x87_eq(*offset, 0.0))
        return Adv_Leave(out, pos, name);
    BYTE** hv = (BYTE**)hops;
    if ((hv[1] - hv[0]) / 12 == 1) {
        out->road = -1;
        out->node = F(int, hv[1], -12);
        out->t = 0.0;
        out->segment = 0;
        out->object = 0;
        out->m_24 = 0;
        out->m_28 = -1;
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, name);
        return out;
    }
    {
        DVec3 pt;
        Road_PointOf(&pt, pos);
    }
    double dir = *offset;
    pos = Adv_Step(pos, offset);
    int node = pos.node;
    while (node != -1) {
        BYTE* begin = hv[0];
        BYTE* end = hv[1];
        int size = (int)(end - begin) / 12;
        int idx = pos.m_28;
        BYTE* r;
        // (sic: the hint is only tried past the end)
        if (!(size >= idx) && idx >= 0 && (r = Find12ByField0(begin + idx * 12, end, node)) != end)
            ;
        else
            r = Find12ByField0(begin, end, node);
        begin = hv[0];
        pos.m_28 = (int)(r - begin) / 12;
        if (r == hv[1])
            break;
        if (r == begin) {
            BYTE* q = r + 12;
            while (q != hv[1] && F(int, q, 0) != node)
                q += 12;
            r = q == hv[1] ? begin : q;
        }
        pos.m_28 = (int)(r - begin) / 12;
        RoadEdge* e;
        if (dir > 0.0) {
            e = F(RoadEdge*, r, 4);
            if (!e)
                break;
            if (e == g_roadEdges[pos.road]) {
                Road_DumpHops(hops, s_failHops66e800);
                *offset = 0.0;
                return Adv_Leave(out, pos, name);
            }
            if (node == F(int, e->from, 16))
                Road_StartOf(&pos, e->id);
            else if (node == F(int, e->to, 16)) {
                Road_EndOf(&pos, e->id);
                pos.m_24 |= 1;
            } else {
                Road_DumpHops(hops, s_failHops66e800);
                AiError(0, 100, s_mainFile, s_mainId, lines[1], s_hops1_66e7e4, &g_aiEmpty, &g_aiEmpty);
            }
        } else {
            if (r == begin)
                break;
            r -= 12;
            pos.m_28 = (int)(r - begin) / 12;
            e = F(RoadEdge*, r, 4);
            if (!e)
                break;
            if (e == g_roadEdges[pos.road]) {
                Road_DumpHops(hops, s_failHops66e800);
                *offset = 0.0;
                return Adv_Leave(out, pos, name);
            }
            if (node == F(int, e->from, 16)) {
                Road_StartOf(&pos, e->id);
                pos.m_24 |= 1;
            } else if (node == F(int, e->to, 16))
                Road_EndOf(&pos, e->id);
            else {
                Road_DumpHops(hops, s_failHops66e800);
                AiError(0, 100, s_mainFile, s_mainId, lines[2], s_hops4_66e7c8, &g_aiEmpty, &g_aiEmpty);
            }
        }
        if (sendmessage) {
            CallC(FUN_40b100, playerid, pos.node, 0, 0, 0, 0, 0, 0);
            Adv_Check(pos, lines[3]);
            Player_SetPosition(player, 0, &pos);
        }
        pos = Adv_Step(pos, offset);
        node = pos.node;
    }
    Adv_Check(pos, lines[4]);
    if (pos.m_28 == -1 && hops) {
        DWORD* d = (DWORD*)&pos;
        pos.m_28 = CallC<int>(FUN_43ab10, hops, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
    }
    *out = pos;
    Trace_Leave(name);
    return out;
}

// [FUN_43b570] advancePosition (player): along the player's hops, telling it when sendmessage
extern "C" PositionId* __cdecl Ai_AdvancePosition(PositionId* out, int playerid, BYTE* player, PositionId pos,
                                                   double* offset, bool sendmessage)
{
    static const int lines[] = {15722, 15786, 15829, 15840, 15853};
    return Adv_Run(out, (HopVec*)(player + 16), playerid, player, pos, offset, sendmessage, s_advancePosition, lines);
}

static const char s_advancePositionHops[] = "advancePosition(vector<Hop> & hops, PositionId pos, double & offset)";

// [FUN_43ade0] advancePosition (hops)
extern "C" PositionId* __cdecl Ai_AdvancePositionHops(PositionId* out, HopVec* hops, PositionId pos, double* offset)
{
    static const int lines[] = {15594, 15658, 15701, 0, 15708};
    return Adv_Run(out, hops, 0, 0, pos, offset, false, s_advancePositionHops, lines);
}

// ---------------------------------------------------------------------------
// reading a 100-byte record of the script

extern "C" void FUN_40b4a0(); KING_ALIAS("_FUN_40b4a0", "_Ai_MerchandiseFlags")    // a value of a list (list by value)
extern "C" BYTE* __fastcall Rec40c220_Copy(BYTE* self, void*, const BYTE* s);
extern "C" void FUN_40c1a0(); KING_ALIAS("_FUN_40c1a0", "@ValueBuf_Destroy@8")

// the number value of the list head
static double Head_Number(StrRef* l)
{
    StrRef m;
    Str_Member20(&m, l);
    double v = CallThis<double>(FUN_4b0e90, m.ptr);
    CallThis(FUN_4a36c0, &m);
    return v;
}

// [FUN_40b920] reads a record: (name - number number (number number [value [number]]) number int bytes)
extern "C" BYTE* __cdecl Ai_ReadRecord40b920(BYTE* out, void* reader, StrRef h)
{
    BYTE r[100];
    memset(r, 0, sizeof r);
    char alloc;
    F(double, r, 80) = 1.0;
    CallThis(FUN_487730, r + 88, &alloc);
    {
        StrRef m;
        Str_Member20(&m, &h);
        Ref_Assign((StrRef*)r, &m);
        Str_Release(&m);
    }
    List_Next(&h);
    List_Next(&h);
    F(double, r, 16) = Head_Number(&h);
    List_Next(&h);
    F(double, r, 24) = Head_Number(&h);
    List_Next(&h);
    StrRef l;
    Str_Member20(&l, &h);
    List_Next(&l);
    F(double, r, 40) = Head_Number(&l);
    List_Next(&l);
    F(double, r, 32) = Head_Number(&l);
    List_Next(&l);
    if ((BYTE)l.flags == 2) {
        StrRef m;
        Str_Member20(&m, &l);
        F(int, r, 48) = CallC<int>(FUN_40b4a0, m.ptr, m.len, m.flags);
        List_Next(&l);
        if ((BYTE)l.flags == 2)
            F(double, r, 80) = Head_Number(&l);
    }
    List_Next(&h);
    F(double, r, 64) = Head_Number(&h);
    List_Next(&h);
    {
        StrRef m;
        Str_Member20(&m, &h);
        F(int, r, 72) = CallThis<int>(FUN_4b0ea0, m.ptr);
        CallThis(FUN_4a36c0, &m);
    }
    List_Next(&h);
    {
        StrRef m;
        Str_Member20(&m, &h);
        PodVec_Assign((BYTE**)(r + 88), 0, (BYTE* const*)(m.ptr + 12));
        CallThis(FUN_4a36c0, &m);
    }
    Rec40c220_Copy(out, 0, r);
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_40c1a0, r);
    CallThis(FUN_4a36c0, &h);
    return out;
}

// ---------------------------------------------------------------------------
// getOrderInfo (town index)

static const char s_getOrderInfoIdx[] = "getOrderInfo(PlayerId player, TownId town, int _orderid, OrderInfo & oi)";

static int OrderInfo_Leave(int r)
{
    Buf_Append(&g_trace, 0, g_tagMpl, 8);
    Buf_AppendString(&g_trace, 0, s_getOrderInfoIdx);
    return r;
}

// [FUN_441120] getOrderInfo: fills oi with a town's order as the player would take it (cargo, towns, times,
// money); -1 no such order, -2 none for the player, -27 an invalid order
extern "C" int __cdecl Ai_GetOrderInfoIdx(BYTE* player, int town, int order, BYTE* oi)
{
    Trace_EnterBuffered(s_getOrderInfoIdx);
    F(BYTE, oi, 184) = 0;
    BYTE* t = (BYTE*)g_towns[town];
    if ((unsigned)((F(BYTE*, t, 32) - F(BYTE*, t, 28)) >> 4) <= (unsigned)order)
        return OrderInfo_Leave(-1);
    BYTE* entry = F(BYTE*, t, 28) + order * 16;
    BYTE* rec = 0;
    BYTE* data = player + 8;
    if (data) {
        BYTE* v = F(BYTE*, data, 508);
        if (v) {
            int kind = F(int, ((BYTE**)g_vehicles)[F(int, v, 4)], 32);
            if (kind == 2 || kind == 3)
                rec = F(BYTE*, entry, 8);
        }
        if (!(BYTE)Player_HasTrader(data))
            rec = F(BYTE*, entry, 12);
    }
    if (!rec)
        return OrderInfo_Leave(-2);
    if (F(int, rec, 0) == -1) {
        AiError(0, 200, s_mainFile, s_mainId, 17078, "Internal error 05712", &g_aiEmpty, &g_aiEmpty);
        return OrderInfo_Leave(-27);
    }
    F(int, oi, 0) = F(int, rec, 0);
    int dest = F(int, rec, 12);
    BYTE* tmpl = g_68ccb8 + F(int, rec, 0) * 104;
    PodVec_Assign((BYTE**)(oi + 52), 0, (BYTE* const*)(tmpl + 88));
    if (F(int, rec, 8) != -1)
        PodVec_Assign((BYTE**)(oi + 52), 0, (BYTE* const*)(g_68ccb8 + F(int, rec, 8) * 104 + 88));
    CharVec_Erase(oi + 64, 0, F(BYTE*, oi, 64), F(BYTE*, oi, 68));
    PodVec_Assign((BYTE**)(oi + 76), 0, (BYTE* const*)((BYTE*)g_towns[town] + 12));
    PodVec_Assign((BYTE**)(oi + 88), 0, (BYTE* const*)((BYTE*)g_towns[dest] + 12));
    F(BYTE*, oi, 100) = F(BYTE*, oi, 52);
    F(int, oi, 48) = dest;
    F(BYTE*, oi, 112) = F(BYTE*, oi, 88);
    F(int, oi, 120) = 0;
    F(int, oi, 128) = 0;
    F(int, oi, 104) = 0;
    F(BYTE*, oi, 108) = F(BYTE*, oi, 76);
    F(int, oi, 124) = 0;
    F(int, oi, 132) = 0;
    F(int, oi, 136) = F(int, rec, 24);
    F(int, oi, 140) = F(int, rec, 28);
    F(int, oi, 144) = 0;
    F(int, oi, 152) = 0;
    F(int, oi, 148) = 0;
    F(int, oi, 156) = 0;
    F(int, oi, 160) = F(int, tmpl, 64);
    F(int, oi, 164) = F(int, tmpl, 68);
    F(double, oi, 168) = F(double, rec, 16) - CallC<double>(FUN_401000);
    if (F(double, oi, 168) < 0.0)
        F(double, oi, 168) = 0.0;
    if (player != g_playersHead) {
        double a, b, q;
        Costs_Balance(player, &a, &b, &q, tmpl);
        F(double, oi, 144) = a;
        F(double, oi, 152) = b;
        BYTE o[144];
        Order_Construct(o, 0);
        DWORD pos[8] = {(DWORD)-1, F(DWORD, entry, 0), 0, 0, 0, 0, 0, (DWORD)-1};
        DWORD qd[2];
        memcpy(qd, &q, 8);
        CallC(FUN_4715f0, o, rec, town, pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7], qd[0], qd[1]);
        Order_SetPrice(player, (int*)o, 0, F(BYTE*, player, 520) != 0);
        if (F(BYTE*, player, 520))
            F(double, oi, 136) = g_bonus66f898 * F(double, oi, 136);
        double pay = Order_Pay(o, 0.0, 1);
        F(double, oi, 120) = pay;
        F(double, oi, 128) = pay;
        BYTE* c = F(BYTE*, player, 516);
        if (c && F(int, F(BYTE*, c, 16), 160) && F(int, rec, 4) != -1) {
            PodVec_Assign((BYTE**)(oi + 64), 0, (BYTE* const*)(g_68ccb8 + F(int, rec, 4) * 104 + 88));
            F(BYTE*, oi, 104) = F(BYTE*, oi, 64);
        }
        Order_Destroy(o);
    }
    F(BYTE, oi, 184) = 1;
    return OrderInfo_Leave(0);
}

// ---------------------------------------------------------------------------
// findClosest(dvector3 const& v, double& md, vector<Hop>& hops, ...): the
// closest point to v along the hops between two positions.

extern "C" void FUN_450150(); KING_ALIAS("_FUN_450150", "@RoadObjs_FindBetween@60")    // objects::findClosest over a range of objects (thiscall, 13 dwords)
KING_VAR(AsmLabel, s_findClosestHops, 66e914)

static void FC_Result(PositionId* out, RoadEdge* e, double d, int seg, int obj, int hop)
{
    PositionId p;
    p.road = e->id;
    p.node = -1;
    p.t = d;
    p.segment = seg;
    p.object = obj;
    p.m_24 = 0;
    p.m_28 = -1;
    *out = p;
    out->m_28 = hop;
}

static int FC_Range(RoadObject*** objs, const DVec3* v, double* md, double* d, int* seg, int* obj,
                    double t0, int seg0, int obj0, double t1, int seg1, int obj1)
{
    return CallThis<int>(FUN_450150, objs, v, md, d, seg, obj, t0, seg0, obj0, t1, seg1, obj1);
}

// objects from (t0, seg0, obj0) to (t1, seg1, obj1) with obj0 <= obj1; the
// object of the last hit goes to *best
static int FC_Walk(RoadObject** objs, const DVec3* v, double* md, double* d, int* seg, int* best,
                   double t0, int seg0, int obj0, double t1, int seg1, int obj1)
{
    int hit = 0;
    if (obj0 == obj1) {
        if (CallVirt<int>(objs[obj0], 8, v, md, d, seg, t0, seg0, t1, seg1)) {
            hit = 1;
            *best = obj0;
        }
        return hit;
    }
    int n = objs[obj0]->SegmentCount() - 1;
    if (CallVirt<int>(objs[obj0], 8, v, md, d, seg, t0, seg0, 1.0, n)) {
        hit = 1;
        *best = obj0;
    }
    for (int k = obj0 + 1; k < obj1; k++)
        if (CallVirt<int>(objs[k], 12, v, md, d, seg)) {
            hit = 1;
            *best = k;
        }
    if (CallVirt<int>(objs[obj1], 8, v, md, d, seg, 0.0, 0, t1, seg1)) {
        hit = 1;
        *best = obj1;
    }
    return hit;
}

// whether (t1, seg1, obj1) comes before (t0, seg0, obj0)
static bool FC_Before(double t1, int seg1, int obj1, double t0, int seg0, int obj0)
{
    if (obj1 != obj0)
        return obj1 < obj0;
    if (seg1 != seg0)
        return seg1 < seg0;
    return t1 < t0;
}

// [FUN_43c2d0]
extern "C" int __cdecl Ai_FindClosestHops(const DVec3* v, double* md, HopVec* hops, PositionId* out,
                                          PositionId from, PositionId to)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_findClosestHops);
    if (from.road == to.road && from.node == to.node && from.t == to.t && from.segment == to.segment
        && from.object == to.object && from.m_24 == to.m_24) {
        *out = from;
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_findClosestHops);
        return 0;
    }
    if (from.m_28 > to.m_28) {
        int r = Ai_FindClosestHops(v, md, hops, out, to, from);
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_findClosestHops);
        return r;
    }
    double d = 0;
    int seg = 0, best = 0;
    if (from.m_28 == to.m_28) {
        RoadEdge* e = hops->begin[from.m_28].edge;
        if (!e) {
            *out = from;
            Buf_Append(&g_trace, 0, g_tagMpl, 8);
            Buf_AppendString(&g_trace, 0, s_findClosestHops);
            return 0;
        }
        RoadObject*** objs = &e->objects;
        if (FC_Before(to.t, to.segment, to.object, from.t, from.segment, from.object))
            FC_Range(objs, v, md, &d, &seg, &best, to.t, to.segment, to.object, from.t, from.segment, from.object);
        else if (from.object == to.object) {
            if (CallVirt<int>(e->objects[from.object], 8, v, md, &d, &seg, from.t, from.segment, to.t, to.segment))
                best = from.object;
        } else
            FC_Walk(e->objects, v, md, &d, &seg, &best, from.t, from.segment, from.object, to.t, to.segment, to.object);
        FC_Result(out, hops->begin[from.m_28].edge, d, seg, best, from.m_28);
    } else {
        RoadEdge* e = hops->begin[from.m_28].edge;
        if (e) {
            PositionId l;
            if (hops->begin[from.m_28].dir & 1)
                l = *Road_EdgeStart(&l, e);
            else
                l = *Road_EdgeEnd(&l, e);
            int hit;
            if (FC_Before(l.t, l.segment, l.object, from.t, from.segment, from.object))
                hit = FC_Range(&e->objects, v, md, &d, &seg, &best, l.t, l.segment, l.object, from.t, from.segment, from.object);
            else
                hit = FC_Walk(e->objects, v, md, &d, &seg, &best, from.t, from.segment, from.object, l.t, l.segment, l.object);
            if (hit)
                FC_Result(out, hops->begin[from.m_28].edge, d, seg, best, from.m_28);
        }
        for (int h = from.m_28 + 1; h < to.m_28; h++) {
            RoadEdge* he = hops->begin[h].edge;
            int hit = 0;
            for (unsigned k = 0; k < (unsigned)(he->objectsEnd - he->objects); k++)
                if (CallVirt<int>(he->objects[k], 12, v, md, &d, &seg)) {
                    hit = 1;
                    best = k;
                }
            if (hit)
                FC_Result(out, hops->begin[h].edge, d, seg, best, h);
        }
        e = hops->begin[to.m_28].edge;
        if (e) {
            PositionId l;
            if (hops->begin[to.m_28].dir & 1)
                l = *Road_EdgeEnd(&l, e);
            else
                l = *Road_EdgeStart(&l, e);
            if (FC_Range(&e->objects, v, md, &d, &seg, &best, to.t, to.segment, to.object, l.t, l.segment, l.object))
                FC_Result(out, hops->begin[to.m_28].edge, d, seg, best, to.m_28);
        }
    }
    if (hops->begin[out->m_28].dir & 1)
        out->m_24 |= 1;
    else
        out->m_24 &= ~1;
    Trace_Tag((const DWORD*)g_tagMpl);
    Buf_AppendString(&g_trace, 0, s_findClosestHops);
    return 0;
}

// ---------------------------------------------------------------------------
// loading evehicles.sch and etrashboxes.sch: records of the tech file

KING_VAR(int, g_carTypeCount6ce32c, 6ce32c)
KING_VAR(AsmLabel, g_carTypes6978c8, 6978c8)     // 2416 bytes per type, the name at +2246
KING_VAR(int, g_carModelCount6ce34c, 6ce34c)
KING_VAR(AsmLabel, g_carModels6c6b68, 6c6b68)    // 876 bytes per model, the name at +0

struct TechList {
    void**      vec;                // the records
    DWORD*      (__cdecl* makePort)(DWORD* out, int unused, int a, const int* b);
    BYTE*       (*read)(StrRef* pair);
    int*        count;
    const char* names;
    int         stride;
    int         field;              // the record's index field
    int         lines[3];           // cannot open (0: skip silently), format, no such
    const char* noSuch;
};

// reads a tech file's list of records; each one's (- - name) is looked up by
// name and replaced by its index, which is also stored in the record
static void Tech_Load(const TechList* t, const char* file)
{
    Vec4_Erase2(t->vec, 0, (BYTE*)t->vec[0], (BYTE*)t->vec[1]);
    int one = 1;
    DWORD tmp[3];
    DWORD* h = t->makePort(tmp, 0, (int)file, &one);
    StrRef port = {0, 0, 0};
    BYTE* o = (BYTE*)h[0];
    if (o) {
        if (CallVirt<int>(o, 0) == 11) {
            port.ptr = o;
            port.flags = h[2];
            F(int, o, 4)++;
        } else
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(o, 4), s_port);
    }
    CallThis(FUN_4a36c0, tmp);
    CheckHandle(&port);
    if (CallVirt<int>(port.ptr, 40)) {
        if (!t->lines[0]) {
            CallThis(FUN_4a36c0, &port);
            return;
        }
        AiError(23, 100, s_mainFile, s_mainId, t->lines[0], "Cannot open", file, &g_aiEmpty);
    }
    StrRef value;
    {
        StrRef copy;
        Str_CopyConstruct(&copy, 0, &port);
        CallC(FUN_4b4c20, &value, g_reader68bb58, copy.ptr, copy.len, copy.flags);
    }
    StrRef l;
    l.CopyFrom(&value);
    while ((BYTE)l.flags == 2) {
        StrRef m;
        Str_Member20(&m, &l);
        if ((BYTE)m.flags != 2)
            AiError(45, 1, s_mainFile, s_mainId, t->lines[1], "Incorrect resource format", &g_aiEmpty, &g_aiEmpty);
        StrRef a;
        PairHandle_From((DWORD*)&a, 0, (const DWORD*)&m);
        BYTE* rec = t->read(&a);
        StrRef t1, t2, t3, name;
        Str_Member32(&t1, &m);
        Str_Member32(&t2, &t1);
        Str_Member20(&t3, &t2);
        StringHandle_From((DWORD*)&name, 0, (const DWORD*)&t3);
        CallThis(FUN_4a36c0, &t3);
        CallThis(FUN_4a36c0, &t2);
        CallThis(FUN_4a36c0, &t1);
        int i;
        for (i = 0; i < *t->count; i++) {
            const char* s = F(const char*, CallThis<BYTE*>(FUN_4aa430, &name), 12);
            if (strcmp(s, t->names + i * t->stride) == 0) {
                StrRef u1, u2, u3, p, n;
                Str_Member32(&u1, &m);
                Str_Member32(&u2, &u1);
                Str_Member32(&u3, &u2);
                PairHandle_From((DWORD*)&p, 0, (const DWORD*)&u3);
                CallThis(FUN_4a36c0, &u3);
                CallThis(FUN_4a36c0, &u2);
                CallThis(FUN_4a36c0, &u1);
                int zero = 0;
                Number_Make((DWORD*)&n, 0, &zero, &i);
                Str_Assign((StrRef*)(CallThis<BYTE*>(FUN_4aa430, &p) + 20), 0, &n);
                CallThis(FUN_4a36c0, &n);
                F(int, rec, t->field) = i;
                CallThis(FUN_4a36c0, &p);
                break;
            }
        }
        if (i == *t->count)
            AiError(43, 100, s_mainFile, s_mainId, t->lines[2], t->noSuch,
                    F(const char*, CallThis<BYTE*>(FUN_4aa430, &name), 12), &g_aiEmpty);
        CallThis(FUN_4a36c0, &name);
        CallThis(FUN_4a36c0, &m);
        List_Advance(&l);
    }
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_4a36c0, &value);
    CallThis(FUN_4a36c0, &port);
}

static BYTE* Tech_ReadVehicle(StrRef* a) { return Ai_ReadVehicle(*a); }
// [FUN_42eff0] reads a spot of etrashboxes.sch: (- id (ints) int kind name); appended to the spots
extern "C" BYTE* __cdecl Ai_ReadTrashbox(StrRef h)
{
    BYTE* o = (BYTE*)operator new(44);
    if (o) {
        char alloc;
        F(int, o, 0) = -1;
        memset(o + 4, 0, 12);
        CallThis(FUN_487730, o + 16, &alloc);
        F(int, o, 28) = -1;
        F(int, o, 32) = -1;
        F(int, o, 36) = -1;
        F(int, o, 40) = 0;
    }
    StrRef m;
    Pair_Advance(&h);
    Pair_Head(&m, &h);
    Ref_AssignTyped((StrRef*)(o + 4), &m, 9, "Identifier");
    Str_Release(&m);
    Pair_Advance(&h);
    Pair_Head(&m, &h);
    PodVec_Assign((BYTE**)(o + 16), 0, (BYTE* const*)(m.ptr + 12));
    CallThis(FUN_4a36c0, &m);
    Pair_Advance(&h);
    Pair_Head(&m, &h);
    F(int, o, 36) = CallThis<int>(FUN_4b0ea0, m.ptr);
    CallThis(FUN_4a36c0, &m);
    Pair_Advance(&h);
    if (Pair_HeadIs(&h, "parked"))
        F(int, o, 28) = 1;
    else if (Pair_HeadIs(&h, "fuel"))
        F(int, o, 28) = 2;
    else if (Pair_HeadIs(&h, "container"))
        F(int, o, 28) = 3;
    else if (Pair_HeadIs(&h, "roadwork"))
        F(int, o, 28) = 4;
    else {
        Pair_Head(&m, &h);
        AiError(4, 100, s_mainFile, s_mainId, 12279, "Unsupported type", F(const char*, m.ptr, 12), &g_aiEmpty);
        CallThis(FUN_4a36c0, &m);
    }
    Vec4_PushBackB(&g_spots68d0b8, 0, &o);
    CallThis(FUN_4a36c0, &h);
    return o;
}

static BYTE* Tech_ReadTrashbox(StrRef* a) { return Ai_ReadTrashbox(*a); }

// [FUN_42e9f0] reads the vehicles of evehicles.sch; each one's type is looked up among the car types
extern "C" void __cdecl Ai_LoadVehicles()
{
    static const TechList t = {(void**)&g_vehicles, Port_MakeG, Tech_ReadVehicle, &g_carTypeCount6ce32c,
                               (const char*)g_carTypes6978c8 + 2246, 2416, 64, {12217, 12229, 12247},
                               "No such vehicle in tech file:"};
    char file[14];
    memcpy(file, "evehicles.sch", 14);
    Tech_Load(&t, file);
}

// [FUN_42f4c0] reads the trashboxes of etrashboxes.sch (if present); each one's model is looked up among the car models
extern "C" void __cdecl Ai_LoadTrashboxes()
{
    static const TechList t = {(void**)&g_spots68d0b8, Port_MakeC, Tech_ReadTrashbox, &g_carModelCount6ce34c,
                               (const char*)g_carModels6c6b68, 876, 32, {0, 12307, 12325},
                               "No such trashbox in tech file:"};
    char file[16];
    memcpy(file, "etrashboxes.sch", 16);
    Tech_Load(&t, file);
}

// ---------------------------------------------------------------------------
// createNewPlayer(int type, SchemePairPtr descr)

extern "C" void __fastcall Player_SetPosition(void* self, void*, const PositionId* p);  // ai_drivers.cpp
KING_VAR(AsmLabel, s_createNewPlayerDescr, 66d050)

// the n-th element of a list, of the type (an error otherwise)
static void Descr_Nth(StrRef* out, const StrRef* l, int n, int type, const char* name)
{
    StrRef c, t;
    Str_CopyConstruct(&c, 0, l);
    CallC(FUN_4af480, &t, n, c.ptr, c.len, c.flags);
    Handle_FromValue((DWORD*)out, (const DWORD*)&t, type, name);
    CallThis(FUN_4a36c0, &t);
}

static double Descr_Number(StrRef* h) { return CallThis<double>(FUN_4b0e90, CallThis<BYTE*>(FUN_4aa430, h)); }

// [FUN_414b30] a new player of a type from (vehicleName owner x y z): its vehicle, position, name and place
extern "C" BYTE* __cdecl Ai_CreatePlayerDescr(int type, StrRef h)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_createNewPlayerDescr);
    BYTE* p = Players_AppendNamed(type);
    F(double, p, 128) = g_newPlayer68cfe8;
    StrRef name, owner, z, x, y;
    Descr_Nth(&name, &h, 0, 3, s_string);
    Descr_Nth(&owner, &h, 1, 6, s_number);
    Descr_Nth(&z, &h, 2, 6, s_number);
    Descr_Nth(&x, &h, 3, 6, s_number);
    Descr_Nth(&y, &h, 4, 6, s_number);
    BYTE* o = (BYTE*)operator new(48);
    if (o) {
        CheckHandle(&owner);
        const char* s = F(const char*, CallThis<BYTE*>(FUN_4aa430, &name), 12);
        int r;
        o = Rec415100_Construct(o, 0, *Vehicle_IndexByName(&r, s, CallThis<int>(FUN_4b0ea0, owner.ptr)));
    }
    F(BYTE*, p, 508) = o;
    DVec3 v;
    v.z = Descr_Number(&y);
    v.y = Descr_Number(&x);
    v.x = Descr_Number(&z);
    BYTE pos[32];
    Player_SetPosition(p, 0, CallC<PositionId*>(FUN_436900, pos, &v));
    F(int, p, 84) = type;
    if (type == 1)
        Rec_SetText(p, 0, g_humanName68d0a8);
    else {
        CallThis(FUN_474160, p, CallC<int>(FUN_4148a0, p));
        PodVec_Assign((BYTE**)(p + 264), 0, (BYTE* const*)(g_images68a958 + F(int, p, 984) * 56 + 4));
    }
    CallThis(FUN_4a36c0, &y);
    CallThis(FUN_4a36c0, &x);
    CallThis(FUN_4a36c0, &z);
    CallThis(FUN_4a36c0, &owner);
    CallThis(FUN_4a36c0, &name);
    Buf_Append(&g_trace, 0, g_tagMpl, 8);
    Buf_AppendString(&g_trace, 0, s_createNewPlayerDescr);
    CallThis(FUN_4a36c0, &h);
    return p;
}

// ---------------------------------------------------------------------------
// findPlayerPosition(vector<Hop>& hops, PositionId approxPosition, dvector3 const& v)

KING_VAR(AsmLabel, s_findPlayerPosition, 66e9c4)

// the position on the hops closest to v: the approximate position's road is searched first, any hop closer
// than 1.5 times (plus 10) that distance wins; at the last node (of the hops, or of the player p) when within 0.1
static void PlayerPosition_Find(PositionId* res, HopVec* hops, PositionId approx, const DVec3* v, BYTE* p, int line)
{
    double md = 1.7976931348623157e308, d = 0;
    int seg = 0, best = 0;
    PositionId r;
    r.road = -1;
    r.node = -1;
    r.t = 0;
    r.segment = 0;
    r.object = 0;
    r.m_24 = 0;
    r.m_28 = -1;
    if (approx.road != -1) {
        RoadEdge* e = g_roadEdges[approx.road];
        int n = hops->end - hops->begin;
        Hop* h;
        if (approx.m_28 < n && approx.m_28 >= 0) {
            h = (Hop*)Find12ByField4((BYTE*)(hops->begin + approx.m_28), (BYTE*)hops->end, (int)e);
            if (h == hops->end)
                h = (Hop*)Find12ByField4((BYTE*)hops->begin, (BYTE*)hops->end, (int)e);
        } else
            h = (Hop*)Find12ByField4((BYTE*)hops->begin, (BYTE*)hops->end, (int)e);
        if (h != hops->end) {
            RoadEdge* re = g_roadEdges[approx.road];
            for (unsigned k = 0; k < (unsigned)(re->objectsEnd - re->objects); k++)
                if (CallVirt<int>(re->objects[k], 12, v, &md, &d, &seg))
                    best = k;
            r.road = approx.road;
            r.node = -1;
            r.t = d;
            r.segment = seg;
            r.object = best;
            r.m_24 = 0;
            r.m_28 = -1;
            md = md * 1.5 + 10.0;
            r.m_28 = h - hops->begin;
        }
    }
    for (;;) {
        int found = 0;
        for (Hop* it = hops->begin; it != hops->end; it++) {
            RoadEdge* e = it->edge;
            if (!e)
                continue;
            int hit = 0;
            for (unsigned k = 0; k < (unsigned)(e->objectsEnd - e->objects); k++)
                if (CallVirt<int>(e->objects[k], 12, v, &md, &d, &seg)) {
                    hit = 1;
                    best = k;
                }
            if (hit) {
                r.road = e->id;
                r.node = -1;
                r.t = d;
                r.segment = seg;
                r.object = best;
                r.m_24 = 0;
                r.m_28 = it - hops->begin;
                if (g_roadNodes[it->node] == e->to)
                    r.m_24 |= 1;
                else
                    r.m_24 &= ~1;
                found = 1;
            }
        }
        if (found || hops->begin == hops->end)
            break;
        md = 1.7976931348623157e308;
        d = 0;
        seg = 0;
        best = 0;
    }
    if (r.road == -1) {
        BYTE tmp[32];
        r = *CallC<PositionId*>(FUN_436900, tmp, v);
    }
    if (hops->begin != hops->end) {
        int last = p ? *Player_NodeOf(&last, p) : hops->end[-1].node;
        const DVec3* n = (const DVec3*)((BYTE*)g_roadNodes[last] + 48);
        DVec3 pt;
        const DVec3* q = Road_PointOf(&pt, r);
        double dx = q->x - n->x, dy = q->y - n->y, dz = q->z - n->z;
        if (0.1 > x87_sqrt((dx * dx + dy * dy) + dz * dz))
            r.node = p ? *Player_NodeOf(&last, p) : hops->end[-1].node;
    }
    PositionId chk = r;
    if (!PositionValid(&chk)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", r.road, r.t, r.segment, r.object, r.m_24);
        AiError(33, 100, s_mainFile, s_mainId, line, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    *res = r;
}

// [FUN_43d1f0]
extern "C" PositionId* __cdecl Ai_FindPlayerPosition(PositionId* out, HopVec* hops, PositionId approx, const DVec3* v)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_findPlayerPosition);
    if (hops->end - hops->begin == 1) {
        PositionInit(out, hops->end[-1].node);
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_findPlayerPosition);
        return out;
    }
    PlayerPosition_Find(out, hops, approx, v, 0, 16145);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_findPlayerPosition);
    return out;
}

KING_VAR(AsmLabel, s_setPlayerPosition, 66eaa8)
extern "C" void __fastcall Player_SetPosition(void* self, void*, const PositionId* p);  // ai_drivers.cpp

// [FUN_43dc60] setPlayerPosition: the player's position on its hops closest to v
extern "C" void __cdecl Ai_SetPlayerPosition(BYTE* p, const DVec3* v)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_setPlayerPosition);
    HopVec* hops = (HopVec*)(p + 24);
    PositionId r;
    if (hops->end - hops->begin == 1) {
        PositionInit(&r, hops->end[-1].node);
        Player_SetPosition(p + 8, 0, &r);
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_setPlayerPosition);
        return;
    }
    PlayerPosition_Find(&r, hops, F(PositionId, p, 40), v, p, 16290);
    Player_SetPosition(p + 8, 0, &r);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_setPlayerPosition);
}

// ---------------------------------------------------------------------------
// cleanBeforeLaw(PlayerId player)

extern "C" void FUN_444f80(); KING_ALIAS("_FUN_444f80", "_Ai_CancelIllegalOrders")    // (player)
extern "C" void __cdecl Player_UpdateFlags(BYTE* p);            // ai_drivers.cpp
extern "C" void __fastcall Rec4725b0_Init(BYTE* self, void*);   // ai_drivers.cpp
KING_VAR(AsmLabel, s_cleanBeforeLaw, 66dfc8)
typedef char FineText[256];
KING_VAR(FineText, g_fineText68a990, 68a990)
typedef char FineLine[256];
KING_VAR(FineLine, g_fineLine77e4e0, 77e4e0)

// one line of the fines message: the text of the id with the amount
static void Fine_AppendLine(const char* id, double amount)
{
    sprintf(g_fineLine77e4e0, Message_Text(id), amount);
    strncat(g_fineLine77e4e0, "\n", 256);
    strcat(g_fineText68a990, g_fineLine77e4e0);
}

// one fine: the banner (event) of the human player, or a line of the message
static void Fine_Add(int count, double* total, double amount, int event, const char* id)
{
    *total += amount;
    if (count == 1) {
        AI_AddToBanner(*total);
        eventBanner(event, 0);
    } else
        Fine_AppendLine(id, amount);
}

// [FUN_430a80] cleanBeforeLaw: the player pays its fines (goods, accident, speeding, red light, bribery, stolen
// vehicle), shown as banners or as one message, then its state is cleaned up
extern "C" int __cdecl Ai_CleanBeforeLaw(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_cleanBeforeLaw);
    if (Player_NotValid(p)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 12684, s_invalidId, "PlayerId", g_numberText);
    }
    BYTE* f = p + 200;
    int count = AI_CountSet(f);
    if (!count) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_cleanBeforeLaw);
        return 0;
    }
    g_fineText68a990[0] = 0;
    double total = 0;
    if (*f) {
        double x = Player_OrdersValue(p);
        total = x;
        if (count == 1) {
            AI_AddToBanner(total);
            eventBanner(9, 0);
        } else
            Fine_AppendLine("FINE_IM", x);
    }
    if (F(double, p, 208) > 0) {
        double x = F(double, f, 8);
        total += x;
        if (count == 1) {
            AI_AddToBanner(total);
            Fine_ViolationText(f, g_fineText68a990);
            eventBanner(14, g_fineText68a990);
        } else
            Fine_AppendLine("FINE_ACCIDENT", x);
    }
    if (F(double, p, 240) > 0)
        Fine_Add(count, &total, F(double, p, 240), 10, "FINE_SPEEDING");
    if (F(double, p, 248) > 0)
        Fine_Add(count, &total, F(double, p, 248), 12, "FINE_RED_LIGHT");
    if (F(double, p, 256) > 0)
        Fine_Add(count, &total, F(double, p, 256), 11, "FINE_BRIBERY");
    if (F(BYTE, p, 264)) {
        double x = 0;
        BYTE* o = F(BYTE*, p, 516);
        if (o && F(int, o, 4) != -1) {
            x = F(double, (BYTE*)g_vehicles[F(int, o, 4)], 48) * 0.8;
            F(DWORD, o, 40) &= ~2u;
            total += x;
        }
        if (count == 1) {
            AI_AddToBanner(total);
            eventBanner(13, 0);
        } else
            Fine_AppendLine("FINE_STOLEN", x);
    }
    if (count > 1) {
        AI_AddToBanner(total);
        eventBanner(15, g_fineText68a990);
    }
    if (total != 0)
        F(double, p, 136) -= total;
    CallC(FUN_444f80, p);
    Rec4725b0_Init(p + 200, 0);
    AI_LightsOff2();
    Player_CheckGoods(p);
    Players_Forget(p);
    Player_CheckTownAA(p);
    Player_UpdateFlags(p);
    Player_CheckBankrupt(p);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_cleanBeforeLaw);
    return 0;
}

KING_VAR(AsmLabel, s_setPlayerPositionLimited, 66e984)

// a player's position step along its route from the position (FUN_43bf10, out, player, PositionId32, double*, int)
static PositionId* Player_StepFrom(PositionId* out, BYTE* p, const PositionId* pos, double* step)
{
    const DWORD* d = (const DWORD*)pos;
    return CallC<PositionId*>(FUN_43bf10, out, p, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], step, 0);
}

// whether the position is on the hop its m_28 names (same edge or same node)
static bool Position_OnHop(HopVec* hops, const PositionId* pos)
{
    Hop* h = hops->begin + pos->m_28;
    return g_roadEdges[pos->road] == h->edge || pos->node == h->node;
}

// [FUN_43cb50] setPlayerPositionLimited: the position closest to v between 10 back and 10 ahead on the route,
// taken only when it is within 3 (or within 50 and not mostly above or below); otherwise setPlayerPosition
extern "C" void __cdecl Ai_SetPlayerPositionLimited(BYTE* p, const DVec3* v)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_setPlayerPositionLimited);
    HopVec* hops = (HopVec*)(p + 24);
    if (hops->begin == hops->end) {
        Ai_SetPlayerPosition(p, v);
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(s_setPlayerPositionLimited);
        return;
    }
    PositionId r = F(PositionId, p, 40);
    double step = 10.0;
    PositionId ahead, back;
    Player_StepFrom(&ahead, p, &r, &step);
    step = -10.0;
    Player_StepFrom(&back, p, &r, &step);
    const PositionId* bad = 0;
    if (!Position_OnHop(hops, &ahead))
        bad = &ahead;
    else if (!Position_OnHop(hops, &back))
        bad = &back;
    if (bad) {
        const DWORD* d = (const DWORD*)bad;
        CallC<int>(FUN_43ab10, hops, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
        Ai_SetPlayerPosition(p, v);
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(s_setPlayerPositionLimited);
        return;
    }
    double md = 1.7976931348623157e308;
    Ai_FindClosestHops(v, &md, hops, &r, back, ahead);
    int node;
    const DVec3* n = (const DVec3*)((BYTE*)g_roadNodes[*Player_NodeOf(&node, p)] + 48);
    DVec3 pt;
    const DVec3* q = Road_PointOf(&pt, r);
    double dx = q->x - n->x, dy = q->y - n->y, dz = q->z - n->z;
    if (0.1 > x87_sqrt((dx * dx + dy * dy) + dz * dz))
        r.node = *Player_NodeOf(&node, p);
    PositionId chk = r;
    if (!PositionValid(&chk)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", r.road, r.t, r.segment, r.object, r.m_24);
        AiError(33, 100, s_mainFile, s_mainId, 16029, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    DVec3 w;
    Road_PointOf(&w, r);
    dx = w.x - v->x;
    dy = w.y - v->y;
    dz = w.z - v->z;
    double dist = x87_sqrt((dy * dy + dz * dz) + dx * dx);
    if (!(dist <= 3.0) && (!(dist <= 50.0) || x87_sqrt(dy * dy + dx * dx) < dz))
        Ai_SetPlayerPosition(p, v);
    else
        Player_SetPosition(p + 8, 0, &r);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_setPlayerPositionLimited);
}

// ---------------------------------------------------------------------------
// addCompetitor()

extern "C" void FUN_425dd0(); KING_ALIAS("_FUN_425dd0", "_Ai_ProvideCompetitionSpecial")    // ()
extern "C" void FUN_425820(); KING_ALIAS("_FUN_425820", "_Ai_TakeOrder")    // (player, goods)
extern "C" void __cdecl Player_ClearOrderLists(BYTE* p);
extern "C" void __cdecl Player_Place(BYTE* p, PositionId pos);           // aiplay.cpp
extern "C" void __cdecl Player_GoTo(BYTE* p, PositionId target, int id); // aiplay.cpp
extern "C" void __cdecl Road_PositionCostBest(int side, PositionId from, PositionId to, double* cost);   // aitrce.cpp
extern "C" void* __fastcall TownId_Town(const int* s, void*);
KING_VAR(AsmLabel, s_addCompetitor, 66d9a8)
KING_VAR(double, g_competitorRange66c6b0, 66c6b0)
KING_VAR(BYTE, g_addingCompetitor68d114, 68d114)

static unsigned Competitors_Count() { return (g_68a984 - g_68a980) + g_68d164; }

// a competitor near the human player's goods: sent to the goods' town, advanced ahead of the camera
static void Competitor_Take(int hn, BYTE* player)
{
    DVec3 at;
    Road_PointOf(&at, F(PositionId, Player_FirstGoods(player), 24));
    if (!AI_RoomForPlayer(player, &at) || !AI_RoomForPlayer(player, &at))
        return;
    int found;
    Players_Find4571a0(&found, hn, (int)g_rng68b9c8);
    if ((void*)found == g_players) {
        Players_Find457210(&found, hn, (int)g_rng68b9c8);
        if ((void*)found == g_players)
            return;
    }
    BYTE* f = (BYTE*)found;
    Player_ClearOrderLists(f + 8);
    CallC(FUN_425820, f, Player_FirstGoods(player));
    g_addingCompetitor68d114 = 1;
    Player_Place(f, F(PositionId, Player_FirstGoods(player), 24));
    int town;
    Town_Id(&town, (BYTE*)TownId_Town((int*)((BYTE*)Player_FirstGoods(player) + 12), 0));
    PositionId target;
    PositionInit(&target, town);
    Player_GoTo(f, target, 1);
    DVec3 q;
    Road_PointOf(&q, F(PositionId, f, 40));
    double dx = g_68bab0.x - q.x, dy = g_68bab0.y - q.y, dz = g_68bab0.z - q.z;
    if (x87_sqrt((dx * dx + dy * dy) + dz * dz) < g_competitorRange66c6b0) {
        double x = (double)(int)(g_68d160 * 400 - 400);
        double y = F(double, Player_FirstGoods(player), 112) * 0.33;
        double r = x <= y ? x : F(double, Player_FirstGoods(player), 112) * 0.33;
        if (r < 400.0)
            r = 400.0;
        else if (!(r <= 3000.0))
            r = 3000.0;
        double gap = (int)g_68d160 > 1 ? r / (double)(int)(g_68d160 - 1) : 400.0;
        if (0.0 < g_competitorRange66c6b0)
            for (int k = 0;;) {
                DVec3 w;
                Road_PointOf(&w, F(PositionId, f, 40));
                double ex = g_68bab0.x - w.x, ey = g_68bab0.y - w.y, ez = g_68bab0.z - w.z;
                if (!(x87_sqrt((ey * ey + ez * ez) + ex * ex) <= g_competitorRange66c6b0))
                    break;
                double step = 4.0;
                Ai_AdvancePlayer(f, &step);
                if (!((double)++k < g_competitorRange66c6b0))
                    break;
            }
        unsigned c = (unsigned)(int)(gap * 0.3333333333333333);
        unsigned rnd = AiRandom() % c;
        double step = (double)rnd + (double)(int)(g_68a984 - g_68a980) * gap;
        Ai_AdvancePlayer(f, &step);
    }
    Player_MarkListed(f);
    F(int, f, 100) = 4;
    Player_EndDealOf(f);
    Player_UpdateFlags(f);
    Ai_UpdateAdvancementRegular(f + 8, F(PositionId, f, 40));
    g_addingCompetitor68d114 = 0;
}

// [FUN_4262a0] addCompetitor: a competitor of the human player is taken (or found) near the player's goods,
// sent to the goods' town and advanced ahead of the camera
extern "C" void __cdecl Ai_AddCompetitor()
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_addCompetitor);
    int hn;
    Player_FromId(&hn);
    BYTE* human = (BYTE*)hn;
    if (human != (BYTE*)g_players && F(int, human, 100) != 5) {
        BYTE* player = human + 8;
        BYTE* goods = (BYTE*)Player_FirstGoods(player);
        if (!goods || F(int, goods, 104) != 1 || (g_68ccb8[F(int, goods, 0) * 104 + 48] & 4)) {
            Player_CheckGoods(human);
            Player_CheckTownAA(human);
            goto leave;
        }
        if (!g_68d15c)
            AI_RandomTraffic();
        if (!Competitors_Count()) {
            double cost = 0;
            BYTE* g = (BYTE*)Player_FirstGoods(player);
            Road_PositionCostBest(0, F(PositionId, g, 24), F(PositionId, player, 32), &cost);
            if (cost > 1000.0) {
                CallC(FUN_425dd0);
                goto leave;
            }
        }
        while (Competitors_Count() < g_68d15c) {
            int found;
            Players_Find457280(&found, hn, (int)goods, (int)g_rng68b9c8);
            BYTE* f = (BYTE*)found;
            if (f == (BYTE*)g_players)
                break;
            Player_MarkListed(f);
            Ai_UpdateAdvancementRegular(f + 8, F(PositionId, f, 40));
        }
        Competitor_Take(hn, player);
        int i = 0;
        for (BYTE** e = (BYTE**)g_68a980; e != (BYTE**)g_68a984; e++, i++)
            F(BYTE, *e, 848) = i % 3 == 0;
    }
leave:
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_addCompetitor);
}

// ---------------------------------------------------------------------------
// provideNoncontradictoryPositions()

KING_VAR(AsmLabel, s_provideNoncontradictoryPositions, 66dc00)
KING_VAR(double, g_pushRange66c760, 66c760)

// [FUN_42a4b0] provideNoncontradictoryPositions: players on the same road and side closer than the range push
// each other apart (plus a little noise); each one not at its target moves one step forward or back along its
// tangent by the push
extern "C" void __cdecl Ai_ProvideNoncontradictoryPositions()
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_provideNoncontradictoryPositions);
    BYTE* list[512];
    DVec3 push[512];
    DVec3 at[512];
    unsigned n = 0;
    PlayerListNode* head = (PlayerListNode*)g_players;
    for (PlayerListNode* it = head->next; it != head; it = it->next) {
        if (!Player_OfIt(it))
            continue;
        BYTE* p = Player_OfIt(it);
        if (F(int, p, 876) == 4096 && F(int, p, 84) != 1)
            continue;
        if (Player_AtTarget(p) & 0xff)
            continue;
        list[n++] = p;
    }
    for (unsigned i = 0; i < n; i++) {
        BYTE* p = list[i];
        at[i] = *(DVec3*)(p + ((F(BYTE, p, 88) & 1) ? 616 : 304));
        push[i].x = push[i].y = push[i].z = 0;
    }
    for (unsigned i = 0; i < n; i++)
        for (unsigned j = 0; (int)i > 0 && j < i; j++) {
            BYTE* a = list[j];
            BYTE* b = list[i];
            if (F(int, b, 876) != F(int, a, 876))
                continue;
            if ((F(int, a, 56) ^ F(int, b, 56)) & 1)
                continue;
            DVec3 d;
            d.x = at[i].x - at[j].x;
            d.y = at[i].y - at[j].y;
            d.z = at[i].z - at[j].z;
            double len = x87_sqrt((d.x * d.x + d.y * d.y) + d.z * d.z);
            if (!(len < g_pushRange66c760))
                continue;
            if (len > 1e-06) {
                double inv = 1.0 / len;
                d.x = inv * d.x;
                d.y = d.y * inv;
                d.z = d.z * inv;
            }
            AiRandom();
            AiRandom();
            double r = (double)(AiRandom() % 100);
            d.x = r * 1e-05 + d.x;
            d.y = r * 1e-05 + d.y;
            d.z = r * 1e-05 + d.z;
            push[i].x = d.x + push[i].x;
            push[i].y = d.y + push[i].y;
            push[i].z = d.z + push[i].z;
            push[j].x = push[j].x - d.x;
            push[j].y = push[j].y - d.y;
            push[j].z = push[j].z - d.z;
        }
    for (unsigned i = 0; i < n; i++) {
        BYTE* p = list[i];
        if (F(BYTE, p, 88) & 1)
            continue;
        if (Player_AtTarget(p) & 0xff)
            continue;
        DVec3 t;
        Ai__GetTangent(&t, F(PositionId, p, 32));
        double inv = 1.0 / x87_sqrt((t.x * t.x + t.y * t.y) + t.z * t.z);
        t.x = inv * t.x;
        t.y = t.y * inv;
        t.z = t.z * inv;
        double s = (push[i].z * t.z + push[i].x * t.x) + push[i].y * t.y;
        if (s < 0.0)
            s = -1.0;
        else if (s > 0.0)
            s = 1.0;
        else if (s == 0.0)
            continue;
        Ai_AdvancePlayer(F(BYTE*, list[i], 96), &s);
        DVec3 q;
        at[i] = *Road_PointOf(&q, F(PositionId, list[i], 32));
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_provideNoncontradictoryPositions);
}

// ---------------------------------------------------------------------------
// the race checkpoints

extern "C" BYTE* __fastcall Vec12_Erase(BYTE* self, void*, BYTE* first, BYTE* last);   // ai_traffic.cpp
typedef double RaceTimes[1];
KING_VAR(RaceTimes, g_raceTimes68cf70, 68cf70)

// the route length from the player's position to its own node
static double Race_CostToNode(BYTE* p)
{
    int node = *Player_NodeOf(&node, p);
    double cost = 0;
    PositionId to;
    Position_InitNode(&to, 0, node);
    CallC(FUN_486800, 0, F(PositionId, p, 40), to, &cost);
    return cost;
}

// the distance from v to a node; the two orders of the sum
static double Race_Dist1(const DVec3* v, int node)
{
    const DVec3* n = (const DVec3*)((BYTE*)g_roadNodes[node] + 48);
    double x = v->x - n->x, y = v->y - n->y, z = v->z - n->z;
    return x87_sqrt((y * y + z * z) + x * x);
}

static double Race_Dist2(const DVec3* v, int node)
{
    const DVec3* n = (const DVec3*)((BYTE*)g_roadNodes[node] + 48);
    double x = v->x - n->x, y = v->y - n->y, z = v->z - n->z;
    return x87_sqrt((x * x + y * y) + z * z);
}

// [FUN_416b80] a racer's next checkpoint: within 100 of one of the three it heads for the next (or, for the
// other kind in this race mode, the previous) one; after 9 laps for its finish; sets the remaining time at +504
extern "C" void __cdecl Ai_RaceNextCheckpoint(BYTE* p)
{
    double cost;
    if (F(int, p, 724) > 9) {
        cost = Race_CostToNode(p);
    } else {
        int c1, c2, c3;
        Node_ByName(&c1, "check_point1");
        Node_ByName(&c2, "check_point2");
        Node_ByName(&c3, "check_point3");
        DVec3 v;
        Road_PointOf(&v, F(PositionId, p, 40));
        bool forward = g_66c6e4 == 1 ? F(int, p, 92) != 7 : g_66c6e4 == 2 ? F(int, p, 92) == 7 : true;
        int next = 0;
        bool atCheckpoint = true;
        if (forward) {
            if (Race_Dist1(&v, c1) < 100.0)
                next = c2;
            else if (Race_Dist1(&v, c2) < 100.0)
                next = c3;
            else if (Race_Dist1(&v, c3) < 100.0)
                next = c1;
            else
                atCheckpoint = false;
        } else {
            if (Race_Dist2(&v, c1) < 100.0)
                next = c3;
            else if (Race_Dist2(&v, c2) < 100.0)
                next = c1;
            else if (Race_Dist1(&v, c3) < 100.0)
                next = c2;
            else
                atCheckpoint = false;
        }
        if (!atCheckpoint) {
            cost = Race_CostToNode(p);
        } else {
            if (F(int, p, 724) == 9) {
                char name[80];
                sprintf(name, "finish%d_%d", g_66c6e4, F(int, p, 728) + 1);
                int f;
                next = *Node_ByName(&f, name);
            }
            Hop* b = F(Hop*, p, 24);
            Hop* e = F(Hop*, p, 28);
            if (b != e && e[-1].node == next)
                cost = Race_CostToNode(p);
            else {
                Vec12_Erase(p + 24, 0, (BYTE*)b, (BYTE*)e);
                F(BYTE, p, 872) = 1;
                PositionId to;
                Position_InitNode(&to, 0, next);
                Player_GoTo(p, to, 1);
                cost = Race_CostToNode(p);
                F(int, p, 724)++;
            }
        }
    }
    F(double, p, 504) = -cost - g_raceTimes68cf70[F(int, p, 724)];
}

// ---------------------------------------------------------------------------
// getOrientationSmoothed(PlayerId player, PositionId pos)

extern "C" void FUN_439a70(); KING_ALIAS("_FUN_439a70", "_Ai_GetOrientation")    // getOrientation (out, PositionId32)
KING_VAR(AsmLabel, s_getOrientationSmoothed, 66e700)
KING_VAR(AsmLabel, s_invalidPositionId, 66cbe0)

// [FUN_439ce0] getOrientationSmoothed: the orientation at pos with its heading averaged over up to 15 steps
// back and ahead along the player's route
extern "C" BYTE* __cdecl Ai_GetOrientationSmoothed(BYTE* out, BYTE* player, PositionId pos)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_getOrientationSmoothed);
    PositionId cur = pos;
    if (!PositionValid(&cur)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", pos.road, pos.t, pos.segment, pos.object, pos.m_24);
        AiError(33, 100, s_mainFile, s_mainId, 15422, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    if (ValidRoad(pos.road)) {
        RoadEdge* e = g_roadEdges[pos.road];
        if (pos.object >= e->objectsEnd - e->objects || pos.segment >= CallVirt<int>(e->objects[pos.object], 24))
            AiError(33, 100, s_mainFile, s_mainId, 15422, s_invalidPositionId, &g_aiEmpty, &g_aiEmpty);
    }
    if (player != (BYTE*)g_players) {
        if (Player_NotValid(player)) {
            sprintf(g_numberText, "()");
            AiError(33, 100, s_mainFile, s_mainId, 15425, s_invalidId, "PlayerId", g_numberText);
        }
    }
    if (player == (BYTE*)g_players || (Player_AtTargetOf(player) & 0xff)) {
        CallC(FUN_439a70, out, pos);
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(s_getOrientationSmoothed);
        return out;
    }
    int steps = 15;
    double step = 15.1;
    PositionId tmp;
    Player_StepFrom(&tmp, player, &pos, &step);
    if (!(step == 0.0)) {
        double x = 15.0 - step;
        if (15.0 > x)
            steps = (int)x;
    }
    step = -15.1;
    Player_StepFrom(&tmp, player, &pos, &step);
    if (!(step == 0.0)) {
        double x = step + 15.0;
        if ((double)steps > x)
            steps = (int)x;
    }
    if (steps <= 0) {
        CallC(FUN_439a70, out, pos);
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(s_getOrientationSmoothed);
        return out;
    }
    DVec3 at, t;
    Road_PointOf(&at, pos);
    Ai__GetTangent(&t, pos);
    DVec3 sum = t;
    double ring[72];
    int head = 36, tail = 37;
    ring[36] = x87_atan2(t.y, t.x);
    for (int back = 0; back < 2; back++) {
        cur = pos;
        if (steps <= 1)
            continue;
        double dir = back ? -1 : 1;
        for (int i = 1; i < steps; i++) {
            double s = dir;
            PositionId next;
            cur = *Player_StepFrom(&next, player, &cur, &s);
            if (!(s == 0.0))
                break;
            DVec3 u;
            Ai__GetTangent(&u, cur);
            sum.x = sum.x + u.x;
            sum.y = u.y + sum.y;
            sum.z = u.z + sum.z;
            double a = x87_atan2(u.y, u.x);
            if (!back)
                ring[tail++] = a;
            else
                ring[--head] = a;
        }
    }
    double avg = ring[head];
    double k = 1.0;
    for (int j = head + 1; j != tail; j++) {
        double a = ring[j];
        k = k + 1.0;
        double lim = avg + 0.01;
        while (a + 3.141592653589793 < lim)
            a = a + 6.283185307179586;
        while (!(a - 3.141592653589793 <= lim))
            a = a - 6.283185307179586;
        avg = (avg * (k - 1.0) + a) / k;
    }
    if (head + 1 == tail || !(k == 0.0)) {
        double inv = 1.0 / x87_sqrt((sum.x * sum.x + sum.y * sum.y) + sum.z * sum.z);
        sum.z = sum.z * inv;
    }
    double c = x87_cos(avg), s = x87_sin(avg);
    double inv = 1.0 / x87_sqrt((c * c + s * s) + sum.z * sum.z);
    DVec3 d;
    d.x = inv * c;
    d.y = s * inv;
    d.z = sum.z;
    CallC(FUN_4397e0, out, &at, &d);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_getOrientationSmoothed);
    return out;
}

// ---------------------------------------------------------------------------
// updateNavigator(PlayerId player)

extern "C" void FUN_4858b0(); KING_ALIAS("_FUN_4858b0", "_Road_FindPositionRoute")    // the route between two positions (out hops, side, PositionId32 from, PositionId32 to, double* cost)
extern "C" BYTE* __cdecl Copy12s(const BYTE* first, const BYTE* last, BYTE* dest);           // ai_containers.cpp
extern "C" void* __cdecl Stl_UninitCopy12(const void* f, const void* l, void* d);             // road_graph.cpp
extern "C" void __cdecl Road_OrientHops(HopVec* hops);                                        // route_search.cpp
extern "C" DVec3* __cdecl DVec3_Cross(DVec3* out, const DVec3* a, const DVec3* b);            // route_search.cpp
extern "C" double __fastcall Filter2_Update(void* self, void*, double x);                     // route_search.cpp
KING_VAR(AsmLabel, s_updateNavigator, 66db44)
KING_VAR(double, g_navigatorBand66c750, 66c750)

// v = src for vectors of 12-byte records
static void Hops_Assign(BYTE** v, BYTE* const* src)
{
    BYTE* first = src[0];
    BYTE* last = src[1];
    int n = (int)(last - first) / 12;
    unsigned cap = (int)(v[2] - v[0]) / 12;
    if ((unsigned)n > cap) {
        BYTE* p = Vec12b_AllocCopyA(n, first, last);
        if (cap)
            Ai_PoolFree(v[0], cap * 12);
        v[0] = p;
        v[2] = p + n * 12;
    } else {
        int have = (int)(v[1] - v[0]) / 12;
        if ((unsigned)have >= (unsigned)n)
            Copy12s(first, last, v[0]);
        else {
            Copy12s(first, first + have * 12, v[0]);
            Stl_UninitCopy12(src[0] + (int)(v[1] - v[0]) / 12 * 12, src[1], v[1]);
        }
    }
    v[1] = v[0] + n * 12;
}

static double Dist3(const DVec3* a, const BYTE* p)
{
    double x = a->x - F(double, p, 624), y = a->y - F(double, p, 632), z = a->z - F(double, p, 640);
    return x87_sqrt((x * x + y * y) + z * z);
}

// [FUN_4289e0] updateNavigator: the turn the navigator shows (1 ahead, 2 back, 3 and 4 the two sides, 0 none):
// the player's route (or the one to its goods' town) is followed from the navigator position until it turns
// off the heading, and that angle is filtered
extern "C" int __cdecl Ai_UpdateNavigator(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_updateNavigator);
    BYTE** v = (BYTE**)(p + 648);
    bool none = (!Player_FirstGoods(p + 8) && F(BYTE*, p, 24) == F(BYTE*, p, 28)) || !(F(DWORD, p, 96) & 0x2000)
                || F(int, p, 100) == 5;
    PositionId at = F(PositionId, p, 816);
    if (!none) {
        DVec3 r;
        Road_PointOf(&r, at);
        none = !(Dist3(&r, p) <= 100.0);
    }
    if (!none) {
        if (F(BYTE*, p, 24) == F(BYTE*, p, 28)) {
            int town;
            Town_Id(&town, (BYTE*)g_townsBegin[F(int, Player_FirstGoods(p + 8), 12)]);
            PositionId to;
            PositionInit(&to, town);
            BYTE* route[3];
            double cost = 0;
            BYTE** res = CallC<BYTE**>(FUN_4858b0, route, 0, at, to, &cost);
            if (res != v)
                Hops_Assign(v, res);
            Vec12_Free(route, 0);
            Road_OrientHops((HopVec*)v);
        } else
            Hops_Assign(v, (BYTE* const*)(p + 24));
        none = v[0] == v[1];
    }
    if (none) {
        F(int, p, 660) = 0;
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(s_updateNavigator);
        return 0;
    }
    if (F(BYTE, v[0], 8) & 1)
        at.m_24 |= 1;
    else
        at.m_24 &= ~1;
    DVec3 dir = *(DVec3*)(p + 576);
    dir.z = 0;
    double inv = 1.0 / x87_sqrt(dir.y * dir.y + dir.x * dir.x);
    dir.x = inv * dir.x;
    dir.y = dir.y * inv;
    double ang;
    for (int k = 0;; k++) {
        double step = k ? 10 : 20;
        PositionId next;
        at = *Ai_AdvancePositionHops(&next, (HopVec*)v, at, &step);
        DVec3 r;
        Road_PointOf(&r, at);
        DVec3 d;
        d.x = r.x - F(double, p, 624);
        d.y = r.y - F(double, p, 632);
        d.z = 0;
        double dinv = 1.0 / x87_sqrt(d.x * d.x + d.y * d.y);
        d.x = dinv * d.x;
        d.y = d.y * dinv;
        DVec3 c;
        DVec3_Cross(&c, &d, &dir);
        double cz = c.z;
        ang = x87_acos((d.z * dir.z + d.y * dir.y) + d.x * dir.x);
        if (cz < 0.0)
            ang = -ang;
        if (x87_fabs(ang) > 0.39269908169872414 || k + 1 >= 14)
            break;
    }
    double last = F(double, p, 664);
    while (ang - 3.141592653589793 > last)
        ang = ang - 6.283185307179586;
    while (ang + 3.141592653589793 < last)
        ang = ang + 6.283185307179586;
    F(double, p, 664) = ang;
    double f = Filter2_Update(p + 672, 0, ang);
    if (f - g_navigatorBand66c750 > F(double, p, 712))
        F(double, p, 712) = f - g_navigatorBand66c750;
    if (f + g_navigatorBand66c750 < F(double, p, 712))
        F(double, p, 712) = f + g_navigatorBand66c750;
    double h = F(double, p, 712);
    while (h > 3.141592653589793)
        h = h - 6.283185307179586;
    while (h < -3.141592653589793)
        h = h + 6.283185307179586;
    if (x87_fabs(h) > 2.356194490192345)
        F(int, p, 660) = 2;
    else if (!(x87_fabs(h) > 0.39269908169872414))
        F(int, p, 660) = 1;
    else
        F(int, p, 660) = h < 0.0 ? 3 : 4;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_updateNavigator);
    return F(int, p, 660);
}

// ---------------------------------------------------------------------------
// offerBribe(PlayerId giver, PlayerId taker, unsigned long int flags)

extern "C" void FUN_431420(); KING_ALIAS("_FUN_431420", "_Ai_LastOfSorted")    // (out int*)
extern "C" void FUN_40a690(); KING_ALIAS("_FUN_40a690", "_Ai_CallHelpers")    // (int by value)
extern "C" void FUN_40aa00(); KING_ALIAS("_FUN_40aa00", "_Ai_CallMafia")    // (int by value)
extern "C" void FUN_4452c0(); KING_ALIAS("_FUN_4452c0", "_Ai_DropOrdersForfeit")    // (player)
extern "C" void __fastcall Rec4725e0_Init(BYTE* self, void*);   // ai_drivers.cpp
extern "C" BYTE* __fastcall Bribe_Construct(BYTE* s, void*, int a);
KING_VAR(AsmLabel, s_offerBribe, 66e074)
KING_VAR(double, g_bribeFine66c770, 66c770)
KING_VAR(double, g_bribeFine66c778, 66c778)
KING_VAR(double, g_bribeFine66c780, 66c780)
KING_VAR(double, g_bribeChance66c788, 66c788)
KING_VAR(BYTE*, g_places68a958c, 68a958)

static int OfferBribe_Leave(int r)
{
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_offerBribe);
    return r;
}

// [FUN_432560] offerBribe: 1 when the taker refuses the sum (queryBribeSum), else by the taker's kind: police take
// it (or fine the giver), 2 when the bribe is not possible, 4 when an inspector refused
extern "C" int __cdecl Ai_OfferBribe(BYTE* giver, BYTE* taker, unsigned flags)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_offerBribe);
    if (Player_NotValid(giver)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 13092, s_invalidId, "PlayerId", g_numberText);
    }
    if (Player_NotValid(taker)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 13093, s_invalidId, "PlayerId", g_numberText);
    }
    double sum = 0;
    if (Ai_QueryBribeSum(giver, taker, flags, &sum) == 1)
        return OfferBribe_Leave(1);
    BYTE flagsGot = Player_UpdateFlagsGet(taker);
    switch (F(int, taker, 92)) {
    case 5:
        if (Player_HasPlaceFlag1(taker + 8) & 0xff) {
            F(double, giver, 136) -= sum;
            Player_CheckBankrupt(giver);
            if (flags & 1) {
                Rec4725b0_Init(giver + 200, 0);
                AI_LightsOff2();
                Player_UpdateFlags(giver);
                F(DWORD, giver, 96) |= 0x800000;
            }
            if (flags & 2) {
                Rec4725e0_Init(giver + 200, 0);
                Player_UpdateFlags(giver);
                F(DWORD, giver, 96) |= 0x100000;
            }
            if (flags & 4) {
                int t;
                CallC(FUN_431420, &t);
                CallC(FUN_40a690, t);
            }
            return OfferBribe_Leave(0);
        } else {
            double x = 0.0;
            if (flags & 1)
                x = g_bribeFine66c770;
            if (flags & 2)
                x = x + g_bribeFine66c778;
            if (flags & 4)
                x = x + g_bribeFine66c780;
            F(double, giver, 256) = x + F(double, giver, 256);
            BYTE* o = (BYTE*)operator new(56);
            BYTE* task = o ? Bribe_Construct(o, 0, (int)giver) : 0;
            BYTE* end;
            SchList_End((BYTE**)&g_68b99c, 0, &end);
            BYTE* it;
            ListB_Insert(&g_68b99c, 0, &it, (BYTE**)end, &task);
            return OfferBribe_Leave(2);
        }
    case 6: {
        F(double, giver, 136) -= sum;
        Player_CheckBankrupt(giver);
        double chance = g_bribeChance66c788;
        unsigned r = Rng_Random(g_rng68b9c8, 0, 1000000);
        if (!(chance * 1000000.0 > (double)r))
            return OfferBribe_Leave(4);
        if (flags & 8)
            F(DWORD, giver, 96) |= 0x100000;
        if (flags & 4) {
            int t;
            CallC(FUN_431420, &t);
            CallC(FUN_40aa00, t);
        }
        return OfferBribe_Leave(0);
    }
    case 2:
        F(double, giver, 136) -= sum;
        Player_CheckBankrupt(giver);
        if (flags & 32)
            F(DWORD, taker, 96) |= 0x1000000;
        if (flags & 16) {
            CallC(FUN_4452c0, taker);
            if (flagsGot) {
                g_68d15c--;
                Player_Leave(taker, 0);
            }
        }
        return OfferBribe_Leave(0);
    case 7:
    case 8:
    case 9:
        if (!(flags & 64))
            return OfferBribe_Leave(0);
        if (F(int, g_places68a958c + F(int, taker, 992) * 56, 0) == 2 || Player_UpdateFlagsGet(taker))
            return OfferBribe_Leave(2);
        return OfferBribe_Leave(0);
    default:
        return OfferBribe_Leave(2);
    }
}

// ---------------------------------------------------------------------------
// onModuleLoadX(char const * moduleName)

extern "C" int __cdecl AI_Level3();          // route_search.cpp
extern "C" int __cdecl AI_TruckInRange();    // route_search.cpp
extern "C" int __cdecl AI_TruckGripLow();    // route_search.cpp
extern "C" void __cdecl updateSwitch(const char* moduleName, const char* switchName, int v);   // ai_task.cpp
KING_VAR(AsmLabel, s_onModuleLoadX, 66dc24)
KING_VAR(BYTE**, g_switches68ceb0, 68ceb0)
KING_VAR(BYTE**, g_switches68ceb4, 68ceb4)

// whether a switch record belongs to the module (its id names the module)
static bool Switch_InModule(BYTE* e, const char* name)
{
    int id = *(int*)(e ? e + 12 : 0);
    if (id == -1)
        return false;
    BYTE** s = (BYTE**)g_registry24[id];
    return strcmp(s[0] != s[1] ? (const char*)s[0] : 0, name) == 0;
}

// a chance at +40 against the AI generator
static bool Switch_Chance(BYTE* e)
{
    double c = F(double, e, 40);
    unsigned r = Rng_Random(g_rng68b9c8, 0, 1000000);
    return c * 1000000.0 > (double)r;
}

// [FUN_42aba0] onModuleLoadX: the switches of a loaded module are updated; random ones (by kind, at the third
// level) are set by chance first
extern "C" int __cdecl Ai_OnModuleLoadX(const char* name)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_onModuleLoadX);
    for (void** it = g_68cc18; it != g_68cc1c; it++) {
        BYTE* e = (BYTE*)*it;
        if (Switch_InModule(e, name))
            Obj_483c10(e ? e + 12 : 0);
    }
    if (AI_Level3() & 0xff) {
        for (BYTE** it = g_switches68ceb0; it != g_switches68ceb4; it++) {
            BYTE* e = *it;
            if (!Switch_InModule(e, name))
                continue;
            switch (F(int, e, 32)) {
            case 1:
            case 3:
                F(BYTE, *it, 28) = (F(int, e, 32) == 1 || (AI_TruckGripLow() & 0xff)) && Switch_Chance(*it);
                break;
            case 2:
                F(BYTE, *it, 28) = (AI_TruckInRange() & 0xff) && Switch_Chance(*it);
                break;
            case 4:
                F(BYTE, *it, 28) = (AI_TruckInRange() & 0xff) && (AI_TruckGripLow() & 0xff) && Switch_Chance(*it);
                break;
            }
            Obj_483c10(*it ? *it + 12 : 0);
        }
    } else {
        for (BYTE** it = g_switches68ceb0; it != g_switches68ceb4; it++) {
            BYTE* e = *it;
            if (!Switch_InModule(e, name))
                continue;
            F(BYTE, e, 28) = 0;
            Obj_483c10(*it ? *it + 12 : 0);
        }
    }
    for (void** it = g_68a970; it != g_68a974; it++) {
        BYTE* e = (BYTE*)*it;
        if (Switch_InModule(e, name))
            Obj_483c10(e ? e + 12 : 0);
    }
    if (_stricmp(name, "dr") == 0)
        updateSwitch(name, "StartKey", g_66c6e4 != 1);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_onModuleLoadX);
    return 0;
}

// ---------------------------------------------------------------------------
// the priced length of an edge between two positions

extern "C" bool __fastcall Pos_Before(PositionId* self, void*, PositionId p);

static BYTE* EdgeObj_Data(BYTE* o) { return o ? o + 8 : 0; }

// [t0 at seg0 of object o0 .. t1 at seg1 of object o1], each object's length (or part) times its price factor
static double Edge_PricedRun(DWORD* factor, BYTE* e, int o0, double t0, int seg0, int o1, double t1, int seg1)
{
    BYTE** objs = F(BYTE**, e, 120);
    int n = F(BYTE**, e, 124) - objs;
    if (o1 == n) {
        o1 = n - 1;
        seg1 = CallVirt<int>(objs[o1], 24) - 1;
        t1 = 0;
    }
    if (o0 == o1) {
        double r = CallVirt<double>(objs[o0], 32, t0, seg0, t1, seg1);
        return Rec_PriceFactor((BYTE*)factor, 0, (const DWORD*)EdgeObj_Data(objs[o0])) * r;
    }
    int last = CallVirt<int>(objs[o0], 24) - 1;
    double sum = CallVirt<double>(objs[o0], 32, t0, seg0, 1.0, last);
    sum = Rec_PriceFactor((BYTE*)factor, 0, (const DWORD*)EdgeObj_Data(objs[o0])) * sum;
    for (int i = o0 + 1; i < o1; i++) {
        BYTE* o = F(BYTE**, e, 120)[i];
        double f = Rec_PriceFactor((BYTE*)factor, 0, (const DWORD*)EdgeObj_Data(o));
        sum = CallVirt<double>(o, 36) * f + sum;
    }
    BYTE* o = F(BYTE**, e, 120)[o1];
    double r = CallVirt<double>(o, 32, 0.0, 0, t1, seg1);
    return Rec_PriceFactor((BYTE*)factor, 0, (const DWORD*)EdgeObj_Data(o)) * r + sum;
}

// a node position as the edge's start or end
static void Edge_NodeToPosition(PositionId* p, BYTE* e, int line, const char* msg)
{
    if (p->road != -1)
        return;
    BYTE* n = (BYTE*)g_roadNodes[p->node];
    PositionId tmp;
    if (n == F(BYTE*, e, 112))
        *p = *Road_EdgeStart(&tmp, (RoadEdge*)e);
    else if (n == F(BYTE*, e, 116))
        *p = *Road_EdgeEnd(&tmp, (RoadEdge*)e);
    else
        AiError(0, 100, s_mainFile, s_mainId, line, msg, &g_aiEmpty, &g_aiEmpty);
}

// [FUN_409aa0] the priced length of the edge from a to b (either order)
extern "C" double __cdecl Ai_EdgePricedLength(BYTE* ctx, BYTE* e, PositionId a, PositionId b)
{
    Edge_NodeToPosition(&a, e, 2127, "Internal error 7644");
    Edge_NodeToPosition(&b, e, 2138, "Internal error 7645");
    DWORD factor[2] = { (DWORD)ctx, 0 };
    if (Pos_Before(&a, 0, b))
        return Edge_PricedRun(factor, e, a.object, a.t, a.segment, b.object, b.t, b.segment);
    factor[1] = 1;
    return Edge_PricedRun(factor, e, b.object, b.t, b.segment, a.object, a.t, a.segment);
}

// ---------------------------------------------------------------------------
// getPlayerData(PlayerId player, GetPlayerData & pd)

extern "C" int __cdecl Player_LicenceTimeLeft(BYTE* p, double* left);   // ai_drivers.cpp
extern "C" int* __fastcall Player_GetPlace(BYTE* self, void*);           // ai_drivers.cpp
KING_VAR(AsmLabel, s_getPlayerData, 66ed70)
KING_VAR(double, g_dayLength66c698, 66c698)

// the order record's town data of a cargo (or none)
static void PlayerData_Cargo(BYTE* o, BYTE* cargo, int at)
{
    if (cargo && F(int, cargo, 4) != -1) {
        BYTE* v = (BYTE*)g_vehicles[F(int, cargo, 4)];
        F(int, o, at) = F(int, v, 64);
        F(int, o, at + 4) = F(int, v, 68);
        F(int, o, at + 8) = F(int, v, 72);
    } else
        F(int, o, at + 8) = 0;
}

// [FUN_43fc40] getPlayerData: the player's state copied out for the interface
extern "C" void __cdecl Ai_GetPlayerData(BYTE* p, BYTE* o)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_getPlayerData);
    if (Player_NotValid(p)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 16719, s_invalidId, "PlayerId", g_numberText);
    }
    Player_UpdateFlags(p);
    F(DWORD, o, 4) = F(DWORD, p, 96);
    F(int, o, 8) = F(int, p, 92);
    F(int, o, 12) = -1;
    F(int, o, 16) = 0;
    F(int, o, 24) = -1;
    F(int, o, 36) = F(int, p, 84);
    PlayerData_Cargo(o, F(BYTE*, p, 516), 12);
    PlayerData_Cargo(o, F(BYTE*, p, 520), 24);
    F(DWORD, o, 40) = F(DWORD, p, 136);
    F(DWORD, o, 44) = F(DWORD, p, 140);
    F(int, o, 48) = F(int, p, 272);
    F(int, o, 52) = (F(int, p, 148) - F(int, p, 144)) >> 3;
    Player_LicenceTimeLeft(p, (double*)(o + 64));
    F(int, o, 56) = F(int, p, 156);
    F(int, o, 72) = F(int, p, 524);
    F(int, o, 76) = F(int, p, 528);
    F(int, o, 80) = (F(int, p, 164) - F(int, p, 160)) >> 4;
    F(int, o, 84) = F(int, p, 660);
    F(double, o, 88) = 0;
    if (F(double, p, 192) > 0.0 && F(double, p, 864) > 0.0) {
        double x = F(double, p, 192) / (F(double, p, 864) * 10.0);
        F(double, o, 88) = x;
        if (x > 1.0)
            F(double, o, 88) = 1.0;
    }
    F(int, o, 104) = F(int, p, 768);
    F(int, o, 108) = F(int, p, 772);
    F(int, o, 112) = F(int, p, 776);
    F(int, o, 116) = F(int, p, 780);
    F(double, o, 120) = 0;
    if (Player_FirstGoods(p + 8)) {
        BYTE* d = (BYTE*)Player_Driver(p);
        double left = F(double, d, 72) - AI_Now();
        F(double, o, 120) = left;
        if (left < 0.0)
            F(double, o, 120) = 0;
        double now = AI_Now();
        double since = now - F(double, (BYTE*)Player_Driver(p), 80);
        F(double, Iter_Data(&p, 0), 800) = since;
    } else if (F(int, p, 100) == 5 || g_68d188 == 1 || g_68d188 == 2) {
        DWORD fl = F(DWORD, p, 96);
        if ((fl & 0x4000) && !(fl & 0x100))
            F(double, p, 808) = AI_Now() - F(double, p, 792);
        F(double, o, 120) = g_dayLength66c698 - F(double, p, 808);
    }
    F(DWORD, o, 128) = F(DWORD, p, 808);
    F(DWORD, o, 132) = F(DWORD, p, 812);
    F(BYTE, o, 136) = g_68a980 != g_68a984;
    if (F(int, p, 100) == 5)
        F(BYTE, o, 136) = 1;
    BYTE** orders = (BYTE**)Player_Orders(p, 0);
    if (orders[0] != orders[1])
        F(BYTE, o, 136) = 1;
    if (Player_UpdateFlagsGet(p))
        F(BYTE, o, 136) = 1;
    F(double, o, 144) = Ai_GetPlayerCapital(p);
    F(double, o, 152) = Ai_GetPlayerVirtualCapital(p);
    F(DWORD, o, 160) = F(DWORD, p, 344);
    F(DWORD, o, 164) = F(DWORD, p, 348);
    if (Player_Flag4(p) & 0xff) {
        F(DWORD, o, 160) = 0;
        F(DWORD, o, 164) = 0;
    }
    F(DWORD, o, 96) = F(DWORD, p, 544);
    F(DWORD, o, 100) = F(DWORD, p, 548);
    Player_AllCosts(p, (double*)(o + 176), (double*)(o + 168));
    int* place = Player_GetPlace(p + 8, 0);
    F(int, o, 184) = place ? *place : 1;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_getPlayerData);
}

// ---------------------------------------------------------------------------
// getOrderInfo(PlayerId player, int orderid, OrderInfo & oi)

KING_VAR(AsmLabel, s_getOrderInfoPlayer, 66f0c4)

static int OrderInfoPlayer_Leave(int r)
{
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_getOrderInfoPlayer);
    return r;
}

// [FUN_442350] getOrderInfo: fills oi with one of the player's own orders; -1 when there is no such order
extern "C" int __cdecl Ai_GetPlayerOrderInfo(BYTE* p, unsigned idx, BYTE* oi)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_getOrderInfoPlayer);
    F(BYTE, oi, 184) = 0;
    if (Player_NotValid(p)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 17333, s_invalidId, "PlayerId", g_numberText);
    }
    BYTE** orders = (BYTE**)Player_Orders(p, 0);
    if ((unsigned)((int)(orders[1] - orders[0]) / 144) <= idx)
        return OrderInfoPlayer_Leave(-1);
    orders = (BYTE**)Player_Orders(p, 0);
    BYTE* rec = orders[0] + idx * 144;
    F(DWORD, oi, 8) = F(DWORD, rec, 56);
    F(DWORD, oi, 12) = F(DWORD, rec, 60);
    F(int, oi, 0) = F(int, rec, 0);
    memcpy(oi + 16, rec + 24, 32);
    char** name = (char**)(oi + 52);
    char* const* src = (char* const*)(g_68ccb8 + F(int, rec, 0) * 104 + 88);
    if (src != name)
        CharVec_Copy(name, src, true);
    if (F(int, rec, 8) != -1) {
        src = (char* const*)(g_68ccb8 + F(int, rec, 8) * 104 + 88);
        if (src != name)
            CharVec_Copy(name, src, true);
    }
    CharVec_Erase(oi + 64, 0, F(BYTE*, oi, 64), F(BYTE*, oi, 68));
    if (F(int, rec, 4) != -1)
        PodVec_Assign((BYTE**)(oi + 64), 0, (BYTE* const*)(g_68ccb8 + F(int, rec, 4) * 104 + 88));
    CharVec_Erase(oi + 76, 0, F(BYTE*, oi, 76), F(BYTE*, oi, 80));
    PodVec_Assign((BYTE**)(oi + 88), 0, (BYTE* const*)((BYTE*)g_towns[F(int, rec, 12)] + 12));
    F(int, oi, 48) = F(int, rec, 12);
    Rec_FirstNonEmpty(oi, 0);
    F(double, oi, 120) = Order_Pay(rec, 0.0, F(int, rec, 104) == 1);
    char bonus = (Player_UpdateFlagsGet(p) || (Player_IsHuman(p) & 0xff)) && F(int, rec, 104) == 1;
    F(double, oi, 128) = Order_Margin((int*)rec, (double)F(int, p, 528), bonus, 0, 0);
    double left = F(double, rec, 72) - AI_Now();
    F(double, oi, 168) = left;
    if (left < 0.0)
        F(double, oi, 168) = 0;
    F(DWORD, oi, 176) = F(DWORD, rec, 96);
    F(DWORD, oi, 180) = F(DWORD, rec, 100);
    F(BYTE, oi, 184) = 1;
    return OrderInfoPlayer_Leave(0);
}

// ---------------------------------------------------------------------------
// getRightDirection(PlayerId player, PositionId pos)

extern "C" void FUN_445c90(); KING_ALIAS("_FUN_445c90", "_Ai_GetRightDirectionOf")    // the position's direction by default (out, PositionId32)
KING_VAR(AsmLabel, s_getRightDirection, 66f364)

static PositionId* RightDirection_Default(PositionId* out, PositionId pos)
{
    CallC(FUN_445c90, out, pos);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_getRightDirection);
    return out;
}

// [FUN_445f60] getRightDirection: pos with its direction bit set for the player's route to its goods' town (a
// one-way room decides first)
extern "C" PositionId* __cdecl Ai_GetRightDirection(PositionId* out, BYTE* player, PositionId pos)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_getRightDirection);
    PositionId chk = pos;
    if (!PositionValid(&chk)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", pos.road, pos.t, pos.segment, pos.object, pos.m_24);
        AiError(33, 100, s_mainFile, s_mainId, 18163, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    if (ValidRoad(pos.road)) {
        RoadEdge* e = g_roadEdges[pos.road];
        if (pos.object >= e->objectsEnd - e->objects || pos.segment >= CallVirt<int>(e->objects[pos.object], 24))
            AiError(33, 100, s_mainFile, s_mainId, 18163, s_invalidPositionId, &g_aiEmpty, &g_aiEmpty);
    }
    if (player == (BYTE*)g_players)
        return RightDirection_Default(out, pos);
    if (Player_NotValid(player)) {
        sprintf(g_numberText, "()");
        AiError(33, 100, s_mainFile, s_mainId, 18168, s_invalidId, "PlayerId", g_numberText);
    }
    if (!Player_FirstGoods(player + 8))
        return RightDirection_Default(out, pos);
    DWORD room[11];
    room[0] = 0;
    room[2] = 0;
    room[3] = 0;
    room[4] = (DWORD)g_undefinedModule;
    room[5] = (DWORD)g_undefinedRoom;
    room[6] = 0;
    room[7] = 0;
    room[9] = (DWORD)-1;
    room[10] = (DWORD)-1;
    const DWORD* d = (const DWORD*)&pos;
    CallC(FUN_438d70, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], room);
    if (room[0] & 8)
        pos.m_24 |= 1;
    else if (room[0] & 4)
        pos.m_24 &= ~1;
    int town;
    Town_Id(&town, (BYTE*)g_townsBegin[F(int, Player_FirstGoods(player + 8), 12)]);
    PositionId to;
    PositionInit(&to, town);
    BYTE* route[3];
    double cost = 0;
    CallC(FUN_4858b0, route, 0, pos, to, &cost);
    Road_OrientHops((HopVec*)route);
    if (route[0] == route[1]) {
        RightDirection_Default(out, pos);
        if ((route[2] - route[0]) / 12)
            Ai_PoolFree(route[0], (route[2] - route[0]) / 12 * 12);
        return out;
    }
    if (F(BYTE, route[0], 8) & 1)
        pos.m_24 |= 1;
    else
        pos.m_24 &= ~1;
    *out = pos;
    if ((route[2] - route[0]) / 12)
        Ai_PoolFree(route[0], (route[2] - route[0]) / 12 * 12);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_getRightDirection);
    return out;
}

// ---------------------------------------------------------------------------
// calculatePlayerSpeedsAndFares()

extern "C" void FUN_476f70(); KING_ALIAS("_FUN_476f70", "_Player_GiveBackCargo")    // a player out of money (owner, player node, 0)
extern "C" double __cdecl Driver_Load(BYTE* p);                   // ai_drivers.cpp
extern "C" void __cdecl Messages_Add(const char* text);           // helicopter.cpp
KING_VAR(AsmLabel, s_calculatePlayerSpeedsAndFares, 66d95c)
KING_VAR(double, g_lastSpeedTime68d0f0, 68d0f0)
KING_VAR(double, g_humanSpeed66c6a0, 66c6a0)
typedef BYTE CarModels[1];
KING_VAR(CarModels, g_carModels6978f8, 6978f8)   // 2416 bytes each

// the owner of a player ran out of money: its handling, and a message for the human
static void Fares_OutOfMoney(BYTE* owner, void* node)
{
    CallC(FUN_476f70, owner, node, 0);
    if (Player_IsHuman(owner) & 0xff)
        Messages_Add(Message_Text("NO_MONEY_FOR_ADVANCE"));
}

// [FUN_423f40] calculatePlayerSpeedsAndFares: each player's movement since the last call gives its speed (filtered
// once per 10 ms), velocity, the fare its passengers pay to it and the costs of driving for hire
extern "C" void __cdecl Ai_CalculatePlayerSpeedsAndFares()
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_calculatePlayerSpeedsAndFares);
    double dt = AI_Now() - g_lastSpeedTime68d0f0;
    if (dt < 1e-06) {
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(s_calculatePlayerSpeedsAndFares);
        return;
    }
    int steps = (int)(dt * 100.0);
    PlayerListNode* head = (PlayerListNode*)g_players;
    for (PlayerListNode* n = head->next; n != head; n = n->next) {
        BYTE* p = Player_OfIt(n);
        DVec3 pt;
        Road_PointOf(&pt, F(PositionId, p, 32));
        DVec3 d;
        d.x = pt.x - F(double, p, 280);
        d.y = pt.y - F(double, p, 288);
        d.z = pt.z - F(double, p, 296);
        F(double, p, 280) = pt.x;
        F(double, p, 288) = pt.y;
        F(double, p, 296) = pt.z;
        if (!g_68d188 && (F(DWORD, p, 88) & 0x800)) {
            double len = x87_sqrt((d.x * d.x + d.y * d.y) + d.z * d.z);
            double fare = F(double, p, 184) - Driver_Load(p) * len * 0.001;
            F(double, p, 184) = fare;
            if (fare < 0.0) {
                BYTE* owner = F(BYTE*, p, 168);
                if (!(Driver_Load(p) * 10.0 <= F(double, owner, 136)))
                    Fares_OutOfMoney(owner, n);
                else {
                    double v = Driver_Load(p);
                    F(double, p, 856) = v;
                    F(double, owner, 136) = F(double, owner, 136) - v * 10.0;
                    double w = F(double, p, 856) * 10.0;
                    F(double, p, 184) = w + F(double, p, 184);
                    F(double, p, 128) = w + F(double, p, 128);
                    F(double, p, 536) = F(double, p, 536) - w;
                }
            }
        }
        if (F(int, p, 84) == 2) {
            BYTE* veh = (BYTE*)g_vehicles[F(int, F(BYTE*, p, 508), 4)];
            double a = *(float*)(g_carModels6978f8 + F(int, veh, 64) * 2416 + 12);
            BYTE* owner = F(BYTE*, p, 168);
            double len = x87_sqrt((d.x * d.x + d.y * d.y) + d.z * d.z);
            if (owner != (BYTE*)g_players) {
                double c = a * len * 2e-05;
                F(double, owner, 136) = F(double, owner, 136) - c;
                F(double, p, 536) = F(double, p, 536) - c;
                if (F(double, owner, 136) < 0.0)
                    Fares_OutOfMoney(owner, n);
            } else
                F(double, p, 128) = F(double, p, 128) - len * a * 0.002;
        }
        double speed = x87_sqrt((d.x * d.x + d.y * d.y) + d.z * d.z) / dt;
        if (speed < 100.0) {
            for (int k = steps - 1; k > 0; k--) {
                Filter2_Update(p + 368, 0, speed);
                Filter2_Update(p + 408, 0, speed);
            }
            F(double, p, 328) = Filter2_Update(p + 368, 0, speed);
            F(double, p, 336) = Filter2_Update(p + 408, 0, speed);
            double inv = 1.0 / dt;
            double vx = inv * d.x;
            double vy = d.y * inv;
            double vz = d.z * inv;
            F(double, p, 344) = vx;
            F(double, p, 352) = vy;
            F(double, p, 360) = vz;
        }
        if (F(int, p, 84) == 1)
            g_humanSpeed66c6a0 = F(double, p, 328);
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_calculatePlayerSpeedsAndFares);
}

// ---------------------------------------------------------------------------
// advancePosition(PositionId pos, double & offset, int __flags)

KING_VAR(AsmLabel, s_advancePositionRoad, 66e3c8)

static void AdvanceRoad_Check(const PositionId* pos, int line)
{
    PositionId chk = *pos;
    if (!PositionValid(&chk)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", pos->road, pos->t, pos->segment, pos->object, pos->m_24);
        AiError(33, 100, s_mainFile, s_mainId, line, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    if (ValidRoad(pos->road)) {
        RoadEdge* e = g_roadEdges[pos->road];
        if (pos->object >= e->objectsEnd - e->objects || pos->segment >= CallVirt<int>(e->objects[pos->object], 24))
            AiError(33, 100, s_mainFile, s_mainId, line, s_invalidPositionId, &g_aiEmpty, &g_aiEmpty);
    }
}

// [FUN_435d60] advancePosition: pos moved by offset along its edge (against it when the direction bit is set);
// what is left of offset stays in it, the position takes the end node when it reached one
extern "C" PositionId* __cdecl Ai_AdvancePositionRoad(PositionId* out, PositionId pos, double* off, int)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_advancePositionRoad);
    AdvanceRoad_Check(&pos, 14404);
    DVec3 pt;
    Road_PointOf(&pt, pos);
    if (pos.m_24 & 1)
        *off = -*off;
    double eps = 0.1;
    RoadEdge* e = g_roadEdges[pos.road];
    if (*off > 0.0) {
        while ((unsigned)pos.object < (unsigned)(e->objectsEnd - e->objects)) {
            if (!CallVirt<int>(e->objects[pos.object], 16, &pos.t, off, &eps, &pos.segment))
                break;
            if ((unsigned)pos.object >= (unsigned)(e->objectsEnd - e->objects - 1))
                break;
            pos.object++;
            pos.segment = 0;
            pos.t = 0;
        }
    } else if (*off < 0.0) {
        while ((unsigned)pos.object < (unsigned)(e->objectsEnd - e->objects)) {
            if (!CallVirt<int>(e->objects[pos.object], 16, &pos.t, off, &eps, &pos.segment))
                break;
            if (pos.object == 0)
                break;
            pos.object--;
            pos.segment = CallVirt<int>(e->objects[pos.object], 24) - 1;
            pos.t = 1.0;
        }
    }
    if (*off > 0.0 || *off < 0.0) {
        BYTE* n = F(BYTE*, g_roadEdges[pos.road], *off > 0.0 ? 116 : 112);
        pos.node = F(int, n, 16);
        if (pos.m_24 & 1)
            *off = -*off;
    } else
        pos.node = -1;
    AdvanceRoad_Check(&pos, 14432);
    *out = pos;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_advancePositionRoad);
    return out;
}

// ---------------------------------------------------------------------------
// the AI world cleared

extern "C" void FUN_470ae0(); KING_ALIAS("_FUN_470ae0", "@Rec470ae0_Destroy@4")    // a node record's destructor (thiscall)
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")    // ()
extern "C" DWORD* __cdecl FillDwords(DWORD* d, unsigned n, const DWORD* v);        // ai_containers.cpp
extern "C" BYTE* __cdecl Rec104_Copy(const BYTE* f, const BYTE* l, BYTE* d);         // ai_containers.cpp
extern "C" void __fastcall Road47fa80_Destroy(BYTE* self, void*);                    // road_graph.cpp
extern "C" BYTE* __fastcall Mult46c8e0_DeletingDestructor(BYTE* self, void*, int flags);   // ai_traffic.cpp
extern "C" BYTE* __fastcall Mult46c920_DeletingDestructor(BYTE* self, void*, int flags);   // ai_traffic.cpp
KING_VAR(BYTE, g_worldLoaded68d0ec, 68d0ec)
typedef BYTE* PtrTriple[3];
KING_VAR(PtrTriple, w_kinds68cf48, 68cf48)      // vector<char const *>, the player kind names
KING_VAR(PtrTriple, w_driverName68d0a8, 68d0a8) // vector<char>
KING_VAR(PtrTriple, w_vehicles68cfd8, 68cfd8)
KING_VAR(PtrTriple, w_nodes68cf00, 68cf00)
KING_VAR(PtrTriple, w_rooms68d0d8, 68d0d8)
KING_VAR(PtrTriple, w_registry68d0c8, 68d0c8)
KING_VAR(PtrTriple, w_edges68cf38, 68cf38)
KING_VAR(PtrTriple, w_templates68ccb8, 68ccb8)   // 104-byte records
KING_VAR(PtrTriple, w_mults68cc08, 68cc08)       // 24-byte records
KING_VAR(BYTE, w_hash68d008, 68d008)
KING_VAR(BYTE, w_hash68cff0, 68cff0)
KING_VAR(BYTE, w_hash68cf10, 68cf10)
KING_VAR(AsmLabel, s_kindLive, 66c8a8)
KING_VAR(AsmLabel, s_kindAi, 66c8a4)
KING_VAR(AsmLabel, s_kindPolice, 66c89c)
KING_VAR(AsmLabel, s_kindMafia, 66c894)
KING_VAR(AsmLabel, s_kindDumb, 66c88c)
KING_VAR(AsmLabel, s_kindGhost, 66c884)
KING_VAR(AsmLabel, s_kindDealer, 66c87c)
KING_VAR(AsmLabel, s_driverName, 66c874)

// each owned object of the vector destroyed and deleted
static void World_DeleteEach(BYTE** v, void (*destroy)(BYTE*))
{
    for (BYTE** it = (BYTE**)v[0]; it != (BYTE**)v[1]; it++)
        if (*it) {
            destroy(*it);
            operator delete(*it);
        }
}

static void World_DestroyNode(BYTE* p) { CallThis(FUN_470ae0, p); }
static void World_DestroyRoom(BYTE* p) { Rec403810_Destroy((BYTE**)p, 0); }
static void World_DestroyName(BYTE* p) { CallThis(FUN_404420, p); }
static void World_DestroyEdge(BYTE* p) { Road47fa80_Destroy(p, 0); }
static void World_DestroyVehicle(BYTE* p) { Rec403680_Destroy(p, 0); }

// [FUN_4031e0] the AI world cleared: the twelve player kinds named again, the driver name reset, the vehicles, nodes,
// rooms, names and edges deleted, their lookups cleared, the order templates and mults destroyed
extern "C" void __cdecl Ai_ClearWorld()
{
    g_worldLoaded68d0ec = 0;
    VecMethods tmp;
    char alloc;
    Vec4_Reserve(&tmp, 0, 12, (int)&alloc);
    DWORD zero = 0;
    tmp.end = (BYTE*)FillDwords((DWORD*)tmp.begin, 12, &zero);
    BYTE** k = w_kinds68cf48;
    const BYTE* first = tmp.begin;
    const BYTE* last = tmp.end;
    unsigned n = (int)(last - first) >> 2;
    unsigned cap = (int)(k[2] - k[0]) >> 2;
    if (n > cap) {
        BYTE* p = (BYTE*)Vec4b_AllocCopyC(n, (const DWORD*)first, (const DWORD*)last);
        if (cap)
            Ai_PoolFree(k[0], cap * 4);
        k[0] = p;
        k[2] = p + n * 4;
    } else {
        unsigned have = (int)(k[1] - k[0]) >> 2;
        if (have >= n)
            CopyDwords((const DWORD*)first, (const DWORD*)last, (DWORD*)k[0]);
        else {
            CopyDwords((const DWORD*)first, (const DWORD*)(first + have * 4), (DWORD*)k[0]);
            CopyDwords((const DWORD*)(first + ((int)(k[1] - k[0]) >> 2) * 4), (const DWORD*)last, (DWORD*)k[1]);
        }
    }
    k[1] = k[0] + n * 4;
    Vec4_Free(&tmp.begin, 0);
    const char** names = (const char**)w_kinds68cf48[0];
    names[1] = s_kindLive;
    names[2] = s_kindAi;
    names[5] = s_kindPolice;
    names[6] = s_kindMafia;
    names[7] = s_kindDumb;
    names[8] = s_kindGhost;
    names[9] = s_kindDealer;
    char driver[7];
    memcpy(driver, s_driverName, 7);
    BYTE** dn = w_driverName68d0a8;
    CharVec_Erase(dn, 0, dn[0], dn[1]);
    CharVec_InsertRange44c220(dn, 0, dn[1], (BYTE*)driver, (BYTE*)driver + strlen(driver) + 1);
    World_DeleteEach(w_vehicles68cfd8, World_DestroyVehicle);
    Vec4_Erase2(w_vehicles68cfd8, 0, w_vehicles68cfd8[0], w_vehicles68cfd8[1]);
    World_DeleteEach(w_nodes68cf00, World_DestroyNode);
    CopyDwords((const DWORD*)w_nodes68cf00[1], (const DWORD*)w_nodes68cf00[1], (DWORD*)w_nodes68cf00[0]);
    w_nodes68cf00[1] = w_nodes68cf00[0];
    Hash_Clear((HashTable*)&w_hash68d008, 0);
    World_DeleteEach(w_rooms68d0d8, World_DestroyRoom);
    CopyDwords((const DWORD*)w_rooms68d0d8[1], (const DWORD*)w_rooms68d0d8[1], (DWORD*)w_rooms68d0d8[0]);
    w_rooms68d0d8[1] = w_rooms68d0d8[0];
    Hash_Clear((HashTable*)&w_hash68cff0, 0);
    World_DeleteEach(w_registry68d0c8, World_DestroyName);
    Copy32((const DWORD*)w_registry68d0c8[1], (const DWORD*)w_registry68d0c8[1], (DWORD*)w_registry68d0c8[0]);
    w_registry68d0c8[1] = w_registry68d0c8[0];
    Hash_Clear((HashTable*)&w_hash68cf10, 0);
    World_DeleteEach(w_edges68cf38, World_DestroyEdge);
    Copy32((const DWORD*)w_edges68cf38[1], (const DWORD*)w_edges68cf38[1], (DWORD*)w_edges68cf38[0]);
    w_edges68cf38[1] = w_edges68cf38[0];
    BYTE** t = w_templates68ccb8;
    BYTE* end = t[1];
    for (BYTE* r = Rec104_Copy(end, end, t[0]); r != end; r += 104)
        Mult46c8e0_DeletingDestructor(r, 0, 0);
    t[1] = t[1] - (int)(end - t[0]) / 104 * 104;
    BYTE** m = w_mults68cc08;
    end = m[1];
    for (BYTE* r = Rec24_Copy(end, end, m[0]); r != end; r += 24)
        Mult46c920_DeletingDestructor(r, 0, 0);
    m[1] = m[1] - (int)(end - m[0]) / 24 * 24;
    CallC(FUN_5e48f0);
}

// ---------------------------------------------------------------------------
// merchandise flags

// [FUN_40b4a0] the flags of a merchandise: a number as it is, or a list of the words formafia (2), fuel (4),
// illegal (1), cars (8) and trucks (16)
extern "C" int __cdecl Ai_MerchandiseFlags(StrRef h)
{
    int flags = 0;
    BYTE type = (BYTE)h.flags;
    if (type == 6) {
        int r = CallThis<int>(FUN_4b0ea0, h.ptr);
        CallThis(FUN_4a36c0, &h);
        return r;
    }
    if (type == 2) {
        do {
            if (Head_Is(&h, "formafia"))
                flags |= 2;
            else if (Head_Is(&h, "fuel"))
                flags |= 4;
            else if (Head_Is(&h, "illegal"))
                flags |= 1;
            else if (Head_Is(&h, "cars"))
                flags |= 8;
            else if (Head_Is(&h, "trucks"))
                flags |= 16;
            else {
                StrRef m;
                Str_Member20(&m, &h);
                const char* text = &g_aiEmpty;
                StrRef t;
                bool have = (BYTE)m.flags == 9;
                if (have) {
                    Str_Member20(&t, &h);
                    text = F(const char*, t.ptr, 12);
                }
                AiError(0, 1, s_mainFile, s_mainId, 2648, "Undefined merchandise flag (see merchandise.sch):", text,
                        &g_aiEmpty);
                if (have)
                    CallThis(FUN_4a36c0, &t);
                CallThis(FUN_4a36c0, &m);
            }
            List_Advance(&h);
        } while ((BYTE)h.flags == 2);
    }
    CallThis(FUN_4a36c0, &h);
    return flags;
}

// ---------------------------------------------------------------------------
// findPosition(vector<Hop> & hops, PositionId pos)

KING_VAR(AsmLabel, s_findPositionHops, 66e794)

static int FindPositionHops_Leave(int r)
{
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_findPositionHops);
    return r;
}

// [FUN_43ab10] findPosition: the index of the hop with pos's edge (else its node), searched from the hint m_28 first
// (as the original, only when the hint is past the end); -1 when none
extern "C" int __cdecl Ai_FindPositionHops(HopVec* hops, PositionId pos)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_findPositionHops);
    int hint = pos.m_28;
    BYTE* begin = (BYTE*)hops->begin;
    BYTE* end = (BYTE*)hops->end;
    if (pos.road != -1) {
        int e = (int)g_roadEdges[pos.road];
        int n = (int)(end - begin) / 12;
        BYTE* it;
        if (!(n >= hint) && hint >= 0) {
            it = Find12ByField4(begin + hint * 12, end, e);
            if (it == end)
                it = Find12ByField4(begin, end, e);
        } else
            it = Find12ByField4(begin, end, e);
        if (it != (BYTE*)hops->end)
            return FindPositionHops_Leave((int)(it - (BYTE*)hops->begin) / 12);
    }
    if (pos.node != -1) {
        begin = (BYTE*)hops->begin;
        end = (BYTE*)hops->end;
        int n = (int)(end - begin) / 12;
        BYTE* it;
        if (!(n >= hint) && hint >= 0) {
            it = Find12ByField0(begin + hint * 12, end, pos.node);
            if (it == end)
                it = Find12ByField0(begin, end, pos.node);
        } else
            it = Find12ByField0(begin, end, pos.node);
        if (it != (BYTE*)hops->end)
            return FindPositionHops_Leave((int)(it - (BYTE*)hops->begin) / 12);
    }
    return FindPositionHops_Leave(-1);
}

// ---------------------------------------------------------------------------
// createDealerForSale(ParkingData & parkingData, double distance)

extern "C" void FUN_418c10(); KING_ALIAS("_FUN_418c10", "_Ai_FindParkingPlace")    // a free parking place (out town, out place, parking data, double distance)
extern "C" void FUN_421a20(); KING_ALIAS("_FUN_421a20", "_Ai_CreateDealerAt")    // a new player at a position (out, PositionId32, int, int)
KING_VAR(AsmLabel, s_createDealerForSale, 66d8a4)

// [FUN_421d90] createDealerForSale: a dealer created on a free place of a parking (its town's place taken)
extern "C" void __cdecl Ai_CreateDealerForSale(int* parking, double distance)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_createDealerForSale);
    int townOut, place = -1;
    int town = *CallC<int*>(FUN_418c10, &townOut, &place, parking, distance);
    BYTE* places = F(BYTE*, (BYTE*)g_68cc98[town], 28);
    PositionId at;
    PositionInit(&at, F(int, places, place * 36));
    BYTE* d;
    CallC(FUN_421a20, &d, at, 0, -1);
    F(int, d, 100) = 256;
    parking[1]++;
    AdvanceRoad_Check(&F(PositionId, d, 40), 7816);
    F(int, d, 120) = town;
    F(int, d, 124) = place;
    BYTE* slot = F(BYTE*, (BYTE*)g_68cc98[town], 28) + place * 36;
    if (F(void*, slot, 8) != g_players)
        AiError(0, 100, s_mainFile, s_mainId, 7822, "Internal error 97725", &g_aiEmpty, &g_aiEmpty);
    slot = F(BYTE*, (BYTE*)g_68cc98[town], 28) + place * 36;
    F(BYTE*, slot, 8) = d;
    F(DWORD, slot, 4) |= 1;
    F(int, (BYTE*)g_68cc98[town], 40)++;
    F(int*, d, 972) = parking;
    BYTE* c = F(BYTE*, d, 516);
    DWORD fl = F(DWORD, c, 40) & ~3u;
    F(DWORD, c, 40) = fl;
    switch (parking[0]) {
    case 0:
        break;
    case 1:
        F(DWORD, c, 40) = fl | 1;
        break;
    case 2:
        F(DWORD, c, 40) = fl | 2;
        break;
    default:
        AiError(0, 1, s_mainFile, s_mainId, 7844, "Internal error 3020", &g_aiEmpty, &g_aiEmpty);
    }
    F(DWORD, d, 96) |= 64;
    F(DWORD, F(BYTE*, (BYTE*)g_68cc98[F(int, d, 120)], 28) + F(int, d, 124) * 36, 4) |= 1;
    F(int, F(BYTE*, d, 972), 12)++;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_createDealerForSale);
}

// ---------------------------------------------------------------------------
// accountForGuilt(PlayerId player)

KING_VAR(AsmLabel, s_accountForGuilt, 66f100)

// [FUN_442850] accountForGuilt: whether the player's orders make it carry illegal goods (+200) and is noticed for
// them (+201, by chance per order), and whether its vehicle counts as stolen (+264)
extern "C" void __cdecl Ai_AccountForGuilt(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_accountForGuilt);
    bool illegal = false, noticed = false;
    int seen = -1;
    BYTE** v = (BYTE**)Player_Orders(p, 0);
    for (BYTE* it = v[0];; it += 144) {
        v = (BYTE**)Player_Orders(p, (BYTE*)v);
        if (it == v[1])
            break;
        BYTE* tmpl = g_68ccb8 + F(int, it, 0) * 104;
        if (F(int, it, 4) != -1) {
            illegal = true;
            if ((g_68ccb8[F(int, it, 4) * 104 + 48] & 2) && (PlayerId_Over015(0, 0) & 0xff))
                noticed = true;
        }
        if ((tmpl[48] & 2) && (PlayerId_Over015(0, 0) & 0xff))
            noticed = true;
        if (!illegal) {
            if (!noticed)
                continue;
            double limit = F(double, it, 64) < 10000.0 ? 500000.0 : 200000.0;
            if ((double)Rng_Random(g_rng68b9c8, 0, 1000000) < limit)
                noticed = false;
        } else {
            if ((double)Rng_Random(g_rng68b9c8, 0, 1000000) < 200000.0) {
                noticed = false;
                illegal = false;
                continue;
            }
            if (!noticed)
                continue;
            if (F(int, it, 136) == -1) {
                AiRandom();
                F(int, it, 136) = 0;
            }
            seen = F(int, it, 136);
        }
    }
    if (illegal) {
        if (noticed && seen != 1)
            noticed = false;
        F(BYTE, p, 200) = !(noticed && seen == 1) ? 1 : 0;
    } else
        F(BYTE, p, 200) = 0;
    F(BYTE, p, 201) = noticed;
    if (F(DWORD, F(BYTE*, p, 516), 40) & 2)
        F(BYTE, p, 264) = (double)(AiRandom() % 1000000) < 500000.0;
    else
        F(BYTE, p, 264) = 0;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_accountForGuilt);
}

// ---------------------------------------------------------------------------
// getPositionIdV(RoomId room, dvector3 const & v)

KING_VAR(AsmLabel, s_getPositionIdVRoom, 66e508)

// the position of the room's road objects closest to v (with skip, not those flagged 0x20)
static PositionId* RoomPosition_Find(PositionId* out, int room, const DVec3* v, const char* name, bool skip, int line1,
                                     int line2)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, name);
    if (room < 0 || (unsigned)room >= (unsigned)(g_registry52End - g_registry52)) {
        sprintf(g_text77e460, "(%d)", room);
        AiError(33, 100, s_mainFile, s_mainId, line1, s_invalidId, "RoomId", g_text77e460);
    }
    double md = 1.7976931348623157e308, d = 0;
    int seg = 0, best = 0;
    BYTE* r = g_registry52[room];
    BYTE** objs = F(BYTE**, r, 28);
    if (objs == F(BYTE**, r, 32)) {
        out->road = -1;
        out->node = -1;
        out->m_28 = -1;
        out->t = 0;
        out->segment = 0;
        out->object = 0;
        out->m_24 = 0;
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(name);
        return out;
    }
    for (unsigned i = 0; i < (unsigned)(F(BYTE**, r, 32) - F(BYTE**, r, 28)); i++) {
        BYTE* o = F(BYTE**, r, 28)[i];
        if (!(skip && ((F(DWORD, o ? o + 8 : 0, 0) >> 5) & 1)) && CallVirt<int>(o, 12, v, &md, &d, &seg))
            best = i;
    }
    BYTE* o = F(BYTE**, g_registry52[room], 28)[best];
    PositionId p;
    p.road = F(int, o, 12);
    p.node = -1;
    p.t = d;
    p.segment = seg;
    p.object = F(int, o, 24);
    p.m_24 = 0;
    p.m_28 = -1;
    PositionId chk = p;
    if (!PositionValid(&chk)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", p.road, p.t, p.segment, p.object, 0);
        AiError(33, 100, s_mainFile, s_mainId, line2, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    *out = p;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(name);
    return out;
}

KING_VAR(AsmLabel, s_getPositionIdRoom, 66e4d8)

// [FUN_436ef0] getPositionIdV: the position of the room's road objects (not flagged 0x20) closest to v
extern "C" PositionId* __cdecl Ai_GetPositionIdVRoom(PositionId* out, int room, const DVec3* v)
{
    return RoomPosition_Find(out, room, v, s_getPositionIdVRoom, true, 14831, 14844);
}

// [FUN_436bd0] getPositionId: the position of the room's road objects closest to v
extern "C" PositionId* __cdecl Ai_GetPositionIdRoom(PositionId* out, int room, const DVec3* v)
{
    return RoomPosition_Find(out, room, v, s_getPositionIdRoom, false, 14790, 14803);
}

// ---------------------------------------------------------------------------
// getPositionIdEx(RoomId room, dvector3 const & v)

KING_VAR(AsmLabel, s_getPositionIdEx, 66e470)

// [FUN_4365d0] getPositionIdEx: the position on the room's edges closest to v (anywhere when room is -1)
extern "C" PositionId* __cdecl Ai_GetPositionIdEx(PositionId* out, int room, const DVec3* v)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_getPositionIdEx);
    if (room == -1) {
        CallC(FUN_436900, out, v);
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(s_getPositionIdEx);
        return out;
    }
    double md = 1.7976931348623157e308, d = 0;
    int seg = 0, best = 0;
    if (g_roadEdges == g_roadEdgesEnd)
        AiError(0, 100, s_mainFile, s_mainId, 14730, "No roads defined", &g_aiEmpty, &g_aiEmpty);
    bool notFound = true;
    RoadEdge** found = 0;
    BYTE* r = g_registry52[room];
    for (RoadEdge** it = F(RoadEdge**, r, 40); it != F(RoadEdge**, g_registry52[room], 44); it++) {
        RoadEdge* e = *it;
        bool hit = false;
        for (unsigned i = 0; i < (unsigned)(e->objectsEnd - e->objects); i++)
            if (CallVirt<int>(e->objects[i], 12, v, &md, &d, &seg)) {
                hit = true;
                best = i;
            }
        if (hit) {
            found = it;
            notFound = false;
        }
    }
    if (notFound)
        AiError(0, 100, s_mainFile, s_mainId, 14744, "Internal error 59812", &g_aiEmpty, &g_aiEmpty);
    PositionId p;
    p.road = F(int, *found, 12);
    p.node = -1;
    p.t = d;
    p.segment = seg;
    p.object = best;
    p.m_24 = 0;
    p.m_28 = -1;
    PositionId chk = p;
    if (!PositionValid(&chk)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", p.road, p.t, p.segment, p.object, 0);
        AiError(33, 100, s_mainFile, s_mainId, 14747, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    *out = p;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_getPositionIdEx);
    return out;
}

// ---------------------------------------------------------------------------
// findPositionOnATrajectory2

KING_VAR(AsmLabel, s_findPositionOnATrajectory2, 66ea8c)

// the hop index of a position, refound when the hop it names is not its edge or node
static void Trajectory_Index(HopVec* hops, PositionId* pos)
{
    Hop* h = hops->begin + pos->m_28;
    if (g_roadEdges[pos->road] != h->edge && pos->node != h->node)
        pos->m_28 = Ai_FindPositionHops(hops, *pos);
}

// [FUN_43d910] findPositionOnATrajectory2: the position closest to v on the player's hops between from and to
extern "C" PositionId* __cdecl Ai_FindPositionOnATrajectory2(PositionId* out, BYTE* p, const float* v, PositionId from,
                                                            PositionId to)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_findPositionOnATrajectory2);
    HopVec* hops = (HopVec*)(p + 24);
    if (to.m_28 == -1)
        to.m_28 = Ai_FindPositionHops(hops, to);
    if (from.m_28 == -1)
        from.m_28 = Ai_FindPositionHops(hops, from);
    if (to.m_28 == -1)
        AiError(0, 1, s_mainFile, s_mainId, 16173, "Error 09623", &g_aiEmpty, &g_aiEmpty);
    if (from.m_28 == -1)
        AiError(0, 1, s_mainFile, s_mainId, 16175, "Error 09624", &g_aiEmpty, &g_aiEmpty);
    Trajectory_Index(hops, &to);
    Trajectory_Index(hops, &from);
    if (to.m_28 == -1)
        AiError(0, 1, s_mainFile, s_mainId, 16189, "Error 09625", &g_aiEmpty, &g_aiEmpty);
    if (from.m_28 == -1)
        AiError(0, 1, s_mainFile, s_mainId, 16191, "Error 09626", &g_aiEmpty, &g_aiEmpty);
    PositionId r = F(PositionId, p, 40);
    DVec3 w;
    w.x = v[0];
    w.y = v[1];
    w.z = v[2];
    double md = 1.7976931348623157e308;
    Ai_FindClosestHops(&w, &md, hops, &r, from, to);
    *out = r;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_findPositionOnATrajectory2);
    return out;
}

// ---------------------------------------------------------------------------
// the rooms' edge lists

extern "C" void FUN_44d170(); KING_ALIAS("_FUN_44d170", "@DwordVec_InsertRange@20")    // vector<4 bytes>::insert(pos, first, last) (thiscall)
extern "C" DWORD* __cdecl UniqueCopyDwords(const DWORD* first, const DWORD* last, DWORD* dest);   // ai_containers.cpp
extern "C" void __cdecl Sort_InsertionDwords(unsigned* first, unsigned* last);                   // ai_containers.cpp
extern "C" void __cdecl Sort_UnguardedInsertionDwords(unsigned* first, unsigned* last);          // ai_containers.cpp
extern "C" void __cdecl Sort_IntroLoopU(unsigned* first, unsigned* last, int, int depth);        // ai_containers.cpp

static void Dwords_PushBack(DWORD* v, const DWORD* x)
{
    if (v[1] != v[2]) {
        Construct4((DWORD*)v[1], x);
        v[1] += 4;
    } else
        Vec4b_Insert44ceb0(v, 0, (DWORD*)v[1], x);
}

// [FUN_406370] each room gets the edges of its road objects, then the edges at both nodes of those, sorted and
// without duplicates
extern "C" void __cdecl Ai_CollectRoomEdges()
{
    for (RoadEdge** it = g_roadEdges; it != g_roadEdgesEnd; it++)
        for (BYTE** o = F(BYTE**, *it, 120); o != F(BYTE**, *it, 124); o++)
            Dwords_PushBack((DWORD*)(F(BYTE*, *o, 16) + 40), (const DWORD*)it);
    for (BYTE** rp = g_registry52; rp != g_registry52End; rp++) {
        DWORD tmp[3] = { 0, 0, 0 };
        for (BYTE** e = F(BYTE**, *rp, 40); e != F(BYTE**, *rp, 44); e++)
            for (int k = 112; k < 120; k += 4) {
                BYTE* n = F(BYTE*, *e, k);
                for (int side = 0; side < 2; side++)
                    for (DWORD* x = F(DWORD*, n, 20 + side * 12); x != F(DWORD*, n, 24 + side * 12); x++)
                        Dwords_PushBack(tmp, x);
            }
        BYTE* r = *rp;
        CallThis(FUN_44d170, r + 40, F(DWORD*, r, 44), (DWORD*)tmp[0], (DWORD*)tmp[1]);
        unsigned* first = F(unsigned*, r, 40);
        unsigned* last = F(unsigned*, r, 44);
        if (first != last) {
            int bytes = (BYTE*)last - (BYTE*)first;
            int depth = 0;
            for (int n = bytes >> 2; n != 1; n >>= 1)
                depth++;
            Sort_IntroLoopU(first, last, 0, depth * 2);
            if ((bytes & ~3) > 64) {
                Sort_InsertionDwords(first, first + 16);
                Sort_UnguardedInsertionDwords(first + 16, last);
            } else
                Sort_InsertionDwords(first, last);
        }
        DWORD* b = F(DWORD*, r, 40);
        DWORD* end = F(DWORD*, r, 44);
        DWORD* dup = end;
        if (b != end)
            for (DWORD* q = b; q + 1 != end; q++)
                if (q[0] == q[1]) {
                    dup = q;
                    break;
                }
        DWORD* ne = UniqueCopyDwords(dup, end, dup);
        F(DWORD*, r, 44) = F(DWORD*, r, 44) - (end - ne);
        unsigned cap = (tmp[2] - tmp[0]) & ~3u;
        if (cap)
            Ai_PoolFree((void*)tmp[0], cap);
    }
}

// ---------------------------------------------------------------------------
// goHireIt(PlayerId player)

KING_VAR(AsmLabel, s_goHireIt, 66d430)

// [FUN_419030] goHireIt: the player (without orders) heads for a free place of a market to hire itself out
extern "C" void __cdecl Ai_GoHireIt(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_goHireIt);
    BYTE** orders = (BYTE**)Player_Orders(p, 0);
    if (orders[0] != orders[1])
        AiError(0, 100, s_mainFile, s_mainId, 4914, "Internal error 742", &g_aiEmpty, &g_aiEmpty);
    Vec12_Clear(p + 24, 0);
    F(BYTE, p, 872) = 1;
    Player_EndDealOf(p);
    F(int, p, 100) = 257;
    g_68d14c++;
    int town = -1, place = -1;
    while (F(BYTE*, p, 24) == F(BYTE*, p, 28)) {
        int t;
        town = *CallC<int*>(FUN_418c10, &t, &place, &g_68ced0, 0.0);
        BYTE* slot = Vec36_At((BYTE*)g_68cc98[town] + 28, 0, place);
        PositionId to;
        PositionInit(&to, F(int, slot, 0));
        Player_GoTo(p, to, 1);
    }
    AdvanceRoad_Check(&F(PositionId, p, 40), 4934);
    F(int, p, 120) = town;
    F(int, p, 124) = place;
    BYTE* slot = F(BYTE*, (BYTE*)g_68cc98[town], 28) + place * 36;
    if (F(void*, slot, 8) != g_players)
        AiError(0, 100, s_mainFile, s_mainId, 4940, "Internal error 97726", &g_aiEmpty, &g_aiEmpty);
    slot = F(BYTE*, (BYTE*)g_68cc98[town], 28) + place * 36;
    F(BYTE*, slot, 8) = p;
    F(DWORD, slot, 4) |= 2;
    F(int, (BYTE*)g_68cc98[town], 40)++;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_goHireIt);
}

// ---------------------------------------------------------------------------
// sorting the towns, the markets and the dealers (three inlined std::sort)

extern "C" void FUN_45c150(); KING_ALIAS("_FUN_45c150", "_Sort_IntroLoop24a")    // introsort loop (first, last, 0, depth, cmp) for the towns
extern "C" void FUN_464aa0(); KING_ALIAS("_FUN_464aa0", "_Sort_InsertionBy24")    // insertion sort (first, last, cmp)
extern "C" void FUN_467a10(); KING_ALIAS("_FUN_467a10", "_Sort_UnguardedInsertionBy24")    // unguarded insertion sort (first, last, 0, cmp)
extern "C" void FUN_464cb0(); KING_ALIAS("_FUN_464cb0", "_Sort_MedianBy24")    // median of three (a, b, c, cmp) -> pointer
extern "C" void FUN_464cf0(); KING_ALIAS("_FUN_464cf0", "_Sort_PartitionBy24")    // unguarded partition (first, last, pivot, cmp)
extern "C" void FUN_45c270(); KING_ALIAS("_FUN_45c270", "_Sort_IntroLoop24b")    // introsort loop (first, last, 0, depth, cmp) for the markets
extern "C" void FUN_464b20(); KING_ALIAS("_FUN_464b20", "_Sort_Partial24")    // partial sort (first, middle, last, cmp)
extern "C" void FUN_45c390(); KING_ALIAS("_FUN_45c390", "_Sort_FinalInsertion24")    // final insertion sort (first, last, cmp)
extern "C" void FUN_464d40(); KING_ALIAS("_FUN_464d40", "_Sort_MedianBy328")    // median of three (a, b, c, cmp) -> pointer, dealers
extern "C" void FUN_46cf00(); KING_ALIAS("_FUN_46cf00", "_Swap32")    // iter_swap(a, b)
extern "C" void FUN_45c4d0(); KING_ALIAS("_FUN_45c4d0", "_Sort_IntroLoop328")    // introsort loop (first, last, 0, depth, cmp) for the dealers
extern "C" void FUN_467a60(); KING_ALIAS("_FUN_467a60", "_Sort_Partial328")    // partial sort (first, middle, last, 0, cmp)
extern "C" void FUN_467c30(); KING_ALIAS("_FUN_467c30", "_Sort_LinearInsertBy328")    // linear insert (first, it, 0, cmp)
extern "C" void FUN_467ca0(); KING_ALIAS("_FUN_467ca0", "_Sort_UnguardedInsertionBy328")    // unguarded insertion sort (first, last, 0, cmp)
extern "C" void FUN_464d90(); KING_ALIAS("_FUN_464d90", "_Sort_InsertionBy328")    // insertion sort (first, last, cmp)

static int Sort_Depth(void** first, void** last)
{
    int d = 0;
    for (int n = (int)(last - first); n != 1; n >>= 1)
        d++;
    return d * 2;
}

// [FUN_4143c0] sorts the towns, the markets and the dealers (by +328); cmp is an empty functor
extern "C" void __cdecl Ai_SortTownsMarketsDealers(int cmp)
{
    void** first = g_towns;
    void** last = g_townsEnd;
    if (first != last) {
        int bytes = (int)((BYTE*)last - (BYTE*)first);
        CallC(FUN_45c150, first, last, 0, Sort_Depth(first, last), cmp);
        if ((bytes & ~3) > 64) {
            CallC(FUN_464aa0, first, first + 16, cmp);
            CallC(FUN_467a10, first + 16, last, 0, cmp);
        } else
            CallC(FUN_464aa0, first, last, cmp);
    }

    first = g_68cc98;
    void** end = g_68cc9c;
    if (first != end) {
        int depth = Sort_Depth(first, end);
        void** l = end;
        if ((int)(((BYTE*)end - (BYTE*)first) & ~3) > 64) {
            for (;;) {
                if (depth == 0) {
                    CallC(FUN_464b20, first, l, l, cmp);
                    break;
                }
                depth--;
                int half = (int)(l - first) / 2;
                void* pivot = *CallC<void**>(FUN_464cb0, first, first + half, l - 1, cmp);
                void** cut = CallC<void**>(FUN_464cf0, first, l, pivot, cmp);
                CallC(FUN_45c270, cut, l, 0, depth, cmp);
                l = cut;
                if ((int)(((BYTE*)cut - (BYTE*)first) & ~3) <= 64)
                    break;
            }
        }
        CallC(FUN_45c390, first, end, cmp);
    }

    first = g_68cbe8;
    end = g_68cbec;
    if (first == end)
        return;
    int bytes = (int)(((BYTE*)end - (BYTE*)first) & ~3);
    if (bytes > 64) {
        int depth = Sort_Depth(first, end);
        void** l = end;
        for (;;) {
            if (depth == 0) {
                CallC(FUN_467a60, first, l, l, 0, end);
                break;
            }
            depth--;
            int half = (int)(l - first) / 2;
            BYTE* pivot = *CallC<BYTE**>(FUN_464d40, first, first + half, l - 1, end);
            int key = F(int, pivot, 328);
            void** a = first;
            void** b = l;
            for (;;) {
                while (F(int, (BYTE*)*a, 328) < key)
                    a++;
                b--;
                while (key < F(int, (BYTE*)*b, 328))
                    b--;
                if (a >= b)
                    break;
                CallC(FUN_46cf00, a, b);
                a++;
            }
            CallC(FUN_45c4d0, a, l, 0, depth, end);
            l = a;
            if ((int)(((BYTE*)a - (BYTE*)first) & ~3) <= 64)
                break;
        }
        void** mid = first + 16;
        for (void** it = first + 1; it != mid; it++)
            CallC(FUN_467c30, first, it, 0, end);
        CallC(FUN_467ca0, mid, end, 0, end);
    } else
        CallC(FUN_464d90, first, end, end);
}

// ---------------------------------------------------------------------------
// vector<4 bytes>::insert(pos, first, last)

// [FUN_44d170] vector<4 bytes>::insert(pos, first, last) (thiscall)
extern "C" void __fastcall DwordVec_InsertRange(DWORD** self, void*, DWORD* pos, const DWORD* first, const DWORD* last)
{
    if (first == last)
        return;
    unsigned n = (unsigned)(last - first);
    DWORD* finish = self[1];
    if ((unsigned)(self[2] - finish) >= n) {
        unsigned after = (unsigned)(finish - pos);
        if (after > n) {
            UninitCopyDwords(finish - n, finish, finish);
            self[1] += n;
            DWORD* src = finish - n;
            DWORD* dst = finish;
            for (int k = (int)(src - pos); k > 0; k--)
                *--dst = *--src;
            for (const DWORD* s = first; s != last; s++, pos++)
                *pos = *s;
        } else {
            UninitCopyDwords(first + after, last, finish);
            self[1] += n - after;
            self[1] = UninitCopyDwords(pos, finish, self[1]);
            for (unsigned k = 0; k < after; k++)
                pos[k] = first[k];
        }
        return;
    }
    unsigned size = (unsigned)(finish - self[0]);
    unsigned len = size + (size >= n ? size : n);
    DWORD* buf = len ? (DWORD*)Stl_Allocate(len * 4) : 0;
    DWORD* e = UninitCopyDwords(self[0], pos, buf);
    e = UninitCopyDwords(first, last, e);
    e = UninitCopyDwords(pos, self[1], e);
    unsigned cap = (unsigned)(self[2] - self[0]);
    if (cap)
        Ai_PoolFree(self[0], cap * 4);
    self[0] = buf;
    self[1] = e;
    self[2] = buf + len;
}

extern "C" BYTE* __cdecl UninitFill4(BYTE* d, unsigned n, const void* v);   // ai_containers.cpp

// [FUN_44f670] vector<4 bytes>::insert(pos, n, x) (thiscall)
extern "C" void __fastcall DwordVec_InsertFill(DWORD** self, void*, DWORD* pos, unsigned n, const DWORD* x)
{
    if (n == 0)
        return;
    DWORD* finish = self[1];
    if ((unsigned)(self[2] - finish) >= n) {
        DWORD val = *x;
        unsigned after = (unsigned)(finish - pos);
        if (after > n) {
            UninitCopyDwords(finish - n, finish, finish);
            self[1] += n;
            DWORD* src = finish - n;
            DWORD* dst = finish;
            for (int k = (int)(src - pos); k > 0; k--)
                *--dst = *--src;
            for (DWORD* d = pos; d != pos + n; d++)
                *d = val;
        } else {
            UninitFill4((BYTE*)finish, n - after, &val);
            self[1] += n - after;
            UninitCopyDwords(pos, finish, self[1]);
            self[1] += after;
            for (DWORD* d = pos; d != finish; d++)
                *d = val;
        }
        return;
    }
    unsigned size = (unsigned)(finish - self[0]);
    unsigned len = size + (size >= n ? size : n);
    DWORD* buf = len ? (DWORD*)Stl_Allocate(len * 4) : 0;
    DWORD* e = UninitCopyDwords(self[0], pos, buf);
    e = (DWORD*)UninitFill4((BYTE*)e, n, x);
    e = UninitCopyDwords(pos, self[1], e);
    unsigned cap = (unsigned)(self[2] - self[0]);
    if (cap)
        Ai_PoolFree(self[0], cap * 4);
    self[0] = buf;
    self[1] = e;
    self[2] = buf + len;
}

// ---------------------------------------------------------------------------
// the two closest free helpers (players in mode 5) head for the player

KING_VAR(AsmLabel, s_callHelpers, 66cc40)
extern "C" void FUN_45bab0(); KING_ALIAS("_FUN_45bab0", "_Holders_IntrosortByDistance")    // introsort loop (first, last, 0, depth, point24 by value): by distance to a point
extern "C" void FUN_45bbc0(); KING_ALIAS("_FUN_45bbc0", "_Near_FinalInsertion")    // final insertion sort (first, last, point24 by value)

// [FUN_40a690] collects the free players in mode 5, sorts them by distance to the player's position and sends the
// closest (at most two) there
extern "C" void __cdecl Ai_CallHelpers(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_callHelpers);
    if (F(int, p, 100) == 5) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_callHelpers);
        return;
    }
    DWORD* v[3] = { 0, 0, 0 };
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = AI_ListData(it);
        if (F(int, d, 84) != 5)
            continue;
        DWORD f = F(DWORD, d, 88);
        if (((f >> 2) & 1) || (f & 0x80000))
            continue;
        int id;
        int* x = Int_Set(&id, 0, (int)it);
        if (v[1] != v[2]) {
            Construct4(v[1], (const DWORD*)x);
            v[1]++;
        } else
            Vec4b_InsertM((DWORD*)v, 0, v[1], (const DWORD*)x);
    }
    DVec3 pt;
    DVec3 at = *Road_PointOf(&pt, F(PositionId, p, 40));
    DWORD* first = v[0];
    DWORD* last = v[1];
    if (first != last) {
        int depth = 0;
        for (int n = (int)(last - first); n != 1; n >>= 1)
            depth++;
        const DWORD* a = (const DWORD*)&at;
        CallC(FUN_45bab0, first, last, 0, depth * 2, a[0], a[1], a[2], a[3], a[4], a[5]);
        CallC(FUN_45bbc0, first, last, a[0], a[1], a[2], a[3], a[4], a[5]);
    }
    DWORD* it = v[0];
    int sent = 0;
    for (int i = 0; it != v[1]; ) {
        BYTE* h = (BYTE*)*it;
        if (sent < 2) {
            Player_GoTo(h, F(PositionId, p, 40), 2);
            if (Route_Open(h)) {
                sent++;
                F(BYTE*, h, 968) = p;
            } else {
                int node;
                CallC(FUN_40a2a0, h, *Player_NodeOf(&node, h));
            }
        } else if (F(BYTE*, h, 968) == p)
            F(void*, h, 968) = g_players;
        it++;
        if (++i >= 2)
            break;
    }
    unsigned cap = (unsigned)(v[2] - v[0]);
    if (cap)
        Ai_PoolFree(v[0], cap * 4);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_callHelpers);
}

// ---------------------------------------------------------------------------
// a list of player kinds

extern "C" void FUN_452380(); KING_ALIAS("_FUN_452380", "@IntVec_InsertRange@20")    // vector<int>::insert(pos, first, last) (thiscall)
extern "C" void __fastcall Vec4b_Insert452000(DWORD* v, void*, DWORD* pos, const DWORD* x);

static void Kinds_PushBack(DWORD* v, int kind)
{
    DWORD k = (DWORD)kind;
    if (v[1] != v[2]) {
        Construct4((DWORD*)v[1], &k);
        v[1] += 4;
    } else
        Vec4b_Insert452000(v, 0, (DWORD*)v[1], &k);
}

// [FUN_42dbf0] the player kinds of a symbol (dumb 7, ghost 8, dealer 9, trucker 2, police 5, mafia 6, none 0) or
// of a list of them
extern "C" DWORD* __cdecl Ai_ReadPlayerKinds(DWORD* out, StrRef v)
{
    DWORD kinds[3] = { 0, 0, 0 };
    int type = v.flags & 255;
    if (type == 9) {
        static const char* const names[] = { "dumb", "ghost", "dealer", "trucker", "police", "mafia", "none" };
        static const int values[] = { 7, 8, 9, 2, 5, 6, 0 };
        int i = 0;
        while (i < 7 && !SchValue_IsSymbol((const BYTE*)&v, names[i]))
            i++;
        if (i < 7)
            Kinds_PushBack(kinds, values[i]);
        else
            AiError(4, 100, s_mainFile, s_mainId, 12083, "Unsupported type", F(const char*, v.ptr, 12), &g_aiEmpty);
    } else if (type == 2) {
        do {
            StrRef head;
            Str_Member20(&head, &v);
            DWORD sub[3];
            Ai_ReadPlayerKinds(sub, head);
            CallThis(FUN_452380, kinds, (DWORD*)kinds[1], (DWORD*)sub[0], (DWORD*)sub[1]);
            Vec4_FreeStorageB((BYTE**)sub, 0);
            StrRef next;
            StrRef* nx = Str_Member32(&next, &v);
            if (v.ptr && v.Owns()) {
                if (--F(int, v.ptr, 4) == 0 && !Str_Same(&v, nx) && v.ptr)
                    CallVirt(v.ptr, 12, 1);
            }
            v.CopyFrom(nx);
            Str_Release(&next);
        } while ((BYTE)v.flags == 2);
    }
    Vec4b_Copy((BYTE*)out, 0, (const BYTE**)kinds);
    Vec4_FreeStorageB((BYTE**)kinds, 0);
    CallThis(FUN_4a36c0, &v);
    return out;
}

// ---------------------------------------------------------------------------
// createNewPlayer(int type, ..., random) with a random free vehicle

KING_VAR(AsmLabel, s_createNewPlayerRandom, 66d0ac)
KING_VAR(AsmLabel, s_noPlayersYet66d098, 66d098)
KING_VAR(AsmLabel, s_noFreeVehicle66d080, 66d080)
extern "C" void FUN_459be0(); KING_ALIAS("_FUN_459be0", "@Vec4_PushBackH@12")    // vector<4 bytes>::push_back

// [FUN_4152a0] a new player of a type with a random one of the free vehicles (Rec415740_Check) and, when that
// vehicle has trailers, a random one of them; then its place and images
extern "C" BYTE* __cdecl Ai_CreatePlayerRandom(int type, int, void* rng)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_createNewPlayerRandom);
    if (!g_68d188)
        AiError(0, 1, s_mainFile, s_mainId, 3877, (const char*)s_noPlayersYet66d098, &g_aiEmpty, &g_aiEmpty);
    BYTE* p = Players_AppendNamed(type);
    F(double, p, 128) = g_newPlayer68cfe8;
    DWORD free_[3] = { 0, 0, 0 };
    for (void** it = g_vehicles; it != g_vehiclesEnd; it++)
        if (Rec415740_Check((BYTE*)*it)) {
            void** x = it;
            CallThis(FUN_459be0, free_, &x);
        }
    void** pick = g_vehiclesEnd;
    if (free_[0] != free_[1])
        pick = ((void***)free_[0])[Rng_Random(rng, 0, (free_[1] - free_[0]) / 4)];
    CallThis(FUN_447970, free_);
    if (pick == g_vehiclesEnd)
        AiError(0, 1, s_mainFile, s_mainId, 3885, (const char*)s_noFreeVehicle66d080, &g_aiEmpty, &g_aiEmpty);
    int idx = (int)(pick - g_vehicles);
    BYTE* veh = (BYTE*)g_vehicles[idx];
    F(BYTE*, veh, 0)[g_68a860] = 0;
    BYTE* o = (BYTE*)operator new(48);
    if (o)
        o = Rec415100_Construct(o, 0, idx);
    F(BYTE*, p, 508) = o;
    veh = (BYTE*)g_vehicles[idx];
    int* tb = F(int*, veh, 96);
    int* te = F(int*, veh, 100);
    if (tb != te) {
        unsigned r = AiRandom() % (unsigned)(te - tb);
        BYTE* t = (BYTE*)operator new(48);
        if (t) {
            int v;
            t = Rec415100_Construct(t, 0, *AI_StoreInt(&v, F(int*, (BYTE*)g_vehicles[idx], 96)[r]));
        }
        F(BYTE*, p, 512) = t;
    }
    F(int, p, 84) = type;
    CallThis(FUN_474160, p, CallC<int>(FUN_4148a0, p));
    PodVec_Assign((BYTE**)(p + 264), 0, (BYTE* const*)(g_images68a958 + F(int, p, 984) * 56 + 4));
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_createNewPlayerRandom);
    return p;
}

// ---------------------------------------------------------------------------
// switching an ambush on or off

KING_VAR(AsmLabel, s_setAmbushActive, 66dc78)
extern "C" void FUN_45f2c0(); KING_ALIAS("_FUN_45f2c0", "_Nodes_SortByDistance")    // sort (first, last, point24 by value): by distance to the point
extern "C" int __cdecl Player_Call477c30(BYTE* p);

// an ambush without a player gets the free player (of the kind) farthest along the sort by distance to g_68bab0,
// placed at the ambush and flagged 0x80000
static void Ambush_Attach(BYTE* amb, DWORD* (__cdecl* select)(DWORD*, BYTE*, BYTE*))
{
    BYTE** slot = &F(BYTE*, amb, 124);
    if (*slot != (BYTE*)g_players)
        return;
    BYTE* end;
    BYTE* begin;
    CallThis(FUN_4afc20, &g_players, &end);
    CallThis(FUN_4afbf0, &g_players, &begin);
    DWORD v[3];
    select(v, begin, end);
    DVec3 c = g_68bab0;
    const DWORD* a = (const DWORD*)&c;
    CallC(FUN_45f2c0, v[0], v[1], a[0], a[1], a[2], a[3], a[4], a[5]);
    if (v[0] != v[1]) {
        int id;
        int* x = Int_Set(&id, 0, *(int*)(v[1] - 4));
        int rec[4];
        Rec_InitFromPtr(rec, 0, x);
        Rec42bfe0_Assign((int*)slot, 0, rec);
        CallThis(FUN_404420, rec + 1);
        BYTE* p = (BYTE*)id;
        Player_Place(p, F(PositionId, amb, 176));
        Player_Call477c30(p);
        F(DWORD, p, 96) |= 0x80000;
    }
    CallThis(FUN_447970, v);
}

// [FUN_42bb90] setAmbushActive: on, an inactive ambush takes a player (kind 2: of kind 5, kind 1: of kind 6) and
// its lights go on; off, an active one releases its player and its lights go off
extern "C" int __cdecl Ai_SetAmbushActive(BYTE* amb, bool on)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_setAmbushActive);
    int kind = F(int, amb, 36);
    if (on) {
        if (!F(BYTE, amb, 28)) {
            if (kind == 2) {
                Ambush_Attach(amb, Select_FreeNodes5);
                for (unsigned* id = F(unsigned*, amb, 112); id != F(unsigned*, amb, 116); id++)
                    Screen_569310(*id % 1000, 0);
            } else if (kind == 1) {
                Ambush_Attach(amb, Select_FreeNodes6);
                for (unsigned* id = F(unsigned*, amb, 112); id != F(unsigned*, amb, 116); id++)
                    Screen_569410(*id % 1000, 0);
            }
        }
    } else if (F(BYTE, amb, 28)) {
        Ai_ReleaseAttachedPlayer(amb);
        kind = F(int, amb, 36);
        if (kind == 2) {
            for (unsigned* id = F(unsigned*, amb, 112); id != F(unsigned*, amb, 116); id++)
                Screen_569310(*id % 1000, 0);
        } else if (kind == 1)
            for (unsigned* id = F(unsigned*, amb, 112); id != F(unsigned*, amb, 116); id++)
                Screen_569410(*id % 1000, 0);
    }
    F(bool, amb, 28) = on;
    Obj_483c10(amb ? amb + 12 : 0);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_setAmbushActive);
    return 0;
}

// ---------------------------------------------------------------------------
// removing a player from the game

KING_VAR(AsmLabel, s_removePlayer, 66f3d4)
extern "C" void __cdecl Players_SetCurrent(BYTE* p);   // ai_drivers.cpp
extern "C" int __cdecl Players_Call478980(int x);      // ai_drivers.cpp

// [FUN_4466a0] a player leaves the game (once: flag 0x40000): its deals and waits end, no vehicle, ambush or
// owner refers to it any more, then it is unregistered
extern "C" void __cdecl Ai_RemovePlayer(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_removePlayer);
    if (Player_NotValid(p)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 18237, s_invalidId, s_playerIdType, g_numberText);
    }
    if (F(DWORD, p, 96) & 0x40000) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_removePlayer);
        return;
    }
    F(DWORD, p, 96) |= 0x40000;
    CallC(FUN_4015a0, p, 7, 0);
    CallC(FUN_419760, p);
    Dealer_CancelSelling(p);
    if (Player_UpdateFlagsGet(p))
        g_68d15c--;
    Player_Leave(p, 0);
    Player_StopWaiting(p);
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        if (!AI_ListData(it))
            continue;
        Ai_ClearPlayerId(F(BYTE*, AI_ListData(it), 508), (int)p);
        Ai_ClearPlayerId(F(BYTE*, AI_ListData(it), 512), (int)p);
    }
    for (BYTE* it = *(BYTE**)g_68cf6c; it != (BYTE*)g_68cf6c; it = *(BYTE**)it)
        Ai_ClearPlayerId(F(BYTE*, *(BYTE**)Iter_Data(&it, 0), 20), (int)p);
    for (void** e = g_68a970; e != g_68a974; e++) {
        BYTE* amb = (BYTE*)*e;
        if (F(BYTE*, amb, 124) != p)
            continue;
        DWORD tmp[3] = { 0, 0, 0 };
        F(void*, amb, 124) = g_players;
        PodVec_Assign((BYTE**)(amb + 128), 0, (BYTE* const*)tmp);
        CallThis(FUN_44b090, tmp);
    }
    if (F(void*, p, 176) != g_players)
        CallC(FUN_476f70, F(int, p, 176), p, 1);
    else {
        BYTE** v = &F(BYTE*, p, 160);
        if (v[0] != v[1])
            while (v[0] != v[1])
                CallC(FUN_476f70, p, *(int*)(v[1] - 16), 0);
    }
    if (Player_IsHuman(p))
        Players_SetCurrent((BYTE*)g_players);
    CallThis(FUN_474160, p + 8, -1);
    Player_Unregister(p);
    Players_Call478980((int)p);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_removePlayer);
}

// ---------------------------------------------------------------------------
// getOrderList(PlayerId player, AI_Town* town, OrderList& list)

KING_VAR(AsmLabel, s_getOrderList, 66ee8c)
extern "C" void FUN_452e80(); KING_ALIAS("_FUN_452e80", "@OrderInfoVec_InsertAux@16")    // vector<192 bytes>::insert_aux (thiscall pos, x)
extern "C" double __cdecl AI_Time();   // ai_task.cpp

static void Orders_PushBack(BYTE** v, const BYTE* oi)
{
    if (v[1] != v[2]) {
        Rec461400_Construct(v[1], oi);
        v[1] += 192;
    } else
        CallThis(FUN_452e80, v, v[1], oi);
}

// [FUN_440970] getOrderList: the town's own orders, then up to 4 random takeable orders to other towns (40 tries,
// each town and order once)
extern "C" int __cdecl Ai_GetOrderList(int player, BYTE* town, BYTE* list)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_getOrderList);
    BYTE** orders = (BYTE**)(list + 20);
    F(int, list, 40) = F(int, town, 96);
    Vec192_Erase(orders, 0, orders[0], orders[1]);
    F(int, list, 36) = 0;
    F(int, list, 0) = F(int, town, 12);
    F(int, list, 4) = F(int, town, 24);
    Vec3f c;
    const Vec3f* pc = Ai_GetCoordinates(&c, town);
    memcpy(list + 8, pc, 12);
    BYTE oi[192];
    for (int i = 0; ; i++) {
        Fx_InitState(oi, 0);
        int r = Ai_GetOrderInfo(player, F(int, town, 24), i, oi);
        if (r == -1) {
            CallThis(FUN_440dd0, oi);
            break;
        }
        if (r == 0)
            Orders_PushBack(orders, oi);
        CallThis(FUN_440dd0, oi);
    }
    BYTE rng[228];
    Rng_Seed(rng, 0, Ftol(AI_Time() * 0.0002));
    DWORD* seen[3] = { 0, 0, 0 };
    DWORD* b = (DWORD*)Vec4b_AllocCopy(40, 0, 0);
    seen[0] = b;
    seen[1] = b;
    seen[2] = b + 40;
    int found = 0;
    for (int k = 0; k < 40; k++) {
        int to;
        Town_Find3(&to, (int)town, (int)rng);
        if (to == -1)
            continue;
        int r = (int)Rng_Random(rng, 0, 9);
        if ((BYTE*)g_towns[to] == town)
            continue;
        DWORD key = (DWORD)(r + to * 1000);
        DWORD* it = seen[0];
        while (it != seen[1] && *it != key)
            it++;
        if (it != seen[1])
            continue;
        if (seen[1] != seen[2]) {
            Construct4(seen[1], &key);
            seen[1]++;
        } else
            Vec4b_Insert452000((DWORD*)seen, 0, seen[1], &key);
        Fx_InitState(oi, 0);
        Ai_GetOrderInfoIdx((BYTE*)player, to, r, oi);
        if (oi[184]) {
            Orders_PushBack(orders, oi);
            if (++found >= 4) {
                CallThis(FUN_440dd0, oi);
                break;
            }
        }
        CallThis(FUN_440dd0, oi);
    }
    F(int, list, 32) = (int)(orders[1] - orders[0]) / 192;
    F(BYTE*, list, 36) = orders[0];
    unsigned cap = (unsigned)(seen[2] - seen[0]);
    if (cap)
        Ai_PoolFree(seen[0], cap * 4);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_getOrderList);
    return 0;
}

// ---------------------------------------------------------------------------
// calculateWeigthsPerSubstate()

KING_VAR(AsmLabel, s_calculateWeigthsPerSubstate, 66d178)
extern "C" void __cdecl Road_NodeCost(int side, int from, int to, double* cost);   // route_search.cpp

// [FUN_4165f0] calculateWeigthsPerSubstate: the race's 9 legs between the three checkpoints (direction by
// g_66c6e4: 1 forward, 2 backward, else none), then each entry becomes the cost of the rest of the race
extern "C" void __cdecl Ai_CalculateWeightsPerSubstate()
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_calculateWeigthsPerSubstate);
    int cp[3];
    Node_ByName(&cp[0], "check_point1");
    Node_ByName(&cp[1], "check_point2");
    Node_ByName(&cp[2], "check_point3");
    double* d = g_raceTimes68cf70;
    if (g_66c6e4 == 1 || g_66c6e4 == 2)
        for (int i = 0; i < 9; i++) {
            int a = g_66c6e4 == 1 ? i % 3 : (3 - i % 3) % 3;
            int b = g_66c6e4 == 1 ? (i + 1) % 3 : (5 - i % 3) % 3;
            double c = 0;
            Road_NodeCost(0, cp[a], cp[b], &c);
            d[i] = c;
        }
    d[9] = 0;
    d[10] = 0;
    d[11] = 0;
    for (int i = 10; i >= 0; i--)
        d[i] = d[i + 1] + d[i];
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_calculateWeigthsPerSubstate);
}

// ---------------------------------------------------------------------------
// sending players to towns, nodes and markets

extern "C" void FUN_45e0a0(); KING_ALIAS("_FUN_45e0a0", "_Player_RandomWaiting")    // a random free player (out, first, last, -, rng)
extern "C" void FUN_43e610(); KING_ALIAS("_FUN_43e610", "_Ai_SetPlayerPositionTo")    // (player, RoadNode position +48, 0)
extern "C" void FUN_46dc50(); KING_ALIAS("_FUN_46dc50", "_Net_SendPlayer")    // multiplayer: announce a new player (player)
KING_VAR(BYTE, g_68d31c, 68d31c)
extern "C" void __cdecl Ai_GoHireIt(BYTE* p);

// [FUN_423ad0] while there are fewer players for hire than wanted (at most 100 tries), a random free player is
// placed at its position and goes hiring
extern "C" void __cdecl Ai_FillHirePlaces(int)
{
    if (g_68d188)
        return;
    int cmp = 0;
    for (int n = 0; g_68d14c < g_68d150; ) {
        n++;
        BYTE* end;
        BYTE* begin;
        CallThis(FUN_4afc20, &g_players, &end);
        CallThis(FUN_4afbf0, &g_players, &begin);
        BYTE* r;
        CallC(FUN_45e0a0, &r, begin, end, cmp, &g_rng68b9c8);
        BYTE* p = (BYTE*)g_players;
        if (r != (BYTE*)g_players) {
            int id;
            p = (BYTE*)*Int_Set(&id, 0, (int)r);
        }
        if (p == (BYTE*)g_players)
            return;
        Player_Place(p, F(PositionId, p, 40));
        Ai_GoHireIt(p);
        int node;
        CallC(FUN_43e610, p, (BYTE*)NodeId_Node(Player_NodeOf(&node, p), 0) + 48, 0);
        CallC(AI_Step3, 104);
        if (n >= 100)
            return;
    }
}

extern "C" void __fastcall Player_SetPosition(void* self, void*, const PositionId* p);
extern "C" void __cdecl Ai_TrimDealerPosition(PositionId* src);
extern "C" void __cdecl Player_ClearDeadline(BYTE* p);
extern "C" void __cdecl Rec_4015a0Both(int id);

// [FUN_421a20] a new dealer (kind 9) at a position, trimmed to a dealer place; registered (and announced in a
// network game)
extern "C" int* __cdecl Ai_CreateDealerAt(int* out, PositionId pos, int, int veh)
{
    BYTE* p = Ai_CreateNewPlayer(9, 0, veh);
    DWORD reg;
    int id = (int)*Player_Register(&reg, p);
    Ai_TrimDealerPosition(&pos);
    Player_SetPosition(p, 0, &pos);
    Player_ClearDeadline(p);
    AdvanceRoad_Check(&F(PositionId, (BYTE*)id, 40), 7769);
    if (g_68d31c)
        CallC(FUN_46dc50, p);
    Rec_4015a0Both(id);
    *out = id;
    return out;
}

// [FUN_417ed0] a player without orders heads for the town of its goods
extern "C" void __cdecl Ai_GoToGoodsTown(BYTE* p)
{
    Vec12_Clear(p + 24, 0);
    F(BYTE, p, 872) = 1;
    if (!Player_FirstGoods(p + 8))
        AiError(356, 1, s_mainFile, s_mainId, 4480, "Internal error 356", &g_aiEmpty, &g_aiEmpty);
    BYTE* town = (BYTE*)g_towns[F(int, (BYTE*)Player_Driver(p), 12)];
    int node;
    Town_Id(&node, town);
    PositionId to;
    PositionInit(&to, node);
    Player_GoTo(p, to, 1);
    AdvanceRoad_Check(&F(PositionId, p, 40), 4485);
}

// [FUN_40a2a0] a player without orders heads for a node; until it has orders, the node nearest to (node, player)
// is tried
extern "C" void __cdecl Ai_GoToNode(BYTE* p, int node)
{
    Vec12_Clear(p + 24, 0);
    F(BYTE, p, 872) = 1;
    while (F(BYTE*, p, 24) == F(BYTE*, p, 28)) {
        int pair[2];
        int pp = (int)p;
        Pair_Set(pair, 0, &node, &pp);
        int n;
        Nodes_Find4558e0(&n, pair[0], pair[1], (int)&g_rng68b9c8);
        if (n == -1)
            AiError(0, 100, s_mainFile, s_mainId, 2283, "Can not find node to trace to", &g_aiEmpty, &g_aiEmpty);
        PositionId to;
        PositionInit(&to, n);
        Player_GoTo(p, to, 1);
    }
    AdvanceRoad_Check(&F(PositionId, p, 40), 2286);
}

// ---------------------------------------------------------------------------
// a new player's image

extern "C" void FUN_45c8d0(); KING_ALIAS("_FUN_45c8d0", "_Sort28_StableSort")    // random_shuffle (first, last, 0, 0, rng-like player)
extern "C" void FUN_4654c0(); KING_ALIAS("_FUN_4654c0", "_Sort32_StableSortAdaptive")    // stable sort with a buffer (first, last, buf, len, cmp)
extern "C" void FUN_462410(); KING_ALIAS("_FUN_462410", "@Vec_FreeStorage8@8")    // temporary buffer destructor (thiscall)
extern "C" int* __fastcall TempBuf_Construct(int* self, void*, BYTE* first, BYTE* last);
extern "C" void __cdecl Sort_Stable32(BYTE** first, BYTE** last, int);
extern "C" DWORD* __cdecl Select_Records56(DWORD* out, BYTE* first, BYTE* last, int a, int b);

// [FUN_4148a0] the image of a new player: one of the images of its kind's class (1 for most, 2 for mafia, 3 for
// police), shuffled for some kinds, sorted for others; -1 when there is none
extern "C" int __cdecl Ai_PickImage(BYTE* p)
{
    int cls = -2;
    switch (F(int, p, 84)) {
    case 1: case 2: case 7: case 8: case 9: cls = 1; break;
    case 5: cls = 3; break;
    case 6: cls = 2; break;
    default:
        AiError(0, 1, s_mainFile, s_mainId, 3742, "Internal error 0374", &g_aiEmpty, &g_aiEmpty);
    }
    DWORD v[3];
    Select_Records56(v, g_images68a958, g_images68a95c, cls, -1);
    if (v[0] == v[1]) {
        AiError(0, 256, s_mainFile, s_mainId, 3749, "No identity found", &g_aiEmpty, &g_aiEmpty);
        unsigned cap = (v[2] - v[0]) & ~3u;
        if (cap)
            Ai_PoolFree((void*)v[0], cap);
        return -1;
    }
    switch (F(int, p, 84)) {
    case 1: case 2: case 5: case 6:
        CallC(FUN_45c8d0, v[0], v[1], 0, 0, p);
        break;
    case 7: case 8: case 9: {
        int tb[4];
        TempBuf_Construct(tb, 0, (BYTE*)v[0], (BYTE*)v[1]);
        if (tb[2] == 0)
            Sort_Stable32((BYTE**)v[0], (BYTE**)v[1], (int)p);
        else
            CallC(FUN_4654c0, v[0], v[1], tb[2], tb[1], p);
        CallThis(FUN_462410, tb);
        break;
    }
    default:
        AiError(0, 1, s_mainFile, s_mainId, 3770, "Internal error 0374", &g_aiEmpty, &g_aiEmpty);
    }
    int idx = (int)(*(BYTE**)v[0] - g_images68a958) / 56;
    unsigned cap = (v[2] - v[0]) & ~3u;
    if (cap)
        Ai_PoolFree((void*)v[0], cap);
    return idx;
}

// ---------------------------------------------------------------------------
// a player without orders gets one

KING_VAR(AsmLabel, s_giveOrder66d304, 66d304)
extern "C" void FUN_417820(); KING_ALIAS("_FUN_417820", "_Ai_ArriveAt")    // a player's arrival at a position (player, PositionId32)
extern "C" void FUN_4180f0(); KING_ALIAS("_FUN_4180f0", "_Ai_GoToOrder")    // sends a player to a town's order (player, town, order)
extern "C" void* __cdecl Player_CopyState(void* out, BYTE* p);
extern "C" int __cdecl Ai_RandomOrder(int player, int town);

// [FUN_4185d0] a trucker (+92 == 2) with goods heads for their town (state 4); one without takes a random order
// of the nearest free town (state 3), arriving where it is when there is none; others just arrive
extern "C" void __cdecl Ai_TruckerNextOrder(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_giveOrder66d304);
    if (F(int, p, 92) != 2) {
        CallC(FUN_417820, p, F(PositionId, p, 40));
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_giveOrder66d304);
        return;
    }
    if (!Player_FirstGoods(p + 8)) {
        BYTE st[64];
        int t;
        int town = *Town_NearestFree(&t, (const DVec3*)Player_CopyState(st, p));
        F(int, p, 128) = F(int, (BYTE*)g_towns[town], 24);
        int r = Ai_RandomOrder((int)p, F(int, p, 128));
        BYTE* it = p;
        F(int, Iter_Data(&it, 0), 124) = r;
        if (F(int, p, 132) == -1) {
            CallC(FUN_417820, p, F(PositionId, p, 40));
            Trace_Tag((const DWORD*)g_tagMpl);
            Trace_CopyName(s_giveOrder66d304);
            return;
        }
        CallC(FUN_4180f0, p, F(int, p, 128), F(int, p, 132));
        F(int, p, 100) = 3;
    } else {
        Ai_GoToGoodsTown(p);
        F(int, p, 100) = 4;
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_giveOrder66d304);
}

// ---------------------------------------------------------------------------
// the AI event hook

KING_VAR(AsmLabel, s_aiEvent66c7f8, 66c7f8)
KING_VAR(BYTE, g_68d114, 68d114)
KING_VAR(BYTE, g_68d115, 68d115)
typedef int (__cdecl* AiEventHook)(BYTE* p, int kind, int x);
KING_VAR(AiEventHook, g_aiEventHook68d118, 68d118)

// [FUN_4015a0] reports an event of a player to the hook (once per player for kind 1; others only after that
// and while not suppressed); none while disabled or for a removed player (except kind 7)
extern "C" int __cdecl Ai_PlayerEvent(BYTE* p, int kind, int x)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_aiEvent66c7f8);
    if (g_68d115 || (kind != 7 && (F(DWORD, p, 96) & 0x40000))) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_aiEvent66c7f8);
        return 0;
    }
    if (kind == 1) {
        if (F(BYTE, p, 800))
            AiError(0, 1, s_mainFile, s_mainId, 186, "Internal error 04721", &g_aiEmpty, &g_aiEmpty);
        else {
            F(BYTE, p, 800) = 1;
            int r = g_aiEventHook68d118(p, 1, x);
            Trace_Tag((const DWORD*)g_tagMpl);
            Buf_AppendString(&g_trace, 0, s_aiEvent66c7f8);
            return r;
        }
    } else if (F(BYTE, p, 800) && !g_68d114) {
        int r = g_aiEventHook68d118(p, kind, x);
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(s_aiEvent66c7f8);
        return r;
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Buf_AppendString(&g_trace, 0, s_aiEvent66c7f8);
    return 0;
}

// ---------------------------------------------------------------------------
// loading the world files

extern "C" void __cdecl Ai_LoadWorld(const char* file);

// [FUN_4140f0] loads every mod_*.sc file of the world directory, sorted by name
extern "C" void __cdecl Ai_LoadWorldFiles()
{
    BYTE* files[3] = {0, 0, 0};
    char alloc;
    BYTE* path[3];
    unsigned dn = (unsigned)(g_wayDirEnd68bacc - g_wayDir68bac8);
    CallThis(FUN_44a490, path, dn, &alloc);
    memmove(path[0], g_wayDir68bac8, dn);
    path[1] = path[0] + dn;
    char pattern[10] = {'m', 'o', 'd', '_', '*', '.', 's', 'c', 'h', 0};
    CallThis(FUN_44c220, path, path[1], pattern, pattern + 10);
    WIN32_FIND_DATAA fd;
    HANDLE fh = FindFirstFileA((const char*)path[0], &fd);
    if (fh != INVALID_HANDLE_VALUE) {
        do {
            BYTE* name[3] = {0, 0, 0};
            size_t n = strlen(fd.cFileName);
            CallThis(FUN_44c220, name, (BYTE*)0, fd.cFileName, fd.cFileName + n + 1);
            if (files[1] != files[2]) {
                CallC(FUN_4611e0, files[1], &g_wayDir68bac8);
                files[1] += 12;
            } else
                CallThis(FUN_452610, files, files[1], &g_wayDir68bac8);
            BYTE* last = files[1] - 12;
            CallThis(FUN_44c220, last, F(BYTE*, last, 4), name[0], name[1]);
            if (name[2] != name[0])
                Ai_PoolFree(name[0], name[2] - name[0]);
        } while (FindNextFileA(fh, &fd));
        FindClose(fh);
    }
    if (files[0] != files[1]) {
        int k = 0;
        for (int n = (int)(files[1] - files[0]) / 12; n != 1; n >>= 1)
            k++;
        CallC(FUN_45b3d0, files[0], files[1], 0, k * 2, 0);
        CallC(FUN_45b4e0, files[0], files[1], 0);
    }
    for (BYTE* f = files[0]; f != files[1]; f += 12)
        Ai_LoadWorld(F(const char*, f, 0));
    if (path[2] != path[0])
        Ai_PoolFree(path[0], path[2] - path[0]);
    for (BYTE* f = files[0]; f != files[1]; f += 12)
        CallThis(FUN_404420, f);
    unsigned cnt = (unsigned)(files[2] - files[0]) / 12;
    if (cnt)
        Ai_PoolFree(files[0], cnt * 12);
}

// ---------------------------------------------------------------------------
// arrivals and order trips

KING_VAR(AsmLabel, s_arrival66d224, 66d224)
KING_VAR(AsmLabel, s_goToOrder66d2bc, 66d2bc)

// [FUN_417820] a player (without orders) at a position heads for the node nearest to the position's node's
// town, retried until it has orders
extern "C" void __cdecl Ai_ArriveAt(BYTE* p, PositionId pos)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_arrival66d224);
    Vec12_Clear(p + 24, 0);
    F(BYTE, p, 872) = 1;
    int town = 0;
    if (pos.node != -1)
        town = F(int, g_68cf00[pos.node], 12);
    while (F(BYTE*, p, 24) == F(BYTE*, p, 28)) {
        int pair[2];
        int pp = (int)p;
        Pair_SetFromPtr(pair, 0, town, &pp);
        int n;
        Nodes_Find456140(&n, pair[0], pair[1], (int)&g_rng68b9c8);
        if (n == -1)
            AiError(0, 100, s_mainFile, s_mainId, 4442, "Can not find node to trace to", &g_aiEmpty, &g_aiEmpty);
        PositionId to;
        PositionInit(&to, n);
        Player_GoTo(p, to, 1);
    }
    AdvanceRoad_Check(&F(PositionId, p, 40), 4445);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_arrival66d224);
}

// [FUN_4180f0] a player (without orders) heads for the place of an order of a town (nothing for an order the
// town does not have)
extern "C" void __cdecl Ai_GoToOrder(BYTE* p, int townId, unsigned order)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_goToOrder66d2bc);
    Vec12_Clear(p + 24, 0);
    F(BYTE, p, 872) = 1;
    int idx;
    Town_ById(&idx, townId);
    BYTE* t = (BYTE*)g_towns[idx];
    if ((unsigned)((F(BYTE*, t, 32) - F(BYTE*, t, 28)) >> 4) <= order) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_goToOrder66d2bc);
        return;
    }
    PositionId to;
    PositionInit(&to, F(int, F(BYTE*, t, 28), order * 16));
    Player_GoTo(p, to, 1);
    AdvanceRoad_Check(&F(PositionId, p, 40), 4503);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_goToOrder66d2bc);
}

// ---------------------------------------------------------------------------
// starting the AI world

extern "C" void FUN_423a90(); KING_ALIAS("_FUN_423a90", "_Ai_CreatePlayers")
KING_VAR(BYTE, g_66f62b, 66f62b)
extern "C" void __cdecl Markets_GrowOnce();

// [FUN_423c00] the markets grow, the hire places fill, then every player's position is checked (and a player
// without orders must not have waypoints)
extern "C" void __cdecl Ai_StartWorld()
{
    CallC(FUN_423a90);
    g_66f62b = 0;
    r_66c68c = 0;
    Markets_GrowOnce();
    Ai_FillHirePlaces(0);
    g_66f62b = 1;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = AI_ListData(it);
        if (!d)
            continue;
        AdvanceRoad_Check(&F(PositionId, d, 32), 8386);
        d = AI_ListData(it);
        if (F(BYTE*, d, 16) == F(BYTE*, d, 20) && F(BYTE*, d, 64) != F(BYTE*, d, 68))
            AiError(0, 1, s_mainFile, s_mainId, 8392, "Internal error 325700", &g_aiEmpty, &g_aiEmpty);
    }
}

// ---------------------------------------------------------------------------
// the two closest free mafia players (kind 6) head for the player

KING_VAR(AsmLabel, s_callMafia66cc5c, 66cc5c)

// [FUN_40aa00] collects the free players of kind 6, sorts them by distance to the player's position and sends the
// first two there (they remember the player at +968)
extern "C" void __cdecl Ai_CallMafia(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_callMafia66cc5c);
    if (F(int, p, 100) == 5) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_callMafia66cc5c);
        return;
    }
    DWORD* v[3] = { 0, 0, 0 };
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = AI_ListData(it);
        if (F(int, d, 84) != 6)
            continue;
        DWORD f = F(DWORD, d, 88);
        if (((f >> 2) & 1) || (f & 0x80000))
            continue;
        int id;
        int* x = Int_Set(&id, 0, (int)it);
        if (v[1] != v[2]) {
            Construct4(v[1], (const DWORD*)x);
            v[1]++;
        } else
            Vec4b_InsertM((DWORD*)v, 0, v[1], (const DWORD*)x);
    }
    DVec3 pt;
    DVec3 at = *Road_PointOf(&pt, F(PositionId, p, 40));
    DWORD* first = v[0];
    DWORD* last = v[1];
    if (first != last) {
        int depth = 0;
        for (int n = (int)(last - first); n != 1; n >>= 1)
            depth++;
        const DWORD* a = (const DWORD*)&at;
        CallC(FUN_45bab0, first, last, 0, depth * 2, a[0], a[1], a[2], a[3], a[4], a[5]);
        CallC(FUN_45bbc0, first, last, a[0], a[1], a[2], a[3], a[4], a[5]);
    }
    DWORD* it = v[0];
    for (int i = 0; it != v[1] && i < 2; i++, it++) {
        BYTE* h = (BYTE*)*it;
        Player_GoTo(h, F(PositionId, p, 40), 2);
        F(BYTE*, h, 968) = p;
    }
    unsigned cap = (unsigned)(v[2] - v[0]);
    if (cap)
        Ai_PoolFree(v[0], cap * 4);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_callMafia66cc5c);
}

// ---------------------------------------------------------------------------
// a player goes idle

KING_VAR(AsmLabel, s_goIdle66d470, 66d470)
extern "C" int __cdecl Player_Flag4(BYTE* p);
extern "C" BYTE* __fastcall Vec12_Erase(BYTE* self, void*, BYTE* first, BYTE* last);
extern "C" int* __cdecl Road_NearestNode(int* out, const DVec3* p);

// [FUN_419760] a player (not a kind-9 one) leaves its hire or sale state and its market place, and (unless human)
// heads for its goods' town, its next order or the nearest node
extern "C" int __cdecl Ai_GoIdle(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_goIdle66d470);
    if (F(int, p, 92) == 9) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_goIdle66d470);
        return 0;
    }
    if (F(int, p, 100) == 256) {
        BYTE* r = F(BYTE*, p, 972);
        F(int, r, 4)--;
        if ((BYTE)Player_Flag4(p))
            F(int, F(BYTE*, p, 972), 12)--;
    }
    if (F(int, p, 100) == 257) {
        g_68d14c--;
        if ((BYTE)Player_Flag4(p))
            g_68d154--;
    }
    Vec12_Clear(p + 24, 0);
    F(BYTE, p, 872) = 1;
    F(int, p, 100) = 0;
    F(DWORD, p, 96) &= 0xfffffbbf;
    Player_EndDealOf(p);
    int m = F(int, p, 120);
    if (m != -1) {
        F(int, p, 120) = -1;
        BYTE* slot = F(BYTE*, (BYTE*)g_68cc98[m], 28) + F(int, p, 124) * 36;
        F(void*, slot, 8) = g_players;
        slot = F(BYTE*, (BYTE*)g_68cc98[m], 28) + F(int, p, 124) * 36;
        F(DWORD, slot, 4) &= ~3u;
        F(int, (BYTE*)g_68cc98[m], 40)--;
    }
    F(int, p, 972) = 0;
    if (F(int, p, 92) == 1) {
        Vec12_Erase(p + 24, 0, F(BYTE*, p, 24), F(BYTE*, p, 28));
        F(BYTE, p, 872) = 1;
        Waypoints_Clear(p + 72, 0);
        Trace_Tag((const DWORD*)g_tagMpl);
        Buf_AppendString(&g_trace, 0, s_goIdle66d470);
        return 0;
    }
    DVec3 pt;
    Road_PointOf(&pt, F(PositionId, p, 40));
    if (!Player_FirstGoods(p + 8)) {
        if (F(int, p, 92) == 2)
            Ai_TruckerNextOrder(p);
        else {
            int n;
            Ai_GoToNode(p, *Road_NearestNode(&n, &pt));
        }
    } else {
        Ai_GoToGoodsTown(p);
        F(int, p, 100) = 4;
    }
    Player_Call477bd0(p, 0, 0x40340000);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_goIdle66d470);
    return 0;
}

// ---------------------------------------------------------------------------
// findAppropriateParkingPlace(int& index, ParkingData& parkingData, double distance)

KING_VAR(AsmLabel, s_findParkingPlace66d378, 66d378)
extern "C" void* __cdecl Recs_Search418bb0(void* out, BYTE* v, const int* key, int b, int c);
extern "C" int* __cdecl Parking_RandomFree(int* market, int* point);

// [FUN_418c10] the market with the fewest used places (below 1000, not full) that has a matching free place;
// a random one of its matching places (index); a random free place of any market when that one has none
extern "C" int* __cdecl Ai_FindParkingPlace(int* market, int* place, const int* key, int b, int c)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_findParkingPlace66d378);
    int best = -1;
    int fewest = 1000;
    for (void** m = g_68cc98; m != g_68cc9c; m++) {
        DWORD v[3];
        Recs_Search418bb0(v, (BYTE*)*m, key, b, c);
        if (v[0] != v[1]) {
            int used = F(int, (BYTE*)*m, 40);
            if (fewest > used && (unsigned)used < (unsigned)((F(BYTE*, (BYTE*)*m, 32) - F(BYTE*, (BYTE*)*m, 28)) / 36)) {
                fewest = used;
                best = (int)(m - g_68cc98);
            }
        }
        unsigned cap = (v[2] - v[0]) & ~3u;
        if (cap)
            Ai_PoolFree((void*)v[0], cap);
    }
    if (best == -1) {
        char text[64];
        sprintf(text, "(%d %d %d %d)", key[0], key[2], key[1], key[3]);
        AiError(0, 1, s_mainFile, s_mainId, 4724, "Internal error 07467", text, &g_aiEmpty);
        *market = -1;
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, s_findParkingPlace66d378);
        return market;
    }
    BYTE* mk = (BYTE*)g_68cc98[best];
    DWORD v[3];
    Recs_Search418bb0(v, mk, key, b, c);
    if (v[0] != v[1]) {
        CallC(FUN_45cf80, v[0], v[1], 0, 0, mk + 28);
        *place = (int)(((BYTE**)v[1])[-1] - F(BYTE*, mk, 28)) / 36;
        *market = best;
    } else
        Parking_RandomFree(market, place);
    unsigned cap = (v[2] - v[0]) & ~3u;
    if (cap)
        Ai_PoolFree((void*)v[0], cap);
    Trace_Tag((const DWORD*)g_tagMpl);
    Buf_AppendString(&g_trace, 0, s_findParkingPlace66d378);
    return market;
}

// ---------------------------------------------------------------------------
// the player with the best key among those of the list at 68a980

extern "C" void FUN_4660b0(); KING_ALIAS("_FUN_4660b0", "_Sort_MedianBy504")    // median of three (a, b, c, cmp) -> pointer
extern "C" void FUN_466160(); KING_ALIAS("_FUN_466160", "_Sort_PartitionBy504")    // unguarded partition (first, last, pivot, cmp)
extern "C" void FUN_45f8d0(); KING_ALIAS("_FUN_45f8d0", "_IntroSort_By504")    // introsort loop (first, last, 0, depth, cmp)
extern "C" void FUN_465fd0(); KING_ALIAS("_FUN_465fd0", "_Recs504_PartialSort")    // partial sort (first, middle, last, cmp)
extern "C" void FUN_45f990(); KING_ALIAS("_FUN_45f990", "_Ptrs504_FinalSort")    // final insertion sort (first, last, cmp)

// [FUN_431420] a sorted copy of the players at 68a980; the last one (the head when there are none)
extern "C" void** __cdecl Ai_LastOfSorted(void** out)
{
    if (g_68a980 == g_68a984) {
        *out = g_players;
        return out;
    }
    unsigned n = (unsigned)(g_68a984 - g_68a980);
    void** buf = n ? (void**)Stl_Allocate(n * 4) : 0;
    void** e = (void**)UninitCopyDwords((const DWORD*)g_68a980, (const DWORD*)g_68a984, (DWORD*)buf);
    int cmp = (int)out;
    if (buf != e) {
        int depth = 0;
        for (int k = (int)(e - buf); k != 1; k >>= 1)
            depth++;
        depth *= 2;
        void** l = e;
        if ((int)(((BYTE*)e - (BYTE*)buf) & ~3) > 64) {
            for (;;) {
                if (depth == 0) {
                    CallC(FUN_465fd0, buf, l, l, cmp);
                    break;
                }
                depth--;
                int half = (int)(l - buf) / 2;
                void* pivot = *CallC<void**>(FUN_4660b0, buf, buf + half, l - 1, cmp);
                void** cut = CallC<void**>(FUN_466160, buf, l, pivot, cmp);
                CallC(FUN_45f8d0, cut, l, 0, depth, cmp);
                l = cut;
                if ((int)(((BYTE*)cut - (BYTE*)buf) & ~3) <= 64)
                    break;
            }
        }
        CallC(FUN_45f990, buf, e, cmp);
    }
    *out = e[-1];
    if (n)
        Ai_PoolFree(buf, n * 4);
    return out;
}

// ---------------------------------------------------------------------------
// building the road edges from the node graph

KING_VAR(int, g_68d0e8, 68d0e8)
KING_VAR(RoadEdge**, g_roadEdgesCap, 68cf40)

static void Ptrs_PushBack(BYTE* vec, void* x, bool k)
{
    void*** v = (void***)vec;
    if (v[1] != v[2]) {
        if (v[1])
            *v[1] = x;
        v[1]++;
    } else if (k)
        Vec4b_InsertK((DWORD*)v, 0, (DWORD*)v[1], (const DWORD*)&x);
    else
        Vec4b_Insert44ceb0((DWORD*)v, 0, (DWORD*)v[1], (const DWORD*)&x);
}

// [FUN_405c00] from a node (once): each of its unvisited links starts a road edge (184 bytes) that follows the
// chain through junction-free nodes to the next junction, collecting the links (reversed where walked backwards);
// the edge goes to the edges and to both junctions, and the far node is visited the same way
extern "C" void __cdecl Ai_BuildEdgesFrom(BYTE* node)
{
    if (F(int, node, 0) != 0)
        return;
    if (!(g_68d0e8 & 63))
        CallC(AI_Step3, 116);
    g_68d0e8++;
    BYTE* junction = F(BYTE*, node, 32);
    F(DWORD, node, 0) |= 1;
    for (BYTE** it = F(BYTE**, node, 36); it != F(BYTE**, node, 40); it += 3) {
        if (F(BYTE, *it, 8) & 1)
            continue;
        BYTE* o = (BYTE*)operator new(184);
        if (o) {
            char alloc;
            CallThis(FUN_487730, o, &alloc);
            for (int k = 16; k <= 44; k += 4)
                F(int, o, k) = 0;
            for (int k = 0; k < 4; k++) {
                F(int, o, 48 + k * 16) = 0;
                F(int, o, 52 + k * 16) = 0;
                F(BYTE, o, 56 + k * 16) = 0;
            }
            CallThis(FUN_487730, o + 120, &alloc);
            DVec3* lo = (DVec3*)(o + 136);
            DVec3* hi = (DVec3*)(o + 160);
            lo->x = lo->y = lo->z = 1.7976931348623157e308;
            hi->x = hi->y = hi->z = -1.7976931348623157e308;
        }
        F(BYTE*, o, 112) = junction;
        BYTE** l = it;
        BYTE* cur = node;
        BYTE* farJ;
        for (;;) {
            if (F(BYTE, *l, 8) & 1)
                AiError(0, 100, s_mainFile, s_mainId, 1413, "Internal error", &g_aiEmpty, &g_aiEmpty);
            F(DWORD, *l, 8) |= 1;
            if ((BYTE*)l[1] == cur)
                cur = l[2];
            else if ((BYTE*)l[2] == cur) {
                cur = l[1];
                CallVirt(*l, 28);
            } else
                AiError(0, 100, s_mainFile, s_mainId, 1426, "Internal error", &g_aiEmpty, &g_aiEmpty);
            Ptrs_PushBack(o + 120, *l, true);
            farJ = F(BYTE*, cur, 32);
            if (farJ)
                break;
            BYTE** e = F(BYTE**, cur, 36);
            if (*l == e[0])
                l = e + 3;
            else {
                l = e;
                if (e[0] == e[3])
                    AiError(0, 100, s_mainFile, s_mainId, 1446, "Internal error", &g_aiEmpty, &g_aiEmpty);
            }
        }
        F(BYTE*, o, 116) = farJ;
        Ai_BuildEdgesFrom(cur);
        Ptrs_PushBack((BYTE*)&g_roadEdges, o, false);
        Ptrs_PushBack(junction + 20, o, false);
        Ptrs_PushBack(farJ + 32, o, false);
    }
}

// ---------------------------------------------------------------------------
// players entering other rooms

KING_VAR(AsmLabel, s_updateRooms66dd14, 66dd14)
KING_VAR(AsmLabel, s_room66d638, 66d638)
KING_VAR(AsmLabel, s_room66d628, 66d628)
extern "C" void FUN_42c9b0(); KING_ALIAS("_FUN_42c9b0", "_Position_Keys")    // the rooms of a position (PositionId32, out48)

// [FUN_42cb20] each player's rooms (from its position); a change is reported to the event hook (kind 5)
extern "C" void __cdecl Ai_UpdatePlayerRooms()
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_updateRooms66dd14);
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        int r[12] = { 0 };
        r[5] = (int)(const char*)s_room66d638;
        r[6] = (int)(const char*)s_room66d628;
        r[10] = -1;
        r[11] = -1;
        PositionId pos;
        Rec_Copy32(AI_ListData(it) + 16, 0, &pos);
        CallC(FUN_42c9b0, pos, r);
        BYTE* d = AI_ListData(it);
        if (r[10] != F(int, d, 848) || r[9] != F(int, AI_ListData(it), 844)) {
            int id;
            Ai_PlayerEvent((BYTE*)*Int_Set(&id, 0, (int)it), 5, 0);
        }
        F(int, AI_ListData(it), 848) = r[10];
        F(int, AI_ListData(it), 844) = r[9];
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_updateRooms66dd14);
}

// ---------------------------------------------------------------------------
// an ambushing player turns on the human

KING_VAR(AsmLabel, s_ambushAttack66dcc8, 66dcc8)

// [FUN_42c570] the ambushes holding the player release it (lights on); it chases the human player (+968) and the
// human calls mafia (ambush kind 1) or helpers (kind 2)
extern "C" void __cdecl Ai_AmbushAttack(BYTE* p)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_ambushAttack66dcc8);
    if (Player_NotValid(p)) {
        sprintf(g_numberText, s_idFormat);
        AiError(33, 100, s_mainFile, s_mainId, 11469, s_invalidId, s_playerIdType, g_numberText);
    }
    for (void** e = g_68a970; e != g_68a974; e++) {
        BYTE* amb = (BYTE*)*e;
        if (F(BYTE*, amb, 124) != p)
            continue;
        Ai_ReleaseAttachedPlayer(amb);
        AI_LightsOn((BYTE*)*e);
        int h;
        Player_FromId(&h);
        Player_GoTo(p, F(PositionId, (BYTE*)h + 8, 32), 2);
        Player_FromId(&h);
        F(int, p, 968) = h;
        int kind = F(int, (BYTE*)*e, 36);
        if (kind == 1) {
            Player_FromId(&h);
            Ai_CallMafia((BYTE*)h);
        } else if (kind == 2) {
            Player_FromId(&h);
            Ai_CallHelpers((BYTE*)h);
        }
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_ambushAttack66dcc8);
}

// ---------------------------------------------------------------------------
// heading for a town, avoiding sharp turns

KING_VAR(AsmLabel, s_goToTown66d264, 66d264)
extern "C" double __cdecl Road_MinTurnCos(const HopVec* hops);

// [FUN_417b70] like FUN_417820, but a route with a turn sharper than cos -0.8 is dropped and searched again
// (up to 3 tries)
extern "C" void __cdecl Ai_GoToTownSmooth(BYTE* p, PositionId pos)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_goToTown66d264);
    Vec12_Clear(p + 24, 0);
    F(BYTE, p, 872) = 1;
    int town = 0;
    if (pos.node != -1)
        town = F(int, g_68cf00[pos.node], 12);
    int tries = 0;
    while (F(BYTE*, p, 24) == F(BYTE*, p, 28)) {
        int pair[2];
        int pp = (int)p;
        Pair_SetFromPtr(pair, 0, town, &pp);
        int n;
        Nodes_Find456140(&n, pair[0], pair[1], (int)&g_rng68b9c8);
        if (n == -1)
            AiError(0, 100, s_mainFile, s_mainId, 4461, "Can not find node to trace to", &g_aiEmpty, &g_aiEmpty);
        PositionId to;
        PositionInit(&to, n);
        Player_GoTo(p, to, 1);
        tries++;
        if (!(Road_MinTurnCos((const HopVec*)(p + 24)) >= -0.8) && tries < 3) {
            Vec12_Clear(p + 24, 0);
            F(BYTE, p, 872) = 1;
        }
    }
    AdvanceRoad_Check(&F(PositionId, p, 40), 4468);
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_goToTown66d264);
}

// ---------------------------------------------------------------------------
// whether a player has an order for a town

KING_VAR(AsmLabel, s_hasOrderFor66f1c8, 66f1c8)

// [FUN_443ac0] whether one of the player's orders (144 bytes) goes to the town
extern "C" bool __cdecl Ai_HasOrderForTown(BYTE* p, int townId)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_hasOrderFor66f1c8);
    int town;
    Town_ById(&town, townId);
    BYTE* end = ((BYTE**)Player_Orders(p, (BYTE*)townId))[1];
    BYTE* o = ((BYTE**)Player_Orders(p, (BYTE*)townId))[0];
    while (o != end && F(int, o, 12) != town)
        o += 144;
    if (o == ((BYTE**)Player_Orders(p, (BYTE*)townId))[1]) {
        Trace_Tag((const DWORD*)g_tagMpl);
        Trace_CopyName(s_hasOrderFor66f1c8);
        return false;
    }
    Trace_Tag((const DWORD*)g_tagMpl);
    Buf_AppendString(&g_trace, 0, s_hasOrderFor66f1c8);
    return true;
}

// ---------------------------------------------------------------------------
// a new piece of goods

KING_VAR(AsmLabel, s_newGoods66d5d4, 66d5d4)
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")    // console: print (text, x)
extern "C" int* __cdecl Recs104_Random(int* out, int a, int b);
extern "C" int* __cdecl Recs104_RandomMasked(int* out, int a, int b);
extern "C" int* __cdecl Items_Find41b2a0(int* out, BYTE* p, int a, int b);
extern "C" void __cdecl Price_Set(BYTE* item, BYTE* table);
extern "C" int __fastcall PlayerId_Over025(int id, void*);

// [FUN_41b3a0] a new piece of goods (32 bytes) of a random kind (or one of the town's kinds), due in 200..499 s,
// priced; sometimes (1 in 5, when allowed) disguised as another kind (the real one at +8)
extern "C" BYTE** __cdecl Ai_NewGoods(BYTE** out, BYTE* from, BYTE* town, int kind)
{
    Buf_Append(&g_trace, 0, g_tagArt, 8);
    Buf_AppendString(&g_trace, 0, s_newGoods66d5d4);
    *out = 0;
    BYTE* o = 0;
    for (int tries = 0; !o; ) {
        int* r;
        int tmp;
        if (F(BYTE, from, 4) & 4) {
            int a2 = (int)town;
            r = Recs104_Random(&a2, 4, 4);
        } else
            r = Items_Find41b2a0(&tmp, town, kind != 1 ? 16 : 8, 28);
        int id = *r;
        if (id == -1) {
            Trace_Tag((const DWORD*)g_tagMpl);
            Buf_AppendString(&g_trace, 0, s_newGoods66d5d4);
            return out;
        }
        o = (BYTE*)operator new(32);
        if (o) {
            int t;
            int v = *Town_Find5(&t, (int)town, id);
            F(int, o, 0) = id;
            F(int, o, 4) = -1;
            F(int, o, 8) = -1;
            F(int, o, 16) = 0;
            F(int, o, 24) = 0;
            F(int, o, 12) = v;
            F(int, o, 20) = 0;
            F(int, o, 28) = 0;
        }
        if (++tries >= 100 && !o) {
            Trace_Tag((const DWORD*)g_tagMpl);
            Buf_AppendString(&g_trace, 0, s_newGoods66d5d4);
            return out;
        }
    }
    unsigned due = AiRandom() % 300 + 200;
    double now = CallC<double>(FUN_401000);
    F(double, o, 16) = now + (double)(__int64)due;
    Price_Set(o, town);
    if (!(F(BYTE, g_68ccb8 + F(int, o, 0) * 104, 48) & 4) && (BYTE)PlayerId_Over025((int)g_68ccb8, 0)
        && AiRandom() % 5 == 2) {
        int k = kind;
        int d = *Recs104_RandomMasked(&k, (kind != 1 ? 16 : 8) | 1, 25);
        F(int, o, 4) = d;
        if (d != -1) {
            F(int, o, 8) = F(int, o, 0);
            F(int, o, 0) = F(int, o, 4);
        } else {
            AiError(0, 200, s_mainFile, s_mainId, 5936, "Internal error 066321", &g_aiEmpty, &g_aiEmpty);
            CallC(FUN_5d2ba0, "additional: ", kind);
        }
    }
    *out = o;
    Trace_Tag((const DWORD*)g_tagMpl);
    Trace_CopyName(s_newGoods66d5d4);
    return out;
}

// ---------------------------------------------------------------------------
// vector inserts

extern "C" void FUN_453110(); KING_ALIAS("_FUN_453110", "@OrderInfo_CopyConstruct@12")    // order info (192 bytes) copy constructor (thiscall src)
extern "C" void FUN_453350(); KING_ALIAS("_FUN_453350", "@OrderInfo_CopyRaw@12")    // order info copy constructor, second instance (thiscall src)
extern "C" void FUN_442800(); KING_ALIAS("_FUN_442800", "@Rec_FirstNonEmpty@8")    // order info: fix its first-non-empty pointers (thiscall)
extern "C" void FUN_463da0(); KING_ALIAS("_FUN_463da0", "_Rec192_CopyBack")    // copy_backward of order infos (first, last, result, -, 0)
extern "C" void FUN_453620(); KING_ALIAS("_FUN_453620", "@Rec192_Assign@12")    // order info assignment (thiscall src)
extern "C" void FUN_467580(); KING_ALIAS("_FUN_467580", "_OrderInfos_UninitCopy")    // uninitialized copy of order infos (first, last, dest, -)
extern "C" void FUN_463b80(); KING_ALIAS("_FUN_463b80", "_CharVecs_CopyBack")    // copy_backward of vector<char> (first, last, result, -, 0)
ASM_FUNC(FUN_467320, _CharVec_UninitCopy)    // uninitialized copy of vector<char> (first, last, dest, -)
extern "C" void FUN_458500(); KING_ALIAS("_FUN_458500", "@Rec458500_Copy@12")    // 36-byte record copy constructor (thiscall src)
extern "C" void FUN_459610(); KING_ALIAS("_FUN_459610", "@Vec4b_CopyB@12")    // vector<4 bytes> copy constructor (thiscall src)
extern "C" void FUN_463940(); KING_ALIAS("_FUN_463940", "_Rec36_CopyBack")    // copy_backward of 36-byte records (first, last, result, -, 0)
extern "C" void FUN_45acc0(); KING_ALIAS("_FUN_45acc0", "@Vec4_Assign@12")    // vector<4 bytes> assignment (thiscall src)
extern "C" void FUN_4671c0(); KING_ALIAS("_FUN_4671c0", "_Recs36_UninitCopy")    // uninitialized copy of 36-byte records (first, last, dest, -)
extern "C" void FUN_44b0d0(); KING_ALIAS("_FUN_44b0d0", "@Vec4_Free@8")    // vector<4 bytes> destructor (thiscall)

// [FUN_452380] vector<int>::insert(pos, first, last) (thiscall), the same as FUN_44d170
extern "C" void __fastcall IntVec_InsertRange(DWORD** self, void*, DWORD* pos, const DWORD* first, const DWORD* last)
{
    DwordVec_InsertRange(self, 0, pos, first, last);
}

// new length of a full vector: twice the size, 1 when empty
static unsigned Vec_Grow(BYTE** v, unsigned size)
{
    unsigned n = (unsigned)(v[1] - v[0]) / size;
    return n ? n * 2 : 1;
}

// [FUN_452e80] vector<order info (192 bytes)>::insert_aux(pos, x) (thiscall)
extern "C" void __fastcall OrderInfoVec_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_453110, v[1], v[1] - 192);
        v[1] += 192;
        BYTE tmp[192];
        CallThis(FUN_453350, tmp, x);
        CallThis(FUN_442800, tmp);
        CallC(FUN_463da0, pos, v[1] - 384, v[1] - 192, pos, 0);
        CallThis(FUN_453620, pos, tmp);
        CallThis(FUN_442800, pos);
        CallThis(FUN_440dd0, tmp);
        return;
    }
    unsigned len = Vec_Grow(v, 192);
    BYTE* buf = (BYTE*)Stl_Allocate(len * 192);
    BYTE* e = CallC<BYTE*>(FUN_467580, v[0], pos, buf, pos);
    if (e)
        CallThis(FUN_453110, e, x);
    e = CallC<BYTE*>(FUN_467580, pos, v[1], e + 192, pos);
    for (BYTE* d = v[0]; d != v[1]; d += 192)
        CallThis(FUN_440dd0, d);
    unsigned cnt = (unsigned)(v[2] - v[0]) / 192;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 192);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 192;
}

// [FUN_452610] vector<vector<char>>::insert_aux(pos, x) (thiscall)
extern "C" void __fastcall CharVecVec_InsertAux(BYTE** v, void*, BYTE* pos, BYTE* const* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_44dd90, v[1], v[1] - 12);
        v[1] += 12;
        BYTE* tmp[3];
        char alloc;
        CallThis(FUN_44a490, tmp, (unsigned)(x[1] - x[0]), &alloc);
        memmove(tmp[0], x[0], x[1] - x[0]);
        tmp[1] = tmp[0] + (x[1] - x[0]);
        CallC(FUN_463b80, pos, v[1] - 24, v[1] - 12, pos, 0);
        if ((BYTE*)tmp != pos)
            PodVec_Assign((BYTE**)pos, 0, tmp);
        CallThis(FUN_44b090, tmp);
        return;
    }
    unsigned len = Vec_Grow(v, 12);
    BYTE* buf = (BYTE*)Stl_Allocate(len * 12);
    BYTE* e = CallC<BYTE*>(FUN_467320, v[0], pos, buf, pos);
    CallC(FUN_4611e0, e, x);
    e = CallC<BYTE*>(FUN_467320, pos, v[1], e + 12, pos);
    for (BYTE* d = v[0]; d != v[1]; d += 12)
        CallThis(FUN_44b090, d);   // (after FUN_50d190, a no-op)
    unsigned cnt = (unsigned)(v[2] - v[0]) / 12;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 12);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 12;
}

// [FUN_451230] vector<36-byte record (3 words, vector<char>, vector<4 bytes>)>::insert_aux(pos, x) (thiscall)
extern "C" void __fastcall Rec36Vec_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_458500, v[1], v[1] - 36);
        v[1] += 36;
        BYTE tmp[36];
        memcpy(tmp, x, 12);
        CallThis(FUN_44dd90, tmp + 12, x + 12);
        CallThis(FUN_459610, tmp + 24, x + 24);
        CallC(FUN_463940, pos, v[1] - 72, v[1] - 36, pos, 0);
        memcpy(pos, tmp, 12);
        PodVec_Assign((BYTE**)(pos + 12), 0, (BYTE* const*)(tmp + 12));
        CallThis(FUN_45acc0, pos + 24, tmp + 24);
        CallThis(FUN_44b0d0, tmp + 24);
        CallThis(FUN_404420, tmp + 12);
        return;
    }
    unsigned len = Vec_Grow(v, 36);
    BYTE* buf = (BYTE*)Stl_Allocate(len * 36);
    BYTE* e = CallC<BYTE*>(FUN_4671c0, v[0], pos, buf, pos);
    if (e)
        CallThis(FUN_458500, e, x);
    e = CallC<BYTE*>(FUN_4671c0, pos, v[1], e + 36, pos);
    for (BYTE* d = v[0]; d != v[1]; d += 36) {
        CallThis(FUN_447970, d + 24);
        CallThis(FUN_404420, d + 12);
    }
    unsigned cnt = (unsigned)(v[2] - v[0]) / 36;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 36);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 36;
}

extern "C" void FUN_458660(); KING_ALIAS("_FUN_458660", "@Rec458660_Copy@12")    // 56-byte record copy constructor (thiscall src)
extern "C" void FUN_463c60(); KING_ALIAS("_FUN_463c60", "_Rec56_CopyBack")    // copy_backward of 56-byte records (first, last, result, -, 0)
extern "C" void FUN_467440(); KING_ALIAS("_FUN_467440", "_Recs56_UninitCopy")    // uninitialized copy of 56-byte records (first, last, dest, -)
extern "C" void FUN_468d10(); KING_ALIAS("_FUN_468d10", "_UninitCopy32Skip1")    // uninitialized copy of 32-byte entries (first, last, dest, -)
extern "C" void FUN_458a40(); KING_ALIAS("_FUN_458a40", "@Rec_Copy8Skip1@12")    // 32-byte entry copy (thiscall src; skips word 1)

// [FUN_452bc0] vector<56-byte record (int, vector<char>, words; +36 not copied)>::insert_aux(pos, x) (thiscall)
extern "C" void __fastcall Rec56Vec_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_458660, v[1], v[1] - 56);
        v[1] += 56;
        BYTE tmp[56];
        F(int, tmp, 0) = F(int, x, 0);
        CallThis(FUN_44dd90, tmp + 4, x + 4);
        memcpy(tmp + 16, x + 16, 20);
        memcpy(tmp + 40, x + 40, 16);
        CallC(FUN_463c60, pos, v[1] - 112, v[1] - 56, pos, 0);
        F(int, pos, 0) = F(int, tmp, 0);
        PodVec_Assign((BYTE**)(pos + 4), 0, (BYTE* const*)(tmp + 4));
        memcpy(pos + 16, tmp + 16, 20);
        memcpy(pos + 40, tmp + 40, 16);
        CallThis(FUN_404420, tmp + 4);
        return;
    }
    unsigned len = Vec_Grow(v, 56);
    BYTE* buf = (BYTE*)Stl_Allocate(len * 56);
    BYTE* e = CallC<BYTE*>(FUN_467440, v[0], pos, buf, pos);
    if (e)
        CallThis(FUN_458660, e, x);
    e = CallC<BYTE*>(FUN_467440, pos, v[1], e + 56, pos);
    for (BYTE* d = v[0]; d != v[1]; d += 56)
        CallThis(FUN_404420, d + 4);
    unsigned cnt = (unsigned)(v[2] - v[0]) / 56;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 56);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 56;
}

// [FUN_456ea0] vector<32-byte entry>::insert_aux(pos, x) (thiscall); new entries are made by FUN_458a40 (which
// skips word 1), and the inserted one keeps pos's word 1 (the original copies an uninitialized word there)
extern "C" void __fastcall Entry32Vec_InsertAux(BYTE** v, void*, BYTE* pos, const DWORD* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_458a40, v[1], v[1] - 32);
        v[1] += 32;
        DWORD tmp[8];
        memcpy(tmp, x, 32);
        tmp[1] = ((DWORD*)pos)[1];
        BYTE* s = v[1] - 64;
        BYTE* d = v[1] - 32;
        for (int n = (int)(s - pos) >> 5; n > 0; n--) {
            s -= 32;
            d -= 32;
            memcpy(d, s, 32);
        }
        memcpy(pos, tmp, 32);
        return;
    }
    unsigned n = (unsigned)(v[1] - v[0]) >> 5;
    unsigned len = n ? n * 2 : 1;
    BYTE* buf = (BYTE*)Stl_Allocate(len * 32);
    BYTE* e = CallC<BYTE*>(FUN_468d10, v[0], pos, buf, pos);
    if (e)
        CallThis(FUN_458a40, e, x);
    e = CallC<BYTE*>(FUN_468d10, pos, v[1], e + 32, pos);
    unsigned cnt = (unsigned)(v[2] - v[0]) >> 5;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 32);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 32;
}

extern "C" void FUN_40c220(); KING_ALIAS("_FUN_40c220", "@Rec40c220_Copy@12")    // 104-byte record copy constructor (thiscall src)
extern "C" void FUN_40c100(); KING_ALIAS("_FUN_40c100", "@Str_Assign@12")    // script handle assignment (thiscall src)
extern "C" void FUN_454580(); KING_ALIAS("_FUN_454580", "@Str_CopyConstruct@12")    // script handle copy constructor (thiscall src)
extern "C" void FUN_462500(); KING_ALIAS("_FUN_462500", "_Rec104_CopyBack")    // copy_backward of 104-byte records (first, last, result, -, 0)
extern "C" void FUN_466590(); KING_ALIAS("_FUN_466590", "_Recs104_UninitCopy")    // uninitialized copy of 104-byte records (first, last, dest, -)
extern "C" void FUN_460960(); KING_ALIAS("_FUN_460960", "_Rec40c220_Construct")    // construct a 104-byte record in place (dest, src)
extern "C" void FUN_4581f0(); KING_ALIAS("_FUN_4581f0", "@Rec4581f0_Copy@12")    // 24-byte record copy constructor (thiscall src)
extern "C" void FUN_463390(); KING_ALIAS("_FUN_463390", "_Rec24b_CopyBack")    // copy_backward of 24-byte records (first, last, result, -, 0)
ASM_FUNC(FUN_466d50, _Rec24_UninitCopy)    // uninitialized copy of 24-byte records (first, last, dest, -)

// the words of a 104-byte record that are copied as words
static void Rec104_CopyWords(BYTE* d, const BYTE* s)
{
    memcpy(d + 16, s + 16, 36);
    memcpy(d + 56, s + 56, 20);
    memcpy(d + 80, s + 80, 8);
}

// [FUN_44da60] vector<104-byte record (script handle, words, vector<char> at +88)>::insert_aux(pos, x) (thiscall)
extern "C" void __fastcall Rec104Vec_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_40c220, v[1], v[1] - 104);
        v[1] += 104;
        BYTE tmp[104];
        CallThis(FUN_454580, tmp, x);
        Rec104_CopyWords(tmp, x);
        CallThis(FUN_44dd90, tmp + 88, x + 88);
        CallC(FUN_462500, pos, v[1] - 208, v[1] - 104, pos, 0);
        CallThis(FUN_40c100, pos, tmp);
        Rec104_CopyWords(pos, tmp);
        PodVec_Assign((BYTE**)(pos + 88), 0, (BYTE* const*)(tmp + 88));
        CallThis(FUN_404420, tmp + 88);
        CallThis(FUN_4a36c0, tmp);
        return;
    }
    unsigned len = Vec_Grow(v, 104);
    BYTE* buf = (BYTE*)Stl_Allocate(len * 104);
    BYTE* e = CallC<BYTE*>(FUN_466590, v[0], pos, buf, pos);
    CallC(FUN_460960, e, x);
    e = CallC<BYTE*>(FUN_466590, pos, v[1], e + 104, pos);
    for (BYTE* d = v[0]; d != v[1]; d += 104) {
        CallThis(FUN_404420, d + 88);
        CallThis(FUN_4a36c0, d);
    }
    unsigned cnt = (unsigned)(v[2] - v[0]) / 104;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 104);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 104;
}

// [FUN_450330] vector<24-byte record (2 words, vector<char>)>::insert_aux(pos, x) (thiscall)
extern "C" void __fastcall Rec24Vec_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_4581f0, v[1], v[1] - 24);
        v[1] += 24;
        BYTE tmp[24];
        memcpy(tmp, x, 8);
        CallThis(FUN_44dd90, tmp + 8, x + 8);
        CallC(FUN_463390, pos, v[1] - 48, v[1] - 24, pos, 0);
        memcpy(pos, tmp, 8);
        PodVec_Assign((BYTE**)(pos + 8), 0, (BYTE* const*)(tmp + 8));
        CallThis(FUN_44b090, tmp + 8);
        return;
    }
    unsigned len = Vec_Grow(v, 24);
    BYTE* buf = (BYTE*)Stl_Allocate(len * 24);
    BYTE* e = CallC<BYTE*>(FUN_466d50, v[0], pos, buf, pos);
    if (e)
        CallThis(FUN_4581f0, e, x);
    e = CallC<BYTE*>(FUN_466d50, pos, v[1], e + 24, pos);
    for (BYTE* d = v[0]; d != v[1]; d += 24)
        CallThis(FUN_404420, d + 8);
    unsigned cnt = (unsigned)(v[2] - v[0]) / 24;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 24);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 24;
}

extern "C" void FUN_462430(); KING_ALIAS("_FUN_462430", "_StrRefs_CopyBack")    // copy_backward of script handles (first, last, result, -, 0)
extern "C" void FUN_460910(); KING_ALIAS("_FUN_460910", "_ConstructStrRef")    // construct a script handle in place (dest, src)

// [FUN_44c710] vector<StrRef>::insert_aux(pos, x) (thiscall)
extern "C" void __fastcall StrRefVec_InsertAux(BYTE** v, void*, StrRef* pos, const StrRef* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_454580, v[1], v[1] - 12);
        v[1] += 12;
        StrRef tmp;
        tmp.CopyFrom(x);
        CallC(FUN_462430, pos, v[1] - 24, v[1] - 12, pos, 0);
        if (pos->ptr && pos->Owns()) {
            if (--F(int, pos->ptr, 4) == 0 && !Str_Same(pos, &tmp) && pos->ptr)
                CallVirt(pos->ptr, 12, 1);
        }
        pos->CopyFrom(&tmp);
        Str_Release(&tmp);
        return;
    }
    unsigned len = Vec_Grow(v, 12);
    BYTE* buf = (BYTE*)Stl_Allocate(len * 12);
    BYTE* e = CallC<BYTE*>(FUN_467780, v[0], (BYTE*)pos, buf, pos);
    CallC(FUN_460910, e, x);
    e = CallC<BYTE*>(FUN_467780, (BYTE*)pos, v[1], e + 12, pos);
    for (BYTE* d = v[0]; d != v[1]; d += 12)
        Str_Release((StrRef*)d);
    unsigned cnt = (unsigned)(v[2] - v[0]) / 12;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 12);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 12;
}

extern "C" void FUN_443860(); KING_ALIAS("_FUN_443860", "@Order144_CopyConstruct@12")    // order copy constructor (thiscall src)
extern "C" void FUN_450ce0(); KING_ALIAS("_FUN_450ce0", "@Vec16Rec_Copy@12")    // order's +120 container copy constructor (thiscall src)
extern "C" void FUN_463700(); KING_ALIAS("_FUN_463700", "_Order144_CopyBack")    // copy_backward of orders (first, last, result, -, 0)
extern "C" void FUN_458bc0(); KING_ALIAS("_FUN_458bc0", "@Vec16_Assign@12")    // order's +120 container assignment (thiscall src)
extern "C" void FUN_448610(); KING_ALIAS("_FUN_448610", "@VecByteVec16_Destroy@8")    // order's +120 container destructor (thiscall)
extern "C" void FUN_467000(); KING_ALIAS("_FUN_467000", "_Orders144_UninitCopy")    // uninitialized copy of orders (first, last, dest, -)
extern "C" void FUN_460d20(); KING_ALIAS("_FUN_460d20", "_Rec458a80_Construct")    // construct an order in place (dest, src)
extern "C" void FUN_450de0(); KING_ALIAS("_FUN_450de0", "@Vec16_Destroy@8")    // order's +120 container destructor, second instance (thiscall)

// the words of an order (144 bytes) copied as words (not +20 and +108)
static void Order144_CopyWords(BYTE* d, const BYTE* s)
{
    memcpy(d, s, 20);
    memcpy(d + 24, s + 24, 84);
    memcpy(d + 112, s + 112, 8);
}

// [FUN_450960] vector<order (144 bytes)>::insert_aux(pos, x) (thiscall)
extern "C" void __fastcall OrderVec_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_443860, v[1], v[1] - 144);
        v[1] += 144;
        BYTE tmp[144];
        Order144_CopyWords(tmp, x);
        CallThis(FUN_450ce0, tmp + 120, x + 120);
        F(int, tmp, 136) = F(int, x, 136);
        CallC(FUN_463700, pos, v[1] - 288, v[1] - 144, pos, 0);
        Order144_CopyWords(pos, tmp);
        CallThis(FUN_458bc0, pos + 120, tmp + 120);
        F(int, pos, 136) = F(int, tmp, 136);
        CallThis(FUN_448610, tmp + 120);
        return;
    }
    unsigned len = Vec_Grow(v, 144);
    BYTE* buf = (BYTE*)Stl_Allocate(len * 144);
    BYTE* e = CallC<BYTE*>(FUN_467000, v[0], pos, buf, pos);
    CallC(FUN_460d20, e, x);
    e = CallC<BYTE*>(FUN_467000, pos, v[1], e + 144, pos);
    for (BYTE* d = v[0]; d != v[1]; d += 144)
        CallThis(FUN_450de0, d + 120);
    unsigned cnt = (unsigned)(v[2] - v[0]) / 144;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 144);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 144;
}

// ---------------------------------------------------------------------------
// Batch: the human's timers, a player's cargo cost limits, competitors' arrivals

KING_VAR(double, g_68d170, 68d170)
KING_VAR(double, g_zero64b678, 64b678)
KING_VAR(double, g_64b6f8, 64b6f8)
KING_VAR(double, g_64b5b8, 64b5b8)
KING_VAR(double, g_64b840, 64b840)
KING_VAR(AsmLabel, s_disqualified66da60, 66da60)
KING_VAR(AsmLabel, s_fmt66ddb4, 66ddb4)
extern "C" void FUN_40b0d0(); KING_ALIAS("_FUN_40b0d0", "_Player_CopyState")    // a player's six state words (out, player)
extern "C" void FUN_41b910(); KING_ALIAS("_FUN_41b910", "_Player_AllCosts")    // (player, double*, double*)
extern "C" void FUN_41b830(); KING_ALIAS("_FUN_41b830", "_Vehicle_AddCosts")    // (int, double*, double*)
extern "C" void FUN_41bad0(); KING_ALIAS("_FUN_41bad0", "_Player_CargoSums")    // (player, double*, double*)
extern "C" void FUN_417680(); KING_ALIAS("_FUN_417680", "_Player_StopWaiting")    // (player)
extern "C" void __cdecl AI_SortTimes(double* first, double* last, int, int, int);
extern "C" void __fastcall Vec8_Insert479760(BYTE** v, void*, BYTE* pos, const BYTE* x);

// [FUN_4194a0] unless the human is in state 5, an empty waiting list restarts the waiting timer
extern "C" void __cdecl Ai_RestartWaitTimer()
{
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (F(int, h, 100) != 5 && g_68cf60 == g_68cf64) {
        g_66c6e0 = 3;
        g_68d170 = CallC<double>(FUN_401000);
    }
}

// [FUN_409f70] the human's lead on a player at +504; true when positive
extern "C" bool __cdecl Ai_HumanLead(BYTE* p, double* out)
{
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    double d = F(double, h, 504) - F(double, p, 504);
    *out = d;
    return d > g_zero64b678;
}

extern "C" int __cdecl Ai_HumanFinished();

// [FUN_427920] a racing human: refreshes its race time and, past a day, finishes
extern "C" void __cdecl Ai_HumanRaceTick()
{
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h == (BYTE*)g_players || F(int, h, 100) != 5)
        return;
    if (F(DWORD, h, 96) & 0x4000) {
        F(double, h, 808) = CallC<double>(FUN_401000) - F(double, h, 792);
        h = (BYTE*)s;
    }
    if (F(double, h, 808) > g_dayLength66c698) {
        Ai_PlayerEvent(h, 13, 0);
        Ai_HumanFinished();
    }
}

// [FUN_427430] the human finishes: a racer is disqualified (and a hired one leaves its hire), then the
// waiting timer, its race time and the bankruptcy check
extern "C" int __cdecl Ai_HumanFinished()
{
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h == (BYTE*)g_players)
        return 0;
    if (F(int, h, 100) == 5) {
        CallC(FUN_417680, h);
        CallC(FUN_56ae60, Message_Text((const char*)s_disqualified66da60));
        if (CallC<char>(FUN_417660, s)) {
            Players_ResetMarks();
            F(int, (BYTE*)s, 524) = 0;
            F(int, (BYTE*)s, 528) = 0;
            F(int, (BYTE*)s, 532) = -1;
        }
        CallC(FUN_477210, s);
    }
    Ai_RestartWaitTimer();
    h = (BYTE*)s;
    F(double, h, 808) = CallC<double>(FUN_401000) - F(double, h, 792);
    if (!g_68d188)
        F(int, (BYTE*)s, 536) = 0;
    CallC(FUN_426f20, s);
    return 0;
}

// [FUN_4774e0] count competitors arrive delay from now; the arrival times stay sorted
extern "C" void __cdecl Ai_AddArrivals(BYTE* p, double delay, int count)
{
    for (int i = count; i > 0; i--) {
        double z = 0;
        double** v = (double**)(p + 144);
        if (v[1] != v[2]) {
            if (v[1])
                *v[1] = z;
            v[1]++;
        } else
            Vec8_Insert479760((BYTE**)v, 0, (BYTE*)v[1], (const BYTE*)&z);
        double* last = F(double*, p, 148);
        last[-1] = CallC<double>(FUN_401000) + delay;
    }
    AI_SortTimes(F(double*, p, 144), F(double*, p, 148), 0, 0, 0);
}

// [FUN_429850] now and then (more often the richer the human) a competitor arrives in 300
extern "C" void __cdecl Ai_MaybeArrival()
{
    int s;
    int div = 1;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h != (BYTE*)g_players) {
        if (F(double, h, 768) > g_64b5b8)
            div = 2;
        if (F(double, h, 768) > g_64b840)
            div = 4;
    }
    unsigned r = AiRandom();
    if (r % (unsigned)(24 / div) == 0)
        Ai_AddArrivals((BYTE*)s, 300.0, 1);
}

// the cargo limits shared by FUN_41bca0 and FUN_41bbe0: each amount kept within what the other allows, scaled,
// and the quantity out of the first
static void Cargo_Limit(double* a, double* b, double* q, const BYTE* rec)
{
    double x = *b * F(double, rec, 16) / F(double, rec, 24);
    if (!(x >= *a))
        *a = x;
    double y = F(double, rec, 24) * *a / F(double, rec, 16);
    if (!(y >= *b))
        *b = y;
    *a *= g_64b6f8;
    *b *= g_64b6f8;
    *q = *a / F(double, rec, 16);
}

// [FUN_41bca0] an order's cargo amounts for a player
extern "C" void __cdecl Ai_CargoAmounts(int player, double* a, double* b, double* q, const BYTE* rec, int arg)
{
    CallC(FUN_41b910, player, a, b);
    if (arg != -1)
        CallC(FUN_41b830, arg, a, b);
    Cargo_Limit(a, b, q, rec);
}

// [FUN_41bbe0] the same, less what the player already carries
extern "C" void __cdecl Ai_CargoCosts(int player, double* a, double* b, double* q, const BYTE* rec, int arg)
{
    CallC(FUN_41b910, player, a, b);
    if (arg != -1)
        CallC(FUN_41b830, arg, a, b);
    double da, db;
    CallC(FUN_41bad0, player, &da, &db);
    *a -= da;
    *b -= db;
    Cargo_Limit(a, b, q, rec);
}

// [FUN_42d640] the human's six state words, printed into the trace
extern "C" void __cdecl Ai_TraceHumanState()
{
    int s;
    DWORD w[6];
    char buf[1024];
    Player_FromId(&s);
    CallC(FUN_40b0d0, w, s);
    sprintf(buf, (const char*)s_fmt66ddb4, w[0], w[1], w[2], w[3], w[4], w[5]);
    Trace_CopyName(buf);
}

// ---------------------------------------------------------------------------
// Batch: rope releases, names to ids, competitors' traces, the road network's totals, a new player on a road

struct Blob32 { DWORD d[8]; };
KING_VAR(AsmLabel, s_retraceCompetitorRegular, 66cb88)
KING_VAR(AsmLabel, s_reportCompetitionState, 66db0c)
KING_VAR(double, g_64b6a8, 64b6a8)
KING_VAR(double, g_66f8a0, 66f8a0)
extern "C" void FUN_478260(); KING_ALIAS("_FUN_478260", "_Player_GoTo")    // (player, 32-byte position, int)
extern "C" void FUN_478630(); KING_ALIAS("_FUN_478630", "_Player_RemoveWaypoint")    // (player, int)
extern "C" void FUN_428230(); KING_ALIAS("_FUN_428230", "_Ai_RecalculateCompetitionPlace")
extern "C" void FUN_427f40(); KING_ALIAS("_FUN_427f40", "_Ai_CalculateNewPlace")    // (int*, double*)
extern "C" void FUN_404e80(); KING_ALIAS("_FUN_404e80", "_Town_Id")    // a town's node (out, town)
extern "C" void FUN_487500(); KING_ALIAS("_FUN_487500", "_Road_PositionCostBest")    // a route's length (int, from, to, double* out)
extern "C" void FUN_422390(); KING_ALIAS("_FUN_422390", "_Player_Register")    // registers a player (out id, player)
extern "C" void FUN_4842f0(); KING_ALIAS("_FUN_4842f0", "_Road_MinTurnCos")    // returns double
extern "C" void FUN_487f60(); KING_ALIAS("_FUN_487f60", "@DBox_Extend@12")    // grows a box over a point (this box, point)

// [FUN_44a720] drops a reference to a rope node, if there is one
extern "C" void __cdecl Rope_UnrefIf(BYTE* n)
{
    if (n)
        Rope_Unref(n, 0);
}

// the id a name maps to in a hash_map<string, int>, or -1 (the body of FUN_449c40, 449f00 and 44a1c0)
static int* NameMap_Find(BYTE* map, int* out, const char* name)
{
    if (!name) {
        *out = -1;
        return out;
    }
    BYTE* key = CallC<BYTE*>(FUN_44f340, 0, name, strlen(name));
    void* it[2];
    NameHash_Find(map, 0, it, (int**)&key);
    *out = it[0] ? F(int, it[0], 8) : -1;
    if (key)
        Rope_Unref(key, 0);
    return out;
}

// [FUN_449c40]
extern "C" int* __cdecl NameMap68cff0_Find(int* out, const char* name) { return NameMap_Find(&w_hash68cff0, out, name); }
// [FUN_449f00]
extern "C" int* __cdecl NameMap68d008_Find(int* out, const char* name) { return NameMap_Find((BYTE*)&g_init68d008, out, name); }
// [FUN_44a1c0]
extern "C" int* __cdecl NameMap68cf10_Find(int* out, const char* name) { return NameMap_Find(&w_hash68cf10, out, name); }

// [FUN_409fc0] retraceCompetitorRegular(PlayerId player)
extern "C" void __cdecl Ai_RetraceCompetitorRegular(int player)
{
    Trace_EnterAppend((const char*)s_retraceCompetitorRegular);
    if (CallC<char>(FUN_4172e0, player) && F(int, (BYTE*)player, 100) != 5) {
        double lead;
        if (Ai_HumanLead((BYTE*)player, &lead) && F(char, (BYTE*)player, 848)) {
            int s;
            BYTE* h = (BYTE*)*Player_FromId(&s);
            CallC(FUN_478260, player, *(const Blob32*)(h + 40), 2);
        } else
            CallC(FUN_478630, player, 2);
    }
    Trace_Leave((const char*)s_retraceCompetitorRegular);
}

// [FUN_4285c0] reportCompetitionState(): with competitors, the human's new place (up to 9) is shown
extern "C" void __cdecl Ai_ReportCompetitionState()
{
    Trace_EnterAppend((const char*)s_reportCompetitionState);
    if (g_68a980 == g_68a984) {
        Trace_Tag((const DWORD*)g_tagMpl);
        Buf_AppendString(&g_trace, 0, (const char*)s_reportCompetitionState);
        return;
    }
    CallC(FUN_428230);
    int n = 0;
    double best = 1.7976931348623157e308;
    CallC(FUN_427f40, &n, &best);
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h != (BYTE*)g_players && F(int, h, 528) != F(int, h, 532)) {
        F(int, h, 532) = F(int, h, 528);
        int place = F(int, (BYTE*)s, 528);
        if (place >= 10)
            place = 9;
        if (place >= 0)
            Messages_ShowNumbered(place);
    }
    Trace_Leave((const char*)s_reportCompetitionState);
}

// [FUN_41b320] a goods record's distance: the route length from a position to its town's node, scaled
extern "C" void __cdecl Goods_SetDistance(BYTE* rec, PositionId from)
{
    int node;
    CallC(FUN_404e80, &node, g_towns[F(int, rec, 12)]);
    PositionId to;
    Position_InitNode(&to, 0, node);
    double len = 0;
    CallC(FUN_487500, 0, from, to, &len);
    F(double, rec, 24) = g_66f8a0 * len;
}

// [FUN_406160] finishes the road network: numbers each road's objects and lists them at their nodes, sums
// their lengths into the road's length (+24) and the running total (+40), and grows the road's box (+136)
// over points 0.05001 apart along each object
extern "C" void __cdecl Road_FinishNetwork()
{
    double total = 0;
    int i = 0;
    for (RoadEdge** ep = g_roadEdges; ep != g_roadEdgesEnd; ep++, i++) {
        BYTE* e = (BYTE*)*ep;
        int k = 0;
        for (BYTE** o = F(BYTE**, e, 120); o != F(BYTE**, e, 124); o++, k++) {
            F(int, *o, 24) = k;
            BYTE* obj = *o;
            DWORD* v = (DWORD*)(F(BYTE*, obj, 16) + 28);
            if (v[1] != v[2]) {
                if (v[1])
                    *(BYTE**)v[1] = obj;
                v[1] += 4;
            } else
                Vec4b_InsertK(v, 0, (DWORD*)v[1], (const DWORD*)o);
            F(int, *o, 12) = i;
        }
        double len = 0;
        for (unsigned j = 0; j < (unsigned)(F(BYTE**, e, 124) - F(BYTE**, e, 120)); j++)
            len = CallVirt<double>(F(BYTE**, e, 120)[j], 36) + len;
        F(double, e, 24) = len;
        total = total + F(double, e, 24);
        F(double, e, 40) = total;
        for (int j = 0; j < F(BYTE**, e, 124) - F(BYTE**, e, 120); j++) {
            PositionId pos;
            pos.road = i;
            pos.node = -1;
            pos.t = 0;
            pos.segment = 0;
            pos.object = j;
            pos.m_24 = 0;
            pos.m_28 = -1;
            while (!CallVirt<int>(F(BYTE**, e, 120)[j], 20, &pos.t, 0.05001, &pos.segment)) {
                DVec3 pt;
                CallThis(FUN_487f60, e + 136, Road_PointOf(&pt, pos));
            }
        }
    }
}

// [FUN_420cb0] a new computer player (type 8) placed at a position, heading for a town; checks the position,
// announces the player, raises its events and sets flag 0x20000 below the 64b6a8 mark
extern "C" void __cdecl Ai_NewPlayerAt(PositionId pos)
{
    BYTE* p = Ai_CreateNewPlayer(8, 0, -1);
    int id;
    int player = *CallC<int*>(FUN_422390, &id, p);
    Player_SetPosition(p, 0, &pos);
    Ai_GoToTownSmooth((BYTE*)player, pos);
    AdvanceRoad_Check((const PositionId*)((BYTE*)player + 40), 7494);
    if (g_68d31c)
        CallC(FUN_46dc50, p);
    Rec_4015a0Both(player);
    if (!(CallC<double>(FUN_4842f0, (BYTE*)player + 24) >= g_64b6a8))
        CallC(FUN_446c80, player);
}

// ---------------------------------------------------------------------------
// Batch: rope appends, hash table growth, order info copies, road object searches, the human's place

extern "C" void FUN_459150(); KING_ALIAS("_FUN_459150", "_SchString_FromChars")    // a rope leaf with a copy of (s, len)
ASM_FUNC(FUN_45a2b0, _Rope_ConcatCharIter)    // rope::_S_concat_char_iter (r, s, len)
extern "C" void FUN_462700(); KING_ALIAS("_FUN_462700", "_SchString_Concat")    // rope::_S_leaf_concat_char_iter (leaf, s, len)
extern "C" void FUN_45a0e0(); KING_ALIAS("_FUN_45a0e0", "_Str_Rounded")    // rope::_S_rounded_up_size (n)
extern "C" void FUN_466560(); KING_ALIAS("_FUN_466560", "_CopyN")    // uninitialized_copy_n (out pair, s, n, dest, -)
ASM_FUNC(FUN_45a5e0, _Rope_TreeConcat)    // rope::_S_tree_concat (left, right)
extern "C" void FUN_44f1b0(); KING_ALIAS("_FUN_44f1b0", "@Rope_FreeTree@8")    // frees a rope node (thiscall)
extern "C" void FUN_44fde0(); KING_ALIAS("_FUN_44fde0", "_LowerBound")    // lower_bound over the primes (first, last, &n)
extern "C" void FUN_44a550(); KING_ALIAS("_FUN_44a550", "@Vec4_Reserve@16")    // vector<4 bytes> storage for n (thiscall: n, alloc)
extern "C" void FUN_466c20(); KING_ALIAS("_FUN_466c20", "_UninitFill4")    // uninitialized_fill_n (first, n, &x, -)
extern "C" void FUN_44fcd0(); KING_ALIAS("_FUN_44fcd0", "@NameHash_Hash@12")    // the string hash (thiscall: &key)
extern "C" void FUN_46cf00(); KING_ALIAS("_FUN_46cf00", "_Swap32")    // iter_swap (a, b)
extern "C" void FUN_40cda0(); KING_ALIAS("_FUN_40cda0", "@Rec40cda0_Copy@12")    // construct a 24-byte record (thiscall: src)
extern "C" void FUN_454580(); KING_ALIAS("_FUN_454580", "@Str_CopyConstruct@12")    // script handle copy (thiscall: src)
extern "C" void FUN_450850(); KING_ALIAS("_FUN_450850", "@Vec24Rec_Copy@12")    // vector<24 bytes> copy (thiscall: src)
extern "C" void FUN_463490(); KING_ALIAS("_FUN_463490", "_Rec24c_CopyBack")    // copy_backward of 24-byte records (first, last, result, -, 0)
extern "C" void FUN_40c100(); KING_ALIAS("_FUN_40c100", "@Str_Assign@12")    // script handle assign (thiscall: src)
extern "C" void FUN_45aa00(); KING_ALIAS("_FUN_45aa00", "@Vec24_Assign@12")    // vector<24 bytes> assign (thiscall: src)
extern "C" void FUN_46c900(); KING_ALIAS("_FUN_46c900", "@Mult46c900_DeletingDestructor@12")    // a 24-byte element's destructor (thiscall: flags)
extern "C" void FUN_44a500(); KING_ALIAS("_FUN_44a500", "@Vec24_Free@8")    // vector storage free (thiscall)
extern "C" void FUN_4a36c0(); KING_ALIAS("_FUN_4a36c0", "@SchHandle_Release@8")    // script handle destructor (thiscall)
extern "C" void FUN_466e50(); KING_ALIAS("_FUN_466e50", "_Recs24_UninitCopy")    // uninitialized_copy of 24-byte records (first, last, dest, -)
extern "C" void FUN_448300(); KING_ALIAS("_FUN_448300", "@Vec24_Destroy@8")    // vector<24 bytes> destructor (thiscall)
extern "C" void FUN_425790(); KING_ALIAS("_FUN_425790", "_Player_CheckGoods")
extern "C" void FUN_41ef90(); KING_ALIAS("_FUN_41ef90", "_Player_CheckTownAA")
extern "C" void FUN_56f6d0(); KING_ALIAS("_FUN_56f6d0", "_Copter_ApplyCargo")
extern "C" void FUN_477880(); KING_ALIAS("_FUN_477880", "_Player_Driver")    // a player's current order (player)
extern "C" void FUN_42c0b0(); KING_ALIAS("_FUN_42c0b0", "_Ai_MarkAmbushes")
extern "C" void FUN_442850(); KING_ALIAS("_FUN_442850", "_Ai_AccountForGuilt")    // accountForGuilt (player)
KING_VAR(AsmLabel, s_calculateNewPlace, 66dab4)
KING_VAR(AsmLabel, s_accountForChangeInLoadState, 66f124)
KING_VAR(double, g_64b898, 64b898)
typedef unsigned Primes[28];
KING_VAR(Primes, g_primes64b530, 64b530)

// [FUN_44f340] rope::_S_destr_concat_char_iter: r with len characters of s appended, in place when r is
// unshared and it (or its right child) is a leaf with room
extern "C" BYTE* __cdecl Rope_DestrConcatChars(BYTE* r, const char* s, unsigned len)
{
    if (!r)
        return CallC<BYTE*>(FUN_459150, s, len);
    unsigned size = F(unsigned, r, 0);
    if (F(unsigned, r, 4) > 1)
        return CallC<BYTE*>(FUN_45a2b0, r, s, len);
    if (!len) {
        F(int, r, 4) = 2;
        return r;
    }
    unsigned newSize = size + len;
    if (newSize <= 23 && F(char, r, 8) == 0) {
        if (F(int, r, 4) > 1)
            return CallC<BYTE*>(FUN_462700, r, s, len);
        if (CallC<unsigned>(FUN_45a0e0, size) - 1 < size + len)
            return CallC<BYTE*>(FUN_462700, r, s, len);
        DWORD out[2];
        CallC(FUN_466560, out, s, len, F(char*, r, 20) + size, s);
        F(char*, r, 20)[size + len] = 0;
        F(unsigned, r, 0) = size + len;
        F(int, r, 4) = 2;
        return r;
    }
    BYTE* right;
    if (F(char, r, 8) == 1 && F(char, right = F(BYTE*, r, 24), 8) == 0 && F(unsigned, right, 0) + len <= 23) {
        unsigned rsize = F(unsigned, right, 0);
        BYTE* nr;
        if (F(int, right, 4) <= 1 && !(CallC<unsigned>(FUN_45a0e0, rsize) - 1 < rsize + len)) {
            DWORD out[2];
            CallC(FUN_466560, out, s, len, F(char*, right, 20) + rsize);
            F(char*, right, 20)[rsize + len] = 0;
            F(unsigned, right, 0) = rsize + len;
            F(int, right, 4) = 2;
            nr = right;
            F(int, nr, 4) = 1;
        } else {
            nr = CallC<BYTE*>(FUN_462700, right, s, len);
            if (right == nr)
                F(int, nr, 4) = 1;
            else if (!InterlockedDecrement((LONG*)(right + 4)))
                CallThis(FUN_44f1b0, right);
        }
        F(int, r, 4) = 2;
        F(BYTE*, r, 24) = nr;
        F(unsigned, r, 0) = newSize;
        void* c = F(void*, r, 16);
        if (c) {
            if (newSize + 1)
                Ai_PoolFree(c, newSize + 1);
            F(void*, r, 16) = 0;
        }
        return r;
    }
    right = CallC<BYTE*>(FUN_459150, s, len);
    InterlockedIncrement((LONG*)(r + 4));
    return CallC<BYTE*>(FUN_45a5e0, r, right);
}

// [FUN_44fac0] hashtable::resize(hint) (thiscall): past the bucket count, rehashes into the next prime's buckets
extern "C" void __fastcall NameHash_Resize(BYTE* self, void*, unsigned hint)
{
    unsigned old = (unsigned)(F(BYTE*, self, 8) - F(BYTE*, self, 4)) >> 2;
    if (hint <= old)
        return;
    unsigned key = hint;
    const unsigned* p = CallC<const unsigned*>(FUN_44fde0, &g_primes64b530[0], &g_primes64b530[28], &key);
    unsigned n = p == &g_primes64b530[28] ? g_primes64b530[27] : *p;
    if (n <= old)
        return;
    BYTE** tmp[3];
    char alloc;
    void* zero = 0;
    CallThis(FUN_44a550, tmp, n, &alloc);
    tmp[1] = CallC<BYTE**>(FUN_466c20, tmp[0], n, &zero, hint);
    for (unsigned b = 0; b < old; b++) {
        BYTE** buckets = F(BYTE**, self, 4);
        for (BYTE* node = buckets[b]; node; node = F(BYTE**, self, 4)[b]) {
            unsigned nb = CallThis<unsigned>(FUN_44fcd0, self, node + 4) % n;
            F(BYTE**, self, 4)[b] = *(BYTE**)node;
            *(BYTE**)node = (BYTE*)tmp[0][nb];
            tmp[0][nb] = node;
        }
    }
    BYTE** oldBegin = F(BYTE**, self, 4);
    F(BYTE**, self, 4) = tmp[0];
    BYTE** oldEnd = F(BYTE**, self, 8);
    F(BYTE**, self, 8) = tmp[1];
    tmp[0] = (BYTE**)oldBegin;
    tmp[1] = (BYTE**)oldEnd;
    CallC(FUN_46cf00, self + 12, &tmp[2]);
    unsigned cnt = (unsigned)((BYTE*)tmp[2] - (BYTE*)oldBegin) >> 2;
    if (cnt)
        Ai_PoolFree(oldBegin, cnt * 4);
}

// [FUN_453110] the 192-byte order info's copy constructor (thiscall)
extern "C" BYTE* __fastcall OrderInfo_CopyConstruct(BYTE* p, void*, const BYTE* s)
{
    Rec461400_Construct(p, s);
    return p;
}

// [FUN_453350] another copy constructor of the order info: all four vectors copied bytewise and the cached
// first elements at +100..+112 kept as they were (thiscall)
extern "C" BYTE* __fastcall OrderInfo_CopyRaw(BYTE* p, void*, const BYTE* s)
{
    F(DWORD, p, 0) = F(DWORD, s, 0);
    F(DWORD, p, 8) = F(DWORD, s, 8);
    F(DWORD, p, 12) = F(DWORD, s, 12);
    for (int k = 16; k < 48; k += 4)
        F(DWORD, p, k) = F(DWORD, s, k);
    F(DWORD, p, 48) = F(DWORD, s, 48);
    static const int vec[4] = {52, 64, 76, 88};
    for (int i = 0; i < 4; i++) {
        const BYTE* b = F(const BYTE*, s, vec[i]);
        unsigned n = F(const BYTE*, s, vec[i] + 4) - b;
        VecReserve((VecMethods*)(p + vec[i]), n);
        memmove(F(BYTE*, p, vec[i]), b, n);
        F(BYTE*, p, vec[i] + 4) = F(BYTE*, p, vec[i]) + n;
    }
    for (int k = 100; k <= 112; k += 4)
        F(DWORD, p, k) = F(DWORD, s, k);
    for (int k = 120; k <= 180; k += 4)
        F(DWORD, p, k) = F(DWORD, s, k);
    F(BYTE, p, 184) = F(BYTE, s, 184);
    return p;
}

// [FUN_4505a0] vector<24-byte record (script handle, vector<24 bytes>)>::insert_aux(pos, x) (thiscall)
extern "C" void __fastcall Rec24bVec_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    if (v[1] != v[2]) {
        if (v[1])
            CallThis(FUN_40cda0, v[1], v[1] - 24);
        v[1] += 24;
        BYTE t0[12], t1[12];
        CallThis(FUN_454580, t0, x);
        CallThis(FUN_450850, t1, x + 12);
        CallC(FUN_463490, pos, v[1] - 48, v[1] - 24, pos, 0);
        CallThis(FUN_40c100, pos, t0);
        CallThis(FUN_45aa00, pos + 12, t1);
        for (BYTE* e = F(BYTE*, t1, 0); e != F(BYTE*, t1, 4); e += 24)
            CallThis(FUN_46c900, e, 0);
        CallThis(FUN_44a500, t1);
        CallThis(FUN_4a36c0, t0);
        return;
    }
    unsigned len = Vec_Grow(v, 24);
    BYTE* buf = (BYTE*)Stl_Allocate(len * 24);
    BYTE* e = CallC<BYTE*>(FUN_466e50, v[0], pos, buf, pos);
    if (e)
        CallThis(FUN_40cda0, e, x);
    e = CallC<BYTE*>(FUN_466e50, pos, v[1], e + 24, pos);
    for (BYTE* d = v[0]; d != v[1]; d += 24) {
        CallThis(FUN_448300, d + 12);
        CallThis(FUN_4a36c0, d);
    }
    unsigned cnt = (unsigned)(v[2] - v[0]) / 24;
    if (cnt)
        Ai_PoolFree(v[0], cnt * 24);
    v[0] = buf;
    v[1] = e;
    v[2] = buf + len * 24;
}

// [FUN_450150] (thiscall on a road's objects) whether the objects between (t1, seg1, obj1) and (t2, seg2, obj2),
// in either order, hold a match for (a0..a3): the first and last objects by their parts, the ones between
// whole; the match's object goes to *out (the last one found)
extern "C" int __fastcall RoadObjs_FindBetween(BYTE*** self, void*, DWORD a0, DWORD a1, DWORD a2, DWORD a3,
                                               int* out, double t1, int seg1, int obj1, double t2, int seg2,
                                               int obj2)
{
    for (;;) {
        bool swap;
        if (obj2 < obj1)
            swap = true;
        else if (obj2 != obj1)
            swap = false;
        else if (seg2 < seg1)
            swap = true;
        else if (seg2 != seg1)
            swap = false;
        else
            swap = !(t2 >= t1);
        if (!swap)
            break;
        double t = t1; t1 = t2; t2 = t;
        int s = seg1; seg1 = seg2; seg2 = s;
        int o = obj1; obj1 = obj2; obj2 = o;
    }
    BYTE** objs = *self;
    if (obj1 == obj2) {
        if (!CallVirt<int>(objs[obj1], 8, a0, a1, a2, a3, t1, seg1, t2, seg2))
            return 0;
        *out = obj1;
        return 1;
    }
    int found = 0;
    int last = CallVirt<int>((*self)[obj1], 24) - 1;
    if (CallVirt<int>((*self)[obj1], 8, a0, a1, a2, a3, t1, seg1, 1.0, last)) {
        found = 1;
        *out = obj1;
    }
    for (int i = obj1 + 1; i < obj2; i++)
        if (CallVirt<int>((*self)[i], 12, a0, a1, a2, a3)) {
            found = 1;
            *out = i;
        }
    if (CallVirt<int>((*self)[obj2], 8, a0, a1, a2, a3, 0.0, 0, t2, seg2)) {
        *out = obj2;
        return 1;
    }
    return found;
}

// [FUN_427f40] calculateNewPlace(int & newPlace, double & mindelta): how many players are ahead of the
// human, and the smallest distance to one of them
extern "C" void __cdecl Ai_CalculateNewPlace(int* place, double* minDelta)
{
    Trace_EnterAppend((const char*)s_calculateNewPlace);
    *place = 0;
    *minDelta = 1.7976931348623157e308;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        if (!CallC<char>(FUN_4172e0, (int)it))
            continue;
        double lead;
        if (!Ai_HumanLead(it, &lead))
            ++*place;
        double d = lead < 0 ? -lead : lead;
        if (!(d >= *minDelta))
            *minDelta = d;
    }
    Trace_Leave((const char*)s_calculateNewPlace);
}

// [FUN_442c60] accountForChangeInLoadState(PlayerId player)
extern "C" void __cdecl Ai_AccountForChangeInLoadState(int player)
{
    Trace_EnterAppend((const char*)s_accountForChangeInLoadState);
    CallC(FUN_442850, player);
    CallC(FUN_425790, player);
    CallC(FUN_41ef90, player);
    CallC(FUN_477210, player);
    if (CallC<char>(FUN_417660, player))
        CallC(FUN_56f6d0);
    BYTE* p = (BYTE*)player;
    int order = CallC<int>(FUN_477880, player);
    if (order) {
        double pay = CallC<double>(FUN_4715a0, order, 0.0, 1) * g_64b840;
        F(double, p, 960) = pay + F(double, p, 960) * g_64b898;
    } else
        F(DWORD, p, 96) &= 0xfeefffff;
    CallC(FUN_42c0b0);
    Trace_Leave((const char*)s_accountForChangeInLoadState);
}

// ---------------------------------------------------------------------------
// Batch: ambush marks, the competition place, advancements, setting positions, orientations, sending players

KING_VAR(AsmLabel, s_markAmbushes, 66dca4)
KING_VAR(AsmLabel, s_recalculateCompetitionPlace, 66daec)
KING_VAR(AsmLabel, s_updateAdvancementsRegular, 66db28)
KING_VAR(AsmLabel, s_setPlayerPositionRoom, 66eae0)
KING_VAR(AsmLabel, s_getOrientation, 66e6e0)
KING_VAR(AsmLabel, s_sendForSomething, 66d44c)
KING_VAR(double, g_64b878, 64b878)
KING_VAR(double, g_64b670, 64b670)
KING_VAR(double, g_64b6b0, 64b6b0)
typedef DVec3 Offsets4[4];
KING_VAR(Offsets4, g_offsets68cc28, 68cc28)
extern "C" void FUN_429820(); KING_ALIAS("_FUN_429820", "@PlayerId_Over020@8")    // (no args) returns bool
extern "C" void FUN_487e50(); KING_ALIAS("_FUN_487e50", "_DMatrix_Multiply")    // matrix product (out, a, b)
extern "C" void FUN_436bd0(); KING_ALIAS("_FUN_436bd0", "_Ai_GetPositionIdRoom")    // getPositionId in a room (out, room, v)
extern "C" void FUN_43dc60(); KING_ALIAS("_FUN_43dc60", "_Ai_SetPlayerPosition")    // setPlayerPosition (player, v)
extern "C" void FUN_4390a0(); KING_ALIAS("_FUN_4390a0", "_Ai__GetTangent")    // a position's direction (out, pos)
extern "C" void FUN_4397e0(); KING_ALIAS("_FUN_4397e0", "_Ai_OrientationFrom")    // an orientation (out, point, direction)
extern "C" void FUN_418fe0(); KING_ALIAS("_FUN_418fe0", "_Player_WaitsInTownOf")    // (player) returns bool
extern "C" void FUN_474210(); KING_ALIAS("_FUN_474210", "@Player_GetPlace@8")    // (thiscall player) its vehicle
extern "C" void FUN_419030(); KING_ALIAS("_FUN_419030", "_Ai_GoHireIt")    // goHireIt (player)
extern "C" void FUN_417820(); KING_ALIAS("_FUN_417820", "_Ai_ArriveAt")    // (player, pos)
extern "C" void FUN_4185d0(); KING_ALIAS("_FUN_4185d0", "_Ai_TruckerNextOrder")    // (player)

static bool Ambush_Roll()
{
    unsigned r = AiRandom() % 1000000;
    return (double)r < g_64b878;
}

// [FUN_42c0b0] markAmbushes(): marks (+240) each kind-2 ambush, and each kind-1 one while the human carries
// something (or its flag at +201 is set) and FUN_429820 allows it, by a random roll under 64b878 per million
extern "C" void __cdecl Ai_MarkAmbushes()
{
    Trace_EnterAppend((const char*)s_markAmbushes);
    for (void** e = g_68a970; e != g_68a974; e++) {
        BYTE* a = (BYTE*)*e;
        F(BYTE, a, 240) = 0;
        int kind = F(int, a, 36);
        if (kind == 2)
            F(BYTE, a, 240) = Ambush_Roll();
        else if (kind == 1) {
            int s;
            BYTE* h = (BYTE*)*Player_FromId(&s) + 200;
            if (!(F(double, h, 24) > g_zero64b678) && !F(char, h, 1))
                continue;
            bool mark = false;
            if (CallC<char>(FUN_429820))
                mark = Ambush_Roll();
            F(BYTE, (BYTE*)*e, 240) = mark;
        }
    }
    Trace_Leave((const char*)s_markAmbushes);
}

// [FUN_428230] recalculateCompetitionPlace(): the human's place (+528) among the competitors (+524 their
// count); when the closest one is over 12 away, the place it would have at the four offsets of 68cc28 from
// its matrix decides (the advancement stays at the first better one)
extern "C" void __cdecl Ai_RecalculateCompetitionPlace()
{
    Trace_EnterBuffered((const char*)s_recalculateCompetitionPlace);
    int n = 0;
    double best = 1.7976931348623157e308;
    Ai_CalculateNewPlace(&n, &best);
    if (best > g_64b710) {
        int s;
        BYTE* h = (BYTE*)*Player_FromId(&s);
        if (h != (BYTE*)g_players) {
            F(int, h, 524) = (g_68a984 - g_68a980) + g_68d164;
            int place = g_68d164 + n;
            if (!g_68d188 && F(int, (BYTE*)s, 100) != 5) {
                h = (BYTE*)s;
                BYTE mat[96];
                memcpy(mat, h + 552, 96);
                for (int k = 0; k < 4; k++) {
                    BYTE m[96], mc[96], out[96];
                    Rec_InitTimes(m, 0, 1.0);
                    DVec3_Assign((DVec3*)(m + 72), 0, &g_offsets68cc28[k]);
                    memcpy(mc, m, 96);
                    CallC(FUN_487e50, out, mc, mat);
                    PositionId pos;
                    Ai_GetPositionIdEx(&pos, F(int, (BYTE*)s, 852), (const DVec3*)(out + 72));
                    if (pos.road == -1 && pos.node == -1 && !(pos.t < 0 || pos.t > 0) && !pos.segment && !pos.object
                        && !pos.m_24)
                        continue;
                    Ai_UpdateAdvancementRegular((BYTE*)s + 8, pos);
                    Ai_CalculateNewPlace(&n, &best);
                    if (place > (int)(g_68d164 + n)) {
                        F(int, (BYTE*)s, 528) = g_68d164 + n;
                        Trace_Tag((const DWORD*)g_tagMpl);
                        Buf_AppendString(&g_trace, 0, (const char*)s_recalculateCompetitionPlace);
                        return;
                    }
                }
                Ai_UpdateAdvancementRegular((BYTE*)s + 8, F(PositionId, (BYTE*)s, 40));
            }
            F(int, (BYTE*)s, 528) = place;
        }
    }
    Trace_Leave((const char*)s_recalculateCompetitionPlace);
}

// [FUN_4287b0] updateAdvancementsRegular(): the human's and each racer's advancement; a racer whose
// advancement (+496) changed by more than 64b670 flips its flag at +505
extern "C" void __cdecl Ai_UpdateAdvancementsRegular()
{
    Trace_EnterBuffered((const char*)s_updateAdvancementsRegular);
    int s;
    BYTE* h;
    if (g_68a980 == g_68a984 || (h = (BYTE*)*Player_FromId(&s)) == (BYTE*)g_players || F(int, h, 100) == 5) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, (const char*)s_updateAdvancementsRegular);
        return;
    }
    Ai_UpdateAdvancementRegular(h + 8, F(PositionId, h, 40));
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = CallC<BYTE*>(FUN_40a9f0, it);
        if (!CallC<char>(FUN_4172e0, (int)it))
            continue;
        double before = F(double, d, 496);
        Ai_UpdateAdvancementRegular(d, F(PositionId, d, 32));
        double ch = F(double, d, 496) - before;
        if (ch < 0)
            ch = -ch;
        if (ch > g_64b670)
            F(BYTE, d, 505) = F(BYTE, d, 505) == 0;
    }
    Trace_Leave((const char*)s_updateAdvancementsRegular);
}

// [FUN_43e2a0] _setPlayerPosition(RoomId room, PlayerId player, dvector3 const & v): a player without
// hops goes to the room's closest road object position, otherwise (or with no room) setPlayerPosition
extern "C" void __cdecl Ai_SetPlayerPositionRoom(int room, int player, const DVec3* v)
{
    Trace_EnterBuffered((const char*)s_setPlayerPositionRoom);
    BYTE* p = (BYTE*)player;
    BYTE* r;
    if (F(BYTE*, p, 24) == F(BYTE*, p, 28) && room != -1
        && F(BYTE*, r = g_vec68d0d8[room], 28) != F(BYTE*, r, 32)) {
        PositionId pos;
        CallC(FUN_436bd0, &pos, room, v);
        PositionId chk = pos;
        if (!PositionValid(&chk)) {
            sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", pos.road, pos.t, pos.segment, pos.object, pos.m_24);
            AiError(33, 100, s_mainFile, s_mainId, 16313, s_invalidId, "PositionId", g_positionText77e6e0);
        }
        Player_SetPosition(p + 8, 0, &pos);
    } else
        CallC(FUN_43dc60, player, v);
    Trace_Leave((const char*)s_setPlayerPositionRoom);
}

// [FUN_439a70] _getOrientation(PositionId pos): the orientation at a position's point along its direction
extern "C" BYTE* __cdecl Ai_GetOrientation(BYTE* out, PositionId pos)
{
    Trace_EnterAppend((const char*)s_getOrientation);
    AdvanceRoad_Check(&pos, 15408);
    DVec3 dir, pt;
    void* d = CallC<void*>(FUN_4390a0, &dir, pos);
    CallC(FUN_4397e0, out, Road_PointOf(&pt, pos), d);
    Trace_Leave((const char*)s_getOrientation);
    return out;
}

// [FUN_4194e0] sendForSomething(PlayerId player): with hire places free, an idle kind-2 player whose dealer
// (+520, else +516) has nothing goes to hire; otherwise by a roll against its vehicle's +40 it takes its next
// order or arrives where it is
extern "C" void __cdecl Ai_SendForSomething(int player)
{
    Trace_EnterBuffered((const char*)s_sendForSomething);
    BYTE* p = (BYTE*)player;
    if (!g_68d188) {
        bool done = false;
        if (g_68d14c < g_68d150 && F(int, p, 92) == 2 && !F(int, p, 100) && CallC<char>(FUN_418fe0, player)) {
            BYTE* m = F(BYTE*, p, 520);
            DWORD* v;
            if (m && F(int, m, 4) != -1)
                v = (DWORD*)(m + 28);
            else if ((m = F(BYTE*, p, 516)) != 0 && F(int, m, 4) != -1)
                v = (DWORD*)(m + 28);
            else {
                AiError(0, 1, &g_strAiPlayH, s_mainId, 740, &g_strErr9012, &g_aiEmpty, &g_aiEmpty);
                v = (DWORD*)p;
            }
            if (v[0] == v[1]) {
                CallC(FUN_419030, player);
                done = true;
            }
        }
        if (!done) {
            BYTE* veh = CallThis<BYTE*>(FUN_474210, p + 8);
            double x = F(double, veh, 40);
            unsigned r = AiRandom() % 1000000;
            if (x * g_64b6b0 > (double)r)
                CallC(FUN_4185d0, player);
            else
                CallC(FUN_417820, player, F(PositionId, p, 40));
        }
    }
    Trace_Leave((const char*)s_sendForSomething);
}

// ---------------------------------------------------------------------------
// Batch: the AI tick, players' moves and checks, loading a hire order, distances along a road

KING_VAR(AsmLabel, s_getPositionIdAll, 66e4a4)
KING_VAR(AsmLabel, s_processMove, 66d56c)
KING_VAR(AsmLabel, s_noRoads66e45c, 66e45c)
KING_VAR(AsmLabel, s_err59812, 66e444)
KING_VAR(AsmLabel, s_err7644, 66cb74)
KING_VAR(AsmLabel, s_err7645, 66cb60)
KING_VAR(AsmLabel, s_err9903, 66dda0)
KING_VAR(AsmLabel, s_err9967, 66dd8c)
KING_VAR(AsmLabel, s_err9969, 66dd78)
KING_VAR(AsmLabel, s_err9971, 66dd64)
KING_VAR(AsmLabel, s_err9972, 66dd50)
KING_VAR(double, t_68a820, 68a820)
KING_VAR(BYTE, t_68b998, 68b998)
KING_VAR(double, t_68d0f8, 68d0f8)
KING_VAR(double, t_68d198, 68d198)
extern "C" void FUN_483a20(); KING_ALIAS("_FUN_483a20", "_AI_Time")    // returns double
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_424550(); KING_ALIAS("_FUN_424550", "_Ai_LoadEntities")
extern "C" void FUN_42d960(); KING_ALIAS("_FUN_42d960", "@Tasks_RunDue@8")    // (thiscall 68b998)
extern "C" void FUN_446af0(); KING_ALIAS("_FUN_446af0", "_Ai_KillDelayedPlayers")
extern "C" void FUN_423a90(); KING_ALIAS("_FUN_423a90", "_Ai_CreatePlayers")
extern "C" void FUN_477420(); KING_ALIAS("_FUN_477420", "_Player_ExpireLicences")    // (player)
extern "C" void FUN_416b80(); KING_ALIAS("_FUN_416b80", "_Ai_RaceNextCheckpoint")    // (player)
extern "C" void FUN_4171a0(); KING_ALIAS("_FUN_4171a0", "_Race_UpdateProgress")    // (player)
extern "C" void FUN_417300(); KING_ALIAS("_FUN_417300", "_Ai_ActOnSemiArrival")    // (player)
extern "C" void FUN_4165a0(); KING_ALIAS("_FUN_4165a0", "_Player_Idle")    // (player) returns bool
extern "C" void FUN_419cb0(); KING_ALIAS("_FUN_419cb0", "_Ai_ActOnArrival")    // (player)
extern "C" void FUN_42a4b0(); KING_ALIAS("_FUN_42a4b0", "_Ai_ProvideNoncontradictoryPositions")
extern "C" void FUN_423f40(); KING_ALIAS("_FUN_423f40", "_Ai_CalculatePlayerSpeedsAndFares")
extern "C" void FUN_42cb20(); KING_ALIAS("_FUN_42cb20", "_Ai_UpdatePlayerRooms")
extern "C" void FUN_42cf50(); KING_ALIAS("_FUN_42cf50", "_Ai_UpdatePlayerBoxes")
extern "C" void FUN_46dee0(); KING_ALIAS("_FUN_46dee0", "_Net_AddPlayerStates")
extern "C" void FUN_460d20(); KING_ALIAS("_FUN_460d20", "_Rec458a80_Construct")    // construct a 144-byte order in place (dest, src)
extern "C" void FUN_450960(); KING_ALIAS("_FUN_450960", "@OrderVec_InsertAux@16")    // vector<order>::insert_aux (thiscall: pos, x)
extern "C" void FUN_402740(); KING_ALIAS("_FUN_402740", "@Pos_Before@40")    // whether a position comes before another (thiscall: other)
extern "C" void FUN_4165e0(); KING_ALIAS("_FUN_4165e0", "_Player_Flag4")    // (player) returns bool
extern "C" void FUN_477d60(); KING_ALIAS("_FUN_477d60", "_Player_EndDealOf")    // (player)
extern "C" void FUN_41a9d0(); KING_ALIAS("_FUN_41a9d0", "_Ai_PrescribeAdvance")    // (player) returns double
extern "C" void FUN_435a90(); KING_ALIAS("_FUN_435a90", "_Ai_AdvancePlayer")    // (player, double*)
extern "C" void FUN_419760(); KING_ALIAS("_FUN_419760", "_Ai_GoIdle")    // goIdle (player)
extern "C" void FUN_41ae60(); KING_ALIAS("_FUN_41ae60", "_Ai_ProcessMove")    // processMove (player)

// [FUN_42d390] checks every player's orders (+56 within [0, 64b6b0]) and that the kind-9 ones and only they
// are in state 256 with flags 4 and 0x40
extern "C" void __cdecl Ai_CheckPlayers()
{
    BYTE* vA = 0;
    BYTE* vB = 0;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = CallC<BYTE*>(FUN_40a9f0, it);
        if (!d)
            continue;
        vA = Player_Orders(it, vA);
        for (BYTE* o = F(BYTE*, vA, 0);; o += 144) {
            vB = Player_Orders(it, vB);
            if (o == F(BYTE*, vB, 4))
                break;
            double x = F(double, o, 56);
            if (!(x >= g_zero64b678) || x > g_64b6b0)
                AiError(0, 100, s_mainFile, s_mainId, 11684, s_err9903, &g_aiEmpty, &g_aiEmpty);
        }
        if (F(int, d, 84) == 9) {
            if (!(F(DWORD, d, 88) >> 2 & 1))
                AiError(0, 100, s_mainFile, s_mainId, 11691, s_err9967, &g_aiEmpty, &g_aiEmpty);
            if (!(F(BYTE, d, 88) & 64))
                AiError(0, 100, s_mainFile, s_mainId, 11693, s_err9969, &g_aiEmpty, &g_aiEmpty);
            if (F(int, d, 92) != 256)
                AiError(0, 100, s_mainFile, s_mainId, 11695, s_err9971, &g_aiEmpty, &g_aiEmpty);
        }
        if (F(int, d, 92) == 256 && F(int, d, 84) != 9)
            AiError(0, 100, s_mainFile, s_mainId, 11700, s_err9972, &g_aiEmpty, &g_aiEmpty);
    }
}

// [FUN_42d6e0] the AI's tick: clocks and the human's camera, then each player's move, then the world's
// updates
extern "C" void __cdecl Ai_Tick()
{
    Ai_TraceHumanState();
    t_68a820 = CallC<double>(FUN_483a20);
    t_68b998 = 1;
    double now = CallC<double>(FUN_401000);
    t_68d0f8 = now;
    g_68d190 = now - t_68d198;
    Ai_CheckPlayers();
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h != (BYTE*)g_players)
        g_68bab0 = F(DVec3, h, 624);
    CallC(FUN_5e48f0);
    r_68d134++;
    if (r_66c60a) {
        g_loadTick68d13c = 200;
        CallC(FUN_424550);
        r_66c60a = 0;
    }
    CallThis(FUN_42d960, &t_68b998);
    CallC(FUN_446af0);
    if (!g_68d31c)
        CallC(FUN_423a90);
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        if (g_68d31c && (F(DWORD, CallC<BYTE*>(FUN_40a9f0, it), 88) & 0x100))
            continue;
        int p = (int)it;
        CallC(FUN_41ae60, p);
        if (!g_68d188)
            CallC(FUN_477420, p);
        if (g_68d188 == 1 || !g_68d188 && F(int, CallC<BYTE*>(FUN_40a9f0, it), 92) == 5)
            CallC(FUN_416b80, p);
        if (g_68d188 == 2)
            CallC(FUN_4171a0, p);
        CallC(FUN_417300, p);
        if (CallC<char>(FUN_4165a0, p))
            CallC(FUN_419cb0, p);
    }
    CallC(FUN_42a4b0);
    if (!g_68d188)
        Ai_HumanRaceTick();
    CallC(FUN_423f40);
    CallC(FUN_424550);
    if (!g_68d188)
        Ai_UpdateAdvancementsRegular();
    Ai_ReportCompetitionState();
    CallC(FUN_42cb20);
    CallC(FUN_42cf50);
    CallC(FUN_46dee0);
    g_lastSpeedTime68d0f0 = t_68d0f8;
}

// [FUN_425820] a computer player takes on an order: its amounts (FUN_41b910) against the cargo's limits pick
// the smaller ratio, which sets the copied order's amount (+56) and its share of the order's +64 (+64)
extern "C" void __cdecl Ai_TakeOrder(int player, const BYTE* order)
{
    if (CallC<char>(FUN_417660, player))
        AiError(0, 1, s_mainFile, s_mainId, 8929, &g_strErr9012, &g_aiEmpty, &g_aiEmpty);
    const BYTE* rec = g_68ccb8 + F(int, order, 0) * 104;
    double lim1 = F(double, rec, 16);
    double lim2 = F(double, rec, 24);
    double a = 0, b = 0;
    CallC(FUN_41b910, player, &a, &b);
    double x = a / lim1;
    double y = b / lim2;
    BYTE* p = (BYTE*)player;
    BYTE* fb = (BYTE*)F(DWORD, &x, 0);
    BYTE** v = (BYTE**)Player_Orders(p, fb);
    if (v[1] != v[2]) {
        CallC(FUN_460d20, v[1], order);
        v[1] += 144;
    } else
        CallThis(FUN_450960, v, v[1], order);
    const double* m = !(x >= y) ? &x : &y;
    BYTE* w = Player_Orders(p, fb);
    F(double, F(BYTE*, w, 4) - 88, 0) = *m;
    m = !(x >= y) ? &x : &y;
    w = Player_Orders(p, fb);
    F(double, F(BYTE*, w, 4) - 80, 0) = F(double, order, 64) * *m / F(double, order, 56);
    Ai_AccountForChangeInLoadState(player);
}

// the length along a road's objects from (t1, s1, o1) to (t2, s2, o2); o2 past the last object means its end
static double Road_SpanLength(BYTE* e, double t1, int s1, int o1, double t2, int s2, int o2)
{
    BYTE** objs = F(BYTE**, e, 120);
    if (o2 == F(BYTE**, e, 124) - objs) {
        o2--;
        s2 = CallVirt<int>(objs[o2], 24) - 1;
        t2 = 0;
    }
    if (o1 == o2)
        return CallVirt<double>(F(BYTE**, e, 120)[o1], 32, t1, s1, t2, s2);
    BYTE* first = F(BYTE**, e, 120)[o1];
    int last = CallVirt<int>(first, 24) - 1;
    double sum = CallVirt<double>(first, 32, t1, s1, 1.0, last);
    for (int i = o1 + 1; i < o2; i++)
        sum = CallVirt<double>(F(BYTE**, e, 120)[i], 36) + sum;
    return CallVirt<double>(F(BYTE**, e, 120)[o2], 32, 0.0, 0, t2, s2) + sum;
}

// a position at one of the road's end nodes becomes the road's start or end
static void Road_PositionAtEnd(BYTE* e, PositionId* p, int line, const char* err)
{
    if (p->road != -1)
        return;
    RoadNode* n = g_roadNodes[p->node];
    PositionId tmp;
    if (n == F(RoadNode*, e, 112))
        *p = *Road_EdgeStart(&tmp, (RoadEdge*)e);
    else if (n == F(RoadNode*, e, 116))
        *p = *Road_EdgeEnd(&tmp, (RoadEdge*)e);
    else
        AiError(0, 100, s_mainFile, s_mainId, line, err, &g_aiEmpty, &g_aiEmpty);
}

// [FUN_409660] the distance along a road between two positions on it
extern "C" double __cdecl Road_DistanceBetween(BYTE* e, PositionId a, PositionId b)
{
    Road_PositionAtEnd(e, &a, 2075, s_err7644);
    Road_PositionAtEnd(e, &b, 2086, s_err7645);
    if (CallThis<char>(FUN_402740, &a, b))
        return Road_SpanLength(e, a.t, a.segment, a.object, b.t, b.segment, b.object);
    return Road_SpanLength(e, b.t, b.segment, b.object, a.t, a.segment, a.object);
}

// [FUN_436900] getPositionId(dvector3 const & v): the closest position to v on any road
extern "C" PositionId* __cdecl Ai_GetPositionIdAll(PositionId* out, const DVec3* v)
{
    Trace_EnterBuffered((const char*)s_getPositionIdAll);
    double best = 1.7976931348623157e308;
    double t = 0;
    int seg = 0, obj = 0;
    if (g_roadEdges == g_roadEdgesEnd)
        AiError(0, 100, s_mainFile, s_mainId, 14766, s_noRoads66e45c, &g_aiEmpty, &g_aiEmpty);
    bool none = true;
    RoadEdge** bestEdge = 0;
    for (RoadEdge** ep = g_roadEdges; ep != g_roadEdgesEnd; ep++) {
        BYTE*** vec = (BYTE***)((BYTE*)*ep + 120);
        bool found = false;
        for (unsigned k = 0; k < (unsigned)(vec[1] - vec[0]); k++)
            if (CallVirt<int>(vec[0][k], 12, v, &best, &t, &seg)) {
                found = true;
                obj = k;
            }
        if (found) {
            bestEdge = ep;
            none = false;
        }
    }
    if (none)
        AiError(0, 100, s_mainFile, s_mainId, 14780, s_err59812, &g_aiEmpty, &g_aiEmpty);
    PositionId pos;
    pos.road = bestEdge - g_roadEdges;
    pos.node = -1;
    pos.t = t;
    pos.segment = seg;
    pos.object = obj;
    pos.m_24 = 0;
    pos.m_28 = -1;
    if (!PositionValid(&pos)) {
        sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", pos.road, pos.t, pos.segment, pos.object, 0);
        AiError(33, 100, s_mainFile, s_mainId, 14783, s_invalidId, "PositionId", g_positionText77e6e0);
    }
    *out = pos;
    Trace_Leave((const char*)s_getPositionIdAll);
    return out;
}

// [FUN_41ae60] processMove(PlayerId player): ends a finished hire or sale, takes the next order when due,
// and for a free player that isn't moving starts a move
extern "C" int __cdecl Ai_ProcessMove(int player)
{
    Trace_EnterBuffered((const char*)s_processMove);
    BYTE* p = (BYTE*)player;
    if (F(int, p, 100) == 256 && CallC<char>(FUN_4165e0, player) && CallC<double>(FUN_401000) >= F(double, p, 112)) {
        BYTE* m = F(BYTE*, p, 972);
        if (F(int, m, 12) >= F(int, m, 8))
            CallC(FUN_419760, player);
    }
    if (F(int, p, 100) == 257 && CallC<char>(FUN_4165e0, player) && CallC<double>(FUN_401000) >= F(double, p, 112)
        && g_68d154 >= g_68d150)
        CallC(FUN_419760, player);
    if (CallC<char>(FUN_4165e0, player) && F(int, p, 100) != 257 && F(int, p, 100) != 256) {
        if (r_66c68c || CallC<char>(FUN_4165e0, player) && CallC<double>(FUN_401000) >= F(double, p, 112))
            CallC(FUN_477d60, player);
    }
    if ((F(DWORD, p, 96) & 0x8000000) && CallC<double>(FUN_401000) >= F(double, p, 112))
        CallC(FUN_477d60, player);
    if (!(F(BYTE, p, 96) & 1) && !CallC<char>(FUN_4165e0, player)) {
        double d = CallC<double>(FUN_41a9d0, player);
        CallC(FUN_435a90, player, &d);
        Ai_PlayerEvent(p, 4, 0);
    }
    Trace_Leave((const char*)s_processMove);
    return 0;
}

// ---------------------------------------------------------------------------
// Batch: setting a player's position with the room change, a position's direction bit, starting races,
// a forfeit

KING_VAR(AsmLabel, s_setPlayerPositionTo66eb70, 66eb70)
KING_VAR(AsmLabel, s_setPlayerPositionRoomLimited, 66ebb8)
KING_VAR(AsmLabel, s_getRightDirection66f340, 66f340)
KING_VAR(AsmLabel, s_startRaces, 66da70)
KING_VAR(AsmLabel, s_checkPoint1, 66d168)
KING_VAR(AsmLabel, s_dropOrdersForfeit, 66f2c4)
KING_VAR(AsmLabel, s_idFormat66d4e4, 66d4e4)
KING_VAR(AsmLabel, s_playerId66d4d8, 66d4d8)
KING_VAR(AsmLabel, s_forfeit66f2bc, 66f2bc)
extern "C" void FUN_47fc80(); KING_ALIAS("_FUN_47fc80", "_Road_CommonNode")    // (out, road, road, DVec3)
extern "C" void FUN_40b100(); KING_ALIAS("_FUN_40b100", "_Ai_OnNodePass")    // (player, int, DVec3)
extern "C" void FUN_43cb50(); KING_ALIAS("_FUN_43cb50", "_Ai_SetPlayerPositionLimited")    // setPlayerPositionLimited (player, v)
extern "C" void FUN_4175c0(); KING_ALIAS("_FUN_4175c0", "_Player_MarkListed")    // (player)
extern "C" void FUN_4272d0(); KING_ALIAS("_FUN_4272d0", "_Ai_ClearPlayerFromTail")    // clearPlayerFromTail (player)
extern "C" void FUN_4165f0(); KING_ALIAS("_FUN_4165f0", "_Ai_CalculateWeightsPerSubstate")    // calculateWeigthsPerSubstate
extern "C" void FUN_436510(); KING_ALIAS("_FUN_436510", "_Node_ByName")    // a node by name (out, name)
extern "C" void FUN_486800(); KING_ALIAS("_FUN_486800", "_Road_PositionCost")    // the route length between two positions (int, from, to, double* out)
extern "C" void FUN_471420(); KING_ALIAS("_FUN_471420", "_Order_FullValue")    // an order's value (order) returns double
extern "C" void FUN_483e60(); KING_ALIAS("_FUN_483e60", "_AI_AddToBanner")    // AI_AddToBanner (double)
extern "C" void FUN_483e80(); KING_ALIAS("_FUN_483e80", "_eventBanner")    // eventBanner (int, int)

static void RoomInfo_Init(RoomInfo* r)
{
    r->flags = 0;
    r->m_8 = 0;
    r->m_12 = 0;
    r->module = g_undefinedModule;
    r->room = g_undefinedRoom;
    r->m_24 = 0;
    r->m_28 = 0;
    r->m_36 = -1;
    r->m_40 = -1;
}

// after a player's position was set: when it changed roads, FUN_40b100 with the step between the two points
static void Player_RoadChanged(int player, const PositionId& p0)
{
    RoomInfo b;
    RoomInfo_Init(&b);
    PositionId p1 = F(PositionId, (BYTE*)player, 40);
    Road_RoomInfo(p1, &b);
    if (p0.road == p1.road)
        return;
    DVec3 a0, a1;
    const DVec3* pt0 = Road_PointOf(&a0, p0);
    const DVec3* pt1 = Road_PointOf(&a1, p1);
    DVec3 d;
    d.x = pt1->x - pt0->x;
    d.y = pt1->y - pt0->y;
    d.z = pt1->z - pt0->z;
    int r;
    int k = *CallC<int*>(FUN_47fc80, &r, p0.road, p1.road, d);
    CallC(FUN_40b100, player, k, d);
}

// [FUN_43e610] setPlayerPosition(PlayerId player, dvector3 const & v, bool limited)
extern "C" void __cdecl Ai_SetPlayerPositionTo(int player, const DVec3* v, char limited)
{
    Trace_EnterBuffered((const char*)s_setPlayerPositionTo66eb70);
    RoomInfo a;
    RoomInfo_Init(&a);
    PositionId p0 = F(PositionId, (BYTE*)player, 40);
    Road_RoomInfo(F(PositionId, (BYTE*)player, 40), &a);
    if (limited)
        CallC(FUN_43cb50, player, v);
    else
        CallC(FUN_43dc60, player, v);
    Player_RoadChanged(player, p0);
    Trace_Leave((const char*)s_setPlayerPositionTo66eb70);
}

// [FUN_43e8c0] setPlayerPosition(RoomId room, PlayerId player, dvector3 const & v, bool limited)
extern "C" void __cdecl Ai_SetPlayerPositionInRoom(int room, int player, const DVec3* v, char limited)
{
    Trace_EnterBuffered((const char*)s_setPlayerPositionRoomLimited);
    RoomInfo a;
    RoomInfo_Init(&a);
    PositionId p0 = F(PositionId, (BYTE*)player, 40);
    Road_RoomInfo(F(PositionId, (BYTE*)player, 40), &a);
    if (limited && F(BYTE*, (BYTE*)player, 24) != F(BYTE*, (BYTE*)player, 28))
        CallC(FUN_43cb50, player, v);
    else
        Ai_SetPlayerPositionRoom(room, player, v);
    Player_RoadChanged(player, p0);
    Trace_Leave((const char*)s_setPlayerPositionRoomLimited);
}

// [FUN_445c90] getRightDirection(PositionId pos): pos with its direction bit (+24 bit 0) set by its room's
// flags 8 (set) or 4 (cleared)
extern "C" PositionId* __cdecl Ai_GetRightDirectionOf(PositionId* out, PositionId pos)
{
    Trace_EnterAppend((const char*)s_getRightDirection66f340);
    AdvanceRoad_Check(&pos, 18145);
    RoomInfo info;
    RoomInfo_Init(&info);
    info.m_8 = 0;
    Road_RoomInfo(pos, &info);
    if (info.flags & 8)
        pos.m_24 |= 1;
    else if (info.flags & 4)
        pos.m_24 &= ~1;
    *out = pos;
    Trace_Leave((const char*)s_getRightDirection66f340);
    return out;
}

// [FUN_427520] startRaces(): once requested (68d168), the human and the waiting racers start: race state 5,
// the start time, and (outside network games) the human's advancement from check_point1
extern "C" int __cdecl Ai_StartRaces()
{
    Trace_EnterBuffered((const char*)s_startRaces);
    if (!g_68d168) {
        Trace_Leave((const char*)s_startRaces);
        return -1;
    }
    int s;
    Player_FromId(&s);
    g_68d168 = 0;
    g_66c6e0 = 1;
    CallC(FUN_4452c0, s);
    CallC(FUN_425790, s);
    for (void** e = g_68cf60; e != g_68cf64; e++) {
        CallC(FUN_4175c0, *e);
        F(double, (BYTE*)*e, 792) = CallC<double>(FUN_401000);
        CallC(FUN_477210, *e);
    }
    BYTE* h = (BYTE*)s;
    F(int, h, 100) = 5;
    F(DWORD, h, 96) |= 0x4000;
    F(int, h, 724) = 0;
    F(double, h, 792) = CallC<double>(FUN_401000);
    CallC(FUN_4272d0, s);
    if (g_68d188 == 0 || g_68d188 == 1) {
        CallC(FUN_4165f0);
        int node;
        CallC(FUN_436510, &node, (const char*)s_checkPoint1);
        int n = node;
        PositionId cp;
        Position_InitNode(&cp, 0, n);
        CallC(FUN_478260, s, cp, 1);
        PositionId here = F(PositionId, (BYTE*)s, 40);
        double len = 0;
        PositionId to;
        Position_InitNode(&to, 0, n);
        CallC(FUN_486800, 0, here, to, &len);
        h = (BYTE*)s;
        F(double, h, 504) = -len - g_raceTimes68cf70[F(int, h, 724)];
    }
    CallC(FUN_477210, s);
    r_68d164 = 0;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = CallC<BYTE*>(FUN_40a9f0, it);
        if (d)
            F(BYTE, CallC<BYTE*>(FUN_40a9f0, it), 864) = 1;
    }
    Trace_Leave((const char*)s_startRaces);
    return 0;
}

// [FUN_4452c0] dropOrders(PlayerId player) with a forfeit: the orders' value is taken off the player's money
// (+136), announced to a human, and the orders dropped
extern "C" int __cdecl Ai_DropOrdersForfeit(int player)
{
    Trace_EnterBuffered((const char*)s_dropOrdersForfeit);
    BYTE* p = (BYTE*)player;
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 17977, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    double sum = 0;
    BYTE* o = 0;
    BYTE* v = Player_Orders(p, (BYTE*)&o);
    o = F(BYTE*, v, 0);
    BYTE* w = 0;
    for (;;) {
        w = Player_Orders(p, w);
        if (o == F(BYTE*, w, 4))
            break;
        sum = CallC<double>(FUN_471420, o) + sum;
        o += 144;
    }
    if (CallC<char>(FUN_417660, player) && (sum < 0 || sum > 0)) {
        char buf[128];
        sprintf(buf, Message_Text((const char*)s_forfeit66f2bc), sum);
        CallC(FUN_56ae60, buf);
        CallC(FUN_483e60, sum);
        CallC(FUN_483e80, 6, 0);
    }
    F(double, p, 136) = F(double, p, 136) - sum;
    CallC(FUN_4778a0, p + 8);
    F(BYTE, p, 200) = 0;
    F(BYTE, p, 201) = 0;
    Ai_AccountForChangeInLoadState(player);
    CallC(FUN_426f20, player);
    Trace_Leave((const char*)s_dropOrdersForfeit);
    return 0;
}

// ---------------------------------------------------------------------------
// Batch: a mafia attack, advancing a player's position, ordering a vehicle

KING_VAR(AsmLabel, s_mafiaAttack, 66f300)
KING_VAR(AsmLabel, s_advancePositionOf, 66e8c0)
KING_VAR(AsmLabel, s_orderVehicle, 66e168)
KING_VAR(double, g_66c788, 66c788)
extern "C" void FUN_42bb10(); KING_ALIAS("_FUN_42bb10", "_AI_LightsOff1")
extern "C" void FUN_4455b0(); KING_ALIAS("_FUN_4455b0", "_Ai_DropOrders")    // __dropOrders (player)
extern "C" void FUN_427090(); KING_ALIAS("_FUN_427090", "_Ai_ClearPlayerFromMafia")    // clearPlayerFromMafia (player)
extern "C" void FUN_414760(); KING_ALIAS("_FUN_414760", "_Vehicle_IndexByKey")    // a vehicle by model (out, model)
extern "C" void FUN_433f60(); KING_ALIAS("_FUN_433f60", "@Rec_Zero10@8")    // a vehicle order's constructor (thiscall)
extern "C" void FUN_433660(); KING_ALIAS("_FUN_433660", "_Ai_PrepareVehicleOrder")    // (player, dealer, model, order*) returns int
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8")    // a vector's storage free (thiscall)
extern "C" void FUN_4466a0(); KING_ALIAS("_FUN_4466a0", "_Ai_RemovePlayer")    // removes a player (player)

// [FUN_445740] mafiaAttack(PlayerId player): carrying orders worth more than its fine (+224), the player
// forfeits them (a forfeit task); otherwise it pays the fine; then the fine is cleared
extern "C" int __cdecl Ai_MafiaAttack(int player)
{
    Trace_EnterBuffered((const char*)s_mafiaAttack);
    BYTE* p = (BYTE*)player;
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 18054, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    CallC(FUN_42bb10);
    double sum = 0;
    double fine = F(double, p, 224);
    BYTE* o = 0;
    o = F(BYTE*, Player_Orders(p, (BYTE*)&o), 0);
    BYTE* w = 0;
    for (;;) {
        w = Player_Orders(p, w);
        if (o == F(BYTE*, w, 4))
            break;
        sum = CallC<double>(FUN_471420, o) + sum;
        o += 144;
    }
    if (sum > fine) {
        CallC(FUN_4455b0, player);
        CallC(FUN_483e80, 21, 0);
        BYTE* t = (BYTE*)operator new(64);
        BYTE* task = t ? Forfeit_Construct(t, 0, player, F(int, &sum, 0), F(int, &sum, 4)) : 0;
        BYTE* it;
        ListB_Insert(&g_68b99c, 0, &it, (BYTE**)*(BYTE**)&g_68b99c, &task);
    } else {
        CallC(FUN_483e60, F(double, p, 224));
        CallC(FUN_483e80, 20, 0);
        F(double, p, 136) = F(double, p, 136) - F(double, p, 224);
    }
    F(DWORD, p, 224) = 0;
    F(DWORD, p, 228) = 0;
    F(DWORD, p, 232) = 0;
    CallC(FUN_477210, player);
    CallC(FUN_426f20, player);
    CallC(FUN_427090, player);
    Trace_Leave((const char*)s_mafiaAttack);
    return 0;
}

// [FUN_43bf10] advancePosition(PlayerId player, PositionId pos, double & offset, bool sendmessage)
extern "C" PositionId* __cdecl Ai_AdvancePlayerPosition(PositionId* out, int player, PositionId pos, double* offset,
                                                        bool sendmessage)
{
    Trace_EnterBuffered((const char*)s_advancePositionOf);
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 15894, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    AdvanceRoad_Check(&pos, 15895);
    DVec3 pt;
    Road_PointOf(&pt, pos);
    PositionId res;
    pos = *Ai_AdvancePosition(&res, player, (BYTE*)player + 8, pos, offset, sendmessage);
    AdvanceRoad_Check(&pos, 15899);
    *out = pos;
    Trace_Leave((const char*)s_advancePositionOf);
    return out;
}

// [FUN_433b60] orderVehicle(PlayerId player, PlayerId dealer, char const * _vehicleModel): returns
// FUN_433660's refusal, or 2 when a kind-6 dealer's roll fails; otherwise the player pays (+136), and a
// new dealer for sale at the order's node takes its market slot
extern "C" int __cdecl Ai_OrderVehicle(int player, int dealer, const char* model)
{
    Trace_EnterBuffered((const char*)s_orderVehicle);
    int veh;
    CallC(FUN_414760, &veh, model);
    BYTE r[80];
    CallThis(FUN_433f60, r);
    F(DWORD, r, 40) = 0;
    F(DWORD, r, 44) = 0;
    F(int, r, 52) = -1;
    F(int, r, 56) = -1;
    int res = CallC<int>(FUN_433660, player, dealer, model, r);
    if (res) {
        CallThis(FUN_404420, r);
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, (const char*)s_orderVehicle);
        return res;
    }
    BYTE* p = (BYTE*)player;
    F(double, p, 136) = F(double, p, 136) - F(double, r, 40);
    CallC(FUN_426f20, player);
    if (F(int, (BYTE*)dealer, 92) == 6) {
        double v = g_66c788;
        unsigned roll = Rng_Random(&g_rng68b9c8, 0, 1000000);
        if (v * g_64b6b0 > (double)roll) {
            CallThis(FUN_404420, r);
            Buf_Append(&g_trace, 0, g_tagMpl, 8);
            Buf_AppendString(&g_trace, 0, (const char*)s_orderVehicle);
            return 2;
        }
    }
    int prev = F(int, F(BYTE*, r, 60), 8);
    if (prev != (int)g_players)
        CallC(FUN_4466a0, prev);
    PositionId pos;
    pos.road = -1;
    pos.node = F(int, r, 52);
    pos.t = 0;
    pos.segment = 0;
    pos.object = 0;
    pos.m_24 = 0;
    pos.m_28 = -1;
    int id;
    Ai_CreateDealerAt(&id, pos, 0, veh);
    BYTE* d = (BYTE*)id;
    F(int, d, 100) = 256;
    g_68ced0[1]++;
    int m = F(int, r, 56);
    int k = F(int, r, 64);
    F(int, d, 120) = m;
    F(int, d, 124) = k;
    BYTE* slot = F(BYTE*, (BYTE*)g_68cc98[m], 28) + k * 36;
    F(BYTE*, slot, 8) = d;
    F(int, (BYTE*)g_68cc98[m], 40)++;
    F(void*, d, 972) = &g_68ced0;
    F(DWORD, d, 96) |= 64;
    slot = F(BYTE*, (BYTE*)g_68cc98[m], 28) + k * 36;
    F(DWORD, slot, 4) |= 1;
    F(int, F(BYTE*, d, 972), 12)++;
    if (F(int, (BYTE*)dealer, 92) == 6)
        F(DWORD, F(BYTE*, d, 516), 40) |= 2;
    if (F(char, r, 68))
        F(DWORD, F(BYTE*, d, 516), 40) |= 1;
    unsigned n = F(BYTE*, r, 8) - F(BYTE*, r, 0);
    if (n)
        Ai_PoolFree(F(BYTE*, r, 0), n);
    Trace_Leave((const char*)s_orderVehicle);
    return 0;
}

KING_VAR(double, g_64b698, 64b698)
KING_VAR(double, g_64b860, 64b860)
extern "C" void FUN_447bc0(); KING_ALIAS("_FUN_447bc0", "_List_Erase@8")    // list erase (thiscall: out, node)
extern "C" void FUN_45faf0(); KING_ALIAS("_FUN_45faf0", "_Find36Flag4")    // (first, last, a, b) returns int*
extern "C" void FUN_4577f0(); KING_ALIAS("_FUN_4577f0", "_DVec3_Length")    // a vector's length (v) returns double
extern "C" void FUN_45fb10(); KING_ALIAS("_FUN_45fb10", "@Vec4b_InsertF@16")    // vector<4 bytes>::insert_aux (thiscall: pos, &x)
extern "C" void FUN_46b0e0(); KING_ALIAS("_FUN_46b0e0", "_UninitCopyDwords")    // uninitialized_copy of 4-byte elements (first, last, dest, -)

// [FUN_42d960] (thiscall on 68b998: flag, list of tasks) when flagged, runs the first task that is due:
// it leaves the list, runs (traced by its name at +32), gets its next time (+16 by its period +24, or from
// now when that is too far behind) and goes back at the end unless it ended (+8 bit 0, then deleted)
extern "C" void __fastcall Tasks_RunDue(BYTE* self, void*)
{
    if (!F(BYTE, self, 0))
        return;
    BYTE* list = self + 4;
    BYTE* head = F(BYTE*, self, 4);
    BYTE* node = *(BYTE**)head;
    BYTE* task;
    for (;; node = *(BYTE**)node) {
        if (node == F(BYTE*, self, 4))
            return;
        task = F(BYTE*, node, 8);
        if (CallC<double>(FUN_401000) > F(double, task, 16))
            break;
    }
    BYTE* next;
    CallThis(FUN_447bc0, list, &next, node);
    const char* name = F(const char*, task, 32);
    Trace_EnterInline(name);
    CallVirt(task, 0);
    Trace_Leave(name);
    F(double, task, 16) = F(double, task, 24) + F(double, task, 16);
    double now = CallC<double>(FUN_401000);
    if (!(F(double, task, 24) * g_64b698 + F(double, task, 16) >= now))
        F(double, task, 16) = CallC<double>(FUN_401000) + F(double, task, 24);
    F(BYTE, self, 0) = 0;
    if (!(F(BYTE, task, 8) & 1)) {
        BYTE* it;
        ListB_Insert(list, 0, &it, (BYTE**)F(BYTE*, self, 4), &task);
    } else
        operator delete(task);
}

// [FUN_4575d0] the positions in [first, last) whose object (+28..+32 list) holds a node farther than 64b860
// from v, as a new vector
extern "C" BYTE** __cdecl Nodes_FarFrom(BYTE** out, BYTE** first, BYTE** last, const DVec3* v)
{
    BYTE* tmp[3] = {0, 0, 0};
    for (BYTE** it = first; it != last; it++) {
        BYTE* e = *it;
        int* r = CallC<int*>(FUN_45faf0, F(BYTE*, e, 28), F(BYTE*, e, 32), first, it);
        if ((BYTE*)r == F(BYTE*, e, 32))
            continue;
        BYTE* n = (BYTE*)g_roadNodes[*r];
        DVec3 d;
        double dz = F(double, n, 64) - v->z;
        double dy = F(double, n, 56) - v->y;
        double dx = F(double, n, 48) - v->x;
        d.x = dx;
        d.y = dy;
        d.z = dz;
        if (CallC<double>(FUN_4577f0, &d) > g_64b860) {
            BYTE** e2 = (BYTE**)tmp[1];
            if (tmp[1] != tmp[2]) {
                if (e2)
                    *e2 = (BYTE*)it;
                tmp[1] += 4;
            } else
                CallThis(FUN_45fb10, tmp, tmp[1], &it);
        }
    }
    unsigned n = (unsigned)(tmp[1] - tmp[0]) >> 2;
    out[0] = out[1] = out[2] = 0;
    BYTE** buf = n ? (BYTE**)Stl_Allocate(n * 4) : 0;
    out[0] = (BYTE*)buf;
    out[1] = (BYTE*)buf;
    out[2] = (BYTE*)(buf + n);
    out[1] = CallC<BYTE*>(FUN_46b0e0, tmp[0], tmp[1], buf, last);
    unsigned cap = (unsigned)(tmp[2] - tmp[0]) >> 2;
    if (cap)
        Ai_PoolFree(tmp[0], cap * 4);
    return out;
}

// ---------------------------------------------------------------------------
// Batch: cheat codes, speeding fines, rescheduling a task, road anchor nodes

KING_VAR(AsmLabel, s_aiCheatCode, 66f328)
KING_VAR(AsmLabel, s_fineForSpeeding, 66dfe8)
KING_VAR(double, g_64b950, 64b950)
KING_VAR(double, g_64b5a0, 64b5a0)
KING_VAR(BYTE*, g_task68d120, 68d120)
extern "C" void FUN_42b360(); KING_ALIAS("_FUN_42b360", "_AI_RunActive")
extern "C" void FUN_41ef90(); KING_ALIAS("_FUN_41ef90", "_Player_CheckTownAA")

// [FUN_445b20] aiCheatCode(int code): 2 runs FUN_42b360, 3 gives the human money and a competitor in 720
extern "C" void __cdecl Ai_CheatCode(int code)
{
    Trace_EnterAppend((const char*)s_aiCheatCode);
    if (code == 2)
        CallC(FUN_42b360);
    else if (code == 3) {
        int s;
        BYTE* h = (BYTE*)*Player_FromId(&s);
        if (h != (BYTE*)g_players) {
            F(double, h, 136) = F(double, h, 136) + g_64b950;
            Ai_AddArrivals((BYTE*)s, 720.0, 1);
        }
    }
    Trace_Leave((const char*)s_aiCheatCode);
}

// [FUN_431170] fineForSpeeding(PlayerId player, double degree): a non-zero fine is announced and paid
extern "C" int __cdecl Ai_FineForSpeeding(int player, double fine)
{
    Trace_EnterAppend((const char*)s_fineForSpeeding);
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 12836, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    if (fine < 0 || fine > 0) {
        CallC(FUN_483e60, fine);
        CallC(FUN_483e80, 16, 0);
        F(double, (BYTE*)player, 136) = F(double, (BYTE*)player, 136) - fine;
    }
    CallC(FUN_425790, player);
    CallC(FUN_41ef90, player);
    CallC(FUN_477210, player);
    CallC(FUN_426f20, player);
    Trace_Leave((const char*)s_fineForSpeeding);
    return 0;
}

// [FUN_401c50] moves the task at 68d120 to the front of the task list, due now unless already overdue,
// and resets the loading state
extern "C" void __cdecl Ai_RescheduleLoadTask()
{
    BYTE* x = g_task68d120;
    BYTE* list = (BYTE*)&g_68b99c;
    BYTE* head = *(BYTE**)list;
    BYTE* found;
    Range_CallA((BYTE*)&found, (DWORD)*(BYTE**)head, (DWORD)head, (int)&x);
    if (found != head) {
        BYTE* next;
        CallThis(FUN_447bc0, list, &next, found);
        BYTE* it;
        ListB_Insert(list, 0, &it, (BYTE**)*(BYTE**)*(BYTE**)list, &x);
        double now = CallC<double>(FUN_401000);
        if (!(now > F(double, x, 16)))
            F(double, x, 16) = now - F(double, x, 24) * g_64b5a0;
    }
    g_loadTick68d13c = 200;
    g_loadRadius68d128 = 0;
    g_loadDone68d130 = 0;
    r_66c60a = 1;
    g_loadNear68d138 = 0;
    Ai_ReUncontradict();
}

// [FUN_405730] the anchor node at a point (24 bytes): found in the list at 68cf58, or appended with a and b
// (+56, +60 of the node)
extern "C" BYTE** __cdecl Anchors_At(BYTE** out, const BYTE* key, int a, int b)
{
    BYTE* head = g_anchors68cf58;
    BYTE* found;
    Range_CallB((BYTE*)&found, (DWORD)*(BYTE**)head, (DWORD)head, (int)key);
    if (found != head) {
        *out = found;
        return out;
    }
    DWORD r[16] = {0};
    memcpy(&r[2], key, 24);
    BYTE* pos = g_anchors68cf58;
    BYTE* it;
    ListC_Insert(&g_anchors68cf58, 0, &it, (BYTE**)pos, r);
    unsigned n = (r[11] - r[9]) / 12;
    if (n)
        Ai_PoolFree((void*)r[9], n * 12);
    BYTE* last = F(BYTE*, g_anchors68cf58, 4);
    F(int, last, 56) = a;
    F(int, last, 60) = b;
    *out = F(BYTE*, g_anchors68cf58, 4);
    return out;
}

KING_VAR(AsmLabel, s_getRoadParameters66e604, 66e604)

static DWORD Str_OrNull(const DWORD* s) { return s[0] != s[1] ? s[0] : 0; }

// [FUN_438d70] getRoadParameters(PositionId pos, LocalRoadParameters & rp): at a node, the node's two names (+188, +184); on a road, the
// object's flags, the position's share of its range (+8), its names and values
extern "C" int __cdecl Road_GetRoadParameters(PositionId pos, RoomInfo* out)
{
    Trace_EnterBuffered((const char*)s_getRoadParameters66e604);
    if (pos.road == -1) {
        if (pos.node != -1) {
            BYTE* n = (BYTE*)g_roadNodes[pos.node];
            out->module = (const char*)Str_OrNull(F(const DWORD*, n, 188));
            out->room = (const char*)Str_OrNull(F(const DWORD*, n, 184));
            out->m_36 = F(int, F(BYTE*, n, 184), 12);
            out->m_40 = F(int, F(BYTE*, n, 188), 12);
        }
    } else {
        PositionId chk = pos;
        if (!PositionValid(&chk)) {
            sprintf(g_positionText77e6e0, "(%d %g %d %d %x)", pos.road, pos.t, pos.segment, pos.object, pos.m_24);
            AiError(33, 100, s_mainFile, s_mainId, 15320, s_invalidId, "PositionId", g_positionText77e6e0);
        }
        BYTE* e = (BYTE*)g_roadEdges[pos.road];
        BYTE* obj = F(BYTE**, e, 120)[pos.object];
        out->flags = F(DWORD, obj, 8);
        const double* range = F(const double*, obj, 40);
        double lo = range[0], hi = range[1];
        double len = CallVirt<double>(obj, 32, 0.0, 0, pos.t, pos.segment);
        double part = len * (hi - lo);
        *(double*)&out->m_8 = part / CallVirt<double>(obj, 36) + lo;
        BYTE* s = F(BYTE*, obj, 16);
        out->module = (const char*)F(DWORD, s, 16);
        out->room = (const char*)Str_OrNull((const DWORD*)s);
        out->m_36 = F(int, s, 12);
        out->m_40 = F(int, F(BYTE*, obj, 20), 12);
        out->m_24 = F(DWORD, obj, 32);
        out->m_28 = F(DWORD, obj, 36);
        out->m_32 = F(DWORD, obj, 28);
        if (F(DWORD, e, 16) & 0x200)
            out->flags &= ~0x20u;
    }
    Trace_Leave((const char*)s_getRoadParameters66e604);
    return 0;
}

// ---------------------------------------------------------------------------
// Batch: loosing a semitrailer, obstacle info

KING_VAR(AsmLabel, s_looseSemitrailer, 66d81c)
KING_VAR(AsmLabel, s_getObstacleInfo, 66f08c)
extern "C" void FUN_4722a0(); KING_ALIAS("_FUN_4722a0", "_Player_Call472110")    // (player)

// [FUN_41fa00] looseSemitrailer(PlayerId player): the player's trailer order (+520) becomes a present at the
// player's point, waiting (68d01c) when it has no orders, otherwise flagged 0x40 (68d0e4)
extern "C" void __cdecl Ai_LooseSemitrailer(int player)
{
    Trace_EnterBuffered((const char*)s_looseSemitrailer);
    BYTE* p = (BYTE*)player;
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 6989, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    BYTE* m = F(BYTE*, p, 520);
    if (m && F(int, m, 4) != -1) {
        if (CallC<char>(FUN_417660, player))
            CallC(FUN_4722a0, player);
        BYTE* o = (BYTE*)operator new(112);
        if (o) {
            F(int, o, 0) = -1;
            F(DWORD, o, 4) = 0x12345678;
            F(DWORD, o, 8) = 0;
            F(int, o, 12) = -1;
            F(int, o, 16) = -1;
            F(DWORD, o, 20) = 0;
            F(int, o, 24) = -1;
            Position_InitNone((PositionId*)(o + 56), 0);
            F(DWORD, o, 96) = 0xffffffff;
            F(DWORD, o, 88) = 0;
            F(DWORD, o, 100) = 0x7fefffff;
            F(BYTE, o, 104) = 0;
            F(int, o, 108) = -1;
        }
        F(int, o, 24) = -1;
        F(BYTE*, o, 20) = F(BYTE*, p, 520);
        F(BYTE*, p, 520) = 0;
        F(DWORD, o, 8) |= 32;
        F(DVec3, o, 32) = F(DVec3, p, 624);
        PositionId tmp;
        F(PositionId, o, 56) = *Ai_GetPositionIdAll(&tmp, (const DVec3*)(o + 32));
        F(BYTE, o, 104) = 1;
        BYTE* v = F(BYTE*, o, 20) + 28;
        if (F(BYTE*, v, 0) == F(BYTE*, v, 4)) {
            F(double, o, 96) = CallC<double>(FUN_401000);
            g_68d01c++;
        } else {
            g_68d0e4++;
            F(DWORD, o, 8) |= 64;
        }
        F(int, o, 0) = F(int, F(BYTE*, o, 20), 0);
        int id = player;
        Presents_Append(&id, (int)o);
        g_presentHook68d11c(id, 1);
        CallC(FUN_425790, player);
        CallC(FUN_41ef90, player);
    }
    Trace_Leave((const char*)s_looseSemitrailer);
}

// [FUN_4420b0] getObstacleInfo(ObstacleId obstacle, ObstacleInfo & si): its point as floats, and for an
// active one whether any road at its two nodes (ends at the first, starts at the second) lacks flag 0x20
extern "C" int __cdecl Ai_GetObstacleInfo(int id, BYTE* out)
{
    Trace_EnterBuffered((const char*)s_getObstacleInfo);
    BYTE* r = (BYTE*)g_68cc18[id];
    F(float, out, 0) = (float)F(double, r, 64);
    F(float, out, 4) = (float)F(double, r, 72);
    F(float, out, 8) = (float)F(double, r, 80);
    F(BYTE, out, 12) = F(BYTE, r, 28);
    if (F(BYTE, r, 28)) {
        F(BYTE, out, 12) = 0;
        for (int k = 48; k < 56; k += 4) {
            int node = F(int, (BYTE*)g_68cc18[id], k);
            if (node == -1) {
                F(BYTE, out, 12) = 1;
                break;
            }
            bool blocked = false;
            for (int j = 0; j < 2 && !blocked; j++) {
                for (BYTE** e = F(BYTE**, g_roadNodes[node], 20 + j * 12);
                     e != F(BYTE**, g_roadNodes[node], 24 + j * 12); e++) {
                    PositionId tmp;
                    PositionId pos = *(j ? Road_EdgeStart(&tmp, (RoadEdge*)*e) : Road_EdgeEnd(&tmp, (RoadEdge*)*e));
                    RoomInfo info;
                    RoomInfo_Init(&info);
                    Road_GetRoadParameters(pos, &info);
                    if (!(info.flags >> 5 & 1)) {
                        blocked = true;
                        break;
                    }
                }
            }
            if (blocked) {
                F(BYTE, out, 12) = 1;
                break;
            }
        }
    }
    Trace_Leave((const char*)s_getObstacleInfo);
    return 0;
}

KING_VAR(AsmLabel, s_noNodes66e384, 66e384)
KING_VAR(AsmLabel, s_noRoads66e36c, 66e36c)
KING_VAR(void*, g_670b04, 670b04)
extern "C" void $L_401310(); KING_ALIAS("_$L_401310", "_Ai_ReportUnless5")
extern "C" void FUN_4a52d0(); KING_ALIAS("_FUN_4a52d0", "_Sch_Exchange69224c")    // registers a function (fn)
extern "C" void FUN_44c160(); KING_ALIAS("_FUN_44c160", "@PodVec_Assign@12")    // a string assigned from a vector<char> (thiscall: &v)
extern "C" void FUN_44bef0(); KING_ALIAS("_FUN_44bef0", "@VecChar_Insert44bef0@16")    // vector<char>::insert_aux (thiscall: pos, &c)
extern "C" void FUN_4031e0(); KING_ALIAS("_FUN_4031e0", "_Ai_ClearWorld")
extern "C" void FUN_406600(); KING_ALIAS("_FUN_406600", "_Ai_LoadRoads")
extern "C" void FUN_403110(); KING_ALIAS("_FUN_403110", "_AI_Renumber")
extern "C" void FUN_402be0(); KING_ALIAS("_FUN_402be0", "_Ai_BuildNameMaps")
extern "C" void FUN_406340(); KING_ALIAS("_FUN_406340", "_Road_NameEdges")
extern "C" void FUN_4842a0(); KING_ALIAS("_FUN_4842a0", "_Road_ResetCosts")
extern "C" void FUN_406370(); KING_ALIAS("_FUN_406370", "_Ai_CollectRoomEdges")
extern "C" void FUN_40d1b0(); KING_ALIAS("_FUN_40d1b0", "_Ai_LoadNames")
extern "C" void FUN_40c3b0(); KING_ALIAS("_FUN_40c3b0", "_Ai_ReadMerchandise")
extern "C" void FUN_40cef0(); KING_ALIAS("_FUN_40cef0", "_Ai_ReadRestable")
extern "C" void FUN_42e9f0(); KING_ALIAS("_FUN_42e9f0", "_Ai_LoadVehicles")
extern "C" void FUN_42f4c0(); KING_ALIAS("_FUN_42f4c0", "_Ai_LoadTrashboxes")
extern "C" void FUN_430230(); KING_ALIAS("_FUN_430230", "_Vehicles_ResolveModels")
extern "C" void FUN_430480(); KING_ALIAS("_FUN_430480", "_Vehicles_AverageRatio")

static void CharVec_Zeroed(VecMethods* v, unsigned n)
{
    VecReserve(v, n);
    memset(v->begin, 0, n);
    v->end = v->begin + n;
}

// [FUN_435730] the AI's start: the hooks, the full path of its directory (ending in '\') at 68bac8, the
// clock, then loading the world, its roads and the rest
extern "C" int __cdecl Ai_Initialize(const char* dir, AiEventHook eventHook, PresentHook presentHook)
{
    g_670b04 = CRT_STREAM(1);                     // stdout
    CallC(FUN_4a52d0, $L_401310);
    g_presentHook68d11c = presentHook;
    g_aiEventHook68d118 = eventHook;
    VecMethods v;
    CharVec_Zeroed(&v, 8192);
    DWORD n = GetFullPathNameA(dir, v.end - v.begin, (char*)v.begin, 0);
    if (n >= (DWORD)(v.end - v.begin)) {
        VecMethods w;
        CharVec_Zeroed(&w, n + 2);
        if (v.capacityEnd != v.begin)
            Ai_PoolFree(v.begin, v.capacityEnd - v.begin);
        v = w;
        GetFullPathNameA(dir, v.end - v.begin, (char*)v.begin, 0);
    }
    v.end = v.begin + strlen((char*)v.begin);
    if (v.end != v.begin && v.end[-1] != '\\') {
        char c = '\\';
        if (v.end != v.capacityEnd) {
            *v.end = c;
            v.end++;
        } else
            CallThis(FUN_44bef0, &v, v.end, &c);
    }
    CallThis(FUN_44c160, &g_init68bac8, &v);
    g_lastSpeedTime68d0f0 = CallC<double>(FUN_401000);
    CallC(FUN_4031e0);
    CallC(FUN_406600);
    CallC(FUN_403110);
    CallC(FUN_402be0);
    CallC(FUN_406340);
    if (g_roadNodes == g_roadNodesEnd)
        AiError(0, 100, s_mainFile, s_mainId, 14311, s_noNodes66e384, &g_aiEmpty, &g_aiEmpty);
    if (g_roadEdges == g_roadEdgesEnd)
        AiError(0, 100, s_mainFile, s_mainId, 14315, s_noRoads66e36c, &g_aiEmpty, &g_aiEmpty);
    CallC(FUN_4842a0);
    CallC(FUN_406370);
    CallC(FUN_40d1b0);
    CallC(FUN_40c3b0);
    CallC(FUN_40cef0);
    CallC(FUN_42e9f0);
    CallC(FUN_42f4c0);
    CallC(FUN_430230);
    CallC(FUN_430480);
    if (v.capacityEnd != v.begin)
        Ai_PoolFree(v.begin, v.capacityEnd - v.begin);
    return 0;
}

// ---------------------------------------------------------------------------
// Batch: picking up a present

KING_VAR(AsmLabel, s_pickPresent, 66d794)
KING_VAR(AsmLabel, s_presentId66d64c, 66d64c)
KING_VAR(AsmLabel, s_pickParked66d770, 66d770)
extern "C" void FUN_456cb0(); KING_ALIAS("_FUN_456cb0", "_Rec_NotValid")    // whether a present id is invalid (present)
extern "C" void FUN_41f3d0(); KING_ALIAS("_FUN_41f3d0", "@Rec41f3d0_Destroy@8")    // an order's destructor (thiscall)
extern "C" void FUN_472270(); KING_ALIAS("_FUN_472270", "_Player_Call471ec0")    // (player)
extern "C" void FUN_41ef10(); KING_ALIAS("_FUN_41ef10", "_Present_Destroy")    // destroys a present (present)

// [FUN_41f0d0] _pickPresent(PlayerId player, PresentId present): a non-race present ends the human's race;
// one holding an order (flag 0x20) replaces the player's trailer order (+520) and starts its first order's
// timer; then its spot (+108) is freed and the present destroyed
extern "C" void __cdecl Ai_PickPresent(int player, int present)
{
    Trace_EnterBuffered((const char*)s_pickPresent);
    BYTE* p = (BYTE*)player;
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 6853, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    if (CallC<int>(FUN_456cb0, present)) {
        sprintf(g_text77e5e0, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 6854, s_invalidId, s_presentId66d64c, g_text77e5e0);
    }
    CallC(FUN_417660, player);
    BYTE* rec = F(BYTE*, (BYTE*)present, 8);
    if ((F(DWORD, rec, 8) & 0xf0) != 16)
        Ai_HumanFinished();
    DWORD fl = F(DWORD, F(BYTE*, (BYTE*)present, 8), 8);
    if (fl & 128) {
        AiError(0, 1, s_mainFile, s_mainId, 6870, s_pickParked66d770, &g_aiEmpty, &g_aiEmpty);
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, (const char*)s_pickPresent);
        return;
    }
    rec = F(BYTE*, (BYTE*)present, 8);
    if ((fl & 32) && F(BYTE*, rec, 20)) {
        BYTE* old = F(BYTE*, p, 520);
        if (old) {
            CallThis(FUN_41f3d0, old);
            operator delete(old);
        }
        BYTE* o = F(BYTE*, F(BYTE*, (BYTE*)present, 8), 20);
        F(BYTE*, p, 520) = o;
        F(int, o, 0) = F(int, F(BYTE*, (BYTE*)present, 8), 0);
        F(BYTE*, F(BYTE*, (BYTE*)present, 8), 20) = 0;
        if (!F(BYTE, F(BYTE*, (BYTE*)present, 8), 104)) {
            BYTE* m = F(BYTE*, p, 520);
            BYTE* first = F(BYTE*, m, 28);
            if (first != F(BYTE*, m, 32))
                F(double, first, 72) = CallC<double>(FUN_401000) + F(double, first, 88);
        }
        if (CallC<char>(FUN_417660, player))
            CallC(FUN_472270, player);
    }
    int spot = F(int, F(BYTE*, (BYTE*)present, 8), 108);
    if (spot != -1)
        F(int, (BYTE*)g_spots68d0b8[spot], 40)--;
    CallC(FUN_41ef10, present);
    CallC(FUN_425790, player);
    CallC(FUN_41ef90, player);
    CallC(FUN_56f6d0);
    Trace_Leave((const char*)s_pickPresent);
}

KING_VAR(AsmLabel, s_pickPresentTask, 66d7c8)

// [FUN_41f5e0] pickPresent(PlayerId player, PresentId present): a non-race present ends the human's race;
// unless parked, a pick task for it goes to the front of the task list
extern "C" void __cdecl Ai_PickPresentLater(int player, int present)
{
    Trace_EnterBuffered((const char*)s_pickPresentTask);
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 6935, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    if (CallC<int>(FUN_456cb0, present)) {
        sprintf(g_text77e5e0, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 6936, s_invalidId, s_presentId66d64c, g_text77e5e0);
    }
    CallC(FUN_417660, player);
    if ((F(DWORD, F(BYTE*, (BYTE*)present, 8), 8) & 0xf0) != 16)
        Ai_HumanFinished();
    if (F(BYTE, F(BYTE*, (BYTE*)present, 8), 8) & 128)
        AiError(0, 1, s_mainFile, s_mainId, 6952, s_pickParked66d770, &g_aiEmpty, &g_aiEmpty);
    else {
        BYTE* t = (BYTE*)operator new(56);
        BYTE* task = t ? Pick_Construct(t, 0, player, present) : 0;
        BYTE* list = (BYTE*)&g_68b99c;
        BYTE* it;
        ListB_Insert(list, 0, &it, (BYTE**)*(BYTE**)*(BYTE**)list, &task);
    }
    Trace_Leave((const char*)s_pickPresentTask);
}

KING_VAR(AsmLabel, s_cancelIllegalOrders, 66f294)
KING_VAR(AsmLabel, s_loadSeized66f288, 66f288)
extern "C" void FUN_457b80(); KING_ALIAS("_FUN_457b80", "_Order144_Copy")    // copy of orders (first, last, dest)
extern "C" void FUN_461b00(); KING_ALIAS("_FUN_461b00", "@Rec41ce50_DeletingDestructor@12")    // an order's destructor (thiscall: flags)

// [FUN_444f80] cancelIllegalOrders(PlayerId player): drops each order whose +4 is set (not -1), telling a
// human LOAD_SEIZED each time
extern "C" int __cdecl Ai_CancelIllegalOrders(int player)
{
    Trace_EnterBuffered((const char*)s_cancelIllegalOrders);
    BYTE* p = (BYTE*)player;
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 17948, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    BYTE* fa = p;
    BYTE* fb = 0;
    BYTE* fc = 0;
    BYTE* fd = p;
    for (;;) {
        fa = Player_Orders(p, fa);
        BYTE* end = F(BYTE*, fa, 4);
        fb = Player_Orders(p, fb);
        BYTE* it = F(BYTE*, fb, 0);
        while (it != end && F(int, it, 4) == -1)
            it += 144;
        fc = Player_Orders(p, fc);
        if (it == F(BYTE*, fc, 4))
            break;
        fd = Player_Orders(p, fd);
        if (it + 144 != F(BYTE*, fd, 4))
            CallC(FUN_457b80, it + 144, F(BYTE*, fd, 4), it);
        F(BYTE*, fd, 4) -= 144;
        CallThis(FUN_461b00, F(BYTE*, fd, 4), 0);
        if (CallC<char>(FUN_417660, player))
            CallC(FUN_56ae60, Message_Text((const char*)s_loadSeized66f288));
    }
    F(BYTE, p, 200) = 0;
    CallC(FUN_425790, player);
    CallC(FUN_41ef90, player);
    CallC(FUN_477210, player);
    CallC(FUN_56f6d0);
    Trace_Tag((const DWORD*)g_tagMpl);
    Buf_AppendString(&g_trace, 0, (const char*)s_cancelIllegalOrders);
    return 0;
}

// ---------------------------------------------------------------------------
// Batch: passing a node

KING_VAR(AsmLabel, s_onNodePass, 66ccd0)
KING_VAR(AsmLabel, s_finishAll66ccc0, 66ccc0)
KING_VAR(AsmLabel, s_fline166ccb8, 66ccb8)
KING_VAR(AsmLabel, s_check166ccb0, 66ccb0)
extern "C" void FUN_40ae80(); KING_ALIAS("_FUN_40ae80", "_Ai_EndHelpCalls")    // (player)
extern "C" void FUN_40a690(); KING_ALIAS("_FUN_40a690", "_Ai_CallHelpers")    // calling the closest helpers (player)
extern "C" void FUN_40aa00(); KING_ALIAS("_FUN_40aa00", "_Ai_CallMafia")    // calling the closest mafia (player)
extern "C" void FUN_40a180(); KING_ALIAS("_FUN_40a180", "_Players_Each409fc0")
extern "C" void FUN_40ad00(); KING_ALIAS("_FUN_40ad00", "_Ai_FinishRacingPlayer")    // finishRacingPlayer (player)

// [FUN_40b100] onNodePass(PlayerId player, NodeId node, dvector3 direction): a node's sign (+192) and, for
// the human, help or mafia calls and the sign's message when facing it; a racer at its finish node
// (finish%d_all, or fline1 in the network race) finishes, check1 restarts its legs
extern "C" void __cdecl Ai_OnNodePass(int player, int node, DVec3 dir)
{
    Trace_EnterBuffered((const char*)s_onNodePass);
    if (node == -1) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, (const char*)s_onNodePass);
        return;
    }
    BYTE* p = (BYTE*)player;
    BYTE* s = F(BYTE*, g_roadNodes[node], 192);
    if (s && (F(BYTE, s, 0) & 16))
        CallC(FUN_40ae80, player);
    if (F(int, p, 92) == 1) {
        if (!g_68d188) {
            if (F(double, p, 208) > g_zero64b678 || F(BYTE, p, 200) || F(double, p, 240) > g_zero64b678
                || F(double, p, 248) > g_zero64b678 || F(double, p, 256) > g_zero64b678 || F(BYTE, p, 264))
                CallC(FUN_40a690, player);
            if (F(double, p, 224) > g_zero64b678 || F(BYTE, p, 201))
                CallC(FUN_40aa00, player);
        }
        if (s && !(F(BYTE, s, 0) & 16)) {
            double dot = F(double, s, 24) * dir.z + F(double, s, 16) * dir.y + F(double, s, 8) * dir.x;
            if (dot > g_zero64b678) {
                double dt = F(double, s, 32) - CallC<double>(FUN_401000);
                if (dt < 0)
                    dt = -dt;
                if (dt > g_64b698) {
                    CallC(FUN_56ae60, F(const char*, s, 40));
                    F(double, s, 32) = CallC<double>(FUN_401000);
                }
            }
        }
        if (!g_68d188)
            CallC(FUN_40a180);
    }
    const DWORD* name = (const DWORD*)g_roadNodes[node];
    if ((g_68d188 == 0 || g_68d188 == 1) && F(int, p, 100) == 5 && F(int, p, 92) != 1 && F(int, p, 724) >= 9) {
        char buf[32];
        sprintf(buf, (const char*)s_finishAll66ccc0, g_66c6e4);
        if (!strcmp(buf, (const char*)Str_OrNull((const DWORD*)g_roadNodes[node])))
            CallC(FUN_40ad00, player);
    }
    if (g_68d188 == 2 && F(int, p, 100) == 5) {
        if (!strncmp((const char*)s_fline166ccb8, (const char*)Str_OrNull(name), 6)) {
            if (F(int, p, 92) != 1)
                CallC(FUN_40ad00, player);
        } else if (!strncmp((const char*)s_check166ccb0, (const char*)Str_OrNull((const DWORD*)g_roadNodes[node]), 6)
                   && F(int, p, 92) != 1)
            F(int, p, 724) = 1;
    }
    Trace_Leave((const char*)s_onNodePass);
}

KING_VAR(AsmLabel, s_finishPlayer, 66d9e0)
KING_VAR(AsmLabel, s_youMade66d9d4, 66d9d4)
KING_VAR(double, g_64b6a0, 64b6a0)

// [FUN_426b00] finishPlayer(PlayerId player): its race time; outside network games, a finish with
// competitors left counts (+536) and brings more of them and a prize of 5000; then event 11 with the
// record, the race state ends and the waiting timer restarts
extern "C" int __cdecl Ai_FinishPlayer(int player)
{
    Trace_EnterBuffered((const char*)s_finishPlayer);
    BYTE* p = (BYTE*)player;
    if (CallC<char>(FUN_417660, player))
        Messages_AddNumbered(g_68d164);
    DWORD rec[16] = {0};
    F(double, rec, 24) = 0;
    F(double, rec, 32) = 0;
    F(double, rec, 40) = 1.0;
    F(double, rec, 48) = 0;
    F(double, rec, 56) = 0;
    unsigned r = AiRandom() % 360 + 360;
    F(double, rec, 16) = (double)r;
    F(double, p, 808) = CallC<double>(FUN_401000) - F(double, p, 792);
    F(double, rec, 24) = F(double, p, 808);
    if (!g_68d188) {
        if (g_68d164 == 0 && g_68a980 != g_68a984) {
            F(int, p, 536)++;
            int k = F(int, p, 536);
            rec[1] = k;
            rec[0] |= k <= 1 ? 1 : 3;
            int v = Ftol(x87_sqrt((double)(k + 1)));
            if (v > 3)
                v = 3;
            rec[2] = v;
            Ai_AddArrivals(p, F(double, rec, 16), v);
            F(double, rec, 32) = 5000.0;
            F(double, rec, 48) = 5000.0;
        } else {
            F(int, p, 536) = 0;
            F(double, rec, 32) = 0;
        }
        F(double, p, 136) = F(double, rec, 32) + F(double, p, 136);
        if (CallC<char>(FUN_417660, player) && (F(double, rec, 32) < 0 || F(double, rec, 32) > 0)) {
            int cents = Ftol(F(double, rec, 32) * g_64b6a0);
            char buf[128];
            sprintf(buf, Message_Text((const char*)s_youMade66d9d4), (double)cents * g_64b5a0);
            CallC(FUN_56ae60, buf);
        }
        Ai_PlayerEvent(p, 11, (int)rec);
    }
    if (g_68d188 == 1 || g_68d188 == 2)
        Ai_PlayerEvent(p, 11, (int)rec);
    if (F(int, p, 100) == 5)
        CallC(FUN_417680, player);
    if (CallC<char>(FUN_417660, player)) {
        Players_ResetMarks();
        F(int, p, 524) = 0;
        F(int, p, 528) = 0;
        F(int, p, 532) = -1;
    }
    Ai_RestartWaitTimer();
    Trace_Leave((const char*)s_finishPlayer);
    return 0;
}

// ---------------------------------------------------------------------------
// Batch: records far from the camera

KING_VAR(AsmLabel, s_err3019, 66f494)
extern "C" void FUN_4561a0(); KING_ALIAS("_FUN_4561a0", "@Vec4b_InsertN@16")    // vector<4 bytes>::insert_aux (thiscall: pos, &x)

// a vector<4 bytes> copied to an exact-size out, the source's storage freed (the tail of 4575d0/456460)
static BYTE** PtrVec_CopyOut(BYTE** out, BYTE** tmp, void* tag)
{
    unsigned n = (unsigned)(tmp[1] - tmp[0]) >> 2;
    out[0] = out[1] = out[2] = 0;
    BYTE* buf = n ? (BYTE*)Stl_Allocate(n * 4) : 0;
    out[0] = buf;
    out[1] = buf;
    out[2] = buf + n * 4;
    out[1] = CallC<BYTE*>(FUN_46b0e0, tmp[0], tmp[1], buf, tag);
    unsigned cap = (unsigned)(tmp[2] - tmp[0]) >> 2;
    if (cap)
        Ai_PoolFree(tmp[0], cap * 4);
    return out;
}

// [FUN_456460] the free records (+8 no player) of [first, last) (36 bytes each, a node first) at least maxDist
// from the camera, by mode: 0 without flags 0x2c, 1 with 0x20, 2 with 8
extern "C" BYTE** __cdecl Recs_FarFromCamera(BYTE** out, BYTE* first, BYTE* last, int mode, int, double maxDist)
{
    BYTE* tmp[3] = {0, 0, 0};
    BYTE* it;
    for (it = first; it != last; it += 36) {
        BYTE* n = (BYTE*)g_roadNodes[F(int, it, 0)];
        DVec3 d;
        d.z = F(double, n, 64) - g_68bab0.z;
        d.y = F(double, n, 56) - g_68bab0.y;
        d.x = F(double, n, 48) - g_68bab0.x;
        double len = x87_sqrt((d.y * d.y + d.z * d.z) + d.x * d.x);
        if (!(len >= maxDist))
            continue;
        bool take;
        if (mode == 0)
            take = !(F(BYTE, it, 4) & 44) && F(void*, it, 8) == g_players;
        else if (mode == 1)
            take = (F(BYTE, it, 4) & 32) && F(void*, it, 8) == g_players;
        else if (mode == 2)
            take = (F(BYTE, it, 4) & 8) && F(void*, it, 8) == g_players;
        else {
            AiError(0, 1, s_mainFile, s_mainId, 4661, s_err3019, &g_aiEmpty, &g_aiEmpty);
            take = false;
        }
        if (!take)
            continue;
        if (tmp[1] != tmp[2]) {
            if (tmp[1])
                *(BYTE**)tmp[1] = it;
            tmp[1] += 4;
        } else
            CallThis(FUN_4561a0, tmp, tmp[1], &it);
    }
    return PtrVec_CopyOut(out, tmp, it);
}

KING_VAR(double, g_64b6d8, 64b6d8)
KING_VAR(double, g_64b738, 64b738)
extern "C" void FUN_4611c0(); KING_ALIAS("_FUN_4611c0", "_Construct4")    // construct 4 bytes in place (dest, src)
extern "C" void FUN_4608f0(); KING_ALIAS("_FUN_4608f0", "_Construct8")    // construct 8 bytes in place (dest, src)
extern "C" void FUN_45d630(); KING_ALIAS("_FUN_45d630", "@Vec4b_Insert45d630@16")    // vector<4 bytes>::insert_aux (thiscall: pos, &x)
extern "C" void FUN_45aed0(); KING_ALIAS("_FUN_45aed0", "@Vec4b_InsertA@16")    // vector<4 bytes>::insert_aux (thiscall: pos, &x)
extern "C" void FUN_44c470(); KING_ALIAS("_FUN_44c470", "@Vec8_Insert44c470@16")    // vector<double>::insert_aux (thiscall: pos, &x)
extern "C" void FUN_4473e0(); KING_ALIAS("_FUN_4473e0", "@Vec8_FreeStorage@8")    // vector<double> destructor (thiscall)
extern "C" void FUN_447970(); KING_ALIAS("_FUN_447970", "@Vec4_FreeStorage@8")    // vector<4 bytes> destructor (thiscall)
extern "C" void FUN_44a530(); KING_ALIAS("_FUN_44a530", "@Vec8_Free@8")    // vector<double> destructor (thiscall)
extern "C" void FUN_44b0d0(); KING_ALIAS("_FUN_44b0d0", "@Vec4_Free@8")    // vector<4 bytes> destructor (thiscall)
extern "C" void FUN_404cf0(); KING_ALIAS("_FUN_404cf0", "_Town_Trades")    // (town, int) returns bool

// a candidate (an iterator) and its weight added to the two lists
static void Weighted_Add(BYTE** c, BYTE** w, BYTE* it, double x, void (*insertC)())
{
    if (c[1] != c[2]) {
        CallC(FUN_4611c0, c[1], &it);
        c[1] += 4;
    } else
        CallThis(insertC, c, c[1], &it);
    if (w[1] != w[2]) {
        CallC(FUN_4608f0, w[1], &x);
        w[1] += 8;
    } else
        CallThis(FUN_44c470, w, w[1], &x);
}

// one candidate picked by its weight with the RNG at rng (the only one without a roll); last when none
static BYTE* Weighted_Pick(BYTE** c, BYTE** w, double sum, void* rng, BYTE* last)
{
    BYTE* res;
    if (c[0] == c[1])
        res = last;
    else if (((c[1] - c[0]) & ~3) == 4)
        res = *(BYTE**)c[0];
    else {
        unsigned r = Rng_Random(rng, 0, 1000000);
        double target = (double)r * sum * g_64b6d8;
        double acc = 0;
        BYTE** p = (BYTE**)c[0];
        const double* q = (const double*)w[0];
        for (;; p++, q++) {
            if (p == (BYTE**)c[1]) {
                CallThis(FUN_4473e0, w);
                CallThis(FUN_447970, c);
                return last;
            }
            if (target >= acc && !(target > acc + *q)) {
                res = *p;
                break;
            }
            acc = acc + *q;
        }
    }
    CallThis(FUN_44a530, w);
    CallThis(FUN_44b0d0, c);
    return res;
}

// [FUN_456a10] a random one of the goods ids in [first, last) by weight: not flag 1, (+48 & mask) == value,
// less likely for each of obj's entries (+28..+32, 16 bytes) holding it
extern "C" BYTE* __cdecl Goods_PickWeighted(BYTE* first, BYTE* last, BYTE* obj, DWORD value, DWORD mask, void* rng)
{
    BYTE* c[3] = {0, 0, 0};
    BYTE* w[3] = {0, 0, 0};
    double sum = 0;
    for (BYTE* it = first; it != last; it += 4) {
        int id = *(int*)it;
        DWORD fl = F(DWORD, g_68ccb8 + id * 104, 48);
        double x;
        if (fl & 1)
            x = g_zero64b678;
        else if ((fl & mask) != value)
            x = g_zero64b678;
        else {
            x = g_64b670;
            for (BYTE* e = F(BYTE*, obj, 28); e != F(BYTE*, obj, 32); e += 16)
                for (int j = 0; j < 2; j++) {
                    int* q = F(int*, e, 8 + j * 4);
                    if (q && *q == id)
                        x = x * g_64b738;
                }
        }
        if (x > g_zero64b678) {
            sum = x + sum;
            Weighted_Add(c, w, it, x, FUN_45d630);
        }
    }
    return Weighted_Pick(c, w, sum, rng, last);
}

// [FUN_454230] a random one of the towns in [first, last) by weight: not the town itself, FUN_404cf0 with
// arg, less likely for each of the town's entries (+28..+32) leading to it
extern "C" BYTE* __cdecl Towns_PickWeighted(BYTE* first, BYTE* last, BYTE* town, int arg, void* rng)
{
    BYTE* c[3] = {0, 0, 0};
    BYTE* w[3] = {0, 0, 0};
    double sum = 0;
    for (BYTE* it = first; it != last; it += 4) {
        BYTE* t = *(BYTE**)it;
        double x;
        if (t == town)
            x = g_zero64b678;
        else if (!CallC<char>(FUN_404cf0, t, arg))
            x = g_zero64b678;
        else {
            x = g_64b670;
            for (BYTE* e = F(BYTE*, town, 28); e != F(BYTE*, town, 32); e += 16)
                for (int j = 0; j < 2; j++) {
                    BYTE* q = F(BYTE*, e, 8 + j * 4);
                    if (q && (BYTE*)g_towns[F(int, q, 12)] == t)
                        x = x * g_round64b658;
                }
        }
        if (x > g_zero64b678) {
            sum = x + sum;
            Weighted_Add(c, w, it, x, FUN_45aed0);
        }
    }
    return Weighted_Pick(c, w, sum, rng, last);
}

// ---------------------------------------------------------------------------
// Batch: ending a player's calls for help

KING_VAR(double, g_64b690, 64b690)
extern "C" void FUN_477cb0(); KING_ALIAS("_FUN_477cb0", "_Player_Call477c60")    // (player, double)

// [FUN_40ae80] outside network games, clears a player's help requests (+208..+260) and, when none is left,
// frees the helpers (kind 5) bound to it (+960), sending the ones near the camera away (FUN_477cb0 with 120);
// then event 18
extern "C" void __cdecl Ai_EndHelpCalls(int player)
{
    if (g_68d188)
        return;
    BYTE* p = (BYTE*)player;
    F(DWORD, p, 248) = 0;
    F(DWORD, p, 252) = 0;
    F(DWORD, p, 208) = 0;
    F(DWORD, p, 212) = 0;
    F(DWORD, p, 216) = 0;
    F(DWORD, p, 256) = 0;
    F(DWORD, p, 260) = 0;
    F(DWORD, p, 240) = 0;
    F(DWORD, p, 244) = 0;
    CallC(FUN_477210, player);
    p = (BYTE*)player;
    if (!(F(double, p, 208) > g_zero64b678) && !F(BYTE, p, 200) && !(F(double, p, 240) > g_zero64b678)
        && !(F(double, p, 248) > g_zero64b678) && !(F(double, p, 256) > g_zero64b678) && !F(BYTE, p, 264)) {
        for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
            BYTE* d = CallC<BYTE*>(FUN_40a9f0, it);
            if (!d || F(int, CallC<BYTE*>(FUN_40a9f0, it), 84) != 5
                || F(int, CallC<BYTE*>(FUN_40a9f0, it), 960) != player)
                continue;
            F(void*, CallC<BYTE*>(FUN_40a9f0, it), 960) = g_players;
            DVec3 tmp;
            const DVec3* pt = CallC<const DVec3*>(FUN_40b0d0, &tmp, it);
            double x = pt->x - g_68bab0.x;
            double y = pt->y - g_68bab0.y;
            double z = pt->z - g_68bab0.z;
            if (x87_sqrt((x * x + y * y) + z * z) < g_64b690)
                CallC(FUN_477cb0, it, 120.0);
        }
    }
    Ai_PlayerEvent((BYTE*)player, 18, 0);
}

KING_VAR(AsmLabel, s_getOrientationSmoothedF, 66e73c)
extern "C" void FUN_41cd40(); KING_ALIAS("_FUN_41cd40", "@Rec48_Construct@56")    // a float matrix from 12 floats (thiscall)

// [FUN_43a4a0] getOrientationSmoothed(PlayerId player, PositionId pos) as a float matrix: the three rows and
// the point of the smoothed orientation
extern "C" BYTE* __cdecl Ai_GetOrientationSmoothedF(BYTE* out, int player, PositionId pos)
{
    Trace_EnterInline((const char*)s_getOrientationSmoothedF);
    BYTE m[96];
    BYTE* r = Ai_GetOrientationSmoothed(m, (BYTE*)player, pos);
    DVec3 t1 = F(DVec3, r, 72), t2 = t1, t3 = t1;
    DVec3 z1 = F(DVec3, r, 48), z2 = z1;
    DVec3 b[7];
    const DVec3* row2 = (const DVec3*)Rec_Get24(r, 0, &b[0], 2);
    const double* r1z = &((const DVec3*)Rec_Get24(r, 0, &b[1], 1))->z;
    const double* r1y = &((const DVec3*)Rec_Get24(r, 0, &b[2], 1))->y;
    const DVec3* row1 = (const DVec3*)Rec_Get24(r, 0, &b[3], 1);
    const double* r0z = &((const DVec3*)Rec_Get24(r, 0, &b[4], 0))->z;
    const double* r0y = &((const DVec3*)Rec_Get24(r, 0, &b[5], 0))->y;
    const DVec3* row0 = (const DVec3*)Rec_Get24(r, 0, &b[6], 0);
    CallThis(FUN_41cd40, out, (float)row0->x, (float)*r0y, (float)*r0z, (float)row1->x, (float)*r1y, (float)*r1z,
             (float)row2->x, (float)z2.y, (float)z1.z, (float)t3.x, (float)t2.y, (float)t1.z);
    Trace_Leave((const char*)s_getOrientationSmoothedF);
    return out;
}

// ---------------------------------------------------------------------------
// Batch: orientations as float matrices

KING_VAR(AsmLabel, s_getOrientationF, 66e774)

// a 96-byte orientation (three rows and a point) as the float matrix at out (FUN_41cd40)
static void Orientation_ToFloats(BYTE* out, BYTE* r)
{
    DVec3 b[3];
    const DVec3* row2 = (const DVec3*)Rec_Get24(r, 0, &b[0], 2);
    const DVec3* row1 = (const DVec3*)Rec_Get24(r, 0, &b[1], 1);
    const DVec3* row0 = (const DVec3*)Rec_Get24(r, 0, &b[2], 0);
    const DVec3& t = F(DVec3, r, 72);
    CallThis(FUN_41cd40, out, (float)row0->x, (float)row0->y, (float)row0->z, (float)row1->x, (float)row1->y,
             (float)row1->z, (float)row2->x, (float)row2->y, (float)F(DVec3, r, 48).z, (float)t.x, (float)t.y,
             (float)t.z);
}

// [FUN_43a780] getOrientation(PositionId pos) as a float matrix
extern "C" BYTE* __cdecl Ai_GetOrientationF(BYTE* out, PositionId pos)
{
    Trace_EnterAppend((const char*)s_getOrientationF);
    AdvanceRoad_Check(&pos, 15566);
    BYTE m[96];
    Orientation_ToFloats(out, Ai_GetOrientation(m, pos));
    Trace_Tag((const DWORD*)g_tagMpl);
    Buf_AppendString(&g_trace, 0, (const char*)s_getOrientationF);
    return out;
}

KING_VAR(AsmLabel, s_err08321, 66d6bc)
KING_VAR(AsmLabel, s_err08322, 66d6a4)
extern "C" void FUN_41d310(); KING_ALIAS("_FUN_41d310", "@Spot_Weight4@12")    // a spot's weight (spot) returns double

// [FUN_41cff0] a present at a node (once: the node's flag 0x10), placed at a spot picked by weight
// (+108 its index, the spot's count +40), then announced
extern "C" void __cdecl Ai_PresentAtNode(int node)
{
    if (F(BYTE, g_roadNodes[node], 12) & 16) {
        AiError(0, 1, s_mainFile, s_mainId, 6512, s_err08321, &g_aiEmpty, &g_aiEmpty);
        return;
    }
    BYTE* o = (BYTE*)operator new(112);
    if (o) {
        F(int, o, 0) = -1;
        F(DWORD, o, 4) = 0x12345678;
        F(DWORD, o, 8) = 0;
        F(int, o, 12) = -1;
        F(int, o, 16) = -1;
        F(DWORD, o, 20) = 0;
        F(int, o, 24) = -1;
        F(int, o, 56) = -1;
        F(int, o, 60) = -1;
        for (int k = 64; k <= 80; k += 4)
            F(DWORD, o, k) = 0;
        F(int, o, 84) = -1;
        F(DWORD, o, 96) = 0xffffffff;
        F(DWORD, o, 88) = 0;
        F(DWORD, o, 100) = 0x7fefffff;
        F(BYTE, o, 104) = 0;
        F(int, o, 108) = -1;
    }
    F(int, o, 24) = node;
    F(DWORD, o, 8) |= 1152;
    F(DVec3, o, 32) = F(DVec3, g_roadNodes[node], 48);
    PositionId tmp;
    F(PositionId, o, 56) = *Ai_GetPositionIdAll(&tmp, (const DVec3*)(o + 32));
    F(DWORD, g_roadNodes[node], 12) |= 16;
    g_68cf34++;
    void** first = g_spots68d0b8;
    void** last = g_spots68d0bc;
    BYTE* c[3] = {0, 0, 0};
    BYTE* w[3] = {0, 0, 0};
    double sum = 0;
    for (void** it = first; it != last; it++) {
        BYTE dummy[4];
        double x = CallThis<double>(FUN_41d310, dummy, *it);
        if (x > g_zero64b678) {
            sum = x + sum;
            Weighted_Add(c, w, (BYTE*)it, x, FUN_45d630);
        }
    }
    void** spot = (void**)Weighted_Pick(c, w, sum, &g_rng68b9c8, (BYTE*)last);
    if (spot == g_spots68d0bc) {
        AiError(0, 1, s_mainFile, s_mainId, 6529, s_err08322, &g_aiEmpty, &g_aiEmpty);
        return;
    }
    F(int, o, 108) = spot - g_spots68d0b8;
    F(int, (BYTE*)*spot, 40)++;
    F(double, o, 96) = CallC<double>(FUN_401000);
    int id;
    Presents_Append(&id, (int)o);
    g_presentHook68d11c(id, 1);
}

// ---------------------------------------------------------------------------
// Batch: a dealer's assortment

KING_VAR(AsmLabel, s_getAssortment, 66e104)
KING_VAR(AsmLabel, s_err0873, 66e0f0)
KING_VAR(AsmLabel, s_err0875, 66e0dc)

// [FUN_433350] getAssortment(PlayerId dealer, vector<VehicleId> * & asst, double & factor): the vehicles of the
// dealer's market slot at its node, or else of the closest non-empty slot of any market; 1 when the node's
// market lacks the slot
extern "C" int __cdecl Ai_GetAssortment(int dealer, BYTE** asst, double* factor)
{
    Trace_EnterBuffered((const char*)s_getAssortment);
    *asst = 0;
    PositionId pos = F(PositionId, (BYTE*)dealer, 40);
    int node = pos.node;
    int m;
    if (node != -1 && (m = F(int, g_roadNodes[node], 172)) != -1) {
        BYTE* market = (BYTE*)g_68cc98[m];
        *factor = F(double, market, 48) * g_64b690;
        BYTE* e = F(BYTE*, (BYTE*)g_68cc98[m], 28);
        BYTE* end = F(BYTE*, (BYTE*)g_68cc98[m], 32);
        while (e != end && F(int, e, 0) != node)
            e += 36;
        if (e == end) {
            AiError(0, 1, s_mainFile, s_mainId, 13363, s_err0873, &g_aiEmpty, &g_aiEmpty);
            Buf_Append(&g_trace, 0, g_tagMpl, 8);
            Buf_AppendString(&g_trace, 0, (const char*)s_getAssortment);
            return 1;
        }
        if (F(BYTE*, e, 24) == F(BYTE*, e, 28))
            AiError(0, 100, s_mainFile, s_mainId, 13368, s_err0875, &g_aiEmpty, &g_aiEmpty);
        *asst = e + 24;
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, (const char*)s_getAssortment);
        return 0;
    }
    DVec3 tmp;
    DVec3 pt = *CallC<const DVec3*>(FUN_40b0d0, &tmp, dealer);
    double best = 1.7976931348623157e308;
    for (void** mp = g_68cc98; mp != g_68cc9c; mp++) {
        for (BYTE* e = F(BYTE*, (BYTE*)*mp, 28); e != F(BYTE*, (BYTE*)*mp, 32); e += 36) {
            if (F(BYTE*, e, 24) == F(BYTE*, e, 28))
                continue;
            const DVec3& n = F(DVec3, g_roadNodes[F(int, e, 0)], 48);
            double dx = pt.x - n.x;
            double dy = pt.y - n.y;
            double dz = pt.z - n.z;
            double d = x87_sqrt((dy * dy + dz * dz) + dx * dx);
            if (best > d) {
                best = d;
                *asst = e + 24;
                *factor = F(double, (BYTE*)*mp, 48) * g_64b690;
            }
        }
    }
    Trace_Leave((const char*)s_getAssortment);
    return 0;
}

KING_VAR(AsmLabel, s_seeIfMustBeRetraced, 66ec0c)
KING_VAR(double, g_64b938, 64b938)
KING_VAR(double, g_64b788, 64b788)
KING_VAR(double, g_64b7c0, 64b7c0)

// [FUN_43eb90] seeIfMustBeRetraced(RoomId room, PlayerId player): a moving computer player whose point has
// strayed from its position (too far, or far and steeply) is routed again to its last hop's node, unless its
// point lies close (or close and not below) to the room's own position for it
extern "C" void __cdecl Ai_SeeIfMustBeRetraced(int room, int player)
{
    Trace_EnterBuffered((const char*)s_seeIfMustBeRetraced);
    BYTE* p = (BYTE*)player;
    if (F(int, p, 92) != 1 && F(BYTE*, p, 24) != F(BYTE*, p, 28) && (F(BYTE, p, 96) & 1)) {
        DVec3 v = F(DVec3, p, 624);
        DVec3 q;
        Road_PointOf(&q, F(PositionId, p, 40));
        DVec3 d;
        d.z = q.z - v.z;
        d.y = q.y - v.y;
        d.x = q.x - v.x;
        double xx = d.x * d.x;
        double len = x87_sqrt((d.y * d.y + d.z * d.z) + xx);
        bool flag = len > g_64b938;
        bool go = flag;
        if (len > g_64b788 && !(x87_sqrt(d.y * d.y + xx) >= d.z))
            go = flag = true;
        if (go) {
            PositionId r;
            Ai_GetPositionIdRoom(&r, room, &v);
            if (!(r.road == -1 && r.node == -1 && !(r.t < 0 || r.t > 0) && !r.segment && !r.object && !r.m_24)) {
                DVec3 q2;
                Road_PointOf(&q2, r);
                DVec3 e;
                e.z = q2.z - v.z;
                e.y = q2.y - v.y;
                e.x = q2.x - v.x;
                double len2 = x87_sqrt((e.y * e.y + e.z * e.z) + e.x * e.x);
                if (len2 >= g_64b788 && !(!(len2 >= g_64b938) && !(e.z >= g_64b7c0)) && flag)
                    CallC(FUN_487230, player, F(int, F(BYTE*, p, 28) - 12, 0));
            }
        }
    }
    Buf_Append(&g_trace, 0, g_tagMpl, 8);
    Buf_AppendString(&g_trace, 0, (const char*)s_seeIfMustBeRetraced);
}

// ---------------------------------------------------------------------------
// Batch: setting a player's matrix

KING_VAR(AsmLabel, s_setPlayerPositionMatrix, 66ec88)
KING_VAR(AsmLabel, s_xControlled66ec40, 66ec40)
extern "C" void FUN_420890(); KING_ALIAS("_FUN_420890", "_Player_DoneWaitingOf")    // (player) returns bool
extern "C" void FUN_4bdef0(); KING_ALIAS("_FUN_4bdef0", "@Vec3_SetThis@20")    // a float vector (thiscall: x, y, z)
extern "C" void FUN_4289e0(); KING_ALIAS("_FUN_4289e0", "_Ai_UpdateNavigator")    // updateNavigator (player)
extern "C" void FUN_422210(); KING_ALIAS("_FUN_422210", "_Player_PlaceAtDealer")    // (player + 8)

// [FUN_43ee70] setPlayerPosition(PlayerId player, vrMatrix const & m): only for a computer-driven player
// (+96 bit 0); unless FUN_420890, the matrix's point also moves its position (limited), and the human's
// navigator or a kind-9 player's dealer follows
extern "C" void __cdecl Ai_SetPlayerMatrix(int player, const float* m)
{
    Trace_EnterBuffered((const char*)s_setPlayerPositionMatrix);
    BYTE* p = (BYTE*)player;
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 16496, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    if (!(F(BYTE, p, 96) & 1)) {
        AiError(0, 1, s_mainFile, s_mainId, 16524, s_xControlled66ec40, &g_aiEmpty, &g_aiEmpty);
        Trace_Tag((const DWORD*)g_tagMpl);
        Buf_AppendString(&g_trace, 0, (const char*)s_setPlayerPositionMatrix);
        return;
    }
    if (CallC<char>(FUN_420890, player)) {
        for (int i = 0; i < 12; i++)
            F(double, p, 552 + i * 8) = (double)m[i];
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, (const char*)s_setPlayerPositionMatrix);
        return;
    }
    float f[3];
    CallThis(FUN_4bdef0, f, m[9], m[10], m[11]);
    DVec3 v;
    v.x = f[0];
    v.y = f[1];
    v.z = f[2];
    Ai_SetPlayerPositionTo(player, &v, 1);
    for (int i = 0; i < 12; i++)
        F(double, p, 552 + i * 8) = (double)m[i];
    if (F(int, p, 92) == 1)
        CallC(FUN_4289e0, player);
    else if (F(int, p, 92) == 9)
        CallC(FUN_422210, p + 8);
    Trace_Tag((const DWORD*)g_tagMpl);
    Buf_AppendString(&g_trace, 0, (const char*)s_setPlayerPositionMatrix);
}

KING_VAR(AsmLabel, s_setPlayerPositionRoomMatrix, 66ecc0)
extern "C" void FUN_43f5b0(); KING_ALIAS("_FUN_43f5b0", "_Vec3_MulMatrix")    // a float point through a float matrix (out, point, m)
extern "C" void FUN_43e4d0(); KING_ALIAS("_FUN_43e4d0", "_Ai_SetPlayerPositionV")    // (room, player, v)

// [FUN_43f240] setPlayerPosition(RoomId room, PlayerId player, vrMatrix const & m): as FUN_43ee70 within a room,
// then the retrace check, and for the human the matrix's origin point through FUN_43e4d0 and the navigator
extern "C" void __cdecl Ai_SetPlayerMatrixInRoom(int room, int player, const float* m)
{
    Trace_EnterBuffered((const char*)s_setPlayerPositionRoomMatrix);
    BYTE* p = (BYTE*)player;
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 16531, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    if (!(F(BYTE, p, 96) & 1))
        AiError(0, 1, s_mainFile, s_mainId, 16558, s_xControlled66ec40, &g_aiEmpty, &g_aiEmpty);
    else if (CallC<char>(FUN_420890, player)) {
        for (int i = 0; i < 12; i++)
            F(double, p, 552 + i * 8) = (double)m[i];
    } else {
        float f[3];
        CallThis(FUN_4bdef0, f, m[9], m[10], m[11]);
        DVec3 v;
        v.x = f[0];
        v.y = f[1];
        v.z = f[2];
        Ai_SetPlayerPositionInRoom(room, player, &v, 1);
        for (int i = 0; i < 12; i++)
            F(double, p, 552 + i * 8) = (double)m[i];
        Ai_SeeIfMustBeRetraced(room, player);
        if (F(int, p, 92) == 1) {
            float zero[3] = {0, 0, 0};
            float out[3];
            const float* o = CallC<const float*>(FUN_43f5b0, out, zero, m);
            DVec3 w;
            w.x = o[0];
            w.y = o[1];
            w.z = o[2];
            CallC(FUN_43e4d0, room, player, &w);
            CallC(FUN_4289e0, player);
        } else if (F(int, p, 92) == 9)
            CallC(FUN_422210, p + 8);
    }
    Buf_Append(&g_trace, 0, g_tagMpl, 8);
    Buf_AppendString(&g_trace, 0, (const char*)s_setPlayerPositionRoomMatrix);
}

// ---------------------------------------------------------------------------
// Batch: competitors for a special race

KING_VAR(AsmLabel, s_provideCompetitionSpecial, 66d98c)
KING_VAR(double, g_64b728, 64b728)
KING_VAR(double, g_64b780, 64b780)
KING_VAR(double, g_64b7d0, 64b7d0)

// [FUN_425dd0] provideCompetitionSpecial(): free players become competitors with the human's goods until there
// are enough: placed at the goods, sent to its town, put at the human's point and spread out by a random
// advancement (-150 or 150 near zero)
extern "C" void __cdecl Ai_ProvideCompetitionSpecial()
{
    Trace_EnterBuffered((const char*)s_provideCompetitionSpecial);
    int hn;
    Player_FromId(&hn);
    if ((void*)hn == g_players) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, (const char*)s_provideCompetitionSpecial);
        return;
    }
    BYTE* player = (BYTE*)hn + 8;
    while (Competitors_Count() < g_68d15c) {
        int found;
        Players_Find4571a0(&found, hn, (int)g_rng68b9c8);
        if ((void*)found == g_players) {
            Players_Find457210(&found, hn, (int)g_rng68b9c8);
            if ((void*)found == g_players)
                break;
        }
        BYTE* f = (BYTE*)found;
        Player_ClearOrderLists(f + 8);
        CallC(FUN_425820, f, Player_FirstGoods(player));
        g_addingCompetitor68d114 = 1;
        Player_Place(f, F(PositionId, Player_FirstGoods(player), 24));
        int town;
        Town_Id(&town, (BYTE*)TownId_Town((int*)((BYTE*)Player_FirstGoods(player) + 12), 0));
        PositionId target;
        PositionInit(&target, town);
        Player_GoTo(f, target, 1);
        DVec3 at;
        Ai_SetPlayerPosition(f, Road_PointOf(&at, F(PositionId, (BYTE*)hn, 40)));
        DVec3 q;
        Road_PointOf(&q, F(PositionId, f, 40));
        int k = (int)(g_68d160 - (unsigned)(g_68a984 - g_68a980) - g_68d164);
        if (k <= 0)
            k--;
        unsigned r = AiRandom() % 20;
        double v = (double)r + (double)k * g_64b728 - g_64b780;
        if (v > g_64b7d0 && v < g_zero64b678)
            v = -150.0;
        else if (!(v >= g_64b728) && v > g_zero64b678)
            v = 150.0;
        CallC(FUN_435a90, f, &v);
        Player_MarkListed(f);
        F(int, f, 100) = 4;
        Player_EndDealOf(f);
        Player_UpdateFlags(f);
        Ai_UpdateAdvancementRegular(f + 8, F(PositionId, f, 40));
        g_addingCompetitor68d114 = 0;
    }
    Trace_Leave((const char*)s_provideCompetitionSpecial);
}

KING_VAR(AsmLabel, s_hitPlayer, 66f420)
KING_VAR(AsmLabel, s_xDamaged66f414, 66f414)
KING_VAR(AsmLabel, s_orderLost66f408, 66f408)
KING_VAR(double, g_64b660, 64b660)
KING_VAR(double, g_64b958, 64b958)
extern "C" void FUN_429e40(); KING_ALIAS("_FUN_429e40", "_Orders_RemoveEmpty")    // whether orders are lost, with their value (orders, double*) returns bool
extern "C" void FUN_417530(); KING_ALIAS("_FUN_417530", "_Player_Leave")    // (player)

// [FUN_446d20] hitPlayer(PlayerId player, double impact, int): above 64b660, each order's integrity (+96) drops
// by the impact against its goods' strength (+64), shown to a human; lost orders are paid for (+136) and
// announced; a racer left without an order quits the race
extern "C" void __cdecl Ai_HitPlayer(int player, double impact, int)
{
    Trace_EnterBuffered((const char*)s_hitPlayer);
    BYTE* p = (BYTE*)player;
    if (CallC<int>(FUN_4566c0, player)) {
        sprintf(g_numberText, (const char*)s_idFormat66d4e4);
        AiError(33, 100, s_mainFile, s_mainId, 18386, s_invalidId, s_playerId66d4d8, g_numberText);
    }
    if (impact > g_64b660) {
        BYTE* fa = 0;
        BYTE* o = F(BYTE*, Player_Orders(p, fa), 0);
        BYTE* fb = 0;
        for (;; o += 144) {
            fb = Player_Orders(p, fb);
            if (o == F(BYTE*, fb, 4))
                break;
            double x = (g_64b670 - F(double, g_68ccb8 + F(int, o, 0) * 104, 64)) * impact * g_64b958;
            if (x > g_64b670)
                x = g_64b670;
            else if (x < g_64b5a0)
                x = g_zero64b678;
            F(double, o, 96) = (g_64b670 - x) * F(double, o, 96);
            if (CallC<char>(FUN_417660, player) && !(F(DWORD, o, 96) == 0 && F(DWORD, o, 100) == 0x3ff00000)) {
                int pct = Ftol((g_64b670 - F(double, o, 96)) * g_64b6a0);
                char buf[128];
                sprintf(buf, Message_Text((const char*)s_xDamaged66f414), (double)pct);
                CallC(FUN_483cb0, buf);
            }
        }
        double lost = 0;
        if (CallC<char>(FUN_429e40, Player_Orders(p, 0), &lost)) {
            F(double, p, 136) = F(double, p, 136) - lost;
            CallC(FUN_477210, player);
            if (CallC<char>(FUN_417660, player)) {
                CallC(FUN_56ae60, Message_Text((const char*)s_orderLost66f408));
                Ai_AccountForChangeInLoadState(player);
                char buf[128];
                sprintf(buf, Message_Text((const char*)s_forfeit66f2bc), lost);
                CallC(FUN_56ae60, buf);
                CallC(FUN_56f6d0);
                CallC(FUN_483e60, lost);
                CallC(FUN_483e80, 7, 0);
            }
            CallC(FUN_426f20, player);
            if (CallC<char>(FUN_4172e0, player) && !CallC<int>(FUN_477880, player)) {
                g_68d15c--;
                CallC(FUN_417530, player);
            }
        }
    }
    Trace_Leave((const char*)s_hitPlayer);
}

// ---------------------------------------------------------------------------
// Batch: the markets' assortments

KING_VAR(AsmLabel, s_noCarsForOrder66e1d4, 66e1d4)
KING_VAR(double, g_64b900, 64b900)
extern "C" void FUN_457820(); KING_ALIAS("_FUN_457820", "_Players_SelectBmw")    // the orderable vehicle types (out, first, last, -)
extern "C" void FUN_434700(); KING_ALIAS("_FUN_434700", "@Height_Closeness@12")    // a type's weight (thiscall {limit}: it) returns double
extern "C" void FUN_45a030(); KING_ALIAS("_FUN_45a030", "@Vec4_PushBackJ@12")    // vector<4 bytes>::push_back (thiscall: &x)
extern "C" void FUN_447530(); KING_ALIAS("_FUN_447530", "@Vec8_PushBack@12")    // vector<double>::push_back (thiscall: &x)
extern "C" void FUN_460770(); KING_ALIAS("_FUN_460770", "_Ptrs_FindName")    // the next type matching a list (first, last, list, -)
extern "C" void FUN_450f70(); KING_ALIAS("_FUN_450f70", "@Vec4b_InsertL@16")    // vector<4 bytes>::insert_aux (thiscall: pos, &x)
extern "C" void FUN_457b50(); KING_ALIAS("_FUN_457b50", "_CopyDwords")    // copy of 4-byte elements (first, last, dest)
extern "C" void FUN_44b090(); KING_ALIAS("_FUN_44b090", "@Vec1_Free@8")    // vector<char> destructor (thiscall)

// [FUN_434270] fills each market slot's assortment (+24): up to the market's count (+48) of vehicle types picked
// by weight, each bringing every type that matches its list (+20 of its record) into the slot
extern "C" void __cdecl Markets_FillAssortments()
{
    DWORD junk = 0;
    BYTE* v0[3];
    CallC(FUN_457820, v0, g_vehicles, g_vehiclesEnd, junk);
    if (v0[0] == v0[1])
        AiError(0, 1, s_mainFile, s_mainId, 13809, s_noCarsForOrder66e1d4, &g_aiEmpty, &g_aiEmpty);
    for (void** mp = g_68cc98; mp != g_68cc9c; mp++) {
        double cnt = F(double, (BYTE*)*mp, 48);
        for (BYTE* s = F(BYTE*, (BYTE*)*mp, 28); s != F(BYTE*, (BYTE*)*mp, 32); s += 36) {
            BYTE** v = (BYTE**)(s + 24);
            BYTE* w[3];
            Vec4_Reserve((VecMethods*)w, 0, (unsigned)(v0[1] - v0[0]) >> 2, 0);
            w[1] = CallC<BYTE*>(FUN_46b0e0, v0[0], v0[1], w[0], s);
            if (g_zero64b678 < cnt)
                for (int k = 0; w[0] != w[1];) {
                    double limit = cnt * g_64b900;
                    BYTE* c[3] = {0, 0, 0};
                    BYTE* wt[3] = {0, 0, 0};
                    double sum = 0;
                    for (BYTE* it = w[0]; it != w[1]; it += 4) {
                        double x = CallThis<double>(FUN_434700, &limit, it);
                        if (x > g_zero64b678) {
                            sum = x + sum;
                            CallThis(FUN_45a030, c, &it);
                            CallThis(FUN_447530, wt, &x);
                        }
                    }
                    BYTE* pick = Weighted_Pick(c, wt, sum, &g_rng68b9c8, w[1]);
                    if (pick == w[1])
                        break;
                    BYTE* r = **(BYTE***)pick;
                    BYTE* q[3];
                    Vec1_Reserve((VecMethods*)q, 0, F(BYTE*, r, 24) - F(BYTE*, r, 20), 0);
                    q[1] = CallC<BYTE*>(FUN_4072f0, F(BYTE*, r, 20), F(BYTE*, r, 24), q[0]);
                    for (;;) {
                        BYTE** it2 = CallC<BYTE**>(FUN_460770, w[0], w[1], q[0], s);
                        if ((BYTE*)it2 == w[1])
                            break;
                        DWORD val = **(DWORD**)*it2;
                        if (v[1] != v[2]) {
                            CallC(FUN_4611c0, v[1], &val);
                            v[1] += 4;
                        } else
                            CallThis(FUN_450f70, v, v[1], &val);
                        if ((BYTE*)(it2 + 1) != w[1])
                            CallC(FUN_457b50, it2 + 1, w[1], it2);
                        w[1] -= 4;
                    }
                    CallThis(FUN_44b090, q);
                    k++;
                    if (!((double)k < cnt))
                        break;
                }
            CallThis(FUN_44b0d0, w);
        }
    }
    CallThis(FUN_447970, v0);
}

// ---------------------------------------------------------------------------
// Batch: the players' boxes on their roads

KING_VAR(AsmLabel, s_updatePlayerBoxes, 66dd3c)
KING_VAR(double, g_64b7b8, 64b7b8)
extern "C" void FUN_487ee0(); KING_ALIAS("_FUN_487ee0", "@DBox_Init@16")    // a box from two points (thiscall: a, b)
extern "C" void FUN_483ff0(); KING_ALIAS("_FUN_483ff0", "_Route_Call529880")    // (player, road)

// [FUN_42cf50] updatePlayerBoxes(): each human's box (+888) of its points along its road (+936, restarted on a
// new road); once it spans enough of the road's box (64b7b8), the road gets flag 0x200 and FUN_483ff0
extern "C" void __cdecl Ai_UpdatePlayerBoxes()
{
    Trace_EnterBuffered((const char*)s_updatePlayerBoxes);
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        if (F(int, CallC<BYTE*>(FUN_40a9f0, it), 84) != 1)
            continue;
        PositionId pos = F(PositionId, CallC<BYTE*>(FUN_40a9f0, it), 32);
        int road = pos.road;
        if (road == -1)
            continue;
        BYTE* d = CallC<BYTE*>(FUN_40a9f0, it);
        DVec3 pt = (F(BYTE, d, 88) & 1) ? F(DVec3, d, 616) : F(DVec3, d, 304);
        if (F(int, CallC<BYTE*>(FUN_40a9f0, it), 936) != road) {
            F(int, CallC<BYTE*>(FUN_40a9f0, it), 936) = road;
            BYTE box[48];
            BYTE* b = CallThis<BYTE*>(FUN_487ee0, box, &pt, &pt);
            BYTE* dst = CallC<BYTE*>(FUN_40a9f0, it) + 888;
            for (int i = 0; i < 2; i++)
                DVec3_Assign((DVec3*)(dst + i * 24), 0, (const DVec3*)(b + i * 24));
            continue;
        }
        CallThis(FUN_487f60, CallC<BYTE*>(FUN_40a9f0, it) + 888, &pt);
        if (F(DWORD, g_roadEdges[F(int, CallC<BYTE*>(FUN_40a9f0, it), 936)], 16) & 512)
            continue;
        BYTE* a = CallC<BYTE*>(FUN_40a9f0, it);
        BYTE* bx = CallC<BYTE*>(FUN_40a9f0, it) + 888;
        double x = F(double, bx, 0) - F(double, a, 912);
        double y = F(double, bx, 8) - F(double, a, 920);
        double z = F(double, bx, 16) - F(double, a, 928);
        double span = x87_sqrt((x * x + y * y) + z * z);
        const BYTE* e160 = (const BYTE*)g_roadEdges[F(int, CallC<BYTE*>(FUN_40a9f0, it), 936)] + 160;
        const BYTE* v = (const BYTE*)RoadId_Edge((const int*)(CallC<BYTE*>(FUN_40a9f0, it) + 936), 0) + 136;
        double x2 = F(double, v, 0) - F(double, e160, 0);
        double y2 = F(double, v, 8) - F(double, e160, 8);
        double z2 = F(double, v, 16) - F(double, e160, 16);
        double size = x87_sqrt((x2 * x2 + y2 * y2) + z2 * z2);
        BYTE* obj = F(BYTE**, (BYTE*)g_roadEdges[road], 120)[pos.object];
        if (F(BYTE, obj, 8) & 16)
            continue;
        if (span * g_64b7b8 > size) {
            F(DWORD, g_roadEdges[F(int, CallC<BYTE*>(FUN_40a9f0, it), 936)], 16) |= 512;
            CallC(FUN_483ff0, it, F(int, CallC<BYTE*>(FUN_40a9f0, it), 936));
        }
    }
    Trace_Leave((const char*)s_updatePlayerBoxes);
}

// ---------------------------------------------------------------------------
// Batch: the navigator's turn at the next junction

KING_VAR(double, g_64b720, 64b720)
extern "C" void FUN_47ff20(); KING_ALIAS("_FUN_47ff20", "_Road_StartOf")    // a road's start position by index (out, road)
extern "C" void FUN_4800a0(); KING_ALIAS("_FUN_4800a0", "_Road_EndOf")    // a road's end position by index (out, road)
extern "C" void FUN_487dc0(); KING_ALIAS("_FUN_487dc0", "_DVec3_Cross")    // the cross product (out, a, b)

// [FUN_420360] the navigator's sign for a player at the next junction (a node with more than two roads) on its
// hops, when within 64b728 of it: 14 left, 15 right, 16 straight by the turn's cross product, 17 none (also
// for a plain fork of two ordinary roads)
extern "C" int __cdecl Ai_NavigatorTurn(BYTE* p)
{
    int idx = Ai_FindPositionHops((HopVec*)(p + 16), F(PositionId, p, 32)) + 1;
    BYTE* hops = F(BYTE*, p, 16);
    BYTE* h = hops + idx * 12;
    if (!F(void*, h, 4))
        return 17;
    int node;
    for (;;) {
        node = F(int, h, 0);
        BYTE* n = (BYTE*)g_roadNodes[node];
        int deg = (int)((F(BYTE*, n, 36) - F(BYTE*, n, 32)) >> 2) + (int)((F(BYTE*, n, 24) - F(BYTE*, n, 20)) >> 2);
        if (deg > 2)
            break;
        if (!F(void*, h, 16))
            return 17;
        h += 12;
        idx++;
    }
    BYTE* n = (BYTE*)g_roadNodes[node];
    int plain = 0, special = 0;
    for (int j = 0; j < 2; j++)
        for (BYTE** e = F(BYTE**, n, 20 + j * 12); e != F(BYTE**, n, 24 + j * 12); e++)
            if (F(BYTE, *F(BYTE**, *e, 120), 8) & 16)
                special++;
            else
                plain++;
    BYTE* edge = F(BYTE*, hops + idx * 12, 4);
    PositionId q;
    q.road = -1;
    q.node = -1;
    q.t = 0;
    q.segment = 0;
    q.object = 0;
    q.m_24 = 0;
    q.m_28 = -1;
    PositionId tmp;
    if (node == F(int, F(BYTE*, edge, 112), 16))
        q = *CallC<const PositionId*>(FUN_47ff20, &tmp, F(int, edge, 12));
    else if (node == F(int, F(BYTE*, edge, 116), 16)) {
        q = *CallC<const PositionId*>(FUN_4800a0, &tmp, F(int, edge, 12));
        q.m_24 |= 1;
    }
    DVec3 pt = (F(BYTE, p, 88) & 1) ? F(DVec3, p, 616) : F(DVec3, p, 304);
    DVec3 r;
    const DVec3* rp = Road_PointOf(&r, q);
    double x = pt.x - rp->x;
    double y = pt.y - rp->y;
    double z = pt.z - rp->z;
    if (x87_sqrt((x * x + y * y) + z * z) > g_64b728)
        return 17;
    double off = 8.0;
    PositionId a1, a2;
    Ai_AdvancePosition(&a1, (int)g_players, p, q, &off, 0);
    off = -8.0;
    Ai_AdvancePosition(&a2, (int)g_players, p, q, &off, 0);
    RoomInfo info;
    RoomInfo_Init(&info);
    Road_GetRoadParameters(a2, &info);
    if (!(info.flags & 16)) {
        Road_GetRoadParameters(a1, &info);
        if (!(info.flags & 16) && plain == 2)
            return 17;
    }
    DVec3 d1, d2;
    CallC(FUN_4390a0, &d1, a1);
    CallC(FUN_4390a0, &d2, a2);
    d1.z = 0;
    double inv = g_64b670 / x87_sqrt(d1.x * d1.x + d1.y * d1.y);
    d2.z = 0;
    d1.x = inv * d1.x;
    d1.y = d1.y * inv;
    double inv2 = g_64b670 / x87_sqrt(d2.x * d2.x + d2.y * d2.y);
    d2.x = inv2 * d2.x;
    d2.y = d2.y * inv2;
    DVec3 c;
    CallC(FUN_487dc0, &c, &d1, &d2);
    if (c.z > g_64b5b8)
        return 14;
    if (!(c.z >= g_64b720))
        return 15;
    return 16;
}

// ---------------------------------------------------------------------------
// Batch: an ambush ahead of the human

KING_VAR(double, g_64b868, 64b868)
KING_VAR(double, g_64b870, 64b870)
extern "C" void FUN_484690(); KING_ALIAS("_FUN_484690", "_Road_OrientHops")    // orients the hops of a route (route)

// [FUN_42b550] whether an ambush lies ahead of the human: between 64b868 and 64b870 away, reachable by a route
// of at least two hops, and facing (+88) along the road at its position (+144)
extern "C" bool __cdecl Ambush_AheadOfHuman(BYTE* a)
{
    if (!a)
        return false;
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h == (BYTE*)g_players)
        return false;
    double dx = F(double, a, 56) - F(double, h, 624);
    double dy = F(double, a, 64) - F(double, h, 632);
    double dz = F(double, a, 72) - F(double, h, 640);
    double dist = x87_sqrt((dy * dy + dz * dz) + dx * dx);
    if (!(dist >= g_64b868))
        return false;
    if (dist > g_64b870)
        return false;
    BYTE* r[3];
    double cost = 0;
    CallC(FUN_4858b0, r, 1, F(PositionId, h, 40), F(PositionId, a, 144), &cost);
    bool res = false;
    if ((unsigned)((r[1] - r[0]) / 12) >= 2) {
        CallC(FUN_484690, r);
        DVec3 d;
        CallC(FUN_4390a0, &d, F(PositionId, a, 144));
        if (F(BYTE, r[1], -16) & 1) {
            d.x = -d.x;
            d.y = -d.y;
            d.z = -d.z;
        }
        res = (d.z * F(double, a, 104) + d.y * F(double, a, 96)) + d.x * F(double, a, 88) > g_zero64b678;
    }
    int cap = (r[2] - r[0]) / 12;
    if (cap)
        Ai_PoolFree(r[0], cap * 12);
    return res;
}

// ---------------------------------------------------------------------------
// Batch: an orientation from a point and a direction

KING_VAR(AsmLabel, s_getOrientationPD, 66e698)

// [FUN_4397e0] the orientation at a point facing a direction: the side (dir x up), the direction and the up
// (side x dir), each normalized, then the point
extern "C" BYTE* __cdecl Ai_OrientationFrom(BYTE* out, const DVec3* point, const DVec3* dir)
{
    Trace_EnterBuffered((const char*)s_getOrientationPD);
    DVec3 f = *dir;
    DVec3 up;
    up.x = 0;
    up.y = 0;
    up.z = 1.0;
    double inv = g_64b670 / x87_sqrt((f.y * f.y + f.z * f.z) + f.x * f.x);
    f.x = inv * f.x;
    f.y = f.y * inv;
    f.z = f.z * inv;
    DVec3 side, u2;
    CallC(FUN_487dc0, &side, &f, &up);
    const DVec3* c = CallC<const DVec3*>(FUN_487dc0, &u2, &side, &f);
    up.x = c->x;
    up.y = c->y;
    double cz = c->z;
    double inv2 = g_64b670 / x87_sqrt((up.y * up.y + cz * cz) + up.x * up.x);
    up.x = inv2 * up.x;
    up.y = up.y * inv2;
    up.z = cz * inv2;
    double inv3 = g_64b670 / x87_sqrt((side.x * side.x + side.y * side.y) + side.z * side.z);
    side.x = inv3 * side.x;
    side.y = side.y * inv3;
    side.z = side.z * inv3;
    DVec3 pt = *point;
    F(DVec3, out, 0) = side;
    F(DVec3, out, 24) = f;
    F(DVec3, out, 48) = up;
    F(DVec3, out, 72) = pt;
    Trace_Leave((const char*)s_getOrientationPD);
    return out;
}

// ---------------------------------------------------------------------------
// Batch: a computer player's advance per tick

KING_VAR(AsmLabel, s_prescribeAdvance, 66d548)
KING_VAR(double, g_64b6e8, 64b6e8)
KING_VAR(double, g_64b6e0, 64b6e0)
KING_VAR(double, g_66c708, 66c708)
KING_VAR(double, g_66c6b8, 66c6b8)
KING_VAR(double, g_66c6c0, 66c6c0)
KING_VAR(double, g_66c6c8, 66c6c8)
KING_VAR(double, g_66c718, 66c718)
KING_VAR(double, g_64b6d0, 64b6d0)
KING_VAR(double, g_64b6c8, 64b6c8)
KING_VAR(double, g_64b6c0, 64b6c0)
KING_VAR(double, g_64b6b8, 64b6b8)
extern "C" void FUN_4d5c50(); KING_ALIAS("_FUN_4d5c50", "_Ai_CarTargetSpeed")    // a vehicle's set speed (vehicle) returns double
extern "C" void FUN_4cd730(); KING_ALIAS("_FUN_4cd730", "_Vehicle_Value4cd730")    // (vehicle) returns double
extern "C" void FUN_4d8a40(); KING_ALIAS("_FUN_4d8a40", "_Vehicle_Exists")    // (vehicle) returns int
extern "C" void FUN_4881b0(); KING_ALIAS("_FUN_4881b0", "@Filter2_Update@16")    // a speed filter step (thiscall: double) returns double

// [FUN_41a9d0] prescribeAdvance(PlayerId player): how far the player moves since the last tick: at its vehicle's
// set speed when there is one, else at the cruise speed slowed by FUN_4cd730, for a racer by its distance to
// the human (faster behind, slower ahead), and through the vehicle's speed filter (+456) with a curve limit
extern "C" double __cdecl Ai_PrescribeAdvance(int player)
{
    Trace_EnterBuffered((const char*)s_prescribeAdvance);
    BYTE* p = (BYTE*)player;
    double v = CallC<double>(FUN_4d5c50, F(int, p, 84));
    if (v >= g_zero64b678) {
        double res = (CallC<double>(FUN_401000) - g_lastSpeedTime68d0f0) * v;
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, (const char*)s_prescribeAdvance);
        return res;
    }
    double s = 13.88888888888889;
    if (g_64b6e8 < g_humanSpeed66c6a0)
        s = g_humanSpeed66c6a0;
    double x = F(int, p, 84) == -1 ? g_zero64b678 : CallC<double>(FUN_4cd730, F(int, p, 84));
    s = (g_64b670 - x * g_64b6e0) * s;
    if (CallC<char>(FUN_4172e0, player) && !(F(DWORD, p, 96) & 0x1000000)) {
        double lead;
        bool ahead = Ai_HumanLead(p, &lead);
        double r = (lead < 0 ? -lead : lead) / g_66c708;
        if (!(r >= g_zero64b678))
            r = g_zero64b678;
        else if (r > g_64b670)
            r = g_64b670;
        if (ahead) {
            s = (g_66c6b8 * r + g_64b670) * g_humanSpeed66c6a0;
            if (!g_68d188 && !F(BYTE, p, 512))
                Ai_RetraceCompetitorRegular(player);
        } else {
            s = (g_64b670 - g_66c6c0 * r) * g_humanSpeed66c6a0;
            if (!g_68d188 && F(BYTE, p, 512))
                Ai_RetraceCompetitorRegular(player);
        }
        if (!(s >= g_66c6c8))
            s = g_66c6c8;
        F(BYTE, p, 512) = ahead;
    }
    if (F(int, p, 92) == 5)
        s = g_66c718 * s;
    if (F(int, p, 84) != -1 && CallC<int>(FUN_4d8a40, F(int, p, 84)) != 0) {
        double t = CallC<double>(FUN_401000) - g_lastSpeedTime68d0f0;
        if (t > g_64b6d8) {
            double m = 1.0;
            if (!F(BYTE, p, 976)) {
                double y = F(double, p, 984) + g_64b6d0;
                if (y > g_zero64b678 && y < g_64b6c8)
                    m = F(double, p, 984) * g_64b6c0;
            }
            for (int k = Ftol(t * g_64b6a0) - 1; k > 0; k--)
                CallThis<double>(FUN_4881b0, p + 456, m);
            s = CallThis<double>(FUN_4881b0, p + 456, m) * s;
            if (F(double, p, 984) > g_zero64b678) {
                double vx = F(double, p, 352), vy = F(double, p, 360), vz = F(double, p, 368);
                double len = x87_sqrt((vx * vx + vy * vy) + vz * vz);
                double lim = len + len * len * g_64b6b8 / (F(double, p, 984) + g_64b5b8) * t;
                if (s > lim)
                    s = lim;
            }
        }
    }
    double res = (CallC<double>(FUN_401000) - g_lastSpeedTime68d0f0) * s;
    Trace_Leave((const char*)s_prescribeAdvance);
    return res;
}

// ---------------------------------------------------------------------------
// Batch: lookups by script identifier

KING_VAR(AsmLabel, s_noMerchClass66c8f0, 66c8f0)
KING_VAR(AsmLabel, s_noVehicleId66cdec, 66cdec)
KING_VAR(AsmLabel, s_identifier66ce4c, 66ce4c)
KING_VAR(AsmLabel, s_pair66cb08, 66cb08)
KING_VAR(BYTE*, g_lists68b9bc, 68b9bc)
extern "C" void FUN_407f60(); KING_ALIAS("_FUN_407f60", "_Str_Member20Checked")    // a pair's head (out, pair)
extern "C" void FUN_407ff0(); KING_ALIAS("_FUN_407ff0", "_Str_Member32Checked")    // a pair's tail (out, pair)
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")    // an empty handle (thiscall)

// [FUN_4052f0] the index of the merchandise class with an identifier (-1, and an error, when none)
extern "C" int* __cdecl Merch_IndexOf(int* out, StrRef h)
{
    StrRef c1, key, c2;
    Str_CopyConstruct(&c1, 0, &h);
    StrRef* k = Str_Construct(&key, 0, c1.ptr, c1.len, c1.flags);
    Str_CopyConstruct(&c2, 0, k);
    BYTE* end = (BYTE*)Rec_Get1((const int*)&g_68ccb8, 0);
    BYTE* begin = (BYTE*)Rec_Get0((const int*)&g_68ccb8, 0);
    BYTE* it = (BYTE*)Find_ValueA(begin, end, (DWORD)c2.ptr, c2.len, c2.flags);
    Str_Release(&key);
    if (it == g_68ccbc) {
        CheckHandle(&h);
        AiError(89, 100, s_mainFile, s_mainId, 1176, s_noMerchClass66c8f0, F(const char*, h.ptr, 12), &g_aiEmpty);
        *out = -1;
    } else
        *out = (it - g_68ccb8) / 104;
    Str_Release(&h);
    return out;
}

// [FUN_40e6d0] the name record of an identifier in the list at 68b9b8 (its string value's +12)
extern "C" BYTE* __cdecl Lists_NameOf(StrRef h)
{
    StrRef c1, key, c2;
    Str_CopyConstruct(&c1, 0, &h);
    StrRef* k = Str_Construct(&key, 0, c1.ptr, c1.len, c1.flags);
    Str_CopyConstruct(&c2, 0, k);
    BYTE* end = (BYTE*)Rec_Get1((const int*)&g_lists68b9b8, 0);
    BYTE* begin = (BYTE*)Rec_Get0((const int*)&g_lists68b9b8, 0);
    BYTE* e = (BYTE*)Find_ValueC(begin, end, (DWORD)c2.ptr, c2.len, c2.flags);
    Str_Release(&key);
    if (e == g_lists68b9bc) {
        CheckHandle(&h);
        AiError(35, 1, s_mainFile, s_mainId, 2914, s_noVehicleId66cdec, F(const char*, h.ptr, 12), &g_aiEmpty);
    }
    StrRef tmp;
    StrRef* v = CallC<StrRef*>(FUN_4090a0, &tmp, e);
    StrRef s = {0, 0, 0};
    if (v->ptr) {
        if (CallVirt<int>(v->ptr, 0) != 3)
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(v->ptr, 4), (const char*)s_string);
        else {
            s.ptr = v->ptr;
            s.flags = v->flags;
            F(int, s.ptr, 4)++;
        }
    }
    Str_Release(&tmp);
    CheckHandle(&s);
    BYTE* res = s.ptr + 12;
    if (s.Owns() && s.ptr && --F(int, s.ptr, 4) == 0)
        CallVirt(s.ptr, 12, 1);
    Str_Release(&h);
    return res;
}

// [FUN_430230] each vehicle type's list of model identifiers (+84) resolved into vehicle indices (+96)
extern "C" void __cdecl Vehicles_ResolveModels()
{
    for (void** vp = g_vehicles; vp != g_vehiclesEnd; vp++) {
        StrRef it = {0, 0, 0};
        CallThis(FUN_40c100, &it, (BYTE*)*vp + 84);
        while ((BYTE)it.flags == 2) {
            StrRef tmp;
            StrRef* car = CallC<StrRef*>(FUN_407f60, &tmp, &it);
            StrRef s;
            CallThis(FUN_4bd4d0, &s);
            if (car->ptr) {
                if (CallVirt<int>(car->ptr, 0) != 9)
                    CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(car->ptr, 4), (const char*)s_identifier66ce4c);
                else {
                    s.ptr = car->ptr;
                    s.flags = car->flags;
                    F(int, s.ptr, 4)++;
                }
            } else {
                s.ptr = 0;
                s.flags = 0;
            }
            Str_Release(&tmp);
            StrRef arg;
            Str_CopyConstruct(&arg, 0, &s);
            int idx = Vehicle_IndexOf(arg);
            Vec4_PushBackG((BYTE*)*vp + 96, 0, &idx);
            CallThis(FUN_4a36c0, &s);
            StrRef tmp2;
            StrRef* next = CallC<StrRef*>(FUN_407ff0, &tmp2, &it);
            if (it.ptr && --F(int, it.ptr, 4) == 0
                && !(it.ptr == next->ptr && it.flags == next->flags && it.len == next->len))
                CallVirt(it.ptr, 12, 1);
            if (next->ptr) {
                if (CallVirt<int>(next->ptr, 0) != 2)
                    CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(next->ptr, 4), (const char*)s_pair66cb08);
                else {
                    it.ptr = next->ptr;
                    it.flags = next->flags;
                    F(int, it.ptr, 4)++;
                    it.len = 0;
                }
            } else {
                it.ptr = 0;
                it.flags = 0;
                it.len = 0;
            }
            Str_Release(&tmp2);
        }
        CallThis(FUN_4a36c0, &it);
    }
}

// ---------------------------------------------------------------------------
// Batch: reading game.cnf

KING_VAR(AsmLabel, s_gameCnf66cb30, 66cb30)
KING_VAR(AsmLabel, s_port66cb28, 66cb28)
KING_VAR(AsmLabel, s_cannotOpen66cb1c, 66cb1c)
KING_VAR(int, g_68d110, 68d110)
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert (thiscall: pos, first, last)

// [FUN_409310] reads game.cnf in the AI's directory: its player types (FUN_409100), then 68d110 into the
// world record (68cf28 +28)
extern "C" void __cdecl Ai_ReadGameCnf()
{
    char name[9];
    memcpy(name, (const char*)s_gameCnf66cb30, 9);
    BYTE* path[3];
    unsigned n = g_wayDirEnd68bacc - g_wayDir68bac8;
    Vec1_Reserve((VecMethods*)path, 0, n, 0);
    memmove(path[0], g_wayDir68bac8, g_wayDirEnd68bacc - g_wayDir68bac8);
    path[1] = path[0] + n;
    CallThis(FUN_44c220, path, path[1], name, name + strlen(name) + 1);
    StrRef r;
    int pb = (int)path[0];
    int one = 1;
    StrRef* rv = (StrRef*)Port_MakeA((DWORD*)&r, 0, &pb, &one);
    StrRef v = {0, 0, 0};
    if (rv->ptr) {
        if (CallVirt<int>(rv->ptr, 0) != 11)
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(rv->ptr, 4), (const char*)s_port66cb28);
        else {
            v.ptr = rv->ptr;
            v.flags = rv->flags;
            F(int, v.ptr, 4)++;
        }
    }
    CallThis(FUN_4a36c0, &r);
    CheckHandle(&v);
    if (CallVirt<int>(v.ptr, 40))
        AiError(23, 100, s_mainFile, s_mainId, 2023, s_cannotOpen66cb1c, name, &g_aiEmpty);
    StrRef vc;
    Str_CopyConstruct(&vc, 0, &v);
    StrRef w;
    CallC(FUN_4b4c20, &w, g_reader68bb58, vc.ptr, vc.len, vc.flags);
    StrRef l = {0, 0, 0};
    if (w.ptr) {
        if (CallVirt<int>(w.ptr, 0) != 2)
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(w.ptr, 4), (const char*)s_pair66cb08);
        else {
            l.ptr = w.ptr;
            l.flags = w.flags;
            F(int, l.ptr, 4)++;
        }
    }
    Pairs_ReadPlayerTypes((int)g_reader68bb58, l);
    F(int, g_68cf28, 28) = g_68d110;
    Str_Release(&w);
    CallThis(FUN_4a36c0, &v);
    if (path[2] != path[0])
        Ai_PoolFree(path[0], path[2] - path[0]);
}

KING_VAR(AsmLabel, s_restable66cd78, 66cd78)
extern "C" void FUN_44aa60(); KING_ALIAS("_FUN_44aa60", "@Vec24_Erase@16")    // vector<24-byte record>::erase (thiscall: first, last)
extern "C" void FUN_40c680(); KING_ALIAS("_FUN_40c680", "_Resource_FromEntry")    // a resource record from a pair (out, reader, pair by value)

// [FUN_40cef0] reads restable.sch into the resource records at 68cc08 (one per list entry, FUN_40c680)
extern "C" void __cdecl Ai_ReadRestable()
{
    CallThis(FUN_44aa60, &g_init68cc08, g_resources68cc08, g_resourcesEnd68cc0c);
    char name[13];
    memcpy(name, (const char*)s_restable66cd78, 13);
    int one = 1;
    StrRef r;
    StrRef* rv = (StrRef*)Port_MakeD((DWORD*)&r, 0, (int)name, &one);
    StrRef v = {0, 0, 0};
    if (rv->ptr) {
        if (CallVirt<int>(rv->ptr, 0) != 11)
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(rv->ptr, 4), (const char*)s_port66cb28);
        else {
            v.ptr = rv->ptr;
            v.flags = rv->flags;
            F(int, v.ptr, 4)++;
        }
    }
    CallThis(FUN_4a36c0, &r);
    CheckHandle(&v);
    if (CallVirt<int>(v.ptr, 40))
        AiError(23, 100, s_mainFile, s_mainId, 2732, s_cannotOpen66cb1c, name, &g_aiEmpty);
    StrRef vc;
    Str_CopyConstruct(&vc, 0, &v);
    StrRef w;
    CallC(FUN_4b4c20, &w, g_reader68bb58, vc.ptr, vc.len, vc.flags);
    StrRef l;
    l.CopyFrom(&w);
    while ((BYTE)l.flags == 2) {
        StrRef car;
        Str_Member20(&car, &l);
        BYTE rec[24];
        BYTE* p = CallC<BYTE*>(FUN_40c680, rec, g_reader68bb58, car.ptr, car.len, car.flags);
        Vec24_PushBackB(&g_init68cc08, 0, p);
        Rec40cb60_Destroy(rec, 0);
        StrRef tmp;
        StrRef* nx = CallC<StrRef*>(FUN_4090a0, &tmp, &l);
        CallThis(FUN_40c100, &l, nx);
        Str_Release(&tmp);
    }
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_4a36c0, &w);
    CallThis(FUN_4a36c0, &v);
}

// ---------------------------------------------------------------------------
// Batch: merchandise.sch and the resource records

KING_VAR(AsmLabel, s_merchandise66cd68, 66cd68)
extern "C" void FUN_44a590(); KING_ALIAS("_FUN_44a590", "@Vec104_Erase@16")    // vector<104-byte record>::erase (thiscall: first, last)
extern "C" void FUN_4af480(); KING_ALIAS("_FUN_4af480", "_SchList_Nth")    // the n-th element of a list (out, n, list by value)
extern "C" void FUN_455760(); KING_ALIAS("_FUN_455760", "@StringHandle_From@12")    // a string handle from a value (thiscall: value)
extern "C" void FUN_4556d0(); KING_ALIAS("_FUN_4556d0", "@NumberHandle_From@12")    // a number handle from a value (thiscall: value)
extern "C" void FUN_4aa430(); KING_ALIAS("_FUN_4aa430", "@SchValue_Object@8")    // a handle's object (thiscall)
extern "C" void FUN_4b0e90(); KING_ALIAS("_FUN_4b0e90", "@SchNumber_Value@8")    // a number's value (thiscall) returns double
extern "C" void FUN_40ca70(); KING_ALIAS("_FUN_40ca70", "@Rec40ca70_Construct@20")    // a resource entry (thiscall: text, double)
extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")    // an empty vector (thiscall: &alloc)

// [FUN_40c3b0] reads merchandise.sch into the merchandise classes at 68ccb8 (one record per list entry)
extern "C" void __cdecl Ai_ReadMerchandise()
{
    CallThis(FUN_44a590, &g_68ccb8, g_68ccb8, g_68ccbc);
    char name[16];
    memcpy(name, (const char*)s_merchandise66cd68, 16);
    int one = 1;
    StrRef r;
    StrRef* rv = (StrRef*)Port_MakeC((DWORD*)&r, 0, (int)name, &one);
    StrRef v = {0, 0, 0};
    if (rv->ptr) {
        if (CallVirt<int>(rv->ptr, 0) != 11)
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(rv->ptr, 4), (const char*)s_port66cb28);
        else {
            v.ptr = rv->ptr;
            v.flags = rv->flags;
            F(int, v.ptr, 4)++;
        }
    }
    CallThis(FUN_4a36c0, &r);
    CheckHandle(&v);
    if (CallVirt<int>(v.ptr, 40))
        AiError(23, 100, s_mainFile, s_mainId, 2698, s_cannotOpen66cb1c, name, &g_aiEmpty);
    StrRef vc;
    Str_CopyConstruct(&vc, 0, &v);
    StrRef w;
    CallC(FUN_4b4c20, &w, g_reader68bb58, vc.ptr, vc.len, vc.flags);
    StrRef l;
    l.CopyFrom(&w);
    while ((BYTE)l.flags == 2) {
        StrRef car;
        Str_Member20(&car, &l);
        BYTE rec[104];
        BYTE* p = Ai_ReadRecord40b920(rec, g_reader68bb58, car);
        Vec104_PushBack(&g_68ccb8, 0, p);
        ValueBuf_Destroy((void**)rec, 0);
        StrRef tmp;
        StrRef* nx = CallC<StrRef*>(FUN_4090a0, &tmp, &l);
        CallThis(FUN_40c100, &l, nx);
        Str_Release(&tmp);
    }
    CallThis(FUN_4a36c0, &l);
    CallThis(FUN_4a36c0, &w);
    CallThis(FUN_4a36c0, &v);
}

// [FUN_40c680] a resource record from a list entry: its name (+20 member) and, for each (text number) pair of
// the rest, a {number, text} entry
extern "C" BYTE* __cdecl Resource_FromEntry(BYTE* out, int reader, StrRef e)
{
    BYTE rec[24];
    CallThis(FUN_4bd4d0, rec);
    char alloc;
    CallThis(FUN_487730, rec + 12, &alloc);
    StrRef tmp;
    CallThis(FUN_40c100, rec, Str_Member20(&tmp, &e));
    Str_Release(&tmp);
    StrRef l = {0, 0, 0};
    StrRef t2;
    CallThis(FUN_40c100, &l, CallC<StrRef*>(FUN_4090a0, &t2, &e));
    Str_Release(&t2);
    while ((BYTE)l.flags == 2) {
        StrRef a;
        Str_Member20(&a, &l);
        StrRef c1;
        Str_CopyConstruct(&c1, 0, &a);
        BYTE o1[12];
        BYTE* r1 = CallC<BYTE*>(FUN_4af480, o1, 0, c1.ptr, c1.len, c1.flags);
        BYTE x[12];
        CallThis(FUN_455760, x, r1);
        CallThis(FUN_4a36c0, o1);
        StrRef c2;
        Str_CopyConstruct(&c2, 0, &a);
        BYTE o2[12];
        BYTE* r2 = CallC<BYTE*>(FUN_4af480, o2, 1, c2.ptr, c2.len, c2.flags);
        BYTE y[12];
        CallThis(FUN_4556d0, y, r2);
        CallThis(FUN_4a36c0, o2);
        double d = CallThis<double>(FUN_4b0e90, CallThis<BYTE*>(FUN_4aa430, y));
        BYTE t[24];
        BYTE* tp = CallThis<BYTE*>(FUN_40ca70, t, CallThis<BYTE*>(FUN_4aa430, x) + 12, d);
        Vec24_PushBackA(rec + 12, 0, tp);
        CallThis(FUN_404420, t + 8);
        CallThis(FUN_4a36c0, y);
        CallThis(FUN_4a36c0, x);
        CallThis(FUN_4a36c0, &a);
        StrRef t3;
        CallThis(FUN_40c100, &l, CallC<StrRef*>(FUN_4090a0, &t3, &l));
        CallThis(FUN_4a36c0, &t3);
    }
    CallThis(FUN_40cda0, out, rec);
    CallThis(FUN_4a36c0, &l);
    Rec40cb60_Destroy(rec, 0);
    CallThis(FUN_4a36c0, &e);
    return out;
}

// ---------------------------------------------------------------------------
// Batch: the name maps

extern "C" void FUN_44a980(); KING_ALIAS("_FUN_44a980", "@NameHash_Insert@16")    // hash_map<string, int>::insert_unique (thiscall: out, &pair)

static const char* NameOf(BYTE* rec)
{
    return F(BYTE*, rec, 0) != F(BYTE*, rec, 4) ? F(const char*, rec, 0) : 0;
}

static void Rope_Drop(BYTE* r)
{
    if (r && !InterlockedDecrement((LONG*)(r + 4)))
        CallThis(FUN_44f1b0, r);
}

// [FUN_402be0] the name maps from scratch: the rooms' names (68cff0), the road nodes' (68d008) and the
// registry's (68cf10), each to its index
extern "C" void __cdecl Ai_BuildNameMaps()
{
    Hash_Clear((HashTable*)&w_hash68cff0, 0);
    for (unsigned i = 0; i < (unsigned)(g_vec68d0dc - g_vec68d0d8); i++) {
        BYTE* rope = 0;
        const char* name = NameOf(g_vec68d0d8[i]);
        Rope_Apply44a840(&rope, 0, (int)name, AI_StrLen(name));
        BYTE* pr[2];
        Ref_Copy(pr, 0, &rope, 0);
        pr[1] = (BYTE*)i;
        DWORD out[3];
        CallThis(FUN_44a980, &w_hash68cff0, out, pr);
        if (pr[0])
            Rope_Unref(pr[0], 0);
        if (rope)
            Rope_Unref(rope, 0);
    }
    Hash_Clear((HashTable*)&g_init68d008, 0);
    for (unsigned i = 0; i < (unsigned)(g_roadNodesEnd - g_roadNodes); i++) {
        BYTE* rope = 0;
        const char* name = NameOf((BYTE*)g_roadNodes[i]);
        Rope_Apply44a840(&rope, 0, (int)name, strlen(name));
        BYTE* pr[2];
        Ref_Copy(pr, 0, &rope, 0);
        pr[1] = (BYTE*)i;
        DWORD out[3];
        CallThis(FUN_44a980, &g_init68d008, out, pr);
        Rope_Drop(pr[0]);
        Rope_Drop(rope);
    }
    Hash_Clear((HashTable*)&w_hash68cf10, 0);
    for (unsigned i = 0; i < (unsigned)(g_vec68d0cc - g_vec68d0c8); i++) {
        BYTE* rope = 0;
        const char* name = NameOf(g_vec68d0c8[i]);
        Rope_Apply44a840(&rope, 0, (int)name, strlen(name));
        BYTE* pr[2];
        pr[0] = rope;
        if (rope)
            InterlockedIncrement((LONG*)(rope + 4));
        pr[1] = (BYTE*)i;
        NameHash_Resize(&w_hash68cf10, 0, F(int, &w_hash68cf10, 16) + 1);
        void* out[3];
        NameHash_InsertNoResize(&w_hash68cf10, 0, out, (int**)pr);
        Rope_Drop(pr[0]);
        Rope_Drop(rope);
    }
}

// ---------------------------------------------------------------------------
// Batch: destructors and an order search

extern "C" void FUN_458a80(); KING_ALIAS("_FUN_458a80", "@Rec458a80_Copy@12")    // an order's copy constructor (thiscall: src)
extern "C" BYTE* __cdecl Orders_Find(BYTE* first, BYTE* last, Blob144 order, BYTE*);
extern "C" void FUN_460d10(); KING_ALIAS("_FUN_460d10", "_MsgBuf_FreeThunk")    // a 16-byte record's destructor (rec)
extern "C" void FUN_457b40(); KING_ALIAS("_FUN_457b40", "_AI_Free")    // frees a large block (p, bytes)

// [FUN_449910] vector<580-byte element>::~vector (thiscall)
extern "C" void __fastcall Vec580_Destroy(BYTE** v, void*)
{
    for (BYTE* e = v[0]; e != v[1]; e += 580)
        CallThis(FUN_50d190, e);
    int n = (v[2] - v[0]) / 580;
    if (n)
        Ai_PoolFree(v[0], n * 580);
}

// [FUN_402ec0] a rope handle's destructor (thiscall on the handle)
extern "C" void __fastcall RopeRef_Destroy(BYTE** self, void*)
{
    if (*self)
        Rope_Unref(*self, 0);
}

// [FUN_447570] vector<StrRef>::~vector (thiscall)
extern "C" void __fastcall StrRefVec_Destroy(BYTE** v, void*)
{
    for (StrRef* e = (StrRef*)v[0]; e != (StrRef*)v[1]; e++)
        Str_Release(e);
    int n = (v[2] - v[0]) / 12;
    if (n)
        Ai_PoolFree(v[0], n * 12);
}

// [FUN_457a40] the order in [first, last) equal to one given by value (last when none)
extern "C" BYTE* __cdecl Orders_FindEqual(BYTE* first, BYTE* last, Blob144 order)
{
    Blob144 copy;
    CallThis(FUN_458a80, &copy, &order);
    BYTE* r = Orders_Find(first, last, copy, last);
    BYTE* b = F(BYTE*, &order, 120);
    for (BYTE* e = b; e != F(BYTE*, &order, 124); e += 16)
        CallC(FUN_460d10, e);
    b = F(BYTE*, &order, 120);
    int n = (F(BYTE*, &order, 128) - b) >> 4;
    if (n) {
        unsigned bytes = n << 4;
        if (bytes > 128)
            CallC(FUN_457b40, b, bytes);
        else
            Ai_PoolFree(b, bytes);
    }
    return r;
}

// ---------------------------------------------------------------------------
// Batch: the spots' weights by kind

KING_VAR(double, g_64b700, 64b700)

static double Spot_WeightOf(const BYTE* spot, int kind)
{
    if (F(int, spot, 28) != kind)
        return g_zero64b678;
    return x87_pow(g_64b700, (double)F(int, spot, 40));
}

// [FUN_41d310] (thiscall) a spot's weight for kind 4: 64b700 to the power of its use count (+40), else 0
extern "C" double __fastcall Spot_Weight4(void*, void*, const BYTE* spot) { return Spot_WeightOf(spot, 4); }
// [FUN_41ecc0] the same for kind 1
extern "C" double __fastcall Spot_Weight1(void*, void*, const BYTE* spot) { return Spot_WeightOf(spot, 1); }
// [FUN_41ecf0] the same for kind 2
extern "C" double __fastcall Spot_Weight2(void*, void*, const BYTE* spot) { return Spot_WeightOf(spot, 2); }
// [FUN_41ed20] the same for kind 3
extern "C" double __fastcall Spot_Weight3(void*, void*, const BYTE* spot) { return Spot_WeightOf(spot, 3); }

// ---------------------------------------------------------------------------
// Batch: bad_alloc and the AI's operator new, creating players, ambush checks

typedef void (__cdecl* AiNewHandlerFn)();
KING_VAR(AsmLabel, g_vtblBadAlloc64b960, 64b960)
KING_VAR(AiNewHandlerFn, g_newHandler68d20c, 68d20c)
KING_VAR(AsmLabel, g_throwBadAlloc659d60, 659d60)
KING_VAR(AsmLabel, s_badAlloc66f484, 66f484)
KING_VAR(AsmLabel, s_updateAmbushes66dcb4, 66dcb4)
extern "C" void Exc_Construct();
KING_ALIAS("_Exc_Construct", "@Exc_Construct@12")    // exception::exception(const char* const&)
extern "C" void Exc_CopyConstruct();
KING_ALIAS("_Exc_CopyConstruct", "@Exc_CopyConstruct@12") // exception::exception(const exception&)
extern "C" void Exc_Destroy();
KING_ALIAS("_Exc_Destroy", "@Exc_Destroy@4")            // exception::~exception()
extern "C" void Cxx_Throw();
KING_ALIAS("_Cxx_Throw", "__CxxThrowException@8")             // _CxxThrowException(object, info)
extern "C" void $L_422400(); KING_ALIAS("_$L_422400", "_Ai_CreatePlayersNetwork")    // creating the players for a network game
extern "C" void $L_422f20(); KING_ALIAS("_$L_422f20", "_Ai_CreatePlayersRegular")    // createPlayersRegular()
extern "C" void FUN_42bb90(); KING_ALIAS("_FUN_42bb90", "_Ai_SetAmbushActive")   // setAmbushActive (ambush, on)

// [FUN_448540] bad_alloc(const char* what) (thiscall)
extern "C" BYTE* __fastcall BadAlloc_Construct(BYTE* self, void*, const char* what)
{
    CallThis(Exc_Construct, self, &what);
    F(void*, self, 0) = &g_vtblBadAlloc64b960;
    return self;
}

// [FUN_448560] bad_alloc's destructor (thiscall)
extern "C" void __fastcall BadAlloc_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vtblBadAlloc64b960;
    CallThis(Exc_Destroy, self);
}

// [FUN_4485b0] bad_alloc's copy constructor (thiscall)
extern "C" BYTE* __fastcall BadAlloc_Copy(BYTE* self, void*, const BYTE* src)
{
    CallThis(Exc_CopyConstruct, self, src);
    F(void*, self, 0) = &g_vtblBadAlloc64b960;
    return self;
}

// [FUN_4581a0] the AI's operator new: the new-handler (68d20c) runs before each try of malloc; with none
// set, bad_alloc is thrown
extern "C" void* __cdecl Ai_OperatorNew(unsigned n)
{
    for (AiNewHandlerFn h = g_newHandler68d20c; h; h = g_newHandler68d20c) {
        h();
        void* p = malloc(n);
        if (p)
            return p;
    }
    BYTE exc[12];
    BadAlloc_Construct(exc, 0, (const char*)s_badAlloc66f484);
    CallStd(Cxx_Throw, exc, &g_throwBadAlloc659d60);
    return 0;
}

// [FUN_423a90] creates the players, the network game's way (68d188 1 or 2) or createPlayersRegular
extern "C" int __cdecl Ai_CreatePlayers()
{
    if (g_68d188 == 1 || g_68d188 == 2)
        return CallC<int>($L_422400);
    return CallC<int>($L_422f20);
}

// [FUN_42b4c0] whether an ambush (if any) is far from the human: over 64b870 away, or no human
extern "C" bool __cdecl Ambush_FarFromHuman(BYTE* a)
{
    if (!a)
        return true;
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h == (BYTE*)g_players)
        return true;
    double dx = F(double, a, 56) - F(double, h, 624);
    double dy = F(double, a, 64) - F(double, h, 632);
    double dz = F(double, a, 72) - F(double, h, 640);
    return x87_sqrt((dy * dy + dz * dz) + dx * dx) > g_64b870;
}

// [FUN_42c330] puts a player at a random far position, arriving there
extern "C" void __cdecl Ai_PlacePlayerRandomly(int player)
{
    PositionId pos;
    Road_RandomFarPosition(&pos);
    Player_Place((BYTE*)player, pos);
    CallC(FUN_417820, player, pos);
}

// updateAmbushes() (the second function in FUN_42c330's PROC at $L_42c390, a task's callback): an active
// ambush far from the human ends, its player placed elsewhere; an armed one ahead of the human starts, and its
// linked active ambushes that are not ahead end
extern "C" void __cdecl Ai_UpdateAmbushes()
{
    Trace_EnterBuffered((const char*)s_updateAmbushes66dcb4);
    for (void** e = g_68a970; e != g_68a974; e++) {
        BYTE* a = (BYTE*)*e;
        if (F(BYTE, a, 28)) {
            if (Ambush_FarFromHuman(a)) {
                int v = F(int, (BYTE*)*e, 124);
                CallC(FUN_42bb90, *e, 0);
                if ((void*)v != g_players)
                    Ai_PlacePlayerRandomly(v);
            }
        } else if (F(BYTE, a, 240) && Ambush_AheadOfHuman(a)) {
            CallC(FUN_42bb90, *e, 1);
            for (BYTE** b = F(BYTE**, (BYTE*)*e, 220); b != F(BYTE**, (BYTE*)*e, 224); b++) {
                if (!F(BYTE, *b, 28) || Ambush_AheadOfHuman(*b))
                    continue;
                int w = F(int, *b, 124);
                CallC(FUN_42bb90, *b, 0);
                if ((void*)w != g_players)
                    Ai_PlacePlayerRandomly(w);
            }
        }
    }
    Trace_Leave((const char*)s_updateAmbushes66dcb4);
}

// ---------------------------------------------------------------------------
// Batch: the vehicle queues

KING_VAR(double, g_64b888, 64b888)
extern "C" void FUN_415790(); KING_ALIAS("_FUN_415790", "_PtrVec_FindFree")    // whether a vehicle's kinds (vector by value) hold a kind (kinds, kind) returns bool
extern "C" void FUN_448ab0(); KING_ALIAS("_FUN_448ab0", "@Vec4_FreeStorageB@8")    // vector<4 bytes> destructor (thiscall)

static bool Vehicle_HasKind(int i, int kind)
{
    BYTE kinds[12];
    Vec4b_Copy(kinds, 0, (const BYTE**)((BYTE*)g_vehicles[i] + 36));
    return CallC<char>(FUN_415790, F(DWORD, kinds, 0), F(DWORD, kinds, 4), F(DWORD, kinds, 8), kind) != 0;
}

static void IntVec_Push(BYTE** v, int* x)
{
    if (v[1] != v[2]) {
        CallC(FUN_4611c0, v[1], x);
        v[1] += 4;
    } else
        CallThis(FUN_452000, v, v[1], x);
}

static void IntVec_Shuffle(BYTE** v)
{
    BYTE* b = v[0];
    BYTE* e = v[1];
    if (b == e)
        return;
    for (BYTE* p = b + 4; p != e; p += 4) {
        unsigned r = Rng_Random(&g_rng68b9c8, 0, (unsigned)((p - b) >> 2) + 1);
        CallC(FUN_46cf00, p, b + r * 4);
    }
}

static unsigned Vehicles_Count() { return (unsigned)(g_vehiclesEnd - g_vehicles); }

// [FUN_42fa90] the vehicle queues of the world record (68cbf8): +96 the kind-7/8/2 vehicles (kind-1 owners
// three times, four rounds) shuffled, +24 the kind-2 ones (only owner kind 3 in network games), and +84/+108
// the kind-7/2 ones interleaved, those of owner kind 1 at the 66c6f0 ratio, +108 shuffled
extern "C" void __cdecl Ai_BuildVehicleQueues()
{
    BYTE* w = (BYTE*)g_68cbf8;
    Vec4_Erase(w + 24, 0, F(BYTE*, w, 24), F(BYTE*, w, 28));
    w = (BYTE*)g_68cbf8;
    Vec4_Erase(w + 84, 0, F(BYTE*, w, 84), F(BYTE*, w, 88));
    w = (BYTE*)g_68cbf8;
    Vec4_Erase(w + 96, 0, F(BYTE*, w, 96), F(BYTE*, w, 100));
    w = (BYTE*)g_68cbf8;
    Vec4_Erase(w + 108, 0, F(BYTE*, w, 108), F(BYTE*, w, 112));
    for (int round = 4; round; round--)
        for (int i = 0; (unsigned)i < Vehicles_Count(); i++) {
            if (!Vehicle_HasKind(i, 7) && !Vehicle_HasKind(i, 8) && !Vehicle_HasKind(i, 2))
                continue;
            IntVec_Push((BYTE**)((BYTE*)g_68cbf8 + 96), &i);
            if (F(int, g_vehicles[i], 32) == 1) {
                IntVec_Push((BYTE**)((BYTE*)g_68cbf8 + 96), &i);
                IntVec_Push((BYTE**)((BYTE*)g_68cbf8 + 96), &i);
            }
        }
    IntVec_Shuffle((BYTE**)((BYTE*)g_68cbf8 + 96));
    for (int start = 0; start < 2; start++)
        for (int i = start; (unsigned)i < Vehicles_Count(); i += 2)
            if ((!g_68d188 || F(int, g_vehicles[i], 32) == 3) && Vehicle_HasKind(i, 2))
                IntVec_Push((BYTE**)((BYTE*)g_68cbf8 + 24), &i);
    BYTE* other[3];
    BYTE* owned[3];
    char alloc;
    CallThis(FUN_487730, other, &alloc);
    CallThis(FUN_487730, owned, &alloc);
    for (int i = 0; (unsigned)i < Vehicles_Count(); i++)
        if ((Vehicle_HasKind(i, 7) || Vehicle_HasKind(i, 2)) && F(int, g_vehicles[i], 32) == 1)
            IntVec_Push(owned, &i);
    for (int start = 1; start >= 0; start--)
        for (int i = start; (unsigned)i < Vehicles_Count(); i += 2)
            if ((Vehicle_HasKind(i, 7) || Vehicle_HasKind(i, 2)) && F(int, g_vehicles[i], 32) != 1)
                IntVec_Push(other, &i);
    double ratio = g_66c6f0 * g_64b888;
    double a = 0, b = 0;
    unsigned ia = 0, ib = 0;
    for (unsigned k = 0; k < (unsigned)((other[1] - other[0]) >> 2) * (unsigned)((owned[1] - owned[0]) >> 2); k++) {
        unsigned n1 = (unsigned)(other[1] - other[0]) >> 2;
        unsigned n2 = (unsigned)(owned[1] - owned[0]) >> 2;
        if (a * ratio > b) {
            IntVec_Push((BYTE**)((BYTE*)g_68cbf8 + 84), (int*)(owned[0] + ib % n2 * 4));
            b = b + g_64b670;
            ib++;
        } else {
            IntVec_Push((BYTE**)((BYTE*)g_68cbf8 + 84), (int*)(other[0] + ia % n1 * 4));
            a = a + g_64b670;
            ia++;
        }
    }
    w = (BYTE*)g_68cbf8;
    VecDword_Assign((DWORD**)(w + 108), 0, (DWORD**)(w + 84));
    IntVec_Shuffle((BYTE**)((BYTE*)g_68cbf8 + 108));
    CallThis(FUN_448ab0, owned);
    CallThis(FUN_448ab0, other);
}

// ---------------------------------------------------------------------------
// Batch: checking a vehicle order

KING_VAR(AsmLabel, s_err0880, 66e154)
KING_VAR(double, g_64b8f8, 64b8f8)
KING_VAR(double, g_64b8f0, 64b8f0)
extern "C" void FUN_41b8a0(); KING_ALIAS("_FUN_41b8a0", "_Vehicle_Costs")    // a vehicle's price parts (vehicle, out, double*)
extern "C" void FUN_45fdd0(); KING_ALIAS("_FUN_45fdd0", "_Markets_IntroSort")    // introsort of market iterators by distance (first, last, -, depth, point)
extern "C" void FUN_4601c0(); KING_ALIAS("_FUN_4601c0", "_Markets_FinalInsertion")    // the final insertion sort (first, last, point)
extern "C" void FUN_433240(); KING_ALIAS("_FUN_433240", "_AI_BaseDistance")    // a price factor at a point (point by value) returns double

// [FUN_433660] fills a vehicle order (r) for a model from a dealer: 3 when the dealer can't sell (a kind-6 one
// with a fine or the mafia, or not kind 6/7/9); the vehicle's data, the closest market's first free slot, the
// price factor; 1 when the dealer's assortment lacks the vehicle
extern "C" int __cdecl Ai_PrepareVehicleOrder(int player, int dealer, const char* model, BYTE* r)
{
    BYTE* d = (BYTE*)dealer;
    int kind = F(int, d, 92);
    if (kind == 6) {
        if (F(double, d, 224) > g_zero64b678 || F(BYTE, d, 201))
            return 3;
    } else if (kind != 7 && kind != 9)
        return 3;
    if (!model)
        return 0;
    int veh;
    CallC(FUN_414760, &veh, model);
    BYTE* v = (BYTE*)g_vehicles[veh];
    F(double, r, 24) = F(double, v, 48);
    F(int, r, 12) = F(int, (BYTE*)g_vehicles[veh], 72);
    DWORD x[2] = {0, 0};
    CallC(FUN_41b8a0, veh, r + 16, x);
    F(double, r, 32) = F(double, (BYTE*)g_vehicles[veh], 56);
    DVec3 pt;
    CallC(FUN_40b0d0, &pt, dealer);
    BYTE* w[3];
    Nodes_FarFrom(w, (BYTE**)g_68cc98, (BYTE**)g_68cc9c, &pt);
    DVec3 p2 = pt;
    if (w[0] != w[1]) {
        int k = 0;
        for (unsigned n = (unsigned)(w[1] - w[0]) >> 2; n != 1; n >>= 1)
            k++;
        CallC(FUN_45fdd0, w[0], w[1], 0, k * 2, p2);
        CallC(FUN_4601c0, w[0], w[1], p2);
    }
    BYTE** first = *(BYTE***)w[0];
    BYTE* m = *first;
    BYTE* s = F(BYTE*, m, 28);
    for (; s != F(BYTE*, m, 32); s += 36)
        if (F(DWORD, s, 4) >> 2 & 1)
            break;
    F(int, r, 56) = first - (BYTE**)g_68cc98;
    F(BYTE*, r, 60) = s;
    F(int, r, 64) = (s - F(BYTE*, *first, 28)) / 36;
    F(DWORD, r, 52) = F(DWORD, s, 0);
    F(DWORD, r, 48) = F(DWORD, s, 12);
    F(BYTE, r, 68) = F(DWORD, s, 4) >> 5 & 1;
    int res = 0;
    if (F(int, d, 92) == 6)
        F(double, r, 40) = CallC<double>(FUN_433240, p2) * g_64b8f8 + g_64b8f0;
    else {
        BYTE* asst;
        res = Ai_GetAssortment(dealer, &asst, (double*)(r + 40));
        if (!res) {
            if (!asst)
                AiError(0, 100, s_mainFile, s_mainId, 13484, s_err0880, &g_aiEmpty, &g_aiEmpty);
            if (F(int*, asst, 0) == F(int*, asst, 4))
                AiError(0, 100, s_mainFile, s_mainId, 13487, s_err0875, &g_aiEmpty, &g_aiEmpty);
            int* e = F(int*, asst, 0);
            for (; e != F(int*, asst, 4); e++)
                if (*e == veh)
                    break;
            res = e == F(int*, asst, 4) ? 1 : 0;
        }
    }
    int cap = (int)(w[2] - w[0]) >> 2;
    if (cap)
        Ai_PoolFree(w[0], cap * 4);
    return res;
}

// ---------------------------------------------------------------------------
// Batch 32: the AI's periodic player check

KING_VAR(double, q32_near64b6a0, 64b6a0)
KING_VAR(double, q32_farVeh66c740, 66c740)
KING_VAR(double, q32_idle64b750, 64b750)
KING_VAR(double, q32_far66c748, 66c748)
KING_VAR(double, q32_angle64b748, 64b748)
KING_VAR(double, q32_radius66c738, 66c738)
KING_VAR(double, q32_size64b740, 64b740)
KING_VAR(double, q32_kind2_64b6e0, 64b6e0)
KING_VAR(double, q32_kind3_64b738, 64b738)
KING_VAR(double, q32_chance64b730, 64b730)

extern "C" {

// [$L_420ed0] the task run every period: marks the parked players too far
// from the current one (or idle too long) with 0x20000 and removes them, then
// now and then adds a new player at a free spot about a radius away
void __cdecl Ai_CheckPlayersNear()
{
    if (!g_loadDone68d130)
        return;
    BYTE* cur;
    CallC(FUN_477b60, &cur);
    BYTE* head = (BYTE*)g_players;
    if (cur == head)
        return;
    BYTE* me = cur + 8;
    BYTE st[24];
    DVec3 at = *(const DVec3*)Player_CopyState(st, cur);
    for (BYTE* it = *(BYTE**)head; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = AI_ListData(it);
        if (!d || F(int, AI_ListData(it), 84) != 8)
            continue;
        int veh = F(int, AI_ListData(it), 76);
        bool mark = false;
        if (!(veh != -1 && Vehicle_Exists(veh))) {
            const DVec3* s = (const DVec3*)Player_CopyState(st, it);
            double x = s->x - at.x, y = s->y - at.y, z = s->z - at.z;
            if (x87_sqrt(x * x + y * y + z * z) < q32_near64b6a0)
                mark = true;
        }
        if (!mark) {
            veh = F(int, AI_ListData(it), 76);
            bool check = true;
            if (veh != -1 && Vehicle_Exists(veh)) {
                const DVec3* s = (const DVec3*)Player_CopyState(st, it);
                double x = s->x - at.x, y = s->y - at.y, z = s->z - at.z;
                if (!(x87_sqrt(y * y + z * z + x * x) > q32_farVeh66c740))
                    check = false;
            }
            if (check) {
                double now = AI_Now();
                if (now - F(double, AI_ListData(it), 944) > q32_idle64b750) {
                    mark = true;
                } else {
                    const DVec3* s = (const DVec3*)Player_CopyState(st, it);
                    double x = s->x - at.x, y = s->y - at.y, z = s->z - at.z;
                    mark = x87_sqrt(y * y + z * z + x * x) > q32_far66c748;
                }
            }
        }
        if (mark)
            F(DWORD, AI_ListData(it), 88) |= 0x20000;
    }
    if (g_loadNear68d138) {
        for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
            if (!AI_ListData(it) || F(int, AI_ListData(it), 84) != 8)
                continue;
            int veh = F(int, AI_ListData(it), 76);
            if (veh != -1 && Vehicle_Exists(veh))
                continue;
            F(DWORD, AI_ListData(it), 88) |= 0x20000;
        }
    }
    for (;;) {
        BYTE* it = *(BYTE**)g_players;
        while (it != (BYTE*)g_players && !(AI_ListData(it) && (F(DWORD, AI_ListData(it), 88) & 0x20000)))
            it = *(BYTE**)it;
        if (it == (BYTE*)g_players)
            break;
        Ai_RemovePlayer(it);
    }
    if (g_loadNear68d138)
        return;
    if (F(int, g_68cca8, 32) >= F(int, g_68cf28, 32))
        return;
    if (F(int, me, 92) == 5)
        return;
    if (!Vehicle_TimeOver(-1) && !(AiRandom() & 1))
        return;
    RoomInfo info;
    info.m_36 = -1;
    info.m_40 = -1;
    info.flags = 0;
    info.m_8 = 0;
    info.m_12 = 0;
    info.module = g_undefinedModule;
    info.room = g_undefinedRoom;
    info.m_24 = 0;
    info.m_28 = 0;
    Road_RoomInfo(*(const PositionId*)(me + 32), &info);
    int room = info.m_36;
    PositionId pos;
    bool found = false;
    for (int k = 0; k < 8 && !found; k++) {
        unsigned r = AiRandom();
        double a = (double)(__int64)(r % 628318) * q32_angle64b748;
        double c = x87_cos(a);
        double s = x87_sin(a);
        double R = q32_radius66c738;
        DVec3 v;
        v.x = R * c + R * c + at.x;
        v.y = s * R + s * R + at.y;
        v.z = 0.0 + at.z;
        PositionId tmp;
        pos = *Ai_GetPositionIdEx(&tmp, room, &v);
        Road_RoomInfo(pos, &info);
        if (info.flags & 0x50)
            continue;
        DVec3 p;
        Road_PointOf(&p, pos);
        double x = p.x - at.x, y = p.y - at.y, z = p.z - at.z;
        if (!(x87_sqrt(x * x + y * y + z * z) < R))
            found = true;
    }
    if (!found)
        return;
    double q = x87_sqrt(F(double, &info, 8) * q32_size64b740);
    if (info.m_32 == 2)
        q = q * q32_kind2_64b6e0;
    else if (info.m_32 == 3)
        q = q * q32_kind3_64b738;
    unsigned r = AiRandom() % 1000;
    if (q * q32_chance64b730 > (double)(__int64)r)
        Ai_NewPlayerAt(pos);
}

}

// ---------------------------------------------------------------------------
// Batch: the AI start of a game

extern "C" void $L_41b720(); KING_ALIAS("_$L_41b720", "_Task_FormOrders")
extern "C" void $L_41fcf0(); KING_ALIAS("_$L_41fcf0", "_Task_Standings")
extern "C" void $L_41ed50(); KING_ALIAS("_$L_41ed50", "_Task_PlacePresents")
extern "C" void $L_422370(); KING_ALIAS("_$L_422370", "_Task_Waiting")
extern "C" void $L_42b3a0(); KING_ALIAS("_$L_42b3a0", "_AI_SwitchToRandom")
extern "C" void $L_4279d0(); KING_ALIAS("_$L_4279d0", "_Ai_FillStartGrid")
extern "C" void $L_429930(); KING_ALIAS("_$L_429930", "_Ai_HireTask")
extern "C" void $L_429fb0(); KING_ALIAS("_$L_429fb0", "_Task_OrdersExpired")
extern "C" void $L_42a120(); KING_ALIAS("_$L_42a120", "_Task_PresentOrdersExpired")
extern "C" void __cdecl Task_DropOldPresents();
extern "C" void __cdecl Task_DropOldRacePresents();
extern "C" void $L_432dd0(); KING_ALIAS("_$L_432dd0", "_Ai_UpdateChasedCars")
extern "C" void $L_420040(); KING_ALIAS("_$L_420040", "_Task_TrafficJam")
extern "C" void __cdecl Task_FarSounds();
extern "C" void $L_4208b0(); KING_ALIAS("_$L_4208b0", "_Task_ChangeLanes")
extern "C" void $L_420b90(); KING_ALIAS("_$L_420b90", "_Task_RandomHits")
extern "C" void FUN_403940(); KING_ALIAS("_FUN_403940", "_Ai_Reset")
extern "C" void FUN_4305f0(); KING_ALIAS("_FUN_4305f0", "_Ambushes_Link")
extern "C" void FUN_414670(); KING_ALIAS("_FUN_414670", "_Towns_Costs")
extern "C" void FUN_433f90(); KING_ALIAS("_FUN_433f90", "_Towns_MarkNodes")
extern "C" void FUN_4340f0(); KING_ALIAS("_FUN_4340f0", "_Parkings_MarkNodes")
extern "C" void FUN_434070(); KING_ALIAS("_FUN_434070", "_Towns_MarkSpecial")
extern "C" void FUN_4095c0(); KING_ALIAS("_FUN_4095c0", "_Cars_CheckCount")
extern "C" void FUN_4279a0(); KING_ALIAS("_FUN_4279a0", "_AI_SetStartKey")
extern "C" void FUN_46d570(); KING_ALIAS("_FUN_46d570", "_Net_ListMachines")
extern "C" void FUN_434720(); KING_ALIAS("_FUN_434720", "_Players_ClearFlags440")
KING_VAR(AsmLabel, s_task66e340, 66e340)
KING_VAR(AsmLabel, s_task66e330, 66e330)
KING_VAR(AsmLabel, s_task66e320, 66e320)
KING_VAR(AsmLabel, s_task66e310, 66e310)
KING_VAR(AsmLabel, s_task66e300, 66e300)
KING_VAR(AsmLabel, s_task66e2f0, 66e2f0)
KING_VAR(AsmLabel, s_task66e2e0, 66e2e0)
KING_VAR(AsmLabel, s_task66e2cc, 66e2cc)
KING_VAR(AsmLabel, s_task66e2b0, 66e2b0)
KING_VAR(AsmLabel, s_task66e2a0, 66e2a0)
KING_VAR(AsmLabel, s_task66e28c, 66e28c)
KING_VAR(AsmLabel, s_task66e278, 66e278)
KING_VAR(AsmLabel, s_task66e25c, 66e25c)
KING_VAR(AsmLabel, s_task66e248, 66e248)
KING_VAR(AsmLabel, s_task66e234, 66e234)
KING_VAR(AsmLabel, s_task66e224, 66e224)
KING_VAR(AsmLabel, s_task66e218, 66e218)
KING_VAR(AsmLabel, s_task66e208, 66e208)
KING_VAR(AsmLabel, s_task66e1fc, 66e1fc)
KING_VAR(AsmLabel, s_task66e1f0, 66e1f0)
KING_VAR(AsmLabel, s_task66e35c, 66e35c)
KING_VAR(AsmLabel, s_task66e34c, 66e34c)
extern "C" void FUN_4143c0(); KING_ALIAS("_FUN_4143c0", "_Ai_SortTownsMarketsDealers")
extern "C" void FUN_4835d0(); KING_ALIAS("_FUN_4835d0", "_AI_LoadFromStorage")
extern "C" void FUN_42cd70(); KING_ALIAS("_FUN_42cd70", "_Ai_ResetPlayerRooms")
extern "C" void FUN_41d340(); KING_ALIAS("_FUN_41d340", "_Ai_PlacePresents")
extern "C" void FUN_453b60(); KING_ALIAS("_FUN_453b60", "@List12_Clear@8")
KING_VAR(double, g_66c6e8, 66c6e8)
KING_VAR(AiText, g_vtbl64b908, 64b908)
KING_VAR(double, t_68d108, 68d108)
KING_VAR(double, g_64b918, 64b918)
KING_VAR(double, g_64b7a8, 64b7a8)
KING_VAR(double, g_64b910, 64b910)
KING_VAR(double, g_64b928, 64b928)

// a periodic AI task: started now, running fn every (lo, hi) seconds
static BYTE* Task_New(DWORD lo, DWORD hi, const void* name, void (*fn)())
{
    BYTE* t = (BYTE*)operator new(48);
    if (t) {
        F(DWORD, t, 8) = 0;
        F(double, t, 16) = CallC<double>(FUN_401000);
        F(DWORD, t, 24) = lo;
        F(DWORD, t, 28) = hi;
        F(const void*, t, 32) = name;
        F(void*, t, 40) = (void*)fn;
        F(void*, t, 0) = &g_vtbl64b920;
    }
    return t;
}

static void Task_Add(BYTE* t)
{
    BYTE* it;
    ListB_Insert(&g_68b99c, 0, &it, (BYTE**)*(BYTE**)&g_68b99c, &t);
}

// [FUN_434780] (with $L_434e2b and $L_434e64, the parts it runs on into past its ENDP) the AI's start of a
// game: the random generator seeded from the clock, the flags (bit 0 network game, bit 1 load), the settings,
// the world's files and tables, a loaded state or a new world, presents, rooms and marks, then the 20 tasks
extern "C" void __cdecl Ai_StartGame(const char* saved, DWORD flags, const BYTE* settings)
{
    CallC(FUN_403940);
    Rng_Seed(&g_rng68a870, 0, (unsigned)Ftol(CallC<double>(FUN_401000)) ^ 161803398);
    g_68d31c = flags & 1;
    if (g_68d31c)
        r_66f62a = flags >> 1 & 1;
    if (settings) {
        g_66c6f0 = F(double, settings, 0);
        g_jeep68d178 = F(BYTE, settings, 24);
    }
    Ai_ReadGameCnf();
    Ai_LoadWorldFiles();
    CallC(FUN_4305f0);
    CallC(FUN_4143c0);
    CallC(FUN_414670);
    CallC(FUN_433f90);
    CallC(FUN_4340f0);
    Markets_FillAssortments();
    CallC(FUN_434070);
    CallC(FUN_5e48f0);
    CallC(FUN_4095c0);
    if (g_68d188 == 0 || g_68d188 == 1)
        CallC(FUN_4279a0, !settings || !g_68d188 ? 1 : F(int, settings, 16));
    Ai_BuildVehicleQueues();
    double x = CallC<double>(FUN_483a20);
    F(DWORD, &t_68d108, 0) = 0;
    t_68a820 = x;
    g_lastSpeedTime68d0f0 = x;
    F(DWORD, &t_68d108, 4) = 0;
    if (saved && r_66f62a)
        CallC(FUN_4835d0, saved);
    else
        t_68d198 = CallC<double>(FUN_401000);
    if (!(t_68d108 < 0 || t_68d108 > 0))
        t_68d108 = CallC<double>(FUN_401000) + g_64b870;
    CallC(FUN_5e48f0);
    if (g_68d31c)
        CallC(FUN_46d570);
    x = CallC<double>(FUN_483a20);
    t_68a820 = x;
    g_lastSpeedTime68d0f0 = x;
    g_humanVehicleKey68d144 = 0;
    if (settings) {
        g_humanVehicleKey68d144 = F(int, settings, 8);
        BYTE** v = (BYTE**)&w_driverName68d0a8;
        CharVec_Erase(v, 0, v[0], v[1]);
        const char* name = F(const char*, settings, 12);
        CallThis(FUN_44c220, v, v[1], name, name + strlen(name) + 1);
    }
    if (saved && r_66f62a) {
        r_66c68c = 0;
        CallC(FUN_42cd70);
    } else {
        Ai_StartWorld();
        CallC(FUN_41d340, 0, 0);
        if (g_68d188 == 1 || g_68d188 == 0) {
            int n;
            Ai_PresentAtNode(*CallC<int*>(FUN_436510, &n, (const char*)s_task66e35c));
            Ai_PresentAtNode(*CallC<int*>(FUN_436510, &n, (const char*)s_task66e34c));
        }
    }
    if (g_68d188 == 1 || g_68d188 == 2)
        g_68d168 = 1;
    g_lastHumanLoad68d100 = -2048.0;
    g_66c6e8 = (double)(Rng_Random(&g_rng68a870, 0, 600) + 600);
    if (saved)
        g_68d170 = CallC<double>(FUN_401000) + g_66c6e8 - g_64b690;
    CallC(FUN_42cb20);
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h != (BYTE*)g_players && F(double, h, 768) > g_64b928)
        g_allowKind7_68d1a0 = 1;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it)
        if (CallC<BYTE*>(FUN_40a9f0, it) && F(int, CallC<BYTE*>(FUN_40a9f0, it), 148) != -1)
            ((DWORD*)r_68cc88[0])[F(int, CallC<BYTE*>(FUN_40a9f0, it), 148)] |= 1;
    if (saved)
        for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it)
            if (CallC<BYTE*>(FUN_40a9f0, it) && F(int, CallC<BYTE*>(FUN_40a9f0, it), 92) == 256
                && (F(DWORD, CallC<BYTE*>(FUN_40a9f0, it), 88) >> 2 & 1))
                F(int, F(BYTE*, CallC<BYTE*>(FUN_40a9f0, it), 964), 12)++;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it)
        if (CallC<BYTE*>(FUN_40a9f0, it) && F(int, CallC<BYTE*>(FUN_40a9f0, it), 92) == 257
            && (F(DWORD, CallC<BYTE*>(FUN_40a9f0, it), 88) >> 2 & 1))
            g_68d154++;
    CallC(FUN_434720);
    for (BYTE* n = *(BYTE**)*(BYTE**)&g_68b99c; n != *(BYTE**)&g_68b99c; n = *(BYTE**)n)
        operator delete(F(void*, n, 8));
    CallThis(FUN_453b60, &g_68b99c);
    if (g_68d188)
        goto done;
    Task_Add(Task_New(0, 0x40240000, s_task66e340, $L_41b720));
    Task_Add(Task_New(0, 0x40200000, s_task66e330, $L_41fcf0));
    Task_Add(Task_New(0, 0x401c0000, s_task66e320, $L_41ed50));
    {
        BYTE* t = (BYTE*)operator new(48);
        if (t) {
            double period = x87_pow(g_64b918, g_64b670 - g_66c6f0 * g_64b7a8) * g_64b910;
            F(DWORD, t, 8) = 0;
            F(double, t, 16) = CallC<double>(FUN_401000);
            F(double, t, 24) = period;
            F(const void*, t, 32) = s_task66e310;
            F(void*, t, 40) = (void*)Ai_CheckPlayersNear;
            F(void*, t, 0) = &g_vtbl64b920;
        }
        Task_Add(t);
    }
    Task_Add(Task_New(0, 0x403b0000, s_task66e300, $L_422370));
    {
        BYTE* t = (BYTE*)operator new(48);
        if (t) {
            double period = (double)(AiRandom() % 300 + 600);
            Note_Construct(t, 0, (int)$L_42b3a0, F(int, &period, 0), F(int, &period, 4),
                           (int)(const char*)s_task66e2f0);
            F(void*, t, 0) = &g_vtbl64b908;
            F(double, t, 16) = t_68d108;
        }
        Task_Add(t);
    }
    Task_Add(Task_New(0, 0x3ff00000, s_task66e2e0, (void (*)())Ai_UpdateAmbushes));
    Task_Add(Task_New(0, 0x403d8000, s_task66e2cc, $L_4279d0));
    {
        BYTE* comp = Task_New(0, 0x403d8000, s_task66e2b0, (void (*)())Ai_AddCompetitor);
        g_task68d120 = comp;
        Task_Add(comp);
    }
    Task_Add(Task_New(0, 0x403d8000, s_task66e2a0, $L_429930));
    Task_Add(Task_New(0, 0x3ff00000, s_task66e28c, $L_429fb0));
    Task_Add(Task_New(0, 0x3ff00000, s_task66e278, $L_42a120));
    Task_Add(Task_New(0, 0x40240000, s_task66e25c, Task_DropOldPresents));
    Task_Add(Task_New(0, 0x40330000, s_task66e248, Task_DropOldRacePresents));
    Task_Add(Task_New(0, 0x40080000, s_task66e234, $L_432dd0));
    Task_Add(Task_New(0, 0x4072c000, s_task66e224, (void (*)())Ai_MarkAmbushes));
    Task_Add(Task_New(0x66666666, 0x40226666, s_task66e218, $L_420040));
    Task_Add(Task_New(0x9999999a, 0x403a9999, s_task66e208, Task_FarSounds));
    Task_Add(Task_New(0x33333333, 0x401d3333, s_task66e1fc, $L_4208b0));
    Task_Add(Task_New(0x66666666, 0x40146666, s_task66e1f0, $L_420b90));
done:
    r_68d0ec = 1;
}

// ---------------------------------------------------------------------------
// Batch: AI task callbacks (orders expiring, presents)

KING_VAR(AsmLabel, s_orderExpired66dbd8, 66dbd8)
KING_VAR(AsmLabel, s_err09768, 66dbe8)
KING_VAR(double, g_64b850, 64b850)
KING_VAR(double, g_64b848, 64b848)
KING_VAR(double, g_64b858, 64b858)
KING_VAR(double, g_64b758, 64b758)
KING_VAR(double, g_64b730, 64b730)
extern "C" void FUN_429cd0(); KING_ALIAS("_FUN_429cd0", "_Orders_RemoveExpired")    // drops expired orders, with their value (orders, double*) returns bool
extern "C" void FUN_569150(); KING_ALIAS("_FUN_569150", "_Screen_569150")    // (id)

// [$L_429fb0] (task) the human's expired orders: their value is taken off its money and announced
extern "C" void __cdecl Task_OrdersExpired()
{
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h == (BYTE*)g_players)
        return;
    double lost = 0;
    bool any = CallC<char>(FUN_429cd0, Player_Orders(h, 0), &lost) != 0;
    F(double, h, 136) = F(double, h, 136) - lost;
    if (any) {
        CallC(FUN_425790, s);
        CallC(FUN_41ef90, s);
        CallC(FUN_477210, s);
        Ai_PlayerEvent((BYTE*)s, 13, 0);
        char buf[128];
        sprintf(buf, Message_Text((const char*)s_orderExpired66dbd8), lost);
        CallC(FUN_56ae60, buf);
        CallC(FUN_56f6d0);
        CallC(FUN_483e60, lost);
        CallC(FUN_483e80, 8, 0);
    }
    CallC(FUN_426f20, s);
}

// [$L_42a120] (task) a present holding a trailer order whose orders all expired waits again (68d01c)
extern "C" void __cdecl Task_PresentOrdersExpired()
{
    BYTE* head = (BYTE*)g_68cf6c;
    for (BYTE* n = *(BYTE**)head; n != (BYTE*)g_68cf6c; n = *(BYTE**)n) {
        BYTE* p = F(BYTE*, n, 8);
        BYTE* o = F(BYTE*, p, 20);
        if (!F(BYTE, p, 104) || !o || F(BYTE*, o, 28) == F(BYTE*, o, 32))
            continue;
        double lost = 0;
        CallC(FUN_429cd0, o + 28, &lost);
        if (F(BYTE*, o, 28) == F(BYTE*, o, 32)) {
            F(DWORD, p, 8) &= ~0x40u;
            F(double, p, 96) = CallC<double>(FUN_401000);
            g_68d01c++;
            g_68d0e4--;
        }
    }
}

static double Present_DistTo(const BYTE* d, const BYTE* p)
{
    double x = F(double, d, 616) - F(double, p, 32);
    double y = F(double, d, 624) - F(double, p, 40);
    double z = F(double, d, 632) - F(double, p, 48);
    return x87_sqrt((y * y + z * z) + x * x);
}

// (task, at $L_42a1c0 in $L_42a120's PROC) long-waiting empty trailer presents far from the human go away
extern "C" void __cdecl Task_DropOldPresents()
{
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h == (BYTE*)g_players)
        return;
    BYTE* d = h + 8;
restart:
    for (BYTE* n = *(BYTE**)g_68cf6c; n != (BYTE*)g_68cf6c; n = *(BYTE**)n) {
        BYTE* p = F(BYTE*, n, 8);
        if (!(CallC<double>(FUN_401000) - F(double, p, 96) > g_64b850))
            continue;
        BYTE* o = F(BYTE*, p, 20);
        if (!F(BYTE, p, 104) || !o || F(BYTE*, o, 28) != F(BYTE*, o, 32))
            continue;
        if (!(Present_DistTo(d, p) > g_64b848))
            continue;
        CallC(FUN_41ef10, n);
        goto restart;
    }
}

// (task, at $L_42a2c0) long-waiting race presents (kind 0x80, not 0x400) far from the human go away,
// freeing their spot
extern "C" void __cdecl Task_DropOldRacePresents()
{
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h == (BYTE*)g_players)
        return;
    BYTE* d = h + 8;
restart:
    for (BYTE* n = *(BYTE**)g_68cf6c; n != (BYTE*)g_68cf6c; n = *(BYTE**)n) {
        BYTE* p = F(BYTE*, n, 8);
        DWORD fl = F(DWORD, p, 8);
        if ((fl & 0x400) || (fl & 0xf0) != 0x80)
            continue;
        if (!(CallC<double>(FUN_401000) - F(double, p, 96) > g_64b858))
            continue;
        if (!(Present_DistTo(d, p) > g_64b758))
            continue;
        if (F(int, p, 108) == -1)
            AiError(0, 1, s_mainFile, s_mainId, 10638, s_err09768, &g_aiEmpty, &g_aiEmpty);
        F(int, (BYTE*)g_spots68d0b8[F(int, p, 108)], 40)--;
        CallC(FUN_41ef10, n);
        goto restart;
    }
}

// (task, at $L_42a3f0) the sounds (FUN_569150 of each id mod 1000) of the records at 68cbe8 far from the camera
extern "C" void __cdecl Task_FarSounds()
{
    for (void** e = g_68cbe8; e != g_68cbec; e++) {
        BYTE* o = (BYTE*)*e;
        double x = g_68bab0.x - F(double, o, 336);
        double y = g_68bab0.y - F(double, o, 344);
        double z = g_68bab0.z - F(double, o, 352);
        if (!(g_64b730 < x87_sqrt((y * y + z * z) + x * x)))
            continue;
        for (unsigned* id = F(unsigned*, o, 360); id != F(unsigned*, o, 364); id++)
            CallC(FUN_569150, *id % 1000);
    }
}

// ---------------------------------------------------------------------------
// Batch: AI task callbacks (orders, presents, standings)

KING_VAR(double, g_64b6f0, 64b6f0)
extern "C" void FUN_41fca0(); KING_ALIAS("_FUN_41fca0", "_Player_CostA")    // a driving player's first cost total (player) returns double
extern "C" void FUN_4880e0(); KING_ALIAS("_FUN_4880e0", "@Filter1_Update@16")    // a filter step (thiscall: double) returns double
extern "C" void FUN_421c00(); KING_ALIAS("_FUN_421c00", "_Players_DropIdleFar")
extern "C" void FUN_422280(); KING_ALIAS("_FUN_422280", "_Players_DropFar")
extern "C" void FUN_4221d0(); KING_ALIAS("_FUN_4221d0", "_Markets_Grow")    // (double)

// [$L_41b720] (task formOrders) outside network games and away from the last human load: each town's due
// goods (+16 passed) go away, and each free route slot (flags not 9) gets new goods when the town has kinds
extern "C" void __cdecl Task_FormOrders()
{
    double dt = CallC<double>(FUN_401000) - g_lastHumanLoad68d100;
    if (dt < 0)
        dt = -dt;
    if (!(dt >= g_64b6f0) || g_68d188)
        return;
    for (void** t = g_towns; t != g_townsEnd; t++) {
        BYTE* town = (BYTE*)*t;
        if (F(BYTE*, town, 28) == F(BYTE*, town, 32))
            continue;
        for (BYTE* e = F(BYTE*, town, 28); e != F(BYTE*, (BYTE*)*t, 32); e += 16) {
            if (F(BYTE, e, 4) & 9)
                continue;
            for (int j = 0; j < 2; j++) {
                BYTE* g = F(BYTE*, e, 8 + j * 4);
                if (g && !(CallC<double>(FUN_401000) < F(double, g, 16))) {
                    operator delete(g);
                    F(BYTE*, e, 8 + j * 4) = 0;
                }
            }
        }
        town = (BYTE*)*t;
        if (F(BYTE*, town, 40) == F(BYTE*, town, 44))
            continue;
        for (BYTE* e = F(BYTE*, town, 28); e != F(BYTE*, town, 32); e += 16) {
            if (F(BYTE, e, 4) & 9)
                continue;
            for (int j = 0; j < 2; j++)
                if (!F(BYTE*, e, 8 + j * 4))
                    Ai_NewGoods((BYTE**)(e + 8 + j * 4), e, (BYTE*)*t, j);
            town = (BYTE*)*t;
        }
    }
}

// [$L_41ed50] (task) places the missing presents, 720 ahead
extern "C" void __cdecl Task_PlacePresents() { Ai_PlacePresents(720.0); }

// [$L_41fcf0] (task) the players' standings: each one's speed filter takes its distance (+752, reset), then
// each team leader's share of all costs (+768 total, +760 share); a leader or the human passing the share
// mark (66c710, then 1.05) for the first time raises event 12
extern "C" void __cdecl Task_Standings()
{
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = CallC<BYTE*>(FUN_40a9f0, it);
        double dist = F(double, d, 752);
        CallThis<double>(FUN_4880e0, CallC<BYTE*>(FUN_40a9f0, it) + 728, dist);
        d = CallC<BYTE*>(FUN_40a9f0, it);
        F(DWORD, d, 752) = 0;
        F(DWORD, d, 756) = 0;
    }
    double total = 0;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it)
        total = CallC<double>(FUN_41fca0, it) + total;
    if (!(total < 0 || total > 0))
        return;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        double y = 0;
        BYTE* d = CallC<BYTE*>(FUN_40a9f0, it);
        if (F(void*, d, 168) == g_players) {
            y = CallC<double>(FUN_41fca0, it);
            for (BYTE* e = F(BYTE*, CallC<BYTE*>(FUN_40a9f0, it), 152); e != F(BYTE*, CallC<BYTE*>(FUN_40a9f0, it), 156);
                 e += 16)
                y = CallC<double>(FUN_41fca0, F(BYTE*, e, 0)) + y;
        }
        F(double, CallC<BYTE*>(FUN_40a9f0, it), 768) = y;
        F(double, CallC<BYTE*>(FUN_40a9f0, it), 760) = y / total;
        if (!(F(double, CallC<BYTE*>(FUN_40a9f0, it), 760) < r_66c710) && !F(BYTE, CallC<BYTE*>(FUN_40a9f0, it), 776)) {
            F(BYTE, CallC<BYTE*>(FUN_40a9f0, it), 776) = 1;
            F(DWORD, &r_66c710, 0) = 0xcccccccd;
            F(DWORD, &r_66c710, 4) = 0x3ff0cccc;
            g_allowKind7_68d1a0 = 1;
            Ai_PlayerEvent(it, 12, 0);
        }
    }
    int s;
    BYTE* h = (BYTE*)*Player_FromId(&s);
    if (h != (BYTE*)g_players && !(F(double, h, 768) < r_66c710) && !F(BYTE, h, 784)) {
        F(BYTE, h, 784) = 1;
        F(DWORD, &r_66c710, 0) = 0xcccccccd;
        F(DWORD, &r_66c710, 4) = 0x3ff0cccc;
        g_allowKind7_68d1a0 = 1;
        int s2;
        Ai_PlayerEvent((BYTE*)*Player_FromId(&s2), 12, 0);
    }
}

// [$L_422370] (task) removing idle waiting players, FUN_422280, then FUN_4221d0 720 ahead
extern "C" void __cdecl Task_Waiting()
{
    CallC(FUN_421c00);
    CallC(FUN_422280);
    CallC(FUN_4221d0, 720.0);
}

// ---------------------------------------------------------------------------
// Batch: AI task callbacks (traffic jams, lanes, collisions) and two small helpers

KING_VAR(AsmLabel, s_changeLanes66d840, 66d840)
KING_VAR(int, g_68d1ac, 68d1ac)
extern "C" void FUN_4207f0(); KING_ALIAS("_FUN_4207f0", "_Player_AtTarget")    // (player + 8) returns bool

// [FUN_420b60] a player's point (+616 when driven by the physics, flag 1, else +304)
extern "C" DVec3* __cdecl Player_Point(DVec3* out, const BYTE* d)
{
    *out = (F(BYTE, d, 88) & 1) ? F(DVec3, d, 616) : F(DVec3, d, 304);
    return out;
}

// [FUN_420840] FUN_4207f0 of a player's data
extern "C" bool __cdecl Player_Check4207f0(int player) { return CallC<char>(FUN_4207f0, (BYTE*)player + 8) != 0; }

// [$L_420040] (task) a traffic jam (68d138) when more than two of the slow players stand within 64b6f0 of
// another one
extern "C" void __cdecl Task_TrafficJam()
{
    g_loadNear68d138 = 0;
    BYTE* v[3];
    Players_SlowList(v, *(BYTE**)g_players, (BYTE*)g_players);
    if ((unsigned)((v[1] - v[0]) >> 2) > 2) {
        int count = 0;
        for (BYTE** i = (BYTE**)v[0]; i != (BYTE**)v[1]; i++)
            for (BYTE** j = (BYTE**)v[0]; j != i; j++) {
                DVec3 a, b;
                const DVec3* pj = CallC<const DVec3*>(FUN_40b0d0, &a, *j);
                const DVec3* pi = CallC<const DVec3*>(FUN_40b0d0, &b, *i);
                double x = pi->x - pj->x;
                double y = pi->y - pj->y;
                double z = pi->z - pj->z;
                if (!(x87_sqrt((x * x + y * y) + z * z) < g_64b6f0))
                    continue;
                if (!(F(DWORD, *i, 96) & 2)) {
                    F(DWORD, *i, 96) |= 2;
                    count++;
                }
                if (!(F(DWORD, *j, 96) & 2)) {
                    F(DWORD, *j, 96) |= 2;
                    count++;
                }
            }
        for (BYTE** i = (BYTE**)v[0]; i != (BYTE**)v[1]; i++)
            F(DWORD, *i, 96) &= ~2u;
        if (count > 2)
            g_loadNear68d138 = 1;
    }
    int cap = (int)(v[2] - v[0]) >> 2;
    if (cap)
        Ai_PoolFree(v[0], cap * 4);
}

// [$L_4208b0] (task changeLanes()) each driving computer player near the camera shows its navigator turn
// (event of the sign) when it changes
extern "C" void __cdecl Task_ChangeLanes()
{
    Trace_EnterBuffered((const char*)s_changeLanes66d840);
    g_68d1ac++;
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = CallC<BYTE*>(FUN_40a9f0, it);
        if (!d || F(int, CallC<BYTE*>(FUN_40a9f0, it), 76) == -1
            || !CallC<int>(FUN_4d8a40, F(int, CallC<BYTE*>(FUN_40a9f0, it), 76))
            || F(int, CallC<BYTE*>(FUN_40a9f0, it), 84) == 1 || Player_Check4207f0((int)it))
            continue;
        DVec3 tmp;
        const DVec3* p = Player_Point(&tmp, CallC<BYTE*>(FUN_40a9f0, it));
        double x = p->x - g_68bab0.x;
        double y = p->y - g_68bab0.y;
        double z = p->z - g_68bab0.z;
        if (!(x87_sqrt((x * x + y * y) + z * z) < g_64b6a0))
            continue;
        int sign = Ai_NavigatorTurn(CallC<BYTE*>(FUN_40a9f0, it));
        if (F(int, CallC<BYTE*>(FUN_40a9f0, it), 880) == sign)
            continue;
        Ai_PlayerEvent(it, sign, 0);
        F(int, CallC<BYTE*>(FUN_40a9f0, it), 880) = sign;
    }
    Trace_Leave((const char*)s_changeLanes66d840);
}

// [$L_420b90] (task) kind-2 players are hit (impact 4) by a roll against their vehicle's +48
extern "C" void __cdecl Task_RandomHits()
{
    for (BYTE* it = *(BYTE**)g_players; it != (BYTE*)g_players; it = *(BYTE**)it) {
        BYTE* d = CallC<BYTE*>(FUN_40a9f0, it);
        if (!d || F(int, (d = CallC<BYTE*>(FUN_40a9f0, it)), 84) != 2)
            continue;
        double val = F(double, CallThis<BYTE*>(FUN_474210, d), 48);
        unsigned r = AiRandom() % 1000000;
        if (!(val * g_64b6b0 > (double)r))
            Ai_HitPlayer((int)it, 4.0, 0);
    }
}

// ---------------------------------------------------------------------------
extern "C" void FUN_4010c0(); KING_ALIAS("_FUN_4010c0", "_Ai_FormatReport")    // formats an AI message (same args) into a new buffer
KING_VAR(int, g_aiLevel68a828, 68a828)
KING_VAR(AsmLabel, s_file66c4e8, 66c4e8)
KING_VAR(AsmLabel, s_mode66c4f4, 66c4f4)
extern "C" int __cdecl fputs(const char* s, FILE* f);
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);
extern "C" void __cdecl Sys_Exit(const char* msg);

// [FUN_401010..$L_40109e] an AI message (level, format args...): appended to the AI log file; level >= 200 is shown, 2..199 ends the program, others are fatal
extern "C" void __cdecl Ai_Report(int a1, int level, int a3, int a4, int a5, int a6, int a7, int a8)
{
    g_aiLevel68a828 = level;
    char* text = CallC<char*>(FUN_4010c0, a1, level, a3, a4, a5, a6, a7, a8);
    FILE* f = fopen(s_file66c4e8, s_mode66c4f4);
    fputs(text, f);
    fputs(s_suffix66c4e4, f);
    fclose(f);
    if (level >= 200)
        CallC(FUN_5d2ba0, text, 0);
    else if (level > 1)
        Sys_Exit(text);
    else
        Sys_Fatal(text, 0);
    if (text)
        operator delete(text);
}


// ---------------------------------------------------------------------------
extern "C" void __cdecl free(void* p);

// [FUN_462410] frees the storage at +8
extern "C" void __fastcall Vec_FreeStorage8(BYTE* self, void*)
{
    free(F(void*, self, 8));
}


// ---------------------------------------------------------------------------

// [FUN_45a270] lower_bound of *key among the unsigned values [first, last)
extern "C" unsigned* __cdecl U32_LowerBound(unsigned* first, unsigned* last, unsigned* key)
{
    int n = last - first;
    while (n > 0)
    {
        int h = n >> 1;
        if (first[h] < *key)
        {
            first += h + 1;
            n -= h + 1;
        }
        else
            n = h;
    }
    return first;
}

// [FUN_462d10] clears the first byte of a string
extern "C" void __stdcall Str_ClearFirst(char* s) { *s = 0; }


// ---------------------------------------------------------------------------

// [FUN_460770] the first entry of [first, last) whose object's name (**p +20) is name
extern "C" BYTE*** __cdecl Ptrs_FindName(BYTE*** first, BYTE*** last, const char* name)
{
    for (; first != last; first++)
        if (!strcmp(name, F(char*, **first, 20)))
            break;
    return first;
}


// ---------------------------------------------------------------------------
KING_VAR(AsmLabel, s_aw66c51c, 66c51c)
KING_VAR(AsmLabel, s_aw66c4f8, 66c4f8)

// [FUN_4010c0] formats an error report (a new buffer) with the local time and the last Windows error
extern "C" char* __cdecl Ai_FormatReport(int a1, int a2, int a3, int a4, int a5, const char* s6, const char* s7, const char* s8)
{
    unsigned size = 1024;
    if (s6)
        size = strlen(s6) + 1024;
    if (s7)
        size += strlen(s7);
    if (s8)
        size += strlen(s8);
    char* buf = (char*)operator new(size);
    SYSTEMTIME t;
    char when[128];
    GetLocalTime(&t);
    sprintf(when, s_aw66c51c, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
    DWORD err = GetLastError();
    sprintf(buf, s_aw66c4f8, a2, a1, when, a4, a3, a5, a1, err, s6, s7, s8);
    return buf;
}


// ---------------------------------------------------------------------------
// Small virtual methods of the AI's world objects and route holders.

extern "C" void FUN_48ce40(); KING_ALIAS("_FUN_48ce40", "@Polyline_Nearest@24")
extern "C" void FUN_48d100(); KING_ALIAS("_FUN_48d100", "@Polyline_Advance@24")
extern "C" void FUN_48d530(); KING_ALIAS("_FUN_48d530", "@Path_StepIndex@24")
extern "C" void FUN_48c710(); KING_ALIAS("_FUN_48c710", "@Road_NearestOn@48")
extern "C" void FUN_48d710(); KING_ALIAS("_FUN_48d710", "@Points_Distance@32")
extern "C" void FUN_458940(); KING_ALIAS("_FUN_458940", "@Rec458940_Destroy@8")
extern "C" void FUN_462b30(); KING_ALIAS("_FUN_462b30", "@Exc_Destroy@4")    // exception::~exception

// [$L_401d80]
extern "C" void __cdecl Ai_Nothing401d80() {}

// [$L_401d30] static initializer of the start position (-2048, 0, 0)
extern "C" void __cdecl Ai_InitStartPos()
{
    (&g_startX68b9a0)[0] = -2048.0;
    (&g_startX68b9a0)[1] = 0;
    (&g_startX68b9a0)[2] = 0;
}

// [$L_462a70]
extern "C" BYTE* __fastcall Ai462a70_At12(BYTE* s) { return s + 12; }

// [$L_4555c0] the number of 24-byte points at +88 less one
extern "C" int __fastcall Path_SegmentCount(BYTE* s)
{
    return (F(BYTE*, s, 92) - F(BYTE*, s, 88)) / 24 - 1;
}

// [$L_4551b0] the number of 1592-byte records at +56
extern "C" int __fastcall Ai4551b0_Count(BYTE* s)
{
    return (F(BYTE*, s, 60) - F(BYTE*, s, 56)) / 1592;
}

// [$L_4554d0] (out, -, -, i): point i+1 less point i of the path at +88
extern "C" double* __fastcall Path_Segment(BYTE* s, void*, double* out, int, int, int i)
{
    double* a = (double*)(F(BYTE*, s, 88) + i * 24);
    double* b = a + 3;
    out[0] = b[0] - a[0];
    out[1] = b[1] - a[1];
    out[2] = b[2] - a[2];
    return out;
}

// [$L_432db0] cleanBeforeLaw for the object at +48 when it is still alive (12345678 at +88)
extern "C" void __fastcall Ai432db0_Update(BYTE* s)
{
    BYTE* o = F(BYTE*, s, 48);
    if (F(int, o, 88) == 305419896)
        Ai_CleanBeforeLaw(o);
}

// [$L_445b00]
extern "C" void __fastcall Ai445b00_Report(BYTE* s)
{
    AI_AddToBanner(F(double, s, 56));
    eventBanner(6, 0);
}

// [$L_455560] forwarders to the member at +56
extern "C" int __fastcall Ai455560_Forward(BYTE* s, void*, DWORD a, DWORD b, DWORD c, DWORD d)
{
    return CallThis<int>(FUN_48ce40, s + 56, a, b, c, d);
}
// [$L_455580]
extern "C" int __fastcall Ai455580_Forward(BYTE* s, void*, DWORD a, DWORD b, DWORD c, DWORD d)
{
    return CallThis<int>(FUN_48d100, s + 56, a, b, c, d);
}
// [$L_4555a0]
extern "C" int __fastcall Ai4555a0_Forward(BYTE* s, void*, DWORD a, DWORD b, DWORD c, DWORD d)
{
    return CallThis<int>(FUN_48d530, s + 56, a, b, c, d);
}
// [$L_455520]
extern "C" int __fastcall Ai455520_Forward(BYTE* s, void*, DWORD a, DWORD b, DWORD c, DWORD d, DWORD e,
                                           DWORD f, DWORD g, DWORD h, DWORD i, DWORD j)
{
    return CallThis<int>(FUN_48c710, s + 56, a, b, c, d, e, f, g, h, i, j);
}
// [$L_455620]
extern "C" int __fastcall Ai455620_Forward(BYTE* s, void*, DWORD a, DWORD b, DWORD c, DWORD d, DWORD e, DWORD f)
{
    return CallThis<int>(FUN_48d710, s + 56, a, b, c, d, e, f);
}

static BYTE* Ai_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [$L_448590]
extern "C" BYTE* __fastcall Ai448590_DeletingDestructor(BYTE* s, void*, int flags) { return Ai_DeleteAfter(s, flags, (void (*)())BadAlloc_Destroy); }
// [$L_458790]
extern "C" BYTE* __fastcall Ai458790_DeletingDestructor(BYTE* s, void*, int flags) { return Ai_DeleteAfter(s, flags, (void (*)())Rec4549a0_Destroy); }
// [$L_4587b0]
extern "C" BYTE* __fastcall Ai4587b0_DeletingDestructor(BYTE* s, void*, int flags) { return Ai_DeleteAfter(s, flags, (void (*)())Rec4587d0_Destroy); }
// [$L_458920]
extern "C" BYTE* __fastcall Ai458920_DeletingDestructor(BYTE* s, void*, int flags) { return Ai_DeleteAfter(s, flags, FUN_458940); }
// [$L_462a80]
extern "C" BYTE* __fastcall Ai462a80_DeletingDestructor(BYTE* s, void*, int flags) { return Ai_DeleteAfter(s, flags, FUN_462b30); }

KING_VAR(AsmLabel, g_aiwor68ced0, 68ced0)

// [$L_402610] static initializer: three 16-byte records at 68ced0 (-1, 0, 0, 0)
extern "C" void __cdecl AiworInit_402610()
{
    DWORD* p = (DWORD*)g_aiwor68ced0;
    for (int i = 0; i < 3; i++, p += 4) {
        p[0] = 0xffffffff;
        p[1] = p[2] = p[3] = 0;
    }
}

// ---------------------------------------------------------------------------
// Static initializers of random generators.

KING_VAR(AsmLabel, g_aiworRng68a870, 68a870)
KING_VAR(DWORD, g_aiworRng68a948, 68a948)
KING_VAR(DWORD, g_aiworRng68a94c, 68a94c)
KING_VAR(DWORD, g_aiworRng68a950, 68a950)

// the subtractive random generator's start (Knuth): 55 words at a, then the seed and the two indices
static void Aiwor_RngInit(DWORD* a, DWORD* seed, DWORD* i1, DWORD* i2)
{
    for (int i = 0; i < 55; i++)
        a[i] = 0;
    DWORD mj = 161803398, mk = 1;
    *seed = mj;
    for (unsigned i = 0; i < 1134; i += 21) {
        unsigned d = i % 55;
        mj -= mk;
        a[d] = mk;
        DWORD t = mj;
        mj = a[d];
        mk = t;
    }
    for (int k = 0; k < 4; k++)
        for (unsigned j = 0; j < 55; j++)
            a[j] -= a[(j + 31) % 55];
    *i1 = 0;
    *i2 = 31;
}

// [$L_4014f0]
extern "C" void __cdecl AiworInit_4014f0() { Aiwor_RngInit((DWORD*)g_aiworRng68a870, &g_aiworRng68a948, &g_aiworRng68a94c, &g_aiworRng68a950); }

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

typedef void (*AiFn)();
extern "C" void FUN_48e450(); KING_ALIAS("_FUN_48e450", "@Poly1392_AbsDiff01@8")
extern "C" void FUN_48dd50(); KING_ALIAS("_FUN_48dd50", "@Polyline_Near@20")
KING_VAR(DWORD, g_aiwor68baa4, 68baa4)
KING_VAR(DWORD, g_aiwor68baa8, 68baa8)
KING_VAR(AsmLabel, g_aiwor68b9c8, 68b9c8)
KING_VAR(DWORD, g_aiwor68d108, 68d108)
KING_VAR(DWORD, g_aiwor68d10c, 68d10c)
KING_VAR(double, g_aiwor64b670, 64b670)
KING_VAR(double, g_aiwor64b678, 64b678)
KING_VAR(AsmLabel, g_aiwor68cc28, 68cc28)

// [$L_455440] (out, t, i): the point at t between points i and i+1 of the path (+88)
extern "C" double* __fastcall Path_Lerp(BYTE* s, void*, double* out, double t, int i)
{
    double* a = (double*)(F(BYTE*, s, 88) + i * 24);
    double* b = a + 3;
    double n0 = b[0] * t, n1 = b[1] * t, n2 = b[2] * t;
    double u = g_aiwor64b670 - t;
    double c0 = a[0] * u, c1 = a[1] * u, c2 = a[2] * u;
    out[0] = c0 + n0;
    out[1] = c1 + n1;
    out[2] = c2 + n2;
    return out;
}

// [$L_435690] runs the task (+40), then its next time 600..899 s ahead from the generator at 68b9c8
extern "C" void __fastcall Task_RunAndReschedule(BYTE* s)
{
    CallThis(F(AiFn, s, 40), s);
    DWORD* a = (DWORD*)g_aiwor68b9c8;
    DWORD i = (g_aiwor68baa4 + 1) % 55;
    g_aiwor68baa4 = i;
    DWORD j = (g_aiwor68baa8 + 1) % 55;
    g_aiwor68baa8 = j;
    a[i] -= a[j];
    F(double, s, 24) = (double)(a[g_aiwor68baa4] % 300 + 600);
    g_aiwor68d108 = F(DWORD, s, 16);
    g_aiwor68d10c = F(DWORD, s, 20);
}

// [$L_4553c0] the sum of FUN_48e450 of the 1592-byte records at +56
extern "C" double __fastcall Ai4553c0_Sum(BYTE* s)
{
    double t = g_aiwor64b678;
    for (unsigned i = 0; i < (unsigned)((F(BYTE*, s, 60) - F(BYTE*, s, 56)) / 1592); i++)
        t = CallThis<double>(FUN_48e450, F(BYTE*, s, 56) + i * 1592) + t;
    return t;
}

// [$L_454e80] the last record (+56) for which FUN_48dd50 (a, b, c) holds, its index to *out
extern "C" int __fastcall Ai454e80_Find(BYTE* s, void*, int a, int b, int c, int* out)
{
    int found = 0;
    for (unsigned i = 0; i < (unsigned)((F(BYTE*, s, 60) - F(BYTE*, s, 56)) / 1592); i++)
        if (CallThis<int>(FUN_48dd50, F(BYTE*, s, 56) + i * 1592, a, b, c)) {
            found = 1;
            *out = i;
        }
    return found;
}

// [$L_428120] static initializer: two records of the doubles (-12, 0, 0, 10, 0, 0)
extern "C" void __cdecl AiworInit_428120()
{
    static const DWORD v[24] = {0, 0xc0280000, 0, 0, 0, 0, 0, 0x40280000, 0, 0, 0, 0,
                                0, 0, 0, 0xc0280000, 0, 0, 0, 0, 0, 0x40280000, 0, 0};
    DWORD* d = (DWORD*)g_aiwor68cc28;
    for (int k = 0; k < 24; k++)
        d[k] = v[k];
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

extern "C" void FUN_469a20(); KING_ALIAS("_FUN_469a20", "_Rec1592_Swap")
extern "C" void FUN_48e190(); KING_ALIAS("_FUN_48e190", "@Curve_Reverse@4")
KING_VAR(AsmLabel, g_aiworRng68ccc8, 68ccc8)
KING_VAR(DWORD, g_aiworRng68cda0, 68cda0)
KING_VAR(DWORD, g_aiworRng68cda4, 68cda4)
KING_VAR(DWORD, g_aiworRng68cda8, 68cda8)
KING_VAR(DWORD, g_aiworRng68baa0, 68baa0)

// [$L_401210] static initializers of random generators
extern "C" void __cdecl AiworInit_401210() { Aiwor_RngInit((DWORD*)g_aiworRng68ccc8, &g_aiworRng68cda0, &g_aiworRng68cda4, &g_aiworRng68cda8); }
// [$L_401440]
extern "C" void __cdecl AiworInit_401440() { Aiwor_RngInit((DWORD*)g_aiwor68b9c8, &g_aiworRng68baa0, &g_aiwor68baa4, &g_aiwor68baa8); }

// [$L_4551d0] reverses the path: flags 4 and 8 swap, the 1592-byte records (+56) in reverse order, each turned (FUN_48e190)
extern "C" void __fastcall Path_Reverse(BYTE* s)
{
    unsigned f = F(unsigned, s, 8);
    unsigned b4 = f & 4, b8 = f & 8;
    F(unsigned, s, 8) = f & ~0xcu;
    if (b4)
        F(unsigned, s, 8) |= 8;
    if (b8)
        F(unsigned, s, 8) |= 4;
    BYTE* lo = F(BYTE*, s, 56);
    BYTE* hi = F(BYTE*, s, 60);
    while (lo < hi) {
        hi -= 1592;
        CallC(FUN_469a20, lo, hi, 0);
        lo += 1592;
    }
    for (unsigned i = 0; i < (unsigned)((F(BYTE*, s, 60) - F(BYTE*, s, 56)) / 1592); i++)
        CallThis(FUN_48e190, F(BYTE*, s, 56) + i * 1592);
}

// ---------------------------------------------------------------------------
// More virtual methods.

extern "C" void FUN_48e100(); KING_ALIAS("_FUN_48e100", "_Bounded_Step@12")
extern "C" void FUN_48df80(); KING_ALIAS("_FUN_48df80", "@Curve_Advance@20")
extern "C" void FUN_48e430(); KING_ALIAS("_FUN_48e430", "@Poly1392_AbsDiff@24")

// the path's 1592-byte record i (+56) and the record count
static BYTE* Path_Rec(BYTE* s, int i) { return F(BYTE*, s, 56) + i * 1592; }
static unsigned Path_Count(BYTE* s) { return (unsigned)((F(BYTE*, s, 60) - F(BYTE*, s, 56)) / 1592); }

// [$L_455060] moves along the path by t from record *idx (forwards for t > 0, backwards for t < 0)
extern "C" int __fastcall Path_Advance(BYTE* s, void*, double* out, double t, int* idx)
{
    int r = 0;
    if (t > g_aiwor64b678) {
        if ((unsigned)*idx >= Path_Count(s))
            return r;
        do {
            r = CallThis<int>(FUN_48e100, Path_Rec(s, *idx), out, t);
            if (!r || (unsigned)*idx >= Path_Count(s) - 1)
                return r;
            (*idx)++;
            ((DWORD*)out)[0] = 0;
            ((DWORD*)out)[1] = 0;
        } while ((unsigned)*idx < Path_Count(s));
        return r;
    }
    if (t >= g_aiwor64b678)
        return r;
    if (*idx < 0)
        return r;
    do {
        r = CallThis<int>(FUN_48e100, Path_Rec(s, *idx), out, t);
        if (!r || *idx == 0)
            return r;
        (*idx)--;
        ((DWORD*)out)[0] = 0;
        ((DWORD*)out)[1] = 0x3ff00000;
    } while (*idx >= 0);
    return r;
}

// [$L_454f10] the same with t at *tp and an extra argument for FUN_48df80
extern "C" int __fastcall Path_Advance2(BYTE* s, void*, double* out, double* tp, int c, int* idx)
{
    int r = 0;
    if (*tp > g_aiwor64b678) {
        if ((unsigned)*idx >= Path_Count(s))
            return r;
        do {
            r = CallThis<int>(FUN_48df80, Path_Rec(s, *idx), out, tp, c);
            if (!r || (unsigned)*idx >= Path_Count(s) - 1)
                return r;
            (*idx)++;
            ((DWORD*)out)[0] = 0;
            ((DWORD*)out)[1] = 0;
        } while ((unsigned)*idx < Path_Count(s));
        return r;
    }
    if (*tp >= g_aiwor64b678)
        return r;
    if (*idx < 0)
        return r;
    do {
        r = CallThis<int>(FUN_48df80, Path_Rec(s, *idx), out, tp, c);
        if (!r || *idx == 0)
            return r;
        (*idx)--;
        ((DWORD*)out)[0] = 0;
        ((DWORD*)out)[1] = 0x3ff00000;
    } while (*idx >= 0);
    return r;
}

// [$L_455280] the length of the path from (t1 in record i1) to (t2 in record i2)
extern "C" double __fastcall Path_Length(BYTE* s, void*, double t1, int i1, double t2, int i2)
{
    if ((unsigned)i1 == Path_Count(s)) {
        t1 = t1 + g_aiwor64b670;
        i1--;
    }
    if ((unsigned)i2 == Path_Count(s)) {
        t2 = t2 + g_aiwor64b670;
        i2--;
    }
    if (i1 == i2)
        return CallThis<double>(FUN_48e430, Path_Rec(s, i1), t1, t2);
    double d = CallThis<double>(FUN_48e430, Path_Rec(s, i1), t1, 1.0);
    for (int k = i1 + 1; k < i2; k++)
        d = CallThis<double>(FUN_48e450, Path_Rec(s, k)) + d;
    return CallThis<double>(FUN_48e430, Path_Rec(s, i2), 0.0, t2) + d;
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

extern "C" void FUN_602ca0(); KING_ALIAS("_FUN_602ca0", "@MsgBuf_FreeStorage@8")
// [FUN_460d10] frees the storage of the message buffer p
extern "C" int __cdecl MsgBuf_FreeThunk(BYTE* p)
{
    return CallThis<int>(FUN_602ca0, p);
}

// ---------------------------------------------------------------------------
// Small methods and static helpers: guarded static construction, releases, frees.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void FUN_447970(); KING_ALIAS("_FUN_447970", "@Vec4_FreeStorage@8")
KING_VAR(BYTE, g_q49_68a84c, 68a84c)
KING_VAR(BYTE, g_q49_68a84d, 68a84d)
KING_VAR(AsmLabel, g_q49_68cff0, 68cff0)
KING_VAR(AsmLabel, g_q49_68d008, 68d008)
KING_VAR(AsmLabel, g_q49_68cf10, 68cf10)
KING_VAR(AsmLabel, g_q49_68ceb0, 68ceb0)
extern "C" void __fastcall RopeHash_Destroy(BYTE* s);
// constructs the static at o once, by the guard bit
static void AiStatic_ConstructOnce(BYTE& guard, BYTE bit, void* o)
{
    if (guard & bit)
        return;
    guard |= bit;
    RopeHash_Destroy((BYTE*)o);
}
// [$L_4028d0] constructs the static 68cff0 once
extern "C" void __cdecl AiStatic68cff0_Init(void)
{
    AiStatic_ConstructOnce(g_q49_68a84c, 1, (void*)g_q49_68cff0);
}
// [$L_402b70] constructs the static 68d008 once
extern "C" void __cdecl AiStatic68d008_Init(void)
{
    AiStatic_ConstructOnce(g_q49_68a84d, 1, (void*)g_q49_68d008);
}
// [$L_402bc0] constructs the static 68cf10 once
extern "C" void __cdecl AiStatic68cf10_Init(void)
{
    AiStatic_ConstructOnce(g_q49_68a84d, 2, (void*)g_q49_68cf10);
}
static void __cdecl AiStatic68ceb0_Free(void)
{
    CallThis(FUN_447970, (void*)g_q49_68ceb0);
}
// [$L_402280] frees the storage of the static 68ceb0 at exit
extern "C" int __cdecl AiStatic68ceb0_AtExit(void)
{
    return atexit(AiStatic68ceb0_Free);
}

// ---------------------------------------------------------------------------
// Small methods: message filters, record copies, frees and deleting destructors.

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
extern "C" void FUN_401010(); KING_ALIAS("_FUN_401010", "_Ai_Report")
KING_VAR(AsmLabel, s_q50_66c7b0, 66c7b0)
KING_VAR(AsmLabel, s_q50_66c558, 66c558)
// [$L_462ca0] returns the vector storage (+0 .. +8) to the pool
extern "C" void __fastcall AiVec_FreeStorage(BYTE* s, void*)
{
    BYTE* p = F(BYTE*, s, 0);
    int n = F(BYTE*, s, 8) - p;
    if (p && n)
        CallC(FUN_44b4a0, p, n);
}
// [$L_401310] reports (kind, a, b) at line 130 unless kind is 5
extern "C" void __cdecl Ai_ReportUnless5(int kind, int a, int b)
{
    if (kind != 5)
        CallC(FUN_401010, kind, a, (const char*)s_q50_66c7b0, (const char*)s_q50_66c558, 130, b,
              &g_empty68d1c0, &g_empty68d1c0);
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
// returns the storage of a vector (+0 .. +8) of size-byte elements to the pool
static void PoolVec_Free_W(BYTE* s, int size)
{
    BYTE* p = F(BYTE*, s, 0);
    int n = (F(BYTE*, s, 8) - p) / size;
    if (n)
        CallC(FUN_44b4a0, p, n * size);
}
// [$L_44abb0] frees the storage of a vector of 144-byte records
extern "C" void __fastcall Vec144_FreeStorage(BYTE* s, void*)
{
    PoolVec_Free_W(s, 144);
}
// [$L_44abe0] frees the storage of a vector of 36-byte records
extern "C" void __fastcall Vec36_FreeStorage(BYTE* s, void*)
{
    PoolVec_Free_W(s, 36);
}
// [$L_44a6c0] frees the storage of a vector of 104-byte records
extern "C" void __fastcall Vec104_FreeStorage(BYTE* s, void*)
{
    PoolVec_Free_W(s, 104);
}

// ---------------------------------------------------------------------------
// Small methods: pool slots, list teardown, loader thread, frame shutdown, message dispatch.

extern "C" void __cdecl free(void*);
extern "C" void FUN_4731e0(); KING_ALIAS("_FUN_4731e0", "@Player_Destroy@4")
extern "C" void __fastcall Points_Reverse(BYTE* s);
// [$L_4485d0] frees a list (sentinel at +0): each node's +8 destroyed, then the sentinel
extern "C" void __fastcall List4485d0_Destroy(BYTE* s, void*)
{
    for (BYTE* n = F(BYTE*, F(BYTE*, s, 0), 0); n != F(BYTE*, s, 0);) {
        BYTE* e = n;
        n = F(BYTE*, n, 0);
        CallThis(FUN_4731e0, e + 8);
        free(e);
    }
    F(BYTE*, F(BYTE*, s, 0), 0) = F(BYTE*, s, 0);
    F(BYTE*, F(BYTE*, s, 0), 4) = F(BYTE*, s, 0);
    free(F(BYTE*, s, 0));
}
// [$L_44b050] frees the storage of a vector of 580-byte records
extern "C" void __fastcall Vec580_FreeStorage(BYTE* s, void*)
{
    PoolVec_Free_W(s, 580);
}
// [$L_4555e0] swaps the flags 4 and 8 (+8), then 48d600 on the +56 part
extern "C" int __fastcall Ai4555e0_SwapSides(BYTE* s, void*)
{
    DWORD f = F(DWORD, s, 8);
    DWORD n = f & ~12u;
    if (f & 4)
        n |= 8;
    if (f & 8)
        n |= 4;
    F(DWORD, s, 8) = n;
    return CallThis<int>((void (*)())Points_Reverse, s + 56);
}

// ---------------------------------------------------------------------------
// Copying the AI's exception types.

extern "C" void AiW157_ExcCopy(); KING_ALIAS("_AiW157_ExcCopy", "@Exc_CopyConstruct@12")
KING_VAR(AsmLabel, g_q157_64b974, 64b974)
KING_VAR(AsmLabel, g_q157_64b96c, 64b96c)
KING_VAR(AsmLabel, g_q157_64b97c, 64b97c)
// copies exception o into s (the base, then its 256-byte message) with vtable vt
static BYTE* AiW157_Copy(BYTE* s, const BYTE* o, void* vt)
{
    CallThis(AiW157_ExcCopy, s, o);
    memcpy(s + 12, o + 12, 256);
    F(void*, s, 0) = vt;
    return s;
}
// [$L_462aa0] copy constructor of the 64b974 exception
extern "C" BYTE* __fastcall AiExc462aa0_Copy(BYTE* s, void*, const BYTE* o)
{
    return AiW157_Copy(s, o, (BYTE*)g_q157_64b974);
}
// [$L_462ad0] copy constructor of the 64b96c exception
extern "C" BYTE* __fastcall AiExc462ad0_Copy(BYTE* s, void*, const BYTE* o)
{
    return AiW157_Copy(s, o, (BYTE*)g_q157_64b96c);
}
// [$L_462b00] copy constructor of the 64b97c exception
extern "C" BYTE* __fastcall AiExc462b00_Copy(BYTE* s, void*, const BYTE* o)
{
    return AiW157_Copy(s, o, (BYTE*)g_q157_64b97c);
}

// the point at t of the 3D polynomial at c (+576 coefficient count, highest first), by Horner's rule
static double* Aiwor190_Poly(double* out, const BYTE* c, double t)
{
    const double* k = (const double*)c;
    double x = k[0], y = k[1], z = k[2];
    unsigned n = F(int, c, 576) * 24 / 24;
    for (unsigned i = 1; i < n; i++) {
        x = x * t + k[i * 3];
        y = y * t + k[i * 3 + 1];
        z = z * t + k[i * 3 + 2];
    }
    out[0] = x;
    out[1] = y;
    out[2] = z;
    return out;
}
// [$L_454b00] the point at t of route segment i's (+56, 1592 bytes each) first curve
extern "C" double* __fastcall Route_CurvePoint(BYTE* s, void*, double* out, double t, int i)
{
    return Aiwor190_Poly(out, F(BYTE*, s, 56) + i * 1592, t);
}
// [$L_454bd0] the same for its second curve (+616)
extern "C" double* __fastcall Route_CurvePoint2(BYTE* s, void*, double* out, double t, int i)
{
    return Aiwor190_Poly(out, F(BYTE*, s, 56) + i * 1592 + 616, t);
}

extern "C" void FUN_42b2a0(); KING_ALIAS("_FUN_42b2a0", "_AI_FirstActive")
extern "C" void FUN_42b2d0(); KING_ALIAS("_FUN_42b2d0", "_AI_FarFrom500")
extern "C" void FUN_42b170(); KING_ALIAS("_FUN_42b170", "_Rec_ActivateOnly")
KING_VAR(double, g_q191_64b860, 64b860)
// [$L_42b3a0] when the active record (42b2a0) is far enough, up to five random records of 68cc18 are tried: the
// first one that is also far enough and lies further than 64b860 from the active one (or with none active) is
// activated instead
extern "C" void __cdecl AI_SwitchToRandom()
{
    if ((void**)g_68cc18 == (void**)g_68cc1c)
        return;
    BYTE* cur = CallC<BYTE*>(FUN_42b2a0);
    if (!CallC<bool>(FUN_42b2d0, cur))
        return;
    for (int k = 0; k < 5; k++) {
        unsigned r = AiRandom();
        BYTE* e = (BYTE*)((void**)g_68cc18)[r % (unsigned)(((void**)g_68cc1c - (void**)g_68cc18))];
        if (!CallC<bool>(FUN_42b2d0, e))
            continue;
        if (cur) {
            double dx = F(double, e, 64) - F(double, cur, 64);
            double dy = F(double, e, 72) - F(double, cur, 72);
            double dz = F(double, e, 80) - F(double, cur, 80);
            if (!(x87_sqrt((dy * dy + dz * dz) + dx * dx) > g_q191_64b860))
                continue;
        }
        CallC(FUN_42b170, e);
        return;
    }
}

// ---------------------------------------------------------------------------
// Static initializers: construct a global, register its teardown at exit.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
KING_VAR(char, g_q197_68a850, 68a850)
KING_VAR(char, g_q197_68a860, 68a860)
KING_VAR(char, g_q197_68a958, 68a958)
KING_VAR(char, g_q197_68a970, 68a970)
KING_VAR(char, g_q197_68a980, 68a980)
KING_VAR(char, g_q197_68b990, 68b990)
KING_VAR(char, g_q197_68b9b8, 68b9b8)
KING_VAR(char, g_q197_68bac8, 68bac8)
KING_VAR(char, g_q197_68bb58, 68bb58)
KING_VAR(char, g_q197_68cbf8, 68cbf8)
KING_VAR(char, g_q197_68cec0, 68cec0)
KING_VAR(char, g_q197_68cf38, 68cf38)
KING_VAR(char, g_q197_68cf58, 68cf58)
KING_VAR(char, g_q197_68cf60, 68cf60)
KING_VAR(char, g_q197_68cf6c, 68cf6c)
KING_VAR(char, g_q197_68cfd8, 68cfd8)
KING_VAR(char, g_q197_68d0a8, 68d0a8)
KING_VAR(char, g_q197_68d0b8, 68d0b8)
KING_VAR(char, g_q197_68d0c8, 68d0c8)
KING_VAR(char, g_q197_68d0d8, 68d0d8)
extern "C" void q197_FUN_4012e0(); KING_ALIAS("_q197_FUN_4012e0", "_AI_Init68bb58")
extern "C" void q197_FUN_401360(); KING_ALIAS("_q197_FUN_401360", "_AI_Init68cf28")
extern "C" void q197_FUN_4013b0(); KING_ALIAS("_q197_FUN_4013b0", "_AI_Init68a840")
extern "C" void q197_FUN_401970(); KING_ALIAS("_q197_FUN_401970", "_AI_Destroy68b998")
extern "C" void q197_FUN_401db0(); KING_ALIAS("_q197_FUN_401db0", "_AI_Init68bac8")
extern "C" void q197_FUN_401e00(); KING_ALIAS("_q197_FUN_401e00", "_AI_Init68ccb8")
extern "C" void q197_FUN_401ef0(); KING_ALIAS("_q197_FUN_401ef0", "_AI_Init68cf38")
extern "C" void q197_FUN_401f40(); KING_ALIAS("_q197_FUN_401f40", "_AI_Init68b990")
extern "C" void q197_FUN_401f90(); KING_ALIAS("_q197_FUN_401f90", "_AI_Init68d0d8")
extern "C" void q197_FUN_401fe0(); KING_ALIAS("_q197_FUN_401fe0", "_AI_Init68d0c8")
extern "C" void q197_FUN_402030(); KING_ALIAS("_q197_FUN_402030", "_AI_Init68cf6c")
extern "C" void q197_FUN_402080(); KING_ALIAS("_q197_FUN_402080", "_AI_Init68cec0")
extern "C" void q197_FUN_4020d0(); KING_ALIAS("_q197_FUN_4020d0", "_AI_Init68cc98")
extern "C" void q197_FUN_402170(); KING_ALIAS("_q197_FUN_402170", "_AI_Init68cfd8")
extern "C" void q197_FUN_4021c0(); KING_ALIAS("_q197_FUN_4021c0", "_AI_Init68d0b8")
extern "C" void q197_FUN_402210(); KING_ALIAS("_q197_FUN_402210", "_AI_Init68cc18")
extern "C" void q197_FUN_4022b0(); KING_ALIAS("_q197_FUN_4022b0", "_AI_Init68a970")
extern "C" void q197_FUN_402300(); KING_ALIAS("_q197_FUN_402300", "_AI_Init68a850")
extern "C" void q197_FUN_402350(); KING_ALIAS("_q197_FUN_402350", "_AI_Init68b9b8")
extern "C" void q197_FUN_4023a0(); KING_ALIAS("_q197_FUN_4023a0", "_AI_Init68a958")
extern "C" void q197_FUN_4023f0(); KING_ALIAS("_q197_FUN_4023f0", "_AI_Init68cca8")
extern "C" void q197_FUN_4024b0(); KING_ALIAS("_q197_FUN_4024b0", "_AI_Init68cbf8")
extern "C" void q197_FUN_4025d0(); KING_ALIAS("_q197_FUN_4025d0", "_AI_Init68d0a8")
extern "C" void q197_FUN_402660(); KING_ALIAS("_q197_FUN_402660", "_AI_Init68a980")
extern "C" void q197_FUN_4026b0(); KING_ALIAS("_q197_FUN_4026b0", "_AI_Init68cf60")
extern "C" void q197_FUN_402700(); KING_ALIAS("_q197_FUN_402700", "_AI_Init68cc88")
extern "C" void q197_FUN_4028b0(); KING_ALIAS("_q197_FUN_4028b0", "_AI_Destroy68cff0")
extern "C" void q197_FUN_402b50(); KING_ALIAS("_q197_FUN_402b50", "_AI_Destroy68d008")
extern "C" void q197_FUN_402ba0(); KING_ALIAS("_q197_FUN_402ba0", "_AI_Destroy68cf10")
extern "C" void q197_FUN_404420(); KING_ALIAS("_q197_FUN_404420", "@VecB_FreeStorage@8")
extern "C" void q197_FUN_4056f0(); KING_ALIAS("_q197_FUN_4056f0", "_AI_Init68cf58")
extern "C" void q197_FUN_415260(); KING_ALIAS("_q197_FUN_415260", "_AI_Init68a860")
extern "C" void q197_FUN_447570(); KING_ALIAS("_q197_FUN_447570", "@StrRefVec_Destroy@8")
extern "C" void q197_FUN_447970(); KING_ALIAS("_q197_FUN_447970", "@Vec4_FreeStorage@8")
extern "C" void q197_L_401390(); KING_ALIAS("_q197_L_401390", "_Static68cf28_Destroy")
extern "C" void q197_L_4013e0(); KING_ALIAS("_q197_L_4013e0", "_Ai_Exit4013e0")
extern "C" void q197_L_401990(); KING_ALIAS("_q197_L_401990", "_Ai_Exit401990")
extern "C" void q197_L_401e30(); KING_ALIAS("_q197_L_401e30", "_Ai_Exit401e30")
extern "C" void q197_L_402100(); KING_ALIAS("_q197_L_402100", "_Ai_Exit402100")
extern "C" void q197_L_402240(); KING_ALIAS("_q197_L_402240", "_Ai_Exit402240")
extern "C" void q197_L_402430(); KING_ALIAS("_q197_L_402430", "_Ai_Exit402430")
extern "C" void q197_L_402730(); KING_ALIAS("_q197_L_402730", "_Ai_Exit402730")
extern "C" void q197_L_4028d0(); KING_ALIAS("_q197_L_4028d0", "_AiStatic68cff0_Init")
extern "C" void q197_L_402b70(); KING_ALIAS("_q197_L_402b70", "_AiStatic68d008_Init")
extern "C" void q197_L_402bc0(); KING_ALIAS("_q197_L_402bc0", "_AiStatic68cf10_Init")
extern "C" void q197_L_4485d0(); KING_ALIAS("_q197_L_4485d0", "@List4485d0_Destroy@8")
extern "C" void q197_L_449130(); KING_ALIAS("_q197_L_449130", "@Vec56b_Destroy@4")
extern "C" void q197_L_4494c0(); KING_ALIAS("_q197_L_4494c0", "@VecVec4_Destroy@4")
extern "C" void q197_L_449740(); KING_ALIAS("_q197_L_449740", "@List12_Destroy@4")
extern "C" void q197_L_4aee50(); KING_ALIAS("_q197_L_4aee50", "@Sch_Destruct@8")
extern "C" void q197_L_4af6b0(); KING_ALIAS("_q197_L_4af6b0", "@List16_Destroy@4")
// [$L_401300] destroys the static object 68bb58 at exit ($L_4aee50)
extern "C" void __cdecl Static68bb58_Destroy(void) { CallThis(q197_L_4aee50, &g_q197_68bb58); }
// [$L_4012f0] registers the teardown at exit
extern "C" int __cdecl Static68bb58_AtExit(void) { return atexit((void(__cdecl*)(void))Static68bb58_Destroy); }
// [$L_4012d0] the static initializer: FUN_4012e0, then the teardown at exit
extern "C" int __cdecl Static68bb58_Init(void)
{
    CallC(q197_FUN_4012e0);
    return Static68bb58_AtExit();
}
// [$L_401380] registers the teardown at exit
extern "C" int __cdecl Static401350_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_401390); }
// [$L_401350] the static initializer: FUN_401360, then the teardown at exit
extern "C" int __cdecl Static401350_Init(void)
{
    CallC(q197_FUN_401360);
    return Static401350_AtExit();
}
// [$L_4013d0] registers the teardown at exit
extern "C" int __cdecl Static4013a0_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_4013e0); }
// [$L_4013a0] the static initializer: FUN_4013b0, then the teardown at exit
extern "C" int __cdecl Static4013a0_Init(void)
{
    CallC(q197_FUN_4013b0);
    return Static4013a0_AtExit();
}
// [$L_401980] registers the teardown at exit
extern "C" int __cdecl Static401960_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_401990); }
// [$L_401960] the static initializer: FUN_401970, then the teardown at exit
extern "C" int __cdecl Static401960_Init(void)
{
    CallC(q197_FUN_401970);
    return Static401960_AtExit();
}
// [$L_401de0] destroys the static object 68bac8 at exit (FUN_404420)
extern "C" void __cdecl Static68bac8_Destroy(void) { CallThis(q197_FUN_404420, &g_q197_68bac8); }
// [$L_401dd0] registers the teardown at exit
extern "C" int __cdecl Static68bac8_AtExit(void) { return atexit((void(__cdecl*)(void))Static68bac8_Destroy); }
// [$L_401da0] the static initializer: FUN_401db0, then the teardown at exit
extern "C" int __cdecl Static68bac8_Init(void)
{
    CallC(q197_FUN_401db0);
    return Static68bac8_AtExit();
}
// [$L_401e20] registers the teardown at exit
extern "C" int __cdecl Static401df0_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_401e30); }
// [$L_401df0] the static initializer: FUN_401e00, then the teardown at exit
extern "C" int __cdecl Static401df0_Init(void)
{
    CallC(q197_FUN_401e00);
    return Static401df0_AtExit();
}
// [$L_401f20] destroys the static object 68cf38 at exit (FUN_447970)
extern "C" void __cdecl Static68cf38_Destroy(void) { CallThis(q197_FUN_447970, &g_q197_68cf38); }
// [$L_401f10] registers the teardown at exit
extern "C" int __cdecl Static68cf38_AtExit(void) { return atexit((void(__cdecl*)(void))Static68cf38_Destroy); }
// [$L_401ee0] the static initializer: FUN_401ef0, then the teardown at exit
extern "C" int __cdecl Static68cf38_Init(void)
{
    CallC(q197_FUN_401ef0);
    return Static68cf38_AtExit();
}
// [$L_401f70] destroys the static object 68b990 at exit ($L_4485d0)
extern "C" void __cdecl Static68b990_Destroy(void) { CallThis(q197_L_4485d0, &g_q197_68b990); }
// [$L_401f60] registers the teardown at exit
extern "C" int __cdecl Static68b990_AtExit(void) { return atexit((void(__cdecl*)(void))Static68b990_Destroy); }
// [$L_401f30] the static initializer: FUN_401f40, then the teardown at exit
extern "C" int __cdecl Static68b990_Init(void)
{
    CallC(q197_FUN_401f40);
    return Static68b990_AtExit();
}
// [$L_401fc0] destroys the static object 68d0d8 at exit (FUN_447970)
extern "C" void __cdecl Static68d0d8_Destroy(void) { CallThis(q197_FUN_447970, &g_q197_68d0d8); }
// [$L_401fb0] registers the teardown at exit
extern "C" int __cdecl Static68d0d8_AtExit(void) { return atexit((void(__cdecl*)(void))Static68d0d8_Destroy); }
// [$L_401f80] the static initializer: FUN_401f90, then the teardown at exit
extern "C" int __cdecl Static68d0d8_Init(void)
{
    CallC(q197_FUN_401f90);
    return Static68d0d8_AtExit();
}
// [$L_402010] destroys the static object 68d0c8 at exit (FUN_447970)
extern "C" void __cdecl Static68d0c8_Destroy(void) { CallThis(q197_FUN_447970, &g_q197_68d0c8); }
// [$L_402000] registers the teardown at exit
extern "C" int __cdecl Static68d0c8_AtExit(void) { return atexit((void(__cdecl*)(void))Static68d0c8_Destroy); }
// [$L_401fd0] the static initializer: FUN_401fe0, then the teardown at exit
extern "C" int __cdecl Static68d0c8_Init(void)
{
    CallC(q197_FUN_401fe0);
    return Static68d0c8_AtExit();
}
// [$L_402060] destroys the static object 68cf6c at exit ($L_4af6b0)
extern "C" void __cdecl Static68cf6c_Destroy(void) { CallThis(q197_L_4af6b0, &g_q197_68cf6c); }
// [$L_402050] registers the teardown at exit
extern "C" int __cdecl Static68cf6c_AtExit(void) { return atexit((void(__cdecl*)(void))Static68cf6c_Destroy); }
// [$L_402020] the static initializer: FUN_402030, then the teardown at exit
extern "C" int __cdecl Static68cf6c_Init(void)
{
    CallC(q197_FUN_402030);
    return Static68cf6c_AtExit();
}
// [$L_4020b0] destroys the static object 68cec0 at exit (FUN_447970)
extern "C" void __cdecl Static68cec0_Destroy(void) { CallThis(q197_FUN_447970, &g_q197_68cec0); }
// [$L_4020a0] registers the teardown at exit
extern "C" int __cdecl Static68cec0_AtExit(void) { return atexit((void(__cdecl*)(void))Static68cec0_Destroy); }
// [$L_402070] the static initializer: FUN_402080, then the teardown at exit
extern "C" int __cdecl Static68cec0_Init(void)
{
    CallC(q197_FUN_402080);
    return Static68cec0_AtExit();
}
// [$L_4020f0] registers the teardown at exit
extern "C" int __cdecl Static4020c0_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_402100); }
// [$L_4020c0] the static initializer: FUN_4020d0, then the teardown at exit
extern "C" int __cdecl Static4020c0_Init(void)
{
    CallC(q197_FUN_4020d0);
    return Static4020c0_AtExit();
}
// [$L_4021a0] destroys the static object 68cfd8 at exit (FUN_447970)
extern "C" void __cdecl Static68cfd8_Destroy(void) { CallThis(q197_FUN_447970, &g_q197_68cfd8); }
// [$L_402190] registers the teardown at exit
extern "C" int __cdecl Static68cfd8_AtExit(void) { return atexit((void(__cdecl*)(void))Static68cfd8_Destroy); }
// [$L_402160] the static initializer: FUN_402170, then the teardown at exit
extern "C" int __cdecl Static68cfd8_Init(void)
{
    CallC(q197_FUN_402170);
    return Static68cfd8_AtExit();
}
// [$L_4021f0] destroys the static object 68d0b8 at exit (FUN_447970)
extern "C" void __cdecl Static68d0b8_Destroy(void) { CallThis(q197_FUN_447970, &g_q197_68d0b8); }
// [$L_4021e0] registers the teardown at exit
extern "C" int __cdecl Static68d0b8_AtExit(void) { return atexit((void(__cdecl*)(void))Static68d0b8_Destroy); }
// [$L_4021b0] the static initializer: FUN_4021c0, then the teardown at exit
extern "C" int __cdecl Static68d0b8_Init(void)
{
    CallC(q197_FUN_4021c0);
    return Static68d0b8_AtExit();
}
// [$L_402230] registers the teardown at exit
extern "C" int __cdecl Static402200_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_402240); }
// [$L_402200] the static initializer: FUN_402210, then the teardown at exit
extern "C" int __cdecl Static402200_Init(void)
{
    CallC(q197_FUN_402210);
    return Static402200_AtExit();
}
// [$L_4022e0] destroys the static object 68a970 at exit (FUN_447970)
extern "C" void __cdecl Static68a970_Destroy(void) { CallThis(q197_FUN_447970, &g_q197_68a970); }
// [$L_4022d0] registers the teardown at exit
extern "C" int __cdecl Static68a970_AtExit(void) { return atexit((void(__cdecl*)(void))Static68a970_Destroy); }
// [$L_4022a0] the static initializer: FUN_4022b0, then the teardown at exit
extern "C" int __cdecl Static68a970_Init(void)
{
    CallC(q197_FUN_4022b0);
    return Static68a970_AtExit();
}
// [$L_402330] destroys the static object 68a850 at exit (FUN_447570)
extern "C" void __cdecl Static68a850_Destroy(void) { CallThis(q197_FUN_447570, &g_q197_68a850); }
// [$L_402320] registers the teardown at exit
extern "C" int __cdecl Static68a850_AtExit(void) { return atexit((void(__cdecl*)(void))Static68a850_Destroy); }
// [$L_4022f0] the static initializer: FUN_402300, then the teardown at exit
extern "C" int __cdecl Static68a850_Init(void)
{
    CallC(q197_FUN_402300);
    return Static68a850_AtExit();
}
// [$L_402380] destroys the static object 68b9b8 at exit (FUN_447570)
extern "C" void __cdecl Static68b9b8_Destroy(void) { CallThis(q197_FUN_447570, &g_q197_68b9b8); }
// [$L_402370] registers the teardown at exit
extern "C" int __cdecl Static68b9b8_AtExit(void) { return atexit((void(__cdecl*)(void))Static68b9b8_Destroy); }
// [$L_402340] the static initializer: FUN_402350, then the teardown at exit
extern "C" int __cdecl Static68b9b8_Init(void)
{
    CallC(q197_FUN_402350);
    return Static68b9b8_AtExit();
}
// [$L_4023d0] destroys the static object 68a958 at exit ($L_449130)
extern "C" void __cdecl Static68a958_Destroy(void) { CallThis(q197_L_449130, &g_q197_68a958); }
// [$L_4023c0] registers the teardown at exit
extern "C" int __cdecl Static68a958_AtExit(void) { return atexit((void(__cdecl*)(void))Static68a958_Destroy); }
// [$L_402390] the static initializer: FUN_4023a0, then the teardown at exit
extern "C" int __cdecl Static68a958_Init(void)
{
    CallC(q197_FUN_4023a0);
    return Static68a958_AtExit();
}
// [$L_402420] registers the teardown at exit
extern "C" int __cdecl Static4023e0_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_402430); }
// [$L_4023e0] the static initializer: FUN_4023f0, then the teardown at exit
extern "C" int __cdecl Static4023e0_Init(void)
{
    CallC(q197_FUN_4023f0);
    return Static4023e0_AtExit();
}
// [$L_4025b0] destroys the static object 68cbf8 at exit ($L_4494c0)
extern "C" void __cdecl Static68cbf8_Destroy(void) { CallThis(q197_L_4494c0, &g_q197_68cbf8); }
// [$L_4025a0] registers the teardown at exit
extern "C" int __cdecl Static68cbf8_AtExit(void) { return atexit((void(__cdecl*)(void))Static68cbf8_Destroy); }
// [$L_4024a0] the static initializer: FUN_4024b0, then the teardown at exit
extern "C" int __cdecl Static68cbf8_Init(void)
{
    CallC(q197_FUN_4024b0);
    return Static68cbf8_AtExit();
}
// [$L_402600] destroys the static object 68d0a8 at exit (FUN_404420)
extern "C" void __cdecl Static68d0a8_Destroy(void) { CallThis(q197_FUN_404420, &g_q197_68d0a8); }
// [$L_4025f0] registers the teardown at exit
extern "C" int __cdecl Static68d0a8_AtExit(void) { return atexit((void(__cdecl*)(void))Static68d0a8_Destroy); }
// [$L_4025c0] the static initializer: FUN_4025d0, then the teardown at exit
extern "C" int __cdecl Static68d0a8_Init(void)
{
    CallC(q197_FUN_4025d0);
    return Static68d0a8_AtExit();
}
// [$L_402690] destroys the static object 68a980 at exit (FUN_447970)
extern "C" void __cdecl Static68a980_Destroy(void) { CallThis(q197_FUN_447970, &g_q197_68a980); }
// [$L_402680] registers the teardown at exit
extern "C" int __cdecl Static68a980_AtExit(void) { return atexit((void(__cdecl*)(void))Static68a980_Destroy); }
// [$L_402650] the static initializer: FUN_402660, then the teardown at exit
extern "C" int __cdecl Static68a980_Init(void)
{
    CallC(q197_FUN_402660);
    return Static68a980_AtExit();
}
// [$L_4026e0] destroys the static object 68cf60 at exit (FUN_447970)
extern "C" void __cdecl Static68cf60_Destroy(void) { CallThis(q197_FUN_447970, &g_q197_68cf60); }
// [$L_4026d0] registers the teardown at exit
extern "C" int __cdecl Static68cf60_AtExit(void) { return atexit((void(__cdecl*)(void))Static68cf60_Destroy); }
// [$L_4026a0] the static initializer: FUN_4026b0, then the teardown at exit
extern "C" int __cdecl Static68cf60_Init(void)
{
    CallC(q197_FUN_4026b0);
    return Static68cf60_AtExit();
}
// [$L_402720] registers the teardown at exit
extern "C" int __cdecl Static4026f0_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_402730); }
// [$L_4026f0] the static initializer: FUN_402700, then the teardown at exit
extern "C" int __cdecl Static4026f0_Init(void)
{
    CallC(q197_FUN_402700);
    return Static4026f0_AtExit();
}
// [$L_4028c0] registers the teardown at exit
extern "C" int __cdecl Static4028a0_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_4028d0); }
// [$L_4028a0] the static initializer: FUN_4028b0, then the teardown at exit
extern "C" int __cdecl Static4028a0_Init(void)
{
    CallC(q197_FUN_4028b0);
    return Static4028a0_AtExit();
}
// [$L_402b60] registers the teardown at exit
extern "C" int __cdecl Static402b40_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_402b70); }
// [$L_402b40] the static initializer: FUN_402b50, then the teardown at exit
extern "C" int __cdecl Static402b40_Init(void)
{
    CallC(q197_FUN_402b50);
    return Static402b40_AtExit();
}
// [$L_402bb0] registers the teardown at exit
extern "C" int __cdecl Static402b90_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_402bc0); }
// [$L_402b90] the static initializer: FUN_402ba0, then the teardown at exit
extern "C" int __cdecl Static402b90_Init(void)
{
    CallC(q197_FUN_402ba0);
    return Static402b90_AtExit();
}
// [$L_405720] destroys the static object 68cf58 at exit ($L_449740)
extern "C" void __cdecl Static68cf58_Destroy(void) { CallThis(q197_L_449740, &g_q197_68cf58); }
// [$L_405710] registers the teardown at exit
extern "C" int __cdecl Static68cf58_AtExit(void) { return atexit((void(__cdecl*)(void))Static68cf58_Destroy); }
// [$L_4056e0] the static initializer: FUN_4056f0, then the teardown at exit
extern "C" int __cdecl Static68cf58_Init(void)
{
    CallC(q197_FUN_4056f0);
    return Static68cf58_AtExit();
}
// [$L_415290] destroys the static object 68a860 at exit (FUN_404420)
extern "C" void __cdecl Static68a860_Destroy(void) { CallThis(q197_FUN_404420, &g_q197_68a860); }
// [$L_415280] registers the teardown at exit
extern "C" int __cdecl Static68a860_AtExit(void) { return atexit((void(__cdecl*)(void))Static68a860_Destroy); }
// [$L_415250] the static initializer: FUN_415260, then the teardown at exit
extern "C" int __cdecl Static68a860_Init(void)
{
    CallC(q197_FUN_415260);
    return Static68a860_AtExit();
}

// ---------------------------------------------------------------------------
// Thunks.

KING_VAR(char, g_q198_68cbe8, 68cbe8)
KING_VAR(char, g_q198_68cf00, 68cf00)
extern "C" int __fastcall q198_Points_Length(BYTE* s); KING_ALIAS("@q198_Points_Length@4", "@Points_Length@4")
extern "C" int __fastcall q198_Vec4_FreeStorage(BYTE* s, void* d); KING_ALIAS("@q198_Vec4_FreeStorage@8", "@Vec4_FreeStorage@8")
// [$L_401ed0] Vec4_FreeStorage on the static object 68cf00
extern "C" void __cdecl Static68cf00_Destroy(void) { q198_Vec4_FreeStorage((BYTE*)&g_q198_68cf00, 0); }
// [$L_402150] Vec4_FreeStorage on the static object 68cbe8
extern "C" void __cdecl Static68cbe8_Destroy(void) { q198_Vec4_FreeStorage((BYTE*)&g_q198_68cbe8, 0); }
// [$L_455650] forwards to Points_Length for the object 56 bytes after
extern "C" int __fastcall Thunk455650(BYTE* s) { return q198_Points_Length(s + 56); }

// [$L_410320] an empty range: start 0, scale 1.0 (+8)
extern "C" BYTE* __fastcall Range410320_Init(BYTE* s)
{
    F(int, s, 8) = 0;
    F(int, s, 0) = 0;
    F(int, s, 12) = 0x3ff00000;
    return s;
}
// [$L_411bd0] an id set to none (-1)
extern "C" int* __fastcall Id411bd0_Init(int* s)
{
    *s = -1;
    return s;
}

// ---------------------------------------------------------------------------
// By-value forwarders of 60-byte records: each record is copied (Rec60_Copy) into the call.

extern "C" void FUN_461b20(); KING_ALIAS("_FUN_461b20", "@Rec60_Copy@12")
extern "C" void FUN_4667d0(); KING_ALIAS("_FUN_4667d0", "_Chars_CompareRope")
extern "C" void FUN_4668d0(); KING_ALIAS("_FUN_4668d0", "_Rope_CompareChars")
extern "C" void FUN_466a00(); KING_ALIAS("_FUN_466a00", "_Rope_CompareRope")
struct Rec60v { DWORD d[15]; };
static Rec60v Rec60v_Copy(const Rec60v& s)
{
    Rec60v r;
    CallThis(FUN_461b20, &r, &s);
    return r;
}
// [FUN_463260] FUN_4667d0 with copies of x and y
extern "C" void __cdecl Rec60_Call4667d0(int a, int b, Rec60v x, Rec60v y)
{
    Rec60v y2 = Rec60v_Copy(y), x2 = Rec60v_Copy(x);
    CallC(FUN_4667d0, a, b, x2, y2);
}
// [FUN_4632d0] FUN_4668d0 with copies of x and y
extern "C" void __cdecl Rec60_Call4668d0(Rec60v x, Rec60v y, int a, int b)
{
    Rec60v y2 = Rec60v_Copy(y), x2 = Rec60v_Copy(x);
    CallC(FUN_4668d0, x2, y2, a, b);
}
// [FUN_463310] FUN_466a00 with copies of the four records
extern "C" void __cdecl Rec60_Call466a00(Rec60v a, Rec60v b, Rec60v c, Rec60v d)
{
    Rec60v d2 = Rec60v_Copy(d), c2 = Rec60v_Copy(c), b2 = Rec60v_Copy(b), a2 = Rec60v_Copy(a);
    CallC(FUN_466a00, a2, b2, c2, d2);
}

// [$L_447c50] drops the rope that s points to (Rope_Unref inlined)
extern "C" void __fastcall RopePtr_Release(BYTE** s)
{
    if (*s)
        Rope_Unref(*s, 0);
}

// [$L_449ad0] returns the storage of a vector of 32-byte elements (begin, -, capacity end) to the allocator
extern "C" void __fastcall Vec32_FreeStorage(BYTE** v)
{
    int n = (int)(v[2] - v[0]) >> 5;
    if (n)
        Ai_PoolFree(v[0], n << 5);
}

// [$L_462b40] returns a byte vector's storage (begin, -, capacity end) to the allocator
extern "C" void __fastcall VecBytes_FreeStorage(BYTE** v)
{
    if (v[0] && v[2] - v[0])
        Ai_PoolFree(v[0], v[2] - v[0]);
}
// [$L_449130] frees the parts (+4) of each 56-byte element of vector v, then its storage
extern "C" void __fastcall Vec56b_Destroy(BYTE** v)
{
    for (BYTE* e = v[0]; e != v[1]; e += 56)
        CallThis(FUN_404420, e + 4);
    int n = (int)(v[2] - v[0]) / 56;
    if (n)
        Ai_PoolFree(v[0], n * 56);
}

// [$L_4073b0] frees the storage of s's two vectors: 8-byte elements at +16 and 24-byte elements at +4
extern "C" void __fastcall Rec4073b0_FreeVectors(BYTE* s)
{
    BYTE* p = F(BYTE*, s, 16);
    int n = (int)(F(BYTE*, s, 24) - p) >> 3;
    if (n)
        Ai_PoolFree(p, n << 3);
    p = F(BYTE*, s, 4);
    n = (int)(F(BYTE*, s, 12) - p) / 24;
    if (n)
        Ai_PoolFree(p, n * 24);
}

// ---------------------------------------------------------------------------
// Thunks.

KING_VAR(char, g_q240_68a830, 68a830)
KING_VAR(char, g_q240_68cf28, 68cf28)
extern "C" int __fastcall q240_Vec4_FreeStorageB(BYTE* s, void* d); KING_ALIAS("@q240_Vec4_FreeStorageB@8", "@Vec4_FreeStorageB@8")
// [$L_401390] Vec4_FreeStorageB on the static object 68cf28
extern "C" void __cdecl Static68cf28_Destroy(void) { q240_Vec4_FreeStorageB((BYTE*)&g_q240_68cf28, 0); }
// [$L_402490] Vec4_FreeStorageB on the static object 68a830
extern "C" void __cdecl Static68a830_Destroy(void) { q240_Vec4_FreeStorageB((BYTE*)&g_q240_68a830, 0); }

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void q241_L_401e80(); KING_ALIAS("_q241_L_401e80", "_Ai_Exit401e80")
// [$L_401e70] registers 401e80 at exit
extern "C" int __cdecl Static401e70_AtExit(void) { return atexit((void(__cdecl*)(void))q241_L_401e80); }
extern "C" void q241_L_401ed0(); KING_ALIAS("_q241_L_401ed0", "_Static68cf00_Destroy")
// [$L_401ec0] registers 401ed0 at exit
extern "C" int __cdecl Static401ec0_AtExit(void) { return atexit((void(__cdecl*)(void))q241_L_401ed0); }
extern "C" void q241_L_402150(); KING_ALIAS("_q241_L_402150", "_Static68cbe8_Destroy")
// [$L_402140] registers 402150 at exit
extern "C" int __cdecl Static402140_AtExit(void) { return atexit((void(__cdecl*)(void))q241_L_402150); }
extern "C" void q241_L_402490(); KING_ALIAS("_q241_L_402490", "_Static68a830_Destroy")
// [$L_402480] registers 402490 at exit
extern "C" int __cdecl Static402480_AtExit(void) { return atexit((void(__cdecl*)(void))q241_L_402490); }

// === createPlayersRegular ($L_422f20)

KING_VAR(AsmLabel, s_cprName66d914, 66d914)       // "createPlayersRegular"
KING_VAR(AsmLabel, s_cprNode66d904, 66d904)       // the human's start node
KING_VAR(AsmLabel, s_cprPosFmt66cc0c, 66cc0c)
KING_VAR(AsmLabel, s_cpr66cc00, 66cc00)
KING_VAR(AsmLabel, s_cpr66d8e4, 66d8e4)
KING_VAR(double, g_cprStartY68b9a8, 68b9a8)
KING_VAR(double, g_cprStartZ68b9b0, 68b9b0)
extern "C" void FUN_46dc50(); KING_ALIAS("_FUN_46dc50", "_Net_SendPlayer")    // (player)
extern "C" void FUN_488100(); KING_ALIAS("_FUN_488100", "@Filter1_Set@16")    // Filter1_Set (this, double)

// the position at +40 of q as text in 77e6e0, then error 33 at line
static void Cpr_BadPosition(BYTE* q, int line)
{
    PositionId p = F(PositionId, q, 40);
    sprintf((char*)g_positionText76, (const char*)s_cprPosFmt66cc0c, p.road, p.t, p.segment, p.object, p.m_24);
    AiError(33, 100, &g_strAiMainFile, &g_strAiMainId, line, (const char*)s_invalidId, (const char*)s_cpr66cc00,
            (const char*)g_positionText76);
}

// error 33 at line when q's position (+40) is on a road but past its objects or the object's segments
static void Cpr_CheckObject(BYTE* q, int line)
{
    PositionId p = F(PositionId, q, 40);
    if (p.road < 0 || (unsigned)p.road >= (unsigned)(g_roadEdgesEnd - g_roadEdges))
        return;
    RoadEdge* e = g_roadEdges[p.road];
    if (p.object < (int)(e->objectsEnd - e->objects) && p.segment < CallVirt<int>(e->objects[p.object], 24))
        return;
    AiError(33, 100, &g_strAiMainFile, &g_strAiMainId, line, (const char*)s_invalidPositionId, &g_aiEmpty, &g_aiEmpty);
}

// out = a copy of the list handle s (an error unless s is a list)
static void Cpr_ListCopy(StrRef* out, const StrRef* s)
{
    out->ptr = 0;
    out->len = 0;
    out->flags = 0;
    if (!s->ptr)
        return;
    if (CallVirt<int>(s->ptr, 0) == 2) {
        out->ptr = s->ptr;
        out->flags = s->flags;
        F(int, out->ptr, 4)++;
    } else
        CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(s->ptr, 4, (const char*)s_pair), 0);
}

// d = the list handle s, dropping d's old list
static void Cpr_ListAssign(StrRef* d, const StrRef* s)
{
    if (d->ptr) {
        F(int, d->ptr, 4)--;
        if (F(int, d->ptr, 4) == 0 && !(d->ptr == s->ptr && d->flags == s->flags && d->len == s->len) && d->ptr)
            CallVirt(d->ptr, 12, 1);
    }
    if (!s->ptr) {
        d->ptr = 0;
        d->flags = 0;
        d->len = 0;
    } else if (CallVirt<int>(s->ptr, 0) == 2) {
        d->ptr = s->ptr;
        d->flags = s->flags;
        F(int, d->ptr, 4)++;
        d->len = 0;
    } else
        CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(s->ptr, 4, (const char*)s_pair), 0);
}

extern "C" {

// [$L_422f20] createPlayersRegular(): the human at the start point (68b9a0) when type 1 has room, then for each
// player type with room (but 8 and 9) new players until the type is full: from the type's list of descriptions
// (a list of lists) or new ones at random free positions; the positions are checked
int __cdecl Ai_CreatePlayersRegular()
{
    Trace_EnterBuffered((const char*)s_cprName66d914);
    int node;
    Node_ByName(&node, (const char*)s_cprNode66d904);
    int* count = (int*)g_68cca8;
    int* limit = (int*)g_68cf28;
    if (count[1] < limit[1]) {
        BYTE* p = Ai_CreateNewPlayer(1, 0, -1);
        DWORD id;
        Player_Register(&id, p);
        PositionId pos;
        Ai_GetPositionIdAll(&pos, (const DVec3*)&g_startX68b9a0);
        RoomInfo ri;
        ri.m_36 = -1;
        ri.m_40 = -1;
        ri.flags = 0;
        ri.m_8 = 0;
        ri.m_12 = 0;
        ri.module = (const char*)g_undefinedModule;
        ri.room = (const char*)g_undefinedRoom;
        ri.m_24 = 0;
        ri.m_28 = 0;
        Road_GetRoadParameters(pos, &ri);
        if ((BYTE)ri.flags & 8)
            pos.m_24 |= 1;
        F(double, p, 616) = g_startX68b9a0;
        F(double, p, 624) = g_cprStartY68b9a8;
        F(double, p, 632) = g_cprStartZ68b9b0;
        Player_SetPosition(p, 0, &pos);
        F(DWORD, p, 88) |= 1;
        if (r_68d31c)
            CallC(FUN_46dc50, p);
        Rec_4015a0Both(id);
    }
    if (!r_66f62a) {
        Buf_Append(&g_trace, 0, g_tagMpl, 8);
        Buf_AppendString(&g_trace, 0, (const char*)s_cprName66d914);
        return 0;
    }
    for (int type = 0; type < 10; type++) {
        StrRef h;
        Str_CopyConstruct(&h, 0, (const StrRef*)(*(BYTE**)&g_init68a840 + type * 12));
        while (count[type] < limit[type] && type != 8 && type != 9) {
            AI_Step3();
            BYTE* p;
            BYTE* q;
            bool described = false;
            if ((BYTE)h.flags == 2) {
                StrRef m;
                Str_Member20Checked(&m, &h);
                described = (BYTE)m.flags == 2;
                Str_Release(&m);
            }
            if (described) {
                StrRef m;
                Str_Member20Checked(&m, &h);
                StrRef descr;
                Cpr_ListCopy(&descr, &m);
                p = Ai_CreatePlayerDescr(type, descr);
                Str_Release(&m);
                DWORD id;
                q = (BYTE*)*Player_Register(&id, p);
                if (F(int, q, 92) == 2)
                    Ai_TruckerNextOrder(q);
                else
                    Ai_ArriveAt(q, F(PositionId, q, 40));
                PositionId at = F(PositionId, q, 40);
                if (!Position_Valid(&at, 0))
                    Cpr_BadPosition(q, 8176);
                Cpr_CheckObject(q, 8176);
                if (F(int, p, 16) == F(int, p, 20))
                    AiError(0, 100, &g_strAiMainFile, &g_strAiMainId, 8178, (const char*)s_cpr66d8e4, &g_aiEmpty, &g_aiEmpty);
            } else {
                p = Ai_CreateNewPlayer(type, 0, -1);
                DWORD id;
                q = (BYTE*)*Player_Register(&id, p);
                if (type == 1 || g_roadNodesEnd == g_roadNodes) {
                    PositionId pos;
                    Player_SetPosition(p, 0, Ai_GetPositionIdAll(&pos, (const DVec3*)&g_startX68b9a0));
                    F(DWORD, p, 88) |= 1;
                } else {
                    PositionId pos;
                    Road_RandomFreePosition(&pos);
                    if (g_68d188)
                        Player_SetPosition(q + 8, 0, &pos);
                    else
                        while (F(int, p, 16) == F(int, p, 20)) {
                            PositionId r;
                            pos = *Road_RandomFreePosition(&r);
                            Player_SetPosition(q + 8, 0, &pos);
                            PositionId target;
                            Position_InitNode(&target, 0, node);
                            Player_GoTo(q, target, 1);
                        }
                    if (F(int, q, 92) == 2)
                        Ai_TruckerNextOrder(q);
                    else
                        Ai_ArriveAt(q, pos);
                    PositionId at = F(PositionId, q, 40);
                    if (!((RoadId_Valid(&at.road, 0) || NodeId_Valid(&at.node, 0)) && at.t >= g_tMin64b688
                          && !(at.t > g_tMax64b680) && at.segment >= 0 && at.object >= 0))
                        Cpr_BadPosition(q, 8207);
                    Cpr_CheckObject(q, 8207);
                }
            }
            if (r_68d31c)
                CallC(FUN_46dc50, p);
            if (F(int, p, 84) != 1 && F(int, p, 84) != 2)
                CallThis(FUN_488100, p + 728, 0.0);
            Rec_4015a0Both((int)q);
            if ((BYTE)h.flags == 2) {
                StrRef next;
                Str_Member32Checked(&next, &h);
                Cpr_ListAssign(&h, &next);
                Str_Release(&next);
            }
        }
        Str_Release(&h);
    }
    Trace_Leave((const char*)s_cprName66d914);
    return 0;
}

} // extern "C"

// === creating the players for a network game ($L_422400)

KING_VAR(BYTE*, g_cpn68cfd8, 68cfd8)
KING_VAR(BYTE*, g_cpn68cfdc, 68cfdc)
KING_VAR(BYTE*, g_cpnFree68a860, 68a860)     // a byte per vehicle: 1 while free (begin, end, capacity end)
KING_VAR(BYTE*, g_cpnFree68a864, 68a864)
KING_VAR(BYTE*, g_cpnFree68a868, 68a868)
KING_VAR(AsmLabel, s_cpnStart66d8f8, 66d8f8)    // the start node name format (prefix, number)
KING_VAR(AsmLabel, s_cpnGoal66d198, 66d198)
KING_VAR(AsmLabel, s_cpnNode66d168, 66d168)
extern "C" void FUN_44a490(); KING_ALIAS("_FUN_44a490", "@Vec1_Reserve@16")
extern "C" void FUN_469410(); KING_ALIAS("_FUN_469410", "_FillBytes")
extern "C" void FUN_459ac0(); KING_ALIAS("_FUN_459ac0", "_Bytes_AllocCopyB@12")
extern "C" void FUN_603540(); KING_ALIAS("_FUN_603540", "_Bytes_Copy")
extern "C" void FUN_46e120(); KING_ALIAS("_FUN_46e120", "_Mult_46e120")
extern "C" void FUN_46e140(); KING_ALIAS("_FUN_46e140", "_Mult_PlayerCount")
extern "C" void FUN_477c30(); KING_ALIAS("_FUN_477c30", "_Player_ClearDeadline")
extern "C" void FUN_4152a0(); KING_ALIAS("_FUN_4152a0", "_Ai_CreatePlayerRandom")
extern "C" void FUN_417770(); KING_ALIAS("_FUN_417770", "_Player_Retire")

static __declspec(noinline) int Cp2_Int(double d)
{
    return (int)d;
}

// a player's start position at the node named by format fmt and number n
static void Cpn_PlaceAt(BYTE* p, int n)
{
    char buf[64];
    sprintf(buf, (const char*)s_cpnStart66d8f8, g_66c6e4, n);
    int node;
    Node_ByName(&node, buf);
    PositionId pos;
    pos.road = -1;
    pos.node = node;
    pos.t = 0;
    pos.segment = 0;
    pos.object = 0;
    pos.m_24 = 0;
    pos.m_28 = -1;
    Player_SetPosition(p, 0, &pos);
}

// game mode 1: the player goes to node 66d168; mode 2: to the goal node of number n
static void Cpn_SendOff(DWORD id, int n)
{
    int node;
    if (g_68d188 == 1) {
        Node_ByName(&node, (const char*)s_cpnNode66d168);
        PositionId to;
        Position_InitNode(&to, 0, node);
        Player_GoTo((BYTE*)id, to, 1);
    }
    if (g_68d188 == 2) {
        char buf[64];
        sprintf(buf, (const char*)s_cpnGoal66d198, g_66c6e4, n);
        Node_ByName(&node, buf);
        PositionId to;
        Position_InitNode(&to, 0, node);
        Player_GoTo((BYTE*)id, to, 1);
    }
}

extern "C" {

// [$L_422400] creates the players of a network game: marks every vehicle free (68a860), the human at start node 1,
// then truckers of type 2 (from the type's descriptions, else random) up to 66c6f0 times the type's limit, each at
// its numbered start node; the positions are checked
int __cdecl Ai_CreatePlayersNetwork()
{
    {
        unsigned n = (unsigned)(g_cpn68cfdc - g_cpn68cfd8) >> 2;
        BYTE* v[3];
        char a, one = 1;
        CallThis(FUN_44a490, v, n, &a);
        v[1] = CallC<BYTE*>(FUN_469410, v[0], n, &one);
        unsigned size = v[1] - v[0];
        if (size > (unsigned)(g_cpnFree68a868 - g_cpnFree68a860)) {
            BYTE* p = CallStd<BYTE*>(FUN_459ac0, size, v[0], v[1]);
            if (unsigned cap = g_cpnFree68a868 - g_cpnFree68a860)
                Ai_PoolFree(g_cpnFree68a860, cap);
            g_cpnFree68a860 = p;
            g_cpnFree68a868 = p + size;
        } else {
            unsigned have = g_cpnFree68a864 - g_cpnFree68a860;
            if (have >= size)
                CallC(FUN_603540, v[0], v[1], g_cpnFree68a860);
            else {
                CallC(FUN_603540, v[0], v[0] + have, g_cpnFree68a860);
                CallC(FUN_603540, v[0] + have, v[1], g_cpnFree68a864);
            }
        }
        g_cpnFree68a864 = g_cpnFree68a860 + size;
        CallThis(FUN_44b090, v);
    }
    int* count = (int*)g_68cca8;
    int* limit = (int*)g_68cf28;
    if (count[1] < limit[1]) {
        int k = r_68d31c ? CallC<int>(FUN_46e120) : 0;
        BYTE* p = Ai_CreateNewPlayer(1, k, -1);
        DWORD id;
        Player_Register(&id, p);
        F(int, p, 720) = k;
        Cpn_PlaceAt(p, k + 1);
        Cpn_SendOff(id, k + 1);
        F(DWORD, p, 88) |= 1;
        if (r_68d31c)
            CallC(FUN_46dc50, p);
        Rec_4015a0Both(id);
    }
    if (!r_66f62a)
        return 0;
    int players = CallC<int>(FUN_46e140);
    double have = players;
    int most = Cp2_Int((double)limit[2] * g_66c6f0 - have);
    StrRef h;
    Str_CopyConstruct(&h, 0, (const StrRef*)(*(BYTE**)&g_init68a840 + 24));
    if (limit[2] * g_66c6f0 > have) {
        int k = players;
        int n = players + 1;
        do {
            if (count[2] >= most)
                break;
            bool described = false;
            if ((BYTE)h.flags == 2) {
                StrRef m;
                Str_Member20Checked(&m, &h);
                described = (BYTE)m.flags == 2;
                Str_Release(&m);
            }
            BYTE* p;
            if (described) {
                StrRef m;
                Str_Member20Checked(&m, &h);
                StrRef descr;
                Cpr_ListCopy(&descr, &m);
                p = Ai_CreatePlayerDescr(2, descr);
                Str_Release(&m);
            } else
                p = CallC<BYTE*>(FUN_4152a0, 2, 0, &g_rng68a870);
            F(int, p, 720) = k;
            CallC(FUN_477c30, p);
            DWORD id;
            BYTE* q = (BYTE*)*Player_Register(&id, p);
            Cpn_PlaceAt(p, n);
            Cpn_SendOff((DWORD)q, n);
            k++;
            n++;
            PositionId at = F(PositionId, q, 40);
            if (!Position_Valid(&at, 0))
                Cpr_BadPosition(q, 8072);
            Cpr_CheckObject(q, 8072);
            if (F(int, p, 16) == F(int, p, 20))
                AiError(0, 100, &g_strAiMainFile, &g_strAiMainId, 8074, (const char*)s_cpr66d8e4, &g_aiEmpty, &g_aiEmpty);
            if (g_68d188 == 1 || g_68d188 == 2)
                CallC(FUN_417770, q);
            if (r_68d31c)
                CallC(FUN_46dc50, p);
            Rec_4015a0Both((int)q);
            if ((BYTE)h.flags == 2) {
                StrRef next;
                Str_Member32Checked(&next, &h);
                Cpr_ListAssign(&h, &next);
                Str_Release(&next);
            }
        } while (limit[2] * g_66c6f0 > (double)k);
    }
    Str_Release(&h);
    return 0;
}

} // extern "C"

// === the AI's hiring task ($L_429930)

KING_VAR(BYTE*, g_hr68b990, 68b990)
KING_VAR(int, g_hr66c700, 66c700)
KING_VAR(AsmLabel, g_hr68b9c8, 68b9c8)
KING_VAR(AsmLabel, s_hr66dbc8, 66dbc8)
KING_VAR(AsmLabel, s_hr66dbb8, 66dbb8)
extern "C" void FUN_419000(); KING_ALIAS("_FUN_419000", "_Player_FreeTrader")
extern "C" void FUN_429340(); KING_ALIAS("_FUN_429340", "@AI_FreeCars@8")
extern "C" void FUN_429380(); KING_ALIAS("_FUN_429380", "@Players_CountBusyTraders@8")
extern "C" void FUN_429690(); KING_ALIAS("_FUN_429690", "_Players_MinKey")
extern "C" void FUN_429760(); KING_ALIAS("_FUN_429760", "@PlayerId_Rank@8")
extern "C" void FUN_448790(); KING_ALIAS("_FUN_448790", "@Vec_EndMinus16@8")
extern "C" void FUN_476bc0(); KING_ALIAS("_FUN_476bc0", "_Player_Hire")
extern "C" void FUN_476f70(); KING_ALIAS("_FUN_476f70", "_Player_GiveBackCargo")
extern "C" void FUN_41d300(); KING_ALIAS("_FUN_41d300", "@Int_Copy@12")
extern "C" void FUN_40e270(); KING_ALIAS("_FUN_40e270", "_Message_Text")
extern "C" void FUN_45ef70(); KING_ALIAS("_FUN_45ef70", "_Pick_RandomNodeB")
extern "C" void FUN_56ae60(); KING_ALIAS("_FUN_56ae60", "_Messages_Add")
extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")
extern "C" void FUN_426280(); KING_ALIAS("_FUN_426280", "@Int_Set@12")
extern "C" void FUN_4afbf0(); KING_ALIAS("_FUN_4afbf0", "@SchList_Begin@12")
extern "C" void FUN_4afc20(); KING_ALIAS("_FUN_4afc20", "@SchList_End@12")

static int Hr_Copy(const void* it)
{
    int t;
    CallThis(FUN_41d300, &t, it);
    return t;
}

static void Hr_Say(const AsmLabel& id) { CallC(FUN_56ae60, CallC<const char*>(FUN_40e270, (const char*)id)); }

extern "C" {

// [$L_429930] the AI's hiring task: while fewer traders are busy than cars are free, the richest suitable
// trader (kind 2, not 257, with goods; with hired drivers above rank 66c700, else free and without a boss)
// hires a random free driver; when at most one trader is free, the cheapest player is let go: from its boss,
// or its own drivers are dismissed (and 66c700 lowered)
void __cdecl Ai_HireTask()
{
    if (CallThis<int>(FUN_429380, (void*)0) < CallThis<int>(FUN_429340, (void*)0)) {
        BYTE* best = g_hr68b990;
        double key = 0;
        // the original AI_FreeCars leaves its unscaled count in ECX, which 429760 takes as its argument
        int leftover = F(int, g_68cf28, 8) - g_68d150;
        bool hasHired = CallThis<int>(FUN_429760, (void*)(INT_PTR)leftover) >= g_hr66c700;
        for (BYTE* it = *(BYTE**)g_hr68b990; it != g_hr68b990; it = *(BYTE**)it) {
            BYTE* p = CallC<BYTE*>(FUN_40a9f0, it);
            if (F(BYTE*, p, 136) == F(BYTE*, p, 140) || !(key < F(double, p, 128)) || F(int, p, 84) != 2 ||
                F(int, p, 92) == 257)
                continue;
            bool empty = F(BYTE*, p, 152) == F(BYTE*, p, 156);
            if (hasHired ? empty : (!empty || F(BYTE*, p, 168) != g_hr68b990))
                continue;
            key = F(double, p, 128);
            CallThis(FUN_426280, &best, it);
        }
        if (best != g_hr68b990) {
            BYTE* end;
            BYTE* begin;
            CallThis(FUN_4afc20, (BYTE*)&g_hr68b990, &end);
            CallThis(FUN_4afbf0, (BYTE*)&g_hr68b990, &begin);
            BYTE* r;
            CallC(FUN_45ef70, &r, begin, end, 0, (BYTE*)g_hr68b9c8);
            BYTE* pick = g_hr68b990;
            if (r != g_hr68b990)
                CallThis(FUN_426280, &pick, r);
            if (pick != g_hr68b990)
                CallC(FUN_476bc0, Hr_Copy(&best), pick);
        }
    }
    int free = 0;
    for (BYTE* it = *(BYTE**)g_hr68b990; it != g_hr68b990; it = *(BYTE**)it) {
        int c = Hr_Copy(&it), id;
        CallThis(FUN_426280, &id, c);
        if (CallC<char>(FUN_419000, id))
            free++;
    }
    if (free > 1)
        return;
    BYTE* m;
    CallC(FUN_429690, &m);
    if (m == g_hr68b990)
        return;
    DWORD keep0 = F(DWORD, m, 136), keep1 = F(DWORD, m, 140);
    BYTE* d = m + 8;
    if (F(BYTE*, d, 168) != g_hr68b990) {
        CallC(FUN_476f70, Hr_Copy(d + 168), Hr_Copy(&m), 0);
        if (F(int, F(BYTE*, d, 168), 92) == 1)
            Hr_Say(s_hr66dbc8);
        return;
    }
    while (F(BYTE*, d, 152) != F(BYTE*, d, 156)) {
        BYTE* last = CallThis<BYTE*>(FUN_448790, d + 152, 0);
        CallC(FUN_476f70, Hr_Copy(&m), Hr_Copy(last), 0);
    }
    if (F(int, d, 84) == 1)
        Hr_Say(s_hr66dbb8);
    F(DWORD, m, 136) = keep0;
    F(DWORD, m, 140) = keep1;
    if (g_hr66c700 > 1)
        g_hr66c700--;
}

} // extern "C"

// === sorting nodes by distance to a point (FUN_45f2c0)

extern "C" void FUN_45f650(); KING_ALIAS("_FUN_45f650", "@Far_Compare@16")
extern "C" void FUN_465d20(); KING_ALIAS("_FUN_465d20", "_CopyBackwardDwords2")
extern "C" void FUN_465e80(); KING_ALIAS("_FUN_465e80", "_Nodes_IntroSortLoop")
extern "C" void FUN_468d60(); KING_ALIAS("_FUN_468d60", "_Holders_MedianByDistance")
extern "C" void FUN_4691a0(); KING_ALIAS("_FUN_4691a0", "_Records_PartitionNear")
extern "C" void FUN_46ab50(); KING_ALIAS("_FUN_46ab50", "_Records_PartialSortNear")
extern "C" void FUN_46c010(); KING_ALIAS("_FUN_46c010", "_Near_LinearInsert")

struct Ns_Pt { DWORD d[6]; };

static int Ns_Copy(const int* x)
{
    int t;
    CallThis(FUN_41d300, &t, x);
    return t;
}

// whether node a is nearer the point than node b
static bool Ns_Less(Ns_Pt* pt, int a, int b)
{
    int ia, ib;
    CallThis(FUN_426280, &ib, b);
    CallThis(FUN_426280, &ia, a);
    return CallThis<char>(FUN_45f650, pt, &ia, &ib) != 0;
}

static void Ns_Insertion(int* first, int* end, Ns_Pt pt)
{
    for (int* i = first + 1; i != end; i++) {
        int v = *i;
        if (Ns_Less(&pt, v, *first)) {
            CallC(FUN_465d20, first, i, i + 1);
            *first = v;
        } else
            CallC(FUN_46c010, i, Ns_Copy(&v), pt);
    }
}

extern "C" {

// [FUN_45f2c0] std::sort of node references [first, last) by distance to the point pt (introsort: median of
// three partitions down to 16, heap sort when too deep, then insertion sort)
void __cdecl Nodes_SortByDistance(int* first, int* last, Ns_Pt pt)
{
    if (first == last)
        return;
    int n = (int)(last - first), depth = 0;
    for (int k = n; k != 1; k >>= 1)
        depth++;
    depth *= 2;
    int* end = last;
    while (end - first > 16) {
        if (depth == 0) {
            CallC(FUN_46ab50, first, end, end, 0, pt);
            break;
        }
        depth--;
        int* mid = first + (int)(end - first) / 2;
        int pivot = Ns_Copy(CallC<int*>(FUN_468d60, first, mid, end - 1, pt));
        int* cut = CallC<int*>(FUN_4691a0, first, end, pivot, pt);
        CallC(FUN_465e80, cut, end, 0, depth, pt);
        end = cut;
    }
    if (n > 16) {
        Ns_Insertion(first, first + 16, pt);
        for (int* i = first + 16; i != last; i++)
            CallC(FUN_46c010, i, Ns_Copy(i), pt);
    } else
        Ns_Insertion(first, last, pt);
}

} // extern "C"

// === introsort of market iterators by distance (FUN_45fdd0)

extern "C" void FUN_460110(); KING_ALIAS("_FUN_460110", "@Point_Farther@16")
extern "C" void FUN_46add0(); KING_ALIAS("_FUN_46add0", "_Holders_SortHeapByDistance")
extern "C" void FUN_46c240(); KING_ALIAS("_FUN_46c240", "_Far_MakeHeap56")
extern "C" void FUN_46cf00(); KING_ALIAS("_FUN_46cf00", "_Swap32")

static bool Fs_Less(Ns_Pt* pt, int a, int b) { return CallThis<char>(FUN_460110, pt, a, b) != 0; }

extern "C" {

// [FUN_45fdd0] introsort loop over market iterators [first, last) by the distance of their positions (+56)
// to pt: median-of-three partitions, recursing on the right part, heap sort when depth runs out
void __cdecl Markets_IntroSort(int* first, int* last, int unused, int depth, Ns_Pt pt)
{
    while (last - first > 16) {
        if (depth == 0) {
            CallC(FUN_46c240, first, last, pt, 0, 0);
            CallC(FUN_46add0, first, last, pt);
            return;
        }
        depth--;
        Ns_Pt cmp = pt;
        int* mid = first + (int)(last - first) / 2;
        int a = *first, b = *mid, c = last[-1];
        int pivot;
        if (Fs_Less(&cmp, a, b))
            pivot = Fs_Less(&cmp, b, c) ? b : Fs_Less(&cmp, a, c) ? c : a;
        else
            pivot = Fs_Less(&cmp, a, c) ? a : Fs_Less(&cmp, b, c) ? c : b;
        int* lo = first;
        int* hi = last;
        for (;;) {
            while (Fs_Less(&cmp, *lo, pivot))
                lo++;
            hi--;
            while (Fs_Less(&cmp, pivot, *hi))
                hi--;
            if (!(lo < hi))
                break;
            CallC(FUN_46cf00, lo, hi);
            lo++;
        }
        Markets_IntroSort(lo, last, 0, depth, pt);
        last = lo;
    }
}

} // extern "C"

// === stable_sort's adaptive step by nearness (FUN_465a20)

extern "C" void __cdecl Near_ChunkInsertion(BYTE** f, BYTE** l, int chunk, int ctx);
extern "C" void __cdecl Near_MergeLoop(BYTE** f, BYTE** l, BYTE** out, int step, int ctx);
extern "C" void __cdecl Near_MergeAdaptive(BYTE** f, BYTE** m, BYTE** l, int n1, int n2, BYTE**, int, int ctx);
extern "C" void** __cdecl Sort_MergeByNearest(void** f1, void** l1, void** f2, void** l2, void** d, const BYTE** list);

// one merge pass of runs of step from [first, last) to out
static void SsN_MergePass(BYTE** first, BYTE** last, BYTE** out, int step, int ctx)
{
    while (last - first >= step * 2) {
        out = (BYTE**)Sort_MergeByNearest((void**)first, (void**)(first + step), (void**)(first + step),
                                          (void**)(first + step * 2), (void**)out, (const BYTE**)ctx);
        first += step * 2;
    }
    int s = step < last - first ? step : (int)(last - first);
    Sort_MergeByNearest((void**)first, (void**)(first + s), (void**)(first + s), (void**)last, (void**)out,
                        (const BYTE**)ctx);
}

// insertion sort of runs of 7, then merge passes through the buffer and back
static void SsN_SortWithBuffer(BYTE** first, BYTE** last, BYTE** buf, int ctx)
{
    int len = (int)(last - first);
    Near_ChunkInsertion(first, last, 7, ctx);
    for (int step = 7; step < len;) {
        SsN_MergePass(first, last, buf, step, ctx);
        step *= 2;
        Near_MergeLoop(buf, buf + len, first, step, ctx);
        step *= 2;
    }
}

extern "C" {

// [FUN_465a20] stable_sort's adaptive step for [first, last) of pointers by nearness (ctx) with the buffer buf
// (size n)
void __cdecl Near_StableSortAdaptive(BYTE** first, BYTE** last, BYTE** buf, int n, int ctx)
{
    int len = (int)(last - first + 1) / 2;
    BYTE** mid = first + len;
    if (len > n) {
        Near_StableSortAdaptive(first, mid, buf, n, ctx);
        Near_StableSortAdaptive(mid, last, buf, n, ctx);
    } else {
        SsN_SortWithBuffer(first, mid, buf, ctx);
        SsN_SortWithBuffer(mid, last, buf, ctx);
    }
    Near_MergeAdaptive(first, mid, last, (int)(mid - first), (int)(last - mid), buf, n, ctx);
}

} // extern "C"

// === stable_sort's adaptive step by the field at +28 / +32 (FUN_464fe0, FUN_4654c0)

extern "C" void __cdecl Sort_InsertionBy28(void** f, void** l);
extern "C" void __cdecl Sort_InsertionBy32(void** f, void** l);
extern "C" void** __cdecl Sort_MergeBy28(void** f1, void** l1, void** f2, void** l2, void** d);
extern "C" void** __cdecl Sort_MergeBy32(void** f1, void** l1, void** f2, void** l2, void** d);
extern "C" void __cdecl Sort_MergeLoop28(BYTE** f, BYTE** l, BYTE** r, int step);
extern "C" void __cdecl Sort_MergeLoop32(BYTE** f, BYTE** l, BYTE** r, int step);
extern "C" void __cdecl Merge28_Adaptive(BYTE** f, BYTE** m, BYTE** l, int n1, int n2, BYTE**, int, int);
extern "C" void __cdecl Merge32_Adaptive(BYTE** f, BYTE** m, BYTE** l, int n1, int n2, BYTE**, int, int);

// the pieces of one stable_sort instance over pointers
struct SsP_Ops {
    void (__cdecl* insertion)(void**, void**);
    void** (__cdecl* merge)(void**, void**, void**, void**, void**);
    void (__cdecl* mergeLoop)(BYTE**, BYTE**, BYTE**, int);
    void (__cdecl* adaptive)(BYTE**, BYTE**, BYTE**, int, int, BYTE**, int, int);
};

static void SsP_SortWithBuffer(const SsP_Ops& o, BYTE** first, BYTE** last, BYTE** buf)
{
    int len = (int)(last - first);
    BYTE** p = first;
    for (; last - p >= 7; p += 7)
        o.insertion((void**)p, (void**)(p + 7));
    o.insertion((void**)p, (void**)last);
    for (int step = 7; step < len;) {
        BYTE** f = first;
        BYTE** out = buf;
        for (; last - f >= step * 2; f += step * 2)
            out = (BYTE**)o.merge((void**)f, (void**)(f + step), (void**)(f + step), (void**)(f + step * 2), (void**)out);
        int s = step < last - f ? step : (int)(last - f);
        o.merge((void**)f, (void**)(f + s), (void**)(f + s), (void**)last, (void**)out);
        step *= 2;
        o.mergeLoop(buf, buf + len, first, step);
        step *= 2;
    }
}

static void SsP_SortAdaptive(const SsP_Ops& o, BYTE** first, BYTE** last, BYTE** buf, int n, int pred)
{
    int len = (int)(last - first + 1) / 2;
    BYTE** mid = first + len;
    if (len > n) {
        SsP_SortAdaptive(o, first, mid, buf, n, pred);
        SsP_SortAdaptive(o, mid, last, buf, n, pred);
    } else {
        SsP_SortWithBuffer(o, first, mid, buf);
        SsP_SortWithBuffer(o, mid, last, buf);
    }
    o.adaptive(first, mid, last, (int)(mid - first), (int)(last - mid), buf, n, pred);
}

static const SsP_Ops k_ssBy28 = {Sort_InsertionBy28, Sort_MergeBy28, Sort_MergeLoop28, Merge28_Adaptive};
static const SsP_Ops k_ssBy32 = {Sort_InsertionBy32, Sort_MergeBy32, Sort_MergeLoop32, Merge32_Adaptive};

extern "C" {

// [FUN_464fe0] stable_sort's adaptive step for [first, last) of pointers by their field at +28, with the buffer
// buf (size n)
void __cdecl Sort28_StableSortAdaptive(BYTE** first, BYTE** last, BYTE** buf, int n, int pred)
{
    SsP_SortAdaptive(k_ssBy28, first, last, buf, n, pred);
}

// [FUN_4654c0] the same by the field at +32
void __cdecl Sort32_StableSortAdaptive(BYTE** first, BYTE** last, BYTE** buf, int n, int pred)
{
    SsP_SortAdaptive(k_ssBy32, first, last, buf, n, pred);
}

} // extern "C"

extern "C" {

// [FUN_45d8f0]
void __fastcall Vec4b_Insert45d8f0(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

// [FUN_45d630]
void __fastcall Vec4b_Insert45d630(DWORD* s, void*, DWORD* pos, const DWORD* x)
{
    Vec4b_InsertAt(s, pos, x);
}

} // extern "C"

// === a random partner near the same goods (FUN_45e8f0)

extern "C" void FUN_425bb0(); KING_ALIAS("_FUN_425bb0", "_Player_CanTrade2")
extern "C" void FUN_425c60(); KING_ALIAS("_FUN_425c60", "_AI_PairOk")
extern "C" void FUN_45bea0(); KING_ALIAS("_FUN_45bea0", "_DVec3_Sub")
extern "C" void FUN_45eb50(); KING_ALIAS("_FUN_45eb50", "@Position_Equals@12")
extern "C" void FUN_4777b0(); KING_ALIAS("_FUN_4777b0", "_Player_FirstGoods")
extern "C" void FUN_45dde0(); KING_ALIAS("_FUN_45dde0", "@Vec4b_InsertE@16")
KING_VAR(double, g_rp66c6a8, 66c6a8)

extern "C" {

// [FUN_45e8f0] *out = a random node of the player list [first, last) whose player (+8) can trade with me, is in
// state 4 (+92) and carries goods at t's place (+24, +12) within 66c6a8 of me (+624; its position +616 or
// +304 by +88 bit 0); last when there is none
BYTE** __cdecl Player_RandomPartner(BYTE** out, BYTE* first, BYTE* last, BYTE* me, BYTE* t, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* n = first; n != last; n = F(BYTE*, n, 0)) {
        BYTE* p = n + 8;
        if (F(BYTE*, p, 96) == me || !CallC<char>(FUN_425c60, me + 8, p) || !CallC<char>(FUN_425bb0, p) ||
            F(int, p, 92) != 4)
            continue;
        BYTE* g = CallC<BYTE*>(FUN_4777b0, p);
        if (!g || !CallThis<char>(FUN_45eb50, g + 24, t + 24) || F(int, g, 12) != F(int, t, 12))
            continue;
        double pos[3], d[3];
        memcpy(pos, p + ((F(BYTE, p, 88) & 1) ? 616 : 304), 24);
        if (!(CallC<double>(FUN_4577f0, CallC<double*>(FUN_45bea0, d, me + 624, pos)) < g_rp66c6a8))
            continue;
        if (v[1] != v[2]) {
            CallC(FUN_4611c0, v[1], &n);
            v[1] += 4;
        } else
            CallThis(FUN_45dde0, v, v[1], &n);
    }
    *out = PickRandom(v, last, rng);
    return out;
}

} // extern "C"

// === stable_sort by nearness (FUN_45cf80)

ASM_FUNC(FUN_462340, @TempBuf4_Construct@16)    // a temporary buffer of pointers for [first, last)
extern "C" void __cdecl Near_StableSort(BYTE** f, BYTE** l, int ctx);   // ai_containers.cpp
extern "C" void __cdecl free(void* p);

extern "C" {

// [FUN_45cf80] stable_sort of the pointers [first, last) by nearness (ctx) through a temporary buffer, in place
// when none can be had
void __cdecl Near_StableSortBuf(BYTE** first, BYTE** last, int, int, int ctx)
{
    int tb[3];
    CallThis(FUN_462340, tb, first, last);
    if (!tb[2])
        Near_StableSort(first, last, ctx);
    else
        Near_StableSortAdaptive(first, last, (BYTE**)tb[2], tb[1], ctx);
    free((void*)tb[2]);
}

} // extern "C"

// === stable_sort by the field at +28 (FUN_45c8d0)

extern "C" void __cdecl Sort_Stable28(BYTE** first, BYTE** last, int);   // ai_containers.cpp

extern "C" {

// [FUN_45c8d0] stable_sort of the pointers [first, last) by their field at +28 through a temporary buffer, in
// place when none can be had
void __cdecl Sort28_StableSort(BYTE** first, BYTE** last, int, int, int pred)
{
    int tb[3];
    TempBuf_Construct(tb, 0, (BYTE*)first, (BYTE*)last);
    if (!tb[2])
        Sort_Stable28(first, last, pred);
    else
        Sort28_StableSortAdaptive(first, last, (BYTE**)tb[2], tb[1], pred);
    free((void*)tb[2]);
}

} // extern "C"

// === a random waiting player (FUN_45e0a0)

extern "C" void FUN_4d8a40(); KING_ALIAS("_FUN_4d8a40", "_Vehicle_Exists")
extern "C" void FUN_418fd0(); KING_ALIAS("_FUN_418fd0", "_Player_WaitsInTown2")

extern "C" {

// [FUN_45e0a0] *out = a random node of the player list [first, last) whose player (+8) has no vehicle (+76),
// isn't flagged (+88 bit 0), is in state 2 (+84) with an order kind other than 256 / 257 (+92), isn't busy (+488)
// and waits in a town; last when there is none
BYTE** __cdecl Player_RandomWaiting(BYTE** out, BYTE* first, BYTE* last, int, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* n = first; n != last; n = F(BYTE*, n, 0)) {
        BYTE* p = n + 8;
        int veh = F(int, p, 76);
        if (veh != -1 && CallC<int>(FUN_4d8a40, veh))
            continue;
        if ((F(BYTE, p, 88) & 1) || F(int, p, 84) != 2 || F(int, p, 92) == 256 || F(int, p, 92) == 257 ||
            F(BYTE, p, 488) || !CallC<char>(FUN_418fd0, p))
            continue;
        if (v[1] != v[2]) {
            CallC(FUN_4611c0, v[1], &n);
            v[1] += 4;
        } else
            CallThis(FUN_45dde0, v, v[1], &n);
    }
    *out = PickRandom(v, last, rng);
    return out;
}

} // extern "C"

// === a random far named object (FUN_45dbb0)

extern "C" void FUN_464230(); KING_ALIAS("_FUN_464230", "@Vec4b_InsertH@16")
extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);
struct Rf_DVec3 { double x, y, z; };

extern "C" {

// [FUN_45dbb0] a random one of the slots [first, last) whose object (not off, +12 bit 4) is named like name and at
// least maxd from p (+48 its position); last when there is none
BYTE** __cdecl Objects_RandomFar(BYTE** first, BYTE** last, const char* name, int, Rf_DVec3 p, double maxd,
                                 DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE** it = first; it != last; it++) {
        BYTE* o = *it;
        if (F(BYTE, o, 12) & 16)
            continue;
        const char* s = F(BYTE*, o, 0) != F(BYTE*, o, 4) ? F(const char*, o, 0) : 0;
        if (strncmp(name, s, strlen(name)))
            continue;
        double d[3];
        if (CallC<double>(FUN_4577f0, CallC<double*>(FUN_45bea0, d, &p, o + 48)) < maxd)
            continue;
        if (v[1] != v[2]) {
            if (v[1])
                *(BYTE***)v[1] = it;
            v[1] += 4;
        } else
            CallThis(FUN_464230, v, v[1], &it);
    }
    return (BYTE**)PickRandom(v, (BYTE*)last, rng);
}

} // extern "C"

// === clearing a hash table of rope references ($L_4028f0)

extern "C" void __cdecl Rope_UnrefIf(BYTE* n);

extern "C" {

// [$L_4028f0] destructor of hash table s (buckets +4 .. +12, count +16): every node (8 bytes: next, rope) released
// to the pool after its rope, then the bucket array
void __fastcall RopeHash_Destroy(BYTE* s)
{
    BYTE** b = F(BYTE**, s, 4);
    for (unsigned i = 0; i < (unsigned)((F(BYTE**, s, 8) - F(BYTE**, s, 4))); i++) {
        for (BYTE* n = F(BYTE**, s, 4)[i]; n;) {
            BYTE* next = F(BYTE*, n, 0);
            Rope_UnrefIf(F(BYTE*, n, 4));
            CallC(FUN_44b4a0, n, 8);
            n = next;
        }
        F(BYTE**, s, 4)[i] = 0;
    }
    F(int, s, 16) = 0;
    b = F(BYTE**, s, 4);
    if (int n = (int)(F(BYTE**, s, 12) - b))
        CallC(FUN_44b4a0, b, n * 4);
}

} // extern "C"

// === a random trade partner waiting (FUN_45eba0)

extern "C" void FUN_425b40(); KING_ALIAS("_FUN_425b40", "_Player_CanTrade")
extern "C" void FUN_425d40(); KING_ALIAS("_FUN_425d40", "_AI_PairOkWaiting")

extern "C" {

// [FUN_45eba0] *out = a random node of the player list [first, last) whose player (+8) other than me can trade
// with me while waiting, isn't my partner (+168), has no vehicle (+76) and isn't flagged (+88 bit 0); last when
// there is none
BYTE** __cdecl Player_RandomTradePartner(BYTE** out, BYTE* first, BYTE* last, BYTE* me, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* n = first; n != last; n = F(BYTE*, n, 0)) {
        BYTE* p = n + 8;
        if (F(BYTE*, p, 96) == me || !CallC<char>(FUN_425d40, me + 8, p) || !CallC<char>(FUN_425b40, p) ||
            F(BYTE*, p, 168) == me)
            continue;
        int veh = F(int, p, 76);
        if (veh != -1 && CallC<int>(FUN_4d8a40, veh))
            continue;
        if (F(BYTE, p, 88) & 1)
            continue;
        if (v[1] != v[2]) {
            CallC(FUN_4611c0, v[1], &n);
            v[1] += 4;
        } else
            CallThis(FUN_45dde0, v, v[1], &n);
    }
    *out = PickRandom(v, last, rng);
    return out;
}

} // extern "C"

// === a random trade partner (FUN_45e520)

extern "C" {

// [FUN_45e520] like Player_RandomTradePartner with the plain pairing and trade checks (AI_PairOk,
// Player_CanTrade2)
BYTE** __cdecl Player_RandomTradePartner2(BYTE** out, BYTE* first, BYTE* last, BYTE* me, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* n = first; n != last; n = F(BYTE*, n, 0)) {
        BYTE* p = n + 8;
        if (F(BYTE*, p, 96) == me || !CallC<char>(FUN_425c60, me + 8, p) || !CallC<char>(FUN_425bb0, p) ||
            F(BYTE*, p, 168) == me)
            continue;
        int veh = F(int, p, 76);
        if (veh != -1 && CallC<int>(FUN_4d8a40, veh))
            continue;
        if (F(BYTE, p, 88) & 1)
            continue;
        if (v[1] != v[2]) {
            CallC(FUN_4611c0, v[1], &n);
            v[1] += 4;
        } else
            CallThis(FUN_45dde0, v, v[1], &n);
    }
    *out = PickRandom(v, last, rng);
    return out;
}

} // extern "C"

// === the final insertion sort of market iterators by distance (FUN_4601c0)

extern "C" void FUN_46b020(); KING_ALIAS("_FUN_46b020", "_Sort_UnguardedInsertByDistance")
extern "C" void FUN_463b50(); KING_ALIAS("_FUN_463b50", "_CopyBackwardDwords")

// the distance of market iterator e's position (+56) from pt
static double Mk_Dist(int e, const Ns_Pt* pt)
{
    double d[3];
    return CallC<double>(FUN_4577f0, CallC<double*>(FUN_45bea0, d, *(BYTE**)e + 56, pt));
}

extern "C" {

// [FUN_4601c0] the insertion sort ending std::sort of market iterators [first, last) by distance to pt
void __cdecl Markets_FinalInsertion(int* first, int* last, Ns_Pt pt)
{
    if (last - first > 16) {
        int* mid = first + 16;
        for (int* i = first + 1; i != mid; i++) {
            int v = *i;
            double dv = Mk_Dist(v, &pt);
            if (Mk_Dist(*first, &pt) > dv) {
                for (int* p = i; p != first; p--)
                    *p = p[-1];
                *first = v;
            } else
                CallC(FUN_46b020, i, v, pt);
        }
        for (int* i = mid; i != last; i++) {
            int v = *i;
            int* d = i;
            for (int* n = i - 1;; n--) {
                double dv = Mk_Dist(v, &pt);
                if (!(Mk_Dist(*n, &pt) > dv))
                    break;
                *d = *n;
                d = n;
            }
            *d = v;
        }
        return;
    }
    if (first == last)
        return;
    for (int* i = first + 1; i != last; i++) {
        int v = *i;
        double dv = Mk_Dist(v, &pt);
        if (Mk_Dist(*first, &pt) > dv) {
            CallC(FUN_463b50, first, i, i + 1, 0, 0);
            *first = v;
        } else {
            Ns_Pt cmp = pt;
            int* d = i;
            for (int* n = i - 1; Fs_Less(&cmp, v, *n); n--) {
                *d = *n;
                d = n;
            }
            *d = v;
        }
    }
}

} // extern "C"

// === the vector of curve records' destructor (458f20)

extern "C" {

// [$L_458f20] destructor of a vector<1592-byte curve record> (their parts need no destruction): its storage freed
void __fastcall Vec1592_Destroy(BYTE** v)
{
    if (int n = (int)(v[2] - v[0]) / 1592)
        Ai_PoolFree(v[0], n * 1592);
}

} // extern "C"

// === random trade partners, partners allowed (FUN_45ed90, FUN_45e710)

// a random node of [first, last) passing pair and trade (and the vehicle and flag checks), not me; last when none
static BYTE** Rp_Pick(BYTE** out, BYTE* first, BYTE* last, BYTE* me, DWORD* rng, void (*pair)(), void (*trade)())
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE* n = first; n != last; n = F(BYTE*, n, 0)) {
        BYTE* p = n + 8;
        if (F(BYTE*, p, 96) == me || !CallC<char>(pair, me + 8, p) || !CallC<char>(trade, p))
            continue;
        int veh = F(int, p, 76);
        if (veh != -1 && CallC<int>(FUN_4d8a40, veh))
            continue;
        if (F(BYTE, p, 88) & 1)
            continue;
        if (v[1] != v[2]) {
            CallC(FUN_4611c0, v[1], &n);
            v[1] += 4;
        } else
            CallThis(FUN_45dde0, v, v[1], &n);
    }
    *out = PickRandom(v, last, rng);
    return out;
}

extern "C" {

// [FUN_45ed90] Player_RandomTradePartner allowing the present partner
BYTE** __cdecl Player_RandomTradePartner3(BYTE** out, BYTE* first, BYTE* last, BYTE* me, DWORD* rng)
{
    return Rp_Pick(out, first, last, me, rng, FUN_425d40, FUN_425b40);
}

// [FUN_45e710] Player_RandomTradePartner2 allowing the present partner
BYTE** __cdecl Player_RandomTradePartner4(BYTE** out, BYTE* first, BYTE* last, BYTE* me, DWORD* rng)
{
    return Rp_Pick(out, first, last, me, rng, FUN_425c60, FUN_425bb0);
}

} // extern "C"

// === the introsort loop of node references by distance (FUN_465e80)

extern "C" void Is_MakeHeap(); KING_ALIAS("_Is_MakeHeap", "_MakeHeap4B")
extern "C" void Is_PopHeap(); KING_ALIAS("_Is_PopHeap", "_PopHeapLast4P")

extern "C" {

// [FUN_465e80] introsort loop of node references [first, last) by distance to pt: median-of-three partitions,
// recursing on the right part, heap sort when depth runs out
void __cdecl Nodes_IntroSortLoop(int* first, int* last, int, int depth, Ns_Pt pt)
{
    while (last - first > 16) {
        if (!depth) {
            CallC(Is_MakeHeap, first, last, pt, 0, 0);
            for (; last - first > 1; last--)
                CallC(Is_PopHeap, first, last, pt);
            return;
        }
        depth--;
        int* mid = first + (int)(last - first) / 2;
        int pivot = Ns_Copy(CallC<int*>(FUN_468d60, first, mid, last - 1, pt));
        int* cut = CallC<int*>(FUN_4691a0, first, last, pivot, pt);
        Nodes_IntroSortLoop(cut, last, 0, depth, pt);
        last = cut;
    }
}

} // extern "C"

// === two container destructors (449300, 449740)

extern "C" {

// [$L_449300] destructor of a vector of script values (12 bytes): each released, the storage freed
void __fastcall SchValues_Destroy(BYTE** v)
{
    for (BYTE* p = v[0]; p != v[1]; p += 12)
        CallThis(FUN_4a36c0, p);
    if (int n = (int)(v[2] - v[0]) / 12)
        Ai_PoolFree(v[0], n * 12);
}

// [$L_449740] destructor of a list (head +0) of 64-byte nodes holding a vector of 12-byte records (+44): the
// vectors' storage and the nodes freed, then the head
void __fastcall List12_Destroy(BYTE** s)
{
    for (BYTE* n = *(BYTE**)s[0]; n != s[0];) {
        BYTE* next = *(BYTE**)n;
        BYTE** v = (BYTE**)(n + 44);
        Stl_Free12(v[0], (unsigned)((v[2] - v[0]) / 12));
        Ai_PoolFree(n, 64);
        n = next;
    }
    F(BYTE*, s[0], 0) = s[0];
    F(BYTE*, s[0], 4) = s[0];
    Ai_PoolFree(s[0], 64);
}

} // extern "C"

// === more vector destructors (447750, 448130, 448f30, 4494c0)

extern "C" void Vd_VecBFree(); KING_ALIAS("_Vd_VecBFree", "@VecB_FreeStorage@8")
extern "C" void Vd_Vec24Destroy(); KING_ALIAS("_Vd_Vec24Destroy", "@Vec24_Destroy@8")
extern "C" void Vd_Vec1Free(); KING_ALIAS("_Vd_Vec1Free", "@Vec1_Free@8")
extern "C" void Vd_Vec4Free(); KING_ALIAS("_Vd_Vec4Free", "@Vec4_Free@8")

// the storage of a vector of size-byte records freed
static void Vd_Storage(BYTE** v, int size)
{
    if (int n = (int)(v[2] - v[0]) / size)
        Ai_PoolFree(v[0], n * size);
}

extern "C" {

// [$L_447750] destructor of a vector of 104-byte records (a value at +0, a vector at +88)
void __fastcall Vec104_Destroy(BYTE** v)
{
    for (BYTE* e = v[0]; e != v[1]; e += 104) {
        CallThis(Vd_VecBFree, e + 88);
        CallThis(FUN_4a36c0, e);
    }
    Vd_Storage(v, 104);
}

// [$L_448130] destructor of a vector of 24-byte records (a value at +0, a vector at +12)
void __fastcall Vec24Rec_Destroy(BYTE** v)
{
    for (BYTE* e = v[0]; e != v[1]; e += 24) {
        CallThis(Vd_Vec24Destroy, e + 12);
        CallThis(FUN_4a36c0, e);
    }
    Vd_Storage(v, 24);
}

// [$L_448f30] destructor of a vector of byte vectors
void __fastcall VecVec1_Destroy(BYTE** v)
{
    for (BYTE* e = v[0]; e != v[1]; e += 12)
        CallThis(Vd_Vec1Free, e);
    Vd_Storage(v, 12);
}

// [$L_4494c0] destructor of a vector of dword vectors
void __fastcall VecVec4_Destroy(BYTE** v)
{
    for (BYTE* e = v[0]; e != v[1]; e += 12)
        CallThis(Vd_Vec4Free, e);
    Vd_Storage(v, 12);
}

} // extern "C"

// === a random road of a kind (FUN_45cdc0)

extern "C" void FUN_464230(); KING_ALIAS("_FUN_464230", "@Vec4b_InsertH@16")

extern "C" {

// [FUN_45cdc0] a random element of [first, last) (objects, flags +12) that player p may use: not closed (bit 3);
// for a truck (+92 == 2) of bit 1 with mask bit 0, else of bit 0; for others of bit 6; last when none (numbers from rng)
BYTE** __cdecl Pick_RandomRoad(BYTE** first, BYTE** last, unsigned mask, DWORD* p, DWORD* rng)
{
    DWORD v[3] = {0, 0, 0};
    for (BYTE** it = first; it != last; it++) {
        DWORD f = F(DWORD, *it, 12);
        if (f & 8)
            continue;
        bool ok;
        if (p[23] == 2)
            ok = (mask & 1) ? (f & 2) != 0 : (f & 1) != 0;
        else
            ok = (f & 64) != 0;
        if (ok)
            PtrVec_Push(v, FUN_464230, (BYTE*)it);
    }
    return (BYTE**)PickRandom(v, (BYTE*)last, rng);
}

} // extern "C"

// === the final insertion sort of 32-byte entries by their key, largest first (FUN_45e370)

struct Fi_E32 { DWORD w[2]; double key; DWORD w4[4]; };
extern "C" void Fi_CopyBack(); KING_ALIAS("_Fi_CopyBack", "_CopyBackward32s")
extern "C" void Fi_Insert(); KING_ALIAS("_Fi_Insert", "_Sort_UnguardedInsertByKey")

// the unguarded insertion of a copy of e (word 1 skipped, as the entries' copy does)
static void Fi_Unguarded(Fi_E32* at, const Fi_E32* e)
{
    Fi_E32 c;
    CallThis(FUN_458a40, &c, e);
    CallC(Fi_Insert, at, c);
}

// the insertion sort of [first, last)
static void Fi_InsertionSort(Fi_E32* first, Fi_E32* last)
{
    if (first == last)
        return;
    for (Fi_E32* i = first + 1; i != last; i++) {
        Fi_E32 v = *i;
        if (v.key > first->key) {
            CallC(Fi_CopyBack, first, i, i + 1, 0, 0);
            *first = v;
        } else
            Fi_Unguarded(i, &v);
    }
}

extern "C" {

// [FUN_45e370] std::__final_insertion_sort of the 32-byte entries [first, last) by the double at +8, descending
void __cdecl Entries32_FinalSort(Fi_E32* first, Fi_E32* last)
{
    if ((int)(((BYTE*)last - (BYTE*)first) & ~31) <= 512) {
        Fi_InsertionSort(first, last);
        return;
    }
    Fi_InsertionSort(first, first + 16);
    for (Fi_E32* i = first + 16; i != last; i++)
        Fi_Unguarded(i, i);
}

} // extern "C"

// === the final insertion sort of pointers by +504 (FUN_45f990)

extern "C" void Fp_IntCopy(); KING_ALIAS("_Fp_IntCopy", "@Int_Copy@12")
extern "C" void Fp_CopyBack(); KING_ALIAS("_Fp_CopyBack", "_CopyBackwardDwords")
extern "C" void Fp_Insert(); KING_ALIAS("_Fp_Insert", "_Sort_UnguardedInsertBy504")

static void Fp_Unguarded(BYTE** at, BYTE* const* v, int cmp)
{
    BYTE* c;
    CallThis(Fp_IntCopy, &c, v);
    CallC(Fp_Insert, at, c, cmp);
}

// the insertion sort of [first, last): an element whose +504 differs from the first's goes to the front
static void Fp_InsertionSort(BYTE** first, BYTE** last, int cmp)
{
    if (first == last)
        return;
    for (BYTE** i = first + 1; i != last; i++) {
        BYTE* v = *i;
        if (!x87_eq(F(double, v, 504) - F(double, *first, 504), 0.0)) {
            CallC(Fp_CopyBack, first, i, i + 1, 0, 0);
            *first = v;
        } else
            Fp_Unguarded(i, &v, cmp);
    }
}

extern "C" {

// [FUN_45f990] std::__final_insertion_sort of the pointers [first, last) by the double at +504 (the comparison
// is "differs", as Sort_UnguardedInsertBy504)
void __cdecl Ptrs504_FinalSort(BYTE** first, BYTE** last, int cmp)
{
    if ((int)(((BYTE*)last - (BYTE*)first) & ~3) <= 64) {
        Fp_InsertionSort(first, last, cmp);
        return;
    }
    Fp_InsertionSort(first, first + 16, cmp);
    for (BYTE** i = first + 16; i != last; i++)
        Fp_Unguarded(i, i, cmp);
}

} // extern "C"

// === a rope iterator's leaf cache (FUN_46b140)

extern "C" {

// [FUN_46b140] rope iterator it (position +0, root +4) finds the leaf holding its position: the path down the
// concatenations (last four nodes kept at +24.., index +40, turns +44), the leaf start +8, and the buffer +12 /
// +16 / +20 into the leaf's characters or, for function leaves, up to 15 characters fetched into +45
void __cdecl Rope_SetCache(BYTE* it)
{
    DWORD pos = F(DWORD, it, 0);
    BYTE* node = F(BYTE*, it, 4);
    if (!(pos < F(DWORD, node, 0)))
    {
        F(int, it, 16) = 0;
        return;
    }
    if (F(BYTE*, node, 16))
    {
        BYTE* cs = F(BYTE*, node, 16);
        F(BYTE*, it, 12) = cs;
        F(BYTE*, it, 20) = cs + F(DWORD, node, 0);
        F(BYTE*, it, 24) = node;
        F(BYTE*, it, 16) = cs + pos;
        F(int, it, 40) = 0;
        F(int, it, 8) = 0;
        return;
    }
    BYTE* path[48];
    int depth = -1;
    BYTE dirns = 0;
    DWORD start = 0;
    for (;;)
    {
        path[++depth] = node;
        int tag = (signed char)node[8];
        if (tag == 1)
        {
            dirns <<= 1;
            BYTE* left = F(BYTE*, node, 20);
            DWORD end = start + F(DWORD, left, 0);
            if (pos >= end)
            {
                dirns |= 1;
                node = F(BYTE*, node, 24);
                start = end;
            }
            else
                node = left;
        }
        else if ((unsigned)tag <= 3)
            break;
    }
    F(DWORD, it, 8) = start;
    int k = depth - 3;
    if (k < 0)
        k = 0;
    int last = -1;
    if (k <= depth)
    {
        for (int j = k; j <= depth; j++)
            F(BYTE*, it, 24 + (j - k) * 4) = path[j];
        last = depth - k;
    }
    F(int, it, 40) = last;
    it[44] = dirns;
    BYTE* leaf = F(BYTE*, it, 24 + last * 4);
    DWORD lp = F(DWORD, it, 8);
    pos = F(DWORD, it, 0);
    int tag = (signed char)leaf[8];
    if (tag == 0)
    {
        BYTE* data = F(BYTE*, leaf, 20);
        F(BYTE*, it, 12) = data;
        F(BYTE*, it, 16) = data + (pos - lp);
        F(BYTE*, it, 20) = data + F(DWORD, leaf, 0);
        return;
    }
    if (tag <= 1 || tag > 3)
        return;
    DWORD leafEnd = F(DWORD, leaf, 0) + lp;
    BYTE* fn = F(BYTE*, leaf, 20);
    DWORD len = 15, from = lp;
    if (!(lp + 15 > pos))
    {
        from = pos - 3;
        if (from + 15 > leafEnd)
            from = leafEnd - 15;
    }
    if (from + 15 > leafEnd)
        len = leafEnd - from;
    BYTE* buf = it + 45;
    CallVirt(fn, 4, from - lp, len, buf);
    F(BYTE*, it, 12) = buf;
    F(BYTE*, it, 16) = buf + (pos - from);
    F(BYTE*, it, 20) = buf + len;
}

} // extern "C"

// === which of two objects is farther from a point (FUN_45f650)

extern "C" {

// [FUN_45f650] comparator at point s (three doubles): 1 when object b's road position (+40) is farther from it
// than object a's
int __fastcall Far_Compare(const double* s, void*, BYTE** a, BYTE** b)
{
    DVec3 pa, pb;
    Road_PointOf(&pa, *(PositionId*)(*a + 40));
    double ax = s[0] - pa.x, ay = s[1] - pa.y, az = s[2] - pa.z;
    Road_PointOf(&pb, *(PositionId*)(*b + 40));
    double bx = s[0] - pb.x, by = s[1] - pb.y, bz = s[2] - pb.z;
    double la = x87_sqrt(ay * ay + az * az + ax * ax);
    double lb = x87_sqrt(by * by + bz * bz + bx * bx);
    return lb > la;
}

} // extern "C"

// === two introsort loops (FUN_45e2a0 / FUN_45f8d0)

struct Is_E32 { DWORD w[2]; double key; DWORD w4[4]; };
extern "C" Is_E32* __cdecl Sort_MedianByKey(Is_E32* a, Is_E32* b, Is_E32* c);
extern "C" Is_E32* __cdecl Sort_PartitionByKey(Is_E32* first, Is_E32* last, Is_E32 pivot);
extern "C" void** __cdecl Sort_MedianBy504(void** a, void** b, void** c);
extern "C" void** __cdecl Sort_PartitionBy504(void** first, void** last, void* pivot);
extern "C" void __cdecl MakeHeap32(BYTE* first, BYTE* last, DWORD pred);
extern "C" void __cdecl PopHeapLast32(BYTE* first, BYTE* last, int unused, DWORD pred);
extern "C" void __cdecl MakeHeap4(DWORD* first, DWORD* last, DWORD pred);
extern "C" void __cdecl PopHeapLast4(DWORD* first, DWORD* last, DWORD pred);

extern "C" {

// [FUN_45e2a0] introsort of 32-byte records by their key (+8) down to runs of 16, heapsort when the depth is used up
void __cdecl IntroSort_ByKey(Is_E32* first, Is_E32* last, int, int depth, DWORD pred)
{
    while ((((BYTE*)last - (BYTE*)first) & ~31) > 512)
    {
        if (!depth)
        {
            MakeHeap32((BYTE*)first, (BYTE*)last, pred);
            for (BYTE* e = (BYTE*)last; ((e - (BYTE*)first) & ~31) > 32; e -= 32)
                PopHeapLast32((BYTE*)first, e, 0, pred);
            return;
        }
        depth--;
        int n = (int)(last - first);
        Is_E32* m = Sort_MedianByKey(first, first + n / 2, last - 1);
        Is_E32 pv;
        Rec_Copy8Skip1((int*)&pv, 0, (const int*)m);
        Is_E32* cut = Sort_PartitionByKey(first, last, pv);
        IntroSort_ByKey(cut, last, 0, depth, pred);
        last = cut;
    }
}

// [FUN_45f8d0] the same for pointers by their objects' +504
void __cdecl IntroSort_By504(void** first, void** last, int, int depth, DWORD pred)
{
    while ((((BYTE*)last - (BYTE*)first) & ~3) > 64)
    {
        if (!depth)
        {
            MakeHeap4((DWORD*)first, (DWORD*)last, pred);
            for (BYTE* e = (BYTE*)last; ((e - (BYTE*)first) & ~3) > 4; e -= 4)
                PopHeapLast4((DWORD*)first, (DWORD*)e, pred);
            return;
        }
        depth--;
        int n = (int)(last - first);
        void** m = Sort_MedianBy504(first, first + n / 2, last - 1);
        void* pv;
        Int_Copy((int*)&pv, 0, (const int*)m);
        void** cut = Sort_PartitionBy504(first, last, pv);
        IntroSort_By504(cut, last, 0, depth, pred);
        last = cut;
    }
}

} // extern "C"

// === moving a distance along a polyline (FUN_48d100)

KING_VAR(double, g_pl64b670, 64b670)                  // 1

// the point at fraction t of segment i of points P (24 bytes each), as DVec3_Times / DVec3_Add make it
static void Pl_At(const double* P, int i, double t, double* out)
{
    const double* a = P + 3 * (i + 1);
    const double* b = P + 3 * i;
    double u = g_pl64b670 - t;
    double ax = a[0] * t, ay = a[1] * t, az = a[2] * t;
    double bx = b[0] * u, by = b[1] * u, bz = b[2] * u;
    out[0] = bx + ax;
    out[1] = by + ay;
    out[2] = bz + az;
}

extern "C" {

// [FUN_48d100] polyline s (points +32..+36, 24 bytes each) at segment *idx, fraction *t, moved on by *dist (back
// when negative): whole segments used up move the index (the rest left in *dist, 1 at the end of the line), else
// the fraction advanced and *dist cleared, 0
int __fastcall Polyline_Advance(BYTE* s, void*, double* t, double* dist, int, int* idx)
{
    const double* P = F(const double*, s, 32);
    int n = (int)((F(BYTE*, s, 36) - F(BYTE*, s, 32)) / 24);
    if (*dist > 0)
    {
        while ((unsigned)*idx < (unsigned)(n - 1))
        {
            int i = *idx;
            double cur[3];
            Pl_At(P, i, *t, cur);
            const double* a = P + 3 * (i + 1);
            double dx = a[0] - cur[0], dy = a[1] - cur[1], dz = a[2] - cur[2];
            double len = x87_sqrt(dy * dy + dz * dz + dx * dx);
            if (!(len < *dist))
            {
                const double* b = P + 3 * i;
                double sx = a[0] - b[0], sy = a[1] - b[1], sz = a[2] - b[2];
                *t += *dist / x87_sqrt(sy * sy + sz * sz + sx * sx);
                *dist = 0;
                if (*t > g_pl64b670)
                    *t = 1.0;
                return 0;
            }
            *dist -= len;
            if ((unsigned)*idx >= (unsigned)(n - 2))
            {
                *t = 1.0;
                return 1;
            }
            (*idx)++;
            *t = 0;
        }
        return 1;
    }
    if (!(*dist < 0))
        return 1;
    while (*idx >= 0)
    {
        int i = *idx;
        double cur[3];
        Pl_At(P, i, *t, cur);
        const double* b = P + 3 * i;
        double dx = b[0] - cur[0], dy = b[1] - cur[1], dz = b[2] - cur[2];
        double len = x87_sqrt(dx * dx + dy * dy + dz * dz);
        if (!(len < -*dist))
        {
            const double* a = P + 3 * (i + 1);
            double sx = a[0] - b[0], sy = a[1] - b[1], sz = a[2] - b[2];
            *t += *dist / x87_sqrt(sx * sx + sy * sy + sz * sz);
            *dist = 0;
            if (*t < 0)
                *t = 0;
            return 0;
        }
        *dist += len;
        if (!*idx)
        {
            *t = 0;
            return 1;
        }
        (*idx)--;
        *t = 1.0;
    }
    return 1;
}

} // extern "C"

// === the nearest point of a whole polyline (FUN_48ce40)

extern "C" {

// [FUN_48ce40] polyline s (bounding centre +0, radius +24, points +32..+36, 24 bytes each) checked for a point
// closer to p than *best (squared): the distance, segment and its fraction written out; whether one was found
int __fastcall Polyline_Nearest(BYTE* s, void*, const double* p, double* best, double* tOut, int* idxOut)
{
    double cx = F(double, s, 0) - p[0], cy = F(double, s, 8) - p[1], cz = F(double, s, 16) - p[2];
    if (F(double, s, 24) + *best < x87_sqrt(cy * cy + cz * cz + cx * cx))
        return 0;
    const double* P = F(const double*, s, 32);
    int n = (int)((F(BYTE*, s, 36) - F(BYTE*, s, 32)) / 24);
    double vx = p[0] - P[0], vy = p[1] - P[1], vz = p[2] - P[2];
    double a = vy * vy + vz * vz + vx * vx;
    int found = 0;
    for (unsigned i = 0; i < (unsigned)(n - 1); i++)
    {
        const double* q = P + 3 * (i + 1);
        const double* r = P + 3 * i;
        double wx = p[0] - q[0], wy = p[1] - q[1], wz = p[2] - q[2];
        double b = wy * wy + wz * wz + wx * wx;
        double dx = q[0] - r[0], dy = q[1] - r[1], dz = q[2] - r[2];
        double L2 = dy * dy + dz * dz + dx * dx;
        if (!(b < L2 + a))
        {
            // the start is the closest
            if (*best > a)
            {
                *best = a;
                *idxOut = i;
                found = 1;
                *tOut = 0;
            }
        }
        else if (!(L2 + b > a))
        {
            // the end is
            if (b < *best)
            {
                found = 1;
                *best = b;
                *idxOut = i;
                *tOut = 1.0;
            }
        }
        else
        {
            double dot = dz * vz + dy * vy + dx * vx;
            double sq = dot * dot / L2;
            double d2 = a - sq;
            if (d2 < *best)
            {
                *best = d2;
                found = 1;
                *idxOut = i;
                *tOut = x87_sqrt(sq / L2);
            }
        }
        vx = wx;
        vy = wy;
        vz = wz;
        a = b;
    }
    return found;
}

} // extern "C"

// === the middle one of three road positions by distance (FUN_4644f0)

// |p - x's point| as written out by component (Rec_Set6, then the root)
static double Md_Near(BYTE** x, const DVec3& p)
{
    DVec3 t;
    const DVec3* r = Road_PointOf(&t, *(PositionId*)(*x + 40));
    double dx = p.x - r->x, dy = p.y - r->y, dz = p.z - r->z;
    return x87_sqrt(dy * dy + dz * dz + dx * dx);
}

extern "C" DVec3* __cdecl DVec3_Sub(DVec3* out, const DVec3* a, const DVec3* b);

// |p - x's point| by DVec3_Sub and DVec3_Length
static double Md_Far(BYTE** x, const DVec3& p)
{
    DVec3 t, d;
    return DVec3_Length(DVec3_Sub(&d, &p, Road_PointOf(&t, *(PositionId*)(*x + 40))));
}

extern "C" {

// [FUN_4644f0] of the three holders a, b, c (their road position at *x + 40) the one whose point lies in the
// middle by distance from p
BYTE** __cdecl Road_MiddleOfThree(BYTE** a, BYTE** b, BYTE** c, DVec3 p)
{
    double dA = Md_Near(a, p);
    if (Md_Far(b, p) > dA)
    {
        double dB = Md_Near(b, p);
        if (Md_Far(c, p) > dB)
            return b;
        return Md_Far(c, p) > Md_Far(a, p) ? c : a;
    }
    if (!(Md_Far(c, p) > Md_Far(a, p)))
    {
        double dB = Md_Far(b, p);
        return Md_Far(c, p) > dB ? c : b;
    }
    return a;
}

} // extern "C"

// === holders sorted by their distance from a point (FUN_46add0)

// the distance of e's point (*e + 56) from p, by component (Rec_Set6) then the root
static double Sd_Dist(BYTE** e, const DVec3& p, bool yFirst)
{
    const double* q = (const double*)(*e + 56);
    double x = q[0] - p.x, y = q[1] - p.y, z = q[2] - p.z;
    return yFirst ? x87_sqrt(y * y + z * z + x * x) : x87_sqrt(x * x + y * y + z * z);
}

extern "C" {

// [FUN_46add0] the heap [first, last) of holders sorted (sort_heap, the larger distance from pt first popped)
void __cdecl Holders_SortHeapByDistance(BYTE*** first, BYTE*** last, DVec3 pt)
{
    while (last - first > 1)
    {
        BYTE** val = last[-1];
        int len = (int)(last - first) - 1;
        last--;
        *last = *first;
        int hole = 0, child = 2;
        while (child < len)
        {
            if (Sd_Dist(first[child], pt, false) < Sd_Dist(first[child - 1], pt, true))
                child--;
            first[hole] = first[child];
            hole = child;
            child = 2 * child + 2;
        }
        if (child == len)
        {
            first[hole] = first[child - 1];
            hole = child - 1;
        }
        DVec3 q = pt;
        for (int parent = (hole - 1) / 2; hole > 0; parent = (hole - 1) / 2)
        {
            DVec3 d;
            double dp = Sd_Dist(first[parent], q, false);
            if (!(DVec3_Length(DVec3_Sub(&d, (const DVec3*)(*val + 56), &q)) > dp))
                break;
            first[hole] = first[parent];
            hole = parent;
        }
        first[hole] = val;
    }
}

} // extern "C"

// === holders partitioned by distance (FUN_464880)

// the distance of road position (o + 40) from p, by component then the root
static double Hp_Dist(BYTE* o, const DVec3& p, bool yFirst)
{
    DVec3 t;
    const DVec3* r = Road_PointOf(&t, *(PositionId*)(o + 40));
    double x = p.x - r->x, y = p.y - r->y, z = p.z - r->z;
    return yFirst ? x87_sqrt(y * y + z * z + x * x) : x87_sqrt(x * x + y * y + z * z);
}

extern "C" {

// [FUN_464880] the unguarded partition of [first, last) around pivot by the distance of their road positions from
// pt: the start of the farther part
BYTE** __cdecl Holders_PartitionByDistance(BYTE** first, BYTE** last, BYTE* pivot, DVec3 pt)
{
    for (;;)
    {
        while (Hp_Dist(*first, pt, true) < Hp_Dist(pivot, pt, false))
            first++;
        double dp;
        do
        {
            last--;
            dp = Hp_Dist(pivot, pt, false);
            DVec3 t;
            const DVec3* r = Road_PointOf(&t, *(PositionId*)(*last + 40));
            DVec3 d = {pt.x - r->x, pt.y - r->y, pt.z - r->z};
            if (!(DVec3_Length(&d) > dp))
                break;
        } while (true);
        if (!(first < last))
            return first;
        BYTE* x = *first;
        *first = *last;
        *last = x;
        first++;
    }
}

} // extern "C"

// === updateChasedCars() (a task at $L_432dd0)

extern "C" void Uc_Drop(); KING_ALIAS("_Uc_Drop", "_Ai_DropOrdersForfeit")
KING_VAR(AsmLabel, s_updateChasedCars, 66e0c8)
KING_VAR(double, g_uc66c798, 66c798)

extern "C" {

// [$L_432dd0] updateChasedCars(): every mafia (type 6) or police (type 5) player chasing another (+960, not one
// standing still +92 == 1) drives to it, and catching it (closer than 66c798) stops both for 5 s, drops the
// target's orders and forgets the chase (the mafia's targets cleared, the police's forgotten)
void __cdecl Ai_UpdateChasedCars()
{
    Trace_EnterBuffered(s_updateChasedCars);
    BYTE* head = g_playersHead;
    for (BYTE* it = *(BYTE**)head; it != g_playersHead; it = *(BYTE**)it)
    {
        if (!CallC<BYTE*>(FUN_40a9f0, it))
            continue;
        int type = F(int, CallC<BYTE*>(FUN_40a9f0, it), 84);
        if (type != 6 && type != 5)
            continue;
        BYTE* tgt = F(BYTE*, CallC<BYTE*>(FUN_40a9f0, it), 960);
        if (tgt == g_playersHead || F(int, tgt, 92) == 1)
            continue;
        BYTE sa[24], sb[24];
        const double* a = (const double*)Player_CopyState(sa, tgt);
        const double* b = (const double*)Player_CopyState(sb, it);
        double dist;
        if (type == 6)
        {
            double d[3];                               // as Rec_Set6 stores them
            d[0] = b[0] - a[0];
            d[1] = b[1] - a[1];
            d[2] = b[2] - a[2];
            dist = x87_sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        }
        else
        {
            double x = b[0] - a[0], y = b[1] - a[1], z = b[2] - a[2];
            dist = x87_sqrt(x * x + y * y + z * z);
        }
        if (!(dist < g_uc66c798))
        {
            Player_GoTo(it, *(PositionId*)(tgt + 40), 2);
            continue;
        }
        Player_Call477bd0(it, 0, 0x40140000);
        Player_Call477bd0(tgt, 0, 0x40140000);
        CallC(Uc_Drop, tgt);
        if (type == 6)
            Ai_ClearPlayerFromMafia((int)tgt);
        else
            CallC(FUN_427220, tgt);
    }
    Trace_Leave(s_updateChasedCars);
}

} // extern "C"

// === filling the start grid (a task at $L_4279d0)

KING_VAR(double, g_sg66c6d8, 66c6d8)
KING_VAR(double, g_sg66c6f8, 66c6f8)
KING_VAR(double, g_sg68d170, 68d170)
KING_VAR(double, g_sg64b690, 64b690)
KING_VAR(double, g_sg64b760, 64b760)
KING_VAR(double, g_sg64b5b8, 64b5b8)
KING_VAR(double, g_sg64b808, 64b808)
KING_VAR(double, g_sg64b800, 64b800)
KING_VAR(double, g_sg64b7f8, 64b7f8)
KING_VAR(BYTE**, g_sgNodes, 68cf00)
extern "C" void Sg_Find2(); KING_ALIAS("_Sg_Find2", "_Players_Find457360")
extern "C" void Sg_Cost(); KING_ALIAS("_Sg_Cost", "_Road_PositionCostBest")

static void Sg_Clear(BYTE* b)
{
    memset(b, 0, 24);
}

extern "C" {

// [$L_4279d0] the start: once the wait (state 2) has run its time the waiting ends; state 3 goes back to 0 after
// 64b690; in state 0, with the camera near the race's first start node, every waiting player found gets the next
// start slot ("start%d_%d"), is placed there heading for check_point1 and marked ready (+100 = 5, +96 | 0x4000);
// a full grid (66c6d0) starts the countdown (state 2) and announces the race's length
void __cdecl Ai_FillStartGrid()
{
    if (g_66c6e0 == 2 && AI_Now() - g_sg66c6d8 > g_sg66c6f8)
    {
        AI_EndWaiting();
        return;
    }
    if (g_66c6e0 == 3 && AI_Now() - g_sg68d170 > g_sg64b690)
    {
        g_66c6e0 = 0;
        AI_SetStartKey(1);
    }
    if (g_66c6e0)
        return;
    int first;
    Player_FromId(&first);
    if (first == (int)g_playersHead)
        return;
    char name[100];
    sprintf(name, "start%d_%d", g_66c6e4, 3);
    int node;
    Node_ByName(&node, name);
    const double* np = (const double*)(g_sgNodes[node] + 48);
    double d[3];
    d[0] = g_68bab0.x - np[0];
    d[1] = g_68bab0.y - np[1];
    d[2] = g_68bab0.z - np[2];
    if (x87_sqrt(d[1] * d[1] + d[2] * d[2] + d[0] * d[0]) < g_sg64b760)
        return;
    BYTE buf[24];
    Sg_Clear(buf);
    if (!AI_FewWaiting())
        return;
    Sg_Clear(buf);
    if (!AI_FewWaiting())
        return;
    do
    {
        int p;
        Players_Find4572f0(&p, first, (int)&g_rng68b9c8);
        if (p == (int)g_playersHead)
        {
            CallC(Sg_Find2, &p, first, &g_rng68b9c8);
            if (p == (int)g_playersHead)
                return;
        }
        BYTE* pl = (BYTE*)p;
        Player_ClearOrderLists(pl + 8);
        Player_Retire(pl);
        int n = (int)(g_vec68cf64 - g_vec68cf60);
        F(int, pl, 728) = n - 1;
        char slot[100];
        sprintf(slot, "start%d_%d", g_66c6e4, n);
        int cp, sn;
        Node_ByName(&cp, "check_point1");
        Node_ByName(&sn, slot);
        PositionId at = {-1, sn, 0.0, 0, 0, 0, -1};
        Player_Place(pl, at);
        PositionId to;
        Position_InitNode(&to, 0, cp);
        Player_GoTo(pl, to, 1);
        F(int, pl, 100) = 5;
        F(DWORD, pl, 96) |= 0x4000;
        Player_Call477c30(pl);
        Player_UpdateFlags(pl);
        if ((unsigned)(g_vec68cf64 - g_vec68cf60) >= g_66c6d0)
        {
            g_66c6e0 = 2;
            g_68d168 = 1;
            g_sg66c6d8 = AI_Now();
            int q;
            Player_FromId(&q);
            if (q != (int)g_playersHead)
            {
                PositionId from = *(PositionId*)((BYTE*)q + 40);
                int xn;
                Node_ByName(&xn, slot);
                double cost = 0;
                PositionId dest;
                Position_InitNode(&dest, 0, xn);
                CallC(Sg_Cost, 0, from, dest, &cost);
                double t = cost * g_sg64b5b8;
                if (t > g_sg64b808)
                    t = g_sg64b808;
                int mins = (int)(t * g_sg64b800);
                g_sg66c6f8 = mins * g_sg64b7f8;
                char msg[64];
                sprintf(msg, Message_Text("RACES_TO_TAKE_PLACE_IN"), g_sg66c6f8 * g_sg64b800);
                Messages_Add(msg);
            }
        }
        Sg_Clear(buf);
    } while (AI_FewWaiting());
}

} // extern "C"

// === the median of three road positions by distance (FUN_468d60)

// the point of handle h's position (+40) and its offset from pt, as Rec_Set6 / DVec3_Sub store it
static DVec3 Md_Offset(int h, const DVec3& pt)
{
    DVec3 p;
    Road_PointOf(&p, *(PositionId*)((BYTE*)h + 40));
    DVec3 d;
    d.x = pt.x - p.x;
    d.y = pt.y - p.y;
    d.z = pt.z - p.z;
    return d;
}

// the four ways the original measures it (the sums run in different orders)
static double Md_ZYX(int h, const DVec3& pt)
{
    DVec3 d = Md_Offset(h, pt);
    return x87_sqrt(d.z * d.z + d.y * d.y + d.x * d.x);
}

static double Md_XYZ(int h, const DVec3& pt)
{
    DVec3 d = Md_Offset(h, pt);
    return x87_sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
}

static double Md_Len(int h, const DVec3& pt)
{
    DVec3 d = Md_Offset(h, pt);
    return DVec3_Length(&d);
}

extern "C" {

// [FUN_468d60] of the handles *a, *b, *c the one whose road position lies at the middle distance from pt
int* __cdecl Holders_MedianByDistance(int* a, int* b, int* c, DVec3 pt)
{
    double da = Md_ZYX(*a, pt);
    if (Md_Len(*b, pt) > da)
    {
        double db = Md_ZYX(*b, pt);
        if (Md_Len(*c, pt) > db)
            return b;
        double da2 = Md_Len(*a, pt);
        return Md_Len(*c, pt) > da2 ? c : a;
    }
    double da2 = Md_Len(*a, pt);
    if (Md_Len(*c, pt) > da2)
        return a;
    double db = Md_XYZ(*b, pt);
    return Md_Len(*c, pt) > db ? c : b;
}

} // extern "C"

// === two 1592-byte order records swapped (FUN_469a20)

// the record's parts: all but the padding at +580 and +1588
static void Sw1592_Copy(BYTE* d, const BYTE* s)
{
    memcpy(d, s, 580);
    memcpy(d + 584, s + 584, 1588 - 584);
}

extern "C" {

// [FUN_469a20] records a and b exchanged part by part (std::swap; the copy's element destructors do nothing)
void __cdecl Rec1592_Swap(BYTE* a, BYTE* b, int)
{
    BYTE t[1592];
    memcpy(t, a, 1592);
    Sw1592_Copy(a, b);
    Sw1592_Copy(b, t);
}

} // extern "C"

// === the nearest records to a point brought to the front (FUN_46ab50, std::partial_sort)

extern "C" void __cdecl Far_AdjustHeap2(BYTE** first, int hole, int len, BYTE* value, double x, double y, double z);
struct Ps_Pt { double x, y, z; };
extern "C" void Ps_Pop(); KING_ALIAS("_Ps_Pop", "_PopHeap4")

// the distance of record r's road point from p
static double Ps_Dist(const BYTE* r, const DVec3* p)
{
    DVec3 q, d;
    Road_PointOf(&q, F(PositionId, r, 40));
    DVec3_Sub(&d, p, &q);
    return DVec3_Length(&d);
}

extern "C" {

// [FUN_46ab50] records first .. last (pointers) sorted so first .. middle hold the nearest to p, by the
// distance of their road points (+40): a heap of the first part, each later nearer one swapped in, the heap sorted
void __cdecl Records_PartialSortNear(BYTE** first, BYTE** middle, BYTE** last, int, Ps_Pt p)
{
    int n = (int)(middle - first);
    if (n >= 2)
        for (int parent = (n - 2) / 2;; parent--)
        {
            Far_AdjustHeap2(first, parent, n, first[parent], p.x, p.y, p.z);
            if (!parent)
                break;
        }
    DVec3 pt = {p.x, p.y, p.z};
    for (BYTE** i = middle; i < last; i++)
    {
        BYTE* a = *first;
        BYTE* b = *i;
        DVec3 q;
        Road_PointOf(&q, F(PositionId, b, 40));
        double dx = p.x - q.x, dy = p.y - q.y, dz = p.z - q.z;
        double db = x87_sqrt(dx * dx + dy * dy + dz * dz);
        if (Ps_Dist(a, &pt) > db)
            CallC(Ps_Pop, first, middle, i, *i, p, 0);
    }
    for (BYTE** e = middle; e - first > 1;)
    {
        e--;
        CallC(Ps_Pop, first, e, e, *e, p, 0);
    }
}

} // extern "C"

// === records split around a pivot by their distance from a point (FUN_4691a0, std::__unguarded_partition)

// the distance of record r's road point from p, summed inline as the asm does
static double Ps_DistInline(const BYTE* r, const Ps_Pt& p)
{
    DVec3 q;
    Road_PointOf(&q, F(PositionId, r, 40));
    double dx = p.x - q.x, dy = p.y - q.y, dz = p.z - q.z;
    return x87_sqrt(dx * dx + dy * dy + dz * dz);
}

extern "C" {

// [FUN_4691a0] first .. last partitioned around pivot: the records nearer to p than the pivot before the others;
// where the second part starts
BYTE** __cdecl Records_PartitionNear(BYTE** first, BYTE** last, BYTE* pivot, Ps_Pt p)
{
    DVec3 pt = {p.x, p.y, p.z};
    for (;;)
    {
        for (;;)
        {
            double da = Ps_DistInline(*first, p);
            if (!(Ps_Dist(pivot, &pt) > da))
                break;
            first++;
        }
        for (;;)
        {
            last--;
            double dp = Ps_DistInline(pivot, p);
            double dl = Ps_DistInline(*last, p);
            if (!(dp < dl))
                break;
        }
        if (first >= last)
            return first;
        BYTE* t = *first;
        *first = *last;
        *last = t;
        first++;
    }
}

} // extern "C"

// === the nearest point of a road's curves to a point ($L_454ca0, a vtable entry)

extern "C" void Cn_Nearest(); KING_ALIAS("_Cn_Nearest", "@Curve_NearestTo@36")
extern "C" void Cn_Inside(); KING_ALIAS("_Cn_Inside", "@Polyline_Near@20")

extern "C" {

// [$L_454ca0] road s's curves (1592 bytes each from +56) between (i0, t0) and (i1, t1), taken in order (else
// through vtable +8 swapped): whether one holds a point nearest to (a, b, c) and where (*out, the last such curve)
int __fastcall Road_NearestCurve(BYTE* s, void*, int a, int b, int c, int* out, double t0, int i0, double t1, int i1)
{
    if (i1 < i0 || (i1 == i0 && t1 < t0))
        return CallVirt<int>(s, 8, a, b, c, out, t1, i1, t0, i0);
    BYTE* segs = F(BYTE*, s, 56);
    if (i1 == i0)
    {
        if (!CallThis<int>(Cn_Nearest, segs + i0 * 1592, a, b, c, t0, t1))
            return 0;
        *out = i0;
        return 1;
    }
    int found = 0;
    if (CallThis<int>(Cn_Nearest, segs + i0 * 1592, a, b, c, t0, 1.0))
    {
        found = 1;
        *out = i0;
    }
    for (int i = i0 + 1; i < i1; i++)
        if (CallThis<int>(Cn_Inside, F(BYTE*, s, 56) + i * 1592, a, b, c))
        {
            found = 1;
            *out = i;
        }
    if (CallThis<int>(Cn_Nearest, F(BYTE*, s, 56) + i1 * 1592, a, b, c, 0.0, t1))
    {
        found = 1;
        *out = i1;
    }
    return found;
}

} // extern "C"

// === rope node destructors: a concatenation's (FUN_459c50) and a substring function's (FUN_459e90, with the
//     function base's vtable reset at its inner entry $L_459ff0)

extern "C" void Rd_FreeData(); KING_ALIAS("_Rd_FreeData", "@Str_FreeData@8")
extern "C" void Rd_Fn(); KING_ALIAS("_Rd_Fn", "@Buf459df0_Destruct@8")
KING_VAR(AsmLabel, g_rd64b5b0, 64b5b0)
KING_VAR(AsmLabel, g_rd64b5a8, 64b5a8)

extern "C" void __fastcall RopeSubstr_Destroy(BYTE* p, void*);
extern "C" void __fastcall RopeConcat_Destroy(BYTE* n, void*);

// child c of a concatenation dropped; freed at its last reference
static void Rd_Drop(BYTE* c)
{
    if (InterlockedDecrement((LONG*)(c + 4)))
        return;
    switch ((signed char)F(BYTE, c, 8))
    {
    case 0:
        if (F(void*, c, 20) != F(void*, c, 16))
            CallThis(Rd_FreeData, c);
        Str_FreeRounded(F(void*, c, 20), F(int, c, 0));
        Ai_PoolFree(c, 24);
        break;
    case 1:
        RopeConcat_Destroy(c, 0);
        Ai_PoolFree(c, 28);
        break;
    case 2:
    {
        BYTE* p = c ? c - 4 : 0;
        RopeSubstr_Destroy(p, 0);
        Ai_PoolFree(p, 40);
        break;
    }
    case 3:
        CallThis(Rd_FreeData, c);
        if (F(BYTE, c, 24) && F(BYTE*, c, 20))
            CallVirt(F(BYTE*, c, 20), 0, 1);
        Ai_PoolFree(c, 28);
        break;
    }
}

extern "C" {

// [FUN_459c50] concatenation n: its C string, then both children dropped
void __fastcall RopeConcat_Destroy(BYTE* n, void*)
{
    Rope_FreeCString(n);
    Rd_Drop(F(BYTE*, n, 20));
    Rd_Drop(F(BYTE*, n, 24));
}

// [FUN_459e90] substring function p (40 bytes, the rope function node at +4): its base rope (+32) dropped, then
// the node's C string and, when it owns it, its function
void __fastcall RopeSubstr_Destroy(BYTE* p, void*)
{
    F(void*, p, 0) = &g_rd64b5b0;
    BYTE* b = F(BYTE*, p, 32);
    if (!InterlockedDecrement((LONG*)(b + 4)))
        switch ((signed char)F(BYTE, b, 8))
        {
        case 0:
            if (F(void*, b, 20) != F(void*, b, 16))
                CallThis(Rd_FreeData, b);
            Str_FreeRounded(F(void*, b, 20), F(int, b, 0));
            Ai_PoolFree(b, 24);
            break;
        case 1:
            CallThis(Rd_FreeData, b);
            Rope_Unref(F(BYTE*, b, 20), 0);
            Rope_Unref(F(BYTE*, b, 24), 0);
            Ai_PoolFree(b, 28);
            break;
        case 2:
        {
            BYTE* q = b ? b - 4 : 0;
            RopeSubstr_Destroy(q, 0);
            Ai_PoolFree(q, 40);
            break;
        }
        case 3:
            CallThis(Rd_Fn, b);
            Ai_PoolFree(b, 28);
            break;
        }
    F(void*, p, 0) = &g_rd64b5a8;
    BYTE* r = p ? p + 4 : 0;
    Rope_FreeCString(r);
    if (F(BYTE, r, 24) && F(BYTE*, r, 20))
        CallVirt(F(BYTE*, r, 20), 0, 1);
}

// (the entry $L_459ff0 inside FUN_459e90, reached by jumps from unwind code) p's vtable set to the function
// base's
void __fastcall RopeFn_ResetVtbl(BYTE* p, void*)
{
    F(void*, p, 0) = &g_rd64b5a8;
}

} // extern "C"

// === a road node made (FUN_4709b0)

extern "C" void Rn_Name(); KING_ALIAS("_Rn_Name", "@CharVec_Construct@12")
extern "C" void Rn_Vec(); KING_ALIAS("_Rn_Vec", "@Vec_Construct@12")
extern "C" void Rn_VecCtorEh(); KING_ALIAS("_Rn_VecCtorEh", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" void Rn_VecFree(); KING_ALIAS("_Rn_VecFree", "@Vec4_FreeStorage@8")

// (the entry $L_470ad0 inside FUN_4709b0) an empty vector, the constructor for the two at +20
static BYTE* __fastcall Rn_EmptyVec(BYTE* v, void*)
{
    char alloc;
    return CallThis<BYTE*>(Rn_Vec, v, &alloc);
}

extern "C" {

// [FUN_4709b0] road node s named name: two empty vectors (+20), three frames of doubles (+48, +80, +112; their
// scales 1 at +72, +104, +136), four -1 links (+168), the rest cleared
BYTE* __fastcall RoadNode_Construct(BYTE* s, void*, const char* name)
{
    CallThis(Rn_Name, s, name);
    F(int, s, 12) = 0;
    CallStd(Rn_VecCtorEh, s + 20, 12, 2, (void*)Rn_EmptyVec, (void*)Rn_VecFree);
    memset(s + 48, 0, 156);
    F(DWORD, s, 76) = 0x3FF00000;
    F(DWORD, s, 108) = 0x3FF00000;
    F(DWORD, s, 140) = 0x3FF00000;
    for (int k = 0; k < 4; k++)
        F(int, s, 168 + 4 * k) = -1;
    return s;
}

} // extern "C"

// === a hash table's node for a key (FUN_4592b0)

extern "C" {

// [FUN_4592b0] a 16-byte node from the STL allocator: no next (+0), key (+4: a counted pointer, its count +4
// raised, and a second word)
void** __fastcall Hash_NewNode(void*, void*, const DWORD* key)
{
    void** n = (void**)Stl_Allocate(16);
    n[0] = 0;
    DWORD* k = (DWORD*)(n + 1);
    k[0] = key[0];
    if (key[0])
        InterlockedIncrement((LONG*)(key[0] + 4));
    k[1] = key[1];
    return n;
}

} // extern "C"

// ---------------------------------------------------------------------------
// Batch 33: sorting by road distance

extern "C" {

// [FUN_469c70] unguarded linear insert of v at last: earlier entries farther from point pt (by their road
// position's point, +40) than v move up one
void __cdecl Ai_InsertByDistance(BYTE** last, BYTE* v, DVec3 pt)
{
    BYTE** prev = last - 1;
    for (;;) {
        DVec3 a, b;
        Road_PointOf(&a, *(PositionId*)(v + 40));
        double ax = pt.x - a.x, ay = pt.y - a.y, az = pt.z - a.z;
        Road_PointOf(&b, *(PositionId*)(*prev + 40));
        double bx = pt.x - b.x, by = pt.y - b.y, bz = pt.z - b.z;
        double dv = x87_sqrt((ay * ay + az * az) + ax * ax);
        double dp = x87_sqrt((by * by + bz * bz) + bx * bx);
        if (!(dp > dv))
            break;
        *last = *prev;
        last = prev;
        prev--;
    }
    *last = v;
}

}

// ---------------------------------------------------------------------------
// Batch 33: the AI hash tables

extern "C" void Ht33_LowerBound(); KING_ALIAS("_Ht33_LowerBound", "_U32_LowerBound")
extern "C" void Ht33_AllocFill(); KING_ALIAS("_Ht33_AllocFill", "_Vec4b_AllocFill@12")
extern "C" void Ht33_InsertFill(); KING_ALIAS("_Ht33_InsertFill", "@DwordVec_InsertFill@20")
KING_VAR(unsigned, ht33_primes64b530, 64b530)
KING_VAR(unsigned, ht33_primesEnd64b5a0, 64b5a0)
KING_VAR(unsigned, ht33_lastPrime64b59c, 64b59c)

extern "C" {

// [$L_447d50] hash table constructor for 100 entries: the bucket vector (+4) holds the next prime from the
// table of null buckets, no entries (+16); the three empty functor bytes (+0) take what the asm stack held
BYTE* __fastcall HashTable_Construct(BYTE* self, void*)
{
    F(BYTE, self, 0) = 0;
    F(BYTE, self, 1) = 0;
    F(BYTE, self, 2) = 0;
    BYTE** v = (BYTE**)(self + 4);
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
    F(int, self, 16) = 0;
    unsigned want = 100;
    const unsigned* p = CallC<const unsigned*>(Ht33_LowerBound, &ht33_primes64b530, &ht33_primesEnd64b5a0, &want, 0);
    unsigned n = p == &ht33_primesEnd64b5a0 ? ht33_lastPrime64b59c : *p;
    if ((unsigned)((v[2] - v[0]) >> 2) < n) {
        int used = (int)((v[1] - v[0]) >> 2);
        BYTE* nb = CallStd<BYTE*>(Ht33_AllocFill, n, v[0], v[1]);
        if (int cap = (int)((v[2] - v[0]) >> 2))
            CallC(FUN_44b4a0, v[0], (unsigned)(cap * 4));
        v[0] = nb;
        v[1] = nb + used * 4;
        v[2] = nb + n * 4;
    }
    void* zero = 0;
    CallThis(Ht33_InsertFill, v, v[1], n, &zero);
    F(int, self, 16) = 0;
    return self;
}

}

extern "C" void Ps33_Adjust(); KING_ALIAS("_Ps33_Adjust", "_Heap504_Adjust")
ASM_FUNC(FUN_46ad50, _PopHeapAux4)    // pop heap step (first, last, result, value, cmp, -)

extern "C" {

// [FUN_465fd0] partial sort (first, middle, last, cmp) of records by their double at +504: a heap of
// first..middle, the later entries that differ from the top swapped in, then the heap sorted
void __cdecl Recs504_PartialSort(BYTE** first, BYTE** middle, BYTE** last, int cmp)
{
    int len = (int)(((BYTE*)middle - (BYTE*)first) & ~3);
    if (len >= 8) {
        int n = (int)(middle - first);
        int parent = (n - 2) / 2;
        for (BYTE** p = first + parent;; p--, parent--) {
            CallC(Ps33_Adjust, first, parent, n, *p, cmp);
            if (!parent)
                break;
        }
    }
    for (BYTE** i = middle; i < last; i++) {
        double d = F(double, *i, 504) - F(double, *first, 504);
        if (d < 0.0 || d > 0.0)
            CallC(FUN_46ad50, first, middle, i, *i, cmp, 0);
    }
    if (len > 4) {
        BYTE** e = middle;
        do {
            e--;
            CallC(FUN_46ad50, first, e, e, *e, cmp, 0);
        } while ((((BYTE*)e - (BYTE*)first) & ~3) > 4);
    }
}

}

// [FUN_401000] the AI clock (seconds, double)
extern "C" double __cdecl Ai_Clock() { return t_68a820; }

// [FUN_462340] a temporary buffer of pointers for [first, last) (thiscall):
// +0 the count, +4 the capacity (halved until malloc succeeds), +8 the
// buffer, filled with copies of *first
extern "C" BYTE* __cdecl UninitFill4s(BYTE* d, int n, const DWORD* v);
extern "C" int* __fastcall TempBuf4_Construct(int* self, void*, DWORD* first, DWORD* last)
{
    int n = (int)(last - first);
    self[1] = n;
    self[0] = n;
    self[2] = 0;
    if (self[1] > 536870911)
        self[1] = 536870911;
    while (self[1] > 0 && !(self[2] = (int)malloc(self[1] * 4)))
        self[1] /= 2;
    if (self[1] > 0)
        CallC(FUN_46b110, self[2], self[1], first, last);
    return self;
}

// [FUN_462d20] the STL's length error: the original threw an exception with
// the text at 66f544; exceptions are fatal in this program
KING_VAR(AsmLabel, aw_s66f544, 66f544)
extern "C" void __fastcall Stl_LengthError(void*)
{
    Sys_Fatal(aw_s66f544, 0);
}

// [FUN_45a5e0] rope::_S_tree_concat (left, right): a new concatenation node
// (28 bytes: size, references 1, tag 1, depth = the deeper child + 1),
// rebalanced (462850) when deeper than 20 and short (< 1000) or deeper than 45
extern "C" BYTE* __cdecl Rope_Balance(BYTE* r);
extern "C" BYTE* __cdecl Rope_TreeConcat(BYTE* left, BYTE* right)
{
    BYTE* n = (BYTE*)Stl_Allocate(28);
    if (n) {
        BYTE depth = F(BYTE, left, 13) < F(BYTE, right, 13) ? F(BYTE, right, 13) : F(BYTE, left, 13);
        F(int, n, 0) = F(int, left, 0) + F(int, right, 0);
        F(int, n, 4) = 1;
        F(BYTE, n, 8) = 1;
        F(BYTE, n, 13) = (BYTE)(depth + 1);
        F(BYTE, n, 12) = 0;
        F(int, n, 16) = 0;
        F(BYTE*, n, 20) = left;
        F(BYTE*, n, 24) = right;
    }
    unsigned d = F(BYTE, n, 13);
    if (d <= 20 || (F(unsigned, n, 0) >= 1000 && d <= 45))
        return n;
    BYTE* b = Rope_Balance(n);
    Rope_Unref(n, 0);
    return b;
}

// [FUN_45a2b0] rope::_S_concat_char_iter (r, s, len): r with len characters
// appended; short leaves (and a concatenation's short right leaf) are
// extended in place of a new node
extern "C" BYTE* __cdecl Rope_ConcatCharIter(BYTE* r, const char* s, unsigned len)
{
    if (!len) {
        if (r)
            InterlockedIncrement((LONG*)(r + 4));
        return r;
    }
    if (!r)
        return SchString_FromChars(s, len);
    BYTE tag = F(BYTE, r, 8);
    if (tag == 0 && F(unsigned, r, 0) + len <= 23)
        return SchString_Concat(r, s, len);
    if (tag == 1) {
        BYTE* right = F(BYTE*, r, 24);
        if (!F(BYTE, right, 8) && F(unsigned, right, 0) + len <= 23) {
            BYTE* left = F(BYTE*, r, 20);
            BYTE* nr = SchString_Concat(right, s, len);
            InterlockedIncrement((LONG*)(left + 4));
            return Rope_TreeConcat(left, nr);
        }
    }
    BYTE* nr = SchString_FromChars(s, len);
    InterlockedIncrement((LONG*)(r + 4));
    return Rope_TreeConcat(r, nr);
}

// [FUN_462f30] rope::_S_concat (left, right): a leaf right joins a short leaf
// left (or the short right leaf of a concatenation) in place of a new node
extern "C" BYTE* __cdecl Rope_Concat(BYTE* l, BYTE* r)
{
    if (!l) {
        if (r)
            InterlockedIncrement((LONG*)(r + 4));
        return r;
    }
    if (!r) {
        InterlockedIncrement((LONG*)(l + 4));
        return l;
    }
    if (!F(BYTE, r, 8)) {
        BYTE tag = F(BYTE, l, 8);
        if (tag == 0 && F(unsigned, l, 0) + F(unsigned, r, 0) <= 23)
            return SchString_Concat(l, F(const char*, r, 20), F(unsigned, r, 0));
        if (tag == 1) {
            BYTE* lr = F(BYTE*, l, 24);
            if (!F(BYTE, lr, 8) && F(unsigned, lr, 0) + F(unsigned, r, 0) <= 23) {
                BYTE* ll = F(BYTE*, l, 20);
                BYTE* nr = SchString_Concat(lr, F(const char*, r, 20), F(unsigned, r, 0));
                InterlockedIncrement((LONG*)(ll + 4));
                return Rope_TreeConcat(ll, nr);
            }
        }
    }
    InterlockedIncrement((LONG*)(l + 4));
    InterlockedIncrement((LONG*)(r + 4));
    return Rope_TreeConcat(l, r);
}

// [FUN_462850] rope::_S_balance (r): the leaves go into a forest of 46 slots
// by length, which is joined back from the shortest; deeper than 45 is the
// "rope too long" error (thrown by the original, fatal here)
extern "C" void __cdecl Rope_AddLeafToForest(BYTE* r, BYTE** forest);
extern "C" void __cdecl Tree_EraseRight(BYTE* node, void* x);
KING_VAR(AsmLabel, aw_s66f534, 66f534)
extern "C" BYTE* __cdecl Rope_Balance(BYTE* r)
{
    BYTE* forest[46] = {0};
    if (F(BYTE, r, 12)) {
        Rope_AddLeafToForest(r, forest);
    } else {
        Tree_EraseRight(F(BYTE*, r, 20), forest);
        Tree_EraseRight(F(BYTE*, r, 24), forest);
    }
    BYTE* result = 0;
    for (int i = 0; i <= 45; i++) {
        if (!forest[i])
            continue;
        BYTE* old = result;
        result = Rope_Concat(forest[i], result);
        Rope_Unref(forest[i], 0);
        forest[i] = 0;
        if (old)
            Rope_Unref(old, 0);
    }
    if (F(BYTE, result, 13) > 45)
        Sys_Fatal(aw_s66f534, 0);
    return result;
}

// [$L_41f930] calls the function pointer at +40 with the caller's arguments
extern "C" __declspec(naked) void Ai_CallAt40() { __asm jmp dword ptr [ecx + 40] }

// [$L_4013f0] [$L_401420] static initializer: the object at 68cf48, its
// storage freed at exit ([$L_401430])
KING_VAR(BYTE, g_ai68cf48, 68cf48)
static void __cdecl Ai_Exit401430() { Vec4_FreeStorageB((BYTE**)&g_ai68cf48, 0); }
extern "C" int __cdecl Ai_Init4013f0()
{
    AI_Init68cf48();
    return atexit(Ai_Exit401430);
}

// [$L_4013e0] at exit: destroys the script values at 68a840
KING_VAR(BYTE, g_ai68a840, 68a840)
extern "C" void __cdecl Ai_Exit4013e0() { SchValues_Destroy((BYTE**)&g_ai68a840); }

// [$L_448570] bad_alloc's raise: the original threw a copy; fatal here
extern "C" void __fastcall Ai_BadAllocRaise(void*) { Sys_Fatal("bad allocation", 0); }

// [$L_41f940] a player's present pick (virtual): when the player (+48) is
// valid, picks the present +52 for it
extern "C" void __fastcall Ai_PresentCheck(BYTE* self)
{
    if (F(int, F(BYTE*, self, 48), 88) == 305419896)
        Ai_PickPresent(F(int, self, 48), F(int, self, 52));
}

// [$L_427020] a player's money check (virtual): flag 0x10000 at +96 when its
// money (+136) is negative; a human player in debt gets event 12
KING_VAR(double, g_ai64b678, 64b678)
extern "C" void __fastcall Ai_DebtCheck(BYTE* self)
{
    BYTE* p = F(BYTE*, self, 48);
    if (F(int, p, 88) != 305419896)
        return;
    if (F(double, p, 136) < g_ai64b678)
        F(DWORD, p, 96) |= 65536;
    else
        F(DWORD, p, 96) &= ~65536u;
    if ((F(DWORD, F(BYTE*, self, 48), 96) & 65536) && Player_IsHuman(F(BYTE*, self, 48)))
        Ai_PlayerEvent(F(BYTE*, self, 48), 12, 0);
}

// [$L_401b90] constructs a list (flag +0 = 1, an empty 16-byte head at +4)
extern "C" BYTE* __cdecl Stl_Alloc16();
extern "C" BYTE* __fastcall Ai_ListConstruct(BYTE* self)
{
    self[0] = 1;
    BYTE* n = Stl_Alloc16();
    F(BYTE*, self, 4) = n;
    F(BYTE*, n, 0) = n;
    F(BYTE*, F(BYTE*, self, 4), 4) = F(BYTE*, self, 4);
    return self;
}

// [$L_4019a0] destroys such a list: deletes the values, frees the nodes and the head
extern "C" void __fastcall Ai_ListDestroy(BYTE* self)
{
    BYTE** head = (BYTE**)(self + 4);
    for (BYTE* n = *(BYTE**)*head; n != *head; n = *(BYTE**)n)
        operator delete(F(void*, n, 8));
    for (BYTE* n = *(BYTE**)*head; n != *head;) {
        BYTE* next = *(BYTE**)n;
        Ai_PoolFree(n, 16);
        n = next;
    }
    F(BYTE*, *head, 0) = *head;
    F(BYTE*, *head, 4) = *head;
    Ai_PoolFree(*head, 16);
}

// [$L_401990] at exit: destroys the list at 68b998
extern "C" void __cdecl Ai_Exit401990() { Ai_ListDestroy((BYTE*)&g_init68b998); }

// [FUN_471780] an order's route cost from the position pos to its town
// (+12, the town's node): the best road cost times 66f8a0, at +88
KING_VAR(BYTE**, g_aw68cec0, 68cec0)            // the towns
KING_VAR(double, g_aw66f8a0, 66f8a0)
extern "C" void __cdecl Road_PositionCostBest(int side, PositionId from, PositionId to, double* cost);
extern "C" void __cdecl Order_RouteCost(BYTE* order, PositionId pos)
{
    double cost = 0;
    int town;
    Town_Id(&town, g_aw68cec0[F(int, order, 12)]);
    PositionId node;
    Position_InitNode(&node, 0, town);
    Road_PositionCostBest(0, pos, node, &cost);
    F(double, order, 88) = g_aw66f8a0 * cost;
}
