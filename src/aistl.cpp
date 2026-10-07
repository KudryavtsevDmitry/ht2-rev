// aistl.cpp - the STL template instances the AI's code generated (the unit
// at 0x458bc0 of king.masm, after Ai_main.cpp): insertion sorts, medians and
// partitions for its sorts, list and vector members, finds, copies.
//
// Building with  nmake ASM_AISTL=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "road.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" void __cdecl List_FreeNode(void* node, int size);   // aimain.cpp
extern "C" int __cdecl _stricmp(const char* a, const char* b);
extern "C" void __cdecl Sort_LinearInsert12(BYTE* first, BYTE* last, int, void* cmp);
KING_ALIAS("_Sort_LinearInsert12", "FUN_467820")
extern "C" void __cdecl Sort_UnguardedInsertion12(BYTE* first, BYTE* last, int, void* cmp);
KING_ALIAS("_Sort_UnguardedInsertion12", "FUN_4679d0")
ASM_PROC(FUN_45b570)    // vector<1592 bytes>::insert_aux
ASM_PROC(FUN_45cb00)    // vector<4 bytes>::insert_aux
ASM_PROC(FUN_45d8f0)
ASM_PROC(FUN_4604b0)
ASM_PROC(FUN_405870)    // a list value's destructor
ASM_PROC(FUN_602ca0)    // a 16-byte record's destructor
ASM_PROC(FUN_41ce50)

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
ASM_PROC(FUN_40cc50)    // a 24-byte record's destructor
extern "C" void* __cdecl Stl_Allocate(unsigned bytes);
KING_ALIAS("_Stl_Allocate", "FUN_44b740")

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
