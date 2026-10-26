// values.cpp - sch_item.cpp of the game, rewritten from king.masm: small
// functions of the AI script's items: numbers (a double at +16, a second
// part at +24 for complex values) and their arithmetic and comparisons,
// value lists, the deque<pointer> of the reader (128 pointers per block),
// character classes of the tokenizer, and constructors and destructors.
//
// Building with  nmake ASM_VALUES=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl _stricmp(const char* a, const char* b);
extern "C" void __cdecl free(void* p);

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // the AI's allocator: free (p, size)
extern "C" void FUN_4b2b60(); KING_ALIAS("_FUN_4b2b60", "@Reader_Fill@12")
extern "C" void FUN_4b20d0(); KING_ALIAS("_FUN_4b20d0", "_Sch_ParseNumber")
extern "C" void FUN_4b58c0(); KING_ALIAS("_FUN_4b58c0", "@SchState_Construct@8")
extern "C" void FUN_4b5490(); KING_ALIAS("_FUN_4b5490", "@SchReader_Close@8")
extern "C" void FUN_4b5b70(); KING_ALIAS("_FUN_4b5b70", "@FilePort_Destroy@4")
extern "C" void FUN_4ad2f0(); KING_ALIAS("_FUN_4ad2f0", "_Pool32_New")
extern "C" void FUN_4b0be0(); KING_ALIAS("_FUN_4b0be0", "@Number_ConstructComplex@20")
extern "C" void FUN_4b1390(); KING_ALIAS("_FUN_4b1390", "@SchItem4b1390_Construct@8")
extern "C" void FUN_4abf10(); KING_ALIAS("_FUN_4abf10", "@CharDeque_Construct@12")
ASM_VAR(AsmLabel, s_tNumber, $L_66ca60)
ASM_VAR(AsmLabel, g_vtbl64c0b0, $L_64c0b0)
extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")
extern "C" void FUN_4b63a0(); KING_ALIAS("_FUN_4b63a0", "@SchBuf4b63a0_Construct@8")
extern "C" void FUN_4b6500(); KING_ALIAS("_FUN_4b6500", "@SchSymbol_Construct@12")
extern "C" void FUN_48c540(); KING_ALIAS("_FUN_48c540", "_BigInt_Negate")
extern "C" void FUN_48acd0(); KING_ALIAS("_FUN_48acd0", "_WStr_Assign")
ASM_VAR(AsmLabel, s_tIdentifier, $L_66ce4c)
ASM_VAR(AsmLabel, g_vtblString, $L_64c108)
ASM_VAR(AsmLabel, g_vtblSymbol, $L_64c118)
ASM_VAR(AsmLabel, s_6703f0, $L_6703f0)
ASM_VAR(AsmLabel, g_vtbl64c054, $L_64c054)
ASM_VAR(AsmLabel, g_vtbl64bed0, $L_64bed0)
ASM_VAR(AsmLabel, g_vtbl64bee4, $L_64bee4)
extern "C" DWORD* __cdecl SchHandle_Take(DWORD* out, BYTE* o, int type, const char* name);  // evaluator.cpp
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
KING_BSS(int, g_smallAllocLock, 0x68d204)       // a lock object; 0 releases it
typedef void* FreeLists[1];
KING_BSS(FreeLists, g_smallFreeLists, 0x68d1c0) // freed blocks by (size + 7) / 8

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

// ---------------------------------------------------------------------------
// medium-sized functions: a node's constructor, the reader's line buffer and
// its error line

extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert(pos, first, last)
extern "C" void FUN_44bef0(); KING_ALIAS("_FUN_44bef0", "@VecChar_Insert44bef0@16")    // vector<char>::insert(pos, &c)
extern "C" void FUN_4a20a0(); KING_ALIAS("_FUN_4a20a0", "@DequeDw_PushBackAux@12")    // deque<int>::push_back when the block is full
extern "C" void FUN_4a52e0(); KING_ALIAS("_FUN_4a52e0", "_Sch_ErrorCode")    // script error (code, fatal)
ASM_VAR(AsmLabel, g_fmtLineCol, $L_671458)      // the "line col" prefix of an error line

// a node linked between p's +16 and +4, holding two values
extern "C" BYTE* __fastcall Node4b1120_Construct(BYTE* self, void*, BYTE* p, const BYTE* a, const BYTE* b)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl64bed0;
    BYTE* next = F(BYTE*, p, 16);
    F(BYTE*, self, 12) = next;
    F(BYTE*, self, 16) = p + 4;
    F(BYTE*, next, 16) = self;
    F(BYTE*, F(BYTE*, self, 16), 12) = self;
    CopyRef(self + 20, a);
    CopyRef(self + 32, b);
    F(void*, self, 0) = g_vtbl64c054;
    return self;
}

static void PushChar(BYTE* v, char c)
{
    char* end = F(char*, v, 4);
    if (end == F(char*, v, 8))
        CallThis(FUN_44bef0, v, end, &c);
    else {
        if (end)
            *end = c;
        F(char*, v, 4) = end + 1;
    }
}

// the reader's error line: "line col" prefix, the line, a caret under the column
extern "C" void __fastcall Reader_ErrorLine(BYTE* self, void*, BYTE* out)
{
    char buf[128];
    sprintf(buf, g_fmtLineCol, F(int, self, 64), F(int, self, 68));
    CallThis(FUN_44c220, out, F(char*, out, 4), buf, buf + strlen(buf));
    CallThis(FUN_44c220, out, F(char*, out, 4), F(char*, self, 52), F(char*, self, 56));
    PushChar(out, '\n');
    for (int i = 0; i < F(int, self, 68) - 1; i++)
        PushChar(out, ' ');
    PushChar(out, '^');
    PushChar(out, '\n');
    PushChar(out, 0);
}

// reads lines into the reader's deque until it holds n characters; the
// current line is kept at +52 for error messages; returns the count
extern "C" unsigned __fastcall Reader_Fill(BYTE* self, void*, unsigned n)
{
    for (;;) {
        unsigned count = ((F(BYTE*, self, 24) - F(BYTE*, self, 28)) >> 2)
                       + ((((F(BYTE*, self, 36) - F(BYTE*, self, 20)) >> 2) + 0x1ffffff) << 7)
                       + ((F(BYTE*, self, 16) - F(BYTE*, self, 8)) >> 2);
        if (count >= n)
            return count;
        F(char*, self, 56) = F(char*, self, 52);
        F(int, self, 68) = 0;
        F(int, self, 64)++;
        int c;
        do {
            if (!F(BYTE*, self, 40))
                CallC(FUN_4a52e0, 0, 1);
            if (F(DWORD, self, 48) & 0x10000)
                CallC(FUN_4a52e0, 4, 1);
            c = CallVirt<int>(F(BYTE*, self, 40), 20);
            int* end = F(int*, self, 24);
            if (end == (int*)(F(BYTE*, self, 32) - 4))
                CallThis(FUN_4a20a0, self, &c);
            else {
                if (end)
                    *end = c;
                F(int*, self, 24) = end + 1;
            }
            PushChar(self + 52, (char)c);
        } while (c != 10 && c != -1);
    }
}

// a caret line for an error: CR, LF, then spaces up to the column; each
// call takes the value by copy
extern "C" void FUN_4b13c0(); KING_ALIAS("_FUN_4b13c0", "_Port_WriteChar")    // writes a character (c, value by copy)
extern "C" void FUN_4b1450(); KING_ALIAS("_FUN_4b1450", "_Port_WriteString")    // writes a string (text, value by copy)
ASM_VAR(AsmLabel, g_strSpace, $L_6715b8)
struct ValueArg { BYTE* obj; int m_4; DWORD flags; };

static ValueArg ValueCopy(const BYTE* v)
{
    ValueArg a;
    CopyRef((BYTE*)&a, v);
    return a;
}

extern "C" void __cdecl Sch_CaretLine(const BYTE* v, const BYTE* pos)
{
    typedef void (__cdecl* PutC)(int, ValueArg);
    typedef void (__cdecl* PutS)(const char*, ValueArg);
    ((PutC)FUN_4b13c0)(13, ValueCopy(v));
    ((PutC)FUN_4b13c0)(10, ValueCopy(v));
    for (int i = 0; i < F(int, pos, 4) - 1; i++)
        ((PutS)FUN_4b1450)(g_strSpace, ValueCopy(v));
}

// Erase a range of the Scheme input's value list, retaining the stop node.
extern "C" BYTE** __fastcall SchInputList_EraseRange(void*, void*, BYTE** out,
                                                     BYTE* first, BYTE* last)
{
    while (first != last) {
        BYTE* node = first;
        first = F(BYTE*, node, 0);
        BYTE* ignored;
        ValueList_Erase(&ignored, node);
    }
    *out = last;
    return out;
}

// --- functions with exception frames (the frames only rethrow)

static void SchRef_Release(DWORD* v)
{
    if (!(v[2] & 0x10000) && v[0]) {
        BYTE* o = (BYTE*)v[0];
        F(int, o, 4) = F(int, o, 4) - 1;
        if (!F(int, (BYTE*)v[0], 4) && v[0])
            CallVirt((BYTE*)v[0], 12, 1);
    }
}

static BYTE* NewIdentifier(DWORD name)
{
    BYTE* o = (BYTE*)operator new(24);
    return o ? CallThis<BYTE*>(FUN_4b6500, o, name) : 0;
}

static void Port_CheckWrite(BYTE* o, DWORD flags, int c)
{
    if (!o)
        CallC(FUN_4a52e0, 0, 1);
    if (flags & 0x10000)
        CallC(FUN_4a52e0, 4, 1);
    CallVirt(o, 28, c);
}

static BYTE* SchText_Construct(BYTE* self, const char* s, void* vtbl)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    F(int, self, 20) = 0;
    F(void*, self, 0) = vtbl;
    CallThis(FUN_44c220, self + 12, F(BYTE*, self, 16), s, s + strlen(s) + 1);
    return self;
}

extern "C" {

// [FUN_4b5060] a new unnamed identifier
DWORD* __cdecl Sch_MakeIdentifierNew(DWORD* out)
{
    BYTE* o = (BYTE*)operator new(24);
    BYTE* id = o ? CallThis<BYTE*>(FUN_4b63a0, o) : 0;
    return SchHandle_Take(out, id, 9, s_tIdentifier);
}

// [FUN_4b5140] a new identifier from *name
DWORD* __cdecl Sch_MakeIdentifierQ(DWORD* out, int unused, const DWORD* name)
{
    return SchHandle_Take(out, NewIdentifier(*name), 9, s_tIdentifier);
}

// [FUN_4b5220]
DWORD* __cdecl Sch_MakeIdentifierC(DWORD* out, int unused, DWORD name)
{
    return SchHandle_Take(out, NewIdentifier(name), 9, s_tIdentifier);
}

// [FUN_4b5300]
DWORD* __cdecl Sch_MakeIdentifierD(DWORD* out, int unused, DWORD name)
{
    return SchHandle_Take(out, NewIdentifier(name), 9, s_tIdentifier);
}

// [FUN_4b62c0] string object constructor (copies s with its terminator)
BYTE* __fastcall SchString_Construct(BYTE* self, void*, const char* s)
{
    return SchText_Construct(self, s, g_vtblString);
}

// [FUN_4b6500] symbol object constructor
BYTE* __fastcall SchSymbol_Construct(BYTE* self, void*, const char* s)
{
    return SchText_Construct(self, s, g_vtblSymbol);
}

// [FUN_4b13c0] writes the character c to the port h (passed by value)
void __cdecl Port_WriteChar(int c, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    Port_CheckWrite((BYTE*)h0, h2, c);
    SchRef_Release(h);
}

// [FUN_4b1450] writes the string s to the port h (passed by value)
void __cdecl Port_WriteString(const char* s, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    for (; *s; s++)
        Port_CheckWrite((BYTE*)h0, h2, (char)*s);
    SchRef_Release(h);
}

// [FUN_4b2050]
DWORD* __cdecl Sch_Convert4b2050(DWORD* out, const DWORD* x)
{
    BYTE* t = CallC<BYTE*>(FUN_48c540, x[0], (const char*)s_6703f0);
    *out = CallC<DWORD>(FUN_48acd0, 0, t);
    if (t && F(WORD, t, 2))
        operator delete(t);
    return out;
}

// [FUN_4b11f0] destructor: two values, then unlinks from its list
void __fastcall Sch4b11f0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64c054;
    SchRef_Release((DWORD*)(self + 32));
    SchRef_Release((DWORD*)(self + 20));
    F(void*, self, 0) = g_vtbl64bed0;
    if (F(BYTE*, self, 12))
        F(BYTE*, F(BYTE*, self, 12), 16) = F(BYTE*, self, 16);
    if (F(BYTE*, self, 16))
        F(BYTE*, F(BYTE*, self, 16), 12) = F(BYTE*, self, 12);
    F(void*, self, 0) = g_vtbl64bee4;
}

}

// the SGI subtractive random number generator: 55 words, then the indexes
static void SubtractiveRng_Init(DWORD* t, DWORD seed)
{
    memset(t, 0, 55 * 4);
    DWORD mj = seed;
    DWORD mk = 1;
    t[54] = mj;
    for (unsigned i = 0; i < 1134; i += 21) {
        unsigned ii = i % 55;
        DWORD next = mj - mk;
        t[ii] = mk;
        mj = t[ii];
        mk = next;
    }
    for (int k = 0; k < 4; k++)
        for (unsigned i = 0; i < 55; i++)
            t[i] -= t[(i + 31) % 55];
    t[55] = 0;
    t[56] = 31;
}

extern "C" {

// [FUN_4b2880] a new number from (*a, b, c)
DWORD* __cdecl Sch_MakeNumber3(DWORD* out, int unused, const DWORD* a, DWORD b, DWORD c)
{
    BYTE* o = CallC<BYTE*>(FUN_4ad2f0, 32);
    BYTE* n = o ? CallThis<BYTE*>(FUN_4b0be0, o, *a, b, c) : 0;
    return SchHandle_Take(out, n, 6, (const char*)s_tNumber);
}

// [FUN_4b58c0] the interpreter state: base, value tables, its random generator
BYTE* __fastcall SchState_Construct(BYTE* self, void*)
{
    CallThis(FUN_4b1390, self);
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    F(int, self, 20) = 0;
    memset(self + 24, 0, 40);
    CallThis(FUN_4abf10, self + 24, 0);
    F(BYTE, self, 64) = 0;
    SubtractiveRng_Init((DWORD*)(self + 68), 161803398);
    F(void*, self, 0) = g_vtbl64c0b0;
    return self;
}

}

extern "C" void FUN_4a1f10(); KING_ALIAS("_FUN_4a1f10", "@IntDeque_Construct@12")
extern "C" void FUN_4a36c0(); KING_ALIAS("_FUN_4a36c0", "@SchHandle_Release@8")

extern "C" {

// [FUN_4b4e10] constructor: the value h (by value) at +40, a at +72
BYTE* __fastcall Item4b4e10_Construct(BYTE* self, void*, int a, DWORD h0, DWORD h1, DWORD h2)
{
    DWORD h[3] = {h0, h1, h2};
    for (int i = 0; i < 10; i++)
        F(int, self, i * 4) = 0;
    CallThis(FUN_4a1f10, self, 0);
    DWORD* d = (DWORD*)(self + 40);
    if (h2 & 0x10000) {
        d[2] = h2;
        d[0] = h0;
        d[1] = h1;
    } else {
        d[1] = 0;
        if (h0) {
            d[0] = h0;
            d[2] = h2;
            F(int, (BYTE*)h0, 4)++;
        } else {
            d[0] = 0;
            d[2] = 0;
        }
    }
    for (int i = 13; i < 18; i++)
        F(int, self, i * 4) = 0;
    F(int, self, 72) = a;
    SchRef_Release(h);
    return self;
}

// [FUN_4b6840] destructor: the values at +20, then unlinks itself
void __fastcall Item4b6840_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &g_vtbl64c128;
    BYTE** v = (BYTE**)(self + 20);
    for (BYTE* e = v[0]; e != v[1]; e += 12)
        CallThis(FUN_4a36c0, e);
    int n = (v[2] - v[0]) / 12;
    if (n)
        AiFree(v[0], n * 12);
    F(void*, self, 0) = &g_vtbl64bed0;
    if (F(BYTE*, self, 12))
        F(BYTE*, F(BYTE*, self, 12), 16) = F(BYTE*, self, 16);
    if (F(BYTE*, self, 16))
        F(BYTE*, F(BYTE*, self, 16), 12) = F(BYTE*, self, 12);
    F(void*, self, 0) = &g_vtbl64bee4;
}

}

extern "C" void* __cdecl Stl_Allocate(unsigned bytes);
extern "C" void* __cdecl memmove(void* d, const void* s, size_t n);

// a list node (vtbl, two links) owning a copy of the bytes of src
static BYTE* BytesNode_Construct(BYTE* self, BYTE* const* src, void* vtbl)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = &g_vtbl64bee4;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    F(int, self, 20) = 0;
    int n = src[1] - src[0];
    BYTE* p = n ? (BYTE*)Stl_Allocate(n) : 0;
    F(BYTE*, self, 12) = p;
    F(BYTE*, self, 16) = p;
    F(BYTE*, self, 20) = p + n;
    memmove(p, src[0], src[1] - src[0]);
    F(BYTE*, self, 16) = p + (src[1] - src[0]);
    F(void*, self, 0) = vtbl;
    return self;
}

extern "C" {

// [FUN_4b61c0]
BYTE* __fastcall BytesNode4b61c0_Construct(BYTE* self, void*, BYTE* const* src) { return BytesNode_Construct(self, src, &g_vtbl64c108); }

// [FUN_4b6400]
BYTE* __fastcall BytesNode4b6400_Construct(BYTE* self, void*, BYTE* const* src) { return BytesNode_Construct(self, src, &g_vtbl64c118); }

}

extern "C" void FUN_4b6f70(); KING_ALIAS("_FUN_4b6f70", "_Sch_Write")

struct Value3v { BYTE* obj; int len; DWORD flags; };

extern "C" {

// [FUN_4b8070] out = the value FUN_4b6f70 makes of a, h1 and h2 (by value)
DWORD* __cdecl Sch4b8070(DWORD* out, int a, Value3v h1, Value3v h2)
{
    int t = a;
    DWORD r[4];
    r[0] = 0;
    CallC(FUN_4b6f70, r, &h1, &h2, &t);
    CopyRef((BYTE*)out, (const BYTE*)(r + 1));
    SchRef_Release(r + 1);
    SchRef_Release((DWORD*)&h1);
    SchRef_Release((DWORD*)&h2);
    return out;
}

}

extern "C" void FUN_4b1390(); KING_ALIAS("_FUN_4b1390", "@SchItem4b1390_Construct@8")
extern "C" void FUN_4abf10(); KING_ALIAS("_FUN_4abf10", "@CharDeque_Construct@12")    // deque<char> constructor (n)
extern "C" void FUN_4b54b0(); KING_ALIAS("_FUN_4b54b0", "@FilePort_ReadHeader@4")    // file port: reads the file
extern "C" struct _iobuf* __cdecl fopen(const char* name, const char* mode);

extern "C" {

// [FUN_4b59f0] file port constructor: mode 1 reads, 2 writes; seeds its
// subtractive random generator (Knuth's, seed 161803398)
BYTE* __fastcall FilePort_Construct(BYTE* self, void*, const char* name, unsigned mode)
{
    CallThis(FUN_4b1390, self);
    F(int, self, 12) = 0;
    F(int, self, 16) = 1;
    F(int, self, 20) = 0;
    for (int i = 0; i < 10; i++)
        F(int, self, 24 + i * 4) = 0;
    CallThis(FUN_4abf10, self + 24, 0);
    DWORD* t = (DWORD*)(self + 68);
    F(BYTE, self, 64) = 0;
    for (int i = 0; i < 55; i++)
        t[i] = 0;
    DWORD mj = 161803398, mk = 1;
    t[54] = mj;
    for (unsigned k = 0; k < 1134; k += 21) {
        unsigned ii = k % 55;
        DWORD next = mj - mk;
        t[ii] = mk;
        mj = t[ii];
        mk = next;
    }
    for (int r = 0; r < 4; r++)
        for (unsigned i = 0; i < 55; i++)
            t[i] -= t[(i + 31) % 55];
    t[55] = 0;
    t[56] = 31;
    F(void*, self, 0) = &g_vtbl64c0b0;
    if (mode & 2)
        F(void*, self, 12) = fopen(name, "wb");
    if (mode & 1)
        F(void*, self, 12) = fopen(name, "rb");
    if (!F(void*, self, 12))
        F(DWORD, self, 20) |= 1;
    else if (mode & 1)
        CallThis(FUN_4b54b0, self);
    return self;
}

}

ASM_VAR(char, g_fmtG4b0c70, $L_671350)      // "%g"
#include <string.h>
extern "C" void __cdecl Byte_Construct(BYTE* p, const BYTE* v);
extern "C" int __cdecl toupper(int c);
extern "C" void FUN_44bef0(); KING_ALIAS("_FUN_44bef0", "@VecChar_Insert44bef0@16")    // vector<char>::insert(pos, &c)
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert(pos, first, last)

static void Chars_PushBack(BYTE** v, char c)
{
    if (v[1] != v[2]) {
        Byte_Construct(v[1], (BYTE*)&c);
        v[1]++;
    } else
        CallThis(FUN_44bef0, v, v[1], &c);
}

// marks the exponent: '#' before the first 'e' or 'E' of the text
static void Chars_MarkExponent(BYTE** v)
{
    for (BYTE* p = v[0]; p != v[1]; p++)
        if (toupper((char)*p) == 'E') {
            char c = '#';
            if (v[1] != v[2] && p == v[1]) {
                *p = '#';
                v[1]++;
            } else
                CallThis(FUN_44bef0, v, p, &c);
            return;
        }
}

// [FUN_4b0c70] writes a number as script text: "#i" when exact, the real part, "@" and the imaginary part, a closing 0
extern "C" void __fastcall Number_Write(BYTE* s, void*, BYTE** v)
{
    char buf[128];
    if (F(BYTE, s, 8) & 1) {
        Chars_PushBack(v, '#');
        Chars_PushBack(v, 'i');
    }
    sprintf(buf, &g_fmtG4b0c70, F(double, s, 16));
    CallThis(FUN_44c220, v, v[1], buf, buf + strlen(buf));
    Chars_MarkExponent(v);
    if (F(double, s, 24) < 0.0 || F(double, s, 24) > 0.0) {
        Chars_PushBack(v, '@');
        sprintf(buf, &g_fmtG4b0c70, F(double, s, 24));
        CallThis(FUN_44c220, v, v[1], buf, buf + strlen(buf));
        Chars_MarkExponent(v);
    }
    Chars_PushBack(v, 0);
}

extern "C" void FUN_48a7c0(); KING_ALIAS("_FUN_48a7c0", "_Rational_ToDouble")    // a big integer's value as a double (digits, end)

// [FUN_4b0be0] constructs a complex number from two big integers (exact when asked)
extern "C" BYTE* __fastcall Number_ConstructComplex(BYTE* s, void*, int exact, BYTE* re, BYTE* im)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = &g_vtbl64c044;
    F(double, s, 16) = CallC<double>(FUN_48a7c0, re, re + 4);
    F(double, s, 24) = CallC<double>(FUN_48a7c0, im, im + 4);
    if (exact)
        F(DWORD, s, 8) |= 1;
    return s;
}

ASM_VAR(char, vt_64bee4, $L_64bee4)
ASM_VAR(char, g_6703f8, $L_6703f8)
extern "C" void FUN_48ade0(); KING_ALIAS("_FUN_48ade0", "_BigInt_FromInt")

// [FUN_4b13b0] the script node base destructor
extern "C" void __fastcall SchNode_Destroy(void** self, int)
{
    self[0] = &vt_64bee4;
}


// [FUN_4b5490] closes the reader's file
extern "C" void __fastcall SchReader_Close(BYTE* self, int)
{
    FILE* f = *(FILE**)(self + 12);
    if (f) {
        fclose(f);
        *(FILE**)(self + 12) = 0;
    }
}

// [FUN_4b26b0] a symbol value: (interned name, symbol type)
extern "C" void** __fastcall SchSymbol_Init(void** self, int, const char* name)
{
    self[0] = (void*)CallC<int>(FUN_48ade0, 0, name);
    self[1] = &g_6703f8;
    return self;
}

// ---------------------------------------------------------------------------
static __declspec(noinline) int Values_Trunc(double x)
{
    return (int)x;
}

// [FUN_4b0ea0] the number at +16 truncated
extern "C" int __fastcall Value_ToInt(BYTE* self, void*)
{
    return Values_Trunc(*(double*)(self + 16));
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_4b5b70(); KING_ALIAS("_FUN_4b5b70", "@FilePort_Destroy@4")
extern "C" void FUN_4b5f50(); KING_ALIAS("_FUN_4b5f50", "@SchItem4b5f50_Destruct@8")
extern "C" void FUN_4b6340(); KING_ALIAS("_FUN_4b6340", "@SchBuf4b6340_Destruct@8")
extern "C" void FUN_4b6580(); KING_ALIAS("_FUN_4b6580", "@SchBuf4b6580_Destruct@8")
extern "C" void FUN_4b6840(); KING_ALIAS("_FUN_4b6840", "@Item4b6840_Destroy@8")
extern "C" void FUN_4b11f0(); KING_ALIAS("_FUN_4b11f0", "@Sch4b11f0_Destroy@8")
extern "C" void FUN_4ad230(); KING_ALIAS("_FUN_4ad230", "_SchPool1_Free")

static BYTE* Val_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_4b59d0]
extern "C" BYTE* __fastcall Val4b59d0_DeletingDestructor(BYTE* s, void*, int flags) { return Val_DeleteAfter(s, flags, FUN_4b5b70); }
// [$L_4b5f10]
extern "C" BYTE* __fastcall Val4b5f10_DeletingDestructor(BYTE* s, void*, int flags) { return Val_DeleteAfter(s, flags, FUN_4b5f50); }
// [$L_4b61a0]
extern "C" BYTE* __fastcall Val4b61a0_DeletingDestructor(BYTE* s, void*, int flags) { return Val_DeleteAfter(s, flags, FUN_4b6340); }
// [$L_4b63e0]
extern "C" BYTE* __fastcall Val4b63e0_DeletingDestructor(BYTE* s, void*, int flags) { return Val_DeleteAfter(s, flags, FUN_4b6580); }
// [$L_4b66d0]
extern "C" BYTE* __fastcall Val4b66d0_DeletingDestructor(BYTE* s, void*, int flags) { return Val_DeleteAfter(s, flags, FUN_4b6840); }
ASM_VAR(AsmLabel, s_val66cb08, $L_66cb08)
ASM_VAR(AsmLabel, s_val66ca58, $L_66ca58)
ASM_VAR(AsmLabel, s_val66ce4c, $L_66ce4c)
ASM_VAR(AsmLabel, s_val6708e4, $L_6708e4)
extern "C" void Val_putc(); KING_ALIAS("_Val_putc", "_putc")

// [$L_4b10e0] type ids and names
extern "C" int __fastcall Pair_TypeId(BYTE*) { return 2; }
// [$L_4b10f0]
extern "C" const char* __fastcall Pair_TypeName(BYTE*) { return s_val66cb08; }
// [$L_4b6180]
extern "C" int __fastcall String_TypeId(BYTE*) { return 3; }
// [$L_4b6190]
extern "C" const char* __fastcall String_TypeName(BYTE*) { return s_val66ca58; }
// [$L_4b63c0]
extern "C" int __fastcall Identifier_TypeId(BYTE*) { return 9; }
// [$L_4b63d0]
extern "C" const char* __fastcall Identifier_TypeName(BYTE*) { return s_val66ce4c; }
// [$L_4b66b0]
extern "C" int __fastcall Val4b66b0_TypeId(BYTE*) { return 10; }
// [$L_4b66c0]
extern "C" const char* __fastcall Val4b66c0_TypeName(BYTE*) { return s_val6708e4; }

// [$L_4b5890] port: +20
extern "C" int __fastcall Port_Get20(BYTE* s) { return F(int, s, 20); }
// [$L_4b5880] port: bit 1 of the flags
extern "C" int __fastcall Port_Flag2(BYTE* s) { return F(int, s, 16) & 2; }
// [$L_4b5f00]
extern "C" int __fastcall Port_None(BYTE*) { return -1; }
// [$L_4b58a0] port: writes a character to its file (+12)
extern "C" int __fastcall Port_PutChar(BYTE* s, void*, char c) { return CallC<int>(Val_putc, (int)c, F(void*, s, 12)); }

// [$L_4b1100] pairs come from the value pool and go back to it
extern "C" BYTE* __fastcall Pair_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_4b11f0, s);
    if (flags & 1)
        CallC(FUN_4ad230, s);
    return s;
}

// ---------------------------------------------------------------------------
// Static initializers of random generators.

KING_BSS(AsmLabel, g_valRng694748, 0x694748)
KING_BSS(DWORD, g_valRng694820, 0x694820)
KING_BSS(DWORD, g_valRng694824, 0x694824)
KING_BSS(DWORD, g_valRng694828, 0x694828)

// the subtractive random generator's start (Knuth): 55 words at a, then the seed and the two indices
static void Val_RngInit(DWORD* a, DWORD* seed, DWORD* i1, DWORD* i2)
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

// [$L_4b53e0]
extern "C" void __cdecl ValInit_4b53e0() { Val_RngInit((DWORD*)g_valRng694748, &g_valRng694820, &g_valRng694824, &g_valRng694828); }

extern "C" void FUN_4b2b60(); KING_ALIAS("_FUN_4b2b60", "@Reader_Fill@12")
extern "C" void FUN_4b2ca0(); KING_ALIAS("_FUN_4b2ca0", "_Reader_SkipBlanks")
extern "C" void FUN_4b2e90(); KING_ALIAS("_FUN_4b2e90", "_Reader_ListTail")
// [FUN_4b3240] reads the next item of reader r into out: drops the current character (the deque at +8: cur, first
// +12, last +16, node +20; +68 the position), refills, then 4b2ca0 and 4b2e90
extern "C" int* __cdecl Reader_NextItem(int* out, int a, BYTE* r)
{
    CallThis(FUN_4b2b60, r, 1);
    if (F(BYTE*, r, 8) != F(BYTE*, r, 16) - 4)
        F(BYTE*, r, 8) += 4;
    else {
        free(F(void*, r, 12));
        F(BYTE**, r, 20)++;
        F(BYTE*, r, 12) = *F(BYTE**, r, 20);
        F(BYTE*, r, 16) = F(BYTE*, r, 12) + 512;
        F(BYTE*, r, 8) = F(BYTE*, r, 12);
    }
    F(int, r, 68)++;
    CallThis(FUN_4b2b60, r, 1);
    CallC(FUN_4b2ca0, a, r);
    CallC(FUN_4b2e90, out, a, r);
    return out;
}

extern "C" void FUN_4abe80(); KING_ALIAS("_FUN_4abe80", "@SchItem_Value@12")
ASM_PROC(FUN_4ac0b0)
struct Val207_It { char* cur; char* first; char* last; char** node; };
extern "C" char* __cdecl fgets(char*, int, void*);
// [$L_4b5e60] the next character of file reader s (-1 at the end): when its buffer (the deque at +24, current +32,
// end +48) is empty, the next line of the file (+12) is appended; at the end of file +16 gets bit 2
extern "C" int __fastcall FileReader_Peek(BYTE* s)
{
    char buf[4096];
    if (F(char*, s, 48) == F(char*, s, 32)) {
        if (fgets(buf, 4096, F(void*, s, 12))) {
            Val207_It end;
            CallThis(FUN_4abe80, s + 24, &end);
            CallThis(FUN_4ac0b0, s + 24, end, buf, buf + strlen(buf));
        } else
            F(int, s, 16) |= 2;
    }
    if (F(char*, s, 48) != F(char*, s, 32))
        return *F(BYTE*, s, 32);
    return -1;
}

extern "C" size_t __cdecl fread(void*, size_t, size_t, void*);
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
// [$L_4b5790] FileReader_Peek for a block file: reads up to 4096 bytes, deciphered when +64 is set (each byte xored
// with the low byte of the next value of the reader's subtractive generator: table +68, indices +288, +292)
extern "C" int __fastcall CipherReader_Peek(BYTE* s)
{
    BYTE buf[4096];
    if (F(char*, s, 48) == F(char*, s, 32)) {
        int n = (int)fread(buf, 1, 4096, F(void*, s, 12));
        if (F(BYTE, s, 64)) {
            DWORD* t = (DWORD*)(s + 68);
            for (int i = 0; i < n; i++) {
                unsigned a = (F(unsigned, s, 288) + 1) % 55;
                unsigned b = (F(unsigned, s, 292) + 1) % 55;
                F(unsigned, s, 288) = a;
                F(unsigned, s, 292) = b;
                t[a] -= t[b];
                buf[i] ^= (BYTE)t[F(unsigned, s, 288)];
            }
        }
        if (n) {
            Val207_It end;
            CallThis(FUN_4abe80, s + 24, &end);
            CallThis(FUN_4ac0b0, s + 24, end, buf, buf + n);
        } else
            F(int, s, 16) |= 2;
    }
    if (F(char*, s, 48) != F(char*, s, 32))
        return *F(BYTE*, s, 32);
    return -1;
}
// [$L_4b5d80] the next character of file reader s, consumed (-1 at the end): a line is read when the buffer is
// empty; a used-up 512-byte page of the buffer goes back to the pool
extern "C" int __fastcall FileReader_Get(BYTE* s)
{
    char buf[4096];
    if (F(char*, s, 48) == F(char*, s, 32) && fgets(buf, 4096, F(void*, s, 12))) {
        Val207_It end;
        CallThis(FUN_4abe80, s + 24, &end);
        CallThis(FUN_4ac0b0, s + 24, end, buf, buf + strlen(buf));
    }
    if (F(char*, s, 48) == F(char*, s, 32))
        return -1;
    int c = *F(BYTE*, s, 32);
    if (F(char*, s, 32) != F(char*, s, 40) - 1) {
        F(char*, s, 32)++;
        return c;
    }
    CallC(FUN_44b4a0, F(void*, s, 36), 512);
    F(char**, s, 44)++;
    F(char*, s, 36) = *F(char**, s, 44);
    F(char*, s, 40) = F(char*, s, 36) + 512;
    F(char*, s, 32) = F(char*, s, 36);
    return c;
}

// [$L_4b5640] the next character of cipher reader s, consumed (-1 at the end): CipherReader_Peek's refill, then
// FileReader_Get's step through the buffer pages
extern "C" int __fastcall CipherReader_Get(BYTE* s)
{
    int c = CipherReader_Peek(s);
    if (c < 0)
        return -1;
    if (F(char*, s, 32) != F(char*, s, 40) - 1) {
        F(char*, s, 32)++;
        return c;
    }
    CallC(FUN_44b4a0, F(void*, s, 36), 512);
    F(char**, s, 44)++;
    F(char*, s, 36) = *F(char**, s, 44);
    F(char*, s, 40) = F(char*, s, 36) + 512;
    F(char*, s, 32) = F(char*, s, 36);
    return c;
}

// ---------------------------------------------------------------------------
// The reader: one datum of the script text.

extern "C" void FUN_4b2730(); KING_ALIAS("_FUN_4b2730", "@SchDequeIt_Plus@16")    // the character iterator + n (out, n)
extern "C" void FUN_4b26e0(); KING_ALIAS("_FUN_4b26e0", "@SchDeque_PopFront@8")
extern "C" void FUN_4b2a00(); KING_ALIAS("_FUN_4b2a00", "@Reader_ErrorLine@12")
extern "C" void FUN_4b3610(); KING_ALIAS("_FUN_4b3610", "@SchReader_Skip@8")
extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8")
extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")
extern "C" void FUN_4a52e0(); KING_ALIAS("_FUN_4a52e0", "_Sch_ErrorCode")
extern "C" void FUN_4a28c0(); KING_ALIAS("_FUN_4a28c0", "_SchValue_MakePairB")
extern "C" void FUN_4b5140(); KING_ALIAS("_FUN_4b5140", "_Sch_MakeIdentifierQ")
extern "C" void FUN_4b5220(); KING_ALIAS("_FUN_4b5220", "_Sch_MakeIdentifierC")
extern "C" void FUN_4b5300(); KING_ALIAS("_FUN_4b5300", "_Sch_MakeIdentifierD")
extern "C" void FUN_4b3670(); KING_ALIAS("_FUN_4b3670", "_SchReader_4b3670")      // a number
extern "C" void FUN_4b29a0(); KING_ALIAS("_FUN_4b29a0", "_Sch_IsSymbolChar")
extern "C" void FUN_409070(); KING_ALIAS("_FUN_409070", "_Str_Same")
extern "C" void FUN_40c100(); KING_ALIAS("_FUN_40c100", "@Str_Assign@12")
extern "C" void FUN_454580(); KING_ALIAS("_FUN_454580", "@Str_CopyConstruct@12")
extern "C" void FUN_4a36c0(); KING_ALIAS("_FUN_4a36c0", "@SchHandle_Release@8")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
extern "C" void FUN_492250(); KING_ALIAS("_FUN_492250", "_SchValue_Make10007")
extern "C" void FUN_4b32d0(); KING_ALIAS("_FUN_4b32d0", "_Reader_String")    // a string
extern "C" void FUN_4b36a0(); KING_ALIAS("_FUN_4b36a0", "_Reader_Symbol")    // a symbol
extern "C" void FUN_4b38d0(); KING_ALIAS("_FUN_4b38d0", "_Reader_Vector")    // a vector
extern "C" void FUN_4b3b10(); KING_ALIAS("_FUN_4b3b10", "_Reader_Char")    // a character
extern "C" int __cdecl isdigit(int c);
extern "C" int __cdecl isalnum(int c);
ASM_VAR(AsmLabel, s_rdBadChar, $L_6714b0)
ASM_VAR(AsmLabel, s_rdUnterminated, $L_671490)
ASM_VAR(AsmLabel, s_rdQuote, $L_64bf60)
ASM_VAR(AsmLabel, s_rdQuasiquote, $L_6714a4)
ASM_VAR(AsmLabel, s_rdUnquoteSplicing, $L_64c09c)
ASM_VAR(AsmLabel, s_rdUnquote, $L_64c094)

// the character n after the current one of reader r
static int Rd_Peek(BYTE* r, int n)
{
    BYTE it[16];
    return **CallThis<int**>(FUN_4b2730, r + 8, it, n);
}

static void Rd_Fill(BYTE* r, int n) { CallThis(FUN_4b2b60, r, n); }

static void Rd_Drop(BYTE* r)
{
    Rd_Fill(r, 1);
    CallThis(FUN_4b26e0, r);
    F(int, r, 68)++;
}

static void Rd_Error(BYTE* r, const char* format)
{
    BYTE* line[3];
    CallThis(FUN_487730, line, line);
    CallThis(FUN_4b2a00, r, line);
    CallC(FUN_4a54e0, 20, 768, format, line[0]);
    CallThis(FUN_404420, line);
}

static void Rd_Check(const BYTE* v)
{
    if (!F(BYTE*, v, 0))
        CallC(FUN_4a52e0, 0, 1);
    if (F(DWORD, v, 8) & 0x10000)
        CallC(FUN_4a52e0, 4, 1);
}

// the value at d = s, keeping d's object when it is the same value
static void Rd_Assign(BYTE* d, const BYTE* s)
{
    BYTE* o = F(BYTE*, d, 0);
    if (o && !(F(DWORD, d, 8) & 0x10000) && --F(int, o, 4) == 0 && !CallC<char>(FUN_409070, d, s)
        && F(BYTE*, d, 0))
        CallVirt(F(BYTE*, d, 0), 12, 1);
    CopyRef(d, s);
}

static void Rd_Release(BYTE* v)
{
    BYTE* o = F(BYTE*, v, 0);
    if (!(F(DWORD, v, 8) & 0x10000) && o && --F(int, o, 4) == 0 && F(BYTE*, v, 0))
        CallVirt(F(BYTE*, v, 0), 12, 1);
}

// out = read(t, a, r) through the copy constructor
static BYTE* Rd_Sub(BYTE* out, void (*read)(), int a, BYTE* r)
{
    BYTE t[16];
    CallThis(FUN_454580, out, CallC<BYTE*>(read, t, a, r));
    CallThis(FUN_4a36c0, t);
    return out;
}

extern "C" {

// [FUN_4b3e30] reads the next datum of reader r (+8 its characters, +68 the position, +72 the line) into out:
// comments are skipped, a bad character is reported and skipped; empty at the end
BYTE* __cdecl Reader_Read(BYTE* out, int a, BYTE* r)
{
    for (;;) {
        Rd_Fill(r, 1);
        CallC(FUN_4b2ca0, a, r);
        int c = Rd_Peek(r, 0);
        switch (c) {
        case ';':
            for (;;) {
                BYTE ch = (BYTE)Rd_Peek(r, 0);
                Rd_Drop(r);
                if (ch == 10)
                    break;
                if (ch == 255) {
                    CallThis(FUN_4bd4d0, out);
                    return out;
                }
                Rd_Fill(r, 2);
            }
            continue;
        case -1:
            Rd_Drop(r);
            F(DWORD, out, 0) = 0;
            F(DWORD, out, 4) = 0;
            F(DWORD, out, 8) = 0;
            return out;
        case '(': {
            BYTE t[16];
            CopyRef(out, (BYTE*)Reader_NextItem((int*)t, a, r));
            CallThis(FUN_4a36c0, t);
            return out;
        }
        case '"': {
            BYTE t[16];
            CopyRef(out, CallC<BYTE*>(FUN_4b32d0, t, a, r));
            CallThis(FUN_4a36c0, t);
            return out;
        }
        case '\'':
        case '`': {
            BYTE head[16], tail[16], t[16];
            CallC(FUN_4a28c0, head, 0, F(void*, r, 72));
            const char* name = Rd_Peek(r, 0) == '\'' ? s_rdQuote : s_rdQuasiquote;
            BYTE* id = CallC<BYTE*>(FUN_4b5140, t, 0, &name);
            Rd_Check(head);
            Rd_Assign(F(BYTE*, head, 0) + 20, id);
            CallThis(FUN_4a36c0, t);
            Rd_Drop(r);
            CallC(FUN_4a28c0, tail, 0, F(void*, r, 72));
            Rd_Check(head);
            Rd_Assign(F(BYTE*, head, 0) + 32, tail);
            BYTE d[16];
            BYTE* datum = Reader_Read(d, a, r);
            Rd_Check(tail);
            Rd_Assign(F(BYTE*, tail, 0) + 20, datum);
            Rd_Release(d);
            CopyRef(out, head);
            CallThis(FUN_4a36c0, tail);
            CallThis(FUN_4a36c0, head);
            return out;
        }
        case ',': {
            BYTE head[16], tail[16], t[16];
            CallC(FUN_4a28c0, head, 0, F(void*, r, 72));
            Rd_Fill(r, 2);
            if (Rd_Peek(r, 1) == '@') {
                BYTE* id = CallC<BYTE*>(FUN_4b5220, t, 0, (const char*)s_rdUnquoteSplicing);
                Rd_Check(head);
                Rd_Assign(F(BYTE*, head, 0) + 20, id);
                CallThis(FUN_4a36c0, t);
                Rd_Drop(r);
            } else {
                BYTE* id = CallC<BYTE*>(FUN_4b5300, t, 0, (const char*)s_rdUnquote);
                Rd_Check(head);
                Rd_Assign(F(BYTE*, head, 0) + 20, id);
                CallThis(FUN_4a36c0, t);
            }
            Rd_Drop(r);
            CallC(FUN_4a28c0, tail, 0, F(void*, r, 72));
            Rd_Check(head);
            Rd_Assign(F(BYTE*, head, 0) + 32, tail);
            BYTE d[16];
            BYTE* datum = Reader_Read(d, a, r);
            Rd_Check(tail);
            CallThis(FUN_40c100, F(BYTE*, tail, 0) + 20, datum);
            Rd_Release(d);
            CallThis(FUN_454580, out, head);
            CallThis(FUN_4a36c0, tail);
            CallThis(FUN_4a36c0, head);
            return out;
        }
        case '+':
        case '-':
        case '.':
            Rd_Fill(r, 2);
            return Rd_Sub(out, isdigit(Rd_Peek(r, 1)) ? FUN_4b3670 : FUN_4b36a0, a, r);
        case '#':
            Rd_Fill(r, 2);
            switch (Rd_Peek(r, 1)) {
            case '(':
                return Rd_Sub(out, FUN_4b38d0, a, r);
            case '\\':
                return Rd_Sub(out, FUN_4b3b10, a, r);
            case 'b':
            case 'd':
            case 'e':
            case 'i':
            case 'o':
            case 'x':
                return Rd_Sub(out, FUN_4b3670, a, r);
            case 't':
                CallThis(FUN_4b3610, r);
                CallThis(FUN_4b3610, r);
                CallC(FUN_492250, out, 0, -1);
                return out;
            case 'f':
                CallThis(FUN_4b3610, r);
                CallThis(FUN_4b3610, r);
                CallC(FUN_492250, out, 0, 0);
                return out;
            case '|':
                // a block comment; at "|#" the reading goes on with its "|" as an ordinary character
                for (;;) {
                    Rd_Fill(r, 2);
                    if (Rd_Peek(r, 0) == '|' && Rd_Peek(r, 1) == '#')
                        break;
                    if (Rd_Peek(r, 0) == -1) {
                        Rd_Error(r, s_rdUnterminated);
                        CallThis(FUN_4bd4d0, out);
                        return out;
                    }
                    CallThis(FUN_4b3610, r);
                }
                break;
            default:
                Rd_Error(r, s_rdBadChar);
                break;
            }
            // fall through to an ordinary character
        default:
            if (isdigit(Rd_Peek(r, 0)))
                return Rd_Sub(out, FUN_4b3670, a, r);
            if (isalnum(Rd_Peek(r, 0)) || CallC<int>(FUN_4b29a0, Rd_Peek(r, 0)))
                return Rd_Sub(out, FUN_4b36a0, a, r);
            Rd_Error(r, s_rdBadChar);
            continue;
        }
    }
}

}  // extern "C"

// ---------------------------------------------------------------------------
// The printer: the written form of a value on a port.

extern "C" void FUN_4b13c0(); KING_ALIAS("_FUN_4b13c0", "_Port_WriteChar")
extern "C" void FUN_4b1450(); KING_ALIAS("_FUN_4b1450", "_Port_WriteString")
extern "C" void FUN_4b6a80(); KING_ALIAS("_FUN_4b6a80", "_Sch_CaretLine")
extern "C" void FUN_4556d0(); KING_ALIAS("_FUN_4556d0", "@NumberHandle_From@12")
extern "C" void FUN_4a1940(); KING_ALIAS("_FUN_4a1940", "@SchValue_AsBool@12")
extern "C" void FUN_4a19c0(); KING_ALIAS("_FUN_4a19c0", "@SchValue_AsChar@12")
extern "C" void FUN_4a6860(); KING_ALIAS("_FUN_4a6860", "@SchValue_Name@8")
extern "C" void FUN_4aa430(); KING_ALIAS("_FUN_4aa430", "@SchValue_Object@8")
extern "C" void FUN_4a5420(); KING_ALIAS("_FUN_4a5420", "_Sch_TypeError")
extern "C" void FUN_4bd510(); KING_ALIAS("_FUN_4bd510", "@Fx_Self@8")
extern "C" void FUN_4b6b90(); KING_ALIAS("_FUN_4b6b90", "_Sch_WriteListTail")    // writes the rest of a list (out, the tail, port, context)
extern "C" void FUN_4b6e20(); KING_ALIAS("_FUN_4b6e20", "_Sch_WriteNumber")    // writes a number (handle by value, port)
extern "C" int __cdecl sprintf(char* buf, const char* format, ...);
ASM_VAR(AsmLabel, s_prEmpty, $L_66d4e4)          // "()"
ASM_VAR(AsmLabel, s_prDot, $L_6715bc)            // " . "
ASM_VAR(AsmLabel, s_prTrue, $L_671664)
ASM_VAR(AsmLabel, s_prFalse, $L_671660)
ASM_VAR(AsmLabel, s_prNewline, $L_671654)
ASM_VAR(AsmLabel, s_prSpace, $L_67164c)
ASM_VAR(AsmLabel, s_prClosure, $L_671608)
ASM_VAR(AsmLabel, s_prClosureEnd, $L_671604)
ASM_VAR(AsmLabel, s_prContinuation, $L_6715f4)
ASM_VAR(AsmLabel, s_prSpecialForm, $L_6715e4)
ASM_VAR(AsmLabel, s_prGt, $L_6712a0)
ASM_VAR(AsmLabel, s_prIdentFmt, $L_6715d0)
ASM_VAR(AsmLabel, s_prIdent, $L_6715dc)
ASM_VAR(AsmLabel, s_prUnsupported, $L_6715c0)
ASM_VAR(AsmLabel, s_prPort, $L_671644)
ASM_VAR(AsmLabel, s_prEnvironment, $L_671634)
ASM_VAR(AsmLabel, s_prUserDefined, $L_671624)
ASM_VAR(AsmLabel, s_prUnspecified, $L_671614)
ASM_VAR(AsmLabel, s_prImmediate, $L_671104)
ASM_VAR(AsmLabel, s_prPair, $L_66cb08)
ASM_VAR(AsmLabel, s_prString, $L_66ca58)
ASM_VAR(AsmLabel, s_prIdentifier, $L_66ce4c)
ASM_VAR(AsmLabel, s_prVector, $L_6708e4)
ASM_VAR(AsmLabel, s_prClosureType, $L_6705f4)

struct PrValue { DWORD d[3]; };

static PrValue Pr_Port(const BYTE* port)
{
    PrValue v;
    CallThis(FUN_454580, &v, port);
    return v;
}

static void Pr_Char(const BYTE* port, int c) { CallC(FUN_4b13c0, c, Pr_Port(port)); }
static void Pr_Str(const BYTE* port, const char* s) { CallC(FUN_4b1450, s, Pr_Port(port)); }

// a typed handle of the value v (type, else a script error and empty)
static void Pr_Typed(BYTE* h, const BYTE* v, int type, const char* name)
{
    F(DWORD, h, 0) = 0;
    F(DWORD, h, 4) = 0;
    F(DWORD, h, 8) = 0;
    BYTE* o = F(BYTE*, v, 0);
    if (o) {
        if (CallVirt<int>(o, 0) == type) {
            F(BYTE*, h, 0) = o;
            F(DWORD, h, 8) = F(DWORD, v, 8);
            F(int, o, 4)++;
        } else
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(o, 4), name);
    } else {
        F(DWORD, h, 0) = 0;
        F(DWORD, h, 8) = 0;
    }
}

static void Pr_Check(const BYTE* h)
{
    if (!F(BYTE*, h, 0))
        CallC(FUN_4a52e0, 0, 1);
    if (F(DWORD, h, 8) & 0x10000)
        CallC(FUN_4a52e0, 4, 1);
}

static void Pr_ReleaseInline(BYTE* v)
{
    BYTE* o = F(BYTE*, v, 0);
    if (!(F(DWORD, v, 8) & 0x10000) && o && --F(int, o, 4) == 0 && F(BYTE*, v, 0))
        CallVirt(F(BYTE*, v, 0), 12, 1);
}

extern "C" {

// [FUN_4b6f70] writes value v on the port (a value) in written form; ctx: +0 the options (bit 0 of its first
// byte: closures with their code), +4 the depth; out is left empty
BYTE* __cdecl Sch_Write(BYTE* out, const BYTE* v, const BYTE* port, BYTE* ctx);

BYTE* __cdecl Sch_Write(BYTE* out, const BYTE* v, const BYTE* port, BYTE* ctx)
{
    BYTE h[16], t[16];
    if (!F(BYTE*, v, 0) && !F(DWORD, v, 8) && !F(DWORD, v, 4)) {
        Pr_Str(port, s_prEmpty);
    } else
        switch ((BYTE)F(DWORD, v, 8)) {
        case 2:
            Pr_Typed(h, v, 2, s_prPair);
            F(int, ctx, 4)++;
            CallC(FUN_4b6a80, port, ctx);
            Pr_Char(port, '(');
            Pr_Check(h);
            Sch_Write(t, F(BYTE*, h, 0) + 20, port, ctx);
            Pr_ReleaseInline(t);
            Pr_Check(h);
            {
                BYTE* cdr = F(BYTE*, h, 0) + 32;
                if (!F(BYTE*, cdr, 0) && !F(DWORD, cdr, 8) && !F(DWORD, cdr, 4)) {
                    Pr_Char(port, ')');
                    F(int, ctx, 4)--;
                } else {
                    Pr_Check(h);
                    if ((BYTE)F(DWORD, F(BYTE*, h, 0) + 32, 8) == 2) {
                        Pr_Check(h);
                        CallC(FUN_4b6b90, t, F(BYTE*, h, 0) + 32, port, ctx);
                        Pr_ReleaseInline(t);
                    } else {
                        Pr_Str(port, s_prDot);
                        Pr_Check(h);
                        Sch_Write(t, F(BYTE*, h, 0) + 32, port, ctx);
                        Pr_ReleaseInline(t);
                        Pr_Char(port, ')');
                        F(int, ctx, 4)--;
                    }
                }
            }
            CallThis(FUN_4a36c0, h);
            break;
        case 3:
            Pr_Typed(h, v, 3, s_prString);
            Pr_Char(port, ' ');
            Pr_Char(port, '"');
            Pr_Check(h);
            for (char* p = F(char*, F(BYTE*, h, 0), 12);; p++) {
                Pr_Check(h);
                if (p == F(char*, F(BYTE*, h, 0), 16))
                    break;
                char c = *p;
                if (c == 10) {
                    Pr_Char(port, '\\');
                    Pr_Char(port, 'n');
                } else if (c == '\\') {
                    Pr_Char(port, '\\');
                    Pr_Char(port, '\\');
                } else if (c == '"') {
                    Pr_Char(port, '\\');
                    Pr_Char(port, '"');
                } else if (!c)
                    break;
                else
                    Pr_Char(port, *p);
            }
            Pr_Char(port, '"');
            Pr_Char(port, ' ');
            CallThis(FUN_4a36c0, h);
            break;
        case 4:
            Pr_Str(port, s_prClosure);
            Pr_Typed(h, v, 4, s_prClosureType);
            Pr_Check(h);
            if (CallVirt<int>(F(BYTE*, h, 0), 20) == 1) {
                BYTE h2[16];
                Pr_Typed(h2, h, 4, s_prClosureType);
                PrValue pv = Pr_Port(port);
                CallC(FUN_4b1450, F(const char*, CallThis<BYTE*>(FUN_4aa430, h2), 20), pv);
                CallThis(FUN_4a36c0, h2);
            } else {
                Pr_Check(h);
                if (CallVirt<int>(F(BYTE*, h, 0), 20) == 2 && (*F(BYTE*, ctx, 0) & 1)) {
                    BYTE h2[16];
                    Pr_Typed(h2, h, 4, s_prClosureType);
                    Pr_Check(h2);
                    Sch_Write(t, F(BYTE*, h2, 0) + 32, port, ctx);
                    Pr_ReleaseInline(t);
                    Pr_Check(h2);
                    Sch_Write(t, F(BYTE*, h2, 0) + 20, port, ctx);
                    Pr_ReleaseInline(t);
                    CallThis(FUN_4a36c0, h2);
                }
            }
            Pr_Str(port, s_prClosureEnd);
            CallThis(FUN_4a36c0, h);
            break;
        case 5:
            Pr_Str(port, s_prContinuation);
            break;
        case 6: {
            Pr_Char(port, ' ');
            PrValue n;
            CallThis(FUN_4556d0, &n, v);
            CallC(FUN_4b6e20, n, port);
            Pr_Char(port, ' ');
            break;
        }
        case 7:
            Pr_Char(port, ' ');
            CallThis(FUN_4a1940, h, v);
            Pr_Str(port, F(BYTE*, h, 0) ? s_prTrue : s_prFalse);
            CallThis(FUN_4a36c0, h);
            Pr_Char(port, ' ');
            break;
        case 8:
            Pr_Char(port, ' ');
            CallThis(FUN_4a19c0, h, v);
            if (F(int, h, 0) == 10)
                Pr_Str(port, s_prNewline);
            else if (F(int, h, 0) == 32)
                Pr_Str(port, s_prSpace);
            else {
                Pr_Char(port, '#');
                Pr_Char(port, '\\');
                PrValue pv = Pr_Port(port);
                CallC(FUN_4b13c0, (int)*CallThis<char*>(FUN_4bd510, h), pv);
            }
            Pr_Char(port, ' ');
            CallThis(FUN_4a36c0, h);
            break;
        case 9:
            Pr_Char(port, ' ');
            Pr_Typed(h, v, 9, s_prIdentifier);
            Pr_Check(h);
            for (char* p = F(char*, F(BYTE*, h, 0), 12);; p++) {
                Pr_Check(h);
                if (p == F(char*, F(BYTE*, h, 0), 16) || !*p)
                    break;
                Pr_Char(port, *p);
            }
            Pr_Char(port, ' ');
            CallThis(FUN_4a36c0, h);
            break;
        case 10:
            Pr_Char(port, ' ');
            Pr_Char(port, '#');
            Pr_Char(port, '(');
            Pr_Typed(h, v, 10, s_prVector);
            Pr_Check(h);
            for (BYTE* p = F(BYTE*, F(BYTE*, h, 0), 20);; p += 12) {
                Pr_Check(h);
                if (p == F(BYTE*, F(BYTE*, h, 0), 24))
                    break;
                Sch_Write(t, p, port, ctx);
                Pr_ReleaseInline(t);
            }
            Pr_Char(port, ')');
            Pr_Char(port, ' ');
            CallThis(FUN_4a36c0, h);
            break;
        case 11: Pr_Str(port, s_prPort); break;
        case 12: Pr_Str(port, s_prEnvironment); break;
        case 64: Pr_Str(port, s_prUserDefined); break;
        case 255: Pr_Str(port, s_prUnspecified); break;
        case 34:
            Pr_Str(port, s_prSpecialForm);
            {
                PrValue pv = Pr_Port(port);
                CallC(FUN_4b1450, CallThis<const char*>(FUN_4a6860, v), pv);
            }
            Pr_Str(port, s_prGt);
            break;
        case 33: {
            char buf[128];
            sprintf(buf, s_prIdentFmt, (const char*)s_prIdent, F(DWORD, v, 0), F(DWORD, v, 4));
            Pr_Str(port, buf);
            break;
        }
        default: {
            Pr_Str(port, s_prUnsupported);
            PrValue pv = Pr_Port(port);
            CallC(FUN_4b1450, CallThis<const char*>(FUN_4a6860, v), pv);
            PrValue cp;
            CopyRef((BYTE*)&cp, port);
            CallC(FUN_4b1450, (const char*)s_prGt, cp);
            const char* name = (F(DWORD, v, 8) & 0x10000) ? (const char*)s_prImmediate
                                                          : CallVirt<const char*>(F(BYTE*, v, 0), 4);
            CallC(FUN_4a5420, 21, 256, name);
            break;
        }
        }
    F(DWORD, out, 0) = 0;
    F(DWORD, out, 4) = 0;
    F(DWORD, out, 8) = 0;
    return out;
}

}  // extern "C"

// ---------------------------------------------------------------------------
// Number syntax

extern "C" void FUN_4b1d40(); KING_ALIAS("_FUN_4b1d40", "_Num_ReadReal")    // a real number from the deque (out, radix, deque)
extern "C" void FUN_48a9d0(); KING_ALIAS("_FUN_48a9d0", "@WBuf_Free@8")
extern "C" void FUN_490ea0(); KING_ALIAS("_FUN_490ea0", "@StrPair_Destroy@4")
ASM_VAR(char, s_badComplex671420, $L_671420)

static int Num_Peek(BYTE* d, int n)
{
    PtrDequeIt it;
    SchDequeIt_Plus((PtrDequeIt*)(d + 8), 0, &it, n);
    return *(int*)it.cur;
}

// a rational number n/1
static void** Num_Int(void** r, int n)
{
    return SchSymbol_Init(r, 0, (const char*)n);
}

static void Num_Free(void** r)
{
    CallThis(FUN_48a9d0, &r[1]);
    CallThis(FUN_48a9d0, &r[0]);
}

extern "C" {

// [FUN_4b20d0] a number read from the deque of characters: #i #e #b #o #d #x prefixes, then +i, -i, a real, or
// real@angle, real+i, real+imag i, real-imag i
DWORD* __cdecl Sch_ParseNumber(DWORD* out, int, BYTE* d)
{
    int inexact = 0, radix = 10;
    for (int k = 0; k < 2 && Num_Peek(d, 0) == '#'; k++) {
        switch (Num_Peek(d, 1)) {
        case 'i': inexact = 1; break;
        case 'e': inexact = 0; break;
        case 'b': radix = 2; break;
        case 'o': radix = 8; break;
        case 'd': radix = 10; break;
        case 'x': radix = 16; break;
        }
        SchDeque_PopFront(d, 0);
        SchDeque_PopFront(d, 0);
    }
    void* re[2];
    void* im[2];
    if ((Num_Peek(d, 0) == '+' || Num_Peek(d, 0) == '-') && Num_Peek(d, 1) == 'i') {
        int sign = Num_Peek(d, 0) == '+' ? 1 : -1;
        SchDeque_PopFront(d, 0);
        SchDeque_PopFront(d, 0);
        Num_Int(im, sign);
        Num_Int(re, 0);
        Sch_MakeNumber3(out, 0, (DWORD*)&inexact, (DWORD)re, (DWORD)im);
        Num_Free(re);
        Num_Free(im);
        return out;
    }
    CallC(FUN_4b1d40, re, radix, d);
    int c = Num_Peek(d, 0);
    if (c == '@') {
        SchDeque_PopFront(d, 0);
        CallC(FUN_4b1d40, im, radix, d);
        Sch_MakeNumber3(out, 0, (DWORD*)&inexact, (DWORD)re, (DWORD)im);
        Num_Free(im);
    } else if (c == '+') {
        SchDeque_PopFront(d, 0);
        if (Num_Peek(d, 0) == 'i') {
            SchDeque_PopFront(d, 0);
            Num_Int(im, 1);
            Sch_MakeNumber3(out, 0, (DWORD*)&inexact, (DWORD)re, (DWORD)im);
            Num_Free(im);
        } else {
            CallC(FUN_4b1d40, im, radix, d);
            if (Num_Peek(d, 0) == 'i')
                SchDeque_PopFront(d, 0);
            else
                CallC(FUN_4a5420, 20, 768, &s_badComplex671420);
            Sch_MakeNumber3(out, 0, (DWORD*)&inexact, (DWORD)re, (DWORD)im);
            Num_Free(im);
            CallThis(FUN_490ea0, re);
            return out;
        }
    } else if (c == '-') {
        // looks at the '-' again, so "-i" after a real is read as an imaginary part
        if (Num_Peek(d, 0) == 'i') {
            SchDeque_PopFront(d, 0);
            SchDeque_PopFront(d, 0);
            Num_Int(im, -1);
            Sch_MakeNumber3(out, 0, (DWORD*)&inexact, (DWORD)re, (DWORD)im);
        } else {
            CallC(FUN_4b1d40, im, radix, d);
            if (Num_Peek(d, 0) == 'i')
                SchDeque_PopFront(d, 0);
            else
                CallC(FUN_4a5420, 20, 768, &s_badComplex671420);
            Sch_MakeNumber3(out, 0, (DWORD*)&inexact, (DWORD)re, (DWORD)im);
        }
        CallThis(FUN_490ea0, im);
        CallThis(FUN_490ea0, re);
        return out;
    } else {
        Num_Int(im, 0);
        Sch_MakeNumber3(out, 0, (DWORD*)&inexact, (DWORD)re, (DWORD)im);
        CallThis(FUN_490ea0, im);
        CallThis(FUN_490ea0, re);
        return out;
    }
    Num_Free(re);
    return out;
}

}  // extern "C"

// === parsing a number's digits (FUN_4b1720)

ASM_VAR(AsmLabel, s_pd671400, $L_671400)
extern "C" void FUN_48afa0(); KING_ALIAS("_FUN_48afa0", "_Int_AddSigned")
extern "C" void FUN_48b350(); KING_ALIAS("_FUN_48b350", "_Int_Mul")
extern "C" void FUN_4b1500(); KING_ALIAS("_FUN_4b1500", "_Num_ReadExponent")
extern "C" void FUN_4b2840(); KING_ALIAS("_FUN_4b2840", "@SchDeque_NextBlock@8")

// the temporary big integer t freed when it owns its digits
static void Pd_Free(BYTE* t)
{
    if (t && F(WORD, t, 2))
        operator delete(t);
}

static void Pd_Mul(BYTE** v, int k)
{
    BYTE* t = CallC<BYTE*>(FUN_48ade0, 0, k);
    *v = CallC<BYTE*>(FUN_48b350, *v, t, *v);
    Pd_Free(t);
}

static void Pd_Add(BYTE** v, int k)
{
    BYTE* t = CallC<BYTE*>(FUN_48ade0, 0, k);
    *v = CallC<BYTE*>(FUN_48afa0, *v, 0, t, 0, *v);
    Pd_Free(t);
}

// the next character of the reader's deque d (iterator +8, block end +16)
static int Pd_Next(BYTE* d)
{
    BYTE** cur = (BYTE**)(d + 8);
    if (*cur != F(BYTE*, d, 16) - 4)
        *cur += 4;
    else
        CallThis(FUN_4b2840, d);
    BYTE it[16];
    return toupper(**CallThis<int**>(FUN_4b2730, d + 8, it, 0));
}

static void Pd_Digit(BYTE** num, int c, int radix)
{
    int k = c - '0';
    if (k >= radix)
        CallC(FUN_4a5420, 20, 768, (const char*)s_pd671400);
    Pd_Mul(num, radix);
    Pd_Add(num, k);
}

// an exponent: num or den times 10 per step
static int Pd_Exponent(BYTE** num, BYTE** den, BYTE* d)
{
    int e = CallC<int>(FUN_4b1500, d);
    for (int i = 0; i < e; i++)
        Pd_Mul(num, 10);
    for (int i = 0; i < -e; i++)
        Pd_Mul(den, 10);
    return 1;
}

static bool Pd_IsExponent(int c) { return c == 'D' || c == 'E' || c == 'F' || c == 'L' || c == 'S'; }

extern "C" {

// [FUN_4b1720] reads the digits of a number in radix from the reader's deque d into num / den (big integers):
// '#' digits skipped, a '.' starts the fraction (each digit also scales den by 10), D E F L S an exponent;
// 1 for a fraction or an exponent, 0 for an integer
int __cdecl Num_ParseDigits(BYTE** num, BYTE** den, int radix, BYTE* d)
{
    BYTE it[16];
    int c = toupper(**CallThis<int**>(FUN_4b2730, d + 8, it, 0));
    for (;;) {
        if (c == '#')
            ;
        else if (c >= '0' && c <= '9')
            Pd_Digit(num, c, radix);
        else if (c == '.')
            break;
        else if (Pd_IsExponent(c))
            return Pd_Exponent(num, den, d);
        else
            return 0;
        c = Pd_Next(d);
    }
    for (;;) {
        c = Pd_Next(d);
        if (c == '#')
            continue;
        if (c >= '0' && c <= '9') {
            Pd_Digit(num, c, radix);
            Pd_Mul(den, 10);
        } else if (Pd_IsExponent(c))
            return Pd_Exponent(num, den, d);
        else
            return 1;
    }
}

} // extern "C"

// === reading the rest of a list (FUN_4b2e90)

ASM_VAR(AsmLabel, s_rl671470, $L_671470)
ASM_VAR(AsmLabel, s_rl671464, $L_671464)
extern "C" void FUN_4b2980(); KING_ALIAS("_FUN_4b2980", "_Sch_IsSpaceOrEof")
extern "C" void FUN_4aa430(); KING_ALIAS("_FUN_4aa430", "@SchValue_Object@8")

extern "C" {

// [FUN_4b2e90] reads the rest of a list after '(' from reader r into out: ')' ends it (the empty list), else
// a pair of the next datum and the rest, read recursively or after " . " a single datum and ')'; end of input
// and a missing ')' are reported
BYTE* __cdecl Reader_ListTail(BYTE* out, int a, BYTE* r)
{
    Rd_Fill(r, 1);
    if (Rd_Peek(r, 0) == ')') {
        Rd_Drop(r);
        CallThis(FUN_4bd4d0, out);
        F(int, out, 0) = 0;
        F(int, out, 8) = 0;
        return out;
    }
    DWORD pair[3], tmp[3];
    CallC(FUN_4a28c0, pair, 0, F(int, r, 72));
    BYTE* v = Reader_Read((BYTE*)tmp, a, r);
    Rd_Check((BYTE*)pair);
    Rd_Assign(F(BYTE*, pair, 0) + 20, v);
    CallThis(FUN_4a36c0, tmp);
    CallC(FUN_4b2ca0, a, r);
    Rd_Fill(r, 2);
    if (Rd_Peek(r, 0) == -1) {
        Rd_Error(r, (const char*)s_rl671470);
        CallThis(FUN_454580, out, pair);
        CallThis(FUN_4a36c0, pair);
        return out;
    }
    if (Rd_Peek(r, 0) == '.' && CallC<int>(FUN_4b2980, Rd_Peek(r, 1))) {
        Rd_Drop(r);
        BYTE* d = Reader_Read((BYTE*)tmp, a, r);
        CallThis(FUN_40c100, CallThis<BYTE*>(FUN_4aa430, pair) + 32, d);
        CallThis(FUN_4a36c0, tmp);
        CallC(FUN_4b2ca0, a, r);
        Rd_Fill(r, 1);
        if (Rd_Peek(r, 0) == ')')
            Rd_Drop(r);
        else
            Rd_Error(r, (const char*)s_rl671464);
    } else {
        BYTE* d = Reader_ListTail((BYTE*)tmp, a, r);
        CallThis(FUN_40c100, CallThis<BYTE*>(FUN_4aa430, pair) + 32, d);
        CallThis(FUN_4a36c0, tmp);
    }
    CallThis(FUN_454580, out, pair);
    CallThis(FUN_4a36c0, pair);
    return out;
}

} // extern "C"

// === reading a string literal (FUN_4b32d0)

extern "C" void FUN_4a2400(); KING_ALIAS("_FUN_4a2400", "_SchValue_MakeStringB")
extern "C" void FUN_47ab20(); KING_ALIAS("_FUN_47ab20", "@CharVec_PushBack@12")
extern "C" void FUN_4b3610(); KING_ALIAS("_FUN_4b3610", "@SchReader_Skip@8")

// appends c to the characters of string value s
static void Rs_Push(DWORD* s, char c)
{
    CallThis(FUN_47ab20, CallThis<BYTE*>(FUN_4aa430, s) + 12, &c);
}

extern "C" {

// [FUN_4b32d0] reads a string literal after its '"' from reader r into out: \n \r \t escapes (any other escaped
// character as itself) up to the closing '"'
BYTE* __cdecl Reader_String(BYTE* out, int a, BYTE* r)
{
    Rd_Drop(r);
    DWORD s[3];
    CallC(FUN_4a2400, s, 0);
    for (;;) {
        Rd_Fill(r, 1);
        int c = Rd_Peek(r, 0);
        if (c == 92) {
            Rd_Drop(r);
            Rd_Fill(r, 1);
            int e = Rd_Peek(r, 0);
            char ch = e == 'n' ? 10 : e == 'r' ? 13 : e == 't' ? 9 : (char)e;
            Rd_Check((BYTE*)s);
            Rs_Push(s, ch);
        } else if (c == '"') {
            Rs_Push(s, 0);
            CallThis(FUN_4b3610, r);
            CallThis(FUN_454580, out, s);
            CallThis(FUN_4a36c0, s);
            return out;
        } else
            Rs_Push(s, (char)c);
        Rd_Drop(r);
    }
}

} // extern "C"

// === reading a character literal (FUN_4b3b10)

ASM_VAR(char, s_rc671480, $L_671480)
ASM_VAR(char, s_rc671488, $L_671488)
extern "C" void FUN_49c210(); KING_ALIAS("_FUN_49c210", "_SchValue_Make10008")
extern "C" void FUN_49c560(); KING_ALIAS("_FUN_49c560", "@SchValue_Assign@12")

// whether the next n characters spell name (from its second letter on); consumes them when they do
static bool Rc_Name(BYTE* r, const char* name, int n)
{
    int i = 1;
    while (i < n && Rd_Peek(r, i) == name[i])
        i++;
    if (i != n)
        return false;
    for (; i; i--)
        CallThis(FUN_4b3610, r);
    return true;
}

// ch = the character value of c
static void Rc_Set(DWORD* ch, int c)
{
    DWORD v[3], t[3];
    CallC(FUN_49c210, v, 0, c);
    CallThis(FUN_4a19c0, t, v);
    CallThis(FUN_49c560, ch, t);
    CallThis(FUN_4a36c0, t);
    CallThis(FUN_4a36c0, v);
}

extern "C" {

// [FUN_4b3b10] reads a character literal #\x from reader r into out; #\newline and #\space by name (a partial
// name gives the empty value)
BYTE* __cdecl Reader_Char(BYTE* out, int a, BYTE* r)
{
    Rd_Drop(r);
    Rd_Drop(r);
    Rd_Fill(r, 1);
    DWORD ch[3];
    CallThis(FUN_4bd4d0, ch);
    int c = Rd_Peek(r, 0);
    if (c == 'n') {
        Rd_Fill(r, 8);
        if (Rc_Name(r, &s_rc671480, 7))
            Rc_Set(ch, 10);
    } else if (c == 's') {
        Rd_Fill(r, 6);
        if (Rc_Name(r, &s_rc671488, 5))
            Rc_Set(ch, 32);
    } else {
        Rc_Set(ch, c);
        Rd_Drop(r);
    }
    CallThis(FUN_454580, out, ch);
    CallThis(FUN_4a36c0, ch);
    return out;
}

} // extern "C"

// === reading a real number (FUN_4b1d40)

extern "C" void FUN_4b1b90(); KING_ALIAS("_FUN_4b1b90", "_Deque_ReadDigits")    // an integer in radix from the deque (out, radix, deque)
extern "C" void FUN_490db0(); KING_ALIAS("_FUN_490db0", "@Rational_Reduce@8")

// drops the current character of the reader's deque d
static void Nm_Pop(BYTE* d)
{
    BYTE** cur = (BYTE**)(d + 8);
    if (*cur != F(BYTE*, d, 16) - 4)
        *cur += 4;
    else
        CallThis(FUN_4b2840, d);
}

extern "C" {

// [FUN_4b1d40] reads a real number in radix from the reader's deque d into out (numerator, denominator): an
// optional sign, the digits (radix 10 may end in a fraction or exponent), then "/denominator" when exact
BYTE** __cdecl Num_ReadReal(BYTE** out, int radix, BYTE* d)
{
    BYTE* num = CallC<BYTE*>(FUN_48ade0, 0, 0);
    BYTE* den = CallC<BYTE*>(FUN_48ade0, 0, 1);
    BYTE it[16];
    bool neg = false;
    int c = **CallThis<int**>(FUN_4b2730, d + 8, it, 0);
    if (c == '-') {
        neg = true;
        Nm_Pop(d);
    } else if (c == '+')
        Nm_Pop(d);
    int inexact = 0;
    if (radix == 10)
        inexact = Num_ParseDigits(&num, &den, 10, d);
    else {
        BYTE* t;
        CallC(FUN_4b1b90, &t, radix, d);
        num = CallC<BYTE*>(FUN_48acd0, num, t);
        Pd_Free(t);
    }
    if (**CallThis<int**>(FUN_4b2730, d + 8, it, 0) == '/' && !inexact) {
        Nm_Pop(d);
        BYTE* t;
        CallC(FUN_4b1b90, &t, radix, d);
        den = CallC<BYTE*>(FUN_48acd0, den, t);
        Pd_Free(t);
    }
    BYTE* r[2];
    BYTE* n = 0;
    if (neg) {
        Sch_Convert4b2050((DWORD*)&n, (DWORD*)&num);
        r[0] = CallC<BYTE*>(FUN_48acd0, 0, n);
    } else
        r[0] = CallC<BYTE*>(FUN_48acd0, 0, num);
    r[1] = CallC<BYTE*>(FUN_48acd0, 0, den);
    CallThis(FUN_490db0, r);
    out[0] = CallC<BYTE*>(FUN_48acd0, 0, r[0]);
    out[1] = CallC<BYTE*>(FUN_48acd0, 0, r[1]);
    Pd_Free(r[1]);
    Pd_Free(r[0]);
    if (neg)
        Pd_Free(n);
    Pd_Free(den);
    Pd_Free(num);
    return out;
}

} // extern "C"

// === writing the rest of a list (FUN_4b6b90)

extern "C" {

// [FUN_4b6b90] writes the elements of the pair v to port after the first: its car, then its cdr (a pair goes on,
// another value after " . "), closing the list with ')'; out = the empty value
BYTE* __cdecl Sch_WriteListTail(BYTE* out, const BYTE* v, const BYTE* port, BYTE* ctx)
{
    BYTE h[16], t[16];
    Pr_Typed(h, v, 2, s_prPair);
    Pr_Check(h);
    Sch_Write(t, F(BYTE*, h, 0) + 20, port, ctx);
    Pr_ReleaseInline(t);
    Pr_Check(h);
    BYTE* cdr = F(BYTE*, h, 0) + 32;
    if (!F(BYTE*, cdr, 0) && !F(DWORD, cdr, 8) && !F(DWORD, cdr, 4)) {
        Pr_Char(port, ')');
        F(int, ctx, 4)--;
    } else {
        Pr_Check(h);
        if ((BYTE)F(DWORD, F(BYTE*, h, 0) + 32, 8) == 2) {
            Pr_Check(h);
            Sch_WriteListTail(t, F(BYTE*, h, 0) + 32, port, ctx);
            CallThis(FUN_4a36c0, t);
        } else {
            Pr_Str(port, s_prDot);
            Sch_Write(t, CallThis<BYTE*>(FUN_4aa430, h) + 32, port, ctx);
            CallThis(FUN_4a36c0, t);
            Pr_Char(port, ')');
            F(int, ctx, 4)--;
        }
    }
    CallThis(FUN_4bd4d0, out);
    CallThis(FUN_4a36c0, h);
    return out;
}

} // extern "C"

// === reading a vector literal (FUN_4b38d0)

extern "C" void FUN_4a2700(); KING_ALIAS("_FUN_4a2700", "_SchValue_MakeVector")
extern "C" void FUN_460910(); KING_ALIAS("_FUN_460910", "_ConstructStrRef")
extern "C" void FUN_44c710(); KING_ALIAS("_FUN_44c710", "@StrRefVec_InsertAux@16")

extern "C" {

// [FUN_4b38d0] reads a vector literal #(...) from reader r into out: the data up to ')' appended to a new
// vector (in r's context +72)
BYTE* __cdecl Reader_Vector(BYTE* out, int a, BYTE* r)
{
    Rd_Drop(r);
    Rd_Drop(r);
    BYTE v[16];
    CallC(FUN_4a2700, v, 0, F(int, r, 72));
    CallC(FUN_4b2ca0, a, r);
    while (Rd_Peek(r, 0) != ')') {
        BYTE t[16];
        BYTE* e = Reader_Read(t, a, r);
        Pr_Check(v);
        BYTE** w = (BYTE**)(F(BYTE*, v, 0) + 20);
        if (w[1] != w[2]) {
            CallC(FUN_460910, w[1], e);
            w[1] += 12;
        } else
            CallThis(FUN_44c710, w, w[1], e);
        Pr_ReleaseInline(t);
        CallC(FUN_4b2ca0, a, r);
    }
    Rd_Drop(r);
    CallThis(FUN_454580, out, v);
    Pr_ReleaseInline(v);
    return out;
}

} // extern "C"

// === reading a symbol (FUN_4b36a0)

// appends c to the name of identifier value v (a vector of characters at +12)
static void Rs_Append(BYTE* v, char c)
{
    Pr_Check(v);
    BYTE** w = (BYTE**)(F(BYTE*, v, 0) + 12);
    if (w[1] != w[2]) {
        if (w[1])
            *w[1] = c;
        w[1]++;
    } else
        CallThis(FUN_44bef0, w, w[1], &c);
}

extern "C" {

// [FUN_4b36a0] reads a symbol from reader r into out: a new identifier named by the following letters, digits
// and symbol characters
BYTE* __cdecl Reader_Symbol(BYTE* out, int a, BYTE* r)
{
    BYTE v[16];
    Sch_MakeIdentifierNew((DWORD*)v);
    SchReader_4b2e80(a, r);
    int n = 0;
    while (isalnum(Rd_Peek(r, n)) || Sch_IsSymbolChar(Rd_Peek(r, n)))
        n++;
    for (; n; n--) {
        char c = (char)Rd_Peek(r, 0);
        Rs_Append(v, c);
        Rd_Drop(r);
    }
    Rs_Append(v, 0);
    CallThis(FUN_454580, out, v);
    Pr_ReleaseInline(v);
    return out;
}

} // extern "C"

// === reading an exponent (FUN_4b1500)

ASM_VAR(AsmLabel, s_ex6713f0, $L_6713f0)

static int Ex_Peek(BYTE* d)
{
    BYTE it[16];
    return **CallThis<int**>(FUN_4b2730, d + 8, it, 0);
}

extern "C" {

// [FUN_4b1500] the exponent after the marker at the front of the reader's deque d: an optional sign and decimal
// digits (a type error without a digit)
int __cdecl Num_ReadExponent(BYTE* d)
{
    Nm_Pop(d);
    int sign = 1, n = 0;
    int c = Ex_Peek(d);
    if (c == '+')
        Nm_Pop(d);
    else if (c == '-') {
        Nm_Pop(d);
        sign = -1;
    }
    if (!isdigit(Ex_Peek(d)))
        CallC(FUN_4a5420, 20, 768, (const char*)s_ex6713f0);
    for (signed char ch = (signed char)toupper(Ex_Peek(d)); ch >= '0' && ch <= '9';
         ch = (signed char)toupper(Ex_Peek(d))) {
        n = n * 10 + ch - '0';
        Nm_Pop(d);
    }
    return sign * n;
}

} // extern "C"

// === the file port destructor (FUN_4b5b70)

extern "C" void FUN_4a29a0(); KING_ALIAS("_FUN_4a29a0", "_DequeIt_Pass0")
struct Fp_It { DWORD d[4]; };     // a deque iterator passed by value
extern "C" void FUN_4abea0(); KING_ALIAS("_FUN_4abea0", "@Sch_Copy16@12")

extern "C" {

// [FUN_4b5b70] destructor of file port s (vtable 64c0b0): closes its file (+16 bit 0), destroys the characters of
// its deque (+24) and frees its blocks and map
void __fastcall FilePort_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_vtbl64c0b0;
    if (F(BYTE, s, 16) & 1)
        CallThis(FUN_4b5490, s);
    BYTE* d = s + 24;
    Fp_It a, b;
    CallThis(FUN_4abea0, &a, d + 8);
    memcpy(&b, d + 24, 16);
    CallC(FUN_4a29a0, a, b);
    if (F(void*, d, 0)) {
        for (void** n = F(void**, d, 20); n < F(void**, d, 36) + 1; n++)
            free(*n);
        if (int m = F(int, d, 4))
            CallC(FUN_44b4a0, F(void*, d, 0), m * 4);
    }
    SchNode_Destroy((void**)s, 0);
}

} // extern "C"

// === reading a datum from a port (FUN_4b4c20)

extern "C" void FUN_4abec0(); KING_ALIAS("_FUN_4abec0", "@Deque_FreeStorage@8")

extern "C" {

// [FUN_4b4c20] out = the next datum read from port p (by value) with a reader of context a
BYTE* __cdecl Port_ReadDatum(BYTE* out, int a, DWORD p0, DWORD p1, DWORD p2)
{
    DWORD p[3] = {p0, p1, p2};
    DWORD c[3];
    CopyRef((BYTE*)c, (const BYTE*)p);
    BYTE rd[96];
    Item4b4e10_Construct(rd, 0, a, c[0], c[1], c[2]);
    BYTE t[16];
    Reader_Read(t, (int)p, rd);
    CopyRef(out, t);
    Pr_ReleaseInline(t);
    if (int n = (int)(F(BYTE*, rd, 60) - F(BYTE*, rd, 52)))
        CallC(FUN_44b4a0, F(BYTE*, rd, 52), n);
    CallThis(FUN_4a36c0, rd + 40);
    Fp_It x, y;
    CallThis(FUN_4abea0, &y, rd + 24);
    CallThis(FUN_4abea0, &x, rd + 8);
    CallC(FUN_4a29a0, x, y);
    CallThis(FUN_4abec0, rd);
    Pr_ReleaseInline((BYTE*)p);
    return out;
}

} // extern "C"

// === another deque destructor (4b5f70)

extern "C" void __fastcall Deque_Destroy(BYTE* d);    // natives.cpp

// [$L_4b5f70] the same destructor as Deque_Destroy (another instance)
extern "C" void __fastcall Deque_Destroy2(BYTE* d) { Deque_Destroy(d); }

// === skipping blanks and comments (FUN_4b2ca0)

// the character at reader r's position (+8)
static int Sk_Peek(BYTE* r)
{
    PtrDequeIt t = *(PtrDequeIt*)(r + 8);
    return *(int*)SchDequeIt_Advance(&t, 0, 0)->cur;
}

extern "C" {

// [FUN_4b2ca0] moves reader r past blanks and ';' comments (to the end of the line; stops at the end of input)
void __cdecl Reader_SkipBlanks(int, BYTE* r)
{
    PtrDequeIt* it = (PtrDequeIt*)(r + 8);
    for (;;) {
        CallThis(FUN_4b2b60, r, 1);
        if (Sch_IsSpaceChar(Sk_Peek(r))) {
            CallThis(FUN_4b2b60, r, 1);
            if ((BYTE*)it->cur != F(BYTE*, r, 16) - 4) {
                it->cur = (void**)((BYTE*)it->cur + 4);
                F(int, r, 68)++;
            } else {
                SchDeque_NextBlock(r, 0);
                F(int, r, 68)++;
            }
            continue;
        }
        if (Sk_Peek(r) != ';') {
            Sk_Peek(r);
            return;
        }
        if (Sk_Peek(r) == '\n')
            continue;
        for (;;) {
            CallThis(FUN_4b2b60, r, 1);
            if ((BYTE*)it->cur != F(BYTE*, r, 16) - 4)
                it->cur = (void**)((BYTE*)it->cur + 4);
            else {
                CallC(FUN_44b4a0, F(void*, r, 12), 512);
                SchDequeIt_SetNode(it, 0, (void***)(F(BYTE*, r, 20) + 4));
                it->cur = F(void**, r, 12);
            }
            F(int, r, 68)++;
            CallThis(FUN_4b2b60, r, 1);
            if (Sk_Peek(r) == -1)
                return;
            if (Sk_Peek(r) == '\n')
                break;
        }
    }
}

} // extern "C"

// === a vector value and writing a number (FUN_4b66f0, FUN_4b6e20)

extern "C" void Vv_Allocate(); KING_ALIAS("_Vv_Allocate", "_Stl_Allocate")
extern "C" void Vv_NumberWrite(); KING_ALIAS("_Vv_NumberWrite", "@Number_Write@12")
extern "C" void Vv_PortWriteString(); KING_ALIAS("_Vv_PortWriteString", "_Port_WriteString")

extern "C" {

// [FUN_4b66f0] constructor (owner, n, v by value) of a vector value of n copies of v (vtable 64c128), linked into
// the owner's list
BYTE* __fastcall SchVector_Construct(BYTE* s, void*, BYTE* owner, unsigned n, DWORD v0, DWORD v1, DWORD v2)
{
    DWORD v[3] = {v0, v1, v2};
    LinkIntoOwner(s, owner);
    BYTE* p = n ? CallC<BYTE*>(Vv_Allocate, n * 12) : 0;
    F(BYTE*, s, 20) = p;
    F(BYTE*, s, 24) = p;
    F(BYTE*, s, 28) = p + n * 12;
    F(BYTE*, s, 24) = SchValues_Fill(p, n, (const BYTE*)v);
    F(void*, s, 0) = g_vtbl64c128;
    Pr_ReleaseInline((BYTE*)v);
    return s;
}

// [FUN_4b6e20] writes number value n (by value) on port p in its written form
void __cdecl Sch_WriteNumber(DWORD n0, DWORD n1, DWORD n2, const DWORD* pv)
{
    BYTE* o = (BYTE*)n0;
    if (!o)
        CallC(FUN_4a52e0, 0, 1);
    if (n2 & 0x10000)
        CallC(FUN_4a52e0, 4, 1);
    BYTE* str[3] = {0, 0, 0};
    CallThis(Vv_NumberWrite, o, str);
    DWORD c[3];
    CopyRef((BYTE*)c, (const BYTE*)pv);
    CallC(Vv_PortWriteString, Sch_At(str, 0, 0), c[0], c[1], c[2]);
    if (int len = (int)(str[2] - str[0]))
        CallC(FUN_44b4a0, str[0], len);
    if (!(n2 & 0x10000) && o && --F(int, o, 4) == 0)
        CallVirt(o, 12, 1);
}

} // extern "C"

// [$L_4b4ee0] destructor of the reader built by Item4b4e10_Construct: its buffer (+52), its value (+40), its deque
extern "C" void __fastcall Item4b4e10_Destroy(BYTE* s)
{
    if (int n = (int)(F(BYTE*, s, 60) - F(BYTE*, s, 52)))
        CallC(FUN_44b4a0, F(BYTE*, s, 52), n);
    Pr_ReleaseInline(s + 40);
    Deque_Destroy(s);
}

// === reading digits from a character deque (FUN_4b1b90)

extern "C" void Rd_Copy16(); KING_ALIAS("_Rd_Copy16", "@Sch_Copy16@12")
extern "C" void Rd_Advance(); KING_ALIAS("_Rd_Advance", "@SchDequeIt_Advance@12")
extern "C" void Rd_AddSigned(); KING_ALIAS("_Rd_AddSigned", "_Int_AddSigned")
extern "C" void Rd_Mul(); KING_ALIAS("_Rd_Mul", "_Int_Mul")
ASM_VAR(char, s_rd671400, $L_671400)

// a big integer freed unless it is a small one (+2 == 0)
static void Rd_Free(WORD* n)
{
    if (n && n[1])
        operator delete(n);
}

extern "C" {

// [FUN_4b1b90] reads the digits in radix r from the front of the character deque q (4-byte characters,
// 512-byte blocks; the front iterator at +8), consuming them, into *out; 'A'..'F' count from 0 as in the
// original; a digit not below r is a type error
DWORD* __cdecl Deque_ReadDigits(DWORD* out, int r, BYTE* q)
{
    WORD* acc = CallC<WORD*>(FUN_48ade0, 0, 0);
    DWORD* it = (DWORD*)(q + 8);
    for (;;) {
        DWORD t1[4], t2[4];
        CallThis(Rd_Copy16, t1, it);
        CallThis(Rd_Copy16, t2, CallThis<DWORD*>(Rd_Advance, t1, 0));
        char c = (char)toupper(*(int*)t2[0]);
        int d;
        if (c >= 'A' && c <= 'F')
            d = c - 'A';
        else if (c >= '0' && c <= '9')
            d = c - '0';
        else
            break;
        if (d >= r)
            CallC(FUN_4a5420, 20, 768, &s_rd671400);
        WORD* m = CallC<WORD*>(FUN_48ade0, 0, r);
        acc = CallC<WORD*>(Rd_Mul, acc, m, acc);
        Rd_Free(m);
        WORD* a = CallC<WORD*>(FUN_48ade0, 0, d);
        acc = CallC<WORD*>(Rd_AddSigned, acc, 0, a, 0, acc);
        Rd_Free(a);
        if (it[0] != F(DWORD, q, 16) - 4)
            it[0] += 4;
        else {
            CallC(FUN_44b4a0, F(void*, q, 12), 512);
            DWORD* node = (DWORD*)(F(DWORD, q, 20) + 4);
            it[3] = (DWORD)node;
            it[1] = *node;
            it[2] = *node + 512;
            it[0] = F(DWORD, q, 12);
        }
    }
    *out = CallC<DWORD>(FUN_48acd0, 0, acc);
    Rd_Free(acc);
    return out;
}

} // extern "C"

// === a rational's value as a double (FUN_48a7c0)

extern "C" void __cdecl BigInt_DivMod(WORD** a, WORD** b, WORD** q, WORD** r);
extern "C" double __fastcall IntRef_ToDouble(void* const* s, void*);
ASM_VAR(BYTE, g_rq6703f0, $L_6703f0)
ASM_VAR(double, g_rq64bc30, $L_64bc30)
ASM_VAR(double, g_rq64beb0, $L_64beb0)

static void Rq_Free(WORD* p)
{
    if (p && p[1])
        operator delete(p);
}

extern "C" {

// [FUN_48a7c0] num / den (big integers: word count, owned flag, sign 1 or -1, the words from the top) as a double:
// the quotient, plus, when it is exact as a double, the remainder over den by long division bit by bit until den's
// double stops growing
double __cdecl Rational_ToDouble(WORD** num, WORD** den)
{
    WORD* q = (WORD*)&g_rq6703f0;
    WORD* r = (WORD*)&g_rq6703f0;
    BigInt_DivMod(num, den, &q, &r);
    double v = IntRef_ToDouble((void* const*)&q, 0);
    double res = v;
    WORD rl;
    if (v < g_rq64bc30 && v > g_rq64beb0 && (rl = r[0]) != 0)
    {
        int sgn = r[2] == 1 ? 1 : -1;
        double n = 0, d = 0;
        const WORD* dw = *den;
        for (int i = dw[0] - 1; i >= 0; i--)
        {
            bool stop = false;
            for (unsigned m = 0x8000; m; m >>= 1)
            {
                if (d + 1.0 == d)
                {
                    stop = true;
                    break;
                }
                d += d;
                if (dw[3 + i] & m)
                    d += 1.0;
                if (i < rl)
                {
                    n += n;
                    if (r[3 + i] & m)
                        n += 1.0;
                }
            }
            if (stop)
                break;
        }
        if (sgn < 0)
            n = -n;
        res = n / d + v;
    }
    Rq_Free(r);
    Rq_Free(q);
    return res;
}

} // extern "C"

// ---------------------------------------------------------------------------
// Batch 32: the file port's header

extern "C" void Fp32_End(); KING_ALIAS("_Fp32_End", "@SchItem_Value@12")
extern "C" void Fp32_PoolFree(); KING_ALIAS("_Fp32_PoolFree", "_Ai_PoolFree")
extern "C" void Fp32_Seed(); KING_ALIAS("_Fp32_Seed", "@Rng_Seed@12")
ASM_VAR(char, fp32_tag671510, $L_671510)
ASM_VAR(int, fp32_seed67150c, $L_67150c)

// drops the last character of the deque whose end iterator is at d (cur, first, last, node)
static void Fp32_PopBack(BYTE* d)
{
    if (F(char*, d, 0) == F(char*, d, 4)) {
        CallC(Fp32_PoolFree, F(char*, d, 4), 512);
        F(char**, d, 12) -= 1;
        F(char*, d, 4) = *F(char**, d, 12);
        F(char*, d, 8) = F(char*, d, 4) + 512;
        F(char*, d, 0) = F(char*, d, 8);
    }
    F(char*, d, 0) -= 1;
}

extern "C" {

// [FUN_4b54b0] appends the file's first 4 bytes to the buffer (deque at +24); when they are the cipher tag
// (671510) they are dropped again, +64 set and the reader's generator (+68) seeded; nothing read sets bit 2 of +16
void __fastcall FilePort_ReadHeader(BYTE* self)
{
    BYTE fr[232];   // the tag at +0, the generator at +4
    int n = (int)fread(fr, 1, 4, F(void*, self, 12));
    if (!n) {
        F(int, self, 16) |= 2;
        return;
    }
    Val207_It end;
    CallThis(Fp32_End, self + 24, &end);
    CallThis(FUN_4ac0b0, self + 24, end, (char*)fr, (char*)fr + n);
    if (n != 4 || memcmp(fr, &fp32_tag671510, 4))
        return;
    F(BYTE, self, 64) = 1;
    for (int i = 0; i < 4; i++)
        Fp32_PopBack(self + 48);
    CallThis(Fp32_Seed, fr + 4, fp32_seed67150c);
    memcpy(self + 68, fr + 4, 228);
}

}
