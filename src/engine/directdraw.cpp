// directdraw.cpp - small functions of the unit at 0x5d3fd0 of king.masm:
// the DirectDraw device: vertical blank, flip waits, the 565 pixel format
// check, video memory, the adapter name, and the requested display mode.
//
// Building with  nmake ASM_DIRECTDRAW=1  makes king.masm assemble the
// original procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" char* __cdecl strncpy(char* d, const char* s, size_t n);

extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")    // DirectDraw error text (hr, where)
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")    // log (text, value)
extern "C" void FUN_5d79d0(); KING_ALIAS("_FUN_5d79d0", "_Dd_CopySprite")
extern "C" void FUN_5d8260(); KING_ALIAS("_FUN_5d8260", "_Dd_Release")
extern "C" void FUN_5d75a0(); KING_ALIAS("_FUN_5d75a0", "_Dd_CreateDevices")

KING_VAR(int, g_bitDepth, 720890)
KING_VAR(int, g_bitDepth2, 720894)
KING_VAR(int, g_width, 72089c)
KING_VAR(int, g_height, 7208a0)
KING_VAR(int, g_is565, 7208ac)
KING_VAR(int, g_is565b, 7208b0)
KING_VAR(int, g_ddMode, 7208ec)
KING_VAR(int, g_ddWindowed, 7208f0)
KING_VAR(BYTE*, g_ddFront, 7208f8)
KING_VAR(BYTE*, g_directDraw, 7208fc)
KING_VAR(int, g_flipPending, 720928)
KING_VAR(AsmLabel, s_checkPf, 67fe70)

template <class... A>
static __forceinline int Com(BYTE* com, unsigned off, A... args)
{
    return (*(int(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

KING_VAR(BYTE*, g_d3d, 72090c)
KING_VAR(BYTE*, g_dd, 720910)
KING_VAR(BYTE*, g_device, 720908)
KING_VAR(BYTE*, g_ddPalette, 7208f4)
KING_VAR(BYTE*, g_720900, 720900)
KING_VAR(BYTE*, g_720904, 720904)
KING_VAR(int, g_7208b4, 7208b4)
KING_VAR(int, g_waitVsync, 7208a8)

extern "C" {

// [FUN_5d41e0]
void __cdecl Dd_WaitVsync()
{
    if (g_ddMode)
        Com(g_directDraw, 128, 0);
}

// [FUN_5d4450]
void __cdecl Dd_WaitFlip()
{
    if (!g_flipPending)
        return;
    while (Com(g_ddFront, 72, 2)) {
    }
    g_flipPending = 0;
}

// [FUN_5d4ae0] notes whether the surface is 16-bit 565 RGB
void __cdecl Dd_CheckPixelFormat(BYTE* surface)
{
    if (!surface)
        return;
    DWORD desc[31];
    for (int k = 0; k < 31; k++)
        desc[k] = 0;
    desc[0] = 124;
    int hr = Com(surface, 88, desc);
    if (hr)
        CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, (const char*)s_checkPf), 0);
    if (!(desc[19] & 64) || desc[21] != 16)
        return;
    if (desc[24] == 31 && desc[23] == 2016 && desc[22] == 63488) {
        g_is565 = 1;
        g_is565b = 1;
    } else {
        g_is565 = 0;
        g_is565b = 0;
    }
}

static DWORD AvailableVidMem(BYTE* dd, DWORD caps0)
{
    DWORD caps[4] = {caps0, 0, 0, 0};
    DWORD total;
    DWORD freeMem = (DWORD)dd;
    Com(dd, 92, caps, &total, &freeMem);
    return freeMem;
}

// [FUN_5d71b0] free video memory
DWORD __cdecl Dd_FreeVideoMemory(BYTE* dd)
{
    return AvailableVidMem(dd, 4096);
}

// [FUN_5d71f0] free local video memory
DWORD __cdecl Dd_FreeLocalVideoMemory(BYTE* dd)
{
    return AvailableVidMem(dd, 0x10000000);
}

// [FUN_5d7230] the adapter's description, up to its third word
void __cdecl Dd_AdapterName(BYTE* dd, char* out)
{
    BYTE id[1072];
    Com(dd, 108, id, 0);
    strncpy(out, (const char*)id + 512, 255);
    int spaces = 0;
    for (int i = 0; i < 256; i++) {
        char c = out[i];
        if (c == ' ')
            spaces++;
        if (!c)
            return;
        if (spaces == 3) {
            out[i] = 0;
            return;
        }
    }
}

// [FUN_5d7740]
void __cdecl Com_Release(BYTE* com)
{
    if (com)
        Com(com, 8);
}

// [FUN_5d79a0]
int __cdecl Dd_Blit7(int a, int b, int c, int d, int e, int f, int unused, int g)
{
    return CallC<int>(FUN_5d79d0, a, b, c, d, e, f, g);
}

// [FUN_5d8190]
void __cdecl Dd_DefaultMode(int* w, int* h, int* c, int* flags)
{
    *w = 640;
    *h = 480;
    *c = 2;
    *flags = 15872;
}

// [FUN_5d81c0] switches the display mode when it changes
void __cdecl Dd_RequestMode(int w, int h, int bits, int flags)
{
    if (bits == -1)
        bits = g_bitDepth;
    g_bitDepth2 = bits;
    g_bitDepth = bits;
    g_height = h;
    int mode = g_ddMode;
    g_width = w;
    if (flags == -1) {
        if (mode == 0)
            flags = 0;
        else if (mode == 1)
            flags = 512;
        else if (mode == 2)
            flags = 1536;
    }
    int m = 0;
    if (flags & 512)
        m = 1;
    if (flags & 1024)
        m = 2;
    if (mode == m && g_ddWindowed == ((flags >> 12) & 1))
        return;
    CallC(FUN_5d8260);
    CallC(FUN_5d75a0, flags);
}

// [FUN_5d8350]
int __cdecl Dd_BitDepth()
{
    return g_bitDepth2;
}

}

extern "C" {

// [FUN_5d8260] releases the DirectDraw/Direct3D objects and restores the
// display
void __cdecl Dd_Release()
{
    if (!g_d3d)
        return;
    if (g_device) {
        Com(g_device, 8);
        g_device = 0;
    }
    if (g_ddPalette) {
        Com(g_ddPalette, 8);
        g_ddPalette = 0;
    }
    if (g_720900) {
        Com(g_720900, 8);
        g_720900 = 0;
    }
    if (g_directDraw && !g_7208b4) {
        Com(g_directDraw, 8);
        g_directDraw = 0;
        g_waitVsync = 0;
    }
    if (g_ddFront) {
        Com(g_ddFront, 8);
        g_ddFront = 0;
    }
    if (g_720904) {
        Com(g_720904, 8);
        g_720904 = 0;
    }
    Com(g_dd, 76);
    Com(g_d3d, 8);
    g_d3d = 0;
    Com(g_dd, 80, g_hWnd, 8);
    Com(g_dd, 8);
    g_dd = 0;
    g_ddMode = 0;
    g_ddWindowed = 0;
}

}

// ---------------------------------------------------------------------------
extern "C" void FUN_5d8460(); KING_ALIAS("_FUN_5d8460", "_Dd_ChooseTextureFormat")
KING_VAR(int, g_7208b8, 7208b8)
KING_VAR(int, g_72092c, 72092c)
KING_VAR(BYTE*, g_7208e0, 7208e0)
KING_VAR(AsmLabel, s_67fdc0, 67fdc0)
KING_VAR(AsmLabel, s_67fdb8, 67fdb8)
extern "C" int __stdcall Dd_EnumDevice(GUID* guid, char*, char*, BYTE* c);
extern "C" void* g_impDirectDrawEnumerateA; KING_ALIAS("_g_impDirectDrawEnumerateA", "__imp__DirectDrawEnumerateA@8")
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// a COM method at vtbl+off
template <class R = long, class... A>
static R Dd_ComCall(void* p, int off, A... a)
{
    return ((R(__stdcall*)(void*, A...))(*(void***)p)[off / 4])(p, a...);
}

// [FUN_5d4200..$L_5d430b] locks the back surface (restoring it when lost); returns its memory and *pitch, 0 when not in a DirectDraw mode
extern "C" BYTE* __cdecl Dd_LockBack(int* pitch)
{
    if (!g_ddMode || g_7208b8)
        return 0;
    DWORD desc[31];
    if (Dd_ComCall(g_directDraw, 96) == (long)0x887601C2)
    {
        long hr = Dd_ComCall(g_directDraw, 108);
        if (hr)
        {
            char text[256];
            strcpy(text, s_67fdc0);
            strcat(text, CallC<const char*>(FUN_5d4c10, hr, 0));
            Sys_Fatal(text, 0);
        }
    }
    desc[0] = 124;
    desc[1] = 8387054;
    long failed = Dd_ComCall(g_directDraw, 100, (void*)0, desc, 1, (void*)0) != 0;
    if (failed)
        Sys_Fatal(CallC<const char*>(FUN_5d4c10, failed, (const char*)s_67fdb8), 0);
    *pitch = desc[4];
    return (BYTE*)desc[9];
}

// [FUN_5d75a0..$L_5d7684] picks the DirectDraw devices by enumeration for the mode flags; 0 (and no DirectDraw mode) without flags 0x600
extern "C" int __cdecl Dd_CreateDevices(DWORD flags)
{
    DWORD s[52];
    memset(s, 0, sizeof s);
    s[51] = 0x80070057;
    if (!(flags & 0x600))
    {
        g_ddMode = 0;
        return 0;
    }
    s[0] = g_width;
    s[3] = flags | 0x8000;
    s[2] = g_bitDepth;
    s[1] = g_height;
    g_72092c = g_deviceIndex;
    CallStd((void (*)())g_impDirectDrawEnumerateA, (void*)Dd_EnumDevice, s);
    if (s[51])
        Sys_Fatal(CallC<const char*>(FUN_5d4c10, s[51], (char*)&s[31]), 0);
    g_dd = (BYTE*)s[10];
    g_d3d = (BYTE*)s[11];
    g_ddFront = (BYTE*)s[14];
    g_device = (BYTE*)s[13];
    g_720904 = (BYTE*)s[12];
    g_directDraw = (BYTE*)s[15];
    g_720900 = (BYTE*)s[16];
    g_ddPalette = (BYTE*)s[30];
    g_7208e0 = (BYTE*)s[8];
    g_ddMode = (flags & 0x400) ? 2 : 1;
    if (flags & 0x1000)
    {
        g_ddWindowed = 1;
        CallC(FUN_5d8460, g_bitDepth2, -1);
    }
    else
        g_ddWindowed = 0;
    g_flipPending = 0;
    g_7208b4 = (flags >> 13) & 1;
    return 0;
}

// ---------------------------------------------------------------------------
extern "C" void FUN_5b6710(); KING_ALIAS("_FUN_5b6710", "@Window_GetRect@12")
extern "C" void FUN_5e4250(); KING_ALIAS("_FUN_5e4250", "@Windows_SetActive@12")

// [FUN_5d85f0] the rect of the window at +12 into out
extern "C" void* __fastcall Win_GetRect(BYTE* self, void*, void* out)
{
    CallThis(FUN_5b6710, F(BYTE*, self, 12), out);
    return out;
}

// [FUN_5d8610] flags 3 and makes this the app's active window
extern "C" int __fastcall Win_Activate(BYTE* self, void*)
{
    F(DWORD, self, 16) |= 3;
    return CallThis<int>(FUN_5e4250, g_pApp, self);
}


// ---------------------------------------------------------------------------
extern "C" void $L_5d8360(); KING_ALIAS("_$L_5d8360", "_Dd_TextureFormat@8")     // texture format enumeration callback
KING_VAR(int, g_dd7208ac, 7208ac)
KING_VAR(int, g_dd7208b0, 7208b0)
KING_VAR(int, g_dd720894, 720894)
KING_VAR(int, g_dd720898, 720898)
KING_VAR(BYTE*, g_ddDevice, 720908)

// [FUN_5d8460] picks the texture format for a kind (b, -1: 7208ac) from the device's formats; without a device kind 2 means 4 bytes
extern "C" void __cdecl Dd_ChooseTextureFormat(int kind, int b)
{
    if (!g_ddDevice)
    {
        g_dd720894 = kind;
        if (kind == 2)
            g_dd720898 = 4;
        return;
    }
    if (b == -1)
        b = g_dd7208ac;
    g_dd7208b0 = b;
    DWORD ctx[52];
    memset(ctx, 0, sizeof ctx);
    ctx[51] = 0x80070057;
    ctx[2] = kind;
    ((long(__stdcall*)(BYTE*, void*, void*))(*(void***)g_ddDevice)[4])(g_ddDevice, (void*)$L_5d8360, ctx);
    g_dd720894 = kind;
    g_dd720898 = ctx[9];
}

// ---------------------------------------------------------------------------
KING_VAR(BYTE*, g_ddPrimary2, 7208fc)
KING_VAR(BYTE*, g_ddLocked2, 7208a8)
KING_VAR(int, g_ddWidth2, 72089c)
KING_VAR(int, g_ddHeight2, 7208a0)
KING_VAR(int, g_ddWindowed2, 7208f0)
KING_VAR(int, g_dd7208b8b, 7208b8)
KING_VAR(AsmLabel, s_dd67fdf0, 67fdf0)
extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")

// [FUN_5d4330] fills a rectangle (clipped to the screen) of the primary surface with a color, unlocking it first and locking it again after
extern "C" void __cdecl Dd_FillRect(int x0, int y0, unsigned x1, unsigned y1, DWORD color)
{
    if (g_ddLocked2)
    {
        if (!Dd_ComCall(g_ddPrimary2, 128, 0))
            g_ddLocked2 = 0;
        else if (g_ddLocked2)
            return;
    }
    DWORD fx[25];
    fx[0] = 100;
    fx[20] = color;
    if (x1 > (unsigned)g_ddWidth2)
        x1 = g_ddWidth2;
    if (y1 > (unsigned)g_ddHeight2)
        y1 = g_ddHeight2;
    RECT r = {x0, y0, (LONG)x1, (LONG)y1};
    long hr = Dd_ComCall(g_ddPrimary2, 20, &r, (void*)0, (void*)0, 16778240, fx);
    if (hr)
        CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, (char*)s_dd67fdf0), 0);
    DWORD desc[31];
    desc[0] = 124;
    desc[1] = 8387054;
    if (g_ddWindowed2 || g_dd7208b8b)
        return;
    if (!Dd_ComCall(g_ddPrimary2, 100, (void*)0, desc, 1, 0))
        g_ddLocked2 = (BYTE*)desc[9];
}


// ---------------------------------------------------------------------------
// Display mode matching.

// [$L_5d4b90] display mode enumeration: marks the request r (+24) when the mode m has its size and the depth
// (1: palette, 2: 16 bit, 4: 32 bit); 0 stops
extern "C" int __stdcall Dd_MatchMode(BYTE* m, int* r)
{
    if (*(int*)(m + 12) != r[0] || *(int*)(m + 8) != r[1])
        return 1;
    switch (r[2]) {
    case 1:
        if (!(m[76] & 32))
            return 1;
        r[6] = 1;
        return 0;
    case 2:
        if (!(m[76] & 64) || *(int*)(m + 84) != 16)
            return 1;
        r[6] = 1;
        return 1;
    case 4:
        if (!(m[76] & 64) || *(int*)(m + 84) != 32)
            return 1;
        r[6] = 1;
        return 0;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// The window frame size.

extern "C" void FUN_5d2e70(); KING_ALIAS("_FUN_5d2e70", "_Sys_Fatal")
KING_VAR(AsmLabel, s_q56_682a30, 682a30)
// [FUN_5d8540] the frame size added by the overlapped window style: 2 the width, 4 the height, else 0
extern "C" int __cdecl Wnd_FrameExtra(int edge)
{
    RECT r = {100, 100, 200, 200};
    BOOL ok = AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
    if (!ok)
        CallC(FUN_5d2e70, (const char*)s_q56_682a30, ok);
    if (edge == 2)
        return 100 - 200 - r.left + r.right;
    if (edge == 4)
        return 100 - 200 - r.top + r.bottom;
    return 0;
}

// [$L_5d6e10] enumeration callback: when record a (+12) is the wanted one of b (+28), its 32 bytes go to b + 68
// and the enumeration stops (0)
extern "C" int __stdcall Dd_MatchRecord(const BYTE* a, BYTE* b)
{
    if (F(int, a, 12) != F(int, b, 28))
        return 1;
    memcpy(b + 68, a, 32);
    return 0;
}

KING_VAR(AsmLabel, g_q105_720870, 720870)
KING_VAR(AsmLabel, g_q105_720128, 720128)
KING_VAR(AsmLabel, g_q105_720148, 720148)
KING_VAR(AsmLabel, g_q105_720728, 720728)
// [$L_5d8360] texture format callback: keeps the 555, 4444 and 565 RGB formats (720870, 720128, 720148; ctx +36
// bits 2, 4 and 1) and the 16-bit bump format (720728, bit 7); always goes on
extern "C" int __stdcall Dd_TextureFormat(const BYTE* pf, BYTE* ctx)
{
    DWORD f = F(DWORD, pf, 4);
    if (f & 64) {
        if (F(int, pf, 24) == 31 && F(int, pf, 20) == 992 && F(int, pf, 16) == 31744 && !F(int, pf, 28)) {
            memcpy((BYTE*)g_q105_720870, pf, 32);
            F(DWORD, ctx, 36) |= 4;
        }
        if (F(int, pf, 24) == 15 && F(int, pf, 20) == 240 && F(int, pf, 16) == 3840 && F(int, pf, 28) == 61440) {
            memcpy((BYTE*)g_q105_720128, pf, 32);
            F(DWORD, ctx, 36) |= 16;
        }
        if (F(int, pf, 24) == 31 && F(int, pf, 20) == 2016 && F(int, pf, 16) == 63488) {
            memcpy((BYTE*)g_q105_720148, pf, 32);
            F(DWORD, ctx, 36) |= 2;
        }
    } else if ((f & 0x80000) && F(int, pf, 12) == 16 && F(int, pf, 16) == 255 && F(int, pf, 20) == 65280) {
        memcpy((BYTE*)g_q105_720728, pf, 32);
        F(DWORD, ctx, 36) |= 128;
    }
    return 1;
}

// DirectDraw error text: "<where> <message>" in a static buffer. The first
// cases of the original also append "(unknown)" after their message.
KING_VAR(AsmLabel, s_dderr67fe88, 67fe88)
KING_VAR(AsmLabel, s_dderr67fed8, 67fed8)
KING_VAR(AsmLabel, s_dderr67ff28, 67ff28)
KING_VAR(AsmLabel, s_dderr67ff90, 67ff90)
KING_VAR(AsmLabel, s_dderr67ffb0, 67ffb0)
KING_VAR(AsmLabel, s_dderr67ffd4, 67ffd4)
KING_VAR(AsmLabel, s_dderr680008, 680008)
KING_VAR(AsmLabel, s_dderr680054, 680054)
KING_VAR(AsmLabel, s_dderr680094, 680094)
KING_VAR(AsmLabel, s_dderr6800b8, 6800b8)
KING_VAR(AsmLabel, s_dderr6800ec, 6800ec)
KING_VAR(AsmLabel, s_dderr680150, 680150)
KING_VAR(AsmLabel, s_dderr680184, 680184)
KING_VAR(AsmLabel, s_dderr6801ac, 6801ac)
KING_VAR(AsmLabel, s_dderr68026c, 68026c)
KING_VAR(AsmLabel, s_dderr6802b0, 6802b0)
KING_VAR(AsmLabel, s_dderr680304, 680304)
KING_VAR(AsmLabel, s_dderr680370, 680370)
KING_VAR(AsmLabel, s_dderr6803d0, 6803d0)
KING_VAR(AsmLabel, s_dderr680420, 680420)
KING_VAR(AsmLabel, s_dderr680458, 680458)
KING_VAR(AsmLabel, s_dderr6804ac, 6804ac)
KING_VAR(AsmLabel, s_dderr680500, 680500)
KING_VAR(AsmLabel, s_dderr68054c, 68054c)
KING_VAR(AsmLabel, s_dderr680580, 680580)
KING_VAR(AsmLabel, s_dderr6805cc, 6805cc)
KING_VAR(AsmLabel, s_dderr680610, 680610)
KING_VAR(AsmLabel, s_dderr680660, 680660)
KING_VAR(AsmLabel, s_dderr6806e0, 6806e0)
KING_VAR(AsmLabel, s_dderr680788, 680788)
KING_VAR(AsmLabel, s_dderr680800, 680800)
KING_VAR(AsmLabel, s_dderr680838, 680838)
KING_VAR(AsmLabel, s_dderr680888, 680888)
KING_VAR(AsmLabel, s_dderr6808c8, 6808c8)
KING_VAR(AsmLabel, s_dderr68091c, 68091c)
KING_VAR(AsmLabel, s_dderr6809a4, 6809a4)
KING_VAR(AsmLabel, s_dderr6809c8, 6809c8)
KING_VAR(AsmLabel, s_dderr680a0c, 680a0c)
KING_VAR(AsmLabel, s_dderr680a70, 680a70)
KING_VAR(AsmLabel, s_dderr680aac, 680aac)
KING_VAR(AsmLabel, s_dderr680b2c, 680b2c)
KING_VAR(AsmLabel, s_dderr680bb8, 680bb8)
KING_VAR(AsmLabel, s_dderr680c38, 680c38)
KING_VAR(AsmLabel, s_dderr680c94, 680c94)
KING_VAR(AsmLabel, s_dderr680cf0, 680cf0)
KING_VAR(AsmLabel, s_dderr680d60, 680d60)
KING_VAR(AsmLabel, s_dderr680da0, 680da0)
KING_VAR(AsmLabel, s_dderr680dd0, 680dd0)
KING_VAR(AsmLabel, s_dderr680e2c, 680e2c)
KING_VAR(AsmLabel, s_dderr680ee0, 680ee0)
KING_VAR(AsmLabel, s_dderr680f10, 680f10)
KING_VAR(AsmLabel, s_dderr680f84, 680f84)
KING_VAR(AsmLabel, s_dderr680fe0, 680fe0)
KING_VAR(AsmLabel, s_dderr68104c, 68104c)
KING_VAR(AsmLabel, s_dderr6810d0, 6810d0)
KING_VAR(AsmLabel, s_dderr6810e4, 6810e4)
KING_VAR(AsmLabel, s_dderr681114, 681114)
KING_VAR(AsmLabel, s_dderr681188, 681188)
KING_VAR(AsmLabel, s_dderr6811b0, 6811b0)
KING_VAR(AsmLabel, s_dderr6811f8, 6811f8)
KING_VAR(AsmLabel, s_dderr681260, 681260)
KING_VAR(AsmLabel, s_dderr6812a0, 6812a0)
KING_VAR(AsmLabel, s_dderr6812d0, 6812d0)
KING_VAR(AsmLabel, s_dderr681330, 681330)
KING_VAR(AsmLabel, s_dderr68139c, 68139c)
KING_VAR(AsmLabel, s_dderr6813d0, 6813d0)
KING_VAR(AsmLabel, s_dderr681434, 681434)
KING_VAR(AsmLabel, s_dderr681474, 681474)
KING_VAR(AsmLabel, s_dderr681490, 681490)
KING_VAR(AsmLabel, s_dderr6814b4, 6814b4)
KING_VAR(AsmLabel, s_dderr681520, 681520)
KING_VAR(AsmLabel, s_dderr68154c, 68154c)
KING_VAR(AsmLabel, s_dderr681594, 681594)
KING_VAR(AsmLabel, s_dderr6815e8, 6815e8)
KING_VAR(AsmLabel, s_dderr681644, 681644)
KING_VAR(AsmLabel, s_dderr68166c, 68166c)
KING_VAR(AsmLabel, s_dderr6816b0, 6816b0)
KING_VAR(AsmLabel, s_dderr6816dc, 6816dc)
KING_VAR(AsmLabel, s_dderr681720, 681720)
KING_VAR(AsmLabel, s_dderr681768, 681768)
KING_VAR(AsmLabel, s_dderr68179c, 68179c)
KING_VAR(AsmLabel, s_dderr681818, 681818)
KING_VAR(AsmLabel, s_dderr681850, 681850)
KING_VAR(AsmLabel, s_dderr6818a4, 6818a4)
KING_VAR(AsmLabel, s_dderr6818fc, 6818fc)
KING_VAR(AsmLabel, s_dderr68194c, 68194c)
KING_VAR(AsmLabel, s_dderr6819c8, 6819c8)
KING_VAR(AsmLabel, s_dderr681a58, 681a58)
KING_VAR(AsmLabel, s_dderr681aac, 681aac)
KING_VAR(AsmLabel, s_dderr681ad4, 681ad4)
KING_VAR(AsmLabel, s_dderr681b2c, 681b2c)
KING_VAR(AsmLabel, s_dderr681b74, 681b74)
KING_VAR(AsmLabel, s_dderr681bd0, 681bd0)
KING_VAR(AsmLabel, s_dderr681c34, 681c34)
KING_VAR(AsmLabel, s_dderr681ca8, 681ca8)
KING_VAR(AsmLabel, s_dderr681ccc, 681ccc)
KING_VAR(AsmLabel, s_dderr681d04, 681d04)
KING_VAR(AsmLabel, s_dderr681d78, 681d78)
KING_VAR(AsmLabel, s_dderr681dfc, 681dfc)
KING_VAR(AsmLabel, s_dderr681e7c, 681e7c)
KING_VAR(AsmLabel, s_dderr681eec, 681eec)
KING_VAR(AsmLabel, s_dderr681f48, 681f48)
KING_VAR(AsmLabel, s_dderr681f80, 681f80)
KING_VAR(AsmLabel, s_dderr681fc0, 681fc0)
KING_VAR(AsmLabel, s_dderr681ffc, 681ffc)
KING_VAR(AsmLabel, s_dderr682080, 682080)
KING_VAR(AsmLabel, s_dderr682094, 682094)
KING_VAR(AsmLabel, s_dderr6820a8, 6820a8)
KING_VAR(AsmLabel, s_dderr6820c4, 6820c4)
KING_VAR(AsmLabel, s_dderr6820e0, 6820e0)
KING_VAR(AsmLabel, s_dderr6820fc, 6820fc)
KING_VAR(AsmLabel, s_dderr682114, 682114)
KING_VAR(AsmLabel, s_dderr68212c, 68212c)
KING_VAR(AsmLabel, s_dderr682148, 682148)
KING_VAR(AsmLabel, s_dderr682160, 682160)
KING_VAR(AsmLabel, s_dderr682174, 682174)
KING_VAR(AsmLabel, s_dderr682188, 682188)
KING_VAR(AsmLabel, s_dderr6821a4, 6821a4)
KING_VAR(AsmLabel, s_dderr6821c0, 6821c0)
KING_VAR(AsmLabel, s_dderr6821dc, 6821dc)
KING_VAR(AsmLabel, s_dderr6821f8, 6821f8)
KING_VAR(AsmLabel, s_dderr682214, 682214)
KING_VAR(AsmLabel, s_dderr682230, 682230)
KING_VAR(AsmLabel, s_dderr68224c, 68224c)
KING_VAR(AsmLabel, s_dderr682268, 682268)
KING_VAR(AsmLabel, s_dderr682280, 682280)
KING_VAR(AsmLabel, s_dderr682298, 682298)
KING_VAR(AsmLabel, s_dderr6822b0, 6822b0)
KING_VAR(AsmLabel, s_dderr6822c8, 6822c8)
KING_VAR(AsmLabel, s_dderr6822e0, 6822e0)
KING_VAR(AsmLabel, s_dderr6822f8, 6822f8)
KING_VAR(AsmLabel, s_dderr682310, 682310)
KING_VAR(AsmLabel, s_dderr682328, 682328)
KING_VAR(AsmLabel, s_dderr682340, 682340)
KING_VAR(AsmLabel, s_dderr682358, 682358)
KING_VAR(AsmLabel, s_dderr682370, 682370)
KING_VAR(AsmLabel, s_dderr682388, 682388)
KING_VAR(AsmLabel, s_dderr6823a0, 6823a0)
KING_VAR(AsmLabel, s_dderr6823b8, 6823b8)
KING_VAR(AsmLabel, s_dderr6823d0, 6823d0)
KING_VAR(AsmLabel, s_dderr6823e8, 6823e8)
KING_VAR(AsmLabel, s_dderr682400, 682400)
KING_VAR(AsmLabel, s_dderr682418, 682418)
KING_VAR(AsmLabel, s_dderr682430, 682430)
KING_VAR(AsmLabel, s_dderr682448, 682448)
KING_VAR(AsmLabel, s_dderr682460, 682460)
KING_VAR(AsmLabel, s_dderr682478, 682478)
KING_VAR(AsmLabel, s_dderr682490, 682490)
KING_VAR(AsmLabel, s_dderr6824a8, 6824a8)
KING_VAR(AsmLabel, s_dderr6824c0, 6824c0)
KING_VAR(AsmLabel, s_dderr6824d8, 6824d8)
KING_VAR(AsmLabel, s_dderr6824f0, 6824f0)
KING_VAR(AsmLabel, s_dderr682508, 682508)
KING_VAR(AsmLabel, s_dderr682520, 682520)
KING_VAR(AsmLabel, s_dderr682538, 682538)
KING_VAR(AsmLabel, s_dderr682550, 682550)
KING_VAR(AsmLabel, s_dderr682568, 682568)
KING_VAR(AsmLabel, s_dderr682580, 682580)
KING_VAR(AsmLabel, s_dderr682598, 682598)
KING_VAR(AsmLabel, s_dderr6825b0, 6825b0)
KING_VAR(AsmLabel, s_dderr6825c8, 6825c8)
KING_VAR(AsmLabel, s_dderr6825e0, 6825e0)
KING_VAR(AsmLabel, s_dderr6825f8, 6825f8)
KING_VAR(AsmLabel, s_dderr682610, 682610)
KING_VAR(AsmLabel, s_dderr682628, 682628)
KING_VAR(AsmLabel, s_dderr682640, 682640)
KING_VAR(AsmLabel, s_dderr682664, 682664)
KING_VAR(AsmLabel, s_dderr68267c, 68267c)
KING_VAR(AsmLabel, s_dderr682694, 682694)
KING_VAR(AsmLabel, s_dderr6826ac, 6826ac)
KING_VAR(AsmLabel, s_dderr6826c4, 6826c4)
KING_VAR(AsmLabel, s_dderr6826dc, 6826dc)
KING_VAR(AsmLabel, s_dderr6826f4, 6826f4)
KING_VAR(AsmLabel, s_dderr68270c, 68270c)
KING_VAR(AsmLabel, s_dderr682724, 682724)
KING_VAR(AsmLabel, s_dderr68273c, 68273c)
KING_VAR(AsmLabel, s_dderr682754, 682754)
KING_VAR(AsmLabel, s_dderr68276c, 68276c)
KING_VAR(AsmLabel, s_dderr682780, 682780)
KING_VAR(AsmLabel, s_dderr682798, 682798)
KING_VAR(AsmLabel, s_dderr6827b4, 6827b4)
KING_VAR(AsmLabel, s_dderr6827d0, 6827d0)
KING_VAR(AsmLabel, s_dderr6827e8, 6827e8)
KING_VAR(AsmLabel, s_dderr682800, 682800)
KING_VAR(AsmLabel, g_ddErrText, 720618)
KING_VAR(AsmLabel, s_ddErrSpace, 67b8b0)
KING_VAR(AsmLabel, s_ddErrUnknown, 67fe7c)

struct DdErrText { DWORD hr; const char* text; int unknown; };
static const DdErrText k_ddErrTexts[] = {
    { 0x80004002, s_dderr682800, 1 },
    { 0x887602BC, s_dderr6827e8, 1 },
    { 0x887602BD, s_dderr6827d0, 1 },
    { 0x8876081E, s_dderr6827b4, 1 },
    { 0x88760826, s_dderr682798, 1 },
    { 0x88760821, s_dderr682780, 1 },
    { 0x88760802, s_dderr68276c, 1 },
    { 0x887602C3, s_dderr682754, 1 },
    { 0x887602CD, s_dderr68273c, 1 },
    { 0x887602C6, s_dderr682724, 1 },
    { 0x887602C7, s_dderr68270c, 1 },
    { 0x887602CC, s_dderr6826f4, 1 },
    { 0x887602C8, s_dderr6826dc, 1 },
    { 0x887602CA, s_dderr6826c4, 1 },
    { 0x887602CB, s_dderr6826ac, 1 },
    { 0x887602C9, s_dderr682694, 1 },
    { 0x887602C2, s_dderr68267c, 1 },
    { 0x88760302, s_dderr682664, 1 },
    { 0x88760835, s_dderr682640, 1 },
    { 0x887602C1, s_dderr682628, 1 },
    { 0x887602DF, s_dderr682610, 1 },
    { 0x887602E8, s_dderr6825f8, 1 },
    { 0x88760824, s_dderr6825e0, 1 },
    { 0x887602E0, s_dderr6825c8, 1 },
    { 0x887602E3, s_dderr6825b0, 1 },
    { 0x887602E1, s_dderr682598, 1 },
    { 0x88760800, s_dderr682580, 1 },
    { 0x88760834, s_dderr682568, 1 },
    { 0x887602EE, s_dderr682550, 1 },
    { 0x887602EF, s_dderr682538, 1 },
    { 0x887602F0, s_dderr682520, 1 },
    { 0x887602E4, s_dderr682508, 1 },
    { 0x887602E5, s_dderr6824f0, 1 },
    { 0x887602E7, s_dderr6824d8, 1 },
    { 0x887602E6, s_dderr6824c0, 1 },
    { 0x887602DA, s_dderr6824a8, 1 },
    { 0x887602DB, s_dderr682490, 1 },
    { 0x887602DD, s_dderr682478, 1 },
    { 0x887602DC, s_dderr682460, 1 },
    { 0x88760307, s_dderr682448, 1 },
    { 0x88760303, s_dderr682430, 1 },
    { 0x88760836, s_dderr682418, 1 },
    { 0x88760304, s_dderr682400, 1 },
    { 0x887602FA, s_dderr6823e8, 1 },
    { 0x887602FB, s_dderr6823d0, 1 },
    { 0x887602F8, s_dderr6823b8, 1 },
    { 0x887602F9, s_dderr6823a0, 1 },
    { 0x887602DE, s_dderr682388, 1 },
    { 0x887602EB, s_dderr682370, 1 },
    { 0x887602E2, s_dderr682358, 1 },
    { 0x887602D1, s_dderr682340, 1 },
    { 0x887602D2, s_dderr682328, 1 },
    { 0x887602D9, s_dderr682310, 1 },
    { 0x887602D5, s_dderr6822f8, 1 },
    { 0x887602D3, s_dderr6822e0, 1 },
    { 0x887602D7, s_dderr6822c8, 1 },
    { 0x887602D0, s_dderr6822b0, 1 },
    { 0x887602D8, s_dderr682298, 1 },
    { 0x887602D6, s_dderr682280, 1 },
    { 0x887602D4, s_dderr682268, 1 },
    { 0x8876081D, s_dderr68224c, 1 },
    { 0x88760823, s_dderr682230, 1 },
    { 0x8876081C, s_dderr682214, 1 },
    { 0x8876081B, s_dderr6821f8, 1 },
    { 0x8876081A, s_dderr6821dc, 1 },
    { 0x88760819, s_dderr6821c0, 1 },
    { 0x8876081F, s_dderr6821a4, 1 },
    { 0x88760822, s_dderr682188, 1 },
    { 0x8876080D, s_dderr682174, 1 },
    { 0x8876080E, s_dderr682160, 1 },
    { 0x8876080C, s_dderr682148, 1 },
    { 0x8876080F, s_dderr68212c, 1 },
    { 0x88760305, s_dderr682114, 1 },
    { 0x88760306, s_dderr6820fc, 1 },
    { 0x88760818, s_dderr6820e0, 1 },
    { 0x887602E9, s_dderr6820c4, 1 },
    { 0x887602EA, s_dderr6820a8, 1 },
    { 0x88760816, s_dderr682094, 1 },
    { 0x88760005, s_dderr682080, 1 },
    { 0x8876023E, s_dderr681ffc, 1 },
    { 0x8876000A, s_dderr681fc0, 1 },
    { 0x88760014, s_dderr681f80, 1 },
    { 0x88760249, s_dderr681f48, 1 },
    { 0x88760247, s_dderr681eec, 1 },
    { 0x887601B3, s_dderr681e7c, 1 },
    { 0x88760280, s_dderr681dfc, 1 },
    { 0x88760294, s_dderr681d78, 1 },
    { 0x88760237, s_dderr681d04, 1 },
    { 0x88760190, s_dderr681ccc, 1 },
    { 0x88760028, s_dderr681ca8, 1 },
    { 0x8876026C, s_dderr681c34, 1 },
    { 0x887602BB, s_dderr681bd0, 1 },
    { 0x88760232, s_dderr681b74, 1 },
    { 0x88760037, s_dderr681b2c, 1 },
    { 0x88760245, s_dderr681ad4, 1 },
    { 0x80004005, s_dderr681aac, 1 },
    { 0x8876005A, s_dderr681a58, 1 },
    { 0x8876023B, s_dderr6819c8, 1 },
    { 0x8876023A, s_dderr68194c, 1 },
    { 0x8876024C, s_dderr6818fc, 1 },
    { 0x8876005F, s_dderr6818a4, 1 },
    { 0x88760064, s_dderr681850, 1 },
    { 0x8876006E, s_dderr681818, 1 },
    { 0x88760231, s_dderr68179c, 1 },
    { 0x88760078, s_dderr681768, 1 },
    { 0x88760082, s_dderr681720, 1 },
    { 0x80070057, s_dderr6816dc, 1 },
    { 0x88760091, s_dderr6816b0, 1 },
    { 0x88760243, s_dderr68166c, 1 },
    { 0x88760096, s_dderr681644, 1 },
    { 0x88760250, s_dderr6815e8, 1 },
    { 0x887600A0, s_dderr681594, 1 },
    { 0x887602B2, s_dderr68154c, 1 },
    { 0x887600AA, s_dderr681520, 1 },
    { 0x887600B4, s_dderr6814b4, 1 },
    { 0x8876023F, s_dderr681490, 1 },
    { 0x887600CD, s_dderr681474, 1 },
    { 0x88760238, s_dderr681434, 1 },
    { 0x887600D2, s_dderr6813d0, 1 },
    { 0x887600D7, s_dderr68139c, 1 },
    { 0x887600DC, s_dderr681330, 1 },
    { 0x887600D4, s_dderr6812d0, 1 },
    { 0x8876024A, s_dderr6812a0, 1 },
    { 0x88760240, s_dderr681260, 1 },
    { 0x88760233, s_dderr6811f8, 1 },
    { 0x887600DE, s_dderr6811b0, 1 },
    { 0x88760235, s_dderr681188, 1 },
    { 0x887600E1, s_dderr681114, 1 },
    { 0x887600E6, s_dderr6810e4, 1 },
    { 0x887600F0, s_dderr6810d0, 1 },
    { 0x88760239, s_dderr68104c, 1 },
    { 0x8876024F, s_dderr680fe0, 1 },
    { 0x887600FA, s_dderr680f84, 1 },
    { 0x88760276, s_dderr680f10, 1 },
    { 0x88760258, s_dderr680ee0, 1 },
    { 0x88760242, s_dderr680e2c, 1 },
    { 0x88760104, s_dderr680dd0, 0 },
    { 0x8876023C, s_dderr680da0, 0 },
    { 0x8876023D, s_dderr680d60, 0 },
    { 0x88760118, s_dderr680cf0, 0 },
    { 0x88760122, s_dderr680c94, 0 },
    { 0x88760136, s_dderr680c38, 0 },
    { 0x8876013C, s_dderr680bb8, 0 },
    { 0x8876013D, s_dderr680b2c, 0 },
    { 0x88760140, s_dderr680aac, 0 },
    { 0x88760244, s_dderr680a70, 0 },
    { 0x8876014A, s_dderr680a0c, 0 },
    { 0x88760246, s_dderr6809c8, 0 },
    { 0x887600FF, s_dderr6809a4, 0 },
    { 0x800401F0, s_dderr68091c, 0 },
    { 0x88760259, s_dderr6808c8, 0 },
    { 0x88760248, s_dderr680888, 0 },
    { 0x887602A8, s_dderr680838, 0 },
    { 0x8876024D, s_dderr680800, 0 },
    { 0x8876014F, s_dderr680788, 0 },
    { 0x88760154, s_dderr6806e0, 0 },
    { 0x8876015E, s_dderr680660, 0 },
    { 0x88760168, s_dderr680610, 0 },
    { 0x8007000E, s_dderr6805cc, 0 },
    { 0x8876017C, s_dderr680580, 0 },
    { 0x8876017E, s_dderr68054c, 0 },
    { 0x88760180, s_dderr680500, 0 },
    { 0x88760241, s_dderr6804ac, 0 },
    { 0x88760183, s_dderr680458, 0 },
    { 0x88760234, s_dderr680420, 0 },
    { 0x88760236, s_dderr6803d0, 0 },
    { 0x8876019A, s_dderr680370, 0 },
    { 0x887601A4, s_dderr680304, 0 },
    { 0x887601AE, s_dderr6802b0, 0 },
    { 0x887601B8, s_dderr68026c, 0 },
    { 0x887601C2, s_dderr6801ac, 0 },
    { 0x887601CC, s_dderr680184, 0 },
    { 0x887601D6, s_dderr680150, 0 },
    { 0x887601E0, s_dderr6800ec, 0 },
    { 0x887601EA, s_dderr6800b8, 0 },
    { 0x80004001, s_dderr680094, 0 },
    { 0x887601FE, s_dderr680054, 0 },
    { 0x88760208, s_dderr680008, 0 },
    { 0x8876024E, s_dderr67ffd4, 0 },
    { 0x88760219, s_dderr67ffb0, 0 },
    { 0x887602B7, s_dderr67ff90, 0 },
    { 0x8876021C, s_dderr67ff28, 0 },
    { 0x8876024B, s_dderr67fed8, 0 },
    { 0x88760230, s_dderr67fe88, 0 },
};

extern "C" {

// [FUN_5d4c10]
const char* __cdecl Dd_ErrorText(DWORD hr, const char* where)
{
    char* text = g_ddErrText;
    if (where) {
        strcpy(text, where);
        strcat(text, s_ddErrSpace);
    } else
        text[0] = 0;
    for (int i = 0; i < sizeof(k_ddErrTexts) / sizeof(k_ddErrTexts[0]); i++)
        if (k_ddErrTexts[i].hr == hr) {
            strcat(text, k_ddErrTexts[i].text);
            if (!k_ddErrTexts[i].unknown)
                return text;
            break;
        }
    strcat(text, s_ddErrUnknown);
    return text;
}

}  // extern "C"

// === creating the DirectDraw display (FUN_5d6e40)

KING_VAR(char, s_dc682810, 682810)
KING_VAR(char, s_dc682828, 682828)
KING_VAR(char, s_dc682848, 682848)
KING_VAR(char, s_dc6828d4, 6828d4)
KING_VAR(char, s_dc6828ec, 6828ec)
KING_VAR(char, s_dc682904, 682904)
KING_VAR(char, s_dc6828a8, 6828a8)
KING_VAR(char, s_dc6828c0, 6828c0)
KING_VAR(char, s_dc682864, 682864)
KING_VAR(char, s_dc682880, 682880)
KING_VAR(int, g_dc7208d8, 7208d8)
KING_VAR(AsmLabel, g_dc655480, 655480)

// the error message m into c+124; hr returned
static int Dc_Fail(BYTE* c, const char* m, int hr)
{
    sprintf((char*)c + 124, m);
    return hr;
}

extern "C" {

// [FUN_5d6e40] sets up display c (+0 width, +4 height, +8 bytes per pixel, +12 flags, +44 DirectDraw, +48
// Direct3D): the full-screen mode (0x400), the primary surface (+56) with a flip chain (0x2000) or a clipped
// window and an off-screen back surface (+60), a z-buffer (+64; 0x4000 with 3D 0x1000), the palette (+120)
// for 8-bit, the 3D device (+52, needs texture formats bits 2 and 16 of +36); 0 or the error with its text
int __cdecl Dd_CreateDisplay(BYTE* c)
{
    BYTE* dd = F(BYTE*, c, 44);
    int hr;
    DWORD fl = F(DWORD, c, 12);
    if (fl & 0x400) {
        hr = Com(F(BYTE*, c, 44), 84, F(int, c, 0), F(int, c, 4), F(int, c, 8) << 3, 0, 0);
        if (hr)
            return Dc_Fail(c, &s_dc682810, hr);
    }
    DWORD d[31];
    memset(d, 0, sizeof d);
    d[0] = 124;
    d[18] = 32;
    DWORD caps;
    if (fl & 0x2000) {
        d[1] = 33;
        caps = 536;
        d[5] = 1;
    } else {
        d[1] = 1;
        caps = 512;
    }
    d[26] = caps | ((fl & 0x1000) << 1);
    hr = Com(F(BYTE*, c, 44), 24, d, c + 56, 0);
    if (hr)
        return Dc_Fail(c, &s_dc682828, hr);
    Dd_CheckPixelFormat(F(BYTE*, c, 56));
    fl = F(DWORD, c, 12);
    if (fl & 0x2000) {
        DWORD sc[4] = {4, 0, 0, 0};
        BYTE* prim = F(BYTE*, c, 56);
        hr = Com(prim, 48, sc, c + 60);
    } else {
        memset(d, 0, sizeof d);
        d[0] = 124;
        d[1] = 7;
        if (fl & 0x1000) {
            BYTE* clip;
            Com(F(BYTE*, c, 44), 16, 0, &clip, 0);
            Com(clip, 32, 0, g_hWnd);
            Com(F(BYTE*, c, 56), 112, clip);
            d[26] = 8256;
        } else
            d[26] = 2048;
        d[3] = F(int, c, 0);
        d[2] = F(int, c, 4);
        hr = Com(F(BYTE*, c, 44), 24, d, c + 60, 0);
    }
    if (hr)
        return Dc_Fail(c, &s_dc682848, hr);
    fl = F(DWORD, c, 12);
    if ((fl & 0x4000) && (fl & 0x1000)) {
        memset(d, 0, sizeof d);
        d[2] = F(int, c, 4);
        d[3] = F(int, c, 0);
        d[0] = 124;
        d[26] = 147456;
        d[1] = 4103;
        hr = Com(F(BYTE*, c, 48), 24, F(void*, c, 100), (void*)Dd_MatchRecord, c);
        if (hr)
            return Dc_Fail(c, &s_dc6828d4, hr);
        memcpy(d + 18, c + 68, 32);
        hr = Com(F(BYTE*, c, 44), 24, d, c + 64, 0);
        if (hr)
            return Dc_Fail(c, &s_dc6828ec, hr);
        hr = Com(F(BYTE*, c, 60), 12, F(BYTE*, c, 64));
        if (hr)
            return Dc_Fail(c, &s_dc682904, hr);
    }
    if (F(int, c, 8) < 2) {
        BYTE pal[1024];
        for (int i = 0; i < 256; i++) {
            pal[i * 4] = pal[i * 4 + 1] = pal[i * 4 + 2] = 0;
            pal[i * 4 + 3] = 4;
        }
        hr = Com(F(BYTE*, c, 44), 20, 68, pal, c + 120, 0);
        if (hr)
            return Dc_Fail(c, &s_dc6828a8, hr);
        hr = Com(F(BYTE*, c, 56), 124, F(BYTE*, c, 120));
        if (hr)
            return Dc_Fail(c, &s_dc6828c0, hr);
    }
    if (!(F(DWORD, c, 12) & 0x1000))
        return hr;
    hr = Com(F(BYTE*, c, 48), 16, F(void*, c, 100), F(BYTE*, c, 60), c + 52);
    if (hr)
        return Dc_Fail(c, &s_dc682864, hr);
    if (!memcmp(F(BYTE*, c, 100), (BYTE*)g_dc655480, 16))
        g_dc7208d8 = 1;
    Com(F(BYTE*, c, 52), 16, (void*)Dd_TextureFormat, c);
    if (!(F(BYTE, c, 36) & 2) || !(F(BYTE, c, 36) & 16)) {
        sprintf((char*)c + 124, &s_dc682880);
        return 0x80070057;
    }
    return hr;
}

} // extern "C"

// === trying a DirectDraw device (the enumeration callback $L_5d72a0)

extern "C" int __stdcall D3d_ChooseDevice(const char*, const char*, const BYTE* dd, BYTE* ctx);   // video_mode.cpp
extern "C" void* g_impDirectDrawCreateEx; KING_ALIAS("_g_impDirectDrawCreateEx", "__imp__DirectDrawCreateEx@16")
KING_VAR(AsmLabel, g_ed6554a0, 6554a0)      // IID_IDirectDraw7
KING_VAR(AsmLabel, g_ed655490, 655490)      // IID_IDirect3D7
KING_VAR(char, g_ed720770, 720770)
KING_VAR(char, s_ed682924, 682924)
KING_VAR(char, s_ed682938, 682938)
KING_VAR(char, s_ed682954, 682954)
KING_VAR(char, s_ed682970, 682970)
KING_VAR(char, s_ed682984, 682984)

// releases the interface at c+off
static void Ed_Release(BYTE* c, int off)
{
    if (BYTE* p = F(BYTE*, c, off)) {
        Com(p, 8);
        F(BYTE*, c, off) = 0;
    }
}

// the display modes of c's DirectDraw (+44) enumerated for the request; E_INVALIDARG with the text when none fits
static int Ed_MatchModes(BYTE* c)
{
    if (Com(F(BYTE*, c, 44), 32, 0, 0, c, (void*)Dd_MatchMode) || !F(int, c, 24)) {
        sprintf((char*)c + 124, &s_ed682984);
        return 0x80070057;
    }
    return 0;
}

extern "C" {

// [$L_5d72a0] DirectDrawEnumerate callback: creates the DirectDraw guid for request c (52 dwords: +0 width, +4
// height, +8 bytes per pixel, +12 flags), with Direct3D (0x1000) a device by D3d_ChooseDevice, checks the mode
// (0x400) and creates the display (0x8000); the result at +204 (0 stops the enumeration and names the adapter)
int __stdcall Dd_EnumDevice(GUID* guid, char*, char*, BYTE* c)
{
    static const int k_rel[] = {52, 120, 64, 60, 56, 48, 44, 40};
    for (int off : k_rel)
        Ed_Release(c, off);
    F(int, c, 100) = 0;
    F(int, c, 28) = 0;
    F(int, c, 24) = 0;
    F(int, c, 32) = 0;
    F(int, c, 36) = 0;
    F(int, c, 80) = 0;
    F(int, c, 204) = 0x80070057;
    int hr = ((int(__stdcall*)(GUID*, BYTE**, void*, void*))g_impDirectDrawCreateEx)(guid, (BYTE**)(c + 40),
                                                                                      g_ed6554a0, 0);
    if (!hr) {
        BYTE* dd = F(BYTE*, c, 40);
        hr = Com(dd, 80, g_hWnd, (F(DWORD, c, 12) & 0x400) ? 0x105B : 8);
        if (hr)
            sprintf((char*)c + 124, &s_ed682924);
        else {
            F(BYTE*, c, 44) = dd;
            Com(dd, 4);
            if (!F(BYTE*, c, 44))
                sprintf((char*)c + 124, &s_ed682938);
            else if (F(DWORD, c, 12) & 0x1000) {
                hr = Com(dd, 0, g_ed655490, c + 48);
                if (hr || !F(BYTE*, c, 48))
                    sprintf((char*)c + 124, &s_ed682954);
                else {
                    int bpp = F(int, c, 8);
                    F(int, c, 20) = (bpp != 1) + 1;
                    if (bpp >= 1 && bpp <= 4)
                        F(int, c, 16) = 2048 >> (bpp - 1);
                    if (Com(F(BYTE*, c, 48), 12, (void*)D3d_ChooseDevice, c) || !F(int, c, 100)) {
                        sprintf((char*)c + 124, &s_ed682970);
                        hr = 0x80070057;
                    } else {
                        if (F(DWORD, c, 12) & 0x400)
                            hr = Ed_MatchModes(c);
                        else
                            F(int, c, 24) = 1;
                        if (!hr && !F(int, c, 24)) {
                            sprintf((char*)c + 124, &s_ed682984);
                            hr = 0x80070057;
                        }
                        if (!hr && (F(DWORD, c, 12) & 0x8000)) {
                            hr = Dd_CreateDisplay(c);
                            if (!hr)
                                F(DWORD, c, 32) = Dd_FreeVideoMemory(F(BYTE*, c, 44));
                        }
                    }
                }
            } else {
                hr = Ed_MatchModes(c);
                if (!hr && (F(DWORD, c, 12) & 0x8000))
                    hr = Dd_CreateDisplay(c);
            }
        }
    }
    F(int, c, 204) = hr;
    if (!hr) {
        Dd_AdapterName(F(BYTE*, c, 44), &g_ed720770);
        return 0;
    }
    if (BYTE* dd = F(BYTE*, c, 40))
        Com(dd, 80, g_hWnd, 8);
    return 1;
}

} // extern "C"

// === a surface from pixels (FUN_5d7750)

KING_VAR(AsmLabel, s_ts6829b4, 6829b4)
KING_VAR(AsmLabel, s_ts6829ac, 6829ac)
KING_VAR(AsmLabel, s_ts6829a4, 6829a4)
KING_VAR(AsmLabel, s_ts68299c, 68299c)

extern "C" {

// [FUN_5d7750] a new surface for the w x h image src (720894 bytes per pixel), cut into columns of 72089c pixels
// stacked downwards (video memory with a device 720904, else system memory); 0 with the error logged
BYTE* __cdecl Dd_SurfaceFromPixels(unsigned w, unsigned h, const BYTE* src)
{
    DWORD d[31];
    memset(d, 0, sizeof d);
    d[26] = g_720904 ? 0x800 : 0x4040;
    unsigned tile = g_width, bpp = g_bitDepth2;
    d[0] = 124;
    d[1] = 7;
    d[3] = tile;
    d[2] = (w / tile + (w % tile ? 1 : 0)) * h;
    BYTE* s = 0;
    int hr = Com(g_d3d, 24, d, &s, 0);
    if (hr) {
        const char* m = hr == (int)0x88760064 ? s_ts6829b4 : hr == (int)0x80070057 ? s_ts6829ac
                      : hr == (int)0x88760091 ? s_ts6829a4 : hr == (int)0x8876005F ? s_ts68299c : 0;
        if (m)
            CallC(FUN_5d2ba0, m, 0);
        return 0;
    }
    if (!s)
        return 0;
    DWORD l[31];
    l[0] = 124;
    l[1] = 8387054;
    BYTE* bits = 0;
    if (!Com(s, 100, 0, l, 1, 0))
        bits = (BYTE*)l[9];
    int pitch = (int)l[4];
    if (!bits)
        return s;
    for (unsigned y = 0; y < h; y++, src += bpp * w, bits += pitch) {
        const BYTE* p = src;
        BYTE* q = bits;
        for (unsigned x = 0; x < w; x += tile, q += pitch * h) {
            unsigned n = (x + tile < w ? tile : w - x) * bpp;
            memcpy(q, p, n);
            p += n;
        }
    }
    Com(s, 128, 0);
    return s;
}

} // extern "C"

// === copying a sprite to the back surface (FUN_5d79d0)

KING_VAR(int, g_bs7208b8, 7208b8)

extern "C" {

// [FUN_5d79d0] copies the w x h block at (sx, sy) of surface src to (dx, dy) of the back surface (7208fc), keyed by
// the source colour (a fast blit for video memory, a SRCCOPY blit else), unlocking it first (7208a8 its locked
// bits) and locking it again after (unless windowed, 7208f0, or 7208b8)
void __cdecl Dd_CopySprite(int sx, int sy, int dx, int dy, int w, int h, BYTE* src)
{
    if (g_waitVsync) {
        if (!Com(g_directDraw, 128, 0))
            g_waitVsync = 0;
        else if (g_waitVsync)
            return;
    }
    DWORD caps[4];
    Com(src, 56, caps);
    RECT r = {sx, sy, sx + w, sy + h};
    int hr;
    if (caps[0] & 0x10000000)
        hr = Com(g_directDraw, 28, dx, dy, src, &r, 16);
    else {
        DWORD fx[25];
        memset(fx, 0, sizeof fx);
        RECT d = {dx, dy, dx + w, dy + h};
        fx[0] = 100;
        fx[2] = 0xCC0020;
        hr = Com(g_directDraw, 20, &d, src, &r, 0x1020000, fx);
    }
    if (hr)
        CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, 0), 0);
    if (g_ddWindowed || g_bs7208b8)
        return;
    DWORD sd[31];
    memset(sd, 0, sizeof sd);
    sd[0] = 124;
    sd[1] = 8387054;
    if (!Com(g_directDraw, 100, (void*)0, sd, 1, 0))
        g_waitVsync = (int)sd[9];
}

} // extern "C"
