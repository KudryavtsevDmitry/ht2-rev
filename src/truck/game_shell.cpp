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
KING_ALIAS("_Carat_Window", "_Shell_ButtonA")
extern "C" int __cdecl Carat_LoadFont(const char* name);
KING_ALIAS("_Carat_LoadFont", "_Font_LoadFile")
extern "C" void __cdecl Carat_Draw5b86c0(int a, int b, int c, int d);
KING_ALIAS("_Carat_Draw5b86c0", "_Bitmap_Draw")
extern "C" BYTE* __cdecl Messages_Clear1276();                     // copter.cpp
extern "C" int __cdecl Carat_Resource(int id);
KING_ALIAS("_Carat_Resource", "_Table71e858_Get")
extern "C" void __cdecl Carat_51d8f0(int a, int b);
KING_ALIAS("_Carat_51d8f0", "_Save_WriteEntry")
extern "C" int __cdecl Carat_5d3ab0();
KING_ALIAS("_Carat_5d3ab0", "_Mode_TextureLevel")
extern "C" void __cdecl Carat_Message(const char* text, int);
KING_ALIAS("_Carat_Message", "_Sys_Log")
extern "C" int __fastcall Lights_Release(BYTE* self, void*, int a);   // aeffects.cpp
extern "C" void FUN_50c010(); KING_ALIAS("_FUN_50c010", "@Screen_Grab@8")
extern "C" void __fastcall CGameApp_SetMousePos(void* app, void*, int x, int y);   // gameapp.cpp
extern "C" void FUN_5d85f0(); KING_ALIAS("_FUN_5d85f0", "@Win_GetRect@12")    // window: its rectangle
extern "C" void FUN_5bd5a0(); KING_ALIAS("_FUN_5bd5a0", "@Style_Assign@12")
extern "C" void FUN_5bd5e0(); KING_ALIAS("_FUN_5bd5e0", "@Control_Construct@16")
extern "C" void FUN_5bef30(); KING_ALIAS("_FUN_5bef30", "@Ctl_SetStyles@28")
extern "C" void FUN_5bef90(); KING_ALIAS("_FUN_5bef90", "@Ctl_SetStyle5@16")
ASM_VAR(AsmLabel, s_676310, $L_676310)
ASM_VAR(AsmLabel, s_676308, $L_676308)
ASM_VAR(AsmLabel, s_676318, $L_676318)
ASM_VAR(AsmLabel, s_676338, $L_676338)
ASM_VAR(AsmLabel, s_67632c, $L_67632c)
ASM_VAR(AsmLabel, s_676320, $L_676320)
extern "C" void FUN_5ffb40(); KING_ALIAS("_FUN_5ffb40", "_Music_Clear")
extern "C" void FUN_5ba4a0(); KING_ALIAS("_FUN_5ba4a0", "@Texture5ba4a0_Construct@20")
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
extern "C" void FUN_5c1b10(); KING_ALIAS("_FUN_5c1b10", "@Media_Close@8")
extern "C" void FUN_5d8850(); KING_ALIAS("_FUN_5d8850", "@Window_Destroy@4")
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
extern "C" void FUN_5d89e0(); KING_ALIAS("_FUN_5d89e0", "@Fw5d89e0_Destroy@4")

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
extern "C" void FUN_5b6bf0(); KING_ALIAS("_FUN_5b6bf0", "@Surface_Prepare@4")
extern "C" void FUN_5b6ed0(); KING_ALIAS("_FUN_5b6ed0", "@Surface_ClearRows@8")
extern "C" void FUN_5b6b10(); KING_ALIAS("_FUN_5b6b10", "@Surface_Present@8")
extern "C" void FUN_4e1320(); KING_ALIAS("_FUN_4e1320", "_Startup_FindCd")
extern "C" void FUN_519750(); KING_ALIAS("_FUN_519750", "@Shell_ShowWithMusic@12")
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

extern "C" void FUN_5dc230(); KING_ALIAS("_FUN_5dc230", "@App_Path@12")
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

extern "C" void FUN_5d89e0(); KING_ALIAS("_FUN_5d89e0", "@Fw5d89e0_Destroy@4")    // the screen base destructor
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
extern "C" void FUN_5b6bf0(); KING_ALIAS("_FUN_5b6bf0", "@Surface_Prepare@4")
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
extern "C" void FUN_5147b0(); KING_ALIAS("_FUN_5147b0", "@Input_ApplyBindings@4")
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
extern "C" void FUN_5b6bf0(); KING_ALIAS("_FUN_5b6bf0", "@Surface_Prepare@4")    // surface: prepare
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


// ---------------------------------------------------------------------------
// Small virtual methods of the shell screens and their static initializers.

extern "C" void FUN_5b6bf0(); KING_ALIAS("_FUN_5b6bf0", "@Surface_Prepare@4")
extern "C" void FUN_5c0020(); KING_ALIAS("_FUN_5c0020", "@List_DrawAt@12")
extern "C" void FUN_5d8610(); KING_ALIAS("_FUN_5d8610", "@Win_Activate@8")
extern "C" void FUN_6037e0(); KING_ALIAS("_FUN_6037e0", "_Tsc_Now")
extern "C" void FUN_50d510(); KING_ALIAS("_FUN_50d510", "@ShellBuf_Destroy@8")
extern "C" void FUN_528fc0(); KING_ALIAS("_FUN_528fc0", "@Shell528fc0_Destroy@8")
ASM_PROC(FUN_5b8980)
extern "C" void FUN_528fb0(); KING_ALIAS("_FUN_528fb0", "@Screen_Set2304@12")
extern "C" void FUN_5b86c0(); KING_ALIAS("_FUN_5b86c0", "_Bitmap_Draw")
extern "C" void FUN_5d8bb0(); KING_ALIAS("_FUN_5d8bb0", "@Frame_Ask@12")
ASM_VAR(int, g_gs6ce9f0, $L_6ce9f0)
ASM_VAR(int, g_gs6968a4, $L_6968a4)

// [$L_528c10]
extern "C" void __fastcall Shell_Nothing8(BYTE*, void*, int, int) {}
// [$L_50d180]
extern "C" int __fastcall Shell_One(BYTE*) { return 1; }
// [$L_50d1b0]
extern "C" int __fastcall Shell_First2(BYTE*, void*, int a, int) { return a; }
// [$L_50d1a0]
extern "C" int __fastcall Shell_First3(BYTE*, void*, int a, int, int) { return a; }
// [$L_50ff60]
extern "C" int __fastcall Shell_Second2(BYTE*, void*, int, int b) { return b; }

// [$L_50ff50]
extern "C" int __fastcall Shell_50ff50(BYTE* s) { return CallThis<int>(FUN_5b6bf0, F(BYTE*, s, 12)); }
// [$L_50ff40]
extern "C" int __fastcall Shell_DrawAt50ff40(BYTE* s, void*, int a) { return CallThis<int>(FUN_5c0020, s, a); }
// [$L_518700]
extern "C" int __fastcall Shell_DrawAt518700(BYTE* s, void*, int a) { return CallThis<int>(FUN_5c0020, s, a); }

// [$L_511780] passes a to the vtbl+160 of the object at +304
extern "C" int __fastcall Shell_511780(BYTE* s, void*, int a) { return CallVirt<int>(F(BYTE*, s, 304), 160, a); }

// [$L_523150] activates, then the time stamp to +368
extern "C" void __fastcall Shell_ActivateStamp(BYTE* s)
{
    CallThis(FUN_5d8610, s);
    F(double, s, 368) = CallC<double>(FUN_6037e0);
}

// [$L_518460] command 1: the owner (64 bytes before) vtbl+32, then 6cec80's vtbl+36
extern "C" void __fastcall Shell_518460(BYTE* s, void*, int cmd)
{
    if (cmd != 1)
        return;
    CallVirt(s - 64, 32);
    CallVirt(g_6cec80, 36);
}

// [$L_518710] command 154 like 518460 with 6cec80's +372 cleared; 153 sets 6968a4
extern "C" void __fastcall Shell_518710(BYTE* s, void*, int cmd)
{
    if (cmd == 153) {
        g_gs6968a4 = 1;
        return;
    }
    if (cmd != 154)
        return;
    CallVirt(s - 64, 32);
    F(int, g_6cec80, 372) = 0;
    CallVirt(g_6cec80, 36);
}

// [$L_50bfc0] static initializers
extern "C" void __cdecl Shell_Init6ce4f4() { g_6ce4f4 = 0; }
// [$L_50c7d0]
extern "C" void __cdecl Shell_Init6ce9f0() { g_gs6ce9f0 = 0; }
// [$L_529180]
extern "C" void __cdecl Shell_Init6ce4f8()
{
    ((DWORD*)g_6ce4f8)[0] = 0;
    ((DWORD*)g_6ce4f8)[1] = 0;
}

// [$L_50d4f0]
extern "C" BYTE* __fastcall Shell50d4f0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_50d510, s);
    if (flags & 1)
        operator delete(s);
    return s;
}
// [$L_528c80]
extern "C" BYTE* __fastcall Shell528c80_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_528fc0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// [$L_51ab50] draws, then the current picture (+744 row, +748 column) at 360,134
extern "C" void __fastcall Shell_51ab50(BYTE* s, void*, int a)
{
    CallThis(FUN_5c0020, s, a);
    int k = F(int, s, 748) + F(int, s, 744) * 4;
    CallC(FUN_5b8980, F(int, s, -52), 360, 134, F(int, s, 308 + k * 4), -1);
}

// [$L_529020] Enter (key 13 of a key-down event 258) leaves the screen unless +2308 is 1
extern "C" int __fastcall Shell_529020(BYTE* s, void*, BYTE* ev)
{
    if (F(WORD, ev, 0) != 258)
        return 0;
    if (F(WORD, ev, 6) != 13 || F(int, s, 2308) != 1)
        CallThis(FUN_528fb0, s, 1);
    return 1;
}

// draws the backdrop 6ce4f4 when there is one, then the screen
static void Shell_DrawWithBackdrop(BYTE* s, int a)
{
    if (g_6ce4f4)
        CallC(FUN_5b86c0, F(int, g_69684c, 12), 0, 0, g_6ce4f4);
    CallThis(FUN_5c0020, s, a);
}
// [$L_5117a0]
extern "C" void __fastcall Shell_5117a0(BYTE* s, void*, int a) { Shell_DrawWithBackdrop(s, a); }
// [$L_51e840]
extern "C" void __fastcall Shell_51e840(BYTE* s, void*, int a) { Shell_DrawWithBackdrop(s, a); }

// [$L_51fd30] asks (FUN_5d8bb0) only in states 0-2, 5 and over except 6, and without +1316 of 6cec6c
extern "C" int __fastcall Shell_51fd30(BYTE* s, void*, int a)
{
    int st = F(int, s, 404);
    if ((st > 2 && st < 5) || st == 6 || F(int, g_6cec6c, 1316))
        return 0;
    return CallThis<int>(FUN_5d8bb0, s, a);
}

extern "C" void FUN_4e12b0(); KING_ALIAS("_FUN_4e12b0", "_Joy_ForceFeedback")
extern "C" void FUN_5bdb50(); KING_ALIAS("_FUN_5bdb50", "@Ctl_FindId@12")
ASM_VAR(BYTE*, g_gamshe696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_gamshe6d2078, $L_6d2078)

// [$L_51bf40] draws, then the pictures of the two choices (+552 at 342,129, +560 at 450,219)
extern "C" void __fastcall Shell_51bf40(BYTE* s, void*, int a)
{
    CallThis(FUN_5c0020, s, a);
    CallC(FUN_5b8980, F(int, s, -52), 342, 129, F(int, s, 304 + F(int, s, 552) * 4), -1);
    CallC(FUN_5b8980, F(int, s, -52), 450, 219, F(int, s, 336 + F(int, s, 560) * 4), -1);
}

// [$L_522060] Esc (key-down 256, key 27) closes the screen, resumes the world and sets the cab's mode 2
extern "C" int __fastcall Shell_522060(BYTE* s, void*, BYTE* ev)
{
    if (F(WORD, ev, 0) == 256 && F(WORD, ev, 6) == 27) {
        CallVirt(s, 32);
        CallVirt(g_gamshe696ca0, 36);
        CallC(FUN_4e12b0, 1);
        F(BYTE, F(BYTE*, F(BYTE*, g_gamshe6d2078, 616), 21600), 9984) = 2;
    }
    return CallThis<int>(FUN_5d8bb0, s, ev);
}

// [$L_51b5a0] activates; of the controls 111/112 (+372) the one for 6cec6c's +1304 is pressed; the colors 388.. to 376..
extern "C" void __fastcall Shell_51b5a0(BYTE* s)
{
    CallThis(FUN_5d8610, s);
    BYTE* c = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, s, 372), F(int, g_6cec6c, 1304) ? 111 : 112);
    F(int, c, 284) |= 2;
    c = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, s, 372), F(int, g_6cec6c, 1304) ? 112 : 111);
    F(int, c, 284) &= ~2;
    F(WORD, s, 376) = F(WORD, s, 388);
    F(WORD, s, 378) = F(WORD, s, 390);
    F(WORD, s, 380) = F(WORD, s, 392);
}

// ---------------------------------------------------------------------------
// More shell screens: keys, mouse, pictures and the screens' start.

extern "C" void FUN_569710(); KING_ALIAS("_FUN_569710", "_Messages_Clear1276")
extern "C" void FUN_50bd60(); KING_ALIAS("_FUN_50bd60", "_Carat_SetScreenSize")
extern "C" void FUN_50bd30(); KING_ALIAS("_FUN_50bd30", "_Carat_UpdateScreenSize")
ASM_PROC(FUN_532950)
extern "C" void FUN_5ea0e0(); KING_ALIAS("_FUN_5ea0e0", "@Font_TextWidth@12")
extern "C" void FUN_5ea170(); KING_ALIAS("_FUN_5ea170", "_Text_Draw")
extern "C" void FUN_50bfe0(); KING_ALIAS("_FUN_50bfe0", "_Carat_HasMovie")
extern "C" void FUN_50c100(); KING_ALIAS("_FUN_50c100", "_Carat_CloseMovie")
extern "C" void FUN_530020(); KING_ALIAS("_FUN_530020", "@Words295_Reset@8")
extern "C" void FUN_50c010(); KING_ALIAS("_FUN_50c010", "@Screen_Grab@8")
extern "C" void FUN_4eb680(); KING_ALIAS("_FUN_4eb680", "@Ramp_High@8")
extern "C" void FUN_4eb690(); KING_ALIAS("_FUN_4eb690", "@Ramp_Position@8")
extern "C" void FUN_51c870(); KING_ALIAS("_FUN_51c870", "_Storage_ReadSubImage")
extern "C" void FUN_518760(); KING_ALIAS("_FUN_518760", "_Storage_ReadImage")
extern "C" void Gs_sprintf(); KING_ALIAS("_Gs_sprintf", "_sprintf")
ASM_VAR(int, g_gamshe696c40, $L_696c40)
ASM_VAR(BYTE*, g_gamshe6ced28, $L_6ced28)
ASM_VAR(AsmLabel, g_gamshe6ce830, $L_6ce830)
ASM_VAR(BYTE*, g_gamshe6cecb0, $L_6cecb0)
ASM_VAR(AsmLabel, g_gamshe6d1948, $L_6d1948)
ASM_VAR(AsmLabel, g_gamshe6ce4f4, $L_6ce4f4)
ASM_VAR(double, g_gamshe64dea0, $L_64dea0)
ASM_VAR(double, g_gamshe64b6a0, $L_64b6a0)
ASM_VAR(BYTE*, g_gamshe696848, $L_696848)
ASM_VAR(AsmLabel, s_gamshe6772b0, $L_6772b0)

// the cab of the player's car
static BYTE* Gs_Cab() { return F(BYTE*, F(BYTE*, g_gamshe6d2078, 616), 21600); }

// closes the screen and goes back to the world (cab mode 2)
static void Gs_BackToWorld(BYTE* s)
{
    CallVirt(s, 32);
    CallVirt(g_gamshe696ca0, 36);
    CallC(FUN_4e12b0, 1);
    F(BYTE, Gs_Cab(), 9984) = 2;
}

// [$L_520c80] Esc goes back to the world
extern "C" int __fastcall Shell_520c80(BYTE* s, void*, BYTE* ev)
{
    if (F(WORD, ev, 0) == 256 && F(WORD, ev, 6) == 27) {
        Gs_BackToWorld(s);
        return 1;
    }
    return CallThis<int>(FUN_5d8bb0, s, ev);
}

// drops the backdrop and lets the world run again
static void Gs_DropBackdrop()
{
    if (g_6ce4f4)
        CallVirt(g_6ce4f4, 0, 1);
    g_6ce4f4 = 0;
    F(int, g_gamshe696ca0, 16) |= 3;
    CallC(FUN_569710);
}

// [$L_525ad0] F5 or Esc: back to the world
extern "C" int __fastcall Shell_525ad0(BYTE* s, void*, BYTE* ev)
{
    if (F(WORD, ev, 0) == 256 && (F(WORD, ev, 6) == 116 || F(WORD, ev, 6) == 27)) {
        Gs_DropBackdrop();
        CallVirt(s, 32);
        CallVirt(g_gamshe696ca0, 36);
        CallC(FUN_50bd60);
        CallC(FUN_4e12b0, 1);
    }
    return CallThis<int>(FUN_5d8bb0, s, ev);
}

// [$L_5277d0] F4 or Esc: back to the world
extern "C" int __fastcall Shell_5277d0(BYTE* s, void*, BYTE* ev)
{
    if (F(WORD, ev, 0) == 256 && (F(WORD, ev, 6) == 115 || F(WORD, ev, 6) == 27)) {
        CallVirt(s, 32);
        CallVirt(g_gamshe696ca0, 36);
        CallC(FUN_50bd60);
        CallC(FUN_4e12b0, 1);
        Gs_DropBackdrop();
        return 1;
    }
    return CallThis<int>(FUN_5d8bb0, s, ev) != 0;
}

// [$L_50ff90] with 696c40, F5-F7 go to 6cec6c's +1356; other than mouse events the car (6d2078) sees them
extern "C" int __fastcall Shell_50ff90(BYTE* s, void*, BYTE* ev)
{
    if (g_gamshe696c40 && F(WORD, ev, 0) == 256) {
        int k = F(short, ev, 6);
        if (k == 116 || k == 117 || k == 118) {
            CallVirt(F(BYTE*, g_6cec6c, 1356), 148, ev);
            return 1;
        }
    }
    WORD t = F(WORD, ev, 0);
    if (t != 513 && t != 514 && t != 512)
        CallThis(FUN_532950, g_gamshe6d2078, ev);
    CallThis(FUN_5d8bb0, s, ev);
    return 1;
}

// [$L_516b10] the arrows (+316, +320) enabled by the page (+348 of +324), then the page text centered at 329..584
extern "C" void __fastcall Shell_516b10(BYTE* s, void*, int a)
{
    F(int, F(BYTE*, s, 316), 40) = F(int, s, 348) ? 0 : 1;
    F(int, F(BYTE*, s, 320), 40) = F(int, s, 348) >= F(int, s, 324) ? 1 : 0;
    CallThis(FUN_5c0020, s, a);
    int w = CallThis<int>(FUN_5ea0e0, g_gamshe6ced28, (BYTE*)g_gamshe6ce830);
    CallC(FUN_5ea170, F(int, s, -52), (255 - w) / 2 + 329, 257, g_gamshe6ced28, (BYTE*)g_gamshe6ce830, 4, 0);
}

// [$L_5116c0] leaving the screen once the game has run (+2992 of +368 at 5 or more)
extern "C" void __fastcall Shell_5116c0(BYTE* s)
{
    if (F(int, F(BYTE*, s, 368), 2992) < 5)
        return;
    if (F(BYTE, g_gamshe6cecb0, 16) & 2)
        return;
    BYTE* g = g_6cec6c;
    if ((F(int, g, 1296) || F(int, g, 1284) || F(int, g, 1288)) && CallC<int>(FUN_50bfe0)) {
        F(int, g_6cec6c, 1284) = 0;
        F(int, g_6cec6c, 1288) = 0;
        F(int, g_6cec6c, 1296) = 0;
        CallC(FUN_50c100);
        CallC(FUN_4e12b0, 1);
        return;
    }
    CallThis(FUN_530020, (BYTE*)g_gamshe6d1948);
    CallC(FUN_50bd30);
    CallC(FUN_4e12b0, 0);
    CallThis(FUN_50c010, (BYTE*)g_gamshe6ce4f4);
    CallThis(FUN_5d8610, s);
}

// [$L_516bc0] a mouse move far from 400,327 or an unhandled event of the current screen goes to 6cec80
extern "C" int __fastcall Shell_516bc0(BYTE* s, void*, BYTE* ev)
{
    if (F(WORD, ev, 0) == 512) {
        int dx = F(short, ev, 2) - 400;
        int dy = F(short, ev, 4) - 327;
        dx = dx < 0 ? -dx : dx;
        dy = dy < 0 ? -dy : dy;
        if (x87_sqrt((double)(dy * dy + dx * dx)) > g_gamshe64dea0)
            return CallVirt<int>(g_6cec80, 52, ev);
    }
    if (!CallThis<int>(FUN_5d8bb0, s, ev) && g_gamshe696848 == s && F(WORD, ev, 0) != 512)
        return CallVirt<int>(g_6cec80, 52, ev);
    return 1;
}

// [$L_5205a0] the gearbox screen's start: the cab's ratio values, the controls and the cab's settings
extern "C" void __fastcall Shell_5205a0(BYTE* s)
{
    CallC(FUN_50bd30);
    CallThis(FUN_5d8610, s);
    F(double, s, 384) = CallThis<double>(FUN_4eb680, Gs_Cab() + 16112);
    double t = CallThis<double>(FUN_4eb690, Gs_Cab() + 16112) * F(double, s, 384);
    F(int, s, 368) = 0;
    F(int, s, 392) = 0;
    F(int, s, 372) = 0;
    F(int, s, 396) = 0;
    F(double, s, 376) = t;
    F(short, s, 400) = (short)(int)(t * g_gamshe64b6a0 / F(double, s, 384));
    F(int, F(BYTE*, s, 404), 284) &= ~2;
    F(BYTE, Gs_Cab(), 9984) = 0;
    F(DWORD, Gs_Cab(), 10000) = 0x3f000000;
    F(int, Gs_Cab(), 9996) = 0;
    F(WORD, Gs_Cab(), 10004) = 0;
    F(WORD, Gs_Cab(), 10006) = 1;
    F(WORD, Gs_Cab(), 10054) = 21;
    CallThis(FUN_530020, (BYTE*)g_gamshe6d1948);
    CallC(FUN_4e12b0, 0);
}

// draws the picture of the chosen item and its four numbers (bytes 2, 3, 5, 6 of its record)
static void Gs_DrawItem(BYTE* s, BYTE* list, void (*find)(), int px, int py, int tx, int ty, bool textFirst)
{
    BYTE* o = F(BYTE*, list, 300);
    if (o)
        o = F(BYTE*, o, 12);
    BYTE rec[164];
    BYTE* pic = CallC<BYTE*>(find, o, rec);
    if (!pic)
        return;
    char buf[64];
    if (!textFirst) {
        CallC(FUN_5b86c0, F(int, s, -52), px, py, pic);
        CallVirt(pic, 0, 1);
    }
    CallC(Gs_sprintf, buf, (const char*)s_gamshe6772b0, rec[2], rec[3], rec[5], rec[6]);
    if (textFirst)
        CallC(FUN_5b86c0, F(int, s, -52), px, py, pic);
    CallC(FUN_5ea170, F(int, s, -52), tx, ty, F(int, g_gamshe696ca0, 2864), buf, 5, 0);
    if (textFirst)
        CallVirt(pic, 0, 1);
}
// [$L_51d600]
extern "C" void __fastcall Shell_51d600(BYTE* s, void*, int a)
{
    CallThis(FUN_5c0020, s, a);
    Gs_DrawItem(s, F(BYTE*, s, 332), FUN_51c870, 487, 390, 490, 464, true);
}
// [$L_518820]
extern "C" void __fastcall Shell_518820(BYTE* s, void*, int a)
{
    CallThis(FUN_5c0020, s, a);
    Gs_DrawItem(s, F(BYTE*, s, 316), FUN_518760, 478, 203, 483, 276, false);
}

// ---------------------------------------------------------------------------
// Larger shell screens: option pages, the race clock, the time table, sounds.

extern "C" void FUN_515270(); KING_ALIAS("_FUN_515270", "@Keys_Bind@12")
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")
ASM_PROC(FUN_5ea1b0)
extern "C" void FUN_5c0cf0(); KING_ALIAS("_FUN_5c0cf0", "@List_Clear@8")
extern "C" void FUN_51d0e0(); KING_ALIAS("_FUN_51d0e0", "_Save_ListEntries")
extern "C" void FUN_5d8b90(); KING_ALIAS("_FUN_5d8b90", "@Wnd5d8b90_Notify@8")
extern "C" void FUN_568f60(); KING_ALIAS("_FUN_568f60", "_Player_CarId")
extern "C" void FUN_43f760(); KING_ALIAS("_FUN_43f760", "_Player_Value136")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
ASM_PROC(FUN_5fdaf0)
extern "C" void FUN_5fcd40(); KING_ALIAS("_FUN_5fcd40", "_Sound_SetVolume")
extern "C" void FUN_5ffc30(); KING_ALIAS("_FUN_5ffc30", "_Set683c58")
extern "C" void FUN_5ffa30(); KING_ALIAS("_FUN_5ffa30", "_Music_Start")
extern "C" void FUN_5ffb20(); KING_ALIAS("_FUN_5ffb20", "_Obj723d44_Delete")
extern "C" void FUN_5e4250(); KING_ALIAS("_FUN_5e4250", "@Windows_SetActive@12")
extern "C" void FUN_603c70(); KING_ALIAS("_FUN_603c70", "_Ini_WriteInt")
extern "C" void FUN_6037e0(); KING_ALIAS("_FUN_6037e0", "_Tsc_Now")
ASM_VAR(int, g_gs6ce9f0n, $L_6ce9f0)
ASM_VAR(AsmLabel, g_gs6ce9f4, $L_6ce9f4)
ASM_VAR(AsmLabel, g_gs6cea74, $L_6cea74)
ASM_VAR(AsmLabel, s_gs66d9c4, $L_66d9c4)
ASM_VAR(BYTE*, g_gs6ced24, $L_6ced24)
ASM_VAR(int, g_gs6ced18, $L_6ced18)
ASM_VAR(int, g_gs6ced14, $L_6ced14)
ASM_VAR(BYTE*, g_gs6cecd8, $L_6cecd8)
ASM_VAR(AsmLabel, g_gs6ce390, $L_6ce390)
ASM_VAR(double, g_gs64b678, $L_64b678)
ASM_VAR(double, g_gs64b5a0, $L_64b5a0)
ASM_VAR(double, g_gs64b6e0, $L_64b6e0)
ASM_VAR(double, g_gs64b670, $L_64b670)
ASM_VAR(AsmLabel, s_gs677a18, $L_677a18)
ASM_VAR(int, g_gs6f6914, $L_6f6914)
ASM_VAR(double, g_gs64b658, $L_64b658)
ASM_VAR(int, g_gs696868, $L_696868)
ASM_VAR(BYTE*, g_gs6ced6c, $L_6ced6c)
ASM_VAR(double, g_gs64b930, $L_64b930)
ASM_VAR(AsmLabel, g_gs696750, $L_696750)
ASM_VAR(AsmLabel, s_gs672998, $L_672998)
ASM_VAR(AsmLabel, s_gs676e10, $L_676e10)
ASM_VAR(AsmLabel, s_gs676e08, $L_676e08)
ASM_VAR(AsmLabel, s_gs676e00, $L_676e00)
ASM_VAR(AsmLabel, s_gs676df8, $L_676df8)
ASM_VAR(BYTE*, g_gs6ced5c, $L_6ced5c)
ASM_VAR(double, g_gs64e918, $L_64e918)
ASM_VAR(double, g_gs64b918, $L_64b918)
ASM_VAR(double, g_gs64b890, $L_64b890)

extern "C" void FUN_5d3080(); KING_ALIAS("_FUN_5d3080", "@Node_Next@8")
extern "C" void FUN_5c0c20(); KING_ALIAS("_FUN_5c0c20", "@List_Select@12")

// the distance of a mouse event from the screen's middle 400,327
static double Gs_DistFromMiddle(BYTE* ev)
{
    int dx = F(short, ev, 2) - 400;
    int dy = F(short, ev, 4) - 327;
    dx = dx < 0 ? -dx : dx;
    dy = dy < 0 ? -dy : dy;
    return x87_sqrt((double)(dy * dy + dx * dx));
}

// [$L_516c60] activates; the buttons of the mode (+408 bits 4-7: 16, 32, 48, 64) from +376's +288 on
extern "C" void __fastcall Shell_516c60(BYTE* s)
{
    CallThis(FUN_5d8610, s);
    F(WORD, s, 368) = F(WORD, s, 420);
    F(WORD, s, 370) = F(WORD, s, 422);
    F(int, s, 372) = F(int, s, 408);
    BYTE* c = F(BYTE*, F(BYTE*, s, 376), 288);
    for (int m = 16; m <= 64; m += 16) {
        if (m > 16)
            c = CallThis<BYTE*>(FUN_5d3080, c);
        if ((BYTE)(F(int, s, 408) & 240) == m)
            F(unsigned, c, 284) |= 2;
        else
            F(unsigned, c, 284) &= ~2u;
    }
}

// [$L_527890] near the middle the target (+372) takes mouse moves; a click elsewhere activates the screen
extern "C" int __fastcall Shell_527890(BYTE* s, void*, BYTE* ev)
{
    if (F(WORD, ev, 0) == 512 && !(Gs_DistFromMiddle(ev) >= g_gamshe64dea0) && F(BYTE*, s, 372))
        return CallVirt<int>(F(BYTE*, s, 372), 52, ev);
    int r = CallThis<int>(FUN_5d8bb0, s, ev);
    BYTE* cur = g_gamshe696848;
    if (!r) {
        if (cur == s && F(BYTE*, s, 372) && F(WORD, ev, 0) != 512)
            return CallVirt<int>(F(BYTE*, s, 372), 52, ev);
    } else if (cur != s && F(WORD, ev, 0) == 513) {
        CallVirt(s, 36);
    }
    return 1;
}

// [$L_5153e0] the same with 6cec80; the current screen first maps key-downs to its choice (+516)
extern "C" int __fastcall Shell_5153e0(BYTE* s, void*, BYTE* ev)
{
    WORD t = F(WORD, ev, 0);
    if (t == 512 && Gs_DistFromMiddle(ev) > g_gamshe64dea0)
        return CallVirt<int>(g_6cec80, 52, ev);
    if (g_gamshe696848 != s)
        return CallThis<int>(FUN_5d8bb0, s, ev);
    if (t == 256) {
        int r = CallThis<int>(FUN_515270, s, (int)F(short, ev, 6));
        if (r >= 0)
            F(int, s, 516) = r;
    }
    if (!CallThis<int>(FUN_5d8bb0, s, ev) && g_gamshe696848 == s && F(WORD, ev, 0) != 512)
        return CallVirt<int>(g_6cec80, 52, ev);
    return 1;
}

// [$L_522af0] draws the list of the 6ce9f0 entries: a yes/no text (98/99, 6cea74) and the name (6ce9f4)
extern "C" void __fastcall Shell_522af0(BYTE* s, void*, int a)
{
    CallThis(FUN_5c0020, s, a);
    for (int i = 0; i < g_gs6ce9f0n; i++) {
        char buf[64];
        const char* t = CallC<const char*>(FUN_5d2e50, ((int*)g_gs6cea74)[i] ? 98 : 99);
        CallC(Gs_sprintf, buf, (const char*)s_gs66d9c4, t);
        if (!g_gs6ced24)
            continue;
        int y = i * 13 + g_gs6ced18 + 538;
        CallC(FUN_5ea1b0, a, g_gs6ced14 + 60, y, 67, g_gs6ced24, (BYTE*)g_gs6ce9f4 + i * 16, 4, 0);
        CallC(FUN_5ea170, a, g_gs6ced14 + 131, y, g_gs6ced24, buf, 4, 0);
    }
}

// [$L_51d220] the options page: buttons 111/112 by 6cec6c's +1304, 273/274 by +1312, the list (+396) refilled
extern "C" void __fastcall Shell_51d220(BYTE* s)
{
    BYTE* c = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, s, 392), F(int, g_6cec6c, 1304) ? 111 : 112);
    F(unsigned, c, 284) |= 2;
    c = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, s, 392), F(int, g_6cec6c, 1304) ? 112 : 111);
    F(unsigned, c, 284) &= ~2u;
    c = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, s, 392), F(int, g_6cec6c, 1312) ? 273 : 274);
    F(unsigned, c, 284) |= 2;
    c = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, s, 392), F(int, g_6cec6c, 1312) ? 274 : 273);
    F(unsigned, c, 284) &= ~2u;
    CallThis(FUN_5c0cf0, F(BYTE*, s, 396));
    CallC(FUN_51d0e0, g_gs6cecd8 + 632, F(BYTE*, s, 396));
    CallThis(FUN_5c0c20, F(BYTE*, s, 396), 0);
    BYTE* o = F(BYTE*, F(BYTE*, s, 396), 300);
    F(int, F(BYTE*, s, 388), 40) = (!o || !F(int, o, 12)) ? 1 : 0;
    F(WORD, s, 382) = F(WORD, s, 402);
    F(WORD, s, 380) = F(WORD, s, 400);
    F(WORD, s, 384) = F(WORD, s, 404);
    CallThis(FUN_5d8610, s);
}

// [$L_50cfa0] static initializer: nine 40-byte records (word 6 = 0.5, words 0-8 otherwise zero)
extern "C" void __cdecl GamsheInit_50cfa0()
{
    static const int off[9] = {0x000, 0x028, 0x050, 0x078, 0x0a0, 0x5a8, 0x5d0, 0x5f8, 0x7a8};
    for (int r = 0; r < 9; r++) {
        DWORD* p = (DWORD*)((BYTE*)g_gs6ce390 + off[r]);
        for (int k = 0; k < 9; k++)
            p[k] = k == 6 ? 0x3f000000 : 0;
    }
}

// [$L_520ce0] the refuelling screen: 2 leaves, 276 a full tank (100), 275 pays for the fuel
extern "C" void __fastcall Shell_Refuel(BYTE* s, void*, int cmd)
{
    if (cmd == 2) {
        Gs_BackToWorld(s - 64);
        return;
    }
    if (cmd == 276) {
        F(short, s, 336) = 100;
        return;
    }
    if (cmd != 275)
        return;
    if (F(double, s, 304) > g_gs64b678)
        return;
    double v = (double)F(short, s, 336) * F(double, s, 320) * g_gs64b5a0 - F(double, s, 312);
    F(double, s, 304) = v;
    if (!(v >= g_gs64b678)) {
        F(DWORD, s, 304) = 0;
        F(DWORD, s, 308) = 0;
    }
    DWORD tmp;
    CallC(FUN_568f60, &tmp);
    double money = CallC<double>(FUN_43f760, tmp);
    if (F(double, s, 304) + F(double, s, 304) > money)
        F(double, s, 304) = money * g_gs64b6e0 - g_gs64b670;
    if (!(F(double, s, 304) >= g_gs64b678)) {
        F(DWORD, s, 304) = 0;
        F(DWORD, s, 308) = 0;
    }
    if (F(double, s, 304) > g_gs64b678) {
        BYTE* snd = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_gs677a18);
        CallThis(FUN_5fdaf0, snd, 0x3f800000, 0x3f800000);
    }
}

// [$L_510010] the race display: the mode picture and the clock (minutes and seconds of 696ca0's +7968)
extern "C" void __fastcall Shell_RaceClock(BYTE* s)
{
    CallThis(FUN_5d8b90, s);
    int* r = (int*)(F(BYTE*, F(BYTE*, s, 56), 288) + 44);
    int x0 = r[0], y0 = r[1];
    if (!F(int, g_gamshe6d2078, 1400)) {
        F(int, s, 416) = 1;
    } else {
        if (g_gs6f6914 == 0)
            F(int, s, 416) = 0;
        else if (g_gs6f6914 == 2)
            F(int, s, 416) = 3;
        if (g_gs6f6914 == 1)
            F(int, s, 416) = 2;
    }
    CallC(FUN_5b8980, F(int, s, 12), x0 + F(int, s, 416) * 52 + 12, y0 + 31, F(int, s, 368), -1);
    unsigned t = (unsigned)(int)(F(double, g_gamshe696ca0, 7968) * g_gs64b658);
    int sec = (int)(t % 60);
    int min = (int)(t / 60) % 99;
    int y = y0 + 53;
    CallC(FUN_5b8980, F(int, s, 12), x0 + 260, y, F(int, s, 372 + (min / 10) * 4), -1);
    CallC(FUN_5b8980, F(int, s, 12), x0 + 268, y, F(int, s, 372 + (min % 10) * 4), -1);
    CallC(FUN_5b8980, F(int, s, 12), x0 + 276, y, F(int, s, 412), -1);
    CallC(FUN_5b8980, F(int, s, 12), x0 + 284, y, F(int, s, 372 + (sec / 10) * 4), -1);
    CallC(FUN_5b8980, F(int, s, 12), x0 + 292, y, F(int, s, 372 + (sec % 10) * 4), -1);
}

// the sound settings (+318 volume, +320) to the sound system
static void Gs_ApplySound(BYTE* s)
{
    CallC<double>(FUN_5fcd40, (float)((double)F(short, s, 318) * g_gs64b5a0));
    CallC(FUN_5ffc30, (double)(F(short, s, 318) * F(short, s, 320)) * g_gs64b930);
}

// [$L_517310] the sound page: Enter/Esc (2, 27) take the values and apply them, 184 also saves them
extern "C" void __fastcall Shell_SoundPage(BYTE* s, void*, int cmd)
{
    int ini = g_gs696868;
    if (cmd == 2 || cmd == 27) {
        F(WORD, s, 318) = F(WORD, s, 308);
        F(WORD, s, 320) = F(WORD, s, 310);
        F(WORD, s, 324) = F(WORD, s, 314);
        F(WORD, s, 326) = F(WORD, s, 316);
        CallVirt(s - 64, 32);
        CallThis(FUN_5d8610, g_gs6ced6c);
        Gs_ApplySound(s);
        if (F(short, s, 320) > 5 && !F(short, s, 328)) {
            CallC(FUN_5ffa30);
            F(short, s, 328) = 1;
        }
        if (F(short, s, 320) < 5 && F(short, s, 328)) {
            CallC(FUN_5ffb20);
            F(short, s, 328) = 0;
        }
        return;
    }
    if (cmd != 184)
        return;
    CallVirt(s - 64, 32);
    Gs_ApplySound(s);
    Gs_DropBackdrop();
    CallVirt(g_gamshe696ca0, 36);
    CallThis(FUN_5e4250, (BYTE*)g_gs696750, F(int, s, 304));
    CallC(FUN_50bd60);
    CallC(FUN_4e12b0, 1);
    CallC(FUN_603c70, (const char*)s_gs672998, (const char*)s_gs676e10, (int)F(short, s, 318), ini);
    CallC(FUN_603c70, (const char*)s_gs672998, (const char*)s_gs676e08, (int)F(short, s, 320), ini);
    CallC(FUN_603c70, (const char*)s_gs672998, (const char*)s_gs676e00, (int)F(short, s, 324), ini);
    CallC(FUN_603c70, (const char*)s_gs672998, (const char*)s_gs676df8, (int)F(short, s, 326), ini);
}

// a text centered on the column at +312 (scaled by 64b6e0, plus 64e918) at y
static void Gs_CenteredText(BYTE* s, const char* buf, int y)
{
    int w = CallThis<int>(FUN_5ea0e0, g_gamshe6ced28, buf);
    int x = (int)((F(double, s, 312) - w) * g_gs64b6e0 + g_gs64e918);
    CallC(FUN_5ea170, F(int, s, -52), x, y, g_gs6ced24, buf, 4, 0);
}

// [$L_522f60] the waiting screen: the picture 6ced5c centered, two texts and the seconds left; at the end command 161
extern "C" void __fastcall Shell_Waiting(BYTE* s, void*, int a)
{
    BYTE* d = g_69684c;
    BYTE* img = g_gs6ced5c;
    int x = (CallVirt<int>(d, 20) - F(WORD, img, 20)) / 2;
    int y = (CallVirt<int>(d, 24) - F(WORD, g_gs6ced5c, 22)) / 2;
    CallC(FUN_5b86c0, F(int, d, 12), x, y, g_gs6ced5c);
    CallThis(FUN_5c0020, s, a);
    char buf[64];
    CallC(Gs_sprintf, buf, CallC<const char*>(FUN_5d2e50, 123));
    Gs_CenteredText(s, buf, 560);
    CallC(Gs_sprintf, buf, CallC<const char*>(FUN_5d2e50, 124));
    Gs_CenteredText(s, buf, 575);
    int left = (int)(g_gs64b918 - (CallC<double>(FUN_6037e0) - F(double, s, 304)) * g_gs64b658);
    CallC(Gs_sprintf, buf, CallC<const char*>(FUN_5d2e50, 125), left);
    Gs_CenteredText(s, buf, 590);
    if (CallC<double>(FUN_6037e0) - F(double, s, 304) > g_gs64b890)
        CallVirt(s, 60, 161);
}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
extern "C" void FUN_53e490(); KING_ALIAS("_FUN_53e490", "@Obj53e490_Restart@8")
extern "C" void FUN_6021b0(); KING_ALIAS("_FUN_6021b0", "_Dp_Send")
extern "C" void FUN_51f380(); KING_ALIAS("_FUN_51f380", "@Shell_SetMode@12")
ASM_VAR(int, g_gs675460, $L_675460)
ASM_VAR(int, g_gs675464, $L_675464)
ASM_VAR(int, g_gs696850, $L_696850)
ASM_VAR(int, g_gs696854, $L_696854)
ASM_VAR(BYTE*, g_gs6ceca8, $L_6ceca8)

// [$L_522be0] the network screen: 162 starts the game (controls ready, screen centre), 163 sends a ready message
extern "C" void __fastcall Shell_NetCommand(BYTE* s, void*, int cmd)
{
    if (cmd == 162) {
        CallVirt(s - 64, 32);
        CallVirt(g_gamshe696ca0, 36);
        CallThis(FUN_53e490, F(BYTE*, g_gamshe6d2078, 1632));
        g_gs675460 = g_gs696850 / 2;
        g_gs675464 = g_gs696854 / 2;
        CallC(FUN_50bd60);
        CallC(FUN_4e12b0, 1);
        return;
    }
    if (cmd != 163)
        return;
    CallVirt(s - 64, 32);
    BYTE pkt[32];
    CallThis(FUN_602be0, pkt);
    CallThis(FUN_603110, pkt, 952, &cmd, 3);
    CallC(FUN_6021b0, F(BYTE*, pkt, 4), F(BYTE*, pkt, 8) - F(BYTE*, pkt, 4));
    CallThis(FUN_51f380, g_gs6ceca8, 4);
    CallThis(FUN_602e80, pkt);
}

// ---------------------------------------------------------------------------
// Shell buttons with their four or five image styles (exception frames dropped).

ASM_VAR(AsmLabel, g_q21_68d1c0, $L_68d1c0)
ASM_VAR(AsmLabel, g_q21_64dcb8, $L_64dcb8)
ASM_VAR(AsmLabel, s_q21_x, $L_x)
ASM_VAR(AsmLabel, s_q21_6762c4, $L_6762c4)
ASM_VAR(AsmLabel, s_q21_6762bc, $L_6762bc)
ASM_VAR(AsmLabel, s_q21_6762b0, $L_6762b0)
ASM_VAR(AsmLabel, s_q21_6762a4, $L_6762a4)
ASM_VAR(AsmLabel, s_q21_676294, $L_676294)
ASM_VAR(AsmLabel, s_q21_676284, $L_676284)
ASM_VAR(AsmLabel, s_q21_676278, $L_676278)
ASM_VAR(AsmLabel, s_q21_6762cc, $L_6762cc)
ASM_VAR(AsmLabel, s_q21_676344, $L_676344)
ASM_VAR(AsmLabel, s_q21_676374, $L_676374)
ASM_VAR(AsmLabel, s_q21_676368, $L_676368)
ASM_VAR(AsmLabel, s_q21_67635c, $L_67635c)
ASM_VAR(AsmLabel, s_q21_676350, $L_676350)
ASM_VAR(AsmLabel, s_q21_6763a4, $L_6763a4)
ASM_VAR(AsmLabel, s_q21_676398, $L_676398)
ASM_VAR(AsmLabel, s_q21_67638c, $L_67638c)
ASM_VAR(AsmLabel, s_q21_676380, $L_676380)
ASM_VAR(AsmLabel, s_q21_6763bc, $L_6763bc)
ASM_VAR(AsmLabel, s_q21_6763b0, $L_6763b0)
ASM_VAR(AsmLabel, s_q21_676460, $L_676460)
ASM_VAR(AsmLabel, s_q21_676450, $L_676450)
ASM_VAR(AsmLabel, s_q21_67643c, $L_67643c)
ASM_VAR(AsmLabel, s_q21_67642c, $L_67642c)
ASM_VAR(AsmLabel, s_q21_67641c, $L_67641c)
ASM_VAR(AsmLabel, s_q21_676408, $L_676408)

// a 36-byte image style (0.5 at +24) copied from proto, its texture (+28) the object named by the format
struct ShellImg { DWORD d[9]; };
static void Shell_Img(ShellImg& r, void* proto, const char* fmt, int a, int b = 0)
{
    char buf[128];
    sprintf(buf, fmt, a, b);
    memset(&r, 0, sizeof r);
    r.d[6] = 0x3f000000;
    CallThis(FUN_5bd5a0, &r, proto);
    r.d[7] = CallC<DWORD>(FUN_5ef3d0, buf);
}
static BYTE* Shell_NewButton(int kind, int id, bool vt)
{
    BYTE* m = (BYTE*)operator new(288);
    if (m) {
        CallThis(FUN_5bf820, m, (BYTE*)g_q21_68d1c0, kind, id);
        if (vt)
            F(void*, m, 0) = g_q21_64dcb8;
    }
    return m;
}
static void Shell_Fill(BYTE* m, ShellImg& a, ShellImg& b, ShellImg& c, ShellImg& d)
{
    CallThis(FUN_5bd5a0, m + 68, &b);
    CallThis(FUN_5bd5a0, m + 104, &a);
    CallThis(FUN_5bd5a0, m + 140, &d);
    CallThis(FUN_5bd5a0, m + 176, &c);
}
static void Shell_MoveTo(BYTE* m, int x, int y)
{
    F(int, m, 52) += x - F(int, m, 44);
    F(int, m, 44) = x;
    F(int, m, 56) += y - F(int, m, 48);
    F(int, m, 48) = y;
}
static const char* Shell_ModeName()
{
    return CallC<int>(FUN_568fd0) == 2 ? (const char*)s_q21_6762c4 : (const char*)s_q21_6762bc;
}
// the mode button shapes (6762b0 ..): the extra image (+212) from fmtE
static BYTE* Shell_ModeButton(int n, int id, int flag, const char* fmtE)
{
    BYTE* m = Shell_NewButton(4, id, true);
    if (flag)
        F(int, m, 284) |= 2;
    int sel = (int)Shell_ModeName();
    ShellImg a, b, c, d, e;
    Shell_Img(a, g_6ce3b8, (const char*)s_q21_6762b0, sel, n);
    Shell_Img(b, g_6ce408, (const char*)s_q21_6762a4, sel, n);
    Shell_Img(c, g_6ce988, (const char*)s_q21_676294, sel, n);
    Shell_Img(d, g_6ceb38, (const char*)s_q21_676284, sel, n);
    Shell_Fill(m, a, b, c, d);
    Shell_Img(e, g_6ceb38, fmtE, sel, n);
    CallThis(FUN_5bd5a0, m + 212, &e);
    CallVirt(m, 20);
    return m;
}
// [FUN_50d1c0] (n, id, flag)
extern "C" BYTE* __cdecl Shell_ModeButtonA(int n, int id, int flag) { return Shell_ModeButton(n, id, flag, (const char*)s_q21_676278); }
// [FUN_50d530] (n, id, flag)
extern "C" BYTE* __cdecl Shell_ModeButtonB(int n, int id, int flag) { return Shell_ModeButton(n, id, flag, (const char*)s_q21_6762cc); }

// [FUN_50def0] (n, id): a mode button whose pressed image is the lit one
extern "C" BYTE* __cdecl Shell_ModeButtonC(int n, int id)
{
    int sel = (int)Shell_ModeName();
    ShellImg a, b, c, d, e;
    Shell_Img(a, g_6ce3b8, (const char*)s_q21_6762b0, sel, n);
    Shell_Img(b, g_6ce408, (const char*)s_q21_6762a4, sel, n);
    Shell_Img(c, g_6ce988, (const char*)s_q21_676294, sel, n);
    Shell_Img(d, g_6ceb38, (const char*)s_q21_676294, sel, n);
    BYTE* m = Shell_NewButton(4, id, false);
    Shell_Fill(m, a, b, c, d);
    Shell_Img(e, g_6ceb38, (const char*)s_q21_676344, sel, n);
    CallThis(FUN_5bd5a0, m + 212, &e);
    CallVirt(m, 20);
    return m;
}

// a button of n's images (formats fa..fe), the extra one at +eOff
static BYTE* Shell_NamedButton(int n, int id, int kind, bool vt, const char* fa, const char* fb, const char* fc,
                               const char* fd, const char* fe, int eOff)
{
    ShellImg a, b, c, d, e;
    Shell_Img(a, g_6ce3b8, fa, n);
    Shell_Img(b, g_6ce408, fb, n);
    Shell_Img(c, g_6ce988, fc, n);
    Shell_Img(d, g_6ceb38, fd, n);
    BYTE* m = Shell_NewButton(kind, id, vt);
    Shell_Fill(m, a, b, c, d);
    Shell_Img(e, g_6ceb38, fe, n);
    CallThis(FUN_5bd5a0, m + eOff, &e);
    CallVirt(m, 20);
    return m;
}
// [FUN_50e200] (n, id)
extern "C" BYTE* __cdecl Shell_ButtonA(int n, int id)
{
    return Shell_NamedButton(n, id, 4, false, (const char*)s_q21_676374, (const char*)s_q21_676368, (const char*)s_q21_67635c,
                             (const char*)s_q21_67635c, (const char*)s_q21_676350, 212);
}
// [FUN_50ec10] (n, id, x, y, flag)
extern "C" BYTE* __cdecl Shell_ButtonC(int n, int id, int x, int y, int flag)
{
    BYTE* m = Shell_NamedButton(n, id, 4, true, (const char*)s_q21_676374, (const char*)s_q21_676368, (const char*)s_q21_67635c,
                                (const char*)s_q21_6763bc, (const char*)s_q21_6763b0, 248);
    Shell_MoveTo(m, x, y);
    if (flag)
        F(int, m, 284) |= 2;
    return m;
}
// [FUN_50f650] (n, id, x, y): images by mode, the pressed one sharing the lit texture
extern "C" BYTE* __cdecl Shell_ButtonD(int n, int id, int x, int y)
{
    bool m2 = CallC<int>(FUN_568fd0) == 2;
    ShellImg a, b, c, d;
    Shell_Img(a, g_6ce3b8, m2 ? (const char*)s_q21_676460 : (const char*)s_q21_67642c, n);
    Shell_Img(b, g_6ce408, m2 ? (const char*)s_q21_676450 : (const char*)s_q21_67641c, n);
    Shell_Img(c, g_6ce988, m2 ? (const char*)s_q21_67643c : (const char*)s_q21_676408, n);
    memset(&d, 0, sizeof d);
    d.d[6] = 0x3f000000;
    CallThis(FUN_5bd5a0, &d, (void*)g_6ceb38);
    d.d[7] = c.d[7];
    BYTE* m = Shell_NewButton(4, id, false);
    Shell_Fill(m, a, b, c, d);
    CallVirt(m, 20);
    Shell_MoveTo(m, x, y);
    return m;
}

// ---------------------------------------------------------------------------
// Shell window with its two arrow buttons (exception frame dropped).

extern "C" void FUN_5d8900(); KING_ALIAS("_FUN_5d8900", "@ControlWindow_Construct@40")
extern "C" void FUN_50ef60(); KING_ALIAS("_FUN_50ef60", "_Carat_MoveWindow")
extern "C" void FUN_5bdd70(); KING_ALIAS("_FUN_5bdd70", "@Ctl_AutoSize@8")
extern "C" void FUN_5bdbf0(); KING_ALIAS("_FUN_5bdbf0", "@Ctl_AddChild@16")
ASM_VAR(int, g_q28_720938, $L_720938)
ASM_VAR(int, g_q28_72093c, $L_72093c)
ASM_VAR(int, g_q28_720940, $L_720940)
ASM_VAR(int, g_q28_720944, $L_720944)
ASM_VAR(int, g_q28_6ced24, $L_6ced24)
ASM_VAR(DWORD, g_q28_6ce3ac, $L_6ce3ac)
ASM_VAR(AsmLabel, g_q28_6ce390, $L_6ce390)
ASM_VAR(AsmLabel, g_q28_64e118, $L_64e118)
ASM_VAR(AsmLabel, g_q28_64e0d8, $L_64e0d8)
ASM_VAR(AsmLabel, g_q28_64dc50, $L_64dc50)
ASM_VAR(AsmLabel, s_q28_676c9c, $L_676c9c)
ASM_VAR(AsmLabel, s_q28_677290, $L_677290)
ASM_VAR(AsmLabel, s_q28_67663c, $L_67663c)
ASM_VAR(AsmLabel, s_q28_67727c, $L_67727c)
ASM_VAR(AsmLabel, s_q28_677274, $L_677274)
ASM_VAR(AsmLabel, s_q28_67726c, $L_67726c)

// the four styles from 6ce390 with the texture named t
static void Shell_PlainStyles(BYTE* at, const char* t)
{
    g_q28_6ce3ac = CallC<DWORD>(FUN_5ef3d0, t);
    for (int k = 0; k < 4; k++)
        CallThis(FUN_5bd5a0, at + k * 36, (void*)g_q28_6ce390);
}
static BYTE* Shell_NewPanel(int id)
{
    BYTE* p = (BYTE*)operator new(296);
    if (p) {
        CallThis(FUN_5bd5e0, p, (const char*)s_q28_67663c, id);
        F(int, p, 288) = 0;
        F(int, p, 292) = 0;
        F(void*, p, 0) = g_q28_64dc50;
    }
    return p;
}
// [FUN_518480] constructor (parent): a shell window (vtables 64e118, 64e0d8 at +64) of the screen size, its
// background and a panel at (155, 229) with the left and right arrow buttons
extern "C" BYTE* __fastcall ShellArrows_Construct(BYTE* s, void*, int parent)
{
    CallThis(FUN_5d8900, s, parent, g_q28_720938, g_q28_72093c, g_q28_720940, g_q28_720944, 4, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q28_64e118;
    F(void*, s, 64) = g_q28_64e0d8;
    DWORD* o = (DWORD*)operator new(12);
    if (o) {
        o[0] = CallC<DWORD>(FUN_5ef3d0, (const char*)s_q28_676c9c);
        o[1] = 0;
        o[2] = 0;
    }
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    Shell_PlainStyles(s + 132, (const char*)s_q28_677290);
    BYTE* p1 = Shell_NewPanel(0);
    BYTE* p2 = Shell_NewPanel(4);
    Shell_PlainStyles(p2 + 68, (const char*)s_q28_67727c);
    CallThis(FUN_5bdbf0, p2, CallC<BYTE*>(FUN_50ef60, (const char*)s_q28_677274, 153, 290, 304), 0);
    CallThis(FUN_5bdbf0, p2, CallC<BYTE*>(FUN_50ef60, (const char*)s_q28_67726c, 154, 381, 304), 0);
    F(int, p2, 52) += 155 - F(int, p2, 44);
    F(int, p2, 44) = 155;
    F(int, p2, 56) += 229 - F(int, p2, 48);
    F(int, p2, 48) = 229;
    CallThis(FUN_5bdd70, p2);
    CallThis(FUN_5bdbf0, p1, p2, 0);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p1, 0);
    return s;
}

// ---------------------------------------------------------------------------
// Shell screen below its parent with two buttons (exception frame dropped).

ASM_PROC(FUN_5bdc80)
ASM_VAR(DWORD, g_q29_6ce3fc, $L_6ce3fc)
ASM_VAR(AsmLabel, g_q29_6ce3e0, $L_6ce3e0)
ASM_VAR(AsmLabel, g_q29_64e848, $L_64e848)
ASM_VAR(AsmLabel, g_q29_64e808, $L_64e808)
ASM_VAR(AsmLabel, s_q29_677aa0, $L_677aa0)
ASM_VAR(AsmLabel, s_q29_677a88, $L_677a88)
ASM_VAR(AsmLabel, s_q29_677a80, $L_677a80)
ASM_VAR(AsmLabel, s_q29_677a78, $L_677a78)
ASM_VAR(int, g_q29_6ced14, $L_6ced14)
ASM_VAR(int, g_q29_6ced18, $L_6ced18)

static void Shell_MoveCtl(BYTE* m, int x, int y)
{
    F(int, m, 52) += x - F(int, m, 44);
    F(int, m, 44) = x;
    F(int, m, 56) += y - F(int, m, 48);
    F(int, m, 48) = y;
}
// [FUN_5228e0] constructor (parent): a shell screen (vtables 64e848, 64e808 at +64) in the area below its
// parent, a panel with two mode buttons (163, 162) placed at 6ced14
extern "C" BYTE* __fastcall ShellScreen5228e0_Construct(BYTE* s, void*, BYTE* parent)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q29_64e848;
    F(void*, s, 64) = g_q29_64e808;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    BYTE* p = Shell_NewPanel(4);
    g_q29_6ce3fc = CallC<DWORD>(FUN_5ef3d0, CallC<int>(FUN_568fd0) == 2 ? (const char*)s_q29_677aa0 : (const char*)s_q29_677a88);
    for (int k = 0; k < 4; k++)
        CallThis(FUN_5bd5a0, p + 68 + k * 36, (void*)g_q29_6ce3e0);
    BYTE* b = Shell_ModeButtonC((int)(const char*)s_q29_677a80, 163);
    CallThis(FUN_5bdbf0, p, b, 0);
    Shell_MoveCtl(b, 56, 729);
    b = Shell_ModeButtonC((int)(const char*)s_q29_677a78, 162);
    CallThis(FUN_5bdbf0, p, b, 0);
    Shell_MoveCtl(b, 56, 689);
    CallThis(FUN_5bdc80, p);
    Shell_MoveCtl(p, g_q29_6ced14, g_q29_6ced18);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p, 0);
    return s;
}

// ---------------------------------------------------------------------------
// Centred shell dialog with two mode buttons (exception frame dropped).

ASM_VAR(BYTE*, g_q30_6ce3b0, $L_6ce3b0)
ASM_VAR(int, g_q30_68b990, $L_68b990)
ASM_VAR(AsmLabel, g_q30_64e9e8, $L_64e9e8)
ASM_VAR(AsmLabel, g_q30_64e9a8, $L_64e9a8)
ASM_VAR(AsmLabel, s_q30_6780a0, $L_6780a0)
ASM_VAR(AsmLabel, s_q30_678088, $L_678088)
ASM_VAR(AsmLabel, s_q30_67807c, $L_67807c)
ASM_VAR(AsmLabel, s_q30_678074, $L_678074)

// [FUN_525b50] constructor (parent): a dialog (vtables 64e9e8, 64e9a8 at +64) whose panel with its background
// (6ce3b0) and buttons 188, 189 is centred on the parent
extern "C" BYTE* __fastcall ShellDialog525b50_Construct(BYTE* s, void*, BYTE* parent)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, 0);
    for (int k = 0; k < 5; k++)
        F(int, s, 380 + k * 4) = g_q30_68b990;
    F(void*, s, 0) = g_q30_64e9e8;
    F(void*, s, 64) = g_q30_64e9a8;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    BYTE* p1 = Shell_NewPanel(0);
    BYTE* p2 = Shell_NewPanel(8);
    g_q30_6ce3b0 = CallC<BYTE*>(FUN_5ef3d0, CallC<int>(FUN_568fd0) == 2 ? (const char*)s_q30_6780a0 : (const char*)s_q30_678088);
    for (int k = 0; k < 4; k++)
        CallThis(FUN_5bd5a0, p2 + 68 + k * 36, (void*)g_q28_6ce390);
    BYTE* b = Shell_ModeButtonC((int)(const char*)s_q30_67807c, 188);
    CallThis(FUN_5bdbf0, p2, b, 0);
    Shell_MoveCtl(b, 30, 135);
    b = Shell_ModeButtonC((int)(const char*)s_q30_678074, 189);
    CallThis(FUN_5bdbf0, p2, b, 0);
    Shell_MoveCtl(b, 137, 146);
    CallThis(FUN_5bdbf0, p1, p2, 0);
    CallThis(FUN_5bdc80, p1);
    int x = (CallVirt<int>(parent, 20) - F(WORD, g_q30_6ce3b0, 4)) / 2;
    int y = (CallVirt<int>(parent, 24) - F(WORD, g_q30_6ce3b0, 6)) / 2;
    Shell_MoveCtl(p1, x, y);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p1, 0);
    return s;
}

ASM_VAR(AsmLabel, g_q30_64e620, $L_64e620)
ASM_VAR(AsmLabel, g_q30_64e5e0, $L_64e5e0)
ASM_VAR(AsmLabel, s_q30_676808, $L_676808)
ASM_VAR(AsmLabel, s_q30_6767f0, $L_6767f0)
ASM_VAR(AsmLabel, s_q30_677824, $L_677824)
ASM_VAR(AsmLabel, s_q30_67781c, $L_67781c)
ASM_VAR(AsmLabel, s_q30_677814, $L_677814)
ASM_VAR(BYTE*, g_q30_6cec6c, $L_6cec6c)
ASM_VAR(int, g_q30_6ced0c, $L_6ced0c)
ASM_VAR(int, g_q30_6ced10, $L_6ced10)

// [FUN_51e9a0] constructor (parent): a shell screen (vtables 64e620, 64e5e0 at +64), a panel at 6ced0c holding
// buttons 172, 171, 170 (the last on in a network game)
extern "C" BYTE* __fastcall ShellScreen51e9a0_Construct(BYTE* s, void*, BYTE* parent)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q30_64e620;
    F(void*, s, 64) = g_q30_64e5e0;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    BYTE* p1 = Shell_NewPanel(4);
    Shell_PlainStyles(p1 + 68, CallC<int>(FUN_568fd0) == 2 ? (const char*)s_q30_676808 : (const char*)s_q30_6767f0);
    BYTE* p2 = Shell_NewPanel(0);
    BYTE* b = Shell_ModeButtonC((int)(const char*)s_q30_677824, 172);
    CallThis(FUN_5bdbf0, p2, b, 0);
    Shell_MoveCtl(b, 18, 664);
    b = Shell_ModeButtonC((int)(const char*)s_q30_67781c, 171);
    CallThis(FUN_5bdbf0, p2, b, 0);
    Shell_MoveCtl(b, 18, 614);
    b = Shell_ModeButtonC((int)(const char*)s_q30_677814, 170);
    CallThis(FUN_5bdbf0, p2, b, 0);
    if (F(int, g_q30_6cec6c, 1316))
        F(int, b, 40) = 1;
    Shell_MoveCtl(b, 18, 564);
    CallThis(FUN_5bdc80, p2);
    CallThis(FUN_5bdbf0, p1, p2, 0);
    CallThis(FUN_5bdc80, p1);
    Shell_MoveCtl(p1, g_q30_6ced0c, g_q30_6ced10);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p1, 0);
    F(int, s, 368) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Shell screen with a slider and three buttons (exception frame dropped).

ASM_PROC(FUN_50d860)
ASM_VAR(AsmLabel, g_q31_64e730, $L_64e730)
ASM_VAR(AsmLabel, g_q31_64e6f0, $L_64e6f0)
ASM_VAR(AsmLabel, s_q31_6779dc, $L_6779dc)
ASM_VAR(AsmLabel, s_q31_6779c8, $L_6779c8)
ASM_VAR(AsmLabel, s_q31_6779c0, $L_6779c0)
ASM_VAR(AsmLabel, s_q31_6779b8, $L_6779b8)
ASM_VAR(AsmLabel, s_q31_6779b0, $L_6779b0)

// the panel styles from 6ce3e0 with the texture (6ce3fc) by mode
static void Shell_PanelStyles(BYTE* at, const char* m2, const char* other)
{
    g_q29_6ce3fc = CallC<DWORD>(FUN_5ef3d0, CallC<int>(FUN_568fd0) == 2 ? m2 : other);
    for (int k = 0; k < 4; k++)
        CallThis(FUN_5bd5a0, at + k * 36, (void*)g_q29_6ce3e0);
}
// [FUN_520240] constructor (parent): a shell screen (vtables 64e730, 64e6f0 at +64): buttons 2, 275, the mode
// button 276 (+404) and a slider (155) on the value +400, the panel at 6ced14
extern "C" BYTE* __fastcall ShellScreen520240_Construct(BYTE* s, void*, BYTE* parent)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q31_64e730;
    F(void*, s, 64) = g_q31_64e6f0;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    BYTE* p1 = Shell_NewPanel(4);
    Shell_PanelStyles(p1 + 68, (const char*)s_q31_6779dc, (const char*)s_q31_6779c8);
    BYTE* p2 = Shell_NewPanel(0);
    BYTE* b = Shell_ModeButtonC((int)(const char*)s_q31_6779c0, 2);
    CallThis(FUN_5bdbf0, p2, b, 0);
    Shell_MoveCtl(b, 123, 755);
    b = Shell_ModeButtonC((int)(const char*)s_q31_6779b8, 275);
    CallThis(FUN_5bdbf0, p2, b, 0);
    Shell_MoveCtl(b, 52, 755);
    b = Shell_ModeButtonA((int)(const char*)s_q31_6779b0, 276, 0);
    F(BYTE*, s, 404) = b;
    CallThis(FUN_5bdbf0, p2, b, 0);
    Shell_MoveCtl(b, 175, 655);
    b = CallC<BYTE*>(FUN_50d860, 0, 0, s + 400, 0, 155);
    CallThis(FUN_5bdbf0, p2, b, 0);
    F(int, b, 28) = 278;
    Shell_MoveCtl(b, 25, 655);
    CallThis(FUN_5bdbf0, p1, p2, 0);
    CallThis(FUN_5bdc80, p1);
    Shell_MoveCtl(p1, g_q29_6ced14, g_q29_6ced18);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p1, 0);
    return s;
}

extern "C" void FUN_5c07c0(); KING_ALIAS("_FUN_5c07c0", "@List_Layout@12")
extern "C" void FUN_5c0550(); KING_ALIAS("_FUN_5c0550", "@ListBox_Construct@20")
extern "C" void FUN_5becd0(); KING_ALIAS("_FUN_5becd0", "@Split_Resize@12")
ASM_VAR(int, g_q31_6ced2c, $L_6ced2c)
ASM_VAR(AsmLabel, g_q31_64e598, $L_64e598)
ASM_VAR(AsmLabel, g_q31_64e558, $L_64e558)
ASM_VAR(AsmLabel, s_q31_6777fc, $L_6777fc)
ASM_VAR(AsmLabel, s_q31_6777e4, $L_6777e4)
ASM_VAR(AsmLabel, s_q31_6777a4, $L_6777a4)
ASM_VAR(AsmLabel, s_q31_67779c, $L_67779c)
ASM_VAR(AsmLabel, s_q31_6777dc, $L_6777dc)
ASM_VAR(AsmLabel, s_q31_677788, $L_677788)
ASM_VAR(AsmLabel, s_q31_677780, $L_677780)
ASM_VAR(AsmLabel, s_q31_67777c, $L_67777c)

// [FUN_51e340] constructor (parent, -): a shell screen (vtables 64e598, 64e558 at +64): buttons 172, 282 and the
// lit 110 (+500), a list box 190 of 10 lines (+504) with its scroll bar; the panel at 6ced0c
extern "C" BYTE* __fastcall ShellScreen51e340_Construct(BYTE* s, void*, BYTE* parent, int)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, g_q31_6ced2c);
    F(void*, s, 0) = g_q31_64e598;
    F(void*, s, 64) = g_q31_64e558;
    F(DWORD*, s, 48) = Shell_NewObjRef();
    BYTE* p = Shell_NewPanel(4);
    Shell_PlainStyles(p + 68, CallC<int>(FUN_568fd0) == 2 ? (const char*)s_q31_6777fc : (const char*)s_q31_6777e4);
    BYTE* b = Shell_ModeButtonC((int)(const char*)s_q31_6777a4, 172);
    CallThis(FUN_5bdbf0, p, b, 0);
    Shell_MoveCtl(b, 24, 769);
    b = Shell_ModeButtonC((int)(const char*)s_q31_67779c, 282);
    CallThis(FUN_5bdbf0, p, b, 0);
    Shell_MoveCtl(b, 24, 724);
    b = Shell_ModeButtonC((int)(const char*)s_q31_6777dc, 110);
    F(BYTE*, s, 500) = b;
    CallThis(FUN_5bdbf0, p, b, 0);
    F(int, b, 40) = 1;
    Shell_MoveCtl(b, 24, 679);
    BYTE* m = (BYTE*)operator new(352);
    BYTE* l = m ? CallThis<BYTE*>(FUN_5c0550, m, (const char*)s_q31_677788, 4, 190) : 0;
    F(BYTE*, s, 504) = l;
    CallThis(FUN_5bd5a0, l + 68, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, l + 104, (void*)g_6ce3b8);
    CallThis(FUN_5bd5a0, l + 140, (void*)g_6ceb38);
    CallThis(FUN_5bd5a0, l + 176, (void*)g_6ce988);
    F(int, F(BYTE*, F(BYTE*, s, 504), 340), 28) = 116;
    F(int, F(BYTE*, s, 504), 28) = 116;
    CallThis(FUN_5c07c0, F(BYTE*, s, 504), 10);
    Button_SetImages(F(BYTE*, F(BYTE*, s, 504), 340), (const char*)s_q31_67777c, (const char*)s_q31_677780);
    Shell_MoveCtl(F(BYTE*, F(BYTE*, s, 504), 340), -28, 11);
    CallThis(FUN_5becd0, F(BYTE*, F(BYTE*, s, 504), 340), 195);
    Shell_MoveCtl(F(BYTE*, s, 504), 44, 457);
    CallThis(FUN_5bdbf0, p, F(BYTE*, s, 504), 0);
    CallThis(FUN_5bdc80, p);
    Shell_MoveCtl(p, g_q30_6ced0c, g_q30_6ced10);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p, 0);
    F(int, s, 368) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Shell screen with four sliders read from the ini (exception frame dropped).

extern "C" void FUN_5be130(); KING_ALIAS("_FUN_5be130", "@Ctl_LayoutRow@12")
ASM_VAR(BYTE*, g_q32_696868, $L_696868)
ASM_VAR(AsmLabel, g_q32_64df80, $L_64df80)
ASM_VAR(AsmLabel, g_q32_64df40, $L_64df40)
extern "C" void __cdecl Shell_MusicSlider(void);
extern "C" void __cdecl Shell_EffectsSlider(void);
ASM_VAR(AsmLabel, s_q32_672998, $L_672998)
#define Q32(x) ((const char*)s_q32_##x)
ASM_VAR(AsmLabel, s_q32_676e78, $L_676e78)
ASM_VAR(AsmLabel, s_q32_676e64, $L_676e64)
ASM_VAR(AsmLabel, s_q32_676e60, $L_676e60)
ASM_VAR(AsmLabel, s_q32_676e58, $L_676e58)
ASM_VAR(AsmLabel, s_q32_676e50, $L_676e50)
ASM_VAR(AsmLabel, s_q32_676e48, $L_676e48)
ASM_VAR(AsmLabel, s_q32_676e40, $L_676e40)
ASM_VAR(AsmLabel, s_q32_676e38, $L_676e38)
ASM_VAR(AsmLabel, s_q32_676e30, $L_676e30)
ASM_VAR(AsmLabel, s_q32_676e28, $L_676e28)
ASM_VAR(AsmLabel, s_q32_676e20, $L_676e20)
ASM_VAR(AsmLabel, s_q32_676e18, $L_676e18)
ASM_VAR(AsmLabel, s_q32_676e10, $L_676e10)
ASM_VAR(AsmLabel, s_q32_676e08, $L_676e08)
ASM_VAR(AsmLabel, s_q32_676e00, $L_676e00)
ASM_VAR(AsmLabel, s_q32_676df8, $L_676df8)

static BYTE* Shell_AddSlider(BYTE* p, const char* a, const char* b, BYTE* value, void* fn, int id)
{
    BYTE* c = CallC<BYTE*>(FUN_50d860, a, b, value, fn, 140);
    CallThis(FUN_5bdbf0, p, c, 0);
    F(int, c, 28) = id;
    return c;
}
static WORD Shell_IniWord(const char* key)
{
    int v;
    CallC(FUN_603b70, Q32(672998), key, 50, &v, g_q32_696868);
    return (WORD)v;
}
// [FUN_516ea0] constructor (parent): a settings screen (vtables 64df80, 64df40 at +64): buttons 184, 2 and four
// sliders 183..180 on the words +390, +388, +384, +382 read from the ini (50 by default)
extern "C" BYTE* __fastcall ShellSliders516ea0_Construct(BYTE* s, void*, BYTE* parent)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q32_64df80;
    F(void*, s, 64) = g_q32_64df40;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    F(int, s, 368) = 0;
    BYTE* p1 = Shell_NewPanel(4);
    Shell_PlainStyles(p1 + 68, CallC<int>(FUN_568fd0) == 2 ? Q32(676e78) : Q32(676e64));
    BYTE* p2 = Shell_NewPanel(0);
    BYTE* b = Shell_ModeButtonC((int)Q32(676e60), 184);
    CallThis(FUN_5bdbf0, p2, b, 0);
    Shell_MoveCtl(b, 21, 762);
    b = Shell_ModeButtonC((int)Q32(676e58), 2);
    CallThis(FUN_5bdbf0, p2, b, 0);
    Shell_MoveCtl(b, 77, 762);
    CallThis(FUN_5bdbf0, p1, p2, 0);
    BYTE* p3 = Shell_NewPanel(0);
    Shell_AddSlider(p3, Q32(676e48), Q32(676e50), s + 390, 0, 183);
    Shell_AddSlider(p3, Q32(676e38), Q32(676e40), s + 388, 0, 182);
    Shell_AddSlider(p3, Q32(676e28), Q32(676e30), s + 384, (void*)Shell_MusicSlider, 181);
    b = Shell_AddSlider(p3, Q32(676e18), Q32(676e20), s + 382, (void*)Shell_EffectsSlider, 180);
    CallThis(FUN_5be130, p3, F(int, b, 48) - F(int, b, 56) + 70);
    Shell_MoveCtl(p3, 22, 489);
    CallThis(FUN_5bdbf0, p1, p3, 0);
    CallThis(FUN_5bdc80, p1);
    Shell_MoveCtl(p1, g_q30_6ced0c, g_q30_6ced10);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p1, 0);
    F(WORD, s, 382) = Shell_IniWord(Q32(676e10));
    F(WORD, s, 384) = Shell_IniWord(Q32(676e08));
    F(WORD, s, 388) = Shell_IniWord(Q32(676e00));
    F(WORD, s, 390) = Shell_IniWord(Q32(676df8));
    F(WORD, s, 386) = 50;
    return s;
}

// ---------------------------------------------------------------------------
// Save screen with a list and a name field (exception frame dropped).

ASM_VAR(AsmLabel, g_q33_64e510, $L_64e510)
ASM_VAR(AsmLabel, g_q33_64e4d0, $L_64e4d0)
ASM_VAR(AsmLabel, g_q33_64e160, $L_64e160)
ASM_VAR(AsmLabel, g_q33_68d1c0, $L_68d1c0)
ASM_VAR(AsmLabel, s_q33_6777c4, $L_6777c4)
ASM_VAR(AsmLabel, s_q33_6777ac, $L_6777ac)
ASM_VAR(AsmLabel, s_q33_677794, $L_677794)

// [FUN_51dab0] constructor (parent, -): a save screen (vtables 64e510, 64e4d0 at +64): buttons 172, 282, 173,
// a list box 160 (+500) and a 32-character name field 174 (+504) on the text +372
extern "C" BYTE* __fastcall ShellSave51dab0_Construct(BYTE* s, void*, BYTE* parent, int)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, g_q31_6ced2c);
    F(void*, s, 0) = g_q33_64e510;
    F(void*, s, 64) = g_q33_64e4d0;
    DWORD st[9] = {0, 5, 0, 0, 0, 0, 0x3f000000, 0, 0};
    F(DWORD*, s, 48) = Shell_NewObjRef();
    BYTE* p = Shell_NewPanel(4);
    Shell_PlainStyles(p + 68, CallC<int>(FUN_568fd0) == 2 ? (const char*)s_q33_6777c4 : (const char*)s_q33_6777ac);
    BYTE* b = Shell_ModeButtonC((int)(const char*)s_q31_6777a4, 172);
    CallThis(FUN_5bdbf0, p, b, 0);
    Shell_MoveCtl(b, 24, 769);
    b = Shell_ModeButtonC((int)(const char*)s_q31_67779c, 282);
    CallThis(FUN_5bdbf0, p, b, 0);
    Shell_MoveCtl(b, 24, 724);
    b = Shell_ModeButtonC((int)(const char*)s_q33_677794, 173);
    CallThis(FUN_5bdbf0, p, b, 0);
    if (F(int, g_q30_6cec6c, 1316))
        F(int, b, 40) = 1;
    Shell_MoveCtl(b, 24, 679);
    BYTE* m = (BYTE*)operator new(352);
    BYTE* l = m ? CallThis<BYTE*>(FUN_5c0550, m, (const char*)s_q31_677788, 4, 160) : 0;
    F(BYTE*, s, 500) = l;
    CallThis(FUN_5bd5a0, l + 68, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, l + 104, (void*)g_6ce3b8);
    CallThis(FUN_5bd5a0, l + 140, (void*)g_6ceb38);
    CallThis(FUN_5bd5a0, l + 176, (void*)g_6ce988);
    l = F(BYTE*, s, 500);
    F(int, F(BYTE*, l, 340), 28) = 117;
    F(int, l, 28) = 117;
    CallThis(FUN_5c07c0, l, 10);
    Button_SetImages(F(BYTE*, l, 340), (const char*)s_q31_67777c, (const char*)s_q31_677780);
    Shell_MoveCtl(l, 44, 497);
    Shell_MoveCtl(F(BYTE*, l, 340), -28, 11);
    CallThis(FUN_5becd0, F(BYTE*, l, 340), 155);
    CallThis(FUN_5bdbf0, p, l, 0);
    BYTE* e = (BYTE*)operator new(300);
    if (e) {
        CallThis(FUN_5bd5e0, e, (const char*)g_q33_68d1c0, 0);
        F(void*, e, 0) = g_q33_64e160;
        F(BYTE*, e, 288) = s + 372;
        F(int, e, 292) = 32;
        F(int, e, 296) = 0;
    }
    F(BYTE*, s, 504) = e;
    for (int k = 0; k < 4; k++)
        CallThis(FUN_5bd5a0, e + 68 + k * 36, st);
    e = F(BYTE*, s, 504);
    F(int, e, 28) = 174;
    CallVirt(e, 20);
    Shell_MoveCtl(e, 44, 472);
    F(int, e, 52) = F(int, e, 44) + 100;
    F(int, e, 32) |= 0x40000000;
    CallThis(FUN_5bdbf0, p, e, 0);
    CallThis(FUN_5bdc80, p);
    Shell_MoveCtl(p, g_q30_6ced0c, g_q30_6ced10);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p, 0);
    F(int, s, 508) = 0;
    F(int, s, 368) = 0;
    F(BYTE, s, 372) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Centred yes/no dialog (exception frame dropped).

ASM_VAR(BYTE*, g_q34_6ce400, $L_6ce400)
ASM_VAR(AsmLabel, g_q34_64e8d0, $L_64e8d0)
ASM_VAR(AsmLabel, g_q34_64e890, $L_64e890)
ASM_VAR(AsmLabel, s_q34_677ad8, $L_677ad8)
ASM_VAR(AsmLabel, s_q34_677ac4, $L_677ac4)
ASM_VAR(AsmLabel, s_q34_677abc, $L_677abc)
ASM_VAR(AsmLabel, s_q34_677ab8, $L_677ab8)

// [FUN_522cf0] constructor (parent): a dialog (vtables 64e8d0, 64e890 at +64) with buttons 160, 161 centred on
// the parent; its background width as a double at +376
extern "C" BYTE* __fastcall ShellDialog522cf0_Construct(BYTE* s, void*, BYTE* parent)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q34_64e8d0;
    F(void*, s, 64) = g_q34_64e890;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    BYTE* p = Shell_NewPanel(8);
    g_q34_6ce400 = CallC<BYTE*>(FUN_5ef3d0, CallC<int>(FUN_568fd0) == 2 ? (const char*)s_q34_677ad8 : (const char*)s_q34_677ac4);
    F(double, s, 376) = (double)(int)F(WORD, g_q34_6ce400, 4);
    for (int k = 0; k < 4; k++)
        CallThis(FUN_5bd5a0, p + 68 + k * 36, (void*)g_q29_6ce3e0);
    BYTE* b = Shell_ModeButtonC((int)(const char*)s_q34_677abc, 160);
    CallThis(FUN_5bdbf0, p, b, 0);
    Shell_MoveCtl(b, 128, 96);
    b = Shell_ModeButtonC((int)(const char*)s_q34_677ab8, 161);
    CallThis(FUN_5bdbf0, p, b, 0);
    Shell_MoveCtl(b, 266, 96);
    CallThis(FUN_5bdc80, p);
    int w = F(WORD, g_q34_6ce400, 4);
    int x = CallVirt<int>(parent, 20) / 2 - w / 2;
    int h = F(WORD, g_q34_6ce400, 6);
    int y = CallVirt<int>(parent, 24) / 2 - h / 2;
    Shell_MoveCtl(p, x, y);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p, 0);
    return s;
}

ASM_VAR(AsmLabel, g_q34_64e008, $L_64e008)
ASM_VAR(AsmLabel, g_q34_64dfc8, $L_64dfc8)
ASM_VAR(AsmLabel, s_q34_672994, $L_672994)
#define Q34(x) ((const char*)s_q34_##x)
ASM_VAR(AsmLabel, s_q34_676f04, $L_676f04)
ASM_VAR(AsmLabel, s_q34_676eec, $L_676eec)
ASM_VAR(AsmLabel, s_q34_676ee4, $L_676ee4)
ASM_VAR(AsmLabel, s_q34_676edc, $L_676edc)
ASM_VAR(AsmLabel, s_q34_676ed4, $L_676ed4)
ASM_VAR(AsmLabel, s_q34_676ecc, $L_676ecc)
ASM_VAR(AsmLabel, s_q34_676ec4, $L_676ec4)
ASM_VAR(AsmLabel, s_q34_676ebc, $L_676ebc)
ASM_VAR(AsmLabel, s_q34_676eb4, $L_676eb4)
ASM_VAR(AsmLabel, s_q34_676eac, $L_676eac)
ASM_VAR(AsmLabel, s_q34_676ea4, $L_676ea4)
ASM_VAR(AsmLabel, s_q34_676e9c, $L_676e9c)
ASM_VAR(AsmLabel, s_q34_676e94, $L_676e94)
ASM_VAR(AsmLabel, s_q34_676e8c, $L_676e8c)

// 1 when the ini key (section 672998) holds the 672994 text (also its default)
static WORD Shell_IniFlag(const char* key)
{
    char buf[16];
    GetPrivateProfileStringA(Q32(672998), key, Q34(672994), buf, 15, (const char*)g_q32_696868);
    return strcmp(buf, Q34(672994)) == 0 ? 1 : 0;
}
// [FUN_517530] constructor (parent): a sound settings screen (vtables 64e008, 64dfc8 at +64): three sliders
// 177..175 (+392, +390, +388), the switches +394/+396 from the ini, buttons 2, 184 and the mode button 178 (+384)
extern "C" BYTE* __fastcall ShellSound517530_Construct(BYTE* s, void*, BYTE* parent)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q34_64e008;
    F(void*, s, 64) = g_q34_64dfc8;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    F(int, s, 368) = 0;
    BYTE* p1 = Shell_NewPanel(4);
    Shell_PlainStyles(p1 + 68, CallC<int>(FUN_568fd0) == 2 ? Q34(676f04) : Q34(676eec));
    BYTE* p2 = Shell_NewPanel(0);
    Shell_AddSlider(p2, Q34(676edc), Q34(676ee4), s + 392, 0, 177);
    Shell_AddSlider(p2, Q34(676ecc), Q34(676ed4), s + 390, 0, 176);
    BYTE* b = Shell_AddSlider(p2, Q34(676ebc), Q34(676ec4), s + 388, 0, 175);
    CallThis(FUN_5be130, p2, F(int, b, 48) - F(int, b, 56) + 70);
    Shell_MoveCtl(p2, 22, 511);
    CallThis(FUN_5bdbf0, p1, p2, 0);
    F(WORD, s, 394) = Shell_IniFlag(Q34(676eb4));
    F(WORD, s, 396) = Shell_IniFlag(Q34(676eac));
    b = Shell_ModeButtonC((int)Q32(676e58), 2);
    CallThis(FUN_5bdbf0, p1, b, 0);
    Shell_MoveCtl(b, 77, 762);
    b = Shell_ModeButtonC((int)Q32(676e60), 184);
    CallThis(FUN_5bdbf0, p1, b, 0);
    Shell_MoveCtl(b, 21, 762);
    F(BYTE*, s, 384) = Shell_ModeButtonA((int)Q34(676ea4), 178, F(short, s, 394));
    CallThis(FUN_5bdbf0, p1, F(BYTE*, s, 384), 0);
    Shell_MoveCtl(F(BYTE*, s, 384), 141, 700);
    CallThis(FUN_5bdc80, p1);
    Shell_MoveCtl(p1, g_q30_6ced0c, g_q30_6ced10);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p1, 0);
    F(WORD, s, 388) = Shell_IniWord(Q34(676e9c));
    F(WORD, s, 390) = Shell_IniWord(Q34(676e94));
    F(WORD, s, 392) = Shell_IniWord(Q34(676e8c));
    return s;
}

// ---------------------------------------------------------------------------
// Picture viewer screen with two button rows (exception frame dropped).

extern "C" void FUN_5bdee0(); KING_ALIAS("_FUN_5bdee0", "@Ctl_LayoutColumn@12")
ASM_VAR(BYTE*, g_q35_69684c, $L_69684c)
ASM_VAR(AsmLabel, g_q35_64e6a8, $L_64e6a8)
ASM_VAR(AsmLabel, g_q35_64e668, $L_64e668)
#define Q35(x) ((const char*)s_q35_##x)
ASM_VAR(AsmLabel, s_q35_677980, $L_677980)
ASM_VAR(AsmLabel, s_q35_67796c, $L_67796c)
ASM_VAR(AsmLabel, s_q35_677958, $L_677958)
ASM_VAR(AsmLabel, s_q35_677944, $L_677944)
ASM_VAR(AsmLabel, s_q35_677930, $L_677930)
ASM_VAR(AsmLabel, s_q35_67791c, $L_67791c)
ASM_VAR(AsmLabel, s_q35_677908, $L_677908)
ASM_VAR(AsmLabel, s_q35_6778f4, $L_6778f4)
ASM_VAR(AsmLabel, s_q35_6778e0, $L_6778e0)
ASM_VAR(AsmLabel, s_q35_6778cc, $L_6778cc)
ASM_VAR(AsmLabel, s_q35_6778b8, $L_6778b8)
ASM_VAR(AsmLabel, s_q35_6778a4, $L_6778a4)
ASM_VAR(AsmLabel, s_q35_677890, $L_677890)
ASM_VAR(AsmLabel, s_q35_67787c, $L_67787c)
ASM_VAR(AsmLabel, s_q35_677868, $L_677868)
ASM_VAR(AsmLabel, s_q35_677854, $L_677854)
ASM_VAR(AsmLabel, s_q35_67784c, $L_67784c)
ASM_VAR(AsmLabel, s_q35_677844, $L_677844)
ASM_VAR(AsmLabel, s_q35_67783c, $L_67783c)
ASM_VAR(AsmLabel, s_q35_677834, $L_677834)
ASM_VAR(AsmLabel, s_q35_67782c, $L_67782c)

static int Shell_FindByMode(const char* m2, const char* other)
{
    return CallC<int>(FUN_5ef3d0, CallC<int>(FUN_568fd0) == 2 ? m2 : other);
}
// [FUN_51eea0] constructor (parent): a full-screen viewer (vtables 64e6a8, 64e668 at +64) with eight pages
// (+368..+396, the first also 6ce3fc), two button rows (+436: 163, 168; +440: 163, 172, 168) centred on the screen
extern "C" BYTE* __fastcall ShellViewer51eea0_Construct(BYTE* s, void*, int parent)
{
    CallThis(FUN_5d8900, s, parent, g_q28_720938, g_q28_72093c, g_q28_720940, g_q28_720944, 4, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q35_64e6a8;
    F(void*, s, 64) = g_q35_64e668;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    BYTE* p = Shell_NewPanel(4);
    g_q29_6ce3fc = Shell_FindByMode(Q35(677980), Q35(67796c));
    F(int, s, 368) = g_q29_6ce3fc;
    F(int, s, 372) = Shell_FindByMode(Q35(677958), Q35(677944));
    F(int, s, 376) = Shell_FindByMode(Q35(677930), Q35(67791c));
    F(int, s, 380) = Shell_FindByMode(Q35(677908), Q35(6778f4));
    F(int, s, 384) = Shell_FindByMode(Q35(6778e0), Q35(6778cc));
    F(int, s, 388) = Shell_FindByMode(Q35(6778b8), Q35(6778a4));
    F(int, s, 392) = Shell_FindByMode(Q35(677890), Q35(67787c));
    F(int, s, 396) = Shell_FindByMode(Q35(677868), Q35(677854));
    F(int, s, 400) = 0;
    for (int k = 0; k < 4; k++)
        CallThis(FUN_5bd5a0, p + 68 + k * 36, (void*)g_q29_6ce3e0);
    BYTE* q = Shell_NewPanel(4);
    F(BYTE*, s, 436) = q;
    F(BYTE*, s, 416) = Shell_ModeButtonC((int)Q35(67784c), 163);
    CallThis(FUN_5bdbf0, q, F(BYTE*, s, 416), 0);
    BYTE* b = Shell_ModeButtonC((int)Q35(677844), 168);
    F(BYTE*, s, 420) = b;
    CallThis(FUN_5bdbf0, q, b, 0);
    CallThis(FUN_5bdee0, q, F(int, b, 44) - F(int, b, 52) + 290);
    Shell_MoveCtl(q, 589, 687);
    CallThis(FUN_5bdbf0, p, q, 0);
    q = Shell_NewPanel(4);
    F(BYTE*, s, 440) = q;
    F(BYTE*, s, 424) = Shell_ModeButtonC((int)Q35(67783c), 163);
    CallThis(FUN_5bdbf0, q, F(BYTE*, s, 424), 0);
    F(BYTE*, s, 428) = Shell_ModeButtonC((int)Q35(677834), 172);
    CallThis(FUN_5bdbf0, q, F(BYTE*, s, 428), 0);
    b = Shell_ModeButtonC((int)Q35(67782c), 168);
    F(BYTE*, s, 432) = b;
    CallThis(FUN_5bdbf0, q, b, 0);
    CallThis(FUN_5bdee0, q, F(int, b, 44) - F(int, b, 52) + 145);
    Shell_MoveCtl(q, 589, 687);
    CallThis(FUN_5bdbf0, p, q, 0);
    CallThis(FUN_5bdc80, p);
    BYTE* scr = g_q35_69684c;
    BYTE* t = (BYTE*)g_q29_6ce3fc;
    int x = (CallVirt<int>(scr, 20) - F(WORD, t, 20)) / 2;
    int w = F(WORD, (BYTE*)g_q29_6ce3fc, 22);
    int y = (CallVirt<int>(scr, 24) - w) / 2;
    Shell_MoveCtl(p, x, y);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p, 0);
    F(int, s, 408) = 0;
    F(int, s, 412) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Shell screen with four columns of mode buttons (exception frame dropped).

ASM_VAR(AsmLabel, g_q36_64e7c0, $L_64e7c0)
ASM_VAR(AsmLabel, g_q36_64e780, $L_64e780)
ASM_VAR(AsmLabel, s_q36_677a58, $L_677a58)
ASM_VAR(AsmLabel, s_q36_677a40, $L_677a40)
ASM_VAR(AsmLabel, s_q36_677a38, $L_677a38)
ASM_VAR(AsmLabel, s_q36_677a2c, $L_677a2c)
ASM_VAR(AsmLabel, s_q36_677a24, $L_677a24)

// a column of n buttons with ids id, id-2, .. at x, from y up by 19
static void Shell_ButtonColumn(BYTE* p, BYTE* (__cdecl* make)(int, int, int), int n, int id, int x, int y)
{
    for (int k = 0; k < n; k++, id -= 2, y -= 19) {
        BYTE* b = make((int)(const char*)s_q36_677a38, id, 0);
        CallThis(FUN_5bdbf0, p, b, 0);
        Shell_MoveCtl(b, x, y);
    }
}
// [FUN_520e30] constructor (parent): a shell screen (vtables 64e7c0, 64e780 at +64): three columns of 50d1c0
// buttons 255.., 241.., 227.. (+368), two columns of 50d530 buttons 271.., 263.. (+372), buttons 2, 279
extern "C" BYTE* __fastcall ShellColumns520e30_Construct(BYTE* s, void*, BYTE* parent)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q36_64e7c0;
    F(void*, s, 64) = g_q36_64e780;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    BYTE* p = Shell_NewPanel(4);
    Shell_PanelStyles(p + 68, (const char*)s_q36_677a58, (const char*)s_q36_677a40);
    BYTE* c = Shell_NewPanel(0);
    F(BYTE*, s, 368) = c;
    Shell_ButtonColumn(c, Shell_ModeButtonA, 7, 255, 362, 95);
    Shell_ButtonColumn(c, Shell_ModeButtonA, 7, 241, 181, 95);
    Shell_ButtonColumn(c, Shell_ModeButtonA, 6, 227, 0, 95);
    Shell_MoveCtl(c, 231, 613);
    CallThis(FUN_5bdbf0, p, c, 0);
    c = Shell_NewPanel(0);
    F(BYTE*, s, 372) = c;
    Shell_ButtonColumn(c, Shell_ModeButtonB, 4, 271, 181, 57);
    Shell_ButtonColumn(c, Shell_ModeButtonB, 4, 263, 0, 57);
    Shell_MoveCtl(c, 412, 481);
    CallThis(FUN_5bdbf0, p, c, 0);
    c = Shell_NewPanel(0);
    BYTE* b = Shell_ModeButtonC((int)(const char*)s_q36_677a2c, 2);
    CallThis(FUN_5bdbf0, c, b, 0);
    Shell_MoveCtl(b, 349, 800);
    b = Shell_ModeButtonC((int)(const char*)s_q36_677a24, 279);
    CallThis(FUN_5bdbf0, c, b, 0);
    Shell_MoveCtl(b, 209, 800);
    CallThis(FUN_5bdbf0, p, c, 0);
    CallThis(FUN_5bdc80, p);
    Shell_MoveCtl(p, F(WORD, F(BYTE*, s, 12), 0) - 640, g_q29_6ced18);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p, 0);
    F(int, s, 376) = 0;
    return s;
}

extern "C" void FUN_50efb0(); KING_ALIAS("_FUN_50efb0", "_Shell_MakeMaskSlider")
extern "C" void FUN_50e830(); KING_ALIAS("_FUN_50e830", "_Shell_MakeTexButton")
extern "C" void FUN_5159d0(); KING_ALIAS("_FUN_5159d0", "_Video_PickModes")
extern "C" void FUN_50e500(); KING_ALIAS("_FUN_50e500", "_Shell_MakeMaskButton")
extern "C" void Wnd_SetPos_(); KING_ALIAS("_Wnd_SetPos_", "?SetWindowPos@CWnd@@QAEHPBV1@HHHHI@Z")
extern "C" void __cdecl _exit(int);
ASM_VAR(BYTE*, g_q37_6968d0, $L_6968d0)
ASM_VAR(AsmLabel, g_q37_72da58, $L_72da58)
ASM_VAR(int, g_q37_7208e4, $L_7208e4)
ASM_VAR(AsmLabel, g_q37_6cecfc, $L_6cecfc)
ASM_VAR(AsmLabel, g_q37_6ced00, $L_6ced00)
ASM_VAR(AsmLabel, g_q37_6ced04, $L_6ced04)
ASM_VAR(AsmLabel, g_q37_6ced08, $L_6ced08)
ASM_VAR(AsmLabel, g_q37_64def8, $L_64def8)
ASM_VAR(AsmLabel, g_q37_64deb8, $L_64deb8)
ASM_VAR(AsmLabel, s_q37_6730a0, $L_6730a0)
#define Q37(x) ((const char*)s_q37_##x)
ASM_VAR(AsmLabel, s_q37_676dd0, $L_676dd0)
ASM_VAR(AsmLabel, s_q37_676dc8, $L_676dc8)
ASM_VAR(AsmLabel, s_q37_676d98, $L_676d98)
ASM_VAR(AsmLabel, s_q37_676d90, $L_676d90)
ASM_VAR(AsmLabel, s_q37_676cec, $L_676cec)
ASM_VAR(AsmLabel, s_q37_676c9c, $L_676c9c)
ASM_VAR(AsmLabel, s_q37_676d80, $L_676d80)
ASM_VAR(AsmLabel, s_q37_676d78, $L_676d78)
ASM_VAR(AsmLabel, s_q37_676d64, $L_676d64)
ASM_VAR(AsmLabel, s_q37_676d10, $L_676d10)
ASM_VAR(AsmLabel, s_q37_676d04, $L_676d04)
ASM_VAR(AsmLabel, s_q37_676c58, $L_676c58)
ASM_VAR(AsmLabel, s_q37_676c50, $L_676c50)
ASM_VAR(AsmLabel, s_q37_676d5c, $L_676d5c)
ASM_VAR(AsmLabel, s_q37_676d54, $L_676d54)
ASM_VAR(AsmLabel, s_q37_676d4c, $L_676d4c)
ASM_VAR(AsmLabel, s_q37_676d44, $L_676d44)
ASM_VAR(AsmLabel, s_q37_676d38, $L_676d38)

// [FUN_515ca0] constructor (parent): the display setup screen (vtables 64def8, 64deb8 at +64): reads the ini,
// picks the video mode through 5159d0 (exits with a message box when none fits), centres the 640x480 window,
// two sliders 139/140 (+422, +420), four depth switches 138..135 (+376) by +408, buttons 134 (+384), 133 (+380)
extern "C" BYTE* __fastcall ShellDisplay515ca0_Construct(BYTE* s, void*, int parent)
{
    CallThis(FUN_5d8900, s, parent, g_q28_720938, g_q28_72093c, g_q28_720940, g_q28_720944, 4, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q37_64def8;
    F(void*, s, 64) = g_q37_64deb8;
    int v;
    CallC(FUN_603b70, Q32(672998), Q37(676dd0), 3, &v, g_q32_696868);
    F(int, s, 388) = v;
    CallC(FUN_603b70, Q37(6730a0), Q37(676dc8), 1, &v, g_q32_696868);
    F(int, s, 412) = v - 1;
    g_q37_7208e4 = v - 1;
    if (!CallC<int>(FUN_5159d0, F(int, s, 412), s + 416, (void*)g_q37_6cecfc, s + 392, (void*)g_q37_6ced00,
                    s + 396, (void*)g_q37_6ced04, s + 400, (void*)g_q37_6ced08, s + 404)) {
        CallC(FUN_5d2ba0, Q37(676d98), 0);
        MessageBoxA(0, Q37(676d98), Q37(676d90), 0);
        _exit(0);
    }
    CallC(FUN_603b70, Q32(672998), Q37(676cec), 290, &v, g_q32_696868);
    F(int, s, 408) = v;
    if (F(int, s, 396) == 0 && (v & 240) > 16)
        F(int, s, 408) = (v & ~0xff) | (v & 31) | 16;
    int w = GetSystemMetrics(0);
    int h = GetSystemMetrics(1);
    CallThis(Wnd_SetPos_, g_q37_6968d0, (void*)g_q37_72da58, (w - 640) / 2, (h - 480) / 2, 640, 480, 64);
    DWORD* o = (DWORD*)operator new(12);
    if (o) {
        o[0] = CallC<DWORD>(FUN_5ef3d0, Q37(676c9c));
        o[1] = 0;
        o[2] = 0;
    }
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    Shell_PlainStyles(s + 132, Q37(676d80));
    BYTE* outer = Shell_NewPanel(0);
    BYTE* p = Shell_NewPanel(4);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50ef60, Q37(676d78), 2, 348, 452), 0);
    g_q28_6ce3ac = CallC<DWORD>(FUN_5ef3d0, Q37(676d64));
    CallThis(FUN_5bd5a0, p + 68, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, p + 104, (void*)g_q28_6ce390);
    CallThis(FUN_5bd5a0, p + 140, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, p + 176, (void*)g_6ce408);
    CallC(FUN_603b70, Q32(672998), Q37(676d10), 100, &v, g_q32_696868);
    F(WORD, s, 422) = (WORD)v;
    BYTE* b = CallC<BYTE*>(FUN_50efb0, Q37(676c50), Q37(676c58), s + 422, 349, 405, 0, 182);
    CallThis(FUN_5bdbf0, p, b, 0);
    F(int, b, 28) = 139;
    CallC(FUN_603b70, Q32(672998), Q37(676d04), 100, &v, g_q32_696868);
    F(WORD, s, 420) = (WORD)v;
    b = CallC<BYTE*>(FUN_50efb0, Q37(676c50), Q37(676c58), s + 420, 349, 370, 0, 182);
    CallThis(FUN_5bdbf0, p, b, 0);
    F(int, b, 28) = 140;
    BYTE* d = Shell_NewPanel(0);
    F(BYTE*, s, 376) = d;
    int depth = F(int, s, 408) & 240;
    CallThis(FUN_5bdbf0, d, CallC<BYTE*>(FUN_50e830, Q37(676d5c), 138, 441, 330, depth == 64, 0), 0);
    CallThis(FUN_5bdbf0, d, CallC<BYTE*>(FUN_50e830, Q37(676d54), 137, 329, 330, depth == 48, 0), 0);
    CallThis(FUN_5bdbf0, d, CallC<BYTE*>(FUN_50e830, Q37(676d4c), 136, 441, 293, depth == 32, 0), 0);
    CallThis(FUN_5bdbf0, d, CallC<BYTE*>(FUN_50e830, Q37(676d44), 135, 329, 293, depth == 16, 0), 0);
    CallThis(FUN_5bdbf0, p, d, 0);
    F(BYTE*, s, 384) = CallC<BYTE*>(FUN_50e500, Q37(676c58), 134, 536, 263);
    CallThis(FUN_5bdbf0, p, F(BYTE*, s, 384), 0);
    F(BYTE*, s, 380) = CallC<BYTE*>(FUN_50e500, Q37(676c50), 133, 324, 263);
    CallThis(FUN_5bdbf0, p, F(BYTE*, s, 380), 0);
    F(int, F(BYTE*, s, 380), 40) = 1;
    Shell_MoveCtl(p, 310, 218);
    CallThis(FUN_5bdd70, p);
    CallThis(FUN_5bdbf0, outer, p, 0);
    CallThis(FUN_5bdbf0, outer, CallC<BYTE*>(FUN_50ef60, Q37(676d38), 132, 348, 157), 0);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), outer, 0);
    return s;
}

// ---------------------------------------------------------------------------
// Network provider screen (exception frame dropped).

extern "C" void Gsh_VecCtor(); KING_ALIAS("_Gsh_VecCtor", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" void FUN_5c39e0(); KING_ALIAS("_FUN_5c39e0", "@Music_Play@20")
extern "C" void FUN_6014a0(); KING_ALIAS("_FUN_6014a0", "@DpRecord_ProviderName@8")
extern "C" void FUN_601570(); KING_ALIAS("_FUN_601570", "_Dp_NextProvider")
extern "C" void FUN_6015b0(); KING_ALIAS("_FUN_6015b0", "_Dp_FirstProvider")
extern "C" void FUN_6016f0(); KING_ALIAS("_FUN_6016f0", "_Providers_Rebuild")
extern "C" void FUN_50f300(); KING_ALIAS("_FUN_50f300", "_Shell_SkinScrollBar")
ASM_VAR(AsmLabel, g_q38_601260, $L_601260)
ASM_VAR(AsmLabel, g_q38_64e2e4, $L_64e2e4)
ASM_VAR(AsmLabel, g_q38_64e2a4, $L_64e2a4)
ASM_VAR(AsmLabel, g_q38_675ab4, $L_675ab4)
#define Q38(x) ((const char*)s_q38_##x)
ASM_VAR(AsmLabel, s_q38_677460, $L_677460)
ASM_VAR(AsmLabel, s_q38_67744c, $L_67744c)
ASM_VAR(AsmLabel, s_q38_677424, $L_677424)
ASM_VAR(AsmLabel, s_q38_6773d8, $L_6773d8)
ASM_VAR(AsmLabel, s_q38_67741c, $L_67741c)
ASM_VAR(AsmLabel, s_q38_677414, $L_677414)
ASM_VAR(AsmLabel, s_q38_67740c, $L_67740c)
ASM_VAR(AsmLabel, s_q38_677400, $L_677400)
ASM_VAR(AsmLabel, s_q38_6773f4, $L_6773f4)

// [FUN_51a580] constructor (parent): the network provider screen (vtables 64e2e4, 64e2a4 at +64): eight 32-byte
// provider records (+552), their names in the list box 123 (+548), buttons 121, 120, 122, 118, 119, 2 and the
// 4 x 11 textures by the 675ab4 names (+372, rows of 16 bytes)
extern "C" BYTE* __fastcall ShellProviders51a580_Construct(BYTE* s, void*, int parent)
{
    CallThis(FUN_5d8900, s, parent, g_q28_720938, g_q28_72093c, g_q28_720940, g_q28_720944, 4, g_q28_6ced24, 0);
    CallStd(Gsh_VecCtor, s + 552, 32, 8, (BYTE*)g_q38_601260, FUN_601470);
    F(void*, s, 0) = g_q38_64e2e4;
    F(void*, s, 64) = g_q38_64e2a4;
    DWORD* o = (DWORD*)operator new(12);
    if (o) {
        o[0] = CallC<DWORD>(FUN_5ef3d0, Q37(676c9c));
        o[1] = 0;
        o[2] = 0;
    }
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    Shell_PlainStyles(s + 132, Q38(677460));
    BYTE* outer = Shell_NewPanel(0);
    BYTE* p = Shell_NewPanel(4);
    g_q28_6ce3ac = CallC<DWORD>(FUN_5ef3d0, Q38(67744c));
    CallThis(FUN_5bd5a0, p + 68, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, p + 104, (void*)g_q28_6ce390);
    CallThis(FUN_5bd5a0, p + 140, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, p + 176, (void*)g_6ce408);
    BYTE* m = (BYTE*)operator new(352);
    BYTE* l = m ? CallThis<BYTE*>(FUN_5c0550, m, Q38(677424), 8, 70) : 0;
    F(BYTE*, s, 548) = l;
    CallThis(FUN_5bd5a0, l + 68, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, l + 104, (void*)g_6ce3b8);
    CallThis(FUN_5bd5a0, l + 140, (void*)g_6ceb38);
    CallThis(FUN_5bd5a0, l + 176, (void*)g_6ce988);
    CallThis(FUN_5c39e0, &g_music696750, Q38(6773d8), 0, 0);
    if (g_music6968f0) {
        CallC(FUN_6016f0);
        if (!CallC<int>(FUN_6015b0, s + 552)) {
            CallThis(FUN_5c1140, F(BYTE*, s, 548), CallThis<const char*>(FUN_6014a0, s + 552));
            for (BYTE* e = s + 584; !CallC<int>(FUN_601570, e); e += 32)
                CallThis(FUN_5c1140, F(BYTE*, s, 548), CallThis<const char*>(FUN_6014a0, e));
        }
        CallThis(FUN_5c3a80, &g_music696750);
    }
    l = F(BYTE*, s, 548);
    F(int, F(BYTE*, l, 340), 28) = 123;
    F(int, l, 28) = 123;
    CallThis(FUN_5c0c20, l, 1);
    Shell_MoveCtl(F(BYTE*, s, 548), 323, 350);
    F(int, F(BYTE*, s, 548), 348) = 41;
    CallThis(FUN_5c07c0, F(BYTE*, s, 548), 10);
    CallC(FUN_50f300, F(BYTE*, F(BYTE*, s, 548), 340), 64);
    CallThis(FUN_5bdbf0, p, F(BYTE*, s, 548), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50e500, Q37(676c50), 121, 323, 200), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50e500, Q37(676c58), 120, 528, 200), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50ef60, Q38(67741c), 122, 390, 303), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50ef60, Q38(677414), 118, 451, 428), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50ef60, Q38(67740c), 119, 348, 428), 0);
    Shell_MoveCtl(p, 310, 102);
    CallThis(FUN_5bdd70, p);
    CallThis(FUN_5bdbf0, outer, p, 0);
    CallThis(FUN_5bdbf0, outer, CallC<BYTE*>(FUN_50ef60, Q38(677400), 2, 323, 488), 0);
    CallThis(FUN_5bdc80, outer);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), outer, 0);
    F(int, s, 368) = 0;
    const char** names = (const char**)(void*)g_q38_675ab4;
    for (int i = 0; i < 4; i++)
        for (int k = 0; k < 11; k++) {
            char buf[64];
            sprintf(buf, Q38(6773f4), names[k], i);
            F(DWORD, s, 372 + i * 4 + k * 16) = CallC<DWORD>(FUN_5ef3d0, buf);
        }
    F(int, s, 808) = 0;
    F(int, s, 812) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Network session screen (exception frame dropped).

ASM_VAR(AsmLabel, g_q39_64e3f4, $L_64e3f4)
ASM_VAR(AsmLabel, g_q39_64e3b4, $L_64e3b4)
#define Q39(x) ((const char*)s_q39_##x)
ASM_VAR(AsmLabel, s_q39_677564, $L_677564)
ASM_VAR(AsmLabel, s_q39_67755c, $L_67755c)
ASM_VAR(AsmLabel, s_q39_677548, $L_677548)
ASM_VAR(AsmLabel, s_q39_67753c, $L_67753c)
ASM_VAR(AsmLabel, s_q39_67752c, $L_67752c)
ASM_VAR(AsmLabel, s_q39_677518, $L_677518)
ASM_VAR(AsmLabel, s_q39_677510, $L_677510)
ASM_VAR(AsmLabel, s_q39_677508, $L_677508)
ASM_VAR(AsmLabel, s_q39_677500, $L_677500)
ASM_VAR(AsmLabel, s_q39_6774f0, $L_6774f0)
ASM_VAR(AsmLabel, s_q39_6774e0, $L_6774e0)

// the 4 x 36-byte frame styles from 6ce408, 6ce390, 6ce408, 6ce408
static void Shell_FrameStyles(BYTE* p)
{
    CallThis(FUN_5bd5a0, p + 68, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, p + 104, (void*)g_q28_6ce390);
    CallThis(FUN_5bd5a0, p + 140, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, p + 176, (void*)g_6ce408);
}
// the list box / edit field styles
static void Shell_FieldStyles(BYTE* l)
{
    CallThis(FUN_5bd5a0, l + 68, (void*)g_6ce408);
    CallThis(FUN_5bd5a0, l + 104, (void*)g_6ce3b8);
    CallThis(FUN_5bd5a0, l + 140, (void*)g_6ceb38);
    CallThis(FUN_5bd5a0, l + 176, (void*)g_6ce988);
}
// [FUN_51b640] constructor (parent): the network session screen (vtables 64e3f4, 64e3b4 at +64): a session list
// 125 (+604), a 34-character name field 126 (+608) on +668, buttons 2, 127, 129..131, the player and session
// names (+632, +668) and two numbers (+616, +624) from the ini, textures 1..8 (+368) and 1..51 (+400)
extern "C" BYTE* __fastcall ShellSession51b640_Construct(BYTE* s, void*, int parent)
{
    CallThis(FUN_5d8900, s, parent, g_q28_720938, g_q28_72093c, g_q28_720940, g_q28_720944, 4, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q39_64e3f4;
    F(void*, s, 64) = g_q39_64e3b4;
    DWORD* o = (DWORD*)operator new(12);
    if (o) {
        o[0] = CallC<DWORD>(FUN_5ef3d0, Q37(676c9c));
        o[1] = 0;
        o[2] = 0;
    }
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    Shell_PlainStyles(s + 132, Q39(677564));
    BYTE* outer = Shell_NewPanel(0);
    CallThis(FUN_5bdbf0, outer, CallC<BYTE*>(FUN_50ef60, Q39(67755c), 2, 323, 481), 0);
    BYTE* p = Shell_NewPanel(8);
    g_q30_6ce3b0 = CallC<BYTE*>(FUN_5ef3d0, Q39(677548));
    Shell_FrameStyles(p);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50ef60, Q39(67753c), 127, 434, 418), 0);
    BYTE* m = (BYTE*)operator new(352);
    BYTE* l = m ? CallThis<BYTE*>(FUN_5c0550, m, Q39(67752c), 8, 120) : 0;
    F(BYTE*, s, 604) = l;
    Shell_FieldStyles(l);
    Shell_MoveCtl(F(BYTE*, s, 604), 308, 322);
    F(int, F(BYTE*, s, 604), 348) = 41;
    F(int, F(BYTE*, F(BYTE*, s, 604), 340), 28) = 125;
    F(int, F(BYTE*, s, 604), 28) = 125;
    CallThis(FUN_5c07c0, F(BYTE*, s, 604), 10);
    CallC(FUN_50f300, F(BYTE*, F(BYTE*, s, 604), 340), 128);
    CallThis(FUN_5bdbf0, p, F(BYTE*, s, 604), 0);
    Shell_MoveCtl(p, 295, 310);
    CallThis(FUN_5bdd70, p);
    CallThis(FUN_5bdbf0, outer, p, 0);
    p = Shell_NewPanel(8);
    g_q30_6ce3b0 = CallC<BYTE*>(FUN_5ef3d0, Q39(677518));
    Shell_FrameStyles(p);
    BYTE* e = (BYTE*)operator new(300);
    if (e) {
        CallThis(FUN_5bd5e0, e, (const char*)g_q33_68d1c0, 0);
        F(void*, e, 0) = g_q33_64e160;
        F(BYTE*, e, 288) = s + 668;
        F(int, e, 292) = 34;
        F(int, e, 296) = 0;
    }
    F(BYTE*, s, 608) = e;
    F(int, e, 28) = 126;
    e = F(BYTE*, s, 608);
    Shell_FieldStyles(e);
    F(int, F(BYTE*, s, 608), 44) = 313;
    F(int, F(BYTE*, s, 608), 48) = 274;
    e = F(BYTE*, s, 608);
    F(int, e, 52) = F(int, e, 44) + 90;
    e = F(BYTE*, s, 608);
    F(int, e, 56) = F(int, e, 48) + 10;
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50e500, Q37(676c50), 129, 308, 175), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50e500, Q37(676c58), 128, 463, 175), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50e500, Q37(676c50), 131, 416, 265), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50e500, Q37(676c58), 130, 571, 265), 0);
    CallThis(FUN_5bdbf0, p, F(BYTE*, s, 608), 0);
    Shell_MoveCtl(p, 295, 100);
    CallThis(FUN_5bdd70, p);
    F(int, p, 56) -= 30;
    CallThis(FUN_5bdbf0, outer, p, 0);
    CallThis(FUN_5bdc80, outer);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), outer, 0);
    F(int, s, 612) = 0;
    GetPrivateProfileStringA(Q32(672998), Q39(677510), CallC<const char*>(FUN_5d2e50, 48), (char*)s + 632, 33,
                             (const char*)g_q32_696868);
    GetPrivateProfileStringA(Q32(672998), Q39(677510), CallC<const char*>(FUN_5d2e50, 48), (char*)s + 668, 33,
                             (const char*)g_q32_696868);
    int v;
    CallC(FUN_603b70, Q32(672998), Q39(677508), 5, &v, g_q32_696868);
    F(int, s, 616) = v;
    CallC(FUN_603b70, Q32(672998), Q39(677500), 0, &v, g_q32_696868);
    F(int, s, 624) = v;
    char buf[32];
    for (int i = 0; i < 8; i++) {
        sprintf(buf, Q39(6774f0), i + 1);
        F(DWORD, s, 368 + i * 4) = CallC<DWORD>(FUN_5ef3d0, buf);
    }
    for (int i = 0; i < 51; i++) {
        sprintf(buf, Q39(6774e0), i + 1);
        F(DWORD, s, 400 + i * 4) = CallC<DWORD>(FUN_5ef3d0, buf);
    }
    return s;
}

// ---------------------------------------------------------------------------
// Shell screen with a list and five texture tables (exception frame dropped).

ASM_VAR(AsmLabel, g_q40_64ea70, $L_64ea70)
ASM_VAR(AsmLabel, g_q40_64ea30, $L_64ea30)
ASM_VAR(AsmLabel, g_q40_675dd8, $L_675dd8)
ASM_VAR(AsmLabel, g_q40_675c90, $L_675c90)
ASM_VAR(AsmLabel, g_q40_675ce4, $L_675ce4)
#define Q40(x) ((const char*)s_q40_##x)
ASM_VAR(AsmLabel, s_q40_678238, $L_678238)
ASM_VAR(AsmLabel, s_q40_678228, $L_678228)
ASM_VAR(AsmLabel, s_q40_678220, $L_678220)
ASM_VAR(AsmLabel, s_q40_678214, $L_678214)
ASM_VAR(AsmLabel, s_q40_6781fc, $L_6781fc)
ASM_VAR(AsmLabel, s_q40_6781e4, $L_6781e4)
ASM_VAR(AsmLabel, s_q40_6781d0, $L_6781d0)
ASM_VAR(AsmLabel, s_q40_6781bc, $L_6781bc)
ASM_VAR(AsmLabel, s_q40_6781a8, $L_6781a8)
ASM_VAR(AsmLabel, s_q40_678194, $L_678194)
ASM_VAR(AsmLabel, s_q40_678180, $L_678180)
ASM_VAR(AsmLabel, s_q40_67816c, $L_67816c)
ASM_VAR(AsmLabel, s_q40_678154, $L_678154)
ASM_VAR(AsmLabel, s_q40_67813c, $L_67813c)
ASM_VAR(AsmLabel, s_q40_678124, $L_678124)
ASM_VAR(AsmLabel, s_q40_67810c, $L_67810c)
ASM_VAR(AsmLabel, s_q40_6780fc, $L_6780fc)
ASM_VAR(AsmLabel, s_q40_6780ec, $L_6780ec)
ASM_VAR(AsmLabel, s_q40_6780d8, $L_6780d8)
ASM_VAR(AsmLabel, s_q40_6780c8, $L_6780c8)
ASM_VAR(AsmLabel, s_q40_6780b8, $L_6780b8)

static const char* Shell_ByMode(const char* m2, const char* other)
{
    return CallC<int>(FUN_568fd0) == 2 ? m2 : other;
}
// fills n texture slots from the format by mode with 1..n
static void Shell_NumberedTex(BYTE* at, const char* m2, const char* other, int n)
{
    char buf[64];
    for (int i = 1; i <= n; i++) {
        sprintf(buf, Shell_ByMode(m2, other), i);
        F(DWORD, at, (i - 1) * 4) = CallC<DWORD>(FUN_5ef3d0, buf);
    }
}
static DWORD Shell_PrefixedTex(const char* fmt)
{
    char buf[64];
    sprintf(buf, fmt, Shell_ByMode((const char*)s_q21_6762c4, (const char*)s_q21_6762bc));
    return CallC<DWORD>(FUN_5ef3d0, buf);
}
// [FUN_526470] constructor (parent): a shell screen (vtables 64ea70, 64ea30 at +64): buttons 185 (+368), 186, a
// list box 187 (+1032), and the textures: 1..51 (+372), three single (+668..+676), 1..8 (+576), 1..15 (+608),
// four prefixed (+1016..+1028) and two per name of 675c90 (+680) and 675ce4 (+848)
extern "C" BYTE* __fastcall ShellScreen526470_Construct(BYTE* s, void*, BYTE* parent)
{
    int r[4];
    int* q = Carat_WindowRectBelow(r, parent);
    CallThis(FUN_5d8900, s, parent, q[0], q[1], q[2], q[3], 0, g_q28_6ced24, g_q31_6ced2c);
    F(void*, s, 0) = g_q40_64ea70;
    F(void*, s, 64) = g_q40_64ea30;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    BYTE* p = Shell_NewPanel(4);
    Shell_PanelStyles(p + 68, Q40(678238), Q40(678228));
    BYTE* c = Shell_NewPanel(0);
    BYTE* b = Shell_ModeButtonC((int)Q40(678220), 185);
    F(BYTE*, s, 368) = b;
    CallThis(FUN_5bdbf0, c, b, 0);
    Shell_MoveCtl(b, 186, 774);
    b = Shell_ModeButtonC((int)(const char*)g_q40_675dd8, 186);
    CallThis(FUN_5bdbf0, c, b, 0);
    Shell_MoveCtl(b, 85, 774);
    CallThis(FUN_5bdbf0, p, c, 0);
    c = Shell_NewPanel(0);
    BYTE* m = (BYTE*)operator new(352);
    BYTE* l = m ? CallThis<BYTE*>(FUN_5c0550, m, Q40(678214), 4, 201) : 0;
    F(BYTE*, s, 1032) = l;
    Shell_FieldStyles(l);
    CallThis(FUN_5c07c0, F(BYTE*, s, 1032), 10);
    Button_SetImages(F(BYTE*, F(BYTE*, s, 1032), 340), (const char*)s_q31_67777c, (const char*)s_q31_677780);
    Shell_MoveCtl(F(BYTE*, F(BYTE*, s, 1032), 340), 319, -12);
    CallThis(FUN_5becd0, F(BYTE*, F(BYTE*, s, 1032), 340), 219);
    Shell_MoveCtl(F(BYTE*, s, 1032), 20, 537);
    F(int, F(BYTE*, s, 1032), 344) = 8;
    F(int, F(BYTE*, F(BYTE*, s, 1032), 340), 28) = 187;
    F(int, F(BYTE*, s, 1032), 28) = 187;
    CallThis(FUN_5bdbf0, c, F(BYTE*, s, 1032), 0);
    CallThis(FUN_5bdbf0, p, c, 0);
    CallThis(FUN_5bdc80, p);
    Shell_MoveCtl(p, g_q30_6ced0c, g_q30_6ced10);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p, 0);
    F(int, s, 1036) = 0;
    Shell_NumberedTex(s + 372, Q40(6781fc), Q40(6781e4), 51);
    F(DWORD, s, 668) = CallC<DWORD>(FUN_5ef3d0, Shell_ByMode(Q40(6781d0), Q40(6781bc)));
    F(DWORD, s, 672) = CallC<DWORD>(FUN_5ef3d0, Shell_ByMode(Q40(6781a8), Q40(678194)));
    F(DWORD, s, 676) = CallC<DWORD>(FUN_5ef3d0, Shell_ByMode(Q40(678180), Q40(67816c)));
    Shell_NumberedTex(s + 576, Q40(678154), Q40(67813c), 8);
    Shell_NumberedTex(s + 608, Q40(678124), Q40(67810c), 15);
    F(DWORD, s, 1016) = Shell_PrefixedTex(Q40(6780fc));
    F(DWORD, s, 1020) = Shell_PrefixedTex(Q40(6780ec));
    F(DWORD, s, 1024) = Shell_PrefixedTex(Q40(6780d8));
    F(DWORD, s, 1028) = Shell_PrefixedTex(Q40(6780c8));
    const char** na = (const char**)(void*)g_q40_675c90;
    const char** nb = (const char**)(void*)g_q40_675ce4;
    char buf[64];
    for (int k = 0; k < 21; k++)
        for (int i = 0; i < 2; i++) {
            sprintf(buf, Q40(6780b8), Shell_ByMode((const char*)s_q21_6762c4, (const char*)s_q21_6762bc), na[k], i);
            F(DWORD, s, 680 + (k * 2 + i) * 4) = CallC<DWORD>(FUN_5ef3d0, buf);
            sprintf(buf, Q40(6780b8), Shell_ByMode((const char*)s_q21_6762c4, (const char*)s_q21_6762bc), nb[k], i);
            F(DWORD, s, 848 + (k * 2 + i) * 4) = CallC<DWORD>(FUN_5ef3d0, buf);
        }
    return s;
}

// ---------------------------------------------------------------------------
// Network join screen (exception frame dropped).

ASM_VAR(AsmLabel, g_q41_64e1d4, $L_64e1d4)
ASM_VAR(AsmLabel, g_q41_64e194, $L_64e194)
#define Q41(x) ((const char*)s_q41_##x)
ASM_VAR(AsmLabel, s_q41_67732c, $L_67732c)
ASM_VAR(AsmLabel, s_q41_677314, $L_677314)
ASM_VAR(AsmLabel, s_q41_677304, $L_677304)
ASM_VAR(AsmLabel, s_q41_6772f8, $L_6772f8)
ASM_VAR(AsmLabel, s_q41_6772f0, $L_6772f0)
ASM_VAR(AsmLabel, s_q41_6772dc, $L_6772dc)
ASM_VAR(AsmLabel, s_q41_6772d4, $L_6772d4)
ASM_VAR(AsmLabel, s_q41_6772cc, $L_6772cc)

// [FUN_518d20] constructor (parent): the network join screen (vtables 64e1d4, 64e194 at +64): a list box 149
// (+380), buttons 281, 145, 2, a 34-character field 148 (+376) on +384 (text 81 at the start), the button 146 (+372)
extern "C" BYTE* __fastcall ShellJoin518d20_Construct(BYTE* s, void*, int parent)
{
    CallThis(FUN_5d8900, s, parent, g_q28_720938, g_q28_72093c, g_q28_720940, g_q28_720944, 4, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q41_64e1d4;
    F(void*, s, 64) = g_q41_64e194;
    DWORD* o = (DWORD*)operator new(12);
    if (o) {
        o[0] = CallC<DWORD>(FUN_5ef3d0, Q37(676c9c));
        o[1] = 0;
        o[2] = 0;
    }
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    Shell_PlainStyles(s + 132, Q41(67732c));
    BYTE* outer = Shell_NewPanel(0);
    BYTE* p = Shell_NewPanel(4);
    g_q28_6ce3ac = CallC<DWORD>(FUN_5ef3d0, Q41(677314));
    Shell_FrameStyles(p);
    BYTE* m = (BYTE*)operator new(352);
    BYTE* l = m ? CallThis<BYTE*>(FUN_5c0550, m, Q41(677304), 8, 110) : 0;
    F(BYTE*, s, 380) = l;
    Shell_FieldStyles(l);
    Shell_MoveCtl(F(BYTE*, s, 380), 332, 199);
    F(int, F(BYTE*, s, 380), 348) = 41;
    F(int, F(BYTE*, F(BYTE*, s, 380), 340), 28) = 149;
    F(int, F(BYTE*, s, 380), 28) = 149;
    CallThis(FUN_5c07c0, F(BYTE*, s, 380), 10);
    CallC(FUN_50f300, F(BYTE*, F(BYTE*, s, 380), 340), 118);
    CallThis(FUN_5bdbf0, p, F(BYTE*, s, 380), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50ef60, Q41(6772f8), 281, 475, 285), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50ef60, Q41(6772f0), 145, 332, 145), 0);
    Shell_MoveCtl(p, 300, 141);
    CallThis(FUN_5bdd70, p);
    CallThis(FUN_5bdbf0, outer, p, 0);
    p = Shell_NewPanel(4);
    g_q28_6ce3ac = CallC<DWORD>(FUN_5ef3d0, Q41(6772dc));
    Shell_FrameStyles(p);
    F(char, s, 384) = 0;
    BYTE* e = (BYTE*)operator new(300);
    if (e) {
        CallThis(FUN_5bd5e0, e, (const char*)g_q33_68d1c0, 0);
        F(void*, e, 0) = g_q33_64e160;
        F(BYTE*, e, 288) = s + 384;
        F(int, e, 292) = 34;
        F(int, e, 296) = 0;
    }
    F(BYTE*, s, 376) = e;
    F(int, e, 28) = 148;
    Shell_FieldStyles(F(BYTE*, s, 376));
    F(int, F(BYTE*, s, 376), 44) = 341;
    F(int, F(BYTE*, s, 376), 48) = 407;
    e = F(BYTE*, s, 376);
    F(int, e, 52) = F(int, e, 44) + 125;
    e = F(BYTE*, s, 376);
    F(int, e, 56) = F(int, e, 48) + 10;
    CallThis(FUN_5bdbf0, p, F(BYTE*, s, 376), 0);
    F(BYTE*, s, 372) = Shell_ButtonC((int)Q41(6772d4), 146, 332, 343, 0);
    CallThis(FUN_5bdbf0, p, F(BYTE*, s, 372), 0);
    Shell_MoveCtl(p, 300, 341);
    CallThis(FUN_5bdd70, p);
    CallThis(FUN_5bdbf0, outer, p, 0);
    CallThis(FUN_5bdbf0, outer, CallC<BYTE*>(FUN_50ef60, Q41(6772cc), 2, 332, 465), 0);
    CallThis(FUN_5bdc80, outer);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), outer, 0);
    strcpy((char*)s + 384, CallC<const char*>(FUN_5d2e50, 81));
    return s;
}

// ---------------------------------------------------------------------------
// Controls screen (exception frame dropped).

ASM_PROC(FUN_5bdf80)
ASM_VAR(int, g_q42_6d207c, $L_6d207c)
ASM_VAR(AsmLabel, g_q42_64de50, $L_64de50)
ASM_VAR(AsmLabel, g_q42_64de10, $L_64de10)
ASM_VAR(AsmLabel, g_q42_675494, $L_675494)
ASM_VAR(AsmLabel, s_q42_676980, $L_676980)
ASM_VAR(AsmLabel, s_q42_67621c, $L_67621c)
ASM_VAR(AsmLabel, s_q42_6712a0, $L_6712a0)
ASM_VAR(AsmLabel, s_q42_6712a4, $L_6712a4)
#define Q42(x) ((const char*)s_q42_##x)
ASM_VAR(AsmLabel, s_q42_676c8c, $L_676c8c)
ASM_VAR(AsmLabel, s_q42_676c84, $L_676c84)
ASM_VAR(AsmLabel, s_q42_6767d0, $L_6767d0)
ASM_VAR(AsmLabel, s_q42_676c7c, $L_676c7c)
ASM_VAR(AsmLabel, s_q42_676c74, $L_676c74)
ASM_VAR(AsmLabel, s_q42_676c60, $L_676c60)
ASM_VAR(AsmLabel, s_q42_676978, $L_676978)
ASM_VAR(AsmLabel, s_q42_67696c, $L_67696c)
ASM_VAR(AsmLabel, s_q42_676964, $L_676964)
ASM_VAR(AsmLabel, s_q42_673384, $L_673384)

// the control name table slots (675494..6759a0) by their original address
static const char*& Keys_NameSlot(DWORD addr)
{
    return *(const char**)((BYTE*)g_q42_675494 + (addr - 0x675494));
}
// a name string in the 676980..676c4c block by its original address
static const char* Keys_Name(DWORD addr)
{
    return (const char*)s_q42_676980 + (addr - 0x676980);
}
// the key names, set for the keyboard layout (696890 == 3: the second column)
static void Keys_SetNames(void)
{
    static const DWORD common4[][2] = {
        {0x675504, 0x676c4c}, {0x67550c, 0x676c48}, {0x675508, 0x676c44}, {0x675510, 0x676c40}};
    static const DWORD layout[][3] = {
        {0x6754f0, 0x676c38, 0x676bf8}, {0x675528, 0x676c30, 0x676bf4}, {0x6754f8, 0x676c28, 0x676bec},
        {0x6754f4, 0x676c20, 0x676be4}, {0x675524, 0x676c18, 0x676be0}, {0x675500, 0x676c10, 0x676bd8},
        {0x6754fc, 0x676c0c, 0x676bd4}, {0x6754a4, 0x676c04, 0x676bcc}};
    for (int k = 0; k < 4; k++)
        Keys_NameSlot(common4[k][0]) = Keys_Name(common4[k][1]);
    bool alt = g_696890 == 3;
    for (int k = 0; k < 8; k++)
        Keys_NameSlot(layout[k][0]) = Keys_Name(layout[k][alt ? 1 : 2]);
    Keys_NameSlot(0x675768) = alt ? Q42(67621c) : Q42(6712a0);
    Keys_NameSlot(0x675760) = alt ? Keys_Name(0x676c00) : Q42(6712a4);
    static const DWORD common[][2] = {
        {0x67561c, 0x676bc4}, {0x675624, 0x676bbc}, {0x675618, 0x676bb4}, {0x67562c, 0x676bac}, {0x675494, 0x676ba8}};
    for (int k = 0; k < 5; k++)
        Keys_NameSlot(common[k][0]) = Keys_Name(common[k][1]);
    // 675870, then the runs 67587c..6758f8 and 675920..67599c, 8 bytes apart in the names from 676ba0 down
    DWORD src = 0x676ba0;
    Keys_NameSlot(0x675870) = Keys_Name(src);
    for (DWORD a = 0x67587c; a <= 0x6758f8; a += 4)
        Keys_NameSlot(a) = Keys_Name(src -= 8);
    for (DWORD a = 0x675920; a <= 0x67599c; a += 4)
        Keys_NameSlot(a) = Keys_Name(src -= 8);
    for (DWORD a = 0x6758fc; a <= 0x675908; a += 4)
        Keys_NameSlot(a) = Keys_Name(src -= 8);
}
// 1 when the ini text (at most n-1 characters) equals 672994
static int Shell_IniIs(const char* sec, const char* key, const char* def, int n)
{
    char buf[64];
    GetPrivateProfileStringA(sec, key, def, buf, n, (const char*)g_q32_696868);
    return strcmp(buf, Q34(672994)) == 0;
}
// [FUN_513b30] constructor (parent): the controls screen (vtables 64de50, 64de10 at +64): the key assignments
// (+1280 the scheme from the ini, below the 6d207c count), 22 action rows (+524) by the texts 102..41, buttons 2,
// 141, 143, 142, the key name table, the ini switches +1320, +1352, +1344 and the values +1304, +1312
extern "C" BYTE* __fastcall ShellControls513b30_Construct(BYTE* s, void*, int parent)
{
    CallThis(FUN_5d8900, s, parent, g_q28_720938, g_q28_72093c, g_q28_720940, g_q28_720944, 4, g_q28_6ced24, 0);
    F(void*, s, 0) = g_q42_64de50;
    F(void*, s, 64) = g_q42_64de10;
    DWORD* o = (DWORD*)operator new(12);
    if (o) {
        o[0] = CallC<DWORD>(FUN_5ef3d0, Q37(676c9c));
        o[1] = 0;
        o[2] = 0;
    }
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    Shell_PlainStyles(s + 132, Q42(676c8c));
    F(int, s, 1280) = 0;
    CallThis(FUN_513850, s);
    F(int, s, 1320) = 0;
    if (Shell_IniIs(Q32(672998), Q42(676c84), (const char*)s_6729a8, 60))
        F(int, s, 1320) = 1;
    int v;
    CallC(FUN_603b70, Q32(672998), Q42(6767d0), 0, &v, g_q32_696868);
    F(int, s, 1280) = v;
    if (v >= g_q42_6d207c)
        F(int, s, 1280) = 0;
    CallC(FUN_531000, F(int, s, 1280));
    BYTE* outer = Shell_NewPanel(0);
    CallThis(FUN_5bdbf0, outer, CallC<BYTE*>(FUN_50ef60, Q42(676c7c), 2, 323, 490), 0);
    BYTE* p = Shell_NewPanel(4);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50ef60, Q42(676c74), 141, 350, 443), 0);
    g_q28_6ce3ac = CallC<DWORD>(FUN_5ef3d0, Q42(676c60));
    Shell_FrameStyles(p);
    BYTE* rows = Shell_NewPanel(0);
    F(BYTE*, s, 524) = rows;
    static const int row[][2] = {
        {102, 1028}, {17, 1034}, {18, 1033}, {21, 1035}, {22, 1025}, {23, 1024}, {24, 1027}, {25, 1026},
        {26, 1020}, {65, 1043}, {29, 1038}, {30, 1021}, {64, 1040}, {62, 1023}, {63, 1041}, {35, 1006},
        {36, 1005}, {37, 1022}, {38, 1003}, {39, 1000}, {40, 1002}, {41, 1001}};
    for (int k = 0; k < 22; k++)
        CallThis(FUN_5bdbf0, rows, Shell_NewScreenA(CallC<int>(FUN_5d2e50, row[k][0]), row[k][1]), 0);
    CallThis(FUN_5bdf80, rows, 2, -180, -20);
    Shell_MoveCtl(rows, 221, 199);
    CallThis(FUN_5bdbf0, p, rows, 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50e500, Q37(676c58), 143, 514, 157), 0);
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50e500, Q37(676c50), 142, 259, 157), 0);
    Shell_MoveCtl(p, 200, 153);
    CallThis(FUN_5bdd70, p);
    CallThis(FUN_5bdbf0, outer, p, 0);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), outer, 0);
    F(int, s, 516) = -1;
    Keys_SetNames();
    F(int, s, 1316) = 0;
    F(int, s, 1328) = 4;
    F(int, s, 1336) = 0;
    F(int, s, 1340) = 0;
    F(int, s, 1300) = 0;
    F(int, s, 1324) = 0;
    F(int, s, 1284) = 0;
    F(int, s, 1288) = 0;
    F(int, s, 1292) = 0;
    F(int, s, 1296) = 0;
    CallC(FUN_603b70, Q32(672998), Q42(676978), 0, &v, g_q32_696868);
    F(int, s, 1304) = v;
    CallC(FUN_603b70, Q32(672998), Q42(67696c), 0, &v, g_q32_696868);
    F(int, s, 1312) = v;
    F(int, s, 1352) = Shell_IniIs(Q32(672998), Q42(676964), (const char*)s_6729a8, 4) ? 0 : 1;
    F(int, s, 1344) = Shell_IniIs(Q37(6730a0), Q42(673384), Q34(672994), 30);
    return s;
}

// ---------------------------------------------------------------------------
// Race setup screen in four variants (exception frame dropped).

ASM_VAR(AsmLabel, g_q43_68b990, $L_68b990)
ASM_VAR(AsmLabel, g_q43_64e960, $L_64e960)
ASM_VAR(AsmLabel, g_q43_64e920, $L_64e920)
ASM_VAR(AsmLabel, g_q43_675b38, $L_675b38)
#define Q43(x) ((const char*)s_q43_##x)
ASM_VAR(AsmLabel, s_q43_678008, $L_678008)
ASM_VAR(AsmLabel, s_q43_677ff4, $L_677ff4)
ASM_VAR(AsmLabel, s_q43_677fdc, $L_677fdc)
ASM_VAR(AsmLabel, s_q43_677fc4, $L_677fc4)
ASM_VAR(AsmLabel, s_q43_677fac, $L_677fac)
ASM_VAR(AsmLabel, s_q43_677f94, $L_677f94)
ASM_VAR(AsmLabel, s_q43_677f88, $L_677f88)
ASM_VAR(AsmLabel, s_q43_677f7c, $L_677f7c)
ASM_VAR(AsmLabel, s_q43_677f70, $L_677f70)
ASM_VAR(AsmLabel, s_q43_677f64, $L_677f64)
ASM_VAR(AsmLabel, s_q43_677f5c, $L_677f5c)
ASM_VAR(AsmLabel, s_q43_677f50, $L_677f50)
ASM_VAR(AsmLabel, s_q43_677f3c, $L_677f3c)
ASM_VAR(AsmLabel, s_q43_677f28, $L_677f28)
ASM_VAR(AsmLabel, s_q43_677f10, $L_677f10)
ASM_VAR(AsmLabel, s_q43_677ef8, $L_677ef8)
ASM_VAR(AsmLabel, s_q43_677ee0, $L_677ee0)
ASM_VAR(AsmLabel, s_q43_677ec8, $L_677ec8)
ASM_VAR(AsmLabel, s_q43_677ebc, $L_677ebc)
ASM_VAR(AsmLabel, s_q43_677eb0, $L_677eb0)
ASM_VAR(AsmLabel, s_q43_677ea4, $L_677ea4)
ASM_VAR(AsmLabel, s_q43_677e98, $L_677e98)
ASM_VAR(AsmLabel, s_q43_677e90, $L_677e90)
ASM_VAR(AsmLabel, s_q43_677e84, $L_677e84)
ASM_VAR(AsmLabel, s_q43_677e5c, $L_677e5c)
ASM_VAR(AsmLabel, s_q43_677e48, $L_677e48)
ASM_VAR(AsmLabel, s_q43_677e30, $L_677e30)
ASM_VAR(AsmLabel, s_q43_677e18, $L_677e18)
ASM_VAR(AsmLabel, s_q43_677e00, $L_677e00)
ASM_VAR(AsmLabel, s_q43_677de8, $L_677de8)
ASM_VAR(AsmLabel, s_q43_677dd0, $L_677dd0)
ASM_VAR(AsmLabel, s_q43_677db8, $L_677db8)
ASM_VAR(AsmLabel, s_q43_677dac, $L_677dac)
ASM_VAR(AsmLabel, s_q43_677da0, $L_677da0)
ASM_VAR(AsmLabel, s_q43_677d94, $L_677d94)
ASM_VAR(AsmLabel, s_q43_677d8c, $L_677d8c)
ASM_VAR(AsmLabel, s_q43_677d80, $L_677d80)
ASM_VAR(AsmLabel, s_q43_677d6c, $L_677d6c)
ASM_VAR(AsmLabel, s_q43_677d58, $L_677d58)
ASM_VAR(AsmLabel, s_q43_677d40, $L_677d40)
ASM_VAR(AsmLabel, s_q43_677d28, $L_677d28)
ASM_VAR(AsmLabel, s_q43_677d10, $L_677d10)
ASM_VAR(AsmLabel, s_q43_677cf8, $L_677cf8)
ASM_VAR(AsmLabel, s_q43_677ce0, $L_677ce0)
ASM_VAR(AsmLabel, s_q43_677cc8, $L_677cc8)
ASM_VAR(AsmLabel, s_q43_677cbc, $L_677cbc)
ASM_VAR(AsmLabel, s_q43_677cb0, $L_677cb0)
ASM_VAR(AsmLabel, s_q43_677ca4, $L_677ca4)
ASM_VAR(AsmLabel, s_q43_677c9c, $L_677c9c)
ASM_VAR(AsmLabel, s_q43_677c90, $L_677c90)
ASM_VAR(AsmLabel, s_q43_677e70, $L_677e70)
ASM_VAR(AsmLabel, s_q43_677c7c, $L_677c7c)
ASM_VAR(AsmLabel, s_q43_677c74, $L_677c74)
ASM_VAR(AsmLabel, s_q43_677c68, $L_677c68)
ASM_VAR(AsmLabel, s_q43_677c60, $L_677c60)
ASM_VAR(AsmLabel, s_q43_677c58, $L_677c58)
ASM_VAR(AsmLabel, s_q43_677c4c, $L_677c4c)
ASM_VAR(AsmLabel, s_q43_677c3c, $L_677c3c)
ASM_VAR(AsmLabel, s_q43_677c2c, $L_677c2c)
ASM_VAR(AsmLabel, s_q43_677c18, $L_677c18)
ASM_VAR(AsmLabel, s_q43_677c04, $L_677c04)
ASM_VAR(AsmLabel, s_q43_677bf0, $L_677bf0)
ASM_VAR(AsmLabel, s_q43_677bdc, $L_677bdc)
ASM_VAR(AsmLabel, s_q43_677bc8, $L_677bc8)
ASM_VAR(AsmLabel, s_q43_677bb4, $L_677bb4)
ASM_VAR(AsmLabel, s_q43_677ba0, $L_677ba0)
ASM_VAR(AsmLabel, s_q43_677b8c, $L_677b8c)
ASM_VAR(AsmLabel, s_q43_677b78, $L_677b78)
ASM_VAR(AsmLabel, s_q43_677b64, $L_677b64)
ASM_VAR(AsmLabel, s_q43_677b50, $L_677b50)
ASM_VAR(AsmLabel, s_q43_677b3c, $L_677b3c)
ASM_VAR(AsmLabel, s_q43_677b28, $L_677b28)
ASM_VAR(AsmLabel, s_q43_677b14, $L_677b14)
ASM_VAR(AsmLabel, s_q43_677b00, $L_677b00)
ASM_VAR(AsmLabel, s_q43_677aec, $L_677aec)

// a column of mode buttons at x 11 from y 205 (or 250) down by 45
static void Shell_ModeColumn(BYTE* p, const char* const* names, const int* ids, int n, int y)
{
    for (int k = 0; k < n; k++, y += 45) {
        BYTE* b = Shell_ModeButtonC((int)names[k], ids[k]);
        CallThis(FUN_5bdbf0, p, b, 0);
        Shell_MoveCtl(b, 11, y);
    }
}
// [FUN_523270] constructor (parent, variant): a race setup screen (vtables 64e960, 64e920 at +64) in four
// variants (+368): two button columns (+384, +388), for 1 and 3 a third panel (+392) with buttons 206, 207,
// 121, 120, 122 and the 27 x 4 textures by the 675b38 names (+428);
// the textures +396..+404 and +868..+896, centred on the parent
extern "C" BYTE* __fastcall ShellRace523270_Construct(BYTE* s, void*, BYTE* parent, int variant)
{
    CallThis(FUN_5d8900, s, parent, g_q28_720938, g_q28_72093c, g_q28_720940, g_q28_720944, 0, g_q28_6ced24, 0);
    F(DWORD, s, 424) = F(DWORD, g_q43_68b990, 0);
    F(void*, s, 0) = g_q43_64e960;
    F(void*, s, 64) = g_q43_64e920;
    F(int, s, 392) = 0;
    F(int, s, 368) = variant;
    DWORD* o = Shell_NewObjRef();
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    F(int, s, 404) = 0;
    for (int k = 920; k <= 936; k += 4)
        F(int, s, k) = 0;
    memset(s + 940, 0, 32);
    BYTE* p = Shell_NewPanel(8);
    // per variant: the background, +396, +400, +404 textures (two names each by mode), the first column
    const struct {
        const char* tex[8];
        const char* col[4];
        int ids[4];
        int n;
        const char* col2[2];
    } V[4] = {
        {{Q43(678008), Q43(677ff4), Q43(677fdc), Q43(677fc4), Q43(677fac), Q43(677f94), 0, 0},
         {Q43(677f88), Q43(677f7c), Q43(677f70), Q43(677f64)}, {190, 191, 192, 193}, 4, {Q43(677f5c), Q43(677f50)}},
        {{Q43(677f3c), Q43(677f28), Q43(677f10), Q43(677ef8), Q43(677ee0), Q43(677ec8), 0, 0},
         {Q43(677ebc), Q43(677eb0), Q43(677ea4), Q43(677e98)}, {194, 195, 196, 197}, 4, {Q43(677e90), Q43(677e84)}},
        {{Q43(677e5c), Q43(677e48), Q43(677e30), Q43(677e18), Q43(677e00), Q43(677de8), Q43(677dd0), Q43(677db8)},
         {Q43(677dac), Q43(677da0), Q43(677d94)}, {198, 199, 200}, 3, {Q43(677d8c), Q43(677d80)}},
        {{Q43(677d6c), Q43(677d58), Q43(677d40), Q43(677d28), Q43(677d10), Q43(677cf8), Q43(677ce0), Q43(677cc8)},
         {Q43(677cbc), Q43(677cb0), Q43(677ca4)}, {202, 203, 204}, 3, {Q43(677c9c), Q43(677c90)}},
    };
    if ((unsigned)F(int, s, 368) <= 3) {
        const auto& v = V[F(int, s, 368)];
        g_q34_6ce400 = CallC<BYTE*>(FUN_5ef3d0, Shell_ByMode(v.tex[0], v.tex[1]));
        for (int k = 0; k < 4; k++)
            CallThis(FUN_5bd5a0, p + 68 + k * 36, (void*)g_q29_6ce3e0);
        F(DWORD, s, 396) = CallC<DWORD>(FUN_5ef3d0, Shell_ByMode(v.tex[2], v.tex[3]));
        F(DWORD, s, 400) = CallC<DWORD>(FUN_5ef3d0, Shell_ByMode(v.tex[4], v.tex[5]));
        if (v.tex[6])
            F(DWORD, s, 404) = CallC<DWORD>(FUN_5ef3d0, Shell_ByMode(v.tex[6], v.tex[7]));
        BYTE* c = Shell_NewPanel(0);
        F(BYTE*, s, 384) = c;
        Shell_ModeColumn(c, v.col, v.ids, v.n, 205);
        CallThis(FUN_5bdbf0, p, c, 0);
        c = Shell_NewPanel(0);
        F(BYTE*, s, 388) = c;
        static const int ids2[2] = {206, 207};
        Shell_ModeColumn(c, v.col2, ids2, 2, 250);
        CallThis(FUN_5bdbf0, p, c, 0);
        if (F(int, s, 368) & 1) {
            c = Shell_NewPanel(8);
            F(BYTE*, s, 392) = c;
            g_q34_6ce400 = CallC<BYTE*>(FUN_5ef3d0, Shell_ByMode(Q43(677e70), Q43(677c7c)));
            for (int k = 0; k < 4; k++)
                CallThis(FUN_5bd5a0, c + 68 + k * 36, (void*)g_q29_6ce3e0);
            const struct { const char* n; int id, x, y; } B[5] = {
                {Q43(677c74), 206, 59, 356}, {Q43(677c68), 207, 141, 356}, {Q43(677c60), 121, 35, 84},
                {Q43(677c58), 120, 225, 84}, {Q43(677c4c), 122, 106, 186}};
            for (int k = 0; k < 5; k++) {
                BYTE* b = Shell_ModeButtonC((int)B[k].n, B[k].id);
                CallThis(FUN_5bdbf0, c, b, 0);
                Shell_MoveCtl(b, B[k].x, B[k].y);
            }
            CallThis(FUN_5bdbf0, p, c, 0);
        }
    }
    if (F(int, s, 368) == 1 || F(int, s, 368) == 3) {
        char buf[64];
        BYTE* at = s + 428;
        const char** names = (const char**)(void*)g_q43_675b38;
        for (int k = 0; k < 27; k++)
            for (int i = 1; i <= 4; i++, at += 4) {
                sprintf(buf, Shell_ByMode(Q43(677c3c), Q43(677c2c)), names[k], i);
                F(DWORD, at, 0) = CallC<DWORD>(FUN_5ef3d0, buf);
            }
    }
    if (F(BYTE*, s, 392))
        F(int, F(BYTE*, s, 392), 40) = 1;
    F(int, F(BYTE*, s, 388), 40) = 1;
    F(int, F(BYTE*, s, 384), 40) = 1;
    CallThis(FUN_5bdc80, p);
    int x = (CallVirt<int>(parent, 20) - F(WORD, g_q34_6ce400, 4)) / 2;
    int y = (CallVirt<int>(parent, 24) - F(WORD, g_q34_6ce400, 6)) / 2;
    Shell_MoveCtl(p, x, y);
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p, 0);
    F(int, s, 860) = 0;
    F(int, s, 864) = 0;
    F(int, s, 376) = 1;
    static const int slot[8] = {884, 888, 892, 896, 868, 872, 876, 880};
    const char* const tn[8][2] = {
        {Q43(677c18), Q43(677c04)}, {Q43(677bf0), Q43(677bdc)}, {Q43(677bc8), Q43(677bb4)}, {Q43(677ba0), Q43(677b8c)},
        {Q43(677b78), Q43(677b64)}, {Q43(677b50), Q43(677b3c)}, {Q43(677b28), Q43(677b14)}, {Q43(677b00), Q43(677aec)}};
    for (int k = 0; k < 8; k++)
        F(DWORD, s, slot[k]) = CallC<DWORD>(FUN_5ef3d0, Shell_ByMode(tn[k][0], tn[k][1]));
    return s;
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_519740(); KING_ALIAS("_FUN_519740", "@Thunk519740@4")
// [$L_50ff70]
extern "C" BYTE* __fastcall Dtor50ff70_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_519740, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_511800(); KING_ALIAS("_FUN_511800", "@Screen511800_Destroy@8")
// [$L_5117e0]
extern "C" BYTE* __fastcall Dtor5117e0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_511800, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_5154c0(); KING_ALIAS("_FUN_5154c0", "@Shell5154c0_Destroy@8")
// [$L_514790]
extern "C" BYTE* __fastcall Dtor514790_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5154c0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_517d40(); KING_ALIAS("_FUN_517d40", "@Shell517d40_Destroy@8")
// [$L_517a20]
extern "C" BYTE* __fastcall Dtor517a20_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_517d40, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_51e000(); KING_ALIAS("_FUN_51e000", "@Shell51e000_Destroy@8")
// [$L_51dfe0]
extern "C" BYTE* __fastcall Dtor51dfe0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_51e000, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_51e730(); KING_ALIAS("_FUN_51e730", "@Shell51e730_Destroy@8")
// [$L_51e710]
extern "C" BYTE* __fastcall Dtor51e710_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_51e730, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_51f360(); KING_ALIAS("_FUN_51f360", "@Screen51f360_Destroy@8")
// [$L_51f340]
extern "C" BYTE* __fastcall Dtor51f340_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_51f360, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_520580(); KING_ALIAS("_FUN_520580", "@Screen520580_Destroy@8")
// [$L_520560]
extern "C" BYTE* __fastcall Dtor520560_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_520580, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_521410(); KING_ALIAS("_FUN_521410", "@Screen521410_Destroy@8")
// [$L_5213f0]
extern "C" BYTE* __fastcall Dtor5213f0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_521410, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_524510(); KING_ALIAS("_FUN_524510", "@Screen524510_Destroy@8")
// [$L_5244f0]
extern "C" BYTE* __fastcall Dtor5244f0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_524510, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_525e20(); KING_ALIAS("_FUN_525e20", "@Screen525e20_Destroy@8")
// [$L_525e00]
extern "C" BYTE* __fastcall Dtor525e00_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_525e20, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_526ba0(); KING_ALIAS("_FUN_526ba0", "@Screen526ba0_Destroy@8")
// [$L_526b80]
extern "C" BYTE* __fastcall Dtor526b80_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_526ba0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_527cd0(); KING_ALIAS("_FUN_527cd0", "@Screen_Destruct@8")
// [$L_527d00]
extern "C" BYTE* __fastcall Dtor527d00_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_527cd0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_51aaf0(); KING_ALIAS("_FUN_51aaf0", "@Shell51aaf0_Destroy@8")
// [$L_51aad0]
extern "C" BYTE* __fastcall Dtor51aad0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_51aaf0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_51eca0(); KING_ALIAS("_FUN_51eca0", "@Shell51eca0_Destroy@8")
// [$L_51ec80]
extern "C" BYTE* __fastcall Dtor51ec80_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_51eca0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Small methods and static helpers: guarded static construction, releases, frees.

extern "C" void FUN_50c770(); KING_ALIAS("_FUN_50c770", "@Names_Add@12")
ASM_VAR(int, g_q49_6ce9f0, $L_6ce9f0)
// [$L_513250] adds the name to the 6ce9f0 list: 255 when it holds 8 already
extern "C" int __cdecl Names_AddLimited(const char* name)
{
    if (g_q49_6ce9f0 >= 8)
        return 255;
    return CallThis<int>(FUN_50c770, &g_q49_6ce9f0, name);
}

// ---------------------------------------------------------------------------
// Driver choice screen commands.

ASM_PROC(FUN_524b30)
ASM_VAR(BYTE*, g_q59_6cecdc, $L_6cecdc)
ASM_VAR(BYTE*, g_q59_6cece0, $L_6cece0)
ASM_VAR(BYTE*, g_q59_6cece4, $L_6cece4)
ASM_VAR(BYTE*, g_q59_6cece8, $L_6cece8)
ASM_VAR(int, g_q59_68b990, $L_68b990)
// [$L_525e40] commands of the screen part (the screen at -64): 188 moves to the next of the five drivers (+316,
// +308 the current) that is set and enabled in the +304 mask; 189 hands the current one to its slot (6cecdc..
// 6cece8) and closes the screen (slot 32)
extern "C" void __fastcall DriverChoice_OnCommand(BYTE* s, void*, int cmd)
{
    if (cmd == 188) {
        if (!F(int, s, 312))
            return;
        for (;;) {
            int i = (F(int, s, 308) + 1) % 5;
            F(int, s, 308) = i;
            if (F(int, s, 316 + i * 4) != g_q59_68b990 && (F(int, s, 304) & (1 << i)))
                return;
        }
    }
    if (cmd != 189 || !F(int, s, 312) || (unsigned)F(int, s, 308) > 4)
        return;
    int i = F(int, s, 308);
    BYTE* const slots[5] = {g_q59_6cecdc, g_q59_6cece0, g_q59_6cece4, g_q59_6cece8, g_q59_6cece8};
    CallThis(FUN_524b30, slots[i], F(int, s, 316 + i * 4));
    CallVirt(s - 64, 32);
}

extern "C" void FUN_527950(); KING_ALIAS("_FUN_527950", "@Screen_SetChild@12")
extern "C" void FUN_50c800(); KING_ALIAS("_FUN_50c800", "_CD_Check")
extern "C" void FUN_5279e0(); KING_ALIAS("_FUN_5279e0", "_Movie_Play")
ASM_VAR(BYTE*, g_q59_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_q59_6cec70, $L_6cec70)
ASM_VAR(BYTE*, g_q59_6cec8c, $L_6cec8c)
ASM_VAR(BYTE*, g_q59_6cec90, $L_6cec90)
ASM_VAR(BYTE*, g_q59_6cec94, $L_6cec94)
ASM_VAR(BYTE*, g_q59_6cec9c, $L_6cec9c)
ASM_VAR(BYTE*, g_q59_6cecd8, $L_6cecd8)
ASM_VAR(AsmLabel, s_q59_678278, $L_678278)
// [$L_527b90] commands of the main menu part (the screen at -64): 101..108 open their screens as the child, 106
// plays the intro movie at 640x480 (with the CD) and closes the menu
extern "C" void __fastcall MainMenu_OnCommand(BYTE* s, void*, int cmd)
{
    BYTE* scr = s - 64;
    switch (cmd) {
    case 101: CallThis(FUN_527950, scr, g_q59_6cec90); break;
    case 102:
        F(int, g_q59_6cec6c, 1316) = 0;
        CallThis(FUN_527950, scr, g_q59_6cec9c);
        break;
    case 103: CallThis(FUN_527950, scr, g_q59_6cec6c); break;
    case 104: CallThis(FUN_527950, scr, g_q59_6cec94); break;
    case 105:
        F(int, g_q59_6cecd8, 612) = 0;
        CallThis(FUN_527950, scr, g_q59_6cecd8);
        break;
    case 106:
        if (!CallC<int>(FUN_50c800, 0))
            break;
        CallThis($L_5dd8c0, (BYTE*)g_gs696750, 640, 480, 2, 15872);
        if (!CallC<int>(FUN_5279e0, (const char*)s_q59_678278, 1))
            break;
        if (F(BYTE*, s, 308))
            CallVirt(F(BYTE*, s, 308), 32);
        F(BYTE*, s, 308) = 0;
        CallVirt(scr, 32);
        break;
    case 107: CallThis(FUN_527950, scr, g_q59_6cec70); break;
    case 108: CallThis(FUN_527950, scr, g_q59_6cec8c); break;
    }
}

// ---------------------------------------------------------------------------
// Driver choice list drawing.

extern "C" void FUN_5b86c0(); KING_ALIAS("_FUN_5b86c0", "_Bitmap_Draw")
extern "C" void FUN_5c0020(); KING_ALIAS("_FUN_5c0020", "@List_DrawAt@12")
extern "C" void FUN_569740(); KING_ALIAS("_FUN_569740", "_Messages_Get1288")
extern "C" void FUN_5ea170(); KING_ALIAS("_FUN_5ea170", "_Text_Draw")
// [$L_525f70] draws the list part on dc: the movie frame first when one plays, the list, then the caption of the
// current driver (texts 111..116; 111 with none) in the message font at +51, +116 of the +288 frame
extern "C" void __fastcall DriverChoice_Draw(BYTE* s, void*, int dc)
{
    if (g_6ce4f4)
        CallC(FUN_5b86c0, F(int, g_69684c, 12), 0, 0, g_6ce4f4);
    CallThis(FUN_5c0020, s, dc);
    BYTE* fr = F(BYTE*, s, 288);
    int x = F(int, fr, 44);
    int y = F(int, fr, 48);
    int id;
    if (!F(int, s, 312))
        id = 111;
    else if ((unsigned)F(int, s, 308) <= 4)
        id = 112 + F(int, s, 308);
    else
        return;
    const char* t = CallC<const char*>(FUN_5d2e50, id);
    int font = CallC<int>(FUN_569740);
    CallC(FUN_5ea170, F(int, s, -52), x + 51, y + 116, font, t, 4, 0);
}

// ---------------------------------------------------------------------------
// Command handlers of the race start and the track choice screens.

extern "C" void FUN_4ff770(); KING_ALIAS("_FUN_4ff770", "@Car_SetAuto@12")
extern "C" void FUN_5c0c00(); KING_ALIAS("_FUN_5c0c00", "@List_IndexOf300@8")
ASM_VAR(BYTE*, g_q61_6cecc8, $L_6cecc8)
ASM_VAR(BYTE*, g_q61_6cecd0, $L_6cecd0)
ASM_VAR(int, g_q61_6968f8, $L_6968f8)
ASM_VAR(DWORD, g_q61_723da8, $L_723da8)
// the transmission of the player's car by +330, then its +1404 flags (kept or reset) with bit 8 by +332
static void RaceStart_ApplyCar(BYTE* s, bool reset)
{
    CallThis(FUN_4ff770, F(BYTE*, F(BYTE*, g_gamshe6d2078, 616), 21600), (int)F(short, s, 330));
    DWORD f = reset ? 0xfffffeff : F(DWORD, g_gamshe6d2078, 1404) & ~0x100u;
    F(DWORD, g_gamshe6d2078, 1404) = f;
    if (F(short, s, 332))
        F(DWORD, g_gamshe6d2078, 1404) |= 0x100;
}
// [$L_517ad0] commands of the race start part (the screen at -64): 2 and 27 keep the choices (+308..+316 to
// +324..+332), close and apply them; 178/179 the manual or automatic gearbox; 184 starts the race: the movie
// and the screens go, the race window +304 becomes active and the car takes the choices
extern "C" void __fastcall RaceStart_OnCommand(BYTE* s, void*, int cmd)
{
    switch (cmd) {
    case 2:
    case 27:
        for (int k = 0; k < 5; k++)
            F(WORD, s, 324 + k * 2) = F(WORD, s, 308 + k * 2);
        CallVirt(s - 64, 32);
        CallThis(FUN_5d8610, g_gs6ced6c);
        RaceStart_ApplyCar(s, false);
        break;
    case 178:
        F(WORD, s, 330) = 1;
        break;
    case 179:
        F(WORD, s, 330) = 0;
        break;
    case 184:
        CallVirt(s - 64, 32);
        if (g_6ce4f4)
            CallVirt(g_6ce4f4, 0, 1);
        g_6ce4f4 = 0;
        F(DWORD, g_696ca0, 16) |= 3;
        CallC(FUN_569710);
        CallVirt(g_696ca0, 36);
        CallThis(FUN_5e4250, (BYTE*)g_gs696750, F(int, s, 304));
        CallC(FUN_50bd60);
        CallC(FUN_4e12b0, 1);
        RaceStart_ApplyCar(s, true);
        break;
    }
}
// [$L_51ab90] commands of the track choice part (the screen at -64): 2 back to 6cec80, 118 starts the chosen track
// (with the CD; the tutorial record 723da8 goes to 6cecc8, others to 6cecd0), 119 to the 6cec98 screen, 120/121
// the next/previous of the eleven pictures (+744), 122 the next of four views (+748)
extern "C" void __fastcall TrackChoice_OnCommand(BYTE* s, void*, int cmd)
{
    switch (cmd) {
    case 2:
        CallVirt(s - 64, 32);
        F(int, g_6cec80, 372) = 0;
        CallVirt(g_6cec80, 36);
        break;
    case 118: {
        if (!CallC<int>(FUN_50c800, g_q59_6cec94))
            break;
        g_q61_6968f8 = CallThis<int>(FUN_5c0c00, F(BYTE*, s, 484));
        CallVirt(s - 64, 32);
        F(int, g_6cec80, 372) = 0;
        F(int, g_q59_6cec6c, 1316) = 2;
        const DWORD* rec = (const DWORD*)(s + 488 + g_q61_6968f8 * 32);
        const DWORD* tut = &g_q61_723da8;
        if (rec[0] == tut[0] && rec[1] == tut[1] && rec[2] == tut[2] && rec[3] == tut[3]) {
            F(BYTE, g_q61_6cecd0, 396) = 0;
            CallThis(FUN_519750, g_q61_6cecc8, 0);
        } else
            CallThis(FUN_519750, g_q61_6cecd0, 0);
        break;
    }
    case 119:
        F(BYTE*, g_6cec80, 372) = g_6cec98s;
        CallVirt(s - 64, 32);
        CallVirt(g_6cec98s, 36);
        g_q61_6968f8 = CallThis<int>(FUN_5c0c00, F(BYTE*, s, 484));
        break;
    case 120:
        F(int, s, 744) = (F(int, s, 744) + 1) % 11;
        break;
    case 121:
        if (--F(int, s, 744) < 0)
            F(int, s, 744) += 11;
        break;
    case 122:
        F(int, s, 748) = (F(int, s, 748) + 1) % 4;
        break;
    }
}

// ---------------------------------------------------------------------------
// Command handlers of the options and the race end screens.

extern "C" void FUN_510bc0(); KING_ALIAS("_FUN_510bc0", "_Shell_StartGame")
extern "C" void FUN_51e070(); KING_ALIAS("_FUN_51e070", "@Screen_SetPlayer@12")
extern "C" void FUN_511820(); KING_ALIAS("_FUN_511820", "_Shell_LeaveRace")
ASM_VAR(AsmLabel, s_q62_676978, $L_676978)
ASM_VAR(AsmLabel, s_q62_67696c, $L_67696c)
ASM_VAR(AsmLabel, s_q62_676cfc, $L_676cfc)
ASM_VAR(AsmLabel, s_q62_677638, $L_677638)
ASM_VAR(AsmLabel, s_q62_677630, $L_677630)
ASM_VAR(AsmLabel, s_q62_66d9c4, $L_66d9c4)
static void Options_WriteIni(const char* key, int v)
{
    CallC(FUN_603c70, (const char*)s_672998, key, v, g_q32_696868);
}
// [$L_51d380] commands of the options part (the screen at -64): 2 cancels (back to 6cec80, the choices
// +316..+320 kept in +336..+340), 109 saves them to the ini and starts, 110 starts the chosen track (+332, its
// +300 name unless the default text 85), 111/112 and 273/274 switch the +1304 and +1312 options
extern "C" void __fastcall Options_OnCommand(BYTE* s, void*, int cmd)
{
    switch (cmd) {
    case 2:
        CallVirt(s - 64, 32);
        F(int, g_6cec80, 372) = 0;
        CallVirt(g_6cec80, 36);
        F(WORD, s, 336) = F(WORD, s, 316);
        F(WORD, s, 338) = F(WORD, s, 318);
        F(WORD, s, 340) = F(WORD, s, 320);
        break;
    case 109:
        if (!CallC<int>(FUN_50c800, g_q59_6cec9c))
            break;
        Options_WriteIni((const char*)s_q62_676978, F(int, g_q59_6cec6c, 1304));
        Options_WriteIni((const char*)s_q62_67696c, F(int, g_q59_6cec6c, 1312));
        Options_WriteIni((const char*)s_q62_676cfc, F(short, s, 336));
        Options_WriteIni((const char*)s_q62_677638, F(short, s, 338));
        Options_WriteIni((const char*)s_q62_677630, F(short, s, 340));
        CallC(FUN_510bc0, 0, 1);
        break;
    case 110: {
        if (!CallC<int>(FUN_50c800, g_q59_6cec9c))
            break;
        BYTE* t = F(BYTE*, F(BYTE*, s, 332), 300);
        if (!t || !F(const char*, t, 12))
            break;
        if (strcmp(F(const char*, t, 12), CallC<const char*>(FUN_5d2e50, 85)) == 0)
            break;
        t = F(BYTE*, F(BYTE*, s, 332), 300);
        char buf[64];
        sprintf(buf, (const char*)s_q62_66d9c4, t ? F(const char*, t, 12) : 0);
        CallC(FUN_510bc0, buf, 1);
        break;
    }
    case 111: F(int, g_q59_6cec6c, 1304) = 1; break;
    case 112: F(int, g_q59_6cec6c, 1304) = 0; break;
    case 273: F(int, g_q59_6cec6c, 1312) = 1; break;
    case 274: F(int, g_q59_6cec6c, 1312) = 0; break;
    }
}
// [$L_51ed30] commands of the race end part (the screen at -64): 27 and 172 close it back to the shell window,
// 170 hands the player +308 to 6ceca0, 171 leaves: the movie and the screens go, then the race (+308 1) or the
// game (2: 6968a4)
extern "C" void __fastcall RaceEnd_OnCommand(BYTE* s, void*, int cmd)
{
    switch (cmd) {
    case 27:
    case 172:
        CallVirt(s - 64, 32);
        CallThis(FUN_5d8610, g_gs6ced6c);
        break;
    case 170:
        CallThis(FUN_51e070, g_6ceca0, F(int, s, 308));
        CallVirt(s - 64, 32);
        break;
    case 171:
        CallVirt(s - 64, 32);
        if (g_6ce4f4)
            CallVirt(g_6ce4f4, 0, 1);
        g_6ce4f4 = 0;
        F(DWORD, g_696ca0, 16) |= 3;
        CallC(FUN_569710);
        CallC(FUN_50bd60);
        CallC(FUN_4e12b0, 0);
        if (F(int, s, 308) == 1)
            CallC(FUN_511820);
        else if (F(int, s, 308) == 2)
            g_gs6968a4 = 1;
        break;
    }
}

// ---------------------------------------------------------------------------
// Command handler of the race settings screen.

ASM_VAR(BYTE*, g_q63_6ceccc, $L_6ceccc)
ASM_VAR(AsmLabel, s_q63_676cf4, $L_676cf4)
ASM_VAR(AsmLabel, s_q63_67748c, $L_67748c)
ASM_VAR(AsmLabel, s_q63_677484, $L_677484)
// [$L_51b350] commands of the race settings part (the screen at -64): 2 cancels (back to 6cec80, the choices
// +312..+316 kept in +324..+328), 111/112 the +1304 option, 124 saves the settings to the ini and starts the
// 6ceccc race (with the CD), 150..152 the +320 choice
extern "C" void __fastcall RaceSettings_OnCommand(BYTE* s, void*, int cmd)
{
    switch (cmd) {
    case 2:
        CallVirt(s - 64, 32);
        F(int, g_6cec80, 372) = 0;
        CallVirt(g_6cec80, 36);
        F(WORD, s, 324) = F(WORD, s, 312);
        F(WORD, s, 326) = F(WORD, s, 314);
        F(WORD, s, 328) = F(WORD, s, 316);
        break;
    case 111: F(int, g_q59_6cec6c, 1304) = 1; break;
    case 112: F(int, g_q59_6cec6c, 1304) = 0; break;
    case 124:
        if (!CallC<int>(FUN_50c800, g_6cec98s))
            break;
        Options_WriteIni((const char*)s_q62_676978, F(int, g_q59_6cec6c, 1304));
        Options_WriteIni((const char*)s_q63_676cf4, F(short, s, 324));
        Options_WriteIni((const char*)s_q63_67748c, F(short, s, 326));
        Options_WriteIni((const char*)s_q63_677484, F(short, s, 328));
        F(int, g_q59_6cec6c, 1316) = 1;
        F(int, g_6cec80, 372) = 0;
        CallVirt(s - 64, 32);
        CallThis(FUN_519750, g_q63_6ceccc, 0);
        break;
    case 150: F(int, s, 320) = 0; break;
    case 151: F(int, s, 320) = 1; break;
    case 152: F(int, s, 320) = 2; break;
    }
}

// ---------------------------------------------------------------------------
// The load screen: listing and commands.

extern "C" void FUN_5c0f00(); KING_ALIAS("_FUN_5c0f00", "@ListBox_AddSorted@12")
extern "C" void FUN_51d9e0(); KING_ALIAS("_FUN_51d9e0", "_Save_DeleteEntry")
extern "C" void FUN_5c0d60(); KING_ALIAS("_FUN_5c0d60", "@List_RemoveName@12")
ASM_VAR(AsmLabel, s_q64_6772c4, $L_6772c4)
// [$L_518900] on show: activates, takes the +1300 game kind (+368, flag 2 of the +372 button), lists the saved
// games (the 6772c4 files, names without the extension) in the +380 list and marks the +376 control
extern "C" void __fastcall LoadScreen_OnShow(BYTE* s, void*)
{
    CallThis(FUN_5d8610, s);
    F(int, s, 368) = F(int, g_q59_6cec6c, 1300);
    if (F(int, s, 368))
        F(DWORD, F(BYTE*, s, 372), 284) |= 2;
    else
        F(DWORD, F(BYTE*, s, 372), 284) &= ~2u;
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA((const char*)s_q64_6772c4, &fd);
    CallThis(FUN_5c0cf0, F(BYTE*, s, 380));
    F(DWORD, F(BYTE*, s, 376), 32) |= 0x80000000;
    if (h == INVALID_HANDLE_VALUE)
        return;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char name[MAX_PATH];
            strcpy(name, fd.cFileName);
            int i = 0;
            while (name[i] && name[i] != '.')
                i++;
            name[i] = 0;
            CallThis(FUN_5c0f00, F(BYTE*, s, 380), name);
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}
// [$L_51e880] commands of the load part (the screen at -64): 27 and 172 close it back to the shell window, 110
// loads the chosen game (+440, its +300 name into 6f2d18) and starts it, 282 deletes the chosen save
extern "C" void __fastcall LoadScreen_OnCommand(BYTE* s, void*, int cmd)
{
    if (cmd == 27 || cmd == 172) {
        CallVirt(s - 64, 32);
        CallThis(FUN_5d8610, g_gs6ced6c);
        return;
    }
    if (cmd != 110 && cmd != 282)
        return;
    BYTE* t = F(BYTE*, F(BYTE*, s, 440), 300);
    const char* name = t ? F(const char*, t, 12) : 0;
    if (cmd == 282) {
        CallC(FUN_51d9e0, name);
        CallThis(FUN_5c0d60, F(BYTE*, s, 440), name);
        return;
    }
    strcpy((char*)&g_6f2d18, name);
    CallVirt(s - 64, 32);
    CallC(FUN_50bd60);
    CallC(FUN_4e12b0, 1);
    if (g_6ce4f4)
        CallVirt(g_6ce4f4, 0, 1);
    g_6ce4f4 = 0;
    F(DWORD, g_696ca0, 16) |= 3;
    CallC(FUN_569710);
    F(int, g_696ca0, 3024) = 1;
    CallVirt(g_696ca0, 36);
}

// ---------------------------------------------------------------------------
// The network session screen on show.

extern "C" void FUN_51bd50(); KING_ALIAS("_FUN_51bd50", "_Storage_OpenStream")
extern "C" void FUN_5c0e90(); KING_ALIAS("_FUN_5c0e90", "@List_FindName@12")
ASM_VAR(AsmLabel, s_q65_677580, $L_677580)
typedef long(__stdcall* StreamRead65)(void*, void*, ULONG, ULONG*);
typedef ULONG(__stdcall* ComRelease65)(void*);
// [$L_51bdc0] on show: lists the saved sessions (the 677580 files, names without the extension) in +604, keeps
// the player name (+632 to +668), activates, reads the two saved values (+616, +624 when present) from the
// settings stream and keeps them (+620, +628), then selects the player's entry (or the first)
extern "C" void __fastcall SessionScreen_OnShow(BYTE* s, void*)
{
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA((const char*)s_q65_677580, &fd);
    CallThis(FUN_5c0cf0, F(BYTE*, s, 604));
    F(DWORD, F(BYTE*, s, 608), 32) |= 0x80000000;
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                char name[MAX_PATH];
                strcpy(name, fd.cFileName);
                int i = 0;
                while (name[i] && name[i] != '.')
                    i++;
                name[i] = 0;
                CallThis(FUN_5c1140, F(BYTE*, s, 604), name);
            }
        } while (FindNextFileA(h, &fd));
    }
    strcpy((char*)s + 668, (const char*)s + 632);
    CallThis(FUN_5d8610, s);
    void* stg = 0;
    void* st = CallC<void*>(FUN_51bd50, &stg, 1);
    if (st) {
        ULONG got;
        DWORD buf[2];
        ((StreamRead65)(*(void***)st)[3])(st, buf, 8, &got);
        F(DWORD, s, 616) = buf[0];
        if (got > 4)
            F(DWORD, s, 624) = buf[1];
        ((ComRelease65)(*(void***)st)[2])(st);
    }
    if (stg)
        ((ComRelease65)(*(void***)stg)[2])(stg);
    F(DWORD, s, 628) = F(DWORD, s, 624);
    F(DWORD, s, 620) = F(DWORD, s, 616);
    int k = CallThis<int>(FUN_5c0e90, F(BYTE*, s, 604), (const char*)s + 632);
    if (k < 0)
        k = 0;
    CallThis(FUN_5c0c20, F(BYTE*, s, 604), k);
}

// ---------------------------------------------------------------------------
// The video mode choice of a driver.

extern "C" void FUN_515710(); KING_ALIAS("_FUN_515710", "_Shell_DefaultSettings")
extern "C" int __cdecl Mode_TextureLevel(void);    // core.cpp
ASM_VAR(int, g_q66_7208e4, $L_7208e4)
ASM_VAR(AsmLabel, s_q66_676d30, $L_676d30)
ASM_VAR(AsmLabel, s_q66_676d28, $L_676d28)
ASM_VAR(AsmLabel, s_q66_676d20, $L_676d20)
ASM_VAR(AsmLabel, s_q66_676d18, $L_676d18)
// [FUN_5159d0] the video modes of driver d: the ini settings of 640x480, 800x600, 1024x768 and 1600x1200 (the
// mode and whether one was chosen); when none ran the dialog and none is chosen, the entries are reset to -1 and
// read again (the last pair then lands on the 1024x768 one, as the original does); 0 when nothing is usable
// (7208e4 restored), else 1 with the texture level and the four pairs
extern "C" int __cdecl Video_PickModes(int d, int* level, int* m640, int* c640, int* m800, int* c800, int* m1024,
                                       int* c1024, int* m1600, int* c1600)
{
    int old = g_q66_7208e4;
    g_q66_7208e4 = d;
    int p640 = 0, q640 = 0, p800 = 0, q800 = 0, p1024 = 0, q1024 = 0, p1600 = 0, q1600 = 0;
    int ran = Shell_VideoSettings(d, (const char*)s_q66_676d30, 640, 480, 2, 30208, &p640, &q640);
    ran |= Shell_VideoSettings(d, (const char*)s_q66_676d28, 800, 600, 2, 30208, &p800, &q800);
    ran |= Shell_VideoSettings(d, (const char*)s_q66_676d20, 1024, 768, 2, 30208, &p1024, &q1024);
    ran |= Shell_VideoSettings(d, (const char*)s_q66_676d18, 1600, 1200, 2, 30208, &p1600, &q1600);
    if (!ran) {
        if (q640 || q800 || q1024 || q1600)
            goto done;
        char sec[16];
        sprintf(sec, s_gs676ce4, d + 1);
        CallC(FUN_603c70, sec, (const char*)s_q66_676d30, -1, g_q32_696868);
        CallC(FUN_603c70, sec, (const char*)s_q66_676d28, -1, g_q32_696868);
        CallC(FUN_603c70, sec, (const char*)s_q66_676d20, -1, g_q32_696868);
        CallC(FUN_603c70, sec, (const char*)s_q66_676d18, -1, g_q32_696868);
        Shell_VideoSettings(d, (const char*)s_q66_676d30, 640, 480, 2, 30208, &p640, &q640);
        Shell_VideoSettings(d, (const char*)s_q66_676d28, 800, 600, 2, 30208, &p800, &q800);
        Shell_VideoSettings(d, (const char*)s_q66_676d20, 1024, 768, 2, 30208, &p1024, &q1024);
        Shell_VideoSettings(d, (const char*)s_q66_676d18, 1600, 1200, 2, 30208, &p1024, &q1024);
    }
    CallC(FUN_515710, q640, q800, q1024, q1600);
    if (!q640 && !q800 && !q1024 && !q1600) {
        g_q66_7208e4 = old;
        return 0;
    }
done:
    *level = Mode_TextureLevel();
    *m640 = p640;
    *c640 = q640;
    *m800 = p800;
    *c800 = q800;
    *m1024 = p1024;
    *c1024 = q1024;
    *m1600 = p1600;
    *c1600 = q1600;
    return 1;
}

// ---------------------------------------------------------------------------
// The sound sliders and the music start of a screen.

ASM_VAR(void*, g_q67_6cec78, $L_6cec78)
ASM_VAR(BYTE*, g_q67_6cec80, $L_6cec80)
ASM_VAR(BYTE*, g_q67_6cecd0, $L_6cecd0)
ASM_VAR(int, g_q67_6968f0, $L_6968f0)
ASM_VAR(void*, g_q67_69690c, $L_69690c)
ASM_VAR(char, g_q67_696750, $L_696750)
ASM_VAR(AsmLabel, s_q67_6773d8, $L_6773d8)
ASM_VAR(double, g_q67_64b5a0, $L_64b5a0)
ASM_VAR(double, g_q67_64b930, $L_64b930)
extern "C" int __cdecl Names_AddLimited(const char* name);

// [$L_516d50] the effects volume slider: the sound volume (+382) and the music volume (+382 * +384)
extern "C" void __cdecl Shell_EffectsSlider(void)
{
    BYTE* o = (BYTE*)g_q67_6cec78;
    CallC<double>(FUN_5fcd40, (float)((double)F(short, o, 382) * g_q67_64b5a0));
    o = (BYTE*)g_q67_6cec78;
    CallC(FUN_5ffc30, (double)(F(short, o, 384) * F(short, o, 382)) * g_q67_64b930);
}

// [$L_516db0] the music slider: the music volume (+382 * +384); above 5 the music starts (+392 set), below 5 it
// stops
extern "C" void __cdecl Shell_MusicSlider(void)
{
    BYTE* o = (BYTE*)g_q67_6cec78;
    CallC(FUN_5ffc30, (double)(F(short, o, 384) * F(short, o, 382)) * g_q67_64b930);
    o = (BYTE*)g_q67_6cec78;
    if (F(short, o, 384) > 5 && !F(short, o, 392)) {
        CallC(FUN_5ffa30);
        F(short, (BYTE*)g_q67_6cec78, 392) = 1;
        o = (BYTE*)g_q67_6cec78;
    }
    if (F(short, o, 384) < 5 && F(short, o, 392)) {
        CallC(FUN_5ffb20);
        F(short, (BYTE*)g_q67_6cec78, 392) = 0;
    }
}

// [FUN_519750] shows the screen in mode m (+368): with +376 == 2 the music of 6cecd0's +396 plays and +384 takes
// the time, with 3 the default music plays and new names go through 513250; without music 6cec80 shows instead
extern "C" void __fastcall Shell_ShowWithMusic(BYTE* s, void*, int m)
{
    F(int, s, 368) = m;
    int k = F(int, s, 376);
    if (k == 2) {
        CallThis(FUN_5c39e0, &g_q67_696750, (const char*)s_q67_6773d8, g_q67_6cecd0 + 396, 0);
        if (!g_q67_6968f0) {
            CallVirt(g_q67_6cec80, 36);
            return;
        }
        F(double, s, 384) = CallC<double>(FUN_6037e0);
    } else if (k == 3) {
        CallThis(FUN_5c39e0, &g_q67_696750, (const char*)s_q67_6773d8, 0, 0);
        if (!g_q67_6968f0) {
            CallVirt(g_q67_6cec80, 36);
            return;
        }
        g_q67_69690c = (void*)Names_AddLimited;
    }
    CallVirt(s, 36);
}

// ---------------------------------------------------------------------------
// Leaving the game.

extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_5ef640(); KING_ALIAS("_FUN_5ef640", "@Node_Root@8")
extern "C" void FUN_5dd4d0(); KING_ALIAS("_FUN_5dd4d0", "_App_CheckThread")
ASM_VAR(BYTE*, g_q68_6d2078, $L_6d2078)
ASM_VAR(BYTE*, g_q68_6cec6c, $L_6cec6c)
ASM_VAR(void*, g_q68_6ced90, $L_6ced90)
ASM_VAR(AsmLabel, s_q68_678288, $L_678288)
// [$L_527d20] on the way out: logs the player truck's position and room (6d2078), stops the music of a race
// (6cec6c's +1344), frees the AI and calls the 6ced90 hook
extern "C" void __cdecl Game_Leave(void)
{
    if (g_q68_6d2078) {
        BYTE* o = F(BYTE*, g_q68_6d2078, 616);
        BYTE m[48];
        float* p = (float*)(CallThis<BYTE*>(FUN_57de90, o + 16, m) + 36);
        float x = p[0], y = p[1], z = p[2];
        const char* room = F(const char*, CallThis<BYTE*>(FUN_5ef640, o + 272), 12);
        char buf[124];
        sprintf(buf, (const char*)s_q68_678288, (double)x, (double)y, (double)z, room);
        CallC(FUN_5d2ba0, buf, 0);
    }
    CallC(FUN_5dd4d0);
    if (g_q68_6cec6c && F(int, g_q68_6cec6c, 1344)) {
        CallC(FUN_5ffb20);
        CallC(FUN_5ffb40);
    }
    CallC(FUN_42db90);
    CallC(FUN_435a80);
    ((void(__cdecl*)())g_q68_6ced90)();
}

// ---------------------------------------------------------------------------
// A line on the map.

ASM_PROC(FUN_5b73a0)
extern "C" void FUN_5b6210(); KING_ALIAS("_FUN_5b6210", "_Pixel_Pack")
// [FUN_5290d0] draws the line of box s (doubles x0, y0, x1, y1) scaled by sx, sy and moved by dx, dy on surface
// d in green; with clip set nothing is drawn when an end lies outside d
extern "C" void __fastcall Map_DrawLine(double* s, void*, BYTE* d, double sx, double sy, double dx, double dy,
                                        int clip)
{
    int x0 = (int)(sx * s[0] + dx);
    int x1 = (int)(sx * s[2] + dx);
    int y0 = (int)(sy * s[1] + dy);
    int y1 = (int)(sy * s[3] + dy);
    if (clip && (Carat_OutOfBox((const unsigned short*)d, x0, y0) || Carat_OutOfBox((const unsigned short*)d, x1, y1)))
        return;
    CallC(FUN_5b73a0, d, x0, y0, x1, y1, CallC<int>(FUN_5b6210, 0, 255, 120, 1));
}

// ---------------------------------------------------------------------------
// Entering the world.

extern "C" void FUN_512920(); KING_ALIAS("_FUN_512920", "_Shell_LoadWorld")
ASM_PROC(FUN_50c9d0)
extern "C" void FUN_5ba300(); KING_ALIAS("_FUN_5ba300", "@Palette_Blend@32")
extern "C" void FUN_597560(); KING_ALIAS("_FUN_597560", "@Wasm_Set92@12")
ASM_VAR(BYTE*, g_q70_696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_q70_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_q70_6cec70, $L_6cec70)
ASM_VAR(BYTE*, g_q70_6cec78, $L_6cec78)
ASM_VAR(BYTE*, g_q70_6cec90, $L_6cec90)
ASM_VAR(int, g_q70_6ced64, $L_6ced64)
ASM_VAR(int, g_q70_6ced0c, $L_6ced0c)
ASM_VAR(int, g_q70_6ced10, $L_6ced10)
ASM_VAR(int, g_q70_6ced14, $L_6ced14)
ASM_VAR(int, g_q70_6ced18, $L_6ced18)
ASM_VAR(int, g_q70_6ced1c, $L_6ced1c)
ASM_VAR(int, g_q70_6ced20, $L_6ced20)
ASM_VAR(AsmLabel, g_q70_6f68b0, $L_6f68b0)
ASM_VAR(char, g_q70_696750, $L_696750)
ASM_VAR(AsmLabel, s_q70_6767a0, $L_6767a0)
ASM_VAR(double, g_q70_64b5a0, $L_64b5a0)
// [FUN_510f20] with movie set plays the 6767a0 movie; else leaves the network wait (6cec6c +1300/+1316, control
// 146 of 6cec90 unmarked), sets the view rectangle of the mode (6cec70 +408 bits 4-7; 50 pixels added on the left,
// top and bottom), loads the world, fades the palette in, restores the sound volume
extern "C" void __cdecl Shell_EnterWorld(int movie)
{
    if (movie) {
        CallC(FUN_5279e0, (const char*)s_q70_6767a0, 6);
        return;
    }
    BYTE* o = g_q70_6cec6c;
    if (F(int, o, 1300) == 1 && F(int, o, 1316)) {
        F(int, o, 1300) = 0;
        BYTE* c = CallThis<BYTE*>(FUN_5bdb50, g_q70_6cec90 + 64, 146);
        F(unsigned, c, 284) &= ~2u;
    }
    g_q70_6ced64 = 1;
    int x, y, b;
    switch (F(int, g_q70_6cec70, 408) & 240) {
    case 16: x = y = b = -360; g_q70_6ced0c = 0; g_q70_6ced14 = 416; g_q70_6ced1c = 254; break;
    case 32: x = y = b = -300; g_q70_6ced0c = 0; g_q70_6ced14 = 576; g_q70_6ced1c = 414; break;
    case 48: x = y = b = -216; g_q70_6ced0c = 0; g_q70_6ced14 = 800; g_q70_6ced1c = 639; break;
    case 64: x = y = b = 0; g_q70_6ced0c = 0; g_q70_6ced14 = 1376; g_q70_6ced1c = 1215; break;
    default: x = g_q70_6ced10; y = g_q70_6ced18; b = g_q70_6ced20; break;
    }
    g_q70_6ced18 = y - 50;
    g_q70_6ced10 = x - 50;
    g_q70_6ced20 = b - 50;
    CallVirt(g_q70_696ca0, 36);
    CallC(FUN_512920);
    CallThis(FUN_5ba300, g_q70_696ca0 + 3096, 0, 255, 0, 0, 0, 255);
    CallThis(FUN_5ba3d0, g_q70_696ca0 + 3096);
    CallThis(FUN_597560, (BYTE*)g_q70_6f68b0, g_q70_696ca0);
    CallC(FUN_50c9d0);
    CallVirt(g_q70_696ca0, 136);
    if (int e = F(int, g_q70_696ca0, 2856))
        CallThis(FUN_5db9f0, &g_q70_696750, e);
    CallVirt(g_q70_696ca0, 116);
    F(int, g_q70_696ca0, 7940) = 0;
    CallC<double>(FUN_5fcd40, (float)((double)F(short, g_q70_6cec78, 382) * g_q70_64b5a0));
    g_q70_6ced64 = 0;
}

// ---------------------------------------------------------------------------
// The input settings screen.

ASM_VAR(AsmLabel, g_q71_6d1bb0, $L_6d1bb0)
ASM_VAR(AsmLabel, g_q71_6d1b4e, $L_6d1b4e)
ASM_VAR(AsmLabel, g_q71_6d1b8e, $L_6d1b8e)
// [$L_513a30] on show: the working copy (+528, 376 bytes) goes to +904 and +1280 to +520; the three axis values
// (6d1bb0, every 12 bytes) to +368 and +432; the 12 bytes from 6d1b4e and 4 from 6d1b8e (every 2) give the bits of
// +500/+496 and +512/+508
extern "C" void __fastcall Shell_InputOnShow(BYTE* s)
{
    CallThis(FUN_5d8610, s);
    int v = F(int, s, 1280);
    memcpy(s + 904, s + 528, 376);
    F(int, s, 520) = v;
    const float* a = (const float*)g_q71_6d1bb0;
    for (int i = 0; i < 3; i++) {
        double d = a[i * 3];
        F(double, s, 368 + 8 * i) = d;
        F(double, s, 432 + 8 * i) = d;
    }
    F(int, s, 496) = 0;
    F(int, s, 500) = 0;
    F(int, s, 504) = 0;
    F(int, s, 508) = 0;
    F(int, s, 512) = 0;
    const BYTE* b = (const BYTE*)g_q71_6d1b4e;
    for (int i = 0; i < 12; i++) {
        F(unsigned, s, 500) |= b[i * 2] ? 1u << i : 0;
        F(unsigned, s, 496) |= F(unsigned, s, 500);
    }
    b = (const BYTE*)g_q71_6d1b8e;
    for (int i = 0; i < 4; i++) {
        F(unsigned, s, 512) |= b[i * 2] ? 1u << i : 0;
        F(unsigned, s, 508) |= F(unsigned, s, 512);
    }
}

// ---------------------------------------------------------------------------
// Applying the input settings.

ASM_VAR(int, g_q84_6d205c, $L_6d205c)
ASM_VAR(int, g_q84_6d2060, $L_6d2060)
ASM_VAR(int, g_q84_6d2068, $L_6d2068)
ASM_VAR(BYTE*, g_q84_6d2040, $L_6d2040)
ASM_VAR(BYTE*, g_q84_6d203c, $L_6d203c)
ASM_VAR(BYTE*, g_q84_6d2044, $L_6d2044)
ASM_VAR(AsmLabel, g_q84_6d1948, $L_6d1948)
ASM_VAR(AsmLabel, g_q84_6d1bb0, $L_6d1bb0)
ASM_VAR(BYTE*, g_q84_6d1ec0, $L_6d1ec0)
ASM_VAR(BYTE*, g_q84_6d1f7c, $L_6d1f7c)
// [FUN_5147b0] applies the bindings of the input screen s: the joystick flag (+1280), the key states cleared
// (6d1969 set), the joystick axes of actions 0-3 (+716.., from 300 on: axis (v-300)/4, odd ones inverted) and the
// key pointers of the 47 actions (+528.. and +716.. below 296); the keys of most actions are marked as used
extern "C" void __fastcall Input_ApplyBindings(BYTE* s)
{
    BYTE* keys = (BYTE*)g_q84_6d1948;
    BYTE* axes = (BYTE*)g_q84_6d1bb0;
    BYTE** k1 = &g_q84_6d1ec0;
    BYTE** k2 = &g_q84_6d1f7c;
    g_q84_6d205c = g_q84_6d2060 = F(int, s, 1280) ? 1 : 0;
    for (BYTE* p = keys + 1; p < keys + 0x24f; p += 2)
        *p = 0;
    keys[0x21] = 1;
    for (int i = 0; i < 47; i++) {
        int inv = 0;
        int v = F(int, s, 716 + i * 4);
        switch (i) {
        case 3:
            inv = 1;
        case 0:
            if (v <= 295)
                break;
            g_q84_6d2068 = (F(int, s, 728) - 300) / 4 != (F(int, s, 716) - 300) / 4;
            if (!g_q84_6d2068) {
                int a = (v - 300) / 4;
                g_q84_6d2040 = axes + a * 12;
                int m = (v - 300) % 4;
                if (m >= 0)
                    F(int, axes, a * 12 + 8) = m & 1 ? inv ^ 1 : inv;
            } else {
                int a = (v - 300) / 4;
                int m = (v - 300) % 4;
                if (m >= 0)
                    F(int, axes, a * 12 + 8) = m & 1;
                if (i == 0)
                    g_q84_6d2040 = axes + a * 12;
                else
                    g_q84_6d203c = axes + a * 12;
            }
            break;
        case 2:
            inv = 1;
        case 1:
            if (v <= 295)
                break;
            {
                int a = (v - 300) / 4;
                g_q84_6d2044 = axes + a * 12;
                int m = (v - 300) % 4;
                if (m >= 0)
                    F(int, axes, a * 12 + 8) = m & 1 ? inv ^ 1 : inv;
            }
            break;
        }
        int w = F(int, s, 528 + i * 4);
        if (w <= 295)
            k1[i] = keys + w * 2;
        if (F(int, s, 716 + i * 4) <= 295)
            k2[i] = keys + F(int, s, 716 + i * 4) * 2;
        if (i <= 27 && !(i == 5 || i == 6 || (i >= 15 && i <= 17) || i == 20 || i == 22 || i == 23)) {
            if (k1[i])
                k1[i][1] = 1;
            if (k2[i])
                k2[i][1] = 1;
        }
    }
}

// ---------------------------------------------------------------------------
// Scene file names of the game.

ASM_VAR(BYTE*, g_q85_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_q85_6cec98, $L_6cec98)
ASM_VAR(int, g_q85_6ced70, $L_6ced70)
// [$L_5125b0] the files of scene name: the folder of s (+648) with ENV\, or in a race (6cec6c +1316) MENV\, MTR\
// or MTB\ by its kind (6cec98 +384); name.b3d; the resources name.res, name1.res or name2.res by the detail level
extern "C" void __fastcall Game_SceneFiles(BYTE* s, void*, const char* name, char* dir, char* b3d, char* res)
{
    strcpy(dir, F(const char*, s, 648));
    const char* sub;
    if (!F(int, g_q85_6cec6c, 1316))
        sub = "ENV\\";
    else if (F(int, g_q85_6cec98, 384) == 0)
        sub = "MENV\\";
    else if (F(int, g_q85_6cec98, 384) == 1)
        sub = "MTR\\";
    else
        sub = "MTB\\";
    strcat(dir, sub);
    strcpy(b3d, name);
    strcat(b3d, ".b3d");
    strcpy(res, name);
    switch (g_q85_6ced70) {
    case 4:
    case 3: strcat(res, ".res"); break;
    case 2:
    case 1: strcat(res, "1.res"); break;
    case 0: strcat(res, "2.res"); break;
    }
}

// ---------------------------------------------------------------------------
// The statistics screen.

ASM_PROC(FUN_574950)
extern "C" void FUN_43fc40(); KING_ALIAS("_FUN_43fc40", "_Ai_GetPlayerData")
ASM_VAR(int, g_q86_6ced14, $L_6ced14)
ASM_VAR(int, g_q86_6ced18, $L_6ced18)
ASM_VAR(BYTE*, g_q86_6ced24, $L_6ced24)
// [$L_521e00] draws the screen at a: the player's money and the race money (+312), the player's name, the 20
// counters of +316 in three columns (the 247 one always) and the 8 of +396 in two
extern "C" void __fastcall Stats_Draw(BYTE* s, void*, int a)
{
    char buf[32];
    BYTE info[192];
    BYTE* dst = F(BYTE*, s, -52);
    CallThis(FUN_5c0020, s, a);
    CallC(FUN_574950, a, g_q86_6ced14 - 362, g_q86_6ced18 - 233, 1);
    int id;
    CallC(FUN_568f60, &id);
    sprintf(buf, "$%d", (int)CallC<double>(FUN_43f760, id));
    CallC(FUN_5ea170, dst, g_q86_6ced14 - 140, g_q86_6ced18 + 764, g_q86_6ced24, buf, 5, 0);
    sprintf(buf, "$%d", F(int, s, 312));
    CallC(FUN_5ea170, F(BYTE*, s, -52), g_q86_6ced14 + 40, g_q86_6ced18 + 764, g_q86_6ced24, buf, 5, 0);
    CallC(FUN_568f60, &id);
    CallC(FUN_43fc40, id, info);
    CallC(FUN_5ea170, F(BYTE*, s, -52), g_q86_6ced14 - 300, g_q86_6ced18 + 478, g_q86_6ced24,
          F(const char*, info, 20), 5, 0);
    int* p = (int*)(s + 316);
    for (int k = -114; k < 266; k += 19, p++) {
        int x = 0, y = k + 133;
        if (k >= 133) {
            x = 362;
            y = k - 133;
        } else if (k >= 0) {
            x = 181;
            y = k;
        }
        if (*p > 0 || k == 247) {
            sprintf(buf, "%4d", *p);
            CallC(FUN_5ea170, F(BYTE*, s, -52), g_q86_6ced14 + x - 221, g_q86_6ced18 + y + 612, g_q86_6ced24, buf,
                  5, 0);
        }
    }
    int* q = (int*)(s + 396);
    for (int i = 0; i < 8; i++)
        if (q[i] > 0) {
            sprintf(buf, "%4d", q[i]);
            CallC(FUN_5ea170, F(BYTE*, s, -52), g_q86_6ced14 + 181 * (i / 4) - 40, g_q86_6ced18 + 19 * (i % 4) + 499,
                  g_q86_6ced24, buf, 5, 0);
        }
}

// ---------------------------------------------------------------------------
// The saved games screen.

ASM_VAR(BYTE*, g_q87_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_q87_6cec80, $L_6cec80)
ASM_VAR(AsmLabel, s_q87_671204, $L_671204)
ASM_VAR(AsmLabel, g_q87_68d1c0, $L_68d1c0)
static const char* SaveList_Chosen(BYTE* s)
{
    BYTE* e = F(BYTE*, F(BYTE*, s, 316), 300);
    return e ? F(const char*, e, 12) : 0;
}
// [$L_5189f0] commands: 145 loads the chosen game (6cec6c +1300 = 2), 1 and 2 close the screen (2 restoring
// +1300 from +304), 146/147 set +1300 to 1/0, 149 takes the chosen name into +320, 281 deletes the chosen game
// (its .tr and the .mvp file) and takes the next one's name
extern "C" void __fastcall SaveList_Command(BYTE* s, void*, int cmd)
{
    const char* name;
    switch (cmd) {
    case 145:
        if (SaveList_Chosen(s)) {
            F(int, g_q87_6cec6c, 1300) = 2;
            CallC(FUN_510bc0, 0, 1);
        }
        break;
    case 2:
    case 1:
        CallVirt(s - 64, 32);
        F(int, g_q87_6cec80, 372) = 0;
        CallVirt(g_q87_6cec80, 36);
        if (cmd == 2)
            F(int, g_q87_6cec6c, 1300) = F(int, s, 304);
        break;
    case 146:
        F(int, g_q87_6cec6c, 1300) = 1;
        break;
    case 147:
        F(int, g_q87_6cec6c, 1300) = 0;
        break;
    case 149:
        if ((name = SaveList_Chosen(s)) != 0)
            strcpy((char*)s + 320, name);
        break;
    case 281: {
        if ((name = SaveList_Chosen(s)) == 0)
            break;
        char copy[64], path[64];
        strcpy(copy, name);
        CallThis(FUN_5c0d60, F(BYTE*, s, 316), name);
        const char* t = CallC<const char*>(FUN_50bbf0, copy);
        strcpy(path, (const char*)s_q87_671204);
        strcat(path, t);
        strcat(path, ".tr");
        DeleteFileA(path);
        strcpy(path + strlen(path) - 2, ".mvp");
        DeleteFileA(path);
        name = SaveList_Chosen(s);
        strcpy((char*)s + 320, name ? name : (const char*)g_q87_68d1c0);
        break;
    }
    }
}

// ---------------------------------------------------------------------------
// Switching the game screen's mode.

ASM_PROC(FUN_574530)
extern "C" void FUN_5bac90(); KING_ALIAS("_FUN_5bac90", "@Texture_ReleaseSurface@8")
extern "C" void FUN_5bdba0(); KING_ALIAS("_FUN_5bdba0", "@Ctl_Select@12")
ASM_VAR(BYTE*, g_q88_6ce3fc, $L_6ce3fc)
ASM_VAR(AsmLabel, g_q88_6ce3e0, $L_6ce3e0)
ASM_VAR(BYTE*, g_q88_696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_q88_6cec78, $L_6cec78)
ASM_VAR(BYTE*, g_q88_6cec74, $L_6cec74)
ASM_VAR(BYTE*, g_q88_6ceca0, $L_6ceca0)
ASM_VAR(BYTE*, g_q88_6cecbc, $L_6cecbc)
ASM_VAR(BYTE*, g_q88_6cecb0, $L_6cecb0)
ASM_VAR(BYTE*, g_q88_6cecac, $L_6cecac)
ASM_VAR(BYTE*, g_q88_6cecb4, $L_6cecb4)
ASM_VAR(BYTE*, g_q88_696cb0, $L_696cb0)
ASM_VAR(BYTE*, g_q88_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_q88_6cec98, $L_6cec98)
ASM_VAR(int, g_q88_67b75c, $L_67b75c)
ASM_VAR(AsmLabel, g_q88_6f68b0, $L_6f68b0)
ASM_VAR(AsmLabel, s_q88_6779a4, $L_6779a4)
ASM_VAR(AsmLabel, s_q88_677994, $L_677994)
// sets the shown state (+40) of the screen's switch buttons: a for +424..+432 and +440, !a for +416, +420, +436
static void Gs88_Buttons(BYTE* s, int a)
{
    F(int, F(BYTE*, s, 432), 40) = a;
    F(int, F(BYTE*, s, 428), 40) = a;
    F(int, F(BYTE*, s, 424), 40) = a;
    F(int, F(BYTE*, s, 440), 40) = a;
    F(int, F(BYTE*, s, 420), 40) = !a;
    F(int, F(BYTE*, s, 416), 40) = !a;
    F(int, F(BYTE*, s, 436), 40) = !a;
}
// [FUN_51f380] switches to mode m (0, 1, 2, 5: the backgrounds +368, +372, +376, +396), resets the styles and
// the windows, stops sound and force feedback and starts the mode's music; in a race the background follows the
// race kind (67b75c, 6cec98 +384); the background (+400) is bound and marked
extern "C" void __fastcall Shell_SetMode(BYTE* s, void*, int m)
{
    if (F(BYTE*, s, 400)) {
        CallThis(FUN_5bac90, F(BYTE*, s, 400));
        F(unsigned, F(BYTE*, s, 400), 52) &= ~1u;
    }
    int bg = -1;
    switch (m) {
    case 0: bg = 368; break;
    case 1: bg = 372; break;
    case 2: bg = 376; break;
    case 5: bg = 396; break;
    }
    if (bg >= 0) {
        g_q88_6ce3fc = F(BYTE*, s, bg);
        F(BYTE*, s, 400) = F(BYTE*, s, bg);
        Gs88_Buttons(s, m != 2);
        CallThis(FUN_5bdba0, F(BYTE*, s, 352), F(BYTE*, s, m == 2 ? 440 : 436));
    }
    F(int, s, 404) = m;
    BYTE* e = F(BYTE*, s, 352);
    CallThis(FUN_5bd5a0, e + 68, (BYTE*)g_q88_6ce3e0);
    CallThis(FUN_5bd5a0, e + 104, (BYTE*)g_q88_6ce3e0);
    CallThis(FUN_5bd5a0, e + 140, (BYTE*)g_q88_6ce3e0);
    CallThis(FUN_5bd5a0, e + 176, (BYTE*)g_q88_6ce3e0);
    CallThis(FUN_5bdba0, F(BYTE*, s, 436), F(BYTE*, s, 420));
    CallThis(FUN_5bdba0, F(BYTE*, s, 440), F(BYTE*, s, 432));
    CallThis(FUN_5d8610, s);
    CallVirt(F(BYTE*, g_q88_696ca0, 2856), 32);
    CallVirt(g_q88_6cec78, 32);
    CallVirt(g_q88_6cec74, 32);
    CallVirt(g_q88_6ceca0, 32);
    CallVirt(g_q88_696ca0, 32);
    CallVirt(g_q88_6cecbc, 32);
    CallVirt(g_q88_6cecb0, 32);
    CallVirt(g_q88_6cecac, 32);
    CallVirt(g_q88_6cecb4, 32);
    CallThis(FUN_598d80, (BYTE*)g_q88_6f68b0);
    CallVirt(g_q88_696cb0, 32);
    CallC(FUN_5fd160);
    CallC(FUN_5fd3e0);
    CallC(FUN_4e12b0, 0);
    if (F(int, g_q88_6cec6c, 1344))
        CallC(FUN_5ffb20);
    F(double, s, 408) = CallC<double>(FUN_6037e0);
    int mode = F(int, s, 404);
    if (F(int, g_q88_6cec6c, 1316) && mode && mode != 5) {
        CallC(FUN_574530);
        if (g_q88_67b75c == 1) {
            CallThis(FUN_5fdaf0, CallC<BYTE*>(FUN_5ef3d0, (const char*)s_q88_6779a4), 1.0f, 1.0f);
            F(BYTE*, s, 400) = F(BYTE*, s, F(int, g_q88_6cec98, 384) < 2 ? 380 : 388);
        } else if (g_q88_67b75c == 0) {
            CallThis(FUN_5fdaf0, CallC<BYTE*>(FUN_5ef3d0, (const char*)s_q88_677994), 1.0f, 1.0f);
            F(BYTE*, s, 400) = F(BYTE*, s, F(int, g_q88_6cec98, 384) < 2 ? 384 : 392);
        }
    } else
        CallThis(FUN_5fdaf0, CallC<BYTE*>(FUN_5ef3d0, (const char*)(mode == 2 ? s_q88_6779a4 : s_q88_677994)), 1.0f,
                 1.0f);
    if (F(BYTE*, s, 400)) {
        CallThis(FUN_5bae30, F(BYTE*, s, 400), 3);
        F(unsigned, F(BYTE*, s, 400), 52) |= 1;
    }
}

// ---------------------------------------------------------------------------
// Starting a game.

extern "C" void FUN_512ea0(); KING_ALIAS("_FUN_512ea0", "_Shell_SetVideoMode")
extern "C" void FUN_5b6bf0(); KING_ALIAS("_FUN_5b6bf0", "@Surface_Prepare@4")
extern "C" void FUN_510a10(); KING_ALIAS("_FUN_510a10", "_Shell_LoadScene")
extern "C" void FUN_603b70(); KING_ALIAS("_FUN_603b70", "_Ini_ReadInt")
extern "C" void FUN_5d2e70(); KING_ALIAS("_FUN_5d2e70", "_Sys_Fatal")
extern "C" void FUN_5b6e00(); KING_ALIAS("_FUN_5b6e00", "@Surface_Clear@16")
extern "C" void FUN_5d88c0(); KING_ALIAS("_FUN_5d88c0", "@Window_Refresh@8")
extern "C" void FUN_5105b0(); KING_ALIAS("_FUN_5105b0", "_Shell_SetupModeObjects")
extern "C" void FUN_568fd0(); KING_ALIAS("_FUN_568fd0", "_Game_568fd0")
extern "C" void FUN_5dba30(); KING_ALIAS("_FUN_5dba30", "@CGameApp_DestroyWindowNow@12")
extern "C" void FUN_522cf0(); KING_ALIAS("_FUN_522cf0", "@ShellDialog522cf0_Construct@12")
extern "C" void FUN_5e41f0(); KING_ALIAS("_FUN_5e41f0", "@Windows_Add2@12")
extern "C" void __fastcall Shell_ShowWithMusic(BYTE* s, void*, int m);
extern "C" void __cdecl Shell_EnterWorld(int movie);
ASM_VAR(int, g_q89_696868, $L_696868)
ASM_VAR(BYTE*, g_q89_6cec70, $L_6cec70)
ASM_VAR(BYTE*, g_q89_6cecd4, $L_6cecd4)
ASM_VAR(const char*, g_q89_6ced68, $L_6ced68)
ASM_VAR(char, g_q89_6ceb5c, $L_6ceb5c)
ASM_VAR(char, g_q89_6f2d18, $L_6f2d18)
ASM_VAR(BYTE*, g_q89_696ca0, $L_696ca0)
ASM_VAR(int, g_q89_6cec68, $L_6cec68)
ASM_VAR(BYTE*, g_q89_69684c, $L_69684c)
ASM_VAR(BYTE*, g_q89_6cec80, $L_6cec80)
ASM_VAR(void*, g_q89_6f2ed0, $L_6f2ed0)
ASM_VAR(void*, g_q89_6f2ed4, $L_6f2ed4)
ASM_VAR(void*, g_q89_6f2ed8, $L_6f2ed8)
ASM_VAR(void**, g_q89_6cec64, $L_6cec64)
ASM_VAR(BYTE*, g_q89_6cecb8, $L_6cecb8)
ASM_VAR(char, g_q89_696750, $L_696750)
ASM_VAR(AsmLabel, s_q89_672998, $L_672998)
ASM_VAR(AsmLabel, s_q89_676798, $L_676798)
ASM_VAR(AsmLabel, s_q89_67677c, $L_67677c)
ASM_VAR(AsmLabel, s_q89_676770, $L_676770)
ASM_VAR(AsmLabel, s_q89_676778, $L_676778)
ASM_VAR(AsmLabel, s_q89_67675c, $L_67675c)
ASM_VAR(AsmLabel, s_q89_676748, $L_676748)
ASM_VAR(AsmLabel, s_q89_676734, $L_676734)
ASM_VAR(AsmLabel, s_q89_676720, $L_676720)
ASM_VAR(AsmLabel, s_q89_67670c, $L_67670c)
ASM_VAR(AsmLabel, s_q89_6766f8, $L_6766f8)
ASM_VAR(AsmLabel, s_q89_6766e4, $L_6766e4)
ASM_VAR(AsmLabel, s_q89_6766d0, $L_6766d0)
static bool Gs89_HighMode(int intro) { return (BYTE)(F(int, g_q89_6cec70, 408) & 240) == 64 && intro; }
// [FUN_510bc0..$L_510d02] starts game name (0 for a new one): in the 1600x1200 mode with the intro set
// (ini 676798) and ask set the intro screen (6cecd4) shows first; else the name goes to 6f2d18, the world view is
// set up (fatal without one), the screen cleared, the mode's objects and the player's sounds found; with the
// intro the intro dialog opens, else the world is entered
extern "C" void __cdecl Shell_StartGame(const char* name, int ask)
{
    int intro = 1;
    CallC(FUN_603b70, (const char*)s_q89_672998, (const char*)s_q89_676798, 1, &intro, g_q89_696868);
    const char* n;
    if (Gs89_HighMode(intro)) {
        if (ask) {
            if (!name)
                g_q89_6ced68 = 0;
            else {
                g_q89_6ced68 = &g_q89_6ceb5c;
                strcpy(&g_q89_6ceb5c, name);
            }
            Shell_ShowWithMusic(g_q89_6cecd4, 0, 0);
            return;
        }
        n = g_q89_6ced68;
    } else {
        n = name;
        g_q89_6ced68 = name;
    }
    if (!n)
        g_q89_6f2d18 = 0;
    else
        strcpy(&g_q89_6f2d18, n);
    CallC(FUN_512ea0, g_q89_696ca0, 1);
    if (!g_q89_6cec68)
        CallC(FUN_5d2e70, (const char*)s_q89_67677c, 0);
    F(int, g_q89_6cec70, 416) = Mode_TextureLevel();
    F(int, g_q89_696ca0, 536) = 0;
    CallThis(FUN_5b6bf0, F(BYTE*, g_q89_69684c, 12));
    CallThis(FUN_5b6e00, F(BYTE*, g_q89_69684c, 12), 0xff000000, 1);
    CallThis(FUN_5d88c0, g_q89_69684c);
    CallThis(FUN_5b6e00, F(BYTE*, g_q89_69684c, 12), 0xff000000, 1);
    CallThis(FUN_5d88c0, g_q89_69684c);
    CallC(FUN_510a10, (const char*)s_q89_676770, (const char*)s_q89_676778);
    if (BYTE* o = F(BYTE*, g_q89_6cec80, 372))
        CallVirt(o, 32);
    F(BYTE*, g_q89_6cec80, 372) = 0;
    CallVirt(g_q89_6cec80, 32);
    CallC(FUN_5105b0);
    if (CallC<int>(FUN_568fd0) == 2) {
        g_q89_6f2ed0 = CallC<void*>(FUN_5ef3d0, (const char*)s_q89_67675c);
        g_q89_6f2ed4 = CallC<void*>(FUN_5ef3d0, (const char*)s_q89_676748);
        g_q89_6f2ed8 = CallC<void*>(FUN_5ef3d0, (const char*)s_q89_676734);
    } else {
        g_q89_6f2ed0 = CallC<void*>(FUN_5ef3d0, (const char*)s_q89_676720);
        g_q89_6f2ed4 = CallC<void*>(FUN_5ef3d0, (const char*)s_q89_67670c);
        g_q89_6f2ed8 = CallC<void*>(FUN_5ef3d0, (const char*)s_q89_6766f8);
    }
    if (g_q89_6cec64)
        operator delete(g_q89_6cec64);
    void** p = (void**)operator new(12);
    if (p) {
        p[0] = CallC<void*>(FUN_5ef3d0,
                            (const char*)(CallC<int>(FUN_568fd0) == 2 ? s_q89_6766e4 : s_q89_6766d0));
        p[1] = 0;
        p[2] = 0;
    }
    g_q89_6cec64 = p;
    if (Gs89_HighMode(intro)) {
        if (g_q89_6cecb8)
            CallThis(FUN_5dba30, &g_q89_696750, g_q89_6cecb8);
        BYTE* q = (BYTE*)operator new(384);
        q = q ? CallThis<BYTE*>(FUN_522cf0, q, g_q89_69684c) : 0;
        g_q89_6cecb8 = q;
        CallThis(FUN_5e41f0, &g_q89_696750, q);
        CallVirt(g_q89_6cecb8, 36);
        return;
    }
    Shell_EnterWorld(0);
}

// ---------------------------------------------------------------------------
// Loading a shell scene.

ASM_PROC(FUN_5ec0f0)
extern "C" void FUN_5ea880(); KING_ALIAS("_FUN_5ea880", "@Cluster5ea880_Construct@12")
extern "C" void FUN_5bc060(); KING_ALIAS("_FUN_5bc060", "@Image_RleToSurface@8")
extern "C" void FUN_5db6e0(); KING_ALIAS("_FUN_5db6e0", "@Scenes_Add@12")
extern "C" char* __fastcall App_Path(BYTE* s, void*, const char* name);
ASM_VAR(BYTE*, g_q90_696ca0, $L_696ca0)
// [FUN_510a10..$L_510b36] loads scene name from folder dir: its resources name15.res or name16.res (by the
// colour depth; fatal when missing), the marked images and textures prepared, the scene added to the world
extern "C" void __cdecl Shell_LoadScene(const char* dir, const char* name)
{
    BYTE* c = (BYTE*)operator new(180);
    c = c ? CallThis<BYTE*>(FUN_5ea880, c, name) : 0;
    char path[260];
    strcpy(path, App_Path(g_q90_696ca0, 0, dir));
    strcat(path, name);
    strcat(path, CallC<int>(FUN_568fd0) == 2 ? "15.res" : "16.res");
    FILE* f = fopen(path, "rb");
    if (!f)
        CallC(FUN_5d2e70, name, " not found");
    CallThis(FUN_5ec0f0, c, dir, f, 2, path);
    for (int i = 0; i < F(int, c, 104); i++) {
        BYTE* t = F(BYTE**, c, 108)[i];
        if (F(BYTE, t, 28) & 1)
            CallThis(FUN_5bc060, t);
    }
    for (int i = 0; i < F(int, c, 80); i++) {
        BYTE* t = F(BYTE**, c, 84)[i];
        if (F(BYTE, t, 52) & 1)
            CallThis(FUN_5bae30, t, 3);
    }
    CallThis(FUN_5db6e0, g_q90_696ca0, c);
}

// ---------------------------------------------------------------------------
// Setting up the game's video mode.

extern "C" void FUN_5d71f0(); KING_ALIAS("_FUN_5d71f0", "_Dd_FreeLocalVideoMemory")
extern "C" void FUN_5e8c60(); KING_ALIAS("_FUN_5e8c60", "_Font_LoadFile")
extern "C" void FUN_4cce70(); KING_ALIAS("_FUN_4cce70", "@Lights_Construct@16")
ASM_VAR(BYTE*, g_q91_6cecf4, $L_6cecf4)
ASM_VAR(BYTE*, g_q91_6cecf8, $L_6cecf8)
ASM_VAR(BYTE*, g_q91_6ced24, $L_6ced24)
ASM_VAR(BYTE*, g_q91_6ced2c, $L_6ced2c)
ASM_VAR(BYTE*, g_q91_6ced30, $L_6ced30)
ASM_VAR(BYTE*, g_q91_6cec70, $L_6cec70)
ASM_VAR(unsigned, g_q91_702e8c, $L_702e8c)
ASM_VAR(int, g_q91_696878, $L_696878)
ASM_VAR(BYTE*, g_q91_69684c, $L_69684c)
ASM_VAR(int, g_q91_6cec68, $L_6cec68)
ASM_VAR(int, g_q91_6ced70, $L_6ced70)
ASM_VAR(DWORD, g_q91_69685c, $L_69685c)
ASM_VAR(AsmLabel, g_q91_696750, $L_696750)
ASM_VAR(int, g_q91_7208e0, $L_7208e0)
ASM_VAR(int, g_q91_7208bc, $L_7208bc)
ASM_VAR(int, g_q91_72090c, $L_72090c)
ASM_VAR(int, g_q91_7208d8, $L_7208d8)
ASM_VAR(int, g_q91_720908, $L_720908)
ASM_VAR(void*, g_q91_6ce520, $L_6ce520)
ASM_VAR(AsmLabel, s_q91_676918, $L_676918)
ASM_VAR(AsmLabel, s_q91_67690c, $L_67690c)
ASM_VAR(AsmLabel, s_q91_676900, $L_676900)
ASM_VAR(AsmLabel, s_q91_6768f4, $L_6768f4)
static void Gs91_ClearTwice(BYTE* w)
{
    CallThis(FUN_5b6bf0, F(BYTE*, w, 12));
    CallThis(FUN_5b6e00, F(BYTE*, w, 12), 0, 1);
    CallThis(FUN_5d88c0, w);
    CallThis(FUN_5b6e00, F(BYTE*, w, 12), 0, 1);
    CallThis(FUN_5d88c0, w);
}
// [FUN_512ea0..$L_513044] sets the game's video mode (6cec70 +408: the size in bits 4-8, 2 = 16-bit colour),
// picks the texture detail (6ced70) and texture mode (6cec68) by the video memory, at most 1 below 60 MB of RAM;
// loads the three fonts, sets the view and creates the lights
extern "C" void __cdecl Shell_SetVideoMode(BYTE* world, int)
{
    if (g_q91_6cecf4)
        CallVirt(g_q91_6cecf4, 36);
    g_q91_6cecf8 = g_q91_6ced24;
    if (F(int, g_q91_6cec70, 408) & 0x100)
        SetCursor(0);
    F(unsigned, world, 3020) |= 0x4000;
    g_q91_702e8c |= 0x4000;
    g_q91_696878 = -1;
    BYTE* w = g_q91_69684c;
    Gs91_ClearTwice(w);
    g_q91_6cec68 = 1;
    int m = F(int, g_q91_6cec70, 408);
    int depth = (BYTE)(m & 15) == 2 ? 2 : 1;
    int sx = 0, sy = 0;
    switch (m & 496) {
    case 320: sx = 1600; sy = 1200; break;
    case 304: sx = 1024; sy = 768; break;
    case 288: sx = 800; sy = 600; break;
    case 272: sx = 640; sy = 480; break;
    }
    if (sx)
        CallThis($L_5dd8c0, (BYTE*)g_q91_696750, sx, sy, depth,
                 (g_q91_69685c | CallC<DWORD>(FUN_50bd80)) | 0x4000);
    int vm = g_q91_7208e0;
    if (vm < 0x200000) {
        g_q91_6cec68 = 0;
        CallC(FUN_5d2e70, (const char*)s_q91_676918, 0);
    } else if (vm < 0x400000) {
        g_q91_6cec68 = 1;
        g_q91_6ced70 = 0;
    } else if (vm >= 0x800000 && g_q91_7208bc) {
        g_q91_6cec68 = 3;
        if (CallC<int>(FUN_5d71f0, g_q91_72090c) < 0x200000)
            g_q91_6ced70 = 0;
        else if (CallC<int>(FUN_5d71f0, g_q91_72090c) < 0x400000)
            g_q91_6ced70 = 1;
        else if (CallC<int>(FUN_5d71f0, g_q91_72090c) < 0x1000000)
            g_q91_6ced70 = 2;
        else
            g_q91_6ced70 = 3 + (CallC<int>(FUN_5d71f0, g_q91_72090c) >= 0x2000000);
    } else {
        g_q91_6cec68 = 2;
        g_q91_6ced70 = CallC<int>(FUN_5d71f0, g_q91_72090c) >= 0x200000;
    }
    MEMORYSTATUS ms;
    GlobalMemoryStatus(&ms);
    if (g_q91_6ced70 > 1 && ms.dwTotalPhys < 62914560)
        g_q91_6ced70 = 1;
    if (F(int, g_q91_6cec70, 408) & 0x100)
        SetCursor(0);
    CallC(FUN_5d8460, 2, -1);
    g_q91_6ced24 = CallC<BYTE*>(FUN_5e8c60, (const char*)s_q91_67690c);
    g_q91_6ced2c = CallC<BYTE*>(FUN_5e8c60, (const char*)s_q91_676900);
    g_q91_6ced30 = CallC<BYTE*>(FUN_5e8c60, (const char*)s_q91_6768f4);
    CallC(FUN_527ea0, 0, 0);
    Gs91_ClearTwice(w);
    int n = (g_q91_7208d8 ? F(short, g_q91_6cec70, 420) : F(short, g_q91_6cec70, 422)) * 4 / 100 + 4;
    void* p = operator new(16);
    g_q91_6ce520 = p ? CallThis<void*>(FUN_4cce70, p, n, g_q91_720908) : 0;
}

// ---------------------------------------------------------------------------
// Loading the world.

ASM_PROC(FUN_50f990)
extern "C" void FUN_510690(); KING_ALIAS("_FUN_510690", "_Loading_Step")
extern "C" void FUN_5a5720(); KING_ALIAS("_FUN_5a5720", "_Set7025b0")
extern "C" void FUN_5107c0(); KING_ALIAS("_FUN_5107c0", "_Model_Load")
extern "C" void FUN_5d27c0(); KING_ALIAS("_FUN_5d27c0", "@NameHash_ResolvePending@8")
extern "C" void FUN_435730(); KING_ALIAS("_FUN_435730", "_Ai_Initialize")
extern "C" void FUN_5b64e0(); KING_ALIAS("_FUN_5b64e0", "@Window_SetSurface@12")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")
extern "C" void FUN_512720(); KING_ALIAS("_FUN_512720", "_Replay_Open")
extern "C" void __cdecl Shell_LoadScene(const char* dir, const char* name);
ASM_VAR(BYTE*, g_q92_696ca0, $L_696ca0)
ASM_VAR(int, g_q92_6ced70, $L_6ced70)
ASM_VAR(int, g_q92_702e98, $L_702e98)
ASM_VAR(unsigned, g_q92_702e8c, $L_702e8c)
ASM_VAR(BYTE*, g_q92_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_q92_6cec70, $L_6cec70)
ASM_VAR(BYTE*, g_q92_6cec98, $L_6cec98)
ASM_VAR(BYTE*, g_q92_6cec84, $L_6cec84)
ASM_VAR(BYTE*, g_q92_69684c, $L_69684c)
ASM_VAR(int, g_q92_7208c0, $L_7208c0)
ASM_VAR(int, g_q92_67b738, $L_67b738)
ASM_VAR(AsmLabel, g_q92_71e820, $L_71e820)
ASM_VAR(AsmLabel, g_q92_696750, $L_696750)
ASM_VAR(AsmLabel, g_q92_4d76b0, $L_4d76b0)
ASM_VAR(AsmLabel, g_q92_4d8b60, $L_4d8b60)
ASM_VAR(AsmLabel, s_q92_676770, $L_676770)
ASM_VAR(AsmLabel, s_q92_672688, $L_672688)
ASM_VAR(AsmLabel, s_q92_6768f0, $L_6768f0)
ASM_VAR(AsmLabel, s_q92_6768e8, $L_6768e8)
ASM_VAR(AsmLabel, s_q92_66cd44, $L_66cd44)
ASM_VAR(AsmLabel, s_q92_6768e0, $L_6768e0)
ASM_VAR(AsmLabel, s_q92_6768d8, $L_6768d8)
ASM_VAR(AsmLabel, s_q92_6768d0, $L_6768d0)
ASM_VAR(AsmLabel, s_q92_67262c, $L_67262c)
ASM_VAR(AsmLabel, s_q92_6768c4, $L_6768c4)
ASM_VAR(AsmLabel, s_q92_6768b8, $L_6768b8)
ASM_VAR(AsmLabel, s_q92_6768a8, $L_6768a8)
// [FUN_512920..$L_5129ca] loads the world with the loading bar: the common models, the escape screen, cabins and
// the truck parts (fatal on a missing name); the AI of the map (ENV, or MENV/MTR in a race) or the network game
// screen; the geometry and texture size switches by the view distance and detail level; then the replay
extern "C" void __cdecl Shell_LoadWorld(void)
{
    F(void*, g_q92_696ca0, 536) = (void*)FUN_510690;
    CallC(FUN_5a5720, 0);
    g_q92_702e98 = g_q92_6ced70 == 4 || g_q92_6ced70 == 3;
    const char* dir = (const char*)s_q92_676770;
    CallC(FUN_5107c0, dir, (const char*)s_q92_672688);
    Shell_LoadScene(dir, (const char*)s_q92_6768f0);
    F(int, CallC<BYTE*>(FUN_5107c0, dir, (const char*)s_q92_6768e8), 56) = 0;
    F(int, CallC<BYTE*>(FUN_5107c0, dir, (const char*)s_q92_66cd44), 56) = 0;
    if (const char* bad = CallThis<const char*>(FUN_5d27c0, (BYTE*)g_q92_71e820))
        CallC(FUN_5d2e70, bad, 0);
    F(BYTE*, g_q92_6cec6c, 1356) = g_q92_696ca0;
    if (F(int, g_q92_6cec6c, 1300) <= 1) {
        const char* map = 0;
        if (!F(int, g_q92_6cec6c, 1316))
            map = (const char*)s_q92_6768e0;
        else if (F(int, g_q92_6cec98, 384) == 0)
            map = (const char*)s_q92_6768d8;
        else if (F(int, g_q92_6cec98, 384) == 1)
            map = (const char*)s_q92_6768d0;
        if (map)
            CallC(FUN_435730, App_Path(g_q92_696ca0, 0, map), (BYTE*)g_q92_4d76b0, (BYTE*)g_q92_4d8b60);
    } else {
        BYTE* q = (BYTE*)operator new(424);
        q = q ? CallThis<BYTE*>(FUN_50f990, q) : 0;
        g_q92_6cec84 = q;
        CallThis(FUN_5e41f0, (BYTE*)g_q92_696750, q);
        CallVirt(g_q92_6cec84, 36);
    }
    F(void*, g_q92_696ca0, 536) = 0;
    CallThis(FUN_5b64e0, F(BYTE*, g_q92_69684c, 12), F(DWORD, F(BYTE*, F(BYTE*, g_q92_696ca0, 468), 100), 0));
    F(int, F(BYTE*, g_q92_696ca0, 472), 112) = F(short, g_q92_6cec70, 422);
    void* geo = CallC<void*>(FUN_5ef2b0, 0, (const char*)s_q92_67262c);
    int g = 2 - F(short, g_q92_6cec70, 422) / 35;
    CallC(FUN_4e480c, geo, g);
    if (BYTE* o = CallC<BYTE*>(FUN_5ef3d0, (const char*)(g == 2 ? s_q92_6768c4 : s_q92_6768b8)))
        CallVirt(o, 40);
    short v = F(short, g_q92_6cec70, 420);
    if (v < 30)
        g_q92_7208c0 = 0;
    if (v < 60) {
        F(unsigned, g_q92_696ca0, 3020) |= 0x2000000;
        g_q92_702e8c |= 0x2000000;
    } else {
        F(unsigned, g_q92_696ca0, 3020) &= 0xfdffffff;
        g_q92_702e8c &= 0xfdffffff;
    }
    void* tex = CallC<void*>(FUN_5ef2b0, 0, (const char*)s_q92_6768a8);
    switch (g_q92_6ced70) {
    case 4: g_q92_67b738 = 460800; CallC(FUN_5e1a10, tex, 0); break;
    case 3: g_q92_67b738 = 163840; CallC(FUN_5e1a10, tex, 0); break;
    case 2: g_q92_67b738 = 163840; CallC(FUN_5e1a10, tex, 1); break;
    case 1: g_q92_67b738 = 122880; CallC(FUN_5e1a10, tex, 1); break;
    case 0: g_q92_7208c0 = 0; g_q92_67b738 = 71680; CallC(FUN_5e1a10, tex, 2); break;
    }
    CallC(FUN_512720);
}

// ---------------------------------------------------------------------------
// The default settings for this machine.

extern "C" void FUN_513740(); KING_ALIAS("_FUN_513740", "_Cpu_SpeedFactor")
ASM_VAR(int, g_q120_696868, $L_696868)
ASM_VAR(BYTE*, g_q120_6cec70, $L_6cec70)
ASM_VAR(BYTE*, g_q120_6cec98, $L_6cec98)
ASM_VAR(BYTE*, g_q120_6cec9c, $L_6cec9c)
ASM_VAR(double, g_q120_64b6a0, $L_64b6a0)
ASM_VAR(double, g_q120_64deb0, $L_64deb0)
ASM_VAR(double, g_q120_64dea8, $L_64dea8)
ASM_VAR(AsmLabel, s_q120_672998, $L_672998)
// ln x the way the original computes it (fldln2, fyl2x)
static __declspec(noinline) double Gs120_Ln(double x) { __asm { fldln2 } __asm { fld x } __asm { fyl2x } }
static void Gs120_Write(const char* key, int v)
{
    CallC(FUN_603c70, (const char*)s_q120_672998, key, v, g_q120_696868);
}
static void Gs120_Mark(BYTE* c, bool on)
{
    if (!c)
        return;
    if (on)
        F(unsigned, c, 284) |= 2;
    else
        F(unsigned, c, 284) &= ~2u;
}
// [FUN_515710] writes the defaults for this machine: all effects, the view distance by the processor speed,
// the detail by the video memory (DRV1 vmem), the traffic by both; the video mode by the usable modes (c640,
// c800, c1024: 800x600 with good detail and 1024x768 usable, else 640x480, 800x600 or 1024x768) marked on the
// options screen's mode buttons (135-138)
extern "C" void __cdecl Shell_DefaultSettings(int c640, int c800, int c1024, int)
{
    Gs120_Write("eff", 1023);
    BYTE *b138 = 0, *b137 = 0, *b136 = 0, *b135 = 0;
    if (BYTE* o = g_q120_6cec70) {
        b138 = CallThis<BYTE*>(FUN_5bdb50, o + 64, 138);
        b137 = CallThis<BYTE*>(FUN_5bdb50, g_q120_6cec70 + 64, 137);
        b136 = CallThis<BYTE*>(FUN_5bdb50, g_q120_6cec70 + 64, 136);
        b135 = CallThis<BYTE*>(FUN_5bdb50, g_q120_6cec70 + 64, 135);
    }
    int d = (int)(CallC<double>(FUN_513740) * g_q120_64b6a0);
    Gs120_Write("dtl", d);
    if (g_q120_6cec70)
        F(short, g_q120_6cec70, 422) = (short)d;
    int vm = 0;
    CallC(FUN_603b70, "DRV1", "vmem", 0, &vm, g_q120_696868);
    int q = 0;
    if (vm) {
        int mb = vm / 0x100000;
        q = (int)((Gs120_Ln((double)mb) - g_q120_64deb0) * g_q120_64dea8 * g_q120_64b6a0);
        if (q > 100)
            q = 100;
        else if (q < 0)
            q = 0;
    }
    Gs120_Write("grf", q);
    if (g_q120_6cec70)
        F(short, g_q120_6cec70, 420) = (short)q;
    int t = q * d / 100;
    if (g_q120_6cec9c)
        F(short, g_q120_6cec9c, 400) = (short)t;
    if (g_q120_6cec98)
        F(short, g_q120_6cec98, 388) = (short)t;
    Gs120_Write("trafs", t);
    Gs120_Write("trafm", t);
    int mode;
    if (q > 50 && c800 && c1024) {
        mode = 290;
        Gs120_Mark(b135, false);
        Gs120_Mark(b138, false);
        Gs120_Mark(b137, false);
        Gs120_Mark(b136, true);
    } else {
        Gs120_Mark(b135, false);
        Gs120_Mark(b138, false);
        Gs120_Mark(b137, false);
        Gs120_Mark(b136, false);
        if (c640) {
            mode = 274;
            Gs120_Mark(b135, true);
        } else if (c800) {
            mode = 290;
            Gs120_Mark(b136, true);
        } else {
            mode = 306;
            Gs120_Mark(b137, true);
        }
    }
    if (g_q120_6cec70)
        F(int, g_q120_6cec70, 408) = mode;
    Gs120_Write("video", mode);
}

// ---------------------------------------------------------------------------
// The input settings screen's commands.

extern "C" void FUN_5134b0(); KING_ALIAS("_FUN_5134b0", "@Settings_Load@8")
extern "C" void __fastcall Input_ApplyBindings(BYTE* s);
ASM_VAR(int, g_q121_6d207c, $L_6d207c)
ASM_VAR(BYTE*, g_q121_696848, $L_696848)
ASM_VAR(BYTE*, g_q121_6cec80, $L_6cec80)
ASM_VAR(BYTE*, g_q121_6cec6c, $L_6cec6c)
ASM_VAR(int, g_q121_696868, $L_696868)
ASM_VAR(AsmLabel, g_q121_6d1bb0, $L_6d1bb0)
ASM_VAR(AsmLabel, s_q121_672998, $L_672998)
ASM_VAR(AsmLabel, s_q121_6767d0, $L_6767d0)
// [$L_514a40] commands of the input screen (s is its +64 part): 142/143 the previous/next player profile (saved,
// switched, loaded, applied), 2 cancel (the profile and bindings restored), 1 OK (saved, applied, the joystick
// flag written), 33 the next control, 141 waits for a key; 1000-1046 start binding an action with the axes shown
extern "C" void __fastcall InputScreen_Command(BYTE* s, void*, int cmd)
{
    BYTE* w = s - 64;
    int idx;
    switch (cmd) {
    case 142:
        if (!F(int, s, 1216))
            return;
        CallThis(FUN_513600, w);
        idx = F(int, s, 1216) - 1;
        goto profile;
    case 143:
        if (F(int, s, 1216) >= g_q121_6d207c - 1)
            return;
        CallThis(FUN_513600, w);
        idx = F(int, s, 1216) + 1;
    profile:
        F(int, s, 1216) = idx;
        CallC(FUN_531000, idx);
        CallThis(FUN_5134b0, w);
        Input_ApplyBindings(w);
        return;
    case 2:
        if (F(int, s, 452) >= 0)
            return;
        CallVirt(w, 32);
        F(int, g_q121_6cec80, 372) = 0;
        CallVirt(g_q121_6cec80, 36);
        F(int, s, 1216) = F(int, s, 456);
        memcpy(s + 464, s + 840, 376);
        return;
    case 1:
        if (F(int, s, 452) >= 0)
            return;
        if (g_q121_696848 == w)
            CallVirt(g_q121_6cec80, 36);
        CallVirt(w, 32);
        F(int, g_q121_6cec80, 372) = 0;
        CallThis(FUN_513600, w);
        Input_ApplyBindings(w);
        CallC(FUN_603c70, (const char*)s_q121_672998, (const char*)s_q121_6767d0, F(int, g_q121_6cec6c, 1280),
              g_q121_696868);
        return;
    case 33: {
        BYTE* l = F(BYTE*, s, 288);
        BYTE* n = CallThis<BYTE*>(FUN_5d3080, F(BYTE*, l, 292));
        CallThis(FUN_5bdba0, l, n ? n : F(BYTE*, l, 288));
        return;
    }
    case 141:
        F(int, s, 452) = -1;
        CallThis(FUN_513850, w);
        return;
    }
    if (F(int, s, 452) >= 0 || cmd < 1000 || cmd >= 1047)
        return;
    F(int, s, 452) = cmd - 1000;
    const BYTE* t = (const BYTE*)g_q121_6d1bb0;
    for (int k = 0; k < 8; k++, t += 12) {
        double d = F(float, t, 0);
        F(double, s, 304 + k * 8) = d;
        F(double, s, 368 + k * 8) = d;
        if (F(int, t, 8)) {
            F(double, s, 368 + k * 8) = -d;
            F(double, s, 304 + k * 8) = -F(double, s, 304 + k * 8);
        }
    }
}

// ---------------------------------------------------------------------------
// Paying for a repair.

ASM_PROC(FUN_533590)
extern "C" void FUN_53b3c0(); KING_ALIAS("_FUN_53b3c0", "@King_53b3c0@12")
extern "C" void FUN_4eb600(); KING_ALIAS("_FUN_4eb600", "@Ramp_SetValue@12")
extern "C" void FUN_43f7c0(); KING_ALIAS("_FUN_43f7c0", "_Player_Set136")
extern "C" void FUN_56ae60(); KING_ALIAS("_FUN_56ae60", "_Messages_Add")
extern "C" void FUN_477300(); KING_ALIAS("_FUN_477300", "_Player_ClearFlags")
ASM_VAR(BYTE*, g_q122_6d2078, $L_6d2078)
ASM_VAR(BYTE*, g_q122_6cec6c, $L_6cec6c)
ASM_VAR(float, g_q122_64c250, $L_64c250)
ASM_VAR(double, g_q122_64b5b8, $L_64b5b8)
ASM_VAR(double, g_q122_64b7a8, $L_64b7a8)
ASM_VAR(double, g_q122_64d0f0, $L_64d0f0)
ASM_VAR(AsmLabel, s_q122_676838, $L_676838)
ASM_VAR(AsmLabel, s_q122_676834, $L_676834)
static int Gs122_PlayerId()
{
    int id;
    CallC(FUN_568f60, &id);
    return id;
}
// [FUN_5119e0] repairs the player's truck for cost when the money is enough: the repair (533590, special for
// two car types), the health and the ramp restored, the money taken (and a flag message cleared), the msg text
// with the cost shown outside races; else message 12 and the repair check alone
extern "C" void __cdecl Shell_PayRepair(double cost, int msg)
{
    char buf[128];
    double money = CallC<double>(FUN_43f760, Gs122_PlayerId());
    BYTE* car = g_q122_6d2078;
    double health = CallVirt<double>(F(BYTE*, car, 616), 84);
    BYTE* ramp = F(BYTE*, F(BYTE*, g_q122_6d2078, 616), 21600) + 16112;
    double high = CallThis<double>(FUN_4eb680, ramp);
    ramp = F(BYTE*, F(BYTE*, g_q122_6d2078, 616), 21600) + 16112;
    double rest = g_q122_64c250 - CallThis<double>(FUN_4eb690, ramp);
    const char* name = F(const char*, F(BYTE*, F(BYTE*, g_q122_6d2078, 616), 308), 60);
    int special = strcmp(name, (const char*)s_q122_676838) == 0 || strcmp(name, (const char*)s_q122_676834) == 0;
    if (!(cost < money)) {
        CallC(FUN_56ae60, CallC<const char*>(FUN_5d2e50, 12));
        CallThis(FUN_533590, g_q122_6d2078, special);
        return;
    }
    CallThis(FUN_533590, g_q122_6d2078, special);
    CallThis(FUN_53b3c0, g_q122_6d2078, (float)(health + g_q122_64b5b8));
    CallVirt(F(BYTE*, g_q122_6d2078, 616), 116);
    CallVirt(F(BYTE*, g_q122_6d2078, 616), 120);
    if (!(rest <= g_q122_64b7a8))
        CallThis(FUN_4eb600, F(BYTE*, F(BYTE*, g_q122_6d2078, 616), 21600) + 16112, (float)(high * g_q122_64d0f0));
    money -= cost;
    CallC(FUN_43f7c0, Gs122_PlayerId(), money);
    if (CallC<int>(FUN_43f620, Gs122_PlayerId()) & 0x2000000) {
        CallC(FUN_56ae60, CallC<const char*>(FUN_5d2e50, 119));
        CallC(FUN_477300, Gs122_PlayerId());
    }
    int c = (int)cost;
    sprintf(buf, CallC<const char*>(FUN_5d2e50, msg), c);
    if (!F(int, g_q122_6cec6c, 1316))
        CallC(FUN_56ae60, buf);
}

// ---------------------------------------------------------------------------
// The thumbnail of a saved game.

extern "C" void FUN_5b6300(); KING_ALIAS("_FUN_5b6300", "_Pixel_Blue")
extern "C" void FUN_5b62c0(); KING_ALIAS("_FUN_5b62c0", "_Pixel_Green")
extern "C" void FUN_5b6280(); KING_ALIAS("_FUN_5b6280", "_Pixel_Red")
extern "C" void FUN_5b9660(); KING_ALIAS("_FUN_5b9660", "@Palette_Nearest@20")
ASM_VAR(BYTE*, g_q123_696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_q123_7208fc, $L_7208fc)
ASM_VAR(BYTE*, g_q123_6ce4f4, $L_6ce4f4)
typedef long(__stdcall* Gs123_Write)(void*, const void*, ULONG, ULONG*);
static void Gs123_Put(void* st, const void* p, ULONG n, ULONG* done)
{
    ((Gs123_Write)(*(void***)st)[4])(st, p, n, done);
}
static int Gs123_Avg(void (*fn)(), int a, int b, int c, int d)
{
    int s = (BYTE)CallC<int>(fn, a);
    s += (BYTE)CallC<int>(fn, b);
    s += (BYTE)CallC<int>(fn, c);
    s += (BYTE)CallC<int>(fn, d);
    return s / 4;
}
// [FUN_50c3a0] writes to stream st a 100-byte header (size w x h, the local date and time) and a w x h thumbnail
// of the screen (the locked back buffer, or with fromImage the 6ce4f4 image), each pixel the palette colour
// nearest to the average of four screen pixels
extern "C" void __cdecl Shell_WriteThumbnail(void* st, int w, int h, int fromImage)
{
    BYTE hdr[100];
    memset(hdr, 0, 100);
    hdr[0] = (BYTE)w;
    hdr[1] = (BYTE)h;
    SYSTEMTIME t;
    GetLocalTime(&t);
    hdr[3] = (BYTE)t.wMonth;
    hdr[2] = (BYTE)t.wDay;
    hdr[7] = (BYTE)t.wSecond;
    hdr[6] = (BYTE)t.wMinute;
    hdr[4] = (BYTE)((int)t.wYear % 100);
    hdr[5] = (BYTE)t.wHour;
    ULONG done;
    Gs123_Put(st, hdr, 100, &done);
    int sw = CallVirt<int>(g_q123_696ca0, 20) - 2;
    int sh = CallVirt<int>(g_q123_696ca0, 24) - 2;
    BYTE sd[124];
    memset(sd, 0, sizeof sd);
    F(DWORD, sd, 0) = 124;
    F(DWORD, sd, 4) = 8387054;
    BYTE* pal = F(BYTE*, F(BYTE*, F(BYTE*, g_q123_696ca0, 9396), 100), 0);
    if (!pal)
        return;
    int pitch;
    BYTE* bits;
    if (!fromImage) {
        BYTE* o = g_q123_7208fc;
        if (((long(__stdcall*)(void*, void*, BYTE*, DWORD, void*))(*(void***)o)[25])(o, 0, sd, 17, 0))
            return;
        pitch = F(int, sd, 16);
        bits = F(BYTE*, sd, 36);
    } else {
        BYTE* o = g_q123_6ce4f4;
        pitch = F(WORD, o, 24) * F(WORD, o, 20);
        bits = F(BYTE*, o, 8);
    }
    if (bits) {
        float step = (float)((double)sw / w);
        float y = 0.0f;
        for (int r = h; r > 0; r--) {
            float x = 0.0f;
            const BYTE* row0 = bits + (int)y * pitch;
            const BYTE* row1 = row0 + pitch;
            for (int k = w; k > 0; k--) {
                int c = (int)x;
                int p00 = F(WORD, row0, c * 2), p01 = F(WORD, row0, c * 2 + 2);
                int p10 = F(WORD, row1, c * 2), p11 = F(WORD, row1, c * 2 + 2);
                x = step + x;
                int b = Gs123_Avg(FUN_5b6300, p00, p01, p10, p11);
                int g = Gs123_Avg(FUN_5b62c0, p00, p01, p10, p11);
                int rr = Gs123_Avg(FUN_5b6280, p00, p01, p10, p11);
                BYTE idx = CallThis<BYTE>(FUN_5b9660, pal, rr, g, b);
                Gs123_Put(st, &idx, 1, &done);
            }
            y = (float)((double)sh / h + y);
        }
    }
    if (!fromImage) {
        BYTE* o = g_q123_7208fc;
        ((long(__stdcall*)(void*, void*))(*(void***)o)[32])(o, 0);
    }
}

// ---------------------------------------------------------------------------
// The vehicle data cache.

extern "C" void Gs124_difftime(); KING_ALIAS("_Gs124_difftime", "_difftime")
extern "C" int __cdecl _stat(const char* path, void* buf);
ASM_VAR(const char*, g_q124_674ae0, $L_674ae0)
ASM_VAR(int, g_q124_697860, $L_697860)
ASM_VAR(int, g_q124_6ce32c, $L_6ce32c)
ASM_VAR(AsmLabel, g_q124_697868, $L_697868)
ASM_VAR(AsmLabel, g_q124_69809c, $L_69809c)
ASM_VAR(AsmLabel, g_q124_6c6b68, $L_6c6b68)
ASM_VAR(double, g_q124_64dad0, $L_64dad0)
ASM_VAR(double, g_q124_64b678, $L_64b678)
static double Gs124_Diff(long a, long b) { return CallC<double>(Gs124_difftime, a, b); }
// [FUN_508000] loads the vehicle data cache (674ae0): valid (697860 = 1) when the data file named in it is not
// newer than the cache and has its recorded time, all records (2416 bytes, up to 6c6b68) read, and every
// vehicle file (+2212 of the first 6ce32c records, repeats skipped) exists unchanged since its recorded time
extern "C" int __cdecl VehCache_Load(void)
{
    g_q124_697860 = 0;
    FILE* f = fopen(g_q124_674ae0, "rb");
    if (!f)
        return g_q124_697860;
    int len;
    fread(&len, 4, 1, f);
    if (len >= 200 || len < 0)
        return g_q124_697860;
    char name[200];
    BYTE st[36];
    fread(name, 1, len, f);
    name[len] = 0;
    _stat(g_q124_674ae0, st);
    long cacheTime = F(long, st, 28);
    long t0;
    fread(&t0, 4, 1, f);
    if (_stat(name, st) != -1) {
        if (!(Gs124_Diff(F(long, st, 28), cacheTime) < g_q124_64dad0))
            return g_q124_697860;
        if (!(Gs124_Diff(F(long, st, 28), t0) <= g_q124_64b678))
            return g_q124_697860;
    }
    int v;
    fread(&v, 4, 1, f);
    if (v != 2416)
        return g_q124_697860;
    for (int k = 0; k < 8; k++)
        fread(&g_q124_6ce32c + k, 4, 1, f);
    int r = v;
    for (BYTE* p = (BYTE*)g_q124_697868; r == 2416;) {
        r = fread(p, 1, r, f);
        p += 2416;
        if (p >= (BYTE*)g_q124_6c6b68) {
            if (r == 2416)
                g_q124_697860 = 1;
            break;
        }
    }
    if (g_q124_697860) {
        int missing = 0, found = 0;
        char* nm = (char*)g_q124_69809c + 112;
        for (int i = 0; i < g_q124_6ce32c; i++, nm += 2416) {
            long t;
            fread(&t, 4, 1, f);
            if (nm > (char*)g_q124_69809c + 112 && strcmp(nm, nm - 2416) == 0)
                continue;
            if (_stat(nm, st) == -1) {
                missing++;
                continue;
            }
            if (!(Gs124_Diff(F(long, st, 28), cacheTime) < g_q124_64dad0) ||
                !(Gs124_Diff(F(long, st, 28), t) == g_q124_64b678)) {
                g_q124_697860 = 0;
                break;
            }
            found++;
        }
        if (g_q124_697860 && missing * found)
            g_q124_697860 = 0;
    }
    fclose(f);
    return g_q124_697860;
}

// ---------------------------------------------------------------------------
// A button with texture faces.

extern "C" void FUN_5bf820(); KING_ALIAS("_FUN_5bf820", "@Button_ConstructId@20")
ASM_VAR(AsmLabel, g_q125_6ce3b8, $L_6ce3b8)
ASM_VAR(AsmLabel, g_q125_6ce408, $L_6ce408)
ASM_VAR(AsmLabel, g_q125_6ce988, $L_6ce988)
ASM_VAR(AsmLabel, g_q125_6ceb38, $L_6ceb38)
ASM_VAR(AsmLabel, g_q125_68d1c0, $L_68d1c0)
ASM_VAR(AsmLabel, g_q125_64dc1c, $L_64dc1c)
// a face style from template t with the texture fmt % name as its bitmap
static void Gs125_Face(DWORD* st, const BYTE* t, const char* fmt, const char* name)
{
    char buf[32];
    sprintf(buf, fmt, name);
    for (int k = 0; k < 9; k++)
        st[k] = 0;
    st[6] = 0x3f000000;
    CallThis(FUN_5bd5a0, st, t);
    st[7] = CallC<DWORD>(FUN_5ef3d0, buf);
}
// [FUN_50e830] a button (id) at x, y with the faces name_a/_p/_ap/_pp/_f/_ph.txr, checked as given
extern "C" BYTE* __cdecl Shell_MakeTexButton(const char* name, int id, int x, int y, int checked)
{
    DWORD a[9], p[9], ap[9], pp[9], f[9], ph[9];
    Gs125_Face(a, (BYTE*)g_q125_6ce3b8, "%s_a.txr", name);
    Gs125_Face(p, (BYTE*)g_q125_6ce408, "%s_p.txr", name);
    Gs125_Face(ap, (BYTE*)g_q125_6ce988, "%s_ap.txr", name);
    Gs125_Face(pp, (BYTE*)g_q125_6ceb38, "%s_pp.txr", name);
    BYTE* b = (BYTE*)operator new(288);
    if (b) {
        CallThis(FUN_5bf820, b, (BYTE*)g_q125_68d1c0, 4, id);
        F(void*, b, 0) = (BYTE*)g_q125_64dc1c;
    }
    CallThis(FUN_5bd5a0, b + 68, p);
    CallThis(FUN_5bd5a0, b + 104, a);
    CallThis(FUN_5bd5a0, b + 140, pp);
    CallThis(FUN_5bd5a0, b + 176, ap);
    Gs125_Face(f, (BYTE*)g_q125_6ceb38, "%s_f.txr", name);
    CallThis(FUN_5bd5a0, b + 212, f);
    Gs125_Face(ph, (BYTE*)g_q125_6ceb38, "%s_ph.txr", name);
    CallThis(FUN_5bd5a0, b + 248, ph);
    CallVirt(b, 20);
    int dx = x - F(int, b, 44);
    F(int, b, 52) += dx;
    F(int, b, 44) = x;
    int dy = y - F(int, b, 48);
    F(int, b, 48) = y;
    F(int, b, 56) += dy;
    if (checked)
        F(unsigned, b, 284) |= 2;
    return b;
}

// ---------------------------------------------------------------------------
// A slider with mask faces.

ASM_PROC(FUN_5be8c0)
extern "C" void FUN_5be700(); KING_ALIAS("_FUN_5be700", "@ScrollBar_Construct@24")
extern "C" void FUN_5bef30(); KING_ALIAS("_FUN_5bef30", "@Ctl_SetStyles@28")
// style st from template t with the mask fmt % name as its pattern
static void Gs126_Mask(DWORD* st, const BYTE* t, const char* fmt, const char* name)
{
    char buf[32];
    sprintf(buf, fmt, name);
    CallThis(FUN_5bd5a0, st, t);
    st[8] = CallC<DWORD>(FUN_5ef3d0, buf);
}
static void Gs126_Masks(BYTE* c, DWORD* a, DWORD* p, DWORD* ap, const char* name, int part)
{
    Gs126_Mask(a, (BYTE*)g_q125_6ce3b8, "%s_a.msk", name);
    Gs126_Mask(p, (BYTE*)g_q125_6ce408, "%s_p.msk", name);
    Gs126_Mask(ap, (BYTE*)g_q125_6ce3b8, "%s_ap.msk", name);
    CallThis(FUN_5bef30, c, a, ap, p, p, part);
}
// [FUN_50efb0] a scroll bar at x, y (value a3, range a6..a7) with the mask faces of name1 (bar), the runner and name2
extern "C" BYTE* __cdecl Shell_MakeMaskSlider(const char* name1, const char* name2, int a3, int x, int y, int a6,
                                              int a7)
{
    DWORD a[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    DWORD p[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    DWORD ap[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    BYTE* c = (BYTE*)operator new(344);
    c = c ? CallThis<BYTE*>(FUN_5be700, c, (BYTE*)g_q125_68d1c0, 0, 8, 1) : 0;
    CallThis(FUN_5be8c0, c, 100, a3, a6, 0, a7, 12, 0, 0);
    Gs126_Masks(c, a, p, ap, name1, 0);
    Gs126_Masks(c, a, p, ap, "runner", 1);
    Gs126_Masks(c, a, p, ap, name2, 2);
    CallVirt(c, 20);
    F(int, c, 56) += 10;
    int dx = x - F(int, c, 44);
    F(int, c, 44) = x;
    F(int, c, 52) += dx;
    int dy = y - F(int, c, 48);
    F(int, c, 48) = y;
    F(int, c, 56) += dy;
    return c;
}

// ---------------------------------------------------------------------------
// The skin of a list's scroll bar.

extern "C" void FUN_5bef90(); KING_ALIAS("_FUN_5bef90", "@Ctl_SetStyle5@16")
extern "C" void FUN_5becd0(); KING_ALIAS("_FUN_5becd0", "@Split_Resize@12")
// [FUN_50f300] skins scroll bar c with the atop (part 0) and abot (part 2) mask buttons and frames, a plain
// runner (part 1), moves it to x 0, y 2 and sizes it to h
extern "C" int __cdecl Shell_SkinScrollBar(BYTE* c, int h)
{
    DWORD a[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    DWORD p[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    DWORD ap[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    Gs126_Masks(c, a, p, ap, "atop", 0);
    Gs126_Mask(ap, (BYTE*)g_q125_6ce3b8, "%s_f.msk", "atop");
    CallThis(FUN_5bef90, c, ap, 0);
    CallThis(FUN_5bd5a0, a, (BYTE*)g_q125_6ce3b8);
    a[8] = 0;
    CallThis(FUN_5bd5a0, p, (BYTE*)g_q125_6ce408);
    p[8] = 0;
    CallThis(FUN_5bd5a0, ap, (BYTE*)g_q125_6ce3b8);
    ap[8] = 0;
    CallThis(FUN_5bef30, c, a, ap, p, p, 1);
    Gs126_Masks(c, a, p, ap, "abot", 2);
    Gs126_Mask(ap, (BYTE*)g_q125_6ce3b8, "%s_f.msk", "abot");
    CallThis(FUN_5bef90, c, ap, 2);
    CallVirt(c, 20);
    F(int, c, 52) -= F(int, c, 44);
    F(int, c, 44) = 0;
    F(int, c, 56) += 2 - F(int, c, 48);
    F(int, c, 48) = 2;
    return CallThis<int>(FUN_5becd0, c, h);
}

// ---------------------------------------------------------------------------
// A button with mask faces.

// [FUN_50e500] a button (id) at x, y with the faces name_a/_p/_ap/_f.msk (the masks are the styles' patterns)
extern "C" BYTE* __cdecl Shell_MakeMaskButton(const char* name, int id, int x, int y)
{
    DWORD a[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    DWORD p[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    DWORD ap[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    DWORD ap2[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    DWORD f[9] = {0, 0, 0, 0, 0, 0, 0x3f000000, 0, 0};
    Gs126_Mask(a, (BYTE*)g_q125_6ce3b8, "%s_a.msk", name);
    Gs126_Mask(p, (BYTE*)g_q125_6ce408, "%s_p.msk", name);
    Gs126_Mask(ap, (BYTE*)g_q125_6ce988, "%s_ap.msk", name);
    Gs126_Mask(ap2, (BYTE*)g_q125_6ceb38, "%s_ap.msk", name);
    BYTE* b = (BYTE*)operator new(288);
    if (b)
        b = CallThis<BYTE*>(FUN_5bf820, b, (BYTE*)g_q125_68d1c0, 8, id);
    CallThis(FUN_5bd5a0, b + 68, p);
    CallThis(FUN_5bd5a0, b + 104, a);
    CallThis(FUN_5bd5a0, b + 140, ap2);
    CallThis(FUN_5bd5a0, b + 176, ap);
    Gs126_Mask(f, (BYTE*)g_q125_6ceb38, "%s_f.msk", name);
    CallThis(FUN_5bd5a0, b + 212, f);
    CallVirt(b, 20);
    int dx = x - F(int, b, 44);
    F(int, b, 52) += dx;
    F(int, b, 44) = x;
    int dy = y - F(int, b, 48);
    F(int, b, 48) = y;
    F(int, b, 56) += dy;
    return b;
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q198_Dtor51aad0_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor51aad0_DeletingDestructor@12", "@Dtor51aad0_DeletingDestructor@12")
// [$L_52abf0] forwards to Dtor51aad0_DeletingDestructor for the object 64 bytes before
extern "C" int __fastcall Thunk52abf0(BYTE* s, void* d, int a0) { return q198_Dtor51aad0_DeletingDestructor(s - 64, d, a0); }

extern "C" void FUN_4cd470(); KING_ALIAS("_FUN_4cd470", "@Lights_Add@20")
ASM_VAR(BYTE*, g_q202_6ce520, $L_6ce520)
struct Shell202_Light { BYTE b[104]; };
// [FUN_50bcf0] adds the light l (by value) with a and b to the light list 6ce520, if there is one
extern "C" void __cdecl Shell_AddLight(Shell202_Light l, int a, int b)
{
    if (g_q202_6ce520)
        CallThis(FUN_4cd470, g_q202_6ce520, &l, a, b);
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q238_Fw5d89e0_Destroy(BYTE* s); KING_ALIAS("@q238_Fw5d89e0_Destroy@4", "@Fw5d89e0_Destroy@4")
// [FUN_519740] forwards to Fw5d89e0_Destroy
extern "C" int __fastcall Thunk519740(BYTE* s) { return q238_Fw5d89e0_Destroy(s); }
