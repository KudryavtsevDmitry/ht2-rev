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

ASM_PROC(FUN_44b4a0)    // the AI's allocator: free (p, size)
extern "C" void FUN_4b2b60(); KING_ALIAS("_FUN_4b2b60", "@Reader_Fill@12")
ASM_PROC(FUN_4b20d0)
extern "C" void FUN_4b58c0(); KING_ALIAS("_FUN_4b58c0", "@SchState_Construct@8")
extern "C" void FUN_4b5490(); KING_ALIAS("_FUN_4b5490", "@SchReader_Close@8")
ASM_PROC(FUN_4b5b70)
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

// ---------------------------------------------------------------------------
// medium-sized functions: a node's constructor, the reader's line buffer and
// its error line

ASM_PROC(FUN_44c220)    // vector<char>::insert(pos, first, last)
ASM_PROC(FUN_44bef0)    // vector<char>::insert(pos, &c)
extern "C" void FUN_4a20a0(); KING_ALIAS("_FUN_4a20a0", "@DequeDw_PushBackAux@12")    // deque<int>::push_back when the block is full
ASM_PROC(FUN_4a52e0)    // script error (code, fatal)
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
KING_ALIAS("_Stl_Allocate", "FUN_44b740")
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

ASM_PROC(FUN_4b6f70)

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
ASM_PROC(FUN_4b54b0)    // file port: reads the file
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
ASM_PROC(FUN_44bef0)    // vector<char>::insert(pos, &c)
ASM_PROC(FUN_44c220)    // vector<char>::insert(pos, first, last)

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

ASM_PROC(FUN_48a7c0)    // a big integer's value as a double (digits, end)

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
