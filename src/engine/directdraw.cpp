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

ASM_PROC(FUN_5d4c10)    // DirectDraw error text (hr, where)
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
ASM_PROC($L_5d8360)     // texture format enumeration callback
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
ASM_PROC(FUN_5d4c10)

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
