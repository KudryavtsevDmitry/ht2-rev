// evaluator.cpp - Sch_eval.cpp of the game, rewritten from king.masm: small
// functions of the AI's script evaluator: reference-counted values and the
// scopes holding them, the constructors of the expression nodes, the map and
// list helpers, pools of freed blocks.
//
// Building with  nmake ASM_EVALUATOR=1  makes king.masm assemble the original
// procedures again.

#include "king.h"

extern "C" BYTE* __cdecl Values_Copy(const BYTE* first, const BYTE* last, BYTE* dest);  // natives.cpp
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl _stricmp(const char* a, const char* b);
extern "C" void** __stdcall List_Erase(void** out, void** node);     // aimain.cpp

ASM_PROC(FUN_4a36f0)    // destructor of the value holders (map and list entries)
ASM_PROC(FUN_44b4a0)    // the AI's allocator: free (p, size)
ASM_PROC(FUN_44b740)    // the AI's allocator: allocate (size)
ASM_PROC(FUN_4a52e0)    // script error (code, fatal)
ASM_PROC(FUN_4ab570)    // constructs a 44-byte element at p from x
ASM_PROC(FUN_4aa460)    // vector<44 bytes>::insert_aux
ASM_PROC(FUN_4ace70)    // copy (first, last, deque iterator, tag)
ASM_PROC(FUN_4afe80)    // list: a new node holding x
ASM_PROC(FUN_4a4950)
extern "C" void FUN_454580(); KING_ALIAS("_FUN_454580", "@Str_CopyConstruct@12")
ASM_VAR(AsmLabel, s_errNotSymbolList, $L_67115c)
extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")
extern "C" void FUN_4b6500(); KING_ALIAS("_FUN_4b6500", "@SchSymbol_Construct@12")
extern "C" void FUN_4a8580(); KING_ALIAS("_FUN_4a8580", "@SchNode4a8580_Construct@8")
extern "C" void FUN_4910a0(); KING_ALIAS("_FUN_4910a0", "@RingB_Construct@12")
ASM_PROC(FUN_491380)
extern "C" void FUN_491040(); KING_ALIAS("_FUN_491040", "@RingA_Construct@20")
extern "C" void FUN_4a81d0(); KING_ALIAS("_FUN_4a81d0", "@SchNode4a81d0_Construct@8")
extern "C" void FUN_4a9040(); KING_ALIAS("_FUN_4a9040", "@SchNode4a9040_Construct@8")
extern "C" void FUN_4a86f0(); KING_ALIAS("_FUN_4a86f0", "@SchNode4a86f0_Construct@8")
extern "C" void FUN_4a8a90(); KING_ALIAS("_FUN_4a8a90", "@SchNode4a8a90_Construct@8")
extern "C" void FUN_4a8bd0(); KING_ALIAS("_FUN_4a8bd0", "@SchNode4a8bd0_Construct@8")
extern "C" void FUN_4a9770(); KING_ALIAS("_FUN_4a9770", "@SchNode4a9770_Construct@8")
extern "C" void FUN_4a9a70(); KING_ALIAS("_FUN_4a9a70", "@SchNode4a9a70_Construct@8")
extern "C" void FUN_4a9e80(); KING_ALIAS("_FUN_4a9e80", "@SchNode4a9e80_Construct@8")
extern "C" void FUN_4b62c0(); KING_ALIAS("_FUN_4b62c0", "@SchString_Construct@12")
ASM_VAR(AsmLabel, s_tIdentifier, $L_66ce4c)
ASM_VAR(AsmLabel, s_tUnspecified, $L_671168)
ASM_VAR(AsmLabel, s_tBegin, $L_671124)
ASM_VAR(AsmLabel, s_tClosure, $L_6705f4)
ASM_VAR(AsmLabel, s_tContinuation, $L_6705fc)
ASM_VAR(AsmLabel, s_tLambda, $L_67111c)
ASM_VAR(AsmLabel, s_tDefine, $L_67113c)
ASM_VAR(AsmLabel, s_tIf, $L_67112c)
ASM_VAR(AsmLabel, s_tQuote, $L_671130)
ASM_VAR(AsmLabel, s_tSet, $L_671138)
ASM_VAR(AsmLabel, s_tEval, $L_671144)
ASM_VAR(AsmLabel, s_tCallCC, $L_67114c)
ASM_VAR(AsmLabel, s_tApply, $L_671154)
ASM_VAR(AsmLabel, s_tString, $L_66ca58)
ASM_VAR(AsmLabel, g_vtblUnspecified, $L_64c034)

ASM_VAR(int, g_69224c, $L_69224c)
ASM_VAR(AsmLabel, g_strImmediate, $L_671104)    // "<IMMEDIATE>"
typedef void* Blocks[1];
ASM_VAR(int, g_free44Count, $L_693650)          // freed 44-byte blocks
ASM_VAR(Blocks, g_free44, $L_692650)
ASM_VAR(int, g_free32Count, $L_694658)          // freed 32-byte blocks
ASM_VAR(Blocks, g_free32, $L_693658)
ASM_VAR(AsmLabel, g_vtbl64bf68, $L_64bf68)
ASM_VAR(AsmLabel, g_vtbl64bf70, $L_64bf70)
ASM_VAR(AsmLabel, g_vtbl64bf80, $L_64bf80)
ASM_VAR(AsmLabel, g_vtbl64bf94, $L_64bf94)
ASM_VAR(AsmLabel, g_vtbl64bfa8, $L_64bfa8)
ASM_VAR(AsmLabel, g_vtbl64bfbc, $L_64bfbc)
ASM_VAR(AsmLabel, g_vtbl64bfd0, $L_64bfd0)
ASM_VAR(AsmLabel, g_vtbl64bfe4, $L_64bfe4)
ASM_VAR(AsmLabel, g_vtbl64bff8, $L_64bff8)
ASM_VAR(AsmLabel, g_vtbl64c00c, $L_64c00c)
ASM_VAR(AsmLabel, g_vtbl64c020, $L_64c020)

static void AiFree(void* p, int size) { CallC(FUN_44b4a0, p, size); }
static void SchError(int code) { CallC(FUN_4a52e0, code, 1); }

// A value of the scripts: an object with its reference count at +4 (vtbl+4
// its name, vtbl+12 the deleting destructor), or with SCH_IMMEDIATE in the
// flags an immediate value in obj and m_4.
struct SchValue {
    BYTE* obj;
    int   m_4;
    DWORD flags;
    int   m_12;
};
enum { SCH_IMMEDIATE = 0x10000 };

// obj, m_4 and flags, as the values' copy constructor copies them
static void CopyRef(SchValue* to, const SchValue* from)
{
    if (from->flags & SCH_IMMEDIATE) {
        to->flags = from->flags;
        to->obj = from->obj;
        to->m_4 = from->m_4;
        return;
    }
    to->m_4 = 0;
    if (from->obj) {
        to->obj = from->obj;
        to->flags = from->flags;
        F(int, to->obj, 4)++;
    } else {
        to->obj = 0;
        to->flags = 0;
    }
}

extern "C" SchValue* __fastcall SchValue_Copy(SchValue* self, void*, const SchValue* from)
{
    CopyRef(self, from);
    self->m_12 = from->m_12;
    return self;
}

// the value at index of the scope depth levels up (+148 the enclosing scope,
// +132 its values, 12 bytes each); *found gets the index, or -1 when there is
// no such scope
extern "C" SchValue* __fastcall SchScope_Get(BYTE* self, void*, SchValue* out, int depth, int index, int* found)
{
    *found = -1;
    if (depth) {
        if (!F(int, self, 156) && !F(BYTE*, self, 148)) {
            out->obj = 0;
            out->m_4 = 0;
            out->flags = 0;
            return out;
        }
        SchScope_Get(F(BYTE*, self, 148), 0, out, depth - 1, index, found);
        return out;
    }
    *found = index;
    CopyRef(out, (const SchValue*)(F(BYTE*, self, 132) + index * 12));
    return out;
}

// a pair: the first value without its m_12 at +0, the second at +12
extern "C" BYTE* __fastcall SchPair_Construct(BYTE* self, void*, const SchValue* a, const SchValue* b)
{
    CopyRef((SchValue*)self, a);
    SchValue* second = (SchValue*)(self + 12);
    CopyRef(second, b);
    second->m_12 = b->m_12;
    return self;
}

// map<name, value>: frees the subtree at node (44-byte nodes, the value at +16)
extern "C" void __fastcall SchMap_Erase(BYTE* self, void*, BYTE* node)
{
    while (node) {
        SchMap_Erase(self, 0, F(BYTE*, node, 12));
        BYTE* left = F(BYTE*, node, 8);
        CallThis(FUN_4a36f0, node + 16);
        AiFree(node, 44);
        node = left;
    }
}

// --it of the map (+0 the colour, +4 parent, +8 left, +12 right)
extern "C" void __fastcall SchMapIt_Dec(BYTE** self, void*)
{
    BYTE* x = *self;
    if (x[0] == 0 && F(BYTE*, F(BYTE*, x, 4), 4) == x) {
        *self = F(BYTE*, x, 12);
        return;
    }
    BYTE* y = F(BYTE*, x, 8);
    if (y) {
        while (F(BYTE*, y, 12))
            y = F(BYTE*, y, 12);
        *self = y;
        return;
    }
    y = F(BYTE*, x, 4);
    if (x == F(BYTE*, y, 8)) {
        do {
            *self = y;
            y = F(BYTE*, y, 4);
        } while (*self == F(BYTE*, y, 8));
    }
    *self = y;
}

// the order of the map: by the names (+12 of the objects), ignoring case
extern "C" bool __stdcall SchValue_LessByName(const SchValue* a, const SchValue* b)
{
    if (!b->obj)
        SchError(0);
    if (b->flags & SCH_IMMEDIATE)
        SchError(4);
    const char* nameB = F(const char*, b->obj, 12);
    if (!a->obj)
        SchError(0);
    if (a->flags & SCH_IMMEDIATE)
        SchError(4);
    return _stricmp(F(const char*, a->obj, 12), nameB) < 0;
}

extern "C" BYTE* __fastcall Sch4a51b0_Construct(BYTE* self, void*, const int* a, const BYTE* b)
{
    F(int, self, 0) = *a;
    self[4] = *b;
    return self;
}

extern "C" BYTE* __fastcall SchHolder_DeletingDestructor(BYTE* self, void*, int flags)
{
    CallThis(FUN_4a36f0, self);
    if (flags & 1)
        operator delete(self);
    return self;
}

extern "C" int __cdecl Sch_Exchange69224c(int v)
{
    int old = g_69224c;
    g_69224c = v;
    return old;
}

extern "C" SchValue* __cdecl SchValue_Immediate(SchValue* out, int unused, int v, int v2)
{
    out->obj = (BYTE*)v;
    out->m_4 = v2;
    out->flags = 0x10021;
    return out;
}

extern "C" const char* __fastcall SchValue_Name(const SchValue* self, void*)
{
    if (self->flags & SCH_IMMEDIATE)
        return g_strImmediate;
    return CallVirt<const char*>(self->obj, 4);
}

// ---------------------------------------------------------------------------
// Expression nodes

extern "C" BYTE* __fastcall SchNode4a7060_Construct(BYTE* self, void*)
{
    for (int i = 4; i <= 40; i += 4)
        F(int, self, i) = 0;
    F(void*, self, 0) = g_vtbl64bf68;
    return self;
}

extern "C" BYTE* __fastcall SchNode_Construct(BYTE* self, void*)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl64bf70;
    return self;
}

extern "C" BYTE* __fastcall SchRef_Construct(BYTE* self, void*, BYTE* obj)
{
    F(BYTE*, self, 0) = obj;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    if (obj) {
        F(int, obj, 4)++;
        F(int, self, 8) = CallVirt<int>(obj, 0) & 255;
    }
    return self;
}

// the subclasses of SchNode that only set their vtable
static BYTE* NodeWithVtbl(BYTE* self, void* vtbl)
{
    SchNode_Construct(self, 0);
    F(void*, self, 0) = vtbl;
    return self;
}
extern "C" BYTE* __fastcall SchNode4a81d0_Construct(BYTE* self, void*) { return NodeWithVtbl(self, g_vtbl64bf80); }
extern "C" BYTE* __fastcall SchNode4a8580_Construct(BYTE* self, void*) { return NodeWithVtbl(self, g_vtbl64bf94); }
extern "C" BYTE* __fastcall SchNode4a86f0_Construct(BYTE* self, void*) { return NodeWithVtbl(self, g_vtbl64bfa8); }
extern "C" BYTE* __fastcall SchNode4a8a90_Construct(BYTE* self, void*) { return NodeWithVtbl(self, g_vtbl64bfbc); }
extern "C" BYTE* __fastcall SchNode4a8bd0_Construct(BYTE* self, void*) { return NodeWithVtbl(self, g_vtbl64bfd0); }
extern "C" BYTE* __fastcall SchNode4a9040_Construct(BYTE* self, void*) { return NodeWithVtbl(self, g_vtbl64bfe4); }
extern "C" BYTE* __fastcall SchNode4a9770_Construct(BYTE* self, void*) { return NodeWithVtbl(self, g_vtbl64bff8); }
extern "C" BYTE* __fastcall SchNode4a9a70_Construct(BYTE* self, void*) { return NodeWithVtbl(self, g_vtbl64c00c); }
extern "C" BYTE* __fastcall SchNode4a9e80_Construct(BYTE* self, void*) { return NodeWithVtbl(self, g_vtbl64c020); }

// ---------------------------------------------------------------------------
// vector<44-byte elements>, vector<12-byte elements>

extern "C" BYTE* __fastcall Vec44_At(BYTE* self, void*, int i)
{
    return F(BYTE*, self, 0) + i * 44;
}

extern "C" BYTE* __fastcall Vec44_Back(BYTE* self, void*)
{
    return F(BYTE*, self, 4) - 44;
}

extern "C" void __fastcall Vec44_PushBack(BYTE* self, void*, const void* x)
{
    BYTE* end = F(BYTE*, self, 4);
    if (end != F(BYTE*, self, 8)) {
        CallC(FUN_4ab570, end, x);
        F(BYTE*, self, 4) += 44;
    } else
        CallThis(FUN_4aa460, self, end, x);
}

// the object of a value; a script error for none and for immediates
extern "C" BYTE* __fastcall SchValue_Object(const SchValue* self, void*)
{
    if (!self->obj)
        SchError(0);
    if (self->flags & SCH_IMMEDIATE)
        SchError(4);
    return self->obj;
}

extern "C" void __cdecl SchValue_Release(SchValue* v)
{
    if ((v->flags & SCH_IMMEDIATE) || !v->obj)
        return;
    F(int, v->obj, 4)--;
    BYTE* o = v->obj;
    if (F(int, o, 4) == 0 && o)
        CallVirt(o, 12, 1);
}

// empty, with room for n elements (the allocator by value is unused)
extern "C" BYTE** __fastcall Vec12_Reserve(BYTE** self, void*, int n, int alloc)
{
    BYTE* p = 0;
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    if (n)
        p = CallC<BYTE*>(FUN_44b740, n * 12);
    self[0] = p;
    self[1] = p;
    self[2] = p + n * 12;
    return self;
}

extern "C" SchValue* __fastcall SchItem_Value(const BYTE* self, void*, SchValue* out)
{
    *out = *(const SchValue*)(self + 24);
    return out;
}

// the copy constructor of 16-byte values and deque iterators
extern "C" DWORD* __fastcall Sch_Copy16(DWORD* self, void*, const DWORD* from)
{
    self[0] = from[0];
    self[1] = from[1];
    self[2] = from[2];
    self[3] = from[3];
    return self;
}

// ---------------------------------------------------------------------------
// deque<char> (blocks of 512 chars): iterators and copies. The copies take
// and return iterators by value; the returned one through out.

struct DequeIt {
    char*  cur;
    char*  first;
    char*  last;
    char** node;
};

// b - a
static int Distance(const DequeIt& a, const DequeIt& b)
{
    return ((((int)b.node - (int)a.node) >> 2) << 9) - (int)b.first + (int)b.cur - (int)a.cur + (int)a.last - 512;
}

extern "C" DequeIt* __fastcall DequeIt_Advance(DequeIt* self, void*, int n)
{
    int off = (int)(self->cur - self->first) + n;
    if (off >= 0 && off < 512) {
        self->cur += n;
        return self;
    }
    int blocks = off > 0 ? off / 512 : -1 - (int)((unsigned)(-1 - off) >> 9);
    self->node += blocks;
    self->first = *self->node;
    self->last = self->first + 512;
    self->cur = self->first + (off - blocks * 512);
    return self;
}

extern "C" int __fastcall DequeIt_Diff(const DequeIt* self, void*, const DequeIt* o)
{
    return Distance(*o, *self);
}

extern "C" DequeIt* __cdecl Deque_CopyChars(DequeIt* out, const char* first, const char* last, DequeIt dest)
{
    CallC(FUN_4ace70, out, first, last, dest, 0);
    return out;
}

extern "C" DequeIt* __cdecl Deque_CopyLoop(DequeIt* out, DequeIt first, DequeIt last, DequeIt dest)
{
    int n = Distance(first, last);
    char* s = first.cur;
    char* sEnd = first.last;
    char** sNode = first.node;
    char* d = dest.cur;
    char* dFirst = dest.first;
    char* dEnd = dest.last;
    char** dNode = dest.node;
    for (; n > 0; n--) {
        *d = *s++;
        if (s == sEnd) {
            s = *++sNode;
            sEnd = s + 512;
        }
        if (++d == dEnd) {
            dFirst = d = *++dNode;
            dEnd = d + 512;
        }
    }
    out->first = dFirst;
    out->node = dNode;
    out->cur = d;
    out->last = dEnd;
    return out;
}

extern "C" DequeIt* __cdecl Deque_CopyCharsLoop(DequeIt* out, const char* first, const char* last, DequeIt dest)
{
    int n = (int)(last - first);
    char* d = dest.cur;
    char* dFirst = dest.first;
    char* dEnd = dest.last;
    char** dNode = dest.node;
    for (; n > 0; n--) {
        *d = *first++;
        if (++d == dEnd) {
            dFirst = d = *++dNode;
            dEnd = d + 512;
        }
    }
    out->last = dEnd;
    out->first = dFirst;
    out->cur = d;
    out->node = dNode;
    return out;
}

extern "C" DequeIt* __cdecl Deque_CopyBackwardLoop(DequeIt* out, DequeIt first, DequeIt last, DequeIt dest)
{
    int n = Distance(first, last);
    char* s = last.cur;
    char* sFirst = last.first;
    char** sNode = last.node;
    char* d = dest.cur;
    char* dFirst = dest.first;
    char* dEnd = dest.last;
    char** dNode = dest.node;
    for (; n > 0; n--) {
        if (s == sFirst) {
            sFirst = *--sNode;
            s = sFirst + 512;
        }
        s--;
        if (d == dFirst) {
            dFirst = *--dNode;
            dEnd = dFirst + 512;
            d = dEnd;
        }
        *--d = *s;
    }
    out->node = dNode;
    out->last = dEnd;
    out->cur = d;
    out->first = dFirst;
    return out;
}

// the layers of std::copy above the loops; the extra arguments the original
// passes down (tags) are unused
extern "C" DequeIt* __cdecl Deque_Copy4acb90(DequeIt* out, DequeIt first, DequeIt last, DequeIt dest)
{
    return Deque_CopyLoop(out, first, last, dest);
}

extern "C" DequeIt* __cdecl Deque_Copy4ad130(DequeIt* out, DequeIt first, DequeIt last, DequeIt dest)
{
    return Deque_Copy4acb90(out, first, last, dest);
}

extern "C" DequeIt* __cdecl Deque_Copy4acea0(DequeIt* out, DequeIt first, DequeIt last, DequeIt dest)
{
    return Deque_Copy4ad130(out, first, last, dest);
}

extern "C" DequeIt* __cdecl Deque_Copy4acb40(DequeIt* out, DequeIt first, DequeIt last, DequeIt dest)
{
    return Deque_Copy4acea0(out, first, last, dest);
}

extern "C" DequeIt* __cdecl Deque_CopyChars4acbe0(DequeIt* out, const char* first, const char* last, DequeIt dest)
{
    return Deque_CopyCharsLoop(out, first, last, dest);
}

extern "C" DequeIt* __cdecl Deque_CopyChars4ad0f0(DequeIt* out, const char* first, const char* last, DequeIt dest)
{
    return Deque_CopyChars4acbe0(out, first, last, dest);
}

// ---------------------------------------------------------------------------
// Pools of freed blocks, lists

extern "C" void* __cdecl Pool44_New(int size)
{
    if (!g_free44Count)
        return operator new(size);
    if (size == 44)
        return g_free44[--g_free44Count];
    return operator new(size);
}

extern "C" void* __cdecl Pool32_New(int size)
{
    if (!g_free32Count)
        return operator new(size);
    if (size == 32)
        return g_free32[--g_free32Count];
    return operator new(size);
}

extern "C" BYTE** __fastcall SchListIt_PostInc(BYTE** self, void*, BYTE** out, int)
{
    BYTE* n = *self;
    *self = *(BYTE**)n;
    *out = n;
    return out;
}

extern "C" BYTE** __fastcall SchList_Begin(BYTE** self, void*, BYTE** out)
{
    *out = *(BYTE**)*self;
    return out;
}

extern "C" BYTE** __fastcall SchList_End(BYTE** self, void*, BYTE** out)
{
    *out = *self;
    return out;
}

// a node holding x before pos
extern "C" BYTE** __stdcall SchList_Insert(BYTE** out, BYTE* pos, int x)
{
    BYTE* n = CallStd<BYTE*>(FUN_4afe80, x);
    F(BYTE*, n, 0) = pos;
    F(BYTE*, n, 4) = F(BYTE*, pos, 4);
    F(BYTE*, F(BYTE*, pos, 4), 0) = n;
    F(BYTE*, pos, 4) = n;
    *out = n;
    return out;
}

extern "C" void __fastcall SchList_PushBack(BYTE** self, void*, int x)
{
    BYTE* end;
    SchList_End(self, 0, &end);
    BYTE* it;
    SchList_Insert(&it, end, x);
}

// the list's destructor (12-byte nodes)
extern "C" void __fastcall SchList_Destroy(BYTE** self, void*)
{
    BYTE* n = F(BYTE*, *self, 0);
    while (n != *self) {
        BYTE* cur = n;
        n = F(BYTE*, n, 0);
        AiFree(cur, 12);
    }
    F(BYTE*, *self, 0) = *self;
    F(BYTE*, *self, 4) = *self;
    AiFree(*self, 12);
}

// the destructor of a list of values (20-byte nodes, the value at +8)
extern "C" void __fastcall SchValueList_Destroy(BYTE** self, void*)
{
    BYTE* n = F(BYTE*, *self, 0);
    while (n != *self) {
        BYTE* cur = n;
        n = F(BYTE*, n, 0);
        SchValue_Release((SchValue*)(cur + 8));
        AiFree(cur, 20);
    }
    F(BYTE*, *self, 0) = *self;
    F(BYTE*, *self, 4) = *self;
    AiFree(*self, 20);
}

extern "C" BYTE** __fastcall SchList_EraseRange(BYTE* self, void*, BYTE** out, BYTE* first, BYTE* last)
{
    while (first != last) {
        BYTE* cur = first;
        first = F(BYTE*, first, 0);
        void* next;
        List_Erase(&next, (void**)cur);
    }
    *out = last;
    return out;
}

// ---------------------------------------------------------------------------
// medium-sized functions: error reports, the name map, value vectors

#include <stdlib.h>

typedef struct _iobuf FILE;
extern "C" int __cdecl fprintf(FILE* f, const char* format, ...);
extern "C" int __cdecl sprintf(char* buf, const char* format, ...);

ASM_PROC(FUN_4a5380)    // the format of an error (code, level)
ASM_PROC(FUN_4a59f0)    // the format of a message (code)
extern "C" void FUN_4b1450(); KING_ALIAS("_FUN_4b1450", "_Port_WriteString")    // shows a message (text, value by copy)
ASM_PROC(FUN_4a5800)
ASM_PROC(FUN_4a36c0)    // a value's destructor
ASM_PROC(FUN_4a4540)    // vector<value>: allocate and copy (n, first, last)
extern "C" SchValue* __fastcall SchValue_Assign(SchValue* self, void*, const SchValue* from);
KING_ALIAS("@SchValue_Assign@12", "@Str_Assign@12")
extern "C" BYTE* __cdecl UninitCopyStrRefs(const BYTE* first, const BYTE* last, BYTE* dest);  // aistl.cpp
typedef int Ints[1];
ASM_VAR(Ints, g_errorOff, $L_691dbc)            // per error code: don't report
ASM_VAR(int, g_lastErrorCode, $L_692244)
ASM_VAR(int, g_lastErrorLevel, $L_692248)
ASM_VAR(FILE*, g_errorFile, $L_670b04)
typedef void (__cdecl* ErrorHandler)(int code, int level, const char* text);

// reports a script error to the log file and the handler; level 1 (and
// below, except 0) ends the program
// returns what the original leaves in EAX: the code's "off" flag, or the level
extern "C" int __cdecl Sch_Error(int code, int level, const char* a, const char* b)
{
    if (g_errorOff[code])
        return g_errorOff[code];
    const char* fmt = CallC<const char*>(FUN_4a5380, code, level);
    g_lastErrorCode = code;
    g_lastErrorLevel = level;
    if (g_errorFile)
        fprintf(g_errorFile, fmt, a, b);
    if (g_69224c) {
        char* text = (char*)operator new(strlen(fmt) + strlen(a) + strlen(b) + 20);
        sprintf(text, fmt, a, b);
        ((ErrorHandler)g_69224c)(code, level, text);
        operator delete(text);
    }
    if (g_lastErrorLevel != 0 && g_lastErrorLevel <= 1)
        exit(-1);
    return g_lastErrorLevel;
}

// the name map's lower bound of a key (case-insensitive)
extern "C" BYTE** __fastcall NameMap_LowerBound(BYTE** self, void*, BYTE** out, const SchValue* key)
{
    BYTE* y = *self;
    for (BYTE* x = F(BYTE*, *self, 4); x;) {
        const char* k = F(const char*, SchValue_Object(key, 0), 12);
        if (_stricmp(F(const char*, SchValue_Object((const SchValue*)(x + 16), 0), 12), k) < 0)
            x = F(BYTE*, x, 12);
        else {
            y = x;
            x = F(BYTE*, x, 8);
        }
    }
    if (y != *self) {
        const char* nk = F(const char*, SchValue_Object((const SchValue*)(y + 16), 0), 12);
        if (_stricmp(F(const char*, SchValue_Object(key, 0), 12), nk) >= 0) {
            *out = y;
            return out;
        }
    }
    *out = *self;
    return out;
}

// red-black tree: the colour fix-up after an insertion (0 red, 1 black)
struct RbNode {
    char    black;
    RbNode* parent;
    RbNode* left;
    RbNode* right;
};

static void RotateLeft(RbNode* x, RbNode** root)
{
    RbNode* y = x->right;
    x->right = y->left;
    if (y->left)
        y->left->parent = x;
    y->parent = x->parent;
    if (x == *root)
        *root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

static void RotateRight(RbNode* x, RbNode** root)
{
    RbNode* y = x->left;
    x->left = y->right;
    if (y->right)
        y->right->parent = x;
    y->parent = x->parent;
    if (x == *root)
        *root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

extern "C" void __cdecl NameMap_Rebalance(RbNode* x, RbNode** root)
{
    x->black = 0;
    while (x != *root && !x->parent->black) {
        RbNode* p = x->parent;
        RbNode* g = p->parent;
        if (p == g->left) {
            RbNode* y = g->right;
            if (y && !y->black) {
                p->black = 1;
                y->black = 1;
                x->parent->parent->black = 0;
                x = x->parent->parent;
                continue;
            }
            if (x == p->right) {
                x = p;
                RotateLeft(x, root);
            }
            x->parent->black = 1;
            x->parent->parent->black = 0;
            RotateRight(x->parent->parent, root);
        } else {
            RbNode* y = g->left;
            if (y && !y->black) {
                p->black = 1;
                y->black = 1;
                x->parent->parent->black = 0;
                x = x->parent->parent;
                continue;
            }
            if (x == p->left) {
                x = p;
                RotateRight(x, root);
            }
            x->parent->black = 1;
            x->parent->parent->black = 0;
            RotateLeft(x->parent->parent, root);
        }
    }
    (*root)->black = 1;
}

// vector<value of 12 bytes>::operator=
extern "C" BYTE** __fastcall Values_Assign(BYTE** d, void*, BYTE* const* s)
{
    if ((const void*)s == (void*)d)
        return d;
    unsigned n = (s[1] - s[0]) / 12;
    if (n > (unsigned)((d[2] - d[0]) / 12)) {
        BYTE* p = CallThis<BYTE*>(FUN_4a4540, d, n, s[0], s[1]);
        for (BYTE* e = d[0]; e != d[1]; e += 12)
            CallThis(FUN_4a36c0, e);
        if ((d[2] - d[0]) / 12)
            AiFree(d[0], ((d[2] - d[0]) / 12) * 12);
        d[0] = p;
        d[2] = p + n * 12;
    } else {
        BYTE* to = d[0];
        BYTE* from = s[0];
        if ((unsigned)((d[1] - d[0]) / 12) >= n) {
            for (int k = (s[1] - s[0]) / 12; k > 0; k--, from += 12, to += 12)
                SchValue_Assign((SchValue*)to, 0, (const SchValue*)from);
            for (; to != d[1]; to += 12)
                CallThis(FUN_4a36c0, to);
        } else {
            for (int k = (((d[1] - d[0]) / 12) * 12) / 12; k > 0; k--, from += 12, to += 12)
                SchValue_Assign((SchValue*)to, 0, (const SchValue*)from);
            UninitCopyStrRefs(s[0] + ((d[1] - d[0]) / 12) * 12, s[1], d[1]);
        }
    }
    d[1] = d[0] + n * 12;
    return d;
}

// vector<44-byte element>::erase(first, last); the elements have a vtable
extern "C" BYTE* __fastcall Vec44_Erase(BYTE** self, void*, BYTE* first, BYTE* last)
{
    BYTE* d = first;
    BYTE* s = last;
    for (int n = (self[1] - last) / 44; n > 0; n--, d += 44, s += 44) {
        SchValue_Assign((SchValue*)(d + 4), 0, (const SchValue*)(s + 4));
        Values_Assign((BYTE**)(d + 16), 0, (BYTE* const*)(s + 16));
        F(DWORD, d, 28) = F(DWORD, s, 28);
        SchValue_Assign((SchValue*)(d + 32), 0, (const SchValue*)(s + 32));
    }
    for (BYTE* e = self[1]; d != e; d += 44)
        CallVirt(d, 4, 0);
    self[1] -= ((last - first) / 44) * 44;
    return first;
}

// a message from a node: its text, shown twice around FUN_4a5800, with the
// node's value at +96 passed by copy
struct ValueArg { BYTE* obj; int m_4; DWORD flags; };

static ValueArg NodeValue(BYTE* p)
{
    ValueArg v;
    CopyRef((SchValue*)&v, (const SchValue*)(p + 96));
    return v;
}

// returns what the last FUN_4b1450 returned: the original leaves it in EAX,
// and FUN_4a5c40 passes it on
static int ShowMessage(BYTE* p, const char* text)
{
    typedef int (__cdecl* Show)(const char*, ValueArg);
    ((Show)FUN_4b1450)(text, NodeValue(p));
    CallC(FUN_4a5800, p);
    int r = ((Show)FUN_4b1450)(text, NodeValue(p));
    F(int, p, 120) = 1;
    return r;
}

extern "C" int __cdecl Sch_Message0(BYTE* p, int code)
{
    char text[2048];
    sprintf(text, CallC<const char*>(FUN_4a59f0, code));
    return ShowMessage(p, text);
}

extern "C" int __cdecl Sch_Message1(BYTE* p, int code, int a)
{
    char text[2048];
    sprintf(text, CallC<const char*>(FUN_4a59f0, code), a);
    return ShowMessage(p, text);
}

extern "C" int __cdecl Sch_Message2(BYTE* p, int code, int a, int b)
{
    char text[2048];
    sprintf(text, CallC<const char*>(FUN_4a59f0, code), a, b);
    return ShowMessage(p, text);
}

// ---------------------------------------------------------------------------
// copies of values and of the 44-byte elements, the deque's map

extern "C" BYTE* __fastcall MultValue_DeletingDestructor(BYTE* self, void*, int flags);

// vector<value>::operator= as the element copies inline it; the elements
// cut off are destroyed by SchValue_Release or the deleting destructor
static void Values_AssignInline(BYTE** d, BYTE* const* s, bool release)
{
    unsigned n = (s[1] - s[0]) / 12;
    if (n > (unsigned)((d[2] - d[0]) / 12)) {
        BYTE* p = CallThis<BYTE*>(FUN_4a4540, d, n, s[0], s[1]);
        for (BYTE* e = d[0]; e != d[1]; e += 12)
            if (release)
                SchValue_Release((SchValue*)e);
            else
                MultValue_DeletingDestructor(e, 0, 0);
        if ((d[2] - d[0]) / 12)
            AiFree(d[0], ((d[2] - d[0]) / 12) * 12);
        d[0] = p;
        d[2] = p + n * 12;
    } else if ((unsigned)((d[1] - d[0]) / 12) < n) {
        Values_Copy(s[0], s[0] + ((d[1] - d[0]) / 12) * 12, d[0]);
        UninitCopyStrRefs(s[0] + ((d[1] - d[0]) / 12) * 12, s[1], d[1]);
    } else {
        for (BYTE* e = Values_Copy(s[0], s[1], d[0]); e != d[1]; e += 12)
            if (release)
                SchValue_Release((SchValue*)e);
            else
                MultValue_DeletingDestructor(e, 0, 0);
    }
    d[1] = d[0] + n * 12;
}

static void Elem44_Assign(BYTE* d, const BYTE* s, bool release)
{
    SchValue_Assign((SchValue*)(d + 4), 0, (const SchValue*)(s + 4));
    if (s + 16 != d + 16)
        Values_AssignInline((BYTE**)(d + 16), (BYTE* const*)(s + 16), release);
    F(DWORD, d, 28) = F(DWORD, s, 28);
    SchValue_Assign((SchValue*)(d + 32), 0, (const SchValue*)(s + 32));
}

extern "C" BYTE* __cdecl Elems44_Copy(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = (last - first) / 44; n > 0; n--, first += 44, dest += 44)
        Elem44_Assign(dest, first, true);
    return dest;
}

extern "C" BYTE* __cdecl Elems44_CopyB(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = (last - first) / 44; n > 0; n--, first += 44, dest += 44)
        Elem44_Assign(dest, first, false);
    return dest;
}

extern "C" BYTE* __cdecl Elems44_CopyBack(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = (last - first) / 44; n > 0; n--) {
        last -= 44;
        dest -= 44;
        Elem44_Assign(dest, last, false);
    }
    return dest;
}

// vector<value>::erase(first, last)
extern "C" BYTE* __fastcall Values_Erase(BYTE** self, void*, BYTE* first, BYTE* last)
{
    BYTE* d = first;
    BYTE* s = last;
    for (int n = (self[1] - last) / 12; n > 0; n--, d += 12, s += 12)
        SchValue_Assign((SchValue*)d, 0, (const SchValue*)s);
    for (BYTE* e = self[1]; d != e; d += 12) {
        SchValue* v = (SchValue*)d;
        if (!(v->flags & SCH_IMMEDIATE) && v->obj) {
            F(int, v->obj, 4)--;
            if (F(int, v->obj, 4) == 0 && v->obj)
                CallVirt(v->obj, 12, 1);
        }
    }
    self[1] -= ((last - first) / 12) * 12;
    return first;
}

// ---------------------------------------------------------------------------
// SchValue constructors: allocate a typed object, type-check, refcount

ASM_PROC(FUN_4b61c0)    // SchString: construct from char*
ASM_PROC(FUN_4b6400)    // SchIdentifier: construct from char*
extern "C" void FUN_4b6160(); KING_ALIAS("_FUN_4b6160", "@SchBuf4b6160_Construct@8")
extern "C" void FUN_4b6670(); KING_ALIAS("_FUN_4b6670", "@SchItem4b6670_Construct@12")
ASM_PROC(FUN_4a2d10)    // SchEnvironment: construct from SchValue*
ASM_PROC(FUN_4581a0)    // allocation failure handler
extern "C" void FUN_4b1120(); KING_ALIAS("_FUN_4b1120", "@Node4b1120_Construct@20")
extern "C" void FUN_4b1090(); KING_ALIAS("_FUN_4b1090", "@SchItem4b1090_Construct@12")
extern "C" void FUN_4b0b60(); KING_ALIAS("_FUN_4b0b60", "@SchNumber_FromInt@16")
extern "C" void FUN_4b0b90(); KING_ALIAS("_FUN_4b0b90", "@SchNumber_FromUnsigned@16")

static SchValue* SchValue_WrapNew(SchValue* out, BYTE* obj, int expectedType, const char* typeName)
{
    out->obj = 0;
    out->m_4 = 0;
    out->flags = 0;
    // These factories return a 12-byte value. The optional fourth word used
    // by some holders belongs to the caller and must not be overwritten.
    if (!obj) return out;
    int type = CallVirt<int>(obj, 0);
    if (type != expectedType) {
        const char* name = CallVirt<const char*>(obj, 4);
        Sch_Error(2, 1, typeName, name);
        return out;
    }
    out->obj = obj;
    out->flags = type & 0xFF;
    F(int, obj, 4)++;
    return out;
}
KING_ALIAS("_SchValue_MakeString", "FUN_4a1e30")
KING_ALIAS("_SchValue_MakeStringB", "FUN_4a2400")
KING_ALIAS("_SchValue_MakeIdent", "FUN_4a2230")
KING_ALIAS("_SchValue_MakePair", "FUN_4a2140")
KING_ALIAS("_SchValue_MakePairB", "FUN_4a28c0")
KING_ALIAS("_SchValue_MakeNumber", "FUN_4a2310")
KING_ALIAS("_SchValue_MakeNumberU", "FUN_4a24e0")
KING_ALIAS("_SchValue_MakeVector", "FUN_4a2700")
KING_ALIAS("_SchValue_MakeEnv", "FUN_4a27e0")

// ---------------------------------------------------------------------------
// value type-check copies: copy a SchValue and assert its immediate type

extern "C" void __cdecl Sch_TypeError(int code, int level, const char* name);
KING_ALIAS("_Sch_TypeError", "FUN_4a5420")
KING_ALIAS("@SchValue_AsBool@12", "FUN_4a1940")
KING_ALIAS("@SchValue_AsChar@12", "FUN_4a19c0")
KING_ALIAS("_Deque_AllocPages@8", "FUN_4a29e0")

// sch_istr.cpp's Environment and Port value factories. Both return exactly
// the three words of a Scheme value; the second argument is a compiler slot.
ASM_PROC(FUN_4a2ef0)
extern "C" BYTE* __fastcall SchItem4b5f30_Construct(BYTE*, void*, int, int);
ASM_VAR(AsmLabel, g_inputEnvironmentName, $L_67060c)
ASM_VAR(AsmLabel, g_inputPortName, $L_66cb28)

static SchValue* SchInput_WrapNew(SchValue* out, BYTE* object, int type, const char* expected)
{
    out->obj = 0;
    out->m_4 = 0;
    out->flags = 0;
    if (!object)
        return out;
    if (CallVirt<int>(object, 0) != type) {
        Sch_Error(2, 1, CallVirt<const char*>(object, 4), expected);
        return out;
    }
    out->obj = object;
    out->flags = CallVirt<unsigned>(object, 0) & 255;
    F(int, out->obj, 4)++;
    return out;
}

extern "C" SchValue* __cdecl SchInput_MakeEnvironment(SchValue* out, int, DWORD a, DWORD b)
{
    BYTE* storage = (BYTE*)operator new(160);
    BYTE* object = storage ? CallThis<BYTE*>(FUN_4a2ef0, storage, a, b) : 0;
    return SchInput_WrapNew(out, object, 12, g_inputEnvironmentName);
}

extern "C" SchValue* __cdecl SchInput_MakePort(SchValue* out, int, const int* a, const int* b)
{
    BYTE* storage = (BYTE*)operator new(296);
    BYTE* object = storage ? SchItem4b5f30_Construct(storage, 0, *a, *b) : 0;
    return SchInput_WrapNew(out, object, 11, g_inputPortName);
}

// --- "make a new object and return a typed handle to it" (exception frames dropped)

extern "C" DWORD* __cdecl SchHandle_Take(DWORD* out, BYTE* o, int type, const char* name)
{
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    if (!o) {
        out[0] = 0;
        out[2] = 0;
        return out;
    }
    if (CallVirt<int>(o, 0) != type) {
        const char* has = CallVirt<const char*>(o, 4);
        CallC(FUN_4a54e0, 2, 1, has, name);
        return out;
    }
    out[0] = (DWORD)o;
    out[2] = CallVirt<int>(o, 0) & 255;
    F(int, o, 4) = F(int, o, 4) + 1;
    return out;
}

static BYTE* SchNew0(unsigned size, void (*ctor)())
{
    BYTE* o = (BYTE*)operator new(size);
    return o ? CallThis<BYTE*>(ctor, o) : 0;
}

static BYTE* SchNew1(unsigned size, void (*ctor)(), DWORD a)
{
    BYTE* o = (BYTE*)operator new(size);
    return o ? CallThis<BYTE*>(ctor, o, a) : 0;
}

extern "C" {

// [FUN_4a50d0] a new identifier from *name
DWORD* __cdecl Sch_MakeIdentifierP(DWORD* out, int unused, const DWORD* name)
{
    return SchHandle_Take(out, SchNew1(24, FUN_4b6500, *name), 9, s_tIdentifier);
}

// [FUN_4aa830] a new identifier
DWORD* __cdecl Sch_MakeIdentifierA(DWORD* out, int unused, DWORD name)
{
    return SchHandle_Take(out, SchNew1(24, FUN_4b6500, name), 9, s_tIdentifier);
}

// [FUN_4aa910]
DWORD* __cdecl Sch_MakeIdentifierB(DWORD* out, int unused, DWORD name)
{
    return SchHandle_Take(out, SchNew1(24, FUN_4b6500, name), 9, s_tIdentifier);
}

// [FUN_4b07c0] a new string from *s
DWORD* __cdecl Sch_MakeStringP(DWORD* out, int unused, const DWORD* s)
{
    return SchHandle_Take(out, SchNew1(24, FUN_4b62c0, *s), 3, s_tString);
}

// [FUN_4aa730] the unspecified value
DWORD* __cdecl Sch_MakeUnspecified(DWORD* out)
{
    BYTE* o = (BYTE*)operator new(12);
    if (o) {
        F(int, o, 4) = 0;
        F(int, o, 8) = 0;
        F(void*, o, 0) = g_vtblUnspecified;
    }
    return SchHandle_Take(out, o, 255, s_tUnspecified);
}

// [FUN_4aaac0] a new closure
DWORD* __cdecl Sch_MakeClosure(DWORD* out, int unused, DWORD a)
{
    return SchHandle_Take(out, SchNew1(56, FUN_4910a0, a), 4, s_tClosure);
}

// [FUN_4affd0] a new closure from (a, *b, *d)
DWORD* __cdecl Sch_MakeClosureB(DWORD* out, int unused, DWORD a, const DWORD* b, const DWORD* d)
{
    BYTE* o = (BYTE*)operator new(28);
    BYTE* c = o ? CallThis<BYTE*>(FUN_491040, o, a, *b, *d) : 0;
    return SchHandle_Take(out, c, 4, s_tClosure);
}

// [FUN_4aaba0] a new continuation
DWORD* __cdecl Sch_MakeContinuation(DWORD* out, int unused, DWORD a)
{
    return SchHandle_Take(out, SchNew1(32, FUN_491380, a), 5, s_tContinuation);
}

// special-form nodes (type 34)
// [FUN_4aa9f0]
DWORD* __cdecl Sch_MakeBegin(DWORD* out)
{
    return SchHandle_Take(out, SchNew0(12, FUN_4a8580), 34, s_tBegin);
}

// [FUN_4b00c0]
DWORD* __cdecl Sch_MakeLambda(DWORD* out)
{
    return SchHandle_Take(out, SchNew0(12, FUN_4a81d0), 34, s_tLambda);
}

// [FUN_4b01a0]
DWORD* __cdecl Sch_MakeDefine(DWORD* out)
{
    return SchHandle_Take(out, SchNew0(12, FUN_4a9040), 34, s_tDefine);
}

// [FUN_4b0280]
DWORD* __cdecl Sch_MakeIf(DWORD* out)
{
    return SchHandle_Take(out, SchNew0(12, FUN_4a86f0), 34, s_tIf);
}

// [FUN_4b0360]
DWORD* __cdecl Sch_MakeQuote(DWORD* out)
{
    return SchHandle_Take(out, SchNew0(12, FUN_4a8a90), 34, s_tQuote);
}

// [FUN_4b0440]
DWORD* __cdecl Sch_MakeSet(DWORD* out)
{
    return SchHandle_Take(out, SchNew0(12, FUN_4a8bd0), 34, s_tSet);
}

// [FUN_4b0520]
DWORD* __cdecl Sch_MakeEval(DWORD* out)
{
    return SchHandle_Take(out, SchNew0(12, FUN_4a9770), 34, s_tEval);
}

// [FUN_4b0600]
DWORD* __cdecl Sch_MakeCallCC(DWORD* out)
{
    return SchHandle_Take(out, SchNew0(12, FUN_4a9a70), 34, s_tCallCC);
}

// [FUN_4b06e0]
DWORD* __cdecl Sch_MakeApply(DWORD* out)
{
    return SchHandle_Take(out, SchNew0(12, FUN_4a9e80), 34, s_tApply);
}


}

extern "C" {

// [FUN_4a47a0] an empty map with comparator *cmp
DWORD* __fastcall SchMap_Construct(DWORD* self, void*, const BYTE* cmp, int unused)
{
    self[0] = 0;
    BYTE* head = CallC<BYTE*>(FUN_44b740, 48);
    self[0] = (DWORD)head;
    self[1] = 0;
    F(BYTE, self, 8) = *cmp;
    F(BYTE, head, 0) = 0;
    F(BYTE*, head, 4) = 0;
    F(BYTE*, head, 8) = head;
    F(BYTE*, head, 12) = head;
    return self;
}

// [FUN_4a4870] map copy constructor (44-byte nodes)
DWORD* __fastcall SchMap_Copy(DWORD* self, void*, const DWORD* s)
{
    self[0] = 0;
    BYTE* head = CallC<BYTE*>(FUN_44b740, 44);
    self[0] = (DWORD)head;
    self[1] = 0;
    F(BYTE, self, 8) = F(BYTE, s, 8);
    BYTE* sroot = F(BYTE*, (BYTE*)s[0], 4);
    F(BYTE, head, 0) = 0;
    if (!sroot) {
        F(BYTE*, head, 4) = 0;
        F(BYTE*, head, 8) = head;
        F(BYTE*, head, 12) = head;
    } else {
        BYTE* h = (BYTE*)self[0];
        F(BYTE*, h, 4) = CallThis<BYTE*>(FUN_4a4950, self, sroot, h);
        h = (BYTE*)self[0];
        BYTE* n = F(BYTE*, h, 4);
        while (F(BYTE*, n, 8))
            n = F(BYTE*, n, 8);
        F(BYTE*, h, 8) = n;
        h = (BYTE*)self[0];
        n = F(BYTE*, h, 4);
        while (F(BYTE*, n, 12))
            n = F(BYTE*, n, 12);
        F(BYTE*, h, 12) = n;
    }
    self[1] = s[1];
    return self;
}

// [FUN_4a51d0] a 48-byte map node holding a copy of the pair x
BYTE* __stdcall SchMap_MakeNode(const BYTE* x)
{
    BYTE* node = CallC<BYTE*>(FUN_44b740, 48);
    BYTE* v = node + 16;
    if (v) {
        CallThis(FUN_454580, v, x);
        CallThis(FUN_454580, v + 12, x + 12);
        F(DWORD, v, 24) = F(DWORD, x, 24);
    }
    return node;
}

// [FUN_4afb30] an empty list (24-byte nodes)
BYTE** __fastcall SchList_Construct(BYTE** self, void*, int unused)
{
    BYTE* n = CallC<BYTE*>(FUN_44b740, 24);
    self[0] = n;
    F(BYTE*, n, 0) = n;
    F(BYTE*, self[0], 4) = self[0];
    return self;
}

// [FUN_4a84d0] out = the value h (passed by value)
DWORD* __cdecl SchValue_Return(DWORD* out, int unused1, int unused2, DWORD h0, DWORD h1, DWORD h2)
{
    BYTE* o = (BYTE*)h0;
    if (h2 & 0x10000) {
        out[2] = h2;
        out[0] = h0;
        out[1] = h1;
    } else {
        out[1] = 0;
        if (o) {
            out[0] = h0;
            out[2] = h2;
            F(int, o, 4) = F(int, o, 4) + 1;
        } else {
            out[0] = 0;
            out[2] = 0;
        }
    }
    if (!(h2 & 0x10000) && o) {
        if (--F(int, o, 4) == 0)
            CallVirt(o, 12, 1);
    }
    return out;
}

// [FUN_4aa320] a copy of v, which must be an immediate symbol-list value
DWORD* __fastcall SchValue_CopyImmediate(DWORD* self, void*, const DWORD* v)
{
    self[0] = v[0];
    self[1] = v[1];
    self[2] = v[2];
    if (!(v[2] & 0x10000))
        CallC(FUN_4a52e0, 6, 1);
    if (F(BYTE, v, 8) != 33)
        Sch_TypeError(2, 1, s_errNotSymbolList);
    return self;
}

// [FUN_4aa3a0] an identifier handle on o
DWORD* __fastcall SchIdentifier_From(DWORD* self, void*, BYTE* o)
{
    return SchHandle_Take(self, o, 9, s_tIdentifier);
}

}
