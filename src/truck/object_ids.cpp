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

// === drawing an item and its marker (FUN_57d580)

extern "C" void Sh_Log(); KING_ALIAS("_Sh_Log", "_VehLog_Value")
extern "C" void Sh_GetLocal(); KING_ALIAS("_Sh_GetLocal", "@Node_GetLocal@12")
extern "C" void Sh_Draw(); KING_ALIAS("_Sh_Draw", "@SceneNode5f98c0_Draw@16")
extern "C" void Sh_sprintf(); KING_ALIAS("_Sh_sprintf", "_sprintf")
extern "C" void FUN_5b73a0(); KING_ALIAS("_FUN_5b73a0", "_Window_DrawLine")    // draws a line on a surface (x0, y0, x1, y1, colour, -)
ASM_VAR(int, g_sh696c6c, $L_696c6c)
ASM_VAR(int, g_sh696be4, $L_696be4)
ASM_VAR(BYTE*, g_sh6ef1fc, $L_6ef1fc)
ASM_VAR(float, g_sh64c250, $L_64c250)
ASM_VAR(float, g_sh64c258, $L_64c258)

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
ASM_VAR(BYTE*, g_iw6d2078, $L_6d2078)
ASM_VAR(float, g_iw6969f4, $L_6969f4)
ASM_VAR(float, g_iw6969f8, $L_6969f8)
ASM_VAR(float, g_iw6969fc, $L_6969fc)
ASM_VAR(float, g_iw64c450, $L_64c450)
ASM_VAR(AsmLabel, s_iwInfoWorld, $L_67d528)
ASM_VAR(AsmLabel, s_iwInfo, $L_67d4f4)
ASM_VAR(AsmLabel, s_iwSetW, $L_67d578)
ASM_VAR(AsmLabel, s_iwNewline, $L_66c4e4)
ASM_VAR(AsmLabel, s_iw696cbc, $L_696cbc)
ASM_VAR(AsmLabel, s_iwEmpty, $L_68d1c0)

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
ASM_VAR(AsmLabel, g_qs7025f0, $L_7025f0)

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

ASM_VAR(AsmLabel, s_iwSetV, $L_67d4d8)
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
ASM_VAR(double, g_rl64b658, $L_64b658)
ASM_VAR(char, s_rl67d1b8, $L_67d1b8)

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
