// ai_world.cpp - Ai_main.cpp of the AI, rewritten from king.masm: its small
// functions: the reference-counted string handle, positions and their
// checks, the nearest node or town to a point, lookups of towns and
// vehicles, the AI's random generator, and the constructors of its records.
//
// Building with  nmake ASM_AI_WORLD=1  makes king.masm assemble the original
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
extern "C" void** __cdecl Towns_Find453d70(void** first, void** last, int a, int b, int c);
extern "C" void** __cdecl Towns_Find453f10(void** first, void** last, int a, int b);
KING_ALIAS("_Towns_Find453f10", "_Pick_RandomOther")
extern "C" void** __cdecl Towns_Find4540a0(void** first, void** last, int* a, int b);
KING_ALIAS("_Towns_Find4540a0", "_Pick_RandomIdle")
extern "C" void** __cdecl Towns_Find454230(void** first, void** last, int a, int b, void* rng);
KING_ALIAS("_Towns_Find454230", "FUN_454230")
extern "C" DVec3* __cdecl Town_Position(DVec3* out, void* town);
KING_ALIAS("_Town_Position", "FUN_404ef0")
extern "C" DVec3* __cdecl Road_PointOf(DVec3* out, PositionId pos);
extern "C" bool __cdecl SchValue_IsSymbol(const BYTE* v, const char* name);   // schitem.cpp
extern "C" void __cdecl Player_UpdateFlags(BYTE* p);        // aiplay.cpp
extern "C" void __fastcall CharVec_Assign(char** self, void*, const char* s);    // aiordr.cpp
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert(pos, first, last)

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

typedef char AsmLabel[1];
ASM_PROC(FUN_44b740)    // the STL node allocator: allocate (bytes)
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
ASM_VAR(BYTE**, g_registry52, $L_68d0d8)
ASM_VAR(BYTE**, g_registry52End, $L_68d0dc)
ASM_VAR(BYTE**, g_registry52Cap, $L_68d0e0)
ASM_VAR(BYTE**, g_registry24, $L_68d0c8)
ASM_VAR(BYTE**, g_registry24End, $L_68d0cc)
ASM_VAR(BYTE**, g_registry24Cap, $L_68d0d0)
ASM_VAR(AsmLabel, s_pair, $L_66cb08)
ASM_VAR(AsmLabel, s_number, $L_66ca60)
ASM_VAR(AsmLabel, s_string, $L_66ca58)
ASM_VAR(AsmLabel, s_port, $L_66cb28)

ASM_PROC(FUN_44b4a0)    // the AI's allocator: free (p, size)
extern "C" void FUN_447970(); KING_ALIAS("_FUN_447970", "@Vec4_FreeStorage@8")
ASM_PROC(FUN_410e40)
ASM_PROC(FUN_466200)    // uninitialized copy of 24-byte records
extern "C" void FUN_4090a0(); KING_ALIAS("_FUN_4090a0", "_Str_Member32")
ASM_PROC(FUN_4a52e0)    // script error (code, fatal)
ASM_PROC(FUN_401000)    // the AI clock, returns double
extern "C" void FUN_4172e0(); KING_ALIAS("_FUN_4172e0", "_Player_UpdateFlagsGet")
extern "C" void FUN_417530(); KING_ALIAS("_FUN_417530", "_Player_Leave")
extern "C" void FUN_483cb0(); KING_ALIAS("_FUN_483cb0", "_displayScreenMessage")    // shows a screen message
ASM_VAR(BYTE*, g_aiMessages, $L_68a850)
ASM_VAR(BYTE*, g_aiMessagesEnd, $L_68a854)
ASM_VAR(AsmLabel, s_noMessage, $L_66cdd8)
ASM_VAR(AsmLabel, g_trace, $L_71e808)
ASM_VAR(BYTE*, g_traceData, $L_71e814)
ASM_VAR(int, g_traceSize, $L_71e810)
ASM_VAR(int, g_tracePos, $L_71e818)
ASM_VAR(AsmLabel, g_tagArt, $L_66c830)
ASM_VAR(AsmLabel, g_tagMpl, $L_66c7ec)
ASM_VAR(AsmLabel, s_finishRacingPlayer, $L_66cc8c)
ASM_VAR(AsmLabel, s_competitorArrived, $L_66cc78)
extern "C" bool __fastcall Item_Matches(BYTE** self, void*, BYTE* x);

extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")    // the object of a list iterator
extern "C" void FUN_427220(); KING_ALIAS("_FUN_427220", "_Players_Forget")
extern "C" void FUN_4566c0(); KING_ALIAS("_FUN_4566c0", "_Player_NotValid")    // whether a player id is invalid
ASM_PROC(FUN_436ef0)
extern "C" void FUN_404c70(); KING_ALIAS("_FUN_404c70", "_Town_ById")    // the index of a VrIndex (out, id)
ASM_PROC(FUN_440970)
ASM_PROC(FUN_441120)
ASM_VAR(BYTE*, g_playersHead, $L_68b990)
ASM_VAR(AsmLabel, s_clearPlayerFromMafia, $L_66da10)
ASM_VAR(AsmLabel, s_clearPlayerFromTail, $L_66da38)
ASM_VAR(AsmLabel, s_getPositionId, $L_66e57c)
ASM_VAR(AsmLabel, s_setPlayerPositionV, $L_66eb28)
ASM_VAR(AsmLabel, s_getTownInfo, $L_66eec8)
ASM_VAR(AsmLabel, s_getOrderInfo, $L_66ef68)
ASM_VAR(AsmLabel, g_numberText, $L_77e660)
ASM_VAR(AsmLabel, s_idFormat, $L_66d4e4)
ASM_VAR(AsmLabel, s_invalidId, $L_66cbf4)
ASM_VAR(AsmLabel, s_playerIdType, $L_66d4d8)
ASM_VAR(BYTE**, g_townsBegin, $L_68cec0)

extern "C" void FUN_4778a0(); KING_ALIAS("_FUN_4778a0", "_Player_ClearOrderLists")
ASM_PROC(FUN_442c60)
extern "C" void FUN_426280(); KING_ALIAS("_FUN_426280", "@Int_Set@12")    // the PlayerId of a list iterator (thiscall out, iterator)
ASM_PROC(FUN_4466a0)    // kills a player
extern "C" void FUN_466400(); KING_ALIAS("_FUN_466400", "_UninitFill16")    // uninitialized fill of 16-byte records
extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")    // uninitialized copy of 12-byte records
ASM_VAR(AsmLabel, s_dropOrders, $L_66f2e0)
ASM_VAR(AsmLabel, s_killDelayedPlayers, $L_66f3f0)

extern "C" void FUN_45aed0(); KING_ALIAS("_FUN_45aed0", "@Vec4b_InsertA@16")    // vector<void*>::insert(pos, &v)
extern "C" void FUN_45c610(); KING_ALIAS("_FUN_45c610", "@Vec4b_InsertB@16")
extern "C" void FUN_45d370(); KING_ALIAS("_FUN_45d370", "@Vec4b_InsertD@16")

extern "C" void FUN_458910(); KING_ALIAS("_FUN_458910", "@AI_Nop458910@8")    // destructor of a 1592-byte record
ASM_PROC(FUN_466300)    // uninitialized copy of 16-byte records
ASM_VAR(AsmLabel, g_vtbl64b600, $L_64b600)
ASM_VAR(AsmLabel, g_vtbl64b62c, $L_64b62c)

extern "C" void FUN_603540(); KING_ALIAS("_FUN_603540", "_Bytes_Copy")
extern "C" void FUN_467780(); KING_ALIAS("_FUN_467780", "_UninitCopyStrRefs")
extern "C" void FUN_4b6610(); KING_ALIAS("_FUN_4b6610", "_SchValue_Same")
extern "C" void FUN_418f70(); KING_ALIAS("_FUN_418f70", "_Player_WaitsInTown")
extern "C" void FUN_45dde0(); KING_ALIAS("_FUN_45dde0", "@Vec4b_InsertE@16")
extern "C" void FUN_4072f0(); KING_ALIAS("_FUN_4072f0", "_Copy1")
extern "C" void FUN_44dd90(); KING_ALIAS("_FUN_44dd90", "@VecChar_Copy@12")
extern "C" void FUN_46b110(); KING_ALIAS("_FUN_46b110", "_UninitFill4s")
ASM_PROC(FUN_462d10)
extern "C" void FUN_40a580(); KING_ALIAS("_FUN_40a580", "@Pair_Set@16")
extern "C" void FUN_4d8a40(); KING_ALIAS("_FUN_4d8a40", "_Vehicle_Exists")

ASM_PROC(FUN_45b190)
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
KING_ALIAS("_AI_42db90", "_AI_FreeList68b99c")
extern "C" void __cdecl AI_4031e0();
KING_ALIAS("_AI_4031e0", "FUN_4031e0")
extern "C" void __cdecl AI_449c40(void* out, void* x);
KING_ALIAS("_AI_449c40", "FUN_449c40")
extern "C" void __cdecl Nodes_FindByName(int* out, const char* name);
KING_ALIAS("_Nodes_FindByName", "FUN_449f00")
extern "C" void __cdecl Modules_FindByName(int* out, const char* name);
KING_ALIAS("_Modules_FindByName", "FUN_44a1c0")
extern "C" void __cdecl Town_440390(void* out, void* town);
KING_ALIAS("_Town_440390", "_Ai_GetCoordinates")
extern "C" void __cdecl List_FreeNode(void* node, int size);
KING_ALIAS("_List_FreeNode", "FUN_44b4a0")
extern "C" void __cdecl Copy104(void* dst, const void* src);
KING_ALIAS("_Copy104", "_Rec40c220_Construct")
extern "C" void __cdecl Copy24a(void* dst, const void* src);
KING_ALIAS("_Copy24a", "_Rec4581f0_Construct")
extern "C" void __cdecl Copy24b(void* dst, const void* src);
KING_ALIAS("_Copy24b", "_Rec40cda0_Construct")
ASM_PROC(FUN_44c470)    // vector<8 bytes>::insert_aux
ASM_PROC(FUN_44da60)    // vector<104 bytes>::insert_aux
extern "C" void FUN_44e3f0(); KING_ALIAS("_FUN_44e3f0", "@Vec4b_Insert44e3f0@16")    // vector<4 bytes>::insert_aux
extern "C" void FUN_44e970(); KING_ALIAS("_FUN_44e970", "@Vec4b_Insert44e970@16")
extern "C" void FUN_44eef0(); KING_ALIAS("_FUN_44eef0", "@Vec4b_Insert44eef0@16")
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
KING_ALIAS("_Copy36", "_Rec458500_Construct")
extern "C" void __cdecl Copy12(void* dst, const void* src);
KING_ALIAS("_Copy12", "_Bytes_Construct")
extern "C" void __cdecl Copy56(void* dst, const void* src);
KING_ALIAS("_Copy56", "_Rec458660_Construct")
extern "C" void __cdecl AI_Error4a54e0(int code, int a, const char* name);
KING_ALIAS("_AI_Error4a54e0", "_Sch_Error")
ASM_PROC(FUN_451230)    // vector<36 bytes>::insert_aux
extern "C" void FUN_4514b0(); KING_ALIAS("_FUN_4514b0", "@Vec4b_Insert4514b0@16")    // vector<4 bytes>::insert_aux
extern "C" void FUN_451740(); KING_ALIAS("_FUN_451740", "@Vec16_Insert451740@16")    // vector<16 bytes>::insert_aux
extern "C" void FUN_451a80(); KING_ALIAS("_FUN_451a80", "@Vec4b_Insert451a80@16")
extern "C" void FUN_451d40(); KING_ALIAS("_FUN_451d40", "@Vec4b_Insert451d40@16")
extern "C" void FUN_452000(); KING_ALIAS("_FUN_452000", "@Vec4b_Insert452000@16")
ASM_PROC(FUN_452610)    // vector<12 bytes>::insert_aux
ASM_PROC(FUN_452bc0)    // vector<56 bytes>::insert_aux
extern "C" void FUN_44b0f0(); KING_ALIAS("_FUN_44b0f0", "_ListNode16_Make@4")    // list: a new node holding x
extern "C" void FUN_44b250(); KING_ALIAS("_FUN_44b250", "_ListNode16_MakeB@4")
extern "C" void FUN_44b310(); KING_ALIAS("_FUN_44b310", "_ListNode64_Make@4")
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
KING_ALIAS("_AI_Text76", "_Message_Text")
extern "C" const char* __cdecl AI_Text40e4c0(const char* key);
KING_ALIAS("_AI_Text40e4c0", "_Message_TextWithSuffix")
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
extern "C" void FUN_452900(); KING_ALIAS("_FUN_452900", "@Vec4b_Insert452900@16")    // vector<void*>::insert(pos, &value)
extern "C" void FUN_458cf0(); KING_ALIAS("_FUN_458cf0", "_Bytes_AllocCopy@12")    // vector<char>: allocate and copy (n, first, last)
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8")
extern "C" void FUN_602ca0(); KING_ALIAS("_FUN_602ca0", "@MsgBuf_FreeStorage@8")
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
extern "C" void FUN_461dc0(); KING_ALIAS("_FUN_461dc0", "_Vec16b_AllocCopy@12")    // vector of 16-byte records: allocate and copy (n, first, last)
extern "C" void FUN_469430(); KING_ALIAS("_FUN_469430", "_Rec16_Copy")    // copy 16-byte records (first, last, dest, -, 0)
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
ASM_PROC(FUN_41ce50)    // destroy a 144-byte order
ASM_PROC(FUN_44fac0)    // hash table: grow for n entries
ASM_PROC(FUN_44f2c0)    // a string's character (s, i)
ASM_PROC(FUN_45a7d0)    // compare two strings
ASM_PROC(FUN_4592b0)    // hash table: a new node for a key
extern "C" void FUN_461b60(); KING_ALIAS("_FUN_461b60", "_Vec24b_AllocCopy@12")    // vector of 24-byte records: allocate and copy (n, first, last)
ASM_PROC(FUN_46c490)    // destroy a 24-byte record
extern "C" void FUN_466c50(); KING_ALIAS("_FUN_466c50", "_Rec24b_Copy")    // copy 24-byte records (first, last, dest, -, 0)
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
ASM_PROC(FUN_41bca0)    // an order's three sums (iterator, &a, &b, &c, cargo record, arg)
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
ASM_PROC(FUN_408080)    // reads the player types of a "Pair" value
ASM_PROC(FUN_4581a0)    // out of memory: retries the allocation
ASM_VAR(int, g_68a860, $L_68a860)
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
ASM_PROC(FUN_409fc0)    // retraceCompetitorRegular(PlayerId player)
ASM_VAR(AsmLabel, g_text77e5e0, $L_77e5e0)

ASM_VAR(BYTE**, g_68cf00, $L_68cf00)
ASM_VAR(int, g_68a968, $L_68a968)
ASM_VAR(int, g_68d01c, $L_68d01c)
ASM_VAR(int, g_68cfe4, $L_68cfe4)
ASM_VAR(int, g_68d0e4, $L_68d0e4)
ASM_VAR(int, g_68cf34, $L_68cf34)
ASM_VAR(unsigned, g_rngI, $L_68a94c)
ASM_VAR(unsigned, g_rngJ, $L_68a950)
typedef DWORD RngTable[55];
ASM_VAR(RngTable, g_rng68a870, $L_68a870)
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

ASM_PROC(FUN_43bf10)    // a player's next position along its route
ASM_PROC(FUN_44c160)    // vector<int>::operator=
ASM_VAR(AsmLabel, g_68b99c, $L_68b99c)
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

ASM_PROC(FUN_466300)    // uninitialized copy of 16-byte records

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
ASM_VAR(BYTE*, g_68ccbc, $L_68ccbc)
ASM_VAR(AsmLabel, g_text77e460, $L_77e460)

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

ASM_PROC(FUN_41ce50)    // 144-byte record destructor
extern "C" void FUN_474160(); KING_ALIAS("_FUN_474160", "@Player_SetPlace@12")    // player: set image (index)
ASM_VAR(BYTE*, g_images68a958, $L_68a958)
ASM_VAR(BYTE*, g_images68a95c, $L_68a95c)

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
ASM_PROC(FUN_45cf80)    // random shuffle of pointers

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

ASM_PROC(FUN_436900)    // getPositionId(vrVector)
ASM_PROC(FUN_436bd0)    // getPositionId(RoomId, vrVector)
ASM_VAR(char, s_getPositionIdV, $L_66e538)       // "getPositionId(vrVector _v)"
ASM_VAR(char, s_getPositionIdRV, $L_66e554)      // "getPositionId(RoomId room, vrVector _v)"
ASM_VAR(BYTE*, g_roomsBegin68d0d8, $L_68d0d8)
ASM_VAR(BYTE*, g_roomsEnd68d0dc, $L_68d0dc)

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

ASM_VAR(char, s_querySpeedLimit, $L_66dcf0)      // "querySpeedLimit(PlayerId player)"
ASM_VAR(char, s__getTangent, $L_66e640)          // "_getTangent(PositionId pos)"
ASM_VAR(char, s_getTangent, $L_66e65c)           // "getTangent(PositionId pos)"
ASM_VAR(char, s_getCoordinatesPos, $L_66e678)       // "getCoordinates(PositionId pos)"
ASM_VAR(char, s_findOnTrajectory, $L_66ea1c)     // "findPositionOnATrajectory(PlayerId player, vrVector const & v)"
ASM_VAR(BYTE**, g_limits68a970, $L_68a970)
ASM_VAR(BYTE**, g_limits68a974, $L_68a974)
ASM_VAR(const double, k_noSpeedLimit, $L_64b880)
ASM_PROC(FUN_43d1f0)

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
ASM_VAR(char, s_getPlayerVirtualCapital, $L_66db68)  // "getPlayerVirtualCapital(PlayerId player)"
ASM_VAR(char, s_getPlayerCapital, $L_66db94)         // "getPlayerCapital(PlayerId player)"

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

ASM_VAR(BYTE*, g_anchors68cf58, $L_68cf58)
ASM_VAR(BYTE**, g_roadNodesCap68cf08, $L_68cf08)
ASM_VAR(char, s_anode, $L_66c920)                    // "anode"
ASM_PROC(FUN_4709b0)    // a road node's constructor (name)
ASM_PROC(FUN_405c00)
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
ASM_PROC(FUN_486800)    // the route length between two positions
ASM_VAR(char, s_updateAdvancementRegular, $L_66d508) // "updateAdvancementRegular(AI_Player * p, PositionId position)"

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
ASM_VAR(double, g_camX68bab0, $L_68bab0)
ASM_VAR(double, g_camY68bab8, $L_68bab8)
ASM_VAR(double, g_camZ68bac0, $L_68bac0)

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
ASM_PROC(FUN_438d70)    // the room info of a position
ASM_VAR(char, s_undefinedModule, $L_66d638)
ASM_VAR(char, s_undefinedRoom, $L_66d628)

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
ASM_PROC(FUN_4484d0)
extern "C" void FUN_448cd0(); KING_ALIAS("_FUN_448cd0", "@Vec4b_Construct@20")
extern "C" void FUN_449680(); KING_ALIAS("_FUN_449680", "@List64_Construct@12")
extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")
ASM_PROC(FUN_4ad440)
ASM_PROC($L_401b90)
ASM_PROC($L_447d50)
ASM_VAR(char, g_init68a830, $L_68a830)
ASM_VAR(char, g_init68a840, $L_68a840)
ASM_VAR(char, g_init68a850, $L_68a850)
ASM_VAR(char, g_init68a860, $L_68a860)
ASM_VAR(char, g_init68a958, $L_68a958)
ASM_VAR(char, g_init68a970, $L_68a970)
ASM_VAR(char, g_init68a980, $L_68a980)
ASM_VAR(char, g_init68b990, $L_68b990)
ASM_VAR(char, g_init68b998, $L_68b998)
ASM_VAR(char, g_init68b9b8, $L_68b9b8)
ASM_VAR(char, g_init68bac8, $L_68bac8)
ASM_VAR(char, g_init68bb58, $L_68bb58)
ASM_VAR(char, g_init68cbe8, $L_68cbe8)
ASM_VAR(char, g_init68cc08, $L_68cc08)
ASM_VAR(char, g_init68cc18, $L_68cc18)
ASM_VAR(char, g_init68cc88, $L_68cc88)
ASM_VAR(char, g_init68cc98, $L_68cc98)
ASM_VAR(char, g_init68cca8, $L_68cca8)
ASM_VAR(char, g_init68ccb8, $L_68ccb8)
ASM_VAR(char, g_init68ceb0, $L_68ceb0)
ASM_VAR(char, g_init68cec0, $L_68cec0)
ASM_VAR(char, g_init68cf00, $L_68cf00)
ASM_VAR(char, g_init68cf10, $L_68cf10)
ASM_VAR(char, g_init68cf28, $L_68cf28)
ASM_VAR(char, g_init68cf38, $L_68cf38)
ASM_VAR(char, g_init68cf48, $L_68cf48)
ASM_VAR(char, g_init68cf58, $L_68cf58)
ASM_VAR(char, g_init68cf60, $L_68cf60)
ASM_VAR(char, g_init68cf6c, $L_68cf6c)
ASM_VAR(char, g_init68cfd8, $L_68cfd8)
ASM_VAR(char, g_init68cff0, $L_68cff0)
ASM_VAR(char, g_init68d008, $L_68d008)
ASM_VAR(char, g_init68d0a8, $L_68d0a8)
ASM_VAR(char, g_init68d0b8, $L_68d0b8)
ASM_VAR(char, g_init68d0c8, $L_68d0c8)
ASM_VAR(char, g_init68d0d8, $L_68d0d8)

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
ASM_VAR(int, g_stlLock68d204, $L_68d204)

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

ASM_VAR(AsmLabel, s_recIdType66d64c, $L_66d64c)
ASM_VAR(AsmLabel, s_suffix66c4e4, $L_66c4e4)
ASM_VAR(AsmLabel, s_fmt66d9c8, $L_66d9c8)
ASM_VAR(AsmLabel, s_fmt66d9c4, $L_66d9c4)
ASM_VAR(AsmLabel, s_fmt66daa8, $L_66daa8)
ASM_VAR(AsmLabel, s_prefix66c7ec, $L_66c7ec)
ASM_VAR(char, g_text68cdac, $L_68cdac)
ASM_VAR(double, g_time68d1b8, $L_68d1b8)
ASM_VAR(void**, g_list68b99c, $L_68b99c)
extern "C" void FUN_477210(); KING_ALIAS("_FUN_477210", "_Player_UpdateFlags")    // updates a player's flags
extern "C" void FUN_56ae60(); KING_ALIAS("_FUN_56ae60", "_Messages_Add")    // adds a message to the message list
ASM_PROC(FUN_403940)

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
ASM_VAR(BYTE*, g_places68a958b, $L_68a958)
ASM_VAR(BYTE*, g_players68b990b, $L_68b990)

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
ASM_VAR(BYTE**, g_vec68cf60, $L_68cf60)
ASM_VAR(BYTE**, g_vec68cf64, $L_68cf64)
ASM_VAR(BYTE**, g_vec68cf68, $L_68cf68)
ASM_VAR(BYTE**, g_vec68a980, $L_68a980)
ASM_VAR(BYTE**, g_vec68a984, $L_68a984)
ASM_VAR(BYTE**, g_vec68a988, $L_68a988)
ASM_VAR(BYTE*, g_recs68ccb8, $L_68ccb8)
ASM_VAR(BYTE*, g_recs68ccbc, $L_68ccbc)
ASM_VAR(char, g_rand68b9c8, $L_68b9c8)
ASM_VAR(AsmLabel, s_fmt66d58c, $L_66d58c)
ASM_VAR(char, g_empty68d1c0, $L_68d1c0)

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

ASM_PROC(FUN_4015a0)
extern "C" void FUN_457b50(); KING_ALIAS("_FUN_457b50", "_CopyDwords")    // copies dwords (first, last, dest)
ASM_PROC(FUN_409fc0)
extern "C" void FUN_42b060(); KING_ALIAS("_FUN_42b060", "_Rec_SetActive")    // marks a record (in)active
extern "C" void FUN_484280(); KING_ALIAS("_FUN_484280", "_Road_ResetSides")    // resets the road sides
extern "C" void FUN_4bd510(); KING_ALIAS("_FUN_4bd510", "@Fx_Self@8")
extern "C" void FUN_455950(); KING_ALIAS("_FUN_455950", "@Rec_Get1@8")
extern "C" void FUN_455940(); KING_ALIAS("_FUN_455940", "@Rec_Get0@8")
extern "C" void FUN_40a1d0(); KING_ALIAS("_FUN_40a1d0", "_Player_CheckRouteStart")    // checks a player's route start
extern "C" void FUN_425610(); KING_ALIAS("_FUN_425610", "_AI_RandomTraffic")    // random traffic
ASM_VAR(int, g_count68d164, $L_68d164)
typedef void (__cdecl* PresentCb)(int, int);
ASM_VAR(PresentCb, g_presentCb68d11c, $L_68d11c)
ASM_VAR(int*, g_edges68cf38, $L_68cf38)
ASM_VAR(BYTE**, g_recs68cc18b, $L_68cc18)
ASM_VAR(BYTE**, g_recs68cc1cb, $L_68cc1c)

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

ASM_VAR(char, s_type66ce4c, $L_66ce4c)
ASM_PROC(FUN_466300)    // copy-constructs 16-byte records (first, last, dest, -)
ASM_PROC(FUN_466200)    // copy-constructs 24-byte records (first, last, dest, -)
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

ASM_VAR(char, vt_64b62c, $L_64b62c)

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
// copy is the instance's uninitialized copy (first, last, dest, -)
extern "C" void __cdecl VecPod_InsertAux(BYTE** v, BYTE* pos, const BYTE* x, int n, void (*copy)())
{
    if (v[1] != v[2]) {
        if (v[1])
            memcpy(v[1], v[1] - n, n);
        v[1] += n;
        BYTE t[16];
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
    BYTE* q = CallC<BYTE*>(copy, v[0], pos, p, pos);
    if (q)
        memcpy(q, x, n);
    BYTE* e = CallC<BYTE*>(copy, pos, v[1], q + n, pos);
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
