// renderer.cpp - small functions of the unit at 0x5ab230 of king.exe (with
// input.cpp and vecmath.cpp): the renderer's light stack and render states,
// object visibility checks, window rectangles and their hierarchy, pixel
// formats (565/555/32-bit), palettes and fog tables, textures with their
// mip levels and surfaces, and bitmaps.

#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

extern "C" DWORD __fastcall Shade_Assign(DWORD* self, void*, const DWORD* s);   // media.cpp
extern "C" int __cdecl Video_BitDepth();                                       // video_mode.cpp
extern "C" void __cdecl Com_Release(BYTE* com);                                // directdraw.cpp

extern "C" void FUN_50bd10(); KING_ALIAS("_FUN_50bd10", "_Carat_ReleaseLight")    // pops a light (renderer)
extern "C" void FUN_5b6e00(); KING_ALIAS("_FUN_5b6e00", "@Surface_Clear@16")
extern "C" void FUN_5b22f0(); KING_ALIAS("_FUN_5b22f0", "@Poly_Prepare@12")
extern "C" void FUN_5b6f90(); KING_ALIAS("_FUN_5b6f90", "_Surface_FillRect")
extern "C" void FUN_5d8350(); KING_ALIAS("_FUN_5d8350", "_Dd_BitDepth")
extern "C" void FUN_5d3b40(); KING_ALIAS("_FUN_5d3b40", "_Gdi_DrawFrame")
extern "C" void FUN_5d4490(); KING_ALIAS("_FUN_5d4490", "_Dd_Present")
extern "C" void FUN_5b6bf0(); KING_ALIAS("_FUN_5b6bf0", "@Surface_Prepare@4")
KING_VAR(int, g_transformSerial, 67ef88)
KING_VAR(int, g_720904, 720904)
extern "C" void FUN_5b3a40(); KING_ALIAS("_FUN_5b3a40", "@Render_TexPolygon@28")
extern "C" void FUN_5b42c0(); KING_ALIAS("_FUN_5b42c0", "@Render_Polygon@20")
extern "C" void FUN_6037e0(); KING_ALIAS("_FUN_6037e0", "_Tsc_Now")
extern "C" void FUN_5ad780(); KING_ALIAS("_FUN_5ad780", "@Light_StrengthAt@24")
extern "C" void FUN_5ad970(); KING_ALIAS("_FUN_5ad970", "@Light_FogAt@20")
extern "C" void __fastcall Holder5baa30_ReleaseAll(BYTE* s, void*);
KING_VAR(double, g_500ms, 64b860)
KING_VAR(float, g_255f, 64c458)
KING_VAR(float, g_2pow24, 651768)
KING_VAR(float, g_fogA, 67f0e0)
KING_VAR(float, g_fogB, 67f0e4)
KING_VAR(double, g_eps001, 64b658)
KING_VAR(double, g_one64b670, 64b670)
extern "C" void FUN_5b2950(); KING_ALIAS("_FUN_5b2950", "@Mesh_Draw@12")
extern "C" void FUN_5b1020(); KING_ALIAS("_FUN_5b1020", "@Material_Select@24")
extern "C" void FUN_5b0810(); KING_ALIAS("_FUN_5b0810", "@Material_SetStages@20")
extern "C" void FUN_5ae8a0(); KING_ALIAS("_FUN_5ae8a0", "@Clip_Pop@8")
ASM_PROC(FUN_5feace)    // fills words
extern "C" void FUN_5b6770(); KING_ALIAS("_FUN_5b6770", "@Surface_Create@24")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_5b81c0(); KING_ALIAS("_FUN_5b81c0", "_Image_DrawPart")    // blits a bitmap (dst, bitmap, rect, x, y)
extern "C" void FUN_5d3d30(); KING_ALIAS("_FUN_5d3d30", "_Video_SetPaletteRgb")    // sets palette entries
extern "C" void FUN_5baa70(); KING_ALIAS("_FUN_5baa70", "_TexPool_Return")
extern "C" void FUN_4cef50(); KING_ALIAS("_FUN_4cef50", "_Effect_Zero")    // checksum (data, size)

KING_VAR(BYTE*, g_device, 720908)            // IDirect3DDevice
KING_VAR(int, g_702640, 702640)
KING_VAR(int, g_702644, 702644)
KING_VAR(int, g_702648, 702648)
KING_VAR(int, g_702650, 702650)
KING_VAR(int, g_702654, 702654)
KING_VAR(int, g_702658, 702658)
KING_VAR(int, g_72006c, 72006c)
KING_VAR(int, g_720070, 720070)
KING_VAR(int, g_720074, 720074)
KING_VAR(int, g_720078, 720078)
KING_VAR(int, g_72007c, 72007c)
KING_VAR(int, g_702e6c, 702e6c)
KING_VAR(int, g_702e74, 702e74)
KING_VAR(int, g_702e78, 702e78)
KING_VAR(int, g_702e7c, 702e7c)
KING_VAR(int, g_702e88, 702e88)               // fog: near (z >> 8)
KING_VAR(DWORD, g_702e8c, 702e8c)
KING_VAR(int, g_pixelFormat, 702e94)          // 2: 565, 3: 555, 6: 32-bit
KING_VAR(int, g_7208d4, 7208d4)
KING_VAR(int, g_fogNear, 67f0e8)
KING_VAR(int, g_fogFar, 67f0ec)
KING_VAR(AsmLabel, g_vtbl65176c, 65176c)

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

extern "C" int __fastcall Render_TexPolygon(BYTE* r, void*, BYTE* t, DWORD flags, int b, float dist, int);

extern "C" {

// [FUN_5b3a00] draws with the 5-argument path when flag 32 is set
int __fastcall Draw_Dispatch(BYTE* r, void*, int a, DWORD flags, int b, int c, int d)
{
    if (flags & 32)
        return Render_TexPolygon(r, 0, (BYTE*)a, flags, b, *(float*)&c, d);
    CallThis(FUN_5b42c0, r, a, flags, b);
    return 0;
}

// [FUN_5bac70] destroys the six 188-byte records
void __fastcall Records188x6_Destroy(BYTE* self, void*)
{
    for (int i = 5; i >= 0; i--)
        Holder5baa30_ReleaseAll(self + i * 188, 0);
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

extern "C" {

// [FUN_5ba300] blends palette entries first..last toward (r, g, b) by alpha/255
void __fastcall Palette_Blend(BYTE* self, void*, int first, int last, int r, int g, int b, int alpha)
{
    if (first < 0)
        first = 0;
    else if (first > 255)
        return;
    if (last < 0)
        return;
    if (last > 255)
        last = 255;
    if (alpha <= 0 || alpha > 255 || first > last)
        return;
    int R = (r & 255) * alpha;
    int G = (g & 255) * alpha;
    int B = (b & 255) * alpha;
    int inv = 255 - alpha;
    BYTE* p = self + 24 + first * 3;
    for (int n = last - first + 1; n; n--, p += 3) {
        p[0] = (BYTE)((p[0] * inv + R) >> 8);
        p[1] = (BYTE)((p[1] * inv + G) >> 8);
        p[2] = (BYTE)((p[2] * inv + B) >> 8);
    }
}

// [FUN_5b6e00] clears the surface to a colour (a palette index when !argb)
void __fastcall Surface_Clear(BYTE* self, void*, int color, int argb)
{
    if (!argb && color < 0)
        return;
    BYTE* dev = g_device;
    int rect[4] = {0, 0, F(WORD, self, 0), F(WORD, self, 2)};
    if (!dev) {
        CallC(FUN_5b6f90, self, rect, color, argb);
        return;
    }
    if (!argb) {
        BYTE* pal = F(BYTE*, self, 84);
        if (!pal)
            return;
        const BYTE* c = pal + 24 + color * 3;
        color = (int)(0xFF000000u | (c[0] << 16) | (c[1] << 8) | c[2]);
    }
    CallStd(F(RFn, F(BYTE*, dev, 0), 40), dev, 1, rect, 1, color, 0x3f800000, 0);
}

// [FUN_5bbf00] converts the 8-bit run-length image to 16 bits through the palette
void __fastcall Image_RleTo16(BYTE* self, void*, int unused)
{
    if (!F(BYTE*, self, 16) || (F(BYTE, self, 28) & 0xA0) || F(WORD, self, 8) != 1)
        return;
    if (CallC<int>(FUN_5d8350) == F(WORD, self, 8))
        return;
    BYTE* buf = (BYTE*)operator new(F(int, self, 12));
    if (!buf)
        return;
    const BYTE* src = F(const BYTE*, self, 16);
    BYTE* dst = buf;
    for (int row = 0; row < F(WORD, self, 6); row++) {
        for (int col = 0; col < F(WORD, self, 4);) {
            BYTE h = *src++;
            *dst++ = h;
            int n = h & 127;
            if (!(h & 128)) {
                for (int k = n; k > 0; k--) {
                    const BYTE* pe = self + 24 + *src++ * 3;
                    *(WORD*)dst = (WORD)Pixel_Pack(pe[0], pe[1], pe[2]);
                    dst += 2;
                }
            }
            col += n;
        }
    }
    if (F(BYTE, self, 28) & 4) {
        memcpy(F(BYTE*, self, 16), buf, F(int, self, 12));
    } else {
        operator delete(F(void*, self, 16));
        F(BYTE*, self, 16) = buf;
    }
    F(DWORD, self, 28) |= 32;
    F(WORD, self, 8) = 2;
}

}

// ---------------------------------------------------------------------------
extern "C" void FUN_5b4d40(); KING_ALIAS("_FUN_5b4d40", "@Render_Sprite@32")    // draws an item (this, a, b, c, d, e, item+60)
KING_VAR(AsmLabel, s_67f0a0, 67f0a0)
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// [FUN_5b4cd0..$L_5b4cf0] draws item idx (124-byte records at +8260, count +1940) when its flag 16 is set
extern "C" void __fastcall Draw_ListItem(BYTE* self, void*, DWORD a, DWORD b, DWORD c, DWORD d, DWORD e, int idx)
{
    if (idx < 0 || idx > F(int, self, 1940))
        Sys_Fatal(s_67f0a0, 0);
    BYTE* item = F(BYTE*, self, 8260) + idx * 124;
    if (F(BYTE, item, 32) & 16)
        CallThis(FUN_5b4d40, self, a, b, c, d, e, F(DWORD, item, 60));
}


// ---------------------------------------------------------------------------
KING_VAR(BYTE*, g_rnD3d, 72090c)
KING_VAR(AsmLabel, s_rn67f3ac, 67f3ac)
KING_VAR(AsmLabel, s_rn67f390, 67f390)
KING_VAR(AsmLabel, s_rn67f37c, 67f37c)
KING_VAR(AsmLabel, s_rn67f354, 67f354)
KING_VAR(AsmLabel, s_rn67f330, 67f330)
KING_VAR(AsmLabel, s_rn67f30c, 67f30c)
KING_VAR(AsmLabel, s_rn67f2e8, 67f2e8)
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")    // log (text, value)

// [FUN_5bc060..$L_5bc291] makes the system-memory surface (+36) of an RLE image (+16, freed): runs of n pixels, bit 7 = transparent; 1 when it is (or was) made
extern "C" int __fastcall Image_RleToSurface(BYTE* self, void*)
{
    if (F(BYTE, self, 28) & 2)
        return 1;
    if (!g_device)
        return 0;
    DWORD desc[31];
    memset(desc, 0, sizeof desc);
    desc[0] = 124;
    desc[1] = 7;
    desc[2] = F(WORD, self, 6);
    desc[3] = F(WORD, self, 4);
    desc[26] = 2048;
    BYTE* surf;
    long hr = Com(g_rnD3d, 24, desc, &surf, 0);
    if (F(WORD, self, 8) != CallC<int>(FUN_5d8350))
        Sys_Fatal(s_rn67f3ac, 0);
    if (hr)
    {
        const char* t = 0;
        if (hr == (long)0x88760064)
            t = s_rn67f354;
        else if (hr == (long)0x80070057)
            t = s_rn67f330;
        else if (hr == (long)0x88760091)
            t = s_rn67f30c;
        else if (hr == (long)0x8876005F)
            t = s_rn67f2e8;
        if (t)
            CallC(FUN_5d2ba0, t, 0);
        return 0;
    }
    if (!surf)
        Sys_Fatal(s_rn67f390, 0);
    DWORD key[2] = {0, 0};
    Com(surf, 116, 8, key);
    DWORD lock[31];
    lock[0] = 124;
    lock[1] = 8387054;
    BYTE* row = 0;
    if (!Com(surf, 100, 0, lock, 1, 0))
        row = (BYTE*)lock[9];
    int pitch = (int)lock[4];
    if (row)
    {
        BYTE* src = F(BYTE*, self, 16);
        for (int y = 0; y < F(WORD, self, 6); y++, row += pitch)
        {
            BYTE* dst = row;
            for (int x = 0; x < F(WORD, self, 4); )
            {
                BYTE b = *src++;
                int n = b & 127;
                int bytes = F(WORD, self, 8) * n;
                if (b & 128)
                    memset(dst, 0, bytes);
                else
                {
                    memcpy(dst, src, bytes);
                    src += bytes;
                }
                dst += bytes;
                x += n;
            }
        }
        Com(surf, 128, 0);
        operator delete(F(BYTE*, self, 16));
        F(BYTE*, self, 16) = 0;
    }
    else
        CallC(FUN_5d2ba0, (char*)s_rn67f37c, 0);
    F(BYTE*, self, 36) = surf;
    F(DWORD, self, 28) |= 2;
    return 1;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5b1740(); KING_ALIAS("_FUN_5b1740", "@Render_FirstVisible@12")    // the next visible entry of a chain
extern "C" void FUN_5b1870(); KING_ALIAS("_FUN_5b1870", "_Vertices_Alpha")    // the alpha bytes of n vertices
extern "C" void FUN_5b17f0(); KING_ALIAS("_FUN_5b17f0", "_Points_TexCoords")    // the texture coordinates of n vertices
extern "C" void FUN_5b18e0(); KING_ALIAS("_FUN_5b18e0", "_Points_Project2")
extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")    // DirectDraw error text (hr, where)
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
typedef void (__cdecl* RnHook)(BYTE*);
KING_VAR(AsmLabel, s_rn67f084, 67f084)
KING_VAR(AsmLabel, s_rn67f058, 67f058)

// [FUN_5b1950..$L_5b1da2] draws strided primitives (n <= 1023) with the clip status of +8272: once per visible texture stage of the material chain, or with the generated texture coordinates (+8656), alphas (+8664) or both passes (+8668); returns the result
extern "C" long __fastcall Rn_DrawStrided(BYTE* self, void*, BYTE* mat, int prim, BYTE* v, int n)
{
    if (n > 1023)
        Sys_Fatal(s_rn67f084, (char*)mat);
    if (!n)
        return 0;
    DWORD fvf = F(DWORD, v, 8);
    DWORD d[24];
    DWORD flags = F(DWORD, self, 8272);
    if (!flags)
        Com(g_device, 80, 136, 0);
    else
    {
        DWORD st = 0;
        if (flags & 1) st = 32;
        if (flags & 32) st |= 16;
        if (flags & 2) st |= 1;
        if (flags & 16) st |= 2;
        if (flags & 4) st |= 4;
        if (flags & 8) st |= 8;
        d[0] = 1;
        d[1] = st;
        F(float, d, 8) = -1.0f;
        F(float, d, 12) = 1.0f;
        F(float, d, 16) = -1.0f;
        F(float, d, 20) = 1.0f;
        F(float, d, 24) = 0.0f;
        F(float, d, 28) = 1.0f;
        Com(g_device, 108, d);
        Com(g_device, 80, 136, 1);
    }
    RnHook hook = F(RnHook, self, 8628);
    if (hook)
        hook(v);
    long hr;
    if (!g_7208d4 && F(int, mat, 44) != -1)
    {
        BYTE* data = F(BYTE*, v, 4);
        memcpy(d, data, 96);
        d[8] = F(DWORD, data, F(int, mat, 48) * 8 + 32);
        d[9] = F(DWORD, data, F(int, mat, 48) * 8 + 36);
        hr = Com(g_device, 116, prim, fvf, d, n, 0);
        while (!(F(DWORD, mat, 32) & 0x4000))
        {
            mat = CallThis<BYTE*>(FUN_5b1740, self, mat);
            if (!mat)
                break;
            BYTE* p = F(BYTE*, v, 4);
            d[8] = F(DWORD, p, F(int, mat, 48) * 8 + 32);
            d[9] = F(DWORD, p, F(int, mat, 48) * 8 + 36);
            hr = Com(g_device, 116, prim, fvf, d, n, 0);
        }
    }
    else
    {
        BYTE* m = self + 5216 + F(int, self, 8236) * 48;
        if (F(int, self, 8656))
        {
            if (n > 1024)
                Sys_Fatal(s_rn67f058, 0);
            fvf = (fvf & ~0x100u) | 0x200;
            BYTE* p = F(BYTE*, v, 4);
            F(BYTE*, p, 40) = self + 8688;
            F(int, p, 44) = 8;
            p = F(BYTE*, v, 4);
            void* r = CallC<void*>(FUN_5aba40, d, m, self + 8564);
            CallC(FUN_5b17f0, self + 8688, F(void*, p, 0), F(int, p, 4), self + 8672, n, r);
        }
        if (F(int, self, 8664))
        {
            if (n > 1024)
                Sys_Fatal(s_rn67f058, 0);
            fvf = (fvf & ~0x10u) | 0x40;
            BYTE* p = F(BYTE*, v, 4);
            F(BYTE*, p, 16) = self + 16880;
            F(int, p, 20) = 4;
            p = F(BYTE*, v, 4);
            void* r = CallC<void*>(FUN_5aba40, d, m, self + 8564);
            CallC(FUN_5b1870, self + 16880, F(void*, p, 0), F(int, p, 4), n, r);
        }
        if (F(int, self, 8668))
        {
            if (n > 1024)
                Sys_Fatal(s_rn67f058, 0);
            BYTE* p = F(BYTE*, v, 4);
            DWORD d2[24];
            memcpy(d2, p, 96);
            d2[8] = (DWORD)(self + 8688);
            d2[9] = 8;
            void* r = CallC<void*>(FUN_5aba40, d, m, self + 8564);
            CallC(FUN_5b18e0, self + 8688, F(void*, p, 0), F(int, p, 4), n, r);
            p = F(BYTE*, v, 4);
            d2[4] = (DWORD)(self + 16880);
            d2[5] = 4;
            r = CallC<void*>(FUN_5aba40, d, m, self + 8564);
            CallC(FUN_5b1870, self + 16880, F(void*, p, 0), F(int, p, 4), n, r);
            return Com(g_device, 116, prim, (fvf & ~0x10u) | 0x40, d2, n, 0);
        }
        hr = Com(g_device, 116, prim, fvf, F(BYTE*, v, 4), n, 0);
    }
    if (hr)
        CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, 0), 0);
    return hr;
}


// ---------------------------------------------------------------------------

// [FUN_5b1df0..$L_5b2285] the indexed variant of Rn_DrawStrided: idx = {count (word), +8 extra vertex flags, +32 indices}, v's vertex count is its word at +0
extern "C" long __fastcall Rn_DrawIndexedStrided(BYTE* self, void*, BYTE* mat, int prim, BYTE* idx, BYTE* v)
{
    if (F(WORD, idx, 0) > 1023)
        Sys_Fatal(s_rn67f084, (char*)mat);
    DWORD fvf = F(DWORD, v, 8);
    DWORD d[24];
    DWORD flags = F(DWORD, self, 8272);
    if (!flags)
        Com(g_device, 80, 136, 0);
    else
    {
        DWORD st = 0;
        if (flags & 1) st = 32;
        if (flags & 32) st |= 16;
        if (flags & 2) st |= 1;
        if (flags & 16) st |= 2;
        if (flags & 4) st |= 4;
        if (flags & 8) st |= 8;
        d[0] = 1;
        d[1] = st;
        F(float, d, 8) = -1.0f;
        F(float, d, 12) = 1.0f;
        F(float, d, 16) = -1.0f;
        F(float, d, 20) = 1.0f;
        F(float, d, 24) = 0.0f;
        F(float, d, 28) = 1.0f;
        Com(g_device, 108, d);
        Com(g_device, 80, 136, 1);
    }
    RnHook hook = F(RnHook, self, 8628);
    if (hook)
        hook(v);
    long hr;
    if (!g_7208d4 && F(int, mat, 44) != -1)
    {
        BYTE* data = F(BYTE*, v, 4);
        memcpy(d, data, 96);
        d[8] = F(DWORD, data, F(int, mat, 48) * 8 + 32);
        d[9] = F(DWORD, data, F(int, mat, 48) * 8 + 36);
        hr = Com(g_device, 120, prim, F(DWORD, idx, 8) | fvf, d, (DWORD)F(WORD, v, 0), F(void*, idx, 32), (DWORD)F(WORD, idx, 0), 0);
        while (!(F(DWORD, mat, 32) & 0x4000))
        {
            mat = CallThis<BYTE*>(FUN_5b1740, self, mat);
            if (!mat)
                break;
            BYTE* p = F(BYTE*, v, 4);
            d[8] = F(DWORD, p, F(int, mat, 48) * 8 + 32);
            d[9] = F(DWORD, p, F(int, mat, 48) * 8 + 36);
            hr = Com(g_device, 120, prim, F(DWORD, idx, 8) | fvf, d, (DWORD)F(WORD, v, 0), F(void*, idx, 32), (DWORD)F(WORD, idx, 0), 0);
        }
    }
    else
    {
        int n = F(WORD, v, 0);
        BYTE* m = self + 5216 + F(int, self, 8236) * 48;
        if (F(int, self, 8656))
        {
            if (n > 1024)
                Sys_Fatal(s_rn67f058, 0);
            fvf |= 0x200;
            BYTE* p = F(BYTE*, v, 4);
            F(BYTE*, p, 40) = self + 8688;
            F(int, p, 44) = 8;
            p = F(BYTE*, v, 4);
            void* r = CallC<void*>(FUN_5aba40, d, m, self + 8564);
            CallC(FUN_5b17f0, self + 8688, F(void*, p, 0), F(int, p, 4), self + 8672, (int)F(WORD, v, 0), r);
        }
        else
            fvf |= F(DWORD, idx, 8);
        if (F(int, self, 8664))
        {
            if (n > 1024)
                Sys_Fatal(s_rn67f058, 0);
            fvf = (fvf & ~0x10u) | 0x40;
            BYTE* p = F(BYTE*, v, 4);
            F(BYTE*, p, 16) = self + 16880;
            F(int, p, 20) = 4;
            p = F(BYTE*, v, 4);
            void* r = CallC<void*>(FUN_5aba40, d, m, self + 8564);
            CallC(FUN_5b1870, self + 16880, F(void*, p, 0), F(int, p, 4), n, r);
        }
        if (F(int, self, 8668))
        {
            if (n > 1024)
                Sys_Fatal(s_rn67f058, 0);
            BYTE* p = F(BYTE*, v, 4);
            DWORD d2[24];
            memcpy(d2, p, 96);
            d2[8] = (DWORD)(self + 8688);
            d2[9] = 8;
            void* r = CallC<void*>(FUN_5aba40, d, m, self + 8564);
            CallC(FUN_5b18e0, self + 8688, F(void*, p, 0), F(int, p, 4), n, r);
            p = F(BYTE*, v, 4);
            d2[4] = (DWORD)(self + 16880);
            d2[5] = 4;
            r = CallC<void*>(FUN_5aba40, d, m, self + 8564);
            CallC(FUN_5b1870, self + 16880, F(void*, p, 0), F(int, p, 4), n, r);
            return Com(g_device, 120, prim, (fvf & ~0x10u) | 0x40, d2, (DWORD)F(WORD, v, 0), F(void*, idx, 32), (DWORD)F(WORD, idx, 0), 0);
        }
        hr = Com(g_device, 120, prim, fvf, F(BYTE*, v, 4), (DWORD)F(WORD, v, 0), F(void*, idx, 32), (DWORD)F(WORD, idx, 0), 0);
    }
    if (hr)
        CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, 0), 0);
    return hr;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5af000(); KING_ALIAS("_FUN_5af000", "@Clip_ByPlane@24")
extern "C" void FUN_5e9480(); KING_ALIAS("_FUN_5e9480", "_Data_Read5e9690")
extern "C" void FUN_5e9690(); KING_ALIAS("_FUN_5e9690", "_Mask_ReadPixels")
extern "C" void FUN_5b9520(); KING_ALIAS("_FUN_5b9520", "@TexSets_Free@4")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_5bad10(); KING_ALIAS("_FUN_5bad10", "@Texture_Release@8")
KING_VAR(AsmLabel, g_vt65176c, 65176c)

// [FUN_5b39a0] passes v through 5af000 with each of the n (+8244) 16-byte records at +7904; 0 when one returns 0
extern "C" int __fastcall Rn_Chain5af000(BYTE* self, void*, int a, int v, int c)
{
    BYTE* r = self + 7904;
    for (int i = 0; i < F(int, self, 8244); i++, r += 16)
    {
        v = CallThis<int>(FUN_5af000, self, a, v, c, r);
        if (!v)
            return 0;
    }
    return v;
}

// [FUN_5bbe10] loads the texture's data once (flag 4 set, 1 not): from its file (+0) or, without one, the +20 bytes of +24 into a new +12-byte buffer
extern "C" int __fastcall Texture_LoadData(BYTE* self, void*)
{
    DWORD f = F(DWORD, self, 28);
    if (!(f & 4) || (f & 1))
        return 0;
    if (F(void*, self, 0))
    {
        CallC(FUN_5e9480, self, F(void*, self, 0));
        F(DWORD, self, 28) |= 1;
        return 0;
    }
    if (F(int, self, 20) && !(f & 0x200))
    {
        F(void*, self, 16) = operator new(F(int, self, 12));
        CallC(FUN_5e9690, self, F(void*, self, 24), F(int, self, 20));
    }
    F(DWORD, self, 28) |= 1;
    return 0;
}

// [FUN_5bbca0] the texture's destructor body
extern "C" void __fastcall Texture_Destruct(BYTE* self, void*)
{
    F(void*, self, 0) = g_vt65176c;
    if (F(void*, self, 8) && !(F(BYTE, self, 52) & 4))
    {
        operator delete(F(void*, self, 8));
        F(void*, self, 8) = 0;
        if (F(void*, self, 12))
            operator delete(F(void*, self, 12));
    }
    BYTE* p = F(BYTE*, self, 16);
    if (p)
    {
        CallThis(FUN_5b9520, p);
        operator delete(p);
    }
    if (F(void*, self, 4))
        operator delete(F(void*, self, 4));
    if (F(void*, self, 28))
        CallC(FUN_5e48f0, F(void*, self, 28));
    CallThis(FUN_5bad10, self);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5b6710(); KING_ALIAS("_FUN_5b6710", "@Window_GetRect@12")
extern "C" void FUN_5b61f0(); KING_ALIAS("_FUN_5b61f0", "_Words_Fill")
extern "C" void FUN_5e7ed0(); KING_ALIAS("_FUN_5e7ed0", "_Data_Read5e8160")
extern "C" void FUN_5e8160(); KING_ALIAS("_FUN_5e8160", "_Tga_Read")
extern "C" void FUN_5bae30(); KING_ALIAS("_FUN_5bae30", "@Texture_Create@12")
extern "C" void FUN_5bb660(); KING_ALIAS("_FUN_5bb660", "@Texture_Upload@24")
extern "C" void FUN_5d7740(); KING_ALIAS("_FUN_5d7740", "_Com_Release")
extern "C" void FUN_5d7750(); KING_ALIAS("_FUN_5d7750", "_Dd_SurfaceFromPixels")
KING_VAR(AsmLabel, s_rn67f0f0, 67f0f0)

// [FUN_5b6ed0] clears the surface (flag 0x4000): the viewport rectangle through the device (+116 == 1), else each row of its memory (+48)
extern "C" void __fastcall Surface_ClearRows(BYTE* self, void*)
{
    if (!(F(DWORD, self, 12) & 0x4000))
        return;
    if (F(int, self, 116) == 1)
    {
        int r[4];
        CallStd(FUN_5b6710, r);
        int rect[4] = {r[0], r[1], r[2], r[3]};
        long hr = Com(g_device, 40, 1, rect, 2, 0, 1.0f, 0);
        if (hr)
            CallC(FUN_5d2ba0, (char*)s_rn67f0f0, hr);
        return;
    }
    if (!F(void*, self, 48))
        return;
    for (int y = 0; y < F(WORD, self, 2); y++)
        CallC(FUN_5b61f0, F(BYTE*, self, 48) + F(WORD, self, 8) * y * 2, 0, (int)F(WORD, self, 0));
}

// [FUN_5ba810] loads the image's data when first used (flag 4, counted at +60): from its file (+4) or the +32 bytes at +48, and makes its texture
extern "C" int __fastcall Image_LoadData(BYTE* self, void*)
{
    DWORD f = F(DWORD, self, 52);
    if (!(f & 4))
        return 0;
    F(int, self, 60)++;
    if (f & 1)
        return 0;
    if (!F(int, self, 32) && F(void*, self, 4))
        CallC(FUN_5e7ed0, self, F(void*, self, 4));
    else if (!(f & 0x200))
    {
        int type = F(WORD, self, 24);
        if (type == 1 || !(f & 0x400))
        {
            F(void*, self, 8) = operator new(F(WORD, self, 22) * F(WORD, self, 20) * type);
            CallC(FUN_5e8160, self, F(void*, self, 48), F(int, self, 32));
        }
    }
    f = F(DWORD, self, 52) | 1;
    F(DWORD, self, 52) = f;
    if (g_rnD3d && F(WORD, self, 24) > 1 && !(f & 0x800))
        CallThis(FUN_5bae30, self, (f >> 10) & 1);
    return 0;
}

// [FUN_5bbbf0] prepares the image for drawing (flag 2): restores and uploads its surface (+72), or without a device makes its bitmap (+44); 1 when the bitmap was made
extern "C" int __fastcall Image_Prepare(BYTE* self, void*, int a, int b)
{
    if (!(F(BYTE, self, 52) & 2))
        return 0;
    BYTE* s = F(BYTE*, self, 72);
    if (g_device && s && (F(WORD, self, 26) || (F(WORD, self, 20) == 1 && F(WORD, self, 22) == 1)))
    {
        if (Com(s, 96) == (int)0x887601C2)
            Com(F(BYTE*, self, 72), 108);
        CallThis(FUN_5bb660, self, (F(DWORD, self, 52) >> 10) & 1, 0, a, b);
        return 0;
    }
    if (F(WORD, self, 26))
        return 0;
    if (F(void*, self, 44))
        CallC(FUN_5d7740, F(void*, self, 44));
    F(void*, self, 44) = CallC<void*>(FUN_5d7750, (int)F(WORD, self, 20), (int)F(WORD, self, 22), F(void*, self, 8));
    if (!F(void*, self, 44))
        return 0;
    F(DWORD, self, 52) |= 2;
    return 1;
}

// ---------------------------------------------------------------------------
extern "C" void FUN_5aeb10(); KING_ALIAS("_FUN_5aeb10", "_Clip_Inside@8")
extern "C" void FUN_5aebc0(); KING_ALIAS("_FUN_5aebc0", "@Clip_EdgeVertex@28")

// [FUN_5aeed0] walks a polygon's n vertex ids, writing to out the ids with an outcode (5aeb10) and the crossings (5aebc0) where it changes; returns the count
extern "C" int __fastcall Rn_ClipIds(BYTE* self, void*, int a1, int n, int* out, int* ids, int a5)
{
    if (!n)
        return 0;
    BYTE* node = self + (ids[0] + 102) * 12;
    int first = CallThis<int>(FUN_5aeb10, self, node, a5);
    int prev = first, id0 = ids[0], lastId = 0, cnt = 0;
    for (int i = 0; i < n; i++)
    {
        int r = CallThis<int>(FUN_5aeb10, self, node, a5);
        if (r != prev)
        {
            CallThis(FUN_5aebc0, self, a1, out++, ids[i], lastId, a5);
            cnt++;
        }
        if (r)
        {
            *out++ = ids[i];
            cnt++;
        }
        lastId = ids[i];
        prev = r;
        node = self + (ids[i + 1] + 102) * 12;
    }
    if (first != prev)
    {
        CallThis(FUN_5aebc0, self, a1, out, id0, lastId, a5);
        cnt++;
    }
    return cnt;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_5bbca0(); KING_ALIAS("_FUN_5bbca0", "@Texture_Destruct@8")

static BYTE* Ren_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_5ba470]
extern "C" BYTE* __fastcall Ren5ba470_DeletingDestructor(BYTE* s, void*, int flags) { return Ren_DeleteAfter(s, flags, FUN_5bbca0); }

KING_VAR(float, g_ren651768, 651768)
KING_VAR(float, g_ren67f0e4, 67f0e4)
KING_VAR(int, g_ren702e88, 702e88)
KING_VAR(AsmLabel, g_ren651798, 651798)

// [$L_5ba0f0] static initializer
extern "C" void __cdecl RenInit_5ba0f0() { g_ren702e88 = (int)((double)g_ren651768 / g_ren67f0e4) >> 8; }

// [$L_5bcf00] frees the two buffers (+12, +8) under the base vtable 651798
extern "C" BYTE* __fastcall Ren5bcf00_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_ren651798;
    operator delete(F(void*, s, 12));
    operator delete(F(void*, s, 8));
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

KING_VAR(AsmLabel, g_ren651794, 651794)
KING_VAR(AsmLabel, g_ren651780, 651780)
KING_VAR(AsmLabel, g_ren64d81c, 64d81c)

// [FUN_5bcf60] (vtables 651794, 651780 at +16) frees +28, +12, +8
extern "C" void __fastcall Ren5bcf60_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_ren651794;
    F(void*, s, 16) = g_ren651780;
    operator delete(F(void*, s, 28));
    *(void**)(s ? s + 16 : 0) = g_ren64d81c;
    F(void*, s, 0) = g_ren651798;
    operator delete(F(void*, s, 12));
    operator delete(F(void*, s, 8));
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_5bcf60(); KING_ALIAS("_FUN_5bcf60", "@Ren5bcf60_Destroy@4")
// [$L_5bcf40]
extern "C" BYTE* __fastcall Dtor5bcf40_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5bcf60, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

KING_VAR(int, g_q51_702ea0, 702ea0)
// [$L_5baa30] releases the held COM objects (+124, count +184), each lowering the 702ea0 count
extern "C" void __fastcall Holder5baa30_ReleaseAll(BYTE* s, void*)
{
    while (F(int, s, 184) > 0) {
        int n = --F(int, s, 184);
        void* o = F(void*, s, 124 + n * 4);
        ((ULONG(__stdcall*)(void*))(*(void***)o)[2])(o);
        g_q51_702ea0--;
    }
}

// ---------------------------------------------------------------------------
// The software texture.

extern "C" void FUN_5b94e0(); KING_ALIAS("_FUN_5b94e0", "@Palette_Construct@8")
extern "C" void FUN_4cef50(); KING_ALIAS("_FUN_4cef50", "_Effect_Zero")
KING_VAR(AsmLabel, g_q58_65176c, 65176c)
// [FUN_5ba4a0] constructor (w, h, bytes per pixel): the 65176c vtable, the size words (+20..+24), the format (+64)
// by the depth, zeroed pixels (+8, w*h*depth), a palette (+16), and for a square the mip level count (+26)
extern "C" BYTE* __fastcall Texture5ba4a0_Construct(BYTE* s, void*, unsigned w, unsigned h, int depth)
{
    F(void*, s, 0) = g_q58_65176c;
    F(int, s, 60) = 0;
    F(WORD, s, 20) = (WORD)w;
    F(WORD, s, 22) = (WORD)h;
    F(WORD, s, 24) = (WORD)depth;
    F(int, s, 56) = 0;
    switch (depth) {
    case 1: F(int, s, 64) = 1; break;
    case 2: F(int, s, 64) = 3; break;
    case 3: F(int, s, 64) = 6; break;
    case 4: F(int, s, 64) = 7; break;
    default: F(int, s, 64) = 0; break;
    }
    F(void*, s, 8) = operator new(w * h * depth);
    F(int, s, 12) = 0;
    BYTE* p = (BYTE*)operator new(816);
    F(BYTE*, s, 16) = p ? CallThis<BYTE*>(FUN_5b94e0, p) : 0;
    F(int, s, 4) = 0;
    F(int, s, 48) = 0;
    F(int, s, 32) = 0;
    F(int, s, 40) = 0;
    F(int, s, 68) = 1;
    F(int, s, 28) = CallC<int>(FUN_4cef50, F(void*, s, 8), F(WORD, s, 24) * F(WORD, s, 20) * F(WORD, s, 22));
    F(int, s, 52) = 1;
    F(int, s, 44) = 0;
    F(WORD, s, 26) = 0;
    if (w == h) {
        for (; w; w >>= 1)
            F(WORD, s, 26)++;
        F(WORD, s, 26)--;
    }
    F(int, s, 72) = 0;
    F(int, s, 76) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Freeing texture sets; fading 4444 pixels.

extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
// [FUN_5b9520] frees the three texture sets of s (+0/+4, +8/+12, +16/+20: the blocks and their resource ids;
// counts +796, +800, +804)
extern "C" void __fastcall TexSets_Free(BYTE* s)
{
    for (int g = 0; g < 3; g++) {
        for (int i = 0; i < F(int, s, 796 + g * 4); i++)
            operator delete(F(void**, s, g * 8)[i]);
        operator delete(F(void*, s, g * 8));
        for (int i = 0; i < F(int, s, 796 + g * 4); i++)
            CallC(FUN_5e48f0, (int)F(WORD*, s, g * 8 + 4)[i]);
        operator delete(F(void*, s, g * 8 + 4));
    }
}

KING_VAR(double, g_q146_64da58, 64da58)
KING_VAR(double, g_q146_64b678, 64b678)
// [FUN_5bb590] scales the alpha of n 4444 pixels by k (at most 64da58): a negative k scales the alpha nibble,
// a positive one gives the top bits of alpha times k
extern "C" void __cdecl Pixels4444_Fade(WORD* p, unsigned n, double k)
{
    if (!(k <= g_q146_64da58) || !n)
        return;
    for (; n; n--, p++) {
        int v = *p;
        int a = (WORD)v >> 12, r = (v >> 8) & 15, g = (v >> 4) & 15, b = v & 15;
        int w;
        if (k < g_q146_64b678)
            w = ((((int)(-k * a) << 4 | r) << 4 | g) << 4) | b;
        else
            w = ((int)(a * k) & 0xf000) | (r << 4 | g) << 4 | b;
        *p = (WORD)w;
    }
}

// ---------------------------------------------------------------------------
// The texture surface pool.

extern "C" void FUN_5bac70(); KING_ALIAS("_FUN_5bac70", "@Records188x6_Destroy@8")
KING_VAR(int, g_q147_702e98, 702e98)
KING_VAR(int, g_q147_702ea0, 702ea0)
KING_VAR(BYTE*, g_q147_702ea8, 702ea8)
KING_VAR(AsmLabel, s_q147_67f114, 67f114)
typedef long(__stdcall* Ren147_GetDesc)(void*, BYTE*);
// IUnknown::Release of a DirectDraw surface
static void Ren147_Release(void* surf) { ((unsigned long(__stdcall*)(void*))(*(void***)surf)[2])(surf); }
static void Ren147_Desc(void* surf, BYTE* d, int line)
{
    memset(d, 0, 124);
    F(DWORD, d, 0) = 124;
    if (((Ren147_GetDesc)(*(void***)surf)[22])(surf, d))
        CallC(FUN_5d2ba0, (const char*)s_q147_67f114, line);
}
// [FUN_5baa70] gives texture surface surf back: with the pool on (702e98) a 64, 128 or 256 high surface goes to
// the pool's set of its size and format (at most 6 sets of 15), else it is released; without surf the pool is
// freed; with the pool off it is just released (702ea0 counting down)
extern "C" void __cdecl TexPool_Return(void* surf)
{
    if (!g_q147_702e98) {
        if (surf) {
            g_q147_702ea0--;
            Ren147_Release(surf);
        }
        return;
    }
    if (!surf) {
        if (BYTE* p = g_q147_702ea8) {
            CallThis(FUN_5bac70, p);
            operator delete(p);
        }
        g_q147_702ea8 = 0;
        return;
    }
    BYTE* pool = g_q147_702ea8;
    BYTE d[124];
    Ren147_Desc(surf, d, 51);
    if (!(F(DWORD, d, 104) & 0x400000)) {
        int n = F(int, pool, 1128);
        int i;
        for (i = 0; i < n; i++) {
            BYTE* e = pool + i * 188;
            if (F(int, d, 8) == F(int, e, 8) && F(int, d, 12) == F(int, e, 12) && F(int, d, 88) == F(int, e, 88) &&
                F(int, d, 92) == F(int, e, 92) && F(int, d, 96) == F(int, e, 96) && F(int, d, 100) == F(int, e, 100))
                break;
        }
        BYTE* e = 0;
        if (i < n)
            e = pool + i * 188;
        else if (n < 6 && (F(int, d, 8) == 256 || F(int, d, 8) == 128 || F(int, d, 8) == 64)) {
            memcpy(pool + n * 188, d, 124);
            e = pool + n * 188;
            F(int, pool, 1128) = n + 1;
        }
        if (e && F(int, e, 184) < 15) {
            F(void*, e, 124 + F(int, e, 184) * 4) = surf;
            F(int, e, 184)++;
            return;
        }
    }
    Ren147_Desc(surf, d, 1);
    Ren147_Release(surf);
}

// ---------------------------------------------------------------------------
// A sub-surface.

extern "C" void FUN_4cef50(); KING_ALIAS("_FUN_4cef50", "_Effect_Zero")
extern "C" void FUN_5d3ab0(); KING_ALIAS("_FUN_5d3ab0", "_Mode_TextureLevel")
KING_VAR(int, g_q148_702e94, 702e94)
// [FUN_5b6530] constructor: surface s as the rectangle r (clipped to it, written back) of surface p with flags
// (p's +12 less some bits, plus flags and 32): its size, p's format and pitch, its pixels and their address; an
// empty one when nothing is left; a 3D surface of p only notes the texture level
extern "C" BYTE* __fastcall SubSurface_Construct(BYTE* s, void*, BYTE* p, int* r, int flags)
{
    memcpy(s + 100, r, 16);
    F(DWORD, s, 12) = (F(DWORD, p, 12) & 0xffffb5fe) | flags | 32;
    int right = r[2] < (int)F(WORD, p, 0) ? r[2] : F(WORD, p, 0) - 1;
    r[2] = right;
    int bottom = r[3] < (int)F(WORD, p, 2) ? r[3] : F(WORD, p, 2) - 1;
    r[3] = bottom;
    int left = right < 0 ? 0 : r[0];
    r[0] = left;
    int top = bottom < 0 ? 0 : r[1];
    r[1] = top;
    if (left > right || top > bottom || right < 0 || bottom < 0) {
        F(WORD, s, 6) = 0;
        F(WORD, s, 4) = 0;
        F(WORD, s, 2) = 0;
        F(WORD, s, 0) = 0;
        F(int, s, 20) = 0;
        F(int, s, 16) = 0;
        F(int, s, 24) = 0;
        F(int, s, 56) = 0;
        return s;
    }
    F(WORD, s, 0) = (WORD)(r[2] - r[0] + 1);
    F(WORD, s, 2) = (WORD)(r[3] - r[1] + 1);
    F(WORD, s, 4) = F(WORD, p, 4);
    F(WORD, s, 6) = F(WORD, p, 6);
    F(WORD, s, 8) = F(WORD, p, 8);
    F(int, s, 80) = 0;
    F(int, s, 84) = 0;
    F(float, s, 88) = 0.5f;
    F(BYTE, s, 92) = 0;
    F(int, s, 120) = 0;
    F(BYTE*, s, 96) = p;
    F(int, s, 48) = 0;
    F(int, s, 24) = 0;
    F(int, s, 16) = 0;
    F(int, s, 20) = 1;
    F(int, s, 116) = 0;
    if (F(DWORD, p, 12) & 0x1000) {
        g_q148_702e94 = CallC<int>(FUN_5d3ab0);
        F(int, s, 116) = 1;
        F(int, s, 24) = 0;
        return s;
    }
    if (F(DWORD, s, 12) & 0x4000)
        F(BYTE*, s, 48) = F(BYTE*, p, 48) + (F(WORD, p, 8) * r[1] + r[0]) * 2;
    else
        F(int, s, 48) = 0;
    if (!F(BYTE*, p, 24))
        F(int, s, 24) = 0;
    else
        F(BYTE*, s, 24) = F(BYTE*, p, 24) + F(WORD, p, 6) * r[1] + F(WORD, s, 4) * r[0];
    F(int, s, 56) = CallC<int>(FUN_4cef50, F(BYTE*, s, 24), F(WORD, s, 6) * F(WORD, s, 2));
    return s;
}

// ---------------------------------------------------------------------------
// Creating a surface.

extern "C" void FUN_5d3fe0(); KING_ALIAS("_FUN_5d3fe0", "_Dd_LockPrimary")
extern "C" void FUN_5d3b00(); KING_ALIAS("_FUN_5d3b00", "_Video_Get7208a4")
// allocates buffer k of surface s (+24.. video memory blocks, +56.. their cleared counts)
static void Ren149_Buffer(BYTE* s, int k)
{
    int n = F(WORD, s, 4) * F(WORD, s, 2) * F(WORD, s, 0);
    BYTE* p = CallC<BYTE*>(FUN_5d3fe0, n);
    F(BYTE*, s, 24 + k * 4) = p;
    F(int, s, 56 + k * 4) = CallC<int>(FUN_4cef50, p, F(WORD, s, 4) * F(WORD, s, 2) * F(WORD, s, 0));
}
// [FUN_5b6770] sets up surface s of w x h pixels of bpp bytes with flags: a 3D one (bit 12) only notes the
// texture level; in video memory (bit 9) with up to 3 or 6 buffers (bits 0 and 8) and the mode's pitch, else
// one block in memory; bit 14 adds a 2-byte-per-pixel side buffer (+48)
extern "C" void __fastcall Surface_Create(BYTE* s, void*, int w, int h, int bpp, DWORD flags)
{
    F(WORD, s, 6) = (WORD)(bpp * w);
    F(int, s, 96) = 0;
    F(DWORD, s, 12) = flags;
    F(WORD, s, 0) = (WORD)w;
    F(WORD, s, 2) = (WORD)h;
    F(WORD, s, 4) = (WORD)bpp;
    F(WORD, s, 8) = 0;
    F(int, s, 80) = 0;
    F(int, s, 84) = 0;
    F(float, s, 88) = 0.5f;
    F(BYTE, s, 92) = 0;
    F(int, s, 120) = 0;
    F(int, s, 48) = 0;
    F(int, s, 16) = 0;
    F(int, s, 20) = 1;
    F(int, s, 116) = 0;
    if (flags & 0x1000) {
        g_q148_702e94 = CallC<int>(FUN_5d3ab0);
        F(int, s, 116) = 1;
        F(int, s, 24) = 0;
        return;
    }
    bool more = false;
    if (flags & 0x200) {
        Ren149_Buffer(s, 0);
        F(WORD, s, 6) = CallC<WORD>(FUN_5d3b00);
        if (F(BYTE, s, 12) & 1) {
            Ren149_Buffer(s, 1);
            Ren149_Buffer(s, 2);
            F(int, s, 16) = 1;
            if (F(DWORD, s, 12) & 0x100) {
                Ren149_Buffer(s, 3);
                Ren149_Buffer(s, 4);
                Ren149_Buffer(s, 5);
                F(int, s, 20) = 6;
            } else
                F(int, s, 20) = 3;
            more = true;
        }
    } else {
        int n = (WORD)bpp * (WORD)h * (WORD)w;
        BYTE* p = (BYTE*)operator new(n);
        F(BYTE*, s, 24) = p;
        F(int, s, 56) = CallC<int>(FUN_4cef50, p, F(WORD, s, 4) * F(WORD, s, 2) * F(WORD, s, 0));
    }
    if (!more) {
        F(int, s, 16) = 0;
        F(int, s, 20) = 1;
    }
    if (F(DWORD, s, 12) & 0x4000) {
        F(void*, s, 48) = operator new(F(WORD, s, 2) * F(WORD, s, 0) * 2);
        F(WORD, s, 8) = F(WORD, s, 0);
    } else
        F(int, s, 48) = 0;
}

// ---------------------------------------------------------------------------
// Preparing a surface for drawing.

extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")
extern "C" void FUN_5b6710(); KING_ALIAS("_FUN_5b6710", "@Window_GetRect@12")
KING_VAR(int, g_q150_72090c, 72090c)
KING_VAR(void*, g_q150_7208f8, 7208f8)
KING_VAR(void*, g_q150_7208fc, 7208fc)
KING_VAR(void*, g_q150_720900, 720900)
KING_VAR(void*, g_q150_720908, 720908)
typedef long(__stdcall* Ren150_Com0)(void*);
// restores DirectDraw surface d when it was lost, logging a failure with line
static void Ren150_Restore(void* d, int line)
{
    if (!d || ((Ren150_Com0)(*(void***)d)[24])(d) != (long)0x887601c2)
        return;
    if (long hr = ((Ren150_Com0)(*(void***)d)[27])(d))
        CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, 0), line);
}
// [FUN_5b6bf0] prepares surface s for drawing: lost DirectDraw surfaces are restored first (for a top surface);
// a 3D surface sets the device viewport to its visible part; a video surface locks its buffer; a sub-surface
// takes its address in the parent's locked buffer
extern "C" void __fastcall Surface_Prepare(BYTE* s)
{
    if (!F(BYTE*, s, 96) && g_q150_72090c) {
        Ren150_Restore(g_q150_7208f8, 18);
        if (!(F(DWORD, s, 12) & 0x2000))
            Ren150_Restore(g_q150_7208fc, 17);
        Ren150_Restore(g_q150_720900, 16);
    }
    BYTE* p = F(BYTE*, s, 96);
    if (!(F(DWORD, s, 12) & 0x1000) && !(p && (F(DWORD, p, 12) & 0x1000))) {
        if (F(DWORD, s, 12) & 0x200) {
            F(BYTE*, s, 24 + F(int, s, 16) * 4) = CallC<BYTE*>(FUN_5d3fe0, F(int, s, 16));
            F(WORD, s, 6) = CallC<WORD>(FUN_5d3b00);
            return;
        }
        if (!p)
            return;
        BYTE* b = F(BYTE*, p, 24 + F(int, p, 16) * 4);
        if (!b)
            F(int, s, 24) = 0;
        else
            F(BYTE*, s, 24) = b + F(WORD, p, 6) * F(int, s, 104) + F(WORD, s, 4) * F(int, s, 100);
        F(WORD, s, 6) = F(WORD, p, 6);
        return;
    }
    DWORD vp[6] = {0, 0, 0, 0, 0, 0};
    int o[2];
    CallThis(FUN_5b6710, s, o);
    int x = o[0], w = F(WORD, s, 0);
    if (x < 0) {
        w += x;
        x = 0;
    }
    vp[0] = x;
    int y = o[1], h = F(WORD, s, 2);
    if (y < 0) {
        h += y;
        y = 0;
    }
    vp[3] = h;
    vp[1] = y;
    vp[2] = w;
    vp[5] = 0x3f800000;
    vp[4] = 0;
    if (void* d = g_q150_720908)
        if (long hr = ((long(__stdcall*)(void*, DWORD*))(*(void***)d)[13])(d, vp))
            CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, 0), 0);
    F(int, s, 24 + F(int, s, 16) * 4) = 0;
}

// ---------------------------------------------------------------------------
// Horizontal lines.

extern "C" void FUN_5b73a0(); KING_ALIAS("_FUN_5b73a0", "_Window_DrawLine")
ASM_PROC(FUN_5fe9b3)
ASM_PROC(FUN_5fea6c)
ASM_PROC(FUN_5feb12)
ASM_PROC(FUN_5feace)
ASM_PROC(FUN_5feafa)
extern "C" void FUN_5b64c0(); KING_ALIAS("_FUN_5b64c0", "@Window_RootSurface@8")
extern "C" void FUN_5b94b0(); KING_ALIAS("_FUN_5b94b0", "@Rows_Get@12")
extern "C" void FUN_5ba3f0(); KING_ALIAS("_FUN_5ba3f0", "@Palette_Color@12")
KING_VAR(double, g_q151_64b670, 64b670)
KING_VAR(double, g_q151_64b678, 64b678)
KING_VAR(float, g_q151_64c458, 64c458)
// x87 equality: also true for unordered values
static bool Ren151_Eq(double a, double b) { return !(a < b || a > b); }
// [FUN_5b7ce0] draws the line x1..x2 of row y on surface s in color (a palette index unless raw), clipped; a 3D
// surface draws it as a line; blend mode 2 (+80) with an alpha (+88) other than 1 blends through the palette's
// tables
extern "C" void __cdecl Surface_HLine(BYTE* s, int y, int x1, int x2, int color, int raw)
{
    BYTE* pal = CallThis<BYTE*>(FUN_5b64c0, s);
    if (y < 0 || y >= (int)F(WORD, s, 2))
        return;
    int a = x1, b = x2;
    if (b < a) {
        a = x2;
        b = x1;
    }
    int w = F(WORD, s, 0);
    if (a >= w || b < 0)
        return;
    if (a < 0)
        a = 0;
    if (b >= w)
        b = w - 1;
    if (F(int, s, 116) == 1) {
        CallC(FUN_5b73a0, s, a, y, b, y, color, 0);
        return;
    }
    BYTE* buf = F(BYTE*, s, 24 + F(int, s, 16) * 4);
    if (!buf)
        return;
    WORD bpp = F(WORD, s, 4);
    int n = b - a + 1;
    BYTE* p = buf + bpp * a + F(WORD, s, 6) * y;
    int mode = F(int, s, 80);
    if (mode != 0 && !(mode == 2 && Ren151_Eq(F(float, s, 88), g_q151_64b670))) {
        if (mode != 2 || !pal || Ren151_Eq(F(float, s, 88), g_q151_64b678))
            return;
        BYTE* t = CallThis<BYTE*>(FUN_5b94b0, pal, (int)(F(int, pal, 796) * F(float, s, 88)));
        if (bpp == 1)
            CallC(FUN_5fe9b3, t, p, color, n);
        else if (bpp == 2)
            CallC(FUN_5fea6c, t, p, color, n);
        else if (bpp == 4) {
            int c = raw ? color : CallThis<int>(FUN_5ba3f0, pal, color);
            CallC(FUN_5feb12, (int)(F(float, s, 88) * g_q151_64c458), p, c, n);
        }
        return;
    }
    if (bpp == 1)
        memset(p, (BYTE)color, n);
    else if (bpp == 2) {
        if (!raw) {
            if (!pal)
                return;
            color = CallThis<int>(FUN_5ba3f0, pal, color);
        }
        CallC(FUN_5feace, p, color, n);
    } else if (bpp == 4) {
        if (!raw) {
            if (!pal)
                return;
            color = CallThis<int>(FUN_5ba3f0, pal, color);
        }
        CallC(FUN_5feafa, p, color, n);
    }
}

// [FUN_5b7f20] draws the line y1..y2 of column x on surface s like 5b7ce0 does for rows
extern "C" void __cdecl Surface_VLine(BYTE* s, int x, int y1, int y2, int color, int raw)
{
    BYTE* pal = CallThis<BYTE*>(FUN_5b64c0, s);
    if (x < 0 || x >= (int)F(WORD, s, 0))
        return;
    int a = y1, b = y2;
    if (b < a) {
        a = y2;
        b = y1;
    }
    int h = F(WORD, s, 2);
    if (a >= h || b < 0)
        return;
    if (a < 0)
        a = 0;
    if (b >= h)
        b = h - 1;
    if (F(int, s, 116) == 1) {
        CallC(FUN_5b73a0, s, x, a, x, b, color, 0);
        return;
    }
    BYTE* buf = F(BYTE*, s, 24 + F(int, s, 16) * 4);
    if (!buf)
        return;
    WORD bpp = F(WORD, s, 4);
    int pitch = F(WORD, s, 6);
    BYTE* p = buf + pitch * a + bpp * x;
    int n = b - a + 1;
    int mode = F(int, s, 80);
    if (mode != 0 && !(mode == 2 && Ren151_Eq(F(float, s, 88), g_q151_64b670))) {
        if (mode != 2 || !pal || Ren151_Eq(F(float, s, 88), g_q151_64b678))
            return;
        BYTE* t = CallThis<BYTE*>(FUN_5b94b0, pal, (int)(F(int, pal, 796) * F(float, s, 88)));
        if (a > b)
            return;
        if (bpp == 1) {
            for (int c = color << 8; n; n--, p += pitch)
                *p = t[c + *p];
        } else if (bpp == 2) {
            for (; n; n--, p += pitch)
                CallC(FUN_5fea6c, t, p, color, 1);
        } else if (bpp == 4) {
            int c = raw ? color : CallThis<int>(FUN_5ba3f0, pal, color);
            for (; n; n--, p += pitch)
                CallC(FUN_5feb12, (int)(F(float, s, 88) * g_q151_64c458), p, c, 1);
        }
        return;
    }
    if (bpp == 1) {
        for (; a <= b && n; n--, p += pitch)
            *p = (BYTE)color;
        return;
    }
    if (bpp != 2 && bpp != 4)
        return;
    if (!raw) {
        if (!pal)
            return;
        color = CallThis<int>(FUN_5ba3f0, pal, color);
    }
    if (a > b)
        return;
    for (; n; n--, p += pitch)
        if (bpp == 2)
            F(WORD, p, 0) = (WORD)color;
        else
            F(int, p, 0) = color;
}

// ---------------------------------------------------------------------------
// Drawing run-length images.

ASM_PROC(FUN_5feff3)
ASM_PROC(FUN_5fea03)
ASM_PROC(FUN_5fe860)
KING_VAR(unsigned, g_q153_702e8c, 702e8c)
// [FUN_5b8720] draws run-length image img (w +4, h +6, data +16: a byte with bit 7 skips that many pixels, else
// copies that many) at x, y on dst; blend mode 2 (+80) with an alpha other than 1 goes through the palette's
// tables; 8-bit surfaces outside the 702e8c bit 14 mode use the fast 5feff3 copy
extern "C" void __cdecl Surface_DrawRle(BYTE* dst, int x, int y, BYTE* img)
{
    BYTE* pal = CallThis<BYTE*>(FUN_5b64c0, dst);
    BYTE* t = 0;
    if (F(int, dst, 80) == 2) {
        if (!pal || Ren151_Eq(F(float, dst, 88), g_q151_64b678))
            return;
        if (!Ren151_Eq(F(float, dst, 88), g_q151_64b670))
            t = CallThis<BYTE*>(FUN_5b94b0, pal, (int)(F(int, pal, 796) * F(float, dst, 88)));
    } else if (F(WORD, dst, 4) == 1 && !(g_q153_702e8c & 0x4000)) {
        int pitch = F(WORD, dst, 6);
        BYTE* d = F(BYTE*, dst, 24 + F(int, dst, 16) * 4) + pitch * y + x;
        CallC(FUN_5feff3, F(BYTE*, img, 16), d, x, (int)F(WORD, img, 4), (int)F(WORD, img, 6), pitch);
        return;
    }
    BYTE* buf = F(BYTE*, dst, 24 + F(int, dst, 16) * 4);
    const BYTE* src = F(const BYTE*, img, 16);
    if (!buf)
        return;
    BYTE* row = buf + F(WORD, dst, 4) * x + F(WORD, dst, 6) * y;
    for (int yy = y; yy - y < (int)F(WORD, img, 6); yy++, row += F(WORD, dst, 6)) {
        BYTE* d = row;
        for (int cx = x; cx - x < (int)F(WORD, img, 4);) {
            BYTE c = *src++;
            WORD bpp = F(WORD, dst, 4);
            if (c & 128) {
                cx += c & 127;
                d += bpp * (c & 127);
                continue;
            }
            int n = c & 127;
            if (bpp == 1) {
                if (F(int, dst, 80) == 2 && t)
                    CallC(FUN_5fe860, t, d, src, n);
                else
                    memcpy(d, src, n);
            } else if (bpp == 2) {
                if (F(int, dst, 80) == 2 && t)
                    CallC(FUN_5fea03, t, d, src, n);
                else
                    memcpy(d, src, n * 2);
            }
            cx += n;
            d += F(WORD, dst, 4) * n;
            src += n;
        }
    }
}

// ---------------------------------------------------------------------------
// Direct3D device states.

KING_VAR(void*, g_q154_702e80, 702e80)
static void Ren154_RS(void* d, int st, int v) { ((long(__stdcall*)(void*, int, int))(*(void***)d)[20])(d, st, v); }
static void Ren154_TSS(void* d, int stage, int type, int v)
{
    ((long(__stdcall*)(void*, int, int, int))(*(void***)d)[37])(d, stage, type, v);
}
// a black material (all colours 0,0,0,1, power 0) with lighting and specular states on
static void Ren154_Material(void* d)
{
    DWORD m[17] = {0, 0, 0, 0x3f800000, 0, 0, 0, 0x3f800000, 0, 0, 0, 0x3f800000, 0, 0, 0, 0x3f800000, 0};
    Ren154_RS(d, 148, 1);
    Ren154_RS(d, 145, 1);
    ((long(__stdcall*)(void*, DWORD*))(*(void***)d)[16])(d, m);
    Ren154_RS(d, 27, 0);
}
// [FUN_5b0120] sets the device's (720908) render states for drawing mode (1: plain textures, 2: with the 702e80
// texture in stage 0 blended over stage 1, fog colour states)
extern "C" void __cdecl Device_SetStates(int mode)
{
    void* d = g_q150_720908;
    Ren154_RS(d, 22, 2);
    Ren154_RS(g_q150_720908, 4, 1);
    Ren154_RS(g_q150_720908, 41, 1);
    Ren154_RS(g_q150_720908, 7, (g_q153_702e8c & 0x4000) ? 1 : 0);
    static const BYTE rs[10][2] = {{9, 2}, {26, 1}, {19, 5}, {20, 6}, {27, 0}, {29, 1}, {141, 1}, {147, 0},
                                   {146, 0}, {15, 0}};
    for (int k = 0; k < 10; k++)
        Ren154_RS(g_q150_720908, rs[k][0], rs[k][1]);
    if (mode == 1) {
        Ren154_TSS(g_q150_720908, 0, 4, 1);
        Ren154_TSS(g_q150_720908, 0, 1, 1);
        Ren154_Material(g_q150_720908);
    } else if (mode == 2) {
        d = g_q150_720908;
        ((long(__stdcall*)(void*, int, void*))(*(void***)d)[35])(d, 0, g_q154_702e80);
        static const BYTE ts[9][3] = {{0, 11, 0}, {0, 6, 2}, {0, 5, 0}, {0, 4, 10}, {0, 3, 2},
                                      {0, 2, 0},  {0, 1, 2}, {1, 4, 1}, {1, 1, 1}};
        for (int k = 0; k < 9; k++)
            Ren154_TSS(g_q150_720908, ts[k][0], ts[k][1], ts[k][2]);
        Ren154_Material(g_q150_720908);
        Ren154_RS(g_q150_720908, 24, 15);
        Ren154_RS(g_q150_720908, 15, 1);
        Ren154_RS(g_q150_720908, 25, 5);
    }
}

// ---------------------------------------------------------------------------
// Projecting vertices.

// [FUN_5b18e0] transforms n points (3 floats, every stride bytes) by the x and z rows of 3x4 matrix m into out
// (2 floats each)
extern "C" void __cdecl Points_Project2(float* out, const BYTE* in, unsigned stride, int n, const float* m)
{
    stride &= ~3u;
    for (; n > 0; n--, in += stride, out += 2) {
        const float* p = (const float*)in;
        double y = p[2] * m[8] + p[0] * m[2];
        y = y + m[5] * p[1];
        y = y + m[11];
        double x = m[3] * p[1] + p[2] * m[6];
        x = x + p[0] * m[0];
        x = x + m[9];
        out[0] = (float)x;
        out[1] = (float)y;
    }
}

// [FUN_5b17f0] the texture coordinates of n points: transformed by m and mapped into rectangle r (u across
// r[0]..r[2], v across r[3]..r[1])
extern "C" void __cdecl Points_TexCoords(float* out, const BYTE* in, unsigned stride, const float* r, int n,
                                         const float* m)
{
    stride &= ~3u;
    for (; n > 0; n--, in += stride, out += 2) {
        const float* p = (const float*)in;
        double u = p[1] * m[4] + p[2] * m[7];
        u = u + p[0] * m[1];
        u = u + m[10];
        double v = m[6] * p[2] + p[0] * m[0];
        v = v + m[3] * p[1];
        v = v + m[9];
        out[0] = (float)((v - r[0]) / (r[2] - r[0]));
        out[1] = (float)((u - r[3]) / (r[1] - r[3]));
    }
}

KING_VAR(double, g_q160_64b6e0, 64b6e0)
// [FUN_5ad970] the fog value at point p for light s at t: a point light (kind 1) scales by how the point faces it
// ((1 + dot(-p, +16..+24)) * 64b6e0, between t and 1); kinds 2 and 3 give t; others the argument's bits
extern "C" double __fastcall Light_FogAt(const BYTE* s, void*, const float* p, double t)
{
    int k = F(int, s, 0);
    if (k == 1) {
        double d = F(float, s, 24) * -p[2] + F(float, s, 20) * -p[1];
        d = d + F(float, s, 16) * -p[0];
        return (d + g_q151_64b670) * g_q160_64b6e0 * (g_q151_64b670 - t) + t;
    }
    if (k == 2 || k == 3)
        return t;
    return *(const float*)&p;
}

KING_VAR(double, g_q161_64eba0, 64eba0)
// [FUN_5addf0] sets the ambient light of renderer s to (r, g, b) (g and b default to r when negative; r kept at
// +8512) as the device's ambient render state
extern "C" void __fastcall Render_SetAmbient(BYTE* s, void*, double r, double g, double b)
{
    F(double, s, 8512) = r;
    if (!(g >= g_q151_64b678))
        g = r;
    if (!(b >= g_q151_64b678))
        b = r;
    void* d = g_q150_720908;
    if (!d)
        return;
    int c = ((int)(r * g_q161_64eba0) | 0xffffff00) << 8;
    c = (c | (int)(g * g_q161_64eba0)) << 8;
    c |= (int)(b * g_q161_64eba0);
    Ren154_RS(d, 139, c);
}

KING_VAR(float, g_q161_64c250, 64c250)
// [FUN_5aeb10] whether vertex v is inside clip plane k (1: y above 64c250, 2: x >= -y, 4: z <= y, 8: z >= -y,
// 16: x <= y; others always)
extern "C" int __stdcall Clip_Inside(const float* v, int k)
{
    switch (k) {
    case 1: return v[1] >= g_q161_64c250 ? 1 : 0;
    case 2: return -v[1] > v[0] ? 0 : 1;
    case 4: return v[2] > v[1] ? 0 : 1;
    case 8: return -v[1] > v[2] ? 0 : 1;
    case 16: return v[0] > v[1] ? 0 : 1;
    }
    return 1;
}

// [FUN_5aeda0] adds the clip vertex at t between vertices i and j of renderer s (positions +1224, 12 bytes
// each; with flags bit 5 the texture coordinates +1488/+1576, with bit 7 the values +1664); its index goes to
// *out
extern "C" void __fastcall Clip_Interpolate(BYTE* s, void*, int flags, int* out, int i, int j, double t)
{
    *out = F(int, s, 1932);
    const float* a = (const float*)(s + 1224 + i * 12);
    const float* b = (const float*)(s + 1224 + j * 12);
    float dy = (float)(b[1] - a[1]);
    float* n = (float*)(s + 1224 + F(int, s, 1932) * 12);
    float dz = (float)(b[2] - a[2]);
    n[0] = (float)((b[0] - a[0]) * t + a[0]);
    n[1] = (float)(dy * t + a[1]);
    n[2] = (float)(dz * t + a[2]);
    float* u = (float*)(s + 1488);
    float* v = (float*)(s + 1576);
    float* w = (float*)(s + 1664);
    int k = F(int, s, 1932);
    if (flags & 32) {
        u[k] = (float)((u[j] - u[i]) * t + u[i]);
        v[k] = (float)((v[j] - v[i]) * t + v[i]);
    }
    if (flags & 128)
        w[k] = (float)((w[j] - w[i]) * t + w[i]);
    F(int, s, 1932)++;
}

// [$L_5ae880] nothing to do
extern "C" void __fastcall Ren5ae880_Nop(BYTE*) {}
// [$L_5ae890] nothing to do
extern "C" void __fastcall Ren5ae890_Nop(BYTE*) {}
// [$L_5b2940] nothing to do
extern "C" void __fastcall Ren5b2940_Nop(BYTE*) {}
// [$L_5b3000] nothing to do
extern "C" void __fastcall Ren5b3000_Nop(BYTE*) {}
// [$L_5b3990] nothing to do
extern "C" void __fastcall Ren5b3990_Nop(BYTE*) {}
// [$L_5baa20] clears +184
extern "C" BYTE* __fastcall Ren5baa20_Init(BYTE* s)
{
    F(int, s, 184) = 0;
    return s;
}

extern "C" void FUN_5ba120(); KING_ALIAS("_FUN_5ba120", "_Fog_SetRange")
extern "C" void FUN_5b6700(); KING_ALIAS("_FUN_5b6700", "@Window_RootSurface@8")
KING_VAR(double, g_q218_64eba8, 64eba8)
// [FUN_5adc80] sets renderer s's fog from start to end (kept over the scale +8464 at +8540, +8536) with palette
// colour mode (+8544; none when negative); with fog on (flag 4 of 702e8c) the device gets the fog states, the
// colour from entry mode + 8 of the root surface's palette
extern "C" void __fastcall Render_SetFog(BYTE* s, void*, double start, double end, int mode)
{
    float e = (float)(end / F(double, s, 8464));
    F(int, s, 8544) = mode;
    F(float, s, 8536) = e;
    float b = (float)(start / F(double, s, 8464));
    F(float, s, 8540) = b;
    if (mode >= 0)
        CallC(FUN_5ba120, b, e, mode);
    void* d = g_q150_720908;
    if (!d)
        return;
    if (!(F(BYTE, &g_q153_702e8c, 0) & 4)) {
        Ren154_RS(d, 28, 0);
        return;
    }
    Ren154_RS(d, 28, 1);
    BYTE* pal = CallThis<BYTE*>(FUN_5b6700, F(BYTE*, s, 1948));
    if (pal) {
        const BYTE* c = pal + 3 * (mode + 8);
        int v = ((int)(c[0] * g_q218_64eba8 * g_q161_64eba0) | 0xffffff00) << 8;
        v = (v | (int)(c[1] * g_q218_64eba8 * g_q161_64eba0)) << 8;
        v |= (int)(c[2] * g_q218_64eba8 * g_q161_64eba0);
        Ren154_RS(g_q150_720908, 34, v);
    }
    Ren154_RS(g_q150_720908, 140, 3);
    float f = (float)start;
    Ren154_RS(g_q150_720908, 36, *(int*)&f);
    f = (float)end;
    Ren154_RS(g_q150_720908, 37, *(int*)&f);
}

extern "C" void FUN_5ad5c0(); KING_ALIAS("_FUN_5ad5c0", "@Light_ReachesSphere@12")
// [FUN_5ad9e0] how sphere sp (centre, radius) lies against renderer s's clip planes (+8240 of them, 80 bytes each
// from +6224): the centre is moved by the current matrix (+8236 into the stack at +5216), then the highest plane
// result; 0 without planes
extern "C" int __fastcall Render_ClipSphere(BYTE* s, void*, const float* sp)
{
    int n = F(int, s, 8240);
    if (n <= 0)
        return 0;
    const float* m = (const float*)(s + 5216 + F(int, s, 8236) * 48);
    float c[4];
    c[0] = (float)(sp[2] * m[6] + sp[1] * m[3] + sp[0] * m[0] + m[9]);
    c[1] = (float)(sp[2] * m[7] + sp[1] * m[4] + sp[0] * m[1] + m[10]);
    c[2] = (float)(sp[2] * m[8] + sp[1] * m[5] + sp[0] * m[2] + m[11]);
    c[3] = sp[3];
    int best = 0;
    for (int i = n - 1; i >= 0; i--) {
        int r = CallThis<int>(FUN_5ad5c0, s + 6224 + i * 80, c);
        if (r > best)
            best = r;
    }
    return best;
}

extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
extern "C" void FUN_5ab660(); KING_ALIAS("_FUN_5ab660", "@Vec3_DivAssign@16")
KING_VAR(float, g_q246_64c264, 64c264)
KING_VAR(float, g_q246_64c450, 64c450)
KING_VAR(double, g_q246_64da00, 64da00)
// [FUN_5ad5c0] how light l (+0 kind, +4 position, +16 direction, +36 attenuation) reaches sphere sp: 1 for kind 1
// (everywhere), 2 when it may light it, 0 when not: a point light (2) within reach, a spot (3) also within its cone
extern "C" int __fastcall Light_ReachesSphere(int* l, void*, const float* sp)
{
    int kind = l[0];
    if (kind == 1)
        return 1;
    if (kind != 2 && kind != 3)
        return 0;
    const float* p = (const float*)(l + 1);
    float v[3] = {(float)(p[0] - sp[0]), (float)(p[1] - sp[1]), (float)(p[2] - sp[2])};
    double d = CallC<double>(FUN_5ab4d0, v) - sp[3];
    if (!(d >= g_q246_64c264))
        return 2;
    bool reach = !(d * F(float, l, 36) * d > g_q246_64c450);
    if (kind == 2)
        return reach ? 2 : 0;
    if (!reach)
        return 0;
    float tmp[3];
    CallThis(FUN_5ab660, v, tmp, (float)CallC<double>(FUN_5ab4d0, v));
    float n[3] = {-v[0], -v[1], -v[2]};
    const float* dir = (const float*)(l + 4);
    double a = x87_acos(dir[2] * n[2] + dir[1] * n[1] + dir[0] * n[0]);
    return a > g_q246_64da00 ? 0 : 2;
}

// === texture stages of a material (FUN_5b0810)

KING_VAR(int, g_ts7208bc, 7208bc)
KING_VAR(int, g_ts7208c0, 7208c0)
KING_VAR(int, g_ts7208c4, 7208c4)
KING_VAR(int, g_ts7208c8, 7208c8)
KING_VAR(int, g_ts7208cc, 7208cc)
KING_VAR(int, g_tsMulti7208d4, 7208d4)        // multitexturing
KING_VAR(int, g_ts702e84, 702e84)
KING_VAR(DWORD, g_tsCaps702e8c, 702e8c)
KING_VAR(float, g_ts64c458, 64c458)
KING_VAR(double, g_ts64fa38, 64fa38)
KING_VAR(double, g_ts651750, 651750)
KING_VAR(double, g_ts651748, 651748)
KING_VAR(AsmLabel, s_ts67f044, 67f044)
extern "C" void FUN_5bbbf0(); KING_ALIAS("_FUN_5bbbf0", "@Image_Prepare@16")

static void Ts_State(int stage, int state, DWORD value)
{
    BYTE* d = g_device;
    (*(HRESULT(__stdcall**)(BYTE*, int, int, DWORD))(*(BYTE**)d + 148))(d, stage, state, value);
}

static void Ts_Texture(int stage, void* tex)
{
    BYTE* d = g_device;
    (*(HRESULT(__stdcall**)(BYTE*, int, void*))(*(BYTE**)d + 140))(d, stage, tex);
}

static DWORD Ts_Bits(float f)
{
    return *(DWORD*)&f;
}

static __declspec(noinline) int Ts_Int(double d)
{
    return (int)d;
}

extern "C" {

// [FUN_5b0810] sets up texture stage stage (and the next ones for linked materials, +44) for material m after
// prev: the texture, coordinates, color and alpha operations by the material's flags (+32), bump mapping,
// addressing and filters; 1 when a stage blends with the vertex alpha
int __fastcall Material_SetStages(BYTE* self, void*, BYTE* m, BYTE* prev, int stage)
{
    int result = 0;
    if (F(DWORD, m, 32) & 0x80000) {
        while (!g_ts7208c4) {
            DWORD f = F(DWORD, m, 32);
            if (!g_ts7208bc || F(int, m, 44) < 0 || (f & 0x20000))
                return 0;
            result = 0;
            m = F(BYTE*, self, 8260) + F(int, m, 44) * 124;
            if (!(F(DWORD, m, 32) & 0x80000))
                break;
        }
    }
    int st = g_tsMulti7208d4 ? stage : 0;
    int coords = g_tsMulti7208d4 ? F(int, m, 48) : 0;
    if (stage == 0 || coords > F(int, self, 8248))
        F(int, self, 8248) = coords;
    DWORD flags = F(DWORD, m, 32);
    if (!(flags & 1)) {
        Ts_Texture(0, 0);
        Ts_State(0, 1, 1);
        Ts_State(0, 4, 1);
        return 0;
    }
    BYTE* tex = F(BYTE*, m, 76);
    if (F(int, tex, 64) == 5 && !(flags & 0x4000) && !g_ts702e84)
        result = 1;
    Ts_Texture(st, F(void*, tex, 76));
    Ts_State(st, 11, coords);
    if ((flags & 0x2000) && g_ts7208cc) {
        BYTE* d = g_device;
        (*(HRESULT(__stdcall**)(BYTE*, int, DWORD))(*(BYTE**)d + 80))(d, 60, Ts_Int((double)F(float, m, 104) * g_ts64c458) << 24);
    }
    BYTE* img = F(BYTE*, tex, 72);
    if (!img) {
        char buf[100];
        sprintf(buf, s_ts67f044, m);
        CallC(FUN_5d2ba0, buf, 0);
    } else if ((*(HRESULT(__stdcall**)(BYTE*))(*(BYTE**)img + 96))(img) == (HRESULT)0x887601C2)
        CallThis(FUN_5bbbf0, tex, 1.0);
    if (!(prev && F(DWORD, prev, 32) == F(DWORD, m, 32)
          && F(int, F(BYTE*, m, 76), 52) == F(int, F(BYTE*, prev, 76), 52))) {
        if (stage == 0) {
            Ts_State(st, 2, 2);
            Ts_State(st, 3, 0);
            Ts_State(st, 1, 4);
            g_ts702e84 = 0;
            flags = F(DWORD, m, 32);
            if (flags & 0x4000) {
                if (g_tsMulti7208d4 && (g_tsCaps702e8c & 0x1000000)) {
                    g_ts702e84 = 1;
                    Ts_State(st, 5, 2);
                    Ts_State(st, 4, 2);
                } else {
                    Ts_State(st, 5, 2);
                    Ts_State(st, 6, 0);
                    Ts_State(st, 4, 4);
                }
            } else if (flags & 2) {
                Ts_State(st, 5, 2);
                Ts_State(st, 4, 2);
            } else {
                Ts_State(st, 5, 2);
                Ts_State(st, 6, 0);
                Ts_State(st, 4, 4);
            }
        } else {
            flags = F(DWORD, m, 32);
            bool plain = false;
            if (flags & 64) {
                if (g_ts7208bc) {
                    Ts_State(st, 2, 2);
                    Ts_State(st, 3, 1);
                    Ts_State(st, 1, 4);
                    if (F(int, F(BYTE*, m, 76), 64) == 5) {
                        Ts_State(st, 5, 2);
                        Ts_State(st, 6, 1);
                        Ts_State(st, 4, 4);
                    } else if (g_ts7208c8) {
                        Ts_State(st, 6, 1);
                        Ts_State(st, 4, 3);
                    } else {
                        Ts_State(st, 5, 1);
                        Ts_State(st, 4, 2);
                    }
                } else
                    plain = true;
            } else if (flags & 0x80000) {
                Ts_State(st, 2, 2);
                Ts_State(st, 3, 1);
                Ts_State(st, 1, 22);
                Ts_State(st, 5, 1);
                Ts_State(st, 4, 2);
                double a = F(double, self, 8488) * g_ts64fa38;
                double s = x87_sin(a);
                float m10 = (float)(s * g_ts651750);
                float m00 = (float)(x87_cos(a) * g_ts651750);
                float m01 = (float)(s * g_ts651748);
                Ts_State(st, 7, Ts_Bits(m00));
                Ts_State(st, 8, Ts_Bits(m10));
                Ts_State(st, 9, Ts_Bits(m01));
                Ts_State(st, 10, Ts_Bits(m00));
            } else if (g_tsMulti7208d4) {
                Ts_State(st, 2, 2);
                Ts_State(st, 3, 1);
                if (g_ts702e84)
                    Ts_State(st, 1, 16);
                else if (!((flags & 0x2000) && g_ts7208cc) && F(int, F(BYTE*, m, 76), 64) == 5)
                    Ts_State(st, 1, 13);
                else
                    Ts_State(st, 1, 14);
                if (F(int, m, 64) == 3)
                    Ts_State(st, 1, 2);
                if (g_ts7208c8) {
                    Ts_State(st, 6, g_ts702e84 == 0);
                    Ts_State(st, 4, 3);
                } else {
                    Ts_State(st, 5, g_ts702e84 == 0);
                    Ts_State(st, 4, 2);
                }
            } else
                plain = true;
            if (plain) {
                Ts_State(st, 2, 2);
                Ts_State(st, 1, 2);
                Ts_State(st, 5, F(int, F(BYTE*, m, 76), 64) == 5 ? 2 : 3);
                Ts_State(st, 4, 2);
            }
        }
        tex = F(BYTE*, m, 76);
        Ts_State(st, 12, !(F(DWORD, tex, 52) & 0x100) && !(F(DWORD, m, 32) & 0x200) ? 1 : 3);
        if ((F(BYTE, F(BYTE*, m, 76), 52) & 8) && !(g_tsCaps702e8c & 0x4000000)) {
            Ts_State(st, 16, 1);
            Ts_State(st, 17, 1);
        } else {
            Ts_State(st, 16, 2);
            Ts_State(st, 17, 2);
        }
        Ts_State(st, 18, g_ts7208c0 && F(int, F(BYTE*, m, 76), 68) > 1 ? 3 : 2);
    }
    int next = stage + 1;
    if (!g_tsMulti7208d4) {
        Ts_State(1, 1, 1);
        Ts_State(1, 4, 1);
        return result;
    }
    flags = F(DWORD, m, 32);
    if (!(flags & 0x4000) || (g_tsCaps702e8c & 0x1000000)) {
        if (next > 7)
            return result;
        BYTE* p = prev && F(int, prev, 44) >= 0 ? F(BYTE*, self, 8260) + F(int, prev, 44) * 124 : 0;
        int link = F(int, m, 44);
        if (link >= 0 && !(flags & 0x20000))
            return result | Material_SetStages(self, 0, F(BYTE*, self, 8260) + link * 124, p, next);
    }
    Ts_State(next, 1, 1);
    Ts_State(next, 4, 1);
    return result;
}

} // extern "C"

// === uploading an image to its texture surfaces (FUN_5bb660)

KING_VAR(BYTE*, g_tuDraw72090c, 72090c)       // IDirectDraw
KING_VAR(int, g_tuMips7208c0, 7208c0)
KING_VAR(AsmLabel, g_tuFmt720148, 720148)
KING_VAR(AsmLabel, g_tuFmt720870, 720870)
KING_VAR(AsmLabel, g_tuFmt720128, 720128)
KING_VAR(AsmLabel, g_tuFmt720728, 720728)
KING_VAR(AsmLabel, s_tu67f2d8, 67f2d8)
KING_VAR(AsmLabel, s_tu67f220, 67f220)
KING_VAR(AsmLabel, s_tu67f1e0, 67f1e0)
KING_VAR(AsmLabel, s_tu67f1a0, 67f1a0)
KING_VAR(AsmLabel, s_tu67f184, 67f184)
KING_VAR(AsmLabel, s_tu67f2b4, 67f2b4)
KING_VAR(AsmLabel, s_tu67f29c, 67f29c)
KING_VAR(AsmLabel, s_tu67f280, 67f280)
extern "C" void FUN_5d8350(); KING_ALIAS("_FUN_5d8350", "_Dd_BitDepth")
extern "C" void FUN_5e6bc0(); KING_ALIAS("_FUN_5e6bc0", "@Mmio_ReadAt@16")
extern "C" void FUN_5e6bb0(); KING_ALIAS("_FUN_5e6bb0", "@Mmio_Reset2@8")
extern "C" void FUN_5bb260(); KING_ALIAS("_FUN_5bb260", "@Texture_LoadRows@44")
extern "C" void FUN_5d3af0(); KING_ALIAS("_FUN_5d3af0", "_Video_Get720898")
extern "C" void FUN_5d2e70(); KING_ALIAS("_FUN_5d2e70", "_Sys_Fatal")
extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")

// a COM method (stdcall, this pushed) at vtbl+off of o
template <class R = HRESULT, class... A>
static R Tu_Com(void* o, int off, A... args)
{
    return ((R(__stdcall*)(void*, A...))*(void**)(*(BYTE**)o + off))(o, args...);
}

static void Tu_LogError(HRESULT hr)
{
    CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, 0), 0);
}

extern "C" {

// [FUN_5bb660] uploads the image's pixels (+8, or +36 read from the file +80 for flag 1; a region if given) to
// its texture surface (+72) and, with mipmapping, the following levels (+68 of them, halving the size): by
// locking the texture, or through a system memory surface of the format +64 and a blit
void __fastcall Texture_Upload(BYTE* self, void*, int flags, int* region, int a, int b)
{
    if (!g_device || !F(BYTE*, self, 72))
        return;
    int bpp = CallC<int>(FUN_5d8350);
    int left = 0, top = 0;
    int fromFile = flags & 1;
    BYTE* src = fromFile ? F(BYTE*, self, 36) : F(BYTE*, self, 8);
    int width = F(WORD, self, 20);
    int rows = F(WORD, self, 22);
    int rowBytes = width * bpp;
    if (region) {
        src = (BYTE*)region[5];
        width = region[4];
        rowBytes = region[2];
        rows = region[3];
        left = region[0];
        top = region[1];
    }
    BYTE* surf = F(BYTE*, self, 72);
    if (Tu_Com<HRESULT>(surf, 96) == (HRESULT)0x887601C2)
        Tu_Com<HRESULT>(surf, 108);
    surf = F(BYTE*, self, 72);
    int levelW = F(WORD, self, 20);
    int levelH = F(WORD, self, 22);
    int levels = F(int, self, 68);
    for (;;) {
        DWORD desc[31];
        memset(desc, 0, sizeof desc);
        desc[0] = 124;
        desc[1] = 8387054;
        if (fromFile)
            CallThis(FUN_5e6bc0, F(BYTE*, self, 80), F(BYTE*, self, 32) + (int)src, rows * rowBytes);
        bool done = false;
        if (flags && Tu_Com<HRESULT>(surf, 100, 0, desc, 33, 0) == 0) {
            int pitch = desc[4];
            BYTE* dst = (BYTE*)desc[9] + pitch * top + left * bpp;
            if (dst) {
                CallThis(FUN_5bb260, self, dst, pitch, src, rowBytes, rows, width, a, b, flags);
                Tu_Com<HRESULT>(surf, 128, 0);
            } else
                CallC(FUN_5d2ba0, (const char*)s_tu67f2d8, 0);
            done = true;
        }
        if (!done) {
            DWORD sd[31];
            memset(sd, 0, sizeof sd);
            sd[0] = 124;
            sd[3] = levelW;
            sd[1] = 4103;
            sd[2] = levelH;
            sd[26] = 6144;
            const void* fmt = 0;
            unsigned caps;
            switch (F(int, self, 64)) {
            case 2:
                caps = CallC<unsigned>(FUN_5d3af0);
                if (caps & 2)
                    fmt = g_tuFmt720148;
                else
                    CallC(FUN_5d2e70, F(const char*, self, 4), (const char*)s_tu67f220);
                break;
            case 3:
                caps = CallC<unsigned>(FUN_5d3af0);
                if (caps & 4)
                    fmt = g_tuFmt720870;
                else
                    CallC(FUN_5d2e70, F(const char*, self, 4), (const char*)s_tu67f1a0);
                break;
            case 5:
                caps = CallC<unsigned>(FUN_5d3af0);
                if (caps & 16)
                    fmt = g_tuFmt720128;
                else
                    CallC(FUN_5d2e70, F(const char*, self, 4), (const char*)s_tu67f1e0);
                break;
            case 8:
                caps = CallC<unsigned>(FUN_5d3af0);
                fmt = (caps & 128) ? (const void*)g_tuFmt720728 : (const void*)g_tuFmt720128;
                break;
            default:
                CallC(FUN_5d2e70, F(const char*, self, 4), (const char*)s_tu67f184);
                break;
            }
            if (fmt)
                memcpy(sd + 18, fmt, 32);
            BYTE* dd = g_tuDraw72090c;
            BYTE* tmp;
            HRESULT hr = Tu_Com<HRESULT>(dd, 24, sd, &tmp, 0);
            if (hr)
                Tu_LogError(hr);
            else if ((hr = Tu_Com<HRESULT>(tmp, 100, 0, desc, 33, 0)) != 0) {
                CallC(FUN_5d2ba0, (const char*)s_tu67f29c, 0);
                Tu_LogError(hr);
            } else {
                int pitch = desc[4];
                BYTE* dst = (BYTE*)desc[9] + pitch * top + left * bpp;
                if (dst)
                    CallThis(FUN_5bb260, self, dst, pitch, src, rowBytes, rows, width, a, b, flags);
                Tu_Com<HRESULT>(tmp, 128, 0);
                DWORD fx[25];
                memset(fx, 0, sizeof fx);
                fx[0] = 100;
                fx[2] = 13369376;
                int right = rowBytes / bpp + left;
                RECT dr = {left, top, right, top + rows};
                RECT sr = dr;
                hr = Tu_Com<HRESULT>(surf, 20, &dr, tmp, &sr, 16777216, fx);
                if (hr) {
                    CallC(FUN_5d2ba0, (const char*)s_tu67f2b4, 0);
                    Tu_LogError(hr);
                }
                Tu_Com<ULONG>(tmp, 8);
            }
        }
        if (fromFile)
            CallThis(FUN_5e6bb0, F(BYTE*, self, 80));
        if (levels <= 1 || !g_tuMips7208c0)
            return;
        if (surf == F(BYTE*, self, 72))
            src = flags ? F(BYTE*, self, 40) : F(BYTE*, self, 12);
        else
            src += levelH * levelW * bpp;
        DWORD scaps[4] = {0x401000, 0, 0, 0};    // DDSCAPS_TEXTURE | DDSCAPS_MIPMAP
        BYTE* next = surf;
        HRESULT hr = Tu_Com<HRESULT>(surf, 48, scaps, &next);
        surf = next;
        if (hr || !surf) {
            CallC(FUN_5d2ba0, (const char*)s_tu67f280, levelW);
            Tu_LogError(hr);
            return;
        }
        Tu_Com<ULONG>(surf, 8);
        levelW /= 2;
        levelH /= 2;
        rowBytes = levelW * bpp;
        width = levelW;
        rows = levelH;
        levels--;
        if (!surf)
            return;
    }
}

} // extern "C"

// === blitting a part of an image (FUN_5b81c0)

KING_VAR(double, g_blOne64b670, 64b670)
KING_VAR(double, g_blZero64b678, 64b678)
extern "C" void FUN_5b64c0(); KING_ALIAS("_FUN_5b64c0", "@Window_RootSurface@8")
extern "C" void FUN_5d79a0(); KING_ALIAS("_FUN_5d79a0", "_Dd_Blit7")
ASM_PROC(FUN_5fe814)
ASM_PROC(FUN_5fe837)
ASM_PROC(FUN_5fe860)
ASM_PROC(FUN_5fe8d4)
ASM_PROC(FUN_5fea03)

static __declspec(noinline) int Bl_Int(double d)
{
    return (int)d;
}

extern "C" {

// [FUN_5b81c0] draws the part r (left, top, right, bottom) of image img on surface dst at (x, y), clipped to
// both: by DirectDraw when both are on video surfaces, else in software by the surface's mode (+80): copy,
// 2 a palette-table blend by the factor +88, 3 an alpha-indexed blend, 4 a color-keyed copy
void __cdecl Image_DrawPart(BYTE* dst, BYTE* img, int* r, int x, int y)
{
    BYTE* pal = CallThis<BYTE*>(FUN_5b64c0, dst);
    if (!(F(BYTE, img, 52) & 1))
        return;
    int left = r[0], top = r[1], right = r[2], bottom = r[3];
    if (x < 0) {
        left -= x;
        x = 0;
    }
    if (y < 0) {
        top -= y;
        y = 0;
    }
    int W = F(WORD, dst, 0), H = F(WORD, dst, 2);
    if (x >= W || y >= H)
        return;
    if (right - left + x >= W)
        right = W - x + left - 1;
    if (bottom - top + y >= H)
        bottom = H - y + top - 1;
    int iw = F(WORD, img, 20);
    if (left >= iw)
        return;
    if (left < 0) {
        x -= left;
        left = 0;
    }
    if (left > right || right < 0)
        return;
    if (right >= iw)
        right = iw - 1;
    int ih = F(WORD, img, 22);
    if (top >= ih)
        return;
    if (top < 0) {
        y -= top;
        top = 0;
    }
    if (bottom < 0 || top > bottom)
        return;
    if (bottom >= ih)
        bottom = ih - 1;
    int cols = right - left + 1;
    if (F(BYTE, img, 52) & 2) {
        if (F(BYTE, dst, 13) & 8) {
            CallC(FUN_5d79a0, left, top, x, y, cols, bottom - top + 1, ih, F(void*, img, 72));
            return;
        }
        BYTE* par = F(BYTE*, dst, 96);
        if (par && (F(BYTE, par, 13) & 8)) {
            CallC(FUN_5d79a0, left, top, x + F(int, dst, 100), y + F(int, dst, 104), cols, bottom - top + 1, ih,
                  F(void*, img, 72));
            return;
        }
    }
    BYTE* bits = F(BYTE*, dst, 24 + F(int, dst, 16) * 4);
    if (!bits)
        return;
    BYTE* dp = bits + F(WORD, dst, 4) * x + F(WORD, dst, 6) * y;
    BYTE* sp = F(BYTE*, img, 8) + (iw * top + left) * F(WORD, img, 24);
    int mode = F(int, dst, 80);
    int srcStep = F(WORD, img, 24) * F(WORD, img, 20);
    if (mode == 0 || (mode == 2 && !(F(float, dst, 88) < g_blOne64b670 || F(float, dst, 88) > g_blOne64b670))) {
        for (int n = bottom - top + 1; n; n--) {
            memcpy(dp, sp, F(WORD, dst, 4) * cols);
            sp += srcStep;
            dp += F(WORD, dst, 6);
        }
        return;
    }
    if (mode == 2 || mode == 4) {
        if (!pal)
            return;
        if (mode == 2 && !(F(float, dst, 88) < g_blZero64b678 || F(float, dst, 88) > g_blZero64b678))
            return;
        BYTE* tbl = F(BYTE**, pal, 0)[Bl_Int(F(int, pal, 796) * (double)F(float, dst, 88))];
        if (top > bottom)
            return;
        for (int n = bottom - top + 1; n; n--) {
            WORD bpp = F(WORD, dst, 4);
            if (mode == 2) {
                if (bpp == 1)
                    CallC(FUN_5fe860, tbl, dp, sp, cols);
                else if (bpp == 2)
                    CallC(FUN_5fea03, tbl, dp, sp, cols);
            } else if (!(F(float, dst, 88) < g_blZero64b678 || F(float, dst, 88) > g_blZero64b678)) {
                if (bpp == 1)
                    CallC(FUN_5fe814, dp, sp, cols);
                else if (bpp == 2)
                    CallC(FUN_5fe837, dp, sp, cols);
            } else {
                if (bpp == 1)
                    CallC(FUN_5fe8d4, tbl, dp, sp, cols, (int)F(BYTE, dst, 92));
                else if (bpp == 2)
                    CallC(FUN_5fe837, dp, sp, cols);
            }
            sp += F(WORD, img, 24) * F(WORD, img, 20);
            dp += F(WORD, dst, 6);
        }
        return;
    }
    if (mode != 3)
        return;
    int div = 256 / F(int, pal, 796);
    if (!pal || !(F(float, dst, 88) < g_blZero64b678 || F(float, dst, 88) > g_blZero64b678))
        return;
    if (top > bottom)
        return;
    for (int n = bottom - top + 1; n; n--) {
        for (int c = 0; c < cols; c++) {
            BYTE s = sp[c];
            if (!s)
                continue;
            if (s == 255)
                dp[c] = F(BYTE, dst, 92);
            else
                dp[c] = F(BYTE**, pal, 0)[s / div][F(BYTE, dst, 92) * 256 + dp[c]];
        }
        sp += F(WORD, img, 20);
        dp += F(WORD, dst, 6);
    }
}

} // extern "C"

// === tiling an image over a rectangle (FUN_5b9020)

KING_VAR(int, g_tlHw720904, 720904)
extern "C" void FUN_5b94b0(); KING_ALIAS("_FUN_5b94b0", "@Rows_Get@12")

extern "C" {

// [FUN_5b9020] tiles image img over rectangle r of surface dst, centered (the leftover split on both sides),
// clipped: through Image_DrawPart with a hardware surface (720904), else row by row in software (mode +80 2
// through the palette blend table)
void __cdecl Image_Tile(BYTE* dst, int* r, BYTE* img)
{
    if (!img)
        return;
    int offX = 0, offY = 0;
    BYTE* pal = CallThis<BYTE*>(FUN_5b64c0, dst);
    int left = r[0], top = r[1], right = r[2], bottom = r[3];
    int iw = F(WORD, img, 20), ih = F(WORD, img, 22);
    int rest = right - left;
    if (rest > 0)
        do
            rest -= iw;
        while (rest > 0);
    int phaseX = rest / 2;
    rest = bottom - top;
    if (rest > 0)
        do
            rest -= ih;
        while (rest > 0);
    int phaseY = rest / 2;
    if (!(F(BYTE, img, 52) & 1))
        return;
    if (left > right || top > bottom)
        return;
    int W = F(WORD, dst, 0), H = F(WORD, dst, 2);
    if (left >= W || top >= H || right < 0 || bottom < 0)
        return;
    if (right >= W)
        right = W - 1;
    if (bottom >= H)
        bottom = H - 1;
    int x0 = left, y0 = top;
    if (left < 0) {
        offX = left;
        x0 = 0;
    }
    if (top < 0) {
        offY = top;
        y0 = 0;
    }
    int bpp = F(WORD, img, 24);
    BYTE* src = F(BYTE*, img, 8) - (iw * phaseY + phaseX) * bpp;
    int rowBytes = bpp * iw;
    if (g_tlHw720904 == 0) {
        BYTE* bits = F(BYTE*, dst, 24 + F(int, dst, 16) * 4);
        if (!bits)
            return;
        BYTE* dp = bits + F(WORD, dst, 4) * x0 + F(WORD, dst, 6) * y0;
        BYTE* tbl = pal;
        if (F(int, dst, 80) == 2) {
            if (!pal || !(F(float, dst, 88) < g_blZero64b678 || F(float, dst, 88) > g_blZero64b678))
                return;
            tbl = CallThis<BYTE*>(FUN_5b94b0, pal, Bl_Int(F(int, pal, 796) * (double)F(float, dst, 88)));
        }
        if (y0 > bottom)
            return;
        BYTE* sp = src;
        for (int y = y0; y <= bottom;) {
            int phase = phaseX;
            int count = bpp * phaseX + rowBytes;
            BYTE* d = dp;
            for (int x = x0; x <= right;) {
                int next = x + count;
                if (next <= right + 1) {
                    if (F(int, dst, 80) == 2) {
                        if (bpp == 1)
                            CallC(FUN_5fe860, tbl, d, sp, count);
                        else if (bpp == 2)
                            CallC(FUN_5fea03, tbl, d, sp, count / 2);
                    } else
                        memcpy(d, sp, count);
                } else {
                    int n = right - x + 1;
                    if (F(int, dst, 80) == 2) {
                        if (bpp == 1)
                            CallC(FUN_5fe860, tbl, d, sp, n);
                        else if (bpp == 2)
                            CallC(FUN_5fea03, tbl, d, sp, n);
                    } else
                        memcpy(d, sp, n * F(WORD, dst, 4));
                }
                d += count;
                sp += bpp * phase;
                count -= bpp * phase;
                phase = 0;
                x = next;
            }
            y++;
            if ((y - y0 - phaseY) % ih == 0)
                sp = F(BYTE*, img, 8) - bpp * phaseX;
            else
                sp += (iw - phaseX) * bpp;
            dp += F(WORD, dst, 6);
        }
        return;
    }
    if (y0 >= bottom)
        return;
    int rc[4] = {0, 0, 0, 0};
    int dyOff = offY;
    for (int y = y0; y < bottom;) {
        int dxOff = offX;
        if (x0 < right) {
            int topOff = -dyOff;
            for (int x = x0; x < right;) {
                rc[0] = -dxOff;
                rc[2] = iw;
                rc[1] = topOff;
                rc[3] = ih;
                if (iw - rc[0] + x > right)
                    rc[2] = rc[0] - x + right;
                if (ih - topOff + y > bottom)
                    rc[3] = topOff - y + bottom;
                Image_DrawPart(dst, img, rc, x, y);
                x += rc[2] - rc[0];
                dxOff = 0;
            }
        }
        y += rc[3] - rc[1];
        dyOff = 0;
    }
}

} // extern "C"

// === setting the camera matrices (FUN_5ae0e0)

KING_VAR(BYTE, g_cm702e08, 702e08)
KING_VAR(AsmLabel, g_cm7029c8, 7029c8)
KING_VAR(AsmLabel, g_cm702998, 702998)
KING_VAR(AsmLabel, g_cm7025f0, 7025f0)
ASM_VAR(AsmLabel, g_cm5ae540, $L_5ae540)
ASM_VAR(AsmLabel, g_cm5ae530, $L_5ae530)
KING_VAR(double, g_cm64b5a0, 64b5a0)
KING_VAR(double, g_cm64b678, 64b678)
extern "C" void FUN_5adea0(); KING_ALIAS("_FUN_5adea0", "@Render_SetDeviceView@4")
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
extern "C" void FUN_5abe90(); KING_ALIAS("_FUN_5abe90", "_Matrix_Scale")
extern "C" void FUN_5abee0(); KING_ALIAS("_FUN_5abee0", "_Matrix_RotationX")
extern "C" void FUN_5ac480(); KING_ALIAS("_FUN_5ac480", "_Matrix_InverseTo")
extern "C" void FUN_5ac5f0(); KING_ALIAS("_FUN_5ac5f0", "_Matrix_TransposeTo")
extern "C" int __cdecl atexit(void (__cdecl*)(void));

struct Cm_V3 { float x, y, z; };

extern "C" {

// [FUN_5ae0e0] sets renderer s's camera m and projection proj (scaled by 1 / +8464 in software): the view
// matrices and their inverses are pushed on the stacks at +2288, +3248, +4304, +5264 with the clip planes
// (+1952..), the pixel scale +1752 and the fog factor +8528; a hardware device gets the view transform
void __fastcall Render_SetCamera(BYTE* s, void*, const BYTE* m, const BYTE* proj)
{
    BYTE* A = (BYTE*)g_cm7029c8;
    BYTE* B = (BYTE*)g_cm702998;
    if (!(g_cm702e08 & 1)) {
        g_cm702e08 |= 1;
        memcpy(A, (BYTE*)g_cm7025f0, 48);
        atexit((void (__cdecl*)(void))(void*)g_cm5ae540);
    }
    if (!(g_cm702e08 & 2)) {
        g_cm702e08 |= 2;
        memcpy(B, (BYTE*)g_cm7025f0, 48);
        atexit((void (__cdecl*)(void))(void*)g_cm5ae530);
    }
    memcpy(s + 4208, m, 48);
    CallC(FUN_5ac480, s + 4256, s + 4208);
    F(int, s, 8224) = 0;
    F(int, s, 8228) = 0;
    F(int, s, 8232) = 0;
    F(int, s, 8236) = 0;
    const BYTE* p = proj;
    BYTE sc[48], ps[48];
    if (!g_device) {
        float f = (float)(g_one64b670 / F(double, s, 8464));
        Cm_V3 v = {f, f, f};
        p = CallC<BYTE*>(FUN_5aba40, ps, proj, CallC<BYTE*>(FUN_5abe90, sc, v));
    }
    memcpy(B, p, 48);
    BYTE t1[48], t2[48];
    memcpy(s + 2288 + F(int, s, 8224) * 48, CallC<BYTE*>(FUN_5aba40, t1, m, B), 48);
    CallC(FUN_5ac480, A, CallC<BYTE*>(FUN_5aba40, t2, m, B));
    CallC(FUN_5ac5f0, s + 3248 + F(int, s, 8224) * 48, A);
    F(int, s, 8224)++;
    memcpy(s + 4304 + F(int, s, 8228) * 48, B, 48);
    F(int, s, 8228)++;
    memcpy(s + 5264 + F(int, s, 8236) * 48, (BYTE*)g_cm7025f0, 48);
    F(int, s, 8236)++;
    int k = F(int, s, 8232);
    F(int, s, 1968 + k * 4) = F(int, s, 1952);
    F(int, s, 2048 + k * 4) = F(int, s, 1956);
    F(int, s, 2128 + k * 4) = F(int, s, 1960);
    F(int, s, 2208 + k * 4) = F(int, s, 1964);
    F(float, s, 1752) = (float)((F(double, s, 8520) + F(double, s, 8520)) /
                                ((double)(F(int, s, 1960) >> 19) * F(float, B, 0)));
    double fog = F(int, s, 8456) * g_cm64b5a0;
    F(double, s, 8528) = fog;
    if (fog > g_one64b670)
        F(double, s, 8528) = 1.0;
    if (F(double, s, 8528) < g_cm64b678)
        F(double, s, 8528) = 0.0;
    F(int, s, 8232)++;
    F(int, s, 8444) = ++g_transformSerial;
    CallThis(FUN_5adea0, s);
    if (!g_tlHw720904)
        return;
    Cm_V3 v = {1.0f, 1.0f, -1.0f};
    BYTE sm[48], rx[48], t3[48], r[48];
    BYTE* scale = CallC<BYTE*>(FUN_5abe90, sm, v);
    BYTE* rot = CallC<BYTE*>(FUN_5abee0, rx, -1.5707963f);
    CallC(FUN_5aba40, r, CallC<BYTE*>(FUN_5aba40, t3, m, rot), scale);
    const float* R = (const float*)r;
    float d[16] = {R[0], R[1], R[2], 0, R[3], R[4], R[5], 0, R[6], R[7], R[8], 0, R[9], R[10], R[11], 1.0f};
    Com(g_device, 44, 2, d);
}

} // extern "C"

// === creating a texture's surface (FUN_5bae30)

KING_VAR(int, g_tc7208c0, 7208c0)
KING_VAR(int, g_tc7208dc, 7208dc)
KING_VAR(const char*, g_tc702e9c, 702e9c)
KING_VAR(AsmLabel, g_tc720148, 720148)
KING_VAR(AsmLabel, g_tc720128, 720128)
KING_VAR(AsmLabel, g_tc720870, 720870)
KING_VAR(AsmLabel, g_tc720728, 720728)
KING_VAR(AsmLabel, s_tc67f260, 67f260)
KING_VAR(AsmLabel, s_tc67f220, 67f220)
KING_VAR(AsmLabel, s_tc67f1e0, 67f1e0)
KING_VAR(AsmLabel, s_tc67f1a0, 67f1a0)
KING_VAR(AsmLabel, s_tc67f184, 67f184)
KING_VAR(AsmLabel, s_tc67f164, 67f164)
KING_VAR(AsmLabel, s_tc67f150, 67f150)
KING_VAR(AsmLabel, s_tc67f140, 67f140)
KING_VAR(AsmLabel, s_tc67f128, 67f128)
KING_VAR(AsmLabel, s_tc67f124, 67f124)
extern "C" void FUN_5ba8d0(); KING_ALIAS("_FUN_5ba8d0", "_TexPool_Get")
extern "C" void FUN_5d2e70(); KING_ALIAS("_FUN_5d2e70", "_Sys_Fatal")
extern "C" void FUN_5d3af0(); KING_ALIAS("_FUN_5d3af0", "_Video_Get720898")

static void Tc_Fatal(const char* a, const char* b)
{
    CallC(FUN_5d2e70, a, b);
}

// a DirectDraw failure hr creating texture t: the message fmt with its name (and arg) in a fatal error
static int Tc_Fail(int hr, const char* fmt, const char* name, const char* arg)
{
    char buf[256];
    sprintf(buf, fmt, name, arg);
    Tc_Fatal((const char*)s_tc67f140, CallC<const char*>(FUN_5d4c10, hr, buf));
    return 0;
}

extern "C" {

// [FUN_5bae30] creates texture t's surface once (+52 bit 2): a Direct3D texture of the image's pixel format
// (managed with 7208c0, mipmaps +68; small or flagged +26 images, unless flags bit 2), a hardware surface
// (720904 and flags bit 2), else a software surface (+44); the pixels uploaded; 0 on failure
int __fastcall Texture_Create(BYTE* t, void*, unsigned flags)
{
    if (F(BYTE, t, 52) & 2)
        return 1;
    F(int, t, 76) = 0;
    bool small = F(WORD, t, 26) != 0 || (F(WORD, t, 20) == 1 && F(WORD, t, 22) == 1);
    if (g_device && !(flags & 2) && small) {
        CallC(FUN_5d8350);
        DWORD d[31];
        memset(d, 0, sizeof d);
        d[0] = 124;
        d[1] = (g_tc7208c0 ? 0x20000 : 0) | 0x1007;
        switch (g_tc7208dc) {
        case 1:
            d[26] = 0x1800;
            break;
        case 2:
        case 3:
            d[26] = 0x5000;
            break;
        default:
            Tc_Fatal((const char*)s_tc67f260, 0);
        }
        if (g_tc7208c0) {
            d[6] = F(DWORD, t, 68);
            d[26] |= 0x400008;
        }
        d[3] = F(WORD, t, 20);
        d[2] = F(WORD, t, 22);
        const char* name = F(const char*, t, 4);
        int caps = CallC<int>(FUN_5d3af0);
        switch (F(int, t, 64)) {
        case 2:
            if (caps & 2)
                memcpy(d + 18, (BYTE*)g_tc720148, 32);
            else
                Tc_Fatal(name, (const char*)s_tc67f220);
            break;
        case 3:
            if (caps & 4)
                memcpy(d + 18, (BYTE*)g_tc720870, 32);
            else
                Tc_Fatal(name, (const char*)s_tc67f1a0);
            break;
        case 5:
            if (caps & 16)
                memcpy(d + 18, (BYTE*)g_tc720128, 32);
            else
                Tc_Fatal(name, (const char*)s_tc67f1e0);
            break;
        case 8:
            memcpy(d + 18, caps & 128 ? (BYTE*)g_tc720728 : (BYTE*)g_tc720128, 32);
            break;
        default:
            Tc_Fatal(name, (const char*)s_tc67f184);
        }
        BYTE* surf;
        int hr = CallC<int>(FUN_5ba8d0, d, &surf);
        if (hr)
            return Tc_Fail(hr, (const char*)s_tc67f150, F(const char*, t, 4), g_tc702e9c);
        if (!surf)
            Tc_Fatal((const char*)s_tc67f164, 0);
        if (F(BYTE, t, 52) & 8) {
            DWORD key[2] = {0, 0};
            Com(surf, 116, 8, key);
        } else
            Com(surf, 116, 8, (DWORD*)0);
        F(BYTE*, t, 72) = surf;
        Texture_Upload(t, 0, flags & 1, 0, 0, 0x3ff00000);
        F(BYTE*, t, 76) = F(BYTE*, t, 72);
        Com(F(BYTE*, t, 72), 4);
        F(DWORD, t, 52) |= 2;
        return 1;
    }
    if (g_tlHw720904 && (flags & 2)) {
        CallC(FUN_5d8350);
        DWORD d[31];
        memset(d, 0, sizeof d);
        d[0] = 124;
        d[3] = F(WORD, t, 20);
        d[26] = (F(int, t, 56) ? 0x10004000 : 0) + 0x800;
        d[1] = 7;
        d[2] = F(WORD, t, 22);
        BYTE* surf;
        int hr = Com(g_rnD3d, 24, d, &surf, (void*)0);
        if (hr)
            return Tc_Fail(hr, (const char*)s_tc67f124, F(const char*, t, 4), 0);
        if (!surf)
            Tc_Fatal((const char*)s_tc67f128, 0);
        F(BYTE*, t, 72) = surf;
        Texture_Upload(t, 0, flags & 1, 0, 0, 0x3ff00000);
        F(DWORD, t, 52) |= 2;
        return 1;
    }
    if (F(BYTE*, t, 44))
        Com_Release(F(BYTE*, t, 44));
    BYTE* s = CallC<BYTE*>(FUN_5d7750, (int)F(WORD, t, 20), (int)F(WORD, t, 22), F(void*, t, 8));
    F(BYTE*, t, 44) = s;
    F(BYTE*, t, 72) = s;
    if (!s)
        return 0;
    F(DWORD, t, 52) |= 2;
    return 1;
}

} // extern "C"

// === filling a rectangle (FUN_5b6f90)

KING_VAR(float, g_fr64c458, 64c458)
extern "C" void FUN_5b6710(); KING_ALIAS("_FUN_5b6710", "@Window_GetRect@12")
extern "C" void FUN_5ba3f0(); KING_ALIAS("_FUN_5ba3f0", "@Palette_Color@12")
ASM_PROC(FUN_5fe9b3)
ASM_PROC(FUN_5fea6c)
ASM_PROC(FUN_5feace)
ASM_PROC(FUN_5feafa)
ASM_PROC(FUN_5feb12)

extern "C" {

// [FUN_5b6f90] fills rectangle r of surface s with color (a palette index unless raw), clipped: blended by
// +88 in mode 2 (+80), else a Direct3D clear on a hardware surface (+116 = 1) or filled row by row
void __cdecl Surface_FillRect(BYTE* s, const int* r, DWORD color, int raw)
{
    BYTE* pal = CallThis<BYTE*>(FUN_5b64c0, s);
    int x0 = r[0], y0 = r[1], x1 = r[2], y1 = r[3];
    int W = F(WORD, s, 0), H = F(WORD, s, 2);
    if (x0 > x1 || y0 > y1 || x1 < 0 || y1 < 0 || x0 >= W || y0 >= H)
        return;
    if (x0 < 0)
        x0 = 0;
    if (y0 < 0)
        y0 = 0;
    if (x1 >= W)
        x1 = W - 1;
    if (y1 >= H)
        y1 = H - 1;
    int mode = F(int, s, 80);
    if (mode != 0) {
        if (mode != 2)
            return;
        if (!(F(float, s, 88) == g_one64b670)) {
            BYTE* bits = F(BYTE*, s, 24 + F(int, s, 16) * 4);
            if (!bits)
                return;
            BYTE* row = bits + F(WORD, s, 6) * y0 + F(WORD, s, 4) * x0;
            if (!pal || F(float, s, 88) == g_cm64b678)
                return;
            BYTE* tbl = CallThis<BYTE*>(FUN_5b94b0, pal, (int)(F(int, pal, 796) * (double)F(float, s, 88)));
            // the blend count is x0, as in the original
            for (int y = y0; y <= y1; y++, row += F(WORD, s, 6)) {
                switch (F(WORD, s, 4)) {
                case 1:
                    CallC(FUN_5fe9b3, tbl, row, color, x0);
                    break;
                case 2:
                    CallC(FUN_5fea6c, tbl, row, color, x0);
                    break;
                case 4: {
                    DWORD c = CallThis<DWORD>(FUN_5ba3f0, pal, color);
                    CallC(FUN_5feb12, (int)((double)F(float, s, 88) * g_fr64c458), row, c, x0);
                    break;
                }
                }
            }
            return;
        }
    }
    if (g_device && F(int, s, 116) == 1) {
        DWORD c = color;
        if (!raw) {
            if (!pal)
                return;
            const BYTE* e = pal + 24 + color * 3;
            c = 0xff000000 | e[0] << 16 | e[1] << 8 | e[2];
        }
        int tmp[4];
        int ox = CallThis<int*>(FUN_5b6710, s, tmp)[0];
        int oy = CallThis<int*>(FUN_5b6710, s, tmp)[1];
        int rc[4] = {x0 + ox, y0 + oy, x1 + ox, y1 + oy};
        Com(g_device, 40, 1, rc, 1, c, 1.0f, 0);
        return;
    }
    BYTE* bits = F(BYTE*, s, 24 + F(int, s, 16) * 4);
    if (!bits)
        return;
    BYTE* row = bits + F(WORD, s, 4) * x0 + F(WORD, s, 6) * y0;
    int n = x1 - x0 + 1;
    switch (F(WORD, s, 4)) {
    case 1:
        for (int y = y0; y <= y1; y++, row += F(WORD, s, 6))
            memset(row, (BYTE)color, n);
        break;
    case 2:
    case 4:
        if (!pal)
            return;
        if (!raw)
            color = CallThis<DWORD>(FUN_5ba3f0, pal, color);
        for (int y = y0; y <= y1; y++, row += F(WORD, s, 6))
            CallC(F(WORD, s, 4) == 2 ? FUN_5feace : FUN_5feafa, row, color, n);
        break;
    }
}

} // extern "C"

// === drawing an RLE image (FUN_5b8980) and marking a pixel ($L_5b8fb0)

KING_VAR(AsmLabel, s_gd67f104, 67f104)
extern "C" void FUN_5b8720(); KING_ALIAS("_FUN_5b8720", "_Surface_DrawRle")
ASM_PROC(FUN_5fe887)
ASM_PROC(FUN_5fe906)
ASM_PROC(FUN_5fe943)

extern "C" {

// [FUN_5b8980] draws image img at (x, y) on surface s: blitted from its surface (+36) on a hardware surface,
// else its RLE rows (+16: a byte n | 0x80 skips n pixels, n copies the next n) clipped and written by the
// surface mode (+80: 2 blended by +88, 4 and 3 special blends, 16-bit and z-tested variants with flags)
void __cdecl Gfx_DrawImage(BYTE* s, int x, int y, BYTE* img, int flags)
{
    BYTE* pal = CallThis<BYTE*>(FUN_5b64c0, s);
    bool z = (g_702e8c & 0x4000) && flags > 0;
    if (!img || !(F(BYTE, img, 28) & 1))
        return;
    int iw = F(WORD, img, 4), ih = F(WORD, img, 6);
    if (F(int, s, 116) == 1) {
        DWORD fx[25] = {100};
        fx[2] = 0xcc0020;
        int d[4], r[4];
        d[0] = x >= 0 ? x : 0;
        r[0] = x >= 0 ? 0 : -x;
        d[2] = iw + x;
        if (d[2] < 0)
            return;
        if (d[2] >= F(WORD, s, 0))
            d[2] = F(WORD, s, 0);
        r[2] = d[2] - d[0] + r[0];
        d[1] = y >= 0 ? y : 0;
        r[1] = y >= 0 ? 0 : -y;
        d[3] = y + ih;
        if (d[3] < 0)
            return;
        if (d[3] >= F(WORD, s, 2))
            d[3] = F(WORD, s, 2);
        r[3] = d[3] - d[1] + r[1];
        if (d[1] > d[3] || d[0] > d[2])
            return;
        if (F(int, s, 96)) {
            d[0] += F(int, s, 100);
            d[2] += F(int, s, 100);
            d[1] += F(int, s, 104);
            d[3] += F(int, s, 104);
        }
        BYTE* dd = (BYTE*)g_q150_7208fc;
        if (int hr = Com(dd, 20, d, F(BYTE*, img, 36), r, 0x1008000, fx))
            CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, (const char*)s_gd67f104), 0);
        return;
    }
    const BYTE* rle = F(const BYTE*, img, 16);
    if (!rle)
        return;
    if (x >= 0 && y >= 0 && x + iw <= F(WORD, s, 0) && y + ih <= F(WORD, s, 2) && !(g_702e8c & 0x4000) && F(WORD, s, 4) == 1 &&
        F(int, s, 80) != 4 && F(int, s, 80) != 3) {
        CallC(FUN_5b8720, s, x, y, img);
        return;
    }
    int mode = 0;
    BYTE* tbl = 0;
    DWORD pal0 = 0;
    int key = 0;
    switch (F(int, s, 80)) {
    case 2:
        if (!pal || F(float, s, 88) == g_cm64b678)
            return;
        if (F(float, s, 88) != g_one64b670) {
            tbl = CallThis<BYTE*>(FUN_5b94b0, pal, (int)(F(int, pal, 796) * (double)F(float, s, 88)));
            mode = 1;
        }
        break;
    case 4:
        mode = 8;
        break;
    case 3:
        pal0 = F(DWORD, pal, 0);
        mode = 16;
        tbl = (BYTE*)(256 / F(int, pal, 796));
        key = F(BYTE, s, 92);
        break;
    }
    int bpp = F(WORD, s, 4);
    if (bpp == 2)
        mode |= 2;
    WORD* zrow = 0;
    if (z) {
        mode |= 4;
        zrow = F(WORD*, s, 48) + F(WORD, s, 8) * y + x;
    }
    BYTE* bits = F(BYTE*, s, 24 + F(int, s, 16) * 4);
    if (!bits)
        return;
    BYTE* row = bits + bpp * x + F(WORD, s, 6) * y;
    int sb = F(WORD, img, 8);
    for (int cy = y; cy - y < ih; cy++, row += F(WORD, s, 6)) {
        WORD* zp = zrow;
        BYTE* dst = row;
        int cx = x;
        while (cx - x < iw) {
            int n = *rle & 127;
            if (*rle++ & 128) {
                dst += bpp * n;
                cx += n;
                if (z)
                    zp += n;
                continue;
            }
            int W = F(WORD, s, 0);
            if (cy >= 0 && cy < F(WORD, s, 2) && cx < W && cx + n >= 0) {
                BYTE* d = dst;
                const BYTE* src = rle;
                WORD* zz = zp;
                int c = n, from = cx;
                if (from < 0) {
                    d -= bpp * from;
                    src -= sb * from;
                    if (z)
                        zz -= from;
                    c = n + from;
                    from = 0;
                }
                if (c + from > W)
                    c = W - from;
                switch (mode) {
                case 0:
                    memcpy(d, src, c);
                    break;
                case 1:
                    CallC(FUN_5fe860, tbl, d, src, c);
                    break;
                case 2:
                    memcpy(d, src, c * 2);
                    break;
                case 3:
                    CallC(FUN_5fea03, tbl, d, src, c);
                    break;
                case 4:
                    CallC(FUN_5fe943, d, src, c, zz, flags);
                    break;
                case 5:
                    CallC(FUN_5fe906, tbl, d, src, c, zz, flags);
                    break;
                case 8:
                case 12:
                    CallC(FUN_5fe814, d, src, c);
                    break;
                case 10:
                case 14:
                    CallC(FUN_5fe837, d, src, c);
                    break;
                case 16:
                    CallC(FUN_5fe887, pal0, d, src, c, key, tbl);
                    break;
                }
            }
            rle += sb * n;
            dst += bpp * n;
            if (z)
                zp += n;
            cx += n;
        }
        if (z)
            zrow += F(WORD, s, 8);
    }
}

// $L_5b8fb0 (inside FUN_5b8980): sets the high nibble of the second byte of pixel (x, y) of surface s, when image img is set
void __cdecl Gfx_MarkPixel(BYTE* s, int x, int y, BYTE* img)
{
    if (!img || !(F(BYTE, img, 28) & 1))
        return;
    BYTE* bits = F(BYTE*, s, 24 + F(int, s, 16) * 4);
    if (!bits || y >= F(WORD, s, 2) || x >= F(WORD, s, 0) || y < 0 || x < 0)
        return;
    bits[F(WORD, s, 4) * x + F(WORD, s, 6) * y + 1] |= 0xf0;
}

} // extern "C"

// === probing the device's multitexturing (FUN_5b04c0)

KING_VAR(int, g_mt7208bc, 7208bc)
KING_VAR(int, g_mt7208c4, 7208c4)
KING_VAR(int, g_mt7208c8, 7208c8)
KING_VAR(int, g_mt7208cc, 7208cc)
KING_VAR(int, g_mt7208d0, 7208d0)
KING_VAR(int, g_mt7208d4, 7208d4)
extern "C" void FUN_4cca90(); KING_ALIAS("_FUN_4cca90", "_Fx_IsRadeon")
extern "C" void FUN_5b0120(); KING_ALIAS("_FUN_5b0120", "_Device_SetStates")

static void Mt_Stage(int stage, int type, int value) { Com(g_device, 148, stage, type, value); }

// whether the device can draw with the stages as set
static bool Mt_Valid()
{
    DWORD passes;
    return Com(g_device, 152, &passes) == 0;
}

static void Mt_Texture(int stage, BYTE* img)
{
    if (img && F(BYTE*, img, 76))
        Com(g_device, 140, stage, F(BYTE*, img, 76));
}

extern "C" {

// [FUN_5b04c0] resets the device states, then (unless 702e8c bit 25 turns multitexturing off) checks with the
// textures of a and b which two-stage blends validate: modulate (7208d4), the 7208cc variant, add (7208d0);
// none of them clears 7208bc, a Radeon clears 7208c4
void __cdecl Device_ProbeMultitexture(BYTE* a, BYTE* b)
{
    CallC(FUN_5b0120, 0);
    Com(g_device, 80, 41, 0);
    Mt_Stage(0, 11, 0);
    Mt_Stage(1, 11, 1);
    Mt_Stage(0, 16, 2);
    Mt_Stage(0, 17, 2);
    Mt_Stage(0, 18, 1);
    Mt_Stage(1, 16, 2);
    Mt_Stage(1, 17, 2);
    Mt_Stage(1, 18, 1);
    if (g_702e8c & 0x2000000) {
        g_mt7208bc = 0;
        g_mt7208d4 = 0;
        g_mt7208d0 = 0;
        g_mt7208cc = 0;
        g_mt7208c4 = 0;
        return;
    }
    if (g_mt7208bc) {
        Mt_Stage(0, 2, 2);
        Mt_Stage(0, 3, 0);
        Mt_Stage(0, 1, 4);
        Mt_Stage(0, 5, 2);
        Mt_Stage(0, 6, 0);
        Mt_Stage(0, 4, 4);
        Mt_Texture(0, a);
        Mt_Stage(1, 2, 2);
        Mt_Stage(1, 3, 1);
        Mt_Stage(1, 1, 13);
        if (g_mt7208c8) {
            Mt_Stage(1, 6, 1);
            Mt_Stage(1, 4, 3);
        } else {
            Mt_Stage(1, 5, 1);
            Mt_Stage(1, 4, 2);
        }
        Mt_Texture(1, b);
        if (!Mt_Valid())
            g_mt7208d4 = 0;
        if (g_mt7208cc) {
            Mt_Stage(1, 1, 14);
            if (!Mt_Valid())
                g_mt7208cc = 0;
        }
        Mt_Stage(1, 2, 2);
        Mt_Stage(1, 3, 1);
        Mt_Stage(1, 1, 4);
        Mt_Stage(1, 5, 2);
        Mt_Stage(1, 6, 1);
        Mt_Stage(1, 4, 4);
        if (!Mt_Valid())
            g_mt7208d0 = 0;
        if (!g_mt7208d4 && !g_mt7208d0)
            g_mt7208bc = 0;
        if (CallC<int>(FUN_4cca90))
            g_mt7208c4 = 0;
    }
    Com(g_device, 80, 41, 1);
}

} // extern "C"

// === pushing a model matrix (FUN_5ae580)

KING_VAR(BYTE, g_pm702a79, 702a79)
KING_VAR(AsmLabel, g_pm702a48, 702a48)
KING_VAR(AsmLabel, g_pm702a10, 702a10)
KING_VAR(AsmLabel, s_pm67ef94, 67ef94)

extern "C" {

// [FUN_5ae580] pushes model matrix m on renderer s's stack at +5216 (count +8236; multiplied onto the current one
// unless replace is 0): the view-model product (by the camera +4208 and +4256) goes on the stack at +2288, its
// inverse transposed at +3248 (count +8224, up to 18 each); a hardware device gets the world transform
void __fastcall Render_PushMatrix(BYTE* s, void*, const BYTE* m, int multiply)
{
    BYTE* A = (BYTE*)g_pm702a48;
    BYTE* B = (BYTE*)g_pm702a10;
    if (!(g_pm702a79 & 1)) {
        g_pm702a79 |= 1;
        memcpy(A, (BYTE*)g_cm7025f0, 48);
        atexit((void (__cdecl*)(void))(void*)Ren5ae890_Nop);
    }
    if (!(g_pm702a79 & 2)) {
        g_pm702a79 |= 2;
        memcpy(B, (BYTE*)g_cm7025f0, 48);
        atexit((void (__cdecl*)(void))(void*)Ren5ae880_Nop);
    }
    if (F(int, s, 8236) >= 18 || F(int, s, 8224) >= 18)
        return;
    BYTE t1[48], t2[48];
    BYTE* top = s + 5264 + F(int, s, 8236) * 48;
    if (!multiply)
        memcpy(top, m, 48);
    else
        memcpy(top, CallC<BYTE*>(FUN_5aba40, t2, m, top - 48), 48);
    F(int, s, 8236)++;
    BYTE* cam = s + 4256 + F(int, s, 8228) * 48;
    BYTE* r = CallC<BYTE*>(FUN_5aba40, t2, CallC<BYTE*>(FUN_5aba40, t1, top, s + 4208), cam);
    memcpy(A, r, 48);
    memcpy(s + 2288 + F(int, s, 8224) * 48, A, 48);
    if (g_720904) {
        const float* f = (const float*)(s + 5216 + F(int, s, 8236) * 48);
        float w[16];
        for (int i = 0; i < 4; i++) {
            w[i * 4] = f[i * 3];
            w[i * 4 + 1] = f[i * 3 + 1];
            w[i * 4 + 2] = f[i * 3 + 2];
            w[i * 4 + 3] = 0.0f;
        }
        w[15] = 1.0f;
        if (((long(__stdcall*)(void*, int, float*))(*(void***)g_device)[11])(g_device, 1, w))
            CallC(FUN_5d2ba0, s_pm67ef94, 0);
    }
    CallC(FUN_5ac480, B, A);
    CallC(FUN_5ac5f0, s + 3248 + F(int, s, 8224) * 48, B);
    F(int, s, 8224)++;
    F(int, s, 8444) = ++g_transformSerial;
}

} // extern "C"

// === drawing a 3D line in software (FUN_5b49e0)

extern "C" void FUN_5ab950(); KING_ALIAS("_FUN_5ab950", "@Vec3_TransformPoint@12")
KING_VAR(float, g_dl64c258, 64c258)

extern "C" {

// [FUN_5b49e0] draws the line (ax, ay, az)..(bx, by, bz) in color on renderer s's screen (+1948): both ends
// transformed by the current matrix (+2240 + 48 * +8224) into vertices 0 and 1 (+1224), clipped by the planes
// 1, 4, 16, 8, 2 between the id lists +1756 / +1844, projected (by 1 / y) into the points at +20 (44 bytes each)
// and joined by segments
void __fastcall Render_Line3D(BYTE* s, void*, float ax, float ay, float az, float bx, float by, float bz,
                              int color)
{
    BYTE* m = s + 2240 + F(int, s, 8224) * 48;
    float p[3] = {ax, ay, az};
    CallThis(FUN_5ab950, p, m);
    memcpy(s + 1224, p, 12);
    int* lists = (int*)(s + 1756);    // two lists of 22 ids
    lists[0] = 0;
    float q[3] = {bx, by, bz};
    CallThis(FUN_5ab950, q, m);
    lists[1] = 1;
    memcpy(s + 1236, q, 12);
    F(int, s, 1932) = 2;
    int n = 2, cur = 0;
    F(DWORD, s, 8272) |= 31;
    static const int k_planes[] = {1, 4, 16, 8, 2};
    for (int plane : k_planes)
        if (F(DWORD, s, 8272) & plane) {
            n = Rn_ClipIds(s, 0, 0, n, lists + (cur ^ 1) * 22, lists + cur * 22, plane);
            cur ^= 1;
        }
    if (!n)
        return;
    BYTE* scr = F(BYTE*, s, 1948);
    double w = F(WORD, scr, 0), h = F(WORD, scr, 2);
    for (int i = 0; i < n; i++) {
        const float* v = (const float*)(s + 1224 + lists[cur * 22 + i] * 12);
        double r = g_one64b670 / v[1];
        F(int, s, 20 + i * 44) = (int)((r * v[0] + g_q161_64c250) * w * g_dl64c258);
        F(int, s, 24 + i * 44) = (int)((g_q161_64c250 - r * v[2]) * h * g_dl64c258);
    }
    for (int i = 1; i < n; i++)
        CallC(FUN_5b73a0, scr, F(int, s, 20 + (i - 1) * 44), F(int, s, 24 + (i - 1) * 44), F(int, s, 20 + i * 44),
              F(int, s, 24 + i * 44), color, 0);
}

} // extern "C"

// === the path constructor (FUN_5bcc60)

extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_5bc320(); KING_ALIAS("_FUN_5bc320", "@Poly_GetRecord@16")
extern "C" void FUN_5bc350(); KING_ALIAS("_FUN_5bc350", "@Poly_SetWeight@20")
extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
struct Pa_V3 { float x, y, z; };

extern "C" {

// [FUN_5bcc60] constructor (n, points, a, b, speed): a path (vtables 651794, 651780 at +16) through the n points
// (12 bytes each) as translation matrices (+8) with weights 0..n-1, the cumulative segment lengths (+28),
// a (+40), b (+44), the speed truncated (+48) and two empty tables of 10 (+64, +104)
BYTE* __fastcall Path_Construct(BYTE* s, void*, int n, const Pa_V3* pts, int a, int b, double speed)
{
    F(void*, s, 0) = g_ren651798;
    F(int, s, 4) = n;
    F(void*, s, 12) = operator new(n * 4);
    BYTE* arr = (BYTE*)operator new(n * 48);
    if (arr)
        for (int i = 0; i < n; i++) {
            CallStd(FUN_4011e0, arr + i * 48, 12, 4, (void*)FUN_4bd4d0);
            memcpy(arr + i * 48, (BYTE*)g_cm7025f0, 48);
        }
    F(BYTE*, s, 8) = arr;
    F(void*, s, 16) = g_ren64d81c;
    F(int, s, 20) = 0;
    F(int, s, 24) = 0;
    F(void*, s, 0) = g_ren651794;
    F(void*, s, 16) = g_ren651780;
    F(int, s, 52) = 0;
    F(int, s, 40) = a;
    F(int, s, 44) = b;
    F(int, s, 48) = (int)speed;
    F(int, s, 56) = 0;
    F(int, s, 60) = 0;
    F(double*, s, 28) = (double*)operator new(n * 8 - 8);
    F(int, s, 32) = 0;
    F(int, s, 36) = 0;
    for (int i = 0; i < F(int, s, 4); i++) {
        BYTE t[48];
        CallThis(FUN_5bc320, s, i, CallC<BYTE*>(FUN_5abc40, t, pts[i]));
        CallThis(FUN_5bc350, s, i, (double)i);
        if (i) {
            BYTE* m = F(BYTE*, s, 8);
            const float* c = (const float*)(m + i * 48 + 36);
            const float* p = (const float*)(m + i * 48 - 12);
            float v[3] = {c[0] - p[0], c[1] - p[1], c[2] - p[2]};
            double* len = F(double*, s, 28);
            len[i - 1] = CallC<double>(FUN_5ab4d0, v);
            if (i > 1)
                len[i - 1] = len[i - 2] + len[i - 1];
        }
    }
    for (int k = 0; k < 10; k++) {
        F(int, s, 104 + k * 4) = 0;
        F(int, s, 64 + k * 4) = 0;
    }
    return s;
}

} // extern "C"

// === the hardware viewport and projection (FUN_5adea0)

KING_VAR(double, g_vp64b930, 64b930)

extern "C" {

// [FUN_5adea0] gives the 3D device renderer s's viewport (centre +1952/+1956 and half size +1960/+1964 in 12.20
// fixed point, in the window's rectangle, kept at +8548..+8560) and the perspective projection of the top camera
// matrix (+4256) with near +8464 and far +8472 (identity when near is not above 64b930)
void __fastcall Render_SetDeviceView(BYTE* s)
{
    if (!g_device)
        return;
    int r[4];
    CallThis(FUN_5b6710, F(BYTE*, s, 1948), r);
    DWORD vp[6] = {0, 0, 0, 0, 0, 0};
    int x = (F(int, s, 1952) >> 20) + r[0], y = (F(int, s, 1956) >> 20) + r[1];
    F(int, s, 8548) = x;
    vp[0] = x;
    F(int, s, 8552) = y;
    vp[1] = y;
    int hw = (F(int, s, 1960) >> 20) + 1, hh = (F(int, s, 1964) >> 20) + 1;
    F(int, s, 8556) = hw;
    F(int, s, 8560) = hh;
    vp[2] = (unsigned)(hw * 2) > (unsigned)(r[2] - r[0]) ? r[2] - r[0] : hw * 2;
    vp[3] = (unsigned)(hh * 2) > (unsigned)(r[3] - r[1]) ? r[3] - r[1] : hh * 2;
    F(float, vp, 16) = 0.0f;
    F(float, vp, 20) = 1.0f;
    ((long(__stdcall*)(void*, DWORD*))(*(void***)g_device)[13])(g_device, vp);
    const float* m = (const float*)(s + 4256 + F(int, s, 8228) * 48);
    float p[16];
    memset(p, 0, sizeof p);
    double n = F(double, s, 8464), f = F(double, s, 8472);
    if (n > g_vp64b930) {
        double d = f - n;
        p[0] = m[0];
        p[5] = m[8];
        p[8] = m[3];
        p[9] = m[5];
        p[10] = (float)(f / d);
        p[11] = 1.0f;
        p[14] = (float)-(n * f / d);
    } else
        p[0] = p[5] = p[10] = p[15] = 1.0f;
    ((long(__stdcall*)(void*, int, float*))(*(void***)g_device)[11])(g_device, 3, p);
}

} // extern "C"

// === drawing a sprite (FUN_5b4d40)

extern "C" void FUN_5b8980(); KING_ALIAS("_FUN_5b8980", "_Gfx_DrawImage")
KING_VAR(float, g_sp651760, 651760)
KING_VAR(AsmLabel, s_sp67f0c0, 67f0c0)

extern "C" {

// [FUN_5b4d40] draws image idx (+8268, count +1944) of renderer s at (x, y, z): projected and scaled by distance
// in 3D (mode 1; skipped outside y in (64c250, far +8472)), else at screen (x, y); anchored by align (1..8:
// centre/right/middle combinations), skipped when off the viewport
void __fastcall Render_Sprite(BYTE* s, void*, int mode, int align, float x, float y, float z, int idx)
{
    if (!F(BYTE*, s, 1948))
        return;
    if (idx < 0 || idx >= F(int, s, 1944))
        CallC(FUN_5d2e70, (const char*)s_sp67f0c0, (const char*)0);
    BYTE* img = F(BYTE**, s, 8268)[idx];
    int w = F(WORD, img, 4), h = F(WORD, img, 6);
    int px, py, scale;
    if (mode == 1) {
        float p[3] = {x, y, z};
        CallThis(FUN_5ab950, p, s + 2240 + F(int, s, 8224) * 48);
        if (!(p[1] > g_q161_64c250) || !(p[1] < F(double, s, 8472)))
            return;
        scale = (int)(g_q161_64c250 / p[1] * g_sp651760);
        float qx = (float)(p[0] / p[1]), qz = (float)(p[2] / p[1]);
        px = ((int)((qx + g_q161_64c250) * F(int, s, 1960)) + F(int, s, 1952) + 128) >> 20;
        py = ((int)((g_q161_64c250 - qz) * F(int, s, 1964)) + F(int, s, 1956) + 128) >> 20;
    } else {
        scale = 0;
        px = (int)x;
        py = (int)y;
    }
    switch (align) {
    case 1: px -= w / 2; break;
    case 2: px -= w; break;
    case 3: py -= h / 2; break;
    case 4: px -= w / 2; py -= h / 2; break;
    case 5: px -= w; py -= h / 2; break;
    case 6: py -= h; break;
    case 7: px -= w / 2; py -= h; break;
    case 8: px -= w; py -= h; break;
    }
    if (px >= (F(int, s, 1952) + F(int, s, 1960) * 2) >> 20 || py >= (F(int, s, 1956) + F(int, s, 1964) * 2) >> 20 ||
        px + w <= F(int, s, 1952) >> 20 || py + h <= F(int, s, 1956) >> 20)
        return;
    CallC(FUN_5b8980, F(BYTE*, s, 1948), px, py, img, scale);
    g_720078++;
}

} // extern "C"

// === clipping a polygon by a plane (FUN_5af000)

KING_VAR(double, g_cp64b748, 64b748)
KING_VAR(double, g_cp651710, 651710)

// the vertex i of the clipper s
static const float* Cp_V(BYTE* s, int i) { return (const float*)(s + 1224 + i * 12); }

// the signed distance of v from plane p
static double Cp_Dist(const float* v, const float* p)
{
    return (double)v[2] * p[2] + (double)v[1] * p[1] + (double)p[0] * v[0] + p[3];
}

// where the edge from vertex c to vertex a crosses plane p, between 0 and 1
static double Cp_Cross(BYTE* s, int a, int c, const float* p)
{
    const float* va = Cp_V(s, a);
    const float* vc = Cp_V(s, c);
    double d = ((double)va[0] - vc[0]) * p[0] + ((double)va[1] - vc[1]) * p[1] + ((double)va[2] - vc[2]) * p[2];
    if ((d < 0 ? -d : d) < g_cp64b748)
        d = d > g_q146_64b678 ? g_cp64b748 : g_cp651710;
    double t = -(((double)vc[2] * p[2] + (double)vc[1] * p[1] + (double)vc[0] * p[0] + p[3]) / d);
    if (t < g_q146_64b678)
        t = 0;
    else if (t > g_one64b670)
        t = 1.0;
    return t;
}

extern "C" {

// [FUN_5af000] clips the n-vertex polygon in index list *which (of the two at +1756, 88 bytes each) by plane p into
// the other, adding the crossings (Clip_Interpolate with flags) and flipping *which; the new count
int __fastcall Clip_ByPlane(BYTE* s, void*, int flags, int n, int* which, const float* p)
{
    int* in = (int*)(s + 1756 + *which * 88);
    int* out = (int*)(s + 1756 + (*which ^ 1) * 88);
    if (!n)
        return 0;
    int first = in[0];
    bool firstIn = Cp_Dist(Cp_V(s, first), p) >= 0, prevIn = firstIn, curIn = firstIn;
    int count = 0, prev = 0, cur = first;
    for (int k = 0; k < n; k++) {
        curIn = Cp_Dist(Cp_V(s, cur), p) >= 0;
        if (curIn != prevIn) {
            Clip_Interpolate(s, 0, flags, out++, cur, prev, Cp_Cross(s, prev, cur, p));
            count++;
        }
        if (curIn) {
            *out++ = in[k];
            count++;
        }
        prev = in[k];
        cur = in[k + 1];
        prevIn = curIn;
    }
    if (firstIn != curIn) {
        Clip_Interpolate(s, 0, flags, out, first, prev, Cp_Cross(s, prev, first, p));
        count++;
    }
    *which ^= 1;
    return count;
}

} // extern "C"

// === pushing a light (FUN_5ad180)

struct Rl_Light {                  // D3DLIGHT7
    int   type;
    float diffuse[4], specular[4], ambient[4], pos[3], dir[3];
    float range, falloff, att0, att1, att2, theta, phi;
};
extern "C" void __cdecl Shell_AddLight(Rl_Light l, BYTE* r, const BYTE* light);
KING_VAR(double, g_rl64b7c0, 64b7c0)

extern "C" {

// [FUN_5ad180] pushes light l (+0 kind, +4 position, +16 direction, +28.. attenuation, +40 intensity, +44/+48 cone,
// +52 colour) of up to 20 into renderer s, in the current frame (+5216, +8236): with a device it goes to the shell
// as a D3D light (kind 1 directional, 2 point, 3 spot), then is kept at +6224 and as the last at +7824
void __fastcall Renderer_PushLight(BYTE* s, void*, const BYTE* l)
{
    if (F(int, s, 8240) >= 20)
        return;
    const float* m = (const float*)(s + 5216 + F(int, s, 8236) * 48);
    const float* lp = (const float*)(l + 4);
    const float* ld = (const float*)(l + 16);
    float p[3], d[3];
    p[0] = (float)((double)m[6] * lp[2] + (double)m[3] * lp[1] + (double)m[0] * lp[0] + m[9]);
    p[1] = (float)((double)m[7] * lp[2] + (double)m[4] * lp[1] + (double)m[1] * lp[0] + m[10]);
    p[2] = (float)((double)m[8] * lp[2] + (double)m[5] * lp[1] + (double)m[2] * lp[0] + m[11]);
    d[0] = (float)((double)m[3] * ld[1] + (double)m[6] * ld[2] + (double)ld[0] * m[0]);
    d[1] = (float)((double)m[4] * ld[1] + (double)m[1] * ld[0] + (double)m[7] * ld[2]);
    d[2] = (float)((double)m[5] * ld[1] + (double)m[2] * ld[0] + (double)m[8] * ld[2]);
    if (g_q150_720908) {
        Rl_Light x;
        memset(&x, 0, sizeof x);
        float k = F(float, l, 40);
        float c[3] = {(float)(F(float, l, 52) * (double)k), (float)(F(float, l, 56) * (double)k),
                      (float)(F(float, l, 60) * (double)k)};
        for (int i = 0; i < 3; i++)
            x.diffuse[i] = x.specular[i] = c[i];
        x.diffuse[3] = x.specular[3] = 1.0f;
        x.ambient[3] = 1.0f;
        memcpy(x.pos, p, 12);
        memcpy(x.dir, d, 12);
        x.att0 = F(float, l, 28);
        x.theta = F(float, l, 44);
        x.att1 = F(float, l, 32);
        x.att2 = F(float, l, 36);
        x.phi = (float)((double)F(float, l, 48) + F(float, l, 44));
        x.falloff = 1.0f;
        switch (F(int, l, 0)) {
        case 1:
            x.type = 3;
            x.range = 10000.0f;
            break;
        case 2:
            x.type = 1;
            x.range = (float)(g_rl64b7c0 / x87_sqrt(F(float, l, 36)));
            break;
        case 3:
            x.type = 2;
            x.range = (float)(g_rl64b7c0 / x87_sqrt(F(float, l, 36)));
            break;
        }
        Shell_AddLight(x, s, l);
    }
    int n = F(int, s, 8240);
    Light_Copy((DWORD*)(s + 6224 + n * 80), 0, (const DWORD*)l);
    Light_Copy((DWORD*)(s + 7824), 0, (const DWORD*)l);
    n = F(int, s, 8240);
    memcpy(s + 6228 + n * 80, p, 12);
    memcpy(s + 7828, p, 12);
    memcpy(s + (n + 78) * 80, d, 12);
    memcpy(s + 7840, d, 12);
    F(int, s, 8240) = n + 1;
}

} // extern "C"

// === the fog blend tables (FUN_5b9850)

extern "C" {

// [FUN_5b9850] builds k fog tables of palette s for its n fog colours (+24 + 3c, +792): table i blends each colour
// (palette entry, or 15-bit value for format 2 at +808) with each fog colour by (i + 1) / (k + 1), as the nearest
// palette index or a packed pixel; s+0 the tables, s+4 their checksums
void __fastcall Fog_BuildTables(BYTE* s, void*, int k, int n)
{
    F(int, s, 792) = n;
    int entries;
    if (F(int, s, 808) == 1)
        entries = 256;
    else if (F(int, s, 808) == 2)
        entries = 32768;
    else
        return;
    F(int, s, 796) = k;
    F(void*, s, 0) = operator new(k * 4);
    F(void*, s, 4) = operator new(k * 2);
    if (k <= 0)
        return;
    float kf = (float)(k + 1);
    for (int i = 0; i < k; i++) {
        BYTE* t = (BYTE*)operator new(F(int, s, 808) * entries * F(int, s, 792));
        F(BYTE**, s, 0)[i] = t;
        F(WORD*, s, 4)[i] =
            (WORD)CallC<int>(FUN_4cef50, F(BYTE**, s, 0)[i], F(int, s, 808) * entries * F(int, s, 792));
        float f = (float)((i + 1) / (double)kf);
        float g = (float)(g_q161_64c250 - (double)f);
        for (int e = 0; e < entries; e++) {
            const BYTE* pe = s + 24 + e * 3;
            for (int c = 0; c < F(int, s, 792); c++) {
                const BYTE* fc = s + 24 + c * 3;
                if (F(int, s, 808) == 1) {
                    int r = (int)(pe[0] * (double)g + fc[0] * (double)f);
                    int gg = (int)(pe[1] * (double)g + fc[1] * (double)f);
                    int b = (int)(pe[2] * (double)g + fc[2] * (double)f);
                    F(BYTE**, s, 0)[i][c * 256 + e] = Palette_Nearest(s, 0, (BYTE)r, (BYTE)gg, (BYTE)b);
                } else if (F(int, s, 808) == 2) {
                    int r = (int)(((e >> 7) & 248) * (double)g + fc[0] * (double)f);
                    int gg = (int)(((e >> 2) & 248) * (double)g + fc[1] * (double)f);
                    int b = (int)(((e & 31) << 3) * (double)g + fc[2] * (double)f);
                    ((WORD*)F(BYTE**, s, 0)[i])[e + c * 32768] = (WORD)Pixel_Pack((BYTE)r, (BYTE)gg, (BYTE)b);
                }
            }
        }
    }
}

} // extern "C"

// === the tables towards one colour (FUN_5b9b60)

extern "C" {

// [FUN_5b9b60] builds k tables of palette s towards its colour c (+24 + 3c): table i blends each colour (palette
// entry, or 15-bit value for format 2 at +808) with it by i / k; s+8 the tables, s+12 their checksums
void __fastcall Fade_BuildTables(BYTE* s, void*, int k, int c)
{
    int entries;
    if (F(int, s, 808) == 1)
        entries = 256;
    else if (F(int, s, 808) == 2)
        entries = 32768;
    else
        return;
    F(int, s, 800) = k;
    F(int, s, 812) = c;
    F(void*, s, 8) = operator new(k * 4);
    F(void*, s, 12) = operator new(k * 2);
    if (k <= 0)
        return;
    float kf = (float)k;
    const BYTE* fc = s + 24 + c * 3;
    for (int i = 0; i < k; i++) {
        BYTE* t = (BYTE*)operator new(entries * F(int, s, 808));
        F(BYTE**, s, 8)[i] = t;
        int sum = CallC<int>(FUN_4cef50, F(BYTE**, s, 8)[i], entries * F(int, s, 808));
        float f = (float)(i / (double)kf);
        F(WORD*, s, 12)[i] = (WORD)sum;
        float g = (float)(g_q161_64c250 - (double)f);
        for (int e = 0; e < entries; e++) {
            const BYTE* pe = s + 24 + e * 3;
            if (F(int, s, 808) == 1) {
                int r = (int)(fc[0] * (double)f + pe[0] * (double)g);
                int gg = (int)(fc[1] * (double)f + pe[1] * (double)g);
                int b = (int)(pe[2] * (double)g + fc[2] * (double)f);
                F(BYTE**, s, 8)[i][e] = Palette_Nearest(s, 0, (BYTE)r, (BYTE)gg, (BYTE)b);
            } else if (F(int, s, 808) == 2) {
                int r = (int)(fc[0] * (double)f + ((e >> 7) & 248) * (double)g);
                int gg = (int)(((e >> 2) & 248) * (double)g + fc[1] * (double)f);
                int b = (int)(((e & 31) << 3) * (double)g + fc[2] * (double)f);
                ((WORD*)F(BYTE**, s, 8)[i])[e] = (WORD)Pixel_Pack((BYTE)r, (BYTE)gg, (BYTE)b);
            }
        }
    }
}

} // extern "C"

// === joining two paths (FUN_5bcff0)

extern "C" void Pt_Length(); KING_ALIAS("_Pt_Length", "_Vec3_Length")

// whether points a and b (3 floats) are less than 1 apart
static bool Pt_Near(const float* a, const float* b)
{
    float d[3] = {(float)((double)a[0] - b[0]), (float)((double)a[1] - b[1]), (float)((double)a[2] - b[2])};
    return CallC<double>(Pt_Length, d) < g_q161_64c250;
}

// adds path q to the start (+64, count +56) or end (+104, count +60) list of p
static void Pt_Add(BYTE* p, bool end, BYTE* q)
{
    int k = end ? 60 : 56;
    F(BYTE*, p, (end ? 104 : 64) + F(int, p, k) * 4) = q;
    F(int, p, k)++;
}

extern "C" {

// [FUN_5bcff0] joins paths s and o where an end of one (frames +8, count +4) meets an end of the other
void __fastcall Path_Join(BYTE* s, void*, BYTE* o)
{
    const float* a = (const float*)(F(BYTE*, s, 8) + 36);
    const float* al = (const float*)(F(BYTE*, s, 8) + F(int, s, 4) * 48 - 12);
    BYTE of[48], ol[48];
    memcpy(of, F(BYTE*, o, 8), 48);
    memcpy(ol, F(BYTE*, o, 8) + F(int, o, 4) * 48 - 48, 48);
    const float* bf = (const float*)(of + 36);
    const float* bl = (const float*)(ol + 36);
    if (Pt_Near(a, bf)) {
        Pt_Add(s, false, o);
        Pt_Add(o, false, s);
    } else if (Pt_Near(a, bl)) {
        Pt_Add(s, false, o);
        Pt_Add(o, true, s);
    } else if (Pt_Near(al, bl)) {
        Pt_Add(s, true, o);
        Pt_Add(o, true, s);
    } else if (Pt_Near(al, bf)) {
        Pt_Add(s, true, o);
        Pt_Add(o, false, s);
    }
}

} // extern "C"

// === the darkening tables (FUN_5b9e30)

extern "C" {

// [FUN_5b9e30] builds k tables of palette s darkening each colour (palette entry, or 15-bit value for format 2 at
// +808) by 1 - i / k, as the nearest palette index or a packed pixel; s+16 the tables, s+20 their checksums
void __fastcall Dark_BuildTables(BYTE* s, void*, int k)
{
    int entries;
    if (F(int, s, 808) == 1)
        entries = 256;
    else if (F(int, s, 808) == 2)
        entries = 32768;
    else
        return;
    F(int, s, 804) = k;
    F(void*, s, 16) = operator new(k * 4);
    F(void*, s, 20) = operator new(k * 2);
    if (k <= 0)
        return;
    float kf = (float)k;
    for (int i = 0; i < k; i++) {
        F(BYTE**, s, 16)[i] = (BYTE*)operator new(entries * F(int, s, 808));
        int sum = CallC<int>(FUN_4cef50, F(BYTE**, s, 16)[i], entries * F(int, s, 808));
        float f = (float)(g_q161_64c250 - i / (double)kf);
        F(WORD*, s, 20)[i] = (WORD)sum;
        for (int e = 0; e < entries; e++) {
            const BYTE* pe = s + 24 + e * 3;
            if (F(int, s, 808) == 1) {
                int r = (int)(pe[0] * (double)f), g = (int)(pe[1] * (double)f), b = (int)(pe[2] * (double)f);
                F(BYTE**, s, 16)[i][e] = Palette_Nearest(s, 0, (BYTE)r, (BYTE)g, (BYTE)b);
            } else if (F(int, s, 808) == 2) {
                int b = (int)(((e & 31) << 3) * (double)f);
                int g = (int)(((e >> 2) & 248) * (double)f);
                int r = (int)(((e >> 7) & 248) * (double)f);
                ((WORD*)F(BYTE**, s, 16)[i])[e] = (WORD)Pixel_Pack(r, g, b);
            }
        }
    }
}

} // extern "C"

// === taking a texture surface (FUN_5ba8d0)

extern "C" void Tp_VecCtorEh(); KING_ALIAS("_Tp_VecCtorEh", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" BYTE* __fastcall Ren5baa20_Init(BYTE* s);
KING_VAR(BYTE*, g_tp72090c, 72090c)

extern "C" {

// [FUN_5ba8d0] *out = a surface for description d: with the pool on (702e98, created at the first use: 6 sets of
// 188 bytes) the last of a set of its size and format (+8, +12, +88..+100) unless d asks for 0x400000; else a new
// one (702ea0 counting up); 0 or the error of the creation
int __cdecl TexPool_Get(BYTE* d, void** out)
{
    if (g_q147_702e98) {
        BYTE* pool = g_q147_702ea8;
        if (!pool) {
            pool = (BYTE*)operator new(1132);
            if (pool) {
                CallStd(Tp_VecCtorEh, pool, 188, 6, (void*)Ren5baa20_Init, (void*)Holder5baa30_ReleaseAll);
                F(int, pool, 1128) = 0;
            }
            g_q147_702ea8 = pool;
        }
        void* s = 0;
        if (!(F(DWORD, d, 104) & 0x400000))
            for (int i = 0; i < F(int, pool, 1128); i++) {
                BYTE* e = pool + i * 188;
                if (F(int, d, 8) == F(int, e, 8) && F(int, d, 12) == F(int, e, 12) && F(int, d, 88) == F(int, e, 88) &&
                    F(int, d, 92) == F(int, e, 92) && F(int, d, 96) == F(int, e, 96) && F(int, d, 100) == F(int, e, 100)) {
                    if (int n = F(int, e, 184)) {
                        F(int, e, 184) = n - 1;
                        s = F(void*, e, 124 + (n - 1) * 4);
                    }
                    break;
                }
            }
        *out = s;
        if (s)
            return 0;
    }
    g_q147_702ea0++;
    return Com(g_tp72090c, 24, d, out, 0);
}

} // extern "C"

// === selecting a material for drawing (FUN_5b1020)

KING_VAR(float, g_mt64c250, 64c250)
KING_VAR(float, g_mt64c488, 64c488)
KING_VAR(double, g_mt64da58, 64da58)
KING_VAR(double, g_mt651758, 651758)
KING_VAR(int, g_mt67ef8c, 67ef8c)
KING_VAR(int, g_mt67ef90, 67ef90)
extern "C" void Mt_RootSurface(); KING_ALIAS("_Mt_RootSurface", "@Window_RootSurface@8")

extern "C" {

// [FUN_5b1020] makes material m (records of 124 bytes at +8260, followed by +52 for up to depth n more steps)
// current for lighting mode lit (0: none) and colour mode c (0: its palette colour (+40) when bit 12, else
// white; 1..3: red, green, blue): the D3D material (diffuse colour with the alpha from the transparency +88 and
// the fade +8440, specular +92, power +36), the stages, the second texture stage for the environment map (+8632,
// flag +8656), alpha blending, the colour key and fog; nothing when it is the current one (702e6c, 67ef8c)
void __fastcall Material_Select(BYTE* self, void*, BYTE* m, int lit, int c, int n)
{
    if (F(int, self, 8664))
        return;
    if (F(int, self, 8668))
        return;
    for (; F(int, self, 8276) > n && F(int, m, 52) >= 0; n++)
        m = F(BYTE*, self, 8260) + F(int, m, 52) * 124;
    if ((BYTE*)g_702e6c == m && lit == g_mt67ef8c)
        return;
    if (!CallThis<BYTE*>(Mt_RootSurface, F(BYTE*, self, 1948)))
        return;
    BYTE* rgb = CallThis<BYTE*>(Mt_RootSurface, F(BYTE*, self, 1948)) + (F(int, m, 40) + 8) * 3;
    if (!g_720904)
        return;
    float mat[17] = {};
    int keep = 1;
    DWORD flags = F(DWORD, m, 32);
    BYTE* prev = (BYTE*)g_702e6c;
    double a = ((double)g_mt64c250 - F(float, m, 88)) * (int)F(BYTE, self, 8440) * g_mt64c488;
    if (a > g_mt64da58)
        a = g_one64b670;
    float r = 0, g = 0, b = 0, al = 0;
    if (flags & 1) {
        if (prev && F(DWORD, prev, 32) == flags && F(int, prev, 36) == F(int, m, 36) && lit == g_mt67ef8c && !(flags & 0x1000))
            keep = 0;
        if (c == 0 && (flags & 0x1000) || c < 0 || c > 3) {
            if (c == 0) {
                r = (float)(rgb[0] * g_mt651758);
                g = (float)(rgb[1] * g_mt651758);
                b = (float)(rgb[2] * g_mt651758);
                al = (float)a;
            }
        } else {
            r = (c == 0 || c == 1) ? 1.0f : 0.0f;
            g = (c == 0 || c == 2) ? 1.0f : 0.0f;
            b = (c == 0 || c == 3) ? 1.0f : 0.0f;
            al = (float)a;
        }
    } else {
        r = (float)(rgb[0] * g_mt651758);
        g = (float)(rgb[1] * g_mt651758);
        b = (float)(rgb[2] * g_mt651758);
        al = (float)a;
    }
    float spec = F(float, m, 92);
    BYTE* stages = CallThis<BYTE*>(FUN_5b0810, self, m, prev, 0);
    int apply = 1;
    if (stages == (BYTE*)g_mt67ef90)
        apply = keep;
    g_mt67ef90 = (int)stages;
    BYTE* env = F(BYTE*, self, 8632);
    if (lit && (g_702e8c & 16) && g_mt7208bc && !(F(DWORD, m, 32) & 2)
        && (F(int, m, 44) == -1 || (F(DWORD, m, 32) & 0x20000)) && env) {
        F(int, self, 8656) = 1;
        Com(g_device, 140, 1, F(int, env, 76));
        Mt_Stage(1, 11, 1);
        BYTE* tex = F(BYTE*, env, 72);
        if (Com(tex, 96) == (int)0x887601C2)
            Image_Prepare(env, 0, F(int, self, 8640), F(int, self, 8644));
        Mt_Stage(1, 2, 2);
        Mt_Stage(1, 3, 1);
        Mt_Stage(1, 1, 4);
        Mt_Stage(1, 5, 2);
        Mt_Stage(1, 4, 2);
        Mt_Stage(1, 16, 2);
        Mt_Stage(1, 17, 2);
        Mt_Stage(1, 12, 3);
        Com(g_device, 140, 2, 0);
        Mt_Stage(2, 4, 1);
        Mt_Stage(2, 1, 1);
    } else
        F(int, self, 8656) = 0;
    if (apply) {
        Com(g_device, 80, 27, (!(F(DWORD, m, 32) & 32) && F(BYTE, self, 8440) == 255 && !stages) ? 0 : 1);
        mat[0] = r, mat[1] = g, mat[2] = b, mat[3] = al;
        mat[8] = mat[9] = mat[10] = spec;
        mat[16] = (float)F(int, m, 36);
        if (lit) {
            mat[4] = r, mat[5] = g, mat[6] = b, mat[7] = al;
            Com(g_device, 80, 148, 0);
            Com(g_device, 80, 145, 0);
        } else {
            mat[4] = mat[5] = spec;
            Com(g_device, 80, 148, 1);
            Com(g_device, 80, 145, 1);
        }
        Com(g_device, 64, mat);
        if (g_702e8c & 4)
            Com(g_device, 80, 28, (F(DWORD, m, 32) & 0x100) ? 0 : 1);
        if (g_702e8c & 0x4000) {
            DWORD f = F(DWORD, m, 32);
            if (f & 128) {
                Com(g_device, 80, 7, 0);
                Com(g_device, 80, 14, 0);
                Com(g_device, 80, 23, 8);
            } else {
                Com(g_device, 80, 7, 1);
                Com(g_device, 80, 14, (f & 0x40000) ? 0 : 1);
                Com(g_device, 80, 23, 4);
            }
        }
    }
    g_702e6c = (int)m;
    g_mt67ef8c = lit;
}

} // extern "C"

// === drawing a line (FUN_5b73a0)

extern "C" void FUN_5b7af0(); KING_ALIAS("_FUN_5b7af0", "_Line_ClipEdge")
KING_VAR(float, g_ln64c250, 64c250)
KING_VAR(double, g_ln64eba8, 64eba8)
KING_VAR(double, g_ln64eba0, 64eba0)

// a colour component scaled to the device's range
static int Ln_Comp(int c)
{
    return (int)(c * g_ln64eba8 * g_ln64eba0);
}

extern "C" {

// [FUN_5b73a0] draws the line (x0, y0)-(x1, y1) on window w in colour c (a palette index unless rgb): with
// the 3D device (+116 == 1) a D3D line, translucent by +88 in mode +80 == 2; else clipped by FUN_5b7af0 and
// stepped into the pixels
void __cdecl Window_DrawLine(BYTE* w, int x0, int y0, int x1, int y1, DWORD c, int rgb)
{
    BYTE* root = (BYTE*)Window_RootSurface(w, 0);
    if (F(int, w, 116) == 1) {
        if (!root)
            return;
        DWORD rc[4];
        int ox = Window_GetRect(w, 0, rc)[0];
        int oy = Window_GetRect(w, 0, rc)[1];
        DWORD col;
        int r, g, b;
        if (!rgb) {
            BYTE* pal = (BYTE*)Window_RootSurface(w, 0);
            if (!pal)
                return;
            pal += (c + 8) * 3;
            r = pal[0], g = pal[1], b = pal[2];
        } else {
            r = Pixel_Red(c) & 255;
            g = Pixel_Green(c) & 255;
            b = Pixel_Blue(c) & 255;
        }
        int blend = F(int, w, 80) == 2;
        DWORD a = blend ? (DWORD)(int)((g_ln64c250 - F(float, w, 88)) * g_255f) : 0xFFFFFFFF;
        col = (((a << 8 | Ln_Comp(r)) << 8 | Ln_Comp(g)) << 8) | Ln_Comp(b);
        Com(g_device, 80, 27, blend);
        float v[16] = {};
        v[0] = (float)(x0 + ox);
        v[1] = (float)(y0 + oy);
        F(DWORD, v, 8) = 953267991;
        v[3] = 1.0f;
        F(DWORD, v, 16) = col;
        F(DWORD, v, 20) = 0xFF000000;
        v[8] = (float)(x1 + ox);
        v[9] = (float)(y1 + oy);
        F(DWORD, v, 40) = 953267991;
        v[11] = 1.0f;
        F(DWORD, v, 48) = col;
        F(DWORD, v, 52) = 0xFF000000;
        Com(g_device, 140, 0, 0);
        Com(g_device, 148, 0, 1, 1);
        Com(g_device, 148, 0, 4, 1);
        Com(g_device, 100, 2, 452, v, 2, 0);
        Com(g_device, 80, 27, 0);
        return;
    }
    if (!rgb && F(WORD, w, 4) != 1) {
        if (!root)
            return;
        c = Palette_Color(root, 0, c);
    }
    if (CallC<int>(FUN_5b7af0, 0, 0, &x0, &y0, &x1, &y1) || CallC<int>(FUN_5b7af0, 1, 0, &x0, &y0, &x1, &y1)
        || CallC<int>(FUN_5b7af0, 2, F(WORD, w, 0) - 1, &x0, &y0, &x1, &y1)
        || CallC<int>(FUN_5b7af0, 3, F(WORD, w, 2) - 1, &x0, &y0, &x1, &y1))
        return;
    if (x0 > x1) {
        int t = x0;
        x0 = x1, x1 = t;
        t = y0;
        y0 = y1, y1 = t;
    }
    int dy = y1 - y0 < 0 ? y0 - y1 : y1 - y0;
    int dx = x1 - x0;
    int bpp = F(WORD, w, 4), pitch = F(WORD, w, 6);
    int e0 = dx - dy, step, sx, sy, sdiag, pmin, pdiag;
    if (y1 > y0) {
        if (dy < dx)
            step = -dy, sx = 1, sy = 0, sdiag = 1, pmin = bpp;
        else
            step = dx, sx = 0, sy = 1, sdiag = 1, pmin = pitch;
        pdiag = pitch + bpp;
    } else {
        if (dy < dx)
            step = -dy, sx = 1, sy = 0, sdiag = -1, pmin = bpp;
        else
            step = dx, sx = 0, sy = -1, sdiag = -1, pmin = -pitch;
        pdiag = bpp - pitch;
    }
    BYTE* p = F(BYTE*, w, 24 + F(int, w, 16) * 4) + pitch * y0 + bpp * x0;
    int x = x0, y = y0;
    if (x > x1)
        return;
    int e1 = e0, e2 = step;
    do {
        if ((y - y0 < 0 ? y0 - y : y - y0) > dy)
            return;
        if (bpp == 1)
            *p = (BYTE)c;
        else if (bpp == 2)
            *(WORD*)p = (WORD)c;
        else if (bpp == 4)
            *(DWORD*)p = c;
        if ((e2 < 0 ? -e2 : e2) < (e1 < 0 ? -e1 : e1)) {
            e1 += step;
            x += sx;
            e2 += step;
            y += sy;
            p += pmin;
        } else {
            e1 += e0;
            e2 += e0;
            x++;
            y += sdiag;
            p += pdiag;
        }
    } while (x <= x1);
}

} // extern "C"

// === the direction of a path at a point (FUN_5bc860)

struct Pd_V3 { float x, y, z; };
extern "C" void Pd_Normalized(); KING_ALIAS("_Pd_Normalized", "_Vec3_Normalized")
extern "C" void Pd_Length(); KING_ALIAS("_Pd_Length", "_Vec3_Length")
KING_VAR(Pd_V3, g_pd702650, 702650)

static Pd_V3 Pd_Diff(const BYTE* a, const BYTE* b)
{
    Pd_V3 d = {F(float, a, 36) - F(float, b, 36), F(float, a, 40) - F(float, b, 40), F(float, a, 44) - F(float, b, 44)};
    return d;
}

extern "C" {

// [FUN_5bc860] the direction of path s (frames of 48 bytes at +8, last index +4) at frame matrix m near frame *k:
// the segment k..k+1 blended into the next one (k+1..k+2, or the first segment of the path linked to s's key +40
// among the +104 links, count +60) by how far m's position lies along it; zero outside the path
Pd_V3* __fastcall Path_DirectionAt(BYTE* s, void*, Pd_V3* out, const BYTE* m, const int* k)
{
    int i = *k;
    if (i + 2 > F(int, s, 4) || i < 0) {
        *out = g_pd702650;
        return out;
    }
    BYTE* fr = F(BYTE*, s, 8);
    const BYTE* a = fr + i * 48;
    Pd_V3 d1 = Pd_Diff(fr + (i + 1) * 48, a);
    Pd_V3 d2;
    if (i + 3 > F(int, s, 4)) {
        int n = F(int, s, 60), j = 0;
        for (; j < n; j++)
            if (F(int, F(BYTE*, s, 104 + j * 4), 40) == F(int, s, 40))
                break;
        if (j == n || !F(BYTE*, s, 104 + j * 4)) {
            CallC(Pd_Normalized, out, &d1);
            return out;
        }
        BYTE* lf = F(BYTE*, F(BYTE*, s, 104 + j * 4), 8);
        d2 = Pd_Diff(lf + 48, lf);
    } else
        d2 = Pd_Diff(fr + (i + 2) * 48, fr + (i + 1) * 48);
    Pd_V3 v = {F(float, m, 36) - F(float, a, 36), F(float, m, 40) - F(float, a, 40), F(float, m, 44) - F(float, a, 44)};
    Pd_V3 t1, t2, t3;
    const Pd_V3* n1 = CallC<const Pd_V3*>(Pd_Normalized, &t1, &d1);
    double proj = (double)n1->z * v.z + (double)n1->y * v.y + (double)v.x * n1->x;
    double t = proj / CallC<double>(Pd_Length, &d1);
    if (t < g_q146_64b678)
        t = 0;
    else if (t > g_one64b670)
        t = 1.0;
    float w = (float)(g_one64b670 - t);
    const Pd_V3* a1 = CallC<const Pd_V3*>(Pd_Normalized, &t2, &d1);
    float ax = w * a1->x, ay = w * a1->y, az = w * a1->z;
    const Pd_V3* b1 = CallC<const Pd_V3*>(Pd_Normalized, &t3, &d2);
    float bx = (float)(t * b1->x), by = (float)(t * b1->y);
    Pd_V3 sum = {bx + ax, ay + by, (float)(az + t * b1->z)};
    CallC(Pd_Normalized, out, &sum);
    return out;
}

} // extern "C"

// === the side distance of a point from a path (FUN_5bc430)

extern "C" void Pd_SetThis(); KING_ALIAS("_Pd_SetThis", "@Vec3_SetThis@20")
extern "C" void Pd_Sub(); KING_ALIAS("_Pd_Sub", "_Vec3_Sub")
extern "C" void Pd_Cross(); KING_ALIAS("_Pd_Cross", "_Vec3_Cross")
KING_VAR(float, g_pd702630, 702630)
KING_VAR(float, g_pd702634, 702634)
KING_VAR(float, g_pd702638, 702638)

// the length of (x, y) made by Vec3_SetThis
static double Pd_FlatLength(float x, float y, float z)
{
    Pd_V3 v;
    CallThis(Pd_SetThis, &v, *(DWORD*)&x, *(DWORD*)&y, *(DWORD*)&z);
    v.z = 0;
    return CallC<double>(Pd_Length, &v);
}

extern "C" {

// [FUN_5bc430] the flat distance of frame m's position from segment *k of path s, signed by the side (up 702630);
// *out the nearest point; past either end the end is used and *k becomes -1 - *k; 0 and *k = -1 outside the path
double __fastcall Path_SideDistance(BYTE* s, void*, const BYTE* m, int* k, Pd_V3* out)
{
    Pd_V3 p = {F(float, m, 36), F(float, m, 40), F(float, m, 44)};
    int i = *k;
    if (i >= F(int, s, 4) - 1 || i < 0) {
        *k = -1;
        return g_q146_64b678;
    }
    const BYTE* fr = F(BYTE*, s, 8);
    Pd_V3 a = {F(float, fr + i * 48, 36), F(float, fr + i * 48, 40), F(float, fr + i * 48, 44)};
    Pd_V3 b = {F(float, fr + (i + 1) * 48, 36), F(float, fr + (i + 1) * 48, 40), F(float, fr + (i + 1) * 48, 44)};
    Pd_V3 d = {b.x - a.x, b.y - a.y, b.z - a.z};
    double len = CallC<double>(Pd_Length, &d);
    Pd_V3 u = {(float)(d.x / len), (float)(d.y / len), (float)(d.z / len)};
    double wx = (double)p.x - a.x, wy = (double)p.y - a.y, wz = (double)p.z - a.z;
    float fwx = (float)wx, fwy = (float)wy, fwz = (float)wz;
    double t = wz * u.z + wy * u.y + wx * u.x;
    float tx = (float)(u.x * t), ty = (float)(u.y * t);
    Pd_V3 q = {tx + a.x, ty + a.y, (float)(t * u.z + a.z)};
    double dist;
    if (t < g_q146_64b678) {
        if (out)
            *out = a;
        dist = Pd_FlatLength(fwx, fwy, fwz);
        *k = -1 - *k;
    } else if (t > len) {
        if (out)
            *out = b;
        dist = Pd_FlatLength(p.x - b.x, p.y - b.y, p.z - b.z);
        *k = -1 - *k;
    } else {
        if (out)
            *out = q;
        dist = Pd_FlatLength(p.x - q.x, p.y - q.y, p.z - q.z);
    }
    Pd_V3 e;
    float ex = p.x - a.x, ey = p.y - a.y, ez = p.z - a.z;
    CallThis(Pd_SetThis, &e, *(DWORD*)&ex, *(DWORD*)&ey, *(DWORD*)&ez);
    Pd_V3 c;
    CallC(Pd_Cross, &c, CallC<Pd_V3*>(Pd_Sub, &p, &b, &a), &e);
    if ((double)c.z * g_pd702638 + (double)c.y * g_pd702634 + (double)c.x * g_pd702630 < g_q146_64b678)
        return -dist;
    return dist;
}

} // extern "C"

// === a light's strength at a point (FUN_5ad780)

extern "C" void Ls_DivAssign(); KING_ALIAS("_Ls_DivAssign", "@Vec3_DivAssign@16")
KING_VAR(float, g_ls64c264, 64c264)
KING_VAR(double, g_ls64b6e0, 64b6e0)

// the direction from p to the light (normalized in d) and the attenuation 1 / (1 + k len^2)
static double Ls_Toward(const BYTE* l, const float* p, float* d)
{
    float t[3];
    d[0] = F(float, l, 4) - p[0];
    d[1] = F(float, l, 8) - p[1];
    d[2] = F(float, l, 12) - p[2];
    float len = CallC<float>(Pd_Length, d);
    CallThis(Ls_DivAssign, d, t, *(DWORD*)&len);
    return g_one64b670 / (len * (double)F(float, l, 36) * len + g_one64b670);
}

extern "C" {

// [FUN_5ad780] light l's strength at point p with normal n, raised from the ambient level amb: directional (kind 1)
// by the normal's facing, point (2) by distance, spot (3) also by the angle off its axis (+16) within the cone
// (+44, fading over +48)
double __fastcall Light_StrengthAt(BYTE* l, void*, const float* p, const float* n, double amb)
{
    double v;
    float d[3];
    switch (F(int, l, 0)) {
    case 3: {
        float att = (float)Ls_Toward(l, p, d);
        double c = (double)F(float, l, 24) * -d[2] + (double)F(float, l, 20) * -d[1] + (double)F(float, l, 16) * -d[0];
        float ang = (float)x87_acos(c);
        float lim = F(float, l, 44) + F(float, l, 48);
        if (ang > lim)
            v = g_ls64c264;
        else if (ang > F(float, l, 44))
            v = (lim - ang) * (double)att / F(float, l, 48);
        else
            v = att;
        break;
    }
    case 2:
        v = Ls_Toward(l, p, d);
        break;
    case 1:
        v = (-n[2] * (double)F(float, l, 24) + -n[1] * (double)F(float, l, 20) + -n[0] * (double)F(float, l, 16)
            + g_one64b670) * g_ls64b6e0;
        break;
    default:
        v = *(const float*)&n;    // as in the original: the argument slot read as a float
        break;
    }
    return v * (g_one64b670 - amb) + amb;
}

} // extern "C"

// === drawing a clipped polygon (FUN_5b42c0)

extern "C" void Rp_Blit(); KING_ALIAS("_Rp_Blit", "_Blit_WithKey")
extern "C" void Rp_PolyDraw(); KING_ALIAS("_Rp_PolyDraw", "_Poly_Draw")
KING_VAR(float, g_rp651764, 651764)
KING_VAR(float, g_rp651760, 651760)
KING_VAR(float, g_rp64c250, 64c250)
KING_VAR(AsmLabel, g_rp702e0c, 702e0c)

extern "C" {

// [FUN_5b42c0] draws the polygon of renderer r's current vertices (count +1932) with material m, flags and level:
// clipped against the planes of +8272 (bits 1, 4, 16, 8, 2) between the id buffers +1756/+1844, finished by vtbl+12;
// beyond the far plane (+8536) nothing, past +8540 flag 64; with the device (+8620 == 2) as a D3D fan of lit or
// plain TL vertices (+712) in the material's palette colour, else projected to the screen vertices (+20, 44 bytes)
// and drawn by Fw_Nop (flat 8/16-bit), Blit_WithKey (flag 64) or Poly_Draw
void __fastcall Render_Polygon(BYTE* r, void*, BYTE* m, DWORD flags, int level)
{
    int buf = 0;
    int n = F(int, r, 1932);
    DWORD clip = F(DWORD, r, 8272);
    if (clip) {
        static const DWORD bits[5] = {1, 4, 16, 8, 2};
        for (int b = 0; b < 5; b++)
            if (clip & bits[b]) {
                n = Rn_ClipIds(r, 0, flags, n, (int*)(r + 1756 + (buf ^ 1) * 88), (int*)(r + 1756 + buf * 88), bits[b]);
                buf ^= 1;
            }
    }
    n = CallVirt<int>(r, 12, flags, n, &buf);
    if (!n)
        return;
    const int* ids = (const int*)(r + 1756 + buf * 88);
    float zmin = 100000.0f, zmax = 0;
    for (int k = 0; k < n; k++) {
        float z = F(float, r, 1228 + ids[k] * 12);
        if (z < zmin)
            zmin = z;
        if (z > zmax)
            zmax = z;
    }
    if (g_702e8c & 4) {
        if (zmin > F(float, r, 8536))
            return;
        if (zmax > F(float, r, 8540))
            flags |= 64;
    }
    if (F(int, r, 8620) == 2) {
        BYTE* col = (BYTE*)Window_RootSurface(F(BYTE*, r, 1948), 0) + (F(int, m, 40) + 8) * 3;
        float* o = (float*)(r + 712);
        int lit = flags & 128;
        for (int k = 0; k < n; k++, o += 8) {
            int id = ids[k];
            const float* v = (const float*)(r + 1224 + id * 12);
            double w = g_one64b670 / v[1];
            double yw = w * v[2];
            o[0] = (float)((w * v[0] + g_one64b670) * F(int, r, 8556) + F(int, r, 8548));
            o[1] = (float)((g_one64b670 - yw) * F(int, r, 8560) + F(int, r, 8552));
            o[2] = (float)((v[1] - F(double, r, 8464)) / (F(double, r, 8472) - F(double, r, 8464)));
            o[3] = (float)w;
            double a = (g_rp64c250 - F(float, m, 88)) * (double)g_255f;
            DWORD c = (DWORD)(int)a;
            if (lit) {
                float l = F(float, r, 1664 + id * 4);
                for (int j = 0; j < 3; j++)
                    c = c << 8 | (DWORD)(int)(col[j] * (double)l * g_rp651764 * g_255f);
            } else {
                for (int j = 0; j < 3; j++)
                    c = c << 8 | (DWORD)(int)(col[j] * g_mt651758 * g_q161_64eba0);
            }
            F(DWORD, o, 16) = c;
            F(DWORD, o, 20) = 0xFF000000;
            F(int, o, 24) = 0;
            F(int, o, 28) = 0;
        }
        Com(g_device, 100, 6, 3, r + 712, n, 0);
        return;
    }
    int shade = flags & 128;
    for (int k = 0; k < n; k++) {
        BYTE* s = r + 20 + k * 44;
        int id = ids[k];
        const float* v = (const float*)(r + 1224 + id * 12);
        double w = g_one64b670 / v[1];
        int hw = F(int, r, 1960);
        F(int, s, 0) = ((int)(w * v[0] * hw) + hw + F(int, r, 1952) + 2048) & ~4095;
        int hh = F(int, r, 1964);
        F(int, s, 4) = (hh - (int)(hh * (w * v[2])) + F(int, r, 1956) + 2048) & ~4095;
        if (g_702e8c & 0x4000)
            F(int, s, 8) = (int)(w * g_rp651760);
        if (shade && Window_RootSurface(F(BYTE*, r, 1948), 0)) {
            int lv = F(int, (BYTE*)Window_RootSurface(F(BYTE*, r, 1948), 0), 804) - 2;
            F(int, s, 28) = (int)(lv * (g_rp64c250 - F(float, r, 1664 + id * 4)) + g_rp64c250) << 20;
        }
    }
    BYTE* win = F(BYTE*, r, 1948);
    if (!(flags & 128) && !(F(DWORD, m, 32) & 32) && (F(DWORD, win, 12) & 0x800)) {
        int* pts = (int*)(BYTE*)g_rp702e0c;
        for (int k = 0; k < n; k++) {
            pts[k * 2] = F(int, r, 20 + k * 44) >> 20;
            pts[k * 2 + 1] = F(WORD, win, 2) * F(int, win, 16) + (F(int, r, 24 + k * 44) >> 20);
        }
        int c = 0;
        if (F(WORD, win, 4) == 1 || F(WORD, win, 4) == 2)
            c = Palette_Shade((BYTE*)Window_RootSurface(win, 0), 0, F(int, m, 40), level);
        CallC(FUN_5e48f0, pts, n, c);
        return;
    }
    if (flags & 64)
        CallC(Rp_Blit, flags, win, n, r + 8, m, level, (int)(g_rp651760 / zmin));
    else
        CallC(Rp_PolyDraw, flags, win, n, r + 8, m, level);
}

} // extern "C"

// === preparing a polygon for drawing (FUN_5b22f0)

extern "C" void Pp_Setup(); KING_ALIAS("_Pp_Setup", "@Render_LayerUVs@16")
extern "C" void Pp_PlaneTransform(); KING_ALIAS("_Pp_PlaneTransform", "@Plane_Transform@12")
extern "C" void Pp_Nop1(); KING_ALIAS("_Pp_Nop1", "@Fw_Nop1@12")
extern "C" void Pp_Normalized(); KING_ALIAS("_Pp_Normalized", "_Vec3_Normalized")
extern "C" int __cdecl atexit(void (__cdecl*)(void));
KING_VAR(BYTE, g_pp702d98, 702d98)
KING_VAR(float, g_pp7029f8, 7029f8)    // a plane: normal, distance at +12
KING_VAR(int, g_pp721770, 721770)
KING_VAR(BYTE*, g_pp720908, 720908)

KING_VAR(double, g_pp64b658, 64b658)

// (M's rotation) x v, plus its translation with move
static void Pp_Xform(const float* M, const float* v, float* out, bool move)
{
    out[0] = (float)((double)M[6] * v[2] + (double)M[3] * v[1] + (double)M[0] * v[0] + (move ? M[9] : 0));
    out[1] = (float)((double)M[7] * v[2] + (double)M[4] * v[1] + (double)M[1] * v[0] + (move ? M[10] : 0));
    out[2] = (float)((double)M[8] * v[2] + (double)M[5] * v[1] + (double)M[2] * v[0] + (move ? M[11] : 0));
}

extern "C" {

// [FUN_5b22f0] renderer r's handling of polygon p (material +48, vertices +8 of +4): in the fast mode (+8620 1)
// drawn at once (culling off with flag 4, a fan with 128); otherwise its plane (+32) taken to the view, nothing
// when facing away; its texture coordinates animated by the material (spin 8, scroll 4), each vertex lit
// (+1664), given its coordinates (+1488, +1576) and number (+1756), the count to +1932; and the plane kept
// normalized for mirrors
void __fastcall Poly_Prepare(BYTE* r, void*, BYTE* p)
{
    if (!(g_pp702d98 & 1)) {
        g_pp702d98 |= 1;
        memset(&g_pp7029f8, 0, 16);
        atexit((void (__cdecl*)(void))(void*)Ren5b2940_Nop);
    }
    if (!F(BYTE*, r, 8252) || !F(BYTE*, r, 1948))
        return;
    BYTE* mat = F(BYTE*, r, 8260) + F(short, p, 48) * 124;
    WORD fl = F(WORD, p, 0);
    if (F(int, r, 8620) == 1) {
        if (!Window_RootSurface(F(BYTE*, r, 1948), 0))
            return;
        int a = (fl & 128) && g_pp721770 ? 1 : 0;
        int b = (fl & 1) && (g_702e8c & 64) ? 1 : 0;
        Render_Entry(r, 0, F(short, p, 48), b, a);
        CallThis(Pp_Setup, r, p, mat);
        if (F(BYTE, p, 0) & 4)
            Com(g_pp720908, 80, 22, 3);
        Rn_DrawStrided(r, 0, mat, F(BYTE, p, 0) & 128 ? 5 : 6, p + 52, F(WORD, p, 4));
        if (F(BYTE, p, 0) & 4)
            Com(g_pp720908, 80, 22, 2);
        if (F(BYTE, p, 0) & 128) {
            F(int, r, 8356) += F(WORD, p, 4) - 2;
            F(int, r, 8384)++;
        }
        return;
    }
    Shade_Assign((DWORD*)&g_pp7029f8, 0, (const DWORD*)(p + 32));
    CallThis(Pp_PlaneTransform, &g_pp7029f8, r + 3200 + F(int, r, 8224) * 48);
    if (!((&g_pp7029f8)[3] > g_pp64b658))
        return;
    if (F(int, r, 8620) == 2) {
        if (!Window_RootSurface(F(BYTE*, r, 1948), 0))
            return;
        Render_Entry(r, 0, F(short, p, 48), 0, 0);
    }
    const float* M = (const float*)(r + 5216 + F(int, r, 8236) * 48);
    double lt = F(double, r, 8512);
    if (Window_RootSurface(F(BYTE*, r, 1948), 0) && !(F(WORD, p, 0) & 16) && (F(WORD, p, 0) & 1) &&
        (g_702e8c & 64) && F(int, r, 8240) && F(int, r, 8448) != 2 && F(int, r, 8448) == 1) {
        float nw[3];
        nw[0] = (float)((double)M[3] * F(float, p, 36) + (double)M[6] * F(float, p, 40) + (double)F(float, p, 32) * M[0]);
        nw[1] = (float)((double)M[4] * F(float, p, 36) + (double)F(float, p, 32) * M[1] + (double)M[7] * F(float, p, 40));
        nw[2] = (float)((double)M[5] * F(float, p, 36) + (double)F(float, p, 32) * M[2] + (double)M[8] * F(float, p, 40));
        Lights_AtPoint(r, 0, (int)nw, lt);
    }
    int n = F(WORD, p, 4);
    if ((F(int, mat, 32) & 12) && n) {
        for (int i = 0; i < F(WORD, p, 4); i++) {
            if (!(F(BYTE, p, 0) & 2))
                continue;
            float* U = F(float*, p, 12) + i;
            float* V = F(float*, p, 16) + i;
            if (F(BYTE, mat, 32) & 8) {
                double ang = F(float, mat, 108) * F(double, r, 8480);
                float sn = (float)x87_sin(ang);
                double cs = x87_cos(ang);
                float du = *U - F(float, mat, 112), dv = *V - F(float, mat, 116);
                double nu = (double)dv * sn + du * cs + F(float, mat, 112);
                *V = (float)(dv * cs - (double)du * sn + F(float, mat, 116));
                *U = (float)nu;
            }
            if (F(BYTE, mat, 32) & 4) {
                *U = (float)(F(float, mat, 80) * F(double, r, 8480) + *U);
                *V = (float)(F(float, mat, 84) * F(double, r, 8480) + *V);
            }
        }
    }
    int i = 0;
    for (; i < F(WORD, p, 4); i++) {
        BYTE* vx = F(BYTE*, r, 8252) + F(WORD*, p, 8)[i] * 28;
        float* lit = (float*)(r + 1664 + i * 4);
        CallThis(Pp_Nop1, r, vx);
        if (Window_RootSurface(F(BYTE*, r, 1948), 0)) {
            WORD f = F(WORD, p, 0);
            if (f & 16) {
                if ((f & 1) && (g_702e8c & 64)) {
                    if (F(int, r, 8240) < 1)
                        *lit = (float)F(double, r, 8512);
                    else {
                        float wp[3];
                        Pp_Xform(M, (const float*)vx, wp, true);
                        *lit = (float)Lights_At(r, 0, (int)wp, F(int, p, 24) + i * 12, lt);
                    }
                } else
                    *lit = F(float*, p, 20)[i];
            } else if (F(int, r, 8448) == 2 && (f & 1) && (g_702e8c & 64)) {
                float wp[3];
                Pp_Xform(M, (const float*)vx, wp, true);
                *lit = (float)Lights_At(r, 0, (int)wp, (int)(p + 32), lt);
            }
        }
        if (F(BYTE, p, 0) & 2) {
            F(DWORD, r, 1488 + i * 4) = F(DWORD*, p, 12)[i];
            F(DWORD, r, 1576 + i * 4) = F(DWORD*, p, 16)[i];
        } else {
            F(DWORD, r, 1488 + i * 4) = F(DWORD, vx, 12);
            F(DWORD, r, 1576 + i * 4) = F(DWORD, vx, 16);
        }
        F(int, r, 1756 + i * 4) = i;
    }
    F(int, r, 1932) = i;
    if (F(int, r, 8620) == 2)
        return;
    short sh = F(short, p, 50);
    if (sh < 0) {
        int v = (100 - F(int, r, 8456)) * sh / -100;
        if (F(float, r, 1228) > v)
            return;
    }
    BYTE* m2 = F(BYTE*, F(BYTE*, r, 8260) + F(short, p, 48) * 124, 76);
    if (!(F(BYTE, m2, 52) & 1) || !(g_702e8c & 2))
        return;
    float t[3];
    const float* nn = CallC<const float*>(Pp_Normalized, t, &g_pp7029f8);
    memcpy(&g_pp7029f8, nn, 12);
}

} // extern "C"

// === a sphere against the view's frustum (FUN_5b4fc0)

#include "vecmath.h"

extern "C" void Fr_DivAssign(); KING_ALIAS("_Fr_DivAssign", "@Vec3_DivAssign@16")
extern "C" void Fr_RectSet(); KING_ALIAS("_Fr_RectSet", "@Rect_Set@24")
extern "C" void Fr_ZeroThis(); KING_ALIAS("_Fr_ZeroThis", "@Vec3_ZeroThis@8")
KING_VAR(BYTE, g_fr702a90, 702a90)                // the static guard
KING_VAR(double, g_fr702990, 702990)              // sqrt(64b660) * 64b6e0
KING_VAR(double, g_fr64b660, 64b660)
KING_VAR(double, g_fr64b6e0, 64b6e0)
KING_VAR(double, g_fr64b678, 64b678)
KING_VAR(float, g_fr64c938, 64c938)
KING_VAR(double, g_fr64b670, 64b670)

// plane p (x, y, z, d) made unit
static void Fr_Unit(float* p)
{
    float L = (float)Vec3_Length((const Vec3*)p);
    Vec3 t;
    CallThis(Fr_DivAssign, p, &t, L);
    p[3] = (float)(p[3] / (double)L);
}

// how sphere s lies to plane p: -1 outside, 1 cut, 0 inside
static int Fr_Side(double dist, const float* s)
{
    if (dist < -(double)s[3])
        return -1;
    return dist < s[3] ? 1 : 0;
}

static double Fr_DistA(const float* p, const float* s) { return (double)s[0] * p[0] + (double)p[2] * s[2] + (double)s[1] * p[1] + p[3]; }

static Vec3 Fr_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_5b4fc0] which frustum planes of view s (its view matrix +2240 + 48 * +8224) sphere sp (x, y, z, r)
// cuts: bits 32 far, 1 near, 2, 16, 8 and 4 the sides; 0 when it is outside, 64 when inside, 31 for no radius
// (also kept at +8272)
int __fastcall View_SphereClip(BYTE* s, void*, const float* sp)
{
    if (!(g_fr702a90 & 1))
    {
        g_fr702a90 |= 1;
        g_fr702990 = x87_sqrt(g_fr64b660) * g_fr64b6e0;
    }
    if (sp[3] == g_fr64b678)
    {
        F(int, s, 8272) = 31;
        return 31;
    }
    int flags = 0;
    F(int, s, 8272) = 0;
    Matrix tm;
    Matrix_Transposed(&tm, (const Matrix*)(s + 2240 + F(int, s, 8224) * 48));
    const float* T = tm.m;
    double nearZ, farZ;
    if (!g_device)
    {
        farZ = F(double, s, 8472) / F(double, s, 8464);
        nearZ = 1.0;
    }
    else
    {
        nearZ = F(double, s, 8464);
        farZ = F(double, s, 8472);
    }
    float p[4];
    *(Vec3*)p = Fr_Set((double)T[3] * g_fr64c938, (double)T[4] * g_fr64c938, (double)T[5] * g_fr64c938);
    p[3] = (float)(farZ - T[10] * g_fr64b670);
    Fr_Unit(p);
    int r = Fr_Side((double)sp[0] * p[0] + (double)p[2] * sp[2] + (double)sp[1] * p[1] + p[3], sp);
    if (r < 0)
        return 0;
    if (r)
        flags = 32;
    p[0] = T[3];
    p[1] = T[4];
    p[2] = T[5];
    p[3] = (float)(T[10] - nearZ);
    Fr_Unit(p);
    r = Fr_Side((double)sp[0] * p[0] + (double)p[2] * sp[2] + (double)sp[1] * p[1] + p[3], sp);
    if (r < 0)
        return 0;
    if (r)
        flags |= 1;
    float c = (float)g_fr702990, cc = c;
    *(Vec3*)p = Fr_Set((double)c * T[0] + (double)cc * T[3], (double)T[1] * c + (double)cc * T[4], (double)T[2] * c + (double)cc * T[5]);
    p[3] = (float)((double)T[9] * c + (double)T[10] * cc);
    Fr_Unit(p);
    r = Fr_Side(Fr_DistA(p, sp), sp);
    if (r < 0)
        return 0;
    if (r)
        flags |= 2;
    c = (float)-g_fr702990;
    cc = (float)g_fr702990;
    *(Vec3*)p = Fr_Set((double)c * T[0] + (double)cc * T[3], (double)T[1] * c + (double)cc * T[4], (double)T[2] * c + (double)cc * T[5]);
    p[3] = (float)((double)T[9] * c + (double)T[10] * cc);
    Fr_Unit(p);
    r = Fr_Side(Fr_DistA(p, sp), sp);
    if (r < 0)
        return 0;
    if (r)
        flags |= 16;
    c = cc = (float)g_fr702990;
    *(Vec3*)p = Fr_Set((double)cc * T[6] + (double)c * T[3], (double)cc * T[7] + (double)c * T[4], (double)c * T[8] + (double)c * T[5]);
    p[3] = (float)((double)T[11] * cc + (double)T[10] * c);
    Fr_Unit(p);
    r = Fr_Side(Vec3_Dot((const Vec3*)p, (const Vec3*)sp) + p[3], sp);
    if (r < 0)
        return 0;
    if (r)
        flags |= 8;
    Vec3 q = Fr_Set(0, g_fr702990, -g_fr702990), t;
    float d = (float)Vec3_Dot(&q, (const Vec3*)&T[9]);
    Vec3 rq = *Vec3_RotateBy(&t, &q, &tm);
    CallThis(Fr_RectSet, p, *(DWORD*)&rq.x, *(DWORD*)&rq.y, *(DWORD*)&rq.z, *(DWORD*)&d);
    Fr_Unit(p);
    r = Fr_Side(Vec3_Dot((const Vec3*)p, (const Vec3*)sp) + p[3], sp);
    if (r < 0)
        return 0;
    if (r)
        flags |= 4;
    F(int, s, 8272) = flags;
    return flags ? flags : 64;
}

} // extern "C"

// === a clipped edge's new vertex (FUN_5aebc0)

KING_VAR(double, g_ce64b670, 64b670)

extern "C" {

// [FUN_5aebc0] clipper s: a new vertex (index +1932, given back in *out) where the edge from vertex i to j
// (+1224, 12 bytes each) crosses clip plane p (1 far, 2 / 16 the sides, 4 / 8 above and below; others keep
// the start's y as the fraction), with its texture coordinates (+1488 / +1576, flag 32) and shade (+1664,
// flag 128) blended alike
void __fastcall Clip_EdgeVertex(BYTE* s, void*, int flags, int* out, int i, int j, int p)
{
    *out = F(int, s, 1932);
    const float* A = (const float*)(s + 1224 + i * 12);
    const float* B = (const float*)(s + 1224 + j * 12);
    float ax = A[0], ay = A[1], az = A[2];
    float dx = (float)((double)B[0] - ax), dy = (float)((double)B[1] - ay), dz = (float)((double)B[2] - az);
    double t;
    switch (p)
    {
    case 1:
        t = (g_ce64b670 - ay) / dy;
        break;
    case 2:
        t = -(((double)ay + ax) / ((double)dy + dx));
        break;
    case 4:
        t = -(((double)ay - az) / ((double)dy - dz));
        break;
    case 8:
        t = -(((double)az + ay) / ((double)dz + dy));
        break;
    case 16:
        t = -(((double)ay - ax) / ((double)dy - dx));
        break;
    default:
        t = ay;
        break;
    }
    float* V = (float*)(s + 1224 + F(int, s, 1932) * 12);
    V[0] = (float)(t * dx + ax);
    V[1] = (float)(t * dy + ay);
    V[2] = (float)(t * dz + az);
    if (flags & 32)
    {
        float* u = (float*)(s + 1488);
        u[F(int, s, 1932)] = (float)(((double)u[j] - u[i]) * t + u[i]);
        float* v = (float*)(s + 1576);
        v[F(int, s, 1932)] = (float)(((double)v[j] - v[i]) * t + v[i]);
    }
    if (flags & 128)
    {
        float* c = (float*)(s + 1664);
        c[F(int, s, 1932)] = (float)(((double)c[j] - c[i]) * t + c[i]);
    }
    F(int, s, 1932)++;
}

} // extern "C"

// === a camera-facing strip of quads at a point (FUN_5b3010)

extern "C" void Sp_Dispatch(); KING_ALIAS("_Sp_Dispatch", "@Draw_Dispatch@28")
extern "C" void Sp_TransformPoint(); KING_ALIAS("_Sp_TransformPoint", "@Vec3_TransformPoint@12")
extern "C" void Sp_AddAssign(); KING_ALIAS("_Sp_AddAssign", "@Vec3_AddAssign@16")
extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
KING_VAR(BYTE, g_sp702a08, 702a08)
KING_VAR(AsmLabel, g_spVerts, 702a98)            // 32-byte vertices
KING_VAR(int, g_sp723a88, 723a88)
KING_VAR(float, g_sp64c250, 64c250)

static Vec3 Sp_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

static void __cdecl Sp_AtexitNop() { Ren5b3990_Nop(0); }

extern "C" {

// [FUN_5b3010] renderer s draws object o's strip (+4 count, offsets along up +20 and side +16, along the view +24,
// texture +8 / +12 with flag 2, material +36 or +32) at point p turned to face the camera (flag 0x100 keeps it
// upright): through the device as a fan of 32-byte vertices tinted by the palette or alpha, else into the clipper
// (+1224..) and drawn by Draw_Dispatch
void __fastcall Sprite_DrawStrip(BYTE* s, void*, BYTE* o, float px, float py, float pz)
{
    Vec3 side = g_zeroVec, fwd = g_zeroVec, tmp;
    if (!F(BYTE*, s, 1948) || !Window_RootSurface(F(BYTE*, s, 1948), 0))
        return;
    BYTE* mat = F(BYTE*, o, 36) ? F(BYTE*, o, 36) : F(BYTE*, s, 8260) + F(short, o, 32) * 124;
    float alpha = 1.0f;
    if (mat[32] & 32)
        alpha = (float)((double)g_sp64c250 - F(float, mat, 88));
    if (!(g_sp702a08 & 1))
    {
        g_sp702a08 |= 1;
        atexit(Sp_AtexitNop);
    }
    Vec3 up;
    if (g_device)
    {
        const float* Fv = (const float*)(s + 2240 + F(int, s, 8224) * 48);
        double depth = (double)pz * Fv[7] + (double)px * Fv[1] + (double)py * Fv[4] + Fv[10];
        if (!(depth > g_sp64c250) || !(depth < F(double, s, 8472)))
            return;
        up = g_axisZ;
        Matrix m1, inv;
        Matrix_Inverse(&inv, Matrix_Multiply(&m1, (const Matrix*)(s + 5216 + F(int, s, 8236) * 48), (const Matrix*)(s + 4208)));
        const float* I = inv.m;
        const float* Y = &g_axisY.x;
        Vec3 f = Sp_Set((double)Y[2] * I[6] + (double)Y[1] * I[3] + (double)Y[0] * I[0],
                        (double)Y[2] * I[7] + (double)Y[1] * I[4] + (double)Y[0] * I[1],
                        (double)Y[2] * I[8] + (double)Y[1] * I[5] + (double)Y[0] * I[2]);
        fwd = *Vec3_Normalized(&tmp, &f);
        Vec3 c = Sp_Set((double)fwd.y * up.z - (double)fwd.z * up.y, (double)fwd.z * up.x - (double)fwd.x * up.z,
                        (double)fwd.x * up.y - (double)fwd.y * up.x);
        if (Vec3_Length(&c) < g_cp64b748)
            side = g_axisX;
        else
            side = *Vec3_Normalized(&tmp, &c);
        if (o[1] & 1)
        {
            Vec3 u = Sp_Set((double)side.y * fwd.z - (double)fwd.y * side.z, (double)fwd.x * side.z - (double)fwd.z * side.x,
                            (double)fwd.y * side.x - (double)side.y * fwd.x);
            up = *Vec3_Normalized(&tmp, &u);
        }
    }
    else
    {
        float pt[3] = {px, py, pz};
        CallThis(Sp_TransformPoint, pt, s + 2240 + F(int, s, 8224) * 48);
        px = pt[0];
        py = pt[1];
        pz = pt[2];
        if (!(py > g_sp64c250) || !(py < F(double, s, 8472)))
            return;
        const float* Z = &g_axisZ.x;
        up = Sp_Set((double)Z[2] * F(float, s, 4232) + (double)Z[1] * F(float, s, 4220) + (double)Z[0] * F(float, s, 4208),
                    (double)Z[2] * F(float, s, 4236) + (double)Z[1] * F(float, s, 4224) + (double)Z[0] * F(float, s, 4212),
                    (double)Z[2] * F(float, s, 4240) + (double)Z[1] * F(float, s, 4228) + (double)Z[0] * F(float, s, 4216));
        const float* Y = &g_axisY.x;
        Vec3 c = Sp_Set((double)up.z * Y[1] - (double)up.y * Y[2], (double)up.x * Y[2] - (double)up.z * Y[0],
                        (double)up.y * Y[0] - (double)up.x * Y[1]);
        side = *Vec3_Normalized(&tmp, &c);
    }
    const BYTE* pal = (const BYTE*)Window_RootSurface(F(BYTE*, s, 1948), 0) + 24 + 3 * F(int, mat, 40);
    int n = F(WORD, o, 4);
    int i = 0, count = 0;
    BYTE* vb = (BYTE*)&g_spVerts;
    for (; i < n; i++)
    {
        if (g_device)
        {
            float b = F(float*, o, 20)[i];
            float ax = (float)(b * (double)up.x), ay = (float)(b * (double)up.y);
            double az = b * (double)up.z;
            float c2 = F(float*, o, 16)[i];
            float bx = (float)(c2 * (double)side.x), by = (float)((double)side.y * c2);
            double bz = c2 * (double)side.z;
            float qx = (float)((double)bx + px), qy = (float)((double)by + py);
            double qz = bz + pz;
            Vec3 v = Sp_Set((double)qx + ax, (double)ay + qy, az + qz);
            if (F(float*, o, 24))
            {
                float c3 = F(float*, o, 24)[i];
                Vec3 w = Sp_Set(c3 * (double)fwd.x, (double)fwd.y * c3, (double)fwd.z * c3);
                CallThis(Sp_AddAssign, &v, &tmp, &w);
            }
            BYTE* V = vb + i * 32;
            F(Vec3, V, 0) = v;
            if (o[0] & 2)
            {
                F(float, V, 24) = F(float*, o, 8)[i];
                F(float, V, 28) = F(float*, o, 12)[i];
            }
            DWORD color;
            if (F(DWORD, mat, 32) & 1)
            {
                if (F(DWORD, mat, 32) & 0x1000)
                {
                    DWORD r = (DWORD)(int)(pal[0] * g_mt651758 * g_q161_64eba0);
                    DWORD a = (DWORD)(int)(alpha * (double)g_255f);
                    color = r | (a << 8);
                    color = (color << 8) | (DWORD)(int)(pal[1] * g_mt651758 * g_q161_64eba0);
                    color = (color << 8) | (DWORD)(int)(pal[2] * g_mt651758 * g_q161_64eba0);
                }
                else
                    color = ((DWORD)(int)(alpha * (double)g_255f) << 24) | 0xffffff;
            }
            else
            {
                const BYTE* p2 = (const BYTE*)Window_RootSurface(F(BYTE*, s, 1948), 0) + 24 + 3 * F(int, mat, 40);
                DWORD c0 = p2[0], c1 = p2[1], c2b = p2[2];
                color = (c2b * 255) | ((c1 * 255) << 8) | ((0u - c0) << 16) | 0xff000000;
            }
            F(DWORD, V, 16) = color;
            F(DWORD, V, 20) = 0xff000000;
            count = i + 1;
        }
        else
        {
            int k = g_sp723a88 ? n - i - 1 : i;
            float a = F(float*, o, 20)[i], b = F(float*, o, 16)[i];
            const float* sc = (const float*)(s + 4256 + F(int, s, 8228) * 48);
            float* cv = (float*)(s + 1224 + k * 12);
            cv[0] = (float)(((double)up.x * a + (double)side.x * b) * sc[0] + px);
            cv[1] = (float)((double)up.y * a + py);
            cv[2] = (float)(((double)up.z * a + (double)side.z * b) * sc[8] + pz);
            if (o[0] & 2)
            {
                F(float, s, 1488 + k * 4) = F(float*, o, 8)[i];
                F(float, s, 1576 + k * 4) = F(float*, o, 12)[i];
            }
            F(int, s, 1756 + k * 4) = k;
            count = k;
        }
    }
    F(int, s, 1932) = i;
    if (g_720904)
    {
        if (!Window_RootSurface(F(BYTE*, s, 1948), 0))
            return;
        CallThis(FUN_5b1020, s, mat, 0, 0, 0);
        Com(g_device, 100, 6, 482, vb, count, 0);
        return;
    }
    DWORD flags = 0;
    if ((F(DWORD, mat, 32) & 1) && F(int, F(BYTE*, mat, 76), 8))
        flags = 35;
    if (flags & 32)
    {
        BYTE* t = F(BYTE*, mat, 76);
        if (t[52] & 16)
        {
            flags |= 0x100;
            float val = (float)(F(WORD, t, 20) * (double)F(float, s, 1752) / ((double)py * F(float, o, 28)));
            DWORD vbits;
            memcpy(&vbits, &val, 4);
            CallThis(Sp_Dispatch, s, mat, flags, 0, vbits, t);
            return;
        }
        CallThis(Sp_Dispatch, s, mat, flags, 0, 0, t);
        return;
    }
    CallThis(Sp_Dispatch, s, mat, flags, 0, 0, 0);
}

} // extern "C"

// === an object's lit mesh (FUN_5b2950)

extern "C" void Rm_Nop1(); KING_ALIAS("_Rm_Nop1", "@Fw_Nop1@12")
extern "C" void Rm_Dispatch(); KING_ALIAS("_Rm_Dispatch", "@Draw_Dispatch@28")
KING_VAR(BYTE, g_rm702dd0, 702dd0)
KING_VAR(AsmLabel, g_rm702a80, 702a80)
KING_VAR(float, g_rm64c458, 64c458)
KING_VAR(double, g_rm64eba0, 64eba0)
KING_VAR(double, g_rm64b678, 64b678)

static void __cdecl Rm_AtexitNop() { Ren5b3000_Nop(0); }

// V's 3x3 part times v (x = v0 V0 + v1 V3 + v2 V6 ...)
static void Rm_Turn(const float* V, const float* v, float* out)
{
    out[0] = (float)((double)V[0] * v[0] + (double)V[6] * v[2] + (double)V[3] * v[1]);
    out[1] = (float)((double)V[7] * v[2] + (double)V[4] * v[1] + (double)V[1] * v[0]);
    out[2] = (float)((double)V[8] * v[2] + (double)V[5] * v[1] + (double)V[2] * v[0]);
}

static void Rm_Point(const float* V, const float* v, float* out)
{
    out[0] = (float)((double)V[6] * v[2] + (double)V[3] * v[1] + (double)V[0] * v[0] + V[9]);
    out[1] = (float)((double)V[7] * v[2] + (double)V[4] * v[1] + (double)V[1] * v[0] + V[10]);
    out[2] = (float)((double)V[8] * v[2] + (double)V[5] * v[1] + (double)V[2] * v[0] + V[11]);
}

extern "C" {

// [FUN_5b2950] renderer s draws mesh object o (flags +0: 1 lit, 4 two-sided, 16 per-vertex, 128 a strip; count +4,
// indexes +8, normals +24, colours +20, centre +32 / radius +44, material +48): straight from its vertex buffers
// (+76 lit / +80) in device mode 1, else its vertices into the clipper with their light (+1664 ..) and drawn
void __fastcall Mesh_Draw(BYTE* s, void*, BYTE* o)
{
    if (!(g_rm702dd0 & 1))
    {
        g_rm702dd0 |= 1;
        memset(&g_rm702a80, 0, 16);
        atexit(Rm_AtexitNop);
    }
    BYTE* mat = F(BYTE*, s, 8260) + F(short, o, 48) * 124;
    bool lit = (o[0] & 1) && (g_702e8c & 64);
    if (F(int, s, 8620) == 1)
    {
        if (!Window_RootSurface(F(BYTE*, s, 1948), 0))
            return;
        Window_RootSurface(F(BYTE*, s, 1948), 0);
        Render_Entry(s, 0, F(short, o, 48), lit ? 1 : 0, 0);
        if (o[0] & 4)
            Com(g_device, 80, 22, 3);
        int type = (o[0] & 128) ? 5 : 6;
        Com(g_device, 80, 136, F(int, s, 8272) ? 1 : 0);
        void* vb = F(void*, o, lit ? 76 : 80);
        if (vb)
            Com(g_device, 100, type, lit ? 274 : 482, vb, (int)F(WORD, o, 4), 0);
        if (o[0] & 4)
            Com(g_device, 80, 22, 2);
        if (o[0] & 128)
        {
            F(int, s, 8352) += F(WORD, o, 4) - 2;
            F(int, s, 8384)++;
        }
        return;
    }
    if (F(int, s, 8620) == 2)
    {
        if (!Window_RootSurface(F(BYTE*, s, 1948), 0))
            return;
        Render_Entry(s, 0, F(short, o, 48), lit ? 1 : 0, 0);
    }
    Matrix V = F(Matrix, s, 5216 + F(int, s, 8236) * 48);
    const float* C = (const float*)(s + 3236 + F(int, s, 8224) * 48);
    double depth = (double)F(float, o, 40) * C[2] + (double)F(float, o, 36) * C[1] + (double)F(float, o, 32) * C[0] + F(float, o, 44);
    if (depth < g_rm64b678)
        return;
    int fog = 0, flg = 0;
    if (Window_RootSurface(F(BYTE*, s, 1948), 0))
    {
        WORD w = F(WORD, o, 0);
        if (w & 16)
        {
            fog = 0;
            flg = 128;
        }
        else if (lit)
        {
            double L;
            if (F(int, s, 8240) && F(int, s, 8448) == 2)
            {
                L = g_rm64b678;
                flg = 128;
            }
            else if (F(int, s, 8240) && F(int, s, 8448) == 1)
            {
                float p[3];
                Rm_Turn(V.m, (const float*)(o + 32), p);
                L = Lights_AtPoint(s, 0, (int)p, F(double, s, 8512));
            }
            else
                L = F(double, s, 8512);
            fog = (int)(g_rm64eba0 - L * g_rm64eba0);
            if (fog < 0)
                fog = 0;
            else if (fog > 255)
                fog = 255;
        }
        else
            fog = (int)(g_rm64c458 - F(float, o, 28) * (double)g_rm64c458);
    }
    int n = F(WORD, o, 4);
    int i = 0;
    for (; i < n; i++)
    {
        const float* v = (const float*)(F(BYTE*, s, 8252) + F(WORD*, o, 8)[i] * 28);
        CallThis(Rm_Nop1, s, v);
        if (Window_RootSurface(F(BYTE*, s, 1948), 0))
        {
            WORD w = F(WORD, o, 0);
            float* light = &F(float, s, 1664 + 4 * i);
            if (w & 16)
            {
                if ((w & 1) && (g_702e8c & 64))
                {
                    if (F(int, s, 8240) < 1)
                        *light = (float)F(double, s, 8512);
                    else
                    {
                        float nrm[3], pos[3];
                        Rm_Turn(V.m, (const float*)(F(BYTE*, o, 24) + 12 * i), nrm);
                        Rm_Point(V.m, v, pos);
                        *light = (float)Lights_At(s, 0, (int)pos, (int)nrm, F(double, s, 8512));
                    }
                }
                else
                    F(int, s, 1664 + 4 * i) = F(int*, o, 20)[i];
            }
            else if (F(int, s, 8448) == 2 && (w & 1) && (g_702e8c & 64))
            {
                float p[3], pos[3];
                Rm_Turn(V.m, (const float*)(o + 32), p);
                Rm_Point(V.m, v, pos);
                *light = (float)Lights_At(s, 0, (int)pos, (int)p, F(double, s, 8512));
            }
        }
        F(int, s, 1756 + 4 * i) = i;
    }
    F(int, s, 1932) = i;
    CallThis(Rm_Dispatch, s, mat, flg, fog, 0, 0);
}

} // extern "C"

// === a portal's view (FUN_5b5710)

extern "C" void __fastcall Rp_Transform(Matrix* s, void*, const Matrix* by); KING_ALIAS("@Rp_Transform@12", "@Matrix_Transform2@12")
KING_VAR(double, g_rp64b5b8, 64b5b8)
KING_VAR(float, g_rp64c938, 64c938)                   // -1
KING_VAR(float, g_rp64c258, 64c258)                   // 0.5
KING_VAR(float, g_rp64c40c, 64c40c)                   // 2
KING_VAR(float, g_rp64c878, 64c878)                   // -0.5

// the view rectangle +1952..+1964 recorded at slot +8232 (+1968 / +2048 / +2128 / +2208), which is then used
static int Rp_Record(BYTE* r)
{
    int k = F(int, r, 8232);
    F(int, r, 1968 + 4 * k) = F(int, r, 1952);
    F(int, r, 2048 + 4 * k) = F(int, r, 1956);
    F(int, r, 2128 + 4 * k) = F(int, r, 1960);
    F(int, r, 2208 + 4 * k) = F(int, r, 1964);
    F(int, r, 8232) = k + 1;
    return 1;
}

extern "C" {

// [FUN_5b5710] renderer r looks through the portal quad q (four points) and, for a mirror, reflected in its
// plane: the corners put in view space (+1224) and clipped to the view (ids +1756, count +1932); none left, or
// their screen box outside it, gives 0; else a new view matrix (+4304 stack, +8228) and frame (+5216, +8236)
// pushed and the view rectangle narrowed to the box, 1; a corner behind the eye keeps the whole view
int __fastcall Render_PortalView(BYTE* r, void*, const float* q, int mirror)
{
    Matrix A = g_identity, B = g_identity;
    const float* M = (const float*)(r + 2240 + 48 * F(int, r, 8224));
    for (int i = 0; i < 4; i++)
    {
        const float* p = q + 3 * i;
        float* o = (float*)(r + 1224 + 12 * i);
        o[0] = (float)((double)p[2] * M[6] + (double)p[1] * M[3] + (double)p[0] * M[0] + M[9]);
        o[1] = (float)((double)M[7] * p[2] + (double)M[1] * p[0] + (double)M[4] * p[1] + M[10]);
        o[2] = (float)((double)M[8] * p[2] + (double)M[2] * p[0] + (double)M[5] * p[1] + M[11]);
        F(int, r, 1488 + 4 * i) = 0;
        F(int, r, 1576 + 4 * i) = 0;
        F(int, r, 1664 + 4 * i) = 0;
        F(int, r, 1756 + 4 * i) = i;
    }
    if (mirror)
    {
        // the reflection: into the plane's frame
        double a = (double)q[6] - q[3], b = (double)q[7] - q[4], c = (double)q[8] - q[5];
        float d0 = (float)((double)q[0] - q[3]), d1 = (float)((double)q[1] - q[4]);
        double d2 = (double)q[2] - q[5];
        Vec3 n;
        n.x = (float)(c * d1 - b * d2);
        n.y = (float)(d2 * a - c * d0);
        n.z = (float)(b * d0 - d1 * a);
        float az, po;
        Vec3_ToAngles(n, &az, &po);
        Vec3 t;
        t.x = -q[0];
        t.y = -q[1];
        t.z = -q[2];
        Matrix RX, RZ, Tr, X;
        Matrix_RotationX(&RX, -po);
        Matrix_RotationZ(&RZ, -az);
        Matrix_Multiply(&X, Matrix_Translation(&Tr, t), &RZ);
        Matrix_Multiply(&B, &X, &RX);
    }
    F(int, r, 1932) = 4;
    int* ids = (int*)(r + 1756);
    int* tmp = (int*)(r + 1844);
    int n = Rn_ClipIds(r, 0, 0, 4, tmp, ids, 4);
    n = Rn_ClipIds(r, 0, 0, n, ids, tmp, 16);
    n = Rn_ClipIds(r, 0, 0, n, tmp, ids, 8);
    n = Rn_ClipIds(r, 0, 0, n, ids, tmp, 2);
    if (!n)
        return 0;
    F(int, r, 1932) = n;
    float x0 = 1.0f, x1 = -1.0f, y0 = 1.0f, y1 = -1.0f;
    for (int i = 0; i < n; i++)
    {
        const float* v = (const float*)(r + 1224 + 12 * ids[i]);
        if (v[1] < g_rp64b5b8)
        {
            // behind the eye: the whole view
            int k = F(int, r, 8228);
            memcpy(r + 4304 + 48 * k, r + 4256 + 48 * k, 48);
            F(int, r, 8228) = k + 1;
            Render_PushMatrix(r, 0, (const BYTE*)&g_identity, 1);
            return Rp_Record(r);
        }
        double x = v[0] / (double)v[1];
        float y = (float)(v[2] / (double)v[1]);
        if (x0 > x)
            x0 = (float)x;
        if (x1 < x)
            x1 = (float)x;
        if (y0 > y)
            y0 = y;
        if (y1 < y)
            y1 = y;
    }
    if (!(x0 <= g_rp64c250) || !(y0 <= g_rp64c250) || x1 < g_rp64c938 || y1 < g_rp64c938)
        return 0;
    if (x0 < g_rp64c938)
        x0 = -1.0f;
    if (x1 > g_rp64c250)
        x1 = 1.0f;
    if (y0 < g_rp64c938)
        y0 = -1.0f;
    if (y1 > g_rp64c250)
        y1 = 1.0f;
    if (!(x0 < x1) || !(y0 < y1))
        return 0;
    // the box's view: centred and scaled to fill it
    double cx = ((double)x1 + x0) * g_rp64c258;
    float cy = (float)(((double)y1 + y0) * g_rp64c258);
    Matrix P;
    memset(&P, 0, sizeof(P));
    P.m[0] = 1.0f;
    P.m[3] = (float)-cx;
    P.m[4] = 1.0f;
    P.m[5] = -cy;
    P.m[8] = 1.0f;
    A = P;
    float w = (float)((double)x1 - x0);
    float h = (float)((double)y1 - y0);
    Vec3 sc;
    sc.x = (float)(g_rp64c40c / (double)w);
    sc.y = 1.0f;
    sc.z = (float)(g_rp64c40c / (double)h);
    Matrix S;
    Rp_Transform(&A, 0, Matrix_Scale(&S, sc));
    if (F(int, r, 8228) >= 18 || F(int, r, 8224) >= 18 || F(int, r, 8236) >= 18)
        return 0;
    int k = F(int, r, 8228);
    Matrix V;
    Matrix_Multiply(&V, (const Matrix*)(r + 4256 + 48 * k), &A);
    memcpy(r + 4304 + 48 * k, &V, 48);
    F(int, r, 8228) = k + 1;
    const Matrix* fr = (const Matrix*)(r + 5216 + 48 * F(int, r, 8236));
    if (mirror)
    {
        Matrix F2, I, X, Y, Z;
        memset(&F2, 0, sizeof(F2));
        F2.m[0] = -1.0f;
        F2.m[4] = 1.0f;
        F2.m[8] = 1.0f;
        Matrix_Inverse(&I, &B);
        Matrix_Multiply(&X, &B, &F2);
        Matrix_Multiply(&Y, &X, &I);
        B = Y;
        Render_PushMatrix(r, 0, (const BYTE*)Matrix_Multiply(&Z, fr, &B), 0);
    }
    else
        Render_PushMatrix(r, 0, (const BYTE*)fr, 0);
    // the view rectangle narrowed to the box
    int W = F(int, r, 1960), H = F(int, r, 1964);
    int bx = (int)(((double)x0 + g_rp64c250) * W);
    float Hf = (float)H;
    int by = (int)(((double)g_rp64c250 - y1) * Hf);
    int bw = 32768 - (int)((double)W * w * g_rp64c878);
    int bh = 32768 - (int)((double)Hf * h * g_rp64c878);
    if (bx + bw * 2 > 2 * W)
        bw = W - (bx >> 1);
    if (by + bh * 2 > 2 * H)
        bh = H - (by >> 1);
    F(int, r, 1964) = bh;
    F(int, r, 1952) += bx;
    F(int, r, 1960) = bw;
    F(int, r, 1956) += by;
    return Rp_Record(r);
}

} // extern "C"

// === a texture's rows loaded (FUN_5bb260)

extern "C" void Tx_Fade(); KING_ALIAS("_Tx_Fade", "_Pixels4444_Fade")
extern "C" void Tx_Read(); KING_ALIAS("_Tx_Read", "@Mmio_Read@20")
KING_VAR(int, g_tx7208c4, 7208c4)
KING_VAR(float, g_tx65177c, 65177c)
KING_VAR(float, g_tx651778, 651778)
KING_VAR(float, g_tx651774, 651774)
KING_VAR(float, g_tx651770, 651770)
KING_VAR(double, g_tx64d900, 64d900)
KING_VAR(float, g_tx64d3fc, 64d3fc)
KING_VAR(float, g_tx64dab0, 64dab0)
KING_VAR(float, g_tx64ca90, 64ca90)
KING_VAR(float, g_tx64c32c, 64c32c)
KING_VAR(double, g_tx64b780, 64b780)

extern "C" {

// [FUN_5bb260] texture s (format +64, row bytes per pixel +24, file +80 at +32) fills rows rows of w pixels at dst
// (pitch dpitch) from src (pitch spitch; an offset in the file when fromFile), faded by k; a format-8 texture in
// the water mode 7208c4 is instead drawn as two bytes per pixel of waves at time k
void __fastcall Texture_LoadRows(BYTE* s, void*, BYTE* dst, int dpitch, BYTE* src, int spitch, int rows, int w,
                                 double k, int fromFile)
{
    int fmt = F(int, s, 64);
    int mode = fmt == 2 ? 1 : fmt == 8 ? 2 : 0;
    if (mode == 2 && g_tx7208c4)
    {
        if (rows <= 0)
            return;
        float rf = (float)rows;
        double base = k;
        for (int r = 0; r < rows; r++, dst += dpitch)
        {
            float v = (float)(r / (double)rf + base);
            if (w <= 0)
                continue;
            float wf = (float)w;
            double a = v * (double)g_tx65177c * g_tx64d900;
            double b = v * (double)g_tx651778 * g_tx64d900;
            double sb = x87_sin(b), sa = x87_sin(a), cb = x87_cos(b), ca = x87_cos(a);
            BYTE* p = dst;
            for (int c = 0; c < w; c++)
            {
                double u = c / (double)wf + base;
                float t28 = (float)(u * g_tx651774 * g_tx64d900);
                float uv = (float)(u + v);
                float t32 = (float)(uv * (double)g_tx64d3fc * g_tx64d900);
                float vu = (float)((double)v - u);
                float tp = (float)(vu * (double)g_tx651770 * g_tx64d900);
                double U1 = u * g_tx64dab0 * g_tx64d900;
                double U2 = uv * (double)g_tx64ca90 * g_tx64d900;
                double U3 = vu * (double)g_tx64c32c * g_tx64d900;
                double s1 = x87_cos(U3) + x87_sin(U2) + x87_cos(U1) + x87_cos(tp) + x87_sin(t32) + x87_cos(t28) + sa + sb;
                p[0] = (BYTE)(int)(s1 * g_tx64b780);
                double s2 = x87_cos(U2) + x87_sin(U3) + x87_sin(U1) + x87_sin(tp) + x87_cos(t32) + x87_sin(t28) + ca + cb;
                p[1] = (BYTE)(int)(s2 * g_tx64b780);
                p += 2;
            }
        }
        return;
    }
    int row = F(WORD, s, 24) * w;
    if (dpitch == spitch && dpitch == row)
    {
        if (!fromFile)
            memcpy(dst, src, dpitch * rows);
        else
            CallThis(Tx_Read, F(BYTE*, s, 80), F(int, s, 32) + (int)src, dst, spitch * rows);
        CallC(Tx_Fade, dst, rows * w, k, mode);
        return;
    }
    for (int r = 0; r < rows; r++, dst += dpitch, src += spitch)
    {
        if (!fromFile)
            memcpy(dst, src, F(WORD, s, 24) * w);
        else
            CallThis(Tx_Read, F(BYTE*, s, 80), F(int, s, 32) + (int)src, dst, F(WORD, s, 24) * w);
        CallC(Tx_Fade, dst, w, k, mode);
    }
}

} // extern "C"

// === drawing a clipped textured polygon (FUN_5b3a40)

extern "C" void Rt_Blit(); KING_ALIAS("_Rt_Blit", "_Blit_WithAlpha")
KING_VAR(float, g_rt64da74, 64da74)
KING_VAR(double, g_rt64b678, 64b678)
KING_VAR(float, g_rt64c258, 64c258)
KING_VAR(float, g_rt64c458, 64c458)

extern "C" {

// [FUN_5b3a40] draws the polygon of renderer r's current vertices with texture parameters t (its texture +76
// unless flag 0x1000), flags, b and the distance factor dist: clipped as Render_Polygon does; with the device
// (+8620 == 2) as a D3D fan of TL vertices (+712) with u / v, else u / v moved to their smallest whole values, a
// mip level picked by the nearest depth (flag 0x100; a one-texel level drawn flat in its colour) and the screen
// vertices (+20, 44 bytes) drawn by Blit_WithAlpha
int __fastcall Render_TexPolygon(BYTE* r, void*, BYTE* t, DWORD flags, int b, float dist, int)
{
    BYTE P[124];
    memset(P, 0, sizeof P);
    F(int, P, 44) = -1;
    F(int, P, 52) = -1;
    F(float, P, 96) = 1.0f;
    F(float, P, 100) = 1.0f;
    P[120] = 238;
    int buf = 0;
    int n = F(int, r, 1932);
    DWORD clip = F(DWORD, r, 8272);
    if (clip) {
        static const DWORD bits[5] = {1, 4, 16, 8, 2};
        for (int k = 0; k < 5; k++)
            if (clip & bits[k]) {
                n = Rn_ClipIds(r, 0, flags, n, (int*)(r + 1756 + (buf ^ 1) * 88), (int*)(r + 1756 + buf * 88), bits[k]);
                buf ^= 1;
            }
    }
    n = CallVirt<int>(r, 12, flags, n, &buf);
    if (!n)
        return 0;
    const int* ids = (const int*)(r + 1756 + buf * 88);
    if (F(int, r, 8620) == 2) {
        Window_RootSurface(F(BYTE*, r, 1948), 0);
        double k = g_one64b670 / (F(double, r, 8472) - F(double, r, 8464));
        float* o = (float*)(r + 712);
        int lit = flags & 128;
        for (int j = 0; j < n; j++, o += 8) {
            int id = ids[j];
            const float* v = (const float*)(r + 1224 + id * 12);
            double w = g_one64b670 / v[1];
            double yw = w * v[2];
            o[0] = (float)((w * v[0] + g_one64b670) * F(int, r, 8556) + F(int, r, 8548));
            o[1] = (float)((g_one64b670 - yw) * F(int, r, 8560) + F(int, r, 8552));
            o[2] = (float)((v[1] - F(double, r, 8464)) * k);
            o[3] = (float)w;
            if (lit) {
                DWORD c = (DWORD)(int)(F(float, r, 1664 + id * 4) * (double)g_rt64c458);
                F(DWORD, o, 16) = (((c | 0xFFFFFF00) << 8 | c) << 8) | c;
            } else
                F(DWORD, o, 16) = 0xFFFFFFFF;
            F(DWORD, o, 20) = 0xFF000000;
            F(float, o, 24) = F(float, r, 1488 + id * 4);
            F(float, o, 28) = F(float, r, 1576 + id * 4);
        }
        Com(g_device, 100, 6, 3, r + 712, n, 0);
        return 0;
    }
    float zmin = 100000.0f, umin = 10000.0f, zmax = 0;
    double vmin = g_rt64da74;
    for (int j = 0; j < n; j++) {
        int id = ids[j];
        float z = F(float, r, 1228 + id * 12);
        if (z < zmin)
            zmin = z;
        if (z > zmax)
            zmax = z;
        if (F(float, r, 1488 + id * 4) < umin)
            umin = F(float, r, 1488 + id * 4);
        if (!(vmin > F(float, r, 1576 + id * 4)))
            continue;
        vmin = F(float, r, 1576 + id * 4);
    }
    umin = (float)(int)umin;
    float vmin2 = (float)(int)vmin;
    BYTE* tex = flags & 0x1000 ? t : F(BYTE*, t, 76);
    BYTE* par = t;
    int level = 0;
    if (flags & 0x100) {
        double lv;
        double q = (double)zmin * zmin * dist;
        if (q < g_rp64c250)
            lv = g_rt64b678;
        else if (!(q < (double)F(WORD, tex, 20)))
            lv = F(WORD, tex, 26);
        else {
            lv = g_rt64b678;
            while (q > g_rp64c250) {
                lv += g_one64b670;
                q *= g_rt64c258;
            }
            lv = lv - (g_rp64c250 - q);
        }
        double m = F(double, r, 8528);
        level = (int)(lv * (lv * m + lv * m + (g_one64b670 - m)));
        if (level > F(WORD, tex, 26))
            level = F(WORD, tex, 26);
    }
    int mw = Texture_MipWidth(tex, 0, level);
    if (mw <= 1) {
        Window_CopyParams(P, 0, t);
        BYTE* px = Texture_MipPixels(tex, 0, F(WORD, tex, 26));
        F(int, P, 40) = F(WORD, tex, 24) == 1 ? *px : *(WORD*)px;
        par = P;
        F(int, P, 32) = F(int, t, 32);
        flags &= ~32u;
    }
    if (mw >= 128)
        mw--;
    if (g_702e8c & 4) {
        if (zmin > F(float, r, 8536))
            return 0;
        if (zmax > F(float, r, 8540))
            flags |= 64;
    }
    for (int j = 0; j < n; j++) {
        int id = ids[j];
        F(float, r, 1488 + id * 4) = (float)((double)F(float, r, 1488 + id * 4) - umin);
        F(float, r, 1576 + id * 4) = (float)((double)F(float, r, 1576 + id * 4) - vmin2);
    }
    double xlo = -1.0, xhi = 1.0;
    float scale = (float)(mw << 16);
    int shade = flags & 128;
    for (int j = 0; j < n; j++) {
        BYTE* s = r + 20 + j * 44;
        int id = ids[j];
        const float* v = (const float*)(r + 1224 + id * 12);
        double w = g_one64b670 / v[1];
        float xw = (float)(w * v[0]);
        if (xw < xlo)
            xlo = xw;
        if (xw > xhi)
            xhi = xw;
        double yw = w * v[2];
        int hw = F(int, r, 1960);
        F(int, s, 0) = ((int)(hw * (double)xw) + F(int, r, 1952) + hw + 2048) & ~4095;
        int hh = F(int, r, 1964);
        F(int, s, 4) = (F(int, r, 1956) - (int)(hh * yw) + hh + 2048) & ~4095;
        F(int, s, 8) = (int)((double)g_rp64c250 / v[1] * g_rp651760);
        if (shade && Window_RootSurface(F(BYTE*, r, 1948), 0)) {
            int lv = F(int, (BYTE*)Window_RootSurface(F(BYTE*, r, 1948), 0), 804) - 2;
            F(int, s, 28) = (int)(lv * (g_rp64c250 - F(float, r, 1664 + id * 4)) + g_rp64c250) << 20;
        }
        F(int, s, 12) = (int)(scale * (double)F(float, r, 1488 + id * 4));
        F(int, s, 16) = (int)(scale * (double)F(float, r, 1576 + id * 4));
    }
    if (flags & 32) {
        if ((int)((int)((xhi - xlo) * F(int, r, 1960)) & 0xFFF00000) < 0x1800000)
            flags |= 16;
    }
    return CallC<int>(Rt_Blit, flags, F(int, r, 1948), n, r + 8, par, b, level);
}

} // extern "C"

// === the texture coordinates of a mesh's layers (FUN_5af3e0)

extern "C" void Lu_Fatal(); KING_ALIAS("_Lu_Fatal", "_Sys_Fatal")
extern "C" void Lu_Point(); KING_ALIAS("_Lu_Point", "@Vec3_TransformPoint@12")
extern "C" void Lu_Frame(); KING_ALIAS("_Lu_Frame", "@Render_CurrentFrame@8")
KING_VAR(BYTE, g_lu702a78, 702a78)
KING_VAR(BYTE, g_lu702dd8, 702dd8)
KING_VAR(BYTE, g_lu7025f0, 7025f0)
KING_VAR(Vec3, g_luUp, 702630)
KING_VAR(AsmLabel, g_lu67efa8, 67efa8)
KING_VAR(double, g_lu64b658, 64b658)
KING_VAR(float, g_lu64c250, 64c250)
KING_VAR(float, g_lu64c938, 64c938)
KING_VAR(double, g_lu64b748, 64b748)
KING_VAR(double, g_lu64b6e0, 64b6e0)
KING_VAR(double, g_lu651730, 651730)
KING_VAR(float, g_lu64c258, 64c258)
KING_VAR(float, g_lu64c264, 64c264)
KING_VAR(double, g_lu64d908, 64d908)
KING_VAR(double, g_lu64e4c8, 64e4c8)
KING_VAR(float, g_lu64d490, 64d490)
KING_VAR(float, g_lu64c334, 64c334)
KING_VAR(double, g_lu64d318, 64d318)

static void __cdecl Lu_AtExit()
{
}

extern "C" {

// [FUN_5af3e0] renderer r's texture coordinates for mesh m's vertices on layer l and the layers chained from it
// (+44 into r +8260): from channel +48 (the mesh's own +12 / +16, or the vertex records' +12 / +16 and +20 table),
// scrolled (flag 4, +80 / +84 by the clock +8488), turned about +112 / +116 (flag 8, +108) or environment-mapped
// (flag 0x800, kind +64: planar, spherical, half-spherical or flat; from the camera or, kind 3, the view matrix),
// into the mesh's channel table +68 or its vertices (+80, else +76)
void __fastcall Render_LayerUVs(BYTE* r, void*, BYTE* m, BYTE* l)
{
    for (;;)
    {
        if (!(g_lu702a78 & 1))
        {
            g_lu702a78 |= 1;
            memset(&g_lu702dd8, 0, 48);
            memcpy(&g_lu702dd8, &g_lu7025f0, 48);
            atexit(Lu_AtExit);
        }
        float du = 0, dv = 0, sn = 0, cs = 1.0f;
        int ch = F(int, l, 48);
        DWORD f = F(DWORD, l, 32);
        if ((f & 4) || (f & 8) || (f & 0x800))
        {
            if (f & 8)
            {
                double a = F(float, l, 108) * F(double, r, 8488);
                sn = (float)x87_sin(a);
                cs = (float)x87_cos(a);
            }
            else if (f & 4)
            {
                double u0 = F(float, l, 80) * F(double, r, 8488);
                float v0 = (float)(F(float, l, 84) * F(double, r, 8488));
                du = (float)(u0 - (int)u0);
                dv = (float)((double)v0 - (int)v0);
            }
            for (int i = 0; i < F(WORD, m, 4); i++)
            {
                double u, v;
                if (F(BYTE, m, 0) & 2)
                {
                    int idx = F(WORD, m, 6) * i + ch;
                    v = F(float*, m, 16)[idx];
                    u = F(float*, m, 12)[idx];
                }
                else
                {
                    BYTE* rec = F(BYTE*, r, 8252) + F(WORD*, m, 8)[i] * 28;
                    if (!ch)
                    {
                        v = F(float, rec, 16);
                        u = F(float, rec, 12);
                    }
                    else
                    {
                        if (!F(float*, rec, 20))
                            CallC(Lu_Fatal, (const char*)&g_lu67efa8, l);
                        rec = F(BYTE*, r, 8252) + F(WORD*, m, 8)[i] * 28;
                        const float* t = F(float*, rec, 20) + (ch - 1) * 2;
                        u = t[0];
                        v = t[1];
                    }
                }
                double u0 = u;
                f = F(DWORD, l, 32);
                if (f & 4)
                {
                    u = u + du;
                    v = dv + v;
                }
                else if (f & 8)
                {
                    float vf = (float)v;
                    double cx = F(float, l, 112), cy = F(float, l, 116);
                    double ux = u - cx;
                    v = (v - cy) * cs - sn * ux + cy;
                    u = (vf - cy) * sn + cs * ux + cx;
                }
                else if (f & 0x800)
                {
                    BYTE* vr = F(BYTE*, r, 8252) + F(WORD*, m, 8)[i] * 28;
                    float P[3] = {F(float, vr, 0), F(float, vr, 4), F(float, vr, 8)};
                    Vec3 U = g_luUp;
                    if (F(int, l, 64) == 3)
                        CallThis(Lu_Point, P, r + 2240 + F(int, r, 8224) * 48);
                    else
                    {
                        const float* c = CallThis<const float*>(Lu_Frame, r);
                        Vec3 d, nb;
                        d.x = (float)((double)c[0] - P[0]);
                        d.y = (float)((double)c[1] - P[1]);
                        d.z = (float)((double)c[2] - P[2]);
                        const Vec3* n = Vec3_Normalized(&nb, &d);
                        P[0] = n->x;
                        P[1] = n->y;
                        P[2] = n->z;
                        double ux2 = (double)U.x + U.x;
                        float uy2 = (float)((double)U.y + U.y), uz2 = (float)((double)U.z + U.z);
                        float dd = (float)((double)U.y * P[1] + (double)U.z * P[2] + (double)U.x * P[0]);
                        double rx = ux2 * dd;
                        float ry = (float)((double)uy2 * dd), rz = (float)((double)uz2 * dd);
                        P[0] = (float)(rx - P[0]);
                        P[1] = (float)((double)ry - P[1]);
                        P[2] = (float)((double)rz - P[2]);
                        if (x87_fabs(P[2]) < g_lu64b658)
                            *(DWORD*)&P[2] = 0x3A83126F;
                        else if (P[2] > g_lu64c250)
                            P[2] = 1.0f;
                        else if (P[2] < g_lu64c938)
                            P[2] = -1.0f;
                        if (P[0] < g_lu64c938)
                            P[0] = -1.0f;
                        else if (P[0] > g_lu64c250)
                            P[0] = 1.0f;
                    }
                    switch (F(int, l, 64))
                    {
                    case 0:
                    {
                        if (P[2] < g_lu64b748)
                            *(DWORD*)&P[2] = 0x3727C5AC;
                        double w = x87_fabs(P[2]);
                        u = F(float, l, 96) * (double)P[0] * g_lu64b6e0 / w + g_lu64b6e0;
                        v = F(float, l, 100) * (double)P[1] * g_lu64b6e0 / w + g_lu64b6e0;
                        break;
                    }
                    case 1:
                    case 2:
                        if (F(int, l, 64) == 1)
                            v = x87_acos(P[2]) * g_lu651730;
                        else
                            v = g_lu64c250 - P[2] * (double)g_lu64c258;
                        u = x87_acos(P[0]);
                        if (P[1] < g_lu64c264)
                            u = g_lu64d908 - u;
                        u *= g_lu64e4c8;
                        break;
                    case 3:
                        u = P[0] * (double)g_lu64d490 + g_lu64b6e0;
                        v = g_lu64d318 - P[2] * (double)g_lu64c334;
                        break;
                    default:
                        u = u0;
                        break;
                    }
                }
                if (ch > 0)
                {
                    int idx = F(BYTE, m, 64) * i + ch;
                    F(float*, m, 68)[idx * 2 - 2] = (float)u;
                    F(float*, m, 68)[idx * 2 - 1] = (float)v;
                }
                else
                {
                    BYTE* vb = F(BYTE*, m, 80) ? F(BYTE*, m, 80) : F(BYTE*, m, 76);
                    F(float, vb, i * 32 + 28) = (float)v;
                    F(float, vb, i * 32 + 24) = (float)u;
                }
            }
        }
        if (F(int, l, 44) < 0)
            return;
        l = F(BYTE*, r, 8260) + F(int, l, 44) * 124;
    }
}

} // extern "C"

// === a line's ends clipped against one edge of a window (FUN_5b7af0)

// the end of (x0, y0)-(x1, y1) beyond the edge moved onto it along the line: (a, b) the coordinate tested and
// (c, d) the other one, keep 1 keeping a >= e (else a <= e); 0 when some part is left, 1 when all of it is beyond
static int Ce_Clip(int e, int* a0, int* c0, int* a1, int* c1, bool keepAbove)
{
    int p = *a0, q = *a1;
    bool in0 = keepAbove ? p >= e : p <= e;
    bool in1 = keepAbove ? q >= e : q <= e;
    if (in0 && in1)
        return 0;
    if (!in0 && !in1)
        return 1;
    int oa, oc;    // the end beyond the edge
    if (keepAbove ? p < q : p > q)
    {
        oa = p;
        oc = *c0;
        *a0 = q;
        *c0 = *c1;
    }
    else
    {
        oa = q;
        oc = *c1;
    }
    *a1 = e;
    int ia = *a0;
    if (oa == ia)
        *c1 = *c0;
    else
        *c1 = (oc - *c0) * (e - ia) / (oa - ia) + *c0;
    return 0;
}

extern "C" {

// [FUN_5b7af0] line (x0, y0)-(x1, y1) clipped by edge kind k at e: 0 x >= e, 1 y >= e, 2 x <= e, 3 y <= e;
// 1 when the line is wholly outside (or k is unknown), else 0 with the outside end moved onto the edge
int __cdecl Line_ClipEdge(int k, int e, int* x0, int* y0, int* x1, int* y1)
{
    switch (k)
    {
    case 0:
        return Ce_Clip(e, x0, y0, x1, y1, true);
    case 1:
        return Ce_Clip(e, y0, x0, y1, x1, true);
    case 2:
        return Ce_Clip(e, x0, y0, x1, y1, false);
    case 3:
        return Ce_Clip(e, y0, x0, y1, x1, false);
    }
    return 1;
}

} // extern "C"
