// road.h - the AI's road graph (ai_road.cpp, ai_trce.cpp): nodes, edges,
// positions on edges, routes as sequences of hops; and the parts of the
// game's C runtime and STL the AI code uses.
#pragma once

#include <stdlib.h>
#include "ai.h"

// the game's C runtime (UCRT's <io.h> and <stdio.h> make these inline)
extern "C" int __cdecl _open(const char* path, int flags, ...);
extern "C" int __cdecl _read(int fd, void* buf, unsigned count);
extern "C" int __cdecl _close(int fd);
typedef struct _iobuf FILE;
extern "C" FILE* __cdecl fopen(const char* path, const char* mode);
extern "C" int __cdecl fclose(FILE* f);
extern "C" int __cdecl fprintf(FILE* f, const char* format, ...);
extern "C" int __cdecl sprintf(char* buf, const char* format, ...);

// double precision vector of the AI
struct DVec3 {
    double x, y, z;
};


// ---------------------------------------------------------------------------
// The road graph (ai_road.cpp): nodes, edges, and a route as a sequence of
// hops along edges.

struct RoadEdge;

struct RoadNode {
    CharVec name;
    DWORD   flags;                  // +12, 8: no route table
    int     id;                     // +16, its index in the node vector
    struct { RoadEdge** begin, ** end, ** capacityEnd; } edges[2];  // +20, leaving by start / end
    BYTE    m_44[152];
    BYTE*   routeTable[2];          // +196, per side: a byte per node, 0xff: none
};

struct RoadCost {                   // 16 bytes
    double cost;
    bool   done;
    int    from;
};

// a piece of an edge's geometry (a segment or an arc)
struct RoadObject {
    void** vtbl;
    BYTE   m_4[4];
    DWORD  id;                      // +8

    int SegmentCount() { return CallVirt<int>(this, 24); }
};

struct RoadEdge {
    CharVec      name;
    int          id;                // +12, its index in the edge vector
    DWORD        flags;             // +16, 0x400: on the route being checked
    BYTE         m_20[28];
    RoadCost     cost[2][2];        // +48, [0][side] and [1][side]
    RoadNode*    from;              // +112
    RoadNode*    to;                // +116
    RoadObject** objects;           // +120, a vector
    RoadObject** objectsEnd;
};

struct Hop {                        // 12 bytes
    int       node;                 // index into the node vector
    RoadEdge* edge;
    short     dir;                  // +8, bit 0: the edge is run from its end
    BYTE      code;                 // +10, route table entry: edges[code >> 7][code & 127]
};

struct HopVec {
    Hop* begin;
    Hop* end;
};

// a position on the road graph, passed by value (32 bytes)
struct PositionId {
    int    road;                    // edge index
    int    node;                    // the node at the position, if any
    double t;                       // 0 at the edge's start, 1 at its end
    int    segment;                 // within the object
    int    object;                  // index into the edge's objects
    int    m_24;
    int    m_28;                    // -1
};

ASM_VAR(RoadNode**, g_roadNodes, $L_68cf00)
ASM_VAR(RoadNode**, g_roadNodesEnd, $L_68cf04)
ASM_VAR(RoadEdge**, g_roadEdges, $L_68cf38)
ASM_VAR(RoadEdge**, g_roadEdgesEnd, $L_68cf3c)
typedef double NodeCosts[1];
ASM_VAR(NodeCosts, g_nodeCosts, $L_68d8c0)    // a double per node
ASM_VAR(char, g_aiEmpty, $L_68d1c0)
typedef void* FreeLists[17];
ASM_VAR(FreeLists, g_stlFreeLists, $L_68d1c0)  // the STL node allocator, by 8 bytes
ASM_VAR(volatile LONG, g_stlLock, $L_68d204)
ASM_PROC(FUN_401aa0)                            // the allocator lock: acquire

extern "C" PositionId* __cdecl Road_EdgeStart(PositionId* out, RoadEdge* e);
KING_ALIAS("_Road_EdgeStart", "FUN_47fcf0")
extern "C" PositionId* __cdecl Road_EdgeEnd(PositionId* out, RoadEdge* e);
KING_ALIAS("_Road_EdgeEnd", "FUN_47fe20")
extern "C" DVec3* __cdecl Road_Tangent(DVec3* out, PositionId pos);
KING_ALIAS("_Road_Tangent", "FUN_4390a0")
extern "C" void __cdecl AiError(int kind, int code, const char* file, const char* id, int line,
                                const char* msg, const char* a, const char* b);
KING_ALIAS("_AiError", "FUN_401010")

inline const char* Str(const CharVec* v)
{
    return v->begin != v->end ? v->begin : 0;
}

inline unsigned RoadNodeCount()
{
    return g_roadNodesEnd - g_roadNodes;
}

struct HopVecFull {
    Hop* begin;
    Hop* end;
    Hop* capacityEnd;
};

extern "C" double __cdecl Road_EdgeCost(int side, RoadEdge* e, int dir);
KING_ALIAS("_Road_EdgeCost", "FUN_409980")
extern "C" HopVecFull* __cdecl Road_FindRoute(HopVecFull* out, int side, int from, int to, double* cost);
KING_ALIAS("_Road_FindRoute", "FUN_484c80")
extern "C" void __cdecl Road_PositionCost(int side, PositionId from, PositionId to, double* cost);
KING_ALIAS("_Road_PositionCost", "FUN_486800")

static const double s_noRoute = 1.7976931348623157e308;    // [$L_64bc30]

// the STL's node allocator, inlined in the game
inline void StlDeallocate(void* p, unsigned bytes)
{
    if (bytes > 128) {
        free(p);
        return;
    }
    void** list = &g_stlFreeLists[(bytes + 7) >> 3];
    CallThis(FUN_401aa0, (const void*)&g_stlLock);
    *(void**)p = *list;
    *list = p;
    g_stlLock = 0;
}
