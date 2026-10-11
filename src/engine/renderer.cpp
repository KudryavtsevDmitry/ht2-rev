// renderer.cpp - small functions of the unit at 0x5ab230 of king.masm (with
// input.cpp and vecmath.cpp): the renderer's light stack and render states,
// object visibility checks, window rectangles and their hierarchy, pixel
// formats (565/555/32-bit), palettes and fog tables, textures with their
// mip levels and surfaces, and bitmaps.
//
// Building with  nmake ASM_RENDERER=1  makes king.masm assemble the original
// procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

extern "C" DWORD __fastcall Shade_Assign(DWORD* self, void*, const DWORD* s);   // media.cpp
extern "C" int __cdecl Video_BitDepth();                                       // video_mode.cpp
extern "C" void __cdecl Com_Release(BYTE* com);                                // directdraw.cpp

extern "C" void FUN_50bd10(); KING_ALIAS("_FUN_50bd10", "_Carat_ReleaseLight")    // pops a light (renderer)
ASM_PROC(FUN_5b6e00)
ASM_PROC(FUN_5b22f0)
ASM_PROC(FUN_5d3b40)
ASM_PROC(FUN_5d4490)
ASM_PROC(FUN_5b6bf0)
ASM_VAR(int, g_transformSerial, $L_67ef88)
ASM_VAR(int, g_720904, $L_720904)
ASM_PROC(FUN_5b3a40)
ASM_PROC(FUN_5b42c0)
ASM_PROC(FUN_6037e0)
ASM_PROC(FUN_5ad780)
ASM_PROC(FUN_5ad970)
ASM_VAR(AsmLabel, g_dtor5baa30, $L_5baa30)
ASM_VAR(double, g_500ms, $L_64b860)
ASM_VAR(float, g_255f, $L_64c458)
ASM_VAR(float, g_2pow24, $L_651768)
ASM_VAR(float, g_fogA, $L_67f0e0)
ASM_VAR(float, g_fogB, $L_67f0e4)
ASM_VAR(double, g_eps001, $L_64b658)
ASM_VAR(double, g_one64b670, $L_64b670)
ASM_PROC(FUN_5b2950)
ASM_PROC(FUN_5b1020)
ASM_PROC(FUN_5b0810)
extern "C" void FUN_5ae8a0(); KING_ALIAS("_FUN_5ae8a0", "@Clip_Pop@8")
ASM_PROC(FUN_5feace)    // fills words
ASM_PROC(FUN_5b6770)
ASM_PROC(FUN_5e48f0)
ASM_PROC(FUN_5b81c0)    // blits a bitmap (dst, bitmap, rect, x, y)
ASM_PROC(FUN_5d3d30)    // sets palette entries
ASM_PROC(FUN_5baa70)
ASM_PROC(FUN_4cef50)    // checksum (data, size)

ASM_VAR(BYTE*, g_device, $L_720908)            // IDirect3DDevice
ASM_VAR(int, g_702640, $L_702640)
ASM_VAR(int, g_702644, $L_702644)
ASM_VAR(int, g_702648, $L_702648)
ASM_VAR(int, g_702650, $L_702650)
ASM_VAR(int, g_702654, $L_702654)
ASM_VAR(int, g_702658, $L_702658)
ASM_VAR(int, g_72006c, $L_72006c)
ASM_VAR(int, g_720070, $L_720070)
ASM_VAR(int, g_720074, $L_720074)
ASM_VAR(int, g_720078, $L_720078)
ASM_VAR(int, g_72007c, $L_72007c)
ASM_VAR(int, g_702e6c, $L_702e6c)
ASM_VAR(int, g_702e74, $L_702e74)
ASM_VAR(int, g_702e78, $L_702e78)
ASM_VAR(int, g_702e7c, $L_702e7c)
ASM_VAR(int, g_702e88, $L_702e88)               // fog: near (z >> 8)
ASM_VAR(DWORD, g_702e8c, $L_702e8c)
ASM_VAR(int, g_pixelFormat, $L_702e94)          // 2: 565, 3: 555, 6: 32-bit
ASM_VAR(int, g_7208d4, $L_7208d4)
ASM_VAR(int, g_fogNear, $L_67f0e8)
ASM_VAR(int, g_fogFar, $L_67f0ec)
ASM_VAR(AsmLabel, g_vtbl65176c, $L_65176c)

template <class... A>
static __forceinline int Com(BYTE* com, unsigned off, A... args)
{
    return (*(int(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

static char* StrDup(const char* s)
{
    size_t n = strlen(s) + 1;
    char* d = (char*)operator new(n);
    memcpy(d, s, n);
    return d;
}

extern "C" {

// [FUN_5ad140]
int __fastcall Render_Level(BYTE* self, void*)
{
    return (F(int, self, 1960) >> 19) + 1;
}

// [FUN_5ad150] pushes a shade (at most 20)
void __fastcall Render_PushShade(BYTE* self, void*, const DWORD* s)
{
    int n = F(int, self, 8244);
    if (n >= 20)
        return;
    F(int, self, 8244) = n + 1;
    Shade_Assign((DWORD*)(self + (n + 494) * 16), 0, s);
}

// [FUN_5ad4c0]
BYTE* __fastcall Light_Init(BYTE* self, void*)
{
    for (int k = 4; k <= 24; k += 4)
        F(int, self, k) = 0;
    F(int, self, 4) = g_702650;
    F(int, self, 8) = g_702654;
    F(int, self, 12) = g_702658;
    F(int, self, 16) = g_702640;
    F(int, self, 20) = g_702644;
    F(int, self, 24) = g_702648;
    F(int, self, 0) = 0;
    F(DWORD, self, 44) = 1078530010;     // pi
    F(int, self, 48) = 0;
    F(float, self, 28) = 1.0f;
    F(int, self, 36) = 0;
    F(int, self, 32) = 0;
    F(float, self, 60) = 1.0f;
    F(float, self, 56) = 1.0f;
    F(float, self, 52) = 1.0f;
    F(float, self, 40) = 1.0f;
    F(int, self, 72) = 70;
    F(int, self, 76) = 5;
    F(float, self, 64) = 1.0f;
    F(int, self, 68) = -1;
    return self;
}

// [FUN_5ad550] copies a light (without +40)
DWORD __fastcall Light_Copy(DWORD* self, void*, const DWORD* s)
{
    static const int k[] = {0, 11, 12, 7, 8, 9, 13, 14, 15, 1, 2, 3, 4, 5, 6};
    for (int i = 0; i < 15; i++)
        self[k[i]] = s[k[i]];
    return s[6];
}

// [FUN_5adc00]
void __fastcall Render_PopShade(BYTE* self, void*)
{
    if (F(int, self, 8244) > 0)
        F(int, self, 8244) = F(int, self, 8244) - 1;
}

// [FUN_5adc20]
void __fastcall Render_PopLight(BYTE* self, void*, int unused)
{
    if (F(int, self, 8240) <= 0)
        return;
    F(int, self, 8240) = F(int, self, 8240) - 1;
    CallC(FUN_50bd10, self);
    int n = F(int, self, 8240);
    if (n > 0)
        Light_Copy((DWORD*)(self + 7824), 0, (DWORD*)(self + 6144 + n * 80));
}

// [FUN_5adc70]
void __fastcall Render_Set8460(BYTE* self, void*, int v)
{
    F(int, self, 8460) = v;
}

// [FUN_5ae550]
BYTE* __fastcall Render_Material(BYTE* self, void*)
{
    return self + 4292;
}

// [FUN_5ae560]
BYTE* __fastcall Render_CurrentFrame(BYTE* self, void*)
{
    return self + 3236 + F(int, self, 8224) * 48;
}

// [FUN_5ae9b0] resets the frame's render state
void __fastcall Render_BeginFrame(BYTE* self, void*, int window)
{
    g_72007c = 0;
    g_72006c = 0;
    g_720070 = 0;
    g_720074 = 0;
    g_720078 = 0;
    F(int, self, 8432) = 0;
    F(int, self, 8424) = 0;
    F(int, self, 8428) = 0;
    F(int, self, 8436) = 0;
    g_702e74 = 0;
    g_702e78 = 0;
    g_702e7c = 0;
    for (int k = 0; k < 36; k++)
        F(int, self, 8280 + k * 4) = 0;
    F(int, self, 8240) = 0;
    F(int, self, 8632) = 0;
    g_702e6c = 0;
    if (window >= 0)
        CallThis(FUN_5b6e00, F(BYTE*, self, 1948), window, 0);
    Com(g_device, 148, 0, 12, 1);
}

// [FUN_5aea50]
void __fastcall Render_EndFrame(BYTE* self, void*)
{
    F(int, self, 8428) = g_702e74;
    F(int, self, 8432) = g_702e78;
    F(int, self, 8436) = g_702e7c;
    int m = F(int, self, 8620);
    if ((m == 1 || m == 2) && F(int, self, 8240) > 0) {
        do {
            CallC(FUN_50bd10, self);
            F(int, self, 8240) = F(int, self, 8240) - 1;
        } while (F(int, self, 8240) > 0);
    }
    for (int i = 0; i < 5; i++)
        Com(g_device, 140, i, 0);
}

// [FUN_5af350]
void __fastcall Render_Object(BYTE* self, void*, BYTE* o)
{
    if (!F(int, self, 8252) || !F(int, self, 1948))
        return;
    BYTE* e = F(BYTE*, self, 8260) + F(short, o, 48) * 124;
    if (F(BYTE, e, 32) & 1) {
        short w = F(short, o, 50);
        if (w < 0 || F(int, self, 8456) < w) {
            CallThis(FUN_5b22f0, self, o);
            return;
        }
    }
    CallThis(FUN_5b2950, self, o);
}

// [FUN_5af3b0]
void __fastcall Render_Entry(BYTE* self, void*, int i, int a, int b)
{
    CallThis(FUN_5b1020, self, F(BYTE*, self, 8260) + i * 124, a, b, 0);
}

// [FUN_5b1740] the first visible entry of o's chain
BYTE* __fastcall Render_FirstVisible(BYTE* self, void*, BYTE* o)
{
    if (g_7208d4)
        return 0;
    if (g_702e8c & 0x2000000)
        return 0;
    for (int i = F(int, o, 44); i >= 0;) {
        BYTE* e = F(BYTE*, self, 8260) + i * 124;
        DWORD f = F(DWORD, e, 32);
        if (f & 0x4000)
            break;
        if (!(f & 0x80000)) {
            CallThis(FUN_5b0810, self, e, 0, 1);
            Com(g_device, 80, 27, 1);
            g_702e6c = 0;
            return e;
        }
        i = F(int, e, 44);
    }
    Com(g_device, 80, 27, 0);
    return 0;
}

// [FUN_5b56e0]
DWORD* __fastcall Rect_Set(DWORD* self, void*, DWORD a, DWORD b, DWORD c, DWORD d)
{
    self[0] = a;
    self[1] = b;
    self[2] = c;
    self[3] = d;
    return self;
}

// [FUN_5b6120]
void __fastcall Render_PopState(BYTE* self, void*)
{
    CallThis(FUN_5ae8a0, self);
    F(int, self, 8228) = F(int, self, 8228) - 1;
    int i = F(int, self, 8232) - 1;
    F(int, self, 8232) = i;
    F(int, self, 1952) = F(int, self, 1964 + i * 4);
    F(int, self, 1956) = F(int, self, 2044 + i * 4);
    F(int, self, 1960) = F(int, self, 2124 + i * 4);
    F(int, self, 1964) = F(int, self, 2204 + i * 4);
}

// [FUN_5b6180]
void __cdecl Device_BeginScene()
{
    if (g_device)
        Com(g_device, 20);
}

// [FUN_5b61f0]
void __cdecl Words_Fill(void* d, int v, int n)
{
    CallC(FUN_5feace, d, v, n);
}

// [FUN_5b6210] packs r, g, b into the current 16-bit format
DWORD __cdecl Pixel_Pack(int r, int g, int b)
{
    switch (g_pixelFormat) {
    case 2:
        return ((((r & 255) & 248) << 5 | ((g & 255) & ~3)) << 3) | ((b & 255) >> 3);
    case 3:
        return ((((r & 255) & 248) << 5 | ((g & 255) & ~7)) << 2) | ((b & 255) >> 3);
    }
    return 0;
}

// [FUN_5b6280]
int __cdecl Pixel_Red(DWORD c)
{
    switch (g_pixelFormat) {
    case 2:
        return (c >> 8) & 248;
    case 3:
        return (c >> 7) & 248;
    case 6:
        return (c >> 16) & 255;
    }
    return 0;
}

// [FUN_5b62c0]
int __cdecl Pixel_Green(DWORD c)
{
    switch (g_pixelFormat) {
    case 2:
        return (c >> 3) & 252;
    case 3:
        return (c >> 2) & 248;
    case 6:
        return (c >> 8) & 255;
    }
    return 0;
}

// [FUN_5b6300]
int __cdecl Pixel_Blue(DWORD c)
{
    int f = g_pixelFormat;
    if (f == 2 || f == 3)
        return (c & 31) << 3;
    if (f == 6)
        return c & 255;
    return 0;
}

// [FUN_5b6330]
int __fastcall Rect_Contains(int* self, void*, int x, int y)
{
    if (x < self[0] || x > self[2] || y < self[1])
        return 0;
    return y <= self[3];
}

// [FUN_5b6370]
void __fastcall Window_CopyParams(BYTE* self, void*, const BYTE* s)
{
    F(int, self, 32) = F(int, s, 32);
    F(int, self, 40) = F(int, s, 40);
    strcpy((char*)self, (const char*)s);
    F(int, self, 88) = F(int, s, 88);
    F(int, self, 76) = F(int, s, 76);
    F(int, self, 80) = F(int, s, 80);
    F(int, self, 84) = F(int, s, 84);
    F(int, self, 108) = F(int, s, 108);
    F(int, self, 56) = F(int, s, 56);
    F(int, self, 60) = F(int, s, 60);
}

// [FUN_5b64c0] the surface of the top window
int __fastcall Window_RootSurface(BYTE* self, void*)
{
    while (F(BYTE*, self, 96))
        self = F(BYTE*, self, 96);
    return F(int, self, 84);
}

// [FUN_5b64e0] sets the surface of the window and its parents
int __fastcall Window_SetSurface(BYTE* self, void*, int s)
{
    int old = F(int, self, 84);
    F(int, self, 84) = s;
    if (F(BYTE*, self, 96))
        Window_SetSurface(F(BYTE*, self, 96), 0, s);
    return old;
}

// [FUN_5b6500] to the coordinates of the top window
void __fastcall Window_ToRoot(BYTE* self, void*, short* x, short* y)
{
    if (!F(BYTE*, self, 96))
        return;
    Window_ToRoot(F(BYTE*, self, 96), 0, x, y);
    *x -= F(short, self, 100);
    *y -= F(short, self, 104);
}

// [FUN_5b6710]
DWORD* __fastcall Window_GetRect(BYTE* self, void*, DWORD* r)
{
    if (!F(BYTE*, self, 96)) {
        r[0] = 0;
        r[2] = F(WORD, self, 0);
        r[1] = 0;
        r[3] = F(WORD, self, 2);
        return r;
    }
    r[0] = F(DWORD, self, 100);
    r[1] = F(DWORD, self, 104);
    r[2] = F(DWORD, self, 108);
    r[3] = F(DWORD, self, 112);
    return r;
}

// [FUN_5b6a50]
BYTE* __fastcall Window_ConstructChild(BYTE* self, void*, int a, int b, int c, int d)
{
    F(int, self, 112) = 0;
    F(int, self, 108) = 0;
    F(int, self, 104) = 0;
    F(int, self, 100) = 0;
    CallThis(FUN_5b6770, self, a, b, c, d);
    return self;
}

// [FUN_5b6a90]
void __fastcall Window_FreeBuffers(BYTE* self, void*)
{
    if ((F(DWORD, self, 12) & 0x1000) && g_device)
        return;
    for (DWORD i = 0; i < F(DWORD, self, 20); i++) {
        if (!(F(DWORD, self, 12) & 544))
            operator delete(F(void*, self, 24 + i * 4));
        if (F(void*, self, 56 + i * 4))
            CallC(FUN_5e48f0, F(void*, self, 56 + i * 4));
    }
    DWORD f = F(DWORD, self, 12);
    if ((f & 0x4000) && !(f & 544))
        operator delete(F(void*, self, 48));
}

// [FUN_5b6b00]
int __fastcall Window_Set80(BYTE* self, void*, int v)
{
    int old = F(int, self, 80);
    F(int, self, 80) = v;
    return old;
}

// [FUN_5b86c0] draws a whole bitmap at (x, y)
void __cdecl Bitmap_Draw(BYTE* dst, int x, int y, BYTE* bmp)
{
    if (!bmp)
        return;
    int r[4] = {0, 0, F(WORD, bmp, 20), F(WORD, bmp, 22)};
    CallC(FUN_5b81c0, dst, bmp, r, x, y);
}

// The fog and palette table: +0 rows, +8 fog rows, +16 shade rows,
// +24 palette (256 RGB), +796 count, +800 fog levels, +804 shades,
// +808 bytes per pixel, +812 fog color.

// [FUN_5b94b0]
int __fastcall Rows_Get(BYTE* self, void*, int i)
{
    int* p = F(int*, self, 0);
    if (!p)
        return 0;
    if (i >= F(int, self, 796))
        return 0;
    return p[i];
}

// [FUN_5b94e0]
BYTE* __fastcall Palette_Construct(BYTE* self, void*)
{
    F(int, self, 792) = 0;
    F(int, self, 796) = 0;
    F(int, self, 800) = 0;
    F(int, self, 804) = 0;
    F(int, self, 16) = 0;
    F(int, self, 0) = 0;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    F(int, self, 20) = 0;
    F(int, self, 808) = 1;
    return self;
}

// [FUN_5ba010] color c at shade level s
int __fastcall Palette_Shade(BYTE* self, void*, int c, int s)
{
    int row = F(int, self, 804) * (s & 255) / 256;
    BYTE* r = F(BYTE**, self, 16)[row];
    if (F(int, self, 808) == 1)
        return r[c];
    return ((WORD*)r)[c];
}

// [FUN_5ba070] color c fogged at depth z
int __fastcall Palette_Fog(BYTE* self, void*, int c, DWORD z)
{
    int t = z >> 8;
    if (t > g_fogFar)
        return c;
    if (t <= g_702e88)
        return g_fogNear;
    int row = (g_fogFar - t) * F(int, self, 800) / (g_fogFar - g_702e88);
    BYTE* r = F(BYTE**, self, 8)[row];
    if (F(int, self, 808) == 1)
        return r[c];
    return ((WORD*)r)[c];
}

// [FUN_5ba3f0] palette entry c in the screen format
DWORD __fastcall Palette_Color(BYTE* self, void*, int c)
{
    c &= 255;
    const BYTE* p = self + 24 + c * 3;
    int m = F(int, self, 808);
    if (m == 1)
        return c;
    if (m == 2)
        return Pixel_Pack(p[0], p[1], p[2]);
    return (DWORD)p[0] << 16 | (DWORD)p[1] << 8 | p[2];
}

// [FUN_5ba170] the fog color for depth z
DWORD __fastcall Palette_FogColor(BYTE* self, void*, int z)
{
    int t = z >> 8;
    if (Video_BitDepth() != 4) {
        if (t >= g_fogFar)
            return F(DWORD*, self, 8)[0];
        if (t <= g_702e88)
            return F(DWORD*, self, 8)[F(int, self, 800) - 1];
        return F(DWORD*, self, 8)[(g_fogFar - t) * F(int, self, 800) / (g_fogFar - g_702e88)];
    }
    int a;
    if (t >= g_fogFar)
        a = 0;
    else if (t <= g_702e88)
        a = 255;
    else
        a = (g_fogFar - t) * 255 / (g_fogFar - g_702e88);
    DWORD c = Palette_Color(self, 0, F(BYTE, self, 812));
    return c | (DWORD)(-1 - a) << 24;
}

// [FUN_5ba220]
DWORD __fastcall Palette_ShadeRow(BYTE* self, void*, int s)
{
    return F(DWORD*, self, 16)[F(int, self, 804) * s / 256];
}

// [FUN_5ba240] the 16 standard colors
void __fastcall Palette_Defaults(BYTE* self, void*)
{
    static const BYTE c[24] = {0, 0, 0, 64, 64, 64, 128, 128, 128, 172, 172, 172,
                               255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255};
    for (int k = 0; k < 24; k++)
        self[24 + k] = c[k];
}

// [FUN_5ba2a0]
void __fastcall Palette_Gray(BYTE* self, void*)
{
    for (int i = 0; i < 256; i++) {
        self[24 + i * 3] = (BYTE)i;
        self[25 + i * 3] = (BYTE)i;
        self[26 + i * 3] = (BYTE)i;
    }
}

// [FUN_5ba2c0]
void __fastcall Palette_Copy(BYTE* self, void*, const BYTE* s)
{
    for (int k = 0; k < 768; k++)
        self[24 + k] = s[24 + k];
}

// [FUN_5ba3d0]
void __fastcall Palette_Apply(BYTE* self, void*)
{
    CallC(FUN_5d3d30, self + 24, 0, 256);
}

// [FUN_5ba490]
int __fastcall Palette_Set8(BYTE* self, void*, int v)
{
    int old = F(int, self, 8);
    F(int, self, 8) = v;
    return old;
}

// A texture: +4 name, +8 pixels, +12 mip levels, +20/+22 width/height,
// +24 bytes per pixel, +26 mip count, +28 checksum, +44 surface,
// +52 flags, +60 references, +72/+76 COM objects.

// [FUN_5ba5c0] width of mip level n
int __fastcall Texture_MipWidth(BYTE* self, void*, int n)
{
    if (!F(WORD, self, 26))
        return 0;
    int w = F(WORD, self, 20);
    for (; n > 0; n--)
        w >>= 1;
    return w;
}

// [FUN_5ba5f0] pixels of mip level n
BYTE* __fastcall Texture_MipPixels(BYTE* self, void*, int n)
{
    int levels = F(WORD, self, 26);
    if (!levels || n > levels)
        return 0;
    if (!n)
        return F(BYTE*, self, 8);
    int size = (int)F(WORD, self, 24) * F(WORD, self, 22) * F(WORD, self, 20) / 4;
    BYTE* p = F(BYTE*, self, 12);
    for (int i = n - 1; i > 0; i--) {
        p += size;
        size /= 4;
    }
    return p;
}

static void Texture_Clear(BYTE* self)
{
    F(void*, self, 0) = g_vtbl65176c;
    F(int, self, 60) = 0;
    F(WORD, self, 20) = 0;
    F(WORD, self, 22) = 0;
    F(WORD, self, 24) = 0;
    F(int, self, 56) = 0;
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
}

// [FUN_5ba660]
BYTE* __fastcall Texture_ConstructNamed(BYTE* self, void*, const char* name, int a, int b)
{
    Texture_Clear(self);
    if (name) {
        size_t n = strlen(name) + 1;
        char* d = (char*)operator new(n);
        F(char*, self, 4) = d;
        if (d)
            memcpy(d, name, n);
    } else {
        F(char*, self, 4) = 0;
    }
    F(int, self, 40) = 0;
    F(int, self, 28) = 0;
    F(int, self, 52) = 0;
    F(int, self, 44) = 0;
    F(WORD, self, 26) = 0;
    F(int, self, 72) = 0;
    F(int, self, 76) = 0;
    F(int, self, 48) = a;
    F(int, self, 32) = b;
    F(int, self, 68) = 1;
    return self;
}

// [FUN_5ba710]
BYTE* __fastcall Texture_Construct(BYTE* self, void*, const char* name)
{
    Texture_Clear(self);
    F(char*, self, 4) = name ? StrDup(name) : 0;
    F(int, self, 48) = 0;
    F(int, self, 32) = 0;
    F(int, self, 40) = 0;
    F(int, self, 28) = 0;
    F(int, self, 52) = 0;
    F(int, self, 44) = 0;
    F(int, self, 72) = 0;
    F(int, self, 76) = 0;
    F(int, self, 68) = 1;
    return self;
}

// [FUN_5ba7b0] the pixel at (x, y), wrapped (y for other formats)
int __fastcall Texture_Pixel(BYTE* self, void*, int x, int y)
{
    int w = F(WORD, self, 20), h = F(WORD, self, 22);
    int i = (y % h) * w + x % w;
    switch (F(WORD, self, 24)) {
    case 1:
        return F(BYTE*, self, 8)[i];
    case 2:
        return F(WORD*, self, 8)[i];
    }
    return y;
}

// [FUN_5bac90] releases the texture's surface
void __fastcall Texture_ReleaseSurface(BYTE* self, void*)
{
    if (!(F(BYTE, self, 52) & 2))
        return;
    if (F(BYTE*, self, 44)) {
        Com_Release(F(BYTE*, self, 44));
        F(int, self, 72) = 0;
        F(DWORD, self, 52) &= ~2u;
        return;
    }
    BYTE* t = F(BYTE*, self, 72);
    if (t) {
        if (F(BYTE*, self, 76)) {
            Com(F(BYTE*, self, 76), 8);
            F(int, self, 76) = 0;
            CallC(FUN_5baa70, F(BYTE*, self, 72));
        } else {
            Com(t, 8);
        }
        F(int, self, 72) = 0;
        F(int, self, 44) = 0;
    }
    F(DWORD, self, 52) &= ~2u;
}

// [FUN_5bad10] drops a reference
void __fastcall Texture_Release(BYTE* self, void*)
{
    if (--F(int, self, 60) > 0)
        return;
    if (F(WORD, self, 24) == 1) {
        if (F(void*, self, 8)) {
            operator delete(F(void*, self, 8));
            F(void*, self, 8) = 0;
        }
    } else if (!(F(DWORD, self, 52) & 0x800)) {
        Texture_ReleaseSurface(self, 0);
    }
    F(DWORD, self, 52) &= ~1u;
}

// [FUN_5bad60] bytes of the pixels (with mip levels)
int __fastcall Texture_Size(BYTE* self, void*)
{
    int n = (int)F(WORD, self, 24) * F(WORD, self, 22) * F(WORD, self, 20);
    if (F(BYTE, self, 52) & 16)
        n += n / 2;
    return n;
}

// [FUN_5bad90]
int __fastcall Texture_SetPixels(BYTE* self, void*, BYTE* data, BYTE* mips, int owned)
{
    if (F(BYTE, self, 52) & 4) {
        F(BYTE*, self, 8) = data;
        return 1;
    }
    if (F(BYTE*, self, 8) && F(BYTE*, self, 8) != data)
        return 0;
    int n = (int)F(WORD, self, 20) * F(WORD, self, 24) * F(WORD, self, 22);
    F(BYTE*, self, 8) = data;
    F(int, self, 28) = CallC<int>(FUN_4cef50, data, n);
    DWORD f = F(DWORD, self, 52);
    if (f & 16)
        F(BYTE*, self, 12) = mips ? mips : data + (int)F(WORD, self, 20) * F(WORD, self, 24) * F(WORD, self, 22);
    f |= 4;
    F(DWORD, self, 52) = f;
    if (owned)
        F(DWORD, self, 52) = f | 1;
    return 1;
}

// A bitmap: +0 name, +4/+6/+8, +12 bytes, +16 pixels, +20, +24, +28 flags,
// +36 COM object.

// [FUN_5bbd20]
BYTE* __fastcall Bitmap_ConstructData(BYTE* self, void*, int a, int b, int size)
{
    F(int, self, 12) = size;
    F(int, self, 24) = a;
    F(WORD, self, 4) = 0;
    F(WORD, self, 6) = 0;
    F(WORD, self, 8) = 0;
    F(int, self, 32) = 0;
    F(int, self, 0) = 0;
    F(int, self, 16) = 0;
    F(int, self, 20) = b;
    F(int, self, 28) = 0;
    F(int, self, 36) = 0;
    return self;
}

// [FUN_5bbd60]
BYTE* __fastcall Bitmap_Construct(BYTE* self, void*, const char* name)
{
    F(WORD, self, 4) = 0;
    F(WORD, self, 6) = 0;
    F(WORD, self, 8) = 0;
    F(int, self, 32) = 0;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    F(char*, self, 0) = StrDup(name);
    F(int, self, 24) = 0;
    F(int, self, 20) = 0;
    F(int, self, 28) = 0;
    F(int, self, 36) = 0;
    return self;
}

// [FUN_5bbdd0]
void __fastcall Bitmap_Destroy(BYTE* self, void*)
{
    if (F(void*, self, 16) && !(F(BYTE, self, 28) & 4)) {
        operator delete(F(void*, self, 16));
        F(void*, self, 16) = 0;
    }
    operator delete(F(void*, self, 0));
    BYTE* o = F(BYTE*, self, 36);
    if (o)
        Com(o, 8);
}

// [FUN_5bbe80]
void __fastcall Bitmap_Unload(BYTE* self, void*)
{
    DWORD f = F(DWORD, self, 28);
    if (!(f & 4))
        return;
    F(DWORD, self, 28) = f & ~0x21u;
    if (F(WORD, self, 8) == 1 && F(void*, self, 16)) {
        operator delete(F(void*, self, 16));
        F(void*, self, 16) = 0;
    }
}

// [FUN_5bbeb0]
int __fastcall Bitmap_SetPixels(BYTE* self, void*, BYTE* data, int owned)
{
    if (F(BYTE*, self, 16) && F(BYTE*, self, 16) != data)
        return 0;
    F(BYTE*, self, 16) = data;
    F(DWORD, self, 28) |= 4;
    if (owned)
        F(DWORD, self, 28) |= 1;
    return 1;
}

// [FUN_5bbef0]
int __fastcall Bitmap_Bytes(BYTE* self, void*)
{
    return F(int, self, 12);
}

}

// --- lights, fog and frame timing

typedef void (*RFn)();

static double Lights_Sum(BYTE* self, void (*fn)(), int a, int b, bool two)
{
    double sum = 0.0;
    BYTE* l = self + 6224 + (F(int, self, 8240) - 1) * 80;
    for (int n = F(int, self, 8240); n > 0; n--, l -= 80)
        sum = (two ? CallThis<double>(fn, l, a, b, 0, 0) : CallThis<double>(fn, l, a, 0, 0)) + sum;
    return sum;
}

extern "C" {

// [FUN_5b3a00] draws with the 5-argument path when flag 32 is set
int __stdcall Draw_Dispatch(int a, DWORD flags, int b, int c, int d)
{
    if (flags & 32)
        return CallStd<int>(FUN_5b3a40, a, flags, b, c, d);
    return CallStd<int>(FUN_5b42c0, a, flags, b);
}

// [FUN_5bac70] destroys the six 188-byte records
void __fastcall Records188x6_Destroy(BYTE* self, void*)
{
    for (int i = 5; i >= 0; i--)
        CallThis((RFn)(void*)g_dtor5baa30, self + i * 188);
}

// [FUN_5b6190] waits up to 500 ms while the device is busy
void __cdecl Device_WaitIdle()
{
    BYTE* d = g_device;
    if (!d)
        return;
    double t = CallC<double>(FUN_6037e0);
    d = g_device;
    if (!CallStd<int>(F(RFn, F(BYTE*, d, 0), 24), d))
        return;
    double deadline = t + g_500ms;
    while (!(CallC<double>(FUN_6037e0) >= deadline)) {
        d = g_device;
        if (!CallStd<int>(F(RFn, F(BYTE*, d, 0), 24), d))
            break;
    }
}

// [FUN_5b1870] the alpha bytes of n vertices: dot(v, m column 0) + m[10], times 255
void __cdecl Vertices_Alpha(DWORD* dst, const BYTE* src, unsigned stride, int n, const float* m)
{
    stride &= ~3u;
    for (; n > 0; n--, src += stride, dst++) {
        const float* v = (const float*)src;
        double a = (double)v[0] * m[1] + (double)v[2] * m[7] + (double)m[4] * v[1] + m[10];
        *dst = (DWORD)(int)(a * g_255f) << 24;
    }
}

// [FUN_5ba120] the fog range: near a, far b, mode c
void __cdecl Fog_SetRange(float a, float b, int c)
{
    g_fogNear = c;
    g_fogA = a;
    g_fogB = b;
    int na = (int)((double)g_2pow24 / a);
    g_fogFar = na >> 8;
    int nb = (int)((double)g_2pow24 / b);
    g_702e88 = nb >> 8;
}

// [FUN_5b4f70] whether the point p is in front of the current clip plane
int __fastcall Clip_InFront(BYTE* self, void*, const float* p)
{
    const float* pl = (const float*)(self + 3236 + F(int, self, 8224) * 48);
    double d = (double)pl[2] * p[2] + (double)pl[1] * p[1] + (double)pl[0] * p[0] + p[3];
    return d > g_eps001 ? 1 : 0;
}

// [FUN_5adaf0] the light at (a, b): t + (1 - t) * the sum of the lights, at most 1
double __fastcall Lights_At(BYTE* self, void*, int a, int b, double t)
{
    double sum = Lights_Sum(self, FUN_5ad780, a, b, true);
    double r = (g_one64b670 - t) * sum + t;
    if (r > g_one64b670)
        r = g_one64b670;
    return r;
}

// [FUN_5adb80] the same for one point
double __fastcall Lights_AtPoint(BYTE* self, void*, int a, double t)
{
    double sum = Lights_Sum(self, FUN_5ad970, a, 0, false);
    double r = (g_one64b670 - t) * sum + t;
    if (r > g_one64b670)
        r = g_one64b670;
    return r;
}

}

static int AbsInt(int x)
{
    return x < 0 ? -x : x;
}

extern "C" {

// [FUN_5b9660] the palette index nearest to (r, g, b) (sum of the differences)
BYTE __fastcall Palette_Nearest(BYTE* self, void*, int r, int g, int b)
{
    r &= 255;
    g &= 255;
    b &= 255;
    const BYTE* e = self + 24;
    int best = AbsInt(b - e[2]) + AbsInt(g - e[1]) + AbsInt(r - e[0]);
    int idx = 0;
    for (int i = 1; i < 256; i++) {
        e += 3;
        int d = AbsInt(b - e[2]) + AbsInt(r - e[0]) + AbsInt(g - e[1]);
        if (d < best) {
            best = d;
            idx = i;
        }
    }
    return (BYTE)idx;
}

// [FUN_5b9740] frees the three texture tables (+0/+4, +8/+12, +16/+20)
void __fastcall Textures_FreeAll(BYTE* self, void*)
{
    for (int t = 0; t < 3; t++) {
        void**& ptrs = F(void**, self, t * 8);
        WORD*& ids = F(WORD*, self, t * 8 + 4);
        for (int i = 0; i < F(int, self, 796 + t * 4); i++) {
            operator delete(ptrs[i]);
            CallC(FUN_5e48f0, (int)ids[i]);
        }
        operator delete(ptrs);
        operator delete(ids);
        ptrs = 0;
        ids = 0;
    }
    F(int, self, 792) = 0;
    F(int, self, 796) = 0;
    F(int, self, 800) = 0;
    F(int, self, 804) = 0;
}

// [FUN_5b6b10] presents the surface: flips the triple/double buffer or blits
void __fastcall Surface_Present(BYTE* self, void*)
{
    DWORD f = F(DWORD, self, 12);
    if (f & 32)
        return;
    if (!(f & 0x200)) {
        CallC(FUN_5d3b40, F(int, self, 24));
        return;
    }
    if (!(f & 1)) {
        CallC(FUN_5d4490, 0);
        CallThis(FUN_5b6bf0, self);
        return;
    }
    unsigned size = (unsigned)F(WORD, self, 4) * F(WORD, self, 2) * F(WORD, self, 0);
    if (!(f & 0x100)) {
        CallC(FUN_5d4490, size * F(int, self, 16));
        F(unsigned, self, 16) = (F(unsigned, self, 16) + 1) % 3;
        return;
    }
    if (F(int, self, 120)) {
        CallC(FUN_5d4490, size * F(int, self, 16));
        F(unsigned, self, 16) = (F(unsigned, self, 16) + 4) % 3;
    } else {
        F(unsigned, self, 16) = (F(unsigned, self, 16) + 3) % 6;
    }
    F(int, self, 120) ^= 1;
}

// [FUN_5b63e0] a software surface copied from the description d
BYTE* __fastcall Surface_Construct(BYTE* self, void*, BYTE* d)
{
    for (int k = 96; k <= 112; k += 4)
        F(int, self, k) = 0;
    F(WORD, self, 4) = F(WORD, d, 24);
    F(WORD, self, 0) = F(WORD, d, 20);
    F(WORD, self, 2) = F(WORD, d, 22);
    unsigned size = (unsigned)F(WORD, self, 4) * F(WORD, self, 2) * F(WORD, self, 0);
    BYTE* buf = (BYTE*)operator new(size);
    F(BYTE*, self, 24) = buf;
    if (F(BYTE*, d, 8))
        memcpy(buf, F(BYTE*, d, 8), (unsigned)F(WORD, self, 4) * F(WORD, self, 0) * F(WORD, self, 2));
    F(DWORD, d, 52) = (F(DWORD, d, 52) & ~0x600u) | 4;
    Palette_Set8(d, 0, (int)buf);
    F(int, self, 80) = 0;
    F(int, self, 84) = 0;
    F(int, self, 56) = 0;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    F(int, self, 120) = 0;
    F(WORD, self, 8) = 0;
    F(int, self, 48) = 0;
    F(int, self, 116) = 0;
    F(float, self, 88) = 0.5f;
    F(BYTE, self, 92) = 0;
    F(int, self, 20) = 1;
    F(WORD, self, 6) = (WORD)(F(WORD, self, 4) * F(WORD, self, 0));
    return self;
}

// [FUN_5ae8a0] pops the clip plane and the transform; sets the world matrix
void __fastcall Clip_Pop(BYTE* self, void*)
{
    if (F(int, self, 8224) <= 1 || F(int, self, 8236) <= 1)
        return;
    F(int, self, 8224)--;
    F(int, self, 8236)--;
    F(int, self, 8444) = ++g_transformSerial;
    if (!g_720904)
        return;
    const float* m = (const float*)(self + 5216 + F(int, self, 8236) * 48);
    float w[16];
    for (int r = 0; r < 4; r++) {
        w[r * 4 + 0] = m[r * 3 + 0];
        w[r * 4 + 1] = m[r * 3 + 1];
        w[r * 4 + 2] = m[r * 3 + 2];
        w[r * 4 + 3] = 0.0f;
    }
    w[15] = 1.0f;
    BYTE* dev = g_device;
    CallStd(F(RFn, F(BYTE*, dev, 0), 44), dev, 1, w);
}

}
