// runtime.cpp - the unit at 0x4883f0 of king.masm (after ai_trce.cpp): small
// helpers of the AI: polynomials and vector polynomials (Horner evaluation,
// derivative, integral), a bounded value, the AI's save file (reading and
// writing padded blocks), magnitudes of its big integers, and some record
// constructors and STL members.
//
// Building with  nmake ASM_RUNTIME=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "road.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" size_t __cdecl fread(void* buf, size_t size, size_t n, FILE* f);
extern "C" long __cdecl ftell(FILE* f);
extern "C" int __cdecl fseek(FILE* f, long off, int origin);
extern "C" void** __stdcall List_Erase(void** out, void** node);     // aimain.cpp
extern "C" double __cdecl Int_ToDouble(const void* n);
KING_ALIAS("_Int_ToDouble", "FUN_48a750")
extern "C" void* __cdecl Int_FromInt(int v, int a);
KING_ALIAS("_Int_FromInt", "_BigInt_FromInt")
extern "C" void __cdecl Poly_48d8f0(void* out, void* a, int b, int n, int c);
KING_ALIAS("_Poly_48d8f0", "FUN_48d8f0")
extern "C" void __cdecl AI_Error4a54e0(int code, int a, const char* name);
KING_ALIAS("_AI_Error4a54e0", "_Sch_Error")
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert(pos, first, last)
extern "C" void FUN_490d30(); KING_ALIAS("_FUN_490d30", "_Rational_Error@4")
extern "C" void FUN_48af50(); KING_ALIAS("_FUN_48af50", "_Int_CompareMagnitude")
ASM_PROC(FUN_48b8a0)
extern "C" void FUN_491010(); KING_ALIAS("_FUN_491010", "@RingMember2_Destruct@8")
extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")
ASM_VAR(AsmLabel, s_divByZero, $L_67055c)
extern "C" void FUN_470950(); KING_ALIAS("_FUN_470950", "@CharVec_Assign@12")
extern "C" void FUN_48ae10(); KING_ALIAS("_FUN_48ae10", "_BigInt_FromUnsigned")
extern "C" void FUN_48acd0(); KING_ALIAS("_FUN_48acd0", "_WStr_Assign")
ASM_PROC(FUN_491420)
ASM_PROC(FUN_48c260)
ASM_VAR(AsmLabel, s_bigint, $L_670408)
ASM_VAR(AsmLabel, s_bigintTooLarge, $L_670410)
ASM_VAR(AsmLabel, s_bigintNull, $L_670430)
ASM_VAR(AsmLabel, g_vtbl64bf38, $L_64bf38)
ASM_VAR(AsmLabel, g_vtbl64bf24, $L_64bf24)
extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")    // vector<char>::insert(pos, first, last) (another instance)
extern "C" void FUN_470900(); KING_ALIAS("_FUN_470900", "@CharVec_Construct@12")
ASM_PROC(FUN_4897b0)    // list: a new node
ASM_PROC(FUN_491750)
ASM_PROC(FUN_488450)
ASM_PROC(FUN_489900)    // vector<24 bytes>::insert_aux
ASM_PROC(FUN_48f730)    // vector<580 bytes>::insert_aux

typedef void (__cdecl* ErrorHandler)(const char* what, int x);
ASM_VAR(ErrorHandler, g_onError, $L_670324)
ASM_VAR(AsmLabel, g_strRational, $L_670550)
ASM_VAR(AsmLabel, g_strEnvironment, $L_67060c)
ASM_VAR(AsmLabel, g_int6703f0, $L_6703f0)       // static big integers
ASM_VAR(AsmLabel, g_int6703f8, $L_6703f8)
ASM_VAR(AsmLabel, g_int670400, $L_670400)
ASM_VAR(DWORD, g_padding, $L_691ccc)
ASM_VAR(void*, g_691cd0, $L_691cd0)
ASM_VAR(AsmLabel, g_vtbl64bed0, $L_64bed0)
ASM_VAR(AsmLabel, g_vtbl64beb8, $L_64beb8)
ASM_VAR(AsmLabel, g_vtbl64bef4, $L_64bef4)
ASM_VAR(AsmLabel, g_vtbl64bf0c, $L_64bf0c)

struct Vec {
    BYTE* begin;
    BYTE* end;
    BYTE* capacityEnd;
};

static void PushBack(Vec* v, const void* x, int n, void (*proc)())
{
    if (v->end != v->capacityEnd) {
        if (v->end)
            memcpy(v->end, x, n);
        v->end += n;
    } else
        CallThis(proc, v, v->end, x);
}

static BYTE** ListInsert(void* list, BYTE** out, BYTE** pos, const void* x, void (*make)())
{
    BYTE** n = CallThis<BYTE**>(make, list, x);
    n[0] = (BYTE*)pos;
    n[1] = pos[1];
    ((BYTE**)pos[1])[0] = (BYTE*)n;
    pos[1] = (BYTE*)n;
    *out = (BYTE*)n;
    return out;
}

// ---------------------------------------------------------------------------
// Polynomials: 24 double coefficients, highest first, the count at +192

struct Poly {
    double c[24];
    unsigned count;                 // +192
    BYTE   m_196[4];

    int N() const { return (int)(count << 3) >> 3; }
    double Eval(double x) const
    {
        double p = c[0];
        unsigned n = N();
        if (n > 1)
            for (unsigned i = 1; i < n; i++)
                p = p * x + c[i];
        return p;
    }
};

// a polynomial at +1392 with its count at +1584
static double EvalAt1392(const BYTE* s, double x) { return ((const Poly*)(s + 1392))->Eval(x); }

// the vector polynomial: DVec3 coefficients, the count at +576
struct VecPoly {
    DVec3    c[24];
    unsigned count;                 // +576
};

// ---------------------------------------------------------------------------
// The save file: blocks of a size and data, padded to 4 bytes

extern "C" int __cdecl Save_ReadVec(DVec3* out, FILE* f)
{
    double x, y, z;
    fread(&x, 8, 1, f);
    fread(&y, 8, 1, f);
    fread(&z, 8, 1, f);
    out->x = x;
    out->y = y;
    out->z = z;
    return 0;
}

extern "C" int __cdecl Save_ReadMatrix(void* out, FILE* f)
{
    double m[12];
    for (int i = 0; i < 12; i++) {
        double v;
        fread(&v, 8, 1, f);
        m[i] = v;
    }
    memcpy(out, m, 96);
    return 0;
}

// the block header: an id, then the size; the data starts at *pos
extern "C" int __cdecl Save_ReadHeader(FILE* f, void* id, long* pos, int* size)
{
    if (fread(id, 4, 1, f) != 1)
        return -1;
    int n;
    size_t r = fread(&n, 4, 1, f);
    *size = n;
    if (r != 1)
        return 1;
    *pos = ftell(f);
    return 0;
}

extern "C" int __cdecl Save_SkipBlock(FILE* f, long pos, int size)
{
    long end = pos + ((size + 3) & ~3);
    if (end != ftell(f))
        fseek(f, end, 0);
    return 0;
}

// reads a header from memory
extern "C" int __cdecl Save_ParseHeader(int** cur, int* id, int** data, int* size)
{
    *id = **cur;
    (*cur)++;
    int n = **cur;
    (*cur)++;
    *size = n;
    *data = *cur;
    return 0;
}

extern "C" int __cdecl Save_BlockEnd(BYTE** out, BYTE* data, int size)
{
    *out = data + ((size + 3) & ~3);
    return 0;
}

// starts a block: the id, a size placeholder whose offset goes to *at
extern "C" int __cdecl Save_BeginBlock(Vec* v, const int* id, int* at)
{
    CallThis(FUN_46e460, v, v->end, id, id + 1);
    *at = v->end - v->begin;
    DWORD p = g_padding;
    CallThis(FUN_46e460, v, v->end, &p, &p + 1);
    return 0;
}

// ends a block: the size at its offset, padding to 4 bytes
extern "C" int __cdecl Save_EndBlock(Vec* v, const int* at)
{
    int pos = *at;
    int len = v->end - pos - v->begin - 4;
    *(int*)(v->begin + pos) = len;
    int pad = len & 3;
    if (pad) {
        DWORD p[2];
        p[0] = g_padding;
        CallThis(FUN_46e460, v, v->end, &p[0], (BYTE*)&p[1] - pad);
    }
    return 0;
}

extern "C" void __stdcall Rational_Error(int x) { g_onError(g_strRational, x); }

// ---------------------------------------------------------------------------
// Big integers: a count of 16-bit digits (+0), a capacity (+2), a sign (+4),
// the digits from +6, lowest first

struct BigInt {
    WORD n, capacity, sign;
    WORD d[1];
};

extern "C" BigInt* __cdecl Int_SetZero(BigInt* p)
{
    if (p && p->capacity) {
        p->n = 0;
        p->sign = 1;
        return p;
    }
    return (BigInt*)g_int6703f0;
}

extern "C" BigInt* __cdecl Int_SetDigit(BigInt* p, int v)
{
    if (p && p->capacity >= 1) {
        p->n = 1;
        p->sign = (WORD)v;
        p->d[0] = 1;
        return p;
    }
    return (BigInt*)(v ? (void*)g_int6703f8 : (void*)g_int670400);
}

extern "C" int __cdecl Int_CompareMagnitude(const BigInt* a, const BigInt* b)
{
    int d = a->n - b->n;
    if (d)
        return d;
    for (int i = a->n - 1; i >= 0; i--) {
        d = (int)a->d[i] - (int)b->d[i];
        if (d)
            return d;
    }
    return 0;
}

// divides n digits by a small divisor; the quotient goes to q if given
extern "C" unsigned __cdecl Int_DivSmall(const WORD* digits, int n, unsigned divisor, WORD* q)
{
    if (!n || (WORD)divisor == 1)
        return 0;
    unsigned dv = divisor & 0xffff;
    unsigned rem = 0;
    if (q) {
        for (int i = n - 1; i >= 0; i--) {
            rem = (rem << 16) | digits[i];
            unsigned v = rem / dv;
            q[i] = (WORD)v;
            rem -= dv * v;
        }
        return rem & 0xffff;
    }
    for (int i = n - 1; i >= 0; i--) {
        rem = (rem << 16) | digits[i];
        rem -= rem / dv * dv;
    }
    return rem & 0xffff;
}

extern "C" void __cdecl Int_InitOne() { g_691cd0 = Int_FromInt(0, 1); }

// ---------------------------------------------------------------------------
// Other helpers

// v moves by d, stopping at 1 going up or at 0 going down; 1 if it was
// already at that end
extern "C" int __stdcall Bounded_Step(double* v, double d)
{
    if (d > 0) {
        if (!(*v < 1.0) && !(*v != *v))
            return 1;
        double r = d + *v;
        *v = r;
        if (!(r >= 1.0))
            return 0;
        F(DWORD, v, 0) = 0;
        F(DWORD, v, 4) = 0x3ff00000;
        return 0;
    }
    if (!(*v > 0))
        return 1;
    double r = d + *v;
    *v = r;
    if (r > 0)
        return 0;
    F(DWORD, v, 0) = 0;
    F(DWORD, v, 4) = 0;
    return 0;
}

static BYTE* Copy24sBackward(const BYTE* first, const BYTE* last, BYTE* destEnd)
{
    for (int n = (last - first) / 24; n > 0; n--) {
        last -= 24;
        destEnd -= 24;
        DVec3 v = *(const DVec3*)last;
        *(DVec3*)destEnd = v;
    }
    return destEnd;
}

extern "C" BYTE* __cdecl CopyBackward24s(const BYTE* f, const BYTE* l, BYTE* d) { return Copy24sBackward(f, l, d); }

extern "C" BYTE* __cdecl Copy24s(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = (last - first) / 24; n > 0; n--, first += 24, dest += 24)
        *(DVec3*)dest = *(const DVec3*)first;
    return dest;
}

extern "C" BYTE* __cdecl UninitCopy24s(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (; first != last; first += 24, dest += 24)
        if (dest)
            memcpy(dest, first, 24);
    return dest;
}

extern "C" BYTE* __cdecl UninitCopy580s(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (; first != last; first += 580, dest += 580)
        if (dest)
            memcpy(dest, first, 580);
    return dest;
}

// queues a task, or drops it when 1024 are queued
extern "C" void __cdecl Tasks_Queue(BYTE* ctx, void** node)
{
    node[2] = 0;
    int n = F(int, ctx, 4220);
    if (n == 1024) {
        void* out;
        List_Erase(&out, node);
        return;
    }
    F(void**, ctx, 124 + n * 4) = node;
    F(int, ctx, 4220) = n + 1;
}

// ---------------------------------------------------------------------------
// thiscall members

struct StrRef {
    BYTE* ptr;
    int   len;
    DWORD flags;
};

static BYTE* RingInsert(BYTE* s, BYTE* ring)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = g_vtbl64bed0;
    BYTE* last = F(BYTE*, ring, 16);
    F(BYTE*, s, 16) = ring + 4;
    F(BYTE*, s, 12) = last;
    F(BYTE*, last, 16) = s;
    F(BYTE*, F(BYTE*, s, 16), 12) = s;
    F(void*, s, 0) = g_vtbl64beb8;
    return s;
}

extern "C" {
int __fastcall Rec_SetText12(BYTE* s, void*, const char* str)                                            // [FUN_488810]
{
    BYTE** v = (BYTE**)(s + 12);
    v[1] = v[1] + (v[0] - v[1]);
    CallThis(FUN_44c220, v, v[1], str, str + strlen(str) + 1);
    return 0;
}
DWORD* __fastcall Rec_Init488880(DWORD* s, void*, DWORD x)                                               // [FUN_488880]
{
    s[0] = 0;
    s[1] = 0x3ff00000;
    s[2] = 0xffffffff;
    s[3] = x;
    for (int i = 4; i <= 12; i++)
        s[i] = 0;
    return s;
}
DWORD* __fastcall Rec_Init488880a(DWORD* s, void*, DWORD x) { Rec_Init488880(s, 0, x | 1); return s; }  // [FUN_4888c0]
DWORD* __fastcall Rec_Init488880b(DWORD* s, void*, DWORD x) { Rec_Init488880(s, 0, x | 2); return s; }  // [FUN_4888e0]
BYTE* __fastcall Rec_Init488900(BYTE* s, void*, int a, int b)                                            // [FUN_488900]
{
    CallThis(FUN_470900, s, a);
    for (int off = 16; off <= 132; off += 4)
        F(int, s, off) = 0;
    F(DWORD, s, 44) = 0x3ff00000;
    F(DWORD, s, 76) = 0x3ff00000;
    F(DWORD, s, 108) = 0x3ff00000;
    F(BYTE, s, 140) = 0;
    F(int, s, 136) = b;
    return s;
}
BYTE** __fastcall List489_Insert(void* s, void*, BYTE** out, BYTE** pos, const void* x) { return ListInsert(s, out, pos, x, FUN_4897b0); } // [FUN_489780]
BYTE** __fastcall List491_Insert(void* s, void*, BYTE** out, BYTE** pos, const void* x) { return ListInsert(s, out, pos, x, FUN_491750); } // [FUN_4916f0]
BYTE* __fastcall Rec488450_DeletingDestructor(BYTE* s, void*, int flags)                                // [FUN_48a680]
{
    CallThis(FUN_488450, s);
    if (flags & 1)
        operator delete(s);
    return s;
}
double __fastcall IntRef_ToDouble(void* const* s, void*) { return Int_ToDouble(*s); }                    // [FUN_48a9f0]
DVec3* __fastcall DVec3_SubAssign(DVec3* s, void*, const DVec3* b)                                       // [FUN_48ce10]
{
    s->x = s->x - b->x;
    s->y = s->y - b->y;
    s->z = s->z - b->z;
    return s;
}
double __fastcall Poly1392_Diff(BYTE* s, void*, double x, double y) { return EvalAt1392(s, y) - EvalAt1392(s, x); } // [FUN_48e3c0]
double __fastcall Poly1392_AbsDiff(BYTE* s, void*, double x, double y) { return x87_fabs(Poly1392_Diff(s, 0, x, y)); } // [FUN_48e430]
double __fastcall Poly1392_AbsDiff01(BYTE* s, void*) { return Poly1392_AbsDiff(s, 0, 0.0, 1.0); }       // [FUN_48e450]
DWORD* __fastcall Rec_Init48e8c0(DWORD* s, void*, DWORD a) { s[0] = a; for (int i = 1; i <= 6; i++) s[i] = 0; return s; } // [FUN_48e8c0]
void* __fastcall Rec_Make48d8f0(int* s, void*, void* out)                                                // [FUN_48e8e0]
{
    Poly_48d8f0(out, (void*)s[4], s[1], (s[5] - s[4]) >> 3, s[0]);
    return out;
}
void __fastcall Vec24_PushBackK(Vec* s, void*, const void* x) { PushBack(s, x, 24, FUN_489900); }       // [FUN_48e920]
void __fastcall Vec580_PushBack(Vec* s, void*, const void* x) { PushBack(s, x, 580, FUN_48f730); }      // [FUN_48f6f0]
double __fastcall Poly_Eval(const Poly* s, void*, const double* x) { return s->Eval(*x); }               // [FUN_48e960]
Poly* __fastcall Poly_Derivative(const Poly* s, void*, Poly* out)                                        // [FUN_48e990]
{
    Poly r;
    r.count = 0;
    unsigned n = s->N();
    for (unsigned i = 0; i + 1 < (unsigned)s->N(); i++) {
        __int64 k = (unsigned)(s->N() - i - 1);
        r.c[r.count++] = (double)k * s->c[i];
    }
    memcpy(out, &r, 196);
    return out;
}
Poly* __fastcall Poly_Integral(const Poly* s, void*, Poly* out)                                          // [FUN_48ea30]
{
    Poly r;
    r.count = 0;
    for (unsigned i = 0; i < (unsigned)s->N(); i++) {
        __int64 k = (unsigned)(s->N() - i);
        r.c[r.count++] = s->c[i] / (double)k;
    }
    r.c[r.count] = 0;
    r.count++;
    memcpy(out, &r, 196);
    return out;
}
void __fastcall Poly_Append(Poly* s, void*, const double* c)                                             // [FUN_48ebf0]
{
    double* p = &s->c[s->count];
    if (p)
        *p = *c;
    s->count++;
}
void __fastcall Poly_Erase(Poly* s, void*, double* first, double* last)                                 // [FUN_48ec30]
{
    double* d = first;
    for (int n = &s->c[s->count] - last; n > 0; n--, d++)
        memcpy(d, (BYTE*)d + ((BYTE*)last - (BYTE*)first), 8);
    s->count -= last - first;
}
VecPoly* __fastcall VecPoly_Line(VecPoly* s, void*, DVec3 a, DVec3 b)                                   // [FUN_48ec90]
{
    s->count = 0;
    if (s)
        s->c[0] = a;
    s->count++;
    DVec3* p = &s->c[s->count];
    if (p)
        *p = b;
    s->count++;
    return s;
}
DVec3* __fastcall VecPoly_Eval(const VecPoly* s, void*, DVec3* out, const double* t)                     // [FUN_48ecf0]
{
    DVec3 r = s->c[0];
    unsigned n = s->count * 24 / 24;
    if (n > 1) {
        double x = *t;
        double rx = r.x;
        for (unsigned i = 1; i < n; i++) {
            rx = rx * x;
            double ry = r.y * x;
            double rz = r.z * x;
            rx = rx + s->c[i].x;
            r.y = s->c[i].y + ry;
            r.z = s->c[i].z + rz;
        }
        r.x = rx;
    }
    *out = r;
    return out;
}
DVec3* __fastcall DVec3_Fill(DVec3* s, void*, double v) { s->x = v; s->y = v; s->z = v; return s; }    // [FUN_48fe10]
void __fastcall VecPoly_Append(VecPoly* s, void*, const DVec3* c)                                        // [FUN_48fe30]
{
    DVec3* p = &s->c[s->count];
    if (p)
        *p = *c;
    s->count++;
}
Poly* __fastcall Poly_Scaled(const Poly* s, void*, Poly* out, const double* k)                           // [FUN_490120]
{
    Poly r;
    r.count = 0;
    double v = *k;
    for (unsigned i = 0; i < (unsigned)s->N(); i++)
        r.c[r.count++] = v * s->c[i];
    memcpy(out, &r, 196);
    return out;
}
BYTE* __fastcall Ring_Insert(BYTE* s, void*, BYTE* ring) { return RingInsert(s, ring); }                 // [FUN_490f10]
BYTE* __fastcall RingA_Construct(BYTE* s, void*, BYTE* ring, int a, int b)                               // [FUN_491040]
{
    RingInsert(s, ring);
    F(int, s, 20) = b;
    F(int, s, 24) = a;
    F(void*, s, 0) = g_vtbl64bef4;
    return s;
}
BYTE* __fastcall RingB_Construct(BYTE* s, void*, BYTE* ring)                                             // [FUN_4910a0]
{
    RingInsert(s, ring);
    for (int off = 20; off <= 52; off += 4)
        F(int, s, off) = 0;
    F(void*, s, 0) = g_vtbl64bf0c;
    return s;
}
// assigns a string handle whose object must be an environment (type 12)
void __fastcall Str_AssignEnvironment(StrRef* s, void*, const StrRef* src)                               // [FUN_491810]
{
    if (s->ptr) {
        F(int, s->ptr, 4)--;
        if (F(int, s->ptr, 4) == 0 && !(s->ptr == src->ptr && s->flags == src->flags && s->len == src->len) && s->ptr)
            CallVirt(s->ptr, 12, 1);
    }
    BYTE* p = src->ptr;
    if (!p) {
        s->ptr = 0;
        s->flags = 0;
        s->len = 0;
        return;
    }
    if (CallVirt<int>(p, 0) != 12) {
        AI_Error4a54e0(2, 1, CallVirt<const char*>(src->ptr, 4, (const char*)g_strEnvironment));
        return;
    }
    s->ptr = p;
    s->flags = src->flags;
    F(int, p, 4)++;
    s->len = 0;
}
}

// ---------------------------------------------------------------------------
// wide strings of the AI's text: +0 the length, +2 the capacity (0: not
// owned), +4 a flag, the characters from +6

typedef void (__cdecl* RangeError)(const char* type, const char* text);
ASM_VAR(RangeError, g_rangeError, $L_670324)

static WORD* WStr_New(unsigned len)
{
    unsigned need = len * 2 + 12;
    unsigned size = 16;
    while (size < need)
        size <<= 1;
    size -= 4;
    if (size >= 131070)
        g_rangeError("Integer", "Requested length out of range");
    WORD* p = (WORD*)operator new(size);
    p[1] = (WORD)((size - 6) >> 1);
    return p;
}

static void WStr_Zero(WORD* from, WORD* to)
{
    for (; from < to; from++)
        *from = 0;
}

// a string of len characters, the first n from src (in old if it fits)
extern "C" WORD* __cdecl WStr_Make(WORD* old, const WORD* src, int n, WORD flag, int len)
{
    WORD* p = old && !(len > (int)old[1]) ? old : WStr_New(len);
    p[2] = flag;
    p[0] = (WORD)len;
    for (int i = 0; i < n; i++)
        p[3 + i] = src[i];
    WStr_Zero(p + 3 + n, p + 3 + p[0]);
    if (old != p && old && old[1])
        operator delete(old);
    return p;
}

// resizes a string to len characters, zero-filled
extern "C" WORD* __cdecl WStr_Resize(WORD* old, unsigned len)
{
    WORD* p;
    unsigned cur;
    if (!old) {
        p = WStr_New(len);
        cur = 0;
        p[2] = 1;
    } else {
        cur = old[0];
        if ((int)len > (int)old[1]) {
            p = WStr_New(len);
            for (unsigned i = 0; i < cur; i++)
                p[3 + i] = old[3 + i];
            p[2] = old[2];
            if (old[1])
                operator delete(old);
        } else
            p = old;
    }
    p[0] = (WORD)len;
    WStr_Zero(p + 3 + cur, p + 3 + (len & 0xffff));
    return p;
}

// dst = src
extern "C" WORD* __cdecl WStr_Assign(WORD* dst, const WORD* src)
{
    if (dst == src)
        return dst;
    if (!src) {
        if (!dst) {
            WORD* p = (WORD*)operator new(12);
            p[1] = 3;
            p[0] = 0;
            p[2] = 1;
            return p;
        }
        WStr_Zero(dst + 3, dst + 3 + dst[0]);
        dst[0] = 0;
        dst[2] = 1;
        return dst;
    }
    unsigned n = src[0];
    WORD* p = dst;
    if (!dst || (int)n > (int)dst[1]) {
        if (dst && dst[1])
            operator delete(dst);
        p = WStr_New(n);
    }
    p[0] = (WORD)n;
    p[2] = src[2];
    for (unsigned i = 0; i < n; i++)
        p[3 + i] = src[3 + i];
    return p;
}

// ---------------------------------------------------------------------------
// the AI's big integers: wide strings holding 16-bit digits, lowest first;
// +4 the sign (1: not negative)

extern "C" void FUN_48aae0(); KING_ALIAS("_FUN_48aae0", "_BigInt_Alloc")    // the integer out made ready for len digits (out, len)

// trims leading zero digits; zero is not negative
static WORD* Int_Normalize(WORD* r)
{
    int c = r[0];
    for (WORD* p = r + 3 + c; c > 0;) {
        p--;
        if (*p)
            break;
        c--;
    }
    r[0] = (WORD)c;
    if (!(WORD)c)
        r[2] = 1;
    return r;
}

// out = a * k
extern "C" WORD* __cdecl Int_MulInt(WORD* a, int k, WORD* out)
{
    if (!a)
        g_rangeError("Integer", "operation on uninitialized Integer");
    int n = a[0];
    WORD* r;
    if (n == 0 || k == 0)
        r = (WORD*)Int_SetZero((BigInt*)out);
    else if (k == 1)
        r = WStr_Assign(out, a);
    else {
        int pos = k >= 0;
        WORD sign = (short)a[2] == pos;
        unsigned u = pos ? k : -k;
        WORD kd[2];
        int kn = 0;
        for (; u; u >>= 16)
            kd[kn++] = (WORD)u;
        bool same = a == out;
        int len = kn + n;
        r = same ? WStr_Resize(out, len) : CallC<WORD*>(FUN_48aae0, out, len);
        WORD* d = r + 3;
        WORD* end = d + len;
        WORD *dst, *outer, *low, *inner, *innerEnd;
        if (same) {
            dst = d + n - 1;
            low = d;
            outer = dst;
            inner = kd;
            innerEnd = kd + kn;
        } else if (n <= kn) {
            low = a + 3;
            dst = d + n - 1;
            outer = a + 3 + n - 1;
            inner = kd;
            innerEnd = kd + kn;
        } else {
            dst = d + kn - 1;
            inner = a + 3;
            low = kd;
            outer = kd + kn - 1;
            innerEnd = a + 3 + n;
        }
        while (outer >= low) {
            unsigned m = *outer--;
            WORD* p = dst--;
            *p = 0;
            if (!m)
                continue;
            unsigned carry = 0;
            for (WORD* q = inner; q < innerEnd; q++, p++) {
                unsigned t = *q * m + *p + carry;
                *p = (WORD)t;
                carry = t >> 16;
            }
            for (; carry && p < end; p++) {
                carry += *p;
                *p = (WORD)carry;
                carry >>= 16;
            }
        }
        r[2] = sign;
    }
    return Int_Normalize(r);
}

// out = a shifted by sh bits: left when sh > 0, right when sh < 0
extern "C" WORD* __cdecl Int_Shift(WORD* a, int sh, WORD* out)
{
    if (!a)
        g_rangeError("Integer", "operation on uninitialized Integer");
    int n = a[0];
    if (n == 0 || sh == 0)
        return WStr_Assign(out, a);
    bool same = a == out;
    int sign = (short)a[2];
    unsigned u = sh >= 0 ? sh : -sh;
    int words = u >> 4;
    int bits = u & 15;
    WORD* r;
    if (sh > 0) {
        int len = n + words + 1;
        r = same ? WStr_Resize(out, len) : CallC<WORD*>(FUN_48aae0, out, len);
        WORD* d = r + 3;
        WORD* p = d + len - 1;
        WORD* src = same ? d : a + 3;
        unsigned acc = 0;
        for (WORD* s = src + n - 1; s >= src; s--) {
            acc = (acc << 16) | ((unsigned)*s << bits);
            *p-- = (WORD)(acc >> 16);
        }
        *p-- = (WORD)acc;
        for (; p >= d; p--)
            *p = 0;
    } else {
        int len = n - words;
        if (len < 0)
            r = (WORD*)Int_SetZero((BigInt*)out);
        else {
            r = same ? WStr_Resize(out, len) : CallC<WORD*>(FUN_48aae0, out, len);
            WORD* d = r + 3;
            WORD* end = d + len;
            WORD* src = same ? d : a + 3;
            WORD* s = src + words + 1;
            WORD* srcEnd = src + n;
            unsigned acc = (unsigned)s[-1] >> bits;
            WORD* q = d;
            for (; s < srcEnd; s++, q++) {
                acc |= (unsigned)*s << (16 - bits);
                *q = (WORD)acc;
                acc >>= 16;
            }
            *q++ = (WORD)acc;
            if (same)
                end = srcEnd;
            for (; q < end; q++)
                *q = 0;
        }
    }
    r[2] = (WORD)sign;
    return Int_Normalize(r);
}

// one pass of long division (Knuth's algorithm D): divides the digits of R
// by the vn digits of V, qn quotient digits from the top; R keeps the
// remainder, Q (if any) gets the quotient. The estimate's high bits are
// kept as the original keeps them; only its low 16 bits are used.
extern "C" void __cdecl Int_DivDigits(WORD* R, const WORD* V, int vn, WORD* Q, int qn)
{
    unsigned v1 = V[vn - 1];
    unsigned v2 = V[vn - 2];
    const WORD* vend = V + vn;
    for (int j = qn - 1; j >= 0; j--) {
        WORD* top = R + j + vn;
        unsigned qhat = v1 == top[0] ? 0xffff : (((unsigned)top[0] << 16) | top[-1]) / v1;
        for (;;) {
            unsigned q = qhat & 0xffff;
            unsigned lo = v2 * q;
            unsigned mid = q * v1 + (lo >> 16);
            WORD t[3] = {(WORD)lo, (WORD)mid, (WORD)(mid >> 16)};
            int diff = 0;
            for (int k = 2; k >= 0; k--)
                if ((diff = (int)t[k] - (int)top[k - 2]) != 0)
                    break;
            if (diff <= 0)
                break;
            qhat += 0xffff;
        }
        unsigned q = qhat & 0xffff;
        unsigned mul = 0, borrow = 1;
        WORD* r = R + j;
        for (const WORD* v = V; v < vend; v++, r++) {
            mul = (mul >> 16) + *v * q;
            borrow = borrow + *r - (mul & 0xffff) + 0xffff;
            *r = (WORD)borrow;
            borrow >>= 16;
        }
        borrow = borrow + *r - (mul >> 16) + 0xffff;
        *r = (WORD)borrow;
        if (!(borrow & 0xffff0000)) {
            qhat += 0xffff;
            unsigned c = 0;
            WORD* rr = R + j;
            for (const WORD* v = V; v < vend; v++, rr++) {
                c = (c >> 16) + *rr + *v;
                *rr = (WORD)c;
            }
            *rr = 0;
        }
        if (Q)
            Q[j] = (WORD)qhat;
    }
}

// --- big integers: +0 word length, +2 word capacity, +4 word sign (1 = not
// negative), +6 the 16-bit digits

static void BigInt_Fail(const char* what)
{
    g_onError((const char*)s_bigint, (int)what);
}

extern "C" {

// [FUN_48aae0] a big integer with room for n digits, all zero (reuses p when it fits)
WORD* __cdecl BigInt_Alloc(WORD* p, unsigned n)
{
    if (!p || n > p[1]) {
        if (p && p[1])
            operator delete(p);
        unsigned need = n * 2 + 12;
        unsigned size = 16;
        while (size < need)
            size <<= 1;
        size -= 4;
        if (size >= 131070)
            BigInt_Fail((const char*)s_bigintTooLarge);
        p = (WORD*)operator new(size);
        p[1] = (WORD)((size - 6) >> 1);
    }
    p[0] = (WORD)n;
    p[2] = 1;
    memset(p + 3, 0, (n & 0xffff) * 2);
    return p;
}

// [FUN_48ade0] a big integer from v
WORD* __cdecl BigInt_FromInt(WORD* p, int v)
{
    WORD sign = v >= 0;
    WORD* r = CallC<WORD*>(FUN_48ae10, p, sign ? v : -v);
    r[2] = sign;
    return r;
}

// [FUN_48c540] r = -a (r reused when it is not a)
WORD* __cdecl BigInt_Negate(WORD* a, WORD* r)
{
    if (!a)
        BigInt_Fail((const char*)s_bigintNull);
    if (r != a)
        r = CallC<WORD*>(FUN_48acd0, r, a);
    if (r[0])
        r[2] = r[2] == 0;
    return r;
}

// [FUN_48c590] *out = a op b through a temporary
WORD** __cdecl BigInt_Op48c260(WORD** out, WORD* const* a, WORD* const* b)
{
    WORD* t = CallC<WORD*>(FUN_48c260, *a, *b);
    *out = CallC<WORD*>(FUN_48acd0, 0, t);
    if (t && t[1])
        operator delete(t);
    return out;
}

// [FUN_4883f0] an empty 36-byte record initialised from s
DWORD* __fastcall Rec4883f0_Construct(DWORD* self, void*, int s)
{
    for (int i = 0; i < 9; i++)
        self[i] = 0;
    CallThis(FUN_470950, self, s);
    return self;
}

// [FUN_491380] a pooled script object for the interpreter x
BYTE* __fastcall Pooled_Construct(BYTE* self, void*, BYTE* x)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(BYTE*, self, 12) = x;
    F(void*, self, 0) = g_vtbl64bf38;
    BYTE* slot;
    DWORD* r = CallC<DWORD*>(FUN_491420, &slot, x, self);
    F(DWORD, self, 16) = *r;
    F(int, self, 20) = 0;
    F(int, self, 24) = 0;
    F(int, self, 28) = 0;
    F(void*, self, 0) = g_vtbl64bf24;
    return self;
}

}

static void Value_Release(DWORD* v)
{
    if (!(v[2] & 0x10000) && v[0]) {
        BYTE* o = (BYTE*)v[0];
        F(int, o, 4) = F(int, o, 4) - 1;
        if (!F(int, (BYTE*)v[0], 4) && v[0])
            CallVirt((BYTE*)v[0], 12, 1);
    }
}

extern "C" {

// [FUN_48ae10] the big integer v (reuses p when it fits)
WORD* __cdecl BigInt_FromUnsigned(WORD* p, unsigned v)
{
    WORD w[2];
    WORD n = 0;
    for (unsigned x = v; x; x >>= 16)
        w[n++] = (WORD)x;
    if (!p || n > p[1]) {
        if (p && p[1])
            operator delete(p);
        unsigned need = n * 2 + 12;
        unsigned size = 16;
        while (size < need)
            size <<= 1;
        size -= 4;
        if (size >= 131070)
            BigInt_Fail((const char*)s_bigintTooLarge);
        p = (WORD*)operator new(size);
        p[1] = (WORD)((size - 6) >> 1);
    }
    p[0] = n;
    p[2] = 1;
    for (int i = 0; i < n; i++)
        p[3 + i] = w[i];
    return p;
}

// [FUN_490db0] reduces the rational (+0 numerator, +4 denominator)
void __fastcall Rational_Reduce(WORD** self, void*)
{
    WORD** q = self + 1;
    if (!(*q)[0]) {
        CallThis(FUN_490d30, self, (const char*)s_divByZero);
    } else if ((*q)[2] != 1) {
        *q = BigInt_Negate(*q, *q);
        self[0] = BigInt_Negate(self[0], self[0]);
    }
    WORD* g;
    BigInt_Op48c260(&g, self, q);
    if (CallC<int>(FUN_48af50, g, g_691cd0)) {
        self[0] = CallC<WORD*>(FUN_48b8a0, self[0], g, self[0]);
        *q = CallC<WORD*>(FUN_48b8a0, *q, g, *q);
    }
    if (g && g[1])
        operator delete(g);
}

// [FUN_490930] vector<8 bytes> copy constructor
DWORD* __fastcall Vec8_Copy(DWORD* self, void*, const DWORD* s)
{
    unsigned n = (s[1] - s[0]) >> 3;
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    BYTE* p = n ? CallC<BYTE*>(FUN_44b740, n * 8) : 0;
    self[0] = (DWORD)p;
    self[1] = (DWORD)p;
    self[2] = (DWORD)(p + n * 8);
    unsigned bytes = ((s[1] - s[0]) >> 3) << 3;
    memmove(p, (const void*)s[0], bytes);
    self[1] = (DWORD)(p + bytes);
    return self;
}

// [FUN_491100] destructor: three script values
void __fastcall Rec491100_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64bf0c;
    Value_Release((DWORD*)(self + 44));
    Value_Release((DWORD*)(self + 32));
    Value_Release((DWORD*)(self + 20));
    CallThis(FUN_491010, self);
}

}

// ---------------------------------------------------------------------------
ASM_VAR(AsmLabel, g_vt64bed0, $L_64bed0)
ASM_VAR(AsmLabel, g_vt64bee4, $L_64bee4)

// [FUN_48a9d0] frees the buffer *this when its word at +2 (capacity) is set
extern "C" void __fastcall WBuf_Free(BYTE** self, void*)
{
    BYTE* p = *self;
    if (p && F(WORD, p, 2))
        operator delete(p);
}

// unlinks a ring member (+12 prev, +16 next) and sets the base vtables
static void Rt_RingUnlink(BYTE* self)
{
    F(void*, self, 0) = g_vt64bed0;
    BYTE* prev = F(BYTE*, self, 12);
    if (prev)
        F(BYTE*, prev, 16) = F(BYTE*, self, 16);
    BYTE* next = F(BYTE*, self, 16);
    if (next)
        F(BYTE*, next, 12) = F(BYTE*, self, 12);
    F(void*, self, 0) = g_vt64bee4;
}

// [FUN_490fa0] the ring member's destructor body
extern "C" void __fastcall RingMember_Destruct(BYTE* self, void*) { Rt_RingUnlink(self); }

// [FUN_491010] the same for the other ring member class
extern "C" void __fastcall RingMember2_Destruct(BYTE* self, void*) { Rt_RingUnlink(self); }


// ---------------------------------------------------------------------------
ASM_PROC(FUN_4890b0)
ASM_VAR(AsmLabel, s_rt6703e8, $L_6703e8)

// [FUN_4895b0] reads a file through 4890b0; -1 when it can't be opened
extern "C" int __fastcall Rt_ReadFile(BYTE* self, void*, const char* name)
{
    FILE* f = fopen(name, s_rt6703e8);
    if (!f)
        return -1;
    int r = CallThis<int>(FUN_4890b0, self, f);
    fclose(f);
    return r;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_490fa0(); KING_ALIAS("_FUN_490fa0", "@RingMember_Destruct@8")
extern "C" void FUN_491010(); KING_ALIAS("_FUN_491010", "@RingMember2_Destruct@8")
ASM_PROC(FUN_491090)
extern "C" void FUN_491100(); KING_ALIAS("_FUN_491100", "@Rec491100_Destroy@8")
extern "C" void FUN_4914c0(); KING_ALIAS("_FUN_4914c0", "@Run4914c0_Destroy@4")
ASM_PROC(FUN_491590)
ASM_VAR(AsmLabel, g_run64bee4, $L_64bee4)
ASM_VAR(AsmLabel, g_run6705ec, $L_6705ec)
ASM_VAR(AsmLabel, g_run6705f4, $L_6705f4)
ASM_VAR(AsmLabel, g_run6705fc, $L_6705fc)

static BYTE* Run_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_490f50]
extern "C" int __fastcall Run_490f50(BYTE* s) { return (int)g_run6705ec; }
// [$L_490f60]
extern "C" int __fastcall Run_490f60(BYTE* s) { return 4; }
// [$L_490f70]
extern "C" int __fastcall Run_490f70(BYTE* s) { return (int)g_run6705f4; }
// [$L_491400]
extern "C" int __fastcall Run_491400(BYTE* s) { return 5; }
// [$L_491410]
extern "C" int __fastcall Run_491410(BYTE* s) { return (int)g_run6705fc; }
// [$L_490f80]
extern "C" BYTE* __fastcall Run490f80_DeletingDestructor(BYTE* s, void*, int flags) { return Run_DeleteAfter(s, flags, FUN_490fa0); }
// [$L_490fd0]
extern "C" BYTE* __fastcall Run490fd0_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_run64bee4;
    if (flags & 1)
        operator delete(s);
    return s;
}
// [$L_490ff0]
extern "C" BYTE* __fastcall Run490ff0_DeletingDestructor(BYTE* s, void*, int flags) { return Run_DeleteAfter(s, flags, FUN_491010); }
// [$L_491070]
extern "C" BYTE* __fastcall Run491070_DeletingDestructor(BYTE* s, void*, int flags) { return Run_DeleteAfter(s, flags, FUN_491090); }
// [$L_4910e0]
extern "C" BYTE* __fastcall Run4910e0_DeletingDestructor(BYTE* s, void*, int flags) { return Run_DeleteAfter(s, flags, FUN_491100); }
// [$L_4914a0]
extern "C" BYTE* __fastcall Run4914a0_DeletingDestructor(BYTE* s, void*, int flags) { return Run_DeleteAfter(s, flags, FUN_4914c0); }
// [$L_491570]
extern "C" BYTE* __fastcall Run491570_DeletingDestructor(BYTE* s, void*, int flags) { return Run_DeleteAfter(s, flags, FUN_491590); }

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

// [$L_491690] destroys each 44-byte element of the vector at +20 (vtbl+0)
extern "C" void __fastcall Run_491690(BYTE* s)
{
    for (unsigned i = 0; i < (unsigned)((F(BYTE*, s, 24) - F(BYTE*, s, 20)) / 44); i++)
        CallVirt(F(BYTE*, s, 20) + i * 44, 0);
}

extern "C" void FUN_491520(); KING_ALIAS("_FUN_491520", "_Tasks_Queue")
ASM_VAR(AsmLabel, g_run64bf38, $L_64bf38)
ASM_VAR(AsmLabel, g_run64bee4, $L_64bee4)

// [FUN_4914c0] (vtable 64bf38) FUN_491520 (+12, +16)
extern "C" void __fastcall Run4914c0_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_run64bf38;
    CallC(FUN_491520, F(int, s, 12), F(int, s, 16));
    F(void*, s, 0) = g_run64bee4;
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

// [FUN_490ea0] a pair of strings: frees each one with a length (+2 word)
extern "C" void __fastcall StrPair_Destroy(BYTE* s)
{
    BYTE* a = F(BYTE*, s, 4);
    if (a && F(WORD, a, 2))
        operator delete(a);
    BYTE* b = F(BYTE*, s, 0);
    if (b && F(WORD, b, 2))
        operator delete(b);
}
