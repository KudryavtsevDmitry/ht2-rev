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
ASM_PROC(FUN_44b4a0)    // the AI's allocator: free (p, size)
ASM_PROC(FUN_44b740)    // the AI's allocator: allocate (size)
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
KING_ALIAS("_Stl_Allocate", "FUN_44b740")

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
ASM_PROC(FUN_4581a0)    // malloc retrying through the new handler
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
