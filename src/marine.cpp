// Marine mesh processing and task helpers. ASM_MARINE selects the original code.
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
extern "C" BYTE* __fastcall MarineTask_SetMode(BYTE* self, void*, DWORD mode)
{
    BYTE* mesh = F(BYTE*, self, 0);
    F(DWORD, F(void*, mesh, 4), 116) = mode;
    return mesh;
}
extern "C" DWORD __fastcall MarineTask_GetMode(BYTE* self, void*)
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
ASM_PROC(FUN_60fee0)
ASM_PROC(FUN_60fc10)
ASM_PROC(FUN_60cdd0)
ASM_PROC(FUN_5ef780)
ASM_PROC(FUN_5d3290)
ASM_PROC(FUN_5ef660)
ASM_PROC(FUN_61c350)
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
    CallThis(FUN_60fee0, mesh, position, first, second, fourth);
    CallThis(FUN_60fc10, F(void*,self,0), 0x41000000u, 0u, 0u);
    return self;
}
extern "C" void __fastcall MarineMesh_Destruct(BYTE* self, void*)
{
    F(void*,self,0)=g_marineMeshVtable;
    BYTE* sink=F(BYTE*,self,4);
    if(sink) { CallVirt(sink,0,1); F(BYTE*,self,4)=0; }
    void* data=F(void*,self,84);
    if(data) { operator delete(data); F(void*,self,84)=0; }
    CallThis(FUN_60cdd0,self);
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
    DWORD* value=CallC<DWORD*>(FUN_61c350,result,(DWORD*)(self+60),(DWORD*)(self+20),factor);
    destination=(DWORD*)(F(BYTE*,F(void*,self,100),16)+(unsigned)F(WORD,indexNode,0)*40);
    for(unsigned i=0;i<10;++i) destination[i]=value[i];
    return 0;
}
