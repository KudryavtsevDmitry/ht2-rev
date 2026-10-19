// game_shell.cpp - carat.cpp of the game, rewritten from king.masm: small
// functions of the game's main unit: 4x4 double matrices, rectangles, string
// helpers, the screen size, name tables, and the methods of menus and
// screens that only set fields and redraw.
//
// Building with  nmake ASM_GAME_SHELL=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS     // strcpy as in the game
#include <intrin.h>
#include <string.h>
#include "vecmath.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl isalnum(int c);
extern "C" int __cdecl sprintf(char* buf, const char* format, ...);

extern "C" int __cdecl Carat_Read7ad0(int a, int b, int* values, int c, int d, int n);
KING_ALIAS("_Carat_Read7ad0", "FUN_507ad0")
extern "C" int __cdecl Carat_Read7df0(int a, int b, int* values, int c, int d, int n);
KING_ALIAS("_Carat_Read7df0", "FUN_507df0")
extern "C" BYTE* __cdecl Carat_Window(int a, int b);
KING_ALIAS("_Carat_Window", "FUN_50e200")
extern "C" int __cdecl Carat_LoadFont(const char* name);
KING_ALIAS("_Carat_LoadFont", "FUN_5e8c60")
extern "C" void __cdecl Carat_Draw5b86c0(int a, int b, int c, int d);
KING_ALIAS("_Carat_Draw5b86c0", "_Bitmap_Draw")
extern "C" BYTE* __cdecl Messages_Clear1276();                     // copter.cpp
extern "C" int __cdecl Carat_Resource(int id);
KING_ALIAS("_Carat_Resource", "_Table71e858_Get")
extern "C" void __cdecl Carat_51d8f0(int a, int b);
KING_ALIAS("_Carat_51d8f0", "_Save_WriteEntry")
extern "C" int __cdecl Carat_5d3ab0();
KING_ALIAS("_Carat_5d3ab0", "FUN_5d3ab0")
extern "C" void __cdecl Carat_Message(const char* text, int);
KING_ALIAS("_Carat_Message", "_Sys_Log")
extern "C" int __fastcall Lights_Release(BYTE* self, void*, int a);   // aeffects.cpp
extern "C" void FUN_50c010(); KING_ALIAS("_FUN_50c010", "@Screen_Grab@8")
extern "C" void __fastcall CGameApp_SetMousePos(void* app, void*, int x, int y);   // gameapp.cpp
extern "C" void FUN_5d85f0(); KING_ALIAS("_FUN_5d85f0", "@Win_GetRect@12")    // window: its rectangle
extern "C" void FUN_5bd5a0(); KING_ALIAS("_FUN_5bd5a0", "@Style_Assign@12")
ASM_PROC(FUN_5bd5e0)
extern "C" void FUN_5bef30(); KING_ALIAS("_FUN_5bef30", "@Ctl_SetStyles@28")
extern "C" void FUN_5bef90(); KING_ALIAS("_FUN_5bef90", "@Ctl_SetStyle5@16")
ASM_VAR(AsmLabel, s_676310, $L_676310)
ASM_VAR(AsmLabel, s_676308, $L_676308)
ASM_VAR(AsmLabel, s_676318, $L_676318)
ASM_VAR(AsmLabel, s_676338, $L_676338)
ASM_VAR(AsmLabel, s_67632c, $L_67632c)
ASM_VAR(AsmLabel, s_676320, $L_676320)
extern "C" void FUN_5ffb40(); KING_ALIAS("_FUN_5ffb40", "_Music_Clear")
ASM_PROC(FUN_5ba4a0)
ASM_PROC(FUN_5bae30)
extern "C" void FUN_5d4200(); KING_ALIAS("_FUN_5d4200", "_Dd_LockBack")
extern "C" void FUN_5d41e0(); KING_ALIAS("_FUN_5d41e0", "_Dd_WaitVsync")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
extern "C" void FUN_568fd0(); KING_ALIAS("_FUN_568fd0", "_Game_568fd0")
extern "C" void FUN_5bf820(); KING_ALIAS("_FUN_5bf820", "@Button_ConstructId@20")
extern "C" void FUN_5bf750(); KING_ALIAS("_FUN_5bf750", "@Button_Construct@16")
extern "C" void FUN_603c70(); KING_ALIAS("_FUN_603c70", "_Ini_WriteInt")
ASM_PROC(FUN_5d8710)
extern "C" void FUN_5c1260(); KING_ALIAS("_FUN_5c1260", "@Avi_Init@8")
ASM_PROC(FUN_5c1b10)
ASM_PROC(FUN_5d8850)
ASM_VAR(BYTE*, g_696ca0, $L_696ca0)
ASM_VAR(int, g_screenW, $L_696850)
ASM_VAR(int, g_screenH, $L_696854)
ASM_VAR(AsmLabel, s_676248, $L_676248)
ASM_VAR(AsmLabel, s_676238, $L_676238)
ASM_VAR(AsmLabel, g_vtbl64dc1c, $L_64dc1c)
ASM_VAR(AsmLabel, g_6ce408, $L_6ce408)
ASM_VAR(AsmLabel, g_6ce3b8, $L_6ce3b8)
ASM_VAR(AsmLabel, g_6ceb38, $L_6ceb38)
ASM_VAR(AsmLabel, g_6ce988, $L_6ce988)
ASM_VAR(AsmLabel, g_6ce430, $L_6ce430)
ASM_VAR(AsmLabel, g_vtbl64dbe8, $L_64dbe8)
ASM_VAR(AsmLabel, g_vtbl64de50, $L_64de50)
ASM_VAR(AsmLabel, g_vtbl64de10, $L_64de10)
ASM_VAR(AsmLabel, g_vtbl64e008, $L_64e008)
ASM_VAR(AsmLabel, g_vtbl64dfc8, $L_64dfc8)
ASM_VAR(const char*, g_iniPath, $L_696868)
ASM_VAR(AsmLabel, s_676e94, $L_676e94)
ASM_VAR(AsmLabel, s_676e9c, $L_676e9c)
ASM_VAR(AsmLabel, s_676eb4, $L_676eb4)
ASM_VAR(AsmLabel, s_672998, $L_672998)
ASM_VAR(AsmLabel, s_672994, $L_672994)
ASM_VAR(AsmLabel, s_6729a8, $L_6729a8)
ASM_VAR(AsmLabel, g_vtbl64e510, $L_64e510)
ASM_VAR(AsmLabel, g_vtbl64e4d0, $L_64e4d0)
ASM_VAR(AsmLabel, g_vtbl64e598, $L_64e598)
ASM_VAR(AsmLabel, g_vtbl64e558, $L_64e558)
ASM_VAR(AsmLabel, g_vtbl64e620, $L_64e620)
ASM_VAR(AsmLabel, g_vtbl64e5e0, $L_64e5e0)
ASM_VAR(AsmLabel, g_720938, $L_720938)
ASM_VAR(AsmLabel, g_vtbl64eb58, $L_64eb58)
ASM_PROC(FUN_5c13a0)    // avi: open
ASM_PROC(FUN_5d89e0)

ASM_VAR(int, g_696890, $L_696890)
typedef wchar_t WideBuf[260];
ASM_VAR(WideBuf, g_wideBuf, $L_6ce628)
typedef char NarrowBuf[260];
ASM_VAR(NarrowBuf, g_narrowBuf, $L_6ce524)
ASM_VAR(AsmLabel, g_defaultChar, $L_67621c)     // "?"
ASM_VAR(BYTE*, g_lights, $L_6ce520)
ASM_VAR(BYTE*, g_world, $L_696ca0)
ASM_VAR(BYTE*, g_696848, $L_696848)
ASM_VAR(int, g_696880, $L_696880)
ASM_VAR(int, g_696884, $L_696884)
ASM_VAR(int, g_screenWidth, $L_675460)
ASM_VAR(int, g_screenHeight, $L_675464)
ASM_VAR(AsmLabel, g_696750, $L_696750)
ASM_VAR(int, g_6cec68, $L_6cec68)
ASM_VAR(void*, g_6ce4f4, $L_6ce4f4)
ASM_VAR(BYTE*, g_69684c, $L_69684c)
typedef int Flags6cea74[1];
ASM_VAR(Flags6cea74, g_6cea74, $L_6cea74)
ASM_VAR(AsmLabel, g_strLargeFon, $L_67622c)     // "large.fon"
ASM_VAR(AsmLabel, g_strMenuFon, $L_676220)      // "menu.fon"
ASM_VAR(int, g_fontLarge, $L_6ced28)
ASM_VAR(int, g_fontMenu, $L_6ced24)
ASM_VAR(int, g_6ced70, $L_6ced70)
ASM_VAR(BYTE*, g_6cec6c, $L_6cec6c)
typedef char Text[1];
ASM_VAR(Text, g_6ce8b4, $L_6ce8b4)
typedef const char* Names[1];
ASM_VAR(Names, g_names675ba4, $L_675ba4)
ASM_VAR(Names, g_names675c10, $L_675c10)
ASM_VAR(int, g_675ab0, $L_675ab0)
ASM_VAR(BYTE*, g_6cecd8, $L_6cecd8)
ASM_VAR(BYTE*, g_6cec70, $L_6cec70)
ASM_VAR(AsmLabel, g_vtbl64eb10, $L_64eb10)
ASM_VAR(AsmLabel, g_vtbl64ead0, $L_64ead0)
ASM_VAR(AsmLabel, g_vtbl64dc50, $L_64dc50)
ASM_VAR(void*, g_6cec60, $L_6cec60)
ASM_VAR(void*, g_6cec64, $L_6cec64)
ASM_VAR(BYTE*, g_6cec80, $L_6cec80)
ASM_VAR(int, g_6f3494, $L_6f3494)
ASM_VAR(int, g_724190, $L_724190)
ASM_VAR(AsmLabel, g_6ce4f8, $L_6ce4f8)

// ---------------------------------------------------------------------------
// Reading values

extern "C" int __cdecl Carat_Read3(int a, int b, int c, int d, int e)
{
    return Carat_Read7ad0(a, b, (int*)c, d, e, 3);
}

extern "C" int __cdecl Carat_ReadFill(int a, int b, int fill, int c, int d, int n)
{
    int v[1000];
    for (int i = 0; i < n; i++)
        v[i] = fill;
    return Carat_Read7ad0(a, b, v, c, d, n);
}

extern "C" int __cdecl Carat_ReadCopy(int a, int b, const int* src, int c, int d, int n)
{
    int v[1000];
    if (n > 0)
        memcpy(v, src, n * 4);
    return Carat_Read7ad0(a, b, v, c, d, n);
}

extern "C" int __cdecl Carat_ReadFill2(int a, int b, int fill, int c, int d, int n)
{
    int v[1000];
    for (int i = 0; i < n; i++)
        v[i] = fill;
    return Carat_Read7df0(a, b, v, c, d, n);
}

// ---------------------------------------------------------------------------
// 4x4 double matrices

struct DRow { double v[4]; };
struct DMat4 { DRow r[4]; };

static DRow* RowZero(DRow* r)
{
    for (int i = 0; i < 4; i++)
        r->v[i] = 0;
    return r;
}

extern "C" double __cdecl Sign(int x) { return (x & 1) ? -1.0 : 1.0; }

// out = v * m
extern "C" DRow* __cdecl DRow_MulMat(DRow* out, const DRow* v, const DMat4* m)
{
    DRow r;
    RowZero(&r);
    for (int j = 0; j < 4; j++) {
        double s = r.v[j] = 0;
        for (int i = 0; i < 4; i++)
            s = s + v->v[i] * m->r[i].v[j];
        r.v[j] = s;
    }
    *out = r;
    return out;
}

// out = m * v
extern "C" DRow* __cdecl DMat_MulRow(DRow* out, const DMat4* m, const DRow* v)
{
    DRow r;
    RowZero(&r);
    for (int j = 0; j < 4; j++) {
        double s = r.v[j] = 0;
        for (int i = 0; i < 4; i++)
            s = s + m->r[j].v[i] * v->v[i];
        r.v[j] = s;
    }
    *out = r;
    return out;
}

struct DMat4Methods : DMat4 {
    // the signed cofactor of an element
    double Cofactor(int row, int col)
    {
        Matrix minor = g_identity;
        float* dst = minor.m;
        for (int i = 0; i < 4; i++) {
            if (i == row)
                continue;
            int k = 0;
            for (int j = 0; j < 4; j++)
                if (j != col)
                    dst[k++] = (float)r[i].v[j];
            dst += 3;
        }
        double sign = Sign(row + col);
        return minor.Determinant() * sign;
    }
};

extern "C" {
DRow* __fastcall DRow_Zero(DRow* s, void*) { return RowZero(s); }                                  // [FUN_50b460]
DRow* __fastcall DRow_Set(DRow* s, void*, DRow v) { *s = v; return s; }                            // [FUN_50b480]
void __fastcall DRow_Copy(DRow* s, void*, const DRow* v) { *s = *v; }                              // [FUN_50b4c0]
DMat4* __fastcall DMat_Zero(DMat4* s, void*)                                                        // [FUN_50b4f0]
{
    for (int i = 0; i < 4; i++)
        RowZero(&s->r[i]);
    memset(s, 0, sizeof *s);
    return s;
}
DMat4* __fastcall DMat_CopyNew(DMat4* s, void*, const DMat4* m)                                     // [FUN_50b520]
{
    for (int i = 0; i < 4; i++)
        RowZero(&s->r[i]);
    for (int i = 0; i < 4; i++)
        s->r[i] = m->r[i];
    return s;
}
DMat4* __fastcall DMat_FromRows(DMat4* s, void*, const DRow* a, const DRow* b, const DRow* c, const DRow* d) // [FUN_50b570]
{
    for (int i = 0; i < 4; i++)
        RowZero(&s->r[i]);
    s->r[0] = *a;
    s->r[1] = *b;
    s->r[2] = *c;
    s->r[3] = *d;
    return s;
}
void __fastcall DMat_Copy(DMat4* s, void*, const DMat4* m)                                          // [FUN_50b5d0]
{
    for (int i = 0; i < 4; i++)
        s->r[i] = m->r[i];
}
double __fastcall DMat_Cofactor(DMat4Methods* s, void*, int row, int col) { return s->Cofactor(row, col); } // [FUN_50b900]
}

// ---------------------------------------------------------------------------
// Strings

// cuts spaces at both ends; 0 for an empty string
extern "C" char* __cdecl Str_Trim(char* s)
{
    while (*s == ' ')
        s++;
    if (*s == 0)
        return 0;
    char* e = s + strlen(s) - 1;
    while (*e == ' ')
        *e-- = 0;
    return s;
}

// cuts a name at the first character that is not a letter, a digit, '-',
// '_' or a space
extern "C" char* __cdecl Str_Name(char* s)
{
    if (g_696890 == 1)
        return Str_Trim(s);
    char* p = s;
    while (isalnum((signed char)*p) || *p == '-' || *p == '_' || *p == ' ')
        p++;
    *p = 0;
    return s;
}

extern "C" int __cdecl Carat_True() { return 1; }

extern "C" wchar_t* __cdecl Str_ToWide(const char* s)
{
    MultiByteToWideChar(0, 0, s, -1, g_wideBuf, 260);
    return g_wideBuf;
}

extern "C" char* __cdecl Str_FromWide(const wchar_t* s)
{
    WideCharToMultiByte(0, 0, s, -1, g_narrowBuf, 260, g_defaultChar, (BOOL*)&s);
    return g_narrowBuf;
}

extern "C" const char* __cdecl Carat_TypeName(int a, int b)
{
    if (a < 23) {
        sprintf(g_6ce8b4, "%s%d", g_names675ba4[a], b + 1);
        return g_6ce8b4;
    }
    sprintf(g_6ce8b4, "%s", g_names675c10[b + a * 4 - 92]);
    return g_6ce8b4;
}

// ---------------------------------------------------------------------------
// The screen, fonts, lights

extern "C" void __cdecl Carat_ReleaseLight(int a)
{
    if (g_lights)
        Lights_Release(g_lights, 0, a);
}

extern "C" void __cdecl Carat_UpdateScreenSize()
{
    if (g_world == g_696848) {
        g_screenWidth = g_696880;
        g_screenHeight = g_696884;
    }
}

extern "C" void __cdecl Carat_SetScreenSize()
{
    CGameApp_SetMousePos(g_696750, 0, g_screenWidth, g_screenHeight);
}

extern "C" int __cdecl Carat_ScreenFlags() { return g_6cec68 ? 0x3e00 : 0x0e00; }
extern "C" int __cdecl Carat_HasMovie() { return g_6ce4f4 != 0; }

extern "C" int __cdecl Carat_OpenMovie()
{
    if (!g_6ce4f4)
        return CallThis<int>(FUN_50c010, &g_6ce4f4);
    return (int)g_6ce4f4;
}

extern "C" int __cdecl Carat_CloseMovie()
{
    if (!g_6ce4f4)
        return 0;
    CallVirt(g_6ce4f4, 0, 1);
    BYTE* w = g_world;
    g_6ce4f4 = 0;
    F(DWORD, w, 16) |= 3;
    return (int)Messages_Clear1276();
}

extern "C" void __cdecl Carat_DrawMovie()
{
    if (g_6ce4f4)
        Carat_Draw5b86c0(F(int, g_69684c, 12), 0, 0, (int)g_6ce4f4);
}

extern "C" void __cdecl Carat_SetFlag(int i) { g_6cea74[i] = 1; }

// returns the menu font: the original leaves it in EAX and a caller uses it
extern "C" void* __cdecl Carat_LoadFonts()
{
    g_fontLarge = Carat_LoadFont(g_strLargeFon);
    g_fontMenu = Carat_LoadFont(g_strMenuFon);
    return (void*)g_fontMenu;
}

extern "C" int __cdecl Carat_Level3() { return g_6ced70 >= 3; }

extern "C" void __cdecl Carat_LoadResource103()
{
    if (!F(int, g_6cec6c, 1316))
        Carat_51d8f0(Carat_Resource(103), 0);
}

// ---------------------------------------------------------------------------
// Rectangles: left, top, right, bottom

extern "C" BYTE* __cdecl Carat_MoveWindow(int a, int b, int x, int y)
{
    BYTE* w = Carat_Window(a, b);
    int dx = x - F(int, w, 44);
    F(int, w, 44) = x;
    F(int, w, 52) += dx;
    int dy = y - F(int, w, 48);
    F(int, w, 56) += dy;
    F(int, w, 48) = y;
    return w;
}

extern "C" int* __cdecl Carat_WindowRectBelow(int* out, void* window)
{
    int r[4];
    CallThis(FUN_5d85f0, window, r);
    out[0] = r[0];
    out[1] = r[1] + 50;
    out[2] = r[2];
    out[3] = r[3];
    return out;
}

extern "C" {
void __fastcall Rect_Offset(int* s, void*, int dx, int dy) { s[0] += dx; s[2] += dx; s[1] += dy; s[3] += dy; }   // [FUN_5115b0]
void __fastcall Rect_MoveTo(int* s, void*, int x, int y)                                                          // [FUN_5115e0]
{
    s[2] += x - s[0];
    s[0] = x;
    s[3] += y - s[1];
    s[1] = y;
}
}

// ---------------------------------------------------------------------------
// A table of up to 8 names of 16 characters

struct NameTable {
    int  count;
    char names[8][16];              // +4
    int  values[8];                 // +132

    int Find(const char* name)
    {
        int i = 0;
        for (; i < count; i++)
            if (strcmp(name, names[i]) == 0)
                break;
        return i < count ? i : -1;
    }

    int Add(const char* name)
    {
        if (Find(name) != -1)
            return 255;
        strcpy(names[count], name);
        values[count] = 0;
        return count++;
    }
};

// ---------------------------------------------------------------------------
// Game, menus and screens

extern "C" int __cdecl Carat_OutOfBox(const unsigned short* size, int x, int y)
{
    int w = size[0], h = size[1];
    int lo = w / 20;
    int hi = w / 3 - lo;
    if (x > hi || x < lo)
        return 1;
    int q = h / 4;
    if (y > h - q)
        return 1;
    return y < q;
}

extern "C" void __cdecl Carat_SetView(int a)
{
    BYTE* s = g_696848;
    if (!s) {
        g_6f3494 = a;
        return;
    }
    if (a == 0) {
        F(int, s, 52) = F(int, s, 48);
        BYTE* p;
        if (s == g_6cec80 && (p = F(BYTE*, s, 372)) != 0)
            F(int, p, 52) = F(int, p, 48);
        g_6f3494 = a;
        return;
    }
    F(void*, s, 52) = Carat_5d3ab0() == 1 ? g_6cec60 : g_6cec64;
    BYTE* p;
    if (s == g_6cec80 && (p = F(BYTE*, s, 372)) != 0)
        F(void*, p, 52) = g_6cec60;
    g_6f3494 = a;
}

struct ListEntry40 {                // 40 bytes
    BYTE  m_0[32];
    int   key;                      // +32
    short marked;                   // +36
    short m_38;
};

struct Screen {
    void InitBox();
    void SetBoxes(int a, int b, int c, int d);
    Screen* Construct(int a, int b);
    void SetState(int a);
    void SetStateB(int a);
    void SetChild(void* x);
    void Destruct();
    void OpenMovie(int a, int b);
    void Mark(int key);
};

void Screen::InitBox()
{
    static const int zero[] = { 12, 8, 4, 0, 32, 28, 20, 16 };
    for (int i = 0; i < 8; i++)
        F(int, this, zero[i]) = 0;
    F(DWORD, this, 24) = 0x3f000000;    // 0.5f
}

void Screen::SetBoxes(int a, int b, int c, int d)
{
    CallThis(FUN_5bd5a0, (BYTE*)this + 68, c);
    CallThis(FUN_5bd5a0, (BYTE*)this + 104, a);
    CallThis(FUN_5bd5a0, (BYTE*)this + 140, d);
    CallThis(FUN_5bd5a0, (BYTE*)this + 176, b);
}

Screen* Screen::Construct(int a, int b)
{
    CallThis(FUN_5bd5e0, this, a, b);
    F(void*, this, 0) = g_vtbl64dc50;
    F(int, this, 288) = 0;
    F(int, this, 292) = 0;
    return this;
}

void Screen::SetState(int a)
{
    F(short, this, 374) = F(short, this, 384);
    F(short, this, 392) = F(short, this, 384) > 5;
    F(int, this, 368) = a;
    F(short, this, 372) = F(short, this, 382);
    F(short, this, 378) = F(short, this, 388);
    F(short, this, 380) = F(short, this, 390);
    CallVirt(this, 36);
}

void Screen::SetStateB(int a)
{
    F(int, this, 368) = a;
    CallVirt(this, 36);
    F(DWORD, F(BYTE*, this, 384), 284) |= 2;
    if (F(short, this, 394) == 0)
        F(DWORD, F(BYTE*, this, 384), 284) &= ~2u;
    F(short, this, 378) = F(short, this, 394);
    F(short, this, 372) = F(short, this, 388);
    F(short, this, 374) = F(short, this, 390);
    F(short, this, 376) = F(short, this, 392);
    F(short, this, 380) = F(short, this, 396);
}

// switches the screen's child: the old one is told, the new one redrawn
void Screen::SetChild(void* x)
{
    g_675ab0 = 1;
    BYTE* cur = F(BYTE*, this, 372);
    if (cur) {
        if (x != cur)
            CallVirt(cur, 32);
        cur = F(BYTE*, this, 372);
        if (cur == g_6cecd8 || cur == g_6cec6c || cur == g_6cec70)
            CallVirt(cur + 64, 60, 1);
        if (!g_675ab0)
            return;
    }
    if (x)
        CallVirt(x, 36);
    else
        CallVirt(this, 36);
    F(void*, this, 372) = x;
}

void Screen::Destruct()
{
    F(void*, this, 0) = g_vtbl64eb10;
    F(void*, this, 64) = g_vtbl64ead0;
    operator delete(g_6cec60);
    CallThis(FUN_5d89e0, this);
}

void Screen::OpenMovie(int a, int b)
{
    F(int, this, 2308) = b;
    int m = b < 2 ? 0 : g_724190;
    if (CallThis<int>(FUN_5c13a0, (BYTE*)this + 56, a, 24, 1, m))
        Carat_Message("avi subsystem error", 0);
    CallVirt(this, 36);
    F(int, this, 2304) = 0;
}

void Screen::Mark(int key)
{
    ListEntry40* e = F(ListEntry40*, this, 0);
    for (int i = 0; i < F(int, this, 4); i++)
        if (e[i].key == key)
            e[i].marked = 1;
}

extern "C" void __cdecl Carat_MarkKey(int, int key)
{
    ((Screen*)g_6ce4f8)->Mark(key);
}

#define S(p) ((Screen*)(p))
#define N(p) ((NameTable*)(p))

extern "C" {
int __fastcall Names_Find(void* s, void*, const char* name) { return N(s)->Find(name); }             // [FUN_50c710]
int __fastcall Names_Add(void* s, void*, const char* name) { return N(s)->Add(name); }               // [FUN_50c770]
void* __fastcall Box_Init(void* s, void*) { S(s)->InitBox(); return s; }                             // [FUN_511610]
void __fastcall Screen_SetBoxes(void* s, void*, int a, int b, int c, int d) { S(s)->SetBoxes(a, b, c, d); } // [FUN_511640]
void* __fastcall Screen_Construct(void* s, void*, int a, int b) { return S(s)->Construct(a, b); }    // [FUN_511690]
void __fastcall Screen_SetState(void* s, void*, int a) { S(s)->SetState(a); }                        // [FUN_516e40]
void __fastcall Screen_SetStateB(void* s, void*, int a) { S(s)->SetStateB(a); }                      // [FUN_517a40]
void __fastcall Screen_Set372(void* s, void*, int a) { F(int, s, 372) = a; CallVirt(s, 36); }        // [FUN_51ed10]
void __fastcall Screen_SetChild(void* s, void*, void* x) { S(s)->SetChild(x); }                      // [FUN_527950]
void __fastcall Screen_Destruct(void* s, void*) { S(s)->Destruct(); }                                // [FUN_527cd0]
void __fastcall Screen_OpenMovie(void* s, void*, int a, int b) { S(s)->OpenMovie(a, b); }            // [FUN_528f50]
void __fastcall Screen_Set2304(void* s, void*, int) { F(int, s, 2304) = 7; }                         // [FUN_528fb0]
void __fastcall Screen_Mark(void* s, void*, int key) { S(s)->Mark(key); }                            // [FUN_529850]
}

// ---------------------------------------------------------------------------
// medium-sized functions of the unit

extern "C" {
size_t __cdecl fread(void* buf, size_t size, size_t n, FILE* f);
int __cdecl fseek(FILE* f, long off, int origin);
}
ASM_PROC(FUN_50b620)    // DMat4: determinant
extern "C" void FUN_530020(); KING_ALIAS("_FUN_530020", "@Words295_Reset@8")
ASM_PROC(FUN_5b81c0)    // draws a part of an image (surface, image, rect, a, y)
ASM_PROC(FUN_5b6bf0)
ASM_PROC(FUN_5b6ed0)
extern "C" void FUN_5b6b10(); KING_ALIAS("_FUN_5b6b10", "@Surface_Present@8")
extern "C" void FUN_4e1320(); KING_ALIAS("_FUN_4e1320", "_Startup_FindCd")
ASM_PROC(FUN_519750)
ASM_VAR(int, g_6ce370, $L_6ce370)               // the vehicle tech table is loaded
ASM_VAR(const char*, g_vehTechName, $L_674ae0)
ASM_VAR(int, g_697860, $L_697860)
ASM_VAR(int, g_6ce32c, $L_6ce32c)
typedef int Ints8[8];
ASM_VAR(Ints8, g_6ce34c, $L_6ce34c)
typedef BYTE VehTech[35 * 876];
ASM_VAR(VehTech, g_6c6b68, $L_6c6b68)
ASM_VAR(int, g_69689c, $L_69689c)
ASM_VAR(AsmLabel, g_6d1948, $L_6d1948)
ASM_VAR(int, g_6ced40, $L_6ced40)
ASM_VAR(int, g_6ced44, $L_6ced44)
ASM_VAR(BYTE*, g_6ced38, $L_6ced38)
ASM_VAR(int, g_6ced3c, $L_6ced3c)
ASM_VAR(void*, g_696868, $L_696868)             // the ini file
ASM_VAR(BYTE*, g_6cecc0, $L_6cecc0)
ASM_VAR(int, g_6ced64, $L_6ced64)
ASM_VAR(int, g_6ced50, $L_6ced50)
ASM_VAR(int, g_loadSteps, $L_675468)            // 800
ASM_VAR(BYTE*, g_6ced5c, $L_6ced5c)
ASM_VAR(BYTE*, g_6ced60, $L_6ced60)

extern "C" int __cdecl VehTech_Load()
{
    g_6ce370 = 0;
    FILE* f = fopen(g_vehTechName, "rb");
    int hdr = 0;
    fread(&hdr, 4, 1, f);
    if (!g_697860) {
        if (f)
            fclose(f);
        return g_6ce370;
    }
    if (!f)
        return g_6ce370;
    if (fseek(f, hdr + g_6ce32c * 4 + 193324, 0) == 0) {
        int n = 0;
        fread(&n, 4, 1, f);
        bool ok = n == 876;
        for (int i = 0; ok && i < 8; i++)
            ok = fread(&g_6ce34c[i], 1, 4, f) == 4;
        if (ok && fread(g_6c6b68, 876, 35, f) == 35 && !(F(BYTE, f, 12) & 16))
            g_6ce370 = 1;
    }
    fclose(f);
    return g_6ce370;
}

extern "C" DMat4* __cdecl DMat_Inverse(DMat4* out, const DMat4* m)
{
    DMat4 t, r;
    DMat_Zero(&r, 0);
    DMat_Zero(&t, 0);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            t.r[j].v[i] = m->r[i].v[j];
    double det = CallThis<double>(FUN_50b620, m);
    if (x87_eq(det, 0.0)) {
        DMat_CopyNew(out, 0, &r);
        return out;
    }
    double inv = 1.0 / det;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            r.r[i].v[j] = DMat_Cofactor((DMat4Methods*)&t, 0, i, j) * inv;
    DMat_CopyNew(out, 0, &r);
    return out;
}

// the loading picture while the game loads
extern "C" void __cdecl Loading_Update()
{
    if (g_69689c) {
        if (!g_6ce4f4) {
            CallThis(FUN_530020, g_6d1948);
            CallThis(FUN_50c010, &g_6ce4f4);
            g_6ced40 = 0;
        } else if (!g_6ced44)
            g_6ced40 = 1;
        BYTE* w = g_69684c;
        int s = F(int, w, 12);
        if (g_6ce4f4)
            Carat_Draw5b86c0(s, 0, 0, (int)g_6ce4f4);
        if (g_6ced38) {
            int x = (int)(CallVirt<int>(w, 20) - (unsigned)F(WORD, g_6ced38, 4)) / 2;
            int y = (int)(CallVirt<int>(w, 24) - (unsigned)F(WORD, g_6ced38, 6)) / 2;
            Gfx_DrawImage((CSurface*)s, x, y, g_6ced38, 0);
        }
        g_6ced3c = 1;
        g_6ced44 = g_69689c;
        return;
    }
    if (g_6ce4f4 && g_6ced3c) {
        g_6ced3c = 0;
        if (!g_6ced40) {
            if (g_6ce4f4)
                CallVirt(g_6ce4f4, 0, 1);
            g_6ce4f4 = 0;
            F(DWORD, g_world, 16) |= 3;
            Messages_Clear1276();
        }
        g_6ced40 = 0;
        g_6ced44 = g_69689c;
        return;
    }
    g_6ced44 = g_69689c;
}

// whether the game CD is in the drive
extern "C" int __cdecl CD_Check(void* screen)
{
    char buf[128];
    CallC(FUN_4e1320);
    GetPrivateProfileStringA("INSTALL", "SOURCE", ".", buf, 100, (const char*)g_696868);
    strcat(buf, "\\disk1");
    FILE* f = fopen(buf, "r");
    if (!f) {
        F(void*, g_6cecc0, 392) = screen;
        CallThis(FUN_519750, g_6cecc0, 0);
        return 0;
    }
    if (!Carat_True()) {
        if (screen) {
            CallVirt(screen, 32);
            CallThis(FUN_519750, g_6cecc0, 9);
        }
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

// a step of the loading bar
extern "C" int __cdecl Loading_Step()
{
    BYTE* w = g_69684c;
    if (!g_6ced64)
        return 0;
    CallThis(FUN_5b6bf0, F(void*, w, 12));
    CallThis(FUN_5b6ed0, F(void*, w, 12));
    int v = g_6ced50++;
    if (v < 0)
        v = 0;
    if (v > g_loadSteps)
        v = g_loadSteps;
    if (v % 10)
        return v / 10;
    int rect[4] = { 0, 0, 0, 0 };
    rect[2] = CallVirt<int>(w, 20);
    rect[3] = CallVirt<int>(w, 24);
    int x = (int)(CallVirt<int>(w, 20) - (unsigned)F(WORD, g_6ced5c, 20)) / 2;
    int y = (int)(CallVirt<int>(w, 24) - (unsigned)F(WORD, g_6ced5c, 22)) / 2;
    Carat_Draw5b86c0(F(int, w, 12), x, y, (int)g_6ced5c);
    rect[0] = -x;
    rect[1] = 0;
    rect[2] = CallVirt<int>(w, 20) * v / g_loadSteps + rect[0];
    rect[3] = F(WORD, g_6ced60, 22);
    CallC(FUN_5b81c0, F(int, w, 12), g_6ced60, rect, 0, y + 552);
    return CallThis<int>(FUN_5b6b10, F(void*, w, 12));
}

ASM_PROC(FUN_5dc230)
ASM_PROC(FUN_5dae90)
extern "C" void FUN_5c43c0(); KING_ALIAS("_FUN_5c43c0", "@Rec_Mark@8")
extern "C" void FUN_5c0cf0(); KING_ALIAS("_FUN_5c0cf0", "@List_Clear@8")
extern "C" void FUN_51d0e0(); KING_ALIAS("_FUN_51d0e0", "_Save_ListEntries")
extern "C" void FUN_5d8610(); KING_ALIAS("_FUN_5d8610", "@Win_Activate@8")
ASM_PROC(FUN_531000)
extern "C" void FUN_5fd160(); KING_ALIAS("_FUN_5fd160", "_Sound_Frame")
extern "C" void FUN_5fd3e0(); KING_ALIAS("_FUN_5fd3e0", "_Sound_StopAll")
extern "C" void FUN_5ffb20(); KING_ALIAS("_FUN_5ffb20", "_Obj723d44_Delete")
extern "C" void __fastcall Wasm_598d80(BYTE* self, void*);      // wasm.cpp
ASM_VAR(BYTE*, g_6cec90, $L_6cec90)
ASM_VAR(AsmLabel, g_6d19fa, $L_6d19fa)
ASM_VAR(AsmLabel, g_6d19ea, $L_6d19ea)
ASM_VAR(AsmLabel, g_6d19f6, $L_6d19f6)
ASM_VAR(AsmLabel, g_6d19fc, $L_6d19fc)
typedef BYTE* KeyPtrs[47];
ASM_VAR(KeyPtrs, g_keys6d1ec0, $L_6d1ec0)
ASM_VAR(KeyPtrs, g_keys6d1f7c, $L_6d1f7c)
ASM_VAR(BYTE*, g_6d2040, $L_6d2040)
ASM_VAR(BYTE*, g_6d203c, $L_6d203c)
ASM_VAR(BYTE*, g_6d2038, $L_6d2038)
ASM_VAR(int, g_6d2068, $L_6d2068)
ASM_VAR(AsmLabel, g_6d1bb0, $L_6d1bb0)          // 12-byte joystick records
ASM_VAR(void*, g_6cec88, $L_6cec88)
ASM_VAR(void*, g_6cec78, $L_6cec78)
ASM_VAR(void*, g_6cec74, $L_6cec74)
ASM_VAR(void*, g_6ceca0, $L_6ceca0)
ASM_VAR(void*, g_6cecbc, $L_6cecbc)
ASM_VAR(void*, g_6cecb0, $L_6cecb0)
ASM_VAR(void*, g_6cecac, $L_6cecac)
ASM_VAR(void*, g_6cecb4, $L_6cecb4)
ASM_VAR(void*, g_696cb0, $L_696cb0)
ASM_VAR(AsmLabel, g_6f68b0, $L_6f68b0)

// loads a model (name) of a directory, with the resource file of the detail
// level; returns what FUN_5dae90 returns (a caller writes through it)
extern "C" void* __cdecl Model_Load(const char* name, const char* dir)
{
    char a[128], b[128], c[128];
    strcpy(a, name);
    strcpy(b, dir);
    strcat(b, ".b3d");
    strcpy(c, dir);
    const char* suf = 0;
    switch (g_6ced70) {
    case 4: suf = ".res"; break;
    case 3:
    case 2: suf = strcmp(c, "trucks") ? ".res" : "1.res"; break;
    case 1: suf = strcmp(c, "trucks") ? "1.res" : "2.res"; break;
    case 0: suf = strcmp(c, "trucks") ? "2.res" : "3.res"; break;
    }
    if (suf)
        strcat(c, suf);
    void* m = CallThis<void*>(FUN_5dc230, g_world, a);
    return CallThis<void*>(FUN_5dae90, g_world, m, b, c);
}

// opens the replay file: recording (1) or playing (2, 3: the demo)
extern "C" void __cdecl Replay_Open()
{
    F(int, g_world, 7936) = 0;
    F(int, g_world, 7932) = 0;
    int mode = F(int, g_6cec6c, 1300);
    if (!mode)
        return;
    if (F(FILE*, g_world, 7928)) {
        fclose(F(FILE*, g_world, 7928));
        F(FILE*, g_world, 7928) = 0;
    }
    mode = F(int, g_6cec6c, 1300);
    char buf[64];
    if (mode == 1) {
        F(int, g_world, 7936) = 1;
        strcpy(buf, (const char*)g_6cec90 + 384);
        strcat(buf, ".tr");
        const char* n = Str_Trim(buf);
        if (!n || !*n)
            n = "tr.tr";
        F(FILE*, g_world, 7928) = fopen(n, "wb");
        CallThis(FUN_5c43c0, g_world);
        return;
    }
    if (mode <= 1)
        return;
    F(int, g_6cec6c, 1292) = 0;
    F(int, g_6cec6c, 1288) = 0;
    F(int, g_6cec6c, 1284) = 0;
    F(int, g_6cec6c, 1296) = 0;
    BYTE* o = F(BYTE*, F(BYTE*, g_6cec90, 380), 300);
    const char* s = o ? F(const char*, o, 12) : 0;
    const char* n;
    if (F(int, g_6cec6c, 1300) == 3 || !s || !*s)
        n = "demo.tr0";
    else {
        sprintf(buf, "%s.tr", s);
        n = buf;
    }
    F(FILE*, g_world, 7928) = fopen(n, "rb");
    F(int, g_world, 7932) = 1;
    F(double, g_world, 7960) = 1.0;
    F(int, g_world, 7940) = 0;
}

extern "C" void __fastcall Screen_SetPlayer(BYTE* self, void*, int a)
{
    F(int, self, 512) = a;
    CallThis(FUN_5c0cf0, F(void*, self, 500));
    CallC(FUN_51d0e0, g_6cecd8 + 632, F(void*, self, 500));
    if (!F(char, self, 372)) {
        BYTE* o = F(BYTE*, F(BYTE*, self, 500), 300);
        const char* s = o ? F(const char*, o, 12) : 0;
        if (!s)
            s = (const char*)Carat_Resource(103);
        strcpy((char*)self + 372, s);
    }
    CallThis(FUN_5d8610, self);
}

static int KeyIndex(const BYTE* p) { return p ? (int)(p - (const BYTE*)g_6d1948) >> 1 : -1; }
static int JoyIndex(const BYTE* p) { return (int)(p - (const BYTE*)g_6d1bb0) / 12; }

// the controls' key numbers of the keyboard layout for the options screen
extern "C" void __fastcall Keys_Assign(BYTE* self, void*)
{
    CallC(FUN_531000, F(int, self, 1280));
    if (g_696890 == 2)
        g_keys6d1ec0[6] = (BYTE*)g_6d19fa;
    else if (g_696890 == 3) {
        g_keys6d1ec0[5] = (BYTE*)g_6d19ea;
        g_keys6d1ec0[6] = (BYTE*)g_6d19f6;
        g_keys6d1ec0[38] = (BYTE*)g_6d19fc;
    }
    for (int i = 0; i < 47; i++) {
        F(int, self, 528 + i * 4) = KeyIndex(g_keys6d1ec0[i]);
        BYTE* q;
        if (i == 0) {
            q = g_6d2040;
            F(int, self, 716) = q ? F(int, q, 8) + (g_6d2068 + 150 + JoyIndex(q) * 2) * 2 : -1;
        } else if (i == 3) {
            q = g_6d203c;
            F(int, self, 728) = q ? (((g_6d2068 == 0) + (g_6d2068 + 150 + JoyIndex(q) * 2) * 2) ^ F(int, q, 8)) : -1;
        } else if (i == 1) {
            q = g_6d2038;
            F(int, self, 720) = q ? F(int, q, 8) + JoyIndex(q) * 4 + 300 : -1;
        } else if (i == 2) {
            q = g_6d2038;
            F(int, self, 724) = q ? (F(int, q, 8) ^ 1) + JoyIndex(q) * 4 + 300 : -1;
        } else
            F(int, self, 716 + i * 4) = KeyIndex(g_keys6d1f7c[i]);
    }
}

// plays a movie (from the CD's movie directory unless mode is 0, 1 or 6)
extern "C" int __cdecl Movie_Play(const char* name, int mode)
{
    if (!g_6cec88)
        return 0;
    char buf[128];
    GetPrivateProfileStringA("INSTALL", "movie", ".", buf, 100, (const char*)g_696868);
    if (mode > 1 && mode != 6)
        strcpy(buf, ".");
    strcat(buf, name);
    FILE* f = fopen(buf, "rb");
    if (!f)
        return 0;
    fclose(f);
    if (mode > 1 && mode != 6) {
        CallVirt(F(void*, g_world, 2856), 32);
        CallVirt(g_6cec78, 32);
        CallVirt(g_6cec74, 32);
        CallVirt(g_6ceca0, 32);
        CallVirt(g_world, 32);
        CallVirt(g_6cecbc, 32);
        CallVirt(g_6cecb0, 32);
        CallVirt(g_6cecac, 32);
        CallVirt(g_6cecb4, 32);
        Wasm_598d80((BYTE*)g_6f68b0, 0);
        CallVirt(g_696cb0, 32);
        CallC(FUN_5fd160);
        CallC(FUN_5fd160);
        CallC(FUN_5fd3e0);
        if (F(int, g_6cec6c, 1344))
            CallC(FUN_5ffb20);
    }
    Screen_OpenMovie(g_6cec88, 0, (int)buf, mode);
    return 1;
}

// --- functions with exception frames (the frames only rethrow)

static void Screen_DestroyWithChild(BYTE* self, void* vtbl, void* vtbl64)
{
    F(void*, self, 0) = vtbl;
    F(void*, self, 64) = vtbl64;
    BYTE* c = F(BYTE*, self, 368);
    if (c)
        CallVirt(c, 0, 1);
    CallThis(FUN_5d89e0, self);
}

typedef void (*StreamFn)();
static StreamFn StreamRead(BYTE* s) { return F(StreamFn, F(BYTE*, s, 0), 12); }

static BYTE* Image_New(int w, int h, int bpp)
{
    BYTE* o = (BYTE*)operator new(84);
    return o ? CallThis<BYTE*>(FUN_5ba4a0, o, w, h, bpp) : 0;
}

extern "C" {

// [FUN_50c010] grabs the screen into a new 16-bit image at *self
void __fastcall Screen_Grab(BYTE** self, void*)
{
    CallVirt(g_696ca0, 32);
    if (*self)
        CallVirt(*self, 0, 1);
    int w = g_screenW;
    int h = g_screenH;
    *self = Image_New(w, h, 2);
    int pitch;
    BYTE* src = CallC<BYTE*>(FUN_5d4200, &pitch);
    BYTE* dst = F(BYTE*, *self, 8);
    for (int y = h; y > 0; y--) {
        memcpy(dst, src, w * 2);
        dst += w * 2;
        src += pitch;
    }
    CallC(FUN_5d41e0, src);
    CallThis(FUN_5bae30, *self, 0);
}

// [FUN_50c300] reads an 8-bit image (w, h, then pixels) from the stream s
BYTE* __cdecl Image_ReadStream(BYTE* s, BYTE* hdr)
{
    memset(hdr, 0, 100);
    DWORD got;
    CallStd(StreamRead(s), s, hdr, 100, &got);
    BYTE* img = Image_New(hdr[0], hdr[1], 1);
    CallStd(StreamRead(s), s, F(BYTE*, img, 8), hdr[0] * hdr[1], &got);
    return img;
}

// [FUN_50c930] a new 12-byte record on the object of this name (depends on the mode)
DWORD* __cdecl Shell_NewObjRef()
{
    int mode = CallC<int>(FUN_568fd0);
    DWORD* r = (DWORD*)operator new(12);
    if (!r)
        return 0;
    r[0] = CallC<DWORD>(FUN_5ef3d0, mode == 2 ? (const char*)s_676248 : (const char*)s_676238);
    r[1] = 0;
    r[2] = 0;
    return r;
}

// [FUN_513680] a new 288-byte screen with four tables
BYTE* __stdcall Shell_NewScreenA(int a, int b)
{
    BYTE* o = (BYTE*)operator new(288);
    if (o) {
        CallThis(FUN_5bf820, o, a, 4096, b);
        F(void*, o, 0) = g_vtbl64dc1c;
    }
    CallThis(FUN_5bd5a0, o + 68, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, o + 104, (void*)g_6ce3b8);
    CallThis(FUN_5bd5a0, o + 140, (void*)g_6ceb38);
    CallThis(FUN_5bd5a0, o + 176, (void*)g_6ce988);
    CallVirt(o, 20);
    F(int, o, 52) = F(int, o, 44) + 180;
    return o;
}

// [FUN_517e00]
BYTE* __cdecl Shell_NewScreenB(int a)
{
    BYTE* o = (BYTE*)operator new(288);
    if (o) {
        CallThis(FUN_5bf750, o, a, 65536);
        F(void*, o, 0) = g_vtbl64dbe8;
    }
    for (int k = 68; k <= 176; k += 36)
        CallThis(FUN_5bd5a0, o + k, (void*)g_6ce430);
    return o;
}

// [FUN_5154c0] destructor
void __fastcall Shell5154c0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64de50;
    F(void*, self, 64) = g_vtbl64de10;
    if (F(int, self, 1344)) {
        CallThis(FUN_5ffb20, self);
        CallThis(FUN_5ffb40, self);
        F(int, self, 1344) = 0;
    }
    g_6cec6c = 0;
    CallThis(FUN_5d89e0, self);
}

// [FUN_517d40] destructor: saves the settings to the ini file
void __fastcall Shell517d40_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64e008;
    F(void*, self, 64) = g_vtbl64dfc8;
    const char* ini = g_iniPath;
    CallC(FUN_603c70, (const char*)s_672998, (const char*)s_676e94, (int)F(short, self, 390), ini);
    CallC(FUN_603c70, (const char*)s_672998, (const char*)s_676e9c, (int)F(short, self, 388), ini);
    WritePrivateProfileStringA(s_672998, s_676eb4, F(short, self, 394) ? s_672994 : s_6729a8, ini);
    CallThis(FUN_5d89e0, self);
}

// [FUN_51e000]
void __fastcall Shell51e000_Destroy(BYTE* self, void*) { Screen_DestroyWithChild(self, g_vtbl64e510, g_vtbl64e4d0); }

// [FUN_51e730]
void __fastcall Shell51e730_Destroy(BYTE* self, void*) { Screen_DestroyWithChild(self, g_vtbl64e598, g_vtbl64e558); }

// [FUN_51eca0]
void __fastcall Shell51eca0_Destroy(BYTE* self, void*) { Screen_DestroyWithChild(self, g_vtbl64e620, g_vtbl64e5e0); }

// [FUN_528c20]
BYTE* __fastcall Shell528c20_Construct(BYTE* self, void*, int a)
{
    CallThis(FUN_5d8710, self, a, (void*)g_720938, 0);
    CallThis(FUN_5c1260, self + 56);
    F(void*, self, 0) = g_vtbl64eb58;
    return self;
}

// [FUN_528fc0]
void __fastcall Shell528fc0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64eb58;
    CallThis(FUN_5c1b10, self ? self + 56 : 0);
    CallThis(FUN_5d8850, self);
}

}

// --- a three-state button's images: for each state (a, the common one, b)
// the normal (676338), pressed (67632c) and disabled (676320) images

static void Button_LoadImage(DWORD* style, const void* tmpl, char* buf, const char* fmt, const char* prefix,
                             const char* name)
{
    sprintf(buf, fmt, prefix, name);
    CallThis(FUN_5bd5a0, style, tmpl);
    style[7] = CallC<DWORD>(FUN_5ef3d0, buf);
}

extern "C" {

// [FUN_50dc00] sets the images of a button: states 0 (a), 1 (common), 2 (b)
void __cdecl Button_SetImages(BYTE* ctrl, const char* a, const char* b)
{
    DWORD on[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    DWORD off[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    char buf[32];
    const char* prefix = CallC<int>(FUN_568fd0) == 2 ? (const char*)s_676310 : (const char*)s_676308;
    const char* names[3] = {a, (const char*)s_676318, b};
    for (int state = 0; state < 3; state++) {
        Button_LoadImage(on, g_6ce3b8, buf, s_676338, prefix, names[state]);
        Button_LoadImage(off, g_6ce408, buf, s_67632c, prefix, names[state]);
        CallThis(FUN_5bef30, ctrl, on, off, on, off, state);
        Button_LoadImage(off, g_6ce408, buf, s_676320, prefix, names[state]);
        CallThis(FUN_5bef90, ctrl, off, state);
    }
    CallVirt(ctrl, 20);
}

}

extern "C" void FUN_601470(); KING_ALIAS("_FUN_601470", "@Gate_FreeBuffers@8")

extern "C" {

// [FUN_51aaf0] destructor: 8 items of 32 bytes at +552, last first
void __fastcall Shell51aaf0_Destroy(BYTE* self, void*)
{
    for (int i = 7; i >= 0; i--)
        CallThis(FUN_601470, self + 552 + i * 32);
    CallThis(FUN_5d89e0, self);
}

}

// ---------------------------------------------------------------------------
// x87 functions of the unit

ASM_VAR(BYTE*, g_game511910, $L_6d2078)
extern "C" double __fastcall Ramp_High(BYTE* self, void*);       // vehicle_physics.cpp (the value stays in ST(0))
extern "C" double __fastcall Ramp_Position(BYTE* self, void*);
extern "C" int* __cdecl Player_CarId(int* out);                  // helicopter.cpp
extern "C" double __cdecl Driver_CargoFactor(int car);           // ai_drivers.cpp
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")    // a vehicle's flags by id

// [FUN_511910] the player truck's load factor: weight, cargo and the pedal over 0.7
extern "C" double __cdecl Player_LoadFactor(double base)
{
    BYTE* car = F(BYTE*, g_game511910, 616);
    double v = CallVirt<double>(car, 84);
    double hi = Ramp_High(F(BYTE*, car, 21600) + 16112, 0);
    car = F(BYTE*, g_game511910, 616);
    double t = 1.0f - Ramp_Position(F(BYTE*, car, 21600) + 16112, 0);
    base = v * 5000.0 + base;
    int id;
    Player_CarId(&id);
    double r;
    if (CallC<int>(FUN_43f620, id) & 0x2000000) {
        Player_CarId(&id);
        r = Driver_CargoFactor(id) + base;
    } else
        r = base;
    if (t > 0.7)
        r += (t - 0.7) * hi + (t - 0.7) * hi;
    return r;
}

extern "C" void FUN_513820(); KING_ALIAS("_FUN_513820", "_Shell_ReadTsc")    // the time stamp counter
ASM_VAR(const double, k_0_0025, $L_64de08)
ASM_VAR(char, g_strMhz, $L_67695c)       // "MHZ="
extern "C" int __cdecl Get71e7e4();      // core.cpp

// [FUN_513740] the processor speed as 0..1 (300..700 MHz), measured over 100 ms
extern "C" double __cdecl Cpu_SpeedFactor()
{
    if ((Get71e7e4() & 255) < 5)
        return 0.0;
    double end = Timer_Now();
    __int64 c0 = CallC<__int64>(FUN_513820);
    end += 100.0;
    while (!(Timer_Now() >= end))
        ;
    __int64 mhz = (CallC<__int64>(FUN_513820) - c0) / 100000;
    Carat_Message(&g_strMhz, (int)mhz);
    if (mhz < 300)
        return 0.0;
    if (mhz > 700)
        return 1.0;
    return (double)(mhz - 300) * k_0_0025;
}

ASM_PROC(FUN_5d89e0)    // the screen base destructor
ASM_VAR(char, vt_64ddc0, $L_64ddc0)
ASM_VAR(char, vt_64dd80, $L_64dd80)
ASM_VAR(char, vt_64e6a8, $L_64e6a8)
ASM_VAR(char, vt_64e668, $L_64e668)
ASM_VAR(char, vt_64e730, $L_64e730)
ASM_VAR(char, vt_64e6f0, $L_64e6f0)
ASM_VAR(char, vt_64e7c0, $L_64e7c0)
ASM_VAR(char, vt_64e780, $L_64e780)
ASM_VAR(char, vt_64e960, $L_64e960)
ASM_VAR(char, vt_64e920, $L_64e920)
ASM_VAR(char, vt_64e9e8, $L_64e9e8)
ASM_VAR(char, vt_64e9a8, $L_64e9a8)
ASM_VAR(char, vt_64ea70, $L_64ea70)
ASM_VAR(char, vt_64ea30, $L_64ea30)

// [FUN_511800] destructor of a screen class (vtables 64ddc0, 64dd80 at +64)
extern "C" void __fastcall Screen511800_Destroy(void** self, int)
{
    self[0] = &vt_64ddc0;
    self[16] = &vt_64dd80;
    CallThis(FUN_5d89e0, self);
}

// [FUN_51f360] destructor of a screen class (vtables 64e6a8, 64e668 at +64)
extern "C" void __fastcall Screen51f360_Destroy(void** self, int)
{
    self[0] = &vt_64e6a8;
    self[16] = &vt_64e668;
    CallThis(FUN_5d89e0, self);
}

// [FUN_520580] destructor of a screen class (vtables 64e730, 64e6f0 at +64)
extern "C" void __fastcall Screen520580_Destroy(void** self, int)
{
    self[0] = &vt_64e730;
    self[16] = &vt_64e6f0;
    CallThis(FUN_5d89e0, self);
}

// [FUN_521410] destructor of a screen class (vtables 64e7c0, 64e780 at +64)
extern "C" void __fastcall Screen521410_Destroy(void** self, int)
{
    self[0] = &vt_64e7c0;
    self[16] = &vt_64e780;
    CallThis(FUN_5d89e0, self);
}

// [FUN_524510] destructor of a screen class (vtables 64e960, 64e920 at +64)
extern "C" void __fastcall Screen524510_Destroy(void** self, int)
{
    self[0] = &vt_64e960;
    self[16] = &vt_64e920;
    CallThis(FUN_5d89e0, self);
}

// [FUN_525e20] destructor of a screen class (vtables 64e9e8, 64e9a8 at +64)
extern "C" void __fastcall Screen525e20_Destroy(void** self, int)
{
    self[0] = &vt_64e9e8;
    self[16] = &vt_64e9a8;
    CallThis(FUN_5d89e0, self);
}

// [FUN_526ba0] destructor of a screen class (vtables 64ea70, 64ea30 at +64)
extern "C" void __fastcall Screen526ba0_Destroy(void** self, int)
{
    self[0] = &vt_64ea70;
    self[16] = &vt_64ea30;
    CallThis(FUN_5d89e0, self);
}

ASM_VAR(char, vt_64dc84, $L_64dc84)
ASM_VAR(char, vt_64d81c, $L_64d81c)

// [FUN_50d190] does nothing (an empty destructor)
extern "C" void __fastcall Shell_Nop(void*, int)
{
}

// [FUN_50d510] destructor: frees the buffer at +12
extern "C" void __fastcall ShellBuf_Destroy(void** self, int)
{
    self[0] = &vt_64dc84;
    operator delete(self[3]);
    self[0] = &vt_64d81c;
}

// [FUN_513820] the time stamp counter, serialized by cpuid before and after
extern "C" unsigned __int64 __cdecl Shell_ReadTsc()
{
    int r[4];
    __cpuid(r, 0);
    unsigned __int64 t = __rdtsc();
    __cpuid(r, 0);
    return t;
}

ASM_VAR(BYTE*, g_shell6cec6c, $L_6cec6c)
extern "C" void FUN_50bfe0(); KING_ALIAS("_FUN_50bfe0", "_Carat_HasMovie")
extern "C" void FUN_50c100(); KING_ALIAS("_FUN_50c100", "_Carat_CloseMovie")
extern "C" void FUN_4e12b0(); KING_ALIAS("_FUN_4e12b0", "_Joy_ForceFeedback")

// [FUN_50c150] stops a pending movie request: clears the three flags, closes the movie, resets force feedback
extern "C" int __cdecl Shell_StopMovieRequest()
{
    BYTE* s = g_shell6cec6c;
    if (!F(int, s, 1296) && !F(int, s, 1284) && !F(int, s, 1288))
        return 0;
    if (!CallC<int>(FUN_50bfe0))
        return 0;
    F(int, g_shell6cec6c, 1288) = 0;
    F(int, g_shell6cec6c, 1284) = 0;
    F(int, g_shell6cec6c, 1296) = 0;
    CallC(FUN_50c100);
    CallC(FUN_4e12b0, 1);
    return 1;
}

ASM_VAR(int, g_6ced50, $L_6ced50)
ASM_VAR(int, g_frameMs675468, $L_675468)
ASM_VAR(BYTE, g_6f2d18, $L_6f2d18)
ASM_VAR(BYTE*, g_6cec98s, $L_6cec98)
ASM_VAR(BYTE*, g_obj6ced5c, $L_6ced5c)
ASM_VAR(BYTE*, g_obj6ced60, $L_6ced60)
ASM_VAR(char, s_obj67667c, $L_67667c)
ASM_VAR(char, s_obj676668, $L_676668)
ASM_VAR(char, s_obj676658, $L_676658)
ASM_VAR(char, s_obj676644, $L_676644)
extern "C" void FUN_568fd0(); KING_ALIAS("_FUN_568fd0", "_Game_568fd0")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")    // finds a scene object by name
ASM_PROC(FUN_5bae30)

// [FUN_5105b0] sets the frame time limit and finds the two objects of the game mode, showing them
extern "C" void __cdecl Shell_SetupModeObjects()
{
    BYTE* s = g_shell6cec6c;
    g_6ced50 = 0;
    if (F(int, s, 1300) > 1)
        g_frameMs675468 = 900;
    else if (!F(int, s, 1316))
        g_frameMs675468 = g_6f2d18 ? 3550 : 4000;
    else
        g_frameMs675468 = F(int, g_6cec98s, 384) >= 2 ? 900 : 970;
    if (CallC<int>(FUN_568fd0) == 2) {
        g_obj6ced5c = CallC<BYTE*>(FUN_5ef3d0, &s_obj67667c);
        g_obj6ced60 = CallC<BYTE*>(FUN_5ef3d0, &s_obj676668);
    } else if (CallC<int>(FUN_568fd0) == 3) {
        g_obj6ced5c = CallC<BYTE*>(FUN_5ef3d0, &s_obj676658);
        g_obj6ced60 = CallC<BYTE*>(FUN_5ef3d0, &s_obj676644);
    }
    if (!(F(BYTE, g_obj6ced5c, 52) & 2))
        CallThis(FUN_5bae30, g_obj6ced5c, 0);
    if (!(F(BYTE, g_obj6ced60, 52) & 2))
        CallThis(FUN_5bae30, g_obj6ced60, 0);
}

ASM_VAR(BYTE*, g_window69684c, $L_69684c)
ASM_VAR(BYTE*, g_game696ca0, $L_696ca0)
ASM_VAR(void*, g_vt6cecbc, $L_6cecbc)
ASM_VAR(void*, g_vt6cec80, $L_6cec80)
ASM_VAR(int, g_music6968f0, $L_6968f0)
ASM_VAR(char, g_music696750, $L_696750)
ASM_VAR(char, g_6f68b0s, $L_6f68b0)
extern "C" void FUN_5fcd40(); KING_ALIAS("_FUN_5fcd40", "_Sound_SetVolume")
extern "C" void FUN_5ffb20(); KING_ALIAS("_FUN_5ffb20", "_Obj723d44_Delete")
ASM_PROC(FUN_5b6bf0)
extern "C" void FUN_5d8630(); KING_ALIAS("_FUN_5d8630", "@Window_Show@12")
extern "C" void FUN_5d88c0(); KING_ALIAS("_FUN_5d88c0", "@Window_Refresh@8")
extern "C" void FUN_598d80(); KING_ALIAS("_FUN_598d80", "@Wasm_598d80@8")
extern "C" void FUN_5c4340(); KING_ALIAS("_FUN_5c4340", "@Rec_Close@8")
extern "C" void FUN_5c3a80(); KING_ALIAS("_FUN_5c3a80", "@Music_Stop@8")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_42db90(); KING_ALIAS("_FUN_42db90", "_AI_FreeList68b99c")
extern "C" void FUN_435a80(); KING_ALIAS("_FUN_435a80", "_AI_Reset")
extern "C" void FUN_602df0(); KING_ALIAS("_FUN_602df0", "@DpMessage_Reset@8")
extern "C" void FUN_512c70(); KING_ALIAS("_FUN_512c70", "_Shell_LeaveGameScreens")

// [FUN_511820] leaves a race: silences the sound, clears the window, resets the world, the AI and the network messages
extern "C" int __cdecl Shell_LeaveRace()
{
    CallC<double>(FUN_5fcd40, 0);
    BYTE* o = F(BYTE*, g_shell6cec6c, 1344);
    if (o)
        CallThis(FUN_5ffb20, o);
    CallThis(FUN_5b6bf0, F(BYTE*, g_window69684c, 12));
    CallThis(FUN_5d8630, g_window69684c, 0);
    CallThis(FUN_5d88c0, g_window69684c);
    CallThis(FUN_598d80, &g_6f68b0s);
    CallVirt(F(void*, g_game696ca0, 2856), 32);
    CallVirt(g_vt6cecbc, 32);
    CallThis(FUN_5c4340, g_game696ca0);
    CallVirt(g_game696ca0, 144);
    CallVirt(g_game696ca0, 32);
    CallVirt(g_vt6cec80, 36);
    if (F(int, g_shell6cec6c, 1316) == 1)
        CallC(FUN_5e48f0);
    if (g_music6968f0)
        CallThis(FUN_5c3a80, &g_music696750);
    CallC(FUN_42db90);
    CallC(FUN_435a80);
    CallThis(FUN_602df0, g_game696ca0 + 8336);
    return CallC<int>(FUN_512c70);
}

extern "C" void FUN_51bfa0(); KING_ALIAS("_FUN_51bfa0", "_Save_OpenStorage@8")    // opens the save storage (path, mode), stdcall
extern "C" void FUN_50bc90(); KING_ALIAS("_FUN_50bc90", "_Str_ToWide")    // a temporary wide string of a name
extern "C" void FUN_513270(); KING_ALIAS("_FUN_513270", "_Save_FindNamedStream")    // opens a stream of a storage (storage, name, mode)
extern "C" void FUN_513850(); KING_ALIAS("_FUN_513850", "@Keys_Assign@8")
extern "C" void FUN_513600(); KING_ALIAS("_FUN_513600", "@Settings_Save@8")
ASM_PROC(FUN_5147b0)
ASM_VAR(char, s_stream676958, $L_676958)
ASM_VAR(char, s_stream6d1db8, $L_6d1db8)

// a COM method at vtbl+off
template <class R = long, class... A>
static R ComCall(void* p, int off, A... a)
{
    return ((R(__stdcall*)(void*, A...))(*(void***)p)[off / 4])(p, a...);
}

// [FUN_5134b0] loads the 376-byte settings at +528 from the save storage when its version stream says 4; otherwise resets them and writes version 4
extern "C" void __fastcall Settings_Load(BYTE* self, int)
{
    void* stg = CallStd<void*>(FUN_51bfa0, g_6cecd8 + 632, 1);
    void* s = 0;
    int version = 0;
    ULONG done;
    ComCall(stg, 16, CallC<const WCHAR*>(FUN_50bc90, &s_stream676958), 0, 16, 0, &s);
    if (s) {
        ComCall(s, 12, &version, 4, &done);
        ComCall(s, 8);
    }
    if (version > 0 && version == 4) {
        s = CallC<void*>(FUN_513270, stg, &s_stream6d1db8, 1);
        if (!s) {
            ComCall(stg, 8);
            CallThis(FUN_513850, self);
            CallThis(FUN_513600, self);
            return;
        }
        ComCall(s, 12, self + 528, 376, &done);
    } else {
        ComCall(stg, 8);
        CallThis(FUN_513850, self);
        CallThis(FUN_513600, self);
        stg = CallStd<void*>(FUN_51bfa0, g_6cecd8 + 632, 2);
        ComCall(stg, 12, CallC<const WCHAR*>(FUN_50bc90, &s_stream676958), 4113, 0, 0, &s);
        version = 4;
        ComCall(s, 16, &version, 4, &done);
    }
    ComCall(s, 8);
    ComCall(stg, 8);
    CallThis(FUN_5147b0, self);
}

extern "C" void FUN_476930(); KING_ALIAS("_FUN_476930", "_Player_GetState")    // a player's state: 19 records {value, -, double} (id, records)
ASM_PROC(FUN_4eb4a0)    // sets a vehicle setting (value)
ASM_VAR(int, g_noVehicle68b990s, $L_68b990)

// [FUN_520000] applies a player's 19 vehicle settings to the vehicle
extern "C" void __cdecl Car_ApplyPlayerSettings(BYTE* car)
{
    struct Rec { int value, pad; double x; } r[19];
    for (int i = 0; i < 19; i++) {
        r[i].x = 1.0;
        r[i].value = 0;
    }
    int id = F(int, car, 20636);
    if (id == g_noVehicle68b990s)
        return;
    CallC(FUN_476930, id, r);
    for (int i = 0; i < 20; i++) {
        switch (i) {
        case 0:  CallThis(FUN_4eb4a0, car + 21204, r[3].value); break;
        case 1:  CallThis(FUN_4eb4a0, F(BYTE*, car, 21600) + 16216, r[4].value); break;
        case 2:  CallThis(FUN_4eb4a0, F(BYTE*, car, 21600) + 16372, r[5].value); break;
        case 3:  CallThis(FUN_4eb4a0, car + 21152, r[6].value); break;
        case 4:  CallThis(FUN_4eb4a0, car + 21256, r[7].value); break;
        case 5:  CallThis(FUN_4eb4a0, car + 21516, r[17].value); break;
        case 6:  CallThis(FUN_4eb4a0, F(BYTE*, car, 21600) + 16060, r[0].value); break;
        case 7:  CallThis(FUN_4eb4a0, F(BYTE*, car, 21600) + 16320, r[1].value); break;
        case 9:
            if (r[9].value == 1)
                CallThis(FUN_4eb4a0, F(BYTE*, car, 21600) + 16164, 1);
            else if (r[9].value != 2)
                CallThis(FUN_4eb4a0, F(BYTE*, car, 21600) + 16164, 0);
            break;
        case 10:
            if (r[9].value == 2)
                CallThis(FUN_4eb4a0, F(BYTE*, car, 21600) + 16164, 2);
            else if (r[9].value != 1)
                CallThis(FUN_4eb4a0, F(BYTE*, car, 21600) + 16164, 0);
            break;
        case 11: CallThis(FUN_4eb4a0, F(BYTE*, car, 21600) + 16112, r[13].value); break;
        case 12: CallThis(FUN_4eb4a0, car + 21412, r[2].value); break;
        case 13: CallThis(FUN_4eb4a0, car + 21464, r[15].value); break;
        case 16: CallThis(FUN_4eb4a0, car + 21360, r[12].value); break;
        case 18: CallThis(FUN_4eb4a0, car + 21308, r[11].value); break;
        }
    }
}

ASM_VAR(BYTE*, g_6cec94, $L_6cec94)
typedef const char* StrTable[1];
ASM_VAR(StrTable, g_names675ae0, $L_675ae0)
ASM_VAR(AsmLabel, s_fmt67254c, $L_67254c)
ASM_VAR(AsmLabel, g_text6ce9ac, $L_6ce9ac)
ASM_VAR(AsmLabel, s_stream677574, $L_677574)

// [FUN_51a4a0] the current screen's title: the name of its kind and its number from 1
extern "C" char* __cdecl Shell_ScreenTitle()
{
    sprintf(g_text6ce9ac, s_fmt67254c, g_names675ae0[F(int, g_6cec94, 808)], F(int, g_6cec94, 812) + 1);
    return g_text6ce9ac;
}

// [FUN_51bd50] opens the save storage (mode) and its stream; the storage goes to *stg, 0 if it can't be opened
extern "C" void* __cdecl Storage_OpenStream(void** stg, int mode)
{
    void* st = CallStd<void*>(FUN_51bfa0, g_6cecd8 + 632, mode);
    if (!st)
        return 0;
    void* s = 0;
    ComCall(st, 16, CallC<const WCHAR*>(FUN_50bc90, s_stream677574), 0, mode != 1 ? 17 : 16, 0, &s);
    *stg = st;
    return s;
}

// [FUN_513600] writes the 376-byte settings at +528 to the save storage
extern "C" void __fastcall Settings_Save(BYTE* self, void*)
{
    void* stg = CallStd<void*>(FUN_51bfa0, g_6cecd8 + 632, 2);
    void* s = CallC<void*>(FUN_513270, stg, &s_stream6d1db8, 2);
    if (!s)
        s = CallC<void*>(FUN_513270, stg, &s_stream6d1db8, 0);
    ULONG done;
    ComCall(s, 16, self + 528, 376, &done);
    ComCall(s, 8);
    ComCall(stg, 8);
    CallThis(FUN_5147b0, self);
}

// [FUN_51e7a0] clears the list at +504, saves it, and enables the button at +500 when the list's item has data
extern "C" void __fastcall Shell51e7a0_Apply(BYTE* self, void*)
{
    CallThis(FUN_5c0cf0, F(void*, self, 504));
    CallC(FUN_51d0e0, g_6cecd8 + 632, F(void*, self, 504));
    if (F(int, g_6cec6c, 1316))
        F(int, F(BYTE*, self, 500), 40) = 1;
    else {
        BYTE* x = F(BYTE*, F(BYTE*, self, 504), 300);
        F(int, F(BYTE*, self, 500), 40) = x && F(int, x, 12) ? 0 : 1;
    }
    CallThis(FUN_5d8610, self);
}

ASM_VAR(AsmLabel, s_fmt6772a8, $L_6772a8)
ASM_VAR(AsmLabel, s_stream6772a0, $L_6772a0)
extern "C" HRESULT __stdcall StgOpenStorage(const WCHAR* name, void* prio, DWORD mode, void* excl, DWORD reserved, void** stg);

// [FUN_518760] reads the image of a storage file (name formatted into the path) from its stream
extern "C" BYTE* __cdecl Storage_ReadImage(const char* name, BYTE* arg)
{
    char path[64];
    void* stg = 0;
    void* s = 0;
    sprintf(path, s_fmt6772a8, name);
    StgOpenStorage(CallC<const WCHAR*>(FUN_50bc90, path), 0, 16, 0, 0, &stg);
    if (!stg)
        return 0;
    ComCall(stg, 16, CallC<const WCHAR*>(FUN_50bc90, s_stream6772a0), 0, 16, 0, &s);
    if (!s) {
        ComCall(stg, 8);
        return 0;
    }
    BYTE* r = Image_ReadStream((BYTE*)s, arg);
    ComCall(s, 8);
    ComCall(stg, 8);
    return r;
}

extern "C" void FUN_50bbf0(); KING_ALIAS("_FUN_50bbf0", "_Str_Trim")
extern "C" void FUN_5ba3d0(); KING_ALIAS("_FUN_5ba3d0", "@Palette_Apply@8")
ASM_VAR(int, g_696878, $L_696878)
ASM_VAR(AsmLabel, s_672998, $L_672998)
ASM_VAR(AsmLabel, s_6767d0, $L_6767d0)

// [FUN_527de0] checks that the save storage opens; if not, clears the save flag in the ini and the options; then reloads the settings and the palette
extern "C" void __cdecl Shell_CheckSave()
{
    char name[36];
    strcpy(name, (const char*)(g_6cecd8 + 632));
    void* stg = CallStd<void*>(FUN_51bfa0, CallC<void*>(FUN_50bbf0, name), 1);
    if (stg)
        ComCall(stg, 8);
    else {
        F(int, g_6cec6c, 1280) = 0;
        CallC(FUN_603c70, s_672998, s_6767d0, 0, g_696868);
        CallVirt(g_6cecd8 + 64, 60, 1);
    }
    Settings_Load(g_6cec6c, 0);
    g_696878 = 0;
    CallThis(FUN_5ba3d0, F(void*, F(BYTE*, F(BYTE*, g_696ca0, 9396), 100), 0));
}

ASM_VAR(AsmLabel, s_storage67683c, $L_67683c)

// [FUN_51c870] reads the image of the sub-storage name (inside 67683c of the save storage) from its stream
extern "C" BYTE* __cdecl Storage_ReadSubImage(const char* name, BYTE* arg)
{
    void* stg = CallStd<void*>(FUN_51bfa0, g_6cecd8 + 632, 1);
    if (!stg)
        return 0;
    void* a = 0;
    void* b = 0;
    void* s = 0;
    ComCall(stg, 24, CallC<const WCHAR*>(FUN_50bc90, s_storage67683c), 0, 16, 0, 0, &a);
    if (a) {
        ComCall(a, 24, CallC<const WCHAR*>(FUN_50bc90, name), 0, 16, 0, 0, &b);
        if (b) {
            ComCall(b, 16, CallC<const WCHAR*>(FUN_50bc90, s_stream6772a0), 0, 16, 0, &s);
            if (s) {
                BYTE* r = Image_ReadStream((BYTE*)s, arg);
                ComCall(s, 8);
                ComCall(b, 8);
                ComCall(stg, 8);
                ComCall(a, 8);
                return r;
            }
            ComCall(b, 8);
        }
        ComCall(a, 8);
    }
    ComCall(stg, 8);
    return 0;
}

// ---------------------------------------------------------------------------
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")
extern "C" void FUN_5bdb50(); KING_ALIAS("_FUN_5bdb50", "@Ctl_FindId@12")
ASM_PROC(FUN_4d90c0)
ASM_PROC(FUN_4833a0)    // writes the save into a storage
ASM_PROC(FUN_51d6d0)    // writes the screenshot (storage, a)
ASM_VAR(AsmLabel, s_671204, $L_671204)
ASM_VAR(AsmLabel, s_677588, $L_677588)
ASM_VAR(AsmLabel, s_676848, $L_676848)
ASM_VAR(AsmLabel, s_677764, $L_677764)
ASM_VAR(AsmLabel, s_67773c, $L_67773c)
ASM_VAR(AsmLabel, s_6773e0, $L_6773e0)
ASM_VAR(AsmLabel, s_676cac, $L_676cac)
ASM_VAR(AsmLabel, s_fmtInt66c8cc, $L_66c8cc)
ASM_VAR(AsmLabel, g_carTypes698166, $L_698166)
ASM_VAR(AsmLabel, g_carNames675b0c, $L_675b0c)
ASM_VAR(AsmLabel, g_keyNames675470, $L_675470)
ASM_VAR(int, g_6d206c, $L_6d206c)
extern "C" void* g_impStgCreateDocfile; KING_ALIAS("_g_impStgCreateDocfile", "__imp__StgCreateDocfile@16")
extern "C" void* g_impStgOpenStorage; KING_ALIAS("_g_impStgOpenStorage", "__imp__StgOpenStorage@24")
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// [FUN_51bfa0..$L_51c06c] opens (mode 1 read, 2 read/write) or creates (mode 0) the storage file "<dir><name>.pl1"
extern "C" void* __stdcall Save_OpenStorage(const char* name, int mode)
{
    char path[40];
    void* stg = 0;
    strcpy(path, s_671204);
    strcat(path, name);
    strcat(path, ".pl1");
    if (!mode)
    {
        if (CallStd<long>((void (*)())g_impStgCreateDocfile, CallC<const WCHAR*>(FUN_50bc90, path), 4113, 0, &stg))
            Sys_Fatal(s_677588, 0);
        return stg;
    }
    DWORD flags = (mode != 1 ? 2 : 0) | 16;
    CallStd<long>((void (*)())g_impStgOpenStorage, CallC<const WCHAR*>(FUN_50bc90, path), (void*)0, flags, (void*)0, 0, &stg);
    return stg;
}

// [FUN_51a4e0..$L_51a570] the index of the chosen car type among the loaded ones; fatal when it isn't loaded
extern "C" int __cdecl Shell_CarTypeIndex()
{
    const char* want = ((const char**)g_carNames675b0c)[F(int, g_6cec94, 808)];
    for (int i = 0; i < g_6ce32c; i++)
        if (!strcmp(want, (char*)g_carTypes698166 + i * 2416))
            return i;
    Sys_Fatal(s_6773e0, ((const char**)g_carNames675b0c)[F(int, g_6cec94, 808)]);
    return 0;
}

// [FUN_51d9e0..$L_51da45] deletes the element name of the save storage's sub-storage 67683c
extern "C" void __cdecl Save_DeleteEntry(const char* name)
{
    CallC(FUN_4d90c0);
    void* stg = Save_OpenStorage((char*)g_6cecd8 + 632, 2);
    if (!stg)
        Sys_Fatal(s_676848, 0);
    void* sub = 0;
    ComCall(stg, 24, CallC<const WCHAR*>(FUN_50bc90, (char*)s_storage67683c), (void*)0, 17, (void*)0, 0, &sub);
    if (!sub)
        Sys_Fatal(s_677764, 0);
    ComCall(sub, 48, CallC<const WCHAR*>(FUN_50bc90, name));
    ComCall(sub, 8);
    ComCall(stg, 8);
}

// [FUN_51d8f0..$L_51d999] creates the element name in the save storage's sub-storage 67683c and writes the save and screenshot into it
extern "C" void __cdecl Save_WriteEntry(const char* name, int a)
{
    CallC(FUN_4d90c0);
    void* stg = Save_OpenStorage((char*)g_6cecd8 + 632, 2);
    if (!stg)
        Sys_Fatal(s_676848, 0);
    void* sub = 0;
    ComCall(stg, 24, CallC<const WCHAR*>(FUN_50bc90, (char*)s_storage67683c), (void*)0, 17, (void*)0, 0, &sub);
    if (!sub)
        Sys_Fatal(s_677764, 0);
    void* entry = 0;
    ComCall(sub, 20, CallC<const WCHAR*>(FUN_50bc90, name), 4113, 0, 0, &entry);
    if (!entry)
        Sys_Fatal(s_67773c, 0);
    CallC(FUN_4833a0, entry);
    CallC(FUN_51d6d0, entry, a);
    ComCall(entry, 8);
    ComCall(sub, 8);
    ComCall(stg, 8);
}

// [FUN_512460..$L_5124ad] when the save storage holds the record named by text 103: copies that name into 6f2d18, closes the record and resets
extern "C" void __cdecl Save_FindRecord103()
{
    if (F(int, g_6cec6c, 1316))
        return;
    void* stg = Save_OpenStorage((char*)g_6cecd8 + 632, 1);
    if (!stg)
        Sys_Fatal(s_676848, 0);
    void* sub = 0;
    void* rec = 0;
    ComCall(stg, 24, CallC<const WCHAR*>(FUN_50bc90, (char*)s_storage67683c), (void*)0, 16, (void*)0, 0, &sub);
    if (!sub)
    {
        ComCall(stg, 8);
        return;
    }
    ComCall(sub, 24, CallC<const WCHAR*>(FUN_50bc90, CallC<const char*>(FUN_5d2e50, 103)), (void*)0, 16, (void*)0, 0, &rec);
    if (!rec)
    {
        ComCall(sub, 8);
        ComCall(stg, 8);
        return;
    }
    sprintf((char*)&g_6f2d18, CallC<const char*>(FUN_5d2e50, 103));
    ComCall(rec, 8);
    ComCall(sub, 8);
    ComCall(stg, 8);
    CallThis(FUN_5c4340, g_696ca0);
    F(int, g_696ca0, 3024) = 1;
    if (F(int, g_6cec6c, 1344))
        CallC(FUN_5ffb20);
}

// [FUN_515270..$L_51532b] binds key to the control being assigned (+516) unless that key is taken by another control; returns -1 bound, -2 refused, or the clashing control
extern "C" int __fastcall Keys_Bind(BYTE* self, void*, int key)
{
    if (F(int, self, 516) < 0 || !((int*)g_keyNames675470)[key])
        return -2;
    if (g_6d206c == 1 && ((key >= 291 && key <= 294) || key == 259 || key == 260 || key == 264 || key == 265))
        return -2;
    BYTE* ctl = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, self, 524), F(int, self, 516) + 1000);
    if (!ctl)
    {
        char text[100];
        sprintf(text, s_fmtInt66c8cc, F(int, self, 516));
        Sys_Fatal(s_676cac, text);
    }
    int old = F(int, self, 716 + F(int, self, 516) * 4);
    for (int i = 0; i < 47; i++)
    {
        int* second = (int*)(self + 716) + i;
        if (second[-47] == key || (old >= 0 && *second == old) || *second == key)
            if (i != F(int, self, 516) && !CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, self, 524), i + 1000))
                return i;
    }
    if (key < 259)
        F(int, self, 528 + F(int, self, 516) * 4) = key;
    else
        F(int, self, 716 + F(int, self, 516) * 4) = key;
    F(DWORD, ctl, 284) &= ~2u;
    F(int, self, 516) = -1;
    return -1;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_51bfa0(); KING_ALIAS("_FUN_51bfa0", "_Save_OpenStorage@8")
extern "C" void FUN_50bcc0(); KING_ALIAS("_FUN_50bcc0", "_Str_FromWide")
extern "C" void FUN_5c1140(); KING_ALIAS("_FUN_5c1140", "@List_AddNamed@12")
extern "C" void FUN_603b70(); KING_ALIAS("_FUN_603b70", "_Ini_ReadInt")
extern "C" void FUN_603c70(); KING_ALIAS("_FUN_603c70", "_Ini_WriteInt")
extern "C" void FUN_4ccf10(); KING_ALIAS("_FUN_4ccf10", "@Lights_Destruct@8")
extern "C" void FUN_5bdb50(); KING_ALIAS("_FUN_5bdb50", "@Ctl_FindId@12")
extern "C" void FUN_5bc2f0(); KING_ALIAS("_FUN_5bc2f0", "@Poly_Destroy@8")
extern "C" void FUN_5db9f0(); KING_ALIAS("_FUN_5db9f0", "@Events_Put@12")
extern "C" void FUN_50bd80(); KING_ALIAS("_FUN_50bd80", "_Carat_ScreenFlags")
extern "C" void FUN_5d8630(); KING_ALIAS("_FUN_5d8630", "@Window_Show@12")
extern "C" void FUN_5d88c0(); KING_ALIAS("_FUN_5d88c0", "@Window_Refresh@8")
extern "C" void FUN_5ba3d0(); KING_ALIAS("_FUN_5ba3d0", "@Palette_Apply@8")
extern "C" void FUN_527ea0(); KING_ALIAS("_FUN_527ea0", "_Carat_SetView")
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")    // log (text, value)
extern "C" __declspec(dllimport) void __stdcall CoTaskMemFree(void* p);
struct GsStatStg { WCHAR* pwcsName; DWORD rest[17]; };    // STATSTG
ASM_PROC(FUN_5b6bf0)    // surface: prepare
extern "C" void FUN_5d8460(); KING_ALIAS("_FUN_5d8460", "_Dd_ChooseTextureFormat")
ASM_PROC($L_5d7b90)     // the video mode dialog (8 args)
ASM_PROC($L_5d7c20)     // tries a video mode (w, h, cres, flags, 3 outs)
ASM_PROC($L_5dd8c0)     // window: set the video mode (w, h, cres, flags)
ASM_VAR(BYTE*, g_gs6cecd8, $L_6cecd8)
ASM_VAR(AsmLabel, s_gs677670, $L_677670)
ASM_VAR(AsmLabel, s_gs67683c, $L_67683c)
ASM_VAR(AsmLabel, s_gs677658, $L_677658)
ASM_VAR(AsmLabel, s_gs676ce4, $L_676ce4)
ASM_VAR(const char*, g_gsIni, $L_696868)
ASM_VAR(AsmLabel, g_gs720770, $L_720770)
ASM_VAR(AsmLabel, s_gs676cdc, $L_676cdc)
ASM_VAR(AsmLabel, g_gs6ce830, $L_6ce830)
ASM_VAR(AsmLabel, s_gs676cd4, $L_676cd4)
ASM_VAR(AsmLabel, s_gs676ccc, $L_676ccc)
ASM_VAR(AsmLabel, s_gs68d1c0, $L_68d1c0)
ASM_VAR(BYTE*, g_gsLights, $L_6ce520)
ASM_VAR(BYTE*, g_gs6cecf4, $L_6cecf4)
ASM_VAR(DWORD, g_gs696874, $L_696874)
ASM_VAR(BYTE*, g_gs6cec6c, $L_6cec6c)
ASM_VAR(int, g_gs696878, $L_696878)
ASM_VAR(BYTE*, g_gs6cec90, $L_6cec90)
ASM_VAR(BYTE*, g_gs6ced54, $L_6ced54)
ASM_VAR(BYTE*, g_gs6ced24, $L_6ced24)
ASM_VAR(BYTE*, g_gs6ced2c, $L_6ced2c)
ASM_VAR(BYTE*, g_gs6ced30, $L_6ced30)
ASM_VAR(int, g_gs6cec74, $L_6cec74)
ASM_VAR(BYTE*, g_gs6ced34, $L_6ced34)
ASM_VAR(BYTE*, g_gs6ced38, $L_6ced38)
ASM_VAR(AsmLabel, g_gs696750, $L_696750)
ASM_VAR(DWORD, g_gs69685c, $L_69685c)
ASM_VAR(BYTE*, g_gs69684c, $L_69684c)
ASM_VAR(BYTE*, g_gs696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_gs6cecf8, $L_6cecf8)
ASM_VAR(int, g_gs6f2ee8, $L_6f2ee8)
ASM_VAR(int, g_gs6f2eec, $L_6f2eec)
ASM_VAR(int, g_gs6f2ef0, $L_6f2ef0)
ASM_VAR(int, g_gs6f2ed0, $L_6f2ed0)
ASM_VAR(int, g_gs6f2ed4, $L_6f2ed4)
ASM_VAR(int, g_gs6f2ed8, $L_6f2ed8)

// [FUN_51d0e0..$L_51d114] adds the names of the elements of the sub-storage 67683c of a save storage to a list
extern "C" void __cdecl Save_ListEntries(const char* name, BYTE* list)
{
    void* stg = CallStd<void*>(FUN_51bfa0, name, 1);
    if (!stg)
        Sys_Fatal((char*)g_gs6cecd8 + 632, s_gs677670);
    void* sub = 0;
    ComCall(stg, 24, CallC<const WCHAR*>(FUN_50bc90, (char*)s_gs67683c), (void*)0, 16, (void*)0, 0, &sub);
    if (sub)
    {
        void* en;
        long hr = ComCall(sub, 44, 0, (void*)0, 0, &en);
        if (!hr)
        {
            ComCall(en, 20);
            GsStatStg st;
            memset(&st, 0, sizeof st);
            if (!ComCall(en, 12, 1, &st, (void*)0))
            {
                do
                {
                    char* s = CallC<char*>(FUN_50bcc0, st.pwcsName);
                    if (s && *s)
                        CallThis(FUN_5c1140, list, s);
                    if (st.pwcsName)
                        CoTaskMemFree(st.pwcsName);
                    memset(&st, 0, sizeof st);
                } while (!ComCall(en, 12, 1, &st, (void*)0));
            }
            ComCall(en, 8);
        }
        else
            CallC(FUN_5d2ba0, (char*)s_gs677658, hr);
        ComCall(sub, 8);
    }
    ComCall(stg, 8);
}

// [FUN_515540..$L_51568b] the video settings of entry idx in a section of the ini: -1 runs the mode dialog and saves its choice; returns 1 after the dialog
extern "C" int __cdecl Shell_VideoSettings(int idx, const char* section, int a3, int a4, int a5, DWORD a6, int* mode, int* chosen)
{
    char key[32];
    int out0, out1, out2;
    sprintf(key, s_gs676ce4, idx + 1);
    int v;
    CallC(FUN_603b70, key, section, -1, &v, g_gsIni);
    if (v == -1)
    {
        int r = CallC<int>($L_5d7b90, a3, a4, a5, a6 | 0x8000, &out2, mode, &out0, &out1);
        *chosen = r != 0;
        int value;
        if (*chosen)
        {
            WritePrivateProfileStringA(key, s_gs676cdc, g_gs720770, g_gsIni);
            strcpy(g_gs6ce830, g_gs720770);
            CallC(FUN_603c70, key, (char*)s_gs676cd4, out0, g_gsIni);
            CallC(FUN_603c70, key, (char*)s_gs676ccc, out1, g_gsIni);
            value = 0;
        }
        else
            value = (*mode != 3) + 1;
        CallC(FUN_603c70, key, section, value, g_gsIni);
        return 1;
    }
    if (v == 0)
    {
        *chosen = 1;
        GetPrivateProfileStringA(key, s_gs676cdc, s_gs68d1c0, g_gs6ce830, 130, g_gsIni);
        return 0;
    }
    if (v == 1)
    {
        *chosen = 0;
        *mode = 3;
    }
    else if (v == 2)
    {
        *chosen = 0;
        *mode = 5;
    }
    return 0;
}

// [FUN_512c70..$L_512e06] leaves the game screens: frees the lights, fonts and menu objects, switches to 800x600 and back to the shell view
extern "C" void __cdecl Shell_LeaveGameScreens()
{
    BYTE* lights = g_gsLights;
    if (lights)
    {
        CallThis(FUN_4ccf10, lights);
        operator delete(lights);
    }
    g_gsLights = 0;
    if (g_gs6cecf4)
        CallVirt(g_gs6cecf4, 40);
    g_gs696878 = 0;
    g_gs696874 |= 64;
    F(int, g_gs6cec6c, 1300) = 0;
    BYTE* c = CallThis<BYTE*>(FUN_5bdb50, g_gs6cec90 + 64, 146);
    F(DWORD, c, 284) &= ~2u;
    if (g_gs6ced54)
    {
        CallVirt(g_gs6ced54, 0, 1);
        g_gs6ced54 = 0;
    }
    BYTE* f = g_gs6ced24;
    if (f) { CallThis(FUN_5bc2f0, f); operator delete(f); }
    f = g_gs6ced2c;
    if (f) { CallThis(FUN_5bc2f0, f); operator delete(f); }
    f = g_gs6ced30;
    if (f) { CallThis(FUN_5bc2f0, f); operator delete(f); }
    g_gs6ced38 = g_gs6ced34;
    if (g_gs6cec74)
    {
        CallThis(FUN_5db9f0, (BYTE*)g_gs696750, g_gs6cec74);
        g_gs6cec74 = 0;
    }
    int o0, o1, o2;
    DWORD flags = g_gs69685c;
    if (!CallC<int>($L_5d7c20, 800, 600, 1, (~(CallC<DWORD>(FUN_50bd80) | 0x4000) & flags) | 0xe00, &o2, &o1, &o0))
    {
        flags = g_gs69685c;
        CallThis($L_5dd8c0, (BYTE*)g_gs696750, 800, 600, 1, ~(CallC<DWORD>(FUN_50bd80) | 0x4000) & flags);
    }
    flags = g_gs69685c;
    CallThis($L_5dd8c0, (BYTE*)g_gs696750, 800, 600, 1, (~(CallC<DWORD>(FUN_50bd80) | 0x4000) & flags) | 0xe00);
    CallThis(FUN_5b6bf0, F(BYTE*, g_gs69684c, 12));
    CallThis(FUN_5d8630, g_gs69684c, 0);
    CallThis(FUN_5d88c0, g_gs69684c);
    CallThis(FUN_5ba3d0, *F(BYTE**, F(BYTE*, g_gs696ca0, 9396), 100));
    F(int, g_gs6cec6c, 1356) = 0;
    CallC(FUN_5d8460, 1, -1);
    g_gs6ced24 = g_gs6cecf8;
    g_gs6f2ed0 = g_gs6f2ee8;
    g_gs6f2ed4 = g_gs6f2eec;
    g_gs6f2ed8 = g_gs6f2ef0;
    CallC(FUN_527ea0, 0, 0);
}


// ---------------------------------------------------------------------------
ASM_VAR(AsmLabel, s_gs67694c, $L_67694c)
ASM_VAR(AsmLabel, s_gs676934, $L_676934)

// [FUN_513270..$L_513423] finds the stream of a storage holding a 256-byte name (streams named 67694c n); mode 0 opens it for writing (creating a new one), 2 rewrites a found one, else returns the found one
extern "C" void* __cdecl Save_FindNamedStream(void* stg, const char* name, int mode)
{
    char sname[32];
    char head[256];
    ULONG n;
    int i = 0;
    void* stream = 0;
    sprintf(sname, s_gs67694c, 0);
    ComCall(stg, 16, CallC<const WCHAR*>(FUN_50bc90, sname), (void*)0, 16, 0, &stream);
    while (stream)
    {
        ComCall(stream, 12, head, 256, &n);
        if (!strcmp(head, name))
            break;
        ComCall(stream, 8);
        i++;
        sprintf(sname, s_gs67694c, i);
        ComCall(stg, 16, CallC<const WCHAR*>(FUN_50bc90, sname), (void*)0, 16, 0, &stream);
    }
    if (mode == 0)
    {
        if (stream)
        {
            ComCall(stream, 8);
            ComCall(stg, 16, CallC<const WCHAR*>(FUN_50bc90, sname), (void*)0, 17, 0, &stream);
        }
        else
            ComCall(stg, 12, CallC<const WCHAR*>(FUN_50bc90, sname), 4113, 0, 0, &stream);
        if (!stream)
            Sys_Fatal(s_gs676934, name);
    }
    else if (mode == 2 && stream)
    {
        ComCall(stream, 8);
        ComCall(stg, 16, CallC<const WCHAR*>(FUN_50bc90, sname), (void*)0, 17, 0, &stream);
    }
    else
        return stream;
    memset(head, 0, sizeof head);
    strcpy(head, name);
    ComCall(stream, 16, head, 256, &n);
    return stream;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_50b4f0(); KING_ALIAS("_FUN_50b4f0", "@DMat_Zero@8")
extern "C" void FUN_50b810(); KING_ALIAS("_FUN_50b810", "_DRow_MulMat")
extern "C" void FUN_50b4c0(); KING_ALIAS("_FUN_50b4c0", "@DRow_Copy@12")
extern "C" void FUN_50b520(); KING_ALIAS("_FUN_50b520", "@DMat_CopyNew@12")

// [FUN_50ba90] out = the four 32-byte parts of a, each combined with b through 50b810
extern "C" BYTE* __cdecl Rec128_Combine(BYTE* out, BYTE* a, int b)
{
    BYTE tmp[32];
    BYTE r[128];
    CallThis(FUN_50b4f0, r);
    for (int i = 0; i < 4; i++)
        CallThis(FUN_50b4c0, r + i * 32, CallC<void*>(FUN_50b810, tmp, a + i * 32, b));
    CallThis(FUN_50b520, out, r);
    return out;
}

