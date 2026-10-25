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

extern "C" void FUN_50d190(); KING_ALIAS("_FUN_50d190", "@Shell_Nop@8")    // a 16-byte record's destructor
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

#include <string.h>
// [FUN_61c250] the midpoint of two grid nodes' positions and normals, with b's remaining four values
extern "C" DWORD* __cdecl GridNode_Midpoint(DWORD* out, const float* a, const float* b)
{
    double s3 = (double)a[3] + b[3];
    float f4 = (float)((double)a[4] + b[4]);
    float f5 = (float)((double)a[5] + b[5]);
    float r[10];
    r[3] = (float)(s3 * 0.5f);
    r[4] = (float)((double)f4 * 0.5f);
    r[5] = (float)((double)f5 * 0.5f);
    double s0 = (double)a[0] + b[0];
    float f1 = (float)((double)a[1] + b[1]);
    float f2 = (float)((double)a[2] + b[2]);
    r[0] = (float)(s0 * 0.5f);
    r[1] = (float)((double)f1 * 0.5f);
    r[2] = (float)((double)f2 * 0.5f);
    r[6] = b[6]; r[7] = b[7]; r[8] = b[8]; r[9] = b[9];
    memcpy(out, r, 40);
    return out;
}

#include "x87.h"
extern "C" void FUN_6072b0(); KING_ALIAS("_FUN_6072b0", "_Grid_DeepestLevel")    // the deepest level allowed

// [FUN_61ba10] the terrain grid's refinement priority of a node: 0 when hidden or too deep, else by distance and the view wedges
extern "C" double __fastcall TGridCamera_NodePriority(BYTE* s, void*, BYTE* node)
{
    if (F(unsigned, node, 24) >= CallThis<unsigned>(FUN_6072b0, s))
        return 0.0f;
    float px = F(float, node, 32), py = F(float, node, 36), pz = F(float, node, 40);
    double dy = (double)py - F(float, s, 8);
    double dx = (double)px - F(float, s, 4);
    double dd = x87_hypot(dx, dy) - pz;
    float d = (float)dd;
    if (!(dd > 0.0f)) return 1000.0f;
    if (d > 1500.0f) return 0.0f;
    if (!(d >= 50.0f)) return 1000.0f;
    double ex = (double)px - F(float, s, 4);
    float exf = (float)ex;
    double ey = (double)py - F(float, s, 8);
    float eyf = (float)ey;
    float mz = -pz;
    if (!(ey * F(float, s, 60) - ex * F(float, s, 64) >= mz)) return 0.0f;
    if (!(ey * F(float, s, 68) - ex * F(float, s, 72) >= mz)) return 0.0f;
    if (F(unsigned, node, 24) < 8) return 10.0f / ((double)d + 1.0f);
    if (!(d >= 500.0f)) return (double)F(float, node, 20) / d;
    if ((double)eyf * F(float, s, 76) - (double)exf * F(float, s, 80) >= mz
        && (double)eyf * F(float, s, 84) - (double)exf * F(float, s, 88) >= mz)
        return (double)F(float, node, 20) / d;
    return (double)F(float, node, 20) * 0.5f / d;
}

struct Vec3G { float x, y, z; };
extern "C" void FUN_608ea0(); KING_ALIAS("_FUN_608ea0", "@Tile_Normal@20")
extern "C" void FUN_608e70(); KING_ALIAS("_FUN_608e70", "@Tile_Height@16")
ASM_VAR(Vec3G, g_tileNormal724378, $L_724378)

// the height table of the tile a grid point lies in (as in TGrid_Height)
static BYTE* TGrid_TableAt(BYTE* self, DWORD x, DWORD y)
{
    BYTE** t = F(BYTE**, self, 40192);
    DWORD tx = x / 257, ty = y / 257;
    if (tx == 3 || ty == 3)
        return t[1];
    if (2 - tx != ty)
        return t[2];
    if (tx == 0)
        return ty == 2 && 771 - y <= 257 - x ? t[1] : t[0];
    if (tx == 2 && ty == 0)
        return 771 - x <= 257 - y ? t[1] : t[0];
    return t[0];
}

// [FUN_608c70] the node p of grid point xy {x, y}: world x/y, height, normal, the scaled offsets
extern "C" float* __fastcall TGrid_MakeNode(BYTE* self, void*, float* p, const DWORD* xy)
{
    DWORD X = xy[0], Y = xy[1];
    double fx = (double)(__int64)X;
    float xf = (float)fx;
    double x1 = fx - F(float, self, 40184);
    float a = (float)(x1 * 0.125f);
    double fy = (double)(__int64)Y;
    float yf = (float)fy;
    double y1 = fy - F(float, self, 40188);
    float y1f = (float)y1;
    float b = (float)(y1 * 0.125f);
    float c = (float)(x1 * 0.3333333432674408f);
    float d = (float)((double)y1f * 0.3333333432674408f);
    CallThis(FUN_608ea0, TGrid_TableAt(self, X, Y), X, Y, &g_tileNormal724378);
    double h = CallThis<double>(FUN_608e70, TGrid_TableAt(self, X, Y), X, Y);
    p[0] = (float)((double)xf * F(float, self, 40152) + F(float, self, 40136));
    p[1] = (float)((double)yf * F(float, self, 40156) + F(float, self, 40148));
    p[2] = (float)h;
    *(Vec3G*)(p + 3) = g_tileNormal724378;
    p[6] = a;
    p[7] = b;
    p[8] = c;
    p[9] = d;
    return p;
}

extern "C" void __fastcall GridBase_SetCameraAndOrigin(BYTE* self, void*, const DWORD* position, const float* direction, const DWORD* origin);
ASM_VAR(char, g_tgrid72c108b, $L_72c108)

// the four view edge normals at +offset for a view half-angle
static void TGrid_ViewEdges(BYTE* s, int offset, float angle, float nx, float ny)
{
    float c = (float)x87_cos(angle);
    double sn = x87_sin(angle);
    float sny = (float)(sn * ny);
    double cnx = (double)c * nx;
    float cnxf = (float)cnx;
    F(float, s, offset) = (float)(cnx + sny);
    float cny = (float)((double)c * ny);
    double snx = sn * nx;
    F(float, s, offset + 4) = (float)((double)cny - snx);
    F(float, s, offset + 8) = (float)-((double)cnxf - sny);
    F(float, s, offset + 12) = (float)-(snx + cny);
}

// [FUN_614ce0] moves the terrain grid's centre with the camera in whole steps (+52/+56) and updates the view edges
extern "C" void __fastcall TGrid_FollowCamera(BYTE* s, void*, const float* pos, const float* dir, const DWORD* origin)
{
    GridBase_SetCameraAndOrigin(s, 0, (const DWORD*)pos, dir, origin);
    double gx = ((double)pos[0] - F(float, s, 40136)) / F(float, s, 40152);
    float gy = (float)(((double)pos[1] - F(float, s, 40148)) / F(float, s, 40156));
    double ax = gx - F(float, s, 40184);
    if (ax < 0) ax = -ax;
    if (ax >= F(float, s, 52)) {
        double two = (double)F(float, s, 52) + F(float, s, 52);
        if (!(ax < two))
            F(float, s, 40184) = (float)gx;
        else if (gx > F(float, s, 40184))
            F(float, s, 40184) = (float)((double)F(float, s, 40184) + F(float, s, 52));
        else
            F(float, s, 40184) = (float)((double)F(float, s, 40184) - F(float, s, 52));
        F(int, s, 40116) = 1;
    }
    double ay = (double)gy - F(float, s, 40188);
    if (ay < 0) ay = -ay;
    if (ay >= F(float, s, 56)) {
        double two = (double)F(float, s, 56) + F(float, s, 56);
        if (!(ay < two))
            F(float, s, 40188) = gy;
        else if (gy > F(float, s, 40188))
            F(float, s, 40188) = (float)((double)F(float, s, 40188) + F(float, s, 56));
        else
            F(float, s, 40188) = (float)((double)F(float, s, 40188) - F(float, s, 56));
        F(int, s, 40116) = 1;
    }
    float fx = F(float, s, 16), fy = F(float, s, 20);
    float len = (float)x87_sqrt((double)fy * fy + (double)fx * fx);
    float nx = (float)((double)fx / len);
    float ny = (float)((double)fy / len);
    TGrid_ViewEdges(s, 60, TGrid_Get16((BYTE*)&g_tgrid72c108b, 0), nx, ny);
    TGrid_ViewEdges(s, 76, TGrid_Get20((BYTE*)&g_tgrid72c108b, 0), nx, ny);
}

// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_61b920(); KING_ALIAS("_FUN_61b920", "@TGrid61b920_Destroy@8")
extern "C" void FUN_61c170(); KING_ALIAS("_FUN_61c170", "@TGrid_Get36@8")

static BYTE* Tgr_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_61b900]
extern "C" BYTE* __fastcall Tgr61b900_DeletingDestructor(BYTE* s, void*, int flags) { return Tgr_DeleteAfter(s, flags, FUN_61b920); }

// static initializers of constants
ASM_VAR(DWORD, g_tgr72c0f0, $L_72c0f0)
ASM_VAR(DWORD, g_tgr72c0f4, $L_72c0f4)
ASM_VAR(DWORD, g_tgr72c0f8, $L_72c0f8)
ASM_VAR(float, g_tgr64c258, $L_64c258)
ASM_VAR(float, g_tgr64d4e8, $L_64d4e8)
ASM_VAR(float, g_tgr6533c4, $L_6533c4)
ASM_VAR(float, g_tgr6533c8, $L_6533c8)
ASM_VAR(float, g_tgr6533cc, $L_6533cc)
ASM_VAR(float, g_tgr6533d8, $L_6533d8)
ASM_VAR(float, g_tgr6533e8, $L_6533e8)
ASM_VAR(float, g_tgr6533ec, $L_6533ec)
ASM_VAR(float, g_tgr6533f0, $L_6533f0)
ASM_VAR(float, g_tgr72c0e0, $L_72c0e0)
ASM_VAR(float, g_tgr72c0e8, $L_72c0e8)
ASM_VAR(float, g_tgr72c0ec, $L_72c0ec)
ASM_VAR(float, g_tgr72c130, $L_72c130)
ASM_VAR(float, g_tgr72c138, $L_72c138)
ASM_VAR(float, g_tgr72c140, $L_72c140)
ASM_VAR(float, g_tgr72c148, $L_72c148)
// [$L_61bff0]
extern "C" void __cdecl TgrInit_61bff0() { g_tgr72c0e0 = (float)((double)g_tgr6533c4 + (double)g_tgr6533c4); }
// [$L_61c010]
extern "C" void __cdecl TgrInit_61c010() { g_tgr72c0e8 = (float)((double)g_tgr6533c8 + (double)g_tgr6533c8); }
// [$L_61c060]
extern "C" void __cdecl TgrInit_61c060() { g_tgr72c0f0 = 0x0; g_tgr72c0f4 = 0x0; g_tgr72c0f8 = 0x0; }
// [$L_61c090]
extern "C" void __cdecl TgrInit_61c090() { g_tgr72c130 = (float)((double)g_tgr6533d8 + (double)g_tgr6533d8); }
// [$L_61c190]
extern "C" void __cdecl TgrInit_61c190() { g_tgr72c138 = (float)((double)g_tgr6533e8 + (double)g_tgr6533e8); }
// [$L_61c1e0]
extern "C" void __cdecl TgrInit_61c1e0() { g_tgr72c140 = (float)((double)g_tgr6533ec + (double)g_tgr6533ec); }
// [$L_61c230]
extern "C" void __cdecl TgrInit_61c230() { g_tgr72c148 = (float)((double)g_tgr6533f0 + (double)g_tgr6533f0); }
// [$L_61c030]
extern "C" void __cdecl TgrInit_61c030() { g_tgr72c0ec = (float)((double)g_tgr6533cc - g_tgr64d4e8 - g_tgr64c258); }
typedef char AsmLabel[1];
ASM_VAR(AsmLabel, g_tgr72c108, $L_72c108)
ASM_VAR(AsmLabel, g_tgr6533a8, $L_6533a8)

// [$L_61bfd0]
extern "C" int __cdecl Tgr_61bfd0() { return CallThis<int>(FUN_61c170, g_tgr72c108); }
// [$L_61bfe0]
extern "C" void __fastcall Tgr_Set92(BYTE* s, void*, int v) { F(int, s, 92) = v; }
// [$L_61bbc0]
extern "C" void __fastcall Tgr_Clear96(BYTE* s)
{
    F(int, s, 96) = 0;
    F(int, s, 100) = 0;
    F(int, s, 104) = 0;
}
// [$L_61b8e0] the base destructor (vtable 6533a8) inlined
extern "C" BYTE* __fastcall Tgr61b8e0_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_tgr6533a8;
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

ASM_VAR(float, g_tgr72c0cc, $L_72c0cc)

// [$L_61bbd0] a triangle (a, b, c vertices of +92, 40 bytes each) to the list below the water level
// 72c0cc (+108, count +100) or to the other list (+20106, count +104)
extern "C" void __fastcall Tgr_AddTriangle(BYTE* s, void*, unsigned a, unsigned b, unsigned c, int)
{
    BYTE* v = F(BYTE*, s, 92);
    bool below = !(F(float, v, (a & 0xffff) * 40 + 8) >= g_tgr72c0cc)
              && !(F(float, v, (b & 0xffff) * 40 + 8) >= g_tgr72c0cc)
              && !(F(float, v, (c & 0xffff) * 40 + 8) >= g_tgr72c0cc);
    int cnt = below ? 100 : 104;
    int base = below ? 108 : 20106;
    int n = F(int, s, cnt);
    F(WORD, s, base + n * 2) = (WORD)a;
    F(WORD, s, base + F(int, s, cnt) * 2 + 2) = (WORD)c;
    F(WORD, s, base + F(int, s, cnt) * 2 + 4) = (WORD)b;
    F(int, s, cnt) += 3;
}

// ---------------------------------------------------------------------------
// More virtual methods.

extern "C" void FUN_5b1020(); KING_ALIAS("_FUN_5b1020", "@Material_Select@24")
extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")

// a method of the Direct3D device d (vtable slot at byte offset off)
template <class... A>
static int Tgr_Dev(void* d, unsigned off, A... args)
{
    void* fn = *(void**)(*(char**)d + off);
    return ((int(__stdcall*)(void*, A...))fn)(d, args...);
}

// [$L_61bf20] draws the two triangle lists (+108 count +100, +20106 count +104) with their textures
extern "C" void __fastcall Tgr_Draw(BYTE* s, void*, void* dev, BYTE* b)
{
    Tgr_Dev(dev, 80, 136, 1);
    CallThis(FUN_5b1020, b, F(int, s, 40104), 1, 0, 0);
    if (F(int, s, 100)) {
        int r = Tgr_Dev(dev, 104, 4, 530, F(int, s, 92), F(int, s, 96), s + 108, F(int, s, 100), 0);
        if (r < 0)
            CallC(FUN_5d4c10, r, 0);
    }
    CallThis(FUN_5b1020, b, F(int, s, 40112), 1, 0, 0);
    if (F(int, s, 104)) {
        int r = Tgr_Dev(dev, 104, 4, 530, F(int, s, 92), F(int, s, 96), s + 20106, F(int, s, 104), 0);
        if (r < 0)
            CallC(FUN_5d4c10, r, 0);
    }
}

// ---------------------------------------------------------------------------
// Grid setup (exception frame dropped).

extern "C" void FUN_613b50(); KING_ALIAS("_FUN_613b50", "@Terrain_ReadTiles@16")
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")
extern "C" void Tgr_VecCtor(); KING_ALIAS("_Tgr_VecCtor", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" BYTE* __fastcall Cell61bee0_Construct(BYTE* s);
ASM_VAR(AsmLabel, s_q27_6724f8, $L_6724f8)
ASM_VAR(AsmLabel, s_q27_685dac, $L_685dac)
ASM_VAR(AsmLabel, s_q27_6724b0, $L_6724b0)

static BYTE* Tgr_NewCells()
{
    BYTE* p = (BYTE*)operator new(1056784);
    if (p)
        CallStd(Tgr_VecCtor, p, 16, 66049, (BYTE*)Cell61bee0_Construct, FUN_50d190);
    return p;
}
// [$L_61bd40] (a, b): three planes of 257x257 cells (+40192) and the grid's table loaded; 0 and an error report
// if that fails
extern "C" int __fastcall Tgr_Setup(BYTE* s, void*, int a, int b)
{
    F(int, s, 44) = a;
    F(int, s, 48) = b;
    F(DWORD, s, 40184) = 1189765120;
    F(DWORD, s, 40188) = 1189765120;
    BYTE** t = (BYTE**)operator new(12);
    if (t) {
        t[0] = Tgr_NewCells();
        t[1] = Tgr_NewCells();
        t[2] = Tgr_NewCells();
    }
    F(BYTE**, s, 40192) = t;
    if (t && CallThis<int>(FUN_613b50, t, F(int, s, 40196), F(int, s, 40200)))
        return 1;
    CallC(FUN_60aad0, 2289444628u, (const char*)s_q27_6724b0, (const char*)s_q27_685dac, 175, (const char*)s_q27_6724f8);
    return 0;
}

// ---------------------------------------------------------------------------
// Destructor thunks: the vtable, then the base destructor; static object cleanups.

ASM_VAR(AsmLabel, g_q46_72c108, $L_72c108)
ASM_VAR(AsmLabel, g_q46_61c180, $L_61c180)
extern "C" int __fastcall Grid_Get32(BYTE* s);
extern "C" BYTE* __fastcall Grid61c0f0_Construct(BYTE* s);
// [FUN_6072b0] the deepest level allowed (+24 of the static grid 72c108)
extern "C" int __cdecl Grid_DeepestLevel(void)
{
    return CallThis<int>((void (*)())(void*)g_q46_61c180, (void*)g_q46_72c108);
}
// [FUN_6072c0] the +32 value of the static grid 72c108
extern "C" int __cdecl Grid_Level32(void)
{
    return Grid_Get32((BYTE*)g_q46_72c108);
}
// [FUN_61c0c0] constructs the static grid 72c108
extern "C" int __cdecl Grid72c108_Construct(void)
{
    return (int)Grid61c0f0_Construct((BYTE*)g_q46_72c108);
}

// [$L_61bee0] constructor: cleared
extern "C" BYTE* __fastcall Cell61bee0_Construct(BYTE* s)
{
    memset(s, 0, 16);
    return s;
}

// [$L_61c160] +32 of s
extern "C" int __fastcall Grid_Get32(BYTE* s) { return F(int, s, 32); }

ASM_VAR(double, g_q159_64f788, $L_64f788)
ASM_VAR(double, g_q159_6533e0, $L_6533e0)
// [$L_61c0f0] constructor: the grid defaults (cell size 8, the cosines of the two limit angles, 12 and 3000)
extern "C" BYTE* __fastcall Grid61c0f0_Construct(BYTE* s)
{
    float c = (float)x87_cos(g_q159_64f788);
    F(int, s, 24) = 12;
    F(int, s, 28) = 12;
    F(DWORD, s, 0) = 1090519040;
    F(DWORD, s, 4) = 1090519040;
    F(DWORD, s, 16) = 1065749138;
    F(DWORD, s, 20) = 1061752795;
    F(int, s, 32) = 3000;
    F(int, s, 36) = 3000;
    F(float, s, 8) = c;
    F(float, s, 12) = (float)x87_cos(g_q159_6533e0);
    return s;
}

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
// [$L_61c0e0] the static grid 72c108's (empty) teardown
extern "C" void __cdecl Grid72c108_Destroy(void) { CallThis(FUN_5e48f0, (void*)&g_tgrid72c108b); }
// [$L_61c0d0] registers it at exit
extern "C" int __cdecl Grid72c108_AtExit(void) { return atexit(Grid72c108_Destroy); }
// [$L_61c0b0] the static initializer of 72c108
extern "C" int __cdecl Grid72c108_Init(void)
{
    Grid72c108_Construct();
    return Grid72c108_AtExit();
}
// [$L_61ba00] sets the base vtable 6533a8
extern "C" void __fastcall TGridBase_SetVtbl(BYTE* s) { F(void*, s, 0) = g_tgr6533a8; }

// === gzip's inflate (FUN_62ee00 .. FUN_630020, inflate.c of gzip 1.2.4)

#include <stdlib.h>

ASM_PROC(FUN_62b8e0)                                // fill_inbuf(eof_ok)
ASM_PROC(FUN_62b9e0)                                // flush_window()
ASM_VAR(unsigned, g_gzInptr, $L_736684)
ASM_VAR(unsigned, g_gzInsize, $L_736680)
ASM_VAR(BYTE, g_gzInbuf, $L_72e634)
ASM_VAR(BYTE, g_gzSlide, $L_75ae90)                 // the 32 KB window
ASM_VAR(unsigned, g_gzOutcnt, $L_736688)            // also the window position wp
ASM_VAR(DWORD, g_gzBb, $L_77e3f4)                   // the bit buffer
ASM_VAR(unsigned, g_gzBk, $L_77e3f8)                // and its bit count
ASM_VAR(unsigned, g_gzHufts, $L_77e3fc)
ASM_VAR(WORD, g_gzMaskBits, $L_68a6f4)
ASM_VAR(unsigned, g_gzBorder, $L_68a5b0)            // the bit length code order
ASM_VAR(WORD, g_gzCplens, $L_68a5fc)
ASM_VAR(WORD, g_gzCplext, $L_68a63c)
ASM_VAR(WORD, g_gzCpdist, $L_68a67c)
ASM_VAR(WORD, g_gzCpdext, $L_68a6b8)
ASM_VAR(int, g_gzLbits, $L_68a718)
ASM_VAR(int, g_gzDbits, $L_68a71c)
ASM_VAR(AsmLabel, g_gzStderr, $L_686380)
ASM_VAR(char, g_gz68a73c, $L_68a73c)                // " incomplete literal tree\n"
ASM_VAR(char, g_gz68a720, $L_68a720)                // " incomplete distance tree\n"
extern "C" int __cdecl Gz_fprintf(void* f, const char* fmt, ...); KING_ALIAS("_Gz_fprintf", "_fprintf")

struct GzHuft
{
    BYTE e;                                         // extra bits or operation
    BYTE b;                                         // bits in this code or subcode
    WORD pad;
    union
    {
        WORD n;                                     // literal, length base or distance base
        GzHuft* t;                                  // the next table
    } v;
};

#define GZ_WSIZE 0x8000
#define GZ_NEXTBYTE() (g_gzInptr < g_gzInsize ? (unsigned)(&g_gzInbuf)[g_gzInptr++] : (unsigned)(BYTE)CallC<int>(FUN_62b8e0, 0))
#define GZ_NEEDBITS(n)                       \
    while (k < (unsigned)(n))                \
    {                                        \
        b |= (DWORD)GZ_NEXTBYTE() << k;      \
        k += 8;                              \
    }
#define GZ_DUMPBITS(n) \
    {                  \
        b >>= (n);     \
        k -= (n);      \
    }
#define GZ_MASK(n) ((&g_gzMaskBits)[n])

static void Gz_FlushOutput(unsigned w)
{
    g_gzOutcnt = w;
    CallC(FUN_62b9e0);
}

extern "C" {

// [FUN_62f2a0] huft_free: the tables linked from t (each block's link sits before it); 0
int __cdecl Gz_HuftFree(GzHuft* t)
{
    GzHuft* p = t;
    while (p)
    {
        GzHuft* q = (--p)->v.t;
        free(p);
        p = q;
    }
    return 0;
}

// [FUN_62ee00] huft_build: decoding tables for the n code lengths b (values below s are literals, the rest
// bases d with extra bits e) into *t, lookup bits in *m; 0, 1 incomplete, 2 bad lengths, 3 out of memory
int __cdecl Gz_HuftBuild(unsigned* b, unsigned n, unsigned s, WORD* d, WORD* e, GzHuft** t, int* m)
{
    unsigned a, c[17], f, i, j, *p, v[288], x[17], *xp, z;
    int g, h, k, l, w, y;
    GzHuft *q, r, *u[16];
    memset(c, 0, sizeof c);
    p = b;
    i = n;
    do
    {
        c[*p]++;
        p++;
    } while (--i);
    if (c[0] == n)
    {
        *t = 0;
        *m = 0;
        return 0;
    }
    l = *m;
    for (j = 1; j <= 16; j++)
        if (c[j])
            break;
    k = j;
    if ((unsigned)l < j)
        l = j;
    for (i = 16; i; i--)
        if (c[i])
            break;
    g = i;
    if ((unsigned)l > i)
        l = i;
    *m = l;
    for (y = 1 << j; j < i; j++, y <<= 1)
        if ((y -= c[j]) < 0)
            return 2;
    if ((y -= c[i]) < 0)
        return 2;
    c[i] += y;
    x[1] = j = 0;
    p = c + 1;
    xp = x + 2;
    while (--i)
        *xp++ = (j += *p++);
    p = b;
    i = 0;
    do
    {
        if ((j = *p++) != 0)
            v[x[j]++] = i;
    } while (++i < n);
    x[0] = i = 0;
    p = v;
    h = -1;
    w = -l;
    u[0] = 0;
    q = 0;
    z = 0;
    for (; k <= g; k++)
    {
        a = c[k];
        while (a--)
        {
            while (k > w + l)
            {
                h++;
                w += l;
                z = (z = g - w) > (unsigned)l ? l : z;
                if ((f = 1 << (j = k - w)) > a + 1)
                {
                    f -= a + 1;
                    xp = c + k;
                    while (++j < z)
                    {
                        if ((f <<= 1) <= *++xp)
                            break;
                        f -= *xp;
                    }
                }
                z = 1 << j;
                if ((q = (GzHuft*)malloc((z + 1) * sizeof(GzHuft))) == 0)
                {
                    if (h)
                        Gz_HuftFree(u[0]);
                    return 3;
                }
                g_gzHufts += z + 1;
                *t = q + 1;
                *(t = &(q->v.t)) = 0;
                u[h] = ++q;
                if (h)
                {
                    x[h] = i;
                    r.b = (BYTE)l;
                    r.e = (BYTE)(16 + j);
                    r.v.t = q;
                    j = i >> (w - l);
                    u[h - 1][j] = r;
                }
            }
            r.b = (BYTE)(k - w);
            if (p >= v + n)
                r.e = 99;
            else if (*p < s)
            {
                r.e = (BYTE)(*p < 256 ? 16 : 15);
                r.v.n = (WORD)*p;
                p++;
            }
            else
            {
                r.e = (BYTE)e[*p - s];
                r.v.n = d[*p++ - s];
            }
            f = 1 << (k - w);
            for (j = i >> w; j < z; j += f)
                q[j] = r;
            for (j = 1 << (k - 1); i & j; j >>= 1)
                i ^= j;
            i ^= j;
            while ((i & ((1 << w) - 1)) != x[h])
            {
                h--;
                w -= l;
            }
        }
    }
    return y != 0 && g != 1;
}

// [FUN_62f2d0] inflate_codes: the literals and matches of a block through the tables tl / td into the window;
// 0 at the end of the block, 1 on a bad code
int __cdecl Gz_InflateCodes(GzHuft* tl, GzHuft* td, int bl, int bd)
{
    unsigned e, n, d, w, ml, md, k;
    GzHuft* t;
    DWORD b;
    b = g_gzBb;
    k = g_gzBk;
    w = g_gzOutcnt;
    ml = GZ_MASK(bl);
    md = GZ_MASK(bd);
    for (;;)
    {
        GZ_NEEDBITS(bl)
        if ((e = (t = tl + ((unsigned)b & ml))->e) > 16)
            do
            {
                if (e == 99)
                    return 1;
                GZ_DUMPBITS(t->b)
                e -= 16;
                GZ_NEEDBITS(e)
            } while ((e = (t = t->v.t + ((unsigned)b & GZ_MASK(e)))->e) > 16);
        GZ_DUMPBITS(t->b)
        if (e == 16)
        {
            (&g_gzSlide)[w++] = (BYTE)t->v.n;
            if (w == GZ_WSIZE)
            {
                Gz_FlushOutput(w);
                w = 0;
            }
        }
        else
        {
            if (e == 15)
                break;
            GZ_NEEDBITS(e)
            n = t->v.n + ((unsigned)b & GZ_MASK(e));
            GZ_DUMPBITS(e)
            GZ_NEEDBITS(bd)
            if ((e = (t = td + ((unsigned)b & md))->e) > 16)
                do
                {
                    if (e == 99)
                        return 1;
                    GZ_DUMPBITS(t->b)
                    e -= 16;
                    GZ_NEEDBITS(e)
                } while ((e = (t = t->v.t + ((unsigned)b & GZ_MASK(e)))->e) > 16);
            GZ_DUMPBITS(t->b)
            GZ_NEEDBITS(e)
            d = w - t->v.n - ((unsigned)b & GZ_MASK(e));
            GZ_DUMPBITS(e)
            do
            {
                n -= (e = (e = GZ_WSIZE - ((d &= GZ_WSIZE - 1) > w ? d : w)) > n ? n : e);
                if (w - d >= e)
                {
                    memcpy(&g_gzSlide + w, &g_gzSlide + d, e);
                    w += e;
                    d += e;
                }
                else
                    do
                    {
                        (&g_gzSlide)[w++] = (&g_gzSlide)[d++];
                    } while (--e);
                if (w == GZ_WSIZE)
                {
                    Gz_FlushOutput(w);
                    w = 0;
                }
            } while (n);
        }
    }
    g_gzOutcnt = w;
    g_gzBb = b;
    g_gzBk = k;
    return 0;
}

// [$L_62f6c0] inflate_stored: a stored block copied to the window; 1 when its length check fails
int __cdecl Gz_InflateStored()
{
    unsigned n, w, k;
    DWORD b;
    b = g_gzBb;
    k = g_gzBk;
    w = g_gzOutcnt;
    n = k & 7;
    GZ_DUMPBITS(n)
    GZ_NEEDBITS(16)
    n = (unsigned)b & 0xffff;
    GZ_DUMPBITS(16)
    GZ_NEEDBITS(16)
    if (n != (unsigned)(~b & 0xffff))
        return 1;
    GZ_DUMPBITS(16)
    while (n--)
    {
        GZ_NEEDBITS(8)
        (&g_gzSlide)[w++] = (BYTE)b;
        if (w == GZ_WSIZE)
        {
            Gz_FlushOutput(w);
            w = 0;
        }
        GZ_DUMPBITS(8)
    }
    g_gzOutcnt = w;
    g_gzBb = b;
    g_gzBk = k;
    return 0;
}

// [$L_62f840] inflate_fixed: a block with the fixed Huffman codes
int __cdecl Gz_InflateFixed()
{
    int i, bl, bd;
    GzHuft *tl, *td;
    unsigned l[288];
    for (i = 0; i < 144; i++)
        l[i] = 8;
    for (; i < 256; i++)
        l[i] = 9;
    for (; i < 280; i++)
        l[i] = 7;
    for (; i < 288; i++)
        l[i] = 8;
    bl = 7;
    if ((i = Gz_HuftBuild(l, 288, 257, &g_gzCplens, &g_gzCplext, &tl, &bl)) != 0)
        return i;
    for (i = 0; i < 30; i++)
        l[i] = 5;
    bd = 5;
    if ((i = Gz_HuftBuild(l, 30, 0, &g_gzCpdist, &g_gzCpdext, &td, &bd)) > 1)
    {
        Gz_HuftFree(tl);
        return i;
    }
    if (Gz_InflateCodes(tl, td, bl, bd))
        return 1;
    Gz_HuftFree(tl);
    Gz_HuftFree(td);
    return 0;
}

// [$L_62f980] inflate_dynamic: a block with its own Huffman codes
int __cdecl Gz_InflateDynamic()
{
    int i, bl, bd;
    unsigned j, l, m, n, nb, nl, nd, k;
    GzHuft *tl, *td;
    unsigned ll[286 + 30];
    DWORD b;
    b = g_gzBb;
    k = g_gzBk;
    GZ_NEEDBITS(5)
    nl = 257 + ((unsigned)b & 0x1f);
    GZ_DUMPBITS(5)
    GZ_NEEDBITS(5)
    nd = 1 + ((unsigned)b & 0x1f);
    GZ_DUMPBITS(5)
    GZ_NEEDBITS(4)
    nb = 4 + ((unsigned)b & 0xf);
    GZ_DUMPBITS(4)
    if (nl > 286 || nd > 30)
        return 1;
    for (j = 0; j < nb; j++)
    {
        GZ_NEEDBITS(3)
        ll[(&g_gzBorder)[j]] = (unsigned)b & 7;
        GZ_DUMPBITS(3)
    }
    for (; j < 19; j++)
        ll[(&g_gzBorder)[j]] = 0;
    bl = 7;
    if ((i = Gz_HuftBuild(ll, 19, 19, 0, 0, &tl, &bl)) != 0)
    {
        if (i == 1)
            Gz_HuftFree(tl);
        return i;
    }
    n = nl + nd;
    m = GZ_MASK(bl);
    i = l = 0;
    while ((unsigned)i < n)
    {
        GZ_NEEDBITS(bl)
        j = (td = tl + ((unsigned)b & m))->b;
        GZ_DUMPBITS(j)
        j = td->v.n;
        if (j < 16)
            ll[i++] = l = j;
        else if (j == 16)
        {
            GZ_NEEDBITS(2)
            j = 3 + ((unsigned)b & 3);
            GZ_DUMPBITS(2)
            if ((unsigned)i + j > n)
                return 1;
            while (j--)
                ll[i++] = l;
        }
        else if (j == 17)
        {
            GZ_NEEDBITS(3)
            j = 3 + ((unsigned)b & 7);
            GZ_DUMPBITS(3)
            if ((unsigned)i + j > n)
                return 1;
            while (j--)
                ll[i++] = 0;
            l = 0;
        }
        else
        {
            GZ_NEEDBITS(7)
            j = 11 + ((unsigned)b & 0x7f);
            GZ_DUMPBITS(7)
            if ((unsigned)i + j > n)
                return 1;
            while (j--)
                ll[i++] = 0;
            l = 0;
        }
    }
    Gz_HuftFree(tl);
    g_gzBb = b;
    g_gzBk = k;
    bl = g_gzLbits;
    if ((i = Gz_HuftBuild(ll, nl, 257, &g_gzCplens, &g_gzCplext, &tl, &bl)) != 0)
    {
        if (i == 1)
        {
            Gz_fprintf(&g_gzStderr, &g_gz68a73c);
            Gz_HuftFree(tl);
        }
        return i;
    }
    bd = g_gzDbits;
    if ((i = Gz_HuftBuild(ll + nl, nd, 0, &g_gzCpdist, &g_gzCpdext, &td, &bd)) != 0)
    {
        if (i == 1)
        {
            Gz_fprintf(&g_gzStderr, &g_gz68a720);
            Gz_HuftFree(td);
        }
        Gz_HuftFree(tl);
        return i;
    }
    if (Gz_InflateCodes(tl, td, bl, bd))
        return 1;
    Gz_HuftFree(tl);
    Gz_HuftFree(td);
    return 0;
}

// [FUN_62ff30] inflate_block: the next block (*e set on the last one) by its kind
int __cdecl Gz_InflateBlock(int* e)
{
    unsigned t, k;
    DWORD b;
    b = g_gzBb;
    k = g_gzBk;
    GZ_NEEDBITS(1)
    *e = (int)b & 1;
    GZ_DUMPBITS(1)
    GZ_NEEDBITS(2)
    t = (unsigned)b & 3;
    GZ_DUMPBITS(2)
    g_gzBb = b;
    g_gzBk = k;
    if (t == 2)
        return Gz_InflateDynamic();
    if (t == 0)
        return Gz_InflateStored();
    if (t == 1)
        return Gz_InflateFixed();
    return 2;
}

// [FUN_630020] inflate: a whole deflated stream, the unused input bytes given back; 0 or an error
int __cdecl Gz_Inflate()
{
    int e, r;
    unsigned h;
    g_gzOutcnt = 0;
    g_gzBk = 0;
    g_gzBb = 0;
    h = 0;
    do
    {
        g_gzHufts = 0;
        if ((r = Gz_InflateBlock(&e)) != 0)
            return r;
        if (g_gzHufts > h)
            h = g_gzHufts;
    } while (!e);
    while (g_gzBk >= 8)
    {
        g_gzBk -= 8;
        g_gzInptr--;
    }
    CallC(FUN_62b9e0);
    return 0;
}

} // extern "C"

// === gzip's trees.c and bits.c, and longest_match / fill_window of deflate.c (gzip 1.2.4)

ASM_PROC(FUN_62b9a0)                                // flush_outbuf()
ASM_VAR(AsmLabel, g_gzFileRead, $L_62b770)          // file_read(buf, size)

struct GzCt
{
    WORD fc;                                        // frequency or code
    WORD dl;                                        // father node or length
};
struct GzDesc
{
    GzCt* dyn_tree;
    GzCt* static_tree;
    int* extra_bits;
    int extra_base;
    int elems;
    int max_length;
    int max_code;
};

ASM_VAR(GzCt, g_gzDynL, $L_77d08c)                  // [286 + 286 + 1]
ASM_VAR(GzCt, g_gzDynD, $L_77e2f0)                  // [2 * 30 + 1]
ASM_VAR(GzCt, g_gzStaticL, $L_77cae8)               // [288]
ASM_VAR(GzCt, g_gzStaticD, $L_77cf6c)               // [30]
ASM_VAR(GzCt, g_gzBlTree, $L_77cfec)                // [2 * 19 + 1]
ASM_VAR(WORD, g_gzBlCount, $L_77b504)               // [16]
ASM_VAR(int, g_gzHeap, $L_77d9f8)                   // [573]
ASM_VAR(int, g_gzHeapLen, $L_77b4fc)
ASM_VAR(int, g_gzHeapMax, $L_77b500)
ASM_VAR(BYTE, g_gzDepth, $L_77c8a8)
ASM_VAR(BYTE, g_gzLengthCode, $L_77b528)            // [256]
ASM_VAR(BYTE, g_gzDistCode, $L_77c6a0)              // [512]
ASM_VAR(int, g_gzBaseLength, $L_77c628)             // [29]
ASM_VAR(int, g_gzBaseDist, $L_77d980)               // [30]
ASM_VAR(WORD, g_gzDBuf, $L_76afd0)
ASM_VAR(BYTE, g_gzFlagBuf, $L_77b628)
ASM_VAR(unsigned, g_gzLastLit, $L_77cfe4)
ASM_VAR(unsigned, g_gzLastDist, $L_77cf68)
ASM_VAR(unsigned, g_gzLastFlags, $L_77d088)
ASM_VAR(BYTE, g_gzFlags, $L_77b525)
ASM_VAR(BYTE, g_gzFlagBit, $L_77b524)
ASM_VAR(DWORD, g_gzOptLen, $L_77c8a4)
ASM_VAR(DWORD, g_gzStaticLen, $L_77c8a0)
ASM_VAR(DWORD, g_gzCompressedLen, $L_77c69c)
ASM_VAR(DWORD, g_gzInputLen, $L_77b4f8)
ASM_VAR(WORD*, g_gzFileType, $L_77e2ec)
ASM_VAR(int*, g_gzFileMethod, $L_77cfe8)
ASM_VAR(int, g_gzLevel, $L_6897d8)
ASM_VAR(unsigned, g_gzStrstart, $L_77b4cc)
ASM_VAR(long, g_gzBlockStart, $L_77b4f0)
ASM_VAR(int, g_gzExtraLbits, $L_68a408)             // [29]
ASM_VAR(int, g_gzExtraDbits, $L_68a47c)             // [30]
ASM_VAR(BYTE, g_gzBlOrder, $L_68a59c)               // [19]
ASM_VAR(GzDesc, g_gzLDesc, $L_68a540)
ASM_VAR(GzDesc, g_gzDDesc, $L_68a560)
ASM_VAR(GzDesc, g_gzBlDesc, $L_68a580)
ASM_VAR(WORD, g_gzBiBuf, $L_77e3e4)
ASM_VAR(int, g_gzBiValid, $L_77e3e8)
ASM_VAR(BYTE, g_gzOutbuf, $L_73668c)                // [16384]
ASM_VAR(int, g_gzZfile, $L_77e3ec)
ASM_VAR(void*, g_gzReadBuf, $L_77e3f0)
ASM_VAR(unsigned, g_gzMaxChain, $L_77b4e0)
ASM_VAR(unsigned, g_gzPrevLength, $L_77b4c8)
ASM_VAR(unsigned, g_gzGoodMatch, $L_77b4e8)
ASM_VAR(unsigned, g_gzMatchStart, $L_77b4ec)
ASM_VAR(int, g_gzNiceMatch, $L_77b4dc)
ASM_VAR(WORD, g_gzPrev, $L_73ae90)                  // [32768]
ASM_VAR(WORD, g_gzHead, $L_74ae90)                  // [32768]
ASM_VAR(DWORD, g_gzWindowSize, $L_68a3a0)
ASM_VAR(unsigned, g_gzLookahead, $L_77b4f4)
ASM_VAR(int, g_gzEofile, $L_77b4d8)

#define GZ_OUTBUFSIZ 16384
#define GZ_HEAP_SIZE 573
#define GZ_SMALLER(tree, n, m) \
    ((tree)[n].fc < (tree)[m].fc || ((tree)[n].fc == (tree)[m].fc && (&g_gzDepth)[n] <= (&g_gzDepth)[m]))

static void Gz_PutByte(unsigned c)
{
    (&g_gzOutbuf)[g_gzOutcnt++] = (BYTE)c;
    if (g_gzOutcnt == GZ_OUTBUFSIZ)
        CallC(FUN_62b9a0);
}

static void Gz_PutShort(unsigned w)
{
    if (g_gzOutcnt < GZ_OUTBUFSIZ - 2)
    {
        (&g_gzOutbuf)[g_gzOutcnt++] = (BYTE)(w & 0xff);
        (&g_gzOutbuf)[g_gzOutcnt++] = (BYTE)((WORD)w >> 8);
    }
    else
    {
        Gz_PutByte((BYTE)(w & 0xff));
        Gz_PutByte((BYTE)((WORD)w >> 8));
    }
}

extern "C" {

// [FUN_62ebf0] bi_reverse: the low len bits of code reversed
unsigned __cdecl Gz_BiReverse(unsigned code, int len)
{
    unsigned res = 0;
    do
    {
        res |= code & 1;
        code >>= 1, res <<= 1;
    } while (--len > 0);
    return res >> 1;
}

// [FUN_62eb10] send_bits: value's length bits into the bit buffer
void __cdecl Gz_SendBits(int value, int length)
{
    if (g_gzBiValid > 16 - length)
    {
        g_gzBiBuf |= (WORD)(value << g_gzBiValid);
        Gz_PutShort(g_gzBiBuf);
        g_gzBiBuf = (WORD)((WORD)value >> (16 - g_gzBiValid));
        g_gzBiValid += length - 16;
    }
    else
    {
        g_gzBiBuf |= (WORD)(value << g_gzBiValid);
        g_gzBiValid += length;
    }
}

// [FUN_62eae0] bi_init: writing to zipfile through file_read
void __cdecl Gz_BiInit(int zipfile)
{
    g_gzReadBuf = (void*)&g_gzFileRead;
    g_gzZfile = zipfile;
    g_gzBiBuf = 0;
    g_gzBiValid = 0;
}

// [FUN_62ec10] bi_windup: the bit buffer flushed to a byte boundary
void __cdecl Gz_BiWindup()
{
    if (g_gzBiValid > 8)
        Gz_PutShort(g_gzBiBuf);
    else if (g_gzBiValid > 0)
        Gz_PutByte(g_gzBiBuf);
    g_gzBiBuf = 0;
    g_gzBiValid = 0;
}

// [FUN_62ece0] copy_block: a stored block, with its length header if asked
void __cdecl Gz_CopyBlock(const char* buf, unsigned len, int header)
{
    Gz_BiWindup();
    if (header)
    {
        Gz_PutShort((WORD)len);
        Gz_PutShort((WORD)~len);
    }
    while (len--)
        Gz_PutByte(*buf++);
}

// [FUN_62ddd0] init_block: a new block's statistics
void __cdecl Gz_InitBlock()
{
    int n;
    for (n = 0; n < 286; n++)
        (&g_gzDynL)[n].fc = 0;
    for (n = 0; n < 30; n++)
        (&g_gzDynD)[n].fc = 0;
    for (n = 0; n < 19; n++)
        (&g_gzBlTree)[n].fc = 0;
    (&g_gzDynL)[256].fc = 1;
    g_gzStaticLen = g_gzOptLen = 0;
    g_gzLastFlags = g_gzLastDist = g_gzLastLit = 0;
    g_gzFlags = 0;
    g_gzFlagBit = 1;
}

// [FUN_62de40] gen_codes: the codes of tree from the bit length counts
void __cdecl Gz_GenCodes(GzCt* tree, int max_code)
{
    WORD next_code[16];
    WORD code = 0;
    for (int bits = 1; bits <= 15; bits++)
        next_code[bits] = code = (WORD)((code + (&g_gzBlCount)[bits - 1]) << 1);
    for (int n = 0; n <= max_code; n++)
    {
        int len = tree[n].dl;
        if (len == 0)
            continue;
        tree[n].fc = (WORD)Gz_BiReverse(next_code[len]++, len);
    }
}

// [FUN_62e1d0] pqdownheap: the heap restored from node k down
void __cdecl Gz_PqDownHeap(GzCt* tree, int k)
{
    int* heap = &g_gzHeap;
    int v = heap[k];
    int j = k << 1;
    while (j <= g_gzHeapLen)
    {
        if (j < g_gzHeapLen && GZ_SMALLER(tree, heap[j + 1], heap[j]))
            j++;
        if (GZ_SMALLER(tree, v, heap[j]))
            break;
        heap[k] = heap[j];
        k = j;
        j <<= 1;
    }
    heap[k] = v;
}

// [FUN_62e270] gen_bitlen: the optimal bit lengths of a tree, limited to max_length
void __cdecl Gz_GenBitlen(GzDesc* desc)
{
    GzCt* tree = desc->dyn_tree;
    int* extra = desc->extra_bits;
    int base = desc->extra_base;
    int max_code = desc->max_code;
    int max_length = desc->max_length;
    GzCt* stree = desc->static_tree;
    int* heap = &g_gzHeap;
    WORD* bl_count = &g_gzBlCount;
    int h, n, m, bits, xbits, overflow = 0;
    WORD f;
    for (bits = 0; bits <= 15; bits++)
        bl_count[bits] = 0;
    tree[heap[g_gzHeapMax]].dl = 0;
    for (h = g_gzHeapMax + 1; h < GZ_HEAP_SIZE; h++)
    {
        n = heap[h];
        bits = tree[tree[n].dl].dl + 1;
        if (bits > max_length)
            bits = max_length, overflow++;
        tree[n].dl = (WORD)bits;
        if (n > max_code)
            continue;
        bl_count[bits]++;
        xbits = 0;
        if (n >= base)
            xbits = extra[n - base];
        f = tree[n].fc;
        g_gzOptLen += (DWORD)f * (bits + xbits);
        if (stree)
            g_gzStaticLen += (DWORD)f * (stree[n].dl + xbits);
    }
    if (overflow == 0)
        return;
    do
    {
        bits = max_length - 1;
        while (bl_count[bits] == 0)
            bits--;
        bl_count[bits]--;
        bl_count[bits + 1] += 2;
        bl_count[max_length]--;
        overflow -= 2;
    } while (overflow > 0);
    for (bits = max_length; bits != 0; bits--)
    {
        n = bl_count[bits];
        while (n != 0)
        {
            m = heap[--h];
            if (m > max_code)
                continue;
            if (tree[m].dl != (unsigned)bits)
            {
                g_gzOptLen += ((long)bits - (long)tree[m].dl) * (long)tree[m].fc;
                tree[m].dl = (WORD)bits;
            }
            n--;
        }
    }
}

// [FUN_62e000] build_tree: a Huffman tree for desc, its lengths and codes
void __cdecl Gz_BuildTree(GzDesc* desc)
{
    GzCt* tree = desc->dyn_tree;
    GzCt* stree = desc->static_tree;
    int elems = desc->elems;
    int* heap = &g_gzHeap;
    BYTE* depth = &g_gzDepth;
    int n, m, max_code = -1, node = elems;
    g_gzHeapLen = 0, g_gzHeapMax = GZ_HEAP_SIZE;
    for (n = 0; n < elems; n++)
    {
        if (tree[n].fc != 0)
        {
            heap[++g_gzHeapLen] = max_code = n;
            depth[n] = 0;
        }
        else
            tree[n].dl = 0;
    }
    while (g_gzHeapLen < 2)
    {
        int nw = heap[++g_gzHeapLen] = (max_code < 2 ? ++max_code : 0);
        tree[nw].fc = 1;
        depth[nw] = 0;
        g_gzOptLen--;
        if (stree)
            g_gzStaticLen -= stree[nw].dl;
    }
    desc->max_code = max_code;
    for (n = g_gzHeapLen / 2; n >= 1; n--)
        Gz_PqDownHeap(tree, n);
    do
    {
        n = heap[1];
        heap[1] = heap[g_gzHeapLen--];
        Gz_PqDownHeap(tree, 1);
        m = heap[1];
        heap[--g_gzHeapMax] = n;
        heap[--g_gzHeapMax] = m;
        tree[node].fc = (WORD)(tree[n].fc + tree[m].fc);
        depth[node] = (BYTE)((depth[n] >= depth[m] ? depth[n] : depth[m]) + 1);
        tree[n].dl = tree[m].dl = (WORD)node;
        heap[1] = node++;
        Gz_PqDownHeap(tree, 1);
    } while (g_gzHeapLen >= 2);
    heap[--g_gzHeapMax] = heap[1];
    Gz_GenBitlen(desc);
    Gz_GenCodes(tree, max_code);
}

// [FUN_62e4d0] scan_tree: the run lengths of tree's code lengths counted into bl_tree
void __cdecl Gz_ScanTree(GzCt* tree, int max_code)
{
    GzCt* bl = &g_gzBlTree;
    int n, prevlen = -1, curlen, nextlen = tree[0].dl, count = 0, max_count = 7, min_count = 4;
    if (nextlen == 0)
        max_count = 138, min_count = 3;
    tree[max_code + 1].dl = (WORD)0xffff;
    for (n = 0; n <= max_code; n++)
    {
        curlen = nextlen;
        nextlen = tree[n + 1].dl;
        if (++count < max_count && curlen == nextlen)
            continue;
        else if (count < min_count)
            bl[curlen].fc += (WORD)count;
        else if (curlen != 0)
        {
            if (curlen != prevlen)
                bl[curlen].fc++;
            bl[16].fc++;
        }
        else if (count <= 10)
            bl[17].fc++;
        else
            bl[18].fc++;
        count = 0;
        prevlen = curlen;
        if (nextlen == 0)
            max_count = 138, min_count = 3;
        else if (curlen == nextlen)
            max_count = 6, min_count = 3;
        else
            max_count = 7, min_count = 4;
    }
}

static void Gz_SendCode(int c, const GzCt* tree)
{
    Gz_SendBits(tree[c].fc, tree[c].dl);
}

// [FUN_62e640] send_tree: tree's code lengths sent in their run length form
void __cdecl Gz_SendTree(GzCt* tree, int max_code)
{
    GzCt* bl = &g_gzBlTree;
    int n, prevlen = -1, curlen, nextlen = tree[0].dl, count = 0, max_count = 7, min_count = 4;
    if (nextlen == 0)
        max_count = 138, min_count = 3;
    for (n = 0; n <= max_code; n++)
    {
        curlen = nextlen;
        nextlen = tree[n + 1].dl;
        if (++count < max_count && curlen == nextlen)
            continue;
        else if (count < min_count)
        {
            do
            {
                Gz_SendCode(curlen, bl);
            } while (--count != 0);
        }
        else if (curlen != 0)
        {
            if (curlen != prevlen)
            {
                Gz_SendCode(curlen, bl);
                count--;
            }
            Gz_SendCode(16, bl);
            Gz_SendBits(count - 3, 2);
        }
        else if (count <= 10)
        {
            Gz_SendCode(17, bl);
            Gz_SendBits(count - 3, 3);
        }
        else
        {
            Gz_SendCode(18, bl);
            Gz_SendBits(count - 11, 7);
        }
        count = 0;
        prevlen = curlen;
        if (nextlen == 0)
            max_count = 138, min_count = 3;
        else if (curlen == nextlen)
            max_count = 6, min_count = 3;
        else
            max_count = 7, min_count = 4;
    }
}

// [FUN_62e470] build_bl_tree: the bit length tree; the last of its codes to send
int __cdecl Gz_BuildBlTree()
{
    int max_blindex;
    Gz_ScanTree(&g_gzDynL, g_gzLDesc.max_code);
    Gz_ScanTree(&g_gzDynD, g_gzDDesc.max_code);
    Gz_BuildTree(&g_gzBlDesc);
    for (max_blindex = 18; max_blindex >= 3; max_blindex--)
        if ((&g_gzBlTree)[(&g_gzBlOrder)[max_blindex]].dl != 0)
            break;
    g_gzOptLen += 3 * (max_blindex + 1) + 5 + 5 + 4;
    return max_blindex;
}

// [FUN_62e5c0] send_all_trees: the dynamic trees' header
void __cdecl Gz_SendAllTrees(int lcodes, int dcodes, int blcodes)
{
    Gz_SendBits(lcodes - 257, 5);
    Gz_SendBits(dcodes - 1, 5);
    Gz_SendBits(blcodes - 4, 4);
    for (int rank = 0; rank < blcodes; rank++)
        Gz_SendBits((&g_gzBlTree)[(&g_gzBlOrder)[rank]].dl, 3);
    Gz_SendTree(&g_gzDynL, lcodes - 1);
    Gz_SendTree(&g_gzDynD, dcodes - 1);
}

// [FUN_62e910] compress_block: the block's literals and matches through the trees
void __cdecl Gz_CompressBlock(GzCt* ltree, GzCt* dtree)
{
    unsigned dist, lx = 0, dx = 0, fx = 0, code;
    int lc, extra;
    BYTE flag = 0;
    if (g_gzLastLit != 0)
        do
        {
            if ((lx & 7) == 0)
                flag = (&g_gzFlagBuf)[fx++];
            lc = (&g_gzInbuf)[lx++];
            if ((flag & 1) == 0)
                Gz_SendCode(lc, ltree);
            else
            {
                code = (&g_gzLengthCode)[lc];
                Gz_SendCode(code + 256 + 1, ltree);
                extra = (&g_gzExtraLbits)[code];
                if (extra != 0)
                {
                    lc -= (&g_gzBaseLength)[code];
                    Gz_SendBits(lc, extra);
                }
                dist = (&g_gzDBuf)[dx++];
                code = dist < 256 ? (&g_gzDistCode)[dist] : (&g_gzDistCode)[256 + (dist >> 7)];
                Gz_SendCode(code, dtree);
                extra = (&g_gzExtraDbits)[code];
                if (extra != 0)
                {
                    dist -= (&g_gzBaseDist)[code];
                    Gz_SendBits(dist, extra);
                }
            }
            flag >>= 1;
        } while (lx < g_gzLastLit);
    Gz_SendCode(256, ltree);
}

// [FUN_62ea70] set_file_type: binary or ascii by the literal counts
void __cdecl Gz_SetFileType()
{
    int n = 0;
    unsigned ascii_freq = 0, bin_freq = 0;
    while (n < 7)
        bin_freq += (&g_gzDynL)[n++].fc;
    while (n < 128)
        ascii_freq += (&g_gzDynL)[n++].fc;
    while (n < 256)
        bin_freq += (&g_gzDynL)[n++].fc;
    *g_gzFileType = bin_freq > (ascii_freq >> 2) ? 0 : 1;
}

// [FUN_62deb0] flush_block: the block written stored, with the fixed or with its own trees, whichever is
// shortest; the compressed length so far
DWORD __cdecl Gz_FlushBlock(const char* buf, DWORD stored_len, int eof)
{
    DWORD opt_lenb, static_lenb;
    (&g_gzFlagBuf)[g_gzLastFlags] = g_gzFlags;
    if (*g_gzFileType == (WORD)0xffff)
        Gz_SetFileType();
    Gz_BuildTree(&g_gzLDesc);
    Gz_BuildTree(&g_gzDDesc);
    int max_blindex = Gz_BuildBlTree();
    opt_lenb = (g_gzOptLen + 3 + 7) >> 3;
    static_lenb = (g_gzStaticLen + 3 + 7) >> 3;
    g_gzInputLen += stored_len;
    if (static_lenb <= opt_lenb)
        opt_lenb = static_lenb;
    if (stored_len + 4 <= opt_lenb && buf != 0)
    {
        Gz_SendBits((0 << 1) + eof, 3);
        g_gzCompressedLen = (g_gzCompressedLen + 3 + 7) & ~7L;
        g_gzCompressedLen += (stored_len + 4) << 3;
        Gz_CopyBlock(buf, (unsigned)stored_len, 1);
    }
    else if (static_lenb == opt_lenb)
    {
        Gz_SendBits((1 << 1) + eof, 3);
        Gz_CompressBlock(&g_gzStaticL, &g_gzStaticD);
        g_gzCompressedLen += 3 + g_gzStaticLen;
    }
    else
    {
        Gz_SendBits((2 << 1) + eof, 3);
        Gz_SendAllTrees(g_gzLDesc.max_code + 1, g_gzDDesc.max_code + 1, max_blindex + 1);
        Gz_CompressBlock(&g_gzDynL, &g_gzDynD);
        g_gzCompressedLen += 3 + g_gzOptLen;
    }
    Gz_InitBlock();
    if (eof)
    {
        Gz_BiWindup();
        g_gzCompressedLen += 7;
    }
    return g_gzCompressedLen >> 3;
}

// [FUN_62e7b0] ct_tally: a literal (dist 0) or a match recorded; whether the block should be flushed now
int __cdecl Gz_CtTally(int dist, int lc)
{
    (&g_gzInbuf)[g_gzLastLit++] = (BYTE)lc;
    if (dist == 0)
        (&g_gzDynL)[lc].fc++;
    else
    {
        dist--;
        (&g_gzDynL)[(&g_gzLengthCode)[lc] + 256 + 1].fc++;
        (&g_gzDynD)[dist < 256 ? (&g_gzDistCode)[dist] : (&g_gzDistCode)[256 + (dist >> 7)]].fc++;
        (&g_gzDBuf)[g_gzLastDist++] = (WORD)dist;
        g_gzFlags |= g_gzFlagBit;
    }
    g_gzFlagBit <<= 1;
    if ((g_gzLastLit & 7) == 0)
    {
        (&g_gzFlagBuf)[g_gzLastFlags++] = g_gzFlags;
        g_gzFlags = 0, g_gzFlagBit = 1;
    }
    if (g_gzLevel > 2 && (g_gzLastLit & 0xfff) == 0)
    {
        DWORD out_length = (DWORD)g_gzLastLit * 8L;
        DWORD in_length = (DWORD)g_gzStrstart - g_gzBlockStart;
        for (int dcode = 0; dcode < 30; dcode++)
            out_length += (DWORD)(&g_gzDynD)[dcode].fc * (5L + (&g_gzExtraDbits)[dcode]);
        out_length >>= 3;
        if (g_gzLastDist < g_gzLastLit / 2 && out_length < in_length / 2)
            return 1;
    }
    return g_gzLastLit == 0x8000 - 1 || g_gzLastDist == 0x8000;
}

// [FUN_62dba0] ct_init: the static trees and code tables set up once; attr gets the file type
void __cdecl Gz_CtInit(WORD* attr, int* methodp)
{
    int n, bits, length, code, dist;
    g_gzFileType = attr;
    g_gzFileMethod = methodp;
    g_gzCompressedLen = g_gzInputLen = 0L;
    if ((&g_gzStaticD)[0].dl != 0)
        return;
    length = 0;
    for (code = 0; code < 28; code++)
    {
        (&g_gzBaseLength)[code] = length;
        for (n = 0; n < (1 << (&g_gzExtraLbits)[code]); n++)
            (&g_gzLengthCode)[length++] = (BYTE)code;
    }
    (&g_gzLengthCode)[length - 1] = (BYTE)code;
    dist = 0;
    for (code = 0; code < 16; code++)
    {
        (&g_gzBaseDist)[code] = dist;
        for (n = 0; n < (1 << (&g_gzExtraDbits)[code]); n++)
            (&g_gzDistCode)[dist++] = (BYTE)code;
    }
    dist >>= 7;
    for (; code < 30; code++)
    {
        (&g_gzBaseDist)[code] = dist << 7;
        for (n = 0; n < (1 << ((&g_gzExtraDbits)[code] - 7)); n++)
            (&g_gzDistCode)[256 + dist++] = (BYTE)code;
    }
    for (bits = 0; bits <= 15; bits++)
        (&g_gzBlCount)[bits] = 0;
    GzCt* sl = &g_gzStaticL;
    n = 0;
    while (n <= 143)
        sl[n++].dl = 8, (&g_gzBlCount)[8]++;
    while (n <= 255)
        sl[n++].dl = 9, (&g_gzBlCount)[9]++;
    while (n <= 279)
        sl[n++].dl = 7, (&g_gzBlCount)[7]++;
    while (n <= 287)
        sl[n++].dl = 8, (&g_gzBlCount)[8]++;
    Gz_GenCodes(sl, 286 + 1);
    for (n = 0; n < 30; n++)
    {
        (&g_gzStaticD)[n].dl = 5;
        (&g_gzStaticD)[n].fc = (WORD)Gz_BiReverse(n, 5);
    }
    Gz_InitBlock();
}

// [FUN_62d400] longest_match: the longest match at strstart along the hash chain from cur_match (its start in
// match_start)
int __cdecl Gz_LongestMatch(unsigned cur_match)
{
    BYTE* window = &g_gzSlide;
    unsigned chain_length = g_gzMaxChain;
    BYTE* scan = window + g_gzStrstart;
    BYTE* match;
    int len;
    int best_len = g_gzPrevLength;
    unsigned limit = g_gzStrstart > 32506 ? g_gzStrstart - 32506 : 0;
    BYTE* strend = window + g_gzStrstart + 258;
    BYTE scan_end1 = scan[best_len - 1];
    BYTE scan_end = scan[best_len];
    if (g_gzPrevLength >= g_gzGoodMatch)
        chain_length >>= 2;
    do
    {
        match = window + cur_match;
        if (match[best_len] != scan_end || match[best_len - 1] != scan_end1 || *match != *scan || *++match != scan[1])
            continue;
        scan += 2, match++;
        do
        {
        } while (*++scan == *++match && *++scan == *++match && *++scan == *++match && *++scan == *++match
                 && *++scan == *++match && *++scan == *++match && *++scan == *++match && *++scan == *++match
                 && scan < strend);
        len = 258 - (int)(strend - scan);
        scan = strend - 258;
        if (len > best_len)
        {
            g_gzMatchStart = cur_match;
            best_len = len;
            if (len >= g_gzNiceMatch)
                break;
            scan_end1 = scan[best_len - 1];
            scan_end = scan[best_len];
        }
    } while ((cur_match = (&g_gzPrev)[cur_match & 0x7fff]) > limit && --chain_length != 0);
    return best_len;
}

// [FUN_62d550] fill_window: more input read into the window, sliding it by 32 KB when needed
void __cdecl Gz_FillWindow()
{
    unsigned n, m;
    unsigned more = (unsigned)(g_gzWindowSize - (DWORD)g_gzLookahead - (DWORD)g_gzStrstart);
    BYTE* window = &g_gzSlide;
    if (more == (unsigned)-1)
        more--;
    else if (g_gzStrstart >= 0x8000 + 32506)
    {
        memcpy(window, window + 0x8000, 0x8000);
        g_gzMatchStart -= 0x8000;
        g_gzStrstart -= 0x8000;
        g_gzBlockStart -= 0x8000L;
        for (n = 0; n < 0x8000; n++)
        {
            m = (&g_gzHead)[n];
            (&g_gzHead)[n] = (WORD)(m >= 0x8000 ? m - 0x8000 : 0);
        }
        for (n = 0; n < 0x8000; n++)
        {
            m = (&g_gzPrev)[n];
            (&g_gzPrev)[n] = (WORD)(m >= 0x8000 ? m - 0x8000 : 0);
        }
        more += 0x8000;
    }
    if (!g_gzEofile)
    {
        n = ((unsigned(__cdecl*)(char*, unsigned))g_gzReadBuf)((char*)window + g_gzStrstart + g_gzLookahead, more);
        if (n == 0 || n == (unsigned)-1)
            g_gzEofile = 1;
        else
            g_gzLookahead += n;
    }
}

} // extern "C"
