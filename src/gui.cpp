// gui.cpp - the engine's window controls (unit at 0x5bc2f0 of king.masm):
// child lists, selection, simple layouts and the control constructors.
//
// A control: vtbl, +16/+20 size, +28 id, +32 flags, +36, +40 hidden,
// +44..+56 rectangle, +60 spacing, +284 flags (bit 0 selected),
// +288 first child, +292 selected child, +296. Children are linked through
// +4 (previous) and +8 (next); [FUN_5d3080] returns the next one.
//
// Building with  nmake ASM_GUI=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include <math.h>

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

ASM_PROC(FUN_5d3080)    // next sibling
ASM_PROC(FUN_5bc430)    // distance of a point to segment i, returns double
ASM_PROC(FUN_5bdc80)    // relayout
ASM_PROC(FUN_5bd5e0)    // base control constructor (a, flags)

ASM_VAR(int, g_702f24, $L_702f24)
ASM_VAR(int, g_702f28, $L_702f28)
ASM_VAR(int, g_702f40, $L_702f40)
ASM_VAR(int, g_702f44, $L_702f44)
typedef void(__cdecl* SelectHook)(BYTE* self, BYTE* child);
ASM_VAR(SelectHook, g_selectHook, $L_702f48)
ASM_VAR(int, g_720904, $L_720904)
ASM_VAR(AsmLabel, g_vtbl651838, $L_651838)
ASM_VAR(AsmLabel, g_vtbl65186c, $L_65186c)

static __forceinline BYTE* Next(BYTE* c)
{
    return CallThis<BYTE*>(FUN_5d3080, c);
}

extern "C" {

// [FUN_5bc2f0]
void __fastcall Poly_Destroy(BYTE* self, void*)
{
    operator delete(F(void*, self, 28));
    operator delete(F(void*, self, 12));
    BYTE* o = F(BYTE*, self, 36);
    if (o && g_720904)
        CallVirt(o, 8);
}

// [FUN_5bc320] copies the 48-byte record i out
void __fastcall Poly_GetRecord(BYTE* self, void*, int i, DWORD* out)
{
    if (i < F(int, self, 4) && i >= 0) {
        const DWORD* s = (DWORD*)(F(BYTE*, self, 8) + i * 48);
        for (int k = 0; k < 12; k++)
            out[k] = s[k];
    }
}

// [FUN_5bc350]
void __fastcall Poly_SetWeight(BYTE* self, void*, int i, double v)
{
    if (i < F(int, self, 4) && i >= 0)
        F(float*, self, 12)[i] = (float)v;
}

// [FUN_5bc370] the segment nearest to p: its distance in *dist, the nearest
// point in *at
int __fastcall Poly_Nearest(BYTE* self, void*, BYTE* p, double* dist, DWORD* at)
{
    dist[0] = 1000000.0;
    if (at) {
        at[0] = F(DWORD, p, 36);
        at[1] = F(DWORD, p, 40);
        at[2] = F(DWORD, p, 44);
    }
    int best = 0;
    for (int i = 0; i < F(int, self, 4) - 1; i++) {
        int idx = i;
        DWORD pt[3];
        double d = CallThis<double>(FUN_5bc430, self, p, &idx, pt);
        if (fabs(*dist) > fabs(d)) {
            *dist = d;
            best = idx;
            if (at) {
                at[0] = pt[0];
                at[1] = pt[1];
                at[2] = pt[2];
            }
        }
    }
    return best;
}

// [FUN_5bcfe0]
BYTE* __fastcall Ctl_Owner(BYTE* self, void*)
{
    BYTE* p = F(BYTE*, self, 24);
    return p ? p - 16 : 0;
}

// [FUN_5bd590]
void __cdecl Gui_Reset702f24()
{
    g_702f24 = 0;
}

// [FUN_5bd5a0] copies a 36-byte style
DWORD __fastcall Style_Assign(DWORD* self, void*, const DWORD* s)
{
    for (int k = 0; k < 9; k++)
        self[k] = s[k];
    return s[5];
}

// [FUN_5bdb50] the descendant with this id
BYTE* __fastcall Ctl_FindId(BYTE* self, void*, int id)
{
    for (BYTE* c = F(BYTE*, self, 288); c; c = Next(c)) {
        if (CallVirt<int>(c, 44)) {
            BYTE* r = Ctl_FindId(c, 0, id);
            if (r)
                return r;
        } else if (id == F(int, c, 28)) {
            return c;
        }
    }
    return 0;
}

// [FUN_5bdba0]
void __fastcall Ctl_Select(BYTE* self, void*, BYTE* child)
{
    if (g_selectHook && child != F(BYTE*, self, 292))
        g_selectHook(self, child);
    BYTE* old = F(BYTE*, self, 292);
    if (old)
        F(DWORD, old, 284) &= ~1u;
    F(BYTE*, self, 292) = child;
    if (child)
        F(DWORD, child, 284) |= 1;
}

// [FUN_5bdbf0] inserts child after `after` (first if 0, last if not found)
void __fastcall Ctl_AddChild(BYTE* self, void*, BYTE* child, BYTE* after)
{
    BYTE* head = F(BYTE*, self, 288);
    if (!head) {
        F(BYTE*, self, 288) = child;
    } else if (!after) {
        F(BYTE*, child, 8) = head;
        F(BYTE*, head, 4) = child;
        F(BYTE*, self, 288) = child;
    } else {
        BYTE* a = head;
        while (F(BYTE*, a, 8) && a != after)
            a = Next(a);
        F(BYTE*, child, 8) = F(BYTE*, a, 8);
        F(BYTE*, F(BYTE*, a, 8), 4) = child;
        F(BYTE*, a, 8) = child;
        F(BYTE*, child, 4) = a;
    }
    if (CallVirt<int>(child, 48))
        Ctl_Select(self, 0, child);
}

// [FUN_5bdee0] stacks the children vertically
void __fastcall Ctl_LayoutColumn(BYTE* self, void*, int gap)
{
    if (F(DWORD, self, 32) & 0x300)
        F(int, self, 60) = 1;
    int w = 0;
    for (BYTE* c = F(BYTE*, self, 288); c; c = Next(c)) {
        int cw = F(int, c, 56) - F(int, c, 48);
        if (w < cw)
            w = cw;
    }
    int y = F(int, self, 60);
    for (BYTE* c = F(BYTE*, self, 288); c; c = Next(c)) {
        int m = F(int, self, 60);
        F(int, c, 44) += y;
        F(int, c, 52) += y;
        F(int, c, 48) += m;
        F(int, c, 56) += m;
        F(int, c, 56) = F(int, self, 60) + w;
        y += F(int, c, 52) - F(int, c, 44) + gap;
    }
    CallThis(FUN_5bdc80, self);
}

// [FUN_5be130] puts the children side by side
void __fastcall Ctl_LayoutRow(BYTE* self, void*, int gap)
{
    if (F(DWORD, self, 32) & 0x300)
        F(int, self, 60) = 1;
    int h = 0;
    for (BYTE* c = F(BYTE*, self, 288); c; c = Next(c)) {
        int ch = F(int, c, 52) - F(int, c, 44);
        if (h < ch)
            h = ch;
    }
    int x = F(int, self, 60);
    for (BYTE* c = F(BYTE*, self, 288); c; c = Next(c)) {
        int m = F(int, self, 60);
        F(int, c, 44) += m;
        F(int, c, 52) += m;
        F(int, c, 48) += x;
        F(int, c, 56) += x;
        F(int, c, 52) = F(int, self, 60) + h;
        x += F(int, c, 56) - F(int, c, 48) + gap;
    }
    CallThis(FUN_5bdc80, self);
}

// [FUN_5be5a0] the first visible child that handles (a, b)
BYTE* __fastcall Ctl_Dispatch(BYTE* self, void*, int a, int b)
{
    if (F(int, self, 40))
        return 0;
    BYTE* c = F(BYTE*, self, 288);
    for (; c; c = Next(c))
        if (!F(int, c, 40) && CallVirt<int>(c, 24, a, b))
            break;
    return c;
}

// [FUN_5bef30] the four styles of child n
void __fastcall Ctl_SetStyles(BYTE* self, void*, const DWORD* a, const DWORD* b, const DWORD* c, const DWORD* d,
                              int n)
{
    BYTE* ch = F(BYTE*, self, 288);
    for (; n > 0; n--)
        ch = Next(ch);
    if (ch) {
        Style_Assign((DWORD*)(ch + 68), 0, c);
        Style_Assign((DWORD*)(ch + 104), 0, a);
        Style_Assign((DWORD*)(ch + 140), 0, d);
        Style_Assign((DWORD*)(ch + 176), 0, b);
    }
}

// [FUN_5bef90]
void __fastcall Ctl_SetStyle5(BYTE* self, void*, const DWORD* s, int n)
{
    BYTE* ch = F(BYTE*, self, 288);
    for (; n > 0; n--)
        ch = Next(ch);
    if (ch)
        Style_Assign((DWORD*)(ch + 212), 0, s);
}

// [FUN_5bf750]
BYTE* __fastcall Button_Construct(BYTE* self, void*, int a, int flags)
{
    CallThis(FUN_5bd5e0, self, a, flags);
    F(void*, self, 0) = g_vtbl651838;
    if (flags & 2)
        F(DWORD, self, 32) |= 0x100;
    else if (flags & 1)
        F(DWORD, self, 32) |= 0x200;
    F(int, self, 28) = -1;
    return self;
}

// [FUN_5bf820]
BYTE* __fastcall Button_ConstructId(BYTE* self, void*, int a, int flags, int id)
{
    CallThis(FUN_5bd5e0, self, a, flags);
    F(void*, self, 0) = g_vtbl651838;
    if (flags & 2)
        F(DWORD, self, 32) |= 0x100;
    else if (flags & 1)
        F(DWORD, self, 32) |= 0x200;
    F(int, self, 28) = id;
    return self;
}

// [FUN_5bfd60]
void __fastcall Panel_Clear(BYTE* self, void*)
{
    F(int, self, 292) = 0;
    F(int, self, 36) = 0;
    g_702f28 = 0;
}

// [FUN_5bfd80]
BYTE* __fastcall Panel_Construct(BYTE* self, void*, int a, int b, int w, int c, int h)
{
    CallThis(FUN_5bd5e0, self, a, b);
    F(void*, self, 0) = g_vtbl65186c;
    F(int, self, 288) = 0;
    F(int, self, 292) = 0;
    g_702f40 = w;
    F(int, self, 16) = w;
    int v = h ? h : w;
    g_702f44 = v;
    F(int, self, 20) = v;
    F(int, self, 296) = c;
    return self;
}

// [FUN_5bfde0]
BYTE* __fastcall Panel_ConstructRect(BYTE* self, void*, int a, int b, int w, int c, int x0, int y0, int x1,
                                     int y1, int h)
{
    CallThis(FUN_5bd5e0, self, a, b);
    F(void*, self, 0) = g_vtbl65186c;
    F(int, self, 288) = 0;
    F(int, self, 292) = 0;
    g_702f40 = w;
    F(int, self, 16) = w;
    int v = h ? h : w;
    g_702f44 = v;
    F(int, self, 20) = v;
    F(int, self, 296) = c;
    F(int, self, 44) = x0;
    F(int, self, 52) = x1;
    F(int, self, 56) = y1;
    F(int, self, 48) = y0;
    return self;
}

}
