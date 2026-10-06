// airoad.cpp - ai_road.cpp of the AI, rewritten from king.masm: the ids of
// the road graph (NodeId, RoadId, PositionId) with their checks, and lookups
// of the roads between nodes.
//
// Building with  nmake ASM_AIROAD=1  makes king.masm assemble the original
// procedures again.

#include "road.h"

typedef char AiText[1];
ASM_VAR(AiText, g_idText, $L_77e460)           // "(%d)" of an invalid id
ASM_VAR(AiText, g_positionText, $L_77e6e0)     // an invalid PositionId

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

ASM_PROC(FUN_44bef0)    // vector<char>::insert_aux(pos, x)
ASM_PROC(FUN_47b790)    // vector<12 bytes>::insert_aux(pos, x)
ASM_PROC(FUN_478fb0)    // list: a new node holding x

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
ASM_VAR(RngTable, g_rngTable, $L_68b9c8)
ASM_VAR(unsigned, g_rngIndex1, $L_68baa4)
ASM_VAR(unsigned, g_rngIndex2, $L_68baa8)

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

ASM_VAR(AiText, g_undefinedModule, $L_66d638)   // "UNDEFINED_MODULE"
ASM_VAR(AiText, g_undefinedRoom, $L_66d628)     // "UNDEFINED_ROOM"

extern "C" PositionId* __cdecl Road_Advance(PositionId* out, PositionId pos, double* distance, int);
KING_ALIAS("_Road_Advance", "FUN_435d60")
extern "C" void __cdecl Road_RoomInfo(PositionId pos, RoomInfo* info);
KING_ALIAS("_Road_RoomInfo", "FUN_438d70")

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
