// terrain_grid.cpp - small functions of tgreed.cpp of the terrain library:
// the terrain mesh's coordinate nodes, height samples of the 3x3 tiles,
// and accessors.
//
// Building with  nmake ASM_TERRAIN_GRID=1  makes king.masm assemble the
// original procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" {

// [FUN_61b8a0]
WORD* __fastcall TNode_Init(BYTE* self, void*, const DWORD* p, WORD v)
{
    WORD* a = (WORD*)operator new(2);
    if (a)
        *a = v;
    F(WORD*, self, 0) = a;
    F(DWORD, self, 8) = p[0];
    F(DWORD, self, 12) = p[1];
    F(int, self, 4) = 0;
    return a;
}

// [FUN_61bc90] the height at (x, y) of the tiles at +40192
float __fastcall TGrid_Height(BYTE* self, void*, const DWORD* xy)
{
    DWORD x = xy[0], y = xy[1];
    BYTE** t = F(BYTE**, self, 40192);
    DWORD tx = x / 257, ty = y / 257;
    BYTE* table;
    if (tx == 3 || ty == 3)
        table = t[1];
    else if (2 - tx != ty)
        table = t[2];
    else if (tx == 0)
        table = ty == 2 && 771 - y <= 257 - x ? t[1] : t[0];
    else if (tx == 2 && ty == 0)
        table = 771 - x <= 257 - y ? t[1] : t[0];
    else
        table = t[0];
    return *(float*)(table + ((x % 257) * 257 + y % 257) * 16);
}

// [FUN_61c140]
float __fastcall TGrid_Get16(BYTE* self, void*)
{
    return F(float, self, 16);
}

// [FUN_61c150]
float __fastcall TGrid_Get20(BYTE* self, void*)
{
    return F(float, self, 20);
}

// [FUN_61c170]
int __fastcall TGrid_Get36(BYTE* self, void*)
{
    return F(int, self, 36);
}

// [FUN_61c200]
int __fastcall TGrid_Set(BYTE* self, void*, int a, int b, int c, BYTE* d)
{
    F(int, self, 4) = a;
    F(int, self, 12) = c;
    F(int, self, 8) = b;
    F(BYTE*, self, 28) = d;
    if (d)
        F(int, self, 24) = F(int, d, 16);
    return 1;
}

}

ASM_PROC(FUN_50d190)    // a 16-byte record's destructor
ASM_VAR(char, g_vtbl65338c, $L_65338c)
ASM_VAR(char, g_vtbl6533a8, $L_6533a8)

extern "C" {

// [FUN_61b920] destructor: frees the three 66049-record tables at +40192
void __fastcall TGrid61b920_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vtbl65338c;
    F(int, self, 104) = 0;
    F(int, self, 100) = 0;
    BYTE** d = F(BYTE**, self, 40192);
    if (!d)
        goto done;
    for (int k = 0; k < 3; k++) {
        BYTE* p = d[k];
        if (p) {
            for (int i = 66049 - 1; i >= 0; i--)
                CallThis(FUN_50d190, p + i * 16);
            operator delete(p);
            d[k] = 0;
        }
    }
    operator delete(d);
    F(BYTE**, self, 40192) = 0;
done:
    F(void*, self, 0) = &g_vtbl6533a8;
}

}
