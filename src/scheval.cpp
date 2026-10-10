// scheval.cpp - Sch_eval.cpp of the game, rewritten from king.masm: small
// functions of the AI's script evaluator: reference-counted values and the
// scopes holding them, the constructors of the expression nodes, the map and
// list helpers, pools of freed blocks, and the copies of the deque<char>
// holding script text.
//
// Building with  nmake ASM_SCHEVAL=1  makes king.masm assemble the original
// procedures again.

#include "king.h"

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
ASM_PROC(FUN_4b1450)    // shows a message (text, value by copy)
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

extern "C" BYTE* __fastcall MultValue_DeletingDestructor(BYTE* self, void*, int flags);   // aimult.cpp

// copies [first, last) of values over dest
extern "C" BYTE* __cdecl Values_Copy(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = (last - first) / 12; n > 0; n--, first += 12, dest += 12)
        SchValue_Assign((SchValue*)dest, 0, (const SchValue*)first);
    return dest;
}

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

// a value set from the second of two arguments; other counts are an error
extern "C" int __cdecl Sch_SecondArg(BYTE* p, SchValue* out, BYTE** args, int where)
{
    BYTE* a = args[0];
    if ((args[1] - a) / 12 != 2) {
        Sch_Message1(p, 33, where);
        return 1;
    }
    SchValue_Assign(out, 0, (const SchValue*)(a + 12));
    return 0;
}

// deque<...>::_Reallocate_map(n, at the front)
extern "C" void __fastcall Deque_ReallocMap(int* d, void*, unsigned add, char front)
{
    int** start = (int**)d[5];
    int** finish = (int**)d[9];
    int old = finish - start + 1;
    unsigned want = old + add;
    unsigned shift = front ? add : 0;
    int** nstart;
    if ((unsigned)d[1] > want * 2) {
        nstart = (int**)d[0] + (((unsigned)d[1] - want) >> 1) + shift;
        if (nstart < start) {
            int** to = nstart;
            for (int k = (int)(((BYTE*)finish - (BYTE*)start) + 4) >> 2; k > 0; k--)
                *to++ = *start++;
        } else {
            int** from = finish + 1;
            int** to = nstart + old;
            for (int k = (int)(((BYTE*)from - (BYTE*)start)) >> 2; k > 0; k--)
                *--to = *--from;
        }
    } else {
        unsigned big = (unsigned)d[1] < add ? add : (unsigned)d[1];
        unsigned size = d[1] + big + 2;
        int** map = size ? CallC<int**>(FUN_44b740, size * 4) : 0;
        nstart = map + ((size - want) >> 1) + shift;
        int** to = nstart;
        int** from = (int**)d[5];
        for (int k = (int)(((BYTE*)d[9] - (BYTE*)from) + 4) >> 2; k > 0; k--)
            *to++ = *from++;
        if (d[1])
            AiFree((void*)d[0], d[1] * 4);
        d[1] = size;
        d[0] = (int)map;
    }
    d[5] = (int)nstart;
    d[3] = (int)*nstart;
    d[4] = d[3] + 512;
    d[9] = (int)(nstart + old - 1);
    d[7] = (int)*(int**)d[9];
    d[8] = d[7] + 512;
}

// values set from the arguments after the first; other counts are an error
extern "C" int __cdecl Sch_Args3(BYTE* p, SchValue* a, SchValue* b, SchValue* c, BYTE** args, int where)
{
    if ((args[1] - args[0]) / 12 != 4) {
        Sch_Message1(p, 33, where);
        return 1;
    }
    SchValue_Assign(a, 0, (const SchValue*)(args[0] + 12));
    SchValue_Assign(b, 0, (const SchValue*)(args[0] + 24));
    SchValue_Assign(c, 0, (const SchValue*)(args[0] + 36));
    return 0;
}

extern "C" int __cdecl Sch_Args2(BYTE* p, SchValue* a, SchValue* b, BYTE** args, int where)
{
    if ((args[1] - args[0]) / 12 != 3) {
        Sch_Message1(p, 33, where);
        return 1;
    }
    SchValue_Assign(a, 0, (const SchValue*)(args[0] + 12));
    SchValue_Assign(b, 0, (const SchValue*)(args[0] + 24));
    return 0;
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

// make a String value from a char*                                      [FUN_4a1e30]
extern "C" SchValue* __cdecl SchValue_MakeString(SchValue* out, int, const char* s)
{
    BYTE* obj = (BYTE*)operator new(24);
    BYTE* p = obj ? CallThis<BYTE*>(FUN_4b61c0, obj, s) : 0;
    return SchValue_WrapNew(out, p, 3, "String");
}
KING_ALIAS("_SchValue_MakeString", "FUN_4a1e30")

// make a String value from a SchBuf (no args constructor)               [FUN_4a2400]
extern "C" SchValue* __cdecl SchValue_MakeStringB(SchValue* out, int)
{
    BYTE* obj = (BYTE*)operator new(24);
    BYTE* p = obj ? CallThis<BYTE*>(FUN_4b6160, obj) : 0;
    return SchValue_WrapNew(out, p, 3, "String");
}
KING_ALIAS("_SchValue_MakeStringB", "FUN_4a2400")

// make an Identifier value from a char*                                 [FUN_4a2230]
extern "C" SchValue* __cdecl SchValue_MakeIdent(SchValue* out, int, const char* s)
{
    BYTE* obj = (BYTE*)operator new(24);
    BYTE* p = obj ? CallThis<BYTE*>(FUN_4b6400, obj, s) : 0;
    return SchValue_WrapNew(out, p, 9, "Identifier");
}
KING_ALIAS("_SchValue_MakeIdent", "FUN_4a2230")

// make a Pair value (3 args to constructor)                             [FUN_4a2140]
extern "C" SchValue* __cdecl SchValue_MakePair(SchValue* out, int unused, void* a, void* b, void* c)
{
    BYTE* obj = (BYTE*)Pool44_New(44);
    BYTE* p = obj ? CallThis<BYTE*>(FUN_4b1120, obj, a, b, c) : 0;
    return SchValue_WrapNew(out, p, 2, "Pair");
}
KING_ALIAS("_SchValue_MakePair", "FUN_4a2140")

// make a Pair value (1 arg to constructor)                              [FUN_4a28c0]
extern "C" SchValue* __cdecl SchValue_MakePairB(SchValue* out, int, void* a)
{
    BYTE* obj = (BYTE*)Pool44_New(44);
    BYTE* p = obj ? CallThis<BYTE*>(FUN_4b1090, obj, a) : 0;
    return SchValue_WrapNew(out, p, 2, "Pair");
}
KING_ALIAS("_SchValue_MakePairB", "FUN_4a28c0")

// make a Number from an exactness flag and a signed integer            [FUN_4a2310]
extern "C" SchValue* __cdecl SchValue_MakeNumber(SchValue* out, int, const int* a, const int* b)
{
    BYTE* obj = (BYTE*)Pool32_New(32);
    BYTE* p = obj ? CallThis<BYTE*>(FUN_4b0b60, obj, *a, *b) : 0;
    return SchValue_WrapNew(out, p, 6, "Number");
}
KING_ALIAS("_SchValue_MakeNumber", "FUN_4a2310")

// make a Number from an exactness flag and an unsigned integer         [FUN_4a24e0]
extern "C" SchValue* __cdecl SchValue_MakeNumberU(SchValue* out, int, const int* a, const unsigned* b)
{
    BYTE* obj = (BYTE*)Pool32_New(32);
    BYTE* p = obj ? CallThis<BYTE*>(FUN_4b0b90, obj, *a, *b) : 0;
    return SchValue_WrapNew(out, p, 6, "Number");
}
KING_ALIAS("_SchValue_MakeNumberU", "FUN_4a24e0")

// make a Vector value from an int                                       [FUN_4a2700]
extern "C" SchValue* __cdecl SchValue_MakeVector(SchValue* out, int, int n)
{
    BYTE* obj = (BYTE*)operator new(32);
    BYTE* p = obj ? CallThis<BYTE*>(FUN_4b6670, obj, n) : 0;
    return SchValue_WrapNew(out, p, 10, "Vector");
}
KING_ALIAS("_SchValue_MakeVector", "FUN_4a2700")

// make an Environment value from a SchValue*                            [FUN_4a27e0]
extern "C" SchValue* __cdecl SchValue_MakeEnv(SchValue* out, int, void* parent)
{
    BYTE* obj = (BYTE*)operator new(160);
    BYTE* p = obj ? CallThis<BYTE*>(FUN_4a2d10, obj, parent) : 0;
    return SchValue_WrapNew(out, p, 12, "Environment");
}
KING_ALIAS("_SchValue_MakeEnv", "FUN_4a27e0")

// ---------------------------------------------------------------------------
// value type-check copies: copy a SchValue and assert its immediate type

extern "C" void __cdecl Sch_TypeError(int code, int level, const char* name);
KING_ALIAS("_Sch_TypeError", "FUN_4a5420")

// copy a value and assert it is an immediate Boolean (type 7)           [FUN_4a1940]
extern "C" SchValue* __fastcall SchValue_AsBool(SchValue* self, void*, const SchValue* src)
{
    self->obj = src->obj;
    self->m_4 = src->m_4;
    self->flags = src->flags;
    if (!(src->flags & SCH_IMMEDIATE))
        SchError(6);
    if ((BYTE)src->flags != 7)
        Sch_TypeError(2, 1, "Boolean");
    return self;
}
KING_ALIAS("@SchValue_AsBool@12", "FUN_4a1940")

// copy a value and assert it is an immediate Char (type 8)              [FUN_4a19c0]
extern "C" SchValue* __fastcall SchValue_AsChar(SchValue* self, void*, const SchValue* src)
{
    self->obj = src->obj;
    self->m_4 = src->m_4;
    self->flags = src->flags;
    if (!(src->flags & SCH_IMMEDIATE))
        SchError(6);
    if ((BYTE)src->flags != 8)
        Sch_TypeError(2, 1, "Char");
    return self;
}
KING_ALIAS("@SchValue_AsChar@12", "FUN_4a19c0")

// ---------------------------------------------------------------------------
// small helpers


// allocate deque map pages                                              [FUN_4a29e0]
// The original's shared epilogue at $L_4a2a62 ends with ret 8.
extern "C" void __stdcall Deque_AllocPages(int** first, int** last)
{
    while (first != last) {
        void* p = malloc(512);
        if (!p) { CallC(FUN_4581a0, 512); }
        *first = (int*)p;
        first++;
    }
}
KING_ALIAS("_Deque_AllocPages@8", "FUN_4a29e0")
