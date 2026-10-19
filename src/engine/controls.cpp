// controls.cpp - the engine's window controls (unit at 0x5bc2f0 of king.masm):
// child lists, selection, simple layouts and the control constructors.
//
// A control: vtbl, +16/+20 size, +28 id, +32 flags, +36, +40 hidden,
// +44..+56 rectangle, +60 spacing, +284 flags (bit 0 selected),
// +288 first child, +292 selected child, +296. Children are linked through
// +4 (previous) and +8 (next); [FUN_5d3080] returns the next one.
//
// Building with  nmake ASM_CONTROLS=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include <math.h>

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

extern "C" void FUN_5d3080(); KING_ALIAS("_FUN_5d3080", "@Node_Next@8")    // next sibling
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

ASM_PROC(FUN_5b6f90)    // fills a rectangle
ASM_PROC(FUN_5b8980)    // draws a pattern
ASM_PROC(FUN_5b9020)    // tiles a bitmap
ASM_PROC(FUN_5b7f20)    // vertical line
ASM_PROC(FUN_5b7ce0)    // horizontal line
extern "C" int __fastcall Font_TextWidth(BYTE* self, void*, const char* s);   // framework.cpp
extern "C" int __fastcall Window_Set80(BYTE* self, void*, int v);               // renderer.cpp
extern "C" void __cdecl Bitmap_Draw(BYTE* dst, int x, int y, BYTE* bmp);       // renderer.cpp

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

extern "C" {

// [FUN_5bd710] draws a control's face (by flags) and its border
// (the style is passed by value: color, -, light, dark, dx, dy, fill, bitmap, pattern)
void __cdecl Ctl_DrawFrame(BYTE* dst, const int* r, DWORD flags, int color, int a5, int light, int dark,
                           int dx, int dy, int fill, BYTE* bmp, int pattern)
{
    int old;
    if (flags & 2) {
        CallC(FUN_5b6f90, dst, r, color, 0);
    } else if (flags & 4) {
        Bitmap_Draw(dst, r[0] + dx, r[1] + dy, bmp);
    } else if (flags & 8) {
        CallC(FUN_5b8980, dst, r[0] + dx, r[1] + dy, pattern, 0);
    } else if (flags & 32) {
        old = Window_Set80(dst, 0, 2);
        F(int, dst, 88) = fill;
        CallC(FUN_5b9020, dst, r, bmp);
        Window_Set80(dst, 0, old);
    } else if (flags & 64) {
        old = Window_Set80(dst, 0, 2);
        F(int, dst, 88) = fill;
        CallC(FUN_5b8980, dst, r[0], r[1], pattern, 0);
        Window_Set80(dst, 0, old);
    } else if (flags & 1) {
        old = Window_Set80(dst, 0, 2);
        F(int, dst, 88) = fill;
        CallC(FUN_5b6f90, dst, r, color, 0);
        Window_Set80(dst, 0, old);
    } else if (flags & 16) {
        old = Window_Set80(dst, 0, 3);
        F(BYTE, dst, 92) = (BYTE)color;
        Bitmap_Draw(dst, r[0], r[1], bmp);
        Window_Set80(dst, 0, old);
    }
    if (flags & 0x100) {
        CallC(FUN_5b7f20, dst, r[0], r[1], r[3], light, 0);
        CallC(FUN_5b7ce0, dst, r[3], r[0], r[2], dark, 0);
        CallC(FUN_5b7f20, dst, r[2], r[3], r[1], dark, 0);
        CallC(FUN_5b7ce0, dst, r[1], r[2], r[0], light, 0);
    } else if (flags & 0x200) {
        old = Window_Set80(dst, 0, 2);
        F(int, dst, 88) = fill;
        CallC(FUN_5b7f20, dst, r[0], r[1], r[3], light, 0);
        CallC(FUN_5b7ce0, dst, r[3], r[0], r[2], dark, 0);
        CallC(FUN_5b7f20, dst, r[2], r[3], r[1], dark, 0);
        CallC(FUN_5b7ce0, dst, r[1], r[2], r[0], light, 0);
        Window_Set80(dst, 0, old);
    }
}

// [FUN_5bdd70] sizes the control to its picture or text, makes the
// children's rectangles relative
void __fastcall Ctl_AutoSize(BYTE* self, void*)
{
    DWORD f = F(DWORD, self, 32);
    BYTE* img = 0;
    int useImg = 0;
    if (f & 16) {
        img = F(BYTE*, self, 96) ? F(BYTE*, self, 96) : F(BYTE*, self, 132);
        useImg = img != 0;
    } else if (f & 36) {
        img = F(BYTE*, self, 96) ? F(BYTE*, self, 96) : F(BYTE*, self, 132);
        useImg = img != 0;
    } else if (!(f & 72)) {
        BYTE* font = F(BYTE*, self, 16);
        F(int, self, 52) = Font_TextWidth(font, 0, F(const char*, self, 12)) + F(int, self, 44) + 4;
        F(int, self, 56) = F(int, F(BYTE*, self, 16), 20) + F(int, self, 48) + 4;
    } else {
        BYTE* p = F(BYTE*, self, 100) ? F(BYTE*, self, 100) : F(BYTE*, self, 136);
        if (p) {
            F(int, self, 52) = F(WORD, p, 4) + F(int, self, 44) - 1;
            F(int, self, 56) = F(WORD, p, 6) + F(int, self, 48) - 1;
        }
    }
    if (useImg) {
        F(int, self, 52) = F(WORD, img, 20) + F(int, self, 44) - 1;
        F(int, self, 56) = F(WORD, img, 22) + F(int, self, 48) - 1;
    }
    if (F(int, self, 52) < F(int, self, 44))
        F(int, self, 52) = F(int, self, 44);
    if (F(int, self, 56) < F(int, self, 48))
        F(int, self, 56) = F(int, self, 48);
    for (BYTE* c = F(BYTE*, self, 288); c; c = Next(c)) {
        int x = F(int, self, 44), y = F(int, self, 48);
        F(int, c, 44) -= x;
        F(int, c, 52) -= x;
        F(int, c, 48) -= y;
        F(int, c, 56) -= y;
    }
    if (F(DWORD, self, 32) & 0x300)
        F(int, self, 60) = 1;
}

// [FUN_5becd0] resizes a two-part control (+328: 1 horizontal) to d
void __fastcall Split_Resize(BYTE* self, void*, int d)
{
    if (F(int, self, 328) == 1) {
        F(int, self, 308) += F(int, self, 44) - F(int, self, 52) + d;
        F(int, self, 52) = F(int, self, 44) + d;
    } else {
        F(int, self, 308) += F(int, self, 48) - F(int, self, 56) + d;
        F(int, self, 56) = F(int, self, 48) + d;
    }
    BYTE* c = Next(Next(F(BYTE*, self, 288)));
    if (F(int, self, 328) == 1) {
        int n = F(int, c, 44) - F(int, c, 52) + d;
        int s = n - F(int, c, 44);
        F(int, c, 44) = n;
        F(int, c, 52) += s;
    } else {
        int n = F(int, c, 48) - F(int, c, 56) + d;
        int s = n - F(int, c, 48);
        F(int, c, 48) = n;
        F(int, c, 56) += s;
    }
}

}

// ---------------------------------------------------------------------------
ASM_VAR(int, g_702f34, $L_702f34)
ASM_VAR(int, g_702f38, $L_702f38)
ASM_VAR(int, g_702f2c, $L_702f2c)
ASM_VAR(int, g_702f30, $L_702f30)

// [FUN_5bd560] sets 702f24 and the offsets 702f2c/30 from the sizes
extern "C" void __cdecl Ctl_SetOrigin(int a, int b, int c)
{
    g_702f2c = g_702f34 - b;
    g_702f24 = a;
    g_702f30 = g_702f38 - c;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5bcfe0(); KING_ALIAS("_FUN_5bcfe0", "@Ctl_Owner@8")
ASM_PROC(FUN_5bcff0)
extern "C" void FUN_5d2190(); KING_ALIAS("_FUN_5d2190", "@Node_Link@12")

// [FUN_5bd260] adds a control to the chain *this: 5bcff0 with every member, then links it after the last
extern "C" void __fastcall Chain_AddControl(BYTE** self, void*, BYTE* c)
{
    BYTE* p = *self;
    if (!p)
    {
        *self = c;
        return;
    }
    if (CallThis<BYTE*>(FUN_5bcfe0, p))
    {
        do
        {
            CallThis(FUN_5bcff0, p, c);
            p = CallThis<BYTE*>(FUN_5bcfe0, p);
        } while (CallThis<BYTE*>(FUN_5bcfe0, p));
    }
    CallThis(FUN_5bcff0, p, c);
    CallThis(FUN_5d2190, c + 16, p ? p + 16 : 0);
}

// [FUN_5bda20] whether (x, y) is inside [+44, +52] x [+48, +56]
extern "C" int __fastcall Ctl_Contains(BYTE* self, void*, int x, int y)
{
    CallVirt<int>(self, 44);
    if (x < F(int, self, 44) || x > F(int, self, 52))
        return 0;
    if (y < F(int, self, 48) || y > F(int, self, 56))
        return 0;
    return 1;
}


// ---------------------------------------------------------------------------
ASM_PROC(FUN_5bd940)
extern "C" void FUN_5be5a0(); KING_ALIAS("_FUN_5be5a0", "@Ctl_Dispatch@16")
extern "C" void FUN_5bdba0(); KING_ALIAS("_FUN_5bdba0", "@Ctl_Select@12")
extern "C" void FUN_5d3080(); KING_ALIAS("_FUN_5d3080", "@Node_Next@8")
ASM_VAR(int, g_ct67f3d8, $L_67f3d8)
ASM_VAR(int, g_ct702f24, $L_702f24)
ASM_VAR(BYTE*, g_ct702f3c, $L_702f3c)
typedef void (__cdecl* CtHook)(BYTE*, BYTE*);
ASM_VAR(CtHook, g_ct702f4c, $L_702f4c)

// the first child (from +288) that holds the input (+36)
static BYTE* Ct_Captured(BYTE* self)
{
    for (BYTE* c = F(BYTE*, self, 288); c; c = CallThis<BYTE*>(FUN_5d3080, c))
        if (F(int, c, 36))
            return c;
    return 0;
}

// [FUN_5be1d0] draws the control (unless hidden) and its children at its offset (+44, +48); a capturing child draws alone
extern "C" int __fastcall Ctl_DrawAll(BYTE* self, void*, int a, int x, int y, int focus)
{
    if (F(int, self, 40) && !F(BYTE, self, 24))
        return 0;
    BYTE* c = Ct_Captured(self);
    if (c)
    {
        CallVirt(c, 28, a, F(int, self, 44) + x, F(int, self, 48) + y, focus);
        return 1;
    }
    CallThis(FUN_5bd940, self, a, x, y, focus);
    for (BYTE* s = F(BYTE*, self, 288); s; s = CallThis<BYTE*>(FUN_5d3080, s))
        CallVirt(s, 28, a, x + F(int, self, 44), y + F(int, self, 48), (focus && s == F(BYTE*, self, 292)) ? 1 : 0);
    return 0;
}

// [FUN_5be630] passes a point (made relative) to the capturing child, or to the child under it (focusing it and telling hook 702f4c)
extern "C" int __fastcall Ctl_PointTo(BYTE* self, void*, int x, int y)
{
    if (F(int, self, 40))
        return 0;
    int rx = x - F(int, self, 44);
    int ry = y - F(int, self, 48);
    BYTE* c = Ct_Captured(self);
    if (c)
        return CallVirt<int>(c, 36, rx, ry);
    BYTE* s = CallThis<BYTE*>(FUN_5be5a0, self, rx, ry);
    if (!s)
        return 0;
    if (g_ct67f3d8 && !g_ct702f24 && !(F(DWORD, s, 32) & 0x40000000))
        CallThis(FUN_5bdba0, self, s);
    if (g_ct702f3c != s && !g_ct702f24 && !CallVirt<int>(s, 44))
    {
        g_ct702f3c = s;
        if (g_ct702f4c)
            g_ct702f4c(self, s);
    }
    return CallVirt<int>(s, 36, rx, ry);
}

