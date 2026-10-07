// schitem.cpp - sch_item.cpp of the game, rewritten from king.masm: small
// functions of the AI script's items: numbers (a double at +16, a second
// part at +24 for complex values) and their arithmetic and comparisons,
// value lists, the deque<pointer> of the reader (128 pointers per block),
// character classes of the tokenizer, and constructors and destructors.
//
// Building with  nmake ASM_SCHITEM=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl _stricmp(const char* a, const char* b);
extern "C" void __cdecl free(void* p);

ASM_PROC(FUN_44b4a0)    // the AI's allocator: free (p, size)
ASM_PROC(FUN_4b2b60)
ASM_PROC(FUN_4b20d0)
ASM_PROC(FUN_4b58c0)
ASM_PROC(FUN_4b5490)
ASM_PROC(FUN_4b5b70)
extern "C" void __fastcall Stl_Lock(volatile LONG* l, void*);   // aimain.cpp

ASM_VAR(AsmLabel, g_vtbl64c044, $L_64c044)
ASM_VAR(AsmLabel, g_vtbl64bed0, $L_64bed0)
ASM_VAR(AsmLabel, g_vtbl64c054, $L_64c054)
ASM_VAR(AsmLabel, g_vtbl64c068, $L_64c068)
ASM_VAR(AsmLabel, g_vtbl64c0dc, $L_64c0dc)
ASM_VAR(AsmLabel, g_vtbl64c108, $L_64c108)
ASM_VAR(AsmLabel, g_vtbl64c118, $L_64c118)
ASM_VAR(AsmLabel, g_vtbl64bee4, $L_64bee4)
ASM_VAR(AsmLabel, g_vtbl64c128, $L_64c128)
ASM_VAR(int, g_smallAllocLock, $L_68d204)       // a lock object; 0 releases it
typedef void* FreeLists[1];
ASM_VAR(FreeLists, g_smallFreeLists, $L_68d1c0) // freed blocks by (size + 7) / 8

static void AiFree(void* p, int size) { CallC(FUN_44b4a0, p, size); }

// value: +0 the object (its reference count at +4), +8 flags (0x10000:
// immediate); obj, m_4 and flags are copied as the copy constructor does
static void CopyRef(BYTE* to, const BYTE* from)
{
    if (F(DWORD, from, 8) & 0x10000) {
        F(DWORD, to, 8) = F(DWORD, from, 8);
        F(BYTE*, to, 0) = F(BYTE*, from, 0);
        F(int, to, 4) = F(int, from, 4);
        return;
    }
    F(int, to, 4) = 0;
    if (F(BYTE*, from, 0)) {
        F(BYTE*, to, 0) = F(BYTE*, from, 0);
        F(DWORD, to, 8) = F(DWORD, from, 8);
        F(int, F(BYTE*, to, 0), 4)++;
    } else {
        F(BYTE*, to, 0) = 0;
        F(DWORD, to, 8) = 0;
    }
}

// list<value>::erase (20-byte nodes, the value at +8); *out gets the next
extern "C" BYTE** __stdcall ValueList_Erase(BYTE** out, BYTE* node)
{
    BYTE* prev = F(BYTE*, node, 4);
    BYTE* next = F(BYTE*, node, 0);
    F(BYTE*, prev, 0) = next;
    F(BYTE*, next, 4) = prev;
    if (!(F(DWORD, node, 16) & 0x10000) && F(BYTE*, node, 8)) {
        F(int, F(BYTE*, node, 8), 4)--;
        BYTE* o = F(BYTE*, node, 8);
        if (F(int, o, 4) == 0 && o)
            CallVirt(o, 12, 1);
    }
    AiFree(node, 20);
    *out = next;
    return out;
}

// ---------------------------------------------------------------------------
// Numbers: +8 bit 0 set for exact ones, +16 the value, +24 the imaginary part

extern "C" BYTE* __fastcall SchNumber_FromInt(BYTE* self, void*, int exact, int v)
{
    F(double, self, 16) = v;
    F(int, self, 24) = 0;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl64c044;
    F(int, self, 28) = 0;
    if (exact)
        F(int, self, 8) = 1;
    return self;
}

extern "C" BYTE* __fastcall SchNumber_FromUnsigned(BYTE* self, void*, int exact, unsigned v)
{
    F(int, self, 24) = 0;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(double, self, 16) = v;
    F(void*, self, 0) = g_vtbl64c044;
    F(int, self, 28) = 0;
    if (exact)
        F(int, self, 8) = 1;
    return self;
}

extern "C" double __fastcall SchNumber_Value(BYTE* self, void*)
{
    return F(double, self, 16);
}

extern "C" int __fastcall SchNumber_IsReal(BYTE* self, void*)
{
    return x87_eq(F(double, self, 24), 0.0) ? 1 : 0;
}

extern "C" int __fastcall SchNumber_IsExact(BYTE* self, void*)
{
    return F(DWORD, self, 8) & 1;
}

// [FUN_4b0ee0] with the number in ECX, as the original passes it through
extern "C" int __fastcall SchNumber_IsInexact(BYTE* self, void*)
{
    return SchNumber_IsExact(self, 0) == 0;
}

extern "C" void __fastcall SchNumber_Add(BYTE* self, void*, const BYTE* o)
{
    F(double, self, 16) = F(double, o, 16) + F(double, self, 16);
    F(double, self, 24) = F(double, o, 24) + F(double, self, 24);
}

extern "C" void __fastcall SchNumber_Sub(BYTE* self, void*, const BYTE* o)
{
    F(double, self, 16) = F(double, self, 16) - F(double, o, 16);
    F(double, self, 24) = F(double, self, 24) - F(double, o, 24);
}

extern "C" void __fastcall SchNumber_Mul(BYTE* self, void*, const BYTE* o)
{
    double re = F(double, self, 16), im = F(double, self, 24);
    double ore = F(double, o, 16), oim = F(double, o, 24);
    double i = oim * re + im * ore;
    F(double, self, 16) = ore * re - oim * im;
    F(double, self, 24) = i;
}

extern "C" void __fastcall SchNumber_Div(BYTE* self, void*, const BYTE* o)
{
    double re = F(double, self, 16);
    double ore = F(double, o, 16), oim = F(double, o, 24);
    double den = ore * ore + oim * oim;
    double im = (ore * F(double, self, 24) - re * oim) / den;
    F(double, self, 16) = (re * ore + oim * F(double, self, 24)) / den;
    F(double, self, 24) = im;
}

// comparisons of the values at +16, true also for NaN where the original's
// flag test was
extern "C" int __cdecl SchNumber_Eq(const BYTE* a, const BYTE* b) { return x87_eq(F(double, a, 16), F(double, b, 16)) ? 1 : 0; }
extern "C" int __cdecl SchNumber_Ne(const BYTE* a, const BYTE* b) { return x87_eq(F(double, a, 16), F(double, b, 16)) ? 0 : 1; }
extern "C" int __cdecl SchNumber_Lt(const BYTE* a, const BYTE* b) { return !(F(double, a, 16) >= F(double, b, 16)); }
extern "C" int __cdecl SchNumber_Gt(const BYTE* a, const BYTE* b) { return F(double, a, 16) > F(double, b, 16); }
extern "C" int __cdecl SchNumber_Le(const BYTE* a, const BYTE* b) { return !(F(double, a, 16) > F(double, b, 16)); }
extern "C" int __cdecl SchNumber_Ge(const BYTE* a, const BYTE* b) { return F(double, a, 16) >= F(double, b, 16); }

extern "C" DWORD* __cdecl Sch_Zero12(DWORD* out)
{
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    return out;
}

// an item linked into the owner's list at +4 (+16 of the owner the last one)
static void LinkIntoOwner(BYTE* self, BYTE* owner)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl64bed0;
    BYTE* last = F(BYTE*, owner, 16);
    F(BYTE*, self, 12) = last;
    F(BYTE*, self, 16) = owner + 4;
    F(BYTE*, last, 16) = self;
    F(BYTE*, F(BYTE*, self, 16), 12) = self;
}

extern "C" BYTE* __fastcall SchItem4b1090_Construct(BYTE* self, void*, BYTE* owner)
{
    LinkIntoOwner(self, owner);
    for (int i = 20; i <= 40; i += 4)
        F(int, self, i) = 0;
    F(void*, self, 0) = g_vtbl64c054;
    return self;
}

extern "C" BYTE* __fastcall SchItem4b6670_Construct(BYTE* self, void*, BYTE* owner)
{
    LinkIntoOwner(self, owner);
    F(int, self, 20) = 0;
    F(int, self, 24) = 0;
    F(int, self, 28) = 0;
    F(void*, self, 0) = g_vtbl64c128;
    return self;
}

extern "C" BYTE* __fastcall SchItem4b1390_Construct(BYTE* self, void*)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl64c068;
    return self;
}

// ---------------------------------------------------------------------------
// The reader's deque<pointer>: +8 cur, +12 first, +16 last, +20 node of the
// front; its iterators: cur, first, last, node

static void NextBlock(BYTE* self)
{
    AiFree(F(void*, self, 12), 512);
    F(BYTE**, self, 20) += 1;
    F(BYTE*, self, 12) = *F(BYTE**, self, 20);
    F(BYTE*, self, 16) = F(BYTE*, self, 12) + 512;
    F(BYTE*, self, 8) = F(BYTE*, self, 12);
}

extern "C" void __fastcall SchDeque_PopFront(BYTE* self, void*)
{
    if (F(BYTE*, self, 8) != F(BYTE*, self, 16) - 4)
        F(BYTE*, self, 8) += 4;
    else
        NextBlock(self);
}

struct PtrDequeIt {
    void** cur;
    void** first;
    void** last;
    void*** node;
};

static void Advance(PtrDequeIt* it, int n)
{
    int off = (int)(it->cur - it->first) + n;
    if (off >= 0 && off < 128) {
        it->cur += n;
        return;
    }
    int blocks = off > 0 ? off / 128 : -1 - (int)((unsigned)(-1 - off) >> 7);
    it->node += blocks;
    it->first = *it->node;
    it->last = it->first + 128;
    it->cur = it->first + (off - blocks * 128);
}

extern "C" PtrDequeIt* __fastcall SchDequeIt_Plus(const PtrDequeIt* self, void*, PtrDequeIt* out, int n)
{
    PtrDequeIt it = *self;
    Advance(&it, n);
    out->last = it.last;
    out->node = it.node;
    out->cur = it.cur;
    out->first = it.first;
    return out;
}

extern "C" PtrDequeIt* __fastcall SchDequeIt_Advance(PtrDequeIt* self, void*, int n)
{
    Advance(self, n);
    return self;
}

extern "C" void __fastcall SchDequeIt_SetNode(PtrDequeIt* self, void*, void*** node)
{
    self->node = node;
    self->first = *node;
    self->last = self->first + 128;
}

extern "C" void __fastcall SchDeque_NextBlock(BYTE* self, void*)
{
    NextBlock(self);
}

// ---------------------------------------------------------------------------
// Characters

extern "C" int __cdecl Sch_IsSpaceChar(int c)
{
    return (BYTE)c <= 32;
}

extern "C" int __cdecl Sch_IsSpaceOrEof(int c)
{
    return c <= 32 || c == -1;
}

extern "C" int __cdecl Sch_IsSymbolChar(int c)
{
    switch ((char)c) {
    case '|': case '!': case '$': case '%': case '&': case '*': case '+': case '-': case '.':
    case '/': case ':': case '<': case '=': case '>': case '?': case '@': case '^': case '_':
    case '~':
        return 1;
    }
    return 0;
}

// [FUN_4b2b60](1) of the reader, which the original takes from the second
// argument
extern "C" int __cdecl SchReader_4b2e80(int unused, BYTE* reader)
{
    return CallThis<int>(FUN_4b2b60, reader, 1);
}

extern "C" void __fastcall SchReader_Skip(BYTE* self, void*)
{
    CallThis(FUN_4b2b60, self, 1);
    if (F(BYTE*, self, 8) != F(BYTE*, self, 16) - 4)
        F(BYTE*, self, 8) += 4;
    else
        NextBlock(self);
    F(int, self, 68)++;
}

extern "C" void* __cdecl SchReader_4b3670(void* out, int a, BYTE* reader)
{
    SchReader_4b2e80(a, reader);
    CallC(FUN_4b20d0, out, F(int, reader, 72), reader);
    return out;
}

// ---------------------------------------------------------------------------
// Constructors and destructors

extern "C" BYTE* __fastcall SchItem4b5f30_Construct(BYTE* self, void*, int a, int unused)
{
    CallThis(FUN_4b58c0, self);
    F(void*, self, 0) = g_vtbl64c0dc;
    F(int, self, 12) = a;
    return self;
}

extern "C" void __fastcall SchItem4b5f50_Destruct(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64c0dc;
    if (self[16] & 1)
        CallThis(FUN_4b5490, self);
    CallThis(FUN_4b5b70, self);
}

static BYTE* Zero5(BYTE* self, void* vtbl)
{
    for (int i = 4; i <= 20; i += 4)
        F(int, self, i) = 0;
    F(void*, self, 0) = vtbl;
    return self;
}
extern "C" BYTE* __fastcall SchBuf4b6160_Construct(BYTE* self, void*) { return Zero5(self, g_vtbl64c108); }
extern "C" BYTE* __fastcall SchBuf4b63a0_Construct(BYTE* self, void*) { return Zero5(self, g_vtbl64c118); }

// frees the buffer at +12 (its end at +20): blocks up to 128 bytes go to the
// free lists of the small allocator
static void FreeBuffer(BYTE* self, void* vtbl)
{
    F(void*, self, 0) = vtbl;
    void** p = F(void**, self, 12);
    unsigned size = (unsigned)(F(BYTE*, self, 20) - (BYTE*)p);
    if (size) {
        if (size <= 128) {
            void** list = &g_smallFreeLists[(size + 7) >> 3];
            Stl_Lock((volatile LONG*)&g_smallAllocLock, 0);
            *p = *list;
            *list = p;
            g_smallAllocLock = 0;
        } else
            free(p);
    }
    F(void*, self, 0) = g_vtbl64bee4;
}
extern "C" void __fastcall SchBuf4b6340_Destruct(BYTE* self, void*) { FreeBuffer(self, g_vtbl64c108); }
extern "C" void __fastcall SchBuf4b6580_Destruct(BYTE* self, void*) { FreeBuffer(self, g_vtbl64c118); }

// whether v is a symbol (type 9) named name
extern "C" bool __cdecl SchValue_IsSymbol(const BYTE* v, const char* name)
{
    if (v[8] != 9)
        return false;
    return _stricmp(name, F(const char*, F(BYTE*, v, 0), 12)) == 0;
}

extern "C" int __cdecl SchValue_Same(const BYTE* a, const BYTE* b)
{
    BYTE* oa = F(BYTE*, a, 0);
    BYTE* ob = F(BYTE*, b, 0);
    if (oa == ob && F(int, a, 8) == F(int, b, 8) && F(int, a, 4) == F(int, b, 4))
        return 1;
    if (a[8] == 9 && b[8] == 9 && _stricmp(F(const char*, oa, 12), F(const char*, ob, 12)) == 0)
        return 1;
    return 0;
}

// uninitialized_fill_n of 12-byte values
extern "C" BYTE* __cdecl SchValues_Fill(BYTE* to, unsigned n, const BYTE* v)
{
    for (; n; n--, to += 12)
        if (to)
            CopyRef(to, v);
    return to;
}

extern "C" BYTE* __fastcall Sch_At(BYTE** self, void*, int i)
{
    return *self + i;
}
