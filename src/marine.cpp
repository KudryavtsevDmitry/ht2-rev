// Marine mesh processing (Greed.cpp). ASM_MARINE selects the original code.
#include "king.h"
#include <mmsystem.h>
#include <stdlib.h>
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
typedef char AsmLabel[1];
extern "C" void __fastcall Lights_Destruct(BYTE*, void*); // aeffects.cpp
ASM_PROC(FUN_613500)
ASM_PROC(FUN_60f4f0)
ASM_PROC(FUN_60f7b0)
ASM_PROC(FUN_605900)
ASM_PROC(FUN_60f690)
ASM_PROC(FUN_6136f0)
ASM_PROC(FUN_606750)
ASM_PROC(FUN_6069d0)
ASM_PROC(FUN_60aad0)
ASM_PROC(FUN_60aa60)
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
        CallThis(FUN_60f4f0, list, node, score);
        F(BYTE*, self, 40) = next;
        if ((DWORD)(timeGetTime() - start) >= 1000)
            break;
    } while (F(BYTE*, self, 40));
    while ((unsigned)F(WORD, self, 38) < CallVirt<unsigned>(F(void*, self, 4), 8)) {
        BYTE* node = CallThis<BYTE*>(FUN_60f7b0, F(void*, self, 32));
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
    CallThis(FUN_60f690, F(void*, self, 32), node);
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
