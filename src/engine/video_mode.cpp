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

// ---------------------------------------------------------------------------
typedef char VmLabel[1];
extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")    // DirectDraw error text (hr, where)
ASM_VAR(int, g_vmWindowed, $L_7208f0)
ASM_VAR(BYTE*, g_vmFront, $L_7208f8)
ASM_VAR(int, g_vm7208b8, $L_7208b8)
ASM_VAR(VmLabel, s_vm67fdd8, $L_67fdd8)
ASM_VAR(VmLabel, s_vm67fdc0, $L_67fdc0)
ASM_VAR(VmLabel, s_vm67fdb8, $L_67fdb8)
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

template <class R = long, class... A>
static R Vm_ComCall(void* p, int off, A... a)
{
    return ((R(__stdcall*)(void*, A...))(*(void***)p)[off / 4])(p, a...);
}

// restores a lost surface; fatal with the text when that fails
static void Vm_RestoreLost(BYTE* s, const char* what)
{
    long hr = Vm_ComCall(s, 108);
    if (hr)
    {
        char text[256];
        strcpy(text, what);
        strcat(text, CallC<const char*>(FUN_5d4c10, hr, 0));
        Sys_Fatal(text, 0);
    }
}

// [FUN_5d3fe0..$L_5d41c5] locks the primary surface (+7208fc) once, restoring lost surfaces; returns its memory (pitch in 7208a4), 0 when not in a fullscreen DirectDraw mode
extern "C" BYTE* __cdecl Dd_LockPrimary()
{
    if (!g_ddMode || g_vmWindowed)
        return 0;
    if (Vm_ComCall(g_vmFront, 96) == (long)0x887601C2)
        Vm_RestoreLost(g_vmFront, s_vm67fdd8);
    if (Vm_ComCall(g_directDraw, 96) == (long)0x887601C2)
    {
        g_waitVsync = 0;
        Vm_RestoreLost(g_directDraw, s_vm67fdc0);
    }
    if (g_waitVsync)
        return (BYTE*)g_waitVsync;
    if (g_vm7208b8)
        return 0;
    DWORD desc[31];
    desc[0] = 124;
    desc[1] = 8387054;
    if (!Vm_ComCall(g_directDraw, 100, (void*)0, desc, 1, 0))
    {
        g_waitVsync = desc[9];
        g_7208a4 = desc[4];
        return (BYTE*)desc[9];
    }
    Sys_Fatal(CallC<const char*>(FUN_5d4c10, 0, (char*)s_vm67fdb8), 0);
    return (BYTE*)g_waitVsync;
}

// ---------------------------------------------------------------------------
extern "C" void FUN_5d4450(); KING_ALIAS("_FUN_5d4450", "_Dd_WaitFlip")    // waits for the flip
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")    // log (text, value)
ASM_VAR(int, g_vm7208b4, $L_7208b4)
ASM_VAR(int, g_vmFlipPending, $L_720928)
ASM_VAR(HWND, g_vmHwnd, $L_720920)
ASM_VAR(VmLabel, s_vm67fe60, $L_67fe60)
ASM_VAR(VmLabel, s_vm67fe4c, $L_67fe4c)
ASM_VAR(VmLabel, s_vm67fe3c, $L_67fe3c)
ASM_VAR(VmLabel, s_vm67fe28, $L_67fe28)
ASM_VAR(VmLabel, s_vm67fe1c, $L_67fe1c)
ASM_VAR(VmLabel, s_vm67fe10, $L_67fe10)
ASM_VAR(VmLabel, s_vm67fe04, $L_67fe04)
ASM_VAR(VmLabel, s_vm67fdf8, $L_67fdf8)

// [FUN_5d4490..$L_5d4623] presents the frame: unlocks the primary, restores lost surfaces, blits (mode 2 without flipping, or windowed) or flips, then locks the primary again for the next frame
extern "C" void __cdecl Dd_Present()
{
    if (!g_ddMode)
        return;
    if (g_waitVsync)
    {
        if (!Vm_ComCall(g_directDraw, 128, 0))
            g_waitVsync = 0;
        else if (g_waitVsync)
            return;
    }
    if (Vm_ComCall(g_vmFront, 96) == (long)0x887601C2)
    {
        CallC(FUN_5d2ba0, (char*)s_vm67fe60, 0);
        Vm_RestoreLost(g_vmFront, s_vm67fe4c);
    }
    if (Vm_ComCall(g_directDraw, 96) == (long)0x887601C2)
    {
        g_waitVsync = 0;
        CallC(FUN_5d2ba0, (char*)s_vm67fe3c, 0);
        Vm_RestoreLost(g_directDraw, s_vm67fe28);
    }
    DWORD fx[25] = {};
    fx[0] = 100;
    fx[2] = 13369376;
    if (g_ddMode == 2)
    {
        if (!g_vm7208b4)
        {
            RECT src = {0, 0, g_width, g_height};
            RECT dst = {0, 0, g_width, g_height};
            if (Vm_ComCall(g_vmFront, 96) == (long)0x887601C2)
                CallC(FUN_5d2ba0, (char*)s_vm67fe1c, 33);
            if (Vm_ComCall(g_directDraw, 96) == (long)0x887601C2)
                CallC(FUN_5d2ba0, (char*)s_vm67fe10, 33);
            long hr = Vm_ComCall(g_vmFront, 20, &dst, g_directDraw, &src, 16908288, fx);
            if (hr)
                CallC(FUN_5d4c10, hr, 0);     // its text was never shown
        }
        else
        {
            for (;;)
            {
                CallC(FUN_5d4450);
                long hr = Vm_ComCall(g_vmFront, 44, 0, 1);
                if (!hr)
                    break;
                CallC(FUN_5d4c10, hr, 0);
            }
            g_vmFlipPending = 1;
        }
    }
    else
    {
        RECT src = {0, 0, g_width, g_height};
        RECT rc;
        GetClientRect(g_vmHwnd, &rc);
        POINT a = {rc.left, rc.top};
        POINT b = {rc.right, rc.bottom};
        ClientToScreen(g_vmHwnd, &a);
        ClientToScreen(g_vmHwnd, &b);
        RECT dst = {a.x, a.y, b.x, b.y};
        Vm_ComCall(g_vmFront, 20, &dst, g_directDraw, &src, 131072, fx);
    }
    if (g_vmWindowed || g_vm7208b8)
        return;
    DWORD desc[31];
    desc[0] = 124;
    desc[1] = 8387054;
    if (!Vm_ComCall(g_directDraw, 100, (void*)0, desc, 1, 0))
        g_waitVsync = (int)desc[9];
}

// ---------------------------------------------------------------------------
ASM_VAR(BYTE, g_vm720120, $L_720120)
ASM_VAR(BYTE*, g_vm72071c, $L_72071c)

// [FUN_5d3b40] draws a frame buffer to the window through GDI (a 256-entry palette-index header), stretched by 67fcf0/67fcf4 unless both are 1
extern "C" int __cdecl Gdi_DrawFrame(void* bits)
{
    if (!(g_vm720120 & 1))
    {
        g_vm720120 |= 1;
        g_vm72071c = (BYTE*)operator new(2044);
    }
    BYTE* b = g_vm72071c;
    F(DWORD, b, 0) = 40;
    F(int, b, 4) = g_width;
    F(int, b, 8) = -g_height;
    F(WORD, b, 12) = 1;
    F(WORD, b, 14) = (WORD)(g_bitDepth * 8);
    F(DWORD, b, 16) = 0;
    F(DWORD, b, 20) = 0;
    F(DWORD, b, 24) = 0;
    F(DWORD, b, 28) = 0;
    F(DWORD, b, 32) = 256;
    F(DWORD, b, 36) = 256;
    for (WORD i = 0; i < 256; i++)
        F(WORD, b, 40 + i * 2) = i;
    UINT usage = g_bitDepth == 1;
    if (g_67fcf0 == 1 && g_67fcf4 == 1)
        return SetDIBitsToDevice(g_paletteDc, 0, 0, g_width, g_height, 0, 0, 0, g_height, bits, (BITMAPINFO*)b, usage);
    return StretchDIBits(g_paletteDc, 0, 0, g_67fcf0 * g_width, g_67fcf4 * g_height, 0, 0, g_width, g_height, bits, (BITMAPINFO*)b, usage, 13369376);
}


// ---------------------------------------------------------------------------
// Choosing the Direct3D device.

ASM_VAR(int, g_q155_72092c, $L_72092c)
ASM_VAR(int, g_q155_7208e8, $L_7208e8)
ASM_VAR(int, g_q155_7208dc, $L_7208dc)
ASM_VAR(int, g_q155_7208bc, $L_7208bc)
ASM_VAR(int, g_q155_7208d0, $L_7208d0)
ASM_VAR(int, g_q155_7208d4, $L_7208d4)
ASM_VAR(int, g_q155_7208c0, $L_7208c0)
ASM_VAR(int, g_q155_7208c4, $L_7208c4)
ASM_VAR(int, g_q155_7208c8, $L_7208c8)
ASM_VAR(int, g_q155_7208cc, $L_7208cc)
// [$L_5d4900] device enumeration callback: takes the 72092c-th hardware device that renders in ctx's bit depth
// (+16): its z-buffer depth (+28: the wanted 7208e8 one or the deepest), its GUID (+104, +100 pointing at it),
// the texture filter level (7208dc) and the multitexture and other capabilities (7208bc..7208cc); 1 goes on
extern "C" int __stdcall D3d_ChooseDevice(const char*, const char*, const BYTE* dd, BYTE* ctx)
{
    if (!(F(DWORD, dd, 0) & 0x80000) || !(F(DWORD, ctx, 16) & F(DWORD, dd, 116)))
        return 1;
    if (g_q155_72092c > 0) {
        g_q155_72092c--;
        return 1;
    }
    DWORD z = F(DWORD, dd, 120);
    if (g_q155_7208e8 > 0) {
        F(int, ctx, 28) = 0;
        if (g_q155_7208e8 == 16) {
            if (z & 0x400)
                F(int, ctx, 28) = 16;
        } else if (g_q155_7208e8 == 24) {
            if (z & 0x200)
                F(int, ctx, 28) = 24;
        } else if (g_q155_7208e8 == 32) {
            if (z & 0x100)
                F(int, ctx, 28) = 32;
        }
    } else
        F(int, ctx, 28) = (z & 0x100) ? 32 : (z & 0x200) ? 24 : (z & 0x400) ? 16 : (z & 0x800) ? 8 : 0;
    memcpy(ctx + 104, dd + 196, 16);
    F(BYTE*, ctx, 100) = ctx + 104;
    DWORD f = F(DWORD, dd, 0);
    if (f & 0x1000)
        g_q155_7208dc = 3;
    else if (f & 0x200)
        g_q155_7208dc = 2;
    else
        g_q155_7208dc = (f >> 8) & 1;
    DWORD t = F(DWORD, dd, 180);
    if (F(WORD, dd, 186) >= 2 && !(f & 0x4000)) {
        g_q155_7208bc = 1;
        g_q155_7208d0 = (t & 255) >> 3 & 1;
        g_q155_7208d4 = t >> 15 & 1;
        if (!g_q155_7208d0 && !g_q155_7208d4)
            g_q155_7208bc = 0;
    } else {
        g_q155_7208bc = 0;
        g_q155_7208d0 = 1;
        g_q155_7208d4 = 1;
    }
    g_q155_7208c0 = (F(DWORD, dd, 96) & 255) >> 5 & 1;
    g_q155_7208c4 = F(DWORD, dd, 180) >> 21 & 1;
    g_q155_7208c8 = (F(DWORD, dd, 180) & 255) >> 2 & 1;
    g_q155_7208cc = F(DWORD, dd, 180) >> 13 & 1;
    return 0;
}
