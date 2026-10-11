// moving_bodies.cpp - movingb.cpp of the game, rewritten from king.masm: small
// functions of its moving bodies: progress counters, planes, the point and
// record lists of a body, flags, the ListItem lists, and a free list.
//
// Building with  nmake ASM_MOVING_BODIES=1  makes king.masm assemble the original
// procedures again.

#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

ASM_PROC(FUN_589640)
ASM_PROC(FUN_592cc0)    // fatal error with a message

ASM_VAR(AsmLabel, g_vtbl65089c, $L_65089c)
ASM_VAR(AsmLabel, g_strPutOnHead, $L_67e088)    // "ListItem::put_on_head():this==NULL||next!=NULL||prev!=NULL"
ASM_VAR(AsmLabel, g_strPutOnTail, $L_67e0c4)    // "ListItem::put_on_tail():this==NULL||next!=NULL||prev!=NULL"

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
ASM_PROC(FUN_5f1a70)
ASM_VAR(char, g_vtbl6508a8, $L_6508a8)

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

ASM_PROC(FUN_591c30)
extern "C" void FUN_592350(); KING_ALIAS("_FUN_592350", "_Bodies_Touch")
ASM_PROC(FUN_5924d0)

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

ASM_PROC(FUN_57cb70)
ASM_PROC(FUN_57d060)
extern "C" void FUN_58b4f0(); KING_ALIAS("_FUN_58b4f0", "@TwoVec_Construct@8")
extern "C" void FUN_58b8c0(); KING_ALIAS("_FUN_58b8c0", "@Body_Reset@16")
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
ASM_VAR(char, g_vtbl650718, $L_650718)
ASM_VAR(char, g_vtbl6506cc, $L_6506cc)
ASM_VAR(char, g_vtbl6506c8, $L_6506c8)

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
ASM_PROC(FUN_587c80)

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


ASM_VAR(Vec3, g_gravity702650, $L_702650)
ASM_VAR(Vec3, g_axis702630, $L_702630)
ASM_PROC(FUN_58ba90)

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
ASM_VAR(Vec3, g_down702630, $L_702630)

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
