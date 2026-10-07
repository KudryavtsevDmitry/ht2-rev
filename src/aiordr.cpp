// aiordr.cpp - small functions of ai_ordr.cpp and ai_svld.cpp of the game,
// rewritten from king.masm: the STL instances of the orders (12-byte
// entries: copies, fills, binary searches by the key at +8, reversal), the
// order record's constructor, its values, and helpers of the save files.
//
// Building with  nmake ASM_AIORDR=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "FUN_401000")
ASM_PROC(FUN_44c220)    // vector<char>::insert(pos, first, last)
ASM_PROC(FUN_470900)
ASM_PROC(FUN_44ac60)    // vector<pointer>: a new buffer (n, first, last)
ASM_PROC(FUN_44b4a0)    // the AI's allocator: free (p, size)

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

extern "C" BYTE* __fastcall SvRec480550_Construct(BYTE* self, void*, int a)
{
    CallThis(FUN_470900, self, a);
    for (int i = 16; i <= 48; i += 4)
        F(int, self, i) = 0;
    return self;
}

extern "C" BYTE* __fastcall SvRec480580_Construct(BYTE* self, void*, int a)
{
    CallThis(FUN_470900, self, a);
    F(int, self, 16) = 0;
    F(int, self, 20) = 0;
    return self;
}

// a saved time moved to the current clock (DBL_MAX stays)
extern "C" void __cdecl Save_RebaseTime(double* t)
{
    if (F(int, t, 0) == -1 && F(DWORD, t, 4) == 0x7fefffff)
        return;
    *t = AI_Now() - g_68d6c8 + *t;
}

// reads three doubles at *cursor
extern "C" void __cdecl Save_Read3Doubles(BYTE** cursor, DWORD* out)
{
    out[0] = F(DWORD, *cursor, 0);
    out[1] = F(DWORD, *cursor, 4);
    *cursor += 8;
    out[2] = F(DWORD, *cursor, 0);
    out[3] = F(DWORD, *cursor, 4);
    *cursor += 8;
    out[4] = F(DWORD, *cursor, 0);
    out[5] = F(DWORD, *cursor, 4);
    *cursor += 8;
}

// vector<pointer>::reserve
extern "C" void __fastcall PtrVec_Reserve(BYTE** self, void*, unsigned n)
{
    BYTE* first = self[0];
    if ((unsigned)((self[2] - first) >> 2) >= n)
        return;
    BYTE* last = self[1];
    int size = (int)(last - first) >> 2;
    BYTE* p = CallThis<BYTE*>(FUN_44ac60, self, n, first, last);
    int cap = (int)(self[2] - self[0]) >> 2;
    if (cap)
        CallC(FUN_44b4a0, self[0], cap * 4);
    self[0] = p;
    self[1] = p + size * 4;
    self[2] = p + n * 4;
}

// the first of the strings (vector<char>*) equal to name
extern "C" char*** __cdecl Strings_Find(char*** first, char*** last, const char* name)
{
    for (; first != last; first++) {
        char* s = (*first)[0];
        if (s != (*first)[1] && s && strcmp(s, name) == 0)
            break;
    }
    return first;
}
