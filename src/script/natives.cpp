// natives.cpp - sch_embd.cpp of the game, rewritten from king.masm: the
// script's built-in functions, its messages and errors, value copies and
// the copies of the deque<char> holding script text.
//
// Building with  nmake ASM_NATIVES=1  makes king.masm assemble the original
// procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
extern "C" BYTE* __fastcall SchNode_Construct(BYTE* self, void*);  // evaluator.cpp

typedef char AsmLabel[1];
extern "C" int __cdecl _stricmp(const char* a, const char* b);
extern "C" void** __stdcall List_Erase(void** out, void** node);     // aimain.cpp

extern "C" void FUN_4a36f0(); KING_ALIAS("_FUN_4a36f0", "@HandlePair_Destroy@8")    // destructor of the value holders (map and list entries)
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // the AI's allocator: free (p, size)
extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")    // the AI's allocator: allocate (size)
extern "C" void FUN_4a52e0(); KING_ALIAS("_FUN_4a52e0", "_Sch_ErrorCode")    // script error (code, fatal)
extern "C" void FUN_4ab570(); KING_ALIAS("_FUN_4ab570", "_Sch4ab570_Construct")    // constructs a 44-byte element at p from x
extern "C" void FUN_4aa460(); KING_ALIAS("_FUN_4aa460", "@Elems44_InsertAux@16")    // vector<44 bytes>::insert_aux
ASM_PROC(FUN_4ace70)    // copy (first, last, deque iterator, tag)
extern "C" void FUN_4afe80(); KING_ALIAS("_FUN_4afe80", "_ValueList_MakeNode@4")    // list: a new node holding x

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

// the subclasses of SchNode that only set their vtable
static BYTE* NodeWithVtbl(BYTE* self, void* vtbl)
{
    SchNode_Construct(self, 0);
    F(void*, self, 0) = vtbl;
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

// ---------------------------------------------------------------------------
// medium-sized functions: error reports, the name map, value vectors

#include <stdlib.h>
extern "C" void* __cdecl Pool32_New(int size);  // evaluator.cpp
extern "C" void* __cdecl Pool44_New(int size);  // evaluator.cpp
extern "C" void __cdecl SchValue_Release(SchValue* v);  // evaluator.cpp
extern "C" int __cdecl Sch_Error(int code, int level, const char* a, const char* b);  // evaluator.cpp
extern "C" int __cdecl Sch_Message1(BYTE* p, int code, int a);  // evaluator.cpp

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
KING_VAR(Ints, g_errorOff, 691dbc)            // per error code: don't report
KING_VAR(int, g_lastErrorCode, 692244)
KING_VAR(int, g_lastErrorLevel, 692248)
KING_VAR(FILE*, g_errorFile, 670b04)
typedef void (__cdecl* ErrorHandler)(int code, int level, const char* text);

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

// sch_istr.cpp's Environment and Port value factories. Both return exactly
// the three words of a Scheme value; the second argument is a compiler slot.
extern "C" void FUN_4a2ef0(); KING_ALIAS("_FUN_4a2ef0", "@SchScope_Copy@16")
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

extern "C" void* __cdecl Stl_Allocate(unsigned bytes);

extern "C" {

// [FUN_4a1f10] deque<int> constructor for n elements (128 per page): a map
// of at least 8 slots with the pages in its middle
void __fastcall IntDeque_Construct(int* self, void*, unsigned n)
{
    unsigned nodes = (n >> 7) + 1;
    unsigned size = nodes + 2 > 8 ? nodes + 2 : 8;
    self[1] = size;
    int** map = size ? (int**)Stl_Allocate(size * 4) : 0;
    self[0] = (int)map;
    int** start = map + ((self[1] - nodes) >> 1);
    int** finish = start + nodes;
    Deque_AllocPages(start, finish);
    self[5] = (int)start;
    self[3] = (int)*start;
    self[4] = (int)*start + 512;
    self[9] = (int)(finish - 1);
    self[7] = (int)finish[-1];
    self[8] = (int)finish[-1] + 512;
    self[2] = self[3];
    self[6] = self[7] + (n & 127) * 4;
}

}

extern "C" void FUN_4b66f0(); KING_ALIAS("_FUN_4b66f0", "@SchVector_Construct@28")
extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")    // script type error (code, level, got, expected)

extern "C" {

// [FUN_4a25d0] out = a new vector object (kind 10) made from a, *b and the
// value *h; a type error when the object isn't a vector
DWORD* __cdecl Sch_MakeVector4a25d0(DWORD* out, int unused, int a, const int* b, const DWORD* h)
{
    BYTE* o = (BYTE*)operator new(32);
    BYTE* v = 0;
    if (o) {
        DWORD c[3];
        if (h[2] & 0x10000) {
            c[2] = h[2];
            c[0] = h[0];
            c[1] = h[1];
        } else {
            c[1] = 0;
            if (h[0]) {
                c[0] = h[0];
                c[2] = h[2];
                F(int, (BYTE*)c[0], 4)++;
            } else {
                c[0] = 0;
                c[2] = 0;
            }
        }
        v = CallThis<BYTE*>(FUN_4b66f0, o, a, *b, c[0], c[1], c[2]);
    }
    out[0] = out[1] = out[2] = 0;
    if (!v) {
        out[0] = 0;
        out[2] = 0;
        return out;
    }
    if (CallVirt<int>(v, 0) != 10) {
        const char* name = CallVirt<const char*>(v, 4);
        CallC(FUN_4a54e0, 2, 1, name, "Vector");
        return out;
    }
    out[0] = (DWORD)v;
    out[2] = CallVirt<int>(v, 0) & 255;
    F(int, (BYTE*)out[0], 4)++;
    return out;
}

}

extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")    // value: constructor (empty)
KING_VAR(char, gv64bf4c_n, 64bf4c)
KING_VAR(char, gv64bed0_n, 64bed0)

extern "C" {

// [FUN_4a2d10] scope constructor: linked after the scope at +4 of outer,
// 8 empty values at +36, a value vector at +132 with room for 8
BYTE* __fastcall SchScope_Construct(BYTE* self, void*, BYTE* outer)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = &gv64bed0_n;
    BYTE* link = outer + 4;
    BYTE* next = F(BYTE*, link, 12);
    F(BYTE*, self, 12) = next;
    F(BYTE*, self, 16) = link;
    F(BYTE*, next, 16) = self;
    F(BYTE*, F(BYTE*, self, 16), 12) = self;
    for (int off = 20; off <= 32; off += 4)
        F(int, self, off) = 0;
    for (int i = 0; i < 8; i++)
        CallThis(FUN_4bd4d0, self + 36 + i * 12);
    BYTE** v = (BYTE**)(self + 132);
    v[0] = v[1] = v[2] = 0;
    for (int off = 144; off <= 156; off += 4)
        F(int, self, off) = 0;
    F(void*, self, 0) = &gv64bf4c_n;
    if ((unsigned)((v[2] - v[0]) / 12) < 8) {
        int n = (v[1] - v[0]) / 12;
        BYTE* p = CallThis<BYTE*>(FUN_4a4540, v, 8, v[0], v[1]);
        for (BYTE* e = v[0]; e != v[1]; e += 12)
            CallThis(FUN_4a36c0, e);
        int cap = (v[2] - v[0]) / 12;
        if (cap)
            CallC(FUN_44b4a0, v[0], cap * 12);
        v[0] = p;
        v[1] = p + n * 12;
        v[2] = p + 96;
    }
    return self;
}

}

extern "C" void FUN_4a54e0(); KING_ALIAS("_FUN_4a54e0", "_Sch_Error")    // reports a script error
extern "C" void FUN_4a2a80(); KING_ALIAS("_FUN_4a2a80", "@Deque_ReallocMap@16")    // deque: reallocates the map (n, at front)
extern "C" void FUN_4581a0(); KING_ALIAS("_FUN_4581a0", "_Ai_OperatorNew")    // malloc retrying through the new handler
KING_VAR(char, s_type6708e4, 6708e4)

// [FUN_4a1da0] a handle copied from a value of type 10, an error for another type
extern "C" void** __fastcall Handle10_Init(void** self, int, void** v)
{
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    BYTE* obj = (BYTE*)v[0];
    if (!obj) {
        self[0] = 0;
        self[2] = 0;
        return self;
    }
    if (CallVirt<int>(obj, 0) != 10) {
        int name = CallVirt<int>(v[0], 4);
        CallC(FUN_4a54e0, 2, 1, name, &s_type6708e4);
        return self;
    }
    self[0] = v[0];
    self[2] = v[2];
    ++*(int*)((BYTE*)v[0] + 4);
    return self;
}

// [FUN_4a20a0] deque<dword>: push_back when the last block is full (a new 512-byte block)
extern "C" void __fastcall DequeDw_PushBackAux(BYTE* self, int, const DWORD* x)
{
    DWORD v = *x;
    if ((unsigned)(F(int, self, 4) - ((F(BYTE*, self, 36) - F(BYTE*, self, 0)) >> 2)) < 2)
        CallThis(FUN_4a2a80, self, 1, 0);
    void* p = malloc(512);
    if (!p)
        p = CallC<void*>(FUN_4581a0, 512);
    F(void**, self, 36)[1] = p;
    DWORD* cur = F(DWORD*, self, 24);
    if (cur)
        *cur = v;
    F(BYTE*, self, 36) += 4;
    BYTE* first = *F(BYTE**, self, 36);
    F(BYTE*, self, 28) = first;
    F(BYTE*, self, 32) = first + 512;
    F(BYTE*, self, 24) = F(BYTE*, self, 28);
}

// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_4a3fd0(); KING_ALIAS("_FUN_4a3fd0", "@SchScope_Destroy@8")
KING_VAR(AsmLabel, g_nat67060c, 67060c)

static BYTE* Nat_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_4a2eb0]
extern "C" int __fastcall Nat_4a2eb0(BYTE* s) { return 12; }
// [$L_4a2ec0]
extern "C" int __fastcall Nat_4a2ec0(BYTE* s) { return (int)g_nat67060c; }
// [$L_4a2ed0]
extern "C" BYTE* __fastcall Nat4a2ed0_DeletingDestructor(BYTE* s, void*, int flags) { return Nat_DeleteAfter(s, flags, FUN_4a3fd0); }

extern "C" void FUN_50d190(); KING_ALIAS("_FUN_50d190", "@Shell_Nop@8")
extern "C" void FUN_4abea0(); KING_ALIAS("_FUN_4abea0", "@Sch_Copy16@12")
static DequeIt Nat201_Copy(const DequeIt& s)
{
    DequeIt r;
    CallThis(FUN_4abea0, &r, &s);
    return r;
}
// [FUN_4a2ce0] copies of the iterators a and b handed to an empty call
extern "C" void __cdecl DequeIt_Pass2(DequeIt a, DequeIt b, int)
{
    DequeIt b2 = Nat201_Copy(b), a2 = Nat201_Copy(a);
    CallC(FUN_50d190, a2, b2, (int)b.cur);
}
// [FUN_4a2cb0] FUN_4a2ce0 with copies of a and b
extern "C" void __cdecl DequeIt_Pass1(DequeIt a, DequeIt b)
{
    DequeIt b2 = Nat201_Copy(b), a2 = Nat201_Copy(a);
    DequeIt_Pass2(a2, b2, 0);
}
// [FUN_4a29a0] FUN_4a2cb0 with a copy of a and b as it is
extern "C" void __cdecl DequeIt_Pass0(DequeIt a, DequeIt b)
{
    DequeIt b2 = b, a2 = Nat201_Copy(a);
    DequeIt_Pass1(a2, b2);
}

// ---------------------------------------------------------------------------
// The number natives: number? complex? real? rational? integer? exact?
// inexact?, the comparison chains = < > <= >= and +.

extern "C" void FUN_491c00(); KING_ALIAS("_FUN_491c00", "_Sch_SecondArg")
extern "C" void FUN_492250(); KING_ALIAS("_FUN_492250", "_SchValue_Make10007")      // an immediate value (out, 0, v)
extern "C" void FUN_40c100(); KING_ALIAS("_FUN_40c100", "@Str_Assign@12")          // value assignment
extern "C" void FUN_454580(); KING_ALIAS("_FUN_454580", "@Str_CopyConstruct@12")   // value copy constructor
extern "C" void FUN_4556d0(); KING_ALIAS("_FUN_4556d0", "@NumberHandle_From@12")   // a number handle from a value
extern "C" void FUN_4574e0(); KING_ALIAS("_FUN_4574e0", "_Number_Make")
extern "C" void FUN_4954c0(); KING_ALIAS("_FUN_4954c0", "@SchEmbd_Copy@12")        // number = number
extern "C" void FUN_4b0ef0(); KING_ALIAS("_FUN_4b0ef0", "@SchNumber_Add@12")       // number += number
extern "C" void FUN_4a5b50(); KING_ALIAS("_FUN_4a5b50", "_Sch_Message2")
extern "C" void FUN_4b0eb0(); KING_ALIAS("_FUN_4b0eb0", "@SchNumber_IsReal@8")
extern "C" void FUN_4b0ed0(); KING_ALIAS("_FUN_4b0ed0", "@SchNumber_IsInexact@8")
extern "C" void FUN_4b0ee0(); KING_ALIAS("_FUN_4b0ee0", "@SchNumber_IsExact@8")
extern "C" void FUN_4cef50(); KING_ALIAS("_FUN_4cef50", "_Effect_Zero")             // returns 0
extern "C" void FUN_4b0fd0(); KING_ALIAS("_FUN_4b0fd0", "_SchNumber_Ne")
extern "C" void FUN_4b0ff0(); KING_ALIAS("_FUN_4b0ff0", "_SchNumber_Lt")
extern "C" void FUN_4b1010(); KING_ALIAS("_FUN_4b1010", "_SchNumber_Gt")
extern "C" void FUN_4b1030(); KING_ALIAS("_FUN_4b1030", "_SchNumber_Le")
extern "C" void FUN_4b1050(); KING_ALIAS("_FUN_4b1050", "_SchNumber_Ge")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
KING_VAR(AsmLabel, s_nNumberP, 670664)
KING_VAR(AsmLabel, s_nComplexP, 67066c)
KING_VAR(AsmLabel, s_nRealP, 670678)
KING_VAR(AsmLabel, s_nRationalP, 670680)
KING_VAR(AsmLabel, s_nIntegerP, 67068c)
KING_VAR(AsmLabel, s_nExactP, 670698)
KING_VAR(AsmLabel, s_nInexactP, 6706a0)
KING_VAR(AsmLabel, s_nPlus, 6706ac)
KING_VAR(AsmLabel, s_nNumber, 66ca60)       // "Number"

static SchValue* Nat_Bool(SchValue* out, int v)
{
    return CallC<SchValue*>(FUN_492250, out, 0, v);
}

static void Nat_Release(SchValue* v)
{
    CallThis(FUN_4a36c0, v);
}

static int Nat_Count(BYTE** args)
{
    return (args[1] - args[0]) / 12;
}

// number?, complex?: the argument is a number
static SchValue* Nat_IsNumber(SchValue* out, BYTE* p, BYTE** args, const char* where)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, where)) {
        out->obj = 0;
        out->m_4 = 0;
        out->flags = 0;
    } else
        Nat_Bool(out, (BYTE)v.flags == 6);
    SchValue_Release(&v);
    return out;
}

// real? .. inexact?: false unless a number, else test(number)
static SchValue* Nat_NumberTest(SchValue* out, BYTE* p, BYTE** args, const char* where, void (*test)())
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, where)) {
        out->obj = 0;
        out->m_4 = 0;
        out->flags = 0;
    } else if ((BYTE)v.flags == 6) {
        BYTE* n = 0;
        DWORD flags = 0;
        if (v.obj) {
            if (CallVirt<int>(v.obj, 0) == 6) {
                n = v.obj;
                flags = v.flags;
                F(int, n, 4)++;
            } else
                CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(v.obj, 4), (const char*)s_nNumber);
        }
        if (!n)
            CallC(FUN_4a52e0, 0, 1);
        if (flags & SCH_IMMEDIATE)
            CallC(FUN_4a52e0, 4, 1);
        Nat_Bool(out, CallThis<int>(test, n));
        if (!(flags & SCH_IMMEDIATE) && n && --F(int, n, 4) == 0)
            CallVirt(n, 12, 1);
    } else
        Nat_Bool(out, 0);
    SchValue_Release(&v);
    return out;
}

// = < > <= >=: true unless stop(previous, next) for a pair of neighbours; false when one is not a number
static SchValue* Nat_Chain(SchValue* out, BYTE** args, void (*stop)())
{
    SchValue a = {0, 0, 0, 0}, b = {0, 0, 0, 0};
    int n = Nat_Count(args);
    if (n == 1) {
        Nat_Bool(out, -1);
        SchValue_Release(&a);
        SchValue_Release(&b);
        return out;
    }
    CopyRef(&a, (SchValue*)(args[0] + 12));
    if (Nat_Count(args) == 2) {
        Nat_Bool(out, -1);
        SchValue_Release(&a);
        SchValue_Release(&b);
        return out;
    }
    int result = -1;
    for (int i = 2; i != Nat_Count(args); i++) {
        CallThis(FUN_40c100, &b, &a);
        CallThis(FUN_40c100, &a, args[0] + i * 12);
        if ((BYTE)b.flags != 6 || (BYTE)a.flags != 6) {
            result = 0;
            break;
        }
        SchValue hb, ha;
        CallThis(FUN_4556d0, &hb, &b);
        CallThis(FUN_4556d0, &ha, &a);
        if (CallC<char>(stop, hb.obj, ha.obj)) {
            Nat_Bool(out, 0);
            Nat_Release(&ha);
            Nat_Release(&hb);
            Nat_Release(&a);
            Nat_Release(&b);
            return out;
        }
        Nat_Release(&ha);
        Nat_Release(&hb);
    }
    Nat_Bool(out, result);
    Nat_Release(&a);
    Nat_Release(&b);
    return out;
}

extern "C" {

// [$L_4932d0] number?
SchValue* __cdecl Nat_NumberP(SchValue* out, BYTE* p, BYTE** args) { return Nat_IsNumber(out, p, args, s_nNumberP); }
// $L_4933f0: complex?
SchValue* __cdecl Nat_ComplexP(SchValue* out, BYTE* p, BYTE** args) { return Nat_IsNumber(out, p, args, s_nComplexP); }
// $L_493510: real?
SchValue* __cdecl Nat_RealP(SchValue* out, BYTE* p, BYTE** args) { return Nat_NumberTest(out, p, args, s_nRealP, FUN_4b0eb0); }
// $L_493740: rational? (the same test as real?)
SchValue* __cdecl Nat_RationalP(SchValue* out, BYTE* p, BYTE** args) { return Nat_NumberTest(out, p, args, s_nRationalP, FUN_4b0eb0); }
// $L_493970: integer? (always false for numbers)
SchValue* __cdecl Nat_IntegerP(SchValue* out, BYTE* p, BYTE** args) { return Nat_NumberTest(out, p, args, s_nIntegerP, FUN_4cef50); }
// $L_493ba0: exact? (calls the test the names call IsInexact)
SchValue* __cdecl Nat_ExactP(SchValue* out, BYTE* p, BYTE** args) { return Nat_NumberTest(out, p, args, s_nExactP, FUN_4b0ed0); }
// $L_493dd0: inexact?
SchValue* __cdecl Nat_InexactP(SchValue* out, BYTE* p, BYTE** args) { return Nat_NumberTest(out, p, args, s_nInexactP, FUN_4b0ee0); }
// $L_494000: =
SchValue* __cdecl Nat_NumEq(SchValue* out, BYTE* p, BYTE** args) { return Nat_Chain(out, args, FUN_4b0fd0); }
// $L_494350: <
SchValue* __cdecl Nat_NumLt(SchValue* out, BYTE* p, BYTE** args) { return Nat_Chain(out, args, FUN_4b1050); }
// $L_4946a0: >
SchValue* __cdecl Nat_NumGt(SchValue* out, BYTE* p, BYTE** args) { return Nat_Chain(out, args, FUN_4b1030); }
// $L_4949f0: <=
SchValue* __cdecl Nat_NumLe(SchValue* out, BYTE* p, BYTE** args) { return Nat_Chain(out, args, FUN_4b1010); }
// $L_494d40: >=
SchValue* __cdecl Nat_NumGe(SchValue* out, BYTE* p, BYTE** args) { return Nat_Chain(out, args, FUN_4b0ff0); }

// $L_495090: + (the sum of the arguments; a non-number is reported and skipped)
SchValue* __cdecl Nat_Plus(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue a = {0, 0, 0, 0};
    SchValue s;
    int zero1 = 0, zero2 = 0;
    CallC(FUN_4574e0, &s, 0, &zero2, &zero1);
    if (Nat_Count(args) == 1) {
        CopyRef(out, &s);
        Nat_Release(&s);
        SchValue_Release(&a);
        return out;
    }
    CopyRef(&a, (SchValue*)(args[0] + 12));
    if (Nat_Count(args) == 2) {
        if ((BYTE)a.flags == 6) {
            SchValue h;
            CallThis(FUN_4bd4d0, &h);
            if (a.obj) {
                if (CallVirt<int>(a.obj, 0) == 6) {
                    h.obj = a.obj;
                    h.flags = a.flags;
                    F(int, a.obj, 4)++;
                } else
                    CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(a.obj, 4), (const char*)s_nNumber);
            } else {
                h.obj = 0;
                h.flags = 0;
            }
            CopyRef(out, &a);
            Nat_Release(&h);
            Nat_Release(&s);
            SchValue_Release(&a);
            return out;
        }
        CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, (const char*)s_nPlus);
    } else {
        if ((BYTE)a.flags == 6) {
            SchValue h;
            CallThis(FUN_4556d0, &h, &a);
            CallThis(FUN_4954c0, s.obj, h.obj);
            Nat_Release(&h);
        } else
            CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, (const char*)s_nPlus);
        for (int i = 2; i != Nat_Count(args); i++) {
            CallThis(FUN_40c100, &a, args[0] + i * 12);
            if ((BYTE)a.flags == 6) {
                SchValue h;
                CallThis(FUN_4556d0, &h, &a);
                CallThis(FUN_4b0ef0, s.obj, h.obj);
                Nat_Release(&h);
            } else
                CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, (const char*)s_nPlus);
        }
    }
    CallThis(FUN_454580, out, &s);
    Nat_Release(&s);
    Nat_Release(&a);
    return out;
}

}  // extern "C"

// ---------------------------------------------------------------------------
// More natives: * and /, the unimplemented number functions (they return an
// empty value), number<->string, pairs, symbols and characters.

extern "C" void FUN_491aa0(); KING_ALIAS("_FUN_491aa0", "_Sch_Args2")
extern "C" void FUN_4b1070(); KING_ALIAS("_FUN_4b1070", "_Sch_Zero12")             // an empty value (out, ...)
extern "C" void FUN_4b0f30(); KING_ALIAS("_FUN_4b0f30", "@SchNumber_Mul@12")
extern "C" void FUN_4b0f60(); KING_ALIAS("_FUN_4b0f60", "@SchNumber_Div@12")
extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")
extern "C" void FUN_4b0c70(); KING_ALIAS("_FUN_4b0c70", "@Number_Write@12")       // writes a number's text (vector)
extern "C" void FUN_44b090(); KING_ALIAS("_FUN_44b090", "@Vec1_Free@8")
extern "C" void FUN_4a1e30(); KING_ALIAS("_FUN_4a1e30", "_SchValue_MakeString")
extern "C" void FUN_4a2230(); KING_ALIAS("_FUN_4a2230", "_SchValue_MakeIdent")
extern "C" void FUN_4a2140(); KING_ALIAS("_FUN_4a2140", "_SchValue_MakePair")
extern "C" void FUN_4a2310(); KING_ALIAS("_FUN_4a2310", "_SchValue_MakeNumber")
extern "C" void FUN_4a1900(); KING_ALIAS("_FUN_4a1900", "@SchEmbd4a1900_Construct@16")    // deque<int> constructor
extern "C" void FUN_4611c0(); KING_ALIAS("_FUN_4611c0", "_Construct4")
extern "C" void FUN_4a20a0(); KING_ALIAS("_FUN_4a20a0", "@DequeDw_PushBackAux@12")
extern "C" void FUN_4b20d0(); KING_ALIAS("_FUN_4b20d0", "_Sch_ParseNumber")    // a number parsed from a deque of characters (out, p, deque)
extern "C" void FUN_4a1710(); KING_ALIAS("_FUN_4a1710", "@Deque_Destroy@4")    // deque<int> destructor
extern "C" void FUN_407f00(); KING_ALIAS("_FUN_407f00", "_Str_Member20")           // a pair's car (out, value)
extern "C" void FUN_4090a0(); KING_ALIAS("_FUN_4090a0", "_Str_Member32")           // a pair's cdr (out, value)
extern "C" void FUN_4a5a60(); KING_ALIAS("_FUN_4a5a60", "_Sch_Message1")
extern "C" void FUN_4a19c0(); KING_ALIAS("_FUN_4a19c0", "@SchValue_AsChar@12")
extern "C" void FUN_49c210(); KING_ALIAS("_FUN_49c210", "_SchValue_Make10008")
extern "C" void FUN_4b0ea0(); KING_ALIAS("_FUN_4b0ea0", "@Value_ToInt@8")
KING_VAR(AsmLabel, s_nTimes, 6706b4)
KING_VAR(AsmLabel, s_nDivide, 670570)
KING_VAR(AsmLabel, s_nQuotient, 6706b8)
KING_VAR(AsmLabel, s_nRemainder, 6706c0)
KING_VAR(AsmLabel, s_nModulo, 6706cc)
KING_VAR(AsmLabel, s_nAtan2, 67073c)
KING_VAR(AsmLabel, s_nNumerator, 6706d4)
KING_VAR(AsmLabel, s_nDenominator, 6706e0)
KING_VAR(AsmLabel, s_nFloor, 6706ec)
KING_VAR(AsmLabel, s_nCeiling, 6706f4)
KING_VAR(AsmLabel, s_nTruncate, 6706fc)
KING_VAR(AsmLabel, s_nRound, 670708)
KING_VAR(AsmLabel, s_nExp, 670710)
KING_VAR(AsmLabel, s_nLog, 670714)
KING_VAR(AsmLabel, s_nSin, 670718)
KING_VAR(AsmLabel, s_nCos, 67071c)
KING_VAR(AsmLabel, s_nTan, 670720)
KING_VAR(AsmLabel, s_nAsin, 670724)
KING_VAR(AsmLabel, s_nAcos, 67072c)
KING_VAR(AsmLabel, s_nAtan, 670734)
KING_VAR(AsmLabel, s_nSqrt, 670744)
KING_VAR(AsmLabel, s_nExpt, 67074c)
KING_VAR(AsmLabel, s_nMakeRect, 670754)
KING_VAR(AsmLabel, s_nMakePolar, 670768)
KING_VAR(AsmLabel, s_nRealPart, 670774)
KING_VAR(AsmLabel, s_nImagPart, 670780)
KING_VAR(AsmLabel, s_nMagnitude, 67078c)
KING_VAR(AsmLabel, s_nAngle, 670798)
KING_VAR(AsmLabel, s_nToInexact, 6707a0)
KING_VAR(AsmLabel, s_nToExact, 6707b0)
KING_VAR(AsmLabel, s_nNumToStr, 6707c0)
KING_VAR(AsmLabel, s_nStrToNum, 6707d0)
KING_VAR(AsmLabel, s_nPairP, 6707e0)
KING_VAR(AsmLabel, s_nCons, 6707e8)
KING_VAR(AsmLabel, s_nCar, 66de28)
KING_VAR(AsmLabel, s_nCdr, 6707f0)
KING_VAR(AsmLabel, s_nSetCar, 6707f4)
KING_VAR(AsmLabel, s_nSetCdr, 670800)
KING_VAR(AsmLabel, s_nSymbolP, 67080c)
KING_VAR(AsmLabel, s_nSymToStr, 670814)
KING_VAR(AsmLabel, s_nStrToSym, 670824)
KING_VAR(AsmLabel, s_nCharP, 670834)
KING_VAR(AsmLabel, s_nCharEq, 67083c)
KING_VAR(AsmLabel, s_nCharLt, 670844)
KING_VAR(AsmLabel, s_nCharGt, 67084c)
KING_VAR(AsmLabel, s_nCharLe, 670854)
KING_VAR(AsmLabel, s_nCharGe, 67085c)
KING_VAR(AsmLabel, s_nCharToInt, 670864)
KING_VAR(AsmLabel, s_nIntToChar, 670874)
KING_VAR(AsmLabel, s_nPair, 66cb08)         // "Pair"
KING_VAR(AsmLabel, s_nIdentifier, 66ce4c)   // "Identifier"
KING_VAR(AsmLabel, s_nString, 66ca58)       // "String"

static void Nat_Zero3(SchValue* v)
{
    v->obj = 0;
    v->m_4 = 0;
    v->flags = 0;
}

// a typed handle of v (an object of the type, its reference counted), else an error and an empty handle
static void Nat_Typed(SchValue* h, const SchValue* v, int type, const char* typeName)
{
    if (v->obj) {
        if (CallVirt<int>(v->obj, 0) == type) {
            h->obj = v->obj;
            h->flags = v->flags;
            F(int, v->obj, 4)++;
        } else
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(v->obj, 4), typeName);
    } else {
        h->obj = 0;
        h->flags = 0;
    }
}

// the checks of a handle's use: not empty, not immediate
static void Nat_Check(const SchValue* h)
{
    if (!h->obj)
        CallC(FUN_4a52e0, 0, 1);
    if (h->flags & SCH_IMMEDIATE)
        CallC(FUN_4a52e0, 4, 1);
}

// type?: the argument's type is kind
static SchValue* Nat_IsKind(SchValue* out, BYTE* p, BYTE** args, const char* where, int kind)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, where))
        Nat_Zero3(out);
    else
        Nat_Bool(out, (BYTE)v.flags == kind);
    SchValue_Release(&v);
    return out;
}

// *, / and -: like + from start (with one argument: * returns it, / its inverse, - its negation)
static SchValue* Nat_Arith(SchValue* out, BYTE* p, BYTE** args, const char* where, void (*op)(), bool divide,
                           int start = 1)
{
    SchValue a = {0, 0, 0, 0};
    SchValue s;
    int one = start, zero = 0;
    CallC(FUN_4574e0, &s, 0, &zero, &one);
    if (Nat_Count(args) == 1) {
        CopyRef(out, &s);
        Nat_Release(&s);
        SchValue_Release(&a);
        return out;
    }
    CopyRef(&a, (SchValue*)(args[0] + 12));
    if (Nat_Count(args) == 2) {
        if (!divide) {
            if ((BYTE)a.flags == 6) {
                SchValue h;
                CallThis(FUN_4bd4d0, &h);
                Nat_Typed(&h, &a, 6, s_nNumber);
                CopyRef(out, &a);
                Nat_Release(&h);
                Nat_Release(&s);
                SchValue_Release(&a);
                return out;
            }
            CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, where);
        } else {
            if ((BYTE)a.flags == 6) {
                SchValue h;
                CallThis(FUN_4bd4d0, &h);
                Nat_Typed(&h, &a, 6, s_nNumber);
                CallThis(op, s.obj, h.obj);
                Nat_Release(&h);
            } else
                CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, where);
            CopyRef(out, &s);
            Nat_Release(&s);
            SchValue_Release(&a);
            return out;
        }
    } else {
        if ((BYTE)a.flags == 6) {
            SchValue h;
            CallThis(FUN_4556d0, &h, &a);
            CallThis(FUN_4954c0, s.obj, h.obj);
            Nat_Release(&h);
        } else
            CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, where);
        for (int i = 2; i != Nat_Count(args); i++) {
            CallThis(FUN_40c100, &a, args[0] + i * 12);
            if ((BYTE)a.flags == 6) {
                SchValue h;
                CallThis(FUN_4556d0, &h, &a);
                CallThis(op, s.obj, h.obj);
                Nat_Release(&h);
            } else
                CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, where);
        }
    }
    CallThis(FUN_454580, out, &s);
    Nat_Release(&s);
    Nat_Release(&a);
    return out;
}

// the unimplemented functions of one number: an empty value
static SchValue* Nat_Number1(SchValue* out, BYTE* p, BYTE** args, const char* where)
{
    SchValue v = {0, 0, 0, 0}, h;
    if (CallC<int>(FUN_491c00, p, &v, args, where)) {
        Nat_Zero3(&h);
        Nat_Zero3(out);
        Nat_Release(&h);
    } else if ((BYTE)v.flags == 6) {
        CallThis(FUN_4bd4d0, &h);
        Nat_Typed(&h, &v, 6, s_nNumber);
        SchValue t;
        CopyRef(out, CallC<SchValue*>(FUN_4b1070, &t, p, h.obj));
        Nat_Release(&t);
        Nat_Release(&h);
    } else {
        CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, where);
        CallThis(FUN_4bd4d0, &h);
        CallThis(FUN_454580, out, &h);
        Nat_Release(&h);
    }
    SchValue_Release(&v);
    return out;
}

// the unimplemented functions of two numbers: an empty value
static SchValue* Nat_Number2(SchValue* out, BYTE* p, BYTE** args, const char* where)
{
    SchValue x = {0, 0, 0, 0}, y = {0, 0, 0, 0};
    if (CallC<int>(FUN_491aa0, p, &x, &y, args, where)) {
        SchValue h;
        CallThis(FUN_4bd4d0, &h);
        CopyRef(out, &h);
        Nat_Release(&h);
        SchValue_Release(&y);
        SchValue_Release(&x);
        return out;
    }
    if ((BYTE)x.flags == 6 && (BYTE)y.flags == 6) {
        SchValue hx, hy, t;
        CallThis(FUN_4bd4d0, &hx);
        Nat_Typed(&hx, &x, 6, s_nNumber);
        CallThis(FUN_4bd4d0, &hy);
        Nat_Typed(&hy, &y, 6, s_nNumber);
        CopyRef(out, CallC<SchValue*>(FUN_4b1070, &t, p, hx.obj, hy.obj));
        Nat_Release(&t);
        Nat_Release(&hy);
        Nat_Release(&hx);
    } else {
        CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, where);
        SchValue t;
        CallThis(FUN_4bd4d0, &t);
        CallThis(FUN_454580, out, &t);
        Nat_Release(&t);
    }
    Nat_Release(&y);
    Nat_Release(&x);
    return out;
}

// car, cdr: get(out, the pair)
static SchValue* Nat_PairPart(SchValue* out, BYTE* p, BYTE** args, const char* where, void (*get)())
{
    BYTE* a = args[0];
    if ((args[1] - a) / 12 != 2) {
        CallC(FUN_4a5a60, p, 33, where);
        Nat_Zero3(out);
        return out;
    }
    if (F(BYTE, a, 20) == 2) {
        CallC(get, out, a + 12);
        return out;
    }
    CallC(FUN_4a5b50, p, 15, (const char*)s_nPair, where);
    Nat_Zero3(out);
    return out;
}

// set-car!, set-cdr!: the pair's value at off = the second argument
static SchValue* Nat_PairSet(SchValue* out, BYTE* p, BYTE** args, const char* where, int off)
{
    SchValue x = {0, 0, 0, 0}, y = {0, 0, 0, 0}, h;
    if (CallC<int>(FUN_491aa0, p, &x, &y, args, where)) {
        Nat_Zero3(&h);
        Nat_Zero3(out);
        Nat_Release(&h);
        SchValue_Release(&y);
        SchValue_Release(&x);
        return out;
    }
    if ((BYTE)x.flags == 2) {
        CallThis(FUN_4bd4d0, &h);
        Nat_Typed(&h, &x, 2, s_nPair);
        Nat_Check(&h);
        CallThis(FUN_40c100, h.obj + off, &y);
        Nat_Release(&h);
    } else
        CallC(FUN_4a5b50, p, 15, (const char*)s_nPair, where);
    Nat_Zero3(out);
    SchValue_Release(&y);
    Nat_Release(&x);
    return out;
}

// symbol->string, string->symbol: make(out, 0, the text at +12 of the argument of the type)
static SchValue* Nat_Convert(SchValue* out, BYTE* p, BYTE** args, const char* where, int type, const char* typeName,
                             void (*make)())
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, where))
        Nat_Zero3(out);
    else if ((BYTE)v.flags == type) {
        SchValue h = {0, 0, 0, 0}, t;
        Nat_Typed(&h, &v, type, typeName);
        Nat_Check(&h);
        CopyRef(out, CallC<SchValue*>(make, &t, 0, h.obj + 12));
        Nat_Release(&t);
        Nat_Release(&h);
    } else {
        CallC(FUN_4a5b50, p, 15, typeName, where);
        Nat_Zero3(out);
    }
    SchValue_Release(&v);
    return out;
}

// char=? .. char>=?: compare the two characters
static SchValue* Nat_CharCmp(SchValue* out, BYTE* p, BYTE** args, const char* where, int op)
{
    SchValue x = {0, 0, 0, 0}, y = {0, 0, 0, 0};
    if (CallC<int>(FUN_491aa0, p, &x, &y, args, where)) {
        SchValue h;
        Nat_Zero3(&h);
        Nat_Zero3(out);
        Nat_Release(&h);
        SchValue_Release(&y);
        SchValue_Release(&x);
        return out;
    }
    if ((BYTE)x.flags == 8 && (BYTE)y.flags == 8) {
        SchValue cx, cy;
        CallThis(FUN_4a19c0, &cx, &x);
        CallThis(FUN_4a19c0, &cy, &y);
        int a = (int)cx.obj, b = (int)cy.obj, r;
        switch (op) {
        case 0: r = a == b; break;
        case 1: r = a < b; break;
        case 2: r = a > b; break;
        case 3: r = a <= b; break;
        default: r = a >= b; break;
        }
        Nat_Bool(out, r);
        Nat_Release(&cy);
        Nat_Release(&cx);
        SchValue_Release(&y);
        SchValue_Release(&x);
        return out;
    }
    Nat_Bool(out, 0);
    Nat_Release(&y);
    Nat_Release(&x);
    return out;
}

extern "C" {

// [$L_495920] *
SchValue* __cdecl Nat_Times(SchValue* out, BYTE* p, BYTE** args) { return Nat_Arith(out, p, args, s_nTimes, FUN_4b0f30, false); }
// $L_495d50: /
SchValue* __cdecl Nat_Divide(SchValue* out, BYTE* p, BYTE** args) { return Nat_Arith(out, p, args, s_nDivide, FUN_4b0f60, true); }
// $L_496180 .. $L_496780, $L_498760: quotent, reminder, modulo, atan2 (unimplemented)
SchValue* __cdecl Nat_Quotient(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number2(out, p, args, s_nQuotient); }
SchValue* __cdecl Nat_Remainder(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number2(out, p, args, s_nRemainder); }
SchValue* __cdecl Nat_Modulo(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number2(out, p, args, s_nModulo); }
SchValue* __cdecl Nat_Atan2(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number2(out, p, args, s_nAtan2); }
// $L_496a80 .. $L_499cf0: the unimplemented functions of one number
SchValue* __cdecl Nat_Numerator(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nNumerator); }
SchValue* __cdecl Nat_Denominator(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nDenominator); }
SchValue* __cdecl Nat_Floor(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nFloor); }
SchValue* __cdecl Nat_Ceiling(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nCeiling); }
SchValue* __cdecl Nat_Truncate(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nTruncate); }
SchValue* __cdecl Nat_Round(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nRound); }
SchValue* __cdecl Nat_Exp(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nExp); }
SchValue* __cdecl Nat_Log(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nLog); }
SchValue* __cdecl Nat_Sin(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nSin); }
SchValue* __cdecl Nat_Cos(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nCos); }
SchValue* __cdecl Nat_Tan(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nTan); }
SchValue* __cdecl Nat_Asin(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nAsin); }
SchValue* __cdecl Nat_Acos(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nAcos); }
SchValue* __cdecl Nat_Atan(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nAtan); }
SchValue* __cdecl Nat_Sqrt(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nSqrt); }
SchValue* __cdecl Nat_Expt(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nExpt); }
SchValue* __cdecl Nat_MakeRect(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nMakeRect); }
SchValue* __cdecl Nat_MakePolar(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nMakePolar); }
SchValue* __cdecl Nat_RealPart(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nRealPart); }
SchValue* __cdecl Nat_ImagPart(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nImagPart); }
SchValue* __cdecl Nat_Magnitude(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nMagnitude); }
SchValue* __cdecl Nat_Angle(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nAngle); }
SchValue* __cdecl Nat_ToInexact(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nToInexact); }
SchValue* __cdecl Nat_ToExact(SchValue* out, BYTE* p, BYTE** args) { return Nat_Number1(out, p, args, s_nToExact); }

// $L_499f00: number->string
SchValue* __cdecl Nat_NumberToString(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0}, h;
    if (CallC<int>(FUN_491c00, p, &v, args, s_nNumToStr)) {
        Nat_Zero3(&h);
        Nat_Zero3(out);
        Nat_Release(&h);
        SchValue_Release(&v);
        return out;
    }
    if ((BYTE)v.flags == 6) {
        CallThis(FUN_4bd4d0, &h);
        Nat_Typed(&h, &v, 6, s_nNumber);
        BYTE text[12];
        CallThis(FUN_487730, text, &args);
        Nat_Check(&h);
        CallThis(FUN_4b0c70, h.obj, text);
        SchValue t;
        CopyRef(out, CallC<SchValue*>(FUN_4a1e30, &t, 0, text));
        Nat_Release(&t);
        CallThis(FUN_44b090, text);
        Nat_Release(&h);
    } else {
        CallC(FUN_4a5b50, p, 15, (const char*)s_nNumber, (const char*)s_nNumToStr);
        SchValue t;
        CallThis(FUN_4bd4d0, &t);
        CallThis(FUN_454580, out, &t);
        Nat_Release(&t);
    }
    Nat_Release(&v);
    return out;
}

// $L_49a150: string->number (the characters go through a deque<int>)
SchValue* __cdecl Nat_StringToNumber(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0}, h;
    if (CallC<int>(FUN_491c00, p, &v, args, s_nStrToNum)) {
        Nat_Zero3(&h);
        Nat_Zero3(out);
        Nat_Release(&h);
        SchValue_Release(&v);
        return out;
    }
    if ((BYTE)v.flags == 3) {
        CallThis(FUN_4bd4d0, &h);
        Nat_Typed(&h, &v, 3, s_nString);
        BYTE dq[40];
        CallThis(FUN_4a1900, dq, &args, 0);
        Nat_Check(&h);
        for (BYTE* c = F(BYTE*, h.obj, 12);; c++) {
            Nat_Check(&h);
            if (c == F(BYTE*, h.obj, 16))
                break;
            int ch = (signed char)*c;
            if (F(BYTE*, dq, 24) == F(BYTE*, dq, 32) - 4)
                CallThis(FUN_4a20a0, dq, &ch);
            else {
                CallC(FUN_4611c0, F(BYTE*, dq, 24), &ch);
                F(BYTE*, dq, 24) += 4;
            }
        }
        SchValue t;
        CopyRef(out, CallC<SchValue*>(FUN_4b20d0, &t, p, dq));
        Nat_Release(&t);
        CallThis(FUN_4a1710, dq);
        Nat_Release(&h);
    } else {
        CallC(FUN_4a5b50, p, 15, (const char*)s_nString, (const char*)s_nStrToNum);
        SchValue t;
        CallThis(FUN_4bd4d0, &t);
        CallThis(FUN_454580, out, &t);
        Nat_Release(&t);
    }
    Nat_Release(&v);
    return out;
}

// $L_49a400: pair?
SchValue* __cdecl Nat_PairP(SchValue* out, BYTE* p, BYTE** args) { return Nat_IsKind(out, p, args, s_nPairP, 2); }

// $L_49a520: cons
SchValue* __cdecl Nat_Cons(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue x = {0, 0, 0, 0}, y = {0, 0, 0, 0}, t;
    if (CallC<int>(FUN_491aa0, p, &x, &y, args, s_nCons)) {
        Nat_Zero3(&t);
        Nat_Zero3(out);
        Nat_Release(&t);
    } else {
        CopyRef(out, CallC<SchValue*>(FUN_4a2140, &t, 0, p, &x, &y));
        Nat_Release(&t);
    }
    SchValue_Release(&y);
    SchValue_Release(&x);
    return out;
}

// $L_49a720: car, $L_49a7c0: cdr
SchValue* __cdecl Nat_Car(SchValue* out, BYTE* p, BYTE** args) { return Nat_PairPart(out, p, args, s_nCar, FUN_407f00); }
SchValue* __cdecl Nat_Cdr(SchValue* out, BYTE* p, BYTE** args) { return Nat_PairPart(out, p, args, s_nCdr, FUN_4090a0); }
// $L_49a860: set-car!, $L_49aab0: set-cdr!
SchValue* __cdecl Nat_SetCar(SchValue* out, BYTE* p, BYTE** args) { return Nat_PairSet(out, p, args, s_nSetCar, 20); }
SchValue* __cdecl Nat_SetCdr(SchValue* out, BYTE* p, BYTE** args) { return Nat_PairSet(out, p, args, s_nSetCdr, 32); }
// $L_49ad00: symbol?
SchValue* __cdecl Nat_SymbolP(SchValue* out, BYTE* p, BYTE** args) { return Nat_IsKind(out, p, args, s_nSymbolP, 9); }
// $L_49ae20: symbol->string, $L_49b040: string->symbol
SchValue* __cdecl Nat_SymbolToString(SchValue* out, BYTE* p, BYTE** args)
{
    return Nat_Convert(out, p, args, s_nSymToStr, 9, s_nIdentifier, FUN_4a1e30);
}
SchValue* __cdecl Nat_StringToSymbol(SchValue* out, BYTE* p, BYTE** args)
{
    return Nat_Convert(out, p, args, s_nStrToSym, 3, s_nString, FUN_4a2230);
}
// $L_49b260: char?
SchValue* __cdecl Nat_CharP(SchValue* out, BYTE* p, BYTE** args) { return Nat_IsKind(out, p, args, s_nCharP, 8); }
// $L_49b380 .. $L_49bc00: char=? char<? char>? char<=? char>=?
SchValue* __cdecl Nat_CharEq(SchValue* out, BYTE* p, BYTE** args) { return Nat_CharCmp(out, p, args, s_nCharEq, 0); }
SchValue* __cdecl Nat_CharLt(SchValue* out, BYTE* p, BYTE** args) { return Nat_CharCmp(out, p, args, s_nCharLt, 1); }
SchValue* __cdecl Nat_CharGt(SchValue* out, BYTE* p, BYTE** args) { return Nat_CharCmp(out, p, args, s_nCharGt, 2); }
SchValue* __cdecl Nat_CharLe(SchValue* out, BYTE* p, BYTE** args) { return Nat_CharCmp(out, p, args, s_nCharLe, 3); }
SchValue* __cdecl Nat_CharGe(SchValue* out, BYTE* p, BYTE** args) { return Nat_CharCmp(out, p, args, s_nCharGe, 4); }

// $L_49be20: char->integer
SchValue* __cdecl Nat_CharToInteger(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, s_nCharToInt))
        Nat_Zero3(out);
    else if ((BYTE)v.flags == 8) {
        SchValue c, t;
        CallThis(FUN_4a19c0, &c, &v);
        int zero = 0;
        CopyRef(out, CallC<SchValue*>(FUN_4a2310, &t, 0, &zero, &c.obj));
        Nat_Release(&t);
        SchValue_Release(&c);
    } else
        Nat_Zero3(out);
    SchValue_Release(&v);
    return out;
}

// $L_49bfe0: integer->char
SchValue* __cdecl Nat_IntegerToChar(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, s_nIntToChar))
        Nat_Zero3(out);
    else if ((BYTE)v.flags == 6) {
        SchValue h = {0, 0, 0, 0};
        Nat_Typed(&h, &v, 6, s_nNumber);
        Nat_Check(&h);
        CallC(FUN_49c210, out, 0, CallThis<int>(FUN_4b0ea0, h.obj));
        if (!(h.flags & SCH_IMMEDIATE) && h.obj && --F(int, h.obj, 4) == 0)
            CallVirt(h.obj, 12, 1);
    } else
        Nat_Zero3(out);
    SchValue_Release(&v);
    return out;
}

}  // extern "C"

// ---------------------------------------------------------------------------
// Strings, vectors, procedures and ports.

extern "C" void FUN_4918c0(); KING_ALIAS("_FUN_4918c0", "_Sch_Args3")
extern "C" void FUN_4a24e0(); KING_ALIAS("_FUN_4a24e0", "_SchValue_MakeNumberU")
extern "C" void FUN_4aa430(); KING_ALIAS("_FUN_4aa430", "@SchValue_Object@8")
extern "C" void FUN_4a25d0(); KING_ALIAS("_FUN_4a25d0", "_Sch_MakeVector4a25d0")
extern "C" void FUN_4a5910(); KING_ALIAS("_FUN_4a5910", "_Sch_Message0")
extern "C" void FUN_4a2700(); KING_ALIAS("_FUN_4a2700", "_SchValue_MakeVector")
extern "C" void FUN_4a16d0(); KING_ALIAS("_FUN_4a16d0", "@Vec12_PushBackRef@12")
extern "C" void FUN_4a27e0(); KING_ALIAS("_FUN_4a27e0", "_SchValue_MakeEnv")
extern "C" void FUN_4557f0(); KING_ALIAS("_FUN_4557f0", "_Port_MakeA")
extern "C" void FUN_4a2be0(); KING_ALIAS("_FUN_4a2be0", "_Values_Copy")
extern "C" void FUN_4a2400(); KING_ALIAS("_FUN_4a2400", "_SchValue_MakeStringB")
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")
extern "C" void FUN_4a1a40(); KING_ALIAS("_FUN_4a1a40", "@SchValues_InsertRange@20")    // vector<value>::insert(pos, first, last)
extern "C" void FUN_4b4c20(); KING_ALIAS("_FUN_4b4c20", "_Port_ReadDatum")    // read (out, p, port by value)
extern "C" void FUN_4b8070(); KING_ALIAS("_FUN_4b8070", "_Sch4b8070")    // write (out, p, value by value, port by value)
KING_VAR(AsmLabel, s_nStrLen, 670898)
KING_VAR(AsmLabel, s_nStrRef, 6708a8)
KING_VAR(AsmLabel, s_nStrSet, 6708b4)
KING_VAR(AsmLabel, s_nVectorP, 6708c0)
KING_VAR(AsmLabel, s_nMakeVector, 6708c8)
KING_VAR(AsmLabel, s_nListToVector, 6708d4)
KING_VAR(AsmLabel, s_nVector, 6708e4)        // "Vector"
KING_VAR(AsmLabel, s_nVecLen, 6708ec)
KING_VAR(AsmLabel, s_nVecRef, 6708fc)
KING_VAR(AsmLabel, s_nVecSet, 670908)
KING_VAR(AsmLabel, s_nProcP, 670914)
KING_VAR(AsmLabel, s_nInPortP, 670920)
KING_VAR(AsmLabel, s_nOutPortP, 67092c)
KING_VAR(AsmLabel, s_nOpenIn, 67093c)
KING_VAR(AsmLabel, s_nOpenOut, 67094c)
KING_VAR(AsmLabel, s_nCloseIn, 670960)
KING_VAR(AsmLabel, s_nCloseOut, 670974)
KING_VAR(AsmLabel, s_nReadChar, 670988)
KING_VAR(AsmLabel, s_nPeekChar, 670994)
KING_VAR(AsmLabel, s_nEofP, 6709a0)
KING_VAR(AsmLabel, s_nCharReady, 6709ac)
KING_VAR(AsmLabel, s_nWriteChar, 6709b8)
KING_VAR(AsmLabel, s_nRead, 6709c4)
KING_VAR(AsmLabel, s_nWrite, 6709cc)
KING_VAR(AsmLabel, s_nMacroExpand, 6709d4)
KING_VAR(AsmLabel, s_nStrAppend, 6709ec)
KING_VAR(AsmLabel, s_nPort, 66cb28)          // "Port"
KING_VAR(AsmLabel, s_nChar, 670650)          // "Char"

struct SchValue12 { DWORD d[3]; };              // a value passed by value

static int Nat_StrLen(BYTE* s) { return F(BYTE*, s, 16) - F(BYTE*, s, 12) - 1; }
static int Nat_VecLen(BYTE* v) { return (F(BYTE*, v, 24) - F(BYTE*, v, 20)) / 12; }

// string-length, vector-length: the length of the argument of the type
static SchValue* Nat_Length(SchValue* out, BYTE* p, BYTE** args, const char* where, int type, const char* typeName,
                            int (*len)(BYTE*))
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, where))
        Nat_Zero3(out);
    else if ((BYTE)v.flags == type) {
        SchValue h = {0, 0, 0, 0}, t;
        Nat_Typed(&h, &v, type, typeName);
        Nat_Check(&h);
        int zero = 0;
        unsigned n = len(h.obj);
        CopyRef(out, CallC<SchValue*>(FUN_4a24e0, &t, 0, &zero, &n));
        Nat_Release(&t);
        Nat_Release(&h);
    } else
        Nat_Zero3(out);
    SchValue_Release(&v);
    return out;
}

// procedure?: types 4 and 5
static SchValue* Nat_IsProcedure(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, (const char*)s_nProcP))
        Nat_Zero3(out);
    else
        Nat_Bool(out, (BYTE)v.flags == 4 || (BYTE)v.flags == 5);
    SchValue_Release(&v);
    return out;
}

// input-port?, output-port?: the port's flags (+8) & mask
static SchValue* Nat_PortIs(SchValue* out, BYTE* p, BYTE** args, const char* where, int mask)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, where)) {
        Nat_Zero3(out);
        SchValue_Release(&v);
        return out;
    }
    if ((BYTE)v.flags == 11) {
        SchValue h = {0, 0, 0, 0};
        Nat_Typed(&h, &v, 11, s_nPort);
        Nat_Check(&h);
        Nat_Bool(out, F(DWORD, h.obj, 8) & mask);
        if (!(h.flags & SCH_IMMEDIATE) && h.obj && --F(int, h.obj, 4) == 0)
            CallVirt(h.obj, 12, 1);
    } else
        Nat_Bool(out, 0);
    SchValue_Release(&v);
    return out;
}

// open-input-file, open-output-file: a port on the file named by the string (mode 1 or 2)
static SchValue* Nat_OpenFile(SchValue* out, BYTE* p, BYTE** args, const char* where, int mode)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, where)) {
        Nat_Zero3(out);
        SchValue_Release(&v);
        return out;
    }
    if ((BYTE)v.flags != 3) {
        CallC(FUN_4a5b50, p, 15, (const char*)s_nString, where);
        CallThis(FUN_4bd4d0, out);
        Nat_Release(&v);
        return out;
    }
    SchValue hs, hp, t;
    CallThis(FUN_4bd4d0, &hs);
    Nat_Typed(&hs, &v, 3, s_nString);
    Nat_Check(&hs);
    char* name = F(char*, hs.obj, 12);
    SchValue* r = CallC<SchValue*>(FUN_4557f0, &t, 0, &name, &mode);
    CallThis(FUN_4bd4d0, &hp);
    if (r->obj) {
        if (CallVirt<int>(r->obj, 0) == 11) {
            hp.obj = r->obj;
            hp.flags = r->flags;
            F(int, r->obj, 4)++;
        } else
            CallC(FUN_4a54e0, 2, 1, CallVirt<const char*>(r->obj, 4), (const char*)s_nPort);
    } else {
        hp.obj = 0;
        hp.flags = 0;
    }
    Nat_Release(&t);
    Nat_Check(&hp);
    if (CallVirt<int>(hp.obj, 40)) {
        Nat_Check(&hs);
        CallC(FUN_4a5a60, p, 8, F(char*, hs.obj, 12));
        Nat_Zero3(out);
        Nat_Release(&hp);
        Nat_Release(&hs);
        SchValue_Release(&v);
        return out;
    }
    CallThis(FUN_454580, out, &hp);
    Nat_Release(&hp);
    Nat_Release(&hs);
    Nat_Release(&v);
    return out;
}

// close-input-port, close-output-port: the port's close (vtbl+16)
static SchValue* Nat_ClosePort(SchValue* out, BYTE* p, BYTE** args, const char* where)
{
    SchValue v = {0, 0, 0, 0};
    if (!CallC<int>(FUN_491c00, p, &v, args, where) && (BYTE)v.flags == 11) {
        SchValue h = {0, 0, 0, 0};
        Nat_Typed(&h, &v, 11, s_nPort);
        Nat_Check(&h);
        CallVirt(h.obj, 16);
        if (!(h.flags & SCH_IMMEDIATE) && h.obj && --F(int, h.obj, 4) == 0)
            CallVirt(h.obj, 12, 1);
    }
    Nat_Zero3(out);
    SchValue_Release(&v);
    return out;
}

// read-char, peek-char, char-ready?: make(the port's method at off) of the port argument or the current input
// port (+72 of p); a non-port is reported when report
static SchValue* Nat_PortGet(SchValue* out, BYTE* p, BYTE** args, const char* where, int off, void (*make)(), bool report)
{
    if ((unsigned)Nat_Count(args) <= 1) {
        Nat_Check((SchValue*)(p + 72));
        CallC(make, out, 0, CallVirt<int>(F(BYTE*, p, 72), off));
        return out;
    }
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, where)) {
        Nat_Zero3(out);
        SchValue_Release(&v);
        return out;
    }
    if ((BYTE)v.flags == 11) {
        SchValue h = {0, 0, 0, 0};
        Nat_Typed(&h, &v, 11, s_nPort);
        Nat_Check(&h);
        CallC(make, out, 0, CallVirt<int>(h.obj, off));
        Nat_Release(&h);
        SchValue_Release(&v);
        return out;
    }
    if (report)
        CallC(FUN_4a5b50, p, 15, (const char*)s_nPort, where);
    SchValue_Release(&v);
    Nat_Zero3(out);
    return out;
}

// $L_49e0b0 .. : a copy of the value at off of p
static SchValue* Nat_Field(SchValue* out, BYTE* p, int off)
{
    CopyRef(out, (SchValue*)(p + off));
    return out;
}

extern "C" {

// [$L_49c730] string-length
SchValue* __cdecl Nat_StringLength(SchValue* out, BYTE* p, BYTE** args)
{
    return Nat_Length(out, p, args, s_nStrLen, 3, s_nString, Nat_StrLen);
}

// $L_49c990: string-ref
SchValue* __cdecl Nat_StringRef(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue x = {0, 0, 0, 0}, y = {0, 0, 0, 0};
    if (CallC<int>(FUN_491aa0, p, &x, &y, args, (const char*)s_nStrRef)) {
        Nat_Zero3(out);
        SchValue_Release(&y);
        SchValue_Release(&x);
        return out;
    }
    if ((BYTE)x.flags == 3 && (BYTE)y.flags == 6) {
        SchValue hs, hn;
        CallThis(FUN_4bd4d0, &hs);
        Nat_Typed(&hs, &x, 3, s_nString);
        CallThis(FUN_4bd4d0, &hn);
        Nat_Typed(&hn, &y, 6, s_nNumber);
        Nat_Check(&hn);
        unsigned i = CallThis<int>(FUN_4b0ea0, hn.obj);
        Nat_Check(&hs);
        if (i >= (unsigned)Nat_StrLen(hs.obj)) {
            CallC(FUN_4a5a60, p, 23, (const char*)s_nStrRef);
            Nat_Release(&hn);
            Nat_Release(&hs);
            Nat_Zero3(out);
        } else {
            Nat_Check(&hs);
            CallC(FUN_49c210, out, 0, (int)F(signed char*, hs.obj, 12)[i]);
            Nat_Release(&hn);
            Nat_Release(&hs);
        }
    } else {
        CallC(FUN_4a5a60, p, 10, (const char*)s_nStrRef);
        Nat_Zero3(out);
    }
    Nat_Release(&y);
    Nat_Release(&x);
    return out;
}

// $L_49cca0: string-set!
SchValue* __cdecl Nat_StringSet(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue a = {0, 0, 0, 0}, b = {0, 0, 0, 0}, c = {0, 0, 0, 0};
    if (CallC<int>(FUN_4918c0, p, &a, &b, &c, args, (const char*)s_nStrSet)) {
        Nat_Zero3(out);
        SchValue_Release(&c);
        SchValue_Release(&b);
        SchValue_Release(&a);
        return out;
    }
    if ((BYTE)a.flags == 3 && (BYTE)b.flags == 6 && (BYTE)c.flags == 8) {
        SchValue hs, hn, ch;
        CallThis(FUN_4bd4d0, &hs);
        Nat_Typed(&hs, &a, 3, s_nString);
        CallThis(FUN_4bd4d0, &hn);
        Nat_Typed(&hn, &b, 6, s_nNumber);
        CallThis(FUN_4a19c0, &ch, &c);
        Nat_Check(&hn);
        unsigned i = CallThis<int>(FUN_4b0ea0, hn.obj);
        if (i >= (unsigned)Nat_StrLen(CallThis<BYTE*>(FUN_4aa430, &hs)))
            CallC(FUN_4a5a60, p, 23, (const char*)s_nStrSet);
        else
            F(BYTE*, CallThis<BYTE*>(FUN_4aa430, &hs), 12)[i] = (BYTE)(DWORD)ch.obj;
        Nat_Release(&ch);
        Nat_Release(&hn);
        Nat_Release(&hs);
    } else
        CallC(FUN_4a5a60, p, 10, (const char*)s_nStrSet);
    Nat_Zero3(out);
    Nat_Release(&c);
    Nat_Release(&b);
    Nat_Release(&a);
    return out;
}

// $L_49cfe0: vector?
SchValue* __cdecl Nat_VectorP(SchValue* out, BYTE* p, BYTE** args) { return Nat_IsKind(out, p, args, s_nVectorP, 10); }

// $L_49d100: make-vector (count [, fill])
SchValue* __cdecl Nat_MakeVector(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue n = {0, 0, 0, 0};
    CopyRef(&n, (SchValue*)(args[0] + 12));
    if ((BYTE)n.flags != 6) {
        CallC(FUN_4a5910, p, 21);
        Nat_Zero3(out);
        SchValue_Release(&n);
        return out;
    }
    SchValue fill = {0, 0, 0, 0};
    int want = 2;
    if (Nat_Count(args) != 2) {
        CopyRef(&fill, (SchValue*)(args[0] + 24));
        want = 3;
    }
    if (want != Nat_Count(args))
        CallC(FUN_4a5a60, p, 33, (const char*)s_nMakeVector);
    SchValue h, t;
    CallThis(FUN_4bd4d0, &h);
    Nat_Typed(&h, &n, 6, s_nNumber);
    Nat_Check(&h);
    int k = CallThis<int>(FUN_4b0ea0, h.obj);
    CallC(FUN_4a25d0, &t, 0, p, &k, &fill);
    CallThis(FUN_454580, out, &t);
    Nat_Release(&t);
    Nat_Release(&h);
    Nat_Release(&fill);
    Nat_Release(&n);
    return out;
}

// $L_49d3b0: list->vector
SchValue* __cdecl Nat_ListToVector(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, (const char*)s_nListToVector)) {
        Nat_Zero3(out);
        SchValue_Release(&v);
        return out;
    }
    if ((BYTE)v.flags != 2) {
        CallC(FUN_4a5a60, p, 21, (const char*)s_nListToVector);
        Nat_Zero3(out);
        SchValue_Release(&v);
        return out;
    }
    SchValue vec, cur;
    CallC(FUN_4a2700, &vec, 0, p);
    CopyRef(&cur, &v);
    while (cur.flags || cur.obj) {
        if ((BYTE)cur.flags == 2) {
            SchValue car;
            SchValue* c = CallC<SchValue*>(FUN_407f00, &car, &cur);
            Nat_Check(&vec);
            CallThis(FUN_4a16d0, vec.obj + 20, c);
            Nat_Release(&car);
        } else
            CallC(FUN_4a5a60, p, 17, (const char*)s_nListToVector);
        SchValue cdr;
        CallThis(FUN_40c100, &cur, CallC<SchValue*>(FUN_4090a0, &cdr, &cur));
        SchValue_Release(&cdr);
    }
    CallThis(FUN_454580, out, &vec);
    Nat_Release(&cur);
    Nat_Release(&vec);
    Nat_Release(&v);
    return out;
}

// $L_49d660: vector-length
SchValue* __cdecl Nat_VectorLength(SchValue* out, BYTE* p, BYTE** args)
{
    return Nat_Length(out, p, args, s_nVecLen, 10, s_nVector, Nat_VecLen);
}

// $L_49d8d0: vector-ref
SchValue* __cdecl Nat_VectorRef(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue x = {0, 0, 0, 0}, y = {0, 0, 0, 0};
    if (CallC<int>(FUN_491aa0, p, &x, &y, args, (const char*)s_nVecRef)) {
        Nat_Zero3(out);
        SchValue_Release(&y);
        SchValue_Release(&x);
        return out;
    }
    if ((BYTE)x.flags == 10 && (BYTE)y.flags == 6) {
        SchValue hv, hn;
        CallThis(FUN_4bd4d0, &hv);
        Nat_Typed(&hv, &x, 10, s_nVector);
        CallThis(FUN_4bd4d0, &hn);
        Nat_Typed(&hn, &y, 6, s_nNumber);
        Nat_Check(&hn);
        unsigned i = CallThis<int>(FUN_4b0ea0, hn.obj);
        Nat_Check(&hv);
        if (i < (unsigned)Nat_VecLen(hv.obj)) {
            Nat_Check(&hv);
            CallThis(FUN_454580, out, F(BYTE*, hv.obj, 20) + i * 12);
            Nat_Release(&hn);
            Nat_Release(&hv);
            SchValue_Release(&y);
            Nat_Release(&x);
            return out;
        }
        CallC(FUN_4a5a60, p, 23, (const char*)s_nVecRef);
        Nat_Release(&hn);
        Nat_Release(&hv);
    } else
        CallC(FUN_4a5a60, p, 10, (const char*)s_nVecRef);
    CallThis(FUN_4bd4d0, out);
    Nat_Release(&y);
    Nat_Release(&x);
    return out;
}

// $L_49dc30: vector-set
SchValue* __cdecl Nat_VectorSet(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue a = {0, 0, 0, 0}, b = {0, 0, 0, 0}, c = {0, 0, 0, 0};
    if (CallC<int>(FUN_4918c0, p, &a, &b, &c, args, (const char*)s_nVecSet)) {
        Nat_Zero3(out);
        SchValue_Release(&c);
        SchValue_Release(&b);
        SchValue_Release(&a);
        return out;
    }
    if ((BYTE)a.flags == 10 && (BYTE)b.flags == 6) {
        SchValue hv, hn;
        CallThis(FUN_4bd4d0, &hv);
        Nat_Typed(&hv, &a, 10, s_nVector);
        CallThis(FUN_4bd4d0, &hn);
        Nat_Typed(&hn, &b, 6, s_nNumber);
        Nat_Check(&hn);
        unsigned i = CallThis<int>(FUN_4b0ea0, hn.obj);
        Nat_Check(&hv);
        if (i >= (unsigned)Nat_VecLen(hv.obj))
            CallC(FUN_4a5a60, p, 23, (const char*)s_nVecSet);
        else
            CallThis(FUN_40c100, F(BYTE*, CallThis<BYTE*>(FUN_4aa430, &hv), 20) + i * 12, &c);
        Nat_Release(&hn);
        Nat_Release(&hv);
    } else
        CallC(FUN_4a5a60, p, 10, (const char*)s_nVecSet);
    CallThis(FUN_4bd4d0, out);
    Nat_Release(&c);
    Nat_Release(&b);
    Nat_Release(&a);
    return out;
}

// $L_49df70: procedure?
SchValue* __cdecl Nat_ProcedureP(SchValue* out, BYTE* p, BYTE** args) { return Nat_IsProcedure(out, p, args); }
// $L_49e0b0: current-input-port (+48), $L_49e1e0: +60, $L_49e6a0: +72, $L_49e700: +84
SchValue* __cdecl Nat_Field48(SchValue* out, BYTE* p) { return Nat_Field(out, p, 48); }
SchValue* __cdecl Nat_Field60(SchValue* out, BYTE* p) { return Nat_Field(out, p, 60); }
SchValue* __cdecl Nat_Field72(SchValue* out, BYTE* p) { return Nat_Field(out, p, 72); }
SchValue* __cdecl Nat_Field84(SchValue* out, BYTE* p) { return Nat_Field(out, p, 84); }

// $L_49e110: a new environment (of p)
SchValue* __cdecl Nat_MakeEnv(SchValue* out, BYTE* p)
{
    SchValue t;
    CopyRef(out, CallC<SchValue*>(FUN_4a27e0, &t, 0, p));
    SchValue_Release(&t);
    return out;
}

// $L_49e240: input-port?, $L_49e470: output-port?
SchValue* __cdecl Nat_InputPortP(SchValue* out, BYTE* p, BYTE** args) { return Nat_PortIs(out, p, args, s_nInPortP, 1); }
SchValue* __cdecl Nat_OutputPortP(SchValue* out, BYTE* p, BYTE** args) { return Nat_PortIs(out, p, args, s_nOutPortP, 2); }

// [$L_49e810] open-input-file, $L_49eb00: open-output-file
SchValue* __cdecl Nat_OpenInputFile(SchValue* out, BYTE* p, BYTE** args) { return Nat_OpenFile(out, p, args, s_nOpenIn, 1); }
SchValue* __cdecl Nat_OpenOutputFile(SchValue* out, BYTE* p, BYTE** args) { return Nat_OpenFile(out, p, args, s_nOpenOut, 2); }
// $L_49edf0: close-input-port, $L_49efd0: close-output-port
SchValue* __cdecl Nat_CloseInputPort(SchValue* out, BYTE* p, BYTE** args) { return Nat_ClosePort(out, p, args, s_nCloseIn); }
SchValue* __cdecl Nat_CloseOutputPort(SchValue* out, BYTE* p, BYTE** args) { return Nat_ClosePort(out, p, args, s_nCloseOut); }
// $L_49f1b0: read-char, $L_49f3f0: peek-char
SchValue* __cdecl Nat_ReadChar(SchValue* out, BYTE* p, BYTE** args) { return Nat_PortGet(out, p, args, s_nReadChar, 20, FUN_49c210, false); }
SchValue* __cdecl Nat_PeekChar(SchValue* out, BYTE* p, BYTE** args) { return Nat_PortGet(out, p, args, s_nPeekChar, 24, FUN_49c210, true); }

// $L_49f6a0: eof-object? (true for a non-character)
SchValue* __cdecl Nat_EofObjectP(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, (const char*)s_nEofP))
        Nat_Zero3(out);
    else if ((BYTE)v.flags == 8) {
        SchValue c;
        CallThis(FUN_4a19c0, &c, &v);
        Nat_Bool(out, (int)c.obj == -1);
        SchValue_Release(&c);
    } else
        Nat_Bool(out, -1);
    SchValue_Release(&v);
    return out;
}

// $L_49f850: char-ready?
SchValue* __cdecl Nat_CharReadyP(SchValue* out, BYTE* p, BYTE** args) { return Nat_PortGet(out, p, args, s_nCharReady, 32, FUN_492250, false); }

// $L_49fa90: write-char (char [, port])
SchValue* __cdecl Nat_WriteChar(SchValue* out, BYTE* p, BYTE** args)
{
    BYTE* first = args[0];
    int n = Nat_Count(args);
    if ((unsigned)n < 2)
        CallC(FUN_4a5a60, p, 33, (const char*)s_nWriteChar);
    else if (F(BYTE, first, 20) != 8)
        CallC(FUN_4a5b50, p, 15, (const char*)s_nChar, (const char*)s_nWriteChar);
    else if (n == 2) {
        SchValue c;
        CallThis(FUN_4a19c0, &c, first + 12);
        Nat_Check((SchValue*)(p + 84));
        CallVirt(F(BYTE*, p, 84), 28, c.obj);
        Nat_Release(&c);
    } else if (n == 3) {
        SchValue a = {0, 0, 0, 0}, b = {0, 0, 0, 0};
        if (CallC<int>(FUN_491aa0, p, &a, &b, args, (const char*)s_nWriteChar)) {
            Nat_Zero3(out);
            SchValue_Release(&b);
            SchValue_Release(&a);
            return out;
        }
        if ((BYTE)b.flags == 11) {
            SchValue c, h;
            CallThis(FUN_4a19c0, &c, &a);
            CallThis(FUN_4bd4d0, &h);
            Nat_Typed(&h, &b, 11, s_nPort);
            Nat_Check(&h);
            CallVirt(h.obj, 28, c.obj);
            Nat_Release(&h);
            Nat_Release(&c);
        } else
            CallC(FUN_4a5b50, p, 15, (const char*)s_nPort, (const char*)s_nWriteChar);
        SchValue_Release(&b);
        Nat_Release(&a);
    } else
        CallC(FUN_4a5a60, p, 33, (const char*)s_nWriteChar);
    Nat_Zero3(out);
    return out;
}

// $L_49fdd0: read ([port]; the current input port +72 of p)
SchValue* __cdecl Nat_Read(SchValue* out, BYTE* p, BYTE** args)
{
    if ((unsigned)Nat_Count(args) <= 1) {
        SchValue12 port;
        CallThis(FUN_454580, &port, p + 72);
        CallC(FUN_4b4c20, out, p, port);
        return out;
    }
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, (const char*)s_nRead)) {
        Nat_Zero3(out);
        SchValue_Release(&v);
        return out;
    }
    if ((BYTE)v.flags == 11) {
        SchValue h = {0, 0, 0, 0};
        Nat_Typed(&h, &v, 11, s_nPort);
        SchValue12 port;
        CallThis(FUN_454580, &port, &h);
        CallC(FUN_4b4c20, out, p, port);
        SchValue_Release(&h);
        SchValue_Release(&v);
        return out;
    }
    SchValue_Release(&v);
    Nat_Zero3(out);
    return out;
}

// $L_4a0040: write (value [, port]; the current output port +84 of p)
SchValue* __cdecl Nat_Write(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue x, y, t;
    CopyRef(&x, (SchValue*)(args[0] + 12));
    if (Nat_Count(args) > 2)
        CopyRef(&y, (SchValue*)(args[0] + 24));
    else {
        Nat_Zero3(&t);
        CopyRef(&y, &t);
    }
    if ((BYTE)y.flags == 11) {
        if (Nat_Count(args) == 3) {
            SchValue12 port = {{0, 0, 0}};
            Nat_Typed((SchValue*)&port, &y, 11, s_nPort);
            SchValue12 val;
            CallThis(FUN_454580, &val, &x);
            CallC(FUN_4b8070, &t, p, val, port);
            SchValue_Release(&t);
        } else
            CallC(FUN_4a5a60, p, 33, (const char*)s_nWrite);
    } else {
        SchValue12 port, val;
        CallThis(FUN_454580, &port, p + 84);
        CallThis(FUN_454580, &val, &x);
        CallC(FUN_4b8070, &t, p, val, port);
        SchValue_Release(&t);
    }
    Nat_Zero3(out);
    Nat_Release(&y);
    Nat_Release(&x);
    return out;
}

// $L_4a0330: internal-macro-expand (the argument itself)
SchValue* __cdecl Nat_MacroExpand(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, (const char*)s_nMacroExpand))
        Nat_Zero3(out);
    else
        CopyRef(out, &v);
    SchValue_Release(&v);
    return out;
}

// $L_4a0460: replaces the values of p (+4224) by the arguments after the first, and reports message 1
SchValue* __cdecl Nat_SetValues(SchValue* out, BYTE* p, BYTE** args)
{
    BYTE** vec = (BYTE**)(p + 4224);
    BYTE* begin = vec[0];
    BYTE* end = vec[1];
    BYTE* e = vec[1];
    for (BYTE* q = CallC<BYTE*>(FUN_4a2be0, end, end, begin, args, 0); q != e; q += 12)
        CallThis(FUN_4a36c0, q);
    vec[1] = vec[1] - (end - begin) / 12 * 12;
    CallThis(FUN_4a1a40, vec, vec[1], args[0] + 12, args[1]);
    CallC(FUN_4a5910, p, 1);
    Nat_Zero3(out);
    return out;
}

// $L_4a0520: string-append (two strings)
SchValue* __cdecl Nat_StringAppend(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue a = {0, 0, 0, 0}, b = {0, 0, 0, 0};
    if (CallC<int>(FUN_491aa0, p, &a, &b, args, (const char*)s_nStrAppend)) {
        Nat_Zero3(out);
        SchValue_Release(&b);
        SchValue_Release(&a);
        return out;
    }
    if ((BYTE)a.flags == 3 && (BYTE)b.flags == 3) {
        SchValue ha, hb, r;
        CallThis(FUN_4bd4d0, &ha);
        Nat_Typed(&ha, &a, 3, s_nString);
        CallThis(FUN_4bd4d0, &hb);
        Nat_Typed(&hb, &b, 3, s_nString);
        CallC(FUN_4a2400, &r, 0);
        Nat_Check(&ha);
        BYTE* last = F(BYTE*, ha.obj, 16);
        Nat_Check(&ha);
        BYTE* first = F(BYTE*, ha.obj, 12);
        Nat_Check(&r);
        BYTE* pos = F(BYTE*, r.obj, 16);
        CallThis(FUN_44c220, CallThis<BYTE*>(FUN_4aa430, &r) + 12, pos, first, last);
        F(BYTE*, CallThis<BYTE*>(FUN_4aa430, &r), 16) -= 1;
        last = F(BYTE*, CallThis<BYTE*>(FUN_4aa430, &hb), 16);
        first = F(BYTE*, CallThis<BYTE*>(FUN_4aa430, &hb), 12);
        pos = F(BYTE*, CallThis<BYTE*>(FUN_4aa430, &r), 16);
        CallThis(FUN_44c220, CallThis<BYTE*>(FUN_4aa430, &r) + 12, pos, first, last);
        CallThis(FUN_454580, out, &r);
        Nat_Release(&r);
        Nat_Release(&hb);
        Nat_Release(&ha);
    } else {
        CallC(FUN_4a5a60, p, 10, (const char*)s_nStrAppend);
        CallThis(FUN_4bd4d0, out);
    }
    Nat_Release(&b);
    Nat_Release(&a);
    return out;
}

}  // extern "C"

// ---------------------------------------------------------------------------
// not, null?, list and caar .. cddr.

extern "C" void FUN_409070(); KING_ALIAS("_FUN_409070", "_Str_Same")
extern "C" void FUN_4a28c0(); KING_ALIAS("_FUN_4a28c0", "_SchValue_MakePairB")
KING_VAR(AsmLabel, s_nNot, 670a04)
KING_VAR(AsmLabel, s_nNullP, 670a08)
KING_VAR(AsmLabel, s_nCaar, 670a10)
KING_VAR(AsmLabel, s_nCadr, 670a18)
KING_VAR(AsmLabel, s_nCdar, 670a20)
KING_VAR(AsmLabel, s_nCddr, 670a28)

// the value at d = s, keeping d's object when s is the same value
static void Nat_AssignSame(SchValue* d, const SchValue* s)
{
    if (d->obj && !(d->flags & SCH_IMMEDIATE) && --F(int, d->obj, 4) == 0
        && !(d->obj == s->obj && d->flags == s->flags && d->m_4 == s->m_4) && d->obj)
        CallVirt(d->obj, 12, 1);
    CopyRef(d, s);
}

// the same, comparing with Str_Same
static void Nat_AssignStr(SchValue* d, const SchValue* s)
{
    if (d->obj && !(d->flags & SCH_IMMEDIATE) && --F(int, d->obj, 4) == 0
        && !CallC<char>(FUN_409070, d, s) && d->obj)
        CallVirt(d->obj, 12, 1);
    CopyRef(d, s);
}

// caar .. cddr: second(first(the pair))
static SchValue* Nat_Cxxr(SchValue* out, BYTE* p, BYTE** args, const char* where, void (*first)(), void (*second)())
{
    BYTE* a = args[0];
    if ((args[1] - a) / 12 != 2) {
        CallC(FUN_4a5a60, p, 33, where);
        Nat_Zero3(out);
        return out;
    }
    if (F(BYTE, a, 20) == 2) {
        SchValue t;
        bool pair = (BYTE)CallC<SchValue*>(first, &t, a + 12)->flags == 2;
        SchValue_Release(&t);
        if (pair) {
            CallC(second, out, CallC<SchValue*>(first, &t, args[0] + 12));
            SchValue_Release(&t);
            return out;
        }
    }
    CallC(FUN_4a5b50, p, 15, (const char*)s_nPair, where);
    Nat_Zero3(out);
    return out;
}

extern "C" {

// [$L_4a0cc0] not
SchValue* __cdecl Nat_Not(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, (const char*)s_nNot))
        Nat_Zero3(out);
    else
        Nat_Bool(out, (BYTE)v.flags == 7 ? v.obj == 0 : 0);
    SchValue_Release(&v);
    return out;
}

// $L_4a0e30: null?
SchValue* __cdecl Nat_NullP(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, (const char*)s_nNullP))
        Nat_Zero3(out);
    else
        Nat_Bool(out, !v.obj && !v.flags && !v.m_4);
    SchValue_Release(&v);
    return out;
}

// $L_4a0f70: list (the arguments, consed from the last)
SchValue* __cdecl Nat_List(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue acc = {0, 0, 0, 0};
    for (int i = Nat_Count(args) - 1; i; i--) {
        SchValue pr;
        CallC(FUN_4a28c0, &pr, 0, p);
        BYTE* e = args[0] + i * 12;
        if (!pr.obj)
            CallC(FUN_4a52e0, 0, 1);
        if (pr.flags & SCH_IMMEDIATE)
            CallC(FUN_4a52e0, 4, 1);
        Nat_AssignSame((SchValue*)(pr.obj + 20), (const SchValue*)e);
        if (!pr.obj)
            CallC(FUN_4a52e0, 0, 1);
        if (pr.flags & SCH_IMMEDIATE)
            CallC(FUN_4a52e0, 4, 1);
        Nat_AssignStr((SchValue*)(pr.obj + 32), &acc);
        CallThis(FUN_40c100, &acc, &pr);
        Nat_Release(&pr);
    }
    CallThis(FUN_454580, out, &acc);
    Nat_Release(&acc);
    return out;
}

// $L_4a11c0: caar, $L_4a1300: cadr, $L_4a1440: cdar, $L_4a1580: cddr
SchValue* __cdecl Nat_Caar(SchValue* out, BYTE* p, BYTE** args) { return Nat_Cxxr(out, p, args, s_nCaar, FUN_407f00, FUN_407f00); }
SchValue* __cdecl Nat_Cadr(SchValue* out, BYTE* p, BYTE** args) { return Nat_Cxxr(out, p, args, s_nCadr, FUN_4090a0, FUN_407f00); }
SchValue* __cdecl Nat_Cdar(SchValue* out, BYTE* p, BYTE** args) { return Nat_Cxxr(out, p, args, s_nCdar, FUN_407f00, FUN_4090a0); }
SchValue* __cdecl Nat_Cddr(SchValue* out, BYTE* p, BYTE** args) { return Nat_Cxxr(out, p, args, s_nCddr, FUN_4090a0, FUN_4090a0); }

}  // extern "C"

// ---------------------------------------------------------------------------
// eq?

extern "C" void FUN_44ca00(); KING_ALIAS("_FUN_44ca00", "@Handle9_Init@12")         // an identifier handle from a value
extern "C" void FUN_455760(); KING_ALIAS("_FUN_455760", "@StringHandle_From@12")
extern "C" void FUN_4921d0(); KING_ALIAS("_FUN_4921d0", "@CharHandle_From@12")
extern "C" void FUN_4b0fb0(); KING_ALIAS("_FUN_4b0fb0", "_SchNumber_Eq")
KING_VAR(AsmLabel, s_nEqP, 670658)

extern "C" {

// [$L_492270] eq?: the same value; else equal booleans, identifiers (case-insensitive), strings, characters or
// numbers
SchValue* __cdecl Nat_EqP(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue x = {0, 0, 0, 0}, y = {0, 0, 0, 0};
    if (CallC<int>(FUN_491aa0, p, &x, &y, args, (const char*)s_nEqP)) {
        Nat_Zero3(out);
        SchValue_Release(&y);
        SchValue_Release(&x);
        return out;
    }
    int r = -2;
    if (x.obj == y.obj && x.flags == y.flags && x.m_4 == y.m_4)
        r = -1;
    else if (!x.obj && !x.flags && !x.m_4)
        r = 0;
    else if (!y.obj && !y.flags && !y.m_4)
        r = 0;
    else if ((BYTE)(x.flags ^ y.flags))
        r = 0;
    if (r != -2) {
        Nat_Bool(out, r);
        SchValue_Release(&y);
        SchValue_Release(&x);
        return out;
    }
    SchValue hx, hy;
    switch ((BYTE)x.flags) {
    case 7:
        SchValue_AsBool(&hx, 0, &x);
        SchValue_AsBool(&hy, 0, &y);
        Nat_Bool(out, (hx.obj != 0) == (hy.obj != 0));
        Nat_Release(&hy);
        Nat_Release(&hx);
        SchValue_Release(&y);
        Nat_Release(&x);
        return out;
    case 9:
        CallThis(FUN_44ca00, &hx, &x);
        CallThis(FUN_44ca00, &hy, &y);
        {
            const char* sy = F(const char*, CallThis<BYTE*>(FUN_4aa430, &hy), 12);
            const char* sx = F(const char*, CallThis<BYTE*>(FUN_4aa430, &hx), 12);
            Nat_Bool(out, _stricmp(sx, sy) == 0);
        }
        Nat_Release(&hy);
        Nat_Release(&hx);
        break;
    case 3:
        CallThis(FUN_455760, &hx, &x);
        CallThis(FUN_455760, &hy, &y);
        {
            const char* sy = F(const char*, CallThis<BYTE*>(FUN_4aa430, &hy), 12);
            const char* sx = F(const char*, CallThis<BYTE*>(FUN_4aa430, &hx), 12);
            Nat_Bool(out, strcmp(sx, sy) == 0);
        }
        Nat_Release(&hy);
        Nat_Release(&hx);
        break;
    case 8:
        CallThis(FUN_4921d0, &hx, &x);
        CallThis(FUN_4921d0, &hy, &y);
        Nat_Bool(out, hx.obj == hy.obj);
        Nat_Release(&hy);
        Nat_Release(&hx);
        break;
    case 6:
        CallThis(FUN_4556d0, &hx, &x);
        CallThis(FUN_4556d0, &hy, &y);
        Nat_Bool(out, CallC<BYTE>(FUN_4b0fb0, hx.obj, hy.obj));
        Nat_Release(&hy);
        Nat_Release(&hx);
        break;
    default:
        Nat_Bool(out, 0);
        break;
    }
    Nat_Release(&y);
    Nat_Release(&x);
    return out;
}

}  // extern "C"

// ---------------------------------------------------------------------------
// equal?

extern "C" void FUN_407f60(); KING_ALIAS("_FUN_407f60", "_Str_Member20Checked")
extern "C" void FUN_407ff0(); KING_ALIAS("_FUN_407ff0", "_Str_Member32Checked")
extern "C" void FUN_44cab0(); KING_ALIAS("_FUN_44cab0", "@PairHandle_From@12")
extern "C" void FUN_4a16c0(); KING_ALIAS("_FUN_4a16c0", "@Vec12_At@12")

// a value as the compiler passes it by value: the callee releases it
struct SchArg {
    BYTE* obj;
    int   m_4;
    DWORD flags;
};

extern "C" int __cdecl Sch_Equal(SchArg a, SchArg b, int extra);

static int Equal_Elems(SchValue* ha, SchValue* hb, int i, int extra)
{
    SchArg ea, eb;
    CallThis(FUN_454580, &eb, CallThis<BYTE*>(FUN_4a16c0, CallThis<BYTE*>(FUN_4aa430, hb) + 20, i));
    CallThis(FUN_454580, &ea, CallThis<BYTE*>(FUN_4a16c0, CallThis<BYTE*>(FUN_4aa430, ha) + 20, i));
    return Sch_Equal(ea, eb, extra);
}

static int Vec_Len(SchValue* h)
{
    BYTE* o = CallThis<BYTE*>(FUN_4aa430, h);
    return (F(BYTE*, o, 24) - F(BYTE*, o, 20)) / 12;
}

extern "C" {

// [FUN_4927d0] equal?: identical values, else the same type compared by content (booleans, identifiers ignoring
// case, strings, characters, numbers, vectors and pairs element by element); nonzero when equal
int __cdecl Sch_Equal(SchArg a, SchArg b, int extra)
{
    SchValue* pa = (SchValue*)&a;
    SchValue* pb = (SchValue*)&b;
    if (a.obj == b.obj && a.flags == b.flags && a.m_4 == b.m_4) {
        Nat_Release(pa);
        Nat_Release(pb);
        return -1;
    }
    if ((!a.obj && !a.m_4 && !a.flags) || (!b.obj && !b.m_4 && !b.flags) || (BYTE)(a.flags ^ b.flags)) {
        Nat_Release(pa);
        Nat_Release(pb);
        return 0;
    }
    SchValue ha, hb;
    int r = 0;
    switch ((BYTE)a.flags) {
    case 7:
        SchValue_AsBool(&ha, 0, pa);
        SchValue_AsBool(&hb, 0, pb);
        r = (ha.obj != 0) == (hb.obj != 0);
        break;
    case 9:
        CallThis(FUN_44ca00, &hb, pa);
        CallThis(FUN_44ca00, &ha, pb);
        {
            const char* sa = F(const char*, CallThis<BYTE*>(FUN_4aa430, &ha), 12);
            const char* sb = F(const char*, CallThis<BYTE*>(FUN_4aa430, &hb), 12);
            r = _stricmp(sb, sa) == 0;
        }
        break;
    case 3:
        CallThis(FUN_455760, &hb, pa);
        CallThis(FUN_455760, &ha, pb);
        {
            const char* sa = F(const char*, CallThis<BYTE*>(FUN_4aa430, &ha), 12);
            const char* sb = F(const char*, CallThis<BYTE*>(FUN_4aa430, &hb), 12);
            r = strcmp(sb, sa) == 0;
        }
        break;
    case 8:
        CallThis(FUN_4921d0, &ha, pa);
        CallThis(FUN_4921d0, &hb, pb);
        r = ha.obj == hb.obj;
        break;
    case 10:
        Handle10_Init((void**)&ha, 0, (void**)pa);
        Handle10_Init((void**)&hb, 0, (void**)pb);
        if (Vec_Len(&hb) == Vec_Len(&ha)) {
            r = 1;
            for (int i = 0; i < Vec_Len(&ha) && r; i++)
                r = Equal_Elems(&ha, &hb, i, extra);
        }
        break;
    case 2:
        CallThis(FUN_44cab0, &ha, pa);
        CallThis(FUN_44cab0, &hb, pb);
        {
            SchArg ca, cb;
            CallC(FUN_407f60, &cb, &hb);
            CallC(FUN_407f60, &ca, &ha);
            if (Sch_Equal(ca, cb, extra)) {
                CallC(FUN_407ff0, &cb, &hb);
                CallC(FUN_407ff0, &ca, &ha);
                r = Sch_Equal(ca, cb, extra) != 0;
            }
        }
        break;
    case 6:
        CallThis(FUN_4556d0, &ha, pa);
        CallThis(FUN_4556d0, &hb, pb);
        r = CallC<BYTE>(FUN_4b0fb0, ha.obj, hb.obj);
        break;
    default:
        Nat_Release(pa);
        Nat_Release(pb);
        return 0;
    }
    Nat_Release(&hb);
    Nat_Release(&ha);
    Nat_Release(pa);
    Nat_Release(pb);
    return r;
}

}  // extern "C"

// === eqv? (FUN_491ce0)

KING_VAR(AsmLabel, s_eq670640, 670640)
extern "C" void FUN_492150(); KING_ALIAS("_FUN_492150", "@BoolHandle_From@12")
extern "C" void FUN_4921d0(); KING_ALIAS("_FUN_4921d0", "@CharHandle_From@12")
extern "C" void FUN_44ca00(); KING_ALIAS("_FUN_44ca00", "@Handle9_Init@12")
extern "C" void FUN_4b0fb0(); KING_ALIAS("_FUN_4b0fb0", "_SchNumber_Eq")

static bool Eq_Zero(const SchValue* v)
{
    return !v->obj && !v->m_4 && !v->flags;
}

extern "C" {

// [$L_491ce0] eqv?: the same value, or equal booleans, symbols (ignoring case), characters or numbers
SchValue* __cdecl Nat_Eqv(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue a = {0, 0, 0, 0}, b = {0, 0, 0, 0};
    if (CallC<int>(FUN_491aa0, p, &a, &b, args, (const char*)s_eq670640)) {
        out->obj = 0;
        out->m_4 = 0;
        out->flags = 0;
        Nat_Release(&b);
        Nat_Release(&a);
        return out;
    }
    int r = 0;
    if (a.obj == b.obj && a.flags == b.flags && a.m_4 == b.m_4)
        r = -1;
    else if (Eq_Zero(&a) || Eq_Zero(&b) || (BYTE)(a.flags ^ b.flags))
        r = 0;
    else {
        SchValue ha, hb;
        switch ((BYTE)a.flags) {
        case 7:
            CallThis(FUN_492150, &ha, &a);
            CallThis(FUN_492150, &hb, &b);
            r = (ha.obj != 0) == (hb.obj != 0);
            break;
        case 9:
            CallThis(FUN_44ca00, &hb, &a);
            CallThis(FUN_44ca00, &ha, &b);
            r = _stricmp(F(const char*, CallThis<BYTE*>(FUN_4aa430, &hb), 12),
                         F(const char*, CallThis<BYTE*>(FUN_4aa430, &ha), 12)) == 0;
            break;
        case 8:
            CallThis(FUN_4921d0, &ha, &a);
            CallThis(FUN_4921d0, &hb, &b);
            r = ha.obj == hb.obj;
            break;
        case 6:
            CallThis(FUN_4556d0, &ha, &a);
            CallThis(FUN_4556d0, &hb, &b);
            r = CallC<BYTE>(FUN_4b0fb0, ha.obj, hb.obj);
            break;
        default:
            CallC(FUN_492250, out, 0, 0);
            Nat_Release(&b);
            Nat_Release(&a);
            return out;
        }
        CallC(FUN_492250, out, 0, r);
        CallThis(FUN_4a36c0, &hb);
        CallThis(FUN_4a36c0, &ha);
        Nat_Release(&b);
        Nat_Release(&a);
        return out;
    }
    CallC(FUN_492250, out, 0, r);
    SchValue_Release(&b);
    SchValue_Release(&a);
    return out;
}

} // extern "C"

KING_VAR(AsmLabel, s_nMinus, 6706b0)
extern "C" void FUN_4b0f10(); KING_ALIAS("_FUN_4b0f10", "@SchNumber_Sub@12")

extern "C" {

// [$L_4954f0] -: the first argument minus the others (with one argument its negation)
SchValue* __cdecl Nat_Minus(SchValue* out, BYTE* p, BYTE** args)
{
    return Nat_Arith(out, p, args, s_nMinus, FUN_4b0f10, true, 0);
}

}  // extern "C"

// === defining a name in an environment (FUN_4a3180)

extern "C" void FUN_44c710(); KING_ALIAS("_FUN_44c710", "@StrRefVec_InsertAux@16")
extern "C" void FUN_460910(); KING_ALIAS("_FUN_460910", "_ConstructStrRef")
extern "C" void FUN_4a16d0(); KING_ALIAS("_FUN_4a16d0", "@Vec12_PushBackRef@12")
extern "C" void FUN_4a3650(); KING_ALIAS("_FUN_4a3650", "@SchValue_Copy@12")
extern "C" void FUN_4a44a0(); KING_ALIAS("_FUN_4a44a0", "@SchPair_Construct@16")
extern "C" void FUN_4a47a0(); KING_ALIAS("_FUN_4a47a0", "@SchMap_Construct@16")
extern "C" void FUN_4a4b00(); KING_ALIAS("_FUN_4a4b00", "@SchMap_InsertUnique@16")

// the name of symbol value v (checked: set, not immediate)
static const char* Df_Name(const SchValue* v)
{
    Nat_Check(v);
    return F(const char*, v->obj, 12);
}

static void Df_Release(SchValue* v)
{
    CallThis(FUN_4a36c0, v);
}

extern "C" {

// [FUN_4a3180] defines name as value in environment e (thiscall, both by value): up to 8 names live inline
// (+36, count +20) with the values in the vector +132, matched without case; then a map (+144) from the name
// to the value and its index takes over; a defined name gets the new value
void __fastcall Env_Define(BYTE* e, void*, BYTE* p, BYTE* n0, int n1, DWORD n2, BYTE* v0, int v1, DWORD v2)
{
    SchValue name = {n0, n1, n2, 0}, val = {v0, v1, v2, 0};
    BYTE** vals = (BYTE**)(e + 132);
    if (F(int, e, 20) < 8 && !F(BYTE*, e, 144)) {
        for (int i = 0; i < F(int, e, 20); i++) {
            const char* a = Df_Name((SchValue*)(e + 36 + i * 12));
            const char* b = Df_Name(&name);
            if (tolower(a[0]) != tolower(b[0]))
                continue;
            b = Df_Name(&name);
            a = Df_Name((SchValue*)(e + 36 + i * 12));
            if (_stricmp(a + 1, b + 1) == 0) {
                CallThis(FUN_40c100, vals[0] + i * 12, &val);
                Df_Release(&name);
                Df_Release(&val);
                return;
            }
        }
        CallThis(FUN_40c100, e + 36 + F(int, e, 20) * 12, &name);
        if (vals[1] != vals[2]) {
            CallC(FUN_460910, vals[1], &val);
            vals[1] += 12;
        } else
            CallThis(FUN_44c710, vals, vals[1], &val);
        F(int, e, 20)++;
        Df_Release(&name);
        Df_Release(&val);
        return;
    }
    if (!F(BYTE*, e, 144)) {
        BYTE* m = (BYTE*)operator new(12);
        BYTE x1, x2;
        if (m)
            m = CallThis<BYTE*>(FUN_4a47a0, m, &x2, &x1);
        F(BYTE*, e, 144) = m;
        for (int i = 0; i < F(int, e, 20); i++) {
            DWORD mapped[4];
            CallThis(FUN_454580, mapped, vals[0] + i * 12);
            mapped[3] = i;
            DWORD pair[7];
            CallThis(FUN_454580, pair, e + 36 + i * 12);
            CallThis(FUN_4a3650, pair + 3, mapped);
            DWORD res[2];
            CallThis(FUN_4a4b00, F(BYTE*, e, 144), res, pair);
            Df_Release((SchValue*)(pair + 3));
            Df_Release((SchValue*)pair);
            Df_Release((SchValue*)mapped);
        }
    }
    DWORD mapped[4];
    CallThis(FUN_454580, mapped, &val);
    mapped[3] = (int)(vals[1] - vals[0]) / 12;
    DWORD pair[7];
    CallThis(FUN_4a44a0, pair, &name, mapped);
    DWORD res[2];
    CallThis(FUN_4a4b00, F(BYTE*, e, 144), res, pair);
    CallThis(FUN_4a36f0, pair);
    BYTE* node = (BYTE*)res[0];
    if ((BYTE)res[1])
        CallThis(FUN_4a16d0, vals, &val);
    else {
        CallThis(FUN_40c100, node + 28, &val);
        CallThis(FUN_40c100, vals[0] + F(int, node, 40) * 12, &val);
    }
    Df_Release((SchValue*)mapped);
    Df_Release(&name);
    Df_Release(&val);
}

} // extern "C"

// === make-string ($L_49c230)

KING_VAR(AsmLabel, s_ms670884, 670884)
extern "C" void FUN_44a490(); KING_ALIAS("_FUN_44a490", "@Vec1_Reserve@16")
extern "C" void FUN_469410(); KING_ALIAS("_FUN_469410", "_FillBytes")
extern "C" void FUN_44c160(); KING_ALIAS("_FUN_44c160", "@PodVec_Assign@12")
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8")
extern "C" void FUN_49c560(); KING_ALIAS("_FUN_49c560", "@SchValue_Assign@12")
extern "C" void FUN_4a2400(); KING_ALIAS("_FUN_4a2400", "_SchValue_MakeStringB")

extern "C" {

// [$L_49c230] make-string: a string of n copies of the character (a space by default)
SchValue* __cdecl Nat_MakeString(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v;
    CopyRef(&v, (SchValue*)(args[0] + 12));
    if ((BYTE)v.flags != 6) {
        CallC(FUN_4a5910, p, 21);
        Nat_Zero3(out);
        Nat_Release(&v);
        return out;
    }
    SchValue ch;
    CallThis(FUN_4bd4d0, &ch);
    int k = 2;
    if (Nat_Count(args) != 2) {
        SchValue c;
        CallThis(FUN_4a19c0, &c, args[0] + 24);
        CallThis(FUN_49c560, &ch, &c);
        CallThis(FUN_4a36c0, &c);
        k = 3;
    }
    if (Nat_Count(args) != k)
        CallC(FUN_4a5a60, p, 33, (const char*)s_ms670884);
    SchValue s, num;
    CallC(FUN_4a2400, &s, 0);
    CallThis(FUN_4bd4d0, &num);
    Nat_Typed(&num, &v, 6, s_nNumber);
    char fill = (!ch.obj && !ch.flags && !ch.m_4) ? ' ' : (char)(int)ch.obj;
    Nat_Check(&num);
    int len = CallThis<int>(FUN_4b0ea0, num.obj) + 1;
    BYTE* tmp[3];
    BYTE alloc;
    CallThis(FUN_44a490, tmp, len, &alloc);
    tmp[1] = CallC<BYTE*>(FUN_469410, tmp[0], len, &fill);
    CallThis(FUN_44c160, CallThis<BYTE*>(FUN_4aa430, &s) + 12, tmp);
    CallThis(FUN_404420, tmp);
    BYTE* str = CallThis<BYTE*>(FUN_4aa430, &s) + 12;
    F(BYTE*, str, 4)[-1] = 0;
    CallThis(FUN_454580, out, &s);
    CallThis(FUN_4a36c0, &num);
    CallThis(FUN_4a36c0, &s);
    CallThis(FUN_4a36c0, &ch);
    CallThis(FUN_4a36c0, &v);
    return out;
}

} // extern "C"

// === vector<value>::insert of a range (FUN_4a1a40)

extern "C" BYTE* __cdecl StrRefs_CopyBack(const BYTE* f, const BYTE* l, BYTE* d);   // ai_containers.cpp

extern "C" {

// [FUN_4a1a40] inserts the values [first, last) (12 bytes each) at pos of vector v: shifted within the capacity,
// or into a new buffer of size + max(size, n) with the old values released and the storage returned to the pool
void __fastcall SchValues_InsertRange(BYTE** v, void*, BYTE* pos, BYTE* first, BYTE* last)
{
    if (first == last)
        return;
    int n = (int)(last - first) / 12;
    BYTE* end = v[1];
    if ((unsigned)((v[2] - end) / 12) >= (unsigned)n) {
        int after = (int)(end - pos) / 12;
        if ((unsigned)after > (unsigned)n) {
            UninitCopyStrRefs(end - n * 12, end, end);
            v[1] += n * 12;
            StrRefs_CopyBack(pos, end - n * 12, end);
            Values_Copy(first, last, pos);
        } else {
            UninitCopyStrRefs(first + after * 12, last, end);
            v[1] += (n - after) * 12;
            UninitCopyStrRefs(pos, end, v[1]);
            v[1] += after * 12;
            for (int i = 0; i < after; i++)
                CallThis(FUN_40c100, pos + i * 12, first + i * 12);
        }
        return;
    }
    unsigned size = (unsigned)(end - v[0]) / 12;
    unsigned len = size + (size < (unsigned)n ? (unsigned)n : size);
    BYTE* p = len ? CallC<BYTE*>(FUN_44b740, len * 12) : 0;
    BYTE* e = UninitCopyStrRefs(v[0], pos, p);
    e = UninitCopyStrRefs(first, last, e);
    e = UninitCopyStrRefs(pos, v[1], e);
    for (BYTE* q = v[0]; q != v[1]; q += 12)
        CallThis(FUN_4a36c0, q);
    if (int cap = (int)(v[2] - v[0]) / 12)
        CallC(FUN_44b4a0, v[0], cap * 12);
    v[1] = e;
    v[0] = p;
    v[2] = p + len * 12;
}

} // extern "C"

// === copying a scope (FUN_4a2ef0)

extern "C" void FUN_4a4870(); KING_ALIAS("_FUN_4a4870", "@SchMap_Copy@12")
extern "C" void Nat_VecCtorEh(); KING_ALIAS("_Nat_VecCtorEh", "??_L@YGXPAXIHP6EX0@Z1@Z")

// d = s for script values (12 bytes: object, -, flags); d's old object released unless it is s's
static void Sc_Assign(DWORD* d, const DWORD* s)
{
    BYTE* o = (BYTE*)d[0];
    if (o && !(d[2] & 0x10000) && --F(int, o, 4) == 0 && !(d[0] == s[0] && d[2] == s[2] && d[1] == s[1]) && d[0])
        CallVirt((BYTE*)d[0], 12, 1);
    if (s[2] & 0x10000) {
        d[2] = s[2];
        d[0] = s[0];
        d[1] = s[1];
        return;
    }
    d[1] = 0;
    if (s[0]) {
        d[0] = s[0];
        d[2] = s[2];
        F(int, (BYTE*)d[0], 4)++;
    } else
        d[0] = d[2] = 0;
}

extern "C" {

// [FUN_4a2ef0] copy constructor (outer, s): a scope linked after outer's like s, with s's count (+20), values
// (the 8 at +36 unless s has a map +144, which is copied), value vector (+132) and value +148
BYTE* __fastcall SchScope_Copy(BYTE* self, void*, BYTE* outer, BYTE* s)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = &gv64bed0_n;
    BYTE* link = outer + 4;
    BYTE* next = F(BYTE*, link, 12);
    F(BYTE*, self, 12) = next;
    F(BYTE*, self, 16) = link;
    F(BYTE*, next, 16) = self;
    F(BYTE*, F(BYTE*, self, 16), 12) = self;
    F(int, self, 20) = F(int, s, 20);
    F(int, self, 24) = 0;
    F(int, self, 28) = 0;
    F(int, self, 32) = 0;
    CallStd(Nat_VecCtorEh, self + 36, 12, 8, (void*)FUN_4bd4d0, (void*)FUN_4a36c0);
    BYTE** v = (BYTE**)(self + 132);
    BYTE** sv = (BYTE**)(s + 132);
    int n = (int)(sv[1] - sv[0]) / 12;
    v[0] = v[1] = v[2] = 0;
    BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n * 12) : 0;
    v[0] = v[1] = p;
    v[2] = p + n * 12;
    v[1] = UninitCopyStrRefs(sv[0], sv[1], p);
    F(int, self, 144) = 0;
    DWORD* d = (DWORD*)(self + 148);
    const DWORD* x = (const DWORD*)(s + 148);
    if (x[2] & 0x10000) {
        d[2] = x[2];
        d[0] = x[0];
        d[1] = x[1];
    } else {
        d[1] = 0;
        if (x[0]) {
            d[0] = x[0];
            d[2] = x[2];
            F(int, (BYTE*)x[0], 4)++;
        } else
            d[0] = d[2] = 0;
    }
    F(void*, self, 0) = &gv64bf4c_n;
    if (F(BYTE*, s, 144)) {
        BYTE* m = (BYTE*)operator new(12);
        if (m)
            CallThis(FUN_4a4870, m, F(BYTE*, s, 144));
        F(BYTE*, self, 144) = m;
    } else
        for (int i = 0; i < F(int, self, 20); i++)
            Sc_Assign((DWORD*)(self + 36 + i * 12), (const DWORD*)(s + 36 + i * 12));
    return self;
}

} // extern "C"

// === reverse (FUN_4a0870)

KING_VAR(AsmLabel, s_rv6709ec, 6709ec)

// releases the value v {object, -, flags}
static void Rv_Release(DWORD* v)
{
    BYTE* o = (BYTE*)v[0];
    if (!(v[2] & 0x10000) && o && --F(int, o, 4) == 0 && v[0])
        CallVirt((BYTE*)v[0], 12, 1);
}

extern "C" {

// [FUN_4a0870] out = the list v (by value) reversed into new pairs of context ctx; a type error (message 10)
// and the empty value for an improper list
DWORD* __cdecl Sch_Reverse(DWORD* out, BYTE* ctx, DWORD v0, DWORD v1, DWORD v2)
{
    DWORD v[3] = {v0, v1, v2};
    DWORD acc[3] = {0, 0, 0};
    while (v[2] || v[0]) {
        if ((BYTE)v[2] != 2) {
            CallC(FUN_4a5a60, ctx, 10, (const char*)s_rv6709ec);
            CallThis(FUN_4bd4d0, out);
            out[0] = 0;
            out[2] = 0;
            CallThis(FUN_4a36c0, acc);
            Rv_Release(v);
            return out;
        }
        DWORD t[3], car[3], cdr[3];
        SchValue_MakePairB((SchValue*)t, 0, ctx);
        CallC(FUN_407f00, car, v);
        CallThis(FUN_40c100, CallThis<BYTE*>(FUN_4aa430, t) + 20, car);
        CallThis(FUN_4a36c0, car);
        CallThis(FUN_40c100, CallThis<BYTE*>(FUN_4aa430, t) + 32, acc);
        CallThis(FUN_49c560, acc, t);
        CallThis(FUN_4a36c0, t);
        DWORD* c = CallC<DWORD*>(FUN_4090a0, cdr, v);
        BYTE* o = (BYTE*)v[0];
        if (o && !(v[2] & 0x10000) && --F(int, o, 4) == 0 && !CallC<char>(FUN_409070, v, c) && v[0])
            CallVirt((BYTE*)v[0], 12, 1);
        if (c[2] & 0x10000) {
            v[2] = c[2];
            v[0] = c[0];
            v[1] = c[1];
        } else {
            v[1] = 0;
            if (c[0]) {
                v[0] = c[0];
                v[2] = c[2];
                F(int, (BYTE*)c[0], 4)++;
            } else
                v[0] = v[2] = 0;
        }
        Rv_Release(cdr);
    }
    CallThis(FUN_454580, out, acc);
    CallThis(FUN_4a36c0, acc);
    CallThis(FUN_4a36c0, v);
    return out;
}

} // extern "C"

// === the deque destructor (FUN_4a1710)

extern "C" {

// [FUN_4a1710] destructor of deque d: its elements destroyed, its blocks freed and its map returned to the pool
void __fastcall Deque_Destroy(BYTE* d)
{
    DequeIt a, b;
    CallThis(FUN_4abea0, &a, d + 8);
    memcpy(&b, d + 24, 16);
    DequeIt_Pass0(a, b);
    if (F(void*, d, 0)) {
        for (void** n = F(void**, d, 20); n < F(void**, d, 36) + 1; n++)
            free(*n);
        if (int m = F(int, d, 4))
            CallC(FUN_44b4a0, F(void*, d, 0), m * 4);
    }
}

} // extern "C"

// === the reverse native (4a0af0)

KING_VAR(AsmLabel, s_nReverse, 6709fc)

extern "C" {

// [$L_4a0af0] reverse
SchValue* __cdecl Nat_Reverse(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue v = {0, 0, 0, 0};
    if (CallC<int>(FUN_491c00, p, &v, args, (const char*)s_nReverse))
        Nat_Zero3(out);
    else {
        SchValue c, r;
        CopyRef(&c, &v);
        Sch_Reverse((DWORD*)&r, p, (DWORD)c.obj, (DWORD)c.m_4, c.flags);
        CopyRef(out, &r);
        SchValue_Release(&r);
    }
    SchValue_Release(&v);
    return out;
}

} // extern "C"

KING_VAR(char, s_nEqualQ, 67065c)    // "equal?"
// [$L_493120] equal?: Sch_Equal of the two arguments as a boolean; an empty value on an argument error
extern "C" SchValue* __cdecl Nat_EqualQ(SchValue* out, BYTE* p, BYTE** args)
{
    SchValue x = {0, 0, 0, 0}, y = {0, 0, 0, 0};
    if (CallC<int>(FUN_491aa0, p, &x, &y, args, &s_nEqualQ)) {
        out->obj = 0;
        out->m_4 = 0;
        out->flags = 0;
    } else {
        SchArg a, b;
        CallThis(FUN_454580, &b, &y);
        CallThis(FUN_454580, &a, &x);
        Nat_Bool(out, Sch_Equal(a, b, (int)p));
    }
    Nat_Release(&y);
    Nat_Release(&x);
    return out;
}
