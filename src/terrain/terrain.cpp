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
extern "C" void FUN_619c20(); KING_ALIAS("_FUN_619c20", "@Terrain_PlacePlants@12")
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
extern "C" void FUN_617180(); KING_ALIAS("_FUN_617180", "@Patch_AddPlant@28")

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
extern "C" void FUN_61a8a0(); KING_ALIAS("_FUN_61a8a0", "@TGridMesh_Build@20")

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
extern "C" void FUN_6157a0(); KING_ALIAS("_FUN_6157a0", "@Ground_SweepSphere@28")    // a segment test against the terrain

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

// === a moving sphere against the ground (FUN_6157a0)

extern "C" void Tg_Height(); KING_ALIAS("_Tg_Height", "@Grid_HeightAt@12")
extern "C" Vec3* __fastcall Vec3_SetThis(Vec3* self, void*, float x, float y, float z);
extern "C" void Tg_LengthSq(); KING_ALIAS("_Tg_LengthSq", "_Vec3_LengthSq")
extern "C" void Tg_DivBy(); KING_ALIAS("_Tg_DivBy", "_Vec3_DivBy")
extern "C" void Tg_Scaled(); KING_ALIAS("_Tg_Scaled", "_Vec3_Scaled")
extern "C" void Tg_Sub(); KING_ALIAS("_Tg_Sub", "_Vec3_Sub")
ASM_VAR(float, g_tg728810, $L_728810)
ASM_VAR(float, g_tg728814, $L_728814)
ASM_VAR(float, g_tg728818, $L_728818)
ASM_VAR(float, g_tg728820, $L_728820)
ASM_VAR(float, g_tg728824, $L_728824)
ASM_VAR(float, g_tg728828, $L_728828)
ASM_VAR(float, g_tg64c258, $L_64c258)
ASM_VAR(float, g_tg64c250, $L_64c250)
ASM_VAR(float, g_tg64c264, $L_64c264)
ASM_VAR(double, g_tg653300, $L_653300)
ASM_VAR(double, g_tg64b930, $L_64b930)

static float Tg_At(BYTE* s, const Vec3* p)
{
    return CallThis<float>(Tg_Height, F(BYTE*, F(BYTE*, s, 0), 4), p);
}

extern "C" {

// [FUN_6157a0] whether a sphere of radius r moving from a to b meets the ground plane fitted through the heights
// at the move's middle and four points around it (728810 / 728820 either way): the touching point to *at
// (moved back along the normal by r) and the plane normal to *n, 1; 0 when it doesn't reach it or starts behind
int __fastcall Ground_SweepSphere(BYTE* s, void*, const float* a, const float* b, float r, Vec3* at, Vec3* n)
{
    Vec3 A = {a[0], a[1], a[2]}, B = {b[0], b[1], b[2]}, sum, M;
    Vec3_SetThis(&sum, 0, B.x + A.x, B.y + A.y, B.z + A.z);
    Vec3_SetThis(&M, 0, sum.x * g_tg64c258, sum.y * g_tg64c258, sum.z * g_tg64c258);
    float ha = Tg_At(s, &A);
    float hb = Tg_At(s, &B);
    M.z = (hb + ha) * g_tg64c258;
    Vec3 p1, p2, p3, p4;
    Vec3_SetThis(&p1, 0, M.x - g_tg728820, M.y - g_tg728824, M.z - g_tg728828);
    Vec3_SetThis(&p2, 0, g_tg728810 + M.x, M.y + g_tg728814, g_tg728818 + M.z);
    Vec3_SetThis(&p3, 0, g_tg728820 + M.x, M.y + g_tg728824, g_tg728828 + M.z);
    Vec3_SetThis(&p4, 0, M.x - g_tg728810, M.y - g_tg728814, M.z - g_tg728818);
    p1.z = Tg_At(s, &p1);
    p2.z = Tg_At(s, &p2);
    p3.z = Tg_At(s, &p3);
    p4.z = Tg_At(s, &p4);
    Vec3 e1, e2;
    Vec3_SetThis(&e1, 0, p1.x - p3.x, p1.y - p3.y, p1.z - p3.z);
    Vec3_SetThis(&e2, 0, p2.x - p4.x, p2.y - p4.y, p2.z - p4.z);
    Vec3 nr = {e1.z * e2.y - e1.y * e2.z, e1.x * e2.z - e2.x * e1.z, e2.x * e1.y - e1.x * e2.y}, nn;
    float len = (float)x87_sqrt(CallC<double>(Tg_LengthSq, &nr));
    CallC(Tg_DivBy, &nn, &nr, *(DWORD*)&len);
    double d = -((double)M.y * nn.y + (double)M.x * nn.x + (double)nn.z * M.z);
    float da = (float)((double)nn.x * A.x + (double)nn.y * A.y + (double)nn.z * A.z + d);
    float db = (float)((double)B.z * nn.z + (double)B.y * nn.y + (double)B.x * nn.x + d);
    if (da < g_tg653300)
        return 0;
    double re = r + g_tg64b930;
    Vec3 t1, t2;
    if (!(x87_fabs(da) > re)) {
        *at = A;
        *n = nn;
        Vec3_SetThis(&t1, 0, r * n->x, r * n->y, r * n->z);
        Vec3_SetThis(&t2, 0, at->x - t1.x, at->y - t1.y, at->z - t1.z);
        *at = t2;
        return 1;
    }
    if (!(da > re) || !(r - g_tg64b930 > db))
        return 0;
    float t = (da - r) / (da - db);
    if (t > g_tg64c250)
        t = 1.0f;
    else if (t < g_tg64c264)
        t = 0;
    Vec3 bt, at2, w;
    Vec3_SetThis(&bt, 0, B.x * t, B.y * t, B.z * t);
    Vec3_Sum(&w, Vec3_Times(&at2, &A, g_tg64c250 - t), &bt);
    Vec3_SetThis(&t1, 0, w.x, w.y, w.z);
    *at = t1;
    Vec3_SetThis(&t1, 0, nn.x, nn.y, nn.z);
    *n = t1;
    CallC(Tg_Sub, &at2, at, CallC<Vec3*>(Tg_Scaled, &t1, n, *(DWORD*)&r));
    *at = at2;
    return 1;
}

} // extern "C"

// === reading three terrain tiles and their normals (FUN_613b50)

extern "C" void Tn_TileHeight(); KING_ALIAS("_Tn_TileHeight", "@Tile_Height@16")
extern "C" int __cdecl rand(void);
ASM_VAR(AsmLabel, s_tn6724b0, $L_6724b0)
ASM_VAR(AsmLabel, s_tn685ce4, $L_685ce4)
ASM_VAR(AsmLabel, s_tn6724f8, $L_6724f8)
ASM_VAR(float, g_tn653234, $L_653234)
ASM_VAR(float, g_tn64c250, $L_64c250)
ASM_VAR(float, g_tn64d690, $L_64d690)
ASM_VAR(float, g_tn64c304, $L_64c304)
ASM_VAR(float, g_tn64c254, $L_64c254)
ASM_VAR(float, g_tn64c258, $L_64c258)

// the height of grid point (i, j) through the tile table of terrain s
static float Tn_HeightAt(BYTE* s, int i, int j)
{
    BYTE* t = CallThis<BYTE*>(FUN_608ab0, s, i, j);
    return F(float, t, (((unsigned)i % 257) * 257 + (unsigned)j % 257) * 16);
}

static float Tn_TileH(BYTE* s, int i, int j)
{
    return CallThis<float>(Tn_TileHeight, CallThis<BYTE*>(FUN_608ab0, s, i, j), i, j);
}

static int Tn_Fail(int line)
{
    CallC(FUN_60aad0, 2289444626u, s_tn6724b0, s_tn685ce4, line, s_tn6724f8);
    return 0;
}

extern "C" {

// [FUN_613b50] reads the heights of terrain s's three tiles from the files f[0..2] (n must be 3) and gives every
// point of the 771 x 771 grid the normal of its neighbours' slopes (+4 of its 16-byte entry); nearly level
// ones get a small random tilt; 0 on a failure (reported)
int __fastcall Terrain_ReadTiles(BYTE* s, void*, FILE** f, int n)
{
    if (n != 3)
        return Tn_Fail(59);
    if (!Tile_ReadHeights(F(BYTE*, s, 0), 0, f[0]))
        return Tn_Fail(64);
    if (!Tile_ReadHeights(F(BYTE*, s, 4), 0, f[1]))
        return Tn_Fail(69);
    if (!Tile_ReadHeights(F(BYTE*, s, 8), 0, f[2]))
        return Tn_Fail(74);
    for (int i = 0; i < 771; i++) {
        float fi = (float)i;
        float xm = (fi - g_tn64c250) * g_tn653234, xp = (fi + g_tn64c250) * g_tn653234, x = fi * g_tn653234;
        for (int j = 0; j < 771; j++) {
            float fj = (float)j;
            float y = fj * g_tn653234, yp = (fj + g_tn64c250) * g_tn653234, ym = (fj - g_tn64c250) * g_tn653234;
            Vec3 A, B, t;
            if (j) {
                float h0 = Tn_HeightAt(s, i, j - 1);
                float h1 = Tn_HeightAt(s, i, j + 1);
                A.x = x - x, A.y = yp - ym, A.z = h1 - h0;
            } else {
                float h0 = Tn_HeightAt(s, i, 0);
                float h1 = Tn_HeightAt(s, i, 1);
                Vec3_SetThis(&A, 0, x - x, yp - ym, h1 - h0);
            }
            if (i) {
                float hn = Tn_HeightAt(s, i + 1, j);
                Vec3_SetThis(&B, 0, xm - xp, y - y, Tn_TileH(s, i - 1, j) - hn);
            } else {
                float h1 = Tn_TileH(s, 1, j);
                Vec3_SetThis(&B, 0, xm - xp, y - y, Tn_TileH(s, 0, j) - h1);
            }
            Vec3 N = {B.z * A.y - B.y * A.z, B.x * A.z - A.x * B.z, A.x * B.y - B.x * A.y};
            float len = (float)x87_sqrt(CallC<double>(Tg_LengthSq, &N));
            Vec3 nn = *CallC<Vec3*>(Tg_DivBy, &t, &N, *(DWORD*)&len);
            if (nn.x > g_tn64d690 && nn.x < g_tn64c304 && nn.y > g_tn64d690 && nn.y < g_tn64c304) {
                float r1 = (float)(rand() * (double)g_tn64c254);
                nn.x = (float)((rand() * (double)g_tn64c254 - r1) * g_tn64c258);
                float r2 = (float)(rand() * (double)g_tn64c254);
                nn.y = (float)((rand() * (double)g_tn64c254 - r2) * g_tn64c258);
                double l = x87_sqrt(CallC<double>(Tg_LengthSq, &nn));
                Vec3_SetThis(&t, 0, (float)(nn.x / l), (float)(nn.y / l), (float)(nn.z / l));
                nn = t;
            }
            BYTE* tile = CallThis<BYTE*>(FUN_608ab0, s, i, j);
            F(Vec3, tile, (((unsigned)i % 257) * 257 + (unsigned)j % 257) * 16 + 4) = nn;
        }
    }
    return 1;
}

} // extern "C"

// === the terrain height at a point (FUN_616850)

ASM_VAR(float, g_th6532f8, $L_6532f8)

static float Th_Sample(BYTE* t, int x, int y) { return Terrain_Sample((BYTE**)t, 0, x, y); }

extern "C" {

// [FUN_616850] the terrain height at (x, y) of view s's grid (+0 +4: bounds +40136.., cell +40152.., tiles
// +40192): the triangle of the grid cell holding it (split along one diagonal or the other by the cell's
// parity) interpolated; 6532f8 outside
double __fastcall Terrain_HeightAt(BYTE** s, void*, float x, float y)
{
    BYTE* g = F(BYTE*, *s, 4);
    if (!(x < F(float, g, 40140) && x > F(float, g, 40136) && y < F(float, g, 40144) && y > F(float, g, 40148)))
        return g_th6532f8;
    float u = (float)(((double)x - F(float, g, 40136)) / F(float, g, 40152));
    float v = (float)(((double)y - F(float, g, 40148)) / F(float, g, 40156));
    int x0 = (int)(CallC<double>(Tn_floor, (double)(u / (double)g_tn653240)) * g_tn653240);
    int y0 = (int)(CallC<double>(Tn_floor, (double)(v / (double)g_tn653244)) * g_tn653244);
    int x1 = (int)(CallC<double>(Tn_ceil, (double)(u / (double)g_tn653240)) * g_tn653240);
    int y1 = (int)(CallC<double>(Tn_ceil, (double)(v / (double)g_tn653244)) * g_tn653244);
    double fud = (u - (double)(__int64)x0) / g_tn653240;
    float fu = (float)fud;
    float fv = (float)((v - (double)(__int64)y0) / g_tn653244);
    int diag, tri;
    if (((y0 & 4) != 0) != ((x0 & 4) != 0))
    {
        diag = 1;
        tri = fud < (double)g_tn64c250 - fv ? 2 : 3;
    }
    else
    {
        diag = 0;
        tri = fud < fv ? 3 : 2;
    }
    BYTE* t = F(BYTE*, g, 40192);
    if (fv == 0)
    {
        float h0 = (float)CallThis<double>(Tn_TileHeight, CallThis<BYTE*>(FUN_608ab0, t, x0, y0), x0, y0);
        return ((double)Th_Sample(t, x1, y0) - h0) * fu + h0;
    }
    if (fv == g_tn64c250)
    {
        float h0 = Th_Sample(t, x0, y1);
        return ((double)Th_Sample(t, x1, y1) - h0) * fu + h0;
    }
    float a, b, q;
    double X, w;
    if (diag == 0)
    {
        a = Th_Sample(t, x0, y0);
        b = Th_Sample(t, x1, y1);
        if (tri == 3)
        {
            q = a;
            X = Th_Sample(t, x0, y1);
            w = (double)fu / fv;
        }
        else
        {
            q = Th_Sample(t, x1, y0);
            X = b;
            w = ((double)fu - fv) / ((double)g_tn64c250 - fv);
        }
    }
    else
    {
        a = Th_Sample(t, x1, y0);
        b = Th_Sample(t, x0, y1);
        if (tri == 3)
        {
            q = a;
            X = Th_Sample(t, x1, y1);
            w = ((double)fv + fu - g_tn64c250) / fv;
        }
        else
        {
            q = Th_Sample(t, x0, y0);
            X = b;
            w = fu / ((double)g_tn64c250 - fv);
        }
    }
    float wf = (float)w;
    float p = (float)(((double)b - a) * fv + a);
    double r = (X - q) * fv + q;
    if ((tri == 2 && diag == 1) || (diag == 0 && tri == 3))
        return ((double)p - r) * wf + r;
    return (r - p) * wf + p;
}

} // extern "C"

// === the grass and trees around the camera (FUN_619c20)

ASM_VAR(float, g_tg6532b8, $L_6532b8)
ASM_VAR(float, g_tg6532bc, $L_6532bc)
ASM_VAR(float, g_tg6532c0, $L_6532c0)
ASM_VAR(float, g_tg6532c4, $L_6532c4)
ASM_VAR(float, g_tg6532c8, $L_6532c8)
ASM_VAR(float, g_tg653354, $L_653354)
ASM_VAR(float, g_tg64c334, $L_64c334)
ASM_VAR(float, g_tg64c258, $L_64c258)
ASM_VAR(float, g_tg64c260, $L_64c260)
ASM_VAR(float, g_tg64c310, $L_64c310)
ASM_VAR(float, g_tg64c350, $L_64c350)
ASM_VAR(float, g_tg64c324, $L_64c324)
ASM_VAR(float, g_tg728838, $L_728838)               // the sea level
ASM_VAR(float, g_tg728808, $L_728808)               // the tree line
ASM_VAR(AsmLabel, g_tgSizes, $L_685d70)             // the plant sizes 685d70 .. 685d84
ASM_VAR(double, g_tg64b938, $L_64b938)
extern "C" void Tg_AddAssign(); KING_ALIAS("_Tg_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Tg_Nop(); KING_ALIAS("_Tg_Nop", "_Fw_Nop")

static float Tg_Size(int k)
{
    return ((const float*)&g_tgSizes)[k];
}

// the grid cell under the point forward of the camera by k
static void Tg_Cell(BYTE* s, const float* fwd, float k, float cx, float cy, int ox, int& ix, int& iy)
{
    BYTE* T = F(BYTE*, s, 0);
    double a = (double)fwd[0] * k;
    float b = (float)((double)fwd[1] * k);
    double gx = a * g_tg64c334 + cx;
    float gy = (float)((double)(float)((double)b * g_tg64c334) + cy);
    ix = (int)CallC<double>(Tn_floor, gx / F(float, T, 56 + ox));
    iy = (int)CallC<double>(Tn_floor, (double)gy / F(float, T, 60 + ox));
}

// a lattice point (ix, iy) jittered by its noise into pos; false when the lattice has nothing there
static bool Tg_Point(BYTE* s, int ix, int iy, int ox, float kx, float ky, float* pos, float* noise, float* scale, int* kind)
{
    if (!Noise_Lattice2(ix, iy, noise, (DWORD*)scale, (DWORD*)kind))
        return false;
    BYTE* T = F(BYTE*, s, 0);
    pos[0] = (float)(ix * (double)F(float, T, 56 + ox) - (double)kx * g_tg64c258);
    pos[1] = (float)(iy * (double)F(float, T, 60 + ox) - (double)ky * g_tg64c258);
    pos[2] = 0;
    float cell = F(float, T, 56 + ox);
    Vec3 off, tmp;
    off.x = (float)(noise[0] * (double)g_tg653354 * cell);
    off.y = (float)((double)(float)(noise[1] * (double)g_tg653354) * cell);
    off.z = (float)((double)(float)(noise[2] * (double)g_tg653354) * cell);
    CallThis(Tg_AddAssign, pos, &tmp, &off);
    return true;
}

// a plant of type t and size k into the patch list l (FUN_617180 on copies of the point and the side)
static void Tg_Plant(BYTE* l, const float* pos, const float* side, float k, int t)
{
    float a[3] = {pos[0], pos[1], pos[2]}, b[3] = {side[0], side[1], side[2]};
    float one = 1.0f;
    DWORD kb, ob;
    memcpy(&kb, &k, 4);
    memcpy(&ob, &one, 4);
    CallThis(FUN_617180, l, a, b, kb, ob, t);
}

extern "C" {

// [FUN_619c20] terrain s's plants around the camera (s +4): the grass lattice (cells +56 / +60, counts +48 / +52)
// below the snow line 6532c8 into patch list l[0], then the trees' lattice (+72 / +76, +64 / +68) above it by the
// noise kind and height into l[1..4], the near ones also made solid tree objects (s +8.., at most 10)
void __fastcall Terrain_PlacePlants(BYTE* s, void*, BYTE** l)
{
    BYTE* g = F(BYTE*, s, 4);
    float cx = (float)F(double, g, 2904), cy = (float)F(double, g, 2912);
    Matrix R;
    const float* M = Matrix_RotationZ(&R, (float)F(double, g, 2928))->m;
    const float* Y = &g_axisY.x;
    float fwd[2];
    fwd[0] = (float)((double)Y[2] * M[6] + (double)Y[1] * M[3] + (double)Y[0] * M[0] + M[9]);
    fwd[1] = (float)((double)Y[2] * M[7] + (double)Y[1] * M[4] + (double)Y[0] * M[1] + M[10]);
    int ix0, iy0;
    Tg_Cell(s, fwd, g_tg6532b8, cx, cy, 0, ix0, iy0);
    float pos[3] = {0, 0, 0}, noise[3] = {0, 0, 0}, scale = 1.0f;
    int kind = 0;
    M = Matrix_RotationZ(&R, (float)F(double, F(BYTE*, s, 4), 2928))->m;
    float f3[3];
    f3[0] = (float)((double)Y[2] * M[6] + (double)Y[1] * M[3] + (double)Y[0] * M[0] + M[9]);
    f3[1] = (float)((double)Y[2] * M[7] + (double)Y[1] * M[4] + (double)Y[0] * M[1] + M[10]);
    f3[2] = (float)((double)Y[2] * M[8] + (double)Y[1] * M[5] + (double)Y[0] * M[2] + M[11]);
    const float* Z = &g_axisZ.x;
    float side[3];
    side[0] = (float)((double)f3[2] * Z[1] - (double)f3[1] * Z[2]);
    side[1] = (float)((double)f3[0] * Z[2] - (double)f3[2] * Z[0]);
    side[2] = (float)((double)f3[1] * Z[0] - (double)f3[0] * Z[1]);
    Terrain_Reset8(l[0], 0);
    BYTE* T = F(BYTE*, s, 0);
    for (int i = 0, ix = ix0; i < F(int, F(BYTE*, s, 0), 48); i++, ix++)
        for (int j = 0, iy = iy0; j < F(int, F(BYTE*, s, 0), 52); j++, iy++)
        {
            if (!Tg_Point(s, ix, iy, 0, g_tg6532b8, g_tg6532bc, pos, noise, &scale, &kind))
                continue;
            if (!Terrain_InView(F(BYTE*, s, 0), 0, pos))
                continue;
            float h = (float)Terrain_HeightAt((BYTE**)F(BYTE*, s, 0), 0, pos[0], pos[1]);
            pos[2] = h;
            if (!(h < g_tg6532c8))
                continue;
            double kx = (double)f3[0] * g_tg64c258;
            float ky = (float)((double)f3[1] * g_tg64c258), kz = (float)((double)f3[2] * g_tg64c258);
            pos[0] = (float)(pos[0] - kx);
            pos[1] = (float)((double)pos[1] - ky);
            pos[2] = (float)((double)h - kz);
            Tg_Plant(l[0], pos, side, (float)((double)Tg_Size(0) * scale), kind & 1);
        }
    CallThis(Tg_Nop, l[0]);
    for (int k = 1; k < 5; k++)
        Terrain_Reset8(l[k], 0);
    Tg_Cell(s, fwd, g_tg6532c0, cx, cy, 16, ix0, iy0);
    for (unsigned k = 0; k < F(unsigned, s, 48); k++)
    {
        if (!F(BYTE*, s, 8 + 4 * k))
            Terrain6199a0_CreateTrees(s, 0);
        F(BYTE*, s, 8 + 4 * k)[40] |= 1;
    }
    F(int, s, 48) = 0;
    T = F(BYTE*, s, 0);
    for (int i = 0, ix = ix0; i < F(int, F(BYTE*, s, 0), 64); i++, ix++)
        for (int j = 0, iy = iy0; j < F(int, F(BYTE*, s, 0), 68); j++, iy++)
        {
            if (!Tg_Point(s, ix, iy, 16, g_tg6532c0, g_tg6532c4, pos, noise, &scale, &kind))
                continue;
            if (!Terrain_InView(F(BYTE*, s, 0), 0, pos))
                continue;
            float h = (float)Terrain_HeightAt((BYTE**)F(BYTE*, s, 0), 0, pos[0], pos[1]);
            pos[2] = h;
            if (!(h > g_tg6532c8))
                continue;
            Vec3 n;
            Terrain_NormalAt((BYTE**)F(BYTE*, s, 0), 0, pos[0], pos[1], &n);
            BYTE* list;
            float size;
            int type;
            if (n.x < g_tg64c260 && n.y < g_tg64c260)
            {
                double kx = (double)f3[0] * g_tg64c310;
                float ky = (float)((double)f3[1] * g_tg64c310), kz = (float)((double)f3[2] * g_tg64c310);
                pos[0] = (float)(pos[0] - kx);
                pos[1] = (float)((double)pos[1] - ky);
                pos[2] = (float)((double)h - kz);
                if (!(pos[2] > g_tg728838))
                    continue;
                if (pos[2] > g_tg728808)
                {
                    if ((unsigned)kind > 3)
                        continue;
                    static const int lists[4] = {4, 1, 2, 3}, sizes[4] = {4, 1, 2, 3}, types[4] = {5, 2, 3, 4};
                    list = l[lists[kind]];
                    size = (float)((double)Tg_Size(sizes[kind]) * scale);
                    type = types[kind];
                }
                else if (kind & 1)
                {
                    list = l[4];
                    size = (float)((double)Tg_Size(4) * scale);
                    type = 5;
                }
                else
                {
                    list = l[4];
                    size = (float)((double)Tg_Size(5) * scale);
                    type = 6;
                }
            }
            else
            {
                if (!(h > g_tg728838))
                    continue;
                list = l[2];
                size = (float)((double)Tg_Size(2) * scale);
                type = 3;
            }
            Tg_Plant(list, pos, side, size, type);
            // a tree close to the camera becomes solid
            if (!(x87_fabs((double)cx - pos[0]) < g_tg64b938) || !(x87_fabs((double)cy - pos[1]) < g_tg64b938))
                continue;
            int t = F(int, s, 48);
            if (t == 10 || !F(BYTE*, s, 8 + 4 * t))
                continue;
            if (!F(int, F(BYTE*, s, 8 + 4 * t), 44))
                RoomObjects_Attach(s, 0);
            BYTE* o = F(BYTE*, s, 8 + 4 * F(int, s, 48));
            o[40] &= 0xfe;
            F(int, o, 52) = 50;
            F(float, F(BYTE*, o, 60), 0) = 10.0f;
            float* q = F(float*, o, 72);
            double ax = (double)Z[0] * g_tg64c350;
            float ay = (float)((double)Z[1] * g_tg64c350), az = (float)((double)Z[2] * g_tg64c350);
            q[0] = (float)(ax + pos[0]);
            q[1] = (float)((double)ay + pos[1]);
            q[2] = (float)((double)az + pos[2]);
            double bx = (double)Z[0] * g_tg64c324;
            float by = (float)((double)Z[1] * g_tg64c324), bz = (float)((double)Z[2] * g_tg64c324);
            q[3] = (float)(bx + pos[0]);
            q[4] = (float)((double)by + pos[1]);
            q[5] = (float)((double)bz + pos[2]);
            F(int, s, 48)++;
        }
    for (int k = 1; k < 5; k++)
        CallThis(Tg_Nop, l[k]);
}

} // extern "C"

// === a plant's quads into a patch list (FUN_617180)

extern "C" Vec3* __cdecl Vec3_Sum(Vec3*, const Vec3*, const Vec3*);
extern "C" Vec3* __cdecl Vec3_Diff(Vec3*, const Vec3*, const Vec3*);
extern "C" Vec3* __cdecl Vec3_Times(Vec3*, const Vec3*, float);
extern "C" Vec3* __cdecl Pp_ScaleBy(Vec3*, float, const Vec3*); KING_ALIAS("_Pp_ScaleBy", "_Vec3_ScaleBy")
extern "C" double __cdecl Fx_Scale75();
extern "C" void Pp_Error(); KING_ALIAS("_Pp_Error", "_Grd_ReportError")
ASM_VAR(float, g_pp64c458, $L_64c458)
ASM_VAR(float, g_pp64c334, $L_64c334)
ASM_VAR(char, g_pp6724f8, $L_6724f8)
ASM_VAR(char, g_pp685dd0, $L_685dd0)
ASM_VAR(char, g_pp6724b0, $L_6724b0)

// the float steps of Vec3_SetThis
static Vec3 Pp_Scale(const Vec3& a, double k)
{
    Vec3 r;
    r.x = (float)(a.x * k);
    r.y = (float)(a.y * k);
    r.z = (float)(a.z * k);
    return r;
}

static Vec3 Pp_Add(const Vec3& a, const Vec3& b)
{
    Vec3 r;
    r.x = (float)((double)a.x + b.x);
    r.y = (float)((double)a.y + b.y);
    r.z = (float)((double)a.z + b.z);
    return r;
}

static Vec3 Pp_Sub(const Vec3& a, const Vec3& b)
{
    Vec3 r;
    r.x = (float)((double)a.x - b.x);
    r.y = (float)((double)a.y - b.y);
    r.z = (float)((double)a.z - b.z);
    return r;
}

struct Pp_Ctx
{
    BYTE* s;
    Vec3 pos, side, up;
    float k;
    DWORD color;
};

static void Pp_Put(Pp_Ctx& c, const Vec3& v, float u, float w)
{
    Decals_Add(c.s, 0, (const DWORD*)&v, &c.color, u, w);
}

// the corner forms of the SetThis cases
static Vec3 Pp_Low(Pp_Ctx& c, const Vec3& d) { return Pp_Sub(c.pos, d); }
static Vec3 Pp_Left(Pp_Ctx& c) { return Pp_Add(Pp_Scale(Pp_Sub(c.up, c.side), c.k), c.pos); }
static Vec3 Pp_Right(Pp_Ctx& c) { return Pp_Add(Pp_Scale(Pp_Add(c.up, c.side), c.k), c.pos); }
static Vec3 Pp_Far(Pp_Ctx& c) { return Pp_Add(Pp_Scale(c.side, c.k), c.pos); }

// the helper forms (Vec3_Times / Diff / ScaleBy / Sum), up doubled by u2
static Vec3 Pp_HLow(Pp_Ctx& c)
{
    Vec3 t, r;
    return *Vec3_Diff(&r, &c.pos, Vec3_Times(&t, &c.side, c.k));
}

static Vec3 Pp_HFar(Pp_Ctx& c)
{
    Vec3 t, r;
    return *Vec3_Sum(&r, &c.pos, Vec3_Times(&t, &c.side, c.k));
}

static Vec3 Pp_HEdge(Pp_Ctx& c, bool plus, bool doubled)
{
    Vec3 u2, e, f, r;
    const Vec3* up = &c.up;
    if (doubled)
        up = Pp_ScaleBy(&u2, 2.0f, &c.up);
    if (plus)
        Vec3_Sum(&e, up, &c.side);
    else
        Vec3_Diff(&e, up, &c.side);
    return *Vec3_Sum(&r, &c.pos, Pp_ScaleBy(&f, c.k, &e));
}

extern "C" {

// [FUN_617180] patch list s gets a plant of type t (0-8) at pos facing along side, size k, brightness b: two or
// four triangles (corners and texture coordinates by type, mirrored by the grid parity of the point), tinted by
// three random shades
void __fastcall Patch_AddPlant(BYTE* s, void*, const Vec3* pos, const Vec3* side, float k, float b, int t)
{
    Pp_Ctx c;
    c.s = s;
    c.side = *side;
    c.up.x = 0;
    c.up.y = 0;
    c.up.z = 1.0f;
    c.pos = *pos;
    c.k = k;
    float a1 = (float)Fx_Scale75();
    float a2 = (float)Fx_Scale75();
    int a3 = (int)(Fx_Scale75() * g_pp64c458);
    DWORD col = (DWORD)(int)(b * (double)g_pp64c458);
    col = (col << 8) | (DWORD)(int)(a1 * (double)g_pp64c458);
    col = (col << 8) | (DWORD)(int)(a2 * (double)g_pp64c458);
    c.color = (col << 8) | (DWORD)a3;
    bool odd = (((unsigned)(int)((double)pos->y + pos->x) >> 3) & 1) != 0;
    switch (t)
    {
    case 0:
    case 1:
    {
        Vec3 U = Pp_Scale(c.up, g_pp64c334);
        float w0 = t == 0 ? 0.0f : 1.0f;
        Pp_Put(c, Pp_Low(c, U), w0, w0);
        if (odd)
        {
            Pp_Put(c, Pp_Left(c), 1.0f, 0);
            Pp_Put(c, Pp_Right(c), 0, 1.0f);
        }
        else
        {
            Pp_Put(c, Pp_Left(c), 0, 1.0f);
            Pp_Put(c, Pp_Right(c), 1.0f, 0);
        }
        return;
    }
    case 2:
    case 3:
    case 4:
    case 5:
    {
        // two triangles on a quad standing on the point (5: half height texture)
        float h = t == 5 ? 0.5f : 1.0f;
        Vec3 D = Pp_Scale(c.side, c.k);
        if (odd)
        {
            Pp_Put(c, Pp_Low(c, D), 0, 1.0f);
            Pp_Put(c, Pp_Left(c), 0, 0);
            Pp_Put(c, Pp_Right(c), h, 0);
            Pp_Put(c, Pp_Low(c, Pp_Scale(c.side, c.k)), 0, 1.0f);
            Pp_Put(c, Pp_Right(c), h, 0);
            Pp_Put(c, Pp_Far(c), h, 1.0f);
        }
        else
        {
            Pp_Put(c, Pp_Low(c, D), h, 1.0f);
            Pp_Put(c, Pp_Left(c), h, 0);
            Pp_Put(c, Pp_Right(c), 0, 0);
            Pp_Put(c, Pp_Low(c, Pp_Scale(c.side, c.k)), h, 1.0f);
            Pp_Put(c, Pp_Right(c), 0, 0);
            Pp_Put(c, Pp_Far(c), 0, 1.0f);
        }
        return;
    }
    case 6:
        if (odd)
        {
            Pp_Put(c, Pp_Low(c, Pp_Scale(c.side, c.k)), 0.5f, 1.0f);
            Pp_Put(c, Pp_Left(c), 0.5f, 0);
            Pp_Put(c, Pp_Right(c), 1.0f, 0);
            Pp_Put(c, Pp_Low(c, Pp_Scale(c.side, c.k)), 0.5f, 1.0f);
            Pp_Put(c, Pp_Right(c), 1.0f, 0);
            Pp_Put(c, Pp_HFar(c), 1.0f, 1.0f);
        }
        else
        {
            Pp_Put(c, Pp_HLow(c), 1.0f, 1.0f);
            Pp_Put(c, Pp_HEdge(c, false, false), 1.0f, 0);
            Pp_Put(c, Pp_HEdge(c, true, false), 0.5f, 0);
            Pp_Put(c, Pp_HLow(c), 1.0f, 1.0f);
            Pp_Put(c, Pp_HEdge(c, true, false), 0.5f, 0);
            Pp_Put(c, Pp_HFar(c), 0.5f, 1.0f);
        }
        return;
    case 7:
    case 8:
    {
        // tall plants: twice the height, the texture's band by type and parity
        float lo = t == 7 ? 0.5f : 0.0f, hi = t == 7 ? 0.95f : 0.45f;
        float a = odd ? lo : hi, z = odd ? hi : lo;
        Pp_Put(c, Pp_HLow(c), a, 1.0f);
        Pp_Put(c, Pp_HEdge(c, false, true), a, 0);
        Pp_Put(c, Pp_HEdge(c, true, true), z, 0);
        Pp_Put(c, Pp_HLow(c), a, 1.0f);
        Pp_Put(c, Pp_HEdge(c, true, true), z, 0);
        Pp_Put(c, Pp_HFar(c), z, 1.0f);
        return;
    }
    default:
        CallC(Pp_Error, 2289444626u, &g_pp6724b0, &g_pp685dd0, 721, &g_pp6724f8);
        return;
    }
}

} // extern "C"

// === the grid's height at a point (FUN_615da0)

#include <math.h>

extern "C" BYTE* __fastcall Gh_Table(BYTE** s, void*, DWORD x, DWORD y); KING_ALIAS("@Gh_Table@16", "@Tiles_Table@16")
extern "C" float __fastcall Gh_Height(BYTE* s, void*, DWORD x, DWORD y); KING_ALIAS("@Gh_Height@16", "@Tile_Height@16")
ASM_VAR(float, g_gh653240, $L_653240)                   // the cell sizes
ASM_VAR(float, g_gh653244, $L_653244)
ASM_VAR(float, g_gh64c250, $L_64c250)                   // 1
ASM_VAR(float, g_gh64c264, $L_64c264)                   // 0
ASM_VAR(float, g_gh6532f8, $L_6532f8)                   // off the grid

// the height of grid point (x, y) of tile set t
static float Gh_At(BYTE** t, DWORD x, DWORD y)
{
    return Gh_Height(Gh_Table(t, 0, x, y), 0, x, y);
}

extern "C" {

// [FUN_615da0] grid s's height at p (x / y within +40136..+40140 / +40148..+40144, cell sizes +40152 / +40156,
// tiles +40192): on a cell's edge between its two corners, else on the cell's triangle (split one way or the
// other by the cell's checker parity); 6532f8 off the grid
double __fastcall Grid_HeightAt(BYTE* s, void*, const float* p)
{
    if (!(p[0] < F(float, s, 40140)) || !(p[0] > F(float, s, 40136)) || !(p[1] < F(float, s, 40144)) ||
        !(p[1] > F(float, s, 40148)))
        return g_gh6532f8;
    float gx = (float)(((double)p[0] - F(float, s, 40136)) / F(float, s, 40152));
    float gy = (float)(((double)p[1] - F(float, s, 40148)) / F(float, s, 40156));
    DWORD X0 = (DWORD)(int)(floor(gx / (double)g_gh653240) * g_gh653240);
    DWORD Y0 = (DWORD)(int)(floor(gy / (double)g_gh653244) * g_gh653244);
    DWORD X1 = (DWORD)(int)(ceil(gx / (double)g_gh653240) * g_gh653240);
    DWORD Y1 = (DWORD)(int)(ceil(gy / (double)g_gh653244) * g_gh653244);
    double fu0 = (gx - (double)X0) / g_gh653240;
    float fu = (float)fu0;
    float fv = (float)((gy - (double)Y0) / g_gh653244);
    // the triangle: across the cell's diagonal by the parity, 3 the one past it
    int cross = ((X0 ^ Y0) & 4) != 0;
    int tri;
    if (cross)
        tri = fu0 < (double)g_gh64c250 - fv ? 2 : 3;
    else
        tri = fu0 < fv ? 3 : 2;
    BYTE** T = F(BYTE**, s, 40192);
    if (fv == g_gh64c264 || fv == g_gh64c250)
    {
        // on a row of points
        DWORD y = fv == g_gh64c264 ? Y0 : Y1;
        float h0 = Gh_At(T, X0, y);
        float h1 = Gh_At(T, X1, y);
        return ((double)h1 - h0) * fu + h0;
    }
    float hA, hB, hC;
    double w;
    double e;
    if (!cross)
    {
        hA = Gh_At(T, X0, Y0);
        hB = Gh_At(T, X1, Y1);
        if (tri == 3)
        {
            hC = Gh_At(T, X0, Y1);
            w = fu / (double)fv;
            e = ((double)hC - hA) * fv + hA;
        }
        else
        {
            hC = Gh_At(T, X1, Y0);
            w = ((double)fu - fv) / ((double)g_gh64c250 - fv);
            e = ((double)hB - hC) * fv + hC;
        }
    }
    else
    {
        hA = Gh_At(T, X1, Y0);
        hB = Gh_At(T, X0, Y1);
        if (tri == 3)
        {
            hC = Gh_At(T, X1, Y1);
            w = ((double)fv + fu - g_gh64c250) / fv;
            e = ((double)hC - hA) * fv + hA;
        }
        else
        {
            hC = Gh_At(T, X0, Y0);
            w = fu / ((double)g_gh64c250 - fv);
            e = ((double)hB - hC) * fv + hC;
        }
    }
    float ww = (float)w;
    float m = (float)(((double)hB - hA) * fv + hA);
    if ((tri == 2 && cross) || (!cross && tri == 3))
        return e + ((double)m - e) * ww;
    return m + (e - m) * ww;
}

} // extern "C"

// === building the terrain grid (FUN_61a8a0)

#include <stdlib.h>

extern "C" void Tb_Set(); KING_ALIAS("_Tb_Set", "@Vec3_SetThis@20")
extern "C" void Tb_Level32(); KING_ALIAS("_Tb_Level32", "_Grid_Level32")
extern "C" void Tb_TaskAlloc(); KING_ALIAS("_Tb_TaskAlloc", "@GridTask_Alloc@16")
extern "C" void Tb_Reset(); KING_ALIAS("_Tb_Reset", "@Grid_Reset@16")
extern "C" void Tb_MakeNode(); KING_ALIAS("_Tb_MakeNode", "@TGrid_MakeNode@16")
extern "C" void Tb_NodeInit(); KING_ALIAS("_Tb_NodeInit", "@TNode_Init@16")
extern "C" void Tb_ObjInit(); KING_ALIAS("_Tb_ObjInit", "@GridObj_Construct@8")
extern "C" void Tb_Sample(); KING_ALIAS("_Tb_Sample", "@Terrain_Sample@16")
extern "C" void Tb_Height(); KING_ALIAS("_Tb_Height", "@TGrid_Height@12")
extern "C" void Tb_Priority(); KING_ALIAS("_Tb_Priority", "@TGridCamera_NodePriority@12")
extern "C" void Tb_Insert(); KING_ALIAS("_Tb_Insert", "@MarineQueue_InsertDeferredNode@12")
extern "C" void Tb_NewCell(); KING_ALIAS("_Tb_NewCell", "@Grid_NewCell@24")
extern "C" void Tb_SetTris(); KING_ALIAS("_Tb_SetTris", "@TerrainCell_SetTriangles@16")
extern "C" void Tb_RemoveCell(); KING_ALIAS("_Tb_RemoveCell", "@Grid_RemoveCell@12")
extern "C" void Tb_Follow(); KING_ALIAS("_Tb_Follow", "@TGrid_FollowCamera@20")
extern "C" void Tb_Reprio(); KING_ALIAS("_Tb_Reprio", "@GridMesh_Reprioritise@8")
extern "C" void Tb_Error(); KING_ALIAS("_Tb_Error", "_Grd_ReportError")
ASM_VAR(AsmLabel, g_tb6533a8, $L_6533a8)
ASM_VAR(AsmLabel, g_tb65338c, $L_65338c)
ASM_VAR(AsmLabel, g_tb652f00, $L_652f00)
ASM_VAR(float, g_tb64dac8, $L_64dac8)
ASM_VAR(float, g_tb64c258, $L_64c258)
ASM_VAR(float, g_tb64c264, $L_64c264)
ASM_VAR(float, g_tb64c904, $L_64c904)

static void Tb_Fail(int line)
{
    CallC(Tb_Error, 0x88750014 - (line == 41 ? 0 : 3), " File = %s, Line = #%ld, Date = %s ",
          "C:\\Nek\\VRAPPLVS\\terrain\\tgreed.cpp", line, "Jan 16 2003");
}

// a new cell (76 bytes) over vertices a, b, c at level 0, its fields as Grid_NewCell sets them; 0 when out of memory
static BYTE* Tb_Cell(WORD* a, WORD* b, WORD* c)
{
    BYTE* n = (BYTE*)operator new(76);
    if (!n)
        return 0;
    F(BYTE*, n, 4) = n + 4;
    F(BYTE*, n, 8) = n + 4;
    CallThis(Tb_ObjInit, n + 12);
    F(void*, n, 0) = &g_tb652f00;
    F(int, n, 68) = 0;
    F(int, n, 64) = 0;
    F(int, n, 60) = 0;
    F(int, n, 72) = 0;
    F(int, n, 56) = 0;
    F(WORD*, n, 16) = a;
    F(WORD*, n, 20) = b;
    F(WORD*, n, 24) = c;
    F(int, n, 36) = 0;
    return n;
}

static float Tb_Height(BYTE* G, const void* xy)
{
    return (float)CallThis<double>(Tb_Height, G, xy);
}

// the height error (+32), the camera priority (+28) and the queue for cell n of mesh s
static void Tb_Finish(BYTE* s, BYTE* n, double ha, const DWORD* c, const DWORD* mid)
{
    BYTE* G = F(BYTE*, s, 4);
    float avg = (float)(((double)Tb_Height(G, c) + ha) * g_tb64c258);
    double d = (double)avg - Tb_Height(G, mid);
    if (d < g_tb64c264)
        d = -d;
    F(float, n, 32) = (float)d;
    F(WORD, s, 42)++;
    F(float, n, 28) = (float)CallThis<double>(Tb_Priority, F(BYTE*, s, 4), n + 12);
    CallThis(Tb_Insert, F(BYTE*, s, 36), n);
}

// the larger of r and the float of x*x + y*y
static double Tb_Max(double r, double x, double y)
{
    float q = (float)(x * x + y * y);
    return r > q ? r : q;
}

// the first inline cell kind (a = the square's corner): all six coordinates rounded, the centre's x kept on the FPU,
// the a corner's height sampled from the terrain
static void Tb_CellA(BYTE* s, BYTE* n)
{
    BYTE* V = F(BYTE*, s, 12);
    BYTE* va = V + *F(WORD*, n, 16) * 16;
    BYTE* vb = V + *F(WORD*, n, 20) * 16;
    BYTE* vc = V + *F(WORD*, n, 24) * 16;
    DWORD mid[2];
    mid[0] = (int)((__int64)(unsigned)(F(int, va, 8) + F(int, vc, 8)) * (double)g_tb64c258);
    mid[1] = (int)((__int64)(unsigned)(F(int, vc, 12) + F(int, va, 12)) * (double)g_tb64c258);
    BYTE* G = F(BYTE*, s, 4);
    double sx = F(float, G, 40152), ox = F(float, G, 40136), sy = F(float, G, 40156), oy = F(float, G, 40148);
    float ax = (float)((__int64)(unsigned)F(int, va, 8) * sx + ox);
    float ay = (float)((__int64)(unsigned)F(int, va, 12) * sy + oy);
    float bx = (float)((__int64)(unsigned)F(int, vb, 8) * sx + ox);
    float by = (float)((__int64)(unsigned)F(int, vb, 12) * sy + oy);
    float cx = (float)((__int64)(unsigned)F(int, vc, 8) * sx + ox);
    float cy = (float)((__int64)(unsigned)F(int, vc, 12) * sy + oy);
    double mx = ((double)cx + bx + ax) * g_tb64c904;
    float my = (float)(((double)cy + by + ay) * g_tb64c904);
    double e = (double)ay - my, d = ax - mx;
    double r = d * d + e * e;
    r = Tb_Max(r, (double)bx - mx, (double)by - my);
    r = Tb_Max(r, (double)cx - mx, (double)cy - my);
    F(float, n, 44) = (float)mx;
    F(float, n, 48) = my;
    F(float, n, 52) = (float)x87_sqrt(r);
    float ha = (float)CallThis<double>(Tb_Sample, F(BYTE*, G, 40192), F(DWORD, va, 8), F(DWORD, va, 12));
    Tb_Finish(s, n, ha, (const DWORD*)(vc + 8), mid);
}

// the second inline kind: the a corner's x and y kept on the FPU, the centre rounded
static void Tb_CellB(BYTE* s, BYTE* n)
{
    BYTE* V = F(BYTE*, s, 12);
    BYTE* va = V + *F(WORD*, n, 16) * 16;
    BYTE* vb = V + *F(WORD*, n, 20) * 16;
    BYTE* vc = V + *F(WORD*, n, 24) * 16;
    DWORD mid[2];
    mid[1] = (int)((__int64)(unsigned)(F(int, va, 12) + F(int, vc, 12)) * (double)g_tb64c258);
    mid[0] = (int)((__int64)(unsigned)(F(int, vc, 8) + F(int, va, 8)) * (double)g_tb64c258);
    BYTE* G = F(BYTE*, s, 4);
    double sx = F(float, G, 40152), ox = F(float, G, 40136), sy = F(float, G, 40156), oy = F(float, G, 40148);
    double ax = (__int64)(unsigned)F(int, va, 8) * sx + ox;
    double ay = (__int64)(unsigned)F(int, va, 12) * sy + oy;
    float bx = (float)((__int64)(unsigned)F(int, vb, 8) * sx + ox);
    float by = (float)((__int64)(unsigned)F(int, vb, 12) * sy + oy);
    float cx = (float)((__int64)(unsigned)F(int, vc, 8) * sx + ox);
    float cy = (float)((__int64)(unsigned)F(int, vc, 12) * sy + oy);
    float mx = (float)(((double)cx + bx + ax) * g_tb64c904);
    float my = (float)(((double)cy + by + ay) * g_tb64c904);
    float e = (float)(ay - my);
    double d = ax - mx;
    double r = d * d + (double)e * e;
    r = Tb_Max(r, (double)bx - mx, (double)by - my);
    r = Tb_Max(r, (double)cx - mx, (double)cy - my);
    float t[3];
    CallThis(Tb_Set, t, mx, my, (float)x87_sqrt(r));
    memcpy(n + 44, t, 12);
    float ha = Tb_Height(G, va + 8);
    Tb_Finish(s, n, ha, (const DWORD*)(vc + 8), mid);
}

extern "C" {

// [FUN_61a8a0] mesh s's terrain grid over size sx * sy at pos: the grid object (40204 bytes: its scale and bounds,
// the view constants) and its task lists; a node per grid point (256 units apart) and two cells per square, made
// inline on odd squares and by Grid_NewCell on even ones, linked to their neighbours (+60 base, +64 / +68 sides),
// paired into the squares' records (+8) and the four corner cells removed; then the links checked, the camera
// followed and the mesh reprioritised; 0 when out of memory
int __fastcall TGridMesh_Build(BYTE* s, void*, const float* pos, float sizeX, float sizeY)
{
    unsigned nx = (int)(sizeX * (double)g_tb64dac8);
    unsigned ny = (int)(sizeY * (double)g_tb64dac8);
    unsigned cells = ny * nx;
    unsigned verts = (ny + 1) * (nx + 1);
    BYTE* G = (BYTE*)operator new(40204);
    if (G)
    {
        F(void*, G, 0) = &g_tb6533a8;
        F(int, G, 48) = 0;
        F(int, G, 44) = 0;
        F(int, G, 40) = 0;
        float z[3];
        CallThis(Tb_Set, z, 0.0f, 0.0f, 0.0f);
        memcpy(G + 4, z, 12);
        CallThis(Tb_Set, z, 0.0f, 0.0f, 0.0f);
        memcpy(G + 16, z, 12);
        CallThis(Tb_Set, z, 0.0f, 0.0f, 0.0f);
        memcpy(G + 28, z, 12);
        F(void*, G, 0) = &g_tb65338c;
        F(int, G, 40196) = F(int, s, 108);
        F(int, G, 40200) = F(int, s, 112);
        F(int, G, 104) = 0;
        F(int, G, 100) = 0;
        F(int, G, 40192) = 0;
        F(int, G, 40116) = 1;
    }
    float sx = (float)(sizeX / (double)(__int64)(nx << 8));
    F(float, G, 40120) = sizeX;
    F(float, G, 40124) = sizeY;
    F(float, G, 40128) = pos[0];
    F(float, G, 40132) = pos[1];
    F(float, G, 40152) = sx;
    double sy = sizeY / (double)(__int64)(ny << 8);
    double hx = sizeX * (double)g_tb64c258;
    F(float, G, 40136) = (float)(F(float, G, 40128) - hx);
    F(float, G, 40140) = (float)(hx + F(float, G, 40128));
    double hy = sizeY * (double)g_tb64c258;
    F(float, G, 40144) = (float)(hy + F(float, G, 40132));
    F(float, G, 40148) = (float)(F(float, G, 40132) - hy);
    F(float, G, 40156) = (float)sy;
    F(float, G, 40160) = (float)(sx * (double)g_tb64c258);
    F(float, G, 40164) = 0;
    F(float, G, 40168) = 0;
    float v[3];
    CallThis(Tb_Set, v, 0.0f, (float)(sy * g_tb64c258), 0.0f);
    memcpy(G + 40172, v, 12);
    F(float, G, 52) = 24.0f;
    F(float, G, 56) = 24.0f;
    CallVirt(G, 8, nx << 8, ny << 8);
    F(BYTE*, s, 4) = G;
    CallThis(Tb_TaskAlloc, s, CallC<int>(Tb_Level32), cells);
    CallThis(Tb_Reset, s, CallC<int>(Tb_Level32), cells);
    for (unsigned i = 0; i < cells; i++)
    {
        BYTE* e = (BYTE*)operator new(20);
        if (e)
        {
            F(int, e, 8) = 0;
            F(int, e, 4) = 0;
            F(int, e, 0) = 0;
            F(int, e, 12) = 0;
        }
        F(BYTE**, s, 8)[i] = e;
        if (!e)
            return 0;
    }
    WORD** vx = (WORD**)malloc(verts * 4);
    BYTE** tris = (BYTE**)malloc(cells * 2 * 4);
    if (!vx || !tris)
        Tb_Fail(41);
    for (unsigned i = 0; i <= nx; i++)
        for (unsigned j = 0; j <= ny; j++)
        {
            DWORD pt[2] = {i << 8, j << 8};
            BYTE node[40];
            BYTE* r = CallThis<BYTE*>(Tb_MakeNode, F(BYTE*, s, 4), node, pt);
            memcpy(F(BYTE*, s, 16) + F(WORD, s, 40) * 40, r, 40);
            WORD k = F(WORD, s, 40);
            vx[j * (nx + 1) + i] = CallThis<WORD*>(Tb_NodeInit, F(BYTE*, s, 12) + k * 16, pt, k);
            F(WORD, s, 40)++;
        }
#define VX(a, b) vx[(b) * (nx + 1) + (a)]
    unsigned k = 0;
    for (unsigned i = 1; i <= nx; i++)
        for (unsigned j = 1; j <= ny; j++, k += 2)
        {
            if ((i ^ j) & 1)
            {
                BYTE* c1 = Tb_Cell(VX(i, j), VX(i, j - 1), VX(i - 1, j - 1));
                if (c1)
                    Tb_CellA(s, c1);
                tris[k] = c1;
                BYTE* c2 = Tb_Cell(VX(i - 1, j - 1), VX(i - 1, j), VX(i, j));
                if (c2)
                    Tb_CellB(s, c2);
                tris[k + 1] = c2;
            }
            else
            {
                tris[k] = CallThis<BYTE*>(Tb_NewCell, s, VX(i, j - 1), VX(i - 1, j - 1), VX(i - 1, j), 0);
                tris[k + 1] = CallThis<BYTE*>(Tb_NewCell, s, VX(i - 1, j), VX(i, j), VX(i, j - 1), 0);
            }
        }
#undef VX
    k = 0;
    for (unsigned i = 1; i <= nx; i++)
        for (unsigned j = 1; j <= ny; j++, k += 2)
        {
            BYTE* t1 = tris[k];
            BYTE* t2 = tris[k + 1];
            if ((i ^ j) & 1)
            {
                F(BYTE*, t1, 64) = i == nx ? 0 : tris[k + 2 * ny];
                F(BYTE*, t1, 60) = t2;
                F(BYTE*, t1, 68) = j == 1 ? 0 : tris[k - 1];
                F(BYTE*, t2, 64) = i == 1 ? 0 : tris[k + 1 - 2 * ny];
                F(BYTE*, t2, 60) = t1;
                F(BYTE*, t2, 68) = j == ny ? 0 : tris[k + 2];
            }
            else
            {
                F(BYTE*, t1, 64) = j == 1 ? 0 : tris[k - 1];
                F(BYTE*, t1, 60) = t2;
                F(BYTE*, t1, 68) = i == 1 ? 0 : tris[k - 2 * ny];
                F(BYTE*, t2, 64) = j == ny ? 0 : tris[k + 2];
                F(BYTE*, t2, 60) = t1;
                F(BYTE*, t2, 68) = i == nx ? 0 : tris[k + 2 * ny + 1];
            }
        }
    for (unsigned m = 0; m < (k + 1) >> 1; m++)
        CallThis(Tb_SetTris, F(BYTE**, s, 8)[m], tris[2 * m], tris[2 * m + 1]);
    BYTE** sq = F(BYTE**, s, 8);
    CallThis(Tb_RemoveCell, s, F(BYTE*, F(BYTE*, sq[0], 0), 12));
    CallThis(Tb_RemoveCell, s, F(BYTE*, F(BYTE*, F(BYTE**, s, 8)[0], 4), 12));
    CallThis(Tb_RemoveCell, s, F(BYTE*, F(BYTE*, F(BYTE**, s, 8)[8], 0), 12));
    CallThis(Tb_RemoveCell, s, F(BYTE*, F(BYTE*, F(BYTE**, s, 8)[8], 4), 12));
    // the links checked along the queue's two lists
    BYTE* q = F(BYTE*, s, 36);
    BYTE* c = F(BYTE*, q, 0);
    if (!c)
        c = F(BYTE*, q, 4);
    while (c)
    {
        if (F(BYTE*, c, 64) && F(BYTE*, F(BYTE*, c, 64), 68) != c)
            Tb_Fail(132);
        if (F(BYTE*, c, 68) && F(BYTE*, F(BYTE*, c, 68), 64) != c)
            Tb_Fail(136);
        if (!F(BYTE*, c, 60) || F(BYTE*, F(BYTE*, c, 60), 60) != c)
            Tb_Fail(140);
        q = F(BYTE*, s, 36);
        BYTE* l0 = F(BYTE*, q, 0);
        if (l0 && F(BYTE*, l0, 8) == c + 4)
        {
            c = F(BYTE*, q, 4);
            continue;
        }
        BYTE* l1 = F(BYTE*, q, 4);
        if (l1 && F(BYTE*, l1, 8) == c + 4)
            break;
        c = F(BYTE*, c, 4);
        if (!c)
            break;
        c -= 4;
    }
    CallThis(Tb_Follow, F(BYTE*, s, 4), s + 68, s + 80, s + 92);
    F(int, s, 28) = 1;
    F(int, s, 24) = 1;
    CallThis(Tb_Reprio, s);
    free(vx);
    free(tris);
    return 1;
}

} // extern "C"
