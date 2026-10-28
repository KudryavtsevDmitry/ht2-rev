// evaluator.cpp - Sch_eval.cpp of the game, rewritten from king.masm: small
// functions of the AI's script evaluator: reference-counted values and the
// scopes holding them, the constructors of the expression nodes, the map and
// list helpers, pools of freed blocks.

#include "king.h"

extern "C" BYTE* __cdecl Values_Copy(const BYTE* first, const BYTE* last, BYTE* dest);  // natives.cpp
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl _stricmp(const char* a, const char* b);
extern "C" void** __stdcall List_Erase(void** out, void** node);     // aimain.cpp

extern "C" void __fastcall HandlePair_Destroy(BYTE* self, int);
extern "C" void FUN_4a36f0(); KING_ALIAS("_FUN_4a36f0", "@HandlePair_Destroy@8")    // destructor of the value holders (map and list entries)
extern "C" void __cdecl Ai_PoolFree(void* p, unsigned size);
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // the AI's allocator: free (p, size)
extern "C" void* __cdecl Stl_Allocate(unsigned bytes);
extern "C" int __cdecl Sch_ErrorCode(int code, int severity);
extern "C" void FUN_4a52e0(); KING_ALIAS("_FUN_4a52e0", "_Sch_ErrorCode")    // script error (code, fatal)
extern "C" void __cdecl Sch4ab570_Construct(BYTE* d, const BYTE* s);
extern "C" void FUN_4ab570(); KING_ALIAS("_FUN_4ab570", "_Sch4ab570_Construct")    // constructs a 44-byte element at p from x
extern "C" void __fastcall Elems44_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x);
extern "C" void FUN_4aa460(); KING_ALIAS("_FUN_4aa460", "@Elems44_InsertAux@16")    // vector<44 bytes>::insert_aux
extern "C" void FUN_4ace70(); KING_ALIAS("_FUN_4ace70", "_Deque_CopyCharsTag")    // copy (first, last, deque iterator, tag)
extern "C" BYTE* __stdcall ValueList_MakeNode(const DWORD* v);
extern "C" void FUN_4afe80(); KING_ALIAS("_FUN_4afe80", "_ValueList_MakeNode@4")    // list: a new node holding x
extern "C" void FUN_467780(); KING_ALIAS("_FUN_467780", "_UninitCopyStrRefs")
extern "C" BYTE* __cdecl Items44_UninitCopy(const BYTE* first, const BYTE* last, BYTE* dest, void*);
extern "C" void FUN_4abd20(); KING_ALIAS("_FUN_4abd20", "_Items44_UninitCopy")
extern "C" BYTE* __fastcall SchMap_CopySubtree(BYTE* tree, void*, const BYTE* x, BYTE* p);
extern "C" void FUN_4a4950(); KING_ALIAS("_FUN_4a4950", "@SchMap_CopySubtree@16")
extern "C" void FUN_454580(); KING_ALIAS("_FUN_454580", "@Str_CopyConstruct@12")
KING_VAR(AsmLabel, s_errNotSymbolList, 67115c)
extern "C" int __cdecl Sch_Error(int code, int level, const char* a, const char* b);
extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")
extern "C" void FUN_4b6500(); KING_ALIAS("_FUN_4b6500", "@SchSymbol_Construct@12")
extern "C" void FUN_4a8580(); KING_ALIAS("_FUN_4a8580", "@SchNode4a8580_Construct@8")
extern "C" void FUN_4910a0(); KING_ALIAS("_FUN_4910a0", "@RingB_Construct@12")
extern "C" void FUN_491380(); KING_ALIAS("_FUN_491380", "@Pooled_Construct@12")
extern "C" BYTE* __fastcall RingA_Construct(BYTE* s, void*, BYTE* ring, int a, int b);
extern "C" void FUN_4a81d0(); KING_ALIAS("_FUN_4a81d0", "@SchNode4a81d0_Construct@8")
extern "C" void FUN_4a9040(); KING_ALIAS("_FUN_4a9040", "@SchNode4a9040_Construct@8")
extern "C" void FUN_4a86f0(); KING_ALIAS("_FUN_4a86f0", "@SchNode4a86f0_Construct@8")
extern "C" void FUN_4a8a90(); KING_ALIAS("_FUN_4a8a90", "@SchNode4a8a90_Construct@8")
extern "C" void FUN_4a8bd0(); KING_ALIAS("_FUN_4a8bd0", "@SchNode4a8bd0_Construct@8")
extern "C" void FUN_4a9770(); KING_ALIAS("_FUN_4a9770", "@SchNode4a9770_Construct@8")
extern "C" void FUN_4a9a70(); KING_ALIAS("_FUN_4a9a70", "@SchNode4a9a70_Construct@8")
extern "C" void FUN_4a9e80(); KING_ALIAS("_FUN_4a9e80", "@SchNode4a9e80_Construct@8")
extern "C" void FUN_4b62c0(); KING_ALIAS("_FUN_4b62c0", "@SchString_Construct@12")
KING_VAR(AsmLabel, s_tIdentifier, 66ce4c)
KING_VAR(AsmLabel, s_tUnspecified, 671168)
KING_VAR(AsmLabel, s_tBegin, 671124)
KING_VAR(AsmLabel, s_tClosure, 6705f4)
KING_VAR(AsmLabel, s_tContinuation, 6705fc)
KING_VAR(AsmLabel, s_tLambda, 67111c)
KING_VAR(AsmLabel, s_tDefine, 67113c)
KING_VAR(AsmLabel, s_tIf, 67112c)
KING_VAR(AsmLabel, s_tQuote, 671130)
KING_VAR(AsmLabel, s_tSet, 671138)
KING_VAR(AsmLabel, s_tEval, 671144)
KING_VAR(AsmLabel, s_tCallCC, 67114c)
KING_VAR(AsmLabel, s_tApply, 671154)
KING_VAR(AsmLabel, s_tString, 66ca58)
KING_VAR(AsmLabel, g_vtblUnspecified, 64c034)

KING_VAR(int, g_69224c, 69224c)
KING_VAR(AsmLabel, g_strImmediate, 671104)    // "<IMMEDIATE>"
typedef void* Blocks[1];
KING_VAR(int, g_free44Count, 693650)          // freed 44-byte blocks
KING_VAR(Blocks, g_free44, 692650)
KING_VAR(int, g_free32Count, 694658)          // freed 32-byte blocks
KING_VAR(Blocks, g_free32, 693658)
KING_VAR(AsmLabel, g_vtbl64bf68, 64bf68)
KING_VAR(AsmLabel, g_vtbl64bf70, 64bf70)
KING_VAR(AsmLabel, g_vtbl64bf80, 64bf80)
KING_VAR(AsmLabel, g_vtbl64bf94, 64bf94)
KING_VAR(AsmLabel, g_vtbl64bfa8, 64bfa8)
KING_VAR(AsmLabel, g_vtbl64bfbc, 64bfbc)
KING_VAR(AsmLabel, g_vtbl64bfd0, 64bfd0)
KING_VAR(AsmLabel, g_vtbl64bfe4, 64bfe4)
KING_VAR(AsmLabel, g_vtbl64bff8, 64bff8)
KING_VAR(AsmLabel, g_vtbl64c00c, 64c00c)
KING_VAR(AsmLabel, g_vtbl64c020, 64c020)

static void AiFree(void* p, int size) { Ai_PoolFree(p, size); }
static void SchError(int code) { Sch_ErrorCode(code, 1); }

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
        HandlePair_Destroy(node + 16, 0);
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
    HandlePair_Destroy(self, 0);
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
        Sch4ab570_Construct(end, (const BYTE*)x);
        F(BYTE*, self, 4) += 44;
    } else
        Elems44_InsertAux((BYTE**)self, 0, end, (const BYTE*)x);
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
        p = (BYTE*)Stl_Allocate(n * 12);
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
    BYTE* n = ValueList_MakeNode((const DWORD*)x);
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

extern "C" char* __cdecl Sch_ErrorText(int code, int severity);
extern "C" void FUN_4a5380(); KING_ALIAS("_FUN_4a5380", "_Sch_ErrorText")    // the format of an error (code, level)
extern "C" char* __cdecl Sch_MessageText(int i);
extern "C" void FUN_4a59f0(); KING_ALIAS("_FUN_4a59f0", "_Sch_MessageText")    // the format of a message (code)
extern "C" void __cdecl Port_WriteString(const char* s, DWORD h0, DWORD h1, DWORD h2);
extern "C" void FUN_4b1450(); KING_ALIAS("_FUN_4b1450", "_Port_WriteString")    // shows a message (text, value by copy)
extern "C" void __cdecl Sch4a5800_Dump(BYTE* s);
extern "C" void FUN_4a5800(); KING_ALIAS("_FUN_4a5800", "_Sch4a5800_Dump")
extern "C" void __fastcall SchHandle_Release(void** h, int);
extern "C" void FUN_4a36c0(); KING_ALIAS("_FUN_4a36c0", "@SchHandle_Release@8")    // a value's destructor
extern "C" void FUN_4a4540(); KING_ALIAS("_FUN_4a4540", "_Vec12_AllocCopyS@12")    // vector<value>: allocate and copy (n, first, last)
extern "C" SchValue* __fastcall SchValue_Assign(SchValue* self, void*, const SchValue* from);
KING_ALIAS("@SchValue_Assign@12", "@Str_Assign@12")
extern "C" BYTE* __cdecl UninitCopyStrRefs(const BYTE* first, const BYTE* last, BYTE* dest);  // aistl.cpp
typedef int Ints[1];
KING_VAR(Ints, g_errorOff, 691dbc)            // per error code: don't report
KING_VAR(int, g_lastErrorCode, 692244)
KING_VAR(int, g_lastErrorLevel, 692248)
KING_VAR(FILE*, g_errorFile, 670b04)
typedef void (__cdecl* ErrorHandler)(int code, int level, const char* text);

// reports a script error to the log file and the handler; level 1 (and
// below, except 0) ends the program
// returns what the original leaves in EAX: the code's "off" flag, or the level
extern "C" int __cdecl Sch_Error(int code, int level, const char* a, const char* b)
{
    if (g_errorOff[code])
        return g_errorOff[code];
    const char* fmt = (const char*)Sch_ErrorText(code, level);
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
            SchHandle_Release((void**)e, 0);
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
                SchHandle_Release((void**)to, 0);
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
    Sch4a5800_Dump(p);
    int r = ((Show)FUN_4b1450)(text, NodeValue(p));
    F(int, p, 120) = 1;
    return r;
}

extern "C" int __cdecl Sch_Message0(BYTE* p, int code)
{
    char text[2048];
    sprintf(text, (const char*)Sch_MessageText(code));
    return ShowMessage(p, text);
}

extern "C" int __cdecl Sch_Message1(BYTE* p, int code, int a)
{
    char text[2048];
    sprintf(text, (const char*)Sch_MessageText(code), a);
    return ShowMessage(p, text);
}

extern "C" int __cdecl Sch_Message2(BYTE* p, int code, int a, int b)
{
    char text[2048];
    sprintf(text, (const char*)Sch_MessageText(code), a, b);
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
extern "C" BYTE* __fastcall SchScope_Copy(BYTE* self, void*, BYTE* outer, BYTE* s);
extern "C" BYTE* __fastcall SchItem4b5f30_Construct(BYTE*, void*, int, int);
KING_VAR(AsmLabel, g_inputEnvironmentName, 67060c)
KING_VAR(AsmLabel, g_inputPortName, 66cb28)

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
    BYTE* object = storage ? SchScope_Copy(storage, 0, (BYTE*)a, (BYTE*)b) : 0;
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
        Sch_Error(2, 1, has, name);
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
    BYTE* c = o ? RingA_Construct(o, 0, (BYTE*)a, *b, *d) : 0;
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
    BYTE* head = (BYTE*)Stl_Allocate(48);
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
    BYTE* head = (BYTE*)Stl_Allocate(44);
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
        F(BYTE*, h, 4) = SchMap_CopySubtree((BYTE*)self, 0, sroot, h);
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
    BYTE* node = (BYTE*)Stl_Allocate(48);
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
    BYTE* n = (BYTE*)Stl_Allocate(24);
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
        Sch_ErrorCode(6, 1);
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
    return bytes ? Stl_Allocate(bytes) : 0;
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
    Items44_UninitCopy(first, last, p, (void*)n);
    return p;
}

// [FUN_4afe80] a 24-byte list node holding a copy of the value v
BYTE* __stdcall ValueList_MakeNode(const DWORD* v)
{
    BYTE* n = (BYTE*)Stl_Allocate(24);
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

extern "C" DWORD* __cdecl Sch_MakeIdentifierP(DWORD* out, int unused, const DWORD* name);
extern "C" void FUN_4a50d0(); KING_ALIAS("_FUN_4a50d0", "_Sch_MakeIdentifierP")
extern "C" void __fastcall Env_Define(BYTE* e, void*, BYTE* p, BYTE* n0, int n1, DWORD n2, BYTE* v0, int v1, DWORD v2);
extern "C" void FUN_4a3180(); KING_ALIAS("_FUN_4a3180", "@Env_Define@36")
extern "C" DWORD* __fastcall Sch_Copy16(DWORD* self, void*, const DWORD* from);
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
    Sch_MakeIdentifierP(c2, 0, (const DWORD*)&b);
    int r = CallThis<int>(FUN_4a3180, self, a, c2[0], c2[1], c2[2], c1[0], c1[1], c1[2]);
    ReleaseRef(&h);
    return r;
}

// [FUN_4acc20] deque algorithm step: t = FUN_4acb40(a, b, e), then
// out = FUN_4acae0(c, d, t)
DequeIt* __cdecl Deque4acc20(DequeIt* out, DequeIt a, DequeIt b, int c, int d, DequeIt e)
{
    DequeIt b2, a2, t;
    Sch_Copy16((DWORD*)&b2, 0, (const DWORD*)&b);
    Sch_Copy16((DWORD*)&a2, 0, (const DWORD*)&a);
    CallC(FUN_4acb40, &t, a2, b2, e);
    CallC(FUN_4acae0, out, c, d, t);
    return out;
}

// [FUN_4acd70] t = FUN_4acae0(c, d, e), then out = FUN_4acb40(a, b, t)
DequeIt* __cdecl Deque4acd70(DequeIt* out, int c, int d, DequeIt a, DequeIt b, DequeIt e)
{
    DequeIt t, b2, a2;
    CallC(FUN_4acae0, &t, c, d, e);
    Sch_Copy16((DWORD*)&b2, 0, (const DWORD*)&b);
    Sch_Copy16((DWORD*)&a2, 0, (const DWORD*)&a);
    CallC(FUN_4acb40, out, a2, b2, t);
    return out;
}

}

extern "C" void FUN_467780(); KING_ALIAS("_FUN_467780", "_UninitCopyStrRefs")    // uninitialized copy of 12-byte values (first, last, to)
extern "C" void __fastcall Deque_AllocPages512(BYTE* self, void*, char** first, char** last);
extern "C" void FUN_4ac330(); KING_ALIAS("_FUN_4ac330", "@Deque_AllocPages512@16")    // deque<char>: allocate the pages of [first, last)
extern "C" BYTE* __fastcall Vec44_At(BYTE* self, void*, int i);
extern "C" void __cdecl Sch_DumpFrame(BYTE* s, BYTE* e, DWORD p0, DWORD p1, DWORD p2);
extern "C" void FUN_4a55c0(); KING_ALIAS("_FUN_4a55c0", "_Sch_DumpFrame")
extern "C" void FUN_4b1450(); KING_ALIAS("_FUN_4b1450", "_Port_WriteString")    // writes a string to a port (by value)
extern "C" DWORD* __cdecl Sch_MakeClosureB(DWORD* out, int unused, DWORD a, const DWORD* b, const DWORD* d);
extern "C" void FUN_4affd0(); KING_ALIAS("_FUN_4affd0", "_Sch_MakeClosureB")
extern "C" BYTE* __fastcall SchValue_Object(const SchValue* self, void*);
extern "C" void FUN_4aa430(); KING_ALIAS("_FUN_4aa430", "@SchValue_Object@8")
KING_VAR(char, g_6710dc, 6710dc)

struct Value3 { BYTE* obj; int len; DWORD flags; };

// a vector of 12-byte values copied from [first, last) into self
static void Values_CopyConstruct(BYTE** self, BYTE* first, BYTE* last, int junk)
{
    int n = (last - first) / 12;
    self[0] = self[1] = self[2] = 0;
    BYTE* p = n ? (BYTE*)Stl_Allocate(n * 12) : 0;
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
    char** map = size ? (char**)Stl_Allocate(size * 4) : 0;
    self[0] = (int)map;
    char** start = map + ((self[1] - nodes) >> 1);
    char** finish = start + nodes;
    Deque_AllocPages512((BYTE*)self, 0, start, finish);
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
    Port_WriteString(&g_6710dc, h[0], h[1], h[2]);
    int n = (F(BYTE*, s, 112) - F(BYTE*, s, 108)) / 44;
    for (int i = n - 1; i != -1; i--) {
        CopyRef((SchValue*)h, (const SchValue*)(s + 96));
        BYTE* e = Vec44_At(s + 108, 0, i);
        Sch_DumpFrame(s, e, h[0], h[1], h[2]);
    }
}

// [FUN_4ad330] makes a closure of a, &c and &b, and passes it with a and
// b to the object of h
void __cdecl Sch4ad330(int a, Value3 h, int b, int c)
{
    DWORD l[3];
    const SchValue* r = (const SchValue*)Sch_MakeClosureB(l, 0, a, (const DWORD*)&c, (const DWORD*)&b);
    DWORD v[3];
    CopyRef((SchValue*)v, r);
    BYTE* o = SchValue_Object((const SchValue*)&h, 0);
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
extern "C" void __cdecl ConstructStrRef(DWORD* d, const DWORD* s);
extern "C" void FUN_44c710(); KING_ALIAS("_FUN_44c710", "@StrRefVec_InsertAux@16")    // vector of values: insert at the end (reallocating)
extern "C" int __cdecl Sch_Message0(BYTE* p, int code);
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
        SchHandle_Release((void**)p, 0);
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
            SchHandle_Release((void**)e, 0);
        int cap = (v[2] - v[0]) / 12;
        if (cap)
            AiFree(v[0], cap * 12);
        v[0] = p;
        v[2] = p + 96;
        v[1] = p + n * 12;
    }
    if (v[1] != v[2]) {
        ConstructStrRef((DWORD*)v[1], h);
        v[1] += 12;
    } else
        CallThis(FUN_44c710, v, v[1], h);
    if (h[0] || h[2] || h[1])
        return;
    if ((unsigned)((F(BYTE*, ctx, 112) - F(BYTE*, ctx, 108)) / 44) > 1 && !F(int, ctx, 120))
        Sch_Message0(ctx, 31);
}

}

extern "C" void FUN_4a51d0(); KING_ALIAS("_FUN_4a51d0", "_SchMap_MakeNode@4")    // map: new node holding a copy of the key (stdcall)
extern "C" void __cdecl NameMap_Rebalance(RbNode* x, RbNode** root);
extern "C" void __fastcall SchMapIt_Dec(BYTE** self, void*);
extern "C" void FUN_4a4f30(); KING_ALIAS("_FUN_4a4f30", "_SchValue_LessByName@8")    // map: key less (stdcall a, b)
extern "C" BYTE* __fastcall Sch4a51b0_Construct(BYTE* self, void*, const int* a, const BYTE* b);

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
    NameMap_Rebalance((RbNode*)z, (RbNode**)(self[0] + 4));
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
        SchMapIt_Dec(&j, 0);
    }
    if (!key->obj)
        Sch_ErrorCode(0, 1);
    if (key->flags & SCH_IMMEDIATE)
        Sch_ErrorCode(4, 1);
    const char* kk = F(const char*, key->obj, 12);
    if (_stricmp(KeyName((const SchValue*)(j + 16)), kk) < 0) {
        bool left = y == self[0] || x || CallThis<BYTE>(FUN_4a4f30, self + 2, key, y + 16);
        F(BYTE*, out, 0) = Map_Link(self, y, left, key);
        F(BYTE, out, 4) = 1;
        return out;
    }
    bool no = false;
    Sch4a51b0_Construct(out, 0, (const int*)&j, (const BYTE*)&no);
    return out;
}

}

extern "C" void __fastcall SchMap_Erase(BYTE* self, void*, BYTE* node);
extern "C" BYTE* __fastcall SchHolder_DeletingDestructor(BYTE* self, void*, int flags);
KING_VAR(char, gv64bf4c, 64bf4c)
KING_VAR(char, gv64bed0, 64bed0)
KING_VAR(char, gv64bee4, 64bee4)

// empties a map (header +0, count +4) without freeing its header
static void Map_Clear(BYTE** m)
{
    if (!((int*)m)[1])
        return;
    SchMap_Erase((BYTE*)m, 0, F(BYTE*, m[0], 4));
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
            SchMap_Erase((BYTE*)self, 0, F(BYTE*, x, 12));
            BYTE* left = F(BYTE*, x, 8);
            SchHolder_DeletingDestructor(x + 16, 0, 0);
            Ai_PoolFree(x, 44);
            x = left;
        }
        BYTE* h = self[0];
        F(BYTE*, h, 8) = h;
        F(BYTE*, h, 4) = 0;
        F(BYTE*, h, 12) = h;
        ((int*)self)[1] = 0;
    }
    Ai_PoolFree(self[0], 44);
}

// [FUN_4a3fd0] destructor of a scope
void __fastcall SchScope_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = &gv64bf4c;
    BYTE** m = F(BYTE**, self, 144);
    if (m) {
        Map_Clear(m);
        Ai_PoolFree(m[0], 44);
        operator delete(m);
    }
    F(BYTE**, self, 144) = 0;
    ReleaseRef((SchValue*)(self + 148));
    BYTE** v = (BYTE**)(self + 132);
    for (BYTE* e = v[0]; e != v[1]; e += 12)
        SchHandle_Release((void**)e, 0);
    int n = (v[2] - v[0]) / 12;
    if (n)
        Ai_PoolFree(v[0], n * 12);
    for (int i = 7; i >= 0; i--)
        SchHandle_Release((void**)(self + 36 + i * 12), 0);
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

extern "C" void __fastcall SchNode_Destroy(void** self, int);
extern "C" void FUN_4b13b0(); KING_ALIAS("_FUN_4b13b0", "@SchNode_Destroy@8")    // the script node base destructor
KING_VAR(char, vt_64bf80, 64bf80)
KING_VAR(char, vt_64bf94, 64bf94)
KING_VAR(char, vt_64bfa8, 64bfa8)
KING_VAR(char, vt_64bfbc, 64bfbc)
KING_VAR(char, vt_64bfd0, 64bfd0)
KING_VAR(char, vt_64bfe4, 64bfe4)
KING_VAR(char, vt_64bff8, 64bff8)
KING_VAR(char, vt_64c00c, 64c00c)
KING_VAR(char, vt_64c020, 64c020)

// [FUN_4a8570] destructor of a script node class (vtable 64bf80)
extern "C" void __fastcall SchNode4a8570_Destroy(void** self, int)
{
    self[0] = &vt_64bf80;
    SchNode_Destroy(self, 0);
}

// [FUN_4a86e0] destructor of a script node class (vtable 64bf94)
extern "C" void __fastcall SchNode4a86e0_Destroy(void** self, int)
{
    self[0] = &vt_64bf94;
    SchNode_Destroy(self, 0);
}

// [FUN_4a8a80] destructor of a script node class (vtable 64bfa8)
extern "C" void __fastcall SchNode4a8a80_Destroy(void** self, int)
{
    self[0] = &vt_64bfa8;
    SchNode_Destroy(self, 0);
}

// [FUN_4a8bc0] destructor of a script node class (vtable 64bfbc)
extern "C" void __fastcall SchNode4a8bc0_Destroy(void** self, int)
{
    self[0] = &vt_64bfbc;
    SchNode_Destroy(self, 0);
}

// [FUN_4a9030] destructor of a script node class (vtable 64bfd0)
extern "C" void __fastcall SchNode4a9030_Destroy(void** self, int)
{
    self[0] = &vt_64bfd0;
    SchNode_Destroy(self, 0);
}

// [FUN_4a9760] destructor of a script node class (vtable 64bfe4)
extern "C" void __fastcall SchNode4a9760_Destroy(void** self, int)
{
    self[0] = &vt_64bfe4;
    SchNode_Destroy(self, 0);
}

// [FUN_4a9a60] destructor of a script node class (vtable 64bff8)
extern "C" void __fastcall SchNode4a9a60_Destroy(void** self, int)
{
    self[0] = &vt_64bff8;
    SchNode_Destroy(self, 0);
}

// [FUN_4a9e70] destructor of a script node class (vtable 64c00c)
extern "C" void __fastcall SchNode4a9e70_Destroy(void** self, int)
{
    self[0] = &vt_64c00c;
    SchNode_Destroy(self, 0);
}

// [FUN_4aa150] destructor of a script node class (vtable 64c020)
extern "C" void __fastcall SchNode4aa150_Destroy(void** self, int)
{
    self[0] = &vt_64c020;
    SchNode_Destroy(self, 0);
}

KING_VAR(void*, g_pool692650, 692650)
KING_VAR(int, g_poolCount693650, 693650)
KING_VAR(void*, g_pool693658, 693658)
KING_VAR(int, g_poolCount694658, 694658)

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

KING_VAR(char, vt_64bf68, 64bf68)

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
        Ai_PoolFree(b, n * 12);
    SchHandle_Release((void**)(self + 4), 0);
}

// [FUN_4abec0] frees a deque's 512-byte blocks (from the map slot at +20 to the one at +36) and its map
extern "C" void __fastcall Deque_FreeStorage(DWORD* self, void*)
{
    if (!self[0])
        return;
    for (void** p = (void**)self[5]; p < (void**)(self[9] + 4); p++)
        Ai_PoolFree(*p, 512);
    if (self[1])
        Ai_PoolFree((void*)self[0], self[1] * 4);
}

extern "C" long __cdecl strtol(const char* s, char** end, int base);
extern "C" char* __cdecl strncat(char* d, const char* s, size_t n);
typedef const char* SchStrTable[1];
KING_VAR(SchStrTable, g_msgs670e08, 670e08)
KING_VAR(AsmLabel, s_670dc8, 670dc8)
KING_VAR(AsmLabel, s_fmt6710ec, 6710ec)
KING_VAR(AsmLabel, g_text692250, 692250)

// [FUN_4a59f0] message i of the table: its number (checked against i) and the text after it
extern "C" char* __cdecl Sch_MessageText(int i)
{
    char* end;
    if (strtol(g_msgs670e08[i], &end, 0) != i)
        fprintf(CRT_STREAM(2), s_670dc8);
    sprintf(g_text692250, s_fmt6710ec, i);
    strncat(g_text692250, end, 1024);
    return g_text692250;
}

KING_VAR(SchStrTable, g_msgs670a7c, 670a7c)
KING_VAR(AsmLabel, s_fmt670db8, 670db8)
KING_VAR(AsmLabel, g_text691e44, 691e44)

// [FUN_4a5380] error message code with its severity letter (I W E F ?): the format and the text after the number
extern "C" char* __cdecl Sch_ErrorText(int code, int severity)
{
    char* end;
    if (strtol(g_msgs670a7c[code], &end, 0) != code)
        fprintf(CRT_STREAM(2), s_670dc8);
    char c = severity >= 768 ? 'I' : severity >= 512 ? 'W' : severity >= 256 ? 'E' : severity >= 1 ? 'F' : '?';
    sprintf(g_text691e44, s_fmt670db8, (int)c, code);
    strncat(g_text691e44, end, 1024);
    return g_text691e44;
}

extern "C" size_t __cdecl strlen(const char* s);
typedef int SchIntTable[1];
KING_VAR(SchIntTable, g_muted691dbc, 691dbc)
KING_VAR(int, g_lastError692244, 692244)
KING_VAR(int, g_lastSeverity692248, 692248)
typedef void (__cdecl* SchErrorHook)(int code, int severity, const char* text);
KING_VAR(SchErrorHook, g_errorHook69224c, 69224c)

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

KING_VAR(AsmLabel, s_tSpecialForm, 671110)
KING_VAR(AsmLabel, s_port, 66cb28)
KING_VAR(AsmLabel, s_tNumber, 66ca60)


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
    SchNode_Destroy((void**)s, 0);
    if (flags & 1)
        SchPool2_Free(s);
    return s;
}

extern "C" int __cdecl Sch_Message1(BYTE* p, int code, int a);
extern "C" void FUN_4a5a60(); KING_ALIAS("_FUN_4a5a60", "_Sch_Message1")
KING_VAR(AsmLabel, s_eva671104, 671104)

// [$L_4a74f0] message 29 about the scope's frame (+112, 28 bytes back): its name or 671104
extern "C" int __fastcall Eva_4a74f0(BYTE*, void*, BYTE* p)
{
    BYTE* e = *(BYTE**)(F(BYTE*, p, 112) - 28);
    const char* name;
    if (F(int, e, 8) & 0x10000)
        name = s_eva671104;
    else
        name = CallVirt<const char*>(F(BYTE*, e, 0), 4);
    return Sch_Message1(p, 29, (int)name);
}

// ---------------------------------------------------------------------------
// Static initializers of random generators.

KING_VAR(AsmLabel, g_evaRng694660, 694660)
KING_VAR(DWORD, g_evaRng694738, 694738)
KING_VAR(DWORD, g_evaRng69473c, 69473c)
KING_VAR(DWORD, g_evaRng694740, 694740)

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
extern "C" void __fastcall Eval4a4940_Free(BYTE* s) { Ai_PoolFree(F(void*, s, 0), 44); }

extern "C" void FUN_4ad030(); KING_ALIAS("_FUN_4ad030", "_Deque_CopyBackwardLoop")
// [FUN_4acd20] copies [a, b) backwards to the end c (copies of the three iterators), the result into out
extern "C" DequeIt* __cdecl Deque_CopyBackward(DequeIt* out, DequeIt a, DequeIt b, DequeIt c)
{
    DequeIt c2, b2, a2;
    Sch_Copy16((DWORD*)&c2, 0, (const DWORD*)&c);
    Sch_Copy16((DWORD*)&b2, 0, (const DWORD*)&b);
    Sch_Copy16((DWORD*)&a2, 0, (const DWORD*)&a);
    CallC(FUN_4ad030, out, a2, b2, c2, out, 0);
    return out;
}

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
// [$L_4af6b0] destroys a list of 16-byte nodes: every node and then the head go back to the allocator
extern "C" void __fastcall List16_Destroy(BYTE** s)
{
    for (BYTE* n = *(BYTE**)*s; n != *s;) {
        BYTE* next = *(BYTE**)n;
        Ai_PoolFree(n, 16);
        n = next;
    }
    BYTE* h = *s;
    F(BYTE*, h, 0) = h;
    F(BYTE*, *s, 4) = *s;
    Ai_PoolFree(*s, 16);
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
        Ai_PoolFree(n, 24);
        n = next;
    }
    BYTE* h = *s;
    F(BYTE*, h, 0) = h;
    F(BYTE*, *s, 4) = *s;
    Ai_PoolFree(*s, 24);
}

// ---------------------------------------------------------------------------
// The interpreter constructor: the special forms and the 107 natives of the global environment, the
// standard ports, and the start-up script.

extern "C" BYTE* __stdcall Vec44_AllocCopy(unsigned n, const BYTE* first, const BYTE* last);
extern "C" void FUN_4aaed0(); KING_ALIAS("_FUN_4aaed0", "_Vec44_AllocCopy@12")
extern "C" SchValue* __cdecl SchValue_MakeEnv(SchValue* out, int, void* parent);
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
extern "C" DWORD* __cdecl Sch_MakeStringP(DWORD* out, int unused, const DWORD* s);
extern "C" void FUN_4b07c0(); KING_ALIAS("_FUN_4b07c0", "_Sch_MakeStringP")
extern "C" void FUN_4b08a0(); KING_ALIAS("_FUN_4b08a0", "_SchInput_MakeEnvironment")
extern "C" void FUN_4b0990(); KING_ALIAS("_FUN_4b0990", "_SchInput_MakePort")
extern "C" DWORD* __cdecl Port_MakeB(DWORD* out, int unused, const int* a, const int* b);
extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")
extern "C" void FUN_4b4c20(); KING_ALIAS("_FUN_4b4c20", "_Port_ReadDatum")    // read (out, p, port by value)
extern "C" void FUN_4a6af0(); KING_ALIAS("_FUN_4a6af0", "_Sch_EvalTop")    // eval (out, p, value by value)
KING_VAR(AsmLabel, g_sv64bed0, 64bed0)
KING_VAR(AsmLabel, s_sv6712f8, 6712f8)
KING_VAR(AsmLabel, s_sv6712f0, 6712f0)
KING_VAR(AsmLabel, s_sv6712e8, 6712e8)
KING_VAR(AsmLabel, s_sv6712e4, 6712e4)
KING_VAR(AsmLabel, s_sv64bf60, 64bf60)
KING_VAR(AsmLabel, s_sv6712dc, 6712dc)
KING_VAR(AsmLabel, s_sv6712d4, 6712d4)
KING_VAR(AsmLabel, s_sv6712b4, 6712b4)
KING_VAR(AsmLabel, s_sv6712ac, 6712ac)
KING_VAR(AsmLabel, s_sv671204, 671204)
KING_VAR(AsmLabel, s_sv6711f4, 6711f4)
KING_VAR(AsmLabel, s_sv6711e8, 6711e8)
KING_VAR(AsmLabel, s_svPort, 66cb28)          // "Port"
extern "C" void Nat_MacroExpand();
KING_VAR(AsmLabel, s_rn6709d4, 6709d4)
extern "C" SchValue* __cdecl Nat_Eqv(SchValue*, BYTE*, BYTE**);  // natives.cpp
KING_VAR(AsmLabel, s_rn670640, 670640)
KING_VAR(AsmLabel, s_rn670658, 670658)
extern "C" void Nat_EqualQ();
KING_VAR(AsmLabel, s_rn67065c, 67065c)
extern "C" void Nat_NumberP();
KING_VAR(AsmLabel, s_rn670664, 670664)
extern "C" void Nat_ComplexP();
KING_VAR(AsmLabel, s_rn67066c, 67066c)
extern "C" void Nat_RealP();
KING_VAR(AsmLabel, s_rn670678, 670678)
extern "C" void Nat_RationalP();
KING_VAR(AsmLabel, s_rn670680, 670680)
extern "C" void Nat_IntegerP();
KING_VAR(AsmLabel, s_rn67068c, 67068c)
extern "C" void Nat_ExactP();
KING_VAR(AsmLabel, s_rn670698, 670698)
extern "C" void Nat_InexactP();
KING_VAR(AsmLabel, s_rn6706a0, 6706a0)
extern "C" void Nat_NumEq();
KING_VAR(AsmLabel, s_rn6712a8, 6712a8)
extern "C" void Nat_NumLt();
KING_VAR(AsmLabel, s_rn6712a4, 6712a4)
extern "C" void Nat_NumGt();
KING_VAR(AsmLabel, s_rn6712a0, 6712a0)
extern "C" void Nat_NumLe();
KING_VAR(AsmLabel, s_rn67129c, 67129c)
extern "C" void Nat_NumGe();
KING_VAR(AsmLabel, s_rn671298, 671298)
extern "C" void Nat_Plus();
KING_VAR(AsmLabel, s_rn6706ac, 6706ac)
extern "C" SchValue* __cdecl Nat_Minus(SchValue*, BYTE*, BYTE**);  // natives.cpp
KING_VAR(AsmLabel, s_rn6706b0, 6706b0)
extern "C" void Nat_Times();
KING_VAR(AsmLabel, s_rn6706b4, 6706b4)
extern "C" void Nat_Divide();
KING_VAR(AsmLabel, s_rn670570, 670570)
extern "C" void Nat_Quotient();
KING_VAR(AsmLabel, s_rn6706b8, 6706b8)
extern "C" void Nat_Remainder();
KING_VAR(AsmLabel, s_rn6706c0, 6706c0)
extern "C" void Nat_Modulo();
KING_VAR(AsmLabel, s_rn6706cc, 6706cc)
extern "C" void Nat_Numerator();
KING_VAR(AsmLabel, s_rn6706d4, 6706d4)
extern "C" void Nat_Denominator();
KING_VAR(AsmLabel, s_rn6706e0, 6706e0)
extern "C" void Nat_Floor();
KING_VAR(AsmLabel, s_rn6706ec, 6706ec)
extern "C" void Nat_Ceiling();
KING_VAR(AsmLabel, s_rn6706f4, 6706f4)
extern "C" void Nat_Truncate();
KING_VAR(AsmLabel, s_rn6706fc, 6706fc)
extern "C" void Nat_Round();
KING_VAR(AsmLabel, s_rn670708, 670708)
extern "C" void Nat_Exp();
KING_VAR(AsmLabel, s_rn670710, 670710)
extern "C" void Nat_Log();
KING_VAR(AsmLabel, s_rn670714, 670714)
extern "C" void Nat_Sin();
KING_VAR(AsmLabel, s_rn670718, 670718)
extern "C" void Nat_Cos();
KING_VAR(AsmLabel, s_rn67071c, 67071c)
extern "C" void Nat_Tan();
KING_VAR(AsmLabel, s_rn670720, 670720)
extern "C" void Nat_Asin();
KING_VAR(AsmLabel, s_rn670724, 670724)
extern "C" void Nat_Acos();
KING_VAR(AsmLabel, s_rn67072c, 67072c)
extern "C" void Nat_Atan();
KING_VAR(AsmLabel, s_rn670734, 670734)
extern "C" void Nat_Atan2();
KING_VAR(AsmLabel, s_rn67073c, 67073c)
extern "C" void Nat_Sqrt();
KING_VAR(AsmLabel, s_rn670744, 670744)
extern "C" void Nat_Expt();
KING_VAR(AsmLabel, s_rn67074c, 67074c)
extern "C" void Nat_MakeRect();
KING_VAR(AsmLabel, s_rn670754, 670754)
extern "C" void Nat_MakePolar();
KING_VAR(AsmLabel, s_rn670768, 670768)
extern "C" void Nat_RealPart();
KING_VAR(AsmLabel, s_rn670774, 670774)
extern "C" void Nat_ImagPart();
KING_VAR(AsmLabel, s_rn670780, 670780)
extern "C" void Nat_Magnitude();
KING_VAR(AsmLabel, s_rn67078c, 67078c)
extern "C" void Nat_Angle();
KING_VAR(AsmLabel, s_rn670798, 670798)
extern "C" void Nat_ToInexact();
KING_VAR(AsmLabel, s_rn6707a0, 6707a0)
extern "C" void Nat_ToExact();
KING_VAR(AsmLabel, s_rn6707b0, 6707b0)
extern "C" void Nat_NumberToString();
KING_VAR(AsmLabel, s_rn6707c0, 6707c0)
extern "C" void Nat_StringToNumber();
KING_VAR(AsmLabel, s_rn6707d0, 6707d0)
extern "C" void Nat_PairP();
KING_VAR(AsmLabel, s_rn6707e0, 6707e0)
extern "C" void Nat_Cons();
KING_VAR(AsmLabel, s_rn6707e8, 6707e8)
extern "C" void Nat_Car();
KING_VAR(AsmLabel, s_rn66de28, 66de28)
extern "C" void Nat_Cdr();
KING_VAR(AsmLabel, s_rn6707f0, 6707f0)
extern "C" void Nat_SetCar();
KING_VAR(AsmLabel, s_rn6707f4, 6707f4)
extern "C" void Nat_SetCdr();
KING_VAR(AsmLabel, s_rn670800, 670800)
extern "C" void Nat_SymbolP();
KING_VAR(AsmLabel, s_rn67080c, 67080c)
extern "C" void Nat_SymbolToString();
KING_VAR(AsmLabel, s_rn670814, 670814)
extern "C" void Nat_StringToSymbol();
KING_VAR(AsmLabel, s_rn670824, 670824)
extern "C" void Nat_CharP();
KING_VAR(AsmLabel, s_rn670834, 670834)
extern "C" void Nat_CharEq();
KING_VAR(AsmLabel, s_rn67083c, 67083c)
extern "C" void Nat_CharLt();
KING_VAR(AsmLabel, s_rn670844, 670844)
extern "C" void Nat_CharGt();
KING_VAR(AsmLabel, s_rn67084c, 67084c)
extern "C" void Nat_CharLe();
KING_VAR(AsmLabel, s_rn670854, 670854)
extern "C" void Nat_CharGe();
KING_VAR(AsmLabel, s_rn67085c, 67085c)
extern "C" void Nat_CharToInteger();
KING_VAR(AsmLabel, s_rn670864, 670864)
extern "C" void Nat_IntegerToChar();
KING_VAR(AsmLabel, s_rn670874, 670874)
extern "C" SchValue* __cdecl Nat_MakeString(SchValue*, BYTE*, BYTE**);  // natives.cpp
KING_VAR(AsmLabel, s_rn670884, 670884)
extern "C" SchValue* __cdecl Nat_IsString(SchValue* out, BYTE* p, BYTE** args);
KING_VAR(AsmLabel, s_rn670890, 670890)
extern "C" void Nat_StringLength();
KING_VAR(AsmLabel, s_rn670898, 670898)
extern "C" void Nat_StringRef();
KING_VAR(AsmLabel, s_rn6708a8, 6708a8)
extern "C" void Nat_StringSet();
KING_VAR(AsmLabel, s_rn6708b4, 6708b4)
extern "C" void Nat_VectorP();
KING_VAR(AsmLabel, s_rn6708c0, 6708c0)
extern "C" void Nat_MakeVector();
KING_VAR(AsmLabel, s_rn6708c8, 6708c8)
extern "C" void Nat_ListToVector();
KING_VAR(AsmLabel, s_rn6708d4, 6708d4)
extern "C" void Nat_VectorLength();
KING_VAR(AsmLabel, s_rn6708ec, 6708ec)
extern "C" void Nat_VectorRef();
KING_VAR(AsmLabel, s_rn6708fc, 6708fc)
extern "C" void Nat_VectorSet();
KING_VAR(AsmLabel, s_rn67128c, 67128c)
extern "C" void Nat_ProcedureP();
KING_VAR(AsmLabel, s_rn670914, 670914)
extern "C" void Nat_Field48();
KING_VAR(AsmLabel, s_rn671270, 671270)
extern "C" void Nat_MakeEnv();
KING_VAR(AsmLabel, s_rn67125c, 67125c)
extern "C" void Nat_Field60();
KING_VAR(AsmLabel, s_rn671244, 671244)
extern "C" void Nat_InputPortP();
KING_VAR(AsmLabel, s_rn670920, 670920)
extern "C" void Nat_OutputPortP();
KING_VAR(AsmLabel, s_rn67092c, 67092c)
extern "C" void Nat_Field72();
KING_VAR(AsmLabel, s_rn671230, 671230)
extern "C" void Nat_Field84();
KING_VAR(AsmLabel, s_rn67121c, 67121c)
extern "C" void Nat_OpenInputFile();
KING_VAR(AsmLabel, s_rn67093c, 67093c)
extern "C" void Nat_OpenOutputFile();
KING_VAR(AsmLabel, s_rn67094c, 67094c)
extern "C" void Nat_CloseInputPort();
KING_VAR(AsmLabel, s_rn670960, 670960)
extern "C" void Nat_CloseOutputPort();
KING_VAR(AsmLabel, s_rn670974, 670974)
extern "C" void Nat_ReadChar();
KING_VAR(AsmLabel, s_rn670988, 670988)
extern "C" void Nat_PeekChar();
KING_VAR(AsmLabel, s_rn670994, 670994)
extern "C" void Nat_EofObjectP();
KING_VAR(AsmLabel, s_rn6709a0, 6709a0)
extern "C" void Nat_CharReadyP();
KING_VAR(AsmLabel, s_rn6709ac, 6709ac)
extern "C" void Nat_WriteChar();
KING_VAR(AsmLabel, s_rn6709b8, 6709b8)
extern "C" void Nat_Read();
KING_VAR(AsmLabel, s_rn6709c4, 6709c4)
extern "C" void Nat_Write();
KING_VAR(AsmLabel, s_rn6709cc, 6709cc)
extern "C" void Nat_SetValues();
KING_VAR(AsmLabel, s_rn671210, 671210)
extern "C" void Nat_StringAppend();
KING_VAR(AsmLabel, s_rn6709ec, 6709ec)
extern "C" void Nat_Reverse();
KING_VAR(AsmLabel, s_rn6709fc, 6709fc)
KING_VAR(AsmLabel, s_rn670a04, 670a04)
KING_VAR(AsmLabel, s_rn670a08, 670a08)
KING_VAR(AsmLabel, s_rn671208, 671208)
KING_VAR(AsmLabel, s_rn670a10, 670a10)
KING_VAR(AsmLabel, s_rn670a18, 670a18)
KING_VAR(AsmLabel, s_rn670a20, 670a20)
KING_VAR(AsmLabel, s_rn670a28, 670a28)
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
    {(void*)Nat_Eqv, s_rn670640},
    {(void*)Nat_EqP, s_rn670658},
    {(void*)Nat_EqualQ, s_rn67065c},
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
    {(void*)Nat_Minus, s_rn6706b0},
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
    {(void*)Nat_MakeString, s_rn670884},
    {(void*)Nat_IsString, s_rn670890},
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
    {(void*)Nat_Reverse, s_rn6709fc},
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
            Sch_Error(2, 1, CallVirt<const char*>(v->obj, 4), (const char*)s_svPort);
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
    SchHandle_Release((void**)&t, 0);
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
    BYTE** head = (BYTE**)Stl_Allocate(12);
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
        BYTE* nb = Vec44_AllocCopy(256, v44[0], v44[1]);
        for (BYTE* q = v44[0]; q != v44[1]; q += 44)
            CallVirt(q, 4, 0);
        if (int cap = (v44[2] - v44[0]) / 44)
            Ai_PoolFree(v44[0], cap * 44);
        v44[0] = nb;
        v44[1] = nb + n * 44;
        v44[2] = nb + 11264;
    }
    F(BYTE*, self, 16) = self + 24;
    F(BYTE*, self, 40) = self + 4;
    F(DWORD, self, 20) = 0;
    F(DWORD, self, 36) = 0;
    SchValue t;
    CallThis(FUN_40c100, self + 48, SchValue_MakeEnv(&t, 0, self));
    SchHandle_Release((void**)&t, 0);

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
    SchValue12 v = Sv_Copy((SchValue*)Sch_MakeStringP((DWORD*)&t, 0, (const DWORD*)&name));
    CallThis(FUN_4a3e40, CallThis<BYTE*>(FUN_4aa430, self + 48), self, (const char*)s_sv6711f4, v);
    SchValue_Release(&t);

    Sv_Assign((SchValue*)(self + 60), CallC<SchValue*>(FUN_4b08a0, &t, 0, self, F(DWORD, self, 48)));
    SchValue_Release(&t);

    const char* port = (const char*)CRT_STREAM(0);
    int mode = 1;
    Sv_AssignPort((SchValue*)(self + 72), CallC<SchValue*>(FUN_4b0990, &t, 0, &port, &mode));
    SchValue_Release(&t);
    port = (const char*)CRT_STREAM(1);
    mode = 2;
    Sv_AssignPort((SchValue*)(self + 84), CallC<SchValue*>(FUN_4b0990, &t, 0, &port, &mode));
    SchValue_Release(&t);
    port = (const char*)CRT_STREAM(2);
    mode = 2;
    Sv_AssignPort((SchValue*)(self + 96), CallC<SchValue*>(FUN_4b0990, &t, 0, &port, &mode));
    SchValue_Release(&t);

    // the start-up script: read one expression and evaluate it
    if (!startup)
        startup = s_sv6711e8;
    mode = 1;
    SchValue* r = (SchValue*)Port_MakeB((DWORD*)&t, 0, (const int*)&startup, &mode);
    SchValue h = {0, 0, 0, 0};
    if (r->obj) {
        if (CallVirt<int>(r->obj, 0) == 11) {
            h.obj = r->obj;
            h.flags = r->flags;
            F(int, h.obj, 4)++;
        } else
            Sch_Error(2, 1, CallVirt<const char*>(r->obj, 4), (const char*)s_svPort);
    }
    SchValue_Release(&t);
    if (!h.obj)
        Sch_ErrorCode(0, 1);
    if (h.flags & SCH_IMMEDIATE)
        Sch_ErrorCode(4, 1);
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

// ---------------------------------------------------------------------------
// Calling a closure

extern "C" void FUN_407f00(); KING_ALIAS("_FUN_407f00", "_Str_Member20")
extern "C" void FUN_4090a0(); KING_ALIAS("_FUN_4090a0", "_Str_Member32")
extern "C" void FUN_409070(); KING_ALIAS("_FUN_409070", "_Str_Same")
extern "C" void FUN_40c100(); KING_ALIAS("_FUN_40c100", "@Str_Assign@12")
extern "C" void FUN_49c560(); KING_ALIAS("_FUN_49c560", "@SchValue_Assign@12")
extern "C" BYTE* __fastcall Vec12_At(BYTE** self, void*, int i);
extern "C" void FUN_4a16c0(); KING_ALIAS("_FUN_4a16c0", "@Vec12_At@12")
extern "C" void FUN_4a27e0(); KING_ALIAS("_FUN_4a27e0", "_SchValue_MakeEnv")
extern "C" SchValue* __cdecl SchValue_MakePairB(SchValue* out, int, void* a);
extern "C" void FUN_4a28c0(); KING_ALIAS("_FUN_4a28c0", "_SchValue_MakePairB")
extern "C" BYTE* __fastcall SchNode4a7060_Construct(BYTE* self, void*);
extern "C" void FUN_4a7060(); KING_ALIAS("_FUN_4a7060", "@SchNode4a7060_Construct@8")
extern "C" void __fastcall SchListNode_Destroy(BYTE* self, int);
extern "C" void FUN_4a7230(); KING_ALIAS("_FUN_4a7230", "@SchListNode_Destroy@8")
extern "C" BYTE* __fastcall SchRef_Construct(BYTE* self, void*, BYTE* obj);
extern "C" BYTE* __fastcall Vec44_Back(BYTE* self, void*);
extern "C" void FUN_4aa180(); KING_ALIAS("_FUN_4aa180", "@Vec44_Back@8")
extern "C" void __fastcall Vec44_PushBack(BYTE* self, void*, const void* x);
extern "C" void FUN_4aa9f0(); KING_ALIAS("_FUN_4aa9f0", "_Sch_MakeBegin")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
extern "C" void** __fastcall Handle9_Init(void** self, int, void** v);

// d = s, keeping d's object when Str_Same(d, s)
static void Eva_AssignSame(SchValue* d, const SchValue* s)
{
    if (d->obj && !(d->flags & SCH_IMMEDIATE) && --F(int, d->obj, 4) == 0
        && !CallC<char>(FUN_409070, d, s) && d->obj)
        CallVirt(d->obj, 12, 1);
    CopyRef(d, s);
}

static void Eva_CheckObject(const SchValue* v)
{
    if (!v->obj)
        SchError(0);
    if (v->flags & SCH_IMMEDIATE)
        SchError(4);
}

// the arguments of the interpreter's top frame
static int Eva_ArgCount(BYTE* p)
{
    BYTE* top = F(BYTE*, p, 112);
    return (F(BYTE*, top, -24) - F(BYTE*, top, -28)) / 12;
}

// env.define(sym, value): both by value
static void Eva_Define(SchValue* env, BYTE* p, const SchValue* sym, const SchValue* val)
{
    SchValue v, s;
    CopyRef(&v, val);
    CallThis(FUN_454580, &s, sym);
    Env_Define(SchValue_Object(env, 0), 0, p, s.obj, s.m_4, s.flags, v.obj, v.m_4, v.flags);
}

extern "C" {

// [$L_4a77f0] closure call: binds the top frame's arguments to the parameter list (a symbol at its end takes the
// rest as a list) in a new environment and replaces the frame with one evaluating the body there
void __fastcall Closure_Apply(BYTE* self, void*, BYTE* p)
{
    SchValue env, cur, tmp;
    SchValue_MakeEnv(&env, 0, p);
    int i = 1;
    CopyRef(&cur, (SchValue*)(self + 32));
    while (cur.flags || cur.obj) {
        BYTE t = (BYTE)cur.flags;
        if (t == 2) {
            if (i >= Eva_ArgCount(p))
                break;
            SchValue sym;
            Handle9_Init((void**)&sym, 0, (void**)(cur.obj + 20));
            BYTE* arg = Vec12_At((BYTE**)(Vec44_Back(p + 108, 0) + 16), 0, i);
            Eva_Define(&env, p, &sym, (SchValue*)arg);
            ReleaseRef(&sym);
        } else if (t == 9) {
            SchValue sym, acc;
            Handle9_Init((void**)&sym, 0, (void**)&cur);
            CallThis(FUN_4bd4d0, &acc);
            int last = Eva_ArgCount(p) - 1;
            if (last != i - 1) {
                int off = last * 12, n = last - (i - 1);
                do {
                    SchValue pr;
                    SchValue_MakePairB(&pr, 0, p);
                    Eva_CheckObject(&pr);
                    Eva_AssignSame((SchValue*)(pr.obj + 32), &acc);
                    CallThis(FUN_40c100, &acc, &pr);
                    BYTE* e = F(BYTE*, F(BYTE*, p, 112), -28) + off;
                    Eva_CheckObject(&acc);
                    Eva_AssignSame((SchValue*)(acc.obj + 20), (SchValue*)e);
                    ReleaseRef(&pr);
                    off -= 12;
                } while (--n);
            }
            Eva_Define(&env, p, &sym, &acc);
            if (i >= Eva_ArgCount(p)) {
                tmp.obj = 0; tmp.m_4 = 0; tmp.flags = 0;
                CallThis(FUN_40c100, &cur, &tmp);
                ReleaseRef(&tmp);
            } else {
                CallThis(FUN_40c100, &cur, CallThis<SchValue*>(FUN_4bd4d0, &tmp));
                ReleaseRef(&tmp);
                i = Eva_ArgCount(p);
            }
            ReleaseRef(&acc);
            ReleaseRef(&sym);
            break;
        } else {
            Sch_Message0(p, 26);
        }
        i++;
        SchValue* next = CallC<SchValue*>(FUN_4090a0, &tmp, &cur);
        Eva_AssignSame(&cur, next);
        ReleaseRef(&tmp);
    }
    if (i >= Eva_ArgCount(p)) {
        if (cur.flags || cur.obj)
            Sch_Message0(p, 27);
    } else if (!cur.flags && !cur.obj)
        Sch_Message0(p, 28);

    CallThis(FUN_40c100, SchValue_Object(&env, 0) + 148, self + 44);
    BYTE* top = F(BYTE*, p, 112) - 44;
    F(BYTE*, p, 112) = top;
    CallVirt(top, 4, 0);
    BYTE node[44];
    Vec44_PushBack(p + 108, 0, SchNode4a7060_Construct(node, 0));
    SchListNode_Destroy(node, 0);
    CallThis(FUN_49c560, F(BYTE*, p, 112) - 12, &env);

    SchValue nil, rest;
    SchRef_Construct((BYTE*)&nil, 0, 0);
    bool one = CallC<char>(FUN_409070, CallC<SchValue*>(FUN_4090a0, &rest, self + 20), &nil) != 0;
    ReleaseRef(&rest);
    ReleaseRef(&nil);
    if (one) {
        CallThis(FUN_40c100, F(BYTE*, p, 112) - 40, CallC<SchValue*>(FUN_407f00, &tmp, self + 20));
    } else {
        SchValue b;
        SchValue_MakePairB(&tmp, 0, p);
        CallThis(FUN_40c100, SchValue_Object(&tmp, 0) + 32, self + 20);
        CallThis(FUN_40c100, CallThis<BYTE*>(FUN_4aa430, &tmp) + 20, CallC<SchValue*>(FUN_4aa9f0, &b, 0));
        ReleaseRef(&b);
        CallThis(FUN_40c100, F(BYTE*, p, 112) - 40, &tmp);
    }
    ReleaseRef(&tmp);
    ReleaseRef(&cur);
    ReleaseRef(&env);
}

}  // extern "C"
// ---------------------------------------------------------------------------
// define

// pops the top frame and passes v to the frame below
static void Eva_PopReturn(BYTE* p, SchValue* v)
{
    BYTE* top = F(BYTE*, p, 112) - 44;
    F(BYTE*, p, 112) = top;
    CallVirt(top, 4, 0);
    Sch4a5c40_Append(p, F(BYTE*, p, 112) - 44, (const DWORD*)v);
}

extern "C" {

// [$L_4a9090] define, one step on the top frame: (define (name . params) body...) makes the closure at once;
// (define name expr) first evaluates expr in a new frame (state 0), then binds its value (state 1)
void __fastcall Define_Step(void*, void*, BYTE* p)
{
    BYTE* frame = F(BYTE*, p, 112) - 44;
    int state = F(int, frame, 28);
    if (state == 0) {
        SchValue form;
        CopyRef(&form, (SchValue*)(F(BYTE*, frame, 4) + 20));
        if ((BYTE)form.flags == 2) {
            SchValue clo, tmp, sym, res;
            Sch_MakeClosure((DWORD*)&clo, 0, (DWORD)p);
            Eva_CheckObject(&clo);
            Eva_AssignSame((SchValue*)(clo.obj + 32), (SchValue*)(form.obj + 32));
            BYTE* body = (BYTE*)Sch_Deref2nd(0, (int*)(Vec44_Back(p + 108, 0) + 4)) + 32;
            SchValue bv;
            CopyRef(&bv, (SchValue*)body);
            SchValue* r = (SchValue*)SchValue_Return((DWORD*)&tmp, 0, 0, (DWORD)bv.obj, bv.m_4, bv.flags);
            Eva_CheckObject(&clo);
            Eva_AssignSame((SchValue*)(clo.obj + 20), r);
            ReleaseRef(&tmp);
            Eva_CheckObject(&clo);
            CallThis(FUN_40c100, clo.obj + 44, F(BYTE*, p, 112) - 12);
            Handle9_Init((void**)&sym, 0, (void**)(form.obj + 20));
            Eva_Define((SchValue*)(Vec44_Back(p + 108, 0) + 32), p, &sym, &clo);
            Sch_MakeUnspecified((DWORD*)&tmp);
            CopyRef(&res, &tmp);
            ReleaseRef(&tmp);
            Eva_PopReturn(p, &res);
            ReleaseRef(&res);
            ReleaseRef(&sym);
            ReleaseRef(&clo);
        } else {
            SchValue tmp;
            Sch4a5c40_Append(p, frame, (const DWORD*)(F(BYTE*, frame, 4) + 20));
            CallThis(FUN_40c100, frame + 4, CallC<SchValue*>(FUN_4090a0, &tmp, frame + 4));
            ReleaseRef(&tmp);
            F(int, frame, 28) = 1;
            BYTE node[44];
            Vec44_PushBack(p + 108, 0, SchNode4a7060_Construct(node, 0));
            SchListNode_Destroy(node, 0);
            BYTE* prev = F(BYTE*, p, 112) - 88;
            CallThis(FUN_40c100, F(BYTE*, p, 112) - 40, F(BYTE*, prev, 4) + 20);
            CallThis(FUN_49c560, F(BYTE*, p, 112) - 12, prev + 32);
        }
        ReleaseRef(&form);
    } else if (state == 1) {
        BYTE* args = F(BYTE*, frame, 16);
        SchValue val, sym, tmp, res;
        CallThis(FUN_454580, &val, args + 24);
        Handle9_Init((void**)&sym, 0, (void**)(args + 12));
        Eva_Define((SchValue*)(Vec44_Back(p + 108, 0) + 32), p, &sym, &val);
        Sch_MakeUnspecified((DWORD*)&tmp);
        CallThis(FUN_454580, &res, &tmp);
        ReleaseRef(&tmp);
        Eva_PopReturn(p, &res);
        ReleaseRef(&res);
        ReleaseRef(&sym);
        ReleaseRef(&val);
    }
}

}  // extern "C"

// ---------------------------------------------------------------------------
// The interpreter's destructor

extern "C" void __fastcall Vec44_Destroy(BYTE** v);
extern "C" void FUN_4af970(); KING_ALIAS("_FUN_4af970", "@Vec44_Destroy@4")    // vector<44 bytes>: destructor
extern "C" void __fastcall StrRefVec_Destroy(BYTE** v, void*);
extern "C" void __fastcall RingMember_Destruct(BYTE* self, void*);
extern "C" BYTE** __fastcall SchInputList_EraseRange(void*, void*, BYTE** out, BYTE* first, BYTE* last);

// a field set to the empty value, released without the immediate check
static void Eva_ClearField(SchValue* f)
{
    SchValue nul = {0, 0, 0, 0};
    if (f->obj && --F(int, f->obj, 4) == 0 && !CallC<char>(FUN_409070, f, &nul) && f->obj)
        CallVirt(f->obj, 12, 1);
    f->obj = 0;
    f->m_4 = 0;
    f->flags = 0;
}

extern "C" {

// [$L_4aee50] the interpreter's destructor: clears the ports and environments, the frames and values, deletes
// the objects of the ring at +16 and the list at +44 (kept alive by a list of references meanwhile)
void __fastcall Sch_Destruct(BYTE* self, void*)
{
    Eva_ClearField((SchValue*)(self + 72));
    Eva_ClearField((SchValue*)(self + 84));
    Eva_ClearField((SchValue*)(self + 96));
    Eva_ClearField((SchValue*)(self + 60));
    Eva_ClearField((SchValue*)(self + 48));
    Vec44_Erase((BYTE**)(self + 108), 0, F(BYTE*, self, 108), F(BYTE*, self, 112));
    Values_Erase((BYTE**)(self + 4224), 0, F(BYTE*, self, 4224), F(BYTE*, self, 4228));
    BYTE* list[4];
    SchList_Construct(list, 0, 0);
    BYTE* ring = self + 24;
    SchValue tmp;
    for (BYTE* n = F(BYTE*, self, 16); n != ring; n = F(BYTE*, n, 12)) {
        SchRef_Construct((BYTE*)&tmp, 0, n);
        SchList_PushBack(list, 0, (int)&tmp);
        ReleaseRef(&tmp);
    }
    BYTE** objs = (BYTE**)(self + 44);
    BYTE* it;
    BYTE* junk;
    for (SchList_Begin(objs, 0, &it); it != *objs; SchListIt_PostInc(&it, 0, &junk, 0))
        if (F(BYTE*, it, 8)) {
            SchRef_Construct((BYTE*)&tmp, 0, F(BYTE*, it, 8));
            SchList_PushBack(list, 0, (int)&tmp);
            ReleaseRef(&tmp);
        }
    for (BYTE* n = F(BYTE*, self, 16); n != ring; n = F(BYTE*, n, 12))
        CallVirt(n, 16);
    for (SchList_Begin(objs, 0, &it); it != *objs; SchListIt_PostInc(&it, 0, &junk, 0))
        if (F(BYTE*, it, 8))
            CallVirt(F(BYTE*, it, 8), 16);
    BYTE* first;
    BYTE* out;
    SchList_Begin(list, 0, &first);
    SchInputList_EraseRange(list, 0, &out, first, list[0]);
    SchList_Begin(objs, 0, &first);
    SchList_EraseRange((BYTE*)objs, 0, &out, first, *objs);
    if (F(BYTE*, self, 16) != ring)
        Sch_ErrorCode(5, 768);
    SchValueList_Destroy(list, 0);
    StrRefVec_Destroy((BYTE**)(self + 4224), 0);
    Vec44_Destroy((BYTE**)(self + 108));
    ReleaseRef((SchValue*)(self + 96));
    ReleaseRef((SchValue*)(self + 84));
    ReleaseRef((SchValue*)(self + 72));
    ReleaseRef((SchValue*)(self + 60));
    ReleaseRef((SchValue*)(self + 48));
    SchList_Destroy(objs, 0);
    RingMember_Destruct(ring, 0);
    RingMember_Destruct(self + 4, 0);
}

}  // extern "C"

// ---------------------------------------------------------------------------
// Evaluating the elements of an expression

extern "C" SchValue* __fastcall Env_Lookup(BYTE* e, void*, SchValue* out, BYTE* s0, int s1, DWORD s2, int* depth, int* index);
extern "C" void FUN_4a3790(); KING_ALIAS("_FUN_4a3790", "@Env_Lookup@32")    // environment: look up an identifier (out, id by value, &depth, &index)
extern "C" SchValue* __fastcall SchScope_Get(BYTE* self, void*, SchValue* out, int depth, int index, int* found);
extern "C" void FUN_4a5a60(); KING_ALIAS("_FUN_4a5a60", "_Sch_Message1")
extern "C" SchValue* __cdecl SchValue_Immediate(SchValue* out, int unused, int v, int v2);
extern "C" DWORD* __fastcall SchValue_CopyImmediate(DWORD* self, void*, const DWORD* v);
extern "C" void FUN_4aa320(); KING_ALIAS("_FUN_4aa320", "@SchValue_CopyImmediate@12")
extern "C" DWORD* __fastcall SchIdentifier_From(DWORD* self, void*, BYTE* o);
extern "C" void FUN_4aa3a0(); KING_ALIAS("_FUN_4aa3a0", "@SchIdentifier_From@12")
KING_VAR(char, s_unbound6710f8, 6710f8)

// pushes a frame evaluating expr in the environment of the frame below
static void Eva_PushFrame(BYTE* p, SchValue* expr)
{
    BYTE node[44];
    Vec44_PushBack(p + 108, 0, SchNode4a7060_Construct(node, 0));
    SchListNode_Destroy(node, 0);
    CallThis(FUN_40c100, F(BYTE*, p, 112) - 40, expr);
    CallThis(FUN_49c560, F(BYTE*, p, 112) - 12, F(BYTE*, p, 112) - 88 + 32);
}

extern "C" {

// [FUN_4a5db0] the next element of the top frame's expression: a variable (looked up, a symbol replaced by its
// resolved position) or a constant is passed to the frame's values; a pair gets a frame of its own. The frame
// then goes on in state next
void __cdecl Sch_EvalNext(BYTE* p, int next)
{
    BYTE* top = F(BYTE*, p, 112);
    SchValue e, c, out, tmp;
    CopyRef(&e, (SchValue*)(top - 40));
    CopyRef(&c, (SchValue*)(e.obj + 20));
    BYTE t = (BYTE)c.flags;
    if (t == 33) {
        SchValue* env = (SchValue*)(top - 12);
        int idx = -1;
        Eva_CheckObject(env);
        SchScope_Get(env->obj, 0, &out, (int)c.obj, c.m_4, &idx);
        if (idx == -1)
            Sch_Message1(p, 32, (int)&s_unbound6710f8);
        Sch4a5c40_Append(p, F(BYTE*, p, 112) - 44, (const DWORD*)&out);
        ReleaseRef(&out);
    } else if (t == 9) {
        int idx = -1, depth = 0;
        SchValue id;
        SchIdentifier_From((DWORD*)&id, 0, (BYTE*)(Sch_Deref2nd(0, (int*)&c)));
        Env_Lookup(SchValue_Object((const SchValue*)(Vec44_Back(p + 108, 0) + 32), 0), 0, &out, id.obj, id.m_4, id.flags, &depth, &idx);
        if (idx == -1) {
            Sch_Message1(p, 32, (int)(F(const char*, c.obj, 12)));
            tmp.obj = 0; tmp.m_4 = 0; tmp.flags = 0;
            Sch4a5c40_Append(p, F(BYTE*, p, 112) - 44, (const DWORD*)&tmp);
            ReleaseRef(&tmp);
        } else {
            Sch4a5c40_Append(p, F(BYTE*, p, 112) - 44, (const DWORD*)&out);
            SchValue imm, cached;
            SchValue_CopyImmediate((DWORD*)&cached, 0, (const DWORD*)(SchValue_Immediate(&imm, 0, depth, idx)));
            ReleaseRef(&imm);
            Eva_AssignSame((SchValue*)(F(BYTE*, F(BYTE*, p, 112), -40) + 20), &cached);
            ReleaseRef(&cached);
        }
        ReleaseRef(&out);
    } else if (t == 2) {
        BYTE* f4 = F(BYTE*, p, 112) - 40;
        Eva_AssignSame((SchValue*)f4, CallC<SchValue*>(FUN_4090a0, &tmp, f4));
        ReleaseRef(&tmp);
        F(int, F(BYTE*, p, 112), -16) = next;
        Eva_PushFrame(p, &c);
        ReleaseRef(&c);
        ReleaseRef(&e);
        return;
    } else {
        Sch4a5c40_Append(p, F(BYTE*, p, 112) - 44, (const DWORD*)&c);
    }
    BYTE* f4 = F(BYTE*, p, 112) - 40;
    CallThis(FUN_40c100, f4, CallC<SchValue*>(FUN_4090a0, &tmp, f4));
    ReleaseRef(&tmp);
    F(int, F(BYTE*, p, 112), -16) = next;
    ReleaseRef(&c);
    ReleaseRef(&e);
}

}  // extern "C"

// === evaluating a value at the top level (FUN_4a6af0)

KING_VAR(AsmLabel, s_ev64bf60, 64bf60)
KING_VAR(AsmLabel, s_ev6709d4, 6709d4)
extern "C" void FUN_4a7060(); KING_ALIAS("_FUN_4a7060", "@SchNode4a7060_Construct@8")
extern "C" void FUN_4ab570(); KING_ALIAS("_FUN_4ab570", "_Sch4ab570_Construct")
extern "C" void FUN_4aa460(); KING_ALIAS("_FUN_4aa460", "@Elems44_InsertAux@16")
extern "C" void FUN_4a7230(); KING_ALIAS("_FUN_4a7230", "@SchListNode_Destroy@8")
extern "C" void FUN_40c100(); KING_ALIAS("_FUN_40c100", "@Str_Assign@12")
extern "C" void FUN_4a28c0(); KING_ALIAS("_FUN_4a28c0", "_SchValue_MakePairB")
extern "C" DWORD* __cdecl Sch_MakeIdentifierA(DWORD* out, int unused, DWORD name);
extern "C" void FUN_4aa830(); KING_ALIAS("_FUN_4aa830", "_Sch_MakeIdentifierA")
extern "C" DWORD* __cdecl Sch_MakeIdentifierB(DWORD* out, int unused, DWORD name);
extern "C" void FUN_4aa910(); KING_ALIAS("_FUN_4aa910", "_Sch_MakeIdentifierB")
extern "C" void FUN_4aa430(); KING_ALIAS("_FUN_4aa430", "@SchValue_Object@8")
extern "C" void FUN_4a6880(); KING_ALIAS("_FUN_4a6880", "_Sch_EvalNested")
extern "C" BYTE* __fastcall Vec44_Erase(BYTE** self, void*, BYTE* first, BYTE* last);
extern "C" void FUN_4aa730(); KING_ALIAS("_FUN_4aa730", "_Sch_MakeUnspecified")
extern "C" void FUN_454580(); KING_ALIAS("_FUN_454580", "@Str_CopyConstruct@12")

// pushes a fresh frame record (44 bytes) on the interpreter's frame stack (+108) with environment +60
static void Ev_PushFrame(BYTE* p)
{
    BYTE node[44];
    BYTE* n = SchNode4a7060_Construct(node, 0);
    BYTE** v = (BYTE**)(p + 108);
    if (v[1] != v[2]) {
        Sch4ab570_Construct(v[1], n);
        v[1] += 44;
    } else
        Elems44_InsertAux(v, 0, v[1], n);
    SchListNode_Destroy(node, 0);
    CallThis(FUN_40c100, F(BYTE*, p, 112) - 12, p + 60);
}

static void Ev_ClearFrames(BYTE* p)
{
    BYTE** v = (BYTE**)(p + 108);
    Vec44_Erase(v, 0, v[0], v[1]);
}

static BYTE* Ev_Obj(SchValue* v)
{
    return SchValue_Object(v, 0);
}

extern "C" {

// [FUN_4a6af0] evaluates the value v at the top level of interpreter p: a fresh frame, the expression wrapped
// in a call to the top environment's handler (64bf60 / 6709d4), the result in out (unspecified when the
// evaluation was cut short, +120)
SchValue* __cdecl Sch_EvalTop(SchValue* out, BYTE* p, SchValue12 arg)
{
    SchValue* v = (SchValue*)&arg;
    F(int, p, 120) = 0;
    Ev_PushFrame(p);
    SchValue h1;
    SchValue_MakePairB(&h1, 0, p);
    Eva_CheckObject(&h1);
    Eva_AssignSame((SchValue*)(h1.obj + 20), v);
    SchValue h2;
    SchValue_MakePairB(&h2, 0, p);
    SchValue t;
    SchValue* id = (SchValue*)Sch_MakeIdentifierA((DWORD*)&t, 0, (DWORD)((const char*)s_ev64bf60));
    Eva_CheckObject(&h2);
    Eva_AssignSame((SchValue*)(h2.obj + 20), id);
    SchHandle_Release((void**)&t, 0);
    Eva_CheckObject(&h2);
    Eva_AssignSame((SchValue*)(h2.obj + 32), &h1);
    SchValue h3;
    SchValue_MakePairB(&h3, 0, p);
    CallThis(FUN_40c100, Ev_Obj(&h3) + 20, &h2);
    SchValue h4;
    SchValue_MakePairB(&h4, 0, p);
    SchValue12 name;
    Sch_MakeIdentifierB((DWORD*)&name, 0, (DWORD)((const char*)s_ev6709d4));
    int depth = 0, index = -1;
    SchValue* found = CallThis<SchValue*>(FUN_4a3790, Ev_Obj((SchValue*)(p + 60)), &t, name, &depth, &index);
    CallThis(FUN_40c100, Ev_Obj(&h4) + 20, found);
    SchHandle_Release((void**)&t, 0);
    CallThis(FUN_40c100, Ev_Obj(&h4) + 32, &h3);
    SchValue r;
    CallC(FUN_4a6880, &r, p, Sv_Copy(&h4));
    if (F(int, p, 120) == 0) {
        Ev_ClearFrames(p);
        Ev_PushFrame(p);
        CallC(FUN_4a6880, &t, p, Sv_Copy(&r));
        Ev_ClearFrames(p);
        CallThis(FUN_454580, out, &t);
    } else
        CallThis(FUN_454580, out, CallC<SchValue*>(FUN_4aa730, &t, 0));
    SchHandle_Release((void**)&t, 0);
    SchHandle_Release((void**)&r, 0);
    SchHandle_Release((void**)&h4, 0);
    SchHandle_Release((void**)&h3, 0);
    SchHandle_Release((void**)&h2, 0);
    SchHandle_Release((void**)&h1, 0);
    SchHandle_Release((void**)&arg, 0);
    return out;
}

} // extern "C"

// === the interpreter's run loop (FUN_4a63d0)

KING_VAR(AsmLabel, s_rl66ce4c, 66ce4c)
extern "C" void FUN_4a5910(); KING_ALIAS("_FUN_4a5910", "_Sch_Message0")
extern "C" void FUN_4a5a60(); KING_ALIAS("_FUN_4a5a60", "_Sch_Message1")
extern "C" void __cdecl Sch4a5c40_Append(BYTE* ctx, BYTE* o, const DWORD* h);
extern "C" void FUN_4a5c40(); KING_ALIAS("_FUN_4a5c40", "_Sch4a5c40_Append")
extern "C" void __cdecl Sch_EvalNext(BYTE* p, int next);
extern "C" void FUN_4a5db0(); KING_ALIAS("_FUN_4a5db0", "_Sch_EvalNext")
extern "C" const char* __fastcall SchValue_Name(const SchValue* self, void*);
extern "C" void FUN_4aa180(); KING_ALIAS("_FUN_4aa180", "@Vec44_Back@8")
extern "C" BYTE* __cdecl Elems44_Copy(const BYTE* first, const BYTE* last, BYTE* dest);

// a value whose obj, m_4 and flags are all zero
static bool Rl_Empty(const SchValue* v)
{
    return !v->obj && !v->flags && !v->m_4;
}

// the value at frame+4 (12 bytes)
static SchValue* Rl_Value(BYTE* frame)
{
    return (SchValue*)(frame + 4);
}

// pops the top frame and hands value r to the frame below (FUN_4a5c40)
static void Rl_Return(BYTE* p, SchValue* r)
{
    BYTE** fr = (BYTE**)(p + 108);
    fr[1] -= 44;
    CallVirt(fr[1], 4, 0);
    Sch4a5c40_Append(p, F(BYTE*, p, 112) - 44, (const DWORD*)r);
}

static int Rl_Frames(BYTE* p)
{
    return (int)(F(BYTE*, p, 112) - F(BYTE*, p, 108)) / 44;
}

extern "C" {

// [FUN_4a63d0] runs the interpreter p until its frame stack is down to one frame with nothing to evaluate:
// continuations (type 34) are resumed, symbols (type 9) looked up in the frame's environment, other values
// returned to the frame below; an abort (+120) clears the stack
void __cdecl Sch_Run(BYTE* p)
{
    BYTE** fr = (BYTE**)(p + 108);
    for (;;) {
        if (fr[0] == fr[1])
            Sch_ErrorCode(12, 1);
        if (F(int, p, 120)) {
            BYTE* end = F(BYTE*, p, 112);
            BYTE* begin = fr[0];
            BYTE* to = Elems44_Copy(end, fr[1], begin);
            for (BYTE* q = to; q != fr[1]; q += 44)
                CallVirt(q, 4, 0);
            fr[1] -= (int)(end - begin) / 44 * 44;
            return;
        }
        BYTE* top = F(BYTE*, p, 112) - 44;
        SchValue* val = Rl_Value(top);
        BYTE** cont = (BYTE**)(top + 16);
        if (Rl_Empty(val) && Rl_Frames(p) < 2)
            return;
        if (cont[0] != cont[1] && F(BYTE, cont[0], 8) == 34) {
            CallVirt(*(BYTE**)cont[0], 8, p);
            continue;
        }
        if (Rl_Empty(val)) {
            if (Rl_Frames(p) < 2)
                return;
            if (cont[0] == cont[1]) {
                SchValue r = {0, 0, 0, 0};
                Rl_Return(p, &r);
                SchValue_Release(&r);
                continue;
            }
            SchValue h;
            CopyRef(&h, (SchValue*)cont[0]);
            if (Rl_Empty(&h))
                Sch_Message0(p, 31);
            else {
                if (h.flags & SCH_IMMEDIATE)
                    CallC(FUN_4a5a60, p, 29, SchValue_Name((const SchValue*)(F(BYTE*, F(BYTE*, p, 112), -28)), 0));
                CallVirt(h.obj, 8, p);
            }
            SchValue_Release(&h);
            continue;
        }
        if (F(BYTE, top, 12) == 2) {
            Sch_EvalNext(p, 0);
            continue;
        }
        SchValue r = {0, 0, 0, 0};
        if (F(BYTE, top, 12) == 9) {
            SchValue sym = {0, 0, 0, 0};
            if (val->obj) {
                if (CallVirt<int>(val->obj, 0) != 9)
                    CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(val->obj, 4, (const char*)s_rl66ce4c));
                else {
                    sym.obj = val->obj;
                    sym.flags = val->flags;
                    F(int, sym.obj, 4)++;
                }
            }
            int index = -1, depth = 0;
            SchValue found;
            BYTE* env = Ev_Obj((SchValue*)(Vec44_Back((BYTE*)fr, 0) + 32));
            CallThis(FUN_40c100, &r, CallThis<SchValue*>(FUN_4a3790, env, &found, Sv_Copy(&sym), &depth, &index));
            SchValue_Release(&found);
            if (index == -1) {
                Eva_CheckObject(&sym);
                Sch_Message1(p, 32, (int)(F(const char*, sym.obj, 12)));
                SchValue z = {0, 0, 0, 0};
                CallThis(FUN_40c100, &r, &z);
                SchHandle_Release((void**)&z, 0);
            }
            SchHandle_Release((void**)&sym, 0);
        } else
            CallThis(FUN_40c100, &r, val);
        Rl_Return(p, &r);
        SchHandle_Release((void**)&r, 0);
    }
}

} // extern "C"

// === set! ($L_4a8c20)

extern "C" void __fastcall Env_Set(BYTE* e, void*, BYTE* s0, int s1, DWORD s2, BYTE* v0, int v1, DWORD v2, int* found);
extern "C" void FUN_4a3ac0(); KING_ALIAS("_FUN_4a3ac0", "@Env_Set@36")

extern "C" {

// [$L_4a8c20] set!, one step on the top frame: state 0 evaluates the value expression in a new frame, state 1
// assigns the value to the variable in the environment (an unbound name is reported) and returns unspecified
void __fastcall Set_Step(void*, void*, BYTE* p)
{
    BYTE* frame = F(BYTE*, p, 112) - 44;
    int state = F(int, frame, 28);
    if (state == 0) {
        SchValue tmp;
        Sch4a5c40_Append(p, frame, (const DWORD*)(F(BYTE*, frame, 4) + 20));
        Eva_AssignSame((SchValue*)(frame + 4), CallC<SchValue*>(FUN_4090a0, &tmp, frame + 4));
        ReleaseRef(&tmp);
        F(int, frame, 28) = 1;
        BYTE node[44];
        Vec44_PushBack(p + 108, 0, SchNode4a7060_Construct(node, 0));
        SchListNode_Destroy(node, 0);
        BYTE* prev = F(BYTE*, p, 112) - 88;
        Eva_AssignSame((SchValue*)(F(BYTE*, p, 112) - 40), (SchValue*)(F(BYTE*, prev, 4) + 20));
        CallThis(FUN_40c100, F(BYTE*, p, 112) - 12, prev + 32);
    } else if (state == 1) {
        BYTE* args = F(BYTE*, frame, 16);
        SchValue val, sym = {0, 0, 0, 0}, tmp, res;
        CopyRef(&val, (SchValue*)(args + 24));
        BYTE* o = F(BYTE*, args, 12);
        if (o) {
            if (CallVirt<int>(o, 0) != 9)
                CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(o, 4, (const char*)s_tIdentifier));
            else {
                sym.obj = o;
                sym.flags = F(DWORD, args, 20);
                F(int, o, 4)++;
            }
        }
        int found = -1;
        SchValue v2, s2;
        CallThis(FUN_454580, &v2, &val);
        CallThis(FUN_454580, &s2, &sym);
        BYTE* env = SchValue_Object((const SchValue*)(Vec44_Back(p + 108, 0) + 32), 0);
        Env_Set(env, 0, s2.obj, s2.m_4, s2.flags, v2.obj, v2.m_4, v2.flags, &found);
        if (found == -1)
            Sch_Message1(p, 32, (int)(F(const char*, SchValue_Object(&sym, 0), 12)));
        Sch_MakeUnspecified((DWORD*)&tmp);
        CallThis(FUN_454580, &res, &tmp);
        ReleaseRef(&tmp);
        Eva_PopReturn(p, &res);
        ReleaseRef(&res);
        ReleaseRef(&sym);
        ReleaseRef(&val);
    }
}

} // extern "C"

// === assigning a variable in an environment (FUN_4a3ac0)

extern "C" BYTE** __fastcall NameMap_LowerBound(BYTE** self, void*, BYTE** out, const SchValue* key);

static const char* Es_Name(const SchValue* v)
{
    Eva_CheckObject(v);
    return F(const char*, v->obj, 12);
}

extern "C" {

// [FUN_4a3ac0] sets the variable sym to val (both by value) in environment e or the nearest parent (+148)
// defining it; *found gets its index there, or stays -1
void __fastcall Env_Set(BYTE* e, void*, BYTE* s0, int s1, DWORD s2, BYTE* v0, int v1, DWORD v2, int* found)
{
    SchValue sym = {s0, s1, s2, 0}, val = {v0, v1, v2, 0};
    *found = -1;
    BYTE** vals = (BYTE**)(e + 132);
    if (BYTE* map = F(BYTE*, e, 144)) {
        BYTE* it;
        NameMap_LowerBound((BYTE**)map, 0, &it, &sym);
        if (it != F(BYTE*, map, 0)) {
            *found = F(int, it, 40);
            Eva_AssignSame((SchValue*)(it + 28), &val);
            Eva_AssignSame((SchValue*)(vals[0] + F(int, it, 40) * 12), &val);
            ReleaseRef(&sym);
            ReleaseRef(&val);
            return;
        }
    } else {
        for (int i = 0; i < F(int, e, 20); i++) {
            const char* a = Es_Name((SchValue*)(e + 36 + i * 12));
            const char* b = Es_Name(&sym);
            if (tolower(b[0]) != tolower(a[0]))
                continue;
            b = Es_Name(&sym);
            a = Es_Name((SchValue*)(e + 36 + i * 12));
            if (_stricmp(a + 1, b + 1) == 0) {
                *found = i;
                CallThis(FUN_40c100, vals[0] + i * 12, &val);
                ReleaseRef(&sym);
                ReleaseRef(&val);
                return;
            }
        }
    }
    if (F(int, e, 156) || F(int, e, 148)) {
        SchValue s, v;
        CallThis(FUN_454580, &v, &val);
        CallThis(FUN_454580, &s, &sym);
        BYTE* up = (BYTE*)Sch_Deref2nd(0, (int*)(e + 148));
        Env_Set(up, 0, s.obj, s.m_4, s.flags, v.obj, v.m_4, v.flags, found);
    }
    ReleaseRef(&sym);
    ReleaseRef(&val);
}

} // extern "C"

// === call/cc ($L_4a9ac0)

// d = s for vectors of 44-byte frames (begin, end, capacity)
static void Cc_AssignFrames(BYTE** d, BYTE* const* s)
{
    int n = (int)(s[1] - s[0]) / 44;
    int size = (int)(d[1] - d[0]) / 44;
    if (n > (int)(d[2] - d[0]) / 44) {
        BYTE* p = Vec44_AllocCopy(n, s[0], s[1]);
        for (BYTE* e = d[0]; e != d[1]; e += 44)
            CallVirt(e, 4, 0);
        if (int cap = (int)(d[2] - d[0]) / 44)
            AiFree(d[0], cap * 44);
        d[0] = p;
        d[2] = p + n * 44;
    } else if (n <= size) {
        BYTE* to = Elems44_Copy(s[0], s[1], d[0]);
        for (BYTE* e = to; e != d[1]; e += 44)
            CallVirt(e, 4, 0);
    } else {
        Elems44_Copy(s[0], s[0] + size * 44, d[0]);
        Items44_UninitCopy(s[0] + size * 44, s[1], d[1], 0);
    }
    d[1] = d[0] + n * 44;
}

extern "C" {

// [$L_4a9ac0] call/cc, one step on the top frame: state 0 evaluates the argument; state 1 makes a
// continuation holding the frame stack (without this frame) and calls the procedure with it
void __fastcall CallCC_Step(void*, void*, BYTE* p)
{
    BYTE* top = F(BYTE*, p, 112);
    if (F(int, top, -16) == 0) {
        Sch_EvalNext(p, 1);
        return;
    }
    if (F(int, top, -16) != 1)
        return;
    SchValue args, cont, call;
    SchValue_MakePairB(&args, 0, p);
    Sch_MakeContinuation((DWORD*)&cont, 0, (DWORD)p);
    Eva_CheckObject(&cont);
    if ((BYTE*)(p + 108) != cont.obj + 20)
        Cc_AssignFrames((BYTE**)(cont.obj + 20), (BYTE* const*)(p + 108));
    Eva_CheckObject(&cont);
    F(BYTE*, cont.obj, 24) -= 44;
    CallVirt(F(BYTE*, cont.obj, 24), 4, 0);
    Eva_CheckObject(&args);
    Eva_AssignSame((SchValue*)(args.obj + 20), &cont);
    SchValue_MakePairB(&call, 0, p);
    CallThis(FUN_40c100, SchValue_Object(&call, 0) + 20, F(BYTE*, F(BYTE*, p, 112), -28) + 12);
    CallThis(FUN_40c100, SchValue_Object(&call, 0) + 32, &args);
    SchValue c;
    CallThis(FUN_454580, &c, &call);
    Sch4a70b0_Reset(Vec44_Back(p + 108, 0), 0, c.obj, c.m_4, c.flags);
    ReleaseRef(&call);
    ReleaseRef(&cont);
    ReleaseRef(&args);
}

} // extern "C"

// === if ($L_4a8740)

extern "C" SchValue* __fastcall SchValue_AsBool(SchValue* self, void*, const SchValue* src);
extern "C" void FUN_4a1940(); KING_ALIAS("_FUN_4a1940", "@SchValue_AsBool@12")

// the frame on top of p continues with the expression at e (a value at +20 of a pair)
static void If_Continue(BYTE* p, const SchValue* e)
{
    SchValue c;
    CopyRef(&c, e);
    Sch4a70b0_Reset(Vec44_Back(p + 108, 0), 0, c.obj, c.m_4, c.flags);
}

// returns unspecified from the top frame
static void If_ReturnUnspecified(BYTE* p)
{
    SchValue tmp, r;
    Sch_MakeUnspecified((DWORD*)&tmp);
    CopyRef(&r, &tmp);
    ReleaseRef(&tmp);
    Eva_PopReturn(p, &r);
    ReleaseRef(&r);
}

extern "C" {

// [$L_4a8740] if, one step on the top frame: state 0 evaluates the condition, state 1 goes on with the then or
// (for #f) the else expression, or returns unspecified without one
void __fastcall If_Step(void*, void*, BYTE* p)
{
    BYTE* top = F(BYTE*, p, 112);
    if (F(int, top, -16) == 0) {
        Sch_EvalNext(p, 1);
        return;
    }
    if (F(int, top, -16) != 1)
        return;
    SchValue v;
    CopyRef(&v, (SchValue*)(F(BYTE*, top, -28) + 12));
    int truth = -1;
    if ((BYTE)v.flags == 7) {
        SchValue h;
        SchValue_AsBool(&h, 0, &v);
        truth = (int)h.obj;
        SchHandle_Release((void**)&h, 0);
    }
    top = F(BYTE*, p, 112);
    if (F(BYTE, top, -32) != 2)
        If_ReturnUnspecified(p);
    else if (truth) {
        BYTE* e = (BYTE*)Sch_Deref2nd(0, (int*)(Vec44_Back(p + 108, 0) + 4));
        If_Continue(p, (SchValue*)(e + 20));
    } else if (F(BYTE, F(BYTE*, top, -40), 40) == 2) {
        BYTE* e = (BYTE*)Sch_Deref2nd(0, (int*)(Vec44_Back(p + 108, 0) + 4));
        e = (BYTE*)Sch_Deref2nd(0, (int*)(e + 32));
        If_Continue(p, (SchValue*)(e + 20));
    } else
        If_ReturnUnspecified(p);
    SchHandle_Release((void**)&v, 0);
}

} // extern "C"

// === looking up a variable (FUN_4a3790)

extern "C" {

// [FUN_4a3790] out = the value of sym (by value) in environment e or its parents (each step up counted in
// *depth); *index its slot there or -1, out empty when unbound
SchValue* __fastcall Env_Lookup(BYTE* e, void*, SchValue* out, BYTE* s0, int s1, DWORD s2, int* depth, int* index)
{
    SchValue sym = {s0, s1, s2, 0};
    *index = -1;
    BYTE** vals = (BYTE**)(e + 132);
    if (BYTE* map = F(BYTE*, e, 144)) {
        BYTE* it;
        NameMap_LowerBound((BYTE**)map, 0, &it, &sym);
        if (it != F(BYTE*, map, 0)) {
            *index = F(int, it, 40);
            CopyRef(out, (SchValue*)(it + 28));
            ReleaseRef(&sym);
            return out;
        }
    } else {
        for (int i = 0; i < F(int, e, 20); i++) {
            const char* a = Es_Name((SchValue*)(e + 36 + i * 12));
            const char* b = Es_Name(&sym);
            if (tolower(b[0]) != tolower(a[0]))
                continue;
            b = Es_Name(&sym);
            a = Es_Name((SchValue*)(e + 36 + i * 12));
            if (_stricmp(a + 1, b + 1) == 0) {
                *index = i;
                CopyRef(out, (SchValue*)(vals[0] + i * 12));
                ReleaseRef(&sym);
                return out;
            }
        }
    }
    if (F(int, e, 156) || F(int, e, 148)) {
        ++*depth;
        SchValue s;
        CallThis(FUN_454580, &s, &sym);
        BYTE* up = (BYTE*)Sch_Deref2nd(0, (int*)(e + 148));
        Env_Lookup(up, 0, out, s.obj, s.m_4, s.flags, depth, index);
        ReleaseRef(&sym);
        return out;
    }
    out->obj = 0;
    out->m_4 = 0;
    out->flags = 0;
    ReleaseRef(&sym);
    return out;
}

} // extern "C"

// === evaluating a lambda (the frame step $L_4a8220)

extern "C" {

// [$L_4a8220] lambda frame step of interpreter p (vtable 64bf80): unless the top frame (+112) is done (-16), a new
// closure gets the frame's name (-40, +20) at +32 and the value of the current expression at +20, the frame's
// result (-12) is set to it, the expression frame (+108) is popped and the closure passed to the frame below
void __fastcall Lambda_Step(BYTE* self, void*, BYTE* p)
{
    BYTE* top = F(BYTE*, p, 112);
    if (F(int, top, -16))
        return;
    SchValue c;
    Sch_MakeClosure((DWORD*)&c, 0, (DWORD)p);
    BYTE* f = F(BYTE*, top, -40);
    Eva_CheckObject(&c);
    Eva_AssignSame((SchValue*)(c.obj + 32), (SchValue*)(f + 20));
    BYTE* back = Vec44_Back(p + 108, 0);
    SchValue v;
    CopyRef(&v, (SchValue*)(Sch_Deref2nd(0, (int*)(back + 4)) + 32));
    SchValue r;
    BYTE* e = (BYTE*)SchValue_Return((DWORD*)&r, (int)p, (int)(Vec44_Back(p + 108, 0) + 32),
                                     (DWORD)v.obj, v.m_4, v.flags);
    Eva_CheckObject(&c);
    Eva_AssignSame((SchValue*)(c.obj + 20), (SchValue*)e);
    ReleaseRef(&r);
    CallThis(FUN_49c560, SchValue_Object(&c, 0) + 44, F(BYTE*, p, 112) - 12);
    BYTE** frames = (BYTE**)(p + 108);
    frames[1] -= 44;
    CallVirt(frames[1], 4, 0);
    Sch4a5c40_Append(p, F(BYTE*, p, 112) - 44, (const DWORD*)&c);
    SchHandle_Release((void**)&c, 0);
}

} // extern "C"

// === the eval frame step (the frame step $L_4a97c0)

extern "C" {

// [$L_4a97c0] eval frame step of interpreter p: evaluates the expression (-16 = 0) and the environment (1), then
// replaces the frame by a node frame for the expression (the frame's -28 node +12) in that environment (+24),
// which must be an environment object (kind 12)
void __fastcall Eval_Step(BYTE* self, void*, BYTE* p)
{
    BYTE* top = F(BYTE*, p, 112);
    if (F(int, top, -16) == 0) {
        Sch_EvalNext(p, 1);
        return;
    }
    if (F(int, top, -16) == 1) {
        Sch_EvalNext(p, 2);
        return;
    }
    BYTE* f = F(BYTE*, top, -28);
    SchValue a, b;
    CopyRef(&a, (SchValue*)(f + 12));
    CopyRef(&b, (SchValue*)(f + 24));
    BYTE** frames = (BYTE**)(p + 108);
    frames[1] = F(BYTE*, p, 112) - 44;
    CallVirt(frames[1], 4, 0);
    BYTE n[44];
    Vec44_PushBack((BYTE*)frames, 0, SchNode4a7060_Construct(n, 0));
    SchListNode_Destroy(n, 0);
    top = F(BYTE*, p, 112);
    SchValue* d = (SchValue*)(top - 40);
    if (d->obj && !(d->flags & SCH_IMMEDIATE) && --F(int, d->obj, 4) == 0 &&
        !(d->obj == a.obj && d->flags == a.flags && d->m_4 == a.m_4))
        CallVirt(d->obj, 12, 1);
    CopyRef(d, &a);
    SchValue* e = (SchValue*)(top - 12);
    if (e->obj && --F(int, e->obj, 4) == 0 && !CallC<char>(FUN_409070, e, &b) && e->obj)
        CallVirt(e->obj, 12, 1);
    if (!b.obj) {
        e->obj = 0;
        e->flags = 0;
        e->m_4 = 0;
    } else if (CallVirt<int>(b.obj, 0) != 12)
        Sch_Error(2, 1, CallVirt<const char*>(b.obj, 4), (const char*)g_inputEnvironmentName);
    else {
        e->obj = b.obj;
        e->flags = b.flags;
        F(int, b.obj, 4)++;
        e->m_4 = 0;
    }
    SchHandle_Release((void**)&b, 0);
    SchHandle_Release((void**)&a, 0);
}

} // extern "C"

// === spreading a list into the arguments (the frame step $L_4a9ed0)

extern "C" void FUN_407f00(); KING_ALIAS("_FUN_407f00", "_Str_Member20")      // a pair's car (out, value)
extern "C" void FUN_4090a0(); KING_ALIAS("_FUN_4090a0", "_Str_Member32")      // a pair's cdr (out, value)

extern "C" {

// [$L_4a9ed0] apply frame step of interpreter p: once the operator is evaluated (-40), the last argument (a list)
// is taken off the argument values (-28; the first, the operator's slot, dropped too) and its elements appended
// (message 25 for an improper list)
void __fastcall Apply_SpreadStep(BYTE* self, void*, BYTE* p)
{
    BYTE* top = F(BYTE*, p, 112);
    if (F(int, top, -32) || F(int, top, -40)) {
        Sch_EvalNext(p, 0);
        return;
    }
    SchValue v;
    CopyRef(&v, (SchValue*)(F(BYTE*, top, -24) - 12));
    F(BYTE*, top, -24) -= 12;
    SchHandle_Release((void**)(F(BYTE*, top, -24)), 0);
    top = F(BYTE*, p, 112);
    BYTE** a = (BYTE**)(top - 28);
    if (a[0] + 12 != a[1])
        Values_Copy(a[0] + 12, a[1], a[0]);
    a[1] -= 12;
    MultValue_DeletingDestructor(a[1], 0, 0);
    while (v.flags || v.obj) {
        if ((BYTE)v.flags != 2) {
            Sch_Message0(p, 25);
            break;
        }
        SchValue car, cdr;
        CallC(FUN_407f00, &car, &v);
        BYTE** w = (BYTE**)(F(BYTE*, p, 112) - 28);
        if (w[1] != w[2]) {
            ConstructStrRef((DWORD*)w[1], (const DWORD*)&car);
            w[1] += 12;
        } else
            CallThis(FUN_44c710, w, w[1], &car);
        ReleaseRef(&car);
        SchValue* c = CallC<SchValue*>(FUN_4090a0, &cdr, &v);
        Eva_AssignSame(&v, c);
        ReleaseRef(&cdr);
    }
    SchHandle_Release((void**)&v, 0);
}

} // extern "C"

// === evaluating a value in a nested run (FUN_4a6880)

extern "C" {

// [FUN_4a6880] out = the value of expression v (by value) evaluated by interpreter p in a new frame (in the
// environment of the frame below) run to its end: its first value, or the unspecified value without one
SchValue* __cdecl Sch_EvalNested(SchValue* out, BYTE* p, BYTE* v0, int v1, DWORD v2)
{
    SchValue v = {v0, v1, v2, 0};
    BYTE node[44];
    BYTE* n = SchNode4a7060_Construct(node, 0);
    BYTE** fr = (BYTE**)(p + 108);
    if (fr[1] != fr[2]) {
        Sch4ab570_Construct(fr[1], n);
        fr[1] += 44;
    } else
        Elems44_InsertAux(fr, 0, fr[1], n);
    SchListNode_Destroy(node, 0);
    BYTE* top = F(BYTE*, p, 112);
    Eva_AssignSame((SchValue*)(top - 40), &v);
    CallThis(FUN_40c100, F(BYTE*, p, 112) - 12, fr[0] + (fr[1] - fr[0]) / 44 * 44 - 56);
    Sch_Run(p);
    top = F(BYTE*, p, 112);
    if (fr[0] != fr[1] && F(BYTE*, top, -28) != F(BYTE*, top, -24))
        CopyRef(out, F(SchValue*, top, -28));
    else {
        SchValue t;
        Sch_MakeUnspecified((DWORD*)&t);
        CopyRef(out, &t);
        SchHandle_Release((void**)&t, 0);
    }
    ReleaseRef(&v);
    return out;
}

} // extern "C"

// === resuming a continuation ($L_4a7590)

extern "C" {

// [$L_4a7590] continuation c applied by interpreter p: the frame stack replaced by the saved one (+20) and the
// argument (the top frame's node +12) returned to the frame below
void __fastcall Continuation_Resume(BYTE* c, void*, BYTE* p)
{
    SchValue v;
    CopyRef(&v, (SchValue*)(F(BYTE*, F(BYTE*, p, 112), -28) + 12));
    BYTE** s = (BYTE**)(c + 20);
    BYTE** d = (BYTE**)(p + 108);
    if (s != d) {
        int n = (int)(s[1] - s[0]) / 44;
        int size = (int)(d[1] - d[0]) / 44;
        if ((unsigned)n > (unsigned)((d[2] - d[0]) / 44)) {
            BYTE* q = Vec44_AllocCopy(n, s[0], s[1]);
            for (BYTE* e = d[0]; e != d[1]; e += 44)
                CallVirt(e, 4, 0);
            if (int cap = (int)(d[2] - d[0]) / 44)
                AiFree(d[0], cap * 44);
            d[0] = q;
            d[2] = q + n * 44;
        } else if ((unsigned)size >= (unsigned)n) {
            BYTE* to = Elems44_CopyB(s[0], s[1], d[0]);
            for (BYTE* e = to; e != d[1]; e += 44)
                CallVirt(e, 4, 0);
        } else {
            Elems44_CopyB(s[0], s[0] + size * 44, d[0]);
            Items44_UninitCopy(s[0] + size * 44, s[1], d[1], 0);
        }
        d[1] = d[0] + n * 44;
    }
    Sch4a5c40_Append(p, F(BYTE*, p, 112) - 44, (const DWORD*)&v);
    ReleaseRef(&v);
}

} // extern "C"

// === dumping a frame (FUN_4a55c0)

extern "C" void FUN_4b8070(); KING_ALIAS("_FUN_4b8070", "_Sch4b8070")     // writes a value (out, ctx, value, port)
extern "C" void FUN_4a16c0(); KING_ALIAS("_FUN_4a16c0", "@Vec12_At@12")
KING_VAR(char, s_fd67109c, 67109c)
KING_VAR(char, s_fd671090, 671090)
KING_VAR(char, s_fd671084, 671084)
KING_VAR(char, s_fd67107c, 67107c)

// a counted copy of value v as three dwords
static void Fd_Copy(DWORD* d, const BYTE* v) { CallThis(FUN_454580, d, v); }

// writes value v of interpreter s to port p
static void Fd_Write(BYTE* s, const BYTE* v, const DWORD* p)
{
    DWORD a[3], b[3], t[3];
    Fd_Copy(a, (const BYTE*)p);
    Fd_Copy(b, v);
    CallC(FUN_4b8070, t, s, b[0], b[1], b[2], a[0], a[1], a[2]);
    ReleaseRef((SchValue*)t);
}

static void Fd_Str(const char* text, const DWORD* p)
{
    DWORD a[3];
    Fd_Copy(a, (const BYTE*)p);
    Port_WriteString(text, a[0], a[1], a[2]);
}

extern "C" {

// [FUN_4a55c0] writes frame e of interpreter s to port p (by value): its values (+16) and its expression (+4)
// between the 67109c / 671090 / 671084 / 67107c markers
void __cdecl Sch_DumpFrame(BYTE* s, BYTE* e, DWORD p0, DWORD p1, DWORD p2)
{
    DWORD p[3] = {p0, p1, p2};
    Fd_Str(&s_fd67109c, p);
    Fd_Str(&s_fd671090, p);
    BYTE** v = (BYTE**)(e + 16);
    for (unsigned i = 0; i < (unsigned)((v[1] - v[0]) / 12); i++) {
        DWORD a[3];
        Fd_Copy(a, (const BYTE*)p);
        BYTE* x = Vec12_At(v, 0, i);
        DWORD b[3], t[3];
        Fd_Copy(b, x);
        CallC(FUN_4b8070, t, s, b[0], b[1], b[2], a[0], a[1], a[2]);
        ReleaseRef((SchValue*)t);
    }
    Fd_Str(&s_fd671084, p);
    Fd_Write(s, e + 4, p);
    DWORD a[3];
    CopyRef((SchValue*)a, (const SchValue*)p);
    Port_WriteString(&s_fd67107c, a[0], a[1], a[2]);
    ReleaseRef((SchValue*)p);
}

} // extern "C"

// === clearing an environment (4a41a0)

extern "C" void __fastcall SchMap_Destroy(BYTE** self, void*);

extern "C" {

// [$L_4a41a0] environment s cleared: its name map (+144) deleted and its values (+132..+136) released and dropped
void __fastcall Env_Clear(BYTE* s)
{
    if (BYTE* m = F(BYTE*, s, 144)) {
        SchMap_Destroy((BYTE**)m, 0);
        operator delete(m);
    }
    F(BYTE*, s, 144) = 0;
    BYTE* b = F(BYTE*, s, 132);
    for (BYTE* p = b; p != F(BYTE*, s, 136); p += 12)
        SchHandle_Release((void**)p, 0);
    F(BYTE*, s, 136) = b;
}

} // extern "C"

// [FUN_4af970] destructor of a vector of 44-byte objects: each destroyed (vtbl+4 (0)), the storage freed
extern "C" void __fastcall Vec44_Destroy(BYTE** v)
{
    for (BYTE* e = v[0]; e != v[1]; e += 44)
        CallVirt(e, 4, 0);
    if (int n = (int)(v[2] - v[0]) / 44)
        Ai_PoolFree(v[0], n * 44);
}

// [$L_4a7360] clears a script list node (like SchListNode_Destroy, keeping it): the handle at +4, the values at
// +16 and the handle at +32 (assigned an empty value)
extern "C" void __fastcall SchListNode_Clear(BYTE* self)
{
    SchHandle_Release((void**)(self + 4), 0);
    F(int, self, 8) = 0;
    F(int, self, 4) = 0;
    F(int, self, 12) = 0;
    BYTE* b = F(BYTE*, self, 16);
    BYTE* e = F(BYTE*, self, 20);
    for (BYTE* p = Values_Copy(e, e, b); p != F(BYTE*, self, 20); p += 12)
        SchHandle_Release((void**)p, 0);
    F(BYTE*, self, 20) -= (e - b) / 12 * 12;
    SchValue empty = {};
    SchValue* d = (SchValue*)(self + 32);
    if (d->obj && --F(int, d->obj, 4) == 0 && !CallC<char>(FUN_409070, d, &empty) && d->obj)
        CallVirt(d->obj, 12, 1);
    d->obj = 0;
    d->flags = 0;
    d->m_4 = 0;
    SchHandle_Release((void**)&empty, 0);
}

// ---------------------------------------------------------------------------
// a native procedure applied ($L_4a80b0, a vtable entry)

extern "C" {

// [$L_4a80b0] native procedure f (its function +24) called with the top frame's arguments (the frame at +112 less
// 28); the top frame popped and the result passed to the frame below
void __fastcall Native_Apply(BYTE* f, void*, BYTE* p)
{
    typedef SchValue* (__cdecl * Fn)(SchValue*, BYTE*, BYTE*);
    SchValue t;
    SchValue* r = ((Fn)F(void*, f, 24))(&t, p, F(BYTE*, p, 112) - 28);
    SchValue v = {0, 0, 0, 0};
    if (r->flags & SCH_IMMEDIATE)
    {
        v.flags = r->flags;
        v.obj = r->obj;
        v.m_4 = r->m_4;
    }
    else if (r->obj)
    {
        v.obj = r->obj;
        v.flags = r->flags;
        F(int, v.obj, 4)++;
    }
    ReleaseRef(r);
    Eva_PopReturn(p, &v);
    ReleaseRef(&v);
}

}  // extern "C"

// ---------------------------------------------------------------------------
// string? ($L_49c610) and a begin-like frame's step ($L_4a85d0)

extern "C" int __cdecl Sch_SecondArg(BYTE* p, SchValue* out, BYTE** args, int where);
extern "C" DWORD* __cdecl SchValue_Make10007(DWORD* out, int unused, DWORD v);
extern "C" int __cdecl Sch_Deref2nd(void*, int* p);
void __fastcall Sch4a70b0_Reset(BYTE* self, void*, BYTE* h0, int h1, DWORD h2);
void __cdecl Sch_EvalNext(BYTE* p, int next);

extern "C" {

// [$L_49c610] (string? x): #t when the argument's type (its flags' low byte) is 3; a cleared value when the
// argument is missing
SchValue* __cdecl Nat_IsString(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (Sch_SecondArg(p, &v, args, (int)s_rn670890))
    {
        out->obj = 0;
        out->m_4 = 0;
        out->flags = 0;
    }
    else
        SchValue_Make10007((DWORD*)out, 0, (BYTE)v.flags == 3);
    ReleaseRef(&v);
    return out;
}

// [$L_4a85d0] interpreter p's top frame (+112, 44 bytes) with nothing left (-40, -36, -32 clear) popped; with a
// pending form (-40) that has more to do (+40 or +32) the next step; else the frame's result is the value of the
// last element of the context stack (+108)
void __fastcall Frame_StepSequence(BYTE* f, void*, BYTE* p)
{
    BYTE* top = F(BYTE*, p, 112);
    if (!F(int, top, -40) && !F(int, top, -32) && !F(int, top, -36))
    {
        F(BYTE*, p, 112) = top - 44;
        CallVirt(top - 44, 4, 0);
        return;
    }
    BYTE* e = F(BYTE*, top, -40);
    if (F(int, e, 40) || F(int, e, 32))
    {
        Sch_EvalNext(p, 0);
        return;
    }
    BYTE* back = Vec44_Back(p + 108, 0);
    BYTE* s = (BYTE*)Sch_Deref2nd(0, (int*)(back + 4)) + 20;
    SchValue v = {0, 0, 0, 0};
    DWORD fl = F(DWORD, s, 8);
    if (fl & SCH_IMMEDIATE)
    {
        v.flags = fl;
        v.obj = F(BYTE*, s, 0);
        v.m_4 = F(int, s, 4);
    }
    else if (F(BYTE*, s, 0))
    {
        v.obj = F(BYTE*, s, 0);
        v.flags = fl;
        F(int, v.obj, 4)++;
    }
    Sch4a70b0_Reset(Vec44_Back(p + 108, 0), 0, v.obj, v.m_4, v.flags);
}

}  // extern "C"

// === a 44-byte element inserted into its vector (FUN_4aa460)

extern "C" void Ia_StrCopy(); KING_ALIAS("_Ia_StrCopy", "@Str_CopyConstruct@12")
extern "C" void Ia_StrAssign(); KING_ALIAS("_Ia_StrAssign", "@Str_Assign@12")
extern "C" BYTE** __fastcall Values4aadc0_Copy(BYTE** self, void*, BYTE* const* src);
extern "C" BYTE** __fastcall Values_Assign(BYTE** d, void*, BYTE* const* s);
extern "C" BYTE* __fastcall Sch4aac80_Copy(BYTE* self, void*, const BYTE* src);
extern "C" BYTE* __cdecl Items44_UninitCopyB(const BYTE* first, const BYTE* last, BYTE* dest, void*);
extern "C" void FUN_4abba0(); KING_ALIAS("_FUN_4abba0", "_Items44_UninitCopyB")
KING_VAR(AsmLabel, g_ia64bf68, 64bf68)

extern "C" {

// [FUN_4aa460] vector v (begin, end, capacity) of 44-byte elements: x inserted at pos, the tail moved up by one
// in place, or everything copied into storage of twice the size (1 when empty) and the old freed
void __fastcall Elems44_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    if (v[1] != v[2])
    {
        if (v[1])
            Sch4aac80_Copy(v[1], 0, v[1] - 44);
        v[1] += 44;
        BYTE t[44];
        CallThis(Ia_StrCopy, t + 4, x + 4);
        Values4aadc0_Copy((BYTE**)(t + 16), 0, (BYTE* const*)(x + 16));
        F(DWORD, t, 28) = F(DWORD, x, 28);
        CallThis(Ia_StrCopy, t + 32, x + 32);
        F(void*, t, 0) = &g_ia64bf68;
        BYTE* e = v[1];
        Elems44_CopyBack(pos, e - 88, e - 44);
        CallThis(Ia_StrAssign, pos + 4, t + 4);
        Values_Assign((BYTE**)(pos + 16), 0, (BYTE* const*)(t + 16));
        F(DWORD, pos, 28) = F(DWORD, t, 28);
        CallThis(Ia_StrAssign, pos + 32, t + 32);
        SchListNode_Destroy(t, 0);
        return;
    }
    int n = (v[1] - v[0]) / 44;
    int len = n ? 2 * n : 1;
    BYTE* buf = len ? (BYTE*)Stl_Allocate(len * 44) : 0;
    BYTE* nf = Items44_UninitCopyB(v[0], pos, buf, pos);
    if (nf)
        Sch4aac80_Copy(nf, 0, x);
    BYTE* ne = Items44_UninitCopyB(pos, v[1], nf + 44, nf);
    for (BYTE* p = v[0]; p != v[1]; p += 44)
        CallVirt(p, 4, 0);
    int cap = (v[2] - v[0]) / 44;
    if (cap)
        AiFree(v[0], cap * 44);
    v[0] = buf;
    v[1] = ne;
    v[2] = buf + len * 44;
}

} // extern "C"

// ---------------------------------------------------------------------------
// Batch 33: uninitialized copies of script items

extern "C" void Si33_Reserve12(); KING_ALIAS("_Si33_Reserve12", "@Vec12_Reserve@16")
extern "C" void Si33_CopyStrRefs(); KING_ALIAS("_Si33_CopyStrRefs", "_UninitCopyStrRefs")
extern "C" void Si33_StrCopy(); KING_ALIAS("_Si33_StrCopy", "@Str_CopyConstruct@12")
KING_VAR(BYTE, si33_vtbl64bf68, 64bf68)

static BYTE* Si33_UninitCopy(const BYTE* first, const BYTE* last, BYTE* dest)
{
    BYTE* dest0 = dest;
    for (; first != last; first += 44, dest += 44) {
        if (!dest)
            continue;
        if (F(DWORD, first, 12) & 0x10000) {
            F(DWORD, dest, 12) = F(DWORD, first, 12);
            F(DWORD, dest, 4) = F(DWORD, first, 4);
            F(DWORD, dest, 8) = F(DWORD, first, 8);
        } else {
            F(DWORD, dest, 8) = 0;
            if (BYTE* o = F(BYTE*, first, 4)) {
                F(BYTE*, dest, 4) = o;
                F(DWORD, dest, 12) = F(DWORD, first, 12);
                F(int, o, 4)++;
            } else {
                F(DWORD, dest, 4) = 0;
                F(DWORD, dest, 12) = 0;
            }
        }
        BYTE alloc;
        CallThis(Si33_Reserve12, dest + 16, (int)(F(BYTE*, first, 20) - F(BYTE*, first, 16)) / 12, &alloc);
        F(BYTE*, dest, 20) = CallC<BYTE*>(Si33_CopyStrRefs, F(BYTE*, first, 16), F(BYTE*, first, 20),
                                          F(BYTE*, dest, 16), dest0);
        F(DWORD, dest, 28) = F(DWORD, first, 28);
        CallThis(Si33_StrCopy, dest + 32, first + 32);
        F(void*, dest, 0) = &si33_vtbl64bf68;
    }
    return dest;
}

extern "C" {

// [FUN_4abd20..$L_4abe43] uninitialized copy of 44-byte script items (head +4, string refs +16, value +28,
// string +32); returns the end of the copies
BYTE* __cdecl Items44_UninitCopy(const BYTE* first, const BYTE* last, BYTE* dest, void*)
{
    return Si33_UninitCopy(first, last, dest);
}

// [FUN_4abba0..$L_4abcc3] the same copy (a second instance)
BYTE* __cdecl Items44_UninitCopyB(const BYTE* first, const BYTE* last, BYTE* dest, void*)
{
    return Si33_UninitCopy(first, last, dest);
}

}

// [FUN_4a4950] the map's _M_copy (thiscall: source subtree x, new parent p):
// clones x (colour +0, parent +4, left +8, right +12) and copies right
// subtrees recursively while walking down the left side
extern "C" BYTE* __fastcall SchMap_CopySubtree(BYTE* tree, void*, const BYTE* x, BYTE* p)
{
    BYTE* top = SchMap_MakeNode(x + 16);
    F(BYTE, top, 0) = F(BYTE, x, 0);
    F(BYTE*, top, 8) = 0;
    F(BYTE*, top, 12) = 0;
    F(BYTE*, top, 4) = p;
    if (F(const BYTE*, x, 12))
        F(BYTE*, top, 12) = SchMap_CopySubtree(tree, 0, F(const BYTE*, x, 12), top);
    p = top;
    for (x = F(const BYTE*, x, 8); x; x = F(const BYTE*, x, 8)) {
        BYTE* y = SchMap_MakeNode(x + 16);
        F(BYTE, y, 0) = F(BYTE, x, 0);
        F(BYTE*, y, 8) = 0;
        F(BYTE*, y, 12) = 0;
        F(BYTE*, p, 8) = y;
        F(BYTE*, y, 4) = p;
        if (F(const BYTE*, x, 12))
            F(BYTE*, y, 12) = SchMap_CopySubtree(tree, 0, F(const BYTE*, x, 12), y);
        p = y;
    }
    return top;
}

// [FUN_4ac330] deque<char>: allocates the 512-byte pages of [first, last)
extern "C" void* __cdecl Ai_OperatorNew(unsigned n);
extern "C" void __fastcall Deque_AllocPages512(BYTE* self, void*, char** first, char** last)
{
    for (; first < last; first++) {
        void* p = malloc(512);
        if (!p)
            p = Ai_OperatorNew(512);
        *first = (char*)p;
    }
}

// [FUN_4ac0b0] deque<char>::insert (pos, first, last): at the front or the
// back after reserving room, elsewhere through insert_aux (4ac3d0)
extern "C" void __fastcall Deque_ReallocMap(int* d, void*, unsigned add, char front);

// [FUN_4ac930] deque<char>: pages for k more characters at the front
// (the map at +0, its size +4, the start node +20)
extern "C" void __fastcall Dq_ReserveFrontPages(BYTE* self, void*, unsigned k)
{
    unsigned n = (k + 511) >> 9;
    if (n > (unsigned)((F(BYTE*, self, 20) - F(BYTE*, self, 0)) >> 2))
        Deque_ReallocMap((int*)self, 0, n, 1);
    for (unsigned i = 1; i <= n; i++) {
        void* page = malloc(512);
        if (!page)
            page = Ai_OperatorNew(512);
        F(void**, self, 20)[-(int)i] = page;
    }
}

// [FUN_4aca10] the same at the back (the finish node +36)
extern "C" void __fastcall Dq_ReserveBackPages(BYTE* self, void*, unsigned k)
{
    unsigned n = (k + 511) >> 9;
    if (n + 1 > F(unsigned, self, 4) - (unsigned)((F(BYTE*, self, 36) - F(BYTE*, self, 0)) >> 2))
        Deque_ReallocMap((int*)self, 0, n, 0);
    for (unsigned i = 1; i <= n; i++) {
        void* page = malloc(512);
        if (!page)
            page = Ai_OperatorNew(512);
        F(void**, self, 36)[i] = page;
    }
}
extern "C" void __fastcall Deque_InsertAux(BYTE* self, void*, DequeIt pos, const char* first, const char* last, unsigned n);
extern "C" void __fastcall Deque_InsertRange(BYTE* self, void*, DequeIt pos, const char* first, const char* last)
{
    unsigned n = (unsigned)(last - first);
    DequeIt& start = *(DequeIt*)(self + 8);
    DequeIt& finish = *(DequeIt*)(self + 24);
    DequeIt junk;
    if (pos.cur == start.cur) {
        unsigned room = (unsigned)(start.cur - start.first);
        if (n > room)
            Dq_ReserveFrontPages(self, 0, n - room);
        DequeIt t = start;
        DequeIt ns = *DequeIt_Advance(&t, 0, -(int)n);
        Deque_CopyChars(&junk, first, last, ns);
        start = ns;
    } else if (pos.cur == finish.cur) {
        unsigned room = (unsigned)(finish.last - finish.cur - 1);
        if (n > room)
            Dq_ReserveBackPages(self, 0, n - room);
        DequeIt t = finish;
        DequeIt nf = *DequeIt_Advance(&t, 0, (int)n);
        Deque_CopyChars(&junk, first, last, finish);
        finish = nf;
    } else {
        Deque_InsertAux(self, 0, pos, first, last, n);
    }
}

// [FUN_4ace70] copy (first, last, deque iterator dest, tag) into out
extern "C" DequeIt* __cdecl Deque_CopyCharsTag(DequeIt* out, const char* first, const char* last, DequeIt dest, int)
{
    Deque_CopyChars4ad0f0(out, first, last, dest);
    return out;
}

// [$L_4a8ae0] quote's step (virtual): takes a copy of the quoted value from
// the frame (value at +20, flags +28 of the frame's object), pops the frame
// (44 bytes, its virtual +4 with 0), appends the value to the new top frame
// and drops the copy
extern "C" void __fastcall Quote_Step(BYTE* self, void*, BYTE* p)
{
    BYTE* v = F(BYTE*, F(BYTE*, p, 112), -40);
    DWORD h[3];
    DWORD fl = F(DWORD, v, 28);
    if (fl & 65536) {
        h[2] = fl;
        h[0] = F(DWORD, v, 20);
        h[1] = F(DWORD, v, 24);
    } else {
        h[1] = 0;
        BYTE* o = F(BYTE*, v, 20);
        if (o) {
            h[0] = (DWORD)o;
            h[2] = F(DWORD, v, 28);
            F(int, o, 4)++;
        } else {
            h[0] = 0;
            h[2] = 0;
        }
    }
    BYTE* fr = F(BYTE*, p, 112) - 44;
    F(BYTE*, p, 112) = fr;
    CallVirt(fr, 4, 0);
    Sch4a5c40_Append(p, F(BYTE*, p, 112) - 44, h);
    BYTE* o = (BYTE*)h[0];
    if (!(h[2] & 65536) && o) {
        F(int, o, 4)--;
        o = (BYTE*)h[0];
        if (!F(int, o, 4) && o)
            CallVirt(o, 12, 1);
    }
}

// [FUN_4ac3d0] deque<char>::_M_insert_aux (pos, first, last, n): the SGI
// algorithm, moving the shorter side (start +8, finish +24)
static DequeIt Dq_Plus(DequeIt it, int n) { return *DequeIt_Advance(&it, 0, n); }
extern "C" void __fastcall Deque_InsertAux(BYTE* self, void*, DequeIt pos, const char* first, const char* last, unsigned n)
{
    DequeIt& start = *(DequeIt*)(self + 8);
    DequeIt& finish = *(DequeIt*)(self + 24);
    DequeIt out;
    int before = Distance(start, pos);
    unsigned length = (unsigned)DequeIt_Diff(&finish, 0, &start);
    if ((unsigned)before < length >> 1) {
        unsigned room = (unsigned)(start.cur - start.first);
        if (n > room)
            Dq_ReserveFrontPages(self, 0, n - room);
        DequeIt newStart = Dq_Plus(start, -(int)n);
        DequeIt oldStart = start;
        pos = Dq_Plus(start, before);
        if (before >= (int)n) {
            DequeIt startN = Dq_Plus(start, (int)n);
            Deque_Copy4acb40(&out, start, startN, newStart);
            start = newStart;
            Deque_Copy4acb90(&out, startN, pos, oldStart);
            Deque_CopyChars4acbe0(&out, first, last, Dq_Plus(pos, -(int)n));
        } else {
            const char* mid = first + (n - before);
            Deque4acc20(&out, start, pos, (int)first, (int)mid, newStart);
            start = newStart;
            Deque_CopyChars4acbe0(&out, mid, last, oldStart);
        }
    } else {
        unsigned room = (unsigned)(finish.last - finish.cur - 1);
        if (n > room)
            Dq_ReserveBackPages(self, 0, n - room);
        DequeIt newFinish = Dq_Plus(finish, (int)n);
        DequeIt oldFinish = finish;
        int after = (int)length - before;
        pos = Dq_Plus(finish, -after);
        if (after > (int)n) {
            DequeIt finishN = Dq_Plus(finish, -(int)n);
            Deque_Copy4acb40(&out, finishN, finish, finish);
            finish = newFinish;
            Deque_CopyBackward(&out, pos, finishN, oldFinish);
            Deque_CopyChars4acbe0(&out, first, last, pos);
        } else {
            const char* mid = first + after;
            Deque4acd70(&out, (int)mid, (int)last, pos, finish, finish);
            finish = newFinish;
            Deque_CopyChars4acbe0(&out, first, mid, pos);
        }
    }
}
