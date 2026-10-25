// terrain.cpp - terrain.cpp of the terrain library, rewritten from
// king.masm: detail levels from the view distance, the height tables of the
// 3x3 tiles (257x257 samples of 16 bytes), the visible-area test, the
// gradient noise lookup, the decal vertex list and the room's objects.
//
// Building with  nmake ASM_TERRAIN=1  makes king.masm assemble the original
// procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef int Ints[1];
typedef DWORD Dwords[1];

extern "C" void FUN_616ca0(); KING_ALIAS("_FUN_616ca0", "_Noise_Seed")    // seeds the noise tables
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")    // the scene object with this name (0, name)
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")    // error report

ASM_VAR(Ints, g_perm, $L_72b068)                 // noise permutation
ASM_VAR(Dwords, g_noiseGrad, $L_729850)          // 12-byte gradients
ASM_VAR(Dwords, g_noiseA, $L_729044)
ASM_VAR(Dwords, g_noiseB, $L_72883c)
ASM_VAR(Ints, g_noiseC, $L_72b870)
ASM_VAR(BYTE*, g_6cec6c, $L_6cec6c)
ASM_VAR(AsmLabel, s_roomAa, $L_685e00)            // "room_aa_000"
ASM_VAR(AsmLabel, s_roomTr, $L_685df4)            // "room_tr_000"
ASM_VAR(AsmLabel, s_buildDate, $L_6724f8)
ASM_VAR(AsmLabel, s_terrainFile, $L_685dd0)
ASM_VAR(AsmLabel, s_fileLine, $L_6724b0)

extern "C" {

// [FUN_6144f0] the detail levels for a view distance a * b
void __fastcall Terrain_SetDetail(BYTE* self, void*, float a, float b)
{
    double d = (double)a * b;
    int n = (int)(d * 40.0f);
    F(int, self, 64) = n;
    F(int, self, 68) = n;
    if (n < 5) {
        F(int, self, 68) = 0;
        F(int, self, 64) = 0;
    }
    F(float, self, 72) = (float)(750.0f / (double)F(int, self, 64));
    F(float, self, 76) = (float)(750.0f / (double)F(int, self, 68));
    n = (int)(d * 20.0f);
    F(int, self, 48) = n;
    F(int, self, 52) = n;
    if (n < 5) {
        F(int, self, 52) = 0;
        F(int, self, 48) = 0;
    }
    F(float, self, 56) = (float)(250.0f / (double)F(int, self, 48));
    F(float, self, 60) = (float)(250.0f / (double)F(int, self, 52));
}

// [FUN_6152a0] the height sample at (x, y) of the 3x3 tiles: the corner
// tiles of one diagonal come from table +4, the others from +0 or +8
float __fastcall Terrain_Sample(BYTE** self, void*, DWORD x, DWORD y)
{
    DWORD tx = x / 257, ty = y / 257;
    BYTE* t;
    if (tx == 3 || ty == 3)
        t = self[1];
    else if (2 - tx != ty)
        t = self[2];
    else if (tx == 0)
        t = ty == 2 && 771 - y <= 257 - x ? self[1] : self[0];
    else if (tx == 2 && ty == 0)
        t = 771 - x <= 257 - y ? self[1] : self[0];
    else
        t = self[0];
    return *(float*)(t + ((x % 257) * 257 + y % 257) * 16);
}

// [FUN_616c10] p lies in the visible square (with a 60 unit margin)
int __fastcall Terrain_InView(BYTE* self, void*, const float* p)
{
    double h = F(float, self, 40) * 0.5f;
    if ((double)F(float, self, 28) - h > p[0])
        return 0;
    if (!(h + F(float, self, 28) - 60.0f >= p[0]))
        return 0;
    h = F(float, self, 44) * 0.5f;
    if ((double)F(float, self, 32) - h > p[1])
        return 0;
    if (!(h + F(float, self, 32) - 60.0f >= p[1]))
        return 0;
    return 1;
}

// [FUN_616f60] the noise lattice values at (x, y, z)
int __cdecl Noise_Lattice(int x, int y, int z, DWORD* grad, DWORD* a, DWORD* b)
{
    if (x <= 0)
        x = -x;
    if (y <= 0)
        y = -y;
    if (z <= 0)
        z = -z;
    x &= 255;
    y &= 255;
    z &= 255;
    int i = g_perm[g_perm[x] + y] + z;
    grad[0] = g_noiseGrad[i * 3];
    grad[1] = g_noiseGrad[i * 3 + 1];
    grad[2] = g_noiseGrad[i * 3 + 2];
    *b = g_noiseA[g_perm[g_perm[x] + y] + z];
    *a = g_noiseB[g_perm[g_perm[x] + y] + z];
    return g_noiseC[g_perm[g_perm[x] + y] + z];
}

// [FUN_617020]
int __cdecl Noise_Lattice2(int x, int y, float* grad, DWORD* a, DWORD* b)
{
    DWORD g[3];
    int r = Noise_Lattice(x, y, 1, g, a, b);
    ((DWORD*)grad)[1] = g[1];
    grad[0] = *(float*)&g[0];
    ((DWORD*)grad)[2] = g[2];
    return r;
}

// The decal list: +0 owner, +4 24-byte vertices, +8 count, +12 capacity,
// +16, +20, +24, +28.

// [FUN_617060]
BYTE* __fastcall Decals_Construct(BYTE* self, void*, int owner, int n, int c, int d)
{
    F(int, self, 0) = owner;
    F(int, self, 20) = c;
    F(int, self, 24) = d;
    F(int, self, 12) = n * 3;
    F(void*, self, 4) = operator new(n * 3 * 24);
    F(int, self, 16) = 0;
    F(int, self, 28) = -1;
    F(int, self, 8) = 0;
    CallC(FUN_616ca0, 2158);
    return self;
}

// [FUN_6170d0]
void __fastcall Decals_Free(BYTE* self, void*)
{
    if (F(void*, self, 4)) {
        operator delete(F(void*, self, 4));
        F(void*, self, 4) = 0;
    }
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
}

// [FUN_617100]
void __fastcall Decals_Reset16(BYTE* self, void*)
{
    F(int, self, 16) = 0;
}

// [FUN_617110]
void __fastcall Decals_Add(BYTE* self, void*, const DWORD* pos, const DWORD* color, float u, float v)
{
    if (F(int, self, 12) == F(int, self, 8))
        return;
    DWORD* r = (DWORD*)(F(BYTE*, self, 4) + F(int, self, 8) * 24);
    r[0] = pos[0];
    r[1] = pos[1];
    r[2] = pos[2];
    r[3] = *color;
    ((float*)r)[4] = u;
    ((float*)r)[5] = v;
    F(int, self, 8) = F(int, self, 8) + 1;
}

// [FUN_619840]
void __fastcall Terrain_Reset8(BYTE* self, void*)
{
    F(int, self, 8) = 0;
}

// The room objects: +0, +4, 10 objects at +8, +48.

// [FUN_619970]
BYTE* __fastcall RoomObjects_Construct(BYTE* self, void*, int a, int b)
{
    F(int, self, 0) = b;
    F(int, self, 4) = a;
    for (int k = 0; k < 10; k++)
        F(int, self, 8 + k * 4) = 0;
    return self;
}

// [FUN_619b50] adds the objects to the terrain's room
void __fastcall RoomObjects_Attach(BYTE* self, void*)
{
    const char* name = F(int, g_6cec6c, 1316) ? s_roomTr : s_roomAa;
    BYTE* room = CallC<BYTE*>(FUN_5ef2b0, 0, name);
    if (!room)
        CallC(FUN_60aad0, 0x88760115, (const char*)s_fileLine, (const char*)s_terrainFile, 810,
              (const char*)s_buildDate);
    for (int k = 0; k < 10; k++)
        if (F(BYTE*, self, 8 + k * 4))
            CallVirt(room, 20, 0, F(BYTE*, self, 8 + k * 4));
}

// [FUN_619bd0]
void __fastcall RoomObjects_Clear(BYTE* self, void*)
{
    F(int, self, 0) = 0;
    F(int, self, 4) = 0;
    for (int k = 0; k < 10; k++) {
        BYTE*& o = F(BYTE*, self, 8 + k * 4);
        if (o) {
            CallVirt(o, 36);
            if (o)
                CallVirt(o, 0, 1);
            o = 0;
        }
    }
    F(int, self, 48) = 0;
}

}

extern "C" void FUN_60bbb0(); KING_ALIAS("_FUN_60bbb0", "@Mesh_Clear@4")
extern "C" void FUN_5f4a90(); KING_ALIAS("_FUN_5f4a90", "@Value_Free@4")
extern "C" void FUN_5ef660(); KING_ALIAS("_FUN_5ef660", "@Sce5ef660_Destroy@4")
ASM_VAR(char, g_vtbl6532ec, $L_6532ec)
ASM_VAR(char, g_vtbl653308, $L_653308)

extern "C" {

// [FUN_614490] destructor: deletes the object at +4
void __fastcall Terrain614490_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vtbl6532ec;
    BYTE* o = F(BYTE*, self, 4);
    if (o) {
        CallVirt(o, 0, 1);
        F(BYTE*, self, 4) = 0;
    }
    CallThis(FUN_60bbb0, self);
}

// [FUN_619ae0] destructor
void __fastcall Terrain619ae0_Destroy(BYTE* self, void*)
{
    int* back = F(int*, self, 76);
    F(void*, self, 0) = &g_vtbl653308;
    *back = 0;
    CallThis(FUN_5f4a90, self + 64);
    if (F(void*, self, 60))
        operator delete(F(void*, self, 60));
    CallThis(FUN_5ef660, self);
}

}

extern "C" void FUN_60c2b0(); KING_ALIAS("_FUN_60c2b0", "@GridMesh_Frame@8")
ASM_PROC(FUN_619c20)
extern "C" void FUN_619850(); KING_ALIAS("_FUN_619850", "@TerrainPart_Draw@12")
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")    // reports an error code with file, line and date
extern "C" void FUN_617100(); KING_ALIAS("_FUN_617100", "@Decals_Reset16@8")
ASM_VAR(BYTE*, g_6cec6c, $L_6cec6c)
extern "C" __declspec(dllimport) DWORD __stdcall timeGetTime(void);

extern "C" {

// [FUN_615670] per-frame update of a terrain object and its five parts
void __fastcall Terrain615670_Update(BYTE** self, void*, int frame)
{
    if (frame == F(int, F(BYTE*, g_6cec6c, 1356), 464)) {
        F(DWORD, self[0], 104) = timeGetTime();
        CallThis(FUN_60c2b0, self[0]);
    }
    CallVirt(F(BYTE*, self[0], 4), 24, self[27], frame);
    BYTE** p = self + 20;
    if (p[0] && p[1] && p[2] && p[3] && p[4]) {
        BYTE* parts[6] = {p[1], p[2], p[3], p[4], p[5], 0};
        CallThis(FUN_619c20, p[0], parts);
        for (int i = 1; i <= 5; i++)
            CallThis(FUN_619850, p[i], frame);
    }
}

// [FUN_616770] sets the three corner records of the mesh (n must be 3)
void __fastcall Terrain616770_SetCorners(BYTE** self, void*, BYTE** c, int n)
{
    BYTE* m = F(BYTE*, self[0], 4);
    if (n == 3 && c[0] && c[2]) {
        F(BYTE*, m, 40104) = c[0];
        F(BYTE*, m, 40108) = c[1];
        F(BYTE*, m, 40112) = c[2];
        F(int, F(BYTE*, m, 40104), 48) = 1;
        F(int, F(BYTE*, m, 40112), 48) = 0;
    } else {
        CallC(FUN_60aad0, 2289444626u, " File = %s, Line = #%ld, Date = %s ",
              "C:\\Nek\\VRAPPLVS\\terrain\\TMesh.inl", 308, "Jan 16 2003");
        F(BYTE*, m, 40104) = 0;
        F(BYTE*, m, 40112) = 0;
    }
    if (self[21] && self[22] && self[23] && self[24] && self[25])
        for (int i = 21; i <= 25; i++)
            CallThis(FUN_617100, self[i]);
}

}

extern "C" void FUN_5d9290(); KING_ALIAS("_FUN_5d9290", "@Scenes_Find@12")
extern "C" void FUN_4b8720(); KING_ALIAS("_FUN_4b8720", "_Fx_FindByName")
extern "C" void FUN_5b1020(); KING_ALIAS("_FUN_5b1020", "@Material_Select@24")
extern "C" void FUN_5ef6d0(); KING_ALIAS("_FUN_5ef6d0", "@Node_Construct@16")    // named object: constructor (name, kind)
ASM_VAR(BYTE*, g_696224, $L_696224)
ASM_VAR(BYTE*, g_d3dDevice720908, $L_720908)

// IDirect3DDevice methods: SetRenderState (+80), DrawPrimitive (+100)
static void Dev_SetRenderState(BYTE* dev, int state, int value)
{
    typedef int (__stdcall* Fn)(BYTE*, int, int);
    (*(Fn*)(*(BYTE**)dev + 80))(dev, state, value);
}

static void Dev_DrawTriangles(BYTE* dev, BYTE* verts, int count)
{
    typedef int (__stdcall* Fn)(BYTE*, int, int, BYTE*, int, int);
    (*(Fn*)(*(BYTE**)dev + 100))(dev, 4, 322, verts, count, 0);
}

extern "C" {

// [FUN_619850] draws a part's triangles (24-byte vertices, at most 1023
// per call) with its texture
void __fastcall TerrainPart_Draw(BYTE* self, void*, BYTE* view)
{
    if (!F(int, self, 8))
        return;
    if (!F(BYTE*, self, 16)) {
        int t = CallThis<int>(FUN_5d9290, g_696224, F(int, self, 24));
        F(int, self, 16) = CallC<int>(FUN_4b8720, F(int, self, 20), t);
        if (!F(int, self, 16))
            CallC(FUN_60aad0, 2289444629u, " File = %s, Line = #%ld, Date = %s ",
                            "C:\\Nek\\VRAPPLVS\\terrain\\terrain.cpp", 745, "Jan 16 2003");
    }
    CallThis(FUN_5b1020, view, F(int, self, 16), 0, 0, 0);
    Dev_SetRenderState(g_d3dDevice720908, 136, 1);
    unsigned n = F(unsigned, self, 8);
    if (n <= 1023) {
        Dev_DrawTriangles(g_d3dDevice720908, F(BYTE*, self, 4), n);
        return;
    }
    unsigned i = 0;
    int off = 0;
    if (n - 1023) {
        do {
            Dev_DrawTriangles(g_d3dDevice720908, F(BYTE*, self, 4) + off, 1023);
            i += 1023;
            off += 24552;
        } while (i < F(unsigned, self, 8) - 1023);
    }
    n = F(unsigned, self, 8);
    if (n > i)
        Dev_DrawTriangles(g_d3dDevice720908, F(BYTE*, self, 4) + i * 24, n - i);
}

// [FUN_6199a0] creates the missing sharpTerrainTree objects of the 10 slots
// at +8
void __fastcall Terrain6199a0_CreateTrees(BYTE* self, void*)
{
    BYTE** slot = (BYTE**)(self + 8);
    for (int k = 0; k < 10; k++, slot++) {
        if (*slot)
            continue;
        BYTE* o = (BYTE*)operator new(80);
        if (o) {
            CallThis(FUN_5ef6d0, o, "sharpTerrainTree", 9);
            F(int, o, 60) = 0;
            F(int, o, 64) = 7;
            F(void*, o, 0) = &g_vtbl653308;
            F(BYTE**, o, 76) = slot;
        }
        *slot = o;
        F(BYTE, o, 40) |= 1;
        F(int, *slot, 48) = 1;
        F(int, *slot, 52) = 50;
        F(int, *slot, 56) = 2;
        BYTE* q = (BYTE*)operator new(8);
        F(BYTE*, *slot, 60) = q;
        F(int, F(BYTE*, *slot, 60), 0) = 0;
        F(WORD, F(BYTE*, *slot, 60), 4) = 2;
        F(WORD, F(BYTE*, *slot, 60), 6) = 0;
        F(int, *slot, 68) = 2;
        DWORD* r = (DWORD*)operator new(24);
        if (r)
            for (int i = 0; i < 6; i++)
                r[i] = 0;
        F(DWORD*, *slot, 72) = r;
    }
    F(int, self, 48) = 0;
}

}

// ---------------------------------------------------------------------------
// x87 functions of the unit: height tables and vector copies

extern "C" void __cdecl rewind(FILE* f);
extern "C" void FUN_5ab0f0(); KING_ALIAS("_FUN_5ab0f0", "_File_ReadShort")    // a 16-bit word from a file

// [FUN_613a80] (tdata.cpp) reads 257x257 16-bit heights into the floats at a 16-byte stride
extern "C" int __fastcall Tile_ReadHeights(BYTE* self,void*,FILE* f)
{
    if(!f) return 0;
    rewind(f);
    static WORD raw[257*257];
    for(int i=0;i<257*257;i++) raw[i]=CallC<WORD>(FUN_5ab0f0,f);
    for(int i=0;i<257*257;i++)
        F(float,self,i*16)=(float)((((double)raw[i]*1.5259021893143654e-05f-0.5f)*100.0f)+474.8999938964844f);
    return 1;
}

struct Vec3T { float x,y,z; };
extern "C" void FUN_614960(); KING_ALIAS("_FUN_614960", "@TerrainView_Move@20")
extern "C" void FUN_614f70(); KING_ALIAS("_FUN_614f70", "@TerrainView_Place@20")
ASM_PROC(FUN_617180)

// [FUN_614f00] FUN_614960 on copies of three vectors
extern "C" int __stdcall Terrain_Call614960(const Vec3T* a,const Vec3T* b,const Vec3T* c)
{
    Vec3T ca=*a,cb=*b,cc=*c;
    return CallStd<int>(FUN_614960,&ca,&cb,&cc);
}

// [FUN_615230] FUN_614f70 on copies of three vectors
extern "C" int __stdcall Terrain_Call614f70(const Vec3T* a,const Vec3T* b,const Vec3T* c)
{
    Vec3T ca=*a,cb=*b,cc=*c;
    return CallStd<int>(FUN_614f70,&ca,&cb,&cc);
}

// [FUN_6197e0] FUN_617180 on copies of two vectors
extern "C" int __stdcall Terrain_Call617180(const Vec3T* a,const Vec3T* b,int c,int d,int e)
{
    Vec3T cb=*b,ca=*a;
    return CallStd<int>(FUN_617180,&ca,&cb,c,d,e);
}

#include <mmsystem.h>
typedef char TerrainLabel[1];
ASM_VAR(TerrainLabel, g_vtbl652f3c, $L_652f3c)
ASM_VAR(const float, k_zero6142f0, $L_64c264)
ASM_VAR(const float, k_0_025, $L_6532e8)
ASM_PROC(FUN_61a8a0)

// [FUN_6142f0] constructs a terrain view: its position, the new 116-byte detail object, the grid (FUN_61a8a0) and the view constants
extern "C" BYTE* __fastcall TerrainView_Construct(BYTE* s, void*, int x, int y, int z, int a3, int a4, int a5, int a6, int a7)
{
    F(int, s, 108) = a7;
    F(int, s, 40) = a3;
    F(int, s, 44) = a4;
    F(int, s, 28) = x;
    F(int, s, 32) = y;
    F(int, s, 36) = z;
    BYTE* o = (BYTE*)operator new(116);
    if (o) {
        F(void*, o, 0) = g_vtbl652f3c;
        F(WORD, o, 64) = 0;
        F(int, o, 32) = 0;
        F(int, o, 36) = 0;
        F(int, o, 44) = 0;
        F(WORD, o, 42) = 0;
        F(WORD, o, 40) = 0;
        F(int, o, 8) = 0;
        F(int, o, 60) = 0;
        F(DWORD, o, 48) = timeGetTime();
        for (int i = 68; i <= 100; i += 4) F(int, o, i) = 0;
        F(void*, o, 0) = &g_vtbl6532ec;
        F(int, o, 4) = 0;
        F(int, o, 108) = a5;
        F(int, o, 112) = a6;
    }
    F(BYTE*, s, 0) = o;
    CallThis(FUN_61a8a0, o, s + 28, F(int, s, 40), F(int, s, 44));   // the new object's grid
    for (int i = 80; i <= 100; i += 4) F(int, s, i) = 0;
    F(int, s, 48) = 0;
    F(int, s, 52) = 0;
    F(float, s, 56) = (float)(250.0f / (double)k_zero6142f0);
    F(int, s, 64) = 40;
    F(int, s, 68) = 40;
    F(float, s, 60) = (float)(250.0f / (double)k_zero6142f0);
    F(float, s, 72) = (float)(750.0f * (double)k_0_025);
    F(float, s, 76) = (float)(750.0f * (double)k_0_025);
    return s;
}

#include "vecmath.h"
ASM_PROC(FUN_6157a0)    // a segment test against the terrain

// [FUN_616520] tests the segment a-b against the terrain in 5 m pieces; 1 at the first hit
extern "C" int __fastcall Terrain_SegmentHits(BYTE* s, void*, const float* a, const float* b, int c, int d, int e)
{
    Vec3T v;
    v.x = (float)((double)b[0] - a[0]);
    v.y = (float)((double)b[1] - a[1]);
    v.z = (float)((double)b[2] - a[2]);
    unsigned n = (unsigned)(int)(Vec3_Length((const Vec3*)&v) / 5.0f);
    Vec3T w, nb;
    w.x = (float)((double)b[0] - a[0]);
    w.y = (float)((double)b[1] - a[1]);
    w.z = (float)((double)b[2] - a[2]);
    const Vec3T* dn = (const Vec3T*)Vec3_Normalized((Vec3*)&nb, (const Vec3*)&w);
    float sx = (float)((double)dn->x * 5.0f), sy = (float)((double)dn->y * 5.0f), sz = (float)((double)dn->z * 5.0f);
    Vec3T pa = *(const Vec3T*)a, pb = *(const Vec3T*)a;
    for (unsigned i = 0; i < n; i++) {
        double t = (double)(__int64)i;
        float x = (float)(sx * t), y = (float)(sy * t);
        double z = sz * t;
        pa.x = (float)((double)x + a[0]);
        pa.y = (float)((double)y + a[1]);
        pa.z = (float)(z + a[2]);
        double t1 = (double)(__int64)(i + 1);
        float x1 = (float)(sx * t1), y1 = (float)(sy * t1);
        double z1 = sz * t1;
        pb.x = (float)((double)x1 + a[0]);
        pb.y = (float)((double)y1 + a[1]);
        pb.z = (float)(z1 + a[2]);
        if (CallThis<int>(FUN_6157a0, s, &pa, &pb, c, d, e))
            return 1;
    }
    return CallThis<int>(FUN_6157a0, s, &pb, b, c, d, e) != 0;
}

extern "C" void FUN_619bd0(); KING_ALIAS("_FUN_619bd0", "@RoomObjects_Clear@8")
extern "C" void FUN_6170d0(); KING_ALIAS("_FUN_6170d0", "@Decals_Free@8")

// [FUN_6148a0] frees the terrain's objects: the device object, room objects and five decal sets
extern "C" void __fastcall Terrain_FreeObjects(BYTE* self, int)
{
    void* d = *(void**)self;
    if (d) {
        CallVirt(d, 0, 1);
        *(void**)self = 0;
    }
    BYTE* r = *(BYTE**)(self + 80);
    if (r) {
        CallThis(FUN_619bd0, r);
        operator delete(r);
        *(BYTE**)(self + 80) = 0;
    }
    for (int off = 84; off <= 100; off += 4) {
        BYTE* p = *(BYTE**)(self + off);
        if (p) {
            CallThis(FUN_6170d0, p);
            operator delete(p);
            *(BYTE**)(self + off) = 0;
        }
    }
}

// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_614490(); KING_ALIAS("_FUN_614490", "@Terrain614490_Destroy@8")
extern "C" void FUN_619ae0(); KING_ALIAS("_FUN_619ae0", "@Terrain619ae0_Destroy@8")

static BYTE* Ter_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_614470]
extern "C" BYTE* __fastcall Ter614470_DeletingDestructor(BYTE* s, void*, int flags) { return Ter_DeleteAfter(s, flags, FUN_614490); }
// [$L_619ac0]
extern "C" BYTE* __fastcall Ter619ac0_DeletingDestructor(BYTE* s, void*, int flags) { return Ter_DeleteAfter(s, flags, FUN_619ae0); }

// static initializers of constants
ASM_VAR(DWORD, g_ter728810, $L_728810)
ASM_VAR(DWORD, g_ter728814, $L_728814)
ASM_VAR(DWORD, g_ter728818, $L_728818)
ASM_VAR(DWORD, g_ter728820, $L_728820)
ASM_VAR(DWORD, g_ter728824, $L_728824)
ASM_VAR(DWORD, g_ter728828, $L_728828)
ASM_VAR(DWORD, g_ter72c078, $L_72c078)
ASM_VAR(DWORD, g_ter72c07c, $L_72c07c)
ASM_VAR(DWORD, g_ter72c080, $L_72c080)
ASM_VAR(DWORD, g_ter72c088, $L_72c088)
ASM_VAR(DWORD, g_ter72c08c, $L_72c08c)
ASM_VAR(DWORD, g_ter72c090, $L_72c090)
ASM_VAR(DWORD, g_ter72c098, $L_72c098)
ASM_VAR(DWORD, g_ter72c09c, $L_72c09c)
ASM_VAR(DWORD, g_ter72c0a0, $L_72c0a0)
ASM_VAR(DWORD, g_ter72c0b0, $L_72c0b0)
ASM_VAR(DWORD, g_ter72c0b4, $L_72c0b4)
ASM_VAR(DWORD, g_ter72c0b8, $L_72c0b8)
ASM_VAR(DWORD, g_ter72c0d0, $L_72c0d0)
ASM_VAR(DWORD, g_ter72c0d4, $L_72c0d4)
ASM_VAR(DWORD, g_ter72c0d8, $L_72c0d8)
ASM_VAR(float, g_ter64c250, $L_64c250)
ASM_VAR(float, g_ter64c258, $L_64c258)
ASM_VAR(float, g_ter64c450, $L_64c450)
ASM_VAR(float, g_ter64c664, $L_64c664)
ASM_VAR(float, g_ter64d4e8, $L_64d4e8)
ASM_VAR(float, g_ter653238, $L_653238)
ASM_VAR(float, g_ter65323c, $L_65323c)
ASM_VAR(float, g_ter6532c8, $L_6532c8)
ASM_VAR(float, g_ter6532d0, $L_6532d0)
ASM_VAR(float, g_ter6532d8, $L_6532d8)
ASM_VAR(float, g_ter653358, $L_653358)
ASM_VAR(float, g_ter653370, $L_653370)
ASM_VAR(float, g_ter653374, $L_653374)
ASM_VAR(float, g_ter653378, $L_653378)
ASM_VAR(float, g_ter65337c, $L_65337c)
ASM_VAR(float, g_ter653380, $L_653380)
ASM_VAR(float, g_ter728808, $L_728808)
ASM_VAR(float, g_ter72882c, $L_72882c)
ASM_VAR(float, g_ter728830, $L_728830)
ASM_VAR(float, g_ter728838, $L_728838)
ASM_VAR(float, g_ter72c0a4, $L_72c0a4)
ASM_VAR(float, g_ter72c0a8, $L_72c0a8)
ASM_VAR(float, g_ter72c0ac, $L_72c0ac)
ASM_VAR(float, g_ter72c0c8, $L_72c0c8)
ASM_VAR(float, g_ter72c0cc, $L_72c0cc)
// [$L_616c80]
extern "C" void __cdecl TerInit_616c80() {  }
// [$L_614180]
extern "C" void __cdecl TerInit_614180() { g_ter72882c = (float)((double)g_ter653238 + (double)g_ter653238); }
// [$L_6141d0]
extern "C" void __cdecl TerInit_6141d0() { g_ter72c088 = 0x0; g_ter72c08c = 0x0; g_ter72c090 = 0x0; }
// [$L_614200]
extern "C" void __cdecl TerInit_614200() { g_ter728838 = (float)((double)g_ter6532c8 + g_ter64c450); }
// [$L_614230]
extern "C" void __cdecl TerInit_614230() { g_ter728808 = (float)((double)g_ter728838 + g_ter64c664); }
// [$L_615740]
extern "C" void __cdecl TerInit_615740() { g_ter728820 = 0x3dcccccd; g_ter728824 = 0x0; g_ter728828 = 0x0; }
// [$L_615770]
extern "C" void __cdecl TerInit_615770() { g_ter728810 = 0x0; g_ter728814 = 0x3dcccccd; g_ter728818 = 0x0; }
// [$L_61a760]
extern "C" void __cdecl TerInit_61a760() { g_ter72c0a4 = (float)((double)g_ter64c250 - g_ter653370); }
// [$L_61a790]
extern "C" void __cdecl TerInit_61a790() { g_ter72c0ac = (float)((double)g_ter653378 - g_ter653374); }
// [$L_61a7c0]
extern "C" void __cdecl TerInit_61a7c0() { g_ter72c0b0 = 0x0; g_ter72c0b4 = 0x0; g_ter72c0b8 = 0x3f800000; }
// [$L_61a7f0]
extern "C" void __cdecl TerInit_61a7f0() { g_ter72c098 = 0x0; g_ter72c09c = 0x0; g_ter72c0a0 = 0x3f800000; }
// [$L_61a820]
extern "C" void __cdecl TerInit_61a820() { g_ter72c0c8 = (float)((double)g_ter65337c + (double)g_ter65337c); }
// [$L_61a870]
extern "C" void __cdecl TerInit_61a870() { g_ter72c0d0 = 0x0; g_ter72c0d4 = 0x0; g_ter72c0d8 = 0x0; }
// [$L_6141a0]
extern "C" void __cdecl TerInit_6141a0() { g_ter728830 = (float)((double)g_ter65323c - g_ter64d4e8 - g_ter64c258); }
// [$L_61a740]
extern "C" void __cdecl TerInit_61a740() { g_ter72c0a8 = (float)((double)g_ter653358 + (double)g_ter653358); }
// [$L_614290]
extern "C" void __cdecl TerInit_614290() { g_ter72c078 = 0x0; g_ter72c07c = 0x0; g_ter72c080 = 0x3f800000; }
// [$L_61a840]
extern "C" void __cdecl TerInit_61a840() { g_ter72c0cc = (float)((double)g_ter653380 - g_ter64d4e8 - g_ter64c258); }
#include "x87.h"
ASM_VAR(float, g_ter72880c, $L_72880c)
ASM_VAR(float, g_ter728834, $L_728834)
ASM_VAR(double, g_ter6532e0, $L_6532e0)

// [$L_6142c0]
extern "C" void __cdecl TerInit_6142c0() { g_ter72880c = (float)-((double)*(float*)&g_ter72c080 * g_ter6532e0); }
// [$L_614260]
extern "C" void __cdecl TerInit_614260() { g_ter728834 = (float)x87_cos((double)g_ter6532d0 * g_ter6532d8); }


// === the terrain view update (FUN_614960)

extern "C" void FUN_614ce0(); KING_ALIAS("_FUN_614ce0", "@TGrid_FollowCamera@20")
extern "C" void FUN_60cae0(); KING_ALIAS("_FUN_60cae0", "@Grid_DrainQueue@8")
extern "C" void FUN_60c6b0(); KING_ALIAS("_FUN_60c6b0", "@GridMesh_Reprioritise@8")
ASM_VAR(float, g_tv6532cc, $L_6532cc)
ASM_VAR(float, g_tv64f3f8, $L_64f3f8)
ASM_VAR(float, g_tv64da10, $L_64da10)
ASM_VAR(float, g_tv653138, $L_653138)
ASM_VAR(float, g_tv6532f4, $L_6532f4)

extern "C" {

// [FUN_614960] view s moves to pos looking along dir with up: a small move that keeps the angle refocuses the
// grid (s+0) at once; otherwise the grid's queue is drained and it refocuses (looking along +x or -y near the two
// fixed points of the aa map) and is reprioritised; the position (+4) and the unit direction (+16) are kept
float* __fastcall TerrainView_Move(BYTE* s, void*, const float* pos, const float* dir, const float* up)
{
    double dx = (double)pos[0] - F(float, s, 4), dy = (double)pos[1] - F(float, s, 8),
           dz = (double)pos[2] - F(float, s, 12);
    double dl = x87_sqrt((double)dir[0] * dir[0] + (double)dir[1] * dir[1] + (double)dir[2] * dir[2]);
    float c = (float)(((double)F(float, s, 24) * dir[2] + (double)F(float, s, 20) * dir[1] +
                       (double)F(float, s, 16) * dir[0]) / dl);
    double l = x87_sqrt(dz * dz + dy * dy + dx * dx);
    BYTE* m = F(BYTE*, s, 0);
    const float* look = dir;
    static const float px[3] = {1.0f, 0, 0}, ny[3] = {0, -1.0f, 0};
    bool quick = l < g_tv6532cc && (c > g_ter728834 || -(double)g_ter728834 > c);
    if (!quick) {
        CallThis(FUN_60cae0, m);
        if (!F(int, g_6cec6c, 1316)) {
            double x1 = (double)pos[0] - g_tv64da10, y1 = (double)pos[1] + g_tv64f3f8;
            double d1 = x87_sqrt(x1 * x1 + y1 * y1);
            double x2 = (double)pos[0] - g_tv6532f4, y2 = (double)pos[1] + g_tv653138;
            double d2 = x87_sqrt(x2 * x2 + y2 * y2);
            if (d2 < g_ter64c450)
                look = px;
            else if (d1 < g_ter64c450)
                look = ny;
        }
    }
    memcpy(m + 68, pos, 12);
    memcpy(m + 80, look, 12);
    memcpy(m + 92, up, 12);
    CallThis(FUN_614ce0, F(BYTE*, m, 4), m + 68, m + 80, m + 92);
    F(int, m, 28) = 1;
    F(int, m, 24) = 1;
    if (quick) {
        F(int, F(BYTE*, s, 0), 24) = 1;
        F(int, F(BYTE*, s, 0), 28) = 1;
    } else
        CallThis(FUN_60c6b0, F(BYTE*, s, 0));
    memcpy(s + 4, pos, 12);
    float* u = (float*)(s + 16);
    double n = x87_sqrt((double)dir[0] * dir[0] + (double)dir[1] * dir[1] + (double)dir[2] * dir[2]);
    u[0] = (float)(dir[0] / n);
    u[1] = (float)(dir[1] / n);
    u[2] = (float)(dir[2] / n);
    return u;
}

} // extern "C"

// === the terrain's decal sets (FUN_614570)

ASM_VAR(AsmLabel, s_dcAa, $L_676838)            // "aa"
ASM_VAR(AsmLabel, s_dcTr, $L_676834)            // "tr"
ASM_VAR(AsmLabel, s_dcKust, $L_685da4)
ASM_VAR(AsmLabel, s_dcTree23, $L_685d9c)
ASM_VAR(AsmLabel, s_dcTree45, $L_685d94)
ASM_VAR(AsmLabel, s_dcAa2, $L_685d90)
ASM_VAR(AsmLabel, s_dcAa3, $L_685d8c)

// a decal set of n named name in directory dir
static BYTE* Dc_New(int owner, int n, const void* name, const void* dir)
{
    BYTE* d = (BYTE*)operator new(32);
    return d ? Decals_Construct(d, 0, owner, n, (int)name, (int)dir) : 0;
}

extern "C" {

// [FUN_614570] terrain s's room objects (+80) and, once, its decal sets for owner a: bushes (+84, as many as the
// cells +48 x +52), trees 2-3 and 4-5 and the two aa sets (+88..+100, cells +64 x +68), from the aa or tr map
void __fastcall Terrain_CreateDecals(BYTE* s, void*, int a)
{
    if (!F(BYTE*, s, 80)) {
        BYTE* o = (BYTE*)operator new(52);
        F(BYTE*, s, 80) = o ? RoomObjects_Construct(o, 0, a, (int)s) : 0;
    }
    if (F(int, s, 84) || F(int, s, 88) || F(int, s, 92) || F(int, s, 96) || F(int, s, 100))
        return;
    const void* dir = F(int, g_6cec6c, 1316) ? (BYTE*)s_dcTr : (BYTE*)s_dcAa;
    F(BYTE*, s, 84) = Dc_New(a, F(int, s, 48) * F(int, s, 52), (BYTE*)s_dcKust, dir);
    F(BYTE*, s, 88) = Dc_New(a, F(int, s, 64) * F(int, s, 68), (BYTE*)s_dcTree23, dir);
    F(BYTE*, s, 92) = Dc_New(a, F(int, s, 64) * F(int, s, 68), (BYTE*)s_dcTree45, dir);
    F(BYTE*, s, 96) = Dc_New(a, F(int, s, 64) * F(int, s, 68), (BYTE*)s_dcAa2, dir);
    F(BYTE*, s, 100) = Dc_New(a, F(int, s, 64) * F(int, s, 68), (BYTE*)s_dcAa3, dir);
}

} // extern "C"

// === the terrain normal at a point (FUN_615350)

extern "C" void FUN_608ab0(); KING_ALIAS("_FUN_608ab0", "@Tiles_Table@16")
extern "C" void FUN_608ea0(); KING_ALIAS("_FUN_608ea0", "@Tile_Normal@20")
extern "C" void Tn_floor(); KING_ALIAS("_Tn_floor", "_floor")
extern "C" void Tn_ceil(); KING_ALIAS("_Tn_ceil", "_ceil")
extern "C" Vec3* __cdecl Vec3_Diff(Vec3* out, const Vec3* a, const Vec3* b);
extern "C" Vec3* __cdecl Vec3_Sum(Vec3* out, const Vec3* a, const Vec3* b);
extern "C" Vec3* __cdecl Vec3_Times(Vec3* out, const Vec3* v, float s);
ASM_VAR(float, g_tn653240, $L_653240)
ASM_VAR(float, g_tn653244, $L_653244)

// the normal of grid point (i, j)
static Vec3 Tn_At(BYTE* tiles, int i, int j)
{
    Vec3 n;
    CallThis(FUN_608ea0, CallThis<BYTE*>(FUN_608ab0, tiles, i, j), i, j, &n);
    return n;
}

extern "C" {

// [FUN_615350] *out = the terrain normal at (x, y) of view s's grid (+0 +4: its bounds +40136.., cell +40152..,
// tiles +40192), blended from the four grid points around it; straight up outside
Vec3* __fastcall Terrain_NormalAt(BYTE** s, void*, float x, float y, Vec3* out)
{
    BYTE* g = F(BYTE*, *s, 4);
    if (!(x < F(float, g, 40140) && x > F(float, g, 40136) && y < F(float, g, 40144) && y > F(float, g, 40148))) {
        out->x = 0;
        out->y = 0;
        out->z = 1.0f;
        return out;
    }
    float u = (float)(((double)x - F(float, g, 40136)) / F(float, g, 40152));
    float v = (float)(((double)y - F(float, g, 40148)) / F(float, g, 40156));
    int i0 = (int)(CallC<double>(Tn_floor, (double)(u / (double)g_tn653240)) * g_tn653240);
    int j0 = (int)(CallC<double>(Tn_floor, (double)(v / (double)g_tn653244)) * g_tn653244);
    int i1 = (int)(CallC<double>(Tn_ceil, (double)(u / (double)g_tn653240)) * g_tn653240);
    int j1 = (int)(CallC<double>(Tn_ceil, (double)(v / (double)g_tn653244)) * g_tn653244);
    float fu = (float)((u - (double)(__int64)i0) / g_tn653240);
    float fv = (float)((v - (double)(__int64)j0) / g_tn653244);
    BYTE* t = F(BYTE*, g, 40192);
    Vec3 n00 = Tn_At(t, i0, j0), n10 = Tn_At(t, i1, j0), n01 = Tn_At(t, i0, j1), n11 = Tn_At(t, i1, j1);
    Vec3 a, b, c, d, e, f, h, r;
    a.Set((float)((double)n10.x - n00.x), (float)((double)n10.y - n00.y), (float)((double)n10.z - n00.z));
    b.Set((float)(a.x * (double)fu), (float)(a.y * (double)fu), (float)(a.z * (double)fu));
    c.Set((float)((double)b.x + n00.x), (float)((double)b.y + n00.y), (float)((double)b.z + n00.z));
    Vec3_Sum(&f, &n01, Vec3_Times(&e, Vec3_Diff(&d, &n11, &n01), fu));
    Vec3_Sum(&r, &c, Vec3_Times(&h, Vec3_Diff(&d, &f, &c), fv));
    *out = r;
    return out;
}

} // extern "C"

// === seeding the noise tables (FUN_616ca0)

extern "C" int __cdecl rand(void);
extern "C" void __cdecl srand(unsigned);
ASM_VAR(float, g_ns64c254, $L_64c254)
ASM_VAR(float, g_ns64c340, $L_64c340)
ASM_VAR(float, g_ns64c334, $L_64c334)
ASM_VAR(float, g_ns64c264, $L_64c264)
ASM_VAR(float, g_ns64c258, $L_64c258)
ASM_VAR(float, g_ns64c250, $L_64c250)
ASM_VAR(DWORD, g_ns729448, $L_729448)
ASM_VAR(DWORD, g_ns728c40, $L_728c40)

// a random number in [0, 1)
static double Ns_Rand() { return rand() * (double)g_ns64c254; }

extern "C" {

// [FUN_616ca0] seeds the noise tables from seed: 256 random unit gradients (729850), signs (72b870), kinds 0..3
// (729044) and scales (72883c), a shuffled permutation (72b068), each table repeated after its 256 entries
void __cdecl Noise_Seed(unsigned seed)
{
    srand(seed);
    float* grad = (float*)g_noiseGrad;
    for (int i = 0; i < 256; i++) {
        double z, l2;
        float x, y;
        do {
            float r1 = (float)Ns_Rand(), r2 = (float)Ns_Rand(), r3 = (float)Ns_Rand(), r4 = (float)Ns_Rand();
            double r5 = Ns_Rand();
            x = (float)(r4 - r5);
            y = (float)((double)r2 - r3);
            z = Ns_Rand() - r1;
            l2 = z * z + (double)x * x + (double)y * y;
        } while (l2 > g_ns64c250);
        double l = x87_sqrt(l2);
        grad[i * 3] = (float)(x / l);
        grad[i * 3 + 1] = (float)(y / l);
        grad[i * 3 + 2] = (float)(z / l);
        g_noiseC[i] = Ns_Rand() > g_ns64c264 ? 1 : 0;
        if (Ns_Rand() > g_ns64c258)
            g_noiseA[i] = Ns_Rand() > g_ns64c258 ? 0 : 1;
        else
            g_noiseA[i] = Ns_Rand() > g_ns64c258 ? 2 : 3;
        ((float*)g_noiseB)[i] = (float)(rand() * (double)g_ns64c340 + g_ns64c334);
    }
    for (int j = 0; j < 256; j++)
        g_perm[j] = j;
    for (int* p = g_perm + 256; p > g_perm; p -= 2) {
        int t = *p;
        int k = rand() % 256;
        *p = g_perm[k];
        g_perm[k] = t;
    }
    for (int i = 0; i < 256; i++) {
        g_perm[256 + i] = g_perm[i];
        memcpy(grad + (256 + i) * 3, grad + i * 3, 12);
        g_noiseC[256 + i] = g_noiseC[i];
        g_ns729448 = g_noiseA[i];
        g_ns728c40 = g_noiseB[i];
    }
}

} // extern "C"

// === placing the terrain view (FUN_614f70)

extern "C" void FUN_60cb10(); KING_ALIAS("_FUN_60cb10", "@Grid_RemoveUnusedPoints@8")

// grid m refocused at pos looking along look with up
static void Tp_Focus(BYTE* m, const float* pos, const float* look, const float* up)
{
    memcpy(m + 68, pos, 12);
    memcpy(m + 80, look, 12);
    memcpy(m + 92, up, 12);
    CallThis(FUN_614ce0, F(BYTE*, m, 4), m + 68, m + 80, m + 92);
    F(int, m, 28) = 1;
    F(int, m, 24) = 1;
}

extern "C" {

// [FUN_614f70] places view s's grid at pos: inside its bounds (+28, +32, size +40, +44) it looks along dir, else
// towards the nearer of the two fixed points of the map (+x, -y, or both when as near); then the grid is
// reprioritised and its unused points dropped
void __fastcall TerrainView_Place(BYTE* s, void*, const float* pos, const float* dir, const float* up)
{
    CallThis(FUN_60cae0, F(BYTE*, s, 0));
    double h = F(float, s, 40) * (double)g_ns64c258;
    float hf = (float)h;
    if (!(h + F(float, s, 28) < pos[0]) && !((double)F(float, s, 28) - hf > pos[0]) &&
        !(F(float, s, 44) * (double)g_ns64c258 + F(float, s, 32) < pos[1]) && !((double)F(float, s, 32) - hf > pos[1]))
        Tp_Focus(F(BYTE*, s, 0), pos, dir, up);
    else {
        double ay = (double)pos[1] + g_tv653138, ax = (double)pos[0] - g_tv6532f4;
        float da = (float)x87_sqrt(ay * ay + ax * ax);
        double bx = (double)pos[0] - g_tv64da10, by = (double)pos[1] + g_tv64f3f8;
        float db = (float)x87_sqrt(bx * bx + by * by);
        static const float px[3] = {1.0f, 0, 0}, ny[3] = {0, -1.0f, 0}, both[3] = {1.0f, -1.0f, 0};
        if (da < db)
            Tp_Focus(F(BYTE*, s, 0), pos, px, up);
        if (da > db)
            Tp_Focus(F(BYTE*, s, 0), pos, ny, up);
        if (da == db)
            Tp_Focus(F(BYTE*, s, 0), pos, both, up);
    }
    CallThis(FUN_60c6b0, F(BYTE*, s, 0));
    CallThis(FUN_60cb10, F(BYTE*, s, 0));
}

} // extern "C"
