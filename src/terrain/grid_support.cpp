// grid_support.cpp - small functions of grd.cpp of the terrain library (see
// also grid_base.cpp and grid_topology.cpp): ring lists, the binary tree
// nodes of btr.h, the 3x3 tile height tables (257x257 samples of 16 bytes),
// degree tables of sine and cosine, and object constructors.
//
// Building with  nmake ASM_GRID_SUPPORT=1  makes king.masm assemble the
// original procedures again.

#include "king.h"
#include "x87.h"
#include <stdarg.h>

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef float Floats[1];

extern "C" void FUN_606750(); KING_ALIAS("_FUN_606750", "@GridEdge_Destruct@8")
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")    // error report (code, format, file, line, date)
extern "C" void FUN_60ab70(); KING_ALIAS("_FUN_60ab70", "_Grd_DxErrorName")
extern "C" void FUN_60aa60(); KING_ALIAS("_FUN_60aa60", "_Grd_Message")
extern "C" void FUN_60f690(); KING_ALIAS("_FUN_60f690", "@MarineQueue_RemoveNode@12")
extern "C" void FUN_6136f0(); KING_ALIAS("_FUN_6136f0", "@MarineQueue_RemoveLink@12")
extern "C" void FUN_608c70(); KING_ALIAS("_FUN_608c70", "@TGrid_MakeNode@16")
extern "C" void FUN_613500(); KING_ALIAS("_FUN_613500", "@MarineQueue_Init@8")
extern "C" void FUN_6064c0(); KING_ALIAS("_FUN_6064c0", "@GridMesh_CreateEdge@16")
ASM_VAR(AsmLabel, s_684a84, $L_684a84)
ASM_VAR(AsmLabel, s_66d9c4, $L_66d9c4)
ASM_VAR(AsmLabel, s_6849fc, $L_6849fc)
extern "C" void FUN_50d190(); KING_ALIAS("_FUN_50d190", "@Shell_Nop@8")
extern "C" void FUN_613790(); KING_ALIAS("_FUN_613790", "@MarineQueue_TopWorkItem@8")
extern "C" void FUN_609de0(); KING_ALIAS("_FUN_609de0", "_GridTree_RemoveEdge")

ASM_VAR(AsmLabel, g_vtbl652ef4, $L_652ef4)
ASM_VAR(AsmLabel, g_vtbl652f00, $L_652f00)
ASM_VAR(AsmLabel, s_buildDate, $L_6724f8)
ASM_VAR(AsmLabel, s_btrFile, $L_684a64)
ASM_VAR(AsmLabel, s_fileLine, $L_6724b0)
ASM_VAR(Floats, g_sinDeg, $L_724964)      // 360 entries
ASM_VAR(Floats, g_cosDeg, $L_7243bc)      // 360 entries
ASM_VAR(int, g_725d28, $L_725d28)

extern "C" {

// [FUN_605810]
int __fastcall Stack_Top(BYTE* self, void*)
{
    return F(int, self, 16 + F(int, self, 12) * 4);
}

// [FUN_606730]
BYTE* __fastcall Grid606750_Delete(BYTE* self, void*, int flags)
{
    CallThis(FUN_606750, self);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [FUN_606a50] the node after node in a two-ended list
int __fastcall Pair_Next(BYTE** self, void*, int* node)
{
    if (self[0] && (BYTE*)node == F(BYTE*, self[0], 4))
        return (int)self[1];
    if (self[1] && (BYTE*)node == F(BYTE*, self[1], 4))
        return 0;
    return *node;
}

// [FUN_607f60]
DWORD* __fastcall Quad_Zero(DWORD* self, void*)
{
    self[2] = 0;
    self[1] = 0;
    self[0] = 0;
    self[3] = 0;
    return self;
}

// [FUN_607f70]
int __fastcall Stack_Last(int* self, void*)
{
    int n = self[0];
    if (!n)
        return 0;
    return self[n + 1];
}

// [FUN_607f80]
BYTE* __fastcall Grid_ConstructLists(BYTE* self, void*)
{
    F(BYTE*, self, 4) = self;
    F(BYTE*, self, 0) = self;
    static const int z[] = {56, 52, 48, 20, 24, 28, 32, 44, 36, 40, 100, 96, 92, 64, 68, 72, 76, 88, 80, 84,
                            148, 144, 140, 112, 116, 120, 124, 136, 128, 132};
    for (int k = 0; k < sizeof(z) / sizeof(z[0]); k++)
        F(int, self, z[k]) = 0;
    F(void*, self, 16) = g_vtbl652ef4;
    F(void*, self, 60) = g_vtbl652ef4;
    F(void*, self, 108) = g_vtbl652ef4;
    return self;
}

// [FUN_608a50]
BYTE* __fastcall GridObj_Construct(BYTE* self, void*)
{
    static const int z[] = {40, 36, 32, 4, 8, 12, 16, 28, 20, 24};
    for (int k = 0; k < sizeof(z) / sizeof(z[0]); k++)
        F(int, self, z[k]) = 0;
    F(void*, self, 0) = g_vtbl652ef4;
    return self;
}

// [FUN_608aa0]
void __fastcall GridObj_SetVtables(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl652f00;
    F(void*, self, 12) = g_vtbl652ef4;
}

// [FUN_608ab0] the table of the tile holding (x, y)
BYTE* __fastcall Tiles_Table(BYTE** self, void*, DWORD x, DWORD y)
{
    DWORD tx = x / 257, ty = y / 257;
    if (tx == 3 || ty == 3)
        return self[1];
    if (2 - tx != ty)
        return self[2];
    if (tx == 0)
        return ty == 2 && 771 - y <= 257 - x ? self[1] : self[0];
    if (tx == 2 && ty == 0)
        return 771 - x <= 257 - y ? self[1] : self[0];
    return self[0];
}

// [FUN_608e70]
float __fastcall Tile_Height(BYTE* self, void*, DWORD x, DWORD y)
{
    return *(float*)(self + ((x % 257) * 257 + y % 257) * 16);
}

// [FUN_608ea0]
void __fastcall Tile_Normal(BYTE* self, void*, DWORD x, DWORD y, DWORD* out)
{
    const DWORD* s = (DWORD*)(self + ((x % 257) * 257 + y % 257) * 16 + 4);
    out[0] = s[0];
    out[1] = s[1];
    out[2] = s[2];
}

// [FUN_609980]
void __fastcall Grid_ResetVtables(BYTE* self, void*)
{
    F(void*, self, 108) = g_vtbl652ef4;
    F(void*, self, 60) = g_vtbl652ef4;
    F(void*, self, 16) = g_vtbl652ef4;
}

// [FUN_609960]
BYTE* __fastcall Grid_Delete(BYTE* self, void*, int flags)
{
    Grid_ResetVtables(self, 0);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [FUN_609ce0] inserts node before list in a ring
void __cdecl Ring_InsertBefore(BYTE** node, BYTE** list)
{
    node[0] = (BYTE*)list;
    node[1] = (BYTE*)list[1];
    *(BYTE**)list[1] = (BYTE*)node;
    list[1] = (BYTE*)node;
}

static void Btr_Detach(BYTE* self)
{
    BYTE* p = F(BYTE*, self, 8);
    if (!p)
        return;
    if (F(BYTE*, p, 0) == self)
        F(BYTE*, p, 0) = 0;
    else if (F(BYTE*, p, 4) == self)
        F(BYTE*, p, 4) = 0;
    else
        CallC(FUN_60aad0, 0x88760112, (const char*)s_fileLine, (const char*)s_btrFile, 35,
              (const char*)s_buildDate);
}

// [FUN_60a030] destroys a tree node and its subtrees
void __fastcall Btr_Destroy(BYTE* self, void*)
{
    Btr_Detach(self);
    BYTE* c = F(BYTE*, self, 0);
    if (c) {
        Btr_Destroy(c, 0);
        operator delete(c);
    }
    c = F(BYTE*, self, 4);
    if (c) {
        Btr_Destroy(c, 0);
        operator delete(c);
    }
}

// [FUN_60a440]
void __cdecl Ring_Unlink(BYTE** node)
{
    F(BYTE*, node[0], 4) = node[1];
    *(BYTE**)node[1] = node[0];
}

// [FUN_60a850]
int __cdecl Grid_IsFine(BYTE* o)
{
    return F(DWORD, o, 36) >= 10;
}

// [FUN_60ba20]
void __cdecl Tables_InitSin()
{
    for (int i = 0; i < 360; i++)
        g_sinDeg[i] = (float)x87_sin((double)i * 3.1415998935699463f * 0.0055555556900799274f);
}

// [FUN_60bab0]
void __cdecl Tables_InitCos()
{
    for (int i = 0; i < 360; i++)
        g_cosDeg[i] = (float)x87_cos((double)i * 3.1415998935699463f * 0.0055555556900799274f);
}

// [FUN_60bda0]
BYTE* __fastcall Btr_Delete(BYTE* self, void*, int flags)
{
    Btr_Detach(self);
    if (F(BYTE*, self, 0))
        Btr_Delete(F(BYTE*, self, 0), 0, 1);
    if (F(BYTE*, self, 4))
        Btr_Delete(F(BYTE*, self, 4), 0, 1);
    if (flags & 1)
        operator delete(self);
    return self;
}

// [FUN_60c280]
void __cdecl Grid_Reset725d28()
{
    g_725d28 = 0;
}

// [FUN_60c5b0]
void __fastcall GridTask_Free(BYTE* self, void*)
{
    if (F(void*, self, 12)) {
        operator delete(F(void*, self, 12));
        F(void*, self, 12) = 0;
    }
    BYTE* o = F(BYTE*, self, 16);
    if (o) {
        CallThis(FUN_50d190, o);
        operator delete(o);
        F(void*, self, 16) = 0;
    }
}

// [FUN_60cae0]
void __fastcall Grid_DrainQueue(BYTE* self, void*)
{
    BYTE* n;
    while ((n = CallThis<BYTE*>(FUN_613790, F(BYTE*, self, 32))) != 0)
        CallC(FUN_609de0, n, self);
}

}

// --- grid support: error reports, rings of nodes, the grid's tables

// the node after node in the ring r (+0 head, +4 tail; a node's link is at +4)
static BYTE* Ring_After(BYTE** r, BYTE* node)
{
    BYTE* link = node ? node + 4 : 0;
    BYTE* h = r[0];
    if (h && link == F(BYTE*, h, 8))
        return r[1];
    BYTE* t = r[1];
    if (t && link == F(BYTE*, t, 8))
        return 0;
    BYTE* n = F(BYTE*, node, 4);
    return n ? n - 4 : 0;
}

extern "C" {

// [FUN_60aad0] reports a DirectX error: the text of hr, then the message
void __cdecl Grd_ReportError(int hr, const char* fmt, ...)
{
    char buf[256];
    char err[256];
    buf[0] = 0;
    err[0] = 0;
    if (!hr)
        return;
    err[0] = 0;
    CallC(FUN_60ab70, hr, err, 255);
    wsprintfA(buf, (const char*)s_684a84, err);
    va_list ap;
    va_start(ap, fmt);
    wvsprintfA(buf + lstrlenA(buf), fmt, ap);
    va_end(ap);
    CallC(FUN_60aa60, 2, (const char*)s_66d9c4, buf);
}

// [FUN_609d00]
BYTE* __fastcall Ring_Next(BYTE** self, void*, BYTE* node)
{
    return Ring_After(self, node);
}

// [FUN_6084c0] removes node from the grid's ring at +36, keeping the cursor (+52) valid
void __fastcall Grid_RemoveNodeA(BYTE* self, void*, BYTE* node)
{
    if (node == F(BYTE*, self, 52))
        F(BYTE*, self, 52) = Ring_After(F(BYTE**, self, 36), node);
    F(short, self, 42)--;
    CallThis(FUN_60f690, F(BYTE*, self, 36), node);
}

// [FUN_608f20] removes the edge e from the ring at +32, keeping the cursor (+56)
// valid, and clears the marks (+72) of its end points
void __fastcall Grid_RemoveEdge(BYTE* self, void*, BYTE* e)
{
    if (e == F(BYTE*, self, 56)) {
        BYTE** r = F(BYTE**, self, 32);
        BYTE* d;
        if (r[0] && e == F(BYTE*, r[0], 4))
            d = r[1];
        else if (r[1] && e == F(BYTE*, r[1], 4))
            d = 0;
        else
            d = F(BYTE*, e, 0);
        F(BYTE*, self, 56) = d;
    }
    F(short, self, 64)--;
    BYTE* a = F(BYTE*, e, 8);
    F(int, F(BYTE*, F(BYTE*, a, 0), 12), 72) = 0;
    F(int, F(BYTE*, F(BYTE*, a, 4), 12), 72) = 0;
    BYTE* b = F(BYTE*, e, 12);
    if (b) {
        F(int, F(BYTE*, F(BYTE*, b, 0), 12), 72) = 0;
        F(int, F(BYTE*, F(BYTE*, b, 4), 12), 72) = 0;
    }
    CallThis(FUN_6136f0, F(BYTE*, self, 32), e);
}

// [FUN_609d60] adds a vertex for the point p (two words): its 40-byte record
// and its 16-byte slot with a new 2-byte index (returned)
WORD* __fastcall Grid_AddVertex(BYTE* self, void*, const DWORD* p)
{
    BYTE local[40];
    const BYTE* rec = CallThis<const BYTE*>(FUN_608c70, F(BYTE*, self, 4), local, p);
    WORD n = F(WORD, self, 40);
    memcpy(F(BYTE*, self, 16) + n * 40, rec, 40);
    BYTE* slot = F(BYTE*, self, 12) + n * 16;
    WORD* idx = (WORD*)operator new(2);
    if (idx)
        *idx = n;
    F(WORD*, slot, 0) = idx;
    F(DWORD, slot, 8) = p[0];
    F(DWORD, slot, 12) = p[1];
    F(int, slot, 4) = 0;
    F(WORD, self, 40)++;
    return idx;
}

// [FUN_60c5f0] clears the tables for n vertices and m cells and makes the two rings
void __fastcall Grid_Reset(BYTE* self, void*, int n, int m)
{
    memset(F(BYTE*, self, 12), 0, n * 16);
    memset(F(BYTE*, self, 8), 0, m * 4);
    F(int, self, 52) = 0;
    F(int, self, 56) = 0;
    F(int, self, 24) = 0;
    F(int, self, 28) = 0;
    BYTE* o = (BYTE*)operator new(12);
    F(BYTE*, self, 36) = o ? CallThis<BYTE*>(FUN_613500, o) : 0;
    o = (BYTE*)operator new(12);
    BYTE* r = o ? CallThis<BYTE*>(FUN_613500, o) : 0;
    F(WORD, self, 40) = 0;
    F(WORD, self, 42) = 0;
    F(BYTE*, self, 32) = r;
}

// [FUN_606a80] joins the triangle t with its neighbour when they share a ring
void __stdcall Grid_TryJoin(BYTE* t)
{
    if (F(int, t, 56))
        return;
    BYTE* edi = F(BYTE*, t, 48);
    if (!edi && !F(int, t, 52))
        return;
    BYTE* ebx = F(BYTE*, F(BYTE*, t, 32), 8);
    BYTE* first = F(BYTE*, ebx, 0);
    if (!first)
        return;
    BYTE* second = F(BYTE*, ebx, 4);
    if (!second)
        return;
    BYTE* ebp = F(BYTE*, second, 12);
    BYTE* esi = F(BYTE*, first, 12);
    BYTE* eax;
    if (ebp != t && esi != t) {
        Grd_ReportError((int)0x88760012, (const char*)s_fileLine, (const char*)s_6849fc, 276, (const char*)s_buildDate);
        return;
    }
    if (esi == t) {
        eax = F(BYTE*, t, 52);
        if (!eax) {
            if (!F(BYTE*, edi, 48))
                goto join_check;   // eax == 0
            return;
        }
        BYTE* n = F(BYTE*, eax, 52);
        if (!n)
            return;
        eax = F(BYTE*, F(BYTE*, eax, 32), 8);
        if (eax != F(BYTE*, F(BYTE*, n, 32), 8))
            return;
    } else {
        if (!edi) {
            eax = F(BYTE*, t, 52);
            if (!F(BYTE*, eax, 52)) {
                eax = 0;
                goto join_check;
            }
            return;
        }
        BYTE* n = F(BYTE*, edi, 48);
        if (!n)
            return;
        eax = F(BYTE*, F(BYTE*, edi, 32), 8);
        if (eax != F(BYTE*, F(BYTE*, n, 32), 8))
            return;
    }
    if (eax)
        goto join;
join_check:
    {
        BYTE* r = F(BYTE*, ebx, 8);
        if (!r)
            return;
        if (F(int, r, 0) && F(int, r, 4))
            return;
    }
join:
    CallStd(FUN_6064c0, ebx, eax);
}

}

ASM_PROC(FUN_608fa0)    // links two branches (stdcall)

extern "C" {

// [FUN_60a0b0] joins a cell with its neighbour branch when both sides
// belong to the same parent
void __stdcall Grd_JoinBranches(BYTE* a)
{
    if (F(BYTE*, a, 72))
        return;
    BYTE* p64 = F(BYTE*, a, 64);
    if (!p64 && !F(BYTE*, a, 68))
        return;
    BYTE** b = F(BYTE**, F(BYTE*, a, 40), 8);
    if (!b[0] || !b[1])
        return;
    BYTE* x1 = F(BYTE*, b[1], 12);
    BYTE* x0 = F(BYTE*, b[0], 12);
    bool first;
    if (x1 != a) {
        if (x0 != a) {
            Grd_ReportError((int)2289444626u, " File = %s, Line = #%ld, Date = %s ",
                            "C:\\Nek\\VRAPPLVS\\terrain\\grd.inl", 372, "Jan 16 2003");
            return;
        }
        first = true;
    } else
        first = x0 == a;
    if (first) {
        BYTE* e = F(BYTE*, a, 68);
        if (!e) {
            if (!F(BYTE*, p64, 64))
                CallStd(FUN_608fa0, b, 0);
            return;
        }
        BYTE* f = F(BYTE*, e, 68);
        if (!f)
            return;
        BYTE* k = F(BYTE*, F(BYTE*, e, 40), 8);
        if (k == F(BYTE*, F(BYTE*, f, 40), 8))
            CallStd(FUN_608fa0, b, k);
        return;
    }
    if (!p64) {
        if (!F(BYTE*, F(BYTE*, a, 68), 68))
            CallStd(FUN_608fa0, b, 0);
        return;
    }
    BYTE* f = F(BYTE*, p64, 64);
    if (!f)
        return;
    BYTE* k = F(BYTE*, F(BYTE*, p64, 40), 8);
    if (k == F(BYTE*, F(BYTE*, f, 40), 8))
        CallStd(FUN_608fa0, b, k);
}

}

extern "C" void FUN_60f690(); KING_ALIAS("_FUN_60f690", "@MarineQueue_RemoveNode@12")    // ring list: remove (item)
extern "C" void FUN_606330(); KING_ALIAS("_FUN_606330", "@GridMesh_CreateTriangle@24")    // grid: new triangle
extern "C" void FUN_6136f0(); KING_ALIAS("_FUN_6136f0", "@MarineQueue_RemoveLink@12")
extern "C" void FUN_609980(); KING_ALIAS("_FUN_609980", "@Grid_ResetVtables@8")
extern "C" void FUN_60bda0(); KING_ALIAS("_FUN_60bda0", "@Btr_Delete@12")    // tree node: deleting destructor (flag)

// the item after x in the ring list l (0 at the end)
static BYTE* Ring_Next(BYTE** l, BYTE* x)
{
    BYTE* xl = x ? x + 4 : 0;
    if (l[0] && xl == F(BYTE*, l[0], 8))
        return l[1];
    if (l[1] && xl == F(BYTE*, l[1], 8))
        return 0;
    BYTE* n = F(BYTE*, x, 4);
    return n ? n - 4 : 0;
}

// repoints the neighbour link of x that names from to to (+44, +52, else +48)
static void Relink3(BYTE* x, BYTE* from, BYTE* to)
{
    if (!x)
        return;
    if (F(BYTE*, x, 44) == from)
        F(BYTE*, x, 44) = to;
    else if (F(BYTE*, x, 52) == from)
        F(BYTE*, x, 52) = to;
    else
        F(BYTE*, x, 48) = to;
}

extern "C" {

// [FUN_606b80] replaces the two triangles a and b by one new triangle
void __fastcall Grid_MergeTriangles(BYTE* self, void*, BYTE* a, BYTE* b, BYTE** out)
{
    if (a == F(BYTE*, self, 40))
        F(BYTE*, self, 40) = Ring_Next(F(BYTE**, self, 32), a);
    F(WORD, self, 38)--;
    CallThis(FUN_60f690, F(BYTE*, self, 32), a);
    if (b == F(BYTE*, self, 40))
        F(BYTE*, self, 40) = Ring_Next(F(BYTE**, self, 32), b);
    F(WORD, self, 38)--;
    CallThis(FUN_60f690, F(BYTE*, self, 32), b);
    BYTE* n = CallThis<BYTE*>(FUN_606330, self, F(int, a, 24), F(int, a, 16), F(int, b, 16), F(int, a, 36) - 1);
    F(BYTE*, n, 44) = F(BYTE*, a, 52);
    F(BYTE*, n, 48) = F(BYTE*, a, 44);
    F(BYTE*, n, 52) = F(BYTE*, b, 44);
    Relink3(F(BYTE*, a, 44), a, n);
    Relink3(F(BYTE*, b, 44), b, n);
    BYTE* x = F(BYTE*, a, 52);
    if (x) {
        if (F(BYTE*, x, 44) == a)
            F(BYTE*, x, 44) = n;
        else
            F(BYTE*, x, 48) = n;
    }
    x = F(BYTE*, b, 48);
    if (x) {
        if (F(BYTE*, x, 44) == b)
            F(BYTE*, x, 44) = n;
        else
            F(BYTE*, x, 52) = n;
    }
    *out = n;
    BYTE* t = F(BYTE*, a, 20);
    WORD k = F(WORD, t, 8);
    F(WORD, self, 36)--;
    WORD c = F(WORD, self, 36);
    BYTE** arr = F(BYTE**, self, 12);
    if (k != c) {
        arr[k] = arr[c];
        F(WORD, F(BYTE**, self, 12)[k], 8) = k;
    }
    F(BYTE**, self, 12)[F(WORD, self, 36)] = 0;
    if (t)
        operator delete(t);
    if (a)
        CallVirt(a, 0, 1);
    if (b)
        CallVirt(b, 0, 1);
}

// [FUN_60c900] removes the cell s: unlinks its neighbours and its edge
// record, deletes it and detaches it from its parent node
void __fastcall Grid_RemoveCell(BYTE* self, void*, BYTE* s)
{
    BYTE* parent = F(BYTE*, s, 40);
    if (s == F(BYTE*, self, 52))
        F(BYTE*, self, 52) = Ring_Next(F(BYTE**, self, 36), s);
    F(WORD, self, 42)--;
    CallThis(FUN_60f690, F(BYTE*, self, 36), s);
    BYTE* x = F(BYTE*, s, 60);
    if (x) {
        if (F(BYTE*, x, 60) == s)
            F(BYTE*, x, 60) = 0;
        else if (F(BYTE*, x, 64) == s)
            F(BYTE*, x, 64) = 0;
        else
            F(BYTE*, x, 68) = 0;
    }
    x = F(BYTE*, s, 68);
    if (x) {
        if (F(BYTE*, x, 64) == s)
            F(BYTE*, x, 64) = 0;
        else
            F(BYTE*, x, 60) = 0;
    }
    x = F(BYTE*, s, 64);
    if (x) {
        if (F(BYTE*, x, 68) == s)
            F(BYTE*, x, 68) = 0;
        else
            F(BYTE*, x, 60) = 0;
    }
    BYTE* e = F(BYTE*, s, 72);
    if (e) {
        if (e == F(BYTE*, self, 56)) {
            BYTE** l = F(BYTE**, self, 32);
            BYTE* nx;
            if (l[0] && e == F(BYTE*, l[0], 4))
                nx = l[1];
            else if (l[1] && e == F(BYTE*, l[1], 4))
                nx = 0;
            else
                nx = F(BYTE*, e, 0);
            F(BYTE*, self, 56) = nx;
        }
        F(WORD, self, 64)--;
        BYTE** p = F(BYTE**, e, 8);
        F(int, F(BYTE*, p[0], 12), 72) = 0;
        F(int, F(BYTE*, F(BYTE**, e, 8)[1], 12), 72) = 0;
        BYTE** q = F(BYTE**, e, 12);
        if (q) {
            F(int, F(BYTE*, q[0], 12), 72) = 0;
            F(int, F(BYTE*, F(BYTE**, e, 12)[1], 12), 72) = 0;
        }
        CallThis(FUN_6136f0, F(BYTE*, self, 32), e);
        BYTE* e2 = F(BYTE*, s, 72);
        if (e2) {
            CallThis(FUN_609980, e2);
            operator delete(e2);
        }
    }
    if (s)
        CallVirt(s, 0, 1);
    BYTE** pp = F(BYTE**, parent, 8);
    if (!pp) {
        Grd_ReportError((int)2289444625u, " File = %s, Line = #%ld, Date = %s ",
                        "C:\\Nek\\VRAPPLVS\\terrain\\grd.cpp", 295, "Jan 16 2003");
        return;
    }
    if (!parent)
        return;
    if (pp[0] == parent)
        pp[0] = 0;
    else if (pp[1] == parent)
        pp[1] = 0;
    else
        Grd_ReportError((int)2289444626u, " File = %s, Line = #%ld, Date = %s ",
                        "C:\\Nek\\VRAPPLVS\\terrain\\btr.h", 35, "Jan 16 2003");
    if (F(BYTE*, parent, 0))
        CallThis(FUN_60bda0, F(BYTE*, parent, 0), 1);
    if (F(BYTE*, parent, 4))
        CallThis(FUN_60bda0, F(BYTE*, parent, 4), 1);
    operator delete(parent);
}

}

ASM_PROC(FUN_608530)    // grid: new cell (4 arguments)
extern "C" void FUN_60a440(); KING_ALIAS("_FUN_60a440", "_Ring_Unlink")    // ring list: unlink
extern "C" void FUN_60f1b0(); KING_ALIAS("_FUN_60f1b0", "@MarineTask_ClearRegistration@8")
extern "C" void FUN_60cd90(); KING_ALIAS("_FUN_60cd90", "@MarineVertex_Delete@12")    // grid point: deleting destructor (flag)
extern "C" void* __cdecl memcpy(void* d, const void* s, size_t n);
extern "C" void* __cdecl malloc(size_t n);
typedef BYTE* FreeRing;
ASM_VAR(FreeRing, g_unusedPoints, $L_725d40)
ASM_VAR(int, g_unusedCount, $L_725d44)

// removes point k (the table at +12 has 16-byte records, +16 40-byte point
// data; +40 the count, already decremented): unlinks its edge ring, then
// moves the last point into its slot
static void Grid_RemovePoint(BYTE* self, WORD k)
{
    BYTE* e = F(BYTE*, F(BYTE*, self, 12) + k * 16, 4);
    if (e) {
        BYTE** l = F(BYTE**, self, 44);
        if (l[0] == e && l[1] == e) {
            if ((BYTE*)l == e)
                F(BYTE*, self, 44) = 0;
            else {
                l[0] = (BYTE*)l;
                BYTE** m = F(BYTE**, self, 44);
                m[1] = m[0];
            }
        } else {
            if ((BYTE*)l == e)
                F(BYTE*, self, 44) = F(BYTE*, e, 0);
            CallC(FUN_60a440, e);
        }
        BYTE* e2 = F(BYTE*, F(BYTE*, self, 12) + k * 16, 4);
        if (e2) {
            CallThis(FUN_60f1b0, e2);
            operator delete(e2);
        }
    }
    WORD c = F(WORD, self, 40);
    if (k != c) {
        BYTE* r = F(BYTE*, self, 12);
        memcpy(r + k * 16, r + c * 16, 16);
        BYTE* pts = F(BYTE*, self, 16);
        memcpy(pts + k * 40, pts + F(WORD, self, 40) * 40, 40);
        BYTE* rk = F(BYTE*, self, 12) + k * 16;
        F(WORD, F(BYTE*, rk, 0), 0) = k;
        if (F(BYTE*, rk, 4))
            F(BYTE*, F(BYTE*, rk, 4), 16) = rk;
    }
}

extern "C" {

// [FUN_60a1c0] replaces the two cells a and b by one new cell and removes
// the point of a's corner record (+20)
void __fastcall Grid_MergeCells(BYTE* self, void*, BYTE* a, BYTE* b, BYTE** out)
{
    if (a == F(BYTE*, self, 52))
        F(BYTE*, self, 52) = Ring_Next(F(BYTE**, self, 36), a);
    F(WORD, self, 42)--;
    CallThis(FUN_60f690, F(BYTE*, self, 36), a);
    if (b == F(BYTE*, self, 52))
        F(BYTE*, self, 52) = Ring_Next(F(BYTE**, self, 36), b);
    F(WORD, self, 42)--;
    CallThis(FUN_60f690, F(BYTE*, self, 36), b);
    BYTE* n = CallThis<BYTE*>(FUN_608530, self, F(int, a, 24), F(int, a, 16), F(int, b, 16), F(int, a, 36) - 1);
    F(BYTE*, n, 60) = F(BYTE*, a, 68);
    F(BYTE*, n, 64) = F(BYTE*, a, 60);
    F(BYTE*, n, 68) = F(BYTE*, b, 60);
    BYTE* x;
    for (int i = 0; i < 2; i++) {
        BYTE* from = i ? b : a;
        x = F(BYTE*, from, 60);
        if (x) {
            if (F(BYTE*, x, 60) == from)
                F(BYTE*, x, 60) = n;
            else if (F(BYTE*, x, 68) == from)
                F(BYTE*, x, 68) = n;
            else
                F(BYTE*, x, 64) = n;
        }
    }
    x = F(BYTE*, a, 68);
    if (x) {
        if (F(BYTE*, x, 60) == a)
            F(BYTE*, x, 60) = n;
        else
            F(BYTE*, x, 64) = n;
    }
    x = F(BYTE*, b, 64);
    if (x) {
        if (F(BYTE*, x, 60) == b)
            F(BYTE*, x, 60) = n;
        else
            F(BYTE*, x, 68) = n;
    }
    *out = n;
    BYTE* t = F(BYTE*, a, 20);
    WORD k = F(WORD, t, 0);
    F(WORD, self, 40)--;
    Grid_RemovePoint(self, k);
    if (t)
        operator delete(t);
    if (a)
        CallVirt(a, 0, 1);
    if (b)
        CallVirt(b, 0, 1);
}

// [FUN_60cb10] removes the points no cell of the ring at +36 uses
void __fastcall Grid_RemoveUnusedPoints(BYTE* self, void*)
{
    int* used = (int*)malloc(F(WORD, self, 40) * 4);
    for (int i = 0; i < F(WORD, self, 40); i++)
        used[i] = 0;
    BYTE** l = F(BYTE**, self, 36);
    BYTE* e = l[0] ? l[0] : l[1];
    while (e) {
        used[F(WORD, F(BYTE*, e, 16), 0)] = 1;
        used[F(WORD, F(BYTE*, e, 20), 0)] = 1;
        used[F(WORD, F(BYTE*, e, 24), 0)] = 1;
        e = Ring_Next(F(BYTE**, self, 36), e);
    }
    for (unsigned i = 0; i < F(WORD, self, 40); i++) {
        if (used[i])
            continue;
        BYTE** node = (BYTE**)operator new(12);
        if (!node)
            continue;
        node[1] = (BYTE*)node;
        node[0] = (BYTE*)node;
        node[2] = F(BYTE*, F(BYTE*, self, 12), i * 16);
        g_unusedCount++;
        BYTE** head = (BYTE**)g_unusedPoints;
        if (!head)
            g_unusedPoints = (BYTE*)node;
        else {
            node[0] = (BYTE*)head;
            node[1] = head[1];
            ((BYTE**)head[1])[0] = (BYTE*)node;
            head[1] = (BYTE*)node;
        }
    }
    while (g_unusedPoints) {
        BYTE* pt = ((BYTE**)g_unusedPoints)[2];
        WORD k = F(WORD, pt, 0);
        F(WORD, self, 40)--;
        Grid_RemovePoint(self, k);
        if (pt)
            CallThis(FUN_60cd90, pt, 1);
        BYTE** head = (BYTE**)g_unusedPoints;
        if (head) {
            g_unusedCount--;
            g_unusedPoints = head[0];
            if ((BYTE*)head == head[0])
                g_unusedPoints = 0;
            ((BYTE**)head[0])[1] = head[1];
            ((BYTE**)head[1])[0] = head[0];
            operator delete(head);
        }
    }
}

}

// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_6063f0(); KING_ALIAS("_FUN_6063f0", "@GridTriangle_Destruct@8")
extern "C" void FUN_608aa0(); KING_ALIAS("_FUN_608aa0", "@GridObj_SetVtables@8")
extern "C" void FUN_60bbb0(); KING_ALIAS("_FUN_60bbb0", "@Mesh_Clear@4")

static BYTE* Gsp_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_6063d0]
extern "C" BYTE* __fastcall Gsp6063d0_DeletingDestructor(BYTE* s, void*, int flags) { return Gsp_DeleteAfter(s, flags, FUN_6063f0); }
// [$L_608a80]
extern "C" BYTE* __fastcall Gsp608a80_DeletingDestructor(BYTE* s, void*, int flags) { return Gsp_DeleteAfter(s, flags, FUN_608aa0); }
// [$L_60be20]
extern "C" BYTE* __fastcall Gsp60be20_DeletingDestructor(BYTE* s, void*, int flags) { return Gsp_DeleteAfter(s, flags, FUN_60bbb0); }

// static initializers of constants
ASM_VAR(DWORD, g_gsp7241b8, $L_7241b8)
ASM_VAR(DWORD, g_gsp725d30, $L_725d30)
ASM_VAR(DWORD, g_gsp725d34, $L_725d34)
ASM_VAR(DWORD, g_gsp725d38, $L_725d38)
ASM_VAR(float, g_gsp64c258, $L_64c258)
ASM_VAR(float, g_gsp64c41c, $L_64c41c)
ASM_VAR(float, g_gsp64d4e8, $L_64d4e8)
ASM_VAR(float, g_gsp652e44, $L_652e44)
ASM_VAR(float, g_gsp652e50, $L_652e50)
ASM_VAR(float, g_gsp652f04, $L_652f04)
ASM_VAR(float, g_gsp652f08, $L_652f08)
ASM_VAR(float, g_gsp652f10, $L_652f10)
ASM_VAR(float, g_gsp652f14, $L_652f14)
ASM_VAR(float, g_gsp652f18, $L_652f18)
ASM_VAR(float, g_gsp652f1c, $L_652f1c)
ASM_VAR(float, g_gsp652f20, $L_652f20)
ASM_VAR(float, g_gsp652f2c, $L_652f2c)
ASM_VAR(float, g_gsp652f30, $L_652f30)
ASM_VAR(float, g_gsp653224, $L_653224)
ASM_VAR(float, g_gsp724288, $L_724288)
ASM_VAR(float, g_gsp724290, $L_724290)
ASM_VAR(float, g_gsp724388, $L_724388)
ASM_VAR(float, g_gsp724394, $L_724394)
ASM_VAR(float, g_gsp72439c, $L_72439c)
ASM_VAR(float, g_gsp7243a4, $L_7243a4)
ASM_VAR(float, g_gsp7243ac, $L_7243ac)
ASM_VAR(float, g_gsp7243b4, $L_7243b4)
ASM_VAR(float, g_gsp72495c, $L_72495c)
ASM_VAR(float, g_gsp724960, $L_724960)
ASM_VAR(float, g_gsp72554c, $L_72554c)
ASM_VAR(float, g_gsp725550, $L_725550)
ASM_VAR(float, g_gsp728800, $L_728800)
// [$L_613840]
extern "C" void __cdecl GspInit_613840() {  }
// [$L_6058e0]
extern "C" void __cdecl GspInit_6058e0() { g_gsp7241b8 = 0x0; }
// [$L_607070]
extern "C" void __cdecl GspInit_607070() { g_gsp724290 = (float)((double)g_gsp652e50 + (double)g_gsp652e50); }
// [$L_60a830]
extern "C" void __cdecl GspInit_60a830() { g_gsp724388 = (float)((double)g_gsp652f04 + (double)g_gsp652f04); }
// [$L_60b930]
extern "C" void __cdecl GspInit_60b930() { g_gsp7243a4 = (float)((double)g_gsp652f14 + (double)g_gsp652f14); }
// [$L_60b950]
extern "C" void __cdecl GspInit_60b950() { g_gsp7243ac = (float)((double)g_gsp652f18 + (double)g_gsp652f18); }
// [$L_60b970]
extern "C" void __cdecl GspInit_60b970() { g_gsp7243b4 = (float)((double)g_gsp652f1c + (double)g_gsp652f1c); }
// [$L_60b990]
extern "C" void __cdecl GspInit_60b990() { g_gsp72495c = (float)((double)g_gsp652f20 + (double)g_gsp652f20); }
// [$L_60b9b0]
extern "C" void __cdecl GspInit_60b9b0() { g_gsp724960 = (float)((double)g_gsp64c41c / g_gsp652f20); }
// [$L_60bb30]
extern "C" void __cdecl GspInit_60bb30() { g_gsp72554c = (float)((double)g_gsp652f2c + (double)g_gsp652f2c); }
// [$L_60bb80]
extern "C" void __cdecl GspInit_60bb80() { g_gsp725d30 = 0x0; g_gsp725d34 = 0x0; g_gsp725d38 = 0x0; }
// [$L_605820]
extern "C" void __cdecl GspInit_605820() { g_gsp724288 = (float)((double)g_gsp652e44 + (double)g_gsp652e44); }
// [$L_60a860]
extern "C" void __cdecl GspInit_60a860() { g_gsp724394 = (float)((double)g_gsp652f08 + (double)g_gsp652f08); }
// [$L_60aa40]
extern "C" void __cdecl GspInit_60aa40() { g_gsp72439c = (float)((double)g_gsp652f10 + (double)g_gsp652f10); }
// [$L_60bb50]
extern "C" void __cdecl GspInit_60bb50() { g_gsp725550 = (float)((double)g_gsp652f30 - g_gsp64d4e8 - g_gsp64c258); }
// [$L_613a60]
extern "C" void __cdecl GspInit_613a60() { g_gsp728800 = (float)((double)g_gsp653224 + (double)g_gsp653224); }
ASM_VAR(AsmLabel, g_gsp652e48, $L_652e48)
ASM_VAR(AsmLabel, g_gsp652ef4, $L_652ef4)

// [$L_606150] the base destructor (vtable 652e48) inlined
extern "C" BYTE* __fastcall Gsp606150_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_gsp652e48;
    if (flags & 1)
        operator delete(s);
    return s;
}
// [$L_608010] the base destructor (vtable 652ef4) inlined
extern "C" BYTE* __fastcall Gsp608010_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_gsp652ef4;
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

extern "C" void FUN_60ba20(); KING_ALIAS("_FUN_60ba20", "_Tables_InitSin")
extern "C" void FUN_60bab0(); KING_ALIAS("_FUN_60bab0", "_Tables_InitCos")
ASM_VAR(float, g_q47_64c264, $L_64c264)
ASM_VAR(float, g_q47_652f24, $L_652f24)
ASM_VAR(float, g_q47_64c904, $L_64c904)
ASM_VAR(float, g_q47_64c250, $L_64c250)
ASM_VAR(float, g_q47_724f04, $L_724f04)
// [$L_60baf0] the 300-entry table 724f04: 64c250 - x * 64c904 with x stepping by 652f24 from 64c264
extern "C" void __cdecl Tables_InitLinear(void)
{
    double x = g_q47_64c264;
    for (float* p = &g_q47_724f04; p < &g_q47_724f04 + 300; p++) {
        x += g_q47_652f24;
        *p = (float)(g_q47_64c250 - x * g_q47_64c904);
    }
}
// [FUN_60bb20] fills the sine, cosine and linear tables
extern "C" void __cdecl Tables_InitAll(void)
{
    CallC(FUN_60ba20);
    CallC(FUN_60bab0);
    Tables_InitLinear();
}

// The DirectX (Direct3D/DirectDraw) error names of the grid code.
ASM_VAR(AsmLabel, s_dx684aa4, $L_684aa4)
ASM_VAR(AsmLabel, s_dx685a74, $L_685a74)
ASM_VAR(AsmLabel, s_dx685a88, $L_685a88)
ASM_VAR(AsmLabel, s_dx685a5c, $L_685a5c)
ASM_VAR(AsmLabel, s_dx685a34, $L_685a34)
ASM_VAR(AsmLabel, s_dx685a48, $L_685a48)
ASM_VAR(AsmLabel, s_dx685a18, $L_685a18)
ASM_VAR(AsmLabel, s_dx6859fc, $L_6859fc)
ASM_VAR(AsmLabel, s_dx6859e0, $L_6859e0)
ASM_VAR(AsmLabel, s_dx6859c8, $L_6859c8)
ASM_VAR(AsmLabel, s_dx6859b8, $L_6859b8)
ASM_VAR(AsmLabel, s_dx6859a4, $L_6859a4)
ASM_VAR(AsmLabel, s_dx685988, $L_685988)
ASM_VAR(AsmLabel, s_dx685974, $L_685974)
ASM_VAR(AsmLabel, s_dx68595c, $L_68595c)
ASM_VAR(AsmLabel, s_dx685948, $L_685948)
ASM_VAR(AsmLabel, s_dx685934, $L_685934)
ASM_VAR(AsmLabel, s_dx685918, $L_685918)
ASM_VAR(AsmLabel, s_dx685904, $L_685904)
ASM_VAR(AsmLabel, s_dx6858ec, $L_6858ec)
ASM_VAR(AsmLabel, s_dx6858e0, $L_6858e0)
ASM_VAR(AsmLabel, s_dx6858d0, $L_6858d0)
ASM_VAR(AsmLabel, s_dx6858bc, $L_6858bc)
ASM_VAR(AsmLabel, s_dx6858a8, $L_6858a8)
ASM_VAR(AsmLabel, s_dx685864, $L_685864)
ASM_VAR(AsmLabel, s_dx685894, $L_685894)
ASM_VAR(AsmLabel, s_dx685880, $L_685880)
ASM_VAR(AsmLabel, s_dx685848, $L_685848)
ASM_VAR(AsmLabel, s_dx685830, $L_685830)
ASM_VAR(AsmLabel, s_dx685820, $L_685820)
ASM_VAR(AsmLabel, s_dx685814, $L_685814)
ASM_VAR(AsmLabel, s_dx685800, $L_685800)
ASM_VAR(AsmLabel, s_dx68574c, $L_68574c)
ASM_VAR(AsmLabel, s_dx6857ec, $L_6857ec)
ASM_VAR(AsmLabel, s_dx6857d8, $L_6857d8)
ASM_VAR(AsmLabel, s_dx6857c4, $L_6857c4)
ASM_VAR(AsmLabel, s_dx6857b0, $L_6857b0)
ASM_VAR(AsmLabel, s_dx68579c, $L_68579c)
ASM_VAR(AsmLabel, s_dx685784, $L_685784)
ASM_VAR(AsmLabel, s_dx685770, $L_685770)
ASM_VAR(AsmLabel, s_dx68575c, $L_68575c)
ASM_VAR(AsmLabel, s_dx68573c, $L_68573c)
ASM_VAR(AsmLabel, s_dx685728, $L_685728)
ASM_VAR(AsmLabel, s_dx685714, $L_685714)
ASM_VAR(AsmLabel, s_dx685548, $L_685548)
ASM_VAR(AsmLabel, s_dx685530, $L_685530)
ASM_VAR(AsmLabel, s_dx685518, $L_685518)
ASM_VAR(AsmLabel, s_dx6854f4, $L_6854f4)
ASM_VAR(AsmLabel, s_dx6854c8, $L_6854c8)
ASM_VAR(AsmLabel, s_dx6856b0, $L_6856b0)
ASM_VAR(AsmLabel, s_dx68546c, $L_68546c)
ASM_VAR(AsmLabel, s_dx68544c, $L_68544c)
ASM_VAR(AsmLabel, s_dx685438, $L_685438)
ASM_VAR(AsmLabel, s_dx6856e4, $L_6856e4)
ASM_VAR(AsmLabel, s_dx685420, $L_685420)
ASM_VAR(AsmLabel, s_dx68540c, $L_68540c)
ASM_VAR(AsmLabel, s_dx6853f0, $L_6853f0)
ASM_VAR(AsmLabel, s_dx6853dc, $L_6853dc)
ASM_VAR(AsmLabel, s_dx6853c8, $L_6853c8)
ASM_VAR(AsmLabel, s_dx6853b4, $L_6853b4)
ASM_VAR(AsmLabel, s_dx68539c, $L_68539c)
ASM_VAR(AsmLabel, s_dx685384, $L_685384)
ASM_VAR(AsmLabel, s_dx685364, $L_685364)
ASM_VAR(AsmLabel, s_dx68534c, $L_68534c)
ASM_VAR(AsmLabel, s_dx68533c, $L_68533c)
ASM_VAR(AsmLabel, s_dx685644, $L_685644)
ASM_VAR(AsmLabel, s_dx685690, $L_685690)
ASM_VAR(AsmLabel, s_dx6855d8, $L_6855d8)
ASM_VAR(AsmLabel, s_dx6854a4, $L_6854a4)
ASM_VAR(AsmLabel, s_dx6855c4, $L_6855c4)
ASM_VAR(AsmLabel, s_dx68548c, $L_68548c)
ASM_VAR(AsmLabel, s_dx6856c8, $L_6856c8)
ASM_VAR(AsmLabel, s_dx685604, $L_685604)
ASM_VAR(AsmLabel, s_dx6855b4, $L_6855b4)
ASM_VAR(AsmLabel, s_dx685660, $L_685660)
ASM_VAR(AsmLabel, s_dx685678, $L_685678)
ASM_VAR(AsmLabel, s_dx685588, $L_685588)
ASM_VAR(AsmLabel, s_dx685574, $L_685574)
ASM_VAR(AsmLabel, s_dx6856fc, $L_6856fc)
ASM_VAR(AsmLabel, s_dx68561c, $L_68561c)
ASM_VAR(AsmLabel, s_dx6855f0, $L_6855f0)
ASM_VAR(AsmLabel, s_dx6854dc, $L_6854dc)
ASM_VAR(AsmLabel, s_dx6855a0, $L_6855a0)
ASM_VAR(AsmLabel, s_dx68562c, $L_68562c)
ASM_VAR(AsmLabel, s_dx685558, $L_685558)
ASM_VAR(AsmLabel, s_dx68531c, $L_68531c)
ASM_VAR(AsmLabel, s_dx68528c, $L_68528c)
ASM_VAR(AsmLabel, s_dx6852f4, $L_6852f4)
ASM_VAR(AsmLabel, s_dx68527c, $L_68527c)
ASM_VAR(AsmLabel, s_dx685308, $L_685308)
ASM_VAR(AsmLabel, s_dx6852b4, $L_6852b4)
ASM_VAR(AsmLabel, s_dx685240, $L_685240)
ASM_VAR(AsmLabel, s_dx6852dc, $L_6852dc)
ASM_VAR(AsmLabel, s_dx685268, $L_685268)
ASM_VAR(AsmLabel, s_dx685250, $L_685250)
ASM_VAR(AsmLabel, s_dx6852a0, $L_6852a0)
ASM_VAR(AsmLabel, s_dx6852c0, $L_6852c0)
ASM_VAR(AsmLabel, s_dx685228, $L_685228)
ASM_VAR(AsmLabel, s_dx6851d4, $L_6851d4)
ASM_VAR(AsmLabel, s_dx685214, $L_685214)
ASM_VAR(AsmLabel, s_dx6851fc, $L_6851fc)
ASM_VAR(AsmLabel, s_dx6851c0, $L_6851c0)
ASM_VAR(AsmLabel, s_dx6851ec, $L_6851ec)
ASM_VAR(AsmLabel, s_dx6851a8, $L_6851a8)
ASM_VAR(AsmLabel, s_dx685190, $L_685190)
ASM_VAR(AsmLabel, s_dx685178, $L_685178)
ASM_VAR(AsmLabel, s_dx685160, $L_685160)
ASM_VAR(AsmLabel, s_dx68514c, $L_68514c)
ASM_VAR(AsmLabel, s_dx68512c, $L_68512c)
ASM_VAR(AsmLabel, s_dx68510c, $L_68510c)
ASM_VAR(AsmLabel, s_dx6850f0, $L_6850f0)
ASM_VAR(AsmLabel, s_dx6850d0, $L_6850d0)
ASM_VAR(AsmLabel, s_dx6850b8, $L_6850b8)
ASM_VAR(AsmLabel, s_dx68509c, $L_68509c)
ASM_VAR(AsmLabel, s_dx685084, $L_685084)
ASM_VAR(AsmLabel, s_dx68506c, $L_68506c)
ASM_VAR(AsmLabel, s_dx685050, $L_685050)
ASM_VAR(AsmLabel, s_dx685030, $L_685030)
ASM_VAR(AsmLabel, s_dx685010, $L_685010)
ASM_VAR(AsmLabel, s_dx684ff4, $L_684ff4)
ASM_VAR(AsmLabel, s_dx684fd4, $L_684fd4)
ASM_VAR(AsmLabel, s_dx684fc0, $L_684fc0)
ASM_VAR(AsmLabel, s_dx684fac, $L_684fac)
ASM_VAR(AsmLabel, s_dx684f94, $L_684f94)
ASM_VAR(AsmLabel, s_dx684f78, $L_684f78)
ASM_VAR(AsmLabel, s_dx684f58, $L_684f58)
ASM_VAR(AsmLabel, s_dx684f3c, $L_684f3c)
ASM_VAR(AsmLabel, s_dx684f1c, $L_684f1c)
ASM_VAR(AsmLabel, s_dx684efc, $L_684efc)
ASM_VAR(AsmLabel, s_dx684edc, $L_684edc)
ASM_VAR(AsmLabel, s_dx684ebc, $L_684ebc)
ASM_VAR(AsmLabel, s_dx684e9c, $L_684e9c)
ASM_VAR(AsmLabel, s_dx684e80, $L_684e80)
ASM_VAR(AsmLabel, s_dx684e64, $L_684e64)
ASM_VAR(AsmLabel, s_dx684e4c, $L_684e4c)
ASM_VAR(AsmLabel, s_dx684e2c, $L_684e2c)
ASM_VAR(AsmLabel, s_dx684e0c, $L_684e0c)
ASM_VAR(AsmLabel, s_dx684dec, $L_684dec)
ASM_VAR(AsmLabel, s_dx684dcc, $L_684dcc)
ASM_VAR(AsmLabel, s_dx684db4, $L_684db4)
ASM_VAR(AsmLabel, s_dx684d94, $L_684d94)
ASM_VAR(AsmLabel, s_dx684d74, $L_684d74)
ASM_VAR(AsmLabel, s_dx684d58, $L_684d58)
ASM_VAR(AsmLabel, s_dx684d40, $L_684d40)
ASM_VAR(AsmLabel, s_dx684d28, $L_684d28)
ASM_VAR(AsmLabel, s_dx684d08, $L_684d08)
ASM_VAR(AsmLabel, s_dx684cf0, $L_684cf0)
ASM_VAR(AsmLabel, s_dx684cd4, $L_684cd4)
ASM_VAR(AsmLabel, s_dx684cb8, $L_684cb8)
ASM_VAR(AsmLabel, s_dx684ca0, $L_684ca0)
ASM_VAR(AsmLabel, s_dx684c90, $L_684c90)
ASM_VAR(AsmLabel, s_dx684c7c, $L_684c7c)
ASM_VAR(AsmLabel, s_dx684c68, $L_684c68)
ASM_VAR(AsmLabel, s_dx684c4c, $L_684c4c)
ASM_VAR(AsmLabel, s_dx684c30, $L_684c30)
ASM_VAR(AsmLabel, s_dx684c0c, $L_684c0c)
ASM_VAR(AsmLabel, s_dx684b74, $L_684b74)
ASM_VAR(AsmLabel, s_dx684b90, $L_684b90)
ASM_VAR(AsmLabel, s_dx684bb4, $L_684bb4)
ASM_VAR(AsmLabel, s_dx684bd0, $L_684bd0)
ASM_VAR(AsmLabel, s_dx684bec, $L_684bec)
ASM_VAR(AsmLabel, s_dx684b54, $L_684b54)
ASM_VAR(AsmLabel, s_dx684b34, $L_684b34)
ASM_VAR(AsmLabel, s_dx684b10, $L_684b10)
ASM_VAR(AsmLabel, s_dx684b00, $L_684b00)
ASM_VAR(AsmLabel, s_dx684ae8, $L_684ae8)
ASM_VAR(AsmLabel, s_dx684ad0, $L_684ad0)
ASM_VAR(AsmLabel, s_dx684abc, $L_684abc)
ASM_VAR(AsmLabel, s_dx684aac, $L_684aac)
ASM_VAR(AsmLabel, s_dxUnknownFmt, $L_684a90)
extern "C" char* __cdecl strncpy(char* d, const char* s, size_t n);
struct DxErrName { DWORD hr; const char* name; };
static const DxErrName k_dxErrNames[] = {
    {0x00000000, s_dx684aa4},
    {0x80004001, s_dx685a74},
    {0x80004005, s_dx685a88},
    {0x800401F0, s_dx685a5c},
    {0x8007000E, s_dx685a34},
    {0x80070057, s_dx685a48},
    {0x88760005, s_dx685a18},
    {0x8876000A, s_dx6859fc},
    {0x88760014, s_dx6859e0},
    {0x88760028, s_dx6859c8},
    {0x88760037, s_dx6859b8},
    {0x8876005A, s_dx6859a4},
    {0x8876005F, s_dx685988},
    {0x88760064, s_dx685974},
    {0x8876006E, s_dx68595c},
    {0x88760078, s_dx685948},
    {0x88760082, s_dx685934},
    {0x88760091, s_dx685918},
    {0x88760096, s_dx685904},
    {0x887600A0, s_dx6858ec},
    {0x887600AA, s_dx6858e0},
    {0x887600B4, s_dx6858d0},
    {0x887600CD, s_dx6858bc},
    {0x887600D2, s_dx6858a8},
    {0x887600D4, s_dx685864},
    {0x887600D7, s_dx685894},
    {0x887600DC, s_dx685880},
    {0x887600DE, s_dx685848},
    {0x887600E1, s_dx685830},
    {0x887600E6, s_dx685820},
    {0x887600F0, s_dx685814},
    {0x887600FA, s_dx685800},
    {0x887600FF, s_dx68574c},
    {0x88760104, s_dx6857ec},
    {0x88760118, s_dx6857d8},
    {0x88760122, s_dx6857c4},
    {0x88760136, s_dx6857b0},
    {0x8876013C, s_dx68579c},
    {0x8876013D, s_dx685784},
    {0x88760140, s_dx685770},
    {0x8876014A, s_dx68575c},
    {0x8876014F, s_dx68573c},
    {0x88760154, s_dx685728},
    {0x8876015E, s_dx685714},
    {0x88760168, s_dx685548},
    {0x8876017C, s_dx685530},
    {0x8876017E, s_dx685518},
    {0x88760180, s_dx6854f4},
    {0x88760183, s_dx6854c8},
    {0x88760190, s_dx6856b0},
    {0x8876019A, s_dx68546c},
    {0x887601A4, s_dx68544c},
    {0x887601AE, s_dx685438},
    {0x887601B3, s_dx6856e4},
    {0x887601B8, s_dx685420},
    {0x887601C2, s_dx68540c},
    {0x887601CC, s_dx6853f0},
    {0x887601D6, s_dx6853dc},
    {0x887601E0, s_dx6853c8},
    {0x887601EA, s_dx6853b4},
    {0x887601FE, s_dx68539c},
    {0x88760208, s_dx685384},
    {0x88760219, s_dx685364},
    {0x8876021C, s_dx68534c},
    {0x88760230, s_dx68533c},
    {0x88760231, s_dx685644},
    {0x88760232, s_dx685690},
    {0x88760233, s_dx6855d8},
    {0x88760234, s_dx6854a4},
    {0x88760235, s_dx6855c4},
    {0x88760236, s_dx68548c},
    {0x88760237, s_dx6856c8},
    {0x88760238, s_dx685604},
    {0x88760239, s_dx6855b4},
    {0x8876023A, s_dx685660},
    {0x8876023B, s_dx685678},
    {0x8876023C, s_dx685588},
    {0x8876023D, s_dx685574},
    {0x8876023E, s_dx6856fc},
    {0x8876023F, s_dx68561c},
    {0x88760240, s_dx6855f0},
    {0x88760241, s_dx6854dc},
    {0x88760242, s_dx6855a0},
    {0x88760243, s_dx68562c},
    {0x88760244, s_dx685558},
    {0x88760245, s_dx68531c},
    {0x88760246, s_dx68528c},
    {0x88760247, s_dx6852f4},
    {0x88760248, s_dx68527c},
    {0x88760249, s_dx685308},
    {0x8876024A, s_dx6852b4},
    {0x8876024B, s_dx685240},
    {0x8876024C, s_dx6852dc},
    {0x8876024D, s_dx685268},
    {0x8876024E, s_dx685250},
    {0x8876024F, s_dx6852a0},
    {0x88760250, s_dx6852c0},
    {0x8876026C, s_dx685228},
    {0x88760276, s_dx6851d4},
    {0x88760280, s_dx685214},
    {0x88760294, s_dx6851fc},
    {0x887602A8, s_dx6851c0},
    {0x887602B2, s_dx6851ec},
    {0x887602B7, s_dx6851a8},
    {0x887602BC, s_dx685190},
    {0x887602BD, s_dx685178},
    {0x887602C1, s_dx685160},
    {0x887602C2, s_dx68514c},
    {0x887602C6, s_dx68512c},
    {0x887602C7, s_dx68510c},
    {0x887602C8, s_dx6850f0},
    {0x887602C9, s_dx6850d0},
    {0x887602CA, s_dx6850b8},
    {0x887602CB, s_dx68509c},
    {0x887602CC, s_dx685084},
    {0x887602CD, s_dx68506c},
    {0x887602D0, s_dx685050},
    {0x887602D1, s_dx685030},
    {0x887602D2, s_dx685010},
    {0x887602D3, s_dx684ff4},
    {0x887602D4, s_dx684fd4},
    {0x887602D5, s_dx684fc0},
    {0x887602D6, s_dx684fac},
    {0x887602D7, s_dx684f94},
    {0x887602D8, s_dx684f78},
    {0x887602D9, s_dx684f58},
    {0x887602DA, s_dx684f3c},
    {0x887602DB, s_dx684f1c},
    {0x887602DC, s_dx684efc},
    {0x887602DD, s_dx684edc},
    {0x887602DE, s_dx684ebc},
    {0x887602DF, s_dx684e9c},
    {0x887602E0, s_dx684e80},
    {0x887602E1, s_dx684e64},
    {0x887602E2, s_dx684e4c},
    {0x887602E4, s_dx684e2c},
    {0x887602E5, s_dx684e0c},
    {0x887602E6, s_dx684dec},
    {0x887602E7, s_dx684dcc},
    {0x887602E8, s_dx684db4},
    {0x887602E9, s_dx684d94},
    {0x887602EA, s_dx684d74},
    {0x887602EB, s_dx684d58},
    {0x887602EE, s_dx684d40},
    {0x887602EF, s_dx684d28},
    {0x887602F0, s_dx684d08},
    {0x887602F8, s_dx684cf0},
    {0x887602F9, s_dx684cd4},
    {0x887602FA, s_dx684cb8},
    {0x887602FB, s_dx684ca0},
    {0x88760302, s_dx684c90},
    {0x88760303, s_dx684c7c},
    {0x88760304, s_dx684c68},
    {0x88760305, s_dx684c4c},
    {0x88760306, s_dx684c30},
    {0x88760819, s_dx684c0c},
    {0x8876081A, s_dx684b74},
    {0x8876081B, s_dx684b90},
    {0x8876081C, s_dx684bb4},
    {0x8876081D, s_dx684bd0},
    {0x8876081E, s_dx684bec},
    {0x8876081F, s_dx684b54},
    {0x88760822, s_dx684b34},
    {0x88760826, s_dx684b10},
    {0x88762711, s_dx684b00},
    {0x88762712, s_dx684ae8},
    {0x88762713, s_dx684ad0},
    {0x88762714, s_dx684abc},
    {0x88762715, s_dx684aac},
};

extern "C" {

// [FUN_60ab70] the name of DirectX error hr into buf (size bytes; an unknown error by its low 16 bits);
// 0 without a buffer. An empty name leaves buf unterminated.
int __cdecl Grd_DxErrorName(DWORD hr, char* buf, unsigned size)
{
    if (!buf || !size)
        return 0;
    char tmp[256];
    const char* s = 0;
    for (int i = 0; i < sizeof(k_dxErrNames) / sizeof(k_dxErrNames[0]); i++)
        if (k_dxErrNames[i].hr == hr) {
            s = k_dxErrNames[i].name;
            break;
        }
    if (!s) {
        wsprintfA(tmp, s_dxUnknownFmt, hr & 65535);
        s = tmp;
    }
    unsigned n = strlen(s);
    if (n >= size)
        n = size - 1;
    if (n) {
        strncpy(buf, s, n);
        buf[n] = 0;
    }
    return 1;
}

}  // extern "C"


// === splitting grid triangles (FUN_605900)

ASM_VAR(int, g_gsDepth7241b8, $L_7241b8)       // the count of the triangle stack
ASM_VAR(AsmLabel, g_gsStack7241bc, $L_7241bc)   // the stack, from index 1
ASM_VAR(float, g_gsHalf64c258, $L_64c258)
extern "C" void FUN_60f4f0(); KING_ALIAS("_FUN_60f4f0", "@MarineQueue_SetNodePriority@16")
extern "C" void FUN_606440(); KING_ALIAS("_FUN_606440", "@GridMesh_RemoveEdge@12")
extern "C" void FUN_606730(); KING_ALIAS("_FUN_606730", "@Grid606750_Delete@12")
extern "C" void FUN_6062c0(); KING_ALIAS("_FUN_6062c0", "@GridMesh_RemoveTriangle@12")
extern "C" void FUN_606400(); KING_ALIAS("_FUN_606400", "@GridMesh_CreatePoint@12")
extern "C" void FUN_606330(); KING_ALIAS("_FUN_606330", "@GridMesh_CreateTriangle@24")
extern "C" void FUN_606100(); KING_ALIAS("_FUN_606100", "@GridEdge_Construct@8")
extern "C" void FUN_606170(); KING_ALIAS("_FUN_606170", "@GridEdge_SetPairs@16")
extern "C" void FUN_60f900(); KING_ALIAS("_FUN_60f900", "@MarineQueue_InsertEdge@12")
extern "C" void FUN_6064c0(); KING_ALIAS("_FUN_6064c0", "@GridMesh_CreateEdge@16")

static BYTE** Gs_Stack()
{
    return (BYTE**)(char*)g_gsStack7241bc;
}

static void Gs_Push(BYTE* t)
{
    Gs_Stack()[++g_gsDepth7241b8] = t;
}

static __declspec(noinline) int Gs_Mid(DWORD a, DWORD b)
{
    unsigned __int64 s = (DWORD)(a + b);
    return (int)((double)(__int64)s * g_gsHalf64c258);
}

// drops the edge object e of a triangle: its triangles' back links (+56), then the edge itself
static void Gs_DropEdge(BYTE* mesh, BYTE* e, bool checked)
{
    BYTE** p = F(BYTE**, e, 8);
    if (!checked || p[0])
        F(BYTE*, F(BYTE*, p[0], 12), 56) = 0;
    if (!checked || p[1])
        F(BYTE*, F(BYTE*, p[1], 12), 56) = 0;
    if (BYTE** q = F(BYTE**, e, 12)) {
        if (!checked || q[0])
            F(BYTE*, F(BYTE*, q[0], 12), 56) = 0;
        if (!checked || q[1])
            F(BYTE*, F(BYTE*, q[1], 12), 56) = 0;
    }
    CallThis(FUN_606440, mesh, e);
    CallThis(FUN_606730, e, 1);
}

// the two halves of triangle t at point p: (+20, p, +16) and (+24, p, +20), one level deeper
static void Gs_Halves(BYTE* mesh, BYTE* t, BYTE* p, BYTE** a, BYTE** b)
{
    *a = CallThis<BYTE*>(FUN_606330, mesh, F(BYTE*, t, 20), p, F(BYTE*, t, 16), F(int, t, 36) + 1);
    *b = CallThis<BYTE*>(FUN_606330, mesh, F(BYTE*, t, 24), p, F(BYTE*, t, 20), F(int, t, 36) + 1);
}

// the neighbor n of old (if any) now has neighbor nu instead (at +44, else at off)
static void Gs_Relink(BYTE* n, BYTE* old, BYTE* nu, int off)
{
    if (!n)
        return;
    if (F(BYTE*, n, 44) == old)
        F(BYTE*, n, 44) = nu;
    else
        F(BYTE*, n, off) = nu;
}

static BYTE* Gs_NewNode()
{
    BYTE* n = (BYTE*)operator new(20);
    if (n) {
        F(int, n, 8) = 0;
        F(int, n, 4) = 0;
        F(int, n, 0) = 0;
        F(int, n, 12) = 0;
    }
    return n;
}

// the quadtree node q of triangle t gets children n0 (triangle a) and n1 (triangle b)
static void Gs_Children(BYTE* t, BYTE* a, BYTE* b, BYTE* n0, BYTE* n1)
{
    BYTE* q = F(BYTE*, t, 32);
    F(BYTE*, n0, 8) = q;
    F(BYTE*, n0, 12) = a;
    F(int, n0, 16) = F(int, t, 36) + 1;
    F(BYTE*, n1, 8) = q;
    F(BYTE*, n1, 12) = b;
    F(int, n1, 16) = F(int, t, 36) + 1;
    F(BYTE*, a, 32) = n0;
    F(BYTE*, b, 32) = n1;
    F(BYTE*, q, 0) = n0;
    F(BYTE*, q, 4) = n1;
    F(int, q, 12) = 0;
}

extern "C" {

// [FUN_605900] refines the grid mesh at triangle node e (+12): unless deep enough (mesh+4 vtbl+16), splits the
// triangle, and the chain of neighbors (+44) up to a mutual pair, at the midpoints of their long edges (+16..+24),
// relinking neighbors and quadtree nodes; a final pair gets a new edge object priced by mesh+4 vtbl+32
void __cdecl GridMesh_Refine(BYTE* e, BYTE* mesh)
{
    if (F(DWORD, e, 16) >= CallVirt<DWORD>(F(BYTE*, mesh, 4), 16)) {
        CallThis(FUN_60f4f0, F(BYTE*, mesh, 32), F(BYTE*, e, 12), 0);
        return;
    }
    BYTE* t = F(BYTE*, e, 12);
    Gs_Push(t);
    for (;;) {
        BYTE* n = F(BYTE*, t, 44);
        Gs_Push(n);
        if (!n || F(BYTE*, n, 44) == t)
            break;
        t = n;
    }
    for (;;) {
        BYTE* t1 = 0;
        BYTE* t2 = 0;
        if (g_gsDepth7241b8) {
            t1 = Gs_Stack()[g_gsDepth7241b8--];
            if (g_gsDepth7241b8)
                t2 = Gs_Stack()[g_gsDepth7241b8--];
        }
        if (t1 && t2) {
            int pt[2];
            pt[0] = Gs_Mid(F(DWORD, F(BYTE*, t1, 24), 0), F(DWORD, F(BYTE*, t1, 16), 0));
            pt[1] = Gs_Mid(F(DWORD, F(BYTE*, t1, 16), 4), F(DWORD, F(BYTE*, t1, 24), 4));
            if (BYTE* e1 = F(BYTE*, t1, 56))
                Gs_DropEdge(mesh, e1, false);
            CallThis(FUN_6062c0, mesh, t1);
            BYTE* p = CallThis<BYTE*>(FUN_606400, mesh, pt);
            BYTE *a, *b, *c, *d;
            Gs_Halves(mesh, t1, p, &a, &b);
            if (BYTE* e2 = F(BYTE*, t2, 56))
                Gs_DropEdge(mesh, e2, false);
            CallThis(FUN_6062c0, mesh, t2);
            Gs_Halves(mesh, t2, p, &c, &d);
            F(BYTE*, a, 52) = d;
            F(BYTE*, d, 48) = a;
            F(BYTE*, b, 48) = c;
            F(BYTE*, c, 52) = b;
            F(BYTE*, a, 48) = b;
            F(BYTE*, b, 52) = a;
            F(BYTE*, a, 44) = F(BYTE*, t1, 48);
            F(BYTE*, b, 44) = F(BYTE*, t1, 52);
            Gs_Relink(F(BYTE*, t1, 48), t1, a, 52);
            Gs_Relink(F(BYTE*, t1, 52), t1, b, 48);
            F(BYTE*, c, 48) = d;
            F(BYTE*, d, 52) = c;
            F(BYTE*, c, 44) = F(BYTE*, t2, 48);
            F(BYTE*, d, 44) = F(BYTE*, t2, 52);
            Gs_Relink(F(BYTE*, t2, 48), t2, c, 52);
            Gs_Relink(F(BYTE*, t2, 52), t2, d, 48);
            BYTE* n0 = Gs_NewNode();
            BYTE* n1 = Gs_NewNode();
            BYTE* n2 = Gs_NewNode();
            BYTE* n3 = Gs_NewNode();
            Gs_Children(t1, a, b, n0, n1);
            Gs_Children(t2, c, d, n2, n3);
            if (g_gsDepth7241b8 == 0) {
                // the last pair: a new edge object between the two quadtree nodes
                BYTE* q2 = F(BYTE*, t2, 32);
                BYTE* q1 = F(BYTE*, t1, 32);
                BYTE* g = (BYTE*)operator new(108);
                g = g ? CallThis<BYTE*>(FUN_606100, g) : 0;
                if (!CallThis<int>(FUN_606170, g, q1, q2)) {
                    if (g)
                        CallThis(FUN_606730, g, 1);
                } else {
                    if (q1) {
                        F(BYTE*, F(BYTE*, F(BYTE*, q1, 0), 12), 56) = g;
                        F(BYTE*, F(BYTE*, F(BYTE*, q1, 4), 12), 56) = g;
                    }
                    if (q2) {
                        F(BYTE*, F(BYTE*, F(BYTE*, q2, 0), 12), 56) = g;
                        F(BYTE*, F(BYTE*, F(BYTE*, q2, 4), 12), 56) = g;
                    }
                    BYTE* pricer = F(BYTE*, mesh, 4);
                    typedef double(__thiscall * PriceFn)(BYTE*, int, int, int, BYTE*, int, int, int, BYTE*);
                    PriceFn fn = *(PriceFn*)(*(BYTE**)pricer + 32);
                    if (F(int, g, 12))
                        F(float, g, 72) = (float)fn(pricer, F(int, g, 20), F(int, g, 24), F(int, g, 28), g + 16,
                                                    F(int, g, 48), F(int, g, 52), F(int, g, 56), g + 44);
                    else
                        F(float, g, 72) = (float)fn(pricer, F(int, g, 20), F(int, g, 24), F(int, g, 28), g + 16,
                                                    0, 0, 0, 0);
                    CallThis(FUN_60f900, F(BYTE*, mesh, 28), g);
                    F(short, mesh, 52)++;
                }
                CallVirt(t1, 0, 1);
                CallVirt(t2, 0, 1);
                return;
            }
            BYTE* top = Gs_Stack()[g_gsDepth7241b8];
            Gs_Push(F(BYTE*, t2, 48) == top ? c : d);
            CallVirt(t1, 0, 1);
            CallVirt(t2, 0, 1);
            continue;
        }
        // a single triangle (no neighbor to split with)
        BYTE* s = t2 ? t2 : t1;
        int pt[2];
        int y = Gs_Mid(F(DWORD, F(BYTE*, s, 24), 4), F(DWORD, F(BYTE*, s, 16), 4));
        pt[0] = Gs_Mid(F(DWORD, F(BYTE*, s, 16), 0), F(DWORD, F(BYTE*, s, 24), 0));
        pt[1] = y;
        if (BYTE* e1 = F(BYTE*, s, 56))
            Gs_DropEdge(mesh, e1, true);
        CallThis(FUN_6062c0, mesh, s);
        BYTE* p = CallThis<BYTE*>(FUN_606400, mesh, pt);
        BYTE *a, *b;
        Gs_Halves(mesh, s, p, &a, &b);
        F(BYTE*, a, 48) = b;
        F(BYTE*, b, 52) = a;
        F(BYTE*, a, 52) = F(BYTE*, s, 44);
        F(BYTE*, b, 48) = F(BYTE*, s, 44);
        F(BYTE*, a, 44) = F(BYTE*, s, 48);
        F(BYTE*, b, 44) = F(BYTE*, s, 52);
        Gs_Relink(F(BYTE*, s, 48), s, a, 52);
        Gs_Relink(F(BYTE*, s, 52), s, b, 48);
        BYTE* n0 = Gs_NewNode();
        BYTE* n1 = Gs_NewNode();
        F(BYTE*, n0, 12) = a;
        F(BYTE*, n1, 12) = b;
        F(int, n0, 16) = F(int, s, 36) + 1;
        F(int, n1, 16) = F(int, s, 36) + 1;
        BYTE* q = F(BYTE*, s, 32);
        F(BYTE*, q, 0) = n0;
        F(BYTE*, q, 4) = n1;
        F(int, q, 12) = 0;
        F(BYTE*, n0, 8) = q;
        F(BYTE*, n1, 8) = q;
        F(BYTE*, b, 32) = n1;
        F(BYTE*, a, 32) = n0;
        if (g_gsDepth7241b8 == 0) {
            CallThis(FUN_6064c0, mesh, F(BYTE*, s, 32), 0);
            if (s)
                CallVirt(s, 0, 1);
            return;
        }
        BYTE* top = Gs_Stack()[g_gsDepth7241b8];
        Gs_Push(F(BYTE*, s, 48) == top ? a : b);
        if (s)
            CallVirt(s, 0, 1);
    }
}

} // extern "C"

// === removing an edge from the grid mesh (FUN_606760)

extern "C" void __fastcall GridBranch_Destruct(BYTE* self, void*);     // grid_topology.cpp
extern "C" void FUN_606d50(); KING_ALIAS("_FUN_606d50", "@Grid_MergeFour@32")    // merges the four triangles around an inner edge (a0, a1, b0, b1, &out a, &out b)

// deletes the two branches of pair p and makes node its triangle
static void Gr_ResetPair(BYTE** p, BYTE* node)
{
    for (int k = 0; k < 2; k++)
        if (BYTE* b = p[k]) {
            GridBranch_Destruct(b, 0);
            operator delete(b);
        }
    p[1] = 0;
    p[0] = 0;
    F(BYTE*, p, 12) = node;
    F(BYTE**, node, 32) = p;
}

extern "C" {

// [FUN_606760] removes edge e from mesh m (+44 the cursor, +52 the count, +28 the queue): the triangles on its
// sides (+8, and +12 for an inner edge) are merged, their branches replaced by the merged triangles, and a new
// edge made across the first one when its neighbours allow; the second one is offered for joining
void __cdecl GridMesh_RemoveEdge(BYTE* e, BYTE* m)
{
    if (e == F(BYTE*, m, 44))
        F(int, m, 44) = Pair_Next(F(BYTE**, m, 28), 0, (int*)e);
    F(short, m, 52)--;
    BYTE** a = F(BYTE**, e, 8);
    BYTE** b = F(BYTE**, e, 12);
    F(int, F(BYTE*, a[0], 12), 56) = 0;
    F(int, F(BYTE*, a[1], 12), 56) = 0;
    if (b) {
        F(int, F(BYTE*, b[0], 12), 56) = 0;
        F(int, F(BYTE*, b[1], 12), 56) = 0;
    }
    CallThis(FUN_6136f0, F(BYTE*, m, 28), e);
    BYTE* r1 = 0;
    BYTE* r2;
    if (!b)
        Grid_MergeTriangles(m, 0, F(BYTE*, a[0], 12), F(BYTE*, a[1], 12), &r2);
    else
        CallThis(FUN_606d50, m, F(BYTE*, a[0], 12), F(BYTE*, a[1], 12), F(BYTE*, b[0], 12), F(BYTE*, b[1], 12), &r2,
                 &r1);
    Gr_ResetPair(F(BYTE**, e, 8), r2);
    if (F(BYTE**, e, 12))
        Gr_ResetPair(F(BYTE**, e, 12), r1);
    if (e) {
        CallThis(FUN_606750, e);
        operator delete(e);
    }
    BYTE* n = r2;
    if (!F(int, n, 56) && (F(BYTE*, n, 48) || F(BYTE*, n, 52))) {
        BYTE* n48 = F(BYTE*, n, 48);
        BYTE** p = F(BYTE**, F(BYTE*, n, 32), 8);
        BYTE* c0 = p[0];
        BYTE* c1 = p[1];
        if (c0 && c1) {
            BYTE* x = 0;
            bool make = false, check = false;
            if (F(BYTE*, c0, 12) == n) {
                BYTE* y = F(BYTE*, n, 52);
                if (!y && !F(BYTE*, n48, 48))
                    check = true;
                else if (y && F(BYTE*, y, 52)) {
                    x = F(BYTE*, F(BYTE*, y, 32), 8);
                    if (x == F(BYTE*, F(BYTE*, F(BYTE*, y, 52), 32), 8))
                        x ? make = true : check = true;
                }
            } else if (F(BYTE*, c1, 12) == n) {
                if (!n48 && !F(BYTE*, F(BYTE*, n, 52), 52))
                    check = true;
                else if (n48 && F(BYTE*, n48, 48)) {
                    x = F(BYTE*, F(BYTE*, n48, 32), 8);
                    if (x == F(BYTE*, F(BYTE*, F(BYTE*, n48, 48), 32), 8))
                        x ? make = true : check = true;
                }
            } else
                CallC(FUN_60aad0, 0x88760112, (const char*)s_fileLine, (const char*)s_6849fc, 276,
                      (const char*)s_buildDate);
            if (check) {
                x = 0;
                BYTE** q = F(BYTE**, (BYTE*)p, 8);
                if (q && (!q[0] || !q[1]))
                    make = true;
            }
            if (make)
                CallThis(FUN_6064c0, m, (BYTE*)p, x);
        }
    }
    if (r1)
        Grid_TryJoin(r1);
}

} // extern "C"

// === removing an edge from the grid tree (FUN_609de0)

extern "C" void FUN_60a1c0(); KING_ALIAS("_FUN_60a1c0", "@Grid_MergeCells@20")
extern "C" void FUN_60a460(); KING_ALIAS("_FUN_60a460", "@Grid_MergeFourCells@32")    // merges the four cells around an inner edge (a0, a1, b0, b1, &out a, &out b)
ASM_VAR(AsmLabel, s_gt684a44, $L_684a44)

// deletes the two branches of pair p and makes cell its node
static void Gt_ResetPair(BYTE** p, BYTE* cell)
{
    for (int k = 0; k < 2; k++)
        if (BYTE* b = p[k]) {
            Btr_Destroy(b, 0);
            operator delete(b);
        }
    p[1] = 0;
    p[0] = 0;
    F(BYTE*, p, 12) = cell;
    F(BYTE**, cell, 40) = p;
}

extern "C" {

// [FUN_609de0] removes edge e from tree grid g (+56 the cursor, +64 the count, +32 the queue): the cells on its
// sides (+8, and +12 for an inner edge) are merged, their branches replaced by the merged cells, and a new link
// made across the first one when its neighbours allow; the second one is joined
void __cdecl GridTree_RemoveEdge(BYTE* e, BYTE* g)
{
    if (e == F(BYTE*, g, 56))
        F(int, g, 56) = Pair_Next(F(BYTE**, g, 32), 0, (int*)e);
    F(short, g, 64)--;
    BYTE** a = F(BYTE**, e, 8);
    BYTE** b = F(BYTE**, e, 12);
    F(int, F(BYTE*, a[0], 12), 72) = 0;
    F(int, F(BYTE*, a[1], 12), 72) = 0;
    if (b) {
        F(int, F(BYTE*, b[0], 12), 72) = 0;
        F(int, F(BYTE*, b[1], 12), 72) = 0;
    }
    CallThis(FUN_6136f0, F(BYTE*, g, 32), e);
    BYTE* r1 = 0;
    BYTE* r2;
    if (!b)
        CallThis(FUN_60a1c0, g, F(BYTE*, a[0], 12), F(BYTE*, a[1], 12), &r2);
    else
        CallThis(FUN_60a460, g, F(BYTE*, a[0], 12), F(BYTE*, a[1], 12), F(BYTE*, b[0], 12), F(BYTE*, b[1], 12), &r2,
                 &r1);
    Gt_ResetPair(F(BYTE**, e, 8), r2);
    if (F(BYTE**, e, 12))
        Gt_ResetPair(F(BYTE**, e, 12), r1);
    if (e) {
        Grid_ResetVtables(e, 0);
        operator delete(e);
    }
    BYTE* n = r2;
    if (!F(int, n, 72) && (F(BYTE*, n, 64) || F(BYTE*, n, 68))) {
        BYTE* n64 = F(BYTE*, n, 64);
        BYTE** p = F(BYTE**, F(BYTE*, n, 40), 8);
        BYTE* c0 = p[0];
        BYTE* c1 = p[1];
        if (c0 && c1) {
            BYTE* x = 0;
            bool link = false;
            if (F(BYTE*, c0, 12) == n) {
                BYTE* y = F(BYTE*, n, 68);
                if (!y && !F(BYTE*, n64, 64))
                    link = true;
                else if (y && F(BYTE*, y, 68)) {
                    x = F(BYTE*, F(BYTE*, y, 40), 8);
                    link = x == F(BYTE*, F(BYTE*, F(BYTE*, y, 68), 40), 8);
                }
            } else if (F(BYTE*, c1, 12) == n) {
                if (!n64 && !F(BYTE*, F(BYTE*, n, 68), 68))
                    link = true;
                else if (n64 && F(BYTE*, n64, 64)) {
                    x = F(BYTE*, F(BYTE*, n64, 40), 8);
                    link = x == F(BYTE*, F(BYTE*, F(BYTE*, n64, 64), 40), 8);
                }
            } else
                CallC(FUN_60aad0, 0x88760112, (const char*)s_fileLine, (const char*)s_gt684a44, 372,
                      (const char*)s_buildDate);
            if (link)
                CallStd(FUN_608fa0, (BYTE*)p, x);
        }
    }
    if (r1)
        Grd_JoinBranches(r1);
}

} // extern "C"

// === merging four triangles into two (FUN_606d50)

// removes triangle a from the mesh's ring list, moving the cursor (+40) past it
static void Gm_Unlink(BYTE* self, BYTE* a)
{
    if (a == F(BYTE*, self, 40))
        F(BYTE*, self, 40) = Ring_Next(F(BYTE**, self, 32), a);
    F(WORD, self, 38)--;
    CallThis(FUN_60f690, F(BYTE*, self, 32), a);
}

extern "C" {

// [FUN_606d50] replaces the triangles a, b (sharing the point a+20) and c, d by two new ones *n1 (a's corner to
// b's) and *n2 (c's to d's), facing each other; the shared point is dropped
void __fastcall Grid_MergeFour(BYTE* self, void*, BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE** n1, BYTE** n2)
{
    Gm_Unlink(self, a);
    Gm_Unlink(self, b);
    Gm_Unlink(self, c);
    Gm_Unlink(self, d);
    BYTE* x = CallThis<BYTE*>(FUN_606330, self, F(int, a, 24), F(int, a, 16), F(int, b, 16), F(int, a, 36) - 1);
    BYTE* y = CallThis<BYTE*>(FUN_606330, self, F(int, c, 24), F(int, c, 16), F(int, d, 16), F(int, a, 36) - 1);
    Relink3(F(BYTE*, a, 44), a, x);
    Relink3(F(BYTE*, b, 44), b, x);
    Relink3(F(BYTE*, c, 44), c, y);
    Relink3(F(BYTE*, d, 44), d, y);
    F(BYTE*, x, 44) = y;
    F(BYTE*, x, 52) = F(BYTE*, b, 44);
    F(BYTE*, x, 48) = F(BYTE*, a, 44);
    F(BYTE*, y, 48) = F(BYTE*, c, 44);
    F(BYTE*, y, 52) = F(BYTE*, d, 44);
    F(BYTE*, y, 44) = x;
    BYTE* t = F(BYTE*, a, 20);
    WORD k = F(WORD, t, 8);
    F(WORD, self, 36)--;
    WORD last = F(WORD, self, 36);
    BYTE** arr = F(BYTE**, self, 12);
    if (k != last) {
        arr[k] = arr[last];
        F(WORD, F(BYTE**, self, 12)[k], 8) = k;
    }
    F(BYTE**, self, 12)[F(WORD, self, 36)] = 0;
    if (t)
        operator delete(t);
    *n1 = x;
    *n2 = y;
    if (a)
        CallVirt(a, 0, 1);
    if (b)
        CallVirt(b, 0, 1);
    if (c)
        CallVirt(c, 0, 1);
    if (d)
        CallVirt(d, 0, 1);
}

} // extern "C"

// === refreshing the grid's work priorities (FUN_60be40)

extern "C" void FUN_6072b0(); KING_ALIAS("_FUN_6072b0", "_Grid_DeepestLevel")
extern "C" void FUN_608460(); KING_ALIAS("_FUN_608460", "@Point_InWedge@32")
extern "C" void FUN_6097d0(); KING_ALIAS("_FUN_6097d0", "@GridCamera_NodePriority@12")
extern "C" void FUN_60c120(); KING_ALIAS("_FUN_60c120", "@Circle_Outside@16")
extern "C" void FUN_613310(); KING_ALIAS("_FUN_613310", "@MarineQueue_SetDeferredPriority@16")
extern "C" void FUN_6135f0(); KING_ALIAS("_FUN_6135f0", "@MarineQueue_SetWorkPriority@16")
ASM_VAR(float, g_gp652efc, $L_652efc)
ASM_VAR(float, g_gp652ef8, $L_652ef8)
ASM_VAR(float, g_gp64c448, $L_64c448)
ASM_VAR(float, g_gp64c450, $L_64c450)

// the first item of ring list l
static BYTE* Gp_Head(BYTE** l) { return l[0] ? l[0] : l[1]; }

// the item after x in the work ring list l (linked at +0, 0 at the end)
static BYTE* Gp_WorkNext(BYTE** l, BYTE* x)
{
    if (l[0] && x == F(BYTE*, l[0], 4))
        return l[1];
    if (l[1] && x == F(BYTE*, l[1], 4))
        return 0;
    return F(BYTE*, x, 0);
}

extern "C" {

// [FUN_60be40] mesh s refreshes the priorities of 400 split candidates (+36 ring, cursor +52: by how far and
// where they lie from the camera +4, 1000 when it is inside) and of 200 work items (+32 ring, cursor +56: by the
// camera's priority of their nodes +16 and +60)
void __fastcall Grid_RefreshPriorities(BYTE* s)
{
    if (F(BYTE*, s, 52) || (F(BYTE*, s, 52) = Gp_Head(F(BYTE**, s, 36))) != 0)
        for (int k = 0; k < 400; k++) {
            BYTE* c = F(BYTE*, s, 52);
            BYTE* n = c + 12;
            BYTE* cam = F(BYTE*, s, 4);
            float pri = 0;
            if ((unsigned)F(int, n, 24) < (unsigned)CallThis<int>(FUN_6072b0, cam)) {
                float ctr[3];
                memcpy(ctr, n + 32, 12);
                float d = (float)CallThis<double>(FUN_60c120, ctr, F(int, cam, 4), F(int, cam, 8));
                if (!(d > g_gp652efc)) {
                    if (d < g_gp64c448)
                        pri = 1000.0f;
                    else if (CallThis<int>(FUN_608460, ctr, F(int, cam, 4), F(int, cam, 8), F(int, cam, 60),
                                           F(int, cam, 64), F(int, cam, 68), F(int, cam, 72))) {
                        if ((unsigned)F(int, n, 24) < 8)
                            pri = (float)(g_gp64c450 / ((double)d + g_q47_64c250));
                        else if (d < g_gp652ef8 ||
                                 CallThis<int>(FUN_608460, ctr, F(int, cam, 4), F(int, cam, 8), F(int, cam, 76),
                                               F(int, cam, 80), F(int, cam, 84), F(int, cam, 88)))
                            pri = (float)(F(float, n, 20) / (double)d);
                        else
                            pri = (float)(F(float, n, 20) * (double)g_gsp64c258 / d);
                    }
                }
            }
            BYTE** l = F(BYTE**, s, 36);
            c = F(BYTE*, s, 52);
            BYTE* next = Ring_Next(l, c);
            if (F(float, c, 28) != pri)
                CallThis(FUN_613310, l, c, pri);
            F(BYTE*, s, 52) = next ? next : Gp_Head(F(BYTE**, s, 36));
        }
    if (!F(BYTE*, s, 56) && !(F(BYTE*, s, 56) = Gp_Head(F(BYTE**, s, 32))))
        return;
    for (int k = 0; k < 200; k++) {
        BYTE* c = F(BYTE*, s, 56);
        BYTE* cam = F(BYTE*, s, 4);
        double pri;
        if (F(int, c, 12)) {
            float p1 = (float)CallThis<double>(FUN_6097d0, cam, c + 16);
            double p2 = c + 60 ? CallThis<double>(FUN_6097d0, cam, c + 60) : 0.0;
            pri = p1 > p2 ? p1 : p2;
        } else {
            pri = CallThis<double>(FUN_6097d0, F(BYTE*, s, 4), c + 16);
            if (!(pri > g_q47_64c264))
                pri = g_q47_64c264;
        }
        float pf = (float)pri;
        BYTE** l = F(BYTE**, s, 32);
        c = F(BYTE*, s, 56);
        BYTE* next = Gp_WorkNext(l, c);
        if (F(float, c, 104) != pri)
            CallThis(FUN_6135f0, l, c, pf);
        F(BYTE*, s, 56) = next ? next : Gp_Head(F(BYTE**, s, 32));
    }
}

} // extern "C"

// === adding a grid point with its transition (FUN_608b40)

extern "C" void Ga_MakeNode(); KING_ALIAS("_Ga_MakeNode", "@TGrid_MakeNode@16")
extern "C" void Ga_Snapshot(); KING_ALIAS("_Ga_Snapshot", "@MarineGeometry_Snapshot@16")
extern "C" void Ga_TransitionCreate(); KING_ALIAS("_Ga_TransitionCreate", "@MarineTransition_Create@32")

extern "C" {

// [FUN_608b40] adds point xy to mesh s: its 40-byte node (from the terrain +4) at +16, its entry (+12: the 2-byte
// index, xy) and a transition (104 bytes: a, b) linked into the ring +44; the index
WORD* __fastcall Grid_AddPoint(BYTE* s, void*, const DWORD* xy, int a, int b)
{
    BYTE node[40];
    BYTE* n = CallThis<BYTE*>(Ga_MakeNode, F(BYTE*, s, 4), node, xy);
    memcpy(F(BYTE*, s, 16) + F(WORD, s, 40) * 40, n, 40);
    WORD k = F(WORD, s, 40);
    BYTE* e = F(BYTE*, s, 12) + k * 16;
    WORD* p = (WORD*)operator new(2);
    if (p)
        *p = k;
    F(WORD*, e, 0) = p;
    F(DWORD, e, 8) = xy[0];
    F(DWORD, e, 12) = xy[1];
    F(int, e, 4) = 0;
    F(WORD, s, 40)++;
    e = F(BYTE*, s, 12) + *p * 16;
    BYTE* t = (BYTE*)operator new(104);
    if (t) {
        BYTE snap[40];
        int sn = CallThis<int>(Ga_Snapshot, s, snap, p);
        t = CallThis<BYTE*>(Ga_TransitionCreate, t, F(int, s, 48), e, a, sn, b, s);
    }
    BYTE* r = F(BYTE*, s, 44);
    if (!r)
        F(BYTE*, s, 44) = t;
    else {
        F(BYTE*, t, 0) = r;
        F(BYTE*, t, 4) = F(BYTE*, r, 4);
        F(BYTE*, F(BYTE*, r, 4), 0) = t;
        F(BYTE*, r, 4) = t;
    }
    return p;
}

} // extern "C"

// === merging four cells into two (FUN_60a460)

// removes cell a from the mesh's ring list (+36, count +42), moving the cursor (+52) past it
static void Gc_Unlink(BYTE* self, BYTE* a)
{
    if (a == F(BYTE*, self, 52))
        F(BYTE*, self, 52) = Ring_Next(F(BYTE**, self, 36), a);
    F(WORD, self, 42)--;
    CallThis(FUN_60f690, F(BYTE*, self, 36), a);
}

// repoints the neighbour link of x that names from to to (+60, +68, else +64)
static void Gc_Relink(BYTE* x, BYTE* from, BYTE* to)
{
    if (!x)
        return;
    if (F(BYTE*, x, 60) == from)
        F(BYTE*, x, 60) = to;
    else if (F(BYTE*, x, 68) == from)
        F(BYTE*, x, 68) = to;
    else
        F(BYTE*, x, 64) = to;
}

extern "C" {

// [FUN_60a460] replaces the cells a, b (sharing the point a+20) and c, d by two new ones *n1 (a's corner to b's) and
// *n2 (c's to d's), facing each other; the shared point is removed
void __fastcall Grid_MergeFourCells(BYTE* self, void*, BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE** n1, BYTE** n2)
{
    Gc_Unlink(self, a);
    Gc_Unlink(self, b);
    Gc_Unlink(self, c);
    Gc_Unlink(self, d);
    BYTE* x = CallThis<BYTE*>(FUN_608530, self, F(int, a, 24), F(int, a, 16), F(int, b, 16), F(int, a, 36) - 1);
    BYTE* y = CallThis<BYTE*>(FUN_608530, self, F(int, c, 24), F(int, c, 16), F(int, d, 16), F(int, a, 36) - 1);
    Gc_Relink(F(BYTE*, a, 60), a, x);
    Gc_Relink(F(BYTE*, b, 60), b, x);
    Gc_Relink(F(BYTE*, c, 60), c, y);
    Gc_Relink(F(BYTE*, d, 60), d, y);
    F(BYTE*, x, 60) = y;
    F(BYTE*, x, 68) = F(BYTE*, b, 60);
    F(BYTE*, x, 64) = F(BYTE*, a, 60);
    F(BYTE*, y, 64) = F(BYTE*, c, 60);
    F(BYTE*, y, 68) = F(BYTE*, d, 60);
    F(BYTE*, y, 60) = x;
    BYTE* t = F(BYTE*, a, 20);
    WORD k = F(WORD, t, 0);
    F(WORD, self, 40)--;
    Grid_RemovePoint(self, k);
    if (t)
        operator delete(t);
    *n1 = x;
    *n2 = y;
    if (b)
        CallVirt(b, 0, 1);
    if (c)
        CallVirt(c, 0, 1);
    if (d)
        CallVirt(d, 0, 1);
    if (a)
        CallVirt(a, 0, 1);
}

} // extern "C"

// === emptying a mesh (FUN_60bbb0)

ASM_VAR(AsmLabel, g_ms652f3c, $L_652f3c)
extern "C" void __cdecl free(void* p);

extern "C" {

// [FUN_60bbb0] destructor body of mesh s: every edge of the ring +32 removed and deleted, every cell of the ring
// +36 unlinked and deleted (vtbl+0), the buffer +12 freed, the point slots +8 (count +60) unhooked from their
// parents (reporting a broken link) and deleted with their subtrees; the counts cleared
void __fastcall Mesh_Clear(BYTE* s)
{
    F(void*, s, 0) = g_ms652f3c;
    for (;;) {
        BYTE** q = F(BYTE**, s, 32);
        if (!q[0] && !q[1])
            break;
        BYTE* e = q[0] ? q[0] : q[1];
        Grid_RemoveEdge(s, 0, e);
        if (e) {
            Grid_ResetVtables(e, 0);
            operator delete(e);
        }
    }
    for (;;) {
        BYTE** r = F(BYTE**, s, 36);
        if (!r[0] && !r[1])
            break;
        BYTE* c = r[0] ? r[0] : r[1];
        Gc_Unlink(s, c);
        if (c)
            CallVirt(c, 0, 1);
    }
    if (F(void*, s, 12)) {
        free(F(void*, s, 12));
        F(void*, s, 12) = 0;
    }
    if (BYTE** slots = F(BYTE**, s, 8)) {
        for (unsigned i = 0; i < F(unsigned, s, 60); i++) {
            BYTE* n = F(BYTE**, s, 8)[i];
            if (!n)
                continue;
            if (BYTE* p = F(BYTE*, n, 8)) {
                if (F(BYTE*, p, 0) == n)
                    F(BYTE*, p, 0) = 0;
                else if (F(BYTE*, p, 4) == n)
                    F(BYTE*, p, 4) = 0;
                else
                    CallC(FUN_60aad0, 0x88760112, (const char*)s_fileLine, (const char*)s_btrFile, 35,
                        (const char*)s_buildDate);
            }
            if (F(BYTE*, n, 0))
                Btr_Delete(F(BYTE*, n, 0), 0, 1);
            if (F(BYTE*, n, 4))
                Btr_Delete(F(BYTE*, n, 4), 0, 1);
            operator delete(n);
            F(BYTE**, s, 8)[i] = 0;
        }
        if (F(void*, s, 8)) {
            operator delete(F(void*, s, 8));
            F(void*, s, 8) = 0;
        }
    }
    F(WORD, s, 42) = 0;
    F(WORD, s, 40) = 0;
    F(int, s, 60) = 0;
}

} // extern "C"
