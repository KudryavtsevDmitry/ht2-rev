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

extern "C" void FUN_566020(); KING_ALIAS("_FUN_566020", "@Copter_NearestVisible@16")    // copter: the vehicle to follow (mode, a)
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_5c3630(); KING_ALIAS("_FUN_5c3630", "@Media_RecordBlock@12")
extern "C" void FUN_5bc2f0(); KING_ALIAS("_FUN_5bc2f0", "@Poly_Destroy@8")
extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_568fd0(); KING_ALIAS("_FUN_568fd0", "_Game_568fd0")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
extern "C" void FUN_5ef640(); KING_ALIAS("_FUN_5ef640", "@Node_Root@8")
ASM_PROC(FUN_5e52b0)
extern "C" void FUN_5a47f0(); KING_ALIAS("_FUN_5a47f0", "@Wea5a47f0_Destroy@4")
ASM_VAR(AsmLabel, g_vtbl64fdd4, $L_64fdd4)
ASM_VAR(AsmLabel, g_vtbl64ff70, $L_64ff70)
ASM_VAR(AsmLabel, g_vtbl64ff24, $L_64ff24)
ASM_VAR(AsmLabel, g_vtbl64ffec, $L_64ffec)
ASM_VAR(AsmLabel, g_vtbl64ffa0, $L_64ffa0)
ASM_VAR(AsmLabel, g_vtbl650068, $L_650068)
ASM_VAR(AsmLabel, g_vtbl65001c, $L_65001c)
ASM_VAR(AsmLabel, g_vtbl6500e4, $L_6500e4)
ASM_VAR(AsmLabel, g_vtbl650098, $L_650098)
ASM_VAR(AsmLabel, s_67b970, $L_67b970)
ASM_VAR(AsmLabel, s_67b95c, $L_67b95c)
ASM_VAR(AsmLabel, s_67b948, $L_67b948)
ASM_VAR(BYTE*, g_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_696ca0, $L_696ca0)
ASM_VAR(AsmLabel, g_copterState, $L_6f2d98)
ASM_PROC(FUN_5662f0)    // copter: a new state
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")    // objects of a screen: by id
extern "C" void FUN_569000(); KING_ALIAS("_FUN_569000", "_Copter_Send963")
extern "C" void FUN_569250(); KING_ALIAS("_FUN_569250", "_Copter_Send961")
extern "C" void __fastcall Messages_AddText(BYTE* self, void*, const char* text);
extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")    // destructor of the notice's base class
ASM_PROC(FUN_5fd430)    // sound: play (volume, pitch, a)
extern "C" void FUN_5b9660(); KING_ALIAS("_FUN_5b9660", "@Palette_Nearest@20")
extern "C" void FUN_5b6210(); KING_ALIAS("_FUN_5b6210", "_Pixel_Pack")
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")    // a text of the resources
extern "C" void FUN_5ea170(); KING_ALIAS("_FUN_5ea170", "_Text_Draw")    // draws a text
extern "C" void FUN_440dd0(); KING_ALIAS("_FUN_440dd0", "@Rec440dd0_FreeVectors@8")
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
extern "C" void FUN_4e4750(); KING_ALIAS("_FUN_4e4750", "_Scene_FindOrFail")    // a child of the object by name (object, name, flags)
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")    // show or hide an object (object, show)
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

extern "C" void FUN_568830(); KING_ALIAS("_FUN_568830", "_Net_CreatePlacedObject")    // adds a helicopter's way point
extern "C" void FUN_4ba0c0(); KING_ALIAS("_FUN_4ba0c0", "_Effect_New4dd2a0")
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
extern "C" int __cdecl Copter_SetSideState(int i, int state, BYTE* s);
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
    CallC(FUN_4ba0c0, &m, side, (void*)Copter_SetSideState, b);
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

// --- network messages of the copter and the report records of the
// copter's objects (sent to [FUN_5e52b0] when they are destroyed)

extern "C" BYTE* __fastcall DpMessage_Construct(BYTE* self, void*);   // sound_gate.cpp

static void Copter_SendPair(int code, int a, int b)
{
    if (F(int, g_6cec6c, 1300) != 1)
        return;
    BYTE msg[28];
    int rec[2] = {a, b};
    DpMessage_Construct(msg, 0);
    CallThis(FUN_603110, msg, code, rec, 8);
    CallThis(FUN_5c3630, g_696ca0, msg);
    CallThis(FUN_602e80, msg);
}

// rec: the keys, the multiplayer flag, the object's name and the second name
static void Copter_Report(BYTE* self, int key, const int* extra, int nextra, const char* name2, int size)
{
    BYTE rec[80];
    memset(rec, 0, sizeof rec);
    BYTE* r = CallThis<BYTE*>(FUN_5ef640, self + 256);
    int o = 0;
    F(int, rec, o) = key;
    o += 4;
    for (int i = 0; i < nextra; i++, o += 4)
        F(int, rec, o) = extra[i];
    F(int, rec, o) = F(int, g_6cec6c, 1300) <= 1;
    o += 4;
    strncpy((char*)rec + o, F(const char*, r, 12), 31);
    strncpy((char*)rec + o + 32, name2, 31);
    CallThis(FUN_5e52b0, F(BYTE*, self, 36) + 704, key, F(int, F(BYTE*, self, 292), 60), size, rec);
}

extern "C" {

// [FUN_569000] message 963 (a, b) when in a network game
void __cdecl Copter_Send963(int a, int b) { Copter_SendPair(963, a, b); }

// [FUN_5691c0]
void __cdecl Copter_Send962(int a, int b) { Copter_SendPair(962, a, b); }

// [FUN_569250]
void __cdecl Copter_Send961(int a, int b) { Copter_SendPair(961, a, b); }

// [FUN_569380]
void __cdecl Copter_Send956(int a, int b) { Copter_SendPair(956, a, b); }

// [FUN_569930] destructor: the two path objects at +1288 and +1292
void __fastcall Copter569930_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64fdd4;
    for (int k = 1288; k <= 1292; k += 4) {
        BYTE* p = F(BYTE*, self, k);
        if (p) {
            CallThis(FUN_5bc2f0, p);
            operator delete(p);
        }
    }
    CallThis(FUN_5d3290, self);
}

// [FUN_56aef0] the copter task on the vehicle v
BYTE* __fastcall CopterTask_Construct(BYTE* self, void*, BYTE* v)
{
    CallThis(FUN_5d31e0, self, 11, (const char*)s_67b970);
    F(void*, self, 0) = g_vtbl64fe34;
    F(int, self, 60) = 0;
    F(BYTE*, self, 40) = v;
    CallThis(FUN_5d3570, v + 2992, self);
    CallThis(FUN_5d33d0, self, 1);
    F(int, self, 56) = 0;
    F(int, self, 52) = 0;
    F(int, self, 48) = 0;
    F(int, self, 44) = CallC<int>(FUN_5ef3d0, CallC<int>(FUN_568fd0) == 2 ? (const char*)s_67b95c : (const char*)s_67b948);
    return self;
}

// [FUN_56bac0] resets the copter's global state
void __cdecl CopterState_Init()
{
    BYTE* s = (BYTE*)g_copterState;
    for (int k = 0; k <= 36; k += 4)
        F(int, s, k) = 0;
    F(int, s, 40) = g_noCar;
    F(int, s, 44) = -1;
    F(int, s, 48) = -1;
    F(int, s, 52) = -1;
    F(int, s, 64) = 0;
}

// [FUN_573700] destructor; reports the object
void __fastcall CopterObj573700_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64ff70;
    F(void*, self, 256) = g_vtbl64ff24;
    int extra[1] = {F(int, self, 416)};
    Copter_Report(self, F(int, self, 368), extra, 1, (const char*)self + 384, 76);
    CallThis(FUN_5a47f0, self);
}

// [FUN_573890]
void __fastcall CopterObj573890_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64ffec;
    F(void*, self, 256) = g_vtbl64ffa0;
    Copter_Report(self, F(int, self, 356), 0, 0, (const char*)self + 376, 72);
    CallThis(FUN_5a47f0, self);
}

// [FUN_573a50]
void __fastcall CopterObj573a50_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl650068;
    F(void*, self, 256) = g_vtbl65001c;
    int extra[1] = {F(int, self, 408)};
    Copter_Report(self, F(int, self, 356), extra, 1, (const char*)self + 376, 76);
    CallThis(FUN_5a47f0, self);
}

// [FUN_573c10]
void __fastcall CopterObj573c10_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl6500e4;
    F(void*, self, 256) = g_vtbl650098;
    int extra[1] = {F(int, self, 408)};
    Copter_Report(self, F(int, self, 356), extra, 1, (const char*)self + 376, 76);
    CallThis(FUN_5a47f0, self);
}

}

extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_5a6160(); KING_ALIAS("_FUN_5a6160", "@Gauge_Construct@48")
ASM_VAR(char, g_vtbl64fe60, $L_64fe60)
ASM_VAR(int, g_6968a8, $L_6968a8)

extern "C" {

// [FUN_56d560] constructor of the wipers (nodes Wiper*, GlassKey, rain_node)
BYTE* __fastcall Wipers_Construct(BYTE* self, void*, int a)
{
    BYTE* w2s = CallC<BYTE*>(FUN_5ef2b0, 0, "Wiper2Space");
    BYTE* w2 = CallC<BYTE*>(FUN_5ef2b0, 0, "Wiper2");
    BYTE* w1s = CallC<BYTE*>(FUN_5ef2b0, 0, "Wiper1Space");
    BYTE* w1 = CallC<BYTE*>(FUN_5ef2b0, 0, "Wiper1");
    BYTE* ws = CallC<BYTE*>(FUN_5ef2b0, 0, "WiperSpace");
    BYTE* w = CallC<BYTE*>(FUN_5ef2b0, 0, "Wiper");
    BYTE* glass = CallC<BYTE*>(FUN_5ef2b0, 0, "GlassKey");
    BYTE* rain = CallC<BYTE*>(FUN_5ef2b0, 0, "rain_node");
    CallThis(FUN_5a6160, self, a, rain, glass, "glass", w, ws, w1, w1s, w2, w2s);
    F(void*, self, 0) = &g_vtbl64fe60;
    F(int, self, 212) = 1;
    F(DWORD, self, 164) |= 14;
    F(BYTE*, self, 208) = g_6968a8 ? CallC<BYTE*>(FUN_5ef3d0, "wipersSound") : 0;
    return self;
}

}

extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")    // message: constructor
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")    // message: set (id, data, size)
extern "C" void FUN_5c3630(); KING_ALIAS("_FUN_5c3630", "@Media_RecordBlock@12")    // sends a message
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")    // message: destructor

static void CopyText(char* to, const char* s)
{
    if (!s) {
        *to = 0;
        return;
    }
    while ((*to++ = *s++) != 0)
        ;
}

extern "C" {

// [FUN_56fae0] sends message 958 (120 bytes: a, two names, 12 dwords, b)
// when the game runs networked (+1300 == 1)
void __cdecl Net_SendObjectMessage(int a, int b, const char* name1, const char* name2,
                                   DWORD d0, DWORD d1, DWORD d2, DWORD d3, DWORD d4, DWORD d5,
                                   DWORD d6, DWORD d7, DWORD d8, DWORD d9, DWORD d10, DWORD d11)
{
    if (F(int, g_6cec6c, 1300) != 1)
        return;
    BYTE m[148];
    CallThis(FUN_602be0, m);
    F(int, m, 28) = a;
    CopyText((char*)m + 32, name1);
    CopyText((char*)m + 64, name2);
    DWORD d[12] = {d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11};
    for (int i = 0; i < 12; i++)
        F(DWORD, m, 96 + i * 4) = d[i];
    F(int, m, 144) = b;
    CallThis(FUN_603110, m, 958, m + 28, 120);
    CallThis(FUN_5c3630, g_696ca0, m);
    CallThis(FUN_602e80, m);
}

}

extern "C" void FUN_4297b0(); KING_ALIAS("_FUN_4297b0", "_AI_True")
extern "C" void FUN_569380(); KING_ALIAS("_FUN_569380", "_Copter_Send956")
extern "C" void FUN_5a40b0(); KING_ALIAS("_FUN_5a40b0", "@Node_Position@12")
extern "C" void FUN_5ef640(); KING_ALIAS("_FUN_5ef640", "@Node_Root@8")
ASM_PROC(FUN_5a4f00)    // explosion effect: constructor
ASM_VAR(int, g_6f33f0, $L_6f33f0)

extern "C" {

// [FUN_56f240] blows up the mine with this id (sound, explosion effect)
void __cdecl Mine_Explode(int id)
{
    if (g_6f33f0 || !CallC<bool>(FUN_4297b0))
        return;
    BYTE* r = CallThis<BYTE*>(FUN_5e5370, F(BYTE*, g_6cec6c, 1356) + 704, id);
    if (!r)
        return;
    BYTE* s = F(BYTE*, r, 28);
    if (!s)
        return;
    if (F(int, s, 360) && F(int, s, 408) == 1) {
        CallC(FUN_569380, F(int, s, 344), 1);
        F(int, s, 408) = 2;
        CallC(FUN_569380, F(int, s, 344), 2);
        CallVirt(s, 24, 1);
    }
    DWORD v[3] = {0, 0, 0};
    CallThis(FUN_5a40b0, s, v);
    BYTE* q = (BYTE*)operator new(192);
    BYTE* e = 0;
    if (q) {
        BYTE* root = CallThis<BYTE*>(FUN_5ef640, s + 256);
        e = CallThis<BYTE*>(FUN_5a4f00, q, F(BYTE*, g_6cec6c, 1356), root, v[0], v[1], v[2], 0,
                            0x40000000, "mineExplose", "mineSound", 3, 0x9999999Au, 0x3fb99999);
    }
    CallVirt(e, 24, 1);
}

}

extern "C" double __cdecl Game_Time();
extern "C" void __cdecl Switch_Select(int sw, int state);
ASM_PROC(FUN_567bb0)

// [FUN_568680] the helicopter's blinking warning lights: the state by the time since the last change
extern "C" void __fastcall Copter_UpdateWarning(BYTE* s, void*)
{
    if (F(int, s, 3284))
        CallThis(FUN_567bb0, s);
    float d = (float)(Game_Time() - F(float, s, 3288));
    if (!(F(float, s, 3288) >= F(float, s, 3292)))
        d = (float)(Game_Time() - F(float, s, 3292));
    int st = F(int, s, 3296);
    if (d >= 1.0f)
        st = 0;
    else if (!(d >= 0.1) || st == 0)
        st = 1;
    else if (!(d >= 0.2) || st == 1)
        st = 2;
    else if (!(d >= 0.3) || st == 2)
        st = 3;
    else
        st = 0;
    F(int, s, 3296) = st;
    if (!(F(float, s, 3288) >= F(float, s, 3292))) {
        Switch_Select(F(int, s, 3304), F(int, s, 3296));
        Switch_Select(F(int, s, 3300), 0);
    } else {
        Switch_Select(F(int, s, 3304), 0);
        Switch_Select(F(int, s, 3300), F(int, s, 3296));
    }
}

extern "C" double __cdecl Game_FrameTime();

// [FUN_5613d0] the helicopter's three control axes: -1/0/1 from the key pairs, ramped in over 0.6 s
extern "C" void __fastcall Copter_UpdateAxes(DWORD* s, void*)
{
    float* f = (float*)s;
    float dt = (float)Game_FrameTime();
    double now = Game_Time();
    if (s[5]) {
        if (s[10] == s[11]) s[11] = s[10] = 0;
        int on = s[11] ? 1 : (s[10] ? -1 : 0);
        s[17] = on;
        if (on != (int)s[2]) f[14] = (float)now;
        double v = ((now - f[14]) + (double)dt * 3.0) * 1.6666666269302368f;
        s[2] = on;
        if (v > 1.0) v = 1.0;
        f[20] = (float)(v * on);
    }
    if (s[4]) {
        if (s[6] == s[7]) s[7] = s[6] = 0;
        int on = s[6] ? 1 : (s[7] ? -1 : 0);
        s[16] = on;
        if (on != (int)s[1]) f[13] = (float)now;
        double v = ((now - f[13]) + (double)dt * 3.0) * 1.6666666269302368f;
        s[1] = on;
        if (v > 1.0) v = 1.0;
        f[19] = (float)(v * on);
    }
    if (s[3]) {
        if (s[8] == s[9]) s[9] = s[8] = 0;
        int on = s[8] ? 1 : (s[9] ? -1 : 0);
        s[15] = on;
        if (on != (int)s[0]) f[12] = (float)now;
        f[18] = (float)on;
        s[0] = on;
    }
}

#include "x87.h"

// [FUN_561dc0] the helicopter's tilt for a control input: *bank from the clamped turn, the result clamped to a limit that shrinks with speed
extern "C" double __fastcall Copter_Tilt(BYTE* s, void*, float input, float turn, const float* dir, float* bank)
{
    const float k = 0.39269909262657166f;
    double a = turn;
    if (a > k) a = k;
    float lo = -k;
    if (!(a >= lo)) a = lo;
    *bank = (float)(x87_sin(a) * 30.0f);
    double base = (double)input * F(float, s, 128) + turn;
    float vd = (float)(((double)F(float, s, 108) * dir[2] + (double)F(float, s, 104) * dir[1]) + (double)F(float, s, 100) * dir[0]);
    int back = !(vd >= 0.0f);
    int fwd = F(float, s, 128) > 0.0f;
    double f;
    if (back == fwd)
        f = 1.0f;
    else {
        double av = vd < 0 ? -(double)vd : (double)vd;
        if (!(av >= 5.0))
            f = 1.0f;
        else if (!(av >= 30.0))
            f = 1.0 - (av - 5.0) * 0.04;
        else
            f = 0.0f;
    }
    float lim = (float)(((double)0.39269909262657166f - 0.19634954631328583f) * f + 0.19634954631328583f);
    double rr = (f * F(float, s, 128) * input * 3.0f + base) * F(float, s, 3228);
    float r = (float)rr;
    if (rr > lim)
        return lim;
    if (!(r >= -lim))
        return -lim;
    return r;
}

extern "C" int __cdecl Query_4e7476(int a, int b, int c, BYTE* d, BYTE* e, BYTE* f);
extern "C" BYTE* __cdecl Car_InListFrom16(BYTE* p);
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")

// [FUN_5663c0] the helicopter's state towards its target: 1 after 60 s, 2 when it took too long for the distance, 0 (restarted) when blocked or not yet due
extern "C" int __fastcall Copter_TargetState(BYTE* s, void*)
{
    if (!F(BYTE*, s, 3248)) {
        F(int, s, 3276) = -1;
        return -1;
    }
    double td = Game_Time() - F(float, s, 3280);
    float t = (float)td;
    if (!(td > 1.0))
        return F(int, s, 3276);
    BYTE node[256];
    BYTE m[48];
    CallThis(FUN_5ef5c0, node);
    BYTE* mat = CallThis<BYTE*>(FUN_57de90, F(BYTE*, s, 3248) + 16, m);
    int r = Query_4e7476(F(int, s, 96), F(int, s, 3480), 0, s + 36, mat + 36, node);
    BYTE* hit = F(BYTE*, node, 116);
    if (!(r & 1) || !hit || (Car_InListFrom16(hit) != F(BYTE*, s, 3248) && Car_InListFrom16(hit) != F(BYTE*, s, 3252))) {
        Vec3 v = *(Vec3*)(F(BYTE*, s, 3248) + 432);
        float a = (float)Vec3_Length(&v);
        double b = Vec3_Length((Vec3*)(s + 100));
        if (t > 60.0f) {
            F(int, s, 3276) = 1;
            return 1;
        }
        if (!((b + a) * 0.2 + 25.0 >= t)) {
            F(int, s, 3276) = 2;
            return 2;
        }
    }
    F(float, s, 3280) = (float)Game_Time();
    F(int, s, 3276) = 0;
    return 0;
}

extern "C" BYTE* __fastcall Obj541960_Self(BYTE* self, void*);
ASM_VAR(BYTE*, g_objects6ef1f8, $L_6ef1f8)

// [FUN_566020] the nearest vehicle the helicopter can see (over 1 m away; within 500 m when a or b is set)
extern "C" BYTE* __fastcall Copter_NearestVisible(BYTE* s, void*, int a, int b)
{
    BYTE* it = g_objects6ef1f8 ? g_objects6ef1f8 - 4 : 0;
    BYTE* best = 0;
    float bestD = 100000.0f;
    float px = F(float, s, 36), py = F(float, s, 40), pz = F(float, s, 44);
    while (it) {
        BYTE* o = Obj541960_Self(it, 0);
        BYTE m[48];
        BYTE* mat = CallThis<BYTE*>(FUN_57de90, o + 16, m);
        float dx = (float)((double)F(float, mat, 36) - px);
        float dy = (float)((double)F(float, mat, 40) - py);
        double dz = (double)F(float, mat, 44) - pz;
        float d = (float)(((double)dx * dx + (double)dy * dy) + dz * dz);
        bool first;
        if (a)
            first = F(int, o, 1220) != 0;
        else
            first = F(int, o, 9544) == b || !b;
        bool take = false;
        if (first && !(d >= bestD) && d > 1.0f)
            take = true;
        else if (F(int, o, 20944)) {
            if (!best || !F(int, best, 20944))
                take = true;
            else if (!(d >= bestD) && d > 1.0f)
                take = true;
        }
        if (take) {
            bestD = d;
            best = o;
        }
        it = F(BYTE*, it, 8);
        if (!it)
            break;
        it -= 4;
    }
    if (!best)
        return 0;
    if ((a || b) && bestD >= 250000.0f)
        return best;
    BYTE node[256];
    BYTE m[48];
    CallThis(FUN_5ef5c0, node);
    BYTE* mat = CallThis<BYTE*>(FUN_57de90, best + 16, m);
    int r = Query_4e7476(F(int, s, 96), F(int, s, 3480), 0, s + 36, mat + 36, node);
    BYTE* other = 0;
    if (CallVirt<int>(best, 4) == 1)
        other = F(BYTE*, best, 21880);
    BYTE* hit = F(BYTE*, node, 116);
    if ((r & 1) && hit) {
        if (Car_InListFrom16(hit) == best)
            return best;
        if (Car_InListFrom16(hit) == other)
            return best;
    }
    return 0;
}

extern "C" void FUN_5cf180(); KING_ALIAS("_FUN_5cf180", "@MediaObj_Construct@20")    // a screen object's constructor (owner, name, -)
ASM_PROC(FUN_5cec90)    // a gauge's constructor
ASM_VAR(char, g_strZero, $L_67c178)      // "zero"
ASM_VAR(char, g_vtbl64feec, $L_64feec)
ASM_VAR(char, g_vtbl64fea0, $L_64fea0)

// [FUN_56efc0] a new 160-byte gauge object with its needle (range d / 7; colours by the flag), shown at once
extern "C" void __cdecl Gauge_Create(DWORD x0, DWORD y0, DWORD z0, DWORD x1, DWORD y1, DWORD z1, double range, int alt)
{
    BYTE* o = (BYTE*)operator new(160);
    if (o) {
        CallThis(FUN_5cf180, o, F(int, g_6cec6c, 1356), &g_strZero, 0);
        DWORD colors[4] = { 0xFF1E1E1E, 0xFF646464, 0xFF1E1E1E, 0xFF323C1E };
        F(void*, o, 0) = &g_vtbl64feec;
        F(void*, o, 32) = &g_vtbl64fea0;
        BYTE* p = (BYTE*)operator new(32);
        if (p) {
            struct V3 { DWORD x, y, z; } a = { x0, y0, z0 }, b = { x1, y1, z1 };
            F(BYTE*, o, 96) = CallThis<BYTE*>(FUN_5cec90, p, 10, a, b, range * 0.14285714285714285, alt ? colors + 2 : colors, 2, 0);
        } else
            F(BYTE*, o, 96) = 0;
    }
    CallVirt(o, 24, 1);
}

extern "C" void FUN_43fc40(); KING_ALIAS("_FUN_43fc40", "_Ai_GetPlayerData")    // a vehicle's info by id (id, out)
ASM_PROC(FUN_5ea1b0)    // text in a box
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")    // a vehicle's flags by id
ASM_VAR(void*, g_font6ced24, $L_6ced24)
ASM_VAR(char, g_fmtPercent, $L_67c3e0)       // "%d%c"
ASM_VAR(char, g_fmtInt66c8cc, $L_66c8cc)     // "%d"
ASM_VAR(char, g_fmtTenths, $L_67c3d8)        // "%4.1f"
typedef void* ImageTable[1];
ASM_VAR(ImageTable, g_images6f2e38, $L_6f2e38)

// [FUN_574190] one line of the vehicle list: name, damage %, count, distance and the type icon; whether it is the player's
extern "C" int __cdecl CarList_DrawLine(void* surface, int x, int y, int id)
{
    DWORD flags = CallC<DWORD>(FUN_43f620, id);
    BYTE info[256];
    CallC(FUN_43fc40, id, info);
    char text[100];
    CallC(FUN_5ea1b0, surface, x + 45, y + 23, 70, g_font6ced24, F(DWORD, info, 48), 6, 0);
    sprintf(text, &g_fmtPercent, (int)(F(double, info, 104) * 100.0), 37);
    CallC(FUN_5ea170, surface, x + 135, y + 23, g_font6ced24, text, 6, 0);
    sprintf(text, &g_fmtInt66c8cc, F(int, info, 80));
    CallC(FUN_5ea170, surface, x + 220, y + 23, g_font6ced24, text, 6, 0);
    sprintf(text, &g_fmtTenths, F(double, info, 112));
    CallC(FUN_5ea170, surface, x + 270, y + 23, g_font6ced24, text, 6, 0);
    if (flags & 0x1000)
        CallC(FUN_5b8980, surface, x, y, g_images6f2e38[F(int, info, 56)], 0);
    return F(int, info, 8) == 1;
}

extern "C" int* __cdecl Players_First(int* out);
extern "C" int* __cdecl Players_Next(int* out, int id);
ASM_VAR(int, g_noVehicle68b990h, $L_68b990)

// [FUN_574310] the vehicle list: up to 60 moving vehicles sorted by damage, then the player's own line
extern "C" void __cdecl CarList_Draw(void* surface, int x, int y, int lines)
{
    int ids[60];
    double vals[60];
    for (int i = 0; i < 60; i++) ids[i] = g_noVehicle68b990h;
    int n = 0;
    int cur;
    Players_First(&cur);
    BYTE info[256];
    while (cur != g_noVehicle68b990h && n < 60) {
        CallC(FUN_43fc40, cur, info);
        int type = F(int, info, 8);
        if ((type == 1 || type == 2) && !(CallC<DWORD>(FUN_43f620, cur) & 0x800)) {
            ids[n] = cur;
            vals[n] = F(double, info, 104);
            n++;
        }
        int nx;
        cur = *Players_Next(&nx, cur);
    }
    bool swapped;
    do {
        swapped = false;
        for (int k = 0; k < n - 1; k++) {
            CallC(FUN_43fc40, ids[k + 1], info);
            if (!(vals[k] >= vals[k + 1]) || (!(vals[k] < vals[k + 1] || vals[k] > vals[k + 1]) && F(int, info, 8) == 1)) {
                double v = vals[k];
                vals[k] = vals[k + 1];
                vals[k + 1] = v;
                int t = ids[k];
                ids[k] = ids[k + 1];
                ids[k + 1] = t;
                swapped = true;
            }
        }
    } while (swapped && n - 1 > 0);
    int acc = 0;
    int i = 0;
    int line = y;
    for (; i < n; i++, line += 33) {
        if (i >= lines)
            break;
        acc |= CarList_DrawLine(surface, x, line, ids[i]);
        if (i == lines - 2 && !acc) {
            i++;
            acc = 0;
            break;
        }
    }
    if (acc)
        return;
    int me;
    Player_CarId(&me);
    CarList_DrawLine(surface, x, y + i * 33, me);
}

extern "C" void FUN_565fe0(); KING_ALIAS("_FUN_565fe0", "@Copter_565fe0@8")
extern "C" void FUN_565ff0(); KING_ALIAS("_FUN_565ff0", "@Copter_565ff0@12")
ASM_PROC(FUN_5662f0)

// [FUN_566380] handles a command: checks it (b == 0: default check), then runs it when allowed or when it is 11
extern "C" void __fastcall Copter_Command(void* self, int, int cmd, int b)
{
    int r = b == 0 ? CallThis<int>(FUN_565fe0, self) : CallThis<int>(FUN_565ff0, self, b);
    if (r || cmd == 11)
        CallThis(FUN_5662f0, self, cmd);
}

extern "C" void FUN_5a71d0(); KING_ALIAS("_FUN_5a71d0", "@Objects_Release@8")
extern "C" void FUN_6148a0(); KING_ALIAS("_FUN_6148a0", "@Terrain_FreeObjects@8")
ASM_VAR(BYTE*, g_objs6ce378, $L_6ce378)
ASM_VAR(int, g_6f340c, $L_6f340c)
ASM_VAR(int, g_replayOpen6f349c, $L_6f349c)
ASM_VAR(BYTE*, g_shell6cec6cH, $L_6cec6c)
ASM_VAR(BYTE*, g_terrain6f33b4, $L_6f33b4)
ASM_VAR(int, g_6ce37c, $L_6ce37c)
ASM_VAR(int, g_6ce384, $L_6ce384)
ASM_VAR(int, g_6ce380, $L_6ce380)
ASM_VAR(int, g_6f3430, $L_6f3430)
ASM_VAR(int, g_6f3434, $L_6f3434)
ASM_VAR(int, g_6f3438, $L_6f3438)
ASM_VAR(int, g_w6f3498, $L_6f3498)
ASM_VAR(int, g_6f33ac, $L_6f33ac)
ASM_VAR(int, g_6f33b0, $L_6f33b0)

// [FUN_56eea0] frees the world's objects, closes the replay file and frees the terrain
extern "C" void __cdecl World_FreeAll()
{
    BYTE* o = g_objs6ce378;
    g_6f340c = 0;
    if (o) {
        CallThis(FUN_5a71d0, o);
        operator delete(o);
        g_objs6ce378 = 0;
    }
    if (g_replayOpen6f349c) {
        BYTE* r = F(BYTE*, g_shell6cec6cH, 1356);
        FILE* f = F(FILE*, r, 7928);
        if (f) {
            fclose(f);
            F(FILE*, r, 7928) = 0;
            F(int, r, 7936) = 0;
        }
        g_replayOpen6f349c = 0;
    }
    BYTE* t = g_terrain6f33b4;
    g_6ce37c = 0;
    g_6ce384 = 0;
    g_6ce380 = 0;
    g_6f3430 = 0;
    g_6f3434 = 0;
    g_6f3438 = 0;
    g_w6f3498 = 0;
    g_6f33ac = 0;
    g_6f33b0 = 0;
    if (t) {
        CallThis(FUN_6148a0, t);
        operator delete(t);
    }
    g_terrain6f33b4 = 0;
}

extern "C" void FUN_4363c0(); KING_ALIAS("_FUN_4363c0", "_Players_First")    // the first player
extern "C" void FUN_4363f0(); KING_ALIAS("_FUN_4363f0", "_Players_Next")    // the next player
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")    // a player's flags
extern "C" void FUN_43f690(); KING_ALIAS("_FUN_43f690", "_Player_SetFlags")    // sets a player's flags
extern "C" void FUN_4278b0(); KING_ALIAS("_FUN_4278b0", "_AI_EndWaiting")
extern "C" void FUN_427520(); KING_ALIAS("_FUN_427520", "_Ai_StartRaces")
ASM_VAR(int, g_players68b990h, $L_68b990)
ASM_VAR(int, g_waiting696328, $L_696328)

// [FUN_56d6e0] clears player flag 4 of the players flagged 0x4000, ends the waiting; in a network game resets the helicopter's state
extern "C" void __cdecl Copter_ResetPlayers()
{
    if (!F(int, g_shell6cec6cH, 1316) || F(int, g_6cec98, 384) != 2) {
        int it;
        CallC(FUN_4363c0, &it);
        while (it != g_players68b990h) {
            if (CallC<int>(FUN_43f620, it) & 0x4000)
                CallC(FUN_43f690, it, CallC<int>(FUN_43f620, it) & ~4);
            int next;
            it = *CallC<int*>(FUN_4363f0, &next, it);
        }
        if (g_waiting696328)
            CallC(FUN_4278b0);
        else
            CallC(FUN_427520);
    }
    if (F(int, g_shell6cec6cH, 1316)) {
        F(int, (BYTE*)g_6f3430, 60) = 0;
        F(DWORD, (BYTE*)g_6f33ac, 572) &= 0xfffeffff;
    }
}

extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")    // scene object base constructor (kind, name)
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_5bd5a0(); KING_ALIAS("_FUN_5bd5a0", "@Style_Assign@12")
extern "C" void FUN_5e8c60(); KING_ALIAS("_FUN_5e8c60", "_Font_LoadFile")    // loads a font
extern "C" void FUN_568fd0(); KING_ALIAS("_FUN_568fd0", "_Game_568fd0")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")    // finds a scene object by name
ASM_VAR(char, vt_64fdd4, $L_64fdd4)
ASM_VAR(char, s_67b8a8, $L_67b8a8)
ASM_VAR(char, g_6ce3e0, $L_6ce3e0)
ASM_VAR(char, s_67b89c, $L_67b89c)
ASM_VAR(char, s_67b890, $L_67b890)
ASM_VAR(char, s_67b880, $L_67b880)
ASM_VAR(char, s_67b870, $L_67b870)
ASM_VAR(char, s_67b860, $L_67b860)
ASM_VAR(char, s_67b850, $L_67b850)
ASM_VAR(char, s_67b840, $L_67b840)
ASM_VAR(char, s_67b830, $L_67b830)
ASM_VAR(char, s_67b824, $L_67b824)
ASM_VAR(char, s_67b814, $L_67b814)

// [FUN_569760] constructor of the helicopter screen object: fonts, the mode's three scene objects, two more, the start time
extern "C" BYTE* __fastcall CopterScreen_Construct(BYTE* self, int, BYTE* game)
{
    CallThis(FUN_5d31e0, self, 11, &s_67b8a8);
    BYTE* t = self + 1220;
    for (int off = 0; off <= 32; off += 4)
        F(DWORD, t, off) = 0;
    F(DWORD, t, 24) = 0x3f000000;
    *(void**)self = &vt_64fdd4;
    for (int k = 0; k < 4; k++) {
        self[44 + k * 128] = 0;
        self[684 + k * 128] = 0;
    }
    self[556] = 0;
    F(int, self, 1216) = 0;
    F(int, self, 1212) = 0;
    F(int, self, 1204) = 0;
    F(int, self, 1208) = 0;
    F(int, self, 1200) = 0;
    F(BYTE*, self, 40) = game;
    CallThis(FUN_5d3570, game + 2992, self);
    CallThis(FUN_5d33d0, self, 1);
    F(int, self, 32) = 0;
    F(int, self, 36) = 0;
    F(int, self, 1196) = 0;
    CallThis(FUN_5bd5a0, t, &g_6ce3e0);
    F(int, t, 0) = 5;
    F(void*, self, 1288) = CallC<void*>(FUN_5e8c60, &s_67b89c);
    F(void*, self, 1292) = CallC<void*>(FUN_5e8c60, &s_67b890);
    if (CallC<int>(FUN_568fd0) == 2) {
        F(void*, self, 1260) = CallC<void*>(FUN_5ef3d0, &s_67b880);
        F(void*, self, 1264) = CallC<void*>(FUN_5ef3d0, &s_67b870);
        F(void*, self, 1256) = CallC<void*>(FUN_5ef3d0, &s_67b860);
    } else {
        F(void*, self, 1260) = CallC<void*>(FUN_5ef3d0, &s_67b850);
        F(void*, self, 1264) = CallC<void*>(FUN_5ef3d0, &s_67b840);
        F(void*, self, 1256) = CallC<void*>(FUN_5ef3d0, &s_67b830);
    }
    F(void*, self, 1280) = CallC<void*>(FUN_5ef3d0, &s_67b824);
    F(void*, self, 1284) = CallC<void*>(FUN_5ef3d0, &s_67b814);
    F(int, self, 1268) = 0;
    F(int, self, 1276) = 0;
    F(int, self, 1272) = (int)Timer_Now();
    return self;
}

// ---------------------------------------------------------------------------
extern "C" void FUN_53f750(); KING_ALIAS("_FUN_53f750", "_Car_InList")
extern "C" void FUN_568680(); KING_ALIAS("_FUN_568680", "@Copter_UpdateWarning@8")
extern "C" void FUN_50bc90(); KING_ALIAS("_FUN_50bc90", "_Str_ToWide")
ASM_PROC(FUN_568090)
ASM_PROC(FUN_566970)
ASM_PROC(FUN_567dc0)
ASM_PROC(FUN_563600)
ASM_PROC(FUN_561f30)
ASM_PROC(FUN_50c3a0)    // writes the state (stream, 72, 54, 0)
ASM_VAR(AsmLabel, s_67b2bc, $L_67b2bc)
ASM_VAR(AsmLabel, s_67b7ec, $L_67b7ec)
ASM_VAR(AsmLabel, s_fmt6772a8, $L_6772a8)
ASM_VAR(AsmLabel, s_stream6772a0, $L_6772a0)
ASM_VAR(AsmLabel, s_677684, $L_677684)
extern "C" void* g_impStgCreateDocfile; KING_ALIAS("_g_impStgCreateDocfile", "__imp__StgCreateDocfile@16")
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);
extern "C" void __cdecl World_Fatal(const char* msg);

// a COM method at vtbl+off
template <class R = long, class... A>
static R Copter_ComCall(void* p, int off, A... a)
{
    return ((R(__stdcall*)(void*, A...))(*(void***)p)[off / 4])(p, a...);
}

// [FUN_560fd0..$L_56101e] drops n entries of the stack (top +3000, words at +0, used flags at +2000) and returns the top's word
extern "C" unsigned short __fastcall Stack_DropTo(BYTE* self, void*, int n)
{
    if (F(int, self, 3000) > n)
        F(int, self, 3000) -= n;
    int i = F(int, self, 3000);
    if (F(char, self, 2000 + i))
    {
        World_Fatal(s_67b2bc);
        i = ++F(int, self, 3000);
    }
    return F(unsigned short, self, i * 2);
}

// [FUN_5687b0..$L_56881c] unless +3548 is set: refreshes the followed car (+3248, its +21880 into +3252) and runs the copter's update steps
extern "C" void __fastcall Copter_UpdateAll(BYTE* self, void*)
{
    if (F(int, self, 3548))
        return;
    BYTE* car = CallC<BYTE*>(FUN_53f750, F(BYTE*, self, 3248));
    F(BYTE*, self, 3248) = car;
    if (car)
    {
        if (CallVirt<int>(car, 4) == 1)
            F(int, self, 3252) = F(int, F(BYTE*, self, 3248), 21880);
        else
            F(int, self, 3252) = 0;
    }
    CallThis(FUN_568090, self);
    CallThis(FUN_566970, self);
    CallThis(FUN_567dc0, self);
    CallThis(FUN_563600, self);
    CallThis(FUN_561f30, self);
    CallThis(FUN_568680, self);
}

// [FUN_568ed0..$L_568f3c] advances *id to a free object id of the screen (keeping id%1000 below 500); fatal after 500 tries
extern "C" void __cdecl Objects_FreeId(int* id)
{
    int tries = 0;
    while (CallThis<int>(FUN_5e5370, g_696ca0 + 704, *id))
    {
        int v = ++*id;
        if (v % 1000 >= 500)
            *id = v / 1000 * 1000;
        if (++tries > 500)
            Sys_Fatal(s_67b7ec, 0);
    }
}

// [FUN_56cd90..$L_56ce0a] saves the copter state n into a new storage file with one stream
extern "C" void __cdecl Copter_SaveState(int n)
{
    char name[64];
    void* stg;
    void* stream;
    sprintf(name, s_fmt6772a8, n);
    CallStd<long>((void (*)())g_impStgCreateDocfile, CallC<const WCHAR*>(FUN_50bc90, name), 4113, 0, &stg);
    Copter_ComCall(stg, 12, CallC<const WCHAR*>(FUN_50bc90, (char*)s_stream6772a0), 4113, 0, 0, &stream);
    if (!stream)
        Sys_Fatal(s_677684, 0);
    CallC(FUN_50c3a0, stream, 72, 54, 0);
    Copter_ComCall(stream, 8);
    Copter_ComCall(stg, 8);
}


// ---------------------------------------------------------------------------
// Split procs (their code ran on past ENDP after the call to Sys_Fatal).
extern "C" void FUN_52ea90(); KING_ALIAS("_FUN_52ea90", "_CarType_Number")
extern "C" void FUN_568ed0(); KING_ALIAS("_FUN_568ed0", "_Objects_FreeId")
extern "C" void FUN_5e5130(); KING_ALIAS("_FUN_5e5130", "@Resources_Create@16")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")
extern "C" void FUN_5e4900(); KING_ALIAS("_FUN_5e4900", "@ResTable_Charge@12")
extern "C" void FUN_5e4b30(); KING_ALIAS("_FUN_5e4b30", "@Resources_Read@12")
extern "C" void FUN_50bc90(); KING_ALIAS("_FUN_50bc90", "_Str_ToWide")
ASM_PROC(FUN_5e52b0)    // resources: send a message (id, kind, size, data)
ASM_VAR(AsmLabel, g_hcModels6c6b68, $L_6c6b68)     // 876 bytes per car model, the name at +0
ASM_VAR(AsmLabel, s_hc6766b4, $L_6766b4)
ASM_VAR(Vec3, g_hcZero702650, $L_702650)
ASM_VAR(AsmLabel, g_hcParts6ced98, $L_6ced98)        // 11 dwords per car type
ASM_VAR(AsmLabel, s_hc677738, $L_677738)
ASM_VAR(AsmLabel, s_hc67c200, $L_67c200)
ASM_VAR(AsmLabel, s_hc677704, $L_677704)
ASM_VAR(AsmLabel, s_hc6776d4, $L_6776d4)
ASM_VAR(AsmLabel, s_hc67c1d8, $L_67c1d8)
ASM_VAR(int, g_6f32e8, $L_6f32e8)                 // 6 dwords read from the stream 677738
ASM_VAR(int, g_6f32ec, $L_6f32ec)
ASM_VAR(int, g_6f32f0, $L_6f32f0)
ASM_VAR(int, g_6f32f4, $L_6f32f4)
ASM_VAR(int, g_6f32f8, $L_6f32f8)
ASM_VAR(int, g_6f32fc, $L_6f32fc)
ASM_VAR(int, g_6f3310, $L_6f3310)                 // the options of the stream 677704
ASM_VAR(int, g_6f3314, $L_6f3314)
ASM_VAR(int, g_6f3318, $L_6f3318)
ASM_VAR(int, g_6f331c, $L_6f331c)
ASM_VAR(int, g_6f3320, $L_6f3320)
struct Blob48 { DWORD d[12]; };

// [FUN_571a80..$L_571b11] creates the network object of a car type (resource id (type*5+1000)*200 + free id) and sends its 156-byte creation message
extern "C" int __cdecl Net_CreateCarObject(BYTE* res, int kind, const char* name, Blob48 state, int type)
{
    int num = CallC<int>(FUN_52ea90, type);
    int id = (num * 5 + 1000) * 200;
    CallC(FUN_568ed0, &id, type);
    BYTE* table = res + 704;
    CallThis(FUN_5e5130, table, id, kind);
    const char* model = (char*)g_hcModels6c6b68 + type * 876;
    BYTE* obj = CallC<BYTE*>(FUN_5ef2b0, 0, model);
    if (!obj)
        Sys_Fatal(model, s_hc6766b4);
    BYTE msg[156];
    memset(msg, 0, sizeof msg);
    F(int, msg, 0) = num;
    F(int, msg, 4) = id % 1000;
    F(Vec3, msg, 20) = g_hcZero702650;
    F(Vec3, msg, 32) = g_hcZero702650;
    F(Blob48, msg, 44) = state;
    strncpy((char*)msg + 92, name, 31);
    strncpy((char*)msg + 124, F(char*, obj, 12), 31);
    CallThis(FUN_5e52b0, table, id, kind, 156, msg);
    BYTE* r = CallThis<BYTE*>(FUN_5e5370, table, id);
    BYTE* part = ((BYTE**)(char*)g_hcParts6ced98)[type * 11];
    if (part)
        F(int, r, 16) = F(int, part, 16);
    CallThis(FUN_5e4900, r, table);
    return id;
}

// [FUN_571d20..$L_571eeb] reads the three streams of a saved game's storage: 6 dwords, the options (counted ints), the resources
extern "C" void __cdecl Copter_LoadStreams(void* stg)
{
    void* stream = 0;
    ULONG got;
    Copter_ComCall(stg, 16, CallC<const WCHAR*>(FUN_50bc90, (char*)s_hc677738), (void*)0, 16, 0, &stream);
    if (!stream)
        Sys_Fatal(s_hc67c200, 0);
    int six[6];
    Copter_ComCall(stream, 12, six, 24, &got);
    g_6f32e8 = six[0];
    g_6f32ec = six[1];
    g_6f32f0 = six[2];
    g_6f32f4 = six[3];
    g_6f32f8 = six[4];
    g_6f32fc = six[5];
    Copter_ComCall(stream, 8);
    Copter_ComCall(stg, 16, CallC<const WCHAR*>(FUN_50bc90, (char*)s_hc677704), (void*)0, 16, 0, &stream);
    if (stream)
    {
        int o[10] = {};
        o[2] = 1;
        Copter_ComCall(stream, 12, o, 4, &got);
        Copter_ComCall(stream, 12, o, o[0] * 4, &got);   // over the count itself
        g_6f3310 = o[0] % 2;
        g_6f3314 = o[1] % 3;
        g_6f3318 = o[2] % 2;
        g_6f331c = o[3] % 4;
        g_6f3320 = o[4] % 3;
        F(int, g_6cec6c, 1304) = o[5] % 2;
        Copter_ComCall(stream, 8);
    }
    Copter_ComCall(stg, 16, CallC<const WCHAR*>(FUN_50bc90, (char*)s_hc6776d4), (void*)0, 16, 0, &stream);
    if (!stream)
        Sys_Fatal(s_hc67c1d8, 0);
    CallThis(FUN_5e4b30, g_696ca0 + 704, stream);
    Copter_ComCall(stream, 8);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_437220(); KING_ALIAS("_FUN_437220", "_Ai_GetPositionIdV")
extern "C" void FUN_437390(); KING_ALIAS("_FUN_437390", "_Ai_GetPositionIdRV")
extern "C" void FUN_438d70(); KING_ALIAS("_FUN_438d70", "_Road_GetRoadParameters")
extern "C" void FUN_4364f0(); KING_ALIAS("_FUN_4364f0", "_AI_Make449c40")
extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
ASM_VAR(int, g_hcTypeClass5, $L_6ce344)
ASM_VAR(int, g_hcCarTypeCount, $L_6ce32c)
ASM_VAR(BYTE*, g_hc6ef204, $L_6ef204)
ASM_VAR(AsmLabel, s_hc672f50, $L_672f50)
ASM_VAR(AsmLabel, s_hc672f78, $L_672f78)
ASM_VAR(AsmLabel, s_hc66d638, $L_66d638)
ASM_VAR(AsmLabel, s_hc66d628, $L_66d628)
ASM_VAR(AsmLabel, s_hc67b2cc, $L_67b2cc)
ASM_VAR(BYTE**, g_hc68cf38, $L_68cf38)          // the road nodes
struct Blob32 { DWORD d[8]; };

// [FUN_568830..$L_568afb] creates the network object of a static model at a position (id n+300000; an empty name takes the position's room name) and sends its 160-byte creation message
extern "C" BYTE* __cdecl Net_CreatePlacedObject(BYTE* res, const char* name, int kind, Vec3 pos, int a7, int n, int a9, int a10)
{
    if (!g_696c5c)
        return 0;
    int id = n + 300000;
    char room[104];
    if (!strcmp(name, g_str68d1c0))
    {
        Blob32 pid;
        CallC(FUN_437220, &pid, pos);
        DWORD info[11] = {};
        info[4] = (DWORD)(char*)s_hc66d638;
        info[5] = (DWORD)(char*)s_hc66d628;
        info[9] = 0xffffffff;
        info[10] = 0xffffffff;
        CallC(FUN_438d70, pid, info);
        strcpy(room, (char*)info[5]);
        name = room;
    }
    BYTE* table = res + 704;
    CallThis(FUN_5e5130, table, id, kind);
    BYTE m[48];
    CallC(FUN_5abc40, m, pos);
    BYTE msg[160];
    memset(msg, 0, sizeof msg);
    F(int, msg, 0) = g_hcTypeClass5 < g_hcCarTypeCount ? g_hcTypeClass5 : 0;
    F(int, msg, 4) = 11;
    F(int, msg, 12) = a9;
    F(int, msg, 16) = a10;
    F(Vec3, msg, 20) = g_zeroVec;
    F(Vec3, msg, 32) = g_zeroVec;
    memcpy(msg + 44, m, 48);
    F(int, msg, 92) = a7;
    strncpy((char*)msg + 96, name, 31);
    strncpy((char*)msg + 128, s_hc672f78, 31);
    CallThis(FUN_5e52b0, table, id, kind, 160, msg);
    BYTE* r = CallThis<BYTE*>(FUN_5e5370, table, id);
    if (!g_hc6ef204)
    {
        World_Fatal(s_hc672f50);
        return r;
    }
    F(int, r, 16) = F(int, g_hc6ef204, 16);
    return r;
}

// [FUN_5610f0..$L_5612f3] puts the road of a point (from a room key) on the route stack: drops back to it when it is there (1 when it wasn't the top, 0 at the top), else pushes it: 2 as a pair, 3/4 one end, -3 full, -1/-2 not found
extern "C" int __fastcall Route_PushRoad(BYTE* self, void*, void* key, Vec3* p)
{
    int room;
    CallC(FUN_4364f0, &room, key);
    if (room == -1)
        return -1;
    int node;
    CallC(FUN_437390, &node, room, *p);
    if (node == -1)
        return -2;
    BYTE* r = g_hc68cf38[node];
    short a = F(short, F(BYTE*, r, 112), 16);
    short b = F(short, F(BYTE*, r, 116), 16);
    int i = 0;
    short x, y;
    for (;;)
    {
        x = Stack_Peek(self, 0, i);
        y = Stack_Peek(self, 0, i + 1);
        if ((a == x && b == y) || (a == y && b == x))
        {
            BYTE* used = self + 2000 + (F(int, self, 3000) - i);
            if (*used)
                *used = 0;
            Stack_DropTo(self, 0, i);
            return i != 0;
        }
        i++;
        if (!y || i >= F(int, self, 3000))
            break;
    }
    x = Stack_Peek(self, 0, 0);
    y = Stack_Peek(self, 0, 1);
    if (self[F(int, self, 3000) + 1999] && a != x && b != x && (a == y || b == y))
    {
        Stack_SwapTop(self, 0);
        x = Stack_Peek(self, 0, 0);
        y = Stack_Peek(self, 0, 1);
    }
    if (a == y || b == y)
    {
        Stack_DropTo(self, 0, 1);
        x = Stack_Peek(self, 0, 0);
        y = Stack_Peek(self, 0, 1);
    }
    if (a != x)
    {
        if (b != x)
            return Stack_PushPair(self, 0, a, b) ? 2 : -3;
        if (Stack_Push(self, 0, a))
            return 3;
    }
    if (b != x && Stack_Push(self, 0, b))
        return 4;
    World_Fatal(s_hc67b2cc);
    return -4;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_569930(); KING_ALIAS("_FUN_569930", "@Copter569930_Destroy@8")
extern "C" void FUN_56afb0(); KING_ALIAS("_FUN_56afb0", "@Notice_Destruct@8")
ASM_PROC(FUN_56d6b0)
ASM_VAR(DWORD, g_hel6f3248, $L_6f3248)
ASM_VAR(DWORD, g_hel6f324c, $L_6f324c)
ASM_VAR(DWORD, g_hel6f3250, $L_6f3250)
ASM_VAR(DWORD, g_hel6f3254, $L_6f3254)
ASM_VAR(DWORD, g_hel6f32e8, $L_6f32e8)
ASM_VAR(DWORD, g_hel6f32ec, $L_6f32ec)
ASM_VAR(DWORD, g_hel6f32f0, $L_6f32f0)
ASM_VAR(DWORD, g_hel6f32f4, $L_6f32f4)
ASM_VAR(DWORD, g_hel6f32f8, $L_6f32f8)
ASM_VAR(DWORD, g_hel6f32fc, $L_6f32fc)
ASM_VAR(DWORD, g_hel6f3300, $L_6f3300)
ASM_VAR(DWORD, g_hel6f3304, $L_6f3304)
ASM_VAR(DWORD, g_hel6f3308, $L_6f3308)
ASM_VAR(DWORD, g_hel6f330c, $L_6f330c)

static BYTE* Hel_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_569910]
extern "C" BYTE* __fastcall Hel569910_DeletingDestructor(BYTE* s, void*, int flags) { return Hel_DeleteAfter(s, flags, FUN_569930); }
// [$L_56af90]
extern "C" BYTE* __fastcall Hel56af90_DeletingDestructor(BYTE* s, void*, int flags) { return Hel_DeleteAfter(s, flags, FUN_56afb0); }
// [$L_56d690]
extern "C" BYTE* __fastcall Hel56d690_DeletingDestructor(BYTE* s, void*, int flags) { return Hel_DeleteAfter(s, flags, FUN_56d6b0); }
// [$L_56f3b0]
extern "C" void __cdecl HelInit_56f3b0() { g_hel6f3248 = 0x0; g_hel6f324c = 0x0; g_hel6f3250 = 0x0; g_hel6f3254 = 0x0; }
// [$L_568e50]
extern "C" void __cdecl HelInit_568e50() { g_hel6f32e8 = 0x0; g_hel6f32ec = 0x0; g_hel6f3300 = 0x47ae147b; g_hel6f3304 = 0x3f847ae1; g_hel6f32f0 = 0x0; g_hel6f32f4 = 0x0; g_hel6f3308 = 0x47ae147b; g_hel6f330c = 0x3f847ae1; g_hel6f32f8 = 0x0; g_hel6f32fc = 0x0; }

extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")
ASM_PROC(FUN_5f98c0)
ASM_PROC(FUN_5f9d20)
extern "C" void FUN_5691c0(); KING_ALIAS("_FUN_5691c0", "_Copter_Send962")
extern "C" void FUN_4297b0(); KING_ALIAS("_FUN_4297b0", "_AI_True")
ASM_VAR(DWORD, g_hel68cf6c, $L_68cf6c)
ASM_VAR(DWORD, g_hel6f2f08, $L_6f2f08)
ASM_VAR(float, g_hel67b294, $L_67b294)
ASM_VAR(float, g_hel67b290, $L_67b290)
ASM_VAR(DWORD, g_hel67b284, $L_67b284)
ASM_VAR(DWORD, g_hel6f2c3c, $L_6f2c3c)
ASM_VAR(float, g_hel6f2c38, $L_6f2c38)
ASM_VAR(BYTE*, g_hel696ca0, $L_696ca0)
ASM_VAR(double, g_hel64b788, $L_64b788)
ASM_VAR(int, g_hel67c4b4, $L_67c4b4)
ASM_VAR(double, g_hel64b670, $L_64b670)
ASM_VAR(double, g_hel6f3520, $L_6f3520)
ASM_VAR(int, g_hel6f33f0, $L_6f33f0)

// [$L_56cd80]
extern "C" int __fastcall Hel_Release56cd80(BYTE* s) { return CallThis<int>(FUN_5d32f0, s, 1); }
// [$L_5736d0]
extern "C" int __fastcall Hel_Release5736d0(BYTE* s) { return CallThis<int>(FUN_5d32f0, s, 1); }

// [$L_56ba90] static initializers
extern "C" void __cdecl HelInit_56ba90() { g_hel6f2f08 = g_hel68cf6c; }
// [$L_561d90]
extern "C" void __cdecl HelInit_561d90()
{
    g_hel6f2c3c = g_hel67b284;
    g_hel6f2c38 = (float)((double)g_hel67b294 / g_hel67b290);
}
// [$L_576d00]
extern "C" void __cdecl HelInit_576d00() { g_hel6f3520 = g_hel64b670 / (double)g_hel67c4b4; }

// [$L_5737e0] FUN_5e1a10 (+96, +152), then FUN_5f98c0 (a, b)
extern "C" int __fastcall Hel_5737e0(BYTE* s, void*, int a, int b)
{
    CallC(FUN_5e1a10, F(int, s, 96), F(int, s, 152));
    return CallThis<int>(FUN_5f98c0, s, a, b);
}

// [$L_573b30] the same with FUN_5f9d20 (a, b, c, d)
extern "C" int __fastcall Hel_573b30(BYTE* s, void*, int a, int b, int c, int d)
{
    CallC(FUN_5e1a10, F(int, s, 96), F(int, s, 152));
    return CallThis<int>(FUN_5f9d20, s, a, b, c, d);
}

// [$L_573960] as 5737e0 unless 6f33f0 is set or FUN_4297b0 says no
extern "C" int __fastcall Hel_573960(BYTE* s, void*, int a, int b)
{
    if (g_hel6f33f0)
        return 0;
    if (!CallThis<char>(FUN_4297b0, s))
        return 0;
    CallC(FUN_5e1a10, F(int, s, 96), F(int, s, 152));
    return CallThis<int>(FUN_5f98c0, s, a, b);
}

// [$L_573810] once the time (+368) plus 64b788 has passed: FUN_5691c0 on +344 with 1, then with +408 (cleared)
extern "C" int __fastcall Hel_573810(BYTE* s)
{
    if (!(F(double, s, 368) + g_hel64b788 >= F(double, g_hel696ca0, 2968))) {
        F(int, s, 408) = 0;
        CallC(FUN_5691c0, F(int, s, 344), 1);
        CallC(FUN_5691c0, F(int, s, 344), F(int, s, 408));
    }
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// ---------------------------------------------------------------------------
// More helicopter tasks and its random generator.

extern "C" void FUN_569380(); KING_ALIAS("_FUN_569380", "_Copter_Send956")
ASM_PROC(FUN_5f9b00)
ASM_VAR(double, g_hel64b730, $L_64b730)
ASM_VAR(double, g_hel64fe58, $L_64fe58)
ASM_VAR(double, g_hel64b7e0, $L_64b7e0)
ASM_VAR(AsmLabel, g_hel6f3160, $L_6f3160)
ASM_VAR(DWORD, g_hel6f3238, $L_6f3238)
ASM_VAR(DWORD, g_hel6f323c, $L_6f323c)
ASM_VAR(DWORD, g_hel6f3240, $L_6f3240)

// [$L_573b70] the start time (+368) once; after 64b670 released, before it state 3; FUN_569380 of +344 with 2 and the state
extern "C" void __fastcall Hel_573b70(BYTE* s)
{
    if (!F(int, s, 16)) {
        F(DWORD, s, 368) = F(DWORD, g_hel696ca0, 2968);
        F(DWORD, s, 372) = F(DWORD, g_hel696ca0, 2972);
    }
    if (F(double, s, 368) + g_hel64b670 > F(double, g_hel696ca0, 2968))
        CallThis(FUN_5d32f0, s, 1);
    else
        F(int, s, 408) = 3;
    CallC(FUN_569380, F(int, s, 344), 2);
    CallC(FUN_569380, F(int, s, 344), F(int, s, 408));
}

// [$L_5739b0] as 5737e0 with FUN_5f9b00; when it succeeds b's record (+100) remembers +100
extern "C" int __fastcall Hel_5739b0(BYTE* s, void*, int a, BYTE* b)
{
    if (g_hel6f33f0)
        return 0;
    if (!CallThis<char>(FUN_4297b0, s))
        return 0;
    CallC(FUN_5e1a10, F(int, s, 96), F(int, s, 152));
    int r = CallThis<int>(FUN_5f9b00, s, a, b);
    if (r) {
        BYTE* o = F(BYTE*, b, 100);
        if (o) {
            if (!F(int, o, 12)) {
                F(void*, o, 12) = operator new(4);
                F(int, o, 8) = 1;
            }
            *F(int*, o, 12) = F(int, s, 100);
        }
    }
    return r;
}

// [$L_56b9f0] the timer (+32) of the mode (+60): modes 5 and 10 end after 64b7e0, the others from 3 after 64fe58
extern "C" int __fastcall Hel_56b9f0(BYTE* s)
{
    if (F(int, s, 16) < 2)
        F(double, s, 32) = F(double, g_hel696ca0, 2968) * g_hel64b730;
    int m = F(int, s, 60);
    if (m >= 3) {
        double lim = F(double, s, 32) + (m == 5 || m == 10 ? g_hel64b7e0 : g_hel64fe58);
        if (F(double, g_hel696ca0, 2968) * g_hel64b730 > lim)
            F(int, s, 60) = 0;
    }
    return CallThis<int>(FUN_5d32f0, s, 1);
}

// [$L_568da0] static initializer: the subtractive random generator (55 words at 6f3160)
extern "C" void __cdecl HelInit_568da0()
{
    DWORD* a = (DWORD*)g_hel6f3160;
    for (int i = 0; i < 55; i++)
        a[i] = 0;
    DWORD mj = 161803398, mk = 1;
    g_hel6f3238 = mj;
    for (unsigned i = 0; i < 1134; i += 21) {
        unsigned d = i % 55;
        mj -= mk;
        a[d] = mk;
        DWORD t = mj;
        mj = a[d];
        mk = t;
    }
    for (int k = 0; k < 4; k++)
        for (unsigned j = 0; j < 55; j++)
            a[j] -= a[(j + 31) % 55];
    g_hel6f323c = 0;
    g_hel6f3240 = 31;
}

// ---------------------------------------------------------------------------
// More virtual methods.

typedef void (*HelFn)();

// the frames of the rotors (+96..+108) by the state bits (+160)
static void Hel_RotorFrames(BYTE* s)
{
    CallC(FUN_5e1a10, F(int, s, 96), F(int, s, 160) ? 1 : 0);
    CallC(FUN_5e1a10, F(int, s, 100), (F(BYTE, s, 160) & 3) ? 1 : 0);
    CallC(FUN_5e1a10, F(int, s, 104), (F(BYTE, s, 160) & 12) ? 1 : 0);
    CallC(FUN_5e1a10, F(int, s, 108), (F(BYTE, s, 160) & 48) ? 1 : 0);
}
// [$L_573590]
extern "C" int __fastcall Hel_573590(BYTE* s, void*, int a, int b)
{
    Hel_RotorFrames(s);
    return CallThis<int>(FUN_5f98c0, s, a, b);
}
// [$L_573630] the same, then fn (the object 256 bytes before, b)
extern "C" int __fastcall Hel_573630(BYTE* s, void*, HelFn fn, int b)
{
    Hel_RotorFrames(s);
    BYTE* o = (DWORD)s != 256 ? s : 0;
    return CallC<int>(fn, o, b);
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

ASM_PROC(FUN_56f110)
// [$L_56f0f0]
extern "C" BYTE* __fastcall Dtor56f0f0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_56f110, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_573700(); KING_ALIAS("_FUN_573700", "@CopterObj573700_Destroy@8")
// [$L_5736e0]
extern "C" BYTE* __fastcall Dtor5736e0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_573700, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_573890(); KING_ALIAS("_FUN_573890", "@CopterObj573890_Destroy@8")
// [$L_573870]
extern "C" BYTE* __fastcall Dtor573870_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_573890, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_573a50(); KING_ALIAS("_FUN_573a50", "@CopterObj573a50_Destroy@8")
// [$L_573a30]
extern "C" BYTE* __fastcall Dtor573a30_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_573a50, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_573c10(); KING_ALIAS("_FUN_573c10", "@CopterObj573c10_Destroy@8")
// [$L_573bf0]
extern "C" BYTE* __fastcall Dtor573bf0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_573c10, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

ASM_PROC(FUN_573d10)
// [$L_573cf0]
extern "C" BYTE* __fastcall Dtor573cf0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_573d10, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Destructor thunks: the vtable, then the base destructor; static object cleanups.

ASM_VAR(BYTE*, g_q46_6f3438, $L_6f3438)
ASM_VAR(AsmLabel, g_q46_56aa00, $L_56aa00)
// [FUN_569750] runs 56aa00 on the helicopter object 6f3438
extern "C" int __cdecl Heli_Run569750(void)
{
    return CallThis<int>((void (*)())(void*)g_q46_56aa00, g_q46_6f3438);
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
// returns the storage of a vector (+0 .. +8) of size-byte elements to the pool
static void PoolVec_Free_H(BYTE* s, int size)
{
    BYTE* p = F(BYTE*, s, 0);
    int n = (F(BYTE*, s, 8) - p) / size;
    if (n)
        CallC(FUN_44b4a0, p, n * size);
}
// [$L_576bd0] frees the storage of a vector of 192-byte records
extern "C" void __fastcall Vec192_FreeStorage(BYTE* s, void*)
{
    PoolVec_Free_H(s, 192);
}

// ---------------------------------------------------------------------------
// The per-side state bits of a helicopter request.

extern "C" void FUN_569000(); KING_ALIAS("_FUN_569000", "_Copter_Send963")
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")
// [$L_569090] sets the two bits of side i in the +416 mask of the request s by the state (0: bit 1, 1: bit 2),
// sends the mask before and after, stores it in the +344 resource; 1 when the side had bits set
extern "C" int __cdecl Copter_SetSideState(int i, int state, BYTE* s)
{
    int sh = i * 2;
    int had = (3 << sh) & F(int, s, 416);
    CallC(FUN_569000, F(int, s, 344), F(int, s, 416));
    if (state == 0)
        F(int, s, 416) |= 1 << sh;
    else if (state == 1)
        F(int, s, 416) |= 2 << sh;
    CallC(FUN_569000, F(int, s, 344), F(int, s, 416));
    BYTE* r = CallThis<BYTE*>(FUN_5e5370, g_696ca0 + 704, F(int, s, 344));
    if (r && F(BYTE*, r, 24))
        F(int, F(BYTE*, r, 24), 4) = F(int, s, 416);
    return had != 0;
}
