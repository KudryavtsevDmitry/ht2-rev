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

ASM_PROC(FUN_616ca0)    // seeds the noise tables
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

ASM_PROC(FUN_60bbb0)
ASM_PROC(FUN_5f4a90)
ASM_PROC(FUN_5ef660)
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

ASM_PROC(FUN_60c2b0)
ASM_PROC(FUN_619c20)
ASM_PROC(FUN_619850)
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
