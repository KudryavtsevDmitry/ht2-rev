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
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" size_t __cdecl fread(void* buf, size_t size, size_t n, FILE* f);
extern "C" long __cdecl ftell(FILE* f);
extern "C" int __cdecl fseek(FILE* f, long off, int origin);
extern "C" void** __stdcall List_Erase(void** out, void** node);     // aimain.cpp
extern "C" double __cdecl Int_ToDouble(const void* n);
extern "C" void* __cdecl Int_FromInt(int v, int a);
KING_ALIAS("_Int_FromInt", "_BigInt_FromInt")
extern "C" void __cdecl Poly_48d8f0(void* out, void* a, int b, int n, int c);
KING_ALIAS("_Poly_48d8f0", "_Spline_Pieces")
extern "C" void __cdecl AI_Error4a54e0(int code, int a, const char* name);
KING_ALIAS("_AI_Error4a54e0", "_Sch_Error")
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert(pos, first, last)
extern "C" void FUN_490d30(); KING_ALIAS("_FUN_490d30", "_Rational_Error@4")
extern "C" void FUN_48af50(); KING_ALIAS("_FUN_48af50", "_Int_CompareMagnitude")
extern "C" void FUN_48b8a0(); KING_ALIAS("_FUN_48b8a0", "_BigInt_Quotient")
extern "C" void FUN_491010(); KING_ALIAS("_FUN_491010", "@RingMember2_Destruct@8")
extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")
ASM_VAR(AsmLabel, s_divByZero, $L_67055c)
extern "C" void FUN_470950(); KING_ALIAS("_FUN_470950", "@CharVec_Assign@12")
extern "C" void FUN_48ae10(); KING_ALIAS("_FUN_48ae10", "_BigInt_FromUnsigned")
extern "C" void FUN_48acd0(); KING_ALIAS("_FUN_48acd0", "_WStr_Assign")
ASM_PROC(FUN_491420)
extern "C" void FUN_48c260(); KING_ALIAS("_FUN_48c260", "_BigInt_Gcd")
ASM_VAR(AsmLabel, s_bigint, $L_670408)
ASM_VAR(AsmLabel, s_bigintTooLarge, $L_670410)
ASM_VAR(AsmLabel, s_bigintNull, $L_670430)
ASM_VAR(AsmLabel, g_vtbl64bf38, $L_64bf38)
ASM_VAR(AsmLabel, g_vtbl64bf24, $L_64bf24)
extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")    // vector<char>::insert(pos, first, last) (another instance)
extern "C" void FUN_470900(); KING_ALIAS("_FUN_470900", "@CharVec_Construct@12")
extern "C" void FUN_4897b0(); KING_ALIAS("_FUN_4897b0", "@List_NewNode@12")    // list: a new node
ASM_PROC(FUN_491750)
extern "C" void FUN_488450(); KING_ALIAS("_FUN_488450", "@RoadSet_Destroy@4")
extern "C" void FUN_489900(); KING_ALIAS("_FUN_489900", "@Vec24_InsertAux@16")    // vector<24 bytes>::insert_aux
extern "C" void FUN_48f730(); KING_ALIAS("_FUN_48f730", "@Vec580_InsertAux@16")    // vector<580 bytes>::insert_aux

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
extern "C" void FUN_4890b0(); KING_ALIAS("_FUN_4890b0", "@Rt_ReadRoads@12")
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
extern "C" void FUN_491090(); KING_ALIAS("_FUN_491090", "@Ring491090_Destruct@8")
extern "C" void FUN_491100(); KING_ALIAS("_FUN_491100", "@Rec491100_Destroy@8")
extern "C" void FUN_4914c0(); KING_ALIAS("_FUN_4914c0", "@Run4914c0_Destroy@4")
extern "C" void FUN_491590(); KING_ALIAS("_FUN_491590", "@Task491590_Destruct@8")
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

// ---------------------------------------------------------------------------
// Destructor thunks: the vtable, then the base destructor; static object cleanups.

extern "C" void FUN_491010(); KING_ALIAS("_FUN_491010", "@RingMember2_Destruct@8")
ASM_VAR(AsmLabel, g_q46_64bef4, $L_64bef4)
// [FUN_491090] destructor: the 64bef4 vtable, then the ring member
extern "C" void __fastcall Ring491090_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = g_q46_64bef4;
    CallThis(FUN_491010, s);
}

// ---------------------------------------------------------------------------
// Small methods and static helpers: guarded static construction, releases, frees.

ASM_VAR(BYTE*, g_q49_691cd0, $L_691cd0)
// [$L_490d90] frees the 691cd0 block when its +2 word is set
extern "C" void __cdecl Block691cd0_Free(void)
{
    if (g_q49_691cd0 && F(WORD, g_q49_691cd0, 2))
        operator delete(g_q49_691cd0);
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
// returns the storage of a vector (+0 .. +8) of size-byte elements to the pool
static void PoolVec_Free_U(BYTE* s, int size)
{
    BYTE* p = F(BYTE*, s, 0);
    int n = (F(BYTE*, s, 8) - p) / size;
    if (n)
        CallC(FUN_44b4a0, p, n * size);
}
// [$L_491720] frees the storage of a vector of 44-byte records
extern "C" void __fastcall Vec44_FreeStorage(BYTE* s, void*)
{
    PoolVec_Free_U(s, 44);
}

// ---------------------------------------------------------------------------
// Stepping along a polyline and polynomial products.

ASM_VAR(double, g_q113_64b678, $L_64b678)
// [FUN_48d530] steps index *i of the 24-byte points of s (+32..+36) forward when t is above 64b678, else back;
// the fraction *f becomes 0 (1 at the second last point); 1 when it cannot move
extern "C" int __fastcall Path_StepIndex(BYTE* s, void*, double* f, double t, unsigned* i)
{
    if (t > g_q113_64b678) {
        int n = (F(int, s, 36) - F(int, s, 32)) / 24;
        if (*i >= (unsigned)(n - 1))
            return 1;
        if (*i >= (unsigned)(n - 2)) {
            if (F(DWORD, f, 0) == 0 && F(DWORD, f, 4) == 0x3ff00000)
                return 1;
            *f = 1.0;
            return 0;
        }
        ++*i;
    } else {
        if ((int)*i <= 0)
            return 1;
        --*i;
    }
    *f = 0.0;
    return 0;
}

// [FUN_48eae0] the product of polynomials s and b (doubles, count at +192) into out
extern "C" BYTE* __fastcall Poly_Multiply(BYTE* s, void*, BYTE* out, BYTE* b)
{
    BYTE r[196];
    F(int, r, 192) = 0;
    const double* a = (const double*)s;
    const double* c = (const double*)b;
    int na = F(int, s, 192), nb = F(int, b, 192);
    if (na && nb)
        for (unsigned k = 0; k < (unsigned)(na + nb - 1); k++) {
            double acc = g_q113_64b678;
            int lo = k >= (unsigned)na ? k - na + 1 : 0;
            int hi = nb < (int)(k + 1) ? nb : k + 1;
            for (int j = lo; j < hi; j++)
                acc += a[k - j] * c[j];
            F(double, r, F(int, r, 192) * 8) = acc;
            F(int, r, 192)++;
            na = F(int, s, 192);
            nb = F(int, b, 192);
        }
    memcpy(out, r, 196);
    return out;
}

// [FUN_48ef00] the derivative of vector polynomial s (24-byte coefficients, highest first, count at +576) into
// out
extern "C" BYTE* __fastcall VPoly_Derivative(BYTE* s, void*, BYTE* out)
{
    BYTE r[580];
    F(int, r, 576) = 0;
    const double* a = (const double*)s;
    for (unsigned i = 0; i < (unsigned)(F(int, s, 576) * 24 / 24 - 1); i++) {
        double k = (double)(__int64)(unsigned)(F(int, s, 576) * 24 / 24 - i - 1);
        double* d = (double*)(r + F(int, r, 576) * 24);
        d[0] = a[i * 3] * k;
        d[1] = a[i * 3 + 1] * k;
        d[2] = a[i * 3 + 2] * k;
        F(int, r, 576)++;
    }
    memcpy(out, r, 580);
    return out;
}

// [FUN_48f1c0] vector polynomial s with each coefficient scaled by v component-wise, into out
extern "C" BYTE* __fastcall VPoly_Scale(BYTE* s, void*, BYTE* out, const double* v)
{
    double k[3] = {v[0], v[1], v[2]};
    BYTE r[580];
    F(int, r, 576) = 0;
    const double* a = (const double*)s;
    for (unsigned i = 0; i < (unsigned)(F(int, s, 576) * 24 / 24); i++) {
        double* d = (double*)(r + F(int, r, 576) * 24);
        d[0] = a[i * 3] * k[0];
        d[1] = a[i * 3 + 1] * k[1];
        d[2] = k[2] * a[i * 3 + 2];
        F(int, r, 576)++;
    }
    memcpy(out, r, 580);
    return out;
}

ASM_VAR(double, g_q115_64b678, $L_64b678)
ASM_VAR(double, g_q115_64bea8, $L_64bea8)
ASM_VAR(double, g_q115_64b670, $L_64b670)
ASM_VAR(double, g_q115_6865d8, $L_6865d8)
// [FUN_48a750] the value of big integer n (word count +0, sign +4: 0 negative, words from +6, last the highest);
// at the 64bea8 limit the 6865d8 one
extern "C" double __cdecl Int_ToDouble(const void* n)
{
    const BYTE* p = (const BYTE*)n;
    double v = g_q115_64b678;
    for (int i = F(WORD, p, 0) - 1; i >= 0; i--)
        for (WORD bit = 0x8000; bit; bit >>= 1) {
            if (v >= g_q115_64bea8)
                return F(short, p, 4) ? g_q115_6865d8 : -g_q115_6865d8;
            v += v;
            if (F(WORD, p, 6 + i * 2) & bit)
                v += g_q115_64b670;
        }
    return F(short, p, 4) ? v : -v;
}

// [$L_48d600] reverses the 24-byte points of s (+32..+36)
extern "C" void __fastcall Points_Reverse(BYTE* s)
{
    BYTE* b = F(BYTE*, s, 32);
    BYTE* e = F(BYTE*, s, 36);
    while (b < e) {
        e -= 24;
        BYTE t[24];
        memcpy(t, b, 24);
        memcpy(b, e, 24);
        memcpy(e, t, 24);
        b += 24;
    }
}

// [$L_48d670] the length of the polyline of s (24-byte points +32..+36)
extern "C" double __fastcall Points_Length(BYTE* s)
{
    double len = g_q115_64b678;
    const double* p = F(const double*, s, 32);
    unsigned n = (F(int, s, 36) - F(int, s, 32)) / 24 - 1;
    for (unsigned i = 0; i < n; i++, p += 3) {
        double dx = p[3] - p[0], dy = p[4] - p[1], dz = p[5] - p[2];
        len += x87_sqrt(dy * dy + dz * dz + dx * dx);
    }
    return len;
}

// [FUN_4903a0] vector polynomial s with each coefficient divided by v component-wise, into out
extern "C" BYTE* __fastcall VPoly_Divide(BYTE* s, void*, BYTE* out, const double* v)
{
    double kx = g_q115_64b670 / v[0], ky = g_q115_64b670 / v[1], kz = g_q115_64b670 / v[2];
    BYTE r[580];
    F(int, r, 576) = 0;
    const double* a = (const double*)s;
    for (unsigned i = 0; i < (unsigned)(F(int, s, 576) * 24 / 24); i++) {
        double* d = (double*)(r + F(int, r, 576) * 24);
        d[0] = a[i * 3] * kx;
        d[1] = a[i * 3 + 1] * ky;
        d[2] = a[i * 3 + 2] * kz;
        F(int, r, 576)++;
    }
    memcpy(out, r, 580);
    return out;
}

// [FUN_48f000] the component-wise product of vector polynomials s and b into out
extern "C" BYTE* __fastcall VPoly_Multiply(BYTE* s, void*, BYTE* out, BYTE* b)
{
    BYTE r[580];
    F(int, r, 576) = 0;
    const double* a = (const double*)s;
    const double* c = (const double*)b;
    unsigned na = F(int, s, 576) * 24 / 24, nb = F(int, b, 576) * 24 / 24;
    if (na && nb)
        for (unsigned k = 0; k < na + nb - 1; k++) {
            double x = g_q115_64b678, y = g_q115_64b678, z = g_q115_64b678;
            int lo = k >= na ? k - na + 1 : 0;
            int hi = (int)nb < (int)(k + 1) ? (int)nb : (int)(k + 1);
            for (int j = lo; j < hi; j++) {
                const double* p = a + (k - j) * 3;
                const double* q = c + j * 3;
                double px = p[0] * q[0], py = p[1] * q[1], pz = p[2] * q[2];
                x += px;
                y += py;
                z += pz;
            }
            double* d = (double*)(r + F(int, r, 576) * 24);
            d[0] = x;
            d[1] = y;
            d[2] = z;
            F(int, r, 576)++;
            na = F(int, s, 576) * 24 / 24;
            nb = F(int, b, 576) * 24 / 24;
        }
    memcpy(out, r, 580);
    return out;
}

// ---------------------------------------------------------------------------
// The bounding sphere of points.

extern "C" void FUN_487ee0(); KING_ALIAS("_FUN_487ee0", "@DBox_Init@16")
extern "C" void FUN_487f60(); KING_ALIAS("_FUN_487f60", "@DBox_Extend@12")
ASM_VAR(double, g_q117_64b6e0, $L_64b6e0)
// [FUN_490a20] the sphere (centre, radius) around the points of vector v (24-byte doubles): the centre of their
// box and the largest distance from it
extern "C" double* __cdecl Points_BoundingSphere(double* out, const double* const* v)
{
    double box[6];
    const double* p = v[0];
    CallThis(FUN_487ee0, box, p, p + 3);
    for (unsigned i = 2; i < (unsigned)((v[1] - v[0]) / 3); i++)
        CallThis(FUN_487f60, box, v[0] + i * 3);
    double sx = box[3] + box[0], sy = box[4] + box[1], sz = box[5] + box[2];
    double s[4] = {sx * g_q117_64b6e0, sy * g_q117_64b6e0, sz * g_q117_64b6e0, 0.0};
    unsigned n = (unsigned)((v[1] - v[0]) / 3);
    for (p = v[0]; n; n--, p += 3) {
        double dx = s[0] - p[0], dy = s[1] - p[1], dz = s[2] - p[2];
        double d = x87_sqrt(dx * dx + dy * dy + dz * dz);
        if (s[3] < d)
            s[3] = d;
    }
    memcpy(out, s, 32);
    return out;
}

// ---------------------------------------------------------------------------
// Moving along a curve by a distance.

extern "C" void FUN_48e3c0(); KING_ALIAS("_FUN_48e3c0", "@Poly1392_Diff@24")
// [FUN_48df80] moves parameter *t of curve s by distance *d (48e3c0 measures the curve between two parameters):
// when the rest of the curve is shorter, *t goes to its end and the rest of *d stays (1); else six secant steps
// refine the parameter step *x, *t moves by it (kept in 0..1) and *d becomes 0 (0)
extern "C" int __fastcall Curve_Advance(BYTE* s, void*, double* t, double* d, double* x)
{
    double t0 = *t;
    if (*d > g_q113_64b678) {
        double v = CallThis<double>(FUN_48e3c0, s, t0, 1.0);
        if (!(v > *d)) {
            *t = 1.0;
            *d = *d - v;
            if (*d < g_q113_64b678)
                *d = 0.0;
            return 1;
        }
    } else if (*d < g_q113_64b678) {
        double v = CallThis<double>(FUN_48e3c0, s, t0, 0.0);
        if (!(v < *d)) {
            *t = 0.0;
            *d = *d - v;
            if (!(*d <= g_q113_64b678))
                *d = 0.0;
            return 1;
        }
    }
    for (int k = 6; k; k--)
        *x = *d / CallThis<double>(FUN_48e3c0, s, t0, t0 + *x) * *x;
    double nt = *x + t0;
    *t = nt;
    if (!(nt <= g_q115_64b670))
        *t = 1.0;
    else if (nt < g_q113_64b678)
        *t = 0.0;
    *d = 0.0;
    return 0;
}

// the length of segment i of polyline points p
static double Run119_SegLen(const double* p, int i)
{
    const double* a = p + i * 3;
    double dx = a[3] - a[0], dy = a[4] - a[1], dz = a[5] - a[2];
    return x87_sqrt(dy * dy + dz * dz + dx * dx);
}
// [FUN_48d710] the distance along the polyline of s (24-byte points +32..+36) from fraction t1 of segment i1 to
// fraction t2 of segment i2 (an index at the last point counts as the end of the segment before it)
extern "C" double __fastcall Points_Distance(BYTE* s, void*, double t1, int i1, double t2, int i2)
{
    const double* p = F(const double*, s, 32);
    int n = (F(int, s, 36) - F(int, s, 32)) / 24;
    if (i1 + 1 == n) {
        t1 += g_q115_64b670;
        i1--;
    }
    if (i2 + 1 == n) {
        t2 += g_q115_64b670;
        i2--;
    }
    if (i1 == i2)
        return Run119_SegLen(p, i1) * (t2 - t1);
    double len = Run119_SegLen(p, i1) * (g_q115_64b670 - t1);
    for (int j = i1 + 1; j < i2; j++)
        len += Run119_SegLen(p, j);
    return len + Run119_SegLen(p, i2) * t2;
}

// ---------------------------------------------------------------------------
// Static initializers: construct a global, register its teardown at exit.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void q197_FUN_490d60(); KING_ALIAS("_q197_FUN_490d60", "_Int_InitOne")
extern "C" void q197_L_490d90(); KING_ALIAS("_q197_L_490d90", "_Block691cd0_Free")
// [$L_490d80] registers the teardown at exit
extern "C" int __cdecl Static490d50_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_490d90); }
// [$L_490d50] the static initializer: FUN_490d60, then the teardown at exit
extern "C" int __cdecl Static490d50_Init(void)
{
    CallC(q197_FUN_490d60);
    return Static490d50_AtExit();
}

extern "C" void FUN_490c00(); KING_ALIAS("_FUN_490c00", "_Save_SkipBlock")
ASM_VAR(char, g_q199_64bee4, $L_64bee4)
// [$L_489760] skips the rest of the load block (the stream at *s[0], start, size)
extern "C" int __fastcall LoadBlock_Skip(BYTE* s)
{
    return CallC<int>(FUN_490c00, *F(int*, s, 0), F(int, s, 4), F(int, s, 8));
}
// [$L_4913f0] sets the base vtable 64bee4
extern "C" void __fastcall Run4913f0_SetVtbl(BYTE* s) { F(void*, s, 0) = &g_q199_64bee4; }

extern "C" void FUN_488450(); KING_ALIAS("_FUN_488450", "@RoadSet_Destroy@4")
// [$L_489600] destroys a list of 48-byte nodes (each value at +8 destroyed by 488450), then the head
extern "C" void __fastcall List48_Destroy(BYTE** s)
{
    for (BYTE* n = *(BYTE**)*s; n != *s;) {
        BYTE* next = *(BYTE**)n;
        CallThis(FUN_488450, n + 8);
        CallC(FUN_44b4a0, n, 48);
        n = next;
    }
    BYTE* h = *s;
    F(BYTE*, h, 0) = h;
    F(BYTE*, *s, 4) = *s;
    CallC(FUN_44b4a0, *s, 48);
}

// === reading a road object block of the save file (FUN_488a90)

ASM_VAR(DWORD, g_sv6703b4, $L_6703b4)
ASM_VAR(DWORD, g_sv6703ac, $L_6703ac)
ASM_VAR(DWORD, g_sv6703a4, $L_6703a4)
ASM_VAR(DWORD, g_sv67039c, $L_67039c)
ASM_VAR(DWORD, g_sv670394, $L_670394)
ASM_VAR(DWORD, g_sv67038c, $L_67038c)
ASM_VAR(DWORD, g_sv670384, $L_670384)
ASM_VAR(DWORD, g_sv67037c, $L_67037c)
ASM_VAR(DWORD, g_sv67036c, $L_67036c)
ASM_VAR(DWORD, g_sv670374, $L_670374)
extern "C" void FUN_48a2c0(); KING_ALIAS("_FUN_48a2c0", "@Vec4_InsertAux48a2c0@16")    // vector<void*>::insert_aux
extern "C" void FUN_48a000(); KING_ALIAS("_FUN_48a000", "@Vec4_InsertAux48a000@16")    // vector<void*>::insert_aux
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")
extern "C" void FUN_44c470(); KING_ALIAS("_FUN_44c470", "@Vec8_Insert44c470@16")

static bool Sv_More(FILE* f, long pos, int size)
{
    return pos + size - ftell(f) != 0;
}

// push_back of pointer x to the vector at v
static void Sv_PushPtr(BYTE** v, BYTE* x, void (*insert)())
{
    if (v[1] != v[2]) {
        if (v[1])
            *(BYTE**)v[1] = x;
        v[1] += 4;
    } else
        CallThis(insert, v, v[1], &x);
}

static BYTE* Sv_Last(BYTE* self, int off)
{
    return F(BYTE**, self, off)[-1];
}

extern "C" {

// [FUN_488a90] reads a block of the save file into the object (thiscall): an object block (6703b4) adds a
// 144-byte record (+24) with its name, position, matrix and value; a list block (67038c) adds 56-byte records
// (+12) with their values, names, points (+16, 24 bytes each) and pairs (+28, 8 bytes each); 0
int __fastcall SaveBlock_Read(BYTE* self, void*, FILE* f)
{
    DWORD tag;
    long pos;
    int size;
    Save_ReadHeader(f, &tag, &pos, &size);
    if (tag == g_sv6703b4) {
        DWORD id;
        long p2;
        int s2;
        char name[1132] = {0};
        Save_ReadHeader(f, &id, &p2, &s2);
        if (id == g_sv6703ac)
            fread(name, s2, 1, f);
        Save_SkipBlock(f, p2, s2);
        BYTE* r = (BYTE*)operator new(144);
        r = r ? Rec_Init488900(r, 0, (int)name, 0) : 0;
        Sv_PushPtr((BYTE**)(self + 24), r, FUN_48a2c0);
        while (Sv_More(f, pos, size)) {
            Save_ReadHeader(f, &id, &p2, &s2);
            if (id == g_sv6703a4) {
                DVec3 v;
                Save_ReadVec(&v, f);
                memcpy(Sv_Last(self, 28) + 16, &v, 24);
            } else if (id == g_sv67039c) {
                double m[12];
                Save_ReadMatrix(m, f);
                memcpy(Sv_Last(self, 28) + 40, m, 96);
                memcpy(Sv_Last(self, 28) + 16, m + 9, 24);
                F(BYTE, Sv_Last(self, 28), 140) = 1;
            } else if (id == g_sv670394) {
                int v;
                fread(&v, 4, 1, f);
                F(int, Sv_Last(self, 28), 136) = v;
            }
            Save_SkipBlock(f, p2, s2);
        }
    } else if (tag == g_sv67038c) {
        while (Sv_More(f, pos, size)) {
            DWORD id;
            long p2;
            int s2;
            Save_ReadHeader(f, &id, &p2, &s2);
            if (id == g_sv670384) {
                DWORD flags;
                fread(&flags, 4, 1, f);
                DWORD* o = (DWORD*)operator new(56);
                if (o)
                    o = (flags & 1) ? Rec_Init488880a(o, 0, flags) : Rec_Init488880b(o, 0, flags);
                Sv_PushPtr((BYTE**)(self + 12), (BYTE*)o, FUN_48a000);
                double d;
                fread(&d, 8, 1, f);
                F(double, Sv_Last(self, 16), 0) = d;
                int v;
                fread(&v, 4, 1, f);
                F(int, Sv_Last(self, 16), 8) = v;
            } else if (id == g_sv67037c) {
                char text[1024];
                fread(text, s2, 1, f);
                BYTE* last = Sv_Last(self, 16);
                BYTE* end = F(BYTE*, last, 44);
                size_t n = strlen(text);
                CallThis(FUN_44c220, last + 40, end, (BYTE*)text, (BYTE*)text + n + 1);
            } else if (id == g_sv67036c) {
                int count;
                fread(&count, 4, 1, f);
                for (int i = count; i > 0; i--) {
                    DVec3 v;
                    Save_ReadVec(&v, f);
                    BYTE** vec = (BYTE**)(Sv_Last(self, 16) + 16);
                    if (vec[1] != vec[2]) {
                        if (vec[1])
                            memcpy(vec[1], &v, 24);
                        vec[1] += 24;
                    } else
                        CallThis(FUN_489900, vec, vec[1], &v);
                }
            } else if (id == g_sv670374) {
                while (Sv_More(f, p2, s2)) {
                    DWORD pair[2];
                    fread(pair, 8, 1, f);
                    BYTE** vec = (BYTE**)(Sv_Last(self, 16) + 28);
                    if (vec[1] != vec[2]) {
                        if (vec[1])
                            memcpy(vec[1], pair, 8);
                        vec[1] += 8;
                    } else
                        CallThis(FUN_44c470, vec, vec[1], pair);
                }
            }
            Save_SkipBlock(f, p2, s2);
        }
    }
    Save_SkipBlock(f, pos, size);
    return 0;
}

} // extern "C"

// === reading the road file (FUN_4890b0)

ASM_VAR(DWORD, g_rf6703dc, $L_6703dc)
ASM_VAR(DWORD, g_rf6703d4, $L_6703d4)
ASM_VAR(DWORD, g_rf6703cc, $L_6703cc)
ASM_VAR(DWORD, g_rf6703c4, $L_6703c4)
ASM_VAR(DWORD, g_rf6703bc, $L_6703bc)
extern "C" void FUN_4afc20(); KING_ALIAS("_FUN_4afc20", "@SchList_End@12")

// a named road (6703c4: its name block 6703bc, then its object blocks) appended to the list at self+24
static void Rf_Road(BYTE* self, FILE* f, long pos, int size)
{
    DWORD id;
    long p;
    int n;
    char name[1024] = {0};
    Save_ReadHeader(f, &id, &p, &n);
    if (id == g_rf6703bc)
        fread(name, n, 1, f);
    Save_SkipBlock(f, p, n);
    DWORD rec[9];
    Rec4883f0_Construct(rec, 0, (int)name);
    BYTE* end;
    CallThis(FUN_4afc20, self + 24, &end);
    BYTE* it;
    List489_Insert(self + 24, 0, &it, (BYTE**)end, rec);
    CallThis(FUN_488450, rec);
    while (Sv_More(f, pos, size))
        SaveBlock_Read(F(BYTE*, F(BYTE*, self, 24), 4) + 8, 0, f);
}

// the roads of a 6703cc block
static void Rf_Roads(BYTE* self, FILE* f, long pos, int size)
{
    while (Sv_More(f, pos, size)) {
        DWORD id;
        long p;
        int n;
        Save_ReadHeader(f, &id, &p, &n);
        if (id == g_rf6703c4)
            Rf_Road(self, f, p, n);
        Save_SkipBlock(f, p, n);
    }
}

extern "C" {

// [FUN_4890b0] reads the road file f into self (thiscall): a 6703dc block holds names (6703d4, appended to the
// text at self+0) and road groups (6703cc); a 6703cc block alone holds roads; 0
int __fastcall Rt_ReadRoads(BYTE* self, void*, FILE* f)
{
    DWORD tag;
    long pos;
    int size;
    Save_ReadHeader(f, &tag, &pos, &size);
    if (tag == g_rf6703dc) {
        while (Sv_More(f, pos, size)) {
            DWORD id;
            long p;
            int n;
            Save_ReadHeader(f, &id, &p, &n);
            if (id == g_rf6703d4) {
                char text[128];
                fread(text, n, 1, f);
                CallThis(FUN_44c220, self, F(BYTE*, self, 4), text, text + strlen(text) + 1);
            } else if (id == g_rf6703cc)
                Rf_Roads(self, f, p, n);
            Save_SkipBlock(f, p, n);
        }
    } else if (tag == g_rf6703cc)
        Rf_Roads(self, f, pos, size);
    Save_SkipBlock(f, pos, size);
    return 0;
}

} // extern "C"

// === adding big integers (FUN_48afa0)

extern "C" {

// [FUN_48afa0] out = (+-a) + (+-b) (na / nb flip the signs of non-zero a / b): magnitudes added for equal
// signs, else the smaller subtracted from the larger (sign of the larger); out may be a or b
WORD* __cdecl Int_AddSigned(WORD* a, int na, WORD* b, int nb, WORD* out)
{
    if (!a)
        BigInt_Fail(s_bigintNull);
    if (!b)
        BigInt_Fail(s_bigintNull);
    int la = a[0], lb = b[0];
    short sa = (na && la) ? (a[2] == 0) : (short)a[2];
    short sb = (nb && lb) ? (b[2] == 0) : (short)b[2];
    bool aOut = a == out, bOut = b == out;
    WORD* r;
    if (lb == 0)
        return Int_Normalize(WStr_Make(out, a + 3, la, sa, la));
    if (la == 0)
        return Int_Normalize(WStr_Make(out, b + 3, lb, sb, lb));
    int mx = la > lb ? la : lb;
    if (sa == sb) {
        r = (aOut || bOut) ? WStr_Resize(out, mx + 1) : BigInt_Alloc(out, mx + 1);
        r[2] = sa;
        WORD* d = r + 3;
        const WORD *lo, *loEnd, *sh, *shEnd;
        if (la >= lb) {
            lo = aOut ? d : a + 3;
            loEnd = lo + la;
            sh = bOut ? d : b + 3;
            shEnd = sh + lb;
        } else {
            sh = aOut ? d : a + 3;
            shEnd = sh + la;
            lo = bOut ? d : b + 3;
            loEnd = lo + lb;
        }
        unsigned c = 0;
        while (sh < shEnd) {
            unsigned s = *sh++ + c + *lo++;
            *d++ = (WORD)s;
            c = s >> 16;
        }
        while (c) {
            if (lo >= loEnd) {
                *d = (WORD)c;
                return Int_Normalize(r);
            }
            unsigned s = *lo++ + c;
            *d++ = (WORD)s;
            c = s >> 16;
        }
        if (d != lo)
            while (lo < loEnd)
                *d++ = *lo++;
        return Int_Normalize(r);
    }
    int cmp = Int_CompareMagnitude((const BigInt*)a, (const BigInt*)b);
    if (cmp == 0)
        return Int_Normalize((WORD*)Int_SetZero((BigInt*)out));
    r = (aOut || bOut) ? WStr_Resize(out, mx) : BigInt_Alloc(out, mx);
    WORD* d = r + 3;
    const WORD *lo, *loEnd, *sh, *shEnd;
    if (cmp > 0) {
        lo = aOut ? d : a + 3;
        loEnd = lo + la;
        sh = bOut ? d : b + 3;
        shEnd = sh + lb;
        r[2] = sa;
    } else {
        sh = aOut ? d : a + 3;
        shEnd = sh + la;
        lo = bOut ? d : b + 3;
        loEnd = lo + lb;
        r[2] = sb;
    }
    unsigned c = 1;
    while (sh < shEnd) {
        unsigned s = c + *lo++ - *sh++ + 65535;
        *d++ = (WORD)s;
        c = s >> 16;
    }
    if (!c)
        while (lo < loEnd) {
            unsigned s = *lo++ + 65535u;
            *d++ = (WORD)s;
            if (s & 0xffff0000)
                break;
        }
    if (d != lo)
        while (lo < loEnd)
            *d++ = *lo++;
    return Int_Normalize(r);
}

} // extern "C"

// === multiplying big integers (FUN_48b350)

// adds v at d[k] and carries up to n
static void Im_Add(WORD* d, int k, int n, unsigned v)
{
    unsigned c = d[k] + v;
    d[k] = (WORD)c;
    c >>= 16;
    for (int i = k + 1; c && i < n; i++) {
        c += d[i];
        d[i] = (WORD)c;
        c >>= 16;
    }
}

extern "C" {

// [FUN_48b350] out = a * b (out may be a or b; a == b == out squares in place); the sign is + for equal signs
WORD* __cdecl Int_Mul(WORD* a, WORD* b, WORD* out)
{
    if (!a)
        BigInt_Fail(s_bigintNull);
    if (!b)
        BigInt_Fail(s_bigintNull);
    WORD sign = a[2] == b[2];
    int la = a[0], lb = b[0], n = la + lb;
    bool aOut = a == out, bOut = b == out;
    WORD* r;
    if (!la || !lb)
        r = (WORD*)Int_SetZero((BigInt*)out);
    else if (la == 1 && a[3] == 1)
        r = WStr_Assign(out, b);
    else if (lb == 1 && b[3] == 1)
        r = WStr_Assign(out, a);
    else if (a == b && aOut) {
        r = WStr_Resize(out, n);
        WORD* d = r + 3;
        int hi = la - 1, lo = la - 1;
        for (int k = n - 2; k >= 0; k--) {
            int i = lo, j = hi;
            unsigned p = (unsigned)d[j] * d[i];
            d[k] = 0;
            for (;;) {
                Im_Add(d, k, n, p);
                if (j > i)
                    Im_Add(d, k, n, p);
                j--;
                i++;
                if (j < i)
                    break;
                p = (unsigned)d[i] * d[j];
            }
            if (lo > 0)
                lo--;
            else
                hi--;
        }
    } else {
        r = (aOut || bOut) ? WStr_Resize(out, n) : BigInt_Alloc(out, n);
        WORD* d = r + 3;
        const WORD *rd, *rdLow, *other;
        int ol, w;
        if (aOut) {
            w = la - 1;
            rdLow = d;
            rd = d + la - 1;
            other = b + 3;
            ol = lb;
        } else if (bOut) {
            w = lb - 1;
            rdLow = d;
            rd = d + lb - 1;
            other = a + 3;
            ol = la;
        } else if (la <= lb) {
            w = la - 1;
            rdLow = a + 3;
            rd = a + 3 + la - 1;
            other = b + 3;
            ol = lb;
        } else {
            w = lb - 1;
            rdLow = b + 3;
            rd = b + 3 + lb - 1;
            other = a + 3;
            ol = la;
        }
        for (; rd >= rdLow; w--) {
            unsigned m = *rd--;
            d[w] = 0;
            if (!m)
                continue;
            unsigned c = 0;
            int k = w;
            for (int i = 0; i < ol; i++, k++) {
                c = other[i] * m + d[k] + c;
                d[k] = (WORD)c;
                c >>= 16;
            }
            for (; c && k < n; k++) {
                c += d[k];
                d[k] = (WORD)c;
                c >>= 16;
            }
        }
    }
    r[2] = sign;
    return Int_Normalize(r);
}

} // extern "C"

// === inserting a range into a vector of 24-byte vectors (FUN_489c30)

extern "C" void FUN_405f70(); KING_ALIAS("_FUN_405f70", "@DVec3_Assign@12")
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
extern "C" void FUN_48a6a0(); KING_ALIAS("_FUN_48a6a0", "_CopyBackward24s")
extern "C" void FUN_48a6e0(); KING_ALIAS("_FUN_48a6e0", "_Copy24s")
extern "C" void FUN_48a720(); KING_ALIAS("_FUN_48a720", "_UninitCopy24s")

static BYTE* Ri24_UninitCopy(const BYTE* first, const BYTE* last, BYTE* to, const BYTE* x)
{
    return CallC<BYTE*>(FUN_48a720, first, last, to, x);
}

extern "C" {

// [FUN_489c30] vector<DVec3>::insert(pos, first, last) (thiscall on v: begin, end, capacity)
void __fastcall VecD3_InsertRange(BYTE** v, void*, BYTE* pos, const BYTE* first, const BYTE* last)
{
    if (first == last)
        return;
    unsigned n = (unsigned)((last - first) / 24);
    if ((unsigned)((v[2] - v[1]) / 24) >= n) {
        BYTE* end = v[1];
        unsigned after = (unsigned)((end - pos) / 24);
        if (after > n) {
            Ri24_UninitCopy(end - n * 24, end, end, first);
            v[1] += n * 24;
            CallC(FUN_48a6a0, pos, end - n * 24, end, first, 0);
            CallC(FUN_48a6e0, first, last, pos, first, 0);
        } else {
            Ri24_UninitCopy(first + after * 24, last, end, first);
            v[1] += (n - after) * 24;
            Ri24_UninitCopy(pos, end, v[1], first);
            v[1] += after * 24;
            for (unsigned k = 0; k < after; k++)
                CallThis(FUN_405f70, pos + k * 24, first + k * 24);
        }
        return;
    }
    unsigned size = (unsigned)((v[1] - v[0]) / 24);
    unsigned len = size + (size < n ? n : size);
    BYTE* p = len ? CallC<BYTE*>(FUN_44b740, len * 24) : 0;
    BYTE* e = Ri24_UninitCopy(v[0], pos, p, p);
    e = Ri24_UninitCopy(first, last, e, p);
    e = Ri24_UninitCopy(pos, v[1], e, p);
    if (unsigned cap = (unsigned)((v[2] - v[0]) / 24))
        CallC(FUN_44b4a0, v[0], cap * 24);
    v[0] = p;
    v[1] = e;
    v[2] = p + len * 24;
}

} // extern "C"

// === a cubic B-line from four points (FUN_4904e0)

extern "C" void FUN_447270(); KING_ALIAS("_FUN_447270", "@Vec24_FreeStorage@8")
extern "C" void FUN_44a530(); KING_ALIAS("_FUN_44a530", "@Vec8_Free@8")
extern "C" void FUN_44c470(); KING_ALIAS("_FUN_44c470", "@Vec8_Insert44c470@16")
extern "C" void FUN_458e00(); KING_ALIAS("_FUN_458e00", "_Vec8b_AllocCopy@12")
extern "C" void FUN_4608f0(); KING_ALIAS("_FUN_4608f0", "_Construct8")
extern "C" void FUN_48e8c0(); KING_ALIAS("_FUN_48e8c0", "@Rec_Init48e8c0@12")
extern "C" void FUN_490820(); KING_ALIAS("_FUN_490820", "@Vec24_CopyConstruct@12")
extern "C" void FUN_490930(); KING_ALIAS("_FUN_490930", "@Vec8_Copy@12")

// push_back of d to the vector of doubles v
static void Bl_PushKnot(BYTE** v, double d)
{
    if (v[1] != v[2]) {
        CallC(FUN_4608f0, v[1], &d);
        v[1] += 8;
    } else
        CallThis(FUN_44c470, v, v[1], &d);
}

extern "C" {

// [FUN_4904e0] out = the cubic B-line through the four points pts (24 bytes each): knots 0 0 0 0 1 1 1 1
BYTE* __cdecl BLine_FromPoints(BYTE* out, const BYTE* pts)
{
    BYTE r[28];
    CallThis(FUN_48e8c0, r, 3);
    BYTE** knots = (BYTE**)(r + 16);
    if ((unsigned)((knots[2] - knots[0]) >> 3) < 8) {
        int n = (int)(knots[1] - knots[0]) >> 3;
        BYTE* p = CallStd<BYTE*>(FUN_458e00, 8, knots[0], knots[1]);
        if (int cap = (int)(knots[2] - knots[0]) >> 3)
            CallC(FUN_44b4a0, knots[0], cap * 8);
        knots[0] = p;
        knots[1] = p + n * 8;
        knots[2] = p + 64;
    }
    for (int i = 0; i < 4; i++)
        Bl_PushKnot(knots, 0.0);
    for (int i = 0; i < 4; i++)
        Bl_PushKnot(knots, 1.0);
    VecD3_InsertRange((BYTE**)(r + 4), 0, F(BYTE*, r, 8), pts, pts + 96);
    F(int, out, 0) = F(int, r, 0);
    CallThis(FUN_490820, out + 4, r + 4);
    CallThis(FUN_490930, out + 16, r + 16);
    CallThis(FUN_44a530, r + 16);
    CallThis(FUN_447270, r + 4);
    return out;
}

} // extern "C"

extern "C" {

// [FUN_48f2c0] vector polynomial s plus b into out, aligned at their last coefficients: the extra leading
// coefficients of the longer one, then the sums
BYTE* __fastcall VPoly_Add(BYTE* s, void*, BYTE* out, BYTE* b)
{
    int na = F(int, s, 576) * 24 / 24, nb = F(int, b, 576) * 24 / 24;
    if (!na) {
        memcpy(out, b, 580);
        return out;
    }
    if (!nb) {
        memcpy(out, s, 580);
        return out;
    }
    BYTE r[580];
    F(int, r, 576) = 0;
    const double* a = (const double*)s;
    const double* c = (const double*)b;
    BYTE* lo = na > nb ? s : b;
    int d = na > nb ? na - nb : nb - na, n = na > nb ? na : nb;
    for (int i = 0; i < d; i++)
        memcpy(r + F(int, r, 576)++ * 24, lo + i * 24, 24);
    for (int i = d; i < n; i++) {
        const double* p = na > nb ? a + i * 3 : a + (i - d) * 3;
        const double* q = na > nb ? c + (i - d) * 3 : c + i * 3;
        double* t = (double*)(r + F(int, r, 576)++ * 24);
        t[0] = q[0] + p[0];
        t[1] = q[1] + p[1];
        t[2] = q[2] + p[2];
    }
    memcpy(out, r, 580);
    return out;
}

} // extern "C"

// === vector<580 bytes>::insert_aux (FUN_48f730)

extern "C" {

// [FUN_48f730] inserts a copy of vector polynomial x (580 bytes) at pos of vector s: shifted up within the
// capacity, or copied into a new buffer of twice the size (1 when empty) with the old storage returned to the pool
void __fastcall Vec580_InsertAux(BYTE** s, void*, BYTE* pos, const BYTE* x)
{
    if (s[1] != s[2]) {
        BYTE* end = s[1];
        if (end)
            memcpy(end, end - 580, 580);
        s[1] = end + 580;
        BYTE t[580];
        memcpy(t, x, 580);
        for (BYTE* d = end - 580; d > pos; d -= 580)
            memcpy(d, d - 580, 580);
        memcpy(pos, t, 580);
        return;
    }
    int n = (int)(s[1] - s[0]) / 580;
    int cap = n ? n * 2 : 1;
    BYTE* buf = CallC<BYTE*>(FUN_44b740, cap * 580);
    BYTE* p = UninitCopy580s(s[0], pos, buf);
    if (p)
        memcpy(p, x, 580);
    BYTE* end = UninitCopy580s(pos, s[1], p + 580);
    if (int bytes = (int)(s[2] - s[0]) / 580 * 580)
        CallC(FUN_44b4a0, s[0], bytes);
    s[1] = end;
    s[0] = buf;
    s[2] = buf + cap * 580;
}

} // extern "C"

// === the greatest common divisor of big integers (FUN_48c260)

// the number of trailing zero bits of the first n words of a and b together (both when b is given)
static int Gcd_TrailingZeros(const WORD* a, int na, const WORD* b, int nb, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        unsigned wa = i < na ? a[3 + i] : 0, wb = b && i < nb ? b[3 + i] : 0;
        for (int j = 0; j < 16; j++, k++, wa >>= 1, wb >>= 1)
            if ((wa | wb) & 1)
                return k;
    }
    return k;
}

extern "C" {

// [FUN_48c260] gcd(a, b) by the binary algorithm, as a new big integer (positive)
WORD* __cdecl BigInt_Gcd(WORD* a, WORD* b)
{
    if (!a)
        g_onError((const char*)s_bigint, (int)(const char*)s_bigintNull);
    if (!b)
        g_onError((const char*)s_bigint, (int)(const char*)s_bigintNull);
    int lb = b[0], la = a[0];
    if (!lb)
        return WStr_Make(0, a + 3, la, 1, la);
    if (!la)
        return WStr_Make(0, b + 3, lb, 1, lb);
    WORD* u = WStr_Make(0, a + 3, la, 1, la);
    WORD* v = WStr_Make(0, b + 3, lb, 1, lb);
    int k = Gcd_TrailingZeros(u, la, v, lb, la < lb ? la : lb);
    if (k) {
        u = Int_Shift(u, -k, u);
        v = Int_Shift(v, -k, v);
    }
    WORD* t = (u[3] & 1) ? WStr_Make(0, v + 3, v[0], v[2] == 0, v[0]) : WStr_Make(0, u + 3, u[0], u[2], u[0]);
    while (t[0]) {
        if (int s = Gcd_TrailingZeros(t, t[0], 0, 0, t[0]))
            t = Int_Shift(t, -s, t);
        if (t[2] == 1) {
            u = WStr_Assign(u, t);
            t = Int_AddSigned(t, 0, v, 1, t);
        } else {
            v = WStr_Make(v, t + 3, t[0], t[2] == 0, t[0]);
            t = Int_AddSigned(t, 0, u, 0, t);
        }
    }
    if (t[1])
        operator delete(t);
    if (v[1])
        operator delete(v);
    if (k)
        return Int_Shift(u, k, u);
    return u;
}

} // extern "C"

// === vector<24 bytes>::insert_aux (FUN_489900)

extern "C" void FUN_48a720(); KING_ALIAS("_FUN_48a720", "_UninitCopy24s")

extern "C" {

// [FUN_489900] inserts a copy of point x (24 bytes) at pos of vector s: shifted up within the capacity, or
// copied into a new buffer of twice the size (1 when empty) with the old storage returned to the pool
void __fastcall Vec24_InsertAux(BYTE** s, void*, BYTE* pos, const BYTE* x)
{
    if (s[1] != s[2]) {
        BYTE* end = s[1];
        if (end)
            memcpy(end, end - 24, 24);
        s[1] = end + 24;
        BYTE t[24];
        memcpy(t, x, 24);
        for (BYTE* d = end - 24; d > pos; d -= 24)
            memcpy(d, d - 24, 24);
        memcpy(pos, t, 24);
        return;
    }
    int n = (int)(s[1] - s[0]) / 24;
    int cap = n ? n * 2 : 1;
    BYTE* buf = CallC<BYTE*>(FUN_44b740, cap * 24);
    BYTE* p = CallC<BYTE*>(FUN_48a720, s[0], pos, buf);
    if (p)
        memcpy(p, x, 24);
    BYTE* end = CallC<BYTE*>(FUN_48a720, pos, s[1], p + 24);
    if (int bytes = (int)(s[2] - s[0]) / 24 * 24)
        CallC(FUN_44b4a0, s[0], bytes);
    s[1] = end;
    s[0] = buf;
    s[2] = buf + cap * 24;
}

} // extern "C"

// === big integer division (FUN_48bd80)

ASM_VAR(AsmLabel, s_bigintDivZero, $L_670454)

extern "C" {

// [FUN_48bd80] *q = *a / *b and *r = *a % *b (r with a's sign): by a single digit, else by Knuth's algorithm on
// the operands normalised by 65536 / (b's top digit + 1)
void __cdecl BigInt_DivMod(WORD** a, WORD** b, WORD** q, WORD** r)
{
    WORD* x = *a;
    if (!x)
        g_onError((const char*)s_bigint, (int)(const char*)s_bigintNull);
    WORD* y = *b;
    if (!y)
        g_onError((const char*)s_bigint, (int)(const char*)s_bigintNull);
    WORD* qq = *q;
    WORD* rr = *r;
    int la = x[0], lb = y[0];
    if (!lb)
        g_onError((const char*)s_bigint, (int)(const char*)s_bigintDivZero);
    int cmp = Int_CompareMagnitude((const BigInt*)x, (const BigInt*)y);
    short sa = (short)x[2];
    int qsign = sa == (short)y[2];
    if (cmp < 0) {
        qq = (WORD*)Int_SetZero((BigInt*)qq);
        rr = WStr_Assign(rr, x);
    } else if (cmp == 0) {
        qq = (WORD*)Int_SetDigit((BigInt*)qq, qsign);
        rr = (WORD*)Int_SetZero((BigInt*)rr);
    } else if (lb == 1) {
        qq = WStr_Assign(qq, x);
        unsigned rem = Int_DivSmall(qq + 3, qq[0], y[3], qq + 3);
        rr = BigInt_FromInt(rr, rem);
        if (rem)
            rr[2] = sa;
    } else {
        WORD d = (WORD)(65536 / ((unsigned)y[2 + lb] + 1));
        WORD* v;
        if (d == 1 && y != qq && y != rr) {
            v = y;
            rr = BigInt_Alloc(rr, la + 1);
            for (int i = 0; i < la; i++)
                rr[3 + i] = x[3 + i];
        } else {
            v = Int_MulInt(y, d, 0);
            rr = Int_MulInt(x, d, rr);
        }
        int n = la - lb + 1;
        qq = BigInt_Alloc(qq, n);
        Int_DivDigits(rr + 3, v + 3, lb, qq + 3, n);
        if (v != y && v[1])
            operator delete(v);
        if (d != 1) {
            Int_Normalize(rr);
            Int_DivSmall(rr + 3, rr[0], d, rr + 3);
        }
    }
    qq[2] = (WORD)qsign;
    *q = Int_Normalize(qq);
    *r = Int_Normalize(rr);
}

} // extern "C"

// === the interpolating polynomial at the Chebyshev nodes (FUN_48fe80)

extern "C" void FUN_4901b0(); KING_ALIAS("_FUN_4901b0", "@Poly_Add@16")    // polynomial sum (out, b)
ASM_VAR(double, g_ch64b828, $L_64b828)      // pi

extern "C" {

// [FUN_48fe80] out = the polynomial through the points (x, sqrt(f(x))) at the n + 1 Chebyshev nodes of [a, b],
// built from the Lagrange basis
Poly* __cdecl Poly_ChebyshevFit(Poly* out, double a, double b, int n, const Poly* f)
{
    double x[24];
    if (n >= 0) {
        double d = ((double)n + g_q115_64b670) * 2, w = b - a, m = a + b;
        for (int i = 0; i <= n; i++)
            x[i] = (x87_cos(((double)i * 2 + g_q115_64b670) * g_ch64b828 / d) * w + m) * g_q117_64b6e0;
    }
    BYTE r[200];
    F(unsigned, r, 192) = 0;
    for (int k = 0; k <= n; k++) {
        double xk = x[k];
        double y = x87_sqrt(Poly_Eval(f, 0, &xk));
        BYTE l[200], t[200], tmp[200];
        F(unsigned, l, 192) = 0;
        double one = 1.0;
        CallC(FUN_4608f0, l, &one);
        F(unsigned, l, 192)++;
        for (int j = 0; j <= n; j++) {
            if (j == k)
                continue;
            y = y / (x[k] - x[j]);
            double neg = -x[j];
            double one2 = 1.0;
            F(unsigned, t, 192) = 0;
            CallC(FUN_4608f0, t, &one2);
            F(unsigned, t, 192)++;
            Poly_Append((Poly*)t, 0, &neg);
            memcpy(l, Poly_Multiply(l, 0, tmp, t), 196);
        }
        memcpy(l, Poly_Scaled((Poly*)l, 0, (Poly*)tmp, &y), 196);
        memcpy(r, CallThis<BYTE*>(FUN_4901b0, r, tmp, l), 196);
    }
    memcpy(out, r, 196);
    return out;
}

} // extern "C"

// === vector<4 bytes>::insert_aux instances (FUN_48a2c0, FUN_48a000)

extern "C" void __cdecl VecPod_InsertAux(BYTE** v, BYTE* pos, const BYTE* x, int n, void (*copy)());   // ai_world.cpp
extern "C" void FUN_46b0e0(); KING_ALIAS("_FUN_46b0e0", "_UninitCopyDwords")

extern "C" {

// [FUN_48a2c0] vector<4 bytes>::insert_aux
void __fastcall Vec4_InsertAux48a2c0(BYTE** v, void*, BYTE* pos, const BYTE* x) { VecPod_InsertAux(v, pos, x, 4, FUN_46b0e0); }

// [FUN_48a000] the same (another instance)
void __fastcall Vec4_InsertAux48a000(BYTE** v, void*, BYTE* pos, const BYTE* x) { VecPod_InsertAux(v, pos, x, 4, FUN_46b0e0); }

} // extern "C"

// === reversing a curve (FUN_48e190)

extern "C" void FUN_48eda0(); KING_ALIAS("_FUN_48eda0", "@VPoly_Evaluate@16")    // vector polynomial composed with another (out, inner)
extern "C" void FUN_48dbb0(); KING_ALIAS("_FUN_48dbb0", "@Curve_Construct@12")    // a curve record from a vector polynomial (src)
extern "C" void FUN_461680(); KING_ALIAS("_FUN_461680", "_Construct24")
extern "C" BYTE* __fastcall Rec1588_Copy(BYTE* s, void*, const BYTE* src);    // ai_containers.cpp

extern "C" {

// [FUN_48e190] curve record s (1588 bytes, its vector polynomial at +0) reversed: the polynomial composed with
// 1 - t and the record rebuilt from it
void __fastcall Curve_Reverse(BYTE* s)
{
    double one[3] = {1.0, 1.0, 1.0}, minus[3] = {-1.0, -1.0, -1.0};
    BYTE p[580];
    F(int, p, 576) = 0;
    CallC(FUN_461680, p, minus);
    F(int, p, 576)++;
    CallC(FUN_461680, p + F(int, p, 576) * 24, one);
    F(int, p, 576)++;
    BYTE t[580], r[1592];
    CallThis(FUN_48dbb0, r, CallThis<BYTE*>(FUN_48eda0, s, t, p));
    Rec1588_Copy(s, 0, r);
}

} // extern "C"

// === big integer quotient and polynomial sum (FUN_48b8a0, FUN_4901b0)

extern "C" {

// [FUN_48b8a0] out = a / b (truncated; by a single digit, else by Knuth's algorithm on the operands normalised by
// 65536 / (b's top digit + 1)); a division by zero is reported
WORD* __cdecl BigInt_Quotient(WORD* a, WORD* b, WORD* out)
{
    if (!a)
        g_onError((const char*)s_bigint, (int)(const char*)s_bigintNull);
    if (!b)
        g_onError((const char*)s_bigint, (int)(const char*)s_bigintNull);
    int lb = b[0], la = a[0];
    if (!lb)
        g_onError((const char*)s_bigint, (int)(const char*)s_bigintDivZero);
    int cmp = Int_CompareMagnitude((const BigInt*)a, (const BigInt*)b);
    int qs = a[2] == b[2];
    WORD* q;
    if (cmp < 0)
        q = (WORD*)Int_SetZero((BigInt*)out);
    else if (cmp == 0)
        q = (WORD*)Int_SetDigit((BigInt*)out, qs);
    else if (lb == 1) {
        q = WStr_Assign(out, a);
        Int_DivSmall(q + 3, q[0], b[3], q + 3);
    } else {
        WORD d = (WORD)(65536 / ((unsigned)b[2 + lb] + 1));
        WORD *v, *r;
        if (d == 1 && b != out) {
            v = b;
            r = BigInt_Alloc(0, la + 1);
            for (int i = 0; i < la; i++)
                r[3 + i] = a[3 + i];
        } else {
            v = Int_MulInt(b, d, 0);
            r = Int_MulInt(a, d, 0);
        }
        int n = la - lb + 1;
        q = BigInt_Alloc(out, n);
        Int_DivDigits(r + 3, v + 3, lb, q + 3, n);
        if (v != b && v[1])
            operator delete(v);
        if (r[1])
            operator delete(r);
    }
    q[2] = (WORD)qs;
    return Int_Normalize(q);
}

// [FUN_4901b0] polynomial s plus b into out, aligned at their last coefficients
Poly* __fastcall Poly_Add(const Poly* s, void*, Poly* out, const Poly* b)
{
    int na = s->N(), nb = b->N();
    if (!na) {
        memcpy(out, b, 196);
        return out;
    }
    if (!nb) {
        memcpy(out, s, 196);
        return out;
    }
    BYTE r[200];
    Poly* p = (Poly*)r;
    p->count = 0;
    const Poly* lo = na > nb ? s : b;
    int d = na > nb ? na - nb : nb - na, n = na > nb ? na : nb;
    for (int i = 0; i < d; i++)
        memcpy(&p->c[p->count++], &lo->c[i], 8);
    for (int i = d; i < n; i++) {
        double x = na > nb ? s->c[i] : s->c[i - d];
        double y = na > nb ? b->c[i - d] : b->c[i];
        p->c[p->count++] = x + y;
    }
    memcpy(out, r, 196);
    return out;
}

} // extern "C"

// === the B-spline basis (FUN_48fa90)

extern "C" {

// [FUN_48fa90] out = the basis vector polynomial of knot k over the knots x[i..i+n] by the Cox-de Boor recursion:
// (t - x[i]) / (x[i+n-1] - x[i]) times the basis over x[i..] plus (x[i+n] - t) / (x[i+n] - x[i+1]) times the one
// over x[i+1..], each of degree one less; for one knot (1, 1, 1) when it is k, else 0
VecPoly* __cdecl VPoly_Basis(VecPoly* out, const double* x, int i, int n, int k)
{
    if (n == 1) {
        out->count = 0;
        if (i == k) {
            DVec3 one = {1.0, 1.0, 1.0};
            VecPoly_Append(out, 0, &one);
        }
        return out;
    }
    VecPoly a, b, z1, z2, l1, l2;
    BYTE q1[580], m1[580], q2[580], m2[580];
    VPoly_Basis(&a, x, i, n - 1, k);
    VPoly_Basis(&b, x, i + 1, n - 1, k);
    BYTE* right;
    if (b.count) {
        double d = x[i + n] - x[i + 1];
        DVec3 dd = {d, d, d}, c, m;
        DVec3_Fill(&c, 0, x[i + n]);
        DVec3_Fill(&m, 0, -1.0);
        VecPoly_Line(&l1, 0, m, c);
        right = VPoly_Multiply(VPoly_Divide((BYTE*)&l1, 0, q1, &dd.x), 0, m1, (BYTE*)&b);
    } else {
        z1.count = 0;
        right = (BYTE*)&z1;
    }
    BYTE* left;
    if (a.count) {
        double d = x[i + n - 1] - x[i];
        DVec3 dd = {d, d, d}, c, p;
        DVec3_Fill(&c, 0, -x[i]);
        DVec3_Fill(&p, 0, 1.0);
        VecPoly_Line(&l2, 0, p, c);
        left = VPoly_Multiply(VPoly_Divide((BYTE*)&l2, 0, q2, &dd.x), 0, m2, (BYTE*)&a);
    } else {
        z2.count = 0;
        left = (BYTE*)&z2;
    }
    VPoly_Add(left, 0, (BYTE*)out, right);
    return out;
}

} // extern "C"

// === a B-spline as polynomial pieces (FUN_48d8f0)

extern "C" void FUN_48f5d0(); KING_ALIAS("_FUN_48f5d0", "@Vec580_CopyConstruct@12")    // vector<580-byte record> copy constructor (src)
extern "C" void __fastcall Vec580_Destroy(BYTE** v, void*);    // ai_world.cpp

extern "C" {

// [FUN_48d8f0] out = the pieces of the closed B-spline of degree deg over knots x (doubles) and the n control points
// c (DVec3): for each non-empty knot span i the sum of the basis polynomials times their points, taken over
// [x[i], x[i+1]] as 0..1
BYTE* __cdecl Spline_Pieces(BYTE* out, const double* x, const DVec3* c, int n, int deg)
{
    Vec v = {0, 0, 0};
    int m = n - deg - 1;
    for (int i = deg; i < m; i++) {
        if (x[i] == x[i + 1])
            continue;
        BYTE acc[580];
        F(int, acc, 576) = 0;
        for (int j = 0; j < deg + 1; j++) {
            int k = j - deg + i;
            DVec3 p = c[(k + m) % m];
            VecPoly b;
            VPoly_Basis(&b, x, k, deg + 1, i);
            BYTE sc[580], sum[580];
            VPoly_Scale((BYTE*)&b, 0, sc, &p.x);
            memcpy(acc, VPoly_Add(acc, 0, sum, sc), 580);
        }
        double a = x[i], h = x[i + 1] - x[i];
        DVec3 base = {a, a, a}, len = {h, h, h};
        VecPoly line;
        VecPoly_Line(&line, 0, len, base);
        BYTE comp[580];
        memcpy(acc, CallThis<BYTE*>(FUN_48eda0, acc, comp, &line), 580);
        Vec580_PushBack(&v, 0, acc);
    }
    CallThis(FUN_48f5d0, out, &v);
    Vec580_Destroy((BYTE**)&v, 0);
    return out;
}

} // extern "C"

// === destroying a road set (FUN_488450)

extern "C" void Rs_Vec1Free(); KING_ALIAS("_Rs_Vec1Free", "@Vec1_Free@8")
extern "C" void Rs_Vec8Free(); KING_ALIAS("_Rs_Vec8Free", "@Vec8_FreeStorage@8")
extern "C" void Rs_Vec24Free(); KING_ALIAS("_Rs_Vec24Free", "@Vec24_FreeStorage@8")

// frees the storage of a vector (begin +0, capacity end +8)
static void Rs_Free(BYTE** v)
{
    if (int n = (int)(v[2] - v[0]))
        CallC(FUN_44b4a0, v[0], n);
}

extern "C" {

// [FUN_488450] destructor: the records of the pointer vector at +12 (each with vectors at +16, +28, +40) and the
// byte vectors of the one at +24 deleted, then the storage of both and of the vector at +0 freed
void __fastcall RoadSet_Destroy(BYTE** s)
{
    for (BYTE** p = (BYTE**)s[3]; p != (BYTE**)s[4]; p++)
        if (BYTE* r = *p) {
            CallThis(Rs_Vec1Free, r + 40);
            CallThis(Rs_Vec8Free, r + 28);
            CallThis(Rs_Vec24Free, r + 16);
            operator delete(r);
        }
    for (BYTE** p = (BYTE**)s[6]; p != (BYTE**)s[7]; p++)
        if (BYTE* r = *p) {
            Rs_Free((BYTE**)r);
            operator delete(r);
        }
    if (int n = (int)(s[8] - s[6]) >> 2)
        CallC(FUN_44b4a0, s[6], n << 2);
    if (int n = (int)(s[5] - s[3]) >> 2)
        CallC(FUN_44b4a0, s[3], n << 2);
    Rs_Free(s);
}

} // extern "C"

// === destroying a road set index (FUN_4886e0)

extern "C" void Ri_RecDelete(); KING_ALIAS("_Ri_RecDelete", "@Rec488450_DeletingDestructor@12")

extern "C" {

// [FUN_4886e0] destructor: the 44-byte nodes of the list at +24 (their records +8 destroyed) and its head freed,
// then the storage of the vectors at +12 and +0
void __fastcall RoadIndex_Destroy(BYTE** s)
{
    BYTE* head = s[6];
    for (BYTE* n = *(BYTE**)head; n != s[6];) {
        BYTE* next = *(BYTE**)n;
        CallThis(Ri_RecDelete, n + 8, 0);
        CallC(FUN_44b4a0, n, 44);
        n = next;
    }
    F(BYTE*, s[6], 0) = s[6];
    F(BYTE*, s[6], 4) = s[6];
    CallC(FUN_44b4a0, s[6], 44);
    Rs_Free(s + 3);
    Rs_Free(s);
}

} // extern "C"

// [$L_4911e0] clears a closure record: the handle at +20 released, +32 assigned an empty value (kept when the
// same), the environment at +44 assigned an empty one
extern "C" void __fastcall Closure_Clear(BYTE* self)
{
    Value_Release((DWORD*)(self + 20));
    F(int, self, 24) = 0;
    F(int, self, 20) = 0;
    F(int, self, 28) = 0;
    StrRef empty = {0, 0, 0};
    StrRef* d = (StrRef*)(self + 32);
    if (d->ptr && !(d->flags & 0x10000)) {
        F(int, d->ptr, 4)--;
        if (F(int, d->ptr, 4) == 0 && !(d->ptr == empty.ptr && d->flags == empty.flags && d->len == empty.len) && d->ptr)
            CallVirt(d->ptr, 12, 1);
    }
    d->len = 0;
    d->ptr = 0;
    d->flags = 0;
    Value_Release((DWORD*)&empty);
    StrRef env = {0, 0, 0};
    Str_AssignEnvironment((StrRef*)(self + 44), 0, &env);
    Value_Release((DWORD*)&env);
}

// [FUN_48e700] the minimum of p on [lo, hi] from n samples, each refined by three Newton steps on p' (kept when
// inside the range): lowers *best (at *at) and returns 1 when it found a lower value
extern "C" int __cdecl Poly_FindMin(const Poly* p, double* best, double* at, double lo, double hi, int n)
{
    Poly d1, d2;
    Poly_Derivative(p, 0, &d1);
    Poly_Derivative(&d1, 0, &d2);
    int found = 0;
    if (n <= 0)
        return 0;
    double span = hi - lo, steps = n - 1;
    for (int i = 0; i < n; i++) {
        double x = i * span / steps + lo;
        double v = p->Eval(x);
        if (v < *best) {
            *best = v;
            *at = x;
            found = 1;
        }
        for (int k = 0; k < 3; k++) {
            double s = d1.Eval(x);
            x -= s / d2.Eval(x);
            v = p->Eval(x);
            if (v < *best && !(x > hi) && !(x < lo)) {
                *best = v;
                *at = x;
                found = 1;
            }
        }
    }
    return found;
}

// [FUN_48e470] out = |v - c|^2 as a polynomial: v - c squared term by term (VPoly_Multiply) and each term's
// components summed
extern "C" void __cdecl Poly_DistanceSq(Poly* out, VecPoly* v, const DVec3* c)
{
    DVec3 n = {-c->x, -c->y, -c->z};
    VecPoly k;
    k.count = 0;
    k.c[0] = n;
    k.count++;
    VecPoly d, t;
    VPoly_Add((BYTE*)v, 0, (BYTE*)&d, (BYTE*)&k);
    memcpy(&d, VPoly_Multiply((BYTE*)&d, 0, (BYTE*)&t, (BYTE*)&d), 580);
    Poly_Erase(out, 0, out->c, out->c + out->count);
    for (unsigned i = 0; i < d.count; i++) {
        double s = d.c[i].z + d.c[i].y + d.c[i].x;
        Poly_Append(out, 0, &s);
    }
}

// [FUN_48de40] the nearest approach of curve s (a vector polynomial, its bounding sphere +584 / +608) to point p on
// [lo, hi] (swapped when reversed): skipped (0) when the sphere is farther than *best, else Poly_FindMin of the
// squared distance from three samples
extern "C" int __fastcall Curve_NearestTo(VecPoly* s, void*, const DVec3* p, double* best, double* at, double lo,
    double hi)
{
    const DVec3* c = (const DVec3*)((BYTE*)s + 584);
    double dx = c->x - p->x, dy = c->y - p->y, dz = c->z - p->z;
    double dist = x87_sqrt(dx * dx + dy * dy + dz * dz);
    if (dist > F(double, s, 608) + *best)
        return 0;
    if (lo > hi)
        return Curve_NearestTo(s, 0, p, best, at, hi, lo);
    Poly q;
    Poly_DistanceSq(&q, s, p);
    return Poly_FindMin(&q, best, at, lo, hi, 3);
}

// === evaluating a polynomial of vectors (FUN_48eda0)

extern "C" {

// [FUN_48eda0] polynomial p (24-byte coefficients, count +576) at x by Horner's rule into out (580 bytes)
BYTE* __fastcall VPoly_Evaluate(const BYTE* p, void*, BYTE* out, BYTE* x)
{
    BYTE acc[580], term[580], prod[580], sum[580];
    memcpy(acc, p, 24);
    F(int, acc, 576) = 1;
    unsigned n = (unsigned)(F(int, p, 576) * 24 / 24);
    for (unsigned k = 1; k < n; k++)
    {
        memcpy(term, p + 24 * k, 24);
        F(int, term, 576) = 1;
        BYTE* m = VPoly_Multiply(acc, 0, prod, x);
        memcpy(acc, VPoly_Add(m, 0, sum, term), 580);
    }
    memcpy(out, acc, 580);
    return out;
}

} // extern "C"

// === the nearest point of a road stretch (FUN_48c710)

ASM_VAR(double, g_rn64b670, $L_64b670)

static DVec3 Rn_Lerp(const DVec3& a, const DVec3& b, double t)
{
    DVec3 d = {b.x - a.x, b.y - a.y, b.z - a.z};
    DVec3 e = {d.x * t, d.y * t, d.z * t};
    DVec3 r = {a.x + e.x, a.y + e.y, a.z + e.z};
    return r;
}

static double Rn_Len2(const DVec3& v)
{
    return v.y * v.y + v.z * v.z + v.x * v.x;
}

extern "C" {

// [FUN_48c710] road s (bounding centre +0, radius +24, points +32, 24 bytes each) checked for a point closer to p
// than *best (squared) on the stretch from segment i0 at t0 to segment i1 at t1: the distance, segment and its
// fraction written out; whether one was found
int __fastcall Road_NearestOn(BYTE* s, void*, const DVec3* p, double* best, double* tOut, int* idxOut, double t0,
                              int i0, double t1, int i1)
{
    DVec3 c = F(DVec3, s, 0);
    DVec3_SubAssign(&c, 0, p);
    if (!(F(double, s, 24) + *best >= x87_sqrt(c.y * c.y + c.z * c.z + c.x * c.x)))
        return 0;
    if (i1 < i0 || (i1 == i0 && t1 < t0))
        return Road_NearestOn(s, 0, p, best, tOut, idxOut, t1, i1, t0, i0);
    const DVec3* pts = F(const DVec3*, s, 32);
    DVec3 start = Rn_Lerp(pts[i0], pts[i0 + 1], t0);
    DVec3 v = {p->x - start.x, p->y - start.y, p->z - start.z};
    double a = Rn_Len2(v);
    int found = 0;
    for (int k = i0; k <= i1; k++)
    {
        DVec3 next = k != i1 ? pts[k + 1] : Rn_Lerp(pts[i1], pts[i1 + 1], t1);
        DVec3 w = {p->x - next.x, p->y - next.y, p->z - next.z};
        double b = w.x * w.x + w.y * w.y + w.z * w.z;
        DVec3 cur = k == i0 ? Rn_Lerp(pts[i0], pts[i0 + 1], t0) : pts[k];
        DVec3 seg = {next.x - cur.x, next.y - cur.y, next.z - cur.z};
        double L2 = Rn_Len2(seg);
        if (!(L2 + a > b))
        {
            // the start is the closest
            if (*best > a)
            {
                *best = a;
                *idxOut = k;
                found = 1;
                *tOut = 0;
            }
        }
        else if (!(L2 + b > a))
        {
            // the end is
            if (*best > b)
            {
                found = 1;
                *best = b;
                *idxOut = k;
                *tOut = 1.0;
            }
        }
        else
        {
            double dot = seg.z * v.z + seg.y * v.y + seg.x * v.x;
            double q = dot * dot / L2;
            double d2 = a - q;
            if (d2 < *best)
            {
                *best = d2;
                *idxOut = k;
                found = 1;
                *tOut = x87_sqrt(q / L2);
            }
        }
        v = w;
        a = b;
    }
    int idx = *idxOut;
    if (idx == i0)
    {
        if (idx == i1)
            *tOut = (t1 - t0) * *tOut + t0;
        else
            *tOut = (g_rn64b670 - t0) * *tOut + t0;
        return found;
    }
    if (idx == i1)
        *tOut = t1 * *tOut;
    return found;
}

} // extern "C"

// === a vector of 24-byte values copied (FUN_490820)

extern "C" void* __cdecl Stl_Allocate(unsigned bytes);

extern "C" {

// [FUN_490820] vector s made a copy of src (begin, end, capacity end): its storage from the STL allocator, the
// values copied in
BYTE** __fastcall Vec24_CopyConstruct(BYTE** s, void*, BYTE* const* src)
{
    int n = (int)(src[1] - src[0]) / 24;
    s[0] = s[1] = s[2] = 0;
    BYTE* p = n ? (BYTE*)Stl_Allocate(n * 24) : 0;
    s[0] = p;
    s[1] = p;
    s[2] = p + n * 24;
    s[1] = UninitCopy24s(src[0], src[1], p);
    return s;
}

} // extern "C"

// === a vector of 580-byte values copied (FUN_48f5d0)

extern "C" void Vc_Copy580(); KING_ALIAS("_Vc_Copy580", "_UninitCopy580s")

extern "C" {

// [FUN_48f5d0] vector s made a copy of src, as Vec24_CopyConstruct with 580-byte values
BYTE** __fastcall Vec580_CopyConstruct(BYTE** s, void*, BYTE* const* src)
{
    int n = (int)(src[1] - src[0]) / 580;
    s[0] = s[1] = s[2] = 0;
    BYTE* p = n ? (BYTE*)Stl_Allocate(n * 580) : 0;
    s[0] = p;
    s[1] = p;
    s[2] = p + n * 580;
    s[1] = CallC<BYTE*>(Vc_Copy580, src[0], src[1], p, n);
    return s;
}

} // extern "C"

// === the squared derivative's coefficient sums of a polynomial (FUN_48e600)

extern "C" BYTE* __fastcall VPoly_Derivative(BYTE* s, void*, BYTE* out);
extern "C" BYTE* __fastcall VPoly_Multiply(BYTE* s, void*, BYTE* out, BYTE* b);

extern "C" {

// [FUN_48e600] list s (doubles, its count +192) made the sums of the three coefficients of each term (24 bytes,
// their count +576) of p's derivative squared
void __cdecl VPoly_SquaredDerivative(BYTE* s, BYTE* p)
{
    __declspec(align(8)) BYTE d[580], t[580];
    VPoly_Derivative(p, 0, d);
    memcpy(d, VPoly_Multiply(d, 0, t, d), 580);
    F(int, s, 192) = 0;
    int n = F(int, d, 576);
    for (int i = 0; i < n; i++)
    {
        const double* c = (const double*)(d + 24 * i);
        F(double, s, 8 * F(int, s, 192)) = c[2] + c[0] + c[1];
        F(int, s, 192)++;
    }
}

} // extern "C"

// === a vector of dwords copied (FUN_48a580) and a task's destructor (FUN_491590)

extern "C" void Vc_CopyDw(); KING_ALIAS("_Vc_CopyDw", "_UninitCopyDwords")
extern "C" void Tk_Queue(); KING_ALIAS("_Tk_Queue", "_Tasks_Queue")
extern "C" void __cdecl Ai_PoolFree(void* p, unsigned size);
ASM_VAR(AsmLabel, g_tk64bf24, $L_64bf24)
ASM_VAR(AsmLabel, g_tk64bf38, $L_64bf38)
ASM_VAR(AsmLabel, g_tk64bee4, $L_64bee4)

extern "C" {

// [FUN_48a580] vector s made a copy of src, as Vec24_CopyConstruct with dwords
BYTE** __fastcall VecDw_CopyConstruct(BYTE** s, void*, BYTE* const* src)
{
    int n = (int)(src[1] - src[0]) >> 2;
    s[0] = s[1] = s[2] = 0;
    BYTE* p = n ? (BYTE*)Stl_Allocate(n * 4) : 0;
    s[0] = p;
    s[1] = p;
    s[2] = p + n * 4;
    s[1] = CallC<BYTE*>(Vc_CopyDw, src[0], src[1], p, n);
    return s;
}

// [FUN_491590] task s destroyed: its 44-byte entries (+20, vtable +4) and their storage, then handed to the task
// queue (+12, +16)
void __fastcall Task491590_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = &g_tk64bf24;
    BYTE* b = F(BYTE*, s, 20);
    BYTE* e = F(BYTE*, s, 24);
    for (BYTE* p = b; p != e; p += 44)
        CallVirt(p, 4, 0);
    b = F(BYTE*, s, 20);
    if (int n = (int)(F(BYTE*, s, 28) - b) / 44)
        Ai_PoolFree(b, n * 44);
    F(void*, s, 0) = &g_tk64bf38;
    CallC(Tk_Queue, F(int, s, 12), F(int, s, 16));
    F(void*, s, 0) = &g_tk64bee4;
}

} // extern "C"

// === a road curve made from a polynomial (FUN_48dbb0)

extern "C" void Cc_Fit(); KING_ALIAS("_Cc_Fit", "_Poly_ChebyshevFit")
extern "C" void Cc_Integral(); KING_ALIAS("_Cc_Integral", "@Poly_Integral@12")
extern "C" void Cc_Eval(); KING_ALIAS("_Cc_Eval", "@VecPoly_Eval@16")
extern "C" void Cc_Push(); KING_ALIAS("_Cc_Push", "@Vec24_PushBackK@12")
extern "C" void Cc_Sphere(); KING_ALIAS("_Cc_Sphere", "_Points_BoundingSphere")
extern "C" void Cc_Assign(); KING_ALIAS("_Cc_Assign", "@DVec3_Assign@12")
extern "C" void Cc_Free(); KING_ALIAS("_Cc_Free", "@Vec24_FreeStorage@8")
extern "C" void Cc_VecNew(); KING_ALIAS("_Cc_VecNew", "@Vec_Construct@12")
ASM_VAR(double, g_cc64b5a0, $L_64b5a0)

extern "C" {

// [FUN_48dbb0] curve s (1592 bytes) of polynomial p: p (+0), its derivative (+616), the squared derivative's
// sums (+1196), their Chebyshev fit over 0 .. 1 integrated (the arc length, +1392), and the bounding sphere of
// 100 points along it (+584, its radius +608)
BYTE* __fastcall Curve_Construct(BYTE* s, void*, const BYTE* p)
{
    memcpy(s, p, 580);
    F(int, s, 608) = 0;
    F(int, s, 612) = 0;
    F(int, s, 616 + 576) = 0;
    F(int, s, 1388) = 0;
    F(int, s, 1584) = 0;
    __declspec(align(8)) BYTE d[580], fit[196], in[196];
    memcpy(s + 616, VPoly_Derivative((BYTE*)p, 0, d), 580);
    VPoly_SquaredDerivative(s + 1196, s);
    BYTE* f = CallC<BYTE*>(Cc_Fit, fit, 0.0, 1.0, 8, s + 1196);
    memcpy(s + 1392, CallThis<BYTE*>(Cc_Integral, f, in), 196);
    BYTE* v[3];
    char alloc;
    CallThis(Cc_VecNew, v, &alloc);
    for (int i = 0; i < 100; i++)
    {
        double t = i * g_cc64b5a0;
        double pt[3];
        CallThis(Cc_Push, v, CallThis<double*>(Cc_Eval, s, pt, &t));
    }
    double sp[4];
    const double* r = CallC<const double*>(Cc_Sphere, sp, v);
    CallThis(Cc_Assign, s + 584, r);
    F(int, s, 608) = ((const int*)r)[6];
    F(int, s, 612) = ((const int*)r)[7];
    CallThis(Cc_Free, v);
    return s;
}

} // extern "C"

// ---------------------------------------------------------------------------
// Batch 32: polylines and list nodes

extern "C" void Rt32_Allocate(); KING_ALIAS("_Rt32_Allocate", "_Stl_Allocate")
extern "C" void Rt32_Copy24s(); KING_ALIAS("_Rt32_Copy24s", "_UninitCopy24s")
extern "C" void Rt32_Sphere(); KING_ALIAS("_Rt32_Sphere", "_Points_BoundingSphere")
extern "C" void Rt32_Lock(); KING_ALIAS("_Rt32_Lock", "@Stl_Lock@8")
extern "C" void Rt32_ChunkAlloc(); KING_ALIAS("_Rt32_ChunkAlloc", "_Stl_ChunkAlloc")
extern "C" void Rt32_CharCopy(); KING_ALIAS("_Rt32_CharCopy", "@VecChar_Copy@12")
extern "C" void Rt32_Reserve4(); KING_ALIAS("_Rt32_Reserve4", "@Vec4_Reserve@16")
extern "C" void Rt32_CopyDwords(); KING_ALIAS("_Rt32_CopyDwords", "_UninitCopyDwords")
extern "C" void Rt32_DwCopy(); KING_ALIAS("_Rt32_DwCopy", "@VecDw_CopyConstruct@12")
ASM_VAR(int, rt32_lock68d204, $L_68d204)
ASM_VAR(BYTE*, rt32_free48_68d1d8, $L_68d1d8)

extern "C" {

// [FUN_48c620] a polyline from the points pts (a vector of 24-byte points): +32 a copy of them, +0 their
// bounding sphere (centre and radius, 32 bytes)
BYTE* __fastcall Polyline_Construct(BYTE* self, void*, BYTE** pts)
{
    F(int, self, 24) = 0;
    F(int, self, 28) = 0;
    int n = (int)(pts[1] - pts[0]) / 24;
    BYTE** v = (BYTE**)(self + 32);
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
    BYTE* p = n ? CallC<BYTE*>(Rt32_Allocate, n * 24) : 0;
    v[0] = p;
    v[1] = p;
    v[2] = p + n * 24;
    v[1] = CallC<BYTE*>(Rt32_Copy24s, pts[0], pts[1], p, pts);
    BYTE tmp[32];
    const BYTE* s = CallC<BYTE*>(Rt32_Sphere, tmp, self + 32);
    memcpy(self, s, 32);
    return self;
}

// [FUN_4897b0] a new list node (48 bytes from the allocator's free list, refilled 20 at a time) holding a copy
// of value src: its char vector, dword vector (+12) and second dword vector (+24)
BYTE* __fastcall List_NewNode(void* list, void*, BYTE* src)
{
    CallThis(Rt32_Lock, &rt32_lock68d204);
    BYTE* node = rt32_free48_68d1d8;
    if (!node) {
        int n = 20;
        node = CallC<BYTE*>(Rt32_ChunkAlloc, 48, &n);
        if (n != 1) {
            BYTE* p = node + 48;
            rt32_free48_68d1d8 = p;
            for (int i = 1; i != n - 1; i++, p += 48)
                *(BYTE**)p = p + 48;
            *(BYTE**)p = 0;
        }
    } else {
        rt32_free48_68d1d8 = *(BYTE**)node;
    }
    rt32_lock68d204 = 0;
    BYTE* v = node + 8;
    if (v) {
        CallThis(Rt32_CharCopy, v, src);
        BYTE alloc;
        CallThis(Rt32_Reserve4, v + 12, (int)(F(BYTE*, src, 16) - F(BYTE*, src, 12)) >> 2, &alloc);
        F(BYTE*, v, 16) = CallC<BYTE*>(Rt32_CopyDwords, F(BYTE*, src, 12), F(BYTE*, src, 16), F(BYTE*, v, 12), src);
        CallThis(Rt32_DwCopy, v + 24, src + 24);
    }
    return node;
}

}

// ---------------------------------------------------------------------------
// Batch 33: polyline nearness, the way file parser

extern "C" void Rt33_Ctor4883f0(); KING_ALIAS("_Rt33_Ctor4883f0", "@Rec4883f0_Construct@12")
extern "C" void Rt33_End(); KING_ALIAS("_Rt33_End", "@SchList_End@12")
extern "C" void Rt33_Insert(); KING_ALIAS("_Rt33_Insert", "@List489_Insert@20")
extern "C" void Rt33_SetDestroy(); KING_ALIAS("_Rt33_SetDestroy", "@RoadSet_Destroy@4")
extern "C" void Rt33_SetText(); KING_ALIAS("_Rt33_SetText", "@Rec_SetText12@12")
ASM_VAR(char, rt33_s66c934, $L_66c934)
ASM_VAR(char, rt33_s67035c, $L_67035c)

extern "C" {

// [FUN_48dd50] whether the polyline (self, its bounding sphere at +584) comes within *range of point p: outside
// the sphere plus range 0, else the nearest parameter of the squared distance polynomial on 0..1 (into at)
int __fastcall Polyline_Near(BYTE* self, void*, const DVec3* p, double* range, double* at)
{
    DVec3 c = *(const DVec3*)(self + 584);
    double dx = c.x - p->x;
    double dy = c.y - p->y;
    double dz = c.z - p->z;
    double d = x87_sqrt((dy * dy + dz * dz) + dx * dx);
    if (d > F(double, self, 608) + *range)
        return 0;
    Poly sq;
    Poly_DistanceSq(&sq, (VecPoly*)self, p);
    return Poly_FindMin(&sq, range, at, 0.0, 1.0, 3);
}

// [FUN_488620] way file parser constructor: empty fields, a list (+24) holding one empty record (66c934) and
// its first node at +28, the text 67035c
BYTE* __fastcall WayParser_Construct(BYTE* self, void*)
{
    for (int i = 0; i < 6; i++)
        F(int, self, i * 4) = 0;
    BYTE** list = (BYTE**)(self + 24);
    BYTE* head = CallC<BYTE*>(FUN_44b740, 44);
    list[0] = head;
    F(BYTE*, head, 0) = head;
    F(BYTE*, list[0], 4) = list[0];
    BYTE tmp[36];
    BYTE* t = CallThis<BYTE*>(Rt33_Ctor4883f0, tmp, (const char*)&rt33_s66c934);
    void* end;
    CallThis(Rt33_End, list, &end);
    void* out;
    CallThis(Rt33_Insert, list, &out, end, t);
    CallThis(Rt33_SetDestroy, tmp);
    CallThis(Rt33_SetText, self, (const char*)&rt33_s67035c);
    F(BYTE*, self, 28) = list[0];
    F(BYTE*, self, 28) = F(BYTE*, list[0], 4);
    return self;
}

}
