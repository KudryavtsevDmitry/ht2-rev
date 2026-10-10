// Marine mesh processing (Greed.cpp). ASM_MARINE selects the original code.
#include "king.h"
#include <mmsystem.h>
#include <stdlib.h>
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
typedef char AsmLabel[1];
extern "C" void __fastcall Lights_Destruct(BYTE*, void*); // aeffects.cpp
ASM_PROC(FUN_613500)
extern "C" DWORD __fastcall MarineQueue_SetNodePriority(BYTE*, void*, BYTE*, DWORD);
extern "C" BYTE* __fastcall MarineQueue_TopNode(BYTE*, void*);
ASM_PROC(FUN_605900)
extern "C" BYTE* __fastcall MarineQueue_RemoveNode(BYTE*,void*,BYTE*);
ASM_PROC(FUN_6136f0)
ASM_PROC(FUN_606750)
ASM_PROC(FUN_6069d0)
ASM_PROC(FUN_60aad0)
ASM_PROC(FUN_60aa60)
extern "C" BYTE* __fastcall MarineQueue_TopEdge(BYTE*, void*);
extern "C" DWORD __fastcall MarineQueue_SetEdgePriority(BYTE*, void*, BYTE*, DWORD);
ASM_PROC(FUN_6137d0)
ASM_PROC(FUN_606760)
ASM_VAR(DWORD, g_marineRefinementEnabled, $L_685abc)
ASM_VAR(DWORD, g_marineRefinementLimit, $L_725e28)
ASM_VAR(AsmLabel, g_marineDate, $L_6724f8)
ASM_VAR(AsmLabel, g_marineErrorFormat, $L_6724b0)
ASM_VAR(AsmLabel, g_marineFile, $L_685ac8)
ASM_VAR(AsmLabel, g_marineAssertFormat, $L_6726a4)
ASM_VAR(AsmLabel, g_marineHeader, $L_685b70)
ASM_VAR(AsmLabel, g_marineNeighbor44, $L_685b44)
ASM_VAR(AsmLabel, g_marineNeighbor52, $L_685b18)
ASM_VAR(AsmLabel, g_marineNeighbor48, $L_685aec)

extern "C" int __fastcall MarineGreed_Allocate(BYTE* self, void*, unsigned vertices, unsigned indices)
{
    F(void*, self, 12) = malloc(vertices * 12);
    F(void*, self, 8) = malloc(indices * 4);
    F(unsigned, self, 48) = indices;
    if (F(void*, self, 12) && F(void*, self, 8))
        return 1;
    Lights_Destruct(self, 0);
    CallC(FUN_60aad0, 0x88762714u, g_marineErrorFormat, g_marineFile, 161, g_marineDate);
    return 0;
}

extern "C" void __fastcall MarineGreed_Init(BYTE* self, void*)
{
    F(DWORD, F(void*, self, 12), 0) = 0;
    F(DWORD, F(void*, self, 8), 0) = 0;
    F(DWORD, self, 20) = F(DWORD, self, 24) = 0;
    BYTE* list = (BYTE*)operator new(12);
    F(BYTE*, self, 32) = list ? CallThis<BYTE*>(FUN_613500, list) : 0;
    list = (BYTE*)operator new(12);
    F(BYTE*, self, 28) = list ? CallThis<BYTE*>(FUN_613500, list) : 0;
    F(WORD, self, 36) = F(WORD, self, 38) = 0;
}

// Two sentinel nodes bound this intrusive traversal. The link base of a
// mesh node starts at +4, whereas the edge list's link base starts at +0.
static BYTE* MarineGreed_NextNode(BYTE* list, BYTE* node)
{
    BYTE* first = F(BYTE*, list, 0);
    BYTE* last = F(BYTE*, list, 4);
    BYTE* link = node ? node + 4 : 0;
    if (first && link == F(BYTE*, first, 8))
        return last;
    if (last && link == F(BYTE*, last, 8))
        return 0;
    BYTE* next = F(BYTE*, node, 4);
    return next ? next - 4 : 0;
}

static BYTE* MarineGreed_NextEdge(BYTE* list, BYTE* edge)
{
    BYTE* first = F(BYTE*, list, 0);
    BYTE* last = F(BYTE*, list, 4);
    if (first && edge == F(BYTE*, first, 4))
        return last;
    if (last && edge == F(BYTE*, last, 4))
        return 0;
    return F(BYTE*, edge, 0);
}

extern "C" void __fastcall MarineGreed_Update(BYTE* self, void*)
{
    BYTE* list = F(BYTE*, self, 32);
    F(BYTE*, self, 40) = F(BYTE*, list, 0) ? F(BYTE*, list, 0) : F(BYTE*, list, 4);
    DWORD start = timeGetTime();
    do {
        BYTE* node = F(BYTE*, self, 40);
        float score = (float)CallVirt<double>(F(void*, self, 4), 36,
                            F(DWORD, node, 16), F(DWORD, node, 20), F(DWORD, node, 24), node + 12);
        list = F(BYTE*, self, 32);
        node = F(BYTE*, self, 40);
        BYTE* next = MarineGreed_NextNode(list, node);
        MarineQueue_SetNodePriority(list, 0, node, F(DWORD, &score, 0));
        F(BYTE*, self, 40) = next;
        if ((DWORD)(timeGetTime() - start) >= 1000)
            break;
    } while (F(BYTE*, self, 40));
    while ((unsigned)F(WORD, self, 38) < CallVirt<unsigned>(F(void*, self, 4), 8)) {
        BYTE* node = MarineQueue_TopNode(F(BYTE*, self, 32), 0);
        CallC(FUN_605900, F(void*, node, 32), self);
    }
}

static void MarineGreed_UnlinkNeighbor(BYTE* node, unsigned slot, unsigned first,
                                     unsigned second, unsigned finalSlot, int line, const char* expression)
{
    BYTE* neighbor = F(BYTE*, node, slot);
    if (!neighbor)
        return;
    if (F(BYTE*, neighbor, first) == node) {
        F(BYTE*, neighbor, first) = 0;
        return;
    }
    if (second != finalSlot && F(BYTE*, neighbor, second) == node) {
        F(BYTE*, neighbor, second) = 0;
        return;
    }
    if (F(BYTE*, neighbor, finalSlot) != node)
        CallC(FUN_60aa60, 2, g_marineAssertFormat, expression, g_marineHeader, line, g_marineDate);
    neighbor = F(BYTE*, node, slot);
    F(BYTE*, neighbor, finalSlot) = 0;
}

extern "C" void __fastcall MarineGreed_RemoveNode(BYTE* self, void*, BYTE* node)
{
    BYTE* group = F(BYTE*, node, 32);
    if (node == F(BYTE*, self, 40))
        F(BYTE*, self, 40) = MarineGreed_NextNode(F(BYTE*, self, 32), node);
    --F(WORD, self, 38);
    MarineQueue_RemoveNode(F(BYTE*, self, 32), 0, node);
    MarineGreed_UnlinkNeighbor(node, 44, 44, 48, 52, 58, g_marineNeighbor44);
    MarineGreed_UnlinkNeighbor(node, 52, 48, 44, 44, 67, g_marineNeighbor52);
    MarineGreed_UnlinkNeighbor(node, 48, 52, 44, 44, 75, g_marineNeighbor48);
    BYTE* edge = F(BYTE*, node, 56);
    if (edge) {
        if (edge == F(BYTE*, self, 44))
            F(BYTE*, self, 44) = MarineGreed_NextEdge(F(BYTE*, self, 28), edge);
        --F(WORD, self, 52);
        BYTE* pair = F(BYTE*, edge, 8);
        F(DWORD, F(BYTE*, F(BYTE*, pair, 0), 12), 56) = 0;
        F(DWORD, F(BYTE*, F(BYTE*, pair, 4), 12), 56) = 0;
        pair = F(BYTE*, edge, 12);
        if (pair) {
            F(DWORD, F(BYTE*, F(BYTE*, pair, 0), 12), 56) = 0;
            F(DWORD, F(BYTE*, F(BYTE*, pair, 4), 12), 56) = 0;
        }
        CallThis(FUN_6136f0, F(void*, self, 28), edge);
        edge = F(BYTE*, node, 56);
        if (edge) {
            CallThis(FUN_606750, edge);
            operator delete(edge);
        }
    }
    if (node)
        CallVirt(node, 0, 1);
    if (!F(void*, group, 8)) {
        CallC(FUN_60aad0, 0x88762711u, g_marineErrorFormat, g_marineFile, 213, g_marineDate);
        return;
    }
    if (group) {
        CallThis(FUN_6069d0, group);
        operator delete(group);
    }
}

// Refresh a small slice of node and edge priorities each frame. The edge
// budget starts at the last node timestamp, as in the original scheduler.
extern "C" int __fastcall MarineGreed_RefreshPriorities(BYTE* self, void*)
{
    DWORD start = timeGetTime(), lastNodeTime = start;
    BYTE* list = F(BYTE*, self, 32);
    if (!F(BYTE*, self, 40))
        F(BYTE*, self, 40) = F(BYTE*, list, 0) ? F(BYTE*, list, 0) : F(BYTE*, list, 4);
    if (F(BYTE*, self, 40)) {
        do {
            BYTE* node = F(BYTE*, self, 40);
            float score = (float)CallVirt<double>(F(void*, self, 4), 36,
                F(DWORD, node, 16), F(DWORD, node, 20), F(DWORD, node, 24), node + 12);
            list = F(BYTE*, self, 32);
            node = F(BYTE*, self, 40);
            BYTE* next = MarineGreed_NextNode(list, node);
            MarineQueue_SetNodePriority(list, 0, node, F(DWORD, &score, 0));
            F(BYTE*, self, 40) = next ? next : MarineQueue_TopNode(F(BYTE*, self, 32), 0);
            lastNodeTime = timeGetTime();
        } while ((DWORD)(lastNodeTime - start) < 2);
    }
    if (!F(BYTE*, self, 44))
        F(BYTE*, self, 44) = MarineQueue_TopEdge(F(BYTE*, self, 28), 0);
    if (F(BYTE*, self, 44)) {
        do {
            BYTE* edge = F(BYTE*, self, 44);
            BYTE* primary = edge + 16;
            float score;
            if (F(void*, edge, 12)) {
                BYTE* secondary = edge + 44;
                score = (float)CallVirt<double>(F(void*, self, 4), 32,
                    F(DWORD, primary, 4), F(DWORD, primary, 8), F(DWORD, primary, 12), primary,
                    F(DWORD, secondary, 4), F(DWORD, secondary, 8), F(DWORD, secondary, 12), secondary);
            } else {
                score = (float)CallVirt<double>(F(void*, self, 4), 32,
                    F(DWORD, primary, 4), F(DWORD, primary, 8), F(DWORD, primary, 12), primary,
                    0u, 0u, 0u, (void*)0);
            }
            list = F(BYTE*, self, 28);
            edge = F(BYTE*, self, 44);
            BYTE* next = MarineGreed_NextEdge(list, edge);
            MarineQueue_SetEdgePriority(list, 0, edge, F(DWORD, &score, 0));
            list = F(BYTE*, self, 28);
            F(BYTE*, self, 44) = next ? next : F(BYTE*, list, 0) ? F(BYTE*, list, 0) : F(BYTE*, list, 4);
        } while ((DWORD)(timeGetTime() - lastNodeTime) < 1);
    }
    return 1;
}

extern "C" int __fastcall MarineGreed_Refine(BYTE* self, void*)
{
    if (!g_marineRefinementEnabled || CallThis<int>(FUN_6137d0, F(void*, self, 28)))
        return 0;
    BYTE* edge = MarineQueue_TopEdge(F(BYTE*, self, 28), 0);
    float edgeScore = F(float, edge, 72);
    BYTE* node = MarineQueue_TopNode(F(BYTE*, self, 32), 0);
    if (!(F(float, node, 28) >= edgeScore))
        return 0;
    unsigned desired = CallVirt<unsigned>(F(void*, self, 4), 8);
    for (unsigned i = 0; i < g_marineRefinementLimit; ++i) {
        if (CallThis<int>(FUN_6137d0, F(void*, self, 28)))
            break;
        edge = MarineQueue_TopEdge(F(BYTE*, self, 28), 0);
        CallC(FUN_606760, edge, self);
    }
    for (unsigned i = 0; i < g_marineRefinementLimit && (unsigned)F(WORD, self, 38) < desired; ++i) {
        node = MarineQueue_TopNode(F(BYTE*, self, 32), 0);
        CallC(FUN_605900, F(void*, node, 32), self);
    }
    return 1;
}

extern "C" int __fastcall MarineGreed_Emit(BYTE* self, void*)
{
    if (F(DWORD, self, 24))
        F(DWORD, self, 24) = MarineGreed_RefreshPriorities(self, 0);
    if (F(DWORD, self, 20))
        F(DWORD, self, 20) = MarineGreed_Refine(self, 0);
    BYTE* sink = F(BYTE*, self, 4);
    CallVirt(sink, 28);
    for (unsigned i = 0; i < (unsigned)F(WORD, self, 36); ++i)
        CallVirt(sink, 52, F(DWORD, F(void*, self, 12), i * 4));
    BYTE* list = F(BYTE*, self, 32);
    BYTE* node = F(BYTE*, list, 0) ? F(BYTE*, list, 0) : F(BYTE*, list, 4);
    while (node) {
        CallVirt(sink, 56, (unsigned)F(WORD, F(void*, node, 16), 8),
            (unsigned)F(WORD, F(void*, node, 20), 8), (unsigned)F(WORD, F(void*, node, 24), 8), node);
        node = MarineGreed_NextNode(F(BYTE*, self, 32), node);
    }
    return 1;
}

// Priority queues specific to marine mesh nodes and edges.
extern "C" DWORD __fastcall MarineQueue_InsertNode(BYTE*,void*,BYTE*);
extern "C" DWORD __fastcall MarineQueue_InsertEdge(BYTE*,void*,BYTE*);
extern "C" DWORD __fastcall MarineQueue_SetNodePriority(BYTE* self, void*, BYTE* node, DWORD priorityBits)
{
    MarineQueue_RemoveNode(self, 0, node);
    F(DWORD, node, 28) = priorityBits;
    MarineQueue_InsertNode(self, 0, node);
    return ++F(DWORD, self, 8);
}
extern "C" DWORD __fastcall MarineQueue_SetEdgePriority(BYTE* self, void*, BYTE* edge, DWORD priorityBits)
{
    CallThis(FUN_6136f0, self, edge);
    F(DWORD, edge, 72) = priorityBits;
    MarineQueue_InsertEdge(self, 0, edge);
    return ++F(DWORD, self, 8);
}
extern "C" BYTE* __fastcall MarineQueue_TopNode(BYTE* self, void*)
{
    BYTE* first = F(BYTE*, self, 0);
    BYTE* last = F(BYTE*, self, 4);
    if (!first) return last;
    if (!last) return first;
    // Unordered scores follow the same branch as a strictly smaller last.
    return !(F(float, last, 28) >= F(float, first, 28)) ? first : last;
}
extern "C" BYTE* __fastcall MarineQueue_TopEdge(BYTE* self, void*)
{
    BYTE* first = F(BYTE*, self, 0);
    BYTE* last = F(BYTE*, self, 4);
    if (!first) return last;
    if (!last) return first;
    return F(float, last, 72) > F(float, first, 72) ? first : last;
}

// Intrusive queue link manipulation. Node links start at +4; edge links at +0.
ASM_PROC(FUN_60f390)
ASM_PROC(FUN_60f7f0)
static void MarineQueue_LinkBefore(BYTE* position, BYTE* link)
{
    F(BYTE*, link, 0) = position;
    F(BYTE*, link, 4) = F(BYTE*, position, 4);
    F(BYTE*, F(BYTE*, position, 4), 0) = link;
    F(BYTE*, position, 4) = link;
}
extern "C" BYTE* __fastcall MarineQueue_RemoveNode(BYTE* self, void*, BYTE* node)
{
    BYTE* first = F(BYTE*, self, 0);
    BYTE* last = F(BYTE*, self, 4);
    BYTE* link = node ? node + 4 : 0;
    if (first && F(BYTE*, first, 4) == link && F(BYTE*, first, 8) == link) {
        if (first == node) F(BYTE*, self, 0) = 0;
        else {
            F(BYTE*, first, 8) = first + 4;
            F(BYTE*, F(BYTE*, self, 0), 4) = first + 4;
        }
    } else if (last && F(BYTE*, last, 4) == link && F(BYTE*, last, 8) == link) {
        if (last == node) F(BYTE*, self, 4) = 0;
        else {
            F(BYTE*, last, 8) = last + 4;
            F(BYTE*, F(BYTE*, self, 4), 4) = last + 4;
        }
    } else {
        BYTE* next = F(BYTE*, node, 4);
        if (first == node) F(BYTE*, self, 0) = next ? next - 4 : 0;
        if (last == node) F(BYTE*, self, 4) = next ? next - 4 : 0;
        F(BYTE*, F(BYTE*, link, 0), 4) = F(BYTE*, link, 4);
        F(BYTE*, F(BYTE*, link, 4), 0) = F(BYTE*, link, 0);
    }
    F(BYTE*, node, 8) = F(BYTE*, node, 4) = link;
    return node;
}
extern "C" DWORD __fastcall MarineQueue_InsertNode(BYTE* self, void*, BYTE* node)
{
    BYTE* last = F(BYTE*, self, 4);
    if (!last) {
        F(BYTE*, self, 4) = node;
    } else {
        float score = F(float, node, 28);
        if (!(score < 0.0f) && !(score > 0.0f)) {
            MarineQueue_LinkBefore(last + 4, node ? node + 4 : 0);
            return (DWORD)(node ? node + 4 : 0);
        }
        BYTE* position = last;
        if (!(F(float, last, 28) > score)) {
            MarineQueue_LinkBefore(last + 4, node ? node + 4 : 0);
            F(BYTE*, self, 4) = node;
        } else {
            BYTE* previousLink = F(BYTE*, last, 8);
            BYTE* previous = previousLink ? previousLink - 4 : 0;
            if (!(F(float, previous, 28) >= score)) {
                BYTE* nextLink = F(BYTE*, last, 4);
                position = nextLink ? nextLink - 4 : 0;
                while (F(float, position, 28) > score) {
                    nextLink = F(BYTE*, position, 4);
                    position = nextLink ? nextLink - 4 : 0;
                }
            }
            MarineQueue_LinkBefore(position ? position + 4 : 0, node ? node + 4 : 0);
        }
    }
    DWORD count = ++F(DWORD, self, 8);
    return count >= 500 ? CallThis<DWORD>(FUN_60f390, self) : count;
}
extern "C" DWORD __fastcall MarineQueue_InsertEdge(BYTE* self, void*, BYTE* edge)
{
    BYTE* last = F(BYTE*, self, 4);
    if (!last) {
        F(BYTE*, self, 4) = edge;
    } else {
        float score = F(float, edge, 72);
        if (F(float, last, 72) >= score) {
            MarineQueue_LinkBefore(last, edge);
            F(BYTE*, self, 4) = edge;
        } else {
            BYTE* position = last;
            if (F(float, F(BYTE*, last, 4), 72) > score) {
                position = F(BYTE*, last, 0);
                while (!(F(float, position, 72) >= score))
                    position = F(BYTE*, position, 0);
            }
            MarineQueue_LinkBefore(position, edge);
        }
    }
    DWORD count = ++F(DWORD, self, 8);
    return count >= 200 ? CallThis<DWORD>(FUN_60f7f0, self) : count;
}
