// moving_bodies.cpp - movingb.cpp of the game, rewritten from king.masm: small
// functions of its moving bodies: progress counters, planes, the point and
// record lists of a body, flags, the ListItem lists, and a free list.

#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

extern "C" void FUN_589640(); KING_ALIAS("_FUN_589640", "_Item_SweepPoints")
extern "C" void FUN_592cc0(); KING_ALIAS("_FUN_592cc0", "_Mb_Fatal")    // fatal error with a message

KING_VAR(AsmLabel, g_vtbl65089c, 65089c)
KING_VAR(AsmLabel, g_strPutOnHead, 67e088)    // "ListItem::put_on_head():this==NULL||next!=NULL||prev!=NULL"
KING_VAR(AsmLabel, g_strPutOnTail, 67e0c4)    // "ListItem::put_on_tail():this==NULL||next!=NULL||prev!=NULL"

struct Vec3Bits { DWORD x, y, z; };     // a Vec3 copied as it is
struct Record28 { DWORD m[7]; };

// ---------------------------------------------------------------------------
// A progress: +4 the step reached, +8 the number of steps, +12 the part done
// (0..1), +16 the part per second

extern "C" void __fastcall Progress_SetStep(BYTE* self, void*, int step)
{
    if (step < 0)
        return;
    int n = F(int, self, 8);
    if (step > n)
        return;
    F(int, self, 4) = step;
    F(float, self, 12) = (float)((double)step / n);
}

extern "C" void __fastcall Progress_SetPart(BYTE* self, void*, float part)
{
    if (!(part >= 0.0f))
        return;
    if (part > 1.0f)
        return;
    F(float, self, 12) = part;
    F(int, self, 4) = (int)(F(int, self, 8) * (double)part);
}

extern "C" void __fastcall Progress_Advance(BYTE* self, void*, float dt)
{
    double part = (double)dt * F(float, self, 16) + F(float, self, 12);
    F(float, self, 12) = (float)part;
    if (part > 1.0f)
        F(float, self, 12) = 1.0f;
    F(int, self, 4) = (int)(F(int, self, 8) * (double)F(float, self, 12));
}

// [FUN_589640] without flag 0x200, and with it and the identity matrix
extern "C" int __cdecl Movingb_589e70(int a, int b, int c, unsigned flags)
{
    return CallC<int>(FUN_589640, a, b, c, flags & ~0x200u);
}

extern "C" int __cdecl Movingb_589e90(int a, int b, unsigned flags)
{
    return CallC<int>(FUN_589640, a, &g_identity, b, flags | 0x200);
}

extern "C" BYTE* __fastcall TwoVec_Construct(BYTE* self, void*)
{
    F(int, self, 0) = 0;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 16) = 0;
    F(int, self, 20) = 0;
    F(int, self, 24) = 0;
    F(Vec3, self, 0) = g_zeroVec;
    F(int, self, 12) = 0;
    F(Vec3, self, 16) = g_zeroVec;
    return self;
}

// the plane (a, b, c, d) at +0 and its point -d * (a, b, c) at +16
extern "C" void __fastcall Plane_SetWithPoint(float* self, void*, const float* p)
{
    F(DWORD, self, 0) = F(DWORD, p, 0);
    F(DWORD, self, 4) = F(DWORD, p, 4);
    F(DWORD, self, 8) = F(DWORD, p, 8);
    self[3] = p[3];
    double minusD = -(double)p[3];
    self[4] = (float)(minusD * self[0]);
    self[5] = (float)(minusD * self[1]);
    self[6] = (float)(minusD * self[2]);
}

// ---------------------------------------------------------------------------
// Points of a body: up to 20 at +3592 (a value each at +3896, count +3572,
// read position +3580) and up to 4 at +3832 (an active flag each at +3880,
// count +3576, read position +3584); +3588 which list Points_Next reads

extern "C" int __fastcall Points_Add(BYTE* self, void*, int second, const Vec3Bits* p, int value)
{
    if (!second) {
        int n = F(int, self, 3572);
        if (n >= 20)
            return 0;
        F(Vec3Bits, self, 3592 + n * 12) = *p;
        F(int, self, 3896 + F(int, self, 3572) * 4) = value;
        return ++F(int, self, 3572);
    }
    int n = F(int, self, 3576);
    if (n >= 4)
        return 0;
    F(Vec3Bits, self, 3832 + n * 12) = *p;
    F(int, self, 3880 + F(int, self, 3576) * 4) = 1;
    return ++F(int, self, 3576);
}

extern "C" void __fastcall Points_ActivateAll(BYTE* self, void*)
{
    for (int i = 0; i < F(int, self, 3576); i++)
        F(int, self, 3880 + i * 4) = 1;
}

extern "C" int __fastcall Points_NextFirst(BYTE* self, void*, Vec3Bits* out)
{
    int i = F(int, self, 3580);
    if (i >= F(int, self, 3572)) {
        F(int, self, 3580) = 0;
        return 0;
    }
    *out = F(Vec3Bits, self, 3592 + i * 12);
    return ++F(int, self, 3580);
}

extern "C" int __fastcall Points_NextActive(BYTE* self, void*, Vec3Bits* out)
{
    int n = F(int, self, 3576);
    if (F(int, self, 3584) < n) {
        for (;;) {
            int i = F(int, self, 3584);
            if (F(int, self, 3880 + i * 4)) {
                *out = F(Vec3Bits, self, 3832 + F(int, self, 3584) * 12);
                return ++F(int, self, 3584);
            }
            F(int, self, 3584) = ++i;
            if (i >= n)
                break;
        }
    }
    F(int, self, 3584) = 0;
    return 0;
}

extern "C" int __fastcall Points_Next(BYTE* self, void*, Vec3Bits* out)
{
    if (F(int, self, 3588) == 0)
        return Points_NextFirst(self, 0, out);
    if (F(int, self, 3588) == 1)
        return Points_NextActive(self, 0, out);
    return 0;
}

// records of 28 bytes at +3988, their count at +5132, read position +5136
extern "C" int __fastcall Records_Next(BYTE* self, void*, Record28* out)
{
    int i = F(int, self, 5136);
    if (i >= F(int, self, 5132)) {
        F(int, self, 5136) = 0;
        return 0;
    }
    *out = F(Record28, self, 3988 + i * 28);
    int next = F(int, self, 5136) + 1;
    F(int, self, 5136) = next;
    return next;
}

extern "C" int __fastcall Records_Get(BYTE* self, void*, Record28* out, int i)
{
    if (i >= F(int, self, 5132) || i < 0)
        return 0;
    *out = F(Record28, self, 3988 + i * 28);
    return 1;
}

// an empty loop over the records in the original
extern "C" void __fastcall Records_58d290(BYTE* self, void*)
{
}

extern "C" void __fastcall Flags1196_Set(BYTE* self, void*, int bit)
{
    if (bit == 1)
        F(DWORD, self, 1196) |= 1;
    else if (bit == 2)
        F(DWORD, self, 1196) |= 2;
    else if (bit == 8)
        F(DWORD, self, 1196) |= 8;
}

extern "C" void __fastcall Flags1196_Clear(BYTE* self, void*, int bit)
{
    if (bit == 1)
        F(DWORD, self, 1196) &= ~1u;
    else if (bit == 2)
        F(DWORD, self, 1196) &= ~2u;
    else if (bit == 4)
        F(DWORD, self, 1196) &= ~4u;
    else if (bit == 8)
        F(DWORD, self, 1196) &= ~8u;
}

extern "C" void __fastcall Mode3976_Set(BYTE* self, void*, int mode)
{
    if (mode == 0 || mode == 1 || mode == 2)
        F(int, self, 3976) = mode;
    else if (mode == 3 && F(int, self, 3976) != 3)
        F(int, self, 3980) = 3;         // overwritten below, as in the original
    F(int, self, 3980) = 0;
}

// advances progress i of the array at +9520 (20 bytes each, the count at
// +9516); counts it in *changed when its step changed and in *finished
// when it reached the last
extern "C" int __fastcall Progresses_Advance(BYTE* self, void*, int i, float dt, int* changed, int* finished)
{
    BYTE* all = F(BYTE*, self, 9520);
    if (!all || !F(int, self, 9516) || i < 0 || i >= F(int, self, 9516))
        return 0;
    int step = F(int, all + i * 20, 4);
    Progress_Advance(all + i * 20, 0, dt);
    if (F(int, F(BYTE*, self, 9520) + i * 20, 4) == step)
        return 0;
    (*changed)++;
    BYTE* p = F(BYTE*, self, 9520) + i * 20;
    if (F(int, p, 4) == F(int, p, 8))
        (*finished)++;
    return 1;
}

// ---------------------------------------------------------------------------
// ListItem: a node (+0 next, +4 prev) at +4, the list's head pointer at +12

extern "C" BYTE* __fastcall ListNode_Construct(BYTE* self, void*)
{
    F(BYTE*, self, 0) = 0;
    F(BYTE*, self, 4) = 0;
    return self;
}

extern "C" void __fastcall ListNode_Unlink(BYTE* self, void*)
{
    if (F(BYTE*, self, 4))
        F(BYTE*, F(BYTE*, self, 4), 0) = F(BYTE*, self, 0);
    if (F(BYTE*, self, 0))
        F(BYTE*, F(BYTE*, self, 0), 4) = F(BYTE*, self, 4);
    F(BYTE*, self, 0) = 0;
    F(BYTE*, self, 4) = 0;
}

extern "C" void __fastcall ListItem_Attach(BYTE* self, void*, BYTE** list)
{
    if (!self || !list)
        return;
    BYTE* node = self + 4;
    ListNode_Unlink(node, 0);
    if (*list) {
        BYTE* next = F(BYTE*, *list, 0);
        if (next)
            F(BYTE*, next, 4) = node;
        F(BYTE*, node, 0) = F(BYTE*, *list, 0);
        F(BYTE*, *list, 0) = node;
        F(BYTE*, self, 8) = *list;
    }
    *list = node;
    F(BYTE**, self, 12) = list;
}

extern "C" BYTE* __fastcall ListItem592bf0_Construct(BYTE* self, void*, BYTE** list)
{
    ListNode_Construct(self + 4, 0);
    F(void*, self, 0) = g_vtbl65089c;
    ListItem_Attach(self, 0, list);
    return self;
}

extern "C" BYTE* __fastcall ListItem_PutOnHead(BYTE* self, void*, BYTE* item)
{
    if (!item)
        return self;
    if (!self || F(BYTE*, self, 4) || F(BYTE*, self, 0))
        CallC(FUN_592cc0, (const char*)g_strPutOnHead);
    while (F(BYTE*, item, 0))
        item = F(BYTE*, item, 0);
    F(BYTE*, item, 0) = self;
    F(BYTE*, self, 4) = item;
    return self;
}

extern "C" BYTE* __fastcall ListItem_PutOnTail(BYTE* self, void*, BYTE* item)
{
    if (!item)
        return self;
    if (!self || F(BYTE*, self, 4) || F(BYTE*, self, 0))
        CallC(FUN_592cc0, (const char*)g_strPutOnTail);
    while (F(BYTE*, item, 4))
        item = F(BYTE*, item, 4);
    F(BYTE*, item, 4) = self;
    F(BYTE*, self, 0) = item;
    return self;
}

extern "C" void __fastcall ListNode_InsertBefore(BYTE* self, void*, BYTE* node)
{
    if (!self)
        return;
    F(BYTE*, node, 0) = self;
    BYTE* prev = F(BYTE*, self, 4);
    F(BYTE*, node, 4) = prev;
    if (prev)
        F(BYTE*, prev, 0) = node;
    F(BYTE*, self, 4) = node;
}

extern "C" void __fastcall ListNode_CutNext(BYTE* self, void*)
{
    if (F(BYTE*, self, 0))
        F(BYTE*, F(BYTE*, self, 0), 4) = 0;
    F(BYTE*, self, 0) = 0;
}

extern "C" BYTE* __fastcall ListNode_592e90(BYTE* self, void*, BYTE* node)
{
    F(BYTE*, node, 0) = F(BYTE*, self, 0);
    if (F(BYTE*, self, 0))
        F(BYTE*, F(BYTE*, self, 0), 4) = node;
    BYTE* first = F(BYTE*, self, 4);
    BYTE* last = self;
    for (BYTE* p = first; p; p = F(BYTE*, p, 4))
        last = p;
    F(BYTE*, last, 4) = first;
    if (F(BYTE*, self, 4))
        F(BYTE*, F(BYTE*, self, 4), 0) = last;
    return last;
}

// deletes the chain at +4 (linked through +4 of each)
extern "C" void __fastcall Chain_DeleteAll(BYTE* self, void*)
{
    BYTE* p = F(BYTE*, self, 4);
    while (p) {
        BYTE* next = F(BYTE*, p, 4);
        operator delete(p);
        p = next;
    }
    F(BYTE*, self, 8) = 0;
    F(BYTE*, self, 4) = 0;
}

// v * the matrix of obj's current frame (+8224) of those at +2240
extern "C" void __cdecl Vec3_TransformByObject(Vec3* v, const BYTE* obj)
{
    Matrix m = F(Matrix, obj, 2240 + F(int, obj, 8224) * 48);
    v->TransformPoint(&m);
}

// ---------------------------------------------------------------------------
// A free list at +104 (linked through +0 of the items); +108 the last taken

extern "C" void __fastcall FreeList_Put(BYTE* self, void*, BYTE** item, BYTE** prevNext)
{
    if (!prevNext)
        F(BYTE*, self, 108) = *item;
    else
        *prevNext = *item;
    *item = F(BYTE*, self, 104);
    F(BYTE**, self, 104) = item;
}

extern "C" BYTE** __fastcall FreeList_Take(BYTE* self, void*)
{
    BYTE** p = F(BYTE**, self, 104);
    if (!p)
        return 0;
    F(BYTE**, self, 104) = (BYTE**)*p;
    *p = F(BYTE*, self, 108);
    F(BYTE**, self, 108) = p;
    return p;
}

extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")
extern "C" void FUN_5f1a70(); KING_ALIAS("_FUN_5f1a70", "@Node5f1a70_Destroy@4")
KING_VAR(char, g_vtbl6508a8, 6508a8)

extern "C" {

// [FUN_593040] constructor: a node with an 8000-byte buffer
BYTE* __fastcall Body593040_Construct(BYTE* self, void*, int name)
{
    CallThis(FUN_5f19b0, self, name, 11, 1);
    F(void*, self, 0) = &g_vtbl6508a8;
    F(void*, self, 56) = operator new(8000);
    return self;
}

// [FUN_5930a0] destructor
void __fastcall Body593040_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vtbl6508a8;
    if (F(void*, self, 56))
        operator delete(F(void*, self, 56));
    CallThis(FUN_5f1a70, self);
}

}

extern "C" void FUN_591c30(); KING_ALIAS("_FUN_591c30", "_Bodies_Overlap")
extern "C" void FUN_592350(); KING_ALIAS("_FUN_592350", "_Bodies_Touch")
extern "C" void FUN_5924d0(); KING_ALIAS("_FUN_5924d0", "_Bodies_TouchTest")

extern "C" {

// [FUN_5928d0] dispatches on the kinds (+392: 1 or 2) of the two bodies a
// and b; a kind-1 first body with a kind-2 second is handled swapped
int __cdecl Bodies_Pair(BYTE* a, int a1, int a2, BYTE* b, int b1, int b2, int x)
{
    if (!a || !b)
        return 0;
    int ka = F(int, a, 392), kb = F(int, b, 392);
    if (ka == 2 && kb == 2)
        return CallC<int>(FUN_591c30, a, a1, a2, b, b1, b2, x);
    if (ka == 1 && kb == 1)
        return CallC<int>(FUN_592350, a, a1, a2, b, b1, b2, x);
    if (ka == 2 && kb == 1)
        return CallC<int>(FUN_5924d0, a, a1, a2, b, b1, b2, x);
    if (kb == 2 && ka == 1)
        return CallC<int>(FUN_5924d0, b, b1, b2, a, a1, a2, x);
    return 0;
}

}

extern "C" void FUN_57cb70(); KING_ALIAS("_FUN_57cb70", "@MovingObj_ConstructListed@44")
extern "C" void FUN_57d060(); KING_ALIAS("_FUN_57d060", "@MovingObj_Construct@44")
extern "C" void FUN_58b4f0(); KING_ALIAS("_FUN_58b4f0", "@TwoVec_Construct@8")
extern "C" void FUN_58b8c0(); KING_ALIAS("_FUN_58b8c0", "@Body_Reset@16")
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
KING_VAR(char, g_vtbl650718, 650718)
KING_VAR(char, g_vtbl6506cc, 6506cc)
KING_VAR(char, g_vtbl6506c8, 6506c8)

static BYTE* Body_Init(BYTE* self, int a8, int a9)
{
    for (int i = 0; i < 20 * 3; i++)
        F(int, self, 3592 + i * 4) = 0;
    for (int i = 0; i < 4 * 3; i++)
        F(int, self, 3832 + i * 4) = 0;
    for (int i = 0; i < 20; i++)
        CallThis(FUN_58b4f0, self + 3988 + i * 28);
    for (int i = 0; i < 20; i++)
        CallThis(FUN_58b4f0, self + 4548 + i * 28);
    for (int i = 0; i < 9; i++)
        F(int, self, 5152 + i * 4) = 0;
    for (int i = 0; i < 20; i++)
        CallThis(FUN_5ef5c0, self + 5352 + i * 208);
    F(void*, self, 0) = &g_vtbl650718;
    F(void*, self, 256) = &g_vtbl6506cc;
    F(void*, self, 376) = &g_vtbl6506c8;
    CallThis(FUN_58b8c0, self, a8, a9);
    return self;
}

extern "C" {

// [FUN_58b5b0] constructor over the FUN_57cb70 base
BYTE* __fastcall Body58b5b0_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5, int a6,
                                      int a7, int a8, int a9, int a10, int a11)
{
    CallThis(FUN_57cb70, self, a1, a2, a3, a4, a5, a6, a7, a10, a11);
    return Body_Init(self, a8, a9);
}

// [FUN_58b720] constructor over the FUN_57d060 base
BYTE* __fastcall Body58b720_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5, int a6,
                                      int a7, int a8, int a9, int a10, int a11)
{
    CallThis(FUN_57d060, self, a1, a2, a3, a4, a5, a6, a7, a10, a11);
    return Body_Init(self, a8, a9);
}

}

#include <string.h>
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_587c80(); KING_ALIAS("_FUN_587c80", "@Track587c80_Construct@4")

// [FUN_592a20] the deepest flagged contact of a body: its points, normal and plane into out, its depth to *depth
extern "C" int __cdecl Body_DeepestContact(BYTE* obj, BYTE* out, float* depth)
{
    BYTE m[48];
    CallThis(FUN_57de90, obj, m);
    static BYTE c[6404];
    CallThis(FUN_587c80, c);
    if (!Movingb_589e90((int)obj, (int)c, 512))
        return 0;
    float best = 0.0f;
    int idx = 0;
    int n = F(int, c, 2816);
    for (int k = 0; k < n; k++)
        if (F(int, c, 2820 + k * 4) && !(best >= F(float, c, 2560 + k * 4))) {
            best = F(float, c, 2560 + k * 4);
            idx = k;
        }
    memcpy(out + 52, c + idx * 12, 12);
    memcpy(out + 64, c + 768 + idx * 12, 12);
    F(DWORD, out, 116) = F(DWORD, c, 1536 + idx * 4);
    F(DWORD, out, 100) = F(DWORD, c, 1792 + idx * 4);
    if (F(int, c, 2820 + idx * 4) == -1)
        memcpy(out + 4, c + 3332 + idx * 48, 48);
    *depth = best;
    return 1;
}

extern "C" double __cdecl Game_FrameTime();

// [FUN_58f320] the first of the six attached bodies within reach of this one, or 0
extern "C" BYTE* __fastcall Body_AttachedInReach(BYTE* self, void*)
{
    float px = F(float, self, 536), py = F(float, self, 540), pz = F(float, self, 544);
    Game_FrameTime();
    for (int k = 0; k < 6; k++) {
        BYTE* o = F(BYTE*, self, 3508 + k * 4);
        if (!o)
            return 0;
        BYTE mb[48];
        const float* m = (const float*)CallThis<BYTE*>(FUN_57de90, o, mb);
        float ox = F(float, o, 3436), oy = F(float, o, 3440), oz = F(float, o, 3444);
        double wx = ((double)m[6] * oz + (double)m[3] * oy) + (double)ox * m[0] + m[9];
        float wy = (float)(((double)m[1] * ox + (double)m[7] * oz) + (double)m[4] * oy + m[10]);
        float wz = (float)(((double)m[2] * ox + (double)m[8] * oz) + (double)m[5] * oy + m[11]);
        o = F(BYTE*, self, 3508 + k * 4);
        float dx = (float)(wx - px);
        float dy = (float)((double)wy - py);
        double dz = (double)wz - pz;
        double sum = ((double)dx * dx + (double)dy * dy) + dz * dz;
        double r1 = (double)F(float, o, 3272) + F(float, self, 3280);
        float r2 = (float)((double)F(float, o, 3280) + F(float, self, 3272));
        if (!(sum > r1 * r1))
            return F(BYTE*, self, 3508 + k * 4);
        if (!(sum >= (double)r2 * r2))
            return F(BYTE*, self, 3508 + k * 4);
    }
    return 0;
}

// [FUN_58f480] whether a point (body space) is inside all of the body's planes
extern "C" int __fastcall Body_ContainsPoint(BYTE* s, void*, const float* p)
{
    float qx = (float)(((double)F(float, s, 608) * p[1] + (double)F(float, s, 620) * p[2]) + (double)F(float, s, 596) * p[0] + F(float, s, 632));
    float qy = (float)(((double)F(float, s, 612) * p[1] + (double)F(float, s, 600) * p[0]) + (double)F(float, s, 624) * p[2] + F(float, s, 636));
    float qz = (float)(((double)F(float, s, 616) * p[1] + (double)F(float, s, 604) * p[0]) + (double)F(float, s, 628) * p[2] + F(float, s, 640));
    int n = F(int, s, 5132);
    for (int i = 0; i < n; i++) {
        const float* c = (const float*)(s + 3988 + i * 28);
        double dot = ((double)c[0] * qx + (double)c[2] * qz) + (double)c[1] * qy;
        if (!(-(double)c[3] >= dot))
            return 0;
    }
    return 1;
}

extern "C" void FUN_57dec0(); KING_ALIAS("_FUN_57dec0", "@SceneObj_Matrix2@12")

// [FUN_589eb0] whether the segment a-b (body space) can touch the body's box in x/y (0.01 margin)
extern "C" int __fastcall Body_SegmentNearBox(BYTE* s, void*, const float* a, const float* b)
{
    float m[12];
    CallThis(FUN_57dec0, s, m);
    float ax = (float)(((double)m[0] * a[0] + (double)m[6] * a[2]) + (double)m[3] * a[1] + m[9]);
    double ay = ((double)m[1] * a[0] + (double)m[7] * a[2]) + (double)m[4] * a[1] + m[10];
    float bx = (float)(((double)m[6] * b[2] + (double)m[3] * b[1]) + (double)m[0] * b[0] + m[9]);
    float by = (float)(((double)m[7] * b[2] + (double)m[4] * b[1]) + (double)m[1] * b[0] + m[10]);
    double c1 = (double)F(float, s, 5116) + 0.01;
    float c2 = (float)((double)F(float, s, 5120) - 0.01);
    float c3 = (float)((double)F(float, s, 5108) - 0.01);
    float c4 = (float)((double)F(float, s, 5112) + 0.01);
    if (ay > c1 && by > c1) return 0;
    if (!(ay >= c2) && !(by >= c2)) return 0;
    if (!(ax >= c3) && !(bx >= c3)) return 0;
    if (ax > c4 && bx > c4) return 0;
    return 1;
}

// [FUN_592350] whether two attached bodies touch: centres within the sum of the outer radii and closer than the sum of the inner
extern "C" int __cdecl Bodies_Touch(BYTE* d, const float* n, int, BYTE* c, const float* m)
{
    float cx = F(float, c, 3436), cy = F(float, c, 3440), cz = F(float, c, 3444);
    double px = ((double)m[6] * cz + (double)m[3] * cy) + (double)cx * m[0] + m[9];
    float py = (float)(((double)m[1] * cx + (double)m[7] * cz) + (double)m[4] * cy + m[10]);
    float pz = (float)(((double)m[2] * cx + (double)m[8] * cz) + (double)m[5] * cy + m[11]);
    float ex = F(float, d, 3436), ey = F(float, d, 3440), ez = F(float, d, 3444);
    double qx = ((double)n[3] * ey + (double)n[6] * ez) + (double)n[0] * ex + n[9];
    float qy = (float)(((double)n[4] * ey + (double)n[7] * ez) + (double)n[1] * ex + n[10]);
    float qz = (float)(((double)n[5] * ey + (double)n[8] * ez) + (double)n[2] * ex + n[11]);
    float dx = (float)(qx - px);
    float dy = (float)((double)qy - py);
    double dz = (double)qz - pz;
    double sum = ((double)dx * dx + (double)dy * dy) + dz * dz;
    double r1 = (double)F(float, d, 3272) + F(float, c, 3272);
    if (sum > r1 * r1)
        return 0;
    double r2 = (double)F(float, d, 3280) + F(float, c, 3280);
    if (!(sum >= r2 * r2))
        return 1;
    return 0;
}


KING_VAR(Vec3, g_gravity702650, 702650)
KING_VAR(Vec3, g_axis702630, 702630)
extern "C" void FUN_58ba90(); KING_ALIAS("_FUN_58ba90", "@Body_Clear9516@8")

// [FUN_58b8c0] resets a body's state: flags, contacts, the 20 contact records, the half size
extern "C" int __fastcall Body_Reset(BYTE* s, void*, int a, int b)
{
    F(DWORD, s, 1196) |= 9;
    F(DWORD, s, 1204) = 0;
    F(DWORD, s, 3568) = 0;
    F(DWORD, s, 392) = 2;
    for (int o = 3572; o <= 3588; o += 4) F(DWORD, s, o) = 0;
    F(DWORD, s, 3976) = 3;
    F(DWORD, s, 3980) = 0;
    F(DWORD, s, 3984) = 0;
    F(DWORD, s, 892) = 0;
    F(DWORD, s, 896) = 0;
    F(DWORD, s, 5132) = 0;
    F(DWORD, s, 5136) = 0;
    F(DWORD, s, 5140) = 0;
    F(Vec3, s, 1120) = g_gravity702650;
    F(Vec3, s, 964) = g_gravity702650;
    F(Vec3, s, 976) = g_gravity702650;
    F(DWORD, s, 1028) = 0;
    F(DWORD, s, 1032) = 0;
    F(int, s, 1424) = a;
    F(int, s, 1428) = b;
    for (int k = 0; k < 20; k++) {
        F(DWORD, s, 5192 + k * 4) = 0;
        F(DWORD, s, 5272 + k * 4) = 0;
        BYTE* r = s + 5416 + k * 208;
        Vec3 v;
        v.x = (float)((double)g_axis702630.x * -1000.0f);
        v.y = (float)((double)g_axis702630.y * -1000.0f);
        v.z = (float)((double)g_axis702630.z * -1000.0f);
        F(Vec3, r, -12) = v;
        F(Vec3, r, 0) = g_axis702630;
        F(DWORD, r, 44) = 0;
        F(DWORD, r, 36) = 0;
    }
    float h = (float)((double)F(float, s, 400) * 0.5f);
    F(float, s, 3276) = h;
    F(float, s, 3280) = h;
    F(float, s, 3284) = h;
    return CallThis<int>(FUN_58ba90, s);
}

extern "C" int __cdecl Query_4e7168(int a, BYTE* skip, float dist, const float* p, const float* d, BYTE* q);
KING_VAR(Vec3, g_down702630, 702630)

// [FUN_590ba0] probes the ground 5 above to 95 below the body's position; on a hit, the body's virtual +224 (point, normal, a)
extern "C" void __fastcall Body_ProbeGround(BYTE* s, void*, int a)
{
    if (!F(int, s, 5132))
        return;
    BYTE node[208];
    CallThis(FUN_5ef5c0, node);
    const Vec3& g = g_down702630;
    double x5 = (double)g.x * 5.0f;
    float y5 = (float)((double)g.y * 5.0f);
    float z5 = (float)((double)g.z * 5.0f);
    float A[3], B[3];
    A[0] = (float)(x5 + F(float, s, 536));
    A[1] = (float)((double)y5 + F(float, s, 540));
    A[2] = (float)((double)z5 + F(float, s, 544));
    double wx = (double)g.x * 100.0f;
    float wy = (float)((double)g.y * 100.0f);
    float wz = (float)((double)g.z * 100.0f);
    B[0] = (float)((double)A[0] - wx);
    B[1] = (float)((double)A[1] - wy);
    B[2] = (float)((double)A[2] - wz);
    if (Query_4e7168(F(int, s, 800), s, 0.0f, A, B, node))
        CallVirt(s, 224, node + 52, node + 64, a);
}

// [FUN_589490] moves a body's level (+12, -1..1) by *amount at rate towards a notch of +8; 1 when the amount was used up
extern "C" int __fastcall BodyLevel_Step(BYTE* s, void*, float* amount, float rate)
{
    if (!(*amount < 0 || *amount > 0))
        return 0;
    if (!(rate < 0 || rate > 0)) {
        F(int, s, 12) = 0;
        F(int, s, 4) = 0;
        return 1;
    }
    double q = (double)rate * F(float, s, 12);
    float qf = (float)q;
    if (*amount > 0.0f) {
        double aq = q < 0 ? -q : q;
        if (aq > *amount) {
            double t = (double)*amount / rate;
            if (qf > 0.0f) {
                double v = F(float, s, 12) - t;
                F(float, s, 12) = (float)v;
                if (!(v >= 0.0f))
                    F(int, s, 12) = 0;
            } else {
                double v = t + F(float, s, 12);
                F(float, s, 12) = (float)v;
                if (v > 0.0f)
                    F(int, s, 12) = 0;
            }
            F(int, s, 4) = (int)((double)F(int, s, 8) * F(float, s, 12));
            *(int*)amount = 0;
            return 1;
        }
        *amount = (float)((double)*amount - aq);
        F(int, s, 12) = 0;
        F(int, s, 4) = 0;
        return 0;
    }
    double l = F(float, s, 12);
    double u = (1.0 - (l < 0 ? -l : l)) * rate;
    double am = *amount < 0 ? -(double)*amount : (double)*amount;
    if (!(am >= u)) {
        double t = (double)*amount / rate;
        if (q > 0.0f) {
            double v = F(float, s, 12) - t;
            F(float, s, 12) = (float)v;
            if (v > 1.0f)
                F(float, s, 12) = 1.0f;
        } else {
            double v = t + F(float, s, 12);
            F(float, s, 12) = (float)v;
            if (!(v >= -1.0f))
                F(float, s, 12) = -1.0f;
        }
        F(int, s, 4) = (int)((double)F(int, s, 8) * F(float, s, 12));
        *(int*)amount = 0;
        return 1;
    }
    *amount = (float)((u < 0 ? -u : u) + *amount);
    F(int, s, 4) = F(int, s, 8);
    F(float, s, 12) = q > 0.0f ? 1.0f : -1.0f;
    return 0;
}

extern "C" int __cdecl Body_DeepestContact(BYTE* obj, BYTE* out, float* depth);
extern "C" void* __cdecl Query_HitObject(void* obj, const void* p, const void* d);
extern "C" Vec3* __fastcall Vec3_AddAssign(Vec3* self, void*, Vec3* ret, const Vec3* v);

// [FUN_58cf60] switches a body's contact mode; mode 0 pushes the body out of its deepest contacts (up to 3 steps)
extern "C" void __fastcall Body_SetContactMode(BYTE* s, void*, int mode)
{
    if (F(int, s, 3984))
        mode = 0;
    if (mode == F(int, s, 3588))
        return;
    if (mode == 1) {
        F(int, s, 3588) = 1;
        return;
    }
    BYTE node[208];
    CallThis(FUN_5ef5c0, node);
    Matrix m0 = F(Matrix, s, 500);
    int room = F(int, s, 800);
    int i;
    for (i = 0; i < 3; i++) {
        float depth;
        if (!Body_DeepestContact(s, node, &depth))
            break;
        double k = depth + 0.01;
        Vec3 v;
        v.x = (float)(F(float, node, 64) * k);
        v.y = (float)(F(float, node, 68) * k);
        v.z = (float)(F(float, node, 72) * k);
        Matrix m1 = F(Matrix, s, 500);
        Vec3 ret;
        Vec3_AddAssign((Vec3*)((BYTE*)&m1 + 36), 0, &ret, &v);
        void* r = Query_HitObject(F(void**, s, 800), s + 536, (BYTE*)&m1 + 36);
        if (!r) {
            r = F(void*, s, 800);
            CallVirt(s, 28, 1);
        }
        CallVirt(s, 140, r, &m1);
    }
    if (i == 3) {
        CallVirt(s, 140, room, &m0);
        return;
    }
    F(int, s, 3588) = mode;
}

extern "C" void FUN_5917f0(); KING_ALIAS("_FUN_5917f0", "_Body_PointContact")    // a contact of a point with the body (point, -, contact, -)
extern "C" Vec3* __cdecl Line_Project(Vec3* out, const Vec3* p, const Vec3* n, const Vec3* a, const Vec3* b);

// [FUN_591b50] a contact of the segment a-b: at a when b goes into the plane, or at b projected onto the plane
extern "C" int __cdecl Body_SegmentContact(const Vec3* a, const Vec3* b, int c, BYTE* contact, int e)
{
    int r = CallC<int>(FUN_5917f0, a, c, contact, e);
    if (r) {
        F(Vec3, contact, 52) = *a;
        double dx = (double)b->x - a->x, dy = (double)b->y - a->y, dz = (double)b->z - a->z;
        double dot = ((double)F(float, contact, 72) * dz + (double)F(float, contact, 68) * dy) + (double)F(float, contact, 64) * dx;
        if (!(dot >= 0.0f))
            return r;
        return 0;
    }
    r = CallC<int>(FUN_5917f0, b, c, contact, e);
    if (!r)
        return 0;
    Vec3 t;
    Line_Project(&t, (Vec3*)(contact + 52), (Vec3*)(contact + 64), a, b);
    F(Vec3, contact, 52) = t;
    return r;
}

extern "C" void FUN_57c9e0(); KING_ALIAS("_FUN_57c9e0", "_Moving_Of")

// [FUN_5893d0] forwards to FUN_57c9e0
extern "C" int __cdecl Body_5893d0(int a)
{
    return CallC<int>(FUN_57c9e0, a);
}

// [FUN_58ba90] clears +9516 and +9520, returns 0
extern "C" int __fastcall Body_Clear9516(BYTE* self, int)
{
    *(int*)(self + 9520) = 0;
    *(int*)(self + 9516) = 0;
    return 0;
}

KING_VAR(char, vt_650718, 650718)
KING_VAR(char, vt_6506cc, 6506cc)
KING_VAR(char, vt_6506c8, 6506c8)
extern "C" void FUN_57d490(); KING_ALIAS("_FUN_57d490", "@MovingItem_Destroy@4")

// [FUN_58b870] destructor: frees the owned buffer at +9520
extern "C" void __fastcall Body58b870_Destroy(BYTE* self, int)
{
    void* p = *(void**)(self + 9520);
    *(void**)self = &vt_650718;
    *(void**)(self + 256) = &vt_6506cc;
    *(void**)(self + 376) = &vt_6506c8;
    if (p && *(int*)(self + 9516))
        operator delete(p);
    CallThis(FUN_57d490, self);
}

KING_VAR(char, vt_65094c, 65094c)
KING_VAR(char, vt_650900, 650900)
extern "C" void FUN_5ef780(); KING_ALIAS("_FUN_5ef780", "@Node_Remove@8")
extern "C" void FUN_5d33b0(); KING_ALIAS("_FUN_5d33b0", "@Handle_Release@8")
extern "C" void FUN_594d70(); KING_ALIAS("_FUN_594d70", "_Faders_DeleteAll")    // deletes all faders
extern "C" void FUN_5f1a70(); KING_ALIAS("_FUN_5f1a70", "@Node5f1a70_Destroy@4")
extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")

// [FUN_593670] destructor of a fading body (vtables 65094c, 650900 at +32)
extern "C" void __fastcall FadeBody_Destroy(BYTE* self, int)
{
    BYTE* sub = self + 32;
    *(void**)self = &vt_65094c;
    *(void**)sub = &vt_650900;
    operator delete(F(void*, self, 112));
    BYTE* a = F(BYTE*, self, 100);
    // the element destructor (FUN_50d190) does nothing
    if (a)
        operator delete(a - 4);
    CallThis(FUN_5ef780, sub);
    CallThis(FUN_5d33b0, self);
    CallC(FUN_594d70);
    CallThis(FUN_5f1a70, self ? sub : 0);
    CallThis(FUN_5d3290, self);
}

// ---------------------------------------------------------------------------
KING_VAR(AsmLabel, s_67deec, 67deec)
extern "C" void __cdecl World_Fatal(const char* msg);

// [FUN_58f290..$L_58f2c2] sets mode 0/1 at +3976, or 2..3 at +3980 (fatal for others)
extern "C" void __fastcall Body_SetMoveMode(BYTE* self, void*, int v)
{
    if (v == 0 || v == 1)
    {
        F(int, self, 3976) = v;
        return;
    }
    if (v < 0 || v > 3)
        World_Fatal(s_67deec);
    F(int, self, 3980) = v;
}


// ---------------------------------------------------------------------------
typedef void (__cdecl* MbFail)(const char*);
KING_VAR(MbFail, g_mbFail6f6634, 6f6634)
extern "C" int __cdecl printf(const char* format, ...);
extern "C" __declspec(noreturn) void __cdecl exit(int code);

// [FUN_592cc0] a fatal error: the hook when set, else prints the text and exits
extern "C" void __cdecl Mb_Fatal(const char* text)
{
    if (g_mbFail6f6634)
    {
        g_mbFail6f6634(text);
        return;
    }
    printf(text);
    exit(0);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_592d70(); KING_ALIAS("_FUN_592d70", "@ListNode_Construct@8")
extern "C" void FUN_592d80(); KING_ALIAS("_FUN_592d80", "@ListItem_PutOnHead@12")
extern "C" void FUN_592dd0(); KING_ALIAS("_FUN_592dd0", "@ListItem_PutOnTail@12")
KING_VAR(AsmLabel, g_vt65089c, 65089c)
KING_VAR(AsmLabel, s_mb67e068, 67e068)

// [FUN_592c40] constructs a holder of the pointer *p (+12 = p): chains onto the existing one (front, or after with flag) or becomes it
extern "C" BYTE* __fastcall Holder_Construct(BYTE* self, void*, BYTE** p, int after)
{
    BYTE* link = self + 4;
    CallThis(FUN_592d70, link);
    F(void*, self, 0) = g_vt65089c;
    if (!p)
        Mb_Fatal(s_mb67e068);
    if (*p)
    {
        if (!after)
            *p = CallThis<BYTE*>(FUN_592d80, link, *p);
        else
            CallThis(FUN_592dd0, link, *p);
    }
    else
        *p = self ? link : 0;
    F(BYTE**, self, 12) = p;
    return self;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_592cf0(); KING_ALIAS("_FUN_592cf0", "@Linked592cf0_Destruct@8")
extern "C" void FUN_5930a0(); KING_ALIAS("_FUN_5930a0", "@Body593040_Destroy@8")
KING_VAR(DWORD, g_movbod6f6638, 6f6638)
KING_VAR(DWORD, g_movbod6f663c, 6f663c)
KING_VAR(DWORD, g_movbod6f6640, 6f6640)
KING_VAR(DWORD, g_movbod6f6648, 6f6648)
KING_VAR(DWORD, g_movbod6f664c, 6f664c)
KING_VAR(DWORD, g_movbod6f6650, 6f6650)
KING_VAR(DWORD, g_movbod6f6654, 6f6654)
KING_VAR(DWORD, g_movbod6f6658, 6f6658)
KING_VAR(DWORD, g_movbod6f665c, 6f665c)
KING_VAR(DWORD, g_movbod6f6660, 6f6660)
KING_VAR(DWORD, g_movbod6f6664, 6f6664)
KING_VAR(DWORD, g_movbod6f6668, 6f6668)
KING_VAR(DWORD, g_movbod6f666c, 6f666c)
KING_VAR(DWORD, g_movbod6f6670, 6f6670)
KING_VAR(DWORD, g_movbod6f6674, 6f6674)
KING_VAR(DWORD, g_movbod6f6678, 6f6678)
KING_VAR(DWORD, g_movbod6f667c, 6f667c)
KING_VAR(DWORD, g_movbod6f6680, 6f6680)
KING_VAR(DWORD, g_movbod6f6684, 6f6684)
KING_VAR(DWORD, g_movbod6f6688, 6f6688)
KING_VAR(DWORD, g_movbod6f668c, 6f668c)
KING_VAR(DWORD, g_movbod6f6690, 6f6690)
KING_VAR(DWORD, g_movbod6f6694, 6f6694)
KING_VAR(DWORD, g_movbod6f6698, 6f6698)
KING_VAR(DWORD, g_movbod6f669c, 6f669c)
KING_VAR(DWORD, g_movbod6f66a0, 6f66a0)
KING_VAR(DWORD, g_movbod6f66a4, 6f66a4)

static BYTE* Movbod_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_593010]
extern "C" int __fastcall Movbod_593010(BYTE* s, void*, int, int, int) { return 1; }
// [$L_592c20]
extern "C" BYTE* __fastcall Movbod592c20_DeletingDestructor(BYTE* s, void*, int flags) { return Movbod_DeleteAfter(s, flags, FUN_592cf0); }
// [$L_593020]
extern "C" BYTE* __fastcall Movbod593020_DeletingDestructor(BYTE* s, void*, int flags) { return Movbod_DeleteAfter(s, flags, FUN_5930a0); }
// [$L_593280]
extern "C" void __cdecl MovbodInit_593280() { g_movbod6f6638 = 0x0; g_movbod6f663c = 0x0; g_movbod6f6640 = 0xc099eb85; }
// [$L_5932b0]
extern "C" void __cdecl MovbodInit_5932b0() { g_movbod6f6668 = 0x3f800000; g_movbod6f666c = 0x3f800000; g_movbod6f6670 = 0x3f73f3f4; g_movbod6f6674 = 0x3f4ccccd; }
// [$L_5933b0]
extern "C" void __cdecl MovbodInit_5933b0() { g_movbod6f6658 = 0x3f800000; g_movbod6f665c = 0x3f800000; g_movbod6f6660 = 0x3f70f0f1; g_movbod6f6664 = 0x3f3ebebf; }
// [$L_5933f0]
extern "C" void __cdecl MovbodInit_5933f0() { g_movbod6f6648 = 0x0; g_movbod6f664c = 0x3f7efeff; g_movbod6f6650 = 0x3f44c4c5; g_movbod6f6654 = 0x3eeeeeef; }
// [$L_5932f0]
extern "C" void __cdecl MovbodInit_5932f0() { g_movbod6f6688 = 0x3f800000; g_movbod6f668c = 0x3f800000; g_movbod6f6690 = 0x3f5fdfe0; g_movbod6f6694 = 0x3efefeff; }
// [$L_593330]
extern "C" void __cdecl MovbodInit_593330() { g_movbod6f6678 = 0x0; g_movbod6f667c = 0x3f77f7f8; g_movbod6f6680 = 0x3f42c2c3; g_movbod6f6684 = 0x3e44c4c5; }
// [$L_593370]
extern "C" void __cdecl MovbodInit_593370() { g_movbod6f6698 = 0x3f800000; g_movbod6f669c = 0x3f800000; g_movbod6f66a0 = 0x3f800000; g_movbod6f66a4 = 0x3f800000; }

extern "C" void FUN_5f1cc0(); KING_ALIAS("_FUN_5f1cc0", "@Buckets_Push@16")
extern "C" void FUN_581ba0(); KING_ALIAS("_FUN_581ba0", "@Item_Apply84@16")
extern "C" void FUN_582960(); KING_ALIAS("_FUN_582960", "@Item_GroundNormal@12")
extern "C" void FUN_585da0(); KING_ALIAS("_FUN_585da0", "@Item_SendContact@20")
extern "C" void FUN_592e70(); KING_ALIAS("_FUN_592e70", "@ListNode_CutNext@8")
extern "C" void FUN_592e90(); KING_ALIAS("_FUN_592e90", "@ListNode_592e90@12")
KING_VAR(float, g_movbod650890, 650890)
KING_VAR(float, g_movbod650894, 650894)
KING_VAR(float, g_movbod650898, 650898)
KING_VAR(float, g_movbod6f662c, 6f662c)
KING_VAR(float, g_movbod6f6628, 6f6628)

// [$L_593270]
extern "C" int __fastcall Movbod_593270(BYTE* s, void*, int, int b) { return CallThis<int>(FUN_5f1cc0, s, 0, b); }
// [$L_58fba0]
extern "C" int __fastcall Movbod_58fba0(BYTE* s, void*, int a, int b) { return CallThis<int>(FUN_581ba0, s, a, b); }
// [$L_58e270]
extern "C" int __fastcall Movbod_58e270(BYTE* s, void*, int a, int b, int c) { return CallThis<int>(FUN_585da0, s, a, b, c); }
// [$L_58fb80]
extern "C" BYTE* __fastcall Movbod_58fb80(BYTE* s, void*, BYTE* p)
{
    CallThis(FUN_582960, s, p);
    return p;
}

// [$L_592f20] removes node p (next at +0) from the front of the list (+4 head, +8 tail)
extern "C" int __fastcall Movbod_ListPop(BYTE* s, void*, BYTE* p)
{
    if (F(BYTE*, s, 4) == p) {
        F(int, s, 4) = 0;
        F(int, s, 8) = 0;
        return (int)s;
    }
    F(int, s, 8) = F(int, p, 0);
    return CallThis<int>(FUN_592e70, s);
}

// [$L_592f50] appends the chain n (linked through +4) to the list; the tail goes to +8
extern "C" void __fastcall Movbod_ListAppend(BYTE* s, void*, BYTE* n)
{
    if (!F(BYTE*, s, 4))
        F(BYTE*, s, 4) = n;
    else
        CallThis(FUN_592e90, F(BYTE*, s, 8), n);
    BYTE* t = n;
    while (F(BYTE*, t, 4))
        t = F(BYTE*, t, 4);
    F(BYTE*, s, 8) = t;
}

// [$L_592b90] static initializers: angles atan(1 / (650890 / x))
extern "C" void __cdecl MovbodInit_592b90() { g_movbod6f662c = (float)x87_atan2((double)g_movbod650890 / g_movbod650894, 1.0); }
// [$L_592bc0]
extern "C" void __cdecl MovbodInit_592bc0() { g_movbod6f6628 = (float)x87_atan2((double)g_movbod650890 / g_movbod650898, 1.0); }

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")
KING_VAR(float, g_movbod702620, 702620)
KING_VAR(float, g_movbod702624, 702624)
KING_VAR(float, g_movbod702628, 702628)
KING_VAR(float, g_movbod64f404, 64f404)
KING_VAR(float, g_movbod6f6558, 6f6558)
KING_VAR(float, g_movbod6f655c, 6f655c)
KING_VAR(float, g_movbod6f6560, 6f6560)
KING_VAR(DWORD, g_movbod6f6548, 6f6548)
KING_VAR(DWORD, g_movbod6f654c, 6f654c)
KING_VAR(DWORD, g_movbod6f6550, 6f6550)

// [$L_58e1e0] static initializer: the up vector 702620 scaled by 64f404, and a copy
extern "C" void __cdecl MovbodInit_58e1e0()
{
    g_movbod6f6558 = (float)((double)g_movbod702620 * g_movbod64f404);
    g_movbod6f6548 = *(DWORD*)&g_movbod702620;
    g_movbod6f655c = (float)((double)g_movbod702624 * g_movbod64f404);
    g_movbod6f654c = *(DWORD*)&g_movbod702624;
    g_movbod6f6560 = (float)((double)g_movbod702628 * g_movbod64f404);
    g_movbod6f6550 = *(DWORD*)&g_movbod702628;
}

// [$L_590ce0] FUN_5e1a10 of each 20-byte record (+9520, count +9516)
extern "C" void __fastcall Movbod_590ce0(BYTE* s)
{
    for (int i = 0; i < F(int, s, 9516); i++) {
        BYTE* r = F(BYTE*, s, 9520) + i * 20;
        CallC(FUN_5e1a10, F(int, r, 0), F(int, r, 4));
    }
}

// ---------------------------------------------------------------------------
// More virtual methods.

extern "C" void FUN_5ef800(); KING_ALIAS("_FUN_5ef800", "@Scene_TestInfo@16")
extern "C" void FUN_593230(); KING_ALIAS("_FUN_593230", "_Vec3_TransformByObject")
extern "C" void FUN_5ef780(); KING_ALIAS("_FUN_5ef780", "@Node_Remove@8")
extern "C" void FUN_5d2190(); KING_ALIAS("_FUN_5d2190", "@Node_Link@12")
extern "C" void FUN_5f2080(); KING_ALIAS("_FUN_5f2080", "@Slots_Call48@20")
extern "C" void Movbod_qsort(); KING_ALIAS("_Movbod_qsort", "_qsort")
ASM_VAR(AsmLabel, g_movbodCmp592fe0, _Rec_CompareF4)

// [$L_593100] sorts the children (+48 list) by their depth for b: they are taken off, sorted (+56, 8-byte
// pairs of node and depth) and linked back in order, then slot 0 is drawn
extern "C" int __fastcall Movbod_SortChildren(BYTE* s, void*, int a, int b)
{
    int r = CallThis<int>(FUN_5ef800, s, a, b);
    if (!r)
        return r;
    if (!*F(BYTE**, s, 48))
        return 0;
    float v[3] = {0, 0, 0};
    unsigned n = 0;
    for (BYTE* o = *F(BYTE**, s, 48); o && n < 1000; o = *F(BYTE**, s, 48), n++) {
        BYTE** pair = (BYTE**)(F(BYTE*, s, 56) + n * 8);
        pair[0] = o;
        CallVirt(pair[0], 40, v);
        CallC(FUN_593230, v, b);
        *(float*)(F(BYTE*, s, 56) + n * 8 + 4) = v[1];
        CallThis(FUN_5ef780, *(BYTE**)(F(BYTE*, s, 56) + n * 8));
    }
    CallC(Movbod_qsort, F(BYTE*, s, 56), n, 8, (BYTE*)g_movbodCmp592fe0);
    CallThis(FUN_5f1cc0, s, 0, *F(BYTE**, s, 56));
    for (unsigned i = 1; i < n; i++) {
        BYTE* prev = *(BYTE**)(F(BYTE*, s, 56) + i * 8 - 8);
        BYTE* cur = *(BYTE**)(F(BYTE*, s, 56) + i * 8);
        F(int, cur, 44) = F(int, prev, 44);
        CallThis(FUN_5d2190, cur, prev);
        F(int, cur, 36) = F(int, prev, 36);
    }
    CallThis(FUN_5f2080, s, 0, a, b);
    return 1;
}

// ---------------------------------------------------------------------------
// Constructors and surfaces (exception frames dropped).

extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_5d9290(); KING_ALIAS("_FUN_5d9290", "@Scenes_Find@12")
extern "C" void FUN_50d190(); KING_ALIAS("_FUN_50d190", "@Shell_Nop@8")
#include <string.h>
extern "C" void Movbod_VecCtor(); KING_ALIAS("_Movbod_VecCtor", "??_L@YGXPAXIHP6EX0@Z1@Z")
KING_VAR(AsmLabel, s_q20_67e100, 67e100)
KING_VAR(AsmLabel, g_q20_64c26c, 64c26c)
KING_VAR(AsmLabel, g_q20_65094c, 65094c)
KING_VAR(AsmLabel, g_q20_650900, 650900)
extern "C" BYTE* __fastcall Rec593610_Construct(BYTE* s);
KING_VAR(AsmLabel, s_q20_672688, 672688)
KING_VAR(AsmLabel, s_q20_67c178, 67c178)

// [FUN_593430] constructor (world): a moving body manager (vtables 65094c, 650900 at +32): 8193 cells (+112),
// a free list of 256 items of 124 bytes (+100, head +104), the room list entry 67c178 (+116)
extern "C" BYTE* __fastcall MovBodies_Construct(BYTE* s, void*, BYTE* w)
{
    CallThis(FUN_5d31e0, s, 98, (const char*)s_q20_67e100);
    CallThis(FUN_5f19b0, s + 32, (const char*)s_q20_67e100, 5, 1);
    F(void*, s, 32) = g_q20_64c26c;
    F(int, s, 88) = 0;
    F(int, s, 92) = 0;
    F(void*, s, 0) = g_q20_65094c;
    F(void*, s, 32) = g_q20_650900;
    BYTE* a = (BYTE*)operator new(131088);
    if (a)
        for (int k = 0; k < 8193; k++) {
            F(int, a, k * 16) = 0;
            F(int, a, k * 16 + 4) = 0;
            F(int, a, k * 16 + 8) = 0;
        }
    F(BYTE*, s, 112) = a;
    BYTE* m = (BYTE*)operator new(31748);
    BYTE* e = 0;
    if (m) {
        *(int*)m = 256;
        e = m + 4;
        CallStd(Movbod_VecCtor, e, 124, 256, (BYTE*)Rec593610_Construct, FUN_50d190);
    }
    F(BYTE*, s, 100) = e;
    memset(e, 0, 31744);
    for (int o = 0; o < 31620; o += 124)
        F(BYTE*, F(BYTE*, s, 100) + o, 0) = F(BYTE*, s, 100) + o + 124;
    F(int, F(BYTE*, s, 100), 31620) = 0;
    F(BYTE*, s, 104) = F(BYTE*, s, 100);
    F(int, s, 108) = 0;
    BYTE* o = F(BYTE*, w, 524);
    if (o)
        CallVirt(o, 20, 0, s ? s + 32 : 0);
    BYTE* l = CallThis<BYTE*>(FUN_5d9290, w, (const char*)s_q20_672688);
    F(int, s, 116) = 0;
    unsigned n = F(unsigned, l, 72);
    for (unsigned i = 0; i < n; i++)
        if (!strcmp((const char*)(F(BYTE*, l, 76) + i * 124), (const char*)s_q20_67c178)) {
            F(BYTE*, s, 116) = F(BYTE*, l, 76) + i * 124;
            break;
        }
    F(BYTE*, s, 96) = w;
    CallThis(FUN_5d3570, w + 2992, s);
    CallThis(FUN_5d33d0, s, 1);
    return s;
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_58b870(); KING_ALIAS("_FUN_58b870", "@Body58b870_Destroy@8")
// [$L_58b700]
extern "C" BYTE* __fastcall Dtor58b700_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_58b870, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_593670(); KING_ALIAS("_FUN_593670", "@FadeBody_Destroy@8")
// [$L_593650]
extern "C" BYTE* __fastcall Dtor593650_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_593670, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Small methods: table constructors and clears, list teardown, cursor handling, flag maps.

extern "C" void FUN_592e40(); KING_ALIAS("_FUN_592e40", "@ListNode_Unlink@8")
KING_VAR(AsmLabel, g_q52_65089c, 65089c)
// [FUN_592cf0] destructor: the 65089c vtable; when the owner's head (+12) is this node (+4), moves it to the next
// (+4) or the previous (+8), then unlinks the node
extern "C" void __fastcall Linked592cf0_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = g_q52_65089c;
    BYTE* node = s ? s + 4 : 0;
    BYTE** head = F(BYTE**, s, 12);
    if (*head == node) {
        BYTE* n = F(BYTE*, s, 4);
        *head = n ? n : F(BYTE*, s, 8);
    }
    CallThis(FUN_592e40, s + 4);
}

// [$L_593610] constructor: +4..+63 cleared
extern "C" BYTE* __fastcall Rec593610_Construct(BYTE* s)
{
    memset(s + 4, 0, 60);
    return s;
}

extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
extern "C" void FUN_5ab400(); KING_ALIAS("_FUN_5ab400", "_Vec3_Normalized")
KING_VAR(float, g_q173_64c40c, 64c40c)
// [$L_5916e0] a hit on body s at p with speed v: for a fast enough hit (|v| / +396 at least 64c40c) of a body
// with +9516/+9520 set and +372 clear, the hit point in body space is computed and normalised (the result is not
// used, as in the original)
extern "C" void __fastcall Body_HitAt(BYTE* s, void*, const float* p, const float* v)
{
    if (F(int, s, 372) || !F(int, s, 9520) || !F(int, s, 9516))
        return;
    float c[3] = {v[0], v[1], v[2]};
    if (!(CallC<double>(FUN_5ab4d0, c) / F(float, s, 396) >= g_q173_64c40c))
        return;
    const float* m = (const float*)(s + 596);
    float q[3], out[3];
    q[0] = (float)(m[3] * p[1] + m[6] * p[2] + p[0] * m[0] + m[9]);
    q[1] = (float)(m[1] * p[0] + m[4] * p[1] + m[7] * p[2] + m[10]);
    q[2] = (float)(m[2] * p[0] + m[5] * p[1] + m[8] * p[2] + m[11]);
    CallC(FUN_5ab400, out, q);
}

// ---------------------------------------------------------------------------
// Thunks.

KING_VAR(char, g_q198_6508a0, 6508a0)
extern "C" int __fastcall q198_Chain_DeleteAll(BYTE* s, void* d); KING_ALIAS("@q198_Chain_DeleteAll@8", "@Chain_DeleteAll@8")
extern "C" int __fastcall q198_Dtor58b700_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor58b700_DeletingDestructor@12", "@Dtor58b700_DeletingDestructor@12")
// [$L_592b40] forwards to Dtor58b700_DeletingDestructor for the object 256 bytes before
extern "C" int __fastcall Thunk592b40(BYTE* s, void* d, int a0) { return q198_Dtor58b700_DeletingDestructor(s - 256, d, a0); }
// [$L_592ed0] sets the vtable 6508a0, then Chain_DeleteAll
extern "C" int __fastcall Thunk592ed0(BYTE* s, void* d)
{
    F(void*, s, 0) = &g_q198_6508a0;
    return q198_Chain_DeleteAll(s, d);
}

// === a body's contacts with its neighbours (FUN_58ae30)

extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_57c9a0(); KING_ALIAS("_FUN_57c9a0", "_Id_Same")
extern "C" void FUN_584c70(); KING_ALIAS("_FUN_584c70", "_Item_SphereContact")    // sphere contacts (centre, other centre, radius, contacts, body, sign)
extern "C" void FUN_58a020(); KING_ALIAS("_FUN_58a020", "_Box_PointContacts")    // box contacts (body, matrix, contacts, other, sign)
KING_VAR(AsmLabel, g_bc7025f0, 7025f0)

static double Bc_Abs(double x) { return x < 0 ? -x : x; }

extern "C" {

// [FUN_58ae30] adds to contacts c the contacts of body b (in matrix m) with its up to 6 neighbours (+3508) while
// they can be closer than their limits (+3532; by the gap +3300 moved for a frame and the radii +3272): spheres
// (+392 = 1) and boxes (2) from their side in b's inverse frame, then b against each; the count added
int __cdecl Body_NeighbourContacts(BYTE* b, BYTE* m, BYTE* c)
{
    float A[12], B[12], C[12], Mo[12], R[12], t1[12], t2[12], t3[12];
    memcpy(A, (BYTE*)g_bc7025f0, 48);
    memcpy(B, (BYTE*)g_bc7025f0, 48);
    memcpy(C, (BYTE*)g_bc7025f0, 48);
    int total = 0;
    float dt = (float)Game_FrameTime();
    for (int i = 0; i < 6; i++) {
        BYTE* o = F(BYTE*, b, 3508 + i * 4);
        if (!o)
            break;
        double d = (Bc_Abs(F(float, o, 3308) - (double)F(float, b, 3308)) +
                    Bc_Abs(F(float, o, 3304) - (double)F(float, b, 3304)) +
                    Bc_Abs(F(float, o, 3300) - (double)F(float, b, 3300))) * dt + F(float, o, 3272) + F(float, b, 3272);
        if (F(float, b, 3532 + i * 4) > d * d)
            break;
        int type = F(int, o, 392);
        if (type == 1 || type == 2) {
            if (!i) {
                CallThis(FUN_57de90, b, t1);
                memcpy(A, CallThis<BYTE*>(FUN_57dec0, b, t2), 48);
                memcpy(B, CallC<BYTE*>(FUN_5aba40, t3, A, m), 48);
                memcpy(C, CallC<BYTE*>(FUN_5ac2e0, t1, B), 48);
            }
            CallThis(FUN_57de90, o, Mo);
            CallC(FUN_5aba40, R, Mo, C);
            int first = F(int, c, 2816);
            if (type == 2)
                total += CallC<int>(FUN_58a020, o, R, c, b, -1);
            else if (int k = CallC<int>(FUN_584c70, Mo + 9, R + 9, F(DWORD, o, 3272), c, b, -1)) {
                total += k;
                F(int, c, 1532 + F(int, c, 2816) * 4) = CallC<int>(FUN_57c9a0, o);
            }
            for (int k = first; k < F(int, c, 2816); k++)
                memcpy(c + 3332 + k * 48, C, 48);
        }
        total += CallC<int>(FUN_58a020, b, m, c, o, 1);
    }
    return total;
}

} // extern "C"

// === placing a body by its position and direction ($L_58fbc0)

extern "C" void FUN_4e3132(); KING_ALIAS("_FUN_4e3132", "_Vec3_DivComponents")
extern "C" void FUN_4e5bf5(); KING_ALIAS("_FUN_4e5bf5", "_Obj_Lookup4e5b22")
extern "C" void FUN_57e070(); KING_ALIAS("_FUN_57e070", "@Vehicle_MakeOut@8")
extern "C" void FUN_581c80(); KING_ALIAS("_FUN_581c80", "@Item_ResetFlags@16")
extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
extern "C" void FUN_5abc90(); KING_ALIAS("_FUN_5abc90", "_Matrix_LookAt")
KING_VAR(int, g_bp6f6568, 6f6568)

extern "C" {

// [$L_58fbc0] body s placed (unless held, +3976: then flags bits 1 / 2): its matrix (+500) at the position
// +3148 looking along +3160 times the frame +548, its inverse (+596), the speed +440 turned back and divided by
// +404 into +428, and its room (+800, kept from +804 and reported when lost); 0, or 1 when held
int __fastcall Body_Place(BYTE* s, void*, int a, DWORD* flags)
{
    g_bp6f6568 = 0;
    CallThis(FUN_581c80, s, a, flags);
    if (F(int, s, 3976)) {
        if (F(int, s, 3976) == 2)
            *flags |= 2;
        if (F(int, s, 3976) == 1)
            *flags |= 4;
        return 1;
    }
    BYTE t1[48], t2[48], t3[48], t4[48], t5[48];
    Vec3 p = F(Vec3, s, 3148), d = F(Vec3, s, 3160);
    BYTE* tr = CallC<BYTE*>(FUN_5abc40, t1, p);
    float len = (float)CallC<double>(FUN_5ab4d0, s + 3160);
    BYTE* look = CallC<BYTE*>(FUN_5abc90, t2, g_gravity702650, d, len);
    BYTE* m = CallC<BYTE*>(FUN_5aba40, t4, CallC<BYTE*>(FUN_5aba40, t3, look, s + 548), tr);
    memcpy(s + 500, m, 48);
    memcpy(s + 596, CallC<BYTE*>(FUN_5ac2e0, t5, s + 500), 48);
    const float* q = (const float*)(s + 596);
    float vx = F(float, s, 440), vy = F(float, s, 444), vz = F(float, s, 448);
    float w[3];
    w[0] = (float)((double)q[3] * vy + (double)q[6] * vz + (double)vx * q[0]);
    w[1] = (float)((double)q[4] * vy + (double)q[7] * vz + (double)q[1] * vx);
    w[2] = (float)((double)q[5] * vy + (double)q[8] * vz + (double)q[2] * vx);
    float r[3];
    memcpy(s + 428, CallC<BYTE*>(FUN_4e3132, r, w, s + 404), 12);
    F(int, s, 800) = CallC<int>(FUN_4e5bf5, F(int, s, 804), s + 584, s + 536, s + 1132);
    if (!F(int, s, 800)) {
        F(int, s, 800) = F(int, s, 804);
        CallVirt(s, 28, 1);
    }
    CallThis(FUN_57e070, s);
    return 0;
}

} // extern "C"

// === spawning sparks (FUN_5940f0)

extern "C" void FUN_5940d0(); KING_ALIAS("_FUN_5940d0", "@FreeList_Take@8")
extern "C" int __cdecl rand(void);
KING_VAR(Vec3, g_sp702620, 702620)
KING_VAR(Vec3, g_sp702640, 702640)
KING_VAR(float, g_sp64c254, 64c254)
KING_VAR(float, g_sp65096c, 65096c)
KING_VAR(float, g_sp64c448, 64c448)
KING_VAR(AsmLabel, g_spColA0, 6f6668)
KING_VAR(AsmLabel, g_spColA1, 6f6688)
KING_VAR(AsmLabel, g_spColA2, 6f6678)
KING_VAR(AsmLabel, g_spColB0, 6f6698)
KING_VAR(AsmLabel, g_spColB1, 6f6658)
KING_VAR(AsmLabel, g_spColB2, 6f6648)

// a random number in [-1, 1): the first rand subtracted from the second
static double Sp_Rand()
{
    float a = (float)(rand() * (double)g_sp64c254);
    return rand() * (double)g_sp64c254 - a;
}

extern "C" {

// [FUN_5940f0] takes n sparks of pool s at source src (+16 position, +28 speed): each moved by a random offset
// (along 702640, 702620 and up), stamped with the time (s+96 +2968) and one of two colour sets (16 bytes each)
void __fastcall Sparks_Spawn(BYTE* s, void*, const BYTE* src, unsigned n)
{
    if (!F(int, s, 104))
        return;
    for (unsigned i = 0; i < n; i++) {
        BYTE* p = CallThis<BYTE*>(FUN_5940d0, s);
        if (!p)
            return;
        float a = (float)Sp_Rand();
        float b = (float)Sp_Rand();
        double c = Sp_Rand();
        const Vec3& u = g_axis702630;
        float cu0 = (float)(u.x * c), cu1 = (float)(u.y * c);
        double cz = c * u.z;
        double fa0 = g_sp702620.x * (double)b;
        float fa1 = (float)(g_sp702620.y * (double)b), fa2 = (float)(g_sp702620.z * (double)b);
        double fb0 = g_sp702640.x * (double)a;
        float fb1 = (float)(g_sp702640.y * (double)a), fb2 = (float)(g_sp702640.z * (double)a);
        float s1 = (float)((double)fa1 + fb1), s2 = (float)((double)fa2 + fb2);
        float v[3] = {(float)(fa0 + fb0 + cu0), (float)((double)cu1 + s1), (float)(cz + s2)};
        memcpy(p + 40, v, 12);
        memcpy(p + 4, v, 12);
        memcpy(p + 16, src + 16, 12);
        memcpy(p + 52, src + 28, 12);
        memcpy(p + 28, src + 28, 12);
        F(int, p, 120) = 1;
        F(float, p, 64) = (float)F(double, F(BYTE*, s, 96), 2968);
        if (rand() * (double)g_sp65096c < g_sp64c448) {
            memcpy(p + 68, (BYTE*)g_spColA0, 16);
            memcpy(p + 84, (BYTE*)g_spColA1, 16);
            memcpy(p + 100, (BYTE*)g_spColA2, 16);
        } else {
            memcpy(p + 68, (BYTE*)g_spColB0, 16);
            memcpy(p + 84, (BYTE*)g_spColB1, 16);
            memcpy(p + 100, (BYTE*)g_spColB2, 16);
        }
        F(float, p, 116) = 1.0f;
    }
}

} // extern "C"

// === a body's collision by kind (58dff0)

extern "C" void Bc_WarnOpen(); KING_ALIAS("_Bc_WarnOpen", "_WarnLog_Open")
extern "C" void Bc_WarnClose(); KING_ALIAS("_Bc_WarnClose", "_WarnLog_Close")
extern "C" void Bc_Log(); KING_ALIAS("_Bc_Log", "_VehLog_Value2")
extern "C" void Bc_sprintf(); KING_ALIAS("_Bc_sprintf", "_sprintf")
extern "C" void Bc_ActivateAll(); KING_ALIAS("_Bc_ActivateAll", "@Points_ActivateAll@8")
extern "C" void FUN_5843c0(); KING_ALIAS("_FUN_5843c0", "_Item_MoveFraction")    // a body's collision with the ground
extern "C" void FUN_5855a0(); KING_ALIAS("_FUN_5855a0", "_Item_BodyContact")    // with another body
extern "C" void FUN_58b120(); KING_ALIAS("_FUN_58b120", "_Body_NearestContact")    // with a moving object
KING_VAR(int, g_bc6f6620, 6f6620)
KING_VAR(AsmLabel, s_bc67dd8c, 67dd8c)
KING_VAR(AsmLabel, s_bc67dd70, 67dd70)
KING_VAR(AsmLabel, s_bc67dd54, 67dd54)
KING_VAR(AsmLabel, s_bc67dd38, 67dd38)
KING_VAR(AsmLabel, s_bc67dd1c, 67dd1c)
KING_VAR(AsmLabel, s_bc67dd10, 67dd10)

// "fmt" with the three floats at p
static void Bc_Fmt3(char* buf, const void* fmt, const float* p)
{
    CallC(Bc_sprintf, buf, (const char*)fmt, (double)p[0], (double)p[1], (double)p[2]);
}

extern "C" {

// [$L_58dff0] collision of body s with (a, b, c) at contact k by kind flags (bit 2 the ground: a hit is logged with
// 6f6620 unless the contact is known +116; bit 1 another body; else a moving object, which then wakes all points)
int __fastcall Body_Collide(BYTE* s, void*, int a, int b, int c, BYTE* k, unsigned flags)
{
    if (flags & 4) {
        int r = CallC<int>(FUN_5843c0, s, a, b, c, k);
        if (g_bc6f6620 && (r & 1) && !F(int, k, 116)) {
            char b1[100], b2[100], b3[100], b4[100], b5[100], b6[490];
            CallC(Bc_sprintf, b1, (const char*)s_bc67dd8c, F(const char*, F(BYTE*, s, 800), 12));
            Bc_Fmt3(b2, s_bc67dd70, (const float*)(s + 584));
            Bc_Fmt3(b3, s_bc67dd54, (const float*)(s + 536));
            Bc_Fmt3(b4, s_bc67dd38, (const float*)(k + 52));
            Bc_Fmt3(b5, s_bc67dd1c, (const float*)(k + 64));
            CallC(Bc_sprintf, b6, (const char*)s_bc67dd10, b1, b2, b3, b4, b5);
            CallC(Bc_WarnOpen);
            CallC(Bc_Log, b6);
            CallC(Bc_WarnClose);
        }
        return r;
    }
    if (flags & 2)
        return CallC<int>(FUN_5855a0, s, a, b, c, k, flags);
    int r = CallC<int>(FUN_58b120, s, a, b, c, k, flags);
    if (r)
        CallThis(Bc_ActivateAll, s);
    return r;
}

} // extern "C"

// === a body's nearest contact (FUN_58b120)

extern "C" void FUN_585150(); KING_ALIAS("_FUN_585150", "_Item_PointContacts")
extern "C" void Nc_Ground(); KING_ALIAS("_Nc_Ground", "_Movingb_589e70")
extern "C" void Nc_Neighbours(); KING_ALIAS("_Nc_Neighbours", "_Body_NeighbourContacts")
extern "C" void Nc_DivAssign(); KING_ALIAS("_Nc_DivAssign", "@Vec3_DivAssign@16")
extern "C" void Nc_AddAssign(); KING_ALIAS("_Nc_AddAssign", "@Vec3_AddAssign@16")
KING_VAR(float, g_nc64c2f8, 64c2f8)
KING_VAR(double, g_nc6506c0, 6506c0)
KING_VAR(double, g_nc64d918, 64d918)

extern "C" {

// [FUN_58b120] collects the contacts of body b (with the ground (a), its neighbours (n), the others (o)) and
// returns the nearest: *scale *= its distance, *value its +2560 value, out its point (+52; without neighbour
// contacts the distance-weighted mean of the contacts facing the same way) and normal (+64), +100, +116 and
// for a contact without an object its 48-byte frame (+4); the result flags: the surface's bits 1/2 (as
// 0x4000000/0x2000000, unless b+3588), 0x8000000 without an object, 3 ground, 8 neighbours, 4 others; 0 for none
int __cdecl Body_NearestContact(BYTE* b, BYTE* m, float* scale, float* value, BYTE* out, int a6)
{
    BYTE t[6404];
    CallThis(FUN_587c80, t);
    F(int, t, 2816) = 0;
    int ground = CallC<int>(Nc_Ground, b, m, t, a6);
    int nb = CallC<int>(Nc_Neighbours, b, m, t);
    int other = CallC<int>(FUN_585150, b, m, t);
    if (!ground && !nb && !other)
        return 0;
    int n = F(int, t, 2816);
    int best = 0;
    float bestValue = 0;
    float nearest = g_nc64c2f8;
    float* dist = (float*)(t + 2048);
    int* flag = (int*)(t + 2820);
    Vec3* pts = (Vec3*)t;
    Vec3* nrm = (Vec3*)(t + 768);
    for (int i = 0; i < n; i++)
        if (flag[i] && nearest > dist[i]) {
            best = i;
            nearest = dist[i];
            bestValue = F(float, t, 2560 + i * 4);
        }
    Vec3 p = pts[best], nb0 = nrm[best];
    *scale = dist[best] * *scale;
    Vec3 acc = g_gravity702650, tmp;
    float wsum = 0;
    if (!nb) {
        for (int j = n - 1; j >= 0; j--) {
            if (!flag[j])
                continue;
            if (!((double)nrm[j].z * nb0.z + (double)nrm[j].x * nb0.x + (double)nrm[j].y * nb0.y > g_nc6506c0))
                continue;
            float w = (float)(g_nc64d918 - dist[j]);
            Vec3 v = {(pts[j].x - p.x) * w, (pts[j].y - p.y) * w, (pts[j].z - p.z) * w};
            CallThis(Nc_AddAssign, &acc, &tmp, &v);
            wsum = (float)(g_nc64d918 - dist[j] + wsum);
        }
        CallThis(Nc_DivAssign, &acc, &tmp, *(DWORD*)&wsum);
        CallThis(Nc_AddAssign, &acc, &tmp, &p);
        F(Vec3, out, 52) = acc;
        F(Vec3, out, 64) = *CallC<Vec3*>(FUN_5ab400, &tmp, &nb0);
    } else {
        F(Vec3, out, 52) = pts[best];
        F(Vec3, out, 64) = nrm[best];
    }
    F(int, out, 100) = F(int, t, 1792 + best * 4);
    F(int, out, 116) = F(int, t, 1536 + best * 4);
    if (flag[best] == -1)
        memcpy(out + 4, t + 3332 + best * 48, 48);
    *value = bestValue;
    int r = 0;
    if (!F(int, b, 3588)) {
        DWORD s = F(DWORD, b, 3896 + F(int, t, 3076 + best * 4) * 4);
        if (s & 1)
            r = 0x4000000;
        if (s & 2)
            r |= 0x2000000;
    }
    if (flag[best] == -1)
        r |= 0x8000000;
    if (ground)
        r |= 3;
    if (nb)
        r |= 8;
    if (other)
        r |= 4;
    return r;
}

} // extern "C"

// === a body's nearest neighbours (a method at $L_5914c0)

extern "C" void Nn_Matrix1(); KING_ALIAS("_Nn_Matrix1", "@SceneObj_Matrix1@12")
KING_VAR(BYTE*, g_nn6f653c, 6f653c)

extern "C" {

// [$L_5914c0] the six bodies (of the list 6f653c, other than s and its +9512) whose centres (+3436 placed) lie
// nearest to s's position (+536): +3508 the bodies, +3532 their squared distances (100000 when free), nearest first
void __fastcall Body_FindNeighbours(BYTE* s)
{
    BYTE* node = g_nn6f653c ? g_nn6f653c - 4 : 0;
    const float* pos = (const float*)(s + 536);
    float px = pos[0], py = pos[1], pz = pos[2];
    BYTE** nb = (BYTE**)(s + 3508);
    float* dist = (float*)(s + 3532);
    for (int k = 0; k < 6; k++) {
        nb[k] = 0;
        *(DWORD*)&dist[k] = 0x47C35000;
    }
    for (; node; node = F(BYTE*, node, 8) ? F(BYTE*, node, 8) - 4 : 0) {
        BYTE* o = node - 376;
        if (o == s || o == F(BYTE*, s, 9512))
            continue;
        BYTE m[48];
        const float* M = CallThis<const float*>(Nn_Matrix1, o, m);
        const float* c = (const float*)(o + 3436);
        double wx = (double)M[3] * c[1] + (double)M[6] * c[2] + (double)c[0] * M[0] + M[9];
        float wy = (float)((double)M[4] * c[1] + (double)M[7] * c[2] + (double)M[1] * c[0] + M[10]);
        float wz = (float)((double)M[5] * c[1] + (double)M[8] * c[2] + (double)M[2] * c[0] + M[11]);
        float dx = (float)(wx - px);
        float dy = (float)(wy - py);
        double dz = (double)wz - pz;
        double d2 = (double)dx * dx + (double)dy * dy + dz * dz;
        int i = 0;
        for (; i < 6; i++)
            if (!nb[i] || d2 < dist[i])
                break;
        if (i == 6)
            continue;
        for (int j = 5; j > i; j--) {
            nb[j] = nb[j - 1];
            dist[j] = dist[j - 1];
        }
        nb[i] = o;
        dist[i] = (float)d2;
    }
}

} // extern "C"

// === whether two bodies touch (FUN_5924d0)

extern "C" Vec3* __fastcall Vec3_SetThis(Vec3* self, void*, float x, float y, float z);
extern "C" void Bt_MulMatrix(); KING_ALIAS("_Bt_MulMatrix", "_Vec3_MulMatrix")
extern "C" void Bt_Sub(); KING_ALIAS("_Bt_Sub", "_Vec3_Sub")
extern "C" void Bt_Dot(); KING_ALIAS("_Bt_Dot", "_Vec3_Dot")
extern "C" void Bt_PointsFirst(); KING_ALIAS("_Bt_PointsFirst", "@Points_NextFirst@12")
extern "C" void Bt_RecordsNext(); KING_ALIAS("_Bt_RecordsNext", "@Records_Next@12")

extern "C" {

// [FUN_5924d0] whether body a (frame ma, its planes in frame pa) touches body b (frame mb): no when their
// bounding spheres (+3272 about the centres +3436) miss; *out = -a's inner radius (+3280) when the inner spheres
// overlap; else when one of a's points lies within b's inner radius of b's centre, or b's centre lies behind
// all of a's face planes by at most that radius
int __cdecl Bodies_TouchTest(BYTE* a, const float* ma, const float* pa, BYTE* b, const float* mb, int unused, float* out)
{
    *out = 0;
    const float* cb = (const float*)(b + 3436);
    float bx = (float)((double)mb[0] * cb[0] + (double)cb[1] * mb[3] + (double)cb[2] * mb[6] + mb[9]);
    float by = (float)((double)cb[2] * mb[7] + (double)mb[1] * cb[0] + (double)cb[1] * mb[4] + mb[10]);
    float bz = (float)((double)mb[8] * cb[2] + (double)mb[5] * cb[1] + (double)cb[0] * mb[2] + mb[11]);
    const float* ca = (const float*)(a + 3436);
    Vec3 c1, d;
    Vec3_SetThis(&c1, 0, (float)((double)ma[6] * ca[2] + (double)ma[3] * ca[1] + (double)ma[0] * ca[0] + ma[9]),
        (float)((double)ma[7] * ca[2] + (double)ma[1] * ca[0] + (double)ma[4] * ca[1] + ma[10]),
        (float)((double)ma[8] * ca[2] + (double)ma[2] * ca[0] + (double)ma[5] * ca[1] + ma[11]));
    Vec3_SetThis(&d, 0, c1.x - bx, c1.y - by, c1.z - bz);
    double dsq = (double)d.x * d.x + (double)d.y * d.y + (double)d.z * d.z;
    double r = (double)F(float, a, 3272) + F(float, b, 3272);
    if (dsq > r * r)
        return 0;
    double ri = (double)F(float, b, 3280) + F(float, a, 3280);
    if (dsq < ri * ri) {
        *out = -F(float, a, 3280);
        return 1;
    }
    Vec3 q;
    Vec3_SetThis(&q, 0, (float)((double)mb[3] * cb[1] + (double)cb[0] * mb[0] + (double)mb[6] * cb[2] + mb[9]),
        (float)((double)mb[7] * cb[2] + (double)mb[4] * cb[1] + (double)cb[0] * mb[1] + mb[10]),
        (float)((double)mb[8] * cb[2] + (double)mb[5] * cb[1] + (double)mb[2] * cb[0] + mb[11]));
    float rr = F(float, b, 3280);
    float rr2 = rr * rr;
    Vec3 pt = {0, 0, 0}, w, e, t;
    int hit = 0;
    if (CallThis<int>(Bt_PointsFirst, a, &pt)) {
        do {
            w = *CallC<Vec3*>(Bt_MulMatrix, &t, &pt, ma);
            Vec3_SetThis(&e, 0, w.x - q.x, w.y - q.y, w.z - q.z);
            if (CallC<double>(Bt_Dot, &e, &e) < rr2)
                hit = 1;
        } while (CallThis<int>(Bt_PointsFirst, a, &pt));
        if (hit)
            return hit;
    }
    BYTE rec[28];
    CallThis(FUN_58b4f0, rec);
    Vec3 w2 = *CallC<Vec3*>(Bt_MulMatrix, &t, &q, pa);
    int all = 1;
    while (CallThis<int>(Bt_RecordsNext, a, rec)) {
        Vec3 df;
        CallC(Bt_Sub, &df, &w2, rec + 16);
        if (rr < CallC<double>(Bt_Dot, &df, rec))
            all = 0;
    }
    if (all)
        hit = 1;
    return hit;
}

} // extern "C"

// === a burst of sparks off a surface (FUN_593720)

extern "C" void Sb_Normalized(); KING_ALIAS("_Sb_Normalized", "_Vec3_Normalized")
KING_VAR(Vec3, g_sb702630, 702630)
KING_VAR(Vec3, g_sb702650, 702650)
extern "C" void __cdecl Fader_Start(int a0, int a1, float strength, int a3, int a4);
KING_VAR(float, g_sb64c264, 64c264)
KING_VAR(float, g_sb64c250, 64c250)
KING_VAR(float, g_sb64c938, 64c938)
KING_VAR(float, g_sb64c260, 64c260)
KING_VAR(float, g_sb64c408, 64c408)
KING_VAR(float, g_sb64ed88, 64ed88)
KING_VAR(float, g_sb64c25c, 64c25c)
KING_VAR(float, g_sb64c350, 64c350)
KING_VAR(float, g_sb64c268, 64c268)
KING_VAR(double, g_sb64b670, 64b670)
KING_VAR(int, g_sb6508f4, 6508f4)

// x, or +-1 (64c250 / 64c938) when it is too small
static float Sb_Away(float x)
{
    if (x87_fabs(x) < g_sb64b670)
        return x > g_sb64c264 ? g_sb64c250 : g_sb64c938;
    return x;
}

// fills spark p moving at v from pos
static void Sb_Fill(BYTE* s, BYTE* p, const float* v, const float* pos)
{
    memcpy(p + 40, v, 12);
    memcpy(p + 4, v, 12);
    memcpy(p + 16, &g_sb702650, 12);
    memcpy(p + 52, pos, 12);
    memcpy(p + 28, pos, 12);
    F(int, p, 120) = 0;
    F(float, p, 64) = (float)F(double, F(BYTE*, s, 96), 2968);
    if (rand() * (double)g_sp65096c < g_sp64c448) {
        memcpy(p + 68, (BYTE*)g_spColA0, 16);
        memcpy(p + 84, (BYTE*)g_spColA1, 16);
        memcpy(p + 100, (BYTE*)g_spColA2, 16);
    } else {
        memcpy(p + 68, (BYTE*)g_spColB0, 16);
        memcpy(p + 84, (BYTE*)g_spColB1, 16);
        memcpy(p + 100, (BYTE*)g_spColB2, 16);
    }
    F(float, p, 116) = 1.0f;
}

extern "C" {

// [FUN_593720] n sparks of pool s from pos off a surface of normal nm: half of n fly in random world directions,
// a quarter of n leave along the normal (up to 1.5 times) spread across it; then a fading light at pos
void __fastcall Sparks_Burst(BYTE* s, void*, const float* pos, const float* nm, int n)
{
    if (!F(int, s, 104))
        return;
    float a[3], t[3];
    if (nm[0] == g_sb64c264 && nm[1] == g_sb64c264) {
        a[0] = g_sp702640.z * nm[1] - g_sp702640.y * nm[2];
        a[1] = g_sp702640.x * nm[2] - g_sp702640.z * nm[0];
        a[2] = g_sp702640.y * nm[0] - g_sp702640.x * nm[1];
    } else {
        a[0] = g_sb702630.z * nm[1] - g_sb702630.y * nm[2];
        a[1] = g_sb702630.x * nm[2] - g_sb702630.z * nm[0];
        a[2] = g_sb702630.y * nm[0] - g_sb702630.x * nm[1];
    }
    const float* r = CallC<const float*>(Sb_Normalized, t, a);
    float u[3] = {r[0], r[1], r[2]};
    float b[3] = {u[2] * nm[1] - u[1] * nm[2], u[0] * nm[2] - u[2] * nm[0], u[1] * nm[0] - u[0] * nm[1]}, t2[3];
    r = CallC<const float*>(Sb_Normalized, t2, b);
    float v[3] = {r[0], r[1], r[2]};
    float nn[3] = {nm[0], nm[1], nm[2]};
    float cnt = (float)(__int64)n;
    float half = cnt * g_sb64c260;
    for (int i = 0; (float)(__int64)i < half; i++) {
        BYTE* p = CallThis<BYTE*>(FUN_5940d0, s);
        if (!p)
            break;
        float x1 = (float)(Sp_Rand() * g_sb64c408);
        float x2 = (float)(Sp_Rand() * g_sb64c408);
        float x3 = (float)(Sp_Rand() * g_sb64c408);
        float s1 = Sb_Away(x1), s2 = Sb_Away(Sb_Away(x2));
        float A[3] = {g_sb702630.x * x3, g_sb702630.y * x3, g_sb702630.z * x3};
        float B[3] = {g_sp702620.x * s2, g_sp702620.y * s2, g_sp702620.z * s2};
        float C[3] = {g_sp702640.x * s1, g_sp702640.y * s1};
        double cz = (double)g_sp702640.z * s1;
        float sx = C[0] + B[0], sy = B[1] + C[1];
        double sz = cz + B[2];
        float vel[3] = {sx + A[0], A[1] + sy, (float)(A[2] + sz)};
        Sb_Fill(s, p, vel, pos);
    }
    float spread = cnt * g_sb64ed88 + g_sb64c250;
    float lim = cnt * g_sb64c25c;
    for (int i = 0; (float)(__int64)i < lim; i++) {
        BYTE* p = CallThis<BYTE*>(FUN_5940d0, s);
        if (!p)
            break;
        float x1 = (float)(Sp_Rand() * spread);
        float x2 = (float)(Sp_Rand() * spread);
        float sp = (float)(rand() * (double)g_sp64c254 + g_sb64c250);
        float s1 = Sb_Away(x1), s2 = Sb_Away(x2);
        if (sp > g_sb64c350)
            *(DWORD*)&sp = 0x3FC00000;
        float A[3] = {nn[0] * sp, nn[1] * sp, nn[2] * sp};
        float B[3] = {v[0] * s2, v[1] * s2, v[2] * s2};
        float C[2] = {u[0] * s1, u[1] * s1};
        double cz = (double)u[2] * s1;
        float sx = C[0] + B[0], sy = B[1] + C[1];
        double sz = cz + B[2];
        float vel[3] = {sx + A[0], A[1] + sy, (float)(A[2] + sz)};
        Sb_Fill(s, p, vel, pos);
    }
    Fader_Start(F(int, s, 96), (int)pos, cnt * g_sb64c268, g_sb6508f4, 0x3DA3D70A);
}

} // extern "C"

// === how deep two bodies overlap (FUN_591c30)

KING_VAR(float, g_bd64c264, 64c264)

// one pass over the planes of body o against point q: clears *inside when q lies in front of one; with points
// the deepest front distance goes to *best, behind ones raise *out; without (the centre) *out rises to the
// distance less radius r
static void Bd_Planes(BYTE* o, BYTE* rec, const Vec3* q, int points, float r, float* out, float* best, int* inside)
{
    while (CallThis<int>(Bt_RecordsNext, o, rec)) {
        Vec3 df;
        CallC(Bt_Sub, &df, q, rec + 16);
        float d = (float)CallC<double>(Bt_Dot, &df, rec);
        if (!points) {
            if (d > g_bd64c264)
                *inside = 0;
            else if (*out < d)
                *out = d - r;
        } else if (d > g_bd64c264) {
            *inside = 0;
            if (*best < d)
                *best = d;
        } else if (*out < d)
            *out = d;
    }
}

extern "C" {

// [FUN_591c30] whether body a (frame ma, plane frame pa) and body b (mb, pb) overlap: no when their bounding
// spheres miss; *out = -a's inner radius and yes when the inner spheres meet; else yes when one of a's points
// (or its centre) lies behind all of b's planes, or one of b's (or its centre) behind all of a's, *out then the
// depth; otherwise *out = the nearest separation
int __cdecl Bodies_Overlap(BYTE* a, const float* ma, const float* pa, BYTE* b, const float* mb, const float* pb,
    float* out)
{
    *out = 0;
    const float* ca = (const float*)(a + 3436);
    const float* cb = (const float*)(b + 3436);
    float A[3], B[3];
    A[0] = (float)((double)ma[0] * ca[0] + (double)ca[1] * ma[3] + (double)ca[2] * ma[6] + ma[9]);
    A[1] = (float)((double)ca[2] * ma[7] + (double)ca[1] * ma[4] + (double)ma[1] * ca[0] + ma[10]);
    A[2] = (float)((double)ma[8] * ca[2] + (double)ma[5] * ca[1] + (double)ca[0] * ma[2] + ma[11]);
    B[0] = (float)((double)mb[6] * cb[2] + (double)mb[3] * cb[1] + (double)mb[0] * cb[0] + mb[9]);
    B[1] = (float)((double)mb[7] * cb[2] + (double)mb[1] * cb[0] + (double)mb[4] * cb[1] + mb[10]);
    B[2] = (float)((double)mb[8] * cb[2] + (double)mb[2] * cb[0] + (double)mb[5] * cb[1] + mb[11]);
    float dx = A[0] - B[0], dy = A[1] - B[1];
    double dz = (double)A[2] - B[2];
    double dsq = dz * dz + (double)dx * dx + (double)dy * dy;
    float R = F(float, a, 3272) + F(float, b, 3272);
    if (dsq > (double)R * R)
        return 0;
    double ri = (double)F(float, a, 3280) + F(float, b, 3280);
    *out = -F(float, a, 3280);
    if (dsq < ri * ri)
        return 1;
    Vec3 pt = {0, 0, 0}, q = {0, 0, 0}, t;
    BYTE rec[28];
    CallThis(FUN_58b4f0, rec);
    int found = 0, more = 1, step = 1;
    for (;;) {
        if (step) {
            step = CallThis<int>(Bt_PointsFirst, a, &pt);
            double wx = (double)pt.x * ma[0] + (double)pt.z * ma[6] + (double)pt.y * ma[3] + ma[9];
            double wy = (double)pt.x * ma[1] + (double)pt.z * ma[7] + (double)pt.y * ma[4] + ma[10];
            double wz = (double)pt.z * ma[8] + (double)pt.y * ma[5] + (double)pt.x * ma[2] + ma[11];
            Vec3_SetThis(&q, 0, (float)(wz * pb[6] + wy * pb[3] + wx * pb[0] + pb[9]),
                (float)(wz * pb[7] + wy * pb[4] + wx * pb[1] + pb[10]),
                (float)(wz * pb[8] + wy * pb[5] + wx * pb[2] + pb[11]));
        } else {
            more = 0;
            Vec3_SetThis(&q, 0, (float)((double)A[2] * pb[6] + (double)A[1] * pb[3] + (double)A[0] * pb[0] + pb[9]),
                (float)((double)A[2] * pb[7] + (double)A[1] * pb[4] + (double)A[0] * pb[1] + pb[10]),
                (float)((double)A[2] * pb[8] + (double)A[1] * pb[5] + (double)A[0] * pb[2] + pb[11]));
        }
        int inside = 1;
        float best = 0;
        Bd_Planes(b, rec, &q, more, F(float, a, 3280), out, &best, &inside);
        if (more && best < R)
            R = best;
        if (inside)
            found = 1;
        if (!more)
            break;
    }
    if (found)
        return found;
    *out = -F(float, b, 3280);
    more = 1, step = 1;
    for (;;) {
        Vec3 w;
        if (step) {
            step = CallThis<int>(Bt_PointsFirst, b, &pt);
            Vec3* m = CallC<Vec3*>(Bt_MulMatrix, &t, &pt, mb);
            w = *CallC<Vec3*>(Bt_MulMatrix, &q, m, pa);
        } else {
            more = 0;
            w = *CallC<Vec3*>(Bt_MulMatrix, (Vec3*)A, (const Vec3*)B, pa);
        }
        q = w;
        int inside = 1;
        float best = 0;
        Bd_Planes(a, rec, &q, more, F(float, b, 3280), out, &best, &inside);
        if (more && best < R)
            R = best;
        if (inside)
            found = 1;
        if (!more)
            break;
    }
    if (!found)
        *out = R;
    return found;
}

} // extern "C"

// === sparks drawn as lines (a method at $L_594650)

extern "C" void Sl_Normalized(); KING_ALIAS("_Sl_Normalized", "_Vec3_Normalized")
extern "C" void Sl_Length(); KING_ALIAS("_Sl_Length", "_Vec3_Length")
extern "C" void Sl_MulAssign(); KING_ALIAS("_Sl_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Sl_Material(); KING_ALIAS("_Sl_Material", "@Material_Select@24")
extern "C" void Sl_DrawTested(); KING_ALIAS("_Sl_DrawTested", "@Obj_DrawTested@16")
KING_VAR(BYTE*, g_sl720908, 720908)
KING_VAR(float, g_sl64c334, 64c334)
KING_VAR(float, g_sl64c250, 64c250)
KING_VAR(float, g_sl64c25c, 64c25c)
KING_VAR(float, g_sl64f754, 64f754)
KING_VAR(float, g_sl64fa40, 64fa40)
KING_VAR(float, g_sl650970, 650970)
KING_VAR(float, g_sl64c458, 64c458)

template <class R = int, class... A>
static R Sl_Com(BYTE* com, unsigned off, A... args)
{
    return (*(R(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

// spark p's colour at life t (1 at birth): from +68 toward +84 in the first quarter, then toward +100
static void Sl_Colour(const BYTE* p, float t, float* c)
{
    float f;
    int a, b;
    if (t < g_sl64c25c) {
        f = t * g_sl64f754;
        a = 68, b = 84;
    } else {
        f = (t - g_sl64c25c) * g_sl64fa40;
        a = 84, b = 100;
    }
    for (int i = 0; i < 4; i++)
        c[i] = (F(float, p, b + i * 4) - F(float, p, a + i * 4)) * f + F(float, p, a + i * 4);
}

static DWORD Sl_Pack(const float* c)
{
    int v = (int)(c[0] * g_sl64c458) << 8;
    v = (v | (int)(c[1] * g_sl64c458)) << 8;
    v = (v | (int)(c[2] * g_sl64c458)) << 8;
    return (DWORD)(v | (int)(c[3] * g_sl64c458));
}

extern "C" {

// [$L_594650] draws spark pool s (list +76, vertex buffer +80, material +84) when visible: each spark a line
// from its position (+28) back along its velocity (+4, scaled by -0.04 and at most 64c334 long), coloured by its
// age (+116) at both ends; 1 when drawn, 0 when not visible
int __fastcall Sparks_DrawLines(BYTE* s, void*, int a, BYTE* r)
{
    if (!CallThis<int>(Sl_DrawTested, s, a, r))
        return 0;
    int n = 0;
    unsigned off = 0;
    for (BYTE* p = F(BYTE*, s, 76); p; p = F(BYTE*, p, 0)) {
        float pos[3] = {F(float, p, 28), F(float, p, 32), F(float, p, 36)};
        float v[3] = {F(float, p, 4), F(float, p, 8), F(float, p, 12)}, t[3];
        CallThis(Sl_MulAssign, v, t, 0xBD23D70Au);
        if (CallC<double>(Sl_Length, v) > g_sl64c334) {
            const float* nv = CallC<const float*>(Sl_Normalized, t, v);
            v[0] = nv[0] * g_sl64c334, v[1] = nv[1] * g_sl64c334, v[2] = nv[2] * g_sl64c334;
        }
        float c1[4], c2[4];
        Sl_Colour(p, g_sl64c250 - F(float, p, 116), c1);
        Sl_Colour(p, g_sl64c250 - (F(float, p, 116) - g_sl650970), c2);
        BYTE* e = F(BYTE*, s, 80) + off;
        memcpy(e, pos, 12);
        F(DWORD, e, 12) = Sl_Pack(c1);
        F(float, e, 16) = v[0] + pos[0];
        F(float, e, 20) = v[1] + pos[1];
        F(float, e, 24) = v[2] + pos[2];
        F(DWORD, e, 28) = Sl_Pack(c2);
        n += 2;
        off += 32;
        if (off >= 131040)
            break;
    }
    if (n) {
        CallThis(Sl_Material, r, F(int, s, 84), 0, 0, 0);
        Sl_Com(g_sl720908, 80, 136, 1);
        Sl_Com(g_sl720908, 100, 2, 66, F(BYTE*, s, 80), n, 0);
    }
    return 1;
}

} // extern "C"

// === a moving body's step against the ground (FUN_58c1e0)

extern "C" void Mg_FrameTime(); KING_ALIAS("_Mg_FrameTime", "_Game_FrameTime")
extern "C" void Mg_PointsNext(); KING_ALIAS("_Mg_PointsNext", "@Points_Next@12")
extern "C" void Mg_CheckPoint(); KING_ALIAS("_Mg_CheckPoint", "_World_CheckPoint")
extern "C" void Mg_AddAssign(); KING_ALIAS("_Mg_AddAssign", "@Vec3_AddAssign@16")
extern "C" void FUN_58baa0(); KING_ALIAS("_FUN_58baa0", "_Body_Bounce")
ASM_PROC(FUN_4e7889)
KING_VAR(float, g_mg64c264, 64c264)
KING_VAR(double, g_mg64b6d0, 64b6d0)
KING_VAR(double, g_mg64d0f0, 64d0f0)
KING_VAR(float, g_mg64c258, 64c258)
KING_VAR(double, g_mg64b7c0, 64b7c0)
KING_VAR(float, g_mg64c334, 64c334)
KING_VAR(float, g_mg64c904, 64c904)
KING_VAR(float, g_mg64d494, 64d494)
KING_VAR(double, g_mg64b678, 64b678)
KING_VAR(float, g_mg64c878, 64c878)
KING_VAR(float, g_mg64c250, 64c250)
KING_VAR(float, g_mg64d4e8, 64d4e8)
KING_VAR(float, g_mg64c2f8, 64c2f8)
KING_VAR(char, s_mg67dcf4, 67dcf4)
KING_VAR(char, s_mg67dcd8, 67dcd8)
KING_VAR(char, s_mg67dcbc, 67dcbc)
KING_VAR(char, s_mg67dca0, 67dca0)

static Vec3 Mg_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

// sorts h into the four smallest distances d; its place, or -1
static int Mg_Insert(float* d, double h)
{
    if (!(h < d[3]))
        return -1;
    int k = 0;
    while (!(h < d[k]))
        k++;
    for (int j = 3; j > k; j--)
        d[j] = d[j - 1];
    d[k] = (float)h;
    return k;
}

// the step of position p by velocity s+788 over dt
static void Mg_Move(BYTE* s, Vec3* p, float dt)
{
    Vec3* v = (Vec3*)(s + 476);
    *v = Mg_Set((double)dt * F(float, s, 788), (double)dt * F(float, s, 792), (double)dt * F(float, s, 796));
    *p = Mg_Set((double)p->x + v->x, (double)p->y + v->y, (double)p->z + v->z);
}

extern "C" {

// [FUN_58c1e0] moves body s (position +416, velocity +788, frame +500, ground points from +5196) over the frame:
// near the ground the four nearest contacts decide a solved bounce (58baa0), else it falls by the root of
// its height (4e7889, mixed by k), with impulses when it moved too far; +3148 gets the step
void __fastcall Body_GroundStep(BYTE* s, void*, float k)
{
    float dt = (float)CallC<double>(Mg_FrameTime);
    Vec3* P = (Vec3*)(s + 416);
    float d[4] = {100.0f, 100.0f, 100.0f, 100.0f};
    Vec3 prev = *P;
    Vec3 tmp;
    const float* R = (const float*)(s + 500);
    if ((double)dt * F(float, s, 796) + P->z < g_mg64c264)
    {
        Vec3 pt = {0, 0, 0}, best = {0, 0, 0}, bestN = {0, 0, 0};
        for (int i = CallThis<int>(Mg_PointsNext, s, &pt); i; i = CallThis<int>(Mg_PointsNext, s, &pt))
        {
            double x = (double)pt.z * R[6] + (double)pt.y * R[3] + (double)pt.x * R[0] + R[9];
            float y = (float)((double)pt.x * R[1] + (double)pt.y * R[4] + (double)pt.z * R[7] + R[10]);
            float z = (float)((double)pt.z * R[8] + (double)pt.x * R[2] + (double)pt.y * R[5] + R[11]);
            BYTE* rec = s + i * 208;
            const float* G = (const float*)(rec + 5196);
            double d0 = G[0] - x;
            float d1 = (float)((double)G[1] - y);
            double d2 = (double)G[2] - z;
            if (!((double)d1 * d1 + (double)d2 * d2 + d0 * d0 < g_mg64b6d0))
                continue;
            const float* N = (const float*)(rec + 5208);
            double h = (x - G[0]) * N[0] + (((double)y - G[1]) * N[1] + ((double)z - G[2]) * N[2]);
            if (Mg_Insert(d, h) == 0)
            {
                best = *(const Vec3*)G;
                bestN = *(const Vec3*)N;
            }
        }
        float d0 = d[0];
        if (d[0] < g_mg64d0f0)
        {
            Vec3 a = {0, 0, 0}, b = {0, 0, 0};
            if (CallC<int>(FUN_58baa0, s + 500, s + 596, P, s + 428, s + 404, &best, &bestN, F(int, s, 396), 1.0f, &a, &b))
            {
                F(Vec3, s, 488) = Mg_Set((double)b.x * dt, (double)b.y * dt, (double)b.z * dt);
                F(Vec3, s, 476) = Mg_Set((double)a.x * dt, (double)a.y * dt, (double)a.z * dt);
                CallThis(Mg_AddAssign, P, &tmp, s + 476);
                CallThis(Mg_AddAssign, s + 428, &tmp, s + 488);
                Vec3 m;
                Vec3_MulComponents(&m, (const Vec3*)(s + 428), (const Vec3*)(s + 404));
                F(Vec3, s, 440) = Mg_Set((double)R[3] * m.y + (double)R[6] * m.z + (double)m.x * R[0],
                                         (double)R[4] * m.y + (double)R[1] * m.x + (double)R[7] * m.z,
                                         (double)R[5] * m.y + (double)R[2] * m.x + (double)R[8] * m.z);
                F(Vec3, s, 3148) = Mg_Set((double)dt * P->x, (double)dt * P->y, (double)dt * P->z);
                const float* w = (const float*)(s + 428);
                F(Vec3, s, 3160) = Mg_Set((double)dt * w[0], (double)dt * w[1], (double)dt * w[2]);
                return;
            }
        }
        for (int i = 0; i < 4; i++)
            d[i] = 100.0f;
        int n = F(int, s, 5132);
        for (int j = 0; j < n; j++)
        {
            const float* q = (const float*)(s + 4004 + j * 28);
            double x = (double)R[0] * q[0] + (double)R[6] * q[2] + (double)R[3] * q[1] + R[9];
            float y = (float)((double)R[1] * q[0] + (double)q[2] * R[7] + (double)R[4] * q[1] + R[10]);
            float z = (float)((double)q[1] * R[5] + (double)R[2] * q[0] + (double)q[2] * R[8] + R[11]);
            double h = (x - F(float, s, 964)) * F(float, s, 976) + ((double)y - F(float, s, 968)) * F(float, s, 980)
                       + ((double)z - F(float, s, 972)) * F(float, s, 984);
            Mg_Insert(d, h);
        }
        double t = x87_fabs((double)dt * F(float, s, 796) * dt * g_mg64c258) * g_mg64b7c0;
        double hh;
        if (t < d[0])
            hh = ((double)d[0] * g_mg64c258 - d0) * g_mg64c334;
        else
            hh = ((double)d[0] * g_mg64c904 - d0) * g_mg64d494;
        hh += d0;
        Vec3 np = *P;
        const float* N = (const float*)(s + 976);
        if (hh < g_mg64c264 && (double)P->z * N[2] + (double)P->y * N[1] + (double)P->x * N[0] < g_mg64b678)
        {
            double c1 = (double)P->z * N[1] - (double)N[2] * P->y;
            float c2 = (float)((double)N[2] * P->x - (double)N[0] * P->z);
            float c3 = (float)((double)N[0] * P->y - (double)P->x * N[1]);
            np = Mg_Set((double)c2 * N[2] - (double)c3 * N[1], (double)c3 * N[0] - c1 * N[2], c1 * N[1] - (double)c2 * N[0]);
            CallC(Mg_CheckPoint, &np, &s_mg67dcf4, 150.0f);
            *P = np;
            F(Vec3, s, 3148) = Mg_Set((double)dt * P->x, (double)dt * P->y, (double)dt * P->z);
            return;
        }
        float t1, t2;
        if (CallC<int>(FUN_4e7889, (float)((double)F(float, s, 796) * g_mg64c878), (float)-(double)F(float, s, 424),
                       (float)-hh, &t1, &t2))
        {
            float r;
            const char* tag;
            if (t1 < t2)
            {
                r = t2;
                tag = &s_mg67dcbc;
            }
            else
            {
                r = t1;
                tag = &s_mg67dcd8;
            }
            if (dt < x87_fabs(r))
                r = (float)((r > g_mg64c264 ? 1 : -1) * (double)dt);
            double f = ((double)g_mg64c250 - k) * r + (double)dt * k;
            Vec3* v = (Vec3*)(s + 476);
            *v = Mg_Set(f * F(float, s, 788), f * F(float, s, 792), f * F(float, s, 796));
            np = Mg_Set((double)v->x + P->x, (double)P->y + v->y, (double)P->z + v->z);
            CallC(Mg_CheckPoint, &np, tag, 150.0f);
            *P = np;
        }
        else
            Mg_Move(s, P, dt);
    }
    else
        Mg_Move(s, P, dt);
    Vec3 delta = Mg_Set((double)P->x - prev.x, (double)P->y - prev.y, (double)P->z - prev.z);
    float L1 = (float)Vec3_Length(P);
    if ((double)dt * g_mg64d4e8 < Vec3_Length(&delta) || L1 > g_mg64c2f8)
    {
        Vec3 h;
        CallVirt(s, 144, Vec3_Scaled(&h, P, 0.5f), 0);
        CallVirt(s, 152, Vec3_Scaled(&h, (const Vec3*)(s + 440), 0.5f), 0);
        Vec3_Length(P);
        CallVirt(s, 144, Vec3_Scaled(&h, P, 0.5f), 0);
        CallVirt(s, 152, Vec3_Scaled(&h, (const Vec3*)(s + 440), 0.5f), 0);
    }
    CallC(Mg_CheckPoint, P, &s_mg67dca0, 150.0f);
    Vec3_Scaled((Vec3*)(s + 3148), P, dt);
}

} // extern "C"

// === drawing a body, with its debug marks (FUN_590d20)

extern "C" void Bd_DrawMarked(); KING_ALIAS("_Bd_DrawMarked", "@Item_DrawMarked@16")
extern "C" void Bd_PointsNext(); KING_ALIAS("_Bd_PointsNext", "@Points_Next@12")
extern "C" void Bd_CameraGet(); KING_ALIAS("_Bd_CameraGet", "@Camera_Get@12")
extern "C" void Bd_DrawLine(); KING_ALIAS("_Bd_DrawLine", "_Window_DrawLine")
KING_VAR(int, g_bd696be4, 696be4)
KING_VAR(BYTE*, g_bd6ef1fc, 6ef1fc)
KING_VAR(float, g_bd64c250, 64c250)
KING_VAR(float, g_bd64c258, 64c258)
KING_VAR(float, g_bd65081c, 65081c)
KING_VAR(double, g_bd64b820, 64b820)

static BYTE* Bd_Window() { return F(BYTE*, F(BYTE*, g_bd6ef1fc, 464), 1948); }

// a mark of color c at view point v (x, depth y, z) when in front
static void Bd_Mark(const float* v, int c)
{
    if (!(v[1] > g_bd64c250))
        return;
    BYTE* w = Bd_Window();
    int ww = F(WORD, w, 0), wh = F(WORD, w, 2);
    int sx = (int)(v[0] / (double)v[1] * ww * g_bd64c258 + ww / 2);
    int sy = (int)(wh / 2 - v[2] / (double)v[1] * wh * g_bd64c258);
    CallC(Bd_DrawLine, w, sx - 2, sy - 2, sx + 2, sy + 2, c, 0);
    CallC(Bd_DrawLine, Bd_Window(), sx + 2, sy - 2, sx - 2, sy + 2, c, 0);
    CallC(Bd_DrawLine, Bd_Window(), sx, sy - 3, sx, sy + 3, 0, 0);
    CallC(Bd_DrawLine, Bd_Window(), sx - 3, sy, sx + 3, sy, 0, 0);
}

extern "C" {

// [FUN_590d20] draws body item s (its owner at -256) with (a, ctx); at debug level 2 marks its contact points
// (mode 3) or a wire sphere around +3180 (mode 2) in the debug view
int __fastcall Item_DrawBody(BYTE* s, void*, int a, BYTE* ctx)
{
    BYTE* owner = s - 256;
    CallVirt(owner, 232);
    int ret = CallThis<int>(Bd_DrawMarked, s, a, ctx);
    if (g_bd696be4 <= 1 || g_bd696be4 != 2)
        return ret;
    BYTE* W = F(BYTE*, g_bd6ef1fc, 464);
    float M[12];
    memcpy(M, W + 2240 + F(int, W, 8224) * 48, 48);
    float pt[3] = {0, 0, 0};
    if (F(int, s, 3720) == 3)
    {
        for (int i = CallThis<int>(Bd_PointsNext, owner, pt); i; i = CallThis<int>(Bd_PointsNext, owner, pt))
        {
            double a0 = (double)pt[0] * F(float, s, 244) + (double)pt[2] * F(float, s, 268) + (double)pt[1] * F(float, s, 256) + F(float, s, 280);
            double b0 = (double)pt[2] * F(float, s, 272) + (double)pt[1] * F(float, s, 260) + (double)pt[0] * F(float, s, 248) + F(float, s, 284);
            double c0 = (double)pt[2] * F(float, s, 276) + (double)pt[1] * F(float, s, 264) + (double)pt[0] * F(float, s, 252) + F(float, s, 288);
            float v[3];
            v[2] = (float)(M[2] * a0 + M[8] * c0 + M[5] * b0 + M[11]);
            v[1] = (float)(M[1] * a0 + M[7] * c0 + M[4] * b0 + M[10]);
            v[0] = (float)(M[0] * a0 + M[6] * c0 + M[3] * b0 + M[9]);
            Bd_Mark(v, 5);
        }
    }
    if (F(int, s, 3720) != 2)
        return ret;
    BYTE cm[48];
    const float* cam = CallThis<const float*>(Bd_CameraGet, g_bd6ef1fc, cm);
    float d[3];
    d[0] = (float)((double)F(float, s, 280) - cam[9]);
    d[1] = (float)((double)F(float, s, 284) - cam[10]);
    d[2] = (float)((double)F(float, s, 288) - cam[11]);
    int n = (int)(g_bd65081c / Vec3_Length((const Vec3*)d));
    if (n > 20)
        n = 20;
    int color = 4;
    const float* p = (const float*)(s + 3180);
    float c0[3];
    c0[0] = (float)((double)F(float, s, 268) * p[2] + (double)F(float, s, 256) * p[1] + (double)F(float, s, 244) * p[0] + F(float, s, 280));
    c0[1] = (float)((double)F(float, s, 272) * p[2] + (double)F(float, s, 248) * p[0] + (double)F(float, s, 260) * p[1] + F(float, s, 284));
    c0[2] = (float)((double)F(float, s, 276) * p[2] + (double)F(float, s, 252) * p[0] + (double)F(float, s, 264) * p[1] + F(float, s, 288));
    float q[3] = {c0[0], c0[1], c0[2]};
    for (int j = 0; j < n; j++)
    {
        for (int k = 0; k < 2; k++)
        {
            if (k || j)
            {
                Matrix rm;
                float ang = (float)(j * g_bd64b820 / n);
                const float* R = (k ? Matrix_RotationX(&rm, ang) : Matrix_RotationY(&rm, ang))->m;
                double r = F(float, s, 3016);
                const float* Z = &g_axisZ.x;
                double x = (double)Z[2] * R[6] + (double)Z[1] * R[3] + (double)Z[0] * R[0] + R[9];
                double y = (double)Z[2] * R[7] + (double)Z[1] * R[4] + (double)Z[0] * R[1] + R[10];
                double z = (double)Z[2] * R[8] + (double)Z[1] * R[5] + (double)Z[0] * R[2] + R[11];
                float t[3] = {(float)(x * r), (float)(y * r), (float)(z * r)};
                q[0] = (float)((double)t[0] + c0[0]);
                q[1] = (float)((double)t[1] + c0[1]);
                q[2] = (float)((double)t[2] + c0[2]);
                color = 5;
            }
            float v[3];
            v[2] = (float)((double)M[8] * q[2] + (double)M[2] * q[0] + (double)M[5] * q[1] + M[11]);
            v[1] = (float)((double)M[7] * q[2] + (double)M[1] * q[0] + (double)M[4] * q[1] + M[10]);
            v[0] = (float)((double)M[6] * q[2] + (double)M[3] * q[1] + (double)M[0] * q[0] + M[9]);
            q[0] = v[0];
            q[1] = v[1];
            q[2] = v[2];
            Bd_Mark(v, color);
        }
    }
    return ret;
}

} // extern "C"

// === a body's bounce off the ground (FUN_58baa0)

KING_VAR(Vec3, g_gb6f63d8, 6f63d8)                // gravity
KING_VAR(double, g_gb64b678, 64b678)

static Vec3 Gb_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_58baa0] the bounce of a body (frame R, inverse inertia frame Ri, velocity v, spin w, inertia scale k)
// off ground point g with normal n, for restitution e: the velocity change *dv and spin change *dw, 1; 0 and
// zeros when the contact point is not moving into the ground
int __cdecl Body_Bounce(const Matrix* R, const Matrix* Ri, const Vec3* v, const Vec3* w, const Vec3* k, const Vec3* g,
                        const Vec3* n, float e, float unused, Vec3* dv, Vec3* dw)
{
    (void)unused;
    float dt = (float)F(double, g_bd6ef1fc, 2960);
    const float* Rm = R->m;
    Vec3 r = Gb_Set((double)g->x - Rm[9], (double)g->y - Rm[10], (double)g->z - Rm[11]);
    Vec3 u, c, t, s, wr, cr, vc, gv, vt;
    Vec3_Normalized(&u, &r);
    Vec3_Normalized(&t, Vec3_Cross(&c, n, &u));
    Vec3_Cross(&s, &u, &t);
    Vec3_RotateBy(&wr, w, R);
    Vec3_Cross(&cr, &wr, &r);
    Vec3_Add(&vc, &cr, v);
    gv = Gb_Set((double)g_gb6f63d8.x * dt, (double)g_gb6f63d8.y * dt, (double)g_gb6f63d8.z * dt);
    vt = Gb_Set((double)vc.x + gv.x, (double)vc.y + gv.y, (double)vc.z + gv.z);
    bool ok = (double)t.x * t.x + (double)t.z * t.z + (double)t.y * t.y != g_gb64b678
              && !((double)vt.y * r.y + (double)vt.z * r.z + (double)vt.x * r.x < g_gb64b678)
              && !((double)n->z * vt.z + (double)n->y * vt.y + (double)n->x * vt.x > g_mg64c264);
    if (!ok)
    {
        *dv = g_zeroVec;
        *dw = g_zeroVec;
        return 0;
    }
    Matrix m516 = *R, m468 = *Ri, m672 = g_identity, m624, m324, m240, m192, m144, m420, m372, m96;
    m516.m[9] = m516.m[10] = m516.m[11] = 0;
    m468.m[9] = m468.m[10] = m468.m[11] = 0;
    *(Vec3*)&m672.m[0] = t;
    *(Vec3*)&m672.m[3] = s;
    *(Vec3*)&m672.m[6] = u;
    Matrix_Inverse(&m624, &m672);
    Matrix_Multiply(&m324, &m516, &m624);
    Matrix_Inverse(&m240, &m324);
    Matrix_Multiply(&m192, &m240, Matrix_ScaleColumns(&m144, &g_identity, k));
    Matrix_Multiply(&m420, &m192, &m324);
    double rr = ((double)r.x * r.x + (double)r.z * r.z + (double)r.y * r.y) * e;
    m372 = m420;
    m372.m[0] = (float)(rr + m420.m[0]);
    m372.m[4] = (float)(rr + m372.m[4]);
    Matrix_Inverse(&m96, &m372);
    Vec3 c564, v540, v552, v696, v576;
    Vec3_RotateBy(&v540, Vec3_Cross(&c564, &r, &g_gb6f63d8), &m624);
    v552 = Gb_Set(-((double)v540.x * e), 0, 0);
    Vec3_RotateBy(&v696, &v552, &m96);
    Vec3_RotateBy(&v576, &v696, &m672);
    Vec3_Cross(dv, &r, &v576);
    Vec3_RotateBy(dw, &v696, &m240);
    return 1;
}

} // extern "C"

// === a body's planes and bounds (FUN_58d2b0)

KING_VAR(float, g_pb650330, 650330)

extern "C" {

// [FUN_58d2b0] body s: its +5132 planes (+4548.., 28 bytes: normal, distance) turned by the frame +692 into the
// world planes +3988.. (normal, distance, the foot point), the lowest distance +5140, and the bounds of its
// points (+3592.. / +3832..) in +5108 .. +5128
void __fastcall Body_UpdatePlanes(BYTE* s, void*)
{
    int n = F(int, s, 5132);
    F(float, s, 5140) = n ? 10000.0f : 0;
    const float* R = (const float*)(s + 692);
    for (int i = 0; i < n; i++)
    {
        const float* N = (const float*)(s + 4548 + i * 28);
        float* A = (float*)(s + 3988 + i * 28);
        double nd = -(double)N[3];
        float px = (float)(nd * N[0]), py = (float)(nd * N[1]);
        double pz = nd * N[2];
        float wx = (float)(pz * R[6] + (double)py * R[3] + (double)px * R[0] + R[9]);
        float wy = (float)(pz * R[7] + (double)py * R[4] + (double)px * R[1] + R[10]);
        double wz = pz * R[8] + (double)py * R[5] + (double)px * R[2] + R[11];
        A[0] = (float)((double)R[6] * N[2] + (double)N[1] * R[3] + (double)R[0] * N[0]);
        A[1] = (float)((double)R[1] * N[0] + (double)R[7] * N[2] + (double)N[1] * R[4]);
        A[2] = (float)((double)N[1] * R[5] + (double)R[2] * N[0] + (double)N[2] * R[8]);
        float mx = (float)-(double)wx, my = (float)-(double)wy;
        double d = (double)A[0] * mx + A[2] * -wz + (double)my * A[1];
        A[3] = (float)d;
        double t = -d;
        if (t < F(float, s, 5140))
            F(float, s, 5140) = (float)t;
        double k = -(double)A[3];
        A[4] = (float)(k * A[0]);
        A[5] = (float)(k * A[1]);
        A[6] = (float)(k * A[2]);
    }
    float minX = 1000000.0f, maxX = -1000000.0f, maxY = -1000000.0f, minY = 1000000.0f, maxZ = -1000000.0f;
    double minZ = g_pb650330;
    for (int pass = 0; pass < 2; pass++)
    {
        int cnt = F(int, s, pass ? 3576 : 3572);
        const float* p = (const float*)(s + (pass ? 3832 : 3592));
        for (int i = 0; i < cnt; i++, p += 3)
        {
            if (maxX < p[0])
                maxX = p[0];
            if (minX > p[0])
                minX = p[0];
            if (maxY < p[1])
                maxY = p[1];
            if (minY > p[1])
                minY = p[1];
            if (maxZ < p[2])
                maxZ = p[2];
            if (minZ > p[2])
                minZ = p[2];
        }
    }
    F(float, s, 5128) = (float)minZ;
    F(float, s, 5108) = minX;
    F(float, s, 5112) = maxX;
    F(float, s, 5116) = maxY;
    F(float, s, 5120) = minY;
    F(float, s, 5124) = maxZ;
}

} // extern "C"

// === the body a body's points run into (FUN_58f560)

extern "C" void Ob_SubAssign(); KING_ALIAS("_Ob_SubAssign", "@Vec3_SubAssign@16")

// point p of body s (frame +500) in the world
static Vec3 Ob_World(BYTE* s, const Vec3& p)
{
    const float* M = (const float*)(s + 500);
    Vec3 w;
    w.x = (float)((double)p.z * M[6] + (double)p.y * M[3] + (double)p.x * M[0] + M[9]);
    w.y = (float)((double)p.z * M[7] + (double)p.y * M[4] + (double)p.x * M[1] + M[10]);
    w.z = (float)((double)p.z * M[8] + (double)p.y * M[5] + (double)p.x * M[2] + M[11]);
    return w;
}

static bool Ob_Next(BYTE* s, Vec3* p) { return Points_NextFirst(s, 0, (Vec3Bits*)p) != 0; }

extern "C" {

// [FUN_58f560] the first of body s's six neighbours (+3508, up to an empty slot) that one of its points
// (Points_NextFirst) falls in: a body (+392 == 2) of kind 3 by its shape, of kind 2 within its sphere
// (+3436, radius +3272) and then its shape, a sphere (+392 == 1) by the sphere; 0 when none
BYTE* __fastcall Body_FindOverlap(BYTE* s, void*)
{
    Game_FrameTime();
    for (int i = 0; i < 6; i++)
    {
        BYTE* o = F(BYTE*, s, 3508 + i * 4);
        if (!o)
            return 0;
        int type = F(int, o, 392);
        Vec3 pt = g_zeroVec, c = g_zeroVec, tmp;
        if (type == 2)
        {
            int k = F(int, o, 3976);
            if (k == 3)
            {
                if (!Ob_Next(s, &pt))
                    continue;
                bool found = false;
                do
                {
                    if (found)
                        continue;
                    Vec3 w = Ob_World(s, pt);
                    Matrix m;
                    const float* R = CallThis<const float*>(FUN_57dec0, o, &m);
                    pt.x = (float)((double)w.z * R[6] + (double)w.y * R[3] + (double)w.x * R[0]);
                    pt.y = (float)((double)w.z * R[7] + (double)w.y * R[4] + (double)w.x * R[1]);
                    pt.z = (float)((double)w.z * R[8] + (double)w.y * R[5] + (double)w.x * R[2]);
                    if (Body_ContainsPoint(o, 0, &pt.x))
                        found = true;
                } while (Ob_Next(s, &pt));
                if (found)
                    return o;
            }
            else if (k == 2)
            {
                Matrix m1;
                const float* R = CallThis<const float*>(FUN_57de90, o, &m1);
                const float* q = (const float*)(o + 3436);
                c.x = (float)((double)R[6] * q[2] + (double)R[3] * q[1] + (double)q[0] * R[0] + R[9]);
                c.y = (float)((double)R[7] * q[2] + (double)R[1] * q[0] + (double)R[4] * q[1] + R[10]);
                c.z = (float)((double)R[8] * q[2] + (double)R[2] * q[0] + (double)R[5] * q[1] + R[11]);
                float r2 = (float)((double)F(float, o, 3272) * F(float, o, 3272));
                if (!Ob_Next(s, &pt))
                    continue;
                bool found = false;
                do
                {
                    if (found)
                        continue;
                    pt = Ob_World(s, pt);
                    CallThis(Ob_SubAssign, &pt, &tmp, &c);
                    if ((double)pt.x * pt.x + (double)pt.y * pt.y + (double)pt.z * pt.z < r2)
                        found = true;
                } while (Ob_Next(s, &pt));
                if (!found)
                    continue;
                if (!Ob_Next(s, &pt))
                    continue;
                found = false;
                do
                {
                    if (found)
                        continue;
                    Vec3 w = Ob_World(s, pt);
                    Matrix m2;
                    const Matrix* R2 = CallThis<const Matrix*>(FUN_57dec0, o, &m2);
                    pt = *Vec3_RotateBy(&tmp, &w, R2);
                    if (Body_ContainsPoint(o, 0, &pt.x))
                        found = true;
                } while (Ob_Next(s, &pt));
                if (found)
                    return o;
            }
        }
        else if (type == 1)
        {
            Matrix m1;
            const Matrix* R = CallThis<const Matrix*>(FUN_57de90, o, &m1);
            c = *Vec3_MulMatrix(&tmp, (const Vec3*)(o + 3436), R);
            float r2 = (float)((double)F(float, o, 3272) * F(float, o, 3272));
            if (!Ob_Next(s, &pt))
                continue;
            do
            {
                pt = *Vec3_MulMatrix(&tmp, &pt, (const Matrix*)(s + 500));
                CallThis(Ob_SubAssign, &pt, &tmp, &c);
                if (Vec3_Dot(&pt, &pt) < r2)
                    return o;
            } while (Ob_Next(s, &pt));
        }
    }
    return 0;
}

} // extern "C"

// === a list of pointers and its ordering (FUN_592f90 with $L_592fe0)

extern "C" {

// [FUN_592f90] list s (head +4, tail +8) gets a node (12 bytes) holding p at its end
void __fastcall PtrList_Append(BYTE* s, void*, void* p)
{
    BYTE* n = (BYTE*)operator new(12);
    if (n)
    {
        ListNode_Construct(n, 0);
        F(void*, n, 8) = p;
    }
    if (!F(BYTE*, s, 4))
    {
        F(BYTE*, s, 8) = n;
        F(BYTE*, s, 4) = n;
        return;
    }
    ListNode_InsertBefore(F(BYTE*, s, 8), 0, n);
    F(BYTE*, s, 8) = n;
}

// ($L_592fe0, inside FUN_592f90) qsort order of 8-byte records by their float +4: 1, -1 or 0
int __cdecl Rec_CompareF4(const BYTE* a, const BYTE* b)
{
    if (F(float, a, 4) > F(float, b, 4))
        return 1;
    if (F(float, a, 4) < F(float, b, 4))
        return -1;
    return 0;
}

} // extern "C"

// === settling a body's frame onto a plane (a method at $L_58d770)

extern "C" void St_AddAssign(); KING_ALIAS("_St_AddAssign", "@Vec3_AddAssign@16")
KING_VAR(double, g_st64b5a0, 64b5a0)
KING_VAR(double, g_st650808, 650808)
KING_VAR(double, g_st64b658, 64b658)

static Vec3 St_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [$L_58d770] body s's frame f set down on the plane through p with normal n (axis turned by r) into out: lifted
// clear of the plane by its planes' depths (+3988.., lowest +5140); nearly flat it is just raised to its lowest
// point (Points_Next), else tilted up to four times about its lowest point towards the plane
void __fastcall Body_SettleOnPlane(BYTE* s, void*, float* out, const float* f, const Matrix* r, const Vec3* n, const Vec3* p)
{
    g_bp6f6568++;
    float M[6][12];
    for (int k = 0; k < 6; k++)
        memcpy(M[k], &g_identity, 48);
    memcpy(M[0], f, 48);
    const float* N = &n->x;
    const float* P = &p->x;
    double dx = (double)M[0][9] - P[0], dy = (double)M[0][10] - P[1], dz = (double)M[0][11] - P[2];
    float d0 = (float)(dx * N[0] + dy * N[1] + dz * N[2]);
    float minDot = 1.0f;
    float low = F(float, s, 5140);
    for (int i = 0; i < F(int, s, 5132); i++)
    {
        const float* A = (const float*)(s + 3988 + i * 28);
        double x = (double)f[6] * A[2] + (double)A[1] * f[3] + (double)f[0] * A[0];
        double y = (double)A[1] * f[4] + (double)f[1] * A[0] + (double)A[2] * f[7];
        double z = (double)f[2] * A[0] + (double)A[1] * f[5] + (double)A[2] * f[8];
        double dot = x * N[0] + y * N[1] + z * N[2];
        if (dot < minDot)
        {
            minDot = (float)dot;
            low = (float)-(double)A[3];
        }
    }
    if ((double)low + g_st64b5a0 > d0)
    {
        double k = ((double)d0 - low) - g_st64b5a0;
        float kx = (float)(k * N[0]), ky = (float)(k * N[1]);
        double kz = k * N[2];
        Vec3 np = St_Set((double)M[0][9] - kx, (double)M[0][10] - ky, M[0][11] - kz);
        M[0][9] = np.x;
        M[0][10] = np.y;
        M[0][11] = np.z;
        Vec3 q = St_Set((double)np.x - P[0], (double)np.y - P[1], (double)np.z - P[2]);
        d0 = (float)((double)q.y * N[1] + (double)N[2] * q.z + (double)q.x * N[0]);
    }
    Vec3Bits pt;
    float* ptf = (float*)&pt;
    float dists[48];
    float best = 100.0f;
    Vec3 tmp;
    if (!(minDot <= g_st650808))
    {
        ptf[0] = ptf[1] = ptf[2] = 0;
        int j = 0;
        const float* m = M[0];
        while (Points_Next(s, 0, &pt))
        {
            Vec3 w = St_Set((double)m[0] * ptf[0] + (double)m[6] * ptf[2] + (double)m[3] * ptf[1] + m[9],
                            (double)m[1] * ptf[0] + (double)m[7] * ptf[2] + (double)m[4] * ptf[1] + m[10],
                            (double)m[2] * ptf[0] + (double)m[8] * ptf[2] + (double)m[5] * ptf[1] + m[11]);
            Vec3 v = St_Set((double)w.x - P[0], (double)w.y - P[1], (double)w.z - P[2]);
            double d = (double)v.y * N[1] + (double)N[2] * v.z + (double)v.x * N[0];
            if (j < 48)
                dists[j] = (float)d;
            j++;
            if (d < best)
                best = (float)d;
        }
        if (best <= g_st64b5a0)
        {
            double k = g_st64b5a0 - best;
            Vec3 up = St_Set(k * N[0], k * N[1], k * N[2]);
            CallThis(St_AddAssign, &M[0][9], &tmp, &up);
        }
        memcpy(out, M[0], 48);
        return;
    }
    int k = 0;
    Vec3 nn = *n;
    for (;;)
    {
        const float* m = M[k];
        Vec3 bestW = g_zeroVec, bestPt = g_zeroVec;
        best = 100.0f;
        int j = 0;
        while (Points_Next(s, 0, &pt))
        {
            Vec3 w = St_Set((double)ptf[2] * m[6] + (double)ptf[1] * m[3] + (double)ptf[0] * m[0] + m[9],
                            (double)ptf[0] * m[1] + (double)ptf[2] * m[7] + (double)ptf[1] * m[4] + m[10],
                            (double)ptf[2] * m[8] + (double)ptf[0] * m[2] + (double)ptf[1] * m[5] + m[11]);
            Vec3 v = St_Set((double)w.x - P[0], (double)w.y - P[1], (double)w.z - P[2]);
            double d = (double)v.y * N[1] + (double)N[2] * v.z + (double)v.x * N[0];
            if (j < 48)
                dists[j] = (float)d;
            j++;
            if (d < best)
            {
                bestW = w;
                bestPt.x = ptf[0];
                bestPt.y = ptf[1];
                bestPt.z = ptf[2];
                best = (float)d;
            }
        }
        if (best > g_st64b658 || k == 3)
            break;
        double r2 = (double)d0 * d0;
        float bp2 = (float)((double)bestPt.y * bestPt.y + (double)bestPt.z * bestPt.z + (double)bestPt.x * bestPt.x);
        float side = r2 > bp2 ? 1.0f : (float)x87_sqrt(bp2 - r2);
        Vec3 mpos = St_Set(m[9], m[10], m[11]);
        ptf[0] = mpos.x;
        ptf[1] = mpos.y;
        ptf[2] = mpos.z;
        Vec3 e = St_Set((double)bestW.x - mpos.x, (double)bestW.y - mpos.y, (double)bestW.z - mpos.z);
        Vec3 a, axis, b, c1, c2, c3, sum, c4, c5;
        Vec3_Normalized(&axis, Vec3_Cross(&a, &e, &nn));
        Vec3_Cross(&b, &nn, &axis);
        Vec3_Scaled(&c1, &b, side);
        Vec3_Scaled(&c2, &nn, d0);
        Vec3_Sub(&c3, &mpos, &c2);
        Vec3_Add(&sum, &c3, &c1);
        Vec3_Sub(&c4, &sum, &mpos);
        Vec3_Cross(&c5, &e, &c4);
        double dot = Vec3_Dot(&axis, &c5);
        float ang = (float)x87_asin(dot / bp2);
        Vec3 ra;
        Matrix look, prod;
        Vec3_RotateBy(&ra, &axis, r);
        Matrix_LookAt(&look, g_zeroVec, ra, ang);
        Matrix_Multiply(&prod, &look, (const Matrix*)M[k]);
        memcpy(M[k + 1], &prod, 48);
        k++;
        if (k >= 4)
            break;
    }
    if (best < g_st64b658)
    {
        Vec3 up;
        Vec3_Scaled(&up, n, (float)(g_st64b5a0 - best));
        CallThis(St_AddAssign, &M[3][9], &tmp, &up);
        memcpy(out, M[3], 48);
    }
    else
        memcpy(out, M[k], 48);
}

} // extern "C"

// === an item's points against the scene (FUN_589640)

extern "C" void Pb_NodeInit(); KING_ALIAS("_Pb_NodeInit", "@Node_Init@8")
extern "C" void Pb_Get800(); KING_ALIAS("_Pb_Get800", "@SceneObj_Get800@8")
extern "C" void Pb_Matrix1(); KING_ALIAS("_Pb_Matrix1", "@SceneObj_Matrix1@12")
extern "C" void Pb_PointsNext(); KING_ALIAS("_Pb_PointsNext", "@Points_Next@12")
extern "C" void Pb_PointsFirst(); KING_ALIAS("_Pb_PointsFirst", "@Points_NextFirst@12")
extern "C" void Pb_HitObject(); KING_ALIAS("_Pb_HitObject", "_Query_HitObject")
extern "C" void Pb_Query1(); KING_ALIAS("_Pb_Query1", "_Query_4e67d1")
extern "C" void Pb_Query2(); KING_ALIAS("_Pb_Query2", "_Query_4e6ac4")
extern "C" void Pb_MulAssign(); KING_ALIAS("_Pb_MulAssign", "@Vec3_MulAssign@16")
KING_VAR(float, g_pb64c258, 64c258)
KING_VAR(double, g_pb64b678, 64b678)
KING_VAR(double, g_pb6506b0, 6506b0)
KING_VAR(double, g_pb64b670, 64b670)
KING_VAR(float, g_pb64c250, 64c250)

static Vec3 Pb_Through(const float* M, const Vec3& p)
{
    Vec3 r;
    r.x = (float)((double)M[6] * p.z + (double)M[3] * p.y + (double)M[0] * p.x + M[9]);
    r.y = (float)((double)M[7] * p.z + (double)M[4] * p.y + (double)M[1] * p.x + M[10]);
    r.z = (float)((double)M[8] * p.z + (double)M[5] * p.y + (double)M[2] * p.x + M[11]);
    return r;
}

// (p - item +5152) along the item's plane normal +5164
static double Pb_Side(BYTE* it, const Vec3& p)
{
    double x = (double)p.x - F(float, it, 5152), y = (double)p.y - F(float, it, 5156), z = (double)p.z - F(float, it, 5160);
    return z * F(float, it, 5172) + y * F(float, it, 5168) + x * F(float, it, 5164);
}

static Vec3 Pb_Diff(const float* a, const Vec3& b)
{
    Vec3 r;
    r.x = (float)((double)a[0] - b.x);
    r.y = (float)((double)a[1] - b.y);
    r.z = (float)((double)a[2] - b.z);
    return r;
}

extern "C" {

// [FUN_589640] each point of item it (flag 512: from the first, its own frame; else frame M from the hit object)
// swept against the scene (Query_4e67d1, else with flag 16 when off the item's plane and flag 32 ahead of its
// face, Query_4e6ac4) into the contact list out (+2816 count, at most 63: points, normals +768, hits +2820,
// shares +2048, kinds +1536, depths +2560, point indexes +3076) and the item's own records (+5144 + 208 i);
// the number of hits
int __cdecl Item_SweepPoints(BYTE* it, const Matrix* Mx, BYTE* out, DWORD flags)
{
    BYTE nd[208];
    CallThis(Pb_NodeInit, nd);
    BYTE* obj = CallThis<BYTE*>(Pb_Get800, it);
    Vec3 pt;
    pt.x = pt.y = pt.z = 0;
    Matrix Wm;
    CallThis(Pb_Matrix1, it, &Wm);
    const float* W = Wm.m;
    const float* M = Mx->m;
    int hits = 0;
    for (;;)
    {
        int i = (flags & 512) ? CallThis<int>(Pb_PointsFirst, it, &pt) : CallThis<int>(Pb_PointsNext, it, &pt);
        if (!i)
            return hits;
        Vec3 a, b;
        BYTE* hit;
        if (flags & 512)
        {
            a.x = W[9];
            a.y = W[10];
            a.z = W[11];
            b = Pb_Through(W, pt);
            hit = obj;
        }
        else
        {
            a = Pb_Through(W, pt);
            b = Pb_Through(M, pt);
            hit = CallC<BYTE*>(Pb_HitObject, obj, &Wm.m[9], &a);
        }
        if (!hit)
        {
            hit = obj;
            CallThis((void (*)())F(void**, it, 0)[7], it, 1);
        }
        int r = CallC<int>(Pb_Query1, hit, &a, &b, nd);
        if (!r)
        {
            bool skip = false;
            if (flags & 16)
            {
                double d1 = Pb_Side(it, a);
                float d2 = (float)Pb_Side(it, b);
                int two = F(int, it, 5188);
                if (two)
                {
                    d1 = x87_fabs(d1);
                    d2 = (float)x87_fabs((double)d2);
                }
                double lim = (double)F(float, it, 5144) - g_pb64c258;
                if (d1 < lim && d2 < lim)
                {
                    if (two)
                        skip = d1 > g_pb64b678 && d2 > g_pb64b678;
                    else
                        skip = d1 > g_pb64c258 && d2 > g_pb64c258;
                }
                if (!skip && (flags & 32))
                {
                    const float* u = (const float*)(it + 5164);
                    const float* v = (const float*)(it + 5176);
                    Vec3 c, tmp;
                    c.x = (float)((double)v[1] * u[2] - (double)v[2] * u[1]);
                    c.y = (float)((double)v[2] * u[0] - (double)u[2] * v[0]);
                    c.z = (float)((double)u[1] * v[0] - (double)v[1] * u[0]);
                    if ((double)c.z * g_axisZ.z + (double)c.y * g_axisZ.y + (double)c.x * g_axisZ.x < g_pb64b678)
                        CallThis(Pb_MulAssign, &c, &tmp, -1.0f);
                    double x = (double)b.x - F(float, it, 5152), y = (double)b.y - F(float, it, 5156);
                    double z = (double)b.z - F(float, it, 5160);
                    if (x * c.x + z * c.z + y * c.y > F(float, it, 5148))
                        skip = true;
                }
            }
            if (!skip)
                r = CallC<int>(Pb_Query2, hit, &a, &b, nd);
        }
        int k = F(int, out, 2816);
        if (k >= 63)
        {
            while (CallThis<int>(Pb_PointsNext, it, &pt))
                ;
            return hits;
        }
        F(int, out, 2820 + 4 * k) = r & 1;
        F(float, out, 2048 + 4 * k) = 1.0f;
        if (!F(int, out, 2820 + 4 * k))
            continue;
        F(float, out, 2048 + 4 * k) = 0;
        hits++;
        F(Vec3, out, 12 * k) = F(Vec3, nd, 52);
        F(Vec3, out, 12 * (k + 64)) = F(Vec3, nd, 64);
        F(int, out, 3076 + 4 * k) = i - 1;
        DWORD f = F(DWORD, it, 5188 + 4 * i) | 1;
        F(DWORD, it, 5188 + 4 * i) = f;
        F(DWORD, it, 5268 + 4 * i) = f;
        memcpy(it + 5144 + 208 * i, nd, 208);
        F(int, out, 1536 + 4 * k) = F(int, nd, 116);
        Vec3 t1 = Pb_Diff((const float*)(nd + 88), a);
        Vec3 t2 = Pb_Diff((const float*)(nd + 52), a);
        Vec3 t3 = Pb_Diff((const float*)(nd + 52), b);
        const Vec3& n = F(Vec3, nd, 64);
        F(float, out, 2560 + 4 * k) = (float)((double)t3.z * n.z + (double)t3.y * n.y + (double)t3.x * n.x);
        float dd = (float)Vec3_Dot(&t1, &t1);
        double ee = Vec3_Dot(&t2, &t2);
        if (!(dd < g_pb6506b0))
        {
            double q = ee / dd;
            if (q > g_pb64b670)
                q = g_pb64c250;
            F(float, out, 2048 + 4 * F(int, out, 2816)) = (float)x87_sqrt(q);
        }
        F(int, out, 2816)++;
    }
}

} // extern "C"

// === a body's points reset onto its plane (a method at $L_58e290)

extern "C" void Bp_ActivateAll(); KING_ALIAS("_Bp_ActivateAll", "@Points_ActivateAll@8")
extern "C" void FUN_58e7f0(); KING_ALIAS("_FUN_58e7f0", "@MovingItem_CheckFrame@20")
KING_VAR(char, g_bp67dda8, 67dda8)
KING_VAR(double, g_bp64b5a0, 64b5a0)
KING_VAR(double, g_bp64b898, 64b898)
KING_VAR(double, g_bp6506c0, 6506c0)
KING_VAR(double, g_bp64b670, 64b670)

// the body's height over its plane (point +1516, normal +1528) at +1996
static double Bp_Height(BYTE* s)
{
    double x = (double)F(float, s, 1996) - F(float, s, 1516);
    double y = (double)F(float, s, 2000) - F(float, s, 1520);
    double z = (double)F(float, s, 2004) - F(float, s, 1524);
    return z * F(float, s, 1536) + y * F(float, s, 1532) + x * F(float, s, 1528);
}

// the lift +1732: the base +2008 moved k along v
static void Bp_Lift(BYTE* s, const float* v, double k)
{
    Vec3& L = F(Vec3, s, 1732);
    L.x = (float)((double)(float)(k * v[0]) + F(float, s, 2008));
    L.y = (float)((double)(float)(k * v[1]) + F(float, s, 2012));
    L.z = (float)(k * v[2] + F(float, s, 2016));
}

// lifted when it sinks below its rest height h
static void Bp_LiftBelow(BYTE* s, double d, float h)
{
    if (!((double)h + g_bp64b5a0 > d))
        return;
    Bp_Lift(s, (const float*)(s + 1528), (double)h - d + g_bp64b5a0);
}

extern "C" {

// [$L_58e290] body s with points (+1432) set again with flags (6: onto its plane by its kind +1672: 1 a box,
// whose frame +1696 is solved again; 3 a wheel, its lift scaled on a slope; 2 and 4 by +3284), its contact
// records cleared and refilled (FUN_58e7f0) and the kind found again (vtable 17)
void __fastcall Body_ResetPoints(BYTE* s, void*, DWORD flags)
{
    if (F(int, s, 1432) > 0)
    {
        CallThis(Bp_ActivateAll, s);
        if (flags & 6)
        {
            switch (F(int, s, 1672))
            {
            case 1:
            {
                Bp_LiftBelow(s, Bp_Height(s), F(float, s, 3276));
                Matrix inv, out;
                F(Matrix, s, 1744) = *Matrix_Inverse(&inv, &F(Matrix, s, 1696));
                const Matrix* r = CallThis<const Matrix*>((void (*)())F(void**, s, 0)[18], s, &out, s + 1696, s + 1744,
                                                          s + 1528, s + 1516);
                F(Matrix, s, 1696) = *r;
                break;
            }
            case 3:
            {
                double d = Bp_Height(s);
                float nz = F(float, s, 1536);
                if (x87_fabs((double)nz) > g_bp64b898)
                    Bp_LiftBelow(s, (float)d, F(float, s, 3280));
                else
                {
                    float h = (float)(F(float, s, 3280) / x87_sqrt(g_bp64b670 - (double)nz * nz * g_bp6506c0));
                    Vec3 src, v;
                    src.x = F(float, s, 1528);
                    src.y = F(float, s, 1532);
                    src.z = 0;
                    Vec3_Normalized(&v, &src);
                    Bp_Lift(s, &v.x, (double)h - (float)d + g_bp64b5a0);
                }
                break;
            }
            case 2:
            case 4:
                Bp_LiftBelow(s, Bp_Height(s), F(float, s, 3284));
                break;
            }
        }
    }
    memset(s + 5192, 0, 80);
    F(float, s, 1456) = 1.0f;
    F(int, s, 1692) = 0;
    F(int, s, 1208) = 0;
    memset(s + 1212, 0, 200);
    CallThis(FUN_58e7f0, s, s + 1696, &g_bp67dda8, F(int, s, 1432));
    F(int, s, 1672) = CallThis<int>((void (*)())F(void**, s, 0)[17], s, s + 1696, s + 1456, s + 1460, s + 1464, flags);
}

} // extern "C"

// === a body set on a plane (a method at $L_590810)

extern "C" void Bn_Decompose(); KING_ALIAS("_Bn_Decompose", "_Matrix_Decompose6FArray")
extern "C" void Bn_FromPose(); KING_ALIAS("_Bn_FromPose", "_Matrix_FromPose")
extern "C" void Bn_Lookup(); KING_ALIAS("_Bn_Lookup", "_Obj_Lookup4e5b22")
KING_VAR(Vec3, g_bnAhead, 702620)
KING_VAR(double, g_bn64b7a8, 64b7a8)
KING_VAR(float, g_bn64c300, 64c300)

extern "C" {

// [$L_590810] body b (when +5132) put on the plane through p with normal n: moved along n onto it (from p lifted
// a bit for a fast mode-2 body), turned to lie on it keeping its heading when align, its frame passed on (vtbl+72)
// and placed in the object found there (or its own +800, after vtbl+28) by vtbl+140
void __fastcall Body_OntoPlane(BYTE* b, void*, const Vec3* p, const Vec3* n, int align)
{
    if (!F(int, b, 5132))
        return;
    Vec3 q = *p;
    if (F(int, b, 3568) == 2 && F(float, b, 532) > g_bn64b7a8)
    {
        float ny = (float)(n->y * (double)g_bn64c300), nz = (float)(n->z * (double)g_bn64c300);
        q.x = (float)(n->x * (double)g_bn64c300 + p->x);
        q.y = (float)((double)ny + p->y);
        q.z = (float)((double)nz + p->z);
    }
    const float* c = (const float*)(b + 536);
    float dist = (float)(((double)c[0] - q.x) * n->x + ((double)c[2] - q.z) * n->z +
                         ((double)c[1] - q.y) * n->y);
    float ang[6];
    CallC(Bn_Decompose, b + 500, ang);
    float dy = (float)(dist * (double)n->y), dz = (float)(dist * (double)n->z);
    Vec3 r;
    r.x = (float)((double)c[0] - dist * (double)n->x);
    r.y = (float)((double)c[1] - dy);
    r.z = (float)((double)c[2] - dz);
    Matrix M = F(Matrix, b, 500);
    if (align)
    {
        const float* R = (const float*)(b + 500);
        const Vec3& A = g_bnAhead;
        Vec3 f, g, h0, h, e;
        f.x = (float)((double)A.z * R[6] + (double)A.y * R[3] + (double)A.x * R[0]);
        f.y = (float)((double)A.z * R[7] + (double)A.y * R[4] + (double)A.x * R[1]);
        f.z = (float)((double)A.z * R[8] + (double)A.y * R[5] + (double)A.x * R[2]);
        g.x = (float)((double)f.z * n->y - (double)f.y * n->z);
        g.y = (float)((double)f.x * n->z - (double)f.z * n->x);
        g.z = (float)((double)f.y * n->x - (double)f.x * n->y);
        Vec3_Normalized(&h, Vec3_Cross(&h0, &g, n));
        Vec3_Cross(&e, &h, n);
        float a1 = (float)-x87_asin(h.z);
        double a2 = -x87_asin(e.z);
        Matrix P;
        memcpy(&M, CallC<const Matrix*>(Bn_FromPose, &P, (double)r.x, (double)r.y, (double)r.z, (double)ang[3],
                                         (double)a1, a2), 48);
    }
    M.m[9] = r.x;
    M.m[10] = r.y;
    M.m[11] = r.z;
    Matrix I;
    Matrix_Inverse(&I, &M);
    BYTE out[48];
    CallVirt(b, 72, out, &M, &I, n, &q);
    BYTE* o = CallC<BYTE*>(Bn_Lookup, F(BYTE*, b, 800), b + 536, out + 36, b + 1132);
    if (!o)
    {
        o = F(BYTE*, b, 800);
        CallVirt(b, 28, 1);
    }
    CallVirt(b, 140, o, out);
}

} // extern "C"

// === the sparks' frame (a method at $L_594400)

extern "C" void Sk_Spawn(); KING_ALIAS("_Sk_Spawn", "@Sparks_Spawn@16")
extern "C" void Sk_Put(); KING_ALIAS("_Sk_Put", "@FreeList_Put@16")
extern "C" void Sk_Faders(); KING_ALIAS("_Sk_Faders", "_Faders_Step")
extern "C" void Sk_Post(); KING_ALIAS("_Sk_Post", "@Queue_PostEvent@12")
KING_VAR(Vec3, g_skGravity, 6f6638)
KING_VAR(float, g_sk6508fc, 6508fc)
KING_VAR(float, g_sk6508f8, 6508f8)
KING_VAR(float, g_sk65096c, 65096c)
KING_VAR(float, g_sk64f338, 64f338)
KING_VAR(float, g_sk64c624, 64c624)
KING_VAR(float, g_sk64c258, 64c258)
KING_VAR(float, g_sk64c964, 64c964)
KING_VAR(float, g_sk64c264, 64c264)

extern "C" {

// [$L_594400] sparks s (list +108, game +96): each spark (born +64) past its life (6508fc for a glowing +120,
// 6508f8 else) is freed, sometimes bursting into new ones; the rest fly on (start +40, velocity +16 + +52..., as
// +28 / +4) under gravity 6f6638 and fade (+116); then the faders step
void __fastcall Sparks_Frame(BYTE* s, void*)
{
    BYTE* prev = 0;
    BYTE* p = F(BYTE*, s, 108);
    while (p)
    {
        float age = (float)(F(double, F(BYTE*, s, 96), 2968) - F(float, p, 64));
        bool dead = F(int, p, 120) ? age > g_sk6508fc : age > g_sk6508f8;
        if (dead)
        {
            BYTE* q = p;
            p = F(BYTE*, p, 0);
            if (!F(int, q, 120) && rand() * (double)g_sk65096c > g_sk64f338)
                CallThis(Sk_Spawn, s, q, (int)(rand() * (double)g_sk64c624));
            CallThis(Sk_Put, s, q, prev);
        }
        else
        {
            const Vec3& G = g_skGravity;
            double gx = G.x * (double)age;
            float gy = (float)(G.y * (double)age), gz = (float)(G.z * (double)age);
            double hx = gx * age;
            float hy = (float)(gy * (double)age), hz = (float)(gz * (double)age);
            double ix = hx * g_sk64c258;
            float iy = (float)(hy * (double)g_sk64c258), iz = (float)(hz * (double)g_sk64c258);
            double vx = (double)F(float, p, 16) + F(float, p, 40);
            float vy = (float)((double)F(float, p, 44) + F(float, p, 20));
            float vz = (float)((double)F(float, p, 48) + F(float, p, 24));
            double px = vx * age + F(float, p, 52);
            float wy = (float)(vy * (double)age), wz = (float)(vz * (double)age);
            float py = (float)((double)wy + F(float, p, 56)), pz = (float)((double)wz + F(float, p, 60));
            F(float, p, 28) = (float)(px + ix);
            F(float, p, 32) = (float)((double)iy + py);
            F(float, p, 36) = (float)((double)iz + pz);
            F(float, p, 4) = (float)(G.x * (double)age + F(float, p, 40));
            F(float, p, 8) = (float)(G.y * (double)age + F(float, p, 44));
            F(float, p, 12) = (float)(G.z * (double)age + F(float, p, 48));
            float f = (float)(F(float, p, 116) - F(double, F(BYTE*, s, 96), 2960) * g_sk64c964);
            F(float, p, 116) = f;
            if (f < g_sk64c264)
                F(float, p, 116) = 0;
        }
        // the next (after a freed spark the original skips one)
        if (!p)
            break;
        prev = p;
        p = F(BYTE*, p, 0);
    }
    float now = (float)F(double, F(BYTE*, s, 96), 2968);
    CallC(Sk_Faders, now);
    CallThis(Sk_Post, s, 1);
}

} // extern "C"

// === a point's contact with a moving body's planes (FUN_5917f0 with its tail $L_59199f)

extern "C" void Pc_Fatal(); KING_ALIAS("_Pc_Fatal", "_World_Fatal")
extern "C" void Pc_RotBy(); KING_ALIAS("_Pc_RotBy", "_Vec3_RotateBy")
extern "C" float* __cdecl Vec3_RejectFrom(float* out, float* a, float* b, float* c);

extern "C" {

// [FUN_5917f0..$L_59199f] point p (world) against body b's planes (records of normal +0, point +16) within depth
// r: 0 when it is outside one by more than r; else the deepest plane: the contact (+52 the point on the plane, +64
// the normal, both back in the world, +4 b's frame, +116 b) filled; 1
int __cdecl Body_PointContact(const float* p, BYTE* b, BYTE* contact, float r)
{
    Record28 rec;
    TwoVec_Construct((BYTE*)&rec, 0);
    float M[12];
    CallThis(FUN_57dec0, b, M);
    float q[3];
    q[0] = (float)((double)M[0] * p[0] + (double)M[6] * p[2] + (double)M[3] * p[1] + M[9]);
    q[1] = (float)((double)M[1] * p[0] + (double)M[7] * p[2] + (double)M[4] * p[1] + M[10]);
    q[2] = (float)((double)M[2] * p[0] + (double)M[8] * p[2] + (double)M[5] * p[1] + M[11]);
    float best_pt[3] = {0, 0, 0};
    float best = -F(float, b, 3280);
    int idx = 0;
    const float* n = (const float*)rec.m;
    int k = Records_Next(b, 0, &rec);
    if (k)
    {
        do
        {
            double d = ((double)q[2] - n[6]) * n[2] + ((double)q[1] - n[5]) * n[1] + ((double)q[0] - n[4]) * n[0];
            if (d > r)
                best = 0;
            else if (best < d)
            {
                idx = k - 1;
                best = (float)d;
                memcpy(best_pt, q, 12);
            }
            k = Records_Next(b, 0, &rec);
        } while (k);
        if (*(DWORD*)&best == 0)
            return 0;
    }
    if (!Records_Get(b, 0, &rec, idx))
        CallC(Pc_Fatal, "checkPMBintersection:: mb->get_col_plane(num) == 0");
    float ty = (float)((double)n[1] * r), tz = (float)((double)n[2] * r);
    float S[3];
    S[0] = (float)((double)n[0] * r + n[4]);
    S[1] = (float)((double)ty + n[5]);
    S[2] = (float)((double)tz + n[6]);
    float nn[3] = {n[0], n[1], n[2]};
    float t[3];
    const float* o = Vec3_RejectFrom(t, S, nn, best_pt);
    float* c52 = &F(float, contact, 52);
    float* c64 = &F(float, contact, 64);
    memcpy(c52, o, 12);
    memcpy(c64, nn, 12);
    float W[12];
    CallThis(FUN_57de90, b, W);
    Vec3 w;
    Vec3_SetThis(&w, 0, (float)((double)W[0] * c52[0] + (double)W[6] * c52[2] + (double)W[3] * c52[1] + W[9]),
                 (float)((double)W[1] * c52[0] + (double)W[7] * c52[2] + (double)W[4] * c52[1] + W[10]),
                 (float)((double)W[2] * c52[0] + (double)W[8] * c52[2] + (double)W[5] * c52[1] + W[11]));
    F(Vec3, contact, 52) = w;
    Vec3 rot;
    memcpy(c64, CallC<const float*>(Pc_RotBy, &rot, c64, W), 12);
    F(BYTE*, contact, 116) = b;
    F(int, contact, 100) = 0;
    F(int, contact, 108) = 0;
    memcpy(contact + 4, M, 48);
    return 1;
}

} // extern "C"

// === a moving box's points against another body (FUN_58a020 with its tail $L_58a9fe)

extern "C" void Bx_Get800(); KING_ALIAS("_Bx_Get800", "@SceneObj_Get800@8")
extern "C" void Bx_Query(); KING_ALIAS("_Bx_Query", "_Query_4e52fc")
extern "C" void Bx_Out(); KING_ALIAS("_Bx_Out", "_Vec3_OutOfBox")
extern "C" void Bx_Fatal(); KING_ALIAS("_Bx_Fatal", "_World_Fatal")
KING_VAR(double, g_bx6506b8, 6506b8)
KING_VAR(double, g_bx64b670, 64b670)
KING_VAR(double, g_bx64b5a0, 64b5a0)
KING_VAR(float, g_bx64c250, 64c250)
KING_VAR(float, g_bx64c264, 64c264)
KING_VAR(char, g_bxColFmt, 67dc70)
KING_VAR(char, g_bx1Fmt, 67dbe8)
KING_VAR(char, g_bx2Fmt, 67db24)
KING_VAR(char, g_bxAllFmt, 67db08)

// p through frame m (rows of 3, the place +9) as the asm sums it: m[6] z + m[3] y + m[0] x + m[9] ...
static void Bx_Frame(const float* m, const float* p, float* out)
{
    out[0] = (float)((double)m[6] * p[2] + (double)m[3] * p[1] + (double)m[0] * p[0] + m[9]);
    out[1] = (float)((double)m[7] * p[2] + (double)m[4] * p[1] + (double)m[1] * p[0] + m[10]);
    out[2] = (float)((double)m[8] * p[2] + (double)m[5] * p[1] + (double)m[2] * p[0] + m[11]);
}

// a body's state for the dump: its name, speed, spin, inertia and frame
static void Bx_Dump(char* buf, const char* fmt, BYTE* o, const float* v, const float* w, const float* M,
                    const float* extra)
{
    double i88 = F(float, o, 3288);
    const float* q = &F(float, o, 3296);
    const char* name = F(const char*, F(BYTE*, o, 320), 12);
    if (extra)
        sprintf(buf, fmt, name, (double)v[0], (double)v[1], (double)v[2], (double)w[0], (double)w[1], (double)w[2], i88,
                i88, (double)q[0], (double)q[1], (double)q[2], (double)q[3], (double)q[4], (double)q[5], (double)q[6],
                (double)M[0], (double)M[1], (double)M[2], (double)M[3], (double)M[4], (double)M[5], (double)M[6],
                (double)M[7], (double)M[8], (double)M[9], (double)M[10], (double)M[11], (double)extra[0],
                (double)extra[1], (double)extra[2], (double)extra[3], (double)extra[4], (double)extra[5],
                (double)extra[6], (double)extra[7], (double)extra[8], (double)extra[9], (double)extra[10],
                (double)extra[11]);
    else
        sprintf(buf, fmt, name, (double)v[0], (double)v[1], (double)v[2], (double)w[0], (double)w[1], (double)w[2], i88,
                i88, (double)q[0], (double)q[1], (double)q[2], (double)q[3], (double)q[4], (double)q[5], (double)q[6],
                (double)M[0], (double)M[1], (double)M[2], (double)M[3], (double)M[4], (double)M[5], (double)M[6],
                (double)M[7], (double)M[8], (double)M[9], (double)M[10], (double)M[11]);
}

extern "C" {

// [FUN_58a020..$L_58a9fe] box b (frame m) against body o: each of b's points moving from its place by b's frame
// to its place by m, tried against o's shape in o's own frame (+692 times its frame; kind 2 also its planes),
// recorded in contacts c (at most 63) with sign: the world point and normal (times sign), the ids, b's point
// flags and node for sign > 0, the depth and the fraction along the move; fatal on a contact far out; the count
int __cdecl Box_PointContacts(BYTE* b, const Matrix* m, BYTE* c, BYTE* o, int sign)
{
    __declspec(align(8)) BYTE node[208];
    CallThis(FUN_5ef5c0, node);
    float Wb[12], Mi[12];
    CallThis(FUN_57de90, b, Wb);
    Matrix_Inverse((Matrix*)Mi, m);
    CallThis(Bx_Get800, b);
    float pt[3] = {0, 0, 0};
    int count = 0;
    int r = Points_Next(b, 0, (Vec3Bits*)pt);
    if (!r)
        return 0;
    const float* mm = m->m;
    while (r)
    {
        if (F(int, c, 2816) >= 63)
        {
            while (Points_Next(b, 0, (Vec3Bits*)pt))
                ;
            return count;
        }
        float p[3], q[3];
        Bx_Frame(Wb, pt, p);
        q[0] = (float)((double)pt[2] * mm[6] + (double)pt[1] * mm[3] + (double)pt[0] * mm[0] + mm[9]);
        q[1] = (float)((double)pt[0] * mm[1] + (double)pt[2] * mm[7] + (double)pt[1] * mm[4] + mm[10]);
        q[2] = (float)((double)pt[0] * mm[2] + (double)pt[2] * mm[8] + (double)pt[1] * mm[5] + mm[11]);
        int flag = 0;
        int id = 0;
        if (F(int, o, 392) != 2 || Body_SegmentNearBox(o, 0, p, q))
        {
            float A[12], M1o[12], C[12], Ci[12];
            memcpy(A, o + 692, 48);
            CallThis(FUN_57de90, o, M1o);
            Matrix_Multiply((Matrix*)C, (const Matrix*)A, (const Matrix*)M1o);
            Matrix_Inverse((Matrix*)Ci, (const Matrix*)C);
            float lp[3], lq[3];
            Bx_Frame(Ci, p, lp);
            Bx_Frame(Ci, q, lq);
            int res = CallC<int>(Bx_Query, F(void*, o, 320), lp, lq, node);
            flag = res;
            int hit = res & 1;
            if (hit)
            {
                const float* np = &F(float, node, 52);
                const float* nn = &F(float, node, 64);
                float Wp[3], Wn[3];
                Bx_Frame(C, np, Wp);
                Wn[0] = (float)((double)C[3] * nn[1] + (double)C[6] * nn[2] + (double)C[0] * nn[0]);
                Wn[1] = (float)((double)C[1] * nn[0] + (double)C[4] * nn[1] + (double)C[7] * nn[2]);
                Wn[2] = (float)((double)C[2] * nn[0] + (double)C[5] * nn[1] + (double)C[8] * nn[2]);
                if (CallC<int>(Bx_Out, Wp, 10000.0f) || CallC<int>(Bx_Out, nn, 2.0f))
                {
                    float M1b[12], M1o2[12];
                    CallThis(FUN_57de90, b, M1b);
                    CallThis(FUN_57de90, o, M1o2);
                    char col[1000], d1[1000], d2[2000], all[3000];
                    sprintf(col, &g_bxColFmt, (double)Wp[0], (double)Wp[1], (double)Wp[2], (double)nn[0],
                            (double)nn[1], (double)nn[2], (double)F(float, b, 3268), (double)F(float, o, 3268));
                    Bx_Dump(d1, &g_bx1Fmt, b, &F(float, b, 416), &F(float, b, 428), M1b, 0);
                    Bx_Dump(d2, &g_bx2Fmt, o, &F(float, o, 416), &F(float, o, 428), M1o2, C);
                    sprintf(all, &g_bxAllFmt, col, d1, d2);
                    CallC(Bx_Fatal, all);
                }
                memcpy(node + 52, Wp, 12);
                memcpy(node + 64, Wn, 12);
                id = CallC<int>(FUN_57c9a0, sign > 0 ? o : b);
                F(int, node, 116) = id;
            }
            else if (F(int, o, 392) == 2 && Body_SegmentContact((const Vec3*)p, (const Vec3*)q, (int)o, node,
                                                                  0x3C23D70A))
                flag = 1;
        }
        int n = F(int, c, 2816);
        F(int, c, 2820 + 4 * n) = (flag & 1) * sign;
        F(float, c, 2048 + 4 * n) = 1.0f;
        F(float, c, 2304 + 4 * n) = 1.0f;
        if (F(int, c, 2820 + 4 * n))
        {
            count++;
            F(int, c, 2048 + 4 * n) = 0;
            F(int, c, 2304 + 4 * n) = 0;
            double sg = sign;
            const float* np = &F(float, node, 52);
            const float* nn = &F(float, node, 64);
            memcpy(c + 12 * n, np, 12);
            float sn[3] = {(float)(sg * nn[0]), (float)(sg * nn[1]), (float)(sg * nn[2])};
            memcpy(c + 12 * (n + 64), sn, 12);
            if (sign > 0)
            {
                F(int, c, 1536 + 4 * n) = F(int, node, 116);
                DWORD fl = F(DWORD, b, 5188 + 4 * r) | 2;
                F(DWORD, b, 5188 + 4 * r) = fl;
                F(DWORD, b, 5268 + 4 * r) = fl;
                memcpy(b + 5144 + r * 208, node, 208);
            }
            else
                F(int, c, 1536 + 4 * n) = CallC<int>(FUN_57c9a0, b);
            F(int, c, 3076 + 4 * n) = r - 1;
            const float* e = &F(float, node, 88);
            double ax = (double)e[0] - p[0], ay = (double)e[1] - p[1], az = (double)e[2] - p[2];
            float h[3] = {(float)((double)np[0] - p[0]), (float)((double)np[1] - p[1]), (float)((double)np[2] - p[2])};
            double gx = (double)np[0] - q[0], gy = (double)np[1] - q[1];
            float gz = (float)((double)np[2] - q[2]);
            double dot = (double)gz * nn[2] + gy * nn[1];
            dot = gx * nn[0] + dot;
            F(float, c, 2560 + 4 * n) = (float)dot;
            float la = (float)(ax * ax + ay * ay + az * az);
            double lh = (double)h[0] * h[0] + (double)h[1] * h[1] + (double)h[2] * h[2];
            if (!(la < g_bx6506b8))
            {
                double rr = lh / la;
                if (rr > g_bx64b670)
                    rr = g_bx64c250;
                F(float, c, 2048 + 4 * n) = (float)x87_sqrt(rr);
                F(float, c, 2304 + 4 * n) = F(float, c, 2048 + 4 * n);
                double t = x87_sqrt(lh) - g_bx64b5a0;
                if (t > g_bx64c264)
                    F(float, c, 2048 + 4 * n) = (float)(t / x87_sqrt(la));
                else
                    F(int, c, 2048 + 4 * n) = 0;
            }
            F(int, c, 2816)++;
        }
        r = Points_Next(b, 0, (Vec3Bits*)pt);
    }
    return count;
}

} // extern "C"

// === a body's switch between lying and standing (FUN_590520 with its tails $L_590786, $L_590791)

extern "C" void Ls_Mode(); KING_ALIAS("_Ls_Mode", "@Body_SetContactMode@12")
extern "C" void Ls_Reach(); KING_ALIAS("_Ls_Reach", "@Body_AttachedInReach@8")
KING_VAR(float, g_ls6969a0, 6969a0)
KING_VAR(float, g_ls64f768, 64f768)
KING_VAR(float, g_ls64f730, 64f730)
KING_VAR(float, g_ls650818, 650818)
KING_VAR(float, g_ls64da74, 64da74)
KING_VAR(char, g_lsBad, 67df30)

// p (+536) through the frame at 6969a0 and its squared length (z unrounded)
static double Ls_Place(BYTE* s, float* v)
{
    const float* M = &g_ls6969a0;
    double a0 = F(float, s, 536), a1 = F(float, s, 540), a2 = F(float, s, 544);
    v[0] = (float)(M[0] * a0 + M[6] * a2 + M[3] * a1 + M[9]);
    v[1] = (float)(M[1] * a0 + M[7] * a2 + M[4] * a1 + M[10]);
    double z = M[2] * a0 + M[8] * a2 + M[5] * a1 + M[11];
    v[2] = (float)z;
    return (double)v[0] * v[0] + (double)v[1] * v[1] + z * z;
}

extern "C" {

// [FUN_590520..$L_590791] body s (only with flag 1 at +1196) in state +3976 (0, 1 free, 2 attached, 3 lying):
// its place near the frame at 6969a0 asks for lying (+3980 or, attached, at once); a pending 2 / 3 taken once
// its contacts (3) or its attachment (2) allow; fatal on a bad request
void __fastcall Body_UpdateLying(BYTE* s)
{
    if (F(BYTE, s, 1196) & 1)
    {
        int st = F(int, s, 3976);
        float v[3];
        if (st == 0 || st == 1)
        {
            double r2 = Ls_Place(s, v);
            if (r2 < g_ls650818 ||
                (r2 < g_ls64da74 && (x87_fabs(v[0]) < v[1] || x87_fabs(v[2]) < v[1])))
                F(int, s, 3980) = 3;
        }
        else if (st == 2)
        {
            double r2 = Ls_Place(s, v);
            if (r2 < g_ls64f768 ||
                (r2 < g_ls64f730 && (x87_fabs(v[0]) < v[1] || x87_fabs(v[2]) < v[1])))
                F(int, s, 3976) = 3;
        }
    }
    int e = F(int, s, 3980);
    if (e != 3 && e != 2)
    {
        if (e != 0 && e != 1)
            World_Fatal(&g_lsBad);
        return;
    }
    int c = F(int, s, 3976);
    if (e == c)
        return;
    bool ok;
    if (e == 3)
    {
        F(int, s, 3588) = 1;
        CallThis(Ls_Mode, s, 0);
        ok = !F(int, s, 3588);
    }
    else
        ok = !CallThis<int>(Ls_Reach, s);
    if (!ok)
        return;
    e = F(int, s, 3980);
    if (e == 2)
        F(int, s, 3976) = 2;
    if (e == 3)
        F(int, s, 3976) = 3;
    F(int, s, 3980) = 0;
}

} // extern "C"

// === a moving body settling on the ground ($L_58fdf0 with its tail $L_590476)

extern "C" void __cdecl St_Decompose(const BYTE* m, float* a, float* b, float* c, float* d, float* e, float* f); KING_ALIAS("_St_Decompose", "_Matrix_Decompose6F")
extern "C" BYTE* __cdecl St_FromPose(BYTE* out, double x, double y, double z, double a, double b, double c); KING_ALIAS("_St_FromPose", "_Matrix_FromPose")
extern "C" void __fastcall St_Probe(BYTE* s, void*, int a); KING_ALIAS("@St_Probe@12", "@Body_ProbeGround@12")
extern "C" BYTE* __fastcall St_NodeInit(BYTE* self, void*); KING_ALIAS("@St_NodeInit@8", "@Node_Init@8")
extern "C" int __cdecl St_Query(BYTE* skip, void* obj, const void* p, const void* d, BYTE* q); KING_ALIAS("_St_Query", "_Query_4e57e8")
extern "C" int __fastcall St_PointsNext(BYTE* self, void*, void* out); KING_ALIAS("@St_PointsNext@12", "@Points_Next@12")
extern "C" BYTE* __cdecl St_MovingOf(BYTE* o); KING_ALIAS("_St_MovingOf", "_Moving_Of")
extern "C" double __cdecl St_FrameTime(); KING_ALIAS("_St_FrameTime", "_Game_FrameTime")
extern "C" void St_MulA(); KING_ALIAS("_St_MulA", "@Vec3_MulAssign@16")
KING_VAR(double, g_st64d318, 64d318)
KING_VAR(double, g_st64b958, 64b958)
KING_VAR(double, g_st650810, 650810)
KING_VAR(double, g_st64b6e0, 64b6e0)
KING_VAR(double, g_st64d338, 64d338)
KING_VAR(float, g_st64c250, 64c250)
KING_VAR(float, g_st64d2f8, 64d2f8)
KING_VAR(float, g_st702630, 702630)
KING_VAR(float, g_st702634, 702634)
KING_VAR(float, g_st702638, 702638)
KING_VAR(char, g_st67df24, 67df24)
KING_VAR(int, g_st6f6568, 6f6568)

#define V(n) (*(float*)(fr + (n)))
#define I(n) (*(int*)(fr + (n)))
#define SF(n) F(float, s, n)

extern "C" {

// [$L_58fdf0..$L_590476] (moving body vtable) body s settling: its lowest contact point against the ground
// normal (+976), then either upright on its pose (+3976 == 2), moved on (+1196 bit 8 clear), put to rest when slow
// (a ray down from its bounding radius, the rest pose by vtable +224), or its speed damped (0.97 ^ 2dt) while
// fewer than three points touch the ground; finally its state flags (+892 .. +900)
void __fastcall MovingBody_Settle(BYTE* s, void*, int)
{
    BYTE fr[356];
    V(96) = (float)((SF(424) * SF(424) + SF(420) * SF(420)) + SF(416) * SF(416));
    V(40) = (float)((SF(436) * SF(436) + SF(432) * SF(432)) + SF(428) * SF(428));
    int ground = F(int, s, 892);
    V(16) = 1.0f;
    if (ground && !(SF(984) <= g_st64d318) && F(int, s, 5132))
    {
        int n = F(int, s, 5132);
        for (int k = 0; k < n; k++)
        {
            const BYTE* p = s + 3988 + 28 * k;
            double a = (SF(500) * F(float, p, 0) + SF(524) * F(float, p, 8)) + SF(512) * F(float, p, 4);
            double b = (SF(504) * F(float, p, 0) + SF(528) * F(float, p, 8)) + SF(516) * F(float, p, 4);
            double c = (F(float, p, 4) * SF(520) + SF(508) * F(float, p, 0)) + SF(532) * F(float, p, 8);
            double v = (c * SF(984) + a * SF(976)) + b * SF(980);
            if (v < V(16))
                V(16) = (float)v;
        }
    }
    else
        V(16) = -1.0f;
    DWORD fl = F(DWORD, s, 1196);
    if (!(fl & 0x100) && F(int, s, 3976) == 2 && ground)
    {
        St_Decompose(s + 500, &V(72), &V(76), &V(80), &V(84), &V(88), &V(92));
        BYTE* m = St_FromPose(fr + 320, V(72), V(76), V(80), V(84), 0.0, 0.0);
        memcpy(s + 500, m, 48);
        St_Probe(s, 0, 0);
    }
    else if (!(fl & 8))
        CallVirt(s, 140, F(BYTE*, s, 800), s + 500);
    else if (ground && V(96) <= g_st64b958 && V(40) <= g_st64b5a0 && V(16) <= g_st650810)
    {
        CallVirt(s, 140, F(BYTE*, s, 804), s + 548);
        if (F(BYTE, s, 1196) & 2)
        {
            int n = F(int, s, 5132);
            if (n)
            {
                if (F(int, s, 892) && !(SF(984) <= g_st64d318) && !F(int, s, 1028))
                    St_Probe(s, 0, 0);
                else
                {
                    V(16) = 0.0f;
                    for (int k = 0; k < n && k < 2; k++)
                    {
                        const BYTE* p = s + 4004 + 28 * k;
                        double q = (F(float, p, 4) * F(float, p, 4) + F(float, p, 8) * F(float, p, 8))
                                   + F(float, p, 0) * F(float, p, 0);
                        if (!(q <= V(16)))
                            V(16) = (float)q;
                    }
                    St_NodeInit(fr + 112, 0);
                    double r = x87_sqrt(V(16));
                    double d = r + r;
                    V(60) = (float)(d * g_st702630);
                    V(64) = (float)(d * g_st702634);
                    double dz = d * g_st702638;
                    V(24) = V(60) + SF(536);
                    I(72) = I(24);
                    V(28) = V(64) + SF(540);
                    I(76) = I(28);
                    V(32) = (float)(dz + SF(544));
                    I(80) = I(32);
                    double rk = r * g_st64d2f8;
                    V(60) = (float)(rk * g_st702630);
                    V(64) = (float)(rk * g_st702634);
                    double rz = rk * g_st702638;
                    V(40) = V(24) - V(60);
                    I(100) = I(40);
                    V(44) = V(28) - V(64);
                    I(104) = I(44);
                    V(48) = (float)(V(32) - rz);
                    I(108) = I(48);
                    if (St_Query(s, F(BYTE*, s, 800), fr + 72, fr + 100, fr + 112) && !I(228)
                        && !(SF(984) <= g_st64d318))
                        CallVirt(s, 224, fr + 164, fr + 176, 0);
                }
            }
            if (!F(int, s, 892) || !St_MovingOf(F(BYTE*, s, 1028)))
                F(int, s, 1192) = 1;
        }
    }
    else
    {
        // the three ground points nearest under the body
        I(24) = 0;
        I(40) = F(int, s, 400);
        I(28) = 0;
        I(32) = 0;
        I(36) = F(int, s, 400);
        I(16) = F(int, s, 400);
        for (int idx = St_PointsNext(s, 0, fr + 24); idx; idx = St_PointsNext(s, 0, fr + 24))
        {
            const BYTE* e = s + 208 * idx;
            double wx = ((V(32) * SF(524) + V(28) * SF(512)) + V(24) * SF(500)) + SF(536);
            V(76) = (float)(((V(24) * SF(504) + V(32) * SF(528)) + V(28) * SF(516)) + SF(540));
            V(80) = (float)(((V(32) * SF(532) + V(28) * SF(520)) + V(24) * SF(508)) + SF(544));
            double dx = wx - F(float, e, 5196);
            V(64) = V(76) - F(float, e, 5200);
            V(68) = V(80) - F(float, e, 5204);
            V(20) = (float)x87_fabs((F(float, e, 5216) * V(68) + F(float, e, 5212) * V(64)) + F(float, e, 5208) * dx);
            if (F(int, e, 5260))
                continue;
            double q = (dx * dx + V(64) * V(64)) + V(68) * V(68);
            if (!(q < SF(400)))
                continue;
            if (SF(400) * g_st64b6e0 <= V(20))
                continue;
            if (V(20) < V(40))
            {
                I(16) = I(36);
                I(36) = I(40);
                I(40) = I(20);
            }
            else if (V(20) < V(36))
            {
                I(16) = I(36);
                I(36) = I(20);
            }
            else if (V(20) < V(16))
                I(16) = I(20);
        }
        if (!(SF(400) * g_st64d338 <= V(16)))
        {
            DWORD bits[2] = { 1889785610u, 1072630333u };    // 0.97
            double t = St_FrameTime();
            V(40) = (float)x87_pow(*(double*)bits, t + t);
            if (V(96) < g_st64c250)
            {
                CallThis(St_MulA, s + 416, fr + 72, V(40));
                CallVirt(s, 144, s + 416, &g_st67df24);
            }
        }
        CallVirt(s, 140, F(BYTE*, s, 800), s + 500);
    }
    if (g_st6f6568)
        F(DWORD, s, 892) |= 32;
    F(int, s, 896) = (F(int, s, 896) || F(int, s, 892)) ? 1 : 0;
    F(int, s, 900) = F(int, s, 892);
}

} // extern "C"

#undef V
#undef I
#undef SF
