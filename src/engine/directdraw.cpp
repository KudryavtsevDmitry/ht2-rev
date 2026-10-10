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
ASM_PROC(FUN_5d2ba0)    // log (text, value)
ASM_PROC(FUN_5d79d0)
extern "C" void FUN_5d8260(); KING_ALIAS("_FUN_5d8260", "_Dd_Release")
ASM_PROC(FUN_5d75a0)

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
