// ai_containers.cpp - the STL template instances the AI's code generated (the unit
// at 0x458bc0 of king.masm, after Ai_main.cpp): insertion sorts, medians and
// partitions for its sorts, list and vector members, finds, copies.
//
// Building with  nmake ASM_AI_CONTAINERS=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "road.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" void __cdecl List_FreeNode(void* node, int size);   // aimain.cpp
extern "C" int __cdecl _stricmp(const char* a, const char* b);
extern "C" void __cdecl Sort_LinearInsert12(BYTE* first, BYTE* last, int, void* cmp);
KING_ALIAS("_Sort_LinearInsert12", "FUN_467820")
extern "C" void __cdecl Sort_UnguardedInsertion12(BYTE* first, BYTE* last, int, void* cmp);
KING_ALIAS("_Sort_UnguardedInsertion12", "FUN_4679d0")
ASM_PROC(FUN_45b570)    // vector<1592 bytes>::insert_aux
extern "C" void FUN_45cb00(); KING_ALIAS("_FUN_45cb00", "@Vec4b_InsertC@16")    // vector<4 bytes>::insert_aux
ASM_PROC(FUN_45d8f0)
extern "C" void FUN_4604b0(); KING_ALIAS("_FUN_4604b0", "@Vec4b_InsertG@16")
extern "C" void FUN_405870(); KING_ALIAS("_FUN_405870", "@Rec405870_Destroy@8")    // a list value's destructor
extern "C" void FUN_602ca0(); KING_ALIAS("_FUN_602ca0", "@MsgBuf_FreeStorage@8")    // a 16-byte record's destructor
extern "C" void FUN_41ce50(); KING_ALIAS("_FUN_41ce50", "@Rec41ce50_DestroyVector@8")

struct Vec {
    BYTE* begin;
    BYTE* end;
    BYTE* capacityEnd;
};

static void PushBack(Vec* v, const void* x, int n, void (*proc)())
{
    if (v->end != v->capacityEnd) {
        if (v->end)
            memcpy(v->end, x, n);
        v->end += n;
    } else
        CallThis(proc, v, v->end, x);
}

// ---------------------------------------------------------------------------
// Lists

extern "C" BYTE** __stdcall List64_Erase(BYTE** out, BYTE** node)
{
    BYTE** prev = (BYTE**)node[1];
    BYTE** next = (BYTE**)node[0];
    prev[0] = (BYTE*)next;
    next[1] = (BYTE*)prev;
    CallThis(FUN_405870, node + 2);
    List_FreeNode(node, 64);
    *out = (BYTE*)next;
    return out;
}

extern "C" BYTE** __cdecl List_FindKey8(BYTE** out, BYTE** first, BYTE** last, const int* key)
{
    BYTE** n = first;
    if (n != last)
        for (int k = *key; F(int, n, 8) != k && (n = (BYTE**)*n) != last; )
            ;
    *out = (BYTE*)n;
    return out;
}

extern "C" BYTE** __cdecl List_FindKey24(BYTE** out, BYTE** first, BYTE** last, int key)
{
    BYTE** n = first;
    for (; n != last; n = (BYTE**)*n)
        if (F(int, F(BYTE*, n, 8), 24) == key)
            break;
    *out = (BYTE*)n;
    return out;
}

extern "C" BYTE** __cdecl List_FindNear(BYTE** out, BYTE** first, BYTE** last, const DVec3* p)
{
    BYTE** n = first;
    for (; n != last; n = (BYTE**)*n) {
        double dx = p->x - F(double, n, 16);
        double dy = p->y - F(double, n, 24);
        double dz = p->z - F(double, n, 32);
        if (!(x87_sqrt((dy * dy + dz * dz) + dx * dx) >= 0.1))
            break;
    }
    *out = (BYTE*)n;
    return out;
}

extern "C" BYTE** __cdecl List_FindName(BYTE** out, BYTE** first, BYTE** last, const char* name)
{
    BYTE** n = first;
    for (; n != last; n = (BYTE**)*n) {
        const CharVec* v = (const CharVec*)(n + 2);
        if (v->begin != v->end && v->begin && strcmp(v->begin, name) == 0)
            break;
    }
    *out = (BYTE*)n;
    return out;
}

// ---------------------------------------------------------------------------
// Buffers of the AI's strings: length +0, data +16, buffer +20

extern "C" unsigned __cdecl Str_Rounded(unsigned n) { return (n + 8) & ~7u; }

extern "C" void __cdecl Str_FreeRounded(void* p, unsigned n)
{
    unsigned m = (n + 8) & ~7u;
    if (m)
        List_FreeNode(p, m);
}

// ---------------------------------------------------------------------------
// Copies and finds

extern "C" BYTE* __cdecl Copy12s(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = (last - first) / 12; n > 0; n--, first += 12, dest += 12)
        memcpy(dest, first, 12);
    return dest;
}

extern "C" DWORD* __cdecl UniqueCopyDwords(const DWORD* first, const DWORD* last, DWORD* dest)
{
    if (first == last)
        return dest;
    *dest = *first;
    for (first++; first != last; first++)
        if (*dest != *first)
            *++dest = *first;
    return dest + 1;
}

extern "C" DWORD* __cdecl CopyBackwardDwords(const DWORD* first, const DWORD* last, DWORD* destEnd)
{
    for (int n = last - first; n > 0; n--)
        *--destEnd = *--last;
    return destEnd;
}

extern "C" DWORD* __cdecl CopyBackwardDwords2(const DWORD* first, const DWORD* last, DWORD* destEnd)
{
    return CopyBackwardDwords(first, last, destEnd);
}

extern "C" BYTE* __cdecl CopyBackward32s(const BYTE* first, const BYTE* last, BYTE* destEnd)
{
    for (int n = (last - first) >> 5; n > 0; n--) {
        last -= 32;
        destEnd -= 32;
        memcpy(destEnd, last, 32);
    }
    return destEnd;
}

extern "C" BYTE* __cdecl Find36Flag4(BYTE* first, BYTE* last)
{
    for (; first != last; first += 36)
        if ((F(DWORD, first, 4) >> 2) & 1)
            break;
    return first;
}

extern "C" DVec3* __cdecl DVec3_Sub(DVec3* out, const DVec3* a, const DVec3* b)
{
    double x = a->x - b->x, y = a->y - b->y, z = a->z - b->z;
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

extern "C" const int* __cdecl MinPtr(const int* a, const int* b) { return *b < *a ? b : a; }

extern "C" void __cdecl Destroy16s(BYTE* first, BYTE* last)
{
    for (; first != last; first += 16)
        CallThis(FUN_602ca0, first);
}

// placement copies
extern "C" void __cdecl Construct8(DWORD* d, const DWORD* s) { if (d) { d[0] = s[0]; d[1] = s[1]; } }
extern "C" void __cdecl Construct4(DWORD* d, const DWORD* s) { if (d) d[0] = s[0]; }
extern "C" void __cdecl Construct24(DWORD* d, const DWORD* s) { if (d) memcpy(d, s, 24); }
extern "C" void __cdecl Construct32(DWORD* d, const DWORD* s) { if (d) memcpy(d, s, 32); }
extern "C" void __cdecl Construct32Skip1(DWORD* d, const DWORD* s)
{
    if (!d)
        return;
    d[0] = s[0];
    for (int i = 2; i < 8; i++)
        d[i] = s[i];
}

// a string handle (see aimain.cpp): copied with its reference count
extern "C" void __cdecl ConstructStrRef(DWORD* d, const DWORD* s)
{
    if (!d)
        return;
    if (s[2] & 0x10000) {
        d[2] = s[2];
        d[0] = s[0];
        d[1] = s[1];
        return;
    }
    d[1] = 0;
    if (s[0]) {
        d[0] = s[0];
        d[2] = s[2];
        F(int, d[0], 4)++;
    } else {
        d[0] = 0;
        d[2] = 0;
    }
}

// ---------------------------------------------------------------------------
// Sorting

// sort of 12-byte elements: insertion sort, unguarded beyond the first 16
extern "C" void __cdecl Sort_FinalInsertion12(BYTE* first, BYTE* last, void* cmp)
{
    if ((last - first) / 12 > 16) {
        BYTE* mid = first + 192;
        if (first != mid)
            for (BYTE* i = first + 12; i != mid; i += 12)
                Sort_LinearInsert12(first, i, 0, cmp);
        Sort_UnguardedInsertion12(mid, last, 0, cmp);
        return;
    }
    if (first == last)
        return;
    for (BYTE* i = first + 12; i != last; i += 12)
        Sort_LinearInsert12(first, i, 0, cmp);
}

extern "C" void __cdecl Sort_InsertionDwords(unsigned* first, unsigned* last)
{
    if (first == last)
        return;
    for (unsigned* i = first + 1; i != last; i++) {
        unsigned v = *i;
        if (v < *first) {
            for (unsigned* p = i; p != first; p--)
                *p = p[-1];
            *first = v;
        } else {
            unsigned* p = i;
            while (v < p[-1]) {
                *p = p[-1];
                p--;
            }
            *p = v;
        }
    }
}

template <int OFF> static int Key(void* p) { return F(int, p, OFF); }

template <int OFF> static void InsertionByField(void** first, void** last)
{
    if (first == last)
        return;
    for (void** i = first + 1; i != last; i++) {
        void* v = *i;
        if (Key<OFF>(v) < Key<OFF>(*first)) {
            for (void** p = i; p != first; p--)
                *p = p[-1];
            *first = v;
        } else {
            void** p = i;
            while (Key<OFF>(v) < Key<OFF>(p[-1])) {
                *p = p[-1];
                p--;
            }
            *p = v;
        }
    }
}

template <int OFF> static void** MedianByField(void** a, void** b, void** c)
{
    int x = Key<OFF>(*a), y = Key<OFF>(*b), z = Key<OFF>(*c);
    if (x < y) {
        if (y < z)
            return b;
        if (x >= z)
            return a;
        return c;
    }
    if (x < z)
        return a;
    if (y < z)
        return c;
    return b;
}

extern "C" void __cdecl Sort_InsertionBy24(void** first, void** last) { InsertionByField<24>(first, last); }
extern "C" void __cdecl Sort_InsertionBy328(void** first, void** last) { InsertionByField<328>(first, last); }
extern "C" void** __cdecl Sort_MedianBy24(void** a, void** b, void** c) { return MedianByField<24>(a, b, c); }
extern "C" void** __cdecl Sort_MedianBy328(void** a, void** b, void** c) { return MedianByField<328>(a, b, c); }

extern "C" void** __cdecl Sort_PartitionBy24(void** first, void** last, void* pivot)
{
    int k = Key<24>(pivot);
    for (;;) {
        while (Key<24>(*first) < k)
            first++;
        last--;
        while (k < Key<24>(*last))
            last--;
        if (!(first < last))
            return first;
        void* t = *first;
        *first = *last;
        *last = t;
        first++;
    }
}

// the median of three names, compared without case
extern "C" const char** __cdecl Sort_MedianByName(const char** a, const char** b, const char** c)
{
    if (_stricmp(*a, *b) < 0) {
        if (_stricmp(*b, *c) < 0)
            return b;
        if (_stricmp(*a, *c) >= 0)
            return a;
        return c;
    }
    if (_stricmp(*a, *c) < 0)
        return a;
    if (_stricmp(*b, *c) < 0)
        return c;
    return b;
}

// ---------------------------------------------------------------------------
// thiscall members

extern "C" {
void __fastcall Vec1592_PushBack(Vec* s, void*, const void* x) { PushBack(s, x, 1592, FUN_45b570); }   // [FUN_459110]
BYTE* __fastcall Vec_EndMinus4(Vec* s, void*) { return s->end - 4; }                                    // [FUN_459bd0]
void __fastcall Vec4_PushBackH(Vec* s, void*, const void* x) { PushBack(s, x, 4, FUN_45cb00); }        // [FUN_459be0]
void __fastcall Vec4_PushBackI(Vec* s, void*, const void* x) { PushBack(s, x, 4, FUN_45d8f0); }        // [FUN_45a000]
void __fastcall Vec4_PushBackJ(Vec* s, void*, const void* x) { PushBack(s, x, 4, FUN_4604b0); }        // [FUN_45a030]
void __fastcall Str_FreeBoth(BYTE* s, void*)                                                            // [FUN_459c10]
{
    void* data = F(void*, s, 16);
    if (F(void*, s, 20) != data && data && F(int, s, 0) + 1 != 0)
        List_FreeNode(data, F(int, s, 0) + 1);
    unsigned n = (F(unsigned, s, 0) + 8) & ~7u;
    if (n)
        List_FreeNode(F(void*, s, 20), n);
}
void __fastcall Str_FreeData(BYTE* s, void*)                                                            // [FUN_45a990]
{
    void* data = F(void*, s, 16);
    if (data && F(int, s, 0) + 1 != 0)
        List_FreeNode(data, F(int, s, 0) + 1);
}
int __fastcall Position_Equals(const PositionId* s, void*, const PositionId* o)                         // [FUN_45eb50]
{
    return s->road == o->road && s->node == o->node && x87_eq(s->t, o->t) && s->segment == o->segment
        && s->object == o->object && s->m_24 == o->m_24;
}
// the second node is farther from the point than the first
int __fastcall Point_Farther(const DVec3* s, void*, BYTE* const* a, BYTE* const* b)                    // [FUN_460110]
{
    const DVec3* pa = (const DVec3*)(*a + 56);
    const DVec3* pb = (const DVec3*)(*b + 56);
    double ax = pa->x - s->x, ay = pa->y - s->y, az = pa->z - s->z;
    double bx = pb->x - s->x, by = pb->y - s->y, bz = pb->z - s->z;
    double la = x87_sqrt((ay * ay + az * az) + ax * ax);
    double lb = x87_sqrt((by * by + bz * bz) + bx * bx);
    return lb > la;
}
BYTE* __fastcall Rec1588_Copy(BYTE* s, void*, const BYTE* src)                                         // [FUN_461a50]
{
    memcpy(s, src, 580);
    F(double, s, 584) = F(double, src, 584);
    F(double, s, 592) = F(double, src, 592);
    F(double, s, 600) = F(double, src, 600);
    F(DWORD, s, 608) = F(DWORD, src, 608);
    F(DWORD, s, 612) = F(DWORD, src, 612);
    memcpy(s + 616, src + 616, 580);
    memcpy(s + 1196, src + 1196, 196);
    memcpy(s + 1392, src + 1392, 196);
    return s;
}
BYTE* __fastcall Rec16_DeletingDestructor(BYTE* s, void*, int flags)                                   // [FUN_461ae0]
{
    CallThis(FUN_602ca0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}
BYTE* __fastcall Rec41ce50_DeletingDestructor(BYTE* s, void*, int flags)                               // [FUN_461b00]
{
    CallThis(FUN_41ce50, s);
    if (flags & 1)
        operator delete(s);
    return s;
}
DWORD* __fastcall Rec60_Copy(DWORD* s, void*, const DWORD* src)                                        // [FUN_461b20]
{
    if (src[4]) {
        memcpy(s, src, 60);
        return s;
    }
    s[0] = src[0];
    s[1] = src[1];
    s[4] = 0;
    return s;
}
}

// ---------------------------------------------------------------------------
// Batch 2

extern "C" double __cdecl Players_NearestTo(const int* const* node, const BYTE** list);   // aimain.cpp
extern "C" void __cdecl Tree_Erase469520(void* node, void* x);
KING_ALIAS("_Tree_Erase469520", "FUN_469520")
extern "C" void FUN_40cc50(); KING_ALIAS("_FUN_40cc50", "@Rec40cc50_Destroy@8")    // a 24-byte record's destructor
extern "C" void* __cdecl Stl_Allocate(unsigned bytes);

struct Elem32 { DWORD w[2]; double key; DWORD w4[4]; };

static double NearestTo(void* const* it, const BYTE** list) { return Players_NearestTo((const int* const*)it, list); }

// the comparison some sorts use: the difference of two doubles is not zero
static bool Differs(double a, double b) { return !x87_eq(a - b, 0.0); }
static double At504(void* p) { return F(double, p, 504); }

extern "C" Elem32* __cdecl Sort_MedianByKey(Elem32* a, Elem32* b, Elem32* c)
{
    if (a->key > b->key) {
        if (b->key > c->key)
            return b;
        if (!(a->key > c->key))
            return a;
        return c;
    }
    if (a->key > c->key)
        return a;
    if (b->key > c->key)
        return c;
    return b;
}

// partition of 32-byte elements, descending by the double at +8
extern "C" Elem32* __cdecl Sort_PartitionByKey(Elem32* first, Elem32* last, Elem32 pivot)
{
    double k = pivot.key;
    for (;;) {
        while (!(k >= first->key))
            first++;
        do
            last--;
        while (k > last->key);
        if (!(first < last))
            return first;
        Elem32 t = *first;
        *first = *last;
        *last = t;
        first++;
    }
}

extern "C" void** __cdecl Sort_MedianBy504(void** a, void** b, void** c)
{
    double x = At504(*a), y = At504(*b), z = At504(*c);
    if (Differs(x, y)) {
        if (Differs(y, z))
            return b;
        if (!Differs(x, z))
            return a;
        return c;
    }
    if (Differs(x, z))
        return a;
    if (Differs(y, z))
        return c;
    return b;
}

extern "C" void** __cdecl Sort_PartitionBy504(void** first, void** last, void* pivot)
{
    for (;;) {
        while (Differs(At504(*first), At504(pivot)))
            first++;
        do
            last--;
        while (Differs(At504(pivot), At504(*last)));
        if (!(first < last))
            return first;
        void* t = *first;
        *first = *last;
        *last = t;
        first++;
    }
}

template <int N> static BYTE* FillN(BYTE* d, unsigned n, const void* v)
{
    for (; n; n--, d += N)
        if (d)
            memcpy(d, v, N);
    return d;
}

extern "C" BYTE* __cdecl UninitFill24(BYTE* d, unsigned n, const void* v) { return FillN<24>(d, n, v); }
extern "C" BYTE* __cdecl UninitFill16(BYTE* d, unsigned n, const void* v) { return FillN<16>(d, n, v); }
extern "C" BYTE* __cdecl UninitFill4(BYTE* d, unsigned n, const void* v) { return FillN<4>(d, n, v); }

extern "C" BYTE* __cdecl UninitFill4s(BYTE* d, int n, const DWORD* v)
{
    for (; n > 0; n--, d += 4)
        if (d)
            *(DWORD*)d = *v;
    return d;
}

extern "C" DWORD* __cdecl FillDwords(DWORD* d, unsigned n, const DWORD* v)
{
    for (; n; n--)
        *d++ = *v;
    return d;
}

extern "C" char* __cdecl FillBytes(char* d, unsigned n, const char* v)
{
    for (; n; n--)
        *d++ = *v;
    return d;
}

extern "C" void __stdcall Stl_Free4s(void* p, unsigned n) { if (n) List_FreeNode(p, n * 4); }
extern "C" void __stdcall Stl_FreeBytes(void* p, unsigned n) { if (n) List_FreeNode(p, n); }

extern "C" BYTE** __cdecl CopyN(BYTE** out, BYTE* first, int n, BYTE* dest)
{
    memmove(dest, first, n);
    out[0] = first + n;
    out[1] = dest + n;
    return out;
}

// erases a subtree of the STL's red-black tree (+12: is nil, +20 left, +24 right)
extern "C" void __cdecl Tree_EraseRight(BYTE* node, void* x)
{
    while (!F(BYTE, node, 12)) {
        Tree_EraseRight(F(BYTE*, node, 20), x);
        node = F(BYTE*, node, 24);
    }
    Tree_Erase469520(node, x);
}

// lexicographical compare of signed chars: -1, 0 or 1
extern "C" int __cdecl LexCompare(const signed char* a, const signed char* aEnd, const signed char* b, const signed char* bEnd)
{
    for (; a != aEnd; a++, b++) {
        if (b == bEnd)
            return a != aEnd;
        if (*b > *a)
            return -1;
        if (*b < *a)
            return 1;
    }
    if (b != bEnd)
        return -1;
    return a != aEnd;
}

extern "C" void __cdecl Destroy24s(BYTE* first, BYTE* last)
{
    for (; first != last; first += 24)
        CallThis(FUN_40cc50, first);
}

extern "C" DWORD* __cdecl UninitCopyStrRefs(const DWORD* first, const DWORD* last, DWORD* dest)
{
    for (; first != last; first += 3, dest += 3)
        ConstructStrRef(dest, first);
    return dest;
}

extern "C" DWORD* __cdecl UninitCopy32Skip1(const DWORD* first, const DWORD* last, DWORD* dest)
{
    for (; first != last; first += 8, dest += 8)
        Construct32Skip1(dest, first);
    return dest;
}

extern "C" BYTE* __cdecl UninitCopy1592(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (; first != last; first += 1592, dest += 1592)
        if (dest)
            memcpy(dest, first, 1592);
    return dest;
}

extern "C" DWORD* __cdecl UninitCopyDwords(const DWORD* first, const DWORD* last, DWORD* dest)
{
    for (; first != last; first++, dest++)
        if (dest)
            *dest = *first;
    return dest;
}

extern "C" void __cdecl Sort_UnguardedInsertionDwords(unsigned* first, unsigned* last)
{
    for (unsigned* i = first; i != last; i++) {
        unsigned v = *i;
        unsigned* p = i;
        while (v < p[-1]) {
            *p = p[-1];
            p--;
        }
        *p = v;
    }
}

template <int OFF> static void UnguardedInsertionByField(void** first, void** last)
{
    for (void** i = first; i != last; i++) {
        void* v = *i;
        void** p = i;
        while (Key<OFF>(v) < Key<OFF>(p[-1])) {
            *p = p[-1];
            p--;
        }
        *p = v;
    }
}

template <int OFF> static void LinearInsertByField(void** first, void** i)
{
    void* v = *i;
    if (Key<OFF>(v) < Key<OFF>(*first)) {
        for (void** p = i; p != first; p--)
            *p = p[-1];
        *first = v;
        return;
    }
    void** p = i;
    while (Key<OFF>(v) < Key<OFF>(p[-1])) {
        *p = p[-1];
        p--;
    }
    *p = v;
}

template <int OFF> static void** MergeByField(void** f1, void** l1, void** f2, void** l2, void** d)
{
    while (f1 != l1 && f2 != l2)
        *d++ = Key<OFF>(*f2) < Key<OFF>(*f1) ? *f2++ : *f1++;
    while (f1 != l1)
        *d++ = *f1++;
    while (f2 != l2)
        *d++ = *f2++;
    return d;
}

extern "C" void __cdecl Sort_UnguardedInsertionBy24(void** f, void** l) { UnguardedInsertionByField<24>(f, l); }
extern "C" void __cdecl Sort_UnguardedInsertionBy328(void** f, void** l) { UnguardedInsertionByField<328>(f, l); }
extern "C" void __cdecl Sort_LinearInsertBy328(void** f, void** i) { LinearInsertByField<328>(f, i); }
extern "C" void __cdecl Sort_LinearInsertBy28(void** f, void** i) { LinearInsertByField<28>(f, i); }
extern "C" void __cdecl Sort_InsertionBy28(void** f, void** l) { InsertionByField<28>(f, l); }
extern "C" void __cdecl Sort_InsertionBy32(void** f, void** l) { InsertionByField<32>(f, l); }
extern "C" void** __cdecl Sort_MergeBy28(void** f1, void** l1, void** f2, void** l2, void** d) { return MergeByField<28>(f1, l1, f2, l2, d); }
extern "C" void** __cdecl Sort_MergeBy32(void** f1, void** l1, void** f2, void** l2, void** d) { return MergeByField<32>(f1, l1, f2, l2, d); }

extern "C" void __cdecl Distance_Add(void** first, void** last, int* n) { *n += last - first; }
extern "C" void __cdecl Iter_Advance(BYTE** it, int n) { *it += n * 4; }

// sorting players by how far the nearest other player is
extern "C" void __cdecl Sort_LinearInsertByNearest(void** first, void** i, int, const BYTE** list)
{
    void* v = *i;
    if (NearestTo(first, list) > NearestTo(&v, list)) {
        for (void** p = i; p != first; p--)
            *p = p[-1];
        *first = v;
        return;
    }
    void* w = v;
    void** d = i;
    void** s = i - 1;
    while (NearestTo(s, list) > NearestTo(&w, list)) {
        *d = *s;
        d = s;
        s--;
    }
    *d = w;
}

extern "C" void** __cdecl Sort_MergeByNearest(void** f1, void** l1, void** f2, void** l2, void** d, const BYTE** list)
{
    while (f1 != l1 && f2 != l2) {
        double r2 = NearestTo(f2, list);
        if (NearestTo(f1, list) > r2)
            *d++ = *f2++;
        else
            *d++ = *f1++;
    }
    while (f1 != l1)
        *d++ = *f1++;
    while (f2 != l2)
        *d++ = *f2++;
    return d;
}

extern "C" void __cdecl Sort_UnguardedInsertByKey(Elem32* last, Elem32 v)
{
    Elem32* d = last;
    Elem32* s = last - 1;
    while (v.key > s->key) {
        *d = *s;
        d = s;
        s--;
    }
    *d = v;
}

extern "C" void __cdecl Sort_UnguardedInsertBy504(void** last, void* v)
{
    void** d = last;
    void** s = last - 1;
    while (Differs(At504(v), At504(*s))) {
        *d = *s;
        d = s;
        s--;
    }
    *d = v;
}

static double DistanceFrom(BYTE* const* p, const DVec3* pt)
{
    const DVec3* a = (const DVec3*)(*p + 56);
    double dx = a->x - pt->x, dy = a->y - pt->y, dz = a->z - pt->z;
    return x87_sqrt((dy * dy + dz * dz) + dx * dx);
}

extern "C" void __cdecl Sort_UnguardedInsertByDistance(BYTE*** last, BYTE** v, DVec3 pt)
{
    BYTE*** d = last;
    BYTE*** s = last - 1;
    for (;;) {
        double la = DistanceFrom(v, &pt);
        double lb = DistanceFrom(*s, &pt);
        if (!(lb > la))
            break;
        *d = *s;
        d = s;
        s--;
    }
    *d = v;
}

extern "C" {
Vec* __fastcall Vec16_Reserve(Vec* s, void*, unsigned n, int)                                 // [FUN_467180]
{
    s->begin = s->end = s->capacityEnd = 0;
    BYTE* p = 0;
    if (n)
        p = (BYTE*)Stl_Allocate(n << 4);
    s->begin = p;
    s->end = p;
    s->capacityEnd = p + (n << 4);
    return s;
}
// player a's nearest other player is farther than b's
int __fastcall Nearest_Farther(const BYTE** s, void*, void* const* a, void* const* b)                    // [FUN_468a60]
{
    double ra = NearestTo(a, (const BYTE**)*s);
    return NearestTo(b, (const BYTE**)*s) > ra;
}
}

// ---------------------------------------------------------------------------
// medium-sized instances: vector assignments, sorts

extern "C" void FUN_461dc0(); KING_ALIAS("_FUN_461dc0", "_Vec16b_AllocCopy@12")    // vector<16-byte record>: allocate and copy (n, first, last)
extern "C" void FUN_469430(); KING_ALIAS("_FUN_469430", "_Rec16_Copy")    // copy 16-byte records (first, last, dest, -, 0)
ASM_PROC(FUN_466300)    // copy-construct 16-byte records (first, last, dest, -)
extern "C" void FUN_461b60(); KING_ALIAS("_FUN_461b60", "_Vec24b_AllocCopy@12")    // vector<24-byte record>: allocate and copy (n, first, last)
ASM_PROC(FUN_466200)    // copy-construct 24-byte records (first, last, dest, -)
extern "C" void FUN_40cc50(); KING_ALIAS("_FUN_40cc50", "@Rec40cc50_Destroy@8")    // a 24-byte record's destructor
extern "C" void FUN_44c160(); KING_ALIAS("_FUN_44c160", "@PodVec_Assign@12")    // vector<double>::operator=
extern "C" void FUN_458cf0(); KING_ALIAS("_FUN_458cf0", "_Bytes_AllocCopy@12")    // vector<char>: allocate and copy (n, first, last)
extern "C" void FUN_462010(); KING_ALIAS("_FUN_462010", "_Vec4b_AllocCopyB@12")    // vector<4 bytes>: allocate and copy (n, first, last)
extern "C" void FUN_46b0e0(); KING_ALIAS("_FUN_46b0e0", "_UninitCopyDwords")    // uninitialized copy of 4-byte values (first, last, dest, -)
extern "C" void FUN_46b330(); KING_ALIAS("_FUN_46b330", "_HeapU_Make")    // make_heap of unsigned values (first, last, 0, 0)
extern "C" void FUN_46c980(); KING_ALIAS("_FUN_46c980", "_HeapU_Pop")    // pop_heap of unsigned values (first, last, 0)
extern "C" void FUN_46b8b0(); KING_ALIAS("_FUN_46b8b0", "_Heap24_Make")    // make_heap of pointers by +24 (first, last, pred, 0, 0)
extern "C" void FUN_46b990(); KING_ALIAS("_FUN_46b990", "_Heap24_Pop")    // pop_heap of pointers by +24 (first, last, pred)
extern "C" void FUN_46bab0(); KING_ALIAS("_FUN_46bab0", "_Heap328_Make")    // make_heap of pointers by +328
extern "C" void FUN_46bc40(); KING_ALIAS("_FUN_46bc40", "_Heap328_Pop")    // pop_heap of pointers by +328
extern "C" BYTE* __fastcall Rec16_DeletingDestructor(BYTE* s, void*, int flags);
extern "C" int* __fastcall Rec42bfe0_Assign(int* self, void*, const int* src);   // aimain.cpp
extern "C" void* __fastcall Str_Assign(void* s, void*, const void* src);          // aimain.cpp

// vector<16-byte record>::operator=
extern "C" BYTE** __fastcall Vec16_Assign(BYTE** d, void*, BYTE* const* s)
{
    if ((const void*)s != (void*)d) {
        unsigned n = (s[1] - s[0]) >> 4;
        if (n > (unsigned)((d[2] - d[0]) >> 4)) {
            BYTE* p = CallThis<BYTE*>(FUN_461dc0, d, n, s[0], s[1]);
            for (BYTE* e = d[0]; e != d[1]; e += 16)
                Rec16_DeletingDestructor(e, 0, 0);
            if ((d[2] - d[0]) >> 4)
                List_FreeNode(d[0], ((d[2] - d[0]) >> 4) << 4);
            d[0] = p;
            d[2] = p + n * 16;
        } else if ((unsigned)((d[1] - d[0]) >> 4) >= n) {
            BYTE* e = CallC<BYTE*>(FUN_469430, s[0], s[1], d[0], s, 0);
            for (; e != d[1]; e += 16)
                CallThis(FUN_602ca0, e);
        } else {
            BYTE* from = s[0];
            BYTE* to = d[0];
            for (int k = (d[1] - d[0]) >> 4; k > 0; k--, from += 16, to += 16)
                Rec42bfe0_Assign((int*)to, 0, (const int*)from);
            CallC(FUN_466300, s[0] + ((d[1] - d[0]) >> 4) * 16, s[1], d[1], s);
        }
        d[1] = d[0] + n * 16;
    }
    d[3] = s[3];
    return d;
}

// operator= of a 24-byte record: two ints and a vector<char>
extern "C" int* __fastcall Rec24b_Assign(int* self, void*, const int* src)
{
    self[0] = src[0];
    self[1] = src[1];
    char** v = (char**)(self + 2);
    char* const* s = (char* const*)(src + 2);
    if ((const void*)s == (void*)v)
        return self;
    char* first = s[0];
    char* last = s[1];
    unsigned n = last - first;
    if (n > (unsigned)(v[2] - v[0])) {
        char* p = CallThis<char*>(FUN_458cf0, v, n, first, last);
        if (v[2] - v[0])
            List_FreeNode(v[0], v[2] - v[0]);
        v[0] = p;
        v[2] = p + n;
    } else if ((unsigned)(v[1] - v[0]) >= n)
        memmove(v[0], first, last - first);
    else {
        memmove(v[0], first, v[1] - v[0]);
        char* mid = s[0] + (v[1] - v[0]);
        memmove(v[1], mid, s[1] - mid);
    }
    v[1] = v[0] + n;
    return self;
}

// vector<24-byte record>::operator=
extern "C" BYTE** __fastcall Vec24_Assign(BYTE** d, void*, BYTE* const* s)
{
    if ((const void*)s == (void*)d)
        return d;
    unsigned n = (s[1] - s[0]) / 24;
    if (n > (unsigned)((d[2] - d[0]) / 24)) {
        BYTE* p = CallThis<BYTE*>(FUN_461b60, d, n, s[0], s[1]);
        for (BYTE* e = d[0]; e != d[1]; e += 24)
            CallThis(FUN_40cc50, e);
        if ((d[2] - d[0]) / 24)
            List_FreeNode(d[0], ((d[2] - d[0]) / 24) * 24);
        d[0] = p;
        d[2] = p + n * 24;
    } else if ((unsigned)((d[1] - d[0]) / 24) >= n) {
        BYTE* to = d[0];
        BYTE* from = s[0];
        for (int k = (s[1] - s[0]) / 24; k > 0; k--, from += 24, to += 24)
            Rec24b_Assign((int*)to, 0, (const int*)from);
        for (; to != d[1]; to += 24)
            CallThis(FUN_40cc50, to);
    } else {
        BYTE* to = d[0];
        BYTE* from = s[0];
        for (int k = (((d[1] - d[0]) / 24) * 24) / 24; k > 0; k--, from += 24, to += 24) {
            F(DWORD, to, 0) = F(DWORD, from, 0);
            F(DWORD, to, 4) = F(DWORD, from, 4);
            CallThis(FUN_44c160, to + 8, from + 8);
        }
        CallC(FUN_466200, s[0] + ((d[1] - d[0]) / 24) * 24, s[1], d[1], s);
    }
    d[1] = d[0] + n * 24;
    return d;
}

// vector<4-byte value>::operator=
extern "C" DWORD** __fastcall Vec4_Assign(DWORD** d, void*, DWORD* const* s)
{
    if ((const void*)s == (void*)d)
        return d;
    DWORD* from = s[0];
    DWORD* to = d[0];
    unsigned n = s[1] - s[0];
    if (n > (unsigned)(d[2] - d[0])) {
        DWORD* p = CallThis<DWORD*>(FUN_462010, d, n, s[0], s[1]);
        if (d[2] - d[0])
            List_FreeNode(d[0], (d[2] - d[0]) * 4);
        d[0] = p;
        d[2] = p + n;
        d[1] = p + n;
        return d;
    }
    if ((unsigned)(d[1] - d[0]) >= n) {
        for (int k = s[1] - from; k > 0; k--)
            *to++ = *from++;
    } else {
        for (int k = d[1] - d[0]; k > 0; k--)
            *to++ = *from++;
        CallC(FUN_46b0e0, s[0] + (d[1] - d[0]), s[1], d[1], s);
    }
    d[1] = d[0] + n;
    return d;
}

// copies [first, last) of string handles over dest
extern "C" BYTE* __cdecl StrRefs_Copy(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = (last - first) / 12; n > 0; n--, first += 12, dest += 12)
        Str_Assign(dest, 0, first);
    return dest;
}

static bool Big(const void* first, const void* last, int limit)
{
    return (int)(((const BYTE*)last - (const BYTE*)first) & ~3) > limit;
}

// the introsort loop of unsigned values
extern "C" void __cdecl Sort_IntroLoopU(unsigned* first, unsigned* last, int, int depth)
{
    if (!Big(first, last, 64))
        return;
    do {
        if (depth == 0) {
            CallC(FUN_46b330, first, last, 0, 0);
            while (Big(first, last, 4)) {
                CallC(FUN_46c980, first, last, 0);
                last--;
            }
            return;
        }
        depth--;
        unsigned* mid = first + (last - first) / 2;
        unsigned a = *first, b = *mid, c = last[-1];
        unsigned* m;
        if (a < b)
            m = b < c ? mid : a < c ? last - 1 : first;
        else
            m = a < c ? first : b < c ? last - 1 : mid;
        unsigned pv = *m;
        unsigned* i = first;
        unsigned* j = last;
        for (;;) {
            while (*i < pv)
                i++;
            j--;
            while (pv < *j)
                j--;
            if (i >= j)
                break;
            unsigned t = *i;
            *i = *j;
            *j = t;
            i++;
        }
        Sort_IntroLoopU(i, last, 0, depth);
        last = i;
    } while (Big(first, last, 64));
}

// the introsort loop of pointers by an int key at +K
template <int K> static void IntroLoopKey(BYTE** first, BYTE** last, int depth, int pred,
                                          void (*makeHeap)(), void (*popHeap)(),
                                          void (*self)(BYTE**, BYTE**, int, int, int))
{
    if (!Big(first, last, 64))
        return;
    do {
        if (depth == 0) {
            CallC(makeHeap, first, last, pred, 0, 0);
            while (Big(first, last, 4)) {
                CallC(popHeap, first, last, pred);
                last--;
            }
            return;
        }
        depth--;
        BYTE** mid = first + (last - first) / 2;
        int a = F(int, *first, K), b = F(int, *mid, K), c = F(int, last[-1], K);
        BYTE** m;
        if (a < b)
            m = b < c ? mid : a < c ? last - 1 : first;
        else
            m = a < c ? first : b < c ? last - 1 : mid;
        BYTE* pv = *m;
        BYTE** i = first;
        BYTE** j = last;
        for (;;) {
            while (F(int, *i, K) < F(int, pv, K))
                i++;
            j--;
            while (F(int, pv, K) < F(int, *j, K))
                j--;
            if (i >= j)
                break;
            BYTE* t = *i;
            *i = *j;
            *j = t;
            i++;
        }
        self(i, last, 0, depth, pred);
        last = i;
    } while (Big(first, last, 64));
}

extern "C" void __cdecl Sort_IntroLoop24a(BYTE** first, BYTE** last, int, int depth, int pred)
{
    IntroLoopKey<24>(first, last, depth, pred, FUN_46b8b0, FUN_46b990, Sort_IntroLoop24a);
}

extern "C" void __cdecl Sort_IntroLoop24b(BYTE** first, BYTE** last, int, int depth, int pred)
{
    IntroLoopKey<24>(first, last, depth, pred, FUN_46b8b0, FUN_46b990, Sort_IntroLoop24b);
}

extern "C" void __cdecl Sort_IntroLoop328(BYTE** first, BYTE** last, int, int depth, int pred)
{
    IntroLoopKey<328>(first, last, depth, pred, FUN_46bab0, FUN_46bc40, Sort_IntroLoop328);
}

// insertion sort of pointers by the int at +24 (the final pass of the sort)
static void Insert24(BYTE** first, BYTE** i)
{
    BYTE* v = *i;
    if (F(int, v, 24) < F(int, *first, 24)) {
        memmove(first + 1, first, (i - first) * sizeof(BYTE*));
        *first = v;
        return;
    }
    BYTE** j = i;
    BYTE* prev = j[-1];
    while (F(int, v, 24) < F(int, prev, 24)) {
        *j = prev;
        j--;
        prev = j[-1];
    }
    *j = v;
}

extern "C" void __cdecl Sort_FinalInsertion24(BYTE** first, BYTE** last)
{
    if (!Big(first, last, 64)) {
        if (first != last)
            for (BYTE** i = first + 1; i != last; i++)
                Insert24(first, i);
        return;
    }
    for (BYTE** i = first + 1; i != first + 16; i++)
        Insert24(first, i);
    for (BYTE** i = first + 16; i != last; i++) {
        BYTE* v = *i;
        BYTE** j = i;
        BYTE* prev = j[-1];
        while (F(int, v, 24) < F(int, prev, 24)) {
            *j = prev;
            j--;
            prev = j[-1];
        }
        *j = v;
    }
}

// ---------------------------------------------------------------------------
// copy and copy_backward of the AI's records

extern "C" void FUN_46c900(); KING_ALIAS("_FUN_46c900", "@Mult46c900_DeletingDestructor@12")    // a 24-byte record's deleting destructor (flags)
extern "C" void FUN_466c50(); KING_ALIAS("_FUN_466c50", "_Rec24b_Copy")    // copy 24-byte records (first, last, dest, -, 0)
extern "C" void FUN_46b300(); KING_ALIAS("_FUN_46b300", "_Copy32")    // copy 4-byte values (first, last, dest, -, 0)
extern "C" BYTE* __cdecl Copy1(const BYTE* first, const BYTE* last, BYTE* dest);   // aimain.cpp
extern "C" void __cdecl Order144_AssignPlain(BYTE* d, const BYTE* s);              // aimain.cpp

static void Copy4(BYTE* d, const BYTE* s, int from, int to)
{
    for (int i = from; i <= to; i += 4)
        F(DWORD, d, i) = F(DWORD, s, i);
}

// vector<char>::operator=, inlined; the split copy through memmove or Copy1
static void CharVec_Copy(char** v, char* const* s, bool copy1)
{
    if ((const void*)s == (void*)v)
        return;
    char* first = s[0];
    char* last = s[1];
    unsigned n = last - first;
    if (n > (unsigned)(v[2] - v[0])) {
        char* p = CallThis<char*>(FUN_458cf0, v, n, first, last);
        if (v[2] - v[0])
            List_FreeNode(v[0], v[2] - v[0]);
        v[0] = p;
        v[2] = p + n;
    } else if ((unsigned)(v[1] - v[0]) >= n) {
        if (copy1)
            Copy1((BYTE*)first, (BYTE*)last, (BYTE*)v[0]);
        else
            memmove(v[0], first, last - first);
    } else if (copy1) {
        Copy1((BYTE*)first, (BYTE*)first + (v[1] - v[0]), (BYTE*)v[0]);
        Copy1((BYTE*)s[0] + (v[1] - v[0]), (BYTE*)s[1], (BYTE*)v[1]);
    } else {
        memmove(v[0], first, v[1] - v[0]);
        char* mid = s[0] + (v[1] - v[0]);
        memmove(v[1], mid, s[1] - mid);
    }
    v[1] = v[0] + n;
}

static void Rec104_Assign(BYTE* d, const BYTE* s)
{
    Str_Assign(d, 0, s);
    Copy4(d, s, 16, 48);
    Copy4(d, s, 56, 72);
    Copy4(d, s, 80, 84);
    CharVec_Copy((char**)(d + 88), (char* const*)(s + 88), false);
}

// a string handle and a vector of 24-byte records
static void Rec24c_Assign(BYTE* dest, const BYTE* first)
{
    Str_Assign(dest, 0, first);
    BYTE** d = (BYTE**)(dest + 12);
    BYTE* const* s = (BYTE* const*)(first + 12);
    if ((const void*)s == (void*)d)
        return;
    unsigned n = (s[1] - s[0]) / 24;
    if (n > (unsigned)((d[2] - d[0]) / 24)) {
        BYTE* p = CallThis<BYTE*>(FUN_461b60, d, n, s[0], s[1]);
        for (BYTE* e = d[0]; e != d[1]; e += 24)
            CallThis(FUN_46c900, e, 0);
        if ((d[2] - d[0]) / 24)
            List_FreeNode(d[0], ((d[2] - d[0]) / 24) * 24);
        d[0] = p;
        d[2] = p + n * 24;
    } else if ((unsigned)((d[1] - d[0]) / 24) >= n) {
        BYTE* e = CallC<BYTE*>(FUN_466c50, s[0], s[1], d[0], first, 0);
        for (; e != d[1]; e += 24)
            CallThis(FUN_46c900, e, 0);
    } else {
        CallC(FUN_466c50, s[0], s[0] + ((d[1] - d[0]) / 24) * 24, d[0], first, 0);
        CallC(FUN_466200, s[0] + ((d[1] - d[0]) / 24) * 24, s[1], d[1], first);
    }
    d[1] = d[0] + n * 24;
}

// three ints, a vector<char> and a vector of 4-byte values
static void Rec36_Assign(BYTE* dest, const BYTE* src)
{
    Copy4(dest, src, 0, 8);
    CharVec_Copy((char**)(dest + 12), (char* const*)(src + 12), false);
    DWORD** d = (DWORD**)(dest + 24);
    DWORD* const* s = (DWORD* const*)(src + 24);
    if ((const void*)s == (void*)d)
        return;
    unsigned n = s[1] - s[0];
    if (n > (unsigned)(d[2] - d[0])) {
        DWORD* p = CallThis<DWORD*>(FUN_462010, d, n, s[0], s[1]);
        if (d[2] - d[0])
            List_FreeNode(d[0], (d[2] - d[0]) * 4);
        d[0] = p;
        d[2] = p + n;
    } else if ((unsigned)(d[1] - d[0]) >= n)
        CallC(FUN_46b300, s[0], s[1], d[0], dest, 0);
    else {
        CallC(FUN_46b300, s[0], s[0] + (d[1] - d[0]), d[0], dest, 0);
        CallC(FUN_46b0e0, s[0] + (d[1] - d[0]), s[1], d[1], dest);
    }
    d[1] = d[0] + n;
}

static void CharVec12_Assign(BYTE* d, const BYTE* s) { CharVec_Copy((char**)d, (char* const*)s, false); }
static void StrRef_Assign(BYTE* d, const BYTE* s) { Str_Assign(d, 0, s); }
static void Rec24b_AssignP(BYTE* d, const BYTE* s) { Rec24b_Assign((int*)d, 0, (const int*)s); }

static void Rec56_Assign(BYTE* d, const BYTE* s)
{
    F(DWORD, d, 0) = F(DWORD, s, 0);
    CharVec_Copy((char**)(d + 4), (char* const*)(s + 4), false);
    Copy4(d, s, 16, 32);
    Copy4(d, s, 40, 52);
}

// a 192-byte record; the pointers at +100..+112 are the first elements of its four vectors
static void Rec192b_Assign(BYTE* d, const BYTE* s)
{
    F(DWORD, d, 0) = F(DWORD, s, 0);
    Copy4(d, s, 8, 12);
    memcpy(d + 16, s + 16, 32);
    F(DWORD, d, 48) = F(DWORD, s, 48);
    CharVec_Copy((char**)(d + 52), (char* const*)(s + 52), true);
    CallThis(FUN_44c160, d + 64, s + 64);
    CallThis(FUN_44c160, d + 76, s + 76);
    CallThis(FUN_44c160, d + 88, s + 88);
    Copy4(d, s, 100, 112);
    Copy4(d, s, 120, 180);
    F(BYTE, d, 184) = F(BYTE, s, 184);
    for (int k = 0; k < 4; k++) {
        F(DWORD, d, 100 + k * 4) = 0;
    }
    for (int k = 0; k < 4; k++)
        if (F(BYTE*, d, 52 + k * 12) != F(BYTE*, d, 56 + k * 12))
            F(BYTE*, d, 100 + k * 4) = F(BYTE*, d, 52 + k * 12);
}

template <int N> static BYTE* CopyFwd(const BYTE* first, const BYTE* last, BYTE* dest,
                                      void (*assign)(BYTE*, const BYTE*))
{
    for (int n = (last - first) / N; n > 0; n--, first += N, dest += N)
        assign(dest, first);
    return dest;
}

template <int N> static BYTE* CopyBwd(const BYTE* first, const BYTE* last, BYTE* dest,
                                      void (*assign)(BYTE*, const BYTE*))
{
    for (int n = (last - first) / N; n > 0; n--) {
        last -= N;
        dest -= N;
        assign(dest, last);
    }
    return dest;
}

extern "C" BYTE* __cdecl Order144_CopyA(const BYTE* f, const BYTE* l, BYTE* d) { return CopyFwd<144>(f, l, d, Order144_AssignPlain); }
extern "C" BYTE* __cdecl Order144_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<144>(f, l, d, Order144_AssignPlain); }
extern "C" BYTE* __cdecl Rec104_Copy(const BYTE* f, const BYTE* l, BYTE* d) { return CopyFwd<104>(f, l, d, Rec104_Assign); }
extern "C" BYTE* __cdecl Rec104_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<104>(f, l, d, Rec104_Assign); }
extern "C" BYTE* __cdecl StrRefs_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<12>(f, l, d, StrRef_Assign); }
extern "C" BYTE* __cdecl Rec24b_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<24>(f, l, d, Rec24b_AssignP); }
extern "C" BYTE* __cdecl Rec24c_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<24>(f, l, d, Rec24c_Assign); }
extern "C" BYTE* __cdecl Rec36_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<36>(f, l, d, Rec36_Assign); }
extern "C" BYTE* __cdecl CharVecs_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<12>(f, l, d, CharVec12_Assign); }
extern "C" BYTE* __cdecl Rec56_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<56>(f, l, d, Rec56_Assign); }
extern "C" BYTE* __cdecl Rec192_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<192>(f, l, d, Rec192b_Assign); }

// vector<16-byte record>::operator= (the other instance)
extern "C" BYTE** __fastcall Vec16_AssignB(BYTE** d, void*, BYTE* const* s)
{
    if ((const void*)s == (void*)d)
        return d;
    unsigned n = (s[1] - s[0]) >> 4;
    if (n > (unsigned)((d[2] - d[0]) >> 4)) {
        BYTE* p = CallThis<BYTE*>(FUN_461dc0, d, n, s[0], s[1]);
        for (BYTE* e = d[0]; e != d[1]; e += 16)
            CallThis(FUN_602ca0, e);
        if ((d[2] - d[0]) >> 4)
            List_FreeNode(d[0], ((d[2] - d[0]) >> 4) << 4);
        d[1] = p + n * 16;
        d[0] = p;
        d[2] = p + n * 16;
        return d;
    }
    BYTE* to = d[0];
    BYTE* from = s[0];
    if ((unsigned)((d[1] - d[0]) >> 4) >= n) {
        for (int k = (s[1] - s[0]) >> 4; k > 0; k--, from += 16, to += 16)
            Rec42bfe0_Assign((int*)to, 0, (const int*)from);
        for (; to != d[1]; to += 16)
            CallThis(FUN_602ca0, to);
    } else {
        for (int k = ((d[1] - d[0]) >> 4 << 4) >> 4; k > 0; k--, from += 16, to += 16) {
            F(DWORD, to, 0) = F(DWORD, from, 0);
            CallThis(FUN_44c160, to + 4, from + 4);
        }
        CallC(FUN_466300, s[0] + ((d[1] - d[0]) >> 4) * 16, s[1], d[1], s);
    }
    d[1] = d[0] + n * 16;
    return d;
}

// ---------------------------------------------------------------------------
// partial sort, stable sorts and merges of pointers by an int key

extern "C" void __cdecl Heap24_Adjust(BYTE** first, int hole, int n, BYTE* v);   // aimult.cpp
extern "C" void __cdecl Heap24_Push(BYTE** first, int hole, int top, BYTE* v);

#define KEY(p, k) F(int, p, k)

// partial_sort(first, middle, last) by the int at +24
extern "C" void __cdecl Sort_Partial24(BYTE** first, BYTE** middle, BYTE** last, int pred)
{
    int bytes = (int)((BYTE*)middle - (BYTE*)first);
    int len = bytes >> 2;
    if ((bytes & ~3) >= 8)
        for (int parent = (len - 2) / 2;; parent--) {
            Heap24_Adjust(first, parent, len, first[parent]);
            if (parent == 0)
                break;
        }
    for (BYTE** i = middle; i < last; i++) {
        BYTE* v = *i;
        if (!(KEY(v, 24) < KEY(*first, 24)))
            continue;
        *i = *first;
        int hole = 0, child = 2;
        while (child < len) {
            if (KEY(first[child], 24) < KEY(first[child - 1], 24))
                child--;
            first[hole] = first[child];
            hole = child;
            child = child * 2 + 2;
        }
        if (child == len) {
            first[hole] = first[child - 1];
            hole = child - 1;
        }
        int parent = (hole - 1) / 2;
        while (hole > 0 && KEY(first[parent], 24) < KEY(v, 24)) {
            first[hole] = first[parent];
            hole = parent;
            parent = (parent - 1) / 2;
        }
        first[hole] = v;
    }
    BYTE** end = middle;
    while ((int)(((BYTE*)end - (BYTE*)first) & ~3) > 4) {
        BYTE* v = end[-1];
        end[-1] = *first;
        end--;
        int n = end - first;
        int hole = 0, child = 2;
        while (child < n) {
            if (KEY(first[child], 24) < KEY(first[child - 1], 24))
                child--;
            first[hole] = first[child];
            hole = child;
            child = child * 2 + 2;
        }
        if (child == n) {
            first[hole] = first[child - 1];
            hole = child - 1;
        }
        Heap24_Push(first, hole, 0, v);
    }
}

// rotate(first, middle, last): returns the new position of *first
extern "C" BYTE** __cdecl Ptrs_Rotate(BYTE** first, BYTE** middle, BYTE** last)
{
    if (first == middle)
        return last;
    if (middle == last)
        return first;
    BYTE** r = first + (last - middle);
    for (BYTE **a = first, **b = middle; a != b && a != --b; a++) {
        BYTE* t = *a; *a = *b; *b = t;
    }
    for (BYTE **a = middle, **b = last; a != b && a != --b; a++) {
        BYTE* t = *a; *a = *b; *b = t;
    }
    for (BYTE **a = first, **b = last; a != b && a != --b; a++) {
        BYTE* t = *a; *a = *b; *b = t;
    }
    return r;
}

// a stable merge without a buffer (the result of a stable merge is unique)
template <int K> static void MergeKey(BYTE** first, BYTE** middle, BYTE** last, int len1, int len2)
{
    if (len1 == 0 || len2 == 0)
        return;
    if (len1 + len2 == 2) {
        if (KEY(*middle, K) < KEY(*first, K)) {
            BYTE* t = *first; *first = *middle; *middle = t;
        }
        return;
    }
    BYTE **cut1, **cut2;
    int len11, len22;
    if (len1 > len2) {
        len11 = len1 / 2;
        cut1 = first + len11;
        BYTE** lo = middle;
        for (int n = last - middle; n > 0;) {
            int h = n >> 1;
            if (KEY(lo[h], K) < KEY(*cut1, K)) {
                lo += h + 1;
                n -= h + 1;
            } else
                n = h;
        }
        cut2 = lo;
        len22 = cut2 - middle;
    } else {
        len22 = len2 / 2;
        cut2 = middle + len22;
        BYTE** lo = first;
        for (int n = middle - first; n > 0;) {
            int h = n >> 1;
            if (KEY(lo[h], K) <= KEY(*cut2, K)) {
                lo += h + 1;
                n -= h + 1;
            } else
                n = h;
        }
        cut1 = lo;
        len11 = cut1 - first;
    }
    BYTE** mid = Ptrs_Rotate(cut1, middle, cut2);
    MergeKey<K>(first, cut1, mid, len11, len22);
    MergeKey<K>(mid, cut2, last, len1 - len11, len2 - len22);
}

template <int K> static void StableSortKey(BYTE** first, BYTE** last)
{
    if ((int)(((BYTE*)last - (BYTE*)first) & ~3) < 60) {
        if (first == last)
            return;
        for (BYTE** i = first + 1; i != last; i++) {
            BYTE* v = *i;
            BYTE** j = i;
            if (KEY(v, K) < KEY(*first, K)) {
                memmove(first + 1, first, (i - first) * sizeof(BYTE*));
                *first = v;
                continue;
            }
            for (; KEY(v, K) < KEY(j[-1], K); j--)
                *j = j[-1];
            *j = v;
        }
        return;
    }
    BYTE** mid = first + (last - first) / 2;
    StableSortKey<K>(first, mid);
    StableSortKey<K>(mid, last);
    MergeKey<K>(first, mid, last, mid - first, last - mid);
}

extern "C" void __cdecl Sort_Stable28(BYTE** first, BYTE** last, int) { StableSortKey<28>(first, last); }
extern "C" void __cdecl Sort_Stable32(BYTE** first, BYTE** last, int) { StableSortKey<32>(first, last); }
extern "C" void __cdecl Merge28(BYTE** f, BYTE** m, BYTE** l, int n1, int n2, int) { MergeKey<28>(f, m, l, n1, n2); }
extern "C" void __cdecl Merge32(BYTE** f, BYTE** m, BYTE** l, int n1, int n2, int) { MergeKey<32>(f, m, l, n1, n2); }

// ---------------------------------------------------------------------------
// the pieces of stable_sort with a buffer: each piece's result is unique

template <int K> static void InsertionKey(BYTE** first, BYTE** last)
{
    if (first == last)
        return;
    for (BYTE** i = first + 1; i != last; i++) {
        BYTE* v = *i;
        if (KEY(v, K) < KEY(*first, K)) {
            memmove(first + 1, first, (i - first) * sizeof(BYTE*));
            *first = v;
            continue;
        }
        BYTE** j = i;
        for (; KEY(v, K) < KEY(j[-1], K); j--)
            *j = j[-1];
        *j = v;
    }
}

// insertion sorts of the chunks of n elements, then of the rest
extern "C" void __cdecl Sort_ChunkInsertion28(BYTE** first, BYTE** last, int n)
{
    for (; last - first >= n; first += n)
        InsertionKey<28>(first, first + n);
    InsertionKey<28>(first, last);
}

template <int K> static BYTE** MergeInto(BYTE** a, BYTE** ae, BYTE** b, BYTE** be, BYTE** out)
{
    while (a != ae && b != be)
        *out++ = KEY(*b, K) < KEY(*a, K) ? *b++ : *a++;
    while (a != ae)
        *out++ = *a++;
    while (b != be)
        *out++ = *b++;
    return out;
}

// merges the runs of `step` elements pairwise into result
template <int K> static void MergeSortLoop(BYTE** first, BYTE** last, BYTE** result, int step)
{
    for (; last - first >= step * 2; first += step * 2)
        result = MergeInto<K>(first, first + step, first + step, first + step * 2, result);
    int s = step < last - first ? step : last - first;
    MergeInto<K>(first, first + s, first + s, last, result);
}

extern "C" void __cdecl Sort_MergeLoop28(BYTE** f, BYTE** l, BYTE** r, int step) { MergeSortLoop<28>(f, l, r, step); }
extern "C" void __cdecl Sort_MergeLoop32(BYTE** f, BYTE** l, BYTE** r, int step) { MergeSortLoop<32>(f, l, r, step); }

// merges with a buffer; the buffer is only scratch space
extern "C" void __cdecl Merge28_Adaptive(BYTE** f, BYTE** m, BYTE** l, int n1, int n2, BYTE**, int, int)
{
    MergeKey<28>(f, m, l, n1, n2);
}

extern "C" void __cdecl Merge32_Adaptive(BYTE** f, BYTE** m, BYTE** l, int n1, int n2, BYTE**, int, int)
{
    MergeKey<32>(f, m, l, n1, n2);
}

// rotate with a buffer: the same result as rotate
extern "C" BYTE** __cdecl Ptrs_RotateAdaptive(BYTE** f, BYTE** m, BYTE** l, int, int, BYTE**, int)
{
    return Ptrs_Rotate(f, m, l);
}

// ---------------------------------------------------------------------------
// partial sort by +328, copies of 16- and 24-byte records

extern "C" void __cdecl Heap328_Adjust(BYTE** first, int hole, int n, BYTE* v);   // aimult.cpp

// the heap's hole at the top moved down to a leaf, then v pushed up from it
template <int K> static void HeapReplaceTop(BYTE** first, int n, BYTE* v)
{
    int hole = 0, child = 2;
    while (child < n) {
        if (KEY(first[child], K) < KEY(first[child - 1], K))
            child--;
        first[hole] = first[child];
        hole = child;
        child = child * 2 + 2;
    }
    if (child == n) {
        first[hole] = first[child - 1];
        hole = child - 1;
    }
    while (hole > 0) {
        int parent = (hole - 1) / 2;
        if (!(KEY(first[parent], K) < KEY(v, K)))
            break;
        first[hole] = first[parent];
        hole = parent;
    }
    first[hole] = v;
}

extern "C" void __cdecl Sort_Partial328(BYTE** first, BYTE** middle, BYTE** last, int, int)
{
    int bytes = (int)((BYTE*)middle - (BYTE*)first);
    int len = bytes >> 2;
    if ((bytes & ~3) >= 8)
        for (int parent = (len - 2) / 2;; parent--) {
            Heap328_Adjust(first, parent, len, first[parent]);
            if (parent == 0)
                break;
        }
    for (BYTE** i = middle; i < last; i++) {
        BYTE* v = *i;
        if (KEY(v, 328) < KEY(*first, 328)) {
            *i = *first;
            HeapReplaceTop<328>(first, len, v);
        }
    }
    for (BYTE** end = middle; (int)(((BYTE*)end - (BYTE*)first) & ~3) > 4;) {
        BYTE* v = end[-1];
        end[-1] = *first;
        end--;
        HeapReplaceTop<328>(first, end - first, v);
    }
}

static void Rec16_AssignP(BYTE* d, const BYTE* s) { Rec42bfe0_Assign((int*)d, 0, (const int*)s); }

extern "C" BYTE* __cdecl Rec16_Copy(const BYTE* f, const BYTE* l, BYTE* d) { return CopyFwd<16>(f, l, d, Rec16_AssignP); }
extern "C" BYTE* __cdecl Rec24b_Copy(const BYTE* f, const BYTE* l, BYTE* d) { return CopyFwd<24>(f, l, d, Rec24b_AssignP); }

// ---------------------------------------------------------------------------
// 16-byte records (an int and a vector<char>), and the stable sort of the
// road's 40-byte records by the int at +32

static void Rec16m_Assign(BYTE* d, const BYTE* s)
{
    F(DWORD, d, 0) = F(DWORD, s, 0);
    CharVec_Copy((char**)(d + 4), (char* const*)(s + 4), false);
}

static void Rec16c_Assign(BYTE* d, const BYTE* s)
{
    F(DWORD, d, 0) = F(DWORD, s, 0);
    CharVec_Copy((char**)(d + 4), (char* const*)(s + 4), true);
}

extern "C" BYTE* __cdecl Rec16m_Copy(const BYTE* f, const BYTE* l, BYTE* d) { return CopyFwd<16>(f, l, d, Rec16m_Assign); }
extern "C" BYTE* __cdecl Rec16m_CopyBack(const BYTE* f, const BYTE* l, BYTE* d) { return CopyBwd<16>(f, l, d, Rec16m_Assign); }

// vector<16-byte record>::erase(first, last)
extern "C" BYTE* __fastcall Vec16c_Erase(BYTE** self, void*, BYTE* first, BYTE* last)
{
    BYTE* d = first;
    BYTE* s = last;
    for (int n = (self[1] - last) >> 4; n > 0; n--, d += 16, s += 16)
        Rec16c_Assign(d, s);
    for (BYTE* e = self[1]; d != e; d += 16)
        CallThis(FUN_602ca0, d);
    self[1] -= ((last - first) >> 4) << 4;
    return first;
}

// rotate of N-byte elements: returns the new position of *first
template <int N> static void SwapN(BYTE* a, BYTE* b)
{
    BYTE t[N];
    memcpy(t, a, N);
    memcpy(a, b, N);
    memcpy(b, t, N);
}

template <int N> static void ReverseN(BYTE* a, BYTE* b)
{
    for (; a != b && a != (b -= N); a += N)
        SwapN<N>(a, b);
}

template <int N> static BYTE* RotateN(BYTE* first, BYTE* middle, BYTE* last)
{
    if (first == middle)
        return last;
    if (middle == last)
        return first;
    ReverseN<N>(first, middle);
    ReverseN<N>(middle, last);
    ReverseN<N>(first, last);
    return first + (last - middle);
}

extern "C" BYTE* __cdecl Rotate8(BYTE* f, BYTE* m, BYTE* l) { return RotateN<8>(f, m, l); }
extern "C" BYTE* __cdecl Rotate40(BYTE* f, BYTE* m, BYTE* l) { return RotateN<40>(f, m, l); }
// rotate with a buffer: the same result
extern "C" BYTE* __cdecl Rotate8_Adaptive(BYTE* f, BYTE* m, BYTE* l, int, int, BYTE*, int) { return RotateN<8>(f, m, l); }

// insertion sort of 40-byte records by the int at +32 (stable)
extern "C" void __cdecl Road40_Insertion(BYTE* first, BYTE* last)
{
    if (first == last)
        return;
    for (BYTE* i = first + 40; i != last; i += 40) {
        BYTE v[40];
        memcpy(v, i, 40);
        BYTE* j = i;
        for (; j != first && KEY(v, 32) < KEY(j - 40, 32); j -= 40)
            memcpy(j, j - 40, 40);
        memcpy(j, v, 40);
    }
}

// merges runs of `step` records pairwise into result (stable)
static BYTE* Merge40(BYTE* a, BYTE* ae, BYTE* b, BYTE* be, BYTE* out)
{
    while (a != ae && b != be) {
        if (KEY(b, 32) < KEY(a, 32)) {
            memcpy(out, b, 40);
            b += 40;
        } else {
            memcpy(out, a, 40);
            a += 40;
        }
        out += 40;
    }
    for (; a != ae; a += 40, out += 40)
        memcpy(out, a, 40);
    for (; b != be; b += 40, out += 40)
        memcpy(out, b, 40);
    return out;
}

extern "C" void __cdecl Road40_MergeLoop(BYTE* first, BYTE* last, BYTE* result, int step)
{
    int run = step * 40;
    for (; (last - first) / 40 >= step * 2; first += run * 2)
        result = Merge40(first, first + run, first + run, first + run * 2, result);
    int n = (last - first) / 40;
    int s = (step < n ? step : n) * 40;
    Merge40(first, first + s, first + s, last, result);
}

// ---------------------------------------------------------------------------
// 12-byte records sorted by the unsigned at +8 (stable)

static unsigned Key12(const BYTE* p) { return F(unsigned, p, 8); }

// inserts *last into the sorted [first, last)
extern "C" void __cdecl Rec12_LinearInsert(BYTE* first, BYTE* last)
{
    BYTE v[12];
    memcpy(v, last, 12);
    if (Key12(v) < Key12(first)) {
        memmove(first + 12, first, last - first);
        memcpy(first, v, 12);
        return;
    }
    BYTE* j = last;
    for (; Key12(v) < Key12(j - 12); j -= 12)
        memcpy(j, j - 12, 12);
    memcpy(j, v, 12);
}

extern "C" void __cdecl Rec12_Insertion(BYTE* first, BYTE* last)
{
    if (first == last)
        return;
    for (BYTE* i = first + 12; i != last; i += 12)
        Rec12_LinearInsert(first, i);
}

extern "C" BYTE* __cdecl Rotate12(BYTE* f, BYTE* m, BYTE* l) { return RotateN<12>(f, m, l); }

// merge of two sorted ranges into out
extern "C" BYTE* __cdecl Rec12_Merge(BYTE* a, BYTE* ae, BYTE* b, BYTE* be, BYTE* out)
{
    while (a != ae && b != be) {
        if (Key12(b) < Key12(a)) {
            memcpy(out, b, 12);
            b += 12;
        } else {
            memcpy(out, a, 12);
            a += 12;
        }
        out += 12;
    }
    for (; a != ae; a += 12, out += 12)
        memcpy(out, a, 12);
    for (; b != be; b += 12, out += 12)
        memcpy(out, b, 12);
    return out;
}

// make_heap of pointers by the int at +24 / +328
extern "C" void __cdecl Heap24_Make(BYTE** first, BYTE** last, int, int, int)
{
    if ((int)(((BYTE*)last - (BYTE*)first) & ~3) < 8)
        return;
    int len = last - first;
    for (int p = (len - 2) / 2;; p--) {
        Heap24_Adjust(first, p, len, first[p]);
        if (p == 0)
            return;
    }
}

extern "C" void __cdecl Heap328_Make(BYTE** first, BYTE** last, int, int, int)
{
    if ((int)(((BYTE*)last - (BYTE*)first) & ~3) < 8)
        return;
    int len = last - first;
    for (int p = (len - 2) / 2;; p--) {
        Heap328_Adjust(first, p, len, first[p]);
        if (p == 0)
            return;
    }
}

// ---------------------------------------------------------------------------
// stable sort of doubles, largest first (the road's costs): elements are
// copied as two dwords; only the comparisons load them

extern "C" void FUN_47f7a0(); KING_ALIAS("_FUN_47f7a0", "_Doubles_LowerBoundDesc")    // the first position in a run whose value is not above x (first, last, &x, -, 0)
extern "C" void FUN_47f7f0(); KING_ALIAS("_FUN_47f7f0", "_Doubles_UpperBoundDesc")    // the first position in a run whose value is below x

typedef unsigned __int64 Bits8;

static double* Copy8(const double* f, const double* l, double* out)
{
    for (; f != l; f++, out++)
        *(Bits8*)out = *(const Bits8*)f;
    return out;
}

static double* CopyBack8(const double* f, const double* l, double* out)
{
    while (l != f)
        *(Bits8*)--out = *(const Bits8*)--l;
    return out;
}

static double* MergeDesc(const double* a, const double* ae, const double* b, const double* be, double* out)
{
    while (a != ae && b != be) {
        if (*b > *a)
            *(Bits8*)out++ = *(const Bits8*)b++;
        else
            *(Bits8*)out++ = *(const Bits8*)a++;
    }
    out = Copy8(a, ae, out);
    return Copy8(b, be, out);
}

extern "C" void __cdecl Doubles_MergeLoop(double* first, double* last, double* result, int step)
{
    for (; last - first >= step * 2; first += step * 2)
        result = MergeDesc(first, first + step, first + step, first + step * 2, result);
    int s = step < last - first ? step : last - first;
    MergeDesc(first, first + s, first + s, last, result);
}

extern "C" BYTE* __cdecl Rotate8_Adaptive(BYTE* f, BYTE* m, BYTE* l, int, int, BYTE*, int);

// merge of [first, middle) and [middle, last) with a buffer
extern "C" void __cdecl Doubles_MergeAdaptive(double* first, double* middle, double* last, int len1, int len2,
                                              double* buf, int bufSize, int pred)
{
    for (;;) {
        if (len1 <= len2 && len1 <= bufSize) {
            double* bufEnd = Copy8(first, middle, buf);
            double* out = first;
            const double* b = buf;
            while (b != bufEnd && middle != last) {
                if (*middle > *b)
                    *(Bits8*)out++ = *(const Bits8*)middle++;
                else
                    *(Bits8*)out++ = *(const Bits8*)b++;
            }
            out = Copy8(b, bufEnd, out);
            Copy8(middle, last, out);
            return;
        }
        if (len2 <= bufSize) {
            double* bufEnd = Copy8(middle, last, buf);
            if (first == middle) {
                CopyBack8(buf, bufEnd, last);
                return;
            }
            if (buf == bufEnd) {
                CopyBack8(first, middle, last);
                return;
            }
            double* a = middle - 1;
            double* b = bufEnd - 1;
            for (;;) {
                if (!(*b > *a)) {
                    *(Bits8*)--last = *(Bits8*)b;
                    if (buf == b) {
                        CopyBack8(first, a + 1, last);
                        return;
                    }
                    b--;
                } else {
                    *(Bits8*)--last = *(Bits8*)a;
                    if (first == a) {
                        CopyBack8(buf, b + 1, last);
                        return;
                    }
                    a--;
                }
            }
        }
        double *cut1, *cut2;
        int len11, len22;
        if (len1 > len2) {
            len11 = len1 / 2;
            cut1 = first + len11;
            cut2 = CallC<double*>(FUN_47f7a0, middle, last, cut1, pred, 0);
            len22 = cut2 - middle;
        } else {
            len22 = len2 / 2;
            cut2 = middle + len22;
            cut1 = CallC<double*>(FUN_47f7f0, first, middle, cut2, pred, 0);
            len11 = cut1 - first;
        }
        double* mid = (double*)Rotate8_Adaptive((BYTE*)cut1, (BYTE*)middle, (BYTE*)cut2, len1 - len11, len22,
                                                (BYTE*)buf, bufSize);
        Doubles_MergeAdaptive(first, cut1, mid, len11, len22, buf, bufSize, pred);
        first = mid;
        middle = cut2;
        len1 -= len11;
        len2 -= len22;
    }
}

// ---------------------------------------------------------------------------
// the stable sort of players by Players_NearestTo(player, ctx): the pieces
// of stable_sort; each piece's result is unique, the key being a pure function

extern "C" double __cdecl Players_NearestTo(const int* const* node, const BYTE** list);   // aimain.cpp

struct NearLess {
    int ctx;
    double Key(BYTE* const* p) const { return Players_NearestTo((const int* const*)p, (const BYTE**)ctx); }
    bool operator()(BYTE* const* a, BYTE* const* b) const { return Key(a) < Key(b); }
};

template <class L> static void InsertionLess(BYTE** first, BYTE** last, L less)
{
    if (first == last)
        return;
    for (BYTE** i = first + 1; i != last; i++) {
        BYTE* v = *i;
        BYTE** j = i;
        for (; j != first && less(&v, j - 1); j--)
            *j = j[-1];
        *j = v;
    }
}

template <class L> static void MergeNoBufLess(BYTE** first, BYTE** middle, BYTE** last, int len1, int len2, L less)
{
    if (len1 == 0 || len2 == 0)
        return;
    if (len1 + len2 == 2) {
        if (less(middle, first)) {
            BYTE* t = *first; *first = *middle; *middle = t;
        }
        return;
    }
    BYTE **cut1, **cut2;
    int len11, len22;
    if (len1 > len2) {
        len11 = len1 / 2;
        cut1 = first + len11;
        cut2 = middle;
        for (int n = last - middle; n > 0;) {
            int h = n >> 1;
            if (less(cut2 + h, cut1)) {
                cut2 += h + 1;
                n -= h + 1;
            } else
                n = h;
        }
        len22 = cut2 - middle;
    } else {
        len22 = len2 / 2;
        cut2 = middle + len22;
        cut1 = first;
        for (int n = middle - first; n > 0;) {
            int h = n >> 1;
            if (!less(cut2, cut1 + h)) {
                cut1 += h + 1;
                n -= h + 1;
            } else
                n = h;
        }
        len11 = cut1 - first;
    }
    BYTE** mid = Ptrs_Rotate(cut1, middle, cut2);
    MergeNoBufLess(first, cut1, mid, len11, len22, less);
    MergeNoBufLess(mid, cut2, last, len1 - len11, len2 - len22, less);
}

template <class L> static void StableSortLess(BYTE** first, BYTE** last, L less)
{
    if ((int)(((BYTE*)last - (BYTE*)first) & ~3) < 60) {
        InsertionLess(first, last, less);
        return;
    }
    BYTE** mid = first + (last - first) / 2;
    StableSortLess(first, mid, less);
    StableSortLess(mid, last, less);
    MergeNoBufLess(first, mid, last, mid - first, last - mid, less);
}

template <class L> static BYTE** MergeRunsLess(BYTE** a, BYTE** ae, BYTE** b, BYTE** be, BYTE** out, L less)
{
    while (a != ae && b != be)
        *out++ = less(b, a) ? *b++ : *a++;
    while (a != ae)
        *out++ = *a++;
    while (b != be)
        *out++ = *b++;
    return out;
}

template <class L> static void MergeLoopLess(BYTE** first, BYTE** last, BYTE** out, int step, L less)
{
    for (; last - first >= step * 2; first += step * 2)
        out = MergeRunsLess(first, first + step, first + step, first + step * 2, out, less);
    int s = step < last - first ? step : last - first;
    MergeRunsLess(first, first + s, first + s, last, out, less);
}

extern "C" void __cdecl Near_Insertion(BYTE** f, BYTE** l, int ctx) { NearLess k = {ctx}; InsertionLess(f, l, k); }
extern "C" void __cdecl Near_StableSort(BYTE** f, BYTE** l, int ctx) { NearLess k = {ctx}; StableSortLess(f, l, k); }
extern "C" void __cdecl Near_Merge(BYTE** f, BYTE** m, BYTE** l, int n1, int n2, int ctx)
{
    NearLess k = {ctx};
    MergeNoBufLess(f, m, l, n1, n2, k);
}
extern "C" void __cdecl Near_MergeAdaptive(BYTE** f, BYTE** m, BYTE** l, int n1, int n2, BYTE**, int, int ctx)
{
    NearLess k = {ctx};
    MergeNoBufLess(f, m, l, n1, n2, k);
}
extern "C" void __cdecl Near_ChunkInsertion(BYTE** f, BYTE** l, int chunk, int ctx)
{
    NearLess k = {ctx};
    for (; l - f >= chunk; f += chunk)
        InsertionLess(f, f + chunk, k);
    InsertionLess(f, l, k);
}
extern "C" void __cdecl Near_MergeLoop(BYTE** f, BYTE** l, BYTE** out, int step, int ctx)
{
    NearLess k = {ctx};
    MergeLoopLess(f, l, out, step, k);
}

// ---------------------------------------------------------------------------
// heaps and insertion of objects by the distance of their road position
// (+40) from a point; the order of the sums is the original's

extern "C" DVec3* __cdecl Road_PointOf(DVec3* out, PositionId p);   // aimain.cpp

static double DistFrom(BYTE* obj, double x, double y, double z)
{
    DVec3 a;
    Road_PointOf(&a, *(PositionId*)(obj + 40));
    double dx = x - a.x;
    double dy = y - a.y;
    double dz = z - a.z;
    return x87_sqrt(dy * dy + dz * dz + dx * dx);
}

// push_heap: value moves up while it is farther than its parent
extern "C" void __cdecl Far_PushHeap(BYTE** first, int hole, int top, BYTE* value, double x, double y, double z)
{
    for (int parent = (hole - 1) / 2; hole > top; parent = (hole - 1) / 2) {
        double dp = DistFrom(first[parent], x, y, z);
        double dv = DistFrom(value, x, y, z);
        if (!(dv > dp))
            break;
        first[hole] = first[parent];
        hole = parent;
    }
    first[hole] = value;
}

// unguarded linear insert, nearest first
extern "C" void __cdecl Near_LinearInsert(BYTE** last, BYTE* value, double x, double y, double z)
{
    for (BYTE** prev = last - 1;; prev--) {
        double dv = DistFrom(value, x, y, z);
        double dp = DistFrom(*prev, x, y, z);
        if (!(dp > dv))
            break;
        *last = *prev;
        last = prev;
    }
    *last = value;
}

extern "C" double __cdecl DVec3_Length(const DVec3* v);   // aimain.cpp

static double SqrtYZX(double dx, double dy, double dz) { return x87_sqrt(dy * dy + dz * dz + dx * dx); }
static double SqrtXYZ(double dx, double dy, double dz) { return x87_sqrt(dx * dx + dy * dy + dz * dz); }

static DVec3 PosOf(BYTE* obj)
{
    DVec3 a;
    Road_PointOf(&a, *(PositionId*)(obj + 40));
    return a;
}

// push_heap (the other instance): the parent's sum y, z, x; the value's x, y, z
extern "C" void __cdecl Far_PushHeap2(BYTE** first, int hole, int top, BYTE* value, double x, double y, double z)
{
    for (int parent = (hole - 1) / 2; hole > top; parent = (hole - 1) / 2) {
        DVec3 a = PosOf(first[parent]);
        DVec3 b = PosOf(value);
        double dp = SqrtYZX(x - a.x, y - a.y, z - a.z);
        double dv = SqrtXYZ(x - b.x, y - b.y, z - b.z);
        if (!(dv > dp))
            break;
        first[hole] = first[parent];
        hole = parent;
    }
    first[hole] = value;
}

// adjust_heap: the hole moves down to the farther child, then value is pushed
extern "C" void __cdecl Far_AdjustHeap(BYTE** first, int hole, int len, BYTE* value, double x, double y, double z)
{
    int top = hole;
    int child = hole * 2 + 2;
    for (; child < len; child = child * 2 + 2) {
        DVec3 a = PosOf(first[child]);
        double sr = SqrtYZX(x - a.x, y - a.y, z - a.z);
        DVec3 b = PosOf(first[child - 1]);
        double sl = SqrtXYZ(x - b.x, y - b.y, z - b.z);
        if (!(sr >= sl))
            child--;
        first[hole] = first[child];
        hole = child;
    }
    if (child == len) {
        first[hole] = first[child - 1];
        hole = child - 1;
    }
    Far_PushHeap(first, hole, top, value, x, y, z);
}

extern "C" void __cdecl Far_AdjustHeap2(BYTE** first, int hole, int len, BYTE* value, double x, double y, double z)
{
    int top = hole;
    int child = hole * 2 + 2;
    for (; child < len; child = child * 2 + 2) {
        DVec3 a = PosOf(first[child]);
        double sr = SqrtXYZ(x - a.x, y - a.y, z - a.z);
        DVec3 b = PosOf(first[child - 1]);
        double sl = SqrtXYZ(x - b.x, y - b.y, z - b.z);
        if (!(sr >= sl))
            child--;
        first[hole] = first[child];
        hole = child;
    }
    if (child == len) {
        first[hole] = first[child - 1];
        hole = child - 1;
    }
    Far_PushHeap2(first, hole, top, value, x, y, z);
}

// make_heap of records whose object (the first dword) has its position at
// +56, by the distance from a point, farthest first
extern "C" void __cdecl Far_MakeHeap56(BYTE** first, BYTE** last, double x, double y, double z)
{
    if ((int)(((BYTE*)last - (BYTE*)first) & ~3) < 8)
        return;
    int len = last - first;
    for (int p = (len - 2) / 2;; p--) {
        BYTE* value = first[p];
        int hole = p;
        int child = p * 2 + 2;
        for (; child < len; child = child * 2 + 2) {
            DVec3* a = (DVec3*)(*(BYTE**)first[child] + 56);
            DVec3* b = (DVec3*)(*(BYTE**)first[child - 1] + 56);
            double sa = SqrtYZX(a->x - x, a->y - y, a->z - z);
            double sb = SqrtYZX(b->x - x, b->y - y, b->z - z);
            if (sb > sa)
                child--;
            first[hole] = first[child];
            hole = child;
        }
        if (child == len) {
            first[hole] = first[child - 1];
            hole = child - 1;
        }
        for (int parent = (hole - 1) / 2; hole > p; parent = (hole - 1) / 2) {
            DVec3* q = (DVec3*)(*(BYTE**)first[parent] + 56);
            double sp = SqrtXYZ(q->x - x, q->y - y, q->z - z);
            DVec3* v = (DVec3*)(*(BYTE**)value + 56);
            DVec3 d = {v->x - x, v->y - y, v->z - z};
            if (!(DVec3_Length(&d) > sp))
                break;
            first[hole] = first[parent];
            hole = parent;
        }
        first[hole] = value;
        if (p == 0)
            return;
    }
}

// insertion sort of doubles, largest first: v goes to the front when it is
// above the first, else back while it is above its predecessor
static void InsertionDesc8(double* first, double* last)
{
    if (first == last)
        return;
    for (double* i = first + 1; i != last; i++) {
        Bits8 v = *(Bits8*)i;
        if (*(double*)&v > *first) {
            CopyBack8(first, i, i + 1);
            *(Bits8*)first = v;
            continue;
        }
        double* j = i;
        for (; *(double*)&v > j[-1]; j--)
            *(Bits8*)j = *(Bits8*)(j - 1);
        *(Bits8*)j = v;
    }
}

extern "C" void __cdecl Doubles_ChunkInsertion(double* first, double* last, int chunk)
{
    for (; last - first >= chunk; first += chunk)
        InsertionDesc8(first, first + chunk);
    InsertionDesc8(first, last);
}

static void Swap8(double* a, double* b)
{
    Bits8 t = *(Bits8*)a;
    *(Bits8*)a = *(Bits8*)b;
    *(Bits8*)b = t;
}

// merge without a buffer; the bounds are found inline
extern "C" void __cdecl Doubles_MergeNoBuf(double* first, double* middle, double* last, int len1, int len2, int pred)
{
    while (len1 != 0 && len2 != 0) {
        if (len1 + len2 == 2) {
            if (*middle > *first)
                Swap8(first, middle);
            return;
        }
        double *cut1, *cut2;
        int len11, len22;
        if (len1 > len2) {
            len11 = len1 / 2;
            cut1 = first + len11;
            cut2 = middle;
            for (int n = last - middle; n > 0;) {
                int h = n >> 1;
                if (*cut1 < cut2[h]) {
                    cut2 += h + 1;
                    n -= h + 1;
                } else
                    n = h;
            }
            len22 = cut2 - middle;
        } else {
            len22 = len2 / 2;
            cut2 = middle + len22;
            cut1 = first;
            for (int n = middle - first; n > 0;) {
                int h = n >> 1;
                if (!(*cut2 > cut1[h])) {
                    cut1 += h + 1;
                    n -= h + 1;
                } else
                    n = h;
            }
            len11 = cut1 - first;
        }
        double* mid = (double*)Rotate8((BYTE*)cut1, (BYTE*)middle, (BYTE*)cut2);
        Doubles_MergeNoBuf(first, cut1, mid, len11, len22, pred);
        first = mid;
        middle = cut2;
        len1 -= len11;
        len2 -= len22;
    }
}

// in-place stable sort of doubles, largest first
extern "C" void __cdecl Doubles_StableSort(double* first, double* last, int pred)
{
    if ((int)(((BYTE*)last - (BYTE*)first) & ~7) < 120) {
        InsertionDesc8(first, last);
        return;
    }
    double* mid = first + (last - first) / 2;
    Doubles_StableSort(first, mid, pred);
    Doubles_StableSort(mid, last, pred);
    int len1 = mid - first, len2 = last - mid;
    if (len1 == 0 || len2 == 0)
        return;
    if (len1 + len2 == 2) {
        if (*mid > *first)
            Swap8(first, mid);
        return;
    }
    double *cut1, *cut2;
    int len11, len22;
    if (len1 > len2) {
        len11 = len1 / 2;
        cut1 = first + len11;
        cut2 = CallC<double*>(FUN_47f7a0, mid, last, cut1, pred, 0);
        len22 = cut2 - mid;
    } else {
        len22 = len2 / 2;
        cut2 = mid + len22;
        cut1 = CallC<double*>(FUN_47f7f0, first, mid, cut2, pred, 0);
        len11 = cut1 - first;
    }
    double* m = (double*)Rotate8((BYTE*)cut1, (BYTE*)mid, (BYTE*)cut2);
    Doubles_MergeNoBuf(first, cut1, m, len11, len22, pred);
    Doubles_MergeNoBuf(m, cut2, last, len1 - len11, len2 - len22, pred);
}

// ---------------------------------------------------------------------------
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // pool free (p, bytes)

// [FUN_459df0] frees the buffer (+16, size +0 + 1) and, when owned (+24), deletes the object at +20
extern "C" void __fastcall Buf459df0_Destruct(BYTE* self, void*)
{
    BYTE* p = F(BYTE*, self, 16);
    if (p && F(int, self, 0) + 1)
        CallC(FUN_44b4a0, p, F(int, self, 0) + 1);
    if (F(BYTE, self, 24) && F(BYTE*, self, 20))
        CallVirt(F(BYTE*, self, 20), 0, 1);
}


// ---------------------------------------------------------------------------

// [FUN_46c7d0] points a stream's window (+12 begin, +16 cursor, +20 end) at its current part: in place for kind 0, else up to 15 bytes copied around the cursor into +45 through the part's source (method +4)
extern "C" void __cdecl Stream_SetWindow(BYTE* s)
{
    BYTE* e = F(BYTE**, s, 24)[F(int, s, 40)];
    int base = F(int, s, 8);
    int cur = F(int, s, 0);
    int kind = (signed char)F(BYTE, e, 8);
    if (kind == 0)
    {
        F(int, s, 12) = F(int, e, 20);
        F(int, s, 16) = F(int, e, 20) - base + cur;
        F(int, s, 20) = F(int, s, 12) + F(int, e, 0);
        return;
    }
    if (kind <= 1 || kind > 3)
        return;
    unsigned end = F(int, e, 0) + base;
    BYTE* src = F(BYTE*, e, 20);
    unsigned from = base;
    int len = 15;
    if ((unsigned)(base + 15) <= (unsigned)cur)
    {
        from = cur - 3;
        if (from + 15 > end)
            from = end - 15;
    }
    if (from + 15 > end)
        len = end - from;
    CallVirt(src, 4, from - base, len, s + 45);
    F(BYTE*, s, 12) = s + 45;
    F(BYTE*, s, 16) = s + 45 + (cur - from);
    F(BYTE*, s, 20) = s + 45 + len;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.
typedef char AsmLabel[1];

ASM_PROC(FUN_459e90)
ASM_VAR(AsmLabel, g_aicon64b5a8, $L_64b5a8)

static BYTE* Aicon_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_45a060]
extern "C" BYTE* __fastcall Aicon45a060_DeletingDestructor(BYTE* s, void*, int flags) { return Aicon_DeleteAfter(s, flags, FUN_459e90); }
// [$L_45a080]
extern "C" BYTE* __fastcall Aicon45a080_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_aicon64b5a8;
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

// [$L_459e20] reads n bytes at pos to dst: from memory (kind 0 of +8 of +32) or the stream (kinds 2, 3)
extern "C" int __fastcall Stream_ReadAt(BYTE* s, void*, int pos, int n, BYTE* dst)
{
    BYTE* d = F(BYTE*, s, 32);
    signed char kind = F(signed char, d, 8);
    if (kind == 0) {
        BYTE* src = F(BYTE*, d, 20) + F(int, s, 36) + pos;
        for (int k = 0; k < n; k++)
            dst[k] = src[k];
        return 0;
    }
    if (kind > 1 && kind <= 3)
        return CallVirt<int>(F(BYTE*, d, 20), 4, F(int, s, 36) + pos, n, dst);
    return 0;
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
// returns the storage of a vector (+0 .. +8) of size-byte elements to the pool
static void PoolVec_Free_C(BYTE* s, int size)
{
    BYTE* p = F(BYTE*, s, 0);
    int n = (F(BYTE*, s, 8) - p) / size;
    if (n)
        CallC(FUN_44b4a0, p, n * size);
}
// [$L_45a0a0] frees the storage of a vector of 1592-byte records
extern "C" void __fastcall Vec1592_FreeStorage(BYTE* s, void*)
{
    PoolVec_Free_C(s, 1592);
}

extern "C" void FUN_460d10(); KING_ALIAS("_FUN_460d10", "_MsgBuf_FreeThunk")
extern "C" void FUN_457b40(); KING_ALIAS("_FUN_457b40", "_AI_Free")
struct Order144 { BYTE b[144]; };
// [FUN_460840] the order in [first, last) with the id (+0) and kind (+12) of order (by value; its parts vector +120
// is freed here); last when none
extern "C" BYTE* __cdecl Orders_Find(BYTE* first, BYTE* last, Order144 order, int)
{
    BYTE* p = first;
    for (; p != last; p += 144)
        if (F(int, p, 0) == F(int, &order, 0) && F(int, p, 12) == F(int, &order, 12))
            break;
    BYTE* b = F(BYTE*, &order, 120);
    for (BYTE* e = b; e != F(BYTE*, &order, 124); e += 16)
        CallC(FUN_460d10, e);
    int n = (F(BYTE*, &order, 128) - b) >> 4;
    if (n) {
        unsigned bytes = n << 4;
        if (bytes > 128)
            CallC(FUN_457b40, b, bytes);
        else
            CallC(FUN_44b4a0, b, bytes);
    }
    return p;
}
