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
KING_ALIAS("_Carat_Read7ad0", "_Ini_ReadFloats")
extern "C" int __cdecl Carat_Read7df0(int a, int b, int* values, int c, int d, int n);
KING_ALIAS("_Carat_Read7df0", "_Ini_ReadInts")
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
extern "C" void FUN_5bae30(); KING_ALIAS("_FUN_5bae30", "@Texture_Create@12")
extern "C" void FUN_5d4200(); KING_ALIAS("_FUN_5d4200", "_Dd_LockBack")
extern "C" void FUN_5d41e0(); KING_ALIAS("_FUN_5d41e0", "_Dd_WaitVsync")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
extern "C" void FUN_568fd0(); KING_ALIAS("_FUN_568fd0", "_Game_568fd0")
extern "C" void FUN_5bf820(); KING_ALIAS("_FUN_5bf820", "@Button_ConstructId@20")
extern "C" void FUN_5bf750(); KING_ALIAS("_FUN_5bf750", "@Button_Construct@16")
extern "C" void FUN_603c70(); KING_ALIAS("_FUN_603c70", "_Ini_WriteInt")
extern "C" void FUN_5d8710(); KING_ALIAS("_FUN_5d8710", "@Window_ConstructSub@20")
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
extern "C" void FUN_5c13a0(); KING_ALIAS("_FUN_5c13a0", "@Video_Open@24")    // avi: open
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
extern "C" void FUN_50b620(); KING_ALIAS("_FUN_50b620", "@DMat_Determinant@8")    // DMat4: determinant
extern "C" void FUN_530020(); KING_ALIAS("_FUN_530020", "@Words295_Reset@8")
extern "C" void FUN_5b81c0(); KING_ALIAS("_FUN_5b81c0", "_Image_DrawPart")    // draws a part of an image (surface, image, rect, a, y)
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
extern "C" void FUN_5dae90(); KING_ALIAS("_FUN_5dae90", "@Module_Load@20")
extern "C" void FUN_5c43c0(); KING_ALIAS("_FUN_5c43c0", "@Rec_Mark@8")
extern "C" void FUN_5c0cf0(); KING_ALIAS("_FUN_5c0cf0", "@List_Clear@8")
extern "C" void FUN_51d0e0(); KING_ALIAS("_FUN_51d0e0", "_Save_ListEntries")
extern "C" void FUN_5d8610(); KING_ALIAS("_FUN_5d8610", "@Win_Activate@8")
extern "C" void FUN_531000(); KING_ALIAS("_FUN_531000", "_Controls_SelectPreset")
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
extern "C" void FUN_5bae30(); KING_ALIAS("_FUN_5bae30", "@Texture_Create@12")

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
extern "C" void FUN_4eb4a0(); KING_ALIAS("_FUN_4eb4a0", "@Switch3_Set@12")    // sets a vehicle setting (value)
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
extern "C" void FUN_4d90c0(); KING_ALIAS("_FUN_4d90c0", "_Fx_PlaceObjects")
extern "C" void FUN_4833a0(); KING_ALIAS("_FUN_4833a0", "_AI_SaveToStorage")    // writes the save into a storage
extern "C" void FUN_51d6d0(); KING_ALIAS("_FUN_51d6d0", "_Save_WriteGame")    // writes the screenshot (storage, a)
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
extern "C" void FUN_5b8980(); KING_ALIAS("_FUN_5b8980", "_Gfx_DrawImage")
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
extern "C" void FUN_532950(); KING_ALIAS("_FUN_532950", "@Game_Input@12")
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
extern "C" void FUN_5ea1b0(); KING_ALIAS("_FUN_5ea1b0", "_Font_DrawText")
extern "C" void FUN_5c0cf0(); KING_ALIAS("_FUN_5c0cf0", "@List_Clear@8")
extern "C" void FUN_51d0e0(); KING_ALIAS("_FUN_51d0e0", "_Save_ListEntries")
extern "C" void FUN_5d8b90(); KING_ALIAS("_FUN_5d8b90", "@Wnd5d8b90_Notify@8")
extern "C" void FUN_568f60(); KING_ALIAS("_FUN_568f60", "_Player_CarId")
extern "C" void FUN_43f760(); KING_ALIAS("_FUN_43f760", "_Player_Value136")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")
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

extern "C" void FUN_5bdc80(); KING_ALIAS("_FUN_5bdc80", "@Control_FitSize@8")
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

extern "C" void FUN_50d860(); KING_ALIAS("_FUN_50d860", "_Shell_MakeScrollBar")
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
extern "C" void Wnd_SetPos_(); KING_ALIAS("_Wnd_SetPos_", "@Wnd_SetWindowPos@32")
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

extern "C" void FUN_5bdf80(); KING_ALIAS("_FUN_5bdf80", "@Panel_GridLayout@20")
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

extern "C" void FUN_524b30(); KING_ALIAS("_FUN_524b30", "@Dealer_Open@12")
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

extern "C" void FUN_5b73a0(); KING_ALIAS("_FUN_5b73a0", "_Window_DrawLine")
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
extern "C" void FUN_50c9d0(); KING_ALIAS("_FUN_50c9d0", "_Shell_CreateScreens")
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

extern "C" void FUN_574950(); KING_ALIAS("_FUN_574950", "_Hud_DrawLights")
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

extern "C" void FUN_574530(); KING_ALIAS("_FUN_574530", "_Score_Build")
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

extern "C" void FUN_5ec0f0(); KING_ALIAS("_FUN_5ec0f0", "@Image_ReadResources@24")
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

extern "C" void FUN_50f990(); KING_ALIAS("_FUN_50f990", "@MainMenu_Construct@8")
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
extern "C" int __cdecl Media_AiEvent(BYTE* it, int kind, BYTE* data);  // effects.cpp
extern "C" int __cdecl Present_OnEvent(BYTE* it, int kind);  // effects.cpp
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
            CallC(FUN_435730, App_Path(g_q92_696ca0, 0, map), (BYTE*)(void*)Media_AiEvent, (BYTE*)(void*)Present_OnEvent);
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

extern "C" void FUN_533590(); KING_ALIAS("_FUN_533590", "@Car_PlaceOnRoad@12")
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

extern "C" void FUN_5be8c0(); KING_ALIAS("_FUN_5be8c0", "@ScrollBar_Layout@40")
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

// ---------------------------------------------------------------------------
// The player selection window

extern "C" void FUN_50bbf0(); KING_ALIAS("_FUN_50bbf0", "_Str_Trim")
extern "C" void FUN_5134b0(); KING_ALIAS("_FUN_5134b0", "@Settings_Load@8")
extern "C" void FUN_513600(); KING_ALIAS("_FUN_513600", "@Settings_Save@8")
extern "C" void FUN_5c0d60(); KING_ALIAS("_FUN_5c0d60", "@List_RemoveName@12")
extern "C" void FUN_5c1140(); KING_ALIAS("_FUN_5c1140", "@List_AddNamed@12")
extern "C" void FUN_603c70(); KING_ALIAS("_FUN_603c70", "_Ini_WriteInt")
extern "C" void FUN_5d2e70(); KING_ALIAS("_FUN_5d2e70", "_Sys_Fatal")
ASM_VAR(char, s_dir671204, $L_671204)               // ".\\"
ASM_VAR(char, s_sect672998, $L_672998)
ASM_VAR(char, s_lastp677510, $L_677510)
ASM_VAR(char, s_logo677508, $L_677508)
ASM_VAR(char, s_photo677500, $L_677500)
ASM_VAR(char, s_settings677574, $L_677574)
ASM_VAR(char, s_ver676958, $L_676958)
ASM_VAR(char, s_games67683c, $L_67683c)
ASM_VAR(char, s_noPlayer6775a0, $L_6775a0)

// the name of the list's selected item, 0 if none
static const char* PlW_Selected(BYTE* self)
{
    BYTE* it = F(BYTE*, F(BYTE*, self, 540), 300);
    return it ? F(const char*, it, 12) : 0;
}

// leaves the window for the game's main menu (or the screen it came from when +548 is set)
static void PlW_Leave(BYTE* self, bool check)
{
    BYTE* w = self - 64;
    CallVirt(w, 32);
    if (F(int, self, 548)) {
        CallVirt(g_6cec94, 36);
    } else {
        F(int, g_6cec80, 372) = 0;
        if (!check || g_696848 == w)
            CallVirt(g_6cec80, 36);
    }
}

static void PlW_WriteIni(BYTE* self)
{
    WritePrivateProfileStringA(&s_sect672998, &s_lastp677510, (const char*)self + 568, g_iniPath);
    CallC(FUN_603c70, &s_sect672998, &s_logo677508, F(int, self, 552), g_iniPath);
    CallC(FUN_603c70, &s_sect672998, &s_photo677500, F(int, self, 560), g_iniPath);
}

static void* PlW_CreateStream(void* stg, const char* name)
{
    void* s;
    ComCall(stg, 12, CallC<const WCHAR*>(FUN_50bc90, name), 0x1011, 0, 0, &s);
    return s;
}

extern "C" {

// [$L_51c0a0] the player window's commands: 1 choose or create the player of the edited name, 2 cancel,
// 125 edit the selected name, 127 delete the selected player, 128..131 next/previous logo and photo
void __fastcall PlayerWindow_Command(BYTE* self, void*, int cmd)
{
    switch (cmd) {
    case 127: {
        const char* name = PlW_Selected(self);
        if (!name)
            return;
        char path[64], a[64], b[48];
        strcpy(a, name);
        strcpy(b, name);
        const char* t = CallC<const char*>(FUN_50bbf0, a);
        strcpy(path, &s_dir671204);
        strcat(path, t);
        strcat(path, ".pl1");
        CallThis(FUN_5c0d60, F(BYTE*, self, 540), name);
        if (PlW_Selected(self)) {
            DeleteFileA(path);
            strcpy((char*)self + 604, PlW_Selected(self));
            strcpy((char*)self + 568, CallC<const char*>(FUN_50bbf0, self + 604));
            CallVirt(self - 64, 36);
        } else {
            CallThis(FUN_5c1140, F(BYTE*, self, 540), b);
        }
        return;
    }
    case 128:
        F(int, self, 552) = (F(int, self, 552) + 1) % 8;
        return;
    case 129:
        F(int, self, 552) = (F(int, self, 552) + 7) % 8;
        return;
    case 130:
        F(int, self, 560) = (F(int, self, 560) + 1) % 51;
        return;
    case 131:
        F(int, self, 560) = (F(int, self, 560) + 50) % 51;
        return;
    case 125: {
        strcpy((char*)self + 604, PlW_Selected(self));
        const char* t = CallC<const char*>(FUN_50bbf0, self + 604);
        if (!t)
            return;
        strcpy((char*)self + 568, t);
        CallVirt(self - 64, 36);
        return;
    }
    case 1: {
        const char* t = CallC<const char*>(FUN_50bbf0, self + 604);
        if (!t)
            return;
        void* stg = CallStd<void*>(FUN_51bfa0, t, 1);
        ULONG done;
        int data[2];
        if (stg) {
            ComCall(stg, 8);
            strcpy((char*)self + 604, t);
            strcpy((char*)self + 568, t);
            CallThis(FUN_5134b0, g_6cec6c);
            PlW_Leave(self, true);
            PlW_WriteIni(self);
            void* s = Storage_OpenStream(&stg, 2);
            data[0] = F(int, self, 552);
            data[1] = F(int, self, 560);
            ComCall(s, 16, data, 8, &done);
            ComCall(s, 8);
            ComCall(stg, 8);
            F(int, self, 548) = 0;
            return;
        }
        stg = CallStd<void*>(FUN_51bfa0, t, 0);
        if (!stg)
            CallC(FUN_5d2e70, &s_noPlayer6775a0, 0);
        strcpy((char*)self + 568, t);
        strcpy((char*)self + 604, t);
        void* s = PlW_CreateStream(stg, &s_settings677574);
        data[0] = F(int, self, 552);
        data[1] = F(int, self, 560);
        ComCall(s, 16, data, 8, &done);
        ComCall(s, 8);
        s = PlW_CreateStream(stg, &s_ver676958);
        data[1] = 4;
        ComCall(s, 16, &data[1], 4, &done);
        ComCall(s, 8);
        void* sub;
        ComCall(stg, 20, CallC<const WCHAR*>(FUN_50bc90, &s_games67683c), 0x1011, 0, 0, &sub);
        ComCall(sub, 8);
        ComCall(stg, 8);
        CallThis(FUN_513600, g_6cec6c);
        PlW_Leave(self, true);
        PlW_WriteIni(self);
        F(int, self, 548) = 0;
        return;
    }
    case 2: {
        void* stg = CallStd<void*>(FUN_51bfa0, self + 568, 1);
        if (stg) {
            PlW_Leave(self, false);
            F(int, self, 548) = 0;
            ComCall(stg, 8);
        }
        F(int, self, 552) = F(int, self, 556);
        F(int, self, 560) = F(int, self, 564);
        return;
    }
    }
}

}  // extern "C"

// === the network connection dialog ($L_519800)

ASM_VAR(int, g_nd6968f4, $L_6968f4)          // 1: joining a game, 2: hosting one
ASM_VAR(int, g_nd6968f0, $L_6968f0)
ASM_VAR(int, g_nd6968f8, $L_6968f8)
ASM_VAR(int, g_nd696908, $L_696908)
ASM_VAR(int, g_ndNames, $L_6ce9f0)           // the count of names, then 16 bytes per name (6ce9f4)
ASM_VAR(AsmLabel, g_ndNameText, $L_6ce9f4)
ASM_VAR(AsmLabel, g_ndFlags6cea74, $L_6cea74)
ASM_VAR(BYTE*, g_nd6cecc0, $L_6cecc0)
ASM_VAR(BYTE*, g_nd6cecc4, $L_6cecc4)
ASM_VAR(BYTE*, g_nd6cecd8, $L_6cecd8)
ASM_VAR(BYTE*, g_nd6cec94, $L_6cec94)
ASM_VAR(BYTE*, g_nd6cec98, $L_6cec98)
ASM_VAR(BYTE*, g_ndFont, $L_6ced28)
ASM_VAR(AsmLabel, g_ndNet696750, $L_696750)
ASM_VAR(AsmLabel, g_nd723d50, $L_723d50)
ASM_VAR(double, g_nd64b5f0, $L_64b5f0)
extern "C" void FUN_5c3c50(); KING_ALIAS("_FUN_5c3c50", "@Music_WaitNetwork@16")    // net: send a message (name, msg)
extern "C" void FUN_5c55b0(); KING_ALIAS("_FUN_5c55b0", "@Net_HostReply@16")    // net: the host's reply (reply, -)
extern "C" void FUN_5c3ac0(); KING_ALIAS("_FUN_5c3ac0", "@Net_CheckSession@8")    // net: whether the session is open
extern "C" void FUN_5c5740(); KING_ALIAS("_FUN_5c5740", "@Chat_Receive@20")    // net: host (name, 0, 0)
extern "C" void FUN_5ea1b0(); KING_ALIAS("_FUN_5ea1b0", "_Font_DrawText")    // text in a width (surface, x, y, width, font, text, align, -)

static const char* Nd_Str(int id)
{
    return CallC<const char*>(FUN_5d2e50, id);
}

static void Nd_Text(BYTE* self, int x, int y, const char* text)
{
    CallC(FUN_5ea170, F(BYTE*, self, -52), x, y, g_ndFont, text, 4, 0);
}

// the text id as a line centered in the dialog's width (+308) at x offset dx
static void Nd_Centered(BYTE* self, int id, int y, int dx = 160)
{
    char buf[64];
    sprintf(buf, Nd_Str(id));
    int w = CallThis<int>(FUN_5ea0e0, g_ndFont, buf);
    CallC(FUN_5ea170, F(BYTE*, self, -52), (F(int, self, 308) - w) / 2 + dx, y, g_ndFont, buf, 4, 0);
}

static void Nd_Close(BYTE* self)
{
    CallVirt(self - 64, 32);
}

extern "C" {

// [$L_519800] draws the network dialog on surface: mode +312 2 connecting (joining: the host's reply starts the
// game; else a timeout after +320), 3 waiting for players (the hosts' list), 1/4/5 messages, else the message
// of state +304
void __fastcall NetDialog_Draw(BYTE* self, void*, BYTE* surface)
{
    CallThis(FUN_5c0020, self, surface);
    int mode = F(int, self, 312);
    if (mode == 2) {
        if (g_nd6968f4 != 1) {
            double end = F(double, self, 320) + g_nd64b5f0;
            if (Timer_Now() > end) {
                Nd_Close(self);
                CallThis(FUN_519750, g_nd6cecc4, 0);
                return;
            }
            BYTE msg[64];
            msg[0] = 32;
            msg[1] = 0;
            memset(msg + 2, 0, 62);
            CallThis(FUN_5c3c50, (BYTE*)g_ndNet696750, g_nd6cecd8 + 632, msg);
            if (g_nd6968f0 == 0) {
                Nd_Close(self);
                CallThis(FUN_519750, g_nd6cecc0, 10);
                return;
            }
            Nd_Centered(self, 74, 292);
            return;
        }
        BYTE reply[200];
        BYTE x[16];
        int r = CallThis<int>(FUN_5c55b0, (BYTE*)g_ndNet696750, reply, x);
        if (r == 0) {
            Nd_Centered(self, 75, 292);
            return;
        }
        if (r == 2) {
            Nd_Close(self);
            CallThis(FUN_519750, g_nd6cecc0, 12);
            return;
        }
        F(int, g_nd6cec98, 384) = reply[0];
        F(short, g_nd6cec98, 388) = reply[1];
        F(short, g_nd6cec98, 392) = reply[2];
        F(short, g_nd6cec98, 390) = reply[3];
        g_nd696908 = reply[4];
        int n = reply[5];
        g_ndNames = 0;
        for (int k = 0; k < n; k++)
            CallThis(FUN_50c770, &g_ndNames, reply + 10 + k * 20);
        Nd_Close(self);
        CallC(FUN_510bc0, 0, 1);
        return;
    }
    if (mode == 3) {
        if (g_nd6968f4 != 2) {
            if (CallThis<int>(FUN_5c3ac0, (BYTE*)g_ndNet696750) == 0) {
                CallThis(FUN_519750, g_nd6cecc0, 11);
                Nd_Close(self);
                return;
            }
            if (g_nd6968f4 != 2)
                return;
            g_ndNames = 0;
            int i = CallThis<int>(FUN_50c770, &g_ndNames, g_nd6cecd8 + 632);
            ((int*)(char*)g_ndFlags6cea74)[i] = 1;
            return;
        }
        CallThis(FUN_5c5740, (BYTE*)g_ndNet696750, g_nd6cecd8 + 632, 0, 0);
        Nd_Text(self, 260, 292, Nd_Str(76));
        int n = g_ndNames;
        int cols = (n + 1) / 2;
        int w = 300 / cols;
        for (int i = 0; i < g_ndNames; i++)
            CallC(FUN_5ea1b0, surface, i / 2 * w + 260, i % 2 * 15 + 307, w - 10, g_ndFont,
                  (char*)g_ndNameText + i * 16, 4, 0);
        return;
    }
    if (mode == 1) {
        Nd_Text(self, 260, 292, Nd_Str(67));
        return;
    }
    if (mode == 4) {
        if (memcmp(g_nd6cec94 + g_nd6968f8 * 32 + 552, g_nd723d50, 16) == 0) {
            Nd_Text(self, 260, 292, Nd_Str(77));
            Nd_Text(self, 260, 307, Nd_Str(78));
            Nd_Text(self, 260, 322, Nd_Str(79));
        } else
            Nd_Text(self, 260, 292, Nd_Str(80));
        return;
    }
    if (mode == 5) {
        Nd_Centered(self, 120, 292);
        Nd_Centered(self, 121, 307);
        Nd_Centered(self, 122, 322);
        return;
    }
    switch (F(int, self, 304)) {
    case 0:
        Nd_Centered(self, 13, 292);
        break;
    case 2:
        Nd_Centered(self, 43, 285, 157);
        Nd_Centered(self, 44, 300, 157);
        Nd_Centered(self, 45, 315, 157);
        break;
    case 4:
        Nd_Centered(self, 46, 282, 157);
        Nd_Centered(self, 47, 297, 157);
        break;
    case 9:
        Nd_Centered(self, 49, 292);
        Nd_Centered(self, 50, 312);
        break;
    case 10:
        Nd_Centered(self, 96, 292);
        break;
    case 11:
        Nd_Centered(self, 68, 292);
        break;
    case 12:
        Nd_Centered(self, 309, 292);
        break;
    }
}

} // extern "C"

// === the service screen ($L_521430)

ASM_VAR(AsmLabel, g_ss6d1948, $L_6d1948)
ASM_VAR(BYTE*, g_ssGame, $L_6d2078)
ASM_VAR(float, g_ssNear64c664, $L_64c664)       // how near a service station must be
ASM_VAR(double, g_ssNoPrice64ca48, $L_64ca48)
ASM_VAR(double, g_ssMin64b5a0, $L_64b5a0)
ASM_VAR(double, g_ss64b6e0, $L_64b6e0)
ASM_VAR(double, g_ssScale64b848, $L_64b848)
extern "C" void FUN_50bd30(); KING_ALIAS("_FUN_50bd30", "_Carat_UpdateScreenSize")
extern "C" void FUN_530020(); KING_ALIAS("_FUN_530020", "@Words295_Reset@8")
extern "C" void FUN_4e12b0(); KING_ALIAS("_FUN_4e12b0", "_Joy_ForceFeedback")
extern "C" void FUN_5d8610(); KING_ALIAS("_FUN_5d8610", "@Win_Activate@8")
extern "C" void FUN_436300(); KING_ALIAS("_FUN_436300", "_List68cbe8_FirstId")
extern "C" void FUN_436330(); KING_ALIAS("_FUN_436330", "_List68cbe8_NextId")
extern "C" void FUN_441f40(); KING_ALIAS("_FUN_441f40", "_Service_Info")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
extern "C" void FUN_4769c0(); KING_ALIAS("_FUN_4769c0", "_Table68cc08_Get")
extern "C" void FUN_476930(); KING_ALIAS("_FUN_476930", "_Player_GetState")
extern "C" void FUN_568f60(); KING_ALIAS("_FUN_568f60", "_Player_CarId")
extern "C" void FUN_5bdb50(); KING_ALIAS("_FUN_5bdb50", "@Ctl_FindId@12")

static __declspec(noinline) int Ss_Int(double d)
{
    return (int)d;
}

static void Ss_InitLights(BYTE* st)
{
    for (int k = 0; k < 19; k++) {
        F(int, st, k * 16 + 8) = 0;
        F(int, st, k * 16) = 0;
        F(int, st, k * 16 + 12) = 1072693248;
    }
}

// per service button 0..18: the light state it repairs and the price slot (self+off)
static const short k_ssService[19][2] = {
    {3, 380}, {4, 384}, {5, 388}, {6, 392}, {7, 396}, {17, 400}, {0, 404}, {1, 408}, {18, 412},
    {9, 416}, {9, 420}, {13, 424}, {2, 428}, {15, 432}, {16, 436}, {10, 440}, {12, 444}, {8, 448}, {11, 452},
};
// per refill button 0..6: the vehicle's virtual getter of the amount
static const short k_ssRefill[7] = {56, 40, 52, 48, 36, 60, 44};

extern "C" {

// [$L_521430] opens the service screen: the state of the service station nearest the vehicle (if close enough),
// then per service button its price (+380..) and whether it applies, the total (+456), and per refill button its
// price (+460..) and the total (+488); resets the vehicle's controls
void __fastcall ServiceScreen_Open(BYTE* self, void*)
{
    CallC(FUN_50bd30);
    CallThis(FUN_530020, (BYTE*)g_ss6d1948);
    CallC(FUN_4e12b0, 0);
    CallThis(FUN_5d8610, self);
    BYTE station[19 * 16];
    Ss_InitLights(station);
    int id;
    CallC(FUN_436300, &id);
    while (id != -1) {
        BYTE info[24 + 19 * 16];
        F(int, info, 4) = -1;
        F(int, info, 8) = 0;
        F(int, info, 12) = 0;
        F(int, info, 16) = 0;
        Ss_InitLights(info + 24);
        CallC(FUN_441f40, id, info);
        float at[3] = {F(float, info, 8), F(float, info, 12), F(float, info, 16)};
        BYTE m[64];
        float* mt = CallThis<float*>(FUN_57de90, F(BYTE*, g_ssGame, 616) + 16, m) + 9;
        float d[3];
        d[0] = at[0] - mt[0];
        d[1] = at[1] - mt[1];
        d[2] = at[2] - mt[2];
        if (!(CallC<double>(FUN_5ab4d0, d) >= g_ssNear64c664)) {
            memcpy(station, info + 24, sizeof station);
            break;
        }
        int next;
        id = *CallC<int*>(FUN_436330, &next, id);
    }
    BYTE own[19 * 16];
    Ss_InitLights(own);
    int car;
    CallC(FUN_476930, *CallC<int*>(FUN_568f60, &car), own);
    F(int, self, 376) = 0;
    F(int, self, 456) = 0;
    F(int, self, 488) = 0;
    for (int k = 0; k < 20; k++) {
        BYTE* ctl = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, self, 368), 2 * k + 217);
        int need = 0, offered = 0;
        if (k == 19)
            offered = 1;
        else {
            int s = k_ssService[k][0], off = k_ssService[k][1];
            double price;
            if (k == 9 || k == 10) {
                int bit = k == 9 ? 1 : 2;
                need = F(int, own, s * 16) == bit;
                offered = (F(BYTE, station, s * 16) & bit) != 0;
                price = offered ? CallC<double>(FUN_4769c0, s, bit) : g_ssNoPrice64ca48;
            } else {
                offered = F(int, station, s * 16);
                need = F(int, own, s * 16);
                price = offered ? CallC<double>(FUN_4769c0, s, 1) : g_ssNoPrice64ca48;
            }
            F(int, self, off) = Ss_Int(price);
        }
        if (need)
            F(DWORD, ctl, 284) |= 2;
        else
            F(DWORD, ctl, 284) &= ~2u;
        if (!offered)
            F(int, ctl, 40) = 1;
        else {
            F(int, ctl, 40) = 0;
            if (!need && k != 19)
                F(int, self, 456) += F(int, self, 380 + k * 4);
        }
    }
    F(int, self, 488) = 0;
    for (int i = 0; i < 8; i++) {
        BYTE* ctl = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, self, 372), 2 * i + 257);
        double v;
        if (i == 7)
            v = g_ss64b6e0;
        else {
            v = CallVirt<double>(F(BYTE*, g_ssGame, 616), k_ssRefill[i]);
            if (!(v >= g_ssMin64b5a0)) {
                F(int, ctl, 40) = 1;
                F(int, self, 460 + i * 4) = -1;
                continue;
            }
        }
        F(DWORD, ctl, 284) &= ~2u;
        F(int, ctl, 40) = 0;
        if (i != 7) {
            int price = Ss_Int(v * g_ssScale64b848);
            F(int, self, 460 + i * 4) = price;
            F(int, self, 488) += price;
        }
    }
    BYTE* veh = F(BYTE*, F(BYTE*, g_ssGame, 616), 21600);
    F(BYTE, veh, 9984) = 0;
    F(DWORD, veh, 10000) = 1056964608;     // 0.5f
    F(int, veh, 9996) = 0;
    F(short, veh, 10004) = 0;
    F(short, veh, 10006) = 1;
    F(short, veh, 10054) = 21;
}

} // extern "C"

// === the vehicle dealer screen ($L_525250)

ASM_VAR(BYTE*, g_vd6ce4f4, $L_6ce4f4)
ASM_VAR(BYTE*, g_vdView, $L_69684c)
ASM_VAR(BYTE*, g_vdFont, $L_6ced24)
ASM_VAR(BYTE*, g_vdImages, $L_6cecbc)
ASM_VAR(BYTE*, g_vdGame, $L_6d2078)
ASM_VAR(double, g_vdWait64b7e0, $L_64b7e0)
ASM_VAR(double, g_vdBlink64b770, $L_64b770)
ASM_VAR(AsmLabel, s_vd66c8cc, $L_66c8cc)
ASM_VAR(AsmLabel, s_vd66d9c4, $L_66d9c4)
extern "C" void FUN_5b8980(); KING_ALIAS("_FUN_5b8980", "_Gfx_DrawImage")
extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")
extern "C" void FUN_5fd160(); KING_ALIAS("_FUN_5fd160", "_Sound_Frame")
extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")
extern "C" void FUN_523210(); KING_ALIAS("_FUN_523210", "_Carat_TypeName")
extern "C" void FUN_433660(); KING_ALIAS("_FUN_433660", "_Ai_PrepareVehicleOrder")
extern "C" void FUN_43f8d0(); KING_ALIAS("_FUN_43f8d0", "_Player_PlaceField20")
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")

static __declspec(noinline) int Vd_Int(double d)
{
    return (int)d;
}

static void Vd_Image(BYTE* self, int x, int y, BYTE* img)
{
    CallC(FUN_5b86c0, F(BYTE*, self, -52), x, y, img);
}

static void Vd_Text(BYTE* self, int x, int y, const char* text)
{
    CallC(FUN_5ea170, F(BYTE*, self, -52), x, y, g_vdFont, text, 4, 0);
}

static void Vd_Price(BYTE* self, int x, int y)
{
    Vd_Image(self, x + 132, y + 20, F(BYTE*, self, 336));
    char buf[100];
    sprintf(buf, s_vd66c8cc, Vd_Int(F(double, self, 344)));
    Vd_Text(self, x + 170, y + 75, buf);
}

// waits: after 64b7e0 past +352 the state goes to 1 and control +320 is enabled
static void Vd_Wait(BYTE* self, int x, int y, BYTE* img)
{
    Vd_Image(self, x + 132, y + 20, img);
    double end = F(double, self, 352) + g_vdWait64b7e0;
    if (Timer_Now() > end) {
        F(int, self, 308) = 1;
        F(int, F(BYTE*, self, 320), 40) = 0;
    }
}

static BYTE* Vd_PlaceImage(int place, int table)
{
    return F(BYTE*, g_vdImages, place * 4 + table);
}

extern "C" {

// [$L_525250] draws the vehicle dealer screen on surface: by state +308 a waiting picture, the price, or the
// offered vehicle (+796 model, +800 color; models the dealer +360 lacks are skipped in the direction +312)
// with its order data; then the dealer's or the player's place picture by mode +304
void __fastcall Dealer_Draw(BYTE* self, void*, BYTE* surface)
{
    if (g_vd6ce4f4)
        CallC(FUN_5b86c0, F(BYTE*, g_vdView, 12), 0, 0, g_vd6ce4f4);
    CallThis(FUN_5c0020, self, surface);
    int x = F(int, F(BYTE*, self, 288), 44);
    int y = F(int, F(BYTE*, self, 288), 48);
    int st = F(int, self, 308);
    switch (st) {
    case 0:
        Vd_Wait(self, x, y, F(BYTE*, self, 332));
        break;
    case 2:
        Vd_Price(self, x, y);
        break;
    case 4:
        Vd_Wait(self, x, y, F(BYTE*, self, 340));
        break;
    case 3: {
        // the vehicle order: +12 count, +16 / +32 / +40 doubles, +48 a text
        BYTE o[64];
        char a;
        CallThis(FUN_487730, o, &a);
        for (int k = 12; k <= 44; k += 4)
            F(int, o, k) = 0;
        F(int, o, 52) = -1;
        F(int, o, 56) = -1;
        for (;;) {
            const char* model = CallC<const char*>(FUN_523210, F(int, self, 796), F(int, self, 800));
            if (CallC<int>(FUN_433660, F(int, F(BYTE*, g_vdGame, 616), 20636), F(int, self, 360), model, o) != 1)
                break;
            if (F(int, self, 312))
                F(int, self, 796) = (F(int, self, 796) + 1) % 27;
            else if (--F(int, self, 796) < 0)
                F(int, self, 796) += 27;
        }
        BYTE* img = F(BYTE*, self, (F(int, self, 800) + F(int, self, 796) * 4) * 4 + 364);
        CallC(FUN_5b8980, F(BYTE*, self, -52), x + 60, y + 18, img, -1);
        char buf[100];
        sprintf(buf, s_vd66d9c4, F(int, o, 12));
        Vd_Text(self, x + 103, y + 235, buf);
        sprintf(buf, s_vd66c8cc, Vd_Int(F(double, o, 32)));
        Vd_Text(self, x + 143, y + 255, buf);
        sprintf(buf, s_vd66c8cc, Vd_Int(F(double, o, 16)));
        Vd_Text(self, x + 143, y + 275, buf);
        if (Vd_Int(Timer_Now() * g_vdBlink64b770) % 2)
            Vd_Text(self, x + 143, y + 295, F(const char*, o, 48));
        sprintf(buf, s_vd66c8cc, Vd_Int(F(double, o, 40)));
        Vd_Text(self, x + 143, y + 335, buf);
        if (unsigned size = F(BYTE*, o, 8) - F(BYTE*, o, 0))
            CallC(FUN_44b4a0, F(BYTE*, o, 0), size);
        return;
    }
    default:
        if (st > 20) {
            if (F(int, F(BYTE*, self, 908), 20) == 0) {
                int m = F(int, self, 304);
                BYTE* s;
                if (st == 22)
                    s = m < 2 ? F(BYTE*, self, 860) : F(BYTE*, self, 864);
                else if (st == 24)
                    s = F(BYTE*, self, 860);
                else
                    s = m == 1 ? F(BYTE*, self, 864) : F(BYTE*, self, 872);
                F(BYTE*, self, 908) = s;
                CallThis(FUN_5fdaf0, s, 1.0f, 1.0f);
                F(int, self, 308) -= 10;
            }
        } else if (st > 10) {
            if (F(int, F(BYTE*, self, 908), 20) == 0) {
                if (st == 12)
                    F(int, F(BYTE*, self, 324), 40) = 0;
                else if (st == 13)
                    F(int, F(BYTE*, self, 328), 40) = 0;
                F(int, self, 308) -= 10;
            } else if (st == 12)
                Vd_Price(self, x, y);
            else if (st == 14)
                Vd_Image(self, x + 132, y + 20, F(BYTE*, self, 340));
        }
        break;
    }
    int mode = F(int, self, 304);
    st = F(int, self, 308);
    static const short k_car[4] = {820, 824, 828, 832};
    static const short k_dealer[4] = {804, 808, 812, 816};
    int i = mode == 0 || mode == 1 || mode == 2 ? mode : 3;
    bool late = i >= 2;
    bool dealer = st == 0 || st == 12 || (late && (st == 13 || st == 14));
    if (late && !dealer && st == 3) {
        CallC(FUN_5fd160);
        return;
    }
    if (!dealer) {
        Vd_Image(self, x + 149, y + 90, F(BYTE*, self, k_car[i]));
        int car;
        int place = CallC<int>(FUN_43f8d0, *CallC<int*>(FUN_568f60, &car));
        Vd_Image(self, x + 155, y + 96, Vd_PlaceImage(place, 368));
    } else {
        Vd_Image(self, x + 19, y + 20, F(BYTE*, self, k_dealer[i]));
        BYTE* img;
        if (late)
            img = F(BYTE*, g_vdImages, 668);
        else
            img = Vd_PlaceImage(CallC<int>(FUN_43f8d0, F(int, self, 360)), mode == 0 ? 604 : 572);
        Vd_Image(self, x + 25, y + 26, img);
    }
    CallC(FUN_5fd160);
}

} // extern "C"

// === the service screen's buttons ($L_5220c0)

ASM_VAR(BYTE*, g_sb696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_sbShell, $L_6cec6c)
ASM_VAR(AsmLabel, s_sb67320c, $L_67320c)
ASM_VAR(double, g_sb64b5b8, $L_64b5b8)
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")
extern "C" void FUN_43f760(); KING_ALIAS("_FUN_43f760", "_Player_Value136")
extern "C" void FUN_43f7c0(); KING_ALIAS("_FUN_43f7c0", "_Player_Set136")
extern "C" void FUN_4769e0(); KING_ALIAS("_FUN_4769e0", "_Vehicle_SetTable516")
extern "C" void FUN_477300(); KING_ALIAS("_FUN_477300", "_Player_ClearFlags")
extern "C" void FUN_477310(); KING_ALIAS("_FUN_477310", "_Driver_CargoFactor")
extern "C" void FUN_520000(); KING_ALIAS("_FUN_520000", "_Car_ApplyPlayerSettings")
extern "C" void FUN_53b3c0(); KING_ALIAS("_FUN_53b3c0", "@King_53b3c0@12")
extern "C" void FUN_56ae60(); KING_ALIAS("_FUN_56ae60", "_Messages_Add")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")

// per refill button 0..6: the vehicle's virtual refill (amount*)
static const short k_sbRefill[7] = {28, 12, 24, 20, 8, 32, 16};

static BYTE* Sb_Ctl(BYTE* self, int list, int id)
{
    return CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, self, list), id);
}

static int Sb_Car()
{
    int car;
    return *CallC<int*>(FUN_568f60, &car);
}

extern "C" {

// [$L_5220c0] a button of the service screen (this = the screen + 64): service buttons 217..253 (even: take
// off, odd: put on; 9 and 10 exclude each other) and refill buttons 257..269 change the total (+312); 255 / 271
// select all; 2 closes; 279 pays: the repairs, the refills and the driver's cargo factor, if the money suffices
void __fastcall ServiceScreen_Command(BYTE* self, void*, int id)
{
    if (id >= 217 && id < 255) {
        int k = (id - 217) / 2;
        if (id % 2 == 0) {
            F(int, self, 312) -= F(int, self, 316 + k * 4);
            F(int, self, 392) += F(int, self, 316 + k * 4);
            F(DWORD, Sb_Ctl(self, 304, 255), 284) &= ~2u;
            return;
        }
        if (k == 9 || k == 10) {
            BYTE* other = Sb_Ctl(self, 304, k == 9 ? 237 : 235);
            if (F(DWORD, other, 284) & 2) {
                F(DWORD, other, 284) &= ~2u;
                int p = F(int, self, k == 9 ? 356 : 352);
                F(int, self, 312) -= p;
                F(int, self, 392) += p;
            }
        }
        F(int, self, 312) += F(int, self, 316 + k * 4);
        F(int, self, 392) -= F(int, self, 316 + k * 4);
        return;
    }
    if (id >= 257 && id < 271) {
        int k = (id - 257) / 2;
        if (id % 2 == 0) {
            F(int, self, 312) -= F(int, self, 396 + k * 4);
            F(int, self, 424) += F(int, self, 396 + k * 4);
            F(DWORD, Sb_Ctl(self, 308, 271), 284) &= ~2u;
        } else {
            F(int, self, 312) += F(int, self, 396 + k * 4);
            F(int, self, 424) -= F(int, self, 396 + k * 4);
        }
        return;
    }
    if (id == 255) {
        for (int i = 0; i < 19; i++) {
            BYTE* ctl = Sb_Ctl(self, 304, 2 * i + 217);
            if (!(F(DWORD, ctl, 284) & 2) && F(int, ctl, 40) == 0) {
                if (i == 10)
                    F(DWORD, ctl, 284) &= ~2u;
                else {
                    F(DWORD, ctl, 284) |= 2;
                    F(int, self, 312) += F(int, self, 316 + i * 4);
                }
            }
        }
        F(int, self, 392) = 0;
        return;
    }
    if (id == 271) {
        for (int j = 0; j < 7; j++) {
            BYTE* ctl = Sb_Ctl(self, 308, 257 + 2 * j);
            if (!(F(DWORD, ctl, 284) & 2) && F(int, ctl, 40) == 0) {
                F(DWORD, ctl, 284) |= 2;
                F(int, self, 312) += F(int, self, 396 + j * 4);
            }
        }
        F(int, self, 424) = 0;
        return;
    }
    if (id == 2) {
        CallVirt(self - 64, 32);
        CallVirt(g_sb696ca0, 36);
        CallC(FUN_4e12b0, 1);
        F(BYTE, F(BYTE*, F(BYTE*, g_ssGame, 616), 21600), 9984) = 2;
        return;
    }
    if (id != 279)
        return;

    int extra = 0;
    BYTE* all = Sb_Ctl(self, 308, 271);
    if ((F(DWORD, all, 284) & 2) && (CallC<DWORD>(FUN_43f620, Sb_Car()) & 0x2000000))
        extra = Ss_Int(CallC<double>(FUN_477310, Sb_Car()));
    int cost = F(int, self, 312) + extra;
    if ((double)cost > CallC<double>(FUN_43f760, Sb_Car())) {
        CallC(FUN_56ae60, CallC<const char*>(FUN_5d2e50, 12));
        return;
    }
    BYTE st[19 * 16];
    Ss_InitLights(st);
    CallC(FUN_476930, Sb_Car(), st);
    F(int, st, 9 * 16) = 0;
    for (int k = 0; k < 19; k++) {
        BYTE* ctl = Sb_Ctl(self, 304, 2 * k + 217);
        int on = F(DWORD, ctl, 284) >> 1 & 1;
        int s = k_ssService[k][0];
        if (k == 9 || k == 10) {
            if (on)
                F(int, st, s * 16) = k == 9 ? 1 : 2;
        } else
            F(int, st, s * 16) = on;
    }
    CallC(FUN_4769e0, Sb_Car(), st);
    CallC(FUN_520000, F(BYTE*, g_ssGame, 616));
    float amount = 1.0f;
    for (int j = 0; j < 8; j++) {
        BYTE* ctl = Sb_Ctl(self, 308, 257 + 2 * j);
        if ((F(DWORD, ctl, 284) & 2) && j < 7)
            CallVirt(F(BYTE*, g_ssGame, 616), k_sbRefill[j], &amount);
    }
    double money = CallC<double>(FUN_43f760, Sb_Car());
    CallVirt(F(BYTE*, g_ssGame, 616), 116);
    CallVirt(F(BYTE*, g_ssGame, 616), 120);
    if (CallC<DWORD>(FUN_43f620, Sb_Car()) & 0x2000000) {
        CallC(FUN_56ae60, CallC<const char*>(FUN_5d2e50, 119));
        if (extra > 1)
            CallC(FUN_477300, Sb_Car());
    }
    if (F(DWORD, all, 284) & 2) {
        float f = (float)(CallVirt<double>(F(BYTE*, g_ssGame, 616), 84) + g_sb64b5b8);
        CallThis(FUN_53b3c0, g_ssGame, f);
    }
    if (F(int, g_sbShell, 1316) == 0) {
        CallC(FUN_43f7c0, Sb_Car(), money - (double)cost);
        char buf[64];
        sprintf(buf, CallC<const char*>(FUN_5d2e50, 10), cost);
        CallC(FUN_56ae60, buf);
    }
    CallThis(FUN_5fdaf0, CallC<BYTE*>(FUN_5ef3d0, (const char*)s_sb67320c), 0.5f, 0.5f);
    CallVirt(g_sb696ca0, 36);
    F(BYTE, F(BYTE*, F(BYTE*, g_ssGame, 616), 21600), 9984) = 2;
    CallC(FUN_4e12b0, 1);
    CallVirt(self - 64, 32);
}

} // extern "C"

// === the video options screen's buttons ($L_5162f0)

ASM_VAR(char*, g_voIni, $L_696868)
ASM_VAR(BYTE*, g_voView, $L_69684c)
ASM_VAR(BYTE*, g_vo696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_vo6cec80, $L_6cec80)
ASM_VAR(BYTE*, g_vo6cecc0, $L_6cecc0)
ASM_VAR(BYTE*, g_vo696848, $L_696848)
ASM_VAR(int, g_vo7208e4, $L_7208e4)
ASM_VAR(int, g_vo675ab0, $L_675ab0)
ASM_VAR(int, g_voMode6cecfc, $L_6cecfc)
ASM_VAR(int, g_voMode6ced00, $L_6ced00)
ASM_VAR(int, g_voMode6ced04, $L_6ced04)
ASM_VAR(int, g_voMode6ced08, $L_6ced08)
ASM_VAR(AsmLabel, s_vo672998, $L_672998)
ASM_VAR(AsmLabel, s_vo6730a0, $L_6730a0)
ASM_VAR(AsmLabel, s_vo676dd0, $L_676dd0)
ASM_VAR(AsmLabel, s_vo676dc8, $L_676dc8)
ASM_VAR(AsmLabel, s_vo676798, $L_676798)
ASM_VAR(AsmLabel, s_vo676ce4, $L_676ce4)
ASM_VAR(AsmLabel, s_vo676d18, $L_676d18)
ASM_VAR(AsmLabel, s_vo676d20, $L_676d20)
ASM_VAR(AsmLabel, s_vo676d28, $L_676d28)
ASM_VAR(AsmLabel, s_vo676d30, $L_676d30)
ASM_VAR(AsmLabel, s_vo676cec, $L_676cec)
ASM_VAR(AsmLabel, s_vo676d04, $L_676d04)
ASM_VAR(AsmLabel, s_vo676d10, $L_676d10)
ASM_VAR(AsmLabel, s_vo676d98, $L_676d98)
ASM_VAR(AsmLabel, s_vo676d90, $L_676d90)
ASM_VAR(AsmLabel, s_vo676dd8, $L_676dd8)
extern "C" void FUN_603c70(); KING_ALIAS("_FUN_603c70", "_Ini_WriteInt")    // Ini_WriteInt (section, key, value, file)
extern "C" void FUN_5159d0(); KING_ALIAS("_FUN_5159d0", "_Video_PickModes")
extern "C" void FUN_5b6bf0(); KING_ALIAS("_FUN_5b6bf0", "@Surface_Prepare@4")
extern "C" void FUN_5d8630(); KING_ALIAS("_FUN_5d8630", "@Window_Show@12")
extern "C" void FUN_5d88c0(); KING_ALIAS("_FUN_5d88c0", "@Window_Refresh@8")
extern "C" void FUN_5ba3d0(); KING_ALIAS("_FUN_5ba3d0", "@Palette_Apply@8")
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")
extern "C" void FUN_5d2e70(); KING_ALIAS("_FUN_5d2e70", "_Sys_Fatal")

static void Vo_Write(const char* section, const char* key, int value)
{
    CallC(FUN_603c70, section, key, value, g_voIni);
}

static void Vo_SetMode(DWORD flags)
{
    CallThis($L_5dd8c0, (BYTE*)g_gs696750, 800, 600, 1, flags);
}

// picks the video modes of mode set +348
static int Vo_Pick(BYTE* self)
{
    return CallC<int>(FUN_5159d0, F(int, self, 348), self + 352, &g_voMode6cecfc, self + 328, &g_voMode6ced00,
                      self + 332, &g_voMode6ced04, self + 336, &g_voMode6ced08, self + 340);
}

static void Vo_Show()
{
    Vo_SetMode(3584);
    CallThis(FUN_5b6bf0, F(BYTE*, g_voView, 12));
    CallThis(FUN_5d8630, g_voView, 0);
    CallThis(FUN_5d88c0, g_voView);
    CallThis(FUN_5ba3d0, *F(BYTE**, F(BYTE*, g_vo696ca0, 9396), 100));
}

extern "C" {

// [$L_5162f0] a button of the video options screen (this = the screen + 64): 1 applies (or asks to restart for
// a mode that isn't available), 2 cancels, 132 resets the options, 133 / 134 pick a lower / higher mode set,
// 135..138 the color depth
void __fastcall VideoOptions_Command(BYTE* self, void*, int id)
{
    switch (id) {
    case 132: {
        F(int, self, 324) = 3;
        Vo_Write(s_vo672998, s_vo676dd0, 3);
        F(int, self, 348) = 0;
        g_vo7208e4 = 0;
        Vo_Write(s_vo6730a0, s_vo676dc8, F(int, self, 348) + 1);
        Vo_Write(s_vo672998, s_vo676798, 1);
        const char* keys[4] = {s_vo676d18, s_vo676d20, s_vo676d28, s_vo676d30};
        for (int i = 0; i < 3; i++) {
            char section[16];
            sprintf(section, s_vo676ce4, i);
            for (int k = 0; k < 4; k++) {
                int v;
                Ini_GetInt(section, keys[k], -1, &v, g_voIni);
                if (v != -1)
                    Vo_Write(section, keys[k], -1);
            }
        }
        Vo_SetMode(0);
        if (!Vo_Pick(self)) {
            CallC(FUN_5d2ba0, (const char*)s_vo676d98, 0);
            MessageBoxA(0, s_vo676d98, s_vo676d90, 0);
            _exit(0);
        }
        Vo_Show();
        F(int, F(BYTE*, self, 316), 40) = 1;
        F(int, F(BYTE*, self, 320), 40) = 0;
        break;
    }
    case 2:
        CallVirt(self - 64, 32);
        F(int, g_vo6cec80, 372) = 0;
        CallVirt(g_vo6cec80, 36);
        F(short, self, 356) = F(short, self, 304);
        F(short, self, 358) = F(short, self, 306);
        F(int, self, 344) = F(int, self, 308);
        break;
    case 1: {
        DWORD m = F(DWORD, self, 344);
        DWORD c = m & 496;
        bool two = (m & 15) == 2;
        int mode = 0;
        bool missing = true;
        if (c == 272 && two && F(int, self, 328) == 0)
            mode = g_voMode6cecfc;
        else if (c == 288 && two && F(int, self, 332) == 0)
            mode = g_voMode6ced00;
        else if (c == 304 && two && F(int, self, 336) == 0)
            mode = g_voMode6ced04;
        else if (c == 320 && two && F(int, self, 340) == 0)
            mode = g_voMode6ced08;
        else
            missing = false;
        if (missing) {
            CallThis(FUN_519750, g_vo6cecc0, mode == 5 ? 4 : 2);
            g_vo675ab0 = 0;
            break;
        }
        CallVirt(self - 64, 32);
        F(int, g_vo6cec80, 372) = 0;
        if (g_vo696848 == self - 64)
            CallVirt(g_vo6cec80, 36);
        Vo_Write(s_vo672998, s_vo676cec, F(int, self, 344));
        Vo_Write(s_vo6730a0, s_vo676dc8, F(int, self, 348) + 1);
        Vo_Write(s_vo672998, s_vo676d04, F(short, self, 356));
        Vo_Write(s_vo672998, s_vo676d10, F(short, self, 358));
        break;
    }
    case 135:
    case 136:
    case 137:
    case 138:
        F(DWORD, self, 344) = (F(DWORD, self, 344) & 271) | (id - 134) * 16;
        break;
    case 133:
        if (F(int, self, 348) == 0)
            break;
        Vo_SetMode(0);
        F(int, self, 348)--;
        if (!Vo_Pick(self)) {
            F(int, self, 348)++;
            CallC(FUN_5d2e70, (const char*)s_vo676dd8, 0);
        }
        Vo_Show();
        if (F(int, self, 348) == 0)
            F(int, F(BYTE*, self, 316), 40) = 1;
        if (F(int, self, 348) < F(int, self, 324))
            F(int, F(BYTE*, self, 320), 40) = 1;
        break;
    case 134:
        if (F(int, self, 348) >= F(int, self, 324))
            break;
        Vo_SetMode(0);
        F(int, self, 348)++;
        if (!Vo_Pick(self)) {
            F(int, self, 348)--;
            F(int, self, 324) = F(int, self, 348);
            Vo_Write(s_vo672998, s_vo676dd0, F(int, self, 348));
        }
        Vo_Show();
        if (F(int, self, 348) > 0)
            F(int, F(BYTE*, self, 316), 40) = 0;
        if (F(int, self, 348) == F(int, self, 324))
            F(int, F(BYTE*, self, 320), 40) = 1;
        break;
    }
}

} // extern "C"

// === the game menu's commands ($L_511c50)

ASM_VAR(BYTE*, g_gm6ce4f4, $L_6ce4f4)
ASM_VAR(BYTE*, g_gm696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_gmShell, $L_6cec6c)
ASM_VAR(BYTE*, g_gm6ceca0, $L_6ceca0)
ASM_VAR(BYTE*, g_gm6ceca4, $L_6ceca4)
ASM_VAR(BYTE*, g_gm6ceca8, $L_6ceca8)
ASM_VAR(BYTE*, g_gm6cecec, $L_6cecec)
ASM_VAR(BYTE*, g_gm6cec74, $L_6cec74)
ASM_VAR(BYTE*, g_gm6cec78, $L_6cec78)
ASM_VAR(BYTE*, g_gm6cec98, $L_6cec98)
ASM_VAR(BYTE*, g_gmGame, $L_6d2078)
ASM_VAR(int, g_gm6968a4, $L_6968a4)
ASM_VAR(AsmLabel, g_gmNet696750, $L_696750)
extern "C" void FUN_569710(); KING_ALIAS("_FUN_569710", "_Messages_Clear1276")
extern "C" void FUN_51f380(); KING_ALIAS("_FUN_51f380", "@Shell_SetMode@12")
extern "C" void FUN_51ed10(); KING_ALIAS("_FUN_51ed10", "@Screen_Set372@12")
extern "C" void FUN_5ffb20(); KING_ALIAS("_FUN_5ffb20", "_Obj723d44_Delete")
extern "C" void FUN_511820(); KING_ALIAS("_FUN_511820", "_Shell_LeaveRace")
extern "C" void FUN_51e070(); KING_ALIAS("_FUN_51e070", "@Screen_SetPlayer@12")
extern "C" void FUN_51e7a0(); KING_ALIAS("_FUN_51e7a0", "@Shell51e7a0_Apply@8")
extern "C" void FUN_50bd60(); KING_ALIAS("_FUN_50bd60", "_Carat_SetScreenSize")
extern "C" void FUN_5c4340(); KING_ALIAS("_FUN_5c4340", "@Rec_Close@8")
extern "C" void FUN_5e4250(); KING_ALIAS("_FUN_5e4250", "@Windows_SetActive@12")
extern "C" void FUN_533590(); KING_ALIAS("_FUN_533590", "@Car_PlaceOnRoad@12")
extern "C" void FUN_53b3c0(); KING_ALIAS("_FUN_53b3c0", "@King_53b3c0@12")
extern "C" void FUN_4eb600(); KING_ALIAS("_FUN_4eb600", "@Ramp_SetValue@12")
extern "C" void FUN_516e40(); KING_ALIAS("_FUN_516e40", "@Screen_SetState@12")
extern "C" void FUN_517a40(); KING_ALIAS("_FUN_517a40", "@Screen_SetStateB@12")
extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_6021b0(); KING_ALIAS("_FUN_6021b0", "_Dp_Send")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")

// closes the screen at 6ce4f4 and the messages
static void Gm_CloseScreen()
{
    if (BYTE* o = g_gm6ce4f4)
        CallVirt(o, 0, 1);
    g_gm6ce4f4 = 0;
    F(DWORD, g_gm696ca0, 16) |= 3;
    CallC(FUN_569710);
}

// sends network message 952 with 3 bytes
static void Gm_Send952(BYTE b0)
{
    BYTE msg[32];
    CallThis(FUN_602be0, msg);
    BYTE data[4] = {b0, 0, 0, 0};
    CallThis(FUN_603110, msg, 952, data, 3);
    CallC(FUN_6021b0, F(BYTE*, msg, 4), F(BYTE*, msg, 8) - F(BYTE*, msg, 4));
    if (F(int, g_gmShell, 1344))
        CallC(FUN_5ffb20);
    CallThis(FUN_602e80, msg);
}

static void Gm_Resume()
{
    CallC(FUN_50bd60);
    CallC(FUN_4e12b0, 1);
}

static void Gm_HideMenu(BYTE* self)
{
    CallVirt(F(BYTE*, self, 2856), 32);
}

extern "C" {

// [$L_511c50] a command of the game menu (1 when handled): 169 / 163 leave the game (to the menu, the network
// lobby or the race end), 167 / 110 two submenus, 168 the replay, 166 continue (paying the tow in a
// race), 164 / 165 two more screens, 27 closes the menu
int __fastcall GameMenu_Command(BYTE* self, void*, int id)
{
    int mode = F(int, g_gmShell, 1316);
    switch (id) {
    case 169:
    case 163:
        Gm_HideMenu(self);
        mode = F(int, g_gmShell, 1316);
        if (mode == 0) {
            CallThis(FUN_51ed10, g_gm6cecec, id == 169 ? 2 : 1);
            return 1;
        }
        Gm_CloseScreen();
        if (mode == 1) {
            CallThis(FUN_51f380, g_gm6ceca8, id == 169 ? 6 : 4);
            Gm_Send952(4);
            return 1;
        }
        if (id == 163)
            CallC(FUN_511820);
        if (F(int, g_gmShell, 1344))
            CallC(FUN_5ffb20);
        if (id == 169)
            g_gm6968a4 = 1;
        return 1;
    case 167:
    case 110:
        Gm_HideMenu(self);
        if (F(int, g_gmShell, 1316) == 0) {
            if (id == 167)
                CallThis(FUN_51e070, g_gm6ceca0, 0);
            else
                CallThis(FUN_51e7a0, g_gm6ceca4);
            return 1;
        }
        Gm_CloseScreen();
        CallVirt(g_gm696ca0, 36);
        Gm_Resume();
        return 1;
    case 168:
        if (mode == 0) {
            Gm_CloseScreen();
            CallVirt(g_gm696ca0, 36);
            CallThis(FUN_5c4340, self);
            F(int, self, 3024) = 1;
            Gm_Resume();
            if (F(int, g_gmShell, 1344))
                CallC(FUN_5ffb20);
        } else if (mode == 1) {
            Gm_CloseScreen();
            CallThis(FUN_51f380, g_gm6ceca8, 3);
            Gm_Send952(3);
        }
        Gm_HideMenu(self);
        CallThis(FUN_5e4250, (BYTE*)g_gmNet696750, self);
        return 1;
    case 166:
        Gm_CloseScreen();
        CallVirt(g_gm696ca0, 36);
        if (F(int, g_gmShell, 1316) != 0 && F(int, g_gm6cec98, 384) == 2) {
            CallThis(FUN_533590, g_gmGame, 0);
            CallThis(FUN_53b3c0, g_gmGame, 1.0f);
            CallVirt(F(BYTE*, g_gmGame, 616), 116);
            CallVirt(F(BYTE*, g_gmGame, 616), 120);
            BYTE* ramp = F(BYTE*, F(BYTE*, g_gmGame, 616), 21600) + 16112;
            float v = (float)Ramp_High(ramp, 0);
            CallThis(FUN_4eb600, F(BYTE*, F(BYTE*, g_gmGame, 616), 21600) + 16112, v);
            Gm_HideMenu(self);
            CallThis(FUN_5e4250, (BYTE*)g_gmNet696750, self);
            Gm_Resume();
            return 1;
        }
        Shell_PayRepair(Player_LoadFactor(1000.0), 9);
        Gm_HideMenu(self);
        Gm_Resume();
        return 1;
    case 164:
        Gm_HideMenu(self);
        CallThis(FUN_516e40, g_gm6cec78, self);
        return 1;
    case 165:
        Gm_HideMenu(self);
        CallThis(FUN_517a40, g_gm6cec74, self);
        return 1;
    case 27:
        if (F(BYTE*, self, 2856)) {
            Gm_CloseScreen();
            CallVirt(g_gm696ca0, 36);
            Gm_HideMenu(self);
            Gm_Resume();
        }
        return 1;
    }
    return 0;
}

} // extern "C"

// === the video options window (FUN_51c980)

ASM_VAR(RECT, g_vw720938, $L_720938)
ASM_VAR(BYTE*, g_vwFont, $L_6ced24)
ASM_VAR(BYTE*, g_vw6ce3ac, $L_6ce3ac)
ASM_VAR(AsmLabel, g_vwStyle6ce390, $L_6ce390)
ASM_VAR(AsmLabel, g_vwStyle6ce408, $L_6ce408)
ASM_VAR(AsmLabel, g_vwStyle6ce3b8, $L_6ce3b8)
ASM_VAR(AsmLabel, g_vwStyle6ceb38, $L_6ceb38)
ASM_VAR(AsmLabel, g_vwStyle6ce988, $L_6ce988)
ASM_VAR(AsmLabel, g_vwVtbl64e47c, $L_64e47c)
ASM_VAR(AsmLabel, g_vwVtbl64e43c, $L_64e43c)
ASM_VAR(AsmLabel, g_vwVtbl64dc50, $L_64dc50)
ASM_VAR(AsmLabel, s_vw676c9c, $L_676c9c)
ASM_VAR(AsmLabel, s_vw677648, $L_677648)
ASM_VAR(AsmLabel, s_vw67663c, $L_67663c)
ASM_VAR(AsmLabel, s_vw677640, $L_677640)
ASM_VAR(AsmLabel, s_vw676cfc, $L_676cfc)
ASM_VAR(AsmLabel, s_vw677638, $L_677638)
ASM_VAR(AsmLabel, s_vw677630, $L_677630)
ASM_VAR(AsmLabel, s_vw67761c, $L_67761c)
ASM_VAR(AsmLabel, s_vw67760c, $L_67760c)
ASM_VAR(AsmLabel, s_vw677604, $L_677604)
ASM_VAR(AsmLabel, s_vw6775f0, $L_6775f0)
ASM_VAR(AsmLabel, s_vw6775e4, $L_6775e4)
ASM_VAR(AsmLabel, s_vw6775dc, $L_6775dc)
ASM_VAR(AsmLabel, s_vw676c50, $L_676c50)
ASM_VAR(AsmLabel, s_vw676c58, $L_676c58)
ASM_VAR(AsmLabel, s_vw6775d4, $L_6775d4)
ASM_VAR(AsmLabel, s_vw6775cc, $L_6775cc)
ASM_VAR(AsmLabel, s_vw6775c4, $L_6775c4)
extern "C" void FUN_5d8900(); KING_ALIAS("_FUN_5d8900", "@ControlWindow_Construct@40")
extern "C" void FUN_5bd5a0(); KING_ALIAS("_FUN_5bd5a0", "@Style_Assign@12")
extern "C" void FUN_5bd5e0(); KING_ALIAS("_FUN_5bd5e0", "@Control_Construct@16")
extern "C" void FUN_50ef60(); KING_ALIAS("_FUN_50ef60", "_Carat_MoveWindow")
extern "C" void FUN_5bdbf0(); KING_ALIAS("_FUN_5bdbf0", "@Ctl_AddChild@16")
extern "C" void FUN_5c0550(); KING_ALIAS("_FUN_5c0550", "@ListBox_Construct@20")
extern "C" void FUN_5c07c0(); KING_ALIAS("_FUN_5c07c0", "@List_Layout@12")
extern "C" void FUN_50f300(); KING_ALIAS("_FUN_50f300", "_Shell_SkinScrollBar")
extern "C" void FUN_5bdd70(); KING_ALIAS("_FUN_5bdd70", "@Ctl_AutoSize@8")
extern "C" void FUN_50e830(); KING_ALIAS("_FUN_50e830", "_Shell_MakeTexButton")
extern "C" void FUN_50efb0(); KING_ALIAS("_FUN_50efb0", "_Shell_MakeMaskSlider")
extern "C" void FUN_5bdc80(); KING_ALIAS("_FUN_5bdc80", "@Control_FitSize@8")

static void Vw_Styles(BYTE* c, void* s0, void* s1, void* s2, void* s3)
{
    CallThis(FUN_5bd5a0, c + 68, s0);
    CallThis(FUN_5bd5a0, c + 104, s1);
    CallThis(FUN_5bd5a0, c + 140, s2);
    CallThis(FUN_5bd5a0, c + 176, s3);
}

// a 296-byte panel control (flags)
static BYTE* Vw_Panel(int flags)
{
    BYTE* c = (BYTE*)operator new(296);
    if (!c)
        return 0;
    CallThis(FUN_5bd5e0, c, (const char*)s_vw67663c, flags);
    F(int, c, 288) = 0;
    F(int, c, 292) = 0;
    F(void*, c, 0) = g_vwVtbl64dc50;
    return c;
}

static void Vw_Add(BYTE* parent, BYTE* child)
{
    CallThis(FUN_5bdbf0, parent, child, 0);
}

// moves the rectangle at +44 of c to (x, y), keeping its size
static void Vw_MoveTo(BYTE* c, int x, int y)
{
    int* r = (int*)(c + 44);
    r[2] += x - r[0];
    r[3] += y - r[1];
    r[0] = x;
    r[1] = y;
}

static BYTE* Vw_Window(const char* name, int id, int x, int y)
{
    return CallC<BYTE*>(FUN_50ef60, name, id, x, y);
}

static BYTE* Vw_Button(const char* name, int id, int x, int y, int flag)
{
    return CallC<BYTE*>(FUN_50e830, name, id, x, y, flag);
}

static BYTE* Vw_Slider(short* value, int y, int id)
{
    BYTE* s = CallC<BYTE*>(FUN_50efb0, (const char*)s_vw676c50, (const char*)s_vw676c58, value, 350, y, 0, 187);
    return s;
}

extern "C" {

// [FUN_51c980] constructor of the video options window (parent): the mode list (+396), the depth buttons, the
// brightness/contrast/gamma sliders (+400/+402/+404 from the ini), and the OK/cancel buttons
BYTE* __fastcall VideoOptions_Construct(BYTE* self, void*, BYTE* parent)
{
    RECT r = g_vw720938;
    CallThis(FUN_5d8900, self, parent, r, 4, g_vwFont, 0);
    F(void*, self, 0) = g_vwVtbl64e47c;
    F(void*, self, 64) = g_vwVtbl64e43c;
    BYTE* o = (BYTE*)operator new(12);
    if (o) {
        F(BYTE*, o, 0) = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_vw676c9c);
        F(int, o, 4) = 0;
        F(int, o, 8) = 0;
    }
    F(BYTE*, self, 52) = o;
    F(BYTE*, self, 48) = o;
    g_vw6ce3ac = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_vw677648);
    CallThis(FUN_5bd5a0, self + 132, (void*)g_vwStyle6ce390);
    CallThis(FUN_5bd5a0, self + 168, (void*)g_vwStyle6ce390);
    CallThis(FUN_5bd5a0, self + 204, (void*)g_vwStyle6ce390);
    CallThis(FUN_5bd5a0, self + 240, (void*)g_vwStyle6ce390);

    BYTE* root = Vw_Panel(0);
    Vw_Add(root, Vw_Window(s_vw677640, 2, 323, 485));
    int v;
    Ini_GetInt(s_vo672998, s_vw676cfc, 50, &v, g_voIni);
    F(short, self, 400) = (short)v;
    Ini_GetInt(s_vo672998, s_vw677638, 50, &v, g_voIni);
    F(short, self, 402) = (short)v;
    Ini_GetInt(s_vo672998, s_vw677630, 60, &v, g_voIni);
    F(short, self, 404) = (short)v;

    BYTE* modes = Vw_Panel(4);
    g_vw6ce3ac = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_vw67761c);
    Vw_Styles(modes, g_vwStyle6ce408, g_vwStyle6ce390, g_vwStyle6ce408, g_vwStyle6ce408);
    BYTE* lb = (BYTE*)operator new(352);
    lb = lb ? CallThis<BYTE*>(FUN_5c0550, lb, (const char*)s_vw67760c, 8, 50) : 0;
    F(BYTE*, self, 396) = lb;
    Vw_Styles(lb, g_vwStyle6ce408, g_vwStyle6ce3b8, g_vwStyle6ceb38, g_vwStyle6ce988);
    F(int, F(BYTE*, F(BYTE*, self, 396), 340), 28) = 116;
    F(int, F(BYTE*, self, 396), 28) = 116;
    Vw_MoveTo(F(BYTE*, self, 396), 315, 407);
    F(int, F(BYTE*, self, 396), 348) = 41;
    CallThis(FUN_5c07c0, F(BYTE*, self, 396), 10);
    CallC(FUN_50f300, F(BYTE*, F(BYTE*, self, 396), 340), 59);
    Vw_Add(modes, F(BYTE*, self, 396));
    BYTE* w = Vw_Window(s_vw677604, 110, 323, 355);
    F(BYTE*, self, 388) = w;
    Vw_Add(modes, w);
    Vw_MoveTo(modes, 300, 353);
    CallThis(FUN_5bdd70, modes);
    Vw_Add(root, modes);

    BYTE* opts = Vw_Panel(4);
    F(BYTE*, self, 392) = opts;
    g_vw6ce3ac = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_vw6775f0);
    Vw_Styles(opts, g_vwStyle6ce408, g_vwStyle6ce390, g_vwStyle6ce408, g_vwStyle6ce408);
    BYTE* p = Vw_Panel(0);
    Vw_Add(p, Vw_Button(s_vw6775e4, 274, 444, 301, 0));
    Vw_Add(p, Vw_Button(s_vw6775dc, 273, 350, 301, 1));
    Vw_Add(opts, p);
    p = Vw_Panel(0);
    BYTE* s = Vw_Slider(&F(short, self, 404), 271, 0);
    Vw_Add(p, s);
    F(int, s, 28) = 115;
    s = Vw_Slider(&F(short, self, 402), 241, 0);
    Vw_Add(p, s);
    F(int, s, 28) = 114;
    s = Vw_Slider(&F(short, self, 400), 211, 0);
    Vw_Add(p, s);
    F(int, s, 28) = 113;
    Vw_Add(opts, p);
    p = Vw_Panel(0);
    Vw_Add(p, Vw_Button(s_vw6775d4, 112, 444, 174, 1));
    Vw_Add(p, Vw_Button(s_vw6775cc, 111, 350, 174, 0));
    Vw_Add(p, Vw_Window(s_vw6775c4, 109, 323, 120));
    Vw_Add(opts, p);
    Vw_MoveTo(opts, 300, 92);
    CallThis(FUN_5bdd70, opts);
    Vw_Add(root, opts);
    CallThis(FUN_5bdc80, root);
    Vw_Add(F(BYTE*, self, 56), root);
    F(int, self, 368) = 0;
    return self;
}

} // extern "C"

// === opening a dealer's screen (FUN_524b30)

ASM_VAR(AsmLabel, s_do678070, $L_678070)
ASM_VAR(AsmLabel, s_do67806c, $L_67806c)
ASM_VAR(AsmLabel, s_do66ca48, $L_66ca48)
ASM_VAR(AsmLabel, s_do66ca40, $L_66ca40)
ASM_VAR(AsmLabel, s_do678064, $L_678064)
ASM_VAR(AsmLabel, s_do67805c, $L_67805c)
ASM_VAR(AsmLabel, s_do67804c, $L_67804c)
ASM_VAR(AsmLabel, s_do678040, $L_678040)
ASM_VAR(AsmLabel, g_doOffers675c50, $L_675c50)  // 4 offers per dealer type: an id, -1 for none
extern "C" void FUN_4315f0(); KING_ALIAS("_FUN_4315f0", "_Ai_QueryBribeSum")
extern "C" void FUN_50bda0(); KING_ALIAS("_FUN_50bda0", "_Surface_Copy16")

// the player picture name prefix of player id
static const char* Do_Prefix(int id)
{
    BYTE info[192];
    CallC(FUN_43fc40, id, info);
    return F(int, info, 184) == 1 ? s_do678070 : s_do67806c;
}

static BYTE* Do_Find3(const char* fmt, const char* a, int n, const char* b)
{
    char buf[64];
    sprintf(buf, fmt, a, n, b);
    return CallC<BYTE*>(FUN_5ef3d0, buf);
}

static BYTE* Do_Find2(const char* fmt, int n, const char* b)
{
    char buf[64];
    sprintf(buf, fmt, n, b);
    return CallC<BYTE*>(FUN_5ef3d0, buf);
}

extern "C" {

// [FUN_524b30] opens the screen of dealer id: its pictures by dealer type (+368) and player kind, the clock
// (+416), and per offer button whether it is available (bribe sums, or a vehicle the dealer can sell) and
// whether it is the current one (+904..)
void __fastcall Dealer_Open(BYTE* self, void*, int id)
{
    CallThis(FUN_5d8610, self);
    F(int, self, 424) = id;
    int player = F(int, F(BYTE*, g_vdGame, 616), 20636);
    char prefix[36];
    strcpy(prefix, Do_Prefix(id));
    char kind[96] = "";
    int first = 0, step = 4;
    switch (F(DWORD, self, 368)) {
    case 0:
        strcpy(kind, s_do66ca48);
        first = 1;
        break;
    case 1:
        strcpy(kind, s_do66ca40);
        first = 7;
        break;
    case 2:
        strcpy(kind, s_do678064);
        first = 13;
        step = 3;
        break;
    case 3:
        strcpy(kind, s_do67805c);
        first = 18;
        step = 3;
        break;
    }
    for (int n = 1; n <= 5; n++)
        F(BYTE*, self, 916 + n * 4) = Do_Find3(s_do67804c, kind, n, prefix);
    if (BYTE* o = F(BYTE*, self, 920))
        CallThis(FUN_5fdaf0, o, 1.0f, 1.0f);
    strcpy(prefix, Do_Prefix(player));
    F(BYTE*, self, 940) = Do_Find2(s_do678040, first, prefix);
    F(BYTE*, self, 944) = Do_Find2(s_do678040, first + 1, prefix);
    F(BYTE*, self, 948) = Do_Find2(s_do678040, first + 2, prefix);
    F(BYTE*, self, 952) = Do_Find2(s_do678040, first + 3, prefix);
    F(BYTE*, self, 956) = Do_Find2(s_do678040, first + step, prefix);
    F(BYTE*, self, 960) = Do_Find2(s_do678040, first + step + 1, prefix);
    F(BYTE*, self, 964) = Do_Find2(s_do678040, 23, prefix);
    F(BYTE*, self, 968) = Do_Find2(s_do678040, 24, prefix);
    F(int, self, 372) = 0;
    F(int, self, 380) = 0;
    F(double, self, 416) = Timer_Now();
    if (BYTE* c = F(BYTE*, self, 392))
        F(int, c, 40) = 1;
    F(int, F(BYTE*, self, 388), 40) = 1;
    F(int, F(BYTE*, self, 384), 40) = 1;
    F(int, self, 916) = 0;
    F(int, self, 912) = 0;
    F(int, self, 908) = 0;
    F(int, self, 904) = 0;
    F(int, self, 900) = 0;
    for (int i = 0; i < 4; i++) {
        int idx = i + F(int, self, 368) * 4;
        int offer = ((int*)(char*)g_doOffers675c50)[idx];
        BYTE* ctl = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, self, 384), idx + 190);
        if (!ctl)
            continue;
        int disabled = 0;
        F(int, self, 904 + i * 4) = 0;
        if (offer != -1) {
            if (F(int, ctl, 28) != 196 && F(int, ctl, 28) != 203) {
                BYTE out[192];
                int r = CallC<int>(FUN_4315f0, player, F(int, self, 424), offer, out);
                if (r == 1)
                    disabled = 1;
                else if (r == 2)
                    F(int, self, 904 + i * 4) = 1;
            } else {
                BYTE o[64];
                char a;
                CallThis(FUN_487730, o, &a);
                for (int k = 12; k <= 44; k += 4)
                    F(int, o, k) = 0;
                F(int, o, 52) = -1;
                F(int, o, 56) = -1;
                if (CallC<int>(FUN_433660, F(int, F(BYTE*, g_vdGame, 616), 20636), F(int, self, 424), 0, o) == 3)
                    disabled = 1;
                if (unsigned size = F(BYTE*, o, 8) - F(BYTE*, o, 0))
                    CallC(FUN_44b4a0, F(BYTE*, o, 0), size);
            }
        }
        F(int, ctl, 40) = disabled;
    }
    if (F(int, self, 368) > 1) {
        int place = CallC<int>(FUN_43f8d0, F(int, self, 424));
        CallC(FUN_50bda0, F(BYTE*, F(BYTE*, g_vdImages, 668), 72), F(BYTE*, F(BYTE*, g_vdImages, place * 4 + 368), 72), 2);
    }
}

} // extern "C"

// === creating the shell screens (FUN_50c9d0)

ASM_VAR(BYTE*, g_scView69684c, $L_69684c)
ASM_VAR(BYTE*, g_sc6ceca4, $L_6ceca4)
ASM_VAR(BYTE*, g_sc6ceca0, $L_6ceca0)
ASM_VAR(BYTE*, g_sc6ceca8, $L_6ceca8)
ASM_VAR(BYTE*, g_sc6cecac, $L_6cecac)
ASM_VAR(BYTE*, g_sc6cecb0, $L_6cecb0)
ASM_VAR(BYTE*, g_sc6cecb4, $L_6cecb4)
ASM_VAR(BYTE*, g_sc6cecbc, $L_6cecbc)
ASM_VAR(BYTE*, g_sc6cecec, $L_6cecec)
ASM_VAR(BYTE*, g_sc6cecf0, $L_6cecf0)
ASM_VAR(BYTE*, g_sc6cecdc, $L_6cecdc)
ASM_VAR(BYTE*, g_sc6cece0, $L_6cece0)
ASM_VAR(BYTE*, g_sc6cece4, $L_6cece4)
ASM_VAR(BYTE*, g_sc6cece8, $L_6cece8)
ASM_VAR(BYTE*, g_sc6cec78, $L_6cec78)
ASM_VAR(BYTE*, g_sc6cec74, $L_6cec74)
ASM_VAR(BYTE*, g_sc6ced38, $L_6ced38)
ASM_VAR(AsmLabel, s_sc676268, $L_676268)
ASM_VAR(AsmLabel, s_sc676258, $L_676258)
extern "C" void FUN_5dba30(); KING_ALIAS("_FUN_5dba30", "@CGameApp_DestroyWindowNow@12")
extern "C" void FUN_5e41f0(); KING_ALIAS("_FUN_5e41f0", "@Windows_Add2@12")
extern "C" void FUN_51e340(); KING_ALIAS("_FUN_51e340", "@ShellScreen51e340_Construct@16")
extern "C" void FUN_51dab0(); KING_ALIAS("_FUN_51dab0", "@ShellSave51dab0_Construct@16")
extern "C" void FUN_51eea0(); KING_ALIAS("_FUN_51eea0", "@ShellViewer51eea0_Construct@12")
extern "C" void FUN_520240(); KING_ALIAS("_FUN_520240", "@ShellScreen520240_Construct@12")
extern "C" void FUN_520e30(); KING_ALIAS("_FUN_520e30", "@ShellColumns520e30_Construct@12")
extern "C" void FUN_5228e0(); KING_ALIAS("_FUN_5228e0", "@ShellScreen5228e0_Construct@12")
extern "C" void FUN_526470(); KING_ALIAS("_FUN_526470", "@ShellScreen526470_Construct@12")
extern "C" void FUN_51e9a0(); KING_ALIAS("_FUN_51e9a0", "@ShellScreen51e9a0_Construct@12")
extern "C" void FUN_525b50(); KING_ALIAS("_FUN_525b50", "@ShellDialog525b50_Construct@12")
extern "C" void FUN_523270(); KING_ALIAS("_FUN_523270", "@ShellRace523270_Construct@16")
extern "C" void FUN_516ea0(); KING_ALIAS("_FUN_516ea0", "@ShellSliders516ea0_Construct@12")
extern "C" void FUN_517530(); KING_ALIAS("_FUN_517530", "@ShellSound517530_Construct@12")

// replaces the screen in slot: the old window destroyed, a new one of size bytes by ctor (view[, arg]), added
// to the window list and shown (vtbl+32)
static void Sc_Make(BYTE** slot, unsigned size, void (*ctor)(), BYTE* view, int arg, bool withArg)
{
    if (*slot)
        CallThis(FUN_5dba30, (BYTE*)g_gs696750, *slot);
    BYTE* p = (BYTE*)operator new(size);
    if (p)
        p = withArg ? CallThis<BYTE*>(ctor, p, view, arg) : CallThis<BYTE*>(ctor, p, view);
    *slot = p;
    CallThis(FUN_5e41f0, (BYTE*)g_gs696750, p);
    CallVirt(*slot, 32);
}

extern "C" {

// [FUN_50c9d0] (re)creates the shell's screens on the main view and picks the menu picture by game mode
void __cdecl Shell_CreateScreens()
{
    BYTE* view = g_scView69684c;
    Sc_Make(&g_sc6ceca4, 512, FUN_51e340, view, 0, true);
    Sc_Make(&g_sc6ceca0, 520, FUN_51dab0, view, 0, true);
    Sc_Make(&g_sc6ceca8, 448, FUN_51eea0, view, 0, false);
    Sc_Make(&g_sc6cecac, 408, FUN_520240, view, 0, false);
    Sc_Make(&g_sc6cecb0, 504, FUN_520e30, view, 0, false);
    Sc_Make(&g_sc6cecb4, 368, FUN_5228e0, view, 0, false);
    Sc_Make(&g_sc6cecbc, 1040, FUN_526470, view, 0, false);
    Sc_Make(&g_sc6cecec, 376, FUN_51e9a0, view, 0, false);
    Sc_Make(&g_sc6cecf0, 440, FUN_525b50, view, 0, false);
    Sc_Make(&g_sc6cecdc, 976, FUN_523270, view, 0, true);
    Sc_Make(&g_sc6cece0, 976, FUN_523270, view, 1, true);
    Sc_Make(&g_sc6cece4, 976, FUN_523270, view, 2, true);
    Sc_Make(&g_sc6cece8, 976, FUN_523270, view, 3, true);
    Sc_Make(&g_sc6cec78, 400, FUN_516ea0, view, 0, false);
    Sc_Make(&g_sc6cec74, 400, FUN_517530, view, 0, false);
    g_sc6ced38 = CallC<BYTE*>(FUN_5ef3d0, CallC<int>(FUN_568fd0) == 2 ? (const char*)s_sc676268 : (const char*)s_sc676258);
}

} // extern "C"

// === the key settings screen (FUN_517eb0)

ASM_VAR(AsmLabel, s_ks677250, $L_677250)
ASM_VAR(AsmLabel, s_ks677238, $L_677238)
ASM_VAR(AsmLabel, s_ks677220, $L_677220)
ASM_VAR(AsmLabel, s_ks677208, $L_677208)
ASM_VAR(AsmLabel, s_ks6771f0, $L_6771f0)
ASM_VAR(AsmLabel, s_ks6771d8, $L_6771d8)
ASM_VAR(AsmLabel, s_ks6771c0, $L_6771c0)
ASM_VAR(AsmLabel, s_ks6771a8, $L_6771a8)
ASM_VAR(AsmLabel, s_ks677190, $L_677190)
ASM_VAR(AsmLabel, s_ks677178, $L_677178)
ASM_VAR(AsmLabel, s_ks677160, $L_677160)
ASM_VAR(AsmLabel, s_ks677148, $L_677148)
ASM_VAR(AsmLabel, s_ks677130, $L_677130)
ASM_VAR(AsmLabel, s_ks677118, $L_677118)
ASM_VAR(AsmLabel, s_ks677100, $L_677100)
ASM_VAR(AsmLabel, s_ks6770e8, $L_6770e8)
ASM_VAR(AsmLabel, s_ks6770cc, $L_6770cc)
ASM_VAR(AsmLabel, s_ks6770b0, $L_6770b0)
ASM_VAR(AsmLabel, s_ks677094, $L_677094)
ASM_VAR(AsmLabel, s_ks677078, $L_677078)
ASM_VAR(AsmLabel, s_ks67705c, $L_67705c)
ASM_VAR(AsmLabel, s_ks677040, $L_677040)
ASM_VAR(AsmLabel, s_ks677024, $L_677024)
ASM_VAR(AsmLabel, s_ks677008, $L_677008)
ASM_VAR(AsmLabel, s_ks676fec, $L_676fec)
ASM_VAR(AsmLabel, s_ks676fd0, $L_676fd0)
ASM_VAR(AsmLabel, s_ks676fb4, $L_676fb4)
ASM_VAR(AsmLabel, s_ks676f98, $L_676f98)
ASM_VAR(AsmLabel, s_ks676f7c, $L_676f7c)
ASM_VAR(AsmLabel, s_ks676f60, $L_676f60)
ASM_VAR(AsmLabel, s_ks676f44, $L_676f44)
ASM_VAR(AsmLabel, s_ks676f28, $L_676f28)
ASM_VAR(AsmLabel, s_ks677268, $L_677268)
ASM_VAR(AsmLabel, s_ks676f1c, $L_676f1c)
ASM_VAR(AsmLabel, g_ksVtbl64e090, $L_64e090)
ASM_VAR(AsmLabel, g_ksVtbl64e050, $L_64e050)
extern "C" void FUN_517e00(); KING_ALIAS("_FUN_517e00", "_Shell_NewScreenB")
extern "C" void FUN_5bf820(); KING_ALIAS("_FUN_5bf820", "@Button_ConstructId@20")
extern "C" void FUN_5be130(); KING_ALIAS("_FUN_5be130", "@Ctl_LayoutRow@12")
extern "C" void FUN_5bdee0(); KING_ALIAS("_FUN_5bdee0", "@Ctl_LayoutColumn@12")
static const char* const k_ksCol1[] = {s_ks677250, s_ks677238, s_ks677220, s_ks677208, s_ks6771f0, s_ks6771d8, s_ks6771c0, s_ks6771a8, s_ks677190, s_ks677178, s_ks677160, s_ks677148, s_ks677130, s_ks677118, s_ks677100, s_ks6770e8, };
static const char* const k_ksCol2[] = {s_ks6770cc, s_ks6770b0, s_ks677094, s_ks677078, s_ks67705c, s_ks677040, s_ks677024, s_ks677008, s_ks676fec, s_ks676fd0, s_ks676fb4, s_ks676f98, s_ks676f7c, s_ks676f60, s_ks676f44, s_ks676f28, };

// a column of rows made by Shell_NewScreenB for names
static BYTE* Ks_Column(const char* const* names, int n)
{
    BYTE* c = Vw_Panel(0);
    for (int i = 0; i < n; i++)
        Vw_Add(c, CallC<BYTE*>(FUN_517e00, names[i]));
    CallThis(FUN_5be130, c, -5);
    return c;
}

extern "C" {

// [FUN_517eb0] constructor of the key settings screen (parent): a button, two columns of 16 key rows, centered
BYTE* __fastcall KeysScreen_Construct(BYTE* self, void*, BYTE* parent)
{
    RECT r = g_vw720938;
    CallThis(FUN_5d8900, self, parent, r, 4, g_vwFont, 0);
    F(void*, self, 0) = g_ksVtbl64e090;
    F(void*, self, 64) = g_ksVtbl64e050;
    CallThis(FUN_5bd5a0, self + 132, (void*)g_vwStyle6ce390);
    CallThis(FUN_5bd5a0, self + 168, (void*)g_vwStyle6ce390);
    CallThis(FUN_5bd5a0, self + 204, (void*)g_vwStyle6ce390);
    CallThis(FUN_5bd5a0, self + 240, (void*)g_vwStyle6ce390);
    BYTE* root = Vw_Panel(0);
    BYTE* b = (BYTE*)operator new(288);
    b = b ? CallThis<BYTE*>(FUN_5bf820, b, (const char*)s_ks677268, 1, 1) : 0;
    Vw_Styles(b, g_vwStyle6ce408, g_vwStyle6ce3b8, g_vwStyle6ceb38, g_vwStyle6ce988);
    Vw_Add(root, b);
    BYTE* cols = Vw_Panel(0);
    Vw_Add(cols, Ks_Column(k_ksCol1, sizeof k_ksCol1 / sizeof *k_ksCol1));
    Vw_Add(cols, Ks_Column(k_ksCol2, sizeof k_ksCol2 / sizeof *k_ksCol2));
    CallThis(FUN_5bdee0, cols, 4);
    Vw_Add(root, cols);
    CallThis(FUN_5be130, root, 30);
    int half = (F(WORD, F(BYTE*, self, 12), 2) - F(int, root, 56) + F(int, root, 48)) / 2;
    F(int, root, 44) += 150;
    F(int, root, 52) += 150;
    F(int, root, 48) += half;
    F(int, root, 56) += half;
    Vw_Add(F(BYTE*, self, 56), root);
    F(BYTE*, self, 368) = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_ks676f1c);
    return self;
}

} // extern "C"

// === the main menu screen (FUN_50f990)

ASM_VAR(AsmLabel, s_mm6765c0, $L_6765c0)
ASM_VAR(AsmLabel, s_mm6765b0, $L_6765b0)
ASM_VAR(AsmLabel, s_mm6765a0, $L_6765a0)
ASM_VAR(AsmLabel, s_mm676590, $L_676590)
ASM_VAR(AsmLabel, s_mm676580, $L_676580)
ASM_VAR(AsmLabel, s_mm676570, $L_676570)
ASM_VAR(AsmLabel, s_mm676560, $L_676560)
ASM_VAR(AsmLabel, s_mm676550, $L_676550)
ASM_VAR(AsmLabel, s_mm676540, $L_676540)
ASM_VAR(AsmLabel, s_mm676530, $L_676530)
ASM_VAR(AsmLabel, s_mm676520, $L_676520)
ASM_VAR(AsmLabel, s_mm676510, $L_676510)
ASM_VAR(AsmLabel, s_mm676500, $L_676500)
ASM_VAR(AsmLabel, s_mm6764f0, $L_6764f0)
ASM_VAR(AsmLabel, s_mm6764e0, $L_6764e0)
ASM_VAR(AsmLabel, s_mm6764d0, $L_6764d0)
ASM_VAR(AsmLabel, s_mm6764c0, $L_6764c0)
ASM_VAR(AsmLabel, s_mm6764b0, $L_6764b0)
ASM_VAR(AsmLabel, s_mm6764a0, $L_6764a0)
ASM_VAR(AsmLabel, s_mm676490, $L_676490)
ASM_VAR(AsmLabel, s_mm676480, $L_676480)
ASM_VAR(AsmLabel, s_mm676470, $L_676470)
ASM_VAR(AsmLabel, s_mm676628, $L_676628)
ASM_VAR(AsmLabel, s_mm676614, $L_676614)
ASM_VAR(AsmLabel, s_mm676600, $L_676600)
ASM_VAR(AsmLabel, s_mm6765fc, $L_6765fc)
ASM_VAR(AsmLabel, s_mm6765f8, $L_6765f8)
ASM_VAR(AsmLabel, s_mm6765f4, $L_6765f4)
ASM_VAR(AsmLabel, s_mm6765f0, $L_6765f0)
ASM_VAR(AsmLabel, s_mm6765ec, $L_6765ec)
ASM_VAR(AsmLabel, s_mm6765e8, $L_6765e8)
ASM_VAR(AsmLabel, s_mm6765e4, $L_6765e4)
ASM_VAR(AsmLabel, s_mm6765e0, $L_6765e0)
ASM_VAR(AsmLabel, s_mm6765d0, $L_6765d0)
ASM_VAR(AsmLabel, g_mmVtbl64dd2c, $L_64dd2c)
ASM_VAR(AsmLabel, g_mmVtbl64dcec, $L_64dcec)
extern "C" void FUN_50c930(); KING_ALIAS("_FUN_50c930", "_Shell_NewObjRef")
extern "C" void FUN_50f650(); KING_ALIAS("_FUN_50f650", "_Shell_ButtonD")
static const char* const k_mmPics2[11] = {s_mm6765c0, s_mm6765b0, s_mm6765a0, s_mm676590, s_mm676580, s_mm676570, s_mm676560, s_mm676550, s_mm676540, s_mm676530, s_mm676520, };
static const char* const k_mmPics[11] = {s_mm676510, s_mm676500, s_mm6764f0, s_mm6764e0, s_mm6764d0, s_mm6764c0, s_mm6764b0, s_mm6764a0, s_mm676490, s_mm676480, s_mm676470, };
ASM_VAR(AsmLabel, s_mm6729a8, $L_6729a8)
static const struct { const char* name; short id, x, y; } k_mmButtons[9] = {
    {s_mm6765fc, 215, 36, 13}, {s_mm6729a8, 216, 6, 13}, {s_mm6765f8, 208, 294, 3}, {s_mm6765f4, 209, 271, 3},
    {s_mm6765f0, 213, 231, 8}, {s_mm6765ec, 210, 187, 8}, {s_mm6765e8, 212, 162, 8}, {s_mm6765e4, 211, 118, 8},
    {s_mm6765e0, 214, 80, 8},
};

// a style: zeroed, scale 0.5 at +24
static void Mm_Style(DWORD* s)
{
    memset(s, 0, 36);
    s[6] = 1056964608;
}

extern "C" {

// [FUN_50f990] constructor of the main menu screen: the button bar (game, options, network, demos, ...) in the
// bottom right, its picture by game mode, and the screen pictures (+372..+412)
BYTE* __fastcall MainMenu_Construct(BYTE* self, void*)
{
    RECT r = g_vw720938;
    CallThis(FUN_5d8900, self, g_scView69684c, r, 0, g_vwFont, 0);
    F(void*, self, 0) = g_mmVtbl64dd2c;
    F(void*, self, 64) = g_mmVtbl64dcec;
    DWORD s1[9], s2[9], s3[9], s4[9];
    Mm_Style(s1);
    Mm_Style(s2);
    Mm_Style(s3);
    Mm_Style(s4);
    CallThis(FUN_5bd5a0, s1, (void*)g_vwStyle6ce3b8);
    CallThis(FUN_5bd5a0, s2, (void*)g_vwStyle6ce408);
    CallThis(FUN_5bd5a0, s3, (void*)g_vwStyle6ce988);
    CallThis(FUN_5bd5a0, s4, (void*)g_vwStyle6ceb38);
    BYTE* o = CallC<BYTE*>(FUN_50c930);
    F(BYTE*, self, 52) = o;
    F(BYTE*, self, 48) = o;
    BYTE* bar = Vw_Panel(8);
    int mode = CallC<int>(FUN_568fd0);
    const char* pic = mode == 1 ? s_mm676628 : CallC<int>(FUN_568fd0) == 2 ? s_mm676614 : s_mm676600;
    s2[8] = (DWORD)CallC<BYTE*>(FUN_5ef3d0, pic);
    CallThis(FUN_5bd5a0, bar + 68, (void*)s2);
    CallThis(FUN_5bd5a0, bar + 104, (void*)s2);
    CallThis(FUN_5bd5a0, bar + 140, (void*)s2);
    CallThis(FUN_5bd5a0, bar + 176, (void*)s2);
    BYTE* row = Vw_Panel(0);
    for (int i = 0; i < 9; i++)
        Vw_Add(row, CallC<BYTE*>(FUN_50f650, k_mmButtons[i].name, k_mmButtons[i].id, k_mmButtons[i].x,
                                 k_mmButtons[i].y));
    Vw_Add(bar, row);
    int x = F(WORD, F(BYTE*, self, 12), 0) - 400;
    if (x < 0)
        x = 0;
    Vw_MoveTo(bar, x, F(WORD, F(BYTE*, self, 12), 2) - 70);
    Vw_Add(F(BYTE*, self, 56), bar);
    F(DWORD, self, 16) |= 4;
    F(int, self, 416) = 0;
    F(BYTE*, self, 368) = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_mm6765d0);
    const char* const* pics = CallC<int>(FUN_568fd0) == 2 ? k_mmPics2 : k_mmPics;
    for (int i = 0; i < 11; i++)
        F(BYTE*, self, 372 + i * 4) = CallC<BYTE*>(FUN_5ef3d0, pics[i]);
    return self;
}

} // extern "C"

// === the vehicle exchange screen ($L_526c90)

ASM_VAR(int, g_veY6ced10, $L_6ced10)
ASM_VAR(int, g_veNone68b990, $L_68b990)
ASM_VAR(double, g_ve64eac8, $L_64eac8)
ASM_VAR(double, g_ve64eac0, $L_64eac0)
ASM_VAR(double, g_ve64eab8, $L_64eab8)
ASM_VAR(int, g_veClass6ce338, $L_6ce338)
ASM_VAR(int, g_veClass6ce33c, $L_6ce33c)
ASM_VAR(AsmLabel, g_veImages6f2e38, $L_6f2e38)
ASM_VAR(AsmLabel, s_ve677a74, $L_677a74)
ASM_VAR(AsmLabel, s_ve66c8cc, $L_66c8cc)
extern "C" void FUN_5c0c00(); KING_ALIAS("_FUN_5c0c00", "@List_IndexOf300@8")
extern "C" void FUN_5c08f0(); KING_ALIAS("_FUN_5c08f0", "@List_IndexOf336@8")
extern "C" void FUN_43f760(); KING_ALIAS("_FUN_43f760", "_Player_Value136")
extern "C" void FUN_4771e0(); KING_ALIAS("_FUN_4771e0", "_Player_FirstEntry")
extern "C" void FUN_4771a0(); KING_ALIAS("_FUN_4771a0", "_Player_NextEntry")
extern "C" void FUN_569740(); KING_ALIAS("_FUN_569740", "_Messages_Get1288")
extern "C" void FUN_5b6f90(); KING_ALIAS("_FUN_5b6f90", "_Surface_FillRect")
extern "C" void FUN_50bda0(); KING_ALIAS("_FUN_50bda0", "_Surface_Copy16")

static int Ve_Car()
{
    int car;
    return *CallC<int*>(FUN_568f60, &car);
}

static double Ve_Money()
{
    return CallC<double>(FUN_43f760, Ve_Car());
}

static bool Ve_Class(int t)
{
    return t >= g_veClass6ce338 && t < g_veClass6ce33c;
}

extern "C" {

// [$L_526c90] draws the vehicle exchange screen on surface: the player's vehicle value and load, the money
// split bar, the other vehicles on offer (value bar, price, name) with the selected one (+968) remembered, and
// whether exchanging is possible (control +304)
void __fastcall Exchange_Draw(BYTE* self, void*, BYTE* surface)
{
    if (g_vd6ce4f4)
        CallC(FUN_5b86c0, F(BYTE*, g_vdView, 12), 0, 0, g_vd6ce4f4);
    CallThis(FUN_5c0020, self, surface);
    F(int, self, 972) = CallThis<int>(FUN_5c0c00, F(BYTE*, self, 968));
    BYTE* s = F(BYTE*, self, -52);
    int y = g_veY6ced10;
    BYTE info[192];
    CallC(FUN_43fc40, Ve_Car(), info);
    char buf[48];
    sprintf(buf, s_ve677a74, Vd_Int(F(double, info, 144)));
    CallC(FUN_5ea170, s, 208, y + 496, g_vdFont, buf, 4, 0);
    sprintf(buf, s_ve66c8cc, F(int, info, 52));
    CallC(FUN_5ea170, s, 292, g_veY6ced10 + 496, g_vdFont, buf, 4, 0);
    int bar1 = Vd_Int(Ve_Money() * g_ve64eac8 / F(double, info, 152));
    int bar2 = Vd_Int((F(double, info, 144) - Ve_Money()) * g_ve64eac8 / F(double, info, 152));
    int r[4];
    r[0] = 32;
    r[2] = bar1 + 32;
    r[1] = g_veY6ced10 + 479;
    r[3] = r[1] + 19;
    CallC(FUN_5b6f90, surface, r, 5, 0);
    r[0] = r[0] + 172 - bar2;
    r[2] = r[0] + bar2;
    CallC(FUN_5b6f90, surface, r, 6, 0);
    sprintf(buf, s_ve677a74, Vd_Int(Ve_Money()));
    CallC(FUN_5ea170, s, 35, g_veY6ced10 + 496, CallC<BYTE*>(FUN_569740), buf, 5, 0);
    sprintf(buf, s_ve677a74, Vd_Int(F(double, info, 144) - Ve_Money()));
    int w = CallThis<int>(FUN_5ea0e0, g_vdFont, buf);
    CallC(FUN_5ea170, s, r[2] - w - 5, g_veY6ced10 + 496, CallC<BYTE*>(FUN_569740), buf, 5, 0);

    int skip = CallThis<int>(FUN_5c08f0, F(BYTE*, self, 968));
    int it;
    CallC(FUN_4771e0, &it, Ve_Car());
    if (it != g_veNone68b990)
        while (skip > 0) {
            skip--;
            int n;
            it = *CallC<int*>(FUN_4771a0, &n, Ve_Car());
            if (it == g_veNone68b990)
                break;
        }
    BYTE* sel = F(BYTE*, F(BYTE*, self, 968), 300);
    const char* selName = sel ? F(const char*, sel, 12) : 0;
    int chosen = g_veNone68b990;
    for (int row = 0; it != g_veNone68b990; row += 20) {
        CallC(FUN_43fc40, it, info);
        if (strcmp(F(const char*, info, 48), selName) == 0)
            chosen = it;
        if (row < 200) {
            int q[4];
            q[0] = 89;
            q[1] = row + g_veY6ced10 + 545;
            q[3] = q[1] + 18;
            q[2] = Vd_Int(F(double, info, 88) * g_ve64eac0 + g_ve64eab8);
            CallC(FUN_5b6f90, s, q, 5, 0);
            sprintf(buf, s_ve677a74, Vd_Int(F(double, info, 96)));
            CallC(FUN_5ea170, s, 163, row + g_veY6ced10 + 563, g_vdFont, buf, 4, 0);
            CallC(FUN_5ea170, s, 228, row + g_veY6ced10 + 563, g_vdFont, F(const char*, info, 20), 4, 0);
        }
        int n;
        it = *CallC<int*>(FUN_4771a0, &n, Ve_Car());
    }
    F(int, F(BYTE*, self, 304), 40) = 1;
    CallC(FUN_43fc40, Ve_Car(), info);
    if (CallC<DWORD>(FUN_43f620, Ve_Car()) & 0x1000)
        CallC(FUN_5b8980, surface, 31, g_veY6ced10 + 420, ((BYTE**)(char*)g_veImages6f2e38)[F(int, info, 56)], 0);
    if (!Ve_Class(F(int, info, 12))) {
        if (chosen == g_veNone68b990)
            return;
        CallC(FUN_43fc40, chosen, info);
        if (!Ve_Class(F(int, info, 12)) && !(CallC<DWORD>(FUN_43f620, Ve_Car()) & 0x4000)
            && !(CallC<DWORD>(FUN_43f620, chosen) & 0x4000))
            F(int, F(BYTE*, self, 304), 40) = 0;
    }
    if (chosen == g_veNone68b990)
        return;
    int place = CallC<int>(FUN_43f8d0, chosen);
    CallC(FUN_50bda0, F(BYTE*, F(BYTE*, self, 612), 72), F(BYTE*, F(BYTE*, self, 304 + place * 4), 72), 6);
    CallC(FUN_5b86c0, s, 33, g_veY6ced10 + 761, F(BYTE*, self, 612));
}

} // extern "C"

// === the display options window (FUN_51add0)

ASM_VAR(AsmLabel, g_doVtbl64e36c, $L_64e36c)
ASM_VAR(AsmLabel, g_doVtbl64e32c, $L_64e32c)
ASM_VAR(AsmLabel, s_dw6774d0, $L_6774d0)
ASM_VAR(AsmLabel, s_dw6774bc, $L_6774bc)
ASM_VAR(AsmLabel, s_dw6774b4, $L_6774b4)
ASM_VAR(AsmLabel, s_dw6774ac, $L_6774ac)
ASM_VAR(AsmLabel, s_dw6774a4, $L_6774a4)
ASM_VAR(AsmLabel, s_dw67749c, $L_67749c)
ASM_VAR(AsmLabel, s_dw677494, $L_677494)
ASM_VAR(AsmLabel, s_dw676cf4, $L_676cf4)
ASM_VAR(AsmLabel, s_dw67748c, $L_67748c)
ASM_VAR(AsmLabel, s_dw677484, $L_677484)
ASM_VAR(AsmLabel, s_dw67747c, $L_67747c)
ASM_VAR(AsmLabel, s_dw677470, $L_677470)

static BYTE* Dw_Slider(short* value, int y)
{
    return CallC<BYTE*>(FUN_50efb0, (const char*)s_vw676c50, (const char*)s_vw676c58, value, 356, y, 0, 187);
}

extern "C" {

// [FUN_51add0] constructor of the display options window (parent): two button groups (+372 the first), the
// three sliders (+388/+390/+392 from the ini) and a caption
BYTE* __fastcall DisplayOptions_Construct(BYTE* self, void*, BYTE* parent)
{
    RECT r = g_vw720938;
    CallThis(FUN_5d8900, self, parent, r, 4, g_vwFont, 0);
    F(void*, self, 0) = g_doVtbl64e36c;
    F(void*, self, 64) = g_doVtbl64e32c;
    BYTE* o = (BYTE*)operator new(12);
    if (o) {
        F(BYTE*, o, 0) = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_vw676c9c);
        F(int, o, 4) = 0;
        F(int, o, 8) = 0;
    }
    F(BYTE*, self, 52) = o;
    F(BYTE*, self, 48) = o;
    g_vw6ce3ac = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_dw6774d0);
    CallThis(FUN_5bd5a0, self + 132, (void*)g_vwStyle6ce390);
    CallThis(FUN_5bd5a0, self + 168, (void*)g_vwStyle6ce390);
    CallThis(FUN_5bd5a0, self + 204, (void*)g_vwStyle6ce390);
    CallThis(FUN_5bd5a0, self + 240, (void*)g_vwStyle6ce390);
    BYTE* root = Vw_Panel(0);
    BYTE* opts = Vw_Panel(4);
    g_vw6ce3ac = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_dw6774bc);
    Vw_Styles(opts, g_vwStyle6ce408, g_vwStyle6ce390, g_vwStyle6ce408, g_vwStyle6ce408);
    BYTE* p = Vw_Panel(0);
    F(BYTE*, self, 372) = p;
    Vw_Add(p, Vw_Button(s_dw6774b4, 111, 348, 254, 0));
    Vw_Add(p, Vw_Button(s_dw6774ac, 112, 450, 254, 1));
    Vw_Add(opts, p);
    p = Vw_Panel(0);
    Vw_Add(p, Vw_Button(s_dw6774a4, 152, 473, 202, 0));
    Vw_Add(p, Vw_Button(s_dw67749c, 151, 402, 202, 0));
    Vw_Add(p, Vw_Button(s_dw677494, 150, 324, 202, 1));
    Vw_Add(opts, p);
    p = Vw_Panel(0);
    int v;
    Ini_GetInt(s_vo672998, s_dw676cf4, 50, &v, g_voIni);
    F(short, self, 388) = (short)v;
    Ini_GetInt(s_vo672998, s_dw67748c, 50, &v, g_voIni);
    F(short, self, 390) = (short)v;
    Ini_GetInt(s_vo672998, s_dw677484, 60, &v, g_voIni);
    F(short, self, 392) = (short)v;
    BYTE* s = Dw_Slider(&F(short, self, 392), 366);
    Vw_Add(p, s);
    F(int, s, 28) = 115;
    s = Dw_Slider(&F(short, self, 390), 336);
    Vw_Add(p, s);
    F(int, s, 28) = 114;
    s = Dw_Slider(&F(short, self, 388), 306);
    Vw_Add(p, s);
    F(int, s, 28) = 113;
    Vw_Add(p, Vw_Window(s_dw67747c, 124, 372, 412));
    Vw_Add(opts, p);
    Vw_MoveTo(opts, 310, 182);
    CallThis(FUN_5bdd70, opts);
    Vw_Add(root, opts);
    Vw_Add(root, Vw_Window(s_dw677470, 2, 323, 477));
    CallThis(FUN_5bdc80, root);
    Vw_Add(F(BYTE*, self, 56), root);
    F(int, self, 368) = 0;
    F(int, self, 384) = 0;
    return self;
}

} // extern "C"

// === the shell's info screen (FUN_519300)

ASM_VAR(AsmLabel, g_is64e25c, $L_64e25c)
ASM_VAR(AsmLabel, g_is64e21c, $L_64e21c)
ASM_VAR(AsmLabel, g_is64e160, $L_64e160)
ASM_VAR(BYTE*, g_is6ce3b0, $L_6ce3b0)
ASM_VAR(AsmLabel, g_is6ce408, $L_6ce408)
ASM_VAR(AsmLabel, g_is6ce3b8, $L_6ce3b8)
ASM_VAR(AsmLabel, g_is6ceb38, $L_6ceb38)
ASM_VAR(AsmLabel, g_is6ce988, $L_6ce988)
ASM_VAR(AsmLabel, s_is68d1c0, $L_68d1c0)
ASM_VAR(AsmLabel, s_is6773d0, $L_6773d0)
ASM_VAR(AsmLabel, s_is6773bc, $L_6773bc)
ASM_VAR(AsmLabel, s_is6773b4, $L_6773b4)
ASM_VAR(AsmLabel, s_is6773ac, $L_6773ac)
ASM_VAR(AsmLabel, s_is677398, $L_677398)
ASM_VAR(AsmLabel, s_is677390, $L_677390)
ASM_VAR(AsmLabel, s_is67737c, $L_67737c)
ASM_VAR(AsmLabel, s_is677374, $L_677374)
ASM_VAR(AsmLabel, s_is67736c, $L_67736c)
ASM_VAR(AsmLabel, s_is677358, $L_677358)
ASM_VAR(AsmLabel, s_is677350, $L_677350)
ASM_VAR(AsmLabel, s_is67733c, $L_67733c)

static void Is_Add(BYTE* p, const AsmLabel& name, int a, int x, int y)
{
    CallThis(FUN_5bdbf0, p, CallC<BYTE*>(FUN_50ef60, (const char*)name, a, x, y), 0);
}

extern "C" {

// [FUN_519300] constructor (parent, mode): a shell window (vtables 64e25c, 64e21c at +64) with a panel at
// (153, 249) holding the mode's buttons (mode 4 also a text field into +396), the picture 6ce3b0 of the mode
// and its height +372
BYTE* __fastcall InfoScreen_Construct(BYTE* s, void*, int parent, int mode)
{
    CallThis(FUN_5d8900, s, parent, g_q28_720938, g_q28_72093c, g_q28_720940, g_q28_720944, 0, g_q28_6ced24, 0);
    F(void*, s, 0) = g_is64e25c;
    F(void*, s, 64) = g_is64e21c;
    F(int, s, 376) = mode;
    DWORD* o = (DWORD*)operator new(12);
    if (o) {
        o[0] = CallC<DWORD>(FUN_5ef3d0, (const char*)s_q28_676c9c);
        o[1] = 0;
        o[2] = 0;
    }
    F(DWORD*, s, 52) = o;
    F(DWORD*, s, 48) = o;
    BYTE* p1 = Shell_NewPanel(0);
    BYTE* p2 = Shell_NewPanel(8);
    const char* pic;
    switch (mode) {
    case 0:
        Is_Add(p2, s_is6773d0, 1, 197, 96);
        pic = (const char*)s_is6773bc;
        break;
    case 1:
        Is_Add(p2, s_is6773b4, 156, 124, 96);
        Is_Add(p2, s_is6773ac, 155, 240, 96);
        pic = (const char*)s_is677398;
        break;
    case 2:
        Is_Add(p2, s_is677390, 155, 182, 96);
        pic = (const char*)s_is67737c;
        break;
    case 3:
        Is_Add(p2, s_is677374, 157, 138, 96);
        Is_Add(p2, s_is677350, 155, 226, 96);
        pic = (const char*)s_is67733c;
        break;
    case 4: {
        Is_Add(p2, s_is67736c, 1, 197, 108);
        F(BYTE, s, 396) = 0;
        BYTE* e = (BYTE*)operator new(300);
        if (e) {
            CallThis(FUN_5bd5e0, e, (const char*)s_is68d1c0, 0);
            F(void*, e, 0) = g_is64e160;
            F(BYTE*, e, 288) = s + 396;
            F(int, e, 292) = 62;
            F(int, e, 296) = 0;
        }
        F(int, e, 28) = 1;
        CallThis(FUN_5bd5a0, e + 68, (void*)g_is6ce408);
        CallThis(FUN_5bd5a0, e + 104, (void*)g_is6ce3b8);
        CallThis(FUN_5bd5a0, e + 140, (void*)g_is6ceb38);
        CallThis(FUN_5bd5a0, e + 176, (void*)g_is6ce988);
        F(int, e, 44) = 118;
        F(int, e, 48) = 86;
        F(int, e, 52) = 401;
        F(int, e, 56) = 100;
        CallThis(FUN_5bdbf0, p2, e, 0);
        pic = (const char*)s_is677358;
        break;
    }
    default:
        Is_Add(p2, s_is677374, 158, 138, 96);
        Is_Add(p2, s_is677350, 159, 226, 96);
        pic = (const char*)s_is67733c;
        break;
    }
    BYTE* img = g_is6ce3b0 = CallC<BYTE*>(FUN_5ef3d0, pic);
    F(int, s, 372) = img ? F(WORD, img, 4) : 100;
    for (int k = 0; k < 4; k++)
        CallThis(FUN_5bd5a0, p2 + 68 + k * 36, (void*)g_q28_6ce390);
    CallThis(FUN_5bdbf0, p1, p2, 0);
    CallVirt(p1, 20);
    F(int, p2, 52) += 153 - F(int, p2, 44);
    F(int, p2, 44) = 153;
    F(int, p2, 56) += 249 - F(int, p2, 48);
    F(int, p2, 48) = 249;
    CallThis(FUN_5bdbf0, F(BYTE*, s, 56), p1, 0);
    return s;
}

} // extern "C"

// === the bribe and dealer dialog's commands ($L_524530)

ASM_VAR(AsmLabel, g_bd675c50, $L_675c50)
ASM_VAR(AsmLabel, s_bd67801c, $L_67801c)
extern "C" void FUN_41d300(); KING_ALIAS("_FUN_41d300", "@Int_Copy@12")
extern "C" void FUN_4315f0(); KING_ALIAS("_FUN_4315f0", "_Ai_QueryBribeSum")
extern "C" void FUN_432560(); KING_ALIAS("_FUN_432560", "_Ai_OfferBribe")
extern "C" void FUN_433b60(); KING_ALIAS("_FUN_433b60", "_Ai_OrderVehicle")
extern "C" void FUN_5bdb50(); KING_ALIAS("_FUN_5bdb50", "@Ctl_FindId@12")
extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")

static int Bd_Copy(const void* it)
{
    int v;
    CallThis(FUN_41d300, &v, it);
    return v;
}

// the player and the dialog's partner (+360), as the AI takes them
static int Bd_Player() { return Bd_Copy(F(BYTE*, g_gamshe6d2078, 616) + 20636); }

static void Bd_Sound(BYTE* snd)
{
    if (snd)
        CallThis(FUN_5fdaf0, snd, 1.0f, 1.0f);
}

// closes the dialog (s is the +64 part of the screen) and goes back to the world
static void Bd_Close(BYTE* s)
{
    Gs_DropBackdrop();
    CallVirt(s - 64, 32);
    CallVirt(g_gamshe696ca0, 36);
    CallC(FUN_50bd60);
    CallC(FUN_4e12b0, 1);
}

// back to the offer page: the model buttons (+328, +324) on, the offers (+320) off
static void Bd_Back(BYTE* s)
{
    if (BYTE* c = F(BYTE*, s, 328))
        F(int, c, 40) = 1;
    F(int, F(BYTE*, s, 324), 40) = 1;
    F(int, F(BYTE*, s, 320), 40) = 0;
    F(int, s, 308) = 1;
}

extern "C" {

// [$L_524530] a command id of the bribe / dealer dialog s (state +308): on the offer page (1) ids 190..205 choose
// an offer (675c50; -1 leaves, others offer a bribe or ask its sum); on the model pages (2, 3) 120..122 pick
// the model and color, 206 orders, repairs or pays, 207 goes back
void __fastcall BribeDialog_Command(BYTE* s, void*, int id)
{
    int st = F(int, s, 308);
    if (st == 1) {
        if (id < 190 || id > 205)
            return;
        int k = id - 190, j = k & 3;
        F(BYTE*, s, 908) = F(BYTE*, s, 876 + j * 4);
        Bd_Sound(F(BYTE*, s, 876 + j * 4));
        BYTE* c = CallThis<BYTE*>(FUN_5bdb50, F(BYTE*, s, 320), id);
        F(BYTE*, s, 316) = c;
        F(int, c, 40) = 1;
        int v = ((const int*)g_bd675c50)[k];
        F(int, s, 836) = v;
        if (v == -1) {
            Bd_Close(s);
            return;
        }
        if (id == 196 || id == 203)
            F(int, s, 308) = 23;
        else if (F(int, s, 840 + j * 4)) {
            if (F(int, s, 304) == 0) {
                int t = Bd_Copy(s + 360);
                CallC(FUN_432560, Bd_Player(), t, v);
                Bd_Close(s);
            } else if (F(int, s, 304) == 1)
                CallC(FUN_5d2e70, (const char*)s_bd67801c, 0);
            else {
                F(int, s, 308) = 24;
                F(double, s, 352) = CallC<double>(FUN_6037e0);
            }
        } else {
            F(int, s, 308) = 22;
            if (id == 202)
                F(double, s, 344) = Player_LoadFactor(500.0);
            else {
                double sum = 0;
                int t = Bd_Copy(s + 360);
                int car;
                Player_CarId(&car);
                CallC(FUN_4315f0, car, t, v, &sum);
                F(double, s, 344) = sum;
            }
        }
        F(int, F(BYTE*, s, 320), 40) = 1;
        return;
    }
    if (st != 2 && st != 3)
        return;
    switch (id) {
    case 120:
        F(int, s, 312) = 1;
        F(int, s, 796) = (F(int, s, 796) + 1) % 27;
        break;
    case 121:
        F(int, s, 312) = 0;
        if (--F(int, s, 796) < 0)
            F(int, s, 796) += 27;
        break;
    case 122:
        F(int, s, 800) = (F(int, s, 800) + 1) % 4;
        break;
    case 206: {
        if (st == 2)
            Bd_Sound(F(BYTE*, s, 892));
        if (F(int, s, 308) == 3)
            Bd_Sound(F(BYTE*, s, 900));
        BYTE* m = F(BYTE*, s, 328);
        if (m && F(int, m, 40) != 1) {
            const char* name = Carat_TypeName(F(int, s, 796), F(int, s, 800));
            int t = Bd_Copy(s + 360);
            CallC(FUN_433b60, Bd_Player(), t, name);
        } else if (F(int, s, 836) == 64)
            Shell_PayRepair(F(double, s, 344), 10);
        else {
            int t = Bd_Copy(s + 360);
            CallC(FUN_432560, Bd_Player(), t, F(int, s, 836));
            if (F(int, s, 836) == 16) {
                Bd_Close(s);
                return;
            }
        }
        Bd_Back(s);
        break;
    }
    case 207:
        if (st == 2)
            Bd_Sound(F(BYTE*, s, 896));
        if (F(int, s, 308) == 3)
            Bd_Sound(F(BYTE*, s, 904));
        Bd_Back(s);
        if (F(int, s, 304) == 0)
            Bd_Close(s);
        break;
    }
}

} // extern "C"

// === opening the CB radio screen ($L_526080)

ASM_VAR(int, g_cb68b990, $L_68b990)
ASM_VAR(double, g_cb64b6a0, $L_64b6a0)
extern "C" void FUN_43fc40(); KING_ALIAS("_FUN_43fc40", "_Ai_GetPlayerData")
extern "C" void FUN_50bd30(); KING_ALIAS("_FUN_50bd30", "_Carat_UpdateScreenSize")
extern "C" void FUN_541960(); KING_ALIAS("_FUN_541960", "@Obj541960_Self@8")
extern "C" void FUN_541970(); KING_ALIAS("_FUN_541970", "@Obj541970_First@8")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")

extern "C" {

// [$L_526080] opens the CB radio screen s (thiscall): grabs the backdrop, resets the keys and the screen, then
// finds the nearest driver within 64b6a0 of the player in each class (kind 5, kind 6, flag 128, flag 64,
// others: ids +380.., distances +400.., bits +368, the last class +372, the count +376)
void __fastcall CbRadio_Open(BYTE* s)
{
    CallThis(FUN_50c010, &g_6ce4f4);
    CallThis(FUN_530020, g_6d1948);
    CallC(FUN_50bd30);
    CallC(FUN_4e12b0, 0);
    CallThis(FUN_5d8610, s);
    F(int, s, 368) = 0;
    F(int, s, 376) = 0;
    F(int, s, 372) = 0;
    for (int k = 0; k < 5; k++) {
        F(int, s, 380 + k * 4) = g_cb68b990;
        F(double, s, 400 + k * 8) = 100.0;
    }
    BYTE m[48];
    BYTE* me = F(BYTE*, g_gamshe6d2078, 616);
    float pos[3];
    memcpy(pos, CallThis<BYTE*>(FUN_57de90, me + 16, m) + 36, 12);
    for (BYTE* it = CallThis<BYTE*>(FUN_541970, F(BYTE*, g_gamshe6d2078, 616)); it;) {
        BYTE* o = CallThis<BYTE*>(FUN_541960, it);
        if (o != F(BYTE*, g_gamshe6d2078, 616) && F(int, CallThis<BYTE*>(FUN_541960, it), 20636) != g_cb68b990) {
            const float* q = (const float*)(CallThis<BYTE*>(FUN_57de90, CallThis<BYTE*>(FUN_541960, it) + 16, m) + 36);
            float d3[3] = {q[0] - pos[0], q[1] - pos[1], q[2] - pos[2]};
            double d = CallC<double>(FUN_5ab4d0, d3);
            if (!(d > g_cb64b6a0)) {
                BYTE m2[48], m3[48], inv[48];
                CallThis(FUN_57de90, CallThis<BYTE*>(FUN_541960, it) + 16, m2);
                CallC(FUN_5ac2e0, inv, CallThis<BYTE*>(FUN_57de90, F(BYTE*, g_gamshe6d2078, 616) + 16, m3));
                int id = F(int, CallThis<BYTE*>(FUN_541960, it), 20636);
                int t;
                CallThis(FUN_41d300, &t, &id);
                BYTE data[176];
                CallC(FUN_43fc40, t, data);
                int kind = F(int, data, 8), cls;
                if (kind == 5)
                    cls = 0;
                else if (kind == 6)
                    cls = 1;
                else if (CallC<int>(FUN_43f620, id) & 128)
                    cls = 2;
                else if (CallC<int>(FUN_43f620, id) & 64)
                    cls = 3;
                else
                    cls = 4;
                if (!(d > F(double, s, 400 + cls * 8))) {
                    F(double, s, 400 + cls * 8) = d;
                    F(int, s, 380 + cls * 4) = id;
                    F(int, s, 368) |= 1 << cls;
                    F(int, s, 372) = cls;
                    F(int, s, 376)++;
                }
            }
        }
        it = F(BYTE*, it, 8);
        if (!it)
            break;
        it -= 4;
    }
}

} // extern "C"

// === the info screen's commands ($L_51a0f0)

ASM_VAR(BYTE*, g_ic6cec80, $L_6cec80)
ASM_VAR(BYTE*, g_ic6cec98, $L_6cec98)
ASM_VAR(BYTE*, g_ic6cec94, $L_6cec94)
ASM_VAR(BYTE*, g_ic6cec9c, $L_6cec9c)
ASM_VAR(BYTE*, g_ic6cec70, $L_6cec70)
ASM_VAR(BYTE*, g_ic6cecd0, $L_6cecd0)
ASM_VAR(BYTE*, g_ic6cecc8, $L_6cecc8)
ASM_VAR(int, g_ic6968f4, $L_6968f4)
ASM_VAR(int, g_ic6968f8, $L_6968f8)
ASM_VAR(BYTE, g_ic696908, $L_696908)
ASM_VAR(int, g_ic6ce9f0, $L_6ce9f0)
ASM_VAR(AsmLabel, g_ic6ce9f4, $L_6ce9f4)
ASM_VAR(AsmLabel, g_ic723da8, $L_723da8)
ASM_VAR(AsmLabel, g_ic696750, $L_696750)
extern "C" void FUN_510bc0(); KING_ALIAS("_FUN_510bc0", "_Shell_StartGame")
extern "C" void FUN_519750(); KING_ALIAS("_FUN_519750", "@Shell_ShowWithMusic@12")
extern "C" void FUN_527950(); KING_ALIAS("_FUN_527950", "@Screen_SetChild@12")
extern "C" void FUN_5c3a80(); KING_ALIAS("_FUN_5c3a80", "@Music_Stop@8")
extern "C" void FUN_6021b0(); KING_ALIAS("_FUN_6021b0", "_Dp_Send")
extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")

static void Ic_StopMusic() { CallThis(FUN_5c3a80, (BYTE*)g_ic696750); }

// closes the screen (s is its +64 part) back to the menu 6cec80, the music stopped
static void Ic_Close(BYTE* s)
{
    CallVirt(s - 64, 32);
    CallVirt(g_ic6cec80, 36);
    Ic_StopMusic();
}

extern "C" {

// [$L_51a0f0] a command id of the info screen s (+312 its mode): 155 closes it; mode 3 157 sends the game setup
// (the options and player names, message 950) to the network and starts; mode 1 156 goes to the next screen
// (by the session GUID); mode 4 1, mode 5 158 / 159 and the other modes' 1 go on as their +304 / +328 say
void __fastcall InfoScreen_Command(BYTE* s, void*, int id)
{
    int mode = F(int, s, 312);
    if (mode == 2) {
        if (id == 155)
            Ic_Close(s);
        return;
    }
    if (mode == 3) {
        if (id == 155)
            Ic_Close(s);
        else if (id == 157 && g_ic6968f4 == 2) {
            BYTE buf[200];
            memset(buf, 0, sizeof buf);  // the original sends the unwritten bytes as they were on the stack
            BYTE* o = g_ic6cec98;
            buf[0] = F(BYTE, o, 384);
            buf[1] = F(BYTE, o, 388);
            buf[2] = F(BYTE, o, 392);
            buf[3] = F(BYTE, o, 390);
            buf[4] = g_ic696908;
            buf[5] = (BYTE)g_ic6ce9f0;
            for (int i = 0; i < g_ic6ce9f0; i++)
                strcpy((char*)buf + 10 + i * 20, (const char*)g_ic6ce9f4 + i * 16);
            BYTE msg[40];
            CallThis(FUN_602be0, msg);
            CallThis(FUN_603110, msg, 950, buf, 200);
            CallC(FUN_6021b0, F(BYTE*, msg, 4), F(BYTE*, msg, 8) - F(BYTE*, msg, 4));
            CallVirt(s - 64, 32);
            CallC(FUN_510bc0, 0, 1);
            CallThis(FUN_602e80, msg);
        }
        return;
    }
    if (mode == 1) {
        if (id == 155)
            Ic_Close(s);
        else if (id == 156) {
            CallVirt(s - 64, 32);
            Ic_StopMusic();
            if (memcmp(g_ic6cec94 + 552 + g_ic6968f8 * 32, (BYTE*)g_ic723da8, 16))
                CallThis(FUN_519750, g_ic6cecd0, 0);
            else {
                F(BYTE, g_ic6cecd0, 396) = 0;
                CallThis(FUN_519750, g_ic6cecc8, 0);
            }
        }
        return;
    }
    if (mode == 4) {
        if (id == 1) {
            CallVirt(s - 64, 32);
            CallThis(FUN_519750, g_ic6cecc8, 0);
        }
        return;
    }
    if (mode == 5) {
        if (id == 158) {
            CallVirt(s - 64, 32);
            CallC(FUN_510bc0, 0, 0);
        } else if (id == 159) {
            CallThis(FUN_527950, g_ic6cec80, g_ic6cec9c);
            CallVirt(s - 64, 32);
            CallVirt(g_ic6cec9c, 36);
        }
        return;
    }
    if (id != 1)
        return;
    int k = F(int, s, 304);
    if (k == 2 || k == 4) {
        F(DWORD, g_ic6cec70, 408) = (F(DWORD, g_ic6cec70, 408) & 0xffffff1f) | 16;
        CallVirt(s - 64, 32);
        CallVirt(g_ic6cec70, 36);
    } else if (k == 11 || k == 12) {
        CallVirt(s - 64, 32);
        CallThis(FUN_527950, g_ic6cec80, g_ic6cec94);
        Ic_StopMusic();
    } else {
        CallVirt(s - 64, 32);
        CallThis(FUN_527950, g_ic6cec80, F(BYTE*, s, 328));
    }
}

} // extern "C"

// === a shell scroll bar (FUN_50d860)

ASM_VAR(char, s_sb676310, $L_676310)
ASM_VAR(char, s_sb676308, $L_676308)
ASM_VAR(char, s_sb6762fc, $L_6762fc)
ASM_VAR(char, s_sb6762f0, $L_6762f0)
ASM_VAR(char, s_sb6762e4, $L_6762e4)
ASM_VAR(char, s_sb6762dc, $L_6762dc)
ASM_VAR(AsmLabel, s_sb68d1c0, $L_68d1c0)
ASM_VAR(AsmLabel, g_sb6ce3b8, $L_6ce3b8)
ASM_VAR(AsmLabel, g_sb6ce408, $L_6ce408)
extern "C" void FUN_568fd0(); KING_ALIAS("_FUN_568fd0", "_Game_568fd0")
extern "C" void FUN_5be700(); KING_ALIAS("_FUN_5be700", "@ScrollBar_Construct@24")
extern "C" void FUN_5be8c0(); KING_ALIAS("_FUN_5be8c0", "@ScrollBar_Layout@40")
extern "C" void FUN_5bef30(); KING_ALIAS("_FUN_5bef30", "@Ctl_SetStyles@28")

// a style (36 bytes: alpha +24 0.5) from base with the texture named fmt(prefix, part) at +32
static void Sb_Style(DWORD* st, const AsmLabel& base, const char* fmt, const char* prefix, const char* part)
{
    char name[32];
    sprintf(name, fmt, prefix, part);
    CallThis(FUN_5bd5a0, st, (void*)base);
    st[8] = CallC<DWORD>(FUN_5ef3d0, name);
}

// the bar's styles for state k (0 plain, 1 pressed, 2 disabled), named with part
static void Sb_States(BYTE* sb, DWORD* s1, DWORD* s2, DWORD* s3, const char* prefix, const char* part, int k)
{
    Sb_Style(s1, g_sb6ce3b8, &s_sb6762fc, prefix, part);
    Sb_Style(s2, g_sb6ce408, &s_sb6762f0, prefix, part);
    Sb_Style(s3, g_sb6ce408, &s_sb6762e4, prefix, part);
    CallThis(FUN_5bef30, sb, s1, s2, s3, s3, k);
}

extern "C" {

// [FUN_50d860] a horizontal scroll bar for the shell (100 steps over value at, width w, step a3): its plain,
// pressed and disabled looks from the textures named by the game mode's prefix with a / 6762dc / b
BYTE* __cdecl Shell_MakeScrollBar(const char* a, const char* b, short* at, int a3, int w)
{
    DWORD s1[9] = {0}, s2[9] = {0}, s3[9] = {0};
    s1[6] = s2[6] = s3[6] = 0x3f000000;
    const char* prefix = CallC<int>(FUN_568fd0) == 2 ? &s_sb676310 : &s_sb676308;
    BYTE* sb = (BYTE*)operator new(344);
    if (sb)
        sb = CallThis<BYTE*>(FUN_5be700, sb, (const char*)s_sb68d1c0, 0, 8, 1);
    CallThis(FUN_5be8c0, sb, (short)100, at, a3, 0, w, 12, 0, 0);
    Sb_States(sb, s1, s2, s3, prefix, a, 0);
    Sb_States(sb, s1, s2, s3, prefix, &s_sb6762dc, 1);
    Sb_States(sb, s1, s2, s3, prefix, b, 2);
    CallVirt(sb, 20);
    return sb;
}

} // extern "C"

// === the leave-race dialog (the message handler $L_51fd70)

extern "C" void FUN_598d60(); KING_ALIAS("_FUN_598d60", "@Wasm_598d60@8")
extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_6021b0(); KING_ALIAS("_FUN_6021b0", "_Dp_Send")
ASM_VAR(int, g_lr6f3428, $L_6f3428)

// sends network message id with n bytes of d
static void Lr_Send(int id, const BYTE* d, int n)
{
    BYTE m[32];
    CallThis(FUN_602be0, m);
    CallThis(FUN_603110, m, id, d, n);
    CallC(FUN_6021b0, F(BYTE*, m, 4), F(BYTE*, m, 8) - F(BYTE*, m, 4));
    CallThis(FUN_602e80, m);
}

extern "C" {

// [$L_51fd70] command of the leave-race dialog s (its window at -64; +340 the race kind): 163 leaves (in a network
// game telling the others, message 952), 168 abandons (single play: the recording closed and the world reset; a
// network host sends 954), 172 returns to the race
void __fastcall LeaveDialog_Command(BYTE* s, void*, int cmd)
{
    BYTE* w = s - 64;
    if (cmd == 163) {
        if (F(int, g_6cec6c, 1316) == 1) {
            BYTE d[3] = {6, 0, 0};
            Lr_Send(952, d, 3);
        }
        CallVirt(w, 32);
        g_lr6f3428 = 0;
        if (F(int, s, 340) == 6) {
            g_gs6968a4 = 1;
            return;
        }
        Shell_LeaveRace();
    } else if (cmd == 168) {
        int net = F(int, g_6cec6c, 1316);
        if (!net) {
            CallThis(FUN_5c4340, g_696ca0);
            CallVirt(g_696ca0, 140);
            CallVirt(g_696ca0, 36);
            CallVirt(w, 32);
            g_lr6f3428 = 0;
            return;
        }
        int kind = F(int, s, 340);
        if (!kind || kind == 5) {
            CallVirt(w, 32);
            Shell_LeaveRace();
        } else if (net == 1) {
            CallVirt(w, 32);
            CallVirt(g_696ca0, 36);
            F(int, g_696ca0, 3024) = 1;
            BYTE d[5] = {0, 0, 0, 0, 0};    // the original sent 5 stack bytes nothing set
            Lr_Send(954, d, 5);
        }
        g_lr6f3428 = 0;
    } else if (cmd == 172) {
        CallVirt(w, 32);
        F(DWORD, g_696cb0, 16) |= 3;
        CallThis(FUN_598d60, (BYTE*)g_6f68b0);
        CallVirt(g_696ca0, 36);
        CallC(FUN_4e12b0, 1);
        if (F(int, g_6cec6c, 1344) && F(short, g_6cec78, 384) > 5)
            CallC(FUN_5ffa30);
    }
}

} // extern "C"

// === the filling station screen (a draw method at $L_520700)

extern "C" double __cdecl AI_BaseFactor(float x, float y, float z);
extern "C" double __cdecl Player_Value136(BYTE* id);
extern "C" void __cdecl Player_Set136(BYTE* id, DWORD lo, DWORD hi);
extern "C" int __cdecl Messages_Get1292();
extern "C" int __fastcall List_DrawAt(BYTE* self, void*, int a);
extern "C" int __fastcall Font_TextWidth(BYTE* self, void*, const char* s);
extern "C" void __cdecl Text_Draw(int a, int b, int c, BYTE* font, const char* s, int d, int e);
extern "C" void Fs_Matrix1(); KING_ALIAS("_Fs_Matrix1", "@SceneObj_Matrix1@12")
extern "C" void Fs_SetValue(); KING_ALIAS("_Fs_SetValue", "@Ramp_SetValue@12")
extern "C" void Fs_FillRect(); KING_ALIAS("_Fs_FillRect", "_Surface_FillRect")
ASM_VAR(int, g_fs6ced14, $L_6ced14)
ASM_VAR(int, g_fs6ced18, $L_6ced18)
ASM_VAR(BYTE*, g_fs6ced24, $L_6ced24)
ASM_VAR(BYTE*, g_fs6d2078, $L_6d2078)
ASM_VAR(BYTE*, g_fs696ca0, $L_696ca0)
ASM_VAR(double, g_fs64e778, $L_64e778)
ASM_VAR(double, g_fs64b780, $L_64b780)
ASM_VAR(double, g_fs64b6a0, $L_64b6a0)
ASM_VAR(double, g_fs64b730, $L_64b730)
ASM_VAR(double, g_fs64b5a0, $L_64b5a0)
ASM_VAR(double, g_fs64b678, $L_64b678)
ASM_VAR(char, s_fs677a0c, $L_677a0c)
ASM_VAR(char, s_fs677a08, $L_677a08)
ASM_VAR(char, s_fs677a00, $L_677a00)
ASM_VAR(char, s_fs6779f8, $L_6779f8)
ASM_VAR(char, s_fs66efdc, $L_66efdc)
ASM_VAR(char, s_fs66c8cc, $L_66c8cc)
ASM_VAR(char, s_fs6779f0, $L_6779f0)

// v as "int.frac" (677a0c) in the game font's upper half, right-aligned at x = 176 on row y
static void Fs_Amount(BYTE* surf, BYTE* font, double v, int y)
{
    char buf[16];
    int i = (int)v;
    sprintf(buf, &s_fs677a0c, i, (int)((v - i) * g_fs64b780));
    for (char* p = buf; *p; p++)
        *p += (char)128;
    int w = Font_TextWidth(font, 0, buf);
    Text_Draw((int)surf, g_fs6ced14 + 176 - w, g_fs6ced18 + y, font, buf, 0, 0);
}

static void Fs_Text(BYTE* surf, int x, int y, const char* s)
{
    Text_Draw((int)surf, g_fs6ced14 + x, g_fs6ced18 + y, g_fs6ced24, s, 0, 0);
}

extern "C" {

// [$L_520700] draws the filling station (s is the panel inside its object at -64): the fill bar, the price at the
// player's position, the amounts and the money; while filling (+304 left) moves fuel at the frame time's rate
// into +328, and when done fills the cab's tank (+16112), charges the player and closes the panel
void __fastcall Station_Draw(BYTE* s, void*, int a)
{
    BYTE* font = (BYTE*)Messages_Get1292();
    BYTE* surf = F(BYTE*, s, -52);
    int rc[4];
    rc[0] = g_fs6ced14 + 40;
    rc[2] = (int)((F(double, s, 328) + F(double, s, 312)) * g_fs64e778 / F(double, s, 320) + rc[0]);
    rc[1] = g_fs6ced18 + 645;
    rc[3] = rc[1] + 9;
    List_DrawAt(s, 0, a);
    CallC(Fs_FillRect, surf, rc, 5, 0);
    BYTE m[48];
    Vec3 pos = F(Vec3, CallThis<BYTE*>(Fs_Matrix1, F(BYTE*, g_fs6d2078, 616) + 16, m), 36);
    Fs_Amount(surf, font, AI_BaseFactor(pos.x, pos.y, pos.z) * F(double, s, 328), 493);
    Fs_Amount(surf, font, F(double, s, 328), 543);
    Fs_Amount(surf, font, AI_BaseFactor(pos.x, pos.y, pos.z), 593);
    Fs_Text(surf, 39, 675, &s_fs677a08);
    char buf[32];
    int n = (int)F(double, s, 320);
    if (n < 10)
        sprintf(buf, &s_fs677a00, n);
    else if (F(double, s, 320) < g_fs64b6a0)
        sprintf(buf, &s_fs6779f8, n);
    else if (F(double, s, 320) < g_fs64b730)
        sprintf(buf, &s_fs66efdc, n);
    else
        sprintf(buf, &s_fs66c8cc, n);
    Fs_Text(surf, 135, 675, buf);
    double v = F(short, s, 336) * F(double, s, 320) * g_fs64b5a0 - F(double, s, 312);
    if (v > g_fs64b678) {
        sprintf(buf, &s_fs6779f0, v);
        Fs_Text(surf, 75, 724, buf);
        sprintf(buf, &s_fs6779f0, AI_BaseFactor(pos.x, pos.y, pos.z) * v);
        Fs_Text(surf, 155, 724, buf);
    }
    if (!(F(double, s, 304) > g_fs64b678))
        return;
    double dt = F(double, g_fs696ca0, 2960) * g_fs64b780;
    F(double, s, 304) -= dt;
    F(double, s, 328) += dt;
    if (F(double, s, 304) > g_fs64b678)
        return;
    F(double, s, 328) += F(double, s, 304);
    BYTE* cab = F(BYTE*, F(BYTE*, g_fs6d2078, 616), 21600);
    float full = (float)(F(double, s, 328) + F(double, s, 312));
    CallThis(Fs_SetValue, cab + 16112, *(DWORD*)&full);
    int id;
    double money = Player_Value136((BYTE*)*Player_CarId(&id));
    double left = money - AI_BaseFactor(pos.x, pos.y, pos.z) * F(double, s, 328);
    Player_Set136((BYTE*)*Player_CarId(&id), ((DWORD*)&left)[0], ((DWORD*)&left)[1]);
    CallVirt(s - 64, 32);
    CallVirt(g_fs696ca0, 36);
    F(BYTE, F(BYTE*, F(BYTE*, g_fs6d2078, 616), 21600), 9984) = 2;
    CallC(FUN_4e12b0, 1);
}

} // extern "C"

// === the input settings screen: capture and drawing (a draw method at $L_514d10)

ASM_VAR(double, g_in64b7a8, $L_64b7a8)
ASM_VAR(double, g_in64de98, $L_64de98)
ASM_VAR(double, g_in64d988, $L_64d988)
ASM_VAR(double, g_in64b678, $L_64b678)
ASM_VAR(double, g_in64b860, $L_64b860)
ASM_VAR(double, g_in6ced78, $L_6ced78)
ASM_VAR(int, g_in6ced74, $L_6ced74)
ASM_VAR(char, s_in676ca8, $L_676ca8)
extern "C" void In_Next(); KING_ALIAS("_In_Next", "@Node_Next@8")

extern "C" {

// [$L_514d10] the input screen's panel (s, inside the screen at -64): while capturing (+1216) binds newly pressed
// joystick buttons (259 + i), hat directions (291 + i) and an axis pushed from rest (300/301 + 4 * axis, +2 for
// a release; +440 then resnaps the rest values +368 from the current +304); then draws the list with each
// control's two keys (the one being bound blinking) and the device name
void __fastcall Input_Draw(BYTE* s, void*, int a)
{
    if (F(int, s, 1216)) {
        int found = -1;
        const BYTE* rec = (const BYTE*)g_6d1bb0;
        for (int k = 0; k < 8; k++, rec += 12) {
            double v = F(float, rec, 0);
            F(double, s, 304 + k * 8) = F(int, rec, 8) ? -v : v;
        }
        F(unsigned, s, 432) = 0;
        F(unsigned, s, 444) = 0;
        const BYTE* b = (const BYTE*)g_q71_6d1b4e;
        for (int i = 0; i < 12; i++)
            F(unsigned, s, 432) |= b[i * 2] ? 1u << i : 0;
        b = (const BYTE*)g_q71_6d1b8e;
        for (int i = 0; i < 4; i++)
            F(unsigned, s, 444) |= b[i * 2] ? 1u << i : 0;
        for (int i = 0; i < 12; i++) {
            unsigned bit = 1u << i;
            if ((F(unsigned, s, 432) & bit) && !(F(unsigned, s, 436) & bit))
                found = Keys_Bind(s - 64, 0, i + 259);
        }
        for (int i = 0; i < 4; i++) {
            unsigned bit = 1u << i;
            if ((F(unsigned, s, 444) & bit) && !(F(unsigned, s, 448) & bit))
                found = Keys_Bind(s - 64, 0, i + 291);
        }
        double best = F(double, s, 304) - F(double, s, 368);
        int axis = 0;
        for (int k = 1; k < 8; k++) {
            double d = F(double, s, 304 + k * 8) - F(double, s, 368 + k * 8);
            if (x87_fabs(best) < x87_fabs(d)) {
                best = d;
                axis = k;
            }
        }
        double rest = x87_fabs(F(double, s, 368 + axis * 8));
        if (rest < g_in64b7a8 && x87_fabs(best) > g_in64de98) {
            found = Keys_Bind(s - 64, 0, (best > g_in64b678 ? 300 : 301) + axis * 4);
            F(int, s, 440) = 1;
        } else if (rest > g_in64b7a8 && x87_fabs(best) > g_in64d988) {
            found = Keys_Bind(s - 64, 0, (best > g_in64b678 ? 300 : 301) + axis * 4 + 2);
            F(int, s, 440) = 1;
        }
        if (found >= 0)
            F(int, s, 452) = found;
        F(unsigned, s, 436) = F(unsigned, s, 432);
        F(unsigned, s, 448) = F(unsigned, s, 444);
        if (F(int, s, 440)) {
            memcpy(s + 368, s + 304, 64);
            F(int, s, 440) = 0;
        }
    }
    List_DrawAt(s, 0, a);
    BYTE* o = F(BYTE*, s, 460);
    int x0 = F(int, o, 44) + 200;
    int y0 = F(int, o, 48) + 153;
    BYTE* node = F(BYTE*, o, 288);
    int h = F(int, node, 56) - F(int, node, 48);
    BYTE* first = F(BYTE*, F(BYTE*, s, -8), 288);
    int lx = F(int, first, 44);
    int ly = F(int, first, 48);
    double t = CallC<double>(FUN_6037e0);
    if (t > g_in6ced78 + g_in64b860) {
        g_in6ced78 = t;
        g_in6ced74 ^= 1;
    }
    BYTE* surf = F(BYTE*, s, -52);
    const char** names = (const char**)g_keyNames675470;
    for (int i = 0; node; i++, node = CallThis<BYTE*>(In_Next, node)) {
        int key = F(int, node, 28) - 1000;
        if (F(int, s, 452) == key && g_in6ced74)
            continue;
        int k1 = F(int, s, 464 + key * 4);
        if (k1 < 0)
            continue;
        int y = y0 + i % 11 * 20 + ly + h;
        int x = lx + i / 11 * 180;
        Text_Draw((int)surf, x + x0 + 107, y, (BYTE*)g_fontMenu, names[k1], 4, 0);
        int k2 = F(int, s, 652 + key * 4);
        if (k2 >= 0)
            Text_Draw((int)surf, x + x0 + 145, y, (BYTE*)g_fontMenu, names[k2], 4, 0);
    }
    char buf[32];
    if (strlen(&s_stream6d1db8) > 29) {
        strncpy(buf, &s_stream6d1db8, 27);
        strcpy(buf + 27, &s_in676ca8);
    } else
        strcpy(buf, &s_stream6d1db8);
    buf[28] = 0;
    int w = Font_TextWidth((BYTE*)g_fontLarge, 0, buf);
    Text_Draw((int)surf, 299 + (204 - w) / 2, 178, (BYTE*)g_fontLarge, buf, 4, 0);
}

} // extern "C"

// === copying a 16-bit surface (FUN_50bda0)

// a COM method: stdcall with the object first
template <class R = void, class... A>
static R Sc_Com(BYTE* o, unsigned off, A... args)
{
    return CallStd<R>(*(void (**)())(*(BYTE**)o + off), o, args...);
}

extern "C" {

// [FUN_50bda0] copies the 16-bit DirectDraw surface src into dst: flags bit 0 flips vertically, bit 1 mirrors,
// bits 2/3/4 take every 2nd/3rd/4th pixel and row (rows past src's end stop the copy); the Lock result
int __cdecl Surface_Copy16(BYTE* dst, BYTE* src, DWORD flags)
{
    int step = 1;
    if (flags & 4)
        step = 2;
    else if (flags & 8)
        step = 3;
    else if (flags & 16)
        step = 4;
    DWORD desc[31] = {};
    desc[0] = 124;
    desc[1] = 8387054;
    int hr = Sc_Com<int>(src, 100, 0, desc, 17, 0);
    if (hr)
        return hr;
    int srcH = desc[2], srcW = desc[3], srcPitch = desc[4];
    BYTE* srcBits = (BYTE*)desc[9];
    hr = Sc_Com<int>(dst, 100, 0, desc, 33, 0);
    if (!hr) {
        int dstH = desc[2], dstW = desc[3], dstPitch = desc[4];
        BYTE* d = (BYTE*)desc[9];
        int rowStep = srcPitch * step;
        BYTE* flip = srcBits + (srcH - 1) * srcPitch;
        BYTE* s = srcBits;
        for (int r = 0, sr = 0; r < dstH && sr < srcH; r++, sr += step, flip -= rowStep, s += rowStep, d += dstPitch) {
            BYTE* p = (flags & 1) ? flip : s;
            if (!(flags & ~1u)) {
                memcpy(d, p, (srcW > dstW ? dstW : srcW) * 2);
                continue;
            }
            int mirror = flags & 2;
            if (mirror)
                p += srcW * 2 - 2;
            WORD* o = (WORD*)d;
            for (int i = 0, k = 0; i < dstW && k < srcW; i++, k += step) {
                *o++ = *(WORD*)p;
                p += mirror ? -step * 2 : step * 2;
            }
        }
        Sc_Com(dst, 128, 0);
    }
    Sc_Com(src, 128, 0);
    return hr;
}

} // extern "C"

// === reading the car types (FUN_508300)

extern "C" size_t __cdecl fwrite(const void*, size_t, size_t, FILE*);

extern "C" int __cdecl sprintf(char*, const char*, ...);
extern "C" {
float __cdecl Car_CruiseGear(int type, float load, int* gear, float* rpm, float* throttle);
double __cdecl Car_TopSpeedRatio(int type, float load, int* gear, float* rpm, float* speed, float* inverse);
double __cdecl Car_CruiseRatio(int type, float load, int* gear, float* rpm, float* speed, float* inverse,
                               float* throttle);
}
extern "C" int __cdecl _stricmp(const char*, const char*);
extern "C" int __cdecl Ini_GetFloat(const char* section, const char* key, double def, float* out, const char* file);
extern "C" int __cdecl Ini_ReadInt(const char* section, const char* key, int def, int* out, const char* file);
extern "C" int __cdecl Ini_ReadShort(const char* section, const char* key, short def, short* out, const char* file);
extern "C" int __cdecl Setup_Fatal(const char* msg);
extern "C" void Ct_ReadFlags(); KING_ALIAS("_Ct_ReadFlags", "_Ini_ReadInts")
extern "C" void Ct_stat(); KING_ALIAS("_Ct_stat", "__stat")
extern "C" void Ct_RotZ(); KING_ALIAS("_Ct_RotZ", "_Matrix_RotationZ")
extern "C" void Ct_Inverse(); KING_ALIAS("_Ct_Inverse", "_Matrix_Inverse")
extern "C" void Ct_Length(); KING_ALIAS("_Ct_Length", "_Vec3_Length")
extern "C" void Ct_NormalizeThis(); KING_ALIAS("_Ct_NormalizeThis", "@Vec3_NormalizeThis@8")
ASM_VAR(char*, g_ct674ae0, $L_674ae0)   // the car types cache file
ASM_VAR(float, g_ct674ae4, $L_674ae4)
ASM_VAR(float, g_ct674ae8, $L_674ae8)
ASM_VAR(int, g_ct6ce32c, $L_6ce32c)     // the number of car types, then the first of each group
ASM_VAR(int, g_ct6ce330, $L_6ce330)
ASM_VAR(int, g_ct6ce334, $L_6ce334)
ASM_VAR(int, g_ct6ce338, $L_6ce338)
ASM_VAR(int, g_ct6ce33c, $L_6ce33c)
ASM_VAR(int, g_ct6ce340, $L_6ce340)
ASM_VAR(int, g_ct6ce344, $L_6ce344)
ASM_VAR(int, g_ct6ce348, $L_6ce348)
ASM_VAR(BYTE, g_ct697868, $L_697868)    // the car types, 80 of 2416 bytes
ASM_VAR(float, g_ct702620, $L_702620)
ASM_VAR(float, g_ct702624, $L_702624)
ASM_VAR(float, g_ct702628, $L_702628)
ASM_VAR(float, g_ct702630, $L_702630)
ASM_VAR(float, g_ct702634, $L_702634)
ASM_VAR(float, g_ct702638, $L_702638)
ASM_VAR(float, g_ct702640, $L_702640)
ASM_VAR(float, g_ct702644, $L_702644)
ASM_VAR(float, g_ct702648, $L_702648)
ASM_VAR(BYTE, g_ct702650, $L_702650)
ASM_VAR(BYTE, g_ct7025f0, $L_7025f0)
ASM_VAR(float, g_ct64c420, $L_64c420)
ASM_VAR(float, g_ct64daf0, $L_64daf0)
ASM_VAR(double, g_ct64dae8, $L_64dae8)
ASM_VAR(float, g_ct64c904, $L_64c904)
ASM_VAR(double, g_ct64b840, $L_64b840)
ASM_VAR(float, g_ct64dac8, $L_64dac8)
ASM_VAR(float, g_ct64d7b8, $L_64d7b8)
ASM_VAR(float, g_ct64c258, $L_64c258)
ASM_VAR(double, g_ct64dae0, $L_64dae0)
ASM_VAR(float, g_ct64c264, $L_64c264)
ASM_VAR(float, g_ct64c250, $L_64c250)
ASM_VAR(float, g_ct64c338, $L_64c338)
ASM_VAR(float, g_ct64c32c, $L_64c32c)
ASM_VAR(float, g_ct64dad8, $L_64dad8)
ASM_VAR(double, g_ct64d0e8, $L_64d0e8)
ASM_VAR(double, g_ct64b670, $L_64b670)
ASM_VAR(char, s_ct671518, $L_671518)
ASM_VAR(char, s_ct675194, $L_675194)
ASM_VAR(char, s_ct675174, $L_675174)
ASM_VAR(char, s_ct67517c, $L_67517c)
ASM_VAR(char, s_ct675184, $L_675184)
ASM_VAR(char, s_ct675150, $L_675150)
ASM_VAR(char, s_ct675148, $L_675148)
ASM_VAR(char, s_ct675140, $L_675140)
ASM_VAR(char, s_ct675128, $L_675128)
ASM_VAR(char, s_ct675120, $L_675120)
ASM_VAR(char, s_ct675114, $L_675114)
ASM_VAR(char, s_ct675104, $L_675104)
ASM_VAR(char, s_ct6750f8, $L_6750f8)
ASM_VAR(char, s_ct6750ec, $L_6750ec)
ASM_VAR(char, s_ct6750ac, $L_6750ac)
ASM_VAR(char, s_ct675088, $L_675088)
ASM_VAR(char, s_ct675080, $L_675080)
ASM_VAR(char, s_ct67301c, $L_67301c)
ASM_VAR(char, s_ct675078, $L_675078)
ASM_VAR(char, s_ct675068, $L_675068)
ASM_VAR(char, s_ct67505c, $L_67505c)
ASM_VAR(char, s_ct675054, $L_675054)
ASM_VAR(char, s_ct67504c, $L_67504c)
ASM_VAR(char, s_ct675034, $L_675034)
ASM_VAR(char, s_ct67501c, $L_67501c)
ASM_VAR(char, s_ct67500c, $L_67500c)
ASM_VAR(char, s_ct674ff8, $L_674ff8)
ASM_VAR(char, s_ct674fe0, $L_674fe0)
ASM_VAR(char, s_ct674fcc, $L_674fcc)
ASM_VAR(char, s_ct674fbc, $L_674fbc)
ASM_VAR(char, s_ct674fa8, $L_674fa8)
ASM_VAR(char, s_ct674f94, $L_674f94)
ASM_VAR(char, s_ct674f8c, $L_674f8c)
ASM_VAR(char, s_ct674f78, $L_674f78)
ASM_VAR(char, s_ct674f6c, $L_674f6c)
ASM_VAR(char, s_ct674f5c, $L_674f5c)
ASM_VAR(char, s_ct674f58, $L_674f58)
ASM_VAR(char, s_ct674f4c, $L_674f4c)
ASM_VAR(char, s_ct674f48, $L_674f48)
ASM_VAR(char, s_ct674f44, $L_674f44)
ASM_VAR(char, s_ct674f40, $L_674f40)
ASM_VAR(char, s_ct674f3c, $L_674f3c)
ASM_VAR(char, s_ct674f38, $L_674f38)
ASM_VAR(char, s_ct674f0c, $L_674f0c)
ASM_VAR(char, s_ct674f00, $L_674f00)
ASM_VAR(char, s_ct674efc, $L_674efc)
ASM_VAR(char, s_ct674ef8, $L_674ef8)
ASM_VAR(char, s_ct674ee0, $L_674ee0)
ASM_VAR(char, s_ct674ec8, $L_674ec8)
ASM_VAR(char, s_ct674eb8, $L_674eb8)
ASM_VAR(char, s_ct674ea0, $L_674ea0)
ASM_VAR(char, s_ct674e90, $L_674e90)
ASM_VAR(char, s_ct674e80, $L_674e80)
ASM_VAR(char, s_ct674e54, $L_674e54)
ASM_VAR(char, s_ct674e44, $L_674e44)
ASM_VAR(char, s_ct674e34, $L_674e34)
ASM_VAR(char, s_ct674e28, $L_674e28)
ASM_VAR(char, s_ct674e20, $L_674e20)
ASM_VAR(char, s_ct674e14, $L_674e14)
ASM_VAR(char, s_ct674e08, $L_674e08)
ASM_VAR(char, s_ct674dfc, $L_674dfc)
ASM_VAR(char, s_ct674df0, $L_674df0)
ASM_VAR(char, s_ct674de8, $L_674de8)
ASM_VAR(char, s_ct674de0, $L_674de0)
ASM_VAR(char, s_ct674dd0, $L_674dd0)
ASM_VAR(char, s_ct674dc0, $L_674dc0)
ASM_VAR(char, s_ct674db0, $L_674db0)
ASM_VAR(char, s_ct674da0, $L_674da0)
ASM_VAR(char, s_ct674d84, $L_674d84)
ASM_VAR(char, s_ct674d78, $L_674d78)
ASM_VAR(char, s_ct674d6c, $L_674d6c)
ASM_VAR(char, s_ct674d54, $L_674d54)
ASM_VAR(char, s_ct674d48, $L_674d48)
ASM_VAR(char, s_ct674d34, $L_674d34)
ASM_VAR(char, s_ct674d20, $L_674d20)
ASM_VAR(char, s_ct674d0c, $L_674d0c)
ASM_VAR(char, s_ct674cfc, $L_674cfc)
ASM_VAR(char, s_ct674cec, $L_674cec)
ASM_VAR(char, s_ct674cdc, $L_674cdc)
ASM_VAR(char, s_ct674ccc, $L_674ccc)
ASM_VAR(char, s_ct674cc0, $L_674cc0)
ASM_VAR(char, s_ct674cb8, $L_674cb8)
ASM_VAR(char, s_ct674cb0, $L_674cb0)
ASM_VAR(char, s_ct674ca4, $L_674ca4)
ASM_VAR(char, s_ct674c9c, $L_674c9c)
ASM_VAR(char, s_ct674c90, $L_674c90)
ASM_VAR(char, s_ct674c88, $L_674c88)
ASM_VAR(char, s_ct674c80, $L_674c80)
ASM_VAR(char, s_ct674c74, $L_674c74)
ASM_VAR(char, s_ct674c3c, $L_674c3c)
ASM_VAR(char, s_ct674c30, $L_674c30)
ASM_VAR(char, s_ct674c20, $L_674c20)
ASM_VAR(char, s_ct674c14, $L_674c14)
ASM_VAR(char, s_ct674c08, $L_674c08)
ASM_VAR(char, s_ct674bfc, $L_674bfc)
ASM_VAR(char, s_ct674bf0, $L_674bf0)
ASM_VAR(char, s_ct674be4, $L_674be4)
ASM_VAR(char, s_ct674bd4, $L_674bd4)
ASM_VAR(char, s_ct674bc0, $L_674bc0)
ASM_VAR(char, s_ct674bb4, $L_674bb4)
ASM_VAR(char, s_ct674ba8, $L_674ba8)
ASM_VAR(char, s_ct674b98, $L_674b98)
ASM_VAR(char, s_ct6729a8, $L_6729a8)
ASM_VAR(char, s_ct674b8c, $L_674b8c)
ASM_VAR(char, s_ct6703e4, $L_6703e4)
ASM_VAR(char, s_ct674b64, $L_674b64)
ASM_VAR(char, s_ct674b4c, $L_674b4c)

#define CT_F(off) ((float*)(T + (off)))

static int Ct_Read3(const char* sec, const char* key, const void* def, BYTE* out, const char* ini)
{
    return Carat_Read3((int)sec, (int)key, (int)def, (int)out, (int)ini);
}

// out = vector (x, y, z)
static void Ct_Vec(float* v, DWORD x, DWORD y, DWORD z)
{
    ((DWORD*)v)[0] = x, ((DWORD*)v)[1] = y, ((DWORD*)v)[2] = z;
}

// v -= up * k
static void Ct_Lower(float* v, float k)
{
    float ux = g_ct702630 * k, uy = g_ct702634 * k, uz = g_ct702638 * k;
    v[0] = v[0] - ux, v[1] = v[1] - uy, v[2] = v[2] - uz;
}

// the place p (forward f turned about up by a, plus the turn's translation)
static void Ct_Turned(float* out, const float* f, float a)
{
    float m[12];
    const float* M = CallC<const float*>(Ct_RotZ, m, *(DWORD*)&a);
    out[0] = (float)((double)f[2] * M[6] + (double)f[1] * M[3] + (double)f[0] * M[0] + M[9]);
    out[1] = (float)((double)f[2] * M[7] + (double)f[1] * M[4] + (double)f[0] * M[1] + M[10]);
    out[2] = (float)((double)f[2] * M[8] + (double)f[1] * M[5] + (double)f[0] * M[2] + M[11]);
}

extern "C" {

// [FUN_508300] reads the car types from ini file path: the groups of type sections (5 kinds, their first
// indices 6ce334..6ce344) and for each type its engine, gears, wheels, body points, hit boxes, cameras, lights
// and names; then writes them with the file times to the cache 674ae0
void __cdecl CarTypes_Read(const char* path, int unused)
{
    char buf[200], section[260], key[200], key2[200], sec[32], ini[32], err[300];
    FILE* f = fopen(path, &s_ct671518);
    if (!f) {
        sprintf(buf, &s_ct675194, g_ct674ae0, path);
        Setup_Fatal(buf);
    }
    fclose(f);
    GetPrivateProfileStringA(&s_ct675174, &s_ct67517c, &s_ct675184, buf, 70, path);
    if (strcmp(buf, &s_ct675184))
        sprintf(buf, &s_ct675150, path);
    strcpy(section, &s_ct675148);
    g_ct6ce32c = 0;
    int count = 0, group = 0, n = 1;
    BYTE* T = &g_ct697868;
    for (;;) {
        sprintf(key, &s_ct675140, n);
        GetPrivateProfileStringA(section, key, &s_ct675128, buf, 30, path);
        if (!strcmp(buf, &s_ct675128)) {
            n = 0;
            group++;
            static int* const firsts[5] = {&g_ct6ce334, &g_ct6ce338, &g_ct6ce33c, &g_ct6ce340, &g_ct6ce344};
            static const char* const sections[5] = {&s_ct675120, &s_ct675114, &s_ct675104, &s_ct6750f8,
                &s_ct6750ec};
            if (group > 5)
                break;
            *firsts[group - 1] = count;
            strcpy(section, sections[group - 1]);
        } else {
            if (T + 2372 >= (BYTE*)&g_ct697868 + 80 * 2416 + 2372) {
                sprintf(err, &s_ct675088, &s_ct6750ac, 80, count + 1);
                Setup_Fatal(err);
            }
            if ((unsigned)group <= 5)
                F(int, T, 2372) = group;
            strcpy((char*)T + 2242, buf);
            strcpy((char*)T + 2272, buf);
            sprintf(key2, &s_ct675080, n);
            GetPrivateProfileStringA(section, key2, path, buf, 30, path);
            sprintf((char*)T + 2212, &s_ct67301c, buf);
            count++;
            T += 2416;
        }
        n++;
        if (T + 2372 > (BYTE*)&g_ct697868 + 80 * 2416 + 2372)
            break;
    }
    g_ct6ce32c = count;
    g_ct6ce348 = count;
    for (int i = 0; i < g_ct6ce32c; i++) {
        T = &g_ct697868 + i * 2416;
        strcpy(sec, (const char*)T + 2272);
        strcpy(ini, (const char*)T + 2212);
        Ini_GetFloat(sec, &s_ct675078, 1000.0, CT_F(96), ini);
        Ini_GetFloat(sec, &s_ct675068, 500.0, CT_F(192), ini);
        Ini_GetFloat(sec, &s_ct67505c, 6000.0, CT_F(200), ini);
        Ini_GetFloat(sec, &s_ct675054, 0.3, CT_F(100), ini);
        Ini_GetFloat(sec, &s_ct67504c, 0.3, CT_F(104), ini);
        Ini_GetFloat(sec, &s_ct675034, 0.2, CT_F(184), ini);
        Ini_GetFloat(sec, &s_ct67501c, 0.4, CT_F(188), ini);
        Ini_GetFloat(sec, &s_ct67500c, 0.3, CT_F(172), ini);
        Ini_GetFloat(sec, &s_ct674ff8, 0.3, CT_F(176), ini);
        Ini_GetFloat(sec, &s_ct674fe0, 0.8, CT_F(180), ini);
        Ini_GetFloat(sec, &s_ct674fcc, 1200.0, CT_F(268), ini);
        *CT_F(268) = *CT_F(268) * g_ct64c420;
        Ini_GetFloat(sec, &s_ct674fbc, 5.0, CT_F(204), ini);
        Ini_GetFloat(sec, &s_ct674fa8, -4.0, CT_F(208), ini);
        F(int, T, 212) = 0;
        F(int, T, 144) = 1;
        for (int g = 2; g < 15; g++) {
            float* r = CT_F(216 + (g - 2) * 4);
            sprintf(buf, &s_ct674f94, g - 1);
            Ini_GetFloat(sec, buf, -100.0, r, ini);
            if (*r < g_ct64daf0) {
                *r = (float)(r[-1] * g_ct64dae8);
                if (F(int, T, 144) == 1)
                    F(int, T, 144) = g > 11 ? 11 : g;
            }
        }
        Ini_GetFloat(sec, &s_ct674f8c, *CT_F(192) * g_ct64c904, CT_F(108), ini);
        Ini_GetFloat(sec, &s_ct674f78, g_ct674ae4, CT_F(112), ini);
        *CT_F(196) = (float)(*CT_F(192) * g_ct64b840 / (*CT_F(268) + *CT_F(268)));
        *CT_F(128) = Car_CruiseGear(i, *CT_F(96), &F(int, T, 144), CT_F(136), CT_F(140));
        float* top = CT_F(168);
        F(DWORD, T, 168) = 0x42480000;
        Ini_GetFloat(sec, &s_ct674f6c, 50.0, top, ini);
        float* mass = CT_F(160);
        *mass = *CT_F(96) * g_ct64dac8;
        Ini_GetFloat(sec, &s_ct674f5c, (double)g_ct674ae8 * *mass, mass, ini);
        *mass = *mass / g_ct674ae8;
        double tq = (double)*mass * *top;
        *CT_F(108) = (float)tq;
        Ini_GetFloat(sec, &s_ct674f8c, tq, CT_F(108), ini);
        *CT_F(112) = (float)((double)*CT_F(140) * *CT_F(108) /
            ((double)*mass * *CT_F(128) * g_ct64dac8 / (*CT_F(136) / *CT_F(268))));
        *CT_F(156) = (float)Car_TopSpeedRatio(i, *CT_F(96), &F(int, T, 148), CT_F(132), CT_F(124), CT_F(164));
        *mass = (float)Car_CruiseRatio(i, *CT_F(96), &F(int, T, 152), CT_F(136), CT_F(128), top, CT_F(140));
        *CT_F(120) = *CT_F(164) * g_ct64d7b8 / *CT_F(124);
        *CT_F(116) = *top * g_ct64d7b8 / *CT_F(128);
        float v[3];
        v[0] = g_ct702630 * g_ct64c258, v[1] = g_ct702634 * g_ct64c258, v[2] = g_ct702638 * g_ct64c258;
        Ct_Read3(sec, &s_ct674f58, v, T + 396, ini);
        Ini_GetFloat(sec, &s_ct674f4c, -0.125, CT_F(408), ini);
        *CT_F(408) = (float)(*CT_F(408) * g_ct64dae0);
        Ct_Vec(v, 0x43FA0000, 0x43480000, 0x43FA0000);
        Ct_Read3(sec, &s_ct674f48, v, T + 412, ini);
        F(int, T, 712) = 1;
        static const DWORD corners[4][3] = {{0xBF19999A, 0x3F800000, 0xBE4CCCCD}, {0x3F19999A, 0x3F800000, 0xBE4CCCCD},
            {0xBF19999A, 0xBF800000, 0xBE4CCCCD}, {0x3F19999A, 0xBF800000, 0xBE4CCCCD}};
        static const char* const ckeys[4] = {&s_ct674f44, &s_ct674f40, &s_ct674f3c, &s_ct674f38};
        int got = 0, all = 1;
        for (int k = 0; k < 4; k++) {
            Ct_Vec(v, corners[k][0], corners[k][1], corners[k][2]);
            int r = Ct_Read3(sec, ckeys[k], v, T + 424 + k * 12, ini);
            got += r;
            all = all && r ? 1 : 0;
            F(int, T, 712) = all;
        }
        if (got && !all) {
            sprintf(buf, &s_ct674f0c, ini, sec, got);
            Setup_Fatal(buf);
        }
        Carat_ReadFill((int)sec, (int)&s_ct674f00, 0x3E99999A, (int)(T + 748), (int)ini, 8);
        memset(T + 716, 0, 32);
        Carat_ReadFill((int)sec, (int)&s_ct674efc, 0x46435000, (int)(T + 780), (int)ini, 8);
        Carat_ReadFill((int)sec, (int)&s_ct674ef8, 0x3F800000, (int)(T + 812), (int)ini, 8);
        Ini_GetFloat(sec, &s_ct674ee0, 0.5, CT_F(864), ini);
        if (*CT_F(864) < g_ct64c264 || *CT_F(864) > g_ct64c250)
            F(DWORD, T, 864) = 0x3E4CCCCD;
        Ini_GetFloat(sec, &s_ct674ec8, 0.5, CT_F(868), ini);
        if (*CT_F(868) < g_ct64c264 || *CT_F(868) > g_ct64c250)
            F(DWORD, T, 868) = 0x3F000000;
        Carat_ReadFill2((int)sec, (int)&s_ct674eb8, 1, (int)(T + 300), (int)ini, 8);
        Carat_ReadFill2((int)sec, (int)&s_ct674ea0, 1, (int)(T + 364), (int)ini, 8);
        int flags[8] = {1, 1, 0, 0, 0, 0, 0, 0};
        CallC(Ct_ReadFlags, sec, &s_ct674e90, flags, T + 332, ini, 8);
        memcpy(T, &g_ct7025f0, 48);
        F(int, T, 860) = Ct_Read3(sec, &s_ct674e80, &g_ct702650, T + 36, ini);
        float inv[12];
        memcpy(T + 48, CallC<void*>(Ct_Inverse, inv, T), 48);
        if (!F(int, T, 860)) {
            sprintf(buf, &s_ct674e54, ini, sec);
            Setup_Fatal(buf);
        }
        Ct_Read3(sec, &s_ct674e44, &g_ct702650, T + 848, ini);
        Ini_GetFloat(sec, &s_ct674e34, 1.0, CT_F(844), ini);
        Ini_ReadInt(sec, &s_ct674e28, 150, &F(int, T, 1988), ini);
        Ini_ReadInt(sec, &s_ct674e20, 10000, &F(int, T, 1992), ini);
        Ini_ReadShort(sec, &s_ct674e14, 1, &F(short, T, 1996), ini);
        Ct_Vec(v, 0, 0xC0000000, 0);
        Ct_Read3(sec, &s_ct674e08, v, T + 2000, ini);
        Ct_Lower(CT_F(2000), g_ct64c338);
        Ct_Vec(v, 0, 0x40000000, 0xBF800000);
        Ct_Read3(sec, &s_ct674dfc, v, T + 2012, ini);
        Ct_Lower(CT_F(2012), g_ct64c338);
        Ini_GetFloat(sec, &s_ct674df0, 0.0, CT_F(2048), ini);
        Ct_Vec(v, 0, 0xC0400000, 0);
        Ct_Read3(sec, &s_ct674de8, v, T + 2024, ini);
        Ct_Vec(v, 0, 0x40000000, 0);
        Ct_Read3(sec, &s_ct674de0, v, T + 2036, ini);
        Ini_GetFloat(sec, &s_ct674dd0, 2.0, CT_F(2052), ini);
        F(int, T, 1396) = 0;
        for (int k = 0; k < 4; k++) {
            sprintf(buf, &s_ct674dc0, k + 1);
            if (!Ct_Read3(sec, buf, &g_ct702650, T + 1400 + F(int, T, 1396) * 12, ini))
                break;
            F(int, T, 1396)++;
        }
        F(int, T, 872) = 0;
        for (int k = 0; k < 20; k++) {
            sprintf(buf, &s_ct674db0, k + 1);
            if (!Ct_Read3(sec, buf, &g_ct702650, T + 880 + F(int, T, 872) * 12, ini))
                break;
            F(int, T, 872)++;
        }
        F(int, T, 876) = 0;
        for (int k = 0; k < 20; k++) {
            sprintf(buf, &s_ct674da0, k);
            if (!Ct_Read3(sec, buf, &g_ct702650, T + 1120 + F(int, T, 876) * 12, ini))
                break;
            F(int, T, 876)++;
        }
        float lo[3], hi[3];
        ((DWORD*)lo)[0] = ((DWORD*)lo)[1] = ((DWORD*)lo)[2] = 0x461C4000;
        ((DWORD*)hi)[0] = ((DWORD*)hi)[1] = ((DWORD*)hi)[2] = 0xC61C4000;
        F(int, T, 1772) = 0;
        for (int k = 0; k < F(int, T, 872); k++) {
            const float* p = CT_F(880 + k * 12);
            for (int c = 0; c < 3; c++) {
                if (p[c] < lo[c])
                    lo[c] = p[c];
                if (p[c] > hi[c])
                    hi[c] = p[c];
            }
            float d[3] = {p[0] - *CT_F(848), p[1] - *CT_F(852), p[2] - *CT_F(856)};
            float l = (float)CallC<double>(Ct_Length, d);
            if (l > *CT_F(1772))
                *CT_F(1772) = l;
        }
        float* ext = CT_F(1360);
        ext[0] = hi[0] - lo[0], ext[1] = hi[1] - lo[1], ext[2] = hi[2] - lo[2];
        memcpy(T + 1372, hi, 12);
        memcpy(T + 1384, lo, 12);
        if (i >= g_ct6ce330 && i < g_ct6ce334) {
            Ini_GetFloat(sec, &s_ct674d84, 400.0, CT_F(272), ini);
            memcpy(T + 288, T + 36, 12);
            *CT_F(276) = *CT_F(96) * g_ct64dac8;
            Ini_GetFloat(sec, &s_ct674d78, *CT_F(1368) * g_ct64c904, CT_F(280), ini);
            Ini_GetFloat(sec, &s_ct674d6c, *CT_F(1364) * *CT_F(1360) * g_ct64c904, CT_F(284), ini);
        } else {
            Ini_GetFloat(sec, &s_ct674d84, 6000.0, CT_F(272), ini);
            Ct_Read3(sec, &s_ct674d54, &g_ct702650, T + 288, ini);
            Ini_GetFloat(sec, &s_ct674d48, 1000.0, CT_F(276), ini);
            Ini_GetFloat(sec, &s_ct674d78, 2.0, CT_F(280), ini);
            Ini_GetFloat(sec, &s_ct674d6c, 10.0, CT_F(284), ini);
        }
        F(int, T, 1768) = 0;
        F(int, T, 1776) = F(int, T, 1772);
        for (int k = 0; k < 20; k++) {
            sprintf(buf, &s_ct674d34, k);
            float* pl = CT_F(1448 + k * 16);
            if (!Carat_ReadFill((int)sec, (int)buf, 0x3F800000, (int)pl, (int)ini, 4))
                break;
            float w = -pl[3];
            float d[3] = {pl[0] * w - *CT_F(848), pl[1] * w - *CT_F(852), pl[2] * w - *CT_F(856)};
            float l = (float)CallC<double>(Ct_Length, d);
            if (l < *CT_F(1776))
                *CT_F(1776) = l;
            F(int, T, 1768)++;
        }
        F(int, T, 1840) = 0;
        for (int k = 0; k < 5; k++) {
            sprintf(buf, &s_ct674d20, k + 1);
            if (!Ct_Read3(sec, buf, &g_ct702650, T + 1780 + F(int, T, 1840) * 12, ini))
                break;
            F(int, T, 1840)++;
        }
        Ct_Read3(sec, &s_ct674d0c, &g_ct702650, T + 1844, ini);
        v[0] = g_ct702630 * g_ct64c32c, v[1] = g_ct702634 * g_ct64c32c, v[2] = g_ct702638 * g_ct64c32c;
        Ct_Read3(sec, &s_ct674cfc, v, T + 1856, ini);
        v[0] = -g_ct702640, v[1] = -g_ct702644, v[2] = -g_ct702648;
        Ct_Read3(sec, &s_ct674cec, v, T + 1868, ini);
        v[0] = g_ct702630 * g_ct64c32c, v[1] = g_ct702634 * g_ct64c32c, v[2] = g_ct702638 * g_ct64c32c;
        Ct_Read3(sec, &s_ct674cdc, v, T + 1880, ini);
        Ct_Read3(sec, &s_ct674ccc, T + 1868, T + 1892, ini);
        CallThis(Ct_NormalizeThis, T + 1868);
        CallThis(Ct_NormalizeThis, T + 1892);
        Ct_Read3(sec, &s_ct674cc0, &g_ct702650, T + 1904, ini);
        Ct_Read3(sec, &s_ct674cb8, &g_ct702650, T + 1916, ini);
        Ct_Read3(sec, &s_ct674cb0, &g_ct702650, T + 1928, ini);
        Ct_Read3(sec, &s_ct674ca4, &g_ct702650, T + 1940, ini);
        Ct_Read3(sec, &s_ct674c9c, &g_ct702650, T + 1964, ini);
        Ct_Read3(sec, &s_ct674c90, &g_ct702650, T + 1952, ini);
        Ct_Read3(sec, &s_ct674c88, &g_ct702650, T + 1976, ini);
        char* tname = (char*)T + 2302;
        GetPrivateProfileStringA(sec, &s_ct674c80, &s_ct675128, tname, 29, ini);
        if (!strcmp(tname, &s_ct675128)) {
            GetPrivateProfileStringA(sec, &s_ct674c74, &s_ct675128, tname, 29, ini);
            sprintf(buf, &s_ct674c3c, sec, ini);
            Setup_Fatal(buf);
        }
        strcpy((char*)T + 2342, tname);
        GetPrivateProfileStringA(sec, &s_ct674c30, tname, (char*)T + 2322, 29, ini);
        F(int, T, 2148) = 0;
        F(int, T, 2144) = 0;
        GetPrivateProfileStringA(sec, &s_ct674c20, &s_ct675128, (char*)T + 2152, 29, ini);
        float a[7];
        a[0] = 50.0f, a[1] = 240.0f;
        Carat_ReadCopy((int)sec, (int)&s_ct674c14, (const int*)a, (int)a, (int)ini, 2);
        *CT_F(2064) = a[0] * g_ct64dad8;
        *CT_F(2068) = (float)(a[1] * g_ct64dae0);
        a[0] = 100.0f, a[1] = 240.0f;
        Carat_ReadCopy((int)sec, (int)&s_ct674c08, (const int*)a, (int)a, (int)ini, 2);
        *CT_F(2056) = (float)(a[0] * g_ct64d0e8);
        *CT_F(2060) = (float)(a[1] * g_ct64dae0);
        a[0] = 1.0f, a[1] = 90.0f;
        Carat_ReadCopy((int)sec, (int)&s_ct674bfc, (const int*)a, (int)a, (int)ini, 2);
        *CT_F(2072) = (float)(g_ct64b670 / a[0] * a[1] * g_ct64dae0);
        a[0] = 1.75f, a[1] = 2.0f, a[2] = 5.0f;
        Carat_ReadCopy((int)sec, (int)&s_ct674bf0, (const int*)a, (int)(T + 2076), (int)ini, 3);
        a[0] = 1.75f, a[1] = 0.5f, a[2] = 5.0f;
        Carat_ReadCopy((int)sec, (int)&s_ct674be4, (const int*)a, (int)(T + 2088), (int)ini, 3);
        static const DWORD tail4[4] = {0x3FAB851F, 0x3F8E147B, 0x3FF33333, 0xBF1C28F6};
        memcpy(a, tail4, 16);
        Carat_ReadCopy((int)sec, (int)&s_ct674bd4, (const int*)a, (int)a, (int)ini, 4);
        memcpy(T + 2100, a, 16);
        static const float seven[7] = {70.0f, 20.0f, 10.0f, 0.0f, -10.0f, -20.0f, -70.0f};
        memcpy(a, seven, 28);
        Carat_ReadCopy((int)sec, (int)&s_ct674bc0, (const int*)a, (int)(T + 2116), (int)ini, 7);
        a[0] = 0, a[1] = 0;
        Carat_ReadCopy((int)sec, (int)&s_ct674bb4, (const int*)a, (int)a, (int)ini, 2);
        float fwd[3] = {g_ct702620, g_ct702624, g_ct702628};
        Ct_Turned(CT_F(2380), fwd, (float)(a[0] * g_ct64dae0));
        float back[3] = {-g_ct702620, -g_ct702624, -g_ct702628};
        Ct_Turned(CT_F(2392), back, (float)(a[1] * g_ct64dae0));
        a[0] = 0.3f, a[1] = 0.2f, a[2] = 0.2f;
        ((DWORD*)a)[0] = 0x3E99999A, ((DWORD*)a)[1] = 0x3E4CCCCD, ((DWORD*)a)[2] = 0x3E4CCCCD;
        Carat_ReadCopy((int)sec, (int)&s_ct674ba8, (const int*)a, (int)a, (int)ini, 3);
        memcpy(T + 2404, a, 12);
        int kind = F(int, T, 2372);
        if (kind == 3 || kind == 4) {
            GetPrivateProfileStringA(sec, &s_ct674b98, &s_ct6729a8, buf, 30, ini);
            if (!_stricmp(buf, &s_ct674b8c))
                F(int, T, 2376) = 2;
            else if (!_stricmp(buf, &s_ct674f8c))
                F(int, T, 2376) = 3;
            else
                F(int, T, 2376) = 1;
        } else
            F(int, T, 2376) = 0;
    }
    FILE* c = fopen(g_ct674ae0, &s_ct6703e4);
    if (!c)
        return;
    int len = (int)strlen(path) + 1;
    if (len > 199)
        len = 199;
    fwrite(&len, 4, 1, c);
    fwrite(path, 1, len, c);
    BYTE st[64];    // the game's (old CRT) struct _stat: the modification time at +28
    if (CallC<int>(Ct_stat, path, st) == -1)
        Setup_Fatal(&s_ct674b64);
    fwrite(st + 28, 4, 1, c);
    int sz = 2416;
    fwrite(&sz, 4, 1, c);
    fwrite(&g_ct6ce32c, 4, 1, c);
    fwrite(&g_ct6ce330, 4, 1, c);
    fwrite(&g_ct6ce334, 4, 1, c);
    fwrite(&g_ct6ce338, 4, 1, c);
    fwrite(&g_ct6ce33c, 4, 1, c);
    fwrite(&g_ct6ce340, 4, 1, c);
    fwrite(&g_ct6ce344, 4, 1, c);
    fwrite(&g_ct6ce348, 4, 1, c);
    for (int i = 0; i < 80; i++)
        fwrite(&g_ct697868 + i * 2416, 1, 2416, c);
    for (int i = 0; i < g_ct6ce32c; i++) {
        const char* f2 = (const char*)(&g_ct697868 + i * 2416 + 2212);
        if (CallC<int>(Ct_stat, f2, st) == -1) {
            sprintf(buf, &s_ct674b4c, f2);
            Setup_Fatal(buf);
        }
        fwrite(st + 28, 4, 1, c);
    }
    fclose(c);
}

} // extern "C"

// === reading the movable object types (FUN_50a740)

extern "C" int __cdecl Ot_Fatal(const char* msg); KING_ALIAS("_Ot_Fatal", "_Setup_Fatal")
extern "C" void Ot_stat(); KING_ALIAS("_Ot_stat", "__stat")
ASM_VAR(int, g_ot6ce34c, $L_6ce34c)     // the number of object types, then the first of each group
ASM_VAR(BYTE, g_ot6c6b68, $L_6c6b68)    // the object types, 35 of 876 bytes
ASM_VAR(float, g_ot64daf4, $L_64daf4)
ASM_VAR(char, s_ot675328, $L_675328)
ASM_VAR(char, s_ot6752c4, $L_6752c4)
ASM_VAR(char, s_ot675290, $L_675290)
ASM_VAR(char, s_ot67527c, $L_67527c)
ASM_VAR(char, s_ot66ce44, $L_66ce44)
ASM_VAR(char, s_ot67526c, $L_67526c)
ASM_VAR(char, s_ot675238, $L_675238)
ASM_VAR(char, s_ot6751e8, $L_6751e8)
ASM_VAR(char, s_ot6751d8, $L_6751d8)
ASM_VAR(char, s_ot6751c8, $L_6751c8)
ASM_VAR(char, s_ot6751c4, $L_6751c4)
ASM_VAR(char, s_ot6751c0, $L_6751c0)
ASM_VAR(char, s_ot6751bc, $L_6751bc)
ASM_VAR(char, s_ot6751b8, $L_6751b8)
ASM_VAR(char, s_ot67531c, $L_67531c)
ASM_VAR(char, s_ot675308, $L_675308)
ASM_VAR(char, s_ot6752f8, $L_6752f8)
ASM_VAR(char, s_ot6752d0, $L_6752d0)

#define OT_F(off) ((float*)(R + (off)))

extern "C" {

// [FUN_50a740] reads up to 35 movable object types (876 bytes each, 6c6b68) from ini file path, by groups
// (section numbers from 0, 30, 40, 50, 60, 80, 90, 100; the first type of each in 6ce354..6ce368): name, mass,
// frame, centre, inertia from the bounding box of the body points, collision points and planes, and kind;
// with edit the type editor library sees each one; then the table is appended to the cache 674ae0
void __cdecl ObjTypes_Read(const char* path, int edit)
{
    char err[300], buf[300], key[100];
    BYTE st[64];
    if (CallC<int>(Ot_stat, path, st) == -1) {
        sprintf(err, &s_ot675328, path);
        Ot_Fatal(err);
    }
    int n = 1, group = 0;
    int* firsts = &g_ct6ce348 + 2;   // 6ce350..6ce368: the first type of each group
    for (int k = 0; k < 7; k++)
        firsts[k] = 0;
    typedef void (__cdecl* EditFn)(BYTE*, int, const char*, const char*);
    EditFn fn = 0;
    HMODULE lib = 0;
    if (edit) {
        lib = LoadLibraryA(&s_ot67531c);
        if (!lib)
            Ot_Fatal(&s_ot675308);
        else {
            fn = (EditFn)GetProcAddress(lib, &s_ot6752f8);
            if (!fn)
                Ot_Fatal(&s_ot6752d0);
        }
    }
    SetFileAttributesA(path, FILE_ATTRIBUTE_NORMAL);
    g_ot6ce34c = 0;
    static const int bases[8] = {0, 30, 40, 50, 60, 80, 90, 100};
    for (;;) {
        BYTE R[876];
        memset(R + 72, 0, 48);
        memcpy(R + 72, &g_ct7025f0, 48);
        memset(R + 120, 0, 48);
        memcpy(R + 120, &g_ct7025f0, 48);
        memset(R + 168, 0, 24);
        memset(R + 204, 0, 240);
        memset(R + 444, 0, 24);
        memset(R + 468, 0, 12);
        memset(R + 488, 0, 48);
        for (int k = 0; k < 20; k++)
            memset(R + 536 + k * 16, 0, 12);
        int num = n + ((unsigned)group <= 7 ? bases[group] : 0);
        char* sec = buf;
        sprintf(sec, &s_ot6752c4, num);
        if ((unsigned)group <= 5)
            for (int k = group; k < 6; k++)
                firsts[1 + k] = g_ot6ce34c;
        Ini_GetFloat(sec, &s_ct675078, -1000.0, OT_F(192), path);
        if (*OT_F(192) < g_ct64c264) {
            if (n == 1)
                break;
            n = 1;
            group++;
        } else {
            if (g_ot6ce34c == 35) {
                sprintf(err, &s_ot675290, sec, 35);
                Ot_Fatal(err);
            }
            sprintf((char*)R + 35, &s_ot67527c, num);
            char name[300];
            GetPrivateProfileStringA(sec, &s_ot66ce44, &s_ct6729a8, name, 20, path);
            if (!strcmp(name, &s_ct6729a8)) {
                sprintf((char*)R, &s_ot67526c, num);
                if (group) {
                    sprintf(err, &s_ot675238, path, sec);
                    Ot_Fatal(err);
                }
            } else
                strcpy((char*)R, name);
            n++;
            memcpy(R + 72, &g_ct7025f0, 48);
            Ct_Read3(sec, &s_ct674e80, &g_ct702650, R + 108, path);
            float inv[12];
            memcpy(R + 120, CallC<void*>(Ct_Inverse, inv, R + 72), 48);
            Ct_Read3(sec, &s_ct674e44, &g_ct702650, R + 180, path);
            Ini_GetFloat(sec, &s_ct674e34, 1.0, OT_F(196), path);
            float v[3];
            Ct_Vec(v, 0x43FA0000, 0x43480000, 0x43FA0000);
            Ct_Read3(sec, &s_ct674f48, v, R + 168, path);
            int& cnt = F(int, R, 480);
            cnt = 0;
            for (int k = 0; k < 20; k++) {
                sprintf(key, &s_ct674db0, k + 1);
                if (!Ct_Read3(sec, key, &g_ct702650, R + 204 + cnt * 12, path))
                    break;
                cnt++;
            }
            float& radius = *OT_F(860);
            radius = 0;
            if (group != 3) {
                float lo[3], hi[3];
                ((DWORD*)lo)[0] = ((DWORD*)lo)[1] = ((DWORD*)lo)[2] = 0x461C4000;
                ((DWORD*)hi)[0] = ((DWORD*)hi)[1] = ((DWORD*)hi)[2] = 0xC61C4000;
                for (int k = 0; k < cnt; k++) {
                    const float* p = OT_F(204 + k * 12);
                    for (int c = 0; c < 3; c++) {
                        if (p[c] < lo[c])
                            lo[c] = p[c];
                        if (p[c] > hi[c])
                            hi[c] = p[c];
                    }
                    float d[3] = {p[0] - *OT_F(180), p[1] - *OT_F(184), p[2] - *OT_F(188)};
                    float l = (float)CallC<double>(Ct_Length, d);
                    if (l > radius)
                        radius = l;
                }
                float dx = hi[0] - lo[0], dy = hi[1] - lo[1], dz = hi[2] - lo[2];
                float* box = OT_F(444);
                box[0] = dx, box[1] = dy, box[2] = dz;
                box[3] = hi[0], box[4] = hi[1], box[5] = hi[2];
                box[6] = lo[0], box[7] = lo[1], box[8] = lo[2];
                F(int, R, 484) = 4;
                float* q = OT_F(488);
                q[0] = lo[0], q[1] = hi[1], q[2] = 0;
                q[3] = hi[0], q[4] = hi[1], q[5] = 0;
                q[6] = lo[0], q[7] = lo[1], q[8] = 0;
                q[9] = hi[0], q[10] = lo[1], q[11] = 0;
                if (dx < g_ct64c250 || dy < g_ct64c250 || dz < g_ct64c250) {
                    sprintf(err, &s_ot6751e8, sec, (double)dx, (double)dy, (double)dz);
                    Ot_Fatal(err);
                }
                float y2 = dy * dy, x2 = dx * dx, z2 = dz * dz;
                *OT_F(168) = (y2 + z2) * *OT_F(192) * g_ot64daf4;
                *OT_F(172) = (x2 + z2) * *OT_F(192) * g_ot64daf4;
                *OT_F(176) = (x2 + y2) * *OT_F(192) * g_ot64daf4;
            }
            if (fn)
                fn(R + 204, cnt, path, sec);
            F(int, R, 856) = 0;
            *OT_F(864) = radius;
            for (int k = 0; k < 20; k++) {
                sprintf(key, &s_ct674d34, k);
                float* pl = OT_F(536 + k * 16);
                if (!Carat_ReadFill((int)sec, (int)key, 0x3F800000, (int)pl, (int)path, 4))
                    break;
                float w = -pl[3];
                float d[3] = {pl[0] * w - *OT_F(180), pl[1] * w - *OT_F(184), pl[2] * w - *OT_F(188)};
                float l = (float)CallC<double>(Ct_Length, d);
                if (l < *OT_F(864))
                    *OT_F(864) = l;
                F(int, R, 856)++;
            }
            Ini_GetFloat(sec, &s_ct674ee0, 0.5, OT_F(200), path);
            if (*OT_F(200) < g_ct64c264 || *OT_F(200) > g_ct64c250)
                F(DWORD, R, 200) = 0x3E4CCCCD;
            F(int, R, 868) = GetPrivateProfileIntA(sec, &s_ot6751d8, 0, path);
            char kind[300];
            GetPrivateProfileStringA(sec, &s_ot6751c8, &s_ct6729a8, kind, 30, path);
            F(int, R, 872) = 0;
            if (!_stricmp(kind, &s_ot6751c4))
                F(int, R, 872) = 1;
            else if (!_stricmp(kind, &s_ot6751c0))
                F(int, R, 872) = 2;
            else if (!_stricmp(kind, &s_ot6751bc))
                F(int, R, 872) = 3;
            memcpy(&g_ot6c6b68 + g_ot6ce34c * 876, R, 876);
            g_ot6ce34c++;
            if (g_ot6ce34c > 35)
                break;
            continue;
        }
        if (g_ot6ce34c > 35)
            break;
    }
    if (lib)
        FreeLibrary(lib);
    FILE* c = fopen(g_ct674ae0, &s_ot6751b8);
    if (!c)
        return;
    int sz = 876;
    fwrite(&sz, 4, 1, c);
    fwrite(&g_ot6ce34c, 4, 1, c);
    fwrite(firsts, 4, 1, c);
    for (int k = 1; k < 7; k++)
        fwrite(firsts + k, 4, 1, c);
    fwrite(&g_ot6c6b68, 876, 35, c);
    fclose(c);
}

} // extern "C"

// === the map's road pieces (FUN_5291a0)

struct RmPos { DWORD m[8]; };       // road.h's PositionId, by value
struct RmInfo {                     // the road parameters (Road_GetRoadParameters)
    DWORD       flags;              // bit 5: not drawn
    DWORD       m_4;
    double      width;
    const char* module;
    const char* room;
    DWORD       m_24, m_28, m_32;
    int         m_36, m_40;
    DWORD       m_44;
};
extern "C" void Rm_FirstId(); KING_ALIAS("_Rm_FirstId", "_Road_FirstId")
extern "C" void Rm_NextId(); KING_ALIAS("_Rm_NextId", "_Road_NextId")
extern "C" void Rm_StartOf(); KING_ALIAS("_Rm_StartOf", "_Road_StartOf")
extern "C" void Rm_Params(); KING_ALIAS("_Rm_Params", "_Road_GetRoadParameters")
extern "C" void Rm_Advance(); KING_ALIAS("_Rm_Advance", "_Ai_AdvancePositionRoad")
extern "C" void Rm_Coords(); KING_ALIAS("_Rm_Coords", "_Ai_GetCoordinatesPos")
extern "C" void Rm_LoadingStep(); KING_ALIAS("_Rm_LoadingStep", "_Loading_Step")
ASM_VAR(char, s_rm66d638, $L_66d638)
ASM_VAR(char, s_rm66d628, $L_66d628)
ASM_VAR(double, g_rm64b670, $L_64b670)
ASM_VAR(double, g_rm64de08, $L_64de08)
ASM_VAR(double, g_rm64b5b8, $L_64b5b8)

static RmPos Rm_Start(int id, RmInfo* info)
{
    RmPos pos, t;
    pos = *CallC<RmPos*>(Rm_StartOf, &t, id);
    info->flags = 0;
    *(DWORD*)&info->width = 0;
    *((DWORD*)&info->width + 1) = 0;
    info->module = &s_rm66d638;
    info->room = &s_rm66d628;
    info->m_24 = 0;
    info->m_28 = 0;
    info->m_36 = -1;
    info->m_40 = -1;
    CallC(Rm_Params, pos, info);
    return pos;
}

// pos advanced along its road by *d (what is left in *d)
static RmPos Rm_Step(RmPos pos, double* d)
{
    RmPos t;
    return *CallC<RmPos*>(Rm_Advance, &t, pos, d, 0);
}

// the map point of pos (y mirrored)
static void Rm_Point(const RmPos& pos, double* x, double* y)
{
    float v[3];
    CallC(Rm_Coords, v, pos);
    v[1] = (float)-(double)v[1];
    *x = v[0];
    *y = v[1];
}

extern "C" {

// [FUN_5291a0] fills map s with its road pieces: the bounds of all roads (+8 .. +32, doubles) walked in steps
// of 100, then pieces of a twentieth of the map's height each (+0, 40 bytes: from, to, road id, drawn)
// counted in +4
void __fastcall Map_BuildRoads(BYTE* s, void*)
{
    if (F(void*, s, 0))
    {
        operator delete(F(void*, s, 0));
        F(void*, s, 0) = 0;
    }
    F(double, s, 8) = 100000.0;
    F(double, s, 24) = 100000.0;
    F(double, s, 16) = -100000.0;
    F(double, s, 32) = -100000.0;
    int id;
    RmInfo info;
    CallC(Rm_FirstId, &id);
    while (id != -1)
    {
        RmPos pos = Rm_Start(id, &info);
        double d;
        do
        {
            d = 100.0;
            pos = Rm_Step(pos, &d);
            double x, y;
            Rm_Point(pos, &x, &y);
            if (x < F(double, s, 8))
                F(double, s, 8) = x;
            if (x > F(double, s, 16))
                F(double, s, 16) = x;
            if (y < F(double, s, 24))
                F(double, s, 24) = y;
            if (y > F(double, s, 32))
                F(double, s, 32) = y;
        } while (!(d > g_rm64b670));
        int t;
        id = *CallC<int*>(Rm_NextId, &t, id);
    }
    for (int i = 0; i < 20; i++)
        CallC(Rm_LoadingStep);
    double step = x87_fabs(F(double, s, 32) - F(double, s, 24)) * g_rm64de08;
    F(int, s, 4) = 0;
    int t;
    id = *CallC<int*>(Rm_FirstId, &t);
    while (id != -1)
    {
        RmPos pos = Rm_Start(id, &info);
        double d;
        do
        {
            d = step;
            pos = Rm_Step(pos, &d);
            F(int, s, 4)++;
        } while (!(d > g_rm64b5b8));
        id = *CallC<int*>(Rm_NextId, &t, id);
    }
    int n = F(int, s, 4);
    BYTE* arr = (BYTE*)operator new(n * 40);
    if (arr)
        for (int i = 0; i < n; i++)
            F(int, arr, i * 40 + 32) = -1;
    F(BYTE*, s, 0) = arr;
    int k = 0;
    for (int i = 0; i < 20; i++)
        CallC(Rm_LoadingStep);
    id = *CallC<int*>(Rm_FirstId, &t);
    while (id != -1)
    {
        RmPos pos = Rm_Start(id, &info);
        double d = step;
        for (;;)
        {
            BYTE* rec = F(BYTE*, s, 0) + k * 40;
            Rm_Point(pos, &F(double, rec, 0), &F(double, rec, 8));
            pos = Rm_Step(pos, &d);
            Rm_Point(pos, &F(double, rec, 16), &F(double, rec, 24));
            F(int, rec, 32) = id;
            F(WORD, rec, 36) = (WORD)(~(info.flags >> 5) & 1);
            k++;
            if (k >= F(int, s, 4) || d > g_rm64b5b8)
                break;
            d = step;
        }
        id = *CallC<int*>(Rm_NextId, &t, id);
    }
    for (int i = 0; i < 20; i++)
        CallC(Rm_LoadingStep);
}

} // extern "C"

// === a 4x4 determinant (FUN_50b620)

extern "C" void Dt_Cofactor(); KING_ALIAS("_Dt_Cofactor", "@Matrix_Cofactor@16")
extern "C" double __cdecl Sign(int x);
ASM_VAR(float, g_dtIdentity, $L_7025f0)

extern "C" {

// [FUN_50b620] the determinant of the 4x4 double matrix s (rows of 32 bytes): row 0 expanded along the minors of
// rows 1-3, each as a float 3x3 by its cofactors
double __fastcall DMat_Determinant(const double* s, void*)
{
    double sum = 0;
    for (int k = 0; k < 4; k++)
    {
        float m[12];
        memcpy(m, &g_dtIdentity, 48);
        for (int r = 0; r < 3; r++)
        {
            const double* row = s + 4 * (r + 1);
            int c = 0;
            for (int j = 0; j < 4; j++)
                if (j != k)
                    m[3 * r + c++] = (float)row[j];
        }
        double c0 = CallThis<double>(Dt_Cofactor, m, 0, 0);
        double c1 = CallThis<double>(Dt_Cofactor, m, 0, 1);
        double c2 = CallThis<double>(Dt_Cofactor, m, 0, 2);
        double det = m[2] * c2 + m[1] * c1 + m[0] * c0;
        sum = Sign(k) * s[k] * det + sum;
    }
    return sum;
}

} // extern "C"

// === the map screen (FUN_529890)

struct MpPos32 { DWORD d[8]; };
extern "C" float* __cdecl Mp_CoordPos(float* out, MpPos32 p); KING_ALIAS("_Mp_CoordPos", "_Ai_GetCoordinatesPos")
extern "C" MpPos32* __cdecl Mp_PositionId(MpPos32* out, void* player); KING_ALIAS("_Mp_PositionId", "_Ai_GetPositionId")
extern "C" void Mp_M1(); KING_ALIAS("_Mp_M1", "@SceneObj_Matrix1@12")
extern "C" void Mp_CarId(); KING_ALIAS("_Mp_CarId", "_Player_CarId")
extern "C" void Mp_Flags(); KING_ALIAS("_Mp_Flags", "_Player_Flags")
extern "C" void Mp_BeginScene(); KING_ALIAS("_Mp_BeginScene", "_Device_BeginScene")
extern "C" void Mp_WaitIdle(); KING_ALIAS("_Mp_WaitIdle", "_Device_WaitIdle")
extern "C" void Mp_First(); KING_ALIAS("_Mp_First", "_List68cf6c_First")
extern "C" void Mp_Next(); KING_ALIAS("_Mp_Next", "_List68cf6c_Next")
extern "C" void Mp_Field8(); KING_ALIAS("_Mp_Field8", "_Rec_Field8")
extern "C" void Mp_ForEach(); KING_ALIAS("_Mp_ForEach", "_Array_ForEach@16")
extern "C" void Mp_ZeroThis(); KING_ALIAS("_Mp_ZeroThis", "@Vec3_ZeroThis@8")
extern "C" void Mp_InitState(); KING_ALIAS("_Mp_InitState", "@Fx_InitState@8")
extern "C" void Mp_Present(); KING_ALIAS("_Mp_Present", "_Ai_GetPresentData")
extern "C" void Mp_Vec1Free(); KING_ALIAS("_Mp_Vec1Free", "@Vec1_Free@8")
extern "C" void Mp_VecBFree(); KING_ALIAS("_Mp_VecBFree", "@VecB_FreeStorage@8")
extern "C" void Mp_OutOfBox(); KING_ALIAS("_Mp_OutOfBox", "_Carat_OutOfBox")
extern "C" void Mp_Image(); KING_ALIAS("_Mp_Image", "_Gfx_DrawImage")
extern "C" void Mp_Line(); KING_ALIAS("_Mp_Line", "_Window_DrawLine")
extern "C" void Mp_Pixel(); KING_ALIAS("_Mp_Pixel", "_Pixel_Pack")
extern "C" void Mp_Text(); KING_ALIAS("_Mp_Text", "_Text_Draw")
extern "C" void Mp_Str(); KING_ALIAS("_Mp_Str", "_Table71e858_Get")
extern "C" void Mp_OrderInfo(); KING_ALIAS("_Mp_OrderInfo", "_Ai_GetPlayerOrderInfo")
extern "C" void Mp_TownFirst(); KING_ALIAS("_Mp_TownFirst", "_Town_FirstId")
extern "C" void Mp_TownNext(); KING_ALIAS("_Mp_TownNext", "_Town_NextId")
extern "C" void Mp_TownPos(); KING_ALIAS("_Mp_TownPos", "_Town_Get440390")
extern "C" void Mp_TownFlags(); KING_ALIAS("_Mp_TownFlags", "_Town_Flags")
extern "C" void Mp_TownName(); KING_ALIAS("_Mp_TownName", "_Town_Field12")
extern "C" void Mp_NameCoords(); KING_ALIAS("_Mp_NameCoords", "_Ai_GetNameCoordinates")
extern "C" void Mp_ObstFirst(); KING_ALIAS("_Mp_ObstFirst", "_List68cc18_FirstId")
extern "C" void Mp_ObstNext(); KING_ALIAS("_Mp_ObstNext", "_List68cc18_NextId")
extern "C" void Mp_Obstacle(); KING_ALIAS("_Mp_Obstacle", "_Ai_GetObstacleInfo")
extern "C" void Mp_NodeFirst(); KING_ALIAS("_Mp_NodeFirst", "_Node_FirstId")
extern "C" void Mp_NodeNext(); KING_ALIAS("_Mp_NodeNext", "_Node_NextId")
extern "C" void Mp_NodePos(); KING_ALIAS("_Mp_NodePos", "_Node_Position")
extern "C" void Mp_NodeName(); KING_ALIAS("_Mp_NodeName", "_Node_Name")
extern "C" void Mp_PlFirst(); KING_ALIAS("_Mp_PlFirst", "_Players_First")
extern "C" void Mp_PlNext(); KING_ALIAS("_Mp_PlNext", "_Players_Next")
extern "C" void Mp_PlayerData(); KING_ALIAS("_Mp_PlayerData", "_Ai_GetPlayerData")
extern "C" void Mp_ResGet(); KING_ALIAS("_Mp_ResGet", "@Resources_Get@12")
extern "C" void Mp_EntryFirst(); KING_ALIAS("_Mp_EntryFirst", "_Player_FirstEntry")
extern "C" void Mp_EntryNext(); KING_ALIAS("_Mp_EntryNext", "_Player_NextEntry")
ASM_VAR(BYTE*, mp_6d2078, $L_6d2078)
ASM_VAR(Vec3, mp_702620, $L_702620)
ASM_VAR(double, mp_64b670, $L_64b670)
ASM_VAR(double, mp_64ca48, $L_64ca48)
ASM_VAR(double, mp_64ebc0, $L_64ebc0)
ASM_VAR(float, mp_64c264, $L_64c264)
ASM_VAR(double, mp_64bb90, $L_64bb90)
ASM_VAR(double, mp_64b6d0, $L_64b6d0)
ASM_VAR(double, mp_64ebb8, $L_64ebb8)
ASM_VAR(double, mp_64b6e0, $L_64b6e0)
ASM_VAR(double, mp_64b7e0, $L_64b7e0)
ASM_VAR(double, mp_64ebb0, $L_64ebb0)
ASM_VAR(double, mp_64b918, $L_64b918)
ASM_VAR(int, mp_6ced94, $L_6ced94)
ASM_VAR(void*, mp_68cf6c, $L_68cf6c)
ASM_VAR(void*, mp_68b990, $L_68b990)
ASM_VAR(BYTE, mp_7025f0, $L_7025f0)
ASM_VAR(BYTE*, mp_696ca0, $L_696ca0)
ASM_VAR(BYTE**, mp_68cf00, $L_68cf00)
ASM_VAR(BYTE*, mp_6cec70, $L_6cec70)
ASM_VAR(BYTE*, mp_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, mp_6cecbc, $L_6cecbc)
ASM_VAR(BYTE*, mp_6ced24, $L_6ced24)
ASM_VAR(BYTE*, mp_6f3268, $L_6f3268)
ASM_VAR(BYTE*, mp_6f3378, $L_6f3378)
ASM_VAR(BYTE*, mp_6f2d08, $L_6f2d08)
ASM_VAR(BYTE*, mp_6f3380, $L_6f3380)
ASM_VAR(BYTE*, mp_6f3258, $L_6f3258)
ASM_VAR(BYTE*, mp_6f2de4, $L_6f2de4)
ASM_VAR(BYTE*, mp_6f2c50, $L_6f2c50)
ASM_VAR(BYTE*, mp_6f2d14, $L_6f2d14)
ASM_VAR(BYTE*, mp_6f337c, $L_6f337c)
ASM_VAR(BYTE*, mp_6f33a8, $L_6f33a8)
ASM_VAR(BYTE*, mp_6f3330, $L_6f3330)
ASM_VAR(BYTE*, mp_6f2df4, $L_6f2df4)
ASM_VAR(BYTE*, mp_6f2f0c, $L_6f2f0c)

// a map point: world x, y scaled by s and moved by ox / oy (y flipped)
struct MpView { double s, ox, oy; BYTE* surf; int mode; };

static void Mp_At(const MpView& v, const float* p, int& x, int& y)
{
    float py = -p[1];
    x = (int)(p[0] * v.s + v.ox);
    y = (int)(py * v.s + v.oy);
}

static bool Mp_Off(const MpView& v, int x, int y)
{
    return v.mode && CallC<int>(Mp_OutOfBox, v.surf, x, y);
}

static void Mp_Icon(const MpView& v, int x, int y, BYTE* img)
{
    CallC(Mp_Image, v.surf, x, y, img, 0);
}

static int Mp_Me()
{
    int id;
    CallC(Mp_CarId, &id);
    return id;
}

static void Mp_Label(const MpView& v, int x, int y, const char* s)
{
    CallC(Mp_Text, v.surf, x, y, mp_6ced24, s, 5, 0);
}

extern "C" {

// [FUN_529890] map m (+0 its 40-byte roads, +4 their count, +8.. the doubles x0, x1, y0, y1) drawn on surface surf,
// the small one in the corner (mode 0) or the big one (mode 1, with a box and nothing outside it): roads, presents,
// towns, obstacles, the start, service nodes, town names, players and their entries, the towns of the player's two
// orders blinking, the car's arrow by its heading and in multiplayer the player's name
void __fastcall Map_Draw(BYTE* m, void*, BYTE* surf, int mode)
{
    if (!F(int, m, 4))
        return;
    BYTE* car = F(BYTE*, mp_6d2078, 616);
    Matrix t;
    Vec3 cp = F(Vec3, CallThis<BYTE*>(Mp_M1, car + 16, &t), 36);
    const float* M = CallThis<const Matrix*>(Mp_M1, car + 16, &t)->m;
    const Vec3& f = mp_702620;
    float fx = (float)((double)f.z * M[6] + (double)f.y * M[3] + (double)f.x * M[0]);
    double fy = (double)f.x * M[1] + (double)f.z * M[7] + (double)f.y * M[4];
    double fz = (double)f.x * M[2] + (double)f.z * M[8] + (double)f.y * M[5];
    double c = fx * (double)f.x + f.z * fz + f.y * fy;
    if (c > mp_64b670)
        c = 1.0;
    else if (c < mp_64ca48)
        c = -1.0;
    int a = (int)(x87_acos(c) * mp_64ebc0);
    if (fx < mp_64c264)
        a = 24 - a;
    int heading = a % 24;
    cp.y = -cp.y;
    int w = F(WORD, surf, 0), h = F(WORD, surf, 2);
    double sx = (w - 80) / (F(double, m, 16) - F(double, m, 8));
    double sy = (h - 130) / (F(double, m, 32) - F(double, m, 24));
    if (mode)
    {
        sx += sx;
        sy += sy;
    }
    MpView v;
    v.s = sx > sy ? sy : sx;
    v.ox = mp_64bb90 - v.s * F(double, m, 8);
    v.oy = mp_64bb90 - v.s * F(double, m, 24);
    v.surf = surf;
    v.mode = mode;
    CallC(Mp_BeginScene);
    if (mode)
    {
        v.ox = w / 6 - cp.x * v.s;
        v.oy = h / 2 - cp.y * v.s;
        int x0 = w / 20, y0 = h / 4, x1 = w / 3 - w / 20, y1 = h - h / 4;
        CallC(Mp_Line, surf, x0, y0, x1, y0, CallC<DWORD>(Mp_Pixel, 0, 120, 255), 1);
        CallC(Mp_Line, surf, x1, y0, x1, y1, CallC<DWORD>(Mp_Pixel, 0, 120, 255), 1);
        CallC(Mp_Line, surf, x1, y1, x0, y1, CallC<DWORD>(Mp_Pixel, 0, 120, 255), 1);
        CallC(Mp_Line, surf, x0, y1, x0, y0, CallC<DWORD>(Mp_Pixel, 0, 120, 255), 1);
    }
    for (int i = 0; i < F(int, m, 4); i++)
    {
        BYTE* r = F(BYTE*, m, 0) + i * 40;
        if (F(short, r, 36) || mp_6ced94)
            Map_DrawLine((double*)r, 0, surf, v.s, v.s, v.ox, v.oy, mode);
    }
    CallC(Mp_WaitIdle);
    // presents
    void* it;
    CallC(Mp_First, &it);
    while (it != mp_68cf6c)
    {
        __declspec(align(8)) BYTE P[288];
        F(int, P, 0) = 0;
        CallStd(Mp_ForEach, P + 4, 12, 4, Mp_ZeroThis);
        memcpy(P + 4, &mp_7025f0, 48);
        memset(P + 52, 0, 20);
        F(int, P, 72) = -1;
        F(int, P, 76) = -1;
        CallThis(Mp_InitState, P + 80);
        F(int, P, 272) = -1;
        F(int, P, 276) = -1;
        CallC(Mp_Present, mp_68b990, it, P);
        float py = -F(float, P, 44);
        float px = F(float, P, 40);
        int x = (int)(px * v.s + v.ox - mp_64b6d0);
        int y = (int)(py * v.s + v.oy - mp_64b6d0);
        if (Mp_Off(v, x, y))
        {
            CallThis(Mp_Vec1Free, P + 168);
            CallThis(Mp_VecBFree, P + 156);
        }
        else
        {
            int fl = CallC<int>(Mp_Field8, it);
            if ((fl & 64) && (fl & 32))
                Mp_Icon(v, x, y, mp_6f3268);
            else if (fl & 64)
            {
                if (mp_6ced94)
                    Mp_Icon(v, x, y, mp_6f3378);
            }
            else if ((fl & 16) && mp_6ced94)
                Mp_Icon(v, x, y, mp_6f2d08);
            CallThis(Mp_Vec1Free, P + 168);
            CallThis(Mp_Vec1Free, P + 156);
        }
        CallThis(Mp_VecBFree, P + 144);
        CallThis(Mp_VecBFree, P + 132);
        void* nx;
        it = *CallC<void**>(Mp_Next, &nx, it);
    }
    // the towns of the player's orders
    int order[4] = {-1, -1, -1, -1};
    __declspec(align(8)) BYTE Q[200];
    CallThis(Mp_InitState, Q);
    int orders = 0;
    while (orders < 2 && !CallC<int>(Mp_OrderInfo, Mp_Me(), orders, Q))
        order[orders++] = F(int, Q, 48);
    int per = (int)(mp_64ebb8 / F(double, mp_696ca0, 2960) + mp_64b6e0);
    if (per < 1)
        per = 1;
    int phase = F(int, mp_696ca0, 2992) / per % 6;
    int blink = phase > 3 ? 6 - phase : phase;
    int id;
    for (CallC(Mp_TownFirst, &id); id != -1; id = *CallC<int*>(Mp_TownNext, &id, id))
    {
        float p[3];
        CallC(Mp_TownPos, p, id);
        int x, y;
        Mp_At(v, p, x, y);
        if (Mp_Off(v, x, y))
            continue;
        int k = 0;
        while (k < orders && order[k] != id)
            k++;
        if (k < orders)
            continue;
        DWORD tf = CallC<DWORD>(Mp_TownFlags, id);
        if (tf & 1)
            Mp_Icon(v, x - 5, y - 5, mp_6f3380);
        else if (tf & 2)
        {
            if (mode)
                Mp_Icon(v, x - 5, y - 5, mp_6f3258);
        }
        else
            Mp_Icon(v, x - 5, y - 5, mp_6f2de4);
    }
    // obstacles
    for (CallC(Mp_ObstFirst, &id); id != -1; id = *CallC<int*>(Mp_ObstNext, &id, id))
    {
        BYTE O[16] = {0};
        CallC(Mp_Obstacle, id, O);
        if (!O[12])
            continue;
        int x, y;
        Mp_At(v, (float*)O, x, y);
        if (!Mp_Off(v, x, y))
            Mp_Icon(v, x - 5, y - 5, mp_6f2c50);
    }
    {
        int x = (int)(v.ox - v.s * mp_64b7e0);
        int y = (int)(v.s * mp_64ebb0 + v.oy);
        if (!Mp_Off(v, x, y))
        {
            Mp_Icon(v, x - 5, y - 5, mp_6f2d14);
            Mp_Label(v, x - 20, y - 5, CallC<const char*>(Mp_Str, 88));
        }
    }
    // service nodes
    for (CallC(Mp_NodeFirst, &id); id != -1; id = *CallC<int*>(Mp_NodeNext, &id, id))
    {
        if (!(F(BYTE, mp_68cf00[id], 12) & 8))
            continue;
        float p[3];
        CallC(Mp_NodePos, p, id);
        int x, y;
        Mp_At(v, p, x, y);
        if (Mp_Off(v, x, y))
            continue;
        if (!strncmp(CallC<const char*>(Mp_NodeName, id), "mapSto", 6))
        {
            if (mode)
                Mp_Icon(v, x - 5, y - 5, mp_6f337c);
        }
        else if (!strncmp(CallC<const char*>(Mp_NodeName, id), "mapPark", 7))
            Mp_Icon(v, x - 5, y - 5, mp_6f33a8);
    }
    // town names
    for (CallC(Mp_TownFirst, &id); id != -1; id = *CallC<int*>(Mp_TownNext, &id, id))
    {
        int res = 3 - ((int)(F(DWORD, mp_6cec70, 408) & 240) - 16) / 16;
        float p[8];
        const float* q = CallC<const float*>(Mp_NameCoords, p, id, mode, res);
        int x, y;
        Mp_At(v, q, x, y);
        if (Mp_Off(v, x, y))
            continue;
        DWORD tf = CallC<DWORD>(Mp_TownFlags, id);
        if ((tf & 1) || ((tf & 2) && !mode))
            continue;
        Mp_Label(v, x - 20, y - 5, CallC<const char*>(Mp_TownName, id));
    }
    // players
    __declspec(align(8)) BYTE PD[256];
    void* pl;
    for (CallC(Mp_PlFirst, &pl); pl != mp_68b990; pl = *CallC<void**>(Mp_PlNext, &pl, pl))
    {
        if (!F(int, mp_6cec6c, 1316) && !(CallC<DWORD>(Mp_Flags, pl) & 128))
            continue;
        CallC(Mp_PlayerData, pl, PD);
        if (F(int, PD, 8) == 1 && !(CallC<DWORD>(Mp_Flags, pl) & 0x100))
            continue;
        MpPos32 pos;
        float p[3];
        Mp_CoordPos(p, *Mp_PositionId(&pos, pl));
        BYTE* rec = CallThis<BYTE*>(Mp_ResGet, mp_696ca0 + 704, F(int, pl, 84));
        if (rec && F(BYTE*, rec, 28))
        {
            Matrix t2;
            const Vec3& o = F(Vec3, CallThis<BYTE*>(Mp_M1, F(BYTE*, rec, 28) + 16, &t2), 36);
            p[0] = o.x;
            p[1] = o.y;
            p[2] = o.z;
        }
        int x, y;
        Mp_At(v, p, x, y);
        if (Mp_Off(v, x, y))
            continue;
        if (F(int, PD, 8) == 1)
        {
            if (mp_6f3330)
                Mp_Icon(v, x - 5, y - 5, mp_6f3330);
            else
                CallC(Mp_Line, surf, x, y, x + 3, y + 3, 4, 0);
            Mp_Label(v, x + 15, y, F(const char*, PD, 48));
        }
        else if (mp_6f2df4)
            Mp_Icon(v, x - 5, y - 5, mp_6f2df4);
        else
            CallC(Mp_Line, surf, x, y, x + 3, y + 3, 4, 0);
    }
    // the player's entries
    void* e;
    for (CallC(Mp_EntryFirst, &e, Mp_Me()); e != mp_68b990; CallC(Mp_EntryNext, &e, Mp_Me()))
    {
        MpPos32 pos;
        float p[3];
        Mp_CoordPos(p, *Mp_PositionId(&pos, e));
        CallC(Mp_PlayerData, e, PD);
        int x, y;
        Mp_At(v, p, x, y);
        if (Mp_Off(v, x, y) || !mp_6f3330)
            continue;
        BYTE* s300 = F(BYTE*, F(BYTE*, mp_6cecbc, 1032), 300);
        const char* name = s300 ? F(const char*, s300, 12) : 0;
        if (name && !strcmp(name, F(const char*, PD, 48)) && !((int)(F(double, mp_696ca0, 2968) + F(double, mp_696ca0, 2968)) % 2))
            continue;
        Mp_Icon(v, x - 5, y - 5, mp_6f3330);
    }
    // the orders' towns blinking
    for (CallC(Mp_TownFirst, &id); id != -1; id = *CallC<int*>(Mp_TownNext, &id, id))
    {
        float p[3];
        CallC(Mp_TownPos, p, id);
        int x, y;
        Mp_At(v, p, x, y);
        if (Mp_Off(v, x, y))
            continue;
        int k = 0;
        while (k < orders && order[k] != id)
            k++;
        if (k >= orders)
            continue;
        DWORD tf = CallC<DWORD>(Mp_TownFlags, id);
        if (tf & 1)
            Mp_Icon(v, x - 5, y - 5, mp_6f3380);
        else if (tf & 2)
            Mp_Icon(v, x - 5, y - 5, (&mp_6f3258)[blink]);
        else
            Mp_Icon(v, x - 5, y - 5, (&mp_6f2de4)[blink]);
    }
    // the car
    v.oy = cp.y * v.s + v.oy;
    v.ox = cp.x * v.s + v.ox;
    Mp_Icon(v, (int)(v.ox - mp_64b6d0), (int)(v.oy - mp_64b6d0), (&mp_6f2f0c)[phase + heading * 6]);
    if (F(int, mp_6cec6c, 1316))
    {
        CallC(Mp_PlayerData, Mp_Me(), PD);
        Mp_Label(v, (int)(v.ox + mp_64b918), (int)v.oy, F(const char*, PD, 48));
    }
    CallThis(Mp_VecBFree, Q + 88);
    CallThis(Mp_VecBFree, Q + 76);
    CallThis(Mp_VecBFree, Q + 64);
    CallThis(Mp_VecBFree, Q + 52);
}

} // extern "C"

// === the garage handing the player another vehicle (a method at $L_527240 with its tail $L_52773b)

extern "C" void Gv_CarId(); KING_ALIAS("_Gv_CarId", "_Player_CarId")
extern "C" void Gv_First(); KING_ALIAS("_Gv_First", "_Player_FirstEntry")
extern "C" void Gv_Next(); KING_ALIAS("_Gv_Next", "_Player_NextEntry")
extern "C" void Gv_Data(); KING_ALIAS("_Gv_Data", "_Ai_GetPlayerData")
extern "C" void Gv_GiveBack(); KING_ALIAS("_Gv_GiveBack", "_Player_GiveBackCargo")
extern "C" void Gv_RemoveName(); KING_ALIAS("_Gv_RemoveName", "@List_RemoveName@12")
extern "C" void Gv_Release(); KING_ALIAS("_Gv_Release", "@ResRef_Release@8")
extern "C" void Gv_ResGet(); KING_ALIAS("_Gv_ResGet", "@Resources_Get@12")
extern "C" void Gv_Place(); KING_ALIAS("_Gv_Place", "_Fx_PlaceObjects")
extern "C" void Gv_ClearHits(); KING_ALIAS("_Gv_ClearHits", "_World_ClearHits")
extern "C" void Gv_Swap(); KING_ALIAS("_Gv_Swap", "_Player_SwapVehicles")
extern "C" void Gv_Send(); KING_ALIAS("_Gv_Send", "_Net_SendObjectMessage")
extern "C" void Gv_Keys(); KING_ALIAS("_Gv_Keys", "@Engine_SetKeys@16")
extern "C" void Gv_Get212(); KING_ALIAS("_Gv_Get212", "_Obj6f3498_Get212")
extern "C" void Gv_Detach(); KING_ALIAS("_Gv_Detach", "@Player_DetachCar@8")
extern "C" void Gv_Flags(); KING_ALIAS("_Gv_Flags", "_Player_Flags")
extern "C" void Gv_Launch(); KING_ALIAS("_Gv_Launch", "_Car_Launch")
extern "C" void Gv_Scene(); KING_ALIAS("_Gv_Scene", "@Scenes_Find@12")
extern "C" void Gv_PosId(); KING_ALIAS("_Gv_PosId", "_Ai_GetPositionId")
extern "C" void Gv_LoadRoom(); KING_ALIAS("_Gv_LoadRoom", "@Fw_LoadRoom@16")
extern "C" void Gv_Self(); KING_ALIAS("_Gv_Self", "@Obj541960_Self@8")
extern "C" void Gv_VRelease(); KING_ALIAS("_Gv_VRelease", "_Vehicle_Release")
extern "C" void Gv_VLoad(); KING_ALIAS("_Gv_VLoad", "_Vehicle_LoadResource")
extern "C" void Gv_SysFatal(); KING_ALIAS("_Gv_SysFatal", "_Sys_Fatal")
extern "C" void Gv_Attach(); KING_ALIAS("_Gv_Attach", "@WorldObj_Attach@12")
extern "C" void Gv_Cargo(); KING_ALIAS("_Gv_Cargo", "_Copter_ApplyCargo")
extern "C" void Gv_MsgClear(); KING_ALIAS("_Gv_MsgClear", "_Messages_Clear1276")
extern "C" void Gv_Screen(); KING_ALIAS("_Gv_Screen", "_Carat_SetScreenSize")
extern "C" void Gv_Ffb(); KING_ALIAS("_Gv_Ffb", "_Joy_ForceFeedback")
extern "C" void Gv_SetVehicle(); KING_ALIAS("_Gv_SetVehicle", "@Viewer_AttachVehicle@12")
struct GvPos { int d[8]; };
extern "C" int __cdecl Gv_RoadParams(GvPos pos, BYTE* info); KING_ALIAS("_Gv_RoadParams", "_Road_GetRoadParameters")
ASM_VAR(int, g_gv68b990, $L_68b990)
ASM_VAR(BYTE*, g_gv696ca0, $L_696ca0)
ASM_VAR(int, g_gv675d38, $L_675d38)
ASM_VAR(BYTE*, g_gv6d2078, $L_6d2078)
ASM_VAR(BYTE*, g_gv6f34b0, $L_6f34b0)
ASM_VAR(BYTE*, g_gv6ef1f8, $L_6ef1f8)
ASM_VAR(BYTE, g_gv6f68b0, $L_6f68b0)
ASM_VAR(BYTE*, g_gv6ce4f4, $L_6ce4f4)

static int Gv_Me()
{
    int id;
    CallC(Gv_CarId, &id);
    return id;
}

extern "C" {

// [$L_527240..$L_52773b] garage list g (its vtable at -64, the list +968) on message 185 (take) / 186 (give
// back) for the player's entry named as the selection: giving back returns the cargo and drops the name; taking
// swaps the player's vehicle for the entry's one (resources +704, the scene loaded and the vehicle resource made
// when it has no object yet), sends it on, resets the cab keys, attaches the new vehicle and refreshes the screen
void __fastcall Garage_Message(BYTE* g, void*, int msg)
{
    BYTE* sel = F(BYTE*, F(BYTE*, g, 968), 300);
    const char* name = sel ? F(const char*, sel, 12) : 0;
    int found = g_gv68b990;
    int e;
    CallC(Gv_First, &e, Gv_Me());
    while (e != g_gv68b990)
    {
        BYTE pd[192];
        CallC(Gv_Data, e, pd);
        if (!strcmp(F(const char*, pd, 48), name))
        {
            found = e;
            break;
        }
        CallC(Gv_Next, &e, Gv_Me());
    }
    if (found == g_gv68b990)
        return;
    if (msg == 186)
    {
        CallC(Gv_GiveBack, Gv_Me(), found, 0);
        CallThis(Gv_RemoveName, F(BYTE*, g, 968), name);
        return;
    }
    if (msg != 185)
        return;
    BYTE* rr = F(BYTE*, g_gv696ca0, 484);
    if (F(int, rr, 20))
    {
        while (!F(int, F(BYTE*, g_gv696ca0, 484), 16))
            Sleep(10);
        CallThis(Gv_Release, F(BYTE*, g_gv696ca0, 484));
    }
    g_gv675d38 = F(int, g_gv696ca0, 2992);
    BYTE* car = F(BYTE*, g_gv6d2078, 616);
    int me = Gv_Me();
    BYTE pd[256];
    CallC(Gv_Data, found, pd);
    if (found == g_gv68b990)
        return;
    BYTE* rec = CallThis<BYTE*>(Gv_ResGet, g_gv696ca0 + 704, F(int, pd, 36));
    BYTE* rec2 = CallThis<BYTE*>(Gv_ResGet, g_gv696ca0 + 704, F(int, car, 360));
    CallC(Gv_Place);
    CallC(Gv_ClearHits);
    if (CallC<int>(Gv_Swap, me, found, 1))
        return;
    BYTE* obj = F(BYTE*, rec, 24);
    const DWORD* m = (const DWORD*)(obj + 248);
    CallC(Gv_Send, F(int, rec, 4), F(int, rec2, 4), F(const char*, F(BYTE*, rec, 12), 4016), obj + 16, m[0], m[1],
          m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11]);
    CallThis(Gv_Keys, F(BYTE*, car, 21600) + 9984, 1, 1);
    F(short, F(BYTE*, car, 21600), 10054) = 100;
    F(short, F(BYTE*, car, 21600), 10048) = 0;
    F(int, F(BYTE*, car, 21600), 10080) = 0;
    F(short, F(BYTE*, car, 21600), 10008) = 0;
    F(int, car, 20636) = found;
    CallC(Gv_Get212);
    int keep = F(int, car, 20920);
    CallThis(Gv_Detach, g_gv6d2078);
    F(int, F(BYTE*, rec, 24), 296) = me;
    F(int, F(BYTE*, rec2, 24), 296) = found;
    F(BYTE, rec, 8) |= 1;
    F(BYTE, rec2, 8) &= ~1;
    BYTE* v = 0;
    if (F(BYTE*, rec, 28))
    {
        v = F(BYTE*, rec, 28);
        F(short, F(BYTE*, v, 21600), 10048) = 1;
        F(int, F(BYTE*, v, 21600), 10080) = 1;
        F(int, v, 20636) = me;
        int fl = CallC<int>(Gv_Flags, me);
        CallC(Gv_Launch, v, me, &fl, 1);
        const char* n2 = F(const char*, F(BYTE*, rec2, 12), 4016);
        if (strcmp(F(const char*, F(BYTE*, rec, 12), 4016), n2))
            g_gv6f34b0 = CallThis<BYTE*>(Gv_Scene, g_gv696ca0, n2);
    }
    else
    {
        GvPos pos;
        CallC(Gv_PosId, &pos, me);
        BYTE info[48];
        memset(info, 0, sizeof info);
        F(const char*, info, 16) = "UNDEFINED_MODULE";
        F(const char*, info, 20) = "UNDEFINED_ROOM";
        F(int, info, 36) = -1;
        F(int, info, 40) = -1;
        Gv_RoadParams(pos, info);
        CallThis(Gv_LoadRoom, g_gv696ca0, F(const char*, info, 16), F(const char*, info, 20));
        if (!F(BYTE*, rec, 28))
        {
            while ((unsigned)F(int, g_gv696ca0, 2704) < (unsigned)F(int, rec, 16))
            {
                BYTE* o = CallThis<BYTE*>(Gv_Self, g_gv6ef1f8 ? g_gv6ef1f8 - 4 : 0);
                if (!o)
                    break;
                CallC(Gv_VRelease, F(int, o, 360));
            }
            CallC(Gv_VLoad, F(int, rec, 4));
            if (!F(BYTE*, rec, 28))
            {
                char buf[128];
                sprintf(buf, " %s/%s", F(const char*, info, 16), F(const char*, F(BYTE*, rec, 12), 4016));
                CallC(Gv_SysFatal, "Vehicle after change plases not found", buf);
            }
        }
        v = F(BYTE*, rec, 28);
        g_gv6f34b0 = 0;
    }
    CallThis(Gv_SetVehicle, g_gv6d2078, v);
    CallThis(Gv_Attach, &g_gv6f68b0, v);
    F(int, v, 20920) = keep;
    CallC(Gv_Cargo);
    CallVirt(g - 64, 32);
    CallVirt(g_gv696ca0, 36);
    if (g_gv6ce4f4)
        CallVirt(g_gv6ce4f4, 0, 1);
    g_gv6ce4f4 = 0;
    F(DWORD, g_gv696ca0, 16) |= 3;
    CallC(Gv_MsgClear);
    CallC(Gv_Screen);
    CallC(Gv_Ffb, 1);
}

} // extern "C"

// === a saved game's streams written (FUN_51d6d0 with its tails up to $L_51d8c0)

extern "C" void Sv_Wide(); KING_ALIAS("_Sv_Wide", "_Str_ToWide")
extern "C" void Sv_Fatal(); KING_ALIAS("_Sv_Fatal", "_Sys_Fatal")
extern "C" void Sv_Get212(); KING_ALIAS("_Sv_Get212", "_Obj6f3498_Get212")
extern "C" void Sv_ResSave(); KING_ALIAS("_Sv_ResSave", "@Resources_Save@12")
extern "C" void Sv_Thumb(); KING_ALIAS("_Sv_Thumb", "_Shell_WriteThumbnail")
ASM_VAR(char, g_svTime, $L_677738)
ASM_VAR(char, g_svTimeErr, $L_67770c)
ASM_VAR(char, g_svState, $L_677704)
ASM_VAR(char, g_svStateErr, $L_6776d8)
ASM_VAR(char, g_svRes, $L_6776d4)
ASM_VAR(char, g_svResErr, $L_6776ac)
ASM_VAR(char, g_svThumb, $L_6772a0)
ASM_VAR(char, g_svThumbErr, $L_677684)
ASM_VAR(BYTE*, g_sv6f33ac, $L_6f33ac)
ASM_VAR(int, g_sv6f33b8, $L_6f33b8)
ASM_VAR(int, g_sv6f33bc, $L_6f33bc)
ASM_VAR(BYTE*, g_sv6d2078, $L_6d2078)
ASM_VAR(BYTE*, g_sv6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_sv696ca0, $L_696ca0)
ASM_VAR(double, g_sv64e4c8, $L_64e4c8)

// a COM method (stdcall, this pushed) at vtbl+off
template <class R = long, class... A>
static R Sv_Com(void* p, int off, A... a)
{
    return ((R(__stdcall*)(void*, A...))(*(void***)p)[off / 4])(p, a...);
}

// storage st's new stream named name (IStorage::CreateStream, create | write), fatal err when it fails
static BYTE* Sv_Stream(BYTE* st, const char* name, const char* err)
{
    BYTE* s = 0;
    void* w = CallC<void*>(Sv_Wide, name);
    Sv_Com(st, 12, w, 4113, 0, 0, &s);
    if (!s)
        CallC(Sv_Fatal, err, 0);
    return s;
}

static void Sv_Write(BYTE* s, const void* p, int n)
{
    int done;
    Sv_Com(s, 16, p, n, &done);
}

extern "C" {

// [FUN_51d6d0..$L_51d8c0] the game saved into storage st: the clock (time, day scaled, speed), the state (version
// 6, the cab's, the car's +20920, the object, two values and the mode), the resources and the thumbnail from shot
void __cdecl Save_WriteGame(BYTE* st, int shot)
{
    BYTE* s = Sv_Stream(st, &g_svTime, &g_svTimeErr);
    double t[3];
    t[0] = F(double, g_sv6f33ac, 88);
    t[1] = F(double, g_sv6f33ac, 104) * g_sv64e4c8;
    t[2] = F(float, g_sv6f33ac, 544);
    Sv_Write(s, t, 24);
    Sv_Com(s, 8);
    s = Sv_Stream(st, &g_svState, &g_svStateErr);
    int v[6] = {6};
    Sv_Write(s, v, 4);
    BYTE* car = F(BYTE*, g_sv6d2078, 616);
    BYTE* cab = F(BYTE*, car, 21600);
    v[0] = cab && F(int, cab, 15440) ? 1 : 0;
    v[1] = F(int, car, 20920);
    v[2] = CallC<int>(Sv_Get212);
    v[3] = g_sv6f33b8;
    v[4] = g_sv6f33bc;
    v[5] = F(int, g_sv6cec6c, 1304);
    Sv_Write(s, v, 24);
    Sv_Com(s, 8);
    s = Sv_Stream(st, &g_svRes, &g_svResErr);
    CallThis(Sv_ResSave, g_sv696ca0 + 704, s);
    Sv_Com(s, 8);
    s = Sv_Stream(st, &g_svThumb, &g_svThumbErr);
    CallC(Sv_Thumb, s, 72, 54, shot);
    Sv_Com(s, 8);
}

} // extern "C"

// === n integers read from one ini key (FUN_507df0)

extern "C" void Ir_New(); KING_ALIAS("_Ir_New", "??0istrstream@@QAE@PAD@Z")
extern "C" void Ir_Free(); KING_ALIAS("_Ir_Free", "??1istrstream@@UAE@XZ")
extern "C" void Ir_FreeIos(); KING_ALIAS("_Ir_FreeIos", "??1ios@@UAE@XZ")
extern "C" int __cdecl Stream_ReadWord(BYTE* is, char* buf);
extern "C" long __cdecl strtol(const char* s, char** end, int base);
ASM_VAR(char, g_irNone, $L_6729a8)
ASM_VAR(char, g_irBadFmt, $L_674b28)

extern "C" {

// [FUN_507df0] out (n values, from defs) read from key of section sec in ini as numbers (the rest of the line from
// a word starting with '#' ignored; fatal on a word that is not a number); 1 when the key was there
int __cdecl Ini_ReadInts(const char* sec, const char* key, const int* defs, int* out, const char* ini, int n)
{
    char line[100];
    GetPrivateProfileStringA(sec, key, &g_irNone, line, 100, ini);
    int has = strcmp(line, &g_irNone) != 0;
    __declspec(align(8)) BYTE is[96];
    CallThis(Ir_New, is, line, 1);
    for (int k = 0; k < n; k++)
    {
        out[k] = defs[k];
        if (!has)
            continue;
        char word[96];
        if (!Stream_ReadWord(is, word) || word[0] == '#')
            continue;
        char* end = 0;
        out[k] = strtol(word, &end, 0);
        if (*end)
        {
            char msg[108];
            sprintf(msg, &g_irBadFmt, ini, sec, key, word);
            Setup_Fatal(msg);
        }
    }
    CallThis(Ir_Free, is + 12);
    CallThis(Ir_FreeIos, is + 12);
    return has;
}

} // extern "C"

// === n floats read from one ini key (FUN_507ad0)

extern "C" double __cdecl strtod(const char* s, char** end);
ASM_VAR(char, g_irBadFloat, $L_674afc)

extern "C" {

// [FUN_507ad0] as Ini_ReadInts with floats; the key counts only when it is not empty
int __cdecl Ini_ReadFloats(const char* sec, const char* key, const float* defs, float* out, const char* ini, int n)
{
    char line[100];
    GetPrivateProfileStringA(sec, key, &g_irNone, line, 100, ini);
    int has = strcmp(line, &g_irNone) && line[0];
    __declspec(align(8)) BYTE is[96];
    CallThis(Ir_New, is, line, 1);
    for (int k = 0; k < n; k++)
    {
        out[k] = defs[k];
        if (!has)
            continue;
        char word[96];
        if (!Stream_ReadWord(is, word) || word[0] == '#')
            continue;
        char* end = 0;
        out[k] = (float)strtod(word, &end);
        if (*end)
        {
            char msg[108];
            sprintf(msg, &g_irBadFloat, ini, sec, key, word);
            Setup_Fatal(msg);
        }
    }
    CallThis(Ir_Free, is + 12);
    CallThis(Ir_FreeIos, is + 12);
    return has;
}

} // extern "C"

// === whether the net session is open (FUN_5c3ac0)

extern "C" void Ns_Stream(); KING_ALIAS("_Ns_Stream", "@Stream_Construct@8")
extern "C" void Ns_Free(); KING_ALIAS("_Ns_Free", "@Gate_602bd0@8")
extern "C" void Ns_Now(); KING_ALIAS("_Ns_Now", "_Tsc_Now")
extern "C" void Ns_Rebuild(); KING_ALIAS("_Ns_Rebuild", "_Providers_Rebuild")
extern "C" void Ns_Call(); KING_ALIAS("_Ns_Call", "_Providers_Call601c70")
ASM_VAR(double, g_ns707f80, $L_707f80)
ASM_VAR(double, g_ns707f78, $L_707f78)
ASM_VAR(double, g_ns64b848, $L_64b848)
ASM_VAR(BYTE, g_ns67f880, $L_67f880)

extern "C" {

// [FUN_5c3ac0] net s (+416 started, +420 the state, +428 connected, +424 the provider): 0 when not started or
// already in a state, or when the provider fails (3); else at most every 64b848 seconds the providers are rebuilt
// and asked, a 0 answer connecting; the state becomes 2 once connected; 1
int __fastcall Net_CheckSession(BYTE* s, void*)
{
    __declspec(align(8)) BYTE tmp[32];
    CallThis(Ns_Stream, tmp);
    CallC<double>(Ns_Now);
    int r = 0;
    if (F(int, s, 416) && !F(int, s, 420))
    {
        CallC<double>(Ns_Now);
        r = 1;
        if (!F(int, s, 428))
        {
            double t = CallC<double>(Ns_Now);
            double dl = g_ns707f80 + g_ns64b848;
            if (CallC<double>(Ns_Now) > dl)
            {
                g_ns707f80 = t;
                CallC(Ns_Rebuild);
                int a = CallC<int>(Ns_Call, F(int, s, 424), 1, 8, &g_ns67f880);
                if (a == 3)
                    r = 0;
                else if (!a)
                {
                    F(int, s, 428) = 1;
                    g_ns707f78 = t;
                }
                else
                    r = 2;
            }
            if (r == 1 && F(int, s, 428))
                F(int, s, 420) = 2;
            if (r == 2)
                r = 1;
        }
        else
            F(int, s, 420) = 2;
    }
    CallThis(Ns_Free, tmp);
    return r;
}

} // extern "C"

// === the save screen's commands ($L_51e140 with its pieces up to $L_51e2d3)

extern "C" char* __cdecl Sc_Name(char* s); KING_ALIAS("_Sc_Name", "_Str_Name")
extern "C" void __cdecl Sc_Write(const char* name, int a); KING_ALIAS("_Sc_Write", "_Save_WriteEntry")
extern "C" void __cdecl Sc_Delete(const char* name); KING_ALIAS("_Sc_Delete", "_Save_DeleteEntry")
extern "C" void __fastcall Sc_ListRemove(BYTE* self, void*, const char* name); KING_ALIAS("@Sc_ListRemove@12", "@List_RemoveName@12")
extern "C" int __fastcall Sc_Activate(BYTE* self, void*); KING_ALIAS("@Sc_Activate@8", "@Win_Activate@8")
extern "C" void __cdecl Sc_ScreenSize(); KING_ALIAS("_Sc_ScreenSize", "_Carat_SetScreenSize")
extern "C" int __cdecl Sc_ForceFeedback(int on); KING_ALIAS("_Sc_ForceFeedback", "_Joy_ForceFeedback")
extern "C" BYTE* __cdecl Sc_ClearMessages(); KING_ALIAS("_Sc_ClearMessages", "_Messages_Clear1276")
extern "C" int __cdecl Sc_LeaveRace(); KING_ALIAS("_Sc_LeaveRace", "_Shell_LeaveRace")
extern "C" const char* __cdecl Sc_Text(int i); KING_ALIAS("_Sc_Text", "_Table71e858_Get")
ASM_VAR(BYTE*, g_scEsc6ced6c, $L_6ced6c)
ASM_VAR(BYTE*, g_sc6ce4f4, $L_6ce4f4)
ASM_VAR(BYTE*, g_scApp696ca0, $L_696ca0)
ASM_VAR(int, g_sc6968a4, $L_6968a4)

extern "C" {

// [$L_51e140..$L_51e2d3] (save screen vtable) command cmd of save screen s (+308 the name typed, +436 the list):
// Esc / 172 back to the Esc menu, 117 the chosen save's name copied, 282 that save deleted, 173 / 174 the game
// saved under the name (then the screen closed and the game left by s +448: the race, a flag, or the app vtable
// +36), or the "no name" text when the name is empty
void __fastcall SaveScreen_Command(BYTE* s, void*, int cmd)
{
    if (cmd > 172)
    {
        if (cmd == 173 || cmd == 174)
        {
            char buf[128];
            strcpy(buf, (char*)s + 308);
            char* n = Sc_Name(buf);
            if (!n || !*n)
            {
                strcpy((char*)s + 308, Sc_Text(103));
                return;
            }
            Sc_Write(n, 1);
            CallVirt(s - 64, 32);
            Sc_ScreenSize();
            Sc_ForceFeedback(1);
            if (g_sc6ce4f4)
                CallVirt(g_sc6ce4f4, 0, 1);
            g_sc6ce4f4 = 0;
            F(DWORD, g_scApp696ca0, 16) |= 3;
            Sc_ClearMessages();
            int mode = F(int, s, 448);
            if (mode == 1)
                Sc_LeaveRace();
            else if (mode == 2)
                g_sc6968a4 = 1;
            else
                CallVirt(g_scApp696ca0, 36);
        }
        else if (cmd == 282)
        {
            BYTE* e = F(BYTE*, F(BYTE*, s, 436), 300);
            const char* name = e ? F(const char*, e, 12) : 0;
            Sc_Delete(name);
            Sc_ListRemove(F(BYTE*, s, 436), 0, name);
        }
        return;
    }
    if (cmd == 172 || cmd == 27)
    {
        CallVirt(s - 64, 32);
        Sc_Activate(g_scEsc6ced6c, 0);
        return;
    }
    if (cmd != 117)
        return;
    BYTE* e = F(BYTE*, F(BYTE*, s, 436), 300);
    if (!e || !F(char*, e, 12))
        return;
    strcpy((char*)s + 308, F(char*, e, 12));
}

} // extern "C"

// === the replay viewer's keys ($L_5101c0, a window vtable entry)

extern "C" void Rk_SetVolume(); KING_ALIAS("_Rk_SetVolume", "_Sound_SetVolume")
extern "C" void Rk_Delete(); KING_ALIAS("_Rk_Delete", "_Obj723d44_Delete")
extern "C" void Rk_Leave(); KING_ALIAS("_Rk_Leave", "_Shell_LeaveGameScreens")
extern "C" void Rk_DestroyWindow(); KING_ALIAS("_Rk_DestroyWindow", "@CGameApp_DestroyWindowNow@12")
extern "C" void Rk_Wasm(); KING_ALIAS("_Rk_Wasm", "@Wasm_598ce0@12")
extern "C" void Rk_fclose(); KING_ALIAS("_Rk_fclose", "_fclose")
ASM_VAR(BYTE*, g_rk696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_rk6cec80, $L_6cec80)
ASM_VAR(BYTE*, g_rk6cec84, $L_6cec84)
ASM_VAR(BYTE*, g_rk6cec6c, $L_6cec6c)
ASM_VAR(char, g_rk696750, $L_696750)
ASM_VAR(char, g_rk6f68b0, $L_6f68b0)
ASM_VAR(double, g_rk64d960, $L_64d960)
ASM_VAR(double, g_rk64ca48, $L_64ca48)
ASM_VAR(double, g_rk64dd78, $L_64dd78)
ASM_VAR(double, g_rk64d0f0, $L_64d0f0)
ASM_VAR(double, g_rk64b840, $L_64b840)
ASM_VAR(double, g_rk64b5b8, $L_64b5b8)
ASM_VAR(double, g_rk64b670, $L_64b670)
ASM_VAR(double, g_rk64b660, $L_64b660)

extern "C" {

// [$L_5101c0] replay window s (its frame at s-64) gets key k: Esc (or 216) closes the replay (silence, the
// player's stop, its file closed, the windows and the game screens left); 208 / 209 speed it up / down in steps
// (the player 696ca0 +7960, a double), 210 play, 211 reverse, 212 pause, 213 / 214 step a frame either way
// (+7952), 215 the player's flag +3024
void __fastcall ReplayWnd_Key(BYTE* s, void*, int k)
{
    BYTE* p = g_rk696ca0;
    double& v = F(double, p, 7960);
    switch (k)
    {
    case 27:
    case 216:
        CallC<double>(Rk_SetVolume, 0.0f);
        CallVirt(g_rk696ca0, 144);
        if (F(void*, g_rk696ca0, 7928))
            CallC(Rk_fclose, F(void*, g_rk696ca0, 7928));
        F(int, g_rk696ca0, 7932) = 0;
        CallVirt(s - 64, 32);
        CallVirt(g_rk696ca0, 32);
        CallVirt(g_rk6cec80, 36);
        CallThis(Rk_DestroyWindow, &g_rk696750, g_rk6cec84);
        g_rk6cec84 = 0;
        CallC(Rk_Leave);
        CallThis(Rk_Wasm, &g_rk6f68b0, 2);
        if (F(int, g_rk6cec6c, 1344))
            CallC(Rk_Delete);
        break;
    case 215:
        F(int, p, 3024) = 1;
        break;
    case 209:
        if (v < g_rk64d960)
            break;
        if (!(v > g_rk64ca48))
            v = v - g_rk64d0f0;
        else if (!(v > g_rk64dd78))
            v = v - g_rk64b840;
        else if (!(v > g_rk64d0f0))
            v = v - g_rk64b5b8;
        else if (!(v > g_rk64b670))
            v = v - g_rk64b840;
        else
            v = v - g_rk64d0f0;
        break;
    case 208:
        if (v > g_rk64b660)
            break;
        if (!(v < g_rk64b670))
            v = v + g_rk64d0f0;
        else if (!(v < g_rk64d0f0))
            v = v + g_rk64b840;
        else if (!(v < g_rk64dd78))
            v = v + g_rk64b5b8;
        else if (!(v < g_rk64ca48))
            v = v + g_rk64b840;
        else
            v = v + g_rk64d0f0;
        break;
    case 210:
        v = 1.0;
        break;
    case 211:
        v = -1.0;
        break;
    case 212:
        v = 0.0;
        break;
    case 213:
        v = 0.0;
        F(int, p, 7952) = 1;
        break;
    case 214:
        v = 0.0;
        F(int, p, 7952) = -1;
        break;
    }
}

} // extern "C"

// ---------------------------------------------------------------------------
// Batch 33: the players screen

extern "C" void Ps33_ScreenGrab(); KING_ALIAS("_Ps33_ScreenGrab", "@Screen_Grab@8")
extern "C" void Ps33_WordsReset(); KING_ALIAS("_Ps33_WordsReset", "@Words295_Reset@8")
extern "C" void Ps33_ScreenSize(); KING_ALIAS("_Ps33_ScreenSize", "_Carat_UpdateScreenSize")
extern "C" void Ps33_ForceFeedback(); KING_ALIAS("_Ps33_ForceFeedback", "_Joy_ForceFeedback")
extern "C" void Ps33_Activate(); KING_ALIAS("_Ps33_Activate", "@Win_Activate@8")
extern "C" void Ps33_ListClear(); KING_ALIAS("_Ps33_ListClear", "@List_Clear@8")
extern "C" void Ps33_CarId(); KING_ALIAS("_Ps33_CarId", "_Player_CarId")
extern "C" void Ps33_FirstEntry(); KING_ALIAS("_Ps33_FirstEntry", "_Player_FirstEntry")
extern "C" void Ps33_NextEntry(); KING_ALIAS("_Ps33_NextEntry", "_Player_NextEntry")
extern "C" void Ps33_PlayerData(); KING_ALIAS("_Ps33_PlayerData", "_Ai_GetPlayerData")
extern "C" void Ps33_AddNamed(); KING_ALIAS("_Ps33_AddNamed", "@List_AddNamed@12")
extern "C" void Ps33_Select(); KING_ALIAS("_Ps33_Select", "@List_Select@12")
ASM_VAR(BYTE, ps33_screen6ce4f4, $L_6ce4f4)
ASM_VAR(BYTE, ps33_words6d1948, $L_6d1948)
ASM_VAR(BYTE*, ps33_players68b990, $L_68b990)

extern "C" {

// [$L_526bc0] the players screen opens (vtable): grabs the screen, resets the words, sizes the screen, stops
// force feedback, activates the window and fills its list (+1032) with the names (+48 of their data) of the
// players along the current car's entries, then selects +1036
void __fastcall PlayersScreen_Open(BYTE* self, void*)
{
    BYTE d[192];
    CallThis(Ps33_ScreenGrab, &ps33_screen6ce4f4);
    CallThis(Ps33_WordsReset, &ps33_words6d1948);
    CallC(Ps33_ScreenSize);
    CallC(Ps33_ForceFeedback, 0);
    CallThis(Ps33_Activate, self);
    CallThis(Ps33_ListClear, F(BYTE*, self, 1032));
    int car;
    CallC(Ps33_CarId, &car);
    BYTE* it;
    CallC(Ps33_FirstEntry, &it, car);
    while (it != ps33_players68b990) {
        CallC(Ps33_PlayerData, it, d);
        CallThis(Ps33_AddNamed, F(BYTE*, self, 1032), F(int, d, 48));
        CallC(Ps33_CarId, &car);
        BYTE* nx;
        it = *CallC<BYTE**>(Ps33_NextEntry, &nx, car);
    }
    CallThis(Ps33_Select, F(BYTE*, self, 1032), F(int, self, 1036));
}

}
