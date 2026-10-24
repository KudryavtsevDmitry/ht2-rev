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
ASM_PROC(FUN_5d79d0)
extern "C" void FUN_5d8260(); KING_ALIAS("_FUN_5d8260", "_Dd_Release")
extern "C" void FUN_5d75a0(); KING_ALIAS("_FUN_5d75a0", "_Dd_CreateDevices")

ASM_VAR(int, g_bitDepth, $L_720890)
ASM_VAR(int, g_bitDepth2, $L_720894)
ASM_VAR(int, g_width, $L_72089c)
ASM_VAR(int, g_height, $L_7208a0)
ASM_VAR(int, g_is565, $L_7208ac)
ASM_VAR(int, g_is565b, $L_7208b0)
ASM_VAR(int, g_ddMode, $L_7208ec)
ASM_VAR(int, g_ddWindowed, $L_7208f0)
ASM_VAR(BYTE*, g_ddFront, $L_7208f8)
ASM_VAR(BYTE*, g_directDraw, $L_7208fc)
ASM_VAR(int, g_flipPending, $L_720928)
ASM_VAR(AsmLabel, s_checkPf, $L_67fe70)

template <class... A>
static __forceinline int Com(BYTE* com, unsigned off, A... args)
{
    return (*(int(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

ASM_VAR(BYTE*, g_d3d, $L_72090c)
ASM_VAR(BYTE*, g_dd, $L_720910)
ASM_VAR(BYTE*, g_device, $L_720908)
ASM_VAR(BYTE*, g_ddPalette, $L_7208f4)
ASM_VAR(BYTE*, g_720900, $L_720900)
ASM_VAR(BYTE*, g_720904, $L_720904)
ASM_VAR(int, g_7208b4, $L_7208b4)
ASM_VAR(int, g_waitVsync, $L_7208a8)
ASM_VAR(HWND, g_hWnd, $L_720920)

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
ASM_VAR(int, g_7208b8, $L_7208b8)
ASM_VAR(int, g_72092c, $L_72092c)
ASM_VAR(BYTE*, g_7208e0, $L_7208e0)
ASM_VAR(AsmLabel, s_67fdc0, $L_67fdc0)
ASM_VAR(AsmLabel, s_67fdb8, $L_67fdb8)
extern "C" void Dd_EnumDevice(); KING_ALIAS("_Dd_EnumDevice", "$L_5d72a0")
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
    CallStd((void (*)())g_impDirectDrawEnumerateA, Dd_EnumDevice, s);
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
ASM_VAR(int, g_dd7208ac, $L_7208ac)
ASM_VAR(int, g_dd7208b0, $L_7208b0)
ASM_VAR(int, g_dd720894, $L_720894)
ASM_VAR(int, g_dd720898, $L_720898)
ASM_VAR(BYTE*, g_ddDevice, $L_720908)

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
ASM_VAR(BYTE*, g_ddPrimary2, $L_7208fc)
ASM_VAR(BYTE*, g_ddLocked2, $L_7208a8)
ASM_VAR(int, g_ddWidth2, $L_72089c)
ASM_VAR(int, g_ddHeight2, $L_7208a0)
ASM_VAR(int, g_ddWindowed2, $L_7208f0)
ASM_VAR(int, g_dd7208b8b, $L_7208b8)
ASM_VAR(AsmLabel, s_dd67fdf0, $L_67fdf0)
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
ASM_VAR(AsmLabel, s_q56_682a30, $L_682a30)
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

ASM_VAR(AsmLabel, g_q105_720870, $L_720870)
ASM_VAR(AsmLabel, g_q105_720128, $L_720128)
ASM_VAR(AsmLabel, g_q105_720148, $L_720148)
ASM_VAR(AsmLabel, g_q105_720728, $L_720728)
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
ASM_VAR(AsmLabel, s_dderr67fe88, $L_67fe88)
ASM_VAR(AsmLabel, s_dderr67fed8, $L_67fed8)
ASM_VAR(AsmLabel, s_dderr67ff28, $L_67ff28)
ASM_VAR(AsmLabel, s_dderr67ff90, $L_67ff90)
ASM_VAR(AsmLabel, s_dderr67ffb0, $L_67ffb0)
ASM_VAR(AsmLabel, s_dderr67ffd4, $L_67ffd4)
ASM_VAR(AsmLabel, s_dderr680008, $L_680008)
ASM_VAR(AsmLabel, s_dderr680054, $L_680054)
ASM_VAR(AsmLabel, s_dderr680094, $L_680094)
ASM_VAR(AsmLabel, s_dderr6800b8, $L_6800b8)
ASM_VAR(AsmLabel, s_dderr6800ec, $L_6800ec)
ASM_VAR(AsmLabel, s_dderr680150, $L_680150)
ASM_VAR(AsmLabel, s_dderr680184, $L_680184)
ASM_VAR(AsmLabel, s_dderr6801ac, $L_6801ac)
ASM_VAR(AsmLabel, s_dderr68026c, $L_68026c)
ASM_VAR(AsmLabel, s_dderr6802b0, $L_6802b0)
ASM_VAR(AsmLabel, s_dderr680304, $L_680304)
ASM_VAR(AsmLabel, s_dderr680370, $L_680370)
ASM_VAR(AsmLabel, s_dderr6803d0, $L_6803d0)
ASM_VAR(AsmLabel, s_dderr680420, $L_680420)
ASM_VAR(AsmLabel, s_dderr680458, $L_680458)
ASM_VAR(AsmLabel, s_dderr6804ac, $L_6804ac)
ASM_VAR(AsmLabel, s_dderr680500, $L_680500)
ASM_VAR(AsmLabel, s_dderr68054c, $L_68054c)
ASM_VAR(AsmLabel, s_dderr680580, $L_680580)
ASM_VAR(AsmLabel, s_dderr6805cc, $L_6805cc)
ASM_VAR(AsmLabel, s_dderr680610, $L_680610)
ASM_VAR(AsmLabel, s_dderr680660, $L_680660)
ASM_VAR(AsmLabel, s_dderr6806e0, $L_6806e0)
ASM_VAR(AsmLabel, s_dderr680788, $L_680788)
ASM_VAR(AsmLabel, s_dderr680800, $L_680800)
ASM_VAR(AsmLabel, s_dderr680838, $L_680838)
ASM_VAR(AsmLabel, s_dderr680888, $L_680888)
ASM_VAR(AsmLabel, s_dderr6808c8, $L_6808c8)
ASM_VAR(AsmLabel, s_dderr68091c, $L_68091c)
ASM_VAR(AsmLabel, s_dderr6809a4, $L_6809a4)
ASM_VAR(AsmLabel, s_dderr6809c8, $L_6809c8)
ASM_VAR(AsmLabel, s_dderr680a0c, $L_680a0c)
ASM_VAR(AsmLabel, s_dderr680a70, $L_680a70)
ASM_VAR(AsmLabel, s_dderr680aac, $L_680aac)
ASM_VAR(AsmLabel, s_dderr680b2c, $L_680b2c)
ASM_VAR(AsmLabel, s_dderr680bb8, $L_680bb8)
ASM_VAR(AsmLabel, s_dderr680c38, $L_680c38)
ASM_VAR(AsmLabel, s_dderr680c94, $L_680c94)
ASM_VAR(AsmLabel, s_dderr680cf0, $L_680cf0)
ASM_VAR(AsmLabel, s_dderr680d60, $L_680d60)
ASM_VAR(AsmLabel, s_dderr680da0, $L_680da0)
ASM_VAR(AsmLabel, s_dderr680dd0, $L_680dd0)
ASM_VAR(AsmLabel, s_dderr680e2c, $L_680e2c)
ASM_VAR(AsmLabel, s_dderr680ee0, $L_680ee0)
ASM_VAR(AsmLabel, s_dderr680f10, $L_680f10)
ASM_VAR(AsmLabel, s_dderr680f84, $L_680f84)
ASM_VAR(AsmLabel, s_dderr680fe0, $L_680fe0)
ASM_VAR(AsmLabel, s_dderr68104c, $L_68104c)
ASM_VAR(AsmLabel, s_dderr6810d0, $L_6810d0)
ASM_VAR(AsmLabel, s_dderr6810e4, $L_6810e4)
ASM_VAR(AsmLabel, s_dderr681114, $L_681114)
ASM_VAR(AsmLabel, s_dderr681188, $L_681188)
ASM_VAR(AsmLabel, s_dderr6811b0, $L_6811b0)
ASM_VAR(AsmLabel, s_dderr6811f8, $L_6811f8)
ASM_VAR(AsmLabel, s_dderr681260, $L_681260)
ASM_VAR(AsmLabel, s_dderr6812a0, $L_6812a0)
ASM_VAR(AsmLabel, s_dderr6812d0, $L_6812d0)
ASM_VAR(AsmLabel, s_dderr681330, $L_681330)
ASM_VAR(AsmLabel, s_dderr68139c, $L_68139c)
ASM_VAR(AsmLabel, s_dderr6813d0, $L_6813d0)
ASM_VAR(AsmLabel, s_dderr681434, $L_681434)
ASM_VAR(AsmLabel, s_dderr681474, $L_681474)
ASM_VAR(AsmLabel, s_dderr681490, $L_681490)
ASM_VAR(AsmLabel, s_dderr6814b4, $L_6814b4)
ASM_VAR(AsmLabel, s_dderr681520, $L_681520)
ASM_VAR(AsmLabel, s_dderr68154c, $L_68154c)
ASM_VAR(AsmLabel, s_dderr681594, $L_681594)
ASM_VAR(AsmLabel, s_dderr6815e8, $L_6815e8)
ASM_VAR(AsmLabel, s_dderr681644, $L_681644)
ASM_VAR(AsmLabel, s_dderr68166c, $L_68166c)
ASM_VAR(AsmLabel, s_dderr6816b0, $L_6816b0)
ASM_VAR(AsmLabel, s_dderr6816dc, $L_6816dc)
ASM_VAR(AsmLabel, s_dderr681720, $L_681720)
ASM_VAR(AsmLabel, s_dderr681768, $L_681768)
ASM_VAR(AsmLabel, s_dderr68179c, $L_68179c)
ASM_VAR(AsmLabel, s_dderr681818, $L_681818)
ASM_VAR(AsmLabel, s_dderr681850, $L_681850)
ASM_VAR(AsmLabel, s_dderr6818a4, $L_6818a4)
ASM_VAR(AsmLabel, s_dderr6818fc, $L_6818fc)
ASM_VAR(AsmLabel, s_dderr68194c, $L_68194c)
ASM_VAR(AsmLabel, s_dderr6819c8, $L_6819c8)
ASM_VAR(AsmLabel, s_dderr681a58, $L_681a58)
ASM_VAR(AsmLabel, s_dderr681aac, $L_681aac)
ASM_VAR(AsmLabel, s_dderr681ad4, $L_681ad4)
ASM_VAR(AsmLabel, s_dderr681b2c, $L_681b2c)
ASM_VAR(AsmLabel, s_dderr681b74, $L_681b74)
ASM_VAR(AsmLabel, s_dderr681bd0, $L_681bd0)
ASM_VAR(AsmLabel, s_dderr681c34, $L_681c34)
ASM_VAR(AsmLabel, s_dderr681ca8, $L_681ca8)
ASM_VAR(AsmLabel, s_dderr681ccc, $L_681ccc)
ASM_VAR(AsmLabel, s_dderr681d04, $L_681d04)
ASM_VAR(AsmLabel, s_dderr681d78, $L_681d78)
ASM_VAR(AsmLabel, s_dderr681dfc, $L_681dfc)
ASM_VAR(AsmLabel, s_dderr681e7c, $L_681e7c)
ASM_VAR(AsmLabel, s_dderr681eec, $L_681eec)
ASM_VAR(AsmLabel, s_dderr681f48, $L_681f48)
ASM_VAR(AsmLabel, s_dderr681f80, $L_681f80)
ASM_VAR(AsmLabel, s_dderr681fc0, $L_681fc0)
ASM_VAR(AsmLabel, s_dderr681ffc, $L_681ffc)
ASM_VAR(AsmLabel, s_dderr682080, $L_682080)
ASM_VAR(AsmLabel, s_dderr682094, $L_682094)
ASM_VAR(AsmLabel, s_dderr6820a8, $L_6820a8)
ASM_VAR(AsmLabel, s_dderr6820c4, $L_6820c4)
ASM_VAR(AsmLabel, s_dderr6820e0, $L_6820e0)
ASM_VAR(AsmLabel, s_dderr6820fc, $L_6820fc)
ASM_VAR(AsmLabel, s_dderr682114, $L_682114)
ASM_VAR(AsmLabel, s_dderr68212c, $L_68212c)
ASM_VAR(AsmLabel, s_dderr682148, $L_682148)
ASM_VAR(AsmLabel, s_dderr682160, $L_682160)
ASM_VAR(AsmLabel, s_dderr682174, $L_682174)
ASM_VAR(AsmLabel, s_dderr682188, $L_682188)
ASM_VAR(AsmLabel, s_dderr6821a4, $L_6821a4)
ASM_VAR(AsmLabel, s_dderr6821c0, $L_6821c0)
ASM_VAR(AsmLabel, s_dderr6821dc, $L_6821dc)
ASM_VAR(AsmLabel, s_dderr6821f8, $L_6821f8)
ASM_VAR(AsmLabel, s_dderr682214, $L_682214)
ASM_VAR(AsmLabel, s_dderr682230, $L_682230)
ASM_VAR(AsmLabel, s_dderr68224c, $L_68224c)
ASM_VAR(AsmLabel, s_dderr682268, $L_682268)
ASM_VAR(AsmLabel, s_dderr682280, $L_682280)
ASM_VAR(AsmLabel, s_dderr682298, $L_682298)
ASM_VAR(AsmLabel, s_dderr6822b0, $L_6822b0)
ASM_VAR(AsmLabel, s_dderr6822c8, $L_6822c8)
ASM_VAR(AsmLabel, s_dderr6822e0, $L_6822e0)
ASM_VAR(AsmLabel, s_dderr6822f8, $L_6822f8)
ASM_VAR(AsmLabel, s_dderr682310, $L_682310)
ASM_VAR(AsmLabel, s_dderr682328, $L_682328)
ASM_VAR(AsmLabel, s_dderr682340, $L_682340)
ASM_VAR(AsmLabel, s_dderr682358, $L_682358)
ASM_VAR(AsmLabel, s_dderr682370, $L_682370)
ASM_VAR(AsmLabel, s_dderr682388, $L_682388)
ASM_VAR(AsmLabel, s_dderr6823a0, $L_6823a0)
ASM_VAR(AsmLabel, s_dderr6823b8, $L_6823b8)
ASM_VAR(AsmLabel, s_dderr6823d0, $L_6823d0)
ASM_VAR(AsmLabel, s_dderr6823e8, $L_6823e8)
ASM_VAR(AsmLabel, s_dderr682400, $L_682400)
ASM_VAR(AsmLabel, s_dderr682418, $L_682418)
ASM_VAR(AsmLabel, s_dderr682430, $L_682430)
ASM_VAR(AsmLabel, s_dderr682448, $L_682448)
ASM_VAR(AsmLabel, s_dderr682460, $L_682460)
ASM_VAR(AsmLabel, s_dderr682478, $L_682478)
ASM_VAR(AsmLabel, s_dderr682490, $L_682490)
ASM_VAR(AsmLabel, s_dderr6824a8, $L_6824a8)
ASM_VAR(AsmLabel, s_dderr6824c0, $L_6824c0)
ASM_VAR(AsmLabel, s_dderr6824d8, $L_6824d8)
ASM_VAR(AsmLabel, s_dderr6824f0, $L_6824f0)
ASM_VAR(AsmLabel, s_dderr682508, $L_682508)
ASM_VAR(AsmLabel, s_dderr682520, $L_682520)
ASM_VAR(AsmLabel, s_dderr682538, $L_682538)
ASM_VAR(AsmLabel, s_dderr682550, $L_682550)
ASM_VAR(AsmLabel, s_dderr682568, $L_682568)
ASM_VAR(AsmLabel, s_dderr682580, $L_682580)
ASM_VAR(AsmLabel, s_dderr682598, $L_682598)
ASM_VAR(AsmLabel, s_dderr6825b0, $L_6825b0)
ASM_VAR(AsmLabel, s_dderr6825c8, $L_6825c8)
ASM_VAR(AsmLabel, s_dderr6825e0, $L_6825e0)
ASM_VAR(AsmLabel, s_dderr6825f8, $L_6825f8)
ASM_VAR(AsmLabel, s_dderr682610, $L_682610)
ASM_VAR(AsmLabel, s_dderr682628, $L_682628)
ASM_VAR(AsmLabel, s_dderr682640, $L_682640)
ASM_VAR(AsmLabel, s_dderr682664, $L_682664)
ASM_VAR(AsmLabel, s_dderr68267c, $L_68267c)
ASM_VAR(AsmLabel, s_dderr682694, $L_682694)
ASM_VAR(AsmLabel, s_dderr6826ac, $L_6826ac)
ASM_VAR(AsmLabel, s_dderr6826c4, $L_6826c4)
ASM_VAR(AsmLabel, s_dderr6826dc, $L_6826dc)
ASM_VAR(AsmLabel, s_dderr6826f4, $L_6826f4)
ASM_VAR(AsmLabel, s_dderr68270c, $L_68270c)
ASM_VAR(AsmLabel, s_dderr682724, $L_682724)
ASM_VAR(AsmLabel, s_dderr68273c, $L_68273c)
ASM_VAR(AsmLabel, s_dderr682754, $L_682754)
ASM_VAR(AsmLabel, s_dderr68276c, $L_68276c)
ASM_VAR(AsmLabel, s_dderr682780, $L_682780)
ASM_VAR(AsmLabel, s_dderr682798, $L_682798)
ASM_VAR(AsmLabel, s_dderr6827b4, $L_6827b4)
ASM_VAR(AsmLabel, s_dderr6827d0, $L_6827d0)
ASM_VAR(AsmLabel, s_dderr6827e8, $L_6827e8)
ASM_VAR(AsmLabel, s_dderr682800, $L_682800)
ASM_VAR(AsmLabel, g_ddErrText, $L_720618)
ASM_VAR(AsmLabel, s_ddErrSpace, $L_67b8b0)
ASM_VAR(AsmLabel, s_ddErrUnknown, $L_67fe7c)

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
