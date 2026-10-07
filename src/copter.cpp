// copter.cpp - copter.cpp of the game, rewritten from king.masm: small
// functions of the copter and of the code around it: a stack of 16-bit
// values, the copter's initialization and its targets, the on-screen notice,
// objects of the game screen by id, enumerations, and a menu line.
//
// Building with  nmake ASM_COPTER=1  makes king.masm assemble the original
// procedures again.

#include <string.h>
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

ASM_PROC(FUN_566020)    // copter: the vehicle to follow (mode, a)
ASM_PROC(FUN_5662f0)    // copter: a new state
ASM_PROC(FUN_5e5370)    // objects of a screen: by id
ASM_PROC(FUN_569000)
ASM_PROC(FUN_569250)
ASM_PROC(FUN_5699b0)    // screen messages: add (text)
ASM_PROC(FUN_5d3290)    // destructor of the notice's base class
ASM_PROC(FUN_5fd430)    // sound: play (volume, pitch, a)
ASM_PROC(FUN_5b9660)
ASM_PROC(FUN_5b6210)
ASM_PROC(FUN_5d2e50)    // a text of the resources
ASM_PROC(FUN_5ea170)    // draws a text
ASM_PROC(FUN_440dd0)
ASM_FUNC(Copter_EnumCallback, $L_56f450)

ASM_VAR(int, g_noCar, $L_68b990)                // the id of no vehicle
ASM_VAR(BYTE*, g_6cec70, $L_6cec70)
ASM_VAR(double, g_6f3410, $L_6f3410)
ASM_VAR(BYTE*, g_messages, $L_6f3438)            // the screen messages
ASM_VAR(BYTE*, g_notice, $L_6f3430)             // the on-screen notice
ASM_VAR(BYTE*, g_6f3498, $L_6f3498)
ASM_VAR(int, g_6ce37c, $L_6ce37c)
ASM_VAR(int, g_6ce380, $L_6ce380)
ASM_VAR(int, g_6ce384, $L_6ce384)
ASM_VAR(int, g_6ce388, $L_6ce388)
ASM_VAR(double, g_6f34b8, $L_6f34b8)
ASM_VAR(BYTE*, g_6f33ec, $L_6f33ec)             // a sound
ASM_VAR(int, g_6f3248, $L_6f3248)               // the enumeration's arguments
ASM_VAR(int, g_6f324c, $L_6f324c)
ASM_VAR(int, g_6f3250, $L_6f3250)
ASM_VAR(int, g_6f3254, $L_6f3254)
ASM_VAR(int, g_6f34c0, $L_6f34c0)               // its count
typedef BYTE* Found[1];
ASM_VAR(Found, g_6f2e58, $L_6f2e58)             // its results
typedef int Ints[1];
ASM_VAR(Ints, g_6f326c, $L_6f326c)
ASM_VAR(int, g_fontMenu, $L_6ced24)
typedef double Doubles[1];
ASM_VAR(Doubles, g_6f3338, $L_6f3338)
ASM_VAR(AsmLabel, g_vtbl64fe34, $L_64fe34)

struct Vec3Bits { DWORD x, y, z; };     // a Vec3 copied or passed by value

// ---------------------------------------------------------------------------
// A stack of 16-bit values: the values from +2 (index 1) on, a flag per
// entry from +2000 (1: the first of a pair), at +3000 the index of the top

extern "C" BYTE* __fastcall Stack_Reset(BYTE* self, void*)
{
    F(int, self, 3000) = -1;
    return self;
}

extern "C" void __fastcall Stack_Init(BYTE* self, void*, int a, int b)
{
    F(short, self, 0) = (short)a;
    F(short, self, 2) = (short)b;
    self[2000] = 1;
    self[2001] = 0;
    F(int, self, 3000) = 1;
}

// the value i below the top
extern "C" short __fastcall Stack_Peek(BYTE* self, void*, int i)
{
    int top = F(int, self, 3000);
    if (i > top)
        return 0;
    return F(short, self, (top - i) * 2);
}

extern "C" short __fastcall Stack_Below(BYTE* self, void*)
{
    int top = F(int, self, 3000);
    if (top > 1)
        return F(short, self, top * 2 - 2);
    return -1;
}

// drops the top (both values of a pair) unless it is the last; the new top
extern "C" short __fastcall Stack_Pop(BYTE* self, void*)
{
    int top = F(int, self, 3000);
    if (top > 1 && !(self[1999 + top] && top == 2)) {
        top--;
        F(int, self, 3000) = top;
        if (self[2000 + top]) {
            top--;
            F(int, self, 3000) = top;
        }
        top = F(int, self, 3000);
    }
    return F(short, self, top * 2);
}

extern "C" int __fastcall Stack_Push(BYTE* self, void*, int v)
{
    if (F(int, self, 3000) >= 1000)
        return 0;
    int top = F(int, self, 3000) + 1;
    F(int, self, 3000) = top;
    F(short, self, top * 2) = (short)v;
    self[2000 + F(int, self, 3000)] = 0;
    return F(int, self, 3000);
}

extern "C" int __fastcall Stack_PushPair(BYTE* self, void*, int a, int b)
{
    if (F(int, self, 3000) >= 999)
        return 0;
    int top = F(int, self, 3000) + 1;
    F(int, self, 3000) = top;
    F(short, self, top * 2) = (short)a;
    self[2000 + F(int, self, 3000)] = 1;
    Stack_Push(self, 0, b);
    return F(int, self, 3000);
}

extern "C" void __fastcall Stack_SwapTop(BYTE* self, void*)
{
    int top = F(int, self, 3000);
    short below = F(short, self, top * 2 - 2);
    short v = F(short, self, top * 2);
    F(short, self, top * 2) = below;
    F(short, self, F(int, self, 3000) * 2 - 2) = v;
}

// ---------------------------------------------------------------------------
// The copter

extern "C" void __fastcall Copter561390_Clear(BYTE* self, void*)
{
    F(int, self, 44) = 0;
    F(int, self, 40) = 0;
    F(int, self, 36) = 0;
    F(int, self, 32) = 0;
    F(int, self, 28) = 0;
    F(int, self, 24) = 0;
    F(int, self, 68) = 0;
    F(int, self, 64) = 0;
    F(int, self, 60) = 0;
    F(int, self, 8) = 0;
    F(int, self, 4) = 0;
    F(int, self, 0) = 0;
    F(int, self, 56) = 0;
    F(int, self, 52) = 0;
    F(int, self, 48) = 0;
    F(int, self, 80) = 0;
    F(int, self, 76) = 0;
    F(int, self, 72) = 0;
    F(int, self, 116) = 0;
}

extern "C" BYTE* __fastcall Copter561310_Construct(BYTE* self, void*)
{
    F(int, self, 96) = 0;
    F(int, self, 100) = 0;
    F(int, self, 104) = 0;
    F(int, self, 120) = 0;
    F(int, self, 124) = 0;
    F(int, self, 128) = 0;
    F(int, self, 12) = 1;
    F(int, self, 16) = 1;
    F(int, self, 20) = 1;
    F(int, self, 88) = 120;
    F(int, self, 84) = 0;
    F(int, self, 92) = 0;
    F(Vec3, self, 96) = g_zeroVec;
    F(int, self, 108) = 0;
    F(int, self, 112) = 0;
    F(Vec3, self, 120) = g_zeroVec;
    Copter561390_Clear(self, 0);
    return self;
}

extern "C" void __fastcall Copter5615f0_Reset(BYTE* self, void*)
{
    F(Vec3, self, 100) = g_zeroVec;
    F(Vec3, self, 112) = g_zeroVec;
    F(int, self, 124) = 0;
    F(int, self, 128) = 0;
    F(int, self, 3232) = 0;
    F(int, self, 3208) = 0;
    F(int, self, 3152) = 0;
    F(int, self, 3156) = 0;
    F(int, self, 3284) = 0;
    F(int, self, 3288) = 0;
    F(int, self, 3292) = 0;
    F(int, self, 3296) = 0;
    F(int, self, 3332) = 0;
    F(int, self, 3328) = 0;
    F(int, self, 3340) = 0;
    F(int, self, 3336) = 0;
    F(float, self, 3224) = 0.99f;
    F(float, self, 3228) = 0.99f;
    F(float, self, 3220) = 0.99f;
    F(float, self, 3216) = 0.99f;
    F(float, self, 3212) = 0.99f;
}

extern "C" void __fastcall Copter561530_Init(BYTE* self, void*)
{
    F(int, self, 3548) = 0;
    F(int, self, 96) = 0;
    F(Matrix, self, 0) = g_identity;
    F(Matrix, self, 48) = g_identity;
    F(int, self, 132) = 0;
    F(int, self, 140) = 0;
    F(int, self, 136) = 0;
    F(int, self, 3236) = 3;
    F(int, self, 3240) = 0;
    F(int, self, 3248) = 0;
    F(int, self, 3252) = 0;
    F(Vec3, self, 3256) = g_zeroVec;
    F(float, self, 3268) = 300.0f;
    F(int, self, 3544) = -1;
    F(int, self, 3276) = 0;
    F(int, self, 3344) = 0;
    F(int, self, 3348) = 0;
    F(int, self, 3352) = 0;
    Copter5615f0_Reset(self, 0);
}

// the matrix at +0 and its inverse at +48
extern "C" void __fastcall Copter_SetMatrix(BYTE* self, void*, int a, const Matrix* m)
{
    Matrix inverse;
    F(Matrix, self, 0) = *m;
    const Matrix* r = Matrix_Inverse(&inverse, (const Matrix*)self);
    F(Matrix, self, 48) = *r;
    F(int, self, 96) = a;
}

extern "C" void __fastcall Copter_SetVec112(BYTE* self, void*, const Vec3Bits* v)
{
    F(Vec3Bits, self, 112) = *v;
}

extern "C" void __fastcall Copter_SetVec100(BYTE* self, void*, const Vec3Bits* v)
{
    F(Vec3Bits, self, 100) = *v;
}

// [FUN_566020] with the copter in ECX, as the original passes it through
extern "C" BYTE* __fastcall Copter_565fe0(BYTE* self, void*)
{
    return CallThis<BYTE*>(FUN_566020, self, 1, 0);
}

extern "C" BYTE* __fastcall Copter_565ff0(BYTE* self, void*, int a)
{
    return CallThis<BYTE*>(FUN_566020, self, 0, a);
}

extern "C" BYTE* __fastcall Copter_566240(BYTE* self, void*)
{
    return CallThis<BYTE*>(FUN_566020, self, 0, 0);
}

// a vehicle's state at +20944; 0 for none and for the states 21..29
extern "C" int __cdecl Car_State20944(const BYTE* car)
{
    if (!car)
        return 0;
    int v = F(int, car, 20944);
    if (v > 20 && v < 30)
        return 0;
    return v;
}

extern "C" void __fastcall Copter_Chase6(BYTE* self, void*, BYTE* target)
{
    if (!target)
        return;
    F(BYTE*, self, 3248) = target;
    F(int, self, 3252) = 0;
    CallThis(FUN_5662f0, self, 6);
    F(float, self, 3280) = (float)Game_Time();
}

extern "C" void __fastcall Copter_Chase10(BYTE* self, void*, BYTE* target)
{
    if (!target)
        return;
    F(BYTE*, self, 3248) = target;
    F(int, self, 3252) = 0;
    CallThis(FUN_5662f0, self, 10);
}

extern "C" void __fastcall Copter_Chase7(BYTE* self, void*, BYTE* target)
{
    if (!target)
        return;
    F(BYTE*, self, 3248) = target;
    F(int, self, 3252) = 0;
    CallThis(FUN_5662f0, self, 7);
}

extern "C" void __fastcall Copter_566550(BYTE* self, void*)
{
    if (F(BYTE*, self, 3248))
        return;
    int state = F(int, self, 3236);
    if (state == 6 || state == 5 || state == 10)
        return;
    BYTE* car = Copter_565fe0(self, 0);
    if (!car || !F(int, car, 20944))
        return;
    if (F(int, self, 3344) && F(BYTE*, self, 3248))
        return;
    F(int, self, 3344) = 1;
    F(BYTE*, self, 3248) = car;
    F(int, self, 3252) = 0;
}

// ---------------------------------------------------------------------------

// the id of the player's vehicle, g_noCar without one
extern "C" int* __cdecl Player_CarId(int* out)
{
    if (g_p6d2078 && g_p6d2078->m_616) {
        *out = F(int, g_p6d2078->m_616, 20636);
        return out;
    }
    *out = g_noCar;
    return out;
}

extern "C" void __cdecl Game_568f90()
{
    F(int, g_pWnd6cec6c, 1288) = 1;
    F(int, g_pWnd6cec6c, 1284) = 0;
    F(int, g_pWnd6cec6c, 1296) = 0;
    g_6f3410 = Timer_Now();
}

extern "C" int __cdecl Game_568fd0()
{
    if ((BYTE)(F(DWORD, g_6cec70, 408) & 15) == 1)
        return 1;
    return (F(int, g_6cec70, 416) != 3) + 2;
}

// objects of the game screen by id: +24 and +28 of the entry
static BYTE* ScreenObject(int id)
{
    return CallThis<BYTE*>(FUN_5e5370, (BYTE*)g_pMainView + 704, id);
}

extern "C" void __cdecl Screen_569150(int id)
{
    BYTE* e = ScreenObject(id + 4000);
    if (!e)
        return;
    if (F(BYTE*, e, 24))
        F(int, F(BYTE*, e, 24), 4) = 0;
    BYTE* o = F(BYTE*, e, 28);
    if (!o)
        return;
    CallC(FUN_569000, F(int, o, 344), F(int, o, 416));
    F(int, o, 416) = 0;
    CallC(FUN_569000, F(int, o, 344), 0);
}

extern "C" int __cdecl Screen_5692e0(int id)
{
    BYTE* e = ScreenObject(id);
    if (!e)
        return 0;
    return F(int, F(BYTE*, e, 24), 4);
}

extern "C" void __cdecl Screen_569310(int id, unsigned v)
{
    BYTE* e = ScreenObject(id + 2000);
    if (!e)
        return;
    if (F(BYTE*, e, 24))
        F(unsigned, F(BYTE*, e, 24), 4) = v;
    BYTE* o = F(BYTE*, e, 28);
    if (!o)
        return;
    F(unsigned, o, 408) = v;
    CallC(FUN_569250, F(int, o, 344), v ^ 1);
    CallC(FUN_569250, F(int, o, 344), v);
}

extern "C" void __cdecl Screen_569410(int id, int v)
{
    BYTE* e = ScreenObject(id + 1000);
    if (!e || !F(BYTE*, e, 24))
        return;
    F(int, F(BYTE*, e, 24), 4) = v;
    if (F(BYTE*, e, 28))
        F(int, F(BYTE*, e, 28), 408) = v;
}

extern "C" int __cdecl Messages_Get1292()
{
    return F(int, g_messages, 1292);
}

// returns the messages object, as the original leaves it in EAX
extern "C" BYTE* __cdecl Messages_Clear1276()
{
    F(int, g_messages, 1276) = 0;
    return g_messages;
}

extern "C" int __cdecl Messages_Add569720(const char* text)
{
    return CallThis<int>(FUN_5699b0, g_messages, text);
}

extern "C" int __cdecl Messages_Get1288()
{
    return F(int, g_messages, 1288);
}

extern "C" void __cdecl Messages_Add(const char* text)
{
    if (g_messages)
        CallThis(FUN_5699b0, g_messages, text);
}

// ---------------------------------------------------------------------------
// The on-screen notice: +32 the time it was set (ms), +48 its text, +60 its
// level

extern "C" int __cdecl Notice_Level()
{
    if (g_notice)
        return F(int, g_notice, 60);
    return 0;
}

// sets the text unless one of a higher level is shown; 1 when set
extern "C" int __fastcall Notice_Set(BYTE* self, void*, int level, const char* text)
{
    int cur = F(int, self, 60);
    if ((level >= cur && cur && level < 8) || cur == 5)
        return 0;
    if (text) {
        if (F(void*, g_notice, 48))
            operator delete(F(void*, g_notice, 48));
        size_t n = strlen(text) + 1;
        F(char*, g_notice, 48) = (char*)operator new(n);
        memcpy(F(char*, g_notice, 48), text, n);
    }
    F(int, self, 60) = level;
    F(double, self, 32) = g_pMainView->m_2968 * 1000.0;
    return 1;
}

extern "C" void __cdecl Notice_Show(int level, const char* text, int a, int b)
{
    if (!g_notice)
        return;
    if (Notice_Set(g_notice, 0, level, text)) {
        F(int, g_notice, 52) = a;
        F(int, g_notice, 56) = b;
    }
}

extern "C" void __cdecl Notice_Banner(int eventId, double value, const char* text)
{
    Notice_Show(10, text, eventId, (int)value);
}

extern "C" void __fastcall Notice_Destruct(BYTE* self, void*)
{
    void* text = F(void*, self, 48);
    F(void*, self, 0) = g_vtbl64fe34;
    if (text)
        operator delete(text);
    CallThis(FUN_5d3290, self);
}

// ---------------------------------------------------------------------------

extern "C" int __cdecl Obj6f3498_Get212()
{
    if (g_6f3498)
        return F(int, g_6f3498, 212);
    return 0;
}

extern "C" void __cdecl Flags_Set6ce37c(unsigned flags)
{
    g_6ce37c = 0;
    g_6ce380 = ((int)(signed char)flags & 64) >> 6;
    g_6ce384 = (flags >> 8) & 1;
    g_6ce388 = (flags >> 9) & 1;
}

// a callback: its arguments a0..a5 are unused, then a speed and flags
extern "C" void __cdecl Sound_56ef60(int a0, int a1, int a2, int a3, int a4, int a5, double speed, int flags)
{
    if (!(flags & 2) || !(flags & 1))
        return;
    if (!(speed > 5.0))
        return;
    double v = speed * 0.05 * 0.05 + g_6f34b8 * 0.95;
    g_6f34b8 = v;
    float x = (float)v;
    CallThis(FUN_5fd430, g_6f33ec, x, x, 1);
}

extern "C" int __cdecl Screen_56f210(int id)
{
    BYTE* e = CallThis<BYTE*>(FUN_5e5370, (BYTE*)g_pWnd6cec6c->m_1356 + 704, id);
    BYTE* o = F(BYTE*, e, 28);
    if (!o)
        return -1;
    return F(int, o, 416);
}

// enumerates obj with [$L_56f450]; the results in g_6f2e58, *count of them
extern "C" BYTE** __cdecl Enum_56f3f0(void* obj, int a, int b, int c, int d, int* count)
{
    g_6f3248 = a;
    g_6f324c = b;
    g_6f3250 = c;
    g_6f3254 = d;
    g_6f34c0 = 0;
    CallVirt(obj, 32, (void*)Copter_EnumCallback, 0);
    *count = g_6f34c0;
    return g_6f2e58;
}

extern "C" int __cdecl Enum_56f650(int i, int* a, int* b, double* c, int* d, int* e, int* f)
{
    BYTE* p = g_6f2e58[i];
    if (F(int, p, 16) != 9)
        return -1;
    if (F(int, p, 64) != 7)
        return -1;
    *f = g_6f326c[i];
    *a = F(int, p, 72);
    *b = F(int, p, 48);
    *c = F(int, p, 52) * 0.01;
    *d = F(int, p, 60) + 4;
    *e = F(int, p, 56) - 1;
    return F(int, p, 68);
}

extern "C" int __cdecl Obj573fe0(BYTE* o, int a, int b, int c)
{
    int kind = F(int, o, 808);
    if (kind == 1)
        return CallThis<int>(FUN_5b9660, o, a, b, c) & 255;
    if (kind == 2)
        return CallC<int>(FUN_5b6210, a, b, c);
    return 0;
}

// entry n of a menu in columns of 18: the resource text, and text 111 to the right
extern "C" void __cdecl Menu_DrawEntry(int surface, int x, int y, int textId, const char* text, int n)
{
    int yy = y + n % 18 * 20 + 10;
    int xx = x + n / 18 * 170;
    const char* s = CallC<const char*>(FUN_5d2e50, textId, 6, 0);
    CallC(FUN_5ea170, surface, xx, yy, g_fontMenu, s, 6, 0);
    CallC(FUN_5ea170, surface, xx + 111, yy, g_fontMenu, text, 6, 0);
}

extern "C" void __cdecl Set6f3338(int i, int v)
{
    g_6f3338[i] = v;
}

extern "C" BYTE* __fastcall Obj576c00_DeletingDestructor(BYTE* self, void*, int flags)
{
    CallThis(FUN_440dd0, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
