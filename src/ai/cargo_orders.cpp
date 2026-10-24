// cargo_orders.cpp - ai_ordr.cpp of the AI, rewritten from king.masm: the
// STL instances of the orders (12-byte entries: copies, fills, binary
// searches by the key at +8, reversal), the order record's constructor and
// its values.
//
// Building with  nmake ASM_CARGO_ORDERS=1  makes king.masm assemble the original
// procedures again.

#include <stdlib.h>
#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "FUN_401000")
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert(pos, first, last)
extern "C" void FUN_470900(); KING_ALIAS("_FUN_470900", "@CharVec_Construct@12")
extern "C" void FUN_44ac60(); KING_ALIAS("_FUN_44ac60", "_Vec4b_AllocCopy@12")    // vector<pointer>: a new buffer (n, first, last)
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // the AI's allocator: free (p, size)

ASM_VAR(BYTE*, g_68ccb8, $L_68ccb8)             // 104-byte records
ASM_VAR(double, g_bonusFactor, $L_66f878)       // 1.5
ASM_VAR(double, g_68d6c8, $L_68d6c8)

struct E12 { DWORD a, b, key; };

extern "C" char* __cdecl CopyBytes(const char* first, const char* last, char* to)
{
    for (int n = (int)(last - first); n > 0; n--)
        *to++ = *first++;
    return to;
}

extern "C" void __cdecl E12_Distance(const E12* first, const E12* last, int* n)
{
    *n += (int)(last - first);
}

extern "C" E12* __cdecl E12_CopyBackward(const E12* first, const E12* last, E12* to)
{
    for (int n = (int)(last - first); n > 0; n--)
        *--to = *--last;
    return to;
}

extern "C" void __cdecl E12_Advance(E12** it, int n)
{
    *it += n;
}

extern "C" void __cdecl E12_FillN(E12* to, int n, const E12* v)
{
    for (; n > 0; n--, to++)
        if (to)
            *to = *v;
}

extern "C" E12* __cdecl E12_LowerBound(E12* first, E12* last, const E12* v)
{
    DWORD k = v->key;
    for (int n = (int)(last - first); n > 0;) {
        int half = n >> 1;
        if (first[half].key < k) {
            first += half + 1;
            n += -1 - half;
        } else
            n = half;
    }
    return first;
}

extern "C" E12* __cdecl E12_UpperBound(E12* first, E12* last, const E12* v)
{
    DWORD k = v->key;
    for (int n = (int)(last - first); n > 0;) {
        int half = n >> 1;
        if (k >= first[half].key) {
            first += half + 1;
            n += -1 - half;
        } else
            n = half;
    }
    return first;
}

extern "C" void __cdecl E12_Reverse(E12* first, E12* last)
{
    while (first != last && first != --last) {
        E12 t = *first;
        *first++ = *last;
        *last = t;
    }
}

extern "C" void __cdecl E12_Swap(E12* a, E12* b)
{
    E12 t = *a;
    *a = *b;
    *b = t;
}

// vector<char> = s (with its terminating zero)
extern "C" void __fastcall CharVec_Assign(char** self, void*, const char* s)
{
    self[1] = self[0];
    if (s)
        CallThis(FUN_44c220, self, self[1], s, s + strlen(s) + 1);
}

extern "C" BYTE* __fastcall Order_Construct(BYTE* self, void*)
{
    for (int i = 0; i <= 16; i += 4)
        F(int, self, i) = -1;
    F(int, self, 24) = -1;
    F(int, self, 28) = -1;
    for (int i = 32; i <= 48; i += 4)
        F(int, self, i) = 0;
    F(int, self, 52) = -1;
    F(int, self, 56) = 0;
    F(int, self, 64) = 0;
    F(int, self, 60) = 0;
    F(int, self, 68) = 0;
    F(double, self, 72) = AI_Now() + 900.0;
    F(double, self, 80) = AI_Now();
    F(double, self, 88) = 900.0;
    F(double, self, 96) = 1.0;
    F(int, self, 104) = 0;
    F(int, self, 112) = 0;
    for (int i = 116; i <= 132; i += 4)
        F(int, self, i) = 0;
    F(int, self, 136) = -1;
    return self;
}

// the order's value: sqrt(record value * amount) * 10, less the part done
extern "C" double __cdecl Order_Value(const int* o)
{
    double v = x87_sqrt(F(double, g_68ccb8, o[0] * 104 + 40) * F(double, o, 56));
    return v * (1.0 - F(double, o, 96)) * 10.0;
}

extern "C" double __cdecl Order_FullValue(const int* o)
{
    return x87_sqrt(F(double, g_68ccb8, o[0] * 104 + 40) * F(double, o, 56)) * 10.0;
}

extern "C" double __cdecl Order_Pay(const BYTE* o, double late, int bonus)
{
    double v = F(double, o, 64);
    if ((char)bonus && x87_eq(late, 0.0) && v > 0.0 && F(int, o, 104) == 1)
        v *= g_bonusFactor;
    return v;
}

// ---------------------------------------------------------------------------
// medium-sized functions: an order's price and margin, the order constructor

struct OrdrPos { int road, node; double t; int segment, object, m_24, m_28; };   // PositionId

extern "C" void __cdecl AiError(int kind, int code, const char* file, const char* id, int line,
                                const char* msg, const char* a, const char* b);
KING_ALIAS("_AiError", "_Ai_Report")
extern "C" int* __cdecl Town_Id(int* out, BYTE* town);                                 // aimain.cpp
extern "C" void __cdecl Road_PositionCostBest(int side, OrdrPos from, OrdrPos to, double* cost);   // aitrce.cpp
extern "C" double __cdecl Order_Value(const int* o);
typedef void* PtrList[1];
ASM_VAR(PtrList, g_ordrTowns, $L_68cec0)
ASM_VAR(PtrList, g_ordrVehicles, $L_68cfd8)
ASM_VAR(void*, g_ordrPlayers, $L_68b990)
ASM_VAR(char, g_ordrEmpty, $L_68d1c0)
ASM_VAR(double, g_priceDirect, $L_66f880)       // 1.2
ASM_VAR(double, g_priceUrgent, $L_66f888)       // 1.5
ASM_VAR(double, g_priceSpecial, $L_66f890)      // 4.0

static const char s_ordrId[] = "$Id: ai_ordr.cpp 2001/9/10 18:14:44";
static const char s_ordrFile[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\Ai_ordr.cpp";

static BYTE* Town(int i) { return (BYTE*)((void**)g_ordrTowns[0])[i]; }

// the unload node of the order's town (reported when missing)
static int UnloadNode(int* o, int line)
{
    int node;
    Town_Id(&node, Town(o[3]));
    if (node == -1)
        AiError(45, 100, s_ordrFile, s_ordrId, line, "Unload node not present for town",
                F(const char*, Town(o[3]), 12), &g_ordrEmpty);
    return node;
}

// the cost of the road from the order's position (+24) to that node
static double UnloadCost(int* o, int node)
{
    OrdrPos to = {-1, node, 0.0, 0, 0, 0, -1};
    double cost = 0;
    Road_PositionCostBest(0, F(OrdrPos, o, 24), to, &cost);
    return cost;
}

// the price of an order (+64) for a player
extern "C" void __cdecl Order_SetPrice(BYTE* pl, int* o, char direct, char urgent)
{
    BYTE* rec = g_68ccb8 + o[0] * 104;
    double base = (1.0 - F(double, rec, 32) + 0.2) * x87_sqrt(F(double, rec, 40) * F(double, o, 56));
    if (x87_eq(F(double, o, 112), 0.0)) {
        int node = UnloadNode(o, 135);
        if (o[4] != -1) {
            BYTE* d = F(BYTE*, Town(o[4]), 100);
            F(DWORD, o, 112) = F(DWORD, d, o[3] * 8);
            F(DWORD, o, 116) = F(DWORD, d, o[3] * 8 + 4);
        } else
            F(double, o, 112) = UnloadCost(o, node);
    }
    double k = 115.0;
    if (pl != g_ordrPlayers && (DWORD)pl + 8 != 0) {
        BYTE* c = F(BYTE*, pl + 8, 508);
        if (!c)
            k = 40.0;
        else {
            int type = F(int, ((void**)g_ordrVehicles[0])[F(int, c, 4)], 32);
            if (type != 2 && type != 3)
                k = 40.0;
        }
    }
    double r = (F(double, o, 112) * k * 0.001 + base) * 3.0;
    if (x87_eq(F(double, o, 56), 0.0))
        r = 0.0;
    if (direct && o[26] != 1)
        r = r * g_priceDirect;
    if (urgent)
        r = r * g_priceUrgent;
    if (o[26] == 3 || (direct && !urgent))
        r = r * g_priceSpecial;
    F(double, o, 64) = r;
}

// what an order brings above its value; checks both
extern "C" double __cdecl Order_Margin(int* o, double done, char bonus, double* price, double* value)
{
    double v = Order_Value(o);
    if (v > 100000000.0 || !(v >= 0.0))
        AiError(0, 200, s_ordrFile, s_ordrId, 187, "Internal error 09458", &g_ordrEmpty, &g_ordrEmpty);
    if (value)
        *value = v;
    double x = F(double, o, 64);
    if (bonus && x87_eq(done, 0.0) && x > 0.0 && o[26] == 1)
        x = x * g_bonusFactor;
    if (price)
        *price = x;
    double d = x - v;
    if (!(d >= -1e-08))
        AiError(0, 200, s_ordrFile, s_ordrId, 201, "Internal error 09459", &g_ordrEmpty, &g_ordrEmpty);
    return d;
}

// an order from src for the position pos and the quantity q; returns src,
// which the original leaves in EAX
extern "C" const int* __cdecl Order_Init4715f0(int* o, const int* src, int arg, OrdrPos pos, double q)
{
    o[0] = src[0];
    o[1] = src[1];
    o[2] = src[2];
    o[3] = src[3];
    o[26] = 1;
    F(double, o, 72) = AI_Now() + F(double, src, 24);
    o[4] = arg;
    F(OrdrPos, o, 24) = pos;
    F(double, o, 80) = AI_Now();
    F(DWORD, o, 56) = F(DWORD, &q, 0);
    F(DWORD, o, 60) = F(DWORD, &q, 4);
    o[24] = 0;
    o[25] = 0x3ff00000;
    if (!(q >= 0.0))
        AiError(0, 512, s_ordrFile, s_ordrId, 244, "Q<0", &g_ordrEmpty, &g_ordrEmpty);
    F(double, o, 112) = UnloadCost(o, UnloadNode(o, 248));
    o[22] = src[6];
    o[23] = src[7];
    return src;
}

extern "C" {

// [FUN_470900] constructor of a char vector from a string
char** __fastcall CharVec_Construct(char** self, void*, const char* s)
{
    self[0] = self[1] = self[2] = 0;
    CharVec_Assign(self, 0, s);
    return self;
}

}

extern "C" void FUN_470760(); KING_ALIAS("_FUN_470760", "_E12_FillN")

extern "C" {

// [FUN_46ed00] temporary buffer of 12-byte records for [first, last):
// {requested, length, buffer}, halving the length while malloc fails
int* __fastcall TempBuf12_Construct(int* self, void*, BYTE* first, BYTE* last)
{
    int n = (last - first) / 12;
    self[1] = n;
    self[0] = n;
    self[2] = 0;
    if (n > 0x0aaaaaaa)
        self[1] = 0x0aaaaaaa;
    while (self[1] > 0) {
        self[2] = (int)malloc(self[1] * 12);
        if (self[2])
            break;
        self[1] /= 2;
    }
    if (self[1] > 0)
        CallC(FUN_470760, self[2], self[1], first, last);
    return self;
}

}

extern "C" void FUN_46ecd0(); KING_ALIAS("_FUN_46ecd0", "_CopyBytes")    // uninitialized copy of bytes (first, last, to), returns the end
extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")    // the AI's allocator: allocate (size)
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
extern "C" void* __cdecl memmove(void* d, const void* s, size_t n);

extern "C" {

// [FUN_46e460] vector<char>::insert(pos, first, last)
void __fastcall CharVec_InsertRange(BYTE** self, void*, BYTE* pos, BYTE* first, BYTE* last)
{
    if (first == last)
        return;
    unsigned n = last - first;
    if ((unsigned)(self[2] - self[1]) >= n) {
        BYTE* oldEnd = self[1];
        unsigned after = oldEnd - pos;
        if (after > n) {
            CallC(FUN_46ecd0, oldEnd - n, oldEnd, oldEnd, first, 0);
            self[1] += n;
            BYTE* src = oldEnd - n;
            BYTE* dst = oldEnd;
            for (int k = src - pos; k > 0; k--)
                *--dst = *--src;
            memmove(pos, first, n);
        } else {
            memmove(oldEnd, first + after, last - (first + after));
            self[1] = oldEnd + (n - after);
            CallC(FUN_46ecd0, pos, oldEnd, self[1], first, 0);
            self[1] += after;
            memmove(pos, first, after);
        }
        return;
    }
    unsigned size = self[1] - self[0];
    unsigned len = (size < n ? n : size) + size;
    BYTE* p = len ? CallC<BYTE*>(FUN_44b740, len) : 0;
    BYTE* q = CallC<BYTE*>(FUN_46ecd0, self[0], pos, p, first, 0);
    memmove(q, first, n);
    q += last - first;
    BYTE* r = CallC<BYTE*>(FUN_46ecd0, pos, self[1], q, first, 0);
    if (self[2] - self[0])
        CallC(FUN_44b4a0, self[0], self[2] - self[0]);
    self[0] = p;
    self[1] = r;
    self[2] = p + len;
}

}

// [FUN_46ea00] the first dword of the object the first field points to
extern "C" int __fastcall Ordr_First(int** self, int)
{
    return *self[0];
}

extern "C" void __cdecl VecPod_InsertAux(BYTE** v, BYTE* pos, const BYTE* x, int n, void (*copy)());
extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")    // uninitialized copy of 12-byte records

// [FUN_46e6b0] vector<12 bytes>::insert_aux
extern "C" void __fastcall Vec12_Insert46e6b0(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    VecPod_InsertAux(v, pos, x, 12, FUN_47e3e0);
}

// ---------------------------------------------------------------------------
extern "C" double __cdecl Order_FullValue(const int* o);
extern "C" double __cdecl Order_Value(const int* o);
extern "C" double __cdecl Order_Pay(const BYTE* o, double late, int bonus);

// [FUN_4715a0] the pay for (late, bonus) plus the value lost so far
extern "C" double __cdecl Order_PayTotal(const int* o, double late, int bonus)
{
    double full = Order_FullValue(o);
    double lost = full - Order_Value(o);
    return Order_Pay((const BYTE*)o, late, bonus) + lost;
}

// ---------------------------------------------------------------------------
// Static initializers of random generators.
typedef char AsmLabel[1];

ASM_VAR(AsmLabel, g_carordRng68d410, $L_68d410)
ASM_VAR(DWORD, g_carordRng68d4e8, $L_68d4e8)
ASM_VAR(DWORD, g_carordRng68d4ec, $L_68d4ec)
ASM_VAR(DWORD, g_carordRng68d4f0, $L_68d4f0)

// the subtractive random generator's start (Knuth): 55 words at a, then the seed and the two indices
static void Carord_RngInit(DWORD* a, DWORD* seed, DWORD* i1, DWORD* i2)
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

// [$L_471800]
extern "C" void __cdecl CarordInit_471800() { Carord_RngInit((DWORD*)g_carordRng68d410, &g_carordRng68d4e8, &g_carordRng68d4ec, &g_carordRng68d4f0); }

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

ASM_VAR(AsmLabel, g_carordRng68d328, $L_68d328)
ASM_VAR(DWORD, g_carordRng68d400, $L_68d400)
ASM_VAR(DWORD, g_carordRng68d404, $L_68d404)
ASM_VAR(DWORD, g_carordRng68d408, $L_68d408)

// [$L_470cd0]
extern "C" void __cdecl CarordInit_470cd0() { Carord_RngInit((DWORD*)g_carordRng68d328, &g_carordRng68d400, &g_carordRng68d404, &g_carordRng68d408); }

// === stable sort of 12-byte records with a buffer (FUN_46f0a0)

extern "C" void FUN_46f470(); KING_ALIAS("_FUN_46f470", "_Rec12_Insertion")
extern "C" void FUN_46f840(); KING_ALIAS("_FUN_46f840", "_Rec12_MergeAdaptive")
extern "C" void FUN_470110(); KING_ALIAS("_FUN_470110", "_Rec12_MergeLoop")
extern "C" void FUN_4703a0(); KING_ALIAS("_FUN_4703a0", "_Rec12_Merge")

static int Ss12_Count(const BYTE* a, const BYTE* b) { return (int)(b - a) / 12; }

// one merge pass of runs of step from [first, last) to result
static void Ss12_MergePass(BYTE* first, BYTE* last, BYTE* result, int step, int comp)
{
    int two = step * 2;
    while (Ss12_Count(first, last) >= two) {
        result = CallC<BYTE*>(FUN_4703a0, first, first + step * 12, first + step * 12, first + two * 12, result, comp);
        first += two * 12;
    }
    int s = Ss12_Count(first, last);
    if (step < s)
        s = step;
    CallC(FUN_4703a0, first, first + s * 12, first + s * 12, last, result, comp);
}

// insertion sort of runs of 7, then merge passes through the buffer and back
static void Ss12_SortWithBuffer(BYTE* first, BYTE* last, BYTE* buf, int comp)
{
    int len = Ss12_Count(first, last);
    BYTE* bufLast = buf + len * 12;
    BYTE* p = first;
    while (Ss12_Count(p, last) >= 7) {
        CallC(FUN_46f470, p, p + 84, comp);
        p += 84;
    }
    CallC(FUN_46f470, p, last, comp);
    for (int step = 7; step < len;) {
        Ss12_MergePass(first, last, buf, step, comp);
        step *= 2;
        CallC(FUN_470110, buf, bufLast, first, step, comp);
        step *= 2;
    }
}

extern "C" {

// [FUN_46f0a0] stable_sort's adaptive step for [first, last) of 12-byte records with the buffer buf (size n):
// each half sorted (recursively when it doesn't fit), then merged
void __cdecl Rec12_StableSortAdaptive(BYTE* first, BYTE* last, BYTE* buf, int n, int comp)
{
    int len = (Ss12_Count(first, last) + 1) / 2;
    BYTE* mid = first + len * 12;
    if (len > n) {
        Rec12_StableSortAdaptive(first, mid, buf, n, comp);
        Rec12_StableSortAdaptive(mid, last, buf, n, comp);
    } else {
        Ss12_SortWithBuffer(first, mid, buf, comp);
        Ss12_SortWithBuffer(mid, last, buf, comp);
    }
    CallC(FUN_46f840, first, mid, last, Ss12_Count(first, mid), Ss12_Count(mid, last), buf, n, comp);
}

} // extern "C"

// === merging 12-byte records with a buffer (FUN_46f840, FUN_470490)

static E12* E12_Copy(const E12* first, const E12* last, E12* to)
{
    for (int n = (int)(last - first); n > 0; n--)
        *to++ = *first++;
    return to;
}

extern "C" {

// [FUN_470490] rotate_adaptive: [first, middle) and [middle, last) swapped through buffer buf (size n) when the
// shorter part fits, else in place; returns the new middle
E12* __cdecl E12_RotateAdaptive(E12* first, E12* middle, E12* last, int len1, int len2, E12* buf, int n)
{
    if (len1 > len2 && len2 <= n) {
        E12* end = E12_Copy(middle, last, buf);
        E12_CopyBackward(first, middle, last);
        return E12_Copy(buf, end, first);
    }
    if (len1 <= n) {
        E12* end = E12_Copy(first, middle, buf);
        E12_Copy(middle, last, first);
        return E12_CopyBackward(buf, end, last);
    }
    if (first == middle)
        return last;
    if (last == middle)
        return first;
    E12_Reverse(first, middle);
    E12_Reverse(middle, last);
    while (first != middle && middle != last)
        E12_Swap(first++, --last);
    if (first == middle) {
        E12_Reverse(middle, last);
        return last;
    }
    E12_Reverse(first, middle);
    return first;
}

// [FUN_46f840] merge_adaptive of the sorted runs [first, middle) and [middle, last) (by key, +8) with buffer buf
// (size n): through the buffer when a run fits, else split and rotated
void __cdecl Rec12_MergeAdaptive(E12* first, E12* middle, E12* last, int len1, int len2, E12* buf, int n, int comp)
{
    for (;;) {
        if (len1 <= len2 && len1 <= n) {
            E12* bufEnd = E12_Copy(first, middle, buf);
            E12* b = buf;
            E12* out = first;
            while (b != bufEnd && middle != last)
                *out++ = middle->key < b->key ? *middle++ : *b++;
            E12_Copy(middle, last, E12_Copy(b, bufEnd, out));
            return;
        }
        if (len2 <= n) {
            E12* bufEnd = E12_Copy(middle, last, buf);
            E12* out = last;
            if (first == middle) {
                E12_CopyBackward(buf, bufEnd, out);
                return;
            }
            if (buf == bufEnd) {
                E12_CopyBackward(first, middle, out);
                return;
            }
            E12* l1 = middle - 1;
            E12* l2 = bufEnd - 1;
            for (;;) {
                if (l2->key < l1->key) {
                    *--out = *l1;
                    if (first == l1) {
                        E12_CopyBackward(buf, l2 + 1, out);
                        return;
                    }
                    l1--;
                } else {
                    *--out = *l2;
                    if (buf == l2) {
                        E12_CopyBackward(first, l1 + 1, out);
                        return;
                    }
                    l2--;
                }
            }
        }
        E12* cut1;
        E12* cut2;
        int len11, len22;
        if (len1 > len2) {
            len11 = len1 / 2;
            cut1 = first + len11;
            cut2 = E12_LowerBound(middle, last, cut1);
            len22 = (int)(cut2 - middle);
        } else {
            len22 = len2 / 2;
            cut2 = middle + len22;
            cut1 = E12_UpperBound(first, middle, cut2);
            len11 = (int)(cut1 - first);
        }
        E12* mid = E12_RotateAdaptive(cut1, middle, cut2, len1 - len11, len22, buf, n);
        Rec12_MergeAdaptive(first, cut1, mid, len11, len22, buf, n, comp);
        first = mid;
        middle = cut2;
        len1 -= len11;
        len2 -= len22;
    }
}

} // extern "C"

// === insertion sort of 12-byte records in chunks (FUN_46fe40)

extern "C" void __cdecl Rec12_Insertion(BYTE* first, BYTE* last);   // ai_containers.cpp

extern "C" {

// [FUN_46fe40] insertion sorts of the runs of chunk records of [first, last) (12 bytes, by key +8)
void __cdecl Rec12_ChunkInsertion(BYTE* first, BYTE* last, int chunk)
{
    for (; (last - first) / 12 >= chunk; first += chunk * 12)
        Rec12_Insertion(first, first + chunk * 12);
    Rec12_Insertion(first, last);
}

} // extern "C"

// === stable_sort of 12-byte records (FUN_46ea10, FUN_46ede0)

extern "C" void FUN_46f580(); KING_ALIAS("_FUN_46f580", "_Rec12_MergeNoBuffer")    // merge without a buffer (first, middle, last, len1, len2, comp)
extern "C" void __cdecl Rec12_Insertion(BYTE* first, BYTE* last);   // ai_containers.cpp
extern "C" void __cdecl free(void* p);

extern "C" {

// [FUN_46ede0] inplace_stable_sort of [first, last) (12 bytes, by key +8): insertion sort below 15, else both
// halves and a merge without a buffer
void __cdecl Rec12_InplaceStableSort(BYTE* first, BYTE* last, int comp)
{
    int n = (int)(last - first) / 12;
    if (n < 15) {
        Rec12_Insertion(first, last);
        return;
    }
    BYTE* mid = first + n / 2 * 12;
    Rec12_InplaceStableSort(first, mid, comp);
    Rec12_InplaceStableSort(mid, last, comp);
    CallC(FUN_46f580, first, mid, last, (int)(mid - first) / 12, (int)(last - mid) / 12, comp);
}

// [FUN_46ea10] stable_sort of [first, last) (12-byte records by key +8) through a temporary buffer, in place
// when none can be had
void __cdecl Rec12_StableSort(BYTE* first, BYTE* last, int, int, int comp)
{
    int tb[3];
    TempBuf12_Construct(tb, 0, first, last);
    if (!tb[2])
        Rec12_InplaceStableSort(first, last, comp);
    else
        Rec12_StableSortAdaptive(first, last, (BYTE*)tb[2], tb[1], comp);
    free((void*)tb[2]);
}

} // extern "C"

// === merging 12-byte records without a buffer, and the merge pass (FUN_46f580, FUN_470110)

extern "C" BYTE* __cdecl Rotate12(BYTE* f, BYTE* m, BYTE* l);                          // ai_containers.cpp

// merge of the sorted runs [f1, l1) and [f2, l2) into out (stable, by key); the end of out
static E12* E12_Merge(const E12* f1, const E12* l1, const E12* f2, const E12* l2, E12* out)
{
    while (f1 != l1 && f2 != l2)
        *out++ = f2->key < f1->key ? *f2++ : *f1++;
    return E12_Copy(f2, l2, E12_Copy(f1, l1, out));
}

extern "C" {

// [FUN_46f580] merge_without_buffer of the sorted runs [first, middle) and [middle, last) (by key): split at the
// longer run's middle, rotated and both parts merged
void __cdecl Rec12_MergeNoBuffer(E12* first, E12* middle, E12* last, int len1, int len2, int comp)
{
    while (len1 && len2) {
        if (len1 + len2 == 2) {
            if (middle->key < first->key)
                E12_Swap(first, middle);
            return;
        }
        E12* cut1;
        E12* cut2;
        int len11, len22;
        if (len1 > len2) {
            len11 = len1 / 2;
            cut1 = first + len11;
            cut2 = E12_LowerBound(middle, last, cut1);
            len22 = (int)(cut2 - middle);
        } else {
            len22 = len2 / 2;
            cut2 = middle + len22;
            cut1 = E12_UpperBound(first, middle, cut2);
            len11 = (int)(cut1 - first);
        }
        Rotate12((BYTE*)cut1, (BYTE*)middle, (BYTE*)cut2);
        E12* mid = cut1 + len22;
        Rec12_MergeNoBuffer(first, cut1, mid, len11, len22, comp);
        first = mid;
        middle = cut2;
        len1 -= len11;
        len2 -= len22;
    }
}

// [FUN_470110] one merge pass: the runs of step records of [first, last) merged in pairs into out
void __cdecl Rec12_MergeLoop(E12* first, E12* last, E12* out, int step, int comp)
{
    while (last - first >= step * 2) {
        out = E12_Merge(first, first + step, first + step, first + step * 2, out);
        first += step * 2;
    }
    int s = (int)(last - first) < step ? (int)(last - first) : step;
    E12_Merge(first, first + s, first + s, last, out);
}

} // extern "C"

// === saving an order (FUN_470e10)

extern "C" void FUN_471d70(); KING_ALIAS("_FUN_471d70", "@PlayerRefs_Save@12")

// appends n bytes of p to the byte vector v
static void Os_Put(BYTE** v, const void* p, int n)
{
    BYTE t[8];
    memcpy(t, p, n);
    CharVec_InsertRange(v, 0, v[1], t, t + n);
}

extern "C" {

// [FUN_470e10] appends order o's fields to the byte vector v (+0, +12, +16, +48, +28, +24, the doubles +32,
// +40, +44, +52, +56, +64, +96, +72, +80, +88, +104, +112), its player references (+120), then +4 and +8
void __fastcall Order_Save(BYTE* o, void*, BYTE** v)
{
    static const signed char k_fields[][2] = {{0, 4}, {12, 4}, {16, 4}, {48, 4}, {28, 4}, {24, 4}, {32, 8},
        {40, 4}, {44, 4}, {52, 4}, {56, 8}, {64, 8}, {96, 8}, {72, 8}, {80, 8}, {88, 8}, {104, 4}, {112, 8}};
    for (const auto& f : k_fields)
        Os_Put(v, o + f[0], f[1]);
    CallThis(FUN_471d70, o + 120, v);
    Os_Put(v, o + 4, 4);
    Os_Put(v, o + 8, 4);
}

} // extern "C"

// === an order list destructor (FUN_470ae0)

extern "C" void FUN_447970(); KING_ALIAS("_FUN_447970", "@Vec4_FreeStorage@8")
extern "C" void Co_VecDtor(); KING_ALIAS("_Co_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")

extern "C" {

// [FUN_470ae0] destructor: the record at +192 (its byte vector at +40) deleted, the two vectors at +20 and the
// vector at +0 freed
void __fastcall Rec470ae0_Destroy(BYTE* s)
{
    if (BYTE* o = F(BYTE*, s, 192)) {
        if (int n = (int)(F(BYTE*, o, 48) - F(BYTE*, o, 40)))
            CallC(FUN_44b4a0, F(BYTE*, o, 40), n);
        operator delete(o);
    }
    CallStd(Co_VecDtor, s + 20, 12, 2, (void*)FUN_447970);
    if (int n = (int)(F(BYTE*, s, 8) - F(BYTE*, s, 0)))
        CallC(FUN_44b4a0, F(BYTE*, s, 0), n);
}

} // extern "C"
