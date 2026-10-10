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
