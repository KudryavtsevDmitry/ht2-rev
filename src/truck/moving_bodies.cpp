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
ASM_PROC(FUN_592350)
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
