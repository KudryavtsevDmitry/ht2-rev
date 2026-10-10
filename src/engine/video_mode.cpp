// video_mode.cpp - the unit at 0x5d3ab0 of king.masm: the current video mode
// (resolution, color depth, windowed flags), the 8-bit palette of the
// window, and palette updates of the DirectDraw primary surface.
//
// Building with  nmake ASM_VIDEO_MODE=1  makes king.masm assemble the
// original procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

ASM_VAR(int, g_bitDepth, $L_720890)
ASM_VAR(int, g_720898, $L_720898)
ASM_VAR(int, g_width, $L_72089c)
ASM_VAR(int, g_height, $L_7208a0)
ASM_VAR(int, g_7208a4, $L_7208a4)
ASM_VAR(int, g_waitVsync, $L_7208a8)
ASM_VAR(int, g_ddMode, $L_7208ec)
ASM_VAR(BYTE*, g_ddPalette, $L_7208f4)
ASM_VAR(BYTE*, g_directDraw, $L_7208fc)
ASM_VAR(BYTE*, g_logPalette, $L_720918)
ASM_VAR(HDC, g_paletteDc, $L_72091c)
ASM_VAR(HPALETTE, g_palette, $L_720924)
ASM_VAR(HPALETTE, g_oldPalette, $L_720718)
ASM_VAR(int, g_67fcf0, $L_67fcf0)
ASM_VAR(int, g_67fcf4, $L_67fcf4)

template <class... A>
static __forceinline int Com(BYTE* com, unsigned off, A... args)
{
    return (*(int(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

extern "C" {

// [FUN_5d3af0]
int __cdecl Video_Get720898()
{
    return g_720898;
}

// [FUN_5d3b00]
int __cdecl Video_Get7208a4()
{
    return g_7208a4;
}

// [FUN_5d3b10]
int __cdecl Video_BitDepth()
{
    return g_bitDepth;
}

// [FUN_5d3b20]
int __cdecl Video_Width()
{
    return g_width;
}

// [FUN_5d3b30]
int __cdecl Video_Height()
{
    return g_height;
}

// [FUN_5d3ca0] a gray 256-color palette for the window's device context
void __cdecl Video_CreatePalette(HDC dc)
{
    g_paletteDc = dc;
    g_logPalette = (BYTE*)LocalAlloc(0, 1032);
    *(WORD*)g_logPalette = 0x300;
    *(WORD*)(g_logPalette + 2) = 256;
    for (int i = 0; i < 256; i++) {
        g_logPalette[4 + i * 4] = (BYTE)i;
        g_logPalette[5 + i * 4] = (BYTE)i;
        g_logPalette[6 + i * 4] = (BYTE)i;
        g_logPalette[7 + i * 4] = 4;
    }
    g_palette = CreatePalette((LOGPALETTE*)g_logPalette);
    g_oldPalette = SelectPalette(dc, g_palette, 0);
    RealizePalette(dc);
}

// [FUN_5d3ec0] palette entries first..count-1 from 8-byte RGB records
void __cdecl Video_SetPalette(const BYTE* rgb, DWORD first, DWORD count)
{
    if (g_ddMode != 2)
        return;
    BYTE entries[1200];
    for (DWORD i = first; i < count; i++) {
        entries[i * 4] = rgb[i * 8 + 2];
        entries[i * 4 + 1] = rgb[i * 8 + 1];
        entries[i * 4 + 2] = rgb[i * 8];
        entries[i * 4 + 3] = 4;
    }
    if (g_waitVsync) {
        Com(g_directDraw, 128, 0);
        g_waitVsync = 0;
    }
    if (g_ddPalette)
        Com(g_ddPalette, 24, 0, first, count, entries);
}

// [FUN_5d3f70]
int __cdecl Video_Get67fcf0()
{
    return g_67fcf0;
}

// [FUN_5d3f80]
int __cdecl Video_Get67fcf4()
{
    return g_67fcf4;
}

// [FUN_5d3f90]
int __cdecl Video_SetMode(int w, int h, int bits, int unused, int a, int b)
{
    g_width = w;
    g_height = h;
    g_67fcf0 = a;
    g_bitDepth = bits;
    g_67fcf4 = b;
    return 0;
}

}
