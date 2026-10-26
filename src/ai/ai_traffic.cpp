// ai_traffic.cpp - ai_mult.cpp of the game, rewritten from king.masm: the STL
// instances it uses (heaps of pointers keyed by an int field, of unsigned
// ints and of 32-byte entries keyed by a double, binary searches, copies,
// swaps, vector<12 bytes>::erase), the deleting destructors of its value
// holders, and a few accessors. The heap functions that copy comparators by
// value stay in king.masm.

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
ASM_FUNC(FUN_46ad50, _PopHeapAux4)
extern "C" void FUN_46cc00(); KING_ALIAS("_FUN_46cc00", "_Mult46cc00")
extern "C" void FUN_4870e0(); KING_ALIAS("_FUN_4870e0", "_Player_ReplanToNode")
extern "C" void FUN_529880(); KING_ALIAS("_FUN_529880", "_Carat_MarkKey")
extern "C" void FUN_447ef0(); KING_ALIAS("_FUN_447ef0", "@Vec12_FreeStorage@8")
extern "C" double __cdecl Players_NearestTo(const int* const* node, const BYTE** list);   // aimain.cpp
extern "C" void FUN_40c1a0(); KING_ALIAS("_FUN_40c1a0", "@ValueBuf_Destroy@8")
extern "C" void FUN_40cc50(); KING_ALIAS("_FUN_40cc50", "@Rec40cc50_Destroy@8")
extern "C" void FUN_40cb60(); KING_ALIAS("_FUN_40cb60", "@Rec40cb60_Destroy@8")
extern "C" void FUN_4a36c0(); KING_ALIAS("_FUN_4a36c0", "@SchHandle_Release@8")
extern "C" void FUN_50d190(); KING_ALIAS("_FUN_50d190", "@Shell_Nop@8")

KING_VAR(BYTE, g_multOn, 68d31c)
KING_VAR(int, g_68d320, 68d320)
KING_VAR(BYTE*, g_68d308, 68d308)             // vector<12 bytes>: begin, end
KING_VAR(BYTE*, g_68d30c, 68d30c)

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
KING_VAR(BYTE, g_68d31c, 68d31c)
KING_VAR(BYTE, g_66f62b, 66f62b)
KING_VAR(BYTE*, g_playersHead68b990, 68b990)

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
KING_VAR(char, g_ctor68d308, 68d308)

// [FUN_46d510] constructs the global container at 68d308
extern "C" void __cdecl Traffic_Init68d308()
{
    char alloc;
    CallThis(FUN_487730, &g_ctor68d308, &alloc);
}

// ---------------------------------------------------------------------------
// Static initializers of random generators.
typedef char AsmLabel[1];

KING_VAR(AsmLabel, g_aitraRng68d220, 68d220)
KING_VAR(DWORD, g_aitraRng68d2f8, 68d2f8)
KING_VAR(DWORD, g_aitraRng68d2fc, 68d2fc)
KING_VAR(DWORD, g_aitraRng68d300, 68d300)
KING_VAR(AsmLabel, g_aitraRng68d7d8, 68d7d8)
KING_VAR(DWORD, g_aitraRng68d8b0, 68d8b0)
KING_VAR(DWORD, g_aitraRng68d8b4, 68d8b4)
KING_VAR(DWORD, g_aitraRng68d8b8, 68d8b8)

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
KING_VAR(char, g_q197_68d308, 68d308)
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

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q238_Blocks487d70_Clear(BYTE* s, void* d); KING_ALIAS("@q238_Blocks487d70_Clear@8", "@Blocks487d70_Clear@8")
// [$L_487db0] forwards to Blocks487d70_Clear
extern "C" int __fastcall Thunk487db0(BYTE* s, void* d) { return q238_Blocks487d70_Clear(s, d); }

// === the AI's player messages (FUN_46d7f0)

KING_VAR(BYTE*, g_pm68b990, 68b990)
KING_VAR(BYTE, g_pm68d0ec, 68d0ec)
KING_VAR(BYTE, g_pm66f62a, 66f62a)
KING_VAR(int, g_pm66f62c, 66f62c)
KING_VAR(BYTE*, g_pm68d308, 68d308)
KING_VAR(BYTE*, g_pm68d30c, 68d30c)
KING_VAR(BYTE*, g_pm68d314, 68d314)
KING_VAR(BYTE*, g_pm68d318, 68d318)
KING_VAR(int, g_pm68d188, 68d188)
KING_VAR(AsmLabel, s_pm68d1c0, 68d1c0)
KING_VAR(AsmLabel, s_pm66f72c, 66f72c)
KING_VAR(AsmLabel, s_pm66f578, 66f578)
KING_VAR(AsmLabel, s_pm66f750, 66f750)
KING_VAR(AsmLabel, s_pm66f704, 66f704)
KING_VAR(AsmLabel, s_pm66f6d4, 66f6d4)
KING_VAR(AsmLabel, s_pm66f674, 66f674)
KING_VAR(AsmLabel, s_pm66f65c, 66f65c)
KING_VAR(AsmLabel, s_pm66f638, 66f638)
KING_VAR(AsmLabel, s_pm66f630, 66f630)
extern "C" void FUN_401010(); KING_ALIAS("_FUN_401010", "_Ai_Report")
extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")
extern "C" void FUN_40ad00(); KING_ALIAS("_FUN_40ad00", "_Ai_FinishRacingPlayer")
extern "C" void FUN_417770(); KING_ALIAS("_FUN_417770", "_Player_Retire")
extern "C" void FUN_41d300(); KING_ALIAS("_FUN_41d300", "@Int_Copy@12")
extern "C" void FUN_420000(); KING_ALIAS("_FUN_420000", "_Rec_4015a0Both")
extern "C" void FUN_422390(); KING_ALIAS("_FUN_422390", "_Player_Register")
extern "C" void FUN_426280(); KING_ALIAS("_FUN_426280", "@Int_Set@12")
extern "C" void FUN_4579e0(); KING_ALIAS("_FUN_4579e0", "_Range_CallD")
extern "C" void FUN_46ea00(); KING_ALIAS("_FUN_46ea00", "@Ordr_First@8")
extern "C" void FUN_4729e0(); KING_ALIAS("_FUN_4729e0", "@Player_Construct@12")
extern "C" void FUN_4731e0(); KING_ALIAS("_FUN_4731e0", "@Player_Destroy@4")
extern "C" void FUN_473420(); KING_ALIAS("_FUN_473420", "_Players_Append")
extern "C" void FUN_474720(); KING_ALIAS("_FUN_474720", "@Player_Load@12")
extern "C" void FUN_4afbf0(); KING_ALIAS("_FUN_4afbf0", "@SchList_Begin@12")
extern "C" void FUN_4afc20(); KING_ALIAS("_FUN_4afc20", "@SchList_End@12")

static void Pm_Report(int level, int line, const char* msg, const void* arg)
{
    CallC(FUN_401010, 0, level, (const char*)s_pm66f72c, (const char*)s_pm66f578, line, msg, arg,
          (const char*)s_pm68d1c0);
}

// the player list's entry of key (the head when missing)
static BYTE* Pm_Find(const void* key)
{
    void* end;
    void* begin;
    CallThis(FUN_4afc20, &g_pm68b990, &end);
    CallThis(FUN_4afbf0, &g_pm68b990, &begin);
    BYTE* r;
    CallC(FUN_4579e0, &r, begin, end, key);
    return r;
}

// bit 0x100 of p's +88: whether its +712 differs from 68d318's +8
static void Pm_SetForeign(BYTE* p)
{
    if (F(int, p, 712) != F(int, g_pm68d318, 8))
        F(DWORD, p, 88) |= 0x100;
    else
        F(DWORD, p, 88) &= ~0x100u;
}

extern "C" {

// [FUN_46d7f0] handles the AI's network messages 991..999 (id, data): 991 a new player loaded from data,
// 992 a player's state reloaded, 994 the current entry of the 68d308 table chosen; 1 when handled
int __cdecl Ai_NetMessage(int id, BYTE* data)
{
    if (id < 991 || id > 999)
        return 0;
    if (!g_pm68d0ec)
        Pm_Report(1, 103, (const char*)s_pm66f750, s_pm68d1c0);
    switch (id) {
    case 994: {
        if (g_pm66f62a)
            Pm_Report(1, 110, (const char*)s_pm66f704, s_pm68d1c0);
        BYTE* d = data;
        int key = CallThis<int>(FUN_46ea00, &d);
        BYTE* e = g_pm68d308;
        while (e != g_pm68d30c && F(int, e, 8) != key)
            e += 12;
        if (e == g_pm68d30c)
            Pm_Report(1, 116, (const char*)s_pm66f6d4, s_pm68d1c0);
        g_pm68d314 = e;
        e[0] = 1;
        break;
    }
    case 991: {
        BYTE* p = CallC<BYTE*>(FUN_473420, s_pm68d1c0);
        BYTE* cur = data + 4;
        if (F(int, data, 0) != g_pm66f62c)
            Pm_Report(100, 130, (const char*)s_pm66f674, s_pm68d1c0);
        if (Pm_Find(cur) != g_pm68b990)
            Pm_Report(1, 137, (const char*)s_pm66f65c, cur);
        CallThis(FUN_474720, p, &cur);
        Pm_SetForeign(p);
        int it;
        CallC(FUN_422390, &it, p);
        CallC(FUN_420000, it);
        if (g_pm68d188 == 1 || g_pm68d188 == 2)
            CallC(FUN_417770, it);
        break;
    }
    case 992: {
        BYTE* cur1 = data;
        BYTE* r = Pm_Find(data);
        if (r == g_pm68b990)
            Pm_Report(1, 162, (const char*)s_pm66f638, data);
        int key;
        CallThis(FUN_426280, &key, r);
        BYTE player[1000];
        BYTE* cur2 = data;
        CallThis(FUN_4729e0, player, (const char*)s_pm66f630);
        CallThis(FUN_474720, player, &cur2);
        if (!(F(DWORD, player, 88) & 0x4000) && (F(DWORD, CallC<BYTE*>(FUN_40a9f0, r), 88) & 0x4000))
            CallC(FUN_40ad00, key);
        int it;
        CallThis(FUN_41d300, &it, &r);
        CallThis(FUN_474720, CallC<BYTE*>(FUN_40a9f0, it), &cur1);
        CallThis(FUN_41d300, &it, &r);
        Pm_SetForeign(CallC<BYTE*>(FUN_40a9f0, it));
        CallThis(FUN_4731e0, player);
        break;
    }
    }
    return 1;
}

} // extern "C"

// === moving a rope iterator to the next leaf (FUN_46c4a0)

extern "C" void FUN_46c7d0(); KING_ALIAS("_FUN_46c7d0", "_Stream_SetWindow")

// rope nodes: +0 size, +8 tag (0 leaf, 1 concatenation, 2 function, 3 substring), +16 C string,
// +20 left / data / function, +24 right. Iterators: +0 position, +4 root, +8 leaf position, +12/+16/+20 the
// buffer start / pointer / end, +24 the path cache (4 nodes), +40 its leaf index, +44 the right turns
static int Rp_Tag(const BYTE* n) { return (signed char)F(BYTE, n, 8); }

// the iterator's whole path from the root (_S_setcache)
static void Rp_SetCache(BYTE* x)
{
    BYTE* path[64];
    int depth = -1;
    unsigned start = 0, pos = F(unsigned, x, 0);
    BYTE dirns = 0;
    BYTE* r = F(BYTE*, x, 4);
    if (pos >= F(unsigned, r, 0)) {
        F(BYTE*, x, 16) = 0;
        return;
    }
    if (BYTE* cs = F(BYTE*, r, 16)) {
        F(BYTE*, x, 12) = cs;
        F(BYTE*, x, 20) = cs + F(unsigned, r, 0);
        F(BYTE*, x, 16) = cs + pos;
        F(BYTE*, x, 24) = r;
        F(int, x, 40) = 0;
        F(unsigned, x, 8) = 0;
        return;
    }
    for (;;) {
        path[++depth] = r;
        int tag = Rp_Tag(r);
        if (tag != 1) {
            if (tag > 3)
                continue;
            break;
        }
        unsigned left = F(unsigned, F(BYTE*, r, 20), 0);
        dirns <<= 1;
        if (pos >= start + left) {
            dirns |= 1;
            r = F(BYTE*, r, 24);
            start += left;
        } else
            r = F(BYTE*, r, 20);
    }
    F(unsigned, x, 8) = start;
    int i = -1, j = depth - 3;
    if (j < 0)
        j = 0;
    while (j <= depth)
        F(BYTE*, x, 24 + 4 * ++i) = path[j++];
    F(int, x, 40) = i;
    F(BYTE, x, 44) = dirns;
    CallC(FUN_46c7d0, x);
}

// the buffer over the leaf at the iterator (_S_setbuf, inlined here)
static void Rp_SetBuf(BYTE* x, BYTE* leaf)
{
    unsigned lp = F(unsigned, x, 8), pos = F(unsigned, x, 0);
    int tag = Rp_Tag(leaf);
    if (tag == 0) {
        F(BYTE*, x, 12) = F(BYTE*, leaf, 20);
        F(BYTE*, x, 16) = F(BYTE*, leaf, 20) + (pos - lp);
        F(BYTE*, x, 20) = F(BYTE*, x, 12) + F(unsigned, leaf, 0);
    } else if (tag == 2 || tag == 3) {
        unsigned len = 15, bs = lp, end = lp + F(unsigned, leaf, 0);
        if (bs + len <= pos) {
            bs = pos - 3;
            if (bs + len > end)
                bs = end - 15;
        }
        if (bs + len > end)
            len = end - bs;
        CallVirt(F(BYTE*, leaf, 20), 4, bs - lp, len, x + 45);
        F(BYTE*, x, 12) = x + 45;
        F(BYTE*, x, 16) = x + 45 + (pos - bs);
        F(BYTE*, x, 20) = x + 45 + len;
    }
}

extern "C" {

// [FUN_46c4a0] rope iterator x after an increment (_S_setcache_for_incr): more of the current leaf, or up the
// cached path to the next right branch and down its left edge, or the whole path again when the cache ran out
void __cdecl Rope_SetCacheForIncr(BYTE* x)
{
    int idx = F(int, x, 40);
    BYTE* node = F(BYTE*, x, 24 + idx * 4);
    unsigned start = F(unsigned, x, 8);
    BYTE dirns = F(BYTE, x, 44);
    if (F(unsigned, x, 0) - start < F(unsigned, node, 0)) {
        Rp_SetBuf(x, node);
        return;
    }
    while (--idx >= 0) {
        if (!(dirns & 1))
            break;
        node = F(BYTE*, x, 24 + idx * 4);
        start -= F(unsigned, F(BYTE*, node, 20), 0);
        dirns >>= 1;
    }
    if (idx < 0) {
        Rp_SetCache(x);
        return;
    }
    node = F(BYTE*, x, 24 + idx * 4);
    start += F(unsigned, F(BYTE*, node, 20), 0);
    node = F(BYTE*, node, 24);
    F(BYTE*, x, 24 + ++idx * 4) = node;
    dirns |= 1;
    while (F(BYTE, node, 8) == 1) {
        if (++idx == 4) {
            for (int i = 0; i < 3; i++)
                F(BYTE*, x, 24 + i * 4) = F(BYTE*, x, 28 + i * 4);
            idx = 3;
        }
        node = F(BYTE*, node, 20);
        F(BYTE*, x, 24 + idx * 4) = node;
        dirns <<= 1;
    }
    F(int, x, 40) = idx;
    F(unsigned, x, 8) = start;
    F(BYTE, x, 44) = dirns;
    Rp_SetBuf(x, F(BYTE*, x, 24 + idx * 4));
}

} // extern "C"

// === the waypoints of a route from the next-hop tables (FUN_484810)

extern "C" double __cdecl Road_EdgeCost(int side, BYTE* edge, int dir);            // ai_world.cpp
extern "C" DWORD* __cdecl Rec12_Construct(DWORD* p, const DWORD* src);            // route_search.cpp
extern "C" void __fastcall Vec12_Insert47b790(BYTE** v, void*, BYTE* pos, const BYTE* x);   // road_graph.cpp
KING_VAR(BYTE**, g_rw68cf00, 68cf00)        // the road nodes
KING_VAR(AsmLabel, s_rw6701e0, 6701e0)
KING_VAR(AsmLabel, s_rw6700c8, 6700c8)
KING_VAR(AsmLabel, s_rw670230, 670230)

// appends waypoint record r (12 bytes) to vector v
static void Rw_Push(BYTE** v, const DWORD* r)
{
    if (v[1] != v[2]) {
        Rec12_Construct((DWORD*)v[1], r);
        v[1] += 12;
    } else
        Vec12_Insert47b790(v, 0, v[1], (const BYTE*)r);
}

extern "C" {

// [FUN_484810] out = the waypoints (node, edge, kind) of the route on side from node `from` to node `to` by the
// next-hop bytes (node +196 + side * 4, per target: 127 none, 255 an error, else bit 7 the direction and the
// edge index) ending with `to`; *cost = their edge costs, or the largest double and an empty vector without one
ByteVec* __cdecl Route_Waypoints(ByteVec* out, int side, int from, int to, double* cost)
{
    BYTE* v[3] = {0, 0, 0};
    *cost = 0.0;
    BYTE hop = F(BYTE*, g_rw68cf00[from], 196 + side * 4)[to];
    int node = from;
    bool found = false;
    while (hop != 127) {
        if (hop == 255)
            CallC(FUN_401010, 0, 1, (const char*)s_rw6701e0, (const char*)s_rw6700c8, 374, (const char*)s_rw670230,
                  (const char*)s_pm68d1c0, (const char*)s_pm68d1c0);
        int dir = hop >> 7, k = hop & 127;
        BYTE* edge = F(BYTE**, g_rw68cf00[node], 20 + dir * 12)[k];
        DWORD r[3] = {(DWORD)node, (DWORD)edge, (DWORD)hop << 16};
        Rw_Push(v, r);
        *cost = Road_EdgeCost(side, F(BYTE*, v[1], -8), dir) + *cost;
        int next = F(int, F(BYTE*, F(BYTE*, v[1], -8), 112 + (dir ^ 1) * 4), 16);
        if (next == to) {
            found = true;
            break;
        }
        node = next;
        hop = F(BYTE*, g_rw68cf00[node], 196 + side * 4)[to];
    }
    out->begin = out->end = out->cap = 0;
    if (!found) {
        F(DWORD, cost, 0) = 0xFFFFFFFF;
        F(DWORD, cost, 4) = 0x7FEFFFFF;
    } else {
        DWORD r[3] = {(DWORD)to, 0, 0x00FF0000};
        Rw_Push(v, r);
        int n = (int)(v[1] - v[0]) / 12;
        BYTE* p = n ? (BYTE*)Stl_Allocate(n * 12) : 0;
        out->begin = out->end = p;
        out->cap = p + n * 12;
        out->end = CallC<BYTE*>(FUN_47e3e0, v[0], v[1], p, side);
    }
    if (int bytes = (int)(v[2] - v[0]) / 12 * 12)
        CallC(FUN_44b4a0, v[0], bytes);
    return out;
}

} // extern "C"

// === sending a player's state (FUN_46dc50)

extern "C" void FUN_475b20(); KING_ALIAS("_FUN_475b20", "@Player_Save@12")
extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")
extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
extern "C" void FUN_6021b0(); KING_ALIAS("_FUN_6021b0", "_Dp_Send")
KING_VAR(BYTE*, g_sp68d318, 68d318)
KING_VAR(DWORD, g_sp66f62c, 66f62c)

extern "C" {

// [FUN_46dc50] in a network game sends player p's state (message 991, after the 66f62c tag): a remote player
// (+84 = 1) gets the local machine's id (68d318 +8) at +712, others the least used machine (68d308, count +4)
// and are marked remote (+88 bit 8) when that isn't the local one
int __cdecl Net_SendPlayer(BYTE* p)
{
    if (!g_multOn)
        return 0;
    if (F(int, p, 84) == 1)
        F(int, p, 712) = F(int, g_sp68d318, 8);
    else {
        BYTE* best = g_68d308;
        for (BYTE* e = g_68d308; e != g_68d30c; e += 12)
            if (F(int, e, 4) < F(int, best, 4))
                best = e;
        F(int, best, 4)++;
        F(int, p, 712) = F(int, best, 8);
        if (F(int, best, 8) != F(int, g_sp68d318, 8))
            F(DWORD, p, 88) |= 0x100;
    }
    BYTE msg[32];
    CallThis(FUN_602be0, msg);
    ByteVec v = {0, 0, 0};
    DWORD tag = g_sp66f62c;
    CallThis(FUN_46e460, &v, (BYTE*)0, (BYTE*)&tag, (BYTE*)&tag + 4);
    CallThis(FUN_475b20, p, &v);
    CallThis(FUN_603110, msg, 991, v.begin, v.end - v.begin);
    CallC(FUN_6021b0, F(BYTE*, msg, 4), F(BYTE*, msg, 8) - F(BYTE*, msg, 4));
    if (v.cap - v.begin)
        CallC(FUN_44b4a0, v.begin, v.cap - v.begin);
    CallThis(FUN_602e80, msg);
    return 0;
}

} // extern "C"

// === listing the network machines (FUN_46d570)

extern "C" void FUN_46e170(); KING_ALIAS("_FUN_46e170", "@Vec12_Erase@16")
extern "C" void FUN_46e6b0(); KING_ALIAS("_FUN_46e6b0", "@Vec12_Insert46e6b0@16")
extern "C" void FUN_46d550(); KING_ALIAS("_FUN_46d550", "@Mult46d550_Construct@12")
extern "C" void FUN_6024d0(); KING_ALIAS("_FUN_6024d0", "_Dp_Receive")
extern "C" void FUN_6024f0(); KING_ALIAS("_FUN_6024f0", "_Dp_TakeMessage")
extern "C" void FUN_602540(); KING_ALIAS("_FUN_602540", "_Dp_Get723d88")
extern "C" void FUN_487cc0(); KING_ALIAS("_FUN_487cc0", "_Rec12_Construct")
extern "C" void __cdecl Rec12_StableSort(BYTE* first, BYTE* last, int, int, int comp);   // cargo_orders.cpp
KING_VAR(BYTE*, g_nm68d310, 68d310)

// appends the machine record for id to the list at 68d308
static void Nm_Add(DWORD id)
{
    BYTE r[12];
    BYTE* p = CallThis<BYTE*>(FUN_46d550, r, id);
    if (g_68d30c != g_nm68d310) {
        CallC(FUN_487cc0, g_68d30c, p);
        g_68d30c += 12;
    } else
        CallThis(FUN_46e6b0, (BYTE*)&g_68d308, g_68d30c, p);
}

extern "C" {

// [FUN_46d570] rebuilds the list of the network's machines (68d308, 12 bytes: +1 local, +4 players, +8 id) from
// the session's players and the local one, sorted by id; 68d318 / 68d320 the local machine, and the host
// (66f62a) marks it and announces its id (message 994)
void __cdecl Net_ListMachines()
{
    CallThis(FUN_46e170, (BYTE*)&g_68d308, g_68d308, g_68d30c);
    if ((g_nm68d310 - g_68d308) / 12 < 8) {
        int n = (int)(g_68d30c - g_68d308) / 12;
        BYTE* p = Alloc12_Copy(8, g_68d308, g_68d30c);
        if (int bytes = (int)(g_nm68d310 - g_68d308) / 12 * 12)
            CallC(FUN_44b4a0, g_68d308, bytes);
        g_68d308 = p;
        g_68d30c = p + n * 12;
        g_nm68d310 = p + 96;
    }
    g_pm68d314 = 0;
    g_sp68d318 = 0;
    DWORD id;
    if (!CallC<int>(FUN_6024d0, &id))
        do
            Nm_Add(id);
        while (!CallC<int>(FUN_6024f0, &id));
    DWORD local = 0;
    CallC(FUN_602540, &local);
    Nm_Add(local);
    F(BYTE, g_68d30c, -11) = 1;
    Rec12_StableSort(g_68d308, g_68d30c, 0, 0, 0);
    BYTE* e = g_68d308;
    while (e != g_68d30c && !F(BYTE, e, 1))
        e += 12;
    g_sp68d318 = e;
    g_68d320 = (int)(e - g_68d308) / 12;
    if (g_pm66f62a) {
        F(BYTE, e, 0) = 1;
        g_pm68d314 = e;
        DWORD x = F(DWORD, e, 8);
        BYTE m[32];
        CallThis(FUN_602be0, m);
        CallThis(FUN_603110, m, 994, &x, 4);
        CallC(FUN_6021b0, F(BYTE*, m, 4), F(BYTE*, m, 8) - F(BYTE*, m, 4));
        CallThis(FUN_602e80, m);
    }
}

} // extern "C"

// === re-planning a player's route (FUN_4872f0)

struct Rp_Pos { int road, node; double t; int segment, object, m_24, m_28; };
extern "C" void Rp_Valid(); KING_ALIAS("_Rp_Valid", "@Position_Valid@8")
extern "C" void Rp_Error(); KING_ALIAS("_Rp_Error", "_Ai_Report")
KING_VAR(BYTE**, g_rp68cf38, 68cf38)
KING_VAR(BYTE**, g_rp68cf3c, 68cf3c)
KING_VAR(char, g_rp68d1c0, 68d1c0)
extern "C" void Rp_FindRoute(); KING_ALIAS("_Rp_FindRoute", "_Road_FindPositionRoute")
extern "C" void Rp_SetHops(); KING_ALIAS("_Rp_SetHops", "@Player_SetHops@12")
extern "C" void Rp_PointOf(); KING_ALIAS("_Rp_PointOf", "_Road_PointOf")
extern "C" void Rp_SetPosition(); KING_ALIAS("_Rp_SetPosition", "_Ai_SetPlayerPosition")
KING_VAR(char, g_rp77e6e0, 77e6e0)
KING_VAR(char, s_rp66cc0c, 66cc0c)    // "(%d %g %d %d %x)"
KING_VAR(char, s_rp66cc00, 66cc00)
KING_VAR(char, s_rp66cbf4, 66cbf4)
KING_VAR(char, s_rp66cbe0, 66cbe0)

extern "C" {

// [FUN_4872f0] gives player p the route from its position (+40) to target (reporting an invalid target or one
// whose segment its road lacks, line 876), then places it at its position's point
void __cdecl Player_Replan(BYTE* p, Rp_Pos target)
{
    Rp_Pos copy = target;
    if (!CallThis<int>(Rp_Valid, &copy)) {
        sprintf(&g_rp77e6e0, &s_rp66cc0c, target.road, target.t, target.segment, target.object, target.m_24);
        CallC(Rp_Error, 33, 100, (const char*)s_rw6701e0, (const char*)s_rw6700c8, 876, &s_rp66cbf4, &s_rp66cc00, &g_rp77e6e0);
    }
    if (target.road >= 0 && (unsigned)target.road < (unsigned)(g_rp68cf3c - g_rp68cf38)) {
        BYTE* e = g_rp68cf38[target.road];
        BYTE** objs = F(BYTE**, e, 120);
        if (!(target.object < F(BYTE**, e, 124) - objs && target.segment < CallVirt<int>(objs[target.object], 24)))
            CallC(Rp_Error, 33, 100, (const char*)s_rw6701e0, (const char*)s_rw6700c8, 876, &s_rp66cbe0, &g_rp68d1c0,
                &g_rp68d1c0);
    }
    BYTE* route[3];
    int tmp[3] = {0, 0, 0};
    Rp_Pos from = *(Rp_Pos*)(p + 40);
    CallThis(Rp_SetHops, p + 8, CallC<BYTE*>(Rp_FindRoute, route, 0, from, target, tmp));
    if (int n = (int)(route[2] - route[0]) / 12)
        CallC(FUN_44b4a0, route[0], n * 12);
    BYTE pt[24];
    BYTE* q = CallC<BYTE*>(Rp_PointOf, pt, *(Rp_Pos*)(p + 40));
    CallC(Rp_SetPosition, p, q);
}

} // extern "C"

// [FUN_4870e0] gives player p the route from its position (+40) to node n (a3 passed on to the search), then
// places it at its position's point
extern "C" void __cdecl Player_ReplanToNode(BYTE* p, int n, int a3)
{
    Rp_Pos target = {-1, n, 0.0, 0, 0, 0, -1};
    BYTE* route[3];
    Rp_Pos from = *(Rp_Pos*)(p + 40);
    CallThis(Rp_SetHops, p + 8, CallC<BYTE*>(Rp_FindRoute, route, 0, from, target, a3));
    if (int k = (int)(route[2] - route[0]) / 12)
        CallC(FUN_44b4a0, route[0], k * 12);
    BYTE pt[24];
    BYTE* q = CallC<BYTE*>(Rp_PointOf, pt, *(Rp_Pos*)(p + 40));
    CallC(Rp_SetPosition, p, q);
}

// [FUN_46ad50] pop_heap of 4-byte elements: the top goes to *result, value
// is sifted in from the root (pred passed on)
extern "C" void __cdecl PopHeapAux4(DWORD* first, DWORD* last, DWORD* result, DWORD value, DWORD pred, int)
{
    *result = *first;
    CallC(FUN_46c190, first, 0, (int)(last - first), value, pred);
}
