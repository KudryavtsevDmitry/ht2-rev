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

// [FUN_608c70] turns a grid point {x, y} in place into its node: world x/y, height, normal, the scaled offsets
extern "C" float* __fastcall TGrid_MakeNode(BYTE* self, void*, float* p, void*)
{
    DWORD X = ((DWORD*)p)[0], Y = ((DWORD*)p)[1];
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

ASM_PROC(FUN_5b1020)
ASM_PROC(FUN_5d4c10)

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

ASM_PROC(FUN_613b50)
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")
extern "C" void Tgr_VecCtor(); KING_ALIAS("_Tgr_VecCtor", "??_L@YGXPAXIHP6EX0@Z1@Z")
ASM_VAR(AsmLabel, g_q27Ctor61bee0, $L_61bee0)
ASM_VAR(AsmLabel, s_q27_6724f8, $L_6724f8)
ASM_VAR(AsmLabel, s_q27_685dac, $L_685dac)
ASM_VAR(AsmLabel, s_q27_6724b0, $L_6724b0)

static BYTE* Tgr_NewCells()
{
    BYTE* p = (BYTE*)operator new(1056784);
    if (p)
        CallStd(Tgr_VecCtor, p, 16, 66049, (BYTE*)g_q27Ctor61bee0, FUN_50d190);
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
ASM_VAR(AsmLabel, g_q46_61c160, $L_61c160)
ASM_VAR(AsmLabel, g_q46_61c0f0, $L_61c0f0)
// [FUN_6072b0] the deepest level allowed (+24 of the static grid 72c108)
extern "C" int __cdecl Grid_DeepestLevel(void)
{
    return CallThis<int>((void (*)())(void*)g_q46_61c180, (void*)g_q46_72c108);
}
// [FUN_6072c0] the +32 value of the static grid 72c108
extern "C" int __cdecl Grid_Level32(void)
{
    return CallThis<int>((void (*)())(void*)g_q46_61c160, (void*)g_q46_72c108);
}
// [FUN_61c0c0] constructs the static grid 72c108
extern "C" int __cdecl Grid72c108_Construct(void)
{
    return CallThis<int>((void (*)())(void*)g_q46_61c0f0, (void*)g_q46_72c108);
}
