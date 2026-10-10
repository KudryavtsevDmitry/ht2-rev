// grid_support.cpp - small functions of grd.cpp of the terrain library (see
// also grid_base.cpp and grid_topology.cpp): ring lists, the binary tree
// nodes of btr.h, the 3x3 tile height tables (257x257 samples of 16 bytes),
// degree tables of sine and cosine, and object constructors.
//
// Building with  nmake ASM_GRID_SUPPORT=1  makes king.masm assemble the
// original procedures again.

#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef float Floats[1];

extern "C" void FUN_606750(); KING_ALIAS("_FUN_606750", "@GridEdge_Destruct@8")
ASM_PROC(FUN_60aad0)    // error report (code, format, file, line, date)
ASM_PROC(FUN_50d190)
extern "C" void FUN_613790(); KING_ALIAS("_FUN_613790", "@MarineQueue_TopWorkItem@8")
ASM_PROC(FUN_609de0)

ASM_VAR(AsmLabel, g_vtbl652ef4, $L_652ef4)
ASM_VAR(AsmLabel, g_vtbl652f00, $L_652f00)
ASM_VAR(AsmLabel, s_buildDate, $L_6724f8)
ASM_VAR(AsmLabel, s_btrFile, $L_684a64)
ASM_VAR(AsmLabel, s_fileLine, $L_6724b0)
ASM_VAR(Floats, g_sinDeg, $L_724964)      // 360 entries
ASM_VAR(Floats, g_cosDeg, $L_7243bc)      // 360 entries
ASM_VAR(int, g_725d28, $L_725d28)

extern "C" {

// [FUN_605810]
int __fastcall Stack_Top(BYTE* self, void*)
{
    return F(int, self, 16 + F(int, self, 12) * 4);
}

// [FUN_606730]
BYTE* __fastcall Grid606750_Delete(BYTE* self, void*, int flags)
{
    CallThis(FUN_606750, self);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [FUN_606a50] the node after node in a two-ended list
int __fastcall Pair_Next(BYTE** self, void*, int* node)
{
    if (self[0] && (BYTE*)node == F(BYTE*, self[0], 4))
        return (int)self[1];
    if (self[1] && (BYTE*)node == F(BYTE*, self[1], 4))
        return 0;
    return *node;
}

// [FUN_607f60]
DWORD* __fastcall Quad_Zero(DWORD* self, void*)
{
    self[2] = 0;
    self[1] = 0;
    self[0] = 0;
    self[3] = 0;
    return self;
}

// [FUN_607f70]
int __fastcall Stack_Last(int* self, void*)
{
    int n = self[0];
    if (!n)
        return 0;
    return self[n + 1];
}

// [FUN_607f80]
BYTE* __fastcall Grid_ConstructLists(BYTE* self, void*)
{
    F(BYTE*, self, 4) = self;
    F(BYTE*, self, 0) = self;
    static const int z[] = {56, 52, 48, 20, 24, 28, 32, 44, 36, 40, 100, 96, 92, 64, 68, 72, 76, 88, 80, 84,
                            148, 144, 140, 112, 116, 120, 124, 136, 128, 132};
    for (int k = 0; k < sizeof(z) / sizeof(z[0]); k++)
        F(int, self, z[k]) = 0;
    F(void*, self, 16) = g_vtbl652ef4;
    F(void*, self, 60) = g_vtbl652ef4;
    F(void*, self, 108) = g_vtbl652ef4;
    return self;
}

// [FUN_608a50]
BYTE* __fastcall GridObj_Construct(BYTE* self, void*)
{
    static const int z[] = {40, 36, 32, 4, 8, 12, 16, 28, 20, 24};
    for (int k = 0; k < sizeof(z) / sizeof(z[0]); k++)
        F(int, self, z[k]) = 0;
    F(void*, self, 0) = g_vtbl652ef4;
    return self;
}

// [FUN_608aa0]
void __fastcall GridObj_SetVtables(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl652f00;
    F(void*, self, 12) = g_vtbl652ef4;
}

// [FUN_608ab0] the table of the tile holding (x, y)
BYTE* __fastcall Tiles_Table(BYTE** self, void*, DWORD x, DWORD y)
{
    DWORD tx = x / 257, ty = y / 257;
    if (tx == 3 || ty == 3)
        return self[1];
    if (2 - tx != ty)
        return self[2];
    if (tx == 0)
        return ty == 2 && 771 - y <= 257 - x ? self[1] : self[0];
    if (tx == 2 && ty == 0)
        return 771 - x <= 257 - y ? self[1] : self[0];
    return self[0];
}

// [FUN_608e70]
float __fastcall Tile_Height(BYTE* self, void*, DWORD x, DWORD y)
{
    return *(float*)(self + ((x % 257) * 257 + y % 257) * 16);
}

// [FUN_608ea0]
void __fastcall Tile_Normal(BYTE* self, void*, DWORD x, DWORD y, DWORD* out)
{
    const DWORD* s = (DWORD*)(self + ((x % 257) * 257 + y % 257) * 16 + 4);
    out[0] = s[0];
    out[1] = s[1];
    out[2] = s[2];
}

// [FUN_609980]
void __fastcall Grid_ResetVtables(BYTE* self, void*)
{
    F(void*, self, 108) = g_vtbl652ef4;
    F(void*, self, 60) = g_vtbl652ef4;
    F(void*, self, 16) = g_vtbl652ef4;
}

// [FUN_609960]
BYTE* __fastcall Grid_Delete(BYTE* self, void*, int flags)
{
    Grid_ResetVtables(self, 0);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [FUN_609ce0] inserts node before list in a ring
void __cdecl Ring_InsertBefore(BYTE** node, BYTE** list)
{
    node[0] = (BYTE*)list;
    node[1] = (BYTE*)list[1];
    *(BYTE**)list[1] = (BYTE*)node;
    list[1] = (BYTE*)node;
}

static void Btr_Detach(BYTE* self)
{
    BYTE* p = F(BYTE*, self, 8);
    if (!p)
        return;
    if (F(BYTE*, p, 0) == self)
        F(BYTE*, p, 0) = 0;
    else if (F(BYTE*, p, 4) == self)
        F(BYTE*, p, 4) = 0;
    else
        CallC(FUN_60aad0, 0x88760112, (const char*)s_fileLine, (const char*)s_btrFile, 35,
              (const char*)s_buildDate);
}

// [FUN_60a030] destroys a tree node and its subtrees
void __fastcall Btr_Destroy(BYTE* self, void*)
{
    Btr_Detach(self);
    BYTE* c = F(BYTE*, self, 0);
    if (c) {
        Btr_Destroy(c, 0);
        operator delete(c);
    }
    c = F(BYTE*, self, 4);
    if (c) {
        Btr_Destroy(c, 0);
        operator delete(c);
    }
}

// [FUN_60a440]
void __cdecl Ring_Unlink(BYTE** node)
{
    F(BYTE*, node[0], 4) = node[1];
    *(BYTE**)node[1] = node[0];
}

// [FUN_60a850]
int __cdecl Grid_IsFine(BYTE* o)
{
    return F(DWORD, o, 36) >= 10;
}

// [FUN_60ba20]
void __cdecl Tables_InitSin()
{
    for (int i = 0; i < 360; i++)
        g_sinDeg[i] = (float)x87_sin((double)i * 3.1415998935699463f * 0.0055555556900799274f);
}

// [FUN_60bab0]
void __cdecl Tables_InitCos()
{
    for (int i = 0; i < 360; i++)
        g_cosDeg[i] = (float)x87_cos((double)i * 3.1415998935699463f * 0.0055555556900799274f);
}

// [FUN_60bda0]
BYTE* __fastcall Btr_Delete(BYTE* self, void*, int flags)
{
    Btr_Detach(self);
    if (F(BYTE*, self, 0))
        Btr_Delete(F(BYTE*, self, 0), 0, 1);
    if (F(BYTE*, self, 4))
        Btr_Delete(F(BYTE*, self, 4), 0, 1);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [FUN_60c280]
void __cdecl Grid_Reset725d28()
{
    g_725d28 = 0;
}

// [FUN_60c5b0]
void __fastcall GridTask_Free(BYTE* self, void*)
{
    if (F(void*, self, 12)) {
        operator delete(F(void*, self, 12));
        F(void*, self, 12) = 0;
    }
    BYTE* o = F(BYTE*, self, 16);
    if (o) {
        CallThis(FUN_50d190, o);
        operator delete(o);
        F(void*, self, 16) = 0;
    }
}

// [FUN_60cae0]
void __fastcall Grid_DrainQueue(BYTE* self, void*)
{
    BYTE* n;
    while ((n = CallThis<BYTE*>(FUN_613790, F(BYTE*, self, 32))) != 0)
        CallC(FUN_609de0, n, self);
}

}
