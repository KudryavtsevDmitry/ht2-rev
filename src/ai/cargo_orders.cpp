// cargo_orders.cpp - ai_ordr.cpp of the AI, rewritten from king.masm: the
// STL instances of the orders (12-byte entries: copies, fills, binary
// searches by the key at +8, reversal), the order record's constructor and
// its values.
//
// Building with  nmake ASM_CARGO_ORDERS=1  makes king.masm assemble the original
// procedures again.

#include <stdlib.h>
#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "FUN_401000")
ASM_PROC(FUN_44c220)    // vector<char>::insert(pos, first, last)
extern "C" void FUN_470900(); KING_ALIAS("_FUN_470900", "@CharVec_Construct@12")
extern "C" void FUN_44ac60(); KING_ALIAS("_FUN_44ac60", "_Vec4b_AllocCopy@12")    // vector<pointer>: a new buffer (n, first, last)
ASM_PROC(FUN_44b4a0)    // the AI's allocator: free (p, size)

ASM_VAR(BYTE*, g_68ccb8, $L_68ccb8)             // 104-byte records
ASM_VAR(double, g_bonusFactor, $L_66f878)       // 1.5
ASM_VAR(double, g_68d6c8, $L_68d6c8)

struct E12 { DWORD a, b, key; };

extern "C" char* __cdecl CopyBytes(const char* first, const char* last, char* to)
{
    for (int n = (int)(last - first); n > 0; n--)
        *to++ = *first++;
    return to;
}

extern "C" void __cdecl E12_Distance(const E12* first, const E12* last, int* n)
{
    *n += (int)(last - first);
}

extern "C" E12* __cdecl E12_CopyBackward(const E12* first, const E12* last, E12* to)
{
    for (int n = (int)(last - first); n > 0; n--)
        *--to = *--last;
    return to;
}

extern "C" void __cdecl E12_Advance(E12** it, int n)
{
    *it += n;
}

extern "C" void __cdecl E12_FillN(E12* to, int n, const E12* v)
{
    for (; n > 0; n--, to++)
        if (to)
            *to = *v;
}

extern "C" E12* __cdecl E12_LowerBound(E12* first, E12* last, const E12* v)
{
    DWORD k = v->key;
    for (int n = (int)(last - first); n > 0;) {
        int half = n >> 1;
        if (first[half].key < k) {
            first += half + 1;
            n += -1 - half;
        } else
            n = half;
    }
    return first;
}

extern "C" E12* __cdecl E12_UpperBound(E12* first, E12* last, const E12* v)
{
    DWORD k = v->key;
    for (int n = (int)(last - first); n > 0;) {
        int half = n >> 1;
        if (k >= first[half].key) {
            first += half + 1;
            n += -1 - half;
        } else
            n = half;
    }
    return first;
}

extern "C" void __cdecl E12_Reverse(E12* first, E12* last)
{
    while (first != last && first != --last) {
        E12 t = *first;
        *first++ = *last;
        *last = t;
    }
}

extern "C" void __cdecl E12_Swap(E12* a, E12* b)
{
    E12 t = *a;
    *a = *b;
    *b = t;
}

// vector<char> = s (with its terminating zero)
extern "C" void __fastcall CharVec_Assign(char** self, void*, const char* s)
{
    self[1] = self[0];
    if (s)
        CallThis(FUN_44c220, self, self[1], s, s + strlen(s) + 1);
}

extern "C" BYTE* __fastcall Order_Construct(BYTE* self, void*)
{
    for (int i = 0; i <= 16; i += 4)
        F(int, self, i) = -1;
    F(int, self, 24) = -1;
    F(int, self, 28) = -1;
    for (int i = 32; i <= 48; i += 4)
        F(int, self, i) = 0;
    F(int, self, 52) = -1;
    F(int, self, 56) = 0;
    F(int, self, 64) = 0;
    F(int, self, 60) = 0;
    F(int, self, 68) = 0;
    F(double, self, 72) = AI_Now() + 900.0;
    F(double, self, 80) = AI_Now();
    F(double, self, 88) = 900.0;
    F(double, self, 96) = 1.0;
    F(int, self, 104) = 0;
    F(int, self, 112) = 0;
    for (int i = 116; i <= 132; i += 4)
        F(int, self, i) = 0;
    F(int, self, 136) = -1;
    return self;
}

// the order's value: sqrt(record value * amount) * 10, less the part done
extern "C" double __cdecl Order_Value(const int* o)
{
    double v = x87_sqrt(F(double, g_68ccb8, o[0] * 104 + 40) * F(double, o, 56));
    return v * (1.0 - F(double, o, 96)) * 10.0;
}

extern "C" double __cdecl Order_FullValue(const int* o)
{
    return x87_sqrt(F(double, g_68ccb8, o[0] * 104 + 40) * F(double, o, 56)) * 10.0;
}

extern "C" double __cdecl Order_Pay(const BYTE* o, double late, int bonus)
{
    double v = F(double, o, 64);
    if ((char)bonus && x87_eq(late, 0.0) && v > 0.0 && F(int, o, 104) == 1)
        v *= g_bonusFactor;
    return v;
}

// ---------------------------------------------------------------------------
// medium-sized functions: an order's price and margin, the order constructor

struct OrdrPos { int road, node; double t; int segment, object, m_24, m_28; };   // PositionId

extern "C" void __cdecl AiError(int kind, int code, const char* file, const char* id, int line,
                                const char* msg, const char* a, const char* b);
KING_ALIAS("_AiError", "FUN_401010")
extern "C" int* __cdecl Town_Id(int* out, BYTE* town);                                 // aimain.cpp
extern "C" void __cdecl Road_PositionCostBest(int side, OrdrPos from, OrdrPos to, double* cost);   // aitrce.cpp
extern "C" double __cdecl Order_Value(const int* o);
typedef void* PtrList[1];
ASM_VAR(PtrList, g_ordrTowns, $L_68cec0)
ASM_VAR(PtrList, g_ordrVehicles, $L_68cfd8)
ASM_VAR(void*, g_ordrPlayers, $L_68b990)
ASM_VAR(char, g_ordrEmpty, $L_68d1c0)
ASM_VAR(double, g_priceDirect, $L_66f880)       // 1.2
ASM_VAR(double, g_priceUrgent, $L_66f888)       // 1.5
ASM_VAR(double, g_priceSpecial, $L_66f890)      // 4.0

static const char s_ordrId[] = "$Id: ai_ordr.cpp 2001/9/10 18:14:44";
static const char s_ordrFile[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\Ai_ordr.cpp";

static BYTE* Town(int i) { return (BYTE*)((void**)g_ordrTowns[0])[i]; }

// the unload node of the order's town (reported when missing)
static int UnloadNode(int* o, int line)
{
    int node;
    Town_Id(&node, Town(o[3]));
    if (node == -1)
        AiError(45, 100, s_ordrFile, s_ordrId, line, "Unload node not present for town",
                F(const char*, Town(o[3]), 12), &g_ordrEmpty);
    return node;
}

// the cost of the road from the order's position (+24) to that node
static double UnloadCost(int* o, int node)
{
    OrdrPos to = {-1, node, 0.0, 0, 0, 0, -1};
    double cost = 0;
    Road_PositionCostBest(0, F(OrdrPos, o, 24), to, &cost);
    return cost;
}

// the price of an order (+64) for a player
extern "C" void __cdecl Order_SetPrice(BYTE* pl, int* o, char direct, char urgent)
{
    BYTE* rec = g_68ccb8 + o[0] * 104;
    double base = (1.0 - F(double, rec, 32) + 0.2) * x87_sqrt(F(double, rec, 40) * F(double, o, 56));
    if (x87_eq(F(double, o, 112), 0.0)) {
        int node = UnloadNode(o, 135);
        if (o[4] != -1) {
            BYTE* d = F(BYTE*, Town(o[4]), 100);
            F(DWORD, o, 112) = F(DWORD, d, o[3] * 8);
            F(DWORD, o, 116) = F(DWORD, d, o[3] * 8 + 4);
        } else
            F(double, o, 112) = UnloadCost(o, node);
    }
    double k = 115.0;
    if (pl != g_ordrPlayers && (DWORD)pl + 8 != 0) {
        BYTE* c = F(BYTE*, pl + 8, 508);
        if (!c)
            k = 40.0;
        else {
            int type = F(int, ((void**)g_ordrVehicles[0])[F(int, c, 4)], 32);
            if (type != 2 && type != 3)
                k = 40.0;
        }
    }
    double r = (F(double, o, 112) * k * 0.001 + base) * 3.0;
    if (x87_eq(F(double, o, 56), 0.0))
        r = 0.0;
    if (direct && o[26] != 1)
        r = r * g_priceDirect;
    if (urgent)
        r = r * g_priceUrgent;
    if (o[26] == 3 || (direct && !urgent))
        r = r * g_priceSpecial;
    F(double, o, 64) = r;
}

// what an order brings above its value; checks both
extern "C" double __cdecl Order_Margin(int* o, double done, char bonus, double* price, double* value)
{
    double v = Order_Value(o);
    if (v > 100000000.0 || !(v >= 0.0))
        AiError(0, 200, s_ordrFile, s_ordrId, 187, "Internal error 09458", &g_ordrEmpty, &g_ordrEmpty);
    if (value)
        *value = v;
    double x = F(double, o, 64);
    if (bonus && x87_eq(done, 0.0) && x > 0.0 && o[26] == 1)
        x = x * g_bonusFactor;
    if (price)
        *price = x;
    double d = x - v;
    if (!(d >= -1e-08))
        AiError(0, 200, s_ordrFile, s_ordrId, 201, "Internal error 09459", &g_ordrEmpty, &g_ordrEmpty);
    return d;
}

// an order from src for the position pos and the quantity q; returns src,
// which the original leaves in EAX
extern "C" const int* __cdecl Order_Init4715f0(int* o, const int* src, int arg, OrdrPos pos, double q)
{
    o[0] = src[0];
    o[1] = src[1];
    o[2] = src[2];
    o[3] = src[3];
    o[26] = 1;
    F(double, o, 72) = AI_Now() + F(double, src, 24);
    o[4] = arg;
    F(OrdrPos, o, 24) = pos;
    F(double, o, 80) = AI_Now();
    F(DWORD, o, 56) = F(DWORD, &q, 0);
    F(DWORD, o, 60) = F(DWORD, &q, 4);
    o[24] = 0;
    o[25] = 0x3ff00000;
    if (!(q >= 0.0))
        AiError(0, 512, s_ordrFile, s_ordrId, 244, "Q<0", &g_ordrEmpty, &g_ordrEmpty);
    F(double, o, 112) = UnloadCost(o, UnloadNode(o, 248));
    o[22] = src[6];
    o[23] = src[7];
    return src;
}

extern "C" {

// [FUN_470900] constructor of a char vector from a string
char** __fastcall CharVec_Construct(char** self, void*, const char* s)
{
    self[0] = self[1] = self[2] = 0;
    CharVec_Assign(self, 0, s);
    return self;
}

}

extern "C" void FUN_470760(); KING_ALIAS("_FUN_470760", "_E12_FillN")

extern "C" {

// [FUN_46ed00] temporary buffer of 12-byte records for [first, last):
// {requested, length, buffer}, halving the length while malloc fails
int* __fastcall TempBuf12_Construct(int* self, void*, BYTE* first, BYTE* last)
{
    int n = (last - first) / 12;
    self[1] = n;
    self[0] = n;
    self[2] = 0;
    if (n > 0x0aaaaaaa)
        self[1] = 0x0aaaaaaa;
    while (self[1] > 0) {
        self[2] = (int)malloc(self[1] * 12);
        if (self[2])
            break;
        self[1] /= 2;
    }
    if (self[1] > 0)
        CallC(FUN_470760, self[2], self[1], first, last);
    return self;
}

}
