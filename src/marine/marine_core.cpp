// Marine mesh processing and task helpers. ASM_MARINE_CORE selects the original code.
#include "king.h"
#include "x87.h"
#include "vecmath.h"
#include <mmsystem.h>
#include <stdlib.h>
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
typedef char AsmLabel[1];
extern "C" void __fastcall Lights_Destruct(BYTE*, void*); // aeffects.cpp
extern "C" BYTE* __fastcall MarineQueue_Init(BYTE*, void*);
extern "C" DWORD __fastcall MarineQueue_SetNodePriority(BYTE*, void*, BYTE*, DWORD);
extern "C" BYTE* __fastcall MarineQueue_TopNode(BYTE*, void*);
extern "C" void FUN_605900(); KING_ALIAS("_FUN_605900", "_GridMesh_Refine")
extern "C" BYTE* __fastcall MarineQueue_RemoveNode(BYTE*,void*,BYTE*);
extern "C" BYTE* __fastcall MarineQueue_RemoveLink(BYTE*,void*,BYTE*);
extern "C" void __fastcall GridEdge_Destruct(BYTE*,void*);
extern "C" void __fastcall GridBranch_Destruct(BYTE*,void*);
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")
extern "C" void FUN_60aa60(); KING_ALIAS("_FUN_60aa60", "_Grd_Message")
extern "C" BYTE* __fastcall MarineQueue_TopEdge(BYTE*, void*);
extern "C" DWORD __fastcall MarineQueue_SetEdgePriority(BYTE*, void*, BYTE*, DWORD);
extern "C" DWORD __fastcall MarineQueue_Empty(BYTE*,void*);
extern "C" void FUN_606760(); KING_ALIAS("_FUN_606760", "_GridMesh_RemoveEdge")
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
    F(BYTE*, self, 32) = list ? MarineQueue_Init(list,0) : 0;
    list = (BYTE*)operator new(12);
    F(BYTE*, self, 28) = list ? MarineQueue_Init(list,0) : 0;
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
        MarineQueue_RemoveLink(F(BYTE*, self, 28),0,edge);
        edge = F(BYTE*, node, 56);
        if (edge) {
            GridEdge_Destruct(edge,0);
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
        GridBranch_Destruct(group,0);
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
    if (!g_marineRefinementEnabled || MarineQueue_Empty(F(BYTE*,self,28),0))
        return 0;
    BYTE* edge = MarineQueue_TopEdge(F(BYTE*, self, 28), 0);
    float edgeScore = F(float, edge, 72);
    BYTE* node = MarineQueue_TopNode(F(BYTE*, self, 32), 0);
    if (!(F(float, node, 28) >= edgeScore))
        return 0;
    unsigned desired = CallVirt<unsigned>(F(void*, self, 4), 8);
    for (unsigned i = 0; i < g_marineRefinementLimit; ++i) {
        if (MarineQueue_Empty(F(BYTE*,self,28),0))
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
    MarineQueue_RemoveLink(self,0,edge);
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
extern "C" DWORD __fastcall MarineQueue_RebuildNodes(BYTE*,void*);
extern "C" DWORD __fastcall MarineQueue_RebuildEdges(BYTE*,void*);
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
    return count >= 500 ? MarineQueue_RebuildNodes(self, 0) : count;
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
    return count >= 200 ? MarineQueue_RebuildEdges(self, 0) : count;
}

// Merge the two sorted circular queues, moving whole runs into the first.
static BYTE* MarineQueue_NodeNext(BYTE* node)
{
    BYTE* next = F(BYTE*, node, 4);
    return next ? next - 4 : 0;
}
static BYTE* MarineQueue_NodePrevious(BYTE* node)
{
    BYTE* previous = F(BYTE*, node, 8);
    return previous ? previous - 4 : 0;
}
static void MarineQueue_SpliceRun(BYTE* firstLink, BYTE* endLink, BYTE* targetLink)
{
    BYTE* runTail = F(BYTE*, endLink, 4);
    BYTE* targetTail = F(BYTE*, targetLink, 4);
    F(BYTE*, firstLink, 4) = targetTail;
    F(BYTE*, runTail, 0) = targetLink;
    F(BYTE*, targetTail, 0) = firstLink;
    F(BYTE*, targetLink, 4) = runTail;
}
extern "C" DWORD __fastcall MarineQueue_RebuildNodes(BYTE* self, void*)
{
    BYTE* second = F(BYTE*, self, 4);
    F(DWORD, self, 8) = 0;
    if (!second) return 0;
    BYTE* first = F(BYTE*, self, 0);
    if (!first) {
        F(BYTE*, self, 0) = second;
        F(BYTE*, self, 4) = 0;
        return (DWORD)second;
    }
    BYTE* firstTail = MarineQueue_NodePrevious(first);
    BYTE* secondTail = MarineQueue_NodePrevious(second);
    BYTE* target;
    while (true) {
        while (F(float, first, 28) > F(float, second, 28)) {
            if (first == firstTail) {
                target = F(BYTE*, self, 0);
                goto finish;
            }
            first = MarineQueue_NodeNext(first);
        }
        BYTE* runEnd = second;
        while (!(F(float, first, 28) > F(float, runEnd, 28))) {
            if (runEnd == secondTail) {
                if (first == F(BYTE*, self, 0)) F(BYTE*, self, 0) = second;
                target = first;
                goto finish;
            }
            runEnd = MarineQueue_NodeNext(runEnd);
        }
        if (first == F(BYTE*, self, 0)) F(BYTE*, self, 0) = second;
        MarineQueue_SpliceRun(second + 4, runEnd + 4, first + 4);
        second = runEnd;
    }
finish:
    BYTE* targetLink = target ? target + 4 : 0;
    BYTE* tailLink = secondTail ? secondTail + 4 : 0;
    BYTE* secondLink = second ? second + 4 : 0;
    BYTE* previous = F(BYTE*, targetLink, 4);
    F(BYTE*, secondLink, 4) = previous;
    F(BYTE*, tailLink, 0) = targetLink;
    F(BYTE*, previous, 0) = secondLink;
    F(BYTE*, targetLink, 4) = tailLink;
    F(BYTE*, self, 4) = 0;
    return (DWORD)targetLink;
}
extern "C" DWORD __fastcall MarineQueue_RebuildEdges(BYTE* self, void*)
{
    BYTE* second = F(BYTE*, self, 4);
    F(DWORD, self, 8) = 0;
    if (!second) return 0;
    BYTE* first = F(BYTE*, self, 0);
    if (!first) {
        F(BYTE*, self, 0) = second;
        F(BYTE*, self, 4) = 0;
        return (DWORD)second;
    }
    BYTE* firstTail = F(BYTE*, first, 4);
    BYTE* secondTail = F(BYTE*, second, 4);
    BYTE* target;
    bool firstExhausted = false;
    while (true) {
        while (!(F(float, first, 72) >= F(float, second, 72))) {
            if (first == firstTail) {
                target = F(BYTE*, self, 0);
                firstExhausted = true;
                goto finish;
            }
            first = F(BYTE*, first, 0);
        }
        BYTE* runEnd = second;
        while (F(float, first, 72) >= F(float, runEnd, 72)) {
            if (runEnd == secondTail) {
                if (first == F(BYTE*, self, 0)) F(BYTE*, self, 0) = second;
                target = first;
                goto finish;
            }
            runEnd = F(BYTE*, runEnd, 0);
        }
        if (first == F(BYTE*, self, 0)) F(BYTE*, self, 0) = second;
        MarineQueue_SpliceRun(second, runEnd, first);
        second = runEnd;
    }
finish:
    BYTE* previous = F(BYTE*, target, 4);
    F(BYTE*, second, 4) = previous;
    F(BYTE*, secondTail, 0) = target;
    F(BYTE*, previous, 0) = second;
    F(BYTE*, target, 4) = secondTail;
    F(BYTE*, self, 4) = 0;
    return (DWORD)(firstExhausted ? target : previous);
}

// Marine task state and registration helpers.
ASM_VAR(DWORD, g_marineStartDelay, $L_653008)
extern "C" BYTE* __fastcall MarineTask_SetMaterial(BYTE* self, void*, DWORD material)
{
    BYTE* mesh = F(BYTE*, self, 0);
    F(DWORD, F(void*, mesh, 4), 116) = material;
    return mesh;
}
extern "C" DWORD __fastcall MarineTask_GetMaterial(BYTE* self, void*)
{
    return F(DWORD, self, 116);
}
extern "C" DWORD __fastcall MarineTask_Activate(BYTE* self, void*, DWORD immediate)
{
    if (F(int, F(void*, self, 112), 2992) > 5) {
        F(DWORD, self, 120) = 0;
        F(DWORD, self, 156) = (!immediate && !F(DWORD, self, 116)) ? 0 : g_marineStartDelay;
        F(DWORD, self, 116) = 1;
        F(DWORD, self, 132) = 0;
        return 0;
    }
    F(DWORD, self, 120) = 1;
    if (F(DWORD, self, 116) != 1) F(DWORD, self, 156) = g_marineStartDelay;
    F(DWORD, self, 116) = 1;
    F(DWORD, self, 132) = 0;
    return 1;
}
extern "C" DWORD __fastcall MarineTask_Deactivate(BYTE* self, void*, DWORD immediate)
{
    F(DWORD, self, 156) = (F(DWORD, self, 116) == 1 && !immediate) ? g_marineStartDelay : 0;
    F(DWORD, self, 116) = 0;
    return 0;
}
extern "C" DWORD __fastcall MarineTask_SetPendingValue(BYTE* self, void*, DWORD value)
{
    F(DWORD, self, 128) = 1;
    F(DWORD, self, 152) = value;
    return value;
}
extern "C" BYTE* __fastcall MarineTask_ClearRegistration(BYTE* self, void*)
{
    BYTE* registration = F(BYTE*, self, 16);
    F(DWORD, registration, 4) = 0;
    return registration;
}
extern "C" DWORD __fastcall MarineTask_SetFirstBounds(BYTE* self, void*, DWORD first, DWORD second)
{
    F(DWORD, self, 44) = first;
    F(DWORD, self, 48) = second;
    return first;
}
extern "C" DWORD __fastcall MarineTask_SetSecondBounds(BYTE* self, void*, DWORD first, DWORD second)
{
    F(DWORD, self, 52) = first;
    F(DWORD, self, 56) = second;
    return first;
}

// Marine task setup and callbacks.
extern "C" DWORD __fastcall MarineMesh_PruneVertices(BYTE*,void*);
extern "C" BYTE* __fastcall MarineTask_InitPoints(BYTE* self, void*, unsigned count, const DWORD* points)
{
    for (unsigned i = 0; i < count; ++i) F(DWORD, self, 16 + i * 4) = points[i];
    F(DWORD, self, 0) = count;
    return self;
}
extern "C" BYTE* __fastcall MarineTask_AllocatePoints(BYTE* self, void*, unsigned count, const DWORD* points)
{
    BYTE* value = (BYTE*)operator new(56);
    if (value) value = MarineTask_InitPoints(value, 0, count, points);
    F(BYTE*, self, 200) = value;
    return value;
}
extern "C" BYTE* __fastcall MarineTask_SetPoints(BYTE* self, void*, unsigned count, const DWORD* points)
{
    return MarineTask_AllocatePoints(F(BYTE*, F(void*, self, 0), 4), 0, count, points);
}
extern "C" BYTE* __fastcall MarineTask_RequestPoints(BYTE* self, void*, unsigned count, const DWORD* points)
{
    F(DWORD, self, 124) = 1;
    return MarineTask_SetPoints(F(BYTE*, self, 108), 0, count, points);
}
static BYTE* MarineTask_CopyBounds(BYTE* self, const DWORD* first, const DWORD* second)
{
    BYTE* mesh = F(BYTE*, self, 0);
    for (unsigned i = 0; i < 3; ++i) F(DWORD, mesh, 56 + i * 4) = first[i];
    for (unsigned i = 0; i < 3; ++i) F(DWORD, mesh, 68 + i * 4) = second[i];
    CallVirt(F(void*, mesh, 4), 4, mesh + 56, mesh + 68);
    F(DWORD, mesh, 24) = F(DWORD, mesh, 20) = 1;
    return mesh;
}
extern "C" int __fastcall MarineTask_SetBounds(BYTE* self, void*, const DWORD* first, const DWORD* second)
{
    MarineTask_CopyBounds(self, first, second);
    F(DWORD, F(void*, self, 0), 20) = 1;
    F(DWORD, F(void*, self, 0), 24) = 1;
    return 1;
}
extern "C" DWORD __fastcall MarineTask_RebuildBounds(BYTE* self, void*, const DWORD* first, const DWORD* second)
{
    MarineTask_CopyBounds(self, first, second);
    MarineGreed_Update(F(BYTE*, self, 0), 0);
    return MarineMesh_PruneVertices(F(BYTE*, self, 0), 0);
}
extern "C" DWORD __fastcall MarineTask_ApplyPoint(BYTE* self, void*, const void* point)
{
    return CallVirt<DWORD>(F(void*, F(void*, self, 0), 4), 40, point);
}
extern "C" DWORD __fastcall MarineTask_ForwardPoint(BYTE* self, void*, const void* point)
{
    return MarineTask_ApplyPoint(F(BYTE*, self, 108), 0, point);
}
__declspec(noinline) static float MarineTask_LoadCoordinate(const float* value)
{
    return *value;
}
extern "C" DWORD __fastcall MarineTask_ForwardPointCopy(BYTE* self, void*, const float* point)
{
    float copy[3];
    for (unsigned i = 0; i < 3; ++i) copy[i] = MarineTask_LoadCoordinate(point + i);
    return MarineTask_ApplyPoint(F(BYTE*, self, 108), 0, copy);
}
extern "C" void __fastcall MarineTask_ReleaseMesh(BYTE* self, void*)
{
    BYTE* mesh = F(BYTE*, self, 0);
    if (mesh) {
        CallVirt(mesh, 0, 1);
        F(BYTE*, self, 0) = 0;
    }
}

// Marine mesh cleanup.
ASM_VAR(BYTE*, g_marineUnusedVertices, $L_725d50)
ASM_VAR(DWORD, g_marineUnusedVertexCount, $L_725d54)
extern "C" DWORD __fastcall MarineMesh_PruneVertices(BYTE* self, void*)
{
    DWORD* used = (DWORD*)malloc((unsigned)F(WORD, self, 36) * 4);
    for (unsigned i = 0; i < (unsigned)F(WORD, self, 36); ++i) used[i] = 0;
    BYTE* list = F(BYTE*, self, 32);
    BYTE* node = F(BYTE*, list, 0) ? F(BYTE*, list, 0) : F(BYTE*, list, 4);
    while (node) {
        used[F(WORD, F(void*, node, 16), 8)] = 1;
        used[F(WORD, F(void*, node, 20), 8)] = 1;
        used[F(WORD, F(void*, node, 24), 8)] = 1;
        node = MarineGreed_NextNode(F(BYTE*, self, 32), node);
    }
    for (unsigned i = 0; i < (unsigned)F(WORD, self, 36); ++i) {
        if (used[i]) continue;
        BYTE* pending = (BYTE*)operator new(12);
        if (!pending) continue;
        F(BYTE*, pending, 0) = F(BYTE*, pending, 4) = pending;
        F(void*, pending, 8) = F(void*, F(void*, self, 12), i * 4);
        ++g_marineUnusedVertexCount;
        if (!g_marineUnusedVertices) g_marineUnusedVertices = pending;
        else MarineQueue_LinkBefore(g_marineUnusedVertices, pending);
    }
    while (g_marineUnusedVertices) {
        BYTE* vertex = F(BYTE*, g_marineUnusedVertices, 8);
        WORD index = F(WORD, vertex, 8);
        WORD last = --F(WORD, self, 36);
        if (index != last) {
            BYTE* moved = F(BYTE*, F(void*, self, 12), (unsigned)last * 4);
            F(BYTE*, F(void*, self, 12), (unsigned)index * 4) = moved;
            F(WORD, moved, 8) = index;
        }
        F(DWORD, F(void*, self, 12), (unsigned)last * 4) = 0;
        if (vertex) operator delete(vertex);
        BYTE* pending = g_marineUnusedVertices;
        if (pending) {
            --g_marineUnusedVertexCount;
            g_marineUnusedVertices = F(BYTE*, pending, 0);
            if (pending == g_marineUnusedVertices) g_marineUnusedVertices = 0;
            F(BYTE*, F(BYTE*, pending, 0), 4) = F(BYTE*, pending, 4);
            F(BYTE*, F(BYTE*, pending, 4), 0) = F(BYTE*, pending, 0);
            operator delete(pending);
        }
    }
    // The original retains its scratch allocation; keep allocator behavior.
    return 0;
}
extern "C" DWORD __fastcall MarineMesh_DrainEdges(BYTE* self, void*)
{
    BYTE* edge;
    while ((edge = MarineQueue_TopEdge(F(BYTE*, self, 28), 0)) != 0)
        CallC(FUN_606760, edge, self);
    return 0;
}

// Marine object lifetime and geometry transitions.
extern "C" DWORD __fastcall MarineMesh_ConstructGrid(BYTE*,void*,const DWORD*,float,float,DWORD);
extern "C" void __fastcall MarineMesh_CreateMotionResponse(BYTE*,void*,DWORD,DWORD,DWORD);
extern "C" void __fastcall MarineGreed_Destruct(BYTE*, void*);
extern "C" void FUN_5ef780(); KING_ALIAS("_FUN_5ef780", "@Node_Remove@8")
extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")
extern "C" void FUN_5ef660(); KING_ALIAS("_FUN_5ef660", "@Sce5ef660_Destroy@4")
extern "C" DWORD* __cdecl MarineGeometry_Interpolate(DWORD*, const DWORD*, const DWORD*, float);
ASM_VAR(AsmLabel, g_marineMeshVtable, $L_652ff8)
ASM_VAR(AsmLabel, g_marineTaskVtable, $L_653040)
ASM_VAR(AsmLabel, g_marineTaskSecondaryVtable, $L_653020)
extern "C" BYTE* __fastcall MarineMesh_Create(BYTE* self, void*, DWORD x, DWORD y, DWORD z,
                                            DWORD first, DWORD second, DWORD unused, DWORD fourth)
{
    BYTE* mesh = (BYTE*)operator new(88);
    if (mesh) {
        F(WORD, mesh, 52) = F(WORD, mesh, 38) = F(WORD, mesh, 36) = 0;
        const unsigned fields[] = {28,32,40,44,8,48,4,84};
        for(unsigned i=0;i<8;++i) F(DWORD,mesh,fields[i])=0;
        F(void*,mesh,0)=g_marineMeshVtable;
        for(unsigned i=56;i<80;i+=4) F(DWORD,mesh,i)=0;
        F(DWORD,mesh,80)=timeGetTime();
    }
    F(BYTE*,self,0)=mesh;
    F(DWORD,mesh,56)=x; F(DWORD,mesh,60)=y; F(DWORD,mesh,64)=z;
    F(DWORD,mesh,68)=F(DWORD,mesh,72)=0x3f800000;
    F(DWORD,mesh,76)=0;
    DWORD position[3]={x,y,z};
    MarineMesh_ConstructGrid(mesh,0,position,F(float,&first,0),F(float,&second,0),fourth);
    MarineMesh_CreateMotionResponse(F(BYTE*,self,0),0,0x41000000u,0u,0u);
    return self;
}
extern "C" void __fastcall MarineMesh_Destruct(BYTE* self, void*)
{
    F(void*,self,0)=g_marineMeshVtable;
    BYTE* sink=F(BYTE*,self,4);
    if(sink) { CallVirt(sink,0,1); F(BYTE*,self,4)=0; }
    void* data=F(void*,self,84);
    if(data) { operator delete(data); F(void*,self,84)=0; }
    MarineGreed_Destruct(self,0);
}
extern "C" BYTE* __fastcall MarineMesh_Delete(BYTE* self, void*, DWORD flags)
{
    MarineMesh_Destruct(self,0);
    if(flags&1) operator delete(self);
    return self;
}
extern "C" void __fastcall MarineTask_Destruct(BYTE* self, void*)
{
    F(void*,self,0)=g_marineTaskVtable;
    F(void*,self,76)=g_marineTaskSecondaryVtable;
    BYTE* owner=F(BYTE*,self,108);
    if(owner) { MarineTask_ReleaseMesh(owner,0); operator delete(owner); F(void*,self,108)=0; }
    CallThis(FUN_5ef780,self);
    F(DWORD,self,124)=F(DWORD,self,128)=0;
    CallThis(FUN_5d3290,self?self+76:0);
    void* data=F(void*,self,72);
    if(data) operator delete(data);
    CallThis(FUN_5ef660,self);
}
extern "C" BYTE* __fastcall MarineTask_Delete(BYTE* self, void*, DWORD flags)
{
    MarineTask_Destruct(self,0);
    if(flags&1) operator delete(self);
    return self;
}
extern "C" BYTE* __fastcall MarineTask_DeleteSecondary(BYTE* self, void*, DWORD flags)
{
    return MarineTask_Delete(self-76,0,flags);
}
extern "C" DWORD* __fastcall MarineGeometry_Snapshot(BYTE* self, void*, DWORD* out, BYTE* registration)
{
    DWORD* source=(DWORD*)(F(BYTE*,self,16)+(unsigned)F(WORD,registration,0)*40);
    for(unsigned i=0;i<10;++i) out[i]=source[i];
    return out;
}
extern "C" BYTE* __fastcall MarineTransition_Create(BYTE* self, void*, DWORD time, BYTE* registration,
                              const DWORD* source, const DWORD* target, unsigned type, BYTE* geometry)
{
    F(BYTE*,self,0)=F(BYTE*,self,4)=self;
    for(unsigned i=0;i<10;++i) F(DWORD,self,20+i*4)=target[i];
    for(unsigned i=0;i<10;++i) F(DWORD,self,60+i*4)=source[i];
    F(BYTE*,self,100)=geometry; F(DWORD,self,8)=time; F(BYTE*,self,16)=registration;
    F(BYTE*,registration,4)=self;
    if(type<9) F(DWORD,self,12)=5;
    else {
        F(DWORD,self,12)=type<11?100:250;
        DWORD snapshot[10];
        MarineGeometry_Snapshot(geometry,0,snapshot,F(BYTE*,registration,0));
        for(unsigned i=6;i<10;++i) F(DWORD,self,20+i*4)=snapshot[i];
    }
    return self;
}
extern "C" int __fastcall MarineTransition_Advance(BYTE* self, void*, DWORD time)
{
    double elapsed=(double)time-(int)F(DWORD,self,8);
    double duration=(double)F(DWORD,self,12);
    BYTE* registration=F(BYTE*,self,16);
    DWORD* destination=(DWORD*)(F(BYTE*,F(void*,self,100),16)+(unsigned)F(WORD,F(void*,registration,0),0)*40);
    if(elapsed>duration) {
        for(unsigned i=0;i<10;++i) destination[i]=F(DWORD,self,20+i*4);
        return 1;
    }
    float factor=(float)(elapsed/duration);
    DWORD result[10];
    BYTE* indexNode=F(BYTE*,registration,0);
    DWORD* value=MarineGeometry_Interpolate(result,(DWORD*)(self+60),(DWORD*)(self+20),factor);
    destination=(DWORD*)(F(BYTE*,F(void*,self,100),16)+(unsigned)F(WORD,indexNode,0)*40);
    for(unsigned i=0;i<10;++i) destination[i]=value[i];
    return 0;
}

// Marine graph ownership and teardown.
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
ASM_VAR(AsmLabel, g_marineBaseVtable, $L_652f48)
ASM_VAR(AsmLabel, g_marineTreeFile, $L_684a20)
static void MarineTree_Detach(BYTE* node)
{
    BYTE* owner=F(BYTE*,node,8);
    if(!owner) return;
    if(F(BYTE*,owner,0)==node) F(BYTE*,owner,0)=0;
    else if(F(BYTE*,owner,4)==node) F(BYTE*,owner,4)=0;
    else CallC(FUN_60aad0,0x88762712u,g_marineErrorFormat,g_marineTreeFile,35,g_marineDate);
}
extern "C" BYTE* __fastcall MarineTree_Delete(BYTE* self, void*, DWORD flags)
{
    MarineTree_Detach(self);
    BYTE* child=F(BYTE*,self,0);
    if(child) MarineTree_Delete(child,0,1);
    child=F(BYTE*,self,4);
    if(child) MarineTree_Delete(child,0,1);
    if(flags&1) operator delete(self);
    return self;
}
extern "C" void __fastcall MarineGreed_Destruct(BYTE* self, void*)
{
    F(void*,self,0)=g_marineBaseVtable;
    BYTE* queue=F(BYTE*,self,28);
    if(queue) { CallThis(FUN_5e48f0,queue); operator delete(queue); F(void*,self,28)=0; }
    queue=F(BYTE*,self,32);
    if(queue) { CallThis(FUN_5e48f0,queue); operator delete(queue); F(void*,self,32)=0; }
    if(F(void*,self,12)) {
        for(unsigned i=0;i<(unsigned)F(WORD,self,36);++i) {
            void* vertex=F(void*,F(void*,self,12),i*4);
            if(vertex) operator delete(vertex);
        }
        free(F(void*,self,12));
        F(void*,self,12)=0;
    }
    if(F(void*,self,8)) {
        for(unsigned i=0;i<F(unsigned,self,48);++i) {
            BYTE* edge=F(BYTE*,F(void*,self,8),i*4);
            if(!edge) continue;
            MarineTree_Detach(edge);
            BYTE* child=F(BYTE*,edge,0);
            if(child) MarineTree_Delete(child,0,1);
            child=F(BYTE*,edge,4);
            if(child) MarineTree_Delete(child,0,1);
            operator delete(edge);
            F(void*,F(void*,self,8),i*4)=0;
        }
        void* edges=F(void*,self,8);
        if(edges) { operator delete(edges); F(void*,self,8)=0; }
    }
    F(DWORD,self,40)=F(DWORD,self,44)=0;
    F(WORD,self,38)=F(WORD,self,36)=0;
    F(DWORD,self,48)=0;
}
extern "C" BYTE* __fastcall MarineGreed_Delete(BYTE* self, void*, DWORD flags)
{
    MarineGreed_Destruct(self,0);
    if(flags&1) operator delete(self);
    return self;
}
extern "C" BYTE* __fastcall MarineVertex_Delete(BYTE* self, void*, DWORD flags)
{
    if(flags&1) operator delete(self);
    return self;
}

// Marine geometry interpolation and queue initialization.
extern "C" BYTE* __fastcall MarineQueue_Init(BYTE* self, void*)
{
    F(DWORD,self,0)=F(DWORD,self,4)=F(DWORD,self,8)=0;
    return self;
}
extern "C" DWORD* __cdecl MarineGeometry_Interpolate(DWORD* out, const DWORD* source, const DWORD* target, float fraction)
{
    const float* a=(const float*)source;
    const float* b=(const float*)target;
    // VC6 retains the first component on the x87 stack, but rounds the
    // other differences and products through float scratch slots.
    double dx=(double)b[3]-a[3];
    float dy=(float)((double)b[4]-a[4]),dz=(float)((double)b[5]-a[5]);
    double x=dx*fraction+a[3];
    float sy=(float)((double)dy*fraction),sz=(float)((double)dz*fraction);
    double y=(double)sy+a[4],z=(double)sz+a[5];
    float length=(float)x87_sqrt((z*z+y*y)+x*x);
    float values[10];
    values[3]=(float)(x/length);values[4]=(float)(y/length);values[5]=(float)(z/length);
    dx=(double)b[0]-a[0];
    dy=(float)((double)b[1]-a[1]);dz=(float)((double)b[2]-a[2]);
    sy=(float)((double)dy*fraction);sz=(float)((double)dz*fraction);
    values[0]=(float)(dx*fraction+a[0]);
    values[1]=(float)((double)sy+a[1]);values[2]=(float)((double)sz+a[2]);
    for(unsigned i=6;i<10;++i) {
        // The original uses FLD/FSTP, which also quiets signaling NaNs.
        volatile double value=b[i];
        values[i]=(float)value;
    }
    for(unsigned i=0;i<10;++i) out[i]=F(DWORD,values,i*4);
    return out;
}

// Marine motion response and frame processing.
ASM_VAR(BYTE*, g_marineApplicationState, $L_6cec6c)
ASM_VAR(BYTE*, g_marineRecordedState, $L_725e08)
ASM_VAR(float, g_mf_652f58, $L_652f58)
ASM_VAR(float, g_mf_652f5c, $L_652f5c)
ASM_VAR(float, g_mf_652f60, $L_652f60)
ASM_VAR(float, g_mf_652f7c, $L_652f7c)
ASM_VAR(float, g_mf_652f80, $L_652f80)
ASM_VAR(float, g_mf_653004, $L_653004)
ASM_VAR(float, g_mf_653000, $L_653000)
ASM_VAR(float, g_mf_725db4, $L_725db4)
ASM_VAR(float, g_mf_64c338, $L_64c338)
ASM_VAR(float, g_mf_64c350, $L_64c350)
ASM_VAR(float, g_mf_64d7b8, $L_64d7b8)
ASM_VAR(float, g_mf_64c250, $L_64c250)
extern "C" DWORD __fastcall MarineTransition_SetActive(BYTE* self, void*, DWORD active)
{
    F(DWORD,self,4)=active;
    return active;
}
static void MarineMotion_SetRange(BYTE* owner,double value)
{
    bool active=value>=g_mf_652f5c && !(value>g_mf_652f60);
    BYTE* transition=F(BYTE*,owner,112);
    if(transition) MarineTransition_SetActive(transition,0,active?1:0);
}
extern "C" void __fastcall MarineTask_UpdateMotion(BYTE* self, void*, const float* position)
{
    BYTE* response=F(BYTE*,F(void*,self,0),84);
    double previous=F(float,response,28);
    float x=position[0],y=position[1],z=position[2];
    F(float,response,12)=(float)x87_atan2(x,y);
    double length=x87_sqrt(((double)z*z+(double)x*x)+(double)y*y);
    F(float,response,16)=(float)length;
    if(length>g_mf_652f7c) F(float,response,16)=g_mf_652f7c;
    double radius=(double)F(float,response,16)/((double)g_mf_652f58*g_mf_653004);
    F(float,response,4)=(float)radius;
    double energy=(radius*radius)*g_mf_652f58/g_mf_725db4;
    F(float,response,8)=(float)energy;
    double target=energy*g_mf_653000+g_mf_64c338;
    F(float,response,28)=(float)target;
    if(target>g_mf_64c350) F(DWORD,response,28)=0x3fc00000;
    F(DWORD,response,20)=1161527296u;
    F(DWORD,response,12)=0;
    F(DWORD,response,32)=F(DWORD,response,28);
    F(float,response,4)=(float)((double)F(float,response,4)*g_mf_64d7b8);
    F(float,response,28)=(float)previous;
    BYTE* owner=F(BYTE*,response,0);
    F(float,owner,72)=(float)previous;
    MarineMotion_SetRange(owner,previous);
}
extern "C" void __fastcall MarineTask_Frame(BYTE* self, void*, DWORD first, DWORD second)
{
    DWORD time;
    if(F(DWORD,g_marineApplicationState,1300)==2)
        time=(DWORD)(__int64)F(double,g_marineRecordedState,7968);
    else time=timeGetTime();
    BYTE* response=F(BYTE*,F(void*,self,0),84);
    double value=((double)g_mf_64c250-g_mf_652f80)*F(float,response,32)
                  +(double)g_mf_652f80*F(float,response,28);
    BYTE* owner=F(BYTE*,response,0);
    F(float,response,28)=(float)value;
    F(float,owner,72)=(float)value;
    MarineMotion_SetRange(owner,value);
    CallVirt(F(void*,F(void*,self,0),4),24,time);
    MarineGreed_Emit(F(BYTE*,self,0),0);
    CallVirt(F(void*,F(void*,self,0),4),60,first,second);
}

// Marine task deferred bounds and update dispatch.
extern "C" void FUN_5dbcc0(); KING_ALIAS("_FUN_5dbcc0", "@Camera_Get@12")
extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")
ASM_PROC(FUN_5b4fc0)
extern "C" void __fastcall MarineTask_DrawPending(BYTE*,void*,const float*,const float*,BYTE*);
ASM_VAR(float, g_mt_axisX, $L_702620)
ASM_VAR(float, g_mt_axisY, $L_702624)
ASM_VAR(float, g_mt_axisZ, $L_702628)
ASM_VAR(BYTE*, g_mt_motionState, $L_7025b8)
ASM_VAR(DWORD, g_mt_frameValue, $L_720908)
__declspec(noinline) static float MarineTask_QuietCoordinate(const float* source)
{
    volatile double value=*source;
    return (float)value;
}
static void MarineTask_TransformAxis(const float* matrix,float* direction)
{
    for(unsigned i=0;i<3;++i)
        direction[i]=(float)(((double)matrix[6+i]*g_mt_axisZ+(double)matrix[3+i]*g_mt_axisY)
                             +(double)matrix[i]*g_mt_axisX);
}
extern "C" void __fastcall MarineTask_DeferredBounds(BYTE* self, void*)
{
    if(F(DWORD,self,56)<20) {
        float scratch[12],matrix[12]={0},direction[3],position[3];
        const DWORD* result=CallThis<const DWORD*>(FUN_5dbcc0,F(void*,self,36),scratch);
        for(unsigned i=0;i<12;++i) F(DWORD,matrix,i*4)=result[i];
        MarineTask_TransformAxis(matrix,direction);
        for(unsigned i=0;i<3;++i) position[i]=MarineTask_QuietCoordinate(matrix+9+i);
        MarineTask_SetBounds(F(BYTE*,self,32),0,(DWORD*)position,(DWORD*)direction);
        ++F(DWORD,self,56);
    } else F(DWORD,self,44)=1;
    CallThis(FUN_5d32f0,self,1);
}
extern "C" DWORD __fastcall MarineTask_DispatchUpdate(BYTE* self, void*, DWORD unused, BYTE* event)
{
    bool active=F(DWORD,self,116)!=0;
    if(!CallThis<DWORD>(FUN_5b4fc0,event,self+136)) return 0;
    float matrix[12],direction[3],position[3];
    CallThis(FUN_5dbcc0,F(void*,self,112),matrix);
    MarineTask_TransformAxis(matrix,direction);
    for(unsigned i=0;i<3;++i) position[i]=MarineTask_QuietCoordinate(matrix+9+i);
    float planar[3]={direction[0],direction[1],0};
    double length=x87_sqrt(((double)planar[0]*planar[0]+(double)planar[1]*planar[1])+(double)planar[2]*planar[2]);
    for(unsigned i=0;i<3;++i) direction[i]=(float)((double)planar[i]/length);
    if(event==F(BYTE*,F(void*,self,112),464) && F(DWORD,self,128))
        MarineTask_DrawPending(self,0,position,direction,event);
    if(!active || !F(DWORD,self,120)) return 0;
    MarineTask_SetBounds(F(BYTE*,self,108),0,(DWORD*)position,(DWORD*)direction);
    double speed=F(float,g_mt_motionState,532);
    float motion[3]={(float)(speed*g_mt_axisX),(float)(speed*g_mt_axisY),(float)(speed*g_mt_axisZ)};
    MarineTask_UpdateMotion(F(BYTE*,self,108),0,motion);
    if(F(DWORD,self,124)) MarineTask_Frame(F(BYTE*,self,108),0,g_mt_frameValue,(DWORD)event);
    return 1;
}

// Marine task construction and scene resources.
extern "C" void FUN_5ef6d0(); KING_ALIAS("_FUN_5ef6d0", "@Node_Construct@16")
extern "C" void FUN_5d3110(); KING_ALIAS("_FUN_5d3110", "@Object_Construct@12")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_5d9290(); KING_ALIAS("_FUN_5d9290", "@Scenes_Find@12")
extern "C" void FUN_5ef390(); KING_ALIAS("_FUN_5ef390", "@Room_FindObject@12")
extern "C" void FUN_5d2190(); KING_ALIAS("_FUN_5d2190", "@Node_Link@12")
ASM_VAR(AsmLabel, g_marineNodeBaseVtable, $L_650430)
ASM_VAR(AsmLabel, g_marineNodeName, $L_685c24)
ASM_VAR(AsmLabel, g_marineCommonLibrary, $L_672688)
ASM_VAR(AsmLabel, g_marineRainNodeName, $L_67ba78)
ASM_VAR(AsmLabel, g_marineZeroName, $L_67c178)
ASM_VAR(AsmLabel, g_marineTaskFile, $L_685bfc)
ASM_VAR(double, g_marineTaskRadiusSquared, $L_653018)
static bool MarineTask_NameEqual(const char* a,const char* b)
{
    while(*a==*b) { if(!*a) return true; ++a; ++b; }
    return false;
}
extern "C" BYTE* __fastcall MarineTask_Construct(BYTE* self, void*, BYTE* world)
{
    CallThis(FUN_5ef6d0,self,g_marineNodeName,24);
    F(DWORD,self,72)=F(DWORD,self,56)=F(DWORD,self,48)=F(DWORD,self,52)=0;
    F(void*,self,0)=g_marineNodeBaseVtable;
    BYTE* secondary=self+76;
    CallThis(FUN_5d3110,secondary,g_marineNodeName);
    for(unsigned i=136;i<=148;i+=4) F(DWORD,self,i)=0;
    F(void*,self,0)=g_marineTaskVtable;F(void*,secondary,0)=g_marineTaskSecondaryVtable;
    g_marineRecordedState=world;F(BYTE*,self,112)=world;
    CallThis(FUN_5d3570,world+2992,secondary);
    CallThis(FUN_5d33d0,secondary,1);
    F(DWORD,self,120)=F(DWORD,self,116)=F(DWORD,self,132)=0;
    BYTE* meshOwner=(BYTE*)operator new(4);
    if(meshOwner) meshOwner=MarineMesh_Create(meshOwner,0,3284664320u,1163575296u,0,
                                            1166843904u,1157234688u,g_mt_frameValue,0);
    F(BYTE*,self,108)=meshOwner;
    float scratch[12],matrix[12]={0},direction[3],position[3];
    const DWORD* result=CallThis<const DWORD*>(FUN_5dbcc0,world,scratch);
    for(unsigned i=0;i<12;++i) F(DWORD,matrix,i*4)=result[i];
    MarineTask_TransformAxis(matrix,direction);
    for(unsigned i=0;i<3;++i) position[i]=MarineTask_QuietCoordinate(matrix+9+i);
    MarineTask_RebuildBounds(F(BYTE*,self,108),0,(DWORD*)position,(DWORD*)direction);
    if(F(DWORD,F(void*,self,112),524)) {
        BYTE* library=CallThis<BYTE*>(FUN_5d9290,F(void*,self,112),g_marineCommonLibrary);
        BYTE* node=CallThis<BYTE*>(FUN_5ef390,library,g_marineRainNodeName);
        if(!node) CallC(FUN_60aad0,0x88762715u,g_marineErrorFormat,g_marineTaskFile,46,g_marineDate);
        F(DWORD,self,44)=F(DWORD,node,44);
        CallThis(FUN_5d2190,self,node);
        F(DWORD,self,36)=F(DWORD,node,36);
    }
    BYTE* library=CallThis<BYTE*>(FUN_5d9290,F(void*,self,112),g_marineCommonLibrary);
    unsigned count=F(unsigned,library,72);
    BYTE* material=0;
    if(count) {
        BYTE* table=F(BYTE*,library,76);
        for(unsigned i=0;i<count;++i) if(MarineTask_NameEqual((char*)(table+i*124),g_marineZeroName)) {
            material=table+i*124;break;
        }
    }
    if(!material) CallC(FUN_60aad0,0x88762715u,g_marineErrorFormat,g_marineTaskFile,61,g_marineDate);
    MarineTask_SetMaterial(F(BYTE*,self,108),0,(DWORD)material);
    F(DWORD,self,136)=3276275712u;F(DWORD,self,140)=1163575296u;F(DWORD,self,144)=0;
    F(DWORD,self,124)=F(DWORD,self,128)=0;
    F(float,self,148)=(float)x87_sqrt(g_marineTaskRadiusSquared);
    return self;
}

// Marine pending-state quad rendering and camera field of view.
extern "C" void FUN_5b1020(); KING_ALIAS("_FUN_5b1020", "@Material_Select@24")
ASM_VAR(float, g_mq_zero, $L_64c264)
ASM_VAR(float, g_mq_one, $L_64c250)
ASM_VAR(float, g_mq_half, $L_64c258)
ASM_VAR(float, g_mq_delay, $L_653008)
ASM_VAR(float, g_mq_offsetScale, $L_65308c)
ASM_VAR(float, g_mq_height, $L_65300c)
ASM_VAR(float, g_mq_lowerHeight, $L_653010)
extern "C" Vec3* __cdecl Vec3_Sum(Vec3*,const Vec3*,const Vec3*);
extern "C" Vec3* __cdecl Vec3_Diff(Vec3*,const Vec3*,const Vec3*);
extern "C" Vec3* __cdecl Vec3_Times(Vec3*,const Vec3*,float);
extern "C" BYTE* __fastcall Fx_Set2Vec(BYTE*,void*,const Vec3*,const Vec3*,int,int);
X87 double MarineCamera_Tan(double angle)
{
    __asm { fld angle }
    __asm { fptan }
    __asm { fstp st(0) }
}
extern "C" double __fastcall MarineCamera_HorizontalAngle(BYTE* self,void*)
{
    double angle=x87_atan2((double)F(float,self,88)*g_mq_half,1.0);
    return angle+angle;
}
extern "C" double __fastcall MarineCamera_VerticalAngle(BYTE* self,void*)
{
    double angle=x87_atan2((double)F(float,self,84)*g_mq_half,1.0);
    return angle+angle;
}
extern "C" void __fastcall MarineTask_DrawPending(BYTE* self,void*,const float* position,const float* direction,BYTE* event)
{
    double elapsed=F(double,F(void*,self,112),2960);
    F(float,self,156)=(float)(F(DWORD,self,116)?elapsed+F(float,self,156):(double)F(float,self,156)-elapsed);
    if(!(F(float,self,156)>g_mq_zero)) return;
    double ratio=(double)F(float,self,156)/g_mq_delay;
    float fade=(float)ratio;
    if(ratio>g_mq_one) fade=1.0f;
    else if(!(fade>=g_mq_zero)) fade=0.0f;
    double distance=F(double,event,8472);
    Vec3 projected={(float)(distance*direction[0]),(float)(distance*direction[1]),(float)(distance*direction[2])};
    Vec3 offset={(float)((double)projected.x*g_mq_offsetScale),(float)((double)projected.y*g_mq_offsetScale),(float)((double)projected.z*g_mq_offsetScale)};
    Vec3 center={(float)((double)offset.x+position[0]),(float)((double)offset.y+position[1]),offset.z};
    float perpendicularX=(float)((double)direction[1]-g_mq_zero);
    Vec3 perpendicular={-perpendicularX,(float)(-((double)g_mq_zero-direction[0])),(float)(-(double)g_mq_zero)};
    Vec3 normal={0,0,1};
    float horizontal=(float)(MarineCamera_HorizontalAngle(F(BYTE*,F(void*,self,112),488),0)*g_mq_half);
    double vertical=MarineCamera_VerticalAngle(F(BYTE*,F(void*,self,112),488),0)*g_mq_half;
    float horizontalSpan=(float)(MarineCamera_Tan(horizontal)*F(double,event,8472));
    double verticalSpan=MarineCamera_Tan(vertical)*F(double,event,8472);
    float radius=(float)x87_sqrt(verticalSpan*verticalSpan+(double)horizontalSpan*horizontalSpan);
    Vec3 scaledNormal={(float)((double)normal.x*g_mq_height),(float)((double)normal.y*g_mq_height),(float)((double)normal.z*g_mq_height)};
    Vec3 lift={(float)((double)scaledNormal.x*fade),(float)((double)scaledNormal.y*fade),(float)((double)scaledNormal.z*fade)};
    Vec3 side={(float)((double)perpendicular.x*radius),(float)((double)perpendicular.y*radius),(float)((double)perpendicular.z*radius)};
    Vec3 left={(float)((double)center.x-side.x),(float)((double)center.y-side.y),(float)((double)center.z-side.z)};
    Vec3 point={(float)((double)lift.x+left.x),(float)((double)lift.y+left.y),(float)((double)lift.z+left.z)};
    DWORD vertices[4][8];
    Fx_Set2Vec((BYTE*)vertices[0],0,&point,&normal,0,0);
    Vec3 right={(float)((double)side.x+center.x),(float)((double)side.y+center.y),(float)((double)side.z+center.z)};
    Vec3_Sum(&point,&right,&lift);
    Fx_Set2Vec((BYTE*)vertices[1],0,&point,&normal,0x3f800000,0);
    Vec3 lower,temporary;
    Vec3_Times(&lower,&normal,g_mq_lowerHeight);
    Vec3_Times(&temporary,&perpendicular,radius);
    Vec3_Sum(&point,&center,&temporary);
    Vec3_Diff(&point,&point,&lower);
    Fx_Set2Vec((BYTE*)vertices[2],0,&point,&normal,0x3f800000,0x3f800000);
    Vec3_Times(&lower,&normal,g_mq_lowerHeight);
    Vec3_Times(&temporary,&perpendicular,radius);
    Vec3_Diff(&point,&center,&temporary);
    Vec3_Diff(&point,&point,&lower);
    Fx_Set2Vec((BYTE*)vertices[3],0,&point,&normal,0,0x3f800000);
    CallThis(FUN_5b1020,event,F(DWORD,self,152),1,0,0);
    BYTE* device=(BYTE*)g_mt_frameValue;
    CallStd((void(*)())F(void*,F(void*,device,0),80),device,136,1);
    device=(BYTE*)g_mt_frameValue;
    CallStd((void(*)())F(void*,F(void*,device,0),100),device,6,274,vertices,4,0);
}

// Marine particle initialization, lifetime and motion.
extern "C" int __cdecl rand();
ASM_VAR(float, g_mp_scale, $L_725f78)
ASM_VAR(float, g_mp_randomUnit, $L_64c254)
ASM_VAR(float, g_mp_angularFactor, $L_64c304)
ASM_VAR(float, g_mp_seconds, $L_64c948)
extern "C" void __fastcall MarineParticle_Start(BYTE* self,void*,const DWORD* position,const DWORD* velocity)
{
    DWORD now=timeGetTime();
    for(unsigned i=0;i<3;++i) F(DWORD,self,i*4)=position[i];
    for(unsigned i=0;i<3;++i) F(DWORD,self,24+i*4)=position[i];
    F(DWORD,self,12)=velocity[0];F(DWORD,self,16)=velocity[1];
    DWORD velocityZ=velocity[2];
    F(DWORD,self,48)=now;F(DWORD,self,20)=velocityZ;
    int first=rand();
    F(float,self,52)=(float)(((double)first*g_mp_scale)*g_mp_randomUnit);
    int second=rand();
    F(DWORD,self,40)=F(DWORD,self,44)+now;
    double randomValue=(double)second*g_mp_randomUnit;
    F(float,self,56)=(float)(((double)g_mp_scale*randomValue)*g_mp_angularFactor);
}
extern "C" void __fastcall MarineParticle_SetLifetime(BYTE* self,void*,DWORD lifetime)
{
    F(DWORD,self,44)=lifetime;
}
extern "C" void __fastcall MarineParticle_Update(BYTE* self,void*,DWORD now)
{
    // Keep FILD of an unsigned tick difference via a zero-extended qword.
    // A direct unsigned-to-double cast rounds differently at 24-bit x87 precision.
    volatile __int64 ticks=(DWORD)(now-F(DWORD,self,48));
    double elapsed=(double)ticks*g_mp_seconds;
    float x=(float)(elapsed*F(float,self,12));
    float y=(float)(elapsed*F(float,self,16));
    double z=elapsed*F(float,self,20);
    float px=(float)((double)x+F(float,self,24));
    float py=(float)((double)y+F(float,self,28));
    float pz=(float)(z+F(float,self,32));
    F(float,self,0)=px;F(float,self,4)=py;F(float,self,8)=pz;
    F(float,self,52)=(float)((double)F(float,self,56)+F(float,self,52));
}

// Marine mesh coordinate mapping and triangle classification.
ASM_VAR(float, g_mg_xLimit, $L_65313c)
ASM_VAR(float, g_mg_yLimit, $L_653138)
ASM_VAR(DWORD, g_mg_width, $L_725e50)
ASM_VAR(DWORD, g_mg_height, $L_725e54)
X87 double MarineGrid_Index(DWORD value)
{
    volatile __int64 index=value;
    return (double)index;
}
extern "C" double __fastcall MarineGrid_MapX(BYTE* self,void*,const DWORD* point)
{
    return MarineGrid_Index(point[0])*F(float,self,192)+F(float,self,176);
}
extern "C" double __fastcall MarineGrid_MapY(BYTE* self,void*,const DWORD* point)
{
    return MarineGrid_Index(point[1])*F(float,self,196)+F(float,self,188);
}
extern "C" void __fastcall MarineGrid_Unmap(BYTE* self,void*,const DWORD* point,float* x,float* y)
{
    *x=(float)((MarineGrid_Index(point[0])-F(float,self,152))/F(float,self,100));
    *y=(float)((MarineGrid_Index(point[1])-F(float,self,156))/F(float,self,104));
}
extern "C" DWORD __fastcall MarineGrid_Outside(BYTE* self,void*,const DWORD* point)
{
    BYTE* grid=F(BYTE*,self,4);
    if(MarineGrid_MapX(grid,0,point)>g_mg_xLimit) return 1;
    return MarineGrid_MapY(grid,0,point)>g_mg_yLimit;
}
extern "C" DWORD __fastcall MarineGrid_ClassifyTriangle(BYTE* self,void*,BYTE* triangle)
{
    DWORD points[3][2];
    for(unsigned i=0;i<3;++i) {
        const DWORD* vertex=F(DWORD*,triangle,16+i*4);
        points[i][0]=vertex[0];points[i][1]=vertex[1];
    }
    DWORD first=MarineGrid_Outside(self,0,points[0]);
    DWORD second=MarineGrid_Outside(self,0,points[1]);
    DWORD third=MarineGrid_Outside(self,0,points[2]);
    if(first && second && third) return 4;
    if(!first && !second && !third) return 1;
    return 2;
}
extern "C" DWORD __cdecl MarineGrid_Width() { return g_mg_width; }
extern "C" DWORD __cdecl MarineGrid_Height() { return g_mg_height; }

// Marine particle pool allocation, emission and randomized geometry.
extern "C" void* __cdecl malloc(size_t);
extern "C" void __cdecl srand(unsigned);
ASM_VAR(DWORD, g_mpool_enabled, $L_725e30)
ASM_VAR(float, g_mpool_angle, $L_64c334)
ASM_VAR(float, g_mpool_colorScale, $L_64c48c)
ASM_VAR(float, g_mpool_half, $L_64c258)
extern "C" void __fastcall MarineParticlePool_BuildGeometry(BYTE* self,void*,DWORD count,DWORD color)
{
    F(void*,self,8)=malloc(count*16u);
    if(!count) return;
    float alpha=(float)(color>>24),red=(float)((color>>16)&255),green=(float)((color>>8)&255),blue=(float)(color&255);
    DWORD offset=0;
    do {
        int random=rand();float first=(float)((double)random*g_mp_randomUnit);
        random=rand();float second=(float)((double)random*g_mp_randomUnit);
        random=rand();float third=(float)((double)random*g_mp_randomUnit);
        random=rand();float fourth=(float)((double)random*g_mp_randomUnit);
        random=rand();double fifth=(double)random*g_mp_randomUnit;
        double dx=(double)fourth-fifth;float x=(float)(dx+dx);
        double dy=(double)second-third;float y=(float)(dy+dy);
        random=rand();BYTE* vertex=F(BYTE*,self,8)+offset;
        F(float,vertex,0)=x;F(float,vertex,4)=y;
        F(float,vertex,8)=(float)(((double)random*g_mp_randomUnit-first)*g_mpool_angle);
        random=rand();double brightness=(double)random*g_mpool_colorScale+g_mpool_half;
        DWORD packed=(DWORD)(__int64)((double)alpha*brightness);
        packed=(packed<<8)|(DWORD)(__int64)((double)red*brightness);
        packed=(packed<<8)|(DWORD)(__int64)((double)green*brightness);
        packed=(packed<<8)|(DWORD)(__int64)((double)blue*brightness);
        F(DWORD,F(BYTE*,self,8)+offset,12)=packed;
        offset+=16;
    } while(--count);
}
extern "C" BYTE* __fastcall MarineParticlePool_Construct(BYTE* self,void*,DWORD capacity,DWORD vertices,DWORD color,DWORD owner)
{
    F(DWORD,self,0)=owner;F(DWORD,self,20)=capacity;F(DWORD,self,24)=vertices;
    DWORD now=timeGetTime();
    DWORD bytes=capacity*60u;
    BYTE* particles=(BYTE*)malloc(bytes);F(BYTE*,self,12)=particles;
    for(DWORD i=0;i<bytes/4;++i) ((volatile DWORD*)particles)[i]=0;
    srand(now);
    MarineParticlePool_BuildGeometry(self,0,F(DWORD,self,24),color);
    F(DWORD,self,32)=6000;F(DWORD,self,16)=F(DWORD,self,28)=0;F(DWORD,self,4)=1;
    return self;
}
extern "C" DWORD __fastcall MarineParticlePool_Add(BYTE* self,void*,const DWORD* position,const DWORD* velocity)
{
    if(!F(DWORD,self,4) || F(DWORD,self,16)>=F(DWORD,self,20) || !g_mpool_enabled) return 0;
    MarineParticle_Start(F(BYTE*,self,12)+F(DWORD,self,16)*60u,0,position,velocity);
    MarineParticle_SetLifetime(F(BYTE*,self,12)+F(DWORD,self,16)*60u,0,F(DWORD,self,32));
    ++F(DWORD,self,16);
    return 1;
}
extern "C" DWORD __fastcall MarineParticlePool_Remove(BYTE* self,void*,DWORD index)
{
    DWORD count=F(DWORD,self,16);
    if(index>=count) return 0;
    DWORD last=count-1;F(DWORD,self,16)=last;
    if(index!=last) {
        BYTE* particles=F(BYTE*,self,12);
        volatile DWORD* from=(volatile DWORD*)(particles+last*60u);
        volatile DWORD* to=(volatile DWORD*)(particles+index*60u);
        for(unsigned i=0;i<15;++i) to[i]=from[i];
    }
    return 1;
}
extern "C" DWORD __fastcall MarineParticlePool_CanAdd(BYTE* self,void*)
{
    return F(DWORD,g_marineApplicationState,1300)!=2 && F(DWORD,self,4) && g_mpool_enabled && F(DWORD,self,16)<F(DWORD,self,20);
}
extern "C" void __fastcall MarineParticlePool_SetFloor(BYTE* self,void*,DWORD height) { F(DWORD,self,28)=height; }
extern "C" void __fastcall MarineParticlePool_SetLifetime(BYTE* self,void*,DWORD lifetime) { F(DWORD,self,32)=lifetime; }

// Marine 4x4 matrix construction and composition.
extern "C" DWORD* __cdecl MarineInit_Identity(DWORD*);
extern "C" DWORD* __cdecl MarineMatrix_Zero(DWORD* out)
{
    for(unsigned i=0;i<16;++i) out[i]=0;
    return out;
}
extern "C" DWORD* __cdecl MarineMatrix_RotateZ(DWORD* out,float angle)
{
    float cosine=(float)x87_cos(angle),sine=(float)x87_sin(angle);
    DWORD matrix[16];MarineInit_Identity(matrix);
    F(float,matrix,0)=cosine;F(float,matrix,4)=-sine;
    F(float,matrix,16)=sine;F(float,matrix,20)=cosine;
    for(unsigned i=0;i<16;++i) out[i]=matrix[i];
    return out;
}
extern "C" DWORD* __cdecl MarineMatrix_Translate(DWORD* out,DWORD x,DWORD y,DWORD z)
{
    DWORD matrix[16];MarineInit_Identity(matrix);
    matrix[12]=x;matrix[13]=y;matrix[14]=z;
    for(unsigned i=0;i<16;++i) out[i]=matrix[i];
    return out;
}
static __forceinline double MarineMatrix_Product(const float* right,const float* left)
{
    // FLD of the right operand precedes FMUL of the left, including signaling NaNs.
    volatile double product=*right;
    product=product*(*left);
    return product;
}
extern "C" DWORD* __cdecl MarineMatrix_Multiply(DWORD* out,const float* right,const float* left)
{
    DWORD matrix[16];MarineMatrix_Zero(matrix);
    for(unsigned row=0;row<4;++row) for(unsigned column=0;column<4;++column) {
        double sum=(double)F(float,matrix,(row*4+column)*4);
        for(unsigned k=0;k<4;++k) sum=MarineMatrix_Product(right+k*4+column,left+row*4+k)+sum;
        F(float,matrix,(row*4+column)*4)=(float)sum;
    }
    for(unsigned i=0;i<16;++i) out[i]=matrix[i];
    return out;
}

// Marine particle rendering, retirement and per-frame updates.
struct MarineParticleMatrix { DWORD v[16]; };
ASM_VAR(MarineParticleMatrix, g_mpool_restoreMatrix, $L_725f38)
extern "C" void __fastcall MarineParticlePool_Render(BYTE* self,void*,BYTE* device,BYTE* event,DWORD material)
{
    if(!g_mpool_enabled) return;
    DWORD now=timeGetTime();
    DWORD translation[16],rotation[16],matrix[16];
    for(DWORD index=0,offset=0;index<F(DWORD,self,16);++index,offset+=60) {
        BYTE* particle=F(BYTE*,self,12)+offset;
        DWORD x=F(DWORD,particle,0),y=F(DWORD,particle,4);
        float height=(float)CallVirt<double>(F(void*,self,0),40,particle);
        MarineMatrix_Translate(translation,x,y,F(DWORD,&height,0));
        MarineMatrix_RotateZ(rotation,F(float,F(BYTE*,self,12)+offset,52));
        MarineMatrix_Multiply(matrix,(float*)translation,(float*)rotation);
        CallStd((void(*)())F(void*,F(void*,device,0),44),device,1,matrix);
        CallThis(FUN_5b1020,event,material,0,0,0);
        CallStd((void(*)())F(void*,F(void*,device,0),80),device,136,1);
        CallStd((void(*)())F(void*,F(void*,device,0),100),device,1,66,F(void*,self,8),F(DWORD,self,24),0);
    }
    for(DWORD index=0,offset=0;index<F(DWORD,self,16);++index,offset+=60) {
        BYTE* particle=F(BYTE*,self,12)+offset;
        if(now>F(DWORD,particle,40) || !(F(float,particle,8)>=F(float,self,28)))
            MarineParticlePool_Remove(self,0,index);
        // The original updates this slot even when removal moved the last particle into it.
        MarineParticle_Update(F(BYTE*,self,12)+offset,0,now);
    }
    CallStd((void(*)())F(void*,F(void*,device,0),44),device,1,g_mpool_restoreMatrix.v);
}

// Marine mesh work-item priority queues and shared link removal.
extern "C" BYTE* __fastcall MarineQueue_RemoveLink(BYTE* self,void*,BYTE* node)
{
    BYTE* first=F(BYTE*,self,0),*last=F(BYTE*,self,4);
    if(first && F(BYTE*,first,0)==node && F(BYTE*,first,4)==node) {
        if(first==node) F(BYTE*,self,0)=0;
        else {F(BYTE*,first,4)=first;F(BYTE*,F(BYTE*,self,0),0)=first;}
    } else if(last && F(BYTE*,last,0)==node && F(BYTE*,last,4)==node) {
        if(last==node) F(BYTE*,self,4)=0;
        else {F(BYTE*,last,4)=last;F(BYTE*,F(BYTE*,self,4),0)=last;}
    } else {
        if(first==node) F(BYTE*,self,0)=F(BYTE*,node,0);
        if(last==node) F(BYTE*,self,4)=F(BYTE*,node,0);
        F(BYTE*,F(BYTE*,node,0),4)=F(BYTE*,node,4);
        F(BYTE*,F(BYTE*,node,4),0)=F(BYTE*,node,0);
    }
    F(BYTE*,node,4)=node;F(BYTE*,node,0)=node;
    return node;
}
extern "C" DWORD __fastcall MarineQueue_Empty(BYTE* self,void*)
{
    return !F(DWORD,self,0) && !F(DWORD,self,4);
}
extern "C" DWORD __fastcall MarineQueue_RebuildWorkItems(BYTE* self, void*)
{
    BYTE* second = F(BYTE*, self, 4);
    F(DWORD, self, 8) = 0;
    if (!second) return 0;
    BYTE* first = F(BYTE*, self, 0);
    if (!first) {
        F(BYTE*, self, 0) = second;
        F(BYTE*, self, 4) = 0;
        return (DWORD)second;
    }
    BYTE* firstTail = F(BYTE*, first, 4);
    BYTE* secondTail = F(BYTE*, second, 4);
    BYTE* target;
    bool firstExhausted = false;
    while (true) {
        while (!(F(float, first, 104) >= F(float, second, 104))) {
            if (first == firstTail) {
                target = F(BYTE*, self, 0);
                firstExhausted = true;
                goto finish;
            }
            first = F(BYTE*, first, 0);
        }
        BYTE* runEnd = second;
        while (F(float, first, 104) >= F(float, runEnd, 104)) {
            if (runEnd == secondTail) {
                if (first == F(BYTE*, self, 0)) F(BYTE*, self, 0) = second;
                target = first;
                goto finish;
            }
            runEnd = F(BYTE*, runEnd, 0);
        }
        if (first == F(BYTE*, self, 0)) F(BYTE*, self, 0) = second;
        MarineQueue_SpliceRun(second, runEnd, first);
        second = runEnd;
    }
finish:
    BYTE* previous = F(BYTE*, target, 4);
    F(BYTE*, second, 4) = previous;
    F(BYTE*, secondTail, 0) = target;
    F(BYTE*, previous, 0) = second;
    F(BYTE*, target, 4) = secondTail;
    F(BYTE*, self, 4) = 0;
    return (DWORD)(firstExhausted ? target : previous);
}
extern "C" DWORD __fastcall MarineQueue_InsertWorkItem(BYTE* self, void*, BYTE* edge)
{
    BYTE* last = F(BYTE*, self, 4);
    if (!last) {
        F(BYTE*, self, 4) = edge;
    } else {
        float score = F(float, edge, 104);
        if (F(float, last, 104) >= score) {
            MarineQueue_LinkBefore(last, edge);
            F(BYTE*, self, 4) = edge;
        } else {
            BYTE* position = last;
            if (F(float, F(BYTE*, last, 4), 104) > score) {
                position = F(BYTE*, last, 0);
                while (!(F(float, position, 104) >= score))
                    position = F(BYTE*, position, 0);
            }
            MarineQueue_LinkBefore(position, edge);
        }
    }
    DWORD count = ++F(DWORD, self, 8);
    if(count<200) return count;
    bool hasFirst=F(DWORD,self,0)!=0;
    DWORD result=MarineQueue_RebuildWorkItems(self,0);
    return hasFirst?result:count;
}
extern "C" DWORD __fastcall MarineQueue_SetWorkPriority(BYTE* self,void*,BYTE* node,DWORD priority)
{
    MarineQueue_RemoveLink(self,0,node);
    F(DWORD,node,104)=priority;
    MarineQueue_InsertWorkItem(self,0,node);
    return ++F(DWORD,self,8);
}
extern "C" BYTE* __fastcall MarineQueue_TopWorkItem(BYTE* self,void*)
{
    BYTE* first=F(BYTE*,self,0),*last=F(BYTE*,self,4);
    if(!first) return last;
    if(!last) return first;
    return !(F(float,last,104)>=F(float,first,104))?last:first;
}

// Marine deferred node insertion, reprioritization and selection.
extern "C" DWORD __fastcall MarineQueue_InsertDeferredNode(BYTE* self, void*, BYTE* node)
{
    BYTE* last = F(BYTE*, self, 4);
    if (!last) {
        F(BYTE*, self, 4) = node;
    } else {
        float score = F(float, node, 28);
        if (!(score < g_mq_zero) && !(score > g_mq_zero)) {
            MarineQueue_LinkBefore(last + 4, node ? node + 4 : 0);
            return ++F(DWORD,self,8);
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
    return count>=500?MarineQueue_RebuildNodes(self,0):count;
}
extern "C" DWORD __fastcall MarineQueue_SetDeferredPriority(BYTE* self,void*,BYTE* node,DWORD priority)
{
    MarineQueue_RemoveNode(self,0,node);
    F(DWORD,node,28)=priority;
    MarineQueue_InsertDeferredNode(self,0,node);
    return ++F(DWORD,self,8);
}
extern "C" BYTE* __fastcall MarineQueue_TopDeferredNode(BYTE* self,void*)
{
    BYTE* first=F(BYTE*,self,0),*last=F(BYTE*,self,4);
    if(!first) return last;
    if(!last) return first;
    return F(float,last,28)>F(float,first,28)?last:first;
}

// Marine mesh scratch counters, coordinate records and buffer cleanup.
ASM_VAR(float, g_ms_recordType, $L_6530c0)
extern "C" DWORD __fastcall MarineScratch_ResetCounts(BYTE* self,void*)
{
    F(DWORD,self,136)=0;F(DWORD,self,128)=0;F(DWORD,self,120)=0;
    return 0;
}
extern "C" DWORD __fastcall MarineScratch_AppendPoint(BYTE* self,void*,const DWORD* point)
{
    BYTE* entries=F(BYTE*,self,124);DWORD offset=F(DWORD,self,120)*48u;
    volatile double type=g_ms_recordType;
    F(float,entries+offset,0)=(float)type;
    entries=F(BYTE*,self,124);volatile __int64 x=point[0];offset=F(DWORD,self,120)*48u;
    F(float,entries+offset,4)=(float)x;
    volatile __int64 y=point[1];offset=F(DWORD,self,120)*48u;entries=F(BYTE*,self,124);
    F(float,entries+offset,8)=(float)y;
    ++F(DWORD,self,120);
    return offset;
}
extern "C" void __fastcall MarineScratch_ReleaseAuxiliary(BYTE* self,void*)
{
    void* buffer=F(void*,self,132);
    if(buffer) {operator delete(buffer);F(DWORD,self,132)=0;}
    buffer=F(void*,self,140);
    if(buffer) {operator delete(buffer);F(DWORD,self,140)=0;}
}
extern "C" void __fastcall MarineScratch_ReleasePoints(BYTE* self,void*)
{
    void* buffer=F(void*,self,124);
    if(buffer) {operator delete(buffer);F(DWORD,self,124)=0;}
}

// Marine mesh motion-response construction and degree-table trigonometry.
struct MarineAngleTable { float v[360]; };
ASM_VAR(float, g_mc_724960, $L_724960)
ASM_VAR(float, g_mc_64c264, $L_64c264)
ASM_VAR(float, g_mc_6530d0, $L_6530d0)
ASM_VAR(float, g_mc_6530ac, $L_6530ac)
ASM_VAR(float, g_mc_653004, $L_653004)
ASM_VAR(float, g_mc_653000, $L_653000)
ASM_VAR(float, g_mc_725ecc, $L_725ecc)
ASM_VAR(float, g_mc_64c338, $L_64c338)
ASM_VAR(float, g_mc_64c350, $L_64c350)
ASM_VAR(float, g_mc_64d7b8, $L_64d7b8)
ASM_VAR(float, g_mc_6530b0, $L_6530b0)
ASM_VAR(float, g_mc_6530b4, $L_6530b4)
ASM_VAR(float, g_mc_650330, $L_650330)
ASM_VAR(float, g_mc_64c320, $L_64c320)
ASM_VAR(float, g_mc_650804, $L_650804)
ASM_VAR(float, g_mc_725ed0, $L_725ed0)
ASM_VAR(float, g_mc_725ed4, $L_725ed4)
ASM_VAR(float, g_mc_725ed8, $L_725ed8)
ASM_VAR(MarineAngleTable, g_mc_sine, $L_7243bc)
ASM_VAR(MarineAngleTable, g_mc_cosine, $L_724964)
extern "C" double __cdecl MarineAngle_Sine(float angle)
{
    bool negative=!(angle>=g_mc_64c264);
    int index=(int)(DWORD)(__int64)((double)g_mc_724960*angle);
    if(negative) index=(int)(0u-(DWORD)index);
    double value=g_mc_sine.v[index%360];
    return negative?-value:value;
}
extern "C" double __cdecl MarineAngle_Cosine(float angle)
{
    double value=angle;
    if(!(value>=g_mc_64c264)) value=-value;
    int index=(int)(DWORD)(__int64)((double)g_mc_724960*value);
    return g_mc_cosine.v[index%360];
}
static void MarineMesh_ResponseValues(BYTE* response,double length)
{
    F(float,response,16)=(float)length;
    if(length>g_mc_6530d0) F(float,response,16)=g_mc_6530d0;
    double radius=(double)F(float,response,16)/((double)g_mc_6530ac*g_mc_653004);
    F(float,response,4)=(float)radius;
    double energy=((radius*radius)*g_mc_6530ac)/g_mc_725ecc;
    F(float,response,8)=(float)energy;
    double target=energy*g_mc_653000+g_mc_64c338;
    F(float,response,28)=(float)target;
    if(target>g_mc_64c350) F(DWORD,response,28)=0x3fc00000;
    F(DWORD,response,20)=1161527296u;F(DWORD,response,12)=0;
    F(float,response,4)=(float)((double)F(float,response,4)*g_mc_64d7b8);
}
static void MarineMesh_ResponseRange(BYTE* owner,double value)
{
    bool active=value>=g_mc_6530b0 && !(value>g_mc_6530b4);
    BYTE* transition=F(BYTE*,owner,112);
    if(transition) MarineTransition_SetActive(transition,0,active?1:0);
}
extern "C" void __fastcall MarineMesh_CreateMotionResponse(BYTE* self,void*,DWORD xBits,DWORD yBits,DWORD zBits)
{
    BYTE* response=(BYTE*)operator new(36);
    if(response) {
        F(BYTE*,response,0)=F(BYTE*,self,4);
        float x=g_mc_725ed0,y=g_mc_725ed4,z=g_mc_725ed8;
        F(float,response,12)=(float)x87_atan2(x,y);
        double length=x87_sqrt(((double)x*x+(double)y*y)+(double)z*z);
        MarineMesh_ResponseValues(response,length);
        BYTE* owner=F(BYTE*,response,0);
        F(DWORD,owner,76)=0;
        F(float,owner,84)=(float)MarineAngle_Sine(0.0f);
        F(float,owner,80)=(float)MarineAngle_Cosine(F(float,owner,76));
        double value=F(float,response,28);
        F(float,F(void*,response,0),72)=(float)value;
        MarineMesh_ResponseRange(F(BYTE*,response,0),value);
        F(float,F(void*,response,0),64)=(float)((double)g_mc_650330/g_mc_725ecc);
        F(float,F(void*,response,0),68)=(float)((double)F(float,response,4)/g_mc_725ecc);
        F(float,F(void*,response,0),56)=(float)(((double)F(float,response,8)*g_mc_64c320)/g_mc_725ecc);
        F(float,F(void*,response,0),60)=(float)((double)F(float,response,8)/g_mc_725ecc);
        owner=F(BYTE*,response,0);
        F(float,owner,92)=(float)(((double)F(float,owner,56)*g_mc_650804)/F(float,owner,64));
        F(float,owner,96)=(float)(((double)F(float,owner,60)*g_mc_650804)/F(float,owner,68));
        F(float,F(void*,response,0),108)=F(float,response,20);
    }
    F(BYTE*,self,84)=response;
    double previous=F(float,response,28);
    float x=F(float,&xBits,0),y=F(float,&yBits,0),z=F(float,&zBits,0);
    F(float,response,12)=(float)x87_atan2(x,y);
    double length=x87_sqrt(((double)z*z+(double)y*y)+(double)x*x);
    MarineMesh_ResponseValues(response,length);
    F(DWORD,response,32)=F(DWORD,response,28);
    F(float,response,28)=(float)previous;
    BYTE* owner=F(BYTE*,response,0);
    F(float,owner,72)=(float)previous;
    MarineMesh_ResponseRange(owner,previous);
}

// ---------------------------------------------------------------------------
#include <stdarg.h>
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// [FUN_60aa60..$L_60aab3] a grid message: printf-style text, fatal for level 2, else logged
extern "C" void __cdecl Grd_Message(int level, const char* fmt, ...)
{
    char text[256];
    text[0] = 0;
    va_list va;
    va_start(va, fmt);
    wvsprintfA(text + lstrlenA(text), fmt, va);
    va_end(va);
    if (level == 2)
        Sys_Fatal(text, 0);
    else
        CallC(FUN_5d2ba0, text, 0);
}

