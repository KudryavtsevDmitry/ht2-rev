// helicopter.cpp - copter.cpp of the game, rewritten from king.masm: small
// functions of the copter and of the code around it: a stack of 16-bit
// values, the copter's initialization and its targets, the on-screen notice,
// objects of the game screen by id, enumerations, and a menu line.
//
// Building with  nmake ASM_HELICOPTER=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

ASM_PROC(FUN_566020)    // copter: the vehicle to follow (mode, a)
ASM_PROC(FUN_5662f0)    // copter: a new state
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")    // objects of a screen: by id
ASM_PROC(FUN_569000)
ASM_PROC(FUN_569250)
extern "C" void __fastcall Messages_AddText(BYTE* self, void*, const char* text);
ASM_PROC(FUN_5d3290)    // destructor of the notice's base class
ASM_PROC(FUN_5fd430)    // sound: play (volume, pitch, a)
ASM_PROC(FUN_5b9660)
ASM_PROC(FUN_5b6210)
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")    // a text of the resources
extern "C" void FUN_5ea170(); KING_ALIAS("_FUN_5ea170", "_Text_Draw")    // draws a text
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
    Messages_AddText(g_messages, 0, text);
    return 0;
}

extern "C" int __cdecl Messages_Get1288()
{
    return F(int, g_messages, 1288);
}

extern "C" void __cdecl Messages_Add(const char* text)
{
    if (g_messages)
        Messages_AddText(g_messages, 0, text);
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

// ---------------------------------------------------------------------------
// medium-sized functions of the unit

ASM_PROC(FUN_561800)
ASM_PROC(FUN_4e4750)    // a child of the object by name (object, name, flags)
ASM_PROC(FUN_4e480c)    // show or hide an object (object, show)
ASM_VAR(BYTE*, g_truck, $L_6f33ac)              // +88: the hour (double)
extern "C" Vec3* __fastcall Vec3_ZeroThis(Vec3* self, void*);
ASM_VAR(double, g_64b840, $L_64b840)            // 0.2
ASM_VAR(double, g_64b668, $L_64b668)            // 1.5

static void Matrix_Construct(Matrix* m)
{
    for (int k = 0; k < 4; k++)
        Vec3_ZeroThis((Vec3*)((BYTE*)m + k * 12), 0);
    *m = g_identity;
}

extern "C" BYTE* __fastcall Copter_Construct(BYTE* self, void*, int a, int b, int c)
{
    Matrix_Construct(&F(Matrix, self, 0));
    Matrix_Construct(&F(Matrix, self, 48));
    for (int i = 100; i <= 120; i += 4)
        F(int, self, i) = 0;
    Stack_Reset(self + 144, 0);
    for (int i = 3172; i <= 3204; i += 4)
        F(int, self, i) = 0;
    F(int, self, 3256) = 0;
    F(int, self, 3260) = 0;
    F(int, self, 3264) = 0;
    F(int, self, 3272) = -1;
    F(int, self, 3316) = 0;
    F(int, self, 3320) = 0;
    F(int, self, 3324) = 0;
    F(int, self, 3360) = 0;
    F(int, self, 3364) = 0;
    F(int, self, 3368) = 0;
    Copter561310_Construct(self + 3372, 0);
    for (int i = 3504; i <= 3536; i += 4)
        F(int, self, i) = 0;
    CallThis(FUN_561800, self, a, b);
    F(int, self, 3268) = c;
    return self;
}

extern "C" int __fastcall Copter_FindLights(BYTE* self, void*, void* model)
{
    F(void*, self, 3160) = CallC<void*>(FUN_4e4750, model, "SearchLightKey", 0);
    F(void*, self, 3164) = CallC<void*>(FUN_4e4750, model, "BlinkLightKey", 0);
    F(void*, self, 3168) = CallC<void*>(FUN_4e4750, model, "StaticLightKey", 0);
    F(void*, self, 3300) = CallC<void*>(FUN_4e4750, model, "GunLeftKey", 0);
    F(void*, self, 3304) = CallC<void*>(FUN_4e4750, model, "GunRightKey", 0);
    F(int, self, 3544) = -1;
    F(Vec3, self, 3504) = g_zeroVec;
    F(Vec3, self, 3516) = g_axisZ;
    F(Vec3, self, 3528) = g_axisY;
    return 0;
}

// the lights at night (from 22 to 5) and the blinking light
extern "C" void __fastcall Copter_UpdateLights(BYTE* self, void*)
{
    int hour = (int)F(double, g_truck, 88);
    int night = !(hour <= 21 && hour >= 6);
    CallC(FUN_4e480c, F(void*, self, 3168), night);
    CallC(FUN_4e480c, F(void*, self, 3164), F(int, self, 3156) ? 1 : 0);
    double d = Game_Time() - F(float, self, 3152);
    if (d > g_64b840)
        F(int, self, 3156) = 0;
    if (d > g_64b668) {
        if (!F(int, self, 3156))
            F(float, self, 3152) = (float)Game_Time();
        F(int, self, 3156) = 1;
    }
    CallC(FUN_4e480c, F(void*, self, 3160), night);
}

ASM_PROC(FUN_568830)    // adds a helicopter's way point
ASM_PROC(FUN_4ba0c0)
ASM_PROC(FUN_5ea1b0)    // draws a text in a box
extern "C" Matrix* __fastcall SceneObj_Matrix1(BYTE* self, void*, Matrix* out);   // engine.cpp
extern "C" void __fastcall Node_Matrix(BYTE* self, void*, Matrix* out);          // engine.cpp
ASM_VAR(int, g_wayCount, $L_67b2b8)
ASM_VAR(int, g_696c5c, $L_696c5c)
ASM_VAR(BYTE*, g_6ef1fc, $L_6ef1fc)
ASM_VAR(Sound*, g_shootSound, $L_6f2c44)
ASM_VAR(Sound*, g_hitgrndSound, $L_6f2c48)
ASM_VAR(AsmLabel, g_str68d1c0, $L_68d1c0)       // ""
ASM_VAR(AsmLabel, g_str67b69c, $L_67b69c)
ASM_VAR(AsmLabel, g_str67b698, $L_67b698)
ASM_VAR(AsmLabel, g_str67b68c, $L_67b68c)       // "room_al_082"
ASM_VAR(AsmLabel, g_str67b688, $L_67b688)
ASM_VAR(AsmLabel, g_str67b67c, $L_67b67c)       // "room_aq_008"
ASM_VAR(AsmLabel, g_str66c4f4, $L_66c4f4)
ASM_VAR(AsmLabel, g_str67b670, $L_67b670)       // "room_at_004"
ASM_VAR(AsmLabel, g_str67b66c, $L_67b66c)
ASM_VAR(AsmLabel, g_str67b668, $L_67b668)
ASM_VAR(AsmLabel, g_cb569090, $L_569090)
ASM_VAR(double, g_64ff10, $L_64ff10)            // 6200
ASM_VAR(double, g_64b5f0, $L_64b5f0)            // 10000
ASM_VAR(Ints, g_colors67b73c, $L_67b73c)        // 8 colors
ASM_VAR(int, g_scoreCount, $L_6f34d8)
ASM_VAR(Ints, g_scoreNames, $L_6f3388)          // char*
ASM_VAR(Doubles, g_scoreValues, $L_6f2df8)
ASM_VAR(BYTE*, g_6cec98, $L_6cec98)

static inline void Way_Add(void* scene, const char* room, const char* name, float x, float y, float z,
                           float r, int index)
{
    CallC(FUN_568830, scene, room, name, x, y, z, r, index, 0, 0);
}

extern "C" void __cdecl Helicopters_Init(void* scene)
{
    g_wayCount = 0;
    if (!g_696c5c)
        return;
    g_wayCount = 1;
    Way_Add(scene, g_str68d1c0, g_str67b69c, 2650.0f, -2400.0f, 550.0f, 600.0f, 0);
    Way_Add(scene, g_str67b68c, g_str67b698, -2205.0f, -1983.0f, 65.0f, 600.0f, g_wayCount++);
    Way_Add(scene, g_str67b67c, g_str67b688, -3400.0f, -3800.0f, 20.0f, 600.0f, g_wayCount++);
    Way_Add(scene, g_str67b670, g_str66c4f4, -4107.0f, -393.0f, 40.0f, 600.0f, g_wayCount++);
    Way_Add(scene, g_str68d1c0, g_str67b66c, -600.0f, -2850.0f, 100.0f, 600.0f, g_wayCount++);
    Way_Add(scene, g_str68d1c0, g_str67b668, -3800.0f, 4000.0f, 20.0f, 600.0f, g_wayCount++);
    g_shootSound = Sound_Find("shootSound");
    g_hitgrndSound = Sound_Find("hitgrndSound");
    if (!g_shootSound || !g_hitgrndSound) {
        char buf[200];
        sprintf(buf, "initHelicopters() did not find sounds: \"shootSound\"=%x, \"hitgrndSound\"=%x",
                (unsigned)g_shootSound, (unsigned)g_hitgrndSound);
    }
}

// the position of way point i and its object's id
extern "C" int __cdecl Way_Point(int i, DWORD* pos, int* id)
{
    if (i >= g_wayCount || i < 0)
        return -1;
    *id = i + 300000;
    int* o = CallThis<int*>(FUN_5e5370, g_6ef1fc + 704, i + 300000);
    if (!o)
        return -2;
    if (o[0] == 1 && o[7]) {
        Matrix m;
        BYTE* r = (BYTE*)SceneObj_Matrix1((BYTE*)o[7] + 16, 0, &m);
        pos[0] = F(DWORD, r, 36);
        pos[1] = F(DWORD, r, 40);
        pos[2] = F(DWORD, r, 44);
        return 0;
    }
    BYTE* d = (BYTE*)o[6];
    pos[0] = F(DWORD, d, 80);
    pos[1] = F(DWORD, d, 84);
    pos[2] = F(DWORD, d, 88);
    return 0;
}

// adds a screen message, not the last one again within 10 s
extern "C" void __fastcall Messages_AddText(BYTE* self, void*, const char* text)
{
    if (strlen(text) >= 128)
        return;
    int cur = F(int, self, 1200);
    if (F(char, self, 44 + cur * 128))
        return;
    int prev = cur - 1;
    if (prev < 0)
        prev += 4;
    if (strcmp(text, (const char*)self + 44 + prev * 128) == 0
        && !(Timer_Now() - F(int, self, 1272) >= g_64b5f0))
        return;
    F(int, self, 1272) = (int)Timer_Now();
    int n = F(int, self, 1200);
    F(int, self, 1200) = n + 1;
    strcpy((char*)self + 44 + n * 128, text);
    F(int, self, 1200) = F(int, self, 1200) % 4;
}

// a gate of the race passed: the time bonus, and the gate lit
extern "C" void __cdecl Gate_Passed(int id, int side, BYTE* car)
{
    BYTE* o = CallThis<BYTE*>(FUN_5e5370, (BYTE*)g_pWnd6cec6c->m_1356 + 704, id);
    BYTE* b = F(BYTE*, o, 28);
    if (!b)
        return;
    if (!F(int, b, 416) && car && F(int, car, 392) == 2 && F(int, car, 3568) == 2
        && car - 16 == F(BYTE*, g_p6d2078, 616)) {
        BYTE* e = F(BYTE*, car, 20620);
        F(double, e, 208) = F(double, e, 208) + g_64ff10;
        F(DWORD, e, 216) |= 32;
    }
    if (F(int, b, 416) & (3 << (side * 2)))
        return;
    Matrix m;
    for (int k = 0; k < 4; k++)
        Vec3_ZeroThis((Vec3*)((BYTE*)&m + k * 12), 0);
    m = g_identity;
    Node_Matrix(b, 0, &m);
    CallC(FUN_4ba0c0, &m, side, (void*)g_cb569090, b);
}

extern "C" void __cdecl Colors_Init(BYTE* surface)
{
    g_colors67b73c[0] = Obj573fe0(surface, 255, 0, 255);
    g_colors67b73c[1] = Obj573fe0(surface, 0, 255, 0);
    g_colors67b73c[2] = Obj573fe0(surface, 0, 0, 255);
    g_colors67b73c[3] = Obj573fe0(surface, 255, 255, 0);
    g_colors67b73c[4] = Obj573fe0(surface, 0, 255, 255);
    g_colors67b73c[5] = Obj573fe0(surface, 255, 255, 255);
    g_colors67b73c[6] = Obj573fe0(surface, 255, 128, 0);
    g_colors67b73c[7] = Obj573fe0(surface, 0, 128, 255);
}

// the table of the results: place, name and time or money
extern "C" void __cdecl Scores_Draw(BYTE* surface, int x, int y)
{
    char buf[64];
    int xx = x + 38;
    for (int i = 0; i < g_scoreCount; i++) {
        sprintf(buf, "%d", i + 1);
        int yy = y + 15 + (i % 4) * 20;
        CallC(FUN_5ea170, surface, xx - 33, yy, g_fontMenu, buf, 6, 0);
        CallC(FUN_5ea1b0, surface, xx, yy, 70, g_fontMenu, g_scoreNames[i], 6, 0);
        double v = g_scoreValues[i];
        if (F(int, g_6cec98, 384) < 2) {
            int t = (int)v;
            sprintf(buf, "%d:%02d", t / 60, t % 60);
        } else
            sprintf(buf, "$%d", (int)v);
        CallC(FUN_5ea170, surface, xx + 74, yy, g_fontMenu, buf, 6, 0);
        if (i == 3)
            xx += 181;
    }
}
