// road_graph.cpp - ai_road.cpp of the AI, rewritten from king.masm: the ids of
// the road graph (NodeId, RoadId, PositionId) with their checks, and lookups
// of the roads between nodes.

#include "road.h"

typedef char AiText[1];
KING_VAR(AiText, g_idText, 77e460)           // "(%d)" of an invalid id
KING_VAR(AiText, g_positionText, 77e6e0)     // an invalid PositionId

static const char s_roadId[] = "$Id: ai_road.cpp 2000/8/29 17:19:18";
static const char s_roadFile[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\Ai_road.cpp";

static bool ValidNode(int node)
{
    return node >= 0 && (unsigned)node < (unsigned)(g_roadNodesEnd - g_roadNodes);
}

static bool ValidRoad(int road)
{
    return road >= 0 && (unsigned)road < (unsigned)(g_roadEdgesEnd - g_roadEdges);
}

static void InvalidId(int line, const char* kind, int id)
{
    sprintf(g_idText, "(%d)", id);
    AiError(33, 100, s_roadFile, s_roadId, line, "Invalid id", kind, g_idText);
}

static void InvalidPosition(int line, const PositionId* p)
{
    sprintf(g_positionText, "(%d %g %d %d %x)", p->road, p->t, p->segment, p->object, p->m_24);
    AiError(33, 100, s_roadFile, s_roadId, line, "Invalid id", "PositionId", g_positionText);
}

static void InitPosition(PositionId* p, int road, int node, double t)
{
    p->road = road;
    p->node = node;
    p->t = t;
    p->segment = 0;
    p->object = 0;
    p->m_24 = 0;
    p->m_28 = -1;
}

// the end of an edge: the last segment of its last object
static void SetToEnd(PositionId* p, RoadEdge* e)
{
    int objects = e->objectsEnd - e->objects;
    p->segment = e->objects[objects - 1]->SegmentCount() - 1;
    p->object = objects - 1;
}

// the node two roads share, -1 if none
extern "C" int* __cdecl Road_CommonNode(int* out, int roadA, int roadB)
{
    if (roadA != -1 && roadB != -1) {
        RoadEdge* a = g_roadEdges[roadA];
        RoadEdge* b = g_roadEdges[roadB];
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                if ((&a->from)[i] == (&b->from)[j]) {
                    *out = (&a->from)[i]->id;
                    return out;
                }
    }
    *out = -1;
    return out;
}

extern "C" PositionId* __cdecl Road_EdgeStart(PositionId* out, RoadEdge* e)
{
    PositionId p;
    InitPosition(&p, e->id, e->from->id, 0);
    if (!ValidNode(p.node))
        InvalidId(38, "NodeId", p.node);
    if (!ValidRoad(p.road) && !ValidNode(p.node))
        InvalidPosition(39, &p);
    *out = p;
    return out;
}

extern "C" PositionId* __cdecl Road_EdgeEnd(PositionId* out, RoadEdge* e)
{
    PositionId p;
    InitPosition(&p, e->id, 0, 1.0);
    SetToEnd(&p, e);
    p.node = e->to->id;
    if ((!ValidRoad(p.road) && !ValidNode(p.node)) || p.segment < 0 || p.object < 0)
        InvalidPosition(47, &p);
    *out = p;
    return out;
}

extern "C" PositionId* __cdecl Road_StartOf(PositionId* out, int road)
{
    if (!ValidRoad(road))
        InvalidId(54, "RoadId", road);
    PositionId p;
    InitPosition(&p, road, g_roadEdges[road]->from->id, 0);
    if (!ValidNode(p.node))
        InvalidId(57, "NodeId", p.node);
    if (!ValidRoad(road) && !ValidNode(p.node))
        InvalidPosition(58, &p);
    *out = p;
    return out;
}

extern "C" PositionId* __cdecl Road_EndOf(PositionId* out, int road)
{
    if (!ValidRoad(road))
        InvalidId(64, "RoadId", road);
    PositionId p;
    InitPosition(&p, road, 0, 1.0);
    SetToEnd(&p, g_roadEdges[road]);
    p.node = g_roadEdges[road]->to->id;
    if ((!ValidRoad(road) && !ValidNode(p.node)) || p.segment < 0 || p.object < 0)
        InvalidPosition(67, &p);
    *out = p;
    return out;
}

// the road joining two nodes, either way; -1 and an error if none
extern "C" int* __cdecl Road_Between(int* out, int nodeA, int nodeB)
{
    if (!ValidNode(nodeA))
        InvalidId(103, "NodeId", nodeA);
    if (!ValidNode(nodeB))
        InvalidId(104, "NodeId", nodeB);
    RoadNode* a = g_roadNodes[nodeA];
    RoadNode* b = g_roadNodes[nodeB];
    for (RoadEdge** e = g_roadEdges; e != g_roadEdgesEnd; e++)
        if (((*e)->from == a && (*e)->to == b) || ((*e)->to == a && (*e)->from == b)) {
            *out = e - g_roadEdges;
            return out;
        }
    const char* nameB = Str(&b->name);
    const char* nameA = Str(&a->name);
    AiError(0, 1, s_roadFile, s_roadId, 113, "Can node find road connecting", nameA, nameB);
    *out = -1;
    return out;
}

// ---------------------------------------------------------------------------
// Instances of the STL's templates for the AI's types

template <int N> struct Blob { DWORD w[N / 4]; };
typedef Blob<8> Item8;
typedef Blob<12> Item12;
typedef Blob<16> Item16;
typedef Blob<40> Item40;

template <class T> static T* UninitializedCopy(const T* first, const T* last, T* dest)
{
    for (; first != last; first++, dest++)
        if (dest)
            *dest = *first;
    return dest;
}

extern "C" Item40* __cdecl Stl_UninitCopy40(const Item40* f, const Item40* l, Item40* d) { return UninitializedCopy(f, l, d); }
extern "C" Item8* __cdecl Stl_UninitCopy8(const Item8* f, const Item8* l, Item8* d) { return UninitializedCopy(f, l, d); }
extern "C" Item16* __cdecl Stl_UninitCopy16(const Item16* f, const Item16* l, Item16* d) { return UninitializedCopy(f, l, d); }
extern "C" Item12* __cdecl Stl_UninitCopy12(const Item12* f, const Item12* l, Item12* d) { return UninitializedCopy(f, l, d); }

extern "C" Item8* __cdecl Stl_Copy8(const Item8* first, const Item8* last, Item8* dest)
{
    for (int n = last - first; n > 0; n--)
        *dest++ = *first++;
    return dest;
}

extern "C" Item40* __cdecl Stl_CopyBackward40(const Item40* first, const Item40* last, Item40* destEnd)
{
    for (int n = last - first; n > 0; n--)
        *--destEnd = *--last;
    return destEnd;
}

extern "C" Item40* __cdecl Stl_UninitFill40(Item40* first, int n, const Item40* value)
{
    for (; n > 0; n--, first++)
        if (first)
            *first = *value;
    return first;
}

// a node of the AI's lists
struct AiListNode {
    AiListNode* next;
    AiListNode* prev;
    BYTE        m_8[84];
    int         kind;               // +92
    DWORD       flags;              // +96
};

// the first node from first on that is of kind 1 without flag 0x100
extern "C" AiListNode** __cdecl AiList_FindActive(AiListNode** out, AiListNode* first, AiListNode* last)
{
    AiListNode* n = first;
    for (; n != last; n = n->next)
        if (n->kind == 1 && !(n->flags & 0x100))
            break;
    *out = n;
    return out;
}

extern "C" void FUN_44bef0(); KING_ALIAS("_FUN_44bef0", "@VecChar_Insert44bef0@16")    // vector<char>::insert_aux(pos, x)
extern "C" void FUN_47b790(); KING_ALIAS("_FUN_47b790", "@Vec12_Insert47b790@16")    // vector<12 bytes>::insert_aux(pos, x)
extern "C" void FUN_478fb0(); KING_ALIAS("_FUN_478fb0", "_List1000_MakeNode@4")    // list: a new node holding x
extern "C" void FUN_477600(); KING_ALIAS("_FUN_477600", "_PlayerRef_Resolve")
extern "C" void FUN_4731e0(); KING_ALIAS("_FUN_4731e0", "@Player_Destroy@4")
extern "C" void FUN_479050(); KING_ALIAS("_FUN_479050", "@Vec40_Insert479050@16")
extern "C" void FUN_47c360(); KING_ALIAS("_FUN_47c360", "_Rec47c360_Construct")
extern "C" void FUN_479520(); KING_ALIAS("_FUN_479520", "@KeyedVecs_InsertAux@16")
extern "C" void FUN_479760(); KING_ALIAS("_FUN_479760", "@Vec8_Insert479760@16")
extern "C" void FUN_47a230(); KING_ALIAS("_FUN_47a230", "@PlayerRec_Copy@12")
extern "C" void FUN_4581a0(); KING_ALIAS("_FUN_4581a0", "_Ai_OperatorNew")
extern "C" void FUN_47c070(); KING_ALIAS("_FUN_47c070", "_AiList_FindActive")
extern "C" void FUN_47b530(); KING_ALIAS("_FUN_47b530", "_Alloc8_Copy@12")
KING_VAR(BYTE*, g_68cf6c, 68cf6c)
KING_VAR(BYTE**, g_68a970, 68a970)
KING_VAR(BYTE**, g_68a974, 68a974)
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")
extern "C" void FUN_458cf0(); KING_ALIAS("_FUN_458cf0", "_Bytes_AllocCopy@12")
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

struct VecOf {
    BYTE* begin;
    BYTE* end;
    BYTE* capacityEnd;
};

extern "C" void __fastcall CharVec_PushBack(VecOf* v, void*, const char* x)
{
    if (v->end != v->capacityEnd) {
        if (v->end)
            *v->end = *x;
        v->end++;
    } else
        CallThis(FUN_44bef0, v, v->end, x);
}

extern "C" void __fastcall Vec12_PushBack(VecOf* v, void*, const Item12* x)
{
    if (v->end != v->capacityEnd) {
        if (v->end)
            *(Item12*)v->end = *x;
        v->end += 12;
    } else
        CallThis(FUN_47b790, v, v->end, x);
}

extern "C" AiListNode** __fastcall AiList_Insert(void* list, void*, AiListNode** out, AiListNode* pos, const void* x)
{
    AiListNode* n = CallThis<AiListNode*>(FUN_478fb0, list, x);
    n->next = pos;
    n->prev = pos->prev;
    pos->prev->next = n;
    pos->prev = n;
    *out = n;
    return out;
}

// ---------------------------------------------------------------------------
// A random position on the roads

// the STL's subtractive random generator of the AI
typedef unsigned RngTable[55];
KING_VAR(RngTable, g_rngTable, 68b9c8)
KING_VAR(unsigned, g_rngIndex1, 68baa4)
KING_VAR(unsigned, g_rngIndex2, 68baa8)

static unsigned AiRandom(unsigned limit)
{
    g_rngIndex1 = (g_rngIndex1 + 1) % 55;
    g_rngIndex2 = (g_rngIndex2 + 1) % 55;
    g_rngTable[g_rngIndex1] -= g_rngTable[g_rngIndex2];
    return g_rngTable[g_rngIndex1] % limit;
}

struct RoomInfo {
    DWORD       flags;              // 8: the position is a place to stop
    DWORD       m_4;
    DWORD       m_8, m_12;
    const char* module;
    const char* room;
    DWORD       m_24, m_28, m_32;
    int         m_36, m_40;
    DWORD       m_44;
};

KING_VAR(AiText, g_undefinedModule, 66d638)   // "UNDEFINED_MODULE"
KING_VAR(AiText, g_undefinedRoom, 66d628)     // "UNDEFINED_ROOM"

extern "C" PositionId* __cdecl Road_Advance(PositionId* out, PositionId pos, double* distance, int);
KING_ALIAS("_Road_Advance", "_Ai_AdvancePositionRoad")
extern "C" void __cdecl Road_RoomInfo(PositionId pos, RoomInfo* info);
KING_ALIAS("_Road_RoomInfo", "_Road_GetRoadParameters")

#define EDGE_DOUBLE(e, off) (*(double*)((BYTE*)(e) + (off)))   // +24 length, +40 total up to its end

static PositionId* Nowhere(PositionId* out)
{
    out->road = -1;
    out->node = -1;
    out->t = 0;
    out->segment = 0;
    out->object = 0;
    out->m_24 = 0;
    out->m_28 = -1;
    return out;
}

extern "C" PositionId* __cdecl Road_RandomPosition(PositionId* out)
{
    if (g_roadEdges == g_roadEdgesEnd)
        return Nowhere(out);
    int range = (int)(EDGE_DOUBLE(g_roadEdgesEnd[-1], 40) * 100.0);
    unsigned r = AiRandom(range);
    double x = (double)r * 0.01 - 1.0;
    if (!(x >= 0))
        x = 0;
    RoadEdge** e = g_roadEdges;
    while (x >= EDGE_DOUBLE(*e, 40)) {
        if (++e == g_roadEdgesEnd)
            return Nowhere(out);
    }
    double along = (x - EDGE_DOUBLE(*e, 40)) + EDGE_DOUBLE(*e, 24);
    PositionId pos;
    Road_EdgeStart(&pos, *e);
    if (!!(along >= 0)) {
        PositionId moved;
        pos = *Road_Advance(&moved, pos, &along, 0);
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
        Road_RoomInfo(pos, &info);
        if (info.flags & 8)
            pos.m_24 |= 1;
    }
    *out = pos;
    return out;
}

// --- STL helpers of the road graph and the route lists

extern "C" void __cdecl Route_Each477600(BYTE* x);   // ai_drivers.cpp

extern "C" {

// [FUN_478910] calls Route_Each477600 on every route of the list
void __cdecl Routes_Each477600()
{
    for (BYTE* n = *(BYTE**)g_68cf6c; n != g_68cf6c; n = *(BYTE**)n)
        Route_Each477600(F(BYTE*, n, 8));
}

// [FUN_478940]
int __cdecl Ambush_Call477600(BYTE* a)
{
    return CallC<int>(FUN_477600, a + 124);
}

// [FUN_478950] calls Ambush_Call477600 on every ambush
void __cdecl Ambushes_Each477600()
{
    for (BYTE** it = g_68a970; it != g_68a974; it++)
        Ambush_Call477600(*it);
}

// [FUN_478980] list erase (1000-byte nodes): *out = the next node
BYTE** __fastcall List1000_Erase(void* self, void*, BYTE** out, BYTE* node)
{
    BYTE* prev = F(BYTE*, node, 4);
    BYTE* next = F(BYTE*, node, 0);
    F(BYTE*, prev, 0) = next;
    F(BYTE*, next, 4) = prev;
    CallThis(FUN_4731e0, node + 8);
    CallC(FUN_44b4a0, node, 1000);
    *out = next;
    return out;
}

// [FUN_478fb0] a new 1000-byte list node holding x
BYTE* __stdcall List1000_MakeNode(int x)
{
    BYTE* p = (BYTE*)malloc(1000);
    if (!p)
        p = CallC<BYTE*>(FUN_4581a0, 1000);
    if (p + 8)
        CallThis(FUN_47a230, p + 8, x);
    return p;
}

// [FUN_4789c0] vector<40 bytes>::push_back
void __fastcall Vec40_PushBack(BYTE** self, void*, const BYTE* x)
{
    BYTE* e = self[1];
    if (e == self[2]) {
        CallThis(FUN_479050, self, e, x);
        return;
    }
    if (e)
        memcpy(e, x, 40);
    self[1] += 40;
}

// [FUN_478a00] vector<40 bytes>::clear
void __fastcall Vec40_Clear(BYTE** self, void*)
{
    int n = (self[1] - self[0]) / 40;
    self[1] -= n * 40;
}

// [FUN_478a80] vector<16 bytes>::push_back
void __fastcall Vec16_PushBackB(BYTE** self, void*, const BYTE* x)
{
    BYTE* e = self[1];
    if (e == self[2]) {
        CallThis(FUN_479520, self, e, x);
        return;
    }
    CallC(FUN_47c360, e, x);
    self[1] += 16;
}

// [FUN_478de0] vector<8 bytes>::push_back
void __fastcall Vec8_PushBackB(BYTE** self, void*, const DWORD* x)
{
    BYTE* e = self[1];
    if (e == self[2]) {
        CallThis(FUN_479760, self, e, x);
        return;
    }
    if (e) {
        F(DWORD, e, 0) = x[0];
        F(DWORD, e, 4) = x[1];
    }
    self[1] += 8;
}

// [FUN_47b4e0] vector<8 bytes>::erase(first, last)
BYTE* __fastcall Vec8_Erase(BYTE** self, void*, BYTE* first, BYTE* last)
{
    BYTE* d = first;
    BYTE* s = last;
    for (int n = (self[1] - last) >> 3; n > 0; n--, d += 8, s += 8) {
        F(DWORD, d, 0) = F(DWORD, s, 0);
        F(DWORD, d, 4) = F(DWORD, s, 4);
    }
    self[1] -= ((last - first) >> 3) << 3;
    return first;
}

// [FUN_47aad0] vector<char>::reserve(n)
void __fastcall Bytes_Reserve(BYTE** self, void*, unsigned n)
{
    BYTE* b = self[0];
    if ((unsigned)(self[2] - b) >= n)
        return;
    int size = self[1] - b;
    BYTE* p = CallStd<BYTE*>(FUN_458cf0, n, b, self[1]);
    if (self[2] != self[0])
        CallC(FUN_44b4a0, self[0], self[2] - self[0]);
    self[0] = p;
    self[2] = p + n;
    self[1] = p + size;
}

// [FUN_47afc0] vector<8 bytes>::reserve(n)
void __fastcall Vec8_Reserve(BYTE** self, void*, unsigned n)
{
    if ((unsigned)((self[2] - self[0]) >> 3) >= n)
        return;
    int size = (self[1] - self[0]) >> 3;
    BYTE* p = CallStd<BYTE*>(FUN_47b530, n, self[0], self[1]);
    if ((self[2] - self[0]) >> 3)
        CallC(FUN_44b4a0, self[0], ((self[2] - self[0]) >> 3) * 8);
    self[0] = p;
    self[1] = p + size * 8;
    self[2] = p + n * 8;
}

// [FUN_47a140] copy constructor: a word and a vector<char>
BYTE* __fastcall Rec47a140_Copy(BYTE* self, void*, const BYTE* s)
{
    F(DWORD, self, 0) = F(DWORD, s, 0);
    const BYTE* b = F(const BYTE*, s, 4);
    const BYTE* e = F(const BYTE*, s, 8);
    unsigned n = e - b;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n) : 0;
    F(BYTE*, self, 4) = p;
    F(BYTE*, self, 8) = p;
    F(BYTE*, self, 12) = p + n;
    memmove(p, b, e - b);
    F(BYTE*, self, 8) = p + (e - b);
    return self;
}

// [FUN_47a010]
BYTE* __cdecl Range_CallE(BYTE* out, DWORD f, DWORD l, int v)
{
    CallC(FUN_47c070, out, f, l, v, v);
    return out;
}

// [FUN_47c460] copy of 16-byte records
BYTE* __cdecl Copy16(const BYTE* first, const BYTE* last, BYTE* d)
{
    for (int n = (last - first) >> 4; n > 0; n--, first += 16, d += 16)
        memcpy(d, first, 16);
    return d;
}

// [FUN_47da40] copy of 8-byte records
BYTE* __cdecl Copy8B(const BYTE* first, const BYTE* last, BYTE* d)
{
    for (int n = (last - first) >> 3; n > 0; n--, first += 8, d += 8)
        memcpy(d, first, 8);
    return d;
}

// [FUN_47da10] copy_backward of 8-byte records
BYTE* __cdecl CopyBackward8(const BYTE* first, const BYTE* last, BYTE* d)
{
    for (int n = (last - first) >> 3; n > 0; n--) {
        last -= 8;
        d -= 8;
        memcpy(d, last, 8);
    }
    return d;
}

// [FUN_47e100] copy of 40-byte records
BYTE* __cdecl Copy40A(const BYTE* first, const BYTE* last, BYTE* d)
{
    for (int n = (last - first) / 40; n > 0; n--, first += 40, d += 40)
        memcpy(d, first, 40);
    return d;
}

// [FUN_47e1a0]
BYTE* __cdecl Copy40B(const BYTE* first, const BYTE* last, BYTE* d)
{
    for (int n = (last - first) / 40; n > 0; n--, first += 40, d += 40)
        memcpy(d, first, 40);
    return d;
}

// [FUN_47d7a0] distance of 8-byte records, added to *n
int* __cdecl Distance8(const BYTE* first, const BYTE* last, int* n)
{
    *n += (last - first) >> 3;
    return n;
}

// [FUN_47de30] distance of 40-byte records
int* __cdecl Distance40(const BYTE* first, const BYTE* last, int* n)
{
    *n += (last - first) / 40;
    return n;
}

// [FUN_47e4e0] advance by n 8-byte records
BYTE** __cdecl Advance8(BYTE** p, int n)
{
    *p += n * 8;
    return p;
}

// [FUN_47ed10] advance by n 40-byte records
BYTE** __cdecl Advance40(BYTE** p, int n)
{
    *p += n * 40;
    return p;
}

// [FUN_47e4b0] swaps two 8-byte records
DWORD* __cdecl Swap8(DWORD* a, DWORD* b)
{
    DWORD a0 = a[0], a1 = a[1];
    a[0] = b[0];
    a[1] = b[1];
    b[0] = a0;
    b[1] = a1;
    return a;
}

// [FUN_47f990] swaps two 40-byte records
BYTE* __cdecl Swap40(BYTE* a, BYTE* b)
{
    BYTE t[40];
    memcpy(t, a, 40);
    memcpy(a, b, 40);
    memcpy(b, t, 40);
    return a;
}

// [FUN_47f900] reverses 8-byte records
BYTE* __cdecl Reverse8(BYTE* first, BYTE* last)
{
    while (first != last && first != (last -= 8)) {
        Swap8((DWORD*)first, (DWORD*)last);
        first += 8;
    }
    return last;
}

// [FUN_47f940] reverses 40-byte records
BYTE* __cdecl Reverse40(BYTE* first, BYTE* last)
{
    while (first != last && first != (last -= 40)) {
        Swap40(first, last);
        first += 40;
    }
    return last;
}

// [FUN_47f740] fills n uninitialised 8-byte records with x
BYTE* __cdecl Fill8(BYTE* d, int n, const DWORD* x)
{
    for (; n > 0; n--, d += 8)
        if (d) {
            F(DWORD, d, 0) = x[0];
            F(DWORD, d, 4) = x[1];
        }
    return d;
}

// [FUN_47f840] lower_bound of 40-byte records by the key at +32
BYTE* __cdecl LowerBound40(BYTE* first, BYTE* last, const BYTE* x)
{
    int key = F(int, x, 32);
    for (int len = (last - first) / 40; len > 0;) {
        int half = len >> 1;
        BYTE* mid = first + half * 40;
        if (F(int, mid, 32) < key) {
            first = mid + 40;
            len -= half + 1;
        } else {
            len = half;
        }
    }
    return first;
}

// [FUN_47f8a0] upper_bound of 40-byte records by the key at +32
BYTE* __cdecl UpperBound40(BYTE* first, BYTE* last, const BYTE* x)
{
    int key = F(int, x, 32);
    for (int len = (last - first) / 40; len > 0;) {
        int half = len >> 1;
        BYTE* mid = first + half * 40;
        if (key < F(int, mid, 32)) {
            len = half;
        } else {
            first = mid + 40;
            len -= half + 1;
        }
    }
    return first;
}

}

// --- sorting doubles in descending order (greater<double>); a comparison
// that the original makes with fcomp + "C0 or C3" is false for NaN

extern "C" {

// [FUN_47e420] insertion step: moves *i back into [first, i]
void __cdecl Doubles_InsertDesc(double* first, double* i)
{
    double v = *i;
    if (v > *first) {
        for (double* p = i; p > first; p--)
            *p = p[-1];
        *first = v;
        return;
    }
    double* p = i;
    while (v > p[-1]) {
        *p = p[-1];
        p--;
    }
    *p = v;
}

// [FUN_47d460] insertion sort, descending
void __cdecl Doubles_InsertionSortDesc(double* first, double* last)
{
    if (first == last)
        return;
    for (double* i = first + 1; i != last; i++) {
        double v = *i;
        if (v > *first) {
            for (double* p = i; p > first; p--)
                *p = p[-1];
            *first = v;
        } else {
            double* p = i;
            while (v > p[-1]) {
                *p = p[-1];
                p--;
            }
            *p = v;
        }
    }
}

// [FUN_47e9d0] merge of two descending ranges into out
double* __cdecl Doubles_MergeDesc(const double* f1, const double* l1, const double* f2, const double* l2, double* out)
{
    while (f1 != l1 && f2 != l2) {
        if (*f2 > *f1)
            *out++ = *f2++;
        else
            *out++ = *f1++;
    }
    while (f1 != l1)
        *out++ = *f1++;
    while (f2 != l2)
        *out++ = *f2++;
    return out;
}

// [FUN_47f7a0] lower_bound in a descending range
const double* __cdecl Doubles_LowerBoundDesc(const double* first, const double* last, const double* x)
{
    for (int len = last - first; len > 0;) {
        int half = len >> 1;
        if (first[half] > *x) {
            first += half + 1;
            len -= half + 1;
        } else {
            len = half;
        }
    }
    return first;
}

// [FUN_47f7f0] upper_bound in a descending range
const double* __cdecl Doubles_UpperBoundDesc(const double* first, const double* last, const double* x)
{
    for (int len = last - first; len > 0;) {
        int half = len >> 1;
        if (*x > first[half]) {
            len = half;
        } else {
            first += half + 1;
            len -= half + 1;
        }
    }
    return first;
}

}

extern "C" void FUN_47f740(); KING_ALIAS("_FUN_47f740", "_Fill8")

extern "C" {

// [FUN_47c4a0] temporary buffer of 8-byte records for [first, last)
int* __fastcall TempBuf8_Construct(int* self, void*, BYTE* first, BYTE* last)
{
    int n = (last - first) >> 3;
    self[1] = n;
    self[0] = n;
    self[2] = 0;
    if (n > 0x0fffffff)
        self[1] = 0x0fffffff;
    while (self[1] > 0) {
        self[2] = (int)malloc(self[1] * 8);
        if (self[2])
            break;
        self[1] /= 2;
    }
    if (self[1] > 0)
        CallC(FUN_47f740, self[2], self[1], first, last);
    return self;
}

}

extern "C" void* __cdecl Stl_Allocate(unsigned bytes);

extern "C" {

// [FUN_47c360] placement copy of a record {id, vector<char>}
void __cdecl Rec47c360_Construct(BYTE** d, BYTE* const* s)
{
    if (!d)
        return;
    d[0] = s[0];
    d[1] = d[2] = d[3] = 0;
    int n = s[2] - s[1];
    BYTE* p = n ? (BYTE*)Stl_Allocate(n) : 0;
    d[1] = p;
    d[2] = p;
    d[3] = p + n;
    memmove(p, s[1], s[2] - s[1]);
    d[2] = p + (s[2] - s[1]);
}

}

extern "C" void FUN_47d330(); KING_ALIAS("_FUN_47d330", "_Stl_UninitCopy40")
extern "C" void FUN_47e220(); KING_ALIAS("_FUN_47e220", "_Stl_UninitCopy16")
extern "C" void FUN_47e260(); KING_ALIAS("_FUN_47e260", "_Orders144_UninitCopyB")
extern "C" void FUN_47e1f0(); KING_ALIAS("_FUN_47e1f0", "_Stl_UninitCopy8")

// n records of size bytes from the allocator, filled by copy (first, last,
// to, n) from [first, last)
static BYTE* AllocAndCopy(unsigned n, unsigned size, void (*copy)(), BYTE* first, BYTE* last)
{
    BYTE* p = n ? (BYTE*)Stl_Allocate(n * size) : 0;
    CallC(copy, first, last, p, n);
    return p;
}

extern "C" {

// [FUN_47acd0]
BYTE* __stdcall Alloc40_Copy(unsigned n, BYTE* first, BYTE* last) { return AllocAndCopy(n, 40, FUN_47d330, first, last); }

// [FUN_47b030]
BYTE* __stdcall Alloc16_Copy(unsigned n, BYTE* first, BYTE* last) { return AllocAndCopy(n, 16, FUN_47e220, first, last); }

// [FUN_47b280]
BYTE* __stdcall Alloc144_Copy(unsigned n, BYTE* first, BYTE* last) { return AllocAndCopy(n, 144, FUN_47e260, first, last); }

// [FUN_47b530]
BYTE* __stdcall Alloc8_Copy(unsigned n, BYTE* first, BYTE* last) { return AllocAndCopy(n, 8, FUN_47e1f0, first, last); }

}

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")

extern "C" {

// [FUN_47fa80] destructor: deletes the objects of the vector at +120
void __fastcall Road47fa80_Destroy(BYTE* self, void*)
{
    BYTE** v = (BYTE**)(self + 120);
    for (BYTE** p = (BYTE**)v[0]; p != (BYTE**)v[1]; p++)
        if (*p)
            CallVirt(*p, 40, 1);
    int n = (v[2] - v[0]) >> 2;
    if (n)
        CallC(FUN_44b4a0, v[0], n * 4);
    int m = F(BYTE*, self, 8) - F(BYTE*, self, 0);
    if (m)
        CallC(FUN_44b4a0, F(BYTE*, self, 0), m);
}

}

extern "C" void FUN_450de0(); KING_ALIAS("_FUN_450de0", "@Vec16_Destroy@8")

extern "C" {

// [FUN_478ac0] destructor of a vector of 144-byte orders
void __fastcall Orders144_Destroy(BYTE** self, void*)
{
    for (BYTE* e = self[0]; e != self[1]; e += 144)
        CallThis(FUN_450de0, e + 120);
    int n = (self[2] - self[0]) / 144;
    if (n)
        CallC(FUN_44b4a0, self[0], n * 144);
}

}

// [FUN_47be20] finds the first 16-byte element in [b, e) whose first dword is *key
extern "C" int* __cdecl Find16_ByKey(int* b, int* e, const int* key)
{
    for (; b != e; b += 4)
        if (*b == *key)
            break;
    return b;
}

extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")    // inserts a byte range into a byte vector

// [FUN_479a30] appends a length-prefixed copy of a byte vector to another byte vector
extern "C" void __cdecl Bytes_AppendCounted(BYTE** dst, BYTE** src)
{
    DWORD len = (DWORD)(src[1] - src[0]);
    CallThis(FUN_46e460, dst, dst[1], (BYTE*)&len, (BYTE*)&len + 4);
    for (DWORD i = 0; i < (DWORD)(src[1] - src[0]); i++) {
        BYTE c = src[0][i];
        CallThis(FUN_46e460, dst, dst[1], &c, &c + 1);
    }
}

// a 40-byte record sorted by the int at +32
struct Rec40 { int a[8]; int key; int b; };

// [FUN_47ec50] linear insertion of *last into the sorted range [first, last)
extern "C" void __cdecl Rec40_LinearInsert(Rec40* first, Rec40* last)
{
    Rec40 v = *last;
    if (v.key < first->key) {
        for (Rec40* p = last; p != first; p--)
            *p = p[-1];
        *first = v;
        return;
    }
    Rec40* next = last;
    for (Rec40* prev = last - 1; v.key < prev->key; prev--) {
        *next = *prev;
        next = prev;
    }
    *next = v;
}

// [FUN_47f3f0] merges two sorted ranges into out (stable), returns the end of out
extern "C" Rec40* __cdecl Rec40_Merge(Rec40* f1, Rec40* l1, Rec40* f2, Rec40* l2, Rec40* out)
{
    for (; f1 != l1 && f2 != l2; out++) {
        if (f2->key < f1->key)
            *out = *f2++;
        else
            *out = *f1++;
    }
    for (; f1 != l1; out++)
        *out = *f1++;
    for (; f2 != l2; out++)
        *out = *f2++;
    return out;
}

extern "C" void FUN_47f770(); KING_ALIAS("_FUN_47f770", "_Stl_UninitFill40")    // fills uninitialized 40-byte records

// [FUN_47c570] temporary buffer for [first, last) of 40-byte records: as large as malloc allows, filled from first
extern "C" int* __fastcall TempBuf40_Init(int* self, int, Rec40* first, Rec40* last)
{
    int n = last - first;
    self[1] = n;
    self[0] = n;
    self[2] = 0;
    if (n > 53687091)
        self[1] = 53687091;
    while (self[1] > 0) {
        self[2] = (int)malloc(self[1] * 40);
        if (self[2])
            break;
        self[1] /= 2;
    }
    if (self[1] > 0)
        CallC(FUN_47f770, self[2], self[1], first, last);
    return self;
}

extern "C" void __cdecl VecPod_InsertAux(BYTE** v, BYTE* pos, const BYTE* x, int n, void (*copy)());
extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")    // uninitialized copy of 12-byte records
extern "C" void FUN_47e220(); KING_ALIAS("_FUN_47e220", "_Stl_UninitCopy16")    // uninitialized copy of 16-byte records

// [FUN_47b790] vector<12 bytes>::insert_aux
extern "C" void __fastcall Vec12_Insert47b790(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    VecPod_InsertAux(v, pos, x, 12, FUN_47e3e0);
}

// [FUN_47bae0] vector<16 bytes>::insert_aux
extern "C" void __fastcall Vec16_Insert47bae0(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    VecPod_InsertAux(v, pos, x, 16, FUN_47e220);
}

extern "C" void FUN_47e1f0(); KING_ALIAS("_FUN_47e1f0", "_Stl_UninitCopy8")    // uninitialized copy of 8-byte records
extern "C" void FUN_47d330(); KING_ALIAS("_FUN_47d330", "_Stl_UninitCopy40")    // uninitialized copy of 40-byte records

// [FUN_479760] vector<8 bytes>::insert_aux
extern "C" void __fastcall Vec8_Insert479760(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    VecPod_InsertAux(v, pos, x, 8, FUN_47e1f0);
}

// [FUN_479050] vector<Waypoint (40 bytes)>::insert_aux
extern "C" void __fastcall Vec40_Insert479050(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    VecPod_InsertAux(v, pos, x, 40, FUN_47d330);
}

extern "C" void FUN_47acd0(); KING_ALIAS("_FUN_47acd0", "_Alloc40_Copy@12")    // allocates n 40-byte records and copies [first, last) (stdcall)
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // the AI's allocator: free (p, size)

// [FUN_479370] vector<40-byte record>::operator=
extern "C" BYTE** __fastcall Vec40_Assign(BYTE** self, int, BYTE* const* src)
{
    if (src == self)
        return self;
    BYTE* sb = src[0];
    BYTE* se = src[1];
    int n = (int)(se - sb) / 40;
    if (n > (int)(self[2] - self[0]) / 40) {
        BYTE* p = CallThis<BYTE*>(FUN_47acd0, self, n, sb, se);
        int cap = (int)(self[2] - self[0]) / 40;
        if (cap)
            CallC(FUN_44b4a0, self[0], cap * 40);
        self[0] = p;
        self[2] = p + n * 40;
    } else if ((int)(self[1] - self[0]) / 40 >= n) {
        BYTE* d = self[0];
        for (int i = 0; i < n; i++)
            memcpy(d + i * 40, sb + i * 40, 40);
    } else {
        int size = (int)(self[1] - self[0]) / 40;
        for (int i = 0; i < size; i++)
            memcpy(self[0] + i * 40, sb + i * 40, 40);
        CallC(FUN_47d330, src[0] + size * 40, src[1], self[1], self[0]);
    }
    self[1] = self[0] + n * 40;
    return self;
}

// [FUN_47c650] copies 12-byte records [first, last) to dest; returns the end of the copy
extern "C" DWORD* __cdecl Rec12_Copy(const DWORD* first, const DWORD* last, DWORD* dest)
{
    for (int n = (int)(last - first) / 3; n > 0; n--, first += 3, dest += 3) {
        dest[0] = first[0];
        dest[1] = first[1];
        dest[2] = first[2];
    }
    return dest;
}

extern "C" void FUN_4593b0(); KING_ALIAS("_FUN_4593b0", "_Vec12b_AllocCopyA@12")    // allocates n 12-byte records and copies [first, last) (stdcall)
extern "C" void FUN_461dc0(); KING_ALIAS("_FUN_461dc0", "_Vec16b_AllocCopy@12")    // the same for 16-byte records
extern "C" void FUN_602ca0(); KING_ALIAS("_FUN_602ca0", "@MsgBuf_FreeStorage@8")    // a 16-byte record's destructor

// reserve(n) of a vector of size-byte records; dtor destroys the old ones
static void Vec_Reserve(BYTE** v, unsigned n, int size, void (*alloc)(), void (*dtor)())
{
    if ((unsigned)((v[2] - v[0]) / size) >= n)
        return;
    int count = (int)(v[1] - v[0]) / size;
    BYTE* p = CallThis<BYTE*>(alloc, v, n, v[0], v[1]);
    if (dtor)
        for (BYTE* e = v[0]; e != v[1]; e += size)
            CallThis(dtor, e);
    int cap = (int)(v[2] - v[0]) / size;
    if (cap)
        CallC(FUN_44b4a0, v[0], cap * size);
    v[0] = p;
    v[1] = p + count * size;
    v[2] = p + n * size;
}

// [FUN_47ab50] vector<12-byte record>::reserve
extern "C" void __fastcall Vec12_Reserve(BYTE** v, void*, unsigned n) { Vec_Reserve(v, n, 12, FUN_4593b0, 0); }

// [FUN_47ac30] vector<40-byte record>::reserve
extern "C" void __fastcall Vec40_Reserve(BYTE** v, void*, unsigned n) { Vec_Reserve(v, n, 40, FUN_47acd0, 0); }

// [FUN_47af30] vector<16-byte record>::reserve
extern "C" void __fastcall Vec16_Reserve(BYTE** v, void*, unsigned n) { Vec_Reserve(v, n, 16, FUN_461dc0, FUN_602ca0); }

// ---------------------------------------------------------------------------
// Static initializers of random generators.
typedef char AsmLabel[1];

KING_VAR(AsmLabel, g_roagraRng68d4f8, 68d4f8)
KING_VAR(DWORD, g_roagraRng68d5d0, 68d5d0)
KING_VAR(DWORD, g_roagraRng68d5d4, 68d5d4)
KING_VAR(DWORD, g_roagraRng68d5d8, 68d5d8)
KING_VAR(AsmLabel, g_roagraRng68d5e0, 68d5e0)
KING_VAR(DWORD, g_roagraRng68d6b8, 68d6b8)
KING_VAR(DWORD, g_roagraRng68d6bc, 68d6bc)
KING_VAR(DWORD, g_roagraRng68d6c0, 68d6c0)

// the subtractive random generator's start (Knuth): 55 words at a, then the seed and the two indices
static void Roagra_RngInit(DWORD* a, DWORD* seed, DWORD* i1, DWORD* i2)
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

// [$L_47f9d0]
extern "C" void __cdecl RoagraInit_47f9d0() { Roagra_RngInit((DWORD*)g_roagraRng68d4f8, &g_roagraRng68d5d0, &g_roagraRng68d5d4, &g_roagraRng68d5d8); }
// [$L_4805a0]
extern "C" void __cdecl RoagraInit_4805a0() { Roagra_RngInit((DWORD*)g_roagraRng68d5e0, &g_roagraRng68d6b8, &g_roagraRng68d6bc, &g_roagraRng68d6c0); }

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
// returns the storage of a vector (+0 .. +8) of size-byte elements to the pool
static void PoolVec_Free_R(BYTE* s, int size)
{
    BYTE* p = F(BYTE*, s, 0);
    int n = (F(BYTE*, s, 8) - p) / size;
    if (n)
        CallC(FUN_44b4a0, p, n * size);
}
// [$L_47aaa0] frees the storage of a vector of 40-byte records
extern "C" void __fastcall Vec40_FreeStorage(BYTE* s, void*)
{
    PoolVec_Free_R(s, 40);
}

extern "C" void FUN_490ce0(); KING_ALIAS("_FUN_490ce0", "_Save_EndBlock")
extern "C" void FUN_490c70(); KING_ALIAS("_FUN_490c70", "_Save_BlockEnd")
// [$L_478e20] ends the save block of stream s[0] started at s+4
extern "C" int __fastcall SaveBlock_End(BYTE* s) { return CallC<int>(FUN_490ce0, F(int, s, 0), s + 4); }
// [$L_478e30] the end check of the save block (stream, start, size)
extern "C" int __fastcall SaveBlock_AtEnd(BYTE* s) { return CallC<int>(FUN_490c70, F(int, s, 0), F(int, s, 4), F(int, s, 8)); }

extern "C" void FUN_46ecd0(); KING_ALIAS("_FUN_46ecd0", "_CopyBytes")
extern "C" void FUN_458cf0(); KING_ALIAS("_FUN_458cf0", "_Bytes_AllocCopy@12")
extern "C" void FUN_44bef0(); KING_ALIAS("_FUN_44bef0", "@VecChar_Insert44bef0@16")
// [FUN_479f50] loads the vector<char> v from the save stream at *cur: a count, then the bytes
extern "C" void __cdecl CharVec_Load(BYTE** cur, char** v)
{
    unsigned n = *(unsigned*)*cur;
    *cur += 4;
    char* b = v[0];
    char* e = v[1];
    CallC(FUN_46ecd0, e, e, b, cur, 0);
    v[1] += b - e;
    if ((unsigned)(v[2] - v[0]) < n) {
        int used = v[1] - v[0];
        char* p = CallStd<char*>(FUN_458cf0, n, v[0], v[1]);
        if (v[2] - v[0])
            CallC(FUN_44b4a0, v[0], v[2] - v[0]);
        v[0] = p;
        v[1] = p + used;
        v[2] = p + n;
    }
    for (; n; n--) {
        char c = *(*cur)++;
        if (v[1] != v[2]) {
            if (v[1])
                *v[1] = c;
            v[1]++;
        } else
            CallThis(FUN_44bef0, v, v[1], &c);
    }
}

// [$L_478c80] returns the storage of a vector of 8-byte elements to the allocator
extern "C" void __fastcall Vec8_FreeStorage(BYTE** v)
{
    int n = (int)(v[2] - v[0]) >> 3;
    if (n)
        CallC(FUN_44b4a0, v[0], n << 3);
}

// === the player record's copy constructor (FUN_47a230)

extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")    // Stl_Allocate (bytes)
extern "C" void FUN_4072f0(); KING_ALIAS("_FUN_4072f0", "_Copy1")    // copies bytes (first, last, to): the end
extern "C" void FUN_4abce0(); KING_ALIAS("_FUN_4abce0", "@Vec12_Reserve@16")
extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")
extern "C" void FUN_47d330(); KING_ALIAS("_FUN_47d330", "_Stl_UninitCopy40")
extern "C" void FUN_47e1f0(); KING_ALIAS("_FUN_47e1f0", "_Stl_UninitCopy8")
ASM_PROC(FUN_466300)

// d = a copy of the vector s of records of size n, made by copy (first, last, to, -)
static void Pc_Vec(BYTE** d, BYTE* const* s, int n, void (*copy)())
{
    int count = (int)(s[1] - s[0]) / n;
    BYTE* p = 0;
    d[0] = d[1] = d[2] = 0;
    if (count)
        p = CallC<BYTE*>(FUN_44b740, count * n);
    d[0] = d[1] = p;
    d[2] = p + count * n;
    d[1] = CallC<BYTE*>(copy, s[0], s[1], p, d);
}

// copies the bytes [from, to) of s to d
static void Pc_Copy(BYTE* d, const BYTE* s, int from, int to)
{
    memcpy(d + from, s + from, to - from);
}

extern "C" {

// [FUN_47a230] copy constructor of the 992-byte player record (thiscall): its vectors (+0 bytes, +16 hops,
// +64 40-byte, +136 8-byte, +152 16-byte, +172 bytes, +264 bytes, +640 12-byte) are copied, the rest field by
// field (the padding between fields isn't)
BYTE* __fastcall PlayerRec_Copy(BYTE* self, void*, const BYTE* src)
{
    BYTE** v = (BYTE**)self;
    BYTE* const* s = (BYTE* const*)src;
    {
        unsigned size = s[1] - s[0];
        v[0] = v[1] = v[2] = 0;
        BYTE* p = size ? CallC<BYTE*>(FUN_44b740, size) : 0;
        v[0] = v[1] = p;
        v[2] = p + size;
        v[1] = CallC<BYTE*>(FUN_4072f0, s[0], s[1], p);
    }
    {
        BYTE** d = (BYTE**)(self + 16);
        BYTE* const* h = (BYTE* const*)(src + 16);
        char a;
        CallThis(FUN_4abce0, d, (int)(h[1] - h[0]) / 12, &a);
        d[1] = CallC<BYTE*>(FUN_47e3e0, h[0], h[1], d[0], 0);
    }
    Pc_Copy(self, src, 32, 64);
    Pc_Vec((BYTE**)(self + 64), (BYTE* const*)(src + 64), 40, FUN_47d330);
    Pc_Copy(self, src, 76, 100);
    Pc_Copy(self, src, 104, 136);
    Pc_Vec((BYTE**)(self + 136), (BYTE* const*)(src + 136), 8, FUN_47e1f0);
    Pc_Copy(self, src, 148, 152);
    Pc_Vec((BYTE**)(self + 152), (BYTE* const*)(src + 152), 16, FUN_466300);
    Pc_Copy(self, src, 164, 172);
    {
        BYTE** d = (BYTE**)(self + 172);
        BYTE* const* b = (BYTE* const*)(src + 172);
        unsigned size = b[1] - b[0];
        d[0] = d[1] = d[2] = 0;
        BYTE* p = size ? CallC<BYTE*>(FUN_44b740, size) : 0;
        d[0] = d[1] = p;
        d[2] = p + size;
        d[1] = CallC<BYTE*>(FUN_4072f0, b[0], b[1], p);
    }
    Pc_Copy(self, src, 184, 264);
    {
        BYTE** d = (BYTE**)(self + 264);
        BYTE* const* b = (BYTE* const*)(src + 264);
        unsigned size = b[1] - b[0];
        d[0] = d[1] = d[2] = 0;
        BYTE* p = size ? CallC<BYTE*>(FUN_44b740, size) : 0;
        d[0] = d[1] = p;
        d[2] = p + size;
        memmove(p, b[0], b[1] - b[0]);
        d[1] = p + (b[1] - b[0]);
    }
    Pc_Copy(self, src, 280, 489);
    Pc_Copy(self, src, 496, 506);
    Pc_Copy(self, src, 508, 532);
    Pc_Copy(self, src, 536, 640);
    {
        BYTE** d = (BYTE**)(self + 640);
        BYTE* const* h = (BYTE* const*)(src + 640);
        int n = (int)(h[1] - h[0]) / 12;
        d[0] = d[1] = d[2] = 0;
        BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n * 12) : 0;
        d[0] = d[1] = p;
        d[2] = p + n * 12;
        d[1] = CallC<BYTE*>(FUN_47e3e0, h[0], h[1], p, d);
    }
    Pc_Copy(self, src, 652, 724);
    Pc_Copy(self, src, 728, 778);
    Pc_Copy(self, src, 784, 793);
    Pc_Copy(self, src, 800, 841);
    Pc_Copy(self, src, 844, 852);
    Pc_Copy(self, src, 856, 865);
    Pc_Copy(self, src, 868, 884);
    Pc_Copy(self, src, 888, 940);
    Pc_Copy(self, src, 944, 969);
    Pc_Copy(self, src, 976, 988);
    return self;
}

} // extern "C"

// === saving the AI's global state (FUN_480680)

KING_VAR(AsmLabel, s_ag66fd68, 66fd68)
KING_VAR(AsmLabel, g_ag66fd64, 66fd64)
KING_VAR(AsmLabel, g_ag66c730, 66c730)
KING_VAR(AsmLabel, g_ag68d6c8, 68d6c8)
KING_VAR(AsmLabel, g_ag68d0f0, 68d0f0)
KING_VAR(AsmLabel, g_ag68d0f8, 68d0f8)
KING_VAR(AsmLabel, g_ag68d100, 68d100)
KING_VAR(AsmLabel, g_ag66c6d8, 66c6d8)
KING_VAR(AsmLabel, g_ag68d190, 68d190)
KING_VAR(AsmLabel, g_ag68d198, 68d198)
KING_VAR(AsmLabel, g_ag68d170, 68d170)
KING_VAR(AsmLabel, g_ag68b9c8, 68b9c8)
KING_VAR(AsmLabel, g_ag68a830, 68a830)
KING_VAR(AsmLabel, g_ag68d158, 68d158)
KING_VAR(AsmLabel, g_ag66c6a0, 66c6a0)
KING_VAR(AsmLabel, g_ag68d164, 68d164)
KING_VAR(AsmLabel, g_ag68d168, 68d168)
KING_VAR(AsmLabel, g_ag66c6e0, 66c6e0)
KING_VAR(AsmLabel, g_ag68cc88, 68cc88)
KING_VAR(AsmLabel, g_ag66c6f8, 66c6f8)
KING_VAR(AsmLabel, g_ag66c710, 66c710)
KING_VAR(AsmLabel, g_ag68d15c, 68d15c)
KING_VAR(AsmLabel, g_ag68d160, 68d160)
KING_VAR(AsmLabel, g_ag68d108, 68d108)
extern "C" void FUN_490c90(); KING_ALIAS("_FUN_490c90", "_Save_BeginBlock")
extern "C" void FUN_490ce0(); KING_ALIAS("_FUN_490ce0", "_Save_EndBlock")
extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")
ASM_FUNC(FUN_401000, _Ai_Clock)

// appends n bytes at p to the byte vector v
static void Ag_Put(BYTE** v, const void* p, int n)
{
    CallThis(FUN_46e460, v, v[1], (const BYTE*)p, (const BYTE*)p + n);
}

static void Ag_PutVec(BYTE** v, const BYTE* g)
{
    const DWORD* b = F(const DWORD*, g, 0);
    int n = (int)(F(const BYTE*, g, 4) - (const BYTE*)b) >> 2;
    Ag_Put(v, &n, 4);
    for (unsigned i = 0; i < (unsigned)((F(const BYTE*, g, 4) - F(const BYTE*, g, 0)) >> 2); i++)
        Ag_Put(v, F(const DWORD*, g, 0) + i, 4);
}

extern "C" {

// [FUN_480680] writes the AI's global state (time, counters, prices, tables, the two id vectors) as a block
// of the save data v
void __cdecl Ai_SaveGlobals(BYTE** v)
{
    int mark;
    CallC(FUN_490c90, v, (const char*)s_ag66fd68, &mark);
    *(double*)(char*)g_ag68d6c8 = CallC<double>(FUN_401000);
    Ag_Put(v, g_ag66fd64, 4);
    Ag_Put(v, g_ag66c730, 4);
    Ag_Put(v, g_ag68d6c8, 8);
    Ag_Put(v, g_ag68d0f0, 8);
    Ag_Put(v, g_ag68d0f8, 8);
    Ag_Put(v, g_ag68d100, 8);
    Ag_Put(v, g_ag66c6d8, 8);
    Ag_Put(v, g_ag68d190, 8);
    Ag_Put(v, g_ag68d198, 8);
    Ag_Put(v, g_ag68d170, 8);
    for (int i = 0; i < 55; i++)
        Ag_Put(v, (const DWORD*)(char*)g_ag68b9c8 + i, 4);
    int zero = 0;
    Ag_Put(v, &zero, 4);
    Ag_PutVec(v, (const BYTE*)g_ag68a830);
    Ag_Put(v, g_ag68d158, 4);
    Ag_Put(v, g_ag66c6a0, 8);
    Ag_Put(v, g_ag68d164, 4);
    Ag_Put(v, g_ag68d168, 1);
    Ag_Put(v, g_ag66c6e0, 4);
    Ag_PutVec(v, (const BYTE*)g_ag68cc88);
    Ag_Put(v, g_ag66c6f8, 8);
    Ag_Put(v, g_ag66c710, 8);
    Ag_Put(v, g_ag68d15c, 4);
    Ag_Put(v, g_ag68d160, 4);
    Ag_Put(v, g_ag68d108, 8);
    CallC(FUN_490ce0, v, &mark);
}

} // extern "C"

// === stable sort of 40-byte road records with a buffer (FUN_47cf50)

extern "C" void FUN_47dab0(); KING_ALIAS("_FUN_47dab0", "_Road40_Insertion")
extern "C" void FUN_47de60(); KING_ALIAS("_FUN_47de60", "_Road40_MergeAdaptive")
extern "C" void FUN_47f170(); KING_ALIAS("_FUN_47f170", "_Road40_MergeLoop")
extern "C" void FUN_47f3f0(); KING_ALIAS("_FUN_47f3f0", "_Rec40_Merge")

static int Ss40_Count(const BYTE* a, const BYTE* b) { return (int)(b - a) / 40; }

// one merge pass of runs of step from [first, last) to result
static void Ss40_MergePass(BYTE* first, BYTE* last, BYTE* result, int step, int comp)
{
    int two = step * 2;
    while (Ss40_Count(first, last) >= two) {
        result = CallC<BYTE*>(FUN_47f3f0, first, first + step * 40, first + step * 40, first + two * 40, result, comp);
        first += two * 40;
    }
    int s = Ss40_Count(first, last);
    if (step < s)
        s = step;
    CallC(FUN_47f3f0, first, first + s * 40, first + s * 40, last, result, comp);
}

// insertion sort of runs of 7, then merge passes through the buffer and back
static void Ss40_SortWithBuffer(BYTE* first, BYTE* last, BYTE* buf, int comp)
{
    int len = Ss40_Count(first, last);
    BYTE* bufLast = buf + len * 40;
    BYTE* p = first;
    while (Ss40_Count(p, last) >= 7) {
        CallC(FUN_47dab0, p, p + 280, comp);
        p += 280;
    }
    CallC(FUN_47dab0, p, last, comp);
    for (int step = 7; step < len;) {
        Ss40_MergePass(first, last, buf, step, comp);
        step *= 2;
        CallC(FUN_47f170, buf, bufLast, first, step, comp);
        step *= 2;
    }
}

extern "C" {

// [FUN_47cf50] stable_sort's adaptive step for [first, last) of 40-byte records with the buffer buf (size n):
// each half sorted (recursively when it doesn't fit), then merged
void __cdecl Road40_StableSortAdaptive(BYTE* first, BYTE* last, BYTE* buf, int n, int comp)
{
    int len = (Ss40_Count(first, last) + 1) / 2;
    BYTE* mid = first + len * 40;
    if (len > n) {
        Road40_StableSortAdaptive(first, mid, buf, n, comp);
        Road40_StableSortAdaptive(mid, last, buf, n, comp);
    } else {
        Ss40_SortWithBuffer(first, mid, buf, comp);
        Ss40_SortWithBuffer(mid, last, buf, comp);
    }
    CallC(FUN_47de60, first, mid, last, Ss40_Count(first, mid), Ss40_Count(mid, last), buf, n, comp);
}

} // extern "C"

// === loading the AI's global state (FUN_480b00)

KING_VAR(int, g_lg68d6d0, 68d6d0)
KING_VAR(AsmLabel, s_lg66fd70, 66fd70)
KING_VAR(AsmLabel, s_lg66fcb4, 66fcb4)
KING_VAR(AsmLabel, s_lg66fd94, 66fd94)
KING_VAR(AsmLabel, s_lg68d1c0, 68d1c0)
extern "C" void FUN_401010(); KING_ALIAS("_FUN_401010", "_Ai_Report")
extern "C" void FUN_448f00(); KING_ALIAS("_FUN_448f00", "@Vec4_PushBackG@12")
extern "C" void FUN_44ac10(); KING_ALIAS("_FUN_44ac10", "@Vec4_Erase@16")
extern "C" void FUN_480650(); KING_ALIAS("_FUN_480650", "_Save_RebaseTime")
extern "C" void FUN_483800(); KING_ALIAS("_FUN_483800", "@PtrVec_Reserve@12")

static void Lg_Get(BYTE** cur, void* to, int n)
{
    memcpy(to, *cur, n);
    *cur += n;
}

static int Lg_Int(BYTE** cur)
{
    int v;
    Lg_Get(cur, &v, 4);
    return v;
}

static void Lg_Report(int a, int line)
{
    CallC(FUN_401010, a, 100, (const char*)s_lg66fd70, (const char*)s_lg66fcb4, line, (const char*)s_lg66fd94,
          (const char*)s_lg68d1c0, (const char*)s_lg68d1c0);
}

// the vector at g refilled with n ints read from cur
static void Lg_Vec(BYTE** cur, BYTE* g)
{
    int n = Lg_Int(cur);
    CallThis(FUN_44ac10, g, F(BYTE*, g, 0), F(BYTE*, g, 4));
    CallThis(FUN_483800, g, n);
    for (int i = 0; i < n; i++) {
        int v = Lg_Int(cur);
        CallThis(FUN_448f00, g, &v);
    }
}

struct Lg_Chunk { BYTE** cur; int a, b; };

static bool Lg_More(const Lg_Chunk* c) { return c->a + c->b - (int)*c->cur != 0; }

extern "C" {

// [FUN_480b00] reads the AI's global state (the block Ai_SaveGlobals writes) from the save cursor cur of chunk c:
// the versions checked, the times rebased, the tables and the two id vectors refilled; the last three fields
// only when the chunk has them
void __cdecl Ai_LoadGlobals(const Lg_Chunk* c, BYTE** cur)
{
    g_lg68d6d0 = Lg_Int(cur);
    if (g_lg68d6d0 != *(int*)(char*)g_ag66fd64)
        Lg_Report(0, 92);
    if (Lg_Int(cur) != *(int*)(char*)g_ag66c730)
        Lg_Report(1, 104);
    Lg_Get(cur, g_ag68d6c8, 8);
    Lg_Get(cur, g_ag68d0f0, 8);
    Lg_Get(cur, g_ag68d0f8, 8);
    Lg_Get(cur, g_ag68d100, 8);
    Lg_Get(cur, g_ag66c6d8, 8);
    Lg_Get(cur, g_ag68d190, 8);
    Lg_Get(cur, g_ag68d198, 8);
    Lg_Get(cur, g_ag68d170, 8);
    Lg_Get(cur, g_ag68b9c8, 55 * 4);
    *cur += 4;
    Lg_Vec(cur, (BYTE*)g_ag68a830);
    Lg_Get(cur, g_ag68d158, 4);
    Lg_Get(cur, g_ag66c6a0, 8);
    Lg_Get(cur, g_ag68d164, 4);
    Lg_Get(cur, g_ag68d168, 1);
    Lg_Get(cur, g_ag66c6e0, 4);
    Lg_Vec(cur, (BYTE*)g_ag68cc88);
    Lg_Get(cur, g_ag66c6f8, 8);
    Lg_Get(cur, g_ag66c710, 8);
    CallC(FUN_480650, (BYTE*)g_ag68d0f0);
    CallC(FUN_480650, (BYTE*)g_ag68d0f8);
    CallC(FUN_480650, (BYTE*)g_ag68d100);
    CallC(FUN_480650, (BYTE*)g_ag66c6d8);
    CallC(FUN_480650, (BYTE*)g_ag68d198);
    CallC(FUN_480650, (BYTE*)g_ag68d170);
    if (Lg_More(c))
        Lg_Get(cur, g_ag68d15c, 4);
    if (Lg_More(c))
        Lg_Get(cur, g_ag68d160, 4);
    if (Lg_More(c)) {
        Lg_Get(cur, g_ag68d108, 8);
        CallC(FUN_480650, (BYTE*)g_ag68d108);
    }
}

} // extern "C"

// === stable_sort's adaptive step for doubles (FUN_47c9a0)

extern "C" void __cdecl Doubles_ChunkInsertion(double* first, double* last, int chunk);
extern "C" void __cdecl Doubles_MergeLoop(double* first, double* last, double* result, int step);
extern "C" void __cdecl Doubles_MergeAdaptive(double* first, double* middle, double* last, int len1, int len2,
                                              double* buf, int bufSize, int pred);

// insertion sort of runs of 7, then merge passes through the buffer and back
static void SsD_SortWithBuffer(double* first, double* last, double* buf)
{
    int len = (int)(last - first);
    Doubles_ChunkInsertion(first, last, 7);
    for (int step = 7; step < len;) {
        Doubles_MergeLoop(first, last, buf, step);
        step *= 2;
        Doubles_MergeLoop(buf, buf + len, first, step);
        step *= 2;
    }
}

extern "C" {

// [FUN_47c9a0] stable_sort's adaptive step for [first, last) of doubles (descending) with the buffer buf (size n)
void __cdecl Doubles_StableSortAdaptive(double* first, double* last, double* buf, int n, int pred)
{
    int len = (int)(last - first + 1) / 2;
    double* mid = first + len;
    if (len > n) {
        Doubles_StableSortAdaptive(first, mid, buf, n, pred);
        Doubles_StableSortAdaptive(mid, last, buf, n, pred);
    } else {
        SsD_SortWithBuffer(first, mid, buf);
        SsD_SortWithBuffer(mid, last, buf);
    }
    Doubles_MergeAdaptive(first, mid, last, (int)(mid - first), (int)(last - mid), buf, n, pred);
}

} // extern "C"

// === stable_sort of 40-byte records (FUN_47c0a0, FUN_47cca0)

extern "C" void FUN_47dba0(); KING_ALIAS("_FUN_47dba0", "_Road40_MergeNoBuffer")    // merge without a buffer (first, middle, last, len1, len2, comp)
extern "C" void __cdecl free(void* p);

extern "C" {

// [FUN_47cca0] inplace_stable_sort of [first, last) (40 bytes, by key +32): insertion sort below 15, else both
// halves and a merge without a buffer
void __cdecl Road40_InplaceStableSort(Rec40* first, Rec40* last, int comp)
{
    int n = (int)(last - first);
    if (n < 15) {
        if (first != last)
            for (Rec40* i = first + 1; i != last; i++)
                Rec40_LinearInsert(first, i);
        return;
    }
    Rec40* mid = first + n / 2;
    Road40_InplaceStableSort(first, mid, comp);
    Road40_InplaceStableSort(mid, last, comp);
    CallC(FUN_47dba0, first, mid, last, (int)(mid - first), (int)(last - mid), comp);
}

// [FUN_47c0a0] stable_sort of [first, last) (40-byte records by key +32) through a temporary buffer, in place
// when none can be had
void __cdecl Road40_StableSort(Rec40* first, Rec40* last, int, int, int comp)
{
    int tb[3];
    TempBuf40_Init(tb, 0, first, last);
    if (!tb[2])
        Road40_InplaceStableSort(first, last, comp);
    else
        Road40_StableSortAdaptive((BYTE*)first, (BYTE*)last, (BYTE*)tb[2], tb[1], comp);
    free((void*)tb[2]);
}

} // extern "C"

// === chunked insertion sort and merge_adaptive of 40-byte records (FUN_47eed0, FUN_47de60)

extern "C" void FUN_47f4b0(); KING_ALIAS("_FUN_47f4b0", "_Road40_RotateAdaptive")    // rotate_adaptive (first, middle, last, len1, len2, buf, n); the new middle

static Rec40* R40_Copy(const Rec40* first, const Rec40* last, Rec40* to)
{
    for (; first != last; first++)
        *to++ = *first;
    return to;
}

static Rec40* R40_CopyBackward(const Rec40* first, const Rec40* last, Rec40* to)
{
    while (last != first)
        *--to = *--last;
    return to;
}

extern "C" {

// [FUN_47eed0] insertion sorts of the runs of chunk records of [first, last) (40 bytes, by key +32)
void __cdecl Road40_ChunkInsertion(Rec40* first, Rec40* last, int chunk, int comp)
{
    for (; last - first >= chunk; first += chunk)
        CallC(FUN_47dab0, first, first + chunk, comp);
    CallC(FUN_47dab0, first, last, comp);
}

// [FUN_47de60] merge_adaptive of the sorted runs [first, middle) and [middle, last) (by key +32) with buffer buf
// (size n): through the buffer when a run fits, else split and rotated
void __cdecl Road40_MergeAdaptive(Rec40* first, Rec40* middle, Rec40* last, int len1, int len2, Rec40* buf, int n,
                                  int comp)
{
    for (;;) {
        if (len1 <= len2 && len1 <= n) {
            Rec40* bufEnd = R40_Copy(first, middle, buf);
            Rec40* b = buf;
            Rec40* out = first;
            while (b != bufEnd && middle != last)
                *out++ = middle->key < b->key ? *middle++ : *b++;
            R40_Copy(middle, last, R40_Copy(b, bufEnd, out));
            return;
        }
        if (len2 <= n) {
            Rec40* bufEnd = R40_Copy(middle, last, buf);
            Rec40* out = last;
            if (first == middle) {
                R40_CopyBackward(buf, bufEnd, out);
                return;
            }
            if (buf == bufEnd) {
                R40_CopyBackward(first, middle, out);
                return;
            }
            Rec40* l1 = middle - 1;
            Rec40* l2 = bufEnd - 1;
            for (;;) {
                if (l2->key < l1->key) {
                    *--out = *l1;
                    if (first == l1) {
                        R40_CopyBackward(buf, l2 + 1, out);
                        return;
                    }
                    l1--;
                } else {
                    *--out = *l2;
                    if (buf == l2) {
                        R40_CopyBackward(first, l1 + 1, out);
                        return;
                    }
                    l2--;
                }
            }
        }
        Rec40* cut1;
        Rec40* cut2;
        int len11, len22;
        if (len1 > len2) {
            len11 = len1 / 2;
            cut1 = first + len11;
            cut2 = (Rec40*)LowerBound40((BYTE*)middle, (BYTE*)last, (const BYTE*)cut1);
            len22 = (int)(cut2 - middle);
        } else {
            len22 = len2 / 2;
            cut2 = middle + len22;
            cut1 = (Rec40*)UpperBound40((BYTE*)first, (BYTE*)middle, (const BYTE*)cut2);
            len11 = (int)(cut1 - first);
        }
        Rec40* mid = CallC<Rec40*>(FUN_47f4b0, cut1, middle, cut2, len1 - len11, len22, buf, n);
        Road40_MergeAdaptive(first, cut1, mid, len11, len22, buf, n, comp);
        first = mid;
        middle = cut2;
        len1 -= len11;
        len2 -= len22;
    }
}

} // extern "C"

// === rotating and merging 40-byte records without a buffer (FUN_47f4b0, FUN_47dba0)

extern "C" BYTE* __cdecl Rotate40(BYTE* f, BYTE* m, BYTE* l);                          // ai_containers.cpp

extern "C" {

// [FUN_47f4b0] rotate_adaptive: [first, middle) and [middle, last) swapped through buffer buf (size n) when the
// shorter part fits, else in place; returns the new middle
Rec40* __cdecl Road40_RotateAdaptive(Rec40* first, Rec40* middle, Rec40* last, int len1, int len2, Rec40* buf, int n)
{
    if (len1 > len2 && len2 <= n) {
        Rec40* end = R40_Copy(middle, last, buf);
        R40_CopyBackward(first, middle, last);
        return R40_Copy(buf, end, first);
    }
    if (len1 <= n) {
        Rec40* end = R40_Copy(first, middle, buf);
        R40_Copy(middle, last, first);
        return R40_CopyBackward(buf, end, last);
    }
    Rotate40((BYTE*)first, (BYTE*)middle, (BYTE*)last);
    return first + (last - middle);
}

// [FUN_47dba0] merge_without_buffer of the sorted runs [first, middle) and [middle, last) (by key +32): split at
// the longer run's middle, rotated and both parts merged
void __cdecl Road40_MergeNoBuffer(Rec40* first, Rec40* middle, Rec40* last, int len1, int len2, int comp)
{
    while (len1 && len2) {
        if (len1 + len2 == 2) {
            if (middle->key < first->key) {
                Rec40 t = *first;
                *first = *middle;
                *middle = t;
            }
            return;
        }
        Rec40* cut1;
        Rec40* cut2;
        int len11, len22;
        if (len1 > len2) {
            len11 = len1 / 2;
            cut1 = first + len11;
            cut2 = (Rec40*)LowerBound40((BYTE*)middle, (BYTE*)last, (const BYTE*)cut1);
            len22 = (int)(cut2 - middle);
        } else {
            len22 = len2 / 2;
            cut2 = middle + len22;
            cut1 = (Rec40*)UpperBound40((BYTE*)first, (BYTE*)middle, (const BYTE*)cut2);
            len11 = (int)(cut1 - first);
        }
        Rotate40((BYTE*)cut1, (BYTE*)middle, (BYTE*)cut2);
        Rec40* mid = cut1 + len22;
        Road40_MergeNoBuffer(first, cut1, mid, len11, len22, comp);
        first = mid;
        middle = cut2;
        len1 -= len11;
        len2 -= len22;
    }
}

} // extern "C"

// === loading a vector of orders (FUN_479cb0)

extern "C" void FUN_460e60(); KING_ALIAS("_FUN_460e60", "_Order144_CopyA")
extern "C" void FUN_461b00(); KING_ALIAS("_FUN_461b00", "@Rec41ce50_DeletingDestructor@12")
extern "C" void FUN_41ce50(); KING_ALIAS("_FUN_41ce50", "@Rec41ce50_DestroyVector@8")
extern "C" void FUN_470d80(); KING_ALIAS("_FUN_470d80", "@Order_Construct@8")
extern "C" void FUN_4710a0(); KING_ALIAS("_FUN_4710a0", "@Rec4710a0_Load@12")
extern "C" void FUN_443860(); KING_ALIAS("_FUN_443860", "@Order144_CopyConstruct@12")
extern "C" void FUN_450960(); KING_ALIAS("_FUN_450960", "@OrderVec_InsertAux@16")

extern "C" {

// [FUN_479cb0] replaces the orders of vector v (144 bytes each) by the count and orders read from *cur, reserving
// room for all first
void __cdecl Orders_Load(BYTE** cur, BYTE** v)
{
    unsigned n = F(unsigned, *cur, 0);
    *cur += 4;
    BYTE* begin = v[0];
    BYTE* end = v[1];
    BYTE* e = CallC<BYTE*>(FUN_460e60, end, end, begin, begin, 0);
    for (BYTE* p = e; p != v[1]; p += 144)
        CallThis(FUN_461b00, p, 0);
    v[1] = end - (end - begin) / 144 * 144;
    if ((unsigned)((v[2] - v[0]) / 144) < n) {
        int size = (int)(v[1] - v[0]) / 144;
        BYTE* p = Alloc144_Copy(n, v[0], v[1]);
        for (BYTE* q = v[0]; q != v[1]; q += 144)
            CallThis(FUN_41ce50, q);
        if (int bytes = (int)(v[2] - v[0]) / 144 * 144)
            CallC(FUN_44b4a0, v[0], bytes);
        v[0] = p;
        v[1] = p + size * 144;
        v[2] = p + n * 144;
    }
    for (unsigned i = 0; i < n; i++) {
        BYTE o[144];
        CallThis(FUN_470d80, o);
        CallThis(FUN_4710a0, o, cur);
        if (v[1] != v[2]) {
            if (v[1])
                CallThis(FUN_443860, v[1], o);
            v[1] += 144;
        } else
            CallThis(FUN_450960, v, v[1], o);
        CallThis(FUN_450de0, o + 120);
    }
}

} // extern "C"

// === stable_sort of doubles (FUN_47be40)

extern "C" void FUN_47c790(); KING_ALIAS("_FUN_47c790", "_Doubles_StableSort")    // inplace stable sort

extern "C" {

// [FUN_47be40] stable_sort of [first, last) (doubles, descending) through a temporary buffer, in place when none
// can be had
void __cdecl Doubles_StableSortBuf(double* first, double* last, int, int, int pred)
{
    int tb[3];
    TempBuf8_Construct(tb, 0, (BYTE*)first, (BYTE*)last);
    if (!tb[2])
        CallC(FUN_47c790, first, last, pred);
    else
        Doubles_StableSortAdaptive(first, last, (double*)tb[2], tb[1], pred);
    free((void*)tb[2]);
}

} // extern "C"

// === loading a vector of named records (FUN_479aa0)

extern "C" void FUN_4471f0(); KING_ALIAS("_FUN_4471f0", "@CharVec_Erase@16")
extern "C" void FUN_47ab20(); KING_ALIAS("_FUN_47ab20", "@CharVec_PushBack@12")
extern "C" void FUN_469430(); KING_ALIAS("_FUN_469430", "_Rec16_Copy")
extern "C" void FUN_461ae0(); KING_ALIAS("_FUN_461ae0", "@Rec16_DeletingDestructor@12")
KING_VAR(BYTE*, g_nr68b990, 68b990)

extern "C" {

// [FUN_479aa0] replaces the records of vector v (16 bytes: an id, none here, and a name) by the count and names
// read from *cur, reserving room for all first
void __cdecl NamedRecs_Load(BYTE** cur, BYTE** v)
{
    unsigned n = F(unsigned, *cur, 0);
    *cur += 4;
    BYTE* end = v[1];
    BYTE* e = CallC<BYTE*>(FUN_469430, end, end, v[0], 0, 0);
    for (BYTE* p = e; p != v[1]; p += 16)
        CallThis(FUN_461ae0, p, 0);
    v[1] = end - (end - v[0]) / 16 * 16;
    if ((unsigned)((v[2] - v[0]) / 16) < n) {
        int size = (int)(v[1] - v[0]) / 16;
        BYTE* p = CallStd<BYTE*>(FUN_461dc0, n, v[0], v[1]);
        for (BYTE* q = v[0]; q != v[1]; q += 16)
            CallThis(FUN_602ca0, q);
        if (int bytes = (int)(v[2] - v[0]) / 16 * 16)
            CallC(FUN_44b4a0, v[0], bytes);
        v[0] = p;
        v[1] = p + size * 16;
        v[2] = p + n * 16;
    }
    for (unsigned i = 0; i < n; i++) {
        BYTE r[16];
        F(BYTE*, r, 0) = g_nr68b990;
        BYTE** name = (BYTE**)(r + 4);
        name[0] = name[1] = name[2] = 0;
        CallThis(FUN_4471f0, name, (BYTE*)0, (BYTE*)0);
        int len = F(int, *cur, 0);
        *cur += 4;
        Bytes_Reserve(name, 0, len);
        for (; len; len--) {
            BYTE c = **cur;
            (*cur)++;
            CallThis(FUN_47ab20, name, &c);
        }
        if (v[1] != v[2]) {
            if (v[1])
                Rec47a140_Copy(v[1], 0, r);
            v[1] += 16;
        } else
            CallThis(FUN_479520, v, v[1], r);
        if (int bytes = (int)(name[2] - name[0]))
            CallC(FUN_44b4a0, name[0], bytes);
    }
}

} // extern "C"

// === inserting a keyed byte vector (FUN_479520)

ASM_FUNC(FUN_47d360, _Rec16_UninitCopy)    // uninitialized copy of keyed vectors (first, last, dest, -)
extern "C" BYTE* __cdecl Rec16m_CopyBack(const BYTE* f, const BYTE* l, BYTE* d);   // ai_containers.cpp
extern "C" void __fastcall Vec1_Free(BYTE** v, int);                                // ai_world.cpp
extern "C" BYTE** __fastcall VecChar_Copy(BYTE** self, int, BYTE* const* src);
extern "C" void __fastcall VecB_FreeStorage(BYTE** v, int);
extern "C" BYTE** __fastcall PodVec_Assign(BYTE** self, void*, BYTE* const* src);

extern "C" {

// [FUN_479520] vector<{int, vector<char>}> s: inserts a copy of x before pos, growing it to twice its size
void __fastcall KeyedVecs_InsertAux(BYTE** s, void*, BYTE* pos, const BYTE* x)
{
    if (s[1] != s[2]) {
        BYTE* end = s[1];
        if (end)
            Rec47a140_Copy(end, 0, end - 16);
        s[1] += 16;
        int key = F(int, x, 0);
        BYTE* v[3];
        VecChar_Copy(v, 0, (BYTE* const*)(x + 4));
        Rec16m_CopyBack(pos, s[1] - 32, s[1] - 16);
        F(int, pos, 0) = key;
        PodVec_Assign((BYTE**)(pos + 4), 0, v);
        Vec1_Free(v, 0);
        return;
    }
    unsigned n = (unsigned)(s[1] - s[0]) >> 4;
    unsigned cap = n ? n * 2 : 1;
    BYTE* buf = cap ? CallC<BYTE*>(FUN_44b740, cap * 16) : 0;
    BYTE* p = CallC<BYTE*>(FUN_47d360, s[0], pos, buf, pos);
    if (p)
        Rec47a140_Copy(p, 0, x);
    BYTE* q = CallC<BYTE*>(FUN_47d360, pos, s[1], p + 16, p);
    for (BYTE* e = s[0]; e != s[1]; e += 16)
        VecB_FreeStorage((BYTE**)(e + 4), 0);
    if (unsigned bytes = (unsigned)(s[2] - s[0]) >> 4 << 4)
        CallC(FUN_44b4a0, s[0], bytes);
    s[0] = buf;
    s[1] = q;
    s[2] = buf + cap * 16;
}

} // extern "C"

// === uninitialized copy of 144-byte orders, the road graph's instance (FUN_47e260 with its tails .. $L_47e3c3)

extern "C" void Rg_Reserve16(); KING_ALIAS("_Rg_Reserve16", "@Vec16_Reserve@16")

extern "C" {

// [FUN_47e260..$L_47e3c3] the same copy as Orders144_UninitCopy (FUN_467000): orders [first, last) copy-
// constructed at dest with their vector of 16-byte records (+120) rebuilt; the end of the copies
BYTE* __cdecl Orders144_UninitCopyB(const BYTE* first, const BYTE* last, BYTE* dest, int)
{
    for (; first != last; first += 144, dest += 144)
    {
        if (!dest)
            continue;
        memcpy(dest, first, 20);
        memcpy(dest + 24, first + 24, 32);
        memcpy(dest + 56, first + 56, 52);
        memcpy(dest + 112, first + 112, 8);
        unsigned n = (unsigned)((F(BYTE*, first, 124) - F(BYTE*, first, 120)) >> 4);
        CallThis(Rg_Reserve16, dest + 120, n, 0);
        F(BYTE*, dest, 124) = CallC<BYTE*>(FUN_466300, F(BYTE*, first, 120), F(BYTE*, first, 124), F(BYTE*, dest, 120), dest);
        F(int, dest, 132) = F(int, first, 132);
        F(int, dest, 136) = F(int, first, 136);
    }
    return dest;
}

} // extern "C"

// [FUN_47d360] uninitialized copy of 16-byte records {int, vector<char>}
// from [first, last) to dest; returns the end of the copies
extern "C" BYTE* __cdecl Rec16_UninitCopy(BYTE* first, BYTE* last, BYTE* dest, BYTE*)
{
    for (; first != last; first += 16, dest += 16) {
        if (!dest)
            continue;
        F(int, dest, 0) = F(int, first, 0);
        BYTE* b = F(BYTE*, first, 4);
        unsigned n = (unsigned)(F(BYTE*, first, 8) - b);
        BYTE* p = n ? (BYTE*)Stl_Allocate(n) : 0;
        F(BYTE*, dest, 4) = p;
        F(BYTE*, dest, 8) = p;
        F(BYTE*, dest, 12) = p + n;
        memmove(p, b, F(BYTE*, first, 8) - b);
        F(BYTE*, dest, 8) = p + (F(BYTE*, first, 8) - b);
    }
    return dest;
}
