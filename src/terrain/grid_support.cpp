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
ASM_PROC(FUN_60ab70)
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
ASM_PROC(FUN_609de0)

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
ASM_PROC(FUN_60bbb0)

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
