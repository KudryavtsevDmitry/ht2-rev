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

extern "C" void FUN_4a36f0(); KING_ALIAS("_FUN_4a36f0", "@HandlePair_Destroy@8")    // destructor of the value holders (map and list entries)
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // the AI's allocator: free (p, size)
extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")    // the AI's allocator: allocate (size)
extern "C" void FUN_4a52e0(); KING_ALIAS("_FUN_4a52e0", "_Sch_ErrorCode")    // script error (code, fatal)
extern "C" void FUN_4ab570(); KING_ALIAS("_FUN_4ab570", "_Sch4ab570_Construct")    // constructs a 44-byte element at p from x
ASM_PROC(FUN_4aa460)    // vector<44 bytes>::insert_aux
ASM_PROC(FUN_4ace70)    // copy (first, last, deque iterator, tag)
extern "C" void FUN_4afe80(); KING_ALIAS("_FUN_4afe80", "_ValueList_MakeNode@4")    // list: a new node holding x
extern "C" void FUN_467780(); KING_ALIAS("_FUN_467780", "_UninitCopyStrRefs")
ASM_PROC(FUN_4abd20)
ASM_PROC(FUN_4a4950)
extern "C" void FUN_454580(); KING_ALIAS("_FUN_454580", "@Str_CopyConstruct@12")
ASM_VAR(AsmLabel, s_errNotSymbolList, $L_67115c)
extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")
extern "C" void FUN_4b6500(); KING_ALIAS("_FUN_4b6500", "@SchSymbol_Construct@12")
extern "C" void FUN_4a8580(); KING_ALIAS("_FUN_4a8580", "@SchNode4a8580_Construct@8")
extern "C" void FUN_4910a0(); KING_ALIAS("_FUN_4910a0", "@RingB_Construct@12")
extern "C" void FUN_491380(); KING_ALIAS("_FUN_491380", "@Pooled_Construct@12")
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

extern "C" void FUN_4a5380(); KING_ALIAS("_FUN_4a5380", "_Sch_ErrorText")    // the format of an error (code, level)
extern "C" void FUN_4a59f0(); KING_ALIAS("_FUN_4a59f0", "_Sch_MessageText")    // the format of a message (code)
extern "C" void FUN_4b1450(); KING_ALIAS("_FUN_4b1450", "_Port_WriteString")    // shows a message (text, value by copy)
extern "C" void FUN_4a5800(); KING_ALIAS("_FUN_4a5800", "_Sch4a5800_Dump")
extern "C" void FUN_4a36c0(); KING_ALIAS("_FUN_4a36c0", "@SchHandle_Release@8")    // a value's destructor
extern "C" void FUN_4a4540(); KING_ALIAS("_FUN_4a4540", "_Vec12_AllocCopyS@12")    // vector<value>: allocate and copy (n, first, last)
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

extern "C" void FUN_4b61c0(); KING_ALIAS("_FUN_4b61c0", "@BytesNode4b61c0_Construct@12")    // SchString: construct from char*
extern "C" void FUN_4b6400(); KING_ALIAS("_FUN_4b6400", "@BytesNode4b6400_Construct@12")    // SchIdentifier: construct from char*
extern "C" void FUN_4b6160(); KING_ALIAS("_FUN_4b6160", "@SchBuf4b6160_Construct@8")
extern "C" void FUN_4b6670(); KING_ALIAS("_FUN_4b6670", "@SchItem4b6670_Construct@12")
extern "C" void FUN_4a2d10(); KING_ALIAS("_FUN_4a2d10", "@SchScope_Construct@12")    // SchEnvironment: construct from SchValue*
extern "C" void FUN_4581a0(); KING_ALIAS("_FUN_4581a0", "_Ai_OperatorNew")    // allocation failure handler
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

static void* SchAlloc(unsigned bytes)
{
    return bytes ? CallC<void*>(FUN_44b740, bytes) : 0;
}

extern "C" {

// [FUN_4a4540] n 12-byte records copied from [first, last)
BYTE* __stdcall Vec12_AllocCopyS(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)SchAlloc(n * 12);
    CallC(FUN_467780, first, last, p, n);
    return p;
}

// [FUN_4aaed0] n 44-byte records copied from [first, last)
BYTE* __stdcall Vec44_AllocCopy(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = (BYTE*)SchAlloc(n * 44);
    CallC(FUN_4abd20, first, last, p, n);
    return p;
}

// [FUN_4afe80] a 24-byte list node holding a copy of the value v
BYTE* __stdcall ValueList_MakeNode(const DWORD* v)
{
    BYTE* n = CallC<BYTE*>(FUN_44b740, 24);
    DWORD* d = (DWORD*)(n + 8);
    if (!d)
        return n;
    if (v[2] & 0x10000) {
        d[2] = v[2];
        d[0] = v[0];
        d[1] = v[1];
    } else {
        d[1] = 0;
        if (v[0]) {
            d[0] = v[0];
            d[2] = v[2];
            F(int, (BYTE*)v[0], 4) = F(int, (BYTE*)v[0], 4) + 1;
        } else {
            d[0] = 0;
            d[2] = 0;
        }
    }
    return n;
}

}

extern "C" void FUN_4a50d0(); KING_ALIAS("_FUN_4a50d0", "_Sch_MakeIdentifierP")
ASM_PROC(FUN_4a3180)
extern "C" void FUN_4abea0(); KING_ALIAS("_FUN_4abea0", "@Sch_Copy16@12")    // deque iterator copy
extern "C" void FUN_4acb40(); KING_ALIAS("_FUN_4acb40", "_Deque_Copy4acb40")
extern "C" void FUN_4acae0(); KING_ALIAS("_FUN_4acae0", "_Deque_CopyChars")

static void ReleaseRef(SchValue* v)
{
    if (!(v->flags & SCH_IMMEDIATE) && v->obj) {
        if (--F(int, v->obj, 4) == 0)
            CallVirt(v->obj, 12, 1);
    }
}

extern "C" {

// [FUN_4a3e40] h (by value) and a value made from b passed on to FUN_4a3180
int __fastcall Eval4a3e40(BYTE* self, void*, int a, int b, BYTE* h0, int h1, DWORD h2)
{
    SchValue h = {h0, h1, h2};
    DWORD c1[3];
    CopyRef((SchValue*)c1, &h);
    DWORD c2[3];
    CallC(FUN_4a50d0, c2, 0, &b);
    int r = CallThis<int>(FUN_4a3180, self, a, c2[0], c2[1], c2[2], c1[0], c1[1], c1[2]);
    ReleaseRef(&h);
    return r;
}

// [FUN_4acc20] deque algorithm step: t = FUN_4acb40(a, b, e), then
// out = FUN_4acae0(c, d, t)
DequeIt* __cdecl Deque4acc20(DequeIt* out, DequeIt a, DequeIt b, int c, int d, DequeIt e)
{
    DequeIt b2, a2, t;
    CallThis(FUN_4abea0, &b2, &b);
    CallThis(FUN_4abea0, &a2, &a);
    CallC(FUN_4acb40, &t, a2, b2, e);
    CallC(FUN_4acae0, out, c, d, t);
    return out;
}

// [FUN_4acd70] t = FUN_4acae0(c, d, e), then out = FUN_4acb40(a, b, t)
DequeIt* __cdecl Deque4acd70(DequeIt* out, int c, int d, DequeIt a, DequeIt b, DequeIt e)
{
    DequeIt t, b2, a2;
    CallC(FUN_4acae0, &t, c, d, e);
    CallThis(FUN_4abea0, &b2, &b);
    CallThis(FUN_4abea0, &a2, &a);
    CallC(FUN_4acb40, out, a2, b2, t);
    return out;
}

}

extern "C" void FUN_467780(); KING_ALIAS("_FUN_467780", "_UninitCopyStrRefs")    // uninitialized copy of 12-byte values (first, last, to)
ASM_PROC(FUN_4ac330)    // deque<char>: allocate the pages of [first, last)
extern "C" void FUN_4aa160(); KING_ALIAS("_FUN_4aa160", "@Vec44_At@12")
ASM_PROC(FUN_4a55c0)
extern "C" void FUN_4b1450(); KING_ALIAS("_FUN_4b1450", "_Port_WriteString")    // writes a string to a port (by value)
extern "C" void FUN_4affd0(); KING_ALIAS("_FUN_4affd0", "_Sch_MakeClosureB")
extern "C" void FUN_4aa430(); KING_ALIAS("_FUN_4aa430", "@SchValue_Object@8")
ASM_VAR(char, g_6710dc, $L_6710dc)

struct Value3 { BYTE* obj; int len; DWORD flags; };

// a vector of 12-byte values copied from [first, last) into self
static void Values_CopyConstruct(BYTE** self, BYTE* first, BYTE* last, int junk)
{
    int n = (last - first) / 12;
    self[0] = self[1] = self[2] = 0;
    BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n * 12) : 0;
    self[0] = p;
    self[1] = p;
    self[2] = p + n * 12;
    self[1] = CallC<BYTE*>(FUN_467780, first, last, p, junk);
}

extern "C" {

// [FUN_4aadc0] copy constructor of a vector of values
BYTE** __fastcall Values4aadc0_Copy(BYTE** self, void*, BYTE* const* src)
{
    Values_CopyConstruct(self, src[0], src[1], (src[1] - src[0]) / 12);
    return self;
}

// [FUN_4aac80] copy constructor: value at +4, values at +16, +28, value at +32
BYTE* __fastcall Sch4aac80_Copy(BYTE* self, void*, const BYTE* src)
{
    CopyRef((SchValue*)(self + 4), (const SchValue*)(src + 4));
    Values_CopyConstruct((BYTE**)(self + 16), F(BYTE*, src, 16), F(BYTE*, src, 20), (int)(self + 16));
    F(int, self, 28) = F(int, src, 28);
    CopyRef((SchValue*)(self + 32), (const SchValue*)(src + 32));
    F(void*, self, 0) = &g_vtbl64bf68;
    return self;
}

// [FUN_4abf10] deque<char> constructor for n elements (512 per page)
void __fastcall CharDeque_Construct(int* self, void*, unsigned n)
{
    unsigned nodes = (n >> 9) + 1;
    unsigned size = nodes + 2 > 8 ? nodes + 2 : 8;
    self[1] = size;
    char** map = size ? CallC<char**>(FUN_44b740, size * 4) : 0;
    self[0] = (int)map;
    char** start = map + ((self[1] - nodes) >> 1);
    char** finish = start + nodes;
    CallThis(FUN_4ac330, self, start, finish);
    self[5] = (int)start;
    self[3] = (int)*start;
    self[4] = (int)*start + 512;
    self[9] = (int)(finish - 1);
    self[7] = (int)finish[-1];
    self[8] = (int)finish[-1] + 512;
    self[2] = self[3];
    self[6] = self[7] + (n & 511);
}

// [FUN_4a5800] writes the marker to the port at +96, then hands each of the
// 44-byte entries at +108 (last first) with the port to FUN_4a55c0
void __cdecl Sch4a5800_Dump(BYTE* s)
{
    DWORD h[3];
    CopyRef((SchValue*)h, (const SchValue*)(s + 96));
    CallC(FUN_4b1450, &g_6710dc, h[0], h[1], h[2]);
    int n = (F(BYTE*, s, 112) - F(BYTE*, s, 108)) / 44;
    for (int i = n - 1; i != -1; i--) {
        CopyRef((SchValue*)h, (const SchValue*)(s + 96));
        BYTE* e = CallThis<BYTE*>(FUN_4aa160, s + 108, i);
        CallC(FUN_4a55c0, s, e, h[0], h[1], h[2]);
    }
}

// [FUN_4ad330] makes a closure of a, &c and &b, and passes it with a and
// b to the object of h
void __cdecl Sch4ad330(int a, Value3 h, int b, int c)
{
    DWORD l[3];
    const SchValue* r = CallC<const SchValue*>(FUN_4affd0, l, 0, a, &c, &b);
    DWORD v[3];
    CopyRef((SchValue*)v, r);
    BYTE* o = CallThis<BYTE*>(FUN_4aa430, &h);
    Eval4a3e40(o, 0, a, b, (BYTE*)v[0], v[1], v[2]);
    ReleaseRef((SchValue*)l);
    ReleaseRef((SchValue*)&h);
}

}

extern "C" {

// [FUN_4ab570] placement copy (the copy constructor FUN_4aac80)
void __cdecl Sch4ab570_Construct(BYTE* d, const BYTE* s)
{
    if (d)
        Sch4aac80_Copy(d, 0, s);
}

}

extern "C" void FUN_4a4540(); KING_ALIAS("_FUN_4a4540", "_Vec12_AllocCopyS@12")    // allocate n values and copy [first, last) (stdcall)
extern "C" void FUN_460910(); KING_ALIAS("_FUN_460910", "_ConstructStrRef")    // construct a value copy at p
extern "C" void FUN_44c710(); KING_ALIAS("_FUN_44c710", "@StrRefVec_InsertAux@16")    // vector of values: insert at the end (reallocating)
extern "C" void FUN_4a5910(); KING_ALIAS("_FUN_4a5910", "_Sch_Message0")    // script message (ctx, code)

extern "C" {

// [FUN_4a70b0] sets the value at +4 to h and clears the values at +16
void __fastcall Sch4a70b0_Reset(BYTE* self, void*, BYTE* h0, int h1, DWORD h2)
{
    SchValue* v = (SchValue*)(self + 4);
    if (v->obj && !(v->flags & SCH_IMMEDIATE)) {
        if (--F(int, v->obj, 4) == 0 && !(v->obj == h0 && v->flags == h2 && v->m_4 == h1) && v->obj)
            CallVirt(v->obj, 12, 1);
    }
    SchValue h = {h0, h1, h2};
    CopyRef(v, &h);
    BYTE* b = F(BYTE*, self, 16);
    BYTE* e = F(BYTE*, self, 20);
    for (BYTE* p = b; p != F(BYTE*, self, 20); p += 12)
        CallThis(FUN_4a36c0, p);
    F(BYTE*, self, 20) = F(BYTE*, self, 20) - ((e - b) / 12) * 12;
    F(int, self, 28) = 0;
    ReleaseRef(&h);
}

// [FUN_4a5c40] appends the value h to the values at o+16 (reserving 8 when
// empty); an empty h appended to an empty list sends message 31
void __cdecl Sch4a5c40_Append(BYTE* ctx, BYTE* o, const DWORD* h)
{
    BYTE** v = (BYTE**)(o + 16);
    if (v[0] != v[1]) {
        if (v[1] != v[2]) {
            if (v[1])
                CallThis(FUN_454580, v[1], h);
            v[1] += 12;
        } else
            CallThis(FUN_44c710, v, v[1], h);
        return;
    }
    if ((unsigned)((v[2] - v[0]) / 12) < 8) {
        BYTE* old = v[0];
        int n = (v[1] - old) / 12;
        BYTE* p = CallThis<BYTE*>(FUN_4a4540, v, 8, old, v[1]);
        for (BYTE* e = v[0]; e != v[1]; e += 12)
            CallThis(FUN_4a36c0, e);
        int cap = (v[2] - v[0]) / 12;
        if (cap)
            AiFree(v[0], cap * 12);
        v[0] = p;
        v[2] = p + 96;
        v[1] = p + n * 12;
    }
    if (v[1] != v[2]) {
        CallC(FUN_460910, v[1], h);
        v[1] += 12;
    } else
        CallThis(FUN_44c710, v, v[1], h);
    if (h[0] || h[2] || h[1])
        return;
    if ((unsigned)((F(BYTE*, ctx, 112) - F(BYTE*, ctx, 108)) / 44) > 1 && !F(int, ctx, 120))
        CallC(FUN_4a5910, ctx, 31);
}

}

extern "C" void FUN_4a51d0(); KING_ALIAS("_FUN_4a51d0", "_SchMap_MakeNode@4")    // map: new node holding a copy of the key (stdcall)
extern "C" void FUN_4a4d80(); KING_ALIAS("_FUN_4a4d80", "_NameMap_Rebalance")    // map: rebalance after insert (node, &root)
extern "C" void FUN_4a4d30(); KING_ALIAS("_FUN_4a4d30", "@SchMapIt_Dec@8")    // map iterator: decrement
extern "C" void FUN_4a4f30(); KING_ALIAS("_FUN_4a4f30", "_SchValue_LessByName@8")    // map: key less (stdcall a, b)
extern "C" void FUN_4a51b0(); KING_ALIAS("_FUN_4a51b0", "@Sch4a51b0_Construct@16")    // pair<iterator, bool> constructor

static const char* KeyName(const SchValue* v) { return F(const char*, SchValue_Object(v, 0), 12); }

// links the new node z for key as y's left (or right) child and rebalances
static BYTE* Map_Link(BYTE** self, BYTE* y, bool left, const SchValue* key)
{
    BYTE* header = self[0];
    BYTE* z = CallThis<BYTE*>(FUN_4a51d0, self, key);
    if (left) {
        F(BYTE*, y, 8) = z;
        if (y == header) {
            F(BYTE*, header, 4) = z;
            F(BYTE*, self[0], 12) = z;
        } else if (y == F(BYTE*, header, 8))
            F(BYTE*, header, 8) = z;
    } else {
        F(BYTE*, y, 12) = z;
        if (y == F(BYTE*, header, 12))
            F(BYTE*, header, 12) = z;
    }
    F(BYTE*, z, 4) = y;
    F(BYTE*, z, 8) = 0;
    F(BYTE*, z, 12) = 0;
    CallC(FUN_4a4d80, z, self[0] + 4);
    ((int*)self)[1]++;
    return z;
}

extern "C" {

// [FUN_4a4b00] map<name, ...>::insert_unique (names compared ignoring case)
BYTE* __fastcall SchMap_InsertUnique(BYTE** self, void*, BYTE* out, const SchValue* key)
{
    BYTE* y = self[0];
    BYTE* x = F(BYTE*, y, 4);
    bool comp = true;
    while (x) {
        y = x;
        const char* kx = KeyName((const SchValue*)(x + 16));
        comp = _stricmp(KeyName(key), kx) < 0;
        x = comp ? F(BYTE*, x, 8) : F(BYTE*, x, 12);
    }
    BYTE* j = y;
    if (comp) {
        if (y == F(BYTE*, self[0], 8)) {
            bool left = y == self[0] || x || _stricmp(KeyName(key), KeyName((const SchValue*)(y + 16))) < 0;
            F(BYTE*, out, 0) = Map_Link(self, y, left, key);
            F(BYTE, out, 4) = 1;
            return out;
        }
        CallThis(FUN_4a4d30, &j);
    }
    if (!key->obj)
        CallC(FUN_4a52e0, 0, 1);
    if (key->flags & SCH_IMMEDIATE)
        CallC(FUN_4a52e0, 4, 1);
    const char* kk = F(const char*, key->obj, 12);
    if (_stricmp(KeyName((const SchValue*)(j + 16)), kk) < 0) {
        bool left = y == self[0] || x || CallThis<BYTE>(FUN_4a4f30, self + 2, key, y + 16);
        F(BYTE*, out, 0) = Map_Link(self, y, left, key);
        F(BYTE, out, 4) = 1;
        return out;
    }
    bool no = false;
    CallThis(FUN_4a51b0, out, &j, &no);
    return out;
}

}

extern "C" void FUN_4a4ac0(); KING_ALIAS("_FUN_4a4ac0", "@SchMap_Erase@12")    // map: erase a subtree
extern "C" void FUN_4a52b0(); KING_ALIAS("_FUN_4a52b0", "@SchHolder_DeletingDestructor@12")    // map value: destructor (flag)
ASM_VAR(char, gv64bf4c, $L_64bf4c)
ASM_VAR(char, gv64bed0, $L_64bed0)
ASM_VAR(char, gv64bee4, $L_64bee4)

// empties a map (header +0, count +4) without freeing its header
static void Map_Clear(BYTE** m)
{
    if (!((int*)m)[1])
        return;
    CallThis(FUN_4a4ac0, m, F(BYTE*, m[0], 4));
    BYTE* h = m[0];
    F(BYTE*, h, 8) = h;
    F(BYTE*, h, 4) = 0;
    F(BYTE*, h, 12) = h;
    ((int*)m)[1] = 0;
}

extern "C" {

// [FUN_4a4300] destructor of the scope's name map: erases the nodes one by
// one (right subtrees recursively) and frees the header
void __fastcall SchMap_Destroy(BYTE** self, void*)
{
    if (((int*)self)[1]) {
        BYTE* x = F(BYTE*, self[0], 4);
        while (x) {
            CallThis(FUN_4a4ac0, self, F(BYTE*, x, 12));
            BYTE* left = F(BYTE*, x, 8);
            CallThis(FUN_4a52b0, x + 16, 0);
            CallC(FUN_44b4a0, x, 44);
            x = left;
        }
        BYTE* h = self[0];
        F(BYTE*, h, 8) = h;
        F(BYTE*, h, 4) = 0;
        F(BYTE*, h, 12) = h;
        ((int*)self)[1] = 0;
    }
    CallC(FUN_44b4a0, self[0], 44);
}

// [FUN_4a3fd0] destructor of a scope
void __fastcall SchScope_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &gv64bf4c;
    BYTE** m = F(BYTE**, self, 144);
    if (m) {
        Map_Clear(m);
        CallC(FUN_44b4a0, m[0], 44);
        operator delete(m);
    }
    F(BYTE**, self, 144) = 0;
    ReleaseRef((SchValue*)(self + 148));
    BYTE** v = (BYTE**)(self + 132);
    for (BYTE* e = v[0]; e != v[1]; e += 12)
        CallThis(FUN_4a36c0, e);
    int n = (v[2] - v[0]) / 12;
    if (n)
        CallC(FUN_44b4a0, v[0], n * 12);
    for (int i = 7; i >= 0; i--)
        CallThis(FUN_4a36c0, self + 36 + i * 12);
    ReleaseRef((SchValue*)(self + 24));
    F(void*, self, 0) = &gv64bed0;
    if (F(BYTE*, self, 12))
        F(BYTE*, F(BYTE*, self, 12), 16) = F(BYTE*, self, 16);
    if (F(BYTE*, self, 16))
        F(BYTE*, F(BYTE*, self, 16), 12) = F(BYTE*, self, 12);
    F(void*, self, 0) = &gv64bee4;
}

}

extern "C" void FUN_4090a0(); KING_ALIAS("_FUN_4090a0", "_Str_Member32")    // value: its +32 member (cdr)
extern "C" void FUN_407f00(); KING_ALIAS("_FUN_407f00", "_Str_Member20")    // value: its +20 member (car)

extern "C" {

// [FUN_4af480] out = the n-th element of the list h (by value); empty when
// the list is shorter
SchValue* __cdecl SchList_Nth(SchValue* out, int n, BYTE* h0, int h1, DWORD h2)
{
    SchValue h = {h0, h1, h2};
    for (int i = 0; i < n; i++) {
        if ((BYTE)h.flags != 2) {
            out->obj = 0;
            out->m_4 = 0;
            out->flags = 0;
            ReleaseRef(&h);
            return out;
        }
        SchValue t;
        SchValue* nx = CallC<SchValue*>(FUN_4090a0, &t, &h);
        if (h.obj && !(h.flags & SCH_IMMEDIATE)) {
            if (--F(int, h.obj, 4) == 0 && !(h.obj == nx->obj && h.flags == nx->flags && h.m_4 == nx->m_4) && h.obj)
                CallVirt(h.obj, 12, 1);
        }
        CopyRef(&h, nx);
        ReleaseRef(&t);
    }
    if ((BYTE)h.flags == 2)
        CallC(FUN_407f00, out, &h);
    else {
        out->obj = 0;
        out->m_4 = 0;
        out->flags = 0;
    }
    ReleaseRef(&h);
    return out;
}

}

extern "C" void FUN_4b13b0(); KING_ALIAS("_FUN_4b13b0", "@SchNode_Destroy@8")    // the script node base destructor
ASM_VAR(char, vt_64bf80, $L_64bf80)
ASM_VAR(char, vt_64bf94, $L_64bf94)
ASM_VAR(char, vt_64bfa8, $L_64bfa8)
ASM_VAR(char, vt_64bfbc, $L_64bfbc)
ASM_VAR(char, vt_64bfd0, $L_64bfd0)
ASM_VAR(char, vt_64bfe4, $L_64bfe4)
ASM_VAR(char, vt_64bff8, $L_64bff8)
ASM_VAR(char, vt_64c00c, $L_64c00c)
ASM_VAR(char, vt_64c020, $L_64c020)

// [FUN_4a8570] destructor of a script node class (vtable 64bf80)
extern "C" void __fastcall SchNode4a8570_Destroy(void** self, int)
{
    self[0] = &vt_64bf80;
    CallThis(FUN_4b13b0, self);
}

// [FUN_4a86e0] destructor of a script node class (vtable 64bf94)
extern "C" void __fastcall SchNode4a86e0_Destroy(void** self, int)
{
    self[0] = &vt_64bf94;
    CallThis(FUN_4b13b0, self);
}

// [FUN_4a8a80] destructor of a script node class (vtable 64bfa8)
extern "C" void __fastcall SchNode4a8a80_Destroy(void** self, int)
{
    self[0] = &vt_64bfa8;
    CallThis(FUN_4b13b0, self);
}

// [FUN_4a8bc0] destructor of a script node class (vtable 64bfbc)
extern "C" void __fastcall SchNode4a8bc0_Destroy(void** self, int)
{
    self[0] = &vt_64bfbc;
    CallThis(FUN_4b13b0, self);
}

// [FUN_4a9030] destructor of a script node class (vtable 64bfd0)
extern "C" void __fastcall SchNode4a9030_Destroy(void** self, int)
{
    self[0] = &vt_64bfd0;
    CallThis(FUN_4b13b0, self);
}

// [FUN_4a9760] destructor of a script node class (vtable 64bfe4)
extern "C" void __fastcall SchNode4a9760_Destroy(void** self, int)
{
    self[0] = &vt_64bfe4;
    CallThis(FUN_4b13b0, self);
}

// [FUN_4a9a60] destructor of a script node class (vtable 64bff8)
extern "C" void __fastcall SchNode4a9a60_Destroy(void** self, int)
{
    self[0] = &vt_64bff8;
    CallThis(FUN_4b13b0, self);
}

// [FUN_4a9e70] destructor of a script node class (vtable 64c00c)
extern "C" void __fastcall SchNode4a9e70_Destroy(void** self, int)
{
    self[0] = &vt_64c00c;
    CallThis(FUN_4b13b0, self);
}

// [FUN_4aa150] destructor of a script node class (vtable 64c020)
extern "C" void __fastcall SchNode4aa150_Destroy(void** self, int)
{
    self[0] = &vt_64c020;
    CallThis(FUN_4b13b0, self);
}

ASM_VAR(void*, g_pool692650, $L_692650)
ASM_VAR(int, g_poolCount693650, $L_693650)
ASM_VAR(void*, g_pool693658, $L_693658)
ASM_VAR(int, g_poolCount694658, $L_694658)

// [FUN_4aa720] the first dword the second argument points to
extern "C" int __cdecl Sch_Deref2nd(void*, int* p)
{
    return *p;
}

// [FUN_4a36c0] releases a value handle {obj, -, flags}: unless flag 0x10000, drops a reference, deleting at 0
extern "C" void __fastcall SchHandle_Release(void** h, int)
{
    if (((DWORD*)h)[2] & 0x10000)
        return;
    BYTE* obj = (BYTE*)h[0];
    if (!obj)
        return;
    --*(int*)(obj + 4);
    obj = (BYTE*)h[0];
    if (*(int*)(obj + 4) == 0 && obj)
        CallThis((*(void (***)())obj)[3], obj, 1);
}

// [FUN_4ad230] returns a block to the pool at 692650 (up to 1024), else deletes it
extern "C" void __cdecl SchPool1_Free(void* p)
{
    if (g_poolCount693650 >= 1024) {
        operator delete(p);
        return;
    }
    (&g_pool692650)[g_poolCount693650] = p;
    g_poolCount693650++;
}

// [FUN_4ad2b0] returns a block to the pool at 693658 (up to 1024), else deletes it
extern "C" void __cdecl SchPool2_Free(void* p)
{
    if (g_poolCount694658 >= 1024) {
        operator delete(p);
        return;
    }
    (&g_pool693658)[g_poolCount694658] = p;
    g_poolCount694658++;
}

ASM_VAR(char, vt_64bf68, $L_64bf68)

// [FUN_4a36f0] destructor of a handle pair: the handle at +12, then the one at +0
extern "C" void __fastcall HandlePair_Destroy(BYTE* self, int)
{
    SchHandle_Release((void**)(self + 12), 0);
    SchHandle_Release((void**)self, 0);
}

// [FUN_4a7230] destructor of a script node with a handle list (vtable 64bf68)
extern "C" void __fastcall SchListNode_Destroy(BYTE* self, int)
{
    *(void**)self = &vt_64bf68;
    SchHandle_Release((void**)(self + 32), 0);
    BYTE* b = F(BYTE*, self, 16);
    BYTE* e = F(BYTE*, self, 20);
    for (BYTE* p = b; p != e; p += 12)
        SchHandle_Release((void**)p, 0);
    b = F(BYTE*, self, 16);
    int n = (int)(F(BYTE*, self, 24) - b) / 12;
    if (n)
        CallC(FUN_44b4a0, b, n * 12);
    SchHandle_Release((void**)(self + 4), 0);
}

// [FUN_4abec0] frees a deque's 512-byte blocks (from the map slot at +20 to the one at +36) and its map
extern "C" void __fastcall Deque_FreeStorage(DWORD* self, void*)
{
    if (!self[0])
        return;
    for (void** p = (void**)self[5]; p < (void**)(self[9] + 4); p++)
        CallC(FUN_44b4a0, *p, 512);
    if (self[1])
        CallC(FUN_44b4a0, self[0], self[1] * 4);
}

extern "C" long __cdecl strtol(const char* s, char** end, int base);
extern "C" char* __cdecl strncat(char* d, const char* s, size_t n);
extern "C" int __cdecl sprintf(char* buf, const char* format, ...);
typedef const char* SchStrTable[1];
ASM_VAR(SchStrTable, g_msgs670e08, $L_670e08)
ASM_VAR(AsmLabel, s_670dc8, $L_670dc8)
ASM_VAR(AsmLabel, s_fmt6710ec, $L_6710ec)
ASM_VAR(AsmLabel, g_text692250, $L_692250)
ASM_VAR(AsmLabel, g_stderr686380e, $L_686380)

// [FUN_4a59f0] message i of the table: its number (checked against i) and the text after it
extern "C" char* __cdecl Sch_MessageText(int i)
{
    char* end;
    if (strtol(g_msgs670e08[i], &end, 0) != i)
        fprintf((FILE*)g_stderr686380e, s_670dc8);
    sprintf(g_text692250, s_fmt6710ec, i);
    strncat(g_text692250, end, 1024);
    return g_text692250;
}

ASM_VAR(SchStrTable, g_msgs670a7c, $L_670a7c)
ASM_VAR(AsmLabel, s_fmt670db8, $L_670db8)
ASM_VAR(AsmLabel, g_text691e44, $L_691e44)

// [FUN_4a5380] error message code with its severity letter (I W E F ?): the format and the text after the number
extern "C" char* __cdecl Sch_ErrorText(int code, int severity)
{
    char* end;
    if (strtol(g_msgs670a7c[code], &end, 0) != code)
        fprintf((FILE*)g_stderr686380e, s_670dc8);
    char c = severity >= 768 ? 'I' : severity >= 512 ? 'W' : severity >= 256 ? 'E' : severity >= 1 ? 'F' : '?';
    sprintf(g_text691e44, s_fmt670db8, (int)c, code);
    strncat(g_text691e44, end, 1024);
    return g_text691e44;
}

extern "C" size_t __cdecl strlen(const char* s);
typedef int SchIntTable[1];
ASM_VAR(SchIntTable, g_muted691dbc, $L_691dbc)
ASM_VAR(int, g_lastError692244, $L_692244)
ASM_VAR(int, g_lastSeverity692248, $L_692248)
typedef void (__cdecl* SchErrorHook)(int code, int severity, const char* text);
ASM_VAR(SchErrorHook, g_errorHook69224c, $L_69224c)

// [FUN_4a52e0] reports an error (code, severity) unless muted: to the error file and the hook; exits on severity 1
extern "C" int __cdecl Sch_ErrorCode(int code, int severity)
{
    int r = g_muted691dbc[code];
    if (r)
        return r;
    const char* text = Sch_ErrorText(code, severity);
    g_lastError692244 = code;
    g_lastSeverity692248 = severity;
    if (g_errorFile)
        fprintf(g_errorFile, text);
    if (g_errorHook69224c) {
        char* b = (char*)operator new(strlen(text) + 19);
        sprintf(b, text);
        g_errorHook69224c(code, severity, b);
        operator delete(b);
    }
    r = g_lastSeverity692248;
    if (r && r <= 1)
        exit(-1);
    return r;
}

// [FUN_4a5420] reports an error (code, severity) with a name, as Sch_ErrorCode
extern "C" void __cdecl Sch_TypeError(int code, int severity, const char* name)
{
    if (g_muted691dbc[code])
        return;
    const char* text = Sch_ErrorText(code, severity);
    g_lastError692244 = code;
    g_lastSeverity692248 = severity;
    if (g_errorFile)
        fprintf(g_errorFile, text, name);
    if (g_errorHook69224c) {
        char* b = (char*)operator new(strlen(text) + strlen(name) + 20);
        sprintf(b, text, name);
        g_errorHook69224c(code, severity, b);
        operator delete(b);
    }
    int r = g_lastSeverity692248;
    if (r && r <= 1)
        exit(-1);
}

// ---------------------------------------------------------------------------
// Virtual methods of the value classes: type ids and names, deleting
// destructors.

ASM_VAR(AsmLabel, s_tSpecialForm, $L_671110)
ASM_VAR(AsmLabel, s_port, $L_66cb28)
ASM_VAR(AsmLabel, s_tNumber, $L_66ca60)


// [$L_4a7550]
extern "C" int __fastcall SpecialForm_TypeId(BYTE*) { return 34; }
// [$L_4a7560]
extern "C" const char* __fastcall SpecialForm_TypeName(BYTE*) { return s_tSpecialForm; }
// [$L_4a81f0]
extern "C" const char* __fastcall Lambda_TypeName(BYTE*) { return s_tLambda; }
// [$L_4a85a0]
extern "C" const char* __fastcall Begin_TypeName(BYTE*) { return s_tBegin; }
// [$L_4a8710]
extern "C" const char* __fastcall If_TypeName(BYTE*) { return s_tIf; }
// [$L_4a8ab0]
extern "C" const char* __fastcall Quote_TypeName(BYTE*) { return s_tQuote; }
// [$L_4a8bf0]
extern "C" const char* __fastcall Set_TypeName(BYTE*) { return s_tSet; }
// [$L_4a9060]
extern "C" const char* __fastcall Define_TypeName(BYTE*) { return s_tDefine; }
// [$L_4a9790]
extern "C" const char* __fastcall Eval_TypeName(BYTE*) { return s_tEval; }
// [$L_4a9a90]
extern "C" const char* __fastcall CallCC_TypeName(BYTE*) { return s_tCallCC; }
// [$L_4a9ea0]
extern "C" const char* __fastcall Apply_TypeName(BYTE*) { return s_tApply; }
// [$L_4aa810]
extern "C" int __fastcall Unspecified_TypeId(BYTE*) { return 255; }
// [$L_4aa820]
extern "C" const char* __fastcall Unspecified_TypeName(BYTE*) { return s_tUnspecified; }
// [$L_4abe60]
extern "C" int __fastcall Port_TypeId(BYTE*) { return 11; }
// [$L_4abe70]
extern "C" const char* __fastcall Port_TypeName(BYTE*) { return s_port; }
// [$L_4b0b20]
extern "C" int __fastcall Number_TypeId(BYTE*) { return 6; }
// [$L_4b0b30]
extern "C" const char* __fastcall Number_TypeName(BYTE*) { return s_tNumber; }

static BYTE* Sch_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [$L_4a7090]
extern "C" BYTE* __fastcall Sch4a7090_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchListNode_Destroy); }
// [$L_4a7570]
extern "C" BYTE* __fastcall SpecialForm_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, FUN_4b13b0); }
// [$L_4a8200]
extern "C" BYTE* __fastcall Lambda_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchNode4a8570_Destroy); }
// [$L_4a85b0]
extern "C" BYTE* __fastcall Begin_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchNode4a86e0_Destroy); }
// [$L_4a8720]
extern "C" BYTE* __fastcall If_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchNode4a8a80_Destroy); }
// [$L_4a8ac0]
extern "C" BYTE* __fastcall Quote_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchNode4a8bc0_Destroy); }
// [$L_4a8c00]
extern "C" BYTE* __fastcall Set_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchNode4a9030_Destroy); }
// [$L_4a9070]
extern "C" BYTE* __fastcall Define_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchNode4a9760_Destroy); }
// [$L_4a97a0]
extern "C" BYTE* __fastcall Eval_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchNode4a9a60_Destroy); }
// [$L_4a9aa0]
extern "C" BYTE* __fastcall CallCC_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchNode4a9e70_Destroy); }
// [$L_4a9eb0]
extern "C" BYTE* __fastcall Apply_DeletingDestructor(BYTE* s, void*, int flags) { return Sch_DeleteAfter(s, flags, (void (*)())SchNode4aa150_Destroy); }

// [$L_4b0b40] numbers come from the value pool and go back to it
extern "C" BYTE* __fastcall Number_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_4b13b0, s);
    if (flags & 1)
        SchPool2_Free(s);
    return s;
}

extern "C" void FUN_4a5a60(); KING_ALIAS("_FUN_4a5a60", "_Sch_Message1")
ASM_VAR(AsmLabel, s_eva671104, $L_671104)

// [$L_4a74f0] message 29 about the scope's frame (+112, 28 bytes back): its name or 671104
extern "C" int __fastcall Eva_4a74f0(BYTE*, void*, BYTE* p)
{
    BYTE* e = *(BYTE**)(F(BYTE*, p, 112) - 28);
    const char* name;
    if (F(int, e, 8) & 0x10000)
        name = s_eva671104;
    else
        name = CallVirt<const char*>(F(BYTE*, e, 0), 4);
    return CallC<int>(FUN_4a5a60, p, 29, name);
}

// ---------------------------------------------------------------------------
// Static initializers of random generators.

ASM_VAR(AsmLabel, g_evaRng694660, $L_694660)
ASM_VAR(DWORD, g_evaRng694738, $L_694738)
ASM_VAR(DWORD, g_evaRng69473c, $L_69473c)
ASM_VAR(DWORD, g_evaRng694740, $L_694740)

// the subtractive random generator's start (Knuth): 55 words at a, then the seed and the two indices
static void Eva_RngInit(DWORD* a, DWORD* seed, DWORD* i1, DWORD* i2)
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

// [$L_4ad180]
extern "C" void __cdecl EvaInit_4ad180() { Eva_RngInit((DWORD*)g_evaRng694660, &g_evaRng694738, &g_evaRng69473c, &g_evaRng694740); }

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
// [$L_4a4940] returns the 44-byte record s[0] to the pool
extern "C" void __fastcall Eval4a4940_Free(BYTE* s) { CallC(FUN_44b4a0, F(void*, s, 0), 44); }

extern "C" void FUN_4ad030(); KING_ALIAS("_FUN_4ad030", "_Deque_CopyBackwardLoop")
// [FUN_4acd20] copies [a, b) backwards to the end c (copies of the three iterators), the result into out
extern "C" DequeIt* __cdecl Deque_CopyBackward(DequeIt* out, DequeIt a, DequeIt b, DequeIt c)
{
    DequeIt c2, b2, a2;
    CallThis(FUN_4abea0, &c2, &c);
    CallThis(FUN_4abea0, &b2, &b);
    CallThis(FUN_4abea0, &a2, &a);
    CallC(FUN_4ad030, out, a2, b2, c2, out, 0);
    return out;
}

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
// [$L_4af6b0] destroys a list of 16-byte nodes: every node and then the head go back to the allocator
extern "C" void __fastcall List16_Destroy(BYTE** s)
{
    for (BYTE* n = *(BYTE**)*s; n != *s;) {
        BYTE* next = *(BYTE**)n;
        CallC(FUN_44b4a0, n, 16);
        n = next;
    }
    BYTE* h = *s;
    F(BYTE*, h, 0) = h;
    F(BYTE*, *s, 4) = *s;
    CallC(FUN_44b4a0, *s, 16);
}

// [$L_4afc30] destroys a list of 24-byte nodes holding script values (+8 object, +16 flags; an object reference is
// released unless the value is immediate), then the head
extern "C" void __fastcall ValueList_Destroy(BYTE** s)
{
    for (BYTE* n = *(BYTE**)*s; n != *s;) {
        BYTE* next = *(BYTE**)n;
        BYTE* o = F(BYTE*, n, 8);
        if (!(F(int, n, 16) & 0x10000) && o) {
            F(int, o, 4)--;
            o = F(BYTE*, n, 8);
            if (!F(int, o, 4) && o)
                CallVirt(o, 12, 1);
        }
        CallC(FUN_44b4a0, n, 24);
        n = next;
    }
    BYTE* h = *s;
    F(BYTE*, h, 0) = h;
    F(BYTE*, *s, 4) = *s;
    CallC(FUN_44b4a0, *s, 24);
}

// ---------------------------------------------------------------------------
// The interpreter constructor: the special forms and the 107 natives of the global environment, the
// standard ports, and the start-up script.

extern "C" void FUN_4aaed0(); KING_ALIAS("_FUN_4aaed0", "_Vec44_AllocCopy@12")
extern "C" void FUN_4a27e0(); KING_ALIAS("_FUN_4a27e0", "_SchValue_MakeEnv")
extern "C" void FUN_40c100(); KING_ALIAS("_FUN_40c100", "@Str_Assign@12")
extern "C" void FUN_4aa430(); KING_ALIAS("_FUN_4aa430", "@SchValue_Object@8")
extern "C" void FUN_4a3e40(); KING_ALIAS("_FUN_4a3e40", "@Eval4a3e40@28")         // environment: define (p, name, value)
extern "C" void FUN_4b00c0(); KING_ALIAS("_FUN_4b00c0", "_Sch_MakeLambda")
extern "C" void FUN_4aa9f0(); KING_ALIAS("_FUN_4aa9f0", "_Sch_MakeBegin")
extern "C" void FUN_4b01a0(); KING_ALIAS("_FUN_4b01a0", "_Sch_MakeDefine")
extern "C" void FUN_4b0280(); KING_ALIAS("_FUN_4b0280", "_Sch_MakeIf")
extern "C" void FUN_4b0360(); KING_ALIAS("_FUN_4b0360", "_Sch_MakeQuote")
extern "C" void FUN_4b0440(); KING_ALIAS("_FUN_4b0440", "_Sch_MakeSet")
extern "C" void FUN_4b0520(); KING_ALIAS("_FUN_4b0520", "_Sch_MakeEval")
extern "C" void FUN_4b0600(); KING_ALIAS("_FUN_4b0600", "_Sch_MakeCallCC")
extern "C" void FUN_4b06e0(); KING_ALIAS("_FUN_4b06e0", "_Sch_MakeApply")
extern "C" void FUN_4ad330(); KING_ALIAS("_FUN_4ad330", "_Sch4ad330")             // defines a native (p, env, name, function)
extern "C" void FUN_4b07c0(); KING_ALIAS("_FUN_4b07c0", "_Sch_MakeStringP")
extern "C" void FUN_4b08a0(); KING_ALIAS("_FUN_4b08a0", "_SchInput_MakeEnvironment")
extern "C" void FUN_4b0990(); KING_ALIAS("_FUN_4b0990", "_SchInput_MakePort")
extern "C" void FUN_455f00(); KING_ALIAS("_FUN_455f00", "_Port_MakeB")
extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")
ASM_PROC(FUN_4b4c20)    // read (out, p, port by value)
ASM_PROC(FUN_4a6af0)    // eval (out, p, value by value)
ASM_VAR(AsmLabel, g_sv64bed0, $L_64bed0)
ASM_VAR(AsmLabel, s_sv6712f8, $L_6712f8)
ASM_VAR(AsmLabel, s_sv6712f0, $L_6712f0)
ASM_VAR(AsmLabel, s_sv6712e8, $L_6712e8)
ASM_VAR(AsmLabel, s_sv6712e4, $L_6712e4)
ASM_VAR(AsmLabel, s_sv64bf60, $L_64bf60)
ASM_VAR(AsmLabel, s_sv6712dc, $L_6712dc)
ASM_VAR(AsmLabel, s_sv6712d4, $L_6712d4)
ASM_VAR(AsmLabel, s_sv6712b4, $L_6712b4)
ASM_VAR(AsmLabel, s_sv6712ac, $L_6712ac)
ASM_VAR(AsmLabel, s_sv671204, $L_671204)
ASM_VAR(AsmLabel, s_sv6711f4, $L_6711f4)
ASM_VAR(AsmLabel, s_sv686340, $L_686340)
ASM_VAR(AsmLabel, s_sv686360, $L_686360)
ASM_VAR(AsmLabel, s_sv686380, $L_686380)
ASM_VAR(AsmLabel, s_sv6711e8, $L_6711e8)
ASM_VAR(AsmLabel, s_svPort, $L_66cb28)          // "Port"
extern "C" void Nat_MacroExpand();
ASM_VAR(AsmLabel, s_rn6709d4, $L_6709d4)
ASM_VAR(AsmLabel, g_rf491ce0, $L_491ce0)
ASM_VAR(AsmLabel, s_rn670640, $L_670640)
ASM_VAR(AsmLabel, s_rn670658, $L_670658)
ASM_VAR(AsmLabel, g_rf493120, $L_493120)
ASM_VAR(AsmLabel, s_rn67065c, $L_67065c)
extern "C" void Nat_NumberP();
ASM_VAR(AsmLabel, s_rn670664, $L_670664)
extern "C" void Nat_ComplexP();
ASM_VAR(AsmLabel, s_rn67066c, $L_67066c)
extern "C" void Nat_RealP();
ASM_VAR(AsmLabel, s_rn670678, $L_670678)
extern "C" void Nat_RationalP();
ASM_VAR(AsmLabel, s_rn670680, $L_670680)
extern "C" void Nat_IntegerP();
ASM_VAR(AsmLabel, s_rn67068c, $L_67068c)
extern "C" void Nat_ExactP();
ASM_VAR(AsmLabel, s_rn670698, $L_670698)
extern "C" void Nat_InexactP();
ASM_VAR(AsmLabel, s_rn6706a0, $L_6706a0)
extern "C" void Nat_NumEq();
ASM_VAR(AsmLabel, s_rn6712a8, $L_6712a8)
extern "C" void Nat_NumLt();
ASM_VAR(AsmLabel, s_rn6712a4, $L_6712a4)
extern "C" void Nat_NumGt();
ASM_VAR(AsmLabel, s_rn6712a0, $L_6712a0)
extern "C" void Nat_NumLe();
ASM_VAR(AsmLabel, s_rn67129c, $L_67129c)
extern "C" void Nat_NumGe();
ASM_VAR(AsmLabel, s_rn671298, $L_671298)
extern "C" void Nat_Plus();
ASM_VAR(AsmLabel, s_rn6706ac, $L_6706ac)
ASM_VAR(AsmLabel, g_rf4954f0, $L_4954f0)
ASM_VAR(AsmLabel, s_rn6706b0, $L_6706b0)
extern "C" void Nat_Times();
ASM_VAR(AsmLabel, s_rn6706b4, $L_6706b4)
extern "C" void Nat_Divide();
ASM_VAR(AsmLabel, s_rn670570, $L_670570)
extern "C" void Nat_Quotient();
ASM_VAR(AsmLabel, s_rn6706b8, $L_6706b8)
extern "C" void Nat_Remainder();
ASM_VAR(AsmLabel, s_rn6706c0, $L_6706c0)
extern "C" void Nat_Modulo();
ASM_VAR(AsmLabel, s_rn6706cc, $L_6706cc)
extern "C" void Nat_Numerator();
ASM_VAR(AsmLabel, s_rn6706d4, $L_6706d4)
extern "C" void Nat_Denominator();
ASM_VAR(AsmLabel, s_rn6706e0, $L_6706e0)
extern "C" void Nat_Floor();
ASM_VAR(AsmLabel, s_rn6706ec, $L_6706ec)
extern "C" void Nat_Ceiling();
ASM_VAR(AsmLabel, s_rn6706f4, $L_6706f4)
extern "C" void Nat_Truncate();
ASM_VAR(AsmLabel, s_rn6706fc, $L_6706fc)
extern "C" void Nat_Round();
ASM_VAR(AsmLabel, s_rn670708, $L_670708)
extern "C" void Nat_Exp();
ASM_VAR(AsmLabel, s_rn670710, $L_670710)
extern "C" void Nat_Log();
ASM_VAR(AsmLabel, s_rn670714, $L_670714)
extern "C" void Nat_Sin();
ASM_VAR(AsmLabel, s_rn670718, $L_670718)
extern "C" void Nat_Cos();
ASM_VAR(AsmLabel, s_rn67071c, $L_67071c)
extern "C" void Nat_Tan();
ASM_VAR(AsmLabel, s_rn670720, $L_670720)
extern "C" void Nat_Asin();
ASM_VAR(AsmLabel, s_rn670724, $L_670724)
extern "C" void Nat_Acos();
ASM_VAR(AsmLabel, s_rn67072c, $L_67072c)
extern "C" void Nat_Atan();
ASM_VAR(AsmLabel, s_rn670734, $L_670734)
extern "C" void Nat_Atan2();
ASM_VAR(AsmLabel, s_rn67073c, $L_67073c)
extern "C" void Nat_Sqrt();
ASM_VAR(AsmLabel, s_rn670744, $L_670744)
extern "C" void Nat_Expt();
ASM_VAR(AsmLabel, s_rn67074c, $L_67074c)
extern "C" void Nat_MakeRect();
ASM_VAR(AsmLabel, s_rn670754, $L_670754)
extern "C" void Nat_MakePolar();
ASM_VAR(AsmLabel, s_rn670768, $L_670768)
extern "C" void Nat_RealPart();
ASM_VAR(AsmLabel, s_rn670774, $L_670774)
extern "C" void Nat_ImagPart();
ASM_VAR(AsmLabel, s_rn670780, $L_670780)
extern "C" void Nat_Magnitude();
ASM_VAR(AsmLabel, s_rn67078c, $L_67078c)
extern "C" void Nat_Angle();
ASM_VAR(AsmLabel, s_rn670798, $L_670798)
extern "C" void Nat_ToInexact();
ASM_VAR(AsmLabel, s_rn6707a0, $L_6707a0)
extern "C" void Nat_ToExact();
ASM_VAR(AsmLabel, s_rn6707b0, $L_6707b0)
extern "C" void Nat_NumberToString();
ASM_VAR(AsmLabel, s_rn6707c0, $L_6707c0)
extern "C" void Nat_StringToNumber();
ASM_VAR(AsmLabel, s_rn6707d0, $L_6707d0)
extern "C" void Nat_PairP();
ASM_VAR(AsmLabel, s_rn6707e0, $L_6707e0)
extern "C" void Nat_Cons();
ASM_VAR(AsmLabel, s_rn6707e8, $L_6707e8)
extern "C" void Nat_Car();
ASM_VAR(AsmLabel, s_rn66de28, $L_66de28)
extern "C" void Nat_Cdr();
ASM_VAR(AsmLabel, s_rn6707f0, $L_6707f0)
extern "C" void Nat_SetCar();
ASM_VAR(AsmLabel, s_rn6707f4, $L_6707f4)
extern "C" void Nat_SetCdr();
ASM_VAR(AsmLabel, s_rn670800, $L_670800)
extern "C" void Nat_SymbolP();
ASM_VAR(AsmLabel, s_rn67080c, $L_67080c)
extern "C" void Nat_SymbolToString();
ASM_VAR(AsmLabel, s_rn670814, $L_670814)
extern "C" void Nat_StringToSymbol();
ASM_VAR(AsmLabel, s_rn670824, $L_670824)
extern "C" void Nat_CharP();
ASM_VAR(AsmLabel, s_rn670834, $L_670834)
extern "C" void Nat_CharEq();
ASM_VAR(AsmLabel, s_rn67083c, $L_67083c)
extern "C" void Nat_CharLt();
ASM_VAR(AsmLabel, s_rn670844, $L_670844)
extern "C" void Nat_CharGt();
ASM_VAR(AsmLabel, s_rn67084c, $L_67084c)
extern "C" void Nat_CharLe();
ASM_VAR(AsmLabel, s_rn670854, $L_670854)
extern "C" void Nat_CharGe();
ASM_VAR(AsmLabel, s_rn67085c, $L_67085c)
extern "C" void Nat_CharToInteger();
ASM_VAR(AsmLabel, s_rn670864, $L_670864)
extern "C" void Nat_IntegerToChar();
ASM_VAR(AsmLabel, s_rn670874, $L_670874)
ASM_VAR(AsmLabel, g_rf49c230, $L_49c230)
ASM_VAR(AsmLabel, s_rn670884, $L_670884)
ASM_VAR(AsmLabel, g_rf49c610, $L_49c610)
ASM_VAR(AsmLabel, s_rn670890, $L_670890)
extern "C" void Nat_StringLength();
ASM_VAR(AsmLabel, s_rn670898, $L_670898)
extern "C" void Nat_StringRef();
ASM_VAR(AsmLabel, s_rn6708a8, $L_6708a8)
extern "C" void Nat_StringSet();
ASM_VAR(AsmLabel, s_rn6708b4, $L_6708b4)
extern "C" void Nat_VectorP();
ASM_VAR(AsmLabel, s_rn6708c0, $L_6708c0)
extern "C" void Nat_MakeVector();
ASM_VAR(AsmLabel, s_rn6708c8, $L_6708c8)
extern "C" void Nat_ListToVector();
ASM_VAR(AsmLabel, s_rn6708d4, $L_6708d4)
extern "C" void Nat_VectorLength();
ASM_VAR(AsmLabel, s_rn6708ec, $L_6708ec)
extern "C" void Nat_VectorRef();
ASM_VAR(AsmLabel, s_rn6708fc, $L_6708fc)
extern "C" void Nat_VectorSet();
ASM_VAR(AsmLabel, s_rn67128c, $L_67128c)
extern "C" void Nat_ProcedureP();
ASM_VAR(AsmLabel, s_rn670914, $L_670914)
extern "C" void Nat_Field48();
ASM_VAR(AsmLabel, s_rn671270, $L_671270)
extern "C" void Nat_MakeEnv();
ASM_VAR(AsmLabel, s_rn67125c, $L_67125c)
extern "C" void Nat_Field60();
ASM_VAR(AsmLabel, s_rn671244, $L_671244)
extern "C" void Nat_InputPortP();
ASM_VAR(AsmLabel, s_rn670920, $L_670920)
extern "C" void Nat_OutputPortP();
ASM_VAR(AsmLabel, s_rn67092c, $L_67092c)
extern "C" void Nat_Field72();
ASM_VAR(AsmLabel, s_rn671230, $L_671230)
extern "C" void Nat_Field84();
ASM_VAR(AsmLabel, s_rn67121c, $L_67121c)
extern "C" void Nat_OpenInputFile();
ASM_VAR(AsmLabel, s_rn67093c, $L_67093c)
extern "C" void Nat_OpenOutputFile();
ASM_VAR(AsmLabel, s_rn67094c, $L_67094c)
extern "C" void Nat_CloseInputPort();
ASM_VAR(AsmLabel, s_rn670960, $L_670960)
extern "C" void Nat_CloseOutputPort();
ASM_VAR(AsmLabel, s_rn670974, $L_670974)
extern "C" void Nat_ReadChar();
ASM_VAR(AsmLabel, s_rn670988, $L_670988)
extern "C" void Nat_PeekChar();
ASM_VAR(AsmLabel, s_rn670994, $L_670994)
extern "C" void Nat_EofObjectP();
ASM_VAR(AsmLabel, s_rn6709a0, $L_6709a0)
extern "C" void Nat_CharReadyP();
ASM_VAR(AsmLabel, s_rn6709ac, $L_6709ac)
extern "C" void Nat_WriteChar();
ASM_VAR(AsmLabel, s_rn6709b8, $L_6709b8)
extern "C" void Nat_Read();
ASM_VAR(AsmLabel, s_rn6709c4, $L_6709c4)
extern "C" void Nat_Write();
ASM_VAR(AsmLabel, s_rn6709cc, $L_6709cc)
extern "C" void Nat_SetValues();
ASM_VAR(AsmLabel, s_rn671210, $L_671210)
extern "C" void Nat_StringAppend();
ASM_VAR(AsmLabel, s_rn6709ec, $L_6709ec)
ASM_VAR(AsmLabel, g_rf4a0af0, $L_4a0af0)
ASM_VAR(AsmLabel, s_rn6709fc, $L_6709fc)
ASM_VAR(AsmLabel, s_rn670a04, $L_670a04)
ASM_VAR(AsmLabel, s_rn670a08, $L_670a08)
ASM_VAR(AsmLabel, s_rn671208, $L_671208)
ASM_VAR(AsmLabel, s_rn670a10, $L_670a10)
ASM_VAR(AsmLabel, s_rn670a18, $L_670a18)
ASM_VAR(AsmLabel, s_rn670a20, $L_670a20)
ASM_VAR(AsmLabel, s_rn670a28, $L_670a28)
extern "C" void Nat_Not();
extern "C" void Nat_NullP();
extern "C" void Nat_List();
extern "C" void Nat_Caar();
extern "C" void Nat_Cadr();
extern "C" void Nat_Cdar();
extern "C" void Nat_Cddr();
extern "C" void Nat_EqP();
struct SchNativeReg { void* fn; const char* name; };
static const SchNativeReg k_schNatives[] = {
    {(void*)Nat_MacroExpand, s_rn6709d4},
    {(void*)g_rf491ce0, s_rn670640},
    {(void*)Nat_EqP, s_rn670658},
    {(void*)g_rf493120, s_rn67065c},
    {(void*)Nat_NumberP, s_rn670664},
    {(void*)Nat_ComplexP, s_rn67066c},
    {(void*)Nat_RealP, s_rn670678},
    {(void*)Nat_RationalP, s_rn670680},
    {(void*)Nat_IntegerP, s_rn67068c},
    {(void*)Nat_ExactP, s_rn670698},
    {(void*)Nat_InexactP, s_rn6706a0},
    {(void*)Nat_NumEq, s_rn6712a8},
    {(void*)Nat_NumLt, s_rn6712a4},
    {(void*)Nat_NumGt, s_rn6712a0},
    {(void*)Nat_NumLe, s_rn67129c},
    {(void*)Nat_NumGe, s_rn671298},
    {(void*)Nat_Plus, s_rn6706ac},
    {(void*)g_rf4954f0, s_rn6706b0},
    {(void*)Nat_Times, s_rn6706b4},
    {(void*)Nat_Divide, s_rn670570},
    {(void*)Nat_Quotient, s_rn6706b8},
    {(void*)Nat_Remainder, s_rn6706c0},
    {(void*)Nat_Modulo, s_rn6706cc},
    {(void*)Nat_Numerator, s_rn6706d4},
    {(void*)Nat_Denominator, s_rn6706e0},
    {(void*)Nat_Floor, s_rn6706ec},
    {(void*)Nat_Ceiling, s_rn6706f4},
    {(void*)Nat_Truncate, s_rn6706fc},
    {(void*)Nat_Round, s_rn670708},
    {(void*)Nat_Exp, s_rn670710},
    {(void*)Nat_Log, s_rn670714},
    {(void*)Nat_Sin, s_rn670718},
    {(void*)Nat_Cos, s_rn67071c},
    {(void*)Nat_Tan, s_rn670720},
    {(void*)Nat_Asin, s_rn670724},
    {(void*)Nat_Acos, s_rn67072c},
    {(void*)Nat_Atan, s_rn670734},
    {(void*)Nat_Atan2, s_rn67073c},
    {(void*)Nat_Sqrt, s_rn670744},
    {(void*)Nat_Expt, s_rn67074c},
    {(void*)Nat_MakeRect, s_rn670754},
    {(void*)Nat_MakePolar, s_rn670768},
    {(void*)Nat_RealPart, s_rn670774},
    {(void*)Nat_ImagPart, s_rn670780},
    {(void*)Nat_Magnitude, s_rn67078c},
    {(void*)Nat_Angle, s_rn670798},
    {(void*)Nat_ToInexact, s_rn6707a0},
    {(void*)Nat_ToExact, s_rn6707b0},
    {(void*)Nat_NumberToString, s_rn6707c0},
    {(void*)Nat_StringToNumber, s_rn6707d0},
    {(void*)Nat_PairP, s_rn6707e0},
    {(void*)Nat_Cons, s_rn6707e8},
    {(void*)Nat_Car, s_rn66de28},
    {(void*)Nat_Cdr, s_rn6707f0},
    {(void*)Nat_SetCar, s_rn6707f4},
    {(void*)Nat_SetCdr, s_rn670800},
    {(void*)Nat_SymbolP, s_rn67080c},
    {(void*)Nat_SymbolToString, s_rn670814},
    {(void*)Nat_StringToSymbol, s_rn670824},
    {(void*)Nat_CharP, s_rn670834},
    {(void*)Nat_CharEq, s_rn67083c},
    {(void*)Nat_CharLt, s_rn670844},
    {(void*)Nat_CharGt, s_rn67084c},
    {(void*)Nat_CharLe, s_rn670854},
    {(void*)Nat_CharGe, s_rn67085c},
    {(void*)Nat_CharToInteger, s_rn670864},
    {(void*)Nat_IntegerToChar, s_rn670874},
    {(void*)g_rf49c230, s_rn670884},
    {(void*)g_rf49c610, s_rn670890},
    {(void*)Nat_StringLength, s_rn670898},
    {(void*)Nat_StringRef, s_rn6708a8},
    {(void*)Nat_StringSet, s_rn6708b4},
    {(void*)Nat_VectorP, s_rn6708c0},
    {(void*)Nat_MakeVector, s_rn6708c8},
    {(void*)Nat_ListToVector, s_rn6708d4},
    {(void*)Nat_VectorLength, s_rn6708ec},
    {(void*)Nat_VectorRef, s_rn6708fc},
    {(void*)Nat_VectorSet, s_rn67128c},
    {(void*)Nat_ProcedureP, s_rn670914},
    {(void*)Nat_Field48, s_rn671270},
    {(void*)Nat_MakeEnv, s_rn67125c},
    {(void*)Nat_Field60, s_rn671244},
    {(void*)Nat_InputPortP, s_rn670920},
    {(void*)Nat_OutputPortP, s_rn67092c},
    {(void*)Nat_Field72, s_rn671230},
    {(void*)Nat_Field84, s_rn67121c},
    {(void*)Nat_OpenInputFile, s_rn67093c},
    {(void*)Nat_OpenOutputFile, s_rn67094c},
    {(void*)Nat_CloseInputPort, s_rn670960},
    {(void*)Nat_CloseOutputPort, s_rn670974},
    {(void*)Nat_ReadChar, s_rn670988},
    {(void*)Nat_PeekChar, s_rn670994},
    {(void*)Nat_EofObjectP, s_rn6709a0},
    {(void*)Nat_CharReadyP, s_rn6709ac},
    {(void*)Nat_WriteChar, s_rn6709b8},
    {(void*)Nat_Read, s_rn6709c4},
    {(void*)Nat_Write, s_rn6709cc},
    {(void*)Nat_SetValues, s_rn671210},
    {(void*)Nat_StringAppend, s_rn6709ec},
    {(void*)g_rf4a0af0, s_rn6709fc},
    {(void*)Nat_Not, s_rn670a04},
    {(void*)Nat_NullP, s_rn670a08},
    {(void*)Nat_List, s_rn671208},
    {(void*)Nat_Caar, s_rn670a10},
    {(void*)Nat_Cadr, s_rn670a18},
    {(void*)Nat_Cdar, s_rn670a20},
    {(void*)Nat_Cddr, s_rn670a28},
};

struct SchValue12 { DWORD d[3]; };

static SchValue12 Sv_Copy(const SchValue* v)
{
    SchValue12 r;
    CopyRef((SchValue*)&r, v);
    return r;
}

// a value assignment that keeps the old object when it is the same value
static void Sv_Assign(SchValue* to, const SchValue* from)
{
    if (to->obj && !(to->flags & SCH_IMMEDIATE) && --F(int, to->obj, 4) == 0
        && !(to->obj == from->obj && to->flags == from->flags && to->m_4 == from->m_4) && to->obj)
        CallVirt(to->obj, 12, 1);
    CopyRef(to, from);
}

// a port (type 11) from v into the value at to, after releasing it; another type is an error and leaves it
static void Sv_AssignPort(SchValue* to, const SchValue* v)
{
    if (to->obj && --F(int, to->obj, 4) == 0
        && !(to->obj == v->obj && to->flags == v->flags && to->m_4 == v->m_4) && to->obj)
        CallVirt(to->obj, 12, 1);
    if (v->obj) {
        if (CallVirt<int>(v->obj, 0) != 11) {
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(v->obj, 4), (const char*)s_svPort);
            return;
        }
        to->obj = v->obj;
        to->flags = v->flags;
        F(int, v->obj, 4)++;
    } else {
        to->obj = 0;
        to->flags = 0;
    }
    to->m_4 = 0;
}

static void Sv_Define(BYTE* self, void (*make)(), const char* name)
{
    SchValue t;
    SchValue12 v = Sv_Copy(CallC<SchValue*>(make, &t, 0));
    CallThis(FUN_4a3e40, CallThis<BYTE*>(FUN_4aa430, self + 48), self, name, v);
    CallThis(FUN_4a36c0, &t);
}

extern "C" {

// [FUN_4ad440] constructs the interpreter (self): startup names the start-up script (default $6711e8), name
// the value defined as $6711f4 (default $671204)
BYTE* __fastcall Sch_Construct(BYTE* self, void*, const char* startup, const char* name)
{
    F(DWORD, self, 0) = 0;
    F(DWORD, self, 8) = 0;
    F(DWORD, self, 12) = 0;
    F(void*, self, 4) = g_sv64bed0;
    F(DWORD, self, 16) = 0;
    F(DWORD, self, 20) = 0;
    F(DWORD, self, 28) = 0;
    F(DWORD, self, 32) = 0;
    F(void*, self, 24) = g_sv64bed0;
    F(DWORD, self, 36) = 0;
    F(DWORD, self, 40) = 0;
    BYTE** head = CallC<BYTE**>(FUN_44b740, 12);
    F(BYTE**, self, 44) = head;
    head[0] = (BYTE*)head;
    F(BYTE**, self, 44)[1] = (BYTE*)F(BYTE**, self, 44);
    for (int k = 48; k <= 120; k += 4)
        F(DWORD, self, k) = 0;
    F(DWORD, self, 4220) = 0;
    F(DWORD, self, 4224) = 0;
    F(DWORD, self, 4228) = 0;
    F(DWORD, self, 4232) = 0;
    // the 44-byte elements at +108: reserve 256
    BYTE** v44 = (BYTE**)(self + 108);
    if ((unsigned)((v44[2] - v44[0]) / 44) < 256) {
        int n = (v44[1] - v44[0]) / 44;
        BYTE* nb = CallStd<BYTE*>(FUN_4aaed0, 256, v44[0], v44[1]);
        for (BYTE* q = v44[0]; q != v44[1]; q += 44)
            CallVirt(q, 4, 0);
        if (int cap = (v44[2] - v44[0]) / 44)
            CallC(FUN_44b4a0, v44[0], cap * 44);
        v44[0] = nb;
        v44[1] = nb + n * 44;
        v44[2] = nb + 11264;
    }
    F(BYTE*, self, 16) = self + 24;
    F(BYTE*, self, 40) = self + 4;
    F(DWORD, self, 20) = 0;
    F(DWORD, self, 36) = 0;
    SchValue t;
    CallThis(FUN_40c100, self + 48, CallC<SchValue*>(FUN_4a27e0, &t, 0, self));
    CallThis(FUN_4a36c0, &t);

    Sv_Define(self, FUN_4b00c0, s_sv6712f8);
    Sv_Define(self, FUN_4aa9f0, s_sv6712f0);
    Sv_Define(self, FUN_4b01a0, s_sv6712e8);
    Sv_Define(self, FUN_4b0280, s_sv6712e4);
    Sv_Define(self, FUN_4b0360, s_sv64bf60);
    Sv_Define(self, FUN_4b0440, s_sv6712dc);
    Sv_Define(self, FUN_4b0520, s_sv6712d4);
    Sv_Define(self, FUN_4b0600, s_sv6712b4);
    Sv_Define(self, FUN_4b06e0, s_sv6712ac);
    for (int i = 0; i < sizeof(k_schNatives) / sizeof(k_schNatives[0]); i++) {
        SchValue12 env;
        CallThis(FUN_454580, &env, self + 48);
        CallC(FUN_4ad330, self, env, k_schNatives[i].name, k_schNatives[i].fn);
    }

    if (!name)
        name = s_sv671204;
    SchValue12 v = Sv_Copy(CallC<SchValue*>(FUN_4b07c0, &t, 0, &name));
    CallThis(FUN_4a3e40, CallThis<BYTE*>(FUN_4aa430, self + 48), self, (const char*)s_sv6711f4, v);
    SchValue_Release(&t);

    Sv_Assign((SchValue*)(self + 60), CallC<SchValue*>(FUN_4b08a0, &t, 0, self, F(DWORD, self, 48)));
    SchValue_Release(&t);

    const char* port = s_sv686340;
    int mode = 1;
    Sv_AssignPort((SchValue*)(self + 72), CallC<SchValue*>(FUN_4b0990, &t, 0, &port, &mode));
    SchValue_Release(&t);
    port = s_sv686360;
    mode = 2;
    Sv_AssignPort((SchValue*)(self + 84), CallC<SchValue*>(FUN_4b0990, &t, 0, &port, &mode));
    SchValue_Release(&t);
    port = s_sv686380;
    mode = 2;
    Sv_AssignPort((SchValue*)(self + 96), CallC<SchValue*>(FUN_4b0990, &t, 0, &port, &mode));
    SchValue_Release(&t);

    // the start-up script: read one expression and evaluate it
    if (!startup)
        startup = s_sv6711e8;
    mode = 1;
    SchValue* r = CallC<SchValue*>(FUN_455f00, &t, 0, &startup, &mode);
    SchValue h = {0, 0, 0, 0};
    if (r->obj) {
        if (CallVirt<int>(r->obj, 0) == 11) {
            h.obj = r->obj;
            h.flags = r->flags;
            F(int, h.obj, 4)++;
        } else
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(r->obj, 4), (const char*)s_svPort);
    }
    SchValue_Release(&t);
    if (!h.obj)
        CallC(FUN_4a52e0, 0, 1);
    if (h.flags & SCH_IMMEDIATE)
        CallC(FUN_4a52e0, 4, 1);
    if (!CallVirt<int>(h.obj, 40)) {
        SchValue e, x;
        CallC(FUN_4b4c20, &e, self, Sv_Copy(&h));
        if (e.flags || e.obj) {
            CallC(FUN_4a6af0, &x, self, Sv_Copy(&e));
            SchValue_Release(&x);
        }
        SchValue_Release(&e);
    }
    if (!(h.flags & SCH_IMMEDIATE) && h.obj && --F(int, h.obj, 4) == 0)
        CallVirt(h.obj, 12, 1);
    return self;
}

}  // extern "C"
