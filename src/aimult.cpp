// aimult.cpp - ai_mult.cpp of the game, rewritten from king.masm: the STL
// instances it uses (heaps of pointers keyed by an int field, of unsigned
// ints and of 32-byte entries keyed by a double, binary searches, copies,
// swaps, vector<12 bytes>::erase), the deleting destructors of its value
// holders, and a few accessors. The heap functions that copy comparators by
// value stay in king.masm.
//
// Building with  nmake ASM_AIMULT=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

ASM_PROC(FUN_46cbd0)
extern "C" double __cdecl Players_NearestTo(const int* const* node, const BYTE** list);   // aimain.cpp
ASM_PROC(FUN_40c1a0)
ASM_PROC(FUN_40cc50)
ASM_PROC(FUN_40cb60)
ASM_PROC(FUN_4a36c0)
ASM_PROC(FUN_50d190)

ASM_VAR(BYTE, g_multOn, $L_68d31c)
ASM_VAR(int, g_68d320, $L_68d320)
ASM_VAR(BYTE*, g_68d308, $L_68d308)             // vector<12 bytes>: begin, end
ASM_VAR(BYTE*, g_68d30c, $L_68d30c)

extern "C" DWORD* __cdecl Copy32(const DWORD* first, const DWORD* last, DWORD* to)
{
    for (int n = (int)(last - first); n > 0; n--)
        *to++ = *first++;
    return to;
}

// make_heap of unsigned ints
extern "C" void __cdecl HeapU_Make(DWORD* first, DWORD* last)
{
    if ((int)(((BYTE*)last - (BYTE*)first) & ~3) < 8)
        return;
    int n = (int)(last - first);
    int parent = (n - 2) / 2;
    int child0 = parent * 2 + 2;
    for (;;) {
        DWORD v = first[parent];
        int hole = parent;
        int c = child0;
        while (c < n) {
            if (first[c] < first[c - 1])
                c--;
            first[hole] = first[c];
            hole = c;
            c = c * 2 + 2;
        }
        if (c == n) {
            first[hole] = first[c - 1];
            hole = c - 1;
        }
        for (int p = (hole - 1) / 2; hole > parent && first[p] < v; p = (hole - 1) / 2) {
            first[hole] = first[p];
            hole = p;
        }
        first[hole] = v;
        if (parent == 0)
            return;
        child0 -= 2;
        parent--;
    }
}

extern "C" void __cdecl Mult_46b6b0(int a, int b, int c)
{
    CallC(FUN_46cbd0, a, b, 0, c);
}

// heaps of pointers ordered by the int at +off (largest first)
static void PopHeapKey(BYTE** first, BYTE** last, int off)
{
    BYTE* v = last[-1];
    last[-1] = first[0];
    int n = (int)(last - first) - 1;
    int hole = 0;
    int c = 2;
    while (c < n) {
        if (F(int, first[c], off) < F(int, first[c - 1], off))
            c--;
        first[hole] = first[c];
        hole = c;
        c = c * 2 + 2;
    }
    if (c == n) {
        first[hole] = first[c - 1];
        hole = c - 1;
    }
    for (int p = (hole - 1) / 2; hole > 0 && F(int, first[p], off) < F(int, v, off); p = (hole - 1) / 2) {
        first[hole] = first[p];
        hole = p;
    }
    first[hole] = v;
}

static void AdjustHeapKey(BYTE** first, int hole, int n, BYTE* v, int off)
{
    int top = hole;
    int c = hole * 2 + 2;
    while (c < n) {
        if (F(int, first[c], off) < F(int, first[c - 1], off))
            c--;
        first[hole] = first[c];
        hole = c;
        c = c * 2 + 2;
    }
    if (c == n) {
        first[hole] = first[c - 1];
        hole = c - 1;
    }
    for (int p = (hole - 1) / 2; hole > top && F(int, first[p], off) < F(int, v, off); p = (hole - 1) / 2) {
        first[hole] = first[p];
        hole = p;
    }
    first[hole] = v;
}

extern "C" void __cdecl Heap24_Pop(BYTE** first, BYTE** last) { PopHeapKey(first, last, 24); }
extern "C" void __cdecl Heap328_Pop(BYTE** first, BYTE** last) { PopHeapKey(first, last, 328); }

extern "C" void __cdecl Heap24_Adjust(BYTE** first, int hole, int n, BYTE* v)
{
    AdjustHeapKey(first, hole, n, v, 24);
}

extern "C" void __cdecl Heap328_Adjust(BYTE** first, int hole, int n, BYTE* v)
{
    AdjustHeapKey(first, hole, n, v, 328);
}

extern "C" void __cdecl Heap24_Push(BYTE** first, int hole, int top, BYTE* v)
{
    for (int p = (hole - 1) / 2; hole > top && F(int, first[p], 24) < F(int, v, 24); p = (hole - 1) / 2) {
        first[hole] = first[p];
        hole = p;
    }
    first[hole] = v;
}

// pop_heap of unsigned ints
extern "C" void __cdecl HeapU_Pop(DWORD* first, DWORD* last)
{
    DWORD v = last[-1];
    last[-1] = first[0];
    int n = (int)(last - first) - 1;
    int hole = 0;
    int c = 2;
    while (c < n) {
        if (first[c] < first[c - 1])
            c--;
        first[hole] = first[c];
        hole = c;
        c = c * 2 + 2;
    }
    if (c == n) {
        first[hole] = first[c - 1];
        hole = c - 1;
    }
    for (int p = (hole - 1) / 2; hole > 0 && first[p] < v; p = (hole - 1) / 2) {
        first[hole] = first[p];
        hole = p;
    }
    first[hole] = v;
}

// lower_bound / upper_bound of pointers by the int at +off of *key
static BYTE** Bound(BYTE** first, BYTE** last, BYTE* const* key, int off, bool upper)
{
    int k = F(int, *key, off);
    for (int n = (int)(last - first); n > 0;) {
        int half = n >> 1;
        int v = F(int, first[half], off);
        if (upper ? !(k < v) : v < k) {
            first += half + 1;
            n += -1 - half;
        } else
            n = half;
    }
    return first;
}

extern "C" BYTE** __cdecl Bound28_Lower(BYTE** first, BYTE** last, BYTE* const* key) { return Bound(first, last, key, 28, false); }
extern "C" BYTE** __cdecl Bound28_Upper(BYTE** first, BYTE** last, BYTE* const* key) { return Bound(first, last, key, 28, true); }
extern "C" BYTE** __cdecl Bound32_Lower(BYTE** first, BYTE** last, BYTE* const* key) { return Bound(first, last, key, 32, false); }
extern "C" BYTE** __cdecl Bound32_Upper(BYTE** first, BYTE** last, BYTE* const* key) { return Bound(first, last, key, 32, true); }

// the same by the distance of Players_NearestTo(element, ctx)
extern "C" BYTE** __cdecl BoundD_Lower(BYTE** first, BYTE** last, BYTE** key, int ctx)
{
    for (int n = (int)(last - first); n > 0;) {
        int half = n >> 1;
        BYTE** mid = first + half;
        double m = Players_NearestTo((const int* const*)mid, (const BYTE**)ctx);
        if (Players_NearestTo((const int* const*)key, (const BYTE**)ctx) > m) {
            first = mid + 1;
            n += -1 - half;
        } else
            n = half;
    }
    return first;
}

extern "C" BYTE** __cdecl BoundD_Upper(BYTE** first, BYTE** last, BYTE** key, int ctx)
{
    for (int n = (int)(last - first); n > 0;) {
        int half = n >> 1;
        BYTE** mid = first + half;
        double k = Players_NearestTo((const int* const*)key, (const BYTE**)ctx);
        if (!(Players_NearestTo((const int* const*)mid, (const BYTE**)ctx) > k)) {
            first = mid + 1;
            n += -1 - half;
        } else
            n = half;
    }
    return first;
}

// ---------------------------------------------------------------------------
// Deleting destructors

// a value holder: an object with its reference count at +4, or with flag
// 0x10000 at +8 an immediate value
extern "C" BYTE* __fastcall MultValue_DeletingDestructor(BYTE* self, void*, int flags)
{
    if (!(F(DWORD, self, 8) & 0x10000) && F(BYTE*, self, 0)) {
        F(int, F(BYTE*, self, 0), 4)--;
        BYTE* o = F(BYTE*, self, 0);
        if (F(int, o, 4) == 0 && o)
            CallVirt(o, 12, 1);
    }
    if (flags & 1)
        operator delete(self);
    return self;
}

static BYTE* DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
extern "C" BYTE* __fastcall Mult46c8e0_DeletingDestructor(BYTE* self, void*, int flags) { return DeleteAfter(self, flags, FUN_40c1a0); }
extern "C" BYTE* __fastcall Mult46c900_DeletingDestructor(BYTE* self, void*, int flags) { return DeleteAfter(self, flags, FUN_40cc50); }
extern "C" BYTE* __fastcall Mult46c920_DeletingDestructor(BYTE* self, void*, int flags) { return DeleteAfter(self, flags, FUN_40cb60); }
extern "C" BYTE* __fastcall Mult46c940_DeletingDestructor(BYTE* self, void*, int flags) { return DeleteAfter(self, flags, FUN_4a36c0); }
extern "C" BYTE* __fastcall Mult46c960_DeletingDestructor(BYTE* self, void*, int flags) { return DeleteAfter(self, flags, FUN_50d190); }

// ---------------------------------------------------------------------------

extern "C" void __cdecl Swap32(DWORD* a, DWORD* b)
{
    DWORD t = *a;
    *a = *b;
    *b = t;
}

extern "C" void __cdecl Reverse32(DWORD* first, DWORD* last)
{
    while (first != last && first != --last) {
        DWORD t = *first;
        *first++ = *last;
        *last = t;
    }
}

// push_heap of 32-byte entries by the double at +8 (smallest first); the
// value is passed by value
struct Entry32 {
    DWORD  m_0, m_4;
    double key;
    DWORD  m_16[4];
};

extern "C" void __cdecl Heap32_Push(Entry32* first, int hole, int top, Entry32 v)
{
    for (int p = (hole - 1) / 2; hole > top; p = (hole - 1) / 2) {
        if (!(first[p].key > v.key))
            break;
        first[hole] = first[p];
        hole = p;
    }
    first[hole] = v;
}

// push_heap of pointers stopping at an equal double at +504
extern "C" void __cdecl Heap504_Push(BYTE** first, int hole, int top, BYTE* v)
{
    for (int p = (hole - 1) / 2; hole > top; p = (hole - 1) / 2) {
        BYTE* e = first[p];
        if (x87_eq(F(double, e, 504) - F(double, v, 504), 0.0))
            break;
        first[hole] = e;
        hole = p;
    }
    first[hole] = v;
}

extern "C" BYTE* __fastcall Mult46d550_Construct(BYTE* self, void*, int v)
{
    self[0] = 0;
    self[1] = 0;
    F(int, self, 4) = 0;
    F(int, self, 8) = v;
    return self;
}

extern "C" int __cdecl Mult_46e120()
{
    return g_multOn ? g_68d320 : 0;
}

extern "C" int __cdecl Mult_PlayerCount()
{
    if (g_multOn)
        return (int)(g_68d30c - g_68d308) / 12;
    return 1;
}

// vector<12 bytes>::erase(first, last)
extern "C" BYTE* __fastcall Vec12_Erase(BYTE* self, void*, BYTE* first, BYTE* last)
{
    int n = (int)(F(BYTE*, self, 4) - last) / 12;
    BYTE* to = first;
    for (; n > 0; n--, to += 12) {
        const BYTE* from = to + (last - first);
        F(DWORD, to, 0) = F(DWORD, from, 0);
        F(DWORD, to, 4) = F(DWORD, from, 4);
        F(DWORD, to, 8) = F(DWORD, from, 8);
    }
    F(BYTE*, self, 4) -= (int)(last - first) / 12 * 12;
    return first;
}
