// object_ids.cpp - small functions of the unit at 0x57c970 of king.masm:
// ids of the game's objects (300 slots from 100 on), the moving object list,
// per-race state reset, and two object constructors.
//
// Building with  nmake ASM_OBJECT_IDS=1  makes king.masm assemble the
// original procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef DWORD Dwords[1];

extern "C" void FUN_5cb9c0(); KING_ALIAS("_FUN_5cb9c0", "_Set70ebf0")
extern "C" void FUN_592bf0(); KING_ALIAS("_FUN_592bf0", "@ListItem592bf0_Construct@12")    // base constructor

ASM_VAR(BYTE*, g_movingList, $L_6f653c)
ASM_VAR(int, g_67844c, $L_67844c)
ASM_VAR(WORD, g_67d10a, $L_67d10a)
ASM_VAR(int, g_nextId, $L_67d10c)
ASM_VAR(int, g_6f6620, $L_6f6620)
ASM_VAR(AsmLabel, g_idUsed, $L_6f63e4)          // 300 flags
ASM_VAR(AsmLabel, g_6f6380, $L_6f6380)
ASM_VAR(DWORD, g_6f65b8, $L_6f65b8)
ASM_VAR(DWORD, g_6f65bc, $L_6f65bc)
ASM_VAR(DWORD, g_6f65c0, $L_6f65c0)
ASM_VAR(DWORD, g_6f65c4, $L_6f65c4)
ASM_VAR(Dwords, g_6f65f8, $L_6f65f8)
ASM_VAR(Dwords, g_6f65ec, $L_6f65ec)
ASM_VAR(Dwords, g_6f6570, $L_6f6570)
ASM_VAR(Dwords, g_6f65a0, $L_6f65a0)
ASM_VAR(Dwords, g_6f65c8, $L_6f65c8)
ASM_VAR(Dwords, g_6f6518, $L_6f6518)
ASM_VAR(DWORD, g_6f6610, $L_6f6610)
ASM_VAR(DWORD, g_6f6614, $L_6f6614)
ASM_VAR(DWORD, g_6f6618, $L_6f6618)
ASM_VAR(DWORD, g_6f65e8, $L_6f65e8)
ASM_VAR(Dwords, g_7025f0, $L_7025f0)
ASM_VAR(AsmLabel, g_vtbl6504f0, $L_6504f0)

static void Zero(DWORD* p, int n)
{
    for (int k = 0; k < n; k++)
        p[k] = 0;
}

extern "C" {

// [FUN_57c9a0]
int __cdecl Id_Same(int x)
{
    return x;
}

// [FUN_57c9b0] the moving object of o
BYTE* __cdecl Moving_Find(BYTE* o)
{
    BYTE* p = g_movingList;
    if (!p)
        return 0;
    for (BYTE* c = p - 4; c;) {
        if (c - 376 == o)
            return c - 376;
        c = F(BYTE*, c, 8);
        if (!c)
            return 0;
        c -= 4;
    }
    return 0;
}

// [FUN_57c9e0]
BYTE* __cdecl Moving_Of(BYTE* o)
{
    if (!o)
        return 0;
    return Moving_Find((BYTE*)Id_Same((int)o));
}

// [FUN_57ca00]
void __cdecl Race_SetMode(WORD v)
{
    g_67d10a = v;
    g_6f6620 = 0;
    CallC(FUN_5cb9c0, g_67844c);
}

// [FUN_57ca30] a free object id
int __cdecl Id_New()
{
    g_nextId++;
    for (int i = 0; i < 300; i++)
        if (!g_idUsed[i]) {
            g_idUsed[i] = 1;
            g_nextId = i + 100;
            return i + 100;
        }
    return g_nextId;
}

// [FUN_57ca70]
void __cdecl Id_Free6f6380(int i)
{
    g_6f6380[i] = 0;
}

// [FUN_57ca80]
void __cdecl Ids_Reset()
{
    Zero((DWORD*)(char*)g_idUsed, 75);
    g_movingList = 0;
    g_nextId = 100;
}

// [FUN_57cab0]
void __cdecl Race_ResetState()
{
    g_6f65c0 = g_6f65b8;
    g_6f65c4 = g_6f65bc;
    Zero(g_6f65f8, 6);
    Zero(g_6f65ec, 6);
    Zero(g_6f6570, 12);
    Zero(g_6f65a0, 12);
    Zero(g_6f65c8, 6);
    g_6f6610 = 0;
    g_6f6614 = 0;
    g_6f65b8 = 0;
    Zero(g_6f6518, 8);
    g_6f65bc = 0;
    g_6f65e8 = 0;
    g_6f6618 = 0;
}

// [FUN_57cb50]
BYTE* __fastcall Object57cb50_Construct(BYTE* self, void*, int a)
{
    CallThis(FUN_592bf0, self, a);
    F(void*, self, 0) = g_vtbl6504f0;
    return self;
}

// [FUN_57cfc0]
void __fastcall Placement_Init(BYTE* self, void*)
{
    Zero((DWORD*)(self + 20), 12);
    Zero((DWORD*)(self + 68), 12);
    for (int k = 0; k < 12; k++)
        F(DWORD, self, 68 + k * 4) = g_7025f0[k];
    Zero((DWORD*)(self + 116), 12);
    for (int k = 0; k < 12; k++)
        F(DWORD, self, 116 + k * 4) = g_7025f0[k];
    Zero((DWORD*)(self + 168), 6);
}

}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

extern "C" void FUN_57ca70(); KING_ALIAS("_FUN_57ca70", "_Id_Free6f6380")
extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")
extern "C" void FUN_587c50(); KING_ALIAS("_FUN_587c50", "@Slots6_Remove@12")
extern "C" void FUN_592cf0(); KING_ALIAS("_FUN_592cf0", "@Linked592cf0_Destruct@8")
extern "C" void FUN_5c6210(); KING_ALIAS("_FUN_5c6210", "@NetItem_Destroy@4")
ASM_VAR(AsmLabel, g_objids650544, $L_650544)
ASM_VAR(AsmLabel, g_objids6504f8, $L_6504f8)
ASM_VAR(AsmLabel, g_objids6504f4, $L_6504f4)
ASM_VAR(BYTE*, g_objids6f653c, $L_6f653c)

// [FUN_57d490] a moving item (vtables 650544, 6504f8 at +256, 6504f4 at +376): its id freed, its parts
// (+3468, count +3504) deleted, the other items forget it
extern "C" void __fastcall MovingItem_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_objids650544;
    F(void*, s, 256) = g_objids6504f8;
    F(void*, s, 376) = g_objids6504f4;
    CallC(FUN_57ca70, F(int, s, 20));
    for (int k = 0; k < F(int, s, 3504); k++) {
        BYTE** p = &F(BYTE*, s, 3468 + k * 4);
        if (!*p)
            continue;
        CallThis(FUN_5d21b0, *p);
        if (*p)
            CallVirt(*p, 0, 1);
        *p = 0;
    }
    BYTE* it = g_objids6f653c;
    if (it) {
        for (BYTE* e = it - 4; e;) {
            CallThis(FUN_587c50, e - 376, s);
            BYTE* n = F(BYTE*, e, 8);
            if (!n)
                break;
            e = n - 4;
        }
    }
    CallThis(FUN_592cf0, s ? s + 376 : 0);
    CallThis(FUN_5c6210, s);
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_57d490(); KING_ALIAS("_FUN_57d490", "@MovingItem_Destroy@4")
// [$L_57cfa0]
extern "C" BYTE* __fastcall Dtor57cfa0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_57d490, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Object placement.

extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
extern "C" void FUN_4e5bf5(); KING_ALIAS("_FUN_4e5bf5", "_Obj_Lookup4e5b22")
// [$L_580ff0] places the object: its world matrix (+692 by +500), the cell looked up from it (or +800 after slot
// 28), then slot 64 with the matrix and the cell
extern "C" void __fastcall Obj580ff0_Place(BYTE* s, void*)
{
    float m[12];
    CallC(FUN_5aba40, m, s + 692, s + 500);
    int c = CallC<int>(FUN_4e5bf5, F(int, s, 800), s + 536, m + 8, s + 1132);
    if (!c) {
        c = F(int, s, 800);
        CallVirt(s, 28, 1);
    }
    CallVirt(s, 64, m, c);
}

// === the moving object base (FUN_57d060)

ASM_VAR(AsmLabel, g_mo650544, $L_650544)
ASM_VAR(AsmLabel, g_mo6504f8, $L_6504f8)
ASM_VAR(AsmLabel, g_mo6504f4, $L_6504f4)
ASM_VAR(AsmLabel, g_mo6f653c, $L_6f653c)
extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
extern "C" void FUN_4bd4e0(); KING_ALIAS("_FUN_4bd4e0", "@Matrix_IdentityThis@8")
extern "C" void FUN_4e5ba7(); KING_ALIAS("_FUN_4e5ba7", "_Query_HitObject")
extern "C" void FUN_57ca30(); KING_ALIAS("_FUN_57ca30", "_Id_New")
extern "C" void FUN_57cb50(); KING_ALIAS("_FUN_57cb50", "@Object57cb50_Construct@12")
extern "C" void FUN_57cfc0(); KING_ALIAS("_FUN_57cfc0", "@Placement_Init@8")
ASM_PROC(FUN_57d860)
extern "C" void FUN_5c3070(); KING_ALIAS("_FUN_5c3070", "@Table_Add6@20")
ASM_PROC(FUN_5c6170)
ASM_PROC(FUN_5c5f60)
extern "C" void FUN_5c32f0(); KING_ALIAS("_FUN_5c32f0", "@List_Add6@20")
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")

struct Mo_M48 { DWORD d[12]; };

static void Mo_Zero(BYTE* s, int from, int to)
{
    for (int k = from; k <= to; k += 4)
        F(int, s, k) = 0;
}

// matrix at s+off: its rows constructed, then the identity
static void Mo_Matrix(BYTE* s, int off)
{
    CallStd(FUN_4011e0, s + off, 12, 4, (void*)FUN_4bd4d0);
    memcpy(s + off, g_7025f0, 48);
}

// registers s+off (size n) in world w's table (+7976) under the next id (+368); the entry goes to +3468
static void Mo_Register(BYTE* s, BYTE* w, int off, int n, int table, void (*add)())
{
    int id = F(int, s, 368)++;
    int e = CallThis<int>(add, w + table, id, s + off, n);
    F(int, s, 3468 + F(int, s, 3504) * 4) = e;
    F(int, s, 3504)++;
}

// the moving object over the scene object base made by base, registered in world table +table through add
static BYTE* Mo_Construct(void (*base)(), int table, void (*add)(), BYTE* s, BYTE* w, BYTE* a2, int a3,
                          const BYTE* m, const BYTE* m5, int a6, int a7, int a8, int a9)
{
    Mo_M48 mv = *(const Mo_M48*)m;
    int id = CallC<int>(FUN_57ca30);
    void* hit = CallC<void*>(FUN_4e5ba7, a2, m5 + 36, m + 36);
    CallThis(base, s, w, hit, a3, id, a6, a7, mv, a8, a9);
    CallThis(FUN_57cb50, s + 376, (void*)g_mo6f653c);
    Mo_Zero(s, 404, 496);
    for (int off = 500; off <= 740; off += 48)
        Mo_Matrix(s, off);
    Mo_Zero(s, 788, 796);
    Mo_Matrix(s, 816);
    Mo_Zero(s, 864, 884);
    CallThis(FUN_5ef5c0, s + 912);
    Mo_Zero(s, 1120, 1128);
    Mo_Zero(s, 1164, 1184);
    CallThis(FUN_5ef5c0, s + 1464);
    for (int off = 1696; off <= 1936; off += 48)
        CallThis(FUN_4bd4e0, s + off);
    for (int off = 1984; off <= 2020; off += 12)
        CallThis(FUN_4bd4d0, s + off);
    CallStd(FUN_4011e0, s + 2040, 12, 5, (void*)FUN_4bd4d0);
    CallStd(FUN_4011e0, s + 2104, 208, 5, (void*)FUN_5ef5c0);
    Mo_Zero(s, 3148, 3156);
    CallThis(FUN_4bd4d0, s + 3160);
    CallThis(FUN_4bd4e0, s + 3172);
    CallThis(FUN_4bd4e0, s + 3220);
    CallThis(FUN_57cfc0, s + 3268);
    F(void*, s, 0) = g_mo650544;
    F(void*, s, 256) = g_mo6504f8;
    F(void*, s, 376) = g_mo6504f4;
    CallThis(FUN_57d860, s, a6, m, m5, a2);
    Mo_Register(s, w, 416, 24, table, add);
    Mo_Register(s, w, 900, 4, table, add);
    return s;
}

extern "C" {

// [FUN_57d060] constructor (world, a2, a3, matrix m, matrix m5, a6..a9): the scene object base (5c6170) with a
// new id and the object hit along m5 -> m, its matrices, nodes and vectors, vtables 650544, 6504f8 (+256),
// 6504f4 (+376), set up by 57d860, and +416 and +900 registered in the world's table +7976
BYTE* __fastcall MovingObj_Construct(BYTE* s, void*, BYTE* w, BYTE* a2, int a3, const BYTE* m, const BYTE* m5,
                                     int a6, int a7, int a8, int a9)
{
    return Mo_Construct(FUN_5c6170, 7976, FUN_5c3070, s, w, a2, a3, m, m5, a6, a7, a8, a9);
}

// [FUN_57cb70] the same over the base 5c5f60, registered in the world's list +8300
BYTE* __fastcall MovingObj_ConstructListed(BYTE* s, void*, BYTE* w, BYTE* a2, int a3, const BYTE* m,
                                           const BYTE* m5, int a6, int a7, int a8, int a9)
{
    return Mo_Construct(FUN_5c5f60, 8300, FUN_5c32f0, s, w, a2, a3, m, m5, a6, a7, a8, a9);
}

} // extern "C"
