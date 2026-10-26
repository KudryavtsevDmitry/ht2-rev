// object_ids.cpp - small functions of the unit at 0x57c970 of king.masm:
// ids of the game's objects (300 slots from 100 on), the moving object list,
// per-race state reset, and two object constructors.
//
// Building with  nmake ASM_OBJECT_IDS=1  makes king.masm assemble the
// original procedures again.

#include "king.h"
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef DWORD Dwords[1];

extern "C" void FUN_5cb9c0(); KING_ALIAS("_FUN_5cb9c0", "_Set70ebf0")
extern "C" void FUN_592bf0(); KING_ALIAS("_FUN_592bf0", "@ListItem592bf0_Construct@12")    // base constructor

KING_VAR(BYTE*, g_movingList, 6f653c)
KING_VAR(int, g_67844c, 67844c)
KING_VAR(WORD, g_67d10a, 67d10a)
KING_VAR(int, g_nextId, 67d10c)
KING_VAR(int, g_6f6620, 6f6620)
KING_VAR(AsmLabel, g_idUsed, 6f63e4)          // 300 flags
KING_VAR(AsmLabel, g_6f6380, 6f6380)
KING_VAR(DWORD, g_6f65b8, 6f65b8)
KING_VAR(DWORD, g_6f65bc, 6f65bc)
KING_VAR(DWORD, g_6f65c0, 6f65c0)
KING_VAR(DWORD, g_6f65c4, 6f65c4)
KING_VAR(Dwords, g_6f65f8, 6f65f8)
KING_VAR(Dwords, g_6f65ec, 6f65ec)
KING_VAR(Dwords, g_6f6570, 6f6570)
KING_VAR(Dwords, g_6f65a0, 6f65a0)
KING_VAR(Dwords, g_6f65c8, 6f65c8)
KING_VAR(Dwords, g_6f6518, 6f6518)
KING_VAR(DWORD, g_6f6610, 6f6610)
KING_VAR(DWORD, g_6f6614, 6f6614)
KING_VAR(DWORD, g_6f6618, 6f6618)
KING_VAR(DWORD, g_6f65e8, 6f65e8)
KING_VAR(Dwords, g_7025f0, 7025f0)
KING_VAR(AsmLabel, g_vtbl6504f0, 6504f0)

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
KING_VAR(AsmLabel, g_objids650544, 650544)
KING_VAR(AsmLabel, g_objids6504f8, 6504f8)
KING_VAR(AsmLabel, g_objids6504f4, 6504f4)
KING_VAR(BYTE*, g_objids6f653c, 6f653c)

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

KING_VAR(AsmLabel, g_mo650544, 650544)
KING_VAR(AsmLabel, g_mo6504f8, 6504f8)
KING_VAR(AsmLabel, g_mo6504f4, 6504f4)
KING_VAR(AsmLabel, g_mo6f653c, 6f653c)
extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
extern "C" void FUN_4bd4e0(); KING_ALIAS("_FUN_4bd4e0", "@Matrix_IdentityThis@8")
extern "C" void FUN_4e5ba7(); KING_ALIAS("_FUN_4e5ba7", "_Query_HitObject")
extern "C" void FUN_57ca30(); KING_ALIAS("_FUN_57ca30", "_Id_New")
extern "C" void FUN_57cb50(); KING_ALIAS("_FUN_57cb50", "@Object57cb50_Construct@12")
extern "C" void FUN_57cfc0(); KING_ALIAS("_FUN_57cfc0", "@Placement_Init@8")
extern "C" void FUN_57d860(); KING_ALIAS("_FUN_57d860", "@MovingItem_Init0@24")
extern "C" void FUN_5c3070(); KING_ALIAS("_FUN_5c3070", "@Table_Add6@20")
ASM_PROC(FUN_5c6170)
extern "C" void FUN_5c5f60(); KING_ALIAS("_FUN_5c5f60", "@NetMoving_Construct@88")
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

// === drawing an item and its marker (FUN_57d580)

extern "C" void Sh_Log(); KING_ALIAS("_Sh_Log", "_VehLog_Value")
extern "C" void Sh_GetLocal(); KING_ALIAS("_Sh_GetLocal", "@Node_GetLocal@12")
extern "C" void Sh_Draw(); KING_ALIAS("_Sh_Draw", "@SceneNode5f98c0_Draw@16")
extern "C" void Sh_sprintf(); KING_ALIAS("_Sh_sprintf", "_sprintf")
extern "C" void FUN_5b73a0(); KING_ALIAS("_FUN_5b73a0", "_Window_DrawLine")    // draws a line on a surface (x0, y0, x1, y1, colour, -)
KING_VAR(int, g_sh696c6c, 696c6c)
KING_VAR(int, g_sh696be4, 696be4)
KING_VAR(BYTE*, g_sh6ef1fc, 6ef1fc)
KING_VAR(float, g_sh64c250, 64c250)
KING_VAR(float, g_sh64c258, 64c258)

// logs " name_what x y z"
static void Sh_Log3(const char* fmt, const char* name, double x, double y, double z)
{
    char buf[96];
    CallC(Sh_sprintf, buf, fmt, name, x, y, z);
    CallC(Sh_Log, buf);
}

extern "C" {

// [FUN_57d580] draws scene node s (its item at -224) with (a, b); with the debug level 696be4 above 1 a cross marks
// its point (+280) on the screen through the current camera, logging both frames when 696c6c
int __fastcall Item_DrawMarked(BYTE* s, void*, int a, int b)
{
    float m[12];
    if (g_sh696c6c) {
        for (int i = 0; i < 4; i++)
            CallThis(FUN_4bd4d0, m + i * 3);
        memcpy(m, g_7025f0, 48);
        CallThis(Sh_GetLocal, F(BYTE*, F(BYTE*, s, -224), 68), m);
        CallC(Sh_Log, "Show");
        const char* name = F(const char*, F(BYTE*, s, 64), 12);
        Sh_Log3(" %s_M0 %6.3f %6.3f %6.3f ", name, m[9], m[10], m[11]);
        Sh_Log3(" %s_M %6.3f %6.3f %6.3f ", F(const char*, F(BYTE*, s, 64), 12), F(float, s, 280), F(float, s, 284),
                F(float, s, 288));
    }
    int r = CallThis<int>(Sh_Draw, s, a, b);
    if (g_sh696be4 <= 1)
        return r;
    BYTE* w = F(BYTE*, g_sh6ef1fc, 464);
    memcpy(m, w + 2240 + F(int, w, 8224) * 48, 48);
    const float* p = (const float*)(s + 280);
    double px = (double)m[0] * p[0] + (double)m[6] * p[2] + (double)m[3] * p[1] + m[9];
    float py = (float)((double)m[1] * p[0] + (double)m[7] * p[2] + (double)m[4] * p[1] + m[10]);
    float pz = (float)((double)m[2] * p[0] + (double)m[8] * p[2] + (double)m[5] * p[1] + m[11]);
    if (py < g_sh64c250)
        return r;
    BYTE* surf = F(BYTE*, w, 1948);
    int sw = F(WORD, surf, 0), sh = F(WORD, surf, 2);
    int x = (int)(px / py * sw * g_sh64c258 + sw / 2);
    int y = (int)(sh / 2 - pz / (double)py * sh * g_sh64c258);
    CallC(FUN_5b73a0, surf, x, y - 4, x, y + 4, 0, 0);
    CallC(FUN_5b73a0, F(BYTE*, F(BYTE*, g_sh6ef1fc, 464), 1948), x - 4, y, x + 4, y, 0, 0);
    CallC(FUN_5b73a0, F(BYTE*, F(BYTE*, g_sh6ef1fc, 464), 1948), x - 3, y - 3, x + 3, y + 3, 4, 0);
    CallC(FUN_5b73a0, F(BYTE*, F(BYTE*, g_sh6ef1fc, 464), 1948), x + 3, y - 3, x - 3, y + 3, 4, 0);
    return r;
}

} // extern "C"

// === setting an item's spin (FUN_581430)

extern "C" void FUN_4e28b0(); KING_ALIAS("_FUN_4e28b0", "_Vec3_OutOfBox")    // whether a vector is longer than a limit (v, limit)
extern "C" void Iw_CheckPoint(); KING_ALIAS("_Iw_CheckPoint", "_World_CheckPoint")
extern "C" void Iw_MulComponents(); KING_ALIAS("_Iw_MulComponents", "_Vec3_MulComponents")
extern "C" void Iw_Normalized(); KING_ALIAS("_Iw_Normalized", "_Vec3_Normalized")
extern "C" void Iw_sprintf(); KING_ALIAS("_Iw_sprintf", "_sprintf")
KING_VAR(BYTE*, g_iw6d2078, 6d2078)
KING_VAR(float, g_iw6969f4, 6969f4)
KING_VAR(float, g_iw6969f8, 6969f8)
KING_VAR(float, g_iw6969fc, 6969fc)
KING_VAR(float, g_iw64c450, 64c450)
KING_VAR(AsmLabel, s_iwInfoWorld, 67d528)
KING_VAR(AsmLabel, s_iwInfo, 67d4f4)
KING_VAR(AsmLabel, s_iwSetW, 67d578)
KING_VAR(AsmLabel, s_iwNewline, 66c4e4)
KING_VAR(AsmLabel, s_iw696cbc, 696cbc)
KING_VAR(AsmLabel, s_iwEmpty, 68d1c0)

extern "C" {

// [FUN_581430] sets item s's spin (+428) to w: one longer than 10 is reported (with the item, its mass, room and
// position, the context ctx) and capped at 10 unless it is wild (over 10000: checked); then its world spin (+440,
// scaled by +404 and turned by +500) and both kept at +3312/+3324
void __fastcall Item_SetSpin(BYTE* s, void*, const float* w, const char* ctx)
{
    float* o = (float*)(s + 428);
    if (!CallC<int>(FUN_4e28b0, w, 10.0f))
        memcpy(o, w, 12);
    else {
        char msg[300], buf[488];
        const char* name = F(const char*, F(BYTE*, F(BYTE*, s, 32), 64), 12);
        const char* room = F(const char*, F(BYTE*, s, 800), 12);
        if (g_iw6d2078)
            CallC(Iw_sprintf, msg, (const char*)s_iwInfoWorld, name, (double)F(float, s, 396), room,
                  (double)F(float, s, 536), (double)F(float, s, 540), (double)F(float, s, 544), (const char*)s_iw696cbc,
                  (double)g_iw6969f4, (double)g_iw6969f8, (double)g_iw6969fc);
        else
            CallC(Iw_sprintf, msg, (const char*)s_iwInfo, name, (double)F(float, s, 396), room,
                  (double)F(float, s, 536), (double)F(float, s, 540), (double)F(float, s, 544));
        const char* a = ctx ? ctx : (const char*)s_iwEmpty;
        const char* b = ctx ? (const char*)s_iwNewline : (const char*)s_iwEmpty;
        CallC(Iw_sprintf, buf, (const char*)s_iwSetW, a, b, msg);
        if (CallC<int>(FUN_4e28b0, w, 10000.0f))
            CallC(Iw_CheckPoint, w, buf, 10000.0f);
        else {
            float nb[3];
            const float* n = CallC<const float*>(Iw_Normalized, nb, w);
            o[0] = (float)(n[0] * (double)g_iw64c450);
            o[1] = (float)(n[1] * (double)g_iw64c450);
            o[2] = (float)(n[2] * (double)g_iw64c450);
        }
    }
    float tb[3];
    const float* t = CallC<const float*>(Iw_MulComponents, tb, o, s + 404);
    const float* m = (const float*)(s + 500);
    float x = (float)((double)m[6] * t[2] + (double)m[3] * t[1] + (double)m[0] * t[0]);
    float y = (float)((double)m[7] * t[2] + (double)m[4] * t[1] + (double)m[1] * t[0]);
    float z = (float)((double)m[8] * t[2] + (double)m[5] * t[1] + (double)m[2] * t[0]);
    F(float, s, 440) = x;
    F(float, s, 444) = y;
    F(float, s, 448) = z;
    memcpy(s + 3312, o, 12);
    memcpy(s + 3324, s + 440, 12);
}

} // extern "C"

// === placing an item by a frame (581060)

extern "C" void Sf_ToAngles(); KING_ALIAS("_Sf_ToAngles", "_Matrix_ToAngles")
extern "C" void Sf_FromPose(); KING_ALIAS("_Sf_FromPose", "_Matrix_FromPose")
extern "C" void Sf_Inverse(); KING_ALIAS("_Sf_Inverse", "_Matrix_Inverse")
extern "C" void Sf_DivComponents(); KING_ALIAS("_Sf_DivComponents", "_Vec3_DivComponents")
struct Sf_Mat { DWORD m[12]; };

extern "C" {

// [$L_581060] places item s in room r with frame m: the frame rebuilt from its position and angles (+500), its
// inverse (+596), the spin in the item's axes (+428, from +440 by the inertia +404); all kept at +3336.. (+3432 room)
void __fastcall Item_SetFrame(BYTE* s, void*, int r, const Sf_Mat* m)
{
    F(int, s, 800) = r;
    memcpy(s + 500, m, 48);
    double d[6];
    CallC(Sf_ToAngles, *m, d, d + 1, d + 2, d + 3, d + 4, d + 5);
    BYTE pm[48], inv[48];
    memcpy(s + 500, CallC<BYTE*>(Sf_FromPose, pm, d[0], d[1], d[2], d[3], d[4], d[5]), 48);
    memcpy(s + 596, CallC<BYTE*>(Sf_Inverse, inv, s + 500), 48);
    const float* iv = (const float*)(s + 596);
    const float* w = (const float*)(s + 440);
    float t[3], o[3];
    t[0] = (float)((double)iv[6] * w[2] + (double)iv[3] * w[1] + (double)iv[0] * w[0]);
    t[1] = (float)((double)iv[7] * w[2] + (double)iv[4] * w[1] + (double)iv[1] * w[0]);
    t[2] = (float)((double)iv[8] * w[2] + (double)iv[5] * w[1] + (double)iv[2] * w[0]);
    memcpy(s + 428, CallC<float*>(Sf_DivComponents, o, t, s + 404), 12);
    F(int, s, 3432) = F(int, s, 800);
    memcpy(s + 3336, s + 500, 48);
    memcpy(s + 3384, s + 596, 48);
}

} // extern "C"

// === querying an object's shape (FUN_57e140)

extern "C" void Qs_Inverse(); KING_ALIAS("_Qs_Inverse", "_Matrix_Inverse")
KING_VAR(AsmLabel, g_qs7025f0, 7025f0)

extern "C" {

// [FUN_57e140] fills hit record h for object o (+400, the ray +584/+536, identity frame +4) and asks o's shape
// (+804, vtbl+52 (6, h)); a result with bit 1 and no object hit (+104) wakes o (vtbl+28 (1)), with bit 0 the hit
// point (+52) and normal (+64) are taken into the frame's space; the result
int __fastcall Object_QueryShape(BYTE* o, void*, BYTE* h)
{
    memcpy(h + 4, g_qs7025f0, 48);
    memcpy(h + 76, o + 584, 12);
    memcpy(h + 88, o + 536, 12);
    F(BYTE*, h, 120) = o;
    F(int, h, 116) = 0;
    F(int, h, 100) = 0;
    F(int, h, 0) = F(int, o, 400);
    int r = CallVirt<int>(F(BYTE*, o, 804), 52, 6, h);
    if ((r & 2) && !F(int, h, 104))
        CallVirt(o, 28, 1);
    if (r & 1) {
        float m[12];
        CallC(Qs_Inverse, m, h + 4);
        float* p = (float*)(h + 52);
        float x = (float)((double)m[6] * p[2] + (double)m[3] * p[1] + (double)m[0] * p[0] + m[9]);
        float y = (float)((double)m[7] * p[2] + (double)m[4] * p[1] + (double)m[1] * p[0] + m[10]);
        float z = (float)((double)m[8] * p[2] + (double)m[5] * p[1] + (double)m[2] * p[0] + m[11]);
        p[0] = x, p[1] = y, p[2] = z;
        float* n = (float*)(h + 64);
        x = (float)((double)m[0] * n[0] + (double)m[6] * n[2] + (double)m[3] * n[1]);
        y = (float)((double)m[1] * n[0] + (double)m[7] * n[2] + (double)m[4] * n[1]);
        z = (float)((double)m[2] * n[0] + (double)m[8] * n[2] + (double)m[5] * n[1]);
        n[0] = x, n[1] = y, n[2] = z;
    }
    return r;
}

} // extern "C"

// === setting an item's velocity (FUN_5812a0)

KING_VAR(AsmLabel, s_iwSetV, 67d4d8)
extern "C" void Iw_Length(); KING_ALIAS("_Iw_Length", "_Vec3_Length")

extern "C" {

// [FUN_5812a0] sets item s's velocity (+416, kept at +3300) to v, checked against 150 (reported with the item's
// details and the context ctx); its speed at +888 and +3460
void __fastcall Item_SetVelocity(BYTE* s, void*, const float* v, const char* ctx)
{
    float* o = (float*)(s + 416);
    memcpy(o, v, 12);
    char msg[300], buf[488];
    const char* name = F(const char*, F(BYTE*, F(BYTE*, s, 32), 64), 12);
    const char* room = F(const char*, F(BYTE*, s, 800), 12);
    if (g_iw6d2078)
        CallC(Iw_sprintf, msg, (const char*)s_iwInfoWorld, name, (double)F(float, s, 396), room,
              (double)F(float, s, 536), (double)F(float, s, 540), (double)F(float, s, 544), (const char*)s_iw696cbc,
              (double)g_iw6969f4, (double)g_iw6969f8, (double)g_iw6969fc);
    else
        CallC(Iw_sprintf, msg, (const char*)s_iwInfo, name, (double)F(float, s, 396), room,
              (double)F(float, s, 536), (double)F(float, s, 540), (double)F(float, s, 544));
    const char* a = ctx ? ctx : (const char*)s_iwEmpty;
    const char* b = ctx ? (const char*)s_iwNewline : (const char*)s_iwEmpty;
    CallC(Iw_sprintf, buf, (const char*)s_iwSetV, a, b, msg);
    CallC(Iw_CheckPoint, o, buf, 150.0f);
    memcpy(s + 3300, o, 12);
    float l = CallC<float>(Iw_Length, o);
    F(float, s, 888) = l;
    F(float, s, 3460) = l;
}

} // extern "C"

// === an object held on a rail (FUN_57e2e0)

#include "x87.h"
extern "C" void Rl_FrameTime(); KING_ALIAS("_Rl_FrameTime", "_Game_FrameTime")
extern "C" void Rl_Cross(); KING_ALIAS("_Rl_Cross", "_Vec3_Cross")
extern "C" void Rl_Scaled(); KING_ALIAS("_Rl_Scaled", "_Vec3_Scaled")
extern "C" void Rl_Add(); KING_ALIAS("_Rl_Add", "_Vec3_Add")
KING_VAR(double, g_rl64b658, 64b658)
KING_VAR(char, s_rl67d1b8, 67d1b8)

extern "C" {

// [FUN_57e2e0] object o kept on rail r (time +0, start +52, direction +64): its frame (+3172) into out with the
// position pulled onto the rail line at the step's time, and its velocity (+3300) turned onto the rail
// (method 144) as the spin about it plus its speed along it times +1424
void __cdecl Obj_HoldOnRail(BYTE* o, const float* r, float* out)
{
    CallC<double>(Rl_FrameTime);
    double T = r[0] + g_rl64b658;
    const float* n = r + 16;
    float F[12];
    memcpy(F, o + 3172, 48);
    float cx = (float)((float)(T * n[0]) + (double)r[13]);
    float cy = (float)((float)(T * n[1]) + (double)r[14]);
    double cz = T * n[2] + r[15];
    double dx = (double)F[9] - cx, dy = (double)F[10] - cy, dz = F[11] - cz;
    float ex = (float)(dz * n[1] - dy * n[2]);
    float ey = (float)(dx * n[2] - dz * n[0]);
    double ez = dy * n[0] - dx * n[1];
    float fx = (float)((double)ey * n[2] - ez * n[1]);
    float fy = (float)(ez * n[0] - (double)ex * n[2]);
    double fz = (double)ex * n[1] - (double)ey * n[0];
    F[9] = (float)((double)fx + cx);
    F[10] = (float)((double)cy + fy);
    F[11] = (float)(cz + fz);
    memcpy(out, F, 48);
    const float* v = (const float*)(o + 3300);
    double d = x87_fabs((double)v[2] * n[2] + (double)v[1] * n[1] + (double)v[0] * n[0]);
    float A[3] = {(float)(d * n[0]), (float)(d * n[1]), (float)(d * n[2])};
    float W[3];
    W[2] = (float)((double)v[1] * n[0] - (double)n[1] * v[0]);
    W[1] = (float)((double)v[0] * n[2] - (double)n[0] * v[2]);
    W[0] = (float)((double)n[1] * v[2] - (double)v[1] * n[2]);
    float C1[3], S2[3], R[3];
    CallC(Rl_Cross, C1, W, n);
    CallC(Rl_Add, R, C1, CallC<float*>(Rl_Scaled, S2, A, F(float, o, 1424)));
    CallC(Iw_CheckPoint, R, &s_rl67d1b8, 150.0f);
    CallVirt(o, 144, R, &s_rl67d1b8);
}

} // extern "C"

// === a moving item's first initialization (FUN_57d860 with its tail $L_57d87f)

extern "C" void Mi_Fatal(); KING_ALIAS("_Mi_Fatal", "_World_Fatal")
extern "C" void Mi_Time(); KING_ALIAS("_Mi_Time", "_Game_Time")
extern "C" void Mi_InitState(); KING_ALIAS("_Mi_InitState", "@SceneObj_InitState@24")
extern "C" void Mi_57e030(); KING_ALIAS("_Mi_57e030", "@SceneObj_57e030@12")
extern "C" void Mi_Frame(); KING_ALIAS("_Mi_Frame", "@MovingItem_CheckFrame@20")
KING_VAR(int, g_mi67d10c, 67d10c)
KING_VAR(int, g_mi696be0, 696be0)
KING_VAR(Vec3, g_miZero, 702650)
KING_VAR(Vec3, g_miAhead, 702630)
KING_VAR(Vec3, g_mi6f63d8, 6f63d8)
KING_VAR(float, g_mi64d7b8, 64d7b8)

static double Mi_Now()
{
    return CallC<double>(Mi_Time);
}

// the point ahead of s's frame origin (+536) by 64d7b8
static Vec3 Mi_Ahead(BYTE* s)
{
    double k = g_mi64d7b8;
    float y = (float)(g_miAhead.y * k), z = (float)(g_miAhead.z * k);
    Vec3 r;
    r.x = (float)(g_miAhead.x * k + F(float, s, 536));
    r.y = (float)((double)y + F(float, s, 540));
    r.z = (float)((double)z + F(float, s, 544));
    return r;
}

extern "C" {

// [FUN_57d860..$L_57d87f] moving item s at scale `scale`, frame m5 (+500) under parent frame m, owner a: its
// priority, clocks and state, the scale's start state, the frames relative to the parent (+596 / +692 / +740), the
// points ahead (+1164 / +1176), the frame nodes for both frames, the placement, cleared tables and limits (1e7)
void __fastcall MovingItem_Init0(BYTE* s, void*, float scale, const Matrix* m, const Matrix* m5, int a)
{
    if (g_mi67d10c >= 400)
        CallC(Mi_Fatal, "Moving_Priority>=BEGIN_PRIOR");
    F(int, s, 1188) = F(int, s, 368);
    F(int, s, 3556) = 0;
    F(int, s, 1196) = 4;
    F(float, s, 808) = (float)Mi_Now();
    F(short, s, 814) = 0;
    F(short, s, 812) = 0;
    F(float, s, 3560) = (float)Mi_Now();
    F(float, s, 400) = scale;
    F(int, s, 1420) = 0;
    F(int, s, 1424) = g_mi696be0;
    F(float, s, 1428) = 0.5f;
    F(int, s, 892) = 0;
    F(int, s, 896) = 0;
    F(float, s, 904) = (float)(Mi_Now() - g_mi64d7b8);
    F(Vec3, s, 3436) = g_miZero;
    F(Vec3, s, 3448) = g_miZero;
    F(int, s, 392) = 0;
    Vec3 sc = {F(float, s, 400), F(float, s, 400), F(float, s, 400)};
    CallThis(Mi_InitState, s, 1.0f, sc);
    F(int, s, 800) = a;
    memcpy(s + 500, m5, 48);
    Matrix t, u;
    memcpy(s + 596, Matrix_Inverse(&t, (const Matrix*)(s + 500)), 48);
    memcpy(s + 692, Matrix_Multiply(&t, m, (const Matrix*)(s + 596)), 48);
    memcpy(s + 740, Matrix_Multiply(&t, (const Matrix*)(s + 500), Matrix_Inverse(&u, m)), 48);
    memcpy(s + 3172, s + 500, 48);
    F(Vec3, s, 1164) = Mi_Ahead(s);
    F(Vec3, s, 1176) = Mi_Ahead(s);
    float out[3] = {0, (float)(g_miAhead.y * g_mi64d7b8), (float)(g_miAhead.z * g_mi64d7b8)};
    CallVirt(s, 220, out);
    F(Vec3, s, 416) = g_miZero;
    F(Vec3, s, 428) = g_miZero;
    F(Vec3, s, 440) = g_miZero;
    F(Vec3, s, 464) = g_miZero;
    memcpy(s + 816, s + 500, 48);
    F(Vec3, s, 864) = F(Vec3, s, 416);
    F(Vec3, s, 876) = F(Vec3, s, 428);
    CallThis(Mi_Frame, s, m5, "_MovingItem::init0 #1", 0);
    CallThis(Mi_Frame, s, m, "_MovingItem::init0 #2", 0);
    CallVirt(s, 52);
    CallThis(Mi_57e030, s, s + 3268);
    F(Vec3, s, 788) = g_mi6f63d8;
    F(BYTE, s, 1132) = 0;
    memset(s + 3468, 0, 36);
    F(int, s, 3504) = 0;
    F(int, s, 1192) = 0;
    F(int, s, 1200) = 0;
    F(Vec3, s, 3148) = g_miZero;
    F(Vec3, s, 3160) = g_miZero;
    F(int, s, 1204) = 0;
    F(int, s, 1208) = 0;
    F(int, s, 1412) = 0;
    F(int, s, 1416) = 0;
    memset(s + 1212, 0, 200);
    for (int k = 0; k < 6; k++)
    {
        F(int, s, 3508 + 4 * k) = 0;
        F(DWORD, s, 3532 + 4 * k) = 0x4B189680;
    }
    F(Vec3, s, 476) = g_miZero;
    F(Vec3, s, 488) = g_miZero;
    F(int, s, 1440) = 0;
    F(float, s, 3144) = (float)(Mi_Now() - g_mi64d7b8);
    CallVirt(s, 76);
}

} // extern "C"

// === a moving item's frame checked against its own frames (FUN_58e7f0 with its tails up to $L_58f1cd)

extern "C" void Ck_Local(); KING_ALIAS("_Ck_Local", "@Node_GetLocal@12")
extern "C" void Ck_LogOpen(); KING_ALIAS("_Ck_LogOpen", "_WarnLog_Open")
extern "C" void Ck_LogClose(); KING_ALIAS("_Ck_LogClose", "_WarnLog_Close")
extern "C" void Ck_Clock(); KING_ALIAS("_Ck_Clock", "_Clock_Text")
extern "C" void Ck_Log(); KING_ALIAS("_Ck_Log", "_VehLog_Value2")
extern "C" void Ck_Out(); KING_ALIAS("_Ck_Out", "_Vec3_OutOfBox")
extern "C" void Ck_Fatal(); KING_ALIAS("_Ck_Fatal", "_World_Fatal")
KING_VAR(Vec3, g_ckDown, 702640)
KING_VAR(Vec3, g_ckSide, 702620)
KING_VAR(Vec3, g_ckViewer, 6969f4)
KING_VAR(double, g_ckHi, 64d918)
KING_VAR(double, g_ckLo, 64da58)
KING_VAR(char, g_ckHead, 67debc)
KING_VAR(char, g_ckViewerFmt, 67de9c)
KING_VAR(char, g_ckTestM, 67de80)
KING_VAR(char, g_ckRow, 67de6c)
KING_VAR(char, g_ckMPos, 67de50)
KING_VAR(char, g_ckPlace, 67de2c)
KING_VAR(char, g_ckWhile, 67de10)
KING_VAR(char, g_ckFarHead, 67dde8)
KING_VAR(char, g_ckOldPos, 67dd70)
KING_VAR(char, g_ckFarM, 67ddcc)
KING_VAR(char, g_ckSix, 67ddbc)

static void Ck_Row(char* buf, const char* fmt, const float* v)
{
    sprintf(buf, fmt, (double)v[0], (double)v[1], (double)v[2]);
}

static void Ck_LogRow(const char* fmt, const float* v)
{
    char buf[500];
    Ck_Row(buf, fmt, v);
    CallC(Ck_Log, buf);
}

// a * the 3x3 part of m (rows of 3 floats; dot of a with column k)
static Vec3 Ck_Mul(const Vec3& a, const float* m)
{
    Vec3 r;
    r.x = (float)((double)a.z * m[6] + (double)a.y * m[3] + (double)a.x * m[0]);
    r.y = (float)((double)a.z * m[7] + (double)a.y * m[4] + (double)a.x * m[1]);
    r.z = (float)((double)a.z * m[8] + (double)a.y * m[5] + (double)a.x * m[2]);
    return r;
}

static bool Ck_Unit(double l)
{
    return !(l > g_ckHi) && !(l < g_ckLo);
}

extern "C" {

// [FUN_58e7f0..$L_58f1cd] moving item s's frame m (while doing `what`, with a code) checked: each axis turned by
// m, by s's frames +500 and +548 and by its node's local frame keeps unit length, and m's place is within 15000;
// else everything logged and a fatal error
void __fastcall MovingItem_CheckFrame(BYTE* s, void*, const float* m, const char* what, int code)
{
    float L[12];
    memcpy(L, g_7025f0, 48);
    CallThis(Ck_Local, F(BYTE*, F(BYTE*, s, 32), 68), L);
    const float* P = &F(float, s, 500);
    const float* Q = &F(float, s, 548);
    const char* room = F(const char*, F(BYTE*, F(BYTE*, s, 32), 64), 12);
    const char* owner = F(const char*, F(BYTE*, s, 800), 12);
    for (int c = 0; c < 3; c++)
    {
        Vec3 a = c == 0 ? g_ckDown : c == 1 ? g_ckSide : g_miAhead;
        double A = (double)a.x * m[0] + (double)a.z * m[6] + (double)a.y * m[3];
        double B = (double)a.x * m[1] + (double)a.z * m[7] + (double)a.y * m[4];
        double C = (double)a.x * m[2] + (double)a.z * m[8] + (double)a.y * m[5];
        Vec3 v1;
        v1.x = (float)((double)a.x * Q[0] + (double)a.z * Q[6] + (double)a.y * Q[3]);
        v1.y = (float)((double)a.z * Q[7] + (double)a.y * Q[4] + (double)a.x * Q[1]);
        v1.z = (float)((double)a.x * Q[2] + (double)a.z * Q[8] + (double)a.y * Q[5]);
        Vec3 v2 = Ck_Mul(a, P);
        Vec3 v3 = Ck_Mul(a, L);
        double lm = A * A + B * B + C * C;
        float l1 = (float)((double)v1.x * v1.x + (double)v1.y * v1.y + (double)v1.z * v1.z);
        float l2 = (float)((double)v2.z * v2.z + (double)v2.x * v2.x + (double)v2.y * v2.y);
        float l3 = (float)((double)v3.y * v3.y + (double)v3.z * v3.z + (double)v3.x * v3.x);
        if (Ck_Unit(lm) && Ck_Unit(l1) && Ck_Unit(l2) && Ck_Unit(l3))
            continue;
        CallC(Ck_LogOpen);
        char clock[100], buf[500];
        CallC(Ck_Clock, clock);
        sprintf(buf, &g_ckHead, clock, code, code, what, room, owner);
        CallC(Ck_Log, buf);
        Ck_LogRow(&g_ckViewerFmt, &g_ckViewer.x);
        Ck_LogRow(&g_ckTestM, m + 9);
        Ck_LogRow(&g_ckRow, m);
        Ck_LogRow(&g_ckRow, m + 3);
        Ck_LogRow(&g_ckRow, m + 6);
        Ck_LogRow(&g_ckMPos, P + 9);
        Ck_LogRow(&g_ckRow, P);
        Ck_LogRow(&g_ckRow, P + 3);
        Ck_LogRow(&g_ckRow, P + 6);
        Ck_LogRow(&g_ckPlace, L + 9);
        Ck_LogRow(&g_ckRow, L);
        Ck_LogRow(&g_ckRow, L + 3);
        Ck_LogRow(&g_ckRow, L + 6);
        CallC(Ck_LogClose);
        sprintf(buf, &g_ckWhile, what);
        CallC(Ck_Fatal, buf);
    }
    if (CallC<int>(Ck_Out, m + 9, 15000.0f))
    {
        char head[100], old[100], pos[100], r0[100], r1[100], r2[100], buf[500];
        sprintf(head, &g_ckFarHead, code, code, what, room, owner);
        Ck_Row(old, &g_ckOldPos, &F(float, s, 584));
        Ck_Row(pos, &g_ckFarM, m + 9);
        Ck_Row(r0, &g_ckRow, m);
        Ck_Row(r1, &g_ckRow, m + 3);
        Ck_Row(r2, &g_ckRow, m + 6);
        sprintf(buf, &g_ckSix, head, old, pos, r0, r1, r2);
        CallC(Ck_Fatal, buf);
    }
}

} // extern "C"

// === a net moving object's base made (FUN_5c5f60)

extern "C" void Nm_Base(); KING_ALIAS("_Nm_Base", "@NetObject_Create@88")
extern "C" void Nm_Root(); KING_ALIAS("_Nm_Root", "@Node_Root@8")
extern "C" void Nm_World(); KING_ALIAS("_Nm_World", "@Node_GetWorld@12")
extern "C" void Nm_Object(); KING_ALIAS("_Nm_Object", "@WeatherItem_Construct@84")
KING_VAR(AsmLabel, g_nm651c34, 651c34)
KING_VAR(AsmLabel, g_nm651be8, 651be8)

extern "C" {

// [FUN_5c5f60] object s: its scene object base (5a44c0) with a .. f, frame m and own, vtables 651c34 / 651be8
// (+256); for own == 1 its world frame (from the node +324) and its root (b) instead; then its net object
// (kind 903 for own == 1, else 900)
BYTE* __fastcall NetMoving_Construct(BYTE* s, void*, int a, int b, int c, int d, int e, int f, Mo_M48 m, int g,
                                     int own)
{
    CallThis(Nm_Object, s, a, b, c, d, e, f, m, own);
    F(void*, s, 0) = &g_nm651c34;
    F(void*, s, 256) = &g_nm651be8;
    if (own == 1)
    {
        b = CallThis<int>(Nm_Root, s + 256);
        CallThis(Nm_World, F(BYTE*, s, 324), &m);
    }
    CallThis(Nm_Base, s, a, b, c, d, e, f, m, g, own == 1 ? 903 : 900);
    return s;
}

} // extern "C"

// === a body's speed and spin after touching a surface (FUN_57e540 with its tails $L_57e7bf .. $L_57e9ca)

extern "C" void Sf_Set(); KING_ALIAS("_Sf_Set", "@Vec3_SetThis@20")
extern "C" void Sf_FrameTime(); KING_ALIAS("_Sf_FrameTime", "_Game_FrameTime")
extern "C" void Sf_Check(); KING_ALIAS("_Sf_Check", "_World_CheckPoint")
KING_VAR(double, g_sf64d9e0, 64d9e0)
KING_VAR(double, g_sf64b898, 64b898)
KING_VAR(double, g_sf64b7a8, 64b7a8)
KING_VAR(float, g_sf64c450, 64c450)
KING_VAR(float, g_sf64c264, 64c264)
KING_VAR(char, g_sfCheck, 67d1f0)
KING_VAR(char, g_sfSpeed, 67d1e0)
KING_VAR(char, g_sfSpin, 67d1d0)

// a x b as the asm's three SetThis calls compute it
static void Sf_Cross(float* out, const float* a, const float* b)
{
    CallThis(Sf_Set, out, (float)((double)a[1] * b[2] - (double)a[2] * b[1]),
             (float)((double)a[2] * b[0] - (double)a[0] * b[2]), (float)((double)a[0] * b[1] - (double)a[1] * b[0]));
}

extern "C" {

// [FUN_57e540..$L_57e9ca] body b touching contact c (its normal +64) in frame m: its speed (+416) split into the
// normal part, bounced by +1424, and the sliding part, slowed by friction; its spin (+428) damped by a power of
// the frame time, while sliding with the roll about the contact's axis (in m) set from the slide and +3280;
// both checked and given to b (vtable +144, +148)
void __cdecl Body_SurfaceResponse(BYTE* b, BYTE* c, const Matrix* m)
{
    float n[3], V[3], W[3];
    memcpy(n, c + 64, 12);
    memcpy(V, b + 416, 12);
    memcpy(W, b + 428, 12);
    float A[3], B[3], T[3];
    Sf_Cross(A, n, V);
    Sf_Cross(B, A, n);
    Vec3_Normalized((Vec3*)T, (const Vec3*)B);
    float dt = (float)CallC<double>(Sf_FrameTime);
    float vn = (float)((double)n[2] * V[2] + (double)V[1] * n[1] + (double)n[0] * V[0]);
    Sf_Cross(A, n, V);
    Sf_Cross(B, A, n);
    double bl = Vec3_Length((const Vec3*)B);
    double qn = -((double)vn * F(float, b, 1424));
    float q = (float)qn;
    float mu = (float)(bl - x87_fabs(qn - vn) * g_sf64d9e0);
    float Wn[3];
    if (!(mu > g_sf64c264))
    {
        mu = 0;
        double p = x87_pow(g_sf64b898, (double)dt * g_sf64c450);
        CallThis(Sf_Set, Wn, (float)(W[0] * p), (float)(W[1] * p), (float)(W[2] * p));
    }
    else
    {
        Matrix inv;
        Matrix_Inverse(&inv, m);
        const float* i = inv.m;
        float C[3], D[3], E[3];
        Sf_Cross(C, n, T);
        CallThis(Sf_Set, D, (float)((double)C[2] * i[6] + (double)C[1] * i[3] + (double)C[0] * i[0]),
                 (float)((double)C[2] * i[7] + (double)C[1] * i[4] + (double)C[0] * i[1]),
                 (float)((double)C[2] * i[8] + (double)C[1] * i[5] + (double)C[0] * i[2]));
        double dw = (double)D[2] * W[2] + (double)D[1] * W[1] + (double)D[0] * W[0];
        CallThis(Sf_Set, E, (float)(D[0] * dw), (float)(D[1] * dw), (float)(D[2] * dw));
        Vec3 Wp, a, d;
        Vec3_Sub(&Wp, (const Vec3*)W, (const Vec3*)E);
        float g = (float)((double)mu / F(float, b, 3280));
        float p2 = (float)x87_pow(g_sf64b7a8, (double)dt * g_sf64c450);
        Vec3_Scaled(&a, &Wp, p2);
        Vec3_Scaled(&d, (const Vec3*)D, g);
        Vec3_Add((Vec3*)Wn, &d, &a);
    }
    Vec3 nq, tm, nv;
    Vec3_Scaled(&nq, (const Vec3*)n, q);
    Vec3_Scaled(&tm, (const Vec3*)T, mu);
    Vec3_Add(&nv, &tm, &nq);
    CallC(Sf_Check, &nv, &g_sfCheck, 150.0f);
    CallVirt(b, 144, &nv, &g_sfSpeed);
    CallVirt(b, 148, Wn, &g_sfSpin);
}

} // extern "C"

// === two bodies' impulse at a contact (FUN_57eae0 with its tail $L_58084e)

extern "C" void Lw_Set(); KING_ALIAS("_Lw_Set", "@Vec3_SetThis@20")
extern "C" void Lw_MulA(); KING_ALIAS("_Lw_MulA", "@Vec3_MulAssign@16")
extern "C" void Lw_DivA(); KING_ALIAS("_Lw_DivA", "@Vec3_DivAssign@16")
extern "C" void Lw_AddA(); KING_ALIAS("_Lw_AddA", "@Vec3_AddAssign@16")
extern "C" float* __cdecl Lw_Negated(float* out, const float* v); KING_ALIAS("_Lw_Negated", "_Vec3_Negated")
extern "C" void __cdecl Lw_Check(const float* v, const char* name, float k); KING_ALIAS("_Lw_Check", "_World_CheckPoint")
extern "C" int __cdecl Lw_Out(const float* v, float k); KING_ALIAS("_Lw_Out", "_Vec3_OutOfBox")
extern "C" void __cdecl Lw_Soften(float* v); KING_ALIAS("_Lw_Soften", "_Vec3_Soften")
extern "C" void __cdecl Lw_Fatal(const char* msg); KING_ALIAS("_Lw_Fatal", "_World_Fatal")
KING_VAR(double, g_lw64b5b8, 64b5b8)
KING_VAR(double, g_lw64b658, 64b658)
KING_VAR(double, g_lw64b670, 64b670)
KING_VAR(double, g_lw64b678, 64b678)
KING_VAR(double, g_lw64b6e0, 64b6e0)
KING_VAR(double, g_lw64b7e8, 64b7e8)
KING_VAR(double, g_lw64b840, 64b840)
KING_VAR(float, g_lw64c250, 64c250)
KING_VAR(float, g_lw64c448, 64c448)
KING_VAR(float, g_lw64c450, 64c450)
KING_VAR(double, g_lw64d918, 64d918)
KING_VAR(float, g_lw64da74, 64da74)
KING_VAR(float, g_lw64f730, 64f730)
KING_VAR(float, g_lw696be0, 696be0)
KING_VAR(char, g_lw67d200, 67d200)
KING_VAR(char, g_lw67d214, 67d214)
KING_VAR(char, g_lw67d228, 67d228)
KING_VAR(char, g_lw67d23c, 67d23c)
KING_VAR(char, g_lw67d250, 67d250)
KING_VAR(char, g_lw67d2a0, 67d2a0)
KING_VAR(char, g_lw67d318, 67d318)
KING_VAR(char, g_lw67d390, 67d390)
KING_VAR(char, g_lw67d3c0, 67d3c0)
KING_VAR(char, g_lw67d3d8, 67d3d8)
KING_VAR(char, g_lw67d3f0, 67d3f0)
KING_VAR(char, g_lw67d408, 67d408)

#define V(n) (*(float*)(fr + (n)))
#define D(n) (*(double*)(fr + (n)))
#define I(n) (*(int*)(fr + (n)))

// the state of both bodies for the fatal message when an impulse ran out of range
static void Lw_Dump(BYTE* b, BYTE* c, BYTE* o, BYTE* fr)
{
    char* b1 = (char*)fr + 1868;
    char* b2 = (char*)fr + 2868;
    char* b3 = (char*)fr + 868;
    char* b4 = (char*)fr + 3868;
    sprintf(b1, &g_lw67d390, (double)F(float, c, 52), (double)F(float, c, 56), (double)F(float, c, 60),
            (double)F(float, c, 64), (double)F(float, c, 68), (double)F(float, c, 72), (double)V(240), D(180), D(68));
    D(52) = V(224);
    sprintf(b2, &g_lw67d318, F(const char*, F(BYTE*, b, 320), 12), (double)V(88), (double)V(92), (double)V(96),
            (double)V(156), (double)V(160), (double)V(164), D(52), D(52), (double)V(232), (double)F(float, b, 3300),
            (double)F(float, b, 3304), (double)F(float, b, 3308), (double)F(float, b, 3312), (double)F(float, b, 3316),
            (double)F(float, b, 3320), (double)V(460), (double)V(464), (double)V(468), (double)V(472), (double)V(476),
            (double)V(480), (double)V(484), (double)V(488), (double)V(492), (double)V(496), (double)V(500),
            (double)V(504));
    D(52) = V(256);
    sprintf(b3, &g_lw67d2a0, F(const char*, F(BYTE*, o, 320), 12), (double)V(100), (double)V(104), (double)V(108),
            (double)V(168), (double)V(172), (double)V(176), D(52), D(52), (double)V(264), (double)F(float, o, 3300),
            (double)F(float, o, 3304), (double)F(float, o, 3308), (double)F(float, o, 3312), (double)F(float, o, 3316),
            (double)F(float, o, 3320), (double)V(412), (double)V(416), (double)V(420), (double)V(424), (double)V(428),
            (double)V(432), (double)V(436), (double)V(440), (double)V(444), (double)V(448), (double)V(452),
            (double)V(456));
    sprintf(b4, &g_lw67d250, (double)V(340), (double)V(344), (double)V(348), (double)V(304), (double)V(308),
            (double)V(312), (double)V(280), (double)V(284), (double)V(288), (double)V(292), (double)V(296),
            (double)V(300), b1, b2, b3);
    Lw_Fatal(b4);
}

extern "C" {

// [FUN_57eae0..$L_58084e] bodies b and o meeting at contact c (point +52, normal +64): their speeds and spins at
// the point, the normal impulse from their masses (+3268), inertia (+3288) and restitution (696be0) and a slide
// against friction, both bodies' new speeds and spins (vtable +144 / +148), their energies kept from growing;
// fatal when a result leaves its range. The asm's frame is kept as fr (V float, D double, I int slots)
void __cdecl Bodies_Impulse(BYTE* b, BYTE* c, BYTE* o)
{
    BYTE fr[5900];
    memcpy(&V(460), b + 3172, 48);
    memcpy(&V(412), o + 3172, 48);
    float* P1 = (float*)Matrix_Inverse((Matrix*)(&V(556)), (const Matrix*)(&V(460)));
    float* P2 = (float*)Matrix_Inverse((Matrix*)(&V(772)), (const Matrix*)(&V(412)));
    V(12) = (float)((F(float, c, 52) - V(496)));
    I(316) = I(12);
    V(16) = (float)((F(float, c, 56) - V(500)));
    I(320) = I(16);
    V(20) = (float)((F(float, c, 60) - V(504)));
    I(324) = I(20);
    V(12) = (float)((F(float, c, 52) - V(448)));
    I(388) = I(12);
    V(16) = (float)((F(float, c, 56) - V(452)));
    I(392) = I(16);
    V(20) = (float)((F(float, c, 60) - V(456)));
    I(396) = I(20);
    I(24) = F(int, c, 64);
    I(28) = F(int, c, 68);
    I(240) = *(int*)&g_lw696be0;
    I(32) = F(int, c, 72);
    I(124) = F(int, b, 3268);
    I(48) = F(int, o, 3268);
    double RET4 = Vec3_Length((const Vec3*)&V(316));
    V(220) = (float)(RET4);
    double RET5 = Vec3_Length((const Vec3*)&V(388));
    V(216) = (float)(RET5);
    I(88) = F(int, b, 416);
    I(92) = F(int, b, 420);
    I(156) = F(int, b, 428);
    I(96) = F(int, b, 424);
    I(160) = F(int, b, 432);
    I(164) = F(int, b, 436);
    I(224) = F(int, b, 3288);
    I(228) = F(int, b, 3292);
    I(232) = F(int, b, 3296);
    I(100) = F(int, o, 416);
    V(132) = (float)((((V(156) * V(460)) + (V(160) * V(472))) + (V(164) * V(484))));
    I(104) = F(int, o, 420);
    I(168) = F(int, o, 428);
    I(108) = F(int, o, 424);
    I(172) = F(int, o, 432);
    V(136) = (float)((((V(156) * V(464)) + (V(160) * V(476))) + (V(164) * V(488))));
    I(176) = F(int, o, 436);
    V(140) = (float)((((V(156) * V(468)) + (V(160) * V(480))) + (V(164) * V(492))));
    V(180) = (float)((((V(168) * V(412)) + (V(172) * V(424))) + (V(176) * V(436))));
    I(256) = F(int, o, 3288);
    I(260) = F(int, o, 3292);
    V(184) = (float)((((V(168) * V(416)) + (V(172) * V(428))) + (V(176) * V(440))));
    I(264) = F(int, o, 3296);
    V(188) = (float)((((V(168) * V(420)) + (V(172) * V(432))) + (V(176) * V(444))));
    V(128) = (float)((((V(108) * V(108)) + (V(104) * V(104))) + (V(100) * V(100))));
    V(84) = (float)((((V(164) * V(164)) + (V(160) * V(160))) + (V(156) * V(156))));
    V(152) = (float)((((V(176) * V(176)) + (V(172) * V(172))) + (V(168) * V(168))));
    double qb = (((V(92) * V(92)) + (V(88) * V(88))) + (V(96) * V(96)));
    if (qb > g_lw64da74)
    {
        V(112) = (float)((V(88) / x87_sqrt((((V(92) * V(92)) + (V(88) * V(88))) + (V(96) * V(96))))));
        V(116) = (float)((V(92) / x87_sqrt((((V(92) * V(92)) + (V(88) * V(88))) + (V(96) * V(96))))));
        V(12) = (float)((V(112) * g_lw64c448));
        double t1 = (V(96) / x87_sqrt((((V(92) * V(92)) + (V(88) * V(88))) + (V(96) * V(96)))));
        I(88) = I(12);
        V(16) = (float)((V(116) * g_lw64c448));
        I(92) = I(16);
        V(20) = (float)((t1 * g_lw64c448));
        I(96) = I(20);
    }
    if (V(128) > g_lw64da74)
    {
        V(112) = (float)((V(100) / x87_sqrt(V(128))));
        V(116) = (float)((V(104) / x87_sqrt(V(128))));
        V(12) = (float)((V(112) * g_lw64c448));
        I(100) = I(12);
        V(16) = (float)((V(116) * g_lw64c448));
        I(104) = I(16);
        V(20) = (float)(((V(108) / x87_sqrt(V(128))) * g_lw64c448));
        I(108) = I(20);
    }
    if (V(84) > g_lw64f730)
    {
        V(112) = (float)((V(156) / x87_sqrt(V(84))));
        V(116) = (float)((V(160) / x87_sqrt(V(84))));
        V(12) = (float)((V(112) * g_lw64c450));
        I(156) = I(12);
        V(16) = (float)((V(116) * g_lw64c450));
        I(160) = I(16);
        V(20) = (float)(((V(164) / x87_sqrt(V(84))) * g_lw64c450));
        I(164) = I(20);
    }
    if (V(152) > g_lw64f730)
    {
        V(112) = (float)((V(168) / x87_sqrt(V(152))));
        V(116) = (float)((V(172) / x87_sqrt(V(152))));
        V(12) = (float)((V(112) * g_lw64c450));
        I(168) = I(12);
        V(16) = (float)((V(116) * g_lw64c450));
        I(172) = I(16);
        V(20) = (float)(((V(176) / x87_sqrt(V(152))) * g_lw64c450));
        I(176) = I(20);
    }
    Lw_Check(&V(88), &g_lw67d408, 150.0f);
    Lw_Check(&V(100), &g_lw67d3f0, 150.0f);
    Lw_Check(&V(156), &g_lw67d3d8, 30.0f);
    Lw_Check(&V(168), &g_lw67d3c0, 30.0f);
    V(16) = (float)((V(104) * V(48)));
    V(20) = (float)((V(108) * V(48)));
    V(116) = (float)((V(92) * V(124)));
    V(120) = (float)((V(96) * V(124)));
    V(72) = (float)((V(16) + V(116)));
    V(76) = (float)((V(20) + V(120)));
    V(84) = (float)((V(48) + V(124)));
    V(52) = (float)((((V(100) * V(48)) + (V(88) * V(124))) / V(84)));
    I(364) = I(52);
    V(56) = (float)((V(72) / V(84)));
    I(368) = I(56);
    V(60) = (float)((V(76) / V(84)));
    I(372) = I(60);
    CallThis(Lw_Set, &V(12), (float)((V(88) - V(52))), (float)((V(92) - V(56))), (float)((V(96) - V(60))));
    I(92) = I(16);
    I(88) = I(12);
    I(96) = I(20);
    CallThis(Lw_Set, &V(12), (float)((V(100) - V(364))), (float)((V(104) - V(368))), (float)((V(108) - V(372))));
    I(100) = I(12);
    I(104) = I(16);
    I(108) = I(20);
    if (((V(316) * V(24)) + (V(324) * V(32))) + (V(320) * V(28)) > g_lw64b678)
        CallThis(Lw_MulA, &V(24), &V(36), -1.0f);
    CallThis(Lw_Set, &V(52), (float)((V(316) / V(220))), (float)((V(320) / V(220))), (float)((V(324) / V(220))));
    CallThis(Lw_Set, &V(68), (float)((V(388) / V(216))), (float)((V(392) / V(216))), (float)((V(396) / V(216))));
    CallThis(Lw_Set, &V(196), (float)(((V(96) * V(28)) - (V(92) * V(32)))), (float)(((V(88) * V(32)) - (V(24) * V(96)))), (float)(((V(24) * V(92)) - (V(88) * V(28)))));
    CallThis(Lw_Set, &V(12), (float)(((V(200) * V(32)) - (V(204) * V(28)))), (float)(((V(24) * V(204)) - (V(196) * V(32)))), (float)(((V(196) * V(28)) - (V(24) * V(200)))));
    I(520) = I(12);
    I(524) = I(16);
    I(528) = I(20);
    CallThis(Lw_Set, &V(12), (float)(((((V(132) * V(52)) + (V(60) * V(140))) + (V(56) * V(136))) * V(52))), (float)(((((V(132) * V(52)) + (V(60) * V(140))) + (V(56) * V(136))) * V(56))), (float)(((((V(132) * V(52)) + (V(60) * V(140))) + (V(56) * V(136))) * V(60))));
    I(400) = I(12);
    I(404) = I(16);
    I(408) = I(20);
    CallThis(Lw_Set, &V(196), (float)(((V(28) * V(140)) - (V(32) * V(136)))), (float)(((V(132) * V(32)) - (V(24) * V(140)))), (float)(((V(24) * V(136)) - (V(132) * V(28)))));
    CallThis(Lw_Set, &V(112), (float)(((V(200) * V(32)) - (V(204) * V(28)))), (float)(((V(24) * V(204)) - (V(196) * V(32)))), (float)(((V(196) * V(28)) - (V(24) * V(200)))));
    CallThis(Lw_Set, &V(12), (float)(((V(120) * V(56)) - (V(116) * V(60)))), (float)(((V(112) * V(60)) - (V(120) * V(52)))), (float)(((V(116) * V(52)) - (V(112) * V(56)))));
    CallThis(Lw_Set, &V(36), (float)(((V(16) * V(60)) - (V(20) * V(56)))), (float)(((V(20) * V(52)) - (V(12) * V(60)))), (float)(((V(12) * V(56)) - (V(16) * V(52)))));
    I(328) = I(36);
    I(332) = I(40);
    I(336) = I(44);
    CallThis(Lw_Set, &V(36), (float)((V(132) - V(400))), (float)((V(136) - V(404))), (float)((V(140) - V(408))));
    CallThis(Lw_Set, &V(12), (float)((V(36) - V(328))), (float)((V(40) - V(332))), (float)((V(44) - V(336))));
    I(508) = I(12);
    I(512) = I(16);
    I(516) = I(20);
    CallThis(Lw_Set, &V(12), (float)(((V(108) * V(28)) - (V(104) * V(32)))), (float)(((V(100) * V(32)) - (V(24) * V(108)))), (float)(((V(24) * V(104)) - (V(100) * V(28)))));
    CallThis(Lw_Set, &V(36), (float)(((V(16) * V(32)) - (V(20) * V(28)))), (float)(((V(24) * V(20)) - (V(12) * V(32)))), (float)(((V(12) * V(28)) - (V(24) * V(16)))));
    I(532) = I(36);
    I(536) = I(40);
    I(540) = I(44);
    CallThis(Lw_Set, &V(36), (float)((V(68) * (((V(68) * V(180)) + (V(76) * V(188))) + (V(72) * V(184))))), (float)((V(72) * (((V(68) * V(180)) + (V(76) * V(188))) + (V(72) * V(184))))), (float)((V(76) * (((V(68) * V(180)) + (V(76) * V(188))) + (V(72) * V(184))))));
    I(376) = I(36);
    I(380) = I(40);
    I(384) = I(44);
    CallThis(Lw_Set, &V(12), (float)(((V(28) * V(188)) - (V(32) * V(184)))), (float)(((V(180) * V(32)) - (V(24) * V(188)))), (float)(((V(24) * V(184)) - (V(180) * V(28)))));
    CallThis(Lw_Set, &V(112), (float)(((V(16) * V(32)) - (V(20) * V(28)))), (float)(((V(24) * V(20)) - (V(12) * V(32)))), (float)(((V(12) * V(28)) - (V(24) * V(16)))));
    CallThis(Lw_Set, &V(132), (float)(((V(120) * V(72)) - (V(116) * V(76)))), (float)(((V(112) * V(76)) - (V(120) * V(68)))), (float)(((V(116) * V(68)) - (V(112) * V(72)))));
    CallThis(Lw_Set, &V(36), (float)(((V(136) * V(76)) - (V(140) * V(72)))), (float)(((V(140) * V(68)) - (V(132) * V(76)))), (float)(((V(132) * V(72)) - (V(136) * V(68)))));
    I(352) = I(36);
    I(356) = I(40);
    I(360) = I(44);
    CallThis(Lw_Set, &V(36), (float)((V(180) - V(376))), (float)((V(184) - V(380))), (float)((V(188) - V(384))));
    CallThis(Lw_Set, &V(12), (float)((V(36) - V(352))), (float)((V(40) - V(356))), (float)((V(44) - V(360))));
    I(544) = I(12);
    I(548) = I(16);
    I(552) = I(20);
    CallThis(Lw_Set, &V(36), (float)(((V(60) * V(28)) - (V(56) * V(32)))), (float)(((V(52) * V(32)) - (V(24) * V(60)))), (float)(((V(24) * V(56)) - (V(52) * V(28)))));
    I(244) = I(36);
    I(248) = I(40);
    I(252) = I(44);
    CallThis(Lw_Set, &V(36), (float)(((V(76) * V(28)) - (V(72) * V(32)))), (float)(((V(68) * V(32)) - (V(24) * V(76)))), (float)(((V(24) * V(72)) - (V(68) * V(28)))));
    I(268) = I(36);
    I(272) = I(40);
    I(276) = I(44);
    double RET35 = Vec3_Length((const Vec3*)&V(244));
    V(84) = (float)(RET35);
    double RET36 = Vec3_Length((const Vec3*)&V(268));
    V(152) = (float)(RET36);
    CallThis(Lw_Set, &V(36), (float)((-V(88))), (float)((-V(92))), (float)((-V(96))));
    I(128) = 1;
    V(180) = (float)((((V(40) * V(28)) + (V(44) * V(32))) + (V(24) * V(36))));
    if (!(((V(244) * V(328)) + (V(252) * V(336))) + (V(248) * V(332)) > g_lw64b678))
        I(128) = -1;
    double RET38 = Vec3_Length((const Vec3*)&V(328));
    V(132) = (float)((RET38 * I(128)));
    V(196) = (float)((((V(232) + V(228)) + V(224)) * g_lw64b7e8));
    CallThis(Lw_Set, &V(36), (float)((-V(100))), (float)((-V(104))), (float)((-V(108))));
    I(236) = 1;
    V(128) = (float)((((V(44) * V(32)) + (V(40) * V(28))) + (V(36) * V(24))));
    if (!(((V(268) * V(352)) + (V(276) * V(360))) + (V(272) * V(356)) > g_lw64b678))
        I(236) = -1;
    double RET40 = Vec3_Length((const Vec3*)&V(352));
    V(52) = (float)((RET40 * I(236)));
    V(68) = (float)((((V(264) + V(260)) + V(256)) * g_lw64b7e8));
    float* P3 = (float*)Matrix_ScaleColumns((Matrix*)(&V(820)), (const Matrix*)(&V(556)), (const Vec3*)(&V(224)));
    float* P4 = (float*)Matrix_Multiply((Matrix*)(&V(724)), (const Matrix*)(P3), (const Matrix*)(&V(460)));
    float* P5 = (float*)Matrix_ScaleColumns((Matrix*)(&V(820)), (const Matrix*)(&V(772)), (const Vec3*)(&V(256)));
    float* P6 = (float*)Matrix_Multiply((Matrix*)(&V(676)), (const Matrix*)(P5), (const Matrix*)(&V(412)));
    I(760) = ((const int*)&g_zeroVec)[0];
    I(712) = ((const int*)&g_zeroVec)[0];
    I(764) = ((const int*)&g_zeroVec)[1];
    I(768) = ((const int*)&g_zeroVec)[2];
    I(716) = ((const int*)&g_zeroVec)[1];
    I(720) = ((const int*)&g_zeroVec)[2];
    if (V(84) > g_lw64b658)
    {
        CallThis(Lw_DivA, &V(244), &V(36), V(84));
        CallThis(Lw_Set, &V(36), (float)((((V(232) * V(580)) + (V(228) * V(568))) + (V(556) * V(224)))), (float)((((V(232) * V(584)) + (V(228) * V(572))) + (V(224) * V(560)))), (float)((((V(232) * V(588)) + (V(228) * V(576))) + (V(224) * V(564)))));
        float* P7 = (float*)Vec3_RotateBy((Vec3*)(&V(36)), (const Vec3*)(&V(244)), (const Matrix*)(&V(724)));
        V(196) = (float)(x87_fabs((((F(float, P7, 8) * V(252)) + (F(float, P7, 4) * V(248))) + (V(244) * F(float, P7, 0)))));
    }
    double X;
    if (V(152) > g_lw64b658)
    {
        CallThis(Lw_DivA, &V(268), &V(36), V(152));
        float* P8 = (float*)Vec3_RotateBy((Vec3*)(&V(36)), (const Vec3*)(&V(256)), (const Matrix*)(&V(772)));
        double RET50 = Vec3_Dot((const Vec3*)(P8), (const Vec3*)(&V(268)));
        float* P9 = (float*)Vec3_RotateBy((Vec3*)(&V(36)), (const Vec3*)(&V(268)), (const Matrix*)(&V(676)));
        double RET52 = Vec3_Dot((const Vec3*)(P9), (const Vec3*)(&V(268)));
        X = x87_fabs(RET52);
    }
    else
        X = V(68);
    double t2 = (((V(84) * V(220)) * V(124)) / V(196));
    V(196) = (float)(t2);
    double t4 = (((V(152) * V(216)) * V(48)) / X);
    V(68) = (float)(t4);
    double t5 = ((((((((V(52) * V(152)) * V(216)) + V(128)) * (V(240) + g_lw64c250)) + (((((-V(240)) - g_lw64c250) * V(132)) * V(84)) * V(220))) + (((((((V(68) * V(152)) + g_lw64c250) * V(124)) / V(48)) + (V(196) * V(84))) - V(240)) * V(180))) / ((((((V(68) * V(152)) + g_lw64c250) * V(124)) / V(48)) + (V(196) * V(84))) + g_lw64c250)) * g_lw64b840);
    double t6 = (((((((((V(52) * V(152)) * V(216)) + V(128)) * (V(240) + g_lw64c250)) + (((((-V(240)) - g_lw64c250) * V(132)) * V(84)) * V(220))) + (((((((V(68) * V(152)) + g_lw64c250) * V(124)) / V(48)) + (V(196) * V(84))) - V(240)) * V(180))) / ((((((V(68) * V(152)) + g_lw64c250) * V(124)) / V(48)) + (V(196) * V(84))) + g_lw64c250)) * g_lw64b840) - V(180));
    V(180) = (float)(t6);
    V(236) = (float)((V(128) - ((V(124) / V(48)) * V(180))));
    float* P10 = (float*)Vec3_Scaled((Vec3*)(&V(616)), (const Vec3*)(&V(24)), (float)(x87_fabs(t5)));
    float* P11 = (float*)Vec3_Add((Vec3*)(&V(340)), (const Vec3*)(&V(616)), (const Vec3*)(&V(520)));
    float* P12 = (float*)Lw_Negated(&V(36), &V(24));
    float* P13 = (float*)Vec3_Scaled((Vec3*)(&V(604)), (const Vec3*)(P12), (float)(x87_fabs(V(236))));
    float* P14 = (float*)Vec3_Add((Vec3*)(&V(280)), (const Vec3*)(&V(604)), (const Vec3*)(&V(532)));
    float* P15 = (float*)Vec3_Scaled((Vec3*)(&V(640)), (const Vec3*)(&V(244)), (float)((((V(196) / V(220)) * V(180)) + V(132))));
    float* P16 = (float*)Vec3_Add((Vec3*)(&V(36)), (const Vec3*)(&V(400)), (const Vec3*)(&V(508)));
    float* P17 = (float*)Vec3_Add((Vec3*)(&V(664)), (const Vec3*)(P16), (const Vec3*)(&V(640)));
    float* P18 = (float*)Vec3_RotateBy((Vec3*)(&V(304)), (const Vec3*)(&V(664)), (const Matrix*)(&V(556)));
    float* P19 = (float*)Vec3_Scaled((Vec3*)(&V(628)), (const Vec3*)(&V(268)), (float)((((V(236) - V(128)) * (V(68) / V(216))) + V(52))));
    float* P20 = (float*)Vec3_Add((Vec3*)(&V(36)), (const Vec3*)(&V(376)), (const Vec3*)(&V(544)));
    float* P21 = (float*)Vec3_Add((Vec3*)(&V(652)), (const Vec3*)(P20), (const Vec3*)(&V(628)));
    float* P22 = (float*)Vec3_RotateBy((Vec3*)(&V(292)), (const Vec3*)(&V(652)), (const Matrix*)(&V(772)));
    CallThis(Lw_AddA, &V(340), &V(36), &V(364));
    CallThis(Lw_AddA, &V(280), &V(36), &V(364));
    float* P23 = (float*)Vec3_MulComponents((Vec3*)(&V(36)), (const Vec3*)(&V(224)), (const Vec3*)(b + 3312));
    double RET69 = Vec3_Dot((const Vec3*)(P23), (const Vec3*)(b + 3312));
    D(52) = RET69;
    double RET70 = Vec3_Dot((const Vec3*)(b + 3300), (const Vec3*)(b + 3300));
    V(68) = (float)((((RET70 * V(124)) + D(52)) * g_lw64b6e0));
    float* P24 = (float*)Vec3_MulComponents((Vec3*)(&V(36)), (const Vec3*)(&V(256)), (const Vec3*)(o + 3312));
    double RET72 = Vec3_Dot((const Vec3*)(P24), (const Vec3*)(o + 3312));
    D(52) = RET72;
    double RET73 = Vec3_Dot((const Vec3*)(o + 3300), (const Vec3*)(o + 3300));
    double t7 = (((RET73 * V(48)) + D(52)) * g_lw64b6e0);
    V(52) = (float)(t7);
    D(180) = V(124);
    float* P25 = (float*)Vec3_MulComponents((Vec3*)(&V(36)), (const Vec3*)(&V(224)), (const Vec3*)(&V(304)));
    double RET75 = Vec3_Dot((const Vec3*)(P25), (const Vec3*)(&V(304)));
    D(132) = RET75;
    double RET76 = Vec3_Dot((const Vec3*)(&V(340)), (const Vec3*)(&V(340)));
    double t8 = (((RET76 * D(180)) + D(132)) * g_lw64b6e0);
    V(132) = (float)(t8);
    double t9 = (V(52) + V(68));
    V(52) = (float)(t9);
    D(68) = V(48);
    float* P26 = (float*)Vec3_MulComponents((Vec3*)(&V(36)), (const Vec3*)(&V(256)), (const Vec3*)(&V(292)));
    double RET78 = Vec3_Dot((const Vec3*)(P26), (const Vec3*)(&V(292)));
    D(196) = RET78;
    double RET79 = Vec3_Dot((const Vec3*)(&V(280)), (const Vec3*)(&V(280)));
    V(48) = (float)((((((RET79 * D(68)) + D(196)) * g_lw64b6e0) + V(132)) / V(52)));
    if (V(52) > g_lw64b678 && V(48) > g_lw64d918)
    {
        double t10 = (g_lw64b670 / (V(48) + g_lw64b5b8));
        V(48) = (float)(t10);
        CallThis(Lw_MulA, &V(340), &V(36), V(48));
        CallThis(Lw_MulA, &V(280), &V(36), V(48));
        CallThis(Lw_MulA, &V(304), &V(36), V(48));
        CallThis(Lw_MulA, &V(292), &V(36), V(48));
    }
    Lw_Soften(&V(304));
    Lw_Soften(&V(292));
    if (Lw_Out(&V(340), 150.0f) || Lw_Out(&V(280), 150.0f) || Lw_Out(&V(304), 30.0f) || Lw_Out(&V(292), 30.0f))
        Lw_Dump(b, c, o, fr);
    CallVirt(b, 144, &V(340), &g_lw67d23c);
    CallVirt(b, 148, &V(304), &g_lw67d228);
    CallVirt(o, 144, &V(280), &g_lw67d214);
    CallVirt(o, 148, &V(292), &g_lw67d200);
}

} // extern "C"

#undef V
#undef D
#undef I
