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
extern "C" void FUN_5bd5e0(); KING_ALIAS("_FUN_5bd5e0", "@Control_Construct@16")    // base control constructor (a, flags)

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
extern "C" void FUN_5b7f20(); KING_ALIAS("_FUN_5b7f20", "_Surface_VLine")    // vertical line
extern "C" void FUN_5b7ce0(); KING_ALIAS("_FUN_5b7ce0", "_Surface_HLine")    // horizontal line
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
extern "C" void FUN_5bd940(); KING_ALIAS("_FUN_5bd940", "@Ctl_DrawFace@24")
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


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_5bda90(); KING_ALIAS("_FUN_5bda90", "@ConGroup_Destroy@4")
extern "C" void FUN_5be870(); KING_ALIAS("_FUN_5be870", "@Con5be870_Destroy@4")
extern "C" void FUN_5bf7d0(); KING_ALIAS("_FUN_5bf7d0", "@Con5bf7d0_Destroy@4")

static BYTE* Con_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_5bd700]
extern "C" int __fastcall Con_5bd700(BYTE* s, void*, int, int) { return 0; }
// [$L_5bdb30]
extern "C" BYTE* __fastcall Con5bdb30_DeletingDestructor(BYTE* s, void*, int flags) { return Con_DeleteAfter(s, flags, FUN_5bda90); }
// [$L_5be850]
extern "C" BYTE* __fastcall Con5be850_DeletingDestructor(BYTE* s, void*, int flags) { return Con_DeleteAfter(s, flags, FUN_5be870); }
// [$L_5bf7b0]
extern "C" BYTE* __fastcall Con5bf7b0_DeletingDestructor(BYTE* s, void*, int flags) { return Con_DeleteAfter(s, flags, FUN_5bf7d0); }

extern "C" void FUN_5be130(); KING_ALIAS("_FUN_5be130", "@Ctl_LayoutRow@12")
extern "C" void FUN_5be630(); KING_ALIAS("_FUN_5be630", "@Ctl_PointTo@16")
extern "C" void FUN_5be5a0(); KING_ALIAS("_FUN_5be5a0", "@Ctl_Dispatch@16")
extern "C" void FUN_5bda20(); KING_ALIAS("_FUN_5bda20", "@Ctl_Contains@16")

// [$L_5bf740]
extern "C" int __fastcall Con_5bf740(BYTE* s) { return CallThis<int>(FUN_5be130, s, 4); }
// [$L_5c06b0]
extern "C" int __fastcall Con_5c06b0(BYTE* s, void*, int a, int b) { return CallThis<int>(FUN_5be630, s, a, b); }

// the command of a toggled control: its id (+28) plus the new state's bit, as a command (0x8000)
static int Con_ToggleCommand(BYTE* s, unsigned mask)
{
    unsigned v = F(unsigned, s, 284) ^ mask;
    F(unsigned, s, 284) = v;
    return (int)((((~v) >> 1) & 1) + F(unsigned, s, 28)) | 0x8000;
}

// [$L_5bfa50] a button: Enter gives its command (13 while disabled, +40)
extern "C" int __fastcall Con_ButtonKey(BYTE* s, void*, int key, int up)
{
    if (up || key != 13)
        return key;
    if (F(int, s, 40))
        return 13;
    return F(int, s, 28) | 0x8000;
}

// [$L_5bfcc0] a check box: Enter or Space toggles it
extern "C" int __fastcall Con_CheckKey(BYTE* s, void*, int key, int up)
{
    if (up || (key != 13 && key != 32) || F(int, s, 40))
        return key;
    return Con_ToggleCommand(s, 2);
}

// [$L_5bfd00] a check box: a press (513) toggles it, a release (514) clears its pressed bit
extern "C" int __fastcall Con_CheckMouse(BYTE* s, void*, int msg, int, int)
{
    if (msg == 514) {
        F(unsigned, s, 284) &= ~4u;
        return 32768;
    }
    if (msg != 513)
        return msg;
    if (F(int, s, 40))
        return 513;
    return Con_ToggleCommand(s, 6);
}

// [$L_5be5f0] a point (x, y): FUN_5be5a0 relative to the origin (+44, +48), else FUN_5bda20
extern "C" int __fastcall Con_5be5f0(BYTE* s, void*, int x, int y)
{
    if (CallThis<int>(FUN_5be5a0, s, x - F(int, s, 44), y - F(int, s, 48)))
        return 1;
    return CallThis<int>(FUN_5bda20, s, x, y);
}

// ---------------------------------------------------------------------------
// Radio buttons, sliders and control layout.

extern "C" void FUN_5d3080(); KING_ALIAS("_FUN_5d3080", "@Node_Next@8")
ASM_VAR(BYTE*, g_con702f24, $L_702f24)

// [$L_5bfb30] vtbl+24 (a, b); for the focused control (702f24) bit 1 of +284 follows the result
extern "C" int __fastcall Con_5bfb30(BYTE* s, void*, int a, int b)
{
    int r = CallVirt<int>(s, 24, a, b);
    if (s != g_con702f24)
        return r != 0;
    if (r)
        F(unsigned, s, 284) |= 2;
    else
        F(unsigned, s, 284) &= ~2u;
    return 1;
}

// clears the bits of +284 on the siblings before (+4 chain) and after (FUN_5d3080) the control
static void Con_ClearGroup(BYTE* s, unsigned keep)
{
    for (BYTE* o = F(BYTE*, s, 4); o; o = F(BYTE*, o, 4))
        F(unsigned, o, 284) &= keep;
    for (BYTE* o = CallThis<BYTE*>(FUN_5d3080, s); o; o = CallThis<BYTE*>(FUN_5d3080, o))
        F(unsigned, o, 284) &= keep;
}

// [$L_5bfb90] a radio button: Enter selects it
extern "C" int __fastcall Con_RadioKey(BYTE* s, void*, int key, int up)
{
    if (up || key != 13)
        return key;
    if (F(int, s, 40))
        return 13;
    Con_ClearGroup(s, ~2u);
    F(unsigned, s, 284) |= 2;
    return F(int, s, 28) | 0x8000;
}

// [$L_5bfc20] a radio button: a press (513) selects it, a release (514) clears its pressed bit
extern "C" int __fastcall Con_RadioMouse(BYTE* s, void*, int msg, int, int)
{
    if (msg == 514) {
        F(unsigned, s, 284) &= ~4u;
        return 32768;
    }
    if (msg != 513)
        return msg;
    if (F(int, s, 40))
        return 513;
    Con_ClearGroup(s, ~6u);
    F(unsigned, s, 284) |= 6;
    return F(int, s, 28) | 0x8000;
}

// [$L_5bf280] a slider: the arrows (left/right when horizontal, +328 == 1, else up/down) move the
// value (*+300) by the step (+340) within 0..+296
extern "C" int __fastcall Con_SliderKey(BYTE* s, void*, int key, int up)
{
    if (up)
        return key;
    int dec = F(int, s, 328) == 1 ? 37 : 38;
    int inc = F(int, s, 328) == 1 ? 39 : 40;
    short* v = F(short*, s, 300);
    if (key == dec) {
        *v = (short)(*v - F(short, s, 340));
        if (*F(short*, s, 300) < 0)
            *F(short*, s, 300) = 0;
        return 32768;
    }
    if (key == inc) {
        *v = (short)(*v + F(short, s, 340));
        if (*F(short*, s, 300) > F(short, s, 296))
            *F(short*, s, 300) = F(short, s, 296);
        return 32768;
    }
    return key;
}

// [$L_5c02e0] the size (+52, +56) of the picture: +20/+22 of +96 (flags 16, 36) or +4/+6 of +100 (72)
extern "C" void __fastcall Con_PictureSize(BYTE* s)
{
    int f = F(int, s, 32);
    F(int, s, 48) = 0;
    F(int, s, 44) = 0;
    if (f & (16 | 36)) {
        BYTE* p = F(BYTE*, s, 96);
        if (!p)
            return;
        F(int, s, 52) = F(WORD, p, 20) - 1;
        F(int, s, 56) = F(WORD, p, 22) - 1;
    } else if (f & 72) {
        BYTE* p = F(BYTE*, s, 100);
        if (!p)
            return;
        F(int, s, 52) = F(WORD, p, 4) - 1;
        F(int, s, 56) = F(WORD, p, 6) - 1;
    }
    if (F(int, s, 52) < 0)
        F(int, s, 52) = 0;
    if (F(int, s, 56) < 0)
        F(int, s, 56) = 0;
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

extern "C" void FUN_5bd590(); KING_ALIAS("_FUN_5bd590", "_Gui_Reset702f24")
extern "C" void FUN_5bd560(); KING_ALIAS("_FUN_5bd560", "_Ctl_SetOrigin")
extern "C" void FUN_5bdba0(); KING_ALIAS("_FUN_5bdba0", "@Ctl_Select@12")
extern "C" void FUN_5bd940(); KING_ALIAS("_FUN_5bd940", "@Ctl_DrawFace@24")
extern "C" void FUN_5ea0e0(); KING_ALIAS("_FUN_5ea0e0", "@Font_TextWidth@12")
extern "C" void FUN_5ea170(); KING_ALIAS("_FUN_5ea170", "_Text_Draw")

// [$L_5bfa80] a push button: a press (513) holds it, the release (514) over it gives its command
extern "C" int __fastcall Con_ButtonMouse(BYTE* s, void*, int msg, int x, int y)
{
    if (msg == 514) {
        if (F(int, s, 40))
            return 514;
        CallC(FUN_5bd590);
        if (CallVirt<int>(s, 24, x, y)) {
            F(unsigned, s, 284) &= ~2u;
            return F(int, s, 28) | 0x8000;
        }
        return F(int, s, 28) | 0xa000;
    }
    if (msg != 513)
        return msg;
    if (F(int, s, 40))
        return 513;
    F(unsigned, s, 284) |= 2;
    CallC(FUN_5bd560, s, x, y);
    return F(int, s, 28) | 0xc000;
}

// [$L_5bf880] the size (+52, +56) from the picture (flags 16/36: +96, 72: +100) or the text (+12 in font +16)
extern "C" void __fastcall Con_Size(BYTE* s)
{
    int f = F(int, s, 32);
    if (f & (16 | 36)) {
        BYTE* p = F(BYTE*, s, 96);
        if (!p)
            return;
        F(int, s, 48) = 0;
        F(int, s, 44) = 0;
        F(int, s, 52) = F(WORD, p, 20) - 1;
        F(int, s, 56) = F(WORD, p, 22) - 1;
    } else if (f & 72) {
        BYTE* p = F(BYTE*, s, 100);
        if (!p)
            return;
        F(int, s, 48) = 0;
        F(int, s, 44) = 0;
        F(int, s, 52) = F(WORD, p, 4) - 1;
        F(int, s, 56) = F(WORD, p, 6) - 1;
    } else {
        F(int, s, 48) = 0;
        F(int, s, 44) = 0;
        int w = CallThis<int>(FUN_5ea0e0, F(BYTE*, s, 16), F(int, s, 12));
        F(int, s, 52) = w + F(int, s, 44) + 4;
        F(int, s, 56) = F(int, F(BYTE*, s, 16), 20) + F(int, s, 48) + 4;
    }
    if (F(int, s, 32) & 0x300)
        F(int, s, 60) = 1;
    if (F(int, s, 52) < F(int, s, 44))
        F(int, s, 52) = F(int, s, 44);
    if (F(int, s, 56) < F(int, s, 48))
        F(int, s, 56) = F(int, s, 48);
}

// [$L_5befd0] a scroll bar: a press on a part (+324) starts it, on the track pages (3 or 4), the release ends it
extern "C" int __fastcall Con_ScrollMouse(BYTE* s, void*, int msg, int x, int y)
{
    BYTE* hit = CallThis<BYTE*>(FUN_5be5a0, s, x - F(int, s, 44), y - F(int, s, 48));
    if (msg == 514) {
        if (F(int, s, 324) == -1)
            return 514;
        if (hit)
            F(unsigned, hit, 284) &= ~6u;
        F(int, s, 324) = -1;
        if (s == g_con702f24)
            CallC(FUN_5bd590);
        return 32768;
    }
    if (msg != 513)
        return msg;
    if (!hit) {
        BYTE* n = CallThis<BYTE*>(FUN_5d3080, F(BYTE*, s, 288));
        F(int, s, 324) = x - F(int, s, 44) < F(int, n, 44) ? 4 : 3;
    } else {
        CallThis(FUN_5bdba0, s, hit);
        F(int, s, 324) = CallVirt<int>(hit, 32, 513, x - F(int, s, 44), y - F(int, s, 48)) & ~0x8000;
    }
    CallC(FUN_5bd560, s, x, y);
    return 32768;
}

// [$L_5be4b0] a mouse event goes to the child that has the capture (+36), else to the one under it
extern "C" int __fastcall Con_GroupMouse(BYTE* s, void*, int msg, int x, int y)
{
    if (F(int, s, 40))
        return msg;
    for (BYTE* c = F(BYTE*, s, 288); c; c = CallThis<BYTE*>(FUN_5d3080, c))
        if (F(int, c, 36)) {
            if (CallVirt<int>(c, 24, x - F(int, s, 44), y - F(int, s, 48)))
                return CallVirt<int>(c, 32, msg, x - F(int, s, 44), y - F(int, s, 48));
            return 32768;
        }
    BYTE* hit = CallThis<BYTE*>(FUN_5be5a0, s, x - F(int, s, 44), y - F(int, s, 48));
    if (!hit)
        return msg;
    if (msg == 513)
        CallThis(FUN_5bdba0, s, hit);
    if (hit == F(BYTE*, s, 292))
        return CallVirt<int>(hit, 32, msg, x - F(int, s, 44), y - F(int, s, 48));
    return msg;
}

// [$L_5bf940] draws the control, then its text (+12, font +16) aligned (+32 bits 12, 13) in the state's color
extern "C" int __fastcall Con_DrawText(BYTE* s, void*, int a, int b, int c, int d)
{
    CallThis(FUN_5bd940, s, a, b, c, d);
    BYTE* font = F(BYTE*, s, 16);
    if (!font)
        return 0;
    int w = CallThis<int>(FUN_5ea0e0, font, F(int, s, 12));
    int pad = F(int, s, 60);
    int x;
    if (F(int, s, 32) & 0x1000)
        x = b + F(int, s, 44) + pad + 3;
    else if (F(int, s, 32) & 0x2000)
        x = b + (F(int, s, 52) - pad - w) - 3;
    else
        x = b + ((F(int, s, 52) - pad * 2 - F(int, s, 44)) / 2 - w / 2 + F(int, s, 44) + pad);
    font = F(BYTE*, s, 16);
    int t2 = (F(int, s, 56) - pad * 2 - F(int, s, 48)) / 2;
    int y = F(int, font, 20) / 2 + F(int, s, 48) + t2 + pad + c;
    int st = F(int, s, 284);
    if (!d)
        st &= 2;
    int k = (st & 3) + 2;
    CallC(FUN_5ea170, a, x, y, font, F(int, s, 12), F(int, s, k * 36), 0);
    return 0;
}

// [$L_5bedb0] lays out the scroll bar's arrows and thumb (children from +288) along its direction (+328)
extern "C" void __fastcall Con_ScrollLayout(BYTE* s)
{
    int a = F(int, s, 312);
    F(int, s, 308) += a * 2;
    F(int, s, 304) -= a;
    BYTE* c = F(BYTE*, s, 288);
    CallVirt(c, 20);
    F(int, s, 304) += F(int, c, F(int, s, 328) == 1 ? 52 : 56);
    c = CallThis<BYTE*>(FUN_5d3080, c);
    CallVirt(c, 20);
    int e;
    if (F(int, s, 328) == 1) {
        F(int, s, 308) += F(int, c, 44) - F(int, c, 52);
        e = F(int, c, 52);
    } else {
        F(int, s, 308) += F(int, c, 44) - F(int, c, 56);
        e = F(int, c, 56);
    }
    e -= F(int, c, 44);
    c = CallThis<BYTE*>(FUN_5d3080, c);
    CallVirt(c, 20);
    int mode = F(int, s, 328);
    F(int, s, 308) += F(int, c, 44) - F(int, c, mode == 1 ? 52 : 56);
    int v = F(int, s, 308) - F(int, s, 60) + e;
    int o = F(int, s, 320);
    if (mode == 1) {
        F(int, c, 52) += v;
        F(int, c, 44) += v;
        F(int, c, 48) += o;
        F(int, c, 56) += o;
    } else {
        F(int, c, 44) += o;
        F(int, c, 52) += o;
        F(int, c, 48) += v;
        F(int, c, 56) += v;
    }
}

// ---------------------------------------------------------------------------
// More virtual methods.

typedef void (*AsmFnC)();

// [$L_5bf0d0] a slider being dragged (+324 == 1) follows the mouse; leaving it ends the drag
extern "C" int __fastcall Con_SliderMove(BYTE* s, void*, int x, int y)
{
    if (!CallVirt<int>(s, 24, x, y)) {
        int st = F(int, s, 324);
        if (st == 0 || st == 2 || st == 1) {
            F(int, s, 324) = -1;
            if (F(BYTE*, s, 292))
                F(int, F(BYTE*, s, 292), 284) = 0;
            if (s == g_con702f24)
                CallC(FUN_5bd590);
        }
    }
    x -= F(int, s, 44);
    y -= F(int, s, 48);
    if (F(int, s, 324) == 1) {
        CallThis(FUN_5d3080, F(BYTE*, s, 288));
        int half = F(int, s, 312) / 2;
        int lo = F(int, s, 304), hi = F(int, s, 308);
        int p = F(int, s, 328) == 1 ? x : y;
        short* v = F(short*, s, 300);
        if (p < lo + half)
            *v = 0;
        else if (p > hi + half)
            *F(short*, s, 300) = F(short, s, 296);
        else
            *F(short*, s, 300) = (short)(int)((double)(p - lo - half) / (hi - lo) * F(short, s, 296));
        AsmFnC hook = F(AsmFnC, s, 332);
        if (hook)
            CallC(hook, F(int, s, 336));
    }
    if (F(int, s, 324) == -1) {
        BYTE* h = CallThis<BYTE*>(FUN_5be5a0, s, x, y);
        CallThis(FUN_5bdba0, s, h);
        if (h)
            return CallVirt<int>(h, 36, x, y);
    }
    return 1;
}

ASM_VAR(AsmLabel, g_con64dc84, $L_64dc84)
ASM_VAR(AsmLabel, g_con64d81c, $L_64d81c)

// [FUN_5be870] (vtable 64dc84) frees +12
extern "C" void __fastcall Con5be870_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_con64dc84;
    operator delete(F(void*, s, 12));
    F(void*, s, 0) = g_con64d81c;
}
// [FUN_5bf7d0]
extern "C" void __fastcall Con5bf7d0_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_con64dc84;
    operator delete(F(void*, s, 12));
    F(void*, s, 0) = g_con64d81c;
}

// ---------------------------------------------------------------------------
// Constructors and window resizing (exception frames dropped).

extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")
ASM_VAR(AsmLabel, g_con65179c, $L_65179c)

// [FUN_5bda90] a control group (vtable 65179c) deletes its children (from +288) and the first one
extern "C" void __fastcall ConGroup_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_con65179c;
    BYTE* c = F(BYTE*, s, 288);
    if (c) {
        for (BYTE* o = CallThis<BYTE*>(FUN_5d3080, c); o; o = CallThis<BYTE*>(FUN_5d3080, c)) {
            CallThis(FUN_5d21b0, o);
            if (o)
                CallVirt(o, 0, 1);
        }
        CallVirt(c, 0, 1);
    }
    F(void*, s, 0) = g_con64dc84;
    operator delete(F(void*, s, 12));
    F(void*, s, 0) = g_con64d81c;
}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

extern "C" void FUN_5be700(); KING_ALIAS("_FUN_5be700", "@ScrollBar_Construct@24")
extern "C" void FUN_5bdbf0(); KING_ALIAS("_FUN_5bdbf0", "@Ctl_AddChild@16")
#include <string.h>
ASM_VAR(AsmLabel, g_con6518b0, $L_6518b0)
ASM_VAR(AsmLabel, g_con68d1c0, $L_68d1c0)
ASM_VAR(BYTE*, g_con702f40, $L_702f40)
ASM_VAR(int, g_con702f44, $L_702f44)

// [FUN_5bd5e0] constructor (name, id): a control (vtable 64dc84) sized for its name in the default font (702f40)
extern "C" BYTE* __fastcall Control_Construct(BYTE* s, void*, const char* name, int id)
{
    F(void*, s, 0) = g_con64d81c;
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(int, s, 56) = 0;
    F(int, s, 52) = 0;
    F(int, s, 48) = 0;
    F(int, s, 44) = 0;
    for (int k = 0; k < 6; k++) {
        BYTE* r = s + 68 + k * 36;
        for (int o = 0; o < 36; o += 4)
            F(DWORD, r, o) = o == 24 ? 0x3f000000 : 0;
    }
    F(void*, s, 0) = g_con64dc84;
    F(BYTE*, s, 16) = g_con702f40;
    F(int, s, 20) = g_con702f44;
    int w = 0, h = 0;
    if (g_con702f40) {
        h = F(int, g_con702f40, 20);
        w = CallThis<int>(FUN_5ea0e0, g_con702f40, name);
    }
    F(int, s, 52) = w + 6;
    F(int, s, 56) = h + 4;
    F(int, s, 48) = 0;
    F(int, s, 44) = 0;
    F(int, s, 60) = 0;
    F(int, s, 64) = 0;
    size_t n = strlen(name) + 1;
    char* t = (char*)operator new(n);
    F(int, s, 32) = id;
    F(char*, s, 12) = t;
    memcpy(t, name, n);
    F(int, s, 284) = 0;
    F(int, s, 36) = 0;
    F(int, s, 28) = -1;
    F(int, s, 40) = 0;
    F(BYTE, s, 24) = 0;
    return s;
}

// [FUN_5c0550] constructor (name, id, rows): a list box (vtable 6518b0) with its scroll bar (+340)
extern "C" BYTE* __fastcall ListBox_Construct(BYTE* s, void*, const char* name, int id, int rows)
{
    Control_Construct(s, 0, name, id);
    static const int zero[] = {288, 292, 324, 320, 316, 312};
    for (int o : zero)
        F(int, s, o) = 0;
    F(void*, s, 0) = g_con6518b0;
    F(int, s, 336) = 0;
    F(int, s, 300) = 0;
    F(int, s, 296) = 0;
    F(WORD, s, 332) = 0;
    BYTE* m = (BYTE*)operator new(344);
    F(BYTE*, s, 340) = m ? CallThis<BYTE*>(FUN_5be700, m, (BYTE*)g_con68d1c0, id, id, 0) : 0;
    F(int, F(BYTE*, s, 340), 340) = 1;
    CallThis(FUN_5bdbf0, s, F(BYTE*, s, 340), 0);
    F(int, s, 308) = rows;
    F(int, s, 328) = 0;
    F(int, s, 312) = 1;
    F(int, s, 56) += rows + 2;
    F(int, s, 316) = F(BYTE*, s, 16) ? F(int, F(BYTE*, s, 16), 20) + 2 : 2;
    F(int, s, 324) = F(int, s, 316) + rows;
    F(int, s, 320) = F(int, s, 52) - F(int, s, 44) - 1;
    F(int, s, 344) = 5;
    F(int, s, 348) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Scroll bar construction and chat (exception frames dropped).

extern "C" void FUN_5bf820(); KING_ALIAS("_FUN_5bf820", "@Button_ConstructId@20")
ASM_VAR(AsmLabel, g_con651804, $L_651804)
ASM_VAR(AsmLabel, g_con6517d0, $L_6517d0)

// [FUN_5be700] constructor (name, id, b, kind): a scroll bar (vtable 651804) with its three buttons (FUN_5bf820, parts 0-2)
extern "C" BYTE* __fastcall ScrollBar_Construct(BYTE* s, void*, const char* name, int id, int b, int kind)
{
    Control_Construct(s, 0, name, id);
    F(int, s, 288) = 0;
    F(int, s, 292) = 0;
    F(void*, s, 0) = g_con651804;
    F(int, s, 328) = kind;
    for (int k = 0; k < 3; k++) {
        BYTE* m = (BYTE*)operator new(288);
        if (m) {
            CallThis(FUN_5bf820, m, (BYTE*)g_con68d1c0, b, k);
            F(void*, m, 0) = g_con6517d0;
        }
        CallThis(FUN_5bdbf0, s, m, 0);
    }
    F(int, s, 324) = -1;
    F(int, s, 340) = 1;
    F(BYTE, s, 24) = 1;
    return s;
}

// ---------------------------------------------------------------------------
// Drawing a control's face.

// [FUN_5bd940] draws control s at x, y on dst in the style of its state: disabled (+40) +212, pressed or held
// (+284 bit 2 with +276/+280) +248, else by +284 bits 0-1 (bit 0 only with focus) one of the four from +68
extern "C" int __fastcall Ctl_DrawFace(BYTE* s, void*, BYTE* dst, int x, int y, int focus)
{
    int r[4] = {F(int, s, 44) + x, F(int, s, 48) + y, F(int, s, 52) + x, F(int, s, 56) + y};
    const DWORD* st;
    if (F(int, s, 40))
        st = (const DWORD*)(s + 212);
    else if ((F(int, s, 284) & 4) && (F(int, s, 280) || F(int, s, 276)))
        st = (const DWORD*)(s + 248);
    else {
        int k = F(int, s, 284);
        if (!focus)
            k &= 2;
        st = (const DWORD*)(s + 68 + (k & 3) * 36);
    }
    Ctl_DrawFrame(dst, r, F(DWORD, s, 32), st[0], st[1], st[2], st[3], st[4], st[5], st[6], (BYTE*)st[7], st[8]);
    return 0;
}

ASM_VAR(AsmLabel, g_q156_651798, $L_651798)
// [$L_5bcee0] destructor: frees the two blocks (+12, +8)
extern "C" void __fastcall Ctl5bcee0_Destruct(BYTE* s)
{
    F(void*, s, 0) = (BYTE*)g_q156_651798;
    operator delete(F(void*, s, 12));
    operator delete(F(void*, s, 8));
}
