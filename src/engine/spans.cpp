// spans.cpp - the software renderer's span loops (unit 0x5fdc70 of king.exe,
// hand-written assembly in the original): two selectors pick an inner loop
// for the pixel size (70f3d8), the mode and the z test, and two drawers set
// up the registers and run it on one span. The loops are translated
// instruction by instruction, including where the original writes a lookup
// result into the low bits of the z accumulator or keeps stale upper bits of
// a register, so the pixels come out the same.
//
// Registers of the original become fields of Span: eax/ebx = u/v (textured)
// or the colour (flat), ecx = light (flat) or the texture shift in its low
// byte (textured), edx = z in 16.16, ebp = light (textured, 12.20 row index).

#include "king.h"

KING_VAR(int, g_spBpp70f3d8, 70f3d8)            // bytes a pixel
KING_VAR(BYTE, g_spTexBits70f3dc, 70f3dc)       // log2 of the texture size
KING_VAR(BYTE**, g_spLightRows70f3e0, 70f3e0)   // light rows, one per 1/4096 of the light
KING_VAR(BYTE*, g_spFog71e7b0, 71e7b0)          // fog table
KING_VAR(BYTE*, g_spTexels71e7c8, 71e7c8)       // texture pixels
KING_VAR(BYTE*, g_spBlend71e7f0, 71e7f0)        // translucency table

struct Span {
    DWORD eax, ebx, ecx, edx, ebp;
    BYTE* dst;
    BYTE* end;
    WORD* zbuf;
    DWORD du, dv;   // 683c34, 683c38: u and v steps shifted to the top bits
    DWORD dl;       // 683c3c: light step
    DWORD dz;       // 683c40: z step
};

typedef void (*SpanLoop)(Span& s);

static SpanLoop s_spanLoop;   // 683c52

#define ZTEST(s) ((WORD)((s).edx >> 16) >= *(s).zbuf)
#define ZVAL(s) ((WORD)((s).edx >> 16))
#define DONE(s) ((int)(s).dst >= (int)(s).end)
#define LIGHTROW(l) (g_spLightRows70f3e0[(DWORD)(l) >> 20])
#define LO16(r, v) ((r) = ((r) & 0xFFFF0000) | (WORD)(v))
#define LO8(r, v) ((r) = ((r) & 0xFFFFFF00) | (BYTE)(v))

// shld esi,ebx,cl; shld esi,eax,cl with esi = 0: the top cl bits of v, then of u
static inline DWORD Texel(const Span& s)
{
    BYTE cl = (BYTE)s.ecx & 31;
    if (!cl)
        return 0;
    return ((s.ebx >> (32 - cl)) << cl) | (s.eax >> (32 - cl));
}
#define T16(s) (*(WORD*)(g_spTexels71e7c8 + Texel(s) * 2))
#define T8(s) (g_spTexels71e7c8[Texel(s)])
#define STEP_UV(s) ((s).eax += (s).du, (s).ebx += (s).dv)

static void Span_None(Span&)
{
}

// === 16-bit flat spans (selected by Span_SelectFlat)

// [$L_5fdc70] colour ax where z passes
static void Span_FlatZ16(Span& s)
{
    do {
        if (ZTEST(s)) {
            *(WORD*)s.dst = (WORD)s.eax;
            *s.zbuf = ZVAL(s);
        }
        s.dst += 2;
        s.edx += s.dz;
        s.zbuf++;
    } while (!DONE(s));
}

// [$L_5fdc9c] translucent colour through the table 71e7f0; the first pixel failing the z test drops into
// Span_FlatZ16's loop with the last blended value, as in the original
static void Span_BlendZ16(Span& s)
{
    BYTE* tbl = g_spBlend71e7f0 + (s.eax << 16);
    s.eax = 0;
    do {
        if (!ZTEST(s)) {
            s.dst += 2;
            s.edx += s.dz;
            s.zbuf++;
            if (!DONE(s))
                Span_FlatZ16(s);
            return;
        }
        LO16(s.eax, *(WORD*)s.dst);
        s.eax <<= 1;
        LO16(s.eax, *(WORD*)(tbl + s.eax));
        *(WORD*)s.dst = (WORD)s.eax;
        *s.zbuf = ZVAL(s);
        s.dst += 2;
        s.edx += s.dz;
        s.zbuf++;
    } while (!DONE(s));
}

// [$L_5fdcde] lit colour, no z
static void Span_Lit16(Span& s)
{
    s.eax <<= 1;
    do {
        BYTE* row = LIGHTROW(s.ecx);
        s.ecx += s.dl;
        LO16(s.edx, *(WORD*)(row + s.eax));
        *(WORD*)s.dst = (WORD)s.edx;
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fdd0b] lit colour where z passes (the pixel lands in the low word of z)
static void Span_LitZ16(Span& s)
{
    s.eax <<= 1;
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            LO16(s.edx, *(WORD*)(LIGHTROW(s.ecx) + s.eax));
            *(WORD*)s.dst = (WORD)s.edx;
        }
        s.ecx += s.dl;
        s.edx += s.dz;
        s.zbuf++;
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fdd55] lit colour through the fog table, no z
static void Span_LitFog16(Span& s)
{
    BYTE* fog = g_spFog71e7b0;
    s.edx = 0;
    s.eax <<= 1;
    do {
        BYTE* row = LIGHTROW(s.ecx);
        s.ecx += s.dl;
        LO16(s.edx, *(WORD*)(row + s.eax));
        s.edx <<= 1;
        LO16(s.edx, *(WORD*)(s.edx + fog));
        *(WORD*)s.dst = (WORD)s.edx;
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fdd93] lit colour through the fog table where z passes (the original indexes the fog table by the low
// byte of twice the colour)
static void Span_LitFogZ16(Span& s)
{
    s.eax <<= 1;
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            WORD c = *(WORD*)(LIGHTROW(s.ecx) + s.eax);
            *(WORD*)s.dst = *(WORD*)(g_spFog71e7b0 + ((WORD)(c << 1) & 255));
        }
        s.ecx += s.dl;
        s.edx += s.dz;
        s.zbuf++;
        s.dst += 2;
    } while (!DONE(s));
}

// === 16-bit textured spans (selected by Span_SelectTextured)

// [$L_5fddef]
static void Span_Tex16(Span& s)
{
    do {
        WORD t = T16(s);
        STEP_UV(s);
        *(WORD*)s.dst = t;
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fde1d] texel 0 is transparent
static void Span_TexKey16(Span& s)
{
    do {
        WORD t = T16(s);
        STEP_UV(s);
        if (t)
            *(WORD*)s.dst = t;
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fde50]
static void Span_TexZ16(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            *(WORD*)s.dst = T16(s);
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fde9b]
static void Span_TexKeyZ16(Span& s)
{
    do {
        if (ZTEST(s)) {
            WORD t = T16(s);
            if (t) {
                *(WORD*)s.dst = t;
                *s.zbuf = ZVAL(s);
            }
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fdef9] through the fog table (edx keeps the bit shifted out of its low word)
static void Span_TexFog16(Span& s)
{
    BYTE* fog = g_spFog71e7b0;
    s.edx = 0;
    do {
        LO16(s.edx, T16(s));
        STEP_UV(s);
        s.edx <<= 1;
        LO16(s.edx, *(WORD*)(fog + s.edx));
        *(WORD*)s.dst = (WORD)s.edx;
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fdf39]
static void Span_TexKeyFog16(Span& s)
{
    BYTE* fog = g_spFog71e7b0;
    do {
        WORD t = T16(s);
        STEP_UV(s);
        if (t)
            *(WORD*)s.dst = *(WORD*)(fog + (WORD)(t << 1));
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fdf7f] (the original ran it with cli, which faults in user mode)
static void Span_TexFogZ16(Span& s)
{
    BYTE* fog = g_spFog71e7b0;
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            *(WORD*)s.dst = *(WORD*)(fog + (WORD)(T16(s) << 1));
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fdfeb] (cli in the original, as Span_TexFogZ16)
static void Span_TexKeyFogZ16(Span& s)
{
    BYTE* fog = g_spFog71e7b0;
    do {
        if (ZTEST(s)) {
            WORD t = T16(s);
            if (t) {
                *(WORD*)s.dst = *(WORD*)(fog + (WORD)(t << 1));
                *s.zbuf = ZVAL(s);
            }
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fe06a] lit
static void Span_TexLit16(Span& s)
{
    do {
        WORD t = T16(s);
        STEP_UV(s);
        *(WORD*)s.dst = *(WORD*)(LIGHTROW(s.ebp) + (WORD)(t << 1));
        s.ebp += s.dl;
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fe0c1] lit; the light steps only on opaque texels
static void Span_TexKeyLit16(Span& s)
{
    do {
        WORD t = T16(s);
        STEP_UV(s);
        if (t) {
            *(WORD*)s.dst = *(WORD*)(LIGHTROW(s.ebp) + (WORD)(t << 1));
            s.ebp += s.dl;
        }
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fe11d] lit where z passes (the original takes the low byte of twice the texel)
static void Span_TexLitZ16(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            WORD t = T16(s);
            *(WORD*)s.dst = *(WORD*)(LIGHTROW(s.ebp) + ((WORD)(t << 1) & 255));
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.ebp += s.dl;
        s.dst += 2;
    } while (!DONE(s));
}

// [$L_5fe1ac]
static void Span_TexKeyLitZ16(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            WORD t = T16(s);
            if (t)
                *(WORD*)s.dst = *(WORD*)(LIGHTROW(s.ebp) + ((WORD)(t << 1) & 255));
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.ebp += s.dl;
        s.dst += 2;
    } while (!DONE(s));
}

// === 8-bit flat spans

// [$L_5fe244]
static void Span_FlatZ8(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.dst = (BYTE)s.eax;
            *s.zbuf = ZVAL(s);
        }
        s.dst++;
        s.edx += s.dz;
        s.zbuf++;
    } while (!DONE(s));
}

// [$L_5fe26d] translucent: the table 71e7f0 at colour * 256 + the pixel (eax keeps its upper word)
static void Span_BlendZ8(Span& s)
{
    s.eax = (s.eax & 0xFFFF00FF) | ((s.eax & 0xFF) << 8);
    BYTE* tbl = g_spBlend71e7f0;
    do {
        if (ZTEST(s)) {
            LO8(s.eax, *s.dst);
            LO8(s.eax, tbl[s.eax]);
            *s.dst = (BYTE)s.eax;
            *s.zbuf = ZVAL(s);
        }
        s.dst++;
        s.edx += s.dz;
        s.zbuf++;
    } while (!DONE(s));
}

// [$L_5fe2a3]
static void Span_Lit8(Span& s)
{
    do {
        BYTE* row = LIGHTROW(s.ecx);
        s.ecx += s.dl;
        *s.dst = row[s.eax];
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe2ca] (the pixel lands in the low byte of z)
static void Span_LitZ8(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            LO8(s.edx, LIGHTROW(s.ecx)[s.eax]);
            *s.dst = (BYTE)s.edx;
        }
        s.ecx += s.dl;
        s.edx += s.dz;
        s.zbuf++;
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe30e]
static void Span_LitFog8(Span& s)
{
    do {
        BYTE* row = LIGHTROW(s.ecx);
        s.ecx += s.dl;
        *s.dst = g_spFog71e7b0[row[s.eax]];
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe343] (the pixel lands in the low byte of z)
static void Span_LitFogZ8(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            BYTE c = g_spFog71e7b0[LIGHTROW(s.ecx)[s.eax]];
            LO8(s.edx, c);
            *s.dst = c;
        }
        s.ecx += s.dl;
        s.edx += s.dz;
        s.zbuf++;
        s.dst++;
    } while (!DONE(s));
}

// === 8-bit textured spans

// [$L_5fe39a]
static void Span_Tex8(Span& s)
{
    do {
        BYTE t = T8(s);
        STEP_UV(s);
        *s.dst++ = t;
    } while (!DONE(s));
}

// [$L_5fe3c2]
static void Span_TexKey8(Span& s)
{
    do {
        BYTE t = T8(s);
        STEP_UV(s);
        if (t)
            *s.dst = t;
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe3ee]
static void Span_TexZ8(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            *s.dst = T8(s);
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe433]
static void Span_TexKeyZ8(Span& s)
{
    do {
        if (ZTEST(s)) {
            BYTE t = T8(s);
            if (t) {
                *s.zbuf = ZVAL(s);
                *s.dst = t;
            }
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe48a]
static void Span_TexFog8(Span& s)
{
    do {
        BYTE t = T8(s);
        STEP_UV(s);
        *s.dst++ = g_spFog71e7b0[t];
    } while (!DONE(s));
}

// [$L_5fe4bd]
static void Span_TexKeyFog8(Span& s)
{
    do {
        BYTE t = T8(s);
        STEP_UV(s);
        if (t)
            *s.dst = g_spFog71e7b0[t];
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe4f4]
static void Span_TexFogZ8(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            *s.dst = g_spFog71e7b0[T8(s)];
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe54c]
static void Span_TexKeyFogZ8(Span& s)
{
    do {
        if (ZTEST(s)) {
            BYTE t = T8(s);
            if (t) {
                *s.dst = g_spFog71e7b0[t];
                *s.zbuf = ZVAL(s);
            }
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe5b6]
static void Span_TexLit8(Span& s)
{
    do {
        BYTE t = T8(s);
        STEP_UV(s);
        *s.dst++ = LIGHTROW(s.ebp)[t];
        s.ebp += s.dl;
    } while (!DONE(s));
}

// [$L_5fe603] the light steps only on opaque texels
static void Span_TexKeyLit8(Span& s)
{
    do {
        BYTE t = T8(s);
        STEP_UV(s);
        if (t) {
            *s.dst = LIGHTROW(s.ebp)[t];
            s.ebp += s.dl;
        }
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe654]
static void Span_TexLitFog8(Span& s)
{
    do {
        BYTE t = T8(s);
        STEP_UV(s);
        *s.dst++ = g_spFog71e7b0[LIGHTROW(s.ebp)[t]];
        s.ebp += s.dl;
    } while (!DONE(s));
}

// [$L_5fe6a9] the light steps only on opaque texels
static void Span_TexKeyLitFog8(Span& s)
{
    do {
        BYTE t = T8(s);
        STEP_UV(s);
        if (t) {
            *s.dst = g_spFog71e7b0[LIGHTROW(s.ebp)[t]];
            s.ebp += s.dl;
        }
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe702]
static void Span_TexLitZ8(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            *s.dst = LIGHTROW(s.ebp)[T8(s)];
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.ebp += s.dl;
        s.dst++;
    } while (!DONE(s));
}

// [$L_5fe789]
static void Span_TexKeyLitZ8(Span& s)
{
    do {
        if (ZTEST(s)) {
            *s.zbuf = ZVAL(s);
            BYTE t = T8(s);
            if (t)
                *s.dst = LIGHTROW(s.ebp)[t];
        }
        s.edx += s.dz;
        s.zbuf++;
        STEP_UV(s);
        s.ebp += s.dl;
        s.dst++;
    } while (!DONE(s));
}

// === selectors and drawers

// [FUN_5feb87] picks the textured loop: mode 0 plain, 1 colour key, 2 lit, 3 lit + key, 4 fog, 5 fog + key,
// 6 lit + fog, 7+ lit + fog + key (only 8-bit without z; elsewhere 6/7 are fog/fog + key); alt > 0 adds the
// z test. The original left the caller's ECX as the loop for 32-bit pixels.
extern "C" void __cdecl Span_SelectTextured(int mode, int alt)
{
    static const SpanLoop t8[8] = {Span_Tex8, Span_TexKey8, Span_TexLit8, Span_TexKeyLit8,
                                   Span_TexFog8, Span_TexKeyFog8, Span_TexLitFog8, Span_TexKeyLitFog8};
    static const SpanLoop z8[8] = {Span_TexZ8, Span_TexKeyZ8, Span_TexLitZ8, Span_TexKeyLitZ8,
                                   Span_TexFogZ8, Span_TexKeyFogZ8, Span_TexFogZ8, Span_TexKeyFogZ8};
    static const SpanLoop t16[8] = {Span_Tex16, Span_TexKey16, Span_TexLit16, Span_TexKeyLit16,
                                    Span_TexFog16, Span_TexKeyFog16, Span_TexFog16, Span_TexKeyFog16};
    static const SpanLoop z16[8] = {Span_TexZ16, Span_TexKeyZ16, Span_TexLitZ16, Span_TexKeyLitZ16,
                                    Span_TexFogZ16, Span_TexKeyFogZ16, Span_TexFogZ16, Span_TexKeyFogZ16};
    int i = mode < 0 ? 0 : mode > 7 ? 7 : mode;
    if (g_spBpp70f3d8 <= 1)
        s_spanLoop = (alt > 0 ? z8 : t8)[i];
    else if (g_spBpp70f3d8 == 2)
        s_spanLoop = (alt > 0 ? z16 : t16)[i];
    else
        s_spanLoop = Span_None;
}

// [FUN_5fedbc] picks the flat loop: mode 0 colour (z only), 1 translucent (z only), 2..5 lit, 6+ lit + fog;
// alt > 0 adds the z test. The cases the original left without a loop (no z with mode 0/1, 32-bit pixels
// for the 8-bit table) draw nothing.
extern "C" void __cdecl Span_SelectFlat(int mode, int alt)
{
    bool z = alt > 0;
    if (g_spBpp70f3d8 <= 1) {
        if (mode > 5)
            s_spanLoop = z ? Span_LitFogZ8 : Span_LitFog8;
        else if (mode > 1)
            s_spanLoop = z ? Span_LitZ8 : Span_Lit8;
        else if (!z)
            s_spanLoop = Span_None;
        else
            s_spanLoop = mode > 0 ? Span_BlendZ8 : Span_FlatZ8;
    } else {
        if (mode > 5)
            s_spanLoop = z ? Span_LitFogZ16 : Span_LitFog16;
        else if (mode > 1)
            s_spanLoop = z ? Span_LitZ16 : Span_Lit16;
        else if (!z)
            s_spanLoop = Span_None;
        else
            s_spanLoop = mode > 0 ? Span_BlendZ16 : Span_FlatZ16;
    }
}

// [FUN_5fee7c] draws count pixels at dst with the selected textured loop: u, v and their steps in
// texture-size fixed point (moved to the top bits), z and dz in 9.7 (to 16.16), the light in 12.20
extern "C" void __cdecl Span_DrawTextured(BYTE* dst, int count, int u, int v, int du, int dv, int z, int dz,
                                          WORD* zbuf, int light, int dlight)
{
    Span s;
    int n = count;
    if (g_spBpp70f3d8 >= 4)
        n <<= 1;
    if (g_spBpp70f3d8 >= 2)
        n <<= 1;
    s.dst = dst;
    s.end = dst + n;
    s.zbuf = zbuf;
    s.ebp = light;
    s.dl = dlight;
    BYTE sh = (BYTE)(16 - g_spTexBits70f3dc) & 31;
    s.du = (DWORD)du << sh;
    s.dv = (DWORD)dv << sh;
    s.edx = (DWORD)z << 7;
    s.dz = (DWORD)dz << 7;
    s.eax = (DWORD)u << sh;
    s.ebx = (DWORD)v << sh;
    s.ecx = g_spTexBits70f3dc;
    s_spanLoop(s);
}

// [FUN_5fef04] draws count pixels at dst with the selected flat loop: colour c, the light in 12.20, z and dz
// in 9.7
extern "C" void __cdecl Span_DrawFlat(BYTE* dst, int count, int light, int dlight, int c, int z, int dz,
                                      WORD* zbuf)
{
    Span s;
    s.dst = dst;
    s.end = dst + (g_spBpp70f3d8 >= 2 ? count * 2 : count);
    s.zbuf = zbuf;
    s.ecx = light;
    s.dl = dlight;
    s.eax = c;
    s.edx = (DWORD)z << 7;
    s.dz = (DWORD)dz << 7;
    s.ebx = 0;
    s.ebp = 0;
    s.du = s.dv = 0;
    s_spanLoop(s);
}
