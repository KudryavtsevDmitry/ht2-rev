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
ASM_PROC(FUN_4aa460)    // vector<44 bytes>::insert_aux
ASM_PROC(FUN_4ace70)    // copy (first, last, deque iterator, tag)
extern "C" void FUN_4afe80(); KING_ALIAS("_FUN_4afe80", "_ValueList_MakeNode@4")    // list: a new node holding x

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
ASM_VAR(Ints, g_errorOff, $L_691dbc)            // per error code: don't report
ASM_VAR(int, g_lastErrorCode, $L_692244)
ASM_VAR(int, g_lastErrorLevel, $L_692248)
ASM_VAR(FILE*, g_errorFile, $L_670b04)
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

ASM_PROC(FUN_4b66f0)
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
ASM_VAR(char, gv64bf4c_n, $L_64bf4c)
ASM_VAR(char, gv64bed0_n, $L_64bed0)

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
ASM_VAR(char, s_type6708e4, $L_6708e4)

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
ASM_VAR(AsmLabel, g_nat67060c, $L_67060c)

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
ASM_VAR(AsmLabel, s_nNumberP, $L_670664)
ASM_VAR(AsmLabel, s_nComplexP, $L_67066c)
ASM_VAR(AsmLabel, s_nRealP, $L_670678)
ASM_VAR(AsmLabel, s_nRationalP, $L_670680)
ASM_VAR(AsmLabel, s_nIntegerP, $L_67068c)
ASM_VAR(AsmLabel, s_nExactP, $L_670698)
ASM_VAR(AsmLabel, s_nInexactP, $L_6706a0)
ASM_VAR(AsmLabel, s_nPlus, $L_6706ac)
ASM_VAR(AsmLabel, s_nNumber, $L_66ca60)       // "Number"

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
ASM_PROC(FUN_4a1710)    // deque<int> destructor
extern "C" void FUN_407f00(); KING_ALIAS("_FUN_407f00", "_Str_Member20")           // a pair's car (out, value)
extern "C" void FUN_4090a0(); KING_ALIAS("_FUN_4090a0", "_Str_Member32")           // a pair's cdr (out, value)
extern "C" void FUN_4a5a60(); KING_ALIAS("_FUN_4a5a60", "_Sch_Message1")
extern "C" void FUN_4a19c0(); KING_ALIAS("_FUN_4a19c0", "@SchValue_AsChar@12")
extern "C" void FUN_49c210(); KING_ALIAS("_FUN_49c210", "_SchValue_Make10008")
extern "C" void FUN_4b0ea0(); KING_ALIAS("_FUN_4b0ea0", "@Value_ToInt@8")
ASM_VAR(AsmLabel, s_nTimes, $L_6706b4)
ASM_VAR(AsmLabel, s_nDivide, $L_670570)
ASM_VAR(AsmLabel, s_nQuotient, $L_6706b8)
ASM_VAR(AsmLabel, s_nRemainder, $L_6706c0)
ASM_VAR(AsmLabel, s_nModulo, $L_6706cc)
ASM_VAR(AsmLabel, s_nAtan2, $L_67073c)
ASM_VAR(AsmLabel, s_nNumerator, $L_6706d4)
ASM_VAR(AsmLabel, s_nDenominator, $L_6706e0)
ASM_VAR(AsmLabel, s_nFloor, $L_6706ec)
ASM_VAR(AsmLabel, s_nCeiling, $L_6706f4)
ASM_VAR(AsmLabel, s_nTruncate, $L_6706fc)
ASM_VAR(AsmLabel, s_nRound, $L_670708)
ASM_VAR(AsmLabel, s_nExp, $L_670710)
ASM_VAR(AsmLabel, s_nLog, $L_670714)
ASM_VAR(AsmLabel, s_nSin, $L_670718)
ASM_VAR(AsmLabel, s_nCos, $L_67071c)
ASM_VAR(AsmLabel, s_nTan, $L_670720)
ASM_VAR(AsmLabel, s_nAsin, $L_670724)
ASM_VAR(AsmLabel, s_nAcos, $L_67072c)
ASM_VAR(AsmLabel, s_nAtan, $L_670734)
ASM_VAR(AsmLabel, s_nSqrt, $L_670744)
ASM_VAR(AsmLabel, s_nExpt, $L_67074c)
ASM_VAR(AsmLabel, s_nMakeRect, $L_670754)
ASM_VAR(AsmLabel, s_nMakePolar, $L_670768)
ASM_VAR(AsmLabel, s_nRealPart, $L_670774)
ASM_VAR(AsmLabel, s_nImagPart, $L_670780)
ASM_VAR(AsmLabel, s_nMagnitude, $L_67078c)
ASM_VAR(AsmLabel, s_nAngle, $L_670798)
ASM_VAR(AsmLabel, s_nToInexact, $L_6707a0)
ASM_VAR(AsmLabel, s_nToExact, $L_6707b0)
ASM_VAR(AsmLabel, s_nNumToStr, $L_6707c0)
ASM_VAR(AsmLabel, s_nStrToNum, $L_6707d0)
ASM_VAR(AsmLabel, s_nPairP, $L_6707e0)
ASM_VAR(AsmLabel, s_nCons, $L_6707e8)
ASM_VAR(AsmLabel, s_nCar, $L_66de28)
ASM_VAR(AsmLabel, s_nCdr, $L_6707f0)
ASM_VAR(AsmLabel, s_nSetCar, $L_6707f4)
ASM_VAR(AsmLabel, s_nSetCdr, $L_670800)
ASM_VAR(AsmLabel, s_nSymbolP, $L_67080c)
ASM_VAR(AsmLabel, s_nSymToStr, $L_670814)
ASM_VAR(AsmLabel, s_nStrToSym, $L_670824)
ASM_VAR(AsmLabel, s_nCharP, $L_670834)
ASM_VAR(AsmLabel, s_nCharEq, $L_67083c)
ASM_VAR(AsmLabel, s_nCharLt, $L_670844)
ASM_VAR(AsmLabel, s_nCharGt, $L_67084c)
ASM_VAR(AsmLabel, s_nCharLe, $L_670854)
ASM_VAR(AsmLabel, s_nCharGe, $L_67085c)
ASM_VAR(AsmLabel, s_nCharToInt, $L_670864)
ASM_VAR(AsmLabel, s_nIntToChar, $L_670874)
ASM_VAR(AsmLabel, s_nPair, $L_66cb08)         // "Pair"
ASM_VAR(AsmLabel, s_nIdentifier, $L_66ce4c)   // "Identifier"
ASM_VAR(AsmLabel, s_nString, $L_66ca58)       // "String"

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

// * and /: like + from 1 (with one argument: * returns it, / its inverse)
static SchValue* Nat_Arith(SchValue* out, BYTE* p, BYTE** args, const char* where, void (*op)(), bool divide)
{
    SchValue a = {0, 0, 0, 0};
    SchValue s;
    int one = 1, zero = 0;
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
ASM_PROC(FUN_4a1a40)    // vector<value>::insert(pos, first, last)
ASM_PROC(FUN_4b4c20)    // read (out, p, port by value)
extern "C" void FUN_4b8070(); KING_ALIAS("_FUN_4b8070", "_Sch4b8070")    // write (out, p, value by value, port by value)
ASM_VAR(AsmLabel, s_nStrLen, $L_670898)
ASM_VAR(AsmLabel, s_nStrRef, $L_6708a8)
ASM_VAR(AsmLabel, s_nStrSet, $L_6708b4)
ASM_VAR(AsmLabel, s_nVectorP, $L_6708c0)
ASM_VAR(AsmLabel, s_nMakeVector, $L_6708c8)
ASM_VAR(AsmLabel, s_nListToVector, $L_6708d4)
ASM_VAR(AsmLabel, s_nVector, $L_6708e4)        // "Vector"
ASM_VAR(AsmLabel, s_nVecLen, $L_6708ec)
ASM_VAR(AsmLabel, s_nVecRef, $L_6708fc)
ASM_VAR(AsmLabel, s_nVecSet, $L_670908)
ASM_VAR(AsmLabel, s_nProcP, $L_670914)
ASM_VAR(AsmLabel, s_nInPortP, $L_670920)
ASM_VAR(AsmLabel, s_nOutPortP, $L_67092c)
ASM_VAR(AsmLabel, s_nOpenIn, $L_67093c)
ASM_VAR(AsmLabel, s_nOpenOut, $L_67094c)
ASM_VAR(AsmLabel, s_nCloseIn, $L_670960)
ASM_VAR(AsmLabel, s_nCloseOut, $L_670974)
ASM_VAR(AsmLabel, s_nReadChar, $L_670988)
ASM_VAR(AsmLabel, s_nPeekChar, $L_670994)
ASM_VAR(AsmLabel, s_nEofP, $L_6709a0)
ASM_VAR(AsmLabel, s_nCharReady, $L_6709ac)
ASM_VAR(AsmLabel, s_nWriteChar, $L_6709b8)
ASM_VAR(AsmLabel, s_nRead, $L_6709c4)
ASM_VAR(AsmLabel, s_nWrite, $L_6709cc)
ASM_VAR(AsmLabel, s_nMacroExpand, $L_6709d4)
ASM_VAR(AsmLabel, s_nStrAppend, $L_6709ec)
ASM_VAR(AsmLabel, s_nPort, $L_66cb28)          // "Port"
ASM_VAR(AsmLabel, s_nChar, $L_670650)          // "Char"

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
ASM_VAR(AsmLabel, s_nNot, $L_670a04)
ASM_VAR(AsmLabel, s_nNullP, $L_670a08)
ASM_VAR(AsmLabel, s_nCaar, $L_670a10)
ASM_VAR(AsmLabel, s_nCadr, $L_670a18)
ASM_VAR(AsmLabel, s_nCdar, $L_670a20)
ASM_VAR(AsmLabel, s_nCddr, $L_670a28)

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
ASM_VAR(AsmLabel, s_nEqP, $L_670658)

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
