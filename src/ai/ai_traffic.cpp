// ai_traffic.cpp - ai_mult.cpp of the game, rewritten from king.masm: the STL
// instances it uses (heaps of pointers keyed by an int field, of unsigned
// ints and of 32-byte entries keyed by a double, binary searches, copies,
// swaps, vector<12 bytes>::erase), the deleting destructors of its value
// holders, and a few accessors. The heap functions that copy comparators by
// value stay in king.masm.
//
// Building with  nmake ASM_AI_TRAFFIC=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" void FUN_46cbd0(); KING_ALIAS("_FUN_46cbd0", "_PopHeapLast12")
extern "C" void FUN_44dd90(); KING_ALIAS("_FUN_44dd90", "@VecChar_Copy@12")
extern "C" void FUN_46b460(); KING_ALIAS("_FUN_46b460", "_Names_AdjustHeap")
extern "C" void FUN_46b740(); KING_ALIAS("_FUN_46b740", "_Far_AdjustHeap")
extern "C" void FUN_458a40(); KING_ALIAS("_FUN_458a40", "@Rec_Copy8Skip1@12")
extern "C" void FUN_46bf20(); KING_ALIAS("_FUN_46bf20", "_Heap32_Adjust")
extern "C" void FUN_46d0b0(); KING_ALIAS("_FUN_46d0b0", "_Far_AdjustHeap2")
extern "C" void FUN_46c190(); KING_ALIAS("_FUN_46c190", "_Heap504_Adjust")
ASM_PROC(FUN_46ad50)
extern "C" void FUN_46cc00(); KING_ALIAS("_FUN_46cc00", "_Mult46cc00")
ASM_PROC(FUN_4870e0)
extern "C" void FUN_529880(); KING_ALIAS("_FUN_529880", "_Carat_MarkKey")
extern "C" void FUN_447ef0(); KING_ALIAS("_FUN_447ef0", "@Vec12_FreeStorage@8")
extern "C" double __cdecl Players_NearestTo(const int* const* node, const BYTE** list);   // aimain.cpp
extern "C" void FUN_40c1a0(); KING_ALIAS("_FUN_40c1a0", "@ValueBuf_Destroy@8")
extern "C" void FUN_40cc50(); KING_ALIAS("_FUN_40cc50", "@Rec40cc50_Destroy@8")
extern "C" void FUN_40cb60(); KING_ALIAS("_FUN_40cb60", "@Rec40cb60_Destroy@8")
extern "C" void FUN_4a36c0(); KING_ALIAS("_FUN_4a36c0", "@SchHandle_Release@8")
extern "C" void FUN_50d190(); KING_ALIAS("_FUN_50d190", "@Shell_Nop@8")

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

// --- heap algorithm wrappers. The element value and the comparator are
// passed by value, as the STL of the time did; the callees destroy them.

template <int N> struct Blob { DWORD d[N]; };
typedef Blob<3> Val12;
typedef Blob<8> Val32;
typedef Blob<6> Pred24;

static Val12 Copy12(const BYTE* p)
{
    Val12 v;
    CallThis(FUN_44dd90, &v, p);
    return v;
}

static Val32 Copy32(const BYTE* p)
{
    Val32 v;
    CallThis(FUN_458a40, &v, p);
    return v;
}

extern "C" {

// [FUN_46b3f0] make_heap of 12-byte records
void __cdecl MakeHeap12(BYTE* first, BYTE* last, DWORD pred)
{
    int n = (last - first) / 12;
    if (n < 2)
        return;
    for (int parent = (n - 2) / 2;; parent--) {
        CallC(FUN_46b460, first, parent, n, Copy12(first + parent * 12), pred);
        if (!parent)
            break;
    }
}

// [FUN_46bec0] make_heap of 32-byte records
void __cdecl MakeHeap32(BYTE* first, BYTE* last, DWORD pred)
{
    if (((last - first) & ~31) < 64)
        return;
    int n = (last - first) >> 5;
    for (int parent = (n - 2) / 2;; parent--) {
        CallC(FUN_46bf20, first, parent, n, Copy32(first + parent * 32), pred);
        if (!parent)
            break;
    }
}

// [FUN_46c130] make_heap of 4-byte elements
void __cdecl MakeHeap4(DWORD* first, DWORD* last, DWORD pred)
{
    if ((((BYTE*)last - (BYTE*)first) & ~3) < 8)
        return;
    int n = last - first;
    for (int parent = (n - 2) / 2;; parent--) {
        CallC(FUN_46c190, first, parent, n, first[parent], pred);
        if (!parent)
            break;
    }
}

static void MakeHeap4P(void (*adjust)(), DWORD* first, DWORD* last, Pred24 pred)
{
    if ((((BYTE*)last - (BYTE*)first) & ~3) < 8)
        return;
    int n = last - first;
    for (int parent = (n - 2) / 2;; parent--) {
        CallC(adjust, first, parent, n, first[parent], pred);
        if (!parent)
            break;
    }
}

// [FUN_46b6d0] make_heap of 4-byte elements with a 24-byte comparator
void __cdecl MakeHeap4A(DWORD* first, DWORD* last, Pred24 pred)
{
    MakeHeap4P(FUN_46b740, first, last, pred);
}

// [FUN_46d040]
void __cdecl MakeHeap4B(DWORD* first, DWORD* last, Pred24 pred)
{
    MakeHeap4P(FUN_46d0b0, first, last, pred);
}

// [FUN_46bfc0] pop_heap step: *result = *first, then the value is sifted in
void __cdecl PopHeap4(DWORD* first, DWORD* last, DWORD* result, DWORD value, Pred24 pred)
{
    *result = *first;
    CallC(FUN_46d0b0, first, 0, (int)(last - first), value, pred);
}

// [FUN_46cff0]
void __cdecl PopHeap32(BYTE* first, BYTE* last, BYTE* result, Val32 value, DWORD pred)
{
    memcpy(result, first, 32);
    CallC(FUN_46bf20, first, 0, (int)((last - first) >> 5), Copy32((const BYTE*)&value), pred);
}

// [FUN_46c210] pop_heap of 4-byte elements
void __cdecl PopHeapLast4(DWORD* first, DWORD* last, DWORD pred)
{
    DWORD* r = last - 1;
    CallC(FUN_46ad50, first, r, r, *r, pred, 0);
}

// [FUN_46d240]
void __cdecl PopHeapLast4P(DWORD* first, DWORD* last, Pred24 pred)
{
    DWORD* r = last - 1;
    PopHeap4(first, r, r, *r, pred);
}

// [FUN_46cfc0] pop_heap of 32-byte records
void __cdecl PopHeapLast32(BYTE* first, BYTE* last, int unused, DWORD pred)
{
    BYTE* r = last - 32;
    PopHeap32(first, r, r, Copy32(r), pred);
}

// [FUN_46cbd0] pop_heap of 12-byte records
void __cdecl PopHeapLast12(BYTE* first, BYTE* last, int unused, DWORD pred)
{
    BYTE* r = last - 12;
    CallC(FUN_46cc00, first, r, r, Copy12(r), pred, 0);
}

// [FUN_487230]
int __cdecl Route_Call4870e0(int a, int b)
{
    DWORD local[2] = {0, 0};
    return CallC<int>(FUN_4870e0, a, b, local);
}

// [FUN_487270]
int __cdecl Route_Call487230(int a, int b)
{
    return Route_Call4870e0(a, b);
}

// [FUN_483ff0]
int __cdecl Route_Call529880(int a, int b)
{
    return CallC<int>(FUN_529880, a, b);
}

// [FUN_487ce0] constructs a byte
void __cdecl Byte_Construct(BYTE* p, const BYTE* v)
{
    if (p)
        *p = *v;
}

// [FUN_487d00] scalar deleting destructor
BYTE* __fastcall Rec487d00_Delete(BYTE* self, void*, int flags)
{
    CallThis(FUN_447ef0, self);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [FUN_487d20] n zeroed 8-byte slots
DWORD* __fastcall Slots8_Construct(DWORD* self, void*, int n)
{
    self[0] = (DWORD)operator new(n * 8);
    self[1] = n;
    self[2] = 0;
    for (int i = 0; i < (int)self[1]; i++) {
        ((DWORD*)self[0])[i * 2] = 0;
        ((DWORD*)self[0])[i * 2 + 1] = 0;
    }
    return self;
}

// [FUN_4867e0] a waypoint (a, b) with no kind
DWORD* __fastcall Waypoint_Make(DWORD* self, void*, DWORD a, DWORD b)
{
    self[0] = a;
    self[1] = b;
    F(WORD, self, 8) = 0;
    F(BYTE, self, 10) = 255;
    return self;
}

}

extern "C" void* __cdecl Stl_Allocate(unsigned bytes);
extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")

extern "C" {

// [FUN_46e200] n 12-byte records from the allocator, filled from [first, last)
BYTE* __stdcall Alloc12_Copy(unsigned n, BYTE* first, BYTE* last)
{
    BYTE* p = n ? (BYTE*)Stl_Allocate(n * 12) : 0;
    CallC(FUN_47e3e0, first, last, p, n);
    return p;
}

}

extern "C" void FUN_458cf0(); KING_ALIAS("_FUN_458cf0", "_Bytes_AllocCopy@12")    // allocate n bytes and copy [first, last) (stdcall)
extern "C" void FUN_44dd90(); KING_ALIAS("_FUN_44dd90", "@VecChar_Copy@12")    // vector copy constructor
extern "C" void FUN_46b460(); KING_ALIAS("_FUN_46b460", "_Names_AdjustHeap")
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
extern "C" void* __cdecl memmove(void* d, const void* s, size_t n);

struct ByteVec { BYTE* begin; BYTE* end; BYTE* cap; };

extern "C" {

// [FUN_46cc00] assigns the bytes of *a to *b, then runs FUN_46b460 over the
// (last - a) / 12 records at a with a copy of v (by value) and x
void __cdecl Mult46cc00(ByteVec* a, BYTE* last, ByteVec* b, ByteVec v, int x)
{
    if (a != b) {
        BYTE* f = a->begin;
        BYTE* l = a->end;
        unsigned n = l - f;
        if (n > (unsigned)(b->cap - b->begin)) {
            BYTE* p = CallThis<BYTE*>(FUN_458cf0, b, n, f, l);
            if (b->cap - b->begin)
                CallC(FUN_44b4a0, b->begin, b->cap - b->begin);
            b->begin = p;
            b->cap = p + n;
        } else if ((unsigned)(b->end - b->begin) >= n) {
            memmove(b->begin, f, l - f);
        } else {
            unsigned have = b->end - b->begin;
            memmove(b->begin, f, have);
            memmove(b->end, a->begin + (b->end - b->begin), a->end - (a->begin + (b->end - b->begin)));
        }
        b->end = b->begin + n;
    }
    ByteVec c;
    CallThis(FUN_44dd90, &c, &v);
    CallC(FUN_46b460, a, 0, (last - (BYTE*)a) / 12, c, x);
    unsigned m = v.cap - v.begin;
    if (m)
        CallC(FUN_44b4a0, v.begin, m);
}

}

extern "C" void FUN_4072f0(); KING_ALIAS("_FUN_4072f0", "_Copy1")    // copy of bytes (first, last, to)
extern "C" int __cdecl _stricmp(const char* a, const char* b);

// vector<char> assignment *d = [f, l); copy moves the bytes of the part
// that fits the old size
static void ByteVec_Assign(ByteVec* d, BYTE* f, BYTE* l, bool useCopy)
{
    unsigned n = l - f;
    if (n > (unsigned)(d->cap - d->begin)) {
        BYTE* p = CallThis<BYTE*>(FUN_458cf0, d, n, f, l);
        if (d->cap - d->begin)
            CallC(FUN_44b4a0, d->begin, d->cap - d->begin);
        d->begin = p;
        d->cap = p + n;
    } else if ((unsigned)(d->end - d->begin) >= n) {
        memmove(d->begin, f, n);
    } else {
        unsigned have = d->end - d->begin;
        if (useCopy) {
            CallC(FUN_4072f0, f, f + have, d->begin);
            CallC(FUN_4072f0, f + (d->end - d->begin), l, d->end);
        } else {
            memmove(d->begin, f, have);
            memmove(d->end, f + (d->end - d->begin), l - (f + (d->end - d->begin)));
        }
    }
    d->end = d->begin + n;
}

extern "C" {

// [FUN_46ca00] push_heap step for names (vector<char>, compared ignoring
// case): moves v up from hole towards top
void __cdecl Names_PushHeap(ByteVec* first, int hole, int top, ByteVec v)
{
    int parent = (hole - 1) / 2;
    while (hole > top && _stricmp((const char*)first[parent].begin, (const char*)v.begin) < 0) {
        ByteVec* d = &first[hole];
        ByteVec* s = &first[parent];
        if (s != d)
            ByteVec_Assign(d, s->begin, s->end, false);
        hole = parent;
        parent = (hole - 1) / 2;
    }
    ByteVec* d = &first[hole];
    if (d != &v)
        ByteVec_Assign(d, v.begin, v.end, true);
    if (v.cap - v.begin)
        CallC(FUN_44b4a0, v.begin, v.cap - v.begin);
}

}

extern "C" void FUN_46ca00(); KING_ALIAS("_FUN_46ca00", "_Names_PushHeap")    // names: push heap (first, hole, top, value by value)
extern "C" void FUN_483a50(); KING_ALIAS("_FUN_483a50", "_AI_483a50")    // the network message being built
extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")    // player of a list node
extern "C" void FUN_4758d0(); KING_ALIAS("_FUN_4758d0", "@Player_SaveX@12")    // player: serialize into a byte vector
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")    // message: add field (id, data, size)
ASM_VAR(BYTE, g_68d31c, $L_68d31c)
ASM_VAR(BYTE, g_66f62b, $L_66f62b)
ASM_VAR(BYTE*, g_playersHead68b990, $L_68b990)

extern "C" {

// [FUN_46b460] adjust_heap for names: sinks the hole to a leaf, then pushes
// v back up
void __cdecl Names_AdjustHeap(ByteVec* first, int hole, int len, ByteVec v, int cmp)
{
    int top = hole;
    int child = 2 * hole + 2;
    while (child < len) {
        if (_stricmp((const char*)first[child].begin, (const char*)first[child - 1].begin) < 0)
            child--;
        if (&first[child] != &first[hole])
            ByteVec_Assign(&first[hole], first[child].begin, first[child].end, false);
        hole = child;
        child = 2 * child + 2;
    }
    if (child == len) {
        if (&first[child - 1] != &first[hole])
            ByteVec_Assign(&first[hole], first[child - 1].begin, first[child - 1].end, true);
        hole = child - 1;
    }
    ByteVec c;
    CallThis(FUN_44dd90, &c, &v);
    CallC(FUN_46ca00, first, hole, top, c, cmp);
    if (v.cap - v.begin)
        CallC(FUN_44b4a0, v.begin, v.cap - v.begin);
}

// [FUN_46dee0] in a network game, adds every player's state (message field
// 992) to the message being built
int __cdecl Net_AddPlayerStates()
{
    if (!g_68d31c || !g_66f62b)
        return 0;
    BYTE* msg = CallC<BYTE*>(FUN_483a50);
    for (BYTE* n = *(BYTE**)g_playersHead68b990; n != g_playersHead68b990; n = *(BYTE**)n) {
        BYTE* p = CallC<BYTE*>(FUN_40a9f0, n);
        if (F(DWORD, p, 88) & 0x100)
            continue;
        ByteVec buf = {0, 0, 0};
        CallThis(FUN_4758d0, CallC<BYTE*>(FUN_40a9f0, n), &buf);
        CallThis(FUN_603110, msg, 992, buf.begin, buf.end - buf.begin);
        if (buf.cap - buf.begin)
            CallC(FUN_44b4a0, buf.begin, buf.cap - buf.begin);
    }
    return 0;
}

}


// [FUN_46c190] sifts a hole down a heap of players keyed by +504 (ties go right) and pushes val back up
extern "C" void __cdecl Heap504_Adjust(BYTE** a, int hole, int len, int val, int extra)
{
    int top = hole;
    int child = 2 * hole + 2;
    while (child < len) {
        double dd = F(double, a[child], 504) - F(double, a[child - 1], 504);
        if (!(dd == 0.0 || dd != dd))
            child--;
        a[hole] = a[child];
        hole = child;
        child = 2 * child + 2;
    }
    if (child == len) {
        a[hole] = a[child - 1];
        hole = child - 1;
    }
    Heap504_Push(a, hole, top, (BYTE*)val);
}

extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")
ASM_VAR(char, g_ctor68d308, $L_68d308)

// [FUN_46d510] constructs the global container at 68d308
extern "C" void __cdecl Traffic_Init68d308()
{
    char alloc;
    CallThis(FUN_487730, &g_ctor68d308, &alloc);
}

// ---------------------------------------------------------------------------
// Static initializers of random generators.
typedef char AsmLabel[1];

ASM_VAR(AsmLabel, g_aitraRng68d220, $L_68d220)
ASM_VAR(DWORD, g_aitraRng68d2f8, $L_68d2f8)
ASM_VAR(DWORD, g_aitraRng68d2fc, $L_68d2fc)
ASM_VAR(DWORD, g_aitraRng68d300, $L_68d300)
ASM_VAR(AsmLabel, g_aitraRng68d7d8, $L_68d7d8)
ASM_VAR(DWORD, g_aitraRng68d8b0, $L_68d8b0)
ASM_VAR(DWORD, g_aitraRng68d8b4, $L_68d8b4)
ASM_VAR(DWORD, g_aitraRng68d8b8, $L_68d8b8)

// the subtractive random generator's start (Knuth): 55 words at a, then the seed and the two indices
static void Aitra_RngInit(DWORD* a, DWORD* seed, DWORD* i1, DWORD* i2)
{
    for (int i = 0; i < 55; i++)
        a[i] = 0;
    DWORD mj = 161803398, mk = 1;
    *seed = mj;
    for (unsigned i = 0; i < 1134; i += 21) {
        unsigned d = i % 55;
        mj -= mk;
        a[d] = mk;
        DWORD t = mj;
        mj = a[d];
        mk = t;
    }
    for (int k = 0; k < 4; k++)
        for (unsigned j = 0; j < 55; j++)
            a[j] -= a[(j + 31) % 55];
    *i1 = 0;
    *i2 = 31;
}

// [$L_46d450]
extern "C" void __cdecl AitraInit_46d450() { Aitra_RngInit((DWORD*)g_aitraRng68d220, &g_aitraRng68d2f8, &g_aitraRng68d2fc, &g_aitraRng68d300); }
// [$L_484090]
extern "C" void __cdecl AitraInit_484090() { Aitra_RngInit((DWORD*)g_aitraRng68d7d8, &g_aitraRng68d8b0, &g_aitraRng68d8b4, &g_aitraRng68d8b8); }

// ---------------------------------------------------------------------------
// Destructor thunks: the vtable, then the base destructor; static object cleanups.

extern "C" void FUN_40cc50(); KING_ALIAS("_FUN_40cc50", "@Rec40cc50_Destroy@8")
// [FUN_46c490] destroys the record p
extern "C" int __cdecl Rec_DestroyThunk46c490(BYTE* p)
{
    return CallThis<int>(FUN_40cc50, p);
}

// ---------------------------------------------------------------------------
// Small methods: table constructors and clears, list teardown, cursor handling, flag maps.

// [$L_487d70] frees the blocks of the (+0, count +4) 8-byte entries, clearing them, and the +8 count
extern "C" void __fastcall Blocks487d70_Clear(BYTE* s, void*)
{
    for (int k = 0; k < F(int, s, 4); k++) {
        void** e = (void**)(F(BYTE*, s, 0) + k * 8);
        if (e[0]) {
            operator delete(e[0]);
            ((void**)(F(BYTE*, s, 0) + k * 8))[0] = 0;
        }
        ((void**)(F(BYTE*, s, 0) + k * 8))[1] = 0;
    }
    F(int, s, 8) = 0;
}

// ---------------------------------------------------------------------------
// Static initializers: construct a global, register its teardown at exit.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
ASM_VAR(char, g_q197_68d308, $L_68d308)
extern "C" void q197_FUN_447ef0(); KING_ALIAS("_q197_FUN_447ef0", "@Vec12_FreeStorage@8")
extern "C" void q197_FUN_46d510(); KING_ALIAS("_q197_FUN_46d510", "_Traffic_Init68d308")
// [$L_46d540] destroys the static object 68d308 at exit (FUN_447ef0)
extern "C" void __cdecl Static68d308_Destroy(void) { CallThis(q197_FUN_447ef0, &g_q197_68d308); }
// [$L_46d530] registers the teardown at exit
extern "C" int __cdecl Static68d308_AtExit(void) { return atexit((void(__cdecl*)(void))Static68d308_Destroy); }
// [$L_46d500] the static initializer: FUN_46d510, then the teardown at exit
extern "C" int __cdecl Static68d308_Init(void)
{
    CallC(q197_FUN_46d510);
    return Static68d308_AtExit();
}

// [FUN_46bf20] adjust_heap of 32-byte entries by the double at +8 (smallest first): the hole at hole moves down to
// a leaf through the smaller children, then v (copied) is pushed back up from there
extern "C" void __cdecl Heap32_Adjust(Entry32* first, int hole, int len, Entry32 v, int)
{
    int top = hole;
    int child = 2 * hole + 2;
    for (; child < len; child = 2 * child + 2) {
        if (first[child].key > first[child - 1].key)
            child--;
        first[hole] = first[child];
        hole = child;
    }
    if (child == len) {
        first[hole] = first[child - 1];
        hole = child - 1;
    }
    Entry32 c;
    CallThis(FUN_458a40, &c, &v);
    Heap32_Push(first, hole, top, c);
}
